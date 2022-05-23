//  Some macros
//
//  written by Song Jaekyung

#ifndef _macro_
#define _macro_

#include "Xlib.h"

#define	Scr	(DefaultScreen(display()))
#define	Root	(DefaultRootWindow(display()))
#define	White	(WhitePixel(display(), Scr))
#define	Black	(BlackPixel(display(), Scr))
#define	Depth	(DefaultDepth(display(), Scr))

#endif	// _macro_
