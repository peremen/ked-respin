// Editor class implementation

#include <string.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include "editor.h"

editor::editor(container* parent, int w, int h, int bw,
	       Bool single_line_f) :
       basic(parent, ExposureMask|KeyPressMask|ButtonPressMask|ButtonMotionMask|
	FocusChangeMask, 0, True, bw)
{
  set_wh(w * kcwidth(), h * char_height());
  first_line = 0;
  cut_start_line = 0;
  delete_all();
  append("");
  single_line = single_line_f;
}

Bool editor::handle(XEvent* event)
{
  if (event->xany.window == window())
    {
      switch (event->type)
	{
	case Expose:
	  expose(event);
	  break;
	case KeyPress:
	  key_press(event);
	  break;
	case ButtonPress:
	  button_press(event);
	  break;
	case MotionNotify:
	  motion(event);
	  break;
	case FocusIn:
	  focus_in();
	  break;
	case FocusOut:
	  focus_out();
	  break;
	}
      basic::handle(event);
      return True;
    }
  return False;
}

void editor::expose(XEvent* event)
{
  aline* i = home_line;
  for (int yy = start_y; screen_y(yy) < height() && i != 0; yy++, i = i->down)
    if (screen_y(yy) + char_height() >= event->xexpose.y &&
	screen_y(yy) < event->xexpose.y + event->xexpose.height)
      clear_print(i->buf, 0, yy,
		  event->xexpose.x,
		  event->xexpose.x + event->xexpose.width - 1);
  if (screen_x(curx) + cursor_w() > event->xexpose.x &&
      screen_x(curx) < event->xexpose.x + event->xexpose.width &&
      screen_y(cury) + char_height() >= event->xexpose.y &&
      screen_y(cury) < event->xexpose.y + event->xexpose.height)
    flip_cursor();
}

void editor::key_press(XEvent* event)
{
  char buf[2];
  KeySym keysym;

  flip_cursor();
  if (XLookupString(&event->xkey, buf, 1, &keysym, 0))
    {
      delete_mark();
      buf[1] = 0;
      kauto(buf[0]);
    }
  else
    if (event->xkey.state & ShiftMask)
      switch (keysym)
	{
	case XK_Left:
	  mark_backward();
	  break;
	case XK_Right:
	  mark_forward();
	  break;
	case XK_Home:
	case XK_R7:
	  mark_beginning_of_line();
	  break;
	case XK_End:
	case XK_R13:
	  mark_end_of_line();
	  break;
	case XK_Up:
	  mark_previous_line();
	  break;
	case XK_Down:
	  mark_next_line();
	}
    else
      {
	if (IsCursorKey(keysym))
	  delete_mark();
	switch (keysym)
	  {
	  case XK_Left:
	    backward();
	    break;
	  case XK_Right:
	    forward();
	    break;
	  case XK_Home:
	  case XK_R7:
	    beginning_of_line();
	    break;
	  case XK_End:
	  case XK_R13:
	    end_of_line();
	    break;
	  case XK_Up:
	    previous_line();
	    break;
	  case XK_Down:
	    next_line();
	    break;
	  case XK_Prior:
	  case XK_R9:
	    page_up();
	    break;
	  case XK_Next:
	  case XK_R15:
	    page_down();
	    break;
	  case XK_F2:
	    if (state > 1)
	      forward();
	    state = state ? 0 : 1;
	    break;
	  }
      }
  flip_cursor();
}

void editor::button_press(XEvent* event)
{
  flip_cursor();
  delete_mark();
  find_mouse_position(event->xbutton.x, event->xbutton.y, curline, cp,
		      curx, cury);
  reset_state();
  flip_cursor();
}

