#include <time.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include "crayfish.h"

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

unsigned cf_width() { return DisplayWidth(dpy, screen); }
unsigned cf_height() { return DisplayHeight(dpy, screen); }

void cf_sleep(unsigned milli)
{
	struct timespec req = {milli/1000, (milli%1000)*1000}, rem;
	nanosleep(&req, &rem);
}

void cf_movepointer(int x, int y) { XWarpPointer(dpy, 0, root, 0,0,0,0, x,y); }

void cf_button(unsigned button, int press)
{
	static const unsigned buttons[] = { Button1, Button3, Button2, Button4, Button5 };
	buttonevent(buttons[button], press ? ButtonPress : ButtonRelease);
	XFlush(dpy);
}

// FIXIT: Crappy workaround. Use XQueryKeymap and XGetModifierMapping?
static void updatemodifiers(KeySym key, int press)
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

void cf_key(unsigned key, int press)
{
	static const unsigned keys[cf_last] =
	{
		0, XK_BackSpace, XK_Tab, XK_Return, XK_Escape, XK_space,
		XK_apostrophe, XK_comma, XK_period, XK_slash,
		XK_0, XK_1, XK_2, XK_3, XK_4, XK_5, XK_6, XK_7, XK_8, XK_9, XK_semicolon, XK_equal,
		XK_bracketleft, XK_backslash, XK_bracketright,
		XK_a, XK_b, XK_c, XK_d, XK_e, XK_f, XK_g, XK_h, XK_i, XK_j, XK_k, XK_l, XK_m,
		XK_n, XK_o, XK_p, XK_q, XK_r, XK_s, XK_t, XK_u, XK_v, XK_w, XK_x, XK_y, XK_z,
		XK_Delete, XK_Pause, XK_Control_L, XK_Control_R, XK_Shift_L, XK_Shift_R, XK_Alt_L, XK_Alt_R, XK_Super_L, XK_Super_R, XK_Menu,
		XK_Up, XK_Down, XK_Left, XK_Right, XK_Home, XK_End, XK_Page_Up, XK_Page_Down, XK_Insert, XK_KP_Separator,
		XK_Caps_Lock, XK_Scroll_Lock, XK_Num_Lock, XK_Select, XK_Print,
		XK_F1, XK_F2, XK_F3, XK_F4, XK_F5, XK_F6, XK_F7, XK_F8, XK_F9, XK_F10, XK_F11, XK_F12,
		XK_KP_0, XK_KP_1, XK_KP_2, XK_KP_3, XK_KP_4, XK_KP_5, XK_KP_6, XK_KP_7, XK_KP_8, XK_KP_9,
		XK_multiply, XK_division, XK_plus, XK_minus, XK_KP_Enter, XK_backslash
	};

	Window focus;
	int revert;
	KeySym ks = keys[key];
	XGetInputFocus(dpy, &focus, &revert);
	keyevent(focus, XKeysymToKeycode(dpy, ks), press ? KeyPress : KeyRelease);
	XFlush(dpy);
	updatemodifiers(ks, press);
}
