#include <windows.h>
#include <time.h>
#include <memory.h>
#define LUA_COMPAT_MODULE
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static int luada_sleep(lua_State *lua)
{
	Sleep(lua_tounsigned(lua,1));
	return 0;
}

static int luada_movepointer(lua_State *lua)
{
	int x = lua_tointeger(lua,1);
	int y = lua_tointeger(lua,2);
	//SetCursorPos(x,y);
	mouse_event(MOUSEEVENTF_MOVE, x,y, 0, 0);
	return 0;
}

static int luada_button(lua_State *lua)
{
	static const DWORD events[3][2] = {
		{MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP},
		{MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP},
		{MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP}
	};
	unsigned button = lua_tounsigned(lua,1);
	int press = lua_toboolean(lua,2);
	mouse_event(events[button][press], 0,0,0,0);
	return 0;
}

static int luada_key(lua_State *lua)
{
	unsigned key = lua_tounsigned(lua,1);
	int press = lua_toboolean(lua,2);
	UINT sc  = MapVirtualKey(key, 0);
	keybd_event(key, sc, press ? 0 : KEYEVENTF_KEYUP, 0);
	return 0;
}

LUALIB_API int luaopen_libluada(lua_State *lua)
{
	static const luaL_Reg functions[] =
	{
		{"sleep",		luada_sleep},
		{"movepointer",	luada_movepointer},
		{"button",		luada_button},
		{"key",			luada_key},
		{0, 0}
	};
	luaL_openlib(lua, "luada", functions, 0);
	lua_pushstring(lua, "width");	lua_pushinteger(lua, GetSystemMetrics(SM_CXSCREEN));	lua_settable(lua, -3);
	lua_pushstring(lua, "height");	lua_pushinteger(lua, GetSystemMetrics(SM_CYSCREEN));	lua_settable(lua, -3);
	return 1;
}
