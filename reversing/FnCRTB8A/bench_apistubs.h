#pragma once
// Interposicion de las llamadas del sistema de CrtStartup.cpp en el banco CRTB8A: las mismas funciones se ponen en la IAT del EXE original (0x858200, 0x85818C, 0x8581E8, 0x858064, 0x8580C8,
// 0x8581D4) y se llaman por nombre desde el C++, asi las dos versiones ven valores identicos y el resultado es determinista.
#include <windows.h>
extern unsigned g_ftLo, g_ftHi, g_pid, g_tid, g_tick, g_qpcLo, g_qpcHi;
extern void* g_prevFilter;   // lo que devuelve SetUnhandledExceptionFilter
extern void* g_setFilterArg; // lo que se le paso
extern int g_gpaMode;      // 0 = GetProcAddress real, 1 = IsProcessorFeaturePresent propia (devuelve g_present), 2 = no la encuentra
extern int g_present;
extern "C++" {
FARPROC WINAPI StubGetProcAddress(HMODULE m, LPCSTR name);
void  WINAPI StubGetSystemTimeAsFileTime(LPFILETIME f);
DWORD WINAPI StubGetCurrentProcessId();
DWORD WINAPI StubGetCurrentThreadId();
DWORD WINAPI StubGetTickCount();
BOOL  WINAPI StubQueryPerformanceCounter(LARGE_INTEGER* c);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI StubSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER f);
}
