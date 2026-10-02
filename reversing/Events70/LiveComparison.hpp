#pragma once
// Banco dentro del motor: 21 clases de eventos de la lista de 70 (42 funciones), constructor + virtual de cada una.
// Constructores (llamados en la direccion del EXE con `this` = buffer propio rellenado con 0xCD, comparado entero con el
// tipo de evento en vez del vptr), AffectsPed, AffectsPedGroup, IsValid (CEventLeanOnVehicle y CEventPassObject) y
// CloneEditable (CEventVehicleDamageCollision/Weapon; el clon sale del pool de eventos, se compara tipo + bytes, se
// borra y el pool se restaura entero). Los metodos virtuales se llaman por la entrada de la vtable del EXE con el
// evento construido a mano sobre la vtable del EXE.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: tres peatones, dos vehiculos, un
// objeto (modelo y modelo de colision falsos colocados temporalmente en ms_modelInfoPtrs), dos inteligencias con
// tareas falsas (vtable propia: GetSubTask/GetTaskType/IsSimple), eventos falsos para el historial de eventos y un
// CPedGroup falso (pertenencia en +8, lider en el miembro 7 = +0x28, evento actual del grupo en +0x34).
// Se simulan y trazan: FindPlayerPed, CTheScripts::IsPlayerOnAMission, CPedGroups::AreInSameGroup,
// CPedGeometryAnalyser::IsInAir/ComputeClosestSurfacePoint/ComputeEntityHitSide, CTaskComplexEnterCar::GetTargetPos y
// CTaskComplexWalkRoundCar::SetNewVehicle. El reloj, el bit bPlayerSafe de CPad::Pads[0], la entrada de
// ms_modelInfoPtrs y el pool de eventos (cabecera, estados y almacenamiento) se guardan y se restauran.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventAcquaintancePedHateBadlyLit.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventGunShotWhizzedBy.h"
#include "Events/EventHitByWaterCannon.h"
#include "Events/EventInAir.h"
#include "Events/EventLeaderEnteredCarAsDriver.h"
#include "Events/EventLeaderExitedCarAsDriver.h"
#include "Events/EventLeaderQuitEnteringCarAsDriver.h"
#include "Events/EventLeanOnVehicle.h"
#include "Events/EventObjectCollision.h"
#include "Events/EventPassObject.h"
#include "Events/EventPlayerCommandToGroup.h"
#include "Events/EventPlayerCommandToGroupAttack.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "Events/EventPotentialWalkIntoVehicle.h"
#include "Events/EventSeenPanickedPed.h"
#include "Events/EventVehicleDamageCollision.h"
#include "Events/EventVehicleDamageWeapon.h"
#include "Events/EventVehicleToSteal.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundCar.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "Scripts/TheScripts.h"
#include "Pools/Pools.h"
#include "Pools/EventPool.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"

namespace Events70Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_FLAGS=0x1C, E_MODEL=0x22, E_AREA=0x2F, PH_FLAGS=0x40, PH_DAMAGE=0xDC, PH_ATTACHED=0xFC,
                 P_FLAGS1=0x46C, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_CREATED=0x484, P_STATE=0x530, P_VEHICLE=0x58C,
                 P_TYPE=0x598, P_FIRE=0x730, V_TOWING=0x4C4, V_TOWED=0x4C8, V_TYPE=0x590, V_SUBTYPE=0x594;
constexpr size_t I_PRIMARY=0x04, I_SECONDARY=0x18, I_HIST_NONTEMP=0x3C, I_HIST_TEMP=0x40;   // CPedIntelligence
constexpr size_t G_ENTER=0x04, G_MEMBERS=0x0C, G_CUR_EVENT=0x34;                            // CPedGroup (miembro 7 = lider)
constexpr size_t FK_TYPE=0x38, FK_SUB=0x3C;                                                 // tarea/evento falsos
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_MI_PTRS=0xA9B0C8, PURECALL=0x82263A, EVENT_SLOT=0x44;
constexpr int16 MI_FAKE=1;            // entrada de ms_modelInfoPtrs sustituida durante cada llamada
constexpr int FAKES=12;

alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> vehA{}, vehB{};
alignas(16) inline std::array<uint8,sizeof(CObject)> objA{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,0x40> groupEvent{}, evA{}, evB{}, fire{}, mi{}, col{}, object{}, cmdProbe{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
inline CPed* Ped(std::array<uint8,sizeof(CPed)>& b) { return reinterpret_cast<CPed*>(b.data()); }
inline CPed* PedA() { return Ped(pedA); } inline CPed* PedB() { return Ped(pedB); } inline CPed* PedC() { return Ped(pedC); }
inline CVehicle* VehA() { return reinterpret_cast<CVehicle*>(vehA.data()); } inline CVehicle* VehB() { return reinterpret_cast<CVehicle*>(vehB.data()); }
inline CObject* ObjA() { return reinterpret_cast<CObject*>(objA.data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pool de eventos (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot, banderas)
inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
#define EV70_IN(buf,nm) if(auto s=in(buf.data(),buf.size(),nm))return *s;
    EV70_IN(pedA,"pedA") EV70_IN(pedB,"pedB") EV70_IN(pedC,"pedC") EV70_IN(vehA,"vehA") EV70_IN(vehB,"vehB") EV70_IN(objA,"objA")
    EV70_IN(intelA,"intelA") EV70_IN(intelB,"intelB") EV70_IN(group,"group") EV70_IN(groupEvent,"groupEvent") EV70_IN(evA,"evA")
    EV70_IN(evB,"evB") EV70_IN(fire,"fire") EV70_IN(mi,"mi") EV70_IN(col,"col") EV70_IN(object,"object")
#undef EV70_IN
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
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
inline uintptr_t playerRet{}; inline bool onMission{}, sameGroup{}, inAir{}; inline CVector surfacePt{}, targetPos{}; inline uint32 hitSalt{};

// tareas falsas: lectores puros (no se trazan: la DLL y el EXE pueden llamarlos un numero distinto de veces)
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask* t,void*) { return At<CTask*>(t,FK_SUB)==nullptr; }
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(t)}); return nullptr; }
inline std::array<void*,16> taskVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// eventos falsos: GetEventType (hueco 1) y GetSourceEntity (hueco 10)
inline int __fastcall EType(void* e,void*) { return At<int32>(e,FK_TYPE); }
inline CEntity* __fastcall ESource(void* e,void*) { return At<CEntity*>(e,FK_SUB); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
inline std::array<void*,20> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline bool __cdecl OnMission() { trace.push_back({"is_player_on_a_mission"}); return onMission; }
inline bool __cdecl SameGroup(const CPed* a,const CPed* b) { trace.push_back({"are_in_same_group",Name(a),Name(b)}); return sameGroup; }
inline bool __cdecl IsInAir(const CPed& p) { trace.push_back({"is_in_air",Name(&p)}); return inAir; }
inline bool __cdecl ClosestSurface(const CPed& p,CEntity& e,CVector& out) { trace.push_back({"closest_surface_point",Name(&p),Name(&e)}); out=surfacePt; return true; }
inline int32 __cdecl HitSide(const CVector& pt,CEntity& e) {
    trace.push_back({"entity_hit_side",F(pt.x),F(pt.y),F(pt.z),Name(&e)});
    return int32(((F(pt.x)*2654435761u)^(F(pt.y)*40503u)^F(pt.z)^hitSalt)%4u);
}
inline CVector* __fastcall TargetPos(void* t,void*,CVector* out) { trace.push_back({"enter_car_target_pos",Name(t)}); *out=targetPos; return out; }
inline void __fastcall SetNewVeh(void* t,void*,CVehicle* v,uint32 dir) { trace.push_back({"walk_round_car_set_new_vehicle",Name(t),Name(v),dir&0xFF}); }

inline std::vector<bool> EvOccupancy() { std::vector<bool> r(EvCap()); for(size_t k=0;k<r.size();++k)r[k]=!(EvStates()[k]&0x80); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Cls { BADLY, UPSIDE, COPCAR, WHIZZ, WATER, INAIR, LENTER, LEXIT, LQUIT, LEAN, OBJCOLL, PASS, CMD, CMDATK, WFIRE, WOBJ, WVEH, SEEN, VDC, VDW, STEAL };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; Cls c; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CEventAcquaintancePedHateBadlyLit","Constructor",0x5FF250,0x86CB10,-1,128,BADLY},
    {"CEventAcquaintancePedHateBadlyLit","AffectsPed",0,0x86CB10,5,512,BADLY},
    {"CEventCarUpsideDown","Constructor",0x4B1CC0,0x85B660,-1,128,UPSIDE},
    {"CEventCarUpsideDown","AffectsPed",0,0x85B660,5,512,UPSIDE},
    {"CEventCopCarBeingStolen","Constructor",0x4B1740,0x85B548,-1,128,COPCAR},
    {"CEventCopCarBeingStolen","AffectsPed",0,0x85B548,5,512,COPCAR},
    {"CEventGunShotWhizzedBy","Constructor",0x4B1FD0,0x85B730,-1,128,WHIZZ},
    {"CEventGunShotWhizzedBy","AffectsPed",0,0x85B730,5,512,WHIZZ},
    {"CEventHitByWaterCannon","Constructor",0x4B1290,0x85B488,-1,128,WATER},
    {"CEventHitByWaterCannon","AffectsPed",0,0x85B488,5,512,WATER},
    {"CEventInAir","Constructor",0x4B0CB0,0x85B3C0,-1,64,INAIR},
    {"CEventInAir","AffectsPed",0,0x85B3C0,5,512,INAIR},
    {"CEventLeaderEnteredCarAsDriver","Constructor",0x48E1C0,0x85A198,-1,128,LENTER},
    {"CEventLeaderEnteredCarAsDriver","AffectsPedGroup",0,0x85A198,6,512,LENTER},
    {"CEventLeaderExitedCarAsDriver","Constructor",0x4B8300,0x85B898,-1,64,LEXIT},
    {"CEventLeaderExitedCarAsDriver","AffectsPedGroup",0,0x85B898,6,512,LEXIT},
    {"CEventLeaderQuitEnteringCarAsDriver","Constructor",0x63A110,0x86E6B0,-1,64,LQUIT},
    {"CEventLeaderQuitEnteringCarAsDriver","AffectsPedGroup",0,0x86E6B0,6,256,LQUIT},
    {"CEventLeanOnVehicle","Constructor",0x65DAF0,0x86F878,-1,128,LEAN},
    {"CEventLeanOnVehicle","IsValid",0,0x86F878,14,256,LEAN},
    {"CEventObjectCollision","Constructor",0x4ACCF0,0x85ACA8,-1,128,OBJCOLL},
    {"CEventObjectCollision","AffectsPed",0,0x85ACA8,5,512,OBJCOLL},
    {"CEventPassObject","Constructor",0x65DC70,0x86F8B8,-1,128,PASS},
    {"CEventPassObject","IsValid",0,0x86F8B8,14,256,PASS},
    {"CEventPlayerCommandToGroup","Constructor",0x4B23D0,0x85B800,-1,128,CMD},
    {"CEventPlayerCommandToGroup","AffectsPedGroup",0,0x85B800,6,512,CMD},
    {"CEventPlayerCommandToGroupAttack","Constructor",0x5F6340,0x86C710,-1,128,CMDATK},
    {"CEventPlayerCommandToGroupAttack","AffectsPedGroup",0,0x86C710,6,512,CMDATK},
    {"CEventPotentialWalkIntoFire","Constructor",0x4B1E20,0x85B6A0,-1,256,WFIRE},
    {"CEventPotentialWalkIntoFire","AffectsPed",0,0x85B6A0,5,512,WFIRE},
    {"CEventPotentialWalkIntoObject","Constructor",0x4AE5D0,0x85AF18,-1,128,WOBJ},
    {"CEventPotentialWalkIntoObject","AffectsPed",0,0x85AF18,5,1024,WOBJ},
    {"CEventPotentialWalkIntoVehicle","Constructor",0x4AE320,0x85AED0,-1,128,WVEH},
    {"CEventPotentialWalkIntoVehicle","AffectsPed",0,0x85AED0,5,1024,WVEH},
    {"CEventSeenPanickedPed","Constructor",0x4B2080,0x85B778,-1,128,SEEN},
    {"CEventSeenPanickedPed","AffectsPed",0,0x85B778,5,512,SEEN},
    {"CEventVehicleDamageCollision","Constructor",0x6A0620,0x8710D0,-1,128,VDC},
    {"CEventVehicleDamageCollision","CloneEditable",0,0x8710D0,16,256,VDC},
    {"CEventVehicleDamageWeapon","Constructor",0x61C2E0,0x86D640,-1,128,VDW},
    {"CEventVehicleDamageWeapon","CloneEditable",0,0x86D640,16,256,VDW},
    {"CEventVehicleToSteal","Constructor",0x4AF670,0x85B210,-1,128,STEAL},
    {"CEventVehicleToSteal","AffectsPed",0,0x85B210,5,512,STEAL},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(PedC()); clean(VehA()); clean(VehB()); clean(ObjA());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"events70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"events70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("events70");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"events70-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,16>{}); taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[3]=reinterpret_cast<void*>(&FSimple); taskVtbl[4]=reinterpret_cast<void*>(&FType);
    FillETraps(std::make_integer_sequence<int,20>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[10]=reinterpret_cast<void*>(&ESource);
    // vtable de CEventPlayerCommandToGroup en la DLL (la del EXE es abstracta: GetEventType es _purecall)
    const uintptr_t dllCmdVt=*reinterpret_cast<uintptr_t*>(::new (cmdProbe.data()) CEventPlayerCommandToGroup(PLAYER_GROUP_COMMAND_ATTACK,nullptr));
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto restoreEvents=[&]{ std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size());
                            std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size()); };
    const auto occupancyStart=EvOccupancy();
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& padByte=*(reinterpret_cast<uint8*>(&CPad::Pads[0])+0x10E); const uint8 savedPad=padByte;
    auto& miSlot=reinterpret_cast<void**>(ADDR_MI_PTRS)[MI_FAKE]; void* const savedMi=miSlot;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), s1e(0x56E210,FindPlayer);
        EntryPatch s2(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),OnMission), s2e(0x464D50,OnMission);
        EntryPatch s3e(0x5F7F40,SameGroup), s4e(0x5F1CB0,IsInAir), s5e(0x5F3AC0,HitSide);   // envolturas de la DLL que llaman al EXE
        EntryPatch s6(bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,CEntity&,CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint)),ClosestSurface), s6e(0x5F3B70,ClosestSurface);
        EntryPatch s7(bit_cast<uintptr_t>(&CTaskComplexEnterCar::GetTargetPos),TargetPos), s7e(0x63A300,TargetPos);
        EntryPatch s8(bit_cast<uintptr_t>(&CTaskComplexWalkRoundCar::SetNewVehicle),SetNewVeh), s8e(0x654290,SetNewVeh);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7070u; if(!st)st=1; trace=json::array();
                    CleanRefs(); restoreEvents();
                    // pool de eventos: los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto)
                    for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    const auto occ=EvOccupancy();
                    const uintptr_t A=uintptr_t(PedA()), B=uintptr_t(PedB()), C=uintptr_t(PedC()), VA=uintptr_t(VehA()), VB=uintptr_t(VehB()), OA=uintptr_t(ObjA());
                    // peatones
                    const CVector center=vec(20.f);
                    for(auto* buf:{&pedA,&pedB,&pedC}){ auto* p=buf->data(); std::memset(p,0,buf->size()); reinterpret_cast<CEntity*>(p)->SetType(ENTITY_TYPE_PED);
                        At<CVector>(p,E_POS)=center+CVector{uni(-12.f,12.f),uni(-12.f,12.f),uni(-2.f,2.f)};
                        At<uint8>(p,E_FLAGS)=uint8(next()|(next()%4 ? 1u : 0u)); At<uint8>(p,E_AREA)=next()%3 ? 0 : uint8(next()%3);
                        At(p,P_TYPE)=uint32(pick({0,1,4,6,6,20,20,7,8})); At(p,P_STATE)=uint32(pick({1,1,1,0x36,0x37,0,7,0x32}));
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next(); At<uint8>(p,P_CREATED)=uint8(pick({1,2,2,0}));
                        At<uintptr_t>(p,P_VEHICLE)=pick({0,VA,VA,VB}); At<void*>(p,P_FIRE)=next()%5 ? nullptr : fire.data();
                        At<uintptr_t>(p,PH_ATTACHED)=next()%5 ? 0 : pick({VA,OA}); At<uintptr_t>(p,PH_DAMAGE)=next()%5 ? 0 : pick({VA,B}); }
                    At<void*>(pedA.data(),P_INTEL)=intelA.data(); At<void*>(pedB.data(),P_INTEL)=intelB.data(); At<void*>(pedC.data(),P_INTEL)=intelB.data();
                    // vehiculos y objeto
                    for(auto* buf:{&vehA,&vehB}){ auto* v=buf->data(); std::memset(v,0,buf->size()); reinterpret_cast<CEntity*>(v)->SetType(ENTITY_TYPE_VEHICLE);
                        At<CVector>(v,E_POS)=center+vec(6.f); At(v,V_TYPE)=uint32(pick({0,0,2,4,5,9,3,6})); At<int32>(v,V_SUBTYPE)=int32(pick({0,0,0,uint32(-1),4,7,8,9,5}));
                        At<uintptr_t>(v,V_TOWING)=next()%3 ? 0 : pick({VA,VB}); At<uintptr_t>(v,V_TOWED)=next()%3 ? 0 : pick({VA,VB}); }
                    std::memset(objA.data(),0,objA.size()); ObjA()->SetType(ENTITY_TYPE_OBJECT); At<CVector>(objA.data(),E_POS)=center+vec(4.f);
                    At(objA.data(),PH_FLAGS)=next(); At<uintptr_t>(objA.data(),PH_ATTACHED)=next()%4 ? 0 : pick({A,B}); At<int16>(objA.data(),E_MODEL)=MI_FAKE;
                    for(auto& b:mi)b=uint8(next()); At<void*>(mi.data(),0x14)=col.data();
                    // caja del modelo de colision: minimo en +0, maximo en +0xC (medidas pequenas, cero y NaN incluidas)
                    for(int k=0;k<3;++k){ static const float sz[]{0.f,0.005f,0.01f,0.0099f}; const float lo=uni(-3.f,3.f);
                        At<float>(col.data(),4*k)=lo; At<float>(col.data(),0xC+4*k)=next()%12==0 ? std::bit_cast<float>(0x7FC00000u) : lo+(next()%3 ? uni(0.f,4.f) : sz[next()%4]); }
                    for(auto& b:fire)b=uint8(next());
                    // tareas falsas
                    static const uint32 taskTypes[]{0xD0,0x6C,0x201,0xF0,0xCF,0x2C0,0x2BC,0x2CE,0x2BD,0x1FC,0x1FE,0x3AB,0x384,0x39E,0xCB,0x190};
                    for(int n=0;n<FAKES;++n){ auto* t=fakes[n].data(); for(auto& b:fakes[n])b=uint8(next()); At<void*>(t,0)=taskVtbl.data(); At<void*>(t,4)=nullptr;
                        const uint32 ty=taskTypes[next()%std::size(taskTypes)]; At<uint32>(t,FK_TYPE)=ty; At<CTask*>(t,FK_SUB)=n<FAKES-1&&next()%3==0 ? Fake(FAKES-1) : nullptr;
                        if(ty==0x2BD)At<uintptr_t>(t,0xC)=pick({VA,VB,0});
                        else if(ty==0x3AB)At<uintptr_t>(t,0xC)=pick({OA,0,VA});
                        else if(ty==0x384||ty==0x39E)At<CVector>(t,0xC)=center+vec(12.f);
                        if(ty==0x1FE)At<uintptr_t>(t,0x1C)=pick({OA,OA,0,VA}); else if(ty==0x1FC)At<uintptr_t>(t,0x1C)=pick({VA,VB,0}); }
                    // inteligencias: tareas primarias (+4), secundarias (+0x18) e historial de eventos (+0x38)
                    for(auto* buf:{&intelA,&intelB}){ std::memset(buf->data(),0,buf->size()); auto* in=buf->data();
                        for(int k=0;k<5;++k)At<CTask*>(in,I_PRIMARY+4*k)=next()%3==0 ? Fake(int(next()%FAKES)) : nullptr;
                        for(int k=0;k<6;++k)At<CTask*>(in,I_SECONDARY+4*k)=next()%4==0 ? Fake(int(next()%FAKES)) : nullptr;
                        At<void*>(in,I_HIST_TEMP)=next()%3==0 ? (next()%2 ? evA.data() : evB.data()) : nullptr;
                        At<void*>(in,I_HIST_NONTEMP)=next()%2 ? (next()%2 ? evA.data() : evB.data()) : nullptr; }
                    if(fn.c==WVEH&&!isCtor){ bool any=false; for(int k=0;k<5;++k)any|=At<CTask*>(intelA.data(),I_PRIMARY+4*k)!=nullptr;
                        if(!any)At<CTask*>(intelA.data(),I_PRIMARY+4*int(next()%5))=Fake(int(next()%FAKES)); }   // el EXE no comprueba null en la tarea mas simple
                    // eventos falsos del historial
                    const uint32 t0=next();
                    for(auto* buf:{&evA,&evB}){ auto* e=buf->data(); for(auto& b:*buf)b=uint8(next()); At<void*>(e,0)=eventVtbl.data();
                        At<uint32>(e,FK_TYPE)=uint32(pick({63,63,63,54,54,13,uint32(next()%100)})); At<uintptr_t>(e,FK_SUB)=pick({B,C,0,A,B});
                        At<uint32>(e,0x18)=t0; At<CVector>(e,0x1C)=center+vec(2.f); }
                    // grupo
                    std::memset(group.data(),0,group.size()); At<uint8>(group.data(),G_ENTER)=next()%4 ? 1 : uint8(next()%2);
                    for(int k=0;k<8;++k)At<uintptr_t>(group.data(),G_MEMBERS+4*k)=next()%3 ? 0 : pick({A,B,C});
                    At<uintptr_t>(group.data(),G_MEMBERS+4*7)=pick({A,B,C,C,0});
                    if(fn.c==LEXIT&&!At<uintptr_t>(group.data(),G_MEMBERS+4*7))At<uintptr_t>(group.data(),G_MEMBERS+4*7)=C;   // el EXE no comprueba null en el lider
                    for(auto& b:groupEvent)b=uint8(next()); At<void*>(groupEvent.data(),0x10)=next()%2 ? evA.data() : evB.data();
                    At<void*>(group.data(),G_CUR_EVENT)=next()%4 ? groupEvent.data() : nullptr;
                    // dependencias simuladas
                    playerRet=pick({B,C}); onMission=next()%3==0; sameGroup=next()%4==0; inAir=next()%4!=0;
                    surfacePt=center+vec(5.f); targetPos=center+vec(8.f); hitSalt=next();
                    padByte=uint8((savedPad&~0x20u)|(next()%3==0 ? 0x20u : 0u)); timeMs=next();
                    // evento probado
                    void* const o=object.data(); CPed* const p=PedA();
                    if(isCtor)std::memset(o,0xCD,object.size());
                    else { for(auto& b:object)b=uint8(next()); At<uintptr_t>(o,0)=fn.vt; At<int32>(o,4)=next()%3 ? int32(next()%5)-2 : int32(next()); At<uint8>(o,8)=next()%2 ? uint8(next()%2) : uint8(next()); }
                    // argumentos de los constructores
                    const CVector v1=center+vec(10.f), v2=center+vec(10.f); const float argF=next()%8 ? uni(-1.f,4.f) : (next()%2 ? std::bit_cast<float>(0x7FC00000u) : 2.f);
                    const int32 argI=next()%2 ? int32(next()%8) : int32(next()); const int16 argS1=int16(next()), argS2=int16(next()); const bool argB=next()%2;
                    const uintptr_t argPed=pick({A,B,0,C}), argVeh=pick({VA,VB,0}), argEnt=pick({A,VA,OA,0});
                    int32 argTime=next()%3 ? -1 : int32(next()); CPed* ctorPed=reinterpret_cast<CPed*>(argPed); if(argTime==-1&&!ctorPed)ctorPed=PedB();
                    // campos del evento para los metodos virtuales
                    if(!isCtor) switch(fn.c){
                    case BADLY: { At<uintptr_t>(o,0x14)=pick({B,C,A,0,B});
                        static const int32 deltas[]{2000,1999,2001,0,-1,-2000,int32(0x80000000),5000,0x7FFFFFFF};
                        At<uint32>(o,0x18)=t0+uint32(next()%2 ? deltas[next()%std::size(deltas)] : int32(next()%10000)-5000);
                        auto* ev=(next()%2 ? evA : evB).data(); CVector d{uni(-1.2f,1.2f),uni(-1.2f,1.2f),uni(-1.2f,1.2f)}; if(next()%10==0)d.x=std::bit_cast<float>(0x7FC00000u);
                        At<CVector>(o,0x1C)=At<CVector>(ev,0x1C)+d; break; }
                    case UPSIDE: At<uintptr_t>(o,0xC)=pick({VA,VB,0,VA});
                        if(At<uint8>(p,P_CREATED)!=2&&!At<uintptr_t>(p,P_VEHICLE))At<uintptr_t>(p,P_VEHICLE)=VB;   // el EXE solo comprueba null en peatones de mision
                        break;
                    case STEAL: At<uintptr_t>(o,0xC)=pick({VA,VB,0,VA}); break;
                    case COPCAR: At<uintptr_t>(o,0xC)=pick({B,B,0,C}); At<uintptr_t>(o,0x10)=pick({VA,VB,0,VA}); break;
                    case WHIZZ: { At<uintptr_t>(o,0x14)=argEnt; const CVector s=At<CVector>(p,E_POS)+vec(15.f); CVector d=vec(1.f); if(next()%10==0)d=CVector{0.f,0.f,0.f};
                        At<CVector>(o,0x18)=s; At<CVector>(o,0x24)=s+d*uni(1.f,40.f); At<uint8>(o,0x30)=uint8(next()%2);
                        if(next()%2){ const CVector e=At<CVector>(o,0x24); At<CVector>(p,E_POS)=s+(e-s)*uni(-0.5f,1.5f)+vec(3.f); } break; }
                    case OBJCOLL: At<uintptr_t>(o,0x14)=pick({OA,OA,0}); break;
                    case WFIRE: { At<CVector>(o,0x14)=At<CVector>(p,E_POS)+vec(6.f); At<float>(o,0x20)=uni(0.f,3.f); At<float>(o,0x24)=uni(0.3f,3.f); At<int32>(o,0x28)=int32(pick({1,4,6,7,0}));
                        for(int n=0;n<FAKES;++n)if(next()%2){ At<uint32>(fakes[n].data(),FK_TYPE)=pick({0x384,0x39E}); At<CVector>(fakes[n].data(),0xC)=At<CVector>(o,0x14)+vec(6.f); } break; }
                    case WOBJ: At<uintptr_t>(o,0x14)=pick({OA,OA,OA,0}); At<int32>(o,0x18)=int32(pick({4,6,1,7})); break;
                    case WVEH: At<uintptr_t>(o,0x14)=pick({VA,VB,VA,0}); At<int32>(o,0x18)=int32(pick({4,6,1,7}));
                        // sin vehiculo, una CTaskComplexWalkRoundCar activa haria que el EXE leyera el remolque de null
                        if(!At<uintptr_t>(o,0x14))for(int n=0;n<FAKES;++n)if(At<uint32>(fakes[n].data(),FK_TYPE)==0x1FC)At<uint32>(fakes[n].data(),FK_TYPE)=0x2C0;
                        break;
                    case SEEN: At<uintptr_t>(o,0x14)=pick({B,C,A,0,B}); break;
                    case LENTER: At<uintptr_t>(o,0x14)=pick({VA,VB,0,VA}); break;
                    case CMD: case CMDATK: At<int32>(o,0x14)=int32(next()%3); At<uintptr_t>(o,0x18)=pick({B,0}); break;
                    case VDC: case VDW: At<uintptr_t>(o,0x14)=pick({VA,VB,0}); At<uintptr_t>(o,0x18)=pick({B,VA,0,A}); At<int32>(o,0x1C)=int32(next()%60); break;
                    default: break;
                    }
                    CPed* const validPed=next()%2 ? PedA() : nullptr;
                    miSlot=mi.data();
                    for(auto* h:hooks)h->State(candidate);
                    // VMT: la entrada de la vtable del EXE cambia con el hook; estatico: la entrada del EXE salta a la DLL
                    const uintptr_t call=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((call>=0x400000&&call<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    using Th1=void*(__thiscall*)(void*,void*);
                    if(isCtor) switch(fn.c){
                    case BADLY: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32,const CVector&)>(call)(o,ctorPed,argTime,v1)); break;
                    case UPSIDE: case LENTER: case STEAL: rawRet=uintptr_t(reinterpret_cast<Th1>(call)(o,reinterpret_cast<void*>(argVeh))); break;
                    case COPCAR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*)>(call)(o,reinterpret_cast<void*>(argPed),reinterpret_cast<void*>(argVeh))); break;
                    case WHIZZ: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,const CVector&,const CVector&,bool)>(call)(o,reinterpret_cast<void*>(argEnt),v1,v2,argB)); break;
                    case WATER: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const CVector&,const CVector&)>(call)(o,v1,v2)); break;
                    case INAIR: case LEXIT: case LQUIT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(call)(o)); break;
                    case LEAN: case WOBJ: case WVEH: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,int32)>(call)(o,reinterpret_cast<void*>(fn.c==WOBJ ? pick({OA,0}) : argVeh),argI)); break;
                    case OBJCOLL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int16,float,void*,const CVector&,const CVector&,int16)>(call)(o,argS1,argF,reinterpret_cast<void*>(pick({OA,0})),v1,v2,argS2)); break;
                    case PASS: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,bool)>(call)(o,reinterpret_cast<void*>(argPed),argB)); break;
                    case CMD: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,void*)>(call)(o,argI,reinterpret_cast<void*>(argPed))); break;
                    case CMDATK: case SEEN: rawRet=uintptr_t(reinterpret_cast<Th1>(call)(o,reinterpret_cast<void*>(argPed))); break;
                    case WFIRE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const CVector&,float,int32)>(call)(o,v1,argF,argI)); break;
                    case VDC: case VDW: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*,int32)>(call)(o,reinterpret_cast<void*>(argVeh),reinterpret_cast<void*>(argEnt),argI)); break;
                    }
                    else if(nm=="AffectsPed") ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(call)(o,p);
                    else if(nm=="AffectsPedGroup") ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(call)(o,group.data());
                    else if(nm=="IsValid") ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(call)(o,validPed);
                    else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(call)(o));
                    for(auto* h:hooks)h->State(false);
                    miSlot=savedMi;
                    if(ret.is_null())ret=Name(rawRet);
                    json objectWords=Region(o,object.size(),isCtor);
                    if(isCtor){ const uintptr_t vp=At<uintptr_t>(o,0);
                        if((vp==fn.vt&&reinterpret_cast<uintptr_t*>(vp)[1]==PURECALL)||vp==dllCmdVt)objectWords.push_back(vp==dllCmdVt||vp==0x85B800 ? "CEventPlayerCommandToGroup" : "abstract");
                        else objectWords.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType())); }   // vptr EXE o DLL: se compara el tipo
                    // clon nuevo del pool de eventos (CloneEditable): tipo + bytes, luego se libera
                    json clone=nullptr; void* fresh=nullptr;
                    { const auto now=EvOccupancy(); for(size_t k=0;k<now.size();++k)if(now[k]&&!occ[k]){ fresh=EvStorage()+k*EVENT_SLOT; break; } }
                    if(fresh)clone={{"at",Name(fresh)},{"type",int(reinterpret_cast<CEvent*>(fresh)->GetEventType())},{"words",Region(fresh,EVENT_SLOT,true)}};
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),static_cast<CEntity*>(PedC()),static_cast<CEntity*>(VehA()),static_cast<CEntity*>(VehB()),static_cast<CEntity*>(ObjA())})refs.push_back(refList(e));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Hash(fakes[n].data()+4,fakes[n].size()-4));
                    json r={{"ret",ret},{"object",objectWords},{"clone",clone},{"refs",refs},
                            {"peds",{EntHash(pedA),EntHash(pedB),EntHash(pedC)}},{"vehs",{EntHash(vehA),EntHash(vehB)}},{"obj",EntHash(objA)},
                            {"intel",{Hash(intelA.data(),intelA.size()),Hash(intelB.data(),intelB.size())}},{"fakes",fakeWords},
                            {"events",{Region(evA.data(),evA.size(),true),Region(evB.data(),evB.size(),true),Region(groupEvent.data(),groupEvent.size())}},
                            {"group",Hash(group.data(),group.size())},{"mi",Hash(mi.data(),mi.size())},{"col",Hash(col.data(),col.size())},{"time",timeMs},{"pad",padByte}};
                    r["trace"]=trace; r["switches"]=switches;
                    CleanRefs();
                    if(fresh)GetEventPool()->Delete(static_cast<CEvent*>(fresh));
                    r["clean"]=EvOccupancy()==occ;
                    restoreEvents(); padByte=savedPad;
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
    timeMs=savedTime; padByte=savedPad; miSlot=savedMi;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",EvOccupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime
        &&std::memcmp(EvHdr(),savedEvHdr.data(),savedEvHdr.size())==0&&padByte==savedPad&&miSlot==savedMi}});
}
}
