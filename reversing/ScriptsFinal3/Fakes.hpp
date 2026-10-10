#pragma once
// Objetos falsos del banco ScriptsFinal3: pools de peds/edificios/objetos/dummies (el puntero real se intercambia y se restaura), peds, vehiculos,
// objetos, matrices, marcos RW, tareas, eventos, model info con tabla virtual falsa, motor RW falso y bloques de checkpoint.
// Todos los identificadores que salen en las trazas son indices en estos arrays (los punteros cambian de una ejecucion a otra solo si el contenido difiere).
#include "Common.hpp"

namespace ScriptsFinal3Live {

// Disposicion de CPool<T> (0x14 bytes): almacenamiento, estados, capacidad, ultima ranura libre, banderas
struct FakePool { uint8* storage; uint8* states; uint32 cap; int32 lastFree; uint8 owns; uint8 deal; uint16 pad; };
static_assert(sizeof(FakePool) == 0x14);

constexpr uint32 N_PED = 16, PED_STRIDE = 0x7C4, N_BLD = 24, BLD_STRIDE = 0x38, N_OBJ = 24, OBJ_STRIDE = 0x19C, N_DUM = 24, DUM_STRIDE = 0x38;
constexpr uint32 N_VEH = 8, VEH_STRIDE = 0xB00, N_ENT = 16, ENT_STRIDE = 0x100, N_MAT = 64, MAT_STRIDE = 0x60;
constexpr uint32 N_TASK = 8, TASK_STRIDE = 0x80, N_EVENT = 4, EVENT_STRIDE = 0x40, N_GROUP = 4, GROUP_STRIDE = 0x80, N_INTEL = 16, INTEL_STRIDE = 0x200;

alignas(16) inline std::array<uint8, N_PED * PED_STRIDE> pedStore{};   inline std::array<uint8, N_PED> pedStates{};
alignas(16) inline std::array<uint8, N_BLD * BLD_STRIDE> bldStore{};   inline std::array<uint8, N_BLD> bldStates{};
alignas(16) inline std::array<uint8, N_OBJ * OBJ_STRIDE> objStore{};   inline std::array<uint8, N_OBJ> objStates{};
alignas(16) inline std::array<uint8, N_DUM * DUM_STRIDE> dumStore{};   inline std::array<uint8, N_DUM> dumStates{};
alignas(16) inline std::array<uint8, N_VEH * VEH_STRIDE> vehStore{};
alignas(16) inline std::array<uint8, N_ENT * ENT_STRIDE> entStore{};
alignas(16) inline std::array<uint8, N_MAT * MAT_STRIDE> matStore{};
alignas(16) inline std::array<uint8, N_TASK * TASK_STRIDE> taskStore{};
alignas(16) inline std::array<uint8, N_EVENT * EVENT_STRIDE> eventStore{};
alignas(16) inline std::array<uint8, N_GROUP * GROUP_STRIDE> groupStore{};
alignas(16) inline std::array<uint8, N_INTEL * INTEL_STRIDE> intelStore{};
alignas(16) inline std::array<uint8, 0x100> rwObjStore{};     // RwObject falso: padre (marco) en +4
alignas(16) inline std::array<uint8, 0x100> rwFrameStore{};   // RwFrame falso: matriz de modelado en +0x10
alignas(16) inline std::array<uint8, 0x80> colModelStore{};   // CColModel falso: radio en +0x24, datos en +0x2C
alignas(16) inline std::array<uint8, 0x40> colDataStore{};    // CCollisionData falso: m_nNumLines en +6
alignas(16) inline std::array<uint8, 0x80> modelInfoStore{};  // CBaseModelInfo falso: tabla virtual en +0, modelo de colision en +0x14
alignas(16) inline std::array<uint8, 0x40> checkpointStore{}; // CCheckpoint falso (tipo +0, id +4)
inline FakePool pedPool{}, bldPool{}, objPool{}, dumPool{}, txdPool{};
alignas(16) inline std::array<uint8, 12 * 8> txdStore{};      inline std::array<uint8, 8> txdStates{};
alignas(16) inline std::array<uint8, 0x40> txdDictStore{};

inline std::array<void*, 32> pedVt{}, vehVt{}, taskVt{}, miVt{};

struct EntRange { uintptr_t base; uint32 stride; uint32 count; uint32 kind; };
inline std::array<EntRange, 12> entRanges{};
inline void BuildRanges() {
    entRanges = {{
        { uintptr_t(pedStore.data()), PED_STRIDE, N_PED, 1 }, { uintptr_t(vehStore.data()), VEH_STRIDE, N_VEH, 2 },
        { uintptr_t(objStore.data()), OBJ_STRIDE, N_OBJ, 3 }, { uintptr_t(bldStore.data()), BLD_STRIDE, N_BLD, 4 },
        { uintptr_t(dumStore.data()), DUM_STRIDE, N_DUM, 5 }, { uintptr_t(entStore.data()), ENT_STRIDE, N_ENT, 6 },
        { uintptr_t(taskStore.data()), TASK_STRIDE, N_TASK, 7 }, { uintptr_t(eventStore.data()), EVENT_STRIDE, N_EVENT, 8 },
        { uintptr_t(groupStore.data()), GROUP_STRIDE, N_GROUP, 9 }, { uintptr_t(intelStore.data()), INTEL_STRIDE, N_INTEL, 10 },
        { uintptr_t(matStore.data()), MAT_STRIDE, N_MAT, 11 }, { uintptr_t(checkpointStore.data()), 0x40, 1, 12 },
    }};
}
// Identificador estable de un objeto falso: kind<<8 | indice; 0xFFFF = nulo; 0xFFFE = desconocido
inline uint32 EntId(const void* e) {
    if (!e) return 0xFFFFu;
    const auto a = uintptr_t(e);
    for (auto& r : entRanges) if (a >= r.base && a < r.base + uintptr_t(r.stride) * r.count) return (r.kind << 8) | uint32((a - r.base) / r.stride);
    return 0xFFFEu;
}
// Posicion de una referencia (CEntity**) dentro de las matrices conocidas del estado: foco (+offset) o entrada de espera de cerebro de script
inline uint32 RefLoc(const void* p) {
    const auto a = uintptr_t(p);
    if (a >= SEARCHLIGHTS && a < SEARCHLIGHTS + 8 * 0x7C) return 0x10000u + uint32(a - SEARCHLIGHTS);
    if (a >= WAITING && a < WAITING + 150 * 8) return 0x20000u + uint32(a - WAITING);
    const auto vb = uintptr_t(vehStore.data());
    if (a >= vb && a < vb + sizeof(vehStore)) return 0x30000u + uint32(a - vb);
    return 0xFFFEu;
}

inline void SetupPool(FakePool& p, uint8* store, uint8* states, uint32 cap, uint32& s, uint32 usedPct) {
    p = { store, states, cap, int32(cap) - 1, 0, 0, 0 };
    for (uint32 i = 0; i < cap; ++i) states[i] = (Rnd(s) % 100 < usedPct) ? uint8(Rnd(s) & 0x7F) : uint8(0x80 | (Rnd(s) & 0x7F));
}
// Instala los pools falsos (peds, edificios, objetos, dummies) con ocupacion aleatoria
inline void InstallPools(uint32& s, uint32 usedPct = 70) {
    SetupPool(pedPool, pedStore.data(), pedStates.data(), N_PED, s, usedPct);
    SetupPool(bldPool, bldStore.data(), bldStates.data(), N_BLD, s, usedPct);
    SetupPool(objPool, objStore.data(), objStates.data(), N_OBJ, s, usedPct);
    SetupPool(dumPool, dumStore.data(), dumStates.data(), N_DUM, s, usedPct);
    Mem<uint32>(POOLS_PTR + 0) = P(&pedPool); Mem<uint32>(POOLS_PTR + 8) = P(&bldPool);
    Mem<uint32>(POOLS_PTR + 12) = P(&objPool); Mem<uint32>(POOLS_PTR + 16) = P(&dumPool);
}
// Referencia del pool (indice << 8 | estado) de la ranura i de cada pool
inline int32 PoolRef(const FakePool& p, uint32 i) { return int32((i << 8) | p.states[i]); }

// Entidad falsa: tipo en el byte +0x36 (3 bits bajos), modelo en +0x22, matriz en +0x14 (nula si m = nullptr)
inline void EntInit(uint8* e, size_t size, uint32 type, int32 model, uint8* mat) {
    std::memset(e, 0, size);
    At<uint8>(e, 0x36) = uint8(type & 7);
    At<int16>(e, 0x22) = int16(model);
    At<uint32>(e, 0x14) = P(mat);
}
inline uint8* Mat(uint32 i) { return matStore.data() + (i % N_MAT) * MAT_STRIDE; }
inline void MatClear(uint8* m) { std::memset(m, 0, MAT_STRIDE); }
inline uint8* PedAt(uint32 i) { return pedStore.data() + (i % N_PED) * PED_STRIDE; }
inline uint8* VehAt(uint32 i) { return vehStore.data() + (i % N_VEH) * VEH_STRIDE; }
inline uint8* ObjAt(uint32 i) { return objStore.data() + (i % N_OBJ) * OBJ_STRIDE; }
inline uint8* BldAt(uint32 i) { return bldStore.data() + (i % N_BLD) * BLD_STRIDE; }
inline uint8* DumAt(uint32 i) { return dumStore.data() + (i % N_DUM) * DUM_STRIDE; }
inline uint8* EntAt(uint32 i) { return entStore.data() + (i % N_ENT) * ENT_STRIDE; }
inline uint8* TaskAt(uint32 i) { return taskStore.data() + (i % N_TASK) * TASK_STRIDE; }
inline uint8* EventAt(uint32 i) { return eventStore.data() + (i % N_EVENT) * EVENT_STRIDE; }
inline uint8* GroupAt(uint32 i) { return groupStore.data() + (i % N_GROUP) * GROUP_STRIDE; }
inline uint8* IntelAt(uint32 i) { return intelStore.data() + (i % N_INTEL) * INTEL_STRIDE; }
inline void ClearAllFakes() {
    auto Z = [](auto& a) { std::memset(a.data(), 0, a.size()); };
    Z(pedStore); Z(vehStore); Z(objStore); Z(bldStore); Z(dumStore); Z(entStore); Z(matStore); Z(taskStore); Z(eventStore); Z(groupStore); Z(intelStore);
    rwObjStore.fill(0); rwFrameStore.fill(0); colModelStore.fill(0); colDataStore.fill(0); modelInfoStore.fill(0); checkpointStore.fill(0);
}
// Entidades ligeras con matriz, objeto RW y marco falsos (para ProcessAllSearchLights)
inline void MakeRwBacked(uint8* e) {
    At<uint32>(rwObjStore.data(), 4) = P(rwFrameStore.data());
    At<uint32>(e, 0x18) = P(rwObjStore.data());
}

// --- Tablas virtuales falsas ---
// CPed / CVehicle: la ranura 0 es el destructor con borrado (`push 1; call [vt]`, thiscall con 1 argumento: ret 4)
inline void* __fastcall SPedDtor(void* self, void*, uint32 flag) { if (Mine()) Push(T_PEDDTOR, { EntId(self), flag & 0xFF }); return self; }
inline void* __fastcall SVehDtor(void* self, void*, uint32 flag) { if (Mine()) Push(T_VEHDTOR, { EntId(self), flag & 0xFF }); return self; }
// CTask::GetTaskType (ranura 4, `call [edx+10h]`): el tipo se guarda en el byte +0x40 del bloque
inline uint32 __fastcall STaskType(void* self, void*) { if (!Mine()) return 0; const uint32 t = At<uint32>(static_cast<uint8*>(self), 0x40); Push(T_TASKTYPE, { EntId(self), t }); return t; }
// CBaseModelInfo (ranuras 12, 13, 14 = +0x30, +0x34, +0x38): SetAnimFile(const char*), ConvertAnimFileIndex(), GetAnimFileIndex()
inline void __fastcall SMiSetAnim(void* self, void*, const char* name) { if (Mine()) Push(T_ANIMSET, { TextHash(name) }); (void)self; }
inline void __fastcall SMiConvert(void*, void*) { if (Mine()) Push(T_ANIMCONVERT); }
inline int32 __fastcall SMiGetAnim(void* self, void*) { if (!Mine()) return 0; const int32 v = At<int32>(static_cast<uint8*>(self), 0x60); Push(T_ANIMGET, { uint32(v) }); return v; }
inline void BuildVtables() {
    pedVt.fill(nullptr); vehVt.fill(nullptr); taskVt.fill(nullptr); miVt.fill(nullptr);
    pedVt[0] = std::bit_cast<void*>(&SPedDtor); vehVt[0] = std::bit_cast<void*>(&SVehDtor);
    taskVt[4] = std::bit_cast<void*>(&STaskType);
    miVt[12] = std::bit_cast<void*>(&SMiSetAnim); miVt[13] = std::bit_cast<void*>(&SMiConvert); miVt[14] = std::bit_cast<void*>(&SMiGetAnim);
}

// --- Motor RW falso (el puntero 0xC97B24 se intercambia): estados de render y lineas 2D trazados ---
inline RwBool __cdecl SRwStateSet(RwRenderState state, void* value) { if (Mine()) Push(T_RWSTATE, { uint32(state), P(value) }); return TRUE; }
inline RwBool __cdecl SRwLine(RwIm2DVertex* v, RwInt32 n, RwInt32 a, RwInt32 b) {
    if (Mine()) Push(T_RWLINE, { uint32(n), uint32(a), uint32(b), Bits(v[0].x), Bits(v[0].y), Bits(v[1].x), Bits(v[1].y), v[0].emissiveColor, v[1].emissiveColor });
    return TRUE;
}
inline RwGlobals& FakeGlobals() { static RwGlobals g{}; return g; }
inline void InstallFakeEngine() {
    auto& g = FakeGlobals(); g.dOpenDevice.fpRenderStateSet = SRwStateSet; g.dOpenDevice.fpIm2DRenderLine = SRwLine;
    Mem<uint32>(0xC97B24) = P(&g);
}

// Modelo falso: la ranura FAKE_MODEL de ModelInfo apunta a un CBaseModelInfo falso con modelo de colision (radio +0x24, datos +0x2C)
inline void InstallFakeModel(float radius, uint8 numLines, int32 animIndex) {
    modelInfoStore.fill(0); colModelStore.fill(0); colDataStore.fill(0);
    At<uint32>(modelInfoStore.data(), 0) = P(miVt.data());
    At<uint32>(modelInfoStore.data(), 0x14) = P(colModelStore.data());
    At<int32>(modelInfoStore.data(), 0x60) = animIndex;
    At<float>(colModelStore.data(), 0x24) = radius;
    At<uint32>(colModelStore.data(), 0x2C) = P(colDataStore.data());
    At<uint8>(colDataStore.data(), 6) = numLines;
    Mem<uint32>(FAKE_MODEL_SLOT) = P(modelInfoStore.data());
}

// Fixtures de campos comunes
inline void SetVec(uint8* base, size_t off, float x, float y, float z) { At<float>(base, off) = x; At<float>(base, off + 4) = y; At<float>(base, off + 8) = z; }
inline void RandMatrix(uint8* m, uint32& s) {
    std::memset(m, 0, 0x48);
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u }) At<float>(m, off) = PickUnit(s);
    SetVec(m, 0x30, PickWorld(s), PickWorld(s), PickWorld(s));
}
}
