/*
MIT License

Copyright (c) 2022-2025 Timur Gafarov

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

#include "gpu.h"
#include "specfont.h"

typedef volatile long * vptr;

#define i_reg (*(vptr)0x1f801070)
#define i_mask (*(vptr)0x1f801074)
#define d2_madr (*(vptr)0x1f8010a0)
#define d2_bcr (*(vptr)0x1f8010a4)
#define d2_chcr (*(vptr)0x1f8010a8)
#define d_pcr (*(vptr)0x1f8010f0)
#define d_icr (*(vptr)0x1f8010f4)
#define gp0 (*(vptr)0x1f801810)
#define gp1 (*(vptr)0x1f801814)
#define d2_linear (0x01000201)
#define d2_linked (0x01000401)

#define wait_gpu while (!(gp1 & 0x04000000))
#define wait_dma while (d2_chcr & 0x01000000)

void GP_SendGPU0(unsigned long gpuPrimitive)
{
    gp0 = gpuPrimitive;
}

DispEnv __dispEnv;
DrawEnv __drawEnv;

unsigned int __videoMode = 0; /* 0: NTSC, 1: PAL */
unsigned long PSDCNT = 0; /* frame switch counter */
unsigned long PSDOFSX[2]; /* double buffer x coords */
unsigned long PSDOFSY[2]; /* double buffer y coords */

unsigned long  __primQueueLength = 0; /* length of primitive queue */
unsigned long* __primQueue = NULL; /* pointer to primitive queue */

GpuImage __font[10];
unsigned long __fontColor[10];
unsigned char __fontWidth[10];
unsigned char __fontHeight[10];
unsigned char __stringBuffer[128];

unsigned long  OBJECT_number = 0;
unsigned long  OBJECT_z[1024];
unsigned long* OBJECT_prev;
unsigned long* OBJECT_first;
unsigned long* OBJECT_start[1024];
unsigned long* OBJECT_end[1024];

static inline void ADD_PACKET(int num, unsigned long data)
{
    *(__primQueue + num + 1) = data;
}

static inline void NEXT_PACKET(int size)
{
    unsigned long data;
    unsigned long address = (unsigned long)(__primQueue + size + 1) & 0x00ffffff;
    data = (size << 24) | address;
    *(__primQueue) = data;
    OBJECT_prev = __primQueue; 
    __primQueue += (size + 1);  
}

static inline void SWITCH_ADDRESS(unsigned long * first, unsigned long * second)
{
    *first = (*first & 0xff000000) | ((unsigned long)second & 0x00ffffff);
}

void object_quick_sort(int s, int e);

void gpuInit(int dw, int dh, int vmode, int intl, int dither, int vram)
{
    /* Detect or set video mode */
    if (vmode >= VMODE_AUTO)
    {
        if (*(char*)0xbfc7ff52 == 'E')
            __videoMode = VMODE_PAL;
        else
            __videoMode = VMODE_NTSC;
    }
    else
    {
        __videoMode = vmode;
    }
    
    /* reset the GPU */
    gp1 = 0x00000000;
    
    /* enable linked-list DMA transfer */
    gp1 = 0x04000002;

    /* enable DMA channel 2 */
    d_pcr |= 0x0800;
    
    /* initialise some values */
    __dispEnv.disp.x = 0;
    __dispEnv.disp.y = 0;
    __dispEnv.disp.w = dw;
    __dispEnv.disp.h = dh;
    
    __drawEnv.tw.x = 0;
    __drawEnv.tw.y = 0;
    __drawEnv.tw.w = 0;
    __drawEnv.tw.h = 0;
    
    __drawEnv.clip.x = 0;
    __drawEnv.clip.y = 0;
    __drawEnv.clip.w = 1023;
    __drawEnv.clip.h = 511;
    
    __dispEnv.screen.x = 0;
    __dispEnv.screen.y = 0;
    __dispEnv.screen.w = 0;
    __dispEnv.screen.h = 0;

    if (__videoMode == VMODE_PAL)
        __dispEnv.screen.y = 23;
    
    __dispEnv.isInter = (intl & 0x01) << 5;
    __dispEnv.isRGB24 = (vram & 0x01) << 4;

    __drawEnv.ofs[0] = 0;
    __drawEnv.ofs[1] = 0;
    __drawEnv.tpage = 0;
    __drawEnv.dtd = (dither & 0x01) << 9;
    __drawEnv.dfe = 1 << 10;
    __drawEnv.isbg = 0;

    gpuPutDrawEnv(&__drawEnv);
    gpuPutDispEnv(&__dispEnv);
    
    gpuFontLoad(0, (TIMHeader*)specfont);

    gp1 = 0x03000000; /* enable the display */
}

