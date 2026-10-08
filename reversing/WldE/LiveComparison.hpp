#pragma once
// Banco dentro del motor: CWorld, grupo E (20 funciones): ResetLineTestOptions, SetAllCarsCanBeDamaged,
// StopAllLawEnforcersInTheirTracks, UseDetonator, SetPedsOnFire, SetPedsChoking, SetCarsOnFire, SetWorldOnFire,
// SprayPaintWorld, TestForBuildingsOnTopOfEachOther (void y lista), TestForUnusedModels (void y lista),
// TestSphereAgainstSectorList, TestSphereAgainstWorld, TriggerExplosionSectorList, TriggerExplosion,
// RepositionOneObject, RepositionCertainDynamicObjects y ShutDown.
// Original (hooks del grupo desactivados) frente a candidato (DLL) con fixtures identicos y comparacion bit a bit
// (trazas de llamadas externas + hash de todos los bloques de memoria tocados).
// Mundo FALSO: los pools de entidades (vehiculos, peatones, objetos, edificios, dummies) y los de nodos (simple y doble)
// se sustituyen temporalmente por pools locales; las listas de sectores, de repeticion y LOD, las listas de entidades en
// movimiento y los modelos 19900..19939 se rellenan con memoria propia (la DLL de depuracion valida nodos e Items contra los
// pools). Nada del juego real se opera: se guardan y restauran los punteros de pools, las listas globales, el scan code,
// las variables ModelIndices, el temporizador, gString, las variables de CWorld de las pruebas de linea, etc.
// Simulados y trazados (en la direccion del EXE y en el simbolo de la DLL): StartFire/GetNumFiresInRange, GenerateDamageEvent,
// GetLocalDirection, RegisterReference/CleanUpOldReference, RemoveDetonatorProjectiles, ProcessVerticalLine, UpdateRwMatrix/UpdateRwFrame,
// FindObjectsInRange, CTagManager, CameraToIgnoreThisObject, ProcessColModels, FindPlayerPed, TryToExplode, CGlass, AddToMovingList,
// RemoveFromMovingList, TriggerWaterHydrant, ObjectDamage, CEventGroup::Add, ComputeDamageResponse, ReportCrime, InflictDamage,
// SwitchVehicleToRealPhysics, ProcessSphereBox, GetSpeed, ApplyForce/ApplyMoveForce/ApplyTurnForce, CIplStore::Shutdown y
// CPlayerPedData::DeAllocateData; las llamadas virtuales a SetIsStatic/Remove/destructor/VehicleDamage van a una vtable falsa.
#include <fstream>
#include <functional>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity.h"
#include "Physical.h"
#include "Ped.h"
#include "Building.h"
#include "Dummy.h"
#include "CutsceneMgr.h"
#include "Object.h"
#include "Vehicle.h"
#include "World.h"
#include "Fire.h"
#include "FireManager.h"
#include "PlayerPed.h"
#include "Weapon.h"
#include "Glass.h"
#include "TagManager.h"
#include "Crime.h"
#include "CarCtrl.h"
#include "ProjectileInfo.h"
#include "IplStore.h"
#include "PlayerPedData.h"
#include "Timer.h"
#include "Fx/Fx.h"
#include "Events/EventDamage.h"
#include "Events/EventKnockOffBike.h"
#include "Events/EventGroup.h"
#include "PedDamageResponseCalculator.h"
#include "Collision/Collision.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace WldELive {
inline bool done{}; inline std::ofstream log;
inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class V=uint32> inline V& At(void* base,size_t off) { return *reinterpret_cast<V*>(static_cast<uint8*>(base)+off); }
template<class V=uint32> inline V& AtA(uintptr_t addr) { return *reinterpret_cast<V*>(addr); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// --- direcciones del EXE ---
constexpr uintptr_t MI_PTRS=0xA9B0C8, POOLS=0xB74484, SECTORS=0xB7D0B8, REPEAT=0xB992B8, LODL=0xB99EB8, SCAN=0xB7CD78, MOVING=0xB9ACC8;
constexpr uintptr_t MIVARS=0x8CD4F0, LINEOPT=0xB7CD60, PLAYERS=0xB7CD98, GSTRING=0xB71670, TIMESTEP=0xB7CB5C, TIMEMS=0xB7CB84, SPRAYVAR=0x8CDEF0;
constexpr size_t SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12, LOD_SIZE=30*30*4, POOLS_SIZE=0x20, MIVARS_SIZE=0x250, PLAYERS_SIZE=0x320;
constexpr int MODEL0=19900, NM=40;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
constexpr size_t NV=6, NP=8, NO=6, NB=6, ND=6;            // NP: 7 peatones de pool y uno "jugador" fuera del pool
constexpr size_t SV=0xA18, SP=0x7C4, SO=0x19C, SB=0x38;   // tamanos de los slots del EXE (objetos: CCutsceneObject, 0x19C)
constexpr size_t NPOOLPED=NP-1;
alignas(16) inline std::array<uint8,SV*NV> vehMem{};
alignas(16) inline std::array<uint8,SP*NP> pedMem{};
alignas(16) inline std::array<uint8,SO*NO> objMem{};
alignas(16) inline std::array<uint8,SB*NB> bldMem{};
alignas(16) inline std::array<uint8,SB*ND> dumMem{};
inline std::array<uint8,NV> vehSt{}; inline std::array<uint8,NP> pedSt{}; inline std::array<uint8,NO> objSt{}; inline std::array<uint8,NB> bldSt{}; inline std::array<uint8,ND> dumSt{};
alignas(16) inline std::array<uint8,0x14> vehHdr{}, pedHdr{}, objHdr{}, bldHdr{}, dumHdr{};
inline uint8* Veh(size_t k) { return vehMem.data()+k*SV; }
inline uint8* Ped(size_t k) { return pedMem.data()+k*SP; }
inline uint8* Obj(size_t k) { return objMem.data()+k*SO; }
inline uint8* Bld(size_t k) { return bldMem.data()+k*SB; }
inline uint8* Dum(size_t k) { return dumMem.data()+k*SB; }
constexpr size_t NMAT=64;
alignas(16) inline std::array<std::array<uint8,0x60>,NMAT> mats{}; inline size_t matNext{};
alignas(16) inline std::array<std::array<uint8,0x400>,NP> intel{};
alignas(16) inline std::array<uint8,0x40> rwObj{}, rwFrame{};
alignas(16) inline std::array<std::array<uint8,0x40>,NM> miBuf{};
alignas(16) inline std::array<std::array<uint8,0x30>,NM> colBuf{};
alignas(16) inline std::array<std::array<uint8,0x20>,NM> cdBuf{};
alignas(16) inline std::array<std::array<std::array<uint8,0x14>,3>,NM> sphBuf{};
alignas(16) inline std::array<std::array<std::array<uint8,0x1C>,2>,NM> boxBuf{};
alignas(16) inline std::array<std::array<uint8,0x40>,NM> oiBuf{};     // CObjectData falsos (uno por modelo)
constexpr size_t NDBL=8192, NSGL=8192;
alignas(16) inline std::array<uint8,NDBL*12> dblStore{}; inline std::array<uint8,NDBL> dblStates{};
alignas(16) inline std::array<uint8,NSGL*8> sglStore{}; inline std::array<uint8,NSGL> sglStates{};
alignas(16) inline std::array<uint8,0x20> dblHdr{}, sglHdr{}, dblImg{}, sglImg{};
inline std::array<void*,64> entVt{};

inline std::string NameS(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    struct R { const char* tag; uintptr_t b; size_t size; size_t stride; };
    const R rs[]{{"veh",uintptr_t(vehMem.data()),vehMem.size(),SV},{"ped",uintptr_t(pedMem.data()),pedMem.size(),SP},{"obj",uintptr_t(objMem.data()),objMem.size(),SO},
                 {"bld",uintptr_t(bldMem.data()),bldMem.size(),SB},{"dum",uintptr_t(dumMem.data()),dumMem.size(),SB},{"mat",uintptr_t(mats.data()),sizeof(mats),0x60},
                 {"intel",uintptr_t(intel.data()),sizeof(intel),0x400},{"mi",uintptr_t(miBuf.data()),sizeof(miBuf),0x40},{"col",uintptr_t(colBuf.data()),sizeof(colBuf),0x30},
                 {"cd",uintptr_t(cdBuf.data()),sizeof(cdBuf),0x20},{"oi",uintptr_t(oiBuf.data()),sizeof(oiBuf),0x40},{"rw",uintptr_t(rwObj.data()),rwObj.size(),0x40},
                 {"player",PLAYERS,PLAYERS_SIZE,0x190}};
    for(auto& r:rs)if(u>=r.b&&u<r.b+r.size)return std::format("{}#{}+{:x}",r.tag,(u-r.b)/r.stride,(u-r.b)%r.stride);
    return std::format("{:08x}",u);
}
inline json Name(const void* p) { return NameS(p); }
// Como NameS pero sin direcciones sueltas (p. ej. de la pila, distintas en cada ejecucion): "ext"
inline std::string NameA(const void* p) { const auto s=NameS(p); return s.find('#')==std::string::npos&&s!="null" ? std::string("ext") : s; }

template<int K> void __fastcall VTrap(void* self,void*) {
    Write({{"event","error"},{"reason","unexpected virtual call"},{"slot",K},{"self",NameS(self)}}); log.flush(); ExitProcess(95);
}
template<int... K> std::array<void*,sizeof...(K)> TrapTable(std::integer_sequence<int,K...>) { return {reinterpret_cast<void*>(&VTrap<K>)...}; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=20;
inline const std::array<Fn,NFNS> fns{{
    {"CWorld","ResetLineTestOptions",0x5631C0,0,-1,64},{"CWorld","SetAllCarsCanBeDamaged",0x5668F0,0,-1,256},{"CWorld","StopAllLawEnforcersInTheirTracks",0x566C10,0,-1,256},
    {"CWorld","UseDetonator",0x5660B0,0,-1,384},{"CWorld","SetPedsOnFire",0x565610,0,-1,640},{"CWorld","SetPedsChoking",0x565800,0,-1,512},
    {"CWorld","SetCarsOnFire",0x5659F0,0,-1,512},{"CWorld","SetWorldOnFire",0x56B910,0,-1,256},{"CWorld","SprayPaintWorld",0x565B70,0,-1,640},
    {"CWorld","TestForBuildingsOnTopOfEachOther-Void",0x5664A0,0,-1,128},{"CWorld","TestForBuildingsOnTopOfEachOther<CPtrListDoubleLink<CPhysical*>>-",0x563950,0,-1,256},
    {"CWorld","TestForUnusedModels-",0x566510,0,-1,128},{"CWorld","TestForUnusedModels<CPtrListDoubleLink<CPhysical*>>-InputArray",0x5639D0,0,-1,256},
    {"CWorld","TestSphereAgainstSectorList<CPtrListDoubleLink<CPhysical*>>",0x566140,0,-1,640},{"CWorld","TestSphereAgainstWorld",0x569E20,0,-1,640},
    {"CWorld","TriggerExplosionSectorList<CPtrListDoubleLink<CPhysical*>>",0x567750,0,-1,1024},{"CWorld","TriggerExplosion",0x56B790,0,-1,768},
    {"CWorld","RepositionOneObject",0x569850,0,-1,768},{"CWorld","RepositionCertainDynamicObjects",0x56B9C0,0,-1,256},{"CWorld","ShutDown",0x564050,0,-1,128},
}};

// ---------------------------------------------------------------------------------------------------------------------
// Guion de decisiones de los stubs (deterministico por caso) y estado de fixtures leido por los stubs
inline std::array<uint32,128> script{}; inline size_t scriptPos{};
inline uint32 SN() { return script[scriptPos++%script.size()]; }
inline float SF(float a,float b) { return a+(b-a)*float(SN()%1000000)/1000000.f; }
inline CPlayerPed* playerResult{};
inline uint32 numFires{};
inline CEntity* twResult{};
inline int foundCount{}; inline std::array<CEntity*,16> foundList{};
inline std::array<bool,16> tagFlag{}; inline std::array<uint8,16> tagAlpha{};
inline int ObjIdx(const void* e) { const auto d=intptr_t(e)-intptr_t(objMem.data()); return d>=0&&d<intptr_t(objMem.size()) ? int(d/intptr_t(SO)) : 15; }

// --- fuego ---
inline CFire* __fastcall FakeStartFireEnt(CFireManager*,void*,CEntity* t,CEntity* c,float size,uint8 a3,uint32 time,int8 gens) {
    trace.push_back(json::array({"start_fire_ent",NameS(t),NameS(c),F(size),int(a3),time,int(gens)})); return nullptr; }
inline CFire* __fastcall FakeStartFirePos(CFireManager*,void*,CVector pos,float size,uint8 u1,CEntity* c,uint32 time,int8 gens,uint8 u2) {
    trace.push_back(json::array({"start_fire_pos",F(pos.x),F(pos.y),F(pos.z),F(size),int(u1),NameS(c),time,int(gens),int(u2)})); return nullptr; }
inline uint32 __fastcall FakeGetNumFires(CFireManager*,void*,const CVector& p,float r) {
    trace.push_back(json::array({"get_num_fires",F(p.x),F(p.y),F(p.z),F(r)})); return numFires; }
// --- peatones / eventos ---
inline bool __cdecl FakeGenerateDamageEvent(CPed* v,CEntity* c,eWeaponType w,int32 dmg,ePedPieceTypes piece,uint8 dir) {
    trace.push_back(json::array({"generate_damage_event",NameS(v),NameS(c),int(w),dmg,int(piece),int(dir)})); return true; }
inline int32 __fastcall FakeGetLocalDirection(CPed* self,void*,const CVector2D& p) {
    const int32 r=int32((F(p.x)*31u+F(p.y))&0xFFu); trace.push_back(json::array({"get_local_direction",NameS(self),F(p.x),F(p.y)})); return r; }
inline void __fastcall FakeRegisterReference(CEntity* self,void*,CEntity** ref) {
    trace.push_back(json::array({"register_reference",NameS(self),NameA(ref),NameS(ref ? *ref : nullptr)})); }
inline void __fastcall FakeCleanUpOldReference(CEntity* self,void*,CEntity** ref) {
    trace.push_back(json::array({"clean_up_old_reference",NameS(self),NameA(ref),NameS(ref ? *ref : nullptr)})); }
inline void __cdecl FakeRemoveDetonatorProjectiles() { trace.push_back(json::array({"remove_detonator_projectiles"})); }
inline CEvent* __fastcall FakeEventGroupAdd(CEventGroup* self,void*,CEvent* ev,bool valid) {
    json r=json::array({"event_add",NameS(self),int(ev->GetEventType()),valid});
    if(ev->GetEventType()==EVENT_KNOCK_OFF_BIKE) {
        auto* k=static_cast<CEventKnockOffBike*>(ev);
        for(float v:{k->m_moveSpeed.x,k->m_moveSpeed.y,k->m_moveSpeed.z,k->m_collisionImpactVelocity.x,k->m_collisionImpactVelocity.y,k->m_collisionImpactVelocity.z,k->m_damageIntensity,k->field_28})r.push_back(F(v));
        r.push_back(k->m_time); r.push_back(NameS(k->m_ped)); r.push_back(int(k->m_knockOffDirection)); r.push_back(int(k->m_isVictimDriver)); r.push_back(int(k->m_forceKnockOff));
        r.push_back(int(k->m_knockOffType)); r.push_back(int(k->m_exitDoor)); r.push_back(NameS(k->m_vehicle));
    } else if(ev->GetEventType()==EVENT_DAMAGE) {
        auto* d=static_cast<CEventDamage*>(ev);
        r.push_back(NameS(d->m_pSourceEntity)); r.push_back(d->m_nStartTime); r.push_back(int(d->m_weaponType)); r.push_back(int(d->m_pedPieceType)); r.push_back(int(d->m_ucDirection));
        r.push_back(int(d->m_bJumpedOutOfMovingCar)); r.push_back(int(d->m_bFallDown)); r.push_back(int(d->m_bAnimAdded)); r.push_back(int(d->m_bWitnessedInVehicle)); r.push_back(int(d->m_bStealthMode));
        r.push_back(F(d->m_damageResponse.m_fDamageHealth)); r.push_back(F(d->m_damageResponse.m_fDamageArmor)); r.push_back(int(d->m_damageResponse.m_bHealthZero));
        r.push_back(int(d->m_damageResponse.m_bForceDeath)); r.push_back(int(d->m_damageResponse.m_bDamageCalculated)); r.push_back(int(d->m_damageResponse.m_bCheckIfAffectsPed));
    }
    trace.push_back(r); return nullptr; }
inline void __fastcall FakeComputeDamageResponse(CPedDamageResponseCalculator* self,void*,CPed* ped,CPedDamageResponse& resp,bool speak) {
    trace.push_back(json::array({"compute_damage_response",NameS(self->m_pDamager),F(self->m_fDamageFactor),int(self->m_bodyPart),int(self->m_weaponType),int(self->m_bSpeak),NameS(ped),speak}));
    resp.m_bDamageCalculated=true; resp.m_fDamageHealth=self->m_fDamageFactor; }
inline void __cdecl FakeReportCrime(eCrimeType t,CEntity* e,CPed* p) { trace.push_back(json::array({"report_crime",int(t),NameS(e),NameS(p)})); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back(json::array({"find_player_ped",id})); return playerResult; }
// --- mundo ---
inline bool __cdecl FakeProcessVerticalLine(const CVector* o,float dist,CColPoint* cp,CEntity** ent,bool b,bool v,bool p,bool ob,bool d,bool see,void* poly) {
    const bool hit=SN()%4!=0; const float z=SF(-5.f,40.f);
    trace.push_back(json::array({"process_vertical_line",F(o->x),F(o->y),F(o->z),F(dist),b,v,p,ob,d,see,NameS(poly),hit,F(z)}));
    if(hit){ cp->m_vecPoint=CVector{o->x,o->y,z}; *ent=reinterpret_cast<CEntity*>(Bld(0)); }
    return hit; }
inline void __fastcall FakeUpdateRwMatrix(CEntity* self,void*) { trace.push_back(json::array({"update_rw_matrix",NameS(At<void*>(self,0x14))})); }
inline void __fastcall FakeUpdateRwFrame(CEntity* self,void*) { trace.push_back(json::array({"update_rw_frame",NameS(self)})); }
inline void __fastcall FakeMatrixUpdateRW(CMatrix* self,void*,void* rw) { trace.push_back(json::array({"update_rw_matrix",NameS(self)})); }
inline void __cdecl FakeFindObjectsInRange(const CVector* p,float r,bool b2,int16* outCount,int16 maxCount,CEntity** out,bool bl,bool v,bool pe,bool o,bool d) {
    trace.push_back(json::array({"find_objects_in_range",F(p->x),F(p->y),F(p->z),F(r),b2,int(maxCount),bl,v,pe,o,d}));
    *outCount=int16(foundCount); for(int k=0;k<std::clamp(foundCount,0,15);++k)out[k]=foundList[size_t(k)]; }
inline bool __cdecl FakeIsTag(CEntity* e) { const bool r=tagFlag[size_t(ObjIdx(e))]; trace.push_back(json::array({"is_tag",NameS(e),r})); return r; }
inline uint8 __cdecl FakeGetAlpha(CEntity* e) { const uint8 r=tagAlpha[size_t(ObjIdx(e))]; trace.push_back(json::array({"get_alpha",NameS(e),int(r)})); return r; }
inline void __cdecl FakeSetAlpha(CEntity* e,uint8 a) { tagAlpha[size_t(ObjIdx(e))]=a; trace.push_back(json::array({"set_alpha",NameS(e),int(a)})); }
inline bool __cdecl FakeCameraToIgnore(CEntity* e) { const bool r=SN()%3==0; trace.push_back(json::array({"camera_to_ignore",NameS(e),r})); return r; }
inline float MF(const void* m,size_t off) { return *reinterpret_cast<const float*>(static_cast<const uint8*>(m)+off); }
inline int32 __cdecl FakeProcessColModels(const CMatrix* a,CColModel* ca,const CMatrix* b,CColModel* cb,void* cps,CColPoint* lines,float* maxt,bool arg7) {
    json r=json::array({"process_col_models"});
    for(size_t o:{size_t(0),size_t(4),size_t(8),size_t(0x10),size_t(0x14),size_t(0x18),size_t(0x20),size_t(0x24),size_t(0x28),size_t(0x30),size_t(0x34),size_t(0x38)})r.push_back(F(MF(a,o))); // matriz de la esfera
    for(size_t o=0;o<0x28;o+=4)r.push_back(F(MF(ca,o)));                                         // caja + esfera delimitadoras del modelo de la esfera
    r.push_back(int(At<uint8>(ca,0x28))); r.push_back(int(At<uint8>(ca,0x29)&7)); r.push_back(At<void*>(ca,0x2C)!=nullptr);
    if(auto* cd=At<uint8*>(ca,0x2C)){
        r.push_back(int(At<uint16>(cd,0))); r.push_back(int(At<uint16>(cd,2))); r.push_back(int(At<uint16>(cd,4))); r.push_back(int(At<uint8>(cd,6)));
        if(At<uint16>(cd,0)>0){ auto* sp=At<uint8*>(cd,8); for(size_t o=0;o<0x10;o+=4)r.push_back(F(MF(sp,o))); /* +0x10 (superficie) queda sin escribir en el EXE: pila */ }
    }
    r.push_back(NameA(b)); r.push_back(NameA(cb)); r.push_back(uint32(uintptr_t(cps))); r.push_back(uint32(uintptr_t(lines))); r.push_back(uint32(uintptr_t(maxt))); r.push_back(arg7);
    for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})r.push_back(F(MF(b,o)));
    const int32 ret= SN()%4==0 ? int32(1+SN()%3) : 0; r.push_back(ret); trace.push_back(r); return ret; }
