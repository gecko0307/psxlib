# ASPSX Assembler Guide

Built-in assembler that is used in CCPSX toolchain.

## Syntax Rules

- Number beginning with `0x` is a hex value.
- Number beginning with `$` is a register.
- Line beginning with `#` is a comment.
- `.byte` is equivalent to `db` on other assemblers.

## Directives

### .text
Starts the actual program code, sometimes called a "text segment" or "code segment".

### .global <symbol>
Makes the following symbol visible to the linker. Otherwise, symbol is lost by link time. All symbols that should be available to the C code should be marked with `.global`.

## Registers

PlayStation R3000A has 32 general purpose registers:
- `$0` - `r0` - Constant (always 0)
- `$1` - `at` - Assembler temporary (destroyed by some assembler pseudoinstructions)
- `$2..$3` - `v0..v1` - Subroutine return values, may be changed by subroutines
- `$4..$7` - `a0..a3` - Subroutine arguments, may be changed by subroutines
- `$8..$15` - `t0..t7` - Temporaries, may be changed by subroutines.
- `$16..$23` - `s0..s7` - Static variables, must be saved by subroutines
- `$24..$25` - `t8..t9` - Temporaries, may be changed by subroutines
- `$26..$27` - `k0..k1` - Reserved for kernel (destroyed by some IRQ handlers). Do not modify
- `$28` - `gp` - Global pointer. Do not modify
- `$29` = `$sp` - Stack pointer. Points to the first free address on the stack.
- `$30` - `fp`/`s8` - Frame Pointer, or 9th Static variable, must be saved. Do not modify
- `$31` - `ra` - Return address. Jumping to this address returns from a subroutine.

Special registers:
- `hi`, `lo` - multiply/divide results, may be changed by subroutines
- `pc` - Program counter.

## O32 ABI
- `$2..$3` (`v0..v1`) - return values
- `$4..$7` (`a0..a3`) - function arguments
- `$29` - stack pointer
- `$31` - return address

If there are more than 4 32-bit arguments, 5th argument and others are placed on the stack.

## R3000 Instructions

### Arithmetic:
- `add $r, $a, $b` - Add. `$r = $a + $b`
- `addu $r, $a, $b` - Add Unsigned. `$r = $a + $b`
- `addi $r, $a, imm` - Add Immediate. `$r = $a + imm`
- `addiu $r, $a, imm` - Add Immediate Unsigned. `$r = $a + imm`
- `sub $r, $a, $b` - Subtract. `$r = $a - $b`
- `subu $r, $a, $b` - Subtract Unsigned. `$r = $a - $b`
- `mult $a, $b` - Multiply. `$a * $b`
- `multu $a, $b` - Multiply Unsigned. `$a * $b`

Unlike most instructions, `mult` doesn't say where to store the result. The product of two 32-bit integers is always 64-bit, and the lower and upper 32 bits of the result are stored separately in `lo` and `hi` special registers. To access them, use the `mflo` and `mfhi`:

`mult $2, $3`
`mflo $4` - the lower part of the product
`mfhi $5` - the upper part of the product

Mathematically: `$2 * $3 = ($5 << 32) | $4`

- `div $a, $b` - Divide. `$a / $b`
- `divu $a, $b` - Divide Unsigned. `$a / $b`

The result of division is stored separately as the quotient and the remainder. The quotient `$a / $b` is stored in `lo` special register. The remainder `$a % $b` is stored in `hi` special register. To access them, use the `mflo` and `mfhi`:

`div $2, $3`
`mflo $4` - the quotient of the division
`mfhi $5` - the remainder of the division

Mathematically: `$2 / $3 = $4 + 1/$5`

- `mflo $r` - Move From Low. Copy from the `lo` special register to the given destination register.
- `mfhi $r` - Move From High. Copy from the `hi` special register to the given destination register.

