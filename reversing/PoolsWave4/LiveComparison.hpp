#pragma once
// Banco dentro del motor (flujo viejo, estilo WorldWave2/StorageFinal1/Small2): las 7 funciones pendientes de CPools.
//   Initialise (0x550F10), ShutDown (0x5519F0), Save (0x5D0880), Load (0x5D0890), LoadObjectPool (0x5D4A40), LoadPedPool (0x5D2D70)
//   y LoadVehiclePool (0x5D2A20).
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo) con fixtures deterministas por caso, el
// estado completo de los pools falsos y la traza de todas las llamadas salientes con sus argumentos. NO se toca ningun pool real,
// ningun fichero de guardado ni el streaming real.
//
// Initialise / ShutDown: el unico estado real tocado son los 17 punteros globales de pool (0xB74484..0xB744C4), que se guardan y se
// restauran en cada ejecucion. TODA la reserva y liberacion de memoria se dobla: durante la llamada (y solo durante ella) se sustituyen
// operator new (0x82119A), operator new[] (0x821195), operator delete (0x8214BD) y operator delete[] (0x8213AE) por dobles con
// registro SIN asignaciones (matriz fija) que sirven memoria de un arena estatico de 5 MB; los hilos ajenos reciben el operator new
// original mediante un trampolin (copia de sus primeros 5 bytes + salto a 0x82119F) y el delete original (0x82413F), asi que nada de
// lo que hagan otros hilos se desvia. En Initialise se ejecutan los constructores de CPool REALES del EXE sobre el arena (el candidato
// reproduce su semantica) y se comparan: orden y tamano de cada reserva (nuevo/nuevo[]), los 17 punteros globales (como desplazamiento
// en el arena), la cabecera de 0x14 bytes de cada pool y todo el arena usado (+256 bytes de guarda) con cuatro patrones de relleno. Con
// 18 variantes de fallo de operator new (ninguno o el de la reserva escalar del pool k) se comprueba la rama "puntero nulo". En ShutDown
// los 17 pools son cabeceras falsas del arena (puntero nulo, tamano <= 0, propietario 0/1/2/0xFF, punteros arbitrarios que nunca se
// desreferencian) y se comparan las llamadas a delete[]/delete/printf (0x821982), las cabeceras resultantes y los globales (que NO se anulan).
//
// Save / Load: SavePedPool (0x5D4B40), SaveObjectPool (0x5D4940), LoadPedPool (0x5D2D70) y LoadObjectPool (0x5D4A40) se doblan con traza y
// resultado programado (en la direccion del EXE y en el cuerpo de la DLL); los pools de peds y objetos globales se cambian por pools
// falsos vacios y SaveDataToWorkBuffer/LoadDataFromWorkBuffer (0x5D1270/0x5D1300) tambien se doblan (red de seguridad: si el compilador
// hubiese integrado alguna subfuncion en la DLL, la traza difiere y no se toca el bufer real). Se comprueba que 0xC16EE8..0xC16EFD no cambia.
//
// LoadObjectPool / LoadPedPool / LoadVehiclePool: el "fichero de guardado" es un flujo en memoria (LoadDataFromWorkBuffer doblado: traza
// tamano, posicion y hash de los bytes entregados). Los pools globales de objetos (0xB7449C), peds (0xB74490) y vehiculos (0xB74494) se
// cambian temporalmente por pools falsos de 16 ranuras (misma disposicion que el CPool del EXE; las ranuras 12..15 siempre estan libres
// para que la busqueda de ranura libre de CreateAtRef termine) con el almacenamiento relleno de bytes aleatorios. El operator new con
// referencia (0x5A1EF0 / 0x5E4730 / 0x6E2D60 y los de la DLL) y CreateAtRef (0x59F610) se EJECUTAN de verdad sobre esos pools. Doblados:
// CObject::CObject (0x5A1D70 y el simbolo exportado de la DLL), CPlayerPed::CPlayerPed (0x60D5B0), los 10 constructores de vehiculo
// (0x6C8D60 CMonsterTruck, 0x6CE370 CQuadBike, 0x6C4190 CHeli, 0x6C8E20 CPlane, 0x6F2940 CBoat, 0x6F6030 CTrain, 0x6BF430 CBike,
// 0x6BF820 CBmx, 0x6D03A0 CTrailer, 0x6B0A90 CAutomobile), CObjectSaveStructure::Extract (0x5D2220), CPopulation::ConvertToDummyObject
// (0x614670; libera la ranura como el original), CWorld::Add (0x563220), CStreaming::RequestModel (0x4087E0) y LoadAllRequestedModels
// (0x40EA10), CCarCtrl::UpdateCarCount (0x424000). Los objetos creados llevan una vtable falsa cuyas 128 ranuras registran su indice (se
// comprueba que el EXE y la DLL llaman a la misma ranura virtual de Load: 25 en peds, 65 en vehiculos). CVehicleModelInfo se falsea
// para 16 ids de modelo reservados (19980..19995, ms_modelInfoPtrs 0xA9B0C8) y se restauran. Se guardan y restauran Players[0].m_pPed
// (0xB7CD98), CCarCtrl::NumRandomCars (0x969094) y el bloque 0xC16EE8..0xC16EFD.
//
// Dominio de los fixtures (leccion 12/21): en LoadPedPool el primer registro es siempre de tipo PED_TYPE_PLAYER1 (el EXE desreferencia un
// puntero nulo si no); las referencias de objetos/peds/vehiculos usan ranuras 0..11; en peds y vehiculos las ranuras creadas son distintas
// y estan libres (la DLL tiene un assert que el EXE no tiene); como mucho 8 registros.
#include <fstream>
#include <set>
#include <cmath>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Pools/Pools.h"
#include "Entity/Object/Object.h"
#include "Entity/Object/CutsceneObject.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Heli.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "GenericGameStorage.h"
#include "Streaming.h"
#include "CarCtrl.h"
#include "Population.h"
#include "ObjectSaveStructure.h"
#include "World.h"
#include "PlayerInfo.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace PoolsWave4Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class V=uint32> V& At(void* base,size_t off) { return *reinterpret_cast<V*>(static_cast<uint8*>(base)+off); }
template<class V=uint32> V& AtA(uintptr_t a) { return *reinterpret_cast<V*>(a); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    int32 pickI(std::initializer_list<int64_t> l){ return int32(l.begin()[next()%l.size()]); } };

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

