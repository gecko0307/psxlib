import std.stdio;
import std.file;

import psyqobj;
import section;
import disasm;

int main(string[] args)
{
    if (args.length < 2)
        return 1;
    
    string objFilename = args[1];
    
    writefln("File: %s", objFilename);
    
    ubyte[] binary = cast(ubyte[])std.file.read(objFilename);
    
    PsyqObject obj = new PsyqObject(binary);
    
    foreach(name, section; obj.sectionsByName)
    {
        if (section.data.length > 0)
        {
            writefln("Section %s (%s):", name, section.index);
            if (section.name == ".text")
            {
                if (section.data.length > 0)
                {
                    auto disassembler = new MipsDisassembler(cast(uint[])section.data);
                }
            }
            else
            {
                writefln("  \"%s\"", cast(string)section.data);
            }
        }
    }
    
    if (obj.parsed)
        return 0;
    else
        return 1;
}
