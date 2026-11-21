// Banco FnRpMatFX6: las 6 funciones de QTML (0x8194B0, 0x8194C0, 0x819520, 0x8195C0, 0x819770, 0x81DAE0) del EXE original mapeado en memoria
// contra source/app/platform/win/QTMLRuntime.cpp (el archivo real, incluido tal cual). Sin ejecutar el juego.
// Las 16 funciones de KERNEL32/ADVAPI32/USER32 que usan se sustituyen por maquetas deterministas (la IAT del EXE apunta a ellas; el C++ las llama por
// macros de nombre); cada llamada registra sus argumentos. Se compara, por escenario aleatorio: registro de llamadas, valor devuelto y los 6 globales de QTML.
#include "../FnLIB4A/common.h"
#include <stdarg.h>

static const u32 A_CNT = 0xC9ABD8, A_MOD = 0xC9ABDC, A_HOOK = 0x8E2A08, A_DISP = 0x8E2A0C, A_CC = 0x8E2A10, A_CCS = 0x8E2A14;
static Rng* S;           // aleatoriedad de las maquetas (se reinicia con la misma semilla para cada lado)
static bool g_cpp = false, g_neg = false, g_gpaOK = false, g_hasArg = false; // g_gpaOK: GetProcAddress nunca falla (los puentes saltan a 0x8E2A0C sin comprobarlo)
static std::vector<u32> g_calls;
static void LG(u32 v) { g_calls.push_back(v); }
static u32 hs(const char* s) { return s ? fnv(s, strlen(s)) : 0xDEADBEEF; }

static const char* kPaths[] = { "C:\\Games\\GTA\\gta_sa.exe", "gta_sa.exe", "D:/x/y/gta_sa.exe", "C:\\a\\b\\", "\\z" };
static const char* kRegVals[] = { "C:\\QT\\", "C:\\QT", "/q/", "D:\\Archivos de programa\\QuickTime" };
static int __cdecl m_dispatch(int flags) { LG(0xD15); if (g_hasArg) LG(flags); return (int)(S->below(3) == 0 ? 0 : (S->next() & 0xFFFF) | (S->below(2) ? 0xABCD0000 : 0)); }

static DWORD WINAPI m_GetCurrentProcessId() { LG(1); return 0x2A4; }
static int __cdecl m_wsprintfA(char* b, const char* f, ...) { va_list a; va_start(a, f); int n = wvsprintfA(b, f, a); va_end(a); LG(2); LG(hs(f)); LG(hs(b)); return n; }
static HANDLE WINAPI m_CreateMutexA(LPSECURITY_ATTRIBUTES at, BOOL o, LPCSTR n) { LG(3); LG((u32)at); LG(o); LG(hs(n)); return S->below(4) ? (HANDLE)0xAAA0 : nullptr; }
static DWORD WINAPI m_Wait(HANDLE h, DWORD t) { LG(4); LG((u32)h); LG(t); return 0; }
static BOOL WINAPI m_ReleaseMutex(HANDLE h) { LG(5); LG((u32)h); return 1; }
static BOOL WINAPI m_CloseHandle(HANDLE h) { LG(6); LG((u32)h); return 1; }
static DWORD WINAPI m_GetLastError() { LG(7); return 5; }
static HMODULE WINAPI m_GetModuleHandleA(LPCSTR n) { LG(8); LG(hs(n)); return S->below(3) == 0 ? (HMODULE)0xB000 : nullptr; }
static DWORD WINAPI m_GetModuleFileNameA(HMODULE h, LPSTR b, DWORD n) {
    LG(9); LG((u32)h); LG(n);
    if (S->below(5) == 0) return 0;
    const char* p = kPaths[S->below(5)]; strcpy(b, p); return (DWORD)strlen(p);
}
static HMODULE WINAPI m_LoadLibraryA(LPCSTR p) { LG(10); LG(hs(p)); return S->below(3) == 0 ? nullptr : (HMODULE)(0xC000 + S->below(16) * 0x100); }
static FARPROC WINAPI m_GetProcAddress(HMODULE h, LPCSTR n) {
    LG(11); LG((u32)h); LG(hs(n));
    if (!g_gpaOK && S->below(8) == 0) return nullptr;
    if (n && !strcmp(n, "theQuickTimeDispatcher")) return (FARPROC)m_dispatch;
    return (FARPROC)(0xD000 + (hs(n) & 0xFF0));
}
static BOOL WINAPI m_FreeLibrary(HMODULE h) { LG(12); LG((u32)h); return 1; }
static LSTATUS WINAPI m_RegOpenKeyExA(HKEY r, LPCSTR s, DWORD o, REGSAM m, PHKEY k) {
    LG(13); LG((u32)r ^ ((g_cpp && g_neg) ? 1 : 0)); LG(hs(s)); LG(o); LG(m);
    if (S->below(3) == 0) return 2;
    *k = (HKEY)(0xE000 + S->below(4)); return 0;
}
static LSTATUS WINAPI m_RegQueryValueExA(HKEY k, LPCSTR n, LPDWORD rs, LPDWORD t, LPBYTE d, LPDWORD sz) {
    LG(14); LG((u32)k); LG(hs(n)); LG((u32)rs); LG((u32)t); LG(*sz);
    if (S->below(3) == 0) return 2;
    const char* v = kRegVals[S->below(4)]; strcpy((char*)d, v); *sz = (DWORD)strlen(v) + 1; return 0;
}
static LSTATUS WINAPI m_RegCloseKey(HKEY k) { LG(15); LG((u32)k); return 0; }
static UINT WINAPI m_GetSystemDirectoryA(LPSTR b, UINT n) { LG(16); LG(n); if (S->below(6) == 0) return 0; strcpy(b, "C:\\Windows\\System32"); return 19; }

