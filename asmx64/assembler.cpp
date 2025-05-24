#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"


X64Assembler::X64Assembler()
{
    
}

void X64Assembler::assemble(const std::string & filepath)
{
    SourceLoader loader;
    loader.loadFile(filepath);
    SourceParser parser(&sourceLinesP, &loader);
    parser.parse();
    
    
    assemblePass1();

    do {
        assemblePass2();
        if (!boNeedReScan) {
            break;
        }
        printf("=====哈哈=====\n");
    } while (true);

    //Console.WriteLine("open ok");
    //string s = Encoding.UTF8.GetString(src);
    //Console.WriteLine(s);
}

void X64Assembler::assemblePass1()
{
    
}

void X64Assembler::assemblePass2()
{
    
}



