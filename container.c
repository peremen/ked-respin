// Container class implementation

#include "container.h"

container::container(container* parent, long mask, char* name = 0,
		     Bool map = True, int bw = 1, Bool pop = False,
                     int *ac = 0, char **av = 0) :
       (parent, mask, name, map, bw, pop, ac, av)
{
  children = 0;
}

void container::calc()
{
  int max_w = 0;
  int max_h = 0;
  for (basic_list* i = children; i != 0; i = i->next)
    if (!i->b->popup() && i->b->width() + i->b->border_width() * 2 > max_w)
      max_w = i->b->width() + i->b->border_width() * 2;
  for (i = children; i != 0; i = i->next)
    if (!i->b->popup())
      {
	i->b->set(0, max_h, max_w - i->b->border_width() * 2, i->b->height());
	max_h += i->b->height() + i->b->border_width();
      }
  set_wh(max_w, max_h + border_width());
}

void container::create()
{
  calc();
  basic::create();
  for (basic_list* i = children; i != 0; i = i->next)
    i->b->create();
}

void container::add_child(basic* child)
{
  basic_list* b = new basic_list;
  b->b = child;
  b->next = 0;
  if (children == 0)
    children = b;
  else
    {
      for (basic_list* i = children; i->next != 0; i = i->next)
	;
      i->next = b;
    }
}

void container::set_focus_window(Window w)
{
  focus_window = w;
}

Bool container::handle(XEvent* event)
{
  for (basic_list* i = children; i != 0; i = i->next)
    if (i->b->handle(event))
      return True;
  if (event->xfocus.window == window() && event->type == FocusIn)
    XSetInputFocus(display(), focus_window, RevertToPointerRoot, CurrentTime);
  return basic::handle(event);
}
