/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** obs
*/

#include "Window.hpp"

#ifdef __APPLE__

#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CGEvent.h>
#import <Carbon/Carbon.h>
#include <Carbon/Carbon.h>

std::vector<std::string> es::utils::window::GetWindowList()
{
	std::vector<std::string> windows;
	windows.resize(0);

	@autoreleasepool {
		NSMutableArray *apps =
			(__bridge NSMutableArray *)CGWindowListCopyWindowInfo(
				kCGWindowListOptionAll, kCGNullWindowID);
		for (NSDictionary *app in apps) {
			std::string name([[app objectForKey:@"kCGWindowName"]
						 UTF8String],
					 [[app objectForKey:@"kCGWindowName"]
						 lengthOfBytesUsingEncoding:
							 NSUTF8StringEncoding]);
			std::string owner(
				[[app objectForKey:@"kCGWindowOwnerName"]
					UTF8String],
				[[app objectForKey:@"kCGWindowOwnerName"]
					lengthOfBytesUsingEncoding:
						NSUTF8StringEncoding]);

			if (!name.empty() &&
			    find(windows.begin(), windows.end(), name) ==
				    windows.end())
				windows.emplace_back(name);

			if (!owner.empty() &&
			    find(windows.begin(), windows.end(), owner) ==
				    windows.end())
				windows.emplace_back(owner);
		}
	}
	return windows;
}

std::string es::utils::window::GetCurrentWindowTitle()
{
	std::string title = "";
	title.resize(0);

	@autoreleasepool {
		NSMutableArray *apps =
			(__bridge NSMutableArray *)CGWindowListCopyWindowInfo(
				kCGWindowListOptionOnScreenOnly,
				kCGNullWindowID);
		for (NSDictionary *app in apps) {
			int layer =
				[[app objectForKey:@"kCGWindowLayer"] intValue];
			// True if window is frontmost
			if (layer == 0) {
				std::string name(
					[[app objectForKey:@"kCGWindowName"]
						UTF8String],
					[[app objectForKey:@"kCGWindowName"]
						lengthOfBytesUsingEncoding:
							NSUTF8StringEncoding]);
				std::string owner(
					[[app objectForKey:@"kCGWindowOwnerName"]
						UTF8String],
					[[app objectForKey:@"kCGWindowOwnerName"]
						lengthOfBytesUsingEncoding:
							NSUTF8StringEncoding]);

				if (!name.empty())
					title = name;
				else if (!owner.empty())
					title = owner;

				break;
			}
		}
	}
	return title;
}

std::pair<int, int> es::utils::window::GetCursorPos()
{
	std::pair<int, int> pos(0, 0);
	CGEventRef event = CGEventCreate(NULL);
	CGPoint cursorPos = CGEventGetLocation(event);
	CFRelease(event);
	pos.first = cursorPos.x;
	pos.second = cursorPos.y;
	return pos;
}

bool isWindowOriginOnScreen(NSDictionary *app, NSScreen *screen,
			    bool fullscreen = false)
{
	NSArray *screens = [NSScreen screens];
	NSRect mainScreenFrame = [screens[0] frame];
	NSRect screenFrame;
	if (fullscreen) {
		screenFrame = [screen frame];
	} else {
		screenFrame = [screen visibleFrame];
	}
	NSRect windowBounds;
	CGRectMakeWithDictionaryRepresentation(
		(CFDictionaryRef)[app objectForKey:@"kCGWindowBounds"],
		&windowBounds);

	return (windowBounds.origin.x == screenFrame.origin.x &&
		(mainScreenFrame.size.height - screenFrame.size.height -
			 windowBounds.origin.y ==
		 screenFrame.origin.y));
}

bool isWindowMaximizedOnScreen(NSDictionary *app, NSScreen *screen)
{
	double maximizedTolerance = 0.99;
	NSRect screenFrame = [screen frame];
	NSRect windowBounds;
	CGRectMakeWithDictionaryRepresentation(
		(CFDictionaryRef)[app objectForKey:@"kCGWindowBounds"],
		&windowBounds);

	int sumX = windowBounds.origin.x + windowBounds.size.width;
	int sumY = windowBounds.origin.y + windowBounds.size.height;

	// Return false if window spans over multiple screens
	if (sumX > screenFrame.size.width) {
		return false;
	}
	if (sumY > screenFrame.size.height) {
		return false;
	}

	return ((double)sumX / (double)screenFrame.size.width) >
		       maximizedTolerance &&
	       ((double)sumY / (double)screenFrame.size.height) >
		       maximizedTolerance;
}

