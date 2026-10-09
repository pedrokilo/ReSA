#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/WorldFinal1): funciones pendientes de CTrain, CPlane y CBike.
// Original (hook de la funcion desactivado: se ejecuta el codigo del EXE) frente a candidato (hook activo) sobre vehiculos falsos
// (buferes de 0xA18 bytes con vtable falsa y guardas), con fixtures deterministas por caso y la traza de todas las llamadas
// salientes (con sus argumentos). Los colaboradores se sustituyen en la direccion del EXE y en el cuerpo de la DLL (EntryPatch).
//
// CTrain (EXE, ver tabla): Shutdown, DisableRandomTrains, FindNumCarriagesPulled, FindSideStationIsOn, IsInTunnel, OpenTrainDoor,
//   AddPassenger, RemovePassenger, ProcessTrainAnnouncements, PlayAnnouncement, UpdateTrains, ReleaseOneMissionTrain,
//   RemoveOneMissionTrain, SetTrainSpeed, SetTrainCruiseSpeed, SetNewTrainPosition, RemoveMissionTrains, ReleaseMissionTrains y
//   MarkSurroundingEntitiesForCollisionWithTrain. Fuera: CTrain::ProcessControl (virtual hueco 10, fisica completa).
// CPlane: Constructor, InitPlaneGenerationAndRemoval, IsAlreadyFlying, SetGearUp, SetGearDown, CountPlanesAndHelis, AreWeInNoPlaneZone,
//   AreWeInNoBigPlaneZone y las virtuales Render (18), OpenDoor (27), SetUpWheelColModel (43), BurstTyre (44), Fix (50), SetupDamageAfterLoad (51).
// CBike: dmgDrawCarCollidingParticles, KnockOffRider, ReduceHornCounter, GetAllWheelsOffGround, DebugCode, PlayHornIfNecessary,
//   CalculateLeanMatrix, ProcessBuoyancy (sin pasajeros ni conductor) y las virtuales SetModelIndex (5), Teleport (14), Render (18),
//   ProcessControlCollisionCheck (23), GetComponentWorldPosition (25), ProcessOpenDoor (28), RemoveRefsToVehicle (40),
//   ProcessDrivingAnims (46), Fix (50), SetupDamageAfterLoad (51).
// Fuera de este banco (motivo en README.md): CBike::Constructor, CBike::Destructor, CBike::DamageKnockOffRider.
//
// El pool de vehiculos real NUNCA se toca: el puntero global 0xB74494 se sustituye temporalmente por un pool falso de 8 ranuras
// (mismo layout que CPool: almacen, estados, capacidad) y se restaura. Todo el estado global tocado se guarda y se restaura.
#include <fstream>
#include <set>
#include <cmath>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Train.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Object/Object.h"
#include "Models/ClumpModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "RepeatSector.h"
#include "RwHelper.h"
#include "Camera.h"
#include "Timer.h"
#include "World.h"
#include "Buoyancy.h"
#include "../ProbeRuntime.hpp"

namespace VehWave2Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };

// Hilo del banco: otros hilos del juego pueden llamar a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

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

// --- modulo de la DLL y busqueda de cuerpos (como WorldFinal1) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline bool Prologue(uintptr_t f){ const auto* b=reinterpret_cast<const uint8*>(f); if(b[0]==0x8B&&b[1]==0xFF)b+=2; return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC; }
inline uintptr_t FirstDllCall(uintptr_t fn){ fn=FollowIn(fn); if(!InDll(fn))return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(InDll(t)&&Prologue(FollowIn(t)))return FollowIn(t); }
    return 0; }
inline uintptr_t Exported(const char* mangled){ if(!modBase)return 0; const auto p=uintptr_t(GetProcAddress(reinterpret_cast<HMODULE>(modBase),mangled)); return p ? FollowIn(p) : 0; }
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
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(std::bit_cast<uintptr_t>(f)); }

