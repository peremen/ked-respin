// Editor class

#ifndef _editor_
#define _editor_

#include "basic.h"
#include "aline.h"

class editor : public basic
{
  int curx;
  int cury;
  int f;
  int m;
  int l;
  int state;
  int pc;
  int stack[10];
  int sp;
  int start_x;
  int start_y;
  int mark_start_x;
  int mark_start_y;
  int mark_end_x;
  int mark_end_y;
  int mark_fix_x;
  int mark_fix_y;
  Bool single_line;
  Bool marked;
  Bool out_line;
  const int change_mode = 'K' - '@';
  aline* curline;
  aline* home_line;
  aline* first_line;
  aline* last_line;
  aline* value_line;
  aline* mark_start_line;
  aline* mark_end_line;
  aline* mark_fix_line;
  aline* cut_start_line;
  char* cp;
  char* mark_start_p;
  char* mark_end_p;
  char* mark_fix_p;
  void expose(XEvent*);
  void key_press(XEvent*);
  void button_press(XEvent*);
  void motion(XEvent*);
  void focus_in();
  void focus_out();
  void forward(Bool = True);
  void backward();
  void insert_char(int, Bool = True, Bool = False);
  void delete_char();
  void delete_backward_char();
  void beginning_of_line();
  void end_of_line();
  void new_line();
  void previous_line();
  void next_line();
  void page_up();
  void page_down();
  void other(int);
  void scroll_left(int);
  void scroll_right(int);
  void up();
  void down();
  void find_position(int, char*&, int&);
  void flip_cursor();
  void store(int);
  void store(int, int, int);
  void push(int c) { stack[sp++] = c; }
  void reset_state() { state = state ? 1 : 0; }
  void mark_forward();
  void mark_backward();
  void mark_beginning_of_line();
  void mark_end_of_line();
  void mark_previous_line();
  void mark_next_line();
  void delete_mark();
  void copy_line(int, int, int, int, int, int);
  void clear_line(int, int, int, int);
  void clear_print(char*, int, int, int, int);
  void print(char*, int, int);
  void print(char*, int, int, int, int);
  void print(int, int, int, int, int);
  void clear_all() { XClearWindow(display(), window()); }
  void reverse_line(int, int, int, int);
  void set_mark_start();
  void set_mark_end();
  void set_mark_fix();
  void set_start_end();
  void find_mouse_position(int, int, aline*&, char*&, int&, int&);
  void delete_cut();
  int fcon(int);
  int vow(int);
  int lcon(int);
  int comvow(int, int);
  int comcon(int, int);
  int pop() { return stack[--sp]; }
  int cursor_w() { return (*cp & 0x80 ? kcwidth() : ecwidth()); }
  int char_len() { return (*cp & 0x80 ? 3 : 1); }
  int screen_x(int x) { return x - start_x; }
  int screen_y(int y) { return (y - start_y) * char_height(); }
  int visible_line() { return (height() + char_height() - 1) / char_height(); }
  int end_x() { return start_x + width(); }
  int end_y() { return start_y + visible_line() - 1; }
public:
  editor(container*, int, int, int = 1, Bool = False);
  Bool handle(XEvent*);
  Bool next_value(char*);
  void kauto(int);
  void delete_all();
  void append(char*);
  void replace(char*);
  void list_all();
  void first_value(char*);
  void cut();
  void copy();
  void paste();
  char* value() { return home_line->buf; }
  int left_offset() { return start_x; }
};

inline void editor::copy_line(int sx, int sy, int w, int h, int dx, int dy)
{
  XCopyArea(display(), window(), window(), gc(),
	    sx - start_x, (sy - start_y) * char_height(),
	    w, h * char_height(),
	    dx - start_x, (dy - start_y) * char_height());
}

inline void editor::clear_line(int x, int y, int w, int h)
{
  XClearArea(display(), window(),
	     x - start_x, (y - start_y) * char_height(),
	     w, h * char_height(), False);
}

inline void editor::print(char* str, int x, int y)
{
  out(str, x - start_x, (y - start_y) * char_height() + char_ascent());
}

inline void editor::print(char* str, int x, int y, int sx, int ex)
{
  out(str, x - start_x, (y - start_y) * char_height() + char_ascent(),
      sx, ex);
}

inline void editor::print(int f, int m, int l, int x, int y)
{
  out(f, m, l, x - start_x, (y - start_y) * char_height() + char_ascent());
}

inline void editor::clear_print(char* str, int x, int y, int sx, int ex)
{
  clear_out(str, x - start_x, (y - start_y) * char_height() + char_ascent(),
	    sx, ex);
}

inline void editor::reverse_line(int x, int y, int w, int h)
{
  XFillRectangle(display(), window(), igc(),
		 x - start_x, (y - start_y) * char_height(),
		 w, h * char_height());
}

inline void editor::set_mark_start()
{
  mark_start_line = curline;
  mark_start_p = cp;
  mark_start_x = curx;
  mark_start_y = cury;
}

inline void editor::set_mark_end()
{
  mark_end_line = curline;
  mark_end_p = cp;
  mark_end_x = curx;
  mark_end_y = cury;
}

inline void editor::set_mark_fix()
{
  mark_fix_line = curline;
  mark_fix_p = cp;
  mark_fix_x = curx;
  mark_fix_y = cury;
}

#endif // _editor_
