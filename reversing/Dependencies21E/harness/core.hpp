// Harness aislado del grupo E: mapea el codigo REAL del EXE (solo bytes, en su direccion original) en este proceso y lo compara con los candidatos C++.
// No ejecuta el juego ni los bancos: solo llama a las tres funciones puras con colaboradores falsos.
#pragma once
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <vector>
#include <string>
typedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef int32_t i32; typedef uint64_t u64;

static const char* kExePath = "D:\\Grand Theft Auto San Andreas\\gta-sa.exe";

inline void Die(const char* m) { fprintf(stderr, "FATAL: %s\n", m); exit(2); }

inline void MapExe() {
    FILE* f = fopen(kExePath, "rb");
    if (!f) Die("no se puede abrir el EXE");
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<u8> d(n); fread(d.data(), 1, n, f); fclose(f);
    u32 pe = *(u32*)&d[0x3c]; u16 nsec = *(u16*)&d[pe + 6]; u16 opt = *(u16*)&d[pe + 20];
    u32 base = *(u32*)&d[pe + 24 + 28], size = *(u32*)&d[pe + 24 + 56];
    // El proceso anfitrion (stub.cpp) ocupa [0x400000, 0xCC0000) con una imagen propia: aqui solo se hace RWX y se copian las secciones.
    MEMORY_BASIC_INFORMATION mi;
    if (!VirtualQuery((void*)(base + 0x1000), &mi, sizeof mi) || mi.AllocationBase != (void*)base) Die("el stub no ocupa la direccion base del EXE");
    DWORD old;
    if (!VirtualProtect((void*)(base + 0x1000), size - 0x1000, PAGE_EXECUTE_READWRITE, &old)) Die("VirtualProtect");
    memset((void*)(base + 0x1000), 0, size - 0x1000);
    u32 o = pe + 24 + opt;
    for (int i = 0; i < nsec; ++i) {
        u32 vs = *(u32*)&d[o + 8 + i * 40], va = *(u32*)&d[o + 12 + i * 40], rs = *(u32*)&d[o + 16 + i * 40], rp = *(u32*)&d[o + 20 + i * 40];
        u32 cp = rs < vs ? rs : vs;
        memcpy((void*)(base + va), &d[rp], cp);
    }
}
inline void Jmp(u32 at, void* target) {
    u8* p = (u8*)at; DWORD old; VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old);
    p[0] = 0xE9; *(i32*)(p + 1) = (i32)((u32)target - at - 5);
}

// ---- estado x87 ----
inline u16 gCw = 0x007F;   // palabra de control inicial (24 bits por defecto)
struct Fp { u8 b[108]; };
inline u16 FpCw(const Fp& f) { return *(u16*)&f.b[0]; }
inline u16 FpSw(const Fp& f) { return *(u16*)&f.b[4]; }
inline u16 FpTw(const Fp& f) { return *(u16*)&f.b[8]; }
inline int FpTop(const Fp& f) { return (FpSw(f) >> 11) & 7; }

inline Fp MakeBaseFp(u16 cw, u16 swBits) {
    Fp f; memset(&f, 0, sizeof f);
    u8 tmp[108];
    __asm { fnsave tmp }       // finit implicito: pila vacia
    memcpy(f.b, tmp, 108);
    *(u16*)&f.b[0] = cw;
    *(u16*)&f.b[4] = swBits;   // incluye TOP y condiciones; no se activan excepciones
    *(u16*)&f.b[8] = 0xFFFF;   // todos vacios
    return f;
}

// ---- llamada cruda ----
static u32 g_fn, g_ecx, g_edx, g_eax, g_nargs, g_args[16], g_cleanup;
static u32 g_saved_esp, g_esp0, g_esp1;
static u32 g_o_eax, g_o_ecx, g_o_edx, g_o_ebx, g_o_esi, g_o_edi, g_o_ebp;
static u8  g_baseFp[108];
static u8  g_outFp[108];
static const u32 kEbx = 0x0B0B0B0B, kEsi = 0x51515151, kEdi = 0xD1D1D1D1, kEbp = 0xBBBBBBBB;

__declspec(noinline) inline void RunRaw() {
    __asm {
        push ebx
        push esi
        push edi
        push ebp
        mov g_saved_esp, esp
        mov ebx, 0x0B0B0B0B
        mov esi, 0x51515151
        mov edi, 0xD1D1D1D1
        mov ebp, 0xBBBBBBBB
        frstor g_baseFp
        mov g_esp0, esp
        mov ecx, g_nargs
    L_push:
        test ecx, ecx
        jz L_go
        push dword ptr g_args[ecx * 4 - 4]
        dec ecx
        jmp L_push
    L_go:
        mov ecx, g_ecx
        mov edx, g_edx
        mov eax, g_eax
        call g_fn
        mov g_o_eax, eax
        mov g_o_ecx, ecx
        mov g_o_edx, edx
        mov g_esp1, esp
        mov g_o_ebx, ebx
        mov g_o_esi, esi
        mov g_o_edi, edi
        mov g_o_ebp, ebp
        fnsave g_outFp
        mov esp, g_saved_esp
        pop ebp
        pop edi
        pop esi
        pop ebx
    }
}

struct Res {
    u32 eax, ecx, edx; bool stackOk, nvOk; Fp fp;
};

