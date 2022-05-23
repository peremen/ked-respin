// Print

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "editor.h"

void PrintLine(char*);
void PrintK(int, int, int);
void PrintA(int, int = True);
void ChangeFont(int);
int FindSet(int);
int FindF(int, int, int);
int FindM(int, int, int);
int FindL(int, int, int);

const char* prolog = 
"%! Ked output file"
"%% prolog"
"/LM 50 def\n"
"/RM 550 def\n"
"/TM 780 def\n"
"/BM 40 def\n"
"/FS 11 def\n"
"/VS { FS 2 add } def\n"
"/E { /Courier findfont FS scalefont setfont } def\n"
"/K1 { /MunjoHan1 findfont FS scalefont setfont } def\n"
"/K2 { /MunjoHan2 findfont FS scalefont setfont } def\n"
"/NP { /VP TM def LM VP moveto } def\n"
"/NL { /VP VP VS sub def LM VP moveto } def\n"
"/S { show } def\n"
"/SN { show NL VP BM sub 2 le { showpage NP } if } def\n"
"/SP { showpage } def\n"
"%% end of prolog\n"
"NP\n";
static int cs = 0;
static int col = 0;
static FILE* fp;

void print(void* arg)
{
  editor* e = (editor *)arg;
  char buf[maxbuf];
  fp = fopen("PrintOut", "w");
  if (fp == NULL)
    return;
  fputs(prolog, fp);
  e->first_value(buf);
  PrintLine(buf);
  while (e->next_value(buf))
    PrintLine(buf);
  if (col != 0)
    fputs(")S\n", fp);
  fputs("showpage\n", fp);
  fclose(fp);
}

void PrintLine(char* buf)
{
  if (buf[0] == '@')
    {
      if (strncmp(buf+1, "\201\223\210\215\201\200\220\223\200\201\225\200", 12) == 0)
	{
	  int ps = atoi(buf+13);
	  fprintf(fp, "/FS %d def\n", ps);
	  switch (cs)
	    {
	    case 1:
	    case 2:
	      fprintf(fp, "K%d ", cs);
	      break;
	    case 'a':
	      fputs("E ", fp);
	    }
	}
      else if (strncmp(buf+1, "\212\202\200\216\211\201", 6) == 0)
	fputs("SP\nNP\n", fp);
    }
  else
    {
      for (char* p = buf; *p != 0;)
	{
	  if (*p & 0x80)
	    {
	      PrintK(*p, *(p + 1), *(p + 2));
	      p += 3;
	    }
	  else
	    {
	      PrintA(*p);
	      p++;
	    }
	}
      if (col == 0)
	fputs("NL\n", fp);
      else
	fputs(")SN\n", fp);
      col = 0;
    }
}

void PrintK(int f, int m, int l)
{
  f &= 0x7f;
  m &= 0x7f;
  l &= 0x7f;
  ChangeFont(FindSet(m));
  PrintA(FindF(f, m, l), False);
  if (m == 0 && l == 0) {
      PrintA(0, False);
      PrintA(0, False);
  } else {
      PrintA(FindM(f, m, l), False);
      if (l > 0)
        PrintA(FindL(f, m, l), False);
  }
}

void ChangeFont(int s)
{
  if (cs != s)
    {
      if (cs != 0 && col != 0)
	fputs(")S ", fp);
      switch (s)
	{
	case 1:
	case 2:
	  fprintf(fp, "K%d ", s);
	  break;
	case 'a':
	  fputs("E ", fp);
	}
      if (cs == 0 && col != 0)
        fprintf(fp, "\\(");
      else if (col != 0)
        fputc('(',fp);
      cs = s;
    }
}
	