inline CEntity* __cdecl FakeTestSphereAgainstWorld(CVector c,float r,CEntity* ign,bool b,bool v,bool p,bool o,bool d,bool cam) {
    trace.push_back(json::array({"test_sphere_against_world",F(c.x),F(c.y),F(c.z),F(r),NameS(ign),b,v,p,o,d,cam})); return twResult; }
// --- explosiones ---
inline bool __fastcall FakeTryToExplode(CObject* self,void*) { trace.push_back(json::array({"try_to_explode",NameS(self)})); return false; }
inline bool __cdecl FakeIsObjectGlass(CEntity* e) { const bool r=SN()%4==0; trace.push_back(json::array({"is_object_glass",NameS(e),r})); return r; }
inline void __cdecl FakeWindowResponds(CEntity* e,CVector p) { trace.push_back(json::array({"window_responds_to_explosion",NameS(e),F(p.x),F(p.y),F(p.z)})); }
inline void __fastcall FakeAddToMovingList(CPhysical* self,void*) { trace.push_back(json::array({"add_to_moving_list",NameS(self)})); }
inline void __fastcall FakeRemoveFromMovingList(CPhysical* self,void*) { trace.push_back(json::array({"remove_from_moving_list",NameS(self)})); }
inline void __fastcall FakeTriggerWaterHydrant(Fx_c*,void*,const CVector& p) { trace.push_back(json::array({"trigger_water_hydrant",F(p.x),F(p.y),F(p.z)})); }
inline void __fastcall FakeObjectDamage(CObject* self,void*,float dmg,const CVector* o,const CVector* d,CEntity* damager,eWeaponType w) {
    trace.push_back(json::array({"object_damage",NameS(self),F(dmg),NameS(o),NameS(d),NameS(damager),int(w)})); }
