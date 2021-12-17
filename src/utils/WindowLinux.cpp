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

#endif