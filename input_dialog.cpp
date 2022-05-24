// Input Dialog class implementation

#include <X11/Xutil.h>
#include "input_dialog.h"
#include "button.h"
#include "margin.h"

static void open(XEvent* event, void* arg)
{
    input_dialog* d = (input_dialog *)arg;
    d->call_func();
}

static void cancel(XEvent* event, void* arg)
{
    ((input_dialog*)arg)->unmap();
}

static void expose(XEvent* event, void* arg)
{
    input_dialog* d = (input_dialog *)arg;
    XDrawRectangle(d->display(), d->window(), d->gc(),
            2, 2, d->width() - 5, d->height() - 5);
    XDrawRectangle(d->display(), d->window(), d->gc(),
            3, 3, d->width() - 7, d->height() - 7);
    XDrawRectangle(d->display(), d->window(), d->gc(),
            border_thick_m+thick_w+box_box_m,
            border_thick_m+thick_w+box_box_m,
            d->input_box()->width()+border_text_m * 2,
            d->input_box()->height() + border_text_m * 2+thin_w);
    XSetInputFocus(d->display(), d->input_box()->window(),
            RevertToParent, CurrentTime);
}

static void key_press(XEvent* event, void* arg)
{
    input_dialog *d = (input_dialog *)arg;
    char buf[2];
    KeySym keysym;
    if (XLookupString(&event->xkey, buf, 1, &keysym, 0))
        if (buf[0] == '\r')
            d->call_func();
}

input_dialog::input_dialog(container* parent) :
    container(parent, ExposureMask, 0, False, 1, True)
{
    ob = new button(this, "ˆ‰€Š„");
    ob->add_event(ButtonRelease, open, this);
    cb = new button(this, "\217\221\200\212\211\200");
    cb->add_event(ButtonRelease, cancel, this);
    ib = new editor(this, 20, 1, 0, True);
    ib->add_event(KeyPress, key_press, this);
    add_event(Expose, expose, this);
}

void input_dialog::calc()
{
    int button_w = ob->width() > cb->width() ? ob->width() : cb->width();
    const int ib_x = border_thick_m+thick_w+box_box_m+thin_w+border_text_m;
    ib->set_xy(ib_x, ib_x);
    const int ob_x = border_thick_m+thick_w+box_box_m;
    const int ob_y = ib->y()+ib->height()+border_text_m+thin_w+box_box_m;
    ob->set(ob_x, ob_y, button_w, ob->height());
    const int cb_x = ob->x()+ob->width()+box_box_m;
    cb->set(cb_x, ob_y, button_w, cb->height());
    int dialog_w = ib->x()+ib->width() + border_text_m+thin_w+box_box_m+thick_w+
        border_thick_m;
    int dialog_h = ob->y()+ob->height()+box_box_m+thick_w+border_thick_m;
    set(parent()->width() - dialog_w >> 1, parent()->height() - dialog_h >> 2,
            dialog_w, dialog_h);
}

void input_dialog::call_func()
{
    unmap();
    (*open_function)(ib->value(), arg);
}

void input_dialog::set_func(func_type func, editor* e)
{
    open_function = func;
    arg = e;
}
