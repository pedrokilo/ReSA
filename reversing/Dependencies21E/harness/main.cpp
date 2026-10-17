#include "core.hpp"
#include <math.h>
#include "../Fixtures.hpp"

int __cdecl PopulationRandomRangeCpp(int random, int range);
void __cdecl CullZoneCenterCpp(int zSum, int y5, int y3, int y1, int x4, int x2, int x0, float* out);
void RunEnterCar(unsigned cases, unsigned seed, int verbosity, bool bank);

static u32 g_randval;
static int __cdecl FakeRandExe() { return int(g_randval); }
static unsigned short GetRandomNumberFake() { return (unsigned short)g_randval; }

// --- candidato 0x610DB0 (copia de la fuente propuesta en Population.cpp) ---
static int __cdecl Cand610DB0() {
    const int hi = *reinterpret_cast<volatile int*>(0x86C6C8);
    const int lo = *reinterpret_cast<volatile int*>(0x86C6C4);
    const unsigned random = GetRandomNumberFake() & 0xFFFFu;
    return int(unsigned(lo) + unsigned(PopulationRandomRangeCpp(int(random), int(unsigned(hi) - unsigned(lo)))));
}
// --- candidato 0x72D6D0 (copia de la fuente propuesta en CullZones.h) ---
static float* __fastcall Cand72D6D0(const short* zone, void* /*edx*/, float* out) {
    CullZoneCenterCpp(int(zone[6]) + int(zone[7]), zone[5], zone[3], zone[1], zone[4], zone[2], zone[0], out);
    return out;
}

struct Stats { unsigned cases = 0, bad = 0; unsigned diffCw = 0, diffTop = 0, diffTw = 0, diffCc = 0, diffExc = 0, diffSt = 0, deadRaw = 0, fdp = 0, retBad = 0, stackBad = 0, nvBad = 0; };

static void Accumulate(Stats& s, const Res& a, const Res& b, bool retEq, const char* tag, u32 caseNo, int verbosity) {
    FpDiff d = CompareFp(a.fp, b.fp);
    s.cases++;
    bool bad = false;
    if (!retEq) { s.retBad++; bad = true; }
    if (!a.stackOk || !b.stackOk) { s.stackBad++; bad = true; }
    if (!a.nvOk || !b.nvOk) { s.nvBad++; bad = true; }
    if (d.fcw) { s.diffCw++; bad = true; }
    if (d.top) { s.diffTop++; bad = true; }
    if (d.ftw) { s.diffTw++; bad = true; }
    if (d.cc) { s.diffCc++; bad = true; }
    if (d.exc) { s.diffExc++; bad = true; }
    if (d.st) { s.diffSt++; bad = true; }
    if (d.deadRaw) { s.deadRaw++; bad = true; }
    if (d.fdp) s.fdp++;
    if (bad) {
        s.bad++;
        if (verbosity && s.bad <= 8) {
            printf("  [%s #%u] ref fsw=%04x cw=%04x tw=%04x | cand fsw=%04x cw=%04x tw=%04x | ret ref=%08x cand=%08x\n", tag, caseNo, FpSw(a.fp), FpCw(a.fp), FpTw(a.fp),
                   FpSw(b.fp), FpCw(b.fp), FpTw(b.fp), a.eax, b.eax);
        }
    }
}
static void Print(const char* name, const Stats& s) {
    printf("%s: casos=%u malos=%u | ret=%u stack=%u nv=%u | fcw=%u top=%u ftw=%u cc=%u exc=%u st=%u | info: deadRaw=%u fdp/fip=%u\n", name, s.cases, s.bad, s.retBad, s.stackBad, s.nvBad,
           s.diffCw, s.diffTop, s.diffTw, s.diffCc, s.diffExc, s.diffSt, s.deadRaw, s.fdp);
}

#define kCwGame gCw
struct BaseCfg { u16 cw, sw; };
static BaseCfg kBases[] = { {0x007F, 0x0000}, {0x007F, 0x4700 | (3 << 11)}, {0x007F, 0x0100 | (5 << 11) | 0x0020}, {0x007F, 0x0400 | (7 << 11)} };

