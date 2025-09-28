module section;

enum RelocationType: ubyte
{
    REL32_BE = 8,
    GPREL16_BE = 12,
    REL32 = 16,
    GPREL16_LE = 30,
    REL26 = 74,
    HI16 = 82,
    LO16 = 84,
    REL26_BE = 92,
    HI16_BE = 96,
    LO16_BE = 98,
    GPREL16 = 100,
};

struct Relocation
{
    RelocationType type;
    uint offset;
    int addend;
};

class Section
{
    uint index;
    uint group;
    uint alignment;
    string name;
    uint pointer;
    uint zeroes;
    uint uninitializedOffset;
    ubyte[] data;
    Relocation[] relocations;
    
    this(uint index, uint group, uint alignment, string name)
    {
        this.index = index;
        this.group = group;
        this.alignment = alignment;
        this.name = name;
    }
    
    size_t getFullSize()
    {
        return data.length + zeroes + uninitializedOffset;
    }
}
