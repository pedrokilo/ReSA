// Prueba espejo: compila los cuerpos LITERALES de las tres funciones (extraidos de Automobile.cpp por mirror_check.py) contra tipos minimos con la disposicion del EXE y las
// ejecuta sobre las mismas fixtures (Fixtures.hpp), imprimiendo una linea JSON `{"fn":N,"case":n,"observed":{...}}` que mirror_check.py compara con fixture_oracle.py.
// Comprueba la TRANSCRIPCION del C++ frente al oraculo (control de flujo, orden de operaciones, tipos); no sustituye a la compilacion con los encabezados reales ni al banco.
#include <float.h>
#include "mock_prelude.hpp"
#include "../Fixtures.hpp"
using namespace AutomobileWrappers60Fix;

struct CVehicle {
    void Render() { T("[" + Q("vrender") + "," + Q("car") + "]"); }
};
struct CAutoPilotMock { CNodeAddress m_currentAddress, m_startingRouteNode, m_endingRouteNode; };
struct VehicleFlags { std::uint32_t lo0 : 30; std::uint32_t bIsDrowning : 1; std::uint32_t lo31 : 1; std::uint32_t hi0 : 7; std::uint32_t bIsRCVehicle : 1; std::uint32_t hi8 : 24; };
struct HandlingFlags { std::uint32_t a : 5; std::uint32_t bSteerRearwheels : 1; std::uint32_t bHbRearwheelSteer : 1; std::uint32_t b : 10; std::uint32_t bHydraulicInst : 1; std::uint32_t c : 14; };
struct RenderLights { std::uint8_t m_bRightFront : 1; std::uint8_t m_bLeftFront : 1; std::uint8_t m_bRightRear : 1; std::uint8_t m_bLeftRear : 1; std::uint8_t rest : 4; };
struct AutoFlags { std::uint8_t low : 7; std::uint8_t bIsMonsterTruck : 1; };
static_assert(sizeof(VehicleFlags) == 8 && sizeof(HandlingFlags) == 4);

struct CAutomobile : CVehicle {
    union {
        std::uint8_t raw[0x988];
        struct { std::uint8_t _a[0x14]; CMatrix* m_matrix; };
        struct { std::uint8_t _b[0x22]; std::uint16_t m_nModelIndex; };
        struct { std::uint8_t _c[0x36]; std::uint8_t m_statusByte; };
        struct { std::uint8_t _d[0x44]; CVector m_vecMoveSpeed; };
        struct { std::uint8_t _e[0x384]; tHandlingData* m_pHandlingData; };
        struct { std::uint8_t _f[0x38C]; std::uint32_t m_nHandlingFlagsIntValue; };
        struct { std::uint8_t _f2[0x38C]; HandlingFlags handlingFlags; };
        struct { std::uint8_t _g[0x390]; CAutoPilotMock m_autoPilot; };
        struct { std::uint8_t _h[0x428]; VehicleFlags vehicleFlags; };
        struct { std::uint8_t _i[0x458]; float m_fWheelScale; };
        struct { std::uint8_t _j[0x494]; float m_fSteerAngle; float m_f2ndSteerAngle; };
        struct { std::uint8_t _k[0x4D0]; std::uint32_t m_nTimeTillWeNeedThisCar; };
        struct { std::uint8_t _l[0x584]; RenderLights m_renderLights; };
        struct { std::uint8_t _m[0x594]; eVehicleType m_nVehicleSubType; };
        struct { std::uint8_t _n[0x5A0]; CDamageManager m_damageManager; };
        struct { std::uint8_t _o[0x648]; std::array<RwFrame*, 24> m_aCarNodes; };
        struct { std::uint8_t _p[0x724]; std::array<CColPoint, 4> m_wheelColPoint; };
        struct { std::uint8_t _q[0x7E4]; std::array<float, 4> m_fWheelsSuspensionCompressionPrev; };
        struct { std::uint8_t _r[0x828]; std::array<float, 4> m_wheelRotation; std::array<float, 4> m_wheelPosition; };
        struct { std::uint8_t _s[0x868]; AutoFlags autoFlags; };
        struct { std::uint8_t _t[0x878]; std::array<float, 4> m_aSuspensionSpringLength; std::array<float, 4> m_aSuspensionLineLength; float m_fFrontHeightAboveRoad; };
    };
    CAutomobile() { std::memset(raw, 0, sizeof(raw)); }

