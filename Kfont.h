// Korean font
//
// written by Song Jaekyung

#ifndef _Kfont_
#define _Kfont_

#include <X11/Xlib.h>

class basic;

class Kfont {
    const int width = 16;
    const int height = 16;
    const int ascent = 14;
    const char *kfont = "/usr/local/fonts/hangul.sft";
    const char *kfont2 = "hangul.sft";
    const char *efont = "*-courier-bold-r-*--17-*";
    const char *efont2 = "*-courier-bold-r-*--14-*";
    const int max = 1193;
    const int size = ((width + 7) / 8) * height;
    int ewidth;
    int total_height;
    int descent;
    int fcon_map1[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 92, 92, 92, 30,
		       61, 123, 123, 123, 61, 30, 92, -1};
    int fcon_map2[] = {0, 154, 154, 154, 154, 154, 154, 154, 154, 185, 247,
                       247, 247, 185, 216, 278, 278, 278, 216, 216, 247, 154};
    int vow_map1[] = {309, 309, 309, 338, 338, 338, 338, 338, 338, 338, 309,
		      309, 338, 309, 309, 309, 309, 338, 338, 338};
    int vow_map2[] = {0, 367, 367, 396, 396, 396, 396, 396, 396, 396, 367, 367,
		      396, 367, 367, 367, 367, 396, 396, 396};
    int lcon_map[] = {0, 425, 487, 425, 487, 456, 487, 456, 487, 518, 425, 487,
		      487, 518, 518, 456, 487, 487, 518, 518, 487, 487};
    char font[max][size];
    char buf[size];
    XImage ximage;
    XFontStruct* fs;
public:
    void init(basic*);
    int out(basic*, char *, int, int);
    void out(basic*, char*, int, int, int, int);
    void clear_out(basic*, char*, int, int, int, int);
    void out(basic*, char, char, char, int, int);
    int text_width(char *);
    int kcwidth() { return width; }
    int char_height() { return total_height; }
    int char_ascent() { return ascent; }
    int char_descent() { return descent; }
    int ecwidth() { return ewidth; }
};

#endif	// _Kfont_
