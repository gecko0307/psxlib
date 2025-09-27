module disasm;

import std.stdio;
import mips;

void disasmMIPSCode(uint[] data)
{
    foreach(instr; data)
    {
        ubyte opcode = cast(ubyte)(instr >> 26);
        if (opcode == 0) // R-type
        {
            ubyte funct = cast(ubyte)(instr & 0x3f);
            ubyte rs    = cast(ubyte)((instr >> 21) & 0x1f);
            ubyte rt    = cast(ubyte)((instr >> 16) & 0x1f);
            ubyte rd    = cast(ubyte)((instr >> 11) & 0x1f);
            ubyte shift = cast(ubyte)((instr >> 6)  & 0x1f);
            
            if (funct == MipsRType.JR)
            {
                writefln("  0x%08x: %s $%s", instr, cast(MipsRType)funct, rs);
            }
            else
            {
                if (funct == MipsRType.SLL && rs == 0 && rt == 0 && rd == 0 && shift == 0)
                {
                    writefln("  0x%08x: NOP", instr);
                }
                else
                {
                    writefln("  0x%08x: %s $%s, $%s, %d($%s)", instr, cast(MipsRType)funct, rd, rs, shift, rt);
                }
            }
        }
        else if (opcode == MipsInstr.J || opcode == MipsInstr.JAL)
        {
            uint target = instr & 0x03ffffff;
            writefln("  0x%08x: %s 0x%08x", instr, cast(MipsInstr)opcode, target);
        }
        else if (opcode == MipsInstr.COP0)
        {
            ubyte op = cast(ubyte)((instr >> 21) & 0x1f);
            if (op == MipsCop0.MTC0 || op == MipsCop0.MFC0)
            {
                ubyte rt = cast(ubyte)((instr >> 16) & 0x1f);
                ubyte rd = cast(ubyte)((instr >> 11) & 0x1f);
                
                writefln("  0x%08x: %s $%s, $%s", instr, cast(MipsCop0)op, rt, rd);
            }
            else
            {
                writefln("  0x%08x: %s", instr, cast(MipsInstr)opcode);
            }
        }
        else if (opcode == MipsInstr.COP2)
        {
            ubyte op = cast(ubyte)((instr >> 21) & 0x1f);
            if (op == MipsCop2.MTC2 || op == MipsCop2.MFC2 ||
                op == MipsCop2.CTC2 || op == MipsCop2.CFC2)
            {
                ubyte rt = cast(ubyte)((instr >> 16) & 0x1f);
                ubyte rd = cast(ubyte)((instr >> 11) & 0x1f);
                
                writefln("  0x%08x: %s $%s, $%s", instr, cast(MipsCop2)op, rt, rd);
            }
            else
            {
                writefln("  0x%08x: %s", instr, cast(MipsInstr)opcode);
            }
        }
        else // I-type
        {
            ubyte rs    = cast(ubyte)((instr >> 21) & 0x1f);
            ubyte rt    = cast(ubyte)((instr >> 16) & 0x1f);
            short imm   = cast(short)(instr & 0xffff);
            
            if (opcode == MipsInstr.LUI)
            {
                writefln("  0x%08x: %s $%s, 0x%04x", instr, cast(MipsInstr)opcode, rt, imm);
            }
            else
            {
                writefln("  0x%08x: %s $%s, $%s, 0x%04x", instr, cast(MipsInstr)opcode, rt, rs, imm);
            }
        }
    }
}