// ======================= objetos falsos =======================
constexpr size_t kGuard=32, kVehSize=0xA18;
struct alignas(16) Veh { uint8 b[kVehSize]; };
alignas(16) inline std::array<uint8,kGuard> guardLo{};
inline std::array<Veh,8> vehs{};                                   // ranuras del pool falso y objetos sueltos
alignas(16) inline std::array<uint8,kGuard> guardHi{};
alignas(16) inline std::array<uint8,0x20> poolRaw{};               // CPool falso: almacen +0, estados +4, capacidad +8, ultima libre +0xC
alignas(16) inline std::array<uint8,8> poolStates{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> mats{};    // CMatrix falsos (derecha 0, adelante 0x10, arriba 0x20, posicion 0x30, RwMatrix* 0x40, posee 0x44)
alignas(16) inline std::array<uint8,0x40> colm{};                  // CColModel falso (caja: min 0, max 0xC)
alignas(16) inline std::array<std::array<uint8,0x40>,10> ltms{};   // RwMatrix falsos (posicion en 0x30)
alignas(16) inline std::array<uint8,0x80> frames{};                // "RwFrame*" falsos (solo identidad)
alignas(16) inline std::array<std::array<uint8,0x10>,4> objs{};    // RwObject falsos (flags en +2)
alignas(16) inline std::array<std::array<uint8,0x10>,4> delbuf{};  // bloques "reservados" falsos para Shutdown
alignas(16) inline std::array<uint8,0x100> handl{}, flyH{};        // tHandlingData / tFlyingHandlingData falsos
inline std::array<void*,96> vt{};                                  // vtable falsa de vehiculos
inline uint8* VP(int i){ return vehs[size_t(i)].b; }
inline void* Mat(int i){ return mats[size_t(i)].data(); }

// estado del caso actual compartido con los dobles
inline uint32 baseSeed{}, goN{}, chkN{}, visN{};
inline void* playerVeh{};                    // resultado de FindPlayerVehicle
inline bool pvlRet{}, buoyRet{};
inline std::array<bool,8> chk{};             // resultados de CheckCollision
inline std::array<CVector,2> buoyV{};        // 0 = punto de giro, 1 = fuerza de flotacion
inline std::array<CVector,3> wheelV{};
inline int32 rsSaved{};
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline void FillBase(void* self,size_t n){ Rng r{baseSeed}; size_t k=4; for(;k+4<=n;k+=4)At<uint32>(self,k)=r.next(); for(;k<n;++k)At<uint8>(self,k)=uint8(r.next()); At<uint32>(self,0)=0x0BADF00Du; }
inline float RandF(Rng& r){ const uint32 k=r.below(100); if(k<6)return 0.f; if(k<10)return r.pick({1.f,-1.f,0.5f,120.f,119.99f,1e-30f,3.0e38f,-3.0e38f}); return r.uni(-300.f,300.f); }
inline float FiniteF(Rng& r,float a,float b){ return r.pct(8) ? 0.f : r.uni(a,b); }

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
inline void __fastcall V_Dtor(void* self,void*,uint32 flags){ if(Mine())T("dtor",self,flags); }
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
inline void __fastcall V_Horn(void* self,void*){ if(Mine())T("PlayCarHorn",self); }
inline void __fastcall V_PlaneDmg(void* self,void*){ At<uint8>(self,0x42A)&=0xFE; }                                  // efecto real de CPlane::SetupDamageAfterLoad
inline bool __fastcall V_IsComp(void* self,void*,int32 c){ return At<void*>(self,0x5A0+4*size_t(c&15))!=nullptr; }    // efecto real de CBike::IsComponentPresent
// CWorld::Add/Remove(CEntity*) cdecl; FindPlayerVehicle(int32,bool) cdecl
inline void __cdecl S_WorldAdd(void* e){ if(Mine())T("world_add",e); }
inline void __cdecl S_WorldRemove(void* e){ if(Mine())T("world_remove",e); }
inline void* __cdecl S_FindPlayerVeh(int32 id,uint32 remote){ if(!Mine())return nullptr; T("find_player_vehicle",id,uint8(remote)); return playerVeh; }
// CWorld::ProcessVerticalLine (11 argumentos cdecl; los bool se apilan como dword)
inline uint8 __cdecl S_Pvl(const float* o,float d,void*,void*,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,uint32 b6,void* poly){
    if(Mine())T("process_vertical_line",o[0],o[1],o[2],d,uint8(b1),uint8(b2),uint8(b3),uint8(b4),uint8(b5),uint8(b6),poly);
    return pvlRet; }
inline void __fastcall S_FindPos(void* self,void*){ if(Mine())T("FindPositionOnTrackFromCoors",self); }
// Solo se trazan los bloques falsos del banco (delbuf); cualquier otro delete (EntryPatch, vectores de Patches, json de la traza)
// es memoria real del heap y se reenvia al free del EXE (0x8214BD/0x8213AE son jmp 0x82413F); sin esto T() reentra al liberar su propio vector.
inline bool InDelBuf(const void* p){ const auto u=uintptr_t(p); return u>=uintptr_t(delbuf.data()) && u<uintptr_t(delbuf.data())+sizeof(delbuf); }
inline void __cdecl S_Del(void* p){ if(Mine()&&InDelBuf(p))T("delete",p); else reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); }
inline void __cdecl S_DelArr(void* p){ if(Mine()&&InDelBuf(p))T("delete_array",p); else reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); }
inline void __cdecl S_ClearScan(){ if(Mine())T("clear_scan_codes"); }
// TrainHitStuff(lista, entidad): la lista es un miembro del CRepeatSector real; se traza su posicion (sector y tipo)
inline void __cdecl S_Hit(void* list,void* ent){ if(!Mine())return; const uintptr_t d=uintptr_t(list)-0xB992B8; T("train_hit_stuff",uint32(d/12),uint32(d%12),ent); }
// CVehicle::Render / CAutomobile::OpenDoor / CVehicle::SetModelIndex / ProcessRiderAnims / FillFrameArray ...
inline void __fastcall S_VehRender(void* self,void*){ if(Mine())T("CVehicle_Render",self); }
inline void __fastcall S_AutoOpenDoor(void* self,void*,void* ped,int32 comp,uint32 door,float ratio,uint32 snd){ if(Mine())T("CAutomobile_OpenDoor",self,ped,comp,door,ratio,uint8(snd)); }
inline void __fastcall S_VehSetModel(void* self,void*,uint32 idx){ if(Mine())T("CVehicle_SetModelIndex",self,idx); }
inline void __cdecl S_FillFrames(void* clump,void** nodes){ if(Mine()){ T("FillFrameArray",uint32(uintptr_t(clump)),nodes); for(int i=0;i<10;++i)nodes[i]=(i*7+goN)%5 ? &frames[size_t(i)*4] : nullptr; } }
inline void __cdecl S_RiderAnims(void* ped,void* veh,void* ride,void* h,int32 a5){ if(Mine())T("ProcessRiderAnims",ped,veh,ride,uint32(uintptr_t(h)),a5); }
inline void* __cdecl S_GetFirstObject(void* f){ const uint32 k=goN++; if(Mine())T("GetFirstObject",uint32(uintptr_t(f))); return objs[k&3].data(); }
inline void __fastcall S_Wheel(void* self,void*,int32 w,CVector& out,bool local){ out=wheelV[size_t(local?1:0)]; out.z+=float(w); if(Mine())T("GetWheelPosn",self,w,local); }
inline void* __fastcall S_ColModel(void* self,void*){ if(Mine())T("GetColModel",self); return colm.data(); }
inline void __fastcall S_SetOrient(void* self,void*,float x,float y,float z){ if(Mine())T("SetOrientation",self,x,y,z); }
inline void __fastcall S_CalcLean(void* self,void*){ if(Mine())T("CalculateLeanMatrix",self); }
inline void __fastcall S_Headlight(void* self,void*,int32 id,void* m,uint32 flag){ if(Mine())T("DoHeadLightBeam",self,id,m,uint8(flag)); }
inline void* __cdecl S_RwLtm(void* frame){ const size_t k=(uintptr_t(frame)-uintptr_t(frames.data()))/4; if(Mine())T("RwFrameGetLTM",uint32(k)); return ltms[k%10].data(); }
// CPhysical: ApplyMoveSpeed / ApplyTurnSpeed (modifican la matriz para comprobar la restauracion) / SkipPhysics / CheckCollision
inline void __fastcall S_ApplyMove(void* self,void*){ if(Mine())T("ApplyMoveSpeed",self); if(auto* m=At<uint8*>(self,0x14))At<float>(m,0x30)+=1.f; }
inline void __fastcall S_ApplyTurn(void* self,void*){ if(Mine())T("ApplyTurnSpeed",self); if(auto* m=At<uint8*>(self,0x14))At<float>(m,0x34)+=2.f; }
inline void __fastcall S_Skip(void* self,void*){ if(Mine())T("SkipPhysics",self); }
inline uint8 __fastcall S_CheckColl(void* self,void*){ const bool v=chk[chkN++&7]; if(Mine())T("CheckCollision",self,uint8(v)); return v; }
inline void __fastcall S_MoveForce(void* self,void*,float x,float y,float z){ if(Mine())T("ApplyMoveForce",self,x,y,z); }
inline void __fastcall S_TurnForce(void* self,void*,float fx,float fy,float fz,float px,float py,float pz){ if(Mine())T("ApplyTurnForce",self,fx,fy,fz,px,py,pz); }
// cBuoyancy::ProcessBuoyancy(CPhysical*, float, CVector* turnPoint, CVector* force) thiscall ret 0x10 (this = 0xC1C890)
inline uint8 __fastcall S_Buoy(void* self,void*,void* ent,float c,float* turn,float* force){
    if(Mine())T("ProcessBuoyancy",self,ent,c);
    turn[0]=buoyV[0].x; turn[1]=buoyV[0].y; turn[2]=buoyV[0].z; force[0]=buoyV[1].x; force[1]=buoyV[1].y; force[2]=buoyV[1].z; return buoyRet; }
