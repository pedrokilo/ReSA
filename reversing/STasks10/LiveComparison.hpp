#pragma once
// Banco dentro del motor: tareas simples de coche (lista de 10, 50 funciones): CTaskSimpleCarCloseDoorFromOutside,
// CTaskSimpleCarGetOut, CTaskSimpleCarJumpOut, CTaskSimpleCarOpenDoorFromOutside y CTaskSimpleCarSlowDragPedOut.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE, ComputeAnimID con sus dos salidas prerrellenadas y los callbacks como
// __cdecl(anim, tarea).
// Peatones falsos A (el de la tarea) y B (conductor / sacado / FindPlayerPed) con vtable de trampas, clump con byte de
// tipo 2, matriz, banderas 0x46C/0x470/0x474, IK y tipo de peaton; vehiculo falso del tamano de CAutomobile (vtable
// falsa: hueco 27 OpenDoor, 28 ProcessOpenDoor, 29 GetDooorAngleOpenRatioU32; handling, matriz, velocidades, masa,
// tipo/subtipo, banderas, conductor/pasajeros, fuego, cierre y CDamageManager propios), evento falso (hueco 1
// GetEventType), utilidad de alineacion falsa (buffer) y dos asociaciones de animacion falsas.
// Se simulan y trazan: CAnimManager::BlendAnimation/AddAnimation, CAnimBlendAssociation::SetFinishCallback,
// CCarEnterExit::RemoveCarSitAnim/CarHasDoorToOpen/ComputePassengerIndexFromCarDoor, CVehicleAnimGroup::GetGroup,
// CTaskUtilityLineUpPedWithCar::ProcessPed, CPed::SetPedPositionInCar/ReplaceWeaponWhenExitingVehicle,
// CPedPlacement::FindZCoorForPed (EXE: CVector* en el sitio; DLL: par por valor), CUpsideDownCarCheck::IsCarUpsideDown,
// CPad::GetTarget, CTaskSimpleCarSetPedOut::ProcessPed (la DLL lo reenvia al EXE), CPlaceable::GetHeading/SetHeading,
// CGeneral::LimitRadianAngle, SayJacked/SayJacking, FindPlayerPed (sin traza, devuelve el peaton B),
// CDamageManager::GetDoorStatus_Component/SetDoorStatus_Component (con la semantica del EXE: el hook de la DLL devuelve
// un uint8 y el EXE compara eax entero) y el generador aleatorio (rand del EXE y CGeneral::GetRandomNumber).
// Se fijan y restauran CTimer::ms_fTimeStep, el truco CHEAT_SMASH_N_BOOM, CWorld::pIgnoreEntity, los mandos 0 y 1
// (CPad::Pads, solo palancas/cruceta/DisablePlayerControls) y los grupos de animacion de vehiculo (banderas especiales y
// tiempo de apertura del grupo usado). Las tareas nuevas del pool se siguen (m_LastFreeSlot restaurado y huecos libres
// con 0xCD).
// FIX_BUGS: CTaskSimpleCarOpenDoorFromOutside::ProcessPed en la DLL pone bCanExitCar en FindPlayerPed() en vez del
// conductor; el conductor de los fixtures es siempre el peaton B, que es lo que devuelve FindPlayerPed.
// Puertas fuera de 8..11 (y 18 en SlowDragPedOut) solo en ComputeAnimID (salida prerrellenada) y en los constructores:
// el resto de llamadores del EXE leen la animacion de una variable de pila sin inicializar.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h"
#include "Tasks/TaskTypes/TaskSimpleCarGetOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarJumpOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.h"
#include "Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Entity/Vehicle/Automobile.h"
#include "VehicleAnimGroupData.h"
#include "DamageManager.h"
#include "CarEnterExit.h"
#include "PedPlacement.h"
#include "UpsideDownCarCheck.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"

namespace STasks10Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE
constexpr size_t E_POS=0x4, E_MATRIX=0x14, E_CLUMP=0x18, PH_FLAGS=0x40, PH_MOVE=0x44, PH_TURN=0x50, PH_MASS=0x8C, PH_FAKE=0xB8, PH_IGNORED=0x128, // CEntity / CPhysical
                 P_FLAGS=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_SHIFTY=0x4DC, P_IKFLAGS=0x528, P_CURROT=0x558, P_AIMROT=0x55C, P_PEDTYPE=0x598, // CPed
                 V_HANDLING=0x384, V_FLAGS=0x428, V_DRIVER=0x460, V_PASS=0x464, V_FIRE=0x490, V_LOCK=0x4F8, V_TYPE=0x590, V_SUBTYPE=0x594, V_DAMAGE=0x5A0,
                 H_ANIMGROUP=0xDE, A_BLEND=0x18, A_DELTA=0x1C, A_TIME=0x20, A_ID=0x2C, A_FLAGS=0x2E,
                 G_SPECIAL=0x8, G_OPENOUT=0x20, PAD_DISABLE=0x10E;
