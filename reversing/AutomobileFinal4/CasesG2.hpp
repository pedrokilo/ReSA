#pragma once
// Grupo G2 del banco AutomobileFinal4: Constructor, Destructor, SetupSuspensionLines, Fix, SetupDamageAfterLoad, FixTyre, FixDoor,
// FixPanel, SetTotalDamage, PopDoor, PopPanel, PopBoot, CloseBoot, PopBootUsingPhysics, CloseAllDoors, SetBumperDamage,
// SetPanelDamage, SetDoorDamage, OpenDoor, ProcessSwingingDoor, SpawnFlyingComponent, RemoveBonnetInPedCollision,
// dmgDrawCarCollidingParticles y SetTowLink de CAutomobile (24 funciones).
//
// Este fichero aporta la infraestructura propia del grupo (todo con prefijo G2/g2/S2_ para no chocar con los demas grupos):
//   * dobles trazados de todo lo que toca estado real (RW, audio, particulas, mundo, fisica, colision, eventos, pools, objetos);
//   * buferes estaticos propios (objetos falsos, matrices, esferas, grupo de eventos, pool de objetos, camara) registrados con Reg();
//   * constructores de fixtures coherentes (marcos RW falsos, puertas, paneles, gestor de dano, handling, model info);
//   * el despachador RunG2 que reparte las funciones en CasesG2_a.hpp (Constructor ... SetTotalDamage), CasesG2_b.hpp (PopDoor ...
//     SetDoorDamage) y CasesG2_c.hpp (OpenDoor ... SetTowLink).
// Reglas del banco: dobles SIEMPRE con `if(!Mine()) return neutro;` al principio, convencion thiscall como __fastcall(self, edx, ...)
// con el mismo 'ret N' que el original, x87 a 24 bits (ya fijado por el bucle comun) y estado global Save()-ado en InitG2().
#include "Ctx.hpp"
#include "Entity/Object/Object.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventGroup.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Glass.h"
#include "VisibilityPlugins.h"
#include "Collision/Collision.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "MemoryMgr.h"
#include "Models/ClumpModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "RwHelper.h"
#include "Door.h"
#include "BouncingPanel.h"
#include "DamageManager.h"

