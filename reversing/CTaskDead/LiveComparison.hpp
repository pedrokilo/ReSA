#pragma once
// Banco dentro del motor: CTaskSimpleDead (constructor, destructor, Clone, GetTaskType, MakeAbortable, ProcessPed).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// peatones falsos de tamano real (m_pReferences = nullptr, matriz enmascarada), escaner de peds, jugadores falsos
// (FindPlayerPed), clump falso, asociacion de animacion falsa y reloj controlado.
// Se simulan y trazan (en el EXE y en la DLL): FindPlayerPed, CCarEnterExit::ComputeTargetDoorToExit,
// CAnimManager::BlendAnimation, CPed::SetPedState / IsPlayer / RemoveWeaponModel / RemoveWeaponAnims /
// CreateDeadPedWeaponPickups / CreateDeadPedMoney / DeadPedMakesTyresBloody, CWeaponInfo::GetWeaponInfo,
// GetEventGlobalGroup + CEventGroup::Add, CAccidentManager::GetInstance / ReportAccident, CLocalisation::Blood,
// CWaterLevel::GetWaterLevelNoWaves, CShadows::StoreStaticShadow y AddPermanentShadow.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// Constructor y destructor se comparan sobre buffers prellenados con 0xCD (sin vptr); Clone compara la tarea nueva del pool.
// CTimer (actual y previo) y gpBloodPoolTex se guardan y se restauran.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "TaskSimpleDead.h"
#include "Events/EventDeadPed.h"
#include "Events/EventGroup.h"
#include "AccidentManager.h"
#include "CarEnterExit.h"
#include "Localisation.h"
#include "Shadows.h"
#include "WaterLevel.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "Animation/AnimManager.h"
#include "PedIntelligence.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTaskDeadLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=6;          // 0 = muerto, 1..3 = entidades del escaner, 4 = jugador 0, 5 = jugador 1
constexpr size_t E_FLAGS=0x1C, E_MATRIX=0x14, E_CLUMP=0x18, E_TYPE=0x36, E_MOVE=0x44;
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_IK=0x50C, P_IKFLAGS=0x528, P_HEALTH=0x540, P_STAND=0x568, P_VEC=0x578,
                 P_VEHICLE=0x58C, P_WEAPONS=0x5A0, P_SLOT=0x718, P_TARGET=0x71C, P_DEATH=0x750;
