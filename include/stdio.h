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

#ifndef _STDIO_H_
#define _STDIO_H_

/**
 * Print formatted string to the TTY console.
 *
 * The string can contain C-style escape codes:
 *   %c         - ASCII character
 *   %s         - ASCII string
 *   %i, %d, %D - signed decimal (d/i=default32bit, D=force32bit)
 *   %u, %U     - unsigned decimal (u=default32bit, U=force32bit)
 *   %o, %O     - unsigned octal (o=default32bit, O=force32bit)
 *   %p, %x, %X - unsigned hex (p=lower/force32bit, x=lower, X=upper)
 *   %n         - 32bit/16bit string length (default32bit)
 *
 * Additionally, following prefixes (inserted between "%" and escape code):
 *   + or SPC   - show leading plus or space character in positive signed numbers
 *   NNN        - fixed width (for padding or so) (first digit must be 1..9) (not 0)
 *   .NNN       - fixed width (for clipping or so)
 *   *          - variable width (using one of the parameters) (negative=ending_spc)
 *   .*         - variable width
 *   -          - force ending space padding (in case of width being specified)
 *   #          - show leading "0x" or "0X" (hex), or ensure 1 leading zero (octal)
 *   0          - show leading zero's
 *   L          - unknown/no effect?
 *   h,l        - force 16bit (h=halfword), or 32bit (l=long/word)
 */
int printf(const char *fmt, ...);

#endif
