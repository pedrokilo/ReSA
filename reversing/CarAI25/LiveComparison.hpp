#pragma once
// Banco dentro del motor: CCarAI (lista de 25, 25 funciones). Original (hooks de CCarAI desactivados, código del EXE)
// frente a candidato con fixtures deterministas sobre objetos FALSOS con desplazamientos crudos del EXE: vehículos
// (piloto automático, matrices, banderas, pasajeros) dentro de un pool de vehículos falso (se cambia 0xB74494),
// peatones/jugador con inteligencia, datos de jugador y CWanted falsos, model infos falsos (se cambian los punteros de
// ms_modelInfoPtrs usados) y un pool de peatones falso (0xB74490) para la cuenta de bandas de UpdateCarAI. Nunca se usa
// un vehículo ni un peatón real. Se simulan y trazan (dirección del EXE + símbolo/cuerpo de la DLL): FindPlayer*,
// rand/CGeneral::GetRandomNumber, CCarCtrl (JoinCarWithRoadSystem*, SwitchVehicleToRealPhysics,
// FindSpeedMultiplierWithSpeedFromNodes, FindMaxSteerAngle), CTrafficLights::ShouldCarStopForBridge, CCullZones
// (NoPolice, PoliceAbandonCars), CVehicle (UsesSiren, IsUpsideDown, GetVehicleAppearance, SetUpDriver,
// SetupPassenger), CPhysical::GetHasCollidedWith, CPed (GiveDelayedWeapon, AttachPedToEntity, SetCurrentWeapon),
// CPedIntelligence (ClearTasks, FindTaskByType, GetTaskSwim), CTaskManager::SetTask (EXE) / SetTask y
// ChangeTaskInSlot (DLL), CTaskSimpleCarSetPedOut::ProcessPed, CEventGroup::Add, CEntity::RegisterReference/
// CleanUpOldReference y la vtable del vehículo (PlayCarHorn). Los stubs no reservan memoria: guardan valores crudos en
// un array fijo y el json se construye después de la llamada. Las tareas nuevas se crean en el pool real de tareas
// (huecos libres a 0xCD y m_LastFreeSlot restaurado en cada ejecución), se copian al pasar por SetTask/Add y se borran
// después de la comparación. Reloj, contador de fotogramas, paso, punteros de pool, zona de bandas, bUseCarAI y
// punteros de model info se guardan y restauran en cada ejecución y al final.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "CarAI.h"
#include "CarCtrl.h"
#include "CullZones.h"
#include "TrafficLights.h"
#include "VehicleRecording.h"
#include "PopCycle.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleCarSetPedOut.h"
#include "EventGroup.h"
#include "EventScriptCommand.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CarAI25Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }

// acceso a CTaskManager::ChangeTaskInSlot (privada): la instanciación explícita ignora el control de acceso
template<class Tag,typename Tag::type M> struct Rob { friend typename Tag::type Get(Tag) { return M; } };
struct ChangeTaskTag { using type=void(CTaskManager::*)(CTask*&,CTask*); friend type Get(ChangeTaskTag); };
template struct Rob<ChangeTaskTag,&CTaskManager::ChangeTaskInSlot>;

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- módulo de la DLL ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; uintptr_t p=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

// ======================= objetos falsos =======================
constexpr size_t VSTRIDE=0xA18, NPOOL=10, MATB=0x60, NMAT=NPOOL+4, PEDB=0x7C4, NPED=6, INTELB=0x300, NPEDPOOL=8, TSLOT=0x80, NFAKET=8, NSLOT=80;
alignas(16) inline std::array<uint8,VSTRIDE*NPOOL> vehStore{};
alignas(16) inline std::array<uint8,NPOOL> vehStates{};
alignas(16) inline std::array<uint8,0x20> vehPoolHdr{};
alignas(16) inline std::array<std::array<uint8,MATB>,NMAT> mats{};
alignas(16) inline std::array<std::array<uint8,PEDB>,NPED> peds{};      // 0 jugador, 1 conductor, 2-4 pasajeros, 5 otro
alignas(16) inline std::array<std::array<uint8,INTELB>,NPED> intels{};
alignas(16) inline std::array<uint8,0x40> playerData{};
alignas(16) inline std::array<uint8,0x2A0> wanted{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<std::array<uint8,0x80>,4> mis{};
alignas(16) inline std::array<uint8,PEDB*NPEDPOOL> pedPoolStore{};
alignas(16) inline std::array<uint8,NPEDPOOL> pedPoolStates{};
alignas(16) inline std::array<uint8,0x20> pedPoolHdr{};
alignas(16) inline std::array<std::array<uint8,TSLOT>,NFAKET> fakeTasks{};
alignas(16) inline std::array<uint8,0x10> coorsArg{};
inline std::array<void*,NSLOT> vehVt{}, pedVt{}; inline std::array<void*,16> taskVt{};
inline const int16 MODELS[]{400,427,430,432,433,490,523,596,597,598,599};
inline uint8* Veh(size_t k){ return vehStore.data()+k*VSTRIDE; }
inline uint8* Ped(size_t k){ return peds[k].data(); }

// --- nombres de punteros (solo fuera de las llamadas probadas) ---
inline uintptr_t taskStor{}; inline int32 taskCap{};
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==uintptr_t(vehVt.data()))return "veh_vt"; if(v==uintptr_t(pedVt.data()))return "ped_vt"; if(v==uintptr_t(taskVt.data()))return "task_vt";
    struct R { const void* p; size_t n, stride; const char* tag; };
    const R rs[]{{vehStore.data(),vehStore.size(),VSTRIDE,"veh"},{mats.data(),sizeof(mats),MATB,"mat"},{peds.data(),sizeof(peds),PEDB,"ped"},
                 {intels.data(),sizeof(intels),INTELB,"intel"},{playerData.data(),playerData.size(),playerData.size(),"player_data"},
                 {wanted.data(),wanted.size(),wanted.size(),"wanted"},{handling.data(),handling.size(),handling.size(),"handling"},
                 {mis.data(),sizeof(mis),0x80,"mi"},{pedPoolStore.data(),pedPoolStore.size(),PEDB,"pool_ped"},{fakeTasks.data(),sizeof(fakeTasks),TSLOT,"fake_task"},
                 {coorsArg.data(),coorsArg.size(),coorsArg.size(),"coors_arg"}};
    for(auto& r:rs)if(v>=uintptr_t(r.p)&&v<uintptr_t(r.p)+r.n)return std::string(r.tag)+std::to_string((v-uintptr_t(r.p))/r.stride)+"+"+std::to_string((v-uintptr_t(r.p))%r.stride);
    if(taskStor&&v>=taskStor&&v<taskStor+size_t(taskCap)*TSLOT)return "task@"+std::to_string((v-taskStor)/TSLOT)+"+"+std::to_string((v-taskStor)%TSLOT);
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Name(const void* p){ return Name(uintptr_t(p)); }
inline json Words(const void* p,size_t from,size_t to){ json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(At<uint32>(p,k))); return r; }

