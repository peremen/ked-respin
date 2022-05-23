// Button class implementation

#include "button.h"

button::button(char* l) : basic(0, ExposureMask|ButtonPressMask|ButtonReleaseMask|
			   EnterWindowMask|LeaveWindowMask, 0, True, 1)
{
  label = l;
  set_wh(text_width(label) + hm * 2, char_height() + vm * 2);
}

button::button(container* parent, char* l) :
       basic(parent, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|
	LeaveWindowMask, 0, True, 0)
{
  label = l;
  set_wh(text_width(label) + hm * 2, char_height() + vm * 2);
}

void button::create()
{
  basic::create();
  points[0].x = 4; points[0].y = 0;
  points[1].x = width() - 5; points[1].y = 0;
  points[2].x = width() - 1; points[2].y = 4;
  points[3].x = width() - 1; points[3].y = height() - 5;
  points[4].x = width() - 5; points[4].y = height() - 1;
  points[5].x = 4; points[5].y = height() - 1;
  points[6].x = 0; points[6].y = height() - 5;
  points[7].x = 0; points[7].y = 4;
  points[8].x = 4; points[8].y = 0;
  fill_points[0].x = 4; fill_points[0].y = 0;
  fill_points[1].x = width() - 4; fill_points[1].y = 0;
  fill_points[2].x = width() - 0; fill_points[2].y = 4;
  fill_points[3].x = width() - 0; fill_points[3].y = height() - 5;
  fill_points[4].x = width() - 5; fill_points[4].y = height() - 0;
  fill_points[5].x = 5; fill_points[5].y = height() - 0;
  fill_points[6].x = 0; fill_points[6].y = height() - 5;
  fill_points[7].x = 0; fill_points[7].y = 4;
  fill_points[8].x = 4; fill_points[8].y = 0;
}

void button::reverse_box()
{
  XFillPolygon(display(), window(), igc(), fill_points, 9,
	       Convex, CoordModeOrigin);
}

Bool button::handle(XEvent* event)
{
  if (event->xany.window == window())
    {
      switch (event->type)
	{
	case Expose:
	  XDrawLines(display(), window(), gc(), points, 9, CoordModeOrigin);
	  out(label, width() - text_width(label) >> 1, char_ascent() + vm);
	  break;
	case ButtonPress:
	  if (!pressed)
	    {
	      reverse_box();
	      pressed = event->xbutton.button;
	    }
	  break;
	case ButtonRelease:
	  if (event->xbutton.button == pressed)
	    {
	      reverse_box();
	      pressed = 0;
	    }
	  else
	    return True;
	  break;
	case EnterNotify:
	  pressed = 0;
	  switch (event->xcrossing.state)
	    {
	    case Button1Mask:
	      pressed = Button1;
	      break;
	    case Button2Mask:
	      pressed = Button2;
	      break;
	    case Button3Mask:
	      pressed = Button3;
	      break;
	    case Button4Mask:
	      pressed = Button4;
	      break;
	    case Button5Mask:
	      pressed = Button5;
	    }
	  if (pressed)
	    reverse_box();
	  break;
	case LeaveNotify:
	  if (pressed)
	    {
	      reverse_box();
	      pressed = 0;
	    }
	}
      basic::handle(event);
      return True;
    }
  return False;
}