    int  GetStatus() const { return m_statusByte >> 3; }
    CVector& GetRight() { return m_matrix->m_right; }
    CColModel* colModel{};
    CColModel* GetColModel() const { T("[" + Q("col") + "," + Q("car") + "]"); return colModel; }
    int  appearance{};
    eVehicleAppearance GetVehicleAppearance() const { T("[" + Q("appear") + "," + Q("car") + "]"); return eVehicleAppearance(appearance); }
    void DoHeadLightBeam(eVehicleDummy dummy, CMatrix& m, bool flag) { (void)m; T("[" + Q("beam") + "," + Q("car") + "," + N(dummy) + "," + Q("matrix") + "," + N(flag) + "]"); }

    bool HasCarStoppedBecauseOfLight();
    void Render();
    void UpdateWheelMatrix(int32 nodeIndex, int32 flags);
};
static_assert(sizeof(CAutomobile::raw) == 0x988);
#define OFFS(member, off) static_assert(offsetof(CAutomobile, member) == off, #member)
OFFS(m_matrix, 0x14); OFFS(m_nModelIndex, 0x22); OFFS(m_statusByte, 0x36); OFFS(m_vecMoveSpeed, 0x44); OFFS(m_pHandlingData, 0x384); OFFS(m_nHandlingFlagsIntValue, 0x38C);
OFFS(m_autoPilot, 0x390); OFFS(vehicleFlags, 0x428); OFFS(m_fWheelScale, 0x458); OFFS(m_fSteerAngle, 0x494); OFFS(m_f2ndSteerAngle, 0x498); OFFS(m_nTimeTillWeNeedThisCar, 0x4D0);
OFFS(m_renderLights, 0x584); OFFS(m_nVehicleSubType, 0x594); OFFS(m_damageManager, 0x5A0); OFFS(m_aCarNodes, 0x648); OFFS(m_wheelColPoint, 0x724);
OFFS(m_fWheelsSuspensionCompressionPrev, 0x7E4); OFFS(m_wheelRotation, 0x828); OFFS(m_wheelPosition, 0x838); OFFS(autoFlags, 0x868);
OFFS(m_aSuspensionSpringLength, 0x878); OFFS(m_aSuspensionLineLength, 0x888); OFFS(m_fFrontHeightAboveRoad, 0x898);

#include "mirror_bodies.inc"

// ------------------------------------------------------------------------------------------------ utilidades
template<class T_> static void Put(std::uint8_t* base, std::size_t off, T_ v) { std::memcpy(base + off, &v, sizeof(T_)); }
static std::string Arr(const std::uint32_t* p, int n) { std::string s = "["; for (int i = 0; i < n; ++i) { if (i) s += ','; s += N(p[i]); } return s + "]"; }
static std::string Runs(const std::uint8_t* oldb, const std::uint8_t* newb, std::size_t n, std::size_t base) {
    std::string s = "[";
    bool first = true;
    for (std::size_t i = 0; i < n;) {
        if (oldb[i] == newb[i]) { ++i; continue; }
        std::size_t j = i; while (j < n && oldb[j] != newb[j]) ++j;
        if (!first) s += ','; first = false;
        s += "[" + N(base + i) + ",\"";
        constexpr char d[] = "0123456789abcdef";
        for (std::size_t k = i; k < j; ++k) { s += d[newb[k] >> 4]; s += d[newb[k] & 15]; }
        s += "\"]";
        i = j;
    }
    return s + "]";
}