// --- modulo de la DLL y busqueda de cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline uintptr_t Exported(const char* mangled){ if(!modBase)return 0; const auto p=uintptr_t(GetProcAddress(reinterpret_cast<HMODULE>(modBase),mangled)); return p ? FollowIn(p) : 0; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(Addr(f)); }

inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()>>7); }

// ======================= registro de reservas SIN asignaciones (para Initialise/ShutDown) =======================
// Registros {tipo,a,b}: 1 new escalar(tamano,desplazamiento|0xFFFFFFFF si fallo), 2 new[](tamano,desplazamiento), 3 delete(puntero),
// 4 delete[](puntero), 5 printf(hash del formato), 9 arena agotado. Los punteros dentro del arena se codifican 0xA0000000|desplazamiento.
struct Rec { uint32 k,a,b; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
inline void Push(uint32 k,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={k,a,b}; }
inline json RecsJson(){ json j=json::array(); for(size_t i=0;i<nrec;++i)j.push_back(json::array({recs[i].k,recs[i].a,recs[i].b})); if(nrec==recs.size())j.push_back("overflow"); return j; }

constexpr size_t kArenaSize=0x500000;
alignas(64) inline std::array<uint8,kArenaSize> arena{};
inline size_t arenaTop{};
inline int scalarIdx{}; inline int oomAt{-1};
inline bool InArena(uintptr_t v){ return v>=uintptr_t(arena.data())&&v<uintptr_t(arena.data())+kArenaSize; }
inline uint32 Tag(uintptr_t v){ return InArena(v) ? 0xA0000000u|uint32(v-uintptr_t(arena.data())) : uint32(v); }
inline json Norm(uint32 v){ if(!v)return nullptr; if(InArena(v))return std::string("arena+")+std::to_string(v-uint32(uintptr_t(arena.data()))); return v; }

// trampolin al operator new original (para hilos ajenos): primeros 5 bytes de 0x82119A (push esi / mov esi,[esp+8]) y salto a 0x82119F
using NewFn=void*(__cdecl*)(size_t);
inline NewFn realNew{};
inline bool MakeTrampoline(){
    static const uint8 head[5]{0x56,0x8B,0x74,0x24,0x08};
    if(std::memcmp(reinterpret_cast<void*>(0x82119A),head,5)!=0)return false;
    auto* mem=static_cast<uint8*>(VirtualAlloc(nullptr,64,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE)); if(!mem)return false;
    std::memcpy(mem,head,5); mem[5]=0xE9; const int32 rel=int32(0x82119Fu-uint32(uintptr_t(mem)+10)); std::memcpy(mem+6,&rel,4);
    realNew=reinterpret_cast<NewFn>(mem); FlushInstructionCache(GetCurrentProcess(),mem,16); return true; }
inline void ForeignDelete(void* p){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); }   // 0x8214BD y 0x8213AE son "jmp 0x82413F"

inline void* Bump(uint32 kind,size_t n){
    const size_t top=(arenaTop+15)&~size_t(15);
    if(n>kArenaSize||top>kArenaSize-n){ Push(9,uint32(n)); return nullptr; }
    arenaTop=top+n; Push(kind,uint32(n),uint32(top)); return arena.data()+top; }
