#pragma once
// Banco dentro del motor: CEventDamage (lista de 20, 20 funciones): los dos constructores, From, los virtuales
// (GetEventType, GetEventPriority, GetLifeTime, AffectsPed, AffectsPedGroup, IsCriminalEvent, ReportCriminalEvent,
// GetSourceEntity, TakesPriorityOver, GetLocalSoundLevel, DoInformVehicleOccupants, CanBeInterruptedBySameEvent,
// CloneEditable; huecos comprobados con el volcado de la vtable 0x85AD78), ProcessDamage, ComputeBodyPartToRemove,
// ComputeDeathAnim y ComputeDamageAnim.
// Original (hooks de la clase desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD y se comparan enteros; los virtuales por la entrada de la vtable
// del EXE. Falsos: peatones (desplazamientos crudos, matriz, inteligencia y datos de jugador propios), un vehiculo, un
// objeto, tareas (vtable trazada: GetSubTask, IsSimple, GetTaskType, MakeAbortable), eventos de referencia y de
// historial (vtable trazada), un CPedGroup, un CWeaponInfo, un CWanted y un clump.
// Se simulan y trazan: FindPlayerPed, FindPlayerWanted, CWanted::RegisterCrime, CPedType::PoliceDontCare...,
// CPedGroups::AreInSameGroup, CPedIntelligence::CanSeeEntityWithLights, CPhysical::CanPhysicalBeDamaged/ApplyMoveForce,
// CWeaponInfo::GetWeaponInfo, CStats::GetFatAndMuscleModifier, CPed::GetBonePosition/RemoveBodyPart,
// RpAnimBlendClumpGetFirstAssociation y el RNG (rand del EXE y CGeneral::GetRandomNumber).
// Se guardan y restauran en cada ejecucion y al final: el pool de eventos (cabecera con m_LastFreeSlot, estados y
// almacenamiento; huecos libres con 0xCD), CTimer::m_snTimeInMilliseconds, CPedAcquaintanceScanner::
// ms_fThresholdDotProduct, el truco MEGA_PUNCH y m_bFireProof del jugador en foco.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventDamage.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "PedType.h"
#include "Wanted.h"
#include "WeaponInfo.h"
#include "Stats.h"
#include "Cheat.h"
#include "General.h"
#include "Pools/Pools.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace Event20Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_RW=0x18, E_TYPE=0x36, PH_FLAGS=0x40, PH_ATTACHED=0xFC;   // CEntity / CPhysical
constexpr size_t P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PDATA=0x480,
                 P_STATE=0x530, P_MOVE=0x534, P_HEALTH=0x540, P_VEHICLE=0x58C, P_TYPE=0x598;            // CPed
constexpr size_t V_DRIVER=0x460;                                                                        // CVehicle
constexpr size_t I_PED=0x00, I_PRIMARY=0x04, I_SECONDARY=0x18, I_NONTEMP=0x3C, I_TEMP=0x40;             // CPedIntelligence
constexpr size_t D_TIMER=0x28, D_FLAGS=0x34;                                                            // CPlayerPedData
constexpr size_t G_MEMBERS=0x0C;                                                                        // CPedGroup (pertenencia en +8)
constexpr size_t EV_ADD=0x0C, EV_SRC=0x14, EV_WEAPON=0x1C, EV_PIECE=0x20, EV_DIR=0x24, EV_FLAGS=0x25, EV_ANIM=0x2C,
                 EV_DMG=0x38, EV_HZERO=0x40, EV_FDEATH=0x41, EV_CALC=0x42, EV_CHECK=0x43;              // CEventDamage
