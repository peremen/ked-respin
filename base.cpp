// Base class implementation

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include "base.h"

base::base(int *argc, char** argv) : container(0, 0, 0, True, 1, False, argc, argv)
{
  sargc = *argc;
  sargv = argv;
}

/*
 * This routine extracts the program name in case it was specified
 * with a directory name as well.
 */
static char *priv_basename(char *fname)
{
    char *pt = rindex(fname, '/');
    char *out;

    out = fname;
    if (pt != NULL) {
        pt++;
        out = (char *)malloc(strlen(pt) + 1);
        (void)strcpy(out, pt);
    }

    return(out);
}

void base::create()
{
  XSizeHints sizehints;
  XWMHints wmhints;
  XClassHint classhints;
  char *pname = priv_basename(sargv[0]);
  XTextProperty window_name, icon_name;

  container::create();
  sizehints.flags = PSize | PPosition;
  sizehints.width = width();
  sizehints.height = height();
  sizehints.x = x();
  sizehints.y = y();

  wmhints.initial_state = NormalState;
  wmhints.input = True;
  wmhints.flags = InputHint | StateHint;

  if (XStringListToTextProperty(&pname, 1, &window_name) == 0) {
      fprintf(stderr, "%s: couldn't allocate window name\n", pname);
      exit(-1);
  }

  if (XStringListToTextProperty(&pname, 1, &icon_name) == 0) {
      fprintf(stderr, "%s: couldn't allocate icon name\n", pname);
      exit(-1);
  }

  classhints.res_name = pname;
  classhints.res_class = "Basicwin";

#ifdef X11R3
  sizehints.flags = PSize;
  XSetStandardProperties(display(), window(), pname, pname, None, sargv,
			 sargc, &sizehints);
#else /* !X11R3 */
  XSetWMProperties(display(), window(), &window_name, &icon_name,
                   sargv, sargc, &sizehints, &wmhints, &classhints);
#endif /* !X11R3 */

}

/*
char* base::display_name(int& argc, char** argv)
{
  for (int i = 0; i < argc; i++)
    if (strcmp(argv[i], "-display") == 0)
      if (argc > i + 1)
	{
	  char* DisplayName = argv[i + 1];
	  argc -= 2;
	  while (i < argc)
	    {
	      argv[i] = argv[i + 2];
	      i++;
	    }	  
	  return DisplayName;
	}
  return 0;
}
*/
