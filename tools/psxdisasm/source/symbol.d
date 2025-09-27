module symbol;

class Symbol
{
    uint index;
    uint sectionIndex;
    uint offset;
    string name;
    
    this(uint index, uint sectionIndex, uint offset, string name)
    {
        this.index = index;
        this.sectionIndex = sectionIndex;
        this.offset = offset;
        this.name = name;
    }
}
