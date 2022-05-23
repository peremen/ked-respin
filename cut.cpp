// cut & paste
//
// written by Song Jaekyung

#include "ked.h"

void cut(void* arg)
{
  editor* e = (editor *)arg;
  e->cut();
}

void copy(void* arg)
{
  editor* e = (editor *)arg;
  e->copy();
}

void paste(void* arg)
{
  editor* e = (editor *)arg;
  e->paste();
}
