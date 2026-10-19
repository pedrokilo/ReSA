// Arnes aislado (sin el juego) de FxEmitter_c::CreateParticle (EXE 0x4A2580): los bytes reales del EXE reubicados (exebytes.inc) frente al codigo C++ de FxEmitter.cpp
// (cp_body.inc, extraido por extract.py) con los ayudantes de X87Math.cpp compilados con las opciones de produccion. Los dos lados usan los MISMOS colaboradores
// sustituidos de Fixtures.hpp y el mismo generador de rand() (el del CRT del EXE: seed * 0x343FD + 0x269EC3; (seed >> 16) & 0x7FFF).
// Uso: cph30.exe [casos] [mostrar] [modo]  (modo: o = volcar el lado EXE para cmp_o.py; d = volcar las fixtures; r = fixtures aleatorias adicionales)
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
using namespace CpFix;

// ---- ayudantes x87 (X87Math.cpp) ----
void __cdecl FxPrtLifeCpp(int rem, float lifeBias, float life, float multLife, float* out);
unsigned int __cdecl FxPrtColorByteCpp(float value);
unsigned int __cdecl FxPrtBrightnessByteCpp(float value);
unsigned int __cdecl FxPrtRandByteCpp(int rem);
void __cdecl FxPrtRotationCpp(int rem, float rotMin, float rotMax, float* out);
void __cdecl FxPrtBoxPositionCpp(int remX, int remY, int remZ, const float* sizeMin, const float* sizeMax, const float* off, const float* mat, float* out);
void __cdecl FxPrtSpherePositionCpp(int remX, int remY, int remZ, bool useExtra, int remExtra, float radius, const float* off, const float* mat, float* out);
void __cdecl FxPrtDirectionCpp(int remA, int remB, float angleMin, float angleMax, const float* table, float* out);
void __cdecl FxPrtSpeedCpp(int rem, float speedBias, float speed, const float* dir, float* vel);
void __cdecl FxPrtVelAddCpp(const float* velAdd, float* vel);

typedef int32_t int32; typedef int8_t int8; typedef uint8_t uint8; typedef uint32_t uint32;

// ---- tipos de apoyo (disposicion identica a la del proyecto) ----
struct RwV3d { float x, y, z; };
struct CVector : RwV3d { CVector() = default; };
static_assert(sizeof(CVector) == 12);
struct RwMatrix { RwV3d right; uint32 flags; RwV3d up; uint32 pad1; RwV3d at; uint32 pad2; RwV3d pos; uint32 pad3; };
static_assert(sizeof(RwMatrix) == 0x40);
struct RwRGBAReal { float red, green, blue, alpha; };
struct FxPrtMult_c { RwRGBAReal m_Color; float m_fSize, m_Rot, m_fLife; };
static_assert(sizeof(FxPrtMult_c) == 0x1C);
struct EmissionInfo_t { float m_fCount, m_fRadius; CVector m_SizeMin, m_SizeMax; float m_Speed, m_fSpeedBias; CVector m_Direction; float m_fAngleMin, m_fAngleMax, m_fLife, m_fLifeBias; CVector m_Pos;
    float m_fRotationMinAngle, m_fRotationMaxAngle, m_fMinWind, m_fMaxWind, m_fMinRain, m_fMaxRain; };