namespace AutomobileFinal4Live {

// ---------------------------------------------------------------------------------------------------------------------
// Mascaras de dobles propias (propietario 2)
// ---------------------------------------------------------------------------------------------------------------------
enum : uint64 {
    G2_COMPVIS   = 1ull<<0,   // CVehicle::SetComponentVisibility 0x6D2700
    G2_AUDF      = 1ull<<1,   // CAEVehicleAudioEntity::AddAudioEvent(event, float) 0x4F6420
    G2_AUDP      = 1ull<<2,   // CAEVehicleAudioEntity::AddAudioEvent(event, CPhysical*) 0x4F7580
    G2_SPAWN     = 1ull<<3,   // CAutomobile::SpawnFlyingComponent 0x6A8580
    G2_FRALL     = 1ull<<4,   // RwFrameForAllObjects 0x7F1200
    G2_CLUMPALL  = 1ull<<5,   // RpClumpForAllAtomics 0x749B70
    G2_FORCE     = 1ull<<6,   // CPhysical::ApplyMoveForce(CVector) 0x5429F0 y ApplyTurnForce 0x542A50
    G2_DOORP     = 1ull<<7,   // CDoor::Process 0x6F4040 y CDoor::ProcessImpact 0x6F4540
    G2_PROCCOL   = 1ull<<8,   // CCollision::ProcessColModels 0x4185C0
    G2_WORLDADD  = 1ull<<9,   // CWorld::Add 0x563220
    G2_OBJNEW    = 1ull<<10,  // CObject::operator new 0x5A1EE0 y CObject::CObject 0x5A1D10
    G2_OBJFN     = 1ull<<11,  // CObject::RefModelInfo 0x59F330 y CEntity::AttachToRwObject 0x533ED0
    G2_RWOBJ     = 1ull<<12,  // RwFrameCreate, RpAtomicClone, RpAtomicSetFrame, RwFrameGetLTM, SetAtomicRenderCallback
    G2_GETFIRST  = 1ull<<13,  // GetFirstObject 0x7348C0
    G2_FX        = 1ull<<14,  // Fx_c::AddSparks 0x49F040, Fx_c::AddDebris 0x49F750, FxSystem_c::AddParticle 0x4AA440
    G2_KILL      = 1ull<<15,  // FxSystem_c::Kill 0x4AA3F0
    G2_ONSCREEN  = 1ull<<16,  // CEntity::GetIsOnScreen 0x534540
    G2_EVENT     = 1ull<<17,  // CEventSoundQuiet (ctor/dtor), CEventGroup::Add 0x4AB420, GetEventGlobalGroup 0x4ABA50
    G2_CTOR      = 1ull<<18,  // CVehicle::CVehicle, SetModelIndex, FillFrameArray, ChooseVehicleColour, AddVehicleUpgrade, audio Initialise
    G2_DTOR      = 1ull<<19,  // CAEVehicleAudioEntity::Terminate 0x4FB8C0 y CVehicle::~CVehicle 0x6E2B40
    G2_MALLOC    = 1ull<<20,  // CMemoryMgr::Malloc 0x72F420
    G2_WHEELPOSN = 1ull<<21,  // CVehicleModelInfo::GetWheelPosn 0x4C7D20
    G2_GLASS     = 1ull<<22,  // CGlass::CarWindscreenShatters 0x71C2B0
    G2_DMGSUB    = 1ull<<23,  // CAutomobile::SetBumperDamage 0x6B1350 / SetPanelDamage 0x6B1480 / SetDoorDamage 0x6B1600
    G2_SUSPSTUB  = 1ull<<24,  // CAutomobile::SetupSuspensionLines 0x6A65D0 (solo en el caso del constructor)
    G2_PLACE     = 1ull<<25,  // CAutomobile::PlaceOnRoadProperly 0x6AF420
    G2_TOW       = 1ull<<26,  // CEntity::RegisterReference 0x571B70, Remove/AddToMovingList 0x542860/0x542800, UpdateTrailerLink 0x6DFC50
};

// ---------------------------------------------------------------------------------------------------------------------
// Desplazamientos crudos del EXE usados por este grupo
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t G2_O_FLAGS=0x1C, G2_O_MODEL=0x22, G2_O_STATUS=0x36, G2_O_PHYS=0x40, G2_O_MOVE=0x44, G2_O_TURN=0x50;
constexpr size_t G2_O_DMGINT=0xD8, G2_O_BRIGHT=0x12C, G2_O_AUDIO=0x138, G2_O_HANDLING=0x384, G2_O_HFLAGS=0x38C;
constexpr size_t G2_O_VFLAGS=0x428, G2_O_COLORS=0x434, G2_O_TOWING=0x4C4, G2_O_TOWED=0x4C8, G2_O_DOORLOCK=0x4F8, G2_O_VTYPE=0x590, G2_O_VSUB=0x594;
constexpr size_t G2_O_DMG=0x5A0, G2_O_DOORS=0x5B8, G2_O_NODES=0x648, G2_O_PANELS=0x6AC, G2_O_CHASSIS=0x70C, G2_O_MISCANGLE=0x86C;
constexpr size_t G2_O_FIRE=0x57C, G2_O_NITRO=0x978, G2_O_HEIGHT=0x898, G2_O_MOVEF=0x8B4, G2_O_TURNF=0x8C0;

// ---------------------------------------------------------------------------------------------------------------------
// Buferes estaticos propios (un unico bloque registrado para Id(); se rehace entero en cada ejecucion)
// ---------------------------------------------------------------------------------------------------------------------
struct G2Buf {
    alignas(16) uint8 obj[3][0x200];       // CObject falsos (0x17C bytes reales): 0..2
    alignas(16) uint8 objMat[3][0x60];     // matrices de los objetos falsos
    alignas(16) uint8 pool[0x40];          // CObjectPool falso (solo importa el byte 0x11, m_DealWithNoMemory)
    alignas(16) uint8 evGroup[0x40];       // grupo de eventos global falso
    alignas(16) uint8 sph[16][0x14];       // CColSphere falsas
    alignas(16) uint8 cam[0x60];           // CMatrix falsa de la camara
    alignas(16) uint8 vec[8][0x10];        // vectores de uso libre (posiciones, colores...)
    alignas(16) uint8 ped[0x20];           // "peaton" falso (solo se pasa como puntero)
    alignas(16) uint8 scratch[0x100];
};
inline G2Buf g2b;

// vtable falsa de CObject (copia de huecos con dobles en 4 = SetIsStatic y 6 = SetModelIndexNoCreate)
inline std::array<uintptr_t,64> g2ObjVt{};

// ---------------------------------------------------------------------------------------------------------------------
// Estado compartido con los dobles (se fija en cada caso antes de la llamada)
// ---------------------------------------------------------------------------------------------------------------------
inline bool g2OnScreen{}, g2ProcRet{}, g2ImpactRet{}, g2PoolFull{};
inline void* g2SpawnRet{};         // objeto que devuelve el doble de SpawnFlyingComponent
inline void* g2CurAtomic{};        // atomo que entrega el callback GetCurrentAtomicObjectCB
inline void* g2FirstObj{};         // objeto que devuelve GetFirstObject
inline int32 g2ColRet{};
inline float g2WheelPos[4][3]{};
inline float g2AttachPos[3]{};
inline bool g2HitchRet{}, g2BarRet{};
inline float g2HitchVec[3]{}, g2BarVec[3]{};
inline uint32 g2NextFrame{}, g2NextAtom{}, g2ObjIdx{};
inline std::array<uint8,64> g2Aux{};

// Comienzo de cada ejecucion: restablece TODO el estado compartido con los dobles (el estado que deja un lado no debe llegar al otro)
inline void G2Begin() {
    std::memset(&g2b, 0xCD, sizeof(g2b));
    g2OnScreen = false; g2ProcRet = false; g2ImpactRet = false; g2PoolFull = false; g2HitchRet = false; g2BarRet = false;
    g2SpawnRet = nullptr; g2CurAtomic = nullptr; g2FirstObj = nullptr; g2ColRet = 0;
    std::memset(g2WheelPos, 0, sizeof(g2WheelPos)); std::memset(g2AttachPos, 0, sizeof(g2AttachPos));
    std::memset(g2HitchVec, 0, sizeof(g2HitchVec)); std::memset(g2BarVec, 0, sizeof(g2BarVec));
    g2NextFrame = 0; g2NextAtom = 0; g2ObjIdx = 0; g2Aux.fill(0);
}

// Modulo de la DLL (para reconocer la vtable y resolver exportaciones)
inline uintptr_t g2ModBase{}, g2ModEnd{};
inline void G2InitModule() {
    HMODULE mod{};
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(&G2InitModule), &mod);
    g2ModBase = uintptr_t(mod);
    if (g2ModBase) {
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(g2ModBase + reinterpret_cast<const IMAGE_DOS_HEADER*>(g2ModBase)->e_lfanew);
        g2ModEnd = g2ModBase + nt->OptionalHeader.SizeOfImage;
    }
}
inline bool G2InDll(uintptr_t a) { return a >= g2ModBase && a < g2ModEnd; }
inline uintptr_t G2Exp(const char* mangled) { return g2ModBase ? uintptr_t(GetProcAddress(reinterpret_cast<HMODULE>(g2ModBase), mangled)) : 0u; }
template<class M> inline uintptr_t G2PA(M m) { static_assert(sizeof(M) == sizeof(uintptr_t)); return std::bit_cast<uintptr_t>(m); }