DrawEnv* gpuPutDrawEnv(DrawEnv* env)
{
    GP_SendGPU0(0xe1000200); /* drawing mode */
    GP_SendGPU0(0xe2000000); /* texture window settings */
    GP_SendGPU0(0xe3000000 | env->clip.y << 10 | env->clip.x); /* clip area start and end */
    GP_SendGPU0(0xe4000000 | (env->clip.h + env->clip.y) << 10 | (env->clip.w + env->clip.x));
    GP_SendGPU0(0xe5000000); /* drawing offset env->ofs[0]  env->ofs[1] */
    GP_SendGPU0(0xe6000000); /* mask settings */
    return env;
}

DispEnv* gpuPutDispEnv(DispEnv* env)
{
    unsigned long x0, y0, x1, y1;

    /* display offset */
    gp1 = (0x05000000 | (env->disp.y << 10) | (env->disp.x & 0xffff));

    /* horizontal screen start/end */
    x0 = env->screen.x *10 + 608;
    x1 = ((env->screen.x + 256) *10) + 608;
    gp1 = (0x06000000 | x1 << 12 | x0);

    /* vertical screen start/end */
    y0 = env->screen.y + 16 + (3 * __videoMode);
    y1 = y0 + 240;
    gp1 = (0x07000000 | y1 << 10 | y0);

    /* display mode */
    switch (env->disp.w)
    {
        case 256: x0 = ENV_W256; break;
        case 384: x0 = ENV_W384; break;
        case 512: x0 = ENV_W512; break;
        case 640: x0 = ENV_W640; break;
        default: x0 = ENV_W320;
    }

    y0 = ENV_H240;
    if (env->disp.h == 480)
        y0 = ENV_H480;

    gp1 = 0x08000000 | x0 | y0 | env->isInter | env->isRGB24 | (__videoMode << 3);
    
    return env;
}

void gpuFontLoad(unsigned char fontNum, TIMHeader* newFont)
{
    newFont = (TIMHeader*)specfont;
    gpuImageGetInfo(newFont, &__font[fontNum]);
    gpuImageUpload(&__font[fontNum]);
    
    /* set RGB to mid values */
    __fontColor[fontNum] = 0x74808080;
    __fontWidth[fontNum] = 0;
    __fontHeight[fontNum] = 0;
}

void gpuPrintChar(unsigned char fontNum, int x, int y, unsigned char asciiCode)
{
    unsigned char u, v;

    if (__fontWidth[fontNum] + __fontWidth[fontNum] == 0)
    {
        /* if the font is only 16 pixels high, it cannot include lower case, so convert to upper */
        if ((__font[fontNum].ph == 16) && (asciiCode > 0x60))
            asciiCode -= 0x20;

        /* work out the character offset in the TIM image */
        u = (asciiCode & 0x1f) << 3;
        v = (((asciiCode & 0xe0) >> 5) - 1) << 3;

        /* work out the offset in the texture page */
        u += __font[fontNum].px;
        v += __font[fontNum].py;

        /* add the character to the Ordering Table */
        ADD_PACKET(0, __fontColor[fontNum]);
        ADD_PACKET(1, H2L(x, y));
        ADD_PACKET(2, HB2L(__font[fontNum].clutid, u, v));
        NEXT_PACKET(3);
    }
}

