// Button class

#ifndef _button_
#define _button_

#include "basic.h"

class button : public basic
{
    const int hm = 11;
    const int vm = 5;
    char* label;
    int pressed = 0;
    XPoint points[9];
    XPoint fill_points[9];
    void reverse_box();
public:
    button(char*);
    button(container*, char*);
    Bool handle(XEvent*);
    void create();
};

#endif // _button_