inline void* __cdecl S_NewScalar(size_t n){ if(!Mine())return realNew(n); const int idx=scalarIdx++; if(idx==oomAt){ Push(1,uint32(n),0xFFFFFFFFu); return nullptr; } return Bump(1,n); }
inline void* __cdecl S_NewArray(size_t n){ if(!Mine())return realNew(n); return Bump(2,n); }
inline void __cdecl S_Delete(void* p){ if(!Mine()){ ForeignDelete(p); return; } Push(3,Tag(uintptr_t(p))); }
inline void __cdecl S_DeleteArr(void* p){ if(!Mine()){ ForeignDelete(p); return; } Push(4,Tag(uintptr_t(p))); }
inline int __cdecl S_PrintfRec(const char* f,...){ if(!Mine())return 0; uint32 n=0; while(n<64&&f[n])++n; Push(5,uint32(Fnv(f,n)),n); return 0; }

// ======================= pools falsos (disposicion del CPool del EXE) =======================
constexpr size_t kSlots=16;
template<size_t Stride> struct FakePool {
    alignas(16) std::array<uint8,kSlots*Stride> store{};
    alignas(16) std::array<uint8,kSlots+16> flags{};
    alignas(16) std::array<uint8,0x14> hdr{};
    // pctUsed: probabilidad de que una de las ranuras 0..11 este ocupada; 12..15 y la guarda siempre libres
    void Init(Rng& r,uint32 pctUsed){
        FillBytes(store.data(),store.size(),r);
        for(size_t s=0;s<kSlots;++s)flags[s]= (s<12&&r.pct(pctUsed)) ? uint8(r.below(128)) : uint8(0x80|r.below(128));
        for(size_t s=kSlots;s<flags.size();++s)flags[s]=0x80;
        std::memset(hdr.data(),0,hdr.size());
        At<void*>(hdr.data(),0)=store.data(); At<void*>(hdr.data(),4)=flags.data(); At<int32>(hdr.data(),8)=int32(kSlots);
        At<int32>(hdr.data(),0xC)=r.pickI({-1,0,5,11,15}); At<uint8>(hdr.data(),0x10)=0; }
    int Index(const void* p) const { return int((static_cast<const uint8*>(p)-store.data())/ptrdiff_t(Stride)); }
    bool Free(size_t s) const { return flags[s]&0x80; }
    json Out(){ json o=json::object(); o["store"]=Fnv(store.data(),store.size()); o["flags"]=Fnv(flags.data(),flags.size());
        o["hdr"]=json::array({Name(At<uint32>(hdr.data(),0)),Name(At<uint32>(hdr.data(),4)),At<int32>(hdr.data(),8),At<int32>(hdr.data(),0xC),At<uint8>(hdr.data(),0x10)});
        o["flags_head"]=json::array(); for(size_t s=0;s<kSlots;++s)o["flags_head"].push_back(flags[s]); return o; }
};
constexpr size_t kObjStride=0x19C, kPedStride=0x7C4, kVehStride=0xA18;
inline FakePool<kObjStride> objPool;
inline FakePool<kPedStride> pedPool;
inline FakePool<kVehStride> vehPool;

// vtable falsa (128 ranuras que registran su indice)
template<size_t N> inline bool __fastcall VSlot(void* self,void*){ if(Mine())T("vcall",uint32(N),self); return true; }
template<size_t... I> inline auto MakeVt(std::index_sequence<I...>){ return std::array<void*,sizeof...(I)>{ reinterpret_cast<void*>(&VSlot<I>)... }; }
inline const auto fakeVt=MakeVt(std::make_index_sequence<128>{});

// flujo de guardado en memoria
struct Stream { std::vector<uint8> d; size_t pos{};
    void I32(int32 v){ const auto* b=reinterpret_cast<const uint8*>(&v); d.insert(d.end(),b,b+4); }
    void Bytes(Rng& r,size_t n){ for(size_t k=0;k<n;++k)d.push_back(uint8(r.next()>>7)); } };
inline Stream st;
inline bool pedRet{}, objRet{};

// modelos de vehiculo falsos
constexpr int32 kMiBase=19980, kMiCount=16;
alignas(16) inline std::array<std::array<uint8,0x50>,kMiCount> fakeMi{};

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
inline bool __cdecl S_LoadData(void* dst,int32 n){
    if(!Mine())return false;
    if(n<=0){ T("load_data",n,uint32(st.pos),uint8(0)); return true; }
    if(st.pos+size_t(n)>st.d.size()){ std::memset(dst,0,size_t(n)); T("load_data",n,uint32(st.pos),uint8(0)); return false; }
    std::memcpy(dst,st.d.data()+st.pos,size_t(n)); T("load_data",n,uint32(st.pos),Fnv(dst,size_t(n))); st.pos+=size_t(n); return true; }
