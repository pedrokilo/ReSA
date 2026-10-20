// Prelude de la prueba espejo (mirror_check.py): tipos minimos del juego con la disposicion de memoria del EXE (static_assert(offsetof)) y los colaboradores sustituidos por
// los mismos falsos que Core.hpp (misma traza). El cuerpo de las tres funciones se extrae LITERALMENTE de source/game_sa/Entity/Vehicle/Automobile.cpp.
#pragma once
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "X87Math.h"
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using uint64 = std::uint64_t;
using int8 = std::int8_t; using int16 = std::int16_t; using int32 = std::int32_t; using int64 = std::int64_t;

inline std::string g_trace;     // elementos JSON de la traza, separados por comas
inline void T(const std::string& element) { if (!g_trace.empty()) g_trace += ','; g_trace += element; }
inline std::string N(std::uint64_t v) { return std::to_string(v); }
inline std::string Q(const char* s) { return std::string("\"") + s + "\""; }
inline std::uint32_t BitsOf(float f) { std::uint32_t b; std::memcpy(&b, &f, 4); return b; }

struct CVector { float x, y, z; CVector() = default; CVector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {} };
struct RwV3d { float x, y, z; };
struct RwMatrix { RwV3d right; std::uint32_t flags; RwV3d up; std::uint32_t pad1; RwV3d at; std::uint32_t pad2; RwV3d pos; std::uint32_t pad3; };
static_assert(sizeof(RwMatrix) == 0x40);
struct RwFrame { std::uint8_t obj[0x10]; RwMatrix modelling; std::uint8_t rest[0x70]; };
static_assert(offsetof(RwFrame, modelling) == 0x10 && offsetof(RwMatrix, pos) == 0x30);
struct RpAtomic;
using RpAtomicCallBackRender = RpAtomic* (*)(RpAtomic*);
struct RpAtomic { std::uint8_t pad[0x48]; RpAtomicCallBackRender renderCallBack; };
struct RwObject { std::uint8_t pad[8]; };
enum RwOpCombineType { rwCOMBINEREPLACE = 0, rwCOMBINEPRECONCAT, rwCOMBINEPOSTCONCAT };
enum { rwRENDERSTATEALPHATESTFUNCTIONREF = 30 };

// ---- ThePaths
struct CNodeAddress { std::uint16_t m_wAreaId{0xFFFF}, m_wNodeId{0xFFFF}; };
struct CCarPathLinkAddress { std::uint16_t m_wCarPathLinkId : 10; std::uint16_t m_wAreaId : 6; };
struct CCarPathLink { std::uint8_t raw[0xE]; };
struct CPathNode { std::uint8_t _0[0x10]; std::int16_t m_wBaseLinkId; std::uint8_t _1[6]; std::uint32_t m_nNumLinks : 4; std::uint32_t _rest : 28; };
static_assert(sizeof(CPathNode) == 0x1C && offsetof(CPathNode, m_wBaseLinkId) == 0x10 && sizeof(CCarPathLink) == 0xE);
struct CPathFind { CPathNode* m_pPathNodes[72]; CCarPathLink* m_pNaviNodes[72]; CNodeAddress* m_pNodeLinks[72]; CCarPathLinkAddress* m_pNaviLinks[64]; };
inline CPathFind ThePaths;

enum eEntityStatus { STATUS_PLAYER = 0, STATUS_PLAYER_PLAYBACKFROMBUFFER, STATUS_SIMPLE, STATUS_PHYSICS };
enum eCarNodes { CAR_WHEEL_RF = 2, CAR_WHEEL_RM = 3, CAR_WHEEL_RB = 4, CAR_WHEEL_LF = 5, CAR_WHEEL_LM = 6, CAR_WHEEL_LB = 7 };
enum eCarWheel { CAR_WHEEL_FRONT_LEFT = 0 };
enum eVehicleType { VEHICLE_TYPE_AUTOMOBILE = 0, VEHICLE_TYPE_MTRUCK = 1, VEHICLE_TYPE_QUAD = 2, VEHICLE_TYPE_HELI = 3 };
enum eVehicleAppearance { VEHICLE_APPEARANCE_NONE = 0, VEHICLE_APPEARANCE_AUTOMOBILE, VEHICLE_APPEARANCE_BIKE, VEHICLE_APPEARANCE_HELI };
enum eVehicleDummy { DUMMY_LIGHT_FRONT_MAIN = 0 };
enum { MODEL_RHINO = 432, MODEL_RCBANDIT = 441, MODEL_COMBINE = 532, MODEL_VORTEX = 539, MODEL_KART = 571 };
enum eCheats { CHEAT_INVISIBLE_CAR = 27, CHEAT_CARS_ON_WATER = 34 };
struct CCheat { static inline bool flags[64]{}; static bool IsActive(int c) { return flags[c]; } };
struct CTimer { static inline std::uint32_t ms{}, frame{}; static std::uint32_t GetTimeInMS() { return ms; } static std::uint32_t GetFrameCounter() { return frame; } };

