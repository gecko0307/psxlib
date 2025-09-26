.text

.global gte_enable

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