// Identificacion de los callbacks de RW por direccion (EXE o cuerpo de la DLL): 1 SetAtomicFlagCB, 2 ClearAtomicFlagCB,
// 3 GetCurrentAtomicObjectCB (el EXE pasa 0x6A0750), 4 HideAllComponentsAtomicCB (0x4C7790), 0xFF otro
inline uintptr_t g2DllSetFlag{}, g2DllClearFlag{}, g2DllCurAtom{}, g2DllHideAll{};
inline int G2CbId(const void* cb) {
    const uintptr_t b = Body(uintptr_t(cb));
    if (b == 0x4C7B90 || (g2DllSetFlag && b == g2DllSetFlag))     return 1;
    if (b == 0x4C7BB0 || (g2DllClearFlag && b == g2DllClearFlag)) return 2;
    if (b == 0x6A0750 || (g2DllCurAtom && b == g2DllCurAtom))     return 3;
    if (b == 0x4C7790 || (g2DllHideAll && b == g2DllHideAll))     return 4;
    return 0xFF;
}

// ---------------------------------------------------------------------------------------------------------------------
// Dobles (todos con Mine() al principio y el 'ret N' del original)
// ---------------------------------------------------------------------------------------------------------------------
// CVehicle::SetComponentVisibility(RwFrame*, uint32) : thiscall ret 8
inline void __fastcall S2_CompVis(void* self, void*, void* frame, uint32 state) { if (!Mine()) return; Push(TG("CompVis"), self, frame, state); }
// CAEVehicleAudioEntity::AddAudioEvent(eAudioEvents, float) : thiscall ret 8
inline void __fastcall S2_AudF(void* self, void*, uint32 ev, float p) { if (!Mine()) return; Push(TG("AudF"), self, ev, p); }
// CAEVehicleAudioEntity::AddAudioEvent(eAudioEvents, CPhysical*) : thiscall ret 8
inline void __fastcall S2_AudP(void* self, void*, uint32 ev, void* phys) { if (!Mine()) return; Push(TG("AudP"), self, ev, phys); }
// CAutomobile::SpawnFlyingComponent(eCarNodes, uint32) : thiscall ret 8
inline void* __fastcall S2_Spawn(void* self, void*, int32 node, uint32 type) { if (!Mine()) return nullptr; Push(TG("Spawn"), self, node, type); return g2SpawnRet; }
// RwFrameForAllObjects(RwFrame*, RwObjectCallBack, void*) : cdecl
inline void* __cdecl S2_FrameForAll(void* frame, void* cb, void* data) {
    if (!Mine()) return frame;
    const int id = G2CbId(cb);
    Push(TG("FrameForAll"), frame, id, data);
    if (id == 3 && data) *static_cast<void**>(data) = g2CurAtomic;     // GetCurrentAtomicObjectCB entrega el atomo
    return frame;
}
// RpClumpForAllAtomics(RpClump*, RpAtomicCallBack, void*) : cdecl
inline void* __cdecl S2_ClumpForAll(void* clump, void* cb, void* data) { if (!Mine()) return clump; Push(TG("ClumpForAll"), clump, G2CbId(cb), data); return clump; }
// CPhysical::ApplyMoveForce(CVector) : thiscall ret 0xC
inline void __fastcall S2_ApplyMove(void* self, void*, float x, float y, float z) { if (!Mine()) return; Push(TG("ApplyMove"), self, x, y, z); }
// CPhysical::ApplyTurnForce(CVector force, CVector point) : thiscall ret 0x18
inline void __fastcall S2_ApplyTurn(void* self, void*, float fx, float fy, float fz, float px, float py, float pz) {
    if (!Mine()) return;
    const float v[6]{fx, fy, fz, px, py, pz};
    Push(TG("ApplyTurn"), self, Fnv32(v, sizeof(v)), F(fx), F(fy), F(fz));
}
// CDoor::Process / ProcessImpact(CVehicle*, const CVector&, const CVector&, const CVector&) : thiscall ret 0x10
inline uint8 __fastcall S2_DoorProcess(void* self, void*, void* veh, const float* mv, const float* tf, const float* off) {
    if (!Mine()) return 0;
    Push(TG("DoorProcess"), self, veh, Fnv32(mv, 12), Fnv32(tf, 12), Fnv32(off, 12)); return g2ProcRet ? 1 : 0; }
inline uint8 __fastcall S2_DoorImpact(void* self, void*, void* veh, const float* mv, const float* tf, const float* off) {
    if (!Mine()) return 0;
    Push(TG("DoorImpact"), self, veh, Fnv32(mv, 12), Fnv32(tf, 12), Fnv32(off, 12)); return g2ImpactRet ? 1 : 0; }
// CCollision::ProcessColModels(const CMatrix&, CColModel&, const CMatrix&, CColModel&, std::array<CColPoint,32>&, CColPoint*, float*, bool) : cdecl
inline int32 __cdecl S2_ProcCol(const void* mA, void* cA, const void* mB, void* cB, void* out, void* wheel, float* ratios, uint32 flag) {
    if (!Mine()) return 0;
    Push(TG("ProcCol"), mA, cA, mB, cB, out); Push(TG("ProcCol2"), wheel, ratios, uint8(flag)); return g2ColRet; }
// CWorld::Add(CEntity*) : cdecl
inline void __cdecl S2_WorldAdd(void* ent) { if (!Mine()) return; Push(TG("WorldAdd"), ent); }

