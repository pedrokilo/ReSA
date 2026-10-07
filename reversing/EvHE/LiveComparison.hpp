#pragma once
// Banco dentro del motor: CEventHandler::ComputeKnockOffBikeResponse (0x4B9FF0).
// Original (hook de la funcion desactivado) frente a candidato con fixtures identicos: el manejador (CEventHandler de 0x34
// bytes con m_Ped en +0 y m_EventResponseTask en +0x24, relleno con un valor centinela), tres peatones con inteligencia
// propia, dos vehiculos del tamano de CAutomobile (velocidad, conductor/pasajeros, tipo en +0x590 y banderas de moto en
// +0x614), un grupo falso y un evento falso con la disposicion del EXE y vtable trazada (CEventKnockOffBike, CEventDamage,
// VehicleDied o VehicleOnFire). Se comparan el valor final de m_EventResponseTask, TODAS las tareas nuevas del pool de tareas
// (tipo y 0x80 bytes con los punteros traducidos), el pool de eventos (el evento de "lider salio del coche" se crea y se
// destruye), las referencias, los fixtures completos (banderas del ped, de la moto...) y la traza.
// Se simulan y trazan: CEventKnockOffBike::SetPedSafePosition/SetPedOutCar/CalcForcesAndAnims (con un vehiculo falso revientan),
// CEventDamage::AffectsPed, CPedDamageResponseCalculator::ComputeDamageResponse, CInterestingEvents::Add,
// CPedGroups::GetPedsGroup, CPedGroupMembership::IsLeader/GetLeader, CPedGroupIntelligence::AddEvent,
// CEventGroup::Add/GetEventOfType y el RNG (rand 0x821B1E y CGeneral::GetRandomNumber). Los constructores, el operador de
// asignacion y los destructores de los eventos y las tareas se usan reales. El pool de tareas y el de eventos (estados y
// m_LastFreeSlot, huecos libres a 0xCD) y el reloj se restauran en cada ejecucion.
#include <fstream>
#include <set>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Events/EventKnockOffBike.h"
#include "Events/EventDamage.h"
#include "Events/EventDraggedOutCar.h"
#include "Events/EventGroup.h"
#include "Events/EventGroupEvent.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "InterestingEvents.h"
#include "PedDamageResponseCalculator.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "References.h"
#include "Pools/Pools.h"
#include "Pools/TaskPool.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace EvHELive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }

