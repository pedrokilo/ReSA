#pragma once
// Infraestructura comun del banco WeaponFinal2 (nueve funciones pendientes de CWeapon), estilo PadMouse1/PoolsWave4:
//   - trazas de llamadas salientes (T) con punteros identificados por nombre (Nm/Name);
//   - dobles (EntryPatch) que se aplican a la direccion del EXE y al cuerpo de la DLL (Patches::Add), solo durante el caso;
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
#include "Pickups.h"
#include "Pickup.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Radar.h"
#include "Shadows.h"
#include "Coronas.h"
#include "Stats.h"
#include "Garages.h"
#include "Font.h"
#include "Messages.h"
#include "Camera.h"
#include "Pad.h"
#include "PedIntelligence.h"
#include "World.h"
#include "General.h"
#include "GenericGameStorage.h"
#include "PlayerInfo.h"
#include "CutsceneMgr.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Crime.h"
#include "ShotInfo.h"
#include "Explosion.h"
#include "ProjectileInfo.h"
#include "BulletInfo.h"
#include "CreepingFire.h"
#include "Sprite.h"
#include "Collision/Collision.h"
#include "Audio/AudioEngine.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Object/Object.h"
#include "InterestingEvents.h"
#include "WeaponEffects.h"
#include "Birds.h"
#include "Cheat.h"
#include "Timer.h"
#include "Pools/Pools.h"
#include "Pools/PedPool.h"
#include "Pools/ObjectPool.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Object/CutsceneObject.h"
#include "ObjectData.h"
#include "PlayerPedData.h"
#include "Events/EventDamage.h"
#include "Events/EventGunShot.h"
#include "Events/EventGunShotWhizzedBy.h"
#include "Events/EventVehicleDamageWeapon.h"
#include "Events/EventGroup.h"
#include "Events/EventGlobalGroup.h"
#include "RwHelper.h"
#include "PedDamageResponseCalculator.h"
#include "Tasks/TaskManager.h"
#include "Fx/FxManager.h"
#include "PointLights.h"
#include "BulletTraces.h"
#include "WaterLevel.h"
#include "Glass.h"
#include "PedGroups.h"
#include "Localisation.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "References.h"
#include "LoadMonitor.h"
#include "../ProbeRuntime.hpp"

namespace WeaponFinal2Live {
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
// Generador de numeros aleatorios del caso en curso: lo consumen los dobles de rand (0x821B1E) y de CGeneral::GetRandomNumber
inline Rng* gRng{};
inline uint32 NextRand15(){ return gRng ? (gRng->next()>>7)&0x7FFF : 0u; }

// --- punteros con nombre en las trazas ---
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

inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()>>7); }
inline uint32 Bits(float f){ return std::bit_cast<uint32>(f); }
inline json Bytes(const void* p,size_t n){ return Fnv(p,n); }
inline json V3(const CVector& v){ return json::array({Bits(v.x),Bits(v.y),Bits(v.z)}); }

// --- tabla de funciones: {clase, nombre del hook, direccion EXE, vtable, ranura, casos}; la expresion regular de run_local.py la lee ---
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
}
