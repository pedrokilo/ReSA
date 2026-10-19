// Arnes aislado (sin el juego) de FxEmitter_c::AddParticle(vector) (EXE 0x4A3EA0): los bytes reales del EXE reubicados (exebytes.inc) frente al codigo C++ de FxEmitter.cpp
// (add_body.inc, extraido por extract.py) con el ayudante FxAddParticleRotZByteCpp de X87Math.cpp compilado con las opciones de produccion. Los dos lados usan los MISMOS colaboradores
// sustituidos de Fixtures.hpp. El _ftol2 (0x821B40) del lado EXE son sus bytes reales reubicados; el del lado C++ es el del CRT.
// Uso: cphadd30.exe [casos] [mostrar] [modo]  (modo: o = volcar el lado EXE para cmp_o.py; d = volcar las fixtures; r = fixtures aleatorias adicionales)
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include "exebytes.inc"
#include "../Fixtures.hpp"
using namespace AddFix;

unsigned int __cdecl FxAddParticleRotZByteCpp(float rotZ);

typedef int32_t int32; typedef int8_t int8; typedef uint8_t uint8; typedef uint32_t uint32;
struct RwV3d { float x, y, z; };
struct CVector : RwV3d { CVector() = default; };
struct RwMatrix { RwV3d right; uint32 flags; RwV3d up; uint32 pad1; RwV3d at; uint32 pad2; RwV3d pos; uint32 pad3; };
static_assert(sizeof(RwMatrix) == 0x40);
struct RwRGBAReal { float red, green, blue, alpha; };
struct FxPrtMult_c { RwRGBAReal m_Color; float m_fSize, m_Rot, m_fLife; };
struct EmissionInfo_t { u32 w[26]; };
static_assert(sizeof(EmissionInfo_t) == 0x68);
struct FxEmitterPrt_c { uint8 pad[0x36]; int8 m_RotZ; uint8 pad2[5]; };
static_assert(sizeof(FxEmitterPrt_c) == 0x3C);
struct FxSystemBP_c { uint8 pad[0xC]; float m_fLength; };
struct FxSystem_c { uint8 pad0[8]; FxSystemBP_c* m_SystemBP; RwMatrix* m_ParentMatrix; uint8 pad1[0x52 - 0x10]; bool m_UseConstTime; };
static_assert(offsetof(FxSystem_c, m_SystemBP) == 8 && offsetof(FxSystem_c, m_ParentMatrix) == 0xC && offsetof(FxSystem_c, m_UseConstTime) == 0x52);
struct FxInfoManager_c { uint8 pad[1]; void ProcessEmissionInfo(float cur, float mult, float length, bool useConst, EmissionInfo_t* out) { StubProcessEmissionInfo(this, nullptr, cur, mult, length, useConst, out); } };
struct FxPrimBP_c { uint8 pad[0x2C]; FxInfoManager_c m_FxInfoManager; void GetRWMatrix(RwMatrix& m) { StubGetRWMatrix(this, nullptr, &m); } };
struct FxManager_c { RwMatrix* FxRwMatrixCreate() { return (RwMatrix*)StubCreate(nullptr, nullptr); } void FxRwMatrixDestroy(RwMatrix* m) { StubDestroy(nullptr, nullptr, m); } };
static FxManager_c g_fxMan;
static RwMatrix* RwMatrixUpdate(RwMatrix* m) { return (RwMatrix*)StubRwMatrixUpdate(m); }
static RwMatrix* RwMatrixMultiply(RwMatrix* d, const RwMatrix* a, const RwMatrix* b) { return (RwMatrix*)StubRwMatrixMultiply(d, a, b); }

struct FxEmitter_c {
    void* vptr; FxPrimBP_c* m_PrimBP; FxSystem_c* m_System; bool m_bEnabled; float m_fEmissionIntensity;
    FxEmitterPrt_c* CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal) {
        return (FxEmitterPrt_c*)StubCreateParticle(this, nullptr, &emissionInfo, &wldMat, velOverride, timeSince, &fxMults, brightness, createLocal);
    }
    void AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal);
};
static_assert(sizeof(FxEmitter_c) == 0x14);

#include "add_body.inc"