constexpr size_t I_ENTITIES=0x130;
constexpr uintptr_t TIMER=0xB7CB84, TIMER_PREV=0xB7CB78, BLOOD_TEX=0xC40400, VT_DEAD=0x86DEA4;
constexpr size_t TASKBUF=0x20;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,0x40> clump{}, vehicle{}, assoc{}, texture{}, group{}, accident{}, wepInfo{}, event{};
alignas(16) inline std::array<uint8,TASKBUF> task{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }

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
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    if(auto s=in(intel.data(),intel.size(),"intel"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(vehicle.data(),vehicle.size(),"vehicle"))return *s; if(auto s=in(assoc.data(),assoc.size(),"assoc"))return *s;
    if(auto s=in(texture.data(),texture.size(),"texture"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(accident.data(),accident.size(),"accident"))return *s; if(auto s=in(wepInfo.data(),wepInfo.size(),"wepinfo"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(task.data(),task.size(),"task"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); return r; }
inline json SerTask(const void* p,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// configuracion por caso de las dependencias simuladas
inline uint8 bloodRet{}, playerRet{}, waterRet{}, player1Present{}; inline int32 doorRet{}; inline float waterLevel{};

// dependencias simuladas (EXE y DLL)
inline CPed* __cdecl FakeFindPlayer(int32 id) { trace.push_back({"find_player",id}); return id==1 ? (player1Present ? Ped(5) : nullptr) : Ped(4); }
inline int32 __cdecl FakeDoor(const CVehicle* v,const CPed* p) { trace.push_back({"door_to_exit",Name(v),Name(p),doorRet}); return doorRet; }
inline void* __cdecl FakeBlend(void* c,int32 g,int32 a,float d) { trace.push_back({"blend_animation",Name(c),g,a,F(d)}); return assoc.data(); }
inline void __fastcall FakeSetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(p),s}); }
inline bool __fastcall FakeIsPlayer(CPed* p,void*) { trace.push_back({"is_player",Name(p),playerRet}); return playerRet!=0; }
inline void* __cdecl FakeWeaponInfo(int32 type,uint32 skill) { trace.push_back({"weapon_info",type,skill&0xFF}); return wepInfo.data(); }
inline void __fastcall FakeRemoveModel(CPed* p,void*,int32 m) { trace.push_back({"remove_weapon_model",Name(p),m}); }
inline void __fastcall FakeRemoveAnims(CPed* p,void*,int32 a,float d) { trace.push_back({"remove_weapon_anims",Name(p),a,F(d)}); }
inline void __fastcall FakePickups(CPed* p,void*) { trace.push_back({"dead_ped_weapon_pickups",Name(p)}); }
inline void __fastcall FakeMoney(CPed* p,void*) { trace.push_back({"dead_ped_money",Name(p)}); }
inline void __fastcall FakeTyres(CPed* p,void*) { trace.push_back({"dead_ped_tyres_bloody",Name(p)}); }
inline void* __cdecl FakeGlobalGroup() { trace.push_back({"global_group"}); return group.data(); }
inline void* __fastcall FakeGroupAdd(void* g,void*,void* ev,bool valid) {
    trace.push_back({"group_add",Name(g),Name(At<uint32>(ev,0x14)),At<uint8>(ev,0x18),At<uint32>(ev,0x1C),valid}); return nullptr;
}
inline void* __cdecl FakeAccidentMgr() { trace.push_back({"accident_instance"}); return accident.data(); }
inline void __fastcall FakeReport(void* m,void*,CPed* p) { trace.push_back({"report_accident",Name(m),Name(p)}); }
inline bool __cdecl FakeBlood() { trace.push_back({"blood",bloodRet}); return bloodRet!=0; }
inline bool __cdecl FakeWater(float x,float y,float z,float* out,float* a,float* b) {
    trace.push_back({"water_level",F(x),F(y),F(z),out!=nullptr,a!=nullptr,b!=nullptr});
    if(waterRet&&out)*out=waterLevel; return waterRet!=0;
}
inline bool __cdecl FakeStoreShadow(uint32 id,uint32 type,void* tex,const CVector* pos,float fx,float fy,float sx,float sy,int32 inten,uint32 r,uint32 g,uint32 b,float zd,float scale,float draw,uint32 temp,float up) {
    trace.push_back({"store_static_shadow",Name(id),type&0xFF,Name(tex),F(pos->x),F(pos->y),F(pos->z),F(fx),F(fy),F(sx),F(sy),inten&0xFFFF,r&0xFF,g&0xFF,b&0xFF,F(zd),F(scale),F(draw),temp&0xFF,F(up)});
    return true;
}
inline void __cdecl FakePermShadow(uint32 type,void* tex,const CVector* pos,float tx,float ty,float rx,float ry,int32 inten,uint32 r,uint32 g,uint32 b,float draw,uint32 time,float up) {
    trace.push_back({"add_permanent_shadow",type&0xFF,Name(tex),F(pos->x),F(pos->y),F(pos->z),F(tx),F(ty),F(rx),F(ry),inten&0xFFFF,r&0xFF,g&0xFF,b&0xFF,F(draw),time,F(up)});
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=6;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleDead","Constructor",0x630590,0,-1,256},
    {"CTaskSimpleDead","Destructor",0x6305F0,0,-1,64},
    {"CTaskSimpleDead","Clone",0,0x86DEA4,1,256},
    {"CTaskSimpleDead","GetTaskType",0,0x86DEA4,4,64},
    {"CTaskSimpleDead","MakeAbortable",0,0x86DEA4,6,128},
    {"CTaskSimpleDead","ProcessPed",0,0x86DEA4,7,1024},
}};

