#pragma once
// Banco dentro del motor: CVehicle, grupo B (ocupantes, ciclo de vida y geometria; 32 funciones).
// Original (hooks del grupo desactivados) frente a candidato (hooks activos) con fixtures identicos sobre un CVehicle FALSO
// (bufer de 0xA18 bytes = sizeof(CHeli) dentro de un almacenamiento de pool de vehiculos FALSO) y peatones FALSOS (bufer de
// 0x7C4 bytes dentro de un pool de peatones FALSO de 6 huecos): nunca vehiculos, peatones ni pools reales. Los punteros que el
// EXE desreferencia apuntan a objetos falsos (matrices, model info, colmodel, inteligencia del peaton, tareas, clump, fuego...);
// el resto del bufer se rellena con bytes deterministas por caso. Los pools reales (0xB74490 peatones, 0xB74494 vehiculos) solo
// se sustituyen durante la llamada probada y se restauran.
// Las funciones virtuales (SetModelIndex, DeleteRwObject, GetHeightAboveRoad, GetTowHitchPos, GetTowBarPos, Save, Load) se
// llaman por la tabla virtual del EXE de CVehicle (0x871E80, huecos 5, 8, 53, 59, 60, 64, 65); las llamadas virtuales que hace el
// codigo probado sobre el propio vehiculo (BlowUpCar, hueco 41; destructor virtual, hueco 0) o sobre los peatones falsos (destructor
// virtual, hueco 0; FlagToDestroyWhenNextProcessed, hueco 21) van por tablas virtuales falsas con trazas (el resto de huecos son
// trampas que anotan "virtual_inesperada").
// Se simulan y trazan (direccion del EXE + simbolo/cuerpo de la DLL, sin reservar memoria dentro de los stubs):
//   comunes: RegisterReference / CleanUpOldReference, rand (0x821B1E) + CGeneral::GetRandomNumber (valores 0..0x7FFF como rand),
//     FindPlayerPed, CGameLogic::IsCoopGameGoingOn, CDarkel::RegisterKillByPlayer / RegisterCarBlownUpByPlayer, CWorld::Remove;
//   ocupantes: CPlayerInfo::AddHealth, CStreaming::RequestModel / SetModelIsDeletable, CStats::IncrementStat / SetStatValue,
//     CPlayerPed::DoesPlayerWantNewWeapon, CPed::GiveWeapon / GrantAmmo, CPopulation::AddPedInCar / AddPed / RemovePed,
//     CCarEnterExit::SetPedInCarDirect / ComputeTargetDoorToEnterAsPassenger, CPedClothesDesc::SetModel, CClothes::RebuildPlayer,
//     IKChainManager_c::LookAt, CPhysical::ApplyMoveForce / ApplyTurnForce, CGarages::TriggerMessage, CTaskManager::
//     FindActiveTaskByType, CEventGroup::Add, CPedDamageResponseCalculator::ComputeDamageResponse, CEventDamage::AffectsPed
//     (los constructores/destructores de los eventos y del calculador se ejecutan de verdad en ambos lados);
//   ciclo de vida: constructor (bases CPhysical, CAEVehicleAudioEntity y CAutoPilot sustituidas por dobles que rellenan su parte
//     con bytes deterministas, para que un campo sin escribir o puesto a cero de mas aparezca como diferencia), CCarCtrl::
//     UpdateCarCount, destructor (CReplay::RecordVehicleDeleted, CVehicle::DeleteRwObject, CRadar::ClearBlipForEntity con el pool
//     de vehiculos falso, CFire::Extinguish, FxManager_c::DestroyFxSystem, RwTextureDestroy, CRopes::FindRope, CRope::Remove,
//     ~CAEVehicleAudioEntity, ~CPhysical), CTxdStore::RemoveRef, RpClumpForAllAtomics, CEntity::DeleteRwObject / SetModelIndex,
//     CVehicle::CustomCarPlate_TextureCreate, CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors,
//     CColModel::RemoveCollisionVolumes, CVehicleSaveStructure::Construct / Extract y CGenericGameStorage::Save/LoadDataToWorkBuffer.
// Se guardan y restauran: jugadores, PlayerInFocus, punteros de los pools, model infos, CTimer, vehiculos con colmodel especial,
// modelos de colision especiales, ms_compsUsed, contadores de ambulancias/bomberos y estados de carga de cuatro modelos.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/PlayerPed.h"
#include "VehicleSaveStructure.h"
#include "Garages.h"
#include "CarCtrl.h"
#include "Radar.h"
#include "Ropes.h"
#include "Rope.h"
#include "PedClothesDesc.h"
#include "Ragdoll/IKChainManager.h"
#include "EventDamage.h"
#include "EventVehicleDied.h"
#include "PedDamageResponseCalculator.h"
#include "../ProbeRuntime.hpp"

namespace CVehBLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };
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

// --- modulo de la DLL y busqueda de cuerpos (como CPedD / VehCtor62) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline bool Prologue(uintptr_t f){ const auto* b=reinterpret_cast<const uint8*>(f); if(b[0]==0x8B&&b[1]==0xFF)b+=2; return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC; }
// llamadas (E8) a funciones de la DLL con prologo push ebp / mov ebp,esp, en orden de aparicion y sin repetir la anterior
inline std::vector<uintptr_t> DllCalls(uintptr_t fn,size_t span=0x1800){ std::vector<uintptr_t> v; fn=FollowIn(fn); if(!InDll(fn))return v;
    for(uintptr_t p=fn;p<fn+span;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(InDll(t)&&Prologue(FollowIn(t))){ const uintptr_t b=FollowIn(t); if(v.empty()||v.back()!=b)v.push_back(b); } }
    return v; }
inline uintptr_t FirstDllCall(uintptr_t fn){ const auto v=DllCalls(fn,0x300); return v.empty() ? 0 : v[0]; }
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
constexpr size_t VEHN=0xA18, VSLOT=0xA18, NVSLOT=8, VA=3, VB=5;            // CHeli = 0xA18; vehiculo probado en el hueco 3 y otro en el 5
constexpr size_t PEDN=0x7C4, NPED=12, NPOOL=6, PNEW0=6, PPLAYER=10, POTHER=11;  // huecos 0..5 = pool de peatones; 6..9 = peatones nuevos; 10 = jugador falso
alignas(16) inline std::array<uint8,VSLOT*NVSLOT+0x400> vehStore{};
alignas(16) inline std::array<uint8,PEDN*NPED+0x400> pedStore{};
alignas(16) inline std::array<uint8,0x40> vstate{}, pstate{};      // estados de hueco (bit 7 = libre) de los pools falsos
alignas(16) inline std::array<uint8,0x20> fakeVehPool{}, fakePedPool{};
alignas(16) inline std::array<std::array<uint8,0x80>,NPED+4> mats{};   // matrices: 0..11 peatones, 12 vehiculo A, 13 vehiculo B
alignas(16) inline std::array<std::array<uint8,0x300>,NPED> intel{};
alignas(16) inline std::array<std::array<std::array<uint8,0x40>,2>,NPED> tasks{};
alignas(16) inline std::array<uint8,0x100> playerData{}, clothes{}, saveBuf{};
alignas(16) inline std::array<uint8,0x80> mi{}, col{}, mi2{}, col2{}, clump{}, fireBuf{}, plate{}, fxBuf{}, ropeBuf{};
inline std::array<void*,72> vehVt{}, pedVt{};
inline uint8* VEH(){ return vehStore.data()+VA*VSLOT; }
inline uint8* VEH2(){ return vehStore.data()+VB*VSLOT; }
inline uint8* PED(size_t k){ return pedStore.data()+k*PEDN; }

// desplazamientos crudos del EXE
constexpr size_t E_VPTR=0, E_MATRIX=0x14, E_RWOBJ=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_STATUS=0x36, E_PHYS=0x40, E_MASS=0x8C;
constexpr size_t V_ALARM=0x45C, V_DRIVER=0x460, V_PSGR=0x464, V_NUMPS=0x484, V_MAXPS=0x488, V_SPECIAL=0x48B, V_FIRE=0x490, V_CREATEDBY=0x4A4, V_BOMB=0x4A8,
                 V_INSTALLER=0x4CC, V_HEALTH=0x4C0, V_BOMBTIMER=0x4DE, V_DETONATOR=0x4E0, V_HORN=0x514, V_HASSLE=0x51B, V_VTYPE=0x590, V_VSUB=0x594,
                 V_PREVREMAP=0x598, V_REMAP=0x59A, V_REMAPTEX=0x59C, V_UPGR=0x43A, V_PART0=0x578, V_PLATE=0x588, V_COMPS=0x438, V_WEAPON=0x513,
                 V_FLAGS=0x428, V_FLAGS5=0x42C, V_DMGMGR=0x5A0, V_BURN=0x8E4, V_MAXPSBYTE=0x488;
constexpr size_t P_ARMOUR=0x548, P_FLAGS1=0x46C, P_INTEL=0x47C, P_PLAYERDATA=0x480, P_TYPE=0x598;
constexpr int16 MI_A=19990, MI_B=19991;
constexpr uintptr_t G_PEDPOOL=0xB74490, G_VEHPOOL=0xB74494, G_PLAYERINFOCUS=0xB7CD74, G_TIMESTEP=0xB7CB5C, G_TIMEMS=0xB7CB84, G_COMPS=0xB4E478,
                    G_SPECVEH=0xC1CC08, G_SPECCOL=0xC1CC78, G_AMBU=0x9690A8, G_VTBL=0x871E80, G_LOAD_GOLF=0x8E66D4, G_LOAD_CHROME=0x8E6814,
                    G_LOAD_M0=0x8E4CD0, G_LOAD_M95=0x8E4CD0+95*20;

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
struct Rec { const char* tag; uint32 n; std::array<Arg,24> a; };
inline std::array<Rec,768> recs{}; inline size_t nrec{}; inline bool recOverflow{};
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
inline uint32 baseSeed{}, randIdx{}, newPedIdx{}, affectsIdx{}, doorIdx{}, taskMask{};
inline std::array<uint32,16> randTab{};
inline bool coopRet{}, wantRet{}, placeDriver{}; inline uint8 maxPedsRet{}; inline int32 ropeRet{-1}, placeSeat{-1}; inline uint32 affectsBits{};
inline void* playerRet{};     // lo que devuelve FindPlayerPed
inline void* curVeh{};        // vehiculo probado (para los dobles de las bases y de los pools)
inline uint32 loadSeed{}, loadCalls{};

