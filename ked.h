// Korean Editor Header

#ifndef _ked_
#define _ked_

#include "dialog.h"

extern char file_name[];

extern void new_file(void*);
extern void load(void*);
extern void load_file(char*, editor*);
extern void save(void*);
extern void save_file(char*, editor*);
extern void cut(void*);
extern void copy(void*);
extern void paste(void*);
extern void print(void*);

#endif // _ked_