void gpuPrintStr(unsigned char fontNum, int x, int y, char* str)
{
    int strn = 0;
    int count = 0;
    int c, w, n, padc;
    char *s;
    char *out = __stringBuffer + strn;
    
    while ((c = *str++) != 0)
    {
        out[count++] = c;
    }
    
    strn += count;
    
    /* set the texture page for the font */
    ADD_PACKET(0, (0xe1000200 | __font[fontNum].tpage));
    NEXT_PACKET(1);

    for (n = 0; n < count; n++)
    {
        gpuPrintChar(fontNum, x, y, *out);
        out++;
        x += 8;
    }
}

void gpuImageGetInfo(TIMHeader* tim, GpuImage* img)
{
    TIMData *clut = NULL;
    TIMData *pixel = NULL;
    unsigned long *pp = NULL;

    /* get the color mode */
    img->pmode = (tim->flag & 0x03);
    
    if ((tim->flag & 0x08) == 0) { /* there is no CLUT */
        pixel = (TIMData*)++tim;
        img->clutid = 0;
    }
    else { /* there is a clut */
        clut = (TIMData*)++tim;
        img->clutid = ((clut->vrampos >> 10) | (clut->vrampos & 0xffff) >> 4);
        pp = (unsigned long *)clut + (clut->datalen >> 2);
        pixel = (TIMData*)pp;
    }
    
    /* x/y address of pixel data */
    img->px = (pixel->vrampos & 0xffff);
    img->py = (pixel->vrampos >> 16);
    
    /* tpage number */
    img->tpage = ((img->py & 0x100) >> 4) | ((img->px & 0x3ff) >> 6) | ((img->py & 0x200) << 2);
    img->tpage = ((img->pmode << 7) | img->tpage);
    
    img->px %= 64; /* x/y offset within tpage */
    if (img->py > 255)
        img->py -= 256;
    
    img->pw = (pixel->size & 0xffff) << (2 - img->pmode); /* width/height of pixel data */
    img->ph = (pixel->size >> 16);
    
    img->clut = clut;
    img->pixel = pixel;
}

long gpuImageUpload(GpuImage* img)
{
    gpuMemToVram(img->pixel, img->pixel->datalen);
    
    if (img->clut != NULL)
    {
        gpuMemToVram(img->clut, img->clut->datalen);
    }
    
    return 0;
}

void LoadImage(Rect *rect, unsigned long *p)
{
    unsigned long vrampos = (rect->y << 16 | rect->x);
    unsigned long size = (rect->h << 16 | rect->w);
    unsigned long size_in_words = (rect->w *rect->h);
    unsigned long i;

    wait_gpu;

    gp1 = 0x01000000; /* reset the command queue */
    GP_SendGPU0(0xa0000000); /* send 'send image' primitive */
    GP_SendGPU0(vrampos); /* send x and y coordinates */
    GP_SendGPU0(size); /* send width and height */

    for (i = 0; i < size_in_words; i++)
        GP_SendGPU0(*(p + i));

    gp1 = 0x01000000; /* reset the command queue again */
}

void gpuMemToVram(TIMData* pTIM, unsigned long sizeInBytes)
{
    unsigned long i;
    unsigned long *pp = (unsigned long *) pTIM;
    unsigned long sizeInWords = (sizeInBytes >> 2) - 3;

    wait_gpu;

    gp1 = 0x01000000; /* reset the command queue */
    GP_SendGPU0(0xa0000000); /* send 'send image' primitive */
    GP_SendGPU0(*(pp + 1)); /* send x and y coordinates */
    GP_SendGPU0(*(pp + 2)); /* send width and height */

    for (i = 0; i < sizeInWords; i++)
        GP_SendGPU0(*(pp + 3 + i));

    gp1 = 0x01000000; /* reset the command queue again */
}

