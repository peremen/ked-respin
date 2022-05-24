// Main

#include <stdlib.h>
#include "button.h"
#include "menu.h"
#include "editor.h"
#include "ked.h"
#include "base.h"
#include "input_dialog.h"

char file_name[100] = "unnamed";

void quit(void* arg)
{
    exit(0);
}

submenu_info file_menu[] = {
    {"\212\202\200 \207\216\204\212\205\200", new_file},
    {"\201\201\200\215\207\200\214\211\200\201\225\200", load},
    {"\210\211\200\201\212\204", save},
    {"-", 0},
    {"\214\225\204\212\213\200", print},
    {"-", 0},
    {"\202\223\231", quit},
    {0, 0}
};

submenu_info edit_menu[] = {
    {"\215\201\200\206\223\200\201\225\200", cut},
    {"\210\211\201\212\201\200", copy},
    {"\210\216\231\214\225\200\201\225\200", paste},
    {0, 0}
};

menu_info ked_menu[] = {
    {"\207\216\204\212\205\200", file_menu},
    {"\222\207\204\215\225\221", edit_menu},
    {0, 0}
};

int main(int argc, char** argv)
{
    base* b = new base(&argc, argv);
    dialog* d = new dialog(b);
    input_dialog* i = new input_dialog(b);
    new menu(b, ked_menu);
    editor* e = new editor(b, 60, 30);
    file_menu[0].arg = e;
    file_menu[1].arg = d;
    file_menu[2].arg = i;
    file_menu[4].arg = e;
    edit_menu[0].arg = e;
    edit_menu[1].arg = e;
    edit_menu[2].arg = e;
    d->set_open_func(load_file, e);
    i->set_func(save_file, e);
    b->create();
    if (argc > 1)
        load_file(argv[1], e);
    b->run();
}