#define GetCurrentProcessId m_GetCurrentProcessId
#define wsprintfA m_wsprintfA
#define CreateMutexA m_CreateMutexA
#define WaitForSingleObject m_Wait
#define ReleaseMutex m_ReleaseMutex
#define CloseHandle m_CloseHandle
#define GetLastError m_GetLastError
#define GetModuleHandleA m_GetModuleHandleA
#define GetModuleFileNameA m_GetModuleFileNameA
#define LoadLibraryA m_LoadLibraryA
#define GetProcAddress m_GetProcAddress
#define FreeLibrary m_FreeLibrary
#define RegOpenKeyExA m_RegOpenKeyExA
#define RegQueryValueExA m_RegQueryValueExA
#define RegCloseKey m_RegCloseKey
#define GetSystemDirectoryA m_GetSystemDirectoryA
#include "../../source/app/platform/win/QTMLRuntime.cpp"

static void patch_iat() {
    struct E { u32 a; void* f; } t[] = { {0x858000, (void*)m_RegCloseKey}, {0x858008, (void*)m_RegOpenKeyExA}, {0x85800C, (void*)m_RegQueryValueExA}, {0x858068, (void*)m_GetModuleHandleA},
        {0x858070, (void*)m_LoadLibraryA}, {0x85806C, (void*)m_GetProcAddress}, {0x858110, (void*)m_FreeLibrary}, {0x85818C, (void*)m_GetCurrentProcessId}, {0x858258, (void*)m_wsprintfA},
        {0x858188, (void*)m_CreateMutexA}, {0x858088, (void*)m_Wait}, {0x858094, (void*)m_GetLastError}, {0x858184, (void*)m_ReleaseMutex}, {0x858078, (void*)m_CloseHandle},
        {0x858194, (void*)m_GetModuleFileNameA}, {0x858190, (void*)m_GetSystemDirectoryA} };
    for (auto& e : t) *(u32*)e.a = (u32)e.f;
}
#define G(a) (*(u32*)(a))
struct Snap { std::vector<u32> calls; u32 ret, g[6]; bool operator==(const Snap& o) const { return calls == o.calls && ret == o.ret && !memcmp(g, o.g, sizeof g); } };
static const u32 GA[6] = { A_CNT, A_MOD, A_HOOK, A_DISP, A_CC, A_CCS };
static Snap take(u32 ret) { Snap s; s.calls = g_calls; s.ret = ret; for (int i = 0; i < 6; i++) s.g[i] = G(GA[i]); return s; }