inline void __fastcall FakeInflictDamage(CVehicle* self,void*,CEntity* damager,eWeaponType w,float inten,CVector c) {
    trace.push_back(json::array({"inflict_damage",NameS(self),NameS(damager),int(w),F(inten),F(c.x),F(c.y),F(c.z)})); }
inline void __cdecl FakeSwitchToRealPhysics(CVehicle* v) { trace.push_back(json::array({"switch_vehicle_to_real_physics",NameS(v)})); }
inline bool __cdecl FakeProcessSphereBox(const CColSphere* s,const CColBox* b,CColPoint* cp,float* maxT) {
    const bool hit=SN()%3!=0;
    json r=json::array({"process_sphere_box",F(s->m_fRadius),F(s->m_vecCenter.x),F(s->m_vecCenter.y),F(s->m_vecCenter.z),int(s->m_Surface.m_nMaterial),int(s->m_Surface.m_nPiece),int(s->m_Surface.m_nLighting.value),
                        F(b->m_vecMin.x),F(b->m_vecMin.y),F(b->m_vecMin.z),F(b->m_vecMax.x),F(b->m_vecMax.y),F(b->m_vecMax.z),int(b->m_Surface.m_nMaterial),int(b->m_Surface.m_nPiece),int(b->m_Surface.m_nLighting.value),F(*maxT),hit});
    // Como el original, un fallo deja el CColPoint sin tocar; aqui se rellena para que ambas ejecuciones lean el mismo valor
    cp->m_vecPoint=CVector{SF(-2.f,2.f),SF(-2.f,2.f),SF(-2.f,2.f)};
    CVector n{SF(-1.f,1.f),SF(-1.f,1.f),SF(-1.f,1.f)}; const float l=std::sqrt(n.x*n.x+n.y*n.y+n.z*n.z); if(l>0.05f)n=CVector{n.x/l,n.y/l,n.z/l}; cp->m_vecNormal=n;
    r.push_back(F(cp->m_vecPoint.x)); r.push_back(F(cp->m_vecPoint.y)); r.push_back(F(cp->m_vecPoint.z)); r.push_back(F(n.x)); r.push_back(F(n.y)); r.push_back(F(n.z));
    trace.push_back(r); return hit; }
inline CVector* __fastcall FakeGetSpeed(CPhysical* self,void*,CVector* ret,CVector p) {
    trace.push_back(json::array({"get_speed",NameS(self),F(p.x),F(p.y),F(p.z)}));
    *ret=CVector{p.x*0.25f+0.5f,p.y*0.5f-0.25f,p.z*0.125f+1.0f}; return ret; }
inline void __fastcall FakeApplyForce(CPhysical* self,void*,CVector f,CVector p,bool u) {
    trace.push_back(json::array({"apply_force",NameS(self),F(f.x),F(f.y),F(f.z),F(p.x),F(p.y),F(p.z),u})); }
inline void __fastcall FakeApplyMoveForce(CPhysical* self,void*,CVector f) { trace.push_back(json::array({"apply_move_force",NameS(self),F(f.x),F(f.y),F(f.z)})); }
inline void __fastcall FakeApplyTurnForce(CPhysical* self,void*,CVector f,CVector p) {
    trace.push_back(json::array({"apply_turn_force",NameS(self),F(f.x),F(f.y),F(f.z),F(p.x),F(p.y),F(p.z)})); }
// --- vtable falsa de las entidades ---
inline void __fastcall FakeSetIsStatic(CEntity* self,void*,bool v) {
    auto& fl=At<uint32>(self,0x1C); fl=(fl&~4u)|(v ? 4u : 0u); trace.push_back(json::array({"set_is_static",NameS(self),v})); }
inline void __fastcall FakeRemoveVt(CEntity* self,void*) { trace.push_back(json::array({"entity_remove",NameS(self)})); }
inline void* __fastcall FakeDeletingDtor(CEntity* self,void*,int flags) { trace.push_back(json::array({"entity_delete",NameS(self),flags})); return self; }
inline void __fastcall FakeVehicleDamage(CVehicle* self,void*,float dmg,eVehicleCollisionComponent comp,CEntity* damager,CVector* coors,CVector* dir,eWeaponType w) {
    json r=json::array({"vehicle_damage",NameS(self),F(dmg),int(comp),NameS(damager),int(w)});
    for(auto* v:{coors,dir}){ r.push_back(v!=nullptr); if(v){ r.push_back(F(v->x)); r.push_back(F(v->y)); r.push_back(F(v->z)); } }
    trace.push_back(r); }
// --- apagado ---
inline void __cdecl FakeIplShutdown() { trace.push_back(json::array({"ipl_shutdown"})); }
inline void __fastcall FakeDeAllocateData(void* self,void*) { trace.push_back(json::array({"player_deallocate_data",NameS(self)})); }