void editor::motion(XEvent* event)
{
  flip_cursor();
  int ox = curx;
  int oy = cury;
  if (marked)
    {
      find_mouse_position(event->xmotion.x, event->xmotion.y,
			  curline, cp, curx, cury);
      set_start_end();
    }
  else
    {
      set_mark_fix();
      find_mouse_position(event->xmotion.x, event->xmotion.y,
			  curline, cp, curx, cury);
      set_start_end();
      marked = True;
    }
  if (cury == oy)
    if (curx > ox)
      reverse_line(ox, cury, curx - ox, 1);
    else
      reverse_line(curx, cury, ox - curx, 1);
  else
    if (cury > oy)
      {
	reverse_line(ox, oy, end_x() - ox, 1);
	if (cury > oy + 1)
	  reverse_line(0, oy + 1, end_x(), cury - oy - 1);
	reverse_line(0, cury, curx, 1);
      }
    else
      {
	reverse_line(curx, cury, end_x() - curx, 1);
	if (oy > cury + 1)
	  reverse_line(0, cury + 1, end_x(), oy - cury - 1);
	reverse_line(0, oy, ox, 1);
      }
  flip_cursor();
}

void editor::focus_in()
{
  flip_cursor();
  out_line = False;
  flip_cursor();
}

void editor::focus_out()
{
  flip_cursor();
  out_line = True;
  flip_cursor();
}

void editor::find_mouse_position(int x, int y,
			  aline*& mouse_line, char*& mouse_p,
			  int& mouse_x, int& mouse_y)
{
  mouse_line = home_line;
  mouse_y = start_y;
  int count = y / char_height();
  while (count-- > 0 && mouse_line->down != 0)
    {
      mouse_line = mouse_line->down;
      mouse_y++;
    }
  mouse_p = mouse_line->buf;
  find_position(x, mouse_p, mouse_x);
}

void editor::find_position(int target_x, char*& p, int& x)
{
  x = 0;
  while (*p)
    {
      if (x + cursor_w() > target_x)
	break;
      if (*p == '\t')
	{
	  x = x / (ecwidth() * 8) * (ecwidth() * 8) + ecwidth() * 8;
	  p++;
	}
      else
	{
	  x += cursor_w();
	  p += char_len();
	}
    }
}
  
void editor::previous_line()
{
  if (curline->up)
    {
      curline = curline->up;
      cp = curline->buf;
      find_position(curx, cp, curx);
      up();
      if (curx < start_x)
	scroll_right(start_x - curx);
      if (curx + cursor_w() > end_x())
	scroll_left(curx + cursor_w() - end_x());
    }
  reset_state();
}

void editor::next_line()
{
  if (curline->down)
    {
      curline = curline->down;
      cp = curline->buf;
      find_position(curx, cp, curx);
      down();
      if (curx < start_x)
	scroll_right(start_x - curx);
      if (curx + cursor_w() > end_x())
	scroll_left(curx + cursor_w() - end_x());
    }
  reset_state();
}

void editor::up()
{
  if (cury - 1 < start_y)
    {
      copy_line(start_x, start_y, width(), visible_line() - 1,
		start_x, start_y + 1);
      clear_line(start_x, start_y, 0, 1);
      home_line = curline;
      start_y--;
      print(curline->buf, 0, start_y, 0, width());
      if (marked)
	if (start_y == mark_start_y)
	  if (start_y == mark_end_y)
	    reverse_line(mark_start_x, start_y, mark_end_x-mark_start_x, 1);
	  else
	    reverse_line(mark_start_x, start_y, end_x()-mark_start_x, 1);
	else if (start_y == mark_end_y)
	  reverse_line(0, start_y, mark_end_x, 1);
	else if (start_y > mark_start_y && start_y < mark_end_y)
	  reverse_line(0, start_y, end_x(), 1);
    }
  cury--;
}

void editor::down()
{
  if (cury + 1 >= end_y())
    {
      copy_line(start_x, start_y + 1, width(), visible_line() - 1,
		start_x, start_y);
      clear_line(start_x, end_y(), 0, 0);
      home_line = home_line->down;
      start_y++;
      if (cury + 1 == end_y())
	print(curline->buf, 0, end_y(), 0, width());
      else if (curline->down)
	print(curline->down->buf, 0, end_y(), 0, width());
      if (marked)
	if (end_y() == mark_start_y)
	  if (end_y() == mark_end_y)
	    reverse_line(mark_start_x, end_y(), mark_end_x-mark_start_x, 1);
	  else
	    reverse_line(mark_start_x, end_y(), end_x()-mark_start_x, 1);
	else if (end_y() == mark_end_y)
	  reverse_line(0, end_y(), mark_end_x, 1);
	else if (end_y() > mark_start_y && end_y() < mark_end_y)
	  reverse_line(0, end_y(), end_x(), 1);
    }
  cury++;
}