// --- dobles ---
// comunes
inline void __fastcall S_RegRef(void* self,void*,void* field){ T("RegisterReference",Pt{self},Pt{field}); }
inline void __fastcall S_CleanRef(void* self,void*,void* field){ T("CleanUpOldReference",Pt{self},Pt{field}); }
inline int __cdecl S_Rand(){ T("rand"); return int(randTab[(randIdx++)&15]); }
inline uint16 __cdecl S_RandNum(){ return uint16(S_Rand()); }
inline CPlayerPed* __cdecl S_FindPlayer(int32 id){ T("FindPlayerPed",I{id}); return static_cast<CPlayerPed*>(playerRet); }
inline bool __cdecl S_Coop(){ T("IsCoopGameGoingOn",Bo{coopRet}); return coopRet; }
inline void __cdecl S_KillByPlayer(const void* ped,int32 weapon,uint32 hs,int32 pid){ T("RegisterKillByPlayer",Pt{ped},I{weapon},U{hs&0xFFu},I{pid}); }
inline void __cdecl S_BlownUp(void* veh,int32 id){ T("RegisterCarBlownUpByPlayer",Pt{veh},I{id}); }
inline void __cdecl S_WorldRemove(void* e){ T("CWorld::Remove",Pt{e}); }
inline void __cdecl S_Garage(const char* tag,int32 a,int32 time,int32 b){ T("TriggerMessage",H{Fnv(tag,strlen(tag))},I{int16(a)},U{uint16(time)},I{int16(b)}); }
// ocupantes
inline void __fastcall S_AddHealth(void* pinfo,void*,int32 h){ T("CPlayerInfo::AddHealth",Pt{pinfo},I{h}); }
inline void __cdecl S_Request(int32 m,int32 flags){ T("CStreaming::RequestModel",I{m},I{flags}); }
inline void __cdecl S_DeletableExe(int32 m){ T("CStreaming::SetModelIsDeletable",I{m}); }                  // el EXE solo recibe el modelo
inline void __cdecl S_DeletableDll(int32 m,uint32){ T("CStreaming::SetModelIsDeletable",I{m}); }          // la DLL pasa ademas "mission" (false)
inline void __cdecl S_IncStat(int32 s,float v){ T("CStats::IncrementStat",I{s},Fl{v}); }
inline void __cdecl S_SetStat(int32 s,float v){ T("CStats::SetStatValue",I{s},Fl{v}); }
inline bool __fastcall S_Wants(void* ped,void*,int32 w,uint32 b){ T("CPlayerPed::DoesPlayerWantNewWeapon",Pt{ped},I{w},U{b&0xFFu},Bo{wantRet}); return wantRet; }
inline int32 __fastcall S_Give(void* self,void*,int32 w,uint32 ammo,uint32 b){ T("CPed::GiveWeapon",Pt{self},I{w},U{ammo},U{b&0xFFu}); return 0; }
inline void __fastcall S_Grant(void* self,void*,int32 w,uint32 ammo){ T("CPed::GrantAmmo",Pt{self},I{w},U{ammo}); }
inline void* __cdecl S_AddPedInCar(void* veh,uint32 driver,int32 gang,int32 seat,uint32 male,uint32 crim){
    T("CPopulation::AddPedInCar",Pt{veh},U{driver&0xFFu},I{gang},I{seat},U{male&0xFFu},U{crim&0xFFu});
    void* ped=PED(PNEW0+((newPedIdx++)&3));
    if(driver&0xFFu){ if(placeDriver)At<void*>(veh,V_DRIVER)=ped; }
    else if(placeSeat>=0&&placeSeat<8)At<void*>(veh,V_PSGR+4*size_t(placeSeat))=ped;
    return ped; }
inline void __cdecl S_RemovePed(void* ped){ T("CPopulation::RemovePed",Pt{ped}); }
inline void* __cdecl S_AddPed(uint32 type,int32 model,const float* pos,uint32 wander){
    T("CPopulation::AddPed",U{type},I{model},Pt{pos},Fl{pos[0]},Fl{pos[1]},Fl{pos[2]},U{wander&0xFFu});
    return PED(PNEW0+((newPedIdx++)&3)); }
inline bool __cdecl S_SetPedInCar(void* ped,void* veh,int32 door,uint32 asDriver){ T("CCarEnterExit::SetPedInCarDirect",Pt{ped},Pt{veh},I{door},U{asDriver&0xFFu}); return true; }
inline int32 __cdecl S_Door(const void* veh,int32 n){ const int32 d=int32(10+3*(doorIdx++)); T("CCarEnterExit::ComputeTargetDoorToEnterAsPassenger",Pt{veh},I{n},I{d}); return d; }
inline void __fastcall S_SetClothes(void* self,void*,const char* name,int32 part){ T("CPedClothesDesc::SetModel",Pt{self},H{Fnv(name,strlen(name))},I{part}); }
inline void __cdecl S_Rebuild(void* player,uint32 b){ T("CClothes::RebuildPlayer",Pt{player},U{b&0xFFu}); }
inline void __fastcall S_Look(void* mgr,void*,const char* name,void* ped,void* tgt,int32 time,int32 bone,void* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force){
    T("IKChainManager_c::LookAt",Pt{mgr},H{Fnv(name,strlen(name))},Pt{ped},Pt{tgt},I{time},I{bone},Pt{off},U{torso&0xFFu},Fl{speed},I{blend},I{prio},U{force&0xFFu}); }
inline void __fastcall S_MoveForce(void* self,void*,CVector f){ T("CPhysical::ApplyMoveForce",Pt{self},Fl{f.x},Fl{f.y},Fl{f.z}); }
inline void __fastcall S_TurnForce(void* self,void*,CVector f,CVector p){ T("CPhysical::ApplyTurnForce",Pt{self},Fl{f.x},Fl{f.y},Fl{f.z},Fl{p.x},Fl{p.y},Fl{p.z}); }
inline void* __fastcall S_FindTask(void* mgr,void*,int32 type){
    void* t=nullptr;
    for(size_t k=0;k<NPED;++k)if(mgr==intel[k].data()+4){
        // tareas 700 (pasajero) y 701 (conductor) del peaton k, solo si el caso las marca como presentes
        if(type==700&&((taskMask>>(2*k))&1u))t=tasks[k][0].data(); else if(type==701&&((taskMask>>(2*k+1))&1u))t=tasks[k][1].data(); }
    T("CTaskManager::FindActiveTaskByType",Pt{mgr},I{type},Pt{t}); return t; }
inline void* __fastcall S_EventAdd(void* grp,void*,CEvent* ev,uint32 valid){
    const int32 type=int32(ev->GetEventType()); const uint8* e=reinterpret_cast<const uint8*>(ev);
    auto D=[&](size_t o){ return *reinterpret_cast<const uint32*>(e+o); };
    if(type==9){   // CEventDamage: solo los campos definidos (el EXE deja sin escribir los bytes de relleno)
        T("CEventGroup::Add",Pt{grp},I{type},U{valid&0xFFu},Pt{reinterpret_cast<const void*>(uintptr_t(D(0x14)))},U{D(0x18)},U{D(0x1C)},U{D(0x20)},U{uint32(e[0x24])},U{uint32(e[0x25]&0x1F)},
          U{D(0x28)},U{D(0x2C)},U{D(0x30)},U{D(0x34)},U{D(0x38)},U{D(0x3C)},U{uint32(e[0x40])},U{uint32(e[0x41])},U{uint32(e[0x42])},U{uint32(e[0x43])}); }
    else T("CEventGroup::Add",Pt{grp},I{type},U{valid&0xFFu},U{D(4)},U{uint32(e[8])},Pt{reinterpret_cast<const void*>(uintptr_t(D(0xC)))},U{uint32(e[0x10])});
    return nullptr; }
inline void __fastcall S_Compute(void* calc,void*,void* ped,void* resp,uint32 speak){
    const uint8* c=static_cast<const uint8*>(calc);
    T("CPedDamageResponseCalculator::ComputeDamageResponse",Pt{calc},Pt{reinterpret_cast<const void*>(uintptr_t(*reinterpret_cast<const uint32*>(c)))},U{*reinterpret_cast<const uint32*>(c+4)},U{*reinterpret_cast<const uint32*>(c+8)},
      U{*reinterpret_cast<const uint32*>(c+0xC)},U{uint32(c[0x10])},Pt{ped},Pt{resp},U{speak&0xFFu});
    At<uint8>(resp,9)=0x77; }                                         // marca en la respuesta (evento + 0x41): se calculo
inline bool __fastcall S_Affects(void* ev,void*,void* ped){ const bool b=((affectsBits>>((affectsIdx++)&7))&1u)!=0; T("CEventDamage::AffectsPed",Pt{ped},Bo{b}); return b; }
// ciclo de vida
inline void* __fastcall S_PhysCtor(void* self,void*){ T("CPhysical::CPhysical",Pt{self}); Rng g{baseSeed|1u}; for(size_t k=0;k<0x138;k+=4)At<uint32>(self,k)=g.next(); return self; }
inline void __fastcall S_PhysDtor(void* self,void*){ T("CPhysical::~CPhysical",Pt{self}); }
inline void* __fastcall S_AudCtor(void* self,void*){ T("CAEVehicleAudioEntity::CAEVehicleAudioEntity",Pt{self}); Rng g{(baseSeed^0xA5A5A5A5u)|1u}; for(size_t k=0;k<0x24C;k+=4)At<uint32>(self,k)=g.next(); return self; }
inline void __fastcall S_AudDtor(void* self,void*){ T("CAEVehicleAudioEntity::~CAEVehicleAudioEntity",Pt{self}); }
inline void* __fastcall S_ApCtor(void* self,void*){ T("CAutoPilot::CAutoPilot",Pt{self}); Rng g{(baseSeed^0x5A5A5A5Au)|1u}; for(size_t k=0;k<0x98;k+=4)At<uint32>(self,k)=g.next(); return self; }
inline void __cdecl S_UpdateCount(void* veh,uint32 dec){ T("CCarCtrl::UpdateCarCount",Pt{veh},U{dec&0xFFu}); }
inline void __cdecl S_RecDel(void* veh){ T("CReplay::RecordVehicleDeleted",Pt{veh}); }
inline void __fastcall S_VehDeleteRw(void* self,void*){ T("CVehicle::DeleteRwObject",Pt{self}); }
inline void __cdecl S_ClearBlip(int32 type,int32 ref){ T("CRadar::ClearBlipForEntity",I{type},I{ref}); }
inline void __fastcall S_Extinguish(void* fire,void*){ T("CFire::Extinguish",Pt{fire}); }
inline void __fastcall S_DestroyFx(void* mgr,void*,void* fx){ T("FxManager_c::DestroyFxSystem",Pt{mgr},Pt{fx}); }
inline int32 __cdecl S_TexDestroy(void* tex){ T("RwTextureDestroy",Pt{tex}); return 1; }
inline int32 __cdecl S_FindRope(uint32 id){ T("CRopes::FindRope",U{id}); return ropeRet; }
inline void __fastcall S_RopeRemove(void* rope,void*){ T("CRope::Remove",Pt{rope}); }
inline void __cdecl S_RemoveRef(int32 i){ T("CTxdStore::RemoveRef",I{i}); }
inline uintptr_t cbCleanDll{};
inline void* __cdecl S_ForAllAtomics(void* c,void* cb,void* data){
    const uintptr_t a=uintptr_t(cb); T("RpClumpForAllAtomics",Pt{c},U{(a==0x6D34D0u||(cbCleanDll&&(a==cbCleanDll||FollowIn(a)==cbCleanDll))) ? 1u : 0u},Pt{data}); return c; }
