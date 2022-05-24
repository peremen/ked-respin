// Dialog class implementation

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <X11/Xutil.h>
#include "dialog.h"
#include "button.h"
#include "margin.h"

static void open(XEvent* event, void* arg)
{
    dialog* d = (dialog *)arg;
    d->call_open_func();
}

static void cancel(XEvent* event, void* arg)
{
    ((dialog*)arg)->unmap();
}

static void expose(XEvent* event, void* arg)
{
    dialog* d = (dialog *)arg;
    XDrawRectangle(d->display(), d->window(), d->gc(),
            2, 2, d->width() - 5, d->height() - 5);
    XDrawRectangle(d->display(), d->window(), d->gc(),
            3, 3, d->width() - 7, d->height() - 7);
    XDrawRectangle(d->display(), d->window(), d->gc(),
            border_thick_m+thick_w+box_box_m,
            border_thick_m+thick_w+box_box_m,
            d->input_box()->width()+border_text_m * 2,
            d->input_box()->height() + border_text_m * 2+thin_w);
}

static void key_press(XEvent* event, void* arg)
{
    dialog *d = (dialog *)arg;
    char buf[2];
    KeySym keysym;
    if (XLookupString(&event->xkey, buf, 1, &keysym, 0))
        if (buf[0] == '\r')
            d->call_open_func();
}

static void button_release(XEvent* event, void* arg)
{
    dialog* d = (dialog *)arg;
    d->input_box()->delete_all();
    d->input_box()->append(d->select_box()->selection()->buf);
    d->input_box()->list_all();
}

static void double_click(XEvent* event, void* arg)
{
    dialog* d = (dialog *)arg;
    d->call_open_func();
}

static void map_event(XEvent* event, void* arg)
{
    dialog* d = (dialog *)arg;
    d->select_box()->list_files();
    XSetInputFocus(d->display(), d->input_box()->window(),
            RevertToParent, CurrentTime);
    d->parent()->set_focus_window(d->window());
}

dialog::dialog(container* parent) :
    container(parent, ExposureMask|StructureNotifyMask, 0, False, 1, True)
{
    ob = new button(this, "\201\201\200\215\207\200\214\211\200\201\225\200");
    ob->add_event(ButtonRelease, open, this);
    cb = new button(this, "\217\221\200\212\211\200");
    cb->add_event(ButtonRelease, cancel, this);
    ib = new editor(this, 20, 1, 0, True);
    ib->add_event(KeyPress, key_press, this);
    sb = new scroll(this, 10, 10);
    sb->add_event(ButtonRelease, button_release, this);
    sb->add_event(DoubleClick, double_click, this);
    sbb = new scrollbar(sb);
    add_event(Expose, expose, this);
    add_event(MapNotify, map_event, this);
}

void dialog::calc()
{
    int button_w = ob->width() > cb->width() ? ob->width() : cb->width();
    const int ib_x = border_thick_m+thick_w+box_box_m+thin_w+border_text_m;
    ib->set_xy(ib_x, ib_x);
    sb->set_xy(border_thick_m+thick_w+box_box_m,
            ib->y()+ib->height()+border_text_m+thin_w+box_box_m);
    sbb->set_xy(sb->x()+sb->width(), sb->y());
    ob->set(sbb->x()+sbb->width()+box_box_m, sb->y(), button_w, ob->height());
    cb->set(sbb->x()+sbb->width()+box_box_m, ob->y()+ob->height()+box_box_m,
            button_w, ob->height());
    int dialog_w = ib->x()+ib->width()+border_text_m+thin_w+box_box_m+thick_w+
        border_thick_m;
    int dialog_h = sb->y()+sb->height()+box_box_m+thick_w+border_thick_m;
    set(parent()->width() - dialog_w >> 1, parent()->height() - dialog_h >> 2,
            dialog_w, dialog_h);
}

void dialog::call_open_func()
{
    struct stat buf;
    if (stat(ib->value(), &buf) == 0)
    {
        if (buf.st_mode & S_IFDIR)
        {
            if (chdir(ib->value()) == 0)
            {
                sb->list_files();
                sb->list();
                sbb->calc_draw_slider();
            }
        }
        else
        {
            unmap();
            (*open_function)(ib->value(), arg);
        }
    }
    else
        unmap();
}

void dialog::set_open_func(open_func_type func, editor* e)
{
    open_function = func;
    arg = e;
}
