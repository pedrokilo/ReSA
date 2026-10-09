#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/MiscWave2): las 5 funciones que faltaban para dejar COMPLETA CCarEnterExit
// del EXE GTA SA 1.0 US: GetNearestCarDoor (0x6528F0), IsVehicleStealable (0x6510D0), MakeUndraggedDriverPedLeaveCar (0x64F600),
// SetAnimOffsetForEnterOrExitVehicle (0x64F860) y SetPedInCarDirect (0x650280). Original (hook desactivado, se ejecuta el codigo
// del EXE) frente a candidato (hook activo), llamando SIEMPRE a la direccion del EXE, sobre objetos falsos en memoria propia.
//
// Colaboradores dobles (EntryPatch en la direccion del EXE, en el simbolo de la DLL y en su cuerpo tras el salto incremental; el
// mismo stub y la misma traza; todas las Body() se calculan ANTES de crear ningun parche; los stubs solo actuan en el hilo del banco):
//  - GetNearestCarDoor: CCarEnterExit::GetPositionToOpenCarDoor (0x64E740, estructura devuelta por puntero oculto), IsRoomForPedToLeaveCar
//    (0x6504C0), IsPathToDoorBlockedByVehicleCollisionModel (0x651210), CTaskManager::GetActiveTask (0x681720; devuelve una tarea falsa
//    con vtable trazada: ranura 4 = GetTaskType) y CPedGroups::AreInSameGroup (0x5F7F40, en la DLL es una llamada directa al EXE).
//  - IsVehicleStealable: CUpsideDownCarCheck::IsCarUpsideDown (0x463830), CVehicle::CanBeDriven (0x6D5400), IsLawEnforcementVehicle
//    (0x6D2370), IsUpsideDown (0x6D1D90), IsOnItsSide (0x6D1DD0), CPedIntelligence::IsFriendlyWith (0x601BC0), CPedGroups::GetPedsGroup
//    (0x5F7E80), AreInSameGroup (0x5F7F40), CPedGroup::IsAnyoneUsingCar (0x5F7DB0) e IsClearToDriveAway (0x6509B0).
//  - MakeUndraggedDriverPedLeaveCar: CEventGroup::Add (0x4AB420), CEntity::RegisterReference (0x571B70) y CleanUpOldReference (0x571A00).
//    El CEventDraggedOutCar se construye y se destruye de verdad (constructor del EXE 0x4AD250 / de la DLL), con peds y vehiculo falsos.
//  - SetAnimOffsetForEnterOrExitVehicle: CAnimManager::GetAnimationBlockIndex (0x4D3990), AddAnimBlockRef (0x4D3FB0), RemoveAnimBlockRef
//    (0x4D3FD0), GetAnimAssociation (0x4D3A60), UncompressAnimation (0x4D41C0) y CStreaming::RequestModel (0x4087E0) /
//    LoadAllRequestedModels (0x40EA10). Asociaciones, jerarquias, secuencias y fotogramas son buferes falsos: no se pide ningun IFP real.
//  - SetPedInCarDirect: CTaskSimpleCarSetPedInAsDriver::ProcessPed (0x64B950) y
//    CTaskSimpleCarSetPedInAsPassenger::ProcessPed (0x64B5D0), CTask::operator new (0x61A5A0; arena propia, NO el pool de tareas real),
//    CTaskManager::SetTask (0x681AF0), CAcquaintance::SetAsAcquaintance (0x608DA0), CPedType::GetPedFlag (0x608830), RegisterReference
//    y CleanUpOldReference. Las tareas locales y las nuevas (CTaskComplexCarDriveWander / CTaskComplexCarDrive) se construyen de verdad.
//    ComputePassengerIndexFromCarDoor (0x64F1E0) NO se dobla: se ejecuta de verdad (es una funcion pura de tipo de vehiculo, bit
//    TandemSeats y puerta) porque SetPedInCarDirect del EXE guarda doorId en EDX a traves de esa llamada (0x650356 -> `push edx` en
//    0x650360) y solo el hook con envoltura que conserva EDX (ComputePassengerIndexFromCarDoor_KeepEdx) la hace correcta con el hook
//    de SetPedInCarDirect desactivado: la comparacion original/candidato tambien ejercita esa envoltura.
// Referencias: RegisterReference/CleanUpOldReference estan dobladas, asi que ninguna entidad falsa entra en la lista real de referencias
// ni se toca el pool de referencias; la traza compara quien se registra/limpia, en que orden y en que puntero (los de la pila se
// expresan relativos a la primera referencia de pila de la ejecucion).
#include <fstream>
#include <set>
#include <cmath>
#include <limits>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "CarEnterExit.h"
#include "Events/EventGroup.h"
#include "Events/EventDraggedOutCar.h"
#include "Tasks/TaskManager.h"
#include "TaskSimpleCarSetPedInAsDriver.h"
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexDriveWander.h"
#include "Entity/Vehicle/Automobile.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "PedStats.h"
#include "PedType.h"
#include "Acquaintance.h"
#include "UpsideDownCarCheck.h"
#include "AnimManager.h"
#include "AnimBlendStaticAssociation.h"
#include "AnimBlendHierarchy.h"
#include "AnimBlendSequence.h"
#include "Streaming.h"
#include "../ProbeRuntime.hpp"

namespace CarEnterWave4Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 Hh(uint32 a,uint32 b=0){ uint32 x=(a+0x9E3779B9u)*2654435761u; x^=(b+0x7F4A7C15u)*40503u; x^=x>>15; x*=2246822519u; x^=x>>13; x*=3266489917u; x^=x>>16; return x; }
inline uint32 F(float f){ return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int64_t> l){ return int32(l.begin()[next()%l.size()]); } };
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline float QNaN(){ return std::numeric_limits<float>::quiet_NaN(); }
inline float RandF(Rng& r){ const uint32 k=r.below(100); if(k<6)return 0.f; if(k<10)return r.pick({1.f,-1.f,0.5f,90.f,89.99f,30.f,-30.f,1e-30f}); return r.uni(-300.f,300.f); }

// Hilo del banco: otros hilos del juego (audio, streaming) pueden llamar a funciones parcheadas; no se trazan ni tocan estado.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

