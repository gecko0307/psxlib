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

#ifndef _GTE_H_
#define _GTE_H_

#define F_ONE 0x1000

/* Vector of three 16-bit elements */
typedef struct
{
    short x, y, z, __padding;
}
Vertex;

/* Screen-space vector of three 16-bit elements */
typedef struct
{
    short x, y;
    unsigned short z;
    short __padding;
}
SVertex;

/* Rotate/translate/perspective transform */
typedef struct
{
    long tx, ty, tz; /* translation, bytes 0..12 */
    short r[9];      /* 3x3 rotation matrix of Q3.12 elements, bytes 14.. */
    short h;
    long ofx, ofy;
    short dqa, dqb;
} RTPSTransform;

/* Enables the GTE. Should be called once at the start of the game */
void gteEnable(void);

/* Transforms a vertex and brings it to the screen space */
void gteRTPS(RTPSTransform* rtpsTransform, Vertex* inVertex, SVertex* outVertex);

#endif
