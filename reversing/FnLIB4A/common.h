// Parte comun de los bancos LIB4A: mapa del EXE original (solo lectura) en 0x400000, arena determinista, RNG y huellas.
// El EXE se copia en memoria dentro de este proceso de 32 bits (no se ejecuta el juego) y sus funciones se llaman por direccion.
#pragma once
#define NOMINMAX
#include <windows.h>
#include <setjmp.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16; typedef uint64_t u64;

struct Rng {
    u64 s;
    explicit Rng(u64 seed = 1) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567) { next(); next(); }
    u32 next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return (u32)(s >> 16); }
    u32 below(u32 n) { return n ? next() % n : 0; }
    bool chance(u32 pct) { return below(100) < pct; }
};
static Rng* R;

static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<u8> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0xCB1000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(u8*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    return true;
}

// ---- arena
static const size_t ARENA = 64u << 20;
static u8* g_ar; static size_t g_brk; static const size_t GAP = 64;
static std::vector<u32> g_log;
static inline void L(u32 v) { g_log.push_back(v); if (g_log.size() > 40000000) { printf("registro desbordado (bucle infinito en callbacks): ultimos %08x %08x %08x %08x %08x %08x", g_log[g_log.size()-6], g_log[g_log.size()-5], g_log[g_log.size()-4], g_log[g_log.size()-3], g_log[g_log.size()-2], g_log[g_log.size()-1]); ExitProcess(3); } }
static void* ar_alloc(size_t n, size_t align = 16) {
    g_brk = (g_brk + align - 1) & ~(align - 1);
    if (g_brk + n + GAP > ARENA) return nullptr;
    void* p = g_ar + g_brk; g_brk += n + GAP; return p;
}
static void* ar_alloc_fill(size_t n) { u8* p = (u8*)ar_alloc(n); if (p) memset(p, 0xCD, n); return p; }
static u32 fnv(const void* p, size_t n, u32 h = 2166136261u) { auto b = (const u8*)p; for (size_t i = 0; i < n; i++) h = (h ^ b[i]) * 16777619u; return h; }

// ---- parches en vivo: sustituye una funcion del EXE por otra (jmp rel32), como hace RH_ScopedGlobalInstall
struct Patch { u32 addr; void* fn; u8 orig[5]; bool on; };
static void patch_on(Patch& p) {
    if (p.on) return; memcpy(p.orig, (void*)p.addr, 5);
    if (!p.fn) { const u8 t[3] = {0x33, 0xC0, 0xC3}; memcpy((void*)p.addr, t, 3); p.on = true; return; } // trampa: xor eax,eax; ret
    u8 j[5]; j[0] = 0xE9; *(u32*)(j + 1) = (u32)((u8*)p.fn - (u8*)p.addr - 5);
    memcpy((void*)p.addr, j, 5); p.on = true;
}
static void patch_off(Patch& p) { if (!p.on) return; memcpy((void*)p.addr, p.orig, 5); p.on = false; }
