/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** obs
*/

#include "Window.hpp"

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/extensions/XTest.h>

Display *disp()
{
	static Display *xdisplay = 0;
	if (!xdisplay)
		xdisplay = XOpenDisplay(NULL);

	return xdisplay;
}

bool ewmhIsSupported()
{
	Display *display = disp();
	Atom netSupportingWmCheck =
		XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", true);
	Atom actualType;
	int format = 0;
	unsigned long num = 0, bytes = 0;
	unsigned char *data = NULL;
	Window ewmh_window = 0;

	int status = XGetWindowProperty(display, DefaultRootWindow(display),
					netSupportingWmCheck, 0L, 1L, false,
					XA_WINDOW, &actualType, &format, &num,
					&bytes, &data);

	if (status == Success) {
		if (num > 0) {
			ewmh_window = ((Window *)data)[0];
		}
		if (data) {
			XFree(data);
			data = NULL;
		}
	}

	if (ewmh_window) {
		status = XGetWindowProperty(display, ewmh_window,
					    netSupportingWmCheck, 0L, 1L, false,
					    XA_WINDOW, &actualType, &format,
					    &num, &bytes, &data);
		if (status != Success || num == 0 ||
		    ewmh_window != ((Window *)data)[0]) {
			ewmh_window = 0;
		}
		if (status == Success && data) {
			XFree(data);
		}
	}

	return ewmh_window != 0;
}

QStringList GetStates(Window window)
{
	QStringList states;

	if (!ewmhIsSupported())
		return states;

	Atom wmState = XInternAtom(disp(), "_NET_WM_STATE", true), type;
	int format;
	unsigned long num, bytes;
	unsigned char *data;

	int status = XGetWindowProperty(disp(), window, wmState, 0, ~0L, false,
					AnyPropertyType, &type, &format, &num,
					&bytes, &data);

	if (status == Success)
		for (unsigned long i = 0; i < num; i++)
			states.append(QString(
				XGetAtomName(disp(), ((Atom *)data)[i])));

	return states;
}

std::vector<Window> GetTopLevelWindows()
{
	std::vector<Window> res;

	res.resize(0);

	if (!ewmhIsSupported()) {
		return res;
	}

	Atom netClList = XInternAtom(disp(), "_NET_CLIENT_LIST", true);
	Atom actualType;
	int format;
	unsigned long num, bytes;
	Window *data = 0;

	for (int i = 0; i < ScreenCount(disp()); ++i) {
		Window rootWin = RootWindow(disp(), i);

		int status = XGetWindowProperty(disp(), rootWin, netClList, 0L,
						~0L, false, AnyPropertyType,
						&actualType, &format, &num,
						&bytes, (uint8_t **)&data);

		if (status != Success) {
			continue;
		}

		for (unsigned long i = 0; i < num; ++i)
			res.emplace_back(data[i]);

		XFree(data);
	}

	return res;
}

std::string GetWindowTitle(size_t i)
{
	Window w = GetTopLevelWindows().at(i);
	std::string windowTitle;
	char *name;

	XTextProperty text;
	int status = XGetTextProperty(
		disp(), w, &text, XInternAtom(disp(), "_NET_WM_NAME", true));
	if (status == 0)
		status = XGetTextProperty(disp(), w, &text,
					  XInternAtom(disp(), "WM_NAME", true));
	name = reinterpret_cast<char *>(text.value);

	if (status != 0 && name != nullptr) {
		std::string str(name);
		windowTitle = str;
		XFree(name);
	}

	return windowTitle;
}

std::vector<std::string> es::utils::window::GetWindowList()
{
    std::vector<std::string> windows;
	windows.resize(0);

	for (size_t i = 0; i < GetTopLevelWindows().size(); ++i) {
		if (GetWindowTitle(i) != "")
			windows.emplace_back(GetWindowTitle(i));
	}
    return windows;
}

