#pragma once
// Infraestructura comun del banco CheatFinal3 (CCheat completa), estilo WeaponFinal2/PadMouse1:
//   - trazas de llamadas salientes (T) con punteros identificados por nombre (Nm/Name);
//   - dobles (EntryPatch) que se aplican a la direccion del EXE y al cuerpo de la DLL (Patches::Add), solo mientras dura cada funcion;
//   - regiones globales guardadas y restauradas en cada ejecucion (Snap/RestoreRegions);
//   - solo el hilo del banco (owner) traza o altera estado: los hilos ajenos (audio, etc.) reciben un resultado inocuo.
#include <fstream>
#include <set>
#include <cmath>
#include <utility>
#include <format>
#include <bit>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cheat.h"
#include "KeyGen.h"
#include "Pad.h"
#include "KeyboardState.h"
#include "PedIntelligence.h"
#include "PedScanner.h"
#include "Acquaintance.h"
#include "PedType.h"
#include "World.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "Wanted.h"
#include "CutsceneMgr.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Stats.h"
#include "Hud.h"
#include "Text/Text.h"
#include "Weather.h"
#include "Timer.h"
#include "Clock.h"
#include "Population.h"
#include "PostEffects.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "PedClothesDesc.h"
#include "Clothes.h"
#include "Pickups.h"
#include "ModelIndices.h"
#include "References.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Bmx.h"
#include "Entity/Vehicle/Boat.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Vehicle/MonsterTruck.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/QuadBike.h"
#include "Entity/Vehicle/Trailer.h"
#include "Pools/Pools.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "Events/EventAcquaintancePedHate.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventDamage.h"
#include "Events/EventGroup.h"
#include "PedDamageResponseCalculator.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleJetPack.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "../ProbeRuntime.hpp"

namespace CheatFinal3Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class V=uint32> V& At(void* base,size_t off) { return *reinterpret_cast<V*>(static_cast<uint8*>(base)+off); }
template<class V=uint32> V& AtA(uintptr_t a) { return *reinterpret_cast<V*>(a); }

struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    int32 pickI(std::initializer_list<int64_t> l){ return int32(l.begin()[next()%l.size()]); }
    float pickF(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    // flotante finito en [lo, hi] con 1/256 de paso (siempre representable: nunca NaN/Inf/sNaN)
    float unit(float lo,float hi){ return lo+(hi-lo)*float(next()%257)/256.0f; } };

// Hilo del banco: otros hilos del juego (audio, etc.) pueden llamar a funciones parcheadas; sus llamadas no se trazan ni cambian estado.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

// --- punteros con nombre en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
// cadena C leida con cuidado (los literales del EXE/DLL o las claves del banco): como mucho 24 caracteres
inline json Str(const char* s){
    if(!s)return nullptr;
    std::string o; for(int k=0;k<24&&s[k];++k)o.push_back(s[k]);
    return o;
}
inline json Jv(const char* s){ return Str(s); }
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

// --- modulo de la DLL y busqueda de cuerpos (se calculan ANTES de crear ningun EntryPatch, con los hooks en su estado inicial) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo). Si el destino cae en el EXE (hook desactivado) se detiene: Body() no entra en el EXE.
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(Addr(f)); }

inline uint32 Bits(float f){ return std::bit_cast<uint32>(f); }
inline uint32 U(const void* p){ return uint32(reinterpret_cast<uintptr_t>(p)); }
inline std::string Hex(const void* p,size_t n){ std::string o; const auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i)o+=std::format("{:02x}",b[i]); return o; }
inline float QNaN(){ return std::bit_cast<float>(0x7FC00000u); }

// --- tabla de funciones: {clase, nombre del hook, direccion EXE, vtable, ranura, casos}; la expresion regular de run_local.py la lee ---
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
}