inline bool __cdecl S_SaveData(void* src,int32 n){ if(!Mine())return true; T("save_data",n,n>0 ? Fnv(src,size_t(n)) : uint64(0)); return true; }
inline bool __cdecl S_SavePed(){ if(!Mine())return false; T("save_ped_pool"); return pedRet; }
inline bool __cdecl S_SaveObj(){ if(!Mine())return false; T("save_object_pool"); return objRet; }
inline bool __cdecl S_LoadPed(){ if(!Mine())return false; T("load_ped_pool"); return pedRet; }
inline bool __cdecl S_LoadObj(){ if(!Mine())return false; T("load_object_pool"); return objRet; }
inline void __cdecl S_WorldAdd(void* e){ if(Mine())T("world_add",e); }
inline void __cdecl S_Convert(void* obj){ if(!Mine())return; T("convert_to_dummy",obj); const int idx=objPool.Index(obj); if(idx>=0&&idx<int(kSlots))objPool.flags[size_t(idx)]|=0x80; }
inline void* __fastcall S_ObjCtor(void* self,void*,int32 model,uint32 create){ if(Mine()){ T("object_ctor",self,model,uint8(create)); At<const void*>(self,0)=fakeVt.data(); } return self; }
inline void __fastcall S_Extract(void* self,void*,void* obj){ if(Mine())T("object_extract",Fnv(self,0x30),obj); }
inline void* __fastcall S_PlayerCtor(void* self,void*,int32 playerId,uint32 group){ if(Mine()){ T("player_ctor",self,playerId,uint8(group)); At<const void*>(self,0)=fakeVt.data(); } return self; }
template<int K> inline void* __fastcall S_VCtor(void* self,void*,int32 model,uint32 by){ if(Mine()){ T("vehicle_ctor",K,self,model,uint8(by)); At<const void*>(self,0)=fakeVt.data(); } return self; }
inline void* __fastcall S_AutoCtor(void* self,void*,int32 model,uint32 by,uint32 susp){ if(Mine()){ T("vehicle_ctor",9,self,model,uint8(by),uint8(susp)); At<const void*>(self,0)=fakeVt.data(); } return self; }
inline void __cdecl S_RequestModel(int32 model,int32 flags){ if(Mine())T("request_model",model,flags); }
inline void __cdecl S_LoadAll(uint32 priorityOnly){ if(Mine())T("load_all_requested_models",uint8(priorityOnly)); }
inline void __cdecl S_UpdateCarCount(void* veh,uint32 dec){ if(Mine())T("update_car_count",veh,uint8(dec)); }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,7> fns{{
    {"CPools","Initialise",0x550F10,0,-1,144},
    {"CPools","ShutDown",0x5519F0,0,-1,256},
    {"CPools","Save",0x5D0880,0,-1,64},
    {"CPools","Load",0x5D0890,0,-1,64},
    {"CPools","LoadObjectPool",0x5D4A40,0,-1,512},
    {"CPools","LoadPedPool",0x5D2D70,0,-1,512},
    {"CPools","LoadVehiclePool",0x5D2A20,0,-1,768},
}};

// ======================= fixtures =======================
constexpr std::array<uintptr_t,17> kGl{0xB74484,0xB74488,0xB7448C,0xB74490,0xB74494,0xB74498,0xB7449C,0xB744A0,0xB744A4,0xB744A8,0xB744AC,0xB744B0,0xB744B4,0xB744B8,0xB744BC,0xB744C0,0xB744C4};
constexpr uintptr_t kPlayerPed=0xB7CD98, kNumRandomCars=0x969094, kModelInfoPtrs=0xA9B0C8, kStorageGlobals=0xC16EE8;
constexpr size_t kStorageSize=0x16;                  // 0xC16EE8..0xC16EFD (ms_WorkBuffer .. ms_bLoading)

inline json ArenaHeaders(){ json a=json::array();
    for(auto g:kGl){ const uint32 v=AtA<uint32>(g); json e=json::object(); e["global"]=Norm(v);
        if(v&&InArena(v)){ const auto* h=reinterpret_cast<const uint8*>(uintptr_t(v)); uint32 w[5]; std::memcpy(w,h,20);
            e["hdr"]=json::array({Norm(w[0]),Norm(w[1]),int32(w[2]),int32(w[3]),w[4]}); e["junk"]=json::array({h[0x11],h[0x12],h[0x13]}); }
        a.push_back(std::move(e)); }
    return a; }

// ---- CPools::Initialise ----
inline json CaseInit(int i,Rng& r){
    static const uint8 pats[4]{0xCD,0x00,0xFD,0x5A};
    std::memset(arena.data(),pats[r.below(4)],kArenaSize);
    arenaTop=size_t(r.below(4))*16; nrec=0; scalarIdx=0; oomAt= (i%18)<17 ? (i%18) : -1;
    for(auto g:kGl)AtA<uint32>(g)=r.next();
    {   // ventana de reservas dobladas: sin ninguna asignacion del banco entre medias
        ReversingProbe::EntryPatch ns(0x82119A,S_NewScalar), na(0x821195,S_NewArray), dd(0x8214BD,S_Delete), da(0x8213AE,S_DeleteArr);
        reinterpret_cast<void(__cdecl*)()>(0x550F10)();
    }
    json o=json::object(); o["alloc"]=RecsJson(); o["pools"]=ArenaHeaders(); o["oom_at"]=oomAt;
    o["arena"]=Fnv(arena.data(),std::min(kArenaSize,arenaTop+256)); o["top"]=arenaTop; return o; }

