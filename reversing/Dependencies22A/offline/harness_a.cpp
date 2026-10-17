#include "hcore.hpp"
#include <Base.h>
#include <Vector.h>
#include <X87Math.h>
#include "PointLights.h"
#include <cfloat>
#include <map>

// ---- entorno falso de la DLL ----
struct RpWorld; struct CEntity;
struct CColPoint { CVector m_vecPoint; float field_C; CVector m_vecNormal; float field_1C; unsigned char rest[0x2C - 0x20]; };
struct CStoredCollPoly;
struct CScene { RpWorld* m_pRpWorld; void* m_pRwCamera; };
static CScene& Scene = *reinterpret_cast<CScene*>(0xC17038 + kShift);
struct Event { int kind; u32 a[12]; };
static std::vector<Event> gTrace;
static void __cdecl FakeAddLightRaw(u32 world, u32 x, u32 y, u32 z, u32 r, u32 g, u32 b) { Event e{1, {world, x, y, z, r, g, b}}; gTrace.push_back(e); }
void AddAnExtraDirectionalLight(RpWorld* world, float x, float y, float z, float red, float green, float blue) {
    FakeAddLightRaw(u32(uintptr_t(world)), FloatToBits(x), FloatToBits(y), FloatToBits(z), FloatToBits(red), FloatToBits(green), FloatToBits(blue));
}
struct PVLFix { bool ret; float z; };
static PVLFix gPvl;
// El EXE llama a 0x5674E0(origin*, dist, colPoint*, entity*&, 7 bools, poly): se registra el origen por valor
static bool __cdecl FakePvlExe(const float* origin, u32 distBits, u32 colPoint, u32 entityRef, u32 b, u32 v, u32 p, u32 o, u32 d, u32 s, u32 poly) {
    u32 ox, oy, oz; memcpy(&ox, origin, 4); memcpy(&oy, origin + 1, 4); memcpy(&oz, origin + 2, 4);
    Event e{2, {ox, oy, oz, distBits, b & 0xFF, v & 0xFF, p & 0xFF, o & 0xFF, d & 0xFF, s & 0xFF, poly}}; gTrace.push_back(e);
    if (gPvl.ret) { memset((void*)colPoint, 0, 0x2C); float z = gPvl.z; memcpy((char*)colPoint + 8, &z, 4); }
    return gPvl.ret;
}
struct CWorld {
    static bool ProcessVerticalLine(const CVector& origin, float distance, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, CStoredCollPoly* outCollPoly) {
        return FakePvlExe(&origin.x, FloatToBits(distance), u32(uintptr_t(&outColPoint)), u32(uintptr_t(&outEntity)), buildings, vehicles, peds, objects, dummies, doSeeThroughCheck, u32(uintptr_t(outCollPoly)));
    }
};
extern "C" float __cdecl PointLightsLightMultiplierFltCpp(const float* point, const void* lights, int count);
#pragma comment(linker, "/alternatename:_PointLightsLightMultiplierFltCpp=_PointLightsLightMultiplierDblCpp")

#include "core3.inc"

static void Patch(uintptr_t at, void* target) {
    unsigned char* p = reinterpret_cast<unsigned char*>(at);
    DWORD old; VirtualProtect(p, 8, PAGE_EXECUTE_READWRITE, &old);
    p[0] = 0xE9; int rel = int(uintptr_t(target) - (at + 5)); memcpy(p + 1, &rel, 4);
}

// ---- memoria vigilada ----
static const uintptr_t kRegionLo = 0xC3F000 + kShift, kRegionHi = 0xC40000 + kShift;
struct Mem { std::vector<unsigned char> b; };
static void Save(Mem& m) { m.b.assign((unsigned char*)kRegionLo, (unsigned char*)kRegionHi); }
static void Load(const Mem& m) { memcpy((void*)kRegionLo, m.b.data(), m.b.size()); }

struct Stats { int pairs = 0, diffAny = 0, diffRet = 0, diffMem = 0, diffTrace = 0, diffFpCore = 0, diffFpDead = 0, diffRegs = 0; std::map<std::string, int> kinds; };

static bool TraceEq(const std::vector<Event>& a, const std::vector<Event>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) if (a[i].kind != b[i].kind || memcmp(a[i].a, b[i].a, sizeof(a[i].a))) return false;
    return true;
}