static_assert(sizeof(EmissionInfo_t) == 0x68);
struct FxSystem_c;
template<class T> struct ListItem_c { T* m_pPrev; T* m_pNext; };
struct Particle_c : ListItem_c<Particle_c> { float m_fTotalLife, m_fCurrentLife; CVector m_Pos, m_Velocity; FxSystem_c* m_System; };
static_assert(sizeof(Particle_c) == 0x2C);
struct FxEmitterPrt_c : Particle_c { uint8 mult[4]; uint8 size, rot, randR, randG, randB, brightness; int8 m_RotZ; bool local; float m_CurrentRotation; };
static_assert(sizeof(FxEmitterPrt_c) == 0x3C);
template<class T> struct TList_c {
    T* m_head; T* m_tail; uint32 m_cnt;
    void AddItem(T* item) {                    // copia literal de TList_c::AddItem (List_c.h) sin los assert
        const auto oldHead = m_head; m_head = item;
        item->m_pPrev = nullptr; item->m_pNext = oldHead;
        if (oldHead) oldHead->m_pPrev = item; else m_tail = item;
        ++m_cnt;
    }
};
struct FxSystem_c {   // solo lo que lee CreateParticle: banderas en +0x62 (bit 5 = m_MustCreateParticles) y m_VelAdd en +0x68
    uint8 pad0[0x62];
    uint8 m_allocatedParentMat : 1, m_createLocal : 1, m_useZTest : 1, m_stopParticleCreation : 1, m_prevCulled : 1, m_MustCreateParticles : 1;
    uint8 pad1; float m_LoopInterval; CVector m_VelAdd;
};
static_assert(offsetof(FxSystem_c, m_VelAdd) == 0x68 && sizeof(FxSystem_c) == 0x74);
struct FxEmitterBP_c;
struct FxPrimBP_c { uint8 pad[0x20]; TList_c<Particle_c> m_Particles; uint8 pad2[0x40 - 0x2C]; void GetRWMatrix(RwMatrix& m) { StubGetRWMatrix(this, nullptr, &m); } };
static_assert(sizeof(FxPrimBP_c) == 0x40);
struct FxEmitterBP_c : FxPrimBP_c { bool UpdateParticle(float t, FxEmitterPrt_c* p) { return StubUpdateParticle(this, nullptr, t, p); } };
struct FxManager_c { Particle_c* GetParticle(int8 t) { return (Particle_c*)StubGetParticle(nullptr, nullptr, t); } void FreeUpParticle() { StubFreeUpParticle(nullptr, nullptr); } };
static FxManager_c g_fxMan;
static float RwV3dNormalize(RwV3d* out, const RwV3d* in) { return StubNormalize(out, in); }
static RwV3d* RwV3dTransformVectors(RwV3d* out, const RwV3d* in, int n, const RwMatrix* m) { return (RwV3d*)StubTransform(out, in, n, m); }
static void RotateVecIntoVec(RwV3d& res, const RwV3d& vec, const RwV3d& align) { StubRotate(&res, &vec, &align); }

static uint32 gRandSeed;
static int __cdecl HarnessExeRand() { gRandSeed = gRandSeed * 0x343FDu + 0x269EC3u; return int((gRandSeed >> 16) & 0x7FFFu); }
static float gSinTable[256];

struct FxEmitter_c {
    void* vptr; FxPrimBP_c* m_PrimBP; FxSystem_c* m_System; bool m_bEnabled; float m_fEmissionIntensity;
    FxEmitterPrt_c* CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal);
};
static_assert(sizeof(FxEmitter_c) == 0x14);

#include "cp_rand.inc"
#include "cp_body.inc"