void editor::end_of_line()
{
  while (*cp)
    {
      if (*cp == '\t')
	curx = curx / (ecwidth() * 8) * (ecwidth() * 8) + ecwidth() * 8;
      else
	curx += cursor_w();
      cp += char_len();
    }
  if (curx + cursor_w() > end_x())
    scroll_left(curx + cursor_w() - end_x());
  reset_state();
}
  
void editor::beginning_of_line()
{
  cp = curline->buf;
  curx = 0;
  if (curx < start_x)
    scroll_right(start_x - curx);
  reset_state();
}

void editor::backward()
{
  if (cp > curline->buf)
    if (*(cp - 1) & 0x80)
      {
	cp -= 3;
	curx -= kcwidth();
      }
    else if (*(cp - 1) == '\t')
      {
	char* target_cp = cp - 1;
	curx = 0;
	cp = curline->buf;
	while (cp < target_cp)
	  {
	    if (*cp == '\t')
	      {
		curx = curx / (ecwidth() * 8) * (ecwidth() * 8)
		  + ecwidth() * 8;
		cp++;
	      }
	    else
	      {
		curx += cursor_w();
		cp += char_len();
	      }
	  }
      }
    else
      {
	cp--;
	curx -= ecwidth();
      }
  else
    if (curline->up)
      {
	previous_line();
	end_of_line();
      }
  if (curx < start_x)
    scroll_right(start_x - curx);
  reset_state();
}

void editor::flip_cursor()
{
  if (out_line)
    XDrawRectangle(display(), window(), igc(),
		   curx - start_x, (cury - start_y) * char_height(),
		   cursor_w(), char_height() - 1);
  else
    reverse_line(curx, cury, cursor_w(), 1);
}

void editor::kauto(int c)
{
  int t;

  if (c == change_mode)
    {
      if (state > 1)
	forward();
      state = state ? 0 : 1;
      return;
    }
  if (strlen(curline->buf) >= maxbuf - 4)
    {
      if (c != '\b' && c != '\r' && c != 127)
	{
	  XBell(display(), 50);
	  return;
	}
    }
  switch (state)
    {
    case 0:
      other(c);
      break;
    case 1:
      sp = 0;
      if (t = fcon(c))
	{
	  insert_char(3);
	  store(t, 128, 128);
	  f = t;
	  state = 2;
	}
      else if (t = vow(c))
	{
	  insert_char(3, False);
	  store(128, t, 128);
	  forward();
	}
      else
	other(c);
      break;
    case 2:
      if (t = vow(c))
	{
	  store(f, t, 128);
	  push(128);
	  m = t;
	  state = 3;
	}
      else if (t = fcon(c))
	{
	  forward(False);
	  insert_char(3);
	  store(t, 128, 128);
	  f = t;
	  sp = 0;
	  state = 2;
	}
      else if (c == '\b')
	{
	  delete_char();
	  state = 1;
	}
      else
	  other(c);
      break;
    case 3:
      if (t = lcon(c))
	{
	  store(f, m, t);
	  push(128);
	  l = t;
	  pc = c;
	  state = 4;
	}
      else if (t = comvow(m, c))
	{
	  store(f, t, 128);
	  push(m);
	  m = t;
	}
      else if (t = fcon(c))
	{
	  forward(False);
	  insert_char(3);
	  store(t, 128, 128);
	  f = t;
	  sp = 0;
	  state = 2;
	}
      else if (t = vow(c))
	{
	  forward(False);
	  insert_char(3, False);
	  store(128, t, 128);
	  forward();
	  state = 1;
	}
      else if (c == '\b')
	{
	  m = pop();
	  store(f, m, 128);
	  if (m == 128)
	    state = 2;
	}
      else
	  other(c);
      break;
    case 4:
      if (t = comcon(l, c))
	{
	  store(f, m, t);
	  push(l);
	  l = t;
	  pc = c;
	}
      else if (t = vow(c))
	{
	  store(f, m, pop());
	  m = t;
	  f = fcon(pc);
	  forward(False);
	  insert_char(3);
	  store(f, m, 128);
	  sp = 0;
	  push(128);
	  state = 3;
	}
      else if (t = fcon(c))
	{
	  forward(False);
	  insert_char(3);
	  store(t, 128, 128);
	  f = t;
	  sp = 0;
	  state = 2;
	}
      else if (c == '\b')
	{
	  l = pop();
	  store(f, m, l);
	  if (l == 128)
	    state = 3;
	}
      else
	  other(c);
      break;
    }
}

