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

#include "gte.h"

void gteEnable(void)
{
    asm volatile ("
        mfc0   $8, $12             # t0 = SR
        nop
        ori    $8, $8, 0x40000000  # Set bit 30
        mtc0   $8, $12             # SR = t0
        nop
        addiu  $8, $0, 0x555       # ZSF3 - average scale factor (0x1000/3)
        ctc2   $8, $29
        nop
    ");
}

void gteRTPS(RTPSTransform* rtpsTransform, Vertex* inVertex, SVertex* outVertex)
{
    asm volatile ("
        # Load translation vector (a0->tx, a0->ty, a0->tz)
        and     $8,  $8, $0      # t0 = 0
        lw      $8,  0($4)       # t0 = a0->tx
        ctc2    $8,  $5          # TRX = $t0
        lw      $8,  4($4)       # t0 = a0->ty
        ctc2    $8,  $6          # TRY = $t0
        lw      $8,  8($4)       # t0 = a0->tz
        ctc2    $8,  $7          # TRZ = t0
        nop
        nop

        # Load rotation matrix from ($a0->r)
        lh      $8,  12($4)      # t0 = a0[12]
        lh      $9,  14($4)      # t1 = a0[14]
        andi    $8,  $8, 0xFFFF
        andi    $9,  $9, 0xFFFF
        sll     $9,  $9, 16      # t1 = t1 << 16
        or      $8,  $8, $9      # t0 = t0 | t1
        ctc2    $8,  $0
        nop
        nop
        
        lh      $8,  16($4)      # t0 = a0[16]
        lh      $9,  18($4)      # t1 = a0[18]
        andi    $8,  $8, 0xFFFF
        andi    $9,  $9, 0xFFFF
        sll     $9,  $9, 16      # t1 = t1 << 16
        or      $8,  $8, $9      # t0 = t0 | t1
        ctc2    $8,  $1          # R13R21 = t0
        nop
        nop
        
        lh      $8,  20($4)      # t0 = a0[20]
        lh      $9,  22($4)      # t1 = a0[22]
        andi    $8,  $8, 0xFFFF
        andi    $9,  $9, 0xFFFF
        sll     $9,  $9, 16      # t1 = t1 << 16
        or      $8,  $8, $9      # t0 = t0 | t1
        ctc2    $8,  $2          # R22R23 = t0
        nop
        nop
        
        lh      $8,  24($4)      # t0 = a0[24]
        lh      $9,  26($4)      # t1 = a0[26]
        andi    $8,  $8, 0xFFFF
        andi    $9,  $9, 0xFFFF
        sll     $9,  $9, 16      # t1 = t1 << 16
        or      $8,  $8, $9      # t0 = t0 | t1
        ctc2    $8,  $3          # R31R32 = t0
        nop
        nop
        
        lh      $8,  28($4)      # t0 = a0[28]
        andi    $8,  $8, 0xFFFF
        ctc2    $8,  $4          # R33 = t0
        nop
        nop
        
        # Projection plane (H)
        and     $8,  $8, $0      # t0 = 0
        lh      $8,  30($4)      # t0 = a0.h
        ctc2    $8,  $26         # H = t0
        
        # OFX (1.0 = 0x10000)
        and     $8,  $8, $0       # t0 = 0
        lw      $8,  32($4)       # t0 = a0.ofx
        ctc2    $8,  $24          # OFX = t0
        
        # OFY (1.0 = 0x10000)
        and     $8,  $8, $0       # t0 = 0
        lw      $8,  36($4)       # t0 = a0.ofy
        ctc2    $8,  $25          # OFY = t0
        
        # DQA, DQB
        and     $8,  $8, $0       # t0 = 0
        lh      $8,  40($4)       # t0 = a0.dqa
        ctc2    $8,  $27          # DQA = t0
        lh      $8,  42($4)       # t0 = a0.dqb
        ctc2    $8,  $28          # DQB = t0
        
        # load vertex (VX, VY, VZ)
        and     $8,  $8, $0       # t0 = 0
        and     $9,  $9, $0       # t1 = 0
        lh      $8, 2($5)         # t0 = a1.y
        lh      $9, 0($5)         # t1 = a1.x
        sll     $8, $8, 16        # t0 = t0 << 16
        or      $8, $8, $9        # t0 = t0 | t1
        mtc2    $8, $0            # VXY0 = t0
        and     $8, $8, $0        # t0 = 0
        lh      $8, 4($5)         # t0 = a1.z
        mtc2    $8, $1            # VZ0 = t0
        nop
        nop
        
        # Run RTPS
        cop2    0x00180001
        nop
        nop
        
        # Read screen coords from SXY2
        mfc2    $8, $14          # t0 = SX | SY
        nop
        nop
        sh      $8, 0($6)        # a2.x = t0
        nop
        srl     $8, $8, 16       # t0 = t0 >> 16
        sh      $8, 2($6)        # a2.y = t0
        nop
        
        andi    $8, 0            # t0 = 0
        mfc2    $8, $19          # t0 = SZ3
        nop
        nop
        sh      $8, 4($6)        # a2.z = t0
        nop
    ");
}