// ---- objetos falsos ----
inline void __fastcall S2_ObjSetStatic(void* self, void*, uint32 isStatic) { if (!Mine()) return; Push(TG("ObjSetStatic"), self, uint8(isStatic)); }
inline void __fastcall S2_ObjSetModel(void* self, void*, uint32 idx) { if (!Mine()) return; Push(TG("ObjSetModel"), self, idx); At<int16>(self, G2_O_MODEL) = int16(idx); }
inline void __fastcall S2_ObjTrap(void* self, void*) { if (!Mine()) return; Push(TG("ObjTrap"), self); }
// CObject::operator new(size_t) : cdecl
inline void* __cdecl S2_ObjNew(size_t size) {
    if (!Mine()) return nullptr;
    Push(TG("ObjNew"), uint32(size));
    if (g2PoolFull) return nullptr;
    return g2b.obj[g2ObjIdx < 3 ? g2ObjIdx : 2];
}
// CObject::CObject() : thiscall sin argumentos; deja un objeto fisico coherente (matriz y vtable propias)
inline void* __fastcall S2_ObjCtor(void* self, void*) {
    if (!Mine() || !self) return self;
    Push(TG("ObjCtor"), self);
    auto* o = static_cast<uint8*>(self);
    std::memset(o, 0, 0x17C);
    const size_t k = size_t((o - &g2b.obj[0][0]) / 0x200);
    auto* m = g2b.objMat[k < 3 ? k : 2];
    std::memset(m, 0, 0x60);
    At<float>(m, 0x00) = 1.f; At<float>(m, 0x14) = 1.f; At<float>(m, 0x28) = 1.f;
    At<void*>(o, 0) = g2ObjVt.data();
    At<void*>(o, 0x14) = m;
    return self;
}
// CObject::RefModelInfo(int32) : thiscall ret 4
// CObject::SetRemapTexture(RwTexture*, int32) : thiscall ret 8
inline void __fastcall S2_RemapTex(void* self, void*, void* tex, int32 txd) { if (!Mine()) return; Push(TG("RemapTex"), self, tex, txd); }
inline void __fastcall S2_RefModel(void* self, void*, int32 model) { if (!Mine()) return; Push(TG("RefModel"), self, model); }
// CEntity::AttachToRwObject(RwObject*, bool) : thiscall ret 8 ; coloca la posicion del objeto (matriz propia o de posicion)
inline void __fastcall S2_Attach(void* self, void*, void* rwObj, uint32 upd) {
    if (!Mine()) return;
    Push(TG("Attach"), self, rwObj, uint8(upd));
    auto* mat = At<uint8*>(self, 0x14);
    float* p = mat ? reinterpret_cast<float*>(mat + 0x30) : reinterpret_cast<float*>(static_cast<uint8*>(self) + 4);
    p[0] = g2AttachPos[0]; p[1] = g2AttachPos[1]; p[2] = g2AttachPos[2];
}
inline void* __cdecl S2_FrameCreate() { if (!Mine()) return nullptr; auto* f = w.fr[30 + (g2NextFrame++ % 16)]; Push(TG("FrameCreate"), f); return f; }
inline void* __cdecl S2_AtomClone(void* a) { if (!Mine()) return nullptr; auto* r = w.at[8 + (g2NextAtom++ % 8)]; Push(TG("AtomClone"), a, r); return r; }
inline void __cdecl S2_AtomSetFrame(void* a, void* f) { if (!Mine()) return; Push(TG("AtomSetFrame"), a, f); }
inline void* __cdecl S2_FrameGetLTM(void* f) { if (!Mine()) return static_cast<uint8*>(f) + 0x50; Push(TG("FrameGetLTM"), f); return static_cast<uint8*>(f) + 0x50; }
inline void __cdecl S2_RenderCb(void* a, void* cb) { if (!Mine()) return; Push(TG("RenderCb"), a, cb); }
inline void* __cdecl S2_GetFirst(void* frame) { if (!Mine()) return nullptr; Push(TG("GetFirst"), frame); return g2FirstObj; }

// ---- particulas ----
// Fx_c::AddSparks(const CVector&, const CVector&, float, int32, CVector, eSparkType, float, float) : thiscall ret 0x28
inline void __fastcall S2_Sparks(void* self, void*, const float* origin, const float* dir, float force, int32 amount, float ax, float ay, float az, uint32 type, float spread, float life) {
    if (!Mine()) return;
    const float across[3]{ax, ay, az};
    Push(TG("Sparks"), self, Fnv32(origin, 12), Fnv32(dir, 12), force, amount); Push(TG("Sparks2"), Fnv32(across, 12), uint8(type), spread, life);
}
// Fx_c::AddDebris(const CVector&, const RwRGBA&, float, int32) : thiscall ret 0x10
inline void __fastcall S2_Debris(void* self, void*, const float* pos, const uint32* color, float scale, int32 amount) {
    if (!Mine()) return; Push(TG("Debris"), self, Fnv32(pos, 12), *color, scale, amount); }
// FxSystem_c::AddParticle(const CVector&, const CVector&, float, const FxPrtMult_c&, float, float, float, bool) : thiscall ret 0x20
inline void __fastcall S2_Particle(void* self, void*, const float* pos, const float* vel, float timeSince, const float* prt, float rotZ, float light, float limit, uint32 local) {
    if (!Mine()) return;
    Push(TG("Particle"), self, Fnv32(pos, 12), Fnv32(vel, 12), timeSince, Fnv32(prt, 0x1C)); Push(TG("Particle2"), rotZ, light, limit, uint8(local)); }
inline void __fastcall S2_Kill(void* self, void*) { if (!Mine()) return; Push(TG("Kill"), self); }
inline uint8 __fastcall S2_OnScreen(void* self, void*) { if (!Mine()) return 0; Push(TG("OnScreen"), self); return g2OnScreen ? 1 : 0; }

// ---- eventos ----
// CEventSoundQuiet::CEventSoundQuiet(CEntity*, float, uint32, const CVector&) : thiscall ret 0x10 ; ~CEventSoundQuiet : thiscall
inline void* __fastcall S2_EvCtor(void* self, void*, void* ent, float level, uint32 start, const float* pos) {
    if (!Mine()) return self; Push(TG("EvCtor"), ent, level, start, Fnv32(pos, 12)); return self; }
inline void __fastcall S2_EvDtor(void* self, void*) { if (!Mine()) return; Push(TG("EvDtor")); (void)self; }
// CEventGroup::Add(CEvent*, bool) : thiscall ret 8
inline void* __fastcall S2_EvAdd(void* self, void*, void* ev, uint32 valid) { if (!Mine()) return ev; Push(TG("EvAdd"), self, uint8(valid)); return ev; }
// GetEventGlobalGroup() : cdecl; no se traza (el EXE lo llama tras construir el evento y la DLL antes)
inline void* __cdecl S2_EvGroup() { return g2b.evGroup; }

