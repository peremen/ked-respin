// menu class

#include "menu.h"
#include "basic.h"

menu::menu(container* parent, menu_info* ami) :
       (parent, ExposureMask|ButtonPressMask|ButtonReleaseMask|
	ButtonMotionMask)
{
  mi = ami;
  set_wh(menu_width(parent, mi), char_height() + margin * 2);
}

void menu::create()
{
  for (menu_info* i = mi; i->str != 0; i++)
    i->sm->create();
  basic::create(); 
}

int menu::menu_width(container* parent, menu_info *m)
{
  int x = ecwidth();
  int h = char_height() + margin * 2 + border_width();
  while (m->str != 0)
    {
      m->startx = x;
      m->sm = new submenu(parent, m->smi, x + border_width(), h);
      x += ecwidth() * 2 + text_width(m->str);
      m->endx = x;
      m++;
    }
  return x;
}

Bool menu::handle(XEvent *event)
{
  if (event->xany.window == window())
    {
      switch (event->type)
	{
	case Expose:
	  expose(event);
	  break;
	case ButtonPress:
	  buttonpress(event);
	  break;
	case ButtonRelease:
	  buttonrelease(event);
	  break;
	case MotionNotify:
	  motion(event);
	}
      return True;
    }
  return False;
}

void menu::expose(XEvent *event)
{
  int x = 0;
  x += out(" ", x, char_ascent() + margin);
  for (menu_info *i = mi; i->str != 0; i++)
    {
      x += out(" ", x, char_ascent() + margin);
      x += out(i->str, x, char_ascent() + margin);
      x += out(" ", x, char_ascent() + margin);
    }
}

void menu::buttonpress(XEvent *event)
{
  if (!pressed_button)
    {
      for (menu_info* i = mi; i->str != 0; i++)
	if (event->xbutton.x > i->startx && event->xbutton.x < i->endx)
	  {
	    i->sm->reset();
	    active = i;
	    reverse(active);
	    pressed_button = event->xbutton.button;
	    return;
	  }
      active = 0;
    }
}

void menu::buttonrelease(XEvent *event)
{
  if (event->xbutton.button == pressed_button)
    {
      if (active)
	{
	  active->sm->call();
	  reverse(active);
	  active = 0;
	}
      pressed_button = 0;
    }
}

void menu::motion(XEvent* event)
{
  if (event->xmotion.y < height())
    {
      for (menu_info* i = mi; i->str != 0; i++)
	if (event->xmotion.x > i->startx && event->xmotion.x < i->endx)
	  {
	    if (active != i)
	      {
		if (active)
		  {
		    active->sm->unmap();
		    reverse(active);
		  }
		i->sm->reset();
		active = i;
		reverse(active);
	      }
	    return;
	  }
      if (active)
	{
	  active->sm->unmap();
	  reverse(active);
	  active = 0;
	}
    }
  else
    if (active)
      {
	event->xany.window = active->sm->window();
	event->xmotion.x -= active->startx;
	event->xmotion.y -= height();
	active->sm->handle(event);
      }
}

void menu::reverse(menu_info* i)
{
  XFillRectangle(display(), window(), igc(), i->startx, 0,
		 i->endx - i->startx, height());
}
    
submenu::submenu(container* parent, submenu_info* asmi, int x, int y) :
       (parent, ExposureMask, 0, False, 0, True)
{
  smi = asmi;
  set(x, y, submenu_width(asmi), submenu_height(asmi));
}

void submenu::reset()
{
  active = 0;
  map();
}

void submenu::call()
{
  unmap();
  if (active)
    (*active->func)(active->arg);
}

Bool submenu::handle(XEvent *event)
{
  if (event->xany.window == window())
    {
      switch (event->type)
	{
	case Expose:
	  expose(event);
	  break;
	case MotionNotify:
	  motion(event);
	}
      return True;
    }
  return False;
}

void submenu::expose(XEvent *event)
{
  int y = 1 + char_ascent() + margin;
  XSetLineAttributes(display(), gc(), 0, LineOnOffDash, CapButt, JoinMiter);
  XSetDashes(display(), gc(), 0, "\01\02", 2);
  for (submenu_info* i = smi; i->str != 0; i++)
    {
      if (*i->str == '-')
	{
	  int ty = y - char_ascent();
	  XDrawLine(display(), window(), gc(), 1, ty, width() - 1, ty);
	  y += margin * 2 + 1;
	}
      else
	{
	  out(i->str, ecwidth(), y);
	  y += char_height() + margin * 2;
	}
    }
  XSetLineAttributes(display(), gc(), 0, LineSolid, CapButt, JoinMiter);
  XDrawRectangle(display(), window(), gc(), 0, 0, width() - 2, height() - 2);
  XDrawLine(display(), window(), gc(),
	    1, height() - 1, width() - 1, height() - 1);
  XDrawLine(display(), window(), gc(),
	    width() - 1, 1, width() - 1, height() - 1);
}

void submenu::motion(XEvent *event)
{
  int y = 1;
  if (event->xmotion.x > 0 && event->xmotion.x < width())
    for (submenu_info* i = smi; i->str != 0; i++)
      {
	int h;
	if (*i->str == '-')
	  h = margin * 2 + 1;
	else
	  h = char_height() + margin * 2;
	if (event->xmotion.y > y && event->xmotion.y < y + h)
	  {
	    if (active == i)
	      return;
	    if (active)
	      XFillRectangle(display(), window(), igc(), 1, oldy,
			     width() - 3, oldh);
	    if (*i->str != '-')
	      {
		XFillRectangle(display(), window(), igc(),
			       1, y, width() - 3, h);
		active = i;
		oldy = y;
		oldh = h;
	      }
	    else
	      active = 0;
	    return;
	  }
	else
	  y += h;
      }
  if (active)
    {
      XFillRectangle(display(), window(), igc(), 1, oldy,
		     width() - 3, char_height() + margin * 2);
      active = 0;
    }
}

int submenu::submenu_width(submenu_info* smi)
{
  int max = 1;
  while (smi->str != 0)
    {
      int w = text_width(smi->str) + ecwidth() * 2 + 3;
      if (w > max)
	max = w;
      smi++;
    }
  return max;
}

int submenu::submenu_height(submenu_info* smi)
{
  int total = 0;
  while (smi->str != 0)
    {
      if (*smi->str == '-')
	total += margin * 2 + 1;
      else
	total += char_height() + margin * 2;
      smi++;
    }
  return total + 3;
}