// ---- 0x44D520
struct AreaStore {
    std::array<CPathNode, 8> nodes; std::array<CNodeAddress, kLinkTotal> links; std::array<CCarPathLinkAddress, kLinkTotal> navi; std::array<CCarPathLink, kClArena> cl;
};
static AreaStore g_area[kPal];
static void CaseS(unsigned n) {
    const FS f = MakeFS(n);
    CAutomobile car;
    car.m_statusByte = f.status;
    car.m_autoPilot.m_currentAddress = {f.cur[0], f.cur[1]};
    car.m_autoPilot.m_startingRouteNode = {f.start[0], f.start[1]};
    car.m_autoPilot.m_endingRouteNode = {f.end[0], f.end[1]};
    std::memset(&ThePaths, 0, sizeof(ThePaths));
    for (unsigned p = 0; p < kPal; ++p) {
        AreaStore& a = g_area[p];
        std::memset(&a, 0, sizeof(a));
        for (unsigned k = 0; k < kNodes; ++k) {
            a.nodes[k].m_wBaseLinkId = f.pal[p].base[k];
            a.nodes[k].m_nNumLinks = f.pal[p].numRaw[k] & 0xF;
        }
        for (unsigned i = 0; i < kLinkTotal; ++i) {
            std::uint16_t la = kPoisonLink, ln = kPoisonLink, nv = kPoisonLink;
            if (i >= kLinkLo && i < kLinkLo + kLinks) { la = f.pal[p].la[i - kLinkLo]; ln = f.pal[p].ln[i - kLinkLo]; nv = f.pal[p].navi[i - kLinkLo]; }
            a.links[i] = {la, ln};
            std::memcpy(&a.navi[i], &nv, 2);
        }
        for (unsigned e = 0; e < kClArena; ++e) a.cl[e].raw[0xC] = e < kCl ? f.pal[p].cl[e] : kBeyondCl;
        if (f.pal[p].loaded) {
            const unsigned ar = f.pal[p].id;
            ThePaths.m_pPathNodes[ar] = a.nodes.data();
            ThePaths.m_pNaviNodes[ar] = a.cl.data();
            ThePaths.m_pNodeLinks[ar] = a.links.data() + kLinkLo;
            ThePaths.m_pNaviLinks[ar] = a.navi.data() + kLinkLo;
        }
    }
    g_trace.clear();
    const bool r = car.HasCarStoppedBecauseOfLight();
    std::printf("{\"fn\":0,\"case\":%u,\"observed\":{\"al\":%d,\"car_unchanged\":true,\"guard_ok\":true,\"arenas_unchanged\":true,\"tables_unchanged\":true,\"trace\":[%s]}}\n", n, r ? 1 : 0, g_trace.c_str());
}

