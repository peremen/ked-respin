// Container class

#ifndef _container_
#define _container_

#include "basic.h"

struct basic_list
{
    basic* b;
    basic_list* next;
};

class container : public basic
{
    basic_list* children = 0;
    Window focus_window;
public:
    container(container*, long, char* = 0, Bool = True, int = 1, Bool = False,
            int* = 0, char** = 0);
    void add_child(basic*);
    void create();
    void set_focus_window(Window);
    Bool handle(XEvent*);
    virtual void calc();
};

#endif // _container_
