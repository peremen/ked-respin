// Dialog class

#ifndef _dialog_
#define _dialog_

#include "container.h"
#include "button.h"
#include "editor.h"
#include "scroll.h"
#include "scrollbar.h"

typedef void (*open_func_type)(char*, editor*);

class dialog : public container
{
  button* ob;
  button* cb;
  editor* ib;
  editor* arg;
  scroll* sb;
  scrollbar* sbb;
  open_func_type open_function;
public:
  dialog(container*);
  editor* input_box() { return ib; }
  scroll* select_box() { return sb; }
  void calc();
  void call_open_func();
  void set_open_func(open_func_type, editor*);
};

#endif // _dialog_