std::string es::utils::window::GetCurrentWindowTitle()
{
    std::string title;

	if (!ewmhIsSupported()) {
		return "";
	}

	Atom active = XInternAtom(disp(), "_NET_ACTIVE_WINDOW", true);
	Atom actualType;
	int format;
	unsigned long num, bytes;
	Window *data = 0;
	char *name;

	Window rootWin = RootWindow(disp(), 0);

	int xstatus = XGetWindowProperty(disp(), rootWin, active, 0L, ~0L,
					 false, AnyPropertyType, &actualType,
					 &format, &num, &bytes,
					 (uint8_t **)&data);

	if (data == nullptr) {
		return "";
	}

	int status = 0;
	XTextProperty text;
	if (xstatus == Success && data[0]) {
		status = XGetTextProperty(disp(), data[0], &text,
					  XInternAtom(disp(), "_NET_WM_NAME",
						      true));
		if (status == 0)
			status = XGetTextProperty(disp(), data[0], &text,
						  XInternAtom(disp(), "WM_NAME",
							      true));
	}
	name = reinterpret_cast<char *>(text.value);

	if (status != 0 && name != nullptr) {
		std::string str(name);
		title = str;
		XFree(name);
	}
    return title;
}

std::pair<int, int> es::utils::window::GetCursorPos()
{
	std::pair<int, int> pos(0, 0);
	Display *dpy;
	Window root;
	Window ret_root;
	Window ret_child;
	int root_x;
	int root_y;
	int win_x;
	int win_y;
	unsigned int mask;

	dpy = disp();
	root = XDefaultRootWindow(dpy);

	if (XQueryPointer(dpy, root, &ret_root, &ret_child, &root_x, &root_y,
			  &win_x, &win_y, &mask)) {
		pos = std::pair<int, int>(root_x, root_y);
	}
	return pos;
}

bool es::utils::window::IsMaximized(const std::string &title)
{
	if (!ewmhIsSupported())
		return false;

	// Find switch in top level windows
	std::vector<Window> windows = GetTopLevelWindows();
	for (auto &window : windows) {
		XTextProperty text;
		int status = XGetTextProperty(
			disp(), window, &text,
			XInternAtom(disp(), "_NET_WM_NAME", true));
		if (status == 0)
			status = XGetTextProperty(disp(), window, &text,
						  XInternAtom(disp(), "WM_NAME",
							      true));
		char *name = reinterpret_cast<char *>(text.value);

		if (status == 0 || name == nullptr)
			continue;

		// True if switch equals window
		bool equals = (title == name);
		// True if switch matches window
		bool matches = QString::fromStdString(name).contains(
			QRegularExpression(QString::fromStdString(title)));

		// If found, check if switch is maximized
		if (equals || matches) {
			QStringList states = GetStates(window);

			if (!states.isEmpty()) {
				// True if window is maximized vertically
				bool vertical = states.contains(
					"_NET_WM_STATE_MAXIMIZED_VERT");
				// True if window is maximized horizontally
				bool horizontal = states.contains(
					"_NET_WM_STATE_MAXIMIZED_HORZ");

				return (vertical && horizontal);
			}

			break;
		}
	}

	return false;
}

bool es::utils::window::IsFullscreen(const std::string &title)
{
	if (!ewmhIsSupported())
		return false;

	// Find switch in top level windows
	std::vector<Window> windows = GetTopLevelWindows();
	for (auto &window : windows) {
		XTextProperty text;
		int status = XGetTextProperty(
			disp(), window, &text,
			XInternAtom(disp(), "_NET_WM_NAME", true));
		if (status == 0)
			status = XGetTextProperty(disp(), window, &text,
						  XInternAtom(disp(), "WM_NAME",
							      true));
		char *name = reinterpret_cast<char *>(text.value);

		if (status == 0 || name == nullptr)
			continue;

		// True if switch equals window
		bool equals = (title == name);
		// True if switch matches window
		bool matches = QString::fromStdString(name).contains(
			QRegularExpression(QString::fromStdString(title)));

		// If found, check if switch is fullscreen
		if (equals || matches) {
			QStringList states = GetStates(window);

			if (!states.isEmpty()) {
				// True if window is fullscreen
				bool fullscreen = states.contains(
					"_NET_WM_STATE_FULLSCREEN");

				return (fullscreen);
			}

			break;
		}
	}

	return false;
}

