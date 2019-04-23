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


bool ReadFile(const char* pFileName, string& outFile)
{
    ifstream f(pFileName);

    bool ret = false;

    if (f.is_open()) {
        string line;
        while (getline(f, line)) {
            outFile.append(line);
            outFile.append("\n");
        }

        f.close();

        ret = true;
    }
    else {
        printf("read file error!");
    }

    return ret;
}


#ifdef WIN32

bool ReadBinaryFile(const char* pFileName, int& size)
{
    HANDLE f = CreateFileA(pFileName, GENERIC_READ, 0, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (f == INVALID_HANDLE_VALUE) {
        printf("read binary file error!");
        return false;
    }

    size = GetFileSize(f, NULL);

    if (size == INVALID_FILE_SIZE) {
        printf("Invalid file size %s\n", pFileName);
        return false;
    }

    // wip for tutorial51
    assert(0);

    return true;
}

#else
char* ReadBinaryFile(const char* pFileName, int& size)
{
    int f = open(pFileName, O_RDONLY);

    if (f == -1) {
        OGLDEV_ERROR("Error opening '%s': %s\n", pFileName, strerror(errno));
        return NULL;
    }

    struct stat stat_buf;
    int error = stat(pFileName, &stat_buf);

    if (error) {
        OGLDEV_ERROR("Error getting file stats: %s\n", strerror(errno));
        return NULL;
    }

    size = stat_buf.st_size;

    char* p = (char*)malloc(size);
    assert(p);

    int read_len = read(f, p, size);

    if (read_len != size) {
        OGLDEV_ERROR("Error reading file: %s\n", strerror(errno));
        return NULL;
    }

    close(f);

    return p;
}
#endif

#endif // OPENGL_UTIL_H
