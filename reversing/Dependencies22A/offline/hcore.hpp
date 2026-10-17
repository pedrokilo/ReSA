#pragma once
// Nucleo del arnes sin juego: mapea la imagen del EXE original en sus direcciones, compuertas cdecl con captura de registros/x87 y generador de fixtures.
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <Base.h>

typedef uint32_t u32;
typedef uint64_t u64;

template<class T> inline T* At(uintptr_t a) { return reinterpret_cast<T*>(a + kShift); }
#include "reloc.inc"
inline void MapImage() {
    void* p = VirtualAlloc((void*)(0x400000 + kShift), 0x8b1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (p != (void*)(0x400000 + kShift)) { printf("VirtualAlloc fallo %p err=%lu\n", p, GetLastError()); exit(2); }
    FILE* f = fopen("exe_image.bin", "rb");
    if (!f) { printf("sin exe_image.bin\n"); exit(2); }
    fread(p, 1, 0x8b1000, f);
    fclose(f);
    for (unsigned a : kRelocs) *At<u32>(a) += u32(kShift);
}

struct Out {
    u32 eax, ecx, edx;
    bool stack, nonvolatile;
    alignas(16) unsigned char fp[108];
    u32 mxcsr;
};

inline void PrimeX87() {
    static const double stale[8] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    const double* p = stale;
    unsigned short cw = 0x007F;
    __asm { fninit }
    __asm { fldcw cw }
    __asm { mov eax,p } __asm { fld qword ptr [eax] } __asm { fld qword ptr [eax+8] } __asm { fld qword ptr [eax+16] } __asm { fld qword ptr [eax+24] }
    __asm { fld qword ptr [eax+32] } __asm { fld qword ptr [eax+40] } __asm { fld qword ptr [eax+48] } __asm { fld qword ptr [eax+56] }
    __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) }
}

// cdecl con n argumentos de 32 bits; ECX/EDX/EAX con semillas; captura registros, ESP, no volatiles y FNSAVE
inline void Gate(uintptr_t fn, const u32* args, int n, Out& o) {
    u32 before{}, after{}, ra{}, rc{}, rd{}, b0{}, s0{}, i0{}, p0{}, b1{}, s1{}, i1{}, p1{};
    unsigned char* fp = o.fp;
    __asm {
        mov before, esp
        mov b0, ebx
        mov s0, esi
        mov i0, edi
        mov p0, ebp
        mov ecx, n
        mov eax, args
    pushloop:
        test ecx, ecx
        jz pushed
        push dword ptr [eax+ecx*4-4]
        dec ecx
        jmp pushloop
    pushed:
        mov eax, 0A5123456h
        mov ecx, 0B1234567h
        mov edx, 0C0DE5921h
        call fn
        mov ra, eax
        mov rc, ecx
        mov rd, edx
        mov eax, fp
        fnsave [eax]
        mov eax, n
        lea esp, [esp+eax*4]
        mov after, esp
        mov b1, ebx
        mov s1, esi
        mov i1, edi
        mov p1, ebp
    }
    o.eax = ra; o.ecx = rc; o.edx = rd;
    o.stack = before == after;
    o.nonvolatile = b0 == b1 && s0 == s1 && i0 == i1 && p0 == p1;
    o.mxcsr = 0;
}

inline u32 W32(const unsigned char* p) { u32 v; memcpy(&v, p, 4); return v; }
inline u32 W16(const unsigned char* p) { unsigned short v; memcpy(&v, p, 2); return v; }