// ---- EXE reubicado ----
static uint8_t* gCode[8]; static uint8_t* gFtol; static uint8_t* gItem; static uint8_t* gConst;
static uint8_t* Alloc(size_t n) { return (uint8_t*)VirtualAlloc(nullptr, (n + 0xFFF) & ~size_t(0xFFF), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); }
static void* Sym(const char* n, uint8_t* self) {
    struct E { const char* n; void* p; };
    const E calls[] = {{"GETPARTICLE", (void*)&StubGetParticle}, {"FREEUP", (void*)&StubFreeUpParticle}, {"RAND", (void*)&HarnessExeRand}, {"FTOL", gFtol}, {"GETRW", (void*)&StubGetRWMatrix},
                       {"NORMALIZE", (void*)&StubNormalize}, {"TRANSFORM", (void*)&StubTransform}, {"ROTATE", (void*)&StubRotate}, {"UPDATEPARTICLE", (void*)&StubUpdateParticle}, {"ADDITEM", gItem}};
    for (const auto& e : calls) if (!strcmp(e.n, n)) return e.p;
    fprintf(stderr, "simbolo sin resolver %s\n", n); exit(2);
}
static uint8_t* Build(const unsigned char* code, size_t n, const Patch* patches, size_t np) {
    uint8_t* mem = Alloc(n);
    memcpy(mem, code, n);
    for (size_t i = 0; patches[i].kind >= 0; ++i) {
        const Patch& p = patches[i];
        if (p.kind == 0) {
            uint32_t d; memcpy(&d, mem + p.off, 4);
            const uint32_t v = !strcmp(p.sym, "TABLE") ? uint32_t((uintptr_t)gSinTable) + (d - 0xBB3E00u) : uint32_t((uintptr_t)gConst) + (d - 0x858000u);
            memcpy(mem + p.off, &v, 4);
        } else {
            const int32_t rel = int32_t((uintptr_t)Sym(p.sym, mem) - ((uintptr_t)mem + p.off + 4));
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
static uintptr_t MemberAddress() { auto pm = &FxEmitter_c::CreateParticle; uintptr_t a; memcpy(&a, &pm, sizeof a); return a; }

// Estado x87 normalizado como en los bancos del lote 28: FCW, TOP, etiquetas, ST validos (ignora banderas de FSW, punteros FIP/FDP y registros vacios).
static void Normalize(uint8_t* fp) {
    uint16_t fsw; memcpy(&fsw, fp + 4, 2); fsw &= 0x3800; memcpy(fp + 4, &fsw, 2);
    memset(fp + 2, 0, 2); memset(fp + 6, 0, 2); memset(fp + 10, 0, 2); memset(fp + 12, 0, 16);
    uint16_t ftw; memcpy(&ftw, fp + 8, 2); const unsigned top = (fsw >> 11) & 7;
    for (unsigned st = 0; st < 8; ++st) if (((ftw >> (2 * ((top + st) & 7))) & 3) == 3) memset(fp + 28 + 10 * st, 0, 10);
}

static u32 gRs = 12345;
static u32 RndU() { gRs = gRs * 1664525u + 1013904223u; return gRs ^ (gRs >> 15); }

static uint8_t* gExeCp;
static void InitHarness() {
    gConst = Alloc(sizeof kConst); memcpy(gConst, kConst, sizeof kConst);
    for (unsigned i = 0; i < 256; ++i) { u32 v = Mix(i * 7919u + 3u); gSinTable[i] = F(((v >> 3) & 0x007FFFFF) | ((v & 1) ? 0x3F000000u : 0xBF000000u)); }   // valores distintos, finitos
    gFtol = Alloc(sizeof kFTOL); memcpy(gFtol, kFTOL, sizeof kFTOL);
    gItem = Alloc(sizeof kITEM); memcpy(gItem, kITEM, sizeof kITEM);
    // el _ftol2 del EXE y AddItem no tienen simbolos propios que resolver (kFTOLPatches / kITEMPatches estan vacios)
    gExeCp = Build(kCP, sizeof kCP, kCPPatches, 0);
}
#ifndef CPH_NO_MAIN
int main(int argc, char** argv) {
    InitHarness();
    uint8_t* exeCp = gExeCp;
    const unsigned N = argc > 1 ? atoi(argv[1]) : kCases;
    const int show = argc > 2 ? atoi(argv[2]) : 6;
    const char mode = argc > 3 ? argv[3][0] : 0;
    if (mode == 'd') {
        for (unsigned c = 0; c < kCases; ++c) {
            const CpFx f = Make(c);
            printf("C %u %u %u %u %u %u %u %u %u %u %u %08x %u", c, f.family, f.hasVel, f.createLocal, f.mustCreate, f.poolMode, f.listMode, f.flagsOther, f.radiusKind, f.dirKind, f.fill, f.seed, 0);
            for (u32 v : f.emission) printf(" %08x", v); for (u32 v : f.mult) printf(" %08x", v); for (u32 v : f.wld) printf(" %08x", v); for (u32 v : f.getRw) printf(" %08x", v);
            for (u32 v : f.velOverride) printf(" %08x", v); for (u32 v : f.velAdd) printf(" %08x", v); printf(" %08x %08x\n", f.timeSince, f.brightness);
        }
        return 0;
    }
    unsigned bad = 0, badMem = 0, badTrace = 0, badRet = 0, badAbi = 0, badFp = 0, badRand = 0, shown = 0;
    unsigned cover[8] = {};
    for (unsigned c = 0; c < N; ++c) {
        CpFx f = Make(c % kCases);
        if (mode == 'r') {   // fixtures aleatorias adicionales: valores crudos de 32 bits
            f = Make(c % kCases);
            for (u32& v : f.emission) if (RndU() % 3 == 0) v = RndU();
            for (u32& v : f.mult) if (RndU() % 3 == 0) v = RndU();
            for (u32& v : f.wld) if (RndU() % 3 == 0) v = RndU();
            for (u32& v : f.getRw) if (RndU() % 3 == 0) v = RndU();
            for (u32& v : f.velAdd) if (RndU() % 3 == 0) v = RndU();
            if (RndU() % 3 == 0) f.timeSince = RndU(); if (RndU() % 3 == 0) f.brightness = RndU(); f.seed = RndU();
            if (!getenv("CPH_SNAN")) {   // las senalizantes (NaN con el bit 22 a 0) dependen de la forma de la instruccion x87 (memoria o registro): fuera del dominio probado
                auto quiet = [](u32& v) { if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x007FFFFFu) && !(v & 0x00400000u)) v |= 0x00400000u; };
                for (u32& v : f.emission) quiet(v); for (u32& v : f.mult) quiet(v); for (u32& v : f.wld) quiet(v); for (u32& v : f.getRw) quiet(v); for (u32& v : f.velAdd) quiet(v);
                for (u32& v : f.velOverride) quiet(v); quiet(f.timeSince); quiet(f.brightness);
            }
        }
        Res res[2]; std::vector<u32> traces[2]; std::vector<uint8_t> mem[2]; u32 nextRand[2];
        alignas(16) static uint8_t store[1][kArena + 2 * kGuard];
        for (int side = 0; side < 2; ++side) {
            memset(store[0], 0xA5, sizeof store[0]);
            uint8_t* base = store[0] + kGuard;
            BuildArena(base, f);
            trace.clear(); traceBase = base; poolCalls = 0; poolMode = f.poolMode; memcpy(getRwWords, f.getRw, sizeof getRwWords);
            gRandSeed = f.seed;
            u32 args[7] = {Ptr(base, kEmission), Ptr(base, kWld), f.hasVel ? Ptr(base, kVel) : 0u, f.timeSince, Ptr(base, kMult), f.brightness, f.createLocal};
            auto* e = (FxEmitter_c*)(base + kEmitter);
            if (side == 0) RunLane((uintptr_t)exeCp, e, args, res[0]); else RunLane(MemberAddress(), e, args, res[1]);
            nextRand[side] = (u32)HarnessExeRand();
            traces[side] = trace; mem[side].assign(store[0], store[0] + sizeof store[0]);
            // el puntero devuelto se anota como desplazamiento (el arena de cada lado esta en otra direccion)
            res[side].eax = res[side].eax ? res[side].eax - (u32)(uintptr_t)base : 0xFFFFFFFFu;
        }
        if (mode == 'o') {
            printf("O %u %08x %08x %08x ", c, (u32)(uintptr_t)(store[0] + kGuard), res[0].eax, nextRand[0]);
            for (size_t i = kGuard; i < kGuard + kArena; ++i) printf("%02x", mem[0][i]);
            printf(" T"); for (u32 v : traces[0]) printf(" %x", v); printf("\n");
        }
        Normalize(res[0].fp); Normalize(res[1].fp);
        const bool okMem = mem[0] == mem[1], okTrace = traces[0] == traces[1], okRet = res[0].eax == res[1].eax, okAbi = res[0].esp && res[1].esp && res[0].nv && res[1].nv,
                   okFp = memcmp(res[0].fp, res[1].fp, 108) == 0, okRand = nextRand[0] == nextRand[1];
        badMem += !okMem; badTrace += !okTrace; badRet += !okRet; badAbi += !okAbi; badFp += !okFp; badRand += !okRand;
        cover[0] += res[0].eax != 0xFFFFFFFFu; cover[1] += f.radiusKind == 0 || f.radiusKind == 3 || f.radiusKind == 1;
        if (!(okMem && okTrace && okRet && okAbi && okFp && okRand)) {
            ++bad;
            if (shown++ < (unsigned)show) {
                printf("DIF c=%u mem=%d trace=%d(%zu/%zu) ret=%d(%08x/%08x) abi=%d fp=%d rand=%d radiusKind=%u dirKind=%u hasVel=%u local=%u pool=%u\n", c, okMem, okTrace, traces[0].size(), traces[1].size(), okRet, res[0].eax, res[1].eax, okAbi, okFp, okRand,
                       f.radiusKind, f.dirKind, f.hasVel, f.createLocal, f.poolMode);
                if (getenv("CPH_DUMP")) {
                    printf("   emission:"); for (u32 v : f.emission) printf(" %08x", v);
                    printf("\n   mult:"); for (u32 v : f.mult) printf(" %08x", v);
                    printf("\n   wld:"); for (u32 v : f.wld) printf(" %08x", v);
                    printf("\n   getRw:"); for (u32 v : f.getRw) printf(" %08x", v);
                    printf("\n   velAdd:"); for (u32 v : f.velAdd) printf(" %08x", v);
                    printf("\n   vel:"); for (u32 v : f.velOverride) printf(" %08x", v);
                    printf("\n   time=%08x bright=%08x seed=%08x\n   particle exe:", f.timeSince, f.brightness, f.seed);
                    for (size_t i = kPartA + kGuard; i < kPartA + kGuard + 0x3C; i += 4) { u32 v; memcpy(&v, &mem[0][i], 4); printf(" %08x", v); }
                    printf("\n   particle cpp:");
                    for (size_t i = kPartA + kGuard; i < kPartA + kGuard + 0x3C; i += 4) { u32 v; memcpy(&v, &mem[1][i], 4); printf(" %08x", v); }
                    printf("\n");
                }
                if (!okMem) { int n = 0; for (size_t i = 0; i < mem[0].size() && n < 8; ++i) if (mem[0][i] != mem[1][i]) { printf("   off=%03zx exe=%02x cpp=%02x\n", i - kGuard, mem[0][i], mem[1][i]); ++n; } }
            }
        }
    }
    printf("casos=%u bad=%u mem=%u traza=%u ret=%u abi=%u fp=%u rand=%u | con particula=%u\n", N, bad, badMem, badTrace, badRet, badAbi, badFp, badRand, cover[0]);
    return bad != 0;
}
#endif
