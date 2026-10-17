// Arnes aislado (sin juego): bytes reales de FxEmitter_c::CreateParticles (0x4A41E0) y de _ftol2 (0x821B40) reubicados, con los colaboradores sustituidos por los mismos
// stubs trazados que usa el candidato C++ (cp_body.inc + ayudantes de X87Math). Compara memoria, traza, retorno y el FNSAVE completo (salvo FIP).
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include "exebytes.inc"
#include "../Fixtures.hpp"
using namespace Dependencies22CFix;
typedef uint32_t u32;

// ---- helpers x87 (X87Math) ----
bool __cdecl FxEmitterAccumulateCpp(float* intensity, const float* cameraDistance, unsigned int rateMult, unsigned int lodStart, unsigned int lodEnd, float count, const float* minWind, const float* maxWind, const float* minRain, const float* maxRain, const float* wind, const float* rain);
int __cdecl FxEmitterTruncateCpp(const float* intensity);
float __cdecl FxEmitterTimeSinceCpp(int step, const float* intensity, float deltaTime);
void __cdecl FxEmitterTakeWholeCpp(float* intensity);

// ---- tipos de apoyo ----
struct RwMatrix { float right[3]; u32 flags; float up[3]; u32 pad1; float at[3]; u32 pad2; float pos[3]; u32 pad3; };
static_assert(sizeof(RwMatrix) == 0x40);
struct EmissionInfo_t { float m_fCount, m_fRadius; float sizeMin[3], sizeMax[3]; float m_Speed, m_fSpeedBias; float dir[3]; float angMin, angMax, life, lifeBias; float pos[3]; float rotMin, rotMax;
    float m_fMinWind, m_fMaxWind, m_fMinRain, m_fMaxRain; };
static_assert(sizeof(EmissionInfo_t) == 0x68);
struct FxPrtMult_c { float a[7]; FxPrtMult_c(); };
struct FxInfoManager_c { int numInfos; void* pInfos; int8_t mov, ren; char al[2]; uint16_t m_nLodStart, m_nLodEnd; bool b1, b2; char c1, c2;
    void ProcessEmissionInfo(float currentTime, float mult, float totalTime, bool useConst, EmissionInfo_t* out); };
struct FxPrimBP_c { char pad[0x2C]; FxInfoManager_c m_FxInfoManager; void GetRWMatrix(RwMatrix& out); };
static_assert(offsetof(FxPrimBP_c, m_FxInfoManager) == 0x2C && offsetof(FxPrimBP_c, m_FxInfoManager.m_nLodStart) == 0x38);
struct FxSystemBP_c { char pad[0xC]; float m_fLength; };
struct FxSystem_c { void* next; void* prev; FxSystemBP_c* m_SystemBP; RwMatrix* m_ParentMatrix; RwMatrix m_LocalMatrix; uint8_t playStatus, killStatus; bool m_UseConstTime; uint8_t pad; float curTime; float m_fCameraDistance;
    uint16_t constTime; uint16_t m_nRateMult; uint16_t timeMult; uint8_t allocated : 1; uint8_t m_createLocal : 1; uint8_t rest : 6; };
static_assert(offsetof(FxSystem_c, m_fCameraDistance) == 0x58 && offsetof(FxSystem_c, m_nRateMult) == 0x5E && offsetof(FxSystem_c, m_UseConstTime) == 0x52 && offsetof(FxSystem_c, m_LocalMatrix) == 0x10);
struct FxEmitterPrt_c;
struct FxEmitter_c { void* vptr; FxPrimBP_c* m_PrimBP; FxSystem_c* m_System; bool m_bEnabled; float m_fEmissionIntensity;
    void CreateParticles(float currentTime, float deltaTime);
    FxEmitterPrt_c* CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const void* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal); };
static_assert(sizeof(FxEmitter_c) == 0x14);
struct FxManager_c { RwMatrix* FxRwMatrixCreate(); void FxRwMatrixDestroy(RwMatrix* m); };
static FxManager_c g_fxMan;
namespace CWeather { static float Wind, Rain; }
typedef int32_t int32;
void RwMatrixUpdate(RwMatrix* m);
RwMatrix* RwMatrixMultiply(RwMatrix* d, const RwMatrix* a, const RwMatrix* b);

