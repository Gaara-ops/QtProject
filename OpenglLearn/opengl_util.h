#ifndef OPENGL_UTIL_H
#define OPENGL_UTIL_H

#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

using namespace std;


#ifdef WIN32
#define SNPRINTF _snprintf_s
#define VSNPRINTF vsnprintf_s
#define RANDOM rand
#define SRANDOM srand((unsigned)time(NULL))
#if (_MSC_VER == 1900)
#elif (_MSC_VER == 1800)
#else
float fmax(float a, float b);
#endif
#else
#define SNPRINTF snprintf
#define VSNPRINTF vsnprintf
#define RANDOM random
#define SRANDOM srandom(getpid())
#endif

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

bool ReadFile(const char* pFileName, string& outFile);

#ifdef WIN32

bool ReadBinaryFile(const char* pFileName, int& size);
#else
char* ReadBinaryFile(const char* pFileName, int& size);
#endif

long long GetCurrentTimeMillis();

#endif // OPENGL_UTIL_H
