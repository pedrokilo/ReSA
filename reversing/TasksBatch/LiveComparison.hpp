#pragma once
// Banco dentro del motor: CTaskComplexStareAtPed, StealCar, StuckInAir, TrackEntity y TreatAccident.
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato (activados) con fixtures
// identicos: peatones/vehiculo/grupo/accidente en memoria propia (matriz e inteligencia incluidas),
// subtareas falsas con vtable propia y dependencias externas controladas (IK, voz, bloques de
// animacion, streaming, pad, huesos, asociaciones de animacion, RNG). Se compara la memoria de todos
// los fixtures (punteros a tareas nuevas traducidos), las tareas creadas, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexStareAtPed.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexStuckInAir.h"
#include "TaskComplexTrackEntity.h"
#include "TaskComplexTreatAccident.h"
#include "TaskComplexGangLeader.h"
#include "TaskSimpleGoToPointFine.h"
#include "Ragdoll/IKChainManager.h"
#include "Accident.h"
#include "Pools/TaskPool.h"
#include "TaskSimpleFall.h"
#include "../ProbeRuntime.hpp"

namespace TasksLive {
inline bool done{}; inline std::ofstream log;
inline json trace; inline int seed{}; inline int rngCalls{}; inline bool trapped{};
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }

// ---------------------------------------------------------------- fixtures
constexpr int PEDS=4, FAKES=6, ASSOCS=6;
struct Region { const char* name; uint8* data; size_t size; };
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,PEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,PEDS> intel{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,PEDS+2> mats{};      // PEDS..: vehiculo, entidad
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> veh{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,sizeof(CAccident)+8> accident{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> playerData{};                                       // tarea bajo prueba
alignas(16) inline std::array<std::array<uint8,0x100>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,ASSOCS> assocs{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh() { return reinterpret_cast<CVehicle*>(veh.data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline std::vector<Region> Regions() {
    std::vector<Region> r;
    static const char* pn[]{"ped0","ped1","ped2","ped3"}; static const char* in[]{"intel0","intel1","intel2","intel3"};
    static const char* mn[]{"mat0","mat1","mat2","mat3","matVeh","matEnt"}; static const char* fn[]{"fake0","fake1","fake2","fake3","fake4","fake5"};
    static const char* an[]{"assoc0","assoc1","assoc2","assoc3","assoc4","assoc5"};
    for(int n=0;n<PEDS;++n){r.push_back({pn[n],peds[n].data(),peds[n].size()});r.push_back({in[n],intel[n].data(),intel[n].size()});}
    for(int n=0;n<PEDS+2;++n)r.push_back({mn[n],mats[n].data(),mats[n].size()});
    for(int n=0;n<FAKES;++n)r.push_back({fn[n],fakes[n].data(),fakes[n].size()});
    for(int n=0;n<ASSOCS;++n)r.push_back({an[n],assocs[n].data(),assocs[n].size()});
    r.push_back({"veh",veh.data(),veh.size()}); r.push_back({"group",group.data(),group.size()});
    r.push_back({"accident",accident.data(),accident.size()}); r.push_back({"object",object.data(),object.size()}); r.push_back({"playerData",playerData.data(),playerData.size()});
    return r;
}
inline std::unordered_map<uintptr_t,int> freshIds; // tareas nuevas del pool -> id
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& r:Regions())if(v>=uintptr_t(r.data)&&v<uintptr_t(r.data)+r.size)return std::string(r.name)+"+"+std::to_string(v-uintptr_t(r.data));
    if(v==uintptr_t(FindPlayerPed()))return "player";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }

// ---------------------------------------------------------------- subtareas falsas
struct FakeInfo { int type=-1; bool simple=false; bool abortable=true; };
inline std::array<FakeInfo,FAKES> fakeInfo{};
inline int FakeId(const void* p) { for(int n=0;n<FAKES;++n)if(p==fakes[n].data())return n; return -1; }
inline void* __fastcall FDtor(CTask* t,void*,unsigned flags) { trace.push_back({"delete",Name(t),flags}); return t; }
inline CTask* __fastcall FSub(CTask* t,void*) { return fakeInfo[FakeId(t)].simple ? nullptr : *reinterpret_cast<CTask**>(reinterpret_cast<uint8*>(t)+8); } // las simples no tienen subtarea
inline bool __fastcall FSimple(CTask* t,void*) { return fakeInfo[FakeId(t)].simple; }
inline int __fastcall FType(CTask* t,void*) { return fakeInfo[FakeId(t)].type; }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { trace.push_back({"make_abortable",Name(t),Name(p),prio,Name(e)}); return fakeInfo[FakeId(t)].abortable; }
template<int N> void* __fastcall FTrap(CTask* t,void*) { trapped=true; trace.push_back({"unexpected_task_slot",N,Name(t)}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// ---------------------------------------------------------------- dependencias controladas
inline int lookAtEntity{-1}; inline bool isLooking{}; inline bool shouldLoadGang{}; inline int16 padX{},padY{}; inline bool padJump{};
inline uint32 assocMask{};
inline void __fastcall LookAt(IKChainManager_c*,void*,const char* purpose,CPed* ped,CEntity* target,int32 time,eBoneTag32 bone,CVector* offset,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"look_at",purpose ? purpose:"",Name(ped),Name(target),time==int32(uintptr_t(&gDefaultTaskTime)) ? json("&gDefaultTaskTime"):json(time),int32(bone),Name(offset),torso,std::bit_cast<uint32>(speed),blend,prio,force});
}
inline CEntity* __fastcall GetLookAtEntity(IKChainManager_c*,void*,CPed* ped) { trace.push_back({"get_look_at_entity",Name(ped)}); return lookAtEntity<0 ? nullptr : reinterpret_cast<CEntity*>(peds[lookAtEntity].data()); }
inline bool __fastcall IsLooking(const IKChainManager_c*,void*,CPed* ped) { trace.push_back({"is_looking",Name(ped)}); return isLooking; }
inline void __fastcall AbortLookAt(IKChainManager_c*,void*,CPed* ped,uint32 blend) { trace.push_back({"abort_look_at",Name(ped),blend}); }
inline bool __cdecl ShouldLoadGangAnims() { trace.push_back({"should_load_gang_anims"}); return shouldLoadGang; }
inline void __cdecl DoGangAbuseSpeech(CPed* a,CPed* b) { trace.push_back({"gang_abuse_speech",Name(a),Name(b)}); }
inline void __cdecl AddAnimBlockRef(int32 i) { trace.push_back({"add_anim_block_ref",i}); }
inline void __cdecl RemoveAnimBlockRef(int32 i) { trace.push_back({"remove_anim_block_ref",i}); }
inline void __cdecl RequestModel(int32 m,int32 f) { trace.push_back({"request_model",m,f}); }
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c) { trace.push_back({"say",Name(p),ctx,delay,std::bit_cast<uint32>(prob),a,b,c}); return -1; }
inline void __fastcall BonePos(CPed* p,void*,CVector* out,int bone,bool update) {
    trace.push_back({"bone_position",Name(p),bone,update});
    const float k=float(bone)+float(seed%17); *out=CVector{k*0.37f-3.f,k*0.11f+1.f,k*0.05f};
}
// Sobrecarga que devuelve CVector (retorno por puntero oculto: thiscall(this, out, bone, update)).
inline CVector* __fastcall BonePosRet(CPed* p,void*,CVector* out,int bone,bool update) { BonePos(p,nullptr,out,bone,update); return out; }
inline int16 __fastcall PadX(CPad*,void*) { return padX; }
inline int16 __fastcall PadY(CPad*,void*) { return padY; }
inline bool __fastcall PadJump(CPad*,void*) { trace.push_back({"jump_just_down"}); return padJump; }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) {
    trace.push_back({"get_association",Name(c),id});
    const int k=int(id)-ANIM_ID_JUMP_LAUNCH; return k>=0&&k<ASSOCS&&(assocMask>>k&1) ? reinterpret_cast<CAnimBlendAssociation*>(assocs[k].data()) : nullptr;
}
inline int __cdecl FakeRand() { return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// ---------------------------------------------------------------- utilidades
inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline size_t TaskSize(int type) { // tamanos de los tipos que se crean (new en el EXE)
    switch(type) {
    case TASK_SIMPLE_STAND_STILL: return 0x20; case TASK_COMPLEX_TURN_TO_FACE_ENTITY: return 0x28; case TASK_SIMPLE_NAMED_ANIM: return 0x28;
    case TASK_COMPLEX_FLEE_POINT: return 0x34; case TASK_COMPLEX_JUMP: return 0x14; case TASK_COMPLEX_FALL_AND_GET_UP: return 0x18;
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: return 0x24; case TASK_SIMPLE_PAUSE: return 0x18; case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: return 0x50;
    case TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER: return 0x0C; case TASK_SIMPLE_GO_TO_POINT_FINE: return 0x28; case TASK_COMPLEX_FOLLOW_NODE_ROUTE: return 0x60;
    case TASK_SIMPLE_GIVE_CPR: return 0x10; case TASK_NONE: return 0x08; case TASK_SIMPLE_ACHIEVE_HEADING: return 0x18;
    case TASK_COMPLEX_STARE_AT_PED: return 0x24; case TASK_COMPLEX_STEAL_CAR: return 0x1C; case TASK_COMPLEX_STUCK_IN_AIR: return 0x0C;
    case TASK_COMPLEX_TRACK_ENTITY: return 0x54; case TASK_COMPLEX_TREAT_ACCIDENT: return 0x10;
    default: return 0x08;
    }
}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int slot; int cases; uintptr_t vtbl; size_t size; };
constexpr uintptr_t VT_STARE=0x86FA2C, VT_STEAL=0x86E7FC, VT_STUCK=0x870608, VT_TRACK=0x86F998, VT_TREAT=0x86F4EC;
// slot >= 0: llamada por la vtable del EXE (hook VMT); slot < 0: direccion directa.
inline const std::array<Fn,39> fns{{
    {"CTaskComplexStareAtPed","Constructor",0x660270,-1,128,VT_STARE,sizeof(CTaskComplexStareAtPed)},{"CTaskComplexStareAtPed","Destructor",0x660310,-1,128,VT_STARE,sizeof(CTaskComplexStareAtPed)},
    {"CTaskComplexStareAtPed","Clone",0,1,256,VT_STARE,sizeof(CTaskComplexStareAtPed)},{"CTaskComplexStareAtPed","GetTaskType",0,4,32,VT_STARE,sizeof(CTaskComplexStareAtPed)},
    {"CTaskComplexStareAtPed","CreateNextSubTask",0,8,256,VT_STARE,sizeof(CTaskComplexStareAtPed)},{"CTaskComplexStareAtPed","CreateFirstSubTask",0,9,256,VT_STARE,sizeof(CTaskComplexStareAtPed)},
    {"CTaskComplexStareAtPed","ControlSubTask",0,10,2048,VT_STARE,sizeof(CTaskComplexStareAtPed)},
    {"CTaskComplexStealCar","Constructor",0x63B7D0,-1,128,VT_STEAL,sizeof(CTaskComplexStealCar)},{"CTaskComplexStealCar","Destructor",0x63B850,-1,128,VT_STEAL,sizeof(CTaskComplexStealCar)},
    {"CTaskComplexStealCar","CreateSubTask",0x640F30,-1,512,VT_STEAL,sizeof(CTaskComplexStealCar)},{"CTaskComplexStealCar","Clone",0,1,128,VT_STEAL,sizeof(CTaskComplexStealCar)},
    {"CTaskComplexStealCar","GetTaskType",0,4,32,VT_STEAL,sizeof(CTaskComplexStealCar)},{"CTaskComplexStealCar","CreateNextSubTask",0,8,1024,VT_STEAL,sizeof(CTaskComplexStealCar)},
    {"CTaskComplexStealCar","CreateFirstSubTask",0,9,256,VT_STEAL,sizeof(CTaskComplexStealCar)},{"CTaskComplexStealCar","ControlSubTask",0,10,1024,VT_STEAL,sizeof(CTaskComplexStealCar)},
    {"CTaskComplexStuckInAir","Clone",0,1,64,VT_STUCK,sizeof(CTaskComplexStuckInAir)},{"CTaskComplexStuckInAir","ControlSubTask",0,10,2048,VT_STUCK,sizeof(CTaskComplexStuckInAir)},
    {"CTaskComplexStuckInAir","CreateFirstSubTask",0,9,256,VT_STUCK,sizeof(CTaskComplexStuckInAir)},{"CTaskComplexStuckInAir","CreateNextSubTask",0,8,1024,VT_STUCK,sizeof(CTaskComplexStuckInAir)},
    {"CTaskComplexStuckInAir","CreateSubTask",0x67BA80,-1,1024,VT_STUCK,sizeof(CTaskComplexStuckInAir)},{"CTaskComplexStuckInAir","GetTaskType",0,4,32,VT_STUCK,sizeof(CTaskComplexStuckInAir)},
    {"CTaskComplexTrackEntity","CalcMoveRatio",0x65F930,-1,2048,VT_TRACK,sizeof(CTaskComplexTrackEntity)},{"CTaskComplexTrackEntity","CalcTargetPos",0x65F780,-1,1024,VT_TRACK,sizeof(CTaskComplexTrackEntity)},
    {"CTaskComplexTrackEntity","Clone",0,1,128,VT_TRACK,sizeof(CTaskComplexTrackEntity)},{"CTaskComplexTrackEntity","Constructor",0x65F3B0,-1,128,VT_TRACK,sizeof(CTaskComplexTrackEntity)},
    {"CTaskComplexTrackEntity","ControlSubTask",0,10,2048,VT_TRACK,sizeof(CTaskComplexTrackEntity)},{"CTaskComplexTrackEntity","CreateFirstSubTask",0,9,512,VT_TRACK,sizeof(CTaskComplexTrackEntity)},
    {"CTaskComplexTrackEntity","CreateNextSubTask",0,8,1024,VT_TRACK,sizeof(CTaskComplexTrackEntity)},{"CTaskComplexTrackEntity","Destructor",0x65F460,-1,128,VT_TRACK,sizeof(CTaskComplexTrackEntity)},
    {"CTaskComplexTrackEntity","GetTaskType",0,4,32,VT_TRACK,sizeof(CTaskComplexTrackEntity)},{"CTaskComplexTrackEntity","MakeAbortable",0,6,256,VT_TRACK,sizeof(CTaskComplexTrackEntity)},
    {"CTaskComplexTrackEntity","SetOffsetPos",0x65F760,-1,128,VT_TRACK,sizeof(CTaskComplexTrackEntity)},
    {"CTaskComplexTreatAccident","Clone",0,1,64,VT_TREAT,sizeof(CTaskComplexTreatAccident)},{"CTaskComplexTreatAccident","ComputeHeading",0x658AF0,-1,512,VT_TREAT,sizeof(CTaskComplexTreatAccident)},
    {"CTaskComplexTreatAccident","Constructor",0x658AB0,-1,64,VT_TREAT,sizeof(CTaskComplexTreatAccident)},{"CTaskComplexTreatAccident","ControlSubTask",0,10,64,VT_TREAT,sizeof(CTaskComplexTreatAccident)},
    {"CTaskComplexTreatAccident","CreateFirstSubTask",0,9,512,VT_TREAT,sizeof(CTaskComplexTreatAccident)},{"CTaskComplexTreatAccident","CreateNextSubTask",0,8,512,VT_TREAT,sizeof(CTaskComplexTreatAccident)},
    {"CTaskComplexTreatAccident","CreateSubTask",0x659E90,-1,512,VT_TREAT,sizeof(CTaskComplexTreatAccident)},
}};

// Instantanea completa en memoria (para comparar) y su version JSON (solo para fallos).
struct Snap { std::vector<int64> words; json ret; json tasks; json trace; int rng{}; bool clean{}; };
// Bytes sin valor definido en el original: relleno y campos que su constructor no inicializa
// (se calculan siempre antes de usarse). Solo se aplican a objetos recien construidos.
inline uint32 ConstructedMask(int type,size_t off) {
    uint32 m=0xFFFFFFFF; auto drop=[&](size_t from,size_t to){ for(size_t b=from;b<to;++b)if(b>=off&&b<off+4)m&=~(0xFFu<<((b-off)*8)); };
    if(type==TASK_COMPLEX_STARE_AT_PED){ drop(0x22,0x24); drop(0x26,0x28); }            // relleno del CTaskTimer y final
    if(type==TASK_COMPLEX_STEAL_CAR){ drop(0x1A,0x1C); }                                 // relleno del CTaskTimer
    if(type==TASK_COMPLEX_TRACK_ENTITY){ drop(0x1D,0x20); drop(0x2D,0x30); drop(0x3A,0x3C); drop(0x40,0x54); } // relleno; float40/m_goToPos/m_distToTargetSq
    return m;
}
inline int64 Encode(uint32 v) {
    if(auto it=freshIds.find(v);it!=freshIds.end())return (int64(1)<<40)|it->second;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return int64(2)<<40; // entrada de CReferences: el orden de la lista libre varia
    return v;
}
// Lista de referencias de cada entidad fixture, por direccion destino (estable).
inline void AppendRefs(std::vector<int64>& w) {
    auto one=[&](CEntity* e){ for(auto* r=e->m_pReferences;r;r=r->m_pNext){ const auto n=Name(r->m_ppEntity); w.push_back(int64(std::hash<std::string>{}(n.dump()))); } w.push_back(-1); };
    for(int n=0;n<PEDS;++n)one(Ped(n)); one(Veh());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,39> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    { json r=json::object(); for(auto& g:Regions())r[g.name]={uintptr_t(g.data),g.size}; Write({{"event","regions"},{"regions",r}}); }
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto savedTime=CTimer::m_snTimeInMilliseconds; const auto savedStep=CTimer::ms_fTimeStep;
    const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed)); const auto occupancyStart=Occupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<bool,39> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using namespace ReversingProbe;
        EntryPatch p1(std::bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), p1e(0x618970,LookAt);
        EntryPatch p2(std::bit_cast<uintptr_t>(&IKChainManager_c::GetLookAtEntity),GetLookAtEntity), p2e(0x6181D0,GetLookAtEntity);
        EntryPatch p3(std::bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),IsLooking), p3e(0x6181A0,IsLooking);
        EntryPatch p4(std::bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLookAt), p4e(0x618280,AbortLookAt);
        EntryPatch p5(std::bit_cast<uintptr_t>(&CTaskComplexGangLeader::ShouldLoadGangAnims),ShouldLoadGangAnims), p5e(0x65E7F0,ShouldLoadGangAnims);
        EntryPatch p6(std::bit_cast<uintptr_t>(&CTaskComplexGangLeader::DoGangAbuseSpeech),DoGangAbuseSpeech), p6e(0x65E860,DoGangAbuseSpeech);
        EntryPatch p7(std::bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),AddAnimBlockRef), p7e(0x4D3FB0,AddAnimBlockRef);
        EntryPatch p8(std::bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),RemoveAnimBlockRef), p8e(0x4D3FD0,RemoveAnimBlockRef);
        EntryPatch p9(std::bit_cast<uintptr_t>(&CStreaming::RequestModel),RequestModel), p9e(0x4087E0,RequestModel);
        EntryPatch p10(std::bit_cast<uintptr_t>(&CPed::Say),Say), p10e(0x5EFFE0,Say);
        EntryPatch p11(std::bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),BonePos), p11e(0x5E4280,BonePos);
        EntryPatch p11b(std::bit_cast<uintptr_t>(static_cast<CVector(CPed::*)(eBoneTag,bool)>(&CPed::GetBonePosition)),BonePosRet);
        EntryPatch p12(std::bit_cast<uintptr_t>(static_cast<int16(CPad::*)()const>(&CPad::GetPedWalkLeftRight)),PadX), p12e(0x53FC90,PadX);
        EntryPatch p13(std::bit_cast<uintptr_t>(static_cast<int16(CPad::*)()const>(&CPad::GetPedWalkUpDown)),PadY), p13e(0x53FD30,PadY);
        EntryPatch p14(std::bit_cast<uintptr_t>(&CPad::JumpJustDown),PadJump), p14e(0x540770,PadJump);
        EntryPatch p15(std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), p15e(0x4D68B0,GetAssoc);
        EntryPatch p16(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), p16e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{};
                auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                int subType=-1; int argType=0; int argInt=0; float argF[6]{}; CVector argV{};
                auto setup=[&]() {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xBADC0DEu; seed=i; rngCalls=0; trapped=false; trace=json::array();
                    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
                    while(auto* r=Veh()->m_pReferences)Veh()->CleanUpOldReference(r->m_ppEntity);
                    { auto* pl=FindPlayerPed(); for(auto* r=pl->m_pReferences;r;){ auto* nx=r->m_pNext; const auto a=uintptr_t(r->m_ppEntity);
                        if(a>=uintptr_t(object.data())&&a<uintptr_t(object.data())+object.size())pl->CleanUpOldReference(r->m_ppEntity); r=nx; } }
                    for(auto* reg:{&veh})std::memset(reg->data(),0,reg->size());
                    for(auto& p:peds)std::memset(p.data(),0,p.size()); for(auto& p:intel)std::memset(p.data(),0,p.size());
                    for(auto& m:mats)std::memset(m.data(),0,m.size()); for(auto& k:fakes)std::memset(k.data(),0,k.size());
                    for(auto& a:assocs)std::memset(a.data(),0,a.size());
                    std::memset(group.data(),0,group.size()); std::memset(accident.data(),0,accident.size()); std::memset(playerData.data(),0,playerData.size()); std::memset(object.data(),0xCD,object.size());
                    CTimer::m_snTimeInMilliseconds=500000+(i%9)*111; CTimer::ms_fTimeStep=uni(0.2f,2.f);
                    auto setMat=[&](CMatrixLink* m){ CMatrix r{}; r.SetRotate(uni(-.5f,.5f),uni(-.5f,.5f),uni(-3.f,3.f)); r.GetPosition()=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,5.f)}; std::memcpy(static_cast<CMatrix*>(m),&r,sizeof(CMatrix)); };
                    for(int n=0;n<PEDS;++n) {
                        auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED);
                        p->m_matrix=reinterpret_cast<CMatrixLink*>(mats[n].data()); setMat(p->m_matrix);
                        *reinterpret_cast<CPedIntelligence**>(peds[n].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intel[n].data());
                        p->m_nPedType=ePedType((n==0&&next()%3==0) ? PED_TYPE_PLAYER1 : next()%3==0 ? PED_TYPE_CRIMINAL : PED_TYPE_CIVMALE);
                        p->bInVehicle=next()%2; p->bIsStanding=next()%2; p->bIsInTheAir=next()%2; p->bFadeOut=next()%4==0;
                        p->m_nMoveState=eMoveState(next()%8); p->m_fCurrentRotation=uni(-3.f,3.f); p->m_fAimingRotation=uni(-3.f,3.f);
                        p->m_nDeathTimeMS=int32(next()%1000); p->m_vecMoveSpeed=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-.1f,.1f)};
                        *reinterpret_cast<void**>(peds[n].data()+0x480)=(n==0&&next()%3==0) ? playerData.data():nullptr; // m_pPlayerData
                        if(*reinterpret_cast<void**>(peds[n].data()+0x480))p->m_nPedType=PED_TYPE_PLAYER1; // datos de jugador => tipo jugador
                        p->m_pVehicle=(n==0||next()%3) ? Veh():nullptr; // el original desreferencia el vehiculo del ped 0 sin comprobar
                        auto& sc=reinterpret_cast<CPedIntelligence*>(intel[n].data())->GetStuckChecker();
                        sc.m_state=ePedStuckState(next()%3); sc.m_radius=uint16(next()%7);
                        reinterpret_cast<CPedIntelligence*>(intel[n].data())->GetTaskManager().m_pPed=p;
                    }
                    Veh()->SetType(ENTITY_TYPE_VEHICLE); Veh()->m_matrix=reinterpret_cast<CMatrixLink*>(mats[PEDS].data()); setMat(Veh()->m_matrix);
                    Veh()->m_pDriver=next()%2 ? Ped(1+next()%3):nullptr; Veh()->m_vecMoveSpeed=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f};
                    *reinterpret_cast<CPed**>(group.data()+0x28)=next()%4 ? Ped(2):nullptr; // lider del grupo
                    reinterpret_cast<CAccident*>(accident.data())->m_pPed=next()%5 ? Ped(1):nullptr;
                    for(int k=0;k<ASSOCS;++k){auto* a=reinterpret_cast<CAnimBlendAssociation*>(assocs[k].data());a->SetBlendAmount(uni(-0.5f,1.f));a->SetBlendDelta(uni(-1.f,1.f));}
                    assocMask=next()&0x3F; lookAtEntity=int(next()%5)-1; isLooking=next()%2; shouldLoadGang=next()%2;
                    padX=int16(next()%3==0 ? 0 : int(next()%256)-128); padY=int16(next()%3==0 ? 0 : int(next()%256)-128); padJump=next()%4==0;
                    for(int k=0;k<FAKES;++k){ *reinterpret_cast<void**>(fakes[k].data())=fakeVtbl.data(); fakeInfo[k].abortable=next()%4!=0; }
                    // Tarea bajo prueba (objeto en memoria propia con la vtable del EXE).
                    std::memset(object.data(),0,fn.size); *reinterpret_cast<uintptr_t*>(object.data())=fn.vtbl;
                    // Subtarea falsa (y sub-subtareas) segun la funcion.
                    static const std::array<int,6> stareSubs{TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_NAMED_ANIM,TASK_SIMPLE_PAUSE,TASK_COMPLEX_JUMP,TASK_NONE};
                    static const std::array<int,5> stealSubs{TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER,TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_PAUSE};
                    static const std::array<int,4> stuckSubs{TASK_COMPLEX_FLEE_POINT,TASK_COMPLEX_FALL_AND_GET_UP,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_JUMP};
                    static const std::array<int,5> trackSubs{TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_NONE};
                    static const std::array<int,4> treatSubs{TASK_SIMPLE_GIVE_CPR,TASK_NONE,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_STAND_STILL};
                    auto vt=fn.vtbl;
                    subType = vt==VT_STARE ? stareSubs[next()%6] : vt==VT_STEAL ? stealSubs[next()%5] : vt==VT_STUCK ? stuckSubs[next()%4] : vt==VT_TRACK ? trackSubs[next()%5] : treatSubs[next()%4];
                    fakeInfo[0].type=subType; for(int k=0;k<FAKES;++k)fakeInfo[k].simple=false;
                    for(int t:{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_NAMED_ANIM,TASK_SIMPLE_PAUSE,TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_GIVE_CPR,TASK_SIMPLE_ACHIEVE_HEADING,TASK_NONE})if(subType==t)fakeInfo[0].simple=true;
                    fakeInfo[1].simple=fakeInfo[2].simple=true;
                    fakeInfo[1].type=next()%2 ? TASK_SIMPLE_FALL:TASK_SIMPLE_IN_AIR; fakeInfo[2].type=next()%2 ? TASK_SIMPLE_IN_AIR:TASK_SIMPLE_STAND_STILL;
                    fakes[1][offsetof(CTaskSimpleFall,m_bIsFinished)]=uint8(next()%2);
                    if(next()%5)*reinterpret_cast<CTask**>(fakes[0].data()+8)=next()%4 ? Fake(1):nullptr;
                    next(); // (antes: subtarea de fake1; las simples no tienen)
                    *reinterpret_cast<CTask**>(object.data()+8)=(next()%8==0&&fn.slot!=10&&fn.slot!=8&&fn.slot!=6) ? nullptr : Fake(0); // Control/Next/MakeAbortable usan la subtarea sin comprobar
                    // Campos propios.
                    auto* o=object.data();
                    if(vt==VT_STARE){auto* t=reinterpret_cast<CTaskComplexStareAtPed*>(o);t->m_pPedGroup=next()%4 ? reinterpret_cast<CPedGroup*>(group.data()):nullptr;t->m_pPed=next()%5 ? (next()%3==0 ? static_cast<CPed*>(FindPlayerPed()) : Ped(1)):nullptr;
                        t->m_timeout=int32(next()%20000);t->m_timer.m_nStartTime=CTimer::m_snTimeInMilliseconds-next()%30000;t->m_timer.m_nInterval=int32(next()%20000);t->m_timer.m_bStarted=next()%2;
                        t->m_lookInitialised=next()%2;t->m_animsReferenced=next()%2;if(t->m_pPed)t->m_pPed->RegisterReference(reinterpret_cast<CEntity**>(&t->m_pPed));}
                    if(vt==VT_STEAL){auto* t=reinterpret_cast<CTaskComplexStealCar*>(o);t->m_veh=next()%5 ? Veh():nullptr;t->m_enterTimer.m_nStartTime=CTimer::m_snTimeInMilliseconds-next()%20000;
                        t->m_enterTimer.m_nInterval=10000;t->m_enterTimer.m_bStarted=next()%2;t->m_originalriver=next()%2 ? Ped(1+next()%3):nullptr;
                        if(t->m_veh)t->m_veh->RegisterReference(reinterpret_cast<CEntity**>(&t->m_veh));if(t->m_originalriver)t->m_originalriver->RegisterReference(reinterpret_cast<CEntity**>(&t->m_originalriver));}
                    if(vt==VT_TRACK){auto* t=reinterpret_cast<CTaskComplexTrackEntity*>(o);
                        CEntity* e=next()%6==0 ? nullptr : next()%2 ? static_cast<CEntity*>(Ped(3)) : static_cast<CEntity*>(Veh());
                        if(!e&&(std::string(fn.name)=="Clone"||std::string(fn.name)=="Destructor"))e=Veh(); // Clone/ctor registran la referencia sin comprobar nulo (precondicion)
                        t->m_toTrack=e;t->m_offsetPosn=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-1.f,1.f)};t->a=next()%2;t->b=int32(next()%5);t->m_rangeMin=uni(0.5f,8.f);t->m_rangeMax=t->m_rangeMin+uni(0.f,30.f);
                        t->f=next()%2;t->m_someStartTimeMs=CTimer::m_snTimeInMilliseconds-next()%5000;t->m_expirationInterval=next()%6000;t->gap2=next()%2;t->gap3=next()%2;
                        t->m_fMoveRatio=next()%4==0 ? -1.f : uni(0.f,3.f);t->float40=uni(0.f,3.f);t->m_goToPos=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,3.f)};
                        const float d=next()%3==0 ? uni(0.f,0.06f) : next()%2 ? uni(0.f,30.f) : uni(0.f,1.2f); t->m_distToTargetSq=d*d*(next()%2 ? 1.f:30.f);
                        if(e)e->RegisterReference(&t->m_toTrack);}
                    if(vt==VT_TREAT){reinterpret_cast<CTaskComplexTreatAccident*>(o)->m_pAccident=reinterpret_cast<CAccident*>(accident.data());}
                    argType=int(next()); argInt=int(next()%30000); for(auto& x:argF)x=uni(-5.f,40.f); argV=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-2.f,2.f)};
                };
                auto run=[&](bool candidate)->Snap {
                // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0; }
                    setup();
                    if(fn.vtbl==VT_STUCK)Write({{"event","side"},{"candidate",candidate},{"sub",subType}});
                    for(auto* h:hooks)h->State(candidate);
                    const auto occupancy=Occupancy();
                    const auto vtE=reinterpret_cast<uintptr_t*>(fn.vtbl);
                    const uintptr_t target=fn.slot>=0 ? vtE[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    void* self=object.data(); CPed* ped=Ped(0); json ret=nullptr;
                    const auto pickType=[&](std::initializer_list<int> ts){ return *(ts.begin()+(unsigned(argType)%ts.size())); };
                    const std::string name=fn.name;
                    if(name=="Constructor") {
                        std::memset(object.data(),0xCD,object.size());
                        if(fn.vtbl==VT_STARE) ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPedGroup*,CPed*,int32)>(target)(self,argInt%3 ? reinterpret_cast<CPedGroup*>(group.data()):nullptr,argInt%5 ? (argInt%2 ? static_cast<CPed*>(FindPlayerPed()) : Ped(1)):nullptr,argInt));
                        if(fn.vtbl==VT_STEAL) ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(self,argInt%5 ? Veh():nullptr));
                        if(fn.vtbl==VT_TRACK) ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,CVector,uint8,int32,float,float,uint8)>(target)(self,argInt%2 ? static_cast<CEntity*>(Ped(3)):static_cast<CEntity*>(Veh()),argV,uint8(argInt%2),argInt%7,argF[0],argF[1],uint8((argInt>>1)%2)));
                        if(fn.vtbl==VT_TREAT) ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CAccident*)>(target)(self,reinterpret_cast<CAccident*>(accident.data())));
                        ret={ret,reinterpret_cast<CTask*>(self)->GetTaskType()};
                    }
                    else if(name=="Destructor") reinterpret_cast<void*(__thiscall*)(void*)>(target)(self);
                    else if(name=="Clone"||name=="CreateFirstSubTask"||name=="CreateNextSubTask"||name=="ControlSubTask") {
                        CTask* r = name=="Clone" ? reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self) : reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,ped);
                        ret=Name(r); (void)r;
                    }
                    else if(name=="GetTaskType") ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(self);
                    else if(name=="MakeAbortable") ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(target)(self,ped,argInt%3,nullptr);
                    else if(name=="CreateSubTask") {
                        int t = fn.vtbl==VT_STEAL ? pickType({TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_SIMPLE_PAUSE,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER,TASK_FINISHED})
                              : fn.vtbl==VT_STUCK ? pickType({TASK_COMPLEX_FLEE_POINT,TASK_FINISHED,TASK_COMPLEX_JUMP,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_FALL_AND_GET_UP})
                              : pickType({TASK_SIMPLE_GIVE_CPR,TASK_NONE,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_STAND_STILL,TASK_FINISHED});
                        if(fn.vtbl==VT_TREAT&&t==TASK_SIMPLE_ACHIEVE_HEADING&&!reinterpret_cast<CAccident*>(accident.data())->m_pPed)t=TASK_NONE; // ComputeHeading exige victima
                        CTask* r=reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(target)(self,t,ped); ret={t,Name(r)};
                    }
                    else if(name=="ComputeHeading") { if(!reinterpret_cast<CAccident*>(accident.data())->m_pPed)reinterpret_cast<CAccident*>(accident.data())->m_pPed=Ped(1);
                        const float h=reinterpret_cast<float(__thiscall*)(void*,CPed*)>(target)(self,ped); ret=std::bit_cast<uint32>(h); }
                    else if(name=="SetOffsetPos") reinterpret_cast<void(__thiscall*)(void*,CVector)>(target)(self,argV);
                    else if(name=="CalcTargetPos") { if(!reinterpret_cast<CTaskComplexTrackEntity*>(self)->m_toTrack)reinterpret_cast<CTaskComplexTrackEntity*>(self)->m_toTrack=Veh(); reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(self,ped); }
                    else if(name=="CalcMoveRatio") reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(self,ped);
                    // Tareas nuevas en el pool -> ids, instantanea y liberacion.
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occupancy.size();++k)if(!occupancy[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    auto remap=[&](json& j){ if(j.is_number_unsigned()&&freshIds.contains(uintptr_t(j.get<uint64>())))j=Name(uintptr_t(j.get<uint64>())); };
                    remap(ret); if(ret.is_array()&&ret.size()==2)remap(ret[1]);
                    Snap s; s.ret=ret; s.trace=trace; s.rng=rngCalls;
                    for(auto& r:Regions()){const auto* w=reinterpret_cast<const uint32*>(r.data);for(size_t k=0;k<r.size/4;++k)s.words.push_back(Encode(w[k]));}
                    s.tasks=json::array();
                    for(auto* t:fresh){const int type=t->GetTaskType();const auto* w=reinterpret_cast<const uint32*>(t);json words=json::array();
                        for(size_t k=1;k<TaskSize(type)/4;++k){const uint32 v=w[k]&ConstructedMask(type,k*4);s.words.push_back(Encode(v));words.push_back(Name(v));} s.words.push_back(type); s.tasks.push_back({{"type",type},{"words",words}});}
                    if(name=="Constructor"){ size_t idx=0; for(auto& r:Regions()){ if(r.data==object.data())break; idx+=r.size/4; }
                        const int type=fn.vtbl==VT_STARE ? TASK_COMPLEX_STARE_AT_PED : fn.vtbl==VT_STEAL ? TASK_COMPLEX_STEAL_CAR : fn.vtbl==VT_TRACK ? TASK_COMPLEX_TRACK_ENTITY : TASK_COMPLEX_TREAT_ACCIDENT;
                        for(size_t k=0;k<object.size()/4;++k)s.words[idx+k]=Encode(reinterpret_cast<const uint32*>(object.data())[k]&ConstructedMask(type,k*4)); }
                    AppendRefs(s.words); s.words.push_back(switches); s.words.push_back(trapped);
                    // vptr del objeto: EXE o DLL segun quien construyo; se compara el tipo, no la direccion.
                    { auto idx=size_t(0); for(auto& r:Regions()){ if(r.data==object.data())break; idx+=r.size/4; } s.words[idx]=int64(3)<<40; }
                    // Liberar: raices = tareas nuevas no referenciadas por otras tareas nuevas.
                    std::vector<CTask*> roots;
                    for(auto* t:fresh){bool child=false;for(auto* o2:fresh)if(o2!=t){const auto* w=reinterpret_cast<const uint32*>(o2);for(size_t k=1;k<TaskSize(o2->GetTaskType())/4;++k)if(w[k]==uintptr_t(t))child=true;}if(!child)roots.push_back(t);}
                    // Una tarea nueva colocada en un slot secundario de una inteligencia fixture tambien es raiz.
                    for(auto* t:roots)delete t;
                    s.clean=Occupancy()==occupancy;
                    s.words.push_back(s.clean);
                    return s;
                };
                Write({{"event","begin"},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i}});
                const auto original=run(false); const auto candidate=run(true);
                const bool ok=original.words==candidate.words&&original.trace==candidate.trace&&original.rng==candidate.rng&&original.ret==candidate.ret&&original.clean;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original.ret},{"candidate",candidate.ret}});
                else {
                    std::vector<std::string> labels; for(auto& r:Regions())for(size_t k=0;k<r.size/4;++k)labels.push_back(std::string(r.name)+"+"+std::to_string(k*4));
                    json diff=json::array(); for(size_t k=0;k<std::min(original.words.size(),candidate.words.size())&&diff.size()<24;++k)if(original.words[k]!=candidate.words[k])diff.push_back({k<labels.size() ? json(labels[k]) : json(k),original.words[k],candidate.words[k]});
                    json rec={{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false}};
                    if(failures<=2){rec["original"]={{"ret",original.ret},{"trace",original.trace},{"tasks",original.tasks},{"rng",original.rng},{"clean",original.clean},{"sizes",original.words.size()}};
                                    rec["candidate"]={{"ret",candidate.ret},{"trace",candidate.trace},{"tasks",candidate.tasks},{"rng",candidate.rng},{"clean",candidate.clean},{"sizes",candidate.words.size()}};rec["word_diff"]=diff;}
                    Write(rec);
                }
            }
            Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
    while(auto* r=Veh()->m_pReferences)Veh()->CleanUpOldReference(r->m_ppEntity);
    for(auto* r=player->m_pReferences;r;){ auto* nx=r->m_pNext; const auto a=uintptr_t(r->m_ppEntity);
        if(a>=uintptr_t(object.data())&&a<uintptr_t(object.data())+object.size())player->CleanUpOldReference(r->m_ppEntity); r=nx; }
    CTimer::m_snTimeInMilliseconds=savedTime; CTimer::ms_fTimeStep=savedStep;
    const bool restored=ReversingProbe::Bytes(player,sizeof(CPlayerPed))==savedPlayer&&Occupancy()==occupancyStart&&freeRefs()==savedFree;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