void PrintA(int a, int check)
{
    int n;
  if (check)
    ChangeFont('a');
  if (col == 0)
    fputc('(', fp);
  switch (a)
    {
    case '\t':
      n = col / 8 * 8 + 8 - col;
      while (n-- > 0)
	{
	  fputc(' ', fp);
	  col++;
	}
      break;
    case '\\':
      fputs("\\\\", fp);
      col++;
      break;
    case '(':
      fputs("\\(", fp);
      col++;
      break;
    case ')':
      fputs("\\)", fp);
      col++;
      break;
    case '{':
      fputs("\\{", fp);
      col++;
      break;
    case '}':
      fputs("\\}", fp);
      col++;
      break;
    default:
      if (a >= '0' && a <= '9')
	fprintf(fp, "\\%o", a);
      else if (a >= ' ' && a < 127)
	fputc(a, fp);
      else
	fprintf(fp, "\\%o", a);
      col++;
    }
}

int FindSet(int m)
{
  static int table[] = {1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 1, 2, 1};
  return table[m];
}

int FindF(int f, int m, int l)
{
  static unsigned char table[][20] = {
    // 0  |-  |=  |   |-| |=|
    {0x00, 0x10, 0x11, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
     0x1f, 0x0f, 0x21, 0x22, 0x23, 0x24, 0x26, 0x28},
    // 1  -|  =|  -|| =||
    {0x00, 0x10, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
     0x1f, 0x0f, 0x21, 0x22, 0x23, 0x25, 0x27, 0x28},
    // 2  o   yo  -
    {0x00, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e,
     0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46},
    // 3  u   yu
    {0x00, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
     0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61},
    // 4  wa  o|  _|
    {0x00, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47},
    // 5  o|-|
    {0x00, 0x50, 0x51, 0x37, 0x52, 0x53, 0x3a, 0x3b, 0x54, 0x3d, 0x55, 0x56,
     0x57, 0x58, 0x59, 0x43, 0x5a, 0x5b, 0x46, 0x5c},
    // 6  u-| u|
    {0x00, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x3d, 0x68, 0x69,
     0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71},
    // 7  u-||
    {0x00, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x3d, 0x80, 0x81,
     0x82, 0x83, 0x6c, 0x84, 0x85, 0x86, 0x70, 0x87},
    // with last
    // 8  |-  |=  |
    {0x00, 0x68, 0x69, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x73, 0x74, 0x75, 0x76,
     0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7e, 0x80},
    // 9  -|  =|
    {0x00, 0x68, 0x69, 0x6b, 0x6d, 0x6f, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
     0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7d, 0x7f, 0x80},
    // 10  o  yo  u  yu  -
    {0x00, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0,
     0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8},
    // 11  |-|  |=|
    {0x00, 0x8a, 0x8b, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x95, 0x96, 0x97, 0x98,
     0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0xa0, 0xa2},
    // 12  -||  =||
    {0x00, 0x8a, 0x8b, 0x8d, 0x8f, 0x91, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
     0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9f, 0xa1, 0xa2},
    // 13  wa  o|  _| u-|  u|
    {0x00, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0x96, 0xbe, 0xbf,
     0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7},
    // 14  o|-|  u-||
    {0x00, 0xdd, 0xde, 0xdf, 0xe0, 0xba, 0xe1, 0xe2, 0xe3, 0x96, 0xe4, 0xe5,
     0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xc5, 0xc6, 0xeb}};
  static unsigned char Mtable[][22] = {
    { 0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  4,  5,  4,  2,  3,  6,  7,  6,
      3,  2,  4,  0},
    { 0,  8, 11,  8, 11,  9, 12,  9, 12, 10, 13, 14, 13, 10, 10, 13, 14, 13,
     10, 10, 13,  8}};
  return table[Mtable[l ? 1 : 0][m]][f];
}