// ---- traza ----
static std::vector<u32> g_trace; static u32 g_base_unused = 0;
static uint8_t* g_base;
static u32 Off(const void* p) { return p ? u32((const uint8_t*)p - g_base) : 0xFFFFFFFFu; }
static u32 Hash(const void* p, size_t n) { u32 h = 2166136261u; for (size_t i = 0; i < n; ++i) { h ^= ((const uint8_t*)p)[i]; h *= 16777619u; } return h; }
static RwMatrix* g_mats[3]; static int g_matCount;
static EmissionInfo_t g_emissionSource;

static void __fastcall StubPEI(FxInfoManager_c* self, void* edx, float cur, float mult, float length, bool useConst, EmissionInfo_t* out) {
    g_trace.insert(g_trace.end(), {1, Off(self), B(cur), B(mult), B(length), (u32)useConst});
    *out = g_emissionSource;
}
static RwMatrix* __fastcall StubCreate(void* self, void* edx) { RwMatrix* m = g_mats[g_matCount++ % 3]; g_trace.insert(g_trace.end(), {2, Off(m)}); return m; }
static void __fastcall StubDestroy(void* self, void* edx, RwMatrix* m) { g_trace.insert(g_trace.end(), {3, Off(m)}); }
static void __fastcall StubGetRW(FxPrimBP_c* self, void* edx, RwMatrix* m) {
    g_trace.insert(g_trace.end(), {4, Off(self), Off(m)});
    u32* w = (u32*)m; for (int i = 0; i < 16; ++i) w[i] = 0x600DF000u + i * 7 + g_matCount;
}
static int __cdecl StubUpdate(RwMatrix* m) { g_trace.insert(g_trace.end(), {5, Off(m), Hash(m, 0x40)}); return 1; }
static RwMatrix* __cdecl StubMult(RwMatrix* d, const RwMatrix* a, const RwMatrix* b) {
    g_trace.insert(g_trace.end(), {6, Off(d), Off(a), Off(b), Hash(a, 0x40), Hash(b, 0x40)});
    u32* w = (u32*)d; const u32 h = Hash(a, 0x40) ^ (Hash(b, 0x40) * 31u); for (int i = 0; i < 16; ++i) w[i] = h + i;
    return d;
}
static void __fastcall StubCtor(FxPrtMult_c* self, void* edx) { for (int i = 0; i < 7; ++i) self->a[i] = 1.0f; }
static void* __fastcall StubCP(FxEmitter_c* self, void* edx, const EmissionInfo_t* em, RwMatrix* wld, const void* vel, float timeSince, const FxPrtMult_c* mult, float brightness, bool createLocal) {
    g_trace.insert(g_trace.end(), {8, Off(self), Hash(em, 0x68), Off(wld), Hash(wld, 0x40), vel ? 1u : 0u, B(timeSince), Hash(mult, 0x1C), B(brightness), (u32)createLocal});
    return nullptr;
}
FxPrtMult_c::FxPrtMult_c() { StubCtor(this, nullptr); }
void FxInfoManager_c::ProcessEmissionInfo(float cur, float mult, float length, bool useConst, EmissionInfo_t* out) { StubPEI(this, nullptr, cur, mult, length, useConst, out); }
void FxPrimBP_c::GetRWMatrix(RwMatrix& out) { StubGetRW(this, nullptr, &out); }
RwMatrix* FxManager_c::FxRwMatrixCreate() { return StubCreate(this, nullptr); }
void FxManager_c::FxRwMatrixDestroy(RwMatrix* m) { StubDestroy(this, nullptr, m); }
void RwMatrixUpdate(RwMatrix* m) { StubUpdate(m); }
RwMatrix* RwMatrixMultiply(RwMatrix* d, const RwMatrix* a, const RwMatrix* b) { return StubMult(d, a, b); }
FxEmitterPrt_c* FxEmitter_c::CreateParticle(const EmissionInfo_t& e, RwMatrix& w, const void* v, float t, const FxPrtMult_c& m, float b, bool cl) { return (FxEmitterPrt_c*)StubCP(this, nullptr, &e, &w, v, t, &m, b, cl); }

