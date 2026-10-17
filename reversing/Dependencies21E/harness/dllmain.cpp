#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern "C" int HarnessMain(int argc, char** argv);
extern "C" __declspec(dllexport) void RunFromStub(const char* cmdline) {
    static char buf[2048]; strncpy(buf, cmdline, sizeof buf - 1);
    char* argv[32]; int argc = 0; char* p = buf; bool inq = false;
    while (*p && argc < 31) {
        while (*p == ' ') ++p;
        if (!*p) break;
        if (*p == '"') { inq = true; ++p; }
        argv[argc++] = p;
        while (*p && (inq ? *p != '"' : *p != ' ')) ++p;
        if (*p) *p++ = 0;
        inq = false;
    }
    int rc = HarnessMain(argc, argv);
    fflush(stdout);
    ExitProcess(rc);
}
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
