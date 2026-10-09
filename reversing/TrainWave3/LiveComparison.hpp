#pragma once
// Banco dentro del motor (flujo viejo, estilo VehWave2/PadMouse1): CTrain::ProcessControl (virtual, hueco 10 de la vtable 0x872370, EXE 0x6F86A0).
// Original (hook de la virtual desactivado: el hueco apunta al EXE) frente a candidato (hook activo: el hueco apunta a la DLL) sobre un tren falso
// (bufer de 0x6C0 bytes con vtable falsa) con carriages falsos enlazados por +0x5D0/+0x5D4, una via falsa (nodos de 10 bytes, longitud total y
// numero de nodos en las tablas globales 0xC38024/0xC38014/0xC37FEC) y un modelo falso (caja de colision y datos de handling falsos).
// Se compara el estado completo de todos los trenes falsos (hash byte a byte, sin el relleno de las matrices), la matriz del sujeto, la via, los
// globales tocados, la palabra de control x87 y la traza completa de llamadas salientes con sus argumentos.
//
// Colaboradores sustituidos y trazados (EntryPatch en la direccion del EXE y en el cuerpo de la DLL, solo durante la ejecucion):
//   CAEVehicleAudioEntity::Service (0x502280) y AddAudioEvent (0x4F6420), CPad::GetAccelerate/GetBrake/StartShake (0x5403F0/0x540080/0x53F920),
//   CPed::IsPlayer (0x5DF8F0), CPlayerPed::GetPadFromPlayer (0x609560), CTrain::FindNumCarriagesPulled (0x6F5CD0), FindMaximumSpeedToStopAtStations
//   (0x6F5BA0), IsInTunnel (0x6F6320), RemoveRandomPassenger (0x6F6850), AddNearbyPedAsRandomPassenger (0x6F8170),
//   CGameLogic::CalcDistanceToForbiddenTrainCrossing (0x4418E0), CCamera::CamShake (0x50A9F0), rand del EXE (0x821B1E) y CGeneral::GetRandomNumber,
//   FindPlayerPed (0x56E210), CCarCtrl::ScanForPedDanger (0x42CE40), CPhysical::ProcessControl (0x5485E0 y el cuerpo VMT de la DLL),
//   CPhysical::ApplyMoveSpeed/SkipPhysics/ApplyMoveForce/ApplyTurnForce/RemoveAndAdd (0x542DD0/0x5433B0/0x5429F0/0x542A50/0x542560),
//   CEntity::UpdateRwFrame (0x532B00), cBuoyancy::ProcessBuoyancy (0x6C3EF0), MarkSurroundingEntitiesForCollisionWithTrain (0x6F6640),
//   CTrainNode::GetLightingFromCollision (0x6F5F80) y tColLighting::GetCurrentLighting (0x59F0C0, el EXE lo llama como funcion estatica).
//   CEntity::SetIsStatic (virtual hueco 4) se traza en la vtable falsa. No se sustituyen (son puras y se comparan de verdad): CVector::Normalise
//   (0x59C910), CrossProduct (0x59C730), CPlaceable::GetForwardVector (0x41CCB0), CPad::GetPad (0x53FB70, devuelve &Pads[0]), pow (0x822130).
//
// El pool de vehiculos, los pools de entidades, los pads y la camara reales no se tocan (UpdateRwMatrix no interviene: m_pRwObject es nulo en el
// tren falso). Estado global guardado y restaurado: CTimer (0xB7CB84/0xB7CB5C), gbModelViewer (0xBA6728), las tablas de via (0xC37FEC/0xC38014/0xC38024)
// y las entradas de model info de los modelos usados. Los stubs solo actuan sobre el hilo del banco (owner/Mine()).
#include <fstream>
#include <set>
#include <cmath>
#include <bit>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Train.h"
#include "Entity/Ped/PlayerPed.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "Collision/ColPoint.h"
#include "TrainNode.h"
#include "Buoyancy.h"
#include "CarCtrl.h"
#include "GameLogic.h"
#include "General.h"
#include "Camera.h"
#include "Pad.h"
#include "Timer.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace TrainWave3Live {
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

// Hilo del banco: otros hilos del juego (audio, etc.) pueden llamar a funciones parcheadas; sus llamadas no se trazan.
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

// --- modulo de la DLL y busqueda de cuerpos (como VehWave2) ---
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
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(std::bit_cast<uintptr_t>(f)); }

// ======================= objetos falsos =======================
constexpr size_t kGuard=32, kVehSize=0x6C0;
struct alignas(16) Veh { uint8 b[kVehSize]; };
alignas(16) inline std::array<uint8,kGuard> guardLo{};
inline std::array<Veh,8> vehs{};                                  // 0..3 cadena de carriages, 6 = conductor falso, 7 = entidad de dano falsa
alignas(16) inline std::array<uint8,kGuard> guardHi{};
alignas(16) inline std::array<uint8,0x60> matBuf{};               // CMatrix falsa (derecha 0, adelante 0x10, arriba 0x20, posicion 0x30, RwMatrix* 0x40, posee 0x44)
alignas(16) inline std::array<uint8,0x40> colm{};                 // CColModel falso (caja: min 0, max 0xC)
alignas(16) inline std::array<uint8,0x100> handl{};               // tHandlingData falso (+0xBC = limite inferior de la suspension)
alignas(16) inline std::array<std::array<uint8,0x40>,6> modelInfos{};   // CBaseModelInfo falsos (+0x14 = CColModel*)
alignas(16) inline std::array<uint8,0x300> nodeMem{};             // via falsa: hasta 72 nodos de 10 bytes mas dos nodos de holgura (el EXE lee nodo n y n-1)
alignas(16) inline std::array<uint8,0x200> fakePad{};             // CPad falso del conductor jugador (solo identidad)
alignas(16) inline std::array<uint8,0x600> fakePlayer{};          // CPlayerPed falso (+0x58C = m_pVehicle)
inline std::array<void*,96> vt{};                                 // vtable falsa de vehiculos
constexpr std::array<int32,6> kModels{537,538,569,570,590,449};
inline uint8* VP(int i){ return vehs[size_t(i)].b; }
inline void* Mat(){ return matBuf.data(); }

