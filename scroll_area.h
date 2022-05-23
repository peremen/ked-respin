// Scrolled Area class

#ifndef _scroll_area_
#define _scroll_area_

#include "basic.h"

class scroll_area : public basic
{
  aline* first;
  aline* top;
  aline* end;
  int h_margin;
  int v_margin;
public:
  scroll_area(container*, int, int, int = 3, int = 0);
  void up(int);
  void down(int);
  void left(int);
  void right(int);
  void append(char*);
  void delete_all();
}
