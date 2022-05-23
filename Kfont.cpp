//  Korean font
//
//  written by Song Jaekyung

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Kfont.h"
#include "macro.h"
#include "basic.h"
#include "editor.h"

void Kfont::init(basic* t)
{
  FILE *fp = fopen(kfont, "r");
  if (fp == 0)
    {
      fp = fopen(kfont2, "r");
      if (fp == 0)
        {
          fprintf(stderr, "Can't open %s\n", kfont);
          exit(1);
        }
    }
  fseek(fp, 32, 0);	// skip header
  for (int i = 0; i < max; i++)
    fread(font[i], size, 1, fp);
  fclose(fp);
  int count;
  XListFonts(t->display(), efont, 1, &count);
  XID fid;
  if (!count)
    {
      XListFonts(t->display(), efont2, 1, &count);
      if (!count)
	{
	  fprintf(stderr, "Can'f find font %s\n", efont);
	  exit(1);
	}
      fid = XLoadFont(t->display(), efont2);
    }
  else
    fid = XLoadFont(t->display(), efont);
  fs = XQueryFont(t->display(), fid);
  XSetFont(t->display(), t->gc(), fid);
  ximage.height = height;
  ximage.width = width;
  ximage.depth = 1;
  ximage.xoffset = 0;
  ximage.format = XYBitmap;
  ximage.data = buf;
  ximage.byte_order = LSBFirst;
  ximage.bitmap_unit = 8;
  ximage.bitmap_bit_order = MSBFirst;
  ximage.bitmap_pad = 8;
  ximage.bytes_per_line = (width+7)/8;
  ewidth = XTextWidth(fs, " ", 1);
  descent = fs->max_bounds.descent;
  total_height = ascent + descent;
}

int Kfont::out(basic* t, char *str, int x, int y)
{
  int org_x = x;
  while (*str && x < t->width())
    {
      if (*str & 0x80)
	{
	  out(t, str[0], str[1], str[2], x, y);
	  x += width;
	  str += 3;
	}
      else if (*str == '\t')
	{
	  x = (x + ((editor*)t)->left_offset()) / (ewidth * 8) * (ewidth * 8)
	      + ewidth * 8 - ((editor*)t)->left_offset();
	  str++;
	}
      else
	{
	  char* p = str;
	  while ((*p & 0x80) == 0 && *p != '\t' && *p != 0)
	    p++;
	  XDrawString(t->display(), t->window(), t->gc(), x, y, str, p - str);
	  x += XTextWidth(fs, str, p - str);
	  str = p;
	}
    }
    return (x - org_x);
}

void Kfont::out(basic* t, char *str, int x, int y, int sx, int ex)
{
  while (*str)
    {
      if (*str & 0x80)
	{
	  if (x + width > sx)
	    out(t, str[0], str[1], str[2], x, y);
	  x += width;
	  str += 3;
	}
      else if (*str == '\t')
	{
	  x = (x + ((editor*)t)->left_offset()) / (ewidth * 8) * (ewidth * 8)
	      + ewidth * 8 - ((editor*)t)->left_offset();
	  str++;
	}
      else
	{
	  if (x + ewidth > sx)
	    {
	      char* p = str;
	      while ((*p & 0x80) == 0 && *p != '\t' && *p != 0)
		p++;
	      XDrawString(t->display(), t->window(), t->gc(),
			  x, y, str, p - str);
	      x += XTextWidth(fs, str, p - str);
	      str = p;
	    }
	  else
	    {
	      x += ewidth;
	      str++;
	    }
	}
      if (x > ex)
	return;
    }
}

void Kfont::clear_out(basic* t, char *str, int x, int y, int sx, int ex)
{
  while (*str)
    {
      if (*str & 0x80)
	{
	  if (x + width > sx)
	    {
	      XClearArea(t->display(), t->window(), x, y - ascent, width,
			 total_height, False);
	      out(t, str[0], str[1], str[2], x, y);
	    }
	  x += width;
	  str += 3;
	}
      else if (*str == '\t')
	{
	  int nx = (x + ((editor*)t)->left_offset())
	         / (ewidth * 8) * (ewidth * 8)
	         + ewidth * 8 - ((editor*)t)->left_offset();
	  if (nx >= sx)
	    XClearArea(t->display(), t->window(), x, y - ascent, nx - x,
		       total_height, False);
	  x = nx;
	  str++;
	}
      else
	{
	  if (x + ewidth > sx)
	    {
	      char* p = str;
	      while ((*p & 0x80) == 0 && *p != '\t' && *p != 0)
		p++;
	      int nx = x + XTextWidth(fs, str, p - str);
	      XClearArea(t->display(), t->window(), x, y - ascent,
			 nx - x, total_height, False);
	      XDrawString(t->display(), t->window(), t->gc(),
			  x, y, str, p - str);
	      x = nx;
	      str = p;
	    }
	  else
	    {
	      x += ewidth;
	      str++;
	    }
	}
      if (x > ex)
	return;
    }
}

inline void orcpy(char *d, char *s, int count)
{
  while (count-- > 0)
    *d++ |= *s++;
}

void Kfont::out(basic* t, char f, char m, char l, int x, int y)
{
  f -= 128;
  m -= 128;
  l -= 128;
  if (f < 0 || f > 19 || m < 0 || m > 21 || l < 0 || l > 27)
    return;
  memset(buf, 0, size);
  if (f)
    orcpy(buf, font[f + (l ? fcon_map2[m] : fcon_map1[m])], size);
  if (m)
    orcpy(buf, font[m + (l ? vow_map2[f] : vow_map1[f])], size);
  if (l)
    orcpy(buf, font[l + lcon_map[m]], size);
  XPutImage(t->display(), t->window(), t->gc(), &ximage, 0, 0,
	    x, y - ascent, width, height);
}

int Kfont::text_width(char *str)
{
  int w = 0;
  while (*str)
    if (*str & 0x80)
      {
	w += width;
	str += 3;
      }
    else
      {
	w += ewidth;
	str++;
      }
  return w;
}