extern "C" __declspec(dllexport) int HarnessMain(int argc, char** argv) {
    unsigned cases = argc > 1 ? atoi(argv[1]) : 100000;
    unsigned seed = argc > 2 ? atoi(argv[2]) : 1;
    int verbosity = argc > 3 ? atoi(argv[3]) : 1;
    const char* only = argc > 4 ? argv[4] : "";
    if (argc > 5) { gCw = (u16)strtoul(argv[5], nullptr, 16); for (auto& b : kBases) b.cw = gCw; }
    MapExe();
    Jmp(0x821B1E, (void*)&FakeRandExe);

    if (!*only || strchr(only, '1')) {
        Stats st; Rng rng(seed * 11 + 1);
        static const int limits[] = { 1, 5, 0, 1, -1, 2, 10, 100, 32768, 65535, 65536, 0x7FFFFFFF, int(0x80000000), 0x40000000, -5, -100000, 0x12345678, 3 };
        for (unsigned i = 0; i < cases; ++i) {
            int lo = rng.below(2) ? 1 : limits[rng.below(sizeof limits / sizeof limits[0])];
            int hi = rng.below(2) ? 5 : limits[rng.below(sizeof limits / sizeof limits[0])];
            if (rng.below(4) == 0) { lo = int(rng.next()); hi = int(rng.next()); }
            static const u32 rands[] = { 0, 1, 0x7FFF, 0x8000, 0xFFFF, 0x4000, 0x7FFE, 0x10000, 0x12345678, 0xFFFFFFFF, 2, 3 };
            g_randval = rng.below(3) == 0 ? rands[rng.below(sizeof rands / sizeof rands[0])] : (rng.below(2) ? (rng.next() & 0x7FFF) : rng.next() & 0xFFFF);
            DWORD old; VirtualProtect((void*)0x86C6C4, 8, PAGE_READWRITE, &old);
            *(int*)0x86C6C4 = lo; *(int*)0x86C6C8 = hi;
            const BaseCfg& bc = kBases[rng.below(4)];
            Fp base = MakeBaseFp(bc.cw, bc.sw);
            Res a = Call(0x610DB0, 0xAAAA0001, 0xBBBB0002, 0xCCCC0003, nullptr, 0, false, base);
            Res b = Call((u32)&Cand610DB0, 0xAAAA0001, 0xBBBB0002, 0xCCCC0003, nullptr, 0, false, base);
            Accumulate(st, a, b, a.eax == b.eax, "610DB0", i, verbosity);
        }
        Print("0x610DB0 rango aleatorio", st);
    }

    if (!*only || strchr(only, '2')) {
        Stats st; Rng rng(seed * 13 + 7);
        static const short pool[] = { 0, 1, -1, 2, -2, 32767, -32768, 32766, -32767, 100, -100, 1000, -1000, 12345, -12345, 255, 256, -255, 16384, -16384 };
        for (unsigned i = 0; i < cases; ++i) {
            alignas(16) short zone[8]; alignas(16) float out[3] = { F(0x7FC0DEAD), F(0xFFC0BEEF), F(0x7F800001) };
            for (int k = 0; k < 8; ++k) zone[k] = rng.below(3) ? pool[rng.below(sizeof pool / sizeof pool[0])] : short(rng.next());
            const BaseCfg& bc = kBases[rng.below(4)];
            Fp base = MakeBaseFp(bc.cw, bc.sw);
            float outA[3] = { out[0], out[1], out[2] }, outB[3] = { out[0], out[1], out[2] };
            u32 args[1] = { (u32)outA };
            Res a = Call(0x72D6D0, (u32)zone, 0xBBBB0002, 0xCCCC0003, args, 1, true, base);
            args[0] = (u32)outB;
            Res b = Call((u32)&Cand72D6D0, (u32)zone, 0xBBBB0002, 0xCCCC0003, args, 1, true, base);
            bool eq = a.eax == (u32)outA && b.eax == (u32)outB && !memcmp(outA, outB, 12);
            Accumulate(st, a, b, eq, "72D6D0", i, verbosity);
        }
        Print("0x72D6D0 centro de zona", st);
    }

    if (strchr(only, 'B')) {
        using namespace Dependencies21EFixtures;
        {
            Stats st;
            for (unsigned i = 0; i < kCases1; ++i) {
                Fx1 f = MakeFx1(i);
                g_randval = f.random;
                DWORD old; VirtualProtect((void*)0x86C6C4, 8, PAGE_READWRITE, &old);
                *(int*)0x86C6C4 = f.lower; *(int*)0x86C6C8 = f.upper;
                Fp base = MakeBaseFp(kCwGame, kFpStatus[f.fp]);
                Res a = Call(0x610DB0, 0xAAAA0001, 0xBBBB0002, 0xCCCC0003, nullptr, 0, false, base);
                Res b = Call((u32)&Cand610DB0, 0xAAAA0001, 0xBBBB0002, 0xCCCC0003, nullptr, 0, false, base);
                Accumulate(st, a, b, a.eax == b.eax, "610DB0", i, verbosity);
            }
            *(int*)0x86C6C4 = 1; *(int*)0x86C6C8 = 5;
            Print("BANCO 0x610DB0", st);
        }
        {
            Stats st;
            for (unsigned i = 0; i < kCases2; ++i) {
                Fx2 f = MakeFx2(i);
                alignas(16) short zone[8]; for (int k = 0; k < 8; ++k) zone[k] = f.zone[k];
                float outA[3] = { F(0x7FC0DEAD), F(0xFFC0BEEF), F(0x7F800001) }, outB[3] = { outA[0], outA[1], outA[2] };
                Fp base = MakeBaseFp(kCwGame, kFpStatus[f.fp]);
                u32 args[1] = { (u32)outA };
                Res a = Call(0x72D6D0, (u32)zone, 0xBBBB0002, 0xCCCC0003, args, 1, true, base);
                args[0] = (u32)outB;
                Res b = Call((u32)&Cand72D6D0, (u32)zone, 0xBBBB0002, 0xCCCC0003, args, 1, true, base);
                bool eq = a.eax == (u32)outA && b.eax == (u32)outB && !memcmp(outA, outB, 12);
                Accumulate(st, a, b, eq, "72D6D0", i, verbosity);
            }
            Print("BANCO 0x72D6D0", st);
        }
    }
    if (!*only || strchr(only, '3')) RunEnterCar(cases, seed, verbosity, false);
    if (strchr(only, 'B')) { RunEnterCar(0, seed, verbosity, true); }
    printf("deadRaw por ST(i):"); for (int i = 0; i < 8; ++i) printf(" %u", gDeadSlot[i]); printf("\n");
    return 0;
}