int main(int argc, char** argv) {
    MapImage(); CapturePrime();
    const int N = argc > 2 ? atoi(argv[2]) : 20000;
    const std::string which = argc > 1 ? argv[1] : "all";
    Patch(0x735840 + kShift, (void*)&FakeAddLightRaw);
    Patch(0x5674E0 + kShift, (void*)&FakePvlExe);
    int reported = 0;
    auto compare = [&](const char* name, int c, Stats& st, const Out& o, const Out& d, const Mem& mo, const Mem& md, const std::vector<Event>& to, const std::vector<Event>& td, bool compareRet, u32 retMask) {
        ++st.pairs;
        bool bad = false;
        std::string why;
        if (!o.stack || !o.nonvolatile || !d.stack || !d.nonvolatile) { why += "abi "; bad = true; }
        if (compareRet && ((o.eax ^ d.eax) & retMask)) { ++st.diffRet; why += "ret "; bad = true; }
        if (mo.b != md.b) { ++st.diffMem; why += "mem "; bad = true; }
        if (!TraceEq(to, td)) { ++st.diffTrace; why += "trace "; bad = true; }
        if (o.ecx != d.ecx || o.edx != d.edx) { ++st.diffRegs; }
        if (getenv("DEADSTAT")) { static std::map<std::string,int> h; static std::string cur; std::string key = std::string(name) + " exe=" + PhysSig(o.fp) + " dll=" + PhysSig(d.fp) + " top " + std::to_string(TopOf(o.fp)) + "/" + std::to_string(TopOf(d.fp)); if (++h[key] == 1 && h.size() < 60) printf("DEAD %s\n", key.c_str()); }
        if (getenv("DEADVAL") && !FpDiff(o.fp, d.fp, true, true).empty() && FpDiff(o.fp, d.fp, false, true).empty()) { static int cnt = 0; if (cnt++ < atoi(getenv("DEADVAL"))) { printf("DV %s case %d sig exe=%s dll=%s", name, c, PhysSig(o.fp).c_str(), PhysSig(d.fp).c_str());
            for (unsigned r = 0; r < 8; ++r) { const unsigned so = (r - TopOf(o.fp)) & 7, sd = (r - TopOf(d.fp)) & 7; if (memcmp(o.fp + 28 + 10 * so, d.fp + 28 + 10 * sd, 10)) printf(" | r%u exe=%g dll=%g", r, Ext2D(o.fp + 28 + 10 * so), Ext2D(d.fp + 28 + 10 * sd)); } printf("\n"); } }
        std::string live = FpDiff(o.fp, d.fp, false, true);
        std::string all = FpDiff(o.fp, d.fp, true, true);
        if (!live.empty()) { ++st.diffFpCore; why += "fp(" + live + ") "; bad = true;
            const u32 fo = W16(o.fp + 4), fd = W16(d.fp + 4); char buf[64]; snprintf(buf, sizeof buf, "x%04x", fo ^ fd); why += buf; }
        else if (!all.empty()) { ++st.diffFpDead; }
        if (getenv("DUMPDEAD") && !all.empty() && reported < 3) { ++reported; printf("case %d deadiff: %s\n", c, all.c_str());
            for (int k = 0; k < 8; ++k) { printf("  st%d%s exe=", k, RegDead(o.fp, k) ? "(dead)" : "(live)"); for (int j = 9; j >= 0; --j) printf("%02x", o.fp[28 + 10 * k + j]); printf(" dll="); for (int j = 9; j >= 0; --j) printf("%02x", d.fp[28 + 10 * k + j]); printf("\n"); }
            printf("  fsw exe=%04x dll=%04x ftw exe=%04x dll=%04x\n", W16(o.fp + 4), W16(d.fp + 4), W16(o.fp + 8), W16(d.fp + 8)); }
        if (bad) { ++st.diffAny; ++st.kinds[why]; if (reported < 8) { ++reported; printf("  DIFF %s case %d: %s  fsw exe=%04x dll=%04x\n", name, c, why.c_str(), W16(o.fp + 4), W16(d.fp + 4)); } }
    };
    auto report = [&](const char* name, const Stats& st) {
        printf("%s: pares=%d difAny=%d ret=%d mem=%d trace=%d fpVivo=%d fpMuertoSolo=%d regs(ecx/edx)=%d\n", name, st.pairs, st.diffAny, st.diffRet, st.diffMem, st.diffTrace, st.diffFpCore, st.diffFpDead, st.diffRegs);
        for (auto& k : st.kinds) printf("   %6d x %s\n", k.second, k.first.c_str());
    };

    // =========================== GenerateLightsAffectingObject
    if (which == "all" || which == "gen") {
        Stats st; reported = 0;
        alignas(16) static float point[4]; alignas(16) static float total[4];
        for (int c = 0; c < N; ++c) {
            Rng r(c + 1);
            memset((void*)kRegionLo, 0, kRegionHi - kRegionLo);
            const int n = (r.below(10) == 0) ? r.below(33) : 1 + r.below(5);
            *At<int>(0xC3F0D0) = (r.below(200) == 0) ? (int)(r.below(3)) - 1 : n;
            for (int i = 0; i < 32; ++i) {
                float* L = At<float>(0xC3F0E0 + i * 0x30);
                const int mode = r.below(4);
                const float cx = 100, cy = 200, cz = 30;
                const float rad = r.below(20) == 0 ? RndF(r, 0, 40, 400) : r.uni(0.5f, 40.f);
                L[0] = RndF(r, cx - 25, cx + 25); L[1] = RndF(r, cy - 25, cy + 25); L[2] = RndF(r, cz - 25, cz + 25);
                for (int k = 0; k < 3; ++k) L[3 + k] = RndF(r, -1.f, 1.f, 60);
                L[6] = rad;
                for (int k = 0; k < 3; ++k) L[7 + k] = RndF(r, 0.f, 2.f, 60);
                const u32 ent = r.below(3) == 0 ? 0x1000 : (r.below(2) ? 0x2000 : 0);
                memcpy((char*)L + 0x28, &ent, 4);
                ((unsigned char*)L)[0x2C] = (unsigned char)(mode == 0 ? r.below(6) : (r.below(3)));
                ((unsigned char*)L)[0x2D] = (unsigned char)r.below(3);
                ((unsigned char*)L)[0x2E] = (unsigned char)r.below(2);
            }
            point[0] = RndF(r, 75, 125, 30); point[1] = RndF(r, 175, 225, 30); point[2] = RndF(r, 5, 55, 30);
            const bool useTotal = r.below(4) != 0;
            const float totalInit = RndF(r, 0, 3, 50);
            const u32 entity = r.below(2) ? 0x1000 : 0x3000;
            *At<void*>(0xC17038) = (void*)(uintptr_t)(0x12340000 + r.below(16));
            if (getenv("SINGLE") && getenv("DUMPCASE") && atoi(getenv("DUMPCASE")) == c) {
                const int s = atoi(getenv("SINGLE"));
                memcpy(At<unsigned char>(0xC3F0E0), At<unsigned char>(0xC3F0E0 + s * 0x30), 0x30);
                *At<int>(0xC3F0D0) = 1;
            }
            Mem base; Save(base);
            if (getenv("DUMPCASE") && atoi(getenv("DUMPCASE")) == c) {
                printf("case %d n=%d point=%08x %08x %08x entity=%x useTotal=%d totalInit=%08x\n", c, *At<int>(0xC3F0D0), FloatToBits(point[0]), FloatToBits(point[1]), FloatToBits(point[2]), entity, int(useTotal), FloatToBits(totalInit));
                for (int i = 0; i < *At<int>(0xC3F0D0) && i < 32; ++i) { u32* L = At<u32>(0xC3F0E0 + i * 0x30); printf("  L%d:", i); for (int k = 0; k < 11; ++k) printf(" %08x", L[k]); printf("\n"); }
            }
            Out o{}, d{}; Mem mo, md; std::vector<Event> to, td;
            Load(base); total[0] = totalInit; total[1] = 0; gTrace.clear(); PrimeX87();
            u32 args[3] = {u32(uintptr_t(point)), u32(uintptr_t(useTotal ? total : nullptr)), entity};
            Gate(0x6FFBB0 + kShift, args, 3, o); Save(mo); to = gTrace;
            const u32 totalO = FloatToBits(total[0]);
            Load(base); total[0] = totalInit; total[1] = 0; gTrace.clear(); PrimeX87();
            Gate(reinterpret_cast<uintptr_t>(&CPointLights::GenerateLightsAffectingObject), args, 3, d); Save(md); td = gTrace;
            const u32 totalD = FloatToBits(total[0]);
            if (totalO != totalD) md.b[0] ^= 1;
            compare("gen", c, st, o, d, mo, md, to, td, false, 0);
        }
        report("GenerateLights", st);
    }
    // =========================== GetLightMultiplier
    if (which == "all" || which == "mult") {
        Stats st; reported = 0;
        alignas(16) static float point[4];
        for (int c = 0; c < N; ++c) {
            Rng r(c + 7777);
            memset((void*)kRegionLo, 0, kRegionHi - kRegionLo);
            const int n = (r.below(10) == 0) ? r.below(33) : 1 + r.below(5);
            *At<int>(0xC3F0D0) = n;
            for (int i = 0; i < 32; ++i) {
                float* L = At<float>(0xC3F0E0 + i * 0x30);
                const float cx = 100, cy = 200, cz = 30;
                const float rad = r.below(20) == 0 ? RndF(r, 0, 40, 400) : r.uni(0.5f, 40.f);
                L[0] = RndF(r, cx - 25, cx + 25); L[1] = RndF(r, cy - 25, cy + 25); L[2] = RndF(r, cz - 25, cz + 25);
                L[6] = rad;
                for (int k = 0; k < 3; ++k) L[7 + k] = RndF(r, 0.f, 2.f, 60);
                ((unsigned char*)L)[0x2C] = (unsigned char)r.below(6);
            }
            point[0] = RndF(r, 75, 125, 30); point[1] = RndF(r, 175, 225, 30); point[2] = RndF(r, 5, 55, 30);
            Mem base; Save(base);
            Out o{}, d{}; Mem mo, md; std::vector<Event> to, td;
            Load(base); gTrace.clear(); PrimeX87();
            u32 args[1] = {u32(uintptr_t(point))};
            Gate(0x6FFE70 + kShift, args, 1, o); Save(mo);
            Load(base); gTrace.clear(); PrimeX87();
            Gate(reinterpret_cast<uintptr_t>(&CPointLights::GetLightMultiplier), args, 1, d); Save(md);
            compare("mult", c, st, o, d, mo, md, to, td, false, 0);
        }
        report("GetLightMultiplier", st);
    }
    // =========================== ProcessVerticalLineUsingCache
    if (which == "all" || which == "cache") {
        Stats st; reported = 0;
        alignas(16) static float outZ[4];
        for (int c = 0; c < N; ++c) {
            Rng r(c + 31337); if (getenv("TRACE")) { fprintf(stderr, "cache case %d\n", c); fflush(stderr); }
            memset((void*)kRegionLo, 0, kRegionHi - kRegionLo);
            float p[3]; for (int k = 0; k < 3; ++k) p[k] = RndF(r, -100, 100, 60);
            for (int i = 0; i < 32; ++i) {
                float* e = At<float>(0xC3F6E0 + i * 12);
                for (int k = 0; k < 3; ++k) e[k] = RndF(r, -100, 100, 60);
                if (r.below(30) == 0) { memcpy(e, p, 12); }
                At<float>(0xC3F050)[i] = RndF(r, -50, 50, 60);
            }
            if (r.below(25) == 0) { for (int i = 0; i < 32; ++i) memcpy(At<float>(0xC3F6E0 + i * 12), p, 12); }
            if (r.below(10) == 0) { float* e = At<float>(0xC3F6E0 + r.below(32) * 12); e[0] = 0.0f; p[0] = -0.0f; }
            *At<int>(0xC3F0D4) = r.below(40) == 0 ? (int)r.below(40) - 4 : (int)r.below(32);
            gPvl.ret = r.below(4) != 0; gPvl.z = RndF(r, -50, 50, 120);
            Mem base; Save(base);
            Out o{}, d{}; Mem mo, md; std::vector<Event> to, td;
            u32 args[4] = {FloatToBits(p[0]), FloatToBits(p[1]), FloatToBits(p[2]), u32(uintptr_t(outZ))};
            Load(base); gTrace.clear(); outZ[0] = 7.0f; PrimeX87();
            if(getenv("TRACE")){fprintf(stderr,"  exe idx=%d ret=%d\n",*At<int>(0xC3F0D4),(int)gPvl.ret);fflush(stderr);} Gate(0x6FFFF0 + kShift, args, 4, o); Save(mo); to = gTrace; const u32 zo = FloatToBits(outZ[0]);
            Load(base); gTrace.clear(); outZ[0] = 7.0f; PrimeX87();
            if(getenv("TRACE")){fprintf(stderr,"  dll\n");fflush(stderr);} Gate(reinterpret_cast<uintptr_t>(&CPointLights::ProcessVerticalLineUsingCache), args, 4, d); Save(md); td = gTrace; const u32 zd = FloatToBits(outZ[0]);
            if (zo != zd) md.b[1] ^= 1;
            compare("cache", c, st, o, d, mo, md, to, td, true, 0xFF);
        }
        report("ProcessVerticalLineUsingCache", st);
    }
    return 0;
}