// ---- buferes con nombre (para identificar punteros en las trazas) ----
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
// Referencias: las de la pila se expresan relativas a la primera vista en la ejecucion (las pilas del EXE y de la DLL difieren).
inline uintptr_t stackLo{}, stackHi{}, firstStackRef{};
inline json RefName(void** ref){
    const auto a=uintptr_t(ref);
    for(auto& g:named)if(a>=uintptr_t(g.p)&&a<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(a-uintptr_t(g.p));
    if(a>=stackLo&&a<stackHi){ if(!firstStackRef)firstStackRef=a; return json::array({"pila",int64_t(a)-int64_t(firstStackRef)}); }
    return "other";
}
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// ---- desplazamientos del EXE usados por los fixtures ----
constexpr size_t V_VPTR=0, V_POS=4, V_HEAD=0x10, V_MAT=0x14, V_HDL=0x384, V_STYLE=0x3B9, V_CRUISE=0x3D0, V_FLAGS=0x428, V_DRIVER=0x460, V_PSGR=0x464,
                 V_GETIN=0x486, V_FIRE=0x490, V_CREATED=0x4A4, V_HEALTH=0x4C0, V_TOWED=0x4C8, V_TYPE=0x590, V_SUB=0x594, V_NODES=0x648;
constexpr size_t H_FLAGS=0xCC;
constexpr size_t P_MAT=0x14, P_FLAGS=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_CREATED=0x484, P_ACQ=0x4E0, P_MOVE=0x534, P_VEH=0x58C, P_TYPE=0x598, P_STATS=0x59C;
constexpr size_t kBig=0x800, kPeds=11, kTaskSlot=0x60, kTaskSlots=4;
constexpr uintptr_t kAnimRegion=0xC18C20; constexpr size_t kAnimRegionSize=0x70;   // ms_bPedOffsetsCalculated + 6 vectores (0xC18C3C..0xC18C84)
constexpr uintptr_t kAnimOffset0=0xC18C48;                                           // ms_vecPedQuickDraggedOutCarAnimOffset

// ---- buferes ----
alignas(16) inline std::array<uint8,kBig> vehBuf{}, veh2Buf{};
alignas(16) inline std::array<uint8,0x100> hdlBuf{};
alignas(16) inline std::array<uint8,0x60> vehMat{};
alignas(16) inline std::array<std::array<uint8,kBig>,kPeds> pedBuf{};      // 0 conductor, 1..8 pasajeros 0..7, 9 sujeto, 10 otro
alignas(16) inline std::array<std::array<uint8,0x400>,kPeds> intelBuf{};
alignas(16) inline std::array<std::array<uint8,0x60>,kPeds> pedMat{}, statsBuf{}, pdataBuf{};
alignas(16) inline std::array<uint8,0x100> grpBuf{};
alignas(16) inline std::array<uint8,0x40> taskFake{};                       // tarea activa falsa (vtable trazada)
alignas(16) inline std::array<uint8,kTaskSlot*kTaskSlots> taskArena{};      // sustituye al pool de tareas (CTask::operator new)
alignas(16) inline std::array<std::array<uint8,0x20>,5> aAssoc{}, aHier{};  // CAnimBlendStaticAssociation / CAnimBlendHierarchy falsas
alignas(16) inline std::array<std::array<uint8,0x24>,5> aSeq{};
alignas(16) inline std::array<std::array<uint8,0x100>,5> aFrU{}, aFrC{};    // fotogramas descomprimidos / comprimidos
inline std::array<void*,128> trapVt{};                                      // vtable falsa de vehiculos y peds
inline std::array<void*,32> taskVt{};                                       // vtable de la tarea falsa
inline CVehicle* Veh(){ return reinterpret_cast<CVehicle*>(vehBuf.data()); }
inline CPed* Ped(size_t k){ return reinterpret_cast<CPed*>(pedBuf[k].data()); }

// ---- configuracion del caso para los dobles ----
inline uint32 sSeed{}, pRoom{50}, pSame{50}, pathN{}, taskNewCount{};
inline int32 sameOverride{-1}, taskTypeRet{}, houseIdx{}, officeIdx{};
inline bool blk[13]{};
inline void* grpRet{};

// ======================= dobles =======================
template<int N> inline uintptr_t __fastcall VTrap(void*,void*){ if(Mine())T("virtual_inesperada",N); return 0; }
template<int... Ns> inline void FillTrap(std::integer_sequence<int,Ns...>){ ((trapVt[size_t(Ns)]=reinterpret_cast<void*>(&VTrap<Ns>)),...); ((taskVt[size_t(Ns)%32]=reinterpret_cast<void*>(&VTrap<Ns>)),...); }
inline int32 __fastcall FTaskType(void* self,void*){ if(Mine())T("get_task_type",self); return taskTypeRet; }

inline int PedIdx(const void* p){ for(size_t k=0;k<kPeds;++k)if(p==pedBuf[k].data())return int(k); return -1; }

// GetNearestCarDoor
inline CVector* __cdecl S_GetPos(CVector* out,const CVehicle* v,int32 door){
    if(!Mine()){ *out=CVector{0.f,0.f,0.f}; return out; }
    T("get_position_to_open_car_door",v,door);
    const uint32 m=Hh(sSeed,900u); const bool tie=(m%8)==0;
    const uint32 hh=tie ? Hh(sSeed,901u) : Hh(sSeed,uint32(door)+7000u);
    float x=float(int(hh%2001)-1000)*0.37f, y=float(int((hh>>11)%2001)-1000)*0.37f, z=float(int((hh>>5)%61)-30)*0.5f;
    if(m%61==5 && door==10)x=QNaN();
    *out=CVector{x,y,z}; return out; }
inline bool __cdecl S_IsRoom(const CVehicle* v,int32 door,const CVector* pos){
    if(!Mine())return false;
    const uint32 kind= !pos ? 0u : (uintptr_t(pos)==kAnimOffset0 ? 1u : 2u);
    json e=json::array({"is_room_for_ped_to_leave_car",Name(uintptr_t(v)),door,kind});
    if(kind==2){ e.push_back(F(pos->x)); e.push_back(F(pos->y)); e.push_back(F(pos->z)); }
    trace.push_back(std::move(e));
    return Hh(sSeed,uint32(door&0xFF)*8u+kind+8000u)%100<pRoom; }
inline bool __cdecl S_PathBlocked(const CPed* ped,const CVehicle* v,const CVector& pos){
    if(!Mine())return false;
    T("is_path_to_door_blocked",ped,v,pos.x,pos.y,pos.z);
    return Hh(sSeed,pathN++ +8500u)%100<50; }
inline void* __fastcall S_GetActive(void* self,void*){ if(!Mine())return nullptr; T("get_active_task",self); return taskFake.data(); }
inline bool __cdecl S_Same(const CPed* a,const CPed* b){
    if(!Mine())return false;
    T("are_in_same_group",a,b);
    if(sameOverride>=0)return sameOverride!=0;
    return Hh(sSeed,uint32(PedIdx(a)+1)*16u+uint32(PedIdx(b)+1)+9000u)%100<pSame; }

// IsVehicleStealable
inline bool __fastcall S_UpsideCheck(void* self,void*,const CVehicle* v){ if(!Mine())return false; T("is_car_upside_down",self!=nullptr,v); return blk[0]; }
inline bool __fastcall S_CanDrive(void* self,void*){ if(!Mine())return true; T("can_be_driven",self); return !blk[1]; }
inline bool __fastcall S_IsLaw(void* self,void*){ if(!Mine())return false; T("is_law_enforcement_vehicle",self); return blk[2]; }
inline bool __fastcall S_Friendly(void* self,void*,const CPed* other){ if(!Mine())return false; T("is_friendly_with",self,other); return blk[5]; }
inline void* __cdecl S_GetGroup(const CPed* ped){ if(!Mine())return nullptr; T("get_peds_group",ped); return grpRet; }
inline bool __fastcall S_Using(void* self,void*,const CVehicle* v){ if(!Mine())return false; T("is_anyone_using_car",self,v); return blk[7]; }
inline bool __fastcall S_UpsideDown(void* self,void*){ if(!Mine())return false; T("is_upside_down",self); return blk[10]; }
inline bool __fastcall S_OnSide(void* self,void*){ if(!Mine())return false; T("is_on_its_side",self); return blk[11]; }
inline bool __cdecl S_Clear(const CVehicle* v){ if(!Mine())return true; T("is_clear_to_drive_away",v); return !blk[12]; }

// MakeUndraggedDriverPedLeaveCar y referencias
inline CEvent* __fastcall S_EvAdd(void* self,void*,CEvent* ev,uint32 valid){
    if(!Mine())return nullptr;
    auto* e=reinterpret_cast<uint8*>(ev);
    json j=json::array({"event_group_add",Name(uintptr_t(self)),int(ev->GetEventType()),ev->GetEventPriority(),
        At<uint32>(e,4),At<uint8>(e,8),At<uint8>(e,0xC),At<uint16>(e,0xE),At<uint16>(e,0x10),Name(At<uint32>(e,0x14)),Name(At<uint32>(e,0x18)),At<uint8>(e,0x1C),valid&0xFF});
    trace.push_back(std::move(j)); return nullptr; }
inline void __fastcall S_Reg(void* self,void*,void** ref){ if(Mine())trace.push_back(json::array({"register_reference",Name(uintptr_t(self)),RefName(ref)})); }
inline void __fastcall S_CleanRef(void* self,void*,void** ref){ if(Mine())trace.push_back(json::array({"clean_up_old_reference",Name(uintptr_t(self)),RefName(ref)})); }

// SetAnimOffsetForEnterOrExitVehicle
inline int32 __cdecl S_BlkIndex(const char* name){
    if(!Mine())return -1;
    std::string s; for(int k=0;k<31&&name&&name[k];++k)s.push_back(name[k]);
    trace.push_back(json::array({"get_animation_block_index",s}));
    return s=="int_house" ? houseIdx : s=="int_office" ? officeIdx : 77; }
inline void __cdecl S_ReqModel(int32 model,int32 flags){ if(Mine())T("request_model",model,flags); }
inline void __cdecl S_LoadAll(uint32 b){ if(Mine())T("load_all_requested_models",b&0xFF); }
inline void __cdecl S_AddRef(int32 idx){ if(Mine())T("add_anim_block_ref",idx); }
inline void __cdecl S_RemRef(int32 idx){ if(Mine())T("remove_anim_block_ref",idx); }
inline int AnimSlot(int32 grp,int32 anim){ if(grp==0&&anim==112)return 0; if(grp==84&&anim==337)return 1; if(grp==84&&anim==340)return 2; if(grp==85&&anim==328)return 3; if(grp==84&&anim==343)return 4; return -1; }
inline void* __cdecl S_GetAssoc(int32 grp,int32 anim){
    if(!Mine())return nullptr;
    const int k=AnimSlot(grp,anim); T("get_anim_association",grp,anim,k);
    return aAssoc[size_t(k<0 ? 0 : k)].data(); }
inline void __cdecl S_Uncompress(void* hier){
    if(!Mine())return;
    int k=-1; for(size_t j=0;j<aHier.size();++j)if(hier==aHier[j].data())k=int(j);
    T("uncompress_animation",k);
    if(k>=0){ auto* sq=aSeq[size_t(k)].data(); At<void*>(sq,8)=aFrU[size_t(k)].data(); At<uint16>(sq,4)=uint16(At<uint16>(sq,4)&~4u); } }

// SetPedInCarDirect
inline bool __fastcall S_ProcDriver(void* self,void*,CPed* ped){
    if(!Mine())return false; auto* t=static_cast<uint8*>(self);
    T("process_ped_as_driver",ped,At<uint32>(t,4),At<uint8>(t,8),At<uint32>(t,0xC),Name(At<uint32>(t,0x10)),Name(At<uint32>(t,0x14)),At<uint8>(t,0x18),At<uint8>(t,0x19),At<uint8>(t,0x1A));
    return true; }
inline bool __fastcall S_ProcPsgr(void* self,void*,CPed* ped){
    if(!Mine())return false; auto* t=static_cast<uint8*>(self);
    T("process_ped_as_passenger",ped,At<uint32>(t,4),At<uint8>(t,8),At<uint32>(t,0xC),Name(At<uint32>(t,0x10)),At<int32>(t,0x14),Name(At<uint32>(t,0x18)),At<uint8>(t,0x1C),At<uint8>(t,0x1D),At<uint8>(t,0x1E));
    return true; }
inline void* __cdecl S_TaskNew(size_t n){
    if(!Mine())return nullptr;
    T("task_new",uint32(n)); const size_t k= taskNewCount<kTaskSlots-1 ? taskNewCount : kTaskSlots-1; ++taskNewCount;
    return taskArena.data()+kTaskSlot*k; }
inline void __fastcall S_SetTask(void* self,void*,void* task,int32 slot,uint32 unused){ if(Mine())T("set_task",self,task,slot,unused&0xFF); }
inline void __fastcall S_SetAcq(void* self,void*,int32 id,uint32 flag){ if(Mine())T("set_as_acquaintance",self,id,flag); }
inline uint32 __cdecl S_PedFlag(int32 type){ if(!Mine())return 0; T("get_ped_flag",type); return 0x9E3779B9u*uint32(type+1); }

// ======================= utilidades de comparacion =======================
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }
inline uint16 FpuTag() {   // pila x87 vacia = 0xFFFF; fnstenv enmascara las excepciones, fldenv restaura el entorno tal cual
    uint8 env[28]{};
    __asm { lea eax, env
            fnstenv [eax]
            fldenv [eax] }
    uint16 tag; std::memcpy(&tag,env+8,2); return tag;
}
// Resumen de TODO el estado de los buferes (los objetos de la arena sin su vptr: el EXE y la DLL ponen el suyo).
inline std::string Digest(){
    uint64 h=1469598103934665603ull;
    h=Fnv(vehBuf.data(),vehBuf.size(),h); h=Fnv(veh2Buf.data(),veh2Buf.size(),h); h=Fnv(hdlBuf.data(),hdlBuf.size(),h); h=Fnv(vehMat.data(),vehMat.size(),h);
    for(size_t k=0;k<kPeds;++k){ h=Fnv(pedBuf[k].data(),kBig,h); h=Fnv(intelBuf[k].data(),0x400,h); h=Fnv(statsBuf[k].data(),0x60,h); h=Fnv(pdataBuf[k].data(),0x60,h); h=Fnv(pedMat[k].data(),0x60,h); }
    h=Fnv(grpBuf.data(),grpBuf.size(),h); h=Fnv(taskFake.data(),taskFake.size(),h);
    for(size_t k=0;k<kTaskSlots;++k)h=Fnv(taskArena.data()+k*kTaskSlot+4,kTaskSlot-4,h);
    for(size_t k=0;k<5;++k){ h=Fnv(aAssoc[k].data(),0x20,h); h=Fnv(aHier[k].data(),0x20,h); h=Fnv(aSeq[k].data(),0x24,h); h=Fnv(aFrU[k].data(),0x100,h); h=Fnv(aFrC[k].data(),0x100,h); }
    for(auto& g:regions)h=Fnv(g.p,g.n,h);
    return std::format("{:016x}",h);
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

// ---- modulo de la DLL y busqueda de cuerpos (solo se siguen saltos E9 que quedan DENTRO de la DLL) ----
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }

struct Site { const char* name; uintptr_t exe, sym, body; void* stub; };
inline std::vector<Site> sites;
inline void AddSite(const char* n,uintptr_t exeAddr,uintptr_t sym,void* stub){ sites.push_back({n,exeAddr,sym,sym ? FollowIn(sym) : 0,stub}); }
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    void One(uintptr_t at,void* stub){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,stub)); }
    void Use(std::initializer_list<const char*> names){
        for(const char* nm:names){ bool found=false;
            for(auto& s:sites)if(std::string(s.name)==nm){ found=true; One(s.exe,s.stub); One(s.sym,s.stub); One(s.body,s.stub); }
            if(!found)std::abort(); } }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,5> fns{{
    {"CCarEnterExit","GetNearestCarDoor",0x6528F0,0,-1,1024},
    {"CCarEnterExit","IsVehicleStealable",0x6510D0,0,-1,1024},
    {"CCarEnterExit","MakeUndraggedDriverPedLeaveCar",0x64F600,0,-1,256},
    {"CCarEnterExit","SetAnimOffsetForEnterOrExitVehicle",0x64F860,0,-1,512},
    {"CCarEnterExit","SetPedInCarDirect",0x650280,0,-1,1024},
}};

// ======================= fixtures =======================
// Peds: 0 conductor, 1..8 pasajeros, 9 sujeto, 10 otro. Campos reales con desplazamientos del EXE; el resto, basura aleatoria.
inline void BuildPeds(Rng& r) {
    for(size_t k=0;k<kPeds;++k){
        auto* p=pedBuf[k].data();
        FillBytes(p,kBig,r); FillBytes(intelBuf[k].data(),0x400,r); FillBytes(statsBuf[k].data(),0x60,r); FillBytes(pdataBuf[k].data(),0x60,r); FillBytes(pedMat[k].data(),0x60,r);
        At<void*>(p,0)=trapVt.data();
        At<void*>(p,P_MAT)=nullptr; At<void*>(p,P_INTEL)=intelBuf[k].data(); At<void*>(p,P_PDATA)=pdataBuf[k].data(); At<void*>(p,P_STATS)=statsBuf[k].data();
        for(size_t o=4;o<0x14;o+=4)At<float>(p,o)=RandF(r);
        At<uint32>(p,P_FLAGS)=(r.next()&~0x20000000u)|(r.pct(30) ? 0x20000000u : 0u);   // bit 29 = bDontDragMeOutCar
        At<uint8>(p,P_CREATED)=uint8(r.below(4));
        At<uint32>(p,P_MOVE)= r.pct(50) ? 6u : r.below(8);
        At<uint32>(p,P_TYPE)= r.pct(30) ? r.below(2) : uint32(r.pickI({2,3,4,5,6,6,7,8,22}));
        At<void*>(p,P_VEH)=nullptr;
    }
    std::memset(grpBuf.data(),0xA5,grpBuf.size()); std::memset(taskFake.data(),0x5A,taskFake.size()); At<void*>(taskFake.data(),0)=taskVt.data();
}
inline void FillVehicle(Rng& r) {
    FillBytes(vehBuf.data(),kBig,r); FillBytes(veh2Buf.data(),kBig,r); FillBytes(hdlBuf.data(),0x100,r); FillBytes(vehMat.data(),0x60,r);
    At<void*>(vehBuf.data(),V_VPTR)=trapVt.data(); At<void*>(veh2Buf.data(),V_VPTR)=trapVt.data();
    At<void*>(vehBuf.data(),V_HDL)=hdlBuf.data(); At<void*>(vehBuf.data(),V_MAT)=nullptr; At<void*>(veh2Buf.data(),V_MAT)=nullptr;
}

