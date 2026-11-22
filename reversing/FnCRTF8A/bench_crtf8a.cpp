// Banco CRTF8A: pares EXE original vs C++ reconstruido para (1) los cuerpos de destructores de la familia std::exception (0x822E83, 0x822EBF, 0x822EFA, 0x822F94) y sus
// destructores con borrado escalar (0x822F40, 0x822F5C, 0x822F78, 0x822FBD), (2) __crtGetEnvironmentStringsA (0x82DBC8), __setargv (0x82DB26, con parse_cmdline 0x82D9BA dentro),
// __setenvp (0x82D8F3) y (3) _doexit (0x823BDB) con retcaller = 1 (con retcaller = 0 sale por ExitProcess y no se puede comparar: se lee el desensamblado, ver README).
// Mismo metodo que FnCRT1A..5A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion; el juego no se ejecuta.
// El estado del CRT vive en las direcciones del EXE y es el mismo para las dos versiones; cada caso se ejecuta con el EXE y luego con el codigo propio sobre copias iguales de la entrada.
// Uso: bench_crtf8a.exe <ruta del EXE> [--casos N] [--semilla S] [--solo dtor|args|env|exit]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#define CRT_BENCH
#include "CrtString.cpp"
#include "CrtMem.cpp"
// Montón con fallo inyectable: el n-esimo HeapAlloc (n >= 0) devuelve NULL (los dos caminos, el del EXE por su IAT y el propio por esta macro)
static long g_failAt = -1;
static LPVOID WINAPI FailHeapAlloc(HANDLE h, DWORD f, SIZE_T n) { if (g_failAt >= 0 && g_failAt-- == 0) return nullptr; return HeapAlloc(h, f, n); }
#define HeapAlloc FailHeapAlloc
#include "CrtHeap.cpp"
#undef HeapAlloc
#include "CrtLocale.h"
#include "CrtException.h"
#include "CrtLowIo.h"
// Piezas ajenas a este banco, con el mismo cuerpo del EXE o sin uso, para no arrastrar la capa de E/S, el locale ni los lanzadores (CRT3A los banqueo):
namespace ExeCrt {
LocInfo* __cdecl CurrentLocInfo() { return nullptr; }
int __cdecl IsctypeMt(LocInfo*, int, int) { return 0; }
int __cdecl Ioinit() { return 0; }
int __cdecl Initstdio() { return 0; }
void __cdecl RunX87StaticInitializers() {}
}
void notsa::StdException::Destruct() { m_vtbl = (const void*)0x887E80; if (m_doFree) ExeCrt::Free((void*)m_what); } // 0x822E2F (cmp [ecx+8],0; vtbl; free(+4))
// operator delete global = el del CRT del EXE (en la DLL lo hace el operator delete del proyecto); sin esto los bloques del monton del CRT del EXE irian al free de libcmt
static bool g_crtReady = false;
void operator delete(void* block) noexcept {
    if (!block) return;
    const HANDLE heap = *(HANDLE*)0xC9C2F4;
    if (g_crtReady && heap && HeapValidate(heap, 0, block)) ExeCrt::OperatorDelete(block); else free(block);
}
#include "CrtState.cpp"
#include "CrtExceptionDtor.cpp"
#include "CrtOnexit.cpp"
// CrtStartup.cpp se compila aparte (bench_startup.obj): sus ayudantes anonimos chocan con los de CrtState.cpp dentro de una sola unidad
#include "CrtStartup.h"

static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    for (auto* d0 = (IMAGE_IMPORT_DESCRIPTOR*)(0x400000 + dir.VirtualAddress); d0->Name; d0++) {
        if (_stricmp((const char*)(0x400000 + d0->Name), "KERNEL32.dll") != 0) continue;
        HMODULE k = GetModuleHandleA("kernel32.dll");
        auto ilt = (uint32_t*)(0x400000 + (d0->OriginalFirstThunk ? d0->OriginalFirstThunk : d0->FirstThunk)); auto iat = (void**)(0x400000 + d0->FirstThunk);
        for (int i = 0; ilt[i]; i++) if (!(ilt[i] & 0x80000000)) { void* p = (void*)GetProcAddress(k, (const char*)(0x400000 + ilt[i] + 2)); if (p) iat[i] = p; }
    }
    return true;
}