// ---- EXE reubicado ----
static uint8_t* gFtol; static uint8_t* gConst; static uint8_t* gTable;
static uint8_t* Alloc(size_t n) { return (uint8_t*)VirtualAlloc(nullptr, (n + 0xFFF) & ~size_t(0xFFF), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); }
static uint8_t* gExeCp;
static void* Sym(const char* n) {
    struct E { const char* n; void* p; };
    const E calls[] = {{"FTOL", gFtol}, {"PEI", (void*)&StubProcessEmissionInfo}, {"CREATE", (void*)&StubCreate}, {"DESTROY", (void*)&StubDestroy}, {"GETRW", (void*)&StubGetRWMatrix},
                       {"RWUPDATE", (void*)&StubRwMatrixUpdate}, {"RWMULTIPLY", (void*)&StubRwMatrixMultiply}, {"CREATEPARTICLE", (void*)&StubCreateParticle}};
    for (const auto& e : calls) if (!strcmp(e.n, n)) return e.p;
    fprintf(stderr, "simbolo sin resolver %s\n", n); exit(2);
}
static uint8_t* Build(const unsigned char* code, size_t n, const Patch* patches) {
    uint8_t* mem = Alloc(n);
    memcpy(mem, code, n);
    for (size_t i = 0; patches[i].kind >= 0; ++i) {
        const Patch& p = patches[i];
        if (p.kind == 0) {
            uint32_t d; memcpy(&d, mem + p.off, 4);
            const uint32_t v = uint32_t((uintptr_t)gConst) + (d - 0x858000u);
            memcpy(mem + p.off, &v, 4);
        } else {
            const int32_t rel = int32_t((uintptr_t)Sym(p.sym) - ((uintptr_t)mem + p.off + 4));
            memcpy(mem + p.off, &rel, 4);
        }
    }
    return mem;
}

alignas(16) static uint8_t gFp[108];
static const double kStale[8] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
struct Res { u32 eax, ecx, edx; bool esp, nv; uint8_t fp[108]; };
static void __declspec(noinline) RunLane(uintptr_t entry, void* self, const void* args /* 7 dwords */, Res& r) {
    uint16_t cw = 0x007F; const double* staleP = kStale; uint8_t* fp = gFp; const u32* a = (const u32*)args;
    u32 ra{}, rc{}, rd{}, spBefore{}, spAfter{}, b0{}, s0{}, i0{}, p0{}, b1{}, s1{}, i1{}, p1{};
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
        mov spBefore, esp
        mov b0, ebx
        mov s0, esi
        mov i0, edi
        mov p0, ebp
        mov ebx, 0x11111111
        mov esi, 0x22222222
        mov edi, 0x33333333
        mov eax, a
        push dword ptr [eax+24]
        push dword ptr [eax+20]
        push dword ptr [eax+16]
        push dword ptr [eax+12]
        push dword ptr [eax+8]
        push dword ptr [eax+4]
        push dword ptr [eax+0]
        mov ecx, self
        mov eax, 0xA5123456
        mov edx, 0xC0DE5921
        call entry
        mov ra, eax
        mov rc, ecx
        mov rd, edx
        mov spAfter, esp
        mov b1, ebx
        mov s1, esi
        mov i1, edi
        mov p1, ebp
        mov ebx, b0
        mov esi, s0
        mov edi, i0
        mov eax, fp
        fnsave [eax]
    }
    r.eax = ra; r.ecx = rc; r.edx = rd; r.esp = spBefore == spAfter; r.nv = (b1 == 0x11111111 && s1 == 0x22222222 && i1 == 0x33333333 && p1 == p0);
    memcpy(r.fp, gFp, 108);
}
static uintptr_t MemberAddress() { auto pm = &FxEmitter_c::AddParticle; uintptr_t a; memcpy(&a, &pm, sizeof a); return a; }

static void Normalize(uint8_t* fp) {
    uint16_t fsw; memcpy(&fsw, fp + 4, 2); fsw &= 0x3800; memcpy(fp + 4, &fsw, 2);
    memset(fp + 2, 0, 2); memset(fp + 6, 0, 2); memset(fp + 10, 0, 2); memset(fp + 12, 0, 16);
    uint16_t ftw; memcpy(&ftw, fp + 8, 2); const unsigned top = (fsw >> 11) & 7;
    for (unsigned st = 0; st < 8; ++st) if (((ftw >> (2 * ((top + st) & 7))) & 3) == 3) memset(fp + 28 + 10 * st, 0, 10);
}
static u32 gRs = 12345;
static u32 RndU() { gRs = gRs * 1664525u + 1013904223u; return gRs ^ (gRs >> 15); }