// ---- configuracion del caso compartida con los dobles ----
inline int16 cfgAccel{}, cfgBrake{};
inline bool cfgDriverPlayer{}, cfgMaxRet{}, cfgBuoyRet{};
inline float cfgMaxSpeed{};
inline uint32 cfgNumCarr{}, tunnelMask{}, calcN{}, randN{};
inline std::array<float,2> calcRet{};
inline std::array<CVector,2> calcOut{};
inline std::array<uint32,8> randV{};
inline CVector buoyPoint{}, buoyForce{};

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
// miembros thiscall -> __fastcall(self, edx, args...); el que llama a ambos lados es el mismo codigo, asi que la pila coincide con el original.
inline void __fastcall V_Dtor(void* self,void*,uint32 flags){ if(Mine())T("dtor",self,flags); }
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
inline void __fastcall V_SetStatic(void* self,void*,uint32 on){ if(Mine())T("SetIsStatic",self,uint8(on)); }       // CEntity::SetIsStatic (hueco 4, ret 4)
inline void __fastcall S_Service(void* self,void*){ if(Mine())T("Service",self); }
inline void __fastcall S_AudioEvent(void* self,void*,int32 ev,float p){ if(Mine())T("AddAudioEvent",self,ev,p); }
inline int16 __fastcall S_Accel(void* self,void*){ if(!Mine())return 0; T("GetAccelerate",self); return cfgAccel; }
inline int16 __fastcall S_Brake(void* self,void*){ if(!Mine())return 0; T("GetBrake",self); return cfgBrake; }
inline void __fastcall S_StartShake(void* self,void*,int32 time,int32 freq,uint32 arg){ if(Mine())T("StartShake",self,time,freq,arg); }
inline uint8 __fastcall S_IsPlayer(void* self,void*){ if(!Mine())return 0; T("IsPlayer",self); return cfgDriverPlayer; }
inline void* __fastcall S_PadFromPlayer(void* self,void*){ if(!Mine())return nullptr; T("GetPadFromPlayer",self); return fakePad.data(); }
inline uint32 __fastcall S_NumCarriages(void* self,void*){ if(!Mine())return 0; T("FindNumCarriagesPulled",self); return cfgNumCarr; }
inline uint8 __fastcall S_FindMax(void* self,void*,float* out){ if(!Mine())return 0; T("FindMaximumSpeedToStopAtStations",self); *out=cfgMaxSpeed; return cfgMaxRet; }
inline uint8 __fastcall S_Tunnel(void* self,void*){ if(!Mine())return 0; const uint32 k=uint32((uintptr_t(self)-uintptr_t(vehs.data()))/sizeof(Veh)); T("IsInTunnel",self); return uint8((tunnelMask>>(k&31))&1); }
inline void __fastcall S_RemovePassenger(void* self,void*){ if(!Mine())return; T("RemoveRandomPassenger",self);
    uint8& b=At<uint8>(self,0x5CB); if(b&0xF)b=uint8((b&0xF0)|((b&0xF)-1)); }           // el original resta un pasajero que se baja
inline void __fastcall S_AddPassenger(void* self,void*){ if(!Mine())return; T("AddNearbyPedAsRandomPassenger",self);
    uint8& b=At<uint8>(self,0x5CB); if(b>>4)b=uint8((b&0x0F)|(((b>>4)-1)<<4)); }        // y uno que se sube
// CGameLogic::CalcDistanceToForbiddenTrainCrossing(CVector, CVector, bool, CVector&) cdecl, devuelve float en ST0
inline float __cdecl S_CalcDist(float px,float py,float pz,float sx,float sy,float sz,uint32 b,float* out){
    if(!Mine())return 1e9f; const size_t k=calcN++&1; T("CalcDistanceToForbiddenTrainCrossing",px,py,pz,sx,sy,sz,uint8(b),uint32(k));
    out[0]=calcOut[k].x; out[1]=calcOut[k].y; out[2]=calcOut[k].z; return calcRet[k]; }
