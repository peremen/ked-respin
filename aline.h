// a line structure

#ifndef _aline_
#define _aline_

const int maxbuf = 160;

struct aline
{
    aline* up;
    aline* down;
    char buf[maxbuf];
};

#endif // _aline_
