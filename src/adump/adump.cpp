
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
extern "C" __declspec(dllimport) int dumper_v1(int argc, char** argv);

int wmain(int argc, WCHAR ** argv)
{
	int i, ret;
	char** byteArgv;

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	
	byteArgv = (char**) malloc(sizeof(char*) * argc);

	for (i = 0; i < argc; i++) 
	{
		size_t wlen = wcslen(argv[i]);
		size_t blen = wlen * 4;
		char* buffer = (char*) malloc(blen);
		WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, buffer, (int)blen, NULL, NULL);
		byteArgv[i] = buffer;
	}

	ret = dumper_v1(argc, byteArgv);

	for (i = 0; i < argc; i++)
		free(byteArgv[i]);
	free(byteArgv);
	return ret;
}

#else

extern "C" int dumper_v1(int argc, char** argv);

int main(int argc, char** argv)
{
	return dumper_v1(argc, argv);
}

#endif