struct CVector { float x, y, z; };
namespace plugin { template<class R, unsigned A, class C, class... Ts> R CallMethodAndReturn(C self, Ts... a) { return (R)StubCP(self, nullptr, a...); } }
#include "cp_body.inc"

// ---- arena (Arenas.hpp del banco) ----
#include "../Arenas.hpp"
enum : unsigned { kArena = 0x280 };
alignas(16) static uint8_t g_arenaStore[kCpArena + 64];
static void BuildArena(const CreateFx& f) {
    uint8_t* a = g_arenaStore + 16; g_base = a; memset(g_arenaStore, 0xA5, sizeof g_arenaStore);
    BuildCreate(a, f);
    for (int i = 0; i < 3; ++i) g_mats[i] = (RwMatrix*)(a + CpMatrix(i));
    g_matCount = 0;
    BuildEmission((u32*)&g_emissionSource, f);
    memcpy(&CWeather::Wind, &f.wind, 4); memcpy(&CWeather::Rain, &f.rain, 4);
}
static const unsigned kEmit = kCpEmitter;

// ---- EXE reubicado ----
static uint8_t* g_exeCode; static uint8_t* g_ftolCode;
static float K001 = 0.001f, K64 = 0.015625f, K1 = 1.0f, K0 = 0.0f; static float& WIND = CWeather::Wind; static float& RAIN = CWeather::Rain;
static void* g_pool = (void*)0x12340000;
static void BuildExe() {
    g_exeCode = (uint8_t*)VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    g_ftolCode = (uint8_t*)VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy(g_exeCode, kCP, sizeof kCP); memcpy(g_ftolCode, kFtol, sizeof kFtol);
    struct Sym { const char* n; void* p; };
    const Sym data[] = {{"K001", &K001}, {"K64", &K64}, {"K1", &K1}, {"K0", &K0}, {"WIND", &CWeather::Wind}, {"RAIN", &CWeather::Rain}, {"POOL", g_pool}};
    const Sym calls[] = {{"PEI", (void*)&StubPEI}, {"CREATE", (void*)&StubCreate}, {"DESTROY", (void*)&StubDestroy}, {"GETRW", (void*)&StubGetRW}, {"UPDATE", (void*)&StubUpdate},
                         {"MULT", (void*)&StubMult}, {"CTOR", (void*)&StubCtor}, {"CP", (void*)&StubCP}, {"FTOL", g_ftolCode}};
    for (const auto& p : kPatches) {
        if (p.kind == 0) { for (const auto& s : data) if (!strcmp(s.n, p.sym)) { u32 v = (u32)(uintptr_t)s.p; memcpy(g_exeCode + p.off, &v, 4); } }
        else for (const auto& s : calls) if (!strcmp(s.n, p.sym)) { int32_t rel = (int32_t)((uintptr_t)s.p - ((uintptr_t)g_exeCode + p.off + 4)); memcpy(g_exeCode + p.off, &rel, 4); }
    }
}

// ---- ejecucion ----
alignas(16) static uint8_t g_fp[108];
static const double kStale[8] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
struct Res { u32 eax, ecx, edx, esp, nv; uint8_t fp[108]; };
static void __declspec(noinline) RunLane(uintptr_t entry, void* self, float cur, float dt, Res& r) {
    uint16_t cw = 0x007F; const double* staleP = kStale; uint8_t* fp = g_fp; u32 ra{}, rc{}, rd{}, spBefore{}, spAfter{}, b0{}, s0{}, i0{}, p0{}, b1{}, s1{}, i1{}, p1{};
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
        push dt
        push cur
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
    memcpy(r.fp, g_fp, 108);
}
static uintptr_t MemberAddress() { auto pm = &FxEmitter_c::CreateParticles; uintptr_t a; memcpy(&a, &pm, sizeof a); return a; }

