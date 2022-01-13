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

#include "types.h"
#include <libcd.h>
#include "cd.h"

/*
 * CD-ROM helper functions (depend on libcd from Psy-Q)
 */

void cdInit(void)
{
    // 1024KB (1,048,576 Bytes)
    InitHeap3((void*)0x800F8000, 0x00100000);

    // Init CD
    CdInit();
}

int roundMult(int num, int factor)
{
    return num + factor - 1 - (num - 1) % factor;
}

int cdRead(char* filename, Data* data)
{
    //Data data;
    CdlFILE file;
    CdlFILE* fp;
    int len;

    // Read the file, if it exists
    fp = CdSearchFile(&file, filename);
    if (fp)
    {
        len = roundMult(file.size, 2048);
        data->length = file.size;
        data->ptr = (char*)malloc3(len);
        CdReadFile(filename, (unsigned long*)data->ptr, 0);
        while(CdReadSync(1, NULL) > 0) { /* TODO: do something while loading */ }
        return 1;
    }

    return 0;
}

int dataFree(Data* data)
{
    free3(data->ptr);
    data->length = 0;
    return 1;
}