int editor::fcon(int c)
{
  switch (c)
    {
    case 'r':
      return 1 + 128;
    case 'R':
      return 2 + 128;
    case 's':
      return 3 + 128;
    case 'e':
      return 4 + 128;
    case 'E':
      return 5 + 128;
    case 'f':
      return 6 + 128;
    case 'a':
      return 7 + 128;
    case 'q':
      return 8 + 128;
    case 'Q':
      return 9 + 128;
    case 't':
      return 10 + 128;
    case 'T':
      return 11 + 128;
    case 'd':
      return 12 + 128;
    case 'w':
      return 13 + 128;
    case 'W':
      return 14 + 128;
    case 'c':
      return 15 + 128;
    case 'z':
      return 16 + 128;
    case 'x':
      return 17 + 128;
    case 'v':
      return 18 + 128;
    case 'g':
      return 19 + 128;
    default:
      return 0;
    }
}

int editor::vow(int c)
{
  switch (c)
    {
    case 'k':
      return 1 + 128;
    case 'o':
      return 2 + 128;
    case 'i':
      return 3 + 128;
    case 'O':
      return 4 + 128;
    case 'j':
      return 5 + 128;
    case 'p':
      return 6 + 128;
    case 'u':
      return 7 + 128;
    case 'P':
      return 8 + 128;
    case 'h':
      return 9 + 128;
    case 'y':
      return 13 + 128;
    case 'n':
      return 14 + 128;
    case 'b':
	return 18 + 128;
    case 'm':
      return 19 + 128;
    case 'l':
      return 21 + 128;
    default:
      return 0;
    }
}

int editor::lcon(int c)
{
  switch (c)
    {
    case 'r':
      return 1 + 128;
    case 'R':
      return 2 + 128;
    case 's':
      return 4 + 128;
    case 'e':
      return 7 + 128;
    case 'f':
      return 8 + 128;
    case 'a':
      return 16 + 128;
    case 'q':
      return 17 + 128;
    case 't':
      return 19 + 128;
    case 'T':
      return 20 + 128;
    case 'd':
      return 21 + 128;
    case 'w':
      return 22 + 128;
    case 'c':
      return 23 + 128;
    case 'z':
      return 24 + 128;
    case 'x':
      return 25 + 128;
    case 'v':
      return 26 + 128;
    case 'g':
      return 27 + 128;
    }
  return 0;
}

int editor::comcon(int k, int c)
{
  switch (k)
    {
    case 1 + 128:
      switch (c)
	{
	case 't':
	  return 3 + 128;
	}
      break;
    case 4 + 128:
      switch (c)
	{
	case 'w':
	  return 5 + 128;
	case 'g':
	  return 6 + 128;
	}
      break;
    case 8 + 128:
      switch (c)
	{
	case 'r':
	  return 9 + 128;
	case 'a':
	  return 10 + 128;
	case 'q':
	  return 11 + 128;
	case 't':
	  return 12 + 128;
	case 'x':
	  return 13 + 128;
	case 'v':
	  return 14 + 128;
	case 'g':
	  return 15 + 128;
	}
      break;
    case 17 + 128:
      switch (c)
	{
	case 't':
	  return 18 + 128;
	}
      break;
    }
  return 0;
}

