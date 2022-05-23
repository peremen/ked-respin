// Basic class

#ifndef _basic_
#define _basic_

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include "Kfont.h"

/*
 * This macro defines the location of the directory in which the
 * X11R4 application default files are kept.
 */
#ifndef APPDEFAULTSDIR
#define APPDEFAULTSDIR "/usr/local/lib/X11/app-defaults"
#endif /* APPDEFAULTSDIR */

const int DoubleClick = LASTEvent + 1;

typedef void (*handler)(XEvent*, void*);
class container;

struct handler_info
{
  int type;
  handler func;
  void* arg;
  handler_info *next;
};

class basic
{
  Display* d;
  Window w;
  GC _gc;
  GC _igc;
  Kfont* _kf;
  int _x, _y, _w, _h;
  int bw, bc;		// border width and border color
  int fg, bg;		// foreground and background
  int reverse;		// reverse video
  long mask;
  Time button_press_time1 = 0;
  Time button_press_time2 = 0;
  Bool mapping = True;
  Bool pop = False;
  container* p;                // parent
  handler_info* handlers;
public:
  basic(container*, long, char* = 0, Bool = True, int = 1, Bool = False,
        int* = 0, char ** = 0);
  Display* display() { return d; }
  Window window() { return w; }
  GC gc() { return _gc; }
  int x() { return _x; }
  int y() { return _y; }
  int width() { return _w; }
  int height() { return _h; }
  int border_width() { return bw; }
  int border_color() { return bc; }
  int foreground() { return fg; }
  int background() { return bg; }
  int reversevid() { return reverse; }
  Bool popup() { return pop; }
  container* parent() { return p; }
  void set(int x, int y, int w, int h) { _x = x; _y = y; _w = w; _h = h; }
  void set_xy(int x, int y) { _x = x; _y = y; }
  void set_wh(int w, int h) { _w = w; _h = h; }
  void map() { XMapRaised(d, w); }
  void unmap() { XUnmapWindow(d, w); }
  void add_event(int, handler, void*);
  void put_bitmap(char*, int, int, int, int, Bool rev = False);
  void run();
  int out(char* str, int x, int y);
  void out(char*, int, int, int, int);
  void clear_out(char*, int, int, int, int);
  void out(char, char, char, int, int);
  int text_width(char*);
  int char_height();
  int char_ascent();
  int char_descent();
  int kcwidth() { return _kf->kcwidth(); }
  int ecwidth() { return _kf->ecwidth(); }
  GC igc() { return _igc; }
  virtual void create();
  virtual Bool handle(XEvent*);
};

inline int basic::out(char* str, int x, int y)
{
  return _kf->out(this, str, x, y);
}

inline void basic::out(char* str, int x, int y, int sx, int ex)
{
  _kf->out(this, str, x, y, sx, ex);
}

inline void basic::clear_out(char* str, int x, int y, int sx, int ex)
{
  _kf->clear_out(this, str, x, y, sx, ex);
}

inline void basic::out(char f, char m, char l, int x, int y)
{
  _kf->out(this, f, m, l, x, y);
}

inline int basic::text_width(char* str)
{
  return _kf->text_width(str);
}

inline int basic::char_height()
{
  return _kf->char_height();
}

inline int basic::char_ascent()
{
  return _kf->char_ascent();
}

inline int basic::char_descent()
{
  return _kf->char_descent();
}

#endif // _basic_
