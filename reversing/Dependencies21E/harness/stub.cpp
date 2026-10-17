// Anfitrion sin CRT: su imagen ocupa [0x400000, 0xCC0000) para que harness.dll pueda copiar ahi el codigo del EXE. Nunca vuelve de RunFromStub.
#include <windows.h>
#pragma section(".agta", read, execute)
__declspec(allocate(".agta")) unsigned char g_gta[0x8C0000];
extern "C" void Start() {
    char path[MAX_PATH];
    DWORD n = GetModuleFileNameA(nullptr, path, MAX_PATH);
    while (n && path[n - 1] != 92) --n;
    const char name[] = "harness.dll";
    for (unsigned i = 0; i < sizeof name; ++i) path[n + i] = name[i];
    HMODULE m = LoadLibraryA(path);
    if (!m) ExitProcess(3);
    auto fn = (void (*)(const char*))GetProcAddress(m, "RunFromStub");
    if (!fn) ExitProcess(4);
    fn(GetCommandLineA());
    ExitProcess(5);
}
