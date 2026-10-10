#pragma once
// Infraestructura comun del banco AutomobileFinal4 (CAutomobile, 99 funciones): registro de trazas sin reservas de memoria,
// identificadores estables de punteros, conjunto de parches con almacenamiento estatico (EntryPatch), globales guardados,
// mundo falso compartido (coches, matrices, marcos, model infos) y utilidades. Cada grupo de funciones (CasesG1..G5.hpp)
// aporta sus propios dobles y fixtures encima de esto. Ver README.md.
#include <fstream>
#include <array>
#include <vector>
#include <memory>
#include <bit>
#include <cmath>
#include <cstring>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "Models/VehicleModelInfo.h"
#include "Models/ModelInfo.h"
#include "World.h"
#include "Timer.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace AutomobileFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan ni tocan el estado.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
template<class T=uint32> inline T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 S(const char* s){ return s ? (Fnv32(s,strlen(s))|1u) : 0u; }
// Etiqueta de traza: hash FNV de un nombre (sin enumeracion compartida entre grupos).
constexpr uint32 TG(const char* s){ uint32 h=2166136261u; for(;*s;++s){ h^=uint8(*s); h*=16777619u; } return h; }

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso compartido. Direcciones estables (estaticas); Id() las convierte en (region, desplazamiento) para las trazas.
// ---------------------------------------------------------------------------------------------------------------------
constexpr int NCAR=4, NMAT=10, NRWM=24, NFR=48, NAT=16, NCLUMP=4, NMI=4, NHD=4, NCOLM=4;
constexpr size_t CAR_SIZE=0xA20, MAT_SIZE=0x60, FR_SIZE=0x100, AT_SIZE=0x100, MI_SIZE=0x500, VS_SIZE=0x330, HD_SIZE=0xE0, FH_SIZE=0x80, COLM_SIZE=0x40, CDATA_SIZE=0x40;
struct World {
    alignas(16) uint8 car[NCAR][CAR_SIZE];       // CAutomobile falsos (0 = el probado; 1..3 = otros vehiculos/remolques/victimas)
    alignas(16) uint8 guard[64];                 // guarda tras los coches (0xCD)
    alignas(16) uint8 mat[NMAT][MAT_SIZE];       // CMatrixLink falsos: right 0, up... ver Ctx::SetMatrix (CMatrix 0x48 + enlaces)
    alignas(16) uint8 rwm[NRWM][0x40];           // RwMatrix falsos (attach, ltm)
    alignas(16) uint8 fr[NFR][FR_SIZE];          // RwFrame falsos: object 0, parent 4, modelling 0x10, ltm 0x50, hijo 0x98, hermano 0x9C
    alignas(16) uint8 at[NAT][AT_SIZE];          // RpAtomic falsos
    alignas(16) uint8 clump[NCLUMP][0x40];       // RpClump falsos (byte de tipo 2 en 0)
    alignas(16) uint8 mi[NMI][MI_SIZE];          // CVehicleModelInfo falsos
    alignas(16) uint8 vs[NMI][VS_SIZE];          // CVehicleStructure falsas
    alignas(16) uint8 hd[NHD][HD_SIZE];          // tHandlingData falsos
    alignas(16) uint8 fh[NHD][FH_SIZE];          // tFlyingHandlingData falsos
    alignas(16) uint8 colm[NCOLM][COLM_SIZE];    // CColModel falsos
    alignas(16) uint8 cdata[NCOLM][CDATA_SIZE];  // CCollisionData falsos
    alignas(16) uint8 lines[NCOLM][16][0x20];    // CColLine falsas (4 de coche + 8 del rhino + holgura)
    alignas(16) uint8 misc[0x400];               // utilidad libre
};
inline World w;
alignas(16) inline std::array<uint8,0x20000> arena{}; inline size_t arenaTop{};

inline uint8* Cr(int k){ return w.car[k]; }
inline CAutomobile* Car(int k){ return reinterpret_cast<CAutomobile*>(w.car[k]); }

// Registro de regiones para Id()
inline std::array<std::pair<uintptr_t,size_t>,96> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }

inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
inline bool IsCode(uintptr_t a){ MEMORY_BASIC_INFORMATION mi{}; return a>=0x10000&&VirtualQuery(reinterpret_cast<void*>(a),&mi,sizeof(mi))&&mi.State==MEM_COMMIT&&(mi.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)); }
inline uintptr_t Final(uintptr_t a){ for(int d=0;d<4&&IsCode(a)&&*reinterpret_cast<uint8*>(a)==0xE9;++d)a=a+5+*reinterpret_cast<int32*>(a+1); return a; }
// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }

