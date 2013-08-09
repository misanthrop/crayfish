#define UNICODE
#include <windows.h>
#include <time.h>
#include <memory.h>
#define LUA_COMPAT_MODULE
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


unsigned cf_width() { return DisplayWidth(dpy, screen); }
unsigned cf_height() { return DisplayHeight(dpy, screen); }

void cf_sleep(unsigned milli)
{
	struct timespec req = {milli/1000, (milli%1000)*1000}, rem;
	nanosleep(&req, &rem);
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
	UINT sc = MapVirtualKey(key, 0);
	keybd_event(key, sc, press ? 0 : KEYEVENTF_KEYUP, 0);
}

LUALIB_API int luaopen_luada(lua_State *lua)
{
	static const luaL_Reg functions[] =
	{
		{"sleep",		luada_sleep},
		{"movepointer",	luada_movepointer},
		{"button",		luada_button},
		{"click",		luada_click},
		{"key",			luada_key},
		{"press",		luada_press},
		{0, 0}
	};
	luaL_openlib(lua, "luada", functions, 0);
	luada_set_uint(lua, "width", GetSystemMetrics(SM_CXSCREEN));
	luada_set_uint(lua, "height", GetSystemMetrics(SM_CYSCREEN));

	lua_pushstring(lua, "keys");
	lua_createtable(lua, 0, 0);
		luada_set_uint(lua, "esc", VK_ESCAPE);
		luada_set_uint(lua, "tab", VK_TAB);
		luada_set_uint(lua, "lshift", VK_SHIFT);
		luada_set_uint(lua, "rshift", VK_SHIFT);
		luada_set_uint(lua, "lcontrol", VK_LCONTROL);
		luada_set_uint(lua, "rcontrol", VK_RCONTROL);
		luada_set_uint(lua, "lalt", VK_LMENU);
		luada_set_uint(lua, "ralt", VK_RMENU);
		luada_set_uint(lua, "lsuper", VK_LWIN);
		luada_set_uint(lua, "rsuper", VK_RWIN);
		luada_set_uint(lua, "menu", VK_MENU);
		luada_set_uint(lua, "numlock", VK_NUMLOCK);
		luada_set_uint(lua, "capslock", VK_CAPITAL);
		luada_set_uint(lua, "scrolllock", VK_SCROLL);
		luada_set_uint(lua, "pause", VK_PAUSE);
		luada_set_uint(lua, "del", VK_DELETE);
		luada_set_uint(lua, "backspace", VK_BACK);
		luada_set_uint(lua, "enter", VK_RETURN);
		luada_set_uint(lua, "home", VK_HOME);
		luada_set_uint(lua, "end", VK_END);
		luada_set_uint(lua, "pageup", VK_PRIOR);
		luada_set_uint(lua, "pagedown", VK_NEXT);
		luada_set_uint(lua, "insert", VK_INSERT);
		luada_set_uint(lua, "up", VK_UP);
		luada_set_uint(lua, "down", VK_DOWN);
		luada_set_uint(lua, "left", VK_LEFT);
		luada_set_uint(lua, "right", VK_RIGHT);
		luada_set_uint(lua, "f1", VK_F1);
		luada_set_uint(lua, "f2", VK_F2);
		luada_set_uint(lua, "f3", VK_F3);
		luada_set_uint(lua, "f4", VK_F4);
		luada_set_uint(lua, "f5", VK_F5);
		luada_set_uint(lua, "f6", VK_F6);
		luada_set_uint(lua, "f7", VK_F7);
		luada_set_uint(lua, "f8", VK_F8);
		luada_set_uint(lua, "f9", VK_F9);
		luada_set_uint(lua, "f10", VK_F10);
		luada_set_uint(lua, "f11", VK_F11);
		luada_set_uint(lua, "f12", VK_F12);

		luada_set_uint(lua, "a", 'A');
		luada_set_uint(lua, "b", 'B');
		luada_set_uint(lua, "c", 'C');
		luada_set_uint(lua, "d", 'D');
		luada_set_uint(lua, "e", 'E');
		luada_set_uint(lua, "f", 'F');
		luada_set_uint(lua, "g", 'G');
		luada_set_uint(lua, "h", 'H');
		luada_set_uint(lua, "i", 'I');
		luada_set_uint(lua, "j", 'J');
		luada_set_uint(lua, "k", 'K');
		luada_set_uint(lua, "l", 'L');
		luada_set_uint(lua, "m", 'M');
		luada_set_uint(lua, "n", 'N');
		luada_set_uint(lua, "o", 'O');
		luada_set_uint(lua, "p", 'P');
		luada_set_uint(lua, "q", 'Q');
		luada_set_uint(lua, "r", 'R');
		luada_set_uint(lua, "s", 'S');
		luada_set_uint(lua, "t", 'T');
		luada_set_uint(lua, "u", 'U');
		luada_set_uint(lua, "v", 'V');
		luada_set_uint(lua, "w", 'W');
		luada_set_uint(lua, "x", 'X');
		luada_set_uint(lua, "y", 'Y');
		luada_set_uint(lua, "z", 'Z');

		luada_set_uint(lua, "multiply", VK_MULTIPLY);
		luada_set_uint(lua, "divide", VK_DIVIDE);
		luada_set_uint(lua, "plus", VK_ADD);
		luada_set_uint(lua, "minus", VK_SUBTRACT);
		luada_set_uint(lua, "equals", '=');
		luada_set_uint(lua, "kpenter", VK_RETURN);
		luada_set_uint(lua, "kp0", VK_NUMPAD0);
		luada_set_uint(lua, "kp1", VK_NUMPAD1);
		luada_set_uint(lua, "kp2", VK_NUMPAD2);
		luada_set_uint(lua, "kp3", VK_NUMPAD3);
		luada_set_uint(lua, "kp4", VK_NUMPAD4);
		luada_set_uint(lua, "kp5", VK_NUMPAD5);
		luada_set_uint(lua, "kp6", VK_NUMPAD6);
		luada_set_uint(lua, "kp7", VK_NUMPAD7);
		luada_set_uint(lua, "kp8", VK_NUMPAD8);
		luada_set_uint(lua, "kp9", VK_NUMPAD9);
		luada_set_uint(lua, "kpdel", VK_SEPARATOR);
	lua_settable(lua, -3);

	return 1;
}