int gpuGetActiveBuffer(void)
{
    return (PSDCNT & 0x01);
}

void gpuSetWorkBase(unsigned char * queueAddr)
{
    __primQueue = (unsigned long *)queueAddr;
}

void gpuSortClear(int r, int g, int b)
{
    r &= 0xff;
    g &= 0xff;
    b &= 0xff;
    ADD_PACKET(0, GPU_COM(0x60, r, g, b));
    ADD_PACKET(1, 0x00000000);
    ADD_PACKET(2, H2L(__dispEnv.disp.w, __dispEnv.disp.h));
    NEXT_PACKET(3);
}

void gpuSortDot(GpuDot* dot)
{
    ADD_PACKET(0, GPU_COM(0x68, dot->r, dot->g, dot->b));
    ADD_PACKET(1, H2L(dot->x, dot->y));
    NEXT_PACKET(2);
}

void gpuSortLine(GpuLine* line)
{
    ADD_PACKET(0, GPU_COM(0x40, line->r, line->g, line->b));
    ADD_PACKET(1, H2L(line->x0, line->y0));
    ADD_PACKET(2, H2L(line->x1, line->y1));
    NEXT_PACKET(3);
}

void gpuSortSprite(GpuSprite* sprite)
{
    ADD_PACKET(0, (0xe1000200 | sprite->tpage));
    ADD_PACKET(1, GPU_COM(0x64, sprite->r, sprite->g, sprite->b));
    ADD_PACKET(2, H2L(sprite->x, sprite->y));
    ADD_PACKET(3, HB2L(sprite->clutid, sprite->u, sprite->v));
    ADD_PACKET(4, H2L(sprite->w, sprite->h));
    NEXT_PACKET(5);
}

void gpuSortPoly3(GpuPoly3* poly)
{
    ADD_PACKET(0, 0xe1000200);
    ADD_PACKET(1, GPU_COM(0x20, poly->r, poly->g, poly->b));
    ADD_PACKET(2, H2L(poly->x0, poly->y0));
    ADD_PACKET(3, H2L(poly->x1, poly->y1));
    ADD_PACKET(4, H2L(poly->x2, poly->y2));
    NEXT_PACKET(5);
}

void gpuSortGPoly3(GpuGPoly3* poly)
{
    ADD_PACKET(0, 0xe1000200);
    ADD_PACKET(1, GPU_COM(0x30, poly->r0, poly->g0, poly->b0));
    ADD_PACKET(2, H2L(poly->x0, poly->y0));
    ADD_PACKET(3, GPU_COM(0x00, poly->r1, poly->g1, poly->b1));
    ADD_PACKET(4, H2L(poly->x1, poly->y1));
    ADD_PACKET(5, GPU_COM(0x00, poly->r2, poly->g2, poly->b2));
    ADD_PACKET(6, H2L(poly->x2, poly->y2));
    NEXT_PACKET(7);
}

void gpuSortTPoly3(GpuTPoly3* poly)
{
    ADD_PACKET(0, 0xe1000200);
    ADD_PACKET(1, GPU_COM(0x24, poly->r, poly->g, poly->b));
    ADD_PACKET(2, H2L(poly->x0, poly->y0));
    ADD_PACKET(3, HB2L(poly->clutid, poly->u0, poly->v0));
    ADD_PACKET(4, H2L(poly->x1, poly->y1));
    ADD_PACKET(5, HB2L(poly->tpage, poly->u1, poly->v1));
    ADD_PACKET(6, H2L(poly->x2, poly->y2));
    ADD_PACKET(7, HB2L(0, poly->u2, poly->v2));
    NEXT_PACKET(8);
}

