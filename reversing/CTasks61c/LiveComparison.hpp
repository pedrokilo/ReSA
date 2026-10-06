#pragma once
// Banco dentro del motor: ocho clases de tareas y atractores (lista 61c).
// CTaskComplexGoToPointAndStandStillAndAchieveHeading (constructor, destructor, Clone, CreateNextSubTask,
// CreateFirstSubTask, ControlSubTask), CTaskComplexGoToPointAnyMeans (2 constructores, destructor, CreateSubTask,
// CreateNextSubTask, CreateFirstSubTask, ControlSubTask), CTaskComplexGotoDoorAndOpen (2 constructores, destructor, Clone,
// CreateNextSubTask, CreateFirstSubTask, ControlSubTask), CTaskComplexGoToPointAiming (constructor, destructor,
// CreateSubTask, Clone, GetTaskType, CreateNextSubTask, CreateFirstSubTask, ControlSubTask),
// CTaskComplexUseClosestFreeScriptedAttractor (constructor, destructor, ComputeClosestFreeScriptedEffect, Clone, GetTaskType,
// CreateNextSubTask, CreateFirstSubTask, ControlSubTask), CPedShelterAttractor (GetDisplacement, ComputeAttractPos,
// ComputeAttractHeading, BroadcastDeparture), CTaskSimpleRunNamedAnim (2 constructores, destructor, Clone, GetTaskType,
// ProcessPed, OffsetPedPosition) y CTaskComplexWalkRoundCar (constructor, destructor, SetNewVehicle, CreateRouteTask,
// ComputeRouteRoundSmallCar, ComputeRouteRoundBigCar, ComputeRoute, GoingForDoor, Clone, GetTaskType, CreateNextSubTask,
// CreateFirstSubTask, ControlSubTask).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 3 peatones (matriz casi siempre, inteligencia con tareas secundarias/primaria y escaner de vehiculos, clump, tipo, armas),
// 2 vehiculos (matriz, banderas, conductor, remolques), una puerta (objeto con matriz y banderas fisicas), 4 tareas falsas
// (vtable trazada: dtor, GetTaskType, MakeAbortable y CreateFirstSubTask), asociaciones/jerarquias de animacion falsas,
// informacion de arma falsa y un atractor con sus tres SArray en buffers propios.
// Se simulan y trazan: RNG (rand del EXE y CGeneral::GetRandomNumber), CAnimManager (GetAnimationBlock, GetAnimation,
// BlendAnimation hier/id), RpAnimBlendClumpGetAssociation (2 variantes), CEntity::UpdateRpHAnim, CPed::GetBonePosition,
// CEventGroup (Add, RemoveInvalidEvents, Reorganise), GetEventGlobalGroup, SetTaskSecondary (EXE) / ChangeTaskInSlot (DLL),
// CTaskManager::FindTaskByType, CPed::GetWeaponSkill, CWeaponInfo::GetWeaponInfo, CPedIntelligence::GetTaskUseGun,
// CTaskSimpleUseGun (ControlGun, ControlGunMove), CTaskComplexGoToPointAndStandStill::GoToPoint, CCarEnterExit
// (IsVehicleStealable, IsPlayerToQuitCarEnter), GetPedAttractorManager, CPedAttractorManager::HasEmptySlot,
// CInterestingEvents::Add, CPedAttractor::SetTaskForPed, CTaskSimpleStandStill::ProcessPed y las funciones de
// CPedGeometryAnalyser que usa CTaskComplexWalkRoundCar (esquinas, planos, lados, puntos de superficie y ruta alrededor).
// Las subtareas que crean las funciones salen del pool real con el constructor que toque (EXE o DLL) y se serializan enteras.
// Pools de tareas, rutas de puntos y rutas de nodos: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada
// ejecucion; lo nuevo se libera sin destructor. Se guardan y restauran el reloj, el pad 0, la distancia de impacto
// 0x8D22B0, la tabla de desplazamientos de CPedShelterAttractor y los 2dfx scripted. Constructores sobre buffer con 0xCD
// comparados enteros (tipo en vez de vptr).
#include <fstream>
#include <functional>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.h"
#include "Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAiming.h"
#include "Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.h"
#include "Tasks/TaskTypes/TaskSimpleRunNamedAnim.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundCar.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Attractors/PedShelterAttractor.h"
#include "Events/EventGroup.h"
#include "Events/EventAreaCodes.h"
#include "Scripts/Scripted2dEffects.h"
#include "PedAttractorManager.h"
#include "InterestingEvents.h"
#include "WeaponInfo.h"
#include "CarEnterExit.h"
#include "PedGeometryAnalyser.h"
#include "PedIntelligence.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "Pools/NodeRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks61cLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=3, NVEH=2, FAKES=4, NASSOC=3, NHIER=2, NWI=4;
// tareas falsas: campos de control por encima de 0x60
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CNT=0x70, FK_CF=0x74;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_RWOBJ=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_TYPE=0x36;                  // CEntity
constexpr size_t PH_FLAGS=0x40;                                                                                  // CPhysical
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_ANIMGROUP=0x4D4, P_VEH=0x58C, P_PEDTYPE=0x598, P_WEAPONS=0x5A0,
                 P_ACTIVESLOT=0x718;                                                                             // CPed
