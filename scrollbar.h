// Scroll Bar class

#ifndef _scrollbar_
#define _scrollbar_

#include "basic.h"
#include "scroll.h"

class scrollbar : public basic
{
    GC sgc;
    int slider_y;
    int slider_size;
    int drag;
    int continue_job = 0;
    scroll* s;
    void expose(XEvent*);
    void button_press(XEvent*);
    void button_release(XEvent*);
    void motion(XEvent*);
public:
    scrollbar(scroll*);
    void create();
    void calc_draw_slider();
    void draw_slider(int, int);
    void set_drag(int);
    void drag_to(int);
    void timer();
    Bool handle(XEvent*);
    Bool in_slider_box(int);
};

#endif // _scrollbar_
