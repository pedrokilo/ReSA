#include "hcore.hpp"
#include <Base.h>
#include <Vector.h>
#include <X87Math.h>
#include "PointLights.h"
#include <cfloat>
#include <map>
#include "Fixtures.hpp"

// ================= entorno falso de la DLL =================
struct RpWorld; struct CEntity;
struct CColPoint { CVector m_vecPoint; float field_C; CVector m_vecNormal; float field_1C; unsigned char rest[0x2C - 0x20]; };
struct CStoredCollPoly;
struct RwV3d { float x, y, z; };
struct RwTexture { void* raster; };
struct CScene { RpWorld* m_pRpWorld; void* m_pRwCamera; };
static CScene& Scene = *reinterpret_cast<CScene*>(0xC17038 + kShift);
static RwTexture*& gpCloudTex = *At<RwTexture*>(0xC6AA78);
#define RWRSTATE(a) (reinterpret_cast<void*>(a))
#define RwTextureGetRaster(t) ((t)->raster)
enum { rwRENDERSTATETEXTURERASTER = 1, rwRENDERSTATEZTESTENABLE = 6, rwRENDERSTATEZWRITEENABLE = 8, rwRENDERSTATESRCBLEND = 10, rwRENDERSTATEDESTBLEND = 11, rwRENDERSTATEVERTEXALPHAENABLE = 12, rwRENDERSTATEFOGENABLE = 14 };
enum { rwBLENDONE = 2 };
inline int RwRenderStateSetImpl(int s, void* v) { auto fn = *reinterpret_cast<int(__cdecl**)(int, void*)>(*At<uintptr_t>(0xC97B24) + 0x20); return fn(s, v); }
#define RwRenderStateSet(s, v) RwRenderStateSetImpl(int(s), v)
struct CCutsceneMgr { static bool IsRunning() { return *At<int8>(0xB5F851) != 0; } };
struct CTimer { static uint32 GetTimeInMS() { return *At<uint32>(0xB7CB84); } };
struct CWeather { static inline auto& Foggyness = StaticRef<float>(0xC81300); };
struct CCamera { CVector& GetPosition() { auto* m = *At<unsigned char*>(0xB6F03C); return m ? *reinterpret_cast<CVector*>(m + 0x30) : *At<CVector>(0xB6F02C); } };
static CCamera TheCamera;

struct Event { int kind; u32 a[14]; };
static std::vector<Event> gTrace;
static void Rec(int kind, std::initializer_list<u32> v) {
    unsigned short sw = 0; __asm { fnstsw sw }
    Event e{}; e.kind = kind; int i = 0; for (u32 x : v) e.a[i++] = x;
    static const bool withFsw = getenv("FSWTRACE") != nullptr;
    if (withFsw) e.a[13] = sw;
    gTrace.push_back(e);
}