static u32 g_rs = 12345;
static u32 Rnd() { g_rs = g_rs * 1664525u + 1013904223u; return g_rs ^ (g_rs >> 15); }
static u32 RndFloat() {
    static const u32 sp[] = {0, 0x80000000, 1, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7F800001, 0xFF800123, 0x7FFFFFFF, 0x7F7FFFFF, 0xFF7FFFFF, 0x7149F2CA, 0x3F000000, 0x40000000, 0x41200000, 0x42C80000, 0x3A83126F};
    const u32 r = Rnd(), k = r % 4;
    if (k == 0) return sp[(r >> 8) % (sizeof sp / sizeof sp[0])];
    if (k == 1) { float f = float(int(Rnd() % 20001) - 10000) / float(1 + Rnd() % 64); return B(f); }
    if (k == 2) return Rnd();
    float f = float(Rnd() % 1000) / 100.0f; return B(f);
}
static CreateFx RandomFx() {
    CreateFx f{};
    f.dist = RndFloat(); f.rate = Rnd() % 65536; f.lodStart = Rnd() % 65536; f.lodEnd = (Rnd() & 1) ? Rnd() % 65536 : f.lodStart + Rnd() % 8; if (Rnd() & 1) { f.lodStart = Rnd() % 200; f.lodEnd = f.lodStart + Rnd() % 200; }
    f.count = RndFloat(); f.init = RndFloat(); f.wind = RndFloat(); f.rain = RndFloat();
    f.minWind = (Rnd() & 1) ? RndFloat() : 0; f.maxWind = (Rnd() & 1) ? RndFloat() : 0x40000000; f.minRain = (Rnd() & 1) ? RndFloat() : 0; f.maxRain = (Rnd() & 1) ? RndFloat() : 0x40000000;
    if (Rnd() & 1) { f.wind = 0x3F800000; f.rain = 0x3F000000; f.minWind = 0; f.maxWind = 0x40000000; f.minRain = 0; f.maxRain = 0x40000000; }
    f.delta = RndFloat(); f.time = Rnd(); f.parent = Rnd() & 1; f.createLocal = Rnd() & 1; f.flagsOther = Rnd() & 0xFD; f.emissionFill = Rnd();
    const double s = PredictedSum(f);
    if (s == s && s >= 16.0 && s < 1.0e19) { f.guarded = true; f.wind = 0x3F000000; f.minWind = 0x7F800000; }
    return f;
}
int main(int argc, char** argv) {
    BuildExe();
    if (argc > 3 && argv[3][0] == 'd') {
        for (unsigned c = 0; c < kWrapCases; ++c) { const auto w = Wrap(c); printf("W %u %08x %u %u %u %08x\n", c, w.angle, w.kind, w.slot, w.fill, w.edx); }
        for (unsigned c = 0; c < kCreateCases; ++c) { const auto f = Create(c); printf("C %u %08x %u %u %u %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %u %u %u %08x %d %d\n", c, f.dist, f.rate, f.lodStart, f.lodEnd, f.count, f.init, f.wind, f.rain, f.minWind, f.maxWind, f.minRain, f.maxRain, f.delta, f.time, f.parent, f.createLocal, f.flagsOther, f.emissionFill, f.benign, f.guarded); }
        return 0;
    }
    const unsigned N = argc > 1 ? atoi(argv[1]) : kCreateCases;
    static unsigned memShown = 0, badSlot[8] = {}, badC = 0, badS = 0, badFtw = 0; unsigned bad = 0, badMem = 0, badTrace = 0, badCtl = 0, badRegs = 0, badStack = 0, shown = 0, loops = 0, badSticky = 0, badNv = 0;
    for (unsigned c = 0; c < N; ++c) {
        const CreateFx f = (argc > 3 && argv[3][0] == 'r') ? RandomFx() : Create(c);
        Res res[2]; std::vector<u32> traces[2]; std::vector<uint8_t> mem[2]; u32 inten[2];
        for (int side = 0; side < 2; ++side) {
            BuildArena(f); g_trace.clear();
            auto* e = (FxEmitter_c*)(g_base + kEmit);
            const float cur = F(f.time), dt = F(f.delta);
            if (side == 0) RunLane((uintptr_t)g_exeCode, e, cur, dt, res[0]); else RunLane(MemberAddress(), e, cur, dt, res[1]);
            traces[side] = g_trace; mem[side].assign(g_base, g_base + kCpArena); memcpy(&inten[side], &e->m_fEmissionIntensity, 4);
        }
        if (argc > 3 && argv[3][0] == 'o') {
            printf("O %u %u ", c, (unsigned)(uintptr_t)g_base);
            for (unsigned i = 0; i < kCpArena; ++i) printf("%02x", mem[0][i]);
            printf(" T"); for (u32 v : traces[0]) printf(" %x", v); printf("\n");
        }
        uint8_t xa[108], xb[108]; memcpy(xa, res[0].fp, 108); memcpy(xb, res[1].fp, 108); memset(xa + 12, 0, 4); memset(xb + 12, 0, 4);
        const bool okMem = mem[0] == mem[1], okTrace = traces[0] == traces[1], ctl = memcmp(xa, xb, 28) == 0, regs = memcmp(xa + 28, xb + 28, 80) == 0;
        { uint8_t ga[28], gb[28]; memcpy(ga, xa, 28); memcpy(gb, xb, 28); ga[4] &= 0xC0; gb[4] &= 0xC0; if (memcmp(ga, gb, 28)) ++badSticky; }
        for (int r = 0; r < 8; ++r) badSlot[r] += memcmp(xa + 28 + 10 * r, xb + 28 + 10 * r, 10) != 0;
        { uint16_t fa = *(uint16_t*)(xa + 4), fb = *(uint16_t*)(xb + 4); badC += ((fa ^ fb) & 0x4700) != 0; badS += ((fa ^ fb) & 0x007F) != 0; badFtw += *(uint16_t*)(xa + 8) != *(uint16_t*)(xb + 8); }
        const bool st = res[0].esp && res[1].esp && res[0].nv && res[1].nv;
        if (traces[0].size() > 40) ++loops;
        badMem += !okMem; badTrace += !okTrace; badCtl += !ctl; badRegs += !regs; badStack += !st;
        if (!okMem && memShown++ < 10) { for (unsigned i = 0; i < kCpArena; ++i) if (mem[0][i] != mem[1][i]) { printf("MEM c=%u off=%03x exe=%02x cpp=%02x dist=%08x init=%08x count=%08x\n", c, i, mem[0][i], mem[1][i], f.dist, f.init, f.count); break; } }
        if (!(okMem && okTrace && ctl && regs && st)) {
            ++bad;
            if (shown++ < (argc > 2 ? atoi(argv[2]) : 6)) {
                printf("DIF c=%u mem=%d trace=%d(%zu/%zu) ctl=%d regs=%d st=%d dist=%08x init=%08x count=%08x inten=%08x/%08x fsw %04x/%04x ftw %04x/%04x\n", c, okMem, okTrace, traces[0].size(), traces[1].size(), ctl, regs, st,
                       f.dist, f.init, f.count, inten[0], inten[1], *(uint16_t*)(xa + 4), *(uint16_t*)(xb + 4), *(uint16_t*)(xa + 8), *(uint16_t*)(xb + 8));
                if (!regs) for (int r = 0; r < 8; ++r) if (memcmp(xa + 28 + 10 * r, xb + 28 + 10 * r, 10)) { printf("  st%d exe=", r); for (int k = 9; k >= 0; --k) printf("%02x", xa[28 + 10 * r + k]); printf(" cpp="); for (int k = 9; k >= 0; --k) printf("%02x", xb[28 + 10 * r + k]); printf("\n"); }
            }
        }
    }
    printf("casos=%u bad=%u mem=%u traza=%u ctl=%u (sin C0-C3: %u) regs=%u pila/nv=%u bucles(>40)=%u\n", N, bad, badMem, badTrace, badCtl, badSticky, badRegs, badStack, loops);
    printf("por registro (st0..st7): "); for (int r = 0; r < 8; ++r) printf("%u ", badSlot[r]); printf(" | C0-C3: %u banderas: %u ftw: %u\n", badC, badS, badFtw);
    return 0;
}