inline bool RegDead(const unsigned char* fp, unsigned st) {
    const u32 ftw = W16(fp + 8), top = (W16(fp + 4) >> 11) & 7;
    return ((ftw >> (2 * ((top + st) & 7))) & 3) == 3;
}
// Devuelve una cadena con los campos distintos entre dos FNSAVE (FIP aparte; FDP/FDS/FCS/FOP incluidos)
inline std::string FpDiff(const unsigned char* a, const unsigned char* b, bool compareDead, bool ignoreFip) {
    std::string out;
    struct F { const char* n; int at, sz; } f[] = {{"fcw", 0, 2}, {"fsw", 4, 2}, {"ftw", 8, 2}, {"fip", 12, 4}, {"fcs_fop", 16, 4}, {"fdp", 20, 4}, {"fds", 24, 4}};
    for (auto& x : f) {
        if (ignoreFip && !strcmp(x.n, "fip")) continue;
        if (memcmp(a + x.at, b + x.at, x.sz)) { out += x.n; out += ' '; }
    }
    for (unsigned st = 0; st < 8; ++st) {
        const bool dead = RegDead(a, st);
        if (dead && !compareDead) continue;
        if (memcmp(a + 28 + 10 * st, b + 28 + 10 * st, 10)) { out += "st" + std::to_string(st) + (dead ? "(dead) " : "(live) "); }
    }
    return out;
}

// ------------- generador -------------
struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567ull) {}
    u32 next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return u32(s >> 16); }
    u32 below(u32 n) { return next() % n; }
    float uni(float lo, float hi) { return lo + (hi - lo) * (float(next() & 0xFFFFFF) / 16777216.0f); }
};
inline float BitsToFloat(u32 b) { float f; memcpy(&f, &b, 4); return f; }
inline u32 FloatToBits(float f) { u32 b; memcpy(&b, &f, 4); return b; }
inline const u32* StressTable(int& n) {
    static const u32 t[] = {
        0x00000000, 0x80000000, 0x00000001, 0x807FFFFF, 0x00800000, 0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000,
        0x7FC00001, 0x7F800001, 0xFF800001, 0x7FC12345, 0xFFC12345, 0x7FC54321, 0xFFC54321, 0x7149F2CA, 0xF149F2CA, 0x0DA24260, 0x8DA24260,
        0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x7FFFFFFF, 0xFFFFFFFF, 0x00400000, 0x80400000, 0x7F7FFFFE, 0x5F000000, 0xDF000000};
    n = int(sizeof(t) / sizeof(t[0]));
    return t;
}
// float "normal" en [lo, hi] o, con probabilidad pStress/1000, un valor de estres
inline u32 StressScale() { static const u32 v = getenv("STRESS") ? u32(atoi(getenv("STRESS"))) : 1; return v; }
inline float RndF(Rng& r, float lo, float hi, u32 pStress = 40) {
    if (r.below(1000) < pStress * StressScale()) { int n; const u32* t = StressTable(n); return BitsToFloat(t[r.below(n)]); }
    return r.uni(lo, hi);
}

// ---- analisis de registros x87 por indice FISICO ----
inline unsigned char gPrimePhys[8][10];
inline void CapturePrime() {
    PrimeX87();
    alignas(16) unsigned char fp[108];
    __asm { lea eax, fp }
    __asm { fnsave [eax] }
    const unsigned top = (W16(fp + 4) >> 11) & 7;
    for (unsigned r = 0; r < 8; ++r) memcpy(gPrimePhys[r], fp + 28 + 10 * ((r - top) & 7), 10);
}
inline unsigned TopOf(const unsigned char* fp) { return (W16(fp + 4) >> 11) & 7; }
// 'p' = igual que el residuo del prime, 'L' = vivo, 'X' = modificado
inline std::string PhysSig(const unsigned char* fp) {
    const unsigned top = TopOf(fp); std::string s;
    for (unsigned r = 0; r < 8; ++r) {
        const unsigned st = (r - top) & 7;
        if (!RegDead(fp, st)) { s += 'L'; continue; }
        s += memcmp(fp + 28 + 10 * st, gPrimePhys[r], 10) ? 'X' : 'p';
    }
    return s;
}
inline double Ext2D(const unsigned char* p) {
    unsigned long long m; memcpy(&m, p, 8); unsigned e = W16(p + 8) & 0x7FFF; bool neg = W16(p + 8) & 0x8000;
    if (e == 0 && m == 0) return neg ? -0.0 : 0.0;
    double v = ldexp(double(m) / 9223372036854775808.0, int(e) - 16383 + 1);
    return neg ? -v : v;
}