typedef u32 (__cdecl *F0)(); typedef u32 (__cdecl *F1)(u32);
int main(int argc, char** argv) {
    const char* exe = argc > 1 ? argv[1] : "D:\\Original\\_crt1a_work\\gta_sa.exe";
    int N = argc > 2 ? atoi(argv[2]) : 20000; g_neg = argc > 3 && !strcmp(argv[3], "neg");
    SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* e) -> LONG { printf("EXCEPCION %08lx en EIP=%08lx acceso=%p tipo %Iu\n", e->ExceptionRecord->ExceptionCode, e->ContextRecord->Eip, (void*)e->ExceptionRecord->ExceptionInformation[1], e->ExceptionRecord->ExceptionInformation[0]); return EXCEPTION_EXECUTE_HANDLER; });
    if (!map_exe(exe)) return 2;
    patch_iat(); setvbuf(stdout, 0, _IONBF, 0); printf("mapeado\n");
    const char* names[6] = { "InitializeThunk 8194B0", "TerminateThunk 8194C0", "ResolveEntryPoints 819520", "Initialize 8195C0", "Terminate 819770", "LoadLibrary 81DAE0" };
    int diffs[6] = {}, runs[6] = {}, shown = 0; long long events = 0;
    const char* libs[3] = { "QuickTime.qts", "otra.dll", nullptr };
    for (int fn = 0; fn < 6; fn++) for (int i = 0; i < N; i++) {
        u64 seed = (u64)i * 7 + fn * 1000003 + 11;
        Rng init(seed ^ 0x55); u32 st[6];
        static const int cnts[] = { -1, 0, 0, 1, 2, 5 };
        st[0] = cnts[init.below(6)];
        st[1] = init.below(2) ? 0 : 0xC1000 + init.below(4) * 0x100;
        st[2] = init.below(3) == 0 ? 0 : (init.below(2) ? 0x819520 : 0x819700);
        st[3] = init.below(2) ? (u32)m_dispatch : 0x8194D0;
        st[4] = init.below(2) ? 0x8194D0 : 0xD0D0;
        st[5] = init.below(2) ? 0x8194D0 : 0xD1D1;
        u32 arg = init.below(2) ? 0 : init.next();
        const char* lib = libs[init.below(3)];
        g_gpaOK = fn == 1 || fn == 4 || fn == 0; g_hasArg = fn == 0 || fn == 3; Snap res[2];
        for (int side = 0; side < 2; side++) {
            g_cpp = side == 1; Rng r(seed); S = &r; g_calls.clear();
            for (int k = 0; k < 6; k++) G(GA[k]) = st[k];
            u32 ret = 0;
            switch (fn) {
            case 0: ret = side ? (u16)QtmlInitializeThunk((int32)arg) : (u16)((F1)0x8194B0)(arg); break;
            case 1: if (side) QtmlTerminateThunk(); else ((F0)0x8194C0)(); break;
            case 2: if (side) QtmlResolveEntryPoints(); else ((F0)0x819520)(); break;
            case 3: ret = side ? (u16)QtmlInitialize((int32)arg) : (u16)((F1)0x8195C0)(arg); break;
            case 4: if (side) QtmlTerminate(); else ((F0)0x819770)(); break;
            case 5: ret = side ? (u32)QtmlLoadLibrary(lib) : ((F1)0x81DAE0)((u32)lib); break;
            }
            res[side] = take(ret);
        }
        runs[fn]++; events += res[0].calls.size();
        if (!(res[0] == res[1])) { diffs[fn]++; if (shown < 6) { size_t k = 0; while (k < res[0].calls.size() && k < res[1].calls.size() && res[0].calls[k] == res[1].calls[k]) k++; printf("  primera diferencia de llamadas en %zu; estado inicial cnt=%d mod=%x hook=%x disp=%x; globales EXE:", k, (int)st[0], st[1], st[2], st[3]); for (int q = 0; q < 6; q++) printf(" %x", res[0].g[q]); printf(" | C++:"); for (int q = 0; q < 6; q++) printf(" %x", res[1].g[q]); printf("\n"); } if (shown++ < 6) printf("DIF %s semilla %d: llamadas %zu/%zu ret %08x/%08x\n", names[fn], i, res[0].calls.size(), res[1].calls.size(), res[0].ret, res[1].ret); }
    }
    for (int fn = 0; fn < 6; fn++) printf("%-28s escenarios %6d diferencias %d\n", names[fn], runs[fn], diffs[fn]);
    printf("eventos de maqueta comparados (lado EXE): %lld\n", events);
    int tot = 0; for (int d : diffs) tot += d; printf("%s: %d diferencias\n", g_neg ? "CONTROL NEGATIVO" : "TOTAL", tot);
    return tot ? 1 : 0;
}