int editor::comvow(int v, int c)
{
  switch (v)
    {
    case 9 + 128:
      switch (c)
	{
	case 'k':
	  return 10 + 128;
	case 'o':
	  return 11 + 128;
	case 'l':
	  return 12 + 128;
	}
      break;
    case 14 + 128:
      switch (c)
	{
	case 'j':
	  return 15 + 128;
	case 'p':
	  return 16 + 128;
	case 'l':
	  return 17 + 128;
	}
      break;
    case 19 + 128:
      switch (c)
	{
	case 'l':
	  return 20 + 128;
	}
      break;
    }
  return 0;
}

void editor::forward(Bool scroll)
{
  if (*cp == '\t')
    {
      curx = curx / (ecwidth() * 8) * (ecwidth() * 8) + ecwidth() * 8;
      cp++;
      if (scroll && curx + cursor_w() > end_x())
	scroll_left(curx + cursor_w() - end_x());
    }
  else if (*cp)
    {
      curx += cursor_w();
      cp += char_len();
      if (scroll && curx + cursor_w() > end_x())
	scroll_left(curx + cursor_w() - end_x());
    }
  else if (curline->down)
    {
      next_line();
      beginning_of_line();
    }
  reset_state();
}

void editor::scroll_left(int w)
{
  copy_line(start_x + w, start_y, width() - w, visible_line(),
	    start_x, start_y);
  clear_line(start_x + width() - w, start_y, w, visible_line());
  start_x += w;
  int yy = start_y;
  for (aline* i = home_line; i != 0; i = i->down)
    {
      if (marked && yy >= mark_start_y && yy <= mark_end_y)
	clear_print(i->buf, 0, yy, width() - w, width());
      else
	print(i->buf, 0, yy, width() - w, width());
      if (marked)
	if (yy == mark_start_y)
	  reverse_line(mark_start_x, yy, end_x() - mark_start_x, 1);
	else if (yy > mark_start_y && yy < mark_end_y)
	  reverse_line(start_x, yy, width(), 1);
	else if (yy == mark_end_y && mark_start_y != mark_end_y)
	  reverse_line(start_x, yy, mark_end_x - start_x, 1);
      yy++;
      if (yy > end_y())
	break;
    }
}

void editor::scroll_right(int w)
{
  copy_line(start_x, start_y, width() - w, visible_line(),
	    start_x + w, start_y);
  clear_line(start_x, start_y, w, visible_line());
  start_x -= w;
  int yy = start_y;
  for (aline* i = home_line; i != 0; i = i->down)
    {
      if (marked && yy >= mark_start_y && yy <= mark_end_y)
	clear_print(i->buf, 0, yy, 0, w);
      else
	print(i->buf, 0, yy, 0, w);
      if (marked)
	if (yy == mark_start_y)
	  reverse_line(mark_start_x, yy, end_x() - mark_start_x, 1);
	else if (yy > mark_start_y && yy < mark_end_y)
	  reverse_line(start_x, yy, width(), 1);
	else if (yy == mark_end_y && mark_start_y != mark_end_y)
	  reverse_line(start_x, yy, mark_end_x - start_x, 1);
      yy++;
      if (yy > end_y())
	break;
    }
}

void editor::delete_char()
{
  if (*cp)
    {
      char *s = cp + char_len();
      char *d = cp;
      while ((*d++ = *s++) != 0);
      clear_line(curx, cury, 0, 1);
      print(cp, curx, cury);
    }
  else
    if (curline->down)
      {
	int rest = maxbuf - (cp - curline->buf) - 1;
	strncpy(cp, curline->down->buf, rest);
	curline->buf[maxbuf - 1] = 0;
	aline* down_line = curline->down;
	curline->down = down_line->down;
	if (down_line->down != 0)
	  down_line->down->up = curline;
	delete down_line;
	print(cp, curx, cury);
	copy_line(start_x, cury + 2, end_x() - start_x, end_y() - cury - 1,
		  start_x, cury + 1);
	clear_line(start_x, end_y(), 0, 0);
	int yy = cury;
	for (aline* i = curline; i != 0; i = i->down, yy++)
	  if (yy >= end_y())
	    {
	      print(i->buf, 0, yy, 0, width());
	      break;
	    }
      }
}