int FindM(int f, int m, int l)
{
  switch (l)
    {
    case 0:                // no last
      switch (m)
	{
	case 1:            // |-
	  return 0x29;
	case 2:            // |-|
	  switch (f)
	    {
	    case 2:        // gg
	    case 5:        // dd
	    case 9:        // bb
	    case 11:       // ss
	    case 14:       // jj
	      return 0x2a;
	    default:
	      return 0x29;
	    }
	case 3:            // |=
	  return 0x2a;
	case 4:            // |=|
	  return 0x2b;
	case 5:            // -|
	  switch (f)
	    {
	    case 3:        // n
	    case 4:        // d
	    case 10:       // s
	      return 0x2c;
	    case 9:        // bb
	    case 11:       // ss
	    case 14:       // jj
	      return 0x2d;
	    case 15:       // ch
	    case 19:       // h
	      return 0x2e;
	    default:
	      return 0x2b;
	    }
	case 6:            // -||
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x2c;
	    case 3:        // n
	    case 4:        // d
	    case 10:       // s
	      return 0x2d;
	    case 5:        // dd
	    case 9:        // bb
	    case 11:       // ss
	    case 14:       // jj
	      return 0x2f;
	    case 15:       // ch
	    case 19:       // h
	      return 0x30;
	    default:
	      return 0x2e;
	    }
	case 7:            // =|
	  switch (f)
	    {
	    case 3:        // n
	    case 4:        // d
	    case 10:       // s
	    case 17:       // t
	      return 0x30;
	    default:
	      return 0x2f;
	    }
	case 8:            // =||
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 8:        // b
	    case 15:       // ch
	    case 16:       // k
	      return 0x31;
	    case 3:        // n
	    case 4:        // d
	    case 10:       // s
	      return 0x32;
	    case 19:       // h
	      return 0x34;
	    default:
	      return 0x33;
	    }
	case 9:            // o
	  switch (f)
	    {
	    case 1:        // g
	      return 0x47;
	    case 19:       // h
	      return 0x49;
	    default:
	      return 0x48;
	    }
	case 10:           // o|-
	  switch (f)
	    {
	    case 1:        // g
	      return 0x48;
	    case 19:       // h
	      return 0x4a;
	    default:
	      return 0x49;
	    }
	case 11:           // o|-|
	  switch (f)
	    {
	    case 1:        // g
	      return 0x5d;
	    case 19:       // h
	      return 0x5f;
	    default:
	      return 0x5e;
	    }
	case 12:           // o|
	  switch (f)
	    {
	    case 1:        // g
	      return 0x4b;
	    case 19:       // h
	      return 0x4d;
	    default:
	      return 0x4c;
	    }
	case 13:           // yo
	  switch (f)
	    {
	    case 1:        // g
	      return 0x4a;
	    case 19:       // h
	      return 0x4c;
	    default:
	      return 0x4b;
	    }
	case 14:           // u
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x62;
	    case 19:       // h
	      return 0x64;
	    default:
	      return 0x63;
	    }
	case 15:           // u-|
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x72;
	    case 19:       // h
	      return 0x74;
	    default:
	      return 0x73;
	    }
	case 16:           // u-||
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x88;
	    default:
	      return 0x89;
	    }
	case 17:           // u|
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x75;
	    case 19:       // h
	      return 0x77;
	    default:
	      return 0x76;
	    }
	case 18:           // yu
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x65;
	    case 19:       // h
	      return 0x67;
	    default:
	      return 0x66;
	    }
	case 19:           // _
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 3:        // n
	    case 16:       // k
	      return 0x4d;
	    default:
	      return 0x4e;
	    }
	case 20:           // _|
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0x4e;
	    default:
	      return 0x4f;
	    }
	case 21:           // |
	  return 0x33;
	}
    default:               // with last
      switch (m)
	{
	case 1:            // |-
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      return 0x81;
	    default:
	      return 0x82;
	    }
	case 2:            // |-|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 2:    // gg
		case 5:    // dd
		case 9:    // bb
		case 11:   // ss
		case 14:   // jj
		  return 0xa6;
		default:
		  return 0xa4;
		}
	    default:
	      switch (f)
		{
		case 2:    // gg
		case 5:    // dd
		case 9:    // bb
		case 11:   // ss
		case 14:   // jj
		  return 0xa5;
		default:
		  return 0xa3;
		}
	    }
	case 3:            // |=
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      return 0x83;
	    default:
	      return 0x84;
	    }
	case 4:            // |=|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      return 0xa8;
	    default:
	      return 0xa7;
	    }
	case 5:            // -|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 1:    // g
		case 2:    // gg
		case 16:   // k
		  return 0x86;
		case 3:    // n
		case 4:    // d
		case 10:   // s
		  return 0x88;
		case 7:    // m
		case 8:    // b
		case 12:   // ng
		case 13:   // j
		  return 0x8a;
		case 15:   // ch
		case 19:   // h
		  return 0x8e;
		default:
		  return 0x8c;
		}
	    default:
	      switch (f)
		{
		case 1:    // g
		case 2:    // gg
		case 16:   // k
		  return 0x85;
		case 3:    // n
		case 4:    // d
		case 10:   // s
		  return 0x87;
		case 7:    // m
		case 8:    // b
		case 12:   // ng
		case 13:   // j
		  return 0x89;
		case 15:   // ch
		case 19:   // h
		  return 0x8d;
		default:
		  return 0x8b;
		}
	    }
	case 6:            // -||
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 3:    // n
		case 4:    // d
		case 10:   // s
		  return 0xac;
		case 5:    // dd
		case 9:    // bb
		case 11:   // ss
		case 14:   // jj
		  return 0xae;
		case 15:   // ch
		case 19:   // h
		  return 0xb0;
		default:
		  return 0xaa;
		}
	    default:
	      switch (f)
		{
		case 3:    // n
		case 4:    // d
		case 10:   // s
		  return 0xab;
		case 5:    // dd
		case 9:    // bb
		case 11:   // ss
		case 14:   // jj
		  return 0xad;
		case 15:   // ch
		case 19:   // h
		  return 0xaf;
		default:
		  return 0xa9;
		}
	    }
	case 7:            // =|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 1:    // g
		case 2:    // gg
		case 7:    // m
		case 8:    // b
		case 12:   // ng
		case 16:   // k
		  return 0x90;
		case 3:    // n
		case 4:    // d
		case 10:   // s
		  return 0x92;
		case 15:   // ch
		case 19:   // h
		  return 0x96;
		default:
		  return 0x94;
		}
	    default:
	      switch (f)
		{
		case 1:    // g
		case 2:    // gg
		case 7:    // m
		case 8:    // b
		case 12:   // ng
		case 16:   // k
		  return 0x8f;
		case 3:    // n
		case 4:    // d
		case 10:   // s
		  return 0x91;
		case 15:   // ch
		case 19:   // h
		  return 0x95;
		default:
		  return 0x93;
		}
	    }
	case 8:            // =||
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 3:    // n
		case 4:    // d
		  return 0xb3;
		case 15:   // ch
		case 19:   // h
		  return 0xb5;
		default:
		  return 0xb2;
		}
	    default:
	      switch (f)
		{
		case 3:    // n
		case 4:    // d
		  return 0xb1;
		case 15:   // ch
		case 19:   // h
		  return 0xb4;
		default:
		  return 0xb1;
		}
	    }
	case 9:            // o
	  switch (f)
	    {
	    case 1:        // g
	      return 0xc9;
	    case 6:        // r
	    case 13:       // j
	    case 14:       // jj
	    case 15:       // ch
	    case 16:       // k
	      return 0xcb;
	    case 17:       // t
	    case 18:       // p
	    case 19:       // h
	      return 0xcc;
	    default:
	      return 0xca;
	    }
	case 10:           // o|-
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xcc;
		default:
		  return 0xca;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xcb;
		default:
		  return 0xc9;
		}
	    }
	case 11:           // o|-|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xef;
		default:
		  return 0xed;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xee;
		default:
		  return 0xec;
		}
	    }
	case 12:           // o|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xd0;
		default:
		  return 0xce;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xcf;
		default:
		  return 0xcd;
		}
	    }
	case 13:           // yo
	  switch (f)
	    {
	    case 1:        // g
	      return 0xcd;
	    case 6:        // r
	    case 12:       // ng
	    case 14:       // jj
	    case 15:       // ch
	    case 17:       // t
	    case 18:       // p
	      return 0xcf;
	    case 19:       // h
	      return 0xd0;
	    default:
	      return 0xce;
	    }
	case 14:           // u
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      switch (l)
		{
		case 23:   // ch
		case 27:   // h
		  return 0xd1;
		case 4:    // n
		  return 0xd2;
		default:
		  return 0xd3;
		}
	    case 19:       // h
	      switch (l)
		{
		case 4:    // n
		  return 0xd4;
		default:
		  return 0xd5;
		}
	    default:
	      switch (l)
		{
		case 1:    // g
		case 7:    // d
		case 16:   // m
		case 17:   // b
		case 20:   // s
		  return 0xd6;
		case 4:    // n
		  return 0xd7;
		default:
		  return 0xd8;
		}
	    }
	case 15:           // u-|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xd8;
		default:
		  return 0xd6;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xd7;
		default:
		  return 0xd5;
		}
	    }
	case 16:           // u-||
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xf3;
		default:
		  return 0xf1;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xf2;
		default:
		  return 0xf0;
		}
	    }
	case 17:           // u|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xdc;
		default:
		  return 0xda;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xdb;
		default:
		  return 0xd9;
		}
	    }
	case 18:           // yu
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      switch (l)
		{
		case 23:   // ch
		case 27:   // h
		  return 0xd9;
		case 4:    // n
		  return 0xda;
		default:
		  return 0xdb;
		}
	    case 19:       // h
	      switch (l)
		{
		case 4:    // n
		  return 0xdc;
		default:
		  return 0xdd;
		}
	    default:
	      switch (l)
		{
		case 1:    // g
		case 7:    // d
		case 16:   // m
		case 17:   // b
		case 19:   // s
		  return 0xde;
		case 4:    // n
		  return 0xdf;
		default:
		  return 0xe0;
		}
	    }
	case 19:           // _
	  switch (f)
	    {
	    case 1:        // g
	    case 2:        // gg
	    case 16:       // k
	      return 0xe1;
	    case 19:       // h
	      return 0xe2;
	    default:
	      return 0xe3;
	    }
	case 20:           // _|
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      switch (f)
		{
		case 19:   // h
		  return 0xd4;
		default:
		  return 0xd2;
		}
	    default:
	      switch (f)
		{
		case 19:   // h
		  return 0xd3;
		default:
		  return 0xd1;
		}
	    }
	case 21:           // |
	  switch (l)
	    {
	    case 4:        // n
	    case 19:       // s
	      return 0x97;
	    default:
	      return 0x98;
	    }
	}
    }
  return 0;
}