inline void __fastcall S_EntityDeleteRw(void* self,void*){ T("CEntity::DeleteRwObject",Pt{self}); }
inline void __fastcall S_EntitySetModel(void* self,void*,uint32 idx){ T("CEntity::SetModelIndex",Pt{self},U{idx}); At<uint16>(self,E_MODEL)=uint16(idx); }
inline bool __fastcall S_PlateCreate(void* self,void*,void* m){ T("CVehicle::CustomCarPlate_TextureCreate",Pt{self},Pt{m}); return true; }
inline int32 __cdecl S_MaxPeds(int32 idx){ T("GetMaximumNumberOfPassengersFromNumberOfDoors",I{idx},U{maxPedsRet}); return int32(maxPedsRet); }
inline void __fastcall S_ColRemove(void* self,void*){ T("CColModel::RemoveCollisionVolumes",Pt{self}); }
// guardado
inline void __fastcall S_VConstruct(void* save,void*,void* veh){ T("CVehicleSaveStructure::Construct",Pt{veh}); Rng g{baseSeed^0x3C3C3C3Cu}; if(!g.s)g.s=1; for(size_t k=0;k<0x68;k+=4)At<uint32>(save,k)=g.next(); }
inline void __fastcall S_VExtract(void* save,void*,void* veh){ T("CVehicleSaveStructure::Extract",Pt{veh},H{Fnv(save,0x68)}); }
inline bool __cdecl S_SaveData(void* d,int32 n){ T("SaveDataToWorkBuffer",I{n},H{Fnv(d,n>0&&n<=0x400 ? size_t(n) : 0)}); return true; }
inline bool __cdecl S_LoadData(void* d,int32 n){ T("LoadDataFromWorkBuffer",I{n}); Rng g{(loadSeed+977u*(loadCalls++))|1u}; if(n>0&&n<=0x400)for(int32 k=0;k<n;++k)At<uint8>(d,size_t(k))=uint8(g.next()>>7); return true; }
// tablas virtuales falsas
inline void* __fastcall V_VehDelete(void* self,void*,uint32 flags){ T("vehicle_destructor_virtual",Pt{self},U{flags}); return self; }
inline void __fastcall V_BlowUp(void* self,void*,void* damager,uint32 hide){ T("BlowUpCar",Pt{self},Pt{damager},U{hide&0xFFu}); }
inline void* __fastcall V_PedDelete(void* self,void*,uint32 flags){ T("ped_destructor_virtual",Pt{self},U{flags}); return self; }
inline void __fastcall V_PedFlag(void* self,void*){ T("FlagToDestroyWhenNextProcessed",Pt{self}); }
template<int N> void* __fastcall VTrap(void* v,void*) { T("virtual_inesperada",I{N},Pt{v}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,72>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }

// ======================= fixtures =======================
inline void RandFill(uint8* p,size_t n,Rng& r){ for(size_t k=0;k<n;++k)p[k]=uint8(r.next()>>5); }
inline float PickF(Rng& r){
    static const float v[]{0.f,-0.f,1.f,-1.f,0.5f,2.f,100.f,-100.f,1e-3f,1e30f,-1e30f,std::bit_cast<float>(0x7F800000u),std::bit_cast<float>(0xFF800000u),std::bit_cast<float>(0x7FC00000u)};
    return r.next()%3==0 ? v[r.next()%std::size(v)] : r.uni(-10.f,10.f); }
inline float PickFin(Rng& r){ static const float v[]{0.f,-0.f,1.f,-1.f,0.5f,2.f,100.f,-100.f,1e-3f}; return r.next()%3==0 ? v[r.next()%std::size(v)] : r.uni(-10.f,10.f); }
// matriz RW/GTA de 0x40 bytes: right 0, flags 0xC, forward 0x10, up 0x20, pos 0x30
// sin NaN ni infinitos cuando specials es false (las salidas en coma flotante se comparan bit a bit: dos NaN distintos operando juntos
// se resuelven distinto en x87 y SSE)
inline float PickFx(Rng& r,bool specials){ return specials ? PickF(r) : (r.pct(8) ? (r.pct(50) ? 1e30f : -1e30f) : PickFin(r)); }
inline void FillMat(uint8* m,Rng& r,bool specials=true){
    std::memset(m,0,0x80);
    const uint32 mode=r.next()%5;
    if(mode==0){ At<float>(m,0)=1.f; At<float>(m,0x14)=1.f; At<float>(m,0x28)=1.f; }
    else if(mode==1){ for(size_t k=0;k<12;++k)At<float>(m,0x10*(k/3)+4*(k%3))=PickFx(r,specials); }
    else { for(size_t k=0;k<9;++k)At<float>(m,0x10*(k/3)+4*(k%3))=r.uni(-1.2f,1.2f); }
    At<uint32>(m,0xC)=0; At<uint32>(m,0x1C)=0; At<uint32>(m,0x2C)=0; At<uint32>(m,0x3C)=0x3F800000u;
    for(size_t c=0;c<3;++c)At<float>(m,0x30+4*c)=mode==1 ? PickFx(r,specials) : (r.pct(8) ? 0.f : r.uni(-300.f,300.f)); }
inline void PrepPed(size_t k,Rng& r){
    uint8* p=PED(k); RandFill(p,PEDN,r);
    At<void*>(p,E_VPTR)=pedVt.data(); At<void*>(p,E_MATRIX)=mats[k].data(); FillMat(mats[k].data(),r);
    At<void*>(p,P_INTEL)=intel[k].data(); RandFill(intel[k].data(),0x300,r);
    At<uint8>(p,E_STATUS)=uint8((r.next()&0xF8)|3);                                             // tipo de entidad 3 = peaton
    At<uint16>(p,E_MODEL)=uint16(r.pickI({7,8,9,10,11,12,19,0x1A4,0x7FFF,0x8000,0xFFFF}));
    At<uint32>(p,P_TYPE)=r.pct(60) ? r.below(32) : r.pickI({0,1,2,3,6,7,16,17,18,19,20,21,22,23,24,200,-1,0x7FFFFFFF,int32(0x80000000)});
    At<float>(p,E_MASS)=r.pct(10) ? PickFin(r) : r.uni(1.f,200.f);
    At<float>(p,P_ARMOUR)=r.pct(10) ? PickFin(r) : r.uni(0.f,150.f);
    At<void*>(p,P_PLAYERDATA)=nullptr;
    for(auto& t:tasks[k])RandFill(t.data(),0x40,r); }
inline void PrepVeh(uint8* v,Rng& r,size_t matIdx){
    RandFill(v,VEHN,r);
    At<void*>(v,E_VPTR)=vehVt.data(); At<void*>(v,E_MATRIX)=mats[matIdx].data(); FillMat(mats[matIdx].data(),r);
    At<uint8>(v,E_STATUS)=uint8((r.next()&0xF8)|2);                                              // tipo 2 = vehiculo
    At<void*>(v,E_RWOBJ)=nullptr;
    At<uint16>(v,E_MODEL)=uint16(MI_A);
    for(size_t o=V_DRIVER;o<V_PSGR+32;o+=4)At<void*>(v,o)=nullptr;                              // conductor y 8 pasajeros
    At<void*>(v,V_FIRE)=nullptr; At<void*>(v,V_INSTALLER)=nullptr; At<void*>(v,V_DETONATOR)=nullptr;
    for(size_t o=V_PART0;o<=V_PART0+8;o+=4)At<void*>(v,o)=nullptr; At<void*>(v,V_PLATE)=nullptr;
    At<uint8>(v,V_MAXPS)=uint8(r.below(9)); At<uint8>(v,V_NUMPS)=uint8(r.below(10));
    At<int8>(v,V_SPECIAL)=-1;
    At<uint32>(v,V_VTYPE)=r.below(12); At<uint32>(v,V_VSUB)=r.below(12); }

// fixture comun
inline void Reset(Rng& r){
    nrec=0; recOverflow=false; randIdx=0; newPedIdx=0; affectsIdx=0; doorIdx=0; loadCalls=0; baseSeed=r.next()|1u; loadSeed=r.next();
    for(auto& v:vehVt)v=nullptr;
    FillTraps(vehVt,std::make_integer_sequence<int,72>{}); FillTraps(pedVt,std::make_integer_sequence<int,72>{});
    vehVt[0]=reinterpret_cast<void*>(&V_VehDelete); vehVt[41]=reinterpret_cast<void*>(&V_BlowUp);
    pedVt[0]=reinterpret_cast<void*>(&V_PedDelete); pedVt[21]=reinterpret_cast<void*>(&V_PedFlag);
    for(size_t k=0;k<NPED;++k)PrepPed(k,r);
    PrepVeh(VEH(),r,12); PrepVeh(VEH2(),r,13);
    RandFill(playerData.data(),playerData.size(),r); At<void*>(playerData.data(),4)=clothes.data(); RandFill(clothes.data(),clothes.size(),r);
    RandFill(mi.data(),mi.size(),r); RandFill(col.data(),col.size(),r); RandFill(mi2.data(),mi2.size(),r); RandFill(col2.data(),col2.size(),r);
    RandFill(clump.data(),clump.size(),r); clump[0]=2; RandFill(fireBuf.data(),fireBuf.size(),r); RandFill(plate.data(),plate.size(),r);
    RandFill(fxBuf.data(),fxBuf.size(),r); RandFill(ropeBuf.data(),ropeBuf.size(),r); RandFill(saveBuf.data(),saveBuf.size(),r);
    At<void*>(mi.data(),0x14)=col.data(); At<void*>(mi2.data(),0x14)=col2.data();
    // pools falsos: vehiculos (8 huecos de 0xA18) y peatones (6 huecos de 0x7C4); el estado de un hueco usado tiene el bit 7 a cero
    for(auto& b:vstate)b=uint8(r.next()&0x7F); for(auto& b:pstate)b=uint8(r.next()&0x7F);
    At<void*>(fakeVehPool.data(),0)=vehStore.data(); At<void*>(fakeVehPool.data(),4)=vstate.data(); At<uint32>(fakeVehPool.data(),8)=uint32(NVSLOT); At<int32>(fakeVehPool.data(),0xC)=-1; fakeVehPool[0x10]=0; fakeVehPool[0x11]=0;
    At<void*>(fakePedPool.data(),0)=pedStore.data(); At<void*>(fakePedPool.data(),4)=pstate.data(); At<uint32>(fakePedPool.data(),8)=uint32(NPOOL); At<int32>(fakePedPool.data(),0xC)=-1; fakePedPool[0x10]=0; fakePedPool[0x11]=0;
    // peaton jugador falso
    { uint8* p=PED(PPLAYER); At<uint32>(p,P_TYPE)=0; At<void*>(p,P_PLAYERDATA)=playerData.data(); }
    playerRet=PED(PPLAYER); curVeh=VEH();
    coopRet=r.pct(30); wantRet=r.pct(50); placeDriver=r.pct(70); maxPedsRet=uint8(r.pct(60) ? r.below(9) : r.next()&0xFF); ropeRet=r.pct(50) ? -1 : int32(r.below(8)); placeSeat=-1; affectsBits=r.next();
    for(auto& v:randTab){ static const uint32 sp[]{0x3FFF,0x3FFE,0x4000,0,1,2,3,4,0x7FFF,0x3FFC,0x4003,0x1FFF,0x2000,0x5FFF,0x6000,0x7000}; v=r.next()%3 ? sp[r.next()%std::size(sp)] : r.next()&0x7FFF; }
}
inline json Globals(){
    json g=json::array();
    g.push_back(Fnv(reinterpret_cast<const void*>(&CWorld::Players[0]),sizeof(CWorld::Players))); g.push_back(*reinterpret_cast<const uint8*>(G_PLAYERINFOCUS));
    g.push_back(Fnv(reinterpret_cast<const void*>(G_SPECVEH),16)); g.push_back(Fnv(reinterpret_cast<const void*>(G_SPECCOL),0xC0));
    g.push_back(Fnv(reinterpret_cast<const void*>(G_AMBU),8)); g.push_back(Fnv(reinterpret_cast<const void*>(G_COMPS),2));
    g.push_back(*reinterpret_cast<const uint8*>(G_LOAD_GOLF)); g.push_back(*reinterpret_cast<const uint8*>(G_LOAD_CHROME));
    return g; }

// ======================= ejecucion de un caso =======================
inline json Run(const std::string& nm,uintptr_t target,Rng& r,int i,json& out,bool candidate){
    Reset(r);
    uint8* const v=VEH(); json ret=nullptr; bool auditVeh=true; bool vptrMask=false; bool ctorMask=false;
    void*& pedPool=*reinterpret_cast<void**>(G_PEDPOOL); void*& vehPool=*reinterpret_cast<void**>(G_VEHPOOL);
    auto withPools=[&](auto&& fn){ void* const sp=pedPool; void* const sv=vehPool; pedPool=fakePedPool.data(); vehPool=fakeVehPool.data(); fn(); pedPool=sp; vehPool=sv; };
    auto U32=[&](size_t off)->uint32&{ return At<uint32>(v,off); };
    auto Seat=[&](size_t s)->void*&{ return At<void*>(v,V_PSGR+4*s); };
    auto RandPed=[&]()->void*{ return PED(r.below(6)); };
    auto OccPed=[&](uint32 pctNull)->void*{ return r.pct(pctNull) ? nullptr : static_cast<void*>(PED(r.below(6))); };
    const uint32 playerInFocus=r.next()%2; *reinterpret_cast<uint8*>(G_PLAYERINFOCUS)=uint8(playerInFocus);
    *reinterpret_cast<uint32*>(G_TIMEMS)=r.next()>>3; AtA<float>(G_TIMESTEP)=r.pct(10) ? r.pickI({0,1,2,50})*1.f : r.uni(0.05f,6.f);
    // jugador 0 falso en CWorld::Players
    CWorld::Players[0].m_pPed=reinterpret_cast<CPlayerPed*>(PED(PPLAYER)); CWorld::Players[0].m_nMaxArmour=uint8(r.pct(30) ? r.next() : r.pickI({0,1,50,100,150,200,255}));
    CWorld::Players[0].m_nMoney=int32(r.next()); CWorld::Players[1].m_nMoney=int32(r.next());

    if(nm=="Constructor"){
        ctorMask=true; vptrMask=true;
        std::memset(v,0xCD,VEHN);
        *reinterpret_cast<uint32*>(G_TIMEMS)=r.next();
        const uint32 createdBy=r.pct(70) ? r.below(6) : r.next();
        void* p=reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(v,createdBy); ret=Name(p); out["created_by"]=createdBy; }
    else if(nm=="Destructor"){
        vptrMask=true;
        // vehiculo en el hueco 3 del pool de vehiculos falso; ocupantes, fuego y particulas segun el caso
        U32(V_FLAGS)=(U32(V_FLAGS)&~6u)|(r.pct(50) ? 2u : 0u)|(r.pct(50) ? 4u : 0u);
        At<uint8>(v,V_MAXPS)=uint8(r.pct(80) ? r.below(9) : r.next()&0xFF);
        At<void*>(v,V_DRIVER)=OccPed(30); for(size_t s=0;s<8;++s)Seat(s)=OccPed(50);
        if(At<uint8>(v,V_MAXPS)>8)At<uint8>(v,V_MAXPS)=8;
        At<void*>(v,V_FIRE)=r.pct(50) ? fireBuf.data() : nullptr;
        At<int8>(v,V_SPECIAL)=r.pct(60) ? int8(-1) : int8(r.below(4));
        for(size_t k=0;k<3;++k)At<void*>(v,V_PART0+4*k)=r.pct(50) ? static_cast<void*>(fxBuf.data()+16*k) : nullptr;
        At<void*>(v,V_PLATE)=r.pct(50) ? plate.data() : nullptr;
        U32(E_PHYS)=(U32(E_PHYS)&~0x20000000u)|(r.pct(30) ? 0x20000000u : 0u);
        { static const float hp[]{0.f,100.f,249.99997f,250.f,250.00003f,1000.f,-5.f,std::bit_cast<float>(0x7FC00000u),1e9f}; At<float>(v,V_HEALTH)=r.pct(70) ? hp[r.below(std::size(hp))] : r.uni(-100.f,1100.f); }
        AtA<uint32>(G_AMBU)=r.pickI({0,1,5,100}); AtA<uint32>(G_AMBU+4)=r.pickI({0,1,5,100});
        for(size_t k=0;k<4;++k)At<void*>(reinterpret_cast<void*>(G_SPECVEH),4*k)=r.pct(50) ? VEH2() : nullptr;
        withPools([&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }); }
    else if(nm=="SetModelIndex"){
        // modelos de las dos tablas de saltos (RC 441/464/465/501/564; armas 476/511/513/520/576) y sus vecinos
        static const int32 models[]{441,464,465,501,564,440,442,463,466,500,502,563,565,476,511,513,520,576,475,477,510,512,514,519,521,575,577,400,420,550,600,0x1B9-1,0x1B9+0x7B,0x1B9+0x7C,0x1DC-1,0x1DC+0x64,0x1DC+0x65,0,1,19,19989};
        const uint32 idx=r.pct(70) ? uint32(models[r.below(std::size(models))]) : r.below(700);
        U32(V_FLAGS5)=r.next(); At<uint8>(v,V_WEAPON)=uint8(r.next()); At<uint8>(v,V_MAXPS)=uint8(r.next());
        for(size_t k=0;k<2;++k)AtA<uint8>(G_COMPS+k)=uint8(r.next());
        CModelInfo::ms_modelInfoPtrs[idx]=reinterpret_cast<CBaseModelInfo*>(r.pct(50) ? mi.data() : mi2.data());
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(v,idx); out["index"]=idx; }
    else if(nm=="DeleteRwObject"){
        At<int16>(v,V_PREVREMAP)=int16(r.pct(40) ? -1 : r.pickI({0,1,5,77,-2,0x7FFF})); At<int16>(v,V_REMAP)=int16(r.next()); At<void*>(v,V_REMAPTEX)=reinterpret_cast<void*>(uintptr_t(r.next()));
        At<void*>(v,E_RWOBJ)=r.pct(90) ? static_cast<void*>(clump.data()) : nullptr;
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="GetHeightAboveRoad"){
        At<uint16>(v,E_MODEL)=uint16(MI_A); CModelInfo::ms_modelInfoPtrs[MI_A]=reinterpret_cast<CBaseModelInfo*>(mi.data());
        for(size_t k=0;k<6;++k)At<float>(col.data(),4*k)=PickF(r);
        const float f=reinterpret_cast<float(__thiscall*)(void*)>(target)(v); ret=F(f); }
    else if(nm=="GetTowHitchPos"||nm=="GetTowBarPos"){
        At<uint16>(v,E_MODEL)=uint16(MI_A); CModelInfo::ms_modelInfoPtrs[MI_A]=reinterpret_cast<CBaseModelInfo*>(mi.data());
        FillMat(mats[12].data(),r,false);
        for(size_t k=0;k<6;++k)At<float>(col.data(),4*k)=PickFx(r,false);
        float outPos[3]; for(auto& f:outPos)f=PickFx(r,false);
        const uint32 check=r.pct(85) ? 1u : 0u;
        const uint32 b=reinterpret_cast<uint32(__thiscall*)(void*,float*,uint32,void*)>(target)(v,outPos,check,VEH2());
        ret=b&0xFF; out["out"]={F(outPos[0]),F(outPos[1]),F(outPos[2])}; }
    else if(nm=="HeightAboveCeiling"){
        static const float hs[]{499.99997f,500.f,500.00003f,949.99994f,950.f,950.00006f,1499.9999f,1500.f,1500.0001f,799.99994f,800.f,800.00006f,0.f,-1.f,1e9f,
                                std::bit_cast<float>(0x7FC00000u),std::bit_cast<float>(0x7F800000u),std::bit_cast<float>(0xFF800000u)};
        const float h=r.pct(60) ? hs[r.below(std::size(hs))] : r.uni(-100.f,2500.f);
        const int32 fm=r.pct(80) ? int32(r.below(8)) : r.pickI({-1,-2,8,100,0x7FFFFFFF,int32(0x80000000)});
        const float f=reinterpret_cast<float(__thiscall*)(void*,float,int32)>(target)(v,h,fm); ret=F(f); out["h"]=F(h); out["fm"]=fm; }
    else if(nm=="IsSphereTouchingVehicle"){
        // colmodel: modelo falso (MI_A) o colmodel especial del vehiculo (indice 0..3 de m_aSpecialColModel, global real restaurado)
        const int32 spec=r.pct(25) ? int32(r.below(4)) : -1; At<int8>(v,V_SPECIAL)=int8(spec);
        At<uint8>(v,E_STATUS)=uint8((At<uint8>(v,E_STATUS)&0xF8)|2);
        CModelInfo::ms_modelInfoPtrs[MI_A]=reinterpret_cast<CBaseModelInfo*>(mi.data()); At<uint16>(v,E_MODEL)=uint16(MI_A);
        uint8* cm=spec>=0 ? reinterpret_cast<uint8*>(G_SPECCOL+0x30*size_t(spec)) : col.data();
        for(size_t k=0;k<6;++k)At<float>(cm,4*k)=r.pct(25) ? PickF(r) : (k<3 ? -r.uni(0.f,4.f) : r.uni(0.f,4.f));
        const float px=r.pct(20) ? PickF(r) : r.uni(-8.f,8.f), py=r.pct(20) ? PickF(r) : r.uni(-8.f,8.f), pz=r.pct(20) ? PickF(r) : r.uni(-8.f,8.f);
        const float rad=r.pct(15) ? PickF(r) : r.uni(0.f,2.f);
        const uint32 b=reinterpret_cast<uint32(__thiscall*)(void*,CVector,float)>(target)(v,CVector{px,py,pz},rad);
        ret=b&0xFF; }
    else if(nm=="FindTyreNearestPoint"){
        const float px=r.pct(15) ? PickF(r) : r.uni(-8.f,8.f), py=r.pct(15) ? PickF(r) : r.uni(-8.f,8.f);
        const int32 t=reinterpret_cast<int32(__thiscall*)(void*,CVector2D)>(target)(v,CVector2D{px,py}); ret=t; }
    else if(nm=="Shutdown"){
        for(size_t k=0;k<4;++k){ uint8* e=reinterpret_cast<uint8*>(G_SPECCOL+0x30*k); RandFill(e,0x30,r); At<uint32>(e,0x2C)=r.pct(50) ? 0u : (r.next()|1u); }
        reinterpret_cast<void(__cdecl*)()>(target)(); }
    else if(nm=="UpdatePassengerList"){
        At<uint8>(v,V_NUMPS)=uint8(r.pct(15) ? 0 : r.pct(70) ? r.below(9) : r.next()&0xFF);
        if(r.pct(40)){ for(size_t s=0;s<8;++s)Seat(s)=nullptr; } else for(size_t s=0;s<8;++s)Seat(s)=OccPed(60);
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="GetSpareHasslePosId"){
        U32(V_VSUB)=r.pct(80) ? r.below(12) : r.next(); At<uint8>(v,V_HASSLE)=uint8(r.pct(50) ? r.next() : (r.next()&0x3F)|(r.pct(30) ? 0x3F : 0));
        const int32 id=reinterpret_cast<int32(__thiscall*)(void*)>(target)(v); ret=id; }
    else if(nm=="SetHasslePosId"){
        At<uint8>(v,V_HASSLE)=uint8(r.next()); const int32 pos=r.pct(80) ? int32(r.below(8)) : r.pickI({-3,-1,8,9,31,32,33,40,200,0x7FFFFFFF,int32(0x80000000)}); const uint32 en=r.below(2);
        reinterpret_cast<void(__thiscall*)(void*,int32,uint32)>(target)(v,pos,en); out["pos"]=pos; out["en"]=en; }
    else if(nm=="ProcessDelayedExplosion"){
        static const uint32 timers[]{0,1,2,16,17,100,1000,7000,0x7FFF,0x8000,0xFFFF,40000,65535,33};
        At<uint16>(v,V_BOMBTIMER)=uint16(r.pct(15) ? 0 : r.pct(60) ? timers[r.below(std::size(timers))] : r.next());
        At<void*>(v,V_DETONATOR)=OccPed(30);
        { static const float steps[]{0.f,0.5f,1.f,2.f,16.666666f,17.f,33.f,100.f,1000.f,-1.f,-50.f,4000.f,60000.f}; AtA<float>(G_TIMESTEP)=r.pct(70) ? steps[r.below(std::size(steps))] : r.uni(0.f,40.f); }
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="ProcessCarAlarm"){
        static const uint32 alarms[]{0,1,2,5,19,20,21,100,1000,0xFFFE,0xFFFF,0x8000,0x7FFF,40};
        At<uint16>(v,V_ALARM)=uint16(r.pct(70) ? alarms[r.below(std::size(alarms))] : r.next());
        { static const float steps[]{0.f,0.25f,0.5f,1.f,1.25f,2.5f,5.f,25.f,50.f,-1.f,-60.f,1000.f,5000.f,100000.f}; AtA<float>(G_TIMESTEP)=r.pct(75) ? steps[r.below(std::size(steps))] : r.uni(0.f,6.f); }
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="ActivateBomb"){
        At<uint8>(v,V_BOMB)=uint8(r.next()); if(r.pct(70))At<uint8>(v,V_BOMB)=uint8((At<uint8>(v,V_BOMB)&0xF8)|r.below(8));
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="ActivateBombWhenEntered"){
        At<void*>(v,V_DRIVER)=r.pct(70) ? RandPed() : nullptr; At<void*>(v,V_INSTALLER)=OccPed(40);
        At<uint8>(v,V_BOMB)=uint8(r.next()); if(r.pct(60))At<uint8>(v,V_BOMB)=uint8((At<uint8>(v,V_BOMB)&0xF8)|5);
        At<uint8>(v,V_FLAGS5)=uint8(r.next()); At<uint16>(v,V_BOMBTIMER)=uint16(r.next()); At<void*>(v,V_DETONATOR)=reinterpret_cast<void*>(uintptr_t(0x9100u+(r.next()&0xFFu)*16u));
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="ExtinguishCarFire"){
        { static const uint32 sts[]{0,1,2,3,4,5,6,7,31,28,5}; At<uint8>(v,E_STATUS)=uint8((At<uint8>(v,E_STATUS)&7)|(sts[r.below(std::size(sts))]<<3)); }
        { static const float hp[]{0.f,299.99997f,300.f,300.00003f,std::bit_cast<float>(0x7FC00000u),1000.f,-5.f,std::bit_cast<float>(0x7F800000u)}; At<float>(v,V_HEALTH)=r.pct(70) ? hp[r.below(std::size(hp))] : r.uni(0.f,1100.f); }
        At<void*>(v,V_FIRE)=r.pct(60) ? fireBuf.data() : nullptr; At<uint32>(v,V_VTYPE)=r.pct(70) ? 0u : r.below(12);
        { static const uint8 es[]{0,100,214,215,216,224,225,226,249,250,255}; At<uint8>(v,V_DMGMGR+4)=r.pct(70) ? es[r.below(std::size(es))] : uint8(r.next()); }
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="ReactToVehicleDamage"){
        At<void*>(v,V_DRIVER)=OccPed(35); Seat(0)=OccPed(35); void* causer=r.pct(15) ? nullptr : RandPed();
        reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(v,causer); }
    else if(nm=="PickRandomPassenger"){
        for(size_t s=0;s<8;++s)Seat(s)=r.pct(20) ? OccPed(30) : (r.pct(15) ? static_cast<void*>(PED(r.below(6))) : nullptr);
        if(r.pct(10))for(size_t s=0;s<8;++s)Seat(s)=nullptr;
        void* p=reinterpret_cast<void*(__thiscall*)(void*)>(target)(v); ret=Name(p); }
    else if(nm=="KillPedsInVehicle"){
        At<uint8>(v,V_MAXPS)=uint8(r.below(9)); At<void*>(v,V_DRIVER)=OccPed(30); for(size_t s=0;s<8;++s)Seat(s)=OccPed(45);
        reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }
    else if(nm=="DestroyVehicleAndDriverAndPassengers"){
        At<uint8>(v,V_MAXPS)=uint8(r.below(9)); At<void*>(v,V_DRIVER)=OccPed(30); for(size_t s=0;s<8;++s)Seat(s)=OccPed(45);
        reinterpret_cast<void(__cdecl*)(void*)>(target)(v); }
    else if(nm=="RemovePassenger"){
        At<uint32>(v,V_VTYPE)=r.pct(30) ? 6u : r.below(12); At<uint8>(v,V_MAXPS)=uint8(r.pct(85) ? r.below(9) : r.pickI({9,0,8}));
        At<uint8>(v,V_NUMPS)=uint8(r.pct(70) ? r.below(9) : r.next());
        for(size_t s=0;s<8;++s)Seat(s)=OccPed(35);
        void* ped=r.pct(10) ? nullptr : (r.pct(70) ? Seat(r.below(8)) : RandPed()); if(!ped&&r.pct(50))ped=RandPed();
        reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(v,ped); }
    else if(nm=="SetUpDriver"){
        At<void*>(v,V_DRIVER)=OccPed(60); At<uint8>(v,V_CREATEDBY)=uint8(r.pct(60) ? 1 : r.below(6));
        const int32 gang=r.pickI({-1,0,1,2,19,20,200}); const uint32 male=r.below(2), crim=r.below(2);
        void* p=reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,uint32)>(target)(v,gang,male,crim); ret=Name(p); }
    else if(nm=="SetupPassenger"){
        const int32 seat=r.pct(92) ? int32(r.below(8)) : r.pickI({-1,8,9});
        const int32 gang=r.pct(60) ? int32(r.below(26)) : r.pickI({-1,200,0x7FFFFFFF,int32(0x80000000),13,14,23,24});
        static const uint32 mods[]{409,420,438,0x1A4,0x199,0x1B6,400,401};
        At<uint16>(v,E_MODEL)=uint16(r.pct(40) ? mods[r.below(std::size(mods))] : r.below(700));
        for(size_t s=0;s<8;++s)Seat(s)=r.pct(60) ? nullptr : static_cast<void*>(PED(r.below(6)));
        if(seat>=0&&seat<8&&r.pct(30))Seat(size_t(seat))=PED(r.below(6));
        At<void*>(v,V_DRIVER)=OccPed(50); At<uint8>(v,V_MAXPS)=uint8(r.below(9)); At<uint8>(v,V_NUMPS)=uint8(r.below(9));
        // el nuevo peaton (hueco PNEW0): tipo y modelo de la tabla pequena para que coincidan con los ocupantes
        placeSeat=r.pct(65) ? seat : (seat>0&&r.pct(60) ? int32(r.below(uint32(seat))) : -1);
        for(size_t k=PNEW0;k<PNEW0+4;++k){ At<uint32>(PED(k),P_TYPE)=r.pct(50) ? r.below(26) : uint32(r.pickI({6,7,16,17,18,19,20,21,23,-1,int32(0x80000000)})); At<uint16>(PED(k),E_MODEL)=uint16(r.pickI({7,8,9,10})); }
        for(size_t k=0;k<6;++k)At<uint16>(PED(k),E_MODEL)=uint16(r.pickI({7,8,9,10}));
        const uint32 male=r.below(2), crim=r.below(2);
        void* p=reinterpret_cast<void*(__thiscall*)(void*,int32,int32,uint32,uint32)>(target)(v,seat,gang,male,crim); ret=Name(p); out["seat"]=seat; out["gang"]=gang; }
    else if(nm=="SetDriver"){
        static const uint32 mods[]{416,420,438,427,457,588,596,597,598,599,400,415,0x1A0-1,0x1A0+0xB7,0x1A0+0xB8,0x1A0,1000,0,0xFFFF};
        At<uint16>(v,E_MODEL)=uint16(r.pct(85) ? mods[r.below(std::size(mods))] : r.below(700));
        U32(V_FLAGS)=(U32(V_FLAGS)&~0x80u)|(r.pct(70) ? 0x80u : 0u); At<void*>(v,V_DRIVER)=OccPed(60);
        At<uint32>(v,V_VTYPE)=r.pct(30) ? 9u : r.below(12);
        void* drv=r.pct(60) ? static_cast<void*>(PED(PPLAYER)) : (r.pct(80) ? RandPed() : static_cast<void*>(PED(POTHER)));
        if(r.pct(20))At<uint32>(drv,P_TYPE)=r.below(3);
        *reinterpret_cast<uint8*>(G_PLAYERINFOCUS)=uint8(r.below(2));
        reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(v,drv); out["driver_is_player"]=(drv==playerRet); }
    else if(nm=="RemoveDriver"){
        static const uint32 mods[]{457,596,597,598,599,400,0x1C9-1,0x1C9+1,0x253,0x254,0x257,0x258,0,0xFFFF};
        At<uint16>(v,E_MODEL)=uint16(r.pct(85) ? mods[r.below(std::size(mods))] : r.below(700));
        U32(V_FLAGS)=(U32(V_FLAGS)&~0x80u&~0x10u)|(r.pct(60) ? 0x80u : 0u)|(r.pct(60) ? 0x10u : 0u);
        At<void*>(v,V_DRIVER)=r.pct(55) ? static_cast<void*>(PED(PPLAYER)) : OccPed(40);
        if(r.pct(30))playerRet=r.pct(50) ? static_cast<void*>(PED(POTHER)) : nullptr;
        if(r.pct(25)&&At<void*>(v,V_DRIVER))At<uint32>(At<void*>(v,V_DRIVER),P_TYPE)=r.below(3);
        AtA<uint8>(G_LOAD_GOLF)=uint8(r.pct(60) ? 1 : r.below(4)); AtA<uint8>(G_LOAD_CHROME)=uint8(r.pct(60) ? 1 : r.below(4));
        const uint32 dont=r.below(2);
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(v,dont); out["dont"]=dont; }
    else if(nm=="KillPedsGettingInVehicle"){
        // 6 peatones del pool falso: libres o no, dentro de un vehiculo o no, con colision o no, con tarea de entrar (700/701) hacia este vehiculo u otro
        for(size_t k=0;k<NPOOL;++k){
            pstate[k]=uint8(r.next()&0x7F)|(r.pct(20) ? 0x80 : 0);
            uint8* p=PED(k); At<uint32>(p,P_FLAGS1)=(At<uint32>(p,P_FLAGS1)&~0x100u)|(r.pct(25) ? 0x100u : 0u);
            At<uint32>(p,E_FLAGS)=(At<uint32>(p,E_FLAGS)&~1u)|(r.pct(25) ? 1u : 0u);
            for(size_t t=0;t<2;++t){ void* car=r.pct(55) ? static_cast<void*>(v) : static_cast<void*>(VEH2()); At<void*>(tasks[k][t].data(),0xC)=car; } }
        // tareas: la de conductor (701) o pasajero (700) solo aparecen si el caso lo pide; FindActiveTaskByType devuelve nullptr si no
        taskMask=r.next()|(r.next()&r.next());
        withPools([&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(v); }); }
    else if(nm=="FillVehicleWithPeds"){
        const uint32 afro=r.below(2);
        At<uint8>(v,V_MAXPS)=uint8(r.below(9)); At<void*>(v,V_DRIVER)=r.pct(35) ? static_cast<void*>(PED(PPLAYER)) : OccPed(40); for(size_t s=0;s<8;++s)Seat(s)=OccPed(45);
        AtA<uint8>(G_LOAD_M0)=uint8(r.pct(55) ? 1 : r.below(4)); AtA<uint8>(G_LOAD_M95)=uint8(r.pct(55) ? 1 : r.below(4));
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(v,afro); out["afro"]=afro; }
    else if(nm=="Save"||nm=="Load"){
        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(v)&0xFF; }
    // ---- recogida ----
    const uint32 vptr=At<uint32>(v,0);
    if(nm=="Constructor"||nm=="Destructor")out["vptr"]=(candidate ? InDll(vptr) : vptr==G_VTBL) ? json("CVehicle") : json(vptr);
    if(auditVeh){
        // vptr normalizada; en el constructor se enmascara el nibble alto del byte 0x42F (el EXE conserva el relleno, la DLL lo pone a cero)
        json w=json::array();
        for(size_t k=0;k<VEHN;k+=4){ if(k==0&&vptrMask)continue; uint32 x=*reinterpret_cast<const uint32*>(v+k); if(ctorMask&&k==V_FLAGS5)x&=0x0FFFFFFFu; w.push_back(x); }
        out["veh"]=w; }
    out["vstore_rest"]=Fnv(vehStore.data(),VA*VSLOT)^(Fnv(vehStore.data()+(VA+1)*VSLOT,VSLOT*(NVSLOT-VA-1)+0x400)*31);   // el resto del almacenamiento (incluye el otro vehiculo y los huecos vecinos)
    out["others"]={Fnv(VEH2(),VEHN),Fnv(pedStore.data(),PEDN*NPED),Fnv(playerData.data(),playerData.size()),Fnv(clothes.data(),clothes.size()),Fnv(fireBuf.data(),fireBuf.size()),
                   Fnv(clump.data(),clump.size()),Fnv(mi.data(),mi.size()),Fnv(col.data(),col.size()),Fnv(plate.data(),plate.size()),Fnv(saveBuf.data(),saveBuf.size())};
    out["globals"]=Globals();
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=32;
inline const std::array<Fn,NFN> fns{{
    {"CVehicle","Constructor",0x6D5F10,0,-1,512},{"CVehicle","Destructor",0x6E2B40,0,-1,512},
    {"CVehicle","SetModelIndex",0,0x871E80,5,512},{"CVehicle","DeleteRwObject",0,0x871E80,8,256},
    {"CVehicle","GetHeightAboveRoad",0,0x871E80,53,256},{"CVehicle","GetTowHitchPos",0,0x871E80,59,256},{"CVehicle","GetTowBarPos",0,0x871E80,60,256},
    {"CVehicle","Save",0,0x871E80,64,128},{"CVehicle","Load",0,0x871E80,65,128},
    {"CVehicle","HeightAboveCeiling",0x6D2600,0,-1,512},{"CVehicle","IsSphereTouchingVehicle",0x6D84D0,0,-1,512},{"CVehicle","FindTyreNearestPoint",0x6D7BC0,0,-1,512},
    {"CVehicle","Shutdown",0x6D0B40,0,-1,128},{"CVehicle","FillVehicleWithPeds",0x6E2900,0,-1,512},{"CVehicle","KillPedsGettingInVehicle",0x6D82F0,0,-1,256},
    {"CVehicle","KillPedsInVehicle",0x6D1C80,0,-1,256},{"CVehicle","PickRandomPassenger",0x6D2A10,0,-1,256},{"CVehicle","RemoveDriver",0x6D1950,0,-1,512},
    {"CVehicle","RemovePassenger",0x6D1610,0,-1,512},{"CVehicle","SetDriver",0x6D16A0,0,-1,1024},{"CVehicle","SetUpDriver",0x6D1A50,0,-1,256},
    {"CVehicle","SetupPassenger",0x6D1AA0,0,-1,1024},{"CVehicle","UpdatePassengerList",0x6D29E0,0,-1,256},{"CVehicle","DestroyVehicleAndDriverAndPassengers",0x6D2250,0,-1,256},
    {"CVehicle","GetSpareHasslePosId",0x6D3AE0,0,-1,256},{"CVehicle","SetHasslePosId",0x6D3B30,0,-1,256},{"CVehicle","ActivateBomb",0x6D24F0,0,-1,128},
    {"CVehicle","ActivateBombWhenEntered",0x6D2570,0,-1,256},{"CVehicle","ExtinguishCarFire",0x6D2460,0,-1,256},{"CVehicle","ReactToVehicleDamage",0x6D5490,0,-1,256},
    {"CVehicle","ProcessCarAlarm",0x6D21F0,0,-1,256},{"CVehicle","ProcessDelayedExplosion",0x6D1340,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cvehb-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cvehb-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cvehb");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"cvehb-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // cuerpos de la DLL: bases del constructor/destructor, DeleteRwObject/SetModelIndex de CEntity y de CVehicle, AffectsPed
    uintptr_t dllPhysCtor{}, dllAudCtor{}, dllApCtor{}, dllPhysDtor{}, dllAudDtor{}, dllVehDeleteRw{}, dllEntDeleteRw{}, dllEntSetModel{}, dllAffects{};
    {
        const uintptr_t ctorBody=FirstDllCall(DllBody("CVehicle","Constructor")); const auto ctorCalls=ctorBody ? DllCalls(ctorBody) : std::vector<uintptr_t>{};
        const uintptr_t a=FirstDllCall(DllBody("CPhysical","Constructor")), b=FirstDllCall(DllBody("CAEVehicleAudioEntity","Constructor"));
        // El ctor de la DLL llama en orden a CPhysical, CAEVehicleAudioEntity y CAutoPilot (lista de inicializacion). Solo CPhysical se
        // contrasta con su hook; el del audio (b) puede no localizarse por el envoltorio, asi que se acepta por posicion (y si b existe, debe coincidir).
        if(ctorCalls.size()>=3&&a&&ctorCalls[0]==a&&(!b||ctorCalls[1]==b)){ dllPhysCtor=a; dllAudCtor=ctorCalls[1]; dllApCtor=ctorCalls[2]; }
        else Write({{"event","warning"},{"reason","CVehicle constructor bases not found"},{"calls",ctorCalls.size()},{"phys",a},{"audio",b}});
        const uintptr_t dtorBody=FirstDllCall(DllBody("CVehicle","Destructor")); const auto dtorCalls=dtorBody ? DllCalls(dtorBody) : std::vector<uintptr_t>{};
        const uintptr_t c=FirstDllCall(DllBody("CPhysical","Destructor")), d=FirstDllCall(DllBody("CAEVehicleAudioEntity","Destructor"));
        // En el destructor de la DLL el miembro de audio se destruye justo antes de la base: ~CAEVehicleAudioEntity y luego ~CPhysical son
        // llamadas consecutivas (se busca ~CPhysical en la lista y el audio es la anterior). Solo ~CPhysical se contrasta con su hook; el audio se acepta por posicion (y si d existe, debe coincidir).
        const auto itC=std::find(dtorCalls.begin(),dtorCalls.end(),c);
        if(c&&itC!=dtorCalls.end()&&itC!=dtorCalls.begin()&&(!d||*(itC-1)==d)){ dllPhysDtor=c; dllAudDtor=*(itC-1); }
        else Write({{"event","warning"},{"reason","CVehicle destructor bases not found"},{"calls",dtorCalls.size()},{"phys",c},{"audio",d}});
        dllVehDeleteRw=DllBody("CVehicle","DeleteRwObject"); dllEntDeleteRw=DllBody("CEntity","DeleteRwObject"); dllEntSetModel=DllBody("CEntity","SetModelIndex"); dllAffects=DllBody("CEventDamage","AffectsPed");
        Write({{"event","info"},{"phys_ctor",dllPhysCtor ? dllPhysCtor-modBase : 0},{"audio_ctor",dllAudCtor ? dllAudCtor-modBase : 0},{"autopilot_ctor",dllApCtor ? dllApCtor-modBase : 0},
               {"phys_dtor",dllPhysDtor ? dllPhysDtor-modBase : 0},{"audio_dtor",dllAudDtor ? dllAudDtor-modBase : 0},{"vehicle_delete_rw",dllVehDeleteRw ? dllVehDeleteRw-modBase : 0},
               {"entity_delete_rw",dllEntDeleteRw ? dllEntDeleteRw-modBase : 0},{"entity_set_model",dllEntSetModel ? dllEntSetModel-modBase : 0},{"affects_ped",dllAffects ? dllAffects-modBase : 0}}); }
    const bool ctorOk=dllPhysCtor&&dllAudCtor&&dllApCtor, dtorOk=dllPhysDtor&&dllAudDtor;
    cbCleanDll=Addr(static_cast<RpAtomic*(*)(RpAtomic*,void*)>(&RemoveAllUpgradesCB));

    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto* V=reinterpret_cast<CVehicle*>(VEH()); auto* A=reinterpret_cast<CAutomobile*>(VEH()); auto* P=reinterpret_cast<CPed*>(PED(0));
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); }; json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      Chk("m_matrix",Off(V,&V->m_matrix),E_MATRIX); Chk("m_nFlags",Off(V,&V->m_nFlags),E_FLAGS); Chk("m_nModelIndex",Off(V,&V->m_nModelIndex),E_MODEL);
      Chk("m_fMass",Off(V,&V->m_fMass),E_MASS); Chk("m_alarm",Off(V,&V->m_nAlarmState),V_ALARM); Chk("m_pDriver",Off(V,&V->m_pDriver),V_DRIVER); Chk("m_apPassengers",Off(V,V->m_apPassengers.data()),V_PSGR);
      Chk("m_nNumPassengers",Off(V,&V->m_nNumPassengers),V_NUMPS); Chk("m_nMaxPassengers",Off(V,&V->m_nMaxPassengers),V_MAXPS); Chk("m_vehicleSpecialColIndex",Off(V,&V->m_vehicleSpecialColIndex),V_SPECIAL);
      Chk("m_pFire",Off(V,&V->m_pFire),V_FIRE); Chk("m_fSteerAngle",Off(V,&V->m_fSteerAngle),0x494); Chk("m_f2ndSteerAngle",Off(V,&V->m_f2ndSteerAngle),0x498); Chk("m_nCreatedBy",Off(V,&V->m_nCreatedBy),V_CREATEDBY);
      Chk("m_nUsedForCover",Off(V,&V->m_nUsedForCover),0x4AA); Chk("m_fDirtLevel",Off(V,&V->m_fDirtLevel),0x4B0); Chk("m_nCurrentGear",Off(V,&V->m_nCurrentGear),0x4B4); Chk("m_fHealth",Off(V,&V->m_fHealth),V_HEALTH);
      Chk("m_pWhoInstalledBombOnMe",Off(V,&V->m_pWhoInstalledBombOnMe),V_INSTALLER); Chk("m_wBombTimer",Off(V,&V->m_wBombTimer),V_BOMBTIMER); Chk("m_pWhoDetonatedMe",Off(V,&V->m_pWhoDetonatedMe),V_DETONATOR);
      Chk("m_nVehicleWeaponInUse",Off(V,&V->m_nVehicleWeaponInUse),V_WEAPON); Chk("m_HornCounter",Off(V,&V->m_HornCounter),V_HORN); Chk("m_nHasslePosId",Off(V,&V->m_nHasslePosId),V_HASSLE);
      Chk("m_anCollisionLighting",Off(V,&V->m_anCollisionLighting[0]),0x574); Chk("m_pOverheatParticle",Off(V,&V->m_pOverheatParticle),V_PART0); Chk("m_pDustParticle",Off(V,&V->m_pDustParticle),V_PART0+8);
      Chk("m_pCustomCarPlate",Off(V,&V->m_pCustomCarPlate),V_PLATE); Chk("m_nVehicleType",Off(V,&V->m_nVehicleType),V_VTYPE); Chk("m_nVehicleSubType",Off(V,&V->m_nVehicleSubType),V_VSUB);
      Chk("m_nPreviousRemapTxd",Off(V,&V->m_nPreviousRemapTxd),V_PREVREMAP); Chk("m_nRemapTxd",Off(V,&V->m_nRemapTxd),V_REMAP); Chk("m_pRemapTexture",Off(V,&V->m_pRemapTexture),V_REMAPTEX);
      Chk("m_anUpgrades",Off(V,V->m_anUpgrades.data()),V_UPGR); Chk("m_anExtras",Off(V,V->m_anExtras.data()),V_COMPS); Chk("vehicleFlags",Off(V,&V->vehicleFlags),V_FLAGS); Chk("m_autoPilot",Off(V,&V->m_autoPilot),0x390);
      Chk("m_pHandlingData",Off(V,&V->m_pHandlingData),0x384); Chk("m_nCreationTime",Off(V,&V->m_nCreationTime),0x430); Chk("m_nDoorLock",Off(V,&V->m_nDoorLock),0x4F8);
      Chk("m_damageManager",Off(A,&A->m_damageManager),V_DMGMGR); Chk("m_fBurnTimer",Off(A,&A->m_fBurnTimer),V_BURN);
      Chk("sizeof(CHeli)",uint32(sizeof(CHeli)),uint32(VSLOT)); Chk("sizeof(CVehicle)",uint32(sizeof(CVehicle)),0x5A0); Chk("sizeof(CCopPed)",uint32(sizeof(CCopPed)),uint32(PEDN));
      Chk("sizeof(CPlayerInfo)",uint32(sizeof(CPlayerInfo)),0x190); Chk("Players[0].m_nMoney",uint32(uintptr_t(&CWorld::Players[0].m_nMoney)),0xB7CE50); Chk("Players[0].m_nMaxArmour",uint32(uintptr_t(&CWorld::Players[0].m_nMaxArmour)),0xB7CEE8);
      Chk("PlayerInFocus",uint32(uintptr_t(&CWorld::PlayerInFocus)),uint32(G_PLAYERINFOCUS)); Chk("m_pPed",Off(&CWorld::Players[0],&CWorld::Players[0].m_pPed),0);
      Chk("CPed.m_fArmour",Off(P,&P->m_fArmour),P_ARMOUR); Chk("CPed.m_nPedType",Off(P,&P->m_nPedType),P_TYPE); Chk("CPed.m_fMass",Off(P,&P->m_fMass),E_MASS); Chk("CPed.m_matrix",Off(P,&P->m_matrix),E_MATRIX);
      At<void*>(PED(0),P_INTEL)=intel[0].data(); At<void*>(PED(0),P_PLAYERDATA)=playerData.data();
      Chk("CPedIntelligence.m_TaskMgr",uint32(reinterpret_cast<const uint8*>(&P->GetTaskManager())-intel[0].data()),4); Chk("CPedIntelligence.m_eventGroup",uint32(reinterpret_cast<const uint8*>(&P->GetEventGroup())-intel[0].data()),0x68);
      Chk("GetPlayerData",uint32(reinterpret_cast<const uint8*>(P->GetPlayerData())-playerData.data()),0);
      Chk("IsModelLoaded(333)",uint32(uintptr_t(&CStreaming::ms_aInfoForModel[MODEL_GOLFCLUB].m_LoadState)),uint32(G_LOAD_GOLF)); Chk("IsModelLoaded(349)",uint32(uintptr_t(&CStreaming::ms_aInfoForModel[MODEL_CHROMEGUN].m_LoadState)),uint32(G_LOAD_CHROME));
      Chk("CCarCtrl.NumAmbulancesOnDuty",uint32(uintptr_t(&CCarCtrl::NumAmbulancesOnDuty)),uint32(G_AMBU)); Chk("CCarCtrl.NumFireTrucksOnDuty",uint32(uintptr_t(&CCarCtrl::NumFireTrucksOnDuty)),uint32(G_AMBU+4));
      Chk("sizeof(CVehicleSaveStructure)",uint32(sizeof(CVehicleSaveStructure)),0x68); Chk("m_aSpecialColModel",uint32(uintptr_t(&CVehicle::m_aSpecialColModel[0])),uint32(G_SPECCOL)); Chk("m_aSpecialColVehicle",uint32(uintptr_t(&CVehicle::m_aSpecialColVehicle[0])),uint32(G_SPECVEH));
      Chk("ms_compsUsed",uint32(uintptr_t(&CVehicleModelInfo::ms_compsUsed[0])),uint32(G_COMPS)); Chk("DamageResponse.size",uint32(sizeof(CPedDamageResponseCalculator)),0x14);
      Write({{"event","layout"},{"layout_ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    regions.clear();
    Snap(G_PEDPOOL,8); Snap(uintptr_t(&CWorld::Players[0]),sizeof(CWorld::Players)); Snap(G_PLAYERINFOCUS,1); Snap(G_TIMESTEP,4); Snap(G_TIMEMS,4); Snap(G_COMPS,2);
    Snap(G_SPECVEH,16); Snap(G_SPECCOL,0xC0); Snap(G_AMBU,8); Snap(G_LOAD_GOLF,1); Snap(G_LOAD_CHROME,1); Snap(G_LOAD_M0,1); Snap(G_LOAD_M95,1);
    Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[0]),4*1024); Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[MI_A]),8); Snap(uintptr_t(&CModelInfo::ms_modelInfoPtrs[19989]),4);

    named.clear();
    named.push_back({vehStore.data(),vehStore.size(),"vstore"}); named.push_back({pedStore.data(),pedStore.size(),"pstore"});
    for(size_t k=0;k<NPED;++k){ static std::array<std::string,NPED> nmPed, nmIntel, nmMat; nmPed[k]="ped"+std::to_string(k); nmIntel[k]="intel"+std::to_string(k); nmMat[k]="mat"+std::to_string(k);
        named.push_back({PED(k),PEDN,nmPed[k].c_str()}); named.push_back({intel[k].data(),intel[k].size(),nmIntel[k].c_str()}); named.push_back({mats[k].data(),mats[k].size(),nmMat[k].c_str()});
        static std::array<std::string,NPED> nmTask0, nmTask1; nmTask0[k]="task_driver"+std::to_string(k); nmTask1[k]="task_psgr"+std::to_string(k);
        named.push_back({tasks[k][0].data(),0x40,nmTask0[k].c_str()}); named.push_back({tasks[k][1].data(),0x40,nmTask1[k].c_str()}); }
    named.push_back({mats[12].data(),mats[12].size(),"matA"}); named.push_back({mats[13].data(),mats[13].size(),"matB"});
    named.push_back({playerData.data(),playerData.size(),"player_data"}); named.push_back({clothes.data(),clothes.size(),"clothes"}); named.push_back({saveBuf.data(),saveBuf.size(),"save"});
    named.push_back({mi.data(),mi.size(),"mi"}); named.push_back({col.data(),col.size(),"col"}); named.push_back({mi2.data(),mi2.size(),"mi2"}); named.push_back({col2.data(),col2.size(),"col2"});
    named.push_back({clump.data(),clump.size(),"clump"}); named.push_back({fireBuf.data(),fireBuf.size(),"fire"}); named.push_back({plate.data(),plate.size(),"plate"});
    named.push_back({fxBuf.data(),fxBuf.size(),"fx"}); named.push_back({ropeBuf.data(),ropeBuf.size(),"rope"});
    named.push_back({fakeVehPool.data(),fakeVehPool.size(),"fake_veh_pool"}); named.push_back({fakePedPool.data(),fakePedPool.size(),"fake_ped_pool"});
    named.push_back({vehVt.data(),sizeof(vehVt),"veh_vt"}); named.push_back({pedVt.data(),sizeof(pedVt),"ped_vt"});

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        if(!ctorOk&&nm=="Constructor"){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","DLL base bodies not found"}}); continue; }
        if(!dtorOk&&nm=="Destructor"){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","DLL base destructor bodies not found"}}); continue; }
        if(!dllAffects&&nm=="KillPedsGettingInVehicle"){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","CEventDamage::AffectsPed body not found"}}); continue; }
        if((!dllVehDeleteRw||!dllEntDeleteRw||!dllEntSetModel)&&(nm=="Destructor"||nm=="DeleteRwObject"||nm=="SetModelIndex")){ passed=false; Write({{"event","function_summary"},{"function",full},{"failures",fn.cases},{"cases",fn.cases},{"reason","DLL virtual bodies not found"}}); continue; }
        // parches de este grupo de funciones: los dobles de las funciones externas, nunca el codigo probado ni lo que este ejecuta de verdad
        Patches p; std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        P(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),S_RegRef);
        P(0x571A00,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),S_CleanRef);
        P(0x821B1E,0,S_Rand); P(0,Addr(&CGeneral::GetRandomNumber),S_RandNum);
        if(nm!="FillVehicleWithPeds")P(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),S_FindPlayer);   // el EXE lee Players[0].m_pPed sin llamar
        P(0x441390,Addr(&CGameLogic::IsCoopGameGoingOn),S_Coop);
        P(0x43DCD0,Addr(&CDarkel::RegisterKillByPlayer),S_KillByPlayer); P(0x43DF20,Addr(&CDarkel::RegisterCarBlownUpByPlayer),S_BlownUp);
        P(0x563280,Addr(static_cast<void(*)(CEntity*)>(&CWorld::Remove)),S_WorldRemove);
        P(0x447B80,Addr(&CGarages::TriggerMessage),S_Garage);
        P(0x56EAB0,Addr(&CPlayerInfo::AddHealth),S_AddHealth);
        P(0x4087E0,Addr(&CStreaming::RequestModel),S_Request); P(0x409C10,0,S_DeletableExe); P(0,Addr(&CStreaming::SetModelIsDeletable),S_DeletableDll);
        P(0x55C180,Addr(&CStats::IncrementStat),S_IncStat); P(0x55A070,Addr(&CStats::SetStatValue),S_SetStat);
        P(0x609710,Addr(&CPlayerPed::DoesPlayerWantNewWeapon),S_Wants);
        P(0x5E6080,Addr(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),S_Give); P(0x5DF220,Addr(&CPed::GrantAmmo),S_Grant);
        P(0x613A00,Addr(&CPopulation::AddPedInCar),S_AddPedInCar); P(0x610F20,Addr(&CPopulation::RemovePed),S_RemovePed); P(0x612710,Addr(&CPopulation::AddPed),S_AddPed);
        P(0x650280,Addr(&CCarEnterExit::SetPedInCarDirect),S_SetPedInCar); P(0x64F190,Addr(&CCarEnterExit::ComputeTargetDoorToEnterAsPassenger),S_Door);
        P(0x5A7920,Addr(static_cast<void(CPedClothesDesc::*)(const char*,eClothesModelPart)>(&CPedClothesDesc::SetModel)),S_SetClothes); P(0x5A82C0,Addr(&CClothes::RebuildPlayer),S_Rebuild);
        P(0x618970,Addr(&IKChainManager_c::LookAt),S_Look);
        P(0x5429F0,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),S_MoveForce); P(0x542A50,Addr(&CPhysical::ApplyTurnForce),S_TurnForce);
        P(0x681740,Addr(&CTaskManager::FindActiveTaskByType),S_FindTask);
        P(0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),S_EventAdd);
        P(0x4B5AC0,Addr(&CPedDamageResponseCalculator::ComputeDamageResponse),S_Compute); P(0x4B35A0,dllAffects,S_Affects);
        P(0x542260,dllPhysCtor,S_PhysCtor); P(0x542450,dllPhysDtor,S_PhysDtor); P(0x4F63E0,dllAudCtor,S_AudCtor); P(0x6D0A10,dllAudDtor,S_AudDtor); P(0x6D5E20,dllApCtor,S_ApCtor);
        P(0x424000,Addr(&CCarCtrl::UpdateCarCount),S_UpdateCount); P(0x45EBB0,Addr(&CReplay::RecordVehicleDeleted),S_RecDel);
        if(nm!="DeleteRwObject")P(0x6D6410,dllVehDeleteRw,S_VehDeleteRw);
        P(0x587C60,Addr(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity)),S_ClearBlip); P(0x5393F0,Addr(&CFire::Extinguish),S_Extinguish);
        P(0x4A9810,Addr(&FxManager_c::DestroyFxSystem),S_DestroyFx); P(0x7F3820,Addr(&RwTextureDestroy),S_TexDestroy);
        P(0x556000,Addr(&CRopes::FindRope),S_FindRope); P(0x556780,Addr(&CRope::Remove),S_RopeRemove);
        P(0x731A30,Addr(&CTxdStore::RemoveRef),S_RemoveRef); P(0x749B70,Addr(&RpClumpForAllAtomics),S_ForAllAtomics);
        P(0x534030,dllEntDeleteRw,S_EntityDeleteRw); P(0x532AE0,dllEntSetModel,S_EntitySetModel);
        P(0x6D10E0,Addr(&CVehicle::CustomCarPlate_TextureCreate),S_PlateCreate);
        P(0x4C89B0,Addr(&CVehicleModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors),S_MaxPeds);
        P(0x40F9E0,Addr(&CColModel::RemoveCollisionVolumes),S_ColRemove);
        P(0x5D2330,Addr(&CVehicleSaveStructure::Construct),S_VConstruct); P(0x5D23B0,Addr(&CVehicleSaveStructure::Extract),S_VExtract);
        P(0x5D1270,Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),S_SaveData);
        P(0x5D1300,Addr(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),S_LoadData);

        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC7EB0Bu}; if(!r.s)r.s=1;
                RestoreRegions();
                for(auto* h:hooks)h->State(candidate);
                for(auto& e:p.v)e->Enable(true); // los parches de entrada se reafirman despues de fijar los hooks
                const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                json out; out["switches"]=fn.slot>=0 ? ((target<0x400000||target>=0x1000000)==candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                json ret=Run(nm,target,r,i,out,candidate);
                trace=RecsToJson(); // el registro de los stubs se convierte a json fuera de la llamada
                RestoreRegions();
                out["ret"]=ret; out["trace"]=trace; return out;
            };
            const json original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& x)->json{ const auto s=x.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,x]:original.items())if(candidate[k]!=x)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()},{"layout_ok",layoutOk}});
}
}
