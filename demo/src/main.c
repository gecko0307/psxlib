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

#include "psxlib.h"
#include "cd.h"

// Two OTs (one for each buffer), each 2000 bytes long
unsigned char orderingTable[2][2000];

int spriteFromImage(GpuSprite* s, GpuImage* img, int x, int y, int u, int v, int w, int h)
{
    s->x = x;
    s->y = y;
    s->w = w;
    s->h = h;
    s->clutid = img->clutid;
    s->tpage = img->tpage;
    s->u = u;
    s->v = v;
    s->mx = 0;
    s->my = 0;
    s->r = 128;
    s->g = 128;
    s->b = 128;
    s->scalex = 1;
    s->scaley = 1;
    s->rotate = 0;
    s->trans = 255;
    return 0;
}

// Vertex
Vertex vertex1 = {0,   0,  0};
Vertex vertex2 = {32,  0,  0};
Vertex vertex3 = {0,  32,  0};

RTPSTransform rtpsTransform = {
    0, 0, 0,
    {
        F_ONE,  0x0000, 0x0000,
        0x0000, F_ONE,  0x0000,
        0x0000, 0x0000, F_ONE
    },
    512,
    0x10000 * 160, // half screen width
    0x10000 * 128, // half screen height
    F_ONE,
    0x0000
};

SVertex svertex = {0, 0, 0};

int main(void)
{
    int activeBuffer;
    int frame = 0;
    int numFrames = 6;
    float animTimer = 0.0f;
    unsigned long padBtn;
    short test;
    short sx, sy, sz;
    float fsx, fsy;
    int x, y, res;
    
    GpuImage img;
    GpuSprite sprite;
    Data imgData;
    
    GpuPoly3 poly;
    poly.r = 255;
    poly.g = 255;
    poly.b = 255;
    poly.p = 255;
    poly.x0 = 0;  poly.y0 = 0;
    poly.x1 = 32; poly.y1 = 0;
    poly.x2 = 32; poly.y2 = 32;
    poly.trans = 255;
    
    cdInit();
    gpuInit(320, 256, VMODE_PAL, 1, 1, 0);
    padInit();
    
    cdRead("\\IMG.TIM;1", &imgData);
    gpuImageGetInfo((TIMHeader*)(imgData.ptr), &img);
    gpuImageUpload(&img);
    dataFree(&imgData);
    
    spriteFromImage(&sprite, &img, 20, 50, 0, 0, 32, 32);
    
    printf("numFrames: %d\n", numFrames);
    
    gteEnable();
    
    gteRTPS(&rtpsTransform, &vertex1, &svertex);
    poly.x0 = svertex.x; poly.y0 = svertex.y;
    printf("%hi, %hi\n", svertex.x, svertex.y);
    
    gteRTPS(&rtpsTransform, &vertex2, &svertex);
    poly.x1 = svertex.x; poly.y1 = svertex.y;
    printf("%hi, %hi\n", svertex.x, svertex.y);
    
    gteRTPS(&rtpsTransform, &vertex3, &svertex);
    poly.x2 = svertex.x; poly.y2 = svertex.y;
    printf("%hi, %hi\n", svertex.x, svertex.y);
    
    /*
    gteResult = gte_rtps_test();
    printf("gteResult = 0x%08X\n", gteResult);
    sx = gteResult & 0xFFFF;         // Screen X
    sy = (gteResult >> 16) & 0xFFFF; // Screen Y
    printf("SX = %hi\n", sx);
    printf("SY = %hi\n", sy);
    
    sx = 0x0800; // 0.5
    sy = 0x2000; // 2.0
    r = (short)(((int)sx * (int)sy) >> 12); // 0x1000 = 1.0
    printf("Div = 0x%04X\n", r);
    
    sx = 0x2000; // 2.0
    sy = 0x4000; // 4.0
    r = (short)(((int)sx << 12) / (int)sy); // 0x0800 = 0.5
    printf("Mult = 0x%04X\n", r);
    
    x = 0x20000; // 0.5
    y = 0x40000; // 2.0
    res = (x << 16) / y; // 0x08000 = 1.0
    printf("res = %i\n", res);
    */
    
    while(1)
    {
        activeBuffer = gpuGetActiveBuffer();
        gpuSetWorkBase(&orderingTable[activeBuffer][0]);
        gpuSortClear(50, 0, 50);
        gpuPrintStr(0, 20, 20, "Hello World");

        animTimer += 0.33f;
        if (animTimer >= 1.0f)
        {
            animTimer = 0.0f;
            frame += 1;
            if (frame == numFrames)
                frame = 0;
        }
        
        padBtn = padRead();
        
        if (padBtn == PAD_RIGHT)
        {
            sprite.x = 50;
        }
        else if (padBtn == PAD_LEFT)
        {
            sprite.x = 20;
        }
        
        sprite.u = 32 * frame;
        gpuSortSprite(&sprite);
        
        rtpsTransform.tz += 1;
        
        gteRTPS(&rtpsTransform, &vertex1, &svertex);
        poly.x0 = svertex.x; poly.y0 = svertex.y;
        gteRTPS(&rtpsTransform, &vertex2, &svertex);
        poly.x1 = svertex.x; poly.y1 = svertex.y;
        gteRTPS(&rtpsTransform, &vertex3, &svertex);
        poly.x2 = svertex.x; poly.y2 = svertex.y;
        
        gpuSortPoly3(&poly);
        
        gpuSwapBuffers();
        gpuDraw((unsigned long*)&orderingTable[activeBuffer][0]);
        padWaitVSync();
    }
    
    return 0;
}
