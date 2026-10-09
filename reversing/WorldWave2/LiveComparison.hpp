#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/WorldFinal1): funciones pendientes de CRoadBlocks (5), CPools (2) y CIdleCam (8).
// Original (hook de la funcion desactivado, se ejecuta el codigo del EXE) frente a candidato (hook activo) sobre objetos falsos con
// guardas, fixtures deterministas por caso y la traza de todas las llamadas salientes (con sus argumentos).
//
// CRoadBlocks (EXE 0x460DF0/0x460EC0/0x460EE0/0x461020/0x4629E0): bloque global 0xA43438..0xA43C78 (InOrOut, NumRoadBlocks, GenerateDynamicRoadBlocks,
// RoadBlockNodes, aScriptRoadBlocks) guardado y restaurado. Las areas de rutas 61 (sin cargar), 62 y 63 de ThePaths se sustituyen temporalmente por
// nodos, enlaces de navegacion y CCarPathLink falsos (punteros y m_anNumAddresses guardados y restaurados). Colaboradores simulados y trazados:
// CWorld::FindObjectsIntersectingCube (0x568DD0), CWorld::Remove (0x563280), CVehicle/CPed/CObject::CanBeDeleted (0x6D1180/0x5DF100/0x59F120), rand (0x821B1E)
// y CGeneral::GetRandomNumber, FindPlayerWanted/FindPlayerVehicle/FindPlayerCoors (0x56E230/0x56E0D0/0x56E010), CreateRoadBlockBetween2Points (0x4619C0),
// CWorld::ProcessLineOfSight (0x56BA00). Las entidades son buferes de 0xA40 bytes con vtable falsa (hueco 0 = destructor con borrado).
//
// CPools (0x550080/0x551950): el puntero global del pool de objetos (0xB7449C) se sustituye temporalmente por un pool falso de 4 ranuras de 0x19C bytes
// (el diseno de CPool coincide con el del EXE: +0 almacenamiento, +4 banderas, +8 tamano, +0xC ultima libre, +0x10 propietario). Simulados: CWorld::Add/Remove,
// CProjectileInfo::RemoveIfThisIsAProjectile (0x739A40), operator new de CObject (0x5A1EE0) y su constructor (0x5A1D70 y el de la DLL), printf (0x821982).
//
// CIdleCam (0x50E6D0/0x50A160/0x50A200/0x50EAE0/0x50E690/0x50A280/0x50EB50/0x517770): objeto falso de 0x9C bytes con camara falsa (m_Cam) y entidades falsas.
// Globales tocados y restaurados: CPad 0 (LastTimeTouched), CTimer (0xB7CB5C/0xB7CB84), contador de ticker de gIdleCam (0xB6FE34), byte de InterestingEvents
// (0xC0B184), camara activa de TheCamera (0xB6F081 y modos 0xB6F1A8), CWorld::pIgnoreEntity (0xB7CD68). Simulados: CEntity::RegisterReference (0x571B70),
// CEntity::CleanUpOldReference (0x571A00), FindPlayerPed (0x56E210), CWorld::GetIsLineOfSightClear (0x56A490).
//
// No se asigna memoria ni se toca ningun pool real; todo el estado global tocado se guarda y se restaura (state_restored).
#include <fstream>
#include <set>
#include <cmath>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "RoadBlocks.h"
#include "IdleCam.h"
#include "Pathfind.h"
#include "ProjectileInfo.h"
#include "Pools/Pools.h"
#include "Entity/Object/Object.h"
#include "../ProbeRuntime.hpp"

