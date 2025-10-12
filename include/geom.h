/*
MIT License

Copyright (c) 2025 Timur Gafarov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _GEOM_H_
#define _GEOM_H_

/* Vector of three 16-bit elements */
typedef struct
{
    short x;
    short y;
    short z;
}
Vertex;

/* Vector of three 32-bit elements */
typedef struct
{
    int x;
    int y;
    int z;
}
Trans;

/* 3x3 matrix of Q3.12 elements */
typedef struct
{
    short m[9];
}
Mat3;

typedef struct
{
    short h; short _h_pad;     /* 0 */
    int ofx;                   /* 4 */
    int ofy;                   /* 8 */
    short dqa; short _dqa_pad; /* 12 */
    short dqb; short _dqb_pad; /* 16 */
}
Proj;

#define PSHORT(val) val, 0

#define PROJ(h_val, ofx_val, ofy_val, dqa_val, dqb_val) { PSHORT(h_val), ofx_val, ofy_val, PSHORT(dqa_val), PSHORT(dqb_val) }

int gte_rtps(Mat3* rotation, Trans* translation, Vertex* vertex, Proj* proj);
int gte_rtps2(Mat3* rotation, Trans* translation, Vertex* vertex, Proj* proj);

#endif
