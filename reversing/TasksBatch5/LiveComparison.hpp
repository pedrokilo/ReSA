#pragma once
// Banco dentro del motor (lote 4 de tareas): CTaskComplexWaitAtAttractor (7), CTaskComplexWaitForBackup (8),
// CTaskComplexWaitForBus (7), CTaskComplexWaitForDryWeather (7) y CTaskComplexWalkAlongsidePed (9).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos y deterministas (xorshift):
// peatones con matriz, inteligencia, clump y datos de jugador falsos; vehiculo falso; subtarea falsa con vtable
// propia. Se simulan y trazan: RpAnimBlendClumpGetAssociation (AnimId), CPed::SetMoveAnimSpeed,
// CPedIntelligence::GetMoveStateFromGoToTask, CTaskManager::FindActiveTaskByType, CTaskSimpleGoToPoint::UpdatePoint,
// FindPlayerPed y el RNG. CTimer::m_snTimeInMilliseconds y CWeather::Rain se fijan por caso y se restauran.
// Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas creadas (tipo + palabras),
// las referencias, el retorno, la traza, las llamadas al RNG y que el pool de tareas quede limpio.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexWaitAtAttractor.h"
#include "TaskComplexWaitForBackup.h"
#include "TaskComplexWaitForBus.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskComplexWalkAlongsidePed.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimplePause.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleWaitForBus.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Weather.h"
#include "PlayerPedData.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Tasks5Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
constexpr int PEDS=3;
using SeekXY=CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>;
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,PEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,PEDS> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,PEDS> intel{};
alignas(16) inline std::array<std::array<uint8,0x40>,PEDS> clumps{};
alignas(16) inline std::array<std::array<uint8,0x40>,PEDS> anims{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPlayerPedData)>,PEDS> pdata{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x100> fake{}, fake2{}, gotoBuf{};
alignas(16) inline std::array<uint8,0x40> attractor{}, event{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh() { return reinterpret_cast<CVehicle*>(veh.data()); }
inline CAnimBlendAssociation* Anim(int n) { return reinterpret_cast<CAnimBlendAssociation*>(anims[n].data()); }
inline uintptr_t dllXYVtbl{}; constexpr uintptr_t EXE_XY_VTBL=0x86F8F8;
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    // vtable de CEntitySeekPosCalculatorXYOffset embebida en CTaskComplexSeekEntity: EXE o DLL segun quien construye
    if(v==EXE_XY_VTBL||(dllXYVtbl&&v==dllXYVtbl))return "vt:xyoffset";
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<PEDS;++n){ const auto k="#"+std::to_string(n);
        if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+k; if(auto s=in(mats[n].data(),mats[n].size(),"mat"))return *s+k;
        if(auto s=in(intel[n].data(),intel[n].size(),"intel"))return *s+k; if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+k;
        if(auto s=in(anims[n].data(),anims[n].size(),"anim"))return *s+k; if(auto s=in(pdata[n].data(),pdata[n].size(),"pdata"))return *s+k; }
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(fake.data(),fake.size(),"fake"))return *s; if(auto s=in(fake2.data(),fake2.size(),"fake2"))return *s;
    if(auto s=in(gotoBuf.data(),gotoBuf.size(),"goto"))return *s; if(auto s=in(attractor.data(),attractor.size(),"attractor"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntityHash(const std::array<uint8,N>& buf) { auto copy=buf; const auto* e=reinterpret_cast<const CEntity*>(buf.data()); const auto off=uintptr_t(&e->m_pReferences)-uintptr_t(e); std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size()); }
inline int IndexIn(const void* p,const auto& arr) { for(int n=0;n<PEDS;++n)if(uintptr_t(p)>=uintptr_t(arr[n].data())&&uintptr_t(p)<uintptr_t(arr[n].data())+arr[n].size())return n; return -1; }

// configuracion por caso de las dependencias simuladas
inline int fakeType{}; inline bool abortRet{};
inline std::array<uint8,PEDS> findMask{}, animOn{}; inline std::array<int,PEDS> moveState{}; inline std::array<float,PEDS> speedDelta{};
inline int playerIdx{}; inline int seed{}, rngCalls{};

// subtarea falsa
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
// GetTaskType no tiene efectos: las llamadas consecutivas repetidas (p. ej. el assert de notsa::cast en depuracion) se colapsan
inline int __fastcall FType(CTask* t,void*) { json e={"get_task_type",Name(uintptr_t(t))}; if(trace.empty()||trace.back()!=e)trace.push_back(e); return fakeType; }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return abortRet; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// tareas devueltas por FindActiveTaskByType (seguir ruta = fake2, ir a punto = goto)
inline int __fastcall OType(CTask* t,void*) { trace.push_back({"found_task_get_type",Name(uintptr_t(t))}); return uintptr_t(t)==uintptr_t(gotoBuf.data()) ? TASK_SIMPLE_GO_TO_POINT : TASK_COMPLEX_FOLLOW_NODE_ROUTE; }
template<int N> void* __fastcall OTrap(CTask*,void*) { trace.push_back({"found_task_unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> otherVtbl{};
template<int... N> void FillOTraps(std::integer_sequence<int,N...>) { ((otherVtbl[N]=reinterpret_cast<void*>(&OTrap<N>)),...); }

// dependencias
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CTask* __fastcall FindActive(void* tm,void*,int type) {
    const int n=IndexIn(tm,intel); trace.push_back({"find_active_task",Name(uintptr_t(tm)),type}); if(n<0)return nullptr;
    if(type==TASK_COMPLEX_FOLLOW_NODE_ROUTE&&(findMask[n]&1))return reinterpret_cast<CTask*>(fake2.data());
    if(type==TASK_SIMPLE_GO_TO_POINT&&(findMask[n]&2))return reinterpret_cast<CTask*>(gotoBuf.data());
    return nullptr;
}
inline eMoveState __fastcall MoveState(void* in,void*) { const int n=IndexIn(in,intel); trace.push_back({"get_move_state_from_go_to_task",Name(uintptr_t(in))}); return eMoveState(n<0 ? 0 : moveState[n]); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { const int n=IndexIn(c,clumps); trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return n>=0&&animOn[n] ? Anim(n) : nullptr; }
inline void __fastcall SetMoveSpeed(CPed* p,void*,CAnimBlendAssociation* a) {
    const int n=IndexIn(p,peds); const float before=a ? a->GetSpeed() : 0.f;
    if(a&&n>=0)a->SetSpeed(before+speedDelta[n]);
    trace.push_back({"set_move_anim_speed",Name(uintptr_t(p)),Name(uintptr_t(a)),F(before),a ? F(a->GetSpeed()) : 0u});
}
inline void __fastcall UpdatePoint(void* t,void*,const CVector& v,float r,bool b) { trace.push_back({"update_point",Name(uintptr_t(t)),F(v.x),F(v.y),F(v.z),F(r),b}); }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(Ped(playerIdx)); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// tamano comparable de las tareas creadas (el resto del hueco del pool es basura del caso anterior)
inline size_t TaskSize(int type) {
    switch(type){
    case TASK_SIMPLE_STAND_STILL: return sizeof(CTaskSimpleStandStill);
    case TASK_COMPLEX_LEAVE_CAR: return sizeof(CTaskComplexLeaveCar);
    case TASK_COMPLEX_SEEK_ENTITY: return sizeof(SeekXY);
    case TASK_SIMPLE_GUN_CTRL: return sizeof(CTaskSimpleGunControl);
    case TASK_SIMPLE_ACHIEVE_HEADING: return sizeof(CTaskSimpleAchieveHeading);
    case TASK_SIMPLE_PAUSE: return sizeof(CTaskSimplePause);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: return sizeof(CTaskComplexEnterCarAsPassenger);
    case TASK_SIMPLE_WAIT_FOR_BUS: return sizeof(CTaskSimpleWaitForBus);
    case TASK_COMPLEX_WAIT_AT_ATTRACTOR: return sizeof(CTaskComplexWaitAtAttractor);
    case TASK_COMPLEX_WAIT_FOR_BACKUP: return sizeof(CTaskComplexWaitForBackup);
    case TASK_COMPLEX_WAIT_FOR_BUS: return sizeof(CTaskComplexWaitForBus);
    case TASK_COMPLEX_WAIT_FOR_DRY_WEATHER: return sizeof(CTaskComplexWaitForDryWeather);
    case TASK_COMPLEX_WALK_ALONGSIDE_PED: return sizeof(CTaskComplexWalkAlongsidePed);
    default: return 0x38;
    }
}
enum Kind { ATT, BAK, BUS, DRY, WLK };
struct Fn { const char* cls; const char* name; uintptr_t address; int slot; int cases; Kind kind; };
constexpr uintptr_t VT[]{0x86E108,0x870DA4,0x86E18C,0x86E160,0x8706CC};
constexpr size_t SZ[]{sizeof(CTaskComplexWaitAtAttractor),sizeof(CTaskComplexWaitForBackup),sizeof(CTaskComplexWaitForBus),sizeof(CTaskComplexWaitForDryWeather),sizeof(CTaskComplexWalkAlongsidePed)};
static_assert(sizeof(CTaskComplexWaitAtAttractor)==0x14&&sizeof(CTaskComplexWaitForBackup)==0x24&&sizeof(CTaskComplexWaitForBus)==0x10);
static_assert(sizeof(CTaskComplexWaitForDryWeather)==0xC&&sizeof(CTaskComplexWalkAlongsidePed)==0x20);
inline const std::array<Fn,38> fns{{
    {"CTaskComplexWaitAtAttractor","Constructor",0x6325F0,-1,64,ATT},{"CTaskComplexWaitAtAttractor","Destructor",0x632630,-1,64,ATT},
    {"CTaskComplexWaitAtAttractor","Clone",0,1,64,ATT},{"CTaskComplexWaitAtAttractor","GetTaskType",0,4,32,ATT},
    {"CTaskComplexWaitAtAttractor","CreateNextSubTask",0,8,32,ATT},{"CTaskComplexWaitAtAttractor","CreateFirstSubTask",0,9,64,ATT},
    {"CTaskComplexWaitAtAttractor","ControlSubTask",0,10,32,ATT},
    {"CTaskComplexWaitForBackup","Constructor",0x697290,-1,128,BAK},{"CTaskComplexWaitForBackup","Destructor",0x697330,-1,128,BAK},
    {"CTaskComplexWaitForBackup","CreateSubTask",0x6973B0,-1,2048,BAK},
    {"CTaskComplexWaitForBackup","Clone",0,1,128,BAK},{"CTaskComplexWaitForBackup","GetTaskType",0,4,32,BAK},
    {"CTaskComplexWaitForBackup","CreateNextSubTask",0,8,2048,BAK},{"CTaskComplexWaitForBackup","CreateFirstSubTask",0,9,512,BAK},
    {"CTaskComplexWaitForBackup","ControlSubTask",0,10,32,BAK},
    {"CTaskComplexWaitForBus","Constructor",0x632900,-1,64,BUS},{"CTaskComplexWaitForBus","Destructor",0x632930,-1,64,BUS},
    {"CTaskComplexWaitForBus","Clone",0x636A60,1,64,BUS},{"CTaskComplexWaitForBus","GetTaskType",0x632920,4,32,BUS},
    {"CTaskComplexWaitForBus","CreateNextSubTask",0x632940,8,1024,BUS},{"CTaskComplexWaitForBus","CreateFirstSubTask",0x638940,9,64,BUS},
    {"CTaskComplexWaitForBus","ControlSubTask",0x632A40,10,32,BUS},
    {"CTaskComplexWaitForDryWeather","Constructor",0x632800,-1,32,DRY},{"CTaskComplexWaitForDryWeather","Destructor",0x632830,-1,64,DRY},
    {"CTaskComplexWaitForDryWeather","Clone",0,1,64,DRY},{"CTaskComplexWaitForDryWeather","GetTaskType",0,4,32,DRY},
    {"CTaskComplexWaitForDryWeather","CreateNextSubTask",0,8,256,DRY},{"CTaskComplexWaitForDryWeather","CreateFirstSubTask",0,9,64,DRY},
    {"CTaskComplexWaitForDryWeather","ControlSubTask",0,10,32,DRY},
    {"CTaskComplexWalkAlongsidePed","Constructor",0x683240,-1,128,WLK},{"CTaskComplexWalkAlongsidePed","Destructor",0x6832C0,-1,128,WLK},
    {"CTaskComplexWalkAlongsidePed","CreateSubTask",0x685000,-1,1024,WLK},
    {"CTaskComplexWalkAlongsidePed","Clone",0,1,128,WLK},{"CTaskComplexWalkAlongsidePed","GetTaskType",0,4,32,WLK},
    {"CTaskComplexWalkAlongsidePed","MakeAbortable",0,6,256,WLK},{"CTaskComplexWalkAlongsidePed","CreateNextSubTask",0,8,512,WLK},
    {"CTaskComplexWalkAlongsidePed","CreateFirstSubTask",0,9,512,WLK},{"CTaskComplexWalkAlongsidePed","ControlSubTask",0,10,2048,WLK},
}};

inline void CleanRefs() {
    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
    while(auto* r=Veh()->m_pReferences)Veh()->CleanUpOldReference(r->m_ppEntity);
}
inline void SetMat(int n,const CVector& right,const CVector& fwd,const CVector& up,const CVector& pos) {
    CMatrix m{}; m.GetRight()=right; m.GetForward()=fwd; m.GetUp()=up; m.GetPosition()=pos;
    std::memcpy(static_cast<CMatrix*>(Ped(n)->m_matrix),&m,sizeof(CMatrix));
}
inline void SetMatRot(int n,float heading,const CVector& pos) {
    CMatrix m{}; m.SetRotate(0.f,0.f,heading); m.GetPosition()=pos; std::memcpy(static_cast<CMatrix*>(Ped(n)->m_matrix),&m,sizeof(CMatrix));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"tasks5-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"tasks5-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("tasks5");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    // "Clase::Funcion" o "Clase" en tasks5-only.txt limita el banco (iteracion rapida)
    std::string only; { std::ifstream f(dir/"tasks5-only.txt"); if(f)std::getline(f,only); while(!only.empty()&&(only.back()=='\r'||only.back()==' '))only.pop_back(); }
    std::array<ReversibleHooks::ReversibleHook::Base*,38> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillOTraps(std::make_integer_sequence<int,12>{}); otherVtbl[4]=reinterpret_cast<void*>(&OType);
    { CEntitySeekPosCalculatorXYOffset tmp{}; dllXYVtbl=*reinterpret_cast<const uintptr_t*>(&tmp); }
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    const float savedRain=CWeather::Rain;
    std::array<bool,38> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),FindActive), d1e(0x681740,FindActive);
        EntryPatch d2(bit_cast<uintptr_t>(&CPedIntelligence::GetMoveStateFromGoToTask),MoveState), d2e(0x601D70,MoveState);
        EntryPatch d3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), d3e(0x4D68B0,GetAssoc);
        EntryPatch d4(bit_cast<uintptr_t>(&CPed::SetMoveAnimSpeed),SetMoveSpeed), d4e(0x5DEC10,SetMoveSpeed);
        EntryPatch d5(bit_cast<uintptr_t>(&CTaskSimpleGoToPoint::UpdatePoint),UpdatePoint), d5e(0x645700,UpdatePoint);
        EntryPatch d6(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), d6e(0x56E210,FindPlayer);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
            if(!only.empty()&&only!=full&&only!=fn.cls)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto dy=[&](int range,float step){return float(int(next()%uint32(2*range+1))-range)*step;}; // diadico exacto
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* st=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(st[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5555u; if(!st)st=1; trace=json::array(); seed=i; rngCalls=0;
                    CleanRefs();
                    const std::string nm=fn.name;
                    // peatones: siempre con matriz, inteligencia, clump (tipo rpCLUMP para el assert de GetRpClump) y datos de jugador
                    for(int n=0;n<PEDS;++n){ std::memset(peds[n].data(),0,peds[n].size()); std::memset(intel[n].data(),0,intel[n].size()); std::memset(mats[n].data(),0,mats[n].size());
                        auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED); p->m_matrix=reinterpret_cast<CMatrixLink*>(mats[n].data());
                        SetMatRot(n,uni(-3.f,3.f),CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(0.f,2.f)});
                        std::memset(clumps[n].data(),0,clumps[n].size()); clumps[n][0]=2; // rpCLUMP
                        *reinterpret_cast<void**>(peds[n].data()+0x18)=clumps[n].data(); // CEntity::m_pRwObject (privado)
                        *reinterpret_cast<void**>(peds[n].data()+0x47C)=intel[n].data(); // m_pIntelligence (protegido)
                        for(auto& b:pdata[n])b=uint8(next()); reinterpret_cast<CPlayerPedData*>(pdata[n].data())->m_nBusFaresCollected=next()%1000;
                        *reinterpret_cast<void**>(peds[n].data()+0x480)=pdata[n].data(); // m_pPlayerData (protegido)
                        reinterpret_cast<CPedIntelligence*>(intel[n].data())->m_AnotherStaticCounter=int32(next()%17);
                        p->m_nPedState=next()%4==0 ? (next()%2 ? PEDSTATE_DIE : PEDSTATE_DEAD) : ePedState(next()%PEDSTATE_DIE);
                        p->m_fAimingRotation=uni(-3.f,3.f);
                        p->bInVehicle=next()%3==0; p->m_pVehicle=next()%2 ? Veh() : nullptr;
                        p->bDontDragMeOutCar=next()%2; p->m_bRemoveFromWorld=next()%2; p->bMoveAnimSpeedHasBeenSetByTask=next()%2;
                        { static const eWeaponType types[]{WEAPON_UNARMED,WEAPON_BRASSKNUCKLE,WEAPON_KNIFE,WEAPON_GRENADE,WEAPON_PISTOL,WEAPON_DESERT_EAGLE,WEAPON_SHOTGUN,
                              WEAPON_MICRO_UZI,WEAPON_MP5,WEAPON_AK47,WEAPON_M4,WEAPON_COUNTRYRIFLE,WEAPON_SNIPERRIFLE,WEAPON_RLAUNCHER,WEAPON_FLAMETHROWER,WEAPON_MINIGUN,
                              WEAPON_SPRAYCAN,WEAPON_EXTINGUISHER,WEAPON_CAMERA,WEAPON_PARACHUTE};
                          p->m_nActiveWeaponSlot=uint8(next()%13); p->m_aWeapons[p->m_nActiveWeaponSlot].m_Type=types[next()%std::size(types)]; }
                        std::memset(anims[n].data(),0,anims[n].size());
                        { static const float speeds[]{0.85f,1.f,1.25f,0.9f,1.2f,0.84f,1.26f,1.1f}; const uint32 m=next()%9; Anim(n)->SetSpeed(m<8 ? speeds[m] : uni(0.7f,1.4f)); }
                        { static const float deltas[]{0.f,0.005f,-0.005f,0.0125f,-0.0125f,0.013f,-0.013f,0.0129f,0.02f,-0.3f,0.3f}; speedDelta[n]=deltas[next()%std::size(deltas)]; }
                        animOn[n]=uint8(next()%5!=0); findMask[n]=uint8(next()%2 ? 2 : next()%4);
                        moveState[n]=next()%5<3 ? (next()%3==0 ? PEDMOVE_WALK : next()%2 ? PEDMOVE_RUN : PEDMOVE_SPRINT) : int(next()%8); }
                    // vehiculo
                    std::memset(veh.data(),0,veh.size()); Veh()->SetType(ENTITY_TYPE_VEHICLE);
                    { auto pick=[&]()->CPed*{ const uint32 m=next()%5; return m<PEDS ? Ped(m) : nullptr; };
                      Veh()->m_pDriver=pick(); for(auto& ps:Veh()->m_apPassengers)ps=next()%3==0 ? pick() : nullptr;
                      Veh()->m_nMaxPassengers=uint8(next()%9); Veh()->m_nNumPassengers=uint8(next()%9); }
                    playerIdx=int(next()%PEDS);
                    timeMs=100000u+next()%100000u;
                    fakeType=TASK_SIMPLE_STAND_STILL; abortRet=next()%3!=0;
                    std::memset(fake.data(),0,fake.size()); *reinterpret_cast<void**>(fake.data())=fakeVtbl.data();
                    std::memset(fake2.data(),0,fake2.size()); *reinterpret_cast<void**>(fake2.data())=otherVtbl.data();
                    std::memset(gotoBuf.data(),0,gotoBuf.size()); *reinterpret_cast<void**>(gotoBuf.data())=otherVtbl.data();
                    for(auto& b:attractor)b=uint8(next()); std::memset(event.data(),0,event.size());
                    // tarea probada
                    std::memset(object.data(),0xCD,object.size()); std::memset(object.data(),0,SZ[fn.kind]);
                    *reinterpret_cast<uintptr_t*>(object.data())=VT[fn.kind]; *reinterpret_cast<void**>(object.data()+8)=fake.data();
                    auto* o=object.data(); auto W=[&](size_t off)->uint32&{return *reinterpret_cast<uint32*>(o+off);};
                    auto Wf=[&](size_t off)->float&{return *reinterpret_cast<float*>(o+off);};
                    auto Reg=[&](size_t off,CPed* p){ W(off)=uint32(uintptr_t(p)); if(p)p->RegisterReference(reinterpret_cast<CEntity**>(o+off)); };
                    int subType=TASK_FINISHED; CPed* ctorA{}; CPed* ctorB{}; float ctorF{}; int32 ctorSlot{};
                    switch(fn.kind){
                    case ATT: W(0xC)=uint32(uintptr_t(attractor.data())); W(0x10)=next()%12; fakeType=TASK_SIMPLE_STAND_STILL;
                        ctorSlot=int32(next()%12)-1; break;
                    case BAK: {
                        // 0xC distancia, 0x10 agresor, 0x14 refuerzo, 0x18 CTaskTimer
                        const bool needAggressor=nm=="CreateNextSubTask"||nm=="CreateFirstSubTask";
                        static const int subs[]{TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_ACHIEVE_HEADING,TASK_SIMPLE_PAUSE,TASK_COMPLEX_LEAVE_CAR,TASK_FINISHED,TASK_SIMPLE_STAND_STILL};
                        subType=subs[next()%std::size(subs)];
                        { const uint32 m=next()%3; CPed* a=m==0 ? nullptr : Ped(m); if(!a&&(needAggressor||subType==TASK_SIMPLE_ACHIEVE_HEADING))a=Ped(1); Reg(0x10,a); }
                        { const uint32 m=next()%4; Reg(0x14,m==0 ? nullptr : Ped(m==3 ? 1 : 2)); }
                        Wf(0xC)=next()%32==0 ? std::numeric_limits<float>::quiet_NaN() : uni(0.f,6.f);
                        if(next()%3==0){ // distancia exactamente igual a la de espera (dyadica) y algunas variantes
                            const float w=float(1+next()%12)*0.25f; Wf(0xC)=w; const CVector pp{dy(12,0.25f),dy(12,0.25f),dy(4,0.25f)};
                            static const float eps[]{0.f,0.f,1.f/128,-1.f/128}; SetMatRot(0,0.f,pp);
                            if(W(0x14)){ const int b=W(0x14)==uint32(uintptr_t(Ped(1))) ? 1 : 2; const bool alongX=next()%2;
                                const float d=w+eps[next()%4]; SetMatRot(b,0.f,pp+(alongX ? CVector{d,0.f,dy(4,0.5f)} : CVector{0.f,-d,0.f})); } }
                        W(0x18)=timeMs-next()%30000u; { const uint32 m=next()%4; W(0x1C)=m==0 ? timeMs-W(0x18) : m==1 ? timeMs-W(0x18)+1 : m==2 ? timeMs-W(0x18)-1 : next()%30000u; }
                        o[0x20]=uint8(next()%4!=0); o[0x21]=uint8(next()%4==0);
                        if(subType==TASK_COMPLEX_LEAVE_CAR&&nm=="CreateSubTask")Ped(0)->m_pVehicle=Veh();
                        { static const int types[]{TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_ACHIEVE_HEADING,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_PAUSE,TASK_SIMPLE_STAND_STILL}; fakeType=types[next()%std::size(types)]; }
                        { const uint32 m=next()%3; ctorA=m==0 ? nullptr : Ped(m); const uint32 k=next()%3; ctorB=k==0 ? nullptr : Ped(k); ctorF=uni(0.f,20.f); }
                        break; }
                    case BUS: {
                        W(0xC)=next()%3 ? uint32(uintptr_t(veh.data())) : 0;
                        static const int types[]{TASK_SIMPLE_WAIT_FOR_BUS,TASK_SIMPLE_WAIT_FOR_BUS,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_SIMPLE_STAND_STILL};
                        fakeType=types[next()%std::size(types)];
                        // CTaskSimpleWaitForBus::m_bus de la subtarea falsa (se copia a m_bus y se pasa a EnterCarAsPassenger)
                        reinterpret_cast<CTaskSimpleWaitForBus*>(fake.data())->m_bus=Veh();
                        if(fakeType==TASK_COMPLEX_ENTER_CAR_AS_PASSENGER)W(0xC)=uint32(uintptr_t(veh.data())); // el original no comprueba nulo
                        break; }
                    case DRY: {
                        static const float rains[]{0.f,0.1f,0.2f,0.2f,std::nextafter(0.2f,0.f),std::nextafter(0.2f,1.f),0.19999999f,0.2000001f,float(0.2),0.5f,1.f};
                        const uint32 m=next()%(std::size(rains)+2); CWeather::Rain=m<std::size(rains) ? rains[m] : uni(0.f,1.f);
                        break; }
                    case WLK: {
                        // 0xC objetivo (CPed::Ref), 0x10 distancia para rendirse, 0x14 desplazamiento
                        auto* ped=Ped(0);
                        const bool sub=nm=="CreateSubTask";
                        static const int subs[]{TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SEEK_ENTITY,TASK_FINISHED,TASK_SIMPLE_GO_TO_POINT};
                        subType=subs[next()%std::size(subs)];
                        int tgt=next()%8==0 ? -1 : int(1+next()%2);
                        if(tgt<0&&(nm=="Clone"||(sub&&subType==TASK_COMPLEX_SEEK_ENTITY)))tgt=1;
                        Reg(0xC,tgt<0 ? nullptr : Ped(tgt));
                        Wf(0x10)=uni(0.5f,8.f); *reinterpret_cast<CVector*>(o+0x14)=CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-0.5f,0.5f)};
                        if(sub&&subType==TASK_COMPLEX_LEAVE_CAR)ped->m_pVehicle=Veh();
                        { static const int types[]{TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_SEEK_ENTITY,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_GO_TO_POINT};
                          fakeType=types[next()%std::size(types)]; }
                        // subtarea falsa con aspecto de SeekEntity<XYOffset>: vtable de calculadora y desplazamiento previos
                        *reinterpret_cast<uintptr_t*>(fake.data()+0x40)=EXE_XY_VTBL; *reinterpret_cast<CVector*>(fake.data()+0x44)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f};
                        *reinterpret_cast<float*>(fake.data()+0x20)=uni(0.f,5.f);
                        if(nm=="ControlSubTask"&&tgt>0){
                            const uint32 mode=next()%8; CVector& off=*reinterpret_cast<CVector*>(o+0x14);
                            if(mode==0&&next()%32==0)Wf(0x10)=std::numeric_limits<float>::quiet_NaN();
                            if(mode>=4){ // geometria exacta: matriz del objetivo alineada con los ejes, posiciones y desplazamiento diadicos
                                static const float ax[4][4]{{1,0,0,1},{0,1,-1,0},{-1,0,0,-1},{0,-1,1,0}}; const auto& a=ax[next()%4];
                                const CVector R{a[0],a[1],0.f}, Fw{a[2],a[3],0.f}, U{0.f,0.f,1.f};
                                const CVector tpos{dy(12,0.25f),dy(12,0.25f),float(next()%8)*0.25f};
                                off=CVector{dy(16,0.125f),dy(16,0.125f),next()%2 ? 0.f : dy(4,0.125f)};
                                SetMat(tgt,R,Fw,U,tpos); Wf(0x10)=20.f;
                                const CVector pt=tpos+R*off.x+Fw*off.y+U*off.z;
                                CVector pp{};
                                if(mode<=5){ // distancia al punto: 0.0625 exacta y vecinos, 2 exacta, delante del punto
                                    static const float de[][2]{{0.25f,0.f},{0.25f,0.f},{0.f,0.25f},{0.25f+1.f/256,0.f},{0.25f-1.f/256,0.f},{1.f,1.f},{1.f,-1.f},{0.5f,0.5f},{1.5f,0.f},{0.125f,0.f},{-0.25f,0.f},{0.f,0.f}};
                                    const auto& d=de[next()%std::size(de)]; pp=pt-Fw*d[0]+R*d[1]; pp.z=tpos.z; }
                                else if(mode==6){ // distancia objetivo-peaton exactamente 2 (dist^2 == 4)
                                    static const CVector ds[]{{2.f,0.f,0.f},{-2.f,0.f,0.f},{0.f,2.f,0.f},{0.f,-2.f,0.f},{1.f,1.f,0.f}}; pp=tpos+ds[next()%std::size(ds)]; }
                                else { // dist^2 == giveUp^2 exacto y justo por encima
                                    const float g=float(1+next()%8)*0.5f; Wf(0x10)=g; pp=tpos+R*(g+(next()%3==0 ? 1.f/64 : 0.f)); }
                                SetMat(0,CVector{1.f,0.f,0.f},CVector{0.f,1.f,0.f},U,pp); }
                        }
                        ctorA=Ped(1+next()%2); ctorF=next()%16==0 ? 0.f : uni(0.f,20.f);
                        break; }
                    }
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(VT[fn.kind]); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    void* self=o; CPed* ped=Ped(0); uintptr_t rawRet=0; json ret=nullptr;
                    if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                        switch(fn.kind){
                        case ATT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,int32)>(target)(self,attractor.data(),ctorSlot)); break;
                        case BAK: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,CPed*,float)>(target)(self,ctorA,ctorB,ctorF)); break;
                        case WLK: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,float)>(target)(self,ctorA,ctorF)); break;
                        default: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(self)); break;
                        } }
                    else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(target)(self);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(self);
                    else if(nm=="MakeAbortable"){ const int prio=int(next()%3); const void* ev=next()%2 ? event.data() : nullptr; ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(self,ped,prio,ev); }
                    else if(nm=="CreateSubTask")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*)>(target)(self,subType,ped));
                    else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,ped));
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&nm!="Destructor")ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh){ const int tt=t->GetTaskType(); tasks.push_back({{"type",tt},{"words",Region(t,std::min<size_t>(TaskSize(tt),0x80),true)}}); }
                    json objectWords=Region(o,SZ[fn.kind],true);
                    if(nm=="Constructor")objectWords.push_back(reinterpret_cast<CTask*>(self)->GetTaskType()); // vptr EXE o DLL: se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int n=0;n<PEDS;++n)refs.push_back(refList(Ped(n))); refs.push_back(refList(Veh()));
                    json pedWords=json::array(); for(int n=0;n<PEDS;++n){ auto* p=Ped(n);
                        pedWords.push_back({EntityHash(peds[n]),F(p->m_fAimingRotation),bool(p->bMoveAnimSpeedHasBeenSetByTask),bool(p->m_bRemoveFromWorld),bool(p->bDontDragMeOutCar),Hash(mats[n].data(),mats[n].size())}); }
                    json animWords=json::array(); for(int n=0;n<PEDS;++n)animWords.push_back(Region(anims[n].data(),sizeof(CAnimBlendAssociation)));
                    json pdataBytes=json::array(); for(int n=0;n<PEDS;++n)pdataBytes.push_back(ReversingProbe::Bytes(pdata[n].data(),pdata[n].size()));
                    json intelHash=json::array(); for(int n=0;n<PEDS;++n)intelHash.push_back(Hash(intel[n].data(),intel[n].size()));
                    const auto vehHash=EntityHash(veh);
                    for(auto* t:fresh)delete t;
                    const bool clean=Occupancy()==occ;
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"fake",Region(fake.data(),0x60,true)},{"refs",refs},{"peds",pedWords},{"anims",animWords},
                            {"pdata",pdataBytes},{"intel",intelHash},{"veh",vehHash},{"veh_after",EntityHash(veh)},{"time",timeMs},{"rain",F(CWeather::Rain)},
                            {"trace",trace},{"rng",rngCalls},{"clean",clean},{"switches",switches}};
                    CleanRefs(); timeMs=savedTime; CWeather::Rain=savedRain;
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
    CleanRefs(); timeMs=savedTime; CWeather::Rain=savedRain;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime}});
}
}