constexpr size_t V_FLAGS=0x428, V_DRIVER=0x460, V_TOWING=0x4C4, V_TOWED=0x4C8;                                   // CVehicle
constexpr size_t I_TASKMGR=4, I_EVENTGROUP=0x68, I_SCANNER_CLOSEST=0x120;                                        // CPedIntelligence
constexpr size_t TM_PRIMARY=0, TM_SECONDARY=0x14;                                                                // CTaskManager
constexpr size_t A_ID=0x2C, A_FLAGS=0x2E, A_CBTYPE=0x30, A_CBFUNC=0x34, A_CBDATA=0x38, A_BLENDDELTA=0x1C;        // CAnimBlendAssociation
constexpr uintptr_t TIMER=0xB7CB84, HITDIST=0x8D22B0, DISPLACEMENTS=0xC0986C;
constexpr uint32 IN_VEH=0x100u, BIG=0x400u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,sizeof(CObject)> door{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> doorMat{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,NASSOC> assocs{};
alignas(16) inline std::array<std::array<uint8,0x20>,NHIER> hiers{};
alignas(16) inline std::array<std::array<uint8,0x80>,NWI> winfos{};
alignas(16) inline std::array<uint8,0x40> clump{}, animBlock{}, fakeMgr{}, fakeGroup{}, vecBuf{}, vecBuf2{}, aaAttractPeds{}, aaArrived{};
alignas(16) inline std::array<uint8,0xC0> aaPairs{};
alignas(16) inline std::array<uint8,0x80> aaDisp{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x100> attractor{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pools (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80, PROUTE_SLOT=sizeof(CPointRoute), NROUTE_SLOT=sizeof(CNodeRoute);
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PRouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* NRouteHdr() { return reinterpret_cast<uint8*>(GetNodeRoutePool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline int SlotIndex(uint8* h,size_t slot,uintptr_t v) {
    const auto s=uintptr_t(Storage(h)); if(v<s||v>=s+size_t(Cap(h))*slot||(v-s)%slot)return -1; return int((v-s)/slot);
}
inline std::vector<bool> Occ(uint8* h) { std::vector<bool> r(size_t(Cap(h))); for(size_t k=0;k<r.size();++k)r[k]=!(States(h)[k]&0x80); return r; }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart) {
    Last(h)=last;
    if(Occ(h)==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}
inline void FreeFresh(uint8* h,const std::vector<bool>& before) { for(size_t k=0;k<before.size();++k)if(!before[k]&&!(States(h)[k]&0x80))States(h)[k]|=0x80; }

inline std::array<void*,16> fakeVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::array<void*,20> eventVtbl{};
inline std::array<void*,8> attVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;       // tareas nuevas del pool de tareas
inline std::unordered_map<uintptr_t,int> freshRoutes;    // rutas de puntos nuevas
inline std::unordered_map<uintptr_t,std::string> callbackNames;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==uintptr_t(attVtbl.data()))return "att_vt";
    if(auto it=callbackNames.find(v);it!=callbackNames.end())return it->second;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    if(auto it=freshRoutes.find(v);it!=freshRoutes.end())return "R"+std::to_string(it->second);
    for(auto& [b,id]:freshRoutes)if(v>b&&v<b+PROUTE_SLOT)return "R"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NASSOC;++n)if(auto s=in(assocs[n].data(),assocs[n].size(),"assoc"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NHIER;++n)if(auto s=in(hiers[n].data(),hiers[n].size(),"hier"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NWI;++n)if(auto s=in(winfos[n].data(),winfos[n].size(),"winfo"))return *s+"#"+std::to_string(n);
    if(auto s=in(door.data(),door.size(),"door"))return *s; if(auto s=in(doorMat.data(),doorMat.size(),"dmat"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(attractor.data(),attractor.size(),"attractor"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(animBlock.data(),animBlock.size(),"ablock"))return *s;
    if(auto s=in(fakeMgr.data(),fakeMgr.size(),"fmgr"))return *s; if(auto s=in(fakeGroup.data(),fakeGroup.size(),"fgroup"))return *s;
    if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s; if(auto s=in(vecBuf2.data(),vecBuf2.size(),"vec2"))return *s;
    if(auto s=in(aaAttractPeds.data(),aaAttractPeds.size(),"attpeds"))return *s; if(auto s=in(aaArrived.data(),aaArrived.size(),"arrived"))return *s;
    if(auto s=in(aaPairs.data(),aaPairs.size(),"pairs"))return *s; if(auto s=in(aaDisp.data(),aaDisp.size(),"disp"))return *s;
    { const auto fxb=uintptr_t(&CScripted2dEffects::ms_effects[0]); if(v>=fxb&&v<fxb+sizeof(C2dEffect)*NUM_SCRIPTED_2D_EFFECTS)return "fx+"+std::to_string(v-fxb); }
    { const auto ib=uintptr_t(&g_InterestingEvents); if(v>=ib&&v<ib+0x400)return "interesting+"+std::to_string(v-ib); }
    if(const int k=SlotIndex(PRouteHdr(),PROUTE_SLOT,v);k>=0)return "proute#"+std::to_string(k);
    if(const int k=SlotIndex(NRouteHdr(),NROUTE_SLOT,v);k>=0)return "nroute#"+std::to_string(k);
    if(SlotIndex(TaskHdr(),TASK_SLOT,v)>=0)return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // locales de la pila (eventos, vectores temporales): no comparables
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); r.push_back(RefList(door.data())); return r; }
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}
// evento de codigos de area (de la pila): tipo por la vtable y solo los campos (sin relleno)
inline json SerAreaCodes(CEvent* e) {
    if(!e)return nullptr;
    return json::array({e->GetEventType(),At<int32>(e,4),At<uint8>(e,8),Name(At<uint32>(e,0xC))});
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()}); b.push_back({"pmat#"+std::to_string(n),pedMats[n].data(),pedMats[n].size()});
                              b.push_back({"intel#"+std::to_string(n),intels[n].data(),intels[n].size()}); }
    for(int n=0;n<NVEH;++n){ b.push_back({"veh#"+std::to_string(n),vehs[n].data(),vehs[n].size()}); b.push_back({"vmat#"+std::to_string(n),vehMats[n].data(),vehMats[n].size()}); }
    b.push_back({"door",door.data(),door.size()}); b.push_back({"dmat",doorMat.data(),doorMat.size()});
    for(int n=0;n<FAKES;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    for(int n=0;n<NASSOC;++n)b.push_back({"assoc#"+std::to_string(n),assocs[n].data(),assocs[n].size()});
    b.push_back({"vec",vecBuf.data(),vecBuf.size()}); b.push_back({"vec2",vecBuf2.data(),vecBuf2.size()});
    b.push_back({"attractor",attractor.data(),attractor.size()});
    b.push_back({"attpeds",aaAttractPeds.data(),aaAttractPeds.size()}); b.push_back({"arrived",aaArrived.data(),aaArrived.size()});
    b.push_back({"pairs",aaPairs.data(),aaPairs.size()}); b.push_back({"disp",aaDisp.data(),aaDisp.size()});
    return b;
}
inline std::vector<std::vector<uint8>> Snapshot() { std::vector<std::vector<uint8>> s; for(auto& b:Buffers())s.push_back(ReversingProbe::Bytes(b.p,b.n)); return s; }
inline json Changes(const std::vector<std::vector<uint8>>& before) {
    json r=json::array(); const auto bufs=Buffers();
    for(size_t k=0;k<bufs.size();++k)for(size_t w=0;w+4<=bufs[k].n;w+=4)
        if(std::memcmp(bufs[k].p+w,before[k].data()+w,4)){
            // la DLL guarda la direccion de su callback y el EXE la del EXE: si ambas se llaman igual no es un cambio
            const uintptr_t nw=At<uint32>(bufs[k].p,w), ol=*reinterpret_cast<const uint32*>(before[k].data()+w);
            const auto a=callbackNames.find(nw), b=callbackNames.find(ol);
            if(a!=callbackNames.end() && b!=callbackNames.end() && a->second==b->second)continue;
            r.push_back({bufs[k].name,w,Name(nw)});
        }
    return r;
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 hitCalls{}, closestCalls{};
inline uint64 mEffectSlot{};
inline uint32 wiPick{}, skillPick{};
inline uintptr_t findTaskRet{};
inline std::array<float,16> planeData{};
inline std::array<int32,4> hitSides{};
inline CVector bonePos{};
inline uint8* fakeGlobalGroup{};
inline bool Bit(uint32 mask,uint32& calls) { return (mask>>(calls++&31))&1; }

// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
inline CTask* __fastcall FCreateFirst(CTask* t,void*,CPed* p) { trace.push_back({"create_first_fake",Name(t),Name(p)}); return At<CTask*>(t,FK_CF); }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// atractor falso: ComputeAttractPos (hueco 3) y ComputeAttractHeading (hueco 4) trazados
inline void __fastcall AttPos(void* a,void*,int32 id,CVector* out) {
    out->x=float(id)*1.5f-2.f; out->y=float(id)*0.25f+1.f; out->z=float(id&3);
    trace.push_back({"att_compute_pos",Name(a),id,Vec(*out)});
}
inline void __fastcall AttHeading(void* a,void*,int32 q,float* out) { *out=float(q)*0.5f+0.125f; trace.push_back({"att_compute_heading",Name(a),q,F(*out)}); }

// mascaras por tipo de dependencia (un bit por llamada)
enum { MK_STEAL, MK_QUIT, MK_ASSOC, MK_USEGUN, MK_BLOCK, MK_HIER, MK_STAND, MK_COUNT };
inline std::array<uint32,MK_COUNT> masks{}, counts{};
inline bool B(int k) { return (masks[k]>>(counts[k]++&31))&1; }
inline uint32 assocPick{}, hierPick{};
inline std::string Str(const char* s,int n) { std::string r; for(int k=0;k<n&&s&&s[k];++k)r+=s[k]; return r; }
inline CVector Pt(uint32 a,uint32 b) { return CVector{float(int32(a%61)-30)*0.5f,float(int32(b%53)-26)*0.5f,float(int32((a^b)%11))*0.25f}; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlock* __cdecl FakeGetBlock(const char* n) { const bool b=B(MK_BLOCK); trace.push_back({"get_anim_block",Str(n,24),b}); return b ? reinterpret_cast<CAnimBlock*>(animBlock.data()) : nullptr; }
inline CAnimBlendHierarchy* __cdecl FakeGetAnimation(const char* n,const CAnimBlock* blk) {
    const bool b=B(MK_HIER); trace.push_back({"get_animation",Str(n,24),Name(blk),b});
    return b ? reinterpret_cast<CAnimBlendHierarchy*>(hiers[hierPick%NHIER].data()) : nullptr;
}
inline void __fastcall FakeUpdateRpHAnim(CEntity* e,void*) { trace.push_back({"update_rphanim",Name(e)}); }
inline CAnimBlendAssociation* __cdecl FakeGetAssocHier(RpClump* c,uint32 b,CAnimBlendHierarchy* h) {
    const bool r=B(MK_ASSOC); auto* a=r ? assocs[assocPick%NASSOC].data() : nullptr;
    trace.push_back({"get_association_hier",Name(c),b&0xFF,Name(h),Name(a)}); return reinterpret_cast<CAnimBlendAssociation*>(a);
}
inline CAnimBlendAssociation* __cdecl FakeGetAssocId(RpClump* c,uint32 id) {
    const bool r=B(MK_ASSOC); auto* a=r ? assocs[assocPick%NASSOC].data() : nullptr;
    trace.push_back({"get_association_id",Name(c),id,Name(a)}); return reinterpret_cast<CAnimBlendAssociation*>(a);
}
inline CAnimBlendAssociation* __cdecl FakeBlendHier(RpClump* c,CAnimBlendHierarchy* h,int32 flags,float d) {
    trace.push_back({"blend_animation_hier",Name(c),Name(h),flags,F(d)}); return reinterpret_cast<CAnimBlendAssociation*>(assocs[assocPick%NASSOC].data());
}
inline CAnimBlendAssociation* __cdecl FakeBlendId(RpClump* c,int32 grp,int32 id,float d) {
    trace.push_back({"blend_animation_id",Name(c),grp,id,F(d)}); return reinterpret_cast<CAnimBlendAssociation*>(assocs[assocPick%NASSOC].data());
}
inline void __fastcall FakeBone(CPed* p,void*,CVector* out,int32 tag,uint32 b) { *out=bonePos; trace.push_back({"get_bone_position",Name(p),Name(out),tag,b&0xFF}); }
inline void __fastcall FakeRemoveInvalid(void* g,void*,uint32 b) { trace.push_back({"remove_invalid_events",Name(g),b&0xFF}); }
inline void __fastcall FakeReorganise(void* g,void*) { trace.push_back({"reorganise_events",Name(g)}); }
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),SerAreaCodes(e),valid&0xFF}); return e; }
inline void* __cdecl FakeGlobalGroup() { trace.push_back({"get_event_global_group"}); return fakeGlobalGroup; }
inline void __fastcall FakeSetSecondaryExe(void* mgr,void*,CTask* t,int32 idx) { trace.push_back({"set_task_secondary",Name(mgr),Name(t),idx}); }
inline void __fastcall FakeChangeInSlotDll(void* mgr,void*,CTask** slot,CTask* t) {
    trace.push_back({"set_task_secondary",Name(mgr),Name(t),int32((uintptr_t(slot)-(uintptr_t(mgr)+TM_SECONDARY))/4)});
}
inline CTask* __fastcall FakeFindTask(void* mgr,void*,int32 idx,int32 type) { trace.push_back({"find_task_by_type",Name(mgr),idx,type,Name(findTaskRet)}); return reinterpret_cast<CTask*>(findTaskRet); }
inline int32 __fastcall FakeWeaponSkill(CPed* p,void*) { trace.push_back({"get_weapon_skill",Name(p),skillPick}); return int32(skillPick); }
inline CWeaponInfo* __cdecl FakeWeaponInfo(int32 type,int32 skill) { trace.push_back({"get_weapon_info",type,skill,wiPick}); return reinterpret_cast<CWeaponInfo*>(winfos[wiPick%NWI].data()); }
inline CTaskSimpleUseGun* __fastcall FakeGetTaskUseGun(void* intel,void*) {
    const bool b=B(MK_USEGUN); trace.push_back({"get_task_use_gun",Name(intel),b}); return b ? reinterpret_cast<CTaskSimpleUseGun*>(Fake(3)) : nullptr;
}
inline bool __fastcall FakeControlGun(void* t,void*,CPed* p,CEntity* tgt,int32 cmd) { trace.push_back({"control_gun",Name(t),Name(p),Name(tgt),int32(int8(cmd))}); return true; }
inline bool __fastcall FakeControlGunMove(void* t,void*,CVector2D* mv) { trace.push_back({"control_gun_move",Name(t),F(mv->x),F(mv->y)}); return true; }
inline bool __cdecl FakeStealable(const CVehicle* v,const CPed* p) { const bool b=B(MK_STEAL); trace.push_back({"is_vehicle_stealable",Name(v),Name(p),b}); return b; }
inline bool __cdecl FakeQuit(const CPed* p,const CVehicle* v,int32 start,CTask* t) { const bool b=B(MK_QUIT); trace.push_back({"is_player_to_quit_car_enter",Name(p),Name(v),start,Name(t),b}); return b; }
inline void* __cdecl FakeAttMgr() { trace.push_back({"get_attractor_manager"}); return fakeMgr.data(); }
inline bool __fastcall FakeHasEmpty(void* m,void*,const void* fx,const CEntity* e) {
    const auto idx=(uintptr_t(fx)-uintptr_t(&CScripted2dEffects::ms_effects[0]))/sizeof(C2dEffect); const bool b=(mEffectSlot>>(idx&63))&1;
    trace.push_back({"has_empty_slot",Name(m),uint32(idx),Name(e),b}); return b;
}
inline void __fastcall FakeInteresting(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_add",Name(ie),type,Name(e)}); }
inline void __fastcall FakeSetTaskForPed(void* a,void*,CPed* p,CTask* t) { trace.push_back({"set_task_for_ped",Name(a),Name(p),Name(t)}); }
inline bool __fastcall FakeStandStill(void* t,void*,CPed* p) { const bool b=B(MK_STAND); trace.push_back({"stand_still_process_ped",Name(p),b}); return b; }
// CPedGeometryAnalyser: salidas deterministas segun la semilla del caso y la cuenta de llamadas
inline void __cdecl FakeCorners(float z,CEntity* e,CVector* out) { trace.push_back({"bb_corners",F(z),Name(e),Name(out)}); }
inline void __cdecl FakePlanes(float z,CEntity* e,CVector* planes,float* dots) {
    for(int k=0;k<4;++k){ planes[k]=CVector{planeData[3*k],planeData[3*k+1],planeData[3*k+2]}; dots[k]=planeData[12+k]; }
    trace.push_back({"bb_planes",F(z),Name(e),Name(planes),Name(dots)});
}
inline int __cdecl FakeHitPed(const CPed* p,CEntity* e) { const int r=hitSides[hitCalls++&3]; trace.push_back({"hit_side_ped",Name(p),Name(e),r}); return r; }
inline int __cdecl FakeHitPoint(const CVector* pt,CEntity* e) { const int r=hitSides[hitCalls++&3]; trace.push_back({"hit_side_point",Vec(*pt),Name(e),r}); return r; }
inline bool __cdecl FakeClosestPed(const CPed* p,CEntity* e,CVector* out) {
    *out=Pt(seed*7+closestCalls*13,seed+closestCalls*5); closestCalls++;
    trace.push_back({"closest_surface_ped",Name(p),Name(e),Name(out),Vec(*out),F(At<float>(reinterpret_cast<void*>(HITDIST),0))}); return true;
}
inline bool __cdecl FakeClosestPoint(const CVector* pt,CEntity* e,CVector* out) {
    *out=Pt(seed*7+closestCalls*13,seed+closestCalls*5); closestCalls++;
    trace.push_back({"closest_surface_point",Vec(*pt),Name(e),Name(out),Vec(*out),F(At<float>(reinterpret_cast<void*>(HITDIST),0))}); return true;
}
inline int __cdecl FakeRouteRound(const CPed* p,CEntity* e,const CVector* pt,CPointRoute* r,int32 dir) {
    const uint32 n=(seed>>3)%5; At<uint32>(r,0)=n;
    for(uint32 k=0;k<n;++k)At<CVector>(r,4+12*k)=Pt(seed*3+k*17,seed*5+k);
    trace.push_back({"route_round_bb",Name(p),Name(e),Vec(*pt),Name(r),dir,n}); return int(seed&0xF);
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Direccion de la funcion de la DLL instalada por un hook virtual (para simularla tambien en llamadas directas)
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

// ChangeTaskInSlot es protegido: se toma su direccion desde una clase derivada
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeInSlot() { return MemberAddress(&TaskManagerAccess::ChangeTaskInSlot); } };

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=60;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","Clone",0,0x86FE58,1,256},
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","Constructor",0x668CD0,0,-1,256},
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","ControlSubTask",0,0x86FE58,10,512},
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","CreateFirstSubTask",0,0x86FE58,9,256},
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","CreateNextSubTask",0,0x86FE58,8,512},
    {"CTaskComplexGoToPointAndStandStillAndAchieveHeading","Destructor",0x668D40,0,-1,256},
    {"CTaskComplexGoToPointAnyMeans","Constructor-1",0x66B720,0,-1,256},
    {"CTaskComplexGoToPointAnyMeans","Constructor-2",0x66B790,0,-1,256},
    {"CTaskComplexGoToPointAnyMeans","ControlSubTask",0,0x86FF68,10,1024},
    {"CTaskComplexGoToPointAnyMeans","CreateFirstSubTask",0,0x86FF68,9,512},
    {"CTaskComplexGoToPointAnyMeans","CreateNextSubTask",0,0x86FF68,8,1024},
    {"CTaskComplexGoToPointAnyMeans","CreateSubTask",0x6705D0,0,-1,1024},
    {"CTaskComplexGoToPointAnyMeans","Destructor",0x66B830,0,-1,256},
    {"CTaskComplexGotoDoorAndOpen","Clone",0,0x86FFC0,1,256},
    {"CTaskComplexGotoDoorAndOpen","Constructor-0",0x66BB20,0,-1,256},
    {"CTaskComplexGotoDoorAndOpen","Constructor-1",0x66BBA0,0,-1,256},
    {"CTaskComplexGotoDoorAndOpen","ControlSubTask",0,0x86FFC0,10,1024},
    {"CTaskComplexGotoDoorAndOpen","CreateFirstSubTask",0,0x86FFC0,9,1024},
    {"CTaskComplexGotoDoorAndOpen","CreateNextSubTask",0,0x86FFC0,8,512},
    {"CTaskComplexGotoDoorAndOpen","Destructor",0x66BC00,0,-1,256},
    {"CTaskComplexGoToPointAiming","Clone",0,0x86FE00,1,256},
    {"CTaskComplexGoToPointAiming","Constructor",0x668790,0,-1,512},
    {"CTaskComplexGoToPointAiming","ControlSubTask",0,0x86FE00,10,1024},
    {"CTaskComplexGoToPointAiming","CreateFirstSubTask",0,0x86FE00,9,1024},
    {"CTaskComplexGoToPointAiming","CreateNextSubTask",0,0x86FE00,8,256},
    {"CTaskComplexGoToPointAiming","CreateSubTask",0x6688D0,0,-1,256},
    {"CTaskComplexGoToPointAiming","Destructor",0x668870,0,-1,256},
    {"CTaskComplexGoToPointAiming","GetTaskType",0,0x86FE00,4,256},
    {"CTaskComplexUseClosestFreeScriptedAttractor","Clone",0,0x86E428,1,256},
    {"CTaskComplexUseClosestFreeScriptedAttractor","ComputeClosestFreeScriptedEffect",0x634740,0,-1,1024},
    {"CTaskComplexUseClosestFreeScriptedAttractor","Constructor",0x6346F0,0,-1,256},
    {"CTaskComplexUseClosestFreeScriptedAttractor","ControlSubTask",0,0x86E428,10,256},
    {"CTaskComplexUseClosestFreeScriptedAttractor","CreateFirstSubTask",0,0x86E428,9,512},
    {"CTaskComplexUseClosestFreeScriptedAttractor","CreateNextSubTask",0,0x86E428,8,256},
    {"CTaskComplexUseClosestFreeScriptedAttractor","Destructor",0x634720,0,-1,256},
    {"CTaskComplexUseClosestFreeScriptedAttractor","GetTaskType",0,0x86E428,4,256},
    {"CPedShelterAttractor","BroadcastDeparture",0,0x86C5B4,5,1024},
    {"CPedShelterAttractor","ComputeAttractHeading",0,0x86C5B4,4,256},
    {"CPedShelterAttractor","ComputeAttractPos",0,0x86C5B4,3,512},
    {"CPedShelterAttractor","GetDisplacement",0x5EF420,0,-1,512},
    {"CTaskSimpleRunNamedAnim","Clone",0,0x86D54C,1,256},
    {"CTaskSimpleRunNamedAnim","Constructor-Anim",0x61A990,0,-1,512},
    {"CTaskSimpleRunNamedAnim","Constructor-Default",0x6674B0,0,-1,256},
    {"CTaskSimpleRunNamedAnim","Destructor",0x61BF10,0,-1,256},
    {"CTaskSimpleRunNamedAnim","GetTaskType",0,0x86D54C,4,256},
    {"CTaskSimpleRunNamedAnim","OffsetPedPosition",0x61AB00,0,-1,512},
    {"CTaskSimpleRunNamedAnim","ProcessPed",0,0x86D54C,7,1024},
    {"CTaskComplexWalkRoundCar","Clone",0,0x86F308,1,256},
    {"CTaskComplexWalkRoundCar","ComputeRoute",0x657B80,0,-1,512},
    {"CTaskComplexWalkRoundCar","ComputeRouteRoundBigCar",0x656BB0,0,-1,1024},
    {"CTaskComplexWalkRoundCar","ComputeRouteRoundSmallCar",0x6544F0,0,-1,1024},
    {"CTaskComplexWalkRoundCar","Constructor",0x6541B0,0,-1,256},
    {"CTaskComplexWalkRoundCar","ControlSubTask",0,0x86F308,10,1024},
    {"CTaskComplexWalkRoundCar","CreateFirstSubTask",0,0x86F308,9,1024},
    {"CTaskComplexWalkRoundCar","CreateNextSubTask",0,0x86F308,8,256},
    {"CTaskComplexWalkRoundCar","CreateRouteTask",0x6542E0,0,-1,256},
    {"CTaskComplexWalkRoundCar","Destructor",0x656B00,0,-1,256},
    {"CTaskComplexWalkRoundCar","GetTaskType",0,0x86F308,4,256},
    {"CTaskComplexWalkRoundCar","GoingForDoor",0x654720,0,-1,256},
    {"CTaskComplexWalkRoundCar","SetNewVehicle",0x654290,0,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data()); clean(door.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks61c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks61c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks61c");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks61c-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    const uintptr_t dllStandStill=Body(OurVirtual("CTaskSimpleStandStill","ProcessPed"));
    if(!dllStandStill){Write({{"event","error"},{"reason","CTaskSimpleStandStill::ProcessPed hook not found"}});return;}
    {
        using std::bit_cast;
        const std::pair<uintptr_t,const char*> cbs[]{
            {0x61AAA0,"cb_move_ped"},{bit_cast<uintptr_t>(&CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB),"cb_move_ped"},
            {Body(bit_cast<uintptr_t>(&CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB)),"cb_move_ped"},
            {0x61A8A0,"cb_finish"},{bit_cast<uintptr_t>(&CTaskSimpleAnim::FinishRunAnimCB),"cb_finish"},
            {Body(bit_cast<uintptr_t>(&CTaskSimpleAnim::FinishRunAnimCB)),"cb_finish"},
            {0x4D1390,"cb_default"},{bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB),"cb_default"},
            {Body(bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB)),"cb_default"}};
        for(auto& [a,n]:cbs)callbackNames[a]=n;
    }
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{}); FillTraps(eventVtbl,std::make_integer_sequence<int,20>{});
    FillTraps(attVtbl,std::make_integer_sequence<int,8>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    fakeVtbl[9]=reinterpret_cast<void*>(&FCreateFirst);
    attVtbl[3]=reinterpret_cast<void*>(&AttPos); attVtbl[4]=reinterpret_cast<void*>(&AttHeading);
    fakeGlobalGroup=fakeGroup.data();
    const auto occTaskStart=Occ(TaskHdr()), occPRStart=Occ(PRouteHdr()), occNRStart=Occ(NRouteHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr()), lp0=Last(PRouteHdr()), ln0=Last(NRouteHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    const std::vector<uint8> stPR(States(PRouteHdr()),States(PRouteHdr())+Cap(PRouteHdr()));
    const std::vector<uint8> stNR(States(NRouteHdr()),States(NRouteHdr())+Cap(NRouteHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); ResetPool(PRouteHdr(),PROUTE_SLOT,lp0,stPR,occPRStart); ResetPool(NRouteHdr(),NROUTE_SLOT,ln0,stNR,occNRStart); };
    // estado global tocado: reloj, pad 0 (0x100..0x120), distancia de impacto, SArray de desplazamientos y 2dfx scripted;
    // se restauran en cada ejecucion y al final
    uint8* const pad=reinterpret_cast<uint8*>(CPad::GetPad(0));
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{uintptr_t(pad)+0x100,0x20},{HITDIST,4},{DISPLACEMENTS,16},
        {uintptr_t(&CScripted2dEffects::ms_activated[0]),NUM_SCRIPTED_2D_EFFECTS},
        {uintptr_t(&CScripted2dEffects::ms_userLists[0]),sizeof(tUserList)*NUM_SCRIPTED_2D_EFFECTS},
        {uintptr_t(&CScripted2dEffects::ms_effects[0]),sizeof(C2dEffect)*NUM_SCRIPTED_2D_EFFECTS}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch b1(Body(bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock))),FakeGetBlock), b1e(0x4D3940,FakeGetBlock);
        EntryPatch b2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendHierarchy*(*)(const char*,const CAnimBlock*)>(&CAnimManager::GetAnimation))),FakeGetAnimation), b2e(0x4D42F0,FakeGetAnimation);
        EntryPatch u1(Body(MemberAddress(&CEntity::UpdateRpHAnim)),FakeUpdateRpHAnim), u1e(0x532B20,FakeUpdateRpHAnim);
        EntryPatch a1(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,bool,CAnimBlendHierarchy*)>(&RpAnimBlendClumpGetAssociation))),FakeGetAssocHier), a1e(0x4D68E0,FakeGetAssocHier);
        EntryPatch a2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FakeGetAssocId), a2e(0x4D68B0,FakeGetAssocId);
        EntryPatch a3(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,CAnimBlendHierarchy*,int32,float)>(&CAnimManager::BlendAnimation))),FakeBlendHier), a3e(0x4D4410,FakeBlendHier);
        EntryPatch a4(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FakeBlendId), a4e(0x4D4610,FakeBlendId);
        EntryPatch p1(Body(MemberAddress(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition))),FakeBone), p1e(0x5E4280,FakeBone);
        EntryPatch q1(Body(MemberAddress(&CEventGroup::RemoveInvalidEvents)),FakeRemoveInvalid), q1e(0x4AB760,FakeRemoveInvalid);
        EntryPatch q2(Body(MemberAddress(&CEventGroup::Reorganise)),FakeReorganise), q2e(0x4AB700,FakeReorganise);
        EntryPatch q3(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), q3e(0x4AB420,FakeEventAdd);
        EntryPatch q4(Body(bit_cast<uintptr_t>(&GetEventGlobalGroup)),FakeGlobalGroup), q4e(0x4ABA50,FakeGlobalGroup);
        // el SetTaskSecondary del DLL es en linea y llama a ChangeTaskInSlot
        EntryPatch m1(Body(TaskManagerAccess::ChangeInSlot()),FakeChangeInSlotDll), m1e(0x681B60,FakeSetSecondaryExe);
        EntryPatch m2(Body(MemberAddress(&CTaskManager::FindTaskByType)),FakeFindTask), m2e(0x6817D0,FakeFindTask);
        EntryPatch w1(Body(MemberAddress(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill))),FakeWeaponSkill), w1e(0x5E6580,FakeWeaponSkill);
        EntryPatch w2(Body(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo))),FakeWeaponInfo), w2e(0x743C60,FakeWeaponInfo);
        EntryPatch g1(Body(MemberAddress(&CPedIntelligence::GetTaskUseGun)),FakeGetTaskUseGun), g1e(0x600F70,FakeGetTaskUseGun);
        EntryPatch g2(Body(MemberAddress(&CTaskSimpleUseGun::ControlGun)),FakeControlGun), g2e(0x61E040,FakeControlGun);
        EntryPatch g3(Body(MemberAddress(&CTaskSimpleUseGun::ControlGunMove)),FakeControlGunMove), g3e(0x61E0C0,FakeControlGunMove);
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsVehicleStealable)),FakeStealable), c1e(0x6510D0,FakeStealable);
        EntryPatch c2(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsPlayerToQuitCarEnter)),FakeQuit), c2e(0x64F240,FakeQuit);
        EntryPatch t1(Body(bit_cast<uintptr_t>(&GetPedAttractorManager)),FakeAttMgr), t1e(0x5EE190,FakeAttMgr);
        EntryPatch t2(Body(MemberAddress(&CPedAttractorManager::HasEmptySlot)),FakeHasEmpty), t2e(0x5EBB00,FakeHasEmpty);
        EntryPatch t3(Body(MemberAddress(&CInterestingEvents::Add)),FakeInteresting), t3e(0x602590,FakeInteresting);
        EntryPatch t4(Body(MemberAddress(&CPedAttractor::SetTaskForPed)),FakeSetTaskForPed), t4e(0x5EECA0,FakeSetTaskForPed);
        EntryPatch s1(dllStandStill,FakeStandStill), s1e(0x62F370,FakeStandStill);
        EntryPatch d1(Body(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners)),FakeCorners), d1e(0x5F3650,FakeCorners);
        EntryPatch d2(Body(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes)),FakePlanes), d2e(0x5F3660,FakePlanes);
        EntryPatch d3(Body(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide))),FakeHitPed), d3e(0x5F3BC0,FakeHitPed);
        EntryPatch d4(Body(bit_cast<uintptr_t>(static_cast<int32(*)(const CVector&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide))),FakeHitPoint), d4e(0x5F3AC0,FakeHitPoint);
        EntryPatch d5(Body(bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,CEntity&,CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint))),FakeClosestPed), d5e(0x5F3B70,FakeClosestPed);
        EntryPatch d6(Body(bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CEntity&,CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint))),FakeClosestPoint), d6e(0x5F36F0,FakeClosestPoint);
        EntryPatch d7(Body(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32)>(&CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox))),FakeRouteRound), d7e(0x5F6110,FakeRouteRound);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x61C0u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=hitCalls=closestCalls=0; masks.fill(0); counts.fill(0); freshIds.clear(); freshRoutes.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    const auto occT0=Occ(TaskHdr()), occP0=Occ(PRouteHdr()), occN0=Occ(NRouteHdr());
                    for(auto& m:masks)m=next(); mEffectSlot=(uint64(next())<<32)|next();
                    assocPick=next(); hierPick=next(); wiPick=next()%NWI; skillPick=next()%4;
                    bonePos=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-3.f,3.f)};
                    for(auto& h:hitSides){ const uint32 m=next()%6; h= m==5 ? -1 : int32(m%4); }
                    for(auto& p:planeData)p=uni(-1.5f,1.5f);
                    // clases y comportamiento comun
                    const bool wrc=cls=="CTaskComplexWalkRoundCar", aim=cls=="CTaskComplexGoToPointAiming", rna=cls=="CTaskSimpleRunNamedAnim";
                    const bool pedMatAlways=wrc||aim||rna;
                    // peatones: matriz (casi siempre), inteligencia, clump, tipo, flags, armas, vehiculo, anim
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=clump.data();
                        if(pedMatAlways||next()%6){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        At<uint32>(p,E_FLAGS)=next();
                        { static const int16 models[]{103,105,0,7,102,104,106,-2,-1}; At<int16>(p,E_MODEL)=models[next()%std::size(models)]; }
                        { static const int32 types[]{0,1,2,4,7,8,9,10,6}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        At<uint32>(p,P_FLAGS)=(next()&~IN_VEH)|(next()%2 ? IN_VEH : 0u);
                        { const uint32 m=next()%4; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                        At<int32>(p,P_ANIMGROUP)=int32(next()%60);
                        At<int8>(p,P_ACTIVESLOT)=int8(next()%13); for(int w=0;w<13;++w)At<int32>(p,P_WEAPONS+0x1C*w)=next()%3 ? 0 : int32(next()%47);
                        auto* in=intels[n].data();
                        { const uint32 m=next()%3; At<CVehicle*>(in,I_SCANNER_CLOSEST)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                        for(int s=0;s<6;++s)At<CTask*>(in,I_TASKMGR+TM_SECONDARY+4*s)=next()%3==0 ? nullptr : Fake(int(next()%3));
                        At<CTask*>(in,I_TASKMGR+TM_PRIMARY+4*3)=next()%3==0 ? nullptr : Fake(int(next()%3));
                    }
                    // vehiculos: matriz, banderas, conductor y remolques
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        At<uint32>(v,V_FLAGS)=(next()&~BIG)|(next()%2 ? BIG : 0u);
                        { const uint32 m=next()%3; At<CPed*>(v,V_DRIVER)=m==0 ? nullptr : Ped(int(next()%NPEDS)); }
                        { const uint32 m=next()%4; At<CVehicle*>(v,V_TOWING)=m==0 ? Veh(1-n) : nullptr; }
                        { const uint32 m=next()%4; At<CVehicle*>(v,V_TOWED)=m==0 ? Veh(1-n) : nullptr; }
                    }
                    // puerta: objeto con matriz y banderas fisicas
                    { auto* d=door.data(); std::memset(d,0,door.size()); std::memset(doorMat.data(),0,doorMat.size());
                      d[E_TYPE]=4; At<void*>(d,E_MATRIX)=doorMat.data(); At<uint32>(d,PH_FLAGS)=next();
                      for(int r=0;r<3;++r)At<CVector>(doorMat.data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                      At<CVector>(doorMat.data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0;
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; At<uint32>(k,FK_CF)=0; }
                    // asociaciones, jerarquias e informacion de arma falsas
                    for(int n=0;n<NASSOC;++n){ auto* a=assocs[n].data(); for(size_t b=0;b<0x40;++b)a[b]=uint8(next());
                        At<float>(a,0x18)=uni(0.f,1.f); At<float>(a,A_BLENDDELTA)=uni(-5.f,5.f); At<uint16>(a,A_FLAGS)=uint16(next()); At<uint32>(a,A_CBTYPE)=next()%3;
                        At<uint32>(a,A_CBFUNC)=next()%3==0 ? 0x4D1390u : next(); At<uint32>(a,A_CBDATA)=next()%2 ? 0u : uint32(uintptr_t(Fake(0))); }
                    for(int n=0;n<NHIER;++n){ auto* h=hiers[n].data(); for(size_t b=0;b<0x20;++b)h[b]=uint8(next()); At<void*>(h,4)=next()%4 ? static_cast<void*>(clump.data()) : nullptr; }
                    for(int n=0;n<NWI;++n){ auto* w=winfos[n].data(); for(size_t b=0;b<0x80;++b)w[b]=uint8(next()); At<uint32>(w,0)=next()%6; At<uint32>(w,0x18)=next(); }
                    for(auto& b:clump)b=uint8(next()); clump[0]=2;
                    for(auto& b:animBlock)b=uint8(next()); for(auto& b:fakeMgr)b=uint8(next()); for(auto& b:fakeGroup)b=uint8(next());
                    for(auto& b:vecBuf)b=uint8(next()); for(auto& b:vecBuf2)b=uint8(next());
                    for(auto& b:aaAttractPeds)b=uint8(next()); for(auto& b:aaArrived)b=uint8(next()); for(auto& b:aaPairs)b=uint8(next()); for(auto& b:aaDisp)b=uint8(next());
                    for(auto& b:attractor)b=uint8(next());
                    // pad: bytes aleatorios (la bandera de transicion de interior es el bit 3 del byte 0x10E)
                    for(size_t b=0;b<0x20;++b)pad[0x100+b]=uint8(next());
                    std::memset(object.data(),0xCD,object.size());

                    auto* o=object.data(); auto* p0=Ped(0);
                    size_t objSize=0x30; bool isCtor=false;
                    uintptr_t target{};
                    json ret=nullptr; uintptr_t retPtr{}; bool isPtr=false;
                    auto Ptr=[&](const void* p)->json{ retPtr=uintptr_t(p); isPtr=true; return nullptr; };
                    std::function<void()> callFn;
                    auto subtask=[&](int32 type){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=type; };
                    auto fakeThis=[&](int32 type,CTask* createFirst){ At<void*>(o,0)=fakeVtbl.data(); At<int32>(o,FK_TYPE)=type; At<CTask*>(o,FK_CF)=createFirst; };
                    auto pedPos=[&](CPed* ped)->CVector{ auto* m=At<uint8*>(ped,E_MATRIX); return m ? At<CVector>(m,0x30) : At<CVector>(ped,E_POS); };
                    // ---- CTaskComplexGoToPointAndStandStillAndAchieveHeading ----
                    if(cls=="CTaskComplexGoToPointAndStandStillAndAchieveHeading") {
                        objSize=0x30; const uintptr_t VT=0x86FE58;
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; At<int32>(o,0xC)=int32(next()%9);
                            At<CVector>(o,0x10)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}; At<float>(o,0x1C)=uni(0.f,4.f); At<float>(o,0x20)=uni(-3.2f,3.2f);
                            At<float>(o,0x24)=uni(0.f,2.f); At<float>(o,0x28)=uni(0.f,1.f); At<uint8>(o,0x2C)=uint8(next()); At<CTask*>(o,8)=next()%8==0 ? nullptr : Fake(0); };
                        if(nm=="Constructor"){ isCtor=true; const int32 ms=int32(next()%9); At<CVector>(vecBuf.data(),0)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)};
                            const float a=uni(-3.2f,3.2f),r=uni(0.f,4.f),c=uni(0.f,2.f),m=uni(0.f,1.f);
                            callFn=[&,ms,a,r,c,m]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,float,float,float)>(target)(o,ms,reinterpret_cast<const CVector*>(vecBuf.data()),a,r,c,m)); }; }
                        else if(nm=="Destructor"){ fill(); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="Clone"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="CreateFirstSubTask"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="CreateNextSubTask"){ fill(); { static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_ACHIEVE_HEADING,TASK_FINISHED,TASK_NONE,TASK_SIMPLE_PAUSE};
                            subtask(ty[next()%std::size(ty)]); }
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="ControlSubTask"){ fill(); At<CTask*>(o,8)=Fake(0); { static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_ACHIEVE_HEADING,TASK_NONE};
                            At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)]; }
                            fakeThis(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL_AND_ACHIEVE_HEADING,next()%3 ? Fake(1) : nullptr);
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                    }
                    // ---- CTaskComplexGoToPointAnyMeans ----
                    else if(cls=="CTaskComplexGoToPointAnyMeans") {
                        objSize=0x34; const uintptr_t VT=0x86FF68;
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0;
                            At<CVector>(o,0xC)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-5.f,5.f)}; At<int32>(o,0x18)=int32(next()%9); At<float>(o,0x1C)=uni(0.f,3.f);
                            { const uint32 m=next()%3; At<CVehicle*>(o,0x20)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                            At<int32>(o,0x24)=next()%3==0 ? -1 : int32(next()%600);
                            At<uint32>(o,0x28)=now-next()%6000; At<uint32>(o,0x2C)=next()%3==0 ? 3000u : next()%8000; At<uint8>(o,0x30)=uint8(next()%2); At<uint8>(o,0x31)=uint8(next()%2);
                            At<CTask*>(o,8)=next()%8==0 ? nullptr : Fake(0); };
                        auto pickPed=[&]()->CPed*{ return Ped(int(next()%NPEDS)); };
                        if(nm=="Constructor-1"){ isCtor=true; At<CVector>(vecBuf.data(),0)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-5.f,5.f)};
                            const int32 ms=int32(next()%9), model=next()%3==0 ? -1 : int32(next()%600); const float r=uni(0.f,3.f);
                            callFn=[&,ms,model,r]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,int32)>(target)(o,ms,reinterpret_cast<const CVector*>(vecBuf.data()),r,model)); }; }
                        else if(nm=="Constructor-2"){ isCtor=true; At<CVector>(vecBuf.data(),0)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-5.f,5.f)};
                            const int32 ms=int32(next()%9), model=next()%3==0 ? -1 : int32(next()%600); const float r=uni(0.f,3.f); CVehicle* const v=next()%4==0 ? nullptr : Veh(int(next()%NVEH));
                            callFn=[&,ms,model,r,v]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,CVehicle*,float,int32)>(target)(o,ms,reinterpret_cast<const CVector*>(vecBuf.data()),v,r,model)); }; }
                        else if(nm=="Destructor"){ fill(); if(next()%2)Reg(At<CVehicle*>(o,0x20),o+0x20); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="CreateSubTask"){ fill(); static const int32 ty[]{TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_CAR_DRIVE_TO_POINT,TASK_SIMPLE_CREATE_CAR_AND_GET_IN,
                                TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_FINISHED,TASK_NONE,1000,TASK_SIMPLE_PAUSE};
                            const int32 t=ty[next()%std::size(ty)]; CPed* const ped=pickPed();
                            callFn=[&,t,ped]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,t,ped)); }; }
                        else if(nm=="CreateNextSubTask"){ fill(); { static const int32 ty[]{TASK_SIMPLE_CREATE_CAR_AND_GET_IN,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_CAR_DRIVE_TO_POINT,
                                TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_FINISHED,TASK_NONE,TASK_SIMPLE_PAUSE};
                            subtask(ty[next()%std::size(ty)]); } CPed* const ped=pickPed();
                            callFn=[&,ped]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped)); }; }
                        else if(nm=="CreateFirstSubTask"){ fill(); CPed* const ped=pickPed();
                            callFn=[&,ped]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped)); }; }
                        else if(nm=="ControlSubTask"){ fill(); At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=next()%5 ? TASK_COMPLEX_FOLLOW_NODE_ROUTE : TASK_NONE;
                            CPed* const ped=pickPed(); const CVector pp=pedPos(ped);
                            At<CVector>(o,0xC)=CVector{pp.x+uni(10.f,80.f)*(next()%2 ? 1.f : -1.f),pp.y+uni(-5.f,5.f),pp.z+uni(-3.f,3.f)};
                            callFn=[&,ped]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped)); }; }
                    }
                    // ---- CTaskComplexGotoDoorAndOpen ----
                    else if(cls=="CTaskComplexGotoDoorAndOpen") {
                        objSize=0x38; const uintptr_t VT=0x86FFC0;
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; At<CObject*>(o,0xC)=next()%4==0 ? nullptr : reinterpret_cast<CObject*>(door.data());
                            At<CVector>(o,0x10)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}; At<CVector>(o,0x1C)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)};
                            At<uint32>(o,0x28)=now-next()%3000; At<uint32>(o,0x2C)=next()%4==0 ? 1000u : next()%5000; At<uint8>(o,0x30)=uint8(next()%2); At<uint8>(o,0x31)=uint8(next()%2);
                            At<uint8>(o,0x34)=uint8(next()); At<CTask*>(o,8)=Fake(0); };
                        if(nm=="Constructor-0"){ isCtor=true; CObject* const d=next()%4==0 ? nullptr : reinterpret_cast<CObject*>(door.data());
                            callFn=[&,d]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CObject*)>(target)(o,d)); }; }
                        else if(nm=="Constructor-1"){ isCtor=true; At<CVector>(vecBuf.data(),0)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}; At<CVector>(vecBuf2.data(),0)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)};
                            callFn=[&]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,const CVector*,const CVector*)>(target)(o,reinterpret_cast<const CVector*>(vecBuf.data()),reinterpret_cast<const CVector*>(vecBuf2.data()))); }; }
                        else if(nm=="Destructor"){ fill(); if(next()%2)Reg(At<CObject*>(o,0xC),o+0xC); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="Clone"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="CreateNextSubTask"){ fill(); subtask(next()%4 ? TASK_SIMPLE_GO_TO_POINT : TASK_SIMPLE_PAUSE);
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="CreateFirstSubTask"){ fill(); if(next()%3)At<CObject*>(o,0xC)=reinterpret_cast<CObject*>(door.data());
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="ControlSubTask"){ fill(); At<CTask*>(o,8)=Fake(0);
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                    }
                    // ---- CTaskComplexGoToPointAiming ----
                    else if(aim) {
                        objSize=0x38; const uintptr_t VT=0x86FE00;
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; At<int32>(o,0xC)=int32(next()%9); At<CEntity*>(o,0x10)=next()%3==0 ? nullptr : reinterpret_cast<CEntity*>(Ped(int(next()%NPEDS)));
                            At<CVector>(o,0x14)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}; At<CVector>(o,0x20)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)};
                            At<float>(o,0x2C)=uni(0.f,5.f); At<float>(o,0x30)=uni(0.f,5.f); At<uint8>(o,0x34)=uint8(next()); At<CTask*>(o,8)=Fake(0); };
                        const int32 thisType=next()%2 ? TASK_COMPLEX_GO_TO_POINT_SHOOTING : TASK_COMPLEX_GO_TO_POINT_AIMING;
                        if(nm=="Constructor"){ isCtor=true; const int32 ms=int32(next()%9); CEntity* const ent=next()%3==0 ? nullptr : reinterpret_cast<CEntity*>(Ped(int(next()%NPEDS)));
                            const CVector mp{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}, ap{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}; const float r=uni(0.f,5.f),s=uni(0.f,5.f);
                            At<CVector>(vecBuf.data(),0)=mp;
                            // el destino anterior del buffer coincide (todo, o todo menos el vector, el radio o la distancia) para recorrer la comparacion del EXE
                            { const uint32 m=next()%5; if(m!=0){ if(m!=4)At<CVector>(o,0x20)=mp; if(m!=3)At<float>(o,0x2C)=r; if(m!=2)At<float>(o,0x30)=s; } }
                            callFn=[&,ms,ent,ap,r,s]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,CEntity*,CVector,float,float)>(target)(o,ms,reinterpret_cast<const CVector*>(vecBuf.data()),ent,ap,r,s)); }; }
                        else if(nm=="Destructor"){ fill(); if(next()%2)Reg(At<CEntity*>(o,0x10),o+0x10); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="Clone"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="GetTaskType"){ fill(); callFn=[&]{ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="CreateSubTask"){ fill(); fakeThis(thisType,nullptr); static const int32 ty[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_GUN_CTRL,TASK_FINISHED,TASK_NONE,1000};
                            const int32 t=ty[next()%std::size(ty)]; callFn=[&,t]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32)>(target)(o,t)); }; }
                        else if(nm=="CreateNextSubTask"){ fill(); { static const int32 ty[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_GUN_CTRL,TASK_FINISHED,TASK_NONE,TASK_SIMPLE_PAUSE}; subtask(ty[next()%std::size(ty)]); }
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="CreateFirstSubTask"){ fill(); fakeThis(thisType,nullptr);
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="ControlSubTask"){ fill(); At<CTask*>(o,8)=Fake(0); { static const int32 ty[]{TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_GUN_CTRL,TASK_NONE};
                                At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)]; }
                            if(next()%5)At<uint8>(o,0x34)&=0xFE; fakeThis(thisType,next()%3 ? Fake(1) : nullptr);
                            // algunos casos con el destino pegado al peatón para recorrer el recorte del radio
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                    }
                    // ---- CTaskComplexUseClosestFreeScriptedAttractor ----
                    else if(cls=="CTaskComplexUseClosestFreeScriptedAttractor") {
                        objSize=0x10; const uintptr_t VT=0x86E428;
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; { static const int32 ms[]{4,6,7,5,0,8,4}; At<int32>(o,0xC)=ms[next()%std::size(ms)]; } At<CTask*>(o,8)=next()%3==0 ? nullptr : Fake(0); };
                        auto effects=[&]{
                            for(size_t k=0;k<NUM_SCRIPTED_2D_EFFECTS;++k){
                                CScripted2dEffects::ms_activated.data()[k]=next()%3==0;
                                auto& ul=CScripted2dEffects::ms_userLists.data()[k]; ul.m_bUseList=next()%2;
                                for(size_t j=0;j<4;++j){ static const int32 mt[]{103,105,0,7,-1,-2,-2,102,104,106,500}; static const int32 pt[]{0,1,2,4,7,8,9,10,6,-1};
                                    ul.m_UserTypes[j]=mt[next()%std::size(mt)]; ul.m_UserTypesByPedType[j]=pt[next()%std::size(pt)]; }
                                auto* fx=reinterpret_cast<uint8*>(&CScripted2dEffects::ms_effects.data()[k]);
                                for(size_t b=0;b<sizeof(C2dEffect);++b)fx[b]=uint8(next());
                                At<CVector>(fx,0)=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(-10.f,10.f)}; } };
                        if(nm=="Constructor"){ isCtor=true; callFn=[&]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(o,next())); }; }
                        else if(nm=="Destructor"){ fill(); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="Clone"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="GetTaskType"){ fill(); callFn=[&]{ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="CreateNextSubTask"||nm=="ControlSubTask"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="CreateFirstSubTask"){ fill(); effects(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="ComputeClosestFreeScriptedEffect"){ effects(); CPed* const ped=Ped(int(next()%NPEDS));
                            callFn=[&,ped]{ ret=Ptr(reinterpret_cast<C2dEffect*(__stdcall*)(const CPed*)>(target)(ped)); }; }
                    }
                    // ---- CPedShelterAttractor ----
                    else if(cls=="CPedShelterAttractor") {
                        objSize=0x8C;
                        auto disp=[&](uint32 n){ auto* h=reinterpret_cast<uint8*>(DISPLACEMENTS); At<uint32>(h,0)=next(); At<uintptr_t>(h,4)=uintptr_t(aaDisp.data());
                            At<uintptr_t>(h,8)=uintptr_t(aaDisp.data())+12*n; At<uintptr_t>(h,12)=uintptr_t(aaDisp.data())+12*8;
                            for(uint32 k=0;k<8;++k)At<CVector>(aaDisp.data(),12*k)=CVector{uni(-2.f,2.f),uni(-2.f,2.f),next()%4 ? 0.f : uni(-1.f,1.f)}; };
                        if(nm=="GetDisplacement"){ { static const uint32 sz[]{0,0,0,1,3,5,8,2}; disp(sz[next()%std::size(sz)]); } const int32 id=int32(next()%8);
                            callFn=[&,id]{ ret=Ptr(reinterpret_cast<CVector*(__cdecl*)(int32)>(target)(id)); }; }
                        else if(nm=="ComputeAttractPos"){ disp(1+next()%8); At<uintptr_t>(attractor.data(),0)=0x86C5B4; At<void*>(attractor.data(),4)=next()%4 ? static_cast<void*>(fakeGroup.data()) : nullptr;
                            At<CVector>(attractor.data(),0x5C)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(-5.f,5.f)}; const int32 id=int32(next()%8);
                            At<CVector>(vecBuf.data(),0)=CVector{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)};
                            callFn=[&,id]{ reinterpret_cast<void(__thiscall*)(void*,int32,CVector*)>(target)(attractor.data(),id,reinterpret_cast<CVector*>(vecBuf.data())); }; }
                        else if(nm=="ComputeAttractHeading"){ At<uintptr_t>(attractor.data(),0)=0x86C5B4; const int32 q=int32(next()%8);
                            callFn=[&,q]{ reinterpret_cast<void(__thiscall*)(void*,int32,float*)>(target)(attractor.data(),q,reinterpret_cast<float*>(vecBuf.data())); }; }
                        else if(nm=="BroadcastDeparture"){
                            auto* a=attractor.data(); std::memset(a,0xCD,attractor.size()); At<void*>(a,0)=attVtbl.data();
                            void* const pool[]{Ped(0),Ped(1),Ped(2),Fake(0),Fake(1)};
                            auto pick=[&]()->void*{ return pool[next()%5]; };
                            auto arr=[&](size_t base,uint8* storage,size_t esz,uint32 n,auto&& fillElem){
                                At<uint32>(a,base)=next();
                                if(n==0&&next()%2){ At<void*>(a,base+4)=nullptr; At<void*>(a,base+8)=nullptr; At<void*>(a,base+12)=nullptr; }
                                else { At<uint8*>(a,base+4)=storage; At<uint8*>(a,base+8)=storage+esz*n; At<uint8*>(a,base+12)=storage+esz*8; for(uint32 k=0;k<n;++k)fillElem(storage+esz*k); } };
                            const uint32 nAtt=next()%5, nArr=next()%7, nPair=next()%7;
                            arr(0xC,aaAttractPeds.data(),4,nAtt,[&](uint8* e){ At<void*>(e,0)=pick(); });
                            arr(0x1C,aaArrived.data(),4,nArr,[&](uint8* e){ At<void*>(e,0)=pick(); });
                            arr(0x2C,aaPairs.data(),0x14,nPair,[&](uint8* e){ At<void*>(e,0)=pick(); At<void*>(e,4)=next()%2 ? pick() : nullptr; At<int32>(e,8)=int32(next()%7)-1; At<int32>(e,0xC)=int32(next()%5); At<uint8>(e,0x10)=uint8(next()%2); });
                            At<uint32>(a,0x3C)=next()%8; At<float>(a,0x44)=uni(0.f,9000.f);
                            // el que se va: de la cola (la mayoria de las veces), de otra lista o ninguno de los conocidos
                            CPed* ped=reinterpret_cast<CPed*>(pick());
                            if(nArr&&next()%3)ped=At<CPed*>(aaArrived.data(),4*(next()%nArr));
                            callFn=[&,ped]{ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(attractor.data(),ped); }; }
                    }
                    // ---- CTaskSimpleRunNamedAnim ----
                    else if(rna) {
                        objSize=0x64; const uintptr_t VT=0x86D54C;
                        static const char* names[]{"walk","IDLE_stance","a","Some_Anim_Name_23chr","","RunToPoint"};
                        static const char* groups[]{"ped","gta_int","a","Group_15_chars_","","SWAT"};
                        auto strInto=[&](uint8* dst,const char* s,size_t cap){ std::memset(dst,0xAB,cap); std::strcpy(reinterpret_cast<char*>(dst),s); };
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; strInto(o+0x10,names[next()%std::size(names)],24); strInto(o+0x28,groups[next()%std::size(groups)],16);
                            { static const float bl[]{4.f,-5.f,50.f,100.f,150.f,1000.f}; At<float>(o,0x38)=bl[next()%std::size(bl)]; }
                            { const uint32 m=next()%4; At<void*>(o,0x3C)=m==0 ? nullptr : static_cast<void*>(hiers[next()%NHIER].data()); }
                            At<int32>(o,0x40)=next()%3==0 ? -1 : int32(next()%5000); At<uint32>(o,0x44)=now-next()%4000; At<int32>(o,0x48)=int32(next()%4000); At<uint8>(o,0x4C)=uint8(next()%2); At<uint8>(o,0x4D)=uint8(next()%2);
                            At<CVector>(o,0x50)=CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)}; At<uint32>(o,0x5C)=next(); At<int16>(o,0x60)=int16(next());
                            { const uint32 m=next()%3; At<void*>(o,8)=m==0 ? nullptr : static_cast<void*>(assocs[next()%NASSOC].data()); }
                            At<uint8>(o,0xC)=uint8(next()); };
                        if(nm=="Constructor-Default"){ isCtor=true; callFn=[&]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="Constructor-Anim"){ isCtor=true; strInto(vecBuf.data(),names[next()%std::size(names)],24); strInto(vecBuf2.data(),groups[next()%std::size(groups)],16);
                            const int32 fl=int32(next()), end=next()%3==0 ? -1 : int32(next()%5000); const float bl=uni(-5.f,200.f); const uint32 b1=next()%2,b2=next()%2,b3=next()%2,b4=next()%2;
                            callFn=[&,fl,end,bl,b1,b2,b3,b4]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,const char*,const char*,int32,float,int32,uint32,uint32,uint32,uint32)>(target)(o,reinterpret_cast<const char*>(vecBuf.data()),reinterpret_cast<const char*>(vecBuf2.data()),fl,bl,end,b1,b2,b3,b4)); }; }
                        else if(nm=="Destructor"){ fill(); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="Clone"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="GetTaskType"){ fill(); callFn=[&]{ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="OffsetPedPosition"){ fill(); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0); }; }
                        else if(nm=="ProcessPed"){ fill(); At<void*>(o,0)=fakeVtbl.data(); At<int32>(o,FK_TYPE)=TASK_SIMPLE_NAMED_ANIM; At<uint32>(o,FK_ABORT)=next(); At<uint32>(o,FK_CNT)=0;
                            { const float d=uni(0.f,2.f); const CVector pp=pedPos(p0); bonePos=CVector{pp.x+d*(next()%2 ? 1.f : -1.f),pp.y,pp.z}; if(next()%5==0)bonePos=pp; }
                            callFn=[&]{ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0); }; }
                    }
                    // ---- CTaskComplexWalkRoundCar ----
                    else if(wrc) {
                        objSize=0x58; const uintptr_t VT=0x86F308;
                        auto route=[&]()->CPointRoute*{ auto* r=static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
                            if(r){ std::memset(r,0xCD,sizeof(CPointRoute)); const uint32 n=next()%4==0 ? 0u : 1+next()%8; At<uint32>(r,0)=n; for(uint32 k=0;k<n;++k)At<CVector>(r,4+12*k)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                            return r; };
                        auto fill=[&]{ At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; { static const uint8 ms[]{4,6,7,5,0,8}; At<uint8>(o,0xC)=ms[next()%std::size(ms)]; }
                            At<uint8>(o,0xD)=uint8(next()); At<uint8>(o,0xE)=uint8(next());
                            At<CVector>(o,0x10)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; At<CVehicle*>(o,0x1C)=Veh(int(next()%NVEH)); At<CPointRoute*>(o,0x20)=route();
                            At<uint32>(o,0x24)=now-next()%20000; At<int32>(o,0x28)=int32(next()%20000); At<uint8>(o,0x2C)=uint8(next()%2); At<uint8>(o,0x2D)=uint8(next()%2);
                            At<CTask*>(o,8)=Fake(0);
                            auto* v=At<uint8*>(o,0x1C); auto* vm=At<uint8*>(v,E_MATRIX);
                            const bool same=next()%2;
                            At<CVector>(o,0x30)=same ? CVector{At<float>(vm,0x30)+uni(-0.2f,0.2f),At<float>(vm,0x34)+uni(-0.2f,0.2f),At<float>(vm,0x38)+uni(-0.1f,0.1f)} : CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                            At<CVector>(o,0x3C)=same&&next()%2 ? At<CVector>(vm,0x10) : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(o,0x48)=same&&next()%2 ? At<CVector>(vm,0) : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<int32>(o,0x54)=next()%3==0 ? -1 : int32(next()); };
                        if(nm=="Constructor"){ isCtor=true; At<CVector>(vecBuf.data(),0)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                            const int32 ms=int32(next()%9); CVehicle* const v=next()%4==0 ? nullptr : Veh(int(next()%NVEH)); const uint32 forDoor=next()%2, force=next()%3==0 ? next()%256 : next()%16;
                            callFn=[&,ms,v,forDoor,force]{ ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,CVehicle*,uint32,uint32)>(target)(o,ms,reinterpret_cast<const CVector*>(vecBuf.data()),v,forDoor,force)); }; }
                        else if(nm=="Destructor"){ fill(); if(next()%2)Reg(At<CVehicle*>(o,0x1C),o+0x1C); callFn=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="SetNewVehicle"){ fill(); if(next()%2)Reg(At<CVehicle*>(o,0x1C),o+0x1C); CVehicle* const v=Veh(int(next()%NVEH)); const uint32 force=next()%3==0 ? next()%256 : next()%16;
                            callFn=[&,v,force]{ reinterpret_cast<void(__thiscall*)(void*,CVehicle*,uint32)>(target)(o,v,force); }; }
                        else if(nm=="CreateRouteTask"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="GoingForDoor"){ fill(); callFn=[&]{ ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="Clone"){ fill(); callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }; }
                        else if(nm=="GetTaskType"){ fill(); callFn=[&]{ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }; }
                        else if(nm=="CreateNextSubTask"){ fill(); subtask(next()%3==0 ? TASK_SIMPLE_ACHIEVE_HEADING : next()%2 ? TASK_COMPLEX_FOLLOW_POINT_ROUTE : TASK_SIMPLE_PAUSE);
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="ComputeRoute"||nm=="ComputeRouteRoundBigCar"||nm=="ComputeRouteRoundSmallCar"){ fill();
                            { auto* v=At<uint8*>(o,0x1C); const bool big=nm=="ComputeRouteRoundBigCar" ? true : nm=="ComputeRouteRoundSmallCar" ? false : next()%2; At<uint32>(v,V_FLAGS)=(At<uint32>(v,V_FLAGS)&~BIG)|(big ? BIG : 0u); }
                            if(nm=="ComputeRouteRoundBigCar"&&next()%3==0){ hitSides[1]=hitSides[0]; }
                            // el destino pegado a un plano (distancias con signo mixtas) y dentro de los cuatro a veces
                            if(next()%4==0)for(int k=0;k<4;++k)planeData[12+k]=uni(0.5f,2.f);
                            callFn=[&]{ const float r=reinterpret_cast<float(__thiscall*)(void*,CPed*)>(target)(o,p0); ret=F(r); }; }
                        else if(nm=="CreateFirstSubTask"){ fill(); fakeThis(TASK_COMPLEX_WALK_ROUND_CAR,nullptr);
                            { auto* v=At<uint8*>(o,0x1C); At<uint32>(v,V_FLAGS)=(At<uint32>(v,V_FLAGS)&~BIG)|(next()%3==0 ? BIG : 0u); }
                            findTaskRet=next()%3==0 ? 0 : uintptr_t(Fake(1)); At<uint8>(o,0xD)=uint8(At<uint8>(o,0xD)&~2);
                            { CVector pp=pedPos(p0); const float d=uni(0.f,12.f); auto* r=At<uint8*>(o,0x20); if(r&&At<uint32>(r,0)&&next()%2)At<CVector>(r,4)=CVector{pp.x+d,pp.y,pp.z}; }
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                        else if(nm=="ControlSubTask"){ fill(); fakeThis(TASK_COMPLEX_WALK_ROUND_CAR,next()%3 ? Fake(1) : nullptr); At<CTask*>(o,8)=Fake(0);
                            At<uint8>(o,0xD)=uint8(At<uint8>(o,0xD)&~2)|uint8(next()%6==0 ? 2 : 0);
                            At<int32>(fakes[0].data(),FK_TYPE)=next()%4 ? TASK_COMPLEX_FOLLOW_POINT_ROUTE : TASK_SIMPLE_PAUSE;
                            if(next()%4==0)At<CVehicle*>(o,0x1C)=nullptr;
                            findTaskRet=0; At<CTask*>(intels[0].data(),I_TASKMGR+TM_PRIMARY+4*3)=next()%2 ? nullptr : Fake(2);
                            At<int32>(fakes[2].data(),FK_TYPE)=next()%2 ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_SIMPLE_PAUSE;
                            callFn=[&]{ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }; }
                    }
                    if(!callFn){ Write({{"event","error"},{"reason","function not prepared"},{"function",full}}); passed=false; return json{{"error",true}}; }

                    const auto occT=Occ(TaskHdr()), occP=Occ(PRouteHdr()), occN=Occ(NRouteHdr());
                    const auto before=Snapshot();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}};
                    callFn();
                    for(auto* h:hooks)h->State(false);

                    // tareas y rutas nuevas (se nombran antes de serializar el objeto)
                    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks; }
                    { json routes=json::array(); int idx=0; auto* h=PRouteHdr();
                      for(size_t k=0;k<occP.size();++k)if(!occP[k]&&!(States(h)[k]&0x80)){ auto* s=Storage(h)+k*PROUTE_SLOT; freshRoutes[uintptr_t(s)]=idx++; routes.push_back(Region(s,PROUTE_SLOT)); }
                      out["routes"]=routes; }
                    { json nroutes=json::array(); auto* h=NRouteHdr();
                      for(size_t k=0;k<occN.size();++k)if(!occN[k]&&!(States(h)[k]&0x80))nroutes.push_back(Region(Storage(h)+k*NROUTE_SLOT,NROUTE_SLOT));
                      out["nroutes"]=nroutes; }
                    if(isPtr)ret=Name(retPtr);
                    out["object"]=Region(o,objSize,true);
                    if(isCtor)out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls;
                    out["pad"]=json(ReversingProbe::Bytes(pad+0x100,0x20)); out["hitdist"]=At<uint32>(reinterpret_cast<void*>(HITDIST),0);
                    out["sarray"]=Region(reinterpret_cast<void*>(DISPLACEMENTS),16); out["proute_occ"]=Occ(PRouteHdr());
                    // limpieza: referencias de los fixtures y slots nuevos (sin destructor); los pools deben quedar como al empezar
                    CleanRefs();
                    FreeFresh(TaskHdr(),occT0); FreeFresh(PRouteHdr(),occP0); FreeFresh(NRouteHdr(),occN0);
                    out["clean"]=Occ(TaskHdr())==occT0&&Occ(PRouteHdr())==occP0&&Occ(NRouteHdr())==occN0;
                    restoreGlobals();
                    return out;
                };
                Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreGlobals(); resetPools();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occ(TaskHdr())==occTaskStart&&Occ(PRouteHdr())==occPRStart&&Occ(NRouteHdr())==occNRStart&&freeRefs()==savedFree&&globalsIntact()
        &&Last(TaskHdr())==lt0&&Last(PRouteHdr())==lp0&&Last(NRouteHdr())==ln0;
    if(restored){ std::memcpy(States(TaskHdr()),stTask.data(),stTask.size()); std::memcpy(States(PRouteHdr()),stPR.data(),stPR.size()); std::memcpy(States(NRouteHdr()),stNR.data(),stNR.size()); }
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