inline void CleanRefs() {
    for(auto& p:peds){ auto* e=reinterpret_cast<CEntity*>(p.data()); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); }
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctaskdead-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctaskdead-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctaskdead");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctaskdead-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: reloj actual y previo, textura de sangre
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{TIMER_PREV,4},{BLOOD_TEX,4}};
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
        EntryPatch a1(bit_cast<uintptr_t>(&FindPlayerPed),FakeFindPlayer), a1e(0x56E210,FakeFindPlayer);
        EntryPatch a2(bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit),FakeDoor), a2e(0x64F110,FakeDoor);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), a3e(0x4D4610,FakeBlend);
        EntryPatch a4(bit_cast<uintptr_t>(&CPed::SetPedState),FakeSetPedState), a4e(0x5E4500,FakeSetPedState);
        EntryPatch a5(bit_cast<uintptr_t>(&CPed::IsPlayer),FakeIsPlayer), a5e(0x5DF8F0,FakeIsPlayer);
        EntryPatch a6(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),FakeWeaponInfo), a6e(0x743C60,FakeWeaponInfo);
        EntryPatch a7(bit_cast<uintptr_t>(&CPed::RemoveWeaponModel),FakeRemoveModel), a7e(0x5E3990,FakeRemoveModel);
        EntryPatch a8(bit_cast<uintptr_t>(&CPed::RemoveWeaponAnims),FakeRemoveAnims), a8e(0x5F0250,FakeRemoveAnims);
        EntryPatch a9(bit_cast<uintptr_t>(&CPed::CreateDeadPedWeaponPickups),FakePickups), a9e(0x4591D0,FakePickups);
        EntryPatch b1(bit_cast<uintptr_t>(&CPed::CreateDeadPedMoney),FakeMoney), b1e(0x4590F0,FakeMoney);
        EntryPatch b2(bit_cast<uintptr_t>(&CPed::DeadPedMakesTyresBloody),FakeTyres), b2e(0x6B4200,FakeTyres);
        EntryPatch b3(bit_cast<uintptr_t>(&GetEventGlobalGroup),FakeGlobalGroup), b3e(0x4ABA50,FakeGlobalGroup);
        EntryPatch b4(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeGroupAdd), b4e(0x4AB420,FakeGroupAdd);
        EntryPatch b5(bit_cast<uintptr_t>(&CAccidentManager::GetInstance),FakeAccidentMgr), b5e(0x56CF20,FakeAccidentMgr);
        EntryPatch b6(bit_cast<uintptr_t>(&CAccidentManager::ReportAccident),FakeReport), b6e(0x56CE80,FakeReport);
        EntryPatch b7(bit_cast<uintptr_t>(&CLocalisation::Blood),FakeBlood), b7e(0x56D230,FakeBlood);
        EntryPatch b8(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),FakeWater), b8e(0x6E8580,FakeWater);
        EntryPatch b9(bit_cast<uintptr_t>(&CShadows::StoreStaticShadow),FakeStoreShadow), b9e(0x70BA00,FakeStoreShadow);
        EntryPatch c1(bit_cast<uintptr_t>(&CShadows::AddPermanentShadow),FakePermShadow), c1e(0x706F60,FakePermShadow);

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
                    if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4D44u; if(!st)st=1; trace=json::array(); freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    const uint32 now=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 frame=next()%5==0 ? next()%2 : next()%4==0 ? next()%1500 : next()%80;
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=now; At<uint32>(reinterpret_cast<void*>(TIMER_PREV),0)=now-frame;
                    At<void*>(reinterpret_cast<void*>(BLOOD_TEX),0)=texture.data();
                    std::memset(intel.data(),0,intel.size());
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;
                    std::memset(vehicle.data(),0xCD,vehicle.size()); std::memset(texture.data(),0xCD,texture.size());
                    std::memset(group.data(),0xCD,group.size()); std::memset(accident.data(),0xCD,accident.size());
                    std::memset(event.data(),0xCD,event.size());
                    for(auto& b:assoc)b=uint8(next());
                    std::memset(wepInfo.data(),0xCD,wepInfo.size()); At<int32>(wepInfo.data(),0xC)=int32(next()%400)-20;
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        p[E_TYPE]=3; At<void*>(p,E_CLUMP)=clump.data(); At<void*>(p,P_INTEL)=intel.data();
                        // flags de entidad: bit 0 = usa colision
                        At<uint32>(p,E_FLAGS)=next(); if(n==0&&next()%4)At<uint32>(p,E_FLAGS)|=1u;
                        if(n==0||next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            At<CVector>(pedMats[n].data(),0x0)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x10)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(0.f,30.f)}; }
                        else At<CVector>(p,4)=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(0.f,30.f)};
                        At<CVector>(p,E_MOVE)=CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)};
                        At<uint32>(p,P_FLAGS)=next(); At<uint32>(p,P_IKFLAGS)=next(); At<uint32>(p,P_DEATH)=next();
                        At<float>(p,P_HEALTH)=uni(0.f,100.f); At<float>(p,P_IK+0xC)=uni(-1.f,1.f); At<float>(p,P_IK+0x14)=uni(-1.f,1.f);
                        const uint32 vm=next()%8;
                        At<CVector>(p,P_VEC)=vm==0 ? CVector{1.f,-1.f,1.f} : vm==1 ? CVector{uni(-1.f,1.f),qnan,uni(-1.f,1.f)} : CVector{uni(-1.5f,1.5f),uni(-1.5f,1.5f),uni(-1.5f,1.5f)};
                        At<void*>(p,P_STAND)=next()%3==0 ? Ped(3) : nullptr;
                        At<void*>(p,P_VEHICLE)=next()%4 ? vehicle.data() : nullptr;
                        for(size_t b=0;b<13;++b)At<uint32>(p,P_WEAPONS+b*0x1C)=next()%50;
                        p[P_SLOT]=uint8(next()%5==0 ? 0xF0+next()%16 : next()%13);
                        At<void*>(p,P_TARGET)=nullptr;
                    }
                    // puntero al peatón: se vuelve a establecer porque Ped(3) se usa antes de rellenar su buffer
                    At<void*>(peds[0].data(),P_STAND)=next()%3==0 ? static_cast<void*>(peds[3].data()) : nullptr;
                    // el peatón muerto: banderas de pie / en vehiculo con probabilidad alta
                    { auto& fl=At<uint32>(peds[0].data(),P_FLAGS); fl=(fl&~0x103u)|(next()%2?1u:0u)|(next()%2?2u:0u)|(next()%3==0?0x100u:0u); }
                    // escaner de peds: 16 entradas
                    for(int k=0;k<16;++k){ const uint32 m=next()%5; At<void*>(intel.data(),I_ENTITIES+4*k)=m==0 ? nullptr : m==4 ? peds[0].data() : peds[1+next()%3].data(); }
                    { const CVector base=reinterpret_cast<CPed*>(peds[0].data())->GetPosition();
                      for(int n=1;n<4;++n){ const CVector q{base.x+uni(-1.f,1.f),base.y+uni(-1.f,1.f),base.z+uni(-0.5f,0.5f)};
                          auto* p=peds[n].data(); if(At<void*>(p,E_MATRIX))At<CVector>(pedMats[n].data(),0x30)=q; else At<CVector>(p,4)=q; } }
                    // jugadores: objetivo bloqueado
                    player1Present=uint8(next()%3!=0);
                    auto tgt=[&]()->void*{ switch(next()%4){ case 0: return peds[0].data(); case 1: return peds[1].data(); default: return nullptr; } };
                    At<void*>(peds[4].data(),P_TARGET)=tgt(); At<void*>(peds[5].data(),P_TARGET)=tgt();
                    bloodRet=uint8(next()%5!=0); playerRet=uint8(next()%3==0); waterRet=uint8(next()%3!=0);
                    { static const int32 doors[]{10,11,8,9,0,18,12}; doorRet=doors[next()%7]; }
                    { const float z=reinterpret_cast<CPed*>(peds[0].data())->GetPosition().z; const uint32 w=next()%6;
                      waterLevel=w==0 ? z : w==1 ? qnan : w<=3 ? z+uni(0.f,3.f) : z-uni(0.f,3.f); }
                    std::memset(task.data(),0xCD,task.size());

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<CTask*> roots;
                    auto* o=task.data(); auto* p0=Ped(0);
                    auto collectFresh=[&](CTask* r){
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    // tarea muerta: vptr del EXE, padre nulo, tiempo y byte de flags (bits 0..2 + bits altos al azar)
                    auto setDead=[&](uint32 deathTime,bool first,bool drowned,bool puddle){
                        std::memset(o,0xCD,TASKBUF); At<uint32>(o,0)=VT_DEAD; At<uint32>(o,4)=0; At<uint32>(o,8)=deathTime;
                        o[0xC]=uint8((next()&0xF8u)|(first?1u:0u)|(drowned?2u:0u)|(puddle?4u:0u));
                    };
                    if(nm=="Constructor") {
                        std::memset(o,0xCD,TASKBUF);
                        const uint32 dt=next()%4==0 ? next() : next()%2000000; const bool dr=next()%2;
                        ret=Name(reinterpret_cast<void*(__thiscall*)(void*,uint32,bool)>(target)(o,dt,dr));
                        for(auto* h:hooks)h->State(false);
                        out["object"]=Region(o,TASKBUF,true); out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                    }
                    else if(nm=="Destructor") {
                        setDead(next(),next()%2,next()%2,next()%2);
                        reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        for(auto* h:hooks)h->State(false);
                        out["object"]=Region(o,TASKBUF,true);
                    }
                    else if(nm=="GetTaskType") {
                        setDead(next(),next()%2,next()%2,next()%2);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); out["object"]=Region(o,TASKBUF,true);
                    }
                    else if(nm=="MakeAbortable") {
                        setDead(next(),next()%2,next()%2,next()%2);
                        const int32 prio=int32(next()%4); const CEvent* ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev);
                        out["object"]=Region(o,TASKBUF,true);
                    }
                    else if(nm=="Clone") {
                        // el EXE pasa el bit de ahogado (bit 1) al constructor, no el de primera vez
                        setDead(next()%4==0 ? next() : next()%2000000,next()%2,next()%2,next()%2);
                        auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r);
                        out["object"]=Region(o,TASKBUF,true);
                    }
                    else { // ProcessPed
                        // tiempo transcurrido desde la muerte: antes de 2 s, justo en 2 s, cruce de 2 s en este fotograma,
                        // rampa 2..7 s, mas de 7 s, y reloj por detras de la muerte (envuelve)
                        uint32 elapsed;
                        switch(next()%12){
                            case 0: elapsed=next()%2000; break;
                            case 1: elapsed=2000; break;
                            case 2: elapsed=2000+next()%(frame+3); break;
                            case 3: elapsed=2000+frame; break;
                            case 4: case 5: elapsed=2001+next()%5000; break;
                            case 6: elapsed=7000+next()%3; break;
                            case 7: elapsed=6998+next()%4; break;
                            case 8: elapsed=7001+next()%20000; break;
                            case 9: elapsed=0u-(1u+next()%1000); break;
                            case 10: elapsed=4990+next()%20; break;
                            default: elapsed=next()%9000; break;
                        }
                        const bool first=next()%3==0, drowned=next()%4==0, puddle=next()%5==0;
                        setDead(now-elapsed,first,drowned,puddle);
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0);
                        out["object"]=Region(o,TASKBUF,true);
                        out["globals"]=json::array({At<uint32>(reinterpret_cast<void*>(TIMER),0),At<uint32>(reinterpret_cast<void*>(TIMER_PREV),0)});
                    }
                    for(auto* h:hooks)h->State(false);
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    json world=json::array(); for(int n=0;n<NPEDS;++n){ world.push_back(Region(peds[n].data(),peds[n].size())); world.push_back(Region(pedMats[n].data(),pedMats[n].size())); }
                    out["world"]=world; out["assoc"]=Region(assoc.data(),assoc.size()); out["intel"]=Region(intel.data(),intel.size());
                    out["refs"]=AllRefs();
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace;
                    out["clean"]=Occupancy()==occ;
                    restoreGlobals();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json odiff=json::array(); // primeras palabras distintas del mundo, para localizar el campo
                    if(original.contains("world")&&candidate.contains("world")&&original["world"]!=candidate["world"])
                        for(size_t a=0;a<original["world"].size()&&odiff.size()<8;++a)for(size_t w=0;w<original["world"][a].size()&&odiff.size()<8;++w)
                            if(original["world"][a][w]!=candidate["world"][a][w])odiff.push_back({{"block",a},{"offset",w*4},{"original",original["world"][a][w]},{"candidate",candidate["world"][a][w]}});
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"world_diff",odiff},
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
