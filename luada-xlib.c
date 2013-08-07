#include <time.h>
#include <memory.h>
#include <stdlib.h>
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
static unsigned modifiers = 0;
static unsigned min_press_sleep = 10, delta_press_sleep = 10;

__attribute__((constructor)) static void init()
{
	dpy = XOpenDisplay(0);
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	im = XOpenIM(dpy, 0, 0, 0);
	ic = XCreateIC(im, XNInputStyle, XIMPreeditNothing|XIMStatusNothing, 0);
	XSync(dpy, 0);
}

__attribute__((destructor)) static void uninit()	// any other ideas?
{
	XSync(dpy, 0);
	XCloseDisplay(dpy);
}

/*unsigned luada_numlock()
{
	XModifierKeymap *modmap = XGetModifierMapping(dpy);
	for(unsigned i = 0; i < 8; ++i) for(unsigned j = 0; j < modmap->max_keypermod; ++j)
		if(modmap->modifiermap[i*modmap->max_keypermod + j] == XKeysymToKeycode(dpy, XK_Num_Lock)) return 1 << i;
	return 0;
}*/

static void sendevent(Window wnd, unsigned mask, XEvent *e)
{
	if(!XSendEvent(dpy, wnd, True, mask, e)) fprintf(stderr, "Send event failed\n");
}

static void buttonevent(unsigned button, int type)
{
	XButtonEvent e;
	memset(&e, 0, sizeof(e));
	e.button = button;
	e.type = type;
	e.same_screen = True;
	e.subwindow = root;
	e.state = modifiers;
	do
	{
		e.window = e.subwindow;
		XQueryPointer(dpy, e.window, &e.root, &e.subwindow, &e.x_root, &e.y_root, &e.x, &e.y, &e.state);
	}
	while(e.subwindow);
	sendevent(PointerWindow, ButtonPressMask, (XEvent*)&e);
}

static void keyevent(Window w, unsigned code, int type)
{
	XKeyEvent e;
	e.display     = dpy;
	e.window      = w;
	e.root        = root;
	e.subwindow   = None;
	e.time        = CurrentTime;
	e.x = e.y = e.x_root = e.y_root = 1;
	e.same_screen = True;
	e.keycode     = code;
	e.state       = modifiers;
	e.type = type;
	sendevent(w, KeyPressMask, (XEvent*)&e);
}

void millisleep(unsigned milli)
{
	struct timespec req = {milli/1000, (milli%1000)*1000}, rem;
	nanosleep(&req, &rem);
}

static int luada_sleep(lua_State *lua)
{
	millisleep(lua_tounsigned(lua,1));
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
	buttonevent(lua_tounsigned(lua,1), lua_toboolean(lua,2) ? ButtonPress : ButtonRelease);
	XFlush(dpy);
	return 0;
}

static int luada_click(lua_State *lua)
{
	buttonevent(lua_tounsigned(lua,1), ButtonPress);
	XFlush(dpy);
	millisleep(min_press_sleep + rand()%(delta_press_sleep+1));

	buttonevent(lua_tounsigned(lua,1), ButtonRelease);
	XFlush(dpy);
	return 0;
}

// FIXIT: Crappy workaround. Use XQueryKeymap and XGetModifierMapping?
void updatemodifiers(KeySym key, int press)
{
	switch(key)
	{
		case XK_Shift_L: case XK_Shift_R:
			if(press) modifiers |= ShiftMask; else modifiers &= ~ShiftMask;
			break;
		case XK_Control_L: case XK_Control_R:
			if(press) modifiers |= ControlMask; else modifiers &= ~ControlMask;
			break;
		case XK_Alt_L: case XK_Alt_R:
			if(press) modifiers |= Mod1Mask; else modifiers &= ~Mod1Mask;
			break;
		case XK_Super_L: case XK_Super_R:
			if(press) modifiers |= Mod4Mask; else modifiers &= ~Mod4Mask;
			break;
		case XK_Caps_Lock:
			if(press) modifiers ^= LockMask;
			break;
	}
}

static int luada_key(lua_State *lua)
{
	Window focus;
	int revert;
	KeySym key = lua_tounsigned(lua,1);
	int press = lua_toboolean(lua,2);
	XGetInputFocus(dpy, &focus, &revert);

	keyevent(focus, XKeysymToKeycode(dpy, key), press ? KeyPress : KeyRelease);
	XFlush(dpy);
	updatemodifiers(key, press);
	return 0;
}

static int luada_press(lua_State *lua)
{
	Window focus;
	int revert;
	KeySym key = lua_tounsigned(lua,1);
	KeyCode code = XKeysymToKeycode(dpy, key);
	XGetInputFocus(dpy, &focus, &revert);

	keyevent(focus, code, KeyPress);
	XFlush(dpy);
	updatemodifiers(key, 1);
	millisleep(min_press_sleep + rand()%(delta_press_sleep+1));

	keyevent(focus, code, KeyRelease);
	XFlush(dpy);
	updatemodifiers(key, 0);
	return 0;
}

static const luaL_Reg luada_functions[] =
{
	{"sleep",		luada_sleep},
	{"movepointer",	luada_movepointer},
	{"button",		luada_button},
	{"key",			luada_key},
	{"click",		luada_click},
	{"press",		luada_press},
	{0, 0}
};

static void luada_set_uint(lua_State *lua, const char *key, unsigned val)
{
	lua_pushstring(lua, key);
	lua_pushinteger(lua, val);
	lua_settable(lua, -3);
}

