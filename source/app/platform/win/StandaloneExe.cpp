// FASE 8, experimento del ejecutable independiente (docs/CodigoVivoExe/fase8-experimento.md).
//
// Este archivo SOLO existe con GTASA_STANDALONE_EXE (objetivo gta_sa_standalone; el ASI no lo compila: queda vacio). Convierte el codigo del proyecto en un
// EXE de base fija 0x400000 que NO ejecuta ninguna instruccion del gta_sa.exe original:
//
//   1. Callback TLS del exe (antes de todo, sin CRT): lee el gta_sa.exe del usuario SOLO COMO FUENTE DE DATOS y copia a sus direcciones fijas las secciones de
//      datos (.rdata, .data, _TEXT_HA, _rwdseg, .rsrc). El codigo original (.text 0x401000-0x857000 y _rwcseg 0x857000-0x858000) NO se copia: la reserva se
//      rellena con INT3 (0xCC). Resuelve la IAT original (el codigo propio lee ranuras de la IAT por direccion).
//   2. Entrada del exe (FaseEntrada): ExeCrt::StandaloneStartup() (montón, hilos, E/S, atexit, constantes x87 en las direcciones del EXE) y despues el
//      arranque del CRT propio (WinMainCRTStartup: inicializadores estaticos propios, que ya pueden usar el operator new del EXE reconstruido).
//   3. WinMain propio: DllMain del proyecto (instala los ganchos: escriben su JMP en la reserva de INT3) y NOTSA_WinMain (el WinMain reconstruido).
//
// Cualquier llamada del codigo propio a una direccion del EXE que no tenga reemplazo cae en un INT3: el manejador vectorizado lo registra (direccion, quien
// llama, registros, pila) y, segun el modo, termina (estricto) o devuelve 0 simulando el retorno (--censo, con la tabla de `ret n` de reversing/FnFase8).
//
// Opciones de linea de comandos propias (se leen sin CRT): --ruta-original <gta_sa.exe> (o la variable GTASA_ORIGINAL_EXE), --directorio-juego <dir>,
// --salida <dir>, --limite <s> (0 = sin limite), --censo, --tabla-ret <retn.bin>, --max-censo <n>.
#ifdef GTASA_STANDALONE_EXE
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cstdarg>
#include <cstdint>
#include <intrin.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved); // dllmain.cpp del proyecto
INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow); // WinMain.cpp
extern "C" { extern uint32_t g_DbgCmdTrace[256][4]; extern uint32_t g_DbgCmdTraceIdx; extern uint32_t g_DbgMainTrace[32][4]; extern uint32_t g_DbgMainTraceIdx; } // anillo de los ultimos comandos de script (Scripts/RunningScript.cpp)
namespace DatosExe { void RunStaticInitializers(); }                    // DatosExe/StaticInit.cpp
namespace ExeCrt { int __cdecl StandaloneStartup(); }                    // Crt/CrtStartup.cpp
extern "C" PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;
extern "C" void WinMainCRTStartup();

namespace {
constexpr uint32_t kBase = 0x400000, kReservaIni = 0x401000, kReservaFin = 0xCB1000;
constexpr uint32_t kCodigoIni = 0x401000, kCodigoFin = 0x858000; // .text + _rwcseg del original: NO se copia
constexpr uint32_t kTamArchivoEsperado = 5189632;
constexpr uint32_t kFrameCounter = 0xB7CB4C; // CTimer::m_FrameCounter (latido)

struct Estado {
    char ruta[MAX_PATH], salida[MAX_PATH], dirJuego[MAX_PATH], tablaRet[MAX_PATH], stubs[MAX_PATH];
    bool censo, cargado, shims, sinInit;
    DWORD limite, maxCenso;
    HANDLE log;
    DWORD t0;
    uint8_t* archivo;
    DWORD tamArchivo;
};
Estado g;

void Escribir(HANDLE h, const char* s, int n) {
    if (h && h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, s, (DWORD)n, &w, nullptr); }
}
void Log(const char* fmt, ...) {
    char buf[1400];
    va_list ap; va_start(ap, fmt);
    int n = wvsprintfA(buf, fmt, ap);
    va_end(ap);
    if (n > 1390) n = 1390;
    buf[n++] = '\r'; buf[n++] = '\n';
    Escribir(g.log, buf, n);
}
void LogT(const char* fmt, ...) {
    char buf[1400];
    int n = wsprintfA(buf, "[%6lu ms] ", GetTickCount() - g.t0);
    va_list ap; va_start(ap, fmt);
    n += wvsprintfA(buf + n, fmt, ap);
    va_end(ap);
    if (n > 1390) n = 1390;
    buf[n++] = '\r'; buf[n++] = '\n';
    Escribir(g.log, buf, n);
}
[[noreturn]] void Salir(UINT codigo) {
    if (g.log) FlushFileBuffers(g.log);
    TerminateProcess(GetCurrentProcess(), codigo);
    for (;;) Sleep(1000);
}
void Ruta(char* dst, const char* dir, const char* nombre) {
    lstrcpyA(dst, dir);
    int l = lstrlenA(dst);
    if (l && dst[l - 1] != '\\') dst[l++] = '\\';
    lstrcpyA(dst + l, nombre);
}
bool Iguales(const char* a, const char* b) { return lstrcmpiA(a, b) == 0; }
bool Legible(uint32_t a, uint32_t n) {
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery((void*)(uintptr_t)a, &mbi, sizeof mbi) || mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    return (uintptr_t)mbi.BaseAddress + mbi.RegionSize >= (uintptr_t)a + n;
}