struct PatchSet {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;
    void Add(uintptr_t exe,uintptr_t dll,void* fn) { if(exe)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(exe,fn)); if(dll)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(dll,fn)); }
};
template<class P> inline uintptr_t Addr(P p) { return std::bit_cast<uintptr_t>(p); }
#define V(f) reinterpret_cast<void*>(&f)
inline void AddStubs(PatchSet& ps,const std::string& nm) {
    using std::bit_cast;
    const bool fire = nm=="SetPedsOnFire"||nm=="SetCarsOnFire";
    const bool exploding = nm.rfind("TriggerExplosion",0)==0;
    const bool sphere = nm.rfind("TestSphereAgainst",0)==0;
    const bool repos = nm.rfind("Reposition",0)==0;
    if(fire||exploding) ps.Add(0x53A050,Addr(static_cast<CFire*(CFireManager::*)(CEntity*,CEntity*,float,uint8,uint32,int8)>(&CFireManager::StartFire)),V(FakeStartFireEnt));
    if(nm=="SetWorldOnFire") {
        ps.Add(0x569E20,Addr(&CWorld::TestSphereAgainstWorld),V(FakeTestSphereAgainstWorld));
        ps.Add(0x5397F0,Addr(&CFireManager::GetNumFiresInRange),V(FakeGetNumFires));
        ps.Add(0x539F00,Addr(static_cast<CFire*(CFireManager::*)(CVector,float,uint8,CEntity*,uint32,int8,uint8)>(&CFireManager::StartFire)),V(FakeStartFirePos));
    }
    if(nm=="SetPedsChoking") {
        ps.Add(0x73A530,Addr(&CWeapon::GenerateDamageEvent),V(FakeGenerateDamageEvent));
        ps.Add(0x5DEF60,Addr(&CPed::GetLocalDirection),V(FakeGetLocalDirection));
    }
    if(nm=="UseDetonator"||exploding) {
        ps.Add(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),V(FakeRegisterReference));
    }
    if(exploding) {
        ps.Add(0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),V(FakeCleanUpOldReference));
        ps.Add(0x73A530,Addr(&CWeapon::GenerateDamageEvent),V(FakeGenerateDamageEvent));
        ps.Add(0x5DEF60,Addr(&CPed::GetLocalDirection),V(FakeGetLocalDirection));
        ps.Add(0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),V(FakeEventGroupAdd));
        ps.Add(0x4B5AC0,Addr(&CPedDamageResponseCalculator::ComputeDamageResponse),V(FakeComputeDamageResponse));
        ps.Add(0x532010,Addr(&CCrime::ReportCrime),V(FakeReportCrime));
        ps.Add(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),V(FakeFindPlayerPed));
        ps.Add(0x59F2D0,Addr(&CObject::TryToExplode),V(FakeTryToExplode));
        ps.Add(0x46A760,Addr(&CGlass::IsObjectGlass),V(FakeIsObjectGlass));
        ps.Add(0x71C1A0,Addr(&CGlass::WindowRespondsToExplosion),V(FakeWindowResponds));
        ps.Add(0x542800,Addr(&CPhysical::AddToMovingList),V(FakeAddToMovingList));
        ps.Add(0x4A0D70,Addr(&Fx_c::TriggerWaterHydrant),V(FakeTriggerWaterHydrant));
        ps.Add(0x5A0D90,Addr(&CObject::ObjectDamage),V(FakeObjectDamage));
        ps.Add(0x6D7C90,Addr(&CVehicle::InflictDamage),V(FakeInflictDamage));
        ps.Add(0x423FC0,Addr(&CCarCtrl::SwitchVehicleToRealPhysics),V(FakeSwitchToRealPhysics));
        ps.Add(0x412130,Addr(&CCollision::ProcessSphereBox),V(FakeProcessSphereBox));
        ps.Add(0x542CE0,Addr(&CPhysical::GetSpeed),V(FakeGetSpeed));
        ps.Add(0x542B50,Addr(&CPhysical::ApplyForce),V(FakeApplyForce));
        ps.Add(0x5429F0,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),V(FakeApplyMoveForce));
        ps.Add(0x542A50,Addr(&CPhysical::ApplyTurnForce),V(FakeApplyTurnForce));
    }
    if(nm=="UseDetonator") ps.Add(0x738860,Addr(&CProjectileInfo::RemoveDetonatorProjectiles),V(FakeRemoveDetonatorProjectiles));
    if(nm=="SprayPaintWorld") {
        ps.Add(0x564A20,Addr(&CWorld::FindObjectsInRange),V(FakeFindObjectsInRange));
        ps.Add(0x49CCE0,Addr(static_cast<bool(*)(const CEntity&)>(&CTagManager::IsTag)),V(FakeIsTag));
        ps.Add(0x49CF90,Addr(static_cast<uint8(*)(const CEntity&)>(&CTagManager::GetAlpha)),V(FakeGetAlpha));
        ps.Add(0x49CEC0,Addr(static_cast<void(*)(CEntity&,uint8)>(&CTagManager::SetAlpha)),V(FakeSetAlpha));
    }
    if(sphere) {
        ps.Add(0x563F40,Addr(&CWorld::CameraToIgnoreThisObject),V(FakeCameraToIgnore));
        ps.Add(0x4185C0,Addr(&CCollision::ProcessColModels),V(FakeProcessColModels));
    }
    if(repos) {
        ps.Add(0x5674E0,Addr(&CWorld::ProcessVerticalLine),V(FakeProcessVerticalLine));
        ps.Add(0x446F90,Addr(&CEntity::UpdateRwMatrix),V(FakeUpdateRwMatrix));
        ps.Add(0x532B00,Addr(&CEntity::UpdateRwFrame),V(FakeUpdateRwFrame));
        ps.Add(0x59AD70,0,V(FakeMatrixUpdateRW));
    }
    if(nm=="ShutDown") {
        ps.Add(0x405FA0,Addr(&CIplStore::Shutdown),V(FakeIplShutdown));
        ps.Add(0x56F890,Addr(&CPlayerPedData::DeAllocateData),V(FakeDeAllocateData));
        ps.Add(0x542860,Addr(&CPhysical::RemoveFromMovingList),V(FakeRemoveFromMovingList));
    }
}
#undef V