// ---- constructor / destructor ----
// CVehicle::CVehicle(eVehicleCreatedBy) : thiscall ret 4 ; rellena la base de forma determinista
inline void* __fastcall S2_VehCtor(void* self, void*, uint32 createdBy) {
    if (!Mine()) return self;
    Push(TG("VehCtor"), self, createdBy);
    uint32 s = createdBy * 2654435761u + 0x1234567u;
    auto* b = static_cast<uint8*>(self);
    for (size_t k = 4; k + 4 <= 0x5A0; k += 4) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; std::memcpy(b + k, &s, 4); }
    At<uint32>(self, 0) = 0x0BADF00Du; At<void*>(self, 0x14) = nullptr;
    At<uint32>(self, G2_O_DOORLOCK) = (createdBy & 1u) ? 1u : (createdBy & 2u ? 0u : 3u);
    return self;
}
// CVehicle::SetModelIndex(uint32) : thiscall ret 4 ; fija el modelo y el clump (como el original)
inline void __fastcall S2_VehSetModel(void* self, void*, uint32 idx) {
    if (!Mine()) return;
    Push(TG("VehSetModel"), self, idx);
    At<int16>(self, G2_O_MODEL) = int16(idx); At<void*>(self, 0x18) = w.clump[0]; w.clump[0][0] = 2;
}
// CClumpModelInfo::FillFrameArray(RpClump*, RwFrame**) : cdecl ; rellena los nodos con marcos o nulos segun g2Aux
inline void __cdecl S2_FillFrames(void* clump, void** nodes) {
    if (!Mine()) return;
    Push(TG("FillFrames"), clump, nodes);
    for (int i = 0; i < 25; ++i) nodes[i] = (g2Aux[size_t(i)] & 1) ? static_cast<void*>(w.fr[i]) : nullptr;
}
// CVehicleModelInfo::ChooseVehicleColour(uint8&, uint8&, uint8&, uint8&, int32) : thiscall ret 0x14
inline void __fastcall S2_Choose(void* self, void*, uint8* a, uint8* b, uint8* c, uint8* d, int32 shift) {
    if (!Mine()) return;
    Push(TG("Choose"), self, a, b, c, d); Push(TG("Choose2"), shift);
    *a = uint8(g2Aux[40] + 1); *b = uint8(g2Aux[41] + 2); *c = uint8(g2Aux[42] + 3); *d = uint8(g2Aux[43] + 4);
}
// CVehicle::AddVehicleUpgrade(int32) : thiscall ret 4
inline void __fastcall S2_AddUpgrade(void* self, void*, int32 model) { if (!Mine()) return; Push(TG("AddUpgrade"), self, model); }
// CAEVehicleAudioEntity::Initialise(CEntity*) : thiscall ret 4 ; Terminate() : thiscall
inline void __fastcall S2_AudioInit(void* self, void*, void* ent) { if (!Mine()) return; Push(TG("AudioInit"), self, ent); }
inline void __fastcall S2_AudioTerm(void* self, void*) { if (!Mine()) return; Push(TG("AudioTerm"), self); }
inline void __fastcall S2_VehDtor(void* self, void*) { if (!Mine()) return; Push(TG("VehDtor"), self); }
inline void __fastcall S2_SetupSusp(void* self, void*) { if (!Mine()) return; Push(TG("SetupSusp"), self); }
// CMemoryMgr::Malloc(uint32, uint32) : cdecl ; la DLL solo apila el tamano
inline void* __cdecl S2_Malloc(uint32 size) { if (!Mine()) return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(size); Push(TG("Malloc"), size); return ArenaAlloc(size); }
// CVehicleModelInfo::GetWheelPosn(int32, CVector&, bool) const : thiscall ret 0xC
inline void __fastcall S2_WheelPosn(void* self, void*, int32 wheel, float* out, uint32 local) {
    if (!Mine()) return;
    Push(TG("WheelPosn"), self, wheel, uint8(local));
    const int k = (wheel >= 0 && wheel < 4) ? wheel : 0;
    out[0] = g2WheelPos[k][0]; out[1] = g2WheelPos[k][1]; out[2] = g2WheelPos[k][2];
}
inline void __cdecl S2_Glass(void* veh) { if (!Mine()) return; Push(TG("Glass"), veh); }
// SetBumperDamage / SetPanelDamage / SetDoorDamage (ePanels|eDoors, bool) : thiscall ret 8
inline void __fastcall S2_DmgBumper(void* self, void*, uint32 p, uint32 f) { if (!Mine()) return; Push(TG("DmgBumper"), self, p, uint8(f)); }
inline void __fastcall S2_DmgPanel(void* self, void*, uint32 p, uint32 f) { if (!Mine()) return; Push(TG("DmgPanel"), self, p, uint8(f)); }
inline void __fastcall S2_DmgDoor(void* self, void*, uint32 p, uint32 f) { if (!Mine()) return; Push(TG("DmgDoor"), self, p, uint8(f)); }
inline void __fastcall S2_Place(void* self, void*) { if (!Mine()) return; Push(TG("PlaceOnRoad"), self); }
// CEntity::RegisterReference(CEntity**) : thiscall ret 4 ; Remove/AddToMovingList : thiscall ; UpdateTrailerLink(bool, bool) : thiscall ret 8
inline void __fastcall S2_RegRef(void* self, void*, void** ref) { if (!Mine()) return; Push(TG("RegRef"), self, ref, *ref); }
inline void __fastcall S2_RemMoving(void* self, void*) { if (!Mine()) return; Push(TG("RemMoving"), self); }
inline void __fastcall S2_AddMoving(void* self, void*) { if (!Mine()) return; Push(TG("AddMoving"), self); }
inline void __fastcall S2_TrailerLink(void* self, void*, uint32 a, uint32 b) { if (!Mine()) return; Push(TG("TrailerLink"), self, uint8(a), uint8(b)); }

