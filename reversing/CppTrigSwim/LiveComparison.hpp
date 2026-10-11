#pragma once
// Banco dentro del motor: helicoptero, ir a la puerta del coche y nadar (lista de 11, 33 funciones).
// CHeli (AddHeliSearchLight, BurstTyre, Fix, InitHelis, Pre/Post_SearchLightCone, Render, RenderAllHeliSearchLights,
// SetUpWheelColModel, SwitchPoliceHelis, TestSniperCollision), CTaskComplexGoToCarDoorAndStandStill (constructor,
// destructor, Clone, GetTaskType, MakeAbortable, CreateSubTask, CreateFirstSubTask, CreateNextSubTask, ControlSubTask,
// IsVehicleInRange, ComputeRouteToDoor) y CTaskSimpleSwim (constructor, MakeAbortable, ProcessPed, ApplyRollAndPitch,
// ProcessSwimAnims, ProcessSwimmingResistance, ProcessEffects, ProcessControlAI, ProcessControlInput, CreateFxSystem,
// DestroyFxSystem).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// helicopteros/vehiculos falsos del tamano de CHeli (nunca vehiculos reales del pool) con vtable de trampas (hueco 41
// BlowUpCar), peatones falsos con clump de tipo 2, matriz, banderas, datos de jugador, IK y armas; subtarea, evento,
// mando, asociaciones de animacion, jerarquia de huesos, sistemas de efectos y rutas falsos.
// Se simulan y trazan: SearchLightCone, RwRenderStateSet (puntero del motor), CCollision::DistToLine, FindPlayerPed,
// CVehicleModelInfo::SetVehicleColour, RwFrameForAllObjects (con el callback GetCurrentAtomicObjectCB del EXE o de la
// DLL), CVehicle::SetComponentAtomicAlpha, CEntity::Render, CDamageManager::ResetDamageStatus, CCarEnterExit (posicion
// de la puerta, puertas cercanas, camino bloqueado, indice de pasajero), CPedGeometryAnalyser (lado, planos, ruta),
// CPointRoute::operator new/delete (arena propia), CTaskSimpleGoToPoint::UpdatePoint, CAnimManager::BlendAnimation,
// RpAnimBlendClumpGetAssociation/SetBlendDeltas, CAnimBlendAssociation::Start, CAnimManager::AddAnimBlockRef,
// CStreaming::RequestModel, CPed::RestoreHeadingRate/SetMoveState/Say, FxManager_c::CreateFxSystem, FxSystem_c (Kill,
// Play, PlayAndKill, CopyParentMatrix, SetMatrix, SetLocalParticles, AddParticle), Fx_c::TriggerWaterSplash,
// CAEPedAudioEntity::AddAudioEvent, RwMatrixCreate/Update/Destroy, GetAnimHierarchyFromSkinClump, RpHAnimIDGetIndex,
// RpHAnimHierarchyGetMatrixArray, CPlayerPed (GetPadFromPlayer, ControlButtonSprint, GetButtonSprintResults,
// HandlePlayerBreath, HandleSprintEnergy), CPad::GetPedWalkUpDown/LeftRight/WeaponJustDown (JumpJustDown es en linea en
// la DLL: se evalua de verdad sobre el mando falso), CGameLogic, CGeneral (angulos), CCamera::GetLookDirection,
// CWaterLevel::GetWaterLevel, CStats, CTaskSimpleClimb::TestForClimb, CPedGroups::GetPedsGroup,
// CPedGroupMembership::GetLeader, CPedIntelligence::GetTaskSwim, CPedIK::RotateTorso, CEntity::UpdateRwFrame,
// CPhysical::ApplyMoveForce y el RNG (rand del EXE y CGeneral::GetRandomNumber). CMatrix (ApplyRollAndPitch),
// CVector::Normalise, pow/asin del CRT del EXE y CEntity::RegisterReference corren de verdad.
// Se guardan y restauran en cada ejecucion y al final: globales de CHeli (0xC1C960..0xC1CAC0, 0x8D338C), reloj, paso,
// contador de fotogramas, raton en tercera persona, orientacion y frente de la camara activa, grupo de animacion de
// nadar y su bloque, el FxPrtMult_c estatico del EXE, el pool de tareas y la lista libre de referencias.
// FIX_BUGS: CTaskSimpleSwim::ProcessEffects saca las burbujas de BONE_JAW (8) en vez de la matriz 3 del EXE; en los
// fixtures las matrices 3 y 8 son iguales.
// Fuera de los fixtures: CTaskSimpleSwim::ProcessControlAI con bCanClimbOntoBoat y m_pPed nulo (el EXE lee m_pPed sin
// comprobar), cuentas de focos > 4 en AddHeliSearchLight (el EXE escribe fuera del array) y m_pRwObject nulo en
// ApplyRollAndPitch (el EXE adjunta una matriz nula).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity/Vehicle/Heli.h"
#include "Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Models/VehicleModelInfo.h"
#include "DamageManager.h"
#include "CarEnterExit.h"
#include "PedGeometryAnalyser.h"
#include "Collision/Collision.h"
#include "Fx/FxManager.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "Pad.h"
#include "GameLogic.h"
#include "General.h"
#include "Camera.h"
#include "WaterLevel.h"
#include "Stats.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "PedIntelligence.h"
#include "Streaming.h"
#include "PedIK.h"
#include "RwHelper.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../ProbeRuntime.hpp"

namespace CppTrigSwimLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// direcciones del EXE
constexpr uintptr_t TIMER=0xB7CB84, TIMESTEP=0xB7CB5C, FRAMES=0xB7CB4C, HELI_G=0xC1C960, HELI_G_N=0x160, POLICE_OK=0x8D338C, ENGINE=0xC97B24;
constexpr uintptr_t MOUSE3RD=0xB6EC2E, CAM_ORIENT=0xB6F178, CAM_ACTIVE=0xB6F081, CAM_FRONT=0xB6F32C, ASSOC_GROUPS=0xB4EA34, FXMULT=0xC19668, FXMULT_GUARD=0xC19684;
constexpr uintptr_t HELI_VT=0x871680, GOTO_VT=0x86EC4C, SWIM_VT=0x870920, EXE_ATOMIC_CB=0x6A0750;
// desplazamientos crudos del EXE
constexpr size_t E_POS=0x4, E_MATRIX=0x14, E_CLUMP=0x18, E_MODEL=0x22, PH_FLAGS=0x40, PH_MOVE=0x44, PH_MASS=0x8C,
                 P_FLAGS=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PDATA=0x480, P_BONE1=0x48C, P_ANIMGRP=0x4D4,
                 P_SHIFT=0x4D8, P_IK=0x50C, P_STATE=0x530, P_MOVE=0x534, P_SWIMMOVE=0x538, P_CURROT=0x558, P_AIMROT=0x55C, P_PEDTYPE=0x598,
                 P_WEAPONS=0x5A0, P_SLOT=0x718, P_AUDIO=0x138,
                 V_FLAGS=0x428, V_DRIVER=0x460, V_PASS=0x464, V_TIMETILL=0x4D0, V_COLORS=0x434, V_DAMAGE=0x5A0, V_NODES=0x648, V_ROTBAL=0x8AC, H_SWAT=0x9B9,
                 A_HIER=0x14, A_BLEND=0x18, A_DELTA=0x1C, A_TIME=0x20, A_SPEED=0x24, A_STEP=0x28, A_ID=0x2C, A_FLAGS=0x2E;
constexpr size_t PED_SIZE=sizeof(CPlayerPed), HELI_SIZE=sizeof(CHeli), NASSOC=9, ROUTE=0x64, NROUTES=6;

// --- buffers propios ---
alignas(16) inline std::array<std::array<uint8,HELI_SIZE>,2> helis{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,2> heliMat{};
alignas(16) inline std::array<uint8,PED_SIZE> pedA{}, pedB{}, pedC{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{}, matB{}, matC{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdata{};
alignas(16) inline std::array<uint8,0x40> clump{}, intel{}, group{}, swimTask{}, evBuf{}, atomics{}, fxA{}, fxB{}, fxNew{}, climbEnt{};
alignas(16) inline std::array<uint8,0x100> frame{}, frames{}, hier{}, rwMat{};
alignas(16) inline std::array<uint8,sizeof(CPad)> padBuf{};
alignas(16) inline std::array<uint8,0x30> sub{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,NASSOC+1> assocs{};
alignas(16) inline std::array<std::array<uint8,0x20>,NASSOC+1> hiers{};
alignas(16) inline std::array<uint8,0x40*64> boneMats{};
alignas(16) inline std::array<uint8,ROUTE*NROUTES> routes{};
alignas(16) inline std::array<float,12> vecs{};
inline std::array<void*,80> heliVtbl{};
inline std::array<void*,16> subVtbl{}, evVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline uintptr_t dllAtomicCb{}, dllAtomicCbTarget{}, modelInfo{};

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_ATOMIC_CB||(dllAtomicCb&&v==dllAtomicCb)||(dllAtomicCbTarget&&v==dllAtomicCbTarget))return "GetCurrentAtomicObjectCB";
    if(modelInfo&&v==modelInfo)return "model_info";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& r:named)if(v>=uintptr_t(r.p)&&v<uintptr_t(r.p)+r.n)return std::string(r.name)+"+"+std::to_string(v-uintptr_t(r.p));
    if(v==uintptr_t(&g_fxMan))return "g_fxMan"; if(v==uintptr_t(&g_fx))return "g_fx";
    if(v==uintptr_t(g_fx.m_Wake))return "fx_wake"; if(v==uintptr_t(g_fx.m_Bubble))return "fx_bubble";
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t from,size_t to) { json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k))); return r; }
inline json Vec(const void* p) { const auto* f=static_cast<const float*>(p); return json::array({F(f[0]),F(f[1]),F(f[2])}); }
inline json Str(const char* s) { std::string r; for(size_t k=0;k<64&&s&&s[k];++k)r+=std::format("{:02x}",uint8(s[k])); return r; }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// --- configuracion por caso ---
inline uint32 rngState{}, maskA{}, callsA{}, maskB{}, callsB{}, maskC{}, callsC{};
inline bool NextA() { return (maskA>>(callsA++%32))&1; }
inline bool NextB() { return (maskB>>(callsB++%32))&1; }
inline bool NextC() { return (maskC>>(callsC++%32))&1; }
inline std::array<float,8> dist{}; inline uint32 distCalls{};
inline std::array<void*,4> nodeAtomic{};
inline std::array<std::array<float,3>,4> doorPos{};
inline std::array<float,16> planes{}; inline std::array<float,4> planeDots{};
inline uint32 routeCount{}, routeUsed{}, passIdx{}, lookDir{}, hitSides{};
inline std::array<void*,512> assocById{};
inline bool weaponJust{}, twoPlayer{}, allowedDir{};
inline int16 walkUD{}, walkLR{};
inline float sprintCtl{}, sprintRes2{}, sprintRes3{}, fatMod{}, waterLvl{};
inline std::array<float,4> waterLevels{}; inline uint32 waterCalls{};
inline void* climbRet{}; inline void* groupRet{}; inline void* leaderRet{}; inline void* swimRet{}; inline void* fxRet{};

