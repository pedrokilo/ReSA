#pragma once
// Banco dentro del motor: tareas complejas y repartidor (lista de 19, 36 funciones).
// CTask (IsTaskPtr, StopTimer, operator new/delete), CTaskAllocatorKillOnFoot (constructor, destructor, GetType,
// AllocateTasks), CTaskComplexCarDriveMissionFleeScene/KillPed, CTaskComplexClimb y CTaskComplexOpenDriverDoor
// (constructor, destructor, Clone, GetTaskType), CTaskComplexInAirAndLand (constructor, CreateFirstSubTask,
// CreateNextSubTask, ControlSubTask) y CTaskComplexWanderGang/Medic (constructor, Clone, GetWanderType, ScanForStuff).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// peatones, vehiculos, grupo, inteligencia de grupo, datos de jugador, ropa, pad, animaciones y tareas falsas
// (vtable trazada). Se simulan y trazan: CPedList (BuildListFromGroup_NoLeader, RemovePedsThatDontListenToPlayer,
// ExtractPedsWithGuns), CFormation (FindCoverPoints, DistributeDestinations_CoverPoints), CWeapon::IsTypeMelee,
// CPedGroupIntelligence::SetTask/SetEventResponseTask (la tarea de la pila se serializa en el momento),
// CAEPedAudioEntity::AddAudioEvent, CPedClothesDesc::GetIsWearingBalaclava, CPlayerPed::GetPadFromPlayer,
// CPad::GetPedWalkLeftRight/UpDown, CEventGroup::Add (se serializa el evento) y el RNG.
// operator new/delete se prueban sobre el pool real de tareas con slots libres: estados, m_LastFreeSlot y contenido
// del slot se guardan y se restauran en cada ejecucion. CTimer::m_snTimeInMilliseconds y el global 0xC1A554 se
// guardan y se restauran. Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del
// pool, las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/Task.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/Allocators/TaskAllocatorKillOnFoot.h"
#include "TaskComplexCarDriveMission.h"
#include "TaskComplexClimb.h"
#include "TaskComplexInAirAndLand.h"
#include "TaskComplexOpenDriverDoor.h"
#include "TaskComplexWanderGang.h"
#include "TaskComplexWanderMedic.h"
#include "PedList.h"
#include "Formation.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "PlayerPedData.h"
#include "PedClothesDesc.h"
#include "Pad.h"
#include "Weapon.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Events/EventGroup.h"
#include "Events/EventSoundQuiet.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Automobile.h"
#include "Animation/AnimBlendAssociation.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks19Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=8, NVEH=2, FAKES=4, NANIM=2;
constexpr size_t FAKE_SIZE=0x50, FK_TYPE=0x48, FK_CLONE=0x4C;   // la configuracion va detras de los campos de CTaskSimpleInAir (0x38)
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_TYPE=0x36;                                         // CEntity
constexpr size_t P_MOVE_Z=0x4C, P_FLAGS2=0x46C, P_PLAYERDATA=0x480, P_VEHICLE=0x58C, P_PEDTYPE=0x598,
                 P_WEAPONS=0x5A0, P_SLOT=0x718;                                                 // CPed