inline RwBool __cdecl S_RenderState(int state,void* value){ if(Mine())T("render_state_set",state,uint32(uintptr_t(value))); return TRUE; }
inline RwBool __cdecl S_RenderGet(int state,void* out){ if(Mine())T("render_state_get",state); *static_cast<int32*>(out)=rsSaved; return TRUE; }
// CAutomobile::CAutomobile(int32, eVehicleCreatedBy, bool) : thiscall ret 0xC (rellena la base de forma determinista, fija el modelo)
inline void* __fastcall S_AutoCtor(void* self,void*,int32 model,uint32 createdBy,uint32 flag){
    T("CAutomobile_ctor",self,model,createdBy,uint8(flag));
    FillBase(self,0x988); At<int16>(self,0x22)=int16(model);
    { Rng r{baseSeed^0x5A5Au}; for(int i=0;i<4;++i)At<float>(self,0x838+4*size_t(i))=r.uni(-3.f,3.f); }   // m_wheelPosition: flotantes finitos
    { Rng q{baseSeed^0x1234u}; if(q.pct(50))At<uint16>(self,0x6AC)=0xFFFF; }                                // CPlane (VORTEX): frame del panel delantero izquierdo sin asignar
    return self; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,51> fns{{
    {"CTrain","Shutdown",0x6F58D0,0,-1,128},
    {"CTrain","DisableRandomTrains",0x6F5DB0,0,-1,64},
    {"CTrain","FindNumCarriagesPulled",0x6F5CD0,0,-1,128},
    {"CTrain","FindSideStationIsOn",0x6F5EF0,0,-1,64},
    {"CTrain","IsInTunnel",0x6F6320,0,-1,256},
    {"CTrain","OpenTrainDoor",0x6F5D80,0,-1,64},
    {"CTrain","AddPassenger",0x6F5D90,0,-1,64},
    {"CTrain","RemovePassenger",0x6F5DA0,0,-1,64},
    {"CTrain","ProcessTrainAnnouncements",0x6F5910,0,-1,64},
    {"CTrain","PlayAnnouncement",0x6F5920,0,-1,64},
    {"CTrain","UpdateTrains",0x6F5900,0,-1,64},
    {"CTrain","ReleaseOneMissionTrain",0x6F5DF0,0,-1,256},
    {"CTrain","RemoveOneMissionTrain",0x6F5DC0,0,-1,256},
    {"CTrain","SetTrainSpeed",0x6F5E20,0,-1,512},
    {"CTrain","SetTrainCruiseSpeed",0x6F5E50,0,-1,512},
    {"CTrain","SetNewTrainPosition",0x6F7140,0,-1,256},
    {"CTrain","RemoveMissionTrains",0x6F6A20,0,-1,256},
    {"CTrain","ReleaseMissionTrains",0x6F6B60,0,-1,256},
    {"CTrain","MarkSurroundingEntitiesForCollisionWithTrain",0x6F6640,0,-1,1024},
    {"CPlane","Constructor",0x6C8E20,0,-1,512},
    {"CPlane","InitPlaneGenerationAndRemoval",0x6CAD90,0,-1,64},
    {"CPlane","IsAlreadyFlying",0x6CAB90,0,-1,128},
    {"CPlane","SetGearUp",0x6CAC20,0,-1,256},
    {"CPlane","SetGearDown",0x6CAC70,0,-1,256},
    {"CPlane","CountPlanesAndHelis",0x6CCA50,0,-1,256},
    {"CPlane","AreWeInNoPlaneZone",0x6CCAA0,0,-1,1024},
    {"CPlane","AreWeInNoBigPlaneZone",0x6CCBB0,0,-1,512},
    {"CPlane","Render",0,0x871948,18,128},
    {"CPlane","OpenDoor",0,0x871948,27,256},
    {"CPlane","SetUpWheelColModel",0,0x871948,43,64},
    {"CPlane","BurstTyre",0,0x871948,44,64},
    {"CPlane","Fix",0,0x871948,50,256},
    {"CPlane","SetupDamageAfterLoad",0,0x871948,51,64},
    {"CBike","dmgDrawCarCollidingParticles",0x6B5A00,0,-1,64},
    {"CBike","KnockOffRider",0x6B5F40,0,-1,64},
    {"CBike","ReduceHornCounter",0x6B5F90,0,-1,128},
    {"CBike","GetAllWheelsOffGround",0x6B6790,0,-1,128},
    {"CBike","DebugCode",0x6B67A0,0,-1,64},
    {"CBike","PlayHornIfNecessary",0x6B7130,0,-1,128},
    {"CBike","CalculateLeanMatrix",0x6B7150,0,-1,1024},
    {"CBike","ProcessBuoyancy",0x6B5FB0,0,-1,1024},
    {"CBike","SetModelIndex",0,0x871360,5,128},
    {"CBike","Teleport",0,0x871360,14,256},
    {"CBike","Render",0,0x871360,18,256},
    {"CBike","ProcessControlCollisionCheck",0,0x871360,23,512},
    {"CBike","GetComponentWorldPosition",0,0x871360,25,128},
    {"CBike","ProcessOpenDoor",0,0x871360,28,64},
    {"CBike","RemoveRefsToVehicle",0,0x871360,40,256},
    {"CBike","ProcessDrivingAnims",0,0x871360,46,256},
    {"CBike","Fix",0,0x871360,50,128},
    {"CBike","SetupDamageAfterLoad",0,0x871360,51,64},
}};
inline constexpr size_t kRealFns=51;
static_assert(true);

// ======================= estado comun =======================
inline bool inCaseVeh[8]{};
inline json VehJson(int n){ json o=json::object(); for(int i=0;i<n;++i)o["veh"+std::to_string(i)]=Fnv(VP(i),kVehSize);
    o["guard"]=Fnv(guardLo.data(),kGuard,Fnv(guardHi.data(),kGuard)); return o; }
inline uint64 MatHash(const void* m){ const auto* b=static_cast<const uint8*>(m); uint64 h=1469598103934665603ull;   // sin el relleno de la CMatrix
    for(size_t off:{size_t(0),size_t(0x10),size_t(0x20),size_t(0x30)})h=Fnv(b+off,12,h); return Fnv(b+0x40,8,h); }
inline void InitVeh(int i,Rng& r){ FillBytes(VP(i),kVehSize,r); At<void*>(VP(i),0)=vt.data(); At<void*>(VP(i),0x14)=nullptr; }
inline void FillMat(int i,Rng& r){ auto& m=mats[size_t(i)]; FillBytes(m.data(),m.size(),r);
    for(int row=0;row<4;++row){ for(int c=0;c<3;++c)At<float>(m.data(),0x10*size_t(row)+4*size_t(c))=r.pct(10)?(row==c?1.f:0.f):r.uni(-2.f,2.f); At<uint32>(m.data(),0x10*size_t(row)+12)=0; }
    for(int c=0;c<3;++c)At<float>(m.data(),0x30+4*size_t(c))=r.uni(-200.f,200.f);
    At<uint32>(m.data(),0x40)=0; At<uint32>(m.data(),0x44)=0; }
inline void SetPlacement(int i,Rng& r,bool withMatrix){ auto* v=VP(i);
    for(int c=0;c<3;++c)At<float>(v,4+4*size_t(c))=r.uni(-3000.f,3000.f);
    if(withMatrix){ FillMat(0,r); At<void*>(v,0x14)=Mat(0); } else At<void*>(v,0x14)=nullptr; }
// cadena de trenes falsos 0..n-1 enlazados por +0x5D4 (siguiente) y +0x5D0 (anterior)
inline void Chain(int n,Rng& r){ for(int i=0;i<n;++i){ InitVeh(i,r); At<void*>(VP(i),0x5D4)= i+1<n ? VP(i+1) : nullptr; At<void*>(VP(i),0x5D0)= i>0 ? VP(i-1) : nullptr; At<int32>(VP(i),0x590)=6; } }

// ---- CTrain ----
inline json CaseShutdown(Rng& r,uintptr_t target){
    for(auto& b:delbuf)FillBytes(b.data(),b.size(),r);
    std::array<void*,4> tab{}; for(auto& p:tab)p=r.pct(65) ? delbuf[r.below(4)].data() : nullptr;
    for(size_t i=0;i<4;++i)AtA<void*>(0xC38024+4*i)=tab[i];
    Patches P; P.Add(0x8213AE,S_DelArr); P.Add(0x8214BD,S_Del);
    reinterpret_cast<void(__cdecl*)()>(target)();
    json o=json::object(); for(size_t i=0;i<4;++i)o["slot"+std::to_string(i)]=Name(uintptr_t(AtA<void*>(0xC38024+4*i)));
    return o; }
inline json CaseDisable(Rng& r,uintptr_t target){ AtA<uint32>(0xC38010)=r.next(); const uint8 v=uint8(r.next()); reinterpret_cast<void(__cdecl*)(bool)>(target)(v!=0);
    json o=json::object(); o["global"]=AtA<uint32>(0xC38010); (void)v; return o; }
inline json CaseNumCarriages(Rng& r,uintptr_t target){ const int n=1+int(r.below(6)); Chain(n,r);
    const uint32 ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(VP(int(r.below(uint32(n)))));
    json o=VehJson(n); o["ret"]=ret; return o; }
inline json CaseSide(Rng& r,uintptr_t target){ InitVeh(0,r);
    const uint8 ret=reinterpret_cast<uint8(__thiscall*)(void*)>(target)(VP(0));
    json o=VehJson(1); o["ret"]=ret; return o; }
inline json CaseTunnel(Rng& r,uintptr_t target,Patches& P,uintptr_t dllPvl){
    InitVeh(0,r); SetPlacement(0,r,r.pct(50)); pvlRet=r.pct(50);
    P.Add(0x5674E0,S_Pvl); P.Add(dllPvl,S_Pvl);
    const uint8 ret=reinterpret_cast<uint8(__thiscall*)(void*)>(target)(VP(0));
    json o=VehJson(1); o["ret"]=ret; return o; }
inline json CaseNop(Rng& r,uintptr_t target,const char* nm){
    InitVeh(0,r); const uint64 before=Fnv(VP(0),kVehSize);
    if(!strcmp(nm,"OpenTrainDoor"))reinterpret_cast<void(__thiscall*)(void*,float)>(target)(VP(0),RandF(r));
    else if(!strcmp(nm,"AddPassenger")||!strcmp(nm,"RemovePassenger"))reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(VP(0),VP(1));
    else if(!strcmp(nm,"PlayAnnouncement"))reinterpret_cast<void(__cdecl*)(uint8,uint8)>(target)(uint8(r.next()),uint8(r.next()));
    else reinterpret_cast<void(__cdecl*)()>(target)();
    json o=VehJson(1); o["unchanged"]=(before==Fnv(VP(0),kVehSize)); return o; }
inline json CaseReleaseOne(Rng& r,uintptr_t target){ const int n=int(r.below(6)); Chain(n,r); // n = 0: argumento nulo
    reinterpret_cast<void(__cdecl*)(void*)>(target)(n ? VP(0) : nullptr);
    return VehJson(std::max(n,1)); }
inline json CaseRemoveOne(Rng& r,uintptr_t target){ const int n=int(r.below(5)); Chain(n,r);
    reinterpret_cast<void(__cdecl*)(void*)>(target)(n ? VP(0) : nullptr);
    return VehJson(std::max(n,1)); }
inline json CaseSpeed(Rng& r,uintptr_t target){ InitVeh(0,r); At<float>(VP(0),0x5A4)=RandF(r);
    reinterpret_cast<void(__cdecl*)(void*,float)>(target)(VP(0),RandF(r)); return VehJson(1); }
inline json CaseCruise(Rng& r,uintptr_t target){ InitVeh(0,r);
    const float v=r.pct(15) ? r.pick({0.f,255.f,255.9f,256.f,300.7f,-1.5f,-0.9f,1.9999f,1e6f,-1e6f}) : r.uni(-20.f,400.f);
    reinterpret_cast<void(__cdecl*)(void*,float)>(target)(VP(0),v); return VehJson(1); }
inline json CaseNewPos(Rng& r,uintptr_t target,Patches& P,uintptr_t dllFind){
    InitVeh(0,r); SetPlacement(0,r,r.pct(50)); P.Add(0x6F6CC0,S_FindPos); P.Add(dllFind,S_FindPos);
    const CVector pos{RandF(r),RandF(r),RandF(r)};
    reinterpret_cast<void(__cdecl*)(void*,CVector)>(target)(VP(0),pos); json o=VehJson(1); o["matrix"]=MatHash(Mat(0)); return o; }
// pool falso de 8 ranuras: tipos de vehiculo y estados aleatorios
inline void BuildPool(Rng& r,bool trainsOnly){
    for(int i=0;i<8;++i){ InitVeh(i,r); const bool used=r.pct(70); poolStates[size_t(i)]=used ? uint8(r.below(0x80)) : uint8(0x80|r.below(0x80));
        At<int32>(VP(i),0x590)= trainsOnly||r.pct(60) ? 6 : int32(r.below(6)); uint8& fl=At<uint8>(VP(i),0x5B8); fl=r.pct(50)?uint8(fl|0x20):uint8(fl&~0x20); }
    At<void*>(poolRaw.data(),0)=vehs.data(); At<void*>(poolRaw.data(),4)=poolStates.data(); At<uint32>(poolRaw.data(),8)=8; At<int32>(poolRaw.data(),0xC)=-1;
    playerVeh= r.pct(60) ? static_cast<void*>(VP(int(r.below(8)))) : nullptr; }
inline json CasePoolMission(Rng& r,uintptr_t target){ BuildPool(r,false);
    const uint32 savedPool=AtA<uint32>(0xB74494); AtA<void*>(0xB74494)=poolRaw.data();
    reinterpret_cast<void(__cdecl*)()>(target)();
    AtA<uint32>(0xB74494)=savedPool; json o=VehJson(8); return o; }
inline json CaseMark(Rng& r,uintptr_t target){
    { const uint32 sc=r.below(100); AtA<uint16>(0xB7CD78)= sc<8 ? uint16(0xFFFF) : sc<15 ? uint16(0xFFFE) : sc<20 ? uint16(0) : uint16(r.next()); }
    const uint32 k=r.below(100); auto C=[&](){ return k<6 ? r.pick({-3000.f,-2999.f,3000.f,2999.f,0.f,-4000.f,4000.f,50.f,-50.f,-3050.f}) : r.uni(-3300.f,3300.f); };
    const CVector pos{C(),C(),C()}; const float radius=r.pct(10) ? r.pick({0.f,50.f,100.f,0.01f,6000.f}) : r.uni(0.f,400.f);
    reinterpret_cast<void(__cdecl*)(CVector,float,void*,bool)>(target)(pos,radius,VP(0),r.pct(50));
    json o=json::object(); o["scan"]=AtA<uint16>(0xB7CD78); return o; }

// ---- CPlane ----
inline void FillHandling(Rng& r){ FillBytes(handl.data(),handl.size(),r); FillBytes(flyH.data(),flyH.size(),r);
    At<float>(handl.data(),0x10)=FiniteF(r,-100.f,3000.f); At<float>(flyH.data(),0x30)=FiniteF(r,-5.f,5.f);
    At<uint32>(handl.data(),0xCC)= r.pct(50) ? (At<uint32>(handl.data(),0xCC)|0x100u) : (At<uint32>(handl.data(),0xCC)&~0x100u); }
inline void InitPlane(Rng& r){ InitVeh(0,r); FillHandling(r); At<void*>(VP(0),0x384)=handl.data(); At<void*>(VP(0),0x388)=flyH.data(); }
inline json PlaneCommon(){ json o=VehJson(1); o["handl"]=Fnv(handl.data(),handl.size()); o["flyh"]=Fnv(flyH.data(),flyH.size()); return o; }
inline json CasePlaneCtor(Rng& r,uintptr_t target){
    std::memset(VP(0),0xCD,kVehSize); baseSeed=r.next()|1u; goN=0;
    for(auto& o:objs)FillBytes(o.data(),o.size(),r);
    wheelV[0]=CVector{FiniteF(r,-2.f,2.f),FiniteF(r,-4.f,4.f),FiniteF(r,-2.f,2.f)}; wheelV[1]=CVector{FiniteF(r,-2.f,2.f),FiniteF(r,-4.f,4.f),FiniteF(r,-2.f,2.f)};
    const int32 model=r.pickI({520,476,512,519,553,539,513,400,411,0,520,539});
    const uint32 createdBy=r.below(5);
    // los datos que el constructor lee de la base se rellenan tras el doble de CAutomobile (word 0x6AC = frame del panel; 50 % = 0xFFFF)
    void* ret=reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(target)(VP(0),model,createdBy);
    json o=json::object(); o["ret_is_this"]=(ret==VP(0));
    const uint32 vp=At<uint32>(VP(0),0); o["vptr_ok"]=(vp==0x871948u)||InDll(vp);
    At<uint32>(VP(0),0)=0;
    o["plane"]=Fnv(VP(0),0xA04); o["tail"]=Fnv(VP(0)+0xA04,kVehSize-0xA04);
    o["guard"]=Fnv(guardLo.data(),kGuard,Fnv(guardHi.data(),kGuard));
    for(size_t i=0;i<objs.size();++i)o["obj"+std::to_string(i)]=Fnv(objs[i].data(),16);
    return o; }
inline json CasePlaneSimple(Rng& r,uintptr_t target,const char* nm){
    InitPlane(r);
    if(!strcmp(nm,"IsAlreadyFlying")){ AtA<uint32>(0xB7CB84)=r.pct(10) ? uint32(r.below(30000)) : r.next(); reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); }
    else if(!strcmp(nm,"SetGearUp")||!strcmp(nm,"SetGearDown")||!strcmp(nm,"SetupDamageAfterLoad")||!strcmp(nm,"Fix"))reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0));
    json o=PlaneCommon(); return o; }
