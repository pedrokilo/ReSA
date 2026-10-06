#pragma once
// Banco dentro del motor: CVehicle, grupo A (consultas, banderas y estado; 31 funciones): AddPassenger (asiento automatico y
// fijo), CanBeDeleted, CanBeDriven, CanDoorsBeDamaged, CanPedEnterCar, CanPedJumpOutCar, CanPedLeanOut, CanPedOpenLocks,
// CanPedStepOutCar, CanVehicleBeDamaged, CarHasRoof, ChangeLawEnforcerState, Set/ClearGettingInFlags, Set/ClearGettingOutFlags,
// Set/ClearWindowOpenFlag, IsDriver y IsPassenger (peaton y modelo), IsLawEnforcementVehicle, IsOnItsSide, IsUpsideDown,
// IsVehicleNormal, IsVehiclePointerValid, UsesSiren, SetVehicleCreatedBy y MakeDirty.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos sobre UN VEHICULO FALSO (buffer mayor que
// sizeof(CHeli) con patron aleatorio; los punteros que el EXE desreferencia apuntan a objetos falsos: matriz, clump, handling,
// peatones, model infos). Nunca vehiculos reales, el jugador ni pools reales: IsVehiclePointerValid se prueba cambiando
// temporalmente el puntero del pool de vehiculos (0xB74494) por un pool FALSO de 8 huecos de 0xA18 bytes.
// Se simulan y trazan en ambos lados (direccion del EXE + simbolo de la DLL): CPhysical::ApplyTurnForce,
// CPhysical::CanPhysicalBeDamaged, CEntity::RegisterReference, FindPlayerPed, FindPlayerVehicle, CCarCtrl::UpdateCarCount,
// CClumpModelInfo::GetFrameFromId y RwFrameForAllObjects (con el callback normalizado a su nombre). CanPedStepOutCar se
// simula por la vtable falsa del vehiculo (CanPedJumpOutCar la llama por la vtable, ranura 57). Estado global guardado y
// restaurado en cada ejecucion y al final: CTimer (tiempo y paso), CWeather::Rain, CCarCtrl::NumLawEnforcerCars, puntero del
// pool de vehiculos y tres entradas de ms_modelInfoPtrs (se cambian por model infos falsos).
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/Train.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Models/ClumpModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Pools/VehiclePool.h"
#include "CarCtrl.h"
#include "Weather.h"
#include "Timer.h"
#include "SurfaceInfos_c.h"
#include "Collision/ColPoint.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace CVehALive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool coin(uint32 n=2){ return next()%n==0; } };
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
// --- modulo de la DLL (cuerpos tras los saltos de enlace incremental) ---
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

