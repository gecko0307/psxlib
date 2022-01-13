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

int main(void)
{
    int activeBuffer;
    int frame = 0;
    int numFrames = 6;
    float animTimer = 0.0f;
    unsigned long padBtn;
    
    GpuImage img;
    GpuSprite sprite;
    Data imgData;
    
    cdInit();
    gpuInit(320, 256, VMODE_PAL, 1, 1, 0);
    padInit();
    
    cdRead("\\IMG.TIM;1", &imgData);
    gpuImageGetInfo((TIMHeader*)(imgData.ptr), &img);
    gpuImageUpload(&img);
    dataFree(&imgData);
    
    spriteFromImage(&sprite, &img, 20, 50, 0, 0, 32, 32);
    
    printf("numFrames: %d\n", numFrames);
    
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
        gpuSwapBuffers();
        gpuDraw((unsigned long*)&orderingTable[activeBuffer][0]);
        padWaitVSync();
    }
    
    return 0;
}
