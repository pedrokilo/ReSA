#pragma once
// Banco dentro del motor: eventos, escaner de entidades, almacen de tareas de cerebro y referencias (lista de 23,
// 32 funciones de 8 clases). CEventAcquaintancePed, CEventAttractor, CEventBuildingCollision, CEventDanger y
// CEventStuckInAir (constructores llamados en la direccion del EXE con `this` = buffer propio rellenado con 0xCD y
// comparado entero con el tipo de evento en vez del vptr; los metodos virtuales por la entrada de la vtable del EXE
// con el evento construido a mano), CEntityScanner (constructor, destructor, Clear y ScanForEntitiesInRange),
// CScriptedBrainTaskStore (SetTask, GetTask y los dos Clear) y CReferences (Init, ListSize,
// PruneAllReferencesInWorld y RemoveReferencesToPlayer).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: peatones, vehiculo, objeto y
// edificios falsos con matrices propias, inteligencias con tareas falsas (vtable propia: destructor trazado,
// GetSubTask/GetTaskType/IsSimple), eventos falsos (tipo, prioridad y entidad origen), un CPedGroup falso, un modelo y
// un modelo de colision falsos colocados temporalmente en ms_modelInfoPtrs y efectos 2D falsos.
// Estado global que se toca y se restaura en cada ejecucion: CReferences::aRefs + pEmptyList (copia entera), el pool
// de eventos (cabecera, estados y almacenamiento), CPedGroups::ms_groups (los 8 grupos), el almacen de cerebros
// 0xC188F0, los sectores repetidos 0xB992B8 (ScanForEntitiesInRange recorre listas falsas), CWorld::ms_nCurrentScanCode,
// los punteros de los pools de peatones/vehiculos/objetos (PruneAllReferencesInWorld recorre pools FALSOS) y la
// entrada de ms_modelInfoPtrs. Nunca se toca una entidad real.
// Se simulan y trazan: FindPlayerPed, FindPlayerVehicle, FindPlayerWanted, CPedGroups::AreInSameGroup,
// CPedIntelligence::FindTaskByType, CBaseModelInfo::Get2dEffect, CWeather::IsRainy, CPopulation::
// PedMICanBeCreatedAtThisAttractor, IKChainManager_c::IsLooking/LookAt, rand (EXE 0x821B1E y CGeneral::GetRandomNumber),
// CPedGroupMembership::SetLeader y CPedGroup::Process.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventAcquaintancePed.h"
#include "Events/EventAttractor.h"
#include "Events/EventBuildingCollision.h"
#include "Events/EventDanger.h"
#include "Events/EventStuckInAir.h"
#include "EntityScanner.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "References.h"
#include "Ragdoll/IKChainManager.h"
#include "Weather.h"
#include "Population.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedIntelligence.h"
#include "Models/BaseModelInfo.h"
#include "Pools/Pools.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace Events23Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_FLAGS=0x1C, E_MODEL=0x22, E_REFS=0x24, E_SCAN=0x2C, PH_ATTACHED=0xFC,
                 P_FLAGS1=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_STATE=0x530, P_VEHICLE=0x58C, P_TYPE=0x598, P_FIRE=0x730, V_DRIVER=0x460;
constexpr size_t I_PRIMARY=0x04, I_HIST_NONTEMP=0x3C, I_HIST_TEMP=0x40, I_EVCOUNT=0x70, I_HEARING=0xBC, I_SEEING=0xC0, I_STUCK=0x272;
constexpr size_t G_PED=0x00, G_MEMBERS=0x0C, GROUP_SIZE=0x2D4, PD_GROUP=0x38;
constexpr size_t FK_TYPE=0x38, FK_SUB=0x3C;                                  // tarea falsa
constexpr size_t EV_SRC=0x30, EV_TYPE=0x38, EV_PRIO=0x3C, EV_SIZE=0x44;      // evento falso
constexpr uintptr_t ADDR_MI_PTRS=0xA9B0C8, ADDR_GROUPS=0xC09920, ADDR_STORE=0xC188F0, ADDR_SECTORS=0xB992B8, ADDR_SCAN_CODE=0xB7CD78,
                    ADDR_PED_POOL=0xB74490, ADDR_VEH_POOL=0xB74494, ADDR_OBJ_POOL=0xB7449C, IK_MAN=0xC15448, EVENT_SLOT=0x44, SCANNER_VT=0x86CD40;
constexpr size_t STORE_SIZE=0x300, SECTORS_SIZE=256*12, NGROUPS=8;
constexpr int16 MI_FAKE=1;
constexpr int FAKES=12, NFX=4, NENT=24, NNODE=1024, NREF=64, NHOLD=32;
constexpr uint32 QNAN=0x7FC00000u;
constexpr int PED_CAP=4, VEH_CAP=3, OBJ_CAP=5; constexpr size_t PED_STRIDE=0x7C4, VEH_STRIDE=0xA18, OBJ_STRIDE=0x19C;

alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> vehA{};
alignas(16) inline std::array<uint8,sizeof(CObject)> objA{};
alignas(16) inline std::array<uint8,0x40> bldA{}, bldB{}, mi{}, col{}, fire{}, wanted{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,0x100> pdata{};
alignas(16) inline std::array<uint8,0x48> matA{}, matB{}, matC{}, matV{}, matO{};
alignas(16) inline std::array<uint8,EV_SIZE> evA{}, evB{};
alignas(16) inline std::array<uint8,0x60> object{};
alignas(16) inline std::array<std::array<uint8,0x40>,NFX> fx{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NENT> ents{};
alignas(16) inline std::array<std::array<uint8,0x48>,NENT> entMats{};
alignas(16) inline std::array<uint32,NNODE*3> nodes{};
alignas(16) inline std::array<uint32,NREF*2> refNodes{};
alignas(16) inline std::array<uint32,NHOLD> holders{};
alignas(16) inline std::array<uint8,0x14> pedHdr{}, vehHdr{}, objHdr{};
alignas(16) inline std::array<uint8,PED_CAP*PED_STRIDE> pedStore{};
alignas(16) inline std::array<uint8,VEH_CAP*VEH_STRIDE> vehStore{};
alignas(16) inline std::array<uint8,OBJ_CAP*OBJ_STRIDE> objStore{};
alignas(16) inline std::array<uint8,8> pedStates{}, vehStates{}, objStates{};
inline CPed* Ped(std::array<uint8,sizeof(CPed)>& b) { return reinterpret_cast<CPed*>(b.data()); }
inline CPed* PedA() { return Ped(pedA); } inline CPed* PedB() { return Ped(pedB); } inline CPed* PedC() { return Ped(pedC); }
inline CEntity* Ent(int k) { return reinterpret_cast<CEntity*>(ents[k].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pool de eventos (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot, banderas)
inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }
inline std::vector<bool> EvOccupancy() { std::vector<bool> r(EvCap()); for(size_t k=0;k<r.size();++k)r[k]=!(EvStates()[k]&0x80); return r; }

// nombres estables para punteros a los fixtures y a las zonas globales
struct Buf { uintptr_t p; size_t n; std::string nm; size_t stride; };
inline std::vector<Buf> bufs;
inline void RegisterBuffers() {
    bufs.clear();
    auto add=[](const void* p,size_t n,std::string nm,size_t stride=0){ bufs.push_back({uintptr_t(p),n,std::move(nm),stride}); };
    add(pedA.data(),pedA.size(),"pedA"); add(pedB.data(),pedB.size(),"pedB"); add(pedC.data(),pedC.size(),"pedC");
    add(vehA.data(),vehA.size(),"vehA"); add(objA.data(),objA.size(),"objA"); add(bldA.data(),bldA.size(),"bldA"); add(bldB.data(),bldB.size(),"bldB");
    add(mi.data(),mi.size(),"mi"); add(col.data(),col.size(),"col"); add(fire.data(),fire.size(),"fire"); add(wanted.data(),wanted.size(),"wanted");
    add(intelA.data(),intelA.size(),"intelA"); add(intelB.data(),intelB.size(),"intelB"); add(group.data(),group.size(),"group"); add(pdata.data(),pdata.size(),"pdata");
    add(matA.data(),matA.size(),"matA"); add(matB.data(),matB.size(),"matB"); add(matC.data(),matC.size(),"matC"); add(matV.data(),matV.size(),"matV"); add(matO.data(),matO.size(),"matO");
    add(evA.data(),evA.size(),"evA"); add(evB.data(),evB.size(),"evB"); add(object.data(),object.size(),"object");
    add(fx.data(),sizeof(fx),"fx",sizeof(fx[0])); add(fakes.data(),sizeof(fakes),"fk",sizeof(fakes[0])); add(ents.data(),sizeof(ents),"ent",sizeof(ents[0]));
    add(entMats.data(),sizeof(entMats),"entMat",sizeof(entMats[0])); add(nodes.data(),sizeof(nodes),"node",12); add(refNodes.data(),sizeof(refNodes),"rn",8);
    add(holders.data(),sizeof(holders),"hold",4); add(pedHdr.data(),pedHdr.size(),"pedHdr"); add(vehHdr.data(),vehHdr.size(),"vehHdr"); add(objHdr.data(),objHdr.size(),"objHdr");
    add(pedStore.data(),pedStore.size(),"pedPool",PED_STRIDE); add(vehStore.data(),vehStore.size(),"vehPool",VEH_STRIDE); add(objStore.data(),objStore.size(),"objPool",OBJ_STRIDE);
    add(&CReferences::aRefs[0],sizeof(CReferences::aRefs),"ref",8); add(reinterpret_cast<void*>(ADDR_GROUPS),NGROUPS*GROUP_SIZE,"groups",GROUP_SIZE);
    add(reinterpret_cast<void*>(ADDR_STORE),STORE_SIZE,"store",0x10); add(reinterpret_cast<void*>(ADDR_SECTORS),SECTORS_SIZE,"sectors",12);
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    for(auto& b:bufs)if(v>=b.p&&v<b.p+b.n){ const size_t off=v-b.p; return b.stride ? b.nm+"#"+std::to_string(off/b.stride)+"+"+std::to_string(off%b.stride) : b.nm+"+"+std::to_string(off); }
    { const auto es=uintptr_t(EvStorage()); if(v>=es&&v<es+EvCap()*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline json RefList(const void* e) { json l=json::array(); int n=0; for(auto* r=*reinterpret_cast<CReference* const*>(static_cast<const uint8*>(e)+E_REFS);r&&n<64;r=r->m_pNext,++n)l.push_back({Name(r),Name(uintptr_t(r->m_ppEntity))}); return l; }
inline json FreeHead() { json l=json::array(); int n=0; for(auto* r=CReferences::pEmptyList;r&&n<8;r=r->m_pNext,++n)l.push_back({Name(r),Name(uintptr_t(r->m_ppEntity))}); return l; }

// configuracion por caso de las dependencias simuladas
inline uintptr_t playerRet{}, vehicleRet{}, taskByTypeRet{}; inline bool sameGroup{}, rainy{}, looking{}, pedMiOk{}; inline uint32 rngState{};
inline std::array<uintptr_t,8> fxMap{};

// tareas falsas: lectores puros (no se trazan); el destructor se apunta en un array fijo
struct Del { uintptr_t at; uint32 flags; };
inline std::array<Del,64> dels{}; inline size_t ndel{};
inline void* __fastcall FDtor(void* t,void*,uint32 flags) { if(ndel<dels.size())dels[ndel++]={uintptr_t(t),flags}; return t; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask* t,void*) { return At<CTask*>(t,FK_SUB)==nullptr; }
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(t)}); return nullptr; }
inline std::array<void*,16> taskVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// eventos falsos: GetEventType (1), GetEventPriority (2) y GetSourceEntity (10)
inline int __fastcall EType(void* e,void*) { return At<int32>(e,EV_TYPE); }
inline int __fastcall EPrio(void* e,void*) { return At<int32>(e,EV_PRIO); }
inline CEntity* __fastcall ESource(void* e,void*) { return At<CEntity*>(e,EV_SRC); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
inline std::array<void*,20> eventVtbl{}, acqVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline CVehicle* __cdecl FindVehicle(int32 id,uint32 remote) { trace.push_back({"find_player_vehicle",id,remote&0xFF}); return reinterpret_cast<CVehicle*>(vehicleRet); }
inline CWanted* __cdecl FindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline bool __cdecl SameGroup(const CPed* a,const CPed* b) { trace.push_back({"are_in_same_group",Name(a),Name(b)}); return sameGroup; }
inline CTask* __fastcall TaskByType(void* intel,void*,int32 type) { trace.push_back({"find_task_by_type",Name(intel),type}); return reinterpret_cast<CTask*>(taskByTypeRet); }
inline void* __fastcall Get2dFx(void* m,void*,int32 idx) { trace.push_back({"get_2d_effect",Name(m),idx}); return reinterpret_cast<void*>(fxMap[uint32(idx)&7]); }
inline bool __cdecl IsRainy() { trace.push_back({"is_rainy"}); return rainy; }
inline bool __cdecl PedMiOk(int32 model,const char* name) { trace.push_back({"ped_mi_can_be_created_at_attractor",model,Name(name)}); return pedMiOk; }
inline bool __fastcall IsLooking(void* self,void*,CPed* p) { trace.push_back({"ik_is_looking",uintptr_t(self)==IK_MAN,Name(p)}); return looking; }
inline void __fastcall LookAt(void* self,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    trace.push_back({"ik_look_at",uintptr_t(self)==IK_MAN,purpose ? json(std::string(purpose)) : json(nullptr),Name(p),Name(e),time,bone,
                     off ? json{F(off->x),F(off->y),F(off->z)} : json(nullptr),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline int __cdecl FakeRand() { trace.push_back({"rand"}); rngState=rngState*1103515245u+12345u; return int((rngState>>16)&0x7FFF); }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline void __fastcall SetLeader(void* m,void*,CPed* p) { trace.push_back({"set_leader",Name(m),Name(p)}); }
inline void __fastcall GroupProcess(void* g,void*) { trace.push_back({"group_process",Name(g)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Cls { ACQ, ATTR, BUILD, DANGER, STUCK, SCAN, BRAIN, REFS };
inline Cls ClassOf(const std::string& c) {
    return c=="CEventAcquaintancePed" ? ACQ : c=="CEventAttractor" ? ATTR : c=="CEventBuildingCollision" ? BUILD : c=="CEventDanger" ? DANGER
         : c=="CEventStuckInAir" ? STUCK : c=="CEntityScanner" ? SCAN : c=="CScriptedBrainTaskStore" ? BRAIN : REFS;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=32;
inline const std::array<Fn,NFNS> fns{{
    {"CEventAcquaintancePed","Constructor",0x4AF820,0x85B250,-1,128},
    {"CEventAcquaintancePed","AffectsPed",0,0x85B250,5,512},
    {"CEventAcquaintancePed","AffectsPedGroup",0,0x85B250,6,512},
    {"CEventAcquaintancePed","TakesPriorityOver",0,0x85B250,11,256},
    {"CEventAttractor","Constructor",0x4AF350,0x85B1B8,-1,128},
    {"CEventAttractor","AffectsPed",0,0x85B1B8,5,1024},
    {"CEventAttractor","CloneEditable",0,0x85B1B8,16,256},
    {"CEventAttractor","IsEffectActive",0x4AF460,0,-1,512},
    {"CEventBuildingCollision","Constructor",0x4ACF00,0x85ACE8,-1,128},
    {"CEventBuildingCollision","AffectsPed",0,0x85ACE8,5,1024},
    {"CEventBuildingCollision","CanTreatBuildingAsObject",0x4B3120,0,-1,512},
    {"CEventBuildingCollision","IsHeadOnCollision",0x4AD1E0,0,-1,512},
    {"CEventDanger","Constructor",0x4B2600,0x85B848,-1,128},
    {"CEventDanger","AffectsPed",0,0x85B848,5,512},
    {"CEventDanger","AffectsPedGroup",0,0x85B848,6,512},
    {"CEventDanger","GetSourceEntity",0,0x85B848,10,256},
    {"CEventStuckInAir","Constructor",0x4B1490,0x85B508,-1,64},
    {"CEventStuckInAir","AffectsPed",0,0x85B508,5,256},
    {"CEventStuckInAir","GetEventPriority",0,0x85B508,2,256},
    {"CEventStuckInAir","TakesPriorityOver",0,0x85B508,11,256},
    {"CEntityScanner","Constructor",0x5FF990,0,-1,64},
    {"CEntityScanner","Destructor",0x603480,0,-1,128},
    {"CEntityScanner","Clear",0x5FF9D0,0,-1,256},
    {"CEntityScanner","ScanForEntitiesInRange",0,0x86CD40,0,1024},
    {"CScriptedBrainTaskStore","SetTask",0x635720,0,-1,512},
    {"CScriptedBrainTaskStore","GetTask",0x6357C0,0,-1,256},
    {"CScriptedBrainTaskStore","Clear-ped",0x635850,0,-1,256},
    {"CScriptedBrainTaskStore","Clear-task",0x6357F0,0,-1,256},
    {"CReferences","Init",0x5719B0,0,-1,64},
    {"CReferences","ListSize",0x5719E0,0,-1,256},
    {"CReferences","PruneAllReferencesInWorld",0x571CC0,0,-1,256},
    {"CReferences","RemoveReferencesToPlayer",0x571AD0,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"events23-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"events23-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("events23");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"events23-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    RegisterBuffers();
    FillTraps(std::make_integer_sequence<int,16>{}); taskVtbl[0]=reinterpret_cast<void*>(&FDtor); taskVtbl[2]=reinterpret_cast<void*>(&FSub);
    taskVtbl[3]=reinterpret_cast<void*>(&FSimple); taskVtbl[4]=reinterpret_cast<void*>(&FType);
    FillETraps(std::make_integer_sequence<int,20>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    eventVtbl[10]=reinterpret_cast<void*>(&ESource);
    // CEventAcquaintancePed es abstracta: vtable del EXE copiada con GetEventType/GetEventPriority falsos
    for(int k=0;k<17;++k)acqVtbl[k]=reinterpret_cast<void**>(0x85B250)[k];
    acqVtbl[1]=reinterpret_cast<void*>(&EType); acqVtbl[2]=reinterpret_cast<void*>(&EPrio);

    // estado global tocado: se guarda y se restaura en cada ejecucion
    const std::vector<uint8> savedRefs(reinterpret_cast<uint8*>(&CReferences::aRefs[0]),reinterpret_cast<uint8*>(&CReferences::aRefs[0])+sizeof(CReferences::aRefs));
    CReference* const savedEmpty=CReferences::pEmptyList;
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto* const groupsMem=reinterpret_cast<uint8*>(ADDR_GROUPS); const std::vector<uint8> savedGroups(groupsMem,groupsMem+NGROUPS*GROUP_SIZE);
    auto* const storeMem=reinterpret_cast<uint8*>(ADDR_STORE); const std::vector<uint8> savedStore(storeMem,storeMem+STORE_SIZE);
    auto* const sectorsMem=reinterpret_cast<uint8*>(ADDR_SECTORS); const std::vector<uint8> savedSectors(sectorsMem,sectorsMem+SECTORS_SIZE);
    auto& scanCode=*reinterpret_cast<uint16*>(ADDR_SCAN_CODE); const uint16 savedScan=scanCode;
    auto& pedPool=*reinterpret_cast<void**>(ADDR_PED_POOL); auto& vehPool=*reinterpret_cast<void**>(ADDR_VEH_POOL); auto& objPool=*reinterpret_cast<void**>(ADDR_OBJ_POOL);
    void* const savedPedPool=pedPool; void* const savedVehPool=vehPool; void* const savedObjPool=objPool;
    auto& miSlot=reinterpret_cast<void**>(ADDR_MI_PTRS)[MI_FAKE]; void* const savedMi=miSlot;
    const auto occupancyStart=EvOccupancy();
    std::vector<uint32> activeGroups; for(uint32 g=0;g<NGROUPS;++g)if(CPedGroups::ms_activeGroups[g])activeGroups.push_back(g);
    if(activeGroups.empty()){Write({{"event","error"},{"reason","no active ped group"}});return;}
    auto restoreAll=[&]{
        std::memcpy(&CReferences::aRefs[0],savedRefs.data(),savedRefs.size()); CReferences::pEmptyList=savedEmpty;
        std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size());
        std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size());
        std::memcpy(groupsMem,savedGroups.data(),savedGroups.size()); std::memcpy(storeMem,savedStore.data(),savedStore.size());
        std::memcpy(sectorsMem,savedSectors.data(),savedSectors.size()); scanCode=savedScan;
        pedPool=savedPedPool; vehPool=savedVehPool; objPool=savedObjPool; miSlot=savedMi;
    };
    auto stateRestored=[&]{
        return std::memcmp(&CReferences::aRefs[0],savedRefs.data(),savedRefs.size())==0&&CReferences::pEmptyList==savedEmpty&&EvOccupancy()==occupancyStart
            &&std::memcmp(EvHdr(),savedEvHdr.data(),savedEvHdr.size())==0&&std::memcmp(groupsMem,savedGroups.data(),savedGroups.size())==0
            &&std::memcmp(storeMem,savedStore.data(),savedStore.size())==0&&std::memcmp(sectorsMem,savedSectors.data(),savedSectors.size())==0
            &&scanCode==savedScan&&pedPool==savedPedPool&&vehPool==savedVehPool&&objPool==savedObjPool&&miSlot==savedMi;
    };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), s1e(0x56E210,FindPlayer);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FindVehicle), s2e(0x56E0D0,FindVehicle);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FindWanted), s3e(0x56E230,FindWanted);
        EntryPatch s4e(0x5F7F40,SameGroup);   // la DLL solo envuelve la del EXE
        EntryPatch s5(bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),TaskByType), s5e(0x600EE0,TaskByType);
        EntryPatch s6(bit_cast<uintptr_t>(&CBaseModelInfo::Get2dEffect),Get2dFx), s6e(0x4C4C70,Get2dFx);
        EntryPatch s7(bit_cast<uintptr_t>(&CWeather::IsRainy),IsRainy), s7e(0x4ABF50,IsRainy);
        EntryPatch s8(bit_cast<uintptr_t>(&CPopulation::PedMICanBeCreatedAtThisAttractor),PedMiOk), s8e(0x6110E0,PedMiOk);
        EntryPatch s9(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),IsLooking), s9e(0x6181A0,IsLooking);
        EntryPatch s10(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), s10e(0x618970,LookAt);
        EntryPatch s11(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), s11e(0x821B1E,FakeRand);
        EntryPatch s12(bit_cast<uintptr_t>(&CPedGroupMembership::SetLeader),SetLeader), s12e(0x5FB9C0,SetLeader);
        EntryPatch s13(bit_cast<uintptr_t>(&CPedGroup::Process),GroupProcess), s13e(0x5FC7E0,GroupProcess);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm; const Cls cls=ClassOf(fn.cls);
            if(!only.empty()&&only!=full)continue;
            const bool isCtor=nm=="Constructor";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l){return *(l.begin()+next()%l.size());};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2323u; if(!st)st=1; trace=json::array(); ndel=0;
                    restoreAll();
                    // pool de eventos: los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto)
                    for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    const auto occ=EvOccupancy();
                    const uintptr_t A=uintptr_t(PedA()), B=uintptr_t(PedB()), C=uintptr_t(PedC()), VA=uintptr_t(vehA.data()), OA=uintptr_t(objA.data()),
                                    BA=uintptr_t(bldA.data()), BB=uintptr_t(bldB.data());
                    const bool needMat=(cls==BUILD&&(nm=="AffectsPed"||nm=="IsHeadOnCollision"))||cls==ATTR;   // el EXE lee la matriz sin comprobar null
                    // matrices: vectores normales aleatorios y posicion en +0x30
                    auto fillMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,0x48); for(int k=0;k<12;++k)if(k%4!=3)At<float>(m,4*k)=uni(-1.f,1.f); At<CVector>(m,0x30)=pos; };
                    const CVector center=cls==SCAN ? CVector{uni(-2990.f,2990.f),uni(-2990.f,2990.f),uni(-50.f,200.f)} : vec(20.f);
                    // peatones
                    std::array<std::array<uint8,0x48>*,3> pm{&matA,&matB,&matC}; int pk=0;
                    for(auto* buf:{&pedA,&pedB,&pedC}){ auto* p=buf->data(); std::memset(p,0,buf->size()); reinterpret_cast<CEntity*>(p)->SetType(ENTITY_TYPE_PED);
                        const CVector pos=center+CVector{uni(-15.f,15.f),uni(-15.f,15.f),uni(-2.f,2.f)}; At<CVector>(p,E_POS)=pos; fillMat(pm[pk]->data(),pos+vec(next()%3 ? 0.f : 0.5f));
                        At<void*>(p,E_MATRIX)=(next()%4||(pk==0&&needMat)) ? pm[pk]->data() : nullptr; At<int16>(p,E_MODEL)=int16(next()%300);
                        At(p,P_TYPE)=uint32(pick({0,1,4,6,6,20,7,8})); At(p,P_STATE)=uint32(pick({1,1,1,0x36,0x37,0,7,0x32}));
                        At(p,P_FLAGS1)=next(); At<uintptr_t>(p,P_VEHICLE)=pick({0,0,VA}); At<void*>(p,P_FIRE)=next()%5 ? nullptr : fire.data();
                        At<uintptr_t>(p,PH_ATTACHED)=next()%6 ? 0 : pick({VA,OA}); At<void*>(p,P_PDATA)=pdata.data(); ++pk; }
                    At<void*>(pedA.data(),P_INTEL)=intelA.data(); At<void*>(pedB.data(),P_INTEL)=intelB.data(); At<void*>(pedC.data(),P_INTEL)=intelB.data();
                    // vehiculo, objeto y edificios (modelo falso en el objeto y en los edificios)
                    std::memset(vehA.data(),0,vehA.size()); reinterpret_cast<CEntity*>(vehA.data())->SetType(ENTITY_TYPE_VEHICLE);
                    { const CVector pos=center+vec(15.f); At<CVector>(vehA.data(),E_POS)=pos; fillMat(matV.data(),pos); At<void*>(vehA.data(),E_MATRIX)=next()%3||needMat ? matV.data() : nullptr;
                      At<uintptr_t>(vehA.data(),V_DRIVER)=pick({0,A,B,C}); At<int16>(vehA.data(),E_MODEL)=MI_FAKE; }
                    std::memset(objA.data(),0,objA.size()); reinterpret_cast<CEntity*>(objA.data())->SetType(ENTITY_TYPE_OBJECT);
                    { const CVector pos=center+vec(15.f); At<CVector>(objA.data(),E_POS)=pos; fillMat(matO.data(),pos); At<void*>(objA.data(),E_MATRIX)=next()%3||needMat ? matO.data() : nullptr;
                      At<int16>(objA.data(),E_MODEL)=MI_FAKE; }
                    for(auto* buf:{&bldA,&bldB}){ auto* b=buf->data(); std::memset(b,0,buf->size()); reinterpret_cast<CEntity*>(b)->SetType(ENTITY_TYPE_BUILDING);
                        At<CVector>(b,E_POS)=center+vec(15.f); At(b,E_FLAGS)=(next()&~0x400000u)|(next()%3==0 ? 0x400000u : 0u); At<int16>(b,E_MODEL)=MI_FAKE; }
                    for(auto& b:fire)b=uint8(next());
                    // modelo falso: numero de efectos 2D en +0xD y modelo de colision en +0x14 (caja: minimo en +0, maximo en +0xC)
                    for(auto& b:mi)b=uint8(next()); At<void*>(mi.data(),0x14)=col.data(); At<uint8>(mi.data(),0xD)=uint8(pick({0,1,2,3,5,8,1}));
                    std::memset(col.data(),0,col.size());
                    { static const float lims[]{2.f,6.f,1.9999999f,2.0000002f,5.9999995f,6.0000005f,0.f};
                      for(int k=0;k<3;++k){ const float lo=uni(-8.f,8.f); const float len=next()%3 ? uni(0.f,9.f) : lims[next()%std::size(lims)]*(next()%5 ? 1.f : -1.f);
                          At<float>(col.data(),4*k)=lo; At<float>(col.data(),0xC+4*k)=next()%14==0 ? std::bit_cast<float>(QNAN) : lo+len; } }
                    // efectos 2D falsos: posicion en +0, tipo en +0xC, tipo de atractor en +0x34, probabilidades en +0x35/+0x36, guion en +0x38
                    for(auto& e:fx){ for(auto& b:e)b=uint8(next()); At<CVector>(e.data(),0)=vec(3.f); At<uint8>(e.data(),0xC)=uint8(pick({3,3,3,1}));
                        At<uint8>(e.data(),0x34)=uint8(pick({4,5,5,0,1,2,4,9})); At<uint8>(e.data(),0x35)=uint8(pick({0,50,100,255,next()%256}));
                        At<uint8>(e.data(),0x36)=uint8(pick({0,50,100,255,next()%256})); for(int k=0;k<7;++k)At<char>(e.data(),0x38+k)=char('a'+next()%26); At<char>(e.data(),0x3F)=0; }
                    for(auto& m:fxMap)m=next()%5 ? uintptr_t(fx[next()%NFX].data()) : uintptr_t(fakes[0].data());
                    // tareas falsas
                    static const uint32 taskTypes[]{0xE9,0x10B,0x4BF,0xCB,0x190,0x2C0,0xE9,0x10B,0x384,0x3E8};
                    for(int n=0;n<FAKES;++n){ auto* t=fakes[n].data(); for(auto& b:fakes[n])b=uint8(next()); At<void*>(t,0)=taskVtbl.data(); At<void*>(t,4)=nullptr;
                        At<uint32>(t,FK_TYPE)=taskTypes[next()%std::size(taskTypes)]; At<CTask*>(t,FK_SUB)=n<FAKES-1&&next()%3==0 ? Fake(FAKES-1) : nullptr; }
                    // eventos falsos (historial y "otro evento" de TakesPriorityOver)
                    for(auto* buf:{&evA,&evB}){ auto* e=buf->data(); for(auto& b:*buf)b=uint8(next()); At<void*>(e,0)=eventVtbl.data();
                        At<uint32>(e,EV_TYPE)=uint32(cls==BUILD ? pick({6,6,60,60,31,13}) : cls==STUCK ? pick({93,93,13,6,31}) : next()%100);
                        At<int32>(e,EV_PRIO)=int32(pick({73,74,75,0,63,80,next()%100})); At<uintptr_t>(e,EV_SRC)=pick({A,B,0});
                        At<uintptr_t>(e,0x14)=cls==BUILD ? pick({BA,BB,BA,0}) : pick({A,B,C,0}); }
                    // inteligencias: tareas primarias (+4), historial de eventos, contador de eventos en cola, rangos y atasco
                    for(auto* buf:{&intelA,&intelB}){ std::memset(buf->data(),0,buf->size()); auto* in=buf->data();
                        for(int k=0;k<5;++k)At<CTask*>(in,I_PRIMARY+4*k)=next()%3==0 ? Fake(int(next()%FAKES)) : nullptr;
                        At<void*>(in,I_HIST_TEMP)=next()%3==0 ? (next()%2 ? evA.data() : evB.data()) : nullptr;
                        At<void*>(in,I_HIST_NONTEMP)=next()%2 ? (next()%2 ? evA.data() : evB.data()) : nullptr;
                        At<int32>(in,I_EVCOUNT)=int32(pick({0,1,1,2,5,uint32(-1),16}));
                        At<float>(in,I_HEARING)=next()%3 ? uni(0.f,60.f) : pick({0,1,2})==0 ? 0.f : uni(60.f,140.f); At<float>(in,I_SEEING)=next()%3 ? uni(0.f,60.f) : At<float>(in,I_HEARING);
                        At<uint16>(in,I_STUCK)=uint16(pick({0,0,1,2})); }
                    // grupo falso y grupos reales (pertenencia en +8, lider en el miembro 7 = +0x28)
                    std::memset(group.data(),0,group.size()); At<uintptr_t>(group.data(),G_PED)=pick({A,B,C,0});
                    for(int k=0;k<8;++k)At<uintptr_t>(group.data(),G_MEMBERS+4*k)=next()%3 ? 0 : pick({A,B,C});
                    At<uintptr_t>(group.data(),G_MEMBERS+4*7)=pick({A,B,C,C,0});
                    for(auto& b:pdata)b=uint8(next()); const uint32 gid=activeGroups[next()%activeGroups.size()]; At<uint32>(pdata.data(),PD_GROUP)=gid;   /* GetGroup de la DLL exige un grupo activo */
                    if(cls==ACQ)for(int k=0;k<8;++k)At<uintptr_t>(groupsMem+gid*GROUP_SIZE,G_MEMBERS+4*k)=next()%3 ? 0 : pick({A,B,C});
                    std::memset(wanted.data(),0,wanted.size()); At<uint32>(wanted.data(),0x2C)=uint32(pick({0,0,1,2,6}));
                    // dependencias simuladas
                    playerRet=C; vehicleRet=0; taskByTypeRet=next()%4 ? 0 : uintptr_t(Fake(int(next()%FAKES))); sameGroup=next()%4==0; rainy=next()%2; looking=next()%3==0;
                    pedMiOk=next()%3!=0; rngState=next();
                    void* const o=object.data(); CPed* const p=PedA();
                    if(isCtor)std::memset(o,0xCD,object.size());
                    else { for(auto& b:object)b=uint8(next()); At<uintptr_t>(o,0)=fn.vt; At<int32>(o,4)=next()%3 ? int32(next()%5)-2 : int32(next()); At<uint8>(o,8)=uint8(next()%2); }
                    const CVector v1=center+vec(10.f), v2=center+vec(10.f);
                    // ---- fixtures por clase ----
                    uintptr_t argA=0, argB=0; float argF=0.f; int32 argI=0, argI2=0;
                    switch(cls){
                    case ACQ:
                        argA=pick({A,B,C,0});
                        if(!isCtor){ At<void*>(o,0)=acqVtbl.data(); At<uintptr_t>(o,0x14)=pick({B,C,A,0,B});
                            At<uint32>(o,EV_TYPE)=uint32(nm=="AffectsPedGroup" ? pick({38,39,38,39,36,37,40}) : pick({36,37,38,39,40}));
                            At<int32>(o,EV_PRIO)=int32(next()%80);
                            auto* ref=(next()%2 ? evA : evB).data(); argB=uintptr_t(ref); At<uintptr_t>(ref,0x14)=pick({A,B,C,0});
                            if(next()%2)At<uint32>(ref,EV_TYPE)=At<uint32>(o,EV_TYPE); }
                        break;
                    case ATTR: {
                        const uintptr_t eff=uintptr_t(fx[next()%NFX].data());
                        if(isCtor){ argA=pick({eff,eff,0}); argB=pick({OA,VA,A,0}); argI=int32(next()%2); }
                        else if(nm=="IsEffectActive"){ argA=pick({OA,VA,BA}); argB=pick({eff,eff,uintptr_t(fakes[1].data())});
                            if(next()%2)fxMap[next()%8]=argB; }   // el efecto esta entre los del modelo
                        else { const bool scripted=nm=="AffectsPed"&&next()%4==0; At<uintptr_t>(o,0)=scripted ? 0x86C9F0 : 0x85B1B8;
                            At<uintptr_t>(o,0x14)=eff; At<uintptr_t>(o,0x18)=scripted||nm=="CloneEditable" ? pick({OA,VA,OA}) : pick({OA,VA,OA,0});
                            At<uint32>(o,0x1C)=next(); At<uint8>(o,0x1C)=uint8(next()%2);
                            if(next()%2)fxMap[next()%8]=eff; }
                        break; }
                    case BUILD:
                        if(isCtor){ argI=int32(int16(next())); argI2=int32(pick({1,4,6,0,7,uint32(-1)})); argF=next()%8 ? uni(-1.f,40.f) : std::bit_cast<float>(QNAN); argA=pick({BA,BB,0}); }
                        else if(nm=="CanTreatBuildingAsObject") argA=pick({BA,BB});
                        else { At<int16>(o,0xC)=int16(next()); At<int16>(o,0xE)=int16(pick({1,4,6,0,7,1,4})); At<float>(o,0x10)=uni(0.f,50.f); At<uintptr_t>(o,0x14)=pick({BA,BB});
                            const CVector fw=At<CVector>(matA.data(),0x10); CVector n=vec(1.f);
                            switch(next()%5){ case 0: n=CVector{-fw.x,-fw.y,-fw.z}+vec(0.3f); break; case 1: n=CVector{-fw.x,-fw.y,uni(0.6f,0.9f)}+vec(0.1f); break; default: break; }
                            if(next()%12==0)n.z=std::bit_cast<float>(QNAN); if(next()%30==0)n.x=std::bit_cast<float>(QNAN); if(next()%20==0){ n.x=0.f; n.y=0.f; }
                            At<CVector>(o,0x18)=n; At<CVector>(o,0x24)=v1; }
                        break;
                    case DANGER:
                        if(isCtor){ argA=pick({A,VA,OA,0,B}); argF=next()%8 ? uni(0.f,30.f) : std::bit_cast<float>(QNAN); }
                        else { At<uintptr_t>(o,0x14)=nm=="GetSourceEntity" ? pick({A,VA,OA,0,BA,VA}) : pick({B,VA,OA,0,C,VA});
                            At<float>(o,0x18)=next()%10 ? uni(0.f,30.f) : (next()%2 ? std::bit_cast<float>(QNAN) : 0.f); }
                        break;
                    case STUCK:
                        argA=pick({A,B,0});
                        if(!isCtor){ At<uintptr_t>(o,0xC)=pick({A,B,0,A}); argB=uintptr_t((next()%2 ? evA : evB).data()); }
                        break;
                    case SCAN: {
                        // escaner: vptr del EXE, contador/periodo del temporizador, entidades previas (con referencia registrada)
                        for(int k=0;k<NENT;++k){ auto* e=ents[k].data(); std::memset(e,0,ents[k].size()); reinterpret_cast<CEntity*>(e)->SetType(ENTITY_TYPE_PED); }
                        if(!isCtor){ std::memset(o,0,object.size()); At<uintptr_t>(o,0)=SCANNER_VT;
                            At<int32>(o,4)=int32(pick({0,0,0,0,3,16,uint32(-1)})); At<int32>(o,8)=int32(pick({16,16,0,5,uint32(-2)}));
                            for(int k=0;k<17;++k){ const size_t off=k<16 ? 0xC+4*k : 0x4C; if(next()%2){ auto* e=Ent(int(next()%NENT)); At<CEntity*>(o,off)=e; e->RegisterReference(&At<CEntity*>(o,off)); } } }
                        if(nm=="ScanForEntitiesInRange"){
                            const float hr=At<float>(intelA.data(),I_HEARING), sr=At<float>(intelA.data(),I_SEEING); const float range=hr>sr ? hr : sr;
                            if(next()%10==0)At<CVector>(pedA.data(),E_POS)=CVector{pick({0,1})==0 ? -3000.f : 2999.f,uni(-3000.f,3000.f),0.f};
                            fillMat(matA.data(),At<CVector>(pedA.data(),E_POS)); if(At<void*>(pedA.data(),E_MATRIX)&&next()%2)At<CVector>(matA.data(),0x30)+=vec(0.5f);
                            const CVector pp=At<void*>(pedA.data(),E_MATRIX) ? At<CVector>(matA.data(),0x30) : At<CVector>(pedA.data(),E_POS);
                            const uint16 cur=uint16(1+next()%0xFFF0); scanCode=cur; At<uint16>(pedA.data(),E_SCAN)=uint16(next());
                            for(int k=0;k<NENT;++k){ auto* e=ents[k].data(); const float r=range*1.25f+0.5f;
                                const CVector pos=pp+CVector{uni(-r,r),uni(-r,r),uni(-r*0.5f,r*0.5f)}; At<CVector>(e,E_POS)=pos; fillMat(entMats[k].data(),pos+vec(next()%3 ? 0.f : 0.5f));
                                At<void*>(e,E_MATRIX)=next()%3 ? entMats[k].data() : nullptr; At<uint16>(e,E_SCAN)=uint16(pick({cur+1u,cur,next()%0x10000,cur}));
                                At(e,P_STATE)=uint32(pick({0x37,1,0x36,1,7})); }
                            // listas de los 256 sectores (vehiculos, peatones, objetos) con nodos falsos { item, next, prev }
                            std::memset(sectorsMem,0,SECTORS_SIZE); int used=0;
                            for(int s=0;s<256*3&&used<NNODE;++s){ if(next()%2)continue; uint32* prev=nullptr; const int len=1+int(next()%4);
                                for(int k=0;k<len&&used<NNODE;++k){ uint32* nd=&nodes[3*used++]; nd[0]=uint32(uintptr_t(Ent(int(next()%NENT)))); nd[1]=0; nd[2]=uint32(uintptr_t(prev));
                                    if(prev)prev[1]=uint32(uintptr_t(nd)); else At<uint32*>(sectorsMem,4*s)=nd; prev=nd; } }
                            playerRet=pick({A,B,B}); argI=int32(pick({0,1,2})); }
                        break; }
                    case BRAIN: {
                        // almacen real con peatones y tareas falsos; las referencias de los peatones se registran como en el juego
                        for(int k=0;k<NENT;++k){ auto* e=ents[k].data(); std::memset(e,0,ents[k].size()); reinterpret_cast<CEntity*>(e)->SetType(ENTITY_TYPE_PED); }
                        const bool full=next()%4==0;
                        for(int k=0;k<48;++k){ uint8* en=storeMem+16*k; const bool used=full||next()%3==0;
                            At<CEntity*>(en,0)=used ? Ent(int(next()%6)) : nullptr; At<uint32>(en,4)=next(); At<uint32>(en,8)=next();
                            At<CTask*>(en,0xC)=next()%3 ? Fake(int(next()%8)) : nullptr;
                            if(auto* e=At<CEntity*>(en,0))e->RegisterReference(&At<CEntity*>(en,0)); }
                        argA=next()%6 ? uintptr_t(Ent(int(next()%7))) : 0; argB=next()%5 ? uintptr_t(Fake(int(next()%9))) : 0;
                        break; }
                    case REFS: {
                        // nodos de referencia falsos (siguiente, puntero a la variable) y variables que apuntan (o no) a la entidad
                        std::memset(refNodes.data(),0,sizeof(refNodes)); int used=0;
                        auto chain=[&](void* ent,int len){ CReference* head=nullptr; CReference** tail=&head;
                            for(int k=0;k<len&&used<NREF;++k){ auto* r=reinterpret_cast<CReference*>(&refNodes[2*used++]); r->m_pNext=nullptr;
                                r->m_ppEntity=reinterpret_cast<CEntity**>(&holders[next()%NHOLD]); *tail=r; tail=&r->m_pNext; }
                            At<CReference*>(ent,E_REFS)=head; };
                        auto fillHolders=[&](std::initializer_list<uintptr_t> l){ for(auto& h:holders)h=uint32(next()%5==0 ? 0 : *(l.begin()+next()%l.size())); };
                        if(nm=="ListSize"){ std::array<int,NREF> perm{}; for(int k=0;k<NREF;++k)perm[k]=k; for(int k=NREF-1;k>0;--k)std::swap(perm[k],perm[next()%(k+1)]);
                            const int len=int(pick({0,1,2,5,17,40,63,64,uint32(next()%64)}));
                            for(int k=0;k<len;++k){ refNodes[2*perm[k]]=k+1<len ? uint32(uintptr_t(&refNodes[2*perm[k+1]])) : 0; refNodes[2*perm[k]+1]=next(); }
                            argA=len ? uintptr_t(&refNodes[2*perm[0]]) : 0; }
                        else if(nm=="PruneAllReferencesInWorld"){
                            std::memset(pedStore.data(),0,pedStore.size()); std::memset(vehStore.data(),0,vehStore.size()); std::memset(objStore.data(),0,objStore.size());
                            auto hdr=[&](std::array<uint8,0x14>& h,void* storage,uint8* states,int cap){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=storage;
                                At<void*>(h.data(),4)=states; At<int32>(h.data(),8)=cap; At<int32>(h.data(),0xC)=-1;
                                for(int k=0;k<8;++k)states[k]=uint8(next()%3 ? (next()&0x7F) : (0x80|next())); };
                            hdr(pedHdr,pedStore.data(),pedStates.data(),PED_CAP); hdr(vehHdr,vehStore.data(),vehStates.data(),VEH_CAP); hdr(objHdr,objStore.data(),objStates.data(),OBJ_CAP);
                            std::vector<uintptr_t> all; for(int k=0;k<PED_CAP;++k)all.push_back(uintptr_t(pedStore.data()+k*PED_STRIDE));
                            for(int k=0;k<VEH_CAP;++k)all.push_back(uintptr_t(vehStore.data()+k*VEH_STRIDE)); for(int k=0;k<OBJ_CAP;++k)all.push_back(uintptr_t(objStore.data()+k*OBJ_STRIDE));
                            for(auto& h:holders)h=next()%6==0 ? 0 : uint32(all[next()%all.size()]);
                            for(auto e:all)chain(reinterpret_cast<void*>(e),int(next()%5));
                            // las variables de cada nodo apuntan a menudo a su propia entidad
                            for(auto e:all)for(auto* r=At<CReference*>(reinterpret_cast<void*>(e),E_REFS);r;r=r->m_pNext)if(next()%2)*reinterpret_cast<uint32*>(r->m_ppEntity)=uint32(e); }
                        else if(nm=="RemoveReferencesToPlayer"){
                            fillHolders({C,VA,A,C,VA});
                            chain(pedC.data(),int(next()%6)); chain(vehA.data(),int(next()%6)); chain(pedA.data(),int(next()%3));
                            playerRet=pick({C,C,C,0}); vehicleRet=pick({VA,VA,0}); }
                        break; }
                    }
                    miSlot=mi.data();
                    if(cls==REFS&&nm=="PruneAllReferencesInWorld"){ pedPool=pedHdr.data(); vehPool=vehHdr.data(); objPool=objHdr.data(); }
                    for(auto* h:hooks)h->State(candidate);
                    // VMT: la entrada de la vtable del EXE cambia con el hook; estatico: la entrada del EXE salta a la DLL
                    const uintptr_t call=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((call>=0x400000&&call<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    using V=void*; using CV=const CVector&;
                    if(isCtor) switch(cls){
                    case ACQ: case STUCK: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t)>(call)(o,argA)); break;
                    case ATTR: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t,uintptr_t,bool)>(call)(o,argA,argB,argI!=0)); break;
                    case BUILD: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,int16,float,uintptr_t,CV,CV,int16)>(call)(o,int16(argI),argF,argA,v1,v2,int16(argI2))); break;
                    case DANGER: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t,float)>(call)(o,argA,argF)); break;
                    case SCAN: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V)>(call)(o)); break;
                    default: break;
                    }
                    else if(cls==SCAN){
                        if(nm=="ScanForEntitiesInRange") reinterpret_cast<void(__thiscall*)(V,int32,const CPed&)>(call)(o,argI,*p);
                        else reinterpret_cast<void(__thiscall*)(V)>(call)(o); }   // Clear / destructor
                    else if(cls==BRAIN){
                        if(nm=="SetTask") rawRet=uintptr_t(reinterpret_cast<V(__cdecl*)(uintptr_t,uintptr_t)>(call)(argA,argB));
                        else if(nm=="GetTask") rawRet=uintptr_t(reinterpret_cast<V(__cdecl*)(uintptr_t)>(call)(argA));
                        else if(nm=="Clear-ped") reinterpret_cast<void(__cdecl*)(uintptr_t)>(call)(argA);
                        else reinterpret_cast<void(__cdecl*)(uintptr_t)>(call)(argB); }
                    else if(cls==REFS){
                        if(nm=="ListSize") ret=reinterpret_cast<uint32(__cdecl*)(uintptr_t)>(call)(argA);
                        else reinterpret_cast<void(__cdecl*)()>(call)(); }
                    else if(nm=="IsEffectActive") ret=reinterpret_cast<bool(__cdecl*)(uintptr_t,uintptr_t)>(call)(argA,argB);
                    else if(nm=="CanTreatBuildingAsObject") ret=reinterpret_cast<bool(__cdecl*)(uintptr_t)>(call)(argA);
                    else if(nm=="IsHeadOnCollision"||nm=="AffectsPed") ret=reinterpret_cast<bool(__thiscall*)(V,CPed*)>(call)(o,p);
                    else if(nm=="AffectsPedGroup") ret=reinterpret_cast<bool(__thiscall*)(V,V)>(call)(o,group.data());
                    else if(nm=="TakesPriorityOver") ret=reinterpret_cast<bool(__thiscall*)(V,uintptr_t)>(call)(o,argB);
                    else if(nm=="GetEventPriority") ret=reinterpret_cast<int32(__thiscall*)(V)>(call)(o);
                    else rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V)>(call)(o));   // GetSourceEntity / CloneEditable
                    for(auto* h:hooks)h->State(false);
                    // estado tras la llamada (antes de restaurar nada)
                    if(cls==REFS&&nm=="PruneAllReferencesInWorld"){ pedPool=savedPedPool; vehPool=savedVehPool; objPool=savedObjPool; }
                    miSlot=savedMi;
                    if(ret.is_null())ret=Name(rawRet);
                    json delList=json::array(); for(size_t k=0;k<ndel;++k)delList.push_back({Name(dels[k].at),dels[k].flags});
                    json objectWords=Region(o,object.size(),isCtor||(cls==SCAN&&nm=="Destructor"));
                    if(isCtor){ const uintptr_t vp=At<uintptr_t>(o,0);
                        if(cls==ACQ||cls==SCAN)objectWords.push_back(vp!=0xCDCDCDCDu ? "vptr" : "no_vptr");   // CEventAcquaintancePed es abstracta; vptr EXE o DLL
                        else objectWords.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType())); }   // vptr EXE o DLL: se compara el tipo
                    // clon nuevo del pool de eventos (CloneEditable): tipo + bytes
                    json clone=nullptr;
                    { const auto now=EvOccupancy(); for(size_t k=0;k<now.size();++k)if(now[k]&&!occ[k]){ void* fresh=EvStorage()+k*EVENT_SLOT;
                        clone={{"at",Name(fresh)},{"type",int(reinterpret_cast<CEvent*>(fresh)->GetEventType())},{"words",Region(fresh,EVENT_SLOT,true)}}; break; } }
                    json refs=json::array(); for(const void* e:{(const void*)pedA.data(),(const void*)pedB.data(),(const void*)pedC.data(),(const void*)vehA.data(),(const void*)objA.data(),(const void*)bldA.data(),(const void*)bldB.data()})refs.push_back(RefList(e));
                    json r={{"ret",ret},{"object",objectWords},{"clone",clone},{"refs",refs},{"dels",delList},{"free",FreeHead()},
                            {"peds",{Hash(pedA.data(),pedA.size()),Hash(pedB.data(),pedB.size()),Hash(pedC.data(),pedC.size())}},
                            {"others",{Hash(vehA.data(),vehA.size()),Hash(objA.data(),objA.size()),Hash(bldA.data(),bldA.size()),Hash(bldB.data(),bldB.size())}},
                            {"intel",{Hash(intelA.data(),intelA.size()),Hash(intelB.data(),intelB.size())}},{"fakes",Hash(fakes.data(),sizeof(fakes))},{"fx",Hash(fx.data(),sizeof(fx))},
                            {"events",{Region(evA.data(),evA.size(),true),Region(evB.data(),evB.size(),true)}},{"group",Region(group.data(),0x30)},
                            {"groups",Hash(groupsMem,NGROUPS*GROUP_SIZE)},{"pdata",Hash(pdata.data(),pdata.size())},{"mi",Hash(mi.data(),mi.size())},{"col",Hash(col.data(),col.size())},
                            {"mats",{Hash(matA.data(),matA.size()),Hash(matB.data(),matB.size()),Hash(matC.data(),matC.size()),Hash(matV.data(),matV.size()),Hash(matO.data(),matO.size())}}};
                    if(cls==SCAN||cls==BRAIN){ json er=json::array(); for(int k=0;k<NENT;++k)er.push_back(RefList(ents[k].data()));
                        r["ents"]=er; r["entsHash"]=Hash(ents.data(),sizeof(ents)); r["scanCode"]=scanCode; r["sectors"]=Hash(sectorsMem,SECTORS_SIZE); r["nodes"]=Hash(nodes.data(),sizeof(nodes)); }
                    if(cls==BRAIN)r["store"]=Region(storeMem,STORE_SIZE);
                    if(cls==REFS){ r["aRefs"]=Hash(&CReferences::aRefs[0],sizeof(CReferences::aRefs)); r["lastRef"]=Region(&CReferences::aRefs[MAX_NUM_REFERENCES-1],8);
                        r["refNodes"]=Region(refNodes.data(),sizeof(refNodes)); r["holders"]=Region(holders.data(),sizeof(holders));
                        json pr=json::array(); for(auto* s:{pedStore.data(),vehStore.data(),objStore.data()}){ const size_t stride=s==pedStore.data() ? PED_STRIDE : s==vehStore.data() ? VEH_STRIDE : OBJ_STRIDE;
                            const int cap=s==pedStore.data() ? PED_CAP : s==vehStore.data() ? VEH_CAP : OBJ_CAP; for(int k=0;k<cap;++k)pr.push_back(RefList(s+k*stride)); }
                        r["poolRefs"]=pr; r["pools"]={Hash(pedStore.data(),pedStore.size()),Hash(vehStore.data(),vehStore.size()),Hash(objStore.data(),objStore.size()),
                            Hash(pedHdr.data(),0x14),Hash(vehHdr.data(),0x14),Hash(objHdr.data(),0x14),Hash(pedStates.data(),8),Hash(vehStates.data(),8),Hash(objStates.data(),8)}; }
                    r["trace"]=trace; r["switches"]=switches;
                    restoreAll();
                    r["clean"]=EvOccupancy()==occupancyStart;
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    restoreAll();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateRestored()}});
}
}
