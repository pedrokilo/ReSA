#pragma once
// Banco dentro del motor: CPed, grupo D (ciclo de vida, estado y consultas; 26 funciones).
// Original (hooks del grupo desactivados) frente a candidato con fixtures identicos sobre peatones FALSOS: nunca
// el jugador ni peatones reales. Los peatones falsos viven en el almacenamiento de un pool de peatones FALSO
// (140 huecos de sizeof(CCopPed) = 0x7C4 bytes, estados de hueco propios) cuyo puntero (0xB74490) solo se cambia
// durante la llamada probada: asi GetRef/GetIndex de la DLL (que comprueban que el puntero sea del pool) funcionan.
// Los constructores/destructores se llaman en la direccion del EXE sobre un buffer relleno con 0xCD (el
// constructor) o con bytes deterministas (el destructor) y se compara el peaton entero (salvo la vptr, los cuatro
// vptr de los subobjetos de audio y, en el constructor, los bits de relleno de los flags y el relleno de CWeapon).
// Se simulan y trazan (direccion del EXE + simbolo/cuerpo de la DLL, sin reservar memoria dentro de los stubs):
//   constructor: base CPhysical (rellena 0..0x138 con bytes por caso), Initialise de los audios (peatonal y de arma),
//     GetPedTypeAcquaintances, GiveWeapon, operator new/constructor de CPedIntelligence, operator new de CTask,
//     CTaskManager::SetTask/SetTaskSecondary (con tipo y campos de la tarea), CPopulation::UpdatePedCount, rand,
//     FindPlayerPed, GetPedFlag, SetAsAcquaintance, CEventGroup::Add, RegisterReference/CleanUpOldReference;
//   destructor: RecordPedDeleted, SetMissionDoesntRequireModel, RemoveFromWaitingForScriptBrainArray, CWorld::Remove,
//     ClearBlipForEntity, RemoveConversationForPed, CleanUpOldReference, CFire::Extinguish, ReleaseCoverPointForPed,
//     ClearWeapons, UpdatePedCount, Terminate de los tres audios (el de voz por vtable falsa), ~CPedIntelligence y su
//     operator delete, ~CPhysical;
//   resto: FindPlayerPed, CPedGroupMembership::IsFollower, IsVehiclePointerValid, CVehicle::RemovePassenger,
//     CPedIntelligence::SetPedDecisionMakerType, CStats::GetStatValue, CPedType::Initialise,
//     CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle, CPedSaveStructure::Construct/Extract y
//     CGenericGameStorage::Save/LoadDataFromWorkBuffer (buffer falso).
// Se guardan y restauran: puntero del pool de peatones, trucos, CPopulation::NumMiamiViceCops, ms_apPedStats,
// ScriptsForBrains y dos model infos.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/PedSaveStructure.h"
#include "PedType.h"
#include "PedStats.h"
#include "Conversations.h"
#include "Radar.h"
#include "CoverPoint.h"
#include "EventAcquaintancePedHate.h"
#include "../ProbeRuntime.hpp"

namespace CPedDLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; } };
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }

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

// --- modulo de la DLL y busqueda de cuerpos (como Boat23 / VehCtor62) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline bool Prologue(uintptr_t f){ const auto* b=reinterpret_cast<const uint8*>(f); if(b[0]==0x8B&&b[1]==0xFF)b+=2; return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC; }
// primera llamada (E8) a una funcion de la DLL con prologo push ebp / mov ebp,esp
inline uintptr_t FirstDllCall(uintptr_t fn){ fn=FollowIn(fn); if(!InDll(fn))return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(InDll(t)&&Prologue(FollowIn(t)))return FollowIn(t); }
    return 0; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// ======================= objetos falsos =======================
