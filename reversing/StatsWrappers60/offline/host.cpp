// Anfitrion del arnes offline: un .exe sin CRT enlazado en 0x400000 (la direccion de gta-sa.exe) cuya seccion .gta reserva la imagen del juego (0x401000..0xCB1000).
// Solo carga harness.dll y le pasa la linea de comandos; el DLL copia las secciones del EXE canonico sobre la reserva y termina el proceso con ExitProcess (nunca vuelve aqui,
// porque este codigo y su tabla de importaciones quedan sobrescritos). Se enlaza con /NODEFAULTLIB /ENTRY:Start /BASE:0x400000 /FIXED /DYNAMICBASE:NO.
#include <windows.h>
#pragma section(".gta", read, write, execute)
__declspec(allocate(".gta")) char gtaReserve[0x8B0000];
using RunFn = void(__cdecl*)(const char*);
extern "C" void Start() {
    char path[MAX_PATH];
    const DWORD n = GetModuleFileNameA(nullptr, path, MAX_PATH);
    DWORD k = n;
    while (k && path[k - 1] != '\\') --k;
    const char name[] = "harness.dll";
    for (unsigned i = 0; i < sizeof name; ++i) path[k + i] = name[i];
    const HMODULE dll = LoadLibraryA(path);
    if (!dll) ExitProcess(10);
    const RunFn run = reinterpret_cast<RunFn>(GetProcAddress(dll, "Run"));
    if (!run) ExitProcess(11);
    run(GetCommandLineA());
    ExitProcess(12);
}