// callee-clean: tras la llamada ESP debe volver a ESP0; cdecl: ESP0 - 4 * nargs.
inline Res Call(u32 fn, u32 ecx, u32 edx, u32 eax, const u32* args, u32 nargs, bool calleeClean, const Fp& base) {
    g_fn = fn; g_ecx = ecx; g_edx = edx; g_eax = eax; g_nargs = nargs;
    for (u32 i = 0; i < nargs; ++i) g_args[i] = args[i];
    memcpy(g_baseFp, base.b, 108);
    RunRaw();
    Res r; r.eax = g_o_eax; r.ecx = g_o_ecx; r.edx = g_o_edx;
    r.stackOk = calleeClean ? (g_esp1 == g_esp0) : (g_esp1 == g_esp0 - 4 * nargs);
    r.nvOk = g_o_ebx == kEbx && g_o_esi == kEsi && g_o_edi == kEdi && g_o_ebp == kEbp;
    memcpy(r.fp.b, g_outFp, 108);
    return r;
}

// ---- numeros ----
inline u32 Bits(float f) { u32 u; memcpy(&u, &f, 4); return u; }
inline float F(u32 u) { float f; memcpy(&f, &u, 4); return f; }

struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 1) {}
    u32 next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return u32(s >> 16) ^ u32(s >> 40); }
    u32 below(u32 n) { return next() % n; }
    float unit() { return float(next() & 0xFFFFFF) / 16777216.f; }
};

// Valores de estres para floats: ceros con signo, subnormales, minimos, maximos, infinitos, NaN con cargas distintas, senalizantes, +-1e30, angulos tipicos.
inline float StressFloat(Rng& r) {
    static const u32 pool[] = {
        0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x80800000, 0x00800001, 0x3F800000, 0xBF800000,
        0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FC00001, 0xFFC00001, 0x7FC12345, 0xFFC54321, 0x7F800001, 0xFF800001, 0x7FA00000, 0xFFBFFFFF,
        0x7149F2CA, 0xF149F2CA, 0x0DA24260, 0x8DA24260, 0x3F000000, 0xBF000000, 0x40490FDB, 0xC0490FDB, 0x3FC90FDB, 0xBFC90FDB, 0x40C90FDB, 0xC0C90FDB, 0x3E0EFA35, 0xBE0EFA35,
        0x3DCCCCCD, 0xBDCCCCCD, 0x3E4CCCCD, 0xBE4CCCCD, 0x3F333333, 0x41200000, 0x41A00000, 0xC1A00000, 0x42C80000, 0x4B000000, 0x4B7FFFFF, 0x7F000000, 0x00FFFFFF, 0x1E3CE508, 0x5F000000 };
    return F(pool[r.below(sizeof(pool) / sizeof(pool[0]))]);
}
// Rango "realista": angulos, distancias, posiciones.
inline float RealFloat(Rng& r, float scale) { return (r.unit() * 2.f - 1.f) * scale; }

// ---- comparacion de estado x87 ----
struct FpDiff {
    bool fcw = false, top = false, ftw = false, cc = false, exc = false, st = false, deadRaw = false, fdp = false;
    bool any() const { return fcw || top || ftw || cc || exc || st; }   // compuerta: todo salvo registros vacios en crudo y FIP/FCS/FDP/FDS/FOP
};
inline bool FpRegDead(const Fp& f, int st) {
    int phys = (FpTop(f) + st) & 7; return ((FpTw(f) >> (2 * phys)) & 3) == 3;
}
inline FpDiff CompareFp(const Fp& a, const Fp& b) {
    FpDiff d;
    d.fcw = FpCw(a) != FpCw(b);
    d.top = FpTop(a) != FpTop(b);
    d.ftw = FpTw(a) != FpTw(b);
    d.cc = (FpSw(a) & 0x4700) != (FpSw(b) & 0x4700);
    d.exc = (FpSw(a) & 0x80FF) != (FpSw(b) & 0x80FF);
    for (int s = 0; s < 8; ++s) {
        bool da = FpRegDead(a, s), db = FpRegDead(b, s);
        bool same = !memcmp(&a.b[28 + 10 * s], &b.b[28 + 10 * s], 10);
        if (!da && !db) { if (!same) d.st = true; }
        else if (da != db) d.st = true;
        else if (!same) d.deadRaw = true;
    }
    d.fdp = memcmp(&a.b[20], &b.b[20], 8) != 0;
    return d;
}

// Adversarial: NaN (todas las cargas/senalizantes) y subnormales mezclados con normales, para coincidencias NaN x subnormal (banderas DE/IE del x87).
inline float AdvFloat(Rng& r) {
    switch (r.below(10)) {
    case 0: case 1: return F(0x7FC00000u | (r.next() & 0x3FFFFF) | (r.below(2) ? 0u : 0x80000000u));
    case 2: return F(0x7F800000u | ((r.next() & 0x3FFFFF) | 1u) | (r.below(2) ? 0u : 0x80000000u));   // senalizante
    case 3: case 4: case 5: return F((r.next() & 0x7FFFFF) | (r.below(2) ? 0u : 0x80000000u));        // subnormal o cero
    case 6: return F(r.below(2) ? 0x7F800000u : 0xFF800000u);
    default: return RealFloat(r, 2.f);
    }
}