inline json CasePlaneInit(Rng& r,uintptr_t target){ AtA<uint32>(0xC1CADC)=r.next(); AtA<uint32>(0xC1CAE0)=r.next(); AtA<uint8>(0x8D33BC)=uint8(r.next());
    reinterpret_cast<void(__cdecl*)()>(target)();
    json o=json::object(); o["status"]=AtA<uint32>(0xC1CADC); o["last"]=AtA<uint32>(0xC1CAE0); o["active"]=AtA<uint8>(0x8D33BC); return o; }
inline json CaseCountPlanes(Rng& r,uintptr_t target){ BuildPool(r,false);
    for(int i=0;i<8;++i)At<int32>(VP(i),0x594)=int32(r.below(7));
    const uint32 savedPool=AtA<uint32>(0xB74494); AtA<void*>(0xB74494)=poolRaw.data();
    const uint32 ret=reinterpret_cast<uint32(__cdecl*)()>(target)();
    AtA<uint32>(0xB74494)=savedPool; json o=VehJson(8); o["ret"]=ret; return o; }
inline void SetCamera(Rng& r,float cx,float cy,float cz){
    // 0xB6F028 = TheCamera; posicion de la transformacion simple en +4; puntero a matriz en +0x14
    const bool withMat=r.pct(40); FillMat(1,r);
    if(withMat){ At<void*>(reinterpret_cast<void*>(0xB6F028),0x14)=Mat(1); At<float>(Mat(1),0x30)=cx; At<float>(Mat(1),0x34)=cy; At<float>(Mat(1),0x38)=cz; }
    else { At<void*>(reinterpret_cast<void*>(0xB6F028),0x14)=nullptr; At<float>(reinterpret_cast<void*>(0xB6F028),4)=cx; At<float>(reinterpret_cast<void*>(0xB6F028),8)=cy; At<float>(reinterpret_cast<void*>(0xB6F028),12)=cz; } }