// comprobacion de la disposicion de campos de la DLL frente al EXE (solo direcciones y bits, sin leer nada mas)
inline std::string LayoutErrors() {
    std::string why;
    auto* veh=reinterpret_cast<CVehicle*>(Veh(0)); auto* ped=reinterpret_cast<CPed*>(Ped(0)); auto* obj=reinterpret_cast<CObject*>(Obj(0)); auto* ent=reinterpret_cast<CEntity*>(Bld(0));
    auto* mi=reinterpret_cast<CBaseModelInfo*>(miBuf[0].data()); auto* cm=reinterpret_cast<CColModel*>(colBuf[0].data()); auto* cd=reinterpret_cast<CCollisionData*>(cdBuf[0].data());
    auto chk=[&](const char* nm,const void* addr,const void* base,size_t off){ if(uintptr_t(addr)-uintptr_t(base)!=off){ why+=nm; why+=' '; } };
    auto chkAbs=[&](const char* nm,const void* addr,uintptr_t want){ if(uintptr_t(addr)!=want){ why+=nm; why+=' '; } };
    chk("m_placement",&ent->m_placement,ent,4); chk("m_matrix",&ent->m_matrix,ent,0x14); chk("m_nModelIndex",&ent->m_nModelIndex,ent,0x22);
    chk("m_vecMoveSpeed",&veh->m_vecMoveSpeed,veh,0x44); chk("m_fMass",&veh->m_fMass,veh,0x8C); chk("m_fTurnMass",&veh->m_fTurnMass,veh,0x90); chk("m_pAttachedTo",&ped->m_pAttachedTo,ped,0xFC);
    chk("veh.m_pDriver",&veh->m_pDriver,veh,0x460); chk("veh.m_pFire",&veh->m_pFire,veh,0x490); chk("veh.m_nVehicleSubType",&veh->m_nVehicleSubType,veh,0x594);
    chk("veh.m_pWhoInstalledBombOnMe",&veh->m_pWhoInstalledBombOnMe,veh,0x4CC); chk("veh.m_wBombTimer",&veh->m_wBombTimer,veh,0x4DE); chk("veh.m_pWhoDetonatedMe",&veh->m_pWhoDetonatedMe,veh,0x4E0);
    chk("veh.m_vehicleSpecialColIndex",&veh->m_vehicleSpecialColIndex,veh,0x48B);
    chk("ped.m_nPedState",&ped->m_nPedState,ped,0x530); chk("ped.m_nPedType",&ped->m_nPedType,ped,0x598); chk("ped.m_pFire",&ped->m_pFire,ped,0x730);
    chk("obj.m_fHealth",&obj->m_fHealth,obj,0x154); chk("obj.m_pObjectInfo",&obj->m_pObjectInfo,obj,0x160);
    chk("mi.m_pColModel",&mi->m_pColModel,mi,0x14); chk("cm.center",&cm->m_boundSphere.m_vecCenter,cm,0x18); chk("cm.radius",&cm->m_boundSphere.m_fRadius,cm,0x24); chk("cm.coldata",&cm->m_pColData,cm,0x2C);
    chk("cd.m_nNumBoxes",&cd->m_nNumBoxes,cd,2); chk("cd.m_pSpheres",&cd->m_pSpheres,cd,8); chk("cd.m_pBoxes",&cd->m_pBoxes,cd,0xC);
    chkAbs("sectors",&CWorld::ms_aSectors,SECTORS); chkAbs("repeat",&CWorld::ms_aRepeatSectors,REPEAT); chkAbs("lod",&CWorld::ms_aLodPtrLists,LODL); chkAbs("scan",&CWorld::ms_nCurrentScanCode,SCAN);
    chkAbs("moving",&CWorld::ms_listMovingEntityPtrs,MOVING); chkAbs("players",&CWorld::Players,PLAYERS);
    if(sizeof(CRepeatSector)!=12)why+="sizeof(CRepeatSector) "; if(sizeof(CSector)!=8)why+="sizeof(CSector) ";
    if(sizeof(CObject)!=0x17C)why+="sizeof(CObject) "; if(sizeof(CEventKnockOffBike)!=0x3C)why+="sizeof(CEventKnockOffBike) ";
    // bits: se escribe el valor crudo y se lee por los accesores de la DLL
    auto bitProbe=[&](const char* nm,void* base,size_t off,uint32 mask,const std::function<bool()>& read){
        auto& w=At<uint32>(base,off); const uint32 save=w; w=mask; const bool a=read(); w=0; const bool b=read(); w=save; if(!a||b){ why+=nm; why+=' '; } };
    { auto& w=At<uint32>(Bld(0),0x18); const uint32 save=w; w=0x1234; const bool ok=reinterpret_cast<uintptr_t>(ent->GetRwObject())==0x1234; w=save; if(!ok)why+="m_pRwObject "; }
    bitProbe("usesCollision",Bld(0),0x1C,1u,[&]{return ent->GetUsesCollision();});
    bitProbe("isStatic",Bld(0),0x1C,4u,[&]{return ent->m_bIsStatic;});
    bitProbe("staticWaiting",Bld(0),0x1C,0x40000u,[&]{return ent->m_bIsStaticWaitingForCollision;});
    bitProbe("bDisableCollisionForce",Veh(0),0x40,4u,[&]{return veh->physicalFlags.bDisableCollisionForce;});
    bitProbe("bDisableZ",Veh(0),0x40,0x80u,[&]{return veh->physicalFlags.bDisableZ;});
    bitProbe("bFireProof",Veh(0),0x40,0x80000u,[&]{return veh->physicalFlags.bFireProof;});
    bitProbe("bInvulnerable",Veh(0),0x40,0x400000u,[&]{return veh->physicalFlags.bInvulnerable;});
    bitProbe("bExplosionProof",Veh(0),0x40,0x800000u,[&]{return veh->physicalFlags.bExplosionProof;});
    bitProbe("bIsLawEnforcer",Veh(0),0x428,1u,[&]{return veh->vehicleFlags.bIsLawEnforcer;});
    bitProbe("bCanBeDamaged",Veh(0),0x428,0x200000u,[&]{return veh->vehicleFlags.bCanBeDamaged;});
    bitProbe("bIsStanding",Ped(0),0x46C,1u,[&]{return ped->bIsStanding;});
    bitProbe("bInVehicle",Ped(0),0x46C,0x100u,[&]{return ped->bInVehicle;});
    bitProbe("bIsExploded",Obj(0),0x140,0x40u,[&]{return obj->objectFlags.bIsExploded;});
    { auto& w=At<uint8>(Ped(0),0x484); const uint8 save=w; w=2; const bool a=ped->IsCreatedByMission(); w=0; const bool b=ped->IsCreatedByMission(); w=save; if(!a||b)why+="IsCreatedByMission "; }
    { auto& w=At<uint8>(Veh(0),0x4A8); const uint8 save=w; w=3; const bool a=veh->m_nBombOnBoard==3; w=0xF8; const bool b=veh->m_nBombOnBoard==0; w=save; if(!a||!b)why+="m_nBombOnBoard "; }
    // tamanos de los slots de los pools reales (solo lectura)
    if(GetVehiclePool()->GetObjectSize()!=SV)why+="vehicle pool size "; if(GetPedPool()->GetObjectSize()!=SP)why+="ped pool size ";
    if(GetObjectPool()->GetObjectSize()!=SO)why+="object pool size ";
    return why;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"wlde-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"wlde-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("wlde");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"wlde-only.txt"); if(f)std::getline(f,only); }
    if(auto bad=LayoutErrors(); !bad.empty()){Write({{"event","error"},{"reason","layout mismatch"},{"fields",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };

    { const auto traps=TrapTable(std::make_integer_sequence<int,64>{}); for(int k=0;k<64;++k)entVt[size_t(k)]=traps[size_t(k)]; }
    entVt[0]=reinterpret_cast<void*>(&FakeDeletingDtor); entVt[3]=reinterpret_cast<void*>(&FakeRemoveVt); entVt[4]=reinterpret_cast<void*>(&FakeSetIsStatic);
    entVt[56]=reinterpret_cast<void*>(&FakeVehicleDamage);

    // --- estado global guardado ---
    struct Blob { uintptr_t at; std::vector<uint8> data; };
    std::vector<Blob> blobs;
    auto Save=[&](uintptr_t at,size_t n){ blobs.push_back({at,std::vector<uint8>(reinterpret_cast<uint8*>(at),reinterpret_cast<uint8*>(at)+n)}); };
    Save(MI_PTRS+size_t(MODEL0)*4,size_t(NM)*4); Save(POOLS,POOLS_SIZE); Save(SECTORS,SECTORS_SIZE); Save(REPEAT,REPEAT_SIZE); Save(LODL,LOD_SIZE); Save(SCAN,2);
    Save(MOVING,8); Save(MIVARS,MIVARS_SIZE); Save(LINEOPT,0x20); Save(PLAYERS,PLAYERS_SIZE); Save(GSTRING,0x100); Save(TIMESTEP,4); Save(TIMEMS,4); Save(SPRAYVAR,2);
    auto RestoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size()); };
    auto Restored=[&]{ bool ok=true; for(auto& b:blobs)ok&=std::memcmp(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size())==0; return ok; };

    // pools de nodos locales (cabecera construida una sola vez) y cabeceras de los pools de entidades falsos
    new (dblHdr.data()) CPtrNodeDoubleLinkPool(NDBL,dblStore.data(),dblStates.data()); dblImg=dblHdr;
    new (sglHdr.data()) CPtrNodeSingleLinkPool(NSGL,sglStore.data(),sglStates.data()); sglImg=sglHdr;
    auto PoolHdr=[&](std::array<uint8,0x14>& h,void* mem,void* states,int32 size){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=mem; At<void*>(h.data(),4)=states; At<int32>(h.data(),8)=size; At<int32>(h.data(),0xC)=-1; };
    auto* const poolPtrs=reinterpret_cast<void**>(POOLS); // 0 simple, 1 doble, 2 entry, 3 peatones, 4 vehiculos, 5 edificios, 6 objetos, 7 dummies
    constexpr uintptr_t MIVAR_A[]{0x8CD548,0x8CD560,0x8CD564,0x8CD568,0x8CD56C,0x8CD570,0x8CD574,0x8CD578,0x8CD57C,0x8CD580,0x8CD584,0x8CD588,0x8CD58C,0x8CD590,0x8CD594,0x8CD5FC,0x8CD60C,0x8CD54C,0x8CD620,0x8CD628,0x8CD618};
    constexpr uintptr_t MIVAR_B[]{0x8CD518,0x8CD51C,0x8CD520,0x8CD4FC,0x8CD500,0x8CD504,0x8CD508,0x8CD50C,0x8CD510,0x8CD614,0x8CD528,0x8CD52C};
    constexpr uintptr_t MIVAR_BUOY=0x8CD608, MIVAR_FENCE1=0x8CD600, MIVAR_FENCE2=0x8CD604, MIVAR_ATM=0x8CD710, MIVAR_HYDRANT=0x8CD594;

    bool passed=true; int pair=0;
    {
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            // las funciones que recorren los pools de entidades toleran huecos libres; las que usan listas de sectores o de nodos necesitan
            // que todas las entidades estén vivas en su pool (la DLL de depuración lo valida al enlazar los nodos)
            const bool poolIter = nm=="SetAllCarsCanBeDamaged"||nm=="StopAllLawEnforcersInTheirTracks"||nm=="UseDetonator"||nm=="SetPedsOnFire"||nm=="SetPedsChoking"||nm=="SetCarsOnFire"
                                  ||nm=="SprayPaintWorld"||nm=="RepositionCertainDynamicObjects"||nm=="RepositionOneObject"||nm=="ResetLineTestOptions"||nm=="SetWorldOnFire";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3E3Eu; if(!st)st=1;
                    trace=json::array(); scriptPos=0; for(auto& s:script)s=next();
                    RestoreAll();
                    std::memcpy(dblHdr.data(),dblImg.data(),dblImg.size()); dblStore.fill(0xFD); dblStates.fill(0x80);
                    std::memcpy(sglHdr.data(),sglImg.data(),sglImg.size()); sglStore.fill(0xFD); sglStates.fill(0x80);
                    vehMem.fill(0); pedMem.fill(0); objMem.fill(0); bldMem.fill(0); dumMem.fill(0); for(auto& m:mats)m.fill(0); for(auto& m:intel)m.fill(0); matNext=0;
                    rwObj.fill(0); rwFrame.fill(0); At<void*>(rwObj.data(),4)=rwFrame.data();
                    // slots de los pools: vivos con identificador aleatorio o libres (0x80)
                    auto States=[&](std::span<uint8> s,size_t poolSize){ for(size_t k=0;k<s.size();++k)s[k]=k>=poolSize ? uint8(0x80) : ((poolIter&&next()%5==0) ? uint8(0x80|(next()&0x7F)) : uint8(next()&0x7F)); };
                    States(vehSt,NV); States(pedSt,NPOOLPED); States(objSt,NO); States(bldSt,NB); States(dumSt,ND);
                    PoolHdr(vehHdr,vehMem.data(),vehSt.data(),int32(NV)); PoolHdr(pedHdr,pedMem.data(),pedSt.data(),int32(NPOOLPED)); PoolHdr(objHdr,objMem.data(),objSt.data(),int32(NO));
                    PoolHdr(bldHdr,bldMem.data(),bldSt.data(),int32(NB)); PoolHdr(dumHdr,dumMem.data(),dumSt.data(),int32(ND));
                    poolPtrs[0]=sglHdr.data(); poolPtrs[1]=dblHdr.data(); poolPtrs[3]=pedHdr.data(); poolPtrs[4]=vehHdr.data(); poolPtrs[5]=bldHdr.data(); poolPtrs[6]=objHdr.data(); poolPtrs[7]=dumHdr.data();
                    std::memset(reinterpret_cast<void*>(SECTORS),0,SECTORS_SIZE); std::memset(reinterpret_cast<void*>(REPEAT),0,REPEAT_SIZE); std::memset(reinterpret_cast<void*>(LODL),0,LOD_SIZE);
                    AtA<uint32>(MOVING)=0; AtA<uint32>(MOVING+4)=0;
                    playerResult=reinterpret_cast<CPlayerPed*>(Ped(NP-1)); numFires=0; twResult=nullptr; foundCount=0; tagFlag.fill(false); tagAlpha.fill(0);
                    { const uint32 m=next()%30; AtA<uint16>(SCAN)= m==0 ? uint16(0xFFFE) : m==1 ? uint16(0) : m==2 ? uint16(1) : uint16(next()%0xFFFE); }
                    const uint16 scan0=AtA<uint16>(SCAN);

                    // ---------- modelos falsos MODEL0 .. MODEL0+NM-1 ----------
                    for(int m=0;m<NM;++m){
                        auto& mb=miBuf[size_t(m)]; auto& cb=colBuf[size_t(m)]; auto& db=cdBuf[size_t(m)]; auto& ob=oiBuf[size_t(m)];
                        mb.fill(0); cb.fill(0); db.fill(0); ob.fill(0);
                        const uint32 mm=next()%8; const float rad= mm==0 ? 0.f : mm==1 ? uni(0.f,0.5f) : mm==2 ? uni(20.f,60.f) : uni(0.3f,15.f);
                        At<CVector>(cb.data(),0)=CVector{-uni(0.05f,6.f),-uni(0.05f,6.f),-uni(0.05f,4.f)}; At<CVector>(cb.data(),0xC)=CVector{uni(0.05f,6.f),uni(0.05f,6.f),uni(0.05f,4.f)};
                        At<CVector>(cb.data(),0x18)=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-3.f,3.f)}; At<float>(cb.data(),0x24)=rad;
                        const int nb=int(next()%3), ns=int(next()%4);
                        At<uint16>(db.data(),0)=uint16(ns); At<uint16>(db.data(),2)=uint16(nb); At<void*>(db.data(),8)=sphBuf[size_t(m)][0].data(); At<void*>(db.data(),0xC)=boxBuf[size_t(m)][0].data();
                        for(int s=0;s<3;++s){ auto& sp=sphBuf[size_t(m)][size_t(s)]; std::memset(sp.data(),0,0x14); At<CVector>(sp.data(),0)=CVector{uni(-4.f,4.f),uni(-4.f,4.f),next()%4==0 ? 1000.f : uni(-5.f,8.f)}; At<float>(sp.data(),0xC)=uni(0.2f,3.f); }
                        for(int b=0;b<2;++b){ auto& bx=boxBuf[size_t(m)][size_t(b)]; std::memset(bx.data(),0,0x1C); At<CVector>(bx.data(),0)=CVector{-uni(0.05f,3.f),-uni(0.05f,3.f),-uni(0.05f,3.f)}; At<CVector>(bx.data(),0xC)=CVector{uni(0.05f,3.f),uni(0.05f,3.f),uni(0.05f,3.f)}; }
                        At<void*>(cb.data(),0x2C)= next()%5==0 ? nullptr : static_cast<void*>(db.data());
                        static const uint16 sway[]{0x0000,0x0800,0x1000,0x1800,0x7800,0x2000,0x0900};
                        At<uint16>(mb.data(),0x12)=sway[next()%std::size(sway)]; At<void*>(mb.data(),0x14)=cb.data();
                        At<float>(ob.data(),0x14)=uni(0.f,6.f); At<uint8>(ob.data(),0x1F)=uint8(next()%2);
                        AtA<void*>(MI_PTRS+size_t(MODEL0+m)*4)=mb.data();
                    }
                    // ---------- variables ModelIndices -> modelos falsos distintos ----------
                    { std::array<uint16,NM> perm{}; for(int k=0;k<NM;++k)perm[size_t(k)]=uint16(MODEL0+k); for(int k=NM-1;k>0;--k)std::swap(perm[size_t(k)],perm[next()%uint32(k+1)]);
                      size_t pi=0; for(auto va:MIVAR_A)AtA<uint16>(va)=perm[pi++]; for(auto vb:MIVAR_B)AtA<uint16>(vb)=perm[pi++]; AtA<uint16>(MIVAR_BUOY)=perm[pi++]; AtA<uint16>(MIVAR_FENCE1)=perm[pi++];
                      AtA<uint16>(MIVAR_FENCE2)=perm[pi++]; AtA<uint16>(MIVAR_ATM)=perm[pi++]; /* el hidrante es la variable 0x8CD594 de la lista A */ }

                    // ---------- utilidades de fixtures ----------
                    auto FillMat=[&](void* m,const CVector& pos,bool rotated){
                        auto unit=[&]{ CVector v{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float l=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); return l<0.1f ? CVector{1.f,0.f,0.f} : CVector{v.x/l,v.y/l,v.z/l}; };
                        auto cross=[&](const CVector& a,const CVector& b){ return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; };
                        CVector r{1.f,0.f,0.f}, fw{0.f,1.f,0.f}, u{0.f,0.f,1.f};
                        if(rotated){ r=unit(); CVector u0=unit(); fw=cross(u0,r); const float l=std::sqrt(fw.x*fw.x+fw.y*fw.y+fw.z*fw.z); fw=l<0.05f ? CVector{0.f,1.f,0.f} : CVector{fw.x/l,fw.y/l,fw.z/l}; u=cross(r,fw); }
                        std::memset(m,0,0x48);
                        At<CVector>(m,0)=r; At<CVector>(m,0x10)=fw; At<CVector>(m,0x20)=u; At<CVector>(m,0x30)=pos;
                    };
                    auto SetPos=[&](uint8* e,const CVector& pos,bool forceMat){
                        if(forceMat||next()%2){ auto& mt=mats[matNext++%NMAT]; FillMat(mt.data(),pos,next()%2!=0); At<void*>(e,0x14)=mt.data(); }
                        else { At<void*>(e,0x14)=nullptr; At<CVector>(e,4)=pos; }
                    };
                    auto GetPos=[&](uint8* e)->CVector{ auto* mt=At<uint8*>(e,0x14); return mt ? At<CVector>(mt,0x30) : At<CVector>(e,4); };
                    // entidad base: vtable falsa, tipo/estado, modelo, banderas; solo las fisicas (vehiculo/peaton/objeto) tienen masas y velocidad
                    auto Base=[&](uint8* e,int type,int status,const CVector& pos,bool forceMat,bool physical){
                        At<void*>(e,0)=entVt.data(); At<uint16>(e,0x22)=uint16(MODEL0+int(next()%NM)); At<uint8>(e,0x36)=uint8(type|(status<<3));
                        { const uint32 m=next()%4; At<uint16>(e,0x2C)= m==0 ? scan0 : m==1 ? uint16(scan0+1) : uint16(next()); }
                        At<void*>(e,0x18)=rwObj.data();
                        { const uint32 u=next()%10<7 ? 1u : 0u, sflag=next()%5==0 ? 4u : 0u, w=next()%10==0 ? 0x40000u : 0u; At<uint32>(e,0x1C)=u|sflag|w; }
                        if(physical){
                            { const uint32 b2=next()%7==0 ? 4u : 0u, b7=next()%7==0 ? 0x80u : 0u, fp=next()%4==0 ? 0x80000u : 0u, inv=next()%4==0 ? 0x400000u : 0u, ex=next()%8==0 ? 0x800000u : 0u, b4=next()%5==0 ? 0x10u : 0u;
                              At<uint32>(e,0x40)=b2|b7|fp|inv|ex|b4; }
                            At<float>(e,0x8C)=uni(40.f,3000.f); At<float>(e,0x90)=At<float>(e,0x8C)*uni(0.3f,3.f);
                            At<CVector>(e,0x44)=CVector{uni(-0.6f,0.6f),uni(-0.6f,0.6f),uni(-0.6f,0.6f)};
                        }
                        SetPos(e,pos,forceMat);
                    };
                    auto PedExtras=[&](uint8* e,size_t k){
                        { const uint32 s1=next()%6==0 ? 0x37u : next()%8==0 ? 0x36u : next()%30; At<uint32>(e,0x530)=s1; }
                        { const uint32 inv=next()%6==0 ? 0x100u : 0u, st1=next()%3==0 ? 1u : 0u, rnd=(next()%2)<<7; At<uint32>(e,0x46C)=inv|st1|rnd; }
                        At<void*>(e,0x730)= next()%5==0 ? static_cast<void*>(rwObj.data()) : nullptr;
                        At<uint8>(e,0x484)= next()%5==0 ? uint8(2) : uint8(next()%5);
                        { const uint32 t=next()%3==0 ? next()%2 : 2+next()%20; At<uint32>(e,0x598)=t; }
                        At<void*>(e,0x47C)=intel[k].data(); At<float>(e,0x540)=next()%8==0 ? 0.f : uni(1.f,100.f);
                    };
                    auto VehExtras=[&](uint8* e){
                        At<uint32>(e,0x428)=next()&0xFFFFFFu; At<void*>(e,0x490)= next()%5==0 ? static_cast<void*>(rwObj.data()) : nullptr;
                        At<int8>(e,0x48B)=int8(-1); { const uint32 t=next()%4==0 ? 4u : next()%4==0 ? 10u : next()%12; At<uint32>(e,0x594)=t; }
                        At<uint16>(e,0x4DE)= next()%3==0 ? uint16(0) : uint16(next()); At<uint8>(e,0x4A8)=uint8(next());
                        At<void*>(e,0x460)= next()%2 ? static_cast<void*>(Ped(next()%NPOOLPED)) : nullptr;
                    };
                    auto ObjExtras=[&](uint8* e){
                        At<uint32>(e,0x140)= (next()%4==0 ? 0x40u : 0u); At<float>(e,0x154)=uni(0.f,1000.f);
                        const int mdl=int(At<uint16>(e,0x22))-MODEL0; At<void*>(e,0x160)= mdl>=0&&mdl<NM ? static_cast<void*>(oiBuf[size_t(mdl)].data()) : static_cast<void*>(oiBuf[0].data());
                    };
                    // creador / victima aleatorios entre las entidades falsas
                    auto AnyEntity=[&]()->CEntity*{
                        switch(next()%7){ case 0:return nullptr; case 1:return reinterpret_cast<CEntity*>(Ped(NP-1)); case 2:return reinterpret_cast<CEntity*>(Ped(next()%NPOOLPED));
                                          case 3:return reinterpret_cast<CEntity*>(Veh(next()%NV)); case 4:return reinterpret_cast<CEntity*>(Obj(next()%NO)); case 5:return reinterpret_cast<CEntity*>(Bld(next()%NB));
                                          default:return reinterpret_cast<CEntity*>(Ped(next()%NPOOLPED)); } };
                    auto Around=[&](const CVector& c,float spread){ return CVector{c.x+uni(-spread,spread),c.y+uni(-spread,spread),c.z+uni(-9.f,9.f)}; };
                    // las entidades de todos los pools; los vehiculos siempre llevan matriz (GetMatrix no debe reservar una del juego real)
                    auto PoolWorld=[&](const CVector& center,float spread,bool matrixAlways){
                        for(size_t k=0;k<NV;++k)  { uint8* e=Veh(k); Base(e,ENTITY_TYPE_VEHICLE,int(next()%6),Around(center,spread),true,true); VehExtras(e); }
                        for(size_t k=0;k<NP;++k)  { uint8* e=Ped(k);  Base(e,ENTITY_TYPE_PED,int(next()%3),Around(center,spread),matrixAlways,true); PedExtras(e,k); }
                        for(size_t k=0;k<NO;++k)  { uint8* e=Obj(k);  Base(e,ENTITY_TYPE_OBJECT,int(next()%3),Around(center,spread),matrixAlways,true); ObjExtras(e); }
                        for(size_t k=0;k<NB;++k)  { uint8* e=Bld(k);  Base(e,ENTITY_TYPE_BUILDING,0,Around(center,spread),matrixAlways,false); }
                        for(size_t k=0;k<ND;++k)  { uint8* e=Dum(k);  Base(e,ENTITY_TYPE_DUMMY,0,Around(center,spread),matrixAlways,false); }
                        // el "jugador" (fuera del pool): peaton de tipo 0/1 (IsPlayer) vivo y fuera de vehiculo
                        { uint8* e=Ped(NP-1); At<uint32>(e,0x598)=next()%2; At<uint32>(e,0x530)=next()%10; At<uint32>(e,0x46C)&=~0x100u; At<void*>(e,0x730)=nullptr; }
                    };
                    auto Sx=[](float v){ return int(std::floor(v*0.02f+60.f)); };
                    auto PlaceInSectors=[&](uint8* e,int type){
                        const CVector p=GetPos(e); const int copies=1+int(next()%3);
                        for(int c=0;c<copies;++c){
                            const int x=Sx(p.x)+int(next()%3)-1, y=Sx(p.y)+int(next()%3)-1; const int cx=std::clamp(x,0,119), cy=std::clamp(y,0,119);
                            switch(type){
                            case ENTITY_TYPE_BUILDING: CWorld::GetSector(cx,cy).Buildings.AddItem(reinterpret_cast<CBuilding*>(e)); break;
                            case ENTITY_TYPE_DUMMY:    CWorld::GetSector(cx,cy).Dummies.AddItem(reinterpret_cast<CDummy*>(e)); break;
                            case ENTITY_TYPE_VEHICLE:  CWorld::ms_aRepeatSectors[y&15][x&15].Vehicles.AddItem(reinterpret_cast<CVehicle*>(e)); break;
                            case ENTITY_TYPE_PED:      CWorld::ms_aRepeatSectors[y&15][x&15].Peds.AddItem(reinterpret_cast<CPed*>(e)); break;
                            default:                   CWorld::ms_aRepeatSectors[y&15][x&15].Objects.AddItem(reinterpret_cast<CObject*>(e)); break;
                            }
                        }
                    };
                    auto PlaceAll=[&](bool veh,bool ped,bool obj,bool bld,bool dum){
                        if(veh)for(size_t k=0;k<NV;++k)PlaceInSectors(Veh(k),ENTITY_TYPE_VEHICLE); if(ped)for(size_t k=0;k<NPOOLPED;++k)PlaceInSectors(Ped(k),ENTITY_TYPE_PED);
                        if(obj)for(size_t k=0;k<NO;++k)PlaceInSectors(Obj(k),ENTITY_TYPE_OBJECT); if(bld)for(size_t k=0;k<NB;++k)PlaceInSectors(Bld(k),ENTITY_TYPE_BUILDING);
                        if(dum)for(size_t k=0;k<ND;++k)PlaceInSectors(Dum(k),ENTITY_TYPE_DUMMY);
                    };
                    auto RndRadius=[&]{ const uint32 m=next()%12; return m==0 ? 0.f : m==1 ? uni(0.f,3.f) : m==2 ? uni(150.f,400.f) : m==3 ? std::bit_cast<float>(0x7FC00000u) : uni(2.f,60.f); };
                    auto Nudge=[&](float v){ const uint32 j=next()%3; return j==0 ? std::nextafter(v,1e9f) : j==1 ? std::nextafter(v,-1e9f) : v; };
                    // coordenada cerca del borde del limite (a veces exacta: empate con +-1 ulp)
                    auto Near=[&](float center,float lim){ const uint32 m=next()%6; return m==0 ? Nudge(center+lim) : m==1 ? Nudge(center-lim) : uni(center-lim*1.3f-1.f,center+lim*1.3f+1.f); };

                    // ======================================================================================
                    // fixtures y llamada, segun la funcion (la llamada se hace con el estado del hook y los stubs ya activos)
                    json o=json::object(); json ret=nullptr; bool switches=false;
                    CPtrListDoubleLink<CPhysical*> flatList{};
                    std::array<int32,20064> models{};     // contadores de TestForUnusedModels (margen de 32 por cada lado)
                    const uintptr_t a=fn.address;
                    auto Exec=[&](const std::function<void()>& call){
                        setState(candidate);
                        switches= ReversingProbe::JumpsToDll(fn.address)==candidate;
                        { PatchSet ps; AddStubs(ps,nm); trace=json::array(); scriptPos=0; call(); }
                        setState(false);
                    };

                    if(nm=="ResetLineTestOptions") {
                        for(size_t b=0;b<0x20;++b)At<uint8>(reinterpret_cast<void*>(LINEOPT),b)=uint8(next());
                        Exec([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); });
                        o["line"]=ReversingProbe::Bytes(reinterpret_cast<void*>(LINEOPT),0x20);
                    }
                    else if(nm=="SetAllCarsCanBeDamaged"||nm=="StopAllLawEnforcersInTheirTracks") {
                        PoolWorld(CVector{0.f,0.f,0.f},50.f,true);
                        for(size_t k=0;k<NV;++k){ At<uint32>(Veh(k),0x428)=next(); if(next()%3==0)At<uint32>(Veh(k),0x428)|=1u; }
                        const bool enable=next()%2!=0;
                        if(nm=="SetAllCarsCanBeDamaged") Exec([&]{ reinterpret_cast<void(__cdecl*)(bool)>(a)(enable); });
                        else Exec([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); });
                        o["veh"]=Hash(vehMem.data(),vehMem.size()); o["states"]=ReversingProbe::Bytes(vehSt.data(),vehSt.size());
                    }
                    else if(nm=="UseDetonator") {
                        PoolWorld(CVector{0.f,0.f,0.f},50.f,true);
                        CPed* creator= next()%6==0 ? nullptr : reinterpret_cast<CPed*>(Ped(next()%NP));
                        for(size_t k=0;k<NV;++k){
                            uint8* e=Veh(k); At<uint8>(e,0x4A8)=uint8((next()%2 ? 3u : next()%8u)|(next()&0xF8u));
                            switch(next()%4){ case 0:At<void*>(e,0x4CC)=creator; break; case 1:At<void*>(e,0x4CC)=nullptr; break; case 2:At<void*>(e,0x4CC)=Ped(next()%NP); break; default:At<void*>(e,0x4CC)=creator; }
                            At<uint16>(e,0x4DE)=uint16(next()); At<void*>(e,0x4E0)=next()%2 ? static_cast<void*>(Ped(next()%NP)) : nullptr;
                        }
                        Exec([&]{ reinterpret_cast<void(__cdecl*)(CPed*)>(a)(creator); });
                        o["veh"]=Hash(vehMem.data(),vehMem.size()); o["creator"]=NameS(creator);
                    }
                    else if(nm=="SetPedsOnFire"||nm=="SetPedsChoking"||nm=="SetCarsOnFire") {
                        const CVector c{uni(-80.f,80.f),uni(-80.f,80.f),uni(-5.f,40.f)}; PoolWorld(c,60.f,false);
                        const bool vehicles=nm=="SetCarsOnFire", onFire=nm=="SetPedsOnFire"; const float radius=RndRadius();
                        CEntity* creator=AnyEntity();
                        const size_t cnt= vehicles ? NV : NPOOLPED;
                        for(size_t k=0;k<cnt;++k){
                            uint8* e= vehicles ? Veh(k) : Ped(k);
                            float lim=radius; if(onFire&&reinterpret_cast<CEntity*>(e)==creator)lim=0.5f*radius;
                            if(!(lim>=0.f&&lim<1e5f))lim=10.f;
                            const float dz= next()%6==0 ? Nudge(5.f) : next()%6==0 ? -Nudge(5.f) : uni(-7.f,7.f);
                            const float px=Near(c.x,lim), py=Near(c.y,lim);
                            SetPos(e,CVector{px,py,c.z+dz},false);
                        }
                        if(vehicles) Exec([&]{ reinterpret_cast<void(__cdecl*)(CVector,float,CEntity*)>(a)(c,radius,creator); });
                        else         Exec([&]{ reinterpret_cast<void(__cdecl*)(float,float,float,float,CEntity*)>(a)(c.x,c.y,c.z,radius,creator); });
                        o["veh"]=Hash(vehMem.data(),vehMem.size()); o["ped"]=Hash(pedMem.data(),pedMem.size());
                    }
                    else if(nm=="SetWorldOnFire") {
                        const CVector c{uni(-80.f,80.f),uni(-80.f,80.f),uni(-5.f,40.f)}; const float radius=RndRadius(); CEntity* creator=AnyEntity();
                        twResult= next()%2 ? reinterpret_cast<CEntity*>(Bld(next()%NB)) : nullptr; numFires=next()%3==0 ? next()%4 : 0;
                        Exec([&]{ reinterpret_cast<void(__cdecl*)(CVector,float,CEntity*)>(a)(c,radius,creator); });
                    }
                    else if(nm=="SprayPaintWorld") {
                        PoolWorld(CVector{0.f,0.f,0.f},20.f,true);
                        foundCount= next()%8==0 ? -int(next()%3) : int(next()%16);
                        for(size_t k=0;k<16;++k){ foundList[k]=reinterpret_cast<CEntity*>(Obj(next()%NO)); tagFlag[k]=next()%3!=0;
                            tagAlpha[k]=uint8(next()%4==0 ? 255u : next()%4==0 ? 247u+next()%9u : next()%256u); }
                        { const uint32 m=next()%10; AtA<int16>(SPRAYVAR)= m==0 ? int16(next()%64) : m==1 ? int16(-int(next()%40)) : int16(8); }
                        CVector posn{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,20.f)}, outDir{1234.f,-5678.f,9.f}; const float radius=uni(0.f,30.f); const bool process=next()%4!=0;
                        Exec([&]{ ret=int(reinterpret_cast<int32(__cdecl*)(CVector&,CVector&,float,bool)>(a)(posn,outDir,radius,process)); });
                        o["out_dir"]=json::array({F(outDir.x),F(outDir.y),F(outDir.z)}); o["alpha"]=ReversingProbe::Bytes(tagAlpha.data(),tagAlpha.size()); o["objs"]=Hash(objMem.data(),objMem.size());
                    }
                    else if(nm.rfind("TestForBuildingsOnTopOfEachOther",0)==0||nm.rfind("TestForUnusedModels",0)==0) {
                        const bool unused=nm.rfind("TestForUnusedModels",0)==0; const bool isVoid=nm.find('<')==std::string::npos;
                        PoolWorld(CVector{0.f,0.f,0.f},300.f,false);
                        if(unused){ // indices de modelo dentro de la tabla (en la lista, algunos negativos pequenos: -16..-1, dentro del margen)
                            auto Model=[&](uint8* e){ At<uint16>(e,0x22)= (!isVoid&&next()%9==0) ? uint16(0xFFF0u+next()%16) : uint16(next()%200); };
                            for(size_t k=0;k<NB;++k)Model(Bld(k)); for(size_t k=0;k<ND;++k)Model(Dum(k));
                        } else { // posiciones y modelos repetidos entre edificios consecutivos
                            for(size_t k=1;k<NB;++k)if(next()%3==0){ At<uint16>(Bld(k),0x22)=At<uint16>(Bld(k-1),0x22); SetPos(Bld(k),GetPos(Bld(k-1)),false); } }
                        if(isVoid){
                            const int nl=2+int(next()%8);
                            for(int l=0;l<nl;++l){ const int x=int(next()%120), y=int(next()%120); CWorld::GetSector(x,y).Buildings.AddItem(reinterpret_cast<CBuilding*>(Bld(next()%NB)));
                                if(next()%2)CWorld::GetSector(x,y).Dummies.AddItem(reinterpret_cast<CDummy*>(Dum(next()%ND)));
                                if(next()%3==0)CWorld::GetSector(int(next()%120),int(next()%120)).Dummies.AddItem(reinterpret_cast<CDummy*>(Dum(next()%ND))); }
                            Exec([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); });
                        } else {
                            const int n=int(next()%7); for(int k=0;k<n;++k)flatList.AddItem(reinterpret_cast<CPhysical*>(next()%2 ? Bld(next()%NB) : Dum(next()%ND)));
                            if(unused) Exec([&]{ reinterpret_cast<void(__cdecl*)(void*,int32*)>(a)(&flatList,models.data()+32); });
                            else Exec([&]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(&flatList); });
                        }
                        o["bld"]=Hash(bldMem.data(),bldMem.size()); o["dum"]=Hash(dumMem.data(),dumMem.size()); o["models"]=Hash(models.data(),models.size()*4); o["scan"]=int(AtA<uint16>(SCAN));
                    }
                    else if(nm.rfind("TestSphereAgainst",0)==0) {
                        const bool world=nm=="TestSphereAgainstWorld"; const CVector c{uni(-60.f,60.f),uni(-60.f,60.f),uni(-5.f,30.f)};
                        PoolWorld(c,40.f,true);
                        CEntity* ignore= next()%3==0 ? AnyEntity() : nullptr; const float radius= next()%10==0 ? std::bit_cast<float>(0x7FC00000u) : next()%6==0 ? 0.f : uni(0.5f,45.f);
                        const bool cam=next()%2!=0;
                        if(world) {
                            PlaceAll(true,true,true,true,true);
                            bool fl[5]; for(auto& b:fl)b=next()%5!=0;
                            Exec([&]{ ret=NameS(reinterpret_cast<CEntity*(__cdecl*)(CVector,float,CEntity*,bool,bool,bool,bool,bool,bool)>(a)(c,radius,ignore,fl[0],fl[1],fl[2],fl[3],fl[4],cam)); });
                        } else {
                            const int n=int(next()%9);
                            for(int k=0;k<n;++k){ switch(next()%4){ case 0:flatList.AddItem(reinterpret_cast<CPhysical*>(Veh(next()%NV))); break; case 1:flatList.AddItem(reinterpret_cast<CPhysical*>(Ped(next()%NPOOLPED))); break;
                                                                  case 2:flatList.AddItem(reinterpret_cast<CPhysical*>(Obj(next()%NO))); break; default:flatList.AddItem(reinterpret_cast<CPhysical*>(Bld(next()%NB))); } }
                            Exec([&]{ ret=NameS(reinterpret_cast<CEntity*(__cdecl*)(void*,CVector,float,CEntity*,bool)>(a)(&flatList,c,radius,ignore,cam)); });
                        }
                        o["veh"]=Hash(vehMem.data(),vehMem.size()); o["ped"]=Hash(pedMem.data(),pedMem.size()); o["obj"]=Hash(objMem.data(),objMem.size()); o["bld"]=Hash(bldMem.data(),bldMem.size());
                        o["dum"]=Hash(dumMem.data(),dumMem.size()); o["scan"]=int(AtA<uint16>(SCAN));
                    }
                    else if(nm.rfind("TriggerExplosion",0)==0) {
                        const bool whole=nm=="TriggerExplosion"; const CVector point{uni(-70.f,70.f),uni(-70.f,70.f),uni(-5.f,30.f)};
                        PoolWorld(point,35.f,false);
                        { const uint32 m=next()%4; AtA<float>(TIMESTEP)= m==0 ? 1.f : uni(0.2f,4.f); }
                        for(size_t k=0;k<NV;++k){
                            uint8* e=Veh(k); { const uint32 m=next()%5; if(m==0)At<uint8>(e,0x36)=uint8(ENTITY_TYPE_VEHICLE|(2<<3)); else if(m==1)At<uint8>(e,0x36)=uint8(ENTITY_TYPE_VEHICLE|(5<<3)); }
                        }
                        for(size_t k=0;k<NPOOLPED;++k){
                            uint8* e=Ped(k); { const uint32 s1=next()%30; At<uint32>(e,0x530)=s1; } At<void*>(e,0x730)=nullptr;
                            switch(next()%5){ case 0:{ uint8* wv=Veh(next()%NV); At<uint8>(wv,0x36)=uint8(ENTITY_TYPE_VEHICLE|(5<<3)); At<void*>(e,0xFC)=wv; break; }
                                              case 1:At<void*>(e,0xFC)=Veh(next()%NV); break; default:At<void*>(e,0xFC)=nullptr; }
                            At<uint8>(e,0x484)=0; At<uint32>(e,0x40)&=~0x400000u; // AffectsPed (real) sin invulnerables
                        }
                        // modelos especiales de los objetos: vallas, hidrante, cajero
                        for(size_t k=0;k<NO;++k){ uint8* e=Obj(k); switch(next()%7){ case 0:At<uint16>(e,0x22)=AtA<uint16>(MIVAR_FENCE1); break; case 1:At<uint16>(e,0x22)=AtA<uint16>(MIVAR_FENCE2); break;
                                                                                         case 2:At<uint16>(e,0x22)=AtA<uint16>(MIVAR_HYDRANT); break; case 3:At<uint16>(e,0x22)=AtA<uint16>(MIVAR_ATM); break; default:break; }
                            ObjExtras(e); }
                        CEntity* victim= AnyEntity(); CEntity* creator= next()%5==0 ? reinterpret_cast<CEntity*>(Ped(NP-1)) : AnyEntity();
                        const float radius= next()%12==0 ? uni(0.f,3.f) : next()%12==0 ? std::bit_cast<float>(0x7FC00000u) : uni(8.f,60.f), vis=uni(0.f,80.f), damage=uni(0.f,3.f); const bool bomb=next()%2!=0;
                        if(whole) {
                            PlaceAll(true,true,true,false,false);
                            Exec([&]{ reinterpret_cast<void(__cdecl*)(const CVector*,float,float,CEntity*,CEntity*,bool,float)>(a)(&point,radius,vis,victim,creator,bomb,damage); });
                        } else {
                            const int n=int(1+next()%9);
                            for(int k=0;k<n;++k){ switch(next()%3){ case 0:flatList.AddItem(reinterpret_cast<CPhysical*>(Veh(next()%NV))); break; case 1:flatList.AddItem(reinterpret_cast<CPhysical*>(Ped(next()%NPOOLPED))); break;
                                                                   default:flatList.AddItem(reinterpret_cast<CPhysical*>(Obj(next()%NO))); } }
                            Exec([&]{ reinterpret_cast<void(__cdecl*)(void*,const CVector*,float,float,CEntity*,CEntity*,bool,float)>(a)(&flatList,&point,radius,vis,victim,creator,bomb,damage); });
                        }
                        o["veh"]=Hash(vehMem.data(),vehMem.size()); o["ped"]=Hash(pedMem.data(),pedMem.size());
                        { auto masked=objMem; for(size_t k=0;k<NO;++k)At<uint32>(masked.data()+k*SO,0x154)=0; /* vida del ATM: valor residual de pila del EXE */ o["obj"]=Hash(masked.data(),masked.size()); }
                        o["scan"]=int(AtA<uint16>(SCAN));
                    }
                    else if(nm=="RepositionOneObject"||nm=="RepositionCertainDynamicObjects") {
                        PoolWorld(CVector{0.f,0.f,0.f},200.f,true);
                        auto AssignModel=[&](uint8* e){
                            const uint32 m=next()%10; uint16 mdl;
                            if(m<3)mdl=AtA<uint16>(MIVAR_A[next()%std::size(MIVAR_A)]); else if(m<5)mdl=AtA<uint16>(MIVAR_B[next()%std::size(MIVAR_B)]); else if(m==5)mdl=AtA<uint16>(MIVAR_BUOY);
                            else mdl=uint16(MODEL0+int(next()%NM));
                            At<uint16>(e,0x22)=mdl; };
                        if(nm=="RepositionOneObject") {
                            uint8* e= next()%2 ? Dum(next()%ND) : Obj(next()%NO); AssignModel(e);
                            Exec([&]{ reinterpret_cast<void(__cdecl*)(CEntity*)>(a)(reinterpret_cast<CEntity*>(e)); });
                        } else {
                            for(size_t k=0;k<ND;++k)AssignModel(Dum(k));
                            Exec([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); });
                        }
                        o["obj"]=Hash(objMem.data(),objMem.size()); o["dum"]=Hash(dumMem.data(),dumMem.size()); o["mats"]=Hash(mats.data(),sizeof(mats));
                        o["scan"]=int(AtA<uint16>(SCAN)); o["crossings"]=AtA<int32>(LINEOPT);
                    }
                    else if(nm=="ShutDown") {
                        PoolWorld(CVector{0.f,0.f,0.f},300.f,false);
                        std::memset(reinterpret_cast<void*>(GSTRING),0x5A,0x100);
                        PlaceAll(true,true,true,true,true);
                        { auto* lod=reinterpret_cast<CPtrListSingleLink<CEntity*>*>(LODL); const int n=int(next()%6);
                          for(int k=0;k<n;++k)lod[next()%900].AddItem(reinterpret_cast<CEntity*>(next()%2 ? Bld(next()%NB) : Dum(next()%ND))); }
                        { const int n=int(next()%5); for(int k=0;k<n;++k)CWorld::ms_listMovingEntityPtrs.AddItem(reinterpret_cast<CPhysical*>(Veh(next()%NV))); }
                        Exec([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); });
                        o["gstring"]=Hash(reinterpret_cast<void*>(GSTRING),0x100);
                        o["gstring_text"]=std::string(reinterpret_cast<const char*>(GSTRING),strnlen(reinterpret_cast<const char*>(GSTRING),0x100));
                        o["moving"]=AtA<uint32>(MOVING); o["lod"]=Hash(reinterpret_cast<void*>(LODL),LOD_SIZE);
                    }

                    o["ret"]=ret; o["trace"]=trace; o["switches"]=switches;
                    o["world"]=json::array({Hash(reinterpret_cast<void*>(SECTORS),SECTORS_SIZE),Hash(reinterpret_cast<void*>(REPEAT),REPEAT_SIZE),Hash(reinterpret_cast<void*>(LODL),LOD_SIZE),
                                            Hash(dblStore.data(),dblStore.size()),Hash(dblStates.data(),dblStates.size()),Hash(sglStore.data(),sglStore.size()),Hash(sglStates.data(),sglStates.size()),AtA<uint32>(MOVING)});
                    flatList.Flush(); // los nodos de la lista suelta vuelven al pool falso antes de restaurar los punteros de los pools
                    RestoreAll();
                    return o;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    const bool intactBefore=Restored();
    RestoreAll();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},{"state_restored",Restored()}});
}
}