void editor::other(int c)
{
  if (state > 1 && c != 127)
    forward(False);
  switch (c)
    {
    case '\b':
      delete_backward_char();
      break;
    case '\r':
      if (!single_line)
	new_line();
      break;
    case 127:
      delete_char();
      break;
    default:
      insert_char(1, True, c == '\t');
      store(c);
      forward();
    }
  reset_state();
}

void editor::new_line()
{
  aline* p = new aline;
  if (curline->down)
    curline->down->up = p;
  p->down = curline->down;
  p->up = curline;
  curline->down = p;
  strcpy(p->buf, cp);
  *cp = 0;
  curline = p;
  cp = curline->buf;
  copy_line(0, cury + 1, width(), end_y() - cury - 1, 0, cury + 2);
  clear_line(0, cury + 1, 0, 1);
  clear_line(curx, cury, 0, 1);
  curx = 0;
  down();
  if (curx < start_x)
    scroll_right(start_x - curx);
  print(cp, curx, cury);
}

void editor::delete_backward_char()
{
  if (cp > curline->buf)
    {
      backward();
      delete_char();
    }
}

void editor::insert_char(int w, Bool scroll, int tab)
{
  int xw;
  if (tab)
    {
      int nx = curx / (ecwidth() * 8) * (ecwidth() * 8) + ecwidth() * 8;
      xw = nx - curx;
    }
  else
    xw = (w == 1 ? ecwidth() : kcwidth());
  char* s = strchr(cp, 0);
  char* d = s + w;
  while (s >= cp)
    *d-- = *s--;
  if (scroll)
    if (curx + xw > end_x())
      scroll_left(curx + xw - end_x());
    else
      {
	clear_line(curx, cury, 0, 1);
	print(cp + w, curx + xw, cury, 0, width());
      }
}

void editor::store(int c)
{
  char buf[2];
  *cp = c;
  buf[0] = c;
  buf[1] = 0;
  print(buf, curx, cury);
}

void editor::store(int f, int m, int l)
{
  *cp = f;
  *(cp + 1) = m;
  *(cp + 2) = l;
  print(f, m, l, curx, cury);
}

void editor::delete_all()
{
  aline* n;
  for (aline* i = first_line; i != 0; i = n)
    {
      n = i->down;
      delete i;
    }
  curx = 0;
  cury = 0;
  start_x = 0;
  start_y = 0;
  curline = 0;
  first_line = curline;
  home_line = curline;
  last_line = curline;
  marked = False;
  state = 0;
  delete_cut();
}

void editor::append(char* str)
{
  if (last_line == 0)
    {
      curline = new aline;
      curline->up = 0;
      curline->down = 0;
      cp = curline->buf;
      *cp = 0;
      first_line = curline;
      home_line = curline;
      last_line = curline;
    }
  else
    {
      aline* n = new aline;
      last_line->down = n;
      n->up = last_line;
      n->down = 0;
      last_line = n;
    }
  strncpy(last_line->buf, str, maxbuf-1);
  last_line->buf[maxbuf-1] = 0;
}

void editor::replace(char* str)
{
  strncpy(curline->buf, str, maxbuf - 1);
  curline->buf[maxbuf-1] = 0;
}

void editor::list_all()
{
  clear_all();
  aline* i = home_line;
  for (int y = start_y; y <= end_y() && i != 0; y++, i = i->down)
    print(i->buf, 0, y, 0, width());
  flip_cursor();
}

void editor::first_value(char* buf)
{
  value_line = first_line;
  strcpy(buf, value_line->buf);
}

Bool editor::next_value(char* buf)
{
  if (value_line->down)
    {
      value_line = value_line->down;
      strcpy(buf, value_line->buf);
      return True;
    }
  return False;
}

void editor::set_start_end()
{
  if (cury < mark_fix_y || cury == mark_fix_y && curx < mark_fix_x)
    {
      set_mark_start();
      mark_end_line = mark_fix_line;
      mark_end_p = mark_fix_p;
      mark_end_x = mark_fix_x;
      mark_end_y = mark_fix_y;
    }
  else
    {
      set_mark_end();
      mark_start_line = mark_fix_line;
      mark_start_p = mark_fix_p;
      mark_start_x = mark_fix_x;
      mark_start_y = mark_fix_y;
    }
}

