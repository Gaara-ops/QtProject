#include "opengl_util.h"

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


long long GetCurrentTimeMillis(){
#ifdef WIN32
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif
}