inline void __fastcall S_CamShake(void* self,void*,float s,float x,float y,float z){ if(Mine())T("CamShake",self,s,x,y,z); }
inline int __cdecl S_Rand(){ if(!Mine())return int(GetTickCount()&0x7FFF); const uint32 v=randV[(randN++)&7]; T("rand",v); return int(v); }
inline uint16 __cdecl S_RandDll(){ if(!Mine())return uint16(GetTickCount()&0x7FFF); const uint32 v=randV[(randN++)&7]; T("rand",v); return uint16(v); }
inline void* __cdecl S_FindPed(int32 id){ if(!Mine())return CWorld::Players[CWorld::PlayerInFocus].m_pPed; T("FindPlayerPed",id); return fakePlayer.data(); }
inline void __cdecl S_ScanDanger(void* veh){ if(Mine())T("ScanForPedDanger",veh); }
inline void __fastcall S_PhysProc(void* self,void*){ if(Mine())T("CPhysical_ProcessControl",self); }
inline void __fastcall S_ApplyMove(void* self,void*){ if(Mine())T("ApplyMoveSpeed",self); }
inline void __fastcall S_Skip(void* self,void*){ if(Mine())T("SkipPhysics",self); }
inline void __fastcall S_RemoveAndAdd(void* self,void*){ if(Mine())T("RemoveAndAdd",self); }
inline void __fastcall S_UpdateFrame(void* self,void*){ if(Mine())T("UpdateRwFrame",self); }
inline void __fastcall S_MoveForce(void* self,void*,float x,float y,float z){ if(Mine())T("ApplyMoveForce",self,x,y,z); }
inline void __fastcall S_TurnForce(void* self,void*,float fx,float fy,float fz,float px,float py,float pz){ if(Mine())T("ApplyTurnForce",self,fx,fy,fz,px,py,pz); }
// cBuoyancy::ProcessBuoyancy(CPhysical*, float, CVector* puntoDeGiro, CVector* fuerza) thiscall ret 0x10 (this = 0xC1C890)
inline uint8 __fastcall S_Buoy(void* self,void*,void* ent,float c,float* turn,float* force){
    if(!Mine())return 0; T("ProcessBuoyancy",self,ent,c);
    turn[0]=buoyPoint.x; turn[1]=buoyPoint.y; turn[2]=buoyPoint.z; force[0]=buoyForce.x; force[1]=buoyForce.y; force[2]=buoyForce.z; return cfgBuoyRet; }
// MarkSurroundingEntitiesForCollisionWithTrain(CVector pos, float radius, CEntity*, bool) cdecl
inline void __cdecl S_Mark(float x,float y,float z,float r,void* e,uint32 b){ if(Mine())T("MarkSurroundingEntitiesForCollisionWithTrain",x,y,z,r,e,uint8(b)); }
// CTrainNode::GetLightingFromCollision (thiscall, devuelve el byte de iluminacion; los nodos del banco ya tienen m_bSurfLightingFound = 1)
inline uint32 __fastcall S_NodeLight(void* self,void*){ if(Mine())T("GetLightingFromCollision",self); return At<uint8>(self,8); }
// En la DLL tColLighting (constructor propio) se devuelve por puntero oculto: thiscall con un argumento de pila (ret 4) y EAX = ese puntero
// (la llamada posterior GetCurrentLighting usa EAX como this). El EXE lo devuelve por valor en AL (S_NodeLight).
inline uint8* __fastcall S_NodeLightDll(void* self,void*,uint8* ret){ if(Mine())T("GetLightingFromCollision",self); *ret=At<uint8>(self,8); return ret; }
// tColLighting::GetCurrentLighting: en el EXE es estatica cdecl(tColLighting por valor en un dword, float); en la DLL es miembro (this = &valor, float)
inline float LightValue(uint32 l,float s){ return (float((l&0xF)*3u+((l>>4)&0xF))*s)*(1.f/17.f); }
inline float __cdecl S_LightExe(uint32 l,float s){ if(Mine())T("GetCurrentLighting",l&0xFFu,s); return LightValue(l&0xFFu,s); }
inline float __fastcall S_LightDll(void* self,void*,float s){ const uint32 l=At<uint8>(self,0); if(Mine())T("GetCurrentLighting",l,s); return LightValue(l,s); }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,1> fns{{
    {"CTrain","ProcessControl",0,0x872370,10,1024},
}};
inline constexpr size_t kRealFns=1;

// ======================= estado comun =======================
inline json VehJson(int n){ json o=json::object(); for(int i=0;i<n;++i)o["veh"+std::to_string(i)]=Fnv(VP(i),kVehSize);
    o["guard"]=Fnv(guardLo.data(),kGuard,Fnv(guardHi.data(),kGuard)); return o; }
inline uint64 MatHash(const void* m){ const auto* b=static_cast<const uint8*>(m); uint64 h=1469598103934665603ull;   // sin el relleno de la CMatrix
    for(size_t off:{size_t(0),size_t(0x10),size_t(0x20),size_t(0x30)})h=Fnv(b+off,12,h); return Fnv(b+0x40,8,h); }
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline void FillMat(Rng& r){ FillBytes(matBuf.data(),matBuf.size(),r);
    for(int row=0;row<4;++row){ for(int c=0;c<3;++c)At<float>(matBuf.data(),0x10*size_t(row)+4*size_t(c))=r.pct(10)?(row==c?1.f:0.f):r.uni(-2.f,2.f); At<uint32>(matBuf.data(),0x10*size_t(row)+12)=0; }
    for(int c=0;c<3;++c)At<float>(matBuf.data(),0x30+4*size_t(c))=r.uni(-200.f,200.f);
    At<uint32>(matBuf.data(),0x40)=0; At<uint32>(matBuf.data(),0x44)=0; }

