// Load

#include <string.h>
#include <stdio.h>
#include "dialog.h"
#include "ked.h"

const int Ascii = 0;
const int First = 1;
const int Middle = 2;
const int Last = 3;
const int Ctrl_N = 'N' - '@';
const int Ctrl_O = 'O' - '@';
static int state = Ascii;

void new_file(void* arg)
{
  editor* e = (editor *)arg;
  e->delete_all();
  e->append("");
  e->list_all();
  strcpy(file_name, "unnamed");
}

void load(void* arg)
{
  dialog* d = (dialog *)arg;
  d->input_box()->delete_all();
  d->input_box()->append(file_name);
  d->map();
}

int fcon(int c)
{
  switch (c)
    {
    case 'A':
      return 1 + 128;
    case 'B':
      return 2 + 128;
    case 'D':
      return 3 + 128;
    case 'G':
      return 4 + 128;
    case 'H':
      return 5 + 128;
    case 'I':
      return 6 + 128;
    case 'Q':
      return 7 + 128;
    case 'R':
      return 8 + 128;
    case 'S':
      return 9 + 128;
    case 'U':
      return 10 + 128;
    case 'V':
      return 11 + 128;
    case 'W':
      return 12 + 128;
    case 'X':
      return 13 + 128;
    case 'Y':
      return 14 + 128;
    case 'Z':
      return 15 + 128;
    case '[':
      return 16 + 128;
    case '\\':
      return 17 + 128;
    case ']':
      return 18 + 128;
    case '^':
      return 19 + 128;
    default:
      return 0;
    }
}

int vow(int c)
{
  switch (c)
    {
    case 'b':
      return 1 + 128;
    case 'c':
      return 2 + 128;
    case 'd':
      return 3 + 128;
    case 'e':
      return 4 + 128;
    case 'f':
      return 5 + 128;
    case 'g':
      return 6 + 128;
    case 'j':
      return 7 + 128;
    case 'k':
      return 8 + 128;
    case 'l':
      return 9 + 128;
    case 'm':
      return 10 + 128;
    case 'n':
      return 11 + 128;
    case 'o':
      return 12 + 128;
    case 'r':
      return 13 + 128;
    case 's':
      return 14 + 128;
    case 't':
      return 15 + 128;
    case 'u':
      return 16 + 128;
    case 'v':
      return 17 + 128;
    case 'w':
      return 18 + 128;
    case 'z':
      return 19 + 128;
    case '{':
      return 20 + 128;
    case '|':
      return 21 + 128;
    default:
      return 0;
    }
}

int comvow(int c, int nc)
{
  switch (c)
    {
    case 'l':
      switch (nc)
	{
	case 'b':
	  return 10 + 128;
	case 'c':
	  return 11 + 128;
	case '|':
	  return 12 + 128;
	default:
	  return 0;
	}
    case 's':
      switch (nc)
	{
	case 'f':
	  return 15 + 128;
	case 'g':
	  return 16 + 128;
	case '|':
	  return 17 + 128;
	default:
	  return 0;
	}
    case 'z':
      switch (nc)
	{
	case '|':
	  return 20 + 128;
	default:
	  return 0;
	}
    default:
      return 0;
    }
}

int lcon(int c)
{
  switch (c)
    {
    case 'A':
      return 1 + 128;
    case 'B':
      return 2 + 128;
    case 'C':
      return 3 + 128;
    case 'D':
      return 4 + 128;
    case 'E':
      return 5 + 128;
    case 'F':
      return 6 + 128;
    case 'G':
      return 7 + 128;
    case 'I':
      return 8 + 128;
    case 'J':
      return 9 + 128;
    case 'K':
      return 10 + 128;
    case 'L':
      return 11 + 128;
    case 'M':
      return 12 + 128;
    case 'N':
      return 13 + 128;
    case 'O':
      return 14 + 128;
    case 'P':
      return 15 + 128;
    case 'Q':
      return 16 + 128;
    case 'R':
      return 17 + 128;
    case 'S':
      return 18 + 128;
    case 'U':
      return 19 + 128;
    case 'V':
      return 20 + 128;
    case 'W':
      return 21 + 128;
    case 'X':
      return 22 + 128;
    case 'Z':
      return 23 + 128;
    case '[':
      return 24 + 128;
    case '\\':
      return 25 + 128;
    case ']':
      return 26 + 128;
    case '^':
      return 27 + 128;
    default:
      return 0;
    }
  return 0;
}

