#include <time.h>
#include <memory.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#define LUA_COMPAT_MODULE
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static Display *dpy;
static int screen;
static Window root;
static XIM im;
static XIC ic;

__attribute__((constructor)) static void init()	// any other ideas?
{
	dpy = XOpenDisplay(0);
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	im = XOpenIM(dpy, 0, 0, 0);
	ic = XCreateIC(im, XNInputStyle, XIMPreeditNothing|XIMStatusNothing, 0);
	XSync(dpy, 0);
}

__attribute__((destructor)) static void uninit()
{
	XSync(dpy, 0);
	XCloseDisplay(dpy);
}

unsigned luada_numlock()
{
	XModifierKeymap *modmap = XGetModifierMapping(dpy);
	for(unsigned i = 0; i < 8; ++i) for(unsigned j = 0; j < modmap->max_keypermod; ++j)
		if(modmap->modifiermap[i*modmap->max_keypermod + j] == XKeysymToKeycode(dpy, XK_Num_Lock)) return 1 << i;
	return 0;
}

static void sendevent(Window wnd, unsigned mask, XEvent *e)
{
	if(!XSendEvent(dpy, wnd, True, mask, e)) fprintf(stderr, "Send event failed\n");
}

static int luada_sleep(lua_State *lua)
{
	unsigned milli = lua_tounsigned(lua,1);
	struct timespec req = {milli/1000, (milli%1000)*1000}, rem;
	nanosleep(&req, &rem);
	return 0;
}

static int luada_movepointer(lua_State *lua)
{
	int x = lua_tointeger(lua,1);
	int y = lua_tointeger(lua,2);
	XWarpPointer(dpy, 0, root, 0,0,0,0, x,y);
	return 0;
}

static int luada_button(lua_State *lua)
{
	XButtonEvent e;
	memset(&e, 0, sizeof(e));
	e.button = lua_tounsigned(lua,1);
	e.type = lua_toboolean(lua,2) ? ButtonPress : ButtonRelease;
	e.same_screen = True;

	e.subwindow = root;
	do
	{
		e.window = e.subwindow;
		XQueryPointer(dpy, e.window, &e.root, &e.subwindow, &e.x_root, &e.y_root, &e.x, &e.y, &e.state);
	}
	while(e.subwindow);

	sendevent(PointerWindow, ButtonPressMask, (XEvent*)&e);
	XSync(dpy,0);
	return 0;
}

static int luada_key(lua_State *lua)
{
	XKeyEvent e;
	Window focus;
	int revert;
	KeySym keycode = lua_tounsigned(lua,1);
	unsigned modifiers = lua_tounsigned(lua,2);
	int press = lua_toboolean(lua,3);

	XGetInputFocus(dpy, &focus, &revert);

	e.display     = dpy;
	e.window      = focus;
	e.root        = root;
	e.subwindow   = None;
	e.time        = CurrentTime;
	e.x           = 1;
	e.y           = 1;
	e.x_root      = 1;
	e.y_root      = 1;
	e.same_screen = True;
	e.keycode     = XKeysymToKeycode(dpy, keycode);
	e.state       = modifiers;
	e.type = press ? KeyPress : KeyRelease;

	sendevent(e.window, KeyPressMask, (XEvent*)&e);
	XSync(dpy,0);
	return 0;
}

static const luaL_Reg luada_functions[] =
{
	{"sleep",		luada_sleep},
	{"movepointer",	luada_movepointer},
	{"button",		luada_button},
	{"key",			luada_key},
	{0, 0}
};

LUALIB_API int luaopen_libluada(lua_State *lua)
{
	luaL_openlib(lua, "luada", luada_functions, 0);
	lua_pushstring(lua, "width");
	lua_pushinteger(lua, DisplayWidth(dpy, screen));
	lua_settable(lua, -3);
	lua_pushstring(lua, "height");
	lua_pushinteger(lua, DisplayHeight(dpy, screen));
	lua_settable(lua, -3);
	return 1;
}
