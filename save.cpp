// Save file

#include <stdio.h>
#include <string.h>
#include "ked.h"
#include "input_dialog.h"
#include "aline.h"

const int Ascii = 0;
const int Korean = 1;
const int Ctrl_N = 'N' - '@';
const int Ctrl_O = 'O' - '@';

void save(void* arg)
{
    input_dialog* d = (input_dialog *)arg;
    d->input_box()->delete_all();
    d->input_box()->append(file_name);
    d->map();
}

static int fcon(int f)
{
    static int map[] = {0, 'A', 'B', 'D', 'G', 'H', 'I', 'Q', 'R', 'S', 'U',
        'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^'};
    if (f > 128 && f < 128 + 20)
        return map[f - 128];
    return 0;
}

static int vow(int m)
{
    static int map[] = {0, 'b', 'c', 'd', 'e', 'f', 'g', 'j', 'k', 'l', 'l',
        'l', 'l', 'r', 's', 's', 's', 's', 'w', 'z', 'z', '|'};
    if (m > 128 && m < 128 + 22)
        return map[m - 128];
    return 0;
}

static int comvow(int m)
{
    static int map[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'b', 'c', '|', 0, 0, 'f',
        'g', '|', 0, 0, '|', 0};
    if (m > 128 && m < 128 + 22)
        return map[m - 128];
    return 0;
}

static int lcon(int l)
{
    static int map[] = {0, 'A', 'B', 'A', 'D', 'D', 'D', 'G', 'I', 'I', 'I', 'I',
        'I', 'I', 'I', 'I', 'Q', 'R', 'R', 'U', 'V', 'W', 'X',
        'Z', '[', '\\', ']', '^'};
    if (l > 128 && l < 128 + 28)
        return map[l - 128];
    return 0;
}

static int comcon(int l)
{
    static int map[] = {0, 0, 0, 'U', 0, 'X', '^', 0, 0, 'A', 'Q', 'R', 'U',
        '\\', ']', '^', 0, 0, 'U', 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (l > 128 && l < 128 + 28)
        return map[l - 128];
    return 0;
}

static int convert(unsigned char f, unsigned char m, unsigned char l,
        char* dst)
{
    char* org_dst = dst;
    int r;
    if (r = fcon(f))
        *dst++ = r;
    if (r = vow(m))
        *dst++ = r;
    if (r = comvow(m))
        *dst++ = r;
    if (r = lcon(l))
        *dst++ = r;
    if (r = comcon(l))
        *dst++ = r;
    return dst - org_dst;
}

static void convert_3_n(char* src, char* dst)
{
    int prev = Ascii;
    char c;
    while (c = *src++)
    {
        if (c & 0x80)
        {
            if (prev != Korean)
                *dst++ = Ctrl_N;
            dst += convert(c, src[0], src[1], dst);
            src += 2;
            prev = Korean;
        }
        else
        {
            if (prev != Ascii)
                *dst++ = Ctrl_O;
            *dst++ = c;
            prev = Ascii;
        }
    }
    if (prev == Korean)
        *dst++ = Ctrl_O;
    *dst++ = '\n';
    *dst = 0;
}

void save_file(char* fname, editor* e)
{
    FILE* fp = fopen(fname, "w");
    if (fp != 0)
    {
        char buf[maxbuf];
        e->first_value(buf);
        char result_buf[maxbuf*3];
        convert_3_n(buf, result_buf);
        fputs(result_buf, fp);
        while (e->next_value(buf))
        {
            convert_3_n(buf, result_buf);
            fputs(result_buf, fp);
        }
        fclose(fp);
        strcpy(file_name, fname);
    }
}