void gpuSortGTPoly3(GpuGTPoly3* poly)
{
    ADD_PACKET(0, (0xe1000200));
    ADD_PACKET(1, GPU_COM(0x34, poly->r0, poly->g0, poly->b0));
    ADD_PACKET(2, H2L(poly->x0, poly->y0));
    ADD_PACKET(3, HB2L(poly->clutid, poly->u0, poly->v0));
    ADD_PACKET(4, GPU_COM(0x00, poly->r1, poly->g1, poly->b1));
    ADD_PACKET(5, H2L(poly->x1, poly->y1));
    ADD_PACKET(6, HB2L(poly->tpage, poly->u1, poly->v1));
    ADD_PACKET(7, GPU_COM(0x00, poly->r2, poly->g2, poly->b2));
    ADD_PACKET(8, H2L(poly->x2, poly->y2));
    ADD_PACKET(9, HB2L(0, poly->u2, poly->v2));
    NEXT_PACKET(10);
}

/*
void GsSortObject(GsObject* object)
{
    OBJECT_start[OBJECT_number] = __primQueue;
    if (OBJECT_number == 0)
        OBJECT_first = OBJECT_prev;
    
    // store the projected z value
    OBJECT_z[OBJECT_number] = GsSortObject2(object);
    OBJECT_end[OBJECT_number] = OBJECT_prev;
    OBJECT_number++;
}
*/

void gpuDraw(unsigned long* pGP)
{
    int i;
    unsigned long *OBJECT_last;

    if (OBJECT_number > 1)
    {
        (unsigned long)OBJECT_last = *OBJECT_end[OBJECT_number-1];
        object_quick_sort(0, OBJECT_number - 1);
        SWITCH_ADDRESS(OBJECT_first, OBJECT_start[0]);
        for (i = 1; i < OBJECT_number; i++) {
            SWITCH_ADDRESS(OBJECT_end[i-1], OBJECT_start[i]);
        }
        SWITCH_ADDRESS(OBJECT_end[OBJECT_number-1], OBJECT_last);
    }
    
    gp1 = (0x05000000 | (PSDOFSY[PSDCNT & 0x01] << 10) | PSDOFSX[PSDCNT & 0x01]);
    *(__primQueue) = 0x00ffffff; /* mark the end of the list */
    
    d_icr = 0;
    d2_madr = (unsigned long)pGP;
    d2_bcr = 0;
    d2_chcr = d2_linked;
    PSDCNT++;
    
    OBJECT_number = 0;
}

void gpuSwapBuffers(void)
{
    unsigned long x, y, data;

    x = PSDOFSX[PSDCNT & 0x01];
    y = PSDOFSY[PSDCNT & 0x01];

    data = ((y + __dispEnv.disp.h - 1) << 10) | (x + __dispEnv.disp.w);

    ADD_PACKET(0, (0xe5000000 | (y << 11) | x));
    ADD_PACKET(1, (0xe3000000 | (y << 10) | x));
    ADD_PACKET(2, (0xe4000000 | data));
    NEXT_PACKET(3);
}

void object_quick_sort(int s, int e)
{
    int h, g, l, r;
    unsigned long *os, *oe;

    if (s < e) {
        g = OBJECT_z[s + (e - s) / 2];
        l = s;
        r = e;
        
        while (l < r)
        {
            /*
             * This quick sort is descending, so that the largest Z coordinate is drawn first.
             * For an ascending sort, simply reverse the < > signs in the next 2 lines 
             */
            while ( (OBJECT_z[l] > g) && (l <= e) ) l++;
            while ( (OBJECT_z[r] < g) && (r >= s) ) r--;

            if (l <= r)
            {
                h = OBJECT_z[l];
                OBJECT_z[l] = OBJECT_z[r];
                OBJECT_z[r] = h;

                os = OBJECT_start[l];
                OBJECT_start[l] = OBJECT_start[r];
                OBJECT_start[r] = os;

                oe = OBJECT_end[l];
                OBJECT_end[l] = OBJECT_end[r];
                OBJECT_end[r] = oe;

                l++;
                r--;
            }
        }
        
        object_quick_sort(s, r);
        object_quick_sort(l, e);
    }
}