### Comparison:
- `slt $r, $a, $b` - Set on Less Than Signed. `$r = (int)$a < (int)$b`
- `slti $r, $a, imm` - Set on Less Than Immediate Signed. `$r = (int)$a < imm`
- `sltiu $r, $a, imm` - Set if Less Than Immediate Unsigned. `$r = $a < imm`

### Bitwise:
- `and $r, $a, $b` - Logical AND. `$r = $a & $b`
- `andi $r, $a, imm` - Logical AND Immediate. `$r = $a & imm`
- `or $r, $a, $b` - Logical OR. `$r = $a | $b`
- `ori $r, $a, imm` - Logical OR Immediate. `$r = $a | imm`
- `xor $r, $a, $b` - Exclusive OR. `$r = $a ^ $b`
- `nor $r, $a, $b` - Negative OR. `$r = !($a | $b)`
- `sll $r, $a, imm` - Shift Left Logical Immediate. `$r = $a << imm`
- `sllv $r, $a, $b` - Shift Left Logical Variable. Shift amount is masked to 5 bits. `$r = $a << ($b & 0x1f)`
- `slr $r, $a, imm` - Shift Right Logical Immediate. Upper bits of the result are filled with zeroes. `$r = $a >> imm`
- `srlv $r, $a, $b` - Shift Right Logical Variable. Upper bits of the result are filled with zeroes. Shift amount is masked to 5 bits. `$r = $a >> ($b & 0x1f)`
- `sra $r, $a, imm` - Shift Right Arithmetic Immediate. Upper bits of the result are filled with the sign bit of the input. `$r = (int)$a >> imm`
- `srav $r, $a, $b` - Shift Right Arithmetic Variable. Upper bits of the result are filled with the sign bit of the input. Shift amount is masked to 5 bits. $r = (int)$a >> ($b & 0x1f)`

### Memory:
Warning: memory instructions require aligned addresses.
- word (32 bits = 4 bytes) - requires address divisible by 4. The hex value of the address must end with 0, 4, 8, or C.
- half-word (16 bits = 2 bytes) - requires address divisible by 2. The hex value of the address must end with one of: 0, 2, 4, 6, 8, A, C, E.
- byte (8 bits) - no alignment requirements.

- `lw $r, imm($a)` - Load Word. `$r = $a + imm`
- `lh $r, imm($a)` - Load Half-Word. `$r = $a + imm`
- `lhu $r, imm($a)` - Load Half-Word Unsigned. `$r = $a + imm`
- `lb $r, imm($a)` - Load Byte. `$r = $a + imm`
- `lbu $r, imm($a)` - Load Byte Unsigned. `$r = $a + imm`
- `lwl $r, imm($a)` - Load Word Left
- `lwr $r, imm($a)` - Load Word Right
Used for unaligned loads (where you want a 4-byte word, but the address is not divisible by 4). `lwr` takes the first byte to load, while `lwl` takes the last byte.

Example: Assume `$7` is aligned. To read the 4 bytes starting from `$7 + 0x4e`:

```
lwl $2, 0x0051($7)
lwr $2, 0x004e($7)
```

- `sw $r, imm($a)` - Store Word. `$a + imm = $r`
- `sh $r, imm($a)` - Store Half-Word. `$a + imm = $r`
- `sb $r, imm($a)` - Store Byte. `$a + imm = $r`
- `lui $r, imm` - Load Upper Immediate. Loads the given 16-bit constant into the upper 16 bits of the given register. Not a memory access, but often used with memory access instructions.

### Control:
- `j imm` - Jump. Jumps to target constant address `imm`:

```
pc = imm
```

- `jr $r` - Jump Register. Jumps to the address stored in given register:

```
pc = $r
```

- `jal imm` - Jump And Link. Jumps to target constant address `imm`, storing return address in `$31`:

```
$31 = pc + 8
pc = imm
```

- `jalr $r` - Jump and Link Register. Like jal, but jumps to the address in the given register instead of a constant. One-operand form assumes storing return address in `$31`.

```
$31 = pc + 8
pc = $r
```

- `jalr $r, $a` - Jump and Link Register. Two-argument form accepts the register to store the return address in:

```
$a = pc + 8
pc = $r
```

- `beq $r, $a, imm` - Branch on Equal. Jumps to target constant address `imm`, if `$r == $a`:

```
if ($r == $a) {
    pc += 4 + (imm << 2)
}
```

- `bne $r, $a, imm` - Branch on Not Equal. Jumps to target constant address `imm`, if `$r != $a`:

```
if ($r = $a) {
    pc += 4 + (imm << 2)`
}
```

- `bgez $r, imm` - Branch if Greater Than or Equal to Zero. Jumps to target constant address `imm`, if `$r >= 0`:

```
if ($r >= 0) {
    pc += 4 + (imm << 2)`
}
```

- `bltz $r, imm` - Branch on Less Than Zero. Jumps to target constant address `imm`, if `$r < 0`.

```
if ($r < 0) {
    pc += 4 + (imm << 2)`
}
```

- `bgezal $r, imm` - Branch if Greater Than or Equal to Zero And Link. Same as `bgez`, but also stores return address in `$31`.

```
if ($r >= 0) {
    $31 = pc + 8
    pc += 4 + (imm << 2)
}
```

- `bltzal $r, imm` - Branch if Less Than Zero And Link. Same as `bltz`, but also stores return address in `$31`.

```
if ($r < 0) {
    $31 = pc + 8
    pc += 4 + (imm << 2)
}
```

### Pseudo-instructions:
- `li $r, imm` - Load Immediate. Loads a register with a value that is immediately available.

Expands into `lui` followed by `ori` if `imm` is larger than 16 bits:

```
lui $1, (imm & 0xffff)
ori $r, $1, (imm >> 16)
```

If `imm` is unsigned and fits into 16-bit, expands into single `ori`:

```
ori $r, $0, imm
```

If `imm` is signed and fits into 16-bit, expands into single `addiu` with zero first operand:

```
addiu $r, $0, imm
```

- `nop` - Same as `sll $0, $0, 0`.

## COP0 - System Control Coprocessor

COP0 registers:
- `$0` - N/A
- `$1` - N/A
- `$2` - N/A
- `$3` - BPC - Breakpoint on execute (R/W)
- `$4` - N/A
- `$5` - BDA - Breakpoint on data access (R/W)
- `$6` - JUMPDEST - Randomly memorized jump address (R)
- `$7` - DCIC - Breakpoint control (R/W)
- `$8` - BadVaddr - Bad Virtual Address (R)
- `$9` - BDAM - Data Access breakpoint mask (R/W)
- `$10` - N/A
- `$11` - BPCM - Execute breakpoint mask (R/W)
- `$12` - SR - System status register (R/W)
- `$13` - CAUSE - (R) Describes the most recently recognised exception
- `$14` - EPC - Return Address from Trap (R)
- `$15` - PRID - Processor ID (R)
- `$16-31` - Garbage
- `$32-63` - N/A

## COP2 - GTE

The GTE doesn't have any memory or I/O ports mapped to the CPU memory bus, instead, it's solely accessed via coprocessor opcodes (`cop2`). Before use, the GTE must be turned on. The GTE has bit 30 allocated to it in the status register of the system control coprocessor (`cop0`). Before any GTE instruction is used, this bit must be set:

```
mfc0   $8, $12             # t0 = SR
ori    $8, $8, 0x40000000  # Set bit 30 in t0 (CU2)
mtc0   $8, $12             # SR = t0
nop
```

- `mtc2 $r, $d` - Move To Coprocessor 2. Writes a register value to the GTE data register. `$d = $r`
- `mfc2 $r, $d` - Move From Coprocessor 2. Reads a value from the GTE register. `$r = $d`
- `ctc2 $r, $c` - writes a value to the GTE control register. `$c = $r`
- `cfc2 $r, $c` - reads a value from the GTE control register. `$r = $c`
- `lwc2 $d, imm(base)` - stores a value at imm(base) in GTE data register gd.
- `swc2 $d, imm(base)` - stores GTE data register at imm(base).
- `cop2 imm` - issues a GTE command

GTE load and store instructions have a delay of 2 instructions, for any GTE commands or operations accessing that register.

GTE data registers (accessed via `mfc2` / `mtc2`):
- `$0` - VXY0  - Vector0 X and Y, 2 packed signed 16-bit integers
- `$1` - VZ0   - Vector0 Z, signed 16-bit integer
- `$2` - VXY1  - Vector1 X and Y, 2 packed signed 16-bit integers
- `$3` - VZ1   - Vector1 Z, signed 16-bit integer
- `$4` - VXY2  - Vector2 X and Y, 2 packed signed 16-bit integers
- `$5` - VZ2   - Vector2 Z, signed 16-bit integer
- `$6` - RGBC  - Color/code value, 4 packed unsigned bytes
- `$7` - OTZ   - Average Z value (for Ordering Table), unsigned 16-bit integer
- `$8` - IR0   - 16-bit Accumulator (Interpolate), signed 16-bit integer
- `$9` - IR1   - 16-bit Accumulator (Vector X), signed 16-bit integer
- `$10` - IR2  - 16-bit Accumulator (Vector Y), signed 16-bit integer
- `$11` - IR3  - 16-bit Accumulator (Vector Z), signed 16-bit integer
- `$12` - SXY0 - Screen XY-coordinate FIFO stage1, 2 packed signed 16-bit integers
- `$13` - SXY1 - Screen XY-coordinate FIFO stage2, 2 packed signed 16-bit integers
- `$14` - SXY2 - Screen XY-coordinate FIFO stage3, 2 packed signed 16-bit integers
- `$15` - SXYP - Screen XY-coordinate FIFO stage4, 2 packed signed 16-bit integers
- `$16` - SZ0 - Screen Z-coordinate FIFO stage1, unsigned 16-bit integer
- `$17` - SZ1 - Screen Z-coordinate FIFO stage2, unsigned 16-bit integer
- `$18` - SZ2 - Screen Z-coordinate FIFO stage3, unsigned 16-bit integer
- `$19` - SZ3 - Screen Z-coordinate FIFO stage4, unsigned 16-bit integer
- `$20` - RGB0 - Color CRGB-code/color FIFO stage1, 4 packed unsigned bytes
- `$21` - RGB1 - Color CRGB-code/color FIFO stage2, 4 packed unsigned bytes
- `$22` - RGB2 - Color CRGB-code/color FIFO stage3, 4 packed unsigned bytes
- `$23` - undefined/prohibited
- `$24` - MAC0 - 32bit Maths Accumulators (Value), signed 32-bit integer
- `$25` - MAC1 - 32bit Maths Accumulators (Vector X), signed 32-bit integer
- `$26` - MAC2 - 32bit Maths Accumulators (Vector Y), signed 32-bit integer
- `$27` - MAC3 - 32bit Maths Accumulators (Vector Z), signed 32-bit integer
- `$28` - IRGB - Convert RGB Color (48bit vs 15bit), unsigned 16-bit integer
- `$29` - ORGB - ?
- `$30` - LZCS - Count Leading-Zeroes/Ones (sign bits), 2 packed signed 32-bit integers
- `$31` - LZCR - Count Leading-Zeroes/Ones (sign bits), 2 packed signed 32-bit integers

The SXYn, SZn and RGBn are first-in-first-out registers (FIFO). The last calculation result is stored in the last register, and previous results are stored in previous registers. So for example when a new SXY value is obtained the following happens:

SXY0 = SXY1
SXY1 = SXY2
SXY2 = SXYP
SXYP = result

GTE control registers (accessed via `cfc2` / `ctc2`):
- `$0`  - R11R12 - Rotation matrix 3x3 (r11, r12), 2 signed 16-bit integers
- `$1`  - R13R21 - Rotation matrix 3x3 (r13, r21), 2 signed 16-bit integers
- `$2`  - R22R23 - Rotation matrix 3x3 (r22, r23), 2 signed 16-bit integers
- `$3`  - R31R32 - Rotation matrix 3x3 (r31, r32), 2 signed 16-bit integers
- `$4`  - R33    - Rotation matrix 3x3 (r33), signed 16-bit integer
- `$5`  - TRX    - Translation vector X, signed 32-bit integer
- `$6`  - TRY    - Translation vector Y, signed 32-bit integer
- `$7`  - TRZ    - Translation vector Z, signed 32-bit integer
- `$8`  - L11L12 - Light source matrix 3x3 (l11, l12), 2 signed 16-bit integers
- `$9`  - L13L21 - Light source matrix 3x3 (l13, l21), 2 signed 16-bit integers
- `$10` - L22L23 - Light source matrix 3x3 (l22, l23), 2 signed 16-bit integers
- `$11` - L31L32 - Light source matrix 3x3 (l31, l32), 2 signed 16-bit integers
- `$12` - L33    - Light source matrix 3x3 (l33), signed 16-bit integer
- `$13` - RBK    - Background color R, unsigned 32-bit integer
- `$14` - GBK    - Background color G, unsigned 32-bit integer
- `$15` - BBK    - Background color B, unsigned 32-bit integer
- `$16` - LR1LR2 - Light color matrix source (lr1, lr2), 2 signed 16-bit integers
- `$17` - LR3LG1 - Light color matrix source (lr3, lg1), 2 signed 16-bit integers
- `$18` - LG2LG3 - Light color matrix source (lg2, lg3), 2 signed 16-bit integers
- `$19` - LB1LB2 - Light color matrix source (lb1, lb2), 2 signed 16-bit integers
- `$20` - LB3    - Light color matrix source (lb3), signed 16-bit integer
- `$21` - RFC    - Far color R, unsigned 32-bit integer
- `$22` - GFC    - Far color G, unsigned 32-bit integer
- `$23` - BFC    - Far color B, unsigned 32-bit integer
- `$24` - OFX    - Screen offset X, signed 32-bit integer
- `$25` - OFY    - Screen offset Y, signed 32-bit integer
- `$26` - H      - Projection plane distance, unsigned 16-bit integer
- `$27` - DQA    - Depth queing parameter A (coefficient), signed 16-bit integer
- `$28` - DQB    - Depth queing parameter B (offset), unsigned 32-bit integer
- `$29` - ZSF3   - Z-averaging scale factor, signed 16-bit integer
- `$30` - ZSF4   - Z-averaging scale factor, signed 16-bit integer
- `$31` - FLAG   - Flag (read-only), returns any calculation errors, 20 bits

GTE Command encoding:

31-25  Must be 0100101b for "COP2 imm25" instructions
20-24  Fake GTE Command Number (00h..1Fh) (ignored by hardware)
19     sf - Shift Fraction in IR registers (0=No fraction, 1=12bit fraction)
17-18  MVMVA Multiply Matrix    (0=Rotation. 1=Light, 2=Color, 3=Reserved)
15-16  MVMVA Multiply Vector    (0=V0, 1=V1, 2=V2, 3=IR/long)
13-14  MVMVA Translation Vector (0=TR, 1=BK, 2=FC/Bugged, 3=None)
11-12  Always zero                        (ignored by hardware)
10     lm - Saturate IR1,IR2,IR3 result (0=To -8000h..+7FFFh, 1=To 0..+7FFFh)
6-9    Always zero                        (ignored by hardware)
0-5    Real GTE Command Number (00h..3Fh) (used by hardware)

GTE Commands:

### RTPS
`cop2 0x0180001` - Rotate, Translate, Perpective Transformation of 1 point. The point is first multiplied with a rotation matrix (R), and after that translated (TR). Finally a perspective transformation is applied, which results in screen coordinates. It also returns an interpolation value to be used with the various depth cueing instructions.

Input:
- R - 1,3,12 (1 bit sign, 3 bits integer, 12 bits fraction, 1.0 = 2^12 = 0x1000).
- TR - 32-bit signed
- H - 16-bit unsigned
- OFX, OFY - 1,15,16 (1 bit sign, 15 bits integer, 16 bits fraction, 1.0 = 2^16 = 0x10000).
- DQA - 1,7,8 (1 bit sign, 7 bit integer, 8 bit fraction, 1.0 = 2^8 = 0x100).
- DQB- 1,7,8 (1 bit sign, 7 bit integer, 8 bit fraction, 1.0 = 2^8 = 0x100).

Output:
- SXY2 Screen XY coordinates - 16-bit signed
- SZ3 Screen Z coordinate - 16-bit unsigned

Z values are limited downwards at 0.5 * H. For smaller z values you'll have write your own routine.

### RTPT
`cop2 0x0280030` - Rotate, Translate, Perpective Transformation of 3 points.

### MVMVA
`cop2 0x0400012` - Matrix-Vector Multiplication and Addition. Multiplies a vector with either the rotation matrix, the light matrix or the color matrix and then adds the translation vector or background color vector.

### DCPL
`cop2 0x0680029` - Depth Cue Light Color. First calculates a color from a light vector (normal vector of a plane multiplied with the light matrix and zero limited) and a provided RGB value. Then performs depth cueing by interpolating between the far color vector and the newfound color.

### DPCS
`cop2 0x0780010` - Depth Cue Single/Triple. Performs depth cueing by interpolating between a color and the far color (on one color).

### DPCT
`cop2 0x0f8002a` - Depth Cue Single/Triple. Performs depth cueing by interpolating between a color and the far color (on three colors).

### INTPL
`cop2 0x0980011` - Interpolation. Interpolates between a vector and the far color.

### SQR
`cop2 0x0a00428` - Square. Calculates the square of a vector.

### NCS
`cop2 0x0c8041e` - Normal Color. Calculates a color from the normal of a point or plane and the light sources and colors. The basic color of the plane or point the normal refers to is assumed to be white.

### NCT
`cop2 0x0d80420` - 

### NCDS
`cop2 0x0e80413` - Normal Color Depth Cue. Same as `NCS` but also performs depth cueing (like `DPCS`).

### NCDT
`cop2 0x0f80416` - Normal Color Depth Cue. Same as `NCT` but also performs depth cueing (like `DPCT`).

### NCCS
`cop2 0x108041b` - Same as `NCS`, but the base color of the plane or point is taken into account.

### NCCT
`cop2 0x118043f` - Same as `NCT`, but the base color of the plane or point is taken into account.

### CDP
`cop2 0x1280414` - A color is calculated from a light vector (base color is assumed to be white) and depth cueing is performed (like DPCS).

### CC
`cop2 0x138041c` - A color is calculated from a light vector and a base color.

### NCLIP
`cop2 0x1400006` - Normal Clipping. Calculates the outer product of three 2D points (i.e. 3 vertices which define a plane after projection).

The 3 vertices should be stored clockwise according to the visual point:

   Z+
  /
 /____ X+
 |
 |
 Y+

If this is so, the result of this function will be negative if we are facing the backside of the plane.

### AVSZ3
`cop2 0x158002d` - Z-Averaging. Adds 3 Z values together and multplies them by a fixed point value. This value is normally chosen so that this function returns the average of the Z values (usually further divided by 2 or 4 for easy adding to the OT).

### AVSZ4
`cop2 0x168002e` - Z-Averaging. Adds 4 Z values together and multplies them by a fixed point value. This value is normally chosen so that this function returns the average of the Z values (usually further divided by 2 or 4 for easy adding to the OT).

### OP
`cop2 0x170000c` - Calculates the outer product of 2 vectors.

### GPF
`cop2 0x190003d` - Multiplies two vectors. Also returns the result as 24-bit RGB value.

### GPL
`cop2 0x1a0003e` - Multiplies a vector with a scalar and adds the result to another vector. Also returns the result as 24-bit RGB value.


## BIOS calls

TODO