// ---- ranuras virtuales de los coches falsos (SetVt) ----
// CVehicle::IsDoorMissing(eDoors) : thiscall ret 4 (hueco 38)
inline uint8 __fastcall S2_VtDoorMissing(void* self, void*, uint32 door) { if (!Mine()) return 0; Push(TG("VtDoorMissing"), self, door); return uint8(((g2Aux[48] >> (door & 7)) & 1) ? 1 : 0); }
// CVehicle::OpenDoor(CPed*, int32, eDoors, float, bool) : thiscall ret 0x14 (hueco 27)
inline void __fastcall S2_VtOpenDoor(void* self, void*, void* ped, int32 node, uint32 door, float ratio, uint32 snd) {
    if (!Mine()) return; Push(TG("VtOpenDoor"), self, ped, node, door, ratio); Push(TG("VtOpenDoor2"), uint8(snd)); }
// CVehicle::GetTowHitchPos / GetTowBarPos(CVector&, bool, CVehicle*) : thiscall ret 0xC (huecos 59 y 60)
inline uint8 __fastcall S2_VtHitch(void* self, void*, float* out, uint32 chk, void* veh) {
    if (!Mine()) return 0; Push(TG("VtHitch"), self, uint8(chk), veh);
    out[0] = g2HitchVec[0]; out[1] = g2HitchVec[1]; out[2] = g2HitchVec[2]; return g2HitchRet ? 1 : 0; }