// --- trampas de vtable ---
template<int N> void* __fastcall VTrap(void* v,void*) { trace.push_back({"veh_unexpected_slot",N,Name(v)}); return nullptr; }
template<int... N> void FillVTraps(std::integer_sequence<int,N...>) { ((heliVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
template<int N> void* __fastcall STrap(void* v,void*) { trace.push_back({"sub_unexpected_slot",N,Name(v)}); return nullptr; }
template<int... N> void FillSTraps(std::integer_sequence<int,N...>) { ((subVtbl[N]=reinterpret_cast<void*>(&STrap<N>)),...); }
template<int N> void* __fastcall ETrap(void* v,void*) { trace.push_back({"event_unexpected_slot",N,Name(v)}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((evVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
inline void __fastcall BlowUpCarV(void* v,void*,void* damager,uint32 hide) { trace.push_back({"blow_up_car",Name(v),Name(damager),hide&0xFF}); }
inline void* __fastcall SubDtorV(void* s,void*,uint32 flags) { trace.push_back({"sub_delete",Name(s),flags}); return s; }
inline int32 __fastcall SubTypeV(void* s,void*) { const int32 t=At<int32>(s,0x2C); trace.push_back({"sub_type",Name(s),t}); return t; }
inline bool __fastcall SubAbortV(void* s,void*,void* ped,int32 prio,const void* ev) { const bool r=NextC(); trace.push_back({"sub_make_abortable",Name(s),Name(ped),prio,Name(ev),r}); return r; }
inline int32 __fastcall EvTypeV(void* e,void*) { const int32 t=At<int32>(e,0x30); trace.push_back({"event_type",Name(e),t}); return t; }
inline int32 __fastcall EvPrioV(void* e,void*) { const int32 t=At<int32>(e,0x34); trace.push_back({"event_priority",Name(e),t}); return t; }

// --- RNG ---
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FakeFindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(pedC.data()); }

// --- CHeli ---
inline void __cdecl FakeSearchLightCone(int32 corona,float ox,float oy,float oz,float tx,float ty,float tz,float radius,float power,uint32 flag,uint32 shadow,
                                        void* u0,void* u1,void* u2,uint32 a11,float baseRadius,float a13,float a14,float a15) {
    trace.push_back({"search_light_cone",corona,F(ox),F(oy),F(oz),F(tx),F(ty),F(tz),F(radius),F(power),flag&0xFF,shadow&0xFF,Name(u0),Name(u1),Name(u2),a11&0xFF,F(baseRadius),F(a13),F(a14),F(a15)});
}
inline RwBool __cdecl FakeRenderState(int32 s,void* v) { trace.push_back({"render_state",s,Name(v)}); return TRUE; }
inline float __cdecl FakeDistToLine(const CVector* a,const CVector* b,const CVector* p) { const float d=dist[distCalls++%dist.size()]; trace.push_back({"dist_to_line",Vec(a),Vec(b),Vec(p),F(d)}); return d; }
inline void __fastcall FakeSetVehicleColour(void* mi,void*,uint32 a,uint32 b,uint32 c,uint32 d) { trace.push_back({"set_vehicle_colour",Name(mi),a&0xFF,b&0xFF,c&0xFF,d&0xFF}); }
inline void* __cdecl FakeFrameForAll(void* f,void* cb,void* data) {
    void* atomic=nullptr; for(size_t k=0;k<4;++k)if(f==frames.data()+0x20*k)atomic=nodeAtomic[k];
    trace.push_back({"frame_for_all_objects",Name(f),Name(cb),Name(data)}); if(data&&atomic)*static_cast<void**>(data)=atomic; return f;
}
inline void __cdecl FakeSetAlpha(void* atomic,int32 alpha) { trace.push_back({"set_component_atomic_alpha",Name(atomic),alpha}); }
inline void __fastcall FakeEntityRender(void* e,void*) { trace.push_back({"entity_render",Name(e)}); }
inline void __fastcall FakeResetDamage(void* dm,void*) { trace.push_back({"reset_damage_status",Name(dm)}); }

// --- ir a la puerta ---
inline CVector* __cdecl FakeDoorPos(CVector* out,const CVehicle* v,int32 door) {
    const auto& p=doorPos[uint32(door)%4]; *out=CVector{p[0],p[1],p[2]}; trace.push_back({"door_position",Name(v),door,Vec(out)}); return out;
}
inline bool __cdecl FakeNearestDoor(const CPed* p,const CVehicle* v,CVector* out,int32* door) {
    const bool r=NextA(); const int32 in=*door; *door=8+int32(passIdx%4); const auto& d=doorPos[uint32(*door)%4]; *out=CVector{d[0]+0.25f,d[1],d[2]};
    trace.push_back({"nearest_car_door",Name(p),Name(v),in,r}); return r;
}
inline bool __cdecl FakeNearestPassengerDoor(const CPed* p,const CVehicle* v,CVector* out,int32* door,uint32 a,uint32 b,uint32 c) {
    const bool r=NextA(); const int32 in=*door; *door=9+int32(callsA%3); const auto& d=doorPos[uint32(*door)%4]; *out=CVector{d[0],d[1]-0.5f,d[2]};
    trace.push_back({"nearest_passenger_door",Name(p),Name(v),in,a&0xFF,b&0xFF,c&0xFF,r}); return r;
}
inline bool __cdecl FakePathBlocked(const CPed* p,const CVehicle* v,const CVector* pos) { const bool r=NextB(); trace.push_back({"path_blocked",Name(p),Name(v),Name(pos),Vec(pos),r}); return r; }
inline int32 __cdecl FakePassengerIndex(const CVehicle* v,int32 door) { trace.push_back({"passenger_index",Name(v),door,passIdx}); return int32(passIdx); }
inline int32 __cdecl FakeHitSide(const CVector* p,CEntity* e) { const int32 s=int32(hitSides&3); hitSides>>=2; trace.push_back({"hit_side",Vec(p),Name(e),s}); return s; }
inline void __cdecl FakeBBPlanes(float z,CEntity* e,CVector (*out)[4],float* dots) {
    trace.push_back({"bb_planes",F(z),Name(e)}); for(size_t k=0;k<4;++k){ (*out)[k]=CVector{planes[k*3],planes[k*3+1],planes[k*3+2]}; dots[k]=planeDots[k]; }
}
inline int32 __cdecl FakeRouteRound(const CPed* p,const CVector* a,CEntity* e,const CVector* b,void* route,int32 flag) {
    trace.push_back({"route_round_bb",Name(p),Vec(a),Name(e),Vec(b),Name(route),At<uint32>(route,0),flag});
    At<uint32>(route,0)=routeCount; for(uint32 k=0;k<routeCount&&k<8;++k){ At<float>(route,4+12*k)=float(k)+0.5f; At<float>(route,8+12*k)=-float(k); At<float>(route,12+12*k)=planes[k%12]; }
    return int32(routeCount);
}
inline void* __cdecl FakeRouteNew(uint32 size) { void* p=routeUsed<NROUTES-1 ? routes.data()+ROUTE*routeUsed++ : nullptr; trace.push_back({"route_new",size,Name(p)}); return p; }
inline void __cdecl FakeRouteDelete(void* p) { trace.push_back({"route_delete",Name(p)}); }
inline void __fastcall FakeUpdatePoint(void* t,void*,const CVector* pt,float radius,uint32 dont) { trace.push_back({"update_point",Name(t),Name(pt),Vec(pt),F(radius),dont&0xFF}); }

// --- nadar ---
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 grp,int32 id,float delta) { trace.push_back({"blend_animation",Name(c),grp,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(assocs[NASSOC].data()); }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) { void* a=id<assocById.size() ? assocById[id] : nullptr; trace.push_back({"get_association",Name(c),id,Name(a)}); return static_cast<CAnimBlendAssociation*>(a); }
inline void __cdecl FakeSetBlendDeltas(RpClump* c,uint32 mask,float d) { trace.push_back({"set_blend_deltas",Name(c),mask,F(d)}); }
inline void __fastcall FakeAssocStart(void* a,void*,float t) { trace.push_back({"assoc_start",Name(a),F(t)}); }
inline void __cdecl FakeAddBlockRef(int32 idx) { trace.push_back({"add_anim_block_ref",idx}); }
inline void __cdecl FakeRequestModel(int32 id,int32 flags) { trace.push_back({"request_model",id,flags}); }
inline void __fastcall FakeRestoreHeading(void* p,void*) { trace.push_back({"restore_heading_rate",Name(p)}); }
inline void __fastcall FakeSetMoveState(void* p,void*,int32 m) { trace.push_back({"set_move_state",Name(p),m}); }
inline int16 __fastcall FakeSay(void* p,void*,uint32 ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 c) { trace.push_back({"say",Name(p),ctx&0xFFFF,delay,F(prob),a&0xFF,b&0xFF,c&0xFF}); return 0; }
inline FxSystem_c* __fastcall FakeCreateFx(void* mgr,void*,const char* name,const CVector* pt,RwMatrix* m,uint32 flag) {
    trace.push_back({"create_fx_system",Name(mgr),Str(name),Vec(pt),Name(m),flag&0xFF,Name(fxRet)}); return static_cast<FxSystem_c*>(fxRet);
}
inline void __fastcall FakeFxKill(void* fx,void*) { trace.push_back({"fx_kill",Name(fx)}); }
inline void __fastcall FakeFxPlay(void* fx,void*) { trace.push_back({"fx_play",Name(fx)}); }
inline void __fastcall FakeFxPlayAndKill(void* fx,void*) { trace.push_back({"fx_play_and_kill",Name(fx)}); }
inline void __fastcall FakeFxCopyParent(void* fx,void*) { trace.push_back({"fx_copy_parent_matrix",Name(fx)}); }
inline void __fastcall FakeFxSetMatrix(void* fx,void*,RwMatrix* m) { trace.push_back({"fx_set_matrix",Name(fx),Name(m),Region(m,0x30,0x3C)}); }
inline void __fastcall FakeFxSetLocal(void* fx,void*,uint32 on) { trace.push_back({"fx_set_local_particles",Name(fx),on&0xFF}); }
inline void __fastcall FakeAddParticle(void* sys,void*,const CVector* pos,const CVector* vel,float t,const void* mult,float rot,float lm,float lml,uint32 local) {
    trace.push_back({"add_particle",Name(sys),Vec(pos),Vec(vel),F(t),Region(mult,0,0x1C),F(rot),F(lm),F(lml),local&0xFF});
}
inline void __fastcall FakeTriggerSplash(void* fx,void*,const CVector* pos) { trace.push_back({"trigger_water_splash",Name(fx),Vec(pos)}); }
inline void __fastcall FakeAddAudioEvent(void* ae,void*,int32 ev,float vol,float speed,void* ped,uint32 surf,int32 a7,uint32 maxVol) {
    trace.push_back({"add_audio_event",Name(ae),ev,F(vol),F(speed),Name(ped),surf&0xFF,a7,maxVol});
}
inline RwMatrix* __cdecl FakeMatrixCreate() { trace.push_back({"rw_matrix_create"}); std::memset(rwMat.data(),0xCD,rwMat.size()); return reinterpret_cast<RwMatrix*>(rwMat.data()); }
inline RwMatrix* __cdecl FakeMatrixUpdate(RwMatrix* m) { trace.push_back({"rw_matrix_update",Name(m)}); return m; }
inline RwBool __cdecl FakeMatrixDestroy(RwMatrix* m) { trace.push_back({"rw_matrix_destroy",Name(m)}); return TRUE; }
inline RpHAnimHierarchy* __cdecl FakeHierFromClump(RpClump* c) { trace.push_back({"anim_hierarchy_from_clump",Name(c)}); return reinterpret_cast<RpHAnimHierarchy*>(hier.data()); }
inline RwInt32 __cdecl FakeIDGetIndex(RpHAnimHierarchy*,RwInt32 id) { return id==BONE_R_HAND ? 10 : id==BONE_L_HAND ? 11 : id==BONE_R_FOOT ? 12 : id==BONE_L_FOOT ? 13 : 20; }
inline RwMatrix* __cdecl FakeMatrixArray(RpHAnimHierarchy*) { return reinterpret_cast<RwMatrix*>(boneMats.data()); }
inline float __cdecl FakeFatMod(int32 stat) { trace.push_back({"fat_muscle_modifier",stat,F(fatMod)}); return fatMod; }
inline void __cdecl FakeStatsSwim(uint32 a,uint32 b) { trace.push_back({"stats_swimming",a&0xFF,b&0xFF}); }
inline void __fastcall FakeBreath(void* p,void*,uint32 dec,float mult) { trace.push_back({"handle_player_breath",Name(p),dec&0xFF,F(mult)}); }
inline bool __fastcall FakeSprintEnergy(void* p,void*,uint32 sprint,float adr) { trace.push_back({"handle_sprint_energy",Name(p),sprint&0xFF,F(adr)}); return false; }
inline float __fastcall FakeControlSprint(void* p,void*,int32 type) { trace.push_back({"control_button_sprint",Name(p),type,F(sprintCtl)}); return sprintCtl; }
inline float __fastcall FakeSprintResults(void* p,void*,int32 type) { const float r=type==3 ? sprintRes3 : sprintRes2; trace.push_back({"button_sprint_results",Name(p),type,F(r)}); return r; }
inline CPad* __fastcall FakeGetPad(const void* p,void*) { trace.push_back({"pad_from_player",Name(p)}); return reinterpret_cast<CPad*>(padBuf.data()); }
inline int16 __fastcall FakeWalkUD(const void* p,void*) { trace.push_back({"walk_up_down",Name(p),walkUD}); return walkUD; }
inline int16 __fastcall FakeWalkLR(const void* p,void*) { trace.push_back({"walk_left_right",Name(p),walkLR}); return walkLR; }
inline bool __fastcall FakeWeaponJust(const void* p,void*,CPed* ped) { trace.push_back({"weapon_just_down",Name(p),Name(ped),weaponJust}); return weaponJust; }
inline bool __cdecl FakeTwoPlayer(CPed* p) { trace.push_back({"two_player_controls",Name(p),twoPlayer}); return twoPlayer; }
inline bool __cdecl FakeAllowedDir(CPed* p,float x,float y,float z,float lim) { trace.push_back({"allowed_direction",Name(p),F(x),F(y),F(z),F(lim),allowedDir}); return allowedDir; }
inline float __cdecl FakeRadianAngle(float x1,float y1,float x2,float y2) { const float r=x2*0.75f-y2*0.5f+x1+y1; trace.push_back({"radian_angle_between_points",F(x1),F(y1),F(x2),F(y2),F(r)}); return r; }
inline float __cdecl FakeLimitRadian(float a) { const float r=a>3.0f ? a-6.0f : a<-3.0f ? a+6.0f : a; trace.push_back({"limit_radian_angle",F(a),F(r)}); return r; }
inline float __cdecl FakeAngleBetween(float x1,float y1,float x2,float y2) { const float r=x1*90.0f+y1*45.0f+x2+y2; trace.push_back({"angle_between_points",F(x1),F(y1),F(x2),F(y2),F(r)}); return r; }
inline float __cdecl FakeLimitAngle(float a) { const float r=a>180.0f ? a-360.0f : a; trace.push_back({"limit_angle",F(a),F(r)}); return r; }
inline int32 __fastcall FakeLookDir(const void* cam,void*) { trace.push_back({"look_direction",Name(cam),lookDir}); return int32(lookDir); }
inline bool __cdecl FakeWaterLevel(float x,float y,float z,float* lvl,uint32 touch,CVector* n) {
    const bool r=NextB(); const float l=waterLevels[waterCalls++%waterLevels.size()]; if(r)*lvl=l;
    trace.push_back({"water_level",F(x),F(y),F(z),touch&0xFF,Name(n),r,F(l)}); return r;
}
inline CEntity* __cdecl FakeTestForClimb(CPed* p,CVector* pos,float* angle,uint8* surface,uint32 launch) {
    trace.push_back({"test_for_climb",Name(p),Name(pos),Name(angle),Name(surface),launch&0xFF,Name(climbRet)});
    *pos=CVector{1.5f,-2.5f,3.25f}; *angle=0.75f; *surface=uint8(17); return static_cast<CEntity*>(climbRet);
}
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { trace.push_back({"peds_group",Name(p),Name(groupRet)}); return static_cast<CPedGroup*>(groupRet); }
inline CPed* __fastcall FakeLeader(const void* m,void*) { trace.push_back({"group_leader",Name(m),Name(leaderRet)}); return static_cast<CPed*>(leaderRet); }
inline CTaskSimpleSwim* __fastcall FakeTaskSwim(void* i,void*) { trace.push_back({"task_swim",Name(i),Name(swimRet)}); return static_cast<CTaskSimpleSwim*>(swimRet); }
inline void __fastcall FakeApplyMoveForce(void* p,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(p),F(x),F(y),F(z)}); }
inline void __fastcall FakeRotateTorso(void* ik,void*,void* bone,const float* o,uint32 flag) { trace.push_back({"rotate_torso",Name(ik),Name(bone),F(o[0]),F(o[1]),flag&0xFF}); }
inline void __fastcall FakeUpdateRwFrame(void* e,void*) { trace.push_back({"update_rw_frame",Name(e)}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t LocalAddress(HMODULE module,const char* name) { auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name)); if(p&&*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1); return p; }
inline uintptr_t Follow(uintptr_t p) { for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=33;
inline const std::array<Fn,NFNS> fns{{
    {"CHeli","AddHeliSearchLight",0x6C45B0,0,-1,256},
    {"CHeli","BurstTyre",0,0x871680,44,64},
    {"CHeli","Fix",0,0x871680,50,128},
    {"CHeli","InitHelis",0x6C4560,0,-1,64},
    {"CHeli","Post_SearchLightCone",0x6C46E0,0,-1,64},
    {"CHeli","Pre_SearchLightCone",0x6C4650,0,-1,64},
    {"CHeli","Render",0,0x871680,18,512},
    {"CHeli","RenderAllHeliSearchLights",0x6C7C50,0,-1,256},
    {"CHeli","SetUpWheelColModel",0,0x871680,43,64},
    {"CHeli","SwitchPoliceHelis",0x6C4800,0,-1,64},
    {"CHeli","TestSniperCollision",0x6C6890,0,-1,1024},
    {"CTaskComplexGoToCarDoorAndStandStill","Clone",0,0x86EC4C,1,256},
    {"CTaskComplexGoToCarDoorAndStandStill","ComputeRouteToDoor",0x645910,0x86EC4C,-1,512},
    {"CTaskComplexGoToCarDoorAndStandStill","Constructor",0x645780,0x86EC4C,-1,256},
    {"CTaskComplexGoToCarDoorAndStandStill","ControlSubTask",0,0x86EC4C,10,2048},
    {"CTaskComplexGoToCarDoorAndStandStill","CreateFirstSubTask",0,0x86EC4C,9,2048},
    {"CTaskComplexGoToCarDoorAndStandStill","CreateNextSubTask",0,0x86EC4C,8,1024},
    {"CTaskComplexGoToCarDoorAndStandStill","CreateSubTask",0x64A5F0,0x86EC4C,-1,512},
    {"CTaskComplexGoToCarDoorAndStandStill","Destructor",0x64A580,0x86EC4C,-1,256},
    {"CTaskComplexGoToCarDoorAndStandStill","GetTaskType",0,0x86EC4C,4,64},
    {"CTaskComplexGoToCarDoorAndStandStill","IsVehicleInRange",0x6458A0,0x86EC4C,-1,1024},
    {"CTaskComplexGoToCarDoorAndStandStill","MakeAbortable",0,0x86EC4C,6,512},
    {"CTaskSimpleSwim","ApplyRollAndPitch",0x68A8E0,0x870920,-1,256},
    {"CTaskSimpleSwim","Constructor",0x688930,0x870920,-1,256},
    {"CTaskSimpleSwim","CreateFxSystem",0x68A9F0,0x870920,-1,128},
    {"CTaskSimpleSwim","DestroyFxSystem",0x68AA50,0x870920,-1,128},
    {"CTaskSimpleSwim","MakeAbortable",0,0x870920,6,512},
    {"CTaskSimpleSwim","ProcessControlAI",0x689640,0x870920,-1,1024},
    {"CTaskSimpleSwim","ProcessControlInput",0x688A90,0x870920,-1,2048},
    {"CTaskSimpleSwim","ProcessEffects",0x68AA70,0x870920,-1,1024},
    {"CTaskSimpleSwim","ProcessPed",0,0x870920,7,1024},
    {"CTaskSimpleSwim","ProcessSwimAnims",0x6899F0,0x870920,-1,1024},
    {"CTaskSimpleSwim","ProcessSwimmingResistance",0x68A1D0,0x870920,-1,1024},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(pedA.data()); clean(pedB.data()); clean(pedC.data()); clean(helis[0].data()); clean(helis[1].data()); clean(climbEnt.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpptrigswim-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpptrigswim-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpptrigswim");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"cpptrigswim-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    HMODULE module{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    const uintptr_t dllEntityRender=LocalAddress(module,"?Render@CEntity@@UAEXXZ");
    if(!dllEntityRender){Write({{"event","error"},{"reason","missing dll export"},{"exports",json::array({"?Render@CEntity@@UAEXXZ"})}});return;}
    dllAtomicCb=std::bit_cast<uintptr_t>(&GetCurrentAtomicObjectCB); dllAtomicCbTarget=Follow(dllAtomicCb);
    // bloque real de la animacion de nadar (se guarda y se restaura)
    auto*& swimGroupBlock=*reinterpret_cast<CAnimBlock**>(*reinterpret_cast<uint8**>(ASSOC_GROUPS)+0x14*ANIM_GROUP_SWIM);
    CAnimBlock* const swimBlockSaved=swimGroupBlock;
    CAnimBlock* const swimBlock=CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(ANIM_GROUP_SWIM));
    if(!swimBlock){Write({{"event","error"},{"reason","swim anim block not found"}});return;}
    const auto swimBlockBytes=ReversingProbe::Bytes(swimBlock,sizeof(CAnimBlock));

    named.clear();
    named.push_back({helis[0].data(),HELI_SIZE,"heli0"}); named.push_back({helis[1].data(),HELI_SIZE,"heli1"});
    named.push_back({heliMat[0].data(),heliMat[0].size(),"heli0_mat"}); named.push_back({heliMat[1].data(),heliMat[1].size(),"heli1_mat"});
    named.push_back({pedA.data(),PED_SIZE,"pedA"}); named.push_back({pedB.data(),PED_SIZE,"pedB"}); named.push_back({pedC.data(),PED_SIZE,"pedC"});
    named.push_back({matA.data(),matA.size(),"matA"}); named.push_back({matB.data(),matB.size(),"matB"}); named.push_back({matC.data(),matC.size(),"matC"});
    named.push_back({pdata.data(),pdata.size(),"pdata"}); named.push_back({clump.data(),clump.size(),"clump"}); named.push_back({intel.data(),intel.size(),"intel"});
    named.push_back({group.data(),group.size(),"group"}); named.push_back({swimTask.data(),swimTask.size(),"swim_task"}); named.push_back({evBuf.data(),evBuf.size(),"event"});
    named.push_back({atomics.data(),atomics.size(),"atomic"}); named.push_back({fxA.data(),fxA.size(),"fxA"}); named.push_back({fxB.data(),fxB.size(),"fxB"});
    named.push_back({fxNew.data(),fxNew.size(),"fx_new"}); named.push_back({climbEnt.data(),climbEnt.size(),"climb_entity"});
    named.push_back({frame.data(),frame.size(),"frame"}); named.push_back({frames.data(),frames.size(),"node_frame"}); named.push_back({hier.data(),hier.size(),"hier"});
    named.push_back({padBuf.data(),padBuf.size(),"pad"}); named.push_back({rwMat.data(),rwMat.size(),"rw_matrix"}); named.push_back({sub.data(),sub.size(),"subtask"});
    named.push_back({object.data(),object.size(),"object"}); named.push_back({boneMats.data(),boneMats.size(),"bone_matrices"}); named.push_back({routes.data(),routes.size(),"route"});
    named.push_back({vecs.data(),sizeof(vecs),"vecs"});
    static const char* an[]{"assoc0","assoc1","assoc2","assoc3","assoc4","assoc5","assoc6","assoc7","assoc8","assoc_blend"};
    static const char* hn[]{"hier0","hier1","hier2","hier3","hier4","hier5","hier6","hier7","hier8","hier_blend"};
    for(size_t k=0;k<=NASSOC;++k){ named.push_back({assocs[k].data(),assocs[k].size(),an[k]}); named.push_back({hiers[k].data(),hiers[k].size(),hn[k]}); }
    named.push_back({heliVtbl.data(),sizeof(heliVtbl),"heli_vtbl"}); named.push_back({subVtbl.data(),sizeof(subVtbl),"sub_vtbl"}); named.push_back({evVtbl.data(),sizeof(evVtbl),"event_vtbl"});
    named.push_back({reinterpret_cast<void*>(HELI_G),HELI_G_N,"heli_globals"});
    FillVTraps(std::make_integer_sequence<int,80>{}); heliVtbl[41]=reinterpret_cast<void*>(&BlowUpCarV);
    FillSTraps(std::make_integer_sequence<int,16>{}); subVtbl[0]=reinterpret_cast<void*>(&SubDtorV); subVtbl[4]=reinterpret_cast<void*>(&SubTypeV); subVtbl[6]=reinterpret_cast<void*>(&SubAbortV);
    FillETraps(std::make_integer_sequence<int,16>{}); evVtbl[1]=reinterpret_cast<void*>(&EvTypeV); evVtbl[2]=reinterpret_cast<void*>(&EvPrioV);

    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    auto* engine=*reinterpret_cast<uint8**>(ENGINE); auto& rsSlot=At<void*>(engine,0x20); void* const rsSaved=rsSlot;
    const uintptr_t camFront=CAM_FRONT+0x238u*(*reinterpret_cast<uint8*>(CAM_ACTIVE));
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{TIMESTEP,4},{FRAMES,4},{HELI_G,HELI_G_N},{POLICE_OK,1},{MOUSE3RD,1},{CAM_ORIENT,4},{camFront,12},
        {FXMULT,0x20},{0x8D2FC0,8},{0x8D2FF4,4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second);
                             rsSlot=rsSaved; swimGroupBlock=swimBlockSaved; std::memcpy(swimBlock,swimBlockBytes.data(),swimBlockBytes.size()); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false;
                            return rsSlot==rsSaved&&swimGroupBlock==swimBlockSaved&&!std::memcmp(swimBlock,swimBlockBytes.data(),swimBlockBytes.size()); };
    auto& lfRef=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); const int32 lf0=lfRef;
    auto fillFreeTasks=[]{ auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                           for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool registered=false; for(size_t f=0;f<fns.size();++f)if(std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleSwim::ProcessControlInput")registered=enabled[f];
    Write({{"event","begin"},{"functions",1},{"pairs",2048},{"registered_hook",registered},{"collaborators_stubbed",true}});
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayer), p1e(0x56E210,FakeFindPlayer);
        // CHeli
        EntryPatch h1(0x6C58E0,FakeSearchLightCone); // la DLL lo reenvia al EXE
        EntryPatch h2(bit_cast<uintptr_t>(&CCollision::DistToLine),FakeDistToLine), h2e(0x417610,FakeDistToLine);
        EntryPatch h3(bit_cast<uintptr_t>(&CVehicleModelInfo::SetVehicleColour),FakeSetVehicleColour), h3e(0x4C84B0,FakeSetVehicleColour);
        EntryPatch h4(0x7F1200,FakeFrameForAll); // la DLL lo reenvia al EXE
        EntryPatch h5(bit_cast<uintptr_t>(&CVehicle::SetComponentAtomicAlpha),FakeSetAlpha), h5e(0x6D2960,FakeSetAlpha);
        EntryPatch h6(dllEntityRender,FakeEntityRender), h6e(0x534310,FakeEntityRender);
        EntryPatch h7(bit_cast<uintptr_t>(&CDamageManager::ResetDamageStatus),FakeResetDamage), h7e(0x6C20E0,FakeResetDamage);
        // ir a la puerta
        EntryPatch g1(bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor),FakeDoorPos), g1e(0x64E740,FakeDoorPos);
        EntryPatch g2(bit_cast<uintptr_t>(&CCarEnterExit::GetNearestCarDoor),FakeNearestDoor), g2e(0x6528F0,FakeNearestDoor);
        EntryPatch g3(bit_cast<uintptr_t>(&CCarEnterExit::GetNearestCarPassengerDoor),FakeNearestPassengerDoor), g3e(0x650BB0,FakeNearestPassengerDoor);
        EntryPatch g4(bit_cast<uintptr_t>(&CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel),FakePathBlocked), g4e(0x651210,FakePathBlocked);
        EntryPatch g5(bit_cast<uintptr_t>(&CCarEnterExit::ComputePassengerIndexFromCarDoor),FakePassengerIndex), g5e(0x64F1E0,FakePassengerIndex);
        EntryPatch g6(bit_cast<uintptr_t>(static_cast<int32(*)(const CVector&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide)),FakeHitSide), g6e(0x5F3AC0,FakeHitSide);
        EntryPatch g7(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes),FakeBBPlanes), g7e(0x5F3660,FakeBBPlanes);
        EntryPatch g8(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,const CVector&,CEntity&,const CVector&,CPointRoute&,int32)>(&CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox)),FakeRouteRound), g8e(0x5F3DD0,FakeRouteRound);
        EntryPatch g9(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CPointRoute::operator new)),FakeRouteNew), g9e(0x41B7C0,FakeRouteNew);
        EntryPatch g10(bit_cast<uintptr_t>(static_cast<void(*)(void*,size_t)>(&CPointRoute::operator delete)),FakeRouteDelete), g10e(0x41B7D0,FakeRouteDelete);
        EntryPatch g11(bit_cast<uintptr_t>(&CTaskSimpleGoToPoint::UpdatePoint),FakeUpdatePoint), g11e(0x645700,FakeUpdatePoint);
        // nadar
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), s1e(0x4D4610,FakeBlend);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FakeGetAssoc), s2e(0x4D68B0,FakeGetAssoc);
        EntryPatch s3(bit_cast<uintptr_t>(&RpAnimBlendClumpSetBlendDeltas),FakeSetBlendDeltas), s3e(0x4D67E0,FakeSetBlendDeltas);
        EntryPatch s4(bit_cast<uintptr_t>(&CAnimBlendAssociation::Start),FakeAssocStart), s4e(0x4CEB70,FakeAssocStart);
        EntryPatch s5(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),FakeAddBlockRef), s5e(0x4D3FB0,FakeAddBlockRef);
        EntryPatch s6(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel), s6e(0x4087E0,FakeRequestModel);
        EntryPatch s7(bit_cast<uintptr_t>(&CPed::RestoreHeadingRate),FakeRestoreHeading), s7e(0x5DFD60,FakeRestoreHeading);
        EntryPatch s8(bit_cast<uintptr_t>(&CPed::SetMoveState),FakeSetMoveState), s8e(0x5DEC00,FakeSetMoveState);
        EntryPatch s9(bit_cast<uintptr_t>(&CPed::Say),FakeSay), s9e(0x5EFFE0,FakeSay);
        EntryPatch s10(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFx), s10e(0x4A9BE0,FakeCreateFx);
        EntryPatch s11(bit_cast<uintptr_t>(&FxSystem_c::Kill),FakeFxKill), s11e(0x4AA3F0,FakeFxKill);
        EntryPatch s12(bit_cast<uintptr_t>(&FxSystem_c::Play),FakeFxPlay), s12e(0x4AA2F0,FakeFxPlay);
        EntryPatch s13(bit_cast<uintptr_t>(&FxSystem_c::PlayAndKill),FakeFxPlayAndKill), s13e(0x4AA3D0,FakeFxPlayAndKill);
        EntryPatch s14(bit_cast<uintptr_t>(&FxSystem_c::CopyParentMatrix),FakeFxCopyParent), s14e(0x4AA890,FakeFxCopyParent);
        EntryPatch s15(bit_cast<uintptr_t>(&FxSystem_c::SetMatrix),FakeFxSetMatrix), s15e(0x4AA630,FakeFxSetMatrix);
        EntryPatch s16(bit_cast<uintptr_t>(&FxSystem_c::SetLocalParticles),FakeFxSetLocal), s16e(0x4AA910,FakeFxSetLocal);
        EntryPatch s17(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle), s17e(0x4AA440,FakeAddParticle);
        EntryPatch s18(bit_cast<uintptr_t>(&Fx_c::TriggerWaterSplash),FakeTriggerSplash), s18e(0x4A1070,FakeTriggerSplash);
        EntryPatch s19(bit_cast<uintptr_t>(&CAEPedAudioEntity::AddAudioEvent),FakeAddAudioEvent), s19e(0x4E2BB0,FakeAddAudioEvent);
        EntryPatch s20(0x7F2A50,FakeMatrixCreate), s21(0x7F18A0,FakeMatrixUpdate), s22(0x7F2A20,FakeMatrixDestroy); // la DLL los reenvia al EXE
        EntryPatch s23(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),FakeHierFromClump), s23e(0x734A40,FakeHierFromClump);
        EntryPatch s24(0x7C51A0,FakeIDGetIndex), s25(0x7C5120,FakeMatrixArray); // la DLL los reenvia al EXE
        EntryPatch s26(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),FakeFatMod), s26e(0x559AF0,FakeFatMod);
        EntryPatch s27(bit_cast<uintptr_t>(&CStats::UpdateStatsWhenSwimming),FakeStatsSwim), s27e(0x55C990,FakeStatsSwim);
        EntryPatch s28(bit_cast<uintptr_t>(&CPlayerPed::HandlePlayerBreath),FakeBreath), s28e(0x60A8D0,FakeBreath);
        EntryPatch s29(bit_cast<uintptr_t>(&CPlayerPed::HandleSprintEnergy),FakeSprintEnergy), s29e(0x60A550,FakeSprintEnergy);
        EntryPatch s30(bit_cast<uintptr_t>(&CPlayerPed::ControlButtonSprint),FakeControlSprint), s30e(0x60A610,FakeControlSprint);
        EntryPatch s31(bit_cast<uintptr_t>(&CPlayerPed::GetButtonSprintResults),FakeSprintResults), s31e(0x60A820,FakeSprintResults);
        EntryPatch s32(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),FakeGetPad), s32e(0x609560,FakeGetPad);
        EntryPatch s33(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown)),FakeWalkUD), s33e(0x53FD30,FakeWalkUD);
        EntryPatch s34(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight)),FakeWalkLR), s34e(0x53FC90,FakeWalkLR);
        EntryPatch s35(bit_cast<uintptr_t>(&CPad::WeaponJustDown),FakeWeaponJust), s35e(0x540250,FakeWeaponJust);
        EntryPatch s36(bit_cast<uintptr_t>(&CGameLogic::IsPlayerUse2PlayerControls),FakeTwoPlayer), s36e(0x442020,FakeTwoPlayer);
        EntryPatch s37(bit_cast<uintptr_t>(&CGameLogic::IsPlayerAllowedToGoInThisDirection),FakeAllowedDir), s37e(0x441E10,FakeAllowedDir);
        EntryPatch s38(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeRadianAngle), s38e(0x53CBE0,FakeRadianAngle);
        EntryPatch s39(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),FakeLimitRadian), s39e(0x53CB50,FakeLimitRadian);
        EntryPatch s40(bit_cast<uintptr_t>(&CGeneral::GetAngleBetweenPoints),FakeAngleBetween), s40e(0x53CEA0,FakeAngleBetween);
        EntryPatch s41(bit_cast<uintptr_t>(&CGeneral::LimitAngle),FakeLimitAngle), s41e(0x53CB00,FakeLimitAngle);
        EntryPatch s42(bit_cast<uintptr_t>(&CCamera::GetLookDirection),FakeLookDir), s42e(0x50AE90,FakeLookDir);
        EntryPatch s43(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),FakeWaterLevel), s43e(0x6EB690,FakeWaterLevel);
        EntryPatch s44(bit_cast<uintptr_t>(&CTaskSimpleClimb::TestForClimb),FakeTestForClimb), s44e(0x6803A0,FakeTestForClimb);
        EntryPatch s45(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakePedsGroup), s45e(0x5F7E80,FakePedsGroup);
        EntryPatch s46(bit_cast<uintptr_t>(&CPedGroupMembership::GetLeader),FakeLeader), s46e(0x5F69A0,FakeLeader);
        EntryPatch s47(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),FakeTaskSwim), s47e(0x601070,FakeTaskSwim);
        EntryPatch s48(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeApplyMoveForce), s48e(0x5429F0,FakeApplyMoveForce);
        EntryPatch s49(bit_cast<uintptr_t>(&CPedIK::RotateTorso),FakeRotateTorso), s49e(0x5FDDB0,FakeRotateTorso);
        EntryPatch s50(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),FakeUpdateRwFrame), s50e(0x532B00,FakeUpdateRwFrame);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(full!="CTaskSimpleSwim::ProcessControlInput")continue;
            if(!only.empty()&&only!=full)continue;
            const bool isHeli=cls=="CHeli", isGoTo=cls=="CTaskComplexGoToCarDoorAndStandStill", isSwim=cls=="CTaskSimpleSwim";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto pick=[&](std::initializer_list<float> l){ return l.begin()[next()%l.size()]; };
                auto run=[&](bool candidate)->json {
                    { lfRef=lf0; fillFreeTasks(); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x11A1u; if(!st)st=1; trace=json::array(); freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    rngState=next(); maskA=next(); maskB=next(); maskC=next(); callsA=callsB=callsC=0; distCalls=0; waterCalls=0; routeUsed=0;
                    std::memset(routes.data(),0xCD,routes.size());
                    *reinterpret_cast<uint32*>(TIMER)=next()%3 ? 100000+next()%1000000 : next();
                    *reinterpret_cast<float*>(TIMESTEP)=next()%3 ? 1.f : pick({0.5f,2.f,uni(0.1f,3.f),0.02f});
                    *reinterpret_cast<uint32*>(FRAMES)=next();
                    const float ts=*reinterpret_cast<float*>(TIMESTEP);
                    // --- peatones falsos ---
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; At<void*>(clump.data(),4)=frame.data();
                    std::memset(frame.data(),0,frame.size());
                    { CMatrix m{}; m.SetRotate(uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,5.f)};
                      auto* rw=frame.data()+0x10; std::memcpy(rw,&m.GetRight(),12); std::memcpy(rw+0x10,&m.GetForward(),12); std::memcpy(rw+0x20,&m.GetUp(),12); std::memcpy(rw+0x30,&m.GetPosition(),12); At<uint32>(rw,0xC)=3; }
                    auto makePed=[&](std::array<uint8,PED_SIZE>& buf,std::array<uint8,sizeof(CMatrixLink)>& mat,uint32 pedType){
                        std::memset(buf.data(),0,buf.size()); std::memset(mat.data(),0,mat.size());
                        auto* p=reinterpret_cast<CPed*>(buf.data()); p->SetType(ENTITY_TYPE_PED); At<void*>(p,E_CLUMP)=clump.data();
                        At(p,P_FLAGS)=next()&~0x100u; At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next(); At(p,P_PEDTYPE)=pedType;
                        At<uint16>(p,0x20)=uint16(next());
                        CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-20.f,20.f),uni(-20.f,20.f),next()%8==0 ? pick({-69.f,-70.f,std::nextafter(-69.f,0.f)}) : uni(-3.f,3.f)};
                        std::memcpy(mat.data(),&m,sizeof(CMatrix)); At<void*>(p,E_MATRIX)=mat.data();
                        for(size_t k=0;k<3;++k)At<float>(p,PH_MOVE+4*k)=uni(-0.5f,0.5f);
                        At<float>(p,PH_MASS)=uni(50.f,150.f); At(p,PH_FLAGS)=next();
                        At<float>(p,P_SHIFT)=uni(-0.2f,0.2f); At<float>(p,P_SHIFT+4)=uni(-0.2f,0.2f);
                        At<float>(p,P_CURROT)=uni(-3.2f,3.2f); At<float>(p,P_AIMROT)=uni(-3.2f,3.2f);
                        At<int32>(p,P_ANIMGRP)=int32(next()%60); At<int32>(p,P_MOVE)=int32(next()%8); At<int32>(p,P_SWIMMOVE)=int32(next()%8);
                        At<int32>(p,P_STATE)=next()%3==0 ? 63 : int32(next()%60);
                        At<void*>(p,P_BONE1)=atomics.data()+8; At<uint8>(p,P_SLOT)=uint8(next()%13);
                        for(size_t k=0;k<13;++k)At<void*>(p,P_WEAPONS+0x1C*k+0x18)=next()%3==0 ? static_cast<void*>(fxB.data()) : nullptr;
                        return p;
                    };
                    static const uint32 typesA[]{0,0,1,4,6,7}; static const uint32 typesB[]{0,1,4,6};
                    CPed* const pa=makePed(pedA,matA,typesA[next()%6]);
                    CPed* const pb=makePed(pedB,matB,typesB[next()%4]);
                    CPed* const pc=makePed(pedC,matC,typesB[next()%4]);
                    At<void*>(pb,P_INTEL)=intel.data();
                    for(auto& b:pdata)b=0;
                    At<float>(pdata.data(),0x14)=pick({0.f,0.25f,0.5f,std::nextafter(0.5f,1.f),1.f,uni(0.f,1.2f)});
                    At<float>(pdata.data(),0x18)=pick({0.f,-1.f,0.05f,0.1f,uni(-1.f,1.f)});
                    At<float>(pdata.data(),0x44)=uni(0.f,1500.f); At<float>(pdata.data(),0x90)=uni(-2.f,2.f);
                    const bool aIsPlayer=At(pa,P_PEDTYPE)<=1;
                    At<void*>(pa,P_PDATA)=aIsPlayer||next()%4==0 ? static_cast<void*>(pdata.data()) : nullptr;
                    if(nm=="ProcessControlInput")At<void*>(pa,P_PDATA)=pdata.data(); // el EXE lee m_pPlayerData sin comprobar
                    // --- otros buffers ---
                    for(auto& b:intel)b=uint8(next()); for(auto& b:group)b=uint8(next()); for(auto& b:atomics)b=uint8(next());
                    for(auto& b:fxA)b=uint8(next()); for(auto& b:fxB)b=uint8(next()); for(auto& b:fxNew)b=uint8(next());
                    std::memset(climbEnt.data(),0,climbEnt.size()); reinterpret_cast<CEntity*>(climbEnt.data())->SetType(ENTITY_TYPE_BUILDING);
                    for(auto& b:frames)b=uint8(next()); for(auto& b:hier)b=uint8(next());
                    std::memset(evBuf.data(),0,evBuf.size()); At<void*>(evBuf.data(),0)=evVtbl.data();
                    { static const int32 evt[]{9,9,33,66,1,0}; At<int32>(evBuf.data(),0x30)=evt[next()%6]; At<int32>(evBuf.data(),0x34)=int32(pick({70.f,71.f,72.f,10.f,0.f})); }
                    At<uint8>(evBuf.data(),0x0C)=uint8(next()%2); At<uint8>(evBuf.data(),0x40)=uint8(next()%2);
                    void* const ev=next()%5 ? static_cast<void*>(evBuf.data()) : nullptr;
                    const int prio=next()%3==0 ? 2 : int(next()%4);
                    for(size_t k=0;k<boneMats.size()/4;++k)At<float>(boneMats.data(),4*k)=uni(-3.f,3.f);
                    { const float pz=At<float>(matA.data(),0x38);
                      for(size_t k:{size_t(10),size_t(11),size_t(12),size_t(13)})At<float>(boneMats.data(),0x40*k+0x38)=pick({pz,pz+0.05f,pz-0.05f,std::nextafter(pz+0.05f,0.f),pz+uni(-0.1f,0.1f)});
                      std::memcpy(boneMats.data()+0x40*8,boneMats.data()+0x40*3,0x40); } // FIX_BUGS: matrices 3 y 8 iguales
                    // asociaciones de animacion
                    static const uint32 animIds[NASSOC]{14,311,312,313,314,315,316,128,3};
                    assocById.fill(nullptr);
                    for(size_t k=0;k<=NASSOC;++k){
                        auto* a=assocs[k].data(); auto* h=hiers[k].data(); for(auto& b:assocs[k])b=uint8(next()); for(auto& b:hiers[k])b=uint8(next());
                        At<void*>(a,A_HIER)=h; const float ct=uni(0.f,2.f); At<float>(a,A_TIME)=ct; At<float>(a,A_STEP)=uni(0.f,0.2f);
                        At<float>(a,A_BLEND)=pick({0.f,1.f,0.99f,std::nextafter(0.99f,0.f),std::nextafter(1.f,0.f),uni(0.f,1.f)});
                        At<float>(a,A_DELTA)=pick({0.f,-1.f,1.f,-0.f,uni(-4.f,4.f)}); At<float>(a,A_SPEED)=uni(0.f,2.f);
                        At<int16>(a,A_ID)=int16(k<NASSOC ? animIds[k] : (next()%2 ? 128 : 316)); At<uint16>(a,A_FLAGS)=uint16(next());
                        At<float>(h,0x10)=next()%3==0 ? ct : next()%2 ? 4.f*(ct+At<float>(a,A_STEP)) : uni(0.f,3.f);
                        if(k<NASSOC&&next()%3)assocById[animIds[k]]=a;
                    }
                    // mando falso: JumpJustDown se evalua de verdad
                    for(size_t k=0;k<padBuf.size()/2;++k)At<int16>(padBuf.data(),2*k)=next()%4==0 ? int16(next()) : int16(0);
                    walkUD=next()%3==0 ? int16(0) : next()%2 ? int16(pick({128.f,-128.f,64.f,-32.f})) : int16(int32(next()%257)-128);
                    walkLR=next()%3==0 ? int16(0) : next()%2 ? int16(pick({128.f,-128.f,64.f,-32.f})) : int16(int32(next()%257)-128);
                    weaponJust=next()%4==0; twoPlayer=next()%3==0; allowedDir=next()%3!=0; lookDir=next()%3 ? 3u : next()%3;
                    sprintCtl=pick({0.f,0.5f,1.f,std::nextafter(1.f,0.f),2.f}); sprintRes2=pick({0.f,0.5f,1.f,1.5f}); sprintRes3=pick({0.f,0.5f,0.7f,1.f,std::nextafter(1.f,0.f),1.5f});
                    fatMod=pick({1000.f,500.f,1500.f,uni(100.f,2000.f)});
                    *reinterpret_cast<uint8*>(MOUSE3RD)=uint8(next()%3==0); *reinterpret_cast<float*>(CAM_ORIENT)=uni(-3.2f,3.2f);
                    { auto* cf=reinterpret_cast<float*>(camFront); cf[0]=uni(-1.f,1.f); cf[1]=uni(-1.f,1.f); cf[2]=next()%4==0 ? 0.f : uni(-0.9f,0.9f); }
                    { const float pz=At<float>(matA.data(),0x38); for(auto& w:waterLevels)w=pick({pz+2.f,std::nextafter(pz+2.f,1000.f),pz+0.5f,std::nextafter(pz+0.5f,1000.f),pz+0.65f,pz+uni(-1.f,3.f),pz-0.2f}); }
                    climbRet=next()%2 ? static_cast<void*>(climbEnt.data()) : nullptr;
                    groupRet=next()%3 ? static_cast<void*>(group.data()) : nullptr;
                    leaderRet=next()%3 ? static_cast<void*>(pb) : next()%2 ? static_cast<void*>(pc) : nullptr;
                    std::memset(swimTask.data(),0,swimTask.size()); At<uint16>(swimTask.data(),0xA)=uint16(next()%2 ? 2 : next()%6);
                    swimRet=next()%4 ? static_cast<void*>(swimTask.data()) : nullptr;
                    fxRet=next()%3 ? static_cast<void*>(fxNew.data()) : nullptr;
                    passIdx=next()%8; hitSides=next();
                    for(auto& d:doorPos){ d[0]=uni(-20.f,20.f); d[1]=uni(-20.f,20.f); d[2]=uni(-3.f,3.f); }
                    for(auto& p:planes)p=uni(-1.f,1.f); for(auto& d:planeDots)d=uni(-5.f,5.f); routeCount=next()%9; // como mucho 8 (el EXE recorre la ruta local sin limite)
                    for(auto& d:dist)d=pick({0.8f,std::nextafter(0.8f,0.f),std::nextafter(0.8f,1.f),0.f,uni(0.f,2.f)});
                    for(auto& v:vecs)v=uni(-30.f,30.f);
                    // --- vehiculos falsos (del tamano de CHeli) ---
                    for(size_t h=0;h<2;++h){
                        auto& buf=helis[h]; std::memset(buf.data(),0,buf.size()); std::memset(heliMat[h].data(),0,heliMat[h].size());
                        auto* v=reinterpret_cast<CVehicle*>(buf.data()); v->SetType(ENTITY_TYPE_VEHICLE); At<void*>(v,0)=heliVtbl.data();
                        CMatrix m{}; m.SetRotate(uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-3.f,30.f)};
                        std::memcpy(heliMat[h].data(),&m,sizeof(CMatrix)); At<void*>(v,E_MATRIX)=next()%8||nm=="TestSniperCollision" ? static_cast<void*>(heliMat[h].data()) : nullptr;
                        if(!At<void*>(v,E_MATRIX)){ At<float>(v,E_POS)=uni(-20.f,20.f); At<float>(v,E_POS+4)=uni(-20.f,20.f); At<float>(v,E_POS+8)=uni(-3.f,3.f); }
                        At(v,PH_FLAGS)=next(); At(v,V_FLAGS)=next(); At(v,V_FLAGS+4)=next();
                        { static const int16 models[]{497,487,488,417,425,563}; At<int16>(v,E_MODEL)=models[next()%6]; }
                        for(size_t k=0;k<4;++k)At<uint8>(v,V_COLORS+k)=uint8(next());
                        At<void*>(v,V_DRIVER)=next()%3 ? static_cast<void*>(pb) : nullptr;
                        for(size_t k=0;k<8;++k){ const uint32 r=next()%4; At<void*>(v,V_PASS+4*k)=r==0 ? static_cast<void*>(pc) : r==1 ? static_cast<void*>(pb) : nullptr; }
                        for(size_t k=0;k<4;++k)At<void*>(v,V_NODES+4*(12+k))=next()%4 ? static_cast<void*>(frames.data()+0x20*k) : nullptr;
                        At<float>(v,V_ROTBAL)=uni(-1.f,1.f); At<uint8>(v,H_SWAT)=uint8(next()%5);
                    }
                    for(size_t k=0;k<4;++k)nodeAtomic[k]=next()%3 ? static_cast<void*>(atomics.data()+4*k) : nullptr;
                    auto* const veh=helis[0].data();
                    modelInfo=uintptr_t(CModelInfo::GetModelInfo(At<int16>(veh,E_MODEL)));

                    json out={{"switches",false}}; json ret=nullptr;
                    const auto occ=Occupancy();
                    void* const o=object.data();

                    // --- fixtures por clase ---
                    if(isHeli) {
                        auto* lights=reinterpret_cast<uint8*>(HELI_G+0x30);
                        for(size_t k=0;k<4*0x4C;++k)lights[k]=uint8(next());
                        for(size_t k=0;k<4;++k)for(size_t w=0;w<8;++w)At<float>(lights,0x4C*k+4*w)=uni(-100.f,100.f);
                        At<uint32>(reinterpret_cast<void*>(HELI_G),0)=next(); // TestForNewRandomHelisTimer
                        At<void*>(reinterpret_cast<void*>(HELI_G),4)=next()%4 ? static_cast<void*>(helis[0].data()) : nullptr;
                        At<void*>(reinterpret_cast<void*>(HELI_G),8)=next()%3 ? static_cast<void*>(helis[1].data()) : nullptr;
                        At<uint32>(reinterpret_cast<void*>(HELI_G),0xC)=nm=="AddHeliSearchLight" ? next()%4 : next()%7;
                        *reinterpret_cast<uint8*>(POLICE_OK)=uint8(next()%2);
                    }
                    if(isGoTo) {
                        std::memset(o,0xCD,object.size());
                        At<uintptr_t>(o,0)=GOTO_VT; At<void*>(o,4)=nullptr;
                        At<void*>(o,0xC)=nm=="Destructor"&&next()%4==0 ? nullptr : static_cast<void*>(veh);
                        At<int32>(o,0x10)=int32(pick({4.f,6.f,7.f})); At<uint8>(o,0x14)=uint8(next()%2);
                        At<float>(o,0x18)=uni(0.1f,1.f); At<float>(o,0x1C)=uni(0.f,2.f); At<int32>(o,0x24)=int32(next()%3==0 ? -1 : next()%5000);
                        At<uint32>(o,0x28)=next(); At<uint32>(o,0x2C)=next(); At<uint8>(o,0x30)=uint8(next()%2); At<uint8>(o,0x31)=uint8(next()%2);
                        At<int32>(o,0x34)=next()%4==0 ? 0 : int32(8+next()%4);
                        At<int32>(o,0x44)=next()%2 ? 0 : int32(8+next()%4);
                        At<uint8>(o,0x48)=uint8(next()%4==0); At<uint8>(o,0x49)=uint8(next()%2);
                        // radio de busqueda: a veces exactamente en el borde (d·d == r·r)
                        { const float r=pick({3.f,5.f,10.f,uni(1.f,30.f)}); At<float>(o,0x20)=r;
                          if(next()%3==0){ // vehiculo y peaton en coordenadas exactas: d.d == r.r
                              const CVector vp{2.f,-3.f,1.f};
                              if(auto* vm=At<CMatrix*>(veh,E_MATRIX))vm->GetPosition()=vp; else { At<float>(veh,E_POS)=vp.x; At<float>(veh,E_POS+4)=vp.y; At<float>(veh,E_POS+8)=vp.z; }
                              const int ax=int(next()%3); CVector pp=vp; (&pp.x)[ax]+=next()%2 ? r : -r;
                              if(next()%2)(&pp.x)[ax]=std::nextafter((&pp.x)[ax],next()%2 ? 1e9f : -1e9f);
                              reinterpret_cast<CMatrix*>(matA.data())->GetPosition()=pp; } }
                        // punto objetivo: a veces la puerta exacta o a 0,01
                        { const auto& d=doorPos[uint32(At<int32>(o,0x34))%4]; CVector t{d[0],d[1],d[2]};
                          if(next()%3==0)t.x+=pick({0.01f,std::nextafter(0.01f,1.f),std::nextafter(0.01f,0.f),0.5f,0.f});
                          else if(next()%3==0)t=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-3.f,3.f)};
                          At<float>(o,0x38)=t.x; At<float>(o,0x3C)=t.y; At<float>(o,0x40)=t.z;
                          if(next()%4==0){ const float pz=At<float>(matA.data(),0x38); auto* pm=reinterpret_cast<CMatrix*>(matA.data());
                              pm->GetPosition()=CVector{t.x+pick({0.5f,0.f,std::nextafter(0.5f,0.f),0.3f}),t.y,pz}; } }
                        // ruta: arena propia (el hueco 5 hace de ruta existente)
                        auto* existing=routes.data()+ROUTE*(NROUTES-1); for(size_t k=0;k<ROUTE;++k)existing[k]=uint8(next()); At<uint32>(existing,0)=next()%9;
                        for(size_t k=0;k<8*3;++k)At<float>(existing,4+4*k)=uni(-10.f,10.f);
                        At<void*>(o,0x4C)=next()%2 ? static_cast<void*>(existing) : nullptr;
                        // subtarea
                        std::memset(sub.data(),0,sub.size()); At<void*>(sub.data(),0)=subVtbl.data();
                        { static const int32 types[]{828,202,203,810,900,905,900,905,400,1302}; At<int32>(sub.data(),0x2C)=types[next()%10]; }
                        At<float>(sub.data(),0x14)=At<float>(matA.data(),0x38)+pick({3.f,-3.f,std::nextafter(3.f,0.f),0.f,uni(-5.f,5.f)});
                        At<uint8>(sub.data(),0x20)=uint8(next());
                        At<void*>(o,8)=nm=="Destructor"&&next()%3==0 ? nullptr : static_cast<void*>(sub.data());
                        if(nm=="Destructor"&&At<void*>(o,0xC))static_cast<CEntity*>(At<void*>(o,0xC))->RegisterReference(reinterpret_cast<CEntity**>(static_cast<uint8*>(o)+0xC));
                    }
                    if(isSwim) {
                        std::memset(o,0xCD,object.size());
                        At<uintptr_t>(o,0)=SWIM_VT; At<void*>(o,4)=nullptr;
                        At<uint8>(o,8)=uint8(next()%4!=0); At<uint8>(o,9)=uint8(next()%4!=0);
                        At<uint16>(o,0xA)=uint16(next()%16==0 ? 6 : next()%6);
                        { static const int32 ids[]{14,191,311,312,313,314,315,316,128,3}; At<int32>(o,0xC)=ids[next()%10]; }
                        At<float>(o,0x10)=pick({-1.f,0.5f,1.f,1.5f,2.5f,0.f,uni(-1.f,3.f)});
                        { const CVector pp=reinterpret_cast<CMatrix*>(matA.data())->GetPosition(); CVector vp;
                          switch(next()%4){ case 0: vp=CVector{0.f,0.f,0.f}; break; case 1: vp=CVector{pp.x+pick({0.5f,1.f,2.f,0.f}),pp.y,pp.z}; break; default: vp=CVector{pp.x+uni(-6.f,6.f),pp.y+uni(-6.f,6.f),pp.z+uni(-1.f,1.f)}; }
                          At<float>(o,0x14)=vp.x; At<float>(o,0x18)=vp.y; At<float>(o,0x1C)=vp.z; }
                        At<void*>(o,0x20)=next()%3 ? static_cast<void*>(pb) : nullptr;
                        for(size_t off:{size_t(0x24),size_t(0x28),size_t(0x2C),size_t(0x30)})At<float>(o,off)=pick({0.f,0.01f,-0.01f,std::nextafter(0.01f,1.f),std::nextafter(-0.01f,-1.f),uni(-1.5f,1.5f),0.8f,-1.4f});
                        At<float>(o,0x34)=pick({0.f,-1.f,-2.f,-1.5f,0.001f,std::nextafter(0.001f,1.f),0.02f,0.025f,0.03f,0.05f,uni(-0.1f,0.1f)});
                        At<void*>(o,0x38)=nullptr;
                        for(size_t k=0;k<3;++k)At<float>(o,0x3C+4*k)=uni(-5.f,5.f); At<float>(o,0x48)=uni(-3.f,3.f); At<uint32>(o,0x4C)=next()%180;
                        At<float>(o,0x50)=pick({0.f,1.5f,2.f,2.5f,3.f});
                        At<float>(o,0x54)=pick({0.f,15.f,std::nextafter(15.f,100.f),20.f,uni(0.f,14.f)});
                        At<uint32>(o,0x58)=next()%3==0 ? 0u : uint32(pick({1.f,20.f,30.f,100.f,1000.f}));
                        At<void*>(o,0x5C)=next()%2 ? static_cast<void*>(fxA.data()) : nullptr;
                        At<uint8>(o,0x60)=uint8(next()%2);
                        if(!At<void*>(o,0x20))At(pa,P_FLAGS4)&=~0x2000u; // el EXE lee m_pPed sin comprobar con bCanClimbOntoBoat
                        if(nm=="ProcessPed"&&next()%8==0)At<void*>(o,0x38)=climbEnt.data();
                        if(nm=="ProcessPed"&&next()%8==0)At(pa,P_FLAGS)|=0x100u; // bInVehicle
                        // bloque de animacion de nadar: puntero del grupo y bandera de cargado
                        swimGroupBlock=next()%4 ? swimBlock : nullptr; swimBlock->IsLoaded=next()%2!=0;
                    }

                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    out["switches"]=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; bool hasRaw=false;
                    if(nm=="AddHeliSearchLight"){ reinterpret_cast<void(__cdecl*)(const float*,const float*,float,float,uint32,uint32,uint32)>(target)(vecs.data(),vecs.data()+3,uni(0.f,10.f),uni(0.f,1.f),next(),next(),next()); }
                    else if(nm=="BurstTyre")ret=reinterpret_cast<uint8(__thiscall*)(void*,uint32,uint32)>(target)(veh,next(),next());
                    else if(nm=="SetUpWheelColModel")ret=reinterpret_cast<uint8(__thiscall*)(void*,void*)>(target)(veh,atomics.data());
                    else if(isHeli&&(nm=="Fix"||nm=="Render"))reinterpret_cast<void(__thiscall*)(void*)>(target)(veh);
                    else if(nm=="InitHelis"||nm=="RenderAllHeliSearchLights")reinterpret_cast<void(__cdecl*)()>(target)();
                    else if(nm=="Pre_SearchLightCone"||nm=="Post_SearchLightCone"){ rsSlot=reinterpret_cast<void*>(&FakeRenderState); reinterpret_cast<void(__cdecl*)()>(target)(); rsSlot=rsSaved; }
                    else if(nm=="SwitchPoliceHelis")reinterpret_cast<void(__cdecl*)(uint32)>(target)(next()%2);
                    else if(nm=="TestSniperCollision"){
                        float* a=vecs.data(); float* b=vecs.data()+3;
                        switch(next()%4){ case 0: b[0]=a[0]; b[1]=a[1]; b[2]=a[2]+uni(-5.f,5.f); break; case 1: b[0]=a[0]; b[1]=a[1]; b[2]=a[2]; break; default: break; }
                        ret=reinterpret_cast<uint8(__cdecl*)(float*,float*)>(target)(a,b)!=0;
                    }
                    else if(isGoTo){
                        if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,int32,uint32,int32,float,float,float,int32)>(target)(o,next()%6 ? veh : nullptr,int32(next()%8),next()%2,next()%2 ? 0 : int32(8+next()%4),uni(0.f,2.f),uni(0.f,5.f),uni(1.f,30.f),int32(next()%5000))); hasRaw=true; }
                        else if(nm=="Destructor")reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Clone"){ rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); hasRaw=true; }
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int32,const void*)>(target)(o,pa,prio,ev)!=0;
                        else if(nm=="IsVehicleInRange")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*)>(target)(o,pa)!=0;
                        else if(nm=="ComputeRouteToDoor")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pa);
                        else if(nm=="CreateSubTask"){ static const int32 types[]{828,202,203,810,905,900,1302,0,400};
                            const int32 t=types[next()%9]; if(t==905&&!At<void*>(o,0x4C))At<void*>(o,0x4C)=routes.data()+ROUTE*(NROUTES-1);
                            rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,t,pa)); hasRaw=true; }
                        else { rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,pa)); hasRaw=true; }
                    }
                    else if(isSwim){
                        if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const float*,CPed*)>(target)(o,next()%4 ? vecs.data() : nullptr,next()%3 ? pb : nullptr)); hasRaw=true; }
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int32,const void*)>(target)(o,pa,prio,ev)!=0;
                        else if(nm=="ProcessPed")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*)>(target)(o,pa)!=0;
                        else if(nm=="CreateFxSystem")reinterpret_cast<void(__thiscall*)(void*,CPed*,void*)>(target)(o,pa,next()%2 ? static_cast<void*>(rwMat.data()) : nullptr);
                        else if(nm=="DestroyFxSystem")reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pa);
                    }
                    setState(false);
                    // tareas nuevas del pool
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(hasRaw)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,4,0x80)}});
                    const bool skipVptr=nm=="Constructor"||nm=="Destructor";
                    json objectWords=Region(o,skipVptr ? 4 : 0,isSwim ? 0x64 : 0x50);
                    if(nm=="Constructor")objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo en vez del vptr
                    json refs=json::array(); auto refList=[&](void* e){json l=json::array();for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(void* e:{static_cast<void*>(pedA.data()),static_cast<void*>(pedB.data()),static_cast<void*>(pedC.data()),static_cast<void*>(helis[0].data()),static_cast<void*>(helis[1].data()),static_cast<void*>(climbEnt.data())})refs.push_back(refList(e));
                    out["ret"]=ret; out["object"]=objectWords; out["tasks"]=tasks; out["refs"]=refs;
                    out["pedA"]=EntHash(pedA); out["pedB"]=EntHash(pedB); out["pedC"]=EntHash(pedC); out["heli0"]=EntHash(helis[0]); out["heli1"]=EntHash(helis[1]);
                    out["ped_fields"]={At(pa,P_FLAGS),At(pa,P_FLAGS3),At(pa,P_MOVE),At(pa,P_SWIMMOVE),At(pa,P_CURROT),At(pa,P_AIMROT),At(pa,P_SHIFT),At(pa,P_SHIFT+4),
                                       At(pa,PH_MOVE),At(pa,PH_MOVE+4),At(pa,PH_MOVE+8),Region(matA.data(),0x30,0x3C)};
                    out["heli_fields"]={At(veh,V_FLAGS),At(veh,V_FLAGS+4),At(veh,V_TIMETILL),At(veh,V_ROTBAL),At<uint8>(veh,H_SWAT),At(helis[1].data(),V_ROTBAL),At<uint8>(helis[1].data(),H_SWAT)};
                    out["pdata"]=Region(pdata.data(),0,pdata.size()); out["frame"]=Region(frame.data(),0x10,0x50);
                    { json a=json::array(); for(auto& b:assocs)a.push_back(Region(b.data(),0x14,0x30)); out["assocs"]=a; }
                    out["routes"]=Region(routes.data(),0,routes.size()); out["heli_globals"]=Region(reinterpret_cast<void*>(HELI_G),0,HELI_G_N); out["police"]=*reinterpret_cast<uint8*>(POLICE_OK);
                    out["sub"]=Hash(sub.data(),sub.size()); out["event"]=Hash(evBuf.data(),evBuf.size()); out["pad"]=Hash(padBuf.data(),padBuf.size());
                    out["mats"]={Hash(matB.data(),matB.size()),Hash(matC.data(),matC.size()),Hash(heliMat[0].data(),heliMat[0].size()),Hash(heliMat[1].data(),heliMat[1].size())};
                    out["misc"]={Hash(clump.data(),clump.size()),Hash(fxA.data(),fxA.size()),Hash(fxB.data(),fxB.size()),Hash(fxNew.data(),fxNew.size()),EntHash(climbEnt),
                                 Hash(swimTask.data(),swimTask.size()),Hash(group.data(),group.size()),Hash(intel.data(),intel.size()),Hash(atomics.data(),atomics.size()),Hash(frames.data(),frames.size()),
                                 Hash(boneMats.data(),boneMats.size()),Hash(vecs.data(),sizeof(vecs)),Hash(rwMat.data(),rwMat.size())};
                    out["block"]={Name(swimGroupBlock),swimBlock->IsLoaded,swimBlock->RefCnt};
                    restoreGlobals();
                    // borrado de las tareas creadas (su traza va aparte)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    out["trace"]=trace; out["delete_trace"]=delTrace; out["clean"]=clean;
                    freshIds.clear();
                    CleanRefs();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",original},{"candidate",candidate}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs();
    const bool globalsTouched=!globalsIntact();
    restoreGlobals(); lfRef=lf0; fillFreeTasks();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},{"state_restored",restored}});
}
}