// ======================= objetos falsos =======================
constexpr int NP=4;                                   // peatones que pueden ser ocupantes; el quinto (indice NP) es "otro"
constexpr size_t PEDN=0x7A0, VEHN=0xA40, SLOTSZ=0xA18, POOLSLOTS=8, MARGIN=0x1000;
static_assert(sizeof(CPed)<=PEDN&&sizeof(CHeli)<=VEHN,"tamanos de los buffers falsos");
alignas(16) inline std::array<uint8,VEHN> veh{};
alignas(16) inline std::array<uint8,0x80> vmat{};
alignas(16) inline std::array<std::array<uint8,PEDN>,NP+1> ped{};
alignas(16) inline std::array<std::array<uint8,0x80>,NP+1> pmat{};
alignas(16) inline std::array<uint8,PEDN> plr{};
alignas(16) inline std::array<uint8,0x80> plrmat{};
alignas(16) inline std::array<uint8,0x100> pveh{};
alignas(16) inline std::array<std::array<uint8,0x100>,3> vmi{};
alignas(16) inline std::array<std::array<uint8,0x400>,3> vstruct{};
alignas(16) inline std::array<uint8,0x100> hand{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> frame{};
alignas(16) inline std::array<uint8,0x80> colpoint{};
alignas(16) inline std::array<uint8,MARGIN*2+POOLSLOTS*SLOTSZ> poolStore{};
alignas(16) inline std::array<uint8,0x40> poolState{};
alignas(16) inline std::array<uint8,0x20> fakePool{};
inline std::array<void*,66> vtbl{};
constexpr int32 MI0=19990;                            // model infos falsos en ms_modelInfoPtrs[19990..19992]
constexpr uintptr_t G_TIME=0xB7CB84, G_STEP=0xB7CB5C, G_RAIN=0xC81324, G_LAW=0x969098, G_VPOOL=0xB74494, G_MIPTRS=0xA9B0C8,
                    EXE_SETFLAGCB=0x4C7B90, EXE_CLEARFLAGCB=0x4C7BB0;
inline CVehicle* Veh(){ return reinterpret_cast<CVehicle*>(veh.data()); }
inline CPed* PedP(int k){ return k<0 ? nullptr : reinterpret_cast<CPed*>(ped[size_t(k)].data()); }

// desplazamientos crudos del EXE
constexpr size_t V_MAT=0x14, V_CLUMP=0x18, V_MODEL=0x22, V_STATUS=0x36, V_LASTCOL=0x3C, V_PFLAGS=0x40, V_MOVE=0x44, V_TURN=0x50, V_COLLIST=0xB0,
                 V_HAND=0x384, V_FLAGS=0x428, V_EXTRA=0x438, V_DRIVER=0x460, V_PASS=0x464, V_NUMPASS=0x484, V_NUMIN=0x485, V_INFLAGS=0x486,
                 V_OUTFLAGS=0x487, V_MAXPASS=0x488, V_CREATEDBY=0x4A4, V_DIRT=0x4B0, V_HEALTH=0x4C0, V_DOORLOCK=0x4F8, V_TYPE=0x590, V_SUBTYPE=0x594, V_TRACK=0x5C0,
                 P_MODEL=0x22, P_MAT=0x14, P_MASS=0x8C, P_ATTACHED=0xFC, P_CREATEDBY=0x484, P_STATE=0x530, P_TYPE=0x598,
                 H_FLAGS=0xCC, H_ANIM=0xDE, M_TYPE=0x3C, M_CLASS=0x4D, M_STRUCT=0x5C, CP_SURF=0x23;

// --- nombres de punteros (para las trazas) ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Name(const void* p){ return Name(uintptr_t(p)); }
inline void Tr(json&& j){ if(Main())trace.push_back(std::move(j)); }

// configuracion por caso de los dobles
inline void* playerRet{}; inline void* pvehRet{}; inline bool physRet{}, physOut{}, stepRet{}; inline void* frameRet{};

// ======================= dobles =======================
inline void __fastcall S_ApplyTurn(void* self,void*,float a,float b,float c,float d,float e,float f){ Tr({"CPhysical::ApplyTurnForce",Name(self),F(a),F(b),F(c),F(d),F(e),F(f)}); }
inline bool __fastcall S_CanPhys(void* self,void*,int32 weapon,bool* out){ const bool before=*out; Tr({"CPhysical::CanPhysicalBeDamaged",Name(self),weapon,Name(out),int(before)}); *out=physOut; return physRet; }
inline void __fastcall S_RegRef(void* self,void*,void* field){ Tr({"CEntity::RegisterReference",Name(self),Name(field)}); }
inline CPlayerPed* __cdecl S_FindPlayer(int32 id){ Tr({"FindPlayerPed",id}); return static_cast<CPlayerPed*>(playerRet); }
inline CVehicle* __cdecl S_FindPlayerVeh(int32 id,uint32 remote){ Tr({"FindPlayerVehicle",id,remote&0xFFu}); return static_cast<CVehicle*>(pvehRet); }
inline void __cdecl S_UpdateCarCount(void* v,uint32 dec){ Tr({"CCarCtrl::UpdateCarCount",Name(v),dec&0xFFu,int(At<uint8>(veh.data(),V_CREATEDBY))}); }
inline RwFrame* __cdecl S_FrameFromId(RpClump* c,int32 id){ Tr({"CClumpModelInfo::GetFrameFromId",Name(c),id}); return static_cast<RwFrame*>(frameRet); }
inline json CbName(uintptr_t cb){
    if(cb==EXE_SETFLAGCB||cb==Addr(&CVehicleModelInfo::SetAtomicFlagCB)||cb==FollowIn(Addr(&CVehicleModelInfo::SetAtomicFlagCB)))return "SetAtomicFlagCB";
    if(cb==EXE_CLEARFLAGCB||cb==Addr(&CVehicleModelInfo::ClearAtomicFlagCB)||cb==FollowIn(Addr(&CVehicleModelInfo::ClearAtomicFlagCB)))return "ClearAtomicFlagCB";
    return cb; }
inline RwFrame* __cdecl S_FrameForAll(RwFrame* f,RwObjectCallBack cb,void* d){ Tr({"RwFrameForAllObjects",Name(f),CbName(uintptr_t(cb)),Name(d)}); return f; }
// vtable falsa del vehiculo: ranura 57 = CanPedStepOutCar, las demas son trampas
template<int N> void* __fastcall VTrap(void* v,void*){ Tr({"vehicle_unexpected_slot",N,Name(v)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((vtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline bool __fastcall S_StepOut(void* v,void*,uint32 a){ Tr({"CVehicle::CanPedStepOutCar",Name(v),a&0xFFu}); return stepRet; }

// ======================= fixtures =======================
inline float Nan(){ return std::bit_cast<float>(0x7FC00000u); }
inline float Up(float v){ return std::nextafter(v,std::numeric_limits<float>::infinity()); }
inline float Dn(float v){ return std::nextafter(v,-std::numeric_limits<float>::infinity()); }
// valor cerca de un umbral: exacto, +-1 ULP, ruido, valores especiales (NaN cuando se pide)
inline float Thr(Rng& r,std::initializer_list<float> th,bool sign=false){
    const float b=th.begin()[r.next()%th.size()]; float v;
    switch(r.next()%10){
        case 0: case 1: case 2: v=b; break;
        case 3: case 4: v=Up(b); break;
        case 5: case 6: v=Dn(b); break;
        case 7: v=b*r.uni(0.5f,1.5f); break;
        case 8: v=r.uni(0.f,2.f*b); break;
        default: { static const float sp[]{0.f,-0.f,1.f,100.f,1e-20f,1e20f}; v=sp[r.next()%6]; if(r.next()%6==0)v=Nan(); } }
    return sign&&r.coin() ? -v : v; }
// vector (3 floats) con modulo cerca de los umbrales dados y una componente NaN de vez en cuando
inline void VecAt(void* base,size_t off,Rng& r,std::initializer_list<float> mags){
    const float m=Thr(r,mags); float d[3]{};
    switch(r.next()%6){
        case 0: d[0]=1.f; break; case 1: d[1]=1.f; break; case 2: d[2]=1.f; break; case 3: d[0]=d[1]=0.70710678f; break;
        case 4: { for(auto& x:d)x=r.uni(-1.f,1.f); const float l=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]); if(l>0.f)for(auto& x:d)x/=l; break; }
        default: d[0]=r.uni(-1.f,1.f); d[1]=r.uni(-1.f,1.f); }
    for(int k=0;k<3;++k)At<float>(base,off+4*size_t(k))=(r.coin() ? -1.f : 1.f)*m*d[k];
    if(r.next()%24==0)At<float>(base,off+4*(r.next()%3))=Nan(); }
inline void SetMat(uint8* m,Rng& r,std::initializer_list<float> upZ,std::initializer_list<float> rightZ){
    for(size_t k=0;k<0x48;k+=4)At<float>(m,k)=r.uni(-1.f,1.f);
    At<float>(m,0x2C)=r.uni(-1.f,1.f); At<float>(m,0x30)=r.uni(-500.f,500.f); At<float>(m,0x34)=r.uni(-500.f,500.f); At<float>(m,0x38)=r.uni(-50.f,50.f);
    At<float>(m,0x28)=Thr(r,upZ,true); At<float>(m,0x08)=Thr(r,rightZ,true); }
inline void Fill(uint8* p,size_t n,Rng& r){ for(size_t k=0;k<n;++k)p[k]=uint8(r.next()>>5); }
template<class A> void Fill(A& a,Rng& r){ Fill(a.data(),a.size(),r); }

// fixture comun: cada campo que cualquier funcion pueda leer como float se rellena con valores validos (nunca sNaN)
inline void Reset(Rng& r){
    trace=json::array();
    Fill(veh,r); Fill(vmat,r); Fill(plr,r); Fill(plrmat,r); Fill(pveh,r); Fill(hand,r); Fill(clump,r); Fill(colpoint,r);
    for(auto& f:frame)Fill(f,r);
    for(size_t k=0;k<3;++k){ Fill(vmi[k],r); Fill(vstruct[k],r); }
    for(size_t k=0;k<=NP;++k){ Fill(ped[k],r); Fill(pmat[k],r);
        SetMat(pmat[k].data(),r,{0.5f},{0.5f});
        At<void*>(ped[k].data(),P_MAT)=pmat[k].data();
        { static const int16 mods[]{0,1,2,407,416,423,432,490,596,601,19990,-1,-2,32767,-32768,5000}; At<int16>(ped[k].data(),P_MODEL)= r.coin(4) ? int16(r.next()) : mods[r.next()%16]; }
        { static const float ms[]{0.f,1.f,70.f,100.f,5000.f,1e5f,-70.f}; At<float>(ped[k].data(),P_MASS)= r.coin(3) ? r.uni(0.f,200.f) : ms[r.next()%7]; }
        At<uint8>(ped[k].data(),P_CREATEDBY)= r.coin(3) ? uint8(2) : uint8(r.next()%5);
        { static const uint32 st[]{50,55,50,55,0,1,5,56,49,0x32,0x37}; At<uint32>(ped[k].data(),P_STATE)= r.coin(4) ? r.next()%90 : st[r.next()%11]; }
        At<uint32>(ped[k].data(),P_TYPE)= r.coin(4) ? r.next()%40 : r.next()%6; }
    At<void*>(plr.data(),P_MAT)=plrmat.data();
    // vehiculo
    At<void*>(veh.data(),0)=vtbl.data(); At<void*>(veh.data(),V_MAT)=vmat.data(); At<void*>(veh.data(),V_CLUMP)= r.next()%10==0 ? nullptr : clump.data();
    At<void*>(veh.data(),V_HAND)=hand.data(); clump[0]=2;
    At<uint16>(veh.data(),V_MODEL)=uint16(MI0+int32(r.next()%3));
    At<uint32>(veh.data(),V_LASTCOL)=r.next();
    SetMat(vmat.data(),r,{0.1f,0.3f,0.9f},{0.8f});
    VecAt(veh.data(),V_MOVE,r,{0.01f,0.05f,0.06f,0.04f,0.2f,0.3162278f,0.7071068f,0.1f,0.f});
    VecAt(veh.data(),V_TURN,r,{0.02f,0.2f,0.05f,0.0f,0.01f});
    At<float>(veh.data(),V_DIRT)=Thr(r,{1.f,4.f,15.f,0.f,8.f},false);
    At<float>(veh.data(),V_HEALTH)=Thr(r,{250.f,1000.f,300.f},false);
    // ocupantes
    At<void*>(veh.data(),V_DRIVER)= r.coin(3) ? nullptr : static_cast<void*>(ped[r.next()%NP].data());
    for(size_t s=0;s<8;++s)At<void*>(veh.data(),V_PASS+4*s)= r.coin(2) ? nullptr : static_cast<void*>(ped[r.next()%NP].data());
    At<uint8>(veh.data(),V_NUMPASS)=uint8(r.next()%9);
    At<uint8>(veh.data(),V_NUMIN)= r.coin(2) ? uint8(0) : uint8(r.next());
    At<uint8>(veh.data(),V_OUTFLAGS)= r.coin(2) ? uint8(0) : uint8(r.next());
    At<uint8>(veh.data(),V_MAXPASS)= r.next()%12==0 ? uint8(9+r.next()%4) : uint8(r.next()%9);
    At<uint8>(veh.data(),V_CREATEDBY)= r.coin(10) ? uint8(r.next()) : uint8(r.next()%6);
    { static const uint32 locks[]{0,1,2,3,4,5,6,7,8,0xFFFFFFFFu}; At<uint32>(veh.data(),V_DOORLOCK)= r.coin(6) ? r.next() : locks[r.next()%10]; }
    { static const int32 ty[]{0,1,2,3,4,5,6,7,8,9,10,11,-1,12,100}; At<int32>(veh.data(),V_TYPE)=ty[r.next()%15]; At<int32>(veh.data(),V_SUBTYPE)=ty[r.next()%15];
      if(r.coin(3))At<int32>(veh.data(),V_TYPE)=At<int32>(veh.data(),V_SUBTYPE); }
    At<uint8>(veh.data(),V_TRACK)= r.coin(2) ? uint8(0) : uint8(r.next());
    { auto& fl=At<uint32>(veh.data(),V_FLAGS); fl=r.next(); }
    { auto& pf=At<uint32>(veh.data(),V_PFLAGS); pf=r.next(); }
    // status: byte 0x36 (bits 3-7 = estado)
    At<uint8>(veh.data(),V_STATUS)= r.coin(3) ? uint8(0x28|(r.next()&7)) : uint8(r.next());
    At<uint8>(veh.data(),V_EXTRA)=uint8(r.next()%3==0 ? 0 : r.next()); At<uint8>(veh.data(),V_EXTRA+1)=uint8(r.next()%3==0 ? 0 : r.next());
    At<uint32>(veh.data(),V_COLLIST)= r.coin(2) ? 0u : r.next()|1u;
    // manejo: indice de animacion y banderas del modelo
    At<uint8>(hand.data(),H_ANIM)= r.coin(3) ? uint8(r.next()) : uint8(10+r.next()%12);
    At<uint32>(hand.data(),H_FLAGS)= r.coin(2) ? (r.next()|0x800u) : (r.next()&~0x800u);
    // model infos falsos
    for(size_t k=0;k<3;++k){ At<int32>(vmi[k].data(),M_TYPE)= r.coin(2) ? 5 : int32(r.next()%12);
        At<uint8>(vmi[k].data(),M_CLASS)= r.coin(3) ? uint8(0xFF) : uint8(r.next()%9);
        At<void*>(vmi[k].data(),M_STRUCT)=vstruct[k].data();
        for(size_t d=0;d<15;++d)for(size_t c=0;c<3;++c){ static const float dv[]{0.f,-0.f,0.5f,-0.5f,1e-30f,1e-20f,-1e-20f,3.f,1e-45f};
            At<float>(vstruct[k].data(),d*12+c*4)= r.coin(3) ? 0.f : (r.coin(12) ? Nan() : dv[r.next()%9]); }
        AtA<void*>(G_MIPTRS+4*size_t(MI0)+4*k)=vmi[k].data(); }
    // mundo
    { static const float ts[]{0.f,0.5f,1.f,1.5f,2.f,5.f,10.f,50.f,0.02f}; AtA<float>(G_STEP)= r.coin(3) ? r.uni(0.f,12.f) : ts[r.next()%9]; }
    AtA<uint32>(G_TIME)= r.coin(4) ? r.next()%3000 : r.next();
    { static const float rn[]{0.f,0.1f,0.2f,0.20000001f,0.19999999f,1.f,0.5f}; AtA<float>(G_RAIN)= r.coin(8) ? Nan() : rn[r.next()%7]; }
    { static const uint32 lc[]{0,1,2,5,0xFFFFFFFFu,0x7FFFFFFFu}; AtA<uint32>(G_LAW)= r.coin(3) ? r.next() : lc[r.next()%6]; }
    // pool falso: huecos de 0xA18, estados aleatorios (bit 7 = libre)
    Fill(poolStore,r); Fill(poolState,r);
    for(size_t s=0;s<POOLSLOTS;++s){ uint8* slot=poolStore.data()+MARGIN+s*SLOTSZ;
        At<int32>(slot,V_TYPE)= r.coin(3) ? 8 : int32(r.next()%12); At<uint32>(slot,V_COLLIST)= r.coin(2) ? 0u : r.next()|1u; }
    At<void*>(fakePool.data(),0)=poolStore.data()+MARGIN; At<void*>(fakePool.data(),4)=poolState.data(); At<uint32>(fakePool.data(),8)=uint32(POOLSLOTS);
    At<int32>(fakePool.data(),0xC)=-1; fakePool[0x10]=0; fakePool[0x11]=0;
    // dobles
    switch(r.next()%5){ case 0: playerRet=nullptr; break; default: playerRet=plr.data(); }
    At<void*>(plr.data(),P_ATTACHED)= r.coin(2) ? static_cast<void*>(veh.data()) : (r.coin() ? nullptr : static_cast<void*>(pveh.data()));
    switch(r.next()%4){ case 0: pvehRet=nullptr; break; case 1: pvehRet=veh.data(); break; default: pvehRet=pveh.data(); }
    physRet=r.coin(); physOut=r.coin(); stepRet=r.coin(); frameRet= r.coin(4) ? nullptr : static_cast<void*>(frame[r.next()%2].data());
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=31;
inline const std::array<Fn,NFN> fns{{
    {"CVehicle","AddPassenger-Auto-Seat",0x6D13A0,0,-1,768},{"CVehicle","AddPassenger-Fixed-Seat",0x6D14D0,0,-1,1024},
    {"CVehicle","CanBeDeleted",0x6D1180,0,-1,768},{"CVehicle","CanBeDriven",0x6D5400,0,-1,512},{"CVehicle","CanDoorsBeDamaged",0x6D1E60,0,-1,128},
    {"CVehicle","CanPedEnterCar",0x6D1E80,0,-1,768},{"CVehicle","CanPedJumpOutCar",0,0x871E80,58,1024},{"CVehicle","CanPedLeanOut",0x6D5CF0,0,-1,512},
    {"CVehicle","CanPedOpenLocks",0x6D1E20,0,-1,256},{"CVehicle","CanPedStepOutCar",0,0x871E80,57,768},{"CVehicle","CanVehicleBeDamaged",0x6D1280,0,-1,1024},
    {"CVehicle","CarHasRoof",0x6D25D0,0,-1,256},{"CVehicle","ChangeLawEnforcerState",0x6D2330,0,-1,256},
    {"CVehicle","ClearGettingInFlags",0x6D3040,0,-1,128},{"CVehicle","ClearGettingOutFlags",0x6D3060,0,-1,128},
    {"CVehicle","SetGettingInFlags",0x6D3000,0,-1,128},{"CVehicle","SetGettingOutFlags",0x6D3020,0,-1,128},
    {"CVehicle","SetWindowOpenFlag",0x6D3080,0,-1,256},{"CVehicle","ClearWindowOpenFlag",0x6D30B0,0,-1,256},
    {"CVehicle","IsDriver-ModelID",0x6D1C60,0,-1,256},{"CVehicle","IsDriver-Ped",0x6D1C40,0,-1,256},
    {"CVehicle","IsPassenger-ModelID",0x6D1C00,0,-1,256},{"CVehicle","IsPassenger-Ped",0x6D1BD0,0,-1,256},
    {"CVehicle","IsLawEnforcementVehicle",0x6D2370,0,-1,512},{"CVehicle","IsOnItsSide",0x6D1DD0,0,-1,256},{"CVehicle","IsUpsideDown",0x6D1D90,0,-1,256},
    {"CVehicle","IsVehicleNormal",0x6D22F0,0,-1,512},{"CVehicle","IsVehiclePointerValid",0x6E38F0,0,-1,1024},{"CVehicle","UsesSiren",0x6D8470,0,-1,512},
    {"CVehicle","SetVehicleCreatedBy",0x6D5D70,0,-1,256},{"CVehicle","MakeDirty",0x6D2BF0,0,-1,1024},
}};

// comprobaciones de diseno: la DLL y el EXE deben coincidir en las posiciones que usa el banco (si no, el banco no vale)
inline std::string LayoutErrors() {
    std::string bad;
    auto chk=[&](const char* nm,uintptr_t got,size_t want){ if(got!=want)bad+=std::string(nm)+"="+std::to_string(got)+" (EXE "+std::to_string(want)+"); "; };
    auto* v=Veh(); const uintptr_t b=uintptr_t(v); auto off=[&](const void* m){ return uintptr_t(m)-b; };
    chk("sizeof(CHeli)",sizeof(CHeli),SLOTSZ);
    chk("m_matrix",off(&v->m_matrix),V_MAT); chk("m_nModelIndex",off(&v->m_nModelIndex),V_MODEL); chk("m_nLastCollisionTime",off(&v->m_nLastCollisionTime),V_LASTCOL);
    chk("m_vecMoveSpeed",off(&v->m_vecMoveSpeed),V_MOVE); chk("m_vecTurnSpeed",off(&v->m_vecTurnSpeed),V_TURN); chk("m_pCollisionList",off(&v->m_pCollisionList),V_COLLIST);
    chk("m_pHandlingData",off(&v->m_pHandlingData),V_HAND); chk("vehicleFlags",off(&v->vehicleFlags),V_FLAGS); chk("m_anExtras",off(v->m_anExtras.data()),V_EXTRA);
    chk("m_pDriver",off(&v->m_pDriver),V_DRIVER); chk("m_apPassengers",off(v->m_apPassengers.data()),V_PASS); chk("m_nNumPassengers",off(&v->m_nNumPassengers),V_NUMPASS);
    chk("m_nNumGettingIn",off(&v->m_nNumGettingIn),V_NUMIN); chk("m_nGettingInFlags",off(&v->m_nGettingInFlags),V_INFLAGS); chk("m_nGettingOutFlags",off(&v->m_nGettingOutFlags),V_OUTFLAGS);
    chk("m_nMaxPassengers",off(&v->m_nMaxPassengers),V_MAXPASS); chk("m_nCreatedBy",off(&v->m_nCreatedBy),V_CREATEDBY); chk("m_fDirtLevel",off(&v->m_fDirtLevel),V_DIRT);
    chk("m_fHealth",off(&v->m_fHealth),V_HEALTH); chk("m_nDoorLock",off(&v->m_nDoorLock),V_DOORLOCK); chk("m_nVehicleType",off(&v->m_nVehicleType),V_TYPE);
    chk("m_nVehicleSubType",off(&v->m_nVehicleSubType),V_SUBTYPE); chk("physicalFlags",off(&v->physicalFlags),V_PFLAGS); chk("m_fMass",off(&v->m_fMass),P_MASS);
    chk("m_pAttachedTo",off(&v->m_pAttachedTo),P_ATTACHED);
    chk("CTrain::m_nTrackId",uintptr_t(&reinterpret_cast<CTrain*>(veh.data())->m_nTrackId)-b,V_TRACK);
    auto* p=reinterpret_cast<CPed*>(ped[0].data()); const uintptr_t pb=uintptr_t(p);
    chk("CPed::m_nModelIndex",uintptr_t(&p->m_nModelIndex)-pb,P_MODEL); chk("CPed::m_matrix",uintptr_t(&p->m_matrix)-pb,P_MAT); chk("CPed::m_fMass",uintptr_t(&p->m_fMass)-pb,P_MASS);
    chk("CPed::m_nPedState",uintptr_t(&p->m_nPedState)-pb,P_STATE); chk("CPed::m_nPedType",uintptr_t(&p->m_nPedType)-pb,P_TYPE);
    auto* mi=reinterpret_cast<CVehicleModelInfo*>(vmi[0].data());
    chk("m_nVehicleType (mi)",uintptr_t(&mi->m_nVehicleType)-uintptr_t(mi),M_TYPE); chk("m_nVehicleClass (mi)",uintptr_t(&mi->m_nVehicleClass)-uintptr_t(mi),M_CLASS);
    chk("m_pVehicleStruct (mi)",uintptr_t(&mi->m_pVehicleStruct)-uintptr_t(mi),M_STRUCT);
    chk("m_nAnimGroup",uintptr_t(&reinterpret_cast<tHandlingData*>(hand.data())->m_nAnimGroup)-uintptr_t(hand.data()),H_ANIM);
    chk("m_nSurfaceTypeB",uintptr_t(&reinterpret_cast<CColPoint*>(colpoint.data())->m_nSurfaceTypeB)-uintptr_t(colpoint.data()),CP_SURF);
    chk("CTimer::m_snTimeInMilliseconds",uintptr_t(&CTimer::m_snTimeInMilliseconds),G_TIME); chk("CTimer::ms_fTimeStep",uintptr_t(&CTimer::ms_fTimeStep),G_STEP);
    chk("CWeather::Rain",uintptr_t(&CWeather::Rain),G_RAIN); chk("NumLawEnforcerCars",uintptr_t(&CCarCtrl::NumLawEnforcerCars),G_LAW);
    chk("ms_modelInfoPtrs",uintptr_t(&CModelInfo::ms_modelInfoPtrs[0]),G_MIPTRS);
    chk("STATUS_WRECKED",size_t(STATUS_WRECKED),5); chk("ATOMIC_DONT_RENDER_ALPHA",size_t(eAtomicComponentFlag::ATOMIC_DONT_RENDER_ALPHA),0x1000);
    chk("MODEL_ENFORCER",size_t(MODEL_ENFORCER),427); chk("MODEL_FIRETRUK",size_t(MODEL_FIRETRUK),407); chk("MODEL_RHINO",size_t(MODEL_RHINO),432);
    // comportamiento: campos que dependen de la disposicion de bits o de miembros privados
    std::memset(veh.data(),0,veh.size());
    At<uint8>(veh.data(),V_FLAGS)=1; if(!v->vehicleFlags.bIsLawEnforcer)bad+="bIsLawEnforcer (byte 0x428 bit 0); "; At<uint8>(veh.data(),V_FLAGS)=0;
    At<uint8>(veh.data(),V_FLAGS+1)=2; if(!v->vehicleFlags.bIsBus)bad+="bIsBus (byte 0x429 bit 1); "; At<uint8>(veh.data(),V_FLAGS+1)=0;
    At<uint8>(veh.data(),V_FLAGS+2)=0x20; if(!v->vehicleFlags.bCanBeDamaged)bad+="bCanBeDamaged (byte 0x42A bit 5); "; At<uint8>(veh.data(),V_FLAGS+2)=0;
    At<uint8>(veh.data(),V_FLAGS+4)=0x80; if(!v->vehicleFlags.bIsRCVehicle)bad+="bIsRCVehicle (byte 0x42C bit 7); "; At<uint8>(veh.data(),V_FLAGS+4)=0;
    At<uint32>(veh.data(),V_PFLAGS)=0x400000; if(!v->physicalFlags.bInvulnerable)bad+="bInvulnerable (bit 22 de 0x40); "; At<uint32>(veh.data(),V_PFLAGS)=0;
    At<uint8>(veh.data(),V_STATUS)=uint8(5<<3)|2; if(v->GetStatus()!=STATUS_WRECKED)bad+="status (byte 0x36 >> 3); "; At<uint8>(veh.data(),V_STATUS)=0;
    At<void*>(veh.data(),V_CLUMP)=clump.data(); clump[0]=2; if(v->GetRpClump()!=reinterpret_cast<RpClump*>(clump.data()))bad+="m_pRwObject (0x18); "; At<void*>(veh.data(),V_CLUMP)=nullptr;
    std::memset(hand.data(),0,hand.size()); At<uint32>(hand.data(),H_FLAGS)=0x800; if(!reinterpret_cast<tHandlingData*>(hand.data())->m_bConvertible)bad+="m_bConvertible (bit 11 de 0xCC); ";
    { std::memset(vstruct[0].data(),0,0x400); At<float>(vstruct[0].data(),0x30)=1.f; std::memset(vmi[0].data(),0,0x100); At<void*>(vmi[0].data(),M_STRUCT)=vstruct[0].data();
      if(&mi->GetModelDummyPosition(DUMMY_SEAT_FRONT)!=reinterpret_cast<CVector*>(vstruct[0].data()+0x30)||&mi->GetModelDummyPosition(DUMMY_LIGHT_FRONT_MAIN)!=reinterpret_cast<CVector*>(vstruct[0].data()))bad+="dummy positions (struct +0, +0x30); "; }
    { void* const saved=AtA<void*>(G_VPOOL); AtA<void*>(G_VPOOL)=fakePool.data();
      At<void*>(fakePool.data(),0)=poolStore.data()+MARGIN; At<void*>(fakePool.data(),4)=poolState.data(); At<uint32>(fakePool.data(),8)=uint32(POOLSLOTS);
      auto* pool=GetVehiclePool();
      if(pool!=reinterpret_cast<CVehiclePool*>(fakePool.data()))bad+="GetVehiclePool() != 0xB74494; ";
      else { if(pool->GetSize()!=int32(POOLSLOTS))bad+="pool capacity (+8); ";
             if(pool->GetIndexUnchecked(reinterpret_cast<CVehicle*>(poolStore.data()+MARGIN+3*SLOTSZ+5))!=3||pool->GetIndexUnchecked(reinterpret_cast<CVehicle*>(poolStore.data()+MARGIN-3))!=0)bad+="pool index; ";
             poolState[2]=0x80; poolState[3]=0x00; if(!pool->IsFreeSlotAtIndex(2)||pool->IsFreeSlotAtIndex(3))bad+="pool slot state (+4, bit 7 = free); "; }
      AtA<void*>(G_VPOOL)=saved; }
    return bad;
}

inline json Region_(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(w[k]); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cveha-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cveha-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cveha");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"cveha-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    named={{veh.data(),veh.size(),"veh"},{vmat.data(),vmat.size(),"vmat"},{plr.data(),plr.size(),"player"},{plrmat.data(),plrmat.size(),"plrmat"},{pveh.data(),pveh.size(),"pveh"},
           {hand.data(),hand.size(),"hand"},{clump.data(),clump.size(),"clump"},{frame[0].data(),0x40,"frame0"},{frame[1].data(),0x40,"frame1"},{colpoint.data(),colpoint.size(),"colpoint"},
           {poolStore.data(),poolStore.size(),"pool_store"},{fakePool.data(),fakePool.size(),"fake_pool"},{vtbl.data(),sizeof(vtbl),"vtbl"}};
    for(int k=0;k<=NP;++k){ static const char* pn[]{"ped0","ped1","ped2","ped3","ped_other"}; static const char* mn[]{"pmat0","pmat1","pmat2","pmat3","pmat_other"};
        named.push_back({ped[size_t(k)].data(),PEDN,pn[k]}); named.push_back({pmat[size_t(k)].data(),0x80,mn[k]}); }
    bool layoutOk=true;
    { const auto bad=LayoutErrors(); layoutOk=bad.empty(); Write({{"event","layout"},{"layout_ok",layoutOk},{"details",bad}}); if(!layoutOk){Write({{"event","error"},{"reason","layout mismatch DLL/EXE"},{"details",bad}});return;} }

    // estado global guardado
    regions.clear();
    Snap(G_TIME,4); Snap(G_STEP,4); Snap(G_RAIN,4); Snap(G_LAW,4); Snap(G_VPOOL,4);
    for(size_t k=0;k<3;++k)Snap(G_MIPTRS+4*(size_t(MI0)+k),4);
    FillTraps(std::make_integer_sequence<int,66>{}); vtbl[57]=reinterpret_cast<void*>(&S_StepOut);

    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        Patches p; std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        P(0x542A50,Addr(&CPhysical::ApplyTurnForce),S_ApplyTurn);
        P(0x5448B0,Addr(&CPhysical::CanPhysicalBeDamaged),S_CanPhys);
        P(0x571B70,Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),S_RegRef);
        P(0x56E210,Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),S_FindPlayer);
        P(0x56E0D0,Addr(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),S_FindPlayerVeh);
        P(0x424000,Addr(&CCarCtrl::UpdateCarCount),S_UpdateCarCount);
        P(0x4C53C0,Addr(&CClumpModelInfo::GetFrameFromId),S_FrameFromId);
        P(0x7F1200,Addr(&RwFrameForAllObjects),S_FrameForAll);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    RestoreRegions();
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xCE4A11u}; if(!r.s)r.s=1;
                    Reset(r);
                    CVehicle* const v=Veh(); uint8* const vb=veh.data();
                    // ---- fixtures y argumentos propios de cada funcion ----
                    auto pedArg=[&]()->CPed*{ const uint32 m=r.next()%7; return m==0 ? nullptr : (m==1 ? PedP(NP) : PedP(int(r.next()%NP))); };
                    auto occupantArg=[&]()->CPed*{ // un ocupante, el "otro" o nulo
                        const uint32 m=r.next()%8; if(m==0)return nullptr; if(m==1)return PedP(NP);
                        if(m<=3)return reinterpret_cast<CPed*>(At<void*>(vb,V_DRIVER)); return reinterpret_cast<CPed*>(At<void*>(vb,V_PASS+4*(r.next()%8))); };
                    CPed* aPed=nullptr; uint32 aU=0; int32 aI=0; bool aB=false; CEntity* aEnt=nullptr; int32 aWeapon=0; bool dmgFlag=false;
                    if(nm=="AddPassenger-Auto-Seat"||nm=="AddPassenger-Fixed-Seat"){
                        aPed=pedArg(); if(!aPed)aPed=PedP(NP); aU=r.next()%11==0 ? 255u : r.next()%10;
                        if(r.coin(4))At<uint8>(vb,V_FLAGS+1)|=2; else At<uint8>(vb,V_FLAGS+1)&=uint8(~2);
                        if(r.coin(3))for(size_t s=0;s<8;++s)At<void*>(vb,V_PASS+4*s)= s<=r.next()%9 ? static_cast<void*>(ped[s%NP].data()) : nullptr; // asientos llenos hasta cierto punto
                        if(r.coin(3))At<int32>(vb,V_TYPE)=9;
                        // las ranuras tras el array (0x484...) se rellenan de vez en cuando con nulos para probar el bucle hasta m_nMaxPassengers
                        if(r.coin(8)&&At<uint8>(vb,V_MAXPASS)>8)At<uint32>(vb,V_NUMPASS+4*(r.next()%4))=0; }
                    else if(nm=="CanBeDeleted"){ if(r.coin(3)){ At<uint8>(vb,V_NUMIN)=0; At<uint8>(vb,V_OUTFLAGS)=0; } if(r.coin(2))At<uint8>(vb,V_CREATEDBY)=uint8(1+r.next()%4); }
                    else if(nm=="CanBeDriven"){ if(r.coin(3))At<int32>(vb,V_SUBTYPE)=6; else if(r.coin(5))At<int32>(vb,V_SUBTYPE)=11; }
                    else if(nm=="CanDoorsBeDamaged"){ }
                    else if(nm=="CanPedEnterCar"){
                        SetMat(vmat.data(),r,{0.1f,0.f,1.f},{0.8f}); VecAt(vb,V_MOVE,r,{0.2f,0.04f,0.16f,0.1f,0.f}); VecAt(vb,V_TURN,r,{0.2f,0.04f,0.16f,0.1f,0.f});
                        if(r.coin(3))At<int32>(vb,V_TYPE)=9; }
                    else if(nm=="CanPedJumpOutCar"){
                        aPed=occupantArg();
                        SetMat(vmat.data(),r,{0.3f,0.f,1.f},{0.8f}); VecAt(vb,V_MOVE,r,{0.07f,0.1f,0.5f,0.2f,0.3162278f,0.2645751f,0.7071068f,0.31622776f,0.f});
                        VecAt(vb,V_TURN,r,{0.2f,0.05f,0.5f,0.f});
                        { static const int32 ty[]{0,0,0,9,3,4,1,2,5,6}; At<int32>(vb,V_TYPE)=ty[r.next()%10]; At<int32>(vb,V_SUBTYPE)=ty[r.next()%10]; }
                        { const uint32 now=AtA<uint32>(G_TIME); static const int32 dt[]{-2,-1,0,1,2,999,1000,1001,5000}; At<uint32>(vb,V_LASTCOL)=now-1000u+uint32(dt[r.next()%9])-uint32(r.coin(3)?r.next()%3:0); }
                        if(r.coin(8))At<void*>(vb,V_PASS)=nullptr; }
                    else if(nm=="CanPedLeanOut"){ aPed=occupantArg(); At<uint8>(hand.data(),H_ANIM)= r.coin(2) ? uint8(12+r.next()%8) : uint8(r.next()); }
                    else if(nm=="CanPedOpenLocks"){ aPed=PedP(int(r.next()%(NP+1))); }
                    else if(nm=="CanPedStepOutCar"){
                        aB=r.coin(); SetMat(vmat.data(),r,{0.1f,0.f,1.f},{0.8f}); VecAt(vb,V_MOVE,r,{0.01f,0.05f,0.f,0.02f}); VecAt(vb,V_TURN,r,{0.02f,0.f,0.01f});
                        if(r.coin(3)){ At<float>(vb,V_MOVE+8)=Thr(r,{0.05f},true); }
                        if(r.coin(3))At<int32>(vb,V_TYPE)=5; }
                    else if(nm=="CanVehicleBeDamaged"){
                        { const uint32 m=r.next()%6; aEnt= m==0 ? nullptr : (m==1 ? reinterpret_cast<CEntity*>(playerRet) : (m==2 ? reinterpret_cast<CEntity*>(pvehRet) : (m==3 ? reinterpret_cast<CEntity*>(PedP(0)) : reinterpret_cast<CEntity*>(veh.data())))); }
                        aWeapon=r.coin(2) ? int32(r.next()%60) : int32(r.next());
                        At<uint8>(vb,V_FLAGS+2)=uint8(r.coin(5) ? At<uint8>(vb,V_FLAGS+2)&~0x20 : At<uint8>(vb,V_FLAGS+2)|0x20);
                        At<float>(vb,V_HEALTH)=Thr(r,{250.f,249.99997f,1000.f,0.f},true); dmgFlag=r.coin(); }
                    else if(nm=="CarHasRoof"){ }
                    else if(nm=="ChangeLawEnforcerState"){ aB=r.coin(); }
                    else if(nm=="ClearGettingInFlags"||nm=="ClearGettingOutFlags"||nm=="SetGettingInFlags"||nm=="SetGettingOutFlags"){ aU=r.coin(3) ? r.next()&0xFFu : (1u<<(r.next()%8)); }
                    else if(nm=="SetWindowOpenFlag"||nm=="ClearWindowOpenFlag"){ aU=r.coin(3) ? r.next()&0xFFu : r.next()%32u; }
                    else if(nm=="IsDriver-ModelID"||nm=="IsPassenger-ModelID"){
                        const uint32 m=r.next()%6; const int16 mi= At<int16>(ped[r.next()%NP].data(),P_MODEL);
                        aI= m==0 ? int32(mi) : (m==1 ? int32(uint16(mi)) : (m==2 ? int32(r.next()) : (m==3 ? int32(r.next()%700) : (m==4 ? int32(mi)+int32(r.coin()?1:-1) : int32(0x10000)+int32(mi))))); }
                    else if(nm=="IsDriver-Ped"||nm=="IsPassenger-Ped"){ aPed=occupantArg(); }
                    else if(nm=="IsLawEnforcementVehicle"||nm=="UsesSiren"){
                        static const uint16 ms[]{427,428,429,430,431,432,433,434,489,490,491,522,523,524,527,528,529,595,596,597,598,599,600,601,602,407,408,415,416,417,422,423,424,432,431,0,1,65535,32768,32769,19990};
                        At<uint16>(vb,V_MODEL)= r.coin(3) ? uint16(r.next()%700) : (r.coin(8) ? uint16(r.next()) : ms[r.next()%41]); }
                    else if(nm=="IsOnItsSide"){ SetMat(vmat.data(),r,{0.9f},{0.8f,0.8f}); }
                    else if(nm=="IsUpsideDown"){ SetMat(vmat.data(),r,{0.9f,0.9f},{0.8f}); }
                    else if(nm=="IsVehicleNormal"){ if(r.coin(3))At<void*>(vb,V_DRIVER)=nullptr; if(r.coin(2))At<uint8>(vb,V_NUMPASS)=0; At<uint8>(vb,V_STATUS)= r.coin(2) ? uint8(0x28|(r.next()&7)) : uint8((r.next()%8)<<3|(r.next()&7)); }
                    else if(nm=="IsVehiclePointerValid"){ }
                    else if(nm=="SetVehicleCreatedBy"){ aU= r.coin(8) ? r.next()&0xFFu : r.next()%7u; if(r.coin(3))aU=At<uint8>(vb,V_CREATEDBY); }
                    else if(nm=="MakeDirty"){
                        uint8* cp=colpoint.data(); const uint32 nsurf=uint32(TOTAL_NUM_SURFACE_TYPES);
                        At<uint8>(cp,CP_SURF)=uint8(r.next()%nsurf);
                        VecAt(vb,V_MOVE,r,{0.06f,0.04f,0.1f,0.f,0.03f,0.05f});
                        At<float>(vb,V_DIRT)=Thr(r,{1.f,4.f,15.f,0.f,8.f,14.99f,4.01f},true); }
                    // ---- llamada ----
                    for(auto* h:hooks)h->State(candidate);
                    for(auto& e:p.v)e->Enable(true); // los parches de entrada se reafirman despues de fijar los hooks
                    json out; const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    out["switches"]=fn.slot>=0 ? ((target<0x400000||target>=0x1000000)==candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json ret=nullptr; void* const savedPool=AtA<void*>(G_VPOOL);
                    uintptr_t poolArg=0;
                    if(nm=="AddPassenger-Auto-Seat")ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*,CPed*)>(target)(v,aPed));
                    else if(nm=="AddPassenger-Fixed-Seat")ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*,CPed*,uint8)>(target)(v,aPed,uint8(aU)));
                    else if(nm=="CanBeDeleted"||nm=="CanBeDriven"||nm=="CanDoorsBeDamaged"||nm=="CanPedEnterCar"||nm=="CarHasRoof"||nm=="IsLawEnforcementVehicle"||nm=="IsOnItsSide"
                          ||nm=="IsUpsideDown"||nm=="IsVehicleNormal"||nm=="UsesSiren")ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*)>(target)(v));
                    else if(nm=="CanPedJumpOutCar"||nm=="CanPedLeanOut"||nm=="CanPedOpenLocks"||nm=="IsDriver-Ped"||nm=="IsPassenger-Ped")ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*,CPed*)>(target)(v,aPed));
                    else if(nm=="CanPedStepOutCar")ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*,bool)>(target)(v,aB));
                    else if(nm=="CanVehicleBeDamaged"){ bool flag=dmgFlag; ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*,CEntity*,int32,bool*)>(target)(v,aEnt,aWeapon,&flag)); out["flag"]=int(flag); }
                    else if(nm=="ChangeLawEnforcerState")reinterpret_cast<void(__thiscall*)(CVehicle*,bool)>(target)(v,aB);
                    else if(nm=="ClearGettingInFlags"||nm=="ClearGettingOutFlags"||nm=="SetGettingInFlags"||nm=="SetGettingOutFlags"||nm=="SetWindowOpenFlag"||nm=="ClearWindowOpenFlag")
                        reinterpret_cast<void(__thiscall*)(CVehicle*,uint8)>(target)(v,uint8(aU));
                    else if(nm=="IsDriver-ModelID"||nm=="IsPassenger-ModelID")ret=int(reinterpret_cast<bool(__thiscall*)(CVehicle*,int32)>(target)(v,aI));
                    else if(nm=="IsVehiclePointerValid"){
                        // puntero a un hueco del pool falso: inicio, interior, antes del almacenamiento, tras el final, nulo o aleatorio dentro del margen
                        uint8* const st=poolStore.data()+MARGIN; const uint32 m=r.next()%10;
                        poolArg= m<3 ? uintptr_t(st+SLOTSZ*(r.next()%POOLSLOTS)) : (m<6 ? uintptr_t(st+r.next()%(POOLSLOTS*SLOTSZ)) : (m==6 ? uintptr_t(st)-1-r.next()%(SLOTSZ-1) : (m==7 ? uintptr_t(st+POOLSLOTS*SLOTSZ+r.next()%0x600) : (m==8 ? 0u : uintptr_t(st+SLOTSZ*(r.next()%POOLSLOTS))))));
                        if(r.coin(3))poolArg=uintptr_t(st+SLOTSZ*(r.next()%POOLSLOTS)); // con la rama del vuelo: hueco libre/ocupado segun el estado aleatorio
                        AtA<void*>(G_VPOOL)=fakePool.data();
                        ret=int(reinterpret_cast<bool(__cdecl*)(CVehicle*)>(target)(reinterpret_cast<CVehicle*>(poolArg)));
                        AtA<void*>(G_VPOOL)=savedPool; out["arg"]=poolArg-uintptr_t(st); }
                    else if(nm=="SetVehicleCreatedBy")reinterpret_cast<void(__thiscall*)(CVehicle*,uint32)>(target)(v,aU);
                    else if(nm=="MakeDirty")reinterpret_cast<void(__thiscall*)(CVehicle*,CColPoint*)>(target)(v,reinterpret_cast<CColPoint*>(colpoint.data()));
                    for(auto* h:hooks)h->State(false);
                    for(auto& e:p.v)e->Enable(true);
                    out["ret"]=ret; out["veh"]=Region_(veh.data(),0x5D0); out["vtail"]=Hash(veh.data()+0x5D0,veh.size()-0x5D0);
                    json peds=json::array(); for(auto& pd:ped)peds.push_back(Hash(pd.data(),pd.size())); out["peds"]=peds;
                    json mats=json::array(); mats.push_back(Hash(vmat.data(),vmat.size())); for(auto& m:pmat)mats.push_back(Hash(m.data(),m.size())); out["mats"]=mats;
                    out["player"]=Hash(plr.data(),plr.size()); out["pveh"]=Hash(pveh.data(),pveh.size()); out["hand"]=Hash(hand.data(),hand.size());
                    out["clump"]=Hash(clump.data(),clump.size()); out["frames"]={Hash(frame[0].data(),0x40),Hash(frame[1].data(),0x40)}; out["colpoint"]=Hash(colpoint.data(),colpoint.size());
                    out["mi"]={Hash(vmi[0].data(),0x100),Hash(vmi[1].data(),0x100),Hash(vmi[2].data(),0x100),Hash(vstruct[0].data(),0x400),Hash(vstruct[1].data(),0x400),Hash(vstruct[2].data(),0x400)};
                    out["pool"]={Hash(poolStore.data(),poolStore.size()),Hash(poolState.data(),poolState.size()),Hash(fakePool.data(),fakePool.size())};
                    out["globals"]={AtA<uint32>(G_TIME),AtA<uint32>(G_STEP),AtA<uint32>(G_RAIN),AtA<uint32>(G_LAW),AtA<void*>(G_VPOOL)==savedPool};
                    out["trace"]=trace;
                    RestoreRegions();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
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