constexpr size_t FK_SUB=0x30, FK_SIMPLE=0x34, FK_TYPE=0x38, FK_ABORT=0x3C;                               // tarea falsa
constexpr size_t FE_TYPE=0x30, FE_PRIO=0x34, FE_SRC=0x38;                                                // evento falso
constexpr uintptr_t VT_DAMAGE=0x85AD78, TIMER=0xB7CB84, THRESHOLD=0xC0B034, IN_FOCUS=0xB7CD74, FIREPROOF0=0xB7CEE6;
constexpr size_t EVENT_SLOT=0x44, PLAYERINFO_SIZE=0x190;
constexpr int NPEDS=4, FAKES=6;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPlayerPedData)>,NPEDS> pdatas{};
alignas(16) inline std::array<uint8,0x600> veh{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> vehMat{};
alignas(16) inline std::array<uint8,0x200> obj{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x80> object{}, srcEv{}, refEv{}, histEv{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,0x80> winfo{}, wanted{}, clump{}, assoc{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }
inline std::vector<bool> EvOccupancy() { std::vector<bool> r(EvCap()); for(size_t k=0;k<r.size();++k)r[k]=!(EvStates()[k]&0x80); return r; }

inline std::array<void*,16> taskVtbl{};
inline std::array<void*,20> eventVtbl{};
inline std::array<void*,80> pedVtbl{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(taskVtbl.data()))return "task_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt"; if(v==uintptr_t(pedVtbl.data()))return "ped_vt";
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){
        if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
        if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); if(auto s=in(pdatas[n].data(),pdatas[n].size(),"pdata"))return *s+"#"+std::to_string(n);
    }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
#define EV20_IN(buf,nm) if(auto s=in(buf.data(),buf.size(),nm))return *s;
    EV20_IN(veh,"veh") EV20_IN(vehMat,"vmat") EV20_IN(obj,"obj") EV20_IN(object,"object") EV20_IN(srcEv,"srcEv") EV20_IN(refEv,"refEv")
    EV20_IN(histEv,"histEv") EV20_IN(group,"group") EV20_IN(winfo,"winfo") EV20_IN(wanted,"wanted") EV20_IN(clump,"clump") EV20_IN(assoc,"assoc")
#undef EV20_IN
    { const auto es=uintptr_t(EvStorage()); if(v>=es&&v<es+EvCap()*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    if(v>=VT_DAMAGE&&v<VT_DAMAGE+0x44)return "vt_damage+"+std::to_string(v-VT_DAMAGE);
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global: se enmascara (las listas se comparan aparte)
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline json RefList(const void* e) { json l=json::array(); for(auto* r=static_cast<const CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); r.push_back(RefList(veh.data())); r.push_back(RefList(obj.data())); return r; }
inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); clean(veh.data()); clean(obj.data());
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uintptr_t playerRet{};
inline bool sameGroupRet{}, canDamageRet{}, policeRet{};
inline float canSeeRet{}, fatRet{}, boneDz{};
inline uintptr_t assocRet{};

// tareas falsas
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask* t,void*) { return At<uint8>(t,FK_SIMPLE)!=0; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* ev) {
    trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(ev))}); return At<uint8>(t,FK_ABORT)!=0;
}
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(uintptr_t(t))}); return nullptr; }
// eventos falsos (referencia de TakesPriorityOver y evento actual del historial)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_type",Name(uintptr_t(e))}); return At<int32>(e,FE_TYPE); }
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"event_priority",Name(uintptr_t(e))}); return At<int32>(e,FE_PRIO); }
inline CEntity* __fastcall ESrc(void* e,void*) { trace.push_back({"event_source",Name(uintptr_t(e))}); return At<CEntity*>(e,FE_SRC); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(uintptr_t(e))}); return nullptr; }
template<int N> void* __fastcall PTrap(void* p,void*) { trace.push_back({"ped_unexpected_slot",N,Name(uintptr_t(p))}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) {
    ((taskVtbl[N%16]=reinterpret_cast<void*>(&FTrap<N%16>)),...); ((eventVtbl[N%20]=reinterpret_cast<void*>(&ETrap<N%20>)),...);
    ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...);
}

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __fastcall FakeRegisterCrime(void* w,void*,int32 type,const CVector* pos,uint32 id,uint32 dontCare) {
    trace.push_back({"register_crime",Name(uintptr_t(w)),type,F(pos->x),F(pos->y),F(pos->z),Name(id),dontCare&0xFF});
}
inline bool __cdecl FakePolice(int32 type) { trace.push_back({"police_dont_care",type}); return policeRet; }
inline bool __cdecl FakeSameGroup(const CPed* a,const CPed* b) { trace.push_back({"are_in_same_group",Name(uintptr_t(a)),Name(uintptr_t(b))}); return sameGroupRet; }
inline float __fastcall FakeCanSee(void* in,void*,CEntity* e,int32 unused) { trace.push_back({"can_see_entity_with_lights",Name(uintptr_t(in)),Name(uintptr_t(e)),unused}); return canSeeRet; }
inline bool __fastcall FakeCanDamage(void* ph,void*,uint32 weapon,bool* out) { trace.push_back({"can_physical_be_damaged",Name(uintptr_t(ph)),weapon,Name(uintptr_t(out))}); return canDamageRet; }
inline CWeaponInfo* __cdecl FakeWeaponInfo(uint32 weapon,uint32 skill) { trace.push_back({"get_weapon_info",weapon,skill&0xFF}); return reinterpret_cast<CWeaponInfo*>(winfo.data()); }
inline float __cdecl FakeFat(int32 mod) { trace.push_back({"fat_and_muscle_modifier",mod}); return fatRet; }
inline void __fastcall FakeBone(CPed* p,void*,CVector* out,int32 bone,uint32 update) {
    trace.push_back({"get_bone_position",Name(uintptr_t(p)),bone,update&0xFF,F(out->x),F(out->y),F(out->z)});
    const CVector pos=p->GetPosition(); *out=CVector{pos.x+0.25f,pos.y-0.5f,pos.z+boneDz};
}
inline void* __cdecl FakeFirstAssoc(void* c,uint32 flags) { trace.push_back({"first_association",Name(uintptr_t(c)),flags}); return reinterpret_cast<void*>(assocRet); }
inline void __fastcall FakeMoveForce(void* ph,void*,CVector f) { trace.push_back({"apply_move_force",Name(uintptr_t(ph)),F(f.x),F(f.y),F(f.z)}); }
inline void __fastcall FakeRemoveBodyPart(CPed* p,void*,int32 node,uint32 dir) { trace.push_back({"remove_body_part",Name(uintptr_t(p)),node,dir&0xFF}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=20;
inline const std::array<Fn,NFNS> fns{{
    {"CEventDamage","GetEventType",0,0x85AD78,1,64},
    {"CEventDamage","GetEventPriority",0,0x85AD78,2,64},
    {"CEventDamage","GetLifeTime",0,0x85AD78,3,64},
    {"CEventDamage","AffectsPed",0,0x85AD78,5,1024},
    {"CEventDamage","AffectsPedGroup",0,0x85AD78,6,512},
    {"CEventDamage","IsCriminalEvent",0,0x85AD78,7,256},
    {"CEventDamage","ReportCriminalEvent",0,0x85AD78,8,512},
    {"CEventDamage","GetSourceEntity",0,0x85AD78,10,128},
    {"CEventDamage","TakesPriorityOver",0,0x85AD78,11,512},
    {"CEventDamage","GetLocalSoundLevel",0,0x85AD78,12,64},
    {"CEventDamage","DoInformVehicleOccupants",0,0x85AD78,13,128},
    {"CEventDamage","CanBeInterruptedBySameEvent",0,0x85AD78,15,64},
    {"CEventDamage","CloneEditable",0,0x85AD78,16,256},
    {"CEventDamage","Constructor-",0x4B33B0,0,-1,256},
    {"CEventDamage","Constructor-1",0x4AD830,0,-1,256},
    {"CEventDamage","From",0x4AD9C0,0,-1,256},
    {"CEventDamage","ProcessDamage",0x4B3A20,0,-1,256},
    {"CEventDamage","ComputeBodyPartToRemove",0x4ADC10,0,-1,256},
    {"CEventDamage","ComputeDeathAnim",0x4B3A60,0,-1,1024},
    {"CEventDamage","ComputeDamageAnim",0x4B3FC0,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"event20-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"event20-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("event20");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"event20-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,80>{});
    taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[3]=reinterpret_cast<void*>(&FSimple); taskVtbl[4]=reinterpret_cast<void*>(&FType); taskVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio); eventVtbl[10]=reinterpret_cast<void*>(&ESrc);
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const uintptr_t fireProof=FIREPROOF0+uintptr_t(*reinterpret_cast<uint8*>(IN_FOCUS))*PLAYERINFO_SIZE;
    const uintptr_t megaPunch=uintptr_t(&CCheat::m_aCheatsActive[CHEAT_MEGA_PUNCH]);
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{THRESHOLD,4},{fireProof,1},{megaPunch,1}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto restoreEvents=[&]{ std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size());
                            std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size()); };
    const auto evOccStart=EvOccupancy();
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FakeFindPlayerWanted), p2e(0x56E230,FakeFindPlayerWanted);
        EntryPatch w1(bit_cast<uintptr_t>(&CWanted::RegisterCrime),FakeRegisterCrime), w1e(0x562410,FakeRegisterCrime);
        EntryPatch t1(bit_cast<uintptr_t>(&CPedType::PoliceDontCareAboutCrimesAgainstPedType),FakePolice), t1e(0x608A40,FakePolice);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroups::AreInSameGroup),FakeSameGroup), g1e(0x5F7F40,FakeSameGroup);
        EntryPatch i1(bit_cast<uintptr_t>(&CPedIntelligence::CanSeeEntityWithLights),FakeCanSee), i1e(0x605550,FakeCanSee);
        EntryPatch h1(bit_cast<uintptr_t>(&CPhysical::CanPhysicalBeDamaged),FakeCanDamage), h1e(0x5448B0,FakeCanDamage);
        EntryPatch h2(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeMoveForce), h2e(0x5429F0,FakeMoveForce);
        EntryPatch n1(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),FakeWeaponInfo), n1e(0x743C60,FakeWeaponInfo);
        EntryPatch d1(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),FakeFat), d1e(0x559AF0,FakeFat);
        EntryPatch b1(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),FakeBone), b1e(0x5E4280,FakeBone);
        EntryPatch b2(bit_cast<uintptr_t>(&CPed::RemoveBodyPart),FakeRemoveBodyPart), b2e(0x5F0140,FakeRemoveBodyPart);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),FakeFirstAssoc), a1e(0x4D6A70,FakeFirstAssoc);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l){return *(l.begin()+next()%l.size());};
                auto pickF=[&](std::initializer_list<float> l){return *(l.begin()+next()%l.size());};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2020u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    CleanRefs(); restoreGlobals(); restoreEvents();
                    // pool de eventos: el New del EXE busca desde m_LastFreeSlot+1 (cabecera restaurada); huecos libres con 0xCD
                    for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    const auto evOcc=EvOccupancy();
                    const uintptr_t P0=uintptr_t(Ped(0)), P1=uintptr_t(Ped(1)), P2=uintptr_t(Ped(2)), P3=uintptr_t(Ped(3)), V0=uintptr_t(veh.data()), O0=uintptr_t(obj.data());
                    // ------------------------------------------------ fixture comun
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0x80000000u+next()%0x7FFF0000u : next()%5==0 ? 0xFFFFF000u+next()%0x1000 : 100000u+next()%50000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    At<float>(reinterpret_cast<void*>(THRESHOLD),0)=pickF({-0.5f,0.f,0.3f,0.7f,0.95f,1.5f});
                    *reinterpret_cast<uint8*>(fireProof)=uint8(next()%2); *reinterpret_cast<uint8*>(megaPunch)=uint8(next()%3==0);
                    const CVector center=vec(30.f);
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); auto* m=pedMats[n].data(); std::memset(m,0,pedMats[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3;
                        const CVector pos=center+vec(8.f);
                        // el EXE lee la matriz sin comprobarla en las ramas de "sigilo": la victima y los miembros siempre la tienen
                        if(n!=1||next()%2){ At<void*>(p,E_MATRIX)=m; At<CVector>(m,0x30)=pos; At<CVector>(m,0x10)=vec(1.f); At<CVector>(m,0)=vec(1.f); }
                        else At<CVector>(p,E_POS)=pos;
                        At<void*>(p,E_RW)=clump.data();
                        At<uint32>(p,PH_FLAGS)=next(); At<uint32>(p,P_FLAGS1)=next(); At<uint32>(p,P_FLAGS2)=next(); At<uint32>(p,P_FLAGS3)=next(); At<uint32>(p,P_FLAGS4)=next();
                        At<uintptr_t>(p,PH_ATTACHED)=next()%4==0 ? O0 : 0;
                        At<uint32>(p,P_TYPE)=uint32(pick({0,1,6,8,8,4,7,20}));
                        At<uint32>(p,P_STATE)=uint32(pick({1,1,1,0x36,0x37,0x32,7}));
                        At<uint32>(p,P_MOVE)=uint32(pick({0,1,1,4,5,6,7,0x80000001u}));
                        At<float>(p,P_HEALTH)=pickF({100.f,100.f,0.f,-5.f,10.f,14.99f,15.f,30.f});
                        At<uintptr_t>(p,P_VEHICLE)=next()%3==0 ? V0 : 0;
                        auto* in=intels[n].data(); std::memset(in,0,intels[n].size()); At<void*>(p,P_INTEL)=in; At<void*>(in,I_PED)=p;
                        auto* d=pdatas[n].data(); for(auto& b:pdatas[n])b=uint8(next());
                        At<uint32>(d,D_TIMER)=next()%2 ? now-uint32(next()%3000) : now+uint32(next()%3000);
                        // datos de jugador: la victima siempre (el EXE los lee si es el jugador); el resto al azar salvo si es jugador
                        At<void*>(p,P_PDATA)=(n==0||At<uint32>(p,P_TYPE)<=1||next()%2) ? (void*)d : nullptr;
                    }
                    { auto* v=veh.data(); std::memset(v,0,veh.size()); std::memset(vehMat.data(),0,vehMat.size()); v[E_TYPE]=2;
                      At<void*>(v,E_MATRIX)=vehMat.data(); At<CVector>(vehMat.data(),0x30)=center+vec(8.f); At<uintptr_t>(v,V_DRIVER)=pick({P1,P2,0,P0}); }
                    { auto* o=obj.data(); std::memset(o,0,obj.size()); o[E_TYPE]=4; At<CVector>(o,E_POS)=center+vec(8.f); }
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto& b:winfo)b=uint8(next()); At<uint32>(winfo.data(),0)=uint32(pick({0,1,1,2,3})); At<int16>(winfo.data(),0x22)=int16(pick({10,50,51,200,0x8001,0}));
                    std::memset(wanted.data(),0,wanted.size()); std::memset(group.data(),0,group.size());
                    // tareas falsas: 0 activa de la victima, 1 su subtarea, 2 activa del origen, 3 secundaria de ataque del origen, 4-5 libres
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[n])b=uint8(next()); At<void*>(fakes[n].data(),0)=taskVtbl.data(); At<void*>(fakes[n].data(),4)=nullptr;
                        At<CTask*>(fakes[n].data(),FK_SUB)=nullptr; At<uint8>(fakes[n].data(),FK_SIMPLE)=1; At<uint8>(fakes[n].data(),FK_ABORT)=uint8(next()%3!=0); }
                    At<int32>(fakes[0].data(),FK_TYPE)=int32(pick({400,203,207,1303}));
                    At<int32>(fakes[1].data(),FK_TYPE)=int32(pick({207,205,207,205,1303,400}));
                    if(next()%3){ At<CTask*>(fakes[0].data(),FK_SUB)=Fake(1); At<uint8>(fakes[0].data(),FK_SIMPLE)=0; }
                    At<int32>(fakes[2].data(),FK_TYPE)=int32(pick({1027,1027,1000,400}));
                    At<uintptr_t>(fakes[2].data(),0x10)=pick({P0,P2,0});   // CTaskComplexKillPedOnFoot::m_target
                    At<int32>(fakes[3].data(),FK_TYPE)=int32(pick({1016,1016,1017,1017,400}));
                    At<int8>(fakes[3].data(),0x10)=int8(pick({5,5,2,uint32(-1)}));       // CTaskSimpleUseGun::m_LastCmd
                    At<int8>(fakes[3].data(),0x24)=int8(int32(next()%18)-2);              // CTaskSimpleFight::m_nComboSet
                    At<int8>(fakes[3].data(),0x25)=int8(pick({0,1,2,3,4,4,5,uint32(-1)})); // CTaskSimpleFight::m_nCurrentMove
                    { auto* in0=intels[0].data(); At<CTask*>(in0,I_PRIMARY+4*(next()%5))=next()%5 ? Fake(0) : nullptr; }
                    { auto* in1=intels[1].data(); At<CTask*>(in1,I_PRIMARY+4*(next()%3))=next()%5 ? Fake(2) : nullptr; At<CTask*>(in1,I_SECONDARY)=next()%5 ? Fake(3) : nullptr; }
                    // eventos falsos
                    for(auto* e:{&refEv,&histEv}){ for(auto& b:*e)b=uint8(next()); At<void*>(e->data(),0)=eventVtbl.data(); }
                    At<int32>(refEv.data(),FE_TYPE)=int32(pick({66,58,8,9,9,9,9,31}));
                    At<int32>(refEv.data(),FE_PRIO)=int32(pick({65,64,66,0,100}));
                    At<uintptr_t>(refEv.data(),FE_SRC)=pick({P1,P2,V0,0,P0});
                    At<int32>(histEv.data(),FE_TYPE)=int32(pick({9,9,9,31}));
                    { auto* in0=intels[0].data(); const uint32 m=next()%4; At<void*>(in0,I_TEMP)=m==0 ? (void*)histEv.data() : nullptr; At<void*>(in0,I_NONTEMP)=m==1||m==2 ? (void*)histEv.data() : nullptr; }
                    // dependencias simuladas
                    playerRet=pick({P0,P1,P2,0,P0}); sameGroupRet=next()%3==0; canDamageRet=next()%4!=0; policeRet=next()%2;
                    canSeeRet=pickF({1.f,0.f,-1.f,0.25f,1.f}); fatRet=pickF({1.f,0.5f,1.75f,0.f}); boneDz=pickF({-0.5f,-0.1f,-0.3f,0.5f,0.f,-0.2f});
                    assocRet=next()%2 ? uintptr_t(assoc.data()) : 0;
                    // evento probado (bytes aleatorios con la vtable del EXE)
                    void* const o=object.data(); for(auto& b:object)b=uint8(next()); At<uintptr_t>(o,0)=VT_DAMAGE;
                    At<int32>(o,4)=int32(next()%5)-2; At<uint8>(o,EV_ADD)=uint8(next()%2);
                    At<uintptr_t>(o,EV_SRC)=pick({P1,P1,P1,V0,O0,0});
                    At<uint32>(o,EV_WEAPON)=next()%10==0 ? uint32(pick({55,58,60,0x80000001u,0xFFFFFFFFu})) : next()%55;
                    At<uint32>(o,EV_PIECE)=uint32(pick({3,3,4,5,6,7,8,9,0}));
                    At<uint8>(o,EV_DIR)=uint8(next()%6==0 ? 4+next()%3 : next()%4);
                    At<float>(o,EV_DMG)=uni(0.f,30.f); At<float>(o,EV_DMG+4)=next()%2 ? uni(0.f,30.f) : 0.f;
                    for(size_t k=EV_HZERO;k<=EV_CHECK;++k)At<uint8>(o,k)=uint8(next()%2);
                    At<uint32>(o,EV_ANIM)=uint32(pick({0xAB,0xAC,0xAD,0xAF,0xB2,0xB5,0xB8,0x20,0xBF}));
                    At<uint32>(o,0x28)=next()%118;
                    // el evento del historial a veces tiene la misma animacion que la que se va a elegir
                    At<uint32>(histEv.data(),EV_ANIM)=uint32(pick({0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0x20}));
                    CPed* const victim=Ped(0);

                    json out=json::object(); json ret=nullptr;
                    bool switches{};
                    { for(auto* h:hooks)h->State(candidate);
                      const uintptr_t t=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                      switches=fn.slot>=0 ? ((t>=0x400000&&t<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                      for(auto* h:hooks)h->State(false); }
                    auto callTarget=[&](auto&& body){
                        for(auto* h:hooks)h->State(candidate);
                        body(fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address);
                        for(auto* h:hooks)h->State(false);
                    };
                    // fuente en el evento: referencia registrada como hace el constructor (se limpia al final)
                    auto regSource=[&](void* ev){ if(auto* e=At<CEntity*>(ev,EV_SRC))e->RegisterReference(reinterpret_cast<CEntity**>(static_cast<uint8*>(ev)+EV_SRC)); };

                    if(nm=="GetEventType"||nm=="GetEventPriority"||nm=="GetLifeTime"||nm=="CanBeInterruptedBySameEvent"){
                        callTarget([&](uintptr_t t){ ret=nm=="CanBeInterruptedBySameEvent" ? json(reinterpret_cast<bool(__thiscall*)(void*)>(t)(o)) : json(reinterpret_cast<int32(__thiscall*)(void*)>(t)(o)); });
                    } else if(nm=="GetLocalSoundLevel"){
                        callTarget([&](uintptr_t t){ ret=F(reinterpret_cast<float(__thiscall*)(void*)>(t)(o)); });
                    } else if(nm=="GetSourceEntity"){
                        callTarget([&](uintptr_t t){ ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o))); });
                    } else if(nm=="IsCriminalEvent"){
                        callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(t)(o); });
                    } else if(nm=="DoInformVehicleOccupants"){
                        if(next()%3==0)At<uint32>(o,EV_WEAPON)=53;
                        callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,victim); });
                    } else if(nm=="ReportCriminalEvent"){
                        At<uint32>(o,EV_WEAPON)=next()%6==0 ? uint32(pick({0x80000001u,0xFFFFFFF7u,60,55})) : next()%55;
                        callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(t)(o,victim); });
                    } else if(nm=="TakesPriorityOver"){
                        callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(t)(o,refEv.data()); });
                    } else if(nm=="AffectsPed"){
                        At<uint32>(o,EV_WEAPON)=uint32(pick({53,53,23,23,18,37,51,54,54,16,22,31,0,9,35,0xFFFFFFF0u,next()%55}));
                        if(next()%3==0)At<uint32>(Ped(1),P_TYPE)=8;
                        callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,victim); });
                    } else if(nm=="AffectsPedGroup"){
                        At<uint32>(o,EV_WEAPON)=next()%5==0 ? 23u : next()%55;
                        for(int k=0;k<8;++k)At<uintptr_t>(group.data(),G_MEMBERS+4*k)=next()%3 ? pick({P0,P2,P3}) : 0;
                        callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(t)(o,group.data()); });
                    } else if(nm=="CloneEditable"){
                        regSource(o);
                        callTarget([&](uintptr_t t){ ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o))); });
                    } else if(nm=="Constructor-"){
                        for(auto& b:srcEv)b=uint8(next()); At<uintptr_t>(srcEv.data(),0)=VT_DAMAGE; At<uintptr_t>(srcEv.data(),EV_SRC)=pick({P1,V0,O0,0,P2});
                        std::memset(o,0xCD,object.size());
                        callTarget([&](uintptr_t t){ ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const void*)>(t)(o,srcEv.data()))); });
                        out["type"]=int(reinterpret_cast<CEvent*>(o)->GetEventType()); At<uintptr_t>(o,0)=0;   // vptr EXE o DLL: se compara el tipo
                    } else if(nm=="Constructor-1"){
                        void* const src=reinterpret_cast<void*>(pick({P1,V0,O0,0,P2})); const uint32 start=next(), weapon=next()%60, piece=next()%10;
                        const uint8 dirArg=uint8(next()); const bool jumped=next()%2, inVeh=next()%2;
                        std::memset(o,0xCD,object.size());
                        callTarget([&](uintptr_t t){ ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,uint32,uint32,uint32,uint8,bool,bool)>(t)(o,src,start,weapon,piece,dirArg,jumped,inVeh))); });
                        out["type"]=int(reinterpret_cast<CEvent*>(o)->GetEventType()); At<uintptr_t>(o,0)=0;
                    } else if(nm=="From"){
                        for(auto& b:srcEv)b=uint8(next()); At<uintptr_t>(srcEv.data(),0)=VT_DAMAGE; At<uintptr_t>(srcEv.data(),EV_SRC)=pick({P1,V0,O0,0,P2});
                        callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,const void*)>(t)(o,srcEv.data()); });
                    } else if(nm=="ProcessDamage"||nm=="ComputeBodyPartToRemove"){
                        At<uint32>(o,EV_WEAPON)=next()%8==0 ? uint32(pick({55,60,0x80000001u,0xFFFFFFFFu})) : next()%55;
                        if(nm=="ProcessDamage") callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(t)(o,victim); });
                        else { int32 node=int32(0xCDCDCDCD); callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,int32*)>(t)(o,&node); }); out["node"]=node; }
                    } else { // ComputeDeathAnim / ComputeDamageAnim
                        const bool death=nm=="ComputeDeathAnim";
                        At<uint32>(o,EV_WEAPON)=next()%10==0 ? uint32(pick({55,58,0x80000001u,0xFFFFFFF0u})) : next()%55;
                        // la victima rara vez en vehiculo; a veces el jugador; sin origen el EXE lee la posicion del origen nulo si hay fuerza
                        if(next()%4)At<uint32>(victim,P_FLAGS1)&=~0x100u;
                        if(next()%4==0){ At<uint32>(victim,P_TYPE)=uint32(next()%2); At<uint32>(victim,P_STATE)=next()%4 ? 1u : 0x32u; }
                        if(!At<uintptr_t>(o,EV_SRC)&&!death&&At<uint32>(o,EV_PIECE)==3)At<uint32>(o,EV_PIECE)=4;
                        if(At<uintptr_t>(o,EV_SRC)==O0&&next()%2)At<uint32>(o,EV_WEAPON)=54;
                        const bool abortable=next()%3!=0;
                        callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,CPed*,bool)>(t)(o,victim,abortable); });
                    }
                    out["ret"]=ret; out["object"]=Region(o,EVENT_SLOT);
                    // clon nuevo del pool de eventos (palabras y tipo); despues se libera
                    json clone=nullptr; void* fresh=nullptr;
                    { const auto now2=EvOccupancy(); for(size_t k=0;k<now2.size();++k)if(now2[k]&&!evOcc[k]){ fresh=EvStorage()+k*EVENT_SLOT; break; } }
                    if(fresh){ json w=Region(fresh,EVENT_SLOT); w[0]=int(reinterpret_cast<CEvent*>(fresh)->GetEventType()); clone={{"at",Name(uintptr_t(fresh))},{"words",w}}; }
                    out["clone"]=clone; out["refs"]=AllRefs();
                    json pedHashes=json::array(); for(auto& p:peds)pedHashes.push_back(EntHash(p)); out["peds"]=pedHashes;
                    json pd=json::array(); for(auto& d:pdatas)pd.push_back(Region(d.data(),0x40)); out["pdata"]=pd;
                    out["fakes"]=[&]{ json r=json::array(); for(auto& fk:fakes)r.push_back(Region(fk.data(),fk.size())); return r; }();
                    out["events"]={Region(srcEv.data(),EVENT_SLOT),Region(refEv.data(),0x40),Region(histEv.data(),0x40)};
                    out["others"]={EntHash(veh),EntHash(obj),Hash(group.data(),group.size()),Hash(wanted.data(),wanted.size())};
                    out["trace"]=trace; out["time"]=now; out["switches"]=switches;
                    CleanRefs(); if(fresh)GetEventPool()->Delete(static_cast<CEvent*>(fresh));
                    out["clean"]=EvOccupancy()==evOcc;
                    restoreGlobals(); restoreEvents();
                    return out;
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
    CleanRefs(); restoreGlobals(); restoreEvents();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",EvOccupancy()==evOccStart&&freeRefs()==savedFree&&globalsIntact()}});
}
}