struct CColPoint { CVector m_vecPoint; float field_C; CVector m_vecNormal; float field_1C; std::uint8_t m_nSurfaceTypeA, m_nPieceTypeA, m_nLightingA, m_nSurfaceTypeB, m_nPieceTypeB, m_nLightingB, _align[2]; float m_fDepth; };
static_assert(sizeof(CColPoint) == 0x2C && offsetof(CColPoint, m_nSurfaceTypeB) == 0x23);
struct CColLine { float s[4], e[4]; };
struct CCollisionData { std::uint8_t pad[0x10]; CColLine* m_pLines; };
struct CColModel { std::uint8_t pad[0x2C]; CCollisionData* m_pColData; };
struct CBaseModelInfo {};
struct CVehicleModelInfo : CBaseModelInfo {
    std::uint8_t pad[0x40]; float m_fWheelSizeFront; float m_fWheelSizeRear; std::int16_t m_nWheelModelIndex;
    float GetWheelSize(bool front) { return front ? m_fWheelSizeFront : m_fWheelSizeRear; }
};
static_assert(offsetof(CVehicleModelInfo, m_fWheelSizeFront) == 0x40 && offsetof(CVehicleModelInfo, m_nWheelModelIndex) == 0x48);
struct CModelInfo { static inline CBaseModelInfo* ms_modelInfoPtrs[20000]; };
struct tHandlingData { std::uint8_t pad[0xCC]; std::uint32_t m_nModelFlags; };

// ---- CMatrix: falsos de Core.hpp (mismas trazas y el mismo efecto sobre la matriz)
struct CMatrix {
    CVector m_right; float pad0; CVector m_forward; float pad1; CVector m_up; float pad2; CVector m_pos; float pad3; RwMatrix* m_pAttachMatrix{}; bool m_bOwnsAttachedMatrix{};
    CVector& GetPosition() { return m_pos; }
    void Attach(RwMatrix* rw, bool own) {
        T("[" + Q("attach") + "," + Q("L") + "," + Q("FM") + "," + N(own) + "]");
        std::memcpy(this, rw, 0x40); m_pAttachMatrix = rw; m_bOwnsAttachedMatrix = own;
    }
    void SetScale(float x, float y, float z) {
        T("[" + Q("scale") + "," + N(BitsOf(x)) + "," + N(BitsOf(y)) + "," + N(BitsOf(z)) + "]");
        float* m = reinterpret_cast<float*>(this);
        m[0] = x; m[1] = 0; m[2] = 0; m[4] = 0; m[5] = y; m[6] = 0; m[8] = 0; m[9] = 0; m[10] = z; m[12] = 0; m[13] = 0; m[14] = 0;
    }
    void Rotate(CVector r) { T("[" + Q("rot") + "," + N(BitsOf(r.x)) + "," + N(BitsOf(r.y)) + "," + N(BitsOf(r.z)) + "]"); }
    void RotateY(float a) { T("[" + Q("roty") + "," + N(BitsOf(a)) + "]"); }
    void UpdateRW() {
        T("[" + Q("upd") + "," + Q("L") + "]");
        if (m_pAttachMatrix) { const float* m = reinterpret_cast<const float*>(this); float* rw = reinterpret_cast<float*>(m_pAttachMatrix); for (int w : {0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14}) rw[w] = m[w]; }
    }
};
static_assert(offsetof(CMatrix, m_pAttachMatrix) == 0x40);

