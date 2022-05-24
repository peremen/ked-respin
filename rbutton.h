// Button class

#ifndef _button_
#define _button_

#include "text.h"
#include "container.h"

class button : public text
{
    const int hm = 11;
    const int vm = 5;
    char* label;
    int pressed = 0;
    void draw_round(char*, int, int);
    void reverse_round(char*, int, int);
    void draw_round_box();
    void reverse_round_box();
public:
    button(char*);
    button(container*, char*);
    Bool handle(XEvent*);
};

#endif // _button_
