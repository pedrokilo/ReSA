#pragma once
// Infraestructura del banco StreamingFinal4 (CStreaming, 94 funciones): trazas sin reservas, identificadores de punteros, arena para
// operator new/delete, conjuntos de parches (EntryPatch en almacenamiento estatico), busqueda de hooks y copia/restauracion de memoria global.
// Ver README.md.
#include <fstream>
#include <atomic>
#include <thread>
#include <array>
#include <vector>
#include <memory>
#include <bit>
#include <cmath>
#include <cstring>
#include <string>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Streaming.h"
#include "StreamingInfo.h"
#include "CdStreamInfo.h"
#include "Entity.h"
#include "Building.h"
#include "Dummy.h"
#include "Object.h"
#include "Ped.h"
#include "World.h"
#include "Camera.h"
#include "Population.h"
#include "PopCycle.h"
#include "Cheat.h"
#include "TxdStore.h"
#include "General.h"
#include "LoadedCarGroup.h"
#include "Pools/Pools.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace StreamingFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio, streaming) llaman a funciones parcheadas; esas llamadas no se trazan ni cambian el estado del banco.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
template<class T=uint32> inline T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
// Resumen rapido de 64 bits por palabras (para regiones grandes)
inline uint64 FastHash(const void* p,size_t n){
    uint64 h=0x9E3779B97F4A7C15ull; const auto* b=static_cast<const uint8*>(p); size_t i=0;
    for(;i+8<=n;i+=8){ uint64 v; std::memcpy(&v,b+i,8); h^=v; h*=0xFF51AFD7ED558CCDull; h^=h>>32; }
    for(;i<n;++i){ h^=b[i]; h*=0x100000001B3ull; }
    return h^n;
}
inline uint32 S(const char* s) { return s ? (Fnv32(s,strlen(s))|1u) : 0u; }
inline uint32 Mix(uint32 a,uint32 b) { uint32 h=a*2654435761u^(b+0x9E3779B9u+(a<<6)+(a>>2)); h^=h>>15; h*=2246822519u; h^=h>>13; return h; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de regiones para Id(): convierte un puntero del mundo falso en (region, desplazamiento) estable entre las dos ejecuciones
// ---------------------------------------------------------------------------------------------------------------------
inline std::array<std::pair<uintptr_t,size_t>,64> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<24|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    if(u<0x1000000)return uint32(u);       // datos/codigo del EXE
    return 0xE0000000u;
}
// Valor de 32 bits que puede ser un puntero del mundo falso o un dato
inline uint32 Val(uint32 v){ const auto u=uintptr_t(v); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<24|(u-regs[k].first)); return v; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas
// ---------------------------------------------------------------------------------------------------------------------
struct Rec { uint32 t,a,b,c,d; };
inline std::array<Rec,40000> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(recOverflow)r.push_back("overflow"); return r; }

// ---------------------------------------------------------------------------------------------------------------------
// Arena de operator new / MallocAlign (sin reservas reales durante la llamada probada)
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t ARENA_SIZE=0x100000;
alignas(4096) inline std::array<uint8,ARENA_SIZE> arena{}; inline size_t arenaTop{};
enum : uint32 { T_ALLOC=1, T_DELETE, T_ALIGNED, T_FREEALIGN };
inline void* ArenaAlloc(size_t n,size_t align=16,uint32 tag=T_ALLOC){
    uintptr_t base=uintptr_t(arena.data())+arenaTop; base=(base+align-1)&~uintptr_t(align-1);
    const size_t off=base-uintptr_t(arena.data());
    if(off+n>arena.size()){ Push(tag,uint32(n),0xFFFFFFFFu); return nullptr; }
    arenaTop=(off+n+15)&~size_t(15); Push(tag,uint32(n),uint32(off)); return arena.data()+off;
}

// ---------------------------------------------------------------------------------------------------------------------
// Hooks y direcciones
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
// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }

// ---------------------------------------------------------------------------------------------------------------------
// Conjuntos de parches. Un stub se aplica en la direccion del EXE, en el simbolo de la DLL y en el cuerpo de la DLL. Los EntryPatch viven en
// almacenamiento estatico: con operator new parcheado un vector/unique_ptr consumiria la arena con bytes que dependen del lado.
// Todos los Body() se calculan ANTES de crear ningun EntryPatch (los saltos de un hook desactivado apuntan al EXE y los de uno activo a la DLL).
// ---------------------------------------------------------------------------------------------------------------------
struct Stub { uint64 mask; uintptr_t exe,dll,body; void* fn; };
inline std::vector<Stub> stubs;
inline void AddStub(uint64 mask,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({mask,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
// Hook por clase y nombre (nombres de docs/hooks.csv). Devuelve falso si el hook no existe.
inline bool AddHookStub(uint64 mask,uintptr_t exe,const char* cls,const char* name,void* fn){
    auto* h=FindHook(cls,name); const auto our=OurAddress(h); if(!h||!our)return false;
    AddStub(mask,exe,our,fn); return true;
}
struct PatchSet {
    static constexpr size_t MAXP=480;
    alignas(EntryPatch) static inline unsigned char storage[MAXP][sizeof(EntryPatch)];
    static inline uintptr_t used[MAXP];
    size_t n{};
    explicit PatchSet(uint64 mask){ for(auto& s:stubs)if(s.mask&mask){ Put(s.exe,s.fn); Put(s.dll,s.fn); Put(s.body,s.fn); } }
    PatchSet(const PatchSet&)=delete; PatchSet& operator=(const PatchSet&)=delete;
    void Put(uintptr_t at,void* fn){ if(!at||n>=MAXP)return; for(size_t k=0;k<n;++k)if(used[k]==at)return; used[n]=at; new(storage[n]) EntryPatch(at,fn); ++n; }
    ~PatchSet(){ while(n>0){ --n; reinterpret_cast<EntryPatch*>(storage[n])->~EntryPatch(); } }
};
template<class P> inline void* FnPtr(P f){ return reinterpret_cast<void*>(f); }

// ---------------------------------------------------------------------------------------------------------------------
// Instantanea de memoria global (se restaura al terminar cada ejecucion)
// ---------------------------------------------------------------------------------------------------------------------
struct Saved { uintptr_t addr; const char* tag; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n,const char* tag){ saved.push_back({a,tag,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }

template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }
}