struct FxPrtMult_c { float r, g, b, a, size, rot, life; FxPrtMult_c(float r_, float g_, float b_, float a_, float s_, float rot_, float l_) : r(r_), g(g_), b(b_), a(a_), size(s_), rot(rot_), life(l_) {} };
struct FxSystem_c {
    const char* label;
    void AddParticle(const CVector& pos, const CVector& vel, float ts, const FxPrtMult_c& m, float rotZ, float lm, float lml, bool cl) {
        std::string e = "[" + Q("addp") + "," + Q(label);
        for (float v : {pos.x, pos.y, pos.z, vel.x, vel.y, vel.z, ts, m.r, m.g, m.b, m.a, m.size, m.rot, m.life, rotZ, lm, lml}) e += "," + N(BitsOf(v));
        T(e + "," + N(cl) + "]");
    }
};
struct Fx_c { FxSystem_c *m_Sand, *m_SmokeHuge, *m_WaterSplash; };
inline Fx_c g_fx;
struct SurfaceInfos_c {
    bool IsWater(std::uint32_t id) { T("[" + Q("iswater") + "," + N(id) + "]"); return (id & 3) == 1; }
    bool IsSand(std::uint32_t id) { T("[" + Q("issand") + "," + N(id) + "]"); return (id & 3) == 2; }
};
inline SurfaceInfos_c g_surfaceInfos;
struct CDamageManager {
    std::uint8_t pad[5]; std::uint8_t wheels[4]; std::uint8_t pad2[0x18 - 9];
    int GetWheelStatus(eCarWheel w) const { return wheels[int(w)]; }
};
static_assert(sizeof(CDamageManager) == 0x18);

// ---- Render: colaboradores
inline int g_alphaGetWrites; inline std::int32_t g_alphaGetVal;
inline int RwRenderStateGet(int state, void* out) { T("[" + Q("sget") + "," + N(state) + "]"); if (g_alphaGetWrites) *static_cast<std::int32_t*>(out) = g_alphaGetVal; return 1; }
inline int RwRenderStateSet(int state, void* value) { T("[" + Q("sset") + "," + N(state) + "," + N(std::uint32_t(reinterpret_cast<std::uintptr_t>(value))) + "]"); return 1; }
#define RWRSTATE(a) (reinterpret_cast<void*>(std::uintptr_t(a)))
inline RwFrame** g_nodes; inline RpAtomic* g_atomA[8]; inline RpAtomic* g_atomB[8]; inline unsigned g_gfoCount[8];
inline RwObject* GetFirstObject(RwFrame* frame) {
    int node = -1; for (int n = 2; n < 8; ++n) if (frame == g_nodes[n]) node = n;
    const bool first = (g_gfoCount[node]++ % 2) == 0;
    T("[" + Q("gfo") + "," + Q(("F" + std::to_string(node)).c_str()) + "," + Q(first ? "A" : "B") + "]");
    return reinterpret_cast<RwObject*>(first ? g_atomA[node] : g_atomB[node]);
}
inline RwMatrix* RwMatrixTranslate(RwMatrix* m, const RwV3d* v, RwOpCombineType op) {
    int node = -1; for (int n = 2; n < 8; ++n) if (m == &g_nodes[n]->modelling) node = n;
    T("[" + Q("xlate") + "," + Q(("M" + std::to_string(node)).c_str()) + "," + N(BitsOf(v->x)) + "," + N(BitsOf(v->y)) + "," + N(BitsOf(v->z)) + "," + N(op) + "]");
    m->pos.x = m->pos.x + v->x; m->pos.y = m->pos.y + v->y; m->pos.z = m->pos.z + v->z;
    return m;
}
inline RwFrame* RwFrameUpdateObjects(RwFrame* f) {
    int node = -1; for (int n = 2; n < 8; ++n) if (f == g_nodes[n]) node = n;
    T("[" + Q("upd") + "," + Q(("F" + std::to_string(node)).c_str()) + "]"); return f;
}
inline RpAtomic* FakeRcb(RpAtomic* a) {
    std::string l = "?"; for (int n = 2; n < 8; ++n) { if (a == g_atomA[n]) l = "A" + std::to_string(n); if (a == g_atomB[n]) l = "B" + std::to_string(n); }
    T("[" + Q("rcb") + "," + Q(l.c_str()) + "]"); return a;
}
inline int g_appearance;
