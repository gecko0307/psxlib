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

#ifndef _IO_H_
#define _IO_H_

/* Seek from start of file (with positive offset) */
#define SEEK_SET 0

/* Seek from current file pointer (with positive/negative offset) */
#define SEEK_CUR 1

/* Bugged! Should be from end of file */
#define SEEK_END 2

/**
 * Opens a file on the target device for IO. Accessmode is set like this:
 * bit0     1 = Read
 * bit1     1 = Write
 * bit2     1 = Exit without waiting for incoming data (when TTY buffer empty)
 * bit9     0 = Open Existing File, 1=Create New file (memory card only)
 * bit15    1 = Asynchronous mode (memory card only; don't wait for completion)
 * bit16-31 Number of memory card blocks for a new file on the memory card
 */
int open(char* name, int accessmode);

/**
 * Moves the file pointer by <offset> bytes, relative to the location specified by <origin>.
 * Movement from the eof is incorrect. Also, movement beyond the end of the file is not checked.
 */
int lseek(int handle, int offset, int origin);

int read(int fd, char* buf, int nbytes);

int write(int fd, char *buf, int nbytes);

/**
 * Close an open file. Returns $r2 = handle (or $r2 = -1 if failed).
 */
int close(int handle);

int ioctl(int fd, int cmd, int arg);

int exit(int exitcode);

#endif
