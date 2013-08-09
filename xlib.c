#include <time.h>
#include <memory.h>
#include <stdlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

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
	Window focus;
	int revert;
	XGetInputFocus(dpy, &focus, &revert);
	keyevent(focus, XKeysymToKeycode(dpy, key), press ? KeyPress : KeyRelease);
	XFlush(dpy);
	updatemodifiers(key, press);
}