int FindL(int f, int m, int l)
{
  static int table[][28] = {
    {0x00, 0x99, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xf1, 0xa8, 0xa9,
     0xaa, 0xab, 0xac, 0xad, 0xae, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5},
    {0x00, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xf1, 0xa8, 0xa9,
     0xaa, 0xab, 0xac, 0xad, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5},
    {0x00, 0xe4, 0xe6, 0xe7, 0xe8, 0x9e, 0xe9, 0xea, 0xeb, 0xed, 0xee, 0xa5, 0xef, 0xf0, 0xf1, 0xf2, 0xf3,
     0xf5, 0xab, 0xf6, 0xad, 0xf7, 0xf9, 0xfa, 0xfb, 0xfc, 0xfe, 0xff},
    {0x00, 0xe5, 0xe6, 0xe7, 0xe8, 0x9e, 0xe9, 0xea, 0xec, 0xed, 0xee, 0xa5, 0xef, 0xf0, 0xf1, 0xf2, 0xf4,
     0xf5, 0xab, 0xf6, 0xad, 0xf8, 0xf9, 0xfa, 0xfb, 0xfd, 0xfe, 0xff},
    {0x00, 0xf4, 0xf4, 0xf4, 0xf5, 0xf5, 0xf5, 0xf6, 0xf7, 0xf7, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf9,
     0xfa, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff}};
  static int Mtable[] = {0, 0, 4, 0, 4, 0, 4, 0, 4, 2, 4, 4, 4, 2, 2, 4, 4, 4, 2, 2, 4, 0};
  switch (Mtable[m])
    {
    case 0:
      switch (f)
	{
	case 1:               // g
	case 3:               // n
	case 4:               // d
	case 5:               // dd
	case 7:               // m
	case 10:              // s
	case 13:              // j
	case 16:              // k
	  return table[0][l];
	default:
	  return table[1][l];
	}
    case 2:
      switch (f)
	{
	case 19:              // h
	  return table[3][l];
	default:
	  return table[2][l];
	}
    default:
      return table[4][l]; 
    }
}