void editor::mark_forward()
{
  int ox = curx;
  int oy = cury;
  if (marked)
    {
      forward();
      set_start_end();
    }
  else
    {
      set_mark_start();
      set_mark_fix();
      forward();
      set_mark_end();
      marked = True;
    }
  if (cury == oy)
    reverse_line(ox, cury, curx - ox, 1);
  else
    {
      reverse_line(ox, oy, end_x() - ox, 1);
      reverse_line(0, cury, curx - 0, 1);
    }
}

void editor::mark_backward()
{
  int ox = curx;
  int oy = cury;
  if (marked)
    {
      backward();
      set_start_end();
    }
  else
    {
      set_mark_end();
      set_mark_fix();
      backward();
      set_mark_start();
      marked = True;
    }
  if (cury == oy)
    reverse_line(curx, cury, ox - curx, 1);
  else
    reverse_line(curx, cury, end_x() - curx, 1);
}

void editor::mark_beginning_of_line()
{
  int ox = curx;
  if (marked)
    {
      beginning_of_line();
      set_start_end();
    }
  else
    {
      set_mark_end();
      set_mark_fix();
      beginning_of_line();
      set_mark_start();
      marked = True;
    }
  reverse_line(curx, cury, ox - curx, 1);
}

void editor::mark_end_of_line()
{
  int ox = curx;
  if (marked)
    {
      end_of_line();
      set_start_end();
    }
  else
    {
      set_mark_start();
      set_mark_fix();
      end_of_line();
      set_mark_end();
      marked = True;
    }
  reverse_line(ox, cury, curx - ox, 1);
}

void editor::mark_previous_line()
{
  int ox = curx;
  int oy = cury;
  if (marked)
    {
      previous_line();
      set_start_end();
    }
  else
    {
      set_mark_end();
      set_mark_fix();
      previous_line();
      set_mark_start();
      marked = True;
    }
  if (cury != oy)
    {
      reverse_line(curx, cury, end_x() - curx, 1);
      reverse_line(0, oy, ox, 1);
    }
}

void editor::mark_next_line()
{
  int ox = curx;
  int oy = cury;
  if (marked)
    {
      next_line();
      set_start_end();
    }
  else
    {
      set_mark_start();
      set_mark_fix();
      next_line();
      set_mark_end();
      marked = True;
    }
  if (cury != oy)
    {
      reverse_line(ox, oy, end_x() - ox, 1);
      reverse_line(0, cury, curx, 1);
    }
}

void editor::delete_mark()
{
  if (marked)
    {
      if (mark_start_y == mark_end_y)
	reverse_line(mark_start_x, mark_start_y,
		     mark_end_x - mark_start_x, 1);
      else
	{
	  reverse_line(mark_start_x, mark_start_y, end_x() - mark_start_x, 1);
	  reverse_line(0, mark_start_y + 1,
		       end_x(), mark_end_y - mark_start_y - 1);
	  reverse_line(0, mark_end_y, mark_end_x, 1);
	}
      marked = False;
    }
}

void editor::cut()
{
  if (marked)
    {
      if (mark_start_y != mark_end_y)
	{
	  delete_cut();
	  aline* n = new aline;
	  strcpy(n->buf, mark_start_p);
	  n->up = 0;
	  n->down = mark_start_line->down;
	  mark_start_line->down->up = n;
	  strncpy(mark_start_p, mark_end_p,
		  maxbuf - (mark_start_p - mark_start_line->buf) - 1);
	  mark_start_line->buf[maxbuf-1] = 0;
	  *mark_end_p = 0;
	  mark_start_line->down = mark_end_line->down;
	  if (mark_end_line->down != 0)
	    {
	      mark_end_line->down->up = mark_start_line;
	      mark_end_line->down = 0;
	    }
	  cut_start_line = n;
	  curx = mark_start_x;
	  cury = mark_start_y;
	  curline = mark_start_line;
	  cp = mark_start_p;
	  marked = False;
	  list_all();
	}
      else
	if (mark_start_x != mark_end_x)
	  {
	    flip_cursor();
	    delete_cut();
	    aline* n = new aline;
	    strncpy(n->buf, mark_start_p, mark_end_p - mark_start_p);
	    n->buf[mark_end_p-mark_start_p]=0;
	    n->up=0;
	    n->down=0;
	    strcpy(mark_start_p, mark_end_p);
	    cut_start_line = n;
	    curx = mark_start_x;
	    cury = mark_start_y;
	    curline = mark_start_line;
	    cp = mark_start_p;
	    marked = False;
	    clear_line(curx, cury, 0, 1);
	    print(cp, curx, cury, 0, width());
	    flip_cursor();
	  }
    }
}

