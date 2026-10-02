#pragma once
// Banco dentro del motor: eventos de la lista de 52 (30 funciones de 10 clases). CEvent (constructor,
// CalcSoundLevelIncrement, GetSoundLevel), CEventGlobalGroup (AddEventsToGroup, AddEventsToPed, GetSoundLevel) y
// constructor + virtuales de CEventAreaCodes, CEventPedCollisionWithPed, CEventPotentialWalkIntoPed, CEventSoundQuiet,
// CEventVehicleCollision, CEventVehicleDied, CEventVehicleHitAndRun y CEventVehicleOnFire.
// Constructores llamados en la direccion del EXE con `this` = buffer propio rellenado con 0xCD (comparado entero con
// el tipo de evento en vez del vptr; CEvent es abstracto). Los metodos virtuales se llaman por la entrada de la
// vtable del EXE con el evento construido a mano sobre la vtable del EXE. Clone/CloneEditable: el clon sale del pool
// de eventos real, se compara tipo + bytes, se borra y el pool se restaura entero en cada ejecucion.
// CEventGlobalGroup trabaja sobre un CEventGlobalGroup falso (nunca el del juego) con eventos falsos de vtable propia
// (tipo, prioridad, entidad origen, nivel de sonido local, Clone y destructor trazados) y un CPedGroup falso.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: tres peatones con matrices
// falsas, dos vehiculos, dos inteligencias con tareas falsas (GetSubTask/GetTaskType/IsSimple), eventos falsos en el
// historial y un CPedGroup falso (pertenencia en +8, lider en el miembro 7 = +0x28).
// Se simulan y trazan: CPedIntelligence::IsThreatenedBy, CPedGroups::GetPedsGroup, CPedGeometryAnalyser::
// ComputeEntityHitSide (peaton y punto) y ComputeEntityBoundingBoxPlanes, CTaskComplexWalkRoundCar::SetNewVehicle,
// FindPlayerWanted, CWanted::RegisterCrime, CEventGroup::Add y CPedGroupIntelligence::AddEvent. El reloj y el pool de
// eventos (cabecera, estados y almacenamiento) se guardan y se restauran.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/Event.h"
#include "Events/EventGroup.h"
#include "Events/EventAreaCodes.h"
#include "Events/EventGlobalGroup.h"
#include "Events/EventPedCollisionWithPed.h"
#include "Events/EventPotentialWalkIntoPed.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventVehicleCollision.h"
#include "Events/EventVehicleDied.h"
#include "Events/EventVehicleHitAndRun.h"
#include "Events/EventVehicleOnFire.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundCar.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "PedGroupIntelligence.h"
#include "Wanted.h"
#include "Pools/Pools.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace Events52Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_AREA=0x2F, PH_ATTACHED=0xFC, P_FLAGS1=0x46C, P_INTEL=0x47C, P_STATE=0x530,
                 P_CONTACT=0x584, P_VEHICLE=0x58C, P_TYPE=0x598, V_DRIVER=0x460, V_TOWING=0x4C4, V_TOWED=0x4C8, V_TYPE=0x590;
constexpr size_t I_PRIMARY=0x04, I_SECONDARY=0x18, I_HIST_NONTEMP=0x3C, I_HIST_TEMP=0x40, I_STATIC=0x274;   // CPedIntelligence
constexpr size_t G_MEMBERS=0x0C;                                                                         // CPedGroup (miembro 7 = lider)
constexpr size_t FK_TYPE=0x38, FK_SUB=0x3C;                                                              // tarea falsa
constexpr size_t EV_TYPE=0x30, EV_SRC=0x34, EV_LEVEL=0x38, EV_PRIO=0x3C, EV_CLONE=0x40, EV_SIZE=0x48;     // evento falso
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, EVENT_SLOT=0x44, GLOBAL_GROUP_VT=0x85AAB4;
constexpr int FAKES=12, GEV=18, CLONES=4;
constexpr uint32 QNAN=0x7FC00000u;

alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> vehA{}, vehB{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,0x50> ggroup{}, wanted{};
alignas(16) inline std::array<uint8,0x48> matA{}, matB{}, matC{}, matVA{}, matVB{};
alignas(16) inline std::array<uint8,EV_SIZE> evA{}, evB{}, object{};
alignas(16) inline std::array<std::array<uint8,EV_SIZE>,GEV> gev{};
alignas(16) inline std::array<std::array<uint8,EV_SIZE>,CLONES> clones{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
inline CPed* Ped(std::array<uint8,sizeof(CPed)>& b) { return reinterpret_cast<CPed*>(b.data()); }
inline CPed* PedA() { return Ped(pedA); } inline CPed* PedB() { return Ped(pedB); } inline CPed* PedC() { return Ped(pedC); }
inline CVehicle* VehA() { return reinterpret_cast<CVehicle*>(vehA.data()); } inline CVehicle* VehB() { return reinterpret_cast<CVehicle*>(vehB.data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pool de eventos (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot, banderas)
inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }

// nombres estables para punteros a los fixtures
struct Buf { const void* p; size_t n; std::string nm; };
inline std::vector<Buf> bufs;
inline void RegisterBuffers() {
    bufs.clear();
    auto add=[](const void* p,size_t n,std::string nm){ bufs.push_back({p,n,std::move(nm)}); };
    add(pedA.data(),pedA.size(),"pedA"); add(pedB.data(),pedB.size(),"pedB"); add(pedC.data(),pedC.size(),"pedC");
    add(vehA.data(),vehA.size(),"vehA"); add(vehB.data(),vehB.size(),"vehB"); add(intelA.data(),intelA.size(),"intelA");
    add(intelB.data(),intelB.size(),"intelB"); add(group.data(),group.size(),"group"); add(ggroup.data(),ggroup.size(),"ggroup");
    add(wanted.data(),wanted.size(),"wanted"); add(matA.data(),matA.size(),"matA"); add(matB.data(),matB.size(),"matB");
    add(matC.data(),matC.size(),"matC"); add(matVA.data(),matVA.size(),"matVA"); add(matVB.data(),matVB.size(),"matVB");
    add(evA.data(),evA.size(),"evA"); add(evB.data(),evB.size(),"evB"); add(object.data(),object.size(),"object");
    for(int n=0;n<GEV;++n)add(gev[n].data(),EV_SIZE,"gev#"+std::to_string(n));
    for(int n=0;n<CLONES;++n)add(clones[n].data(),EV_SIZE,"clone#"+std::to_string(n));
    for(int n=0;n<FAKES;++n)add(fakes[n].data(),fakes[n].size(),"fk#"+std::to_string(n));
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return b.nm+"+"+std::to_string(v-uintptr_t(b.p));
    { const auto es=uintptr_t(EvStorage()); if(v>=es&&v<es+EvCap()*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline bool threatRet{}; inline std::array<uintptr_t,3> groupOf{}; inline uint32 hitSalt{}, planeSalt{};

// tareas falsas: lectores puros (no se trazan: la DLL y el EXE pueden llamarlos un numero distinto de veces)
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask* t,void*) { return At<CTask*>(t,FK_SUB)==nullptr; }
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(t)}); return nullptr; }
inline std::array<void*,16> taskVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// eventos falsos: destructor (0), GetEventType (1), GetEventPriority (2), Clone (4), GetSourceEntity (10) y
// GetLocalSoundLevel (12). Los lectores son puros; Clone se traza; el destructor se apunta en un array fijo.
struct Del { uintptr_t at; uint32 flags; };
inline std::array<Del,64> dels{}; inline size_t ndel{};
inline void* __fastcall EDtor(void* e,void*,uint32 flags) { if(ndel<dels.size())dels[ndel++]={uintptr_t(e),flags}; return e; }
inline int __fastcall EType(void* e,void*) { return At<int32>(e,EV_TYPE); }
inline int __fastcall EPrio(void* e,void*) { return At<int32>(e,EV_PRIO); }
inline void* __fastcall EClone(void* e,void*) { trace.push_back({"clone",Name(e)}); return At<void*>(e,EV_CLONE); }
inline CEntity* __fastcall ESource(void* e,void*) { return At<CEntity*>(e,EV_SRC); }
inline float __fastcall ELevel(void* e,void*) { return At<float>(e,EV_LEVEL); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
inline std::array<void*,20> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline bool __fastcall Threatened(void* intel,void*,const CPed& p) { trace.push_back({"is_threatened_by",Name(intel),Name(&p)}); return threatRet; }
inline CPedGroup* __cdecl PedsGroup(const CPed* p) {
    trace.push_back({"get_peds_group",Name(p)});
    const auto v=uintptr_t(p); const int k=v==uintptr_t(PedA()) ? 0 : v==uintptr_t(PedB()) ? 1 : v==uintptr_t(PedC()) ? 2 : -1;
    return reinterpret_cast<CPedGroup*>(k>=0 ? groupOf[k] : 0);
}
inline int32 __cdecl HitSidePed(const CPed& p,CEntity& e) { trace.push_back({"entity_hit_side_ped",Name(&p),Name(&e)}); return int32(((uintptr_t(&p)>>4)^hitSalt)%2u); }
inline int32 __cdecl HitSidePoint(const CVector& pt,CEntity& e) {
    trace.push_back({"entity_hit_side_point",F(pt.x),F(pt.y),F(pt.z),Name(&e)});
    return int32(((F(pt.x)*2654435761u)^(F(pt.y)*40503u)^F(pt.z)^hitSalt)%2u);
}
inline void __cdecl Planes(float z,CEntity& e,CVector(*out)[4],float* d) {
    trace.push_back({"bounding_box_planes",F(z),Name(&e)});
    uint32 s=planeSalt|1u; auto nx=[&]{s^=s<<13;s^=s>>17;s^=s<<5;return float(int32(s%2001u)-1000)/1000.f;};
    const uint32 mode=planeSalt%3u;
    for(int k=0;k<4;++k){ (*out)[k]=CVector{nx(),nx(),nx()*0.1f}; d[k]=mode==0 ? nx()*8.f : mode==1 ? -500.f : 500.f; }
}
inline void __fastcall SetNewVeh(void* t,void*,CVehicle* v,uint32 dir) { trace.push_back({"walk_round_car_set_new_vehicle",Name(t),Name(v),dir&0xFF}); }
inline CWanted* __cdecl FindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __fastcall RegCrime(void* w,void*,int32 crime,const CVector& pos,uint32 id,uint32 care) {
    trace.push_back({"register_crime",Name(w),crime,F(pos.x),F(pos.y),F(pos.z),Name(uintptr_t(id)),care&0xFF});
}
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),Name(e),valid&0xFF}); return e; }
inline bool __fastcall GroupIntelAdd(void* g,void*,CEvent* e) { trace.push_back({"group_intel_add_event",Name(g),Name(e)}); return true; }