// ---- CPools::ShutDown ----
inline json CaseShut(int i,Rng& r){
    (void)i;
    std::memset(arena.data(),0xCD,0x1000); arenaTop=0x1000; nrec=0;
    json setup=json::array();
    for(size_t k=0;k<kGl.size();++k){
        auto* h=arena.data()+k*0x20;
        const bool present=!r.pct(12);
        const uint32 storage= r.pct(10) ? 0u : 0x30000000u+(r.below(0xFFFF)<<4);
        const uint32 states = r.pct(10) ? 0u : 0x38000000u+(r.below(0xFFFF)<<4);
        const int32 cap=r.pickI({-1,0,1,2,5,100,70000,70000,140,140,140,int64_t(INT32_MIN)});
        const int32 last=r.pickI({-1,0,3,99});
        const uint8 own=uint8(r.pickI({1,1,1,1,0,2,0xFF}));
        At<uint32>(h,0)=storage; At<uint32>(h,4)=states; At<int32>(h,8)=cap; At<int32>(h,0xC)=last; At<uint8>(h,0x10)=own;
        At<uint8>(h,0x11)=uint8(r.next()); At<uint8>(h,0x12)=uint8(r.next()); At<uint8>(h,0x13)=uint8(r.next());
        AtA<uint32>(kGl[k])= present ? uint32(uintptr_t(h)) : 0u;
        setup.push_back(json::array({present,storage,states,cap,last,own})); }
    const auto before=ReversingProbe::Bytes(reinterpret_cast<void*>(kGl[0]),kGl.size()*4);
    {   ReversingProbe::EntryPatch dd(0x8214BD,S_Delete), da(0x8213AE,S_DeleteArr), pf(0x821982,S_PrintfRec);
        reinterpret_cast<void(__cdecl*)()>(0x5519F0)();
    }
    json o=json::object(); o["setup"]=setup; o["alloc"]=RecsJson(); o["pools"]=ArenaHeaders();
    o["globals_unchanged"]=(ReversingProbe::Bytes(reinterpret_cast<void*>(kGl[0]),kGl.size()*4)==before);
    o["arena"]=Fnv(arena.data(),0x1000); return o; }

// ---- CPools::Save / Load ----
inline json CaseSaveLoad(Rng& r,bool load){
    pedPool.Init(r,0); objPool.Init(r,0);
    AtA<void*>(0xB74490)=pedPool.hdr.data(); AtA<void*>(0xB7449C)=objPool.hdr.data();
    st=Stream{}; for(int k=0;k<64;++k)st.d.push_back(uint8(r.next()>>9));
    FillBytes(reinterpret_cast<void*>(kStorageGlobals),kStorageSize,r);
    pedRet=r.pct(60); objRet=r.pct(60);
    const auto before=ReversingProbe::Bytes(reinterpret_cast<void*>(kStorageGlobals),kStorageSize);
    const uint32 al=reinterpret_cast<uint32(__cdecl*)()>(load ? 0x5D0890 : 0x5D0880)()&0xFF;
    json o=json::object(); o["al"]=al; o["ped_ret"]=pedRet; o["obj_ret"]=objRet; o["pos"]=st.pos;
    o["storage_unchanged"]=(ReversingProbe::Bytes(reinterpret_cast<void*>(kStorageGlobals),kStorageSize)==before);
    o["ped_pool"]=pedPool.Out(); o["obj_pool"]=objPool.Out(); return o; }

// ---- CPools::LoadObjectPool ----
inline json CaseLoadObj(Rng& r){
    objPool.Init(r,40); AtA<void*>(0xB7449C)=objPool.hdr.data();
    st=Stream{};
    const int32 n=r.pickI({0,0,1,2,3,4,6,8,-1,-5});
    st.I32(n);
    for(int32 k=0;k<n;++k){
        const int32 slot=int32(r.below(12)), id=int32(r.below(128));
        st.I32((slot<<8)|id);
        st.I32(r.pickI({0,1,2000,19999,-1,int64_t(r.below(30000))}));
        st.I32(r.pickI({0x30,0x30,0x30,0,-1,48,int64_t(int32(r.next()))}));
        st.Bytes(r,0x30); }
    if(r.pct(20))st.Bytes(r,r.below(9));
    const uint32 al=reinterpret_cast<uint32(__cdecl*)()>(0x5D4A40)()&0xFF;
    json o=objPool.Out(); o["al"]=al; o["pos"]=st.pos; o["size"]=st.d.size(); o["records"]=n; return o; }