// ---- GetNearestCarDoor ----
inline json CaseNearest(Rng& r,uintptr_t addr) {
    BuildPeds(r); FillVehicle(r);
    auto* v=vehBuf.data(); auto* h=hdlBuf.data(); auto* P=pedBuf[9].data();
    const uint32 cls=r.below(10);
    uint32 type=0,sub=0; bool bus=false,low=false;
    switch(cls){ case 2: bus=true; break; case 3: low=true; break; case 5: type=9; sub=9; break; case 6: type=9; sub=10; break; case 7: type=2; sub=2; break;
                 case 8: type=1; sub=1; break; case 9: type=5; sub=5; break; default: break; }
    if(r.pct(10))bus=!bus;
    if(r.pct(10))low=!low;
    const bool force=(cls==4)||r.pct(10), tandem=r.pct(20);
    At<uint32>(v,V_TYPE)=type; At<uint32>(v,V_SUB)=sub;
    At<uint32>(v,V_FLAGS)=(r.next()&~0xA00u)|(bus ? 0x200u : 0u)|(low ? 0x800u : 0u);   // bit 9 = bIsBus, bit 11 = bLowVehicle
    At<uint32>(h,H_FLAGS)=(r.next()&~0x8200u)|(force ? 0x8000u : 0u)|(tandem ? 0x200u : 0u);   // bit 15 = ForceDoorCheck, bit 9 = TandemSeats
    At<uint32>(v,V_NODES+4*8)= r.pct(60) ? 0x1000u : 0u;
    At<uint32>(v,V_NODES+4*10)= r.pct(60) ? 0x1000u : 0u;
    At<void*>(v,V_DRIVER)= r.pct(70) ? static_cast<void*>(pedBuf[0].data()) : nullptr;
    for(unsigned k=0;k<8;++k)At<void*>(v,V_PSGR+4*k)= r.pct(k==0 ? 55 : 30) ? static_cast<void*>(pedBuf[1+k].data()) : nullptr;
    At<uint8>(v,V_GETIN)=uint8(r.next()); At<uint8>(v,V_CREATED)=uint8(r.pickI({0,1,2,2,3,4}));
    At<void*>(v,V_TOWED)= r.pct(35) ? static_cast<void*>(veh2Buf.data()) : nullptr;
    const bool bikeBranch=(type==9&&sub!=10)||sub==2;
    const bool useMat=bikeBranch||r.pct(50);   // el EXE lee la matriz del vehiculo sin comprobarla en la rama de moto
    const float hdg= r.pct(20) ? r.pick({3.0f,3.1f,-3.0f,-3.1f,3.14159f,-3.14159f,0.f}) : r.uni(-3.1415f,3.1415f);
    const float rz= r.pct(65) ? r.uni(-0.099f,0.099f) : r.pick({0.1f,-0.1f,0.0999999f,std::bit_cast<float>(0x3DCCCCCCu),0.2f,-0.5f,1.f,0.f,-0.f,QNaN()});
    const float vx=r.uni(-300.f,300.f), vy=r.uni(-300.f,300.f), vz=r.uni(0.f,30.f);
    if(useMat){
        auto* m=vehMat.data(); At<void*>(v,V_MAT)=m;
        At<float>(m,0)=std::cos(hdg); At<float>(m,4)=std::sin(hdg); At<float>(m,8)=rz;
        At<float>(m,0x10)=-std::sin(hdg); At<float>(m,0x14)=std::cos(hdg); At<float>(m,0x18)=0.f;
        At<float>(m,0x30)=vx; At<float>(m,0x34)=vy; At<float>(m,0x38)=vz;
    } else { At<float>(v,V_POS)=vx; At<float>(v,V_POS+4)=vy; At<float>(v,V_POS+8)=vz; At<float>(v,V_HEAD)=hdg; }
    // sujeto: rumbo hacia el vehiculo cercano al del vehiculo (rama |angulo| < PI/6), con fronteras y envolturas
    static constexpr float kDelta[]{0.f,0.1f,-0.1f,0.4f,-0.4f,0.5235988f,-0.5235988f,0.52359873f,0.5235989f,-0.52359873f,-0.5235989f,1.0472f,2.0f,-2.0f,3.0f,-3.0f,0.3f,-0.3f};
    const float delta= r.pct(40) ? r.uni(-0.9f,0.9f) : kDelta[r.below(18)];
    const double a=double(hdg)+double(delta); const float dist=r.pick({0.f,0.001f,1.f,5.f,50.f,200.f});
    const float px=float(double(vx)+double(dist)*(-std::sin(a))), py=float(double(vy)+double(dist)*std::cos(a)), pz=r.uni(0.f,30.f);
    if(r.pct(50)){ auto* m=pedMat[9].data(); At<void*>(P,P_MAT)=m; At<float>(m,0x30)=px; At<float>(m,0x34)=py; At<float>(m,0x38)=pz; }
    else { At<float>(P,4)=px; At<float>(P,8)=py; At<float>(P,12)=pz; }
    // jugador / no jugador, temperamento, estado de movimiento y ratio de movimiento
    At<uint32>(P,P_TYPE)= r.pct(25) ? r.below(2) : uint32(r.pickI({2,3,4,5,6,6,7,8,22}));
    At<uint32>(P,P_MOVE)= r.pct(55) ? 6u : r.below(8);
    At<uint8>(statsBuf[9].data(),0x25)=uint8(r.pickI({0x41,0x42,0x40,0x7F,0x80,0xFF,0x00,0x50,0x5A}));
    At<float>(pdataBuf[9].data(),0x14)=r.pick({2.0f,1.5f,1.5000001f,3.f,0.f,-1.f,QNaN(),1.4999999f});
    const int32 doorInit=r.pickI({0,0,0,8,8,10,10,18,18,9,11,-1,5,100});
    sSeed=r.next(); pRoom=uint32(r.pickI({15,50,85})); pSame=uint32(r.pickI({10,50,90})); sameOverride=-1;
    taskTypeRet= r.pct(30) ? 700 : r.pickI({0,699,701,1000,-1});
    std::array<uint32,3> outPos; std::memset(outPos.data(),0xAB,sizeof(outPos)); int32 door=doorInit;
    const bool ret=reinterpret_cast<bool(__cdecl*)(CPed*,CVehicle*,CVector*,int32*)>(addr)(Ped(9),Veh(),reinterpret_cast<CVector*>(outPos.data()),&door);
    return json{{"ret",ret},{"pos",outPos},{"door",door},{"door_in",doorInit}};
}

