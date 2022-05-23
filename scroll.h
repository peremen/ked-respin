// Scroll class

#ifndef _scroll_
#define _scroll_

#include "basic.h"
#include "aline.h"

class scroll : public basic
{
  aline* fl = 0;
  aline* sl;
  aline* last_line;
  aline* select_line;
  int sy;
  int ts;
  int sp;
  int vs;
  int select_y;
public:
  scroll(container*, int, int);
  aline* first_line() { return fl; }
  aline* selection() { return select_line; }
  void list_files();
  void add_line(char*);
  void delete_all();
  void list();
  void up();
  void down();
  void set_start(int);
  void select(int);
  int total_size() { return ts; }
  int start_position() { return sp; }
  int visible_size() { return vs; }
};

#endif // _scroll_
