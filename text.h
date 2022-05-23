// Text class

#ifndef _text_
#define _text_

#include "basic.h"
#include "Kfont.h"

extern GC _igc;
extern Kfont* _kf;

class text : public basic
{
public:
  text(container* parent, long, char* = 0, Bool = True, int = 1, Bool = False);
  int out(char* str, int x, int y);
  void out(char*, int, int, int, int);
  void out(char, char, char, int, int);
  int text_width(char*);
  int char_height();
  int char_ascent();
  int kcwidth() { return _kf->kcwidth(); }
  int ecwidth() { return _kf->ecwidth(); }
  GC igc() { return _igc; }
};

inline int text::out(char* str, int x, int y)
{
  return _kf->out(this, str, x, y);
}

inline void text::out(char* str, int x, int y, int sx, int ex)
{
  _kf->out(this, str, x, y, sx, ex);
}

inline void text::out(char f, char m, char l, int x, int y)
{
  _kf->out(this, f, m, l, x, y);
}

inline int text::text_width(char* str)
{
  return _kf->text_width(str);
}

inline int text::char_height()
{
  return _kf->char_height();
}

inline int text::char_ascent()
{
  return _kf->char_ascent();
}

#endif // _text_
