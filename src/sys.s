.text

.global printf
.global pad_init
.global pad_stop

printf:
    li $9, 0x3f
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

pad_init:
    li $9, 0x15
    li $10, 0xb0
    jr $10
    nop
    jr $31
    nop

pad_stop:
    li $9, 0x14
    li $10, 0xb0
    jr $10
    nop
    jr $31
    nop
