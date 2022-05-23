// Text class implementation

#include <std.h>
#include "text.h"
#include "macro.h"

GC _igc = 0;
Kfont* _kf;

text::text(container* parent, long m, char* name = 0, Bool map = True,
	   int bw = 1, Bool pop = False) :
       (parent, m, name, map, bw, pop)
{
  if (_igc == 0)
    {
      XGCValues values;
      if (White)
	{
	  values.foreground = White;
	  values.background = Black;
	}
      else
	{
	  values.foreground = Black;
	  values.background = White;
	}
      values.function = GXxor;
      _igc = XCreateGC(display(), Root, GCFunction|GCForeground|GCBackground,
		       &values);
      _kf = new Kfont;
      _kf->init(this);
    }
}
