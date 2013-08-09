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

static int luacf_width(lua_State *lua) { lua_pushunsigned(lua, cf_width()); return 1; }
static int luacf_height(lua_State *lua) { lua_pushunsigned(lua, cf_height()); return 1; }
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
	cf_key(key, 1);
	cf_wait_down();
	cf_key(key, 0);
	cf_wait_up();
	return 0;
}

static void set_uint(lua_State *lua, const char *key, unsigned val)
{
	lua_pushstring(lua, key);
	lua_pushinteger(lua, val);
	lua_settable(lua, -3);
}

LUALIB_API int luaopen_crayfish(lua_State *lua)
{
	static const luaL_Reg functions[] =
	{
		{"width",		luacf_width},
		{"height",		luacf_height},
		{"sleep",		luacf_sleep},
		{"movepointer",	luacf_movepointer},
		{"button",		luacf_button},
		{"key",			luacf_key},
		{"click",		luacf_click},
		{"press",		luacf_press},
		{0, 0}
	};

	luaL_openlib(lua, "luada", functions, 0);

#define key(name) set_uint(lua, #name, cf_##name)
	key(lbutton); key(rbutton); key(mbutton);
	key(esc);
	key(tab);
	key(lshift); key(rshift); key(lcontrol); key(rcontrol); key(lalt); key(ralt); key(lsuper); key(rsuper); key(menu);
	key(numlock); key(capslock); key(scrolllock);
	key(pause);
	key(del);
	key(backspace);
	key(enter);
	key(home); key(end); key(pageup); key(pagedown);
	key(insert); key(up); key(down); key(left); key(right);
	key(f1); key(f2); key(f3); key(f4); key(f5); key(f6); key(f7); key(f8); key(f9); key(f10); key(f11); key(f12);
	key(a); key(b); key(c); key(d); key(e); key(f); key(g); key(h); key(i); key(j); key(k); key(l); key(m);
	key(n); key(o); key(p); key(q); key(r); key(s); key(t); key(u); key(v); key(w); key(x); key(y); key(z);
	key(multiply); key(divide); key(plus); key(minus); key(equals); key(kpenter); key(separator);
	key(kp0); key(kp1); key(kp2); key(kp3); key(kp4); key(kp5); key(kp6); key(kp7); key(kp8); key(kp9);
#undef key

	return 1;
}
