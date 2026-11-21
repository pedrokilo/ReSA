// Banco NEWDEL: pares EXE original vs C++ reconstruido para operator new / new[] / delete / delete[] y el manejador de new (_set_new_handler 0x826A2D,
// _query_new_handler 0x826A51, _callnewh 0x826A57) del CRT estatico del EXE. Mismo metodo que FnCRT1A..5A: el gta_sa.exe original (solo lectura) se mapea en
// 0x400000 dentro de este proceso de 32 bits y se llama por su direccion; el juego no se ejecuta.
// Las funciones del EXE leen _crtheap (0xC9C2F4), __active_heap (0xC9C2F8), _newmode (0xC9ADCC) y el manejador (0xC9AC68) de su propia imagen y llaman a
// HeapAlloc/HeapFree por la IAT; cada implementacion trabaja en SU montón (dos montones identicos, se cambia _crtheap antes de cada llamada).
// Se compara: nulo/no nulo, HeapSize del bloque, numero de llamadas al manejador de new, si la llamada lanzo (el manejador lanza una excepcion de C++ de tipo int al
// agotar sus reintentos: asi se recorre la ruta "sin memoria" de operator new sin la excepcion std::bad_alloc del EXE), valor devuelto y estado de 0xC9AC68 tras
// _set_new_handler/_query_new_handler/_callnewh, y la validez de los dos montones al final de cada secuencia.
// Uso: bench_newdel.exe <ruta del EXE> [--casos N] [--semilla S] [--solo newdel|handler]   (devuelve 0 si todo coincide)
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
#include "CrtHeap.cpp"
#include "CrtState.cpp"

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
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();
    *(DWORD*)0x8E3110 = TlsAlloc();
    *(int*)0xC9AC08 = 2;
    ((IFn)0x82AC74)(); // _mtinitlocks: cerrojos estaticos
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }

// ------------------------------------------------------------------ manejador de new
static int g_calls, g_limit, g_mode; // modo 1: devuelve 1 hasta g_limit llamadas y despues lanza int; modo 2: devuelve 0 (sin reintento); modo 3: devuelve 1 siempre hasta g_limit y despues 0
static int __cdecl handler_a(unsigned) { g_calls++; if (g_mode == 1) { if (g_calls <= g_limit) return 1; throw 7; } if (g_mode == 2) return 0; return g_calls <= g_limit ? 1 : 0; }
static int __cdecl handler_b(unsigned) { g_calls++; return 5; }  // cualquier valor distinto de cero cuenta como "reintentar"
static int __cdecl handler_c(unsigned) { g_calls++; return 0; }

typedef void*(__cdecl* PNew)(unsigned); typedef void(__cdecl* PDel)(void*);
struct Impl { PNew nw, nwa; PDel del, dela; HANDLE heap; std::vector<uint8_t*> pool; };

static size_t busy(HANDLE heap) { // bloques ocupados del montón (HeapWalk)
    size_t n = 0; PROCESS_HEAP_ENTRY e; e.lpData = nullptr;
    while (HeapWalk(heap, &e)) if (e.wFlags & PROCESS_HEAP_ENTRY_BUSY) n++;
    return n;
}