// ---- via falsa: poligono cerrado de n nodos; distancias acumuladas estrictamente crecientes ----
struct TrackInfo { int n; float total; };
inline TrackInfo MakeTrack(Rng& r,uint32 id){
    std::memset(nodeMem.data(),0,nodeMem.size());
    const int n=r.pct(10) ? int(60+r.below(13)) : int(6+r.below(40));            // 6..72 (los de 60+ dejan curvas suaves: producto escalar > 0.996)
    const float cx=r.uni(-1000.f,1000.f), cy=r.uni(-1000.f,1000.f), rad=r.uni(100.f,1200.f), z0=r.uni(0.f,50.f);
    float L=0.f; float px=0.f,py=0.f,pz=0.f; uint32 last=0;
    for(int i=0;i<n;++i){
        const float ang=(6.2831853f*float(i))/float(n)+(r.pct(30)?r.uni(-0.02f,0.02f):0.f);
        const float rr=rad*(r.pct(50)?1.f:r.uni(0.85f,1.15f));
        uint8* nd=nodeMem.data()+10*size_t(i);
        const int16 x=int16(std::lround((cx+rr*std::cos(ang))*8.f)), y=int16(std::lround((cy+rr*std::sin(ang))*8.f)), z=int16(std::lround((z0+r.uni(0.f,6.f))*8.f));
        At<int16>(nd,0)=x; At<int16>(nd,2)=y; At<int16>(nd,4)=z;
        const float fx=float(x)*0.125f, fy=float(y)*0.125f, fz=float(z)*0.125f;
        if(i>0){ const float dx=fx-px, dy=fy-py, dz=fz-pz; L+=std::max(std::sqrt((dz*dz+dy*dy)+dx*dx),2.f); }
        uint32 d=uint32(L*3.f); if(i>0&&d<=last)d=last+1; last=d; L=std::max(L,float(d)*(1.f/3.f));
        At<uint16>(nd,6)=uint16(d); At<uint8>(nd,8)=uint8(r.next()); At<uint8>(nd,9)=1;      // iluminacion ya encontrada
        px=fx; py=fy; pz=fz;
    }
    const float total=float(last)*(1.f/3.f)+r.uni(2.f,100.f);
    AtA<int32>(0xC38014+4*id)=n; AtA<float>(0xC37FEC+4*id)=total; AtA<void*>(0xC38024+4*id)=nodeMem.data();
    return {n,total}; }
inline float NodeDist(int i){ return float(At<uint16>(nodeMem.data()+10*size_t(i),6))*std::bit_cast<float>(0x3EAAAAABu); }