int TrocearLinea(char* bufer, int cap, char** argv, int maxArgs) {
    const char* p = GetCommandLineA();
    int argc = 0, o = 0;
    while (*p) {
        while (*p == ' ' || *p == '\t') ++p;
        if (!*p || argc >= maxArgs) break;
        argv[argc++] = bufer + o;
        bool q = false;
        while (*p && (q || (*p != ' ' && *p != '\t'))) {
            if (*p == '"') q = !q; else if (o < cap - 1) bufer[o++] = *p;
            ++p;
        }
        bufer[o++] = 0;
    }
    return argc;
}
void LeerArgumentos() {
    static char bufer[4096];
    char* argv[64];
    const int argc = TrocearLinea(bufer, sizeof bufer, argv, 64);
    g.limite = 0;
    g.maxCenso = 100000;
    for (int i = 1; i < argc; ++i) {
        const char* a = argv[i];
        if (Iguales(a, "--ruta-original") && i + 1 < argc) lstrcpynA(g.ruta, argv[++i], MAX_PATH);
        else if (Iguales(a, "--directorio-juego") && i + 1 < argc) lstrcpynA(g.dirJuego, argv[++i], MAX_PATH);
        else if (Iguales(a, "--salida") && i + 1 < argc) lstrcpynA(g.salida, argv[++i], MAX_PATH);
        else if (Iguales(a, "--stubs") && i + 1 < argc) lstrcpynA(g.stubs, argv[++i], MAX_PATH);
        else if (Iguales(a, "--tabla-ret") && i + 1 < argc) lstrcpynA(g.tablaRet, argv[++i], MAX_PATH);
        else if (Iguales(a, "--limite") && i + 1 < argc) { g.limite = 0; for (const char* d = argv[++i]; *d >= '0' && *d <= '9'; ++d) g.limite = g.limite * 10 + (*d - '0'); }
        else if (Iguales(a, "--max-censo") && i + 1 < argc) { g.maxCenso = 0; for (const char* d = argv[++i]; *d >= '0' && *d <= '9'; ++d) g.maxCenso = g.maxCenso * 10 + (*d - '0'); }
        else if (Iguales(a, "--censo")) g.censo = true;
        else if (Iguales(a, "--shims")) g.shims = true;
        else if (Iguales(a, "--sin-init")) g.sinInit = true;
    }
    if (!g.ruta[0]) GetEnvironmentVariableA("GTASA_ORIGINAL_EXE", g.ruta, MAX_PATH);
}

bool LeerArchivo() {
    HANDLE h = CreateFileA(g.ruta, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) { Log("ERROR: no se pudo abrir %s (error %lu)", g.ruta, GetLastError()); return false; }
    LARGE_INTEGER sz;
    GetFileSizeEx(h, &sz);
    g.tamArchivo = (DWORD)sz.QuadPart;
    g.archivo = (uint8_t*)VirtualAlloc(nullptr, g.tamArchivo + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD n = 0;
    const bool ok = g.archivo && ReadFile(h, g.archivo, g.tamArchivo, &n, nullptr) && n == g.tamArchivo;
    CloseHandle(h);
    if (!ok) Log("ERROR: lectura incompleta de %s", g.ruta);
    return ok;
}
DWORD ProteccionDe(DWORD car) {
    const bool x = (car & IMAGE_SCN_MEM_EXECUTE) != 0, w = (car & IMAGE_SCN_MEM_WRITE) != 0;
    if (x) return w ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    return w ? PAGE_READWRITE : PAGE_READONLY;
}

// Copia solo las secciones de DATOS; el rango de codigo original se rellena con INT3.
bool MapearDatos() {
    const uint8_t* f = g.archivo;
    if (g.tamArchivo != kTamArchivoEsperado) { Log("ERROR: tamano %lu distinto de %lu (no es el gta_sa.exe 1.0 US)", g.tamArchivo, kTamArchivoEsperado); return false; }
    auto* dos = (const IMAGE_DOS_HEADER*)f;
    auto* nt = (const IMAGE_NT_HEADERS32*)(f + dos->e_lfanew);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.ImageBase != kBase) { Log("ERROR: cabecera PE inesperada"); return false; }
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery((void*)kReservaIni, &mbi, sizeof mbi);
    if (mbi.State != MEM_COMMIT || mbi.Type != MEM_IMAGE) { Log("ERROR: 0x401000 no esta mapeado como imagen del exe propio (falta la reserva)"); return false; }
    DWORD viejo = 0;
    if (!VirtualProtect((void*)kReservaIni, kReservaFin - kReservaIni, PAGE_EXECUTE_READWRITE, &viejo)) { Log("ERROR: VirtualProtect de la reserva (%lu)", GetLastError()); return false; }
    for (uint32_t a = kCodigoIni; a < kCodigoFin; ++a) *(uint8_t*)a = 0xCC;
    Log("codigo original 0x%08X-0x%08X: NO copiado, relleno con INT3 (%lu bytes)", kCodigoIni, kCodigoFin, kCodigoFin - kCodigoIni);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    uint32_t copiados = 0;
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uint32_t va = kBase + sec[i].VirtualAddress, vs = sec[i].Misc.VirtualSize, rs = sec[i].SizeOfRawData;
        if (va >= kCodigoIni && va < kCodigoFin) { Log("seccion %-8.8s VA 0x%08X virtual %8lu: codigo, omitida", (const char*)sec[i].Name, va, vs); continue; }
        if (va < kReservaIni || va + vs > kReservaFin) { Log("ERROR: seccion %.8s fuera de la reserva", (const char*)sec[i].Name); return false; }
        const uint32_t n = rs < vs ? rs : vs;
        for (uint32_t k = 0; k < n; ++k) ((uint8_t*)va)[k] = f[sec[i].PointerToRawData + k];
        copiados += n;
        Log("seccion %-8.8s VA 0x%08X virtual %8lu archivo %8lu car 0x%08lX copiada", (const char*)sec[i].Name, va, vs, rs, sec[i].Characteristics);
    }
    Log("datos copiados desde el original: %lu bytes", copiados);
    return true;
}

