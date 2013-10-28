#define UNICODE
#include <windows.h>
#include <time.h>
#include <memory.h>
#define LUA_COMPAT_MODULE
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "crayfish.h"

unsigned cf_width() { return GetSystemMetrics(SM_CXSCREEN); }
unsigned cf_height() { return GetSystemMetrics(SM_CYSCREEN); }

void cf_sleep(unsigned milli)
{
	Sleep(milli);
}

void cf_movepointer(int x, int y) { SetCursorPos(x,y); }

void cf_button(unsigned button, int press)
{
	static const DWORD buttonevents[3][2] = {
		{MOUSEEVENTF_LEFTUP, MOUSEEVENTF_LEFTDOWN},
		{MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_RIGHTDOWN},
		{MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_MIDDLEDOWN}
	};
	mouse_event(buttonevents[button][press], 0,0,0,0);
}

void cf_key(unsigned key, int press)
{
	static const unsigned keys[cf_last] =
	{
		0, VK_BACK, VK_TAB, VK_RETURN, VK_ESCAPE, VK_SPACE,
		'\'', ',', '.', '/',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ';', '=',
		'(', '\\', ')',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
		'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		VK_DELETE, VK_PAUSE, VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LWIN, VK_RWIN, VK_MENU,
		VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_HOME, VK_END, VK_PRIOR, VK_NEXT, VK_INSERT, VK_DELETE,
		VK_CAPITAL, VK_SCROLL, VK_NUMLOCK, VK_SELECT, VK_PRINT,
		VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
		VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,
		VK_MULTIPLY, VK_DIVIDE, VK_ADD, VK_SUBTRACT, VK_RETURN, '\\'
	};
	UINT sc = MapVirtualKey(keys[key], 0);
	keybd_event(keys[key], sc, press ? 0 : KEYEVENTF_KEYUP, 0);
}
