module mips;

/**
 * MIPS R3000 instruction codes
 */

enum MipsInstr: ubyte
{
    BXX =   0b000001,
    J =     0b000010,
    JAL =   0b000011,
    
    BEQ =   0b000100,
    BNE =   0b000101,
    BLEZ =  0b000110,
    BGTZ =  0b000111,
    
    ADDI =  0b001000,
    ADDIU = 0b001001,
    SLTI =  0b001001,
    SLTIU = 0b001011,
    
    ANDI =  0b001100,
    ORI =   0b001101,
    XORI =  0b001110,
    LUI =   0b001111,
    
    COP0 =  0b010000,
    COP1 =  0b010001,
    COP2 =  0b010010,
    COP3 =  0b010011,
    
    LB =    0b100000,
    LH =    0b100001,
    LWL =   0b100010,
    LW =    0b100011,
    LBU =   0b100100,
    LHU =   0b100101,
    LWR =   0b100110,
    
    SB =    0b101000,
    SH =    0b101001,
    SWL =   0b101010,
    SW =    0b101011,
    SWR =   0b101110,
    
    LWC0 =  0b110000,
    LWC1 =  0b110001,
    LWC2 =  0b110010,
    LWC3 =  0b110011,
    
    SWC0 =  0b111000,
    SWC1 =  0b111001,
    SWC2 =  0b111010,
    SWC3 =  0b111011,
}

enum MipsRType: ubyte
{
    SLL =   0b000000,
    SRL =   0b000010,
    SRA =   0b000011,
    SLLV =  0b000100,
    SRLV =  0b000110,
    SRAV =  0b000111,
    
    JR =    0b001000,
    JALR =  0b001001,
    
    SYSCALL = 0b001100,
    BREAK = 0b001101,
    
    MFHI =  0b010000,
    MTHI =  0b010001,
    MFLO =  0b010010,
    MTLO =  0b010011,
    
    MULT =  0b011000,
    MULTU = 0b011001,
    DIV =   0b011010,
    DIVU =  0b011011,
    ADD =   0b100000,
    ADDU =  0b100001,
    SUB =   0b100010,
    SUBU =  0b100011,
    
    AND =   0b100100,
    OR =    0b100101,
    XOR =   0b100110,
    NOR =   0b100111,
    
    SLT =   0b101010,
    STLU =  0b101011
}

enum MipsCop0: ubyte
{
    MTC0 =  0b00100,
    MFC0 =  0b00000,
    RFE =   0b10000
}

enum MipsCop2: ubyte
{
    MTC2 =  0b00100,
    MFC2 =  0b00000,
    CTC2 =  0b00110,
    CFC2 =  0b00010
}