// ---- IsVehicleStealable ----
inline json CaseSteal(Rng& r,uintptr_t addr) {
    BuildPeds(r); FillVehicle(r);
    auto* v=vehBuf.data(); auto* S=pedBuf[9].data(); auto* D=pedBuf[0].data();
    for(auto& b:blk)b=false;
    const uint32 stage=r.below(20); if(stage<13)blk[stage]=true;
    for(auto& b:blk)if(r.pct(3))b=true;
    const uint32 pre=r.below(24);   // 0: subtipo avion/helicoptero, 1: tipo no valido, 2: creador no valido sin vehiculo propio
    uint32 sub=uint32(r.pickI({0,0,0,9,2,10,1,5})), type= r.pct(70) ? 0u : 9u;
    if(pre==0)sub= r.pct(50) ? 3u : 4u;
    if(pre==1)type=uint32(r.pickI({1,2,3,4,5,6,7,8,10,11,12}));
    At<uint32>(v,V_SUB)=sub; At<uint32>(v,V_TYPE)=type;
    uint8 created; void* pedVeh=nullptr;
    if(pre==2){ created=uint8(r.pickI({0,2,4})); pedVeh= r.pct(50) ? nullptr : static_cast<void*>(veh2Buf.data()); }
    else switch(r.below(4)){
        case 0: case 1: created=uint8(r.pickI({1,3})); pedVeh= r.pct(50) ? static_cast<void*>(v) : nullptr; break;
        case 2: created=uint8(r.pickI({0,2,4})); pedVeh=v; break;
        default: created=uint8(r.pickI({1,3})); pedVeh=veh2Buf.data(); break; }
    At<uint8>(v,V_CREATED)=created; At<void*>(S,P_VEH)=pedVeh;
    At<void*>(v,V_FIRE)= blk[8] ? static_cast<void*>(hdlBuf.data()) : nullptr;
    At<float>(v,V_HEALTH)= blk[9] ? r.pick({600.f,599.99994f,0.f,-5.f,QNaN(),-std::numeric_limits<float>::infinity()})
                                  : r.pick({1000.f,601.f,650.f,std::nextafter(600.f,1000.f),std::numeric_limits<float>::infinity()});
    const bool needDrv=blk[3]||blk[4]||blk[5]||blk[6]||r.pct(55);
    At<void*>(v,V_DRIVER)= needDrv ? static_cast<void*>(D) : nullptr;
    At<uint8>(D,P_CREATED)= blk[3] ? uint8(2) : uint8(r.pickI({0,1,3}));
    At<uint32>(D,P_TYPE)= blk[4] ? r.below(2) : uint32(r.pickI({2,3,4,5,6,7,8,22}));
    grpRet= (blk[7]||r.pct(50)) ? static_cast<void*>(grpBuf.data()) : nullptr;
    sameOverride= blk[6] ? 1 : 0; sSeed=r.next();
    const bool ret=reinterpret_cast<bool(__cdecl*)(CVehicle*,CPed*)>(addr)(Veh(),Ped(9));
    return json{{"ret",ret}};
}

// ---- MakeUndraggedDriverPedLeaveCar ----
inline json CaseUndrag(Rng& r,uintptr_t addr) {
    BuildPeds(r); FillVehicle(r);
    auto* v=vehBuf.data();
    At<void*>(v,V_DRIVER)=pedBuf[0].data();
    for(unsigned k=0;k<8;++k)At<void*>(v,V_PSGR+4*k)= r.pct(40) ? static_cast<void*>(pedBuf[1+k].data()) : nullptr;
    CPed* ped= r.pct(15) ? nullptr : Ped(r.below(uint32(kPeds)));
    reinterpret_cast<void(__cdecl*)(CVehicle*,CPed*)>(addr)(Veh(),ped);
    return json{{"ret",0},{"ped",Name(uintptr_t(ped))}};
}

// ---- SetAnimOffsetForEnterOrExitVehicle ----
inline json CaseAnim(Rng& r,uintptr_t addr) {
    BuildPeds(r);
    for(size_t k=0;k<5;++k){
        FillBytes(aAssoc[k].data(),0x20,r); FillBytes(aHier[k].data(),0x20,r); FillBytes(aSeq[k].data(),0x24,r);
        At<void*>(aAssoc[k].data(),0x10)=aHier[k].data();
        At<void*>(aHier[k].data(),4)=aSeq[k].data(); At<uint16>(aHier[k].data(),8)=uint16(1+r.below(3));
        const bool trans=r.pct(50), comp=r.pct(60);
        At<uint16>(aSeq[k].data(),4)=uint16((r.next()&~0x6u)|(trans ? 2u : 0u)|(comp ? 4u : 0u));   // bit 1 = traslacion, bit 2 = comprimida
        At<uint16>(aSeq[k].data(),6)=uint16(int16(r.pickI({-1,0,1,1,2,3,4,7,-32768,5})));
        At<void*>(aSeq[k].data(),8)= comp ? static_cast<void*>(aFrC[k].data()) : static_cast<void*>(aFrU[k].data());
        for(size_t o=0;o<0x100;o+=4){ At<float>(aFrU[k].data(),o)=float(int(r.below(2001))-1000)*0.37f; At<float>(aFrC[k].data(),o)=float(int(r.below(2001))-1000)*0.11f; }
    }
    houseIdx=r.pickI({0,1,5,17,100,-1,299}); officeIdx= r.pct(10) ? houseIdx : r.pickI({0,1,5,17,100,-1,299,42});
    AtA<uint8>(kAnimRegion)= r.pct(10) ? 1 : 0;
    for(uintptr_t a=0xC18C3C;a<0xC18C88;a+=4)AtA<float>(a)=RandF(r);
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return json{{"ret",0},{"flag",AtA<uint8>(kAnimRegion)}};
}