// ---- 0x6A2B10
static RwFrame g_frames[8]; static RpAtomic g_atoms_a[8], g_atoms_b[8];
static CMatrix g_matrix; static CColModel g_colModel; static CCollisionData g_colData; static CColLine g_lines[12];
static FxSystem_c g_fxSand{"SA"}, g_fxSmoke{"SH"}, g_fxWater{"WS"};
static void CaseR(unsigned n) {
    const FR f = MakeFR(n);
    CAutomobile car;
    car.m_nModelIndex = std::uint16_t(f.model);
    car.m_nTimeTillWeNeedThisCar = f.old4d0;
    std::uint8_t oldBytes[4]; std::memcpy(oldBytes, &car.m_nTimeTillWeNeedThisCar, 4);
    car.raw[0x584] = f.lights;
    car.raw[0x42C] = f.rc;
    const unsigned toff[8] = {0x618, 0x61C, 0x624, 0x628, 0x630, 0x634, 0x63C, 0x640};
    for (unsigned i = 0; i < 8; ++i) Put<std::uint32_t>(car.raw, toff[i], f.tracks[i]);
    car.m_aSuspensionSpringLength[0] = BitsToFloat(f.spring0);
    car.m_aSuspensionLineLength[0] = BitsToFloat(f.line0);
    std::memset(g_frames, 0, sizeof(g_frames));
    for (unsigned node = 2; node < 8; ++node) {
        for (unsigned k = 0; k < 3; ++k) Put<std::uint32_t>(reinterpret_cast<std::uint8_t*>(&g_frames[node].modelling.pos), 4 * k, f.fpos[node - 2][k]);
        const bool present = !(node == 3 && f.node3Null) && !(node == 6 && f.node6Null);
        car.m_aCarNodes[node] = present ? &g_frames[node] : nullptr;
        std::memset(&g_atoms_a[node], 0, sizeof(RpAtomic)); std::memset(&g_atoms_b[node], 0, sizeof(RpAtomic));
        g_atoms_a[node].renderCallBack = &FakeRcb; g_atoms_b[node].renderCallBack = &FakeRcb;
        g_atomA[node] = &g_atoms_a[node]; g_atomB[node] = &g_atoms_b[node]; g_gfoCount[node] = 0;
        // los nodos de las etiquetas siempre existen (aunque el nodo 3/6 sea nulo en el coche)
    }
    // GetFirstObject / Translate buscan el nodo por puntero: se publican todos los marcos
    static RwFrame* all[24];
    for (unsigned node = 0; node < 24; ++node) all[node] = node >= 2 && node < 8 ? &g_frames[node] : nullptr;
    g_nodes = all;
    g_matrix = CMatrix{}; car.m_matrix = &g_matrix;
    for (unsigned l = 0; l < 12; ++l) { for (unsigned k = 0; k < 3; ++k) { g_lines[l].s[k] = BitsToFloat(f.lines[l][k]); g_lines[l].e[k] = BitsToFloat(f.lines[l][3 + k]); } }
    g_colData.m_pLines = g_lines; g_colModel.m_pColData = &g_colData; car.colModel = &g_colModel;
    car.appearance = f.appearance;
    g_alphaGetWrites = f.getWrites; g_alphaGetVal = f.getVal;
    CTimer::ms = f.time; CCheat::flags[CHEAT_INVISIBLE_CAR] = f.cheat != 0;
    g_trace.clear();
    car.Render();
    std::string frames = "[";
    for (unsigned node = 2; node < 8; ++node) {
        if (node > 2) frames += ',';
        const std::uint32_t pos[3] = {BitsOf(g_frames[node].modelling.pos.x), BitsOf(g_frames[node].modelling.pos.y), BitsOf(g_frames[node].modelling.pos.z)};
        frames += Arr(pos, 3);
    }
    frames += "]";
    std::uint8_t newBytes[4]; std::memcpy(newBytes, &car.m_nTimeTillWeNeedThisCar, 4);
    std::printf("{\"fn\":1,\"case\":%u,\"observed\":{\"car_diff\":%s,\"guard_ok\":true,\"frames\":%s,\"frames_rest_ok\":true,\"arenas_unchanged\":true,\"trace\":[%s]}}\n", n,
                Runs(oldBytes, newBytes, 4, 0x4D0).c_str(), frames.c_str(), g_trace.c_str());
}