bool nameMachesPattern(std::string windowName, std::string pattern)
{
	return QString::fromStdString(windowName)
		.contains(QRegularExpression(QString::fromStdString(pattern)));
}

bool es::utils::window::IsMaximized(const std::string &title)
{
	@autoreleasepool {
		NSArray *screens = [NSScreen screens];
		NSMutableArray *apps =
			(__bridge NSMutableArray *)CGWindowListCopyWindowInfo(
				kCGWindowListOptionAll, kCGNullWindowID);
		for (NSDictionary *app in apps) {
			std::string name([[app objectForKey:@"kCGWindowName"]
						 UTF8String],
					 [[app objectForKey:@"kCGWindowName"]
						 lengthOfBytesUsingEncoding:
							 NSUTF8StringEncoding]);
			std::string owner(
				[[app objectForKey:@"kCGWindowOwnerName"]
					UTF8String],
				[[app objectForKey:@"kCGWindowOwnerName"]
					lengthOfBytesUsingEncoding:
						NSUTF8StringEncoding]);

			bool equals = (title == name || title == owner);
			bool matches = nameMachesPattern(name, title) ||
				       nameMachesPattern(owner, title);
			if (equals || matches) {
				for (NSScreen *screen in screens) {
					if (isWindowOriginOnScreen(app,
								   screen) &&
					    isWindowMaximizedOnScreen(app,
								      screen)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool isWindowFullscreenOnScreen(NSDictionary *app, NSScreen *screen)
{
	NSRect screenFrame = [screen frame];
	NSRect windowBounds;
	CGRectMakeWithDictionaryRepresentation(
		(CFDictionaryRef)[app objectForKey:@"kCGWindowBounds"],
		&windowBounds);

	return NSEqualSizes(windowBounds.size, screenFrame.size);
}

bool es::utils::window::IsFullscreen(const std::string &title)
{
	@autoreleasepool {
		NSArray *screens = [NSScreen screens];
		NSMutableArray *apps =
			(__bridge NSMutableArray *)CGWindowListCopyWindowInfo(
				kCGWindowListOptionAll, kCGNullWindowID);
		for (NSDictionary *app in apps) {
			std::string name([[app objectForKey:@"kCGWindowName"]
						 UTF8String],
					 [[app objectForKey:@"kCGWindowName"]
						 lengthOfBytesUsingEncoding:
							 NSUTF8StringEncoding]);
			std::string owner(
				[[app objectForKey:@"kCGWindowOwnerName"]
					UTF8String],
				[[app objectForKey:@"kCGWindowOwnerName"]
					lengthOfBytesUsingEncoding:
						NSUTF8StringEncoding]);

			bool equals = (title == name || title == owner);
			bool matches = nameMachesPattern(name, title) ||
				       nameMachesPattern(owner, title);
			if (equals || matches) {
				for (NSScreen *screen in screens) {
					if (isWindowOriginOnScreen(app, screen,
								   true) &&
					    isWindowFullscreenOnScreen(app,
								       screen))
						return true;
				}
			}
		}
	}
	return false;
}

void GetProcessList(QStringList &list)
{
	list.clear();
	@autoreleasepool {
		NSWorkspace *ws = [NSWorkspace sharedWorkspace];
		NSArray *array = [ws runningApplications];
		for (NSRunningApplication *app in array) {
			NSString *name = app.localizedName;
			if (!name)
				continue;

			const char *str = name.UTF8String;
			if (str && *str)
				list << (str);
		}
	}
}

bool es::utils::window::IsInFocus(const QString &executable)
{
	std::string current = es::utils::window::GetCurrentWindowTitle();

	// True if executable switch equals current window
	bool equals = (executable.toStdString() == current);
	// True if executable switch matches current window
	bool matches = QString::fromStdString(current).contains(
		QRegularExpression(executable));

	return (equals || matches);
}

#endif