// ---- una escena de ProcessControl ----
enum Scen { SModelViewer, SRearPrev, SRearFree, SFrontAuto, SFrontPlayer, SPhys };
inline json CaseTrain(Rng& r,uintptr_t target){
    const uint32 sc=r.below(100);
    const Scen scen= sc<3 ? SModelViewer : sc<25 ? SRearPrev : sc<31 ? SRearFree : sc<56 ? SFrontAuto : sc<74 ? SFrontPlayer : SPhys;
    int n=1+int(r.below(4)); if(scen==SRearPrev)n=std::max(n,2);
    const int subj= scen==SRearPrev ? 1+int(r.below(uint32(n-1))) : scen==SPhys ? int(r.below(uint32(n))) : 0;
    // cadena de carriages 0..n-1 (+0x5D0 anterior, +0x5D4 siguiente); el resto de ranuras se inicializa pero queda fuera de la cadena
    for(int i=0;i<8;++i){ FillBytes(VP(i),kVehSize,r); At<void*>(VP(i),0)=vt.data(); At<void*>(VP(i),0x14)=nullptr; At<void*>(VP(i),0x18)=nullptr;
        At<void*>(VP(i),0x5D4)= (i+1<n) ? VP(i+1) : nullptr; At<void*>(VP(i),0x5D0)= (i>0&&i<n) ? VP(i-1) : nullptr;
        At<int32>(VP(i),0x590)=6; At<uint8>(VP(i),0x36)=uint8((r.below(6)<<3)|2); At<uint16>(VP(i),0x5B8)=uint16(r.next()&0x7FF);
        At<uint8>(VP(i),0x5CA)=uint8(r.below(5)); At<uint8>(VP(i),0x429)=uint8(r.next()); At<void*>(VP(i),0x460)=nullptr; At<void*>(VP(i),0xDC)=nullptr; At<void*>(VP(i),0x384)=handl.data(); }
    uint8* v=VP(subj);
    // ---- modelo, caja de colision, handling ----
    const int32 model= r.pct(45) ? 570 : kModels[r.below(6)];
    At<int16>(v,0x22)=int16(model);
    At<float>(colm.data(),0)=-r.uni(1.f,2.f); At<float>(colm.data(),4)=-r.uni(5.f,15.f); At<float>(colm.data(),8)=-r.uni(0.5f,3.f);
    At<float>(colm.data(),0xC)=r.uni(1.f,2.f); At<float>(colm.data(),0x10)=r.uni(5.f,15.f); At<float>(colm.data(),0x14)=r.uni(1.f,4.f);
    FillBytes(handl.data(),handl.size(),r); At<float>(handl.data(),0xBC)=r.uni(-1.f,1.f);
    // ---- via, posicion en la via ----
    const uint32 trackId=r.below(4); At<uint8>(v,0x5C0)=uint8(trackId);
    const TrackInfo tk=MakeTrack(r,trackId);
    for(int i=0;i<8;++i){ At<float>(VP(i),0x5A4)=r.uni(-1.6f,1.6f); At<float>(VP(i),0x5A8)=r.uni(0.f,tk.total); At<float>(VP(i),0x5AC)=r.uni(5.f,25.f); }   // carriage anterior: velocidad y distancia finitas
    const uint32 kk=r.below(100);
    const float rail= kk<4 ? 0.f : kk<7 ? tk.total*0.999f : kk<9 ? tk.total : kk<11 ? -1.f : kk<13 ? tk.total-0.01f : r.uni(0.f,tk.total);
    int idx=0; for(int i=0;i<tk.n;++i)if(NodeDist(i)<=rail)idx=i;
    if(!r.pct(60))idx=int(r.below(uint32(tk.n)));
    At<int16>(v,0x5A0)=int16(idx);
    At<float>(v,0x5A8)=rail; At<float>(v,0x5AC)=r.uni(5.f,25.f);
    // ---- velocidad del tren ----
    const float spd= r.pct(25) ? r.pick({0.f,0.0009f,-0.0009f,0.001f,-0.001f,0.95f,0.96f,1.f,1.01f,-1.2f,0.5f,-0.5f}) : r.uni(-1.6f,1.6f);
    At<float>(v,0x5A4)=spd; At<float>(v,0x5B0)=r.uni(-300.f,300.f); At<float>(v,0x5B4)=r.uni(-300.f,300.f);
    // ---- estado de la entidad ----
    const int status= scen==SFrontPlayer ? 0 : scen==SFrontAuto ? r.pickI({1,2,3,4,5,7}) : r.pct(40) ? 0 : r.pickI({1,2,3,4,5});
    At<uint8>(v,0x36)=uint8((status<<3)|2);
    At<uint8>(v,0x3D0)=uint8(r.next());
    const bool front= scen==SFrontAuto||scen==SFrontPlayer||(scen==SPhys&&r.pct(40))||scen==SModelViewer;
    uint16 f=0; auto B=[&](int bit,bool on){ if(on)f|=uint16(1u<<bit); };
    B(0,r.pct(75)); B(1,r.pct(50)); B(2,r.pct(50)); B(3,front); B(4,r.pct(30)); B(5,r.pct(8)); B(6,r.pct(50)); B(7,r.pct(60)); B(8,scen==SPhys); B(9,r.pct(30)); B(10,r.pct(40));
    At<uint16>(v,0x5B8)=f;
    At<uint8>(v,0x5CA)=uint8(r.below(5)); At<uint8>(v,0x5CB)=uint8(r.next());
    At<uint32>(v,0x5BC)=0;
    // ---- temporizador ----
    const float ts= r.pct(15) ? r.pick({1.f,0.5f,2.f,0.1f,4.f}) : r.uni(0.1f,4.f);
    const uint32 now=r.next()>>1; AtA<float>(0xB7CB5C)=ts; AtA<uint32>(0xB7CB84)=now;
    { const bool streak=(f&0x400)!=0; const uint32 t1=streak?20000u:10000u, t2=streak?28000u:18000u, t3=streak?32000u:22000u;
      const uint32 k=r.below(100); uint32 diff;
      if(k<20)diff=r.below(t1); else if(k<40)diff=t1+r.below(t2-t1); else if(k<60)diff=t2+r.below(t3-t2); else if(k<85)diff=t3+r.below(20000);
      else diff=r.pickI({int32(t1-1),int32(t1),int32(t2-1),int32(t2),int32(t3-1),int32(t3),0});
      At<uint32>(v,0x5BC)= r.pct(2) ? now+1u : now-diff; }
    // ---- posicion y matriz del sujeto ----
    const bool withMat= scen!=SPhys || r.pct(60);
    FillMat(r);
    if(r.pct(40)){ uint8* nd=nodeMem.data()+10*size_t(idx); At<float>(matBuf.data(),0x30)=float(At<int16>(nd,0))*0.125f+r.uni(-1.5f,1.5f); At<float>(matBuf.data(),0x34)=float(At<int16>(nd,2))*0.125f+r.uni(-1.5f,1.5f); At<float>(matBuf.data(),0x38)=float(At<int16>(nd,4))*0.125f+r.uni(-1.5f,1.5f); }
    At<void*>(v,0x14)= withMat ? Mat() : nullptr;
    for(int c=0;c<3;++c)At<float>(v,4+4*size_t(c))=r.uni(-300.f,300.f);              // m_placement
    At<float>(v,0x10)=r.uni(-3.f,3.f);
    // ---- fisica ----
    At<float>(v,0x8C)=r.uni(2000.f,40000.f); At<float>(v,0xA0)=r.uni(0.f,2.f);
    const bool still=r.pct(50);
    for(size_t o:{size_t(0x44),size_t(0x48),size_t(0x4C),size_t(0x50),size_t(0x54),size_t(0x58),size_t(0x74),size_t(0x78),size_t(0x7C),size_t(0x80),size_t(0x84),size_t(0x88)})
        At<float>(v,o)= still&&r.pct(80) ? 0.f : r.uni(-0.02f,0.02f);
    At<float>(v,0xD4)= still ? r.pick({0.f,0.001f,0.004f,0.005f,0.0051f,0.014f,0.015f,0.016f}) : r.uni(0.f,0.03f);
    At<float>(v,0xD8)= r.pct(50) ? 0.f : r.uni(0.f,5.f);
    At<uint8>(v,0xB8)=uint8(r.pickI({0,1,5,8,9,10,11,200,255}));
    { uint32& fl=At<uint32>(v,0x1C); fl=r.next(); if(scen==SPhys&&r.pct(30))fl|=0x10u; else fl&=~0x10u; }
    if(scen==SPhys&&r.pct(60)){ At<void*>(v,0xDC)=VP(7); At<uint8>(VP(7),0x36)=uint8(r.pct(60)?((r.below(5)<<3)|3):((r.below(5)<<3)|2)); }
    // ---- conductor, pad, jugador ----
    if(front&&r.pct(60))At<void*>(v,0x460)=VP(6);
    cfgDriverPlayer=r.pct(60);
    cfgAccel=int16(r.pickI({0,255,-255,128,100,3,-3,-128,1})); cfgBrake=int16(r.pickI({0,255,-255,128,100,3,-3,64,1}));
    cfgNumCarr=r.below(6); tunnelMask=r.pct(35) ? r.next() : 0u;
    cfgMaxRet=r.pct(30); cfgMaxSpeed= r.pct(20) ? 100000.f : r.uni(0.f,120.f);
    for(size_t k=0;k<2;++k){ calcRet[k]=r.pick({10.f,100.f,229.9f,230.f,231.f,500.f,1e9f,0.f,-5.f}); calcOut[k]=CVector{r.uni(-3.f,3.f),r.uni(-3.f,3.f),r.uni(-3.f,3.f)}; }
    calcN=0; randN=0; for(auto& x:randV)x=r.next()&0x7FFF;
    cfgBuoyRet=r.pct(50); buoyPoint=CVector{r.uni(-4.f,4.f),r.uni(-4.f,4.f),r.uni(-4.f,4.f)}; buoyForce=CVector{r.uni(-5.f,5.f),r.uni(-5.f,5.f),r.uni(-5.f,5.f)};
    At<void*>(fakePlayer.data(),0x58C)= r.pct(35) ? static_cast<void*>(v) : r.pct(50) ? static_cast<void*>(VP(0)) : nullptr;
    AtA<uint8>(0xBA6728)= scen==SModelViewer ? 1 : 0;

    reinterpret_cast<void(__thiscall*)(void*)>(target)(v);

    json o=VehJson(8); o["matrix"]=MatHash(Mat()); o["nodes"]=Fnv(nodeMem.data(),nodeMem.size()); o["handling"]=Fnv(handl.data(),handl.size());
    o["colm"]=Fnv(colm.data(),colm.size()); o["scen"]=int(scen); o["subj"]=subj;
    o["ms"]=AtA<uint32>(0xB7CB84); o["ts"]=AtA<uint32>(0xB7CB5C); o["player"]=Fnv(fakePlayer.data(),fakePlayer.size()); o["pad"]=Fnv(fakePad.data(),fakePad.size());
    return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"trainwave3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"trainwave3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("trainwave3");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"trainwave3-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,kRealFns> hooks{};
    for(size_t f=0;f<kRealFns;++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (TODOS antes de crear ningun EntryPatch: leccion 11) ---
    const uintptr_t dllService=Body(&CAEVehicleAudioEntity::Service);
    const uintptr_t dllAudioEvent=Body(static_cast<void (CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent));
    const uintptr_t dllAccel=Body(&CPad::GetAccelerate), dllBrake=Body(&CPad::GetBrake), dllShake=Body(&CPad::StartShake);
    const uintptr_t dllIsPlayer=Body(&CPed::IsPlayer), dllPadFromPlayer=Body(&CPlayerPed::GetPadFromPlayer);
    const uintptr_t dllNumCarr=Body(&CTrain::FindNumCarriagesPulled), dllFindMax=Body(&CTrain::FindMaximumSpeedToStopAtStations), dllTunnel=Body(&CTrain::IsInTunnel);
    const uintptr_t dllRemovePass=Body(&CTrain::RemoveRandomPassenger), dllAddPass=Body(&CTrain::AddNearbyPedAsRandomPassenger);
    const uintptr_t dllCalc=Body(&CGameLogic::CalcDistanceToForbiddenTrainCrossing);
    const uintptr_t dllCamShake=Body(static_cast<void (CCamera::*)(float,CVector)>(&CCamera::CamShake));
    const uintptr_t dllRand=Body(&CGeneral::GetRandomNumber);
    const uintptr_t dllFindPed=Body(&FindPlayerPed);
    const uintptr_t dllScan=Body(&CCarCtrl::ScanForPedDanger);
    const uintptr_t dllPhysProc=DllBody("CPhysical","ProcessControl");
    const uintptr_t dllApplyMove=Body(&CPhysical::ApplyMoveSpeed), dllSkip=Body(&CPhysical::SkipPhysics), dllRemoveAndAdd=Body(&CPhysical::RemoveAndAdd);
    const uintptr_t dllMoveForceV=Body(static_cast<void (CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce));
    const uintptr_t dllMoveForceF=Body(static_cast<void (CPhysical::*)(float,float,float)>(&CPhysical::ApplyMoveForce));
    const uintptr_t dllTurnForce=Body(&CPhysical::ApplyTurnForce);
    const uintptr_t dllUpdateFrame=Body(&CEntity::UpdateRwFrame);
    const uintptr_t dllBuoy=Body(&cBuoyancy::ProcessBuoyancy);
    const uintptr_t dllMark=Body(&MarkSurroundingEntitiesForCollisionWithTrain);
    const uintptr_t dllNodeLight=Body(&CTrainNode::GetLightingFromCollision);
    const uintptr_t dllLight=Body(&tColLighting::GetCurrentLighting);
    if(!dllPhysProc){ Write({{"event","error"},{"reason","DLL body of CPhysical::ProcessControl not found"}}); return; }

    std::array<bool,kRealFns> enabled{},locked{};
    for(size_t f=0;f<kRealFns;++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtable falsa de vehiculos y nombres de buferes
    for(auto& v:vt)v=reinterpret_cast<void*>(&V_Trap); vt[0]=reinterpret_cast<void*>(&V_Dtor); vt[4]=reinterpret_cast<void*>(&V_SetStatic);
    if(named.empty()){
        for(size_t i=0;i<vehs.size();++i){ static const char* nn[8]{"veh0","veh1","veh2","veh3","veh4","veh5","driver","dmg"}; Nm(vehs[i].b,kVehSize,nn[i]); }
        Nm(matBuf.data(),matBuf.size(),"mat"); Nm(colm.data(),colm.size(),"colm"); Nm(nodeMem.data(),nodeMem.size(),"nodes");
        Nm(fakePad.data(),fakePad.size(),"fakepad"); Nm(fakePlayer.data(),fakePlayer.size(),"player"); Nm(CPad::GetPad(0),sizeof(CPad),"pad0"); }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto Chk=[&](json& lay,const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      json lay=json::object();
      auto* Tr=reinterpret_cast<CTrain*>(VP(0));
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      Chk(lay,"CTrain.m_nNodeIndex",Off(Tr,&Tr->m_nNodeIndex),0x5A0);
      Chk(lay,"CTrain.m_fTrainSpeed",Off(Tr,&Tr->m_fTrainSpeed),0x5A4); Chk(lay,"CTrain.m_fCurrentRailDistance",Off(Tr,&Tr->m_fCurrentRailDistance),0x5A8);
      Chk(lay,"CTrain.m_fLength",Off(Tr,&Tr->m_fLength),0x5AC); Chk(lay,"CTrain.m_fTrainGas",Off(Tr,&Tr->m_fTrainGas),0x5B0); Chk(lay,"CTrain.m_fTrainBrake",Off(Tr,&Tr->m_fTrainBrake),0x5B4);
      Chk(lay,"CTrain.m_nTrainFlags",Off(Tr,&Tr->m_nTrainFlags),0x5B8); Chk(lay,"CTrain.m_nTimeWhenStoppedAtStation",Off(Tr,&Tr->m_nTimeWhenStoppedAtStation),0x5BC);
      Chk(lay,"CTrain.m_nTrackId",Off(Tr,&Tr->m_nTrackId),0x5C0); Chk(lay,"CTrain.m_nPassengersGenerationState",Off(Tr,&Tr->m_nPassengersGenerationState),0x5CA);
      Chk(lay,"CTrain.m_pPrevCarriage",Off(Tr,&Tr->m_pPrevCarriage),0x5D0); Chk(lay,"CTrain.m_pNextCarriage",Off(Tr,&Tr->m_pNextCarriage),0x5D4);
      Chk(lay,"CVehicle.m_pDriver",Off(Tr,&Tr->m_pDriver),0x460); Chk(lay,"CVehicle.m_pHandlingData",Off(Tr,&Tr->m_pHandlingData),0x384);
      Chk(lay,"CVehicle.m_vehicleAudio",Off(Tr,&Tr->m_vehicleAudio),0x138); Chk(lay,"CVehicle.m_autoPilot.m_nCruiseSpeed",Off(Tr,&Tr->m_autoPilot.m_nCruiseSpeed),0x3D0);
      Chk(lay,"CVehicle.m_nVehicleType",Off(Tr,&Tr->m_nVehicleType),0x590);
      Chk(lay,"CPhysical.m_vecMoveSpeed",Off(Tr,&Tr->m_vecMoveSpeed),0x44); Chk(lay,"CPhysical.m_vecTurnSpeed",Off(Tr,&Tr->m_vecTurnSpeed),0x50);
      Chk(lay,"CPhysical.m_vecForce",Off(Tr,&Tr->m_vecForce),0x74); Chk(lay,"CPhysical.m_vecTorque",Off(Tr,&Tr->m_vecTorque),0x80);
      Chk(lay,"CPhysical.m_fMass",Off(Tr,&Tr->m_fMass),0x8C); Chk(lay,"CPhysical.m_fBuoyancyConstant",Off(Tr,&Tr->m_fBuoyancyConstant),0xA0);
      Chk(lay,"CPhysical.m_nFakePhysics",Off(Tr,&Tr->m_nFakePhysics),0xB8); Chk(lay,"CPhysical.m_fMovingSpeed",Off(Tr,&Tr->m_fMovingSpeed),0xD4);
      Chk(lay,"CPhysical.m_fDamageIntensity",Off(Tr,&Tr->m_fDamageIntensity),0xD8); Chk(lay,"CPhysical.m_pDamageEntity",Off(Tr,&Tr->m_pDamageEntity),0xDC);
      Chk(lay,"CPhysical.m_fContactSurfaceBrightness",Off(Tr,&Tr->m_fContactSurfaceBrightness),0x12C);
      Chk(lay,"CEntity.m_nModelIndex",Off(Tr,&Tr->m_nModelIndex),0x22);
      { auto* Pd=reinterpret_cast<CPed*>(fakePlayer.data()); Chk(lay,"CPed.m_pVehicle",Off(Pd,&Pd->m_pVehicle),0x58C); }
      { auto* Cm=reinterpret_cast<CColModel*>(colm.data()); Chk(lay,"CColModel.m_boundBox.min",Off(Cm,&Cm->m_boundBox.m_vecMin),0); Chk(lay,"CColModel.m_boundBox.max",Off(Cm,&Cm->m_boundBox.m_vecMax),0xC); }
      { auto* Hd=reinterpret_cast<tHandlingData*>(handl.data()); Chk(lay,"tHandlingData.m_fSuspensionLowerLimit",Off(Hd,&Hd->m_fSuspensionLowerLimit),0xBC); }
      { auto* Nd=reinterpret_cast<CTrainNode*>(nodeMem.data()); Chk(lay,"CTrainNode.m_nDistanceFromStart",Off(Nd,&Nd->m_nDistanceFromStart),6); Chk(lay,"CTrainNode.m_bSurfLightingFound",Off(Nd,&Nd->m_bSurfLightingFound),9); Chk(lay,"sizeof(CTrainNode)",uint32(sizeof(CTrainNode)),10); }
      Chk(lay,"sizeof(CTrain)",uint32(sizeof(CTrain)),0x6AC);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(0xB7CB84,4); Snap(0xB7CB5C,4);                      // CTimer::m_snTimeInMilliseconds / ms_fTimeStep
        Snap(0xBA6728,1);                                        // gbModelViewer
        Snap(0xC37FEC,16); Snap(0xC38014,16); Snap(0xC38024,16); // arrTotalTrackLength / NumTrackNodes / pTrackNodes
        for(int32 m:kModels)Snap(0xA9B0C8+4*uint32(m),4);        // punteros de model info de los modelos usados
    }
    for(auto& mi:modelInfos)At<void*>(mi.data(),0x14)=colm.data();
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<kRealFns;++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F1Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                hooks[f]->State(candidate);
                const bool switches= InDll(AtA<uintptr_t>(fn.vt+4*uint32(fn.slot)))==candidate;     // despues de State(), no antes
                json out={{"switches",switches}};
                const uintptr_t target=AtA<uintptr_t>(fn.vt+4*uint32(fn.slot));
                std::memset(guardLo.data(),0xCD,kGuard); std::memset(guardHi.data(),0xCD,kGuard);
                for(size_t k=0;k<kModels.size();++k)AtA<void*>(0xA9B0C8+4*uint32(kModels[k]))=modelInfos[k].data();
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                Patches P;
                P.Add(0x502280,S_Service); P.Add(dllService,S_Service);
                P.Add(0x4F6420,S_AudioEvent); P.Add(dllAudioEvent,S_AudioEvent);
                P.Add(0x5403F0,S_Accel); P.Add(dllAccel,S_Accel); P.Add(0x540080,S_Brake); P.Add(dllBrake,S_Brake);
                P.Add(0x53F920,S_StartShake); P.Add(dllShake,S_StartShake);
                P.Add(0x5DF8F0,S_IsPlayer); P.Add(dllIsPlayer,S_IsPlayer); P.Add(0x609560,S_PadFromPlayer); P.Add(dllPadFromPlayer,S_PadFromPlayer);
                P.Add(0x6F5CD0,S_NumCarriages); P.Add(dllNumCarr,S_NumCarriages); P.Add(0x6F5BA0,S_FindMax); P.Add(dllFindMax,S_FindMax);
                P.Add(0x6F6320,S_Tunnel); P.Add(dllTunnel,S_Tunnel);
                P.Add(0x6F6850,S_RemovePassenger); P.Add(dllRemovePass,S_RemovePassenger); P.Add(0x6F8170,S_AddPassenger); P.Add(dllAddPass,S_AddPassenger);
                P.Add(0x4418E0,S_CalcDist); P.Add(dllCalc,S_CalcDist);
                P.Add(0x50A9F0,S_CamShake); P.Add(dllCamShake,S_CamShake);
                P.Add(0x821B1E,S_Rand); P.Add(dllRand,S_RandDll);
                P.Add(0x56E210,S_FindPed); P.Add(dllFindPed,S_FindPed);
                P.Add(0x42CE40,S_ScanDanger); P.Add(dllScan,S_ScanDanger);
                P.Add(0x5485E0,S_PhysProc); P.Add(dllPhysProc,S_PhysProc);
                P.Add(0x542DD0,S_ApplyMove); P.Add(dllApplyMove,S_ApplyMove); P.Add(0x5433B0,S_Skip); P.Add(dllSkip,S_Skip);
                P.Add(0x542560,S_RemoveAndAdd); P.Add(dllRemoveAndAdd,S_RemoveAndAdd); P.Add(0x532B00,S_UpdateFrame); P.Add(dllUpdateFrame,S_UpdateFrame);
                P.Add(0x5429F0,S_MoveForce); P.Add(dllMoveForceV,S_MoveForce); P.Add(dllMoveForceF,S_MoveForce);
                P.Add(0x542A50,S_TurnForce); P.Add(dllTurnForce,S_TurnForce);
                P.Add(0x6C3EF0,S_Buoy); P.Add(dllBuoy,S_Buoy);
                P.Add(0x6F6640,S_Mark); P.Add(dllMark,S_Mark);
                P.Add(0x6F5F80,S_NodeLight); P.Add(dllNodeLight,S_NodeLightDll);
                P.Add(0x59F0C0,S_LightExe); P.Add(dllLight,S_LightDll);
                out["state"]=CaseTrain(r,target);
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
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
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<kRealFns;++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
