/*
MIT License

Copyright (c) 2022 Timur Gafarov

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

#ifndef _GPU_H_
#define _GPU_H_

#include "types.h"

#define VMODE_NTSC (0)
#define VMODE_PAL  (1)
#define VMODE_AUTO (2)

#define ENV_W256 (0 << 6 | 0)
#define ENV_W320 (0 << 6 | 1)
#define ENV_W512 (0 << 6 | 2)
#define ENV_W640 (0 << 6 | 3)
#define ENV_W384 (1 << 6 | 0)
#define ENV_H240 (0 << 2)
#define ENV_H480 (1 << 2)

#define GPU_COM(com, r, g, b) ((b << 16) | (g << 8) | r | (com << 24))
#define H2L(x, y) ((x & 0xffff) | (y << 16))
#define HB2L(id, u, v) ((id << 16) | (v << 8) | u)

typedef struct
{
    short x;
    short y;
    short w;
    short h;
} Rect;

typedef struct {
    unsigned long id;
    unsigned long flag;
} TIMHeader;

typedef struct {
    unsigned long datalen;
    unsigned long vrampos;
    unsigned long size;
    unsigned long data;
} TIMData;

typedef struct
{
    Rect disp;
    Rect screen;
    unsigned char isInter;
    unsigned char isRGB24;
} DispEnv;

typedef struct
{
  Rect clip;
  short ofs[2];
  Rect tw;
  unsigned char tpage;
  unsigned short dtd;
  unsigned short dfe;
  unsigned char isbg;
  unsigned char r0;
  unsigned char g0;
  unsigned char b0;
} DrawEnv;

typedef struct
{
    unsigned long pmode;
    short px, py;
    unsigned short pw, ph;
    unsigned short clutid;
    unsigned short tpage;
    TIMData* clut;
    TIMData* pixel;
} GpuImage;

typedef struct
{
    unsigned char r, g, b, p;
    short x, y;
    long trans;
} GpuDot;

typedef struct
{
    unsigned char r, g, b, p;
    short x0, y0;
    short x1, y1;
    long trans;
} GpuLine;

typedef struct
{
    unsigned char r, g, b, p;
    short x, y;
    unsigned short w, h;
    unsigned char u, v;
    unsigned short clutid;
    unsigned short tpage;
    short mx, my;
    short scalex, scaley;
    long rotate;
    long trans;
} GpuSprite;

// x-res = horizontal resolution (256/320/384/512/640)
// y-res = vertical (240/480 NTSC, 256,512 PAL)
// vmode = VMODE_NTSC, VMODE_PAL, or VMODE_AUTO
// intl = bit 0 - interlace flag (0:off, 1:on)
// dither = dither flag (0:off, 1:on)
// vram = frame buffer mode (0:16bit, 1:24bit)
void gpuInit(int dw, int dh, int vmode, int intl, int dither, int vram);

DrawEnv* gpuPutDrawEnv(DrawEnv* env);
DispEnv* gpuPutDispEnv(DispEnv* env);

void gpuMemToVram(TIMData* pTIM, unsigned long sizeInBytes);

void gpuImageGetInfo(TIMHeader* tim, GpuImage* img);
long gpuImageUpload(GpuImage* img);

void gpuFontLoad(unsigned char fontNum, TIMHeader* newFont);
void gpuPrintChar(unsigned char fontNum, int x, int y, unsigned char asciiCode);
void gpuPrintStr(unsigned char fontNum, int x, int y, char* fmt);

int gpuGetActiveBuffer(void);
void gpuSetWorkBase(unsigned char * queueAddr);
void gpuDraw(unsigned long * pGP);
void gpuSwapBuffers(void);

void gpuSortClear(int r, int g, int b);
void gpuSortDot(GpuDot* dot);
void gpuSortLine(GpuLine* line);
void gpuSortSprite(GpuSprite* sprite);

#endif