// ---- comportamiento de los colaboradores (determinista por caso y por orden de llamada) ----
struct Fix { u32 seed; int calcCalls, pvlCalls, cacheCalls; int pCalc, pPvl, pCache; };
static Fix gFix;
static u32 gExtraStress;
static std::string gMutDesc;
static u32 H(u32 a, u32 b) { u32 x = a * 0x9E3779B1u + b * 0x85EBCA6Bu + 0x1234567u; x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
static float StressOr(u32 h, float lo, float hi, u32 pStress) {
    Rng r(h); return RndF(r, lo, hi, pStress + gExtraStress);
}
static int __cdecl FakeRenderState(int s, void* v) { Rec(10, {u32(s), u32(uintptr_t(v))}); return 1; }
static void __cdecl FakeInit() { Rec(11, {}); }
static void __cdecl FakeFlush() { Rec(12, {}); }
static bool CalcImpl(const float* posn, float* out, float* w, float* h, u32 maxv, u32 minv) {
    const u32 k = H(gFix.seed, 100 + gFix.calcCalls++);
    Rec(13, {FloatToBits(posn[0]), FloatToBits(posn[1]), FloatToBits(posn[2]), maxv & 0xFF, minv & 0xFF});
    const bool ok = (k % 10) < gFix.pCalc;
    if (!ok) return false;
    u32* o = reinterpret_cast<u32*>(out);
    o[0] = FloatToBits(StressOr(H(k, 1), 0, 640, 25)); o[1] = FloatToBits(StressOr(H(k, 2), 0, 480, 25)); o[2] = FloatToBits(StressOr(H(k, 3), 0.5f, 90, 40));
    *reinterpret_cast<u32*>(w) = FloatToBits(StressOr(H(k, 4), 0.1f, 40, 40)); *reinterpret_cast<u32*>(h) = FloatToBits(StressOr(H(k, 5), 0.1f, 40, 40));
    return true;
}
static bool __cdecl FakeCalcExe(const float* posn, float* out, float* w, float* h, u32 maxv, u32 minv) { return CalcImpl(posn, out, w, h, maxv, minv); }
static bool PvlImpl(const float* origin, u32 distBits, unsigned char* colPoint, u32 b, u32 v, u32 p, u32 o, u32 d, u32 s, u32 poly) {
    const u32 k = H(gFix.seed, 200 + gFix.pvlCalls++);
    Rec(14, {FloatToBits(origin[0]), FloatToBits(origin[1]), FloatToBits(origin[2]), distBits, b & 0xFF, v & 0xFF, p & 0xFF, o & 0xFF, d & 0xFF, s & 0xFF, poly});
    const bool ok = (k % 10) < gFix.pPvl;
    if (ok) { memset(colPoint, 0, 0x2C); const u32 z = FloatToBits(StressOr(H(k, 7), 0, 30, 60)); memcpy(colPoint + 8, &z, 4); }
    return ok;
}
static bool __cdecl FakePvlExe(const float* origin, u32 distBits, u32 colPoint, u32 entityRef, u32 b, u32 v, u32 p, u32 o, u32 d, u32 s, u32 poly) { return PvlImpl(origin, distBits, reinterpret_cast<unsigned char*>(colPoint), b, v, p, o, d, s, poly); }
static bool __cdecl FakeCacheRaw(u32 x, u32 y, u32 z, u32 outZ) {
    const u32 k = H(gFix.seed, 300 + gFix.cacheCalls++);
    Rec(15, {x, y, z});
    const bool ok = (k % 10) < gFix.pCache;
    if (ok) *reinterpret_cast<u32*>(outZ) = FloatToBits(StressOr(H(k, 9), 0, 30, 60));
    return ok;
}
static void __cdecl FakeSpriteRaw(u32 x, u32 y, u32 z, u32 w, u32 h, u32 r, u32 g, u32 b, u32 intensity, u32 recip, u32 angle, u32 alpha) {
    Rec(16, {x, y, z, w, h, r & 0xFF, g & 0xFF, b & 0xFF, intensity & 0xFFFF, recip, angle, alpha & 0xFF});
}
struct CSprite {
    static void InitSpriteBuffer() { FakeInit(); }
    static void FlushSpriteBuffer() { FakeFlush(); }
    static bool CalcScreenCoors(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible) { return CalcImpl(&posn.x, &out->x, w, h, checkMaxVisible, checkMinVisible); }
    static void RenderBufferedOneXLUSprite_Rotate_Aspect(float x, float y, float z, float w, float h, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, float angle, uint8 alpha) {
        FakeSpriteRaw(FloatToBits(x), FloatToBits(y), FloatToBits(z), FloatToBits(w), FloatToBits(h), r, g, b, uint16(intensity), FloatToBits(recipNearZ), FloatToBits(angle), alpha);
    }
};
struct CWorld {
    static bool ProcessVerticalLine(const CVector& origin, float distance, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
        return PvlImpl(&origin.x, FloatToBits(distance), reinterpret_cast<unsigned char*>(&outColPoint), buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, u32(uintptr_t(outCollPoly)));
    }
};
void AddAnExtraDirectionalLight(RpWorld*, float, float, float, float, float, float) {}
extern "C" float __cdecl PointLightsLightMultiplierFltCpp(const float* point, const void* lights, int count);
#pragma comment(linker, "/alternatename:_PointLightsLightMultiplierFltCpp=_PointLightsLightMultiplierDblCpp")

#include "core4.inc"

static void Patch(uintptr_t at, void* target) {
    unsigned char* p = reinterpret_cast<unsigned char*>(at);
    DWORD old; VirtualProtect(p, 8, PAGE_EXECUTE_READWRITE, &old);
    p[0] = 0xE9; int rel = int(uintptr_t(target) - (at + 5)); memcpy(p + 1, &rel, 4);
}

static const uintptr_t kRegionLo = 0xC3F000 + kShift, kRegionHi = 0xC40000 + kShift;
struct Mem { std::vector<unsigned char> b; };
static void Save(Mem& m) { m.b.assign((unsigned char*)kRegionLo, (unsigned char*)kRegionHi); }
static void Load(const Mem& m) { memcpy((void*)kRegionLo, m.b.data(), m.b.size()); }

static bool TraceEq(const std::vector<Event>& a, const std::vector<Event>& b, size_t* at = nullptr) {
    size_t n = a.size() < b.size() ? a.size() : b.size();
    for (size_t i = 0; i < n; ++i) if (a[i].kind != b[i].kind || memcmp(a[i].a, b[i].a, sizeof(a[i].a))) { if (at) *at = i; return false; }
    if (a.size() != b.size()) { if (at) *at = n; return false; }
    return true;
}

int main(int argc, char** argv) {
    MapImage(); CapturePrime();
    const int N = argc > 1 ? atoi(argv[1]) : 2000;
    const int firstCase = argc > 2 ? atoi(argv[2]) : 0;
    // motor falso: 0xC97B24 -> bloque con la funcion de estados de render en +0x20
    static unsigned char engine[0x100];
    *reinterpret_cast<void**>(engine + 0x20) = (void*)&FakeRenderState;
    *At<void*>(0xC97B24) = engine;
    static RwTexture texture{reinterpret_cast<void*>(0x5555AAAA)};
    gpCloudTex = &texture;
    Patch(0x70CFB0 + kShift, (void*)&FakeInit);
    Patch(0x70CF20 + kShift, (void*)&FakeFlush);
    Patch(0x70CE30 + kShift, (void*)&FakeCalcExe);
    Patch(0x5674E0 + kShift, (void*)&FakePvlExe);
    Patch(0x6FFFF0 + kShift, (void*)&FakeCacheRaw);
    Patch(0x70E780 + kShift, (void*)&FakeSpriteRaw);
    // el ProcessVerticalLineUsingCache de la DLL tambien se sustituye (EntryPatch en su entrada)
    Patch(reinterpret_cast<uintptr_t>(&CPointLights::ProcessVerticalLineUsingCache), (void*)&FakeCacheRaw);

    int pairs = 0, diffAny = 0, diffTrace = 0, diffFpLive = 0, diffFpDead = 0, diffAbi = 0, diffRegs = 0, reported = 0;
    std::map<std::string, int> kinds;
    std::map<int, long> evCount; long covCalc = 0, covPvl = 0, covSprite = 0, covCache = 0;
    static unsigned char cameraBlock[0x100];
    for (int c = firstCase; c < firstCase + N; ++c) {
        Rng r(c + 99);
        memset((void*)kRegionLo, 0, kRegionHi - kRegionLo);
        const int n = (r.below(8) == 0) ? r.below(5) : 1 + r.below(4);
        *At<int>(0xC3F0D0) = n;
        const float cx = 100, cy = 200, cz = 30;
        for (int i = 0; i < 32; ++i) {
            float* L = At<float>(0xC3F0E0 + i * 0x30);
            L[0] = RndF(r, cx - 14, cx + 14, 15); L[1] = RndF(r, cy - 14, cy + 14, 15); L[2] = RndF(r, cz - 4, cz + 4, 15);
            const float ang = r.uni(0, 6.2832f);
            L[3] = r.below(30) == 0 ? RndF(r, -1, 1, 1000) : cosf(ang); L[4] = r.below(30) == 0 ? RndF(r, -1, 1, 1000) : sinf(ang); L[5] = r.below(30) == 0 ? RndF(r, -1, 1, 1000) : r.uni(-0.3f, 0.3f);
            L[6] = RndF(r, 1, 30, 30);
            for (int k = 0; k < 3; ++k) L[7 + k] = RndF(r, 0.f, 255.f, 50);
            ((unsigned char*)L)[0x2C] = (unsigned char)(r.below(3) == 0 ? r.below(7) : (r.below(2) ? 1 : 0));
            ((unsigned char*)L)[0x2D] = (unsigned char)(r.below(8) == 0 ? r.below(4) : 1 + r.below(2));
            ((unsigned char*)L)[0x2E] = (unsigned char)r.below(2);
        }
        *At<int8>(0xB5F851) = (r.below(40) == 0) ? 1 : 0;
        *At<float>(0xC81300) = r.below(10) == 0 ? 0.0f : RndF(r, 0.f, 1.f, 60);
        *At<uint32>(0xB7CB84) = r.next();
        if (r.below(2)) { *At<void*>(0xB6F03C) = cameraBlock; float* p = reinterpret_cast<float*>(cameraBlock + 0x30); p[0] = RndF(r, cx - 8, cx + 8, 30); p[1] = RndF(r, cy - 8, cy + 8, 30); p[2] = RndF(r, cz - 3, cz + 3, 30); }
        else { *At<void*>(0xB6F03C) = nullptr; float* p = At<float>(0xB6F02C); p[0] = RndF(r, cx - 8, cx + 8, 30); p[1] = RndF(r, cy - 8, cy + 8, 30); p[2] = RndF(r, cz - 3, cz + 3, 30); }
        float* tab = At<float>(0x8D5068);   // tabla del EXE (se deja la original)
        (void)tab;
        gExtraStress = 0;
        if (getenv("MUT")) {
            std::vector<u32*> targets;
            for (int i = 0; i < n; ++i) { u32* L = At<u32>(0xC3F0E0 + i * 0x30); for (int k : {0, 1, 2, 3, 4, 5, 7, 8, 9}) targets.push_back(L + k); }
            u32* camp = *At<void*>(0xB6F03C) ? reinterpret_cast<u32*>(cameraBlock + 0x30) : At<u32>(0xB6F02C);
            for (int k = 0; k < 3; ++k) targets.push_back(camp + k);
            targets.push_back(At<u32>(0xC81300)); targets.push_back(At<u32>(0xC81300));
            const int m = 1 + r.below(atoi(getenv("MUT")));
            int sz; const u32* tb = StressTable(sz);
            gMutDesc.clear();
            for (int j = 0; j < m; ++j) { u32* tg = targets[r.below(targets.size())]; const u32 v = tb[r.below(sz)]; *tg = v; char b[96]; snprintf(b, sizeof b, " [@%08x=%08x]", unsigned(uintptr_t(tg) - kShift), v); gMutDesc += b; }
            gExtraStress = r.below(4) == 0 ? r.below(400) : 0;
        }
        gFix.seed = c * 7 + 1; gFix.pCalc = 3 + r.below(8); gFix.pPvl = 3 + r.below(8); gFix.pCache = 3 + r.below(8);
        if (getenv("BANKFIX")) {
            namespace F = Dependencies22AFix;
            const F::FogFx f = F::Fog(c);
            memset((void*)kRegionLo, 0, kRegionHi - kRegionLo);
            *At<int>(0xC3F0D0) = int(f.count);
            for (int i = 0; i < 32; ++i) {
                const F::LightFx l = F::FogLight(c, i); u32* L = At<u32>(0xC3F0E0 + i * 0x30);
                u32 w[11] = {l.pos[0], l.pos[1], l.pos[2], l.dir[0], l.dir[1], l.dir[2], l.radius, l.color[0], l.color[1], l.color[2], l.entity};
                memcpy(L, w, 44); ((unsigned char*)L)[0x2C] = (unsigned char)l.type; ((unsigned char*)L)[0x2D] = (unsigned char)l.fog; ((unsigned char*)L)[0x2E] = (unsigned char)l.shadows;
            }
            *At<int8>(0xB5F851) = f.cutscene; *At<u32>(0xC81300) = f.foggy; *At<u32>(0xB7CB84) = f.timer;
            u32* camp;
            if (f.cameraMatrix) { *At<void*>(0xB6F03C) = cameraBlock; camp = reinterpret_cast<u32*>(cameraBlock + 0x30); } else { *At<void*>(0xB6F03C) = nullptr; camp = At<u32>(0xB6F02C); }
            for (int k = 0; k < 3; ++k) camp[k] = f.camera[k];
            gFix.seed = f.seed; gFix.pCalc = f.pCalc; gFix.pPvl = f.pPvl; gFix.pCache = f.pCache;
        }
        Mem base; Save(base);
        if (getenv("DUMPCASE") && atoi(getenv("DUMPCASE")) == c) {
            printf("case %d n=%d foggy=%08x timer=%08x cutscene=%d cam=%p\n", c, *At<int>(0xC3F0D0), *At<u32>(0xC81300), *At<u32>(0xB7CB84), *At<int8>(0xB5F851), *At<void*>(0xB6F03C));
            u32* cp = *At<void*>(0xB6F03C) ? reinterpret_cast<u32*>(cameraBlock + 0x30) : At<u32>(0xB6F02C);
            printf(" camera: %08x %08x %08x\n", cp[0], cp[1], cp[2]);
            for (int i = 0; i < *At<int>(0xC3F0D0); ++i) { u32* L = At<u32>(0xC3F0E0 + i * 0x30); printf("  L%d:", i); for (int k = 0; k < 11; ++k) printf(" %08x", L[k]); unsigned char* b = (unsigned char*)L; printf(" type=%d fog=%d\n", b[0x2C], b[0x2D]); }
        }
        Out o{}, d{}; Mem mo, md; std::vector<Event> to, td;
        Load(base); gFix.calcCalls = gFix.pvlCalls = gFix.cacheCalls = 0; gTrace.clear(); PrimeX87();
        Gate(0x7002D0 + kShift, nullptr, 0, o); Save(mo); to = gTrace;
        Load(base); gFix.calcCalls = gFix.pvlCalls = gFix.cacheCalls = 0; gTrace.clear(); PrimeX87();
        Gate(reinterpret_cast<uintptr_t>(&CPointLights::RenderFogEffect), nullptr, 0, d); Save(md); td = gTrace;
        ++pairs;
        if (getenv("DUMPCASE") && atoi(getenv("DUMPCASE")) == c) { for (auto& e : to) { printf("  ev kind=%d:", e.kind); for (int k = 0; k < 12; ++k) printf(" %08x", e.a[k]); printf("\n"); } }
        { bool k13 = false, k14 = false, k16 = false, k15 = false; for (auto& e : to) { k13 |= e.kind == 13; k14 |= e.kind == 14; k16 |= e.kind == 16; k15 |= e.kind == 15; } covCalc += k13; covPvl += k14; covSprite += k16; covCache += k15; }
        for (auto& e : to) evCount[e.kind]++;
        bool bad = false; std::string why;
        if (!o.stack || !o.nonvolatile || !d.stack || !d.nonvolatile) { why += "abi "; bad = true; ++diffAbi; }
        size_t at = 0;
        if (!TraceEq(to, td, &at)) { ++diffTrace; why += "trace "; bad = true; if (reported < 6) { printf("  trace case %d: exe %zu eventos, dll %zu, primera diferencia en %zu", c, to.size(), td.size(), at);
            if (at < to.size()) { printf(" exe kind=%d:", to[at].kind); for (int k = 0; k < 12; ++k) printf(" %08x", to[at].a[k]); }
            if (at < td.size()) { printf("\n      dll kind=%d:", td[at].kind); for (int k = 0; k < 12; ++k) printf(" %08x", td[at].a[k]); }
            printf("\n"); } }
        if (mo.b != md.b) { why += "mem "; bad = true; }
        if (o.ecx != d.ecx || o.edx != d.edx) ++diffRegs;
        if (getenv("DEADSTAT") || getenv("DEADVAL")) {
            const bool deadOnly = !FpDiff(o.fp, d.fp, true, true).empty() && FpDiff(o.fp, d.fp, false, true).empty();
            if (getenv("DEADSTAT")) { static std::map<std::string, int> h; std::string key = "exe=" + PhysSig(o.fp) + " dll=" + PhysSig(d.fp) + (deadOnly ? " DIF" : " ok"); if (++h[key] == 1) printf("DEAD %s\n", key.c_str()); }
            if (getenv("DEADVAL") && deadOnly) { static int cnt = 0; if (cnt++ < atoi(getenv("DEADVAL"))) { printf("DV case %d exe=%s dll=%s", c, PhysSig(o.fp).c_str(), PhysSig(d.fp).c_str());
                for (unsigned r = 0; r < 8; ++r) { const unsigned so = (r - TopOf(o.fp)) & 7, sd = (r - TopOf(d.fp)) & 7; if (memcmp(o.fp + 28 + 10 * so, d.fp + 28 + 10 * sd, 10)) printf(" | r%u exe=%g dll=%g", r, Ext2D(o.fp + 28 + 10 * so), Ext2D(d.fp + 28 + 10 * sd)); } printf("\n"); } }
        }
        const std::string live = FpDiff(o.fp, d.fp, false, true), all = FpDiff(o.fp, d.fp, true, true);
        if (!live.empty()) { ++diffFpLive; why += "fp(" + live + ") "; bad = true; char buf[64]; snprintf(buf, sizeof buf, "x%04x", W16(o.fp + 4) ^ W16(d.fp + 4)); why += buf; }
        else if (!all.empty()) ++diffFpDead;
        if (bad) { ++diffAny; ++kinds[why]; if (reported < 40) { ++reported; printf("  DIFF case %d: %s fsw exe=%04x dll=%04x events=%zu mut:%s\n", c, why.c_str(), W16(o.fp + 4), W16(d.fp + 4), to.size(), gMutDesc.c_str()); } }
    }
    printf("RenderFogEffect: pares=%d difAny=%d trace=%d fpVivo=%d fpMuertoSolo=%d abi=%d regs=%d\n", pairs, diffAny, diffTrace, diffFpLive, diffFpDead, diffAbi, diffRegs);
    for (auto& k : kinds) printf("   %6d x %s\n", k.second, k.first.c_str());
    printf("cobertura casos: calc=%ld pvl=%ld sprite=%ld cache=%ld\n", covCalc, covPvl, covSprite, covCache);
    printf("eventos EXE: "); for (auto& e : evCount) printf("[%d]=%ld ", e.first, e.second); printf("\n");
    return 0;
}