// Identificador estable de un puntero: (region<<20 | desplazamiento) para el mundo falso, 0xD0000000 para la pila, la propia
// direccion para el EXE (< 0x1000000) y 0xE0000000 para cualquier otra cosa (montón, DLL).
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    if(u<0x1000000)return uint32(u);
    return 0xE0000000u;
}
// Valor de 32 bits que puede ser un puntero del mundo falso o un dato (color, banderas...)
inline uint32 Val(uint32 v){ const auto u=uintptr_t(v); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first)); return v; }
inline bool Known(const void* p){ const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return true; return false; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas (hasta 6 argumentos por evento)
// ---------------------------------------------------------------------------------------------------------------------
template<class T> inline uint32 Cv(T v){
    if constexpr(std::is_floating_point_v<T>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<T>) return Id(reinterpret_cast<const void*>(v));
    else if constexpr(std::is_enum_v<T>) return uint32(static_cast<std::underlying_type_t<T>>(v));
    else if constexpr(std::is_same_v<T,bool>) return v ? 1u : 0u;
    else return uint32(v);
}
struct Rec { uint32 t; uint32 a[6]; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{}; inline bool recOverflow{};
template<class... A> inline void Push(uint32 t,A... args){
    static_assert(sizeof...(A)<=6,"Push admite hasta 6 argumentos");
    if(nrec>=recs.size()){ recOverflow=true; return; }
    Rec& r=recs[nrec++]; r.t=t; for(auto& x:r.a)x=0; uint32 k=0; ((r.a[k++]=Cv(args)),...);
}
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a[0],recs[i].a[1],recs[i].a[2],recs[i].a[3],recs[i].a[4],recs[i].a[5]}); if(recOverflow)r.push_back("overflow"); return r; }

// Azar determinista de los dobles del EXE (0x821B1E) y de CGeneral::GetRandomNumber
inline uint32 rngState{1};
inline uint32 NextRng(){ rngState^=rngState<<13; rngState^=rngState>>17; rngState^=rngState<<5; return rngState; }

// Arena de operator new/delete (sin reservas durante la llamada probada)
inline void* ArenaAlloc(size_t n){ const size_t off=arenaTop; if(n>arena.size()-off){ Push(TG("alloc"),uint32(n),0xFFFFFFFFu); return nullptr; } arenaTop=(off+n+15)&~size_t(15); Push(TG("alloc"),uint32(n),uint32(off)); return arena.data()+off; }

// ---------------------------------------------------------------------------------------------------------------------
// Dobles comunes (todos respetan la convencion y el 'ret N' del original; los hilos ajenos no se trazan)
// ---------------------------------------------------------------------------------------------------------------------
inline int __cdecl S_RandExe(){ if(!Mine())return 0x1234; const uint32 v=NextRng()&0x7FFF; Push(TG("rand"),v); return int(v); }
inline uint16 __cdecl S_RandDll(){ return uint16(S_RandExe()); }
inline void* __cdecl S_New(size_t n){ if(!Mine())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n); return ArenaAlloc(n); }
inline void __cdecl S_Delete(void* p){ if(!Mine()){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } Push(TG("delete"),p); }
inline void* tbPlayerVehicle{}; inline void* tbPlayerPed{};
inline void* __cdecl S_FindPlayerVehicle(int32 pid,bool remote){ if(!Mine())return nullptr; Push(TG("FindPlayerVehicle"),pid,remote); return tbPlayerVehicle; }
inline void* __cdecl S_FindPlayerPed(int32 pid){ if(!Mine())return nullptr; Push(TG("FindPlayerPed"),pid); return tbPlayerPed; }