inline json CaseZone(Rng& r,uintptr_t target,bool big){
    float cx,cy,cz; const uint32 k=r.below(100);
    if(!big){
        if(k<40){ const float a=r.uni(0.f,6.2831853f), d=r.pick({199.f,199.99f,200.f,200.01f,201.f,150.f,250.f,200.f*(1.f+1e-7f),200.f*(1.f-1e-7f)})*(r.pct(30)?1.f:r.uni(0.98f,1.02f)); cx=-1073.f+d*std::cos(a); cy=-675.f+d*std::sin(a); cz=50.f+r.uni(-3.f,3.f); }
        else if(k<65){ cx=r.pick({-2743.f,-2742.99f,-2700.f,-2626.f,-2626.01f,-2650.f}); cy=r.pick({1300.f,1300.01f,1500.f,2200.f,2199.99f,1250.f}); cz=r.uni(0.f,100.f); }
        else if(k<85){ cx=r.pick({-1668.f,-1667.99f,-1400.f,-1122.f,-1122.01f,-1130.f}); cy=r.pick({541.f,541.01f,800.f,1118.f,1117.99f,500.f}); cz=r.uni(0.f,100.f); }
        else if(k<92){ cx=std::bit_cast<float>(0x7FC00000u); cy=r.uni(-3000.f,3000.f); cz=r.uni(0.f,100.f); }
        else { cx=r.uni(-3000.f,3000.f); cy=r.uni(-3000.f,3000.f); cz=r.uni(-10.f,300.f); }
    } else {
        const bool second=r.pct(50); const float ox= second ? -1836.f : 1522.f, oy= second ? 659.f : -1237.f;
        if(k<70){ const float a=r.uni(0.f,6.2831853f), d=r.pick({799.f,799.99f,800.f,800.01f,801.f,500.f,900.f})*(r.pct(30)?1.f:r.uni(0.99f,1.01f)); cx=ox+d*std::cos(a); cy=oy+d*std::sin(a); cz=r.uni(-10.f,300.f); }
        else if(k<80){ cx=r.uni(-3000.f,3000.f); cy=std::bit_cast<float>(0x7FC00000u); cz=0.f; }
        else { cx=r.uni(-3000.f,3000.f); cy=r.uni(-3000.f,3000.f); cz=r.uni(-10.f,300.f); } }
    SetCamera(r,cx,cy,cz);
    const uint8 ret=reinterpret_cast<uint8(__cdecl*)()>(target)();
    json o=json::object(); o["ret"]=ret; return o; }
inline json CasePlaneVirt(Rng& r,uintptr_t target,const char* nm){
    InitPlane(r); json o;
    if(!strcmp(nm,"Render")){ AtA<uint32>(0xB7CB84)=r.next(); reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=PlaneCommon(); }
    else if(!strcmp(nm,"OpenDoor")){ At<int16>(VP(0),0x22)= r.pct(50) ? int16(0x201) : int16(r.next());
        reinterpret_cast<void(__thiscall*)(void*,void*,int32,int32,float,bool)>(target)(VP(0),VP(1),int32(r.below(24)),int32(r.below(6)),RandF(r),r.pct(50)); o=PlaneCommon(); }
    else if(!strcmp(nm,"SetUpWheelColModel")){ const uint8 ret=reinterpret_cast<uint8(__thiscall*)(void*,void*)>(target)(VP(0),colm.data()); o=PlaneCommon(); o["ret"]=ret; }
    else if(!strcmp(nm,"BurstTyre")){ const uint8 ret=reinterpret_cast<uint8(__thiscall*)(void*,uint8,bool)>(target)(VP(0),uint8(r.next()),r.pct(50)); o=PlaneCommon(); o["ret"]=ret; }
    else { reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=PlaneCommon(); }   // Fix / SetupDamageAfterLoad
    return o; }

// ---- CBike ----
inline void InitBike(Rng& r){ InitVeh(0,r); At<void*>(VP(0),0x14)=nullptr; At<uint8>(VP(0),0x5C8)=uint8(r.below(2)); }
inline json CaseBikeSimple(Rng& r,uintptr_t target,const char* nm){
    InitBike(r); json o;
    if(!strcmp(nm,"dmgDrawCarCollidingParticles")){ CVector p{RandF(r),RandF(r),RandF(r)}; reinterpret_cast<void(__thiscall*)(void*,const CVector*,float,int32)>(target)(VP(0),&p,RandF(r),int32(r.below(60))); o=VehJson(1); }
    else if(!strcmp(nm,"KnockOffRider")){ void* ped= r.pct(25) ? nullptr : static_cast<void*>(VP(1)); void* ret=reinterpret_cast<void*(__thiscall*)(void*,int32,uint8,void*,bool)>(target)(VP(0),int32(r.below(60)),uint8(r.next()),ped,r.pct(50)); o=VehJson(1); o["ret"]=Name(uintptr_t(ret)); }
    else if(!strcmp(nm,"ReduceHornCounter")){ At<uint32>(VP(0),0x514)= r.pct(30) ? uint32(r.below(3)) : r.next(); reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=VehJson(1); }
    else if(!strcmp(nm,"GetAllWheelsOffGround")){ At<uint8>(VP(0),0x805)= r.pct(40) ? uint8(0) : uint8(r.next()); const uint8 ret=reinterpret_cast<uint8(__thiscall*)(void*)>(target)(VP(0)); o=VehJson(1); o["ret"]=ret; }
    else if(!strcmp(nm,"PlayHornIfNecessary")){ At<uint8>(VP(0),0x3DB)=uint8(r.next()); reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=VehJson(1); }
    else { reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=VehJson(1); }   // DebugCode
    return o; }
inline json CaseLean(Rng& r,uintptr_t target,Patches& P,uintptr_t dllCol){
    InitBike(r); FillMat(0,r); At<void*>(VP(0),0x14)=Mat(0);
    At<uint32>(VP(0),0x60C)=0; At<uint32>(VP(0),0x610)=0;                        // CMatrix de inclinacion sin RwMatrix propia (como el constructor)
    for(size_t off:{size_t(0x5CC)+0xC,size_t(0x5CC)+0x1C,size_t(0x5CC)+0x2C,size_t(0x5CC)+0x3C})At<uint32>(VP(0),off)=0;   // relleno de la CMatrix
    At<float>(VP(0),0x648)=r.pct(10) ? r.pick({0.f,-0.f,0.5f,-0.5f,1.f,-1.f,3.1f}) : r.uni(-2.5f,2.5f);              // LeanAngle
    for(size_t off:{size_t(0x5CC),size_t(0x5CC)+0x10,size_t(0x5CC)+0x20,size_t(0x5CC)+0x30})for(int c=0;c<3;++c)At<float>(VP(0),off+4*size_t(c))=r.uni(-5.f,5.f);
    std::memset(colm.data(),0,colm.size()); for(int c=0;c<6;++c)At<float>(colm.data(),4*size_t(c))=r.uni(-3.f,3.f);
    P.Add(0x535300,S_ColModel); P.Add(dllCol,S_ColModel);
    reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0));
    /* el EXE copia con rep movs el temporal de 0x59BE30 (relleno de pila sin escribir) a la matriz de inclinacion: se enmascara el relleno de sus 4 filas */
    for(size_t off:{size_t(0x5CC)+0xC,size_t(0x5CC)+0x1C,size_t(0x5CC)+0x2C,size_t(0x5CC)+0x3C})At<uint32>(VP(0),off)=0;
    json o=VehJson(1); o["lean"]=MatHash(VP(0)+0x5CC); o["flag"]=At<uint8>(VP(0),0x5C8); o["lean_pos"]=Fnv(VP(0)+0x5CC+0x30,12); o["entity_matrix"]=MatHash(Mat(0));
    return o; }
