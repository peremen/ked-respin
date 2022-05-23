// Scroll Bar class implementation

#include <std.h>
#include <signal.h>
#include <time.h>
#include "scrollbar.h"
#include "margin.h"
#include "up"
#include "down"
#include "up_fill"
#include "down_fill"
#include "stipple"
#include "macro.h"

const int Up = 1;
const int Down = 2;

static scrollbar* sb;
void (*timer_func)();

void set_timer(int msec, void (*func)())
{
  timer_func = func;
  struct itimerval value;
  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = msec * 1000;
  value.it_value.tv_sec = 0;
  value.it_value.tv_usec = msec * 1000;
  setitimer(ITIMER_REAL, &value, 0);
  signal(SIGALRM, func);
}

static void timer_caller()
{
  sb->timer();
}

scrollbar::scrollbar(scroll* scrll) :
       (scrll->parent(), ExposureMask|ButtonPressMask|ButtonReleaseMask|
	ButtonMotionMask)
{
  s = scrll;
  sb = this;
  set_wh(up_width + border_text_m * 2, s->height());
}

void scrollbar::create()
{
  basic::create();
  XGCValues values;
  values.foreground = Black;
  values.background = White;
  values.fill_style = FillOpaqueStippled;
  values.stipple = XCreateBitmapFromData(display(), window(), stipple_bits,
					 stipple_width, stipple_height);
  sgc = XCreateGC(display(), window(),
		  GCForeground|GCBackground|GCFillStyle|GCStipple,
		  &values);
}

Bool scrollbar::handle(XEvent* event)
{
  if (event->xany.window == window())
    {
      switch (event->type)
	{
	case Expose:
	  expose(event);
	  break;
	case ButtonPress:
	  button_press(event);
	  break;
	case ButtonRelease:
	  button_release(event);
	  break;
	case MotionNotify:
	  motion(event);
	}
      basic::handle(event);
      return True;
    }
  return False;
}

void scrollbar::timer()
{
  switch (continue_job)
    {
    case Up:
      s->up();
      calc_draw_slider();
      XFlush(display());
      break;
    case Down:
      s->down();
      calc_draw_slider();
      XFlush(display());
    }
}

void scrollbar::expose(XEvent* event)
{
  put_bitmap(up_bits, border_text_m, border_text_m, up_width, up_height);
  put_bitmap(down_bits, border_text_m, height() - down_height - border_text_m,
	     down_width, down_height);
  XDrawLine(display(), window(), gc(), 0, border_text_m * 2 + up_height,
	    width(), border_text_m * 2 + up_height);
  XDrawLine(display(), window(), gc(), 0,
	    height() - border_text_m * 2 - down_height - 1,
	    width(), height() - border_text_m * 2 - down_height - 1);
  calc_draw_slider();
}

void scrollbar::button_press(XEvent* event)
{
  if (event->xbutton.y < border_text_m * 2 + up_height)
    {
      put_bitmap(up_fill_bits, border_text_m, border_text_m,
		 up_fill_width, up_fill_height, True);
      s->up();
      calc_draw_slider();
      continue_job = Up;
      set_timer(100, timer_caller);
    }
  else if (event->xbutton.y > s->height() - border_text_m * 2 - down_height
	   - 1)
    {
      put_bitmap(down_fill_bits,
		 border_text_m, height() - down_fill_height - border_text_m,
		 down_fill_width, down_fill_height, True);
      s->down();
      calc_draw_slider();
      continue_job = Down;
      set_timer(100, timer_caller);
    }
  else if (in_slider_box(event->xbutton.y))
    set_drag(event->xbutton.y);
  else
    {
      if (event->xbutton.y < slider_y)
	s->set_start(s->start_position() - s->visible_size() + 1);
      else
	s->set_start(s->start_position() + s->visible_size() - 1);
      calc_draw_slider();
    }
}

void scrollbar::motion(XEvent* event)
{
  if (drag)
    drag_to(event->xmotion.y);
}

void scrollbar::button_release(XEvent* event)
{
  if (drag)
    {
      int ts = s->total_size();
      int ms = height() - up_height - down_height - border_text_m * 4 - 3;
      int y = slider_y - up_height - border_text_m * 2 - 1;
      int sp = ts * y / ms;
      s->set_start(sp);
      set_drag(0);
    }
  set_timer(0, SIG_IGN);
  if (continue_job == Up)
    put_bitmap(up_fill_bits, border_text_m, border_text_m,
	       up_fill_width, up_fill_height, True);
  else if (continue_job == Down)
    put_bitmap(down_fill_bits,
	       border_text_m, height() - down_fill_height - border_text_m,
	       down_fill_width, down_fill_height, True);
  continue_job = 0;
}

void scrollbar::draw_slider(int by, int bs)
{
  int y1=border_text_m*2+up_height+1;
  XFillRectangle(display(), window(), sgc, 0, y1, width(), by-y1);
  int y2=by+bs;
  int h=height()-border_text_m*2-up_height-y2-1;
  XFillRectangle(display(), window(), sgc, 0, y2, width(), h);
  XClearArea(display(), window(), 0, by, 0, bs, False);
  XDrawRectangle(display(), window(), gc(), 0, by, width() - 1, bs);
  slider_y = by;
  slider_size = bs;
}

void scrollbar::calc_draw_slider()
{
  int ts = s->total_size();
  int ms = height() - up_height - down_height - border_text_m * 4 - 2;
  if (ts == 0)
    draw_slider(up_height + border_text_m * 2 + 1, ms);
  else
    {
      int sp = s->start_position();
      int vs = s->visible_size();
      int by = ms * sp / ts + up_height + border_text_m * 2 + 1;
      int bs = ms * vs / ts;
      int h = height()-border_text_m*4-up_height*2-3;
      if (bs > h)
	bs = h;
      draw_slider(by, bs);
    }
}

void scrollbar::set_drag(int y)
{
  if (y == 0)
    drag = 0;
  else
    drag = y - slider_y;
}

void scrollbar::drag_to(int y)
{
  int newy = y - drag;
  if (newy > up_height + border_text_m * 2 &&
      newy + slider_size < height() - border_text_m * 2 - down_height - 1)
    draw_slider(newy, slider_size);
  else if (newy <= up_height + border_text_m * 2)
    draw_slider(up_height + border_text_m * 2 + 1, slider_size);
  else if (newy + slider_size >= height() - border_text_m * 2 - down_height
	   - 1)
    draw_slider(height() - border_text_m * 2 - down_height - 2 - slider_size,
		slider_size);
}

Bool scrollbar::in_slider_box(int y)
{
  if (y > slider_y && y < slider_y + slider_size)
    return True;
  else
    return False;
}