LUALIB_API int luaopen_luada(lua_State *lua)
{
	luaL_openlib(lua, "luada", luada_functions, 0);
	luada_set_uint(lua, "width", DisplayWidth(dpy, screen));
	luada_set_uint(lua, "height", DisplayHeight(dpy, screen));

	lua_pushstring(lua, "keys");
	lua_createtable(lua, 0, 0);
		luada_set_uint(lua, "esc", XK_Escape);
		luada_set_uint(lua, "tab", XK_Tab);
		luada_set_uint(lua, "lshift", XK_Shift_L);
		luada_set_uint(lua, "rshift", XK_Shift_R);
		luada_set_uint(lua, "lcontrol", XK_Control_L);
		luada_set_uint(lua, "rcontrol", XK_Control_R);
		luada_set_uint(lua, "lalt", XK_Alt_L);
		luada_set_uint(lua, "ralt", XK_Alt_R);
		luada_set_uint(lua, "lsuper", XK_Super_L);
		luada_set_uint(lua, "rsuper", XK_Super_R);
		luada_set_uint(lua, "menu", XK_Menu);
		luada_set_uint(lua, "numlock", XK_Num_Lock);
		luada_set_uint(lua, "capslock", XK_Caps_Lock);
		luada_set_uint(lua, "numlock", XK_Num_Lock);
		luada_set_uint(lua, "scrolllock", XK_Scroll_Lock);
		luada_set_uint(lua, "pause", XK_Pause);
		luada_set_uint(lua, "del", XK_Delete);
		luada_set_uint(lua, "backspace", XK_BackSpace);
		luada_set_uint(lua, "enter", XK_Return);
		luada_set_uint(lua, "home", XK_Home);
		luada_set_uint(lua, "end", XK_End);
		luada_set_uint(lua, "pageup", XK_Page_Up);
		luada_set_uint(lua, "pagedown", XK_Page_Down);
		luada_set_uint(lua, "insert", XK_Insert);
		luada_set_uint(lua, "up", XK_Up);
		luada_set_uint(lua, "down", XK_Down);
		luada_set_uint(lua, "left", XK_Left);
		luada_set_uint(lua, "right", XK_Right);
		luada_set_uint(lua, "f1", XK_F1);
		luada_set_uint(lua, "f2", XK_F2);
		luada_set_uint(lua, "f3", XK_F3);
		luada_set_uint(lua, "f4", XK_F4);
		luada_set_uint(lua, "f5", XK_F5);
		luada_set_uint(lua, "f6", XK_F6);
		luada_set_uint(lua, "f7", XK_F7);
		luada_set_uint(lua, "f8", XK_F8);
		luada_set_uint(lua, "f9", XK_F9);
		luada_set_uint(lua, "f10", XK_F10);
		luada_set_uint(lua, "f11", XK_F11);
		luada_set_uint(lua, "f12", XK_F12);

		luada_set_uint(lua, "a", XK_a);
		luada_set_uint(lua, "b", XK_b);
		luada_set_uint(lua, "c", XK_c);
		luada_set_uint(lua, "d", XK_d);
		luada_set_uint(lua, "e", XK_e);
		luada_set_uint(lua, "f", XK_f);
		luada_set_uint(lua, "g", XK_g);
		luada_set_uint(lua, "h", XK_h);
		luada_set_uint(lua, "i", XK_i);
		luada_set_uint(lua, "j", XK_j);
		luada_set_uint(lua, "k", XK_k);
		luada_set_uint(lua, "l", XK_l);
		luada_set_uint(lua, "m", XK_m);
		luada_set_uint(lua, "n", XK_n);
		luada_set_uint(lua, "o", XK_o);
		luada_set_uint(lua, "p", XK_p);
		luada_set_uint(lua, "q", XK_q);
		luada_set_uint(lua, "r", XK_r);
		luada_set_uint(lua, "s", XK_s);
		luada_set_uint(lua, "t", XK_t);
		luada_set_uint(lua, "u", XK_u);
		luada_set_uint(lua, "v", XK_v);
		luada_set_uint(lua, "w", XK_w);
		luada_set_uint(lua, "x", XK_x);
		luada_set_uint(lua, "y", XK_y);
		luada_set_uint(lua, "z", XK_z);

		luada_set_uint(lua, "multiply", XK_KP_Multiply);
		luada_set_uint(lua, "divide", XK_KP_Divide);
		luada_set_uint(lua, "plus", XK_KP_Add);
		luada_set_uint(lua, "minus", XK_KP_Subtract);
		luada_set_uint(lua, "equals", XK_KP_Equal);
		luada_set_uint(lua, "kpenter", XK_KP_Enter);
		luada_set_uint(lua, "kp0", XK_KP_0);
		luada_set_uint(lua, "kp1", XK_KP_1);
		luada_set_uint(lua, "kp2", XK_KP_2);
		luada_set_uint(lua, "kp3", XK_KP_3);
		luada_set_uint(lua, "kp4", XK_KP_4);
		luada_set_uint(lua, "kp5", XK_KP_5);
		luada_set_uint(lua, "kp6", XK_KP_6);
		luada_set_uint(lua, "kp7", XK_KP_7);
		luada_set_uint(lua, "kp8", XK_KP_8);
		luada_set_uint(lua, "kp9", XK_KP_9);
		luada_set_uint(lua, "kpdel", XK_KP_Decimal);
	lua_settable(lua, -3);

	return 1;
}