static void InitHarness() {
    gConst = Alloc(sizeof kConst); memcpy(gConst, kConst, sizeof kConst);
    gFtol = Alloc(sizeof kFTOL); memcpy(gFtol, kFTOL, sizeof kFTOL);
    gExeCp = Build(kADD, sizeof kADD, kADDPatches);
}
#ifndef CPH_NO_MAIN
int main(int argc, char** argv) {
    InitHarness();
    const unsigned N = argc > 1 ? atoi(argv[1]) : kCases;
    const int show = argc > 2 ? atoi(argv[2]) : 6;
    const char mode = argc > 3 ? argv[3][0] : 0;
    if (mode == 'd') {
        for (unsigned c = 0; c < kCases; ++c) {
            const AddFx f = Make(c);
            printf("A %u %u %u %u %u %u %u %u", c, f.family, f.useConst, f.createLocal, f.parent, f.particleMode, f.fill, 0);
            for (u32 v : f.pos) printf(" %08x", v); for (u32 v : f.vel) printf(" %08x", v); printf(" %08x %08x %08x", f.timeSince, f.rotZ, f.brightness); for (u32 v : f.mult) printf(" %08x", v);
            printf(" %08x", f.length); for (u32 v : f.emission) printf(" %08x", v); for (auto& m : f.matInit) for (u32 v : m) printf(" %08x", v); for (u32 v : f.parentMat) printf(" %08x", v); for (u32 v : f.getRw) printf(" %08x", v);
            printf("\n");
        }
        return 0;
    }
    unsigned bad = 0, badMem = 0, badTrace = 0, badAbi = 0, badFp = 0, shown = 0, withParticle = 0, rotWritten = 0;
    for (unsigned c = 0; c < N; ++c) {
        AddFx f = Make(c % kCases);
        if (mode == 'r') {
            for (u32& v : f.pos) if (RndU() % 3 == 0) v = RndU();
            for (u32& v : f.vel) if (RndU() % 3 == 0) v = RndU();
            if (RndU() % 2 == 0) f.rotZ = RndU(); if (RndU() % 3 == 0) f.timeSince = RndU(); if (RndU() % 3 == 0) f.brightness = RndU();
            for (u32& v : f.parentMat) if (RndU() % 3 == 0) v = RndU();
            for (u32& v : f.getRw) if (RndU() % 3 == 0) v = RndU();
        }
        Res res[2]; std::vector<u32> traces[2]; std::vector<uint8_t> mem[2];
        alignas(16) static uint8_t store[kArena + 2 * kGuard];
        for (int side = 0; side < 2; ++side) {
            memset(store, 0xA5, sizeof store);
            uint8_t* base = store + kGuard;
            BuildArena(base, f);
            trace.clear(); traceBase = base; matrixCount = 0; particleMode = f.particleMode;
            memcpy(emissionWords, f.emission, sizeof emissionWords); memcpy(getRwWords, f.getRw, sizeof getRwWords);
            u32 args[7] = {Ptr(base, kPos), Ptr(base, kVel), f.timeSince, Ptr(base, kMult), f.rotZ, f.brightness, f.createLocal};
            auto* e = (FxEmitter_c*)(base + kEmitter);
            if (side == 0) RunLane((uintptr_t)gExeCp, e, args, res[0]); else RunLane(MemberAddress(), e, args, res[1]);
            traces[side] = trace; mem[side].assign(store, store + sizeof store);
        }
        if (mode == 'o') {
            printf("O %u %08x ", c, (u32)(uintptr_t)(store + kGuard));
            for (size_t i = kGuard; i < kGuard + kArena; ++i) printf("%02x", mem[0][i]);
            printf(" T"); for (u32 v : traces[0]) printf(" %x", v); printf("\n");
        }
        Normalize(res[0].fp); Normalize(res[1].fp);
        const bool okMem = mem[0] == mem[1], okTrace = traces[0] == traces[1], okAbi = res[0].esp && res[1].esp && res[0].nv && res[1].nv, okFp = memcmp(res[0].fp, res[1].fp, 108) == 0;
        badMem += !okMem; badTrace += !okTrace; badAbi += !okAbi; badFp += !okFp; withParticle += f.particleMode; rotWritten += f.particleMode && mem[0][kGuard + kPart + 0x36] != 0xA5 && 0;
        if (!(okMem && okTrace && okAbi && okFp)) {
            ++bad;
            if (shown++ < (unsigned)show) {
                printf("DIF c=%u mem=%d trace=%d(%zu/%zu) abi=%d fp=%d rotZ=%08x parent=%u particle=%u\n", c, okMem, okTrace, traces[0].size(), traces[1].size(), okAbi, okFp, f.rotZ, f.parent, f.particleMode);
                if (!okMem) { int n = 0; for (size_t i = 0; i < mem[0].size() && n < 8; ++i) if (mem[0][i] != mem[1][i]) { printf("   off=%03zx exe=%02x cpp=%02x\n", i - kGuard, mem[0][i], mem[1][i]); ++n; } }
            }
        }
    }
    printf("casos=%u bad=%u mem=%u traza=%u abi=%u fp=%u | con particula=%u\n", N, bad, badMem, badTrace, badAbi, badFp, withParticle);
    return bad != 0;
}
#endif