inline json CaseBuoyancy(Rng& r,uintptr_t target,Patches& P,uintptr_t dllBuoy,uintptr_t dllMove,uintptr_t dllTurn){
    InitBike(r); AtA<float>(0xB7CB5C)= r.pct(10) ? r.pick({0.f,0.005f,0.01f,1.f,0.02f}) : r.uni(0.f,4.f);
    At<float>(VP(0),0x8C)=r.pct(5)?r.pick({0.f,125.f}):r.uni(50.f,3000.f);     // masa
    At<float>(VP(0),0xA0)=r.pct(5)?r.pick({0.f,1.f}):r.uni(0.1f,8.f);          // constante de flotabilidad
    for(size_t off:{size_t(0x44),size_t(0x48),size_t(0x4C),size_t(0x50),size_t(0x54),size_t(0x58)})At<float>(VP(0),off)=r.uni(-2.f,2.f);
    for(int i=0;i<4;++i)At<float>(VP(0),0x710+4*size_t(i))= r.pct(45) ? 1.f : r.uni(0.f,0.999f);
    At<uint32>(VP(0),0x40)=r.next(); At<uint8>(VP(0),0x42B)=uint8(r.next()); At<uint8>(VP(0),0x614)=uint8(r.next());
    At<void*>(VP(0),0x460)=nullptr; At<uint8>(VP(0),0x488)=0;                      // sin conductor ni pasajeros (ver README)
    buoyRet=r.pct(85); buoyV[0]=CVector{RandF(r),RandF(r),RandF(r)}; buoyV[1]=CVector{r.uni(-3.f,3.f),r.uni(-3.f,3.f),r.pct(10)?0.f:r.uni(-5.f,40.f)};
    P.Add(0x6C3EF0,S_Buoy); P.Add(dllBuoy,S_Buoy); P.Add(0x5429F0,S_MoveForce); P.Add(dllMove,S_MoveForce); P.Add(0x542A50,S_TurnForce); P.Add(dllTurn,S_TurnForce);
    reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0));
    return VehJson(1); }
