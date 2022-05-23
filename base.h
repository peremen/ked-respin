// Base class

#ifndef _base_
#define _base_

#include "container.h"

class base : public container
{
  int sargc;
  char** sargv;
//  char* display_name(int, char**);
public:
  base(int *, char**);
  void create();
};

#endif // _base_