constexpr uintptr_t ADDR_TIMESTEP=0xB7CB5C, ADDR_CHEAT_SMASH=0x969164, ADDR_IGNORE=0xB7CD68, ANIMGROUPS=0xC1CDC0, ADDR_PADS=0xB73458, EXE_DEFAULT_ANIM_CB=0x4D1390;
constexpr size_t NGROUPS=30, GROUPSIZE=0x94;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehA{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> vehMat{}, pedMat{}, pedMatB{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<uint8,0x80> clump{};
alignas(16) inline std::array<uint8,0x40> lineup{}, object{}, eventBuf{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim1{}, anim2{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CVehicle* VehA() { return reinterpret_cast<CVehicle*>(vehA.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,80> vehVtbl{};
inline std::array<void*,16> evVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline std::unordered_map<uintptr_t,std::string> cbNames;   // callbacks del EXE y de la DLL

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=cbNames.find(v);it!=cbNames.end())return it->second;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    if(v>=ANIMGROUPS&&v<ANIMGROUPS+GROUPSIZE*NGROUPS)return "animgroup+"+std::to_string(v-ANIMGROUPS);
    if(v>=ADDR_PADS&&v<ADDR_PADS+2*sizeof(CPad))return "pads+"+std::to_string(v-ADDR_PADS);
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(vehA.data(),vehA.size(),"vehA"))return *s;
    if(auto s=in(vehMat.data(),vehMat.size(),"vehmat"))return *s; if(auto s=in(pedMat.data(),pedMat.size(),"pedmat"))return *s;
    if(auto s=in(pedMatB.data(),pedMatB.size(),"pedmatB"))return *s; if(auto s=in(handling.data(),handling.size(),"handling"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(lineup.data(),lineup.size(),"lineup"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(eventBuf.data(),eventBuf.size(),"event"))return *s;
    if(auto s=in(anim1.data(),anim1.size(),"anim1"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(vehVtbl.data(),sizeof(vehVtbl),"vehvtbl"))return *s;
    if(auto s=in(evVtbl.data(),sizeof(evVtbl),"evvtbl"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline uint32 rngState{}, eventType{}, sayMask{}; inline int32 groupRet{}, passIdx{};
inline bool lineupRet{}, upsideRet{}, hasDoorRet{}, targetRet{}, setPedOutRet{}, zFoundOk{};
inline float zFound{}, ratioRet{}, headingRet{};

// vtables falsas: todo trazado
template<int N> void* __fastcall PTrap(void* p,void*) { trace.push_back({"ped_unexpected_slot",N,Name(uintptr_t(p))}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
template<int N> void* __fastcall VTrap(void* v,void*) { trace.push_back({"veh_unexpected_slot",N,Name(uintptr_t(v))}); return nullptr; }
template<int... N> void FillVTraps(std::integer_sequence<int,N...>) { ((vehVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
template<int N> void* __fastcall ETrap(void* v,void*) { trace.push_back({"event_unexpected_slot",N,Name(uintptr_t(v))}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((evVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
inline void __fastcall OpenDoorV(CVehicle* v,void*,CPed* p,int32 comp,int32 door,float r,uint32 snd) { trace.push_back({"open_door",Name(uintptr_t(v)),Name(uintptr_t(p)),comp,door,F(r),snd&0xFF}); }
inline void __fastcall ProcessOpenDoorV(CVehicle* v,void*,CPed* p,uint32 door,uint32 grp,uint32 id,float t) { trace.push_back({"process_open_door",Name(uintptr_t(v)),Name(uintptr_t(p)),door,grp,id,F(t)}); }
inline float __fastcall DoorRatioV(CVehicle* v,void*,uint32 door) { trace.push_back({"door_angle_open_ratio",Name(uintptr_t(v)),door,F(ratioRet)}); return ratioRet; }
inline int32 __fastcall EventTypeV(void* e,void*) { trace.push_back({"get_event_type",Name(uintptr_t(e)),eventType}); return int32(eventType); }

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 grp,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),grp,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(anim2.data()); }
inline CAnimBlendAssociation* __cdecl AddAnim(RpClump* c,int32 grp,int32 id) { trace.push_back({"add_animation",Name(uintptr_t(c)),grp,id}); return reinterpret_cast<CAnimBlendAssociation*>(anim2.data()); }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __cdecl RemoveSitAnim(const CPed* p) { trace.push_back({"remove_car_sit_anim",Name(uintptr_t(p))}); }
inline bool __cdecl HasDoorToOpen(const CVehicle* v,int32 door) { trace.push_back({"car_has_door_to_open",Name(uintptr_t(v)),door,hasDoorRet}); return hasDoorRet; }
inline int32 __cdecl PassengerIndex(const CVehicle* v,int32 door) { trace.push_back({"passenger_index_from_door",Name(uintptr_t(v)),door,passIdx}); return passIdx; }
inline int32 __fastcall GetGroup(const void* g,void*,int32 id) { trace.push_back({"anim_group_get_group",Name(uintptr_t(g)),id,groupRet}); return groupRet; }
inline bool __fastcall LineUp(void* u,void*,CPed* p,CVehicle* v,CAnimBlendAssociation* a) { trace.push_back({"line_up_process_ped",Name(uintptr_t(u)),Name(uintptr_t(p)),Name(uintptr_t(v)),Name(uintptr_t(a))}); return lineupRet; }
inline void __fastcall PosInCar(CPed* p,void*) { trace.push_back({"set_ped_position_in_car",Name(uintptr_t(p))}); }
inline void __fastcall ReplaceWeapon(CPed* p,void*) { trace.push_back({"replace_weapon_when_exiting_vehicle",Name(uintptr_t(p))}); }
inline void FindZTrace(const CVector& pos) { trace.push_back({"find_z_coor_for_ped",F(pos.x),F(pos.y),F(pos.z),Name(*reinterpret_cast<uintptr_t*>(ADDR_IGNORE)),zFoundOk,F(zFound)}); }
inline bool __cdecl FindZExe(CVector* pos) { FindZTrace(*pos); if(zFoundOk)pos->z=zFound; return zFoundOk; }
inline std::pair<CVector,bool> __cdecl FindZDll(CVector pos) { FindZTrace(pos); if(zFoundOk)pos.z=zFound; return {pos,zFoundOk}; }
inline bool __fastcall UpsideDown(void* self,void*,const CVehicle* v) { trace.push_back({"is_car_upside_down",Name(uintptr_t(self)),Name(uintptr_t(v)),upsideRet}); return upsideRet; }
inline bool __fastcall GetTarget(const void* pad,void*) { trace.push_back({"pad_get_target",Name(uintptr_t(pad)),targetRet}); return targetRet; }
inline bool __fastcall SetPedOutProcess(void* t,void*,CPed* p) {
    trace.push_back({"set_ped_out_process_ped",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(At(t,8)),At(t,0xC),At<uint8>(t,0x10),At<uint8>(t,0x11),At<uint8>(t,0x12),At<uint8>(t,0x13),At<uint8>(t,0x14),At<uint8>(t,0x15)});
    return setPedOutRet;
}
inline float __fastcall GetHeading(const void* p,void*) { trace.push_back({"get_heading",Name(uintptr_t(p)),F(headingRet)}); return headingRet; }
inline void __fastcall SetHeading(void* p,void*,float h) { trace.push_back({"set_heading",Name(uintptr_t(p)),F(h)}); }
inline float __cdecl LimitAngle(float a) { const float r=a-6.2831855f; trace.push_back({"limit_radian_angle",F(a),F(r)}); return r; }
inline bool __cdecl SayJackedF(CPed* jacked,CVehicle* v,uint32 off) { const bool r=sayMask&1; trace.push_back({"say_jacked",Name(uintptr_t(jacked)),Name(uintptr_t(v)),off,r}); return r; }
inline bool __cdecl SayJackingF(CPed* jacker,CPed* jacked,CVehicle* v,uint32 off) { const bool r=(sayMask>>1)&1; trace.push_back({"say_jacking",Name(uintptr_t(jacker)),Name(uintptr_t(jacked)),Name(uintptr_t(v)),off,r}); return r; }
inline CPlayerPed* __cdecl FindPlayer(int32) { return reinterpret_cast<CPlayerPed*>(pedB.data()); } // sin traza (ver FIX_BUGS en la cabecera)
// CDamageManager con la semantica del EXE (0x6C2250 / 0x6C21E0): puertas 8..11 en los bytes +0xB..+0xE
inline uint32 __fastcall GetDoorStatus(const void* dm,void*,int32 comp) {
    const uint32 r=uint32(comp-8)<=3 ? uint32(static_cast<const uint8*>(dm)[0xB+(comp-8)]) : 4u;
    trace.push_back({"get_door_status_component",Name(uintptr_t(dm)),comp,r}); return r;
}
inline void __fastcall SetDoorStatus(void* dm,void*,int32 comp,uint32 status) {
    trace.push_back({"set_door_status_component",Name(uintptr_t(dm)),comp,status&0xFF});
    if(uint32(comp-8)<=3)static_cast<uint8*>(dm)[0xB+(comp-8)]=uint8(status);
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; uintptr_t p=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

enum Kind { CLOSEDOOR, GETOUT, JUMPOUT, OPENDOOR, SLOWDRAG };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; const char* cb; uintptr_t cbAddr; size_t vehOff, doorOff, lineOff; };
inline const std::array<Cls,5> classes{{
    {"CTaskSimpleCarCloseDoorFromOutside",0x86ED2C,0x1C,CLOSEDOOR,"FinishAnimCarCloseDoorFromOutsideCB",0x646680,0x10,0x14,0x18},
    {"CTaskSimpleCarGetOut",0x86EE4C,0x20,GETOUT,"FinishAnimCarGetOutCB",0x647480,0x14,0x18,0x1C},
    {"CTaskSimpleCarJumpOut",0x86EE70,0x24,JUMPOUT,"FinishAnimCarJumpOutCB",0x647700,0x10,0x18,0x20},
    {"CTaskSimpleCarOpenDoorFromOutside",0x86EC9C,0x24,OPENDOOR,"FinishAnimCarOpenDoorFromOutsideCB",0x646020,0x10,0x14,0x1C},
    {"CTaskSimpleCarSlowDragPedOut",0x86EEE8,0x20,SLOWDRAG,"FinishAnimCarSlowDragPedOutCB",0x648180,0x10,0x14,0x18},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=50;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleCarCloseDoorFromOutside","Clone",0,0x86ED2C,1,128},
    {"CTaskSimpleCarCloseDoorFromOutside","ComputeAnimID",0x646600,0x86ED2C,-1,256},
    {"CTaskSimpleCarCloseDoorFromOutside","Constructor",0x6464F0,0x86ED2C,-1,256},
    {"CTaskSimpleCarCloseDoorFromOutside","Destructor",0x646570,0x86ED2C,-1,128},
    {"CTaskSimpleCarCloseDoorFromOutside","FinishAnimCarCloseDoorFromOutsideCB",0x646680,0x86ED2C,-1,128},
    {"CTaskSimpleCarCloseDoorFromOutside","GetTaskType",0,0x86ED2C,4,64},
    {"CTaskSimpleCarCloseDoorFromOutside","MakeAbortable",0,0x86ED2C,6,256},
    {"CTaskSimpleCarCloseDoorFromOutside","ProcessPed",0,0x86ED2C,7,1024},
    {"CTaskSimpleCarCloseDoorFromOutside","SetPedPosition",0,0x86ED2C,8,128},
    {"CTaskSimpleCarCloseDoorFromOutside","StartAnim",0x64B080,0x86ED2C,-1,1024},
    {"CTaskSimpleCarGetOut","Clone",0,0x86EE4C,1,128},
    {"CTaskSimpleCarGetOut","ComputeAnimID_ToHook",0x647330,0x86EE4C,-1,512},
    {"CTaskSimpleCarGetOut","Constructor",0x6471D0,0x86EE4C,-1,256},
    {"CTaskSimpleCarGetOut","Destructor",0x647260,0x86EE4C,-1,128},
    {"CTaskSimpleCarGetOut","FinishAnimCarGetOutCB",0x647480,0x86EE4C,-1,128},
    {"CTaskSimpleCarGetOut","GetTaskType",0,0x86EE4C,4,64},
    {"CTaskSimpleCarGetOut","MakeAbortable",0,0x86EE4C,6,256},
    {"CTaskSimpleCarGetOut","ProcessPed",0,0x86EE4C,7,1024},
    {"CTaskSimpleCarGetOut","SetPedPosition",0,0x86EE4C,8,256},
    {"CTaskSimpleCarGetOut","StartAnim",0x64BDE0,0x86EE4C,-1,256},
    {"CTaskSimpleCarJumpOut","Clone",0,0x86EE70,1,128},
    {"CTaskSimpleCarJumpOut","ComputeAnimID_ToHook",0x647690,0x86EE70,-1,256},
    {"CTaskSimpleCarJumpOut","Constructor",0x64BE40,0x86EE70,-1,256},
    {"CTaskSimpleCarJumpOut","Destructor",0x647490,0x86EE70,-1,128},
    {"CTaskSimpleCarJumpOut","FinishAnimCarJumpOutCB",0x647700,0x86EE70,-1,128},
    {"CTaskSimpleCarJumpOut","GetTaskType",0,0x86EE70,4,64},
    {"CTaskSimpleCarJumpOut","MakeAbortable",0,0x86EE70,6,512},
    {"CTaskSimpleCarJumpOut","ProcessPed",0,0x86EE70,7,2048},
    {"CTaskSimpleCarJumpOut","SetPedPosition",0,0x86EE70,8,1024},
    {"CTaskSimpleCarJumpOut","StartAnim",0x64BF00,0x86EE70,-1,256},
    {"CTaskSimpleCarOpenDoorFromOutside","Clone",0,0x86EC9C,1,128},
    {"CTaskSimpleCarOpenDoorFromOutside","ComputeAnimID_ToHook",0x645FA0,0x86EC9C,-1,256},
    {"CTaskSimpleCarOpenDoorFromOutside","Constructor",0x645E50,0x86EC9C,-1,256},
    {"CTaskSimpleCarOpenDoorFromOutside","Destructor",0x645EE0,0x86EC9C,-1,256},
    {"CTaskSimpleCarOpenDoorFromOutside","FinishAnimCarOpenDoorFromOutsideCB",0x646020,0x86EC9C,-1,256},
    {"CTaskSimpleCarOpenDoorFromOutside","GetTaskType",0,0x86EC9C,4,64},
    {"CTaskSimpleCarOpenDoorFromOutside","MakeAbortable",0,0x86EC9C,6,512},
    {"CTaskSimpleCarOpenDoorFromOutside","ProcessPed",0,0x86EC9C,7,2048},
    {"CTaskSimpleCarOpenDoorFromOutside","SetPedPosition",0,0x86EC9C,8,128},
    {"CTaskSimpleCarOpenDoorFromOutside","StartAnim",0x64AD90,0x86EC9C,-1,256},
    {"CTaskSimpleCarSlowDragPedOut","Clone",0,0x86EEE8,1,128},
    {"CTaskSimpleCarSlowDragPedOut","ComputeAnimID_Wrapper",0x648100,0x86EEE8,-1,256},
    {"CTaskSimpleCarSlowDragPedOut","Constructor",0x647FE0,0x86EEE8,-1,256},
    {"CTaskSimpleCarSlowDragPedOut","Destructor",0x648070,0x86EEE8,-1,128},
    {"CTaskSimpleCarSlowDragPedOut","FinishAnimCarSlowDragPedOutCB",0x648180,0x86EEE8,-1,256},
    {"CTaskSimpleCarSlowDragPedOut","GetTaskType",0,0x86EEE8,4,64},
    {"CTaskSimpleCarSlowDragPedOut","MakeAbortable",0,0x86EEE8,6,256},
    {"CTaskSimpleCarSlowDragPedOut","ProcessPed",0,0x86EEE8,7,2048},
    {"CTaskSimpleCarSlowDragPedOut","SetPedPosition",0,0x86EEE8,8,128},
    {"CTaskSimpleCarSlowDragPedOut","StartAnim",0x64C010,0x86EEE8,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(VehA());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks10-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks10-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks10");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks10-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // nombres de los callbacks: direccion del EXE, funcion de la DLL (y su destino si es un thunk)
    cbNames.clear();
    cbNames[EXE_DEFAULT_ANIM_CB]="DefaultAnimCB"; cbNames[std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB)]="DefaultAnimCB";
    cbNames[Follow(std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))]="DefaultAnimCB";
    for(auto& c:classes){
        auto* h=FindHook(c.cls,c.cb);
        if(!h||h->Symbol()[0]!='S'){Write({{"event","error"},{"reason","callback hook not found"},{"function",std::string(c.cls)+"::"+c.cb}});return;}
        const auto our=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
        cbNames[c.cbAddr]=c.cb; cbNames[our]=c.cb; cbNames[Follow(our)]=c.cb;
    }
    FillPTraps(std::make_integer_sequence<int,64>{});
    FillVTraps(std::make_integer_sequence<int,80>{});
    FillETraps(std::make_integer_sequence<int,16>{});
    vehVtbl[27]=reinterpret_cast<void*>(&OpenDoorV); vehVtbl[28]=reinterpret_cast<void*>(&ProcessOpenDoorV); vehVtbl[29]=reinterpret_cast<void*>(&DoorRatioV);
    evVtbl[1]=reinterpret_cast<void*>(&EventTypeV);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIMESTEP); const float savedStep=timeStep;
    auto& cheat=*reinterpret_cast<uint8*>(ADDR_CHEAT_SMASH); const uint8 savedCheat=cheat;
    auto& ignore=*reinterpret_cast<uintptr_t*>(ADDR_IGNORE); const uintptr_t savedIgnore=ignore;
    auto* const pads=reinterpret_cast<uint8*>(ADDR_PADS); std::vector<uint8> savedPads(pads,pads+2*sizeof(CPad));
    auto* const groups=reinterpret_cast<uint8*>(ANIMGROUPS); std::vector<uint8> savedGroups(groups,groups+GROUPSIZE*NGROUPS);
    auto restoreGlobals=[&]{ timeStep=savedStep; cheat=savedCheat; ignore=savedIgnore; std::memcpy(pads,savedPads.data(),savedPads.size()); std::memcpy(groups,savedGroups.data(),savedGroups.size()); };
    auto globalsIntact=[&]{ return timeStep==savedStep&&cheat==savedCheat&&ignore==savedIgnore&&!std::memcmp(pads,savedPads.data(),savedPads.size())&&!std::memcmp(groups,savedGroups.data(),savedGroups.size()); };
    auto& lfRef=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); const int32 lf0=lfRef;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation)),AddAnim), a2e(0x4D3AA0,AddAnim);
        EntryPatch a3(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a3e(0x4CEBE0,SetFinishCb);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarEnterExit::RemoveCarSitAnim),RemoveSitAnim), c1e(0x64F680,RemoveSitAnim);
        EntryPatch c2(bit_cast<uintptr_t>(static_cast<AssocGroupId(CVehicleAnimGroup::*)(AnimationId) const>(&CVehicleAnimGroup::GetGroup)),GetGroup), c2e(0x6E3B00,GetGroup);
        EntryPatch c3(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::ProcessPed),LineUp), c3e(0x6513A0,LineUp);
        EntryPatch c4(bit_cast<uintptr_t>(&CCarEnterExit::CarHasDoorToOpen),HasDoorToOpen), c4e(0x64EDD0,HasDoorToOpen);
        EntryPatch c5(bit_cast<uintptr_t>(&CCarEnterExit::ComputePassengerIndexFromCarDoor),PassengerIndex), c5e(0x64F1E0,PassengerIndex);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetPedPositionInCar),PosInCar), p1e(0x5DF910,PosInCar);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::ReplaceWeaponWhenExitingVehicle),ReplaceWeapon), p2e(0x5E6490,ReplaceWeapon);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<std::pair<CVector,bool>(*)(CVector)>(&CPedPlacement::FindZCoorForPed)),FindZDll), p3e(0x616920,FindZExe);
        EntryPatch p4(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), p4e(0x56E210,FindPlayer);
        EntryPatch p5(bit_cast<uintptr_t>(static_cast<bool(CUpsideDownCarCheck::*)(const CVehicle*)>(&CUpsideDownCarCheck::IsCarUpsideDown)),UpsideDown), p5e(0x463830,UpsideDown);
        EntryPatch p6(bit_cast<uintptr_t>(&CPad::GetTarget),GetTarget), p6e(0x540670,GetTarget);
        EntryPatch p7(OurVirtual("CTaskSimpleCarSetPedOut","ProcessPed"),SetPedOutProcess), p7e(0x647D10,SetPedOutProcess); // la version de la DLL es un reenvio al EXE (no reconstruida)
        EntryPatch p8(bit_cast<uintptr_t>(&CPlaceable::GetHeading),GetHeading), p8e(0x441DB0,GetHeading);
        EntryPatch p9(bit_cast<uintptr_t>(&CPlaceable::SetHeading),SetHeading), p9e(0x43E0C0,SetHeading);
        EntryPatch g1(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),LimitAngle), g1e(0x53CB50,LimitAngle);
        EntryPatch s1(bit_cast<uintptr_t>(&SayJacked),SayJackedF), s1e(0x6497A0,SayJackedF);
        EntryPatch s2(bit_cast<uintptr_t>(&SayJacking),SayJackingF), s2e(0x6497F0,SayJackingF);
        EntryPatch d1(bit_cast<uintptr_t>(&CDamageManager::GetDoorStatus_Component),GetDoorStatus), d1e(0x6C2250,GetDoorStatus);
        EntryPatch d2(bit_cast<uintptr_t>(&CDamageManager::SetDoorStatus_Component),SetDoorStatus), d2e(0x6C21E0,SetDoorStatus);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; const auto& cl=ClassOf(fn.cls); int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto around=[&](float t){ switch(next()%4){ case 0: return t; case 1: return std::nextafter(t,100.f); case 2: return std::nextafter(t,-100.f); default: return uni(0.f,2.f*t); } };
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { lfRef=lf0; auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA10Au; if(!st)st=1; trace=json::array(); freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    rngState=next();
                    timeStep=next()%3 ? 1.f : uni(0.1f,3.f); cheat=uint8(next()%2); ignore=0;
                    // mandos 0 y 1: palancas, cruceta y DisablePlayerControls (desplazamientos del EXE)
                    for(size_t n=0;n<2;++n){ auto* pd=pads+n*sizeof(CPad);
                        for(size_t off:{size_t(0),size_t(2),size_t(0x10),size_t(0x12),size_t(0x14),size_t(0x16)})At<int16>(pd,off)=next()%5==0 ? int16(next()) : int16(0);
                        At<uint16>(pd,PAD_DISABLE)=uint16(next()%3==0 ? 1+next()%3 : 0); }
                    // peatones
                    auto makePed=[&](std::array<uint8,sizeof(CPed)>& buf,std::array<uint8,sizeof(CMatrixLink)>& mat,uint32 pedType){
                        std::memset(buf.data(),0,buf.size()); std::memset(mat.data(),0,mat.size());
                        CPed* const p=reinterpret_cast<CPed*>(buf.data()); p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,E_CLUMP)=clump.data();
                        At(p,P_FLAGS)=(next()&~0x100u)|(next()%2 ? 0x100u : 0u); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_IKFLAGS)=next();
                        At(p,P_PEDTYPE)=pedType;
                        CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        std::memcpy(mat.data(),&m,sizeof(CMatrix)); At<void*>(p,E_MATRIX)=mat.data();
                        At<float>(p,PH_MOVE)=uni(-1.f,1.f); At<float>(p,PH_MOVE+4)=uni(-1.f,1.f); At<float>(p,PH_MOVE+8)=uni(-1.f,1.f);
                        At<float>(p,P_SHIFTY)=uni(-1.f,1.f); At<float>(p,P_CURROT)=uni(-3.f,3.f); At<float>(p,P_AIMROT)=uni(-3.f,3.f);
                        return p;
                    };
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    static const uint32 typesA[]{0,1,4,6,7,6}; static const uint32 typesB[]{0,1,4,6};
                    CPed* const pd=makePed(pedA,pedMat,typesA[next()%6]);
                    CPed* const pb=makePed(pedB,pedMatB,typesB[next()%4]);
                    // vehiculo
                    std::memset(vehA.data(),0,vehA.size()); std::memset(vehMat.data(),0,vehMat.size());
                    CVehicle* const vh=VehA(); vh->SetType(ENTITY_TYPE_VEHICLE); *reinterpret_cast<void**>(vh)=vehVtbl.data();
                    for(auto& b:handling)b=uint8(next()); { static const uint8 g[]{15,16,24,0,3,7}; handling[H_ANIMGROUP]=next()%2 ? g[next()%6] : uint8(next()%NGROUPS); }
                    At<void*>(vh,V_HANDLING)=handling.data();
                    { CMatrix m{}; const float rx=next()%4==0 ? uni(2.f,4.f) : uni(-0.6f,0.6f); m.SetRotate(rx,uni(-1.2f,1.2f),uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                      std::memcpy(vehMat.data(),&m,sizeof(CMatrix));
                      if(next()%6==0){ static const float rz[]{-0.5f,0.5f,0.f,-0.f}; At<float>(vehMat.data(),8)=rz[next()%4]; }      // right.z en los umbrales
                      if(next()%6==0)At<float>(vehMat.data(),0x28)=next()%2 ? 0.f : -0.f;                                             // up.z = 0
                      At<void*>(vh,E_MATRIX)=vehMat.data(); }
                    for(size_t k=0;k<3;++k){ At<float>(vh,PH_MOVE+4*k)=uni(-1.f,1.f); At<float>(vh,PH_TURN+4*k)=uni(-0.1f,0.1f); }
                    At<float>(vh,PH_MASS)=uni(100.f,3000.f); At(vh,PH_FLAGS)=next(); At(vh,V_FLAGS)=next(); At(vh,V_FLAGS+4)=next(); At<uint8>(vh,PH_FAKE)=uint8(next());
                    { static const uint32 types[]{0,0,0,9,4,2,5}; At(vh,V_TYPE)=types[next()%7]; static const uint32 sub[]{0,2,4,9,10,3}; At(vh,V_SUBTYPE)=sub[next()%6]; }
                    At<void*>(vh,V_DRIVER)=next()%3 ? static_cast<void*>(pb) : nullptr;
                    for(size_t k=0;k<8;++k){ const uint32 r=next()%4; At<void*>(vh,V_PASS+4*k)=r==0 ? static_cast<void*>(pb) : r==1 ? static_cast<void*>(pd) : nullptr; }
                    At<uintptr_t>(vh,V_FIRE)=next()%5==0 ? 0x1234u : 0u; At(vh,V_LOCK)=next()%4==0 ? 6u : next()%8;
                    for(size_t k=0;k<0x18;++k)vehA[V_DAMAGE+k]=uint8(next()); for(size_t k=0;k<4;++k)vehA[V_DAMAGE+0xB+k]=uint8(next()%5);
                    { static const uint8 eng[]{0,224,225,250,255}; vehA[V_DAMAGE+4]=next()%2 ? eng[next()%5] : uint8(next()); }
                    // grupo de animacion usado: banderas especiales y tiempo de apertura
                    auto* const grp=groups+GROUPSIZE*handling[H_ANIMGROUP];
                    At(grp,G_SPECIAL)=next(); const float openOut=next()%2 ? 0.2f : uni(0.1f,0.9f); At<float>(grp,G_OPENOUT)=openOut;
                    // utilidad de alineacion, evento y asociaciones (floats controlados, sin NaN de senalizacion)
                    for(auto& b:lineup)b=uint8(next());
                    std::memset(eventBuf.data(),0,eventBuf.size()); At<void*>(eventBuf.data(),0)=evVtbl.data();
                    { static const uint32 evs[]{33,66,33,66,1,0}; eventType=evs[next()%6]; }
                    for(auto* a:{&anim1,&anim2}){ for(auto& b:*a)b=uint8(next());
                        const float blends[]{0.f,1.f,-0.f,std::nextafter(1.f,0.f),std::nextafter(0.f,1.f)};
                        At<float>(a->data(),A_BLEND)=next()%2 ? uni(0.f,1.f) : blends[next()%5];
                        At<float>(a->data(),A_DELTA)=uni(-4.f,4.f);
                        { const float thr[]{std::bit_cast<float>(0x3F3BBBBCu),0.45f,0.35f,0.15f,0.07f,openOut}; At<float>(a->data(),A_TIME)=around(thr[next()%6]); }
                        { static const int16 ids[]{0x180,0x181,0x163,0x164,0x165,0x166,0x16C,0x16D,0x16E,0x175,0x176,0x17C,0x17D,0x6C,0x6D,0};
                          At<int16>(a->data(),A_ID)=next()%6 ? ids[next()%16] : int16(next()%500); }
                        At<uint16>(a->data(),A_FLAGS)=uint16(next()); }
                    { static const int32 grps[]{107,107,88,89,90,0}; groupRet=grps[next()%6]; }
                    lineupRet=next()%2; upsideRet=next()%2; hasDoorRet=next()%2; targetRet=next()%4==0; setPedOutRet=next()%2; sayMask=next()&3; passIdx=int32(next()%8);
                    ratioRet=uni(0.f,1.f); headingRet=uni(-3.2f,3.2f);
                    { const float pz=At<float>(pedMat.data(),0x38); const float opts[]{pz-1.f,std::nextafter(pz-1.f,1000.f),std::nextafter(pz-1.f,-1000.f),pz,std::nextafter(pz,1000.f),pz+uni(0.f,1.f),pz-uni(0.f,3.f)};
                      zFoundOk=next()%5!=0; zFound=opts[next()%7]; }
                    const int prio=next()%8==0 ? 3 : int(next()%3);
                    // tarea probada
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor", isCB=nm.ends_with("CB"), isCompute=nm.starts_with("ComputeAnimID");
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    At<uint8>(o,8)=uint8(next()%3==0);
                    At<void*>(o,0xC)=next()%3 ? static_cast<void*>(anim2.data()) : nullptr;
                    // vehiculo: StartAnim, ComputeAnimID, el callback de OpenDoorFromOutside y SetPedPosition de JumpOut lo leen sin comprobar
                    const bool needVeh=nm=="StartAnim"||isCompute||(isCB&&cl.kind==OPENDOOR)||(nm=="SetPedPosition"&&cl.kind==JUMPOUT);
                    At<void*>(o,cl.vehOff)=(needVeh||next()%6) ? static_cast<void*>(vh) : nullptr;
                    // puertas: fuera de 8..11 (18) solo donde la salida no viene de la pila sin inicializar
                    auto pickDoor=[&](bool wide)->int32{
                        if(wide&&next()%3==0){ static const int32 d[]{0,7,12,17,18,19,40,-1}; return next()%2 ? d[next()%8] : int32(next()%40); }
                        if(cl.kind==SLOWDRAG){ static const int32 d[]{8,9,10,11,18,8}; return d[next()%6]; }
                        static const int32 d[]{8,9,10,11}; return d[next()%4];
                    };
                    At<int32>(o,cl.doorOff)=pickDoor(isCompute||nm=="GetTaskType");
                    At<void*>(o,cl.lineOff)=lineup.data();
                    switch(cl.kind){
                    case GETOUT: At<uint8>(o,9)=uint8(next()%2); At<uint8>(o,0x10)=uint8(next()%2); break;
                    case JUMPOUT: At<float>(o,0x14)=uni(0.f,2.f); break;
                    case OPENDOOR: At<uint8>(o,0x18)=uint8(next()%2); At<uint8>(o,0x19)=uint8(next()%2); At<float>(o,0x20)=uni(0.f,1.f); break;
                    case SLOWDRAG: At<uint8>(o,0x1C)=uint8(next()%2); break;
                    default: break;
                    }
                    if(isDtor)Reg(At<void*>(o,cl.vehOff),static_cast<uint8*>(o)+cl.vehOff);
                    CVehicle* const ctorVeh=next()%6 ? vh : nullptr; const int32 ctorDoor=pickDoor(true); const uint32 ctorFlag=next()%2;
                    void* const cbAnim=next()%2 ? static_cast<void*>(anim2.data()) : static_cast<void*>(anim1.data());
                    void* const ev=next()%4 ? static_cast<void*>(eventBuf.data()) : nullptr;
                    int32 outGrp=int32(0x5A5A5A5A), outId=int32(next()%2 ? next()%500 : 0x7777);
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        if(cl.kind==OPENDOOR||cl.kind==SLOWDRAG)rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,void*,uint32)>(target)(o,ctorVeh,ctorDoor,lineup.data(),ctorFlag));
                        else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,void*)>(target)(o,ctorVeh,ctorDoor,lineup.data())); }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(isCompute){ reinterpret_cast<void(__thiscall*)(void*,int32*,int32*)>(target)(o,&outGrp,&outId); ret=json::array({outGrp,outId}); }
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev)!=0;
                    else if(nm=="ProcessPed"||nm=="SetPedPosition")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*)>(target)(o,pd)!=0;
                    else if(nm=="StartAnim")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(isCB)reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(cbAnim,o);
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&nm!="StartAnim"&&!isCB)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(pd),static_cast<CEntity*>(pb),static_cast<CEntity*>(vh)})refs.push_back(refList(e));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"vehA",EntHash(vehA)},
                            {"ped_fields",{At(pd,P_FLAGS),At(pd,P_FLAGS2),At(pd,P_FLAGS3),At(pd,P_IKFLAGS),At(pd,P_SHIFTY),At(pd,P_CURROT),At(pd,P_AIMROT),Name(At(pd,PH_IGNORED)),
                                           At(pd,PH_MOVE),At(pd,PH_MOVE+4),At(pd,PH_MOVE+8),At(pb,P_FLAGS3)}},
                            {"veh_fields",{At(vh,PH_FLAGS),At(vh,V_FLAGS),At(vh,V_FLAGS+4),At<uint8>(vh,PH_FAKE),At(vh,PH_MOVE),At(vh,PH_MOVE+4),At(vh,PH_MOVE+8),
                                           At(vh,PH_TURN),At(vh,PH_TURN+4),At(vh,PH_TURN+8),Region(vehA.data()+V_DAMAGE,0x18)}},
                            {"anims",{Region(anim1.data(),anim1.size()),Region(anim2.data(),anim2.size())}},{"lineup",Region(lineup.data(),lineup.size())},
                            {"vehmat",Hash(vehMat.data(),vehMat.size())},{"pedmat",Hash(pedMat.data(),pedMat.size())},{"pedmatB",Hash(pedMatB.data(),pedMatB.size())},
                            {"handling",Hash(handling.data(),handling.size())},{"clump",Hash(clump.data(),clump.size())},{"event",Hash(eventBuf.data(),eventBuf.size())},
                            {"pads",Hash(pads,2*sizeof(CPad))},{"groups",Hash(groups,GROUPSIZE*NGROUPS)},
                            {"timestep",F(timeStep)},{"cheat",cheat},{"ignore",Name(ignore)}};
                    restoreGlobals();
                    // borrado de las tareas creadas (su traza va aparte)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    freshIds.clear();
                    CleanRefs();
                    return r;
                };
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
    CleanRefs();
    const bool globalsTouched=!globalsIntact();
    restoreGlobals(); lfRef=lf0;
    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},{"state_restored",restored}});
}
}
