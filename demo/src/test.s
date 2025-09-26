.text
.global gte_rtps_test

# GTE RTPS Test
#
# Input:
#   R - 1,3,12 (1 bit sign, 3 bits integer, 12 bits fraction, 1.0 = 2^12 = 0x1000).
#   TR - plain 32-bit signed
#   H - plain 16-bit unsigned
#   OFX, OFY - 1,15,16 (1 bit sign, 15 bits integer, 16 bits fraction, 1.0 = 2^16 = 0x10000).
#   DQA - 1,7,8 (1 bit sign, 7 bit integer, 8 bit fraction, 1.0 = 2^8 = 0x100).
#   DQB- 1,7,8 (1 bit sign, 7 bit integer, 8 bit fraction, 1.0 = 2^8 = 0x100).
#
# Output:
#   SXY2 Screen XY coordinates - 1,15,0
#   SZ3 Screen Z coordinate - 0,16,0

gte_rtps_test:
    # R = identity matrix (1.0 = 0x1000)
    li     $8, 0x10000000  # t0 = (1.0, 0.0)
    ctc2   $8, $0          # R11 = 1.0, R12 = 0.0
    ctc2   $0, $1          # R13 = 0.0, R21 = 0.0
    ctc2   $8, $2          # R22 = 1.0, R23 = 0.0
    ctc2   $0, $3          # R31 = 0.0, R32 = 0.0
    ctc2   $8, $4          # R33 = 1.0

    # TR = (0, 0, 0), plain 32-bit integers
    ctc2    $0, $5
    ctc2    $0, $6
    ctc2    $0, $7
    
    # Projection plane (H)
    li      $8, 512            # t0 = 512
    ctc2    $8, $26            # H = t0
    
    # DQA (1.0 = 0x100), DQB (1.0 = 0x1000000)
    li      $8, 0x100          # t0 = 1.0
    ctc2    $8, $27            # DQA = t0
    ctc2    $0, $28            # DQB = 0
    
    # OFX/OFY (1.0 = 0x10000)
    li      $8, 0x1400000            # OFX = 320
    ctc2    $8, $24
    li      $8, 0xF00000             # OFY = 240
    ctc2    $8, $25
    
    # V0 = (1.0, 1.0, 1.0)
    li      $9,  0x00010001    # Y = 1.0, X = 1.0
    mtc2    $9,  $0            # VXY0 = X,Y
    li      $10, 0x0001        # Z = 1.0
    mtc2    $10, $1            # VZ0 = Z
    
    # Run RTPS
    cop2    0x00180001
    nop
    nop
    
    # Read screen coords from SXY2
    mfc2    $2, $14            # v0 = SX | SY
    nop
    nop
    
    # Return
    jr      $31
    nop
