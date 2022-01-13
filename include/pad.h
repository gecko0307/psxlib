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

#ifndef _PAD_H_
#define _PAD_H_

#define PAD_L2       (1 << 0)
#define PAD_R2       (1 << 1)
#define PAD_L1       (1 << 2)
#define PAD_R1       (1 << 3)
#define PAD_TRIANGLE (1 << 4)
#define PAD_CIRCLE   (1 << 5)
#define PAD_CROSS    (1 << 6)
#define PAD_SQUARE   (1 << 7)
#define PAD_SELECT   (1 << 8)
#define PAD_START    (1 << 11)
#define PAD_UP       (1 << 12)
#define PAD_RIGHT    (1 << 13)
#define PAD_DOWN     (1 << 14)
#define PAD_LEFT     (1 << 15)

void padInit(void);
unsigned long padRead(void);
unsigned long padRead2(void);
void padStop(void);
void padWaitVSync(void);

#endif