inline uint8 __fastcall S2_VtBar(void* self, void*, float* out, uint32 chk, void* veh) {
    if (!Mine()) return 0; Push(TG("VtBar"), self, uint8(chk), veh);
    out[0] = g2BarVec[0]; out[1] = g2BarVec[1]; out[2] = g2BarVec[2]; return g2BarRet ? 1 : 0; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de dobles
// ---------------------------------------------------------------------------------------------------------------------
inline std::string StubsG2() {
    std::string miss;
    G2InitModule();
    auto H = [&](uint64 mask, uintptr_t exe, const char* cls, const char* nm, void* fn) { if (!AddHookStub(2, mask, exe, cls, nm, fn)) { miss += std::string(cls) + "::" + nm + ";"; } };
    auto E = [&](uint64 mask, uintptr_t exe, const char* mangled, void* fn, const char* label) {
        const uintptr_t d = G2Exp(mangled); if (!d) { miss += std::string(label) + ";"; return; } AddStub(2, mask, exe, d, fn); };

    H(G2_COMPVIS, 0x6D2700, "CVehicle", "SetComponentVisibility", FnPtr(&S2_CompVis));
    H(G2_AUDF, 0x4F6420, "CAEVehicleAudioEntity", "AddAudioEvent-0", FnPtr(&S2_AudF));
    H(G2_AUDP, 0x4F7580, "CAEVehicleAudioEntity", "AddAudioEvent-1", FnPtr(&S2_AudP));
    H(G2_SPAWN, 0x6A8580, "CAutomobile", "SpawnFlyingComponent", FnPtr(&S2_Spawn));
    AddStub(2, G2_FRALL, 0x7F1200, 0, FnPtr(&S2_FrameForAll));
    AddStub(2, G2_CLUMPALL, 0x749B70, 0, FnPtr(&S2_ClumpForAll));
    H(G2_FORCE, 0x5429F0, "CPhysical", "ApplyMoveForce-vec", FnPtr(&S2_ApplyMove));
    H(G2_FORCE, 0x542A50, "CPhysical", "ApplyTurnForce", FnPtr(&S2_ApplyTurn));
    H(G2_DOORP, 0x6F4040, "CDoor", "Process", FnPtr(&S2_DoorProcess));
    H(G2_DOORP, 0x6F4540, "CDoor", "ProcessImpact", FnPtr(&S2_DoorImpact));
    H(G2_PROCCOL, 0x4185C0, "CCollision", "ProcessColModels", FnPtr(&S2_ProcCol));
    H(G2_WORLDADD, 0x563220, "CWorld", "Add", FnPtr(&S2_WorldAdd));
    E(G2_OBJNEW, 0x5A1EE0, "??2CObject@@SAPAXI@Z", FnPtr(&S2_ObjNew), "CObject::operator new");
    E(G2_OBJNEW, 0x5A1D10, "??0CObject@@QAE@XZ", FnPtr(&S2_ObjCtor), "CObject::CObject");
    H(G2_OBJFN, 0x59F330, "CObject", "RefModelInfo", FnPtr(&S2_RefModel));
    H(G2_OBJFN, 0x59F350, "CObject", "SetRemapTexture", FnPtr(&S2_RemapTex));
    H(G2_OBJFN, 0x533ED0, "CEntity", "AttachToRwObject", FnPtr(&S2_Attach));
    AddStub(2, G2_RWOBJ, 0x7F0410, 0, FnPtr(&S2_FrameCreate));
    AddStub(2, G2_RWOBJ, 0x749E60, 0, FnPtr(&S2_AtomClone));
    AddStub(2, G2_RWOBJ, 0x74BF20, 0, FnPtr(&S2_AtomSetFrame));
    AddStub(2, G2_RWOBJ, 0x7F0990, 0, FnPtr(&S2_FrameGetLTM));
    H(G2_RWOBJ, 0x7328A0, "CVisibilityPlugins", "SetAtomicRenderCallback", FnPtr(&S2_RenderCb));
    H(G2_GETFIRST, 0x7348C0, "RwHelper", "GetFirstObject", FnPtr(&S2_GetFirst));
    AddStub(2, G2_FX, 0x49F040, G2PA(&Fx_c::AddSparks), FnPtr(&S2_Sparks));
    AddStub(2, G2_FX, 0x49F750, G2PA(&Fx_c::AddDebris), FnPtr(&S2_Debris));
    H(G2_FX, 0x4AA440, "FxSystem_c", "AddParticle-v3d", FnPtr(&S2_Particle));
    H(G2_KILL, 0x4AA3F0, "FxSystem_c", "Kill", FnPtr(&S2_Kill));
    H(G2_ONSCREEN, 0x534540, "CEntity", "GetIsOnScreen", FnPtr(&S2_OnScreen));
    E(G2_EVENT, 0x5E05B0, "??0CEventSoundQuiet@@QAE@PAVCEntity@@MIABVCVector@@@Z", FnPtr(&S2_EvCtor), "CEventSoundQuiet::CEventSoundQuiet");
    E(G2_EVENT, 0x5DEA00, "??1CEventSoundQuiet@@UAE@XZ", FnPtr(&S2_EvDtor), "CEventSoundQuiet::~CEventSoundQuiet");
    H(G2_EVENT, 0x4AB420, "CEventGroup", "Add-", FnPtr(&S2_EvAdd));
    H(G2_EVENT, 0x4ABA50, "RwHelper", "GetEventGlobalGroup", FnPtr(&S2_EvGroup));
    E(G2_CTOR, 0x6D5F10, "??0CVehicle@@QAE@W4eVehicleCreatedBy@@@Z", FnPtr(&S2_VehCtor), "CVehicle::CVehicle");
    H(G2_CTOR, 0x6D6A40, "CVehicle", "SetModelIndex", FnPtr(&S2_VehSetModel));
    H(G2_CTOR, 0x4C5440, "CClumpModelInfo", "FillFrameArray", FnPtr(&S2_FillFrames));
    H(G2_CTOR, 0x4C8500, "CVehicleModelInfo", "ChooseVehicleColour", FnPtr(&S2_Choose));
    E(G2_CTOR, 0x6E3290, "?AddVehicleUpgrade@CVehicle@@QAEXH@Z", FnPtr(&S2_AddUpgrade), "CVehicle::AddVehicleUpgrade");
    H(G2_CTOR, 0x4F7670, "CAEVehicleAudioEntity", "Initialise", FnPtr(&S2_AudioInit));
    H(G2_DTOR, 0x4FB8C0, "CAEVehicleAudioEntity", "Terminate", FnPtr(&S2_AudioTerm));
    E(G2_DTOR, 0x6E2B40, "??1CVehicle@@UAE@XZ", FnPtr(&S2_VehDtor), "CVehicle::~CVehicle");
    AddStub(2, G2_MALLOC, 0x72F420, 0, FnPtr(&S2_Malloc));
    H(G2_WHEELPOSN, 0x4C7D20, "CVehicleModelInfo", "GetWheelPosn", FnPtr(&S2_WheelPosn));
    H(G2_GLASS, 0x71C2B0, "CGlass", "CarWindscreenShatters", FnPtr(&S2_Glass));
    H(G2_DMGSUB, 0x6B1350, "CAutomobile", "SetBumperDamage", FnPtr(&S2_DmgBumper));
    H(G2_DMGSUB, 0x6B1480, "CAutomobile", "SetPanelDamage", FnPtr(&S2_DmgPanel));
    H(G2_DMGSUB, 0x6B1600, "CAutomobile", "SetDoorDamage", FnPtr(&S2_DmgDoor));
    H(G2_SUSPSTUB, 0x6A65D0, "CAutomobile", "SetupSuspensionLines", FnPtr(&S2_SetupSusp));
    H(G2_PLACE, 0x6AF420, "CAutomobile", "PlaceOnRoadProperly", FnPtr(&S2_Place));
    H(G2_TOW, 0x571B70, "CEntity", "RegisterReference-", FnPtr(&S2_RegRef));
    H(G2_TOW, 0x542860, "CPhysical", "RemoveFromMovingList", FnPtr(&S2_RemMoving));
    H(G2_TOW, 0x542800, "CPhysical", "AddToMovingList", FnPtr(&S2_AddMoving));
    E(G2_TOW, 0x6DFC50, "?UpdateTrailerLink@CVehicle@@QAEX_N0@Z", FnPtr(&S2_TrailerLink), "CVehicle::UpdateTrailerLink");

    // direcciones de la DLL de los callbacks de RW (para reconocerlos en G2CbId)
    g2DllSetFlag   = Body(G2PA(&CVehicleModelInfo::SetAtomicFlagCB));
    g2DllClearFlag = Body(G2PA(&CVehicleModelInfo::ClearAtomicFlagCB));
    g2DllHideAll   = Body(G2PA(&CVehicleModelInfo::HideAllComponentsAtomicCB));
    g2DllCurAtom   = Body(uintptr_t(&GetCurrentAtomicObjectCB));
    // vtable falsa de los CObject falsos
    for (auto& v : g2ObjVt) v = reinterpret_cast<uintptr_t>(&S2_ObjTrap);
    g2ObjVt[4] = reinterpret_cast<uintptr_t>(&S2_ObjSetStatic);
    g2ObjVt[6] = reinterpret_cast<uintptr_t>(&S2_ObjSetModel);
    return miss;
}

inline void InitG2() {
    Reg(&g2b, sizeof(G2Buf));
    Save(0xB7449C, 4);                   // CPools::ms_pObjectPool (se intercambia por un pool falso)
    Save(0xBB4A70, 4);                   // CObject::nNoTempObjects (word) y su vecino
    Save(0xB7CB84, 4);                   // CTimer::m_snTimeInMilliseconds
    Save(0xB7CB5C, 4);                   // CTimer::ms_fTimeStep
    Save(0xC1BFD0, 4);                   // CAutomobile::m_sAllTaxiLights (bool)
    Save(0xB6F028, 0x20);                // TheCamera: vptr, posicion, rumbo y puntero a matriz
    Save(MODELPTRS + 4u * 374u, 4u * 8u);  // model infos TEMPCOL (374..381)
}

inline bool LayoutG2(json& l) {
    bool ok = true;
    auto chk = [&](const char* n, size_t got, size_t want) { const bool g = got == want; ok &= g; l[std::string("g2_") + n] = g; };
    chk("doors", offsetof(CAutomobile, m_doors), G2_O_DOORS); chk("nodes", offsetof(CAutomobile, m_aCarNodes), G2_O_NODES);
    chk("panels", offsetof(CAutomobile, m_panels), G2_O_PANELS); chk("chassis", offsetof(CAutomobile, m_swingingChassis), G2_O_CHASSIS);
    chk("sizeof_door", sizeof(CDoor), 0x18); chk("sizeof_panel", sizeof(CBouncingPanel), 0x20); chk("sizeof_dmg", sizeof(CDamageManager), 0x18);
    chk("door_angle", offsetof(CDoor, m_angle), 0xC); chk("door_angvel", offsetof(CDoor, m_angVel), 0x14); chk("door_axis", offsetof(CDoor, m_axis), 0xA);
    chk("susp_spring", offsetof(CAutomobile, m_aSuspensionSpringLength), 0x878); chk("susp_line", offsetof(CAutomobile, m_aSuspensionLineLength), 0x888);
    chk("front_height", offsetof(CAutomobile, m_fFrontHeightAboveRoad), 0x898); chk("wheel_pos", offsetof(CAutomobile, m_wheelPosition), 0x838);
    chk("misc_angle", offsetof(CAutomobile, m_wMiscComponentAngle), G2_O_MISCANGLE); chk("nitro", offsetof(CAutomobile, m_exhaustNitroFxSystem), G2_O_NITRO);
    chk("fire", offsetof(CAutomobile, m_pFireParticle), G2_O_FIRE);
    chk("towing", offsetof(CAutomobile, m_pTowingVehicle), G2_O_TOWING); chk("towed", offsetof(CAutomobile, m_pVehicleBeingTowed), G2_O_TOWED);
    chk("sizeof_obj_buf", sizeof(g2b.obj[0]) >= 0x17C ? 1 : 0, 1);
    // handling falso y model info / estructura de vehiculo: desplazamientos que usan las fixtures y el EXE
    chk("hd_upper", offsetof(tHandlingData, m_fSuspensionUpperLimit), 0xB8); chk("hd_lower", offsetof(tHandlingData, m_fSuspensionLowerLimit), 0xBC);
    chk("hd_force", offsetof(tHandlingData, m_fSuspensionForceLevel), 0xAC); chk("hd_modelflags", offsetof(tHandlingData, m_nModelFlags), 0xCC);
    chk("hd_flags", offsetof(tHandlingData, m_nHandlingFlags), 0xD0);
    chk("mi_wheel_front", offsetof(CVehicleModelInfo, m_fWheelSizeFront), 0x40); chk("mi_wheel_rear", offsetof(CVehicleModelInfo, m_fWheelSizeRear), 0x44);
    chk("mi_handling_id", offsetof(CVehicleModelInfo, m_nHandlingId), 0x4A); chk("mi_num_doors", offsetof(CVehicleModelInfo, m_nNumDoors), 0x4C);
    chk("mi_struct", offsetof(CVehicleModelInfo, m_pVehicleStruct), 0x5C);
    chk("vs_mask", offsetof(CVehicleModelInfo::CVehicleStructure, m_nMaskComponentsDamagable), 0x310);
    chk("obj_type", offsetof(CObject, m_nObjectType), 0x13C); chk("obj_removal", offsetof(CObject, m_nRemovalTime), 0x150);
    chk("obj_ignored", offsetof(CObject, m_pEntityIgnoredCollision), 0x128);
    chk("phys_dmg_int", offsetof(CPhysical, m_fDamageIntensity), G2_O_DMGINT); chk("phys_bright", offsetof(CPhysical, m_fContactSurfaceBrightness), G2_O_BRIGHT);
    chk("veh_doorlock", offsetof(CVehicle, m_nDoorLock), G2_O_DOORLOCK); chk("veh_colors", offsetof(CVehicle, m_nPrimaryColor), G2_O_COLORS);
    chk("veh_audio", offsetof(CVehicle, m_vehicleAudio), G2_O_AUDIO); chk("veh_hflags", offsetof(CVehicle, m_nHandlingFlagsIntValue), G2_O_HFLAGS);
    chk("veh_vtype", offsetof(CVehicle, m_nVehicleType), G2_O_VTYPE); chk("veh_vsub", offsetof(CVehicle, m_nVehicleSubType), G2_O_VSUB);
    chk("veh_vflags", offsetof(CVehicle, m_nVehicleUpperFlags), G2_O_VFLAGS);
    return ok;
}

} // namespace

