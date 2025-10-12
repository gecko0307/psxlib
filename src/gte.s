.text

.global gte_enable
.global gte_rtps2
.global gte_rtps

gte_enable:
    mfc0   $8, $12             # t0 = SR
    nop
    nop
    ori    $8, $8, 0x40000000  # Set bit 30
    mtc0   $8, $12             # SR = t0
    nop
    nop
    jr $31
    nop

# Rotate-translate-perspective transform
# - a0 ($4) = Mat3*
# - a1 ($5) = Trans*
# - a2 ($6) = Vertex*
# - a3 ($7) = Proj*
# returns v0 = SXY2, v1 = SZ3
gte_rtps2:
    # Load rotation matrix from $a0
    lh      $8,  0($4)       # $t0 = $a0[0]
    lh      $9,  2($4)       # $t1 = $a0[1]
    sll     $8,  $8, 16      # $t0 << 16
    or      $8,  $8, $9      # $t0 = $t0 | $t1
    ctc2    $8,  $0          # R11R12 = $t0
    
    lh      $8,  4($4)       # $t0 = $a0[2]
    lh      $9,  6($4)       # $t1 = $a0[3]
    sll     $8,  $8, 16      # $t0 << 16
    or      $8,  $8, $9      # $t0 = $t0 | $t1
    ctc2    $8,  $1          # R13R21 = $t0
    
    lh      $8,  8($4)       # $t0 = $a0[4]
    lh      $9, 10($4)       # $t1 = $a0[5]
    sll     $8,  $8, 16      # $t0 << 16
    or      $8,  $8, $9      # $t0 = $t0 | $t1
    ctc2    $8,  $2          # R22R23 = $t0
    
    lh      $8, 12($4)       # $t0 = $a0[6]
    lh      $9, 14($4)       # $t1 = $a0[7]
    sll     $8,  $8, 16      # $t0 << 16
    or      $8,  $8, $9      # $t0 = $t0 | $t1
    ctc2    $8,  $3          # R31R32 = $t0
    
    lh      $8, 16($4)       # $t0 = $a0[8]
    sll     $8,  $8, 16      # $t0 << 16
    ctc2    $8,  $4          # R33 = = $t0

    # Load translation vector from $a1
    lw      $8,  0($5)       # $t0 = $a1.x
    ctc2    $8,  $5          # TRX = $t0
    lw      $8,  4($5)       # $t0 = $a1.y
    ctc2    $8,  $6          # TRY = $t0
    lw      $8,  8($5)       # $t0 = $a1.z
    ctc2    $8,  $7          # TRZ = $t0
    
    # load vertex (VX, VY, VZ)
    lh      $8, 0($6)        # $t0 = $a2.x
    lh      $9, 2($6)        # $t1 = $a2.y
    sll     $8, $8, 16       # $t0 << 16
    or      $8, $8, $9       # $t0 = $t0 | $t1
    mtc2    $8, $0           # VXY0 = $t0
    lh      $8, 6($6)        # $t0 = $a2.z
    mtc2    $8, $1           # VZ0 = $t0
    
    # Projection plane (H)
    lh      $8, 0($7)        # $t0 = $a3.h
    ctc2    $8, $26          # H = $t0
    
    # OFX (1.0 = 0x10000)
    lw      $8, 4($7)        # $t0 = $a3.ofx
    ctc2    $8, $24          # OFX = $t0
    
    # OFY (1.0 = 0x10000)
    lw      $8, 8($7)        # $t0 = $a3.ofy
    ctc2    $8, $25          # OFY = $t0
    
    # DQA, DQB
    lh      $8, 12($7)       # $t0 = $a3.dqa
    ctc2    $8, $27          # DQA = $t0
    lh      $8, 16($7)       # $t0 = $a3.dqb
    ctc2    $8, $28          # DQB = $t0
    
    # Run RTPS
    cop2    0x00180001
    nop
    nop
    
    # Read screen coords from SXY2
    mfc2    $2, $14          # v0 = SX | SY
    nop
    nop
    mfc2    $3, $19          # v1 = SZ
    nop
    nop
    
    # Return
    jr      $31
    nop
