// Scroll class implementation

#include <stdio.h>
#include <string.h>
#include "scroll.h"
#include "margin.h"

static void expose(XEvent* event, void* arg)
{
    scroll* s = (scroll *)arg;
    s->list();
}

static void button_press(XEvent* event, void* arg)
{
    scroll* s = (scroll *)arg;
    int i = event->xbutton.y / s->char_height();
    s->select(i);
}

scroll::scroll(container* parent, int w, int h) :
    basic(parent, ExposureMask|ButtonPressMask|ButtonReleaseMask)
{
    set_wh(w * kcwidth(), h * char_height());
    vs = h;
    list_files();
    add_event(Expose, expose, this);
    add_event(ButtonPress, button_press, this);
}

void scroll::list_files()
{
    delete_all();
    FILE* fp = popen("ls -a", "r");
    char buf[81];
    while (fgets(buf, 80, fp) != 0)
    {
        char* n = strchr(buf, '\n');
        if (n)
            *n = 0;
        add_line(buf);
    }
    pclose(fp);
}

void scroll::delete_all()
{
    aline* n;
    for (aline* i = fl; i != 0; i = n)
    {
        n = i->down;
        delete i;
    }
    fl = 0;
    sl = 0;
    last_line = 0;
    select_line = 0;
    ts = 0;
}

void scroll::add_line(char* str)
{
    aline* n = new aline;
    if (last_line)
        last_line->down = n;
    n->up = last_line;
    n->down = 0;
    strcpy(n->buf, str);
    last_line = n;
    if (fl == 0)
    {
        fl = n;
        sp = 0;
    }
    ts++;
}

void scroll::list()
{
    XClearWindow(display(), window());
    select_y = -1;
    int y = char_ascent();
    for (aline* i = fl; i != 0 && y < height(); i = i->down)
    {
        out(i->buf, border_text_m, y);
        if (i == select_line)
        {
            select_y = y-char_ascent();
            XFillRectangle(display(), window(), igc(),
                    0, select_y, width(), char_height());
        }
        sl = i;
        y += char_height();
    }
    sy = y - char_height();
}

void scroll::up()
{
    if (fl->up)
    {
        fl = fl->up;
        sp--;
        XCopyArea(display(), window(), window(), gc(), 0, 0,
                width(), height() - char_height(), 0, char_height());
        sl = sl->up;
        XClearArea(display(), window(), 0, 0, 0, char_height(), False);
        out(fl->buf, border_text_m, char_ascent());
        if (fl == select_line)
        {
            select_y = 0;
            XFillRectangle(display(), window(), igc(),
                    0, select_y, width(), char_height());
        }
        if (select_line)
            select_y += char_height();
    }
}

void scroll::down()
{
    if (sl->down)
    {
        fl = fl->down;
        sp++;
        XCopyArea(display(), window(), window(), gc(), 0, char_height(),
                width(), height() - char_height(), 0, 0);
        sl = sl->down;
        XClearArea(display(), window(), 0, sy-char_ascent(), 0, 0, False);
        out(sl->buf, border_text_m, sy);
        if (sl == select_line)
        {
            select_y = sy-char_ascent();
            XFillRectangle(display(), window(), igc(),
                    0, select_y, width(), char_height());
        }
        if (select_line)
            select_y -= char_height();
    }
}

void scroll::set_start(int l)
{
    if (l > sp)
    {
        while (l > sp && sl->down)
        {
            fl = fl->down;
            sp++;
            sl = sl->down;
        }
        list();
    }
    else
    {
        while (l < sp && fl->up)
        {
            fl = fl->up;
            sp--;
            sl = sl->up;
        }
        list();
    }
}

void scroll::select(int i)
{
    if (select_line && select_y >= 0 && select_y < height())
        XFillRectangle(display(), window(), igc(),
                0, select_y, width(), char_height());
    select_line = fl;
    int y = 0;
    while (i-- > 0)
    {
        select_line = select_line->down;
        y += char_height();
    }
    select_y = y;
    XFillRectangle(display(), window(), igc(),
            0, select_y, width(), char_height());
}