#include "CasesG2_a.hpp"
#include "CasesG2_b.hpp"
#include "CasesG2_c.hpp"

namespace AutomobileFinal4Live {
inline bool RunG2(Ctx& c) {
    const std::string& n = c.nm;
    if (n == "Constructor")                  return G2RunCtor(c);
    if (n == "Destructor")                   return G2RunDtor(c);
    if (n == "SetupSuspensionLines")         return G2RunSetupSusp(c);
    if (n == "Fix")                          return G2RunFix(c);
    if (n == "SetupDamageAfterLoad")         return G2RunSetupDamage(c);
    if (n == "FixTyre")                      return G2RunFixTyre(c);
    if (n == "FixDoor")                      return G2RunFixDoor(c);
    if (n == "FixPanel")                     return G2RunFixPanel(c);
    if (n == "SetTotalDamage")               return G2RunSetTotalDamage(c);
    if (n == "PopDoor")                      return G2RunPopDoor(c);
    if (n == "PopPanel")                     return G2RunPopPanel(c);
    if (n == "PopBoot")                      return G2RunPopBoot(c);
    if (n == "CloseBoot")                    return G2RunCloseBoot(c);
    if (n == "PopBootUsingPhysics")          return G2RunPopBootPhysics(c);
    if (n == "CloseAllDoors")                return G2RunCloseAllDoors(c);
    if (n == "SetBumperDamage")              return G2RunSetBumperDamage(c);
    if (n == "SetPanelDamage")               return G2RunSetPanelDamage(c);
    if (n == "SetDoorDamage")                return G2RunSetDoorDamage(c);
    if (n == "OpenDoor")                     return G2RunOpenDoor(c);
    if (n == "ProcessSwingingDoor")          return G2RunSwingingDoor(c);
    if (n == "SpawnFlyingComponent")         return G2RunSpawn(c);
    if (n == "RemoveBonnetInPedCollision")   return G2RunRemoveBonnet(c);
    if (n == "dmgDrawCarCollidingParticles") return G2RunDmgParticles(c);
    if (n == "SetTowLink")                   return G2RunSetTowLink(c);
    return false;
}
} // namespace