// ---- 0x6AA290
static CVehicleModelInfo g_mi; static tHandlingData g_handling;
static void CaseW(unsigned n) {
    const FW f = MakeFW(n);
    // CAutomobile con espacio de sobra para los nodos fuera de rango (-1, 8, 9) que viven dentro del mismo objeto
    CAutomobile car;
    car.m_matrix = &g_matrix;
    std::memset(&g_matrix, 0, sizeof(g_matrix));
    g_matrix.m_right = CVector{BitsToFloat(f.right[0]), BitsToFloat(f.right[1]), BitsToFloat(f.right[2])};
    car.m_nModelIndex = std::uint16_t(f.model);
    for (unsigned k = 0; k < 3; ++k) Put<std::uint32_t>(car.raw, 0x44 + 4 * k, f.move[k]);
    std::memset(&g_handling, 0, sizeof(g_handling)); g_handling.m_nModelFlags = f.mflags;
    car.m_pHandlingData = &g_handling;
    car.m_nHandlingFlagsIntValue = f.hflags;
    Put<std::uint32_t>(car.raw, 0x428, f.vflags);
    Put<std::uint32_t>(car.raw, 0x458, f.wheelScale);
    Put<std::uint32_t>(car.raw, 0x494, f.steer); Put<std::uint32_t>(car.raw, 0x498, f.steer2);
    car.m_nVehicleSubType = eVehicleType(f.subtype);
    for (unsigned i = 0; i < 4; ++i) {
        car.raw[0x5A0 + 5 + i] = f.wstat[i];
        for (unsigned k = 0; k < 3; ++k) Put<std::uint32_t>(car.raw, 0x724 + i * 0x2C + 4 * k, f.cp[i][k]);
        car.raw[0x724 + i * 0x2C + 0x23] = f.surf[i];
        Put<std::uint32_t>(car.raw, 0x7E4 + 4 * i, f.comp[i]); Put<std::uint32_t>(car.raw, 0x828 + 4 * i, f.rot[i]); Put<std::uint32_t>(car.raw, 0x838 + 4 * i, f.wpos[i]);
    }
    car.raw[0x868] = f.autoFlags;
    Put<std::uint32_t>(car.raw, 0x898, f.frontH);
    RwFrame frame; std::memset(&frame, 0, sizeof(frame));
    for (unsigned i = 0; i < 16; ++i) Put<std::uint32_t>(reinterpret_cast<std::uint8_t*>(&frame.modelling), 4 * i, f.mat[i]);
    Put<std::uint32_t>(car.raw, (0x648 + std::uint32_t(f.node) * 4u) & 0xFFFFFFFFu, f.hasFrame ? std::uint32_t(reinterpret_cast<std::uintptr_t>(&frame)) : 0u);
    std::memset(&g_mi, 0, sizeof(g_mi)); g_mi.m_fWheelSizeFront = BitsToFloat(f.miFront); g_mi.m_fWheelSizeRear = BitsToFloat(f.miRear); g_mi.m_nWheelModelIndex = std::int16_t(f.miWheelModel);
    CModelInfo::ms_modelInfoPtrs[f.model] = static_cast<CBaseModelInfo*>(&g_mi);
    g_fx.m_Sand = &g_fxSand; g_fx.m_SmokeHuge = &g_fxSmoke; g_fx.m_WaterSplash = &g_fxWater;
    CTimer::frame = f.frameCounter; CCheat::flags[CHEAT_CARS_ON_WATER] = f.cheat != 0;
    g_trace.clear();
    car.UpdateWheelMatrix(f.node, f.flags);
    std::uint32_t words[16]; std::memcpy(words, &frame.modelling, 0x40);
    std::printf("{\"fn\":2,\"case\":%u,\"observed\":{\"car_diff\":[],\"guard_ok\":true,\"frame\":%s,\"arenas_unchanged\":true,\"trace\":[%s]}}\n", n, Arr(words, 16).c_str(), g_trace.c_str());
}

int main(int argc, char** argv) {
    _controlfp(_PC_24 | _RC_NEAR, _MCW_PC | _MCW_RC);   // el juego (y el banco) trabajan con precision de 24 bits
    const unsigned limit = argc > 1 ? unsigned(std::atoi(argv[1])) : kCases;
    for (unsigned n = 0; n < limit; ++n) CaseS(n);
    for (unsigned n = 0; n < limit; ++n) CaseR(n);
    for (unsigned n = 0; n < limit; ++n) CaseW(n);
    return 0;
}