namespace WorldWave2Live {
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
inline void SnapP(const void* p,size_t n){ Snap(reinterpret_cast<uintptr_t>(p),n); }
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

// ======================= objetos falsos =======================
constexpr size_t kEntSize=0xA40, kObjStride=0x19C, kPoolSlots=4;
struct alignas(16) Ent { uint8 b[kEntSize]; };
inline std::array<Ent,6> ents{};                               // 0..5 entidades falsas (0 = jugador falso en CIdleCam)
inline std::array<void*,80> entVt{};                           // vtable falsa de las entidades
alignas(16) inline std::array<std::array<uint8,0x60>,3> fmats{};  // matrices falsas (posicion en +0x30)
inline void* FMat(int i){ return fmats[size_t(i)].data(); }
// pool de objetos falso
alignas(16) inline std::array<uint8,kPoolSlots*kObjStride> poolStore{};
inline std::array<uint8,kPoolSlots> poolFlags{};
alignas(16) inline std::array<uint8,0x14> fakePool{};
alignas(16) inline std::array<uint8,kObjStride> newObjBuf{};
// rutas falsas: areas 62 y 63 (indices 0 y 1)
constexpr size_t kNodes=8, kLinks=16, kNaviEntries=32;
alignas(16) inline std::array<uint8,2*kNodes*0x1C> pathNodes{};
alignas(16) inline std::array<uint16,2*kNaviEntries> naviLinks{};
alignas(16) inline std::array<uint8,2*kLinks*0xE> carLinks{};
alignas(16) inline std::array<uint8,0x100> fakeWanted{};
alignas(16) inline std::array<uint8,0x100> fakeVeh{};
// CIdleCam falso
constexpr size_t kIdleSize=0x9C, kGuard=32;
alignas(16) inline std::array<uint8,kIdleSize+2*kGuard> idleRaw{};
alignas(16) inline std::array<uint8,0x300> camRaw{};
inline uint8* IdleP(){ return idleRaw.data()+kGuard; }

// estado del caso actual compartido con los dobles
inline std::array<bool,8> script{};      // resultados programados
inline uint32 scN{};                     // contador de uso de `script`
inline Rng rr{1};                        // generador de los dobles de rand
inline CVector playerPos{};
inline bool hasVeh{};
inline int32 findCount{}; inline std::array<uint32,2> findOrder{};
inline std::array<bool,6> canDel{};
inline int EntIdx(const void* p){ for(size_t i=0;i<ents.size();++i)if(p==&ents[i])return int(i); return -1; }

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
inline void __fastcall V_Dtor(void* self,void*,uint32 flags){ if(Mine())T("dtor",self,flags); }
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
inline void __fastcall S_Reg(void* self,void*,void** ref){ if(Mine())T("register_reference",self,ref); }
inline void __fastcall S_Clean(void* self,void*,void** ref){ if(Mine())T("clean_up_old_reference",self,ref); }
inline void __cdecl S_WorldAdd(void* e){ if(Mine())T("world_add",e); }
inline void __cdecl S_WorldRemove(void* e){ if(Mine())T("world_remove",e); }
inline void* __cdecl S_New(size_t n){ if(Mine())T("object_new",uint32(n)); return newObjBuf.data(); }
inline void* __fastcall S_ObjCtor(void* self,void*,int32 model,uint32 create){
    T("object_ctor",self,model,uint8(create)); At<void*>(self,0)=entVt.data(); return self; }
inline bool __cdecl S_Projectile(void* obj){ if(!Mine())return false; const bool res=script[scN++&7]; T("remove_if_projectile",obj,res); return res; }
inline int __cdecl S_Printf(const char*,...){ return 0; }
// CRoadBlocks
inline void __cdecl S_FindCube(const CVector* a,const CVector* b,int16* cnt,uint32 mx,void** out,uint32 f0,uint32 f1,uint32 f2,uint32 f3,uint32 f4){
    if(!Mine()){ *cnt=0; return; }
    T("find_objects_cube",a->x,a->y,a->z,b->x,b->y,b->z,mx&0xFFFF,f0&0xFF,f1&0xFF,f2&0xFF,f3&0xFF,f4&0xFF);
    *cnt=int16(findCount);
    for(int32 i=0;i<findCount&&i<int32(mx)&&i<2;++i)out[i]=&ents[findOrder[size_t(i)]];
}
inline bool __fastcall S_CbdVeh(void* self,void*){ if(!Mine())return false; const int i=EntIdx(self); const bool r=i>=0&&canDel[size_t(i)]; T("vehicle_can_be_deleted",self,r); return r; }
inline bool __fastcall S_CbdPed(void* self,void*){ if(!Mine())return false; const int i=EntIdx(self); const bool r=i>=0&&canDel[size_t(i)]; T("ped_can_be_deleted",self,r); return r; }
inline bool __fastcall S_CbdObj(void* self,void*){ if(!Mine())return false; const int i=EntIdx(self); const bool r=i>=0&&canDel[size_t(i)]; T("object_can_be_deleted",self,r); return r; }
inline int __cdecl S_RandExe(){ if(!Mine())return 0; const int v=int(rr.next()&0x7FFF); T("rand",v); return v; }
inline uint16 __cdecl S_RandDll(){ if(!Mine())return 0; const uint16 v=uint16(rr.next()&0x7FFF); T("rand",int(v)); return v; }
inline void* __cdecl S_Wanted(int32){ return fakeWanted.data(); }
inline void* __cdecl S_Veh(int32,uint32){ return hasVeh ? fakeVeh.data() : nullptr; }
inline CVector* __cdecl S_Coors(CVector* out,int32){ *out=playerPos; return out; }
inline void* __cdecl S_Player(int32){ return &ents[0]; }
inline void __cdecl S_Create(float ax,float ay,float az,float bx,float by,float bz,uint32 gang){ if(Mine())T("create_road_block",ax,ay,az,bx,by,bz,gang&0xFF); }
inline bool __cdecl S_LosPoint(const CVector* a,const CVector* b,void* cp,void** ent,uint32 f0,uint32 f1,uint32 f2,uint32 f3,uint32 f4,uint32 f5,uint32 f6,uint32 f7){
    (void)cp; (void)ent;
    if(!Mine())return false;
    const bool res=script[scN++&7];
    T("process_line_of_sight",a->x,a->y,a->z,b->x,b->y,b->z,f0&0xFF,f1&0xFF,f2&0xFF,f3&0xFF,f4&0xFF,f5&0xFF,f6&0xFF,f7&0xFF,res);
    return res; }
inline bool __cdecl S_LosClear(const CVector* a,const CVector* b,uint32 f0,uint32 f1,uint32 f2,uint32 f3,uint32 f4,uint32 f5,uint32 f6){
    if(!Mine())return true;
    const bool res=script[scN++&7];
    T("is_line_of_sight_clear",a->x,a->y,a->z,b->x,b->y,b->z,f0&0xFF,f1&0xFF,f2&0xFF,f3&0xFF,f4&0xFF,f5&0xFF,f6&0xFF,Name(AtA<uint32>(0xB7CD68)),res);
    return res; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,15> fns{{
    {"CRoadBlocks","RegisterScriptRoadBlock",0x460DF0,0,-1,512},
    {"CRoadBlocks","ClearScriptRoadBlocks",0x460EC0,0,-1,128},
    {"CRoadBlocks","GetRoadBlockNodeInfo",0x460EE0,0,-1,768},
    {"CRoadBlocks","ClearSpaceForRoadBlockObject",0x461020,0,-1,1024},
    {"CRoadBlocks","GenerateRoadBlocks",0x4629E0,0,-1,1024},
    {"CPools","MakeSureSlotInObjectPoolIsEmpty",0x550080,0,-1,512},
    {"CPools","CheckPoolsEmpty",0x551950,0,-1,128},
    {"CIdleCam","Init",0x50E6D0,0,-1,64},
    {"CIdleCam","Reset",0x50A160,0,-1,128},
    {"CIdleCam","ProcessIdleCamTicker",0x50A200,0,-1,256},
    {"CIdleCam","GetLookAtPositionOnTarget",0x50EAE0,0,-1,512},
    {"CIdleCam","IdleCamGeneralProcess",0x50E690,0,-1,256},
    {"CIdleCam","SetTarget",0x50A280,0,-1,384},
    {"CIdleCam","SetTargetPlayer",0x50EB50,0,-1,128},
    {"CIdleCam","IsTargetValid",0x517770,0,-1,512},
}};

// ======================= fixtures =======================
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline float RandF(Rng& r){ const uint32 k=r.below(100); if(k<6)return 0.f; if(k<10)return r.pick({1.f,-1.f,0.5f,90.f,89.99f,30.f,-30.f,1e-30f}); return r.uni(-300.f,300.f); }
// dword seguro: nunca un patron entero aleatorio de 32 bits (podria ser sNaN y el x87 lo silenciaria al copiarlo)
inline void FillSafe(void* p,size_t from,size_t to,Rng& r){ for(size_t o=from;o+4<=to;o+=4){ if(r.pct(50))At<float>(p,o)=RandF(r); else At<uint32>(p,o)=r.below(200); } }
inline void ResetEntity(int i,Rng& r,uint8 type){
    FillBytes(&ents[size_t(i)],sizeof(Ent),r);
    auto* e=ents[size_t(i)].b;
    At<void*>(e,0)=entVt.data(); At<void*>(e,0x14)=r.pct(50) ? FMat(i%3) : nullptr;
    At<uint8>(e,0x36)=uint8((At<uint8>(e,0x36)&~7u)|(type&7u));
    FillSafe(e,4,0x14,r); FillSafe(e,0x18,0x20,r); }
inline void FillMats(Rng& r){ for(size_t i=0;i<fmats.size();++i){ FillBytes(fmats[i].data(),0x60,r); FillSafe(fmats[i].data(),0,0x60,r); } }

inline uint64 H(const void* p,size_t n){ return Fnv(p,n); }
// bloque global de CRoadBlocks
constexpr uintptr_t kRbBase=0xA43438; constexpr size_t kRbSize=0xA43C78-0xA43438;
inline json RbCommon(){ json o=json::object(); o["rb"]=H(reinterpret_cast<void*>(kRbBase),kRbSize);
    o["paths"]=H(pathNodes.data(),pathNodes.size()); o["links"]=H(carLinks.data(),carLinks.size()); o["navi"]=H(naviLinks.data(),naviLinks.size() * 2);
    for(int i=0;i<3;++i)o["ent"+std::to_string(i)]=H(&ents[size_t(i)],sizeof(Ent));
    return o; }

// ---- rutas falsas ----
inline void SetupPaths(Rng& r,bool positions){
    FillBytes(pathNodes.data(),pathNodes.size(),r); FillBytes(carLinks.data(),carLinks.size(),r);
    for(size_t a=0;a<2;++a)for(size_t n=0;n<kNodes;++n){
        auto* nd=pathNodes.data()+(a*kNodes+n)*0x1C;
        At<int16>(nd,0x10)=int16(r.below(uint32(kNaviEntries-1)));                                     // m_wBaseLinkId (hay enlace +1)
        At<uint8>(nd,0x18)=uint8((r.next()&0xF0)|(2+r.below(10)));                                   // m_nNumLinks >= 2
        At<uint8>(nd,0x16)=r.pct(45) ? uint8(0) : uint8(1+r.below(60));                              // m_nPathWidth
        if(positions){ At<int16>(nd,8)=int16(r.pickI({0,100,-100,500,-500,int32(r.below(1600))-800})); At<int16>(nd,10)=int16(r.pickI({0,100,-100,500,-500,int32(r.below(1600))-800})); At<int16>(nd,12)=int16(r.below(400)); }
    }
    for(size_t a=0;a<2;++a)for(size_t k=0;k<kNaviEntries;++k){
        const uint32 area=r.pct(90) ? 62+r.below(2) : 61u;                                           // 61 = area sin cargar
        naviLinks[a*kNaviEntries+k]=uint16((area<<10)|r.below(uint32(kLinks)));
    }
    for(size_t a=0;a<2;++a)for(size_t k=0;k<kLinks;++k){
        auto* l=carLinks.data()+(a*kLinks+k)*0xE;
        At<int16>(l,0)=int16(r.pickI({0,1,-1,40,-40,r.below(8000)-4000})); At<int16>(l,2)=int16(r.pickI({0,1,-1,40,-40,r.below(8000)-4000}));
    }
    ThePaths.m_pPathNodes[61]=nullptr;
    ThePaths.m_pPathNodes[62]=reinterpret_cast<CPathNode*>(pathNodes.data()); ThePaths.m_pPathNodes[63]=reinterpret_cast<CPathNode*>(pathNodes.data()+kNodes*0x1C);
    ThePaths.m_pNaviNodes[62]=reinterpret_cast<CCarPathLink*>(carLinks.data()); ThePaths.m_pNaviNodes[63]=reinterpret_cast<CCarPathLink*>(carLinks.data()+kLinks*0xE);
    ThePaths.m_pNaviLinks[62]=reinterpret_cast<CCarPathLinkAddress*>(naviLinks.data()); ThePaths.m_pNaviLinks[63]=reinterpret_cast<CCarPathLinkAddress*>(naviLinks.data()+kNaviEntries);
    ThePaths.m_anNumAddresses[62]=ThePaths.m_anNumAddresses[63]=uint32(kNaviEntries);
}

// ---- CRoadBlocks::RegisterScriptRoadBlock / ClearScriptRoadBlocks ----
inline void FillScripts(Rng& r){
    const uint32 mode=r.below(5);                                       // 0 todas activas, 1 ninguna, 2 aleatorias
    for(size_t i=0;i<16;++i){ auto* s=reinterpret_cast<uint8*>(0xA43AB8)+i*0x1C;
        FillBytes(s,0x1C,r); FillSafe(s,0,0x18,r);
        At<uint8>(s,0x18)= mode==0 ? 1 : mode==1 ? 0 : uint8(r.pct(60));
        At<uint8>(s,0x19)=uint8(r.below(2)); At<uint8>(s,0x1A)=uint8(r.below(2)); } }
inline json CaseRegister(Rng& r){
    FillScripts(r);
    const float v[6]{RandF(r),RandF(r),RandF(r),RandF(r),RandF(r),RandF(r)}; const uint32 gang=r.below(2);
    reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float,uint32)>(0x460DF0)(v[0],v[1],v[2],v[3],v[4],v[5],gang);
    return RbCommon(); }
inline json CaseClear(Rng& r){
    FillScripts(r);
    reinterpret_cast<void(__cdecl*)()>(0x460EC0)();
    return RbCommon(); }

// ---- CRoadBlocks::GetRoadBlockNodeInfo ----
inline json CaseNodeInfo(Rng& r){
    SetupPaths(r,false);
    const uint32 area=62+r.below(2), node=r.below(uint32(kNodes)); const uint32 addr=area|(node<<16);
    float w=std::bit_cast<float>(0xDEADBEEFu); CVector d{}; At<uint32>(&d,0)=0xDEADBEEFu; At<uint32>(&d,4)=0xDEADBEEFu; At<uint32>(&d,8)=0xDEADBEEFu;
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(uint32,float*,CVector*)>(0x460EE0)(addr,&w,&d)&0xFF;
    json o=RbCommon(); o["ret"]=al; o["w"]=std::bit_cast<uint32>(w);
    o["dir"]=json::array({At<uint32>(&d,0),At<uint32>(&d,4),At<uint32>(&d,8)}); return o; }

// ---- CRoadBlocks::ClearSpaceForRoadBlockObject ----
inline json CaseClearSpace(Rng& r){
    for(int i=0;i<3;++i){ const uint8 t=uint8(r.pickI({2,3,4,2,3,4,1,0,5,7})); ResetEntity(i,r,t);
        At<uint8>(&ents[size_t(i)],0x42A)=uint8(r.next()); At<uint8>(&ents[size_t(i)],0x13C)=uint8(r.pickI({1,3,5,2,0,1})); canDel[size_t(i)]=r.pct(65); }
    findCount=r.pickI({-2,0,0,1,1,1,2,2,2,3,6}); findOrder={r.below(3),r.below(3)};
    const float v[6]{RandF(r),RandF(r),RandF(r),RandF(r),RandF(r),RandF(r)};
    const uint32 al=reinterpret_cast<uint32(__cdecl*)(float,float,float,float,float,float)>(0x461020)(v[0],v[1],v[2],v[3],v[4],v[5])&0xFF;
    json o=RbCommon(); o["ret"]=al; return o; }

// ---- CRoadBlocks::GenerateRoadBlocks ----
inline json CaseGenerate(Rng& r){
    SetupPaths(r,true);
    FillBytes(reinterpret_cast<void*>(kRbBase),kRbSize,r);
    AtA<int32>(0xA43580)=int32(1+r.below(45));                                 // NumRoadBlocks
    AtA<uint8>(0xA43584)=uint8(r.below(2));                                    // GenerateDynamicRoadBlocks
    for(size_t k=0;k<325;++k){
        AtA<uint8>(0xA43438+k)=uint8(r.pct(60));                               // InOrOut
        const uint32 area=r.pct(92) ? 62+r.below(2) : 61u;
        AtA<uint32>(0xA435A0+4*k)=area|(r.below(uint32(kNodes))<<16);          // RoadBlockNodes
    }
    for(size_t i=0;i<16;++i){ auto* s=reinterpret_cast<uint8*>(0xA43AB8)+i*0x1C;
        FillSafe(s,0,0x18,r); At<uint8>(s,0x18)=uint8(r.pct(60)); At<uint8>(s,0x19)=uint8(r.below(2)); At<uint8>(s,0x1A)=uint8(r.below(2));
        if(r.pct(50)){ for(int c=0;c<3;++c){ At<float>(s,4*c)=r.uni(-60.f,60.f); At<float>(s,0xC+4*c)=r.uni(-60.f,60.f); } } }
    AtA<uint32>(0xB7CB4C)=r.pct(50) ? r.below(4)*16u+r.below(2) : r.next();     // CTimer::m_FrameCounter
    FillBytes(fakeWanted.data(),fakeWanted.size(),r);
    At<uint16>(fakeWanted.data(),0x1C)=uint16(r.pickI({0,1,10,40,64,100,127,128,200,65535}));
    hasVeh=r.pct(85);
    playerPos=CVector{r.uni(-120.f,120.f),r.uni(-120.f,120.f),r.uni(-5.f,5.f)};
    rr=Rng{r.next()|1u}; for(auto& b:script)b=r.pct(50); scN=0;
    reinterpret_cast<void(__cdecl*)()>(0x4629E0)();
    json o=RbCommon(); o["rand_state"]=rr.s; return o; }

// ---- CPools ----
inline void SetupPool(Rng& r){
    FillBytes(poolStore.data(),poolStore.size(),r);
    for(size_t s=0;s<kPoolSlots;++s){ auto* o=poolStore.data()+s*kObjStride;
        FillSafe(o,4,0x14,r); FillSafe(o,0x18,0x20,r); FillSafe(o,0x28,0x100,r);
        At<void*>(o,0)=entVt.data(); At<void*>(o,0x14)=r.pct(50) ? FMat(int(s%3)) : nullptr;
        At<uint8>(o,0x13C)=uint8(r.pickI({3,3,1,2,5,5,0,4}));
        At<int16>(o,0x22)=int16(r.pickI({0,1,100,-1,-200,19999,r.below(30000)}));
        if(r.pct(80))At<uint32>(o,0x18)=r.below(1000)+1; if(r.pct(80))At<uint32>(o,0x24)=r.below(1000)+1;
        poolFlags[s]=uint8(r.pct(35) ? (0x80|r.below(128)) : r.below(128)); }
    std::memset(fakePool.data(),0,fakePool.size());
    At<void*>(fakePool.data(),0)=poolStore.data(); At<void*>(fakePool.data(),4)=poolFlags.data(); At<int32>(fakePool.data(),8)=int32(kPoolSlots); At<int32>(fakePool.data(),0xC)=-1;
    std::memset(newObjBuf.data(),0xCD,newObjBuf.size());
    AtA<void*>(0xB7449C)=fakePool.data(); }
inline json PoolCommon(){ json o=json::object(); o["store"]=H(poolStore.data(),poolStore.size()); o["flags"]=H(poolFlags.data(),poolFlags.size());
    o["pool"]=H(fakePool.data(),fakePool.size()); o["new_obj"]=H(newObjBuf.data(),newObjBuf.size());
    o["new_obj_head"]=json::array({At<uint32>(newObjBuf.data(),0x18),At<uint32>(newObjBuf.data(),0x24),At<uint32>(newObjBuf.data(),0x22)&0xFFFF}); return o; }
inline json CaseMakeSure(Rng& r){
    SetupPool(r); FillMats(r); for(auto& b:script)b=r.pct(50); scN=0;
    const int32 slot=int32(r.below(uint32(kPoolSlots)));
    reinterpret_cast<void(__cdecl*)(int32)>(0x550080)(slot);
    json o=PoolCommon(); o["slot"]=slot; return o; }
inline json CaseCheckPools(Rng& r){
    SetupPool(r); FillMats(r);
    reinterpret_cast<void(__cdecl*)()>(0x551950)();
    return PoolCommon(); }

// ---- CIdleCam ----
inline void FillIdle(Rng& r){
    FillBytes(idleRaw.data(),idleRaw.size(),r); std::memset(idleRaw.data(),0xCD,kGuard); std::memset(idleRaw.data()+kGuard+kIdleSize,0xCD,kGuard);
    FillSafe(IdleP(),4,0x98,r);
    At<void*>(IdleP(),0)= r.pct(40) ? nullptr : static_cast<void*>(&ents[1+r.below(3)]);
    At<void*>(IdleP(),0x98)=camRaw.data();
    At<int32>(IdleP(),0x90)=int32(r.below(60)); At<int32>(IdleP(),0x94)=int32(r.pickI({0,5,100,90000,89999,90001,r.below(300000)}));
    At<float>(IdleP(),0x28)=r.pickI({0,1,50,90000,100000}) * 1.f;                            // m_TimeControlsIdleForIdleToKickIn
    At<float>(IdleP(),0x44)=r.pick({4.f,0.f,15.f}); At<float>(IdleP(),0x4C)=r.pick({80.f,10.f,500.f}); At<float>(IdleP(),0x24)=r.pick({1.f,0.5f,0.f,2.f});
    At<int32>(IdleP(),0x50)=int32(r.pickI({14,0,3,100})); At<int32>(IdleP(),0x54)=int32(r.pickI({0,1,13,14,15,99}));
    FillBytes(camRaw.data(),camRaw.size(),r); FillSafe(camRaw.data(),0,camRaw.size(),r);
    for(int c=0;c<3;++c){ At<float>(camRaw.data(),0x19C+4*c)=r.pct(20) ? 0.f : r.uni(-200.f,200.f); }
    // Los vectores de la camara (0x190..0x1A4) se leen como float: FillSafe deja enteros pequenos = denormales. El codigo SSE de la DLL
    // los trata como 0 (DAZ/FTZ del juego) y el x87 del EXE no (denormal + 0.0 -> distinto): ruido del banco, no de la fuente. Se normalizan.
    for(int c=0;c<6;++c){ uint32& b=At<uint32>(camRaw.data(),0x190+4*c); if(b&&!(b&0x7F800000u)) b=std::bit_cast<uint32>(float(b)); } }
inline void FillWorldTime(Rng& r){
    AtA<uint32>(0xB7CB84)=uint32(r.pickI({0,1000,0x7FFFFFFF,int32(0x80000000),int32(0xFFFFFFF0),int32(r.next())}));
    AtA<float>(0xB7CB5C)=r.pct(10) ? 0.f : r.uni(0.f,12.f);
    auto* pad=CPad::GetPad(0); pad->LastTimeTouched = r.pct(40) ? uint32(At<int32>(IdleP(),0x90)) : r.below(500000); }
inline json IdleCommon(){ json o=json::object();
    o["idle"]=H(idleRaw.data(),idleRaw.size()); o["cam"]=H(camRaw.data(),camRaw.size());
    for(int i=0;i<4;++i)o["ent"+std::to_string(i)]=H(&ents[size_t(i)],sizeof(Ent));
    o["pad_touch"]=CPad::GetPad(0)->LastTimeTouched; o["pad"]=H(CPad::GetPad(0),sizeof(CPad));
    o["gidle_ticker"]=AtA<uint32>(0xB6FE34); o["ie_byte"]=AtA<uint8>(0xC0B184); o["ignore"]=Name(AtA<uint32>(0xB7CD68));
    return o; }
inline json CaseIdleInit(Rng& r){ FillIdle(r); FillWorldTime(r);
    reinterpret_cast<void(__thiscall*)(void*)>(0x50E6D0)(IdleP()); return IdleCommon(); }
inline json CaseIdleReset(Rng& r){ FillIdle(r); FillWorldTime(r); const bool rc=r.pct(50);
    reinterpret_cast<void(__thiscall*)(void*,bool)>(0x50A160)(IdleP(),rc); json o=IdleCommon(); o["rc"]=rc; return o; }
inline json CaseIdleTicker(Rng& r){ FillIdle(r); FillWorldTime(r);
    reinterpret_cast<void(__thiscall*)(void*)>(0x50A200)(IdleP()); return IdleCommon(); }
inline json CaseIdleLookAt(Rng& r){ FillIdle(r); FillWorldTime(r); FillMats(r);
    const uint8 type=uint8(r.pct(70) ? 3 : r.below(8)); ResetEntity(1,r,type); At<int32>(&ents[1],0x598)=r.pickI({5,22,0,1,7,31,4,6,21,23});
    CVector out{}; At<uint32>(&out,0)=0xDEADBEEFu; At<uint32>(&out,4)=0xDEADBEEFu; At<uint32>(&out,8)=0xDEADBEEFu;
    reinterpret_cast<void(__thiscall*)(void*,void*,CVector*)>(0x50EAE0)(IdleP(),&ents[1],&out);
    json o=IdleCommon(); o["out"]=json::array({At<uint32>(&out,0),At<uint32>(&out,4),At<uint32>(&out,8)}); return o; }
inline json CaseIdleGeneral(Rng& r){ FillIdle(r); FillWorldTime(r);
    AtA<uint8>(0xC0B184)=uint8(r.next()); AtA<uint32>(0xB6FE34)=r.below(200000);
    const uint8 act=uint8(r.below(3)); AtA<uint8>(0xB6F081)=act;
    for(uint32 k=0;k<3;++k)AtA<uint16>(0xB6F1A8+0x238*k)=uint16(r.pct(45) ? 4 : r.below(60));
    reinterpret_cast<void(__thiscall*)(void*)>(0x50E690)(IdleP()); json o=IdleCommon(); o["act"]=act; return o; }
inline json CaseIdleSetTarget(Rng& r){ FillIdle(r); FillWorldTime(r);
    for(int i=1;i<4;++i)ResetEntity(i,r,uint8(r.below(8)));
    void* tgt= r.pct(25) ? nullptr : static_cast<void*>(&ents[1+r.below(3)]);
    reinterpret_cast<void(__thiscall*)(void*,void*)>(0x50A280)(IdleP(),tgt); return IdleCommon(); }
inline json CaseIdleSetPlayer(Rng& r){ FillIdle(r); FillWorldTime(r);
    for(int i=0;i<4;++i)ResetEntity(i,r,uint8(r.below(8)));
    reinterpret_cast<void(__thiscall*)(void*)>(0x50EB50)(IdleP()); return IdleCommon(); }
inline json CaseIdleValid(Rng& r){ FillIdle(r); FillWorldTime(r); FillMats(r);
    for(int i=0;i<4;++i)ResetEntity(i,r,uint8(r.pct(70) ? 3 : r.below(8)));
    for(int i=0;i<4;++i)At<int32>(&ents[size_t(i)],0x598)=r.pickI({5,22,0,1,7,31});
    for(auto& b:script)b=r.pct(50); scN=0; AtA<uint32>(0xB7CD68)=reinterpret_cast<uint32>(&ents[3]);
    void* tgt= r.pct(12) ? nullptr : static_cast<void*>(&ents[r.below(4)]);
    const uint32 al=reinterpret_cast<uint32(__thiscall*)(void*,void*)>(0x517770)(IdleP(),tgt)&0xFF;
    json o=IdleCommon(); o["ret"]=al; return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"worldwave2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"worldwave2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("worldwave2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"worldwave2-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,15> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ---
    const uintptr_t dllReg=Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t dllClean=Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t dllWorldAdd=Body(&CWorld::Add), dllWorldRemove=Body(&CWorld::Remove);
    const uintptr_t dllFind=Body(&CWorld::FindObjectsIntersectingCube);
    const uintptr_t dllCbdVeh=Body(&CVehicle::CanBeDeleted), dllCbdPed=Body(&CPed::CanBeDeleted), dllCbdObj=Body(&CObject::CanBeDeleted);
    const uintptr_t dllRand=Body(&CGeneral::GetRandomNumber);
    const uintptr_t dllWanted=Body(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted));
    const uintptr_t dllVeh=Body(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle));
    const uintptr_t dllCoors=Body(static_cast<CVector(*)(int32)>(&FindPlayerCoors));
    const uintptr_t dllPlayer=Body(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed));
    const uintptr_t dllCreate=Body(&CRoadBlocks::CreateRoadBlockBetween2Points);
    const uintptr_t dllLosPoint=Body(static_cast<bool(*)(const CVector&,const CVector&,CColPoint&,CEntity*&,bool,bool,bool,bool,bool,bool,bool,bool)>(&CWorld::ProcessLineOfSight));
    const uintptr_t dllLosClear=Body(&CWorld::GetIsLineOfSightClear);
    const uintptr_t dllProj=Body(&CProjectileInfo::RemoveIfThisIsAProjectile);
    const uintptr_t dllNew=Body(static_cast<void*(*)(size_t)>(&CObject::operator new));
    const uintptr_t dllObjCtor=Exported("??0CObject@@QAE@H_N@Z");
    if(!dllReg||!dllClean||!dllWorldAdd||!dllWorldRemove||!dllFind||!dllCbdVeh||!dllCbdPed||!dllCbdObj||!dllRand||!dllWanted||!dllVeh||!dllCoors||!dllPlayer||
       !dllCreate||!dllLosPoint||!dllLosClear||!dllProj||!dllNew||!dllObjCtor){
        Write({{"event","error"},{"reason","DLL body not found"},{"reg",dllReg},{"clean",dllClean},{"add",dllWorldAdd},{"remove",dllWorldRemove},{"find",dllFind},
               {"cbd_veh",dllCbdVeh},{"cbd_ped",dllCbdPed},{"cbd_obj",dllCbdObj},{"rand",dllRand},{"wanted",dllWanted},{"veh",dllVeh},{"coors",dllCoors},{"player",dllPlayer},
               {"create",dllCreate},{"los_point",dllLosPoint},{"los_clear",dllLosClear},{"proj",dllProj},{"new",dllNew},{"obj_ctor",dllObjCtor}}); return; }
    {   std::set<uintptr_t> all{dllReg,dllClean,dllWorldAdd,dllWorldRemove,dllFind,dllCbdVeh,dllCbdPed,dllCbdObj,dllRand,dllWanted,dllVeh,dllCoors,dllPlayer,
                                dllCreate,dllLosPoint,dllLosClear,dllProj,dllNew,dllObjCtor};
        if(all.size()!=19){ Write({{"event","error"},{"reason","DLL bodies overlap"}}); return; } }

    std::array<bool,15> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtable falsa de entidad y nombres de buferes
    for(auto& v:entVt)v=reinterpret_cast<void*>(&V_Trap); entVt[0]=reinterpret_cast<void*>(&V_Dtor);
    if(named.empty()){
        Nm(IdleP(),kIdleSize,"idle"); Nm(camRaw.data(),camRaw.size(),"cam"); Nm(newObjBuf.data(),newObjBuf.size(),"new_obj"); Nm(fakeWanted.data(),fakeWanted.size(),"wanted"); Nm(fakeVeh.data(),fakeVeh.size(),"vehicle");
        for(size_t i=0;i<ents.size();++i){ static const char* nn[6]{"ent0","ent1","ent2","ent3","ent4","ent5"}; Nm(&ents[i],sizeof(Ent),nn[i]); }
        for(size_t i=0;i<kPoolSlots;++i){ static const char* nn[4]{"slot0","slot1","slot2","slot3"}; Nm(poolStore.data()+i*kObjStride,kObjStride,nn[i]); }
        for(size_t i=0;i<fmats.size();++i){ static const char* nn[3]{"mat0","mat1","mat2"}; Nm(fmats[i].data(),0x60,nn[i]); } }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto* I=reinterpret_cast<CIdleCam*>(IdleP());
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      Chk("sizeof(CIdleCam)",uint32(sizeof(CIdleCam)),uint32(kIdleSize));
      Chk("CIdleCam.m_PositionToSlerpFrom",Off(I,&I->m_PositionToSlerpFrom),0x4); Chk("CIdleCam.m_TimeSlerpStarted",Off(I,&I->m_TimeSlerpStarted),0x10);
      Chk("CIdleCam.m_LastIdlePos",Off(I,&I->m_LastIdlePos),0x18); Chk("CIdleCam.m_SlerpTime",Off(I,&I->m_SlerpTime),0x24); Chk("CIdleCam.m_TimeControlsIdleForIdleToKickIn",Off(I,&I->m_TimeControlsIdleForIdleToKickIn),0x28);
      Chk("CIdleCam.m_DistTooClose",Off(I,&I->m_DistTooClose),0x44); Chk("CIdleCam.m_DistTooFar",Off(I,&I->m_DistTooFar),0x4C); Chk("CIdleCam.m_TargetLOSFramestoReject",Off(I,&I->m_TargetLOSFramestoReject),0x50);
      Chk("CIdleCam.m_TargetLOSCounter",Off(I,&I->m_TargetLOSCounter),0x54); Chk("CIdleCam.m_ZoomState",Off(I,&I->m_ZoomState),0x58); Chk("CIdleCam.m_CurFOV",Off(I,&I->m_CurFOV),0x70);
      Chk("CIdleCam.m_nForceAZoomOut",Off(I,&I->m_nForceAZoomOut),0x78); Chk("CIdleCam.m_bHasZoomedIn",Off(I,&I->m_bHasZoomedIn),0x79); Chk("CIdleCam.m_LastTimePadTouched",Off(I,&I->m_LastTimePadTouched),0x90);
      Chk("CIdleCam.m_IdleTickerFrames",Off(I,&I->m_IdleTickerFrames),0x94); Chk("CIdleCam.m_Cam",Off(I,&I->m_Cam),0x98);
      Chk("CScriptRoadBlock",uint32(sizeof(CScriptRoadBlock)),0x1C);
      Chk("CPathNode.m_wBaseLinkId",Off(reinterpret_cast<CPathNode*>(pathNodes.data()),&reinterpret_cast<CPathNode*>(pathNodes.data())->m_wBaseLinkId),0x10);
      Chk("CPathNode.m_nPathWidth",Off(reinterpret_cast<CPathNode*>(pathNodes.data()),&reinterpret_cast<CPathNode*>(pathNodes.data())->m_nPathWidth),0x16);
      Chk("ThePaths.m_pPathNodes",Off(&ThePaths,&ThePaths.m_pPathNodes[0]),0x804); Chk("ThePaths.m_pNaviNodes",Off(&ThePaths,&ThePaths.m_pNaviNodes[0]),0x924); Chk("ThePaths.m_pNaviLinks",Off(&ThePaths,&ThePaths.m_pNaviLinks[0]),0xDA4);
      Chk("ThePaths addr",uint32(reinterpret_cast<uintptr_t>(&ThePaths)),0x96F050);
      Chk("CWanted.m_ChanceOnRoadBlock",uint32(offsetof(CWanted,m_ChanceOnRoadBlock)),0x1C);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(kRbBase,kRbSize);
        SnapP(&ThePaths.m_pPathNodes[61],12); SnapP(&ThePaths.m_pNaviNodes[61],12); SnapP(&ThePaths.m_pNaviLinks[61],12); SnapP(&ThePaths.m_anNumAddresses[61],12);
        Snap(0xB7449C,4);                    // puntero al pool de objetos
        Snap(0xB7CB4C,4); Snap(0xB7CB5C,4); Snap(0xB7CB84,4);   // CTimer: contador de fotogramas, paso, tiempo en ms
        SnapP(CPad::GetPad(0),sizeof(CPad));
        Snap(0xB6FE34,4); Snap(0xC0B184,1); Snap(0xB6F081,1); Snap(0xB6F1A8,0x238*3); Snap(0xB7CD68,4); }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2B7Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); scN=0;
                hooks[f]->State(candidate);
                const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                Patches P;
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                if(nm=="RegisterScriptRoadBlock"){ out["state"]=CaseRegister(r); }
                else if(nm=="ClearScriptRoadBlocks"){ out["state"]=CaseClear(r); }
                else if(nm=="GetRoadBlockNodeInfo"){ out["state"]=CaseNodeInfo(r); }
                else if(nm=="ClearSpaceForRoadBlockObject"){
                    P.Add(0x568DD0,S_FindCube); P.Add(dllFind,S_FindCube); P.Add(0x563280,S_WorldRemove); P.Add(dllWorldRemove,S_WorldRemove);
                    P.Add(0x6D1180,S_CbdVeh); P.Add(dllCbdVeh,S_CbdVeh); P.Add(0x5DF100,S_CbdPed); P.Add(dllCbdPed,S_CbdPed); P.Add(0x59F120,S_CbdObj); P.Add(dllCbdObj,S_CbdObj);
                    out["state"]=CaseClearSpace(r); }
                else if(nm=="GenerateRoadBlocks"){
                    P.Add(0x821B1E,S_RandExe); P.Add(dllRand,S_RandDll); P.Add(0x56E230,S_Wanted); P.Add(dllWanted,S_Wanted); P.Add(0x56E0D0,S_Veh); P.Add(dllVeh,S_Veh);
                    P.Add(0x56E010,S_Coors); P.Add(dllCoors,S_Coors); P.Add(0x4619C0,S_Create); P.Add(dllCreate,S_Create); P.Add(0x56BA00,S_LosPoint); P.Add(dllLosPoint,S_LosPoint);
                    out["state"]=CaseGenerate(r); }
                else if(nm=="MakeSureSlotInObjectPoolIsEmpty"){
                    P.Add(0x563280,S_WorldRemove); P.Add(dllWorldRemove,S_WorldRemove); P.Add(0x563220,S_WorldAdd); P.Add(dllWorldAdd,S_WorldAdd);
                    P.Add(0x739A40,S_Projectile); P.Add(dllProj,S_Projectile); P.Add(0x5A1EE0,S_New); P.Add(dllNew,S_New); P.Add(0x5A1D70,S_ObjCtor); P.Add(dllObjCtor,S_ObjCtor);
                    out["state"]=CaseMakeSure(r); }
                else if(nm=="CheckPoolsEmpty"){ P.Add(0x821982,S_Printf); out["state"]=CaseCheckPools(r); }
                else if(nm=="Init"){ out["state"]=CaseIdleInit(r); }
                else if(nm=="Reset"){ out["state"]=CaseIdleReset(r); }
                else if(nm=="ProcessIdleCamTicker"){ out["state"]=CaseIdleTicker(r); }
                else if(nm=="GetLookAtPositionOnTarget"){ out["state"]=CaseIdleLookAt(r); }
                else if(nm=="IdleCamGeneralProcess"){ out["state"]=CaseIdleGeneral(r); }
                else if(nm=="SetTarget"){ P.Add(0x571A00,S_Clean); P.Add(dllClean,S_Clean); P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg); out["state"]=CaseIdleSetTarget(r); }
                else if(nm=="SetTargetPlayer"){
                    P.Add(0x571A00,S_Clean); P.Add(dllClean,S_Clean); P.Add(0x571B70,S_Reg); P.Add(dllReg,S_Reg); P.Add(0x56E210,S_Player); P.Add(dllPlayer,S_Player);
                    out["state"]=CaseIdleSetPlayer(r); }
                else if(nm=="IsTargetValid"){
                    P.Add(0x56E210,S_Player); P.Add(dllPlayer,S_Player); P.Add(0x56A490,S_LosClear); P.Add(dllLosClear,S_LosClear);
                    out["state"]=CaseIdleValid(r); }
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