// ---- SetPedInCarDirect ----
inline json CaseSetPed(Rng& r,uintptr_t addr) {
    BuildPeds(r); FillVehicle(r);
    auto* v=vehBuf.data();
    const bool bike=r.pct(30);
    At<uint32>(v,V_TYPE)= bike ? 9u : uint32(r.pickI({0,0,0,1,5,2}));
    At<void*>(v,V_DRIVER)= r.pct(70) ? static_cast<void*>(pedBuf[0].data()) : nullptr;
    for(unsigned k=0;k<8;++k)At<void*>(v,V_PSGR+4*k)= r.pct(45) ? static_cast<void*>(pedBuf[1+k].data()) : nullptr;
    At<uint8>(v,V_STYLE)=uint8(r.pickI({0,1,2,3,4,5,6,0x7F,0x80,0xFF,0xFE}));
    At<uint8>(v,V_CRUISE)=uint8(r.pickI({0,1,20,40,0x7F,0x80,0xFF}));
    std::memset(taskArena.data(),0xCD,taskArena.size()); taskNewCount=0;
    At<uint32>(hdlBuf.data(),H_FLAGS)=(r.next()&~0x200u)|(r.pct(30) ? 0x200u : 0u);   // bit 9 = TandemSeats (ComputePassengerIndex real)
    const bool asDriver=r.pct(40); const int32 doorId=r.pickI({-1,0,8,9,10,11,18,100,8,9,11});
    sSeed=r.next();
    const bool ret=reinterpret_cast<bool(__cdecl*)(CPed*,CVehicle*,int32,bool)>(addr)(Ped(9),Veh(),doorId,asDriver);
    return json{{"ret",ret}};
}

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carenterwave4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carenterwave4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carenterwave4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    { auto* tib=reinterpret_cast<NT_TIB*>(NtCurrentTeb()); stackHi=uintptr_t(tib->StackBase); stackLo=stackHi-0x100000; }
    std::string only; { std::ifstream f(dir/"carenterwave4-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,5> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- vtables falsas, nombres de buferes ---
    FillTrap(std::make_integer_sequence<int,32>{});
    for(size_t k=32;k<trapVt.size();++k)trapVt[k]=reinterpret_cast<void*>(&VTrap<31>);
    taskVt[4]=reinterpret_cast<void*>(&FTaskType);
    if(named.empty()){
        Nm(vehBuf.data(),kBig,"veh"); Nm(veh2Buf.data(),kBig,"veh2"); Nm(hdlBuf.data(),0x100,"handling"); Nm(vehMat.data(),0x60,"vehmat");
        static const char* np[kPeds]{"driver","psgr0","psgr1","psgr2","psgr3","psgr4","psgr5","psgr6","psgr7","subject","other"};
        static const char* ni[kPeds]{"intel_driver","intel_psgr0","intel_psgr1","intel_psgr2","intel_psgr3","intel_psgr4","intel_psgr5","intel_psgr6","intel_psgr7","intel_subject","intel_other"};
        static const char* nm[kPeds]{"mat_driver","mat_psgr0","mat_psgr1","mat_psgr2","mat_psgr3","mat_psgr4","mat_psgr5","mat_psgr6","mat_psgr7","mat_subject","mat_other"};
        static const char* ns[kPeds]{"stats_driver","stats_psgr0","stats_psgr1","stats_psgr2","stats_psgr3","stats_psgr4","stats_psgr5","stats_psgr6","stats_psgr7","stats_subject","stats_other"};
        static const char* nd[kPeds]{"pdata_driver","pdata_psgr0","pdata_psgr1","pdata_psgr2","pdata_psgr3","pdata_psgr4","pdata_psgr5","pdata_psgr6","pdata_psgr7","pdata_subject","pdata_other"};
        for(size_t k=0;k<kPeds;++k){ Nm(pedBuf[k].data(),kBig,np[k]); Nm(intelBuf[k].data(),0x400,ni[k]); Nm(pedMat[k].data(),0x60,nm[k]); Nm(statsBuf[k].data(),0x60,ns[k]); Nm(pdataBuf[k].data(),0x60,nd[k]); }
        Nm(grpBuf.data(),grpBuf.size(),"group"); Nm(taskFake.data(),taskFake.size(),"active_task");
        static const char* nt[kTaskSlots]{"task0","task1","task2","task3"};
        for(size_t k=0;k<kTaskSlots;++k)Nm(taskArena.data()+k*kTaskSlot,kTaskSlot,nt[k]);
        static const char* na[5]{"assoc0","assoc1","assoc2","assoc3","assoc4"}; static const char* nh[5]{"hier0","hier1","hier2","hier3","hier4"};
        for(size_t k=0;k<5;++k){ Nm(aAssoc[k].data(),0x20,na[k]); Nm(aHier[k].data(),0x20,nh[k]); }
    }

    // --- disposicion de campos de la DLL frente a los desplazamientos del EXE usados por los fixtures ---
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      alignas(16) static std::array<uint8,0x800> cb{};
      auto Zero=[&]{ std::memset(cb.data(),0,cb.size()); };
      auto OffOf=[&](const void* m){ return uint32(static_cast<const uint8*>(m)-cb.data()); };
      auto* V=reinterpret_cast<CVehicle*>(cb.data()); auto* P=reinterpret_cast<CPed*>(cb.data()); auto* A=reinterpret_cast<CAutomobile*>(cb.data());
      Chk("CVehicle.m_matrix",OffOf(&V->m_matrix),0x14); Chk("CVehicle.m_placement.heading",OffOf(&V->m_placement.m_fHeading),0x10);
      Chk("CVehicle.m_pHandlingData",OffOf(&V->m_pHandlingData),0x384); Chk("CVehicle.m_autoPilot.drivingStyle",OffOf(&V->m_autoPilot.m_nCarDrivingStyle),0x3B9);
      Chk("CVehicle.m_autoPilot.cruiseSpeed",OffOf(&V->m_autoPilot.m_nCruiseSpeed),0x3D0);
      Chk("CVehicle.m_pDriver",OffOf(&V->m_pDriver),0x460); Chk("CVehicle.m_apPassengers",OffOf(V->m_apPassengers.data()),0x464);
      Chk("CVehicle.m_nGettingInFlags",OffOf(&V->m_nGettingInFlags),0x486); Chk("CVehicle.m_pFire",OffOf(&V->m_pFire),0x490);
      Chk("CVehicle.m_nCreatedBy",OffOf(&V->m_nCreatedBy),0x4A4); Chk("CVehicle.m_fHealth",OffOf(&V->m_fHealth),0x4C0);
      Chk("CVehicle.m_pVehicleBeingTowed",OffOf(&V->m_pVehicleBeingTowed),0x4C8);
      Chk("CVehicle.m_nVehicleType",OffOf(&V->m_nVehicleType),0x590); Chk("CVehicle.m_nVehicleSubType",OffOf(&V->m_nVehicleSubType),0x594);
      Chk("CAutomobile.m_aCarNodes[DOOR_RF]",OffOf(&A->m_aCarNodes[CAR_DOOR_RF]),V_NODES+4*8); Chk("CAutomobile.m_aCarNodes[DOOR_LF]",OffOf(&A->m_aCarNodes[CAR_DOOR_LF]),V_NODES+4*10);
      Zero(); V->vehicleFlags.bIsBus=true; Chk("CVehicle.bIsBus",At<uint32>(cb.data(),V_FLAGS),0x200);
      Zero(); V->vehicleFlags.bLowVehicle=true; Chk("CVehicle.bLowVehicle",At<uint32>(cb.data(),V_FLAGS),0x800);
      { auto* H=reinterpret_cast<tHandlingData*>(cb.data());
        Chk("tHandlingData.m_nModelFlags",OffOf(&H->m_nModelFlags),H_FLAGS);
        Zero(); H->m_bTandemSeats=true; Chk("tHandlingData.m_bTandemSeats",At<uint32>(cb.data(),H_FLAGS),0x200);
        Zero(); H->m_bForceDoorCheck=true; Chk("tHandlingData.m_bForceDoorCheck",At<uint32>(cb.data(),H_FLAGS),0x8000); }
      Chk("CPed.m_acquaintance",OffOf(&P->m_acquaintance),P_ACQ); Chk("CPed.m_nMoveState",OffOf(&P->m_nMoveState),P_MOVE); Chk("CPed.m_pVehicle",OffOf(&P->m_pVehicle),P_VEH);
      Chk("CPed.m_nPedType",OffOf(&P->m_nPedType),P_TYPE); Chk("CPed.m_pStats",OffOf(&P->m_pStats),P_STATS);
      Zero(); P->bDontDragMeOutCar=true; Chk("CPed.bDontDragMeOutCar",At<uint32>(cb.data(),P_FLAGS),0x20000000);
      Chk("CPlayerPedData.m_fMoveBlendRatio",OffOf(&reinterpret_cast<CPlayerPedData*>(cb.data())->m_fMoveBlendRatio),0x14);
      Chk("CPedStat.m_nTemper",OffOf(&reinterpret_cast<CPedStat*>(cb.data())->m_nTemper),0x25);
      Chk("CPedIntelligence.m_TaskMgr",OffOf(&reinterpret_cast<CPedIntelligence*>(cb.data())->m_TaskMgr),4);
      Chk("CPedIntelligence.m_eventGroup",OffOf(&reinterpret_cast<CPedIntelligence*>(cb.data())->m_eventGroup),0x68);
      { auto* E=reinterpret_cast<CEventDraggedOutCar*>(cb.data());
        Chk("CEventDraggedOutCar.m_CarJacker",OffOf(&E->m_CarJacker),0x14); Chk("CEventDraggedOutCar.m_Vehicle",OffOf(&E->m_Vehicle),0x18);
        Chk("CEventDraggedOutCar.m_IsDriverSeat",OffOf(&E->m_IsDriverSeat),0x1C); Chk("sizeof(CEventDraggedOutCar)",uint32(sizeof(CEventDraggedOutCar)),0x20); }
      { auto* D=reinterpret_cast<CTaskSimpleCarSetPedInAsDriver*>(cb.data());
        Chk("CTaskSimpleCarSetPedInAsDriver.m_pTargetVehicle",OffOf(&D->m_pTargetVehicle),0x10); Chk("CTaskSimpleCarSetPedInAsDriver.m_pUtility",OffOf(&D->m_pUtility),0x14);
        Chk("CTaskSimpleCarSetPedInAsDriver.m_bWarpingInToCar",OffOf(&D->m_bWarpingInToCar),0x18); Chk("sizeof(CTaskSimpleCarSetPedInAsDriver)",uint32(sizeof(CTaskSimpleCarSetPedInAsDriver)),0x1C); }
      { auto* Q=reinterpret_cast<CTaskSimpleCarSetPedInAsPassenger*>(cb.data());
        Chk("CTaskSimpleCarSetPedInAsPassenger.m_pTargetVehicle",OffOf(&Q->m_pTargetVehicle),0x10); Chk("CTaskSimpleCarSetPedInAsPassenger.m_nTargetDoor",OffOf(&Q->m_nTargetDoor),0x14);
        Chk("CTaskSimpleCarSetPedInAsPassenger.m_pUtility",OffOf(&Q->m_pUtility),0x18); Chk("CTaskSimpleCarSetPedInAsPassenger.m_bWarpingInToCar",OffOf(&Q->m_bWarpingInToCar),0x1C);
        Chk("sizeof(CTaskSimpleCarSetPedInAsPassenger)",uint32(sizeof(CTaskSimpleCarSetPedInAsPassenger)),0x20); }
      Chk("CTaskComplexCarDrive.m_bAsDriver",OffOf(&reinterpret_cast<CTaskComplexCarDrive*>(cb.data())->m_bAsDriver),0x1C); Chk("sizeof(CTaskComplexCarDrive)",uint32(sizeof(CTaskComplexCarDrive)),0x24);
      Chk("CAnimBlendStaticAssociation.m_BlendHier",OffOf(&reinterpret_cast<CAnimBlendStaticAssociation*>(cb.data())->m_BlendHier),0x10);
      Chk("CAnimBlendHierarchy.m_pSequences",OffOf(&reinterpret_cast<CAnimBlendHierarchy*>(cb.data())->m_pSequences),4);
      Chk("CAnimBlendHierarchy.m_nSeqCount",OffOf(&reinterpret_cast<CAnimBlendHierarchy*>(cb.data())->m_nSeqCount),8);
      { auto* Sq=reinterpret_cast<CAnimBlendSequence*>(cb.data());
        Chk("CAnimBlendSequence.m_FramesNum",OffOf(&Sq->m_FramesNum),6); Chk("CAnimBlendSequence.m_Frames",OffOf(&Sq->m_Frames),8);
        Zero(); Sq->m_bHasTranslation=true; Chk("CAnimBlendSequence.m_bHasTranslation",At<uint16>(cb.data(),4),2); }
      Chk("ms_bPedOffsetsCalculated",uint32(uintptr_t(&CCarEnterExit::ms_bPedOffsetsCalculated)),0xC18C20);
      Chk("ms_vecPedGetUpAnimOffset",uint32(uintptr_t(&CCarEnterExit::ms_vecPedGetUpAnimOffset)),0xC18C3C);
      Chk("ms_vecPedQuickDraggedOutCarAnimOffset",uint32(uintptr_t(&CCarEnterExit::ms_vecPedQuickDraggedOutCarAnimOffset)),kAnimOffset0);
      Chk("ms_vecPedChairAnimOffset",uint32(uintptr_t(&CCarEnterExit::ms_vecPedChairAnimOffset)),0xC18C78);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }
    if(!layoutOk){ Write({{"event","error"},{"reason","layout mismatch between DLL members and EXE offsets"}}); return; }

    // --- colaboradores: direcciones del EXE y simbolos de la DLL. TODOS los cuerpos se calculan ANTES de crear ningun EntryPatch ---
    AddSite("get_pos",0x64E740,Addr(&CCarEnterExit::GetPositionToOpenCarDoor),reinterpret_cast<void*>(&S_GetPos));
    AddSite("is_room",0x6504C0,Addr(&CCarEnterExit::IsRoomForPedToLeaveCar),reinterpret_cast<void*>(&S_IsRoom));
    AddSite("path_blocked",0x651210,Addr(&CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel),reinterpret_cast<void*>(&S_PathBlocked));
    AddSite("active_task",0x681720,Addr(&CTaskManager::GetActiveTask),reinterpret_cast<void*>(&S_GetActive));
    AddSite("same_group",0x5F7F40,Addr(&CPedGroups::AreInSameGroup),reinterpret_cast<void*>(&S_Same));
    AddSite("upside_check",0x463830,Addr(static_cast<bool(CUpsideDownCarCheck::*)(const CVehicle*)>(&CUpsideDownCarCheck::IsCarUpsideDown)),reinterpret_cast<void*>(&S_UpsideCheck));
    AddSite("can_be_driven",0x6D5400,Addr(&CVehicle::CanBeDriven),reinterpret_cast<void*>(&S_CanDrive));
    AddSite("is_law",0x6D2370,Addr(&CVehicle::IsLawEnforcementVehicle),reinterpret_cast<void*>(&S_IsLaw));
    AddSite("friendly",0x601BC0,Addr(&CPedIntelligence::IsFriendlyWith),reinterpret_cast<void*>(&S_Friendly));
    AddSite("get_group",0x5F7E80,Addr(&CPedGroups::GetPedsGroup),reinterpret_cast<void*>(&S_GetGroup));
    AddSite("anyone_using",0x5F7DB0,Addr(&CPedGroup::IsAnyoneUsingCar),reinterpret_cast<void*>(&S_Using));
    AddSite("upside_down",0x6D1D90,Addr(&CVehicle::IsUpsideDown),reinterpret_cast<void*>(&S_UpsideDown));
    AddSite("on_side",0x6D1DD0,Addr(&CVehicle::IsOnItsSide),reinterpret_cast<void*>(&S_OnSide));
    AddSite("clear_drive",0x6509B0,Addr(&CCarEnterExit::IsClearToDriveAway),reinterpret_cast<void*>(&S_Clear));
    AddSite("event_add",0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),reinterpret_cast<void*>(&S_EvAdd));
    AddSite("reg_ref",0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),reinterpret_cast<void*>(&S_Reg));
    AddSite("clean_ref",0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),reinterpret_cast<void*>(&S_CleanRef));
    AddSite("blk_index",0x4D3990,Addr(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),reinterpret_cast<void*>(&S_BlkIndex));
    AddSite("req_model",0x4087E0,Addr(&CStreaming::RequestModel),reinterpret_cast<void*>(&S_ReqModel));
    AddSite("load_all",0x40EA10,Addr(&CStreaming::LoadAllRequestedModels),reinterpret_cast<void*>(&S_LoadAll));
    AddSite("add_ref",0x4D3FB0,Addr(&CAnimManager::AddAnimBlockRef),reinterpret_cast<void*>(&S_AddRef));
    AddSite("get_assoc",0x4D3A60,Addr(static_cast<CAnimBlendStaticAssociation*(*)(AssocGroupId,AnimationId)>(&CAnimManager::GetAnimAssociation)),reinterpret_cast<void*>(&S_GetAssoc));
    AddSite("uncompress",0x4D41C0,Addr(&CAnimManager::UncompressAnimation),reinterpret_cast<void*>(&S_Uncompress));
    AddSite("rem_ref",0x4D3FD0,Addr(&CAnimManager::RemoveAnimBlockRef),reinterpret_cast<void*>(&S_RemRef));
    AddSite("proc_driver",0x64B950,OurVirtual("CTaskSimpleCarSetPedInAsDriver","ProcessPed"),reinterpret_cast<void*>(&S_ProcDriver));
    AddSite("proc_psgr",0x64B5D0,OurVirtual("CTaskSimpleCarSetPedInAsPassenger","ProcessPed"),reinterpret_cast<void*>(&S_ProcPsgr));
    AddSite("task_new",0x61A5A0,Addr(static_cast<void*(*)(size_t)>(&CTask::operator new)),reinterpret_cast<void*>(&S_TaskNew));
    AddSite("set_task",0x681AF0,Addr(&CTaskManager::SetTask),reinterpret_cast<void*>(&S_SetTask));
    AddSite("set_acq",0x608DA0,Addr(&CAcquaintance::SetAsAcquaintance),reinterpret_cast<void*>(&S_SetAcq));
    AddSite("ped_flag",0x608830,Addr(&CPedType::GetPedFlag),reinterpret_cast<void*>(&S_PedFlag));
    bool sitesOk=true; json siteInfo=json::array(); std::set<uintptr_t> seenAt;
    for(auto& s:sites){
        const bool ok= s.exe>=0x401000&&s.exe<0x1000000 && InDll(s.sym) && InDll(s.body) && seenAt.insert(s.exe).second && seenAt.insert(s.sym).second && (s.body==s.sym||seenAt.insert(s.body).second);
        sitesOk&=ok; siteInfo.push_back({{"name",s.name},{"exe",s.exe},{"sym",s.sym},{"body",s.body},{"ok",ok}}); }
    if(!sitesOk){ Write({{"event","error"},{"reason","DLL body not found, overlapping or outside the DLL"},{"sites",siteInfo}}); return; }

    std::array<bool,5> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& s:sites)for(auto a:{s.exe,s.sym,s.body})entryBefore.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),8)});
    for(auto& fn:fns)entryBefore.push_back({fn.address,ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address),8)});
    Snap(kAnimRegion,kAnimRegionSize);
    const unsigned savedFP=_controlfp(0,0);
    Write({{"event","begin"},{"sites",siteInfo},{"fp_control",savedFP},{"functions",fns.size()}});

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4C57u; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); firstStackRef=0; taskNewCount=0; pathN=0;
                hooks[f]->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out=json::object(); unsigned fp=0; uint16 tag=0;
                {
                    Patches P;
                    switch(f){
                    case 0: P.Use({"get_pos","is_room","path_blocked","active_task","same_group"}); break;
                    case 1: P.Use({"upside_check","can_be_driven","is_law","friendly","get_group","same_group","anyone_using","upside_down","on_side","clear_drive"}); break;
                    case 2: P.Use({"event_add","reg_ref","clean_ref"}); break;
                    case 3: P.Use({"blk_index","req_model","load_all","add_ref","get_assoc","uncompress","rem_ref"}); break;
                    default: P.Use({"proc_driver","proc_psgr","task_new","set_task","set_acq","ped_flag","reg_ref","clean_ref"}); break;
                    }
                    _controlfp(_PC_24,_MCW_PC);                                           // x87 a 24 bits como el juego
                    switch(f){
                    case 0: out=CaseNearest(r,fn.address); break;
                    case 1: out=CaseSteal(r,fn.address); break;
                    case 2: out=CaseUndrag(r,fn.address); break;
                    case 3: out=CaseAnim(r,fn.address); break;
                    default: out=CaseSetPed(r,fn.address); break;
                    }
                    fp=_controlfp(0,0); tag=FpuTag(); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                }
                out["fp"]=fp; out["fpu_tag"]=tag; out["state"]=Digest(); out["trace"]=trace; out["switches"]=switches;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["fpu_tag"]==0xFFFF;
            passed&=ok; failures+=!ok;
            if(ok){ const std::string od=original.dump(), cd=candidate.dump();
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                       {"original",{{"digest",Fnv(od.data(),od.size())},{"ret",original.value("ret",json(nullptr))}}},
                       {"candidate",{{"digest",Fnv(cd.data(),cd.size())},{"ret",candidate.value("ret",json(nullptr))}}},{"events",original["trace"].size()}}); }
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=3 ? original:json(nullptr)},{"candidate",failures<=3 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions(); _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    for(auto& [a,b]:entryBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(a),8);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
