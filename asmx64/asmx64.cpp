
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    printf("测试输出中文!\n");
}