inline std::vector<bool> EvOccupancy() { std::vector<bool> r(EvCap()); for(size_t k=0;k<r.size();++k)r[k]=!(EvStates()[k]&0x80); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Cls { EVENT, AREA, GLOBAL, PCWP, WIPED, SOUND, VCOLL, VDIED, HITRUN, FIRE };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; Cls c; };
constexpr size_t NFNS=30;
inline const std::array<Fn,NFNS> fns{{
    {"CEvent","Constructor",0x4ABFC0,0x85AB38,-1,64,EVENT},
    {"CEvent","CalcSoundLevelIncrement",0x4AC050,0,-1,1024,EVENT},
    {"CEvent","GetSoundLevel",0x4B2850,0,-1,1024,EVENT},
    {"CEventAreaCodes","Constructor",0x4B2190,0x85B7C0,-1,128,AREA},
    {"CEventAreaCodes","AffectsPed",0,0x85B7C0,5,1024,AREA},
    {"CEventAreaCodes","TakesPriorityOver",0,0x85B7C0,11,256,AREA},
    {"CEventGlobalGroup","AddEventsToGroup",0x4AB8A0,0,-1,256,GLOBAL},
    {"CEventGlobalGroup","AddEventsToPed",0x4AB9C0,0,-1,256,GLOBAL},
    {"CEventGlobalGroup","GetSoundLevel",0x4AB900,0,-1,1024,GLOBAL},
    {"CEventPedCollisionWithPed","Constructor",0x4AC990,0x85AC68,-1,128,PCWP},
    {"CEventPedCollisionWithPed","AffectsPed",0,0x85AC68,5,1024,PCWP},
    {"CEventPedCollisionWithPed","TakesPriorityOver",0,0x85AC68,11,256,PCWP},
    {"CEventPotentialWalkIntoPed","Constructor",0x4AE6E0,0x85AF60,-1,128,WIPED},
    {"CEventPotentialWalkIntoPed","AffectsPed",0,0x85AF60,5,1024,WIPED},
    {"CEventPotentialWalkIntoPed","TakesPriorityOver",0,0x85AF60,11,256,WIPED},
    {"CEventSoundQuiet","Constructor",0x5E05B0,0x86C2B0,-1,128,SOUND},
    {"CEventSoundQuiet","AffectsPed",0,0x86C2B0,5,1024,SOUND},
    {"CEventSoundQuiet","CloneEditable",0,0x86C2B0,16,256,SOUND},
    {"CEventVehicleCollision","Constructor",0x4AC840,0x85AC28,-1,128,VCOLL},
    {"CEventVehicleCollision","AffectsPed",0,0x85AC28,5,1024,VCOLL},
    {"CEventVehicleCollision","Clone",0,0x85AC28,4,256,VCOLL},
    {"CEventVehicleDied","Constructor",0x4B0D10,0x85B400,-1,128,VDIED},
    {"CEventVehicleDied","AffectsPed",0,0x85B400,5,512,VDIED},
    {"CEventVehicleDied","Clone",0,0x85B400,4,256,VDIED},
    {"CEventVehicleHitAndRun","Constructor",0x4AE990,0x85AFA8,-1,128,HITRUN},
    {"CEventVehicleHitAndRun","Clone",0,0x85AFA8,4,256,HITRUN},
    {"CEventVehicleHitAndRun","ReportCriminalEvent",0,0x85AFA8,8,512,HITRUN},
    {"CEventVehicleOnFire","Constructor",0x4B10C0,0x85B440,-1,128,FIRE},
    {"CEventVehicleOnFire","AffectsPed",0,0x85B440,5,1024,FIRE},
    {"CEventVehicleOnFire","CloneEditable",0,0x85B440,16,256,FIRE},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(PedC()); clean(VehA()); clean(VehB());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"events52-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"events52-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("events52");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"events52-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    RegisterBuffers();
    FillTraps(std::make_integer_sequence<int,16>{}); taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[3]=reinterpret_cast<void*>(&FSimple); taskVtbl[4]=reinterpret_cast<void*>(&FType);
    FillETraps(std::make_integer_sequence<int,20>{}); eventVtbl[0]=reinterpret_cast<void*>(&EDtor); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    eventVtbl[2]=reinterpret_cast<void*>(&EPrio); eventVtbl[4]=reinterpret_cast<void*>(&EClone); eventVtbl[10]=reinterpret_cast<void*>(&ESource);
    eventVtbl[12]=reinterpret_cast<void*>(&ELevel);
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto restoreEvents=[&]{ std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size());
                            std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size()); };
    const auto occupancyStart=EvOccupancy();
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(&CPedIntelligence::IsThreatenedBy),Threatened), s1e(0x601C30,Threatened);
        EntryPatch s2e(0x5F7E80,PedsGroup);   // la DLL solo envuelve la del EXE
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide)),HitSidePed), s3e(0x5F3BC0,HitSidePed);
        EntryPatch s4e(0x5F3AC0,HitSidePoint);   // la DLL solo envuelve la del EXE
        EntryPatch s5(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes),Planes), s5e(0x5F3660,Planes);
        EntryPatch s6(bit_cast<uintptr_t>(&CTaskComplexWalkRoundCar::SetNewVehicle),SetNewVeh), s6e(0x654290,SetNewVeh);
        EntryPatch s7(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FindWanted), s7e(0x56E230,FindWanted);
        EntryPatch s8(bit_cast<uintptr_t>(&CWanted::RegisterCrime),RegCrime), s8e(0x562410,RegCrime);
        EntryPatch s9(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), s9e(0x4AB420,GroupAdd);
        EntryPatch s10(bit_cast<uintptr_t>(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent)),GroupIntelAdd), s10e(0x5F7470,GroupIntelAdd);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isCtor=nm=="Constructor";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l){return *(l.begin()+next()%l.size());};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5252u; if(!st)st=1; trace=json::array(); ndel=0;
                    CleanRefs(); restoreEvents();
                    // pool de eventos: los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto)
                    for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    const auto occ=EvOccupancy();
                    const uintptr_t A=uintptr_t(PedA()), B=uintptr_t(PedB()), C=uintptr_t(PedC()), VA=uintptr_t(VehA()), VB=uintptr_t(VehB());
                    const bool needFwd=!isCtor&&nm=="AffectsPed"&&(fn.c==PCWP||fn.c==VCOLL);   // el EXE lee la matriz del peaton sin comprobar null
                    // matrices: vectores normales y posicion en +0x30
                    for(auto* m:{&matA,&matB,&matC,&matVA,&matVB}){ std::memset(m->data(),0,m->size());
                        for(int k=0;k<12;++k)if(k%4!=3)At<float>(m->data(),4*k)=uni(-1.f,1.f); }
                    // peatones
                    const CVector center=vec(20.f);
                    std::array<std::array<uint8,0x48>*,3> pm{&matA,&matB,&matC}; int pk=0;
                    for(auto* buf:{&pedA,&pedB,&pedC}){ auto* p=buf->data(); std::memset(p,0,buf->size()); reinterpret_cast<CEntity*>(p)->SetType(ENTITY_TYPE_PED);
                        const CVector pos=center+CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-2.f,2.f)};
                        At<CVector>(p,E_POS)=pos; At<CVector>(pm[pk]->data(),0x30)=pos;
                        At<void*>(p,E_MATRIX)=(next()%4||(pk==0&&needFwd)) ? pm[pk]->data() : nullptr;
                        At(p,P_TYPE)=uint32(pick({0,1,4,6,6,20,7,8,6})); At(p,P_STATE)=uint32(pick({1,1,1,0x36,0x37,0,7,0x32}));
                        At(p,P_FLAGS1)=next(); At<uintptr_t>(p,P_VEHICLE)=pick({0,VA,VA,VB}); At<uintptr_t>(p,P_CONTACT)=pick({0,VA,VB,VA});
                        At<uintptr_t>(p,PH_ATTACHED)=next()%6 ? 0 : pick({VA,B}); ++pk; }
                    At<void*>(pedA.data(),P_INTEL)=intelA.data(); At<void*>(pedB.data(),P_INTEL)=intelB.data(); At<void*>(pedC.data(),P_INTEL)=intelB.data();
                    // vehiculos
                    std::array<std::array<uint8,0x48>*,2> vm{&matVA,&matVB}; int vk=0;
                    for(auto* buf:{&vehA,&vehB}){ auto* v=buf->data(); std::memset(v,0,buf->size()); reinterpret_cast<CEntity*>(v)->SetType(ENTITY_TYPE_VEHICLE);
                        const CVector pos=center+vec(10.f); At<CVector>(v,E_POS)=pos; At<CVector>(vm[vk]->data(),0x30)=pos+vec(0.5f);
                        At<void*>(v,E_MATRIX)=next()%2 ? vm[vk]->data() : nullptr; At<uint8>(v,E_AREA)=uint8(pick({0,0,1,13,13,2}));
                        At(v,V_TYPE)=uint32(pick({0,0,5,3,9,0})); At<uintptr_t>(v,V_DRIVER)=pick({0,A,B,C,A});
                        At<uintptr_t>(v,V_TOWING)=next()%3 ? 0 : pick({VA,VB}); At<uintptr_t>(v,V_TOWED)=next()%3 ? 0 : pick({VA,VB}); ++vk; }
                    // tareas falsas
                    static const uint32 taskTypes[]{0x3E8,0x3E8,0x44D,0x3A4,0x4B2,0x4B3,0x4BF,0x3A8,0x395,0x3FB,0x2C0,0x1FC,0x1F9,0x384,0x39E,0xCB,0x190,0x384};
                    for(int n=0;n<FAKES;++n){ auto* t=fakes[n].data(); for(auto& b:fakes[n])b=uint8(next()); At<void*>(t,0)=taskVtbl.data(); At<void*>(t,4)=nullptr;
                        const uint32 ty=taskTypes[next()%std::size(taskTypes)]; At<uint32>(t,FK_TYPE)=ty; At<CTask*>(t,FK_SUB)=n<FAKES-1&&next()%3==0 ? Fake(FAKES-1) : nullptr;
                        if(ty==0x3E8||ty==0x44D)At<uintptr_t>(t,0x10)=pick({B,C,0,A,B});
                        else if(ty==0x3A8||ty==0x395)At<uintptr_t>(t,0xC)=pick({B,C,0,A});
                        else if(ty==0x1F9)At<uintptr_t>(t,0xC)=pick({VA,VB,0});
                        else if(ty==0x1FC)At<uintptr_t>(t,0x1C)=pick({VA,VB,0,VA});
                        else if(ty==0x384||ty==0x39E)At<CVector>(t,0xC)=center+vec(12.f); }
                    // inteligencias: tareas primarias (+4), secundarias (+0x18), historial de eventos y contador estatico
                    for(auto* buf:{&intelA,&intelB}){ std::memset(buf->data(),0,buf->size()); auto* in=buf->data();
                        for(int k=0;k<5;++k)At<CTask*>(in,I_PRIMARY+4*k)=next()%3==0 ? Fake(int(next()%FAKES)) : nullptr;
                        for(int k=0;k<6;++k)At<CTask*>(in,I_SECONDARY+4*k)=next()%4==0 ? Fake(int(next()%FAKES)) : nullptr;
                        At<void*>(in,I_HIST_TEMP)=next()%3==0 ? (next()%2 ? evA.data() : evB.data()) : nullptr;
                        At<void*>(in,I_HIST_NONTEMP)=next()%2 ? (next()%2 ? evA.data() : evB.data()) : nullptr;
                        At<int32>(in,I_STATIC)=int32(pick({0,29,30,31,100,uint32(-5),30,5})); }
                    // eventos falsos (historial y "otro evento" de TakesPriorityOver): disposicion de CEventSoundQuiet en +0x14..+0x2B
                    const uint32 t0=next();
                    for(auto* buf:{&evA,&evB}){ auto* e=buf->data(); for(auto& b:*buf)b=uint8(next()); At<void*>(e,0)=eventVtbl.data();
                        At<uint32>(e,EV_TYPE)=uint32(pick({62,62,62,1,2,13,67,14,35,79,uint32(next()%100)})); At<uintptr_t>(e,EV_SRC)=pick({B,C,0,A,VA,B});
                        At<float>(e,EV_LEVEL)=next()%8 ? uni(20.f,120.f) : (next()%2 ? 0.f : std::bit_cast<float>(QNAN)); At<int32>(e,EV_PRIO)=int32(next()%80);
                        At<uintptr_t>(e,EV_CLONE)=0; At<uint32>(e,0x1C)=t0; At<CVector>(e,0x20)=center+vec(3.f); }
                    // eventos del grupo global y clones
                    for(int n=0;n<GEV;++n){ auto* e=gev[n].data(); for(auto& b:gev[n])b=uint8(next()); At<void*>(e,0)=eventVtbl.data();
                        At<uint32>(e,EV_TYPE)=uint32(pick({62,62,14,79,35,13})); At<uintptr_t>(e,EV_SRC)=pick({A,B,VA,0,C});
                        At<float>(e,EV_LEVEL)=next()%10 ? uni(-20.f,120.f) : (next()%3 ? 0.f : std::bit_cast<float>(QNAN)); At<int32>(e,EV_PRIO)=int32(next()%80);
                        At<CVector>(e,0x20)=center+vec(25.f); At<void*>(e,EV_CLONE)=next()%5 ? clones[next()%CLONES].data() : nullptr; }
                    for(int n=0;n<CLONES;++n){ auto* e=clones[n].data(); for(auto& b:clones[n])b=uint8(next()); At<void*>(e,0)=eventVtbl.data(); }
                    std::memset(ggroup.data(),0,ggroup.size()); At<uintptr_t>(ggroup.data(),0)=GLOBAL_GROUP_VT; At<uintptr_t>(ggroup.data(),4)=pick({A,0});
                    At<uint32>(ggroup.data(),8)=next()%8 ? next()%17 : 0; for(int k=0;k<16;++k)At<void*>(ggroup.data(),0xC+4*k)=gev[next()%GEV].data();
                    // grupo
                    std::memset(group.data(),0,group.size());
                    for(int k=0;k<8;++k)At<uintptr_t>(group.data(),G_MEMBERS+4*k)=next()%3 ? 0 : pick({A,B,C});
                    At<uintptr_t>(group.data(),G_MEMBERS+4*7)=pick({A,B,C,C,0,B});
                    for(auto& g:groupOf)g=next()%3 ? uintptr_t(group.data()) : 0;
                    std::memset(wanted.data(),0,wanted.size());
                    // dependencias simuladas
                    threatRet=next()%4==0; hitSalt=next(); planeSalt=next(); timeMs=next();
                    // evento probado
                    void* const o=object.data(); CPed* const p=PedA();
                    if(isCtor)std::memset(o,0xCD,object.size());
                    else { for(auto& b:object)b=uint8(next()); At<uintptr_t>(o,0)=fn.vt; At<int32>(o,4)=next()%3 ? int32(next()%5)-2 : int32(next()); At<uint8>(o,8)=uint8(next()%2); }
                    // argumentos
                    const CVector v1=center+vec(10.f), v2=center+vec(10.f);
                    const float argF=next()%8 ? uni(-1.f,120.f) : (next()%2 ? std::bit_cast<float>(QNAN) : 0.f);
                    const uint32 argW1=next(), argW2=next(), argW3=next();
                    const uintptr_t argPed=pick({A,B,0,C}), argPedNN=pick({A,B,C}), argVeh=pick({VA,VB,0}), argVehNN=pick({VA,VB}), argEnt=pick({A,VA,0,B});
                    uint32 argTime=next()%3 ? uint32(-1) : next(); const uintptr_t sqEnt=argTime==uint32(-1)&&!argEnt ? B : argEnt;   // con -1 el EXE lee la posicion sin comprobar null
                    // CalcSoundLevelIncrement / GetSoundLevel
                    auto level=[&]{ switch(next()%10){ case 0: return 0.f; case 1: return -0.f; case 2: return std::bit_cast<float>(QNAN); case 3: return uni(-50.f,0.f);
                                                        case 4: return uni(200.f,500.f); case 5: return uni(0.f,1.f); default: return uni(0.f,150.f); } };
                    const float lv1=level(), lv2=level();
                    CVector posArg=center+vec(25.f); if(next()%16==0)posArg.y=std::bit_cast<float>(QNAN);
                    const uintptr_t entArg=next()%2 ? 0 : pick({A,B,VA,C});
                    CPed* groupPed=reinterpret_cast<CPed*>(pick({A,B}));
                    // campos del evento para los metodos virtuales
                    if(!isCtor) switch(fn.c){
                    case EVENT: if(nm=="GetSoundLevel"){
                            if(next()%3){ At<void*>(o,0)=eventVtbl.data(); At<uint32>(o,EV_TYPE)=uint32(pick({62,62,14,79,35})); At<uintptr_t>(o,EV_SRC)=pick({A,B,VA,0,C});
                                          At<float>(o,EV_LEVEL)=next()%6 ? uni(-20.f,150.f) : (next()%2 ? 0.f : std::bit_cast<float>(QNAN)); }
                            else { At<uintptr_t>(o,0)=0x86C2B0; At<uintptr_t>(o,0x14)=pick({A,B,VA,0,C}); At<float>(o,0x18)=next()%6 ? uni(-20.f,150.f) : 0.f; }   // CEventSoundQuiet real
                            At<CVector>(o,0x20)=posArg+vec(next()%4 ? 30.f : 0.8f); }
                        break;
                    case AREA: At<uintptr_t>(o,0xC)=pick({B,C,A,0,B}); break;
                    case PCWP: At<uintptr_t>(o,0x14)=pick({B,C,0,B,A}); At<CVector>(o,0x18)=vec(1.f); At<int16>(o,0x30)=int16(pick({0,1,2,3,4,6,7,uint32(-1),0x7FFF,1}));
                        if(next()%3==0){ const CVector fw=At<CVector>(matA.data(),0x10); At<CVector>(o,0x18)=CVector{-fw.x,-fw.y,-fw.z}+vec(0.3f); }
                        break;
                    case WIPED: At<CVector>(o,0x14)=v1; At<uintptr_t>(o,0x20)=pick({B,C,0,A,B}); At<int32>(o,0x24)=int32(pick({1,4,6,7,0,4})); break;
                    case SOUND: { At<uintptr_t>(o,0x14)=pick({B,C,VA,0,B}); At<float>(o,0x18)=next()%8 ? uni(20.f,120.f) : (next()%2 ? 0.f : std::bit_cast<float>(QNAN));
                        static const int32 deltas[]{2000,1999,2001,0,-1,-2000,int32(0x80000000),5000,0x7FFFFFFF};
                        At<uint32>(o,0x1C)=t0+uint32(next()%2 ? deltas[next()%std::size(deltas)] : int32(next()%10000)-5000);
                        auto* ev=(next()%2 ? evA : evB).data(); CVector d{uni(-1.2f,1.2f),uni(-1.2f,1.2f),uni(-1.2f,1.2f)}; if(next()%10==0)d.x=std::bit_cast<float>(QNAN);
                        At<CVector>(o,0x20)=At<CVector>(ev,0x20)+d;
                        if(next()%2)At<uintptr_t>(ev,EV_SRC)=At<uintptr_t>(o,0x14);
                        if(nm=="CloneEditable"){ At<uint32>(o,0x1C)=next()%3 ? next() : uint32(-1); if(At<uint32>(o,0x1C)==uint32(-1)&&!At<uintptr_t>(o,0x14))At<uintptr_t>(o,0x14)=B; }
                        break; }
                    case VCOLL: At<int16>(o,0xC)=int16(next()); At<int16>(o,0xE)=next()%6 ? 0 : int16(pick({1,2,uint32(-1)})); At<float>(o,0x10)=argF;
                        At<uintptr_t>(o,0x14)=pick({VA,VB,0,VA}); At<CVector>(o,0x18)=vec(1.f); At<CVector>(o,0x24)=v2; At<uint8>(o,0x30)=uint8(next()); At<uint8>(o,0x31)=uint8(next());
                        if(next()%3==0){ const CVector fw=At<CVector>(matA.data(),0x10); At<CVector>(o,0x18)=CVector{-fw.x,-fw.y,-fw.z}+vec(0.3f); }
                        break;
                    case VDIED: At<uintptr_t>(o,0xC)=nm=="Clone" ? argVehNN : pick({VA,VB,0,VA}); At<uint8>(o,0x10)=uint8(next()%2); break;
                    case HITRUN: At<uintptr_t>(o,0xC)=argPedNN; At<uintptr_t>(o,0x10)=argVehNN; break;
                    case FIRE: At<uintptr_t>(o,0x14)=pick({VA,VB,0,VA});
                        if(next()%2){ auto* v=(At<uintptr_t>(o,0x14)==VB ? vehB : vehA).data(); At<void*>(v,E_MATRIX)=nullptr;
                            CVector d=vec(next()%2 ? 12.f : 7.f); if(next()%12==0)d.z=std::bit_cast<float>(QNAN); At<CVector>(v,E_POS)=At<CVector>(pedA.data(),E_POS)+d; }
                        break;
                    default: break;
                    }
                    CEvent* const other=reinterpret_cast<CEvent*>(next()%2 ? evA.data() : evB.data());
                    for(auto* h:hooks)h->State(candidate);
                    // VMT: la entrada de la vtable del EXE cambia con el hook; estatico: la entrada del EXE salta a la DLL
                    const uintptr_t call=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((call>=0x400000&&call<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    using V=void*; using CV=const CVector&;
                    if(isCtor) switch(fn.c){
                    case EVENT: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V)>(call)(o)); break;
                    case AREA: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t)>(call)(o,argPed)); break;
                    case PCWP: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uint32,float,uintptr_t,CV,CV,uint32,uint32)>(call)(o,argW1,argF,argPed,v1,v2,argW2,argW3)); break;
                    case WIPED: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t,CV,uint32)>(call)(o,argPedNN,v1,argW1%9)); break;
                    case SOUND: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t,float,uint32,CV)>(call)(o,sqEnt,argF,argTime,v1)); break;
                    case VCOLL: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uint32,float,uintptr_t,CV,CV,uint32,uint32)>(call)(o,argW1,argF,argVeh,v1,v2,argW2,argW3)); break;
                    case VDIED: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t)>(call)(o,argVehNN)); break;
                    case HITRUN: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t,uintptr_t)>(call)(o,argPedNN,argVehNN)); break;
                    case FIRE: rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V,uintptr_t)>(call)(o,argVeh)); break;
                    default: break;
                    }
                    else if(nm=="CalcSoundLevelIncrement") ret=F(reinterpret_cast<float(__cdecl*)(float,float)>(call)(lv1,lv2));
                    else if(nm=="GetSoundLevel") ret=F(reinterpret_cast<float(__thiscall*)(V,uintptr_t,CVector*)>(call)(fn.c==GLOBAL ? V(ggroup.data()) : o,entArg,&posArg));
                    else if(nm=="AddEventsToGroup") reinterpret_cast<void(__thiscall*)(V,V)>(call)(ggroup.data(),group.data());
                    else if(nm=="AddEventsToPed") reinterpret_cast<void(__thiscall*)(V,CPed*)>(call)(ggroup.data(),groupPed);
                    else if(nm=="AffectsPed") ret=reinterpret_cast<bool(__thiscall*)(V,CPed*)>(call)(o,p);
                    else if(nm=="TakesPriorityOver") ret=reinterpret_cast<bool(__thiscall*)(V,CEvent*)>(call)(o,other);
                    else if(nm=="ReportCriminalEvent") reinterpret_cast<void(__thiscall*)(V,CPed*)>(call)(o,p);
                    else rawRet=uintptr_t(reinterpret_cast<V(__thiscall*)(V)>(call)(o));   // Clone / CloneEditable
                    for(auto* h:hooks)h->State(false);
                    if(ret.is_null())ret=Name(rawRet);
                    json delList=json::array(); for(size_t k=0;k<ndel;++k)delList.push_back({Name(dels[k].at),dels[k].flags});
                    json objectWords=Region(o,object.size(),isCtor);
                    if(isCtor){ const uintptr_t vp=At<uintptr_t>(o,0);
                        if(fn.c==EVENT)objectWords.push_back(vp==fn.vt||vp<0x400000||vp>=0x1000000 ? json("abstract") : Name(vp));   // CEvent es abstracto (vtable del EXE o de la DLL)
                        else objectWords.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType())); }   // vptr EXE o DLL: se compara el tipo
                    // clon nuevo del pool de eventos (Clone/CloneEditable): tipo + bytes, luego se libera
                    json clone=nullptr; void* fresh=nullptr;
                    { const auto now=EvOccupancy(); for(size_t k=0;k<now.size();++k)if(now[k]&&!occ[k]){ fresh=EvStorage()+k*EVENT_SLOT; break; } }
                    if(fresh)clone={{"at",Name(fresh)},{"type",int(reinterpret_cast<CEvent*>(fresh)->GetEventType())},{"words",Region(fresh,EVENT_SLOT,true)}};
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),static_cast<CEntity*>(PedC()),static_cast<CEntity*>(VehA()),static_cast<CEntity*>(VehB())})refs.push_back(refList(e));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Hash(fakes[n].data()+4,fakes[n].size()-4));
                    json gevWords=json::array(); for(int n=0;n<GEV;++n)gevWords.push_back(Hash(gev[n].data()+4,EV_SIZE-4));
                    for(int n=0;n<CLONES;++n)gevWords.push_back(Hash(clones[n].data()+4,EV_SIZE-4));
                    json r={{"ret",ret},{"object",objectWords},{"clone",clone},{"refs",refs},{"dels",delList},
                            {"peds",{EntHash(pedA),EntHash(pedB),EntHash(pedC)}},{"vehs",{EntHash(vehA),EntHash(vehB)}},
                            {"intel",{Hash(intelA.data(),intelA.size()),Hash(intelB.data(),intelB.size())}},{"fakes",fakeWords},
                            {"events",{Region(evA.data(),evA.size(),true),Region(evB.data(),evB.size(),true)}},{"gev",gevWords},
                            {"ggroup",Region(ggroup.data(),ggroup.size())},{"group",Hash(group.data(),group.size())},{"wanted",Hash(wanted.data(),wanted.size())},
                            {"mats",{Hash(matA.data(),matA.size()),Hash(matB.data(),matB.size()),Hash(matC.data(),matC.size()),Hash(matVA.data(),matVA.size()),Hash(matVB.data(),matVB.size())}},
                            {"pos",{F(posArg.x),F(posArg.y),F(posArg.z)}},{"time",timeMs}};
                    r["trace"]=trace; r["switches"]=switches;
                    CleanRefs();
                    if(fresh)GetEventPool()->Delete(static_cast<CEvent*>(fresh));
                    r["clean"]=EvOccupancy()==occ;
                    restoreEvents();
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
    CleanRefs(); restoreEvents();
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",EvOccupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime
        &&std::memcmp(EvHdr(),savedEvHdr.data(),savedEvHdr.size())==0}});
}
}