constexpr size_t PED=sizeof(CPed), SLOT=sizeof(CCopPed), NSLOT=140;
static_assert(PED==0x79C&&SLOT==0x7C4,"tamano de CPed / elemento del pool de peatones");
constexpr size_t SA=5, SB=9, SPL=2;       // huecos: peaton A (probado), peaton B (otro), "jugador"
constexpr size_t AUD0=0x138, AUD1=0x43C;   // audio del peaton, de voz y de armas (subobjetos incrustados)
constexpr size_t VEHN=0xA00, INTELN=0x300;
alignas(16) inline std::array<uint8,SLOT*NSLOT+0x1000> pedStore{};
alignas(16) inline std::array<uint8,0x400> slotRaw{};     // estados de hueco: el array empieza en +0x100
alignas(16) inline std::array<uint8,0x20> fakePool{};
alignas(16) inline std::array<uint8,VEHN> veh{}, veh2{};
alignas(16) inline std::array<uint8,INTELN> intel{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> taskBuf{};
alignas(16) inline std::array<uint8,0x100> playerData{};
alignas(16) inline std::array<uint8,0x40> statBuf{}, acq{}, mi{}, mi2{};
inline std::array<void*,16> speechVt{};
inline uint8* Slot(size_t k){ return pedStore.data()+k*SLOT; }
inline uint8* StateBase(){ return slotRaw.data()+0x100; }

// desplazamientos crudos del EXE
constexpr size_t P_ENTFLAGS=0x1C, P_STATUS=0x36, P_MODEL=0x22, P_COLLIST=0xB0, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478,
                 P_INTEL=0x47C, P_PLAYERDATA=0x480, P_CREATEDBY=0x484, P_STATE=0x530, P_VEHICLE=0x58C, P_TYPE=0x598, P_STATS=0x59C,
                 P_WEAPONS=0x5A0, P_FIRE=0x730, P_LOOKTARGET=0x738, P_COVER=0x788, P_SCRIPTIDX=0x794, V_DRIVER=0x460, V_RADIO=0x1D2;
constexpr uintptr_t G_PEDPOOL=0xB74490, G_CHEATS=0x969130, G_MIAMI=0xC0FCB8, G_PEDSTATS=0xC0BBEC, G_GROUPS=0xC09928;
constexpr int16 MI_A=19990, MI_B=19991;

struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Name(const void* p){ return Name(uintptr_t(p)); }

// --- registro de stubs sin reservas de memoria ni E/S: tipo + valores crudos en un array fijo; el json (con los
//     nombres de punteros) se construye despues de que la funcion probada vuelve ---
struct I { int32 v; }; struct U { uint32 v; }; struct Fl { float v; }; struct Pt { const void* p; }; struct Bo { bool v; }; struct H { uint64 v; };
struct Arg { uint8 kind; uint32 v; };
struct Rec { const char* tag; uint32 n; std::array<Arg,16> a; };
inline std::array<Rec,512> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Put(Rec& r,uint8 k,uint32 v){ if(r.n<r.a.size())r.a[r.n++]={k,v}; }
inline void Add(Rec& r,I x){ Put(r,0,uint32(x.v)); }
inline void Add(Rec& r,U x){ Put(r,1,x.v); }
inline void Add(Rec& r,Fl x){ Put(r,2,std::bit_cast<uint32>(x.v)); }
inline void Add(Rec& r,Pt x){ Put(r,3,uint32(uintptr_t(x.p))); }
inline void Add(Rec& r,Bo x){ Put(r,4,x.v ? 1u : 0u); }
inline void Add(Rec& r,H x){ Put(r,6,uint32(x.v)); Put(r,7,uint32(x.v>>32)); }
template<class... A> void T(const char* tag,A... a){
    if(!Main())return; if(nrec>=recs.size()){ recOverflow=true; return; }
    Rec& r=recs[nrec++]; r.tag=tag; r.n=0; (Add(r,a),...); }
inline json RecsToJson(){
    json out=json::array();
    for(size_t i=0;i<nrec;++i){ const Rec& r=recs[i]; json j=json::array(); j.push_back(r.tag);
        for(uint32 k=0;k<r.n;++k){ const Arg& a=r.a[k];
            switch(a.kind){ case 0: j.push_back(int32(a.v)); break; case 1: case 2: j.push_back(a.v); break; case 3: j.push_back(Name(uintptr_t(a.v))); break;
                case 4: j.push_back(a.v!=0); break; case 6: { const uint32 hi=k+1<r.n ? r.a[k+1].v : 0; j.push_back(uint64(a.v)|(uint64(hi)<<32)); ++k; break; } default: j.push_back(a.v); } }
        out.push_back(std::move(j)); }
    if(recOverflow)out.push_back("overflow");
    return out; }

// --- estado de los dobles (se fija por caso) ---
inline uint32 baseSeed{}, loadSeed{}, loadCalls{}, nTaskNew{}, randIdx{};
inline std::array<uint32,16> randTab{};
inline bool followerRet{}, vehValidRet{}; inline uint32 pedFlagRet{}; inline float statRet{};
inline void* playerRet{};     // lo que devuelve FindPlayerPed

// --- dobles ---
// base: CPhysical::CPhysical / ~CPhysical
inline void* __fastcall S_PhysCtor(void* self,void*){ T("CPhysical::CPhysical",Pt{self}); Rng g{baseSeed|1u}; for(size_t k=0;k<0x138;k+=4)At<uint32>(self,k)=g.next(); return self; }
inline void __fastcall S_PhysDtor(void* self,void*){ T("CPhysical::~CPhysical",Pt{self}); }
// audio
inline void __fastcall S_WAInit(void* self,void*,void* ped){ T("CAEPedWeaponAudioEntity::Initialise",Pt{self},Pt{ped}); }
inline void __fastcall S_PAInit(void* self,void*,void* ped){ T("CAEPedAudioEntity::Initialise",Pt{self},Pt{ped}); }
inline void __fastcall S_WATerm(void* self,void*){ T("CAEPedWeaponAudioEntity::Terminate",Pt{self}); }
inline void __fastcall S_PATerm(void* self,void*){ T("CAEPedAudioEntity::Terminate",Pt{self}); }
inline void __fastcall S_SpeechTerm(void* self,void*){ T("CAEPedSpeechAudioEntity::Terminate",Pt{self}); }
template<int N> void* __fastcall SpTrap(void* v,void*) { T("speech_unexpected_slot",I{N},Pt{v}); return nullptr; }
template<int... N> void FillSpTraps(std::integer_sequence<int,N...>) { ((speechVt[N]=reinterpret_cast<void*>(&SpTrap<N>)),...); }
// constructor del peaton
inline void* __cdecl S_Acq(int32 type){ T("CPedType::GetPedTypeAcquaintances",I{type}); return acq.data(); }
inline int32 __fastcall S_Give(void* self,void*,int32 w,uint32 ammo,uint32 b){ T("CPed::GiveWeapon",Pt{self},I{w},U{ammo},U{b&0xFF}); return 0; }
inline void* __cdecl S_IntelNew(uint32 n){ T("CPedIntelligence::new",U{n}); return intel.data(); }
inline void* __fastcall S_IntelCtor(void* self,void*,void* ped){ T("CPedIntelligence::CPedIntelligence",Pt{self},Pt{ped}); return self; }
inline void __fastcall S_IntelDtor(void* self,void*){ T("CPedIntelligence::~CPedIntelligence",Pt{self}); }
inline void __cdecl S_IntelDelete(void* p){ T("CPedIntelligence::delete",Pt{p}); }
inline void* __cdecl S_TaskNew(uint32 n){ T("CTask::new",U{n}); return taskBuf[(nTaskNew++)&1].data(); }
inline void TaskRec(const char* tag,void* mgr,void* task,int32 idx,uint32 unused){
    const int32 type=task ? int32(reinterpret_cast<CTask*>(task)->GetTaskType()) : -1;   // el tipo en vez de la vptr
    const uint8* t=static_cast<const uint8*>(task);
    T(tag,Pt{mgr},Pt{task},I{idx},U{unused&0xFF},I{type},
      U{t ? *reinterpret_cast<const uint32*>(t+4) : 0u},U{t ? *reinterpret_cast<const uint32*>(t+8) : 0u},U{t ? *reinterpret_cast<const uint32*>(t+0xC) : 0u},
      U{t ? *reinterpret_cast<const uint32*>(t+0x10) : 0u},U{t ? *reinterpret_cast<const uint32*>(t+0x14) : 0u},U{t ? *reinterpret_cast<const uint32*>(t+0x18) : 0u},
      U{t ? *reinterpret_cast<const uint32*>(t+0x1C) : 0u}); }
inline void __fastcall S_SetTask(void* mgr,void*,void* task,int32 idx,uint32 unused){ TaskRec("CTaskManager::SetTask",mgr,task,idx,unused); }
inline void __fastcall S_SetTask2(void* mgr,void*,void* task,int32 idx){ TaskRec("CTaskManager::SetTaskSecondary",mgr,task,idx,0); }
inline void __cdecl S_PedCount(void* ped,uint32 b){ T("CPopulation::UpdatePedCount",Pt{ped},U{b&0xFF}); }
inline int __cdecl S_Rand(){ T("rand"); return int(randTab[(randIdx++)&15]); }
inline uint16 S_RandNum(){ return uint16(S_Rand()); }
inline CPlayerPed* __cdecl S_FindPlayer(int32 id){ T("FindPlayerPed",I{id}); return static_cast<CPlayerPed*>(playerRet); }
inline uint32 __cdecl S_PedFlag(int32 t){ T("CPedType::GetPedFlag",I{t}); return pedFlagRet; }
inline void __fastcall S_SetAcq(void* self,void*,int32 id,uint32 mask){ T("CAcquaintance::SetAsAcquaintance",Pt{self},I{id},U{mask}); }
inline void* __fastcall S_EventAdd(void* grp,void*,CEvent* ev,uint32 valid){ T("CEventGroup::Add",Pt{grp},I{int32(ev->GetEventType())},U{valid&0xFF},Pt{*reinterpret_cast<void**>(reinterpret_cast<uint8*>(ev)+0x14)}); return nullptr; }
inline void __fastcall S_RegRef(void* self,void*,void* field){ T("RegisterReference",Pt{self},Pt{field}); }
inline void __fastcall S_CleanRef(void* self,void*,void* field){ T("CleanUpOldReference",Pt{self},Pt{field}); }
// destructor del peaton
inline void __cdecl S_RecDeleted(void* ped){ T("CReplay::RecordPedDeleted",Pt{ped}); }
inline void __cdecl S_MissionNoReq(int32 m){ T("CStreaming::SetMissionDoesntRequireModel",I{m}); }
inline void __cdecl S_RemoveWait(void* ent,int32 raw){ T("CTheScripts::RemoveFromWaitingForScriptBrainArray",Pt{ent},U{uint32(raw)&0xFFFFu}); }   // el EXE empuja el indice con cero extendido, la DLL con signo: solo cuentan los 16 bits bajos (la funcion lee un word)
inline void __cdecl S_WorldRemove(void* e){ T("CWorld::Remove",Pt{e}); }
inline void __cdecl S_ClearBlip(int32 type,int32 ref){ T("CRadar::ClearBlipForEntity",I{type},I{ref}); }
inline void __cdecl S_RemoveConv(void* ped){ T("CConversations::RemoveConversationForPed",Pt{ped}); }
inline void __fastcall S_Extinguish(void* fire,void*){ T("CFire::Extinguish",Pt{fire}); }
inline void __fastcall S_ReleaseCover(void* cover,void*,void* ped){ T("CCoverPoint::ReleaseCoverPointForPed",Pt{cover},Pt{ped}); }
inline void __fastcall S_ClearWeapons(void* ped,void*){ T("CPed::ClearWeapons",Pt{ped}); }
// FlagToDestroyWhenNextProcessed
inline bool __cdecl S_VehValid(void* v){ T("IsVehiclePointerValid",Pt{v},Bo{vehValidRet}); return vehValidRet; }
inline void __fastcall S_RemovePassenger(void* v,void*,void* ped){ T("CVehicle::RemovePassenger",Pt{v},Pt{ped}); }
// otros
inline bool __fastcall S_IsFollower(void* grp,void*,void* ped){ T("CPedGroupMembership::IsFollower",Pt{grp},Pt{ped},Bo{followerRet}); return followerRet; }
inline void __fastcall S_SetDM(void* intelligence,void*,int32 t){ T("CPedIntelligence::SetPedDecisionMakerType",Pt{intelligence},I{t}); }
inline float __cdecl S_StatValue(int32 s){ T("CStats::GetStatValue",I{s},Fl{statRet}); return statRet; }
inline void __cdecl S_PedTypeInit(){ T("CPedType::Initialise"); }
inline void __cdecl S_AnimOffset(){ T("CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle"); }
// guardado
inline void __fastcall S_Construct(void* save,void*,void* ped){ T("CPedSaveStructure::Construct",Pt{ped}); Rng g{baseSeed^0x5A5A5A5Au}; if(!g.s)g.s=1; for(size_t k=0;k<0x18C;k+=4)At<uint32>(save,k)=g.next(); }
inline bool __cdecl S_SaveData(void* d,int32 n){ T("SaveDataToWorkBuffer",I{n},H{Fnv(d,n>0&&n<=0x400 ? size_t(n) : 0)}); return true; }
inline bool __cdecl S_LoadData(void* d,int32 n){ T("LoadDataFromWorkBuffer",I{n}); Rng g{(loadSeed+977u*(loadCalls++))|1u}; if(n>0&&n<=0x400)for(int32 k=0;k<n;++k)At<uint8>(d,size_t(k))=uint8(g.next()>>7); return true; }
inline void __fastcall S_Extract(void* save,void*,void* ped){ T("CPedSaveStructure::Extract",Pt{ped},H{Fnv(save,0x18C)}); }

// ======================= fixtures =======================
inline CPed* PedA(){ return reinterpret_cast<CPed*>(Slot(SA)); }
inline void RandFill(uint8* p,size_t n,Rng& r){ for(size_t k=0;k<n;++k)p[k]=uint8(r.next()>>5); }
inline int32 StateOf(int i,Rng& r){ return i<128 ? i-24 : (r.next()%3 ? int32(r.next()%90)-10 : int32(r.next())); }

// fixture comun: pool falso, "jugador", peatones, vehiculos, inteligencia, tareas y dobles
inline void Reset(Rng& r){
    nrec=0; recOverflow=false; randIdx=0; nTaskNew=0; loadCalls=0; baseSeed=r.next(); loadSeed=r.next();
    for(auto& t:taskBuf)std::memset(t.data(),0xCD,t.size());
    RandFill(Slot(SA),SLOT,r); RandFill(Slot(SB),SLOT,r); RandFill(Slot(SPL),SLOT,r);
    RandFill(veh.data(),veh.size(),r); RandFill(veh2.data(),veh2.size(),r); RandFill(intel.data(),intel.size(),r);
    RandFill(playerData.data(),playerData.size(),r); RandFill(statBuf.data(),statBuf.size(),r); RandFill(acq.data(),acq.size(),r);
    RandFill(mi.data(),mi.size(),r); RandFill(mi2.data(),mi2.size(),r);
    At<uint32>(playerData.data(),0x38)=r.next()%8;                 // grupo del jugador (0..7)
    At<void*>(Slot(SPL),P_PLAYERDATA)=playerData.data();
    // pool falso: almacenamiento, estados (el array empieza en +0x100 de slotRaw), capacidad 140
    for(auto& b:slotRaw)b=uint8(r.next()>>9);
    At<void*>(fakePool.data(),0)=pedStore.data(); At<void*>(fakePool.data(),4)=StateBase(); At<uint32>(fakePool.data(),8)=uint32(NSLOT);
    At<int32>(fakePool.data(),0xC)=-1; fakePool[0x10]=0; fakePool[0x11]=0;
    followerRet=r.next()%2; vehValidRet=r.next()%2; pedFlagRet=r.next(); playerRet=Slot(SPL);
    { static const float stats[]{0.f,-0.f,1000.f,999.99994f,1000.0001f,500.f,2000.f,1e9f,-5.f,1.f,0.001f,std::bit_cast<float>(0x7FC00000u)}; statRet=r.next()%3 ? stats[r.next()%std::size(stats)] : r.uni(0.f,2500.f); }
    for(auto& v:randTab){ static const uint32 sp[]{0x3FFF,0x3FFE,0x4000,0,1,2,3,4,0x7FFF,0x3FFC,0x4003}; v=r.next()%3 ? sp[r.next()%std::size(sp)] : r.next()&0x7FFF; }
}

// palabras del peaton: sin la vptr, sin los subobjetos de audio; masks=true aplica las del constructor
inline json PedWords(const uint8* p,bool masks){
    json r=json::array();
    for(size_t k=4;k<PED;k+=4){
        if(k>=AUD0&&k<AUD1)continue;
        uint32 w=*reinterpret_cast<const uint32*>(p+k);
        if(masks){
            if(k==P_FLAGS4)w&=0x003FFFFFu;                                       // bits 118..127 de los flags: relleno sin nombre en la DLL (el EXE conserva lo que habia)
            if(k>=P_WEAPONS&&k<P_WEAPONS+13*0x1C&&((k-P_WEAPONS)%0x1C)==0x14)w&=0xFFFFu; // relleno entre los dos bytes y el puntero del arma
        }
        r.push_back(w); }
    return r; }
inline json AudioWords(const uint8* p){ json r=json::array(); for(size_t k=AUD0;k<AUD1;k+=4)r.push_back(*reinterpret_cast<const uint32*>(p+k)); return r; }
inline bool ExeVt(uint32 v){ return v>=0x850000u&&v<0x880000u; }
inline bool DllVt(uint32 v){ return InDll(v); }
// compara los subobjetos de audio: vptr (0x138/0x1E0/0x294/0x394) por tipo (EXE frente a DLL); con lenient los
// bytes que el EXE deja sin escribir (0xCD) pueden valer cualquier cosa (valor-inicializacion de los miembros de audio:
// la DLL construye CAEPedAudioEntity/CAEPedSpeechAudioEntity/CAEPedWeaponAudioEntity con sus constructores C++, el EXE
// solo escribe unos pocos campos). Devuelve las diferencias (maximo 24) en `diffs`; extra cuenta los bytes de mas.
inline bool AudioEqual(const json& o,const json& c,bool lenient,int& extra,json& diffs){
    diffs=json::array(); if(o.size()!=c.size()){ diffs.push_back("size"); return false; }
    for(size_t k=0;k<o.size();++k){
        const uint32 ov=o[k].get<uint32>(), cv=c[k].get<uint32>(); const size_t off=AUD0+4*k;
        const bool vslot=off==0x138||off==0x1E0||off==0x294||off==0x394;
        if(lenient&&ov==0xCDCDCDCDu){ if(cv!=ov)++extra; continue; }
        if(vslot){ if(!(ExeVt(ov)&&(DllVt(cv)||ExeVt(cv)))&&diffs.size()<24)diffs.push_back({{"offset",off},{"orig",ov},{"cand",cv},{"kind","vptr"}}); continue; }
        if(ov!=cv&&diffs.size()<24)diffs.push_back({{"offset",off},{"orig",ov},{"cand",cv}}); }
    return diffs.empty(); }

inline json Globals(){
    return {*reinterpret_cast<uint32*>(G_MIAMI),*reinterpret_cast<uint32*>(G_PEDSTATS),*reinterpret_cast<uint8*>(G_CHEATS+15),*reinterpret_cast<uint32*>(G_PEDPOOL)!=0}; }

// ======================= ejecucion de un caso =======================
inline json Run(const std::string& nm,uintptr_t target,Rng& r,int i,json& out,bool candidate){
    Reset(r);
    uint8* const pa=Slot(SA); json ret=nullptr; bool pedMasks=false, auditPed=true;
    void*& poolPtr=*reinterpret_cast<void**>(G_PEDPOOL);
    auto withPool=[&](auto&& fn){ void* const saved=poolPtr; poolPtr=fakePool.data(); fn(); poolPtr=saved; };
    auto U32=[&](size_t off)->uint32&{ return At<uint32>(pa,off); };
    // bytes de guarda (tras el CPed dentro del hueco): 0xCD, no deben cambiar
    std::memset(pa+PED,0xCD,SLOT-PED);
    if(nm=="Constructor"){
        pedMasks=true; std::memset(pa,0xCD,SLOT);
        { static const uint32 types[]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,0xFFFFFFFFu,200u}; const uint32 type=r.next()%3==0 ? uint32(r.next()%2) : types[r.next()%std::size(types)];
          *reinterpret_cast<uint8*>(G_CHEATS+15)=uint8(r.next()%3==0);        // CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD
          auto* p=reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(pa,type); ret=Name(p); out["type"]=type; } }
    else if(nm=="Destructor"){
        RandFill(pa,PED,r); std::memset(pa+AUD0,0,AUD1-AUD0); At<void*>(pa,0x294)=speechVt.data();
        { auto& flags2=At<uint32>(pa,P_FLAGS2); flags2=(flags2&~0x10000u)|(r.next()%2 ? 0x10000u : 0u); }          // bMiamiViceCop (bit 16 de +0x470)
        { auto& flags3=At<uint32>(pa,P_FLAGS3); flags3=(flags3&~0x01002000u)|(r.next()%2 ? 0x2000u : 0u)|(r.next()%2 ? 0x01000000u : 0u); }   // bClearRadarBlipOnDeath y bWaitingForScriptBrainToLoad
        const bool waiting=(At<uint32>(pa,P_FLAGS3)&0x01000000u)!=0;
        At<int16>(pa,P_SCRIPTIDX)=waiting||r.next()%2 ? int16(r.next()%70) : int16(r.next());
        At<void*>(pa,P_VEHICLE)=r.next()%3 ? static_cast<void*>(veh.data()) : nullptr;
        At<void*>(pa,P_FIRE)=r.next()%2 ? reinterpret_cast<void*>(0x7700u+(r.next()&0xFF)*16u) : nullptr;
        At<void*>(pa,P_COVER)=r.next()%2 ? reinterpret_cast<void*>(0x7800u+(r.next()&0xFF)*16u) : nullptr;
        At<void*>(pa,P_INTEL)=r.next()%8 ? static_cast<void*>(intel.data()) : nullptr;
        At<void*>(pa,P_LOOKTARGET)=r.next()%3 ? reinterpret_cast<void*>(0x7900u+(r.next()&0xFF)*16u) : nullptr;
        *reinterpret_cast<uint32*>(G_MIAMI)=r.next()%12;
        for(size_t k=0;k<CTheScripts::ScriptsForBrains.m_aScriptForBrains.size();++k)CTheScripts::ScriptsForBrains.m_aScriptForBrains[k].m_StreamedScriptIndex=int16(r.next()%4==0 ? -1 : int32(r.next()%3000));
        out["miami_before"]=*reinterpret_cast<uint32*>(G_MIAMI);
        withPool([&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(pa); }); }
    else if(nm=="Initialise"){ reinterpret_cast<void(__cdecl*)()>(target)(); }
    else if(nm=="Update"||nm=="UpdateStatEnteringVehicle"||nm=="UpdateStatLeavingVehicle"){ reinterpret_cast<void(__thiscall*)(void*)>(target)(pa); }
    else if(nm=="Save"||nm=="Load"){ ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(pa)&0xFF; }
    else if(nm=="FlagToDestroyWhenNextProcessed"){
        // flags: bInVehicle (bit 8 de +0x46C) casi siempre; bClearRadarBlipOnDeath (bit 13 de +0x474)
        { auto& f1=At<uint32>(pa,P_FLAGS1); f1=(f1&~0x100u)|(r.next()%5 ? 0x100u : 0u); }
        { auto& f3=At<uint32>(pa,P_FLAGS3); f3=(f3&~0x2000u)|(r.next()%2 ? 0x2000u : 0u); }
        At<uint32>(pa,P_ENTFLAGS)=r.next();
        { static const uint32 types[]{0,1,2,3,5,9,32}; U32(P_TYPE)=types[r.next()%std::size(types)]; }
        pa[P_CREATEDBY]=uint8(r.next()%4==0 ? r.next() : r.next()%4);
        U32(P_STATE)=uint32(StateOf(int(r.next()%130),r));
        At<void*>(pa,P_COVER)=r.next()%2 ? reinterpret_cast<void*>(0x7800u+(r.next()&0xFF)*16u) : nullptr;
        At<void*>(pa,P_VEHICLE)=r.next()%7 ? static_cast<void*>(veh.data()) : nullptr;
        { const uint32 d=r.next()%4; At<void*>(veh.data(),V_DRIVER)=d<2 ? static_cast<void*>(pa) : d==2 ? static_cast<void*>(Slot(SB)) : nullptr; }
        { static const uint8 st[]{0,1,2,3,4,5,6,8,10,31}; veh[P_STATUS]=uint8((r.next()&7)|(st[r.next()%std::size(st)]<<3)); }
        withPool([&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(pa); }); }
    else if(nm=="CanBeArrested"||nm=="CanPedReturnToState"||nm=="CanSetPedState"||nm=="CanStrafeOrMouseControl"||nm=="IsAlive"||nm=="IsPedShootable"){
        U32(P_STATE)=uint32(StateOf(i,r));
        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(pa)&0xFF; }
    else if(nm=="IsPedInControl"){
        U32(P_STATE)=uint32(StateOf(i,r)); { auto& f1=At<uint32>(pa,P_FLAGS1); if(r.next()%2)f1&=~0x600u; }
        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(pa)&0xFF; }
    else if(nm=="IsPlayer"){
        U32(P_TYPE)=i<64 ? uint32(i) : r.next()%2 ? uint32(r.next()%40) : r.next();
        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(pa)&0xFF; }
    else if(nm=="CanBeDeletedEvenInVehicle"){
        pa[P_CREATEDBY]=uint8(i<256 ? i : r.next()); ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(pa)&0xFF; }
    else if(nm=="CanBeDeleted"){
        pa[P_CREATEDBY]=uint8(r.next()%2 ? r.next()%6 : r.next()); { auto& f1=At<uint32>(pa,P_FLAGS1); f1=(f1&~0x100u)|(r.next()%3==0 ? 0x100u : 0u); }
        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(pa)&0xFF; }
    else if(nm=="IsPointerValid"){
        // puntero dentro del almacenamiento (con desplazamiento dentro del hueco), fuera del rango del indice o por debajo del inicio
        int32 k; size_t off; const uint32 mode=r.next()%8;
        if(mode<5){ k=int32(r.next()%NSLOT); off=r.next()%3==0 ? 0 : r.next()%SLOT; }
        else if(mode<7){ static const int32 ks[]{139,140,141,145,160,255,-2,-3,-100}; k=ks[r.next()%std::size(ks)]; off=r.next()%SLOT; }
        else { k=r.next()%2 ? -1 : 0; off=0; }      // -1 con desplazamiento 0 -> indice -1; el desplazamiento no nulo daria indice 0 con el puntero fuera del almacenamiento
        void* const self=pedStore.data()+ptrdiff_t(k)*ptrdiff_t(SLOT)+ptrdiff_t(off);
        const bool inside=k>=0&&k<int32(NSLOT);
        if(inside)At<uint32>(self,P_COLLIST)=r.next()%2 ? r.next()|1u : 0u;      // lista de colision vacia o no (el EXE lee [puntero+0xB0])
        playerRet=inside&&r.next()%2 ? self : Slot(SPL);
        out["k"]=k; out["off"]=off;
        withPool([&]{ ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(self)&0xFF; });
        auditPed=false; }
    else if(nm=="SetPedState"){
        { auto& f3=At<uint32>(pa,P_FLAGS3); f3=(f3&~0x2000u)|(r.next()%2 ? 0x2000u : 0u); }
        At<void*>(pa,P_COVER)=r.next()%2 ? reinterpret_cast<void*>(0x7800u+(r.next()&0xFF)*16u) : nullptr;
        const int32 st=StateOf(i,r); out["state"]=st;
        withPool([&]{ reinterpret_cast<void(__thiscall*)(void*,int32)>(target)(pa,st); }); }
    else if(nm=="SetPedStats"){
        const int32 t=i<64 ? i-8 : r.next()%2 ? int32(r.next()%80)-10 : int32(r.next());
        *reinterpret_cast<uint32*>(G_PEDSTATS)=0x30000000u+(r.next()&0x0FFFFFFCu); out["type"]=t;
        reinterpret_cast<void(__thiscall*)(void*,int32)>(target)(pa,t); }
    else if(nm=="SetPedDefaultDecisionMaker"){
        { static const uint32 types[]{0,1,2,3,5,9,32,0xFFFFFFFFu}; U32(P_TYPE)=types[r.next()%std::size(types)]; }
        pa[P_CREATEDBY]=uint8(r.next()%3==0 ? r.next() : r.next()%4);
        At<void*>(pa,P_STATS)=statBuf.data(); At<void*>(pa,P_INTEL)=intel.data();
        reinterpret_cast<void(__thiscall*)(void*)>(target)(pa); }
    else if(nm=="SetStayInSamePlace"){
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(pa,uint32(i&1)); }
    else if(nm=="GetBikeRidingSkill"){
        At<void*>(pa,P_PLAYERDATA)=r.next()%2 ? reinterpret_cast<void*>(0x1230u+(r.next()&0xFF)*16u) : nullptr;
        pa[P_CREATEDBY]=uint8(r.next()%2 ? r.next()%4 : r.next());
        const float v=reinterpret_cast<float(__thiscall*)(void*)>(target)(pa); ret=F(v); }
    else if(nm=="SetRadioStation"){
        At<int16>(pa,P_MODEL)=r.next()%2 ? MI_A : MI_B;
        { static const uint32 types[]{0,1,2,3,5,9,32}; U32(P_TYPE)=types[r.next()%std::size(types)]; }
        At<void*>(pa,P_VEHICLE)=r.next()%5 ? static_cast<void*>(veh.data()) : nullptr;
        { const uint32 d=r.next()%4; At<void*>(veh.data(),V_DRIVER)=d<2 ? static_cast<void*>(pa) : d==2 ? static_cast<void*>(Slot(SB)) : nullptr; }
        reinterpret_cast<void(__thiscall*)(void*)>(target)(pa); }
    // ---- recogida ----
    const uint32 vptr=At<uint32>(pa,0);
    if(nm=="Constructor"||nm=="Destructor")out["vptr"]=(candidate ? InDll(vptr) : vptr==0x86C358u) ? json("CPed") : json(vptr);
    if(auditPed){
        out["ped"]=PedWords(pa,pedMasks); out["audio"]=AudioWords(pa);
        if(nm!="Constructor"&&nm!="Destructor")out["vptr_untouched"]=vptr; }
    out["tail"]=Fnv(pa+PED,SLOT-PED);
    out["others"]={Fnv(Slot(SB),SLOT),Fnv(Slot(SPL),SLOT),Fnv(veh.data(),veh.size()),Fnv(veh2.data(),veh2.size()),Fnv(intel.data(),intel.size()),Fnv(playerData.data(),playerData.size()),
                   Fnv(taskBuf[0].data()+4,0x3C),Fnv(taskBuf[1].data()+4,0x3C),   /* sin la vptr de la tarea (constructor del EXE o de la DLL) */Fnv(statBuf.data(),statBuf.size()),Fnv(acq.data(),acq.size())};
    out["globals"]=Globals();
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=26;
inline const std::array<Fn,NFN> fns{{
    {"CPed","Constructor",0x5E8030,0,-1,512},{"CPed","Destructor",0x5E8620,0,-1,512},{"CPed","Initialise",0x5DEBB0,0,-1,16},{"CPed","Update",0x5DEBE0,0,-1,16},
    {"CPed","Save",0,0x86C358,24,128},{"CPed","Load",0,0x86C358,25,128},{"CPed","FlagToDestroyWhenNextProcessed",0,0x86C358,21,1024},
    {"CPed","CanBeArrested",0x5DF060,0,-1,256},{"CPed","CanBeDeleted",0x5DF100,0,-1,512},{"CPed","CanBeDeletedEvenInVehicle",0x5DF150,0,-1,256},
    {"CPed","CanPedReturnToState",0x5DF000,0,-1,256},{"CPed","CanSetPedState",0x5DF030,0,-1,256},{"CPed","CanStrafeOrMouseControl",0x5DF090,0,-1,256},
    {"CPed","IsAlive",0x5E0170,0,-1,256},{"CPed","IsPedInControl",0x5E3960,0,-1,512},{"CPed","IsPedShootable",0x5DEFD0,0,-1,256},{"CPed","IsPlayer",0x5DF8F0,0,-1,128},
    {"CPed","IsPointerValid",0x5E4220,0,-1,1024},{"CPed","SetPedState",0x5E4500,0,-1,512},{"CPed","SetPedStats",0x5DEBC0,0,-1,128},
    {"CPed","SetPedDefaultDecisionMaker",0x5E06E0,0,-1,256},{"CPed","SetStayInSamePlace",0x481090,0,-1,128},{"CPed","SetRadioStation",0x5DFD90,0,-1,512},
    {"CPed","GetBikeRidingSkill",0x5DF510,0,-1,512},{"CPed","UpdateStatEnteringVehicle",0x5E01A0,0,-1,16},{"CPed","UpdateStatLeavingVehicle",0x5E01B0,0,-1,16},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpedd-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpedd-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpedd");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"cpedd-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // cuerpos de la DLL: base CPhysical (constructor y destructor), CPedIntelligence (constructor y destructor), voz (virtual)
    uintptr_t dllPhysCtor{}, dllPhysDtor{}, dllIntelCtor{}, dllIntelDtor{}, dllSpeechTerm{};
    { const uintptr_t pedCtor=FirstDllCall(DllBody("CPed","Constructor")); const uintptr_t a=pedCtor ? FirstDllCall(pedCtor) : 0; const uintptr_t b=FirstDllCall(DllBody("CPhysical","Constructor"));
      if(!a||a!=b)Write({{"event","warning"},{"reason","CPhysical constructor not found"},{"from_ped",a},{"from_wrapper",b}}); else dllPhysCtor=a;
      dllPhysDtor=FirstDllCall(DllBody("CPhysical","Destructor")); if(!dllPhysDtor)Write({{"event","warning"},{"reason","CPhysical destructor not found"}});
      dllIntelCtor=FirstDllCall(DllBody("CPedIntelligence","Constructor")); if(!dllIntelCtor)Write({{"event","warning"},{"reason","CPedIntelligence constructor not found"}});
      dllIntelDtor=FirstDllCall(DllBody("CPedIntelligence","Destructor")); if(!dllIntelDtor)Write({{"event","warning"},{"reason","CPedIntelligence destructor not found"}});
      dllSpeechTerm=DllBody("CAEPedSpeechAudioEntity","Terminate"); if(!dllSpeechTerm)Write({{"event","warning"},{"reason","speech Terminate body not found"}});
      Write({{"event","info"},{"dll_phys_ctor",dllPhysCtor ? dllPhysCtor-modBase : 0},{"dll_phys_dtor",dllPhysDtor ? dllPhysDtor-modBase : 0},{"dll_intel_ctor",dllIntelCtor ? dllIntelCtor-modBase : 0},
             {"dll_intel_dtor",dllIntelDtor ? dllIntelDtor-modBase : 0},{"dll_speech_terminate",dllSpeechTerm ? dllSpeechTerm-modBase : 0}}); }
    const bool bodiesOk=dllPhysCtor&&dllPhysDtor&&dllIntelCtor&&dllIntelDtor&&dllSpeechTerm;

    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto* p=PedA(); auto Off=[&](const void* m){ return uint32(static_cast<const uint8*>(m)-reinterpret_cast<const uint8*>(p)); }; json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      Chk("m_pCollisionList",Off(&p->m_pCollisionList),0xB0); Chk("m_pedAudio",Off(&p->GetAE()),0x138); Chk("m_pedSpeech",Off(&p->GetSpeechAE()),0x294); Chk("m_weaponAudio",Off(&p->GetWeaponAE()),0x394);
      Chk("m_roadRageWith",Off(&p->m_roadRageWith),0x460); Chk("m_nPedState",Off(&p->m_nPedState),0x530); Chk("m_pVehicle",Off(&p->m_pVehicle),0x58C); Chk("m_nPedType",Off(&p->m_nPedType),0x598);
      Chk("m_pStats",Off(&p->m_pStats),0x59C); Chk("m_aWeapons",Off(p->m_aWeapons.data()),0x5A0); Chk("m_pFire",Off(&p->m_pFire),0x730); Chk("m_pLookTarget",Off(&p->m_pLookTarget),0x738);
      Chk("m_pCoverPoint",Off(&p->m_pCoverPoint),0x788); Chk("m_StreamedScriptBrainToLoad",Off(&p->m_StreamedScriptBrainToLoad),0x794); Chk("m_pedIK",Off(&p->m_pedIK),0x50C);
      Chk("m_nWeaponShootingRate",Off(&p->m_nWeaponShootingRate),0x719); Chk("m_nBodypartToRemove",Off(&p->m_nBodypartToRemove),0x754);
      Chk("m_fHealth",Off(&p->m_fHealth),0x540); Chk("m_pLookTarget",Off(&p->m_pLookTarget),0x738); Chk("field_588",Off(&p->field_588),0x588);
      Write({{"event","layout"},{"layout_ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    regions.clear();
    Snap(G_PEDPOOL,4); Snap(G_CHEATS,0x80); Snap(G_MIAMI,4); Snap(G_PEDSTATS,4); Snap(uintptr_t(&CTheScripts::ScriptsForBrains),sizeof(CScriptsForBrains));
    Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[MI_A]),4); Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[MI_B]),4);

    FillSpTraps(std::make_integer_sequence<int,16>{}); speechVt[2]=reinterpret_cast<void*>(&S_SpeechTerm);
    named={{Slot(SA),SLOT,"pedA"},{Slot(SB),SLOT,"pedB"},{Slot(SPL),SLOT,"player"},{pedStore.data(),pedStore.size(),"ped_store"},{veh.data(),veh.size(),"veh"},{veh2.data(),veh2.size(),"veh2"},
           {intel.data(),intel.size(),"intel"},{taskBuf[0].data(),0x40,"task0"},{taskBuf[1].data(),0x40,"task1"},{playerData.data(),playerData.size(),"player_data"},{statBuf.data(),statBuf.size(),"stat"},
           {acq.data(),acq.size(),"acq"},{mi.data(),mi.size(),"mi"},{mi2.data(),mi2.size(),"mi2"},{fakePool.data(),fakePool.size(),"fake_pool"},{speechVt.data(),sizeof(speechVt),"speech_vt"},
           {reinterpret_cast<void*>(G_GROUPS),8*0x2D4,"groups"}};

    bool passed=true; int pair=0;
    {
        using std::bit_cast;
        Patches p;
        // Cuerpo de la DLL resuelto ANTES de escribir parches (ver Boat23): nunca se parchea un stub ya registrado.
        std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        if(bodiesOk){
            P(0x542260,dllPhysCtor,S_PhysCtor); P(0x542450,dllPhysDtor,S_PhysDtor);
            P(0x607140,dllIntelCtor,S_IntelCtor); P(0x607300,dllIntelDtor,S_IntelDtor);
            P(0,dllSpeechTerm,S_SpeechTerm); }
        P(0x4E6AA0,Addr(&CAEPedWeaponAudioEntity::Initialise),S_WAInit); P(0x4E0E80,Addr(&CAEPedAudioEntity::Initialise),S_PAInit);
        P(0x4E6AC0,Addr(&CAEPedWeaponAudioEntity::Terminate),S_WATerm); P(0x4E1360,Addr(&CAEPedAudioEntity::Terminate),S_PATerm);
        P(0x6089B0,Addr(static_cast<CAcquaintance&(*)(ePedType)>(&CPedType::GetPedTypeAcquaintances)),S_Acq);
        P(0x5E6080,Addr(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),S_Give);
        P(0x6074A0,Addr(static_cast<void*(*)(unsigned)>(&CPedIntelligence::operator new)),S_IntelNew);
        P(0x6074E0,Addr(static_cast<void(*)(void*)>(&CPedIntelligence::operator delete)),S_IntelDelete);
        P(0x61A5A0,Addr(static_cast<void*(*)(size_t)>(&CTask::operator new)),S_TaskNew);
        P(0x681AF0,Addr(&CTaskManager::SetTask),S_SetTask); P(0x681B60,Addr(&CTaskManager::SetTaskSecondary),S_SetTask2);
        P(0x611570,Addr(&CPopulation::UpdatePedCount),S_PedCount);
        P(0x821B1E,0,S_Rand); P(0,Addr(&CGeneral::GetRandomNumber),S_RandNum);
        P(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),S_FindPlayer);
        P(0x608830,Addr(&CPedType::GetPedFlag),S_PedFlag);
        P(0x608DA0,Addr(&CAcquaintance::SetAsAcquaintance),S_SetAcq);
        P(0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),S_EventAdd);
        P(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),S_RegRef);
        P(0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),S_CleanRef);
        P(0x45EC20,Addr(&CReplay::RecordPedDeleted),S_RecDeleted);
        P(0x409C90,Addr(&CStreaming::SetMissionDoesntRequireModel),S_MissionNoReq);
        P(0x46ABC0,Addr(&CTheScripts::RemoveFromWaitingForScriptBrainArray),S_RemoveWait);
        P(0x563280,Addr(&CWorld::Remove),S_WorldRemove);
        P(0x587C60,Addr(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity)),S_ClearBlip);
        P(0x43A960,Addr(&CConversations::RemoveConversationForPed),S_RemoveConv);
        P(0x5393F0,Addr(&CFire::Extinguish),S_Extinguish);
        P(0x698EF0,Addr(&CCoverPoint::ReleaseCoverPointForPed),S_ReleaseCover);
        P(0x5E6320,Addr(&CPed::ClearWeapons),S_ClearWeapons);
        P(0x6E38F0,Addr(&IsVehiclePointerValid),S_VehValid);
        P(0x6D1610,Addr(&CVehicle::RemovePassenger),S_RemovePassenger);
        P(0x5F69E0,Addr(&CPedGroupMembership::IsFollower),S_IsFollower);
        P(0x600B50,Addr(&CPedIntelligence::SetPedDecisionMakerType),S_SetDM);
        P(0x558E40,Addr(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),S_StatValue);
        P(0x608E40,Addr(&CPedType::Initialise),S_PedTypeInit);
        P(0x64F860,Addr(&CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle),S_AnimOffset);
        P(0x5D43D0,Addr(&CPedSaveStructure::Construct),S_Construct); P(0x5D44B0,Addr(&CPedSaveStructure::Extract),S_Extract);
        P(0x5D1270,Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),S_SaveData);
        P(0x5D1300,Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),S_LoadData);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(!bodiesOk&&(nm=="Constructor"||nm=="Destructor")){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","DLL base bodies not found"}}); continue; }
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC9ED0Du}; if(!r.s)r.s=1;
                    RestoreRegions();
                    for(auto* h:hooks)h->State(candidate);
                    for(auto& e:p.v)e->Enable(true); // los parches de entrada se reafirman despues de fijar los hooks
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    json out; out["switches"]=fn.slot>=0 ? ((target<0x400000||target>=0x1000000)==candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    // model infos falsos para SetRadioStation (se restauran con las regiones)
                    CModelInfo::ms_modelInfoPtrs[MI_A]=reinterpret_cast<CBaseModelInfo*>(mi.data()); CModelInfo::ms_modelInfoPtrs[MI_B]=reinterpret_cast<CBaseModelInfo*>(mi2.data());
                    json ret=Run(nm,target,r,i,out,candidate);
                    trace=RecsToJson(); // el registro de los stubs se convierte a json fuera de la llamada
                    RestoreRegions();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                json original=run(false), candidate=run(true);
                // los subobjetos de audio se comparan aparte (vptr por tipo y, en el constructor, solo lo que escribe el EXE)
                // En el constructor la diferencia de audio es solo informativa (ver README: los miembros de audio de CPed se
                // construyen con los constructores C++ de la DLL); en el destructor es estricta salvo las vptr.
                int extra=0; json why; bool audioOk=true;
                if(original.contains("audio")&&candidate.contains("audio")){ audioOk=AudioEqual(original["audio"],candidate["audio"],nm=="Constructor",extra,why); original.erase("audio"); candidate.erase("audio"); }
                const bool audioCounts=nm=="Destructor" ? audioOk : true;
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&audioCounts;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()},{"audio_extra_writes",extra},{"audio_diffs",why}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k); if(!audioCounts)diff.push_back("audio");
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"audio_diffs",why},{"audio_extra_writes",extra},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()},{"layout_ok",layoutOk}});
}
}
