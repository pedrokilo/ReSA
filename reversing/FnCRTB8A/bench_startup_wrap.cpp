// CrtStartup.cpp se compila aparte (sus ayudantes anonimos chocan con los de CrtState.cpp) y con las llamadas del sistema interpuestas (bench_apistubs.h).
#include <windows.h>
#include <intrin.h>
#include "bench_apistubs.h"
#define GetSystemTimeAsFileTime StubGetSystemTimeAsFileTime
#define GetCurrentProcessId StubGetCurrentProcessId
#define GetCurrentThreadId StubGetCurrentThreadId
#define GetTickCount StubGetTickCount
#define QueryPerformanceCounter StubQueryPerformanceCounter
#define SetUnhandledExceptionFilter StubSetUnhandledExceptionFilter
#define GetProcAddress StubGetProcAddress
#include "CrtStartup.cpp"
