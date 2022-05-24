// Input dialog class

#ifndef _input_dialog_
#define _input_dialog_

#include "container.h"
#include "button.h"
#include "editor.h"
#include "scroll.h"
#include "scrollbar.h"

typedef void (*func_type)(char*, editor*);

class input_dialog : public container
{
    button* ob;
    button* cb;
    editor* ib;
    editor* arg;
    func_type open_function;
public:
    input_dialog(container*);
    editor* input_box() { return ib; }
    void calc();
    void call_func();
    void set_func(func_type, editor*);
};

#endif // _input_dialog_
