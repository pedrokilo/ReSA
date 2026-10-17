#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <random>
#include "exebytes.inc"
#include "../Fixtures.hpp"
void __cdecl CamWrapAngleCpp(float* angle);
alignas(16) static uint8_t g_fp[108];
static const double kStale[8] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
struct Res { uint32_t v; uint8_t fp[108]; uint32_t edx; };
static void __declspec(noinline) Run(uintptr_t entry, float* p, Res& r) {
    uint16_t cw = 0x007F; const double* staleP = kStale; uint8_t* fp = g_fp; uint32_t edxOut = 0;
    __asm {
        fninit
        fldcw cw
        mov eax, staleP
        fld qword ptr [eax+0]
        fld qword ptr [eax+8]
        fld qword ptr [eax+16]
        fld qword ptr [eax+24]
        fld qword ptr [eax+32]
        fld qword ptr [eax+40]
        fld qword ptr [eax+48]
        fld qword ptr [eax+56]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        push p
        mov edx, 0x5A5A1234
        call entry
        mov edxOut, edx
        add esp, 4
        mov eax, fp
        fnsave [eax]
    }
    memcpy(&r.v, p, 4); memcpy(r.fp, g_fp, 108); r.edx = edxOut;
}
static float F(uint32_t b) { float f; memcpy(&f, &b, 4); return f; }
int main(int argc, char** argv) {
    auto* code = (uint8_t*)VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy(code, kWrap, sizeof kWrap);
    static float consts[3]; memcpy(consts, kCons, 12);
    const uint32_t olds[3] = {0x858CB8, 0x858CBC, 0x858CC0};
    int patched = 0;
    for (size_t i = 0; i + 4 <= sizeof(kWrap); ++i) for (int k = 0; k < 3; ++k) if (*(uint32_t*)(code + i) == olds[k]) { *(uint32_t*)(code + i) = (uint32_t)&consts[k]; ++patched; }
    printf("constantes reubicadas: %d", patched); putchar(10);
    const uintptr_t ADDR_EXE = (uintptr_t)code;
    if (argc > 2 && argv[2][0] == 'f') {   // fixtures del banco: resultado y clase de la ultima x87 de los bytes del EXE y del candidato
        for (unsigned c = 0; c < Dependencies22CFix::kWrapCases; ++c) {
            const auto w = Dependencies22CFix::Wrap(c);
            float fa = F(w.angle), fb = fa; Res a, b; Run(ADDR_EXE, &fa, a); Run((uintptr_t)&CamWrapAngleCpp, &fb, b);
            uint32_t ip; memcpy(&ip, a.fp + 12, 4);
            printf("R %u %08x %08x %08x %u %u\n", c, w.angle, a.v, b.v, a.edx == 0x5A5A1234, 0);
        }
        return 0;
    }
    std::vector<uint32_t> sp = {0, 0x80000000, 1, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x40490FDA, 0x40490FDB, 0x40490FDC, 0xC0490FDA, 0xC0490FDB, 0xC0490FDC, 0x40C90FDB, 0xC0C90FDB, 0x40C90FDA, 0x40C90FDC,
        0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7F800001, 0xFF800123, 0x7FFFFFFF, 0xFFFFFFFF, 0x4B000000, 0x4B800000, 0x4B7FFFFF, 0x4A989680, 0xCA989680, 0x49742400, 0xC9742400, 0x4C000000, 0xCC000000, 0x4CFFFFFF, 0x412AAAAB, 0x4116CBE4, 0x41490FDB, 0xC1490FDB, 0x3FC90FDB};
    std::mt19937 rng(777);
    long N = argc > 1 ? atol(argv[1]) : 2000000, bad = 0, shown = 0, badV = 0, badFp = 0, badRegs = 0, badSticky = 0, badIp = 0;
    for (long i = 0; i < N; ++i) {
        uint32_t bits;
        int m = i % 6;
        if (m == 0) bits = sp[rng() % sp.size()];
        else if (m == 1) { float f = (float)((int)(rng() % 200001) - 100000) / 1000.0f; memcpy(&bits, &f, 4); }
        else if (m == 2) { float f = (float)((int)(rng() % 2001) - 1000) / 100.0f; memcpy(&bits, &f, 4); }
        else if (m == 3) { float f = (float)((int)(rng() % 20001) - 10000); memcpy(&bits, &f, 4); }
        else if (m == 4) { float f = (float)(rng() % 7) * 3.14159265f + (float)(rng() % 3 - 1) * 1e-6f; memcpy(&bits, &f, 4); if (rng() & 1) bits ^= 0x80000000; }
        else { bits = rng(); uint32_t e = (bits >> 23) & 0xFF; if (e > 0x95 && e != 0xFF) bits = (bits & 0x807FFFFF) | (0x8F << 23); }   // sin huecos que no terminan
        float fa = F(bits), fb = fa;
        { uint32_t e = (bits >> 23) & 0xFF; if (e != 0xFF && e > 0x98) continue; }   // |x| >= 2^26 no termina
        Res a, b; Run(ADDR_EXE, &fa, a); Run((uintptr_t)&CamWrapAngleCpp, &fb, b);
        uint8_t xa[108], xb[108]; memcpy(xa, a.fp, 108); memcpy(xb, b.fp, 108); memset(xa + 12, 0, 4); memset(xb + 12, 0, 4);
        bool vOK = a.v == b.v, ctl = memcmp(xa, xb, 28) == 0, regs = memcmp(xa + 28, xb + 28, 80) == 0;
        { uint8_t ga[28], gb[28]; memcpy(ga, xa, 28); memcpy(gb, xb, 28); ga[4] &= 0xC0; gb[4] &= 0xC0; if (memcmp(ga, gb, 28)) ++badSticky; }
        if (!vOK) ++badV; if (!ctl) ++badFp; if (!regs) ++badRegs;
        if (!(vOK && ctl && regs)) { ++bad; if (shown++ < 10) { printf("DIF in=%08x exe=%08x cpp=%08x ctl=%d regs=%d fsw %04x/%04x fcs_fop %08x/%08x fdp %08x/%08x\n", bits, a.v, b.v, ctl, regs, *(uint16_t*)(xa+4), *(uint16_t*)(xb+4), *(uint32_t*)(xa+16), *(uint32_t*)(xb+16), *(uint32_t*)(xa+20), *(uint32_t*)(xb+20));
            for (int r = 0; r < 8; ++r) if (memcmp(xa+28+10*r, xb+28+10*r, 10)) { printf("  st%d exe=", r); for (int k=9;k>=0;--k) printf("%02x", xa[28+10*r+k]); printf(" cpp="); for (int k=9;k>=0;--k) printf("%02x", xb[28+10*r+k]); printf("\n"); } } }
    }
    printf("N=%ld bad=%ld valor=%ld ctl=%ld regs=%ld (ctl sin condiciones C0-C3: dif=%ld)\n", N, bad, badV, badFp, badRegs, badSticky);
    return 0;
}
