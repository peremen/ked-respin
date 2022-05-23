// Button class implementation

#include "button.h"
#include "round1"
#include "round2"
#include "round3"
#include "round4"
#include "round1_rev"
#include "round2_rev"
#include "round3_rev"
#include "round4_rev"

button::button(char* l) : (0, ExposureMask|ButtonPressMask|ButtonReleaseMask|
			   EnterWindowMask|LeaveWindowMask, 0, True, 0)
{
  label = l;
  set_width_height(text_width(label) + hm * 2, char_height() + vm * 2);
}

button::button(container* parent, char* l) :
       (parent, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|
	LeaveWindowMask, 0, True, 0)
{
  label = l;
  set_width_height(text_width(label) + hm * 2, char_height() + vm * 2);
}

void button::draw_round(char* bits, int x, int y)
{
  XImage ximage;
  ximage.height = round1_height;
  ximage.width = round1_width;
  ximage.depth = 1;
  ximage.xoffset = 0;
  ximage.format = XYBitmap;
  ximage.data = bits;
  ximage.byte_order = LSBFirst;
  ximage.bitmap_unit = 8;
  ximage.bitmap_bit_order = LSBFirst;
  ximage.bitmap_pad = 8;
  ximage.bytes_per_line = (round1_width+7)/8;
  XPutImage(display(), window(), gc(), &ximage, 0, 0,
	    x, y, round1_width, round1_height);
}

void button::reverse_round(char* bits, int x, int y)
{
  XImage ximage;
  ximage.height = round1_height;
  ximage.width = round1_width;
  ximage.depth = 1;
  ximage.xoffset = 0;
  ximage.format = XYBitmap;
  ximage.data = bits;
  ximage.byte_order = LSBFirst;
  ximage.bitmap_unit = 8;
  ximage.bitmap_bit_order = LSBFirst;
  ximage.bitmap_pad = 8;
  ximage.bytes_per_line = (round1_width+7)/8;
  XPutImage(display(), window(), igc(), &ximage, 0, 0,
	    x, y, round1_width, round1_height);
}

void button::draw_round_box()
{
  draw_round(round1_bits, 0, 0);
  draw_round(round2_bits, width() - round1_width, 0);
  draw_round(round3_bits, width() - round1_width, height() - round1_height);
  draw_round(round4_bits, 0, height() - round1_height);
  XDrawLine(display(), window(), gc(), 0, round1_height,
	    0, height() - round1_height);
  XDrawLine(display(), window(), gc(), width() - 1, round1_height,
	    width() - 1, height() - round1_height);
  XDrawLine(display(), window(), gc(), round1_width, 0,
	    width() - round1_width, 0);
  XDrawLine(display(), window(), gc(), round1_width, height() - 1,
	    width() - round1_height, height() - 1);
}

void button::reverse_round_box()
{
  reverse_round(round1_rev_bits, 0, 0);
  reverse_round(round2_rev_bits, width() - round1_width, 0);
  reverse_round(round3_rev_bits, width() - round1_width,
	     height() - round1_height);
  reverse_round(round4_rev_bits, 0, height() - round1_height);
  XFillRectangle(display(), window(), igc(), round1_width, 1,
		 width() - round1_width * 2, height() - 2);
  XFillRectangle(display(), window(), igc(), 1, round1_height,
		 round1_width - 1, height() - round1_height * 2);
  XFillRectangle(display(), window(), igc(), width() - round1_width,
		 round1_height, round1_width - 1,
		 height() - round1_height * 2);
}

Bool button::handle(XEvent* event)
{
  if (event->xany.window == window())
    {
      switch (event->type)
	{
	case Expose:
	  out(label, width() - text_width(label) >> 1, char_ascent() + vm);
	  draw_round_box();
	  break;
	case ButtonPress:
	  if (!pressed)
	    {
	      reverse_round_box();
	      pressed = event->xbutton.button;
	    }
	  break;
	case ButtonRelease:
	  if (event->xbutton.button == pressed)
	    {
	      reverse_round_box();
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
	    reverse_round_box();
	  break;
	case LeaveNotify:
	  if (pressed)
	    {
	      reverse_round_box();
	      pressed = 0;
	    }
	}
      basic::handle(event);
      return True;
    }
  return False;
}