void editor::copy()
{
  if (marked)
    {
      flip_cursor();
      delete_cut();
      delete_mark();
      if (mark_start_y != mark_end_y)
	{
	  cut_start_line = new aline;
	  strcpy(cut_start_line->buf, mark_start_p);
	  cut_start_line->up = 0;
	  aline* up = cut_start_line;
	  aline* i;
	  aline* n;
	  for (i = mark_start_line->down; i != mark_end_line; i = i->down)
	    {
	      n = new aline;
	      strcpy(n->buf, i->buf);
	      n->up = up;
	      up->down = n;
	      up = n;
	    }
	  n = new aline;
	  strncpy(n->buf, i->buf, mark_end_p - mark_end_line->buf);
	  n->buf[mark_end_p - mark_end_line->buf] = 0;
	  n->up = up;
	  n->down = 0;
	  up->down = n;
	}
      else
	if (mark_start_x != mark_end_x)
	  {
	    cut_start_line = new aline;
	    strncpy(cut_start_line->buf, mark_start_p,
		    mark_end_p - mark_start_p);
	    cut_start_line->buf[mark_end_p-mark_start_p] = 0;
	    cut_start_line->up = 0;
	    cut_start_line->down = 0;
	  }
      marked = False;
      flip_cursor();
    }
}

void editor::delete_cut()
{
  if (cut_start_line)
    {
      aline* n;
      for (aline* i = cut_start_line; i != 0; i = n)
	{
	  n = i->down;
	  delete i;
	}
      cut_start_line = 0;
    }
}

void editor::paste()
{
  if (marked)
    delete_mark();
  if (cut_start_line != 0)
    if (cut_start_line->down == 0)
      {
	flip_cursor();
	char buf[maxbuf];
	int len = strlen(cut_start_line->buf);
	strcpy(buf, cp);
	strncpy(cp, cut_start_line->buf, curline->buf + maxbuf - cp - 1);
	strncpy(cp + len, buf, curline->buf + maxbuf - cp - len - 1);
	curline->buf[maxbuf-1] = 0;
	clear_line(curx, cury, 0, 1);
	print(cp, curx, cury);
	flip_cursor();
      }
    else
      {
	char buf[maxbuf];
	strcpy(buf, cp);
	strncpy(cp, cut_start_line->buf, curline->buf+maxbuf-cp-1);
	aline* n;
	aline* up = curline;
	aline* down = curline->down;
	for (aline* i = cut_start_line->down; i != 0; i = i->down)
	  {
	    n = new aline;
	    strcpy(n->buf, i->buf);
	    n->up = up;
	    up->down = n;
	    up = n;
	  }
	int len = strlen(n->buf);
	strncpy(n->buf+len, buf, maxbuf-len-1);
	n->down = down;
	if (down != 0)
	  down->up = n;
	list_all();
      }
}

void editor::page_up()
{
  if (home_line->up)
    {
      int count = visible_line() - 1;
      while (count > 0 && home_line->up != 0)
	{
	  home_line = home_line ->up;
	  curline = curline->up;
	  start_y--;
	  cury--;
	  count--;
	}
      cp = curline->buf;
      find_position(curx, cp, curx);
      list_all();
      flip_cursor();
    }
}

void editor::page_down()
{
  if (curline->down)
    {
      int count = visible_line() - 1;
      while (count > 0 && curline->down != 0)
	{
	  home_line = home_line ->down;
	  curline = curline->down;
	  start_y++;
	  cury++;
	  count--;
	}
      cp = curline->buf;
      find_position(curx, cp, curx);
      list_all();
      flip_cursor();
    }
}
