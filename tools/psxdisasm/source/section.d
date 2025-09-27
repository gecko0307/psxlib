module section;

import disasm;

class Section
{
    uint index;
    uint group;
    uint alignment;
    string name;
    ubyte[] data;
    
    this(uint index, uint group, uint alignment, string name)
    {
        this.index = index;
        this.group = group;
        this.alignment = alignment;
        this.name = name;
    }
    
    void disassemble()
    {
        if (data.length > 0)
            disasmMIPSCode(cast(uint[])data);
    }
}
