module psyqobj;

import std.stdio;
import std.bitmanip;

import section;
import symbol;
import mips;

enum PsyqOpcode: ubyte
{
    END = 0,
    BYTES = 2,
    SWITCH = 6,
    ZEROES = 8,
    RELOCATION = 10,
    EXPORTED_SYMBOL = 12,
    IMPORTED_SYMBOL = 14,
    SECTION = 16,
    LOCAL_SYMBOL = 18,
    FILENAME = 28,
    PROGRAMTYPE = 46,
    UNINITIALIZED = 48,
    INC_SLD_LINENUM = 50,
    INC_SLD_LINENUM_BY_BYTE = 52,
    INC_SLD_LINENUM_BY_WORD = 54,
    SET_SLD_LINENUM = 56,
    SET_SLD_LINENUM_FILE = 58,
    END_SLD = 60,
    FUNCTION = 74,
    FUNCTION_END = 76,
    BLOCK_START = 78,
    BLOCK_END = 80,
    SECTION_DEF = 82,
    SECTION_DEF2 = 84,
    FUNCTION_START2 = 86,
};

class PsyqObject
{
    public:
    
    ubyte ver;
    ubyte programType = 0;
    Section[string] sectionsByName;
    Section[uint] sectionsByIndex;
    Symbol[string] symbolsByName;
    bool parsed = false;
    
    this(ubyte[] binary)
    {
        this.binary = binary;
        parsed = parse();
    }
    
    protected:
    
    ubyte[] binary;
    size_t index = 3;
    Section currentSection = null;
    
    ubyte readUByte()
    {
        ubyte b = binary[index];
        index++;
        return b;
    }
    
    ushort readUShort()
    {
        ushort u = *cast(ushort*)&binary[index];
        index += 2;
        return u;
    }
    
    uint readUInt()
    {
        uint ui = *cast(uint*)&binary[index];
        index += 4;
        return ui;
    }
    
    ubyte[] readUBytes(size_t num)
    {
        if (index + num < binary.length)
        {
            ubyte[] b = binary[index..index+num];
            index += num;
            return b;
        }
        else
        {
            return binary[index..$];
        }
    }
    
    string readString()
    {
        ubyte len = binary[index];
        index++;
        size_t sStart = index;
        size_t sEnd = sStart + len;
        index += len;
        return cast(string)binary[sStart..sEnd];
    }
    
    bool error(T...)(T args)
    {
        writefln(args);
        return false;
    }
    
    void parseExpression()
    {
        // TODO
    }
    
    bool parse()
    {
        string magic = cast(string)binary[0..3];
        writefln("Magic: %s [%(0x%02X, %)]", magic, magic);
        
        if (magic != "LNK")
        {
            return error("Unknown object file format!");
        }
        
        index = 3;
        
        ver = readUByte();
        writefln("Version: 0x%02X", ver);
    
        if (ver != 2)
        {
            return error("Unknown LNK version!");
        }
        
        bool readingStream = true;
        while(readingStream && index < binary.length)
        {
            ubyte opcode = readUByte();
            writefln("Opcode 0x%02X %s", opcode, cast(PsyqOpcode)opcode);
            
            switch(opcode)
            {
                case PsyqOpcode.PROGRAMTYPE:
                    ubyte t = readUByte();
                    writefln("  Program type: 0x%02X", t);
                    if (t != 7 && t != 9)
                        return error("Unknown program type!");
                    if (programType)
                        return error("Program type already declared!");
                    else
                        programType = t;
                    break;
                
                case PsyqOpcode.SECTION:
                    ushort sectionIndex = readUShort();
                    ushort group = readUShort();
                    ubyte alignment = readUByte();
                    string name = readString();
                    writefln("  Section index: %s, group: %s, alignment: %s, name: %s", sectionIndex, group, alignment, name);
                    Section section = new Section(sectionIndex, group, alignment, name);
                    sectionsByName[name] = section;
                    sectionsByIndex[sectionIndex] = section;
                    if ((alignment - 1) & alignment)
                        return error("Section alignment isn't a power of two!");
                    break;
                
                case PsyqOpcode.SWITCH:
                    ushort sectionIndex = readUShort();
                    writefln("  Switch to section %s...", sectionIndex);
                    currentSection = sectionsByIndex[sectionIndex];
                    if (currentSection is null)
                        return error("Unknown section index!");
                    break;
                
                case PsyqOpcode.BYTES:
                    ushort size = readUShort();
                    writefln("  %s byte(s):", size);
                    ubyte[] slice = readUBytes(size);
                    writefln("  [%(0x%02X, %)]", slice);
                    if (currentSection is null)
                        return error("Current section is not set!");
                    currentSection.pointer = cast(uint)currentSection.getFullSize();
                    if (currentSection.zeroes)
                    {
                        ubyte[] zeroes = new ubyte[currentSection.zeroes];
                        currentSection.zeroes = 0;
                        currentSection.data ~= zeroes;
                    }
                    currentSection.data ~= slice;
                    break;
                
                case PsyqOpcode.EXPORTED_SYMBOL:
                    ushort symbolIndex = readUShort();
                    ushort sectionIndex = readUShort();
                    uint offset = readUInt();
                    string name = readString();
                    writefln("  symbolIndex: %s, sectionIndex: %s, offset: 0x%08X, name: %s", symbolIndex, sectionIndex, offset, name);
                    Symbol symbol = new Symbol(symbolIndex, sectionIndex, offset, name);
                    symbolsByName[name] = symbol;
                    break;
                
                case PsyqOpcode.FILENAME:
                    ushort fileIndex = readUShort();
                    string fileName = readString();
                    writefln("  %s \"%s\"", fileIndex, fileName);
                    break;
                
                case PsyqOpcode.RELOCATION:
                    RelocationType relocType = cast(RelocationType)readUByte();
                    writefln("  Type: %s", relocType);
                    ushort offset = readUShort();
                    if (currentSection is null)
                        return error("Current section is not set!");
                    writefln("  Offset 0x%08X+0x%08X", offset, currentSection.pointer);
                    parseExpression();
                    //Expression expression = parseExpression(file, verbose);
                    // Addend will be populated later during expression evaluation
                    currentSection.relocations ~= Relocation(relocType, offset + currentSection.pointer, 0);
                    break;
                
                case PsyqOpcode.END_SLD:
                    ushort zero = readUShort(); // 2 bytes of nothing
                    if (zero != 0)
                        return error("Invalid END_SLD");
                    break;
                
                case PsyqOpcode.END:
                    readingStream = false;
                    break;
                
                default:
                    return error("Unknown opcode!");
            }
        }
        
        return true;
    }
}