// --- registro de stubs sin reservas: tipo + valores crudos en un array fijo ---
struct I { int32 v; }; struct U { uint32 v; }; struct Fl { float v; }; struct Pt { const void* p; }; struct Bo { bool v; }; struct V3 { const void* p; };
struct Arg { uint8 kind; uint32 v; };
struct Rec { const char* tag; uint32 n; std::array<Arg,16> a; };
inline std::array<Rec,512> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Put(Rec& r,uint8 k,uint32 v){ if(r.n<r.a.size())r.a[r.n++]={k,v}; }
inline void Add(Rec& r,I x){ Put(r,0,uint32(x.v)); }
inline void Add(Rec& r,U x){ Put(r,1,x.v); }
inline void Add(Rec& r,Fl x){ Put(r,1,std::bit_cast<uint32>(x.v)); }
inline void Add(Rec& r,Pt x){ Put(r,3,uint32(uintptr_t(x.p))); }
inline void Add(Rec& r,Bo x){ Put(r,4,x.v ? 1u : 0u); }
inline void Add(Rec& r,V3 x){ const auto* f=static_cast<const uint32*>(x.p); for(int k=0;k<3;++k)Put(r,1,f[k]); }
template<class... A> void T(const char* tag,A... a){
    if(!Main())return; if(nrec>=recs.size()){ recOverflow=true; return; }
    Rec& r=recs[nrec++]; r.tag=tag; r.n=0; (Add(r,a),...); }
inline json RecsToJson(){
    json out=json::array();
    for(size_t i=0;i<nrec;++i){ const Rec& r=recs[i]; json j=json::array(); j.push_back(r.tag);
        for(uint32 k=0;k<r.n;++k){ const Arg& a=r.a[k];
            switch(a.kind){ case 0: j.push_back(int32(a.v)); break; case 3: j.push_back(Name(uintptr_t(a.v))); break; case 4: j.push_back(a.v!=0); break; default: j.push_back(a.v); } }
        out.push_back(std::move(j)); }
    if(recOverflow)out.push_back("overflow");
    return out; }

// --- copias de tareas (sin reservas) ---
struct TSnap { int32 type; std::array<uint32,TSLOT/4> w; };
inline std::array<TSnap,32> tsnaps{}; inline size_t ntsnap{};
inline uint32 SnapTask(const CTask* t){
    if(!t)return 0xFFFFFFFFu; if(ntsnap>=tsnaps.size())return 0xFFFFFFFEu;
    auto& s=tsnaps[ntsnap]; s.type=int32(t->GetTaskType()); std::memcpy(s.w.data(),t,TSLOT); return uint32(ntsnap++); }
inline json SnapsToJson(){ json out=json::array(); for(size_t i=0;i<ntsnap;++i){ json w=json::array(); w.push_back(tsnaps[i].type); for(size_t k=1;k<TSLOT/4;++k)w.push_back(Name(tsnaps[i].w[k])); out.push_back(std::move(w)); } return out; }
inline std::array<CTask*,24> setTasks{}; inline size_t nsetTasks{};