// ---------------------------------------------------------------------------------------------------------------------
// Hooks, direcciones de la DLL y conjuntos de parches
// ---------------------------------------------------------------------------------------------------------------------
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurAddress(ReversibleHooks::ReversibleHook::Base* h) {
    if(!h)return 0;
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return 0;
}
// Grupos de parches comunes (owner 0). Cada grupo de funciones (owner 1..5) dispone de 64 bits propios.
enum : uint64 { G_RNG=1,G_NEW=2,G_PLAYER=4 };
struct StubDef { uint32 owner; uint64 mask; uintptr_t exe,dll,body; void* fn; };
inline std::vector<StubDef> stubs;
// IMPORTANTE (leccion 11/20): Body(dll) se calcula AL REGISTRAR (al arrancar, con todos los hooks activos) y ANTES de crear
// ningun EntryPatch. dllSym = direccion del simbolo de la DLL (0 si el colaborador no existe en la DLL como funcion propia).
inline void AddStub(uint32 owner,uint64 mask,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({owner,mask,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
// Igual que AddStub pero el simbolo de la DLL sale del hook registrado cls::name (devuelve false si no existe).
inline bool AddHookStub(uint32 owner,uint64 mask,uintptr_t exe,const char* cls,const char* name,void* fn){
    auto* h=FindHook(cls,name); const auto our=OurAddress(h); if(!h||!our)return false;
    AddStub(owner,mask,exe,our,fn); return true;
}
// Conjunto de parches SIN reservas de memoria: los EntryPatch viven en almacenamiento estatico. Con operator new parcheado
// (G_NEW) un vector/unique_ptr consumiria la arena con bytes que dependen del lado (direcciones de salto) y ensuciaria la traza.
// Usa siempre dentro de un bloque { auto P=MakePatches(...); llamada; } y construye el JSON de salida DESPUES de cerrarlo.
struct PatchSet {
    static constexpr size_t MAXP=320;
    alignas(EntryPatch) static inline unsigned char storage[MAXP][sizeof(EntryPatch)];
    static inline uintptr_t used[MAXP];
    size_t n{};
    PatchSet(uint64 common,uint32 own,uint64 ownMask){
        for(auto& s:stubs)if((s.owner==0&&(s.mask&common))||(own&&s.owner==own&&(s.mask&ownMask))){ Put(s.exe,s.fn); Put(s.dll,s.fn); Put(s.body,s.fn); }
    }
    PatchSet(const PatchSet&)=delete; PatchSet& operator=(const PatchSet&)=delete;
    void Put(uintptr_t at,void* fn){ if(!at||n>=MAXP)return; for(size_t k=0;k<n;++k)if(used[k]==at)return; used[n]=at; new(storage[n]) EntryPatch(at,fn); ++n; }
    ~PatchSet(){ while(n>0){ --n; reinterpret_cast<EntryPatch*>(storage[n])->~EntryPatch(); } }
};
// MakePatches(comunes, propietario, mascara propia). Ejemplo: MakePatches(G_NEW|G_RNG, 2, G2_RW)
inline PatchSet MakePatches(uint64 common,uint32 own=0,uint64 ownMask=0){ return PatchSet(common,own,ownMask); }
template<class P> inline void* FnPtr(P f){ return reinterpret_cast<void*>(f); }

// Instantanea de globales (se restaura al terminar cada ejecucion)
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }

template<class R,class... A> inline R CallThis(uintptr_t a,void* self,A... args){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }
template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }
template<class R,class... A> inline R CallStd(uintptr_t a,A... args){ return reinterpret_cast<R(__stdcall*)(A...)>(a)(args...); }

// ---------------------------------------------------------------------------------------------------------------------
// Constantes del EXE
// ---------------------------------------------------------------------------------------------------------------------
constexpr uintptr_t VT_AUTOMOBILE=0x871120;          // vtable de CAutomobile (71 huecos)
constexpr uintptr_t HANDLING_BASE=0xC2B9DC;          // gHandlingDataMgr: tHandlingData[210] de 0xE0 bytes
constexpr uintptr_t MODELPTRS=0xA9B0C8;              // CModelInfo::ms_modelInfoPtrs
constexpr uintptr_t VT_VEHMI=0x85C5C8;               // vtable de CVehicleModelInfo (17 huecos)
// Direcciones de las 71 funciones de la vtable de CAutomobile en el EXE (hooks desactivados). La vtable falsa de los coches
// de prueba se rellena con ESTAS direcciones (no con las de la vtable viva, que dependen del estado de los hooks).
inline constexpr std::array<uintptr_t,71> kExeVt{
    0x6b1330,0x5347d0,0x544a30,0x5424c0,0x403e20,0x6a6590,0x533700,0x533d30,0x6d6410,0x5449b0,0x6b1880,0x54dfb0,0x54db10,0x54dec0,0x6a9ca0,0x6d6640,
    0x6d0e90,0x6aab50,0x6a2b10,0x553f20,0x5533d0,0x6a6290,0x6ace70,0x6a29c0,0x6ad690,0x6a2210,0x6a2250,0x6a6ae0,0x6d56c0,0x6a62c0,0x6a2270,0x6a6350,
    0x6a2290,0x6a63e0,0x6a22d0,0x6a6470,0x6a2310,0x6a6500,0x6a2330,0x6a2350,0x6a3030,0x6b3780,0x6b3bb0,0x6a3060,0x6a32b0,0x6a3850,0x6df4a0,0x6a6280,
    0x6a65d0,0x6a1ed0,0x6a3440,0x6b3e90,0x6a47f0,0x6a62b0,0x6a3770,0x6a62a0,0x6a7650,0x6d1f30,0x6d2030,0x6af1d0,0x6af250,0x6b4410,0x6a4400,0x6a6090,
    0x5d47e0,0x5d2980,0x6b4800,0x6a2ae0,0x6a8500,0x6a45c0,0x6afb10};
// Hueco de la vtable por funcion (verificado con rd.py i871120...): ver tabla de LiveComparison.hpp.
inline std::array<uintptr_t,72> vtFake{};      // vtable falsa de los coches (copia de kExeVt con sustituciones por ejecucion)
inline void ResetVt(){ for(size_t k=0;k<kExeVt.size();++k)vtFake[k]=kExeVt[k]; vtFake[71]=0; }
inline void SetVt(size_t slot,void* fn){ vtFake[slot]=reinterpret_cast<uintptr_t>(fn); }
}