// ---- CPools::LoadPedPool ----
inline json CaseLoadPed(Rng& r){
    pedPool.Init(r,40); AtA<void*>(0xB74490)=pedPool.hdr.data();
    st=Stream{};
    const int32 n=r.pickI({0,0,1,1,2,3,4,6,-2});
    st.I32(n);
    std::set<int32> taken;
    if(n>0){ bool any=false; for(size_t s=0;s<12;++s)any|=pedPool.Free(s); if(!any)pedPool.flags[0]|=0x80; }
    for(int32 k=0;k<n;++k){
        int32 type= k==0 ? 0 : r.pickI({0,1,2,4,7,13,0});
        int32 slot=int32(r.below(12));
        if(type==0){ int tries=0; while((taken.count(slot)||!pedPool.Free(size_t(slot)))&&tries++<64)slot=(slot+1)%12;
            if(taken.count(slot)||!pedPool.Free(size_t(slot))){ if(k==0){ slot=0; pedPool.flags[0]|=0x80; taken.clear(); } else type=1; } }
        if(type==0)taken.insert(slot);
        st.I32((slot<<8)|int32(r.below(128)));
        st.I32(r.pickI({0,7,100,299,-1,int64_t(r.below(400))}));
        st.I32(type); }
    if(r.pct(20))st.Bytes(r,r.below(9));
    const uint32 al=reinterpret_cast<uint32(__cdecl*)()>(0x5D2D70)()&0xFF;
    json o=pedPool.Out(); o["al"]=al; o["pos"]=st.pos; o["size"]=st.d.size(); o["records"]=n;
    o["player_ptr"]=Name(AtA<uint32>(kPlayerPed)); return o; }