inline json CaseBikeVirt(Rng& r,uintptr_t target,const char* nm){
    InitBike(r); json o;
    if(!strcmp(nm,"Fix")||!strcmp(nm,"SetupDamageAfterLoad")){ reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=VehJson(1); }
    else if(!strcmp(nm,"RemoveRefsToVehicle")){ void* e=VP(1); for(int i=0;i<4;++i)At<void*>(VP(0),0x7C0+4*size_t(i))= r.pct(45) ? e : (r.pct(50)?static_cast<void*>(VP(2)):nullptr);
        reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(VP(0),r.pct(85)?e:static_cast<void*>(VP(3))); o=VehJson(1); }
    else if(!strcmp(nm,"ProcessOpenDoor")){ reinterpret_cast<void(__thiscall*)(void*,void*,uint32,uint32,uint32,float)>(target)(VP(0),VP(1),r.next(),r.next(),r.next(),RandF(r)); o=VehJson(1); }
    else if(!strcmp(nm,"ProcessDrivingAnims")){ At<uint32>(VP(0),0x1C)= r.pct(50) ? (At<uint32>(VP(0),0x1C)|0x20000u) : (At<uint32>(VP(0),0x1C)&~0x20000u);
        At<uint8>(VP(0),0x36)= r.pct(40) ? uint8(At<uint8>(VP(0),0x36)&7) : At<uint8>(VP(0),0x36);
        reinterpret_cast<void(__thiscall*)(void*,void*,bool)>(target)(VP(0),VP(1),r.pct(50)); o=VehJson(1); }
    else if(!strcmp(nm,"SetModelIndex")){ goN=uint32(r.next()%5); reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(VP(0),r.next()); o=VehJson(1); }
    else if(!strcmp(nm,"GetComponentWorldPosition")){
        for(int i=0;i<10;++i)At<void*>(VP(0),0x5A0+4*size_t(i))= r.pct(75) ? static_cast<void*>(&frames[size_t(i)*4]) : nullptr;
        for(auto& m:ltms)FillBytes(m.data(),m.size(),r);
        const int32 comp=int32(r.below(10)); if(!At<void*>(VP(0),0x5A0+4*size_t(comp)))At<void*>(VP(0),0x5A0+4*size_t(comp))=&frames[size_t(comp)*4];   // solo componentes presentes (la ausente solo escribe un log)
        CVector out{RandF(r),RandF(r),RandF(r)};
        reinterpret_cast<void(__thiscall*)(void*,int32,CVector*)>(target)(VP(0),comp,&out); o=VehJson(1); o["out"]=json::array({std::bit_cast<uint32>(out.x),std::bit_cast<uint32>(out.y),std::bit_cast<uint32>(out.z)}); }
    else if(!strcmp(nm,"Teleport")){ const bool withMat=r.pct(50); SetPlacement(0,r,withMat); At<uint32>(VP(0),0x40)=r.next();
        for(size_t off:{size_t(0x44),size_t(0x48),size_t(0x4C),size_t(0x50),size_t(0x54),size_t(0x58)})At<float>(VP(0),off)=r.uni(-2.f,2.f);
        const CVector d{RandF(r),RandF(r),RandF(r)};
        reinterpret_cast<void(__thiscall*)(void*,CVector,bool)>(target)(VP(0),d,r.pct(50)); o=VehJson(1); o["matrix"]=MatHash(Mat(0)); }
    else if(!strcmp(nm,"ProcessControlCollisionCheck")){ FillMat(0,r); At<void*>(VP(0),0x14)=Mat(0); chkN=0; for(auto& c:chk)c=r.pct(55);
        At<uint32>(VP(0),0x1C)=r.next(); At<uint32>(VP(0),0x40)=r.next();
        reinterpret_cast<void(__thiscall*)(void*,bool)>(target)(VP(0),r.pct(60)); o=VehJson(1); o["matrix"]=MatHash(Mat(0)); }
    else if(!strcmp(nm,"Render")){ rsSaved=int32(r.next()); AtA<uint32>(0xB7CB84)=r.next(); At<uint8>(VP(0),0x584)=uint8(r.next());
        reinterpret_cast<void(__thiscall*)(void*)>(target)(VP(0)); o=VehJson(1); }
    return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"vehwave2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehwave2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehwave2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"vehwave2-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,kRealFns> hooks{};
    for(size_t f=0;f<kRealFns;++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (TODOS antes de crear ningun EntryPatch: leccion 11) ---
    const uintptr_t dllWorldAdd=Body(&CWorld::Add), dllWorldRemove=Body(&CWorld::Remove);
    const uintptr_t dllFindVeh=Body(&FindPlayerVehicle);
    const uintptr_t dllPvl=Body(&CWorld::ProcessVerticalLine);
    const uintptr_t dllFindPos=Body(&CTrain::FindPositionOnTrackFromCoors);
    const uintptr_t dllClearScan=Body(&CWorld::ClearScanCodes);
    const uintptr_t dllHitV=Body(&TrainHitStuff<CPtrListDoubleLink<CVehicle*>>), dllHitP=Body(&TrainHitStuff<CPtrListDoubleLink<CPed*>>), dllHitO=Body(&TrainHitStuff<CPtrListDoubleLink<CObject*>>);
    uintptr_t dllVehRender=DllBody("CVehicle","Render"); if(!dllVehRender)dllVehRender=FirstDllCall(DllBody("CBike","Render"));
    uintptr_t dllAutoOpen=DllBody("CAutomobile","OpenDoor");
    uintptr_t dllVehSetModel=DllBody("CVehicle","SetModelIndex");
    const uintptr_t dllRider=Body(&CBike::ProcessRiderAnims);
    const uintptr_t dllFill=Body(&CClumpModelInfo::FillFrameArray);
    const uintptr_t dllGetFirst=Body(&GetFirstObject);
    const uintptr_t dllWheel=Body(&CVehicleModelInfo::GetWheelPosn);
    const uintptr_t dllCol=Body(&CEntity::GetColModel);
    const uintptr_t dllOrient=Body(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation));
    const uintptr_t dllCalcLean=Body(&CBike::CalculateLeanMatrix);
    const uintptr_t dllHead=Body(&CVehicle::DoHeadLightBeam);
    const uintptr_t dllLtm=Body(&RwFrameGetLTM);
    const uintptr_t dllMoveSpeed=Body(&CPhysical::ApplyMoveSpeed), dllTurnSpeed=Body(&CPhysical::ApplyTurnSpeed), dllSkip=Body(&CPhysical::SkipPhysics), dllCheck=Body(&CPhysical::CheckCollision);
    const uintptr_t dllMoveForce=Body(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)), dllTurnForce=Body(&CPhysical::ApplyTurnForce);
    const uintptr_t dllBuoy=Body(&cBuoyancy::ProcessBuoyancy);
    uintptr_t dllAutoCtor{};
    { const uintptr_t planeCtor=FirstDllCall(DllBody("CPlane","Constructor")); const uintptr_t a=planeCtor ? FirstDllCall(planeCtor) : 0;
      uintptr_t b=FirstDllCall(DllBody("CAutomobile","Constructor")); if(!b)b=Exported("??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z");
      if(!a||a!=b){ Write({{"event","error"},{"reason","CAutomobile constructor not found"},{"from_plane",a},{"from_other",b}}); return; } dllAutoCtor=a; }
    if(!dllVehRender||!dllAutoOpen||!dllVehSetModel){ Write({{"event","error"},{"reason","DLL body of a virtual collaborator not found"},{"render",dllVehRender},{"open",dllAutoOpen},{"setmodel",dllVehSetModel}}); return; }

    std::array<bool,kRealFns> enabled{},locked{};
    for(size_t f=0;f<kRealFns;++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtable falsa de vehiculos y nombres de buferes
    for(auto& v:vt)v=reinterpret_cast<void*>(&V_Trap); vt[0]=reinterpret_cast<void*>(&V_Dtor); vt[26]=reinterpret_cast<void*>(&V_IsComp); vt[51]=reinterpret_cast<void*>(&V_PlaneDmg); vt[54]=reinterpret_cast<void*>(&V_Horn);
    if(named.empty()){
        for(size_t i=0;i<vehs.size();++i){ static const char* nn[8]{"veh0","veh1","veh2","veh3","veh4","veh5","veh6","veh7"}; Nm(vehs[i].b,kVehSize,nn[i]); }
        for(size_t i=0;i<mats.size();++i){ static const char* nn[3]{"mat0","mat1","mat2"}; Nm(mats[i].data(),0x60,nn[i]); }
        Nm(colm.data(),colm.size(),"colm"); Nm(frames.data(),frames.size(),"frames");
        for(size_t i=0;i<delbuf.size();++i){ static const char* nn[4]{"node0","node1","node2","node3"}; Nm(delbuf[i].data(),16,nn[i]); } }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto Chk=[&](json& lay,const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      json lay=json::object();
      auto* Tr=reinterpret_cast<CTrain*>(VP(0)); auto* Pl=reinterpret_cast<CPlane*>(VP(0)); auto* Bk=reinterpret_cast<CBike*>(VP(0));
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      Chk(lay,"CTrain.m_fTrainSpeed",Off(Tr,&Tr->m_fTrainSpeed),0x5A4); Chk(lay,"CTrain.m_nTrainFlags",Off(Tr,&Tr->m_nTrainFlags),0x5B8);
      Chk(lay,"CTrain.m_pPrevCarriage",Off(Tr,&Tr->m_pPrevCarriage),0x5D0); Chk(lay,"CTrain.m_pNextCarriage",Off(Tr,&Tr->m_pNextCarriage),0x5D4);
      Chk(lay,"CVehicle.m_nVehicleType",Off(Tr,&Tr->m_nVehicleType),0x590); Chk(lay,"CVehicle.m_nVehicleSubType",Off(Tr,&Tr->m_nVehicleSubType),0x594);
      Chk(lay,"CVehicle.m_pDriver",Off(Tr,&Tr->m_pDriver),0x460); Chk(lay,"CVehicle.m_nMaxPassengers",Off(Tr,&Tr->m_nMaxPassengers),0x488);
      Chk(lay,"CVehicle.m_pHandlingData",Off(Tr,&Tr->m_pHandlingData),0x384); Chk(lay,"CVehicle.m_pFlyingHandlingData",Off(Tr,&Tr->m_pFlyingHandlingData),0x388);
      Chk(lay,"CVehicle.m_nTimeTillWeNeedThisCar",Off(Tr,&Tr->m_nTimeTillWeNeedThisCar),0x4D0);
      Chk(lay,"CPlane.m_fLeftRightSkid",Off(Pl,&Pl->m_fLeftRightSkid),0x988); Chk(lay,"CPlane.m_fSteeringFactor",Off(Pl,&Pl->m_fSteeringFactor),0x99C);
      Chk(lay,"CPlane.m_nStartedFlyingTime",Off(Pl,&Pl->m_nStartedFlyingTime),0x9C0); Chk(lay,"CPlane.m_fLandingGearStatus",Off(Pl,&Pl->m_fLandingGearStatus),0x9CC);
      Chk(lay,"CPlane.m_nFiringMultiplier",Off(Pl,&Pl->m_nFiringMultiplier),0x9D8); Chk(lay,"CPlane.m_pSmokeParticle",Off(Pl,&Pl->m_pSmokeParticle),0x9F8);
      Chk(lay,"CPlane.m_bSmokeEjectorEnabled",Off(Pl,&Pl->m_bSmokeEjectorEnabled),0xA00); Chk(lay,"sizeof(CPlane)",uint32(sizeof(CPlane)),0xA04);
      Chk(lay,"CBike.m_aBikeNodes",Off(Bk,Bk->m_aBikeNodes.data()),0x5A0); Chk(lay,"CBike.m_bLeanMatrixCalculated",Off(Bk,&Bk->m_bLeanMatrixCalculated),0x5C8);
      Chk(lay,"CBike.m_mLeanMatrix",Off(Bk,&Bk->m_mLeanMatrix),0x5CC); Chk(lay,"CBike.m_nWheelStatus",Off(Bk,Bk->m_nWheelStatus.data()),0x65C);
      Chk(lay,"CBike.m_BikeHandling",Off(Bk,&Bk->m_BikeHandling),0x63C); Chk(lay,"CBike.m_RideAnimData",Off(Bk,&Bk->m_RideAnimData),0x640);
      Chk(lay,"CBike.m_aWheelRatios",Off(Bk,Bk->m_aWheelRatios.data()),0x710); Chk(lay,"CBike.m_aGroundPhysicalPtrs",Off(Bk,Bk->m_aGroundPhysicalPtrs.data()),0x7C0);
      Chk(lay,"CBike.m_nNoOfContactWheels",Off(Bk,&Bk->m_nNoOfContactWheels),0x804); Chk(lay,"sizeof(CBike)",uint32(sizeof(CBike)),0x814);
      Chk(lay,"CAutomobile.m_wheelPosition",Off(Pl,Pl->m_wheelPosition.data()),0x838); Chk(lay,"sizeof(CTrain)",uint32(sizeof(CTrain)),0x6AC);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(0xB74494,4);        // puntero al pool de vehiculos
        Snap(0xC38010,4);        // bDisableRandomTrains
        Snap(0xC38024,16);       // pTrackNodes[4]
        Snap(0xB7CD78,2);        // codigo de escaneo actual
        Snap(0xC1CADC,8); Snap(0x8D33BC,1);   // GenPlane_Status / LastTimeGenerated / Active
        Snap(0xB7CB84,4); Snap(0xB7CB5C,4);   // CTimer::m_snTimeInMilliseconds / ms_fTimeStep
        Snap(0xB6F028,0x20);     // TheCamera (posicion y puntero a matriz)
        for(int32 m:{520,476,512,519,553,539,513,400,411,0})Snap(0xA9B0C8+4*uint32(m),4);
    }
    const uintptr_t renderFpAddr=AtA<uintptr_t>(0xC97B24)+0x20; const uintptr_t renderFpSaved=AtA<uintptr_t>(renderFpAddr);
    const uintptr_t renderGetAddr=AtA<uintptr_t>(0xC97B24)+0x24; const uintptr_t renderGetSaved=AtA<uintptr_t>(renderGetAddr);
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<kRealFns;++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F1Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); goN=0; chkN=0; visN=0; playerVeh=nullptr;
                hooks[f]->State(candidate);
                const bool switches= fn.address ? ReversingProbe::JumpsToDll(fn.address)==candidate : InDll(AtA<uintptr_t>(fn.vt+4*uint32(fn.slot)))==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                const uintptr_t target= fn.address ? fn.address : AtA<uintptr_t>(fn.vt+4*uint32(fn.slot));
                std::memset(guardLo.data(),0xCD,kGuard); std::memset(guardHi.data(),0xCD,kGuard);
                { Rng rs{stt^0xC0FFEEu}; for(int s=0;s<8;++s)InitVeh(s,rs); }
                for(int32 m:{520,476,512,519,553,539,513,400,411,0})AtA<void*>(0xA9B0C8+4*uint32(m))=Mat(2);   // model info "falso": solo se usa su identidad (GetWheelPosn esta sustituido)
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                Patches P;
                const char* n=nm.c_str();
                if(cls=="CTrain"){
                    if(nm=="Shutdown")out["state"]=CaseShutdown(r,target);
                    else if(nm=="DisableRandomTrains")out["state"]=CaseDisable(r,target);
                    else if(nm=="FindNumCarriagesPulled")out["state"]=CaseNumCarriages(r,target);
                    else if(nm=="FindSideStationIsOn")out["state"]=CaseSide(r,target);
                    else if(nm=="IsInTunnel")out["state"]=CaseTunnel(r,target,P,dllPvl);
                    else if(nm=="OpenTrainDoor"||nm=="AddPassenger"||nm=="RemovePassenger"||nm=="ProcessTrainAnnouncements"||nm=="PlayAnnouncement"||nm=="UpdateTrains")out["state"]=CaseNop(r,target,n);
                    else if(nm=="ReleaseOneMissionTrain")out["state"]=CaseReleaseOne(r,target);
                    else if(nm=="RemoveOneMissionTrain"){ P.Add(0x563280,S_WorldRemove); P.Add(dllWorldRemove,S_WorldRemove); out["state"]=CaseRemoveOne(r,target); }
                    else if(nm=="SetTrainSpeed")out["state"]=CaseSpeed(r,target);
                    else if(nm=="SetTrainCruiseSpeed")out["state"]=CaseCruise(r,target);
                    else if(nm=="SetNewTrainPosition")out["state"]=CaseNewPos(r,target,P,dllFindPos);
                    else if(nm=="RemoveMissionTrains"||nm=="ReleaseMissionTrains"){
                        P.Add(0x56E0D0,S_FindPlayerVeh); P.Add(dllFindVeh,S_FindPlayerVeh); P.Add(0x563280,S_WorldRemove); P.Add(dllWorldRemove,S_WorldRemove);
                        out["state"]=CasePoolMission(r,target); }
                    else if(nm=="MarkSurroundingEntitiesForCollisionWithTrain"){
                        P.Add(0x563470,S_ClearScan); P.Add(dllClearScan,S_ClearScan); P.Add(0x6F5CF0,S_Hit); P.Add(dllHitV,S_Hit); P.Add(dllHitP,S_Hit); P.Add(dllHitO,S_Hit);
                        out["state"]=CaseMark(r,target); }
                }
                else if(cls=="CPlane"){
                    if(nm=="Constructor"){
                        P.Add(0x6B0A90,S_AutoCtor); P.Add(dllAutoCtor,S_AutoCtor); P.Add(0x4C7D20,S_Wheel); P.Add(dllWheel,S_Wheel); P.Add(0x7348C0,S_GetFirstObject); P.Add(dllGetFirst,S_GetFirstObject);
                        out["state"]=CasePlaneCtor(r,target); }
                    else if(nm=="InitPlaneGenerationAndRemoval")out["state"]=CasePlaneInit(r,target);
                    else if(nm=="CountPlanesAndHelis")out["state"]=CaseCountPlanes(r,target);
                    else if(nm=="AreWeInNoPlaneZone")out["state"]=CaseZone(r,target,false);
                    else if(nm=="AreWeInNoBigPlaneZone")out["state"]=CaseZone(r,target,true);
                    else if(nm=="IsAlreadyFlying"||nm=="SetGearUp"||nm=="SetGearDown")out["state"]=CasePlaneSimple(r,target,n);
                    else {
                        if(nm=="Render"){ P.Add(0x6D0E60,S_VehRender); P.Add(dllVehRender,S_VehRender); }
                        if(nm=="OpenDoor"){ P.Add(0x6A6AE0,S_AutoOpenDoor); P.Add(dllAutoOpen,S_AutoOpenDoor); }
                        out["state"]=CasePlaneVirt(r,target,n); }
                }
                else if(cls=="CBike"){
                    if(nm=="CalculateLeanMatrix")out["state"]=CaseLean(r,target,P,dllCol);
                    else if(nm=="ProcessBuoyancy")out["state"]=CaseBuoyancy(r,target,P,dllBuoy,dllMoveForce,dllTurnForce);
                    else if(fn.address)out["state"]=CaseBikeSimple(r,target,n);
                    else {
                        if(nm=="SetModelIndex"){ P.Add(0x6D6A40,S_VehSetModel); P.Add(dllVehSetModel,S_VehSetModel); P.Add(0x4C5440,S_FillFrames); P.Add(dllFill,S_FillFrames); }
                        if(nm=="GetComponentWorldPosition"){ P.Add(0x7F0990,S_RwLtm); P.Add(dllLtm,S_RwLtm); }
                        if(nm=="ProcessDrivingAnims"){ P.Add(0x6B7280,S_RiderAnims); P.Add(dllRider,S_RiderAnims); }
                        if(nm=="Teleport"){ P.Add(0x563280,S_WorldRemove); P.Add(dllWorldRemove,S_WorldRemove); P.Add(0x563220,S_WorldAdd); P.Add(dllWorldAdd,S_WorldAdd); P.Add(0x439A80,S_SetOrient); P.Add(dllOrient,S_SetOrient); }
                        if(nm=="ProcessControlCollisionCheck"){ P.Add(0x542DD0,S_ApplyMove); P.Add(dllMoveSpeed,S_ApplyMove); P.Add(0x542E20,S_ApplyTurn); P.Add(dllTurnSpeed,S_ApplyTurn);
                            P.Add(0x5433B0,S_Skip); P.Add(dllSkip,S_Skip); P.Add(0x54D920,S_CheckColl); P.Add(dllCheck,S_CheckColl); }
                        if(nm=="Render"){ P.Add(0x6D0E60,S_VehRender); P.Add(dllVehRender,S_VehRender); P.Add(0x6B7150,S_CalcLean); P.Add(dllCalcLean,S_CalcLean); P.Add(0x6E0E20,S_Headlight); P.Add(dllHead,S_Headlight);
                            AtA<uintptr_t>(renderFpAddr)=reinterpret_cast<uintptr_t>(&S_RenderState); AtA<uintptr_t>(renderGetAddr)=reinterpret_cast<uintptr_t>(&S_RenderGet); }
                        out["state"]=CaseBikeVirt(r,target,n);
                        AtA<uintptr_t>(renderFpAddr)=renderFpSaved; AtA<uintptr_t>(renderGetAddr)=renderGetSaved; }
                }
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            (void)dllMoveSpeed; (void)dllTurnSpeed;
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            if(nm=="Constructor"&&cls=="CPlane")ok=ok&&original["state"]["vptr_ok"]==true&&original["state"]["ret_is_this"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state"))for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<kRealFns;++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    AtA<uintptr_t>(renderFpAddr)=renderFpSaved; AtA<uintptr_t>(renderGetAddr)=renderGetSaved;
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP&&AtA<uintptr_t>(renderFpAddr)==renderFpSaved&&AtA<uintptr_t>(renderGetAddr)==renderGetSaved;
    for(size_t f=0;f<kRealFns;++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