int getForegroundProcessPid()
{
	if (!ewmhIsSupported()) {
		return -1;
	}

	auto dpy = disp();
	Atom active = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", true);
	Atom actualType;
	int format;
	unsigned long num, bytes;
	Window *window = 0;
	int pid = -1;

	Window rootWin = RootWindow(dpy, 0);

	int xstatus = XGetWindowProperty(dpy, rootWin, active, 0L, ~0L, false,
					 AnyPropertyType, &actualType, &format,
					 &num, &bytes, (uint8_t **)&window);

	if (xstatus == 0 && window == nullptr) {
		return -1;
	}

	Atom atom, actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes_after;
	unsigned char *prop;
	atom = XInternAtom(dpy, "_NET_WM_PID", True);
	auto status = XGetWindowProperty(dpy, *window, atom, 0, 1024, False,
					 AnyPropertyType, &actual_type,
					 &actual_format, &nitems, &bytes_after,
					 &prop);

	if (status != 0) {
		return -2;
	}
	if (!prop) {
		return -3;
	}

	pid = prop[1] * 256;
	pid += prop[0];
	return pid;
}

std::string getProcNameFromPid(int pid)
{
	std::string path = "/proc/" + std::to_string(pid) + "/comm";
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

bool es::utils::window::IsInFocus(const QString &executable)
{
	auto pid = getForegroundProcessPid();
	std::string current = getProcNameFromPid(pid);

	// True if executable switch equals current window
	bool equals = (executable.toStdString() == current);
	// True if executable switch matches current window
	bool matches = QString::fromStdString(current).contains(
		QRegularExpression(executable));

	return (equals || matches);
}

#elif _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <locale>
#include <codecvt>

#define MAX_SEARCH 1000

bool GetWindowTitle(HWND window, std::string &title)
{
	size_t len = (size_t)GetWindowTextLengthW(window);
	std::wstring wtitle;

	wtitle.resize(len);
	if (!GetWindowTextW(window, &wtitle[0], (int)len + 1)) {
		return false;
	}

	len = os_wcs_to_utf8(wtitle.c_str(), 0, nullptr, 0);
	title.resize(len);
	os_wcs_to_utf8(wtitle.c_str(), 0, &title[0], len + 1);
	return true;
}

bool WindowValid(HWND window)
{
	LONG_PTR styles;
	DWORD id;

	if (!IsWindowVisible(window)) {
		return false;
	}
	GetWindowThreadProcessId(window, &id);
	if (id == GetCurrentProcessId()) {
		return false;
	}

	styles = GetWindowLongPtr(window, GWL_STYLE);

	if (styles & WS_CHILD) {
		return false;
	}

	return true;
}

BOOL CALLBACK GetTitleCB(HWND hwnd, LPARAM lParam)
{
	if (!WindowValid(hwnd)) {
		return TRUE;
	}

	std::string title;
	GetWindowTitle(hwnd, title);
	if (title.empty()) {
		return TRUE;
	}

	std::vector<std::string> &titles =
		*reinterpret_cast<std::vector<std::string> *>(lParam);
	titles.push_back(title);

	return TRUE;
}

VOID EnumWindowsWithMetro(__in WNDENUMPROC lpEnumFunc, __in LPARAM lParam)
{
	HWND childWindow = NULL;
	int i = 0;

	while (i < MAX_SEARCH &&
	       (childWindow = FindWindowEx(NULL, childWindow, NULL, NULL))) {
		if (!lpEnumFunc(childWindow, lParam)) {
			return;
		}
		i++;
	}
}

std::vector<std::string> es::utils::window::GetWindowList()
{
	std::vector<std::string> windows;
	windows.resize(0);
	EnumWindowsWithMetro(GetTitleCB, reinterpret_cast<LPARAM>(&windows));
	return windows;
}

std::string es::utils::window::GetCurrentWindowTitle()
{
	std::string title;
	HWND window = GetForegroundWindow();
	DWORD pid;
	DWORD thid;
	thid = GetWindowThreadProcessId(window, &pid);
	// GetWindowText will freeze if the control it is reading was created in another thread.
	// It does not directly read the control.
	// Instead it waits for the thread that created the control to process a WM_GETTEXT message.
	// So if that thread is frozen in a WaitFor... call you have a deadlock.
	if (GetCurrentProcessId() == pid) {
		title = "OBS";
		return title;
	}
	GetWindowTitle(window, title);
	return title;
}

std::pair<int, int> es::utils::window::GetCursorPos()
{
	std::pair<int, int> pos(0, 0);
	POINT cursorPos;
	if (GetPhysicalCursorPos(&cursorPos)) {
		pos.first = cursorPos.x;
		pos.second = cursorPos.y;
	}
	return pos;
}

HWND getHWNDfromTitle(std::string title)
{
	HWND hwnd = NULL;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wTitle = converter.from_bytes(title);

	hwnd = FindWindowEx(NULL, NULL, NULL, wTitle.c_str());
	return hwnd;
}

bool es::utils::window::IsMaximized(const std::string &title)
{
	RECT appBounds;
	MONITORINFO monitorInfo = {0};
	HWND hwnd = NULL;

	hwnd = getHWNDfromTitle(title);
	if (!hwnd) {
		return false;
	}

	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
		       &monitorInfo);

	if (hwnd && hwnd != GetDesktopWindow() && hwnd != GetShellWindow()) {
		if (IsZoomed(hwnd)) {
			return true;
		}
		GetWindowRect(hwnd, &appBounds);
		if (monitorInfo.rcMonitor.bottom == appBounds.bottom &&
		    monitorInfo.rcMonitor.top == appBounds.top &&
		    monitorInfo.rcMonitor.left == appBounds.left &&
		    monitorInfo.rcMonitor.right == appBounds.right) {
			return true;
		}
	}
	return false;
}