// trampas de vtable
template<int N> void* __fastcall VTrap(void* v,void*) { T("unexpected_slot",I{N},Pt{v}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,NSLOT>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline void __fastcall VHorn(void* self,void*){ T("PlayCarHorn",Pt{self}); }
inline int32 __fastcall FTType(void* self,void*){ return At<int32>(self,TSLOT-4); }
inline void* __fastcall FTTrap(void* self,void*){ T("fake_task_slot",Pt{self}); return nullptr; }
inline void* __fastcall FTDel(void* self,void*,uint32 flag){ T("fake_task_delete",Pt{self},U{flag}); return self; }

// ======================= dependencias simuladas =======================
inline void* plyrVeh{}; inline CVector playerCoors{}, playerSpeed{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32){ return reinterpret_cast<CPlayerPed*>(Ped(0)); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32,bool){ return static_cast<CVehicle*>(plyrVeh); }
inline CWanted* __cdecl FakeFindPlayerWanted(int32){ return reinterpret_cast<CWanted*>(wanted.data()); }
inline CVector __cdecl FakeFindPlayerCoors(int32){ return playerCoors; }
inline CVector& __cdecl FakeFindPlayerSpeed(int32){ return playerSpeed; }
inline CEntity* __cdecl FakeFindPlayerEntity(int32){ return reinterpret_cast<CEntity*>(plyrVeh ? plyrVeh : Ped(0)); }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand(){ T("rand"); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline uint32 maskA{}, callsA{}, maskB{}, callsB{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; } inline bool BitB(){ return (maskB>>(callsB++%32))&1; }
inline void __cdecl FakeJoin(CVehicle* v){ T("JoinCarWithRoadSystem",Pt{v}); }
inline bool __cdecl FakeJoinGoto(CVehicle* v,const CVector* p,uint32 a,uint32 b){ const bool r=BitA(); T("JoinCarWithRoadSystemGotoCoors",Pt{v},Pt{p},V3{p},U{a&0xFF},U{b&0xFF},Bo{r}); return r; }
inline void __cdecl FakeSwitchReal(CVehicle* v){ T("SwitchVehicleToRealPhysics",Pt{v}); }
inline float speedMultRet{}, maxSteerRet{};
inline float __cdecl FakeSpeedMult(int32 a){ T("FindSpeedMultiplierWithSpeedFromNodes",U{uint32(a)&0xFF}); return speedMultRet; }
inline float __cdecl FakeMaxSteer(CVehicle* v){ T("FindMaxSteerAngle",Pt{v}); return maxSteerRet; }
inline bool __cdecl FakeBridge(CVehicle* v){ const bool r=BitB(); T("ShouldCarStopForBridge",Pt{v},Bo{r}); return r; }
inline bool noPolice{}, abandon{}, usesSiren{}, upsideDown{}, collided{}, swim{};
inline bool __cdecl FakeNoPolice(){ return noPolice; }
inline bool __cdecl FakeAbandon(){ T("PoliceAbandonCars"); return abandon; }
inline bool __fastcall FakeUsesSiren(void*,void*){ return usesSiren; }
inline bool __fastcall FakeUpsideDown(void*,void*){ return upsideDown; }
inline bool __fastcall FakeCollided(void*,void*,void*){ return collided; }
inline std::array<uint32,NPOOL> appear{};
inline uint32 __fastcall FakeAppearance(void* v,void*){ const auto u=uintptr_t(v), b=uintptr_t(vehStore.data()); return u>=b&&u<b+vehStore.size() ? appear[(u-b)/VSTRIDE] : 0u; }
inline CPed* __fastcall FakeSetUpDriver(void* v,void*,int32 type,uint32 a,uint32 b){ T("SetUpDriver",Pt{v},I{type},U{a&0xFF},U{b&0xFF}); return reinterpret_cast<CPed*>(Ped(1)); }
inline CPed* __fastcall FakeSetupPassenger(void* v,void*,int32 seat,int32 type,uint32 a,uint32 b){ T("SetupPassenger",Pt{v},I{seat},I{type},U{a&0xFF},U{b&0xFF}); return reinterpret_cast<CPed*>(Ped(2+uint32(seat)%3)); }
inline void __fastcall FakeGiveDelayed(void* p,void*,int32 w,uint32 ammo){ T("GiveDelayedWeapon",Pt{p},I{w},U{ammo}); }
inline CEntity* __fastcall FakeAttach(void* p,void*,CEntity* e,CVector off,uint32 dir,float ang,int32 w){ T("AttachPedToEntity",Pt{p},Pt{e},V3{&off},U{dir&0xFFFF},Fl{ang},I{w}); return e; }
inline void __fastcall FakeSetWeapon(void* p,void*,int32 w){ T("SetCurrentWeapon",Pt{p},I{w}); }
inline bool __fastcall FakeSetPedOutProcess(void* t,void*,CPed* ped){ T("CarSetPedOut::ProcessPed",Pt{ped},Pt{At<void*>(t,8)},I{At<int32>(t,0xC)},U{At<uint32>(t,0x10)},U{At<uint16>(t,0x14)}); return true; }
inline void __fastcall FakeClearTasks(void* in,void*,uint32 a,uint32 b){ T("ClearTasks",Pt{in},U{a&0xFF},U{b&0xFF}); }
inline std::array<bool,NPED> copFound{};
inline CTask* __fastcall FakeFindTaskByType(void* in,void*,int32 type){
    for(size_t k=0;k<NPED;++k)if(in==intels[k].data())return type==TASK_COMPLEX_COP_IN_CAR&&copFound[k] ? reinterpret_cast<CTask*>(fakeTasks[k].data()) : nullptr;
    return nullptr; }
inline void* __fastcall FakeTaskSwim(void*,void*){ return swim ? fakeTasks[7].data() : nullptr; }
inline void SetTaskImpl(void* tm,int32 slot,CTask* t){ T("SetTask",Pt{tm},I{slot},U{SnapTask(t)}); if(t&&nsetTasks<setTasks.size())setTasks[nsetTasks++]=t; }
inline void __fastcall FakeSetTaskExe(void* tm,void*,CTask* t,int32 slot,uint32){ SetTaskImpl(tm,slot,t); }
inline void __fastcall FakeChangeTask(void* tm,void*,CTask** ref,CTask* t){ SetTaskImpl(tm,int32((uintptr_t(ref)-uintptr_t(tm))/4),t); }
inline void* __fastcall FakeEventAdd(void* grp,void*,CEvent* ev,uint32 valid){
    // CEventScriptCommand: +4 tiempo, +8 válido (byte), +0xC hueco primario, +0x10 tarea, +0x14 muertos (byte); sin relleno
    auto* const task=At<CTask*>(ev,0x10);
    const uint32 s=SnapTask(task); uint32 subs[2]{0xFFFFFFFFu,0xFFFFFFFFu};
    if(task&&task->GetTaskType()==TASK_COMPLEX_SEQUENCE)for(int k=0;k<2;++k)subs[k]=SnapTask(At<CTask*>(task,0x10+4*size_t(k)));
    T("CEventGroup::Add",Pt{grp},I{int32(ev->GetEventType())},I{At<int32>(ev,4)},U{At<uint8>(ev,8)},I{At<int32>(ev,0xC)},U{s},U{At<uint8>(ev,0x14)},U{valid&0xFF},U{subs[0]},U{subs[1]});
    return nullptr; }
inline void __fastcall FakeRegRef(void* self,void*,void* field){ T("RegisterReference",Pt{self},Pt{field}); }
inline void __fastcall FakeCleanRef(void* self,void*,void* field){ T("CleanUpOldReference",Pt{self},Pt{field}); }

// ======================= fixtures =======================
inline void RandMat(Rng& r,uint8* buf,const CVector& pos,bool flip){
    std::memset(buf,0,MATB); CMatrix m{};
    if(flip)m.SetRotate(r.uni(2.5f,3.8f),r.uni(-0.4f,0.4f),r.uni(-3.1f,3.1f));
    else m.SetRotate(r.next()%3 ? 0.f : r.uni(-0.5f,0.5f),r.next()%3 ? 0.f : r.uni(-0.5f,0.5f),r.uni(-3.14f,3.14f));
    m.GetPosition()=pos; std::memcpy(buf,&m,0x40); }
inline CVector Around(Rng& r,const CVector& c,float d){
    if(r.next()%3==0){ const float s=r.next()%2 ? d : -d; return r.next()%2 ? CVector{c.x+s,c.y,c.z} : CVector{c.x,c.y+s,c.z}; }
    const float a=r.uni(-3.14f,3.14f); return CVector{c.x+d*std::cos(a),c.y+d*std::sin(a),c.z+r.pick({0.f,0.f,r.uni(-3.f,3.f)})}; }
inline float Dist(Rng& r){ return r.next()%2 ? r.pick({0.f,0.5f,1.f,1.5f,2.f,3.f,5.f,9.9f,10.f,10.1f,12.f,13.f,20.f,30.f,40.f,45.f,50.f,55.f,70.f,71.f,100.f}) : r.uni(0.f,120.f); }
inline CVector RandSpeed(Rng& r){ const float m=r.pick({0.f,0.01f,0.012f,0.04f,0.05f,0.06f,0.1f,0.3f,0.5f,0.6f,0.8f,r.uni(0.f,1.2f)}); const float a=r.uni(-3.14f,3.14f);
    return CVector{m*std::cos(a),m*std::sin(a),r.next()%3 ? 0.f : r.uni(-0.1f,0.1f)}; }

// vehículo falso k del pool (estado ocupado)
inline uint8* MakeVeh(size_t k,Rng& r,const CVector& pos,bool withMat,bool flip=false){
    uint8* v=Veh(k); std::memset(v,0,VSTRIDE); vehStates[k]=uint8(r.next()&0x7F);
    At<void*>(v,0)=vehVt.data(); At<CVector>(v,4)=pos; At<float>(v,0x10)=r.uni(-3.14f,3.14f);
    if(withMat){ RandMat(r,mats[k].data(),pos,flip); At<void*>(v,0x14)=mats[k].data(); }
    At<uint32>(v,0x1C)=r.next(); At<uint16>(v,0x20)=uint16(r.next()); At<int16>(v,0x22)=MODELS[r.next()%std::size(MODELS)];
    { static const uint8 st[]{2,3,3,2,12,4,5,0,1,7}; v[0x36]=uint8((st[r.next()%std::size(st)]<<3)|(r.next()&7)); }
    At<uint32>(v,0x3C)=CTimer::m_snTimeInMilliseconds-r.next()%1500; At<CVector>(v,0x44)=RandSpeed(r);
    At<void*>(v,0x384)=handling.data();
    // piloto automático (0x390)
    At<uint32>(v,0x3AC)=CTimer::m_snTimeInMilliseconds-r.next()%40000; At<uint32>(v,0x3B0)=CTimer::m_snTimeInMilliseconds-(r.next()%3 ? 29000+r.next()%2002 : r.next()%60000);
    v[0x3B9]=uint8(r.next()%8); v[0x3BA]=uint8(r.next()%4 ? r.next()%69 : r.next()); v[0x3BB]=uint8(r.next()%2 ? 0 : r.pickI({1,3,7,8,20,21,23,24,int32(r.next()%25)}));
    At<uint32>(v,0x3BC)=r.next(); At<uint32>(v,0x3C0)=CTimer::m_snTimeInMilliseconds-r.next()%20000; v[0x3C4]=uint8(r.next()%4);
    v[0x3D0]=uint8(r.next()%4 ? r.next()%60 : 0); v[0x3D1]=uint8(r.next()); At<float>(v,0x3D4)=r.pick({1.f,0.5f,r.uni(0.f,1.5f)});
    v[0x3DD]=uint8(r.next()%6 ? r.next()%60 : r.pickI({0,239,240,255})); v[0x3DE]=uint8(r.next());
    At<CVector>(v,0x3EC)=Around(r,pos,Dist(r)); At<int8>(v,0x424)=-1;
    for(size_t b=0;b<8;++b)v[0x428+b]=uint8(r.next());
    At<float>(v,0x494)=r.uni(-0.6f,0.6f); v[0x4A4]=uint8(r.next()%4 ? 1 : r.pickI({0,2,3}));
    At<uint16>(v,0x4DC)=uint16(r.pickI({0,0,2400,2500,2501,20000,int32(r.next()%30000)})); At<uint32>(v,0x514)=r.next()%100;
    At<uint32>(v,0x590)=uint32(r.pickI({0,0,0,9,5,3,4})); At<uint32>(v,0x594)=uint32(r.next()%8 ? 0 : r.pickI({3,5,9}));
    appear[k]=uint32(r.next()%6);
    return v; }

inline void MakePed(size_t k,Rng& r,const CVector& pos){
    uint8* p=Ped(k); std::memset(p,0,PEDB); std::memset(intels[k].data(),0,INTELB);
    At<void*>(p,0)=pedVt.data(); At<CVector>(p,4)=pos; At<CVector>(p,0x44)=RandSpeed(r);
    At<uint32>(p,0x46C)=r.next()&~0x200000u; At<void*>(p,0x47C)=intels[k].data(); p[0x484]=uint8(r.next()%4 ? 1 : 2); At<uint32>(p,0x598)=r.next()%20;
    if(k==0)At<void*>(p,0x480)=playerData.data(); }

inline void MakeFakeTask(size_t k,int32 type){ std::memset(fakeTasks[k].data(),0,TSLOT); At<void*>(fakeTasks[k].data(),0)=taskVt.data(); At<int32>(fakeTasks[k].data(),TSLOT-4)=type; }

// mundo común
inline void MakeWorld(Rng& r){
    vehStore.fill(0); vehStates.fill(0x80);
    std::memset(vehPoolHdr.data(),0,vehPoolHdr.size()); At<void*>(vehPoolHdr.data(),0)=vehStore.data(); At<void*>(vehPoolHdr.data(),4)=vehStates.data(); At<int32>(vehPoolHdr.data(),8)=int32(NPOOL); At<int32>(vehPoolHdr.data(),0xC)=-1;
    std::memset(pedPoolHdr.data(),0,pedPoolHdr.size()); At<void*>(pedPoolHdr.data(),0)=pedPoolStore.data(); At<void*>(pedPoolHdr.data(),4)=pedPoolStates.data(); At<int32>(pedPoolHdr.data(),8)=int32(NPEDPOOL); At<int32>(pedPoolHdr.data(),0xC)=-1;
    *reinterpret_cast<void**>(0xB74494)=vehPoolHdr.data(); *reinterpret_cast<void**>(0xB74490)=pedPoolHdr.data();
    CTimer::m_snTimeInMilliseconds=100000+r.next()%100000000; CTimer::m_snPreviousTimeInMilliseconds=CTimer::m_snTimeInMilliseconds-r.next()%100;
    CTimer::m_FrameCounter=r.next()%3==0 ? (r.next()&~0xFu)|14u : r.next()%3==0 ? r.next()&~7u : r.next();
    CTimer::ms_fTimeStep=r.pick({1.f,0.5f,2.f,3.f,r.uni(0.f,3.f)});
    for(size_t k=0;k<NFAKET;++k)MakeFakeTask(k,TASK_COMPLEX_COP_IN_CAR);
    for(auto& m:mis){ std::memset(m.data(),0,m.size()); m[0x4D]=uint8(r.next()%3 ? r.next()%5 : 5); }
    for(size_t k=0;k<std::size(MODELS);++k)reinterpret_cast<void*&>(CModelInfo::ms_modelInfoPtrs[MODELS[k]])=mis[k%mis.size()].data();
    std::memset(handling.data(),0,handling.size()); At<float>(handling.data(),0x88)=r.pick({0.5f,1.f,r.uni(0.f,3.f),r.uni(0.f,300.f)});
    std::memset(playerData.data(),0,playerData.size()); At<void*>(playerData.data(),0)=wanted.data();
    std::memset(wanted.data(),0,wanted.size()); wanted[0x1E]=uint8(r.next()%3 ? 0 : r.next()&0x3F); At<int32>(wanted.data(),0x2C)=r.next()%8 ? int32(r.next()%7) : r.pickI({-1,7,100});
    for(size_t k=0;k<NPED;++k){ MakePed(k,r,CVector{r.uni(-100.f,100.f),r.uni(-100.f,100.f),r.uni(0.f,20.f)}); copFound[k]=r.next()%3==0; }
    pedPoolStore.fill(0); pedPoolStates.fill(0x80);
    plyrVeh=nullptr; playerCoors=CVector{0.f,0.f,0.f}; playerSpeed=RandSpeed(r);
    maskA=r.next(); callsA=0; maskB=r.next(); callsB=0;
    noPolice=r.next()%4==0; abandon=r.next()%3==0; usesSiren=r.next()%2; upsideDown=r.next()%5==0; collided=r.next()%2; swim=r.next()%3==0;
    speedMultRet=r.pick({1.f,0.5f,0.8f,r.uni(0.f,1.5f)}); maxSteerRet=r.pick({0.2f,0.7f,r.uni(0.f,0.9f)});
    std::memset(coorsArg.data(),0,coorsArg.size());
    *reinterpret_cast<uint8*>(0xC0BC5C)=uint8(r.next()%2);
}

// jugador en un vehículo (V2) o a pie, cerca de pos
inline void MakePlayer(Rng& r,const CVector& pos){
    playerCoors=Around(r,pos,Dist(r));
    At<CVector>(Ped(0),4)=playerCoors; At<CVector>(Ped(0),0x44)=r.next()%3==0 ? RandSpeed(r) : playerSpeed;
    if(r.next()%5<3){ uint8* pv=MakeVeh(2,r,playerCoors,true,r.next()%8==0); plyrVeh=pv; if(r.next()%4)At<CVector>(pv,0x44)=playerSpeed; }
}

inline json Run(const std::string& nm,uintptr_t a,Rng& r,json& out){
    MakeWorld(r); json ret=nullptr;
    const CVector P{r.uni(-2000.f,2000.f),r.uni(-2000.f,2000.f),r.uni(0.f,50.f)};
    using VP=void*;
    if(nm=="AddAmbulanceOccupants"||nm=="AddFiretruckOccupants"){
        uint8* v=MakeVeh(0,r,P,r.next()%2); reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    else if(nm=="AddPoliceCarOccupants"){
        uint8* v=MakeVeh(0,r,P,r.next()%2); At<int16>(v,0x22)=r.next()%6 ? int16(r.pickI({427,490,430,432,523,433,596,597,598,599})) : int16(r.pickI({400,401,428,431,520}));
        if(r.next()%4)v[0x42A]&=~0x40; At<int32>(wanted.data(),0x2C)=r.next()%8 ? int32(r.next()%7) : 0;
        reinterpret_cast<void(__cdecl*)(VP,uint32)>(a)(v,r.next()%2); }
    else if(nm=="BackToCruisingIfNoWantedLevel"){
        uint8* v=MakeVeh(0,r,P,r.next()%2); if(r.next()%4)v[0x428]|=1; At<int32>(wanted.data(),0x2C)=int32(r.next()%4); wanted[0x1E]=uint8(r.next()%2 ? 0 : 1u<<(r.next()%4));
        reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    else if(nm=="CarHasReasonToStop"){ uint8* v=MakeVeh(0,r,P,true); reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    else if(nm=="EntitiesGoHeadOn"){
        auto Ent=[&](int which,const CVector& pos)->uint8*{ const bool m=r.next()%3!=0;
            if(which==2){ uint8* p=Ped(5); At<CVector>(p,4)=pos; At<float>(p,0x10)=r.uni(-3.14f,3.14f); if(m){ RandMat(r,mats[NPOOL+1].data(),pos,false); At<void*>(p,0x14)=mats[NPOOL+1].data(); } else At<void*>(p,0x14)=nullptr; return p; }
            return MakeVeh(size_t(which),r,pos,m); };
        uint8* e1=Ent(0,P); const CVector Q=r.next()%10==0 ? P : Around(r,P,r.uni(0.1f,60.f)); uint8* e2=Ent(r.next()%2 ? 1 : 2,Q);
        // orientaciones cara a cara a veces
        if(r.next()%2){ const CVector d=P-Q; const float h=std::atan2(-d.x,d.y)+r.pick({0.f,r.uni(-0.8f,0.8f),3.14159f});
            for(uint8* e:{e1,e2}){ auto* m=At<uint8*>(e,0x14); const float hh=e==e1 ? h+3.14159f+r.pick({0.f,r.uni(-0.7f,0.7f)}) : h;
                if(m){ CMatrix mm{}; mm.SetRotate(0.f,0.f,hh); mm.GetPosition()=At<CVector>(m,0x30); std::memcpy(m,&mm,0x40); } else At<float>(e,0x10)=hh; } }
        ret=reinterpret_cast<uint8(__cdecl*)(VP,VP)>(a)(e1,e2)!=0; }
    else if(nm=="FindPoliceBikeMissionForWantedLevel"||nm=="FindPoliceBoatMissionForWantedLevel"||nm=="FindPoliceCarMissionForWantedLevel"){
        At<int32>(wanted.data(),0x2C)=r.next()%5 ? int32(r.next()%7) : r.pickI({-1,7,8,100}); if(r.next()%2)plyrVeh=MakeVeh(2,r,P,true);
        ret=int32(int8(reinterpret_cast<uint8(__cdecl*)()>(a)())); }
    else if(nm=="FindPoliceCarSpeedForWantedLevel"){
        uint8* v=MakeVeh(0,r,P,true); At<int32>(wanted.data(),0x2C)=r.next()%5 ? int32(r.next()%7) : r.pickI({-1,7,8,100});
        ret=reinterpret_cast<int32(__cdecl*)(VP)>(a)(v); }
    else if(nm=="FindSwitchDistanceClose"||nm=="FindSwitchDistanceFar"){
        uint8* v=MakeVeh(0,r,P,true); v[0x3DD]=uint8(r.next()); ret=F(reinterpret_cast<float(__cdecl*)(VP)>(a)(v)); }
    else if(nm.rfind("GetCarToGoToCoors",0)==0){
        uint8* v=MakeVeh(0,r,P,r.next()%2);
        { static const int8 ms[]{8,9,12,13,33,45,46,47,48,49,50,1,0,2,4,11}; v[0x3BA]=uint8(r.next()%5 ? ms[r.next()%std::size(ms)] : int8(r.next())); }
        const CVector dest=At<CVector>(v,0x3EC); const float th=r.pick({2.f,5.f,std::nextafter(2.f,9.f),std::nextafter(5.f,9.f),r.uni(0.f,8.f)});
        At<CVector>(coorsArg.data(),0)=r.next()%3 ? CVector{dest.x+(r.next()%2 ? th : -th),dest.y+r.uni(-th,th),dest.z} : CVector{dest.x+r.uni(-th,th),dest.y+(r.next()%2 ? th : -th),r.uni(0.f,30.f)};
        if(r.next()%6==0)At<CVector>(coorsArg.data(),0)=Around(r,P,Dist(r));
        const int32 style=int32(r.next()%7); const uint32 cruise=r.next()%2;
        ret=F(reinterpret_cast<float(__cdecl*)(VP,const void*,int32,uint32)>(a)(v,coorsArg.data(),style,cruise)); }
    else if(nm=="GetCarToParkAtCoors"){
        uint8* v=MakeVeh(0,r,P,r.next()%2); { static const int8 ms[]{8,9,45,46,49,50,1,0}; v[0x3BA]=uint8(ms[r.next()%std::size(ms)]); }
        const CVector dest=At<CVector>(v,0x3EC); At<CVector>(coorsArg.data(),0)=r.next()%2 ? CVector{dest.x+r.uni(-6.f,6.f),dest.y+r.uni(-6.f,6.f),dest.z} : Around(r,P,Dist(r));
        ret=F(reinterpret_cast<float(__cdecl*)(VP,const void*)>(a)(v,coorsArg.data())); }
    else if(nm=="MakeWayForCarWithSiren"){
        uint8* s=MakeVeh(0,r,P,true); { const float m=r.pick({0.f,0.05f,0.0999f,0.1f,0.2f,0.5f,r.uni(0.f,1.f)}); const float an=r.uni(-3.14f,3.14f); At<CVector>(s,0x44)=CVector{m*std::cos(an),m*std::sin(an),0.f}; }
        const float speed=std::sqrt(At<float>(s,0x44)*At<float>(s,0x44)+At<float>(s,0x48)*At<float>(s,0x48)); const float reach=speed*45.f+40.f;
        for(size_t k=1;k<NPOOL;++k){
            if(r.next()%10==0)continue;
            CVector q=Around(r,P,r.next()%3 ? r.uni(0.f,reach) : r.uni(0.f,reach+30.f)); q.z=P.z+r.pick({0.f,4.9f,5.f,5.1f,-4.9f,-5.f,r.uni(-8.f,8.f)});
            uint8* v=MakeVeh(k,r,q,true);
            At<uint32>(v,0x590)=uint32(r.pickI({0,0,0,9,9,5,3})); v[0x36]=uint8((r.pickI({2,3,3,2,4,12})<<3)|(v[0x36]&7));
            v[0x4A4]=uint8(r.next()%5 ? 1 : 2); v[0x428]=uint8(r.next()%3 ? r.next()&0xF0 : r.next()); v[0x42E]=uint8(r.next()%5 ? r.next()&~8u : r.next());
            At<void*>(v,0x460)=r.next()%2 ? nullptr : Ped(1+r.next()%2);
            if(r.next()%3==0){ CMatrix mm{}; const float h=std::atan2(-At<float>(s,0x44),At<float>(s,0x48))+r.pick({0.f,3.14159f,r.uni(-1.f,1.f)}); mm.SetRotate(0.f,0.f,h); mm.GetPosition()=q; std::memcpy(mats[k].data(),&mm,0x40);
                               At<CVector>(v,0x44)=CVector{-std::sin(h)*r.pick({0.05f,0.06f,0.3f}),std::cos(h)*r.pick({0.05f,0.06f,0.3f}),0.f}; }
        }
        reinterpret_cast<void(__cdecl*)(VP)>(a)(s); }
    else if(nm=="MellowOutChaseSpeed"){
        uint8* v=MakeVeh(0,r,P,r.next()%2); At<int32>(wanted.data(),0x2C)=r.next()%6 ? int32(r.next()%3) : int32(r.next()%7);
        playerCoors=Around(r,P,r.next()%2 ? r.pick({9.9f,10.f,10.1f,19.9f,20.f,20.1f,29.9f,30.f,30.1f,39.9f,40.f,40.1f}) : r.uni(0.f,60.f));
        if(r.next()%2)plyrVeh=MakeVeh(2,r,playerCoors,true);
        { const float m=r.pick({0.f,0.069f,0.07f,0.0701f,r.uni(0.f,0.2f)}); At<CVector>(Ped(0),0x44)=CVector{m,0.f,0.f}; }
        reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    else if(nm=="MellowOutChaseSpeedBoat"){
        uint8* v=MakeVeh(0,r,P,true); At<int32>(wanted.data(),0x2C)=r.next()%5 ? int32(r.next()%7) : r.pickI({-1,7,8,100}); reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    else if(nm=="TellCarToBlockOtherCar"||nm=="TellCarToRamOtherCar"||nm=="TellCarToFollowOtherCar"){
        uint8* v=MakeVeh(0,r,P,true); uint8* t=MakeVeh(1,r,Around(r,P,Dist(r)),true);
        if(nm=="TellCarToFollowOtherCar")reinterpret_cast<void(__cdecl*)(VP,VP,float)>(a)(v,t,r.pick({0.f,5.f,10.5f,255.9f,256.f,-3.f,r.uni(0.f,300.f)}));
        else reinterpret_cast<void(__cdecl*)(VP,VP)>(a)(v,t); }
    else if(nm=="TellOccupantsToLeaveCar"){
        uint8* v=MakeVeh(0,r,P,true); At<void*>(v,0x460)=r.next()%4 ? Ped(1) : nullptr;
        const uint32 n=r.next()%9; v[0x488]=uint8(n); for(uint32 k=0;k<8;++k)At<void*>(v,0x464+4*k)=k<n&&r.next()%4 ? Ped(2+k%4) : nullptr;
        reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    else if(nm=="UpdateCarAI"){
        const bool flip=r.next()%8==0; uint8* v=MakeVeh(0,r,P,true,flip);
        if(r.next()%5)v[0x428]=uint8((v[0x428]&~1u)|(r.next()%5<3 ? 1u : 0u));
        { static const uint8 st[]{2,3,12,2,3,12,2,3,4,5,0,1,7}; v[0x36]=uint8((st[r.next()%std::size(st)]<<3)|(v[0x36]&7)); }
        v[0x3BA]=uint8(r.next()%10 ? r.next()%69 : r.pickI({-1,69,100}));
        At<uint32>(v,0x594)=uint32(r.next()%12 ? 0 : r.pickI({3,5,9}));
        if(r.next()%8==0){ At<int8>(v,0x424)=int8(r.next()%4); *reinterpret_cast<uint8*>(0x97D6C0+At<int8>(v,0x424))=uint8(r.next()%2); }
        At<int16>(v,0x22)=int16(r.pickI({432,523,596,400,427,523,432}));
        At<uint16>(v,0x20)=uint16(r.next()%3 ? r.next() : r.pickI({10000,10001,9999,0,8,16}));
        // objetivo, destino y jugador a distancias de los umbrales
        At<CVector>(v,0x3EC)=Around(r,P,r.next()%3 ? r.pick({0.5f,1.f,1.5f,3.f,float(v[0x3DD]),float(v[0x3DD])+5.f}) : Dist(r));
        At<void*>(v,0x41C)=r.next()%4 ? static_cast<void*>(MakeVeh(1,r,Around(r,P,r.next()%3 ? r.pick({float(v[0x3DD]),float(v[0x3DD])+5.f,50.f}) : Dist(r)),r.next()%4!=0)) : nullptr;
        MakePlayer(r,P);
        if(r.next()%3==0){ const float d=r.pick({float(v[0x3DD]),float(v[0x3DD])+5.f,50.f,10.f,13.f,70.f}); playerCoors=Around(r,P,d); At<CVector>(Ped(0),4)=playerCoors; }
        if(plyrVeh&&r.next()%2){ At<void*>(v,0x41C)=plyrVeh; }
        At<CVector>(v,0x44)=r.next()%4 ? RandSpeed(r) : CVector{r.pick({0.f,0.01f,0.05f,0.0501f}),0.f,0.f};
        // persecución: forwards parecidos/opuestos y velocidades del jugador
        if(plyrVeh&&r.next()%2){ auto* pm=At<uint8*>(static_cast<uint8*>(plyrVeh),0x14); auto* vm=At<uint8*>(v,0x14); const float h=r.uni(-3.14f,3.14f);
            CMatrix a1{}; a1.SetRotate(0.f,0.f,h); a1.GetPosition()=At<CVector>(pm,0x30); std::memcpy(pm,&a1,0x40);
            CMatrix a2{}; a2.SetRotate(0.f,0.f,h+r.pick({0.f,0.3f,3.14159f,3.f,r.uni(-3.f,3.f)})); a2.GetPosition()=P; std::memcpy(vm,&a2,0x40);
            playerSpeed=CVector{-std::sin(h),std::cos(h),0.f}*r.pick({0.05f,0.11f,0.31f,0.61f,0.9f,r.uni(0.f,1.f)}); At<CVector>(static_cast<uint8*>(plyrVeh),0x44)=playerSpeed;
            if(r.next()%2)playerCoors=P+CVector{-std::sin(h),std::cos(h),0.f}*r.pick({5.f,11.f,19.f,30.f,44.f}); }
        // conductor (moto de policía): tarea primaria falsa
        if(r.next()%2){ At<void*>(v,0x460)=Ped(1);
            switch(r.next()%4){ case 0: break;
                case 1: MakeFakeTask(5,TASK_COMPLEX_LEAVE_CAR); At<void*>(intels[1].data(),0x10)=fakeTasks[5].data(); break;
                default: MakeFakeTask(5,TASK_COMPLEX_SEQUENCE); MakeFakeTask(6,r.next()%2 ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER : TASK_SIMPLE_GANG_DRIVEBY);
                    At<void*>(fakeTasks[5].data(),0x10+4*(r.next()%3))=r.next()%3 ? fakeTasks[6].data() : nullptr; At<void*>(intels[1].data(),0x10)=fakeTasks[5].data(); break; } }
        if(r.next()%3){ const uint32 n=r.next()%5; v[0x488]=uint8(n); for(uint32 k=0;k<n;++k)At<void*>(v,0x464+4*k)=Ped(2+k%3); }
        // peatones del pool falso (zona de bandas)
        for(size_t k=0;k<NPEDPOOL;++k){ if(r.next()%4==0)continue; pedPoolStates[k]=uint8(r.next()&0x7F); uint8* p=pedPoolStore.data()+k*PEDB;
            At<CVector>(p,4)=Around(r,P,r.pick({5.f,9.99f,10.f,10.01f,r.uni(0.f,20.f)})); At<uint32>(p,0x598)=uint32(r.pickI({6,7,8,12,16,17,18,int32(r.next()%25)})); }
        { const float target=v[0x3BA]==1 ? speedMultRet : 1.f; const float step=CTimer::ms_fTimeStep*0.01f; At<float>(v,0x3D4)=r.pick({target,target+step,target-step,target+step*0.5f,target+2.f*step,r.uni(0.f,1.5f)}); }
        reinterpret_cast<void(__cdecl*)(VP)>(a)(v); }
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=25;
inline const std::array<Fn,NFN> fns{{
    {"CCarAI","AddAmbulanceOccupants",0x41C4A0,0,-1,128},{"CCarAI","AddFiretruckOccupants",0x41C600,0,-1,128},{"CCarAI","AddPoliceCarOccupants",0x41C070,0,-1,512},
    {"CCarAI","BackToCruisingIfNoWantedLevel",0x41BFA0,0,-1,512},{"CCarAI","CarHasReasonToStop",0x41C050,0,-1,64},{"CCarAI","EntitiesGoHeadOn",0x41CD00,0,-1,1024},
    {"CCarAI","FindPoliceBikeMissionForWantedLevel",0x41CA40,0,-1,64},{"CCarAI","FindPoliceBoatMissionForWantedLevel",0x41CA50,0,-1,256},
    {"CCarAI","FindPoliceCarMissionForWantedLevel",0x41C9D0,0,-1,512},{"CCarAI","FindPoliceCarSpeedForWantedLevel",0x41CAA0,0,-1,512},
    {"CCarAI","FindSwitchDistanceClose",0x41BF50,0,-1,128},{"CCarAI","FindSwitchDistanceFar",0x41BF70,0,-1,128},
    {"CCarAI","GetCarToGoToCoors",0x41CE30,0,-1,512},{"CCarAI","GetCarToGoToCoorsAccurate",0x41D0E0,0,-1,512},{"CCarAI","GetCarToGoToCoorsRacing",0x41D210,0,-1,512},
    {"CCarAI","GetCarToGoToCoorsStraightLine",0x41CFB0,0,-1,512},{"CCarAI","GetCarToParkAtCoors",0x41D350,0,-1,256},{"CCarAI","MakeWayForCarWithSiren",0x41D660,0,-1,1024},
    {"CCarAI","MellowOutChaseSpeed",0x41D3D0,0,-1,1024},{"CCarAI","MellowOutChaseSpeedBoat",0x41CB70,0,-1,128},{"CCarAI","TellCarToBlockOtherCar",0x41C900,0,-1,128},
    {"CCarAI","TellCarToFollowOtherCar",0x41C960,0,-1,256},{"CCarAI","TellCarToRamOtherCar",0x41C8A0,0,-1,128},{"CCarAI","TellOccupantsToLeaveCar",0x41C760,0,-1,512},
    {"CCarAI","UpdateCarAI",0x41DA30,0,-1,2048},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carai25-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carai25-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carai25");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"carai25-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // pool de tareas real: m_LastFreeSlot inicial y ocupación de referencia
    auto* const tpool=reinterpret_cast<uint8*>(GetTaskPool());
    taskStor=uintptr_t(*reinterpret_cast<uint8**>(tpool)); taskCap=*reinterpret_cast<int32*>(tpool+8);
    auto* const tstates=*reinterpret_cast<uint8**>(tpool+4); const int32 lf0=*reinterpret_cast<int32*>(tpool+0xC);
    auto Used=[&]{ int32 n=0; for(int32 k=0;k<taskCap;++k)n+=!(tstates[k]&0x80); return n; };
    const int32 used0=Used();

    // estado global guardado
    regions.clear();
    Snap(0xB7CB84,4); Snap(0xB7CB78,4); Snap(0xB7CB4C,4); Snap(0xB7CB5C,4); Snap(0xB74494,4); Snap(0xB74490,4); Snap(0xC0BC5C,1); Snap(0x97D6C0,8);
    for(const int16 m:MODELS)Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[m]),4);

    FillTraps(vehVt,std::make_integer_sequence<int,int(NSLOT)>{}); FillTraps(pedVt,std::make_integer_sequence<int,int(NSLOT)>{});
    vehVt[54]=reinterpret_cast<void*>(&VHorn);
    for(auto& s:taskVt)s=reinterpret_cast<void*>(&FTTrap); taskVt[0]=reinterpret_cast<void*>(&FTDel); taskVt[4]=reinterpret_cast<void*>(&FTType);

    bool passed=true; int pair=0;
    {
        using std::bit_cast;
        Patches p;
        // Cuerpo de la DLL resuelto ANTES de escribir parches (si no, FollowIn seguiría el salto recién escrito hasta el
        // propio stub y lo parchearía con un salto a sí mismo). Nunca se parchea un stub ya registrado.
        std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t x){ return x&&!stubSet.count(x); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        P(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed);
        P(0x56E0D0,Addr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle);
        P(0x56E230,Addr(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FakeFindPlayerWanted);
        P(0x56E010,Addr(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeFindPlayerCoors);
        P(0x56E090,Addr(static_cast<CVector&(*)(int32)>(&FindPlayerSpeed)),FakeFindPlayerSpeed);
        P(0x56E120,Addr(static_cast<CEntity*(*)(int32)>(&FindPlayerEntity)),FakeFindPlayerEntity);
        P(0x821B1E,0,FakeRand); P(0,Addr(&CGeneral::GetRandomNumber),FakeRandomNumber);
        P(0x42F5A0,Addr(&CCarCtrl::JoinCarWithRoadSystem),FakeJoin); P(0x42F870,Addr(&CCarCtrl::JoinCarWithRoadSystemGotoCoors),FakeJoinGoto);
        P(0x423FC0,Addr(&CCarCtrl::SwitchVehicleToRealPhysics),FakeSwitchReal); P(0x424130,Addr(&CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes),FakeSpeedMult);
        P(0x427FE0,Addr(&CCarCtrl::FindMaxSteerAngle),FakeMaxSteer); P(0x49D420,Addr(&CTrafficLights::ShouldCarStopForBridge),FakeBridge);
        P(0x72DD50,Addr(&CCullZones::NoPolice),FakeNoPolice); P(0x72DD60,Addr(&CCullZones::PoliceAbandonCars),FakeAbandon);
        P(0x6D8470,Addr(&CVehicle::UsesSiren),FakeUsesSiren); P(0x6D1D90,Addr(&CVehicle::IsUpsideDown),FakeUpsideDown);
        P(0x543540,Addr(&CPhysical::GetHasCollidedWith),FakeCollided);
        P(0x6D1080,Addr(&CVehicle::GetVehicleAppearance),FakeAppearance); P(0,Addr(&CVehicle::GetVehicleAppearance_Reversed),FakeAppearance);
        P(0x6D1A50,Addr(&CVehicle::SetUpDriver),FakeSetUpDriver); P(0x6D1AA0,Addr(&CVehicle::SetupPassenger),FakeSetupPassenger);
        P(0x5E89B0,Addr(&CPed::GiveDelayedWeapon),FakeGiveDelayed); P(0x5E7CB0,Addr(&CPed::AttachPedToEntity),FakeAttach);
        P(0x5E6280,Addr(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),FakeSetWeapon);
        P(0x647D10,OurVirtual("CTaskSimpleCarSetPedOut","ProcessPed"),FakeSetPedOutProcess);
        P(0x601420,Addr(&CPedIntelligence::ClearTasks),FakeClearTasks); P(0x600EE0,Addr(&CPedIntelligence::FindTaskByType),FakeFindTaskByType);
        P(0x601070,Addr(&CPedIntelligence::GetTaskSwim),FakeTaskSwim);
        P(0x681AF0,Addr(&CTaskManager::SetTask),FakeSetTaskExe); P(0,Addr(Get(ChangeTaskTag{})),FakeChangeTask);
        P(0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeEventAdd);
        P(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegRef);
        P(0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanRef);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xCA25u}; if(!r.s)r.s=1;
                    nrec=0; recOverflow=false; ntsnap=0; nsetTasks=0; seed=uint32(i); rngCalls=0; RestoreRegions();
                    // el New del pool busca desde m_LastFreeSlot+1; huecos libres a 0xCD (bytes que el constructor no escribe)
                    *reinterpret_cast<int32*>(tpool+0xC)=lf0;
                    for(int32 k=0;k<taskCap;++k)if(tstates[k]&0x80)std::memset(reinterpret_cast<uint8*>(taskStor)+size_t(k)*TSLOT,0xCD,TSLOT);
                    for(auto* h:hooks)h->State(candidate);
                    for(auto& e:p.v)e->Enable(true); // los parches se reafirman después de fijar los hooks
                    json out; out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate; // comprobado DESPUÉS de State
                    json ret=Run(nm,fn.address,r,out);
                    out["ret"]=ret; out["trace"]=RecsToJson(); out["tasks"]=SnapsToJson();
                    out["veh0"]=Words(Veh(0),4,0x5A0);
                    { json h=json::array(); for(size_t k=1;k<NPOOL;++k)h.push_back(Fnv(Veh(k),VSTRIDE)); out["vehs"]=h; }
                    { json h=json::array(); for(size_t k=0;k<NPED;++k){ h.push_back(Fnv(peds[k].data(),PEDB)); h.push_back(Fnv(intels[k].data(),INTELB)); } out["peds"]=h; }
                    { json h=json::array(); for(auto& t:fakeTasks)h.push_back(Fnv(t.data(),TSLOT)); out["fake_tasks"]=h; }
                    out["wanted"]=Fnv(wanted.data(),wanted.size()); out["pool_peds"]=Fnv(pedPoolStore.data(),pedPoolStore.size());
                    // tareas entregadas a SetTask: se borran después de copiarlas (los stubs de referencias siguen puestos)
                    for(size_t k=0;k<nsetTasks;++k)delete setTasks[k];
                    nsetTasks=0;
                    out["task_pool_used"]=Used()-used0;
                    RestoreRegions();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["task_pool_used"]==0;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions(); *reinterpret_cast<int32*>(tpool+0xC)=lf0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()&&Used()==used0}});
}
}
