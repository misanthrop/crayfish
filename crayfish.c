#include <stdlib.h>
#define LUA_COMPAT_MODULE
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "crayfish.h"

static unsigned min_down_sleep = 10, delta_down_sleep = 10;
static unsigned min_up_sleep = 50, delta_up_sleep = 50;

void cf_wait_down() { cf_sleep(min_down_sleep + rand()%(delta_down_sleep+1)); }
void cf_wait_up() { cf_sleep(min_up_sleep + rand()%(delta_up_sleep+1)); }

static int luacf_sleep(lua_State *lua) { cf_sleep(lua_tounsigned(lua,1)); return 0; }
static int luacf_movepointer(lua_State *lua) { cf_movepointer(lua_tointeger(lua,1), lua_tointeger(lua,2)); return 0; }
static int luacf_button(lua_State *lua) { cf_button(lua_tounsigned(lua,1), lua_toboolean(lua,2)); return 0; }
static int luacf_key(lua_State *lua) { cf_key(lua_tounsigned(lua,1), lua_toboolean(lua,2)); return 0; }

static int luacf_click(lua_State *lua)
{
	unsigned button = lua_tounsigned(lua,1);
	cf_button(button, 1);
	cf_wait_down();
	cf_button(button, 0);
	cf_wait_up();
	return 0;
}

static int luacf_press(lua_State *lua)
{
	unsigned key = lua_tounsigned(lua,1);
	cf_key(button, 1);
	cf_wait_down();
	cf_key(button, 0);
	cf_wait_up();
	return 0;
}

static void set_uint(lua_State *lua, const char *key, unsigned val)
{
	lua_pushstring(lua, key);
	lua_pushinteger(lua, val);
	lua_settable(lua, -3);
}

LUALIB_API int luaopen_luada(lua_State *lua)
{
	static const luaL_Reg functions[] =
	{
		{"sleep",		luacf_sleep},
		{"movepointer",	luacf_movepointer},
		{"button",		luacf_button},
		{"key",			luacf_key},
		{"click",		luacf_click},
		{"press",		luacf_press},
		{0, 0}
	};

	luaL_openlib(lua, "luada", functions, 0);
	set_uint(lua, "width", DisplayWidth(dpy, screen));
	set_uint(lua, "height", DisplayHeight(dpy, screen));

	lua_pushstring(lua, "keys");
	lua_createtable(lua, 0, 0);
		set_uint(lua, "esc", XK_Escape);
		set_uint(lua, "tab", XK_Tab);
		set_uint(lua, "lshift", XK_Shift_L);
		set_uint(lua, "rshift", XK_Shift_R);
		set_uint(lua, "lcontrol", XK_Control_L);
		set_uint(lua, "rcontrol", XK_Control_R);
		set_uint(lua, "lalt", XK_Alt_L);
		set_uint(lua, "ralt", XK_Alt_R);
		set_uint(lua, "lsuper", XK_Super_L);
		set_uint(lua, "rsuper", XK_Super_R);
		set_uint(lua, "menu", XK_Menu);
		set_uint(lua, "numlock", XK_Num_Lock);
		set_uint(lua, "capslock", XK_Caps_Lock);
		set_uint(lua, "scrolllock", XK_Scroll_Lock);
		set_uint(lua, "pause", XK_Pause);
		set_uint(lua, "del", XK_Delete);
		set_uint(lua, "backspace", XK_BackSpace);
		set_uint(lua, "enter", XK_Return);
		set_uint(lua, "home", XK_Home);
		set_uint(lua, "end", XK_End);
		set_uint(lua, "pageup", XK_Page_Up);
		set_uint(lua, "pagedown", XK_Page_Down);
		set_uint(lua, "insert", XK_Insert);
		set_uint(lua, "up", XK_Up);
		set_uint(lua, "down", XK_Down);
		set_uint(lua, "left", XK_Left);
		set_uint(lua, "right", XK_Right);
		set_uint(lua, "f1", XK_F1);
		set_uint(lua, "f2", XK_F2);
		set_uint(lua, "f3", XK_F3);
		set_uint(lua, "f4", XK_F4);
		set_uint(lua, "f5", XK_F5);
		set_uint(lua, "f6", XK_F6);
		set_uint(lua, "f7", XK_F7);
		set_uint(lua, "f8", XK_F8);
		set_uint(lua, "f9", XK_F9);
		set_uint(lua, "f10", XK_F10);
		set_uint(lua, "f11", XK_F11);
		set_uint(lua, "f12", XK_F12);

		set_uint(lua, "a", XK_a);
		set_uint(lua, "b", XK_b);
		set_uint(lua, "c", XK_c);
		set_uint(lua, "d", XK_d);
		set_uint(lua, "e", XK_e);
		set_uint(lua, "f", XK_f);
		set_uint(lua, "g", XK_g);
		set_uint(lua, "h", XK_h);
		set_uint(lua, "i", XK_i);
		set_uint(lua, "j", XK_j);
		set_uint(lua, "k", XK_k);
		set_uint(lua, "l", XK_l);
		set_uint(lua, "m", XK_m);
		set_uint(lua, "n", XK_n);
		set_uint(lua, "o", XK_o);
		set_uint(lua, "p", XK_p);
		set_uint(lua, "q", XK_q);
		set_uint(lua, "r", XK_r);
		set_uint(lua, "s", XK_s);
		set_uint(lua, "t", XK_t);
		set_uint(lua, "u", XK_u);
		set_uint(lua, "v", XK_v);
		set_uint(lua, "w", XK_w);
		set_uint(lua, "x", XK_x);
		set_uint(lua, "y", XK_y);
		set_uint(lua, "z", XK_z);

		set_uint(lua, "multiply", XK_KP_Multiply);
		set_uint(lua, "divide", XK_KP_Divide);
		set_uint(lua, "plus", XK_KP_Add);
		set_uint(lua, "minus", XK_KP_Subtract);
		set_uint(lua, "equals", XK_KP_Equal);
		set_uint(lua, "kpenter", XK_KP_Enter);
		set_uint(lua, "kp0", XK_KP_0);
		set_uint(lua, "kp1", XK_KP_1);
		set_uint(lua, "kp2", XK_KP_2);
		set_uint(lua, "kp3", XK_KP_3);
		set_uint(lua, "kp4", XK_KP_4);
		set_uint(lua, "kp5", XK_KP_5);
		set_uint(lua, "kp6", XK_KP_6);
		set_uint(lua, "kp7", XK_KP_7);
		set_uint(lua, "kp8", XK_KP_8);
		set_uint(lua, "kp9", XK_KP_9);
		set_uint(lua, "kpdel", XK_KP_Decimal);
	lua_settable(lua, -3);

	return 1;
}