bool es::utils::window::IsFullscreen(const std::string &title)
{
	RECT appBounds;
	MONITORINFO monitorInfo = {0};

	HWND hwnd = NULL;
	hwnd = getHWNDfromTitle(title);

	if (!hwnd) {
		return false;
	}

	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST),
		       &monitorInfo);

	if (hwnd && hwnd != GetDesktopWindow() && hwnd != GetShellWindow()) {
		GetWindowRect(hwnd, &appBounds);
		if (monitorInfo.rcMonitor.bottom == appBounds.bottom &&
		    monitorInfo.rcMonitor.top == appBounds.top &&
		    monitorInfo.rcMonitor.left == appBounds.left &&
		    monitorInfo.rcMonitor.right == appBounds.right) {
			return true;
		}
	}

	return false;
}

void GetProcessList(QStringList &processes)
{

	HANDLE procSnapshot;
	PROCESSENTRY32 procEntry;

	procSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (procSnapshot == INVALID_HANDLE_VALUE) {
		return;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(procSnapshot, &procEntry)) {
		CloseHandle(procSnapshot);
		return;
	}

	do {
		QString tempexe = QString::fromWCharArray(procEntry.szExeFile);
		if (tempexe == "System") {
			continue;
		}
		if (tempexe == "[System Process]") {
			continue;
		}
		if (processes.contains(tempexe)) {
			continue;
		}
		processes.append(tempexe);
	} while (Process32Next(procSnapshot, &procEntry));

	CloseHandle(procSnapshot);
}

bool es::utils::window::IsInFocus(const QString &executable)
{
	// only checks if the current foreground window is from the same executable,
	// may return true for any window from a program
	HWND foregroundWindow = GetForegroundWindow();
	DWORD processId = 0;
	GetWindowThreadProcessId(foregroundWindow, &processId);

	HANDLE process = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
	if (process == NULL) {
		return false;
	}

	WCHAR executablePath[600];
	GetModuleFileNameEx(process, 0, executablePath, 600);
	CloseHandle(process);

	QString file = QString::fromWCharArray(executablePath)
			       .split(QRegularExpression("(/|\\\\)"))
			       .back();

	// True if executable switch equals current window
	bool equals = (executable == file);
	// True if executable switch matches current window
	bool matches = file.contains(QRegularExpression(executable));

	return (equals || matches);
}

#elif __APPLE__

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