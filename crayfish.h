#ifndef CRAYFISH_H
#define CRAYFISH_H

enum { cf_lbutton, cf_rbutton, cf_mbutton, cf_xbutton1, cf_xbutton2 };

enum
{
	cf_unknown, cf_backspace, cf_tab, cf_enter, cf_esc, cf_space,
	cf_apostrophe, cf_comma, cf_period, cf_slash,
	cf_0, cf_1, cf_2, cf_3, cf_4, cf_5, cf_6, cf_7, cf_8, cf_9, cf_semicolon, cf_equals,
	cf_lbracket, cf_backslash, cf_rbracket,
	cf_a, cf_b, cf_c, cf_d, cf_e, cf_f, cf_g, cf_h, cf_i, cf_j, cf_k, cf_l, cf_m,
	cf_n, cf_o, cf_p, cf_q, cf_r, cf_s, cf_t, cf_u, cf_v, cf_w, cf_x, cf_y, cf_z,
	cf_del, cf_lbutton, cf_rbutton, cf_mbutton, cf_xbutton1, cf_xbutton2, cf_pause, cf_brk, cf_clear,
	cf_lcontrol, cf_rcontrol, cf_lshift, cf_rshift, cf_lalt, cf_ralt, cf_menu,
	cf_up, cf_down, cf_left, cf_right, cf_home, cf_end, cf_pageup, cf_pagedown, cf_insert, cf_separator,
	cf_capslock, cf_scroll, cf_numlock, cf_select, cf_print, cf_sleep,
	cf_f1, cf_f2, cf_f3, cf_f4, cf_f5, cf_f6, cf_f7, cf_f8, cf_f9, cf_f10, cf_f11, cf_f12,
	cf_kp0, cf_kp1, cf_kp2, cf_kp3, cf_kp4, cf_kp5, cf_kp6, cf_kp7, cf_kp8, cf_kp9,
	cf_multiply, cf_divide, cf_plus, cf_minus, cf_kpenter, cf_backtick, cf_last
};

extern unsigned cf_nativekeys[cf_last];

unsigned cf_width();
unsigned cf_height();
void cf_movepointer(int x, int y);
void cf_button(unsigned button, int press);
void cf_key(unsigned key, int press);
void cf_sleep(unsigned milliseconds);
void cf_wait_down();
void cf_wait_up();

#endif
