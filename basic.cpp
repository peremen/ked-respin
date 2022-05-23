// Basic class implementation

#include <std.h>
#include <ctype.h>
#include "basic.h"
#include "macro.h"
#include "container.h"

static XrmDatabase rDB;

static int num_cmd_opt = 13;
static XrmOptionDescRec cmd_opt[] = {
    {"-background",	"*background",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-bd",		"*borderColor",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-bg",		"*background",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-bordercolor",	".borderColor",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-borderwidth",	".borderWidth",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-bw",		".borderWidth",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-display",	".display",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-fg",		"*foreground",	XrmoptionSepArg,  (caddr_t) NULL},
    {"-foreground",	"*foreground",	XrmoptionSepArg,  (caddr_t) NULL},
    {"+reverse",	"*reverseVideo",XrmoptionNoArg,   (caddr_t) "off"},
    {"-reverse",	"*reverseVideo",XrmoptionNoArg,   (caddr_t) "on"},
    {"+rv",		"*reverseVideo",XrmoptionNoArg,   (caddr_t) "off"},
    {"-rv",		"*reverseVideo",XrmoptionNoArg,   (caddr_t) "on"},
};

/*
 * This routine extracts the program name in case it was specified
 * with a directory name as well.
 */
static char *basename(char *fname)
{
    register char *pt, *out;

    if (fname == NULL)
      return(NULL);

    pt = rindex(fname, '/');
    out = fname;
    if (pt != NULL) {
        pt++;
        out = (char *)malloc(strlen(pt) + 1);
        (void)strcpy(out, pt);
    }

    return(out);
}

/*
 * This routine capitalizes the program name so it can be used as
 * as class name for looking up resources.
 */
static char *capprogname(char *pname)
{
    register char *npname, *ptr;

    if (pname == NULL)
      return(NULL);

    npname = (char *)strcpy((char *)malloc(strlen(pname)),pname);
    ptr = npname + 1;

    npname[0] = (islower(npname[0]) ? toupper(npname[0]) : npname[0]);
    while(ptr && *ptr) {
        *ptr = isupper(*ptr) ? tolower(*ptr) : *ptr;
        ptr++;
    }
    return(npname);
}

/*
 * The order in which the resource databases are merged is important.
 * The resource file in the app-defaults directory should come first,
 * then the server resources, then the .Xdefaults file in the users
 * home directory, then the app-defaults in the users home directory,
 * and finally the command line resources, which override any of the
 * others.
 */
static void mergeResourceDBs(Display *disp, char *classname)
{
    XrmDatabase homeDB, appDB, serverDB, homeclassDB;
    char filenamebuf[BUFSIZ];
    char *filename = &filenamebuf[0];
    char *homedir, *environ;

    /*
     * Build the home directory first.  This isn't really robust,
     * but it's easier than checking the password file and more
     * portable.
     */
    homedir = getenv("HOME");
    if (homedir == NULL) {
        homedir = (char *)malloc(2);
        sprintf(homedir, ".");            
    }

    /*
     * First, get the default resources from the application defaults
     * file.
     */
    sprintf(filename, "%s/%s", APPDEFAULTSDIR, classname);
    appDB = XrmGetFileDatabase(filename);
    (void) XrmMergeDatabases(appDB, &rDB);

    /*
     * Next, get the resources stored in the server already.
     */
    if (XResourceManagerString(disp) != NULL)
      serverDB = XrmGetStringDatabase(XResourceManagerString(disp));
    else {
        /*
         * If there was no resource string in the server, then look for
         * the ~/.Xdefaults file.
         */
        sprintf(filename, "%s/.Xdefaults", homedir);
        serverDB = XrmGetFileDatabase(filename);
    }
    (void) XrmMergeDatabases(serverDB, &rDB);

    /*
     * Now check to see if there is a host specific .Xdefaults file.
     */
    environ = getenv("XENVIRONMENT");
    if (environ == NULL) {
        int len;

        environ = filename;
        sprintf(filename, "%s/.Xdefaults-", homedir);
        len = strlen(filename);
        (void) gethostname(environ+len, BUFSIZ-len);
    }
    homeDB = XrmGetFileDatabase(environ);
    (void) XrmMergeDatabases(homeDB, &rDB);

    /*
     * Finally, check for a specific resource file that has the
     * class name as the file name in the users home directory.
     * e.g. ~/Ked
     */
    sprintf(filename, "%s/%s", homedir, classname);
    homeclassDB = XrmGetFileDatabase(filename);
    (void) XrmMergeDatabases(homeclassDB, &rDB);
}

/*
 * This routine gets the flag that tells us whether reverse video is
 * wanted.
 */
static int DoReverseVideo(char *progname, char *classname,
                          char *res_name, char *res_class)
{
    char str_type[50], str_name[128], str_class[128];
    XrmValue value;
    int flag = 0;

    sprintf(str_name, "%s.%s", progname, res_name);
    sprintf(str_class, "%s.%s", classname, res_class);
    if (XrmGetResource(rDB, (const char *)str_name, (const char *) str_class,
                       (char **)&str_type, &value) == True) {
        if (strncmp((char *)value.addr, "on", value.size) == 0)
          flag = 1;
    }

    return(flag);
}

/*
 * This routine gets the border width resource.  Not used because it doesn't
 * work too well on instances of the editor class.
 */
/*
static int GetBorderWidth(char *progname, char *classname,
                          char *res_name, char *res_class, int default_width)
{
    char str_type[50], str_name[128], str_class[128], tmp_str[128];
    XrmValue value;
    int bw = default_width;

    sprintf(str_name, "%s.%s", progname, res_name);
    sprintf(str_class, "%s.%s", classname, res_class);
    if (XrmGetResource(rDB, (const char *)str_name, (const char *)str_class,
                       (char **)&str_type, &value) == True) {
        (void)strncpy(tmp_str, (char *)value.addr, value.size);
        tmp_str[value.size] = '\0';
        if (value.size != 0)
          bw = atoi(tmp_str);
    }

    return(bw);
}
*/

/*
 * This routine gets specified colors from the resource database.
 */
static int GetColor(char *progname, char *classname,
                    char *res_name, char *res_class, char *msg,
                    Display *disp, Visual *visual, Colormap colormap,
                    int default_color)
{
    char str_type[50], str_name[128], str_class[128], tmp_str[128];
    XrmValue value;
    XColor color;
    int pix;

    sprintf(str_name, "%s.%s", progname, res_name);
    sprintf(str_class, "%s.%s", classname, res_class);
    if (XrmGetResource(rDB, (const char *)str_name, (const char *)str_class,
                       (char **)&str_type, &value) == True) {
        (void)strncpy(tmp_str, (char *)value.addr, value.size);
        tmp_str[value.size] = '\0';
        if (XParseColor(disp, colormap, tmp_str, &color) == 0) {
            fprintf(stderr, "%s: %s color specification %s invalid\n",
                    progname, msg, tmp_str);
            pix = default_color;
        } else {
            if (visual->c_class == StaticGray || visual->c_class == GrayScale)
              pix = default_color;
            else if (XAllocColor(disp, colormap, &color) == 0) {
                fprintf(stderr, "%s: couldn't allocate %s color\n",
                        progname, msg);
                pix = default_color;
            } else {
                pix = color.pixel;
            }
        }
    } else {
        pix = default_color;
    }

    return(pix);
}

basic::basic(container* parent, long m, char *display_name = 0,
	     Bool map = True, int border = 1, Bool popupflag = False,
             int *ac = 0, char **av = 0)
{
    char *progname, *classname, *dispname = 0;
    char str_name[256], str_class[256], str_type[50];
    int tmp;
    int screen_num;
    Visual *visual;
    Colormap colormap;
    XGCValues values;
    XrmDatabase cmdDB;
    XrmValue value;

    p = parent;
    if (parent == 0)
      {
          progname = basename(av[0]);
          classname = capprogname(progname);
          XrmInitialize();
          XrmParseCommand(&cmdDB, cmd_opt, num_cmd_opt,
                          progname, ac, av);
          sprintf(str_name, "%s.display", progname);
          sprintf(str_class, "%s.Display", classname);
          if (XrmGetResource(cmdDB, (const char *)str_name,
                             (const char *)str_class, (char **)&str_type,
                             &value) == True) {
              dispname = (char *)malloc(value.size + 1);
              (void)strcpy(dispname, (char *)value.addr);
          }

          d = XOpenDisplay(dispname);
          if (d == 0)
            {
                fprintf(stderr, "%s: Can't open display '%s'\n", progname,
                        dispname);
                exit(1);
            }
          screen_num = DefaultScreen(d);
          visual = DefaultVisual(d, screen_num);
          colormap = DefaultColormap(d, screen_num);
          mergeResourceDBs(d, classname);
          (void) XrmMergeDatabases(cmdDB, &rDB);

          /*
           * Fill in the basic fields with info.
           */
          fg = GetColor(progname, classname,
                        "foreground", "Foreground",
                        "foreground", d, visual,
                        colormap, Black);
          bg = GetColor(progname, classname,
                        "background", "Background",
                        "background", d, visual,
                        colormap, White);
          bc = GetColor(progname, classname,
                        "borderColor", "BorderColor",
                        "border", d, visual,
                        colormap, Black);
          /*
           * Don't worry about border width at the moment, because
           * it doesn't come out right on instances of the editor
           * class.
           */
/*          bw = GetBorderWidth(progname, classname,
                              "borderWidth", "BorderWidth", border); */
          reverse = DoReverseVideo(progname, classname,
                                   "reverseVideo", "ReverseVideo");
          values.foreground = fg;
          values.background = bg;

          if (values.foreground == values.background) {
              values.foreground = Black;
              values.background = White;
          }

          if (reverse == 1)
            {
                tmp = bg;
                values.foreground = bg;
                values.background = fg;
                bg = fg;
                fg = tmp;
                /*
                 * If the border color is the same as the background color,
                 * change it to be the forground color so we can see the
                 * borders of the windows.
                 */
                if (bc == bg)
                  bc = fg;
            }

          _gc = XCreateGC(d, Root, GCForeground|GCBackground, &values);

          /*
           * Only switch the foreground and background if reverse video
           * is wanted.
           */
          if (reverse == 1) {
              values.foreground = bg;
              values.background = fg;
          }
          values.function = GXxor;
          _igc = XCreateGC(display(), Root,
                           GCFunction|GCForeground|GCBackground,
                           &values);
          _kf = new Kfont;
          _kf->init(this);
      }
    else
      {
          d = parent->d;
          _gc = parent->_gc;
          _igc = parent->_igc;
          _kf = parent->_kf;
          bg = parent->bg;
          fg = parent->fg;
          bc = parent->bc;
          bw = border;
          p->add_child(this);
      }
    mask = m;
    mapping = map;
    pop = popupflag;
    handlers = 0;
}

void basic::create()
{
  XSetWindowAttributes wa;
  wa.background_pixel = bg;
  wa.border_pixel = bc;
  wa.event_mask = mask;
  w = XCreateWindow(d, p ? p->w : Root, _x, _y, width(), height(),
                    bw, Depth, InputOutput, CopyFromParent,
                    CWBackPixel|CWBorderPixel|CWEventMask, &wa);
  if (mapping)
    map();
}

void basic::add_event(int type, handler func, void* arg)
{
  handler_info *h = new handler_info;
  h->type = type;
  h->func = func;
  h->arg = arg;
  h->next = handlers;
  handlers = h;
}

Bool basic::handle(XEvent* event)
{
  if (event->xany.window == window())
    {
      for (handler_info *i = handlers; i != 0; i = i->next)
	if (i->type == event->type)
	  (*(i->func))(event, i->arg);
      return True;
    }
  return False;
}

void basic::put_bitmap(char* bits, int x, int y, int w, int h,
		       Bool rev = False)
{
  XImage ximage;
  ximage.height = h;
  ximage.width = w;
  ximage.depth = 1;
  ximage.xoffset = 0;
  ximage.format = XYBitmap;
  ximage.data = bits;
  ximage.byte_order = LSBFirst;
  ximage.bitmap_unit = 8;
  ximage.bitmap_bit_order = LSBFirst;
  ximage.bitmap_pad = 8;
  ximage.bytes_per_line = (w+7)/8;
  XPutImage(display(), window(), rev ? igc() : gc(), &ximage, 0, 0,
	    x, y, w, h);
}

void basic::run()
{
  XEvent event;
  for (;;)
    {
      XNextEvent(d, &event);
      switch (event.type)
	{
	case ButtonPress:
	  button_press_time1 = button_press_time2;
	  button_press_time2 = event.xbutton.time;
	  break;
	case ButtonRelease:
	  if (event.xbutton.time < button_press_time1 + 500)
	    event.type = DoubleClick;
	}
      handle(&event);
    }
}
