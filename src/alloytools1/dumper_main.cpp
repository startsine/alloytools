#include <stdio.h>

extern "C" 
#ifdef _WIN32
__declspec(dllexport) 
#endif
int dumper_v1(int argc, char** argv)
{
    printf("我测试看呀");
    return 0;
}