int g_importsTotal, g_importsFaltan;
int ResolverImports() {
    SetDllDirectoryA(g.dirJuego);
    auto* nt = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
    const auto& dd = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    for (auto* d = (const IMAGE_IMPORT_DESCRIPTOR*)(kBase + dd.VirtualAddress); d->Name; ++d) {
        const char* dll = (const char*)(kBase + d->Name);
        HMODULE m = LoadLibraryA(dll);
        auto* oft = (const uint32_t*)(kBase + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* iat = (uint32_t*)(kBase + d->FirstThunk);
        for (; *oft; ++oft, ++iat, ++g_importsTotal) {
            const bool ord = (*oft & 0x80000000) != 0;
            const char* nombre = ord ? (const char*)(uintptr_t)(*oft & 0xFFFF) : (const char*)(kBase + *oft + 2);
            FARPROC p = m ? GetProcAddress(m, nombre) : nullptr;
            *iat = (uint32_t)(uintptr_t)p;
            if (!p) { ++g_importsFaltan; if (ord) Log("import sin resolver: %s!#%u", dll, (unsigned)(*oft & 0xFFFF)); else Log("import sin resolver: %s!%s", dll, nombre); }
        }
    }
    Log("imports de la IAT original: %d, sin resolver: %d", g_importsTotal, g_importsFaltan);
    return g_importsFaltan;
}

// ---------------- fallos y censo ----------------
volatile LONG g_terminando, g_enVeh, g_nHits, g_nDistintas, g_nExc;
uint16_t* g_tablaRet; // por direccion (uint16 por byte de .text): 0xFFFF = desconocido; otro = bytes que saca `ret n`. 0 si no hay tabla
uint8_t*  g_visto;    // bitmap sobre el rango de codigo
uint16_t* g_cuenta;   // aciertos por direccion de entrada (guardia de bucle del censo)
constexpr DWORD kMaxAciertos = 400;
struct Stub { uint32_t dir, eax, pop; bool pop_dado; };
Stub g_stubs[256];
int g_nStubs;
HANDLE    g_csv;

const char* Clase(uint32_t a) {
    if (a >= kCodigoIni && a < kCodigoFin) return "original(codigo)";
    if (a >= kCodigoFin && a < kReservaFin) return "original(datos)";
    if (a >= kReservaFin && a < 0x4000000) return "propio";
    return "otro";
}
BOOL CALLBACK EnumVentanas(HWND h, LPARAM);
void InformeFallo(EXCEPTION_POINTERS* ep, const char* origen, bool terminal) {
    if (terminal && InterlockedExchange(&g_terminando, 1)) return;
    char ruta[MAX_PATH], buf[400];
    Ruta(ruta, g.salida, "fase8_fallo.txt");
    HANDLE h = terminal ? CreateFileA(ruta, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr) : nullptr;
    auto* r = ep->ExceptionRecord;
    auto* c = ep->ContextRecord;
    int n;
#define EMIT(...) do { n = wsprintfA(buf, __VA_ARGS__); Escribir(h, buf, n); Escribir(g.log, buf, n); } while (0)
    EMIT("FALLO (%s) codigo 0x%08lX en EIP=0x%08lX [%s] t=%lu ms\r\n", origen, r->ExceptionCode, (DWORD)c->Eip, Clase(c->Eip), GetTickCount() - g.t0);
    if (r->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && r->NumberParameters >= 2)
        EMIT("  acceso %s a 0x%08lX\r\n", r->ExceptionInformation[0] == 0 ? "lectura" : r->ExceptionInformation[0] == 1 ? "escritura" : "ejecucion", (DWORD)r->ExceptionInformation[1]);
    EMIT("  EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX ESI=%08lX EDI=%08lX EBP=%08lX ESP=%08lX\r\n", c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
    EMIT("  pila (palabras que apuntan a codigo propio u original):\r\n");
    int vistas = 0;
    for (uint32_t a = c->Esp; a < c->Esp + 8192 && vistas < 30; a += 4) {
        if (!Legible(a, 4)) break;
        const uint32_t v = *(uint32_t*)a;
        if ((v >= kCodigoIni && v < kCodigoFin) || (v >= kReservaFin && v < kReservaFin + 0x2000000)) { EMIT("    [esp+0x%lX] 0x%08lX %s\r\n", a - c->Esp, v, Clase(v)); ++vistas; }
    }
#undef EMIT
    if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
    if (terminal) { // estado del juego en el momento de la ruptura
        LogT("estado: dispositivo D3D9 [0xC97C28]=0x%08lX, RwCamera [0xC1703C]=0x%08lX, contador de fotogramas=%lu, ventanas del proceso:", *(volatile DWORD*)0xC97C28, *(volatile DWORD*)0xC1703C, *(volatile DWORD*)kFrameCounter);
        EnumWindows(EnumVentanas, 0);
    }
}
LONG CALLBACK Filtro(EXCEPTION_POINTERS* ep) {
    InformeFallo(ep, "no manejada", true);
    TerminateProcess(GetCurrentProcess(), 0xDEAD0001);
    return EXCEPTION_EXECUTE_HANDLER;
}

// Un INT3 del rango de codigo original: el codigo propio llamo (o salto) a una direccion del EXE sin reemplazo.
LONG AtenderCodigoOriginal(EXCEPTION_POINTERS* ep, uint32_t a) {
    auto* c = ep->ContextRecord;
    const LONG hits = InterlockedIncrement(&g_nHits);
    const uint32_t idx = a - kCodigoIni;
    const bool nuevo = !(g_visto[idx >> 3] & (1u << (idx & 7)));
    const uint32_t ret = Legible(c->Esp, 4) ? *(uint32_t*)c->Esp : 0;
    if (nuevo) {
        g_visto[idx >> 3] |= (uint8_t)(1u << (idx & 7));
        const LONG nd = InterlockedIncrement(&g_nDistintas);
        char b[300];
        const int n = wsprintfA(b, "%ld,0x%08lX,0x%08lX,%s,0x%08lX,0x%08lX,0x%08lX,0x%08lX,%lu\r\n", nd, a, ret, Clase(ret), c->Ecx,
                                Legible(c->Esp + 4, 4) ? *(uint32_t*)(c->Esp + 4) : 0, Legible(c->Esp + 8, 4) ? *(uint32_t*)(c->Esp + 8) : 0, c->Eax, GetTickCount() - g.t0);
        Escribir(g_csv, b, n);
        LogT("CODIGO ORIGINAL #%ld: 0x%08lX llamado desde 0x%08lX [%s] ecx=%08lX args=%08lX %08lX", nd, a, ret, Clase(ret), c->Ecx,
             Legible(c->Esp + 4, 4) ? *(uint32_t*)(c->Esp + 4) : 0, Legible(c->Esp + 8, 4) ? *(uint32_t*)(c->Esp + 8) : 0);
    }
    if (g_cuenta && g_cuenta[idx] < 0xFFFF) ++g_cuenta[idx];
    if (g_cuenta && g_cuenta[idx] > kMaxAciertos) {
        LogT("BUCLE: 0x%08lX llamado mas de %lu veces (el retorno simulado no basta)", a, kMaxAciertos);
        InformeFallo(ep, "bucle en el censo", true);
        Salir(0xDEAD0004);
    }
    if (!g.censo || (DWORD)g_nDistintas > g.maxCenso || !Legible(c->Esp, 4)) {
        InformeFallo(ep, "llamada a codigo original sin reemplazo", true);
        Salir(0xDEAD0003);
    }
    // Censo: se simula el retorno (eax = 0, edx = 0) sacando los argumentos de `ret n` si la tabla lo conoce (si no, 0 bytes: cdecl).
    uint32_t pop = 0, eax = 0;
    if (g_tablaRet && g_tablaRet[idx] != 0xFFFF) pop = g_tablaRet[idx];
    for (int i = 0; i < g_nStubs; ++i) if (g_stubs[i].dir == a) { eax = g_stubs[i].eax; if (g_stubs[i].pop_dado) pop = g_stubs[i].pop; }
    c->Eax = eax; c->Edx = 0;
    c->Eip = ret;
    c->Esp += 4 + pop;
    return EXCEPTION_CONTINUE_EXECUTION;
}

LONG CALLBACK Vectorizado(EXCEPTION_POINTERS* ep) {
    if (InterlockedIncrement(&g_enVeh) > 1) { InterlockedDecrement(&g_enVeh); return EXCEPTION_CONTINUE_SEARCH; }
    LONG res = EXCEPTION_CONTINUE_SEARCH;
    auto* r = ep->ExceptionRecord;
    const uint32_t a = (uint32_t)(uintptr_t)r->ExceptionAddress;
    if (r->ExceptionCode == EXCEPTION_BREAKPOINT && a >= kCodigoIni && a < kCodigoFin) {
        res = AtenderCodigoOriginal(ep, a);
    } else if (r->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && r->NumberParameters >= 2) {
        const LONG nexc = InterlockedIncrement(&g_nExc);
        if (nexc <= 30) {
            InformeFallo(ep, "primera oportunidad", false);
        }
    }
    InterlockedDecrement(&g_enVeh);
    return res;
}

BOOL CALLBACK EnumVentanas(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    char t[96] = {}, c[64] = {};
    GetWindowTextA(h, t, sizeof t);
    GetClassNameA(h, c, sizeof c);
    RECT r = {};
    GetWindowRect(h, &r);
    LogT("  ventana 0x%08lX clase=%s titulo=\"%s\" visible=%d %ldx%ld", (DWORD)(uintptr_t)h, c, t, (int)IsWindowVisible(h), r.right - r.left, r.bottom - r.top);
    return TRUE;
}
// Contadores de la API de D3D9 (segunda ronda): se parchean las ranuras del vtable del dispositivo (Present 17, EndScene 42, Clear 43, DrawPrimitive 81,
// DrawIndexedPrimitive 82, DrawPrimitiveUP 83, DrawIndexedPrimitiveUP 84) con envolturas que cuentan y reenvian; prueba por memoria de que el juego dibuja.
volatile LONG g_cPresent, g_cEndScene, g_cClear, g_cDraw;
typedef long(__stdcall* FnPresent)(void*, const void*, const void*, void*, const void*);
typedef long(__stdcall* FnVoid)(void*);
typedef long(__stdcall* FnClear)(void*, unsigned long, const void*, unsigned long, unsigned long, float, unsigned long);
typedef long(__stdcall* FnDraw3)(void*, unsigned long, unsigned long, unsigned long);
typedef long(__stdcall* FnDraw6)(void*, unsigned long, long, unsigned long, unsigned long, unsigned long, unsigned long);
typedef long(__stdcall* FnDrawUP)(void*, unsigned long, unsigned long, const void*, unsigned long);
typedef long(__stdcall* FnDrawIUP)(void*, unsigned long, unsigned long, unsigned long, unsigned long, const void*, unsigned long, const void*, unsigned long);
void *o17, *o42, *o43, *o81, *o82, *o83, *o84;
long __stdcall W17(void* d, const void* a, const void* b, void* c, const void* e) { InterlockedIncrement(&g_cPresent); return ((FnPresent)o17)(d, a, b, c, e); }
long __stdcall W42(void* d) { InterlockedIncrement(&g_cEndScene); return ((FnVoid)o42)(d); }
long __stdcall W43(void* d, unsigned long a, const void* b, unsigned long c, unsigned long e, float f, unsigned long g2) { InterlockedIncrement(&g_cClear); return ((FnClear)o43)(d, a, b, c, e, f, g2); }
long __stdcall W81(void* d, unsigned long a, unsigned long b, unsigned long c) { InterlockedIncrement(&g_cDraw); return ((FnDraw3)o81)(d, a, b, c); }
long __stdcall W82(void* d, unsigned long a, long b, unsigned long c, unsigned long e, unsigned long f, unsigned long g2) { InterlockedIncrement(&g_cDraw); return ((FnDraw6)o82)(d, a, b, c, e, f, g2); }
long __stdcall W83(void* d, unsigned long a, unsigned long b, const void* c, unsigned long e) { InterlockedIncrement(&g_cDraw); return ((FnDrawUP)o83)(d, a, b, c, e); }
long __stdcall W84(void* d, unsigned long a, unsigned long b, unsigned long c, unsigned long e, const void* f, unsigned long g2, const void* h, unsigned long i) { InterlockedIncrement(&g_cDraw); return ((FnDrawIUP)o84)(d, a, b, c, e, f, g2, h, i); }
bool g_contadoresD3D;
void InstalarContadoresD3D() {
    if (g_contadoresD3D || !Legible(0xC97C28, 4)) return;
    void* const dev = *(void* volatile*)0xC97C28;
    if (!dev || !Legible((uint32_t)dev, 4)) return;
    void** vt = *(void***)dev;
    struct { int slot; void** orig; void* nuevo; } t[] = {{17, &o17, (void*)W17}, {42, &o42, (void*)W42}, {43, &o43, (void*)W43}, {81, &o81, (void*)W81}, {82, &o82, (void*)W82}, {83, &o83, (void*)W83}, {84, &o84, (void*)W84}};
    DWORD viejo = 0;
    if (!VirtualProtect(vt, 100 * sizeof(void*), PAGE_READWRITE, &viejo)) return;
    for (auto& e : t) { *e.orig = vt[e.slot]; vt[e.slot] = e.nuevo; }
    VirtualProtect(vt, 100 * sizeof(void*), viejo, &viejo);
    g_contadoresD3D = true;
    LogT("contadores de D3D9 instalados en el vtable del dispositivo 0x%08lX", (DWORD)(uintptr_t)dev);
}
HANDLE g_hiloPrincipal; // duplicado del hilo que ejecuta NOTSA_WinMain (para volcar su pila cuando la guardia lo mata)
// Tercera ronda: vuelca los scripts activos (CTheScripts::pActiveScripts 0xA8B42C): nombre, base e IP (relativa a ScriptSpace 0xA49960) y los bytes alrededor de la IP,
// para los fallos del motor de scripts ("Unknown param type"), que son de datos o de lectura de argumentos.
void VolcarScripts() {
    char buf[1200];
    int n = wsprintfA(buf, "  ScriptSpace[0..16]:");
    if (Legible(0xA49960, 16)) for (int i = 0; i < 16; ++i) n += wsprintfA(buf + n, " %02X", *(uint8_t*)(0xA49960 + i));
    n += wsprintfA(buf + n, "  MainScriptSize=%lu LargestMissionScriptSize=%lu\r\n", Legible(0xA444C4, 4) ? *(uint32_t*)0xA444C4 : 0, Legible(0xA444C0, 4) ? *(uint32_t*)0xA444C0 : 0);
    Escribir(g.log, buf, n);
    {
        const uint32_t idx = g_DbgCmdTraceIdx;
        n = wsprintfA(buf, "  ultimos comandos (de %lu): IP-ScriptSpace:comando(hex)\r\n   ", idx);
        Escribir(g.log, buf, n);
        for (uint32_t i = idx > 256 ? idx - 256 : 0; i < idx; ++i) {
            const uint32_t* e = g_DbgCmdTrace[i & 255];
            if (idx - i > 40 && e[2] != 0x6E69616D) continue; // los 40 ultimos y, de los anteriores, solo los del script main
            n = wsprintfA(buf, " %lX:%04lX", e[0] - 0xA49960, e[1] & 0x7FFF);
            Escribir(g.log, buf, n);
        }
        Escribir(g.log, "\r\n", 2);
        n = wsprintfA(buf, "  ultimos 32 del script main (IP:comando:n):");
        for (uint32_t i = g_DbgMainTraceIdx > 32 ? g_DbgMainTraceIdx - 32 : 0; i < g_DbgMainTraceIdx; ++i) {
            const uint32_t* e = g_DbgMainTrace[i & 31];
            n += wsprintfA(buf + n, " %lX:%04lX:%lu", e[0] - 0xA49960, e[1] & 0x7FFF, e[3]);
        }
        n += wsprintfA(buf + n, "\r\n");
        Escribir(g.log, buf, n);
    }
    uint32_t sc = Legible(0xA8B42C, 4) ? *(uint32_t*)0xA8B42C : 0;
    for (int k = 0; sc && k < 16 && Legible(sc, 0x28); ++k) {
        const uint32_t base = *(uint32_t*)(sc + 0x10), ip = *(uint32_t*)(sc + 0x14);
        char nombre[9] = {};
        for (int i = 0; i < 8; ++i) nombre[i] = *(char*)(sc + 8 + i) >= 32 ? *(char*)(sc + 8 + i) : '.';
        n = wsprintfA(buf, "  script %s @0x%08lX base=0x%08lX ip=0x%08lX (offset %ld) bytes[-8..+24]:", nombre, sc, base, ip, (long)(ip - 0xA49960));
        if (Legible(ip - 8, 32)) for (int i = -8; i < 24; ++i) n += wsprintfA(buf + n, " %02X", *(uint8_t*)(ip + i));
        n += wsprintfA(buf + n, "\r\n");
        Escribir(g.log, buf, n);
        sc = *(uint32_t*)sc;
    }
}
// Segunda ronda: antes de matar el proceso por falta de latido o por tiempo, se suspende el hilo principal y se vuelca donde esta (EIP, registros,
// cadena de EBP y palabras de la pila que apuntan a codigo propio u original), para saber en que funcion esta colgado.
void VolcarHiloPrincipal(const char* motivo) {
    if (!g_hiloPrincipal || SuspendThread(g_hiloPrincipal) == (DWORD)-1) return;
    CONTEXT c = {};
    c.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
    if (GetThreadContext(g_hiloPrincipal, &c)) {
        EXCEPTION_RECORD er = {};
        er.ExceptionCode = 0xDEAD0002;
        EXCEPTION_POINTERS ep = {&er, &c};
        char motivoBuf[96];
        wsprintfA(motivoBuf, "hilo principal en la guardia: %s", motivo);
        InformeFallo(&ep, motivoBuf, false);
        char buf[200];
        int n = wsprintfA(buf, "  cadena EBP:");
        uint32_t ebp = c.Ebp;
        for (int i = 0; i < 24 && Legible(ebp, 8); ++i) {
            n += wsprintfA(buf + n, " %08lX", *(uint32_t*)(ebp + 4));
            const uint32_t sig = *(uint32_t*)ebp;
            if (sig <= ebp) break;
            ebp = sig;
        }
        n += wsprintfA(buf + n, "\r\n");
        Escribir(g.log, buf, n);
        VolcarScripts();
    }
    ResumeThread(g_hiloPrincipal);
}
DWORD WINAPI Guardia(LPVOID) {
    uint32_t ultimoFrame = 0;
    DWORD ultimoCambio = GetTickCount(), ultimoLog = 0;
    bool hubo = false;
    int nLat = 0;
    for (;;) {
        Sleep(250);
        const DWORD ahora = GetTickCount(), t = ahora - g.t0;
        const uint32_t frame = Legible(kFrameCounter, 4) ? *(volatile uint32_t*)kFrameCounter : 0;
        if (frame != ultimoFrame) { ultimoFrame = frame; ultimoCambio = ahora; hubo = true; }
        if (ahora - ultimoLog >= 2000) {
            ultimoLog = ahora;
            InstalarContadoresD3D();
            LogT("latido: frame=%lu estado-juego=%ld Present=%ld EndScene=%ld Clear=%ld Draw*=%ld codigo-original-distinto=%ld hits=%ld excepciones=%ld", frame, Legible(0xC8D4C0, 4) ? *(volatile LONG*)0xC8D4C0 : -1L, g_cPresent, g_cEndScene, g_cClear, g_cDraw, g_nDistintas, g_nHits, g_nExc);
            if (++nLat % 5 == 1) EnumWindows(EnumVentanas, 0);
        }
        const bool sinLatido = g.limite && hubo && ahora - ultimoCambio > 20000;
        if ((g.limite && t > g.limite * 1000) || sinLatido) {
            if (sinLatido) LogT("GUARDIA: sin latido durante 20 s; se mata el proceso"); else LogT("GUARDIA: limite de tiempo (%lu s); se mata el proceso", g.limite);
            VolcarHiloPrincipal(sinLatido ? "sin latido" : "limite de tiempo");
            if (g.log) FlushFileBuffers(g.log);
            TerminateProcess(GetCurrentProcess(), 0xDEAD0002);
        }
    }
}

PVOID g_vehHandle;

// Callback TLS dinamico del CRT propio: una DLL cargada por el cargador que cree un hilo lo ejecutaria con el CRT propio sin iniciar.
uint32_t* g_ranuraDyn;
uint32_t g_dynGuardado;
void SuspenderDynTls() {
    auto* dos = (const IMAGE_DOS_HEADER*)kBase;
    auto* nt = (const IMAGE_NT_HEADERS32*)(kBase + dos->e_lfanew);
    auto* tls = (const IMAGE_TLS_DIRECTORY32*)(kBase + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
    auto* cb = (uint32_t*)tls->AddressOfCallBacks;
    const uint32_t dyn = (uint32_t)(uintptr_t)__dyn_tls_init_callback;
    for (int i = 0; cb[i] && i < 16; ++i) if (cb[i] == dyn) g_ranuraDyn = &cb[i];
    if (!g_ranuraDyn) { Log("AVISO: no se encontro la ranura de __dyn_tls_init"); return; }
    DWORD v;
    VirtualProtect(g_ranuraDyn, 4, PAGE_READWRITE, &v);
    g_dynGuardado = *g_ranuraDyn;
    *g_ranuraDyn = 0;
}
void RestaurarDynTls() {
    if (!g_ranuraDyn) return;
    DWORD v;
    *g_ranuraDyn = g_dynGuardado;
    VirtualProtect(g_ranuraDyn, 4, PAGE_READONLY, &v);
}

// Archivo de stubs explicitos del censo: lineas `0xDIR EAX [POP]` (hex); comentarios con #. Son DATOS DE PRUEBA, no reconstrucciones.
uint32_t Hex(const char*& p) {
    uint32_t v = 0;
    while (*p == ' ' || *p == '	') ++p;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;
    for (;; ++p) {
        const char c = *p;
        if (c >= '0' && c <= '9') v = v * 16 + (c - '0'); else if (c >= 'a' && c <= 'f') v = v * 16 + (c - 'a' + 10); else if (c >= 'A' && c <= 'F') v = v * 16 + (c - 'A' + 10); else break;
    }
    return v;
}
void CargarStubs() {
    if (!g.stubs[0]) return;
    HANDLE h = CreateFileA(g.stubs, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) { Log("AVISO: sin archivo de stubs (%s)", g.stubs); return; }
    static char b[16384];
    DWORD n = 0;
    ReadFile(h, b, sizeof b - 1, &n, nullptr);
    CloseHandle(h);
    b[n] = 0;
    for (const char* p = b; *p;) {
        while (*p == ' ' || *p == '	') ++p;
        if (*p == '0' && g_nStubs < 256) {
            Stub& st = g_stubs[g_nStubs];
            st.dir = Hex(p); st.eax = Hex(p);
            while (*p == ' ') ++p;
            st.pop_dado = (*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f');
            st.pop = st.pop_dado ? Hex(p) : 0;
            ++g_nStubs;
        }
        while (*p && *p != 10) ++p;
        if (*p) ++p;
    }
    Log("stubs explicitos cargados: %d", g_nStubs);
}
void CargarTablaRet() {
    if (!g.tablaRet[0]) return;
    HANDLE h = CreateFileA(g.tablaRet, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) { Log("AVISO: sin tabla de ret n (%s)", g.tablaRet); return; }
    const DWORD total = (kCodigoFin - kCodigoIni) * 2;
    g_tablaRet = (uint16_t*)VirtualAlloc(nullptr, total, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD n = 0;
    ReadFile(h, g_tablaRet, total, &n, nullptr);
    CloseHandle(h);
    Log("tabla de ret n cargada: %lu bytes", n);
}

// ---------------- Inicializadores estaticos pendientes de DatosExe (segunda ronda) ----------------
// El EXE original construye sus objetos globales (vtables, valores iniciales) con la tabla _initterm; el EXE propio no ejecuta esa tabla, asi que
// objetos como AudioEngine.m_FrontendAEAudioEntity quedan con la vtable a cero (primer fallo real tras el menu). Mientras DatosExe no los define en C++,
// este puente ejecuta, con el C++ ya enganchado, lo que haria cada rutina (StandaloneInit.inc, generado de fase8-inicializadores.csv; sin atexit).
enum TipoInit : uint32_t { kCtor0, kCtor1, kCtor4, kVec, kFill16, kCopy32, kCopy16, kIndex24, kFdivr16, kWrite8, kWrite16, kWrite32 };
struct FilaInit { uint32_t va, tipo, a, b, c, d, e, f; };
const FilaInit kInit[] = {
#include "StandaloneInit.inc"
};
void EjecutarInicializadoresEstaticos() {
    using Ctor0 = void(__thiscall*)(void*);
    using Ctor1 = void(__thiscall*)(void*, uint32_t);
    using Ctor4 = void(__thiscall*)(void*, uint32_t, uint32_t, uint32_t, uint32_t);
    int n = 0;
    for (const FilaInit& f : kInit) {
        switch (f.tipo) {
        case kCtor0: ((Ctor0)f.b)((void*)f.a); break;
        case kCtor1: ((Ctor1)f.b)((void*)f.a, f.c); break;
        case kCtor4: ((Ctor4)f.b)((void*)f.a, f.c, f.d, f.e, f.f); break;
        case kVec:   for (uint32_t i = 0; i < f.c; ++i) ((Ctor0)f.d)((void*)(f.a + i * f.b)); break;
        case kFill16: for (uint32_t i = 0; i < f.b; ++i) *(volatile uint16_t*)(f.a + 4 * i) = (uint16_t)f.c; break;
        case kCopy32: *(volatile uint32_t*)f.a = *(volatile uint32_t*)f.b; break;
        case kCopy16: *(volatile uint16_t*)f.a = *(volatile uint16_t*)f.b; break;
        case kIndex24: *(volatile uint32_t*)f.a = f.c + 24u * *(volatile uint8_t*)f.b; break;
        case kWrite8:  *(volatile uint8_t*)f.a = (uint8_t)f.b; break;
        case kWrite16: *(volatile uint16_t*)f.a = (uint16_t)f.b; break;
        case kWrite32: *(volatile uint32_t*)f.a = f.b; break;
        case kFdivr16: *(volatile float*)f.a = *(volatile float*)f.c / (float)(int)*(volatile int16_t*)f.b; break;
        }
        ++n;
    }
    LogT("inicializadores estaticos pendientes ejecutados: %d filas (StandaloneInit.inc)", n);
}

// ---------------- Reconstrucciones minimas del propio experimento ("shims") ----------------
// Retirados (codigo vivo CAM): 0x750430, 0x750460 y 0x750480 (envolturas del plugin 0x509 de RwCamera), 0x750AC0, 0x7F98D0, 0x7F99B0 y 0x7FFF00 ya son funciones C++ con gancho
// (rpworld.cpp y rwcore.cpp); instalar aqui un salto propio sobre esas direcciones pisaria el gancho real. `--shims` queda aceptado y no hace nada.
void InstalarShims() {}

void DirectorioDeLaRuta() {
    if (g.dirJuego[0]) return;
    lstrcpyA(g.dirJuego, g.ruta);
    int l = lstrlenA(g.dirJuego);
    while (l > 0 && g.dirJuego[l - 1] != '\\' && g.dirJuego[l - 1] != '/') --l;
    if (l > 0) --l;
    g.dirJuego[l] = 0;
}
} // namespace

static void FaseCargar() {
    g.t0 = GetTickCount();
    LeerArgumentos();
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    if (!g.salida[0]) lstrcpyA(g.salida, cwd);
    if (!g.ruta[0]) return;
    char rutaLog[MAX_PATH];
    Ruta(rutaLog, g.salida, "fase8_arranque.log");
    g.log = CreateFileA(rutaLog, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
    LogT("FaseCargar (callback TLS, antes del CRT propio): modo %s", g.censo ? "censo" : "estricto");
    DirectorioDeLaRuta();
    char rutaCsv[MAX_PATH];
    Ruta(rutaCsv, g.salida, "fase8_codigo_original.csv");
    g_csv = CreateFileA(rutaCsv, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
    Escribir(g_csv, "n,funcion,llamada_desde,quien_llama,ecx,arg1,arg2,eax,ms\r\n", 53);
    g_visto = (uint8_t*)VirtualAlloc(nullptr, (kCodigoFin - kCodigoIni) / 8 + 16, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    SetUnhandledExceptionFilter(Filtro);
    g_vehHandle = AddVectoredExceptionHandler(1, Vectorizado);
    SuspenderDynTls();
    if (!LeerArchivo()) Salir(3);
    if (!MapearDatos()) Salir(4);
    ResolverImports();
    CargarTablaRet();
    CargarStubs();
    g_cuenta = (uint16_t*)VirtualAlloc(nullptr, (kCodigoFin - kCodigoIni) * 2, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    g.cargado = true;
}
static void NTAPI FaseTls(PVOID, DWORD motivo, PVOID) {
    if (motivo == DLL_PROCESS_ATTACH) FaseCargar();
}
#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma comment(linker, "/INCLUDE:_g_FaseTls")
#pragma section(".CRT$XLB", long, read)
extern "C" __declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK g_FaseTls = FaseTls;

// Punto de entrada del exe (/ENTRY:FaseEntrada).
extern "C" void __cdecl FaseEntrada() {
    if (!g.cargado) {
        MessageBoxA(nullptr, "gta_sa_standalone: falta --ruta-original <gta_sa.exe> (o la variable GTASA_ORIGINAL_EXE); se usa solo como fuente de datos", "gta_sa_standalone", MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 2);
    }
    SetCurrentDirectoryA(g.dirJuego);
    LogT("FaseEntrada: ExeCrt::StandaloneStartup (estado del CRT del EXE en sus direcciones)");
    if (const int codigo = ExeCrt::StandaloneStartup()) {
        LogT("ERROR: StandaloneStartup devolvio 0x%X", codigo);
        Salir((UINT)codigo);
    }
    LogT("StandaloneStartup correcto (tabla C de __cinit completa); iniciando el CRT propio");
    RestaurarDynTls();
    WinMainCRTStartup();
    TerminateProcess(GetCurrentProcess(), 0);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    LogT("WinMain propio: CRT propio iniciado (inicializadores estaticos propios hechos)");
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_hiloPrincipal, 0, FALSE, DUPLICATE_SAME_ACCESS);
    CreateThread(nullptr, 0, Guardia, nullptr, 0, nullptr);
    LogT("DllMain del proyecto: instala los ganchos (escriben su JMP en la reserva de INT3)");
    const BOOL ok = DllMain(GetModuleHandleW(nullptr), DLL_PROCESS_ATTACH, nullptr);
    LogT("DllMain devolvio %d", ok);
    RemoveVectoredExceptionHandler(g_vehHandle);
    g_vehHandle = AddVectoredExceptionHandler(1, Vectorizado);
    if (g.shims) { InstalarShims(); LogT("--shims: sin efecto (las funciones de camara y D3D9 ya estan reconstruidas con gancho)"); }
    {
        auto* nt = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
        auto* sec = IMAGE_FIRST_SECTION(nt);
        for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
            const uint32_t va = kBase + sec[i].VirtualAddress, vs = (sec[i].Misc.VirtualSize + 0xFFF) & ~0xFFFu;
            DWORD prot = ProteccionDe(sec[i].Characteristics), viejo = 0;
            const BOOL r = VirtualProtect((void*)va, vs, prot, &viejo);
            LogT("proteccion %-8.8s 0x%08X +0x%lX -> 0x%lX %s", (const char*)sec[i].Name, va, vs, prot, r ? "ok" : "FALLO");
        }
    }
    // Tercera ronda: primero lo que define DatosExe (93 de las 175 filas no-CRT: constructores con su C++, valores iniciales), despues el puente solo de lo que falta.
    // No puede ir en ExeCrt::Cinit (tras RunX87StaticInitializers): ahi todavia no han corrido los inicializadores dinamicos del C++ propio y las referencias
    // `static inline auto& X = StaticRef<...>(dir)` (CStats::StatTypesFloat...) son nulas (fallo real: CPlayerPedData::SetInitialState -> CStats::GetFatAndMuscleModifier lee 0xA0).
    if (!g.sinInit) { DatosExe::RunStaticInitializers(); LogT("DatosExe::RunStaticInitializers ejecutado"); }
    if (!g.sinInit) EjecutarInicializadoresEstaticos();
    LogT("llamando a NOTSA_WinMain (WinMain reconstruido)");
    const int r = NOTSA_WinMain(hInst, hPrev, cmd, show);
    LogT("NOTSA_WinMain devolvio %d", r);
    if (g.log) FlushFileBuffers(g.log);
    return r;
}
#endif // GTASA_STANDALONE_EXE
