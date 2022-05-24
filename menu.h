// menu class

#ifndef _menu_
#define _menu_

#include "basic.h"

struct submenu_info
{
    char *str;
    void (*func)(void*);
    void *arg;
};

struct menu_info
{
    char *str;
    submenu_info *smi;
    int startx;
    int endx;
    class submenu *sm;
};

class menu : public basic
{
    menu_info* mi;
    menu_info* active;
    const int margin = 3;
    int pressed_button = 0;
    int menu_width(container*, menu_info*);
    void expose(XEvent*);
    void buttonpress(XEvent*);
    void buttonrelease(XEvent*);
    void motion(XEvent*);
    void reverse(menu_info*);
public:
    menu(container*, menu_info*);
    void create();
    Bool handle(XEvent*);
};

class submenu : public basic
{
    int oldy;
    int oldh;
    submenu_info *smi;
    submenu_info *active;
    const int margin = 3;
    int submenu_width(submenu_info*);
    int submenu_height(submenu_info*);
    void expose(XEvent*);
    void motion(XEvent*);
public:
    submenu(container*, submenu_info*, int, int);
    void reset();
    void call();
    Bool handle(XEvent*);
};

#endif // _menu_