typedef int(__cdecl* IFn)();
static void init_crt_state() {
    *(void**)0x8581F8 = (void*)&FailHeapAlloc;                    // HeapAlloc del EXE con el mismo fallo inyectable
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();   // montón del CRT = montón del proceso (__active_heap 1)
    *(DWORD*)0x8E3110 = TlsAlloc();
    *(int*)0xC9AC08 = 2;                                          // _osplatform
    ((IFn)0x82AC74)();                                            // _mtinitlocks: cerrojos estaticos
    *(int*)0xC9D430 = 1;                                          // __initmbctable ya hecha (no se ejecuta _setmbcp)
    // bloque de datos del hilo (errno etc.) para Getptd
    auto* ptd = (uint8_t*)calloc(1, 0x88); *(void**)(ptd + 0x5C) = (void*)0x8E3510; TlsSetValue(*(DWORD*)0x8E3110, ptd);
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint64_t fails = 0, total = 0;
#define CHECK(cond, ...) do { total++; if (!(cond)) { if (fails++ < 12) { printf("DIFERENCIA: " __VA_ARGS__); printf("\n"); } } } while (0)

// ------------------------------------------------------------------ cuerpos de destructores
static size_t busy_blocks(HANDLE h) {
    size_t n = 0; PROCESS_HEAP_ENTRY e; e.lpData = nullptr;
    HeapLock(h); while (HeapWalk(h, &e)) if (e.wFlags & PROCESS_HEAP_ENTRY_BUSY) n++; HeapUnlock(h);
    return n;
}
typedef void(__fastcall* BodyFn)(void*, void*);
typedef void*(__fastcall* DelFn)(void*, void*, uint32_t);
static void run_dtor(uint64_t n) {
    HANDLE hExe = HeapCreate(0, 0, 0), hOurs = HeapCreate(0, 0, 0);
    struct Body { uintptr_t exe; BodyFn ours; } bodies[] = {
        {0x822E83, &ExeCrt::DtorBody822E83}, {0x822EBF, &ExeCrt::DtorBody822EBF}, {0x822EFA, &ExeCrt::DtorBody822EFA}, {0x822F94, &ExeCrt::DtorBody822F94}};
    struct Del { uintptr_t exe; DelFn ours; } dels[] = {
        {0x822F40, &ExeCrt::DeletingDtor822F40}, {0x822F5C, &ExeCrt::DeletingDtor822F5C}, {0x822F78, &ExeCrt::DeletingDtor822F78}, {0x822FBD, &ExeCrt::DeletingDtor822FBD}};
    for (uint64_t c = 0; c < n; c++) {
        const bool hasText = rnd(4) != 0;
        const int len = rnd(60);
        for (int which = 0; which < 4; which++) {
            const uint32_t vt0 = (uint32_t)rng(), fl0 = rnd(3);   // mismos valores en las dos pasadas
            // cuerpo suelto, sobre un objeto de pila
            for (int pass = 0; pass < 2; pass++) {
                *(HANDLE*)0xC9C2F4 = pass ? hOurs : hExe;
                char* text = nullptr;
                if (hasText) { text = (char*)HeapAlloc(pass ? hOurs : hExe, 0, len + 1); memset(text, 'x', len); text[len] = 0; }
                uint32_t obj[3] = { vt0, (uint32_t)(uintptr_t)text, fl0 };
                const uint32_t doFree = obj[2]; (void)doFree;
                const size_t before = busy_blocks(pass ? hOurs : hExe);
                if (pass) bodies[which].ours(obj, nullptr); else ((BodyFn)bodies[which].exe)(obj, nullptr);
                static uint32_t resObj[2][3]; static size_t resBlocks[2];
                memcpy(resObj[pass], obj, 12); resObj[pass][1] = obj[1] ? 1 : 0; resBlocks[pass] = before - busy_blocks(pass ? hOurs : hExe);
                if (pass) {
                    CHECK(resObj[0][0] == resObj[1][0] && resObj[0][1] == resObj[1][1] && resObj[0][2] == resObj[1][2], "cuerpo %#llx: objeto (vtbl %x/%x, what %x/%x, flag %u/%u)", (unsigned long long)bodies[which].exe,
                          resObj[0][0], resObj[1][0], resObj[0][1], resObj[1][1], resObj[0][2], resObj[1][2]);
                    CHECK(resBlocks[0] == resBlocks[1], "cuerpo %#llx: bloques liberados %zu / %zu", (unsigned long long)bodies[which].exe, resBlocks[0], resBlocks[1]);
                }
                if (text && which != 3 && !(obj[2])) HeapFree(pass ? hOurs : hExe, 0, text); // bandera 0: el texto sigue vivo (no es del objeto)
                else if (text && which == 3 && false) {}
            }
            // destructor con borrado escalar, objeto en el monton del CRT; bandera 0 o 1
            for (int flag = 0; flag < 2; flag++) {
                size_t freed[2] = {0, 0}; uint32_t res[2][3] = {};
                for (int pass = 0; pass < 2; pass++) {
                    HANDLE h = pass ? hOurs : hExe; *(HANDLE*)0xC9C2F4 = h;
                    auto* self = (uint32_t*)HeapAlloc(h, 0, 12);
                    char* text = hasText ? (char*)HeapAlloc(h, 0, len + 1) : nullptr; if (text) { memset(text, 'y', len); text[len] = 0; }
                    self[0] = 0x12345678; self[1] = (uint32_t)(uintptr_t)text; self[2] = hasText ? 1 : 0;
                    const size_t before = busy_blocks(h);
                    void* ret = pass ? dels[which].ours(self, nullptr, flag) : ((DelFn)dels[which].exe)(self, nullptr, flag);
                    freed[pass] = before - busy_blocks(h);
                    CHECK(ret == self, "borrado escalar %#llx devuelve otro puntero", (unsigned long long)dels[which].exe);
                    if (!flag) { memcpy(res[pass], self, 12); res[pass][1] = self[1] ? 1 : 0; HeapFree(h, 0, self); }
                }
                CHECK(freed[0] == freed[1], "borrado escalar %#llx bandera %d: bloques %zu / %zu", (unsigned long long)dels[which].exe, flag, freed[0], freed[1]);
                if (!flag) CHECK(!memcmp(res[0], res[1], 12), "borrado escalar %#llx: objeto", (unsigned long long)dels[which].exe);
            }
        }
    }
    *(HANDLE*)0xC9C2F4 = GetProcessHeap();
}

// ------------------------------------------------------------------ linea de comandos
static std::string random_cmdline() {
    static const char* atoms[] = {" ", " ", "\t", "\"", "\"", "\\", "\\\\", "\\\"", "a", "bb", "C:\\dir", "x=y", "-w", "\"\"", "\xE3\x81\x82", "\x81\x40", "\x90\xFF", "\xFD", "é"};
    std::string s; const int k = rnd(8) == 0 ? 0 : 1 + rnd(14);
    for (int i = 0; i < k; i++) s += atoms[rnd(sizeof atoms / sizeof *atoms)];
    if (rnd(6) == 0) s = "\"" + s;
    if (rnd(4) == 0) { std::string p = "prog.exe"; if (rnd(2)) p = "\"C:\\Program Files\\g.exe\""; s = p + (rnd(2) ? " " : "") + s; }
    return s;
}
struct ArgsState { int ret; int argc; std::vector<std::string> argv; std::string pgm; };
static ArgsState snapshot(int ret) {
    ArgsState st; st.ret = ret < 0 ? -1 : 0;
    if (ret >= 0) {
        st.argc = *(int*)0xC9AC1C; char** av = *(char***)0xC9AC20;
        for (int i = 0; i < st.argc; i++) st.argv.push_back(av[i]);
        CHECK(av[st.argc] == nullptr, "argv sin nulo final");
        st.pgm = *(char**)0xC9AC38;
    }
    return st;
}
static void run_args(uint64_t n) {
    for (uint64_t c = 0; c < n; c++) {
        // tabla de bytes iniciales: ninguna, o la de un juego de doble byte (0x81..0x9F, 0xE0..0xFC)
        memset((void*)0xC9C0A0, 0, 0x101);
        if (rnd(2)) for (int b = 0; b < 256; b++) if ((b >= 0x81 && b <= 0x9F) || (b >= 0xE0 && b <= 0xFC)) *(uint8_t*)(0xC9C0A0 + 1 + b) |= 4;
        const std::string cmd = random_cmdline();
        static std::vector<char> buf; buf.assign(cmd.begin(), cmd.end()); buf.push_back(0); buf.push_back(0);
        const bool empty = rnd(10) == 0;
        for (int fail = (rnd(3) == 0 ? (int)rnd(2) : -1); ; fail = -1) {
            ArgsState st[2];
            for (int pass = 0; pass < 2; pass++) {
                *(char**)0xC9C414 = empty ? (rnd(2) ? (char*)"" : nullptr) : buf.data();
                if (empty && pass == 1) {}
                g_failAt = fail;
                const int r = pass ? ExeCrt::Setargv() : ((IFn)0x82DB26)();
                g_failAt = -1;
                st[pass] = snapshot(r);
            }
            CHECK(st[0].ret == st[1].ret && st[0].argc == st[1].argc && st[0].argv == st[1].argv && st[0].pgm == st[1].pgm, "Setargv linea [%s] fallo=%d: argc %d/%d ret %d/%d", cmd.c_str(), fail, st[0].argc, st[1].argc, st[0].ret, st[1].ret);
            break;
        }
    }
}

// ------------------------------------------------------------------ entorno
struct EnvDump { std::vector<std::string> v; int ret; int inited; };
static std::string block_to_string(const char* p) { const char* e = p; while (*e || e[1]) ++e; return std::string(p, e - p + 2); }
static void run_env(uint64_t n) {
    for (uint64_t c = 0; c < n; c++) {
        // variables de entorno aleatorias (incluye las "=C:" de Windows)
        const int k = rnd(6);
        for (int i = 0; i < k; i++) {
            std::string name = "BENCHF8_" + std::to_string(rnd(40)), val; const int len = rnd(30); for (int j = 0; j < len; j++) val += (char)(1 + rnd(254)); if (val.find('\0') != std::string::npos) val.clear();
            if (rnd(3) == 0) SetEnvironmentVariableA(name.c_str(), nullptr); else SetEnvironmentVariableA(name.c_str(), val.c_str());
        }
        // __crtGetEnvironmentStringsA, con f_use sin decidir, ancha y ANSI
        for (int kind : {0, 1, 2}) {
            char* a = nullptr; char* b = nullptr;
            *(int*)0xC9AEE0 = kind; a = ((char*(__cdecl*)())0x82DBC8)();
            *(int*)0xC9AEE0 = kind; b = ExeCrt::CrtGetEnvironmentStrings();
            CHECK((a == nullptr) == (b == nullptr), "GetEnvironmentStrings: nulo distinto (f_use %d)", kind);
            if (a && b) CHECK(block_to_string(a) == block_to_string(b), "GetEnvironmentStrings: contenido distinto (f_use %d)", kind);
            CHECK(*(int*)0xC9AEE0 == (kind == 0 ? 1 : kind), "f_use final %d", *(int*)0xC9AEE0);
            if (kind == 0) { *(int*)0xC9AEE0 = 0; g_failAt = 0; char* x = ((char*(__cdecl*)())0x82DBC8)(); g_failAt = 0; char* y = ExeCrt::CrtGetEnvironmentStrings(); g_failAt = -1; CHECK((x == nullptr) == (y == nullptr), "GetEnvironmentStrings sin memoria"); }
            if (kind == 0 && a && b) {
                // __setenvp sobre copias iguales del bloque, con fallo inyectado en la reserva n-esima
                const std::string blk = block_to_string(a);
                const int vars = (int)std::count(blk.begin(), blk.end(), '\0');
                for (int fail = -1; fail <= vars + 1; fail++) {
                    EnvDump d[2];
                    for (int pass = 0; pass < 2; pass++) {
                        char* copy = (char*)HeapAlloc(GetProcessHeap(), 0, blk.size()); memcpy(copy, blk.data(), blk.size());
                        *(char**)0xC9AC50 = copy; *(char***)0xC9AC28 = (char**)0x1; *(int*)0xC9D424 = 0;
                        g_failAt = fail;
                        d[pass].ret = pass ? ExeCrt::Setenvp() : ((IFn)0x82D8F3)();
                        g_failAt = -1;
                        d[pass].inited = *(int*)0xC9D424;
                        if (d[pass].ret == 0) { for (char** e = *(char***)0xC9AC28; *e; ++e) d[pass].v.push_back(*e); }
                        d[pass].v.push_back(*(char***)0xC9AC28 ? "ENV" : "NOENV");
                        d[pass].v.push_back(*(char**)0xC9AC50 ? "BLK" : "NOBLK");
                    }
                    CHECK(d[0].ret == d[1].ret && d[0].inited == d[1].inited && d[0].v == d[1].v, "Setenvp fallo=%d: ret %d/%d vars %zu/%zu init %d/%d", fail, d[0].ret, d[1].ret, d[0].v.size(), d[1].v.size(), d[0].inited, d[1].inited);
                }
                // sin bloque
                for (int pass = 0; pass < 2; pass++) { *(char**)0xC9AC50 = nullptr; *(char***)0xC9AC28 = (char**)0x7; const int r = pass ? ExeCrt::Setenvp() : ((IFn)0x82D8F3)(); CHECK(r == -1 && *(char***)0xC9AC28 == (char**)0x7, "Setenvp sin bloque"); }
            }
        }
    }
}

// ------------------------------------------------------------------ _doexit con retcaller = 1
static std::vector<int> g_calls;
static void __cdecl T1() { g_calls.push_back(1); }
static void __cdecl T2() { g_calls.push_back(2); }
static void __cdecl T3() { g_calls.push_back(3); }
static void __cdecl Pre1() { g_calls.push_back(11); }
static void __cdecl Pre2() { g_calls.push_back(12); }
static void __cdecl Post1() { g_calls.push_back(21); }
static void __cdecl Post2() { g_calls.push_back(22); }
static void __cdecl Reg() { g_calls.push_back(5); ExeCrt::Onexit((void*)&T3); } // registra otra durante la ejecucion
struct ExitResult { std::vector<int> calls; uint32_t flagExit, flagTerm; uint32_t end; };
static void run_exit(uint64_t n) {
    for (uint64_t c = 0; c < n; c++) {
        const int nfun = rnd(6); const bool withReg = rnd(3) == 0; const int quick = rnd(2); const int code = rnd(100); const bool nullPre = rnd(5) == 0;
        std::vector<int> plan; for (int i = 0; i < nfun; i++) plan.push_back(rnd(7) == 0 ? 0 : 1 + (int)rnd(3));
        ExitResult r[2];
        for (int pass = 0; pass < 2; pass++) {
            // tabla de atexit propia (0x80 bytes) y tablas de terminadores del EXE sustituidas por registradores
            void* table = ExeCrt::Malloc(0x80); *(uint32_t*)0xC9D42C = (uint32_t)(uintptr_t)table; *(uint32_t*)0xC9D428 = (uint32_t)(uintptr_t)table;
            void(__cdecl * fs[4])() = { nullptr, &T1, &T2, &T3 };
            for (int id : plan) ExeCrt::Onexit((void*)fs[id]);
            if (withReg) ExeCrt::Onexit((void*)&Reg);
            *(void**)0x8A5A34 = (void*)&Pre1; *(void**)0x8A5A38 = nullPre ? nullptr : (void*)&Pre2; *(void**)0x8A5A40 = (void*)&Post1; *(void**)0x8A5A44 = (void*)&Post2;
            *(uint32_t*)0xC9AC44 = 0; *(uint8_t*)0xC9AC40 = 0xAB; *(uint32_t*)0xC9AC48 = 0;
            g_calls.clear();
            if (pass) ExeCrt::DoExit(code, quick, 1); else ((void(__cdecl*)(int, int, int))0x823BDB)(code, quick, 1);
            r[pass].calls = g_calls; r[pass].flagExit = *(uint32_t*)0xC9AC44; r[pass].flagTerm = *(uint8_t*)0xC9AC40; r[pass].end = *(uint32_t*)0xC9D428 - *(uint32_t*)0xC9D42C;
            ExeCrt::Free(table);
        }
        CHECK(r[0].calls == r[1].calls && r[0].flagExit == r[1].flagExit && r[0].flagTerm == r[1].flagTerm && r[0].end == r[1].end, "DoExit nfun=%d quick=%d reg=%d: llamadas %zu/%zu flags %u/%u %u/%u fin %u/%u", nfun, quick, (int)withReg, r[0].calls.size(), r[1].calls.size(),
              r[0].flagExit, r[1].flagExit, r[0].flagTerm, r[1].flagTerm, r[0].end, r[1].end);
    }
}

static LONG WINAPI CrashFilter(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(CrashFilter);
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) { printf("uso: bench_crtf8a.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo dtor|args|env|exit]\n"); return 2; }
    uint64_t n = 3000, seed = 1; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    rng.seed(seed);
    if (!map_exe(argv[1])) return 2;
    init_crt_state();
    g_crtReady = true;
    int rc = 0;
    auto section = [&](const char* name, const char* label, void (*fn)(uint64_t), uint64_t cases) {
        if (!solo.empty() && solo != name) return;
        const uint64_t f0 = fails, t0 = total; fn(cases);
        printf("%s: %llu comparaciones, %llu diferencias\n", label, (unsigned long long)(total - t0), (unsigned long long)(fails - f0));
    };
    section("dtor", "cuerpos de destructores std::exception", run_dtor, n);
    section("args", "__setargv / parse_cmdline", run_args, n * 4);
    section("env", "__crtGetEnvironmentStringsA / __setenvp", run_env, n / 10 + 5);
    section("exit", "_doexit (retcaller = 1)", run_exit, n);
    if (fails) rc = 1;
    printf("RESULTADO: %s (%llu comparaciones, %llu diferencias)\n", rc ? "HAY DIFERENCIAS" : "TODO IGUAL", (unsigned long long)total, (unsigned long long)fails);
    return rc;
}