constexpr size_t V_AUTOPILOT=0x390;                                                            // CVehicle
constexpr size_t PD_CLOTHES=4, PD_BLEND=0x14;                                                  // CPlayerPedData
constexpr size_t G_MEMBERS=0xC, I_PAIRS=0xC, PAIR_SIZE=0x14;                                   // CPedGroup, CPedGroupIntelligence
constexpr size_t AN_DELTA=0x1C, AN_ID=0x2C, AN_FLAGS=0x2E;                                     // CAnimBlendAssociation
constexpr uintptr_t TIMER=0xB7CB84, FORCE_GUNS=0xC1A554, EXE_DEFAULT_ANIM_CB=0x4D1390;
constexpr uintptr_t VT_TASK=0x86D48C, VT_KOF=0x870E48, VT_FLEE=0x85B95C, VT_KILL=0x85B994, VT_CLIMB=0x859DCC,
                    VT_AIR=0x8704AC, VT_DOOR=0x86EB0C, VT_GANG=0x8700D4, VT_MEDIC=0x86F48C;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,sizeof(CPedGroupIntelligence)> intel{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,NANIM> anims{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> playerData{};
alignas(16) inline std::array<uint8,0x40> clothes{};
alignas(16) inline std::array<uint8,0x40> pad{};
alignas(16) inline std::array<uint8,0x40> event{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uintptr_t dllAnimCb{};

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
inline int32& PoolLast() { return At<int32>(PoolHdr(),0xC); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}

inline std::array<void*,12> fakeVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||(dllAnimCb&&v==dllAnimCb))return "default_anim_cb";
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NANIM;++n)if(auto s=in(anims[n].data(),anims[n].size(),"anim"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(intel.data(),intel.size(),"intel"))return *s; if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s;
    if(auto s=in(clothes.data(),clothes.size(),"clothes"))return *s; if(auto s=in(pad.data(),pad.size(),"pad"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
// entidad: hash sin el nodo de referencias (sale de la lista libre global; su direccion no es comparable)
inline std::string EntHash(const void* p,size_t n) {
    std::vector<uint8> copy(static_cast<const uint8*>(p),static_cast<const uint8*>(p)+n);
    const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(p)->m_pReferences)-uintptr_t(p); std::memset(copy.data()+off,0,4); return Hash(copy.data(),n);
}

// tarea serializada sin vptr (se compara el tipo); los bytes 0x39..0x3B de CTaskComplexSequence son relleno de la pila
inline json Word(int type,size_t off,uint32 v,int depth);
inline json SerTask(const void* p,int depth,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Word(type,k*4,w[k],depth));
    return {{"type",type},{"words",words}};
}
inline json Word(int type,size_t off,uint32 v,int depth) {
    if(type==TASK_COMPLEX_SEQUENCE&&off==0x38)return v&0xFF;
    if(depth>=0&&depth<3&&IsPoolTask(v)&&!freshIds.contains(v))return SerTask(reinterpret_cast<const void*>(uintptr_t(v)),depth+1);
    return Name(v);
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 listCount{}, removeMask{}, extractMask{}; inline std::array<int,8> listPeds{};
inline uint8 balaRet{}; inline uintptr_t padRet{}; inline int16 lrRet{}, udRet{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// CPedList / CFormation / CWeapon (AllocateTasks)
inline json ListContents(const uint8* l) { json r=json::array(); const uint32 n=At<uint32>(l,0); r.push_back(n); for(uint32 k=0;k<n&&k<30;++k)r.push_back(Name(At<uint32>(l,4+4*k))); return r; }
inline void SetList(uint8* l,const std::vector<uint32>& v) { At<uint32>(l,0)=uint32(v.size()); for(uint32 k=0;k<30;++k)At<uint32>(l,4+4*k)=k<v.size() ? v[k] : 0; }
// las listas viven en la pila (direccion no comparable): solo se traza su contenido
inline void __fastcall FakeBuildList(uint8* l,void*,void* membership) {
    trace.push_back({"build_list",Name(membership)}); std::vector<uint32> v; for(uint32 k=0;k<listCount;++k)v.push_back(uint32(uintptr_t(Ped(listPeds[k])))); SetList(l,v);
}
inline void __fastcall FakeRemoveNoListen(uint8* l,void*) {
    trace.push_back({"remove_dont_listen",ListContents(l)});
    if((removeMask&1)&&At<uint32>(l,0)){ const uint32 n=--At<uint32>(l,0); At<uint32>(l,4+4*n)=0; }
}
inline void __fastcall FakeExtract(uint8* a,void*,uint8* from) {
    trace.push_back({"extract_with_guns",ListContents(a),ListContents(from)});
    std::vector<uint32> to, keep; for(uint32 k=0;k<At<uint32>(from,0)&&k<30;++k)((extractMask>>k)&1 ? to : keep).push_back(At<uint32>(from,4+4*k));
    SetList(a,to); SetList(from,keep);
}
inline void __cdecl FakeFindCover(CVector p,float r) { trace.push_back({"find_cover_points",F(p.x),F(p.y),F(p.z),F(r)}); }
inline void __cdecl FakeDistribute(const uint8* l,CVector p) { trace.push_back({"distribute_cover",ListContents(l),F(p.x),F(p.y),F(p.z)}); }
inline bool __fastcall FakeMelee(uint8* w,void*) { const int32 t=At<int32>(w,0); trace.push_back({"is_type_melee",Name(w),t}); return t<16; }
// CPedGroupIntelligence: la tarea llega en la pila; se serializa en el momento (subtareas del pool incluidas)
inline json SerStack(const CTask* t) { return t ? SerTask(t,0,0x40) : json(nullptr); }
inline void __fastcall FakeGroupSetTask(void* in,void*,CPed* ped,const CTask* task,void* pairs,int32 slot,uint32 force) {
    trace.push_back({"group_set_task",Name(in),Name(ped),Name(pairs),slot,uint8(force),SerStack(task)});
}
inline void __fastcall FakeEventResponse(void* in,void*,CPed* ped,uint32 hasMain,const CTask* main,uint32 hasSec,const CTask* sec,int32 slot) {
    trace.push_back({"group_event_response",Name(in),Name(ped),uint8(hasMain),SerStack(main),uint8(hasSec),sec ? json(const_cast<CTask*>(sec)->GetTaskType()) : json(nullptr),slot});
}
// CTaskComplexInAirAndLand::CreateNextSubTask
inline void __fastcall FakeAudio(void* ae,void*,int32 ev,float vol,float speed,void* phys,uint32 surf,int32 a7,uint32 maxVol) {
    trace.push_back({"add_audio_event",Name(ae),ev,F(vol),F(speed),Name(phys),surf&0xFF,a7,maxVol});
}
inline bool __fastcall FakeBalaclava(void* cd,void*) { trace.push_back({"wearing_balaclava",Name(cd),balaRet}); return balaRet!=0; }
inline CPad* __fastcall FakePadFromPlayer(void* p,void*) { trace.push_back({"pad_from_player",Name(p)}); return reinterpret_cast<CPad*>(padRet); }
inline int16 __fastcall FakeLeftRight(void* p,void*) { trace.push_back({"walk_left_right",Name(p)}); return lrRet; }
inline int16 __fastcall FakeUpDown(void* p,void*) { trace.push_back({"walk_up_down",Name(p)}); return udRet; }
// el evento vive en la pila: se enmascaran los bytes de relleno (0x9..0xB, 0xD, 0x12..0x13)
inline CEvent* __fastcall FakeEventAdd(void* grp,void*,CEvent* e,uint32 valid) {
    json w=nullptr;
    if(e){ w=Region(e,sizeof(CEventSoundQuiet),true); w[1]=At<uint32>(e,8)&0xFF; w[2]=At<uint32>(e,0xC)&0xFFFF00FFu; w[3]=At<uint32>(e,0x10)&0xFFFF; }
    trace.push_back({"event_group_add",uint8(valid),e ? json(e->GetEventType()) : json(nullptr),w});
    return nullptr;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=36;
inline const std::array<Fn,NFNS> fns{{
    {"CTask","IsTaskPtr",0x61A4B0,0,-1,512},
    {"CTask","StopTimer",0,0x86D48C,5,64},
    {"CTask","operator delete",0x61A5B0,0,-1,512},
    {"CTask","operator new",0x61A5A0,0,-1,512},
    {"CTaskAllocatorKillOnFoot","AllocateTasks",0,0x870E48,1,2048},
    {"CTaskAllocatorKillOnFoot","Constructor",0x69BB80,0,-1,256},
    {"CTaskAllocatorKillOnFoot","Destructor",0x69BBF0,0,-1,256},
    {"CTaskAllocatorKillOnFoot","GetType",0,0x870E48,5,64},
    {"CTaskComplexCarDriveMissionFleeScene","Clone",0,0x85B95C,1,256},
    {"CTaskComplexCarDriveMissionFleeScene","Constructor",0x4B8920,0,-1,256},
    {"CTaskComplexCarDriveMissionFleeScene","Destructor",0x4B89F0,0,-1,256},
    {"CTaskComplexCarDriveMissionFleeScene","GetTaskType",0,0x85B95C,4,64},
    {"CTaskComplexCarDriveMissionKillPed","Clone",0,0x85B994,1,256},
    {"CTaskComplexCarDriveMissionKillPed","Constructor",0x4B8A00,0,-1,256},
    {"CTaskComplexCarDriveMissionKillPed","Destructor",0x4B8AD0,0,-1,256},
    {"CTaskComplexCarDriveMissionKillPed","GetTaskType",0,0x85B994,4,64},
    {"CTaskComplexClimb","Clone",0,0x859DCC,1,128},
    {"CTaskComplexClimb","Constructor",0x46A630,0,-1,128},
    {"CTaskComplexClimb","Destructor",0x46A6E0,0,-1,128},
    {"CTaskComplexClimb","GetTaskType",0,0x859DCC,4,64},
    {"CTaskComplexInAirAndLand","Constructor",0x678C80,0,-1,256},
    {"CTaskComplexInAirAndLand","ControlSubTask",0,0x8704AC,10,1024},
    {"CTaskComplexInAirAndLand","CreateFirstSubTask",0,0x8704AC,9,256},
    {"CTaskComplexInAirAndLand","CreateNextSubTask",0,0x8704AC,8,2048},
    {"CTaskComplexOpenDriverDoor","Clone",0,0x86EB0C,1,256},
    {"CTaskComplexOpenDriverDoor","Constructor",0x640390,0,-1,256},
    {"CTaskComplexOpenDriverDoor","Destructor",0x6403D0,0,-1,256},
    {"CTaskComplexOpenDriverDoor","GetTaskType",0,0x86EB0C,4,64},
    {"CTaskComplexWanderGang","Clone",0,0x8700D4,1,512},
    {"CTaskComplexWanderGang","Constructor",0x66F5C0,0,-1,512},
    {"CTaskComplexWanderGang","GetWanderType",0,0x8700D4,11,64},
    {"CTaskComplexWanderGang","ScanForStuff",0,0x8700D4,12,64},
    {"CTaskComplexWanderMedic","Clone",0,0x86F48C,1,256},
    {"CTaskComplexWanderMedic","Constructor",0x658770,0,-1,256},
    {"CTaskComplexWanderMedic","GetWanderType",0,0x86F48C,11,64},
    {"CTaskComplexWanderMedic","ScanForStuff",0,0x86F48C,12,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks19-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks19-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks19");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks19-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    dllAnimCb=std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB);
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{FORCE_GUNS,1}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const int32 lf0=PoolLast();
    const std::vector<uint8> statesStart(PoolStates(),PoolStates()+PoolCap());
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch l1(bit_cast<uintptr_t>(&CPedList::BuildListFromGroup_NoLeader),FakeBuildList), l1e(0x699DD0,FakeBuildList);
        EntryPatch l2(bit_cast<uintptr_t>(&CPedList::RemovePedsThatDontListenToPlayer),FakeRemoveNoListen), l2e(0x69A420,FakeRemoveNoListen);
        EntryPatch l3(bit_cast<uintptr_t>(&CPedList::ExtractPedsWithGuns),FakeExtract), l3e(0x69A4C0,FakeExtract);
        EntryPatch f1(bit_cast<uintptr_t>(&CFormation::FindCoverPoints),FakeFindCover), f1e(0x69B860,FakeFindCover);
        EntryPatch f2(bit_cast<uintptr_t>(&CFormation::DistributeDestinations_CoverPoints),FakeDistribute), f2e(0x69B5B0,FakeDistribute);
        EntryPatch w1(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),FakeMelee), w1e(0x73B1C0,FakeMelee);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroupIntelligence::SetTask),FakeGroupSetTask), g1e(0x5F7540,FakeGroupSetTask);
        EntryPatch g2(bit_cast<uintptr_t>(static_cast<void(CPedGroupIntelligence::*)(CPed*,bool,const CTask&,bool,const CTask&,eSecondaryTask)>(&CPedGroupIntelligence::SetEventResponseTask)),FakeEventResponse), g2e(0x5F8510,FakeEventResponse);
        EntryPatch a1(bit_cast<uintptr_t>(&CAEPedAudioEntity::AddAudioEvent),FakeAudio), a1e(0x4E2BB0,FakeAudio);
        EntryPatch a2(bit_cast<uintptr_t>(&CPedClothesDesc::GetIsWearingBalaclava),FakeBalaclava), a2e(0x5A7950,FakeBalaclava);
        EntryPatch a3(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),FakePadFromPlayer), a3e(0x609560,FakePadFromPlayer);
        EntryPatch a4(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight)),FakeLeftRight), a4e(0x53FC90,FakeLeftRight);
        EntryPatch a5(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown)),FakeUpDown), a5e(0x53FD30,FakeUpDown);
        EntryPatch a6(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeEventAdd), a6e(0x4AB420,FakeEventAdd);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // slots libres con 0xCD: los bytes que los constructores no escriben quedan iguales en ambas ejecuciones
                    PoolLast()=lf0;
                    // los IDs de 7 bits de los slots suben con cada New: se devuelven al estado inicial si la ocupacion no cambio
                    if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1919u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    // peatones y vehiculos
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        p[E_TYPE]=3;
                        if(next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data(); At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{0,1,4,5,7,8,6}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        v[E_TYPE]=2; At<void*>(v,E_MATRIX)=vehMats[n].data(); At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        for(size_t b=0;b<0x98;++b)v[V_AUTOPILOT+b]=uint8(next());
                        for(size_t b=0x4C0;b<0x4D0;++b)v[b]=uint8(next());
                    }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; }
                    for(auto& a:anims)for(auto& b:a)b=uint8(next());
                    for(auto& b:playerData)b=uint8(next()); At<void*>(playerData.data(),PD_CLOTHES)=clothes.data();
                    std::memset(clothes.data(),0,clothes.size()); std::memset(pad.data(),0,pad.size()); std::memset(event.data(),0,event.size());
                    std::memset(group.data(),0,group.size()); std::memset(intel.data(),0,intel.size());
                    std::memset(object.data(),0xCD,object.size());

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<CTask*> roots;
                    auto* o=object.data();
                    auto collectFresh=[&](CTask* r){
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t,-1)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto pickVeh=[&]()->CVehicle*{ return next()%5 ? Veh(0) : nullptr; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    // tareas sin efectos: GetTaskType/GetWanderType/GetType/StopTimer/ScanForStuff
                    if(nm=="GetTaskType"||nm=="GetWanderType"||nm=="GetType") {
                        setTask(fn.vt,0x60);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x60,true);
                    }
                    else if(nm=="StopTimer"||nm=="ScanForStuff") {
                        setTask(fn.vt ? (cls=="CTask" ? uintptr_t(fakeVtbl.data()) : fn.vt) : 0,0x60);
                        void* arg=nm=="StopTimer" ? (next()%2 ? (void*)event.data() : nullptr) : (next()%2 ? (void*)Ped(0) : nullptr);
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(o,arg);
                        out["object"]=Region(o,0x60,true); out["arg"]=Name(arg);
                    }
                    else if(nm=="IsTaskPtr") {
                        std::vector<int32> used, freeS; for(int32 k=0;k<PoolCap();++k)(PoolStates()[k]&0x80 ? freeS : used).push_back(k);
                        const uint32 m=next()%8; uintptr_t arg=0; const auto s=uintptr_t(PoolStorage());
                        switch(m){
                        case 0: arg=0; break;
                        case 1: case 2: arg=used.empty() ? 0 : s+used[next()%used.size()]*TASK_SLOT; break;
                        case 3: arg=used.empty() ? 0 : s+used[next()%used.size()]*TASK_SLOT+4*(1+next()%31); break; // dentro de un slot ocupado, sin alinear
                        case 4: arg=freeS.empty() ? 0 : s+freeS[next()%freeS.size()]*TASK_SLOT; break;
                        case 5: arg=uintptr_t(Fake(0)); break;
                        case 6: arg=s+size_t(PoolCap())*TASK_SLOT; break;
                        default: arg=s-TASK_SLOT; break;
                        }
                        out["arg_kind"]=m; out["arg_slot"]=arg>=s ? json(int64(arg-s)) : json(nullptr);
                        ret=reinterpret_cast<bool(__cdecl*)(uintptr_t)>(target)(arg);
                    }
                    else if(nm=="operator new"||nm=="operator delete") {
                        const int32 cap=PoolCap(); const std::vector<uint8> savedStates(PoolStates(),PoolStates()+cap);
                        std::vector<int32> freeS; for(int32 k=0;k<cap;++k)if(PoolStates()[k]&0x80)freeS.push_back(k);
                        { const uint32 m=next()%4; const int32 v=m==0 ? -1 : m==1&&!freeS.empty() ? freeS[next()%freeS.size()]-1-int32(next()%3) : int32(next()%uint32(cap))-(next()%16==0 ? 1 : 0);
                          PoolLast()=std::max(v,-1); } // el EXE lee m_SlotState[m_LastFreeSlot+1]: nunca por debajo de -1
                        int32 slot=-1;
                        if(nm=="operator new") {
                            auto* r=reinterpret_cast<uint8*(__cdecl*)(size_t)>(target)(size_t(next()%0x80));
                            for(auto* h:hooks)h->State(false);
                            slot=r ? int32((uintptr_t(r)-uintptr_t(PoolStorage()))/TASK_SLOT) : -1;
                            ret=r ? json(int64(uintptr_t(r)-uintptr_t(PoolStorage()))) : json(nullptr);
                        } else {
                            slot=freeS.empty() ? -1 : freeS[next()%freeS.size()];
                            if(slot>=0){ PoolStates()[slot]&=0x7F; for(size_t b=0;b<TASK_SLOT;++b)PoolStorage()[slot*TASK_SLOT+b]=uint8(next());
                                         reinterpret_cast<void(__cdecl*)(void*)>(target)(PoolStorage()+slot*TASK_SLOT); }
                            for(auto* h:hooks)h->State(false);
                            ret=slot;
                        }
                        out["last_free"]=PoolLast();
                        if(slot>=0){ out["state"]=PoolStates()[slot]; out["slot_hash"]=Hash(PoolStorage()+slot*TASK_SLOT,TASK_SLOT); }
                        json changed=json::array(); for(int32 k=0;k<cap;++k)if(PoolStates()[k]!=savedStates[k])changed.push_back({k,PoolStates()[k]}); out["changed_states"]=changed;
                        std::memcpy(PoolStates(),savedStates.data(),cap); PoolLast()=lf0;
                        if(slot>=0)std::memset(PoolStorage()+slot*TASK_SLOT,0xCD,TASK_SLOT);
                    }
                    else if(cls=="CTaskAllocatorKillOnFoot") {
                        if(nm=="Constructor"){
                            CPed* p=next()%4 ? Ped(6) : nullptr; static const int32 ids[]{-1,-1,0,3,7,100}; const int32 id=ids[next()%6];
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32)>(target)(o,p,id));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x10,true); out["type"]=int32(reinterpret_cast<CTaskAllocator*>(o)->GetType());
                        } else if(nm=="Destructor"){
                            At<uintptr_t>(o,0)=VT_KOF; CPed* p=next()%4 ? Ped(6) : nullptr; At<CPed*>(o,4)=p; Reg(p,o+4); At<uint32>(o,8)=next();
                            reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                            out["object"]=Region(o,0x10,true);
                        } else { // AllocateTasks
                            At<uintptr_t>(o,0)=VT_KOF; CPed* tg=next()%5 ? Ped(6) : nullptr; At<CPed*>(o,4)=tg; Reg(tg,o+4);
                            { static const int32 ids[]{-1,-1,-1,0,2,7}; At<int32>(o,8)=ids[next()%6]; }
                            if(tg){ At<int8>(tg,P_SLOT)=int8(next()%13); static const int32 wt[]{0,1,8,15,16,22,24,31,46}; At<int32>(tg,P_WEAPONS+0x1C*At<int8>(tg,P_SLOT))=wt[next()%9];
                                    if(next()%8==0)At<uint8>(reinterpret_cast<void*>(FORCE_GUNS),0)=1; }
                            // inteligencia: grupo falso (lider = miembro 7 = ped#7) y pares principales
                            At<void*>(intel.data(),0)=group.data();
                            for(int k=0;k<7;++k)At<CPed*>(group.data(),G_MEMBERS+4*k)=next()%3 ? Ped(k) : nullptr;
                            At<CPed*>(group.data(),G_MEMBERS+28)=next()%4 ? Ped(7) : nullptr;
                            { static const int32 lt[]{0,1,4,7}; At<int32>(Ped(7),P_PEDTYPE)=lt[next()%4]; }
                            static const int32 types[]{TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_THROW_CTRL,TASK_SIMPLE_GANG_DRIVEBY,TASK_COMPLEX_KILL_PED_ON_FOOT_TIMED,
                                TASK_COMPLEX_KILL_PED_ON_FOOT_STAND_STILL,TASK_UNUSED2,TASK_KILL_PED_ON_FOOT_WHILE_DUCKING,TASK_SIMPLE_STEALTH_KILL,
                                TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH,TASK_COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL,TASK_SIMPLE_ARREST_PED,TASK_COMPLEX_ARREST_PED,
                                TASK_GROUP_FOLLOW_LEADER_WITH_LIMITS,TASK_GROUP_KILL_THREATS_BASIC,TASK_GROUP_KILL_PLAYER_BASIC,TASK_SIMPLE_STAND_STILL};
                            for(int k=0;k<FAKES;++k)At<int32>(fakes[k].data(),FK_TYPE)=types[next()%16];
                            for(int k=0;k<8;++k){ auto* pp=intel.data()+I_PAIRS+PAIR_SIZE*k; At<CPed*>(pp,0)=Ped(k); At<CTask*>(pp,4)=next()%3==0 ? nullptr : Fake(k%FAKES); At<int32>(pp,8)=-1; }
                            for(int k=0;k<6;++k){ auto* p=Ped(k); At<uint32>(p,P_FLAGS2)=next()%3==0 ? (next()|0x100u) : (next()&~0x100u); At<CVehicle*>(p,P_VEHICLE)=next()%3 ? Veh(0) : nullptr; }
                            listCount=next()%7; for(auto& lp:listPeds)lp=int(next()%6); removeMask=next()%4; extractMask=next();
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(o,intel.data());
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x10,true); out["intel"]=Region(intel.data(),I_PAIRS+PAIR_SIZE*8);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexCarDriveMissionFleeScene"||cls=="CTaskComplexCarDriveMissionKillPed") {
                        const bool kill=cls=="CTaskComplexCarDriveMissionKillPed"; const uintptr_t vtbl=kill ? VT_KILL : VT_FLEE;
                        auto pickTarget=[&]()->CEntity*{ switch(next()%4){ case 0: return nullptr; case 1: return Ped(2); default: return Veh(1); } };
                        if(nm=="Constructor"){
                            std::memset(o,0xCD,0x40); CVehicle* v=pickVeh();
                            if(kill) ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,CEntity*)>(target)(o,v,pickTarget()));
                            else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x40,true); out["type"]=typeOf(o);
                        } else {
                            setTask(vtbl,0x40);
                            At<CVehicle*>(o,0xC)=pickVeh(); At<float>(o,0x10)=uni(0.f,60.f); At<int32>(o,0x14)=next()%3 ? -1 : int32(400+next()%200);
                            At<int32>(o,0x18)=int32(next()%5); o[0x1C]=uint8(next()%2); o[0x20]=uint8(next()%3==0);
                            At<CEntity*>(o,0x24)=pickTarget(); At<int32>(o,0x28)=int32(next()%60);
                            if(nm=="Destructor"){
                                At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr;
                                Reg(At<CEntity*>(o,0xC),o+0xC); Reg(At<CEntity*>(o,0x24),o+0x24);
                                reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                                out["object"]=Region(o,0x40,true);
                            } else { // Clone
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o);
                                collectFresh(r); ret=Name(r);
                            }
                        }
                        json ap=json::array(); for(int n=0;n<NVEH;++n)ap.push_back(Region(vehs[n].data()+V_AUTOPILOT,0x98)); out["autopilot"]=ap;
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexClimb") {
                        if(nm=="Constructor"){
                            std::memset(o,0xCD,0x20); ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x20,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_CLIMB,0x20); At<int32>(o,0xC)=int32(next()%3)-1; o[0x10]=uint8(next());
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; reinterpret_cast<void(__thiscall*)(void*)>(target)(o); out["object"]=Region(o,0x20,true); }
                            else { auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r); }
                        }
                    }
                    else if(cls=="CTaskComplexOpenDriverDoor") {
                        if(nm=="Constructor"){
                            std::memset(o,0xCD,0x60); ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,pickVeh()));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x60,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_DOOR,0x60); CVehicle* v=pickVeh(); At<CVehicle*>(o,0xC)=v; At<int32>(o,0x24)=int32(next()%8);
                            At<void*>(o,0x38)=nullptr; At<CPed*>(o,0x40)=nullptr; o[0x28]=uint8(next()%3); o[0x44]=uint8(next());
                            if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; Reg(v,o+0xC); reinterpret_cast<void(__thiscall*)(void*)>(target)(o); out["object"]=Region(o,0x60,true); }
                            else { auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r); }
                        }
                        out["veh"]=EntHash(vehs[0].data(),vehs[0].size()); out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexWanderGang"||cls=="CTaskComplexWanderMedic") {
                        const bool gang=cls=="CTaskComplexWanderGang";
                        static const int32 ms[]{4,6,7,1,0,-1}; const int32 moveState=ms[next()%6]; const uint32 dirArg=next()%3 ? next()%8 : next()&0xFF;
                        if(nm=="Constructor"){
                            std::memset(o,0xCD,0x40); if(next()%2)o[0x24]=uint8(next());
                            const uint32 sens=next()%2;
                            if(gang){ const uint32 scan=next()%3 ? 30000u+next()%30000u : next(); const float rad=next()%3 ? 0.5f : uni(0.f,5.f);
                                      ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,uint32,uint32,float)>(target)(o,moveState,dirArg,scan,sens,rad)); }
                            else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,uint32)>(target)(o,moveState,dirArg,sens));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x40,true); out["type"]=typeOf(o);
                        } else { // Clone
                            setTask(gang ? VT_GANG : VT_MEDIC,0x40); At<int32>(o,0xC)=moveState; o[0x10]=uint8(dirArg);
                            auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r);
                        }
                    }
                    else if(cls=="CTaskComplexInAirAndLand") {
                        auto* p=Ped(0);
                        if(nm=="Constructor"){
                            std::memset(o,0xCD,0x20); const uint32 a=next()%2, b=next()%2;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,uint32,uint32)>(target)(o,a,b));
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x20,true); out["type"]=typeOf(o);
                        } else {
                            setTask(VT_AIR,0x20); o[0xC]=uint8(next()%2); o[0xD]=uint8(next()%3==0); o[0xE]=uint8(next()%4==0);
                            // subtarea: CTaskSimpleInAir falsa (posicion 0x8, angulo 0x14, superficie 0x18, anim 0x1C, minZ 0x20, entidad 0x38)
                            auto* sub=fakes[0].data(); At<CTask*>(o,8)=Fake(0);
                            for(size_t b=8;b<0x40;++b)sub[b]=uint8(next());
                            const float pz=Ped(0)->GetPosition().z;
                            At<CVector>(sub,8)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),0.f};
                            { const uint32 m=next()%6; At<float>(sub,0x10)=m==0 ? pz+1.1f : m==1 ? qnan : m==2 ? pz+std::bit_cast<float>(std::bit_cast<uint32>(1.1f)+1) : pz+uni(-1.f,3.f); }
                            At<float>(sub,0x14)=uni(-3.2f,3.2f);
                            { const uint32 m=next()%4; At<CEntity*>(sub,0x38)=m==0 ? nullptr : m==1 ? (CEntity*)Ped(3) : (CEntity*)Veh(1); }
                            { const uint32 m=next()%3; At<void*>(sub,0x1C)=m==0 ? nullptr : (void*)anims[m-1].data(); }
                            { static const float mz[]{-0.4f,-0.41f,-0.39f,-1.f,0.f}; const uint32 m=next()%6; At<float>(sub,0x20)=m<5 ? mz[m] : qnan; }
                            for(auto& a:anims){ static const int16 ids[]{ANIM_ID_FALL_FALL,ANIM_ID_FALL_FALL,ANIM_ID_IDLE_HBHB_1,ANIM_ID_FALL_LAND,0}; At<int16>(a.data(),AN_ID)=ids[next()%5]; }
                            if(nm=="CreateFirstSubTask"){
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p); collectFresh(r); ret=Name(r);
                            } else if(nm=="ControlSubTask"){
                                { const uint32 m=next()%8; if(m==0)At<CTask*>(o,8)=nullptr; else At<int32>(sub,FK_TYPE)=m==1 ? TASK_SIMPLE_LAND : TASK_SIMPLE_IN_AIR; }
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p); collectFresh(r); ret=Name(r);
                            } else { // CreateNextSubTask
                                { static const int32 types[]{TASK_SIMPLE_GET_UP,TASK_SIMPLE_FALL,TASK_SIMPLE_LAND,TASK_SIMPLE_CLIMB,TASK_SIMPLE_STAND_STILL};
                                  const uint32 m=next()%10; At<int32>(sub,FK_TYPE)=m<5 ? types[m] : TASK_SIMPLE_IN_AIR; }
                                { static const float vz[]{-0.1f,-0.11f,-0.09f,-2.f,0.f}; const uint32 m=next()%6; At<float>(p,P_MOVE_Z)=m<5 ? vz[m] : qnan; }
                                { static const int32 pt[]{0,1,0,4,7}; At<int32>(p,P_PEDTYPE)=pt[next()%5]; }
                                const bool player=At<int32>(p,P_PEDTYPE)<2;
                                At<void*>(p,P_PLAYERDATA)=player||next()%2 ? (void*)playerData.data() : nullptr;
                                { static const float br[]{1.5f,1.51f,0.f,2.f,3.f}; const uint32 m=next()%6; At<float>(playerData.data(),PD_BLEND)=m<5 ? br[m] : qnan; }
                                balaRet=uint8(next()%2); padRet=next()%4 ? uintptr_t(pad.data()) : 0;
                                lrRet=next()%2 ? 0 : int16(int(next()%256)-128); udRet=next()%2 ? 0 : int16(int(next()%256)-128);
                                auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p); collectFresh(r); ret=Name(r);
                            }
                            out["object"]=Region(o,0x20,true); out["sub"]=Region(sub,0x40,true);
                            json an=json::array(); for(auto& a:anims)an.push_back(Region(a.data(),a.size())); out["anims"]=an;
                        }
                        out["refs"]=AllRefs();
                    }
                    for(auto* h:hooks)h->State(false);
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls;
                    out["clean"]=Occupancy()==occ;
                    restoreGlobals();
                    return out;
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
    CleanRefs(); restoreGlobals(); PoolLast()=lf0;
    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    if(restored)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
