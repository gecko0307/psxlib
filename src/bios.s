.text

.global open
.global printf
.global pad_init
.global pad_stop

open:
    li $9, 0x00
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

lseek:
    li $9, 0x01
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

read:
    li $9, 0x02
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

write:
    li $9, 0x03
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

close:
    li $9, 0x04
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

ioctl:
    li $9, 0x05
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

exit:
    li $9, 0x06
    li $10, 0xa0
    jr $10
    nop
    jr $31
    nop

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
