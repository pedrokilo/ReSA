#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
#include <random>
#include "exebytes.inc"
void __cdecl QuaternionSlerpCpp(float* dst, const float* from, const float* to, float theta, float invSin, float t);

alignas(16) static uint8_t g_fp[108]; static uint32_t g_entry;
static const double kStale[8] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
struct Res { uint32_t q[4]; uint8_t fp[108]; };

static void PrepFpu() {}
static void __declspec(noinline) RunExe(float* dst, const float* from, const float* to, float th, float inv, float t, Res& r) {
    uint16_t cw = 0x007F; const double* staleP = kStale; uint8_t* fp = g_fp; const uint32_t entry = g_entry;
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
        push t
        push inv
        push th
        push to
        push from
        mov ecx, dst
        call entry
        mov eax, fp
        fnsave [eax]
    }
    memcpy(r.q, dst, 16); memcpy(r.fp, g_fp, 108);
}
static void __declspec(noinline) RunCpp(float* dst, const float* from, const float* to, float th, float inv, float t, Res& r) {
    uint16_t cw = 0x007F; const double* staleP = kStale; uint8_t* fp = g_fp; void* entry = (void*)&QuaternionSlerpCpp;
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
        push t
        push inv
        push th
        push to
        push from
        push dst
        call entry
        add esp, 24
        mov eax, fp
        fnsave [eax]
    }
    memcpy(r.q, dst, 16); memcpy(r.fp, g_fp, 108);
}
static float F(uint32_t b) { float f; memcpy(&f, &b, 4); return f; }
int main(int argc, char** argv) {
    // codigo del EXE reubicado: las cuatro constantes absolutas se redirigen a copias propias
    auto* code = (uint8_t*)VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy(code, kSlerp, sizeof(kSlerp));
    static uint32_t consts[4] = {0, 0x3FC90FDB, 0x40490FDB, 0x3F800000};
    const uint32_t olds[4] = {0x858b50, 0x858fe4, 0x858cb8, 0x858624};
    int patched = 0;
    for (size_t i = 0; i + 4 <= sizeof(kSlerp); ++i) for (int k = 0; k < 4; ++k) if (*(uint32_t*)(code + i) == olds[k]) { *(uint32_t*)(code + i) = (uint32_t)&consts[k]; ++patched; }
    printf("constantes reubicadas: %d\n", patched);
    g_entry = (uint32_t)code;
    std::vector<uint32_t> sp = {0, 0x80000000, 1, 0x80000001, 0x007FFFFF, 0x00800000, 0x80800000, 0x3F800000, 0xBF800000, 0x3F000000, 0x3FC90FDA, 0x3FC90FDB, 0x3FC90FDC, 0x40490FDA, 0x40490FDB, 0x40490FDC, 0x40C90FDB,
        0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7F800001, 0xFF800123, 0x7F7FFFFF, 0xFF7FFFFF, 0x7149F2CA, 0xF149F2CA, 0x0DA24260, 0x4F000000, 0x40000000, 0x40400000, 0x42C80000, 0x3E800000, 0x3F7FFFFF, 0x3F800001, 0x3A83126F, 0x358637BD, 0x7E967699, 0x1E3CE508, 0x7FC00001, 0xFFC00001, 0x7FC00002, 0xFFC00002, 0x7FA00000, 0xFFA00000, 0x7F800002, 0xFF800002, 0x7FFFFFFF, 0xFFFFFFFF, 0x00000002, 0x80000002, 0x00400000, 0x80400000, 0x00123456, 0x3F800001, 0xBF7FFFFF};
    std::mt19937 rng(12345);
    auto rnd = [&](int mode) -> uint32_t {
        uint32_t r = rng();
        switch (mode) { case 0: return sp[r % sp.size()]; case 1: { float f = (float)((int)(r % 2000001) - 1000000) / 1000000.0f; return *(uint32_t*)&f; } case 2: return rng(); default: { float f = (float)((r & 0xFFFF)) / 65536.0f * 4.0f; return *(uint32_t*)&f; } }
    };
    long badCodes = 0; long total = 0, bad = 0, badOut = 0, badFp = 0, badDead = 0; long shown = 0;
    long N = argc > 1 ? atol(argv[1]) : 2000000;
    alignas(16) float from[4], to[4], o1[4], o2[4];
    for (long i = 0; i < N; ++i) {
        int mt = i % 4, mq = (i / 4) % 4;
        float th = F(rnd(mt)), inv = F(rnd((i / 16) % 4)), t = F(rnd((i / 64) % 3 == 0 ? 0 : 3));
        if (i % 7 == 0) th = F(sp[(i / 7) % sp.size()]);
        for (int k = 0; k < 4; ++k) { from[k] = F(rnd(mq)); to[k] = F(rnd((mq + k) % 4)); }
        int alias = i % 11 == 0 ? 1 : i % 13 == 0 ? 2 : 0;
        float pre[4]; memset(o1, 0xA5, 16); memset(o2, 0xA5, 16);
        Res a, b;
        if (alias == 0) { RunExe(o1, from, to, th, inv, t, a); RunCpp(o2, from, to, th, inv, t, b); }
        else {
            float f1[4], t1[4], f2[4], t2[4]; memcpy(f1, from, 16); memcpy(t1, to, 16); memcpy(f2, from, 16); memcpy(t2, to, 16);
            if (alias == 1) { RunExe(f1, f1, t1, th, inv, t, a); RunCpp(f2, f2, t2, th, inv, t, b); }
            else { RunExe(t1, f1, t1, th, inv, t, a); RunCpp(t2, f2, t2, th, inv, t, b); }
        }
        ++total;
        bool oOK = memcmp(a.q, b.q, 16) == 0;
        uint8_t fa[108], fb[108]; memcpy(fa, a.fp, 108); memcpy(fb, b.fp, 108);
        memset(fa + 12, 0, 4); memset(fb + 12, 0, 4);   // FIP
        bool ctl = memcmp(fa, fb, 28) == 0;  // fcw,fsw,ftw,fcs,fdp,fds
        { uint8_t ga[28], gb[28]; memcpy(ga, fa, 28); memcpy(gb, fb, 28); ga[4] &= 0xC0; gb[4] &= 0xC0; if (memcmp(ga, gb, 28) != 0) ++badCodes; }
        bool regs = memcmp(fa + 28, fb + 28, 80) == 0;
        if (!oOK) ++badOut; if (!ctl) ++badFp; if (!regs) ++badDead;
        if (!(oOK && ctl && regs)) { ++bad; if ((argc > 2 ? !ctl : true) && shown++ < 12) { printf("DIF i=%ld th=%08x inv=%08x t=%08x out=%d ctl=%d regs=%d alias=%d\n", i, *(uint32_t*)&th, *(uint32_t*)&inv, *(uint32_t*)&t, oOK, ctl, regs, alias);
            if (!ctl) { printf("  fsw %04x vs %04x ftw %04x vs %04x fcw %04x %04x fdp %08x %08x fcs %08x %08x\n", *(uint16_t*)(fa+4), *(uint16_t*)(fb+4), *(uint16_t*)(fa+8), *(uint16_t*)(fb+8), *(uint16_t*)fa, *(uint16_t*)fb, *(uint32_t*)(fa+20), *(uint32_t*)(fb+20), *(uint32_t*)(fa+16), *(uint32_t*)(fb+16)); }
            if (!regs) for (int r = 0; r < 8; ++r) if (memcmp(fa+28+10*r, fb+28+10*r, 10)) { printf("  st%d exe=", r); for (int k=9;k>=0;--k) printf("%02x", fa[28+10*r+k]); printf(" cpp="); for (int k=9;k>=0;--k) printf("%02x", fb[28+10*r+k]); printf("\n"); }
            if (!oOK) printf("  out exe=%08x %08x %08x %08x cpp=%08x %08x %08x %08x\n", a.q[0], a.q[1], a.q[2], a.q[3], b.q[0], b.q[1], b.q[2], b.q[3]); } }
    }
    printf("sin_banderas_sticky: dif=%ld\n", badCodes); printf("total=%ld bad=%ld salida=%ld ctl=%ld regs_muertos=%ld\n", total, bad, badOut, badFp, badDead);
    return 0;
}