int comcon(int c, int nc)
{
  switch (c)
    {
    case 'A':
      switch (nc)
	{
	case 'U':
	  return 3 + 128;
	default:
	  return 0;
	}
    case 'D':
      switch (nc)
	{
	case 'X':
	  return 5 + 128;
	case '^':
	  return 6 + 128;
	default:
	  return 0;
	}
    case 'I':
      switch (nc)
	{
	case 'A':
	  return 9 + 128;
	case 'Q':
	  return 10 + 128;
	case 'R':
	  return 11 + 128;
	case 'U':
	  return 12 + 128;
	case '\\':
	  return 13 + 128;
	case ']':
	  return 14 + 128;
	case '^':
	  return 15 + 128;
	default:
	  return 0;
	}
    case 'R':
      switch (nc)
	{
	case 'U':
	  return 18 + 128;
	default:
	  return 0;
	}
    default:
      return 0;
    }
}

void convert_n_3(char* src, char* dst)
{
  int c;
  int r;
  while (c = *src++)
    {
      switch (state)
	{
	case Ascii:
	  switch (c)
	    {
	    case Ctrl_N:
	      state = First;
	      break;
	    default:
	      *dst++ = c;
	    }
	  break;
	case First:
	  if (r = fcon(c))
	    {
	      *dst++ = r;
	      state = Middle;
	    }
	  else if (r = vow(c))
	    {
	      *dst++ = 0x80;
	      *dst++ = r;
	      *dst++ = 0x80;
	    }
	  else if (r = lcon(c))
	    {
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = r;
	    }
	  else if (c == Ctrl_O)
	    state = Ascii;
	  else
	    *dst++ = c;
	  break;
	case Middle:
	  if (r = comvow(c, *src))
	    {
	      *dst++ = r;
	      state = Last;
	      src++;
	    }
	  else if (r = vow(c))
	    {
	      *dst++ = r;
	      state = Last;
	    }
	  else if (r = fcon(c))
	    {
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = r;
	    }
	  else if (r = lcon(c))
	    {
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = r;
	      state = First;
	    }
	  else if (c == Ctrl_O) {
              state = Ascii;
              *dst++ = 0x80;
              *dst++ = 0x80;
	    }
	  else
	    {
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = c;
	    }
	  break;
	case Last:
	  if ((r = fcon(c)) && vow(*src))
	    {
	      *dst++ = 0x80;
	      *dst++ = r;
	      state = Middle;
	    }
	  else if ((r = comcon(c, *src)) && !vow(src[1]))
	    {
	      *dst++ = r;
	      state = First;
	      src++;
	    }
	  else if (r = lcon(c))
	    {
	      *dst++ = r;
	      state = First;
	    }
	  else if (r = fcon(c))
	    {
	      *dst++ = 0x80;
	      *dst++ = r;
	      state = Middle;
	    }
	  else if (r = vow(c))
	    {
	      *dst++ = 0x80;
	      *dst++ = 0x80;
	      *dst++ = r;
	      *dst++ = 0x80;
	      state = First;
	    }
	  else if (c == Ctrl_O)
	    {
	      *dst++ = 0x80;
	      state = Ascii;
	    }
	  else
	    {
	      *dst++ = 0x80;
	      *dst++ = c;
	      state = First;
	    }
	}
    }
  *dst = 0;
}

void load_file(char* fname, editor* e)
{
  FILE* fp = fopen(fname, "r");
  if (fp != 0)
    {
      e->delete_all();
      char buf[maxbuf];
      while (fgets(buf, maxbuf - 1, fp) != 0)
	{
	  char* n = strchr(buf, '\n');
	  if (n)
	    *n = 0;
	  char result_buf[maxbuf*4];
	  convert_n_3(buf, result_buf);
	  e->append(result_buf);
	}
      fclose(fp);
      e->list_all();
      strcpy(file_name, fname);
    }
}