static void run_newdel(uint64_t nseq, uint64_t& fails, uint64_t* oc) {
    Impl H[2] = {{(PNew)0x82119A, (PNew)0x821195, (PDel)0x8214BD, (PDel)0x8213AE, HeapCreate(0, 0x1000, 0), {}},
                 {(PNew)ExeCrt::OperatorNew, (PNew)ExeCrt::OperatorNewArray, (PDel)ExeCrt::OperatorDelete, (PDel)ExeCrt::OperatorDeleteArray, HeapCreate(0, 0x1000, 0), {}}};
    auto report = [&](const char* what, uint64_t seq, int op) { fails++; if (fails <= 8) printf("DIFERENCIA new/delete: %s (secuencia %llu, operacion %d)\n", what, (unsigned long long)seq, op); };
    auto setheap = [&](int i) { *(HANDLE*)0xC9C2F4 = H[i].heap; };
    for (uint64_t sq = 0; sq < nseq; sq++) {
        g_mode = 1; g_limit = (int)rnd(4); // modo 1: reintenta g_limit veces y despues lanza int
        *(int*)0xC9ADCC = (int)rnd(2); // _newmode: no cambia el resultado de operator new, pero se varia
        *(void**)0xC9AC68 = rnd(5) ? (void*)handler_a : nullptr;
        bool haveHandler = *(void**)0xC9AC68 != nullptr;
        int nops = 1 + (int)rnd(60);
        for (int k = 0; k < nops; k++) {
            int op = (int)rnd(5); uint32_t sz; uint32_t r = rnd(100);
            if (r < 55) sz = rnd(64); else if (r < 85) sz = rnd(8192); else if (r < 93) sz = rnd(1 << 20); else if (r < 96) sz = 0xFFFFFFE0u + rnd(32); else if (r < 98) sz = 0x7FFFFFF0u + rnd(32); else sz = 0x80000000u + rnd(0x7FFFFF00u);
            // sin manejador, un fallo lanza std::bad_alloc por 0x8212ED (maquinaria de excepciones del EXE, no disponible en el banco): solo tamaños que caben
            if (!haveHandler && sz > (1u << 20)) sz = rnd(8192);
            oc[op]++;
            uint32_t idx = H[0].pool.empty() ? 0 : rnd((uint32_t)H[0].pool.size());
            uint8_t* res[2] = {nullptr, nullptr}; int calls[2] = {0, 0}; bool thrown[2] = {false, false};
            for (int i = 0; i < 2; i++) {
                setheap(i); g_calls = 0; Impl& h = H[i];
                uint8_t* blk = (!h.pool.empty() && h.pool[idx]) ? h.pool[idx] : nullptr;
                try {
                    switch (op) {
                    case 0: res[i] = (uint8_t*)h.nw(sz); break;
                    case 1: res[i] = (uint8_t*)h.nwa(sz); break;
                    case 2: h.del(blk); break;
                    case 3: h.dela(blk); break;
                    case 4: h.del(nullptr); h.dela(nullptr); break;
                    }
                } catch (int) { thrown[i] = true; }
                calls[i] = g_calls;
            }
            if (thrown[0] != thrown[1]) report("uno lanza y el otro no", sq, op);
            if (calls[0] != calls[1]) report("llamadas distintas al manejador de new", sq, op);
            if ((res[0] == nullptr) != (res[1] == nullptr)) report("uno devuelve nulo y el otro no", sq, op);
            if (res[0] && res[1]) {
                setheap(0); SIZE_T s0 = HeapSize(H[0].heap, 0, res[0]); setheap(1); SIZE_T s1 = HeapSize(H[1].heap, 0, res[1]);
                if (s0 != s1) report("HeapSize distinto", sq, op);
                memset(res[0], 0xA5, std::min<SIZE_T>(s0, 64)); memset(res[1], 0xA5, std::min<SIZE_T>(s1, 64));
            }
            for (int i = 0; i < 2; i++) {
                Impl& h = H[i];
                if (op == 0 || op == 1) h.pool.push_back(res[i]);
                else if ((op == 2 || op == 3) && !h.pool.empty()) h.pool[idx] = nullptr;
            }
            if (H[0].pool.size() > 300) for (int i = 0; i < 2; i++) { setheap(i); for (auto p : H[i].pool) if (p) H[i].del(p); H[i].pool.clear(); }
        }
        for (int i = 0; i < 2; i++) { setheap(i); for (auto p : H[i].pool) if (p) H[i].del(p); H[i].pool.clear(); if (!HeapValidate(H[i].heap, 0, nullptr)) report("montón corrupto", sq, -1); }
        if (busy(H[0].heap) != busy(H[1].heap) || busy(H[1].heap) != 0) report("bloques sin liberar al final (delete que no libera)", sq, -1);
    }
    *(void**)0xC9AC68 = nullptr;
}