// ---- CPools::LoadVehiclePool ----
constexpr std::array<int32,17> kVehTypes{0,1,2,3,4,5,6,7,8,9,10,11,-1,12,255,0x7FFFFFFF,INT32_MIN};
inline json CaseLoadVeh(Rng& r){
    vehPool.Init(r,40); AtA<void*>(0xB74494)=vehPool.hdr.data();
    for(int32 j=0;j<kMiCount;++j){ auto& m=fakeMi[size_t(j)]; FillBytes(m.data(),m.size(),r);
        At<int32>(m.data(),0x3C)=kVehTypes[r.below(uint32(kVehTypes.size()))];
        AtA<void*>(kModelInfoPtrs+4u*uint32(kMiBase+j))=m.data(); }
    AtA<int32>(kNumRandomCars)=r.pickI({0,5,100,-3,1000});
    st=Stream{};
    int32 n=r.pickI({0,0,1,2,3,4,6,8,-1});
    std::set<int32> taken;
    if(n>0){ bool any=false; for(size_t s=0;s<12;++s)any|=vehPool.Free(s); if(!any)vehPool.flags[0]|=0x80; }
    std::vector<std::array<int32,3>> recsOut;
    for(int32 k=0;k<n;++k){
        int32 slot=int32(r.below(12)); int tries=0;
        while((taken.count(slot)||!vehPool.Free(size_t(slot)))&&tries++<64)slot=(slot+1)%12;
        if(taken.count(slot)||!vehPool.Free(size_t(slot)))break;       // sin ranura libre: se recorta el fixture
        taken.insert(slot);
        recsOut.push_back({(slot<<8)|int32(r.below(128)),kMiBase+int32(r.below(uint32(kMiCount))),0}); }
    st.I32(n>0 ? int32(recsOut.size()) : n);
    for(auto& rec:recsOut){ st.I32(rec[0]); st.I32(rec[1]); }
    if(r.pct(20))st.Bytes(r,r.below(9));
    const uint32 al=reinterpret_cast<uint32(__cdecl*)()>(0x5D2A20)()&0xFF;
    json o=vehPool.Out(); o["al"]=al; o["pos"]=st.pos; o["size"]=st.d.size(); o["records"]=int(recsOut.size());
    o["num_random_cars"]=AtA<int32>(kNumRandomCars); return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"poolswave4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"poolswave4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("poolswave4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"poolswave4-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,7> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ---
    const uintptr_t dLoadData=Body(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer));
    const uintptr_t dSaveData=Body(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer));
    const uintptr_t dWorldAdd=Body(&CWorld::Add), dConvert=Body(&CPopulation::ConvertToDummyObject), dExtract=Body(&CObjectSaveStructure::Extract);
    const uintptr_t dReq=Body(&CStreaming::RequestModel), dLoadAll=Body(&CStreaming::LoadAllRequestedModels), dUpd=Body(&CCarCtrl::UpdateCarCount);
    const uintptr_t dSavePed=Body(&CPools::SavePedPool), dSaveObj=Body(&CPools::SaveObjectPool), dLoadPed=Body(&CPools::LoadPedPool), dLoadObj=Body(&CPools::LoadObjectPool);
    const uintptr_t dObjCtor=Exported("??0CObject@@QAE@H_N@Z"), dPlayerCtor=Exported("??0CPlayerPed@@QAE@H_N@Z");
    struct VC { uintptr_t exe; const char* sym; uintptr_t dll; };
    std::array<VC,10> vc{{
        {0x6C8D60,"??0CMonsterTruck@@QAE@HW4eVehicleCreatedBy@@@Z",0}, {0x6CE370,"??0CQuadBike@@QAE@HW4eVehicleCreatedBy@@@Z",0},
        {0x6C4190,"??0CHeli@@QAE@HW4eVehicleCreatedBy@@@Z",0},        {0x6C8E20,"??0CPlane@@QAE@HW4eVehicleCreatedBy@@@Z",0},
        {0x6F2940,"??0CBoat@@QAE@HW4eVehicleCreatedBy@@@Z",0},       {0x6F6030,"??0CTrain@@QAE@HW4eVehicleCreatedBy@@@Z",0},
        {0x6BF430,"??0CBike@@QAE@HW4eVehicleCreatedBy@@@Z",0},       {0x6BF820,"??0CBmx@@QAE@HW4eVehicleCreatedBy@@@Z",0},
        {0x6D03A0,"??0CTrailer@@QAE@HW4eVehicleCreatedBy@@@Z",0},    {0x6B0A90,"??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z",0}}};
    for(auto& v:vc)v.dll=Exported(v.sym);
    {   std::set<uintptr_t> all{dLoadData,dSaveData,dWorldAdd,dConvert,dExtract,dReq,dLoadAll,dUpd,dSavePed,dSaveObj,dLoadPed,dLoadObj,dObjCtor,dPlayerCtor};
        bool ok=!all.count(0)&&all.size()==14;
        for(auto& v:vc){ ok&=v.dll!=0; ok&=all.insert(v.dll).second; }
        if(!ok){ json miss=json::array(); for(auto& v:vc)if(!v.dll)miss.push_back(v.sym);
            Write({{"event","error"},{"reason","DLL bodies not found or overlapping"},{"missing_ctors",miss},{"load_data",dLoadData},{"save_data",dSaveData},{"world_add",dWorldAdd},{"convert",dConvert},
                   {"extract",dExtract},{"request",dReq},{"load_all",dLoadAll},{"update_car_count",dUpd},{"save_ped",dSavePed},{"save_obj",dSaveObj},{"load_ped",dLoadPed},
                   {"load_obj",dLoadObj},{"obj_ctor",dObjCtor},{"player_ctor",dPlayerCtor}}); return; } }
    if(!MakeTrampoline()){ Write({{"event","error"},{"reason","operator new trampoline: unexpected bytes at 0x82119A"}}); return; }

    std::array<bool,7> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // nombres de buferes para identificar punteros en las trazas
    if(named.empty()){
        Nm(objPool.store.data(),objPool.store.size(),"objstore"); Nm(objPool.flags.data(),objPool.flags.size(),"objflags"); Nm(objPool.hdr.data(),objPool.hdr.size(),"objhdr");
        Nm(pedPool.store.data(),pedPool.store.size(),"pedstore"); Nm(pedPool.flags.data(),pedPool.flags.size(),"pedflags"); Nm(pedPool.hdr.data(),pedPool.hdr.size(),"pedhdr");
        Nm(vehPool.store.data(),vehPool.store.size(),"vehstore"); Nm(vehPool.flags.data(),vehPool.flags.size(),"vehflags"); Nm(vehPool.hdr.data(),vehPool.hdr.size(),"vehhdr"); }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      Chk("sizeof(CCopPed) == paso del pool de peds (0x5E4742 imul 7C4h)",uint32(sizeof(CCopPed)),0x7C4);
      Chk("sizeof(CHeli) == paso del pool de vehiculos (0x6E2D72 imul 0A18h)",uint32(sizeof(CHeli)),0xA18);
      Chk("sizeof(CCutsceneObject) == paso del pool de objetos (0x5A1F02 imul 19Ch)",uint32(sizeof(CCutsceneObject)),0x19C);
      Chk("sizeof(CObjectSaveStructure) (0x5D4B07 push 30h)",uint32(sizeof(CObjectSaveStructure)),0x30);
      { alignas(16) uint8 mi[0x60]{}; auto* m=reinterpret_cast<CVehicleModelInfo*>(mi); Chk("CVehicleModelInfo.m_nVehicleType (0x5D2A96 mov eax,[eax+3Ch])",uint32(reinterpret_cast<uint8*>(&m->m_nVehicleType)-mi),0x3C); }
      { alignas(16) static uint8 bike[sizeof(CBike)+16]{}; std::memset(bike,0,sizeof(bike)); auto* b=reinterpret_cast<CBike*>(bike); b->bikeFlags.bOnSideStand=true;
        uint32 off=0xFFFF; for(size_t k=0;k<sizeof(CBike);++k)if(bike[k]){ off=uint32(k); break; }
        Chk("CBike.bikeFlags offset (0x5D2B4F or byte ptr [eax+614h],10h)",off,0x614); Chk("CBike.bikeFlags.bOnSideStand mask",off<sizeof(bike)?bike[off]:0u,0x10); }
      { alignas(16) static uint8 ped[sizeof(CPed)+16]{}; std::memset(ped,0,sizeof(ped)); auto* p=reinterpret_cast<CPed*>(ped); p->SetWeaponAccuracy(100);
        uint32 off=0xFFFF; for(size_t k=0;k<sizeof(CPed);++k)if(ped[k]){ off=uint32(k); break; }
        Chk("CPed.m_nWeaponAccuracy offset (0x5D2E10 mov byte ptr [eax+71Ah],64h)",off,0x71A); Chk("CPed.m_nWeaponAccuracy valor",off<sizeof(ped)?ped[off]:0u,0x64); }
      Chk("&CWorld::Players (0x5D2E21 mov [00B7CD98h],eax)",uint32(reinterpret_cast<uintptr_t>(&CWorld::Players[0])),0xB7CD98);
      { uintptr_t cnt=0; for(int j=0;j<kMiCount;++j)cnt+=AtA<uint32>(kModelInfoPtrs+4u*uint32(kMiBase+j)); Chk("ids de modelo 19980..19995 libres (suma de punteros)",uint32(cnt),0); }
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(kGl[0],kGl.size()*4);                              // los 17 punteros de pool
        Snap(kPlayerPed,4); Snap(kNumRandomCars,4); Snap(kStorageGlobals,kStorageSize);
        Snap(kModelInfoPtrs+4u*uint32(kMiBase),4u*uint32(kMiCount)); }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2B7Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                hooks[f]->State(candidate);
                const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                Patches P;
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                if(nm=="Initialise"){ out["state"]=CaseInit(i,r); }
                else if(nm=="ShutDown"){ out["state"]=CaseShut(i,r); }
                else if(nm=="Save"||nm=="Load"){
                    P.Add(0x5D4B40,S_SavePed); P.Add(dSavePed,S_SavePed); P.Add(0x5D4940,S_SaveObj); P.Add(dSaveObj,S_SaveObj);
                    P.Add(0x5D2D70,S_LoadPed); P.Add(dLoadPed,S_LoadPed); P.Add(0x5D4A40,S_LoadObj); P.Add(dLoadObj,S_LoadObj);
                    P.Add(0x5D1270,S_SaveData); P.Add(dSaveData,S_SaveData); P.Add(0x5D1300,S_LoadData); P.Add(dLoadData,S_LoadData);
                    out["state"]=CaseSaveLoad(r,nm=="Load"); }
                else {
                    P.Add(0x5D1300,S_LoadData); P.Add(dLoadData,S_LoadData); P.Add(0x563220,S_WorldAdd); P.Add(dWorldAdd,S_WorldAdd);
                    if(nm=="LoadObjectPool"){
                        P.Add(0x614670,S_Convert); P.Add(dConvert,S_Convert); P.Add(0x5A1D70,S_ObjCtor); P.Add(dObjCtor,S_ObjCtor);
                        P.Add(0x5D2220,S_Extract); P.Add(dExtract,S_Extract);
                        out["state"]=CaseLoadObj(r); }
                    else if(nm=="LoadPedPool"){
                        P.Add(0x60D5B0,S_PlayerCtor); P.Add(dPlayerCtor,S_PlayerCtor);
                        out["state"]=CaseLoadPed(r); }
                    else {
                        P.Add(0x4087E0,S_RequestModel); P.Add(dReq,S_RequestModel); P.Add(0x40EA10,S_LoadAll); P.Add(dLoadAll,S_LoadAll);
                        P.Add(0x424000,S_UpdateCarCount); P.Add(dUpd,S_UpdateCarCount);
                        P.Add(vc[0].exe,&S_VCtor<0>); P.Add(vc[0].dll,&S_VCtor<0>); P.Add(vc[1].exe,&S_VCtor<1>); P.Add(vc[1].dll,&S_VCtor<1>);
                        P.Add(vc[2].exe,&S_VCtor<2>); P.Add(vc[2].dll,&S_VCtor<2>); P.Add(vc[3].exe,&S_VCtor<3>); P.Add(vc[3].dll,&S_VCtor<3>);
                        P.Add(vc[4].exe,&S_VCtor<4>); P.Add(vc[4].dll,&S_VCtor<4>); P.Add(vc[5].exe,&S_VCtor<5>); P.Add(vc[5].dll,&S_VCtor<5>);
                        P.Add(vc[6].exe,&S_VCtor<6>); P.Add(vc[6].dll,&S_VCtor<6>); P.Add(vc[7].exe,&S_VCtor<7>); P.Add(vc[7].dll,&S_VCtor<7>);
                        P.Add(vc[8].exe,&S_VCtor<8>); P.Add(vc[8].dll,&S_VCtor<8>); P.Add(vc[9].exe,&S_AutoCtor); P.Add(vc[9].dll,&S_AutoCtor);
                        out["state"]=CaseLoadVeh(r); } }
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
