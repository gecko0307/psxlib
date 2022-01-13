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

unsigned long pad_buf = 0;
unsigned long pad_data = 0;

void padInit(void)
{
    pad_data = 0xffffffff;
    pad_init(0x20000001, &pad_buf);
}

unsigned long padRead(void)
{
    return (~pad_data);
}

unsigned long padRead2(void)
{
    return (~pad_data >> 16);
}

void padStop(void)
{
    pad_stop();
}

void padWaitVSync(void)
{
    volatile unsigned long *p = (volatile unsigned long *)&pad_buf;
    *p = 0x0000;
    while (*p == 0x0000) {};
    pad_data = *p;
}