// ------------------------------------------------------------------ _set_new_handler / _query_new_handler / _callnewh
typedef void*(__cdecl* PSet)(void*); typedef void*(__cdecl* PQuery)(); typedef int(__cdecl* PCall)(unsigned);
static void run_handler(uint64_t n, uint64_t& fails, uint64_t* oc) {
    PSet  set[2]   = {(PSet)0x826A2D, (PSet)ExeCrt::SetNewHandler};
    PQuery qry[2]  = {(PQuery)0x826A51, (PQuery)ExeCrt::QueryNewHandler};
    PCall call[2]  = {(PCall)0x826A57, (PCall)ExeCrt::CallNewHandler};
    void* handlers[] = {nullptr, (void*)handler_a, (void*)handler_b, (void*)handler_c, (void*)0x12345678};
    auto report = [&](const char* what, uint64_t i, int op) { fails++; if (fails <= 8) printf("DIFERENCIA manejador: %s (caso %llu, operacion %d)\n", what, (unsigned long long)i, op); };
    for (uint64_t c = 0; c < n; c++) {
        // estado inicial comun
        void* start = handlers[rnd(5)]; *(void**)0xC9AC68 = start;
        int op = (int)rnd(3); oc[op]++;
        void* arg = handlers[rnd(5)]; unsigned sz = rnd(2) ? rnd(100) : rnd(0xFFFFFFFFu);
        uintptr_t ret[2] = {0, 0}; void* after[2] = {nullptr, nullptr}; int calls[2] = {0, 0};
        for (int i = 0; i < 2; i++) {
            *(void**)0xC9AC68 = start; g_calls = 0; g_mode = 3; g_limit = 1;
            if (op == 0) ret[i] = (uintptr_t)set[i](arg);
            else if (op == 1) ret[i] = (uintptr_t)qry[i]();
            else if (arg == (void*)0x12345678 || start == (void*)0x12345678) ret[i] = 0xDEAD; // no se llama a un puntero basura
            else ret[i] = (uintptr_t)call[i](sz);
            after[i] = *(void**)0xC9AC68; calls[i] = g_calls;
        }
        if (ret[0] != ret[1]) report("valor devuelto distinto", c, op);
        if (after[0] != after[1]) report("estado de 0xC9AC68 distinto", c, op);
        if (calls[0] != calls[1]) report("llamadas distintas al manejador", c, op);
    }
    *(void**)0xC9AC68 = nullptr;
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_newdel <gta_sa.exe> [--casos N] [--semilla S] [--solo newdel|handler]\n"); return 2; }
    uint64_t n = 60000, seed = 12345; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    if (!map_exe(argv[1])) return 2;
    init_crt_state();
    rng.seed(seed); int rc = 0;
    if (solo.empty() || solo == "newdel") {
        uint64_t fails = 0, oc[8] = {};
        run_newdel(n / 4, fails, oc);
        printf("new/new[]/delete/delete[]: %llu secuencias; operaciones new=%llu new[]=%llu delete=%llu delete[]=%llu delete(0)=%llu; %llu diferencias\n", (unsigned long long)(n / 4),
               (unsigned long long)oc[0], (unsigned long long)oc[1], (unsigned long long)oc[2], (unsigned long long)oc[3], (unsigned long long)oc[4], (unsigned long long)fails);
        if (fails) rc = 1;
    }
    if (solo.empty() || solo == "handler") {
        uint64_t fails = 0, oc[8] = {};
        run_handler(n, fails, oc);
        printf("_set_new_handler/_query_new_handler/_callnewh: %llu casos (set=%llu query=%llu callnewh=%llu); %llu diferencias\n", (unsigned long long)n, (unsigned long long)oc[0], (unsigned long long)oc[1], (unsigned long long)oc[2], (unsigned long long)fails);
        if (fails) rc = 1;
    }
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