// desplazamientos crudos del EXE (miembros protegidos o para no depender de la DLL)
constexpr size_t E_MATRIX=0x14, E_FLAGS=0x1C, P_PHYS=0x40, P_FLAGS=0x46C, P_INTEL=0x47C, P_HEALTH=0x540, P_VEHICLE=0x58C;
constexpr size_t V_MOVE=0x44, V_DRIVER=0x460, V_PASSENGER0=0x464, V_TYPE=0x590, V_BIKEFLAGS=0x614;
constexpr size_t H_PED=0x00, H_RESP=0x24, EVENT_SLOT=0x44, TASK_SLOT=0x80;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehA{}, vehB{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{}, intelC{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> grp{};
alignas(16) inline std::array<uint8,0x60> event{}, evOther{}, hbuf{};
inline uint8* PedBuf(int k) { switch(k){case 0:return pedA.data();case 1:return pedB.data();default:return pedC.data();} }
inline uint8* IntelBuf(int k) { switch(k){case 0:return intelA.data();case 1:return intelB.data();default:return intelC.data();} }
inline uint8* VehBuf(int k) { return k==0 ? vehA.data() : vehB.data(); }
inline CPed* Ped(int k) { return reinterpret_cast<CPed*>(PedBuf(k)); }
inline CEntity* EntOf(void* b) { return reinterpret_cast<CEntity*>(b); }
inline std::unordered_map<uintptr_t,int> freshIds;

// pools: cabecera CPool (m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s; if(auto s=in(pedC.data(),pedC.size(),"pedC"))return *s;
    if(auto s=in(matA.data(),matA.size(),"matA"))return *s;
    if(auto s=in(vehA.data(),vehA.size(),"vehA"))return *s; if(auto s=in(vehB.data(),vehB.size(),"vehB"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(intelB.data(),intelB.size(),"intelB"))return *s; if(auto s=in(intelC.data(),intelC.size(),"intelC"))return *s;
    if(auto s=in(grp.data(),grp.size(),"group"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(evOther.data(),evOther.size(),"evOther"))return *s; if(auto s=in(hbuf.data(),hbuf.size(),"handler"))return *s;
    { const auto es=uintptr_t(EvStorage()); if(v>=es&&v<es+EvCap()*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
// puntero a un objeto que puede estar en la pila (eventos locales): los desconocidos se nombran "local"
inline json NameL(uintptr_t v) { auto j=Name(v); return j.is_number() ? json("local") : j; }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline int eventType{}; inline bool cfgSafePos{}, cfgAffects{}, cfgIsLeader{}, cfgAddEventRet{};
inline int32 cfgAnim{}; inline uintptr_t cfgGroup{}, cfgLeader{}, cfgFoundEvent{};
inline uint32 seed{}, rngCalls{};

// evento falso (GetEventType en el hueco 1)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// contenido de un CEventKnockOffBike (evento falso o local de la funcion): +0x35 solo el bit 0 (el resto lo deja el EXE sin tocar)
inline json KDesc(void* k) {
    json j=json::array(); for(size_t o=0xC;o<0x30;o+=4)j.push_back(At(k,o));
    j.push_back(Name(At<uintptr_t>(k,0x30))); j.push_back(At<uint8>(k,0x34)); j.push_back(At<uint8>(k,0x35)&1); j.push_back(At<uint8>(k,0x36)); j.push_back(At<uint8>(k,0x37));
    j.push_back(Name(At<uintptr_t>(k,0x38))); return j;
}

// dependencias simuladas
inline int __cdecl FakeRand() {
    trace.push_back({"rand"}); const int v=int((seed*2654435761u+rngCalls*40503u+rngCalls*rngCalls*977u)>>7)&0xFFFF; ++rngCalls; return v;
}
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline bool __fastcall SafePos(void* k,void*,CPed* p) { trace.push_back({"set_ped_safe_position",NameL(uintptr_t(k)),KDesc(k),Name(uintptr_t(p))}); return cfgSafePos; }
inline void __fastcall OutCar(void* k,void*,CPed* p) { trace.push_back({"set_ped_out_car",NameL(uintptr_t(k)),KDesc(k),Name(uintptr_t(p))}); }
inline int32 __fastcall CalcForces(void* k,void*,CPed* p) { trace.push_back({"calc_forces_and_anims",NameL(uintptr_t(k)),KDesc(k),Name(uintptr_t(p))}); return cfgAnim; }
inline void __fastcall InterestingAdd(void*,void*,int type,CEntity* e) { trace.push_back({"interesting_event_add",type,Name(uintptr_t(e))}); }
inline bool __fastcall AffectsS(void* ev,void*,CPed* p) {
    trace.push_back({"affects_ped",At<uint8>(ev,0xC),At<int16>(ev,0xE),At<int16>(ev,0x10),Name(At<uintptr_t>(ev,0x14)),At(ev,0x18),At(ev,0x1C),At(ev,0x20),At<uint8>(ev,0x24),
                     At<uint8>(ev,0x25)&0x1F,At(ev,0x28),At(ev,0x2C),At(ev,0x30),At(ev,0x34),At(ev,0x38),At(ev,0x3C),At(ev,0x40),Name(uintptr_t(p))}); return cfgAffects;
}
inline void __fastcall CompResp(void* calc,void*,CPed* p,void* resp,uint32 speak) {
    trace.push_back({"compute_damage_response",Name(At<uintptr_t>(calc,0)),At(calc,4),At(calc,8),At(calc,0xC),At<uint8>(calc,0x10),Name(uintptr_t(p)),
                     At(resp,0),At(resp,4),At(resp,8),speak&0xFF}); }
inline CPedGroup* __cdecl GetGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(uintptr_t(p))}); return reinterpret_cast<CPedGroup*>(cfgGroup); }
inline bool __fastcall IsLeaderS(void* m,void*,const CPed* p) { trace.push_back({"is_leader",Name(uintptr_t(m)),Name(uintptr_t(p))}); return cfgIsLeader; }
inline CPed* __fastcall GetLeaderS(void* m,void*) { trace.push_back({"get_leader",Name(uintptr_t(m))}); return reinterpret_cast<CPed*>(cfgLeader); }
inline CEvent* __fastcall GetEvOfType(void* g,void*,int type) { trace.push_back({"get_event_of_type",Name(uintptr_t(g)),type}); return reinterpret_cast<CEvent*>(cfgFoundEvent); }
inline CEvent* __fastcall EvAdd(void* g,void*,CEvent* e,bool valid) {
    trace.push_back({"event_group_add",Name(uintptr_t(g)),int(e->GetEventType()),Name(At<uintptr_t>(e,0x14)),Name(At<uintptr_t>(e,0x18)),At<uint8>(e,0x1C),valid}); return nullptr;
}
inline bool __fastcall AddEventS(void* intel,void*,CEvent* ge) {
    auto* inner=reinterpret_cast<CEvent*>(At<uintptr_t>(ge,0x10));
    trace.push_back({"group_add_event",Name(uintptr_t(intel)),int(ge->GetEventType()),Name(At<uintptr_t>(ge,0xC)),Name(uintptr_t(inner)),int(inner->GetEventType()),Region(inner,0x14,true)});
    return cfgAddEventRet;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// sigue los saltos de la ILT de la DLL sin entrar en la imagen del EXE
inline uintptr_t Body(uintptr_t p) {
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p;
}
// cuerpo de la DLL de una funcion enganchada (virtual o simple)
inline uintptr_t DllBody(const char* cls,const char* fn) {
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? Body(a) : 0;
}
// parches de entrada sin duplicados (se deshacen en orden inverso)
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    template<class Fp> void Both(uintptr_t exeAddr,Fp f,uintptr_t dllSym){ Add(exeAddr,f); if(dllSym)Add(Body(dllSym),f); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=1;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputeKnockOffBikeResponse",0x4B9FF0,0,-1,2048},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int k=0;k<3;++k)clean(EntOf(PedBuf(k)));
    clean(EntOf(vehA.data())); clean(EntOf(vehB.data()));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evhe-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evhe-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evhe");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evhe-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllAffects=DllBody("CEventDamage","AffectsPed");
    if(!dllAffects){Write({{"event","error"},{"reason","CEventDamage::AffectsPed body not found"}});return;}
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado del pool de tareas: se restaura en cada ejecucion (estados y m_LastFreeSlot) para que ambas reciban los mismos slots
    auto* poolHdr=reinterpret_cast<uint8*>(GetTaskPool()); auto* poolStor=*reinterpret_cast<uint8**>(poolHdr); auto* poolStates=*reinterpret_cast<uint8**>(poolHdr+4);
    const int32 poolCap=*reinterpret_cast<int32*>(poolHdr+8); const int32 poolLf0=*reinterpret_cast<int32*>(poolHdr+0xC);
    const std::vector<uint8> savedPoolStates(poolStates,poolStates+poolCap);
    // pool de eventos: estados, cabecera y almacenamiento
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto restoreEvents=[&]{ std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size()); std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size()); };
    const std::vector<uint8> savedRefs(reinterpret_cast<uint8*>(&CReferences::aRefs[0]),reinterpret_cast<uint8*>(&CReferences::aRefs[0])+sizeof(CReferences::aRefs));
    CReference* const savedEmpty=CReferences::pEmptyList;
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        Patches P;
        // CEventKnockOffBike (con un vehiculo falso estas rutas reventarian)
        P.Both(0x4B4AC0,SafePos,Addr(&CEventKnockOffBike::SetPedSafePosition));
        P.Both(0x4AFF60,OutCar,Addr(&CEventKnockOffBike::SetPedOutCar));
        P.Both(0x4B0020,CalcForces,Addr(&CEventKnockOffBike::CalcForcesAndAnims));
        P.Both(0x602590,InterestingAdd,Addr(&CInterestingEvents::Add));
        P.Add(0x4B35A0,AffectsS); P.Add(dllAffects,AffectsS); // CEventDamage::AffectsPed (virtual: se parchea el cuerpo de la DLL)
        P.Both(0x4B5AC0,CompResp,Addr(&CPedDamageResponseCalculator::ComputeDamageResponse));
        P.Both(0x5F7E80,GetGroup,Addr(&CPedGroups::GetPedsGroup));
        P.Both(0x5F69C0,IsLeaderS,Addr(&CPedGroupMembership::IsLeader));
        P.Both(0x5F69A0,GetLeaderS,Addr(&CPedGroupMembership::GetLeader));
        P.Both(0x5F7470,AddEventS,Addr(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent)));
        P.Both(0x4AB650,GetEvOfType,Addr(&CEventGroup::GetEventOfType));
        P.Both(0x4AB420,EvAdd,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)));
        P.Add(0x821B1E,FakeRand); P.Add(Addr(&CGeneral::GetRandomNumber),FakeRandomNumber);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto fl=[&]()->float{ static const float sp[]{0.f,-0.f,1.f,-1.f,0.5f,-0.5f,0.75f,100.f,-100.f,3.0e38f,-3.0e38f,1e-3f}; return next()%5==0 ? sp[next()%12] : uni(-40.f,40.f); };
                auto run=[&](bool candidate)->json {
                    // pools: estados, m_LastFreeSlot y huecos libres con 0xCD (el New del EXE no escribe en el objeto)
                    std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0;
                    for(int32 k=0;k<poolCap;++k)if(poolStates[k]&0x80)std::memset(poolStor+k*TASK_SLOT,0xCD,TASK_SLOT);
                    restoreEvents(); for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4B49u; if(!st)st=1; trace=json::array(); seed=uint32(i)*7u+uint32(f); rngCalls=0;
                    CleanRefs();
                    timeMs=next();
                    // ---------- tipo de evento: 8 KnockOffBike, 9 Damage, 35 VehicleDied, 79 VehicleOnFire ----------
                    const uint32 pick=next()%20; eventType=pick<7 ? 8 : pick<13 ? 9 : pick<16 ? 35 : 79;
                    // ---------- peatones ----------
                    static const float hp[]{50.f,1.f,0.f,-0.f,-20.f,100.f,1e-30f,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity()};
                    for(int k=0;k<3;++k) {
                        auto* b=PedBuf(k); std::memset(b,0,sizeof(CPed)); auto* p=Ped(k); p->SetType(ENTITY_TYPE_PED);
                        At(b,E_FLAGS)=next(); At(b,P_PHYS)=next(); At(b,P_FLAGS)=next();
                        At<float>(b,P_HEALTH)=next()%3==0 ? hp[next()%10] : next()%4==0 ? uni(-50.f,0.f) : uni(1.f,200.f);
                        std::memset(IntelBuf(k),0,sizeof(CPedIntelligence)); At<void*>(b,P_INTEL)=IntelBuf(k);
                    }
                    // matriz del ped A (derecha y adelante con valores variados)
                    std::memset(matA.data(),0,matA.size());
                    for(size_t o=0;o<0x20;o+=4)if(o!=0xC&&o!=0x1C)At<float>(matA.data(),o)=fl();
                    At<void*>(pedA.data(),E_MATRIX)=matA.data();
                    // ---------- vehiculos ----------
                    for(int k=0;k<2;++k) {
                        auto* b=VehBuf(k); std::memset(b,0,sizeof(CAutomobile)); EntOf(b)->SetType(ENTITY_TYPE_VEHICLE);
                        At(b,E_FLAGS)=next();
                        for(size_t o=0;o<12;o+=4)At<float>(b,V_MOVE+o)=fl();
                        static const uint32 vt[]{0,1,2,3,4,5,6,7,8,9,9,9,9,10,11,12};
                        At<uint32>(b,V_TYPE)=vt[next()%16]; At<uint8>(b,V_BIKEFLAGS)=uint8(next());
                        // conductor y pasajeros: A en un asiento con probabilidad alta, o ocupantes ajenos
                        uintptr_t seats[4]{}; for(auto& s:seats){ const uint32 c=next()%6; s=c<2 ? 0 : c==2 ? uintptr_t(pedB.data()) : c==3 ? uintptr_t(pedC.data()) : c==4 ? uintptr_t(pedA.data()) : 0; }
                        if(next()%100<70)seats[next()%4]=uintptr_t(pedA.data());
                        At<uintptr_t>(b,V_DRIVER)=seats[0]; At<uintptr_t>(b,V_PASSENGER0)=seats[1]; At<uintptr_t>(b,V_PASSENGER0+4)=seats[2]; At<uintptr_t>(b,V_PASSENGER0+8)=seats[3];
                    }
                    const uintptr_t pedVeh=uintptr_t(VehBuf(next()%2));
                    At<uintptr_t>(pedA.data(),P_VEHICLE)=pedVeh;
                    // ---------- grupo falso y evento ajeno ----------
                    std::memset(grp.data(),0xAB,grp.size()); std::memset(evOther.data(),0x5A,evOther.size());
                    // ---------- evento ----------
                    for(auto& b:event)b=uint8(next()%4==0 ? next() : 0); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    if(eventType==8) {
                        // CEventKnockOffBike falso: velocidades, dano, tiempo, ped, direccion, bits, tipo, puerta, vehiculo
                        for(size_t o=0xC;o<0x2C;o+=4)At<float>(event.data(),o)=fl();
                        At<uint32>(event.data(),0x2C)=next();
                        { uintptr_t t[]{0,uintptr_t(pedB.data()),uintptr_t(pedC.data()),uintptr_t(pedA.data())}; At<uintptr_t>(event.data(),0x30)=t[next()%4==0 ? 0 : 1+next()%3]; }
                        { static const uint8 dirs[]{0,1,2,3,0,1,2,3,0x80,0xFF,7}; At<uint8>(event.data(),0x34)=dirs[next()%11]; }
                        At<uint8>(event.data(),0x35)=uint8(next());
                        { static const uint8 types[]{0,53,53,53,54,54,0,0x31,0x33,0x36,0xFF,0x35,1}; At<uint8>(event.data(),0x36)=types[next()%13]; }
                        At<uint8>(event.data(),0x37)=uint8(next());
                        At<uintptr_t>(event.data(),0x38)=next()%12==0 ? 0 : uintptr_t(VehBuf(next()%2));
                    } else if(eventType==9) {
                        // CEventDamage falso: arma (dword con basura en los bytes altos) y direccion (byte)
                        { static const uint32 ws[]{0,1,0x35,0x36,0x31,0x33,22,0x1234u,0xFFFFFF35u,0xAB000036u,0x7F}; At<uint32>(event.data(),0x1C)=ws[next()%11]; }
                        { static const uint8 dirs[]{0,1,2,3,0,1,2,3,4,0x80,0xFF}; At<uint8>(event.data(),0x24)=dirs[next()%11]; }
                    }
                    // ---------- manejador ----------
                    const uintptr_t sentinel=next()%3==0 ? 0 : next()%2 ? uintptr_t(evOther.data()) : 0x13572468u;
                    std::memset(hbuf.data(),0,hbuf.size()); At<CPed*>(hbuf.data(),H_PED)=Ped(0); At<uintptr_t>(hbuf.data(),H_RESP)=sentinel;
                    // ---------- dependencias simuladas ----------
                    cfgSafePos=next()%100<80; cfgAffects=next()%2; cfgIsLeader=next()%3==0; cfgAddEventRet=next()%2;
                    { static const int32 anims[]{0xBF,0xBF,0xBF,0x6A,0x19,0x1B,0,1,-1,0x7FFFFFFF,0x100,200}; cfgAnim=anims[next()%12]; }
                    cfgGroup=next()%4==0 ? 0 : uintptr_t(grp.data());
                    { uintptr_t t[]{0,uintptr_t(pedB.data()),uintptr_t(pedC.data()),uintptr_t(pedA.data())}; cfgLeader=t[next()%4]; }
                    cfgFoundEvent=next()%2 ? uintptr_t(evOther.data()) : 0;
                    CTask* tactive=reinterpret_cast<CTask*>(0x11223344); CTask* tsimplest=reinterpret_cast<CTask*>(0x55667788); // no se usan
                    void* const ev=event.data(); void* const h=hbuf.data();
                    const auto occ=Occupancy();
                    for(auto* hk:hooks)hk->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*)>(fn.address)(h,ev,tactive,tsimplest);
                    for(auto* hk:hooks)hk->State(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x80,true)}});
                    const json resp=Name(At<uintptr_t>(h,H_RESP));
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int k=0;k<3;++k)refs.push_back(refList(EntOf(PedBuf(k)))); refs.push_back(refList(EntOf(vehA.data()))); refs.push_back(refList(EntOf(vehB.data())));
                    json types=json::array(); for(auto* t:fresh)types.push_back(t->GetTaskType());
                    json evPool={{"hdr",Hash(EvHdr(),0x14)},{"states",Hash(EvStates(),EvCap())},{"storage",Hash(EvStorage(),EvCap()*EVENT_SLOT)}};
                    json r={{"ret",{{"resp",resp},{"types",types}}},{"handler",Region(h,0x34)},{"tasks",tasks},
                            {"ped_a",EntHash(pedA)},{"ped_b",EntHash(pedB)},{"ped_c",EntHash(pedC)},{"mat",Hash(matA.data(),matA.size())},
                            {"veh_a",EntHash(vehA)},{"veh_b",EntHash(vehB)},
                            {"intel",{Hash(intelA.data(),intelA.size()),Hash(intelB.data(),intelB.size()),Hash(intelC.data(),intelC.size())}},
                            {"group",Hash(grp.data(),grp.size())},{"event",Region(event.data(),event.size(),true)},{"ev_other",Hash(evOther.data(),evOther.size())},
                            {"ev_pool",evPool},{"refs",refs},{"time",timeMs}};
                    r["trace"]=trace; r["rng"]=rngCalls; r["switches"]=switches;
                    // las tareas nuevas no se destruyen: se restauran los estados de los pools al empezar la siguiente ejecucion
                    std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0;
                    r["clean"]=Occupancy()==occupancyStart;
                    restoreEvents(); CleanRefs();
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
    CleanRefs(); std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0; restoreEvents();
    std::memcpy(&CReferences::aRefs[0],savedRefs.data(),savedRefs.size()); CReferences::pEmptyList=savedEmpty;
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime}});
}
}
