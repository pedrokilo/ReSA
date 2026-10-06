#pragma once
// Banco dentro del motor: CVehicle grupo C (mejoras, atomics, remaps y componentes), 26 funciones. Cada funcion se ejecuta sobre un
// CVehicle FALSO (bufer de sizeof(CVehicle) con patron aleatorio por caso mas una franja de guarda 0xCD; vptr a una vtable falsa con
// trampas y el hueco 48 = SetupSuspensionLines trazado) una vez con el codigo del EXE (hooks del grupo desactivados) y otra a traves
// del hook (envoltorio -> DLL), siempre llamando a la direccion del EXE.
// Se compara el bufer completo del vehiculo, la guarda, el retorno, los argumentos de salida, todos los objetos falsos tocados
// (clumps, marcos, atomics, geometrias, materiales, texturas, model infos), el estado lateral de los dobles (identificadores de
// atomics, jerarquia de marcos, alfa de clump) y las globales tocadas, mas la traza ordenada de llamadas salientes.
// Todo lo externo se SIMULA y se traza con parches de entrada (exe + cuerpo de la DLL cuando existe): CVisibilityPlugins (GetAtomicId,
// GetModelInfo, GetModelInfoIndex, Get/SetFrameHierarchyId, SetAtomicId/Flag/RenderCallback, Get/SetClumpAlpha), RenderWare (Rp/Rw
// ClumpForAllAtomics, FrameForAllObjects/Children (ejecutan los callbacks sobre listas falsas), GeometryForAllMaterials, AtomicClone/
// Destroy/SetFrame, ClumpAdd/RemoveAtomic, FrameAddChild/Destroy, MatrixUpdate, TextureDestroy), CBaseModelInfo AddRef/RemoveRef y
// CreateInstance (hueco 11 de una vtable falsa), CClumpModelInfo::GetFrameFromId, CVehicleModelInfo (GetNumRemaps, Get/SetCustomCarPlateText,
// SetRenderPipelinesCB), CCustomCarPlateMgr::CreatePlateTexture, CTxdStore::RemoveRef, CAutomobile::NitrousControl,
// CAudioEngine::SetRadioBassSetting, CEntity::RegisterReference, CCollision::RemoveTrianglePlanes (0x4185A0), CColModel::AllocateData/operator=,
// CQuaternion::Get(RwMatrix*) y AddVehicleUpgrade (0x6E3290). Se ejecutan de verdad (en ambos lados): HasAlphaMaterialCB, los callbacks
// del EXE RemoveUpgradeCB/FindUpgradeCB/FindReplacementUpgradeCB/SetCompAlphaCB (la DLL los llama por su direccion), CMatrix (ctor,
// SetRotateX, RotateY, UpdateRW, dtor: el EXE usa el suyo y la DLL el de la DLL) y RwMatrix sobre marcos falsos.
// Los punteros a callbacks se normalizan por nombre (exe 0x6D2690 == funcion de la DLL). No se toca ningun vehiculo real, pool ni
// textura real. Globales guardadas/restauradas: model infos 15..18, indices de modelo de mejoras (0x8CD760..), ms_bCreateDamagedVersion
// (0xA9B0B0), CopyObjectsCB_TargetClump y los vehiculos/colmodels/hidraulicos especiales (0xC1CB58..0xC1CD38).
#include <fstream>
#include <set>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Models/ClumpModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "VisibilityPlugins.h"
#include "CustomCarPlateMgr.h"
#include "TxdStore.h"
#include "Collision/ColModel.h"
#include "Collision/Collision.h"
#include "Core/Quaternion.h"
#include "Audio/AudioEngine.h"
#include "../ProbeRuntime.hpp"

namespace CVehCLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
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

// --- bufers con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v<0x10000)return v;                       // valores pequenos pasados como puntero (identificadores, alfa...)
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

// --- modulo de la DLL y cuerpos de funciones (como VehCtor62) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
template<class Fp> uintptr_t Dll(Fp f) { return FollowIn(Addr(f)); }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

// ======================= objetos falsos =======================
template<size_t S> struct alignas(16) Blob { uint8 b[S]; };
constexpr size_t kVehSize=0x5A0, kGuard=32;
constexpr int NA=10, NF=10;                          // atomics (0..5 escena, 6/7 CreateInstance, 8 clon, 9 libre) y marcos (0..7 escena, 8/9 de CreateInstance)
alignas(16) inline std::array<uint8,kVehSize+kGuard> veh{};
alignas(16) inline std::array<uint8,0x120> hnd{};            // tHandlingData falso (m_nModelFlags en +0xCC)
alignas(16) inline std::array<Blob<0x40>,2> clump{};         // 0 = clump del vehiculo, 1 = clump del model info
alignas(16) inline std::array<Blob<0xB0>,NF> frames{};       // RwFrame falso (0xA8 reales; padre en +4, matriz de modelado en +0x10)
alignas(16) inline std::array<Blob<0x90>,NA> atoms{};        // RpAtomic falso (frame/padre +4, geometria +0x18, clump +0x3C)
alignas(16) inline std::array<Blob<0x60>,3> geos{};          // RpGeometry falso (flags en +8)
alignas(16) inline std::array<Blob<0x20>,6> mats{};          // RpMaterial falso (textura +0, alfa +7); 4 = material de matricula
alignas(16) inline std::array<Blob<0x60>,4> tex{};           // RwTexture falso (refCount en +0x54); 3 = textura devuelta por CreatePlateTexture
alignas(16) inline std::array<Blob<0x600>,2> miBuf{};        // CVehicleModelInfo falso: 0 = del vehiculo, 1 = argumento (CreateUpgradeAtomic/CreateReplacementAtomic/plate)
alignas(16) inline std::array<Blob<0x40>,4> miA{};           // CBaseModelInfo de los atomics (word en +0x12)
alignas(16) inline Blob<0x500> vstruct{};                    // estructura del vehiculo del model info (m_aUpgrades[15].m_nParentComponentId en +0x2B0)
alignas(16) inline Blob<0x60> colM{};                        // CColModel del model info
alignas(16) inline Blob<0x20> posn{};                        // UpgradePosnDesc falso
alignas(16) inline std::array<char,64> plateText{};
alignas(16) inline std::array<int32,8> outInt{};             // int& / data de los callbacks
inline std::array<void*,64> vtVeh{}, vtMi{};

struct World {
    int32 aId[NA]; int32 aModelIdx[NA]; int32 aCb[NA]; int8 aMi[NA]; int8 aGeo[NA];
    int32 fHier[NF]; int8 fObjs[NF][4]; int8 fKids[NF][4]; int8 cAtoms[6]; int8 gMats[3][3]; int8 frameMap[2][8];
    int32 clumpAlpha; int32 numRemaps; int32 createCount; int32 hasPlateText;
};
inline World W{};
constexpr uintptr_t kGlobBase=0xC1CB58, kGlobSize=0x1E0;     // CopyObjectsCB_TargetClump .. fin de m_aSpecialColModel

template<size_t N,size_t S> int IndexOf(const std::array<Blob<S>,N>& a,const void* p){ for(size_t i=0;i<N;++i)if(p==a[i].b)return int(i); return -1; }
inline int AtomIdx(const void* p){ return IndexOf(atoms,p); }
inline int FrameIdx(const void* p){ return IndexOf(frames,p); }
inline int GeoIdx(const void* p){ return IndexOf(geos,p); }
inline int MatIdx(const void* p){ return IndexOf(mats,p); }

// --- nombres de callbacks (exe == DLL) ---
struct CbEntry { const char* name; uintptr_t exe; uintptr_t dll; };
inline std::vector<CbEntry> cbs;
inline json CbName(const void* cb,const char* fallbackDll=nullptr){
    const uintptr_t a=uintptr_t(cb); const uintptr_t f=FollowIn(a);
    for(auto& c:cbs){ if(a==c.exe)return c.name; if(c.dll&&(a==c.dll||f==FollowIn(c.dll)))return c.name; }
    if(fallbackDll&&InDll(f))return fallbackDll;
    return "other";
}

using ObjCb=RwObject*(__cdecl*)(RwObject*,void*);
using FrmCb=RwFrame*(__cdecl*)(RwFrame*,void*);
using AtomCb=RpAtomic*(__cdecl*)(RpAtomic*,void*);
using MatCb=RpMaterial*(__cdecl*)(RpMaterial*,void*);

// ======================= dobles (se parchean en la direccion del EXE y en el cuerpo de la DLL) =======================
// CVisibilityPlugins (estaticas, cdecl)
inline int32 __cdecl S_GetAtomicId(RpAtomic* a){ const int i=AtomIdx(a); const int32 v=i>=0 ? W.aId[i] : 0x5A5A; T("GetAtomicId",a,v); return v; }
inline CBaseModelInfo* __cdecl S_GetModelInfo(RpAtomic* a){ const int i=AtomIdx(a); void* mi=(i>=0&&W.aMi[i]>=0) ? miA[W.aMi[i]].b : nullptr; T("GetModelInfo",a,mi); return static_cast<CBaseModelInfo*>(mi); }
inline int32 __cdecl S_GetModelInfoIndex(RpAtomic* a){ const int i=AtomIdx(a); const int32 v=i>=0 ? W.aModelIdx[i] : -1; T("GetModelInfoIndex",a,v); return v; }
inline int32 __cdecl S_GetFrameHierarchyId(RwFrame* f){ const int i=FrameIdx(f); const int32 v=i>=0 ? W.fHier[i] : 0x33; T("GetFrameHierarchyId",f,v); return v; }
inline void __cdecl S_SetFrameHierarchyId(RwFrame* f,int32 id){ const int i=FrameIdx(f); if(i>=0)W.fHier[i]=id; T("SetFrameHierarchyId",f,id); }
inline void __cdecl S_SetAtomicId(RpAtomic* a,int32 id){ const int i=AtomIdx(a); if(i>=0)W.aId[i]=id; T("SetAtomicId",a,id); }
inline void __cdecl S_SetAtomicFlag(RpAtomic* a,int32 f){ const int i=AtomIdx(a); if(i>=0)W.aId[i]|=f; T("SetAtomicFlag",a,f); }
inline void __cdecl S_SetAtomicRenderCallback(RpAtomic* a,void* cb){ const auto nm=CbName(cb); const int i=AtomIdx(a); const auto s=nm.dump(); if(i>=0)W.aCb[i]=int32(Fnv(s.data(),s.size())); T("SetAtomicRenderCallback",a,nm); }
inline int32 __cdecl S_GetClumpAlpha(RpClump* c){ T("GetClumpAlpha",c,W.clumpAlpha); return W.clumpAlpha; }
inline void __cdecl S_SetClumpAlpha(RpClump* c,int32 v){ W.clumpAlpha=v; T("SetClumpAlpha",c,v); }
inline RpAtomic* __cdecl S_SetRenderPipelinesCB(RpAtomic* a,void* d){ T("SetRenderPipelinesCB",a,d); return a; }
// CBaseModelInfo / CClumpModelInfo / CVehicleModelInfo (thiscall -> fastcall con dummy)
inline void __fastcall S_AddRef(void* self,void*){ T("AddRef",self); }
inline void __fastcall S_RemoveRef(void* self,void*){ T("RemoveRef",self); }
inline RwFrame* __cdecl S_GetFrameFromId(RpClump* c,int32 id){ const int v=(c==static_cast<void*>(clump[1].b)) ? 1 : 0; RwFrame* f=reinterpret_cast<RwFrame*>(frames[W.frameMap[v][uint32(id)%8]].b); T("GetFrameFromId",c,id,f); return f; }
inline int32 __fastcall S_GetNumRemaps(void* self,void*){ T("GetNumRemaps",self,W.numRemaps); return W.numRemaps; }
inline char* __fastcall S_GetPlateText(void* self,void*){ char* t=W.hasPlateText ? plateText.data() : nullptr; T("GetCustomCarPlateText",self,t); return t; }
inline void __fastcall S_SetPlateText(void* self,void*,char* t){ T("SetCustomCarPlateText",self,t); }
inline RwTexture* __cdecl S_CreatePlate(const char* text,uint32 type){ T("CreatePlateTexture",text,uint8(type)); return reinterpret_cast<RwTexture*>(tex[3].b); }   // la parte alta del tipo es basura de pila en el EXE
inline void __cdecl S_TxdRemoveRef(int32 id){ T("CTxdStore::RemoveRef",id); }
// vtable falsa del model info: hueco 11 = CreateInstance()
inline void* __fastcall V_CreateInstance(void* self,void*){ const int k=W.createCount++; void* a=atoms[6+(k&1)].b; T("CreateInstance",self,a); return a; }
// CAutomobile / audio / entidad
inline void __fastcall S_Nitrous(void* self,void*,int32 v){ T("NitrousControl",self,int8(v)); }
inline void __fastcall S_Bass(void* self,void*,int32 v){ T("SetRadioBassSetting",self,uint8(v)); }
inline void __fastcall S_RegRef(void* self,void*,void** ref){ T("RegisterReference",self,ref); }
inline void __fastcall S_AddVehicleUpgrade(void* self,void*,int32 id){ T("AddVehicleUpgrade",self,id,Fnv(static_cast<uint8*>(self)+0x43A,30)); }
// colisiones
inline void __cdecl S_RemoveTrianglePlanes(void* cm){ T("CCollision::RemoveTrianglePlanes",cm); }
inline void __fastcall S_AllocateData(void* self,void*){ T("AllocateData",self); At<uint32>(self,0x2C)=0xA11C0000u; }
inline void* __fastcall S_ColAssign(void* self,void*,const void* src){ T("CColModel::operator=",self,src); return self; }
// CQuaternion::Get(RwMatrix*) const: escribe 9 floats deterministas segun el cuaternion
inline void __fastcall S_QuatGet(void* self,void*,RwMatrix* m){
    const uint64 h=Fnv(self,16); T("CQuaternion::Get",self,m,h);
    for(int k=0;k<9;++k){ const uint32 w=uint32(h>>(k*3))*2654435761u+uint32(k); const float f=float(int32(w&0xFFFF)-0x8000)/16384.f; At<float>(m,(k/3)*0x10+(k%3)*4)=f; } }
// RenderWare (cdecl, solo direccion del EXE: los envoltorios de la DLL llaman a esas direcciones)
inline RpClump* __cdecl S_ClumpRemoveAtomic(RpClump* c,RpAtomic* a){ T("RpClumpRemoveAtomic",c,a); return c; }
inline int32 __cdecl S_AtomicDestroy(RpAtomic* a){ T("RpAtomicDestroy",a); return 1; }
inline int32 __cdecl S_FrameDestroy(RwFrame* f){ T("RwFrameDestroy",f); return 1; }
inline RpAtomic* __cdecl S_AtomicClone(RpAtomic* a){ void* c=atoms[8].b; T("RpAtomicClone",a,c); return static_cast<RpAtomic*>(c); }
inline RpClump* __cdecl S_ClumpAddAtomic(RpClump* c,RpAtomic* a){ T("RpClumpAddAtomic",c,a); At<void*>(a,0x3C)=c; return c; }
inline RpAtomic* __cdecl S_AtomicSetFrame(RpAtomic* a,RwFrame* f){ T("RpAtomicSetFrame",a,f); At<void*>(a,4)=f; return a; }
inline RwFrame* __cdecl S_FrameAddChild(RwFrame* p,RwFrame* c){ T("RwFrameAddChild",p,c); return p; }
inline RwMatrix* __cdecl S_MatrixUpdate(RwMatrix* m){ T("RwMatrixUpdate",m,Fnv(m,0x40)); return m; }
inline int32 __cdecl S_TextureDestroy(RwTexture* t){ T("RwTextureDestroy",t); return 1; }
inline RpClump* __cdecl S_ClumpForAllAtomics(RpClump* c,AtomCb cb,void* data){
    T("RpClumpForAllAtomics",c,CbName(reinterpret_cast<const void*>(cb)),data);
    if(c==static_cast<void*>(clump[0].b))for(int k=0;k<6;++k){ const int a=W.cAtoms[k]; if(a<0)break; if(!cb(reinterpret_cast<RpAtomic*>(atoms[a].b),data))break; }
    return c; }
inline RwFrame* __cdecl S_FrameForAllObjects(RwFrame* f,ObjCb cb,void* data){
    T("RwFrameForAllObjects",f,CbName(reinterpret_cast<const void*>(cb)),data);
    const int fi=FrameIdx(f); if(fi>=0)for(int k=0;k<4;++k){ const int a=W.fObjs[fi][k]; if(a<0)break; if(!cb(reinterpret_cast<RwObject*>(atoms[a].b),data))break; }
    return f; }
inline RwFrame* __cdecl S_FrameForAllChildren(RwFrame* f,FrmCb cb,void* data){
    T("RwFrameForAllChildren",f,CbName(reinterpret_cast<const void*>(cb)),data);
    const int fi=FrameIdx(f); if(fi>=0)for(int k=0;k<4;++k){ const int c=W.fKids[fi][k]; if(c<0)break; if(!cb(reinterpret_cast<RwFrame*>(frames[c].b),data))break; }
    return f; }
inline RpGeometry* __cdecl S_GeometryForAllMaterials(RpGeometry* g,MatCb cb,void* data){
    T("RpGeometryForAllMaterials",g,CbName(reinterpret_cast<const void*>(cb),"SetCompAlphaCB"),data);
    const int gi=GeoIdx(g); if(gi>=0)for(int k=0;k<3;++k){ const int m=W.gMats[gi][k]; if(m<0)break; if(!cb(reinterpret_cast<RpMaterial*>(mats[m].b),data))break; }
    return g; }
// vtable falsa del vehiculo: hueco 48 (0xC0) = SetupSuspensionLines
inline void __fastcall V_Setup(void* self,void*){ T("SetupSuspensionLines",self,Fnv(self,kVehSize)); }
template<int N> void __fastcall V_Trap(void* self,void*){ T("virtual_inesperada",self,N); }
template<size_t... I> void FillTraps(std::array<void*,64>& v,std::index_sequence<I...>){ ((v[I]=reinterpret_cast<void*>(&V_Trap<int(I)>)),...); }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,26> fns{{
    {"CVehicle","ClearVehicleUpgradeFlags",0x6D3210,0,-1,512},
    {"CVehicle","SetVehicleUpgradeFlags",0x6D30E0,0,-1,512},
    {"CVehicle","GetUpgrade",0x6D3650,0,-1,384},
    {"CVehicle","RemoveUpgrade",0x6D3630,0,-1,384},
    {"CVehicle","RemoveAllUpgrades",0x6D3AB0,0,-1,192},
    {"CVehicle","GetReplacementUpgrade",0x6D3A50,0,-1,256},
    {"CVehicle","RemoveReplacementUpgrade",0x6D39E0,0,-1,256},
    {"CVehicle","CreateUpgradeAtomic",0x6D3510,0,-1,384},
    {"CVehicle","CreateReplacementAtomic",0x6D3700,0,-1,512},
    {"CVehicle","CustomCarPlate_TextureCreate",0x6D10E0,0,-1,256},
    {"CVehicle","CustomCarPlate_TextureDestroy",0x6D1150,0,-1,64},
    {"CVehicle","GetRemapIndex",0x6D0B70,0,-1,384},
    {"CVehicle","SetRemap",0x6D0C00,0,-1,384},
    {"CVehicle","GetVehicleAppearance",0x6D1080,0,-1,256},
    {"CVehicle","SetComponentVisibility",0x6D2700,0,-1,256},
    {"CVehicle","SetComponentAtomicAlpha",0x6D2960,0,-1,192},
    {"CVehicle","UpdateClumpAlpha",0x6D2980,0,-1,384},
    {"CVehicle","SetVehicleAtomicVisibilityCB-Object",0x6D2690,0,-1,384},
    {"CVehicle","SetVehicleAtomicVisibilityCB-Frame",0x6D26D0,0,-1,256},
    {"CVehicle","RemoveObjectsCB-Object",0x6D33B0,0,-1,512},
    {"CVehicle","RemoveObjectsCB-Frame",0x6D3420,0,-1,256},
    {"CVehicle","CopyObjectsCB",0x6D3450,0,-1,256},
    {"CVehicle","RemoveAllUpgradesCB",0x6D34D0,0,-1,256},
    {"CVehicle","SetupUpgradesAfterLoad",0x6E3400,0,-1,128},
    {"CVehicle","GetSpecialColModel",0x6DF3D0,0,-1,512},
    {"CVehicle","SetTransmissionRotation",0x6DBBB0,0,-1,512},
}};

// ======================= fixtures =======================
inline void FillFrameMatrix(uint8* f,Rng& r){
    for(size_t k=0;k<sizeof(Blob<0xB0>);++k)f[k]=uint8(r.next());
    uint8* m=f+0x10;                                // RwMatrix: right 0, flags 0xC, up 0x10, at 0x20, pos 0x30
    const bool ident=r.pct(12);
    for(int row=0;row<3;++row)for(int c=0;c<3;++c)At<float>(m,0x10*row+4*c)=ident ? (row==c?1.f:0.f) : r.uni(-1.5f,1.5f);
    At<uint32>(m,0xC)=ident ? 0x20003u : uint32(r.pickI({3,2,1,0x20003,0x20000,0}));
    At<uint32>(m,0x1C)=0; At<uint32>(m,0x2C)=0; At<uint32>(m,0x3C)=0x3F800000u;
    for(int c=0;c<3;++c)At<float>(m,0x30+4*c)=r.pct(5) ? r.uni(-500.f,500.f) : r.uni(-4.f,4.f);
    if(r.pct(8))At<float>(m,0x34)=0.f;
    At<void*>(f,4)=nullptr; }

inline uint32 Alpha(Rng& r){ return uint32(r.pickI({0,1,5,8,9,100,239,240,247,254,255,256,300,1000,int32(r.below(300)),int32(r.below(300)),-5})); }

// escena comun: vehiculo, clumps, marcos (con listas de objetos e hijos), atomics, geometrias, materiales, texturas, model infos
inline void BuildScene(Rng& r,int32 idx){
    std::memset(&W,0,sizeof(W));
    std::memset(W.aMi,-1,sizeof(W.aMi)); std::memset(W.aGeo,-1,sizeof(W.aGeo)); std::memset(W.fObjs,-1,sizeof(W.fObjs)); std::memset(W.fKids,-1,sizeof(W.fKids));
    std::memset(W.cAtoms,-1,sizeof(W.cAtoms)); std::memset(W.gMats,-1,sizeof(W.gMats));
    // vehiculo
    for(size_t k=0;k<kVehSize;++k)veh[k]=uint8(r.next());
    std::memset(veh.data()+kVehSize,0xCD,kGuard);
    At<void*>(veh.data(),0)=vtVeh.data(); At<void*>(veh.data(),0x18)=clump[0].b; At<int16>(veh.data(),0x22)=int16(idx); At<void*>(veh.data(),0x384)=hnd.data();
    for(auto& b:hnd)b=uint8(r.next());
    for(auto& c:clump){ for(auto& b:c.b)b=uint8(r.next()); c.b[0]=2; }          // type rpCLUMP
    for(auto& f:frames)FillFrameMatrix(f.b,r);
    // marcos: objetos e hijos
    for(int f=0;f<8;++f){ W.fHier[f]=r.pickI({0,0,1,7,-1,int32(r.below(100))}); int n=0; for(int c=f+1;c<8&&n<4;++c)if(r.pct(25))W.fKids[f][n++]=int8(c); }
    W.fHier[8]=r.pickI({0,5,-1}); W.fHier[9]=r.pickI({0,5,-1});
    for(int m=0;m<2;++m)for(int k=0;k<8;++k)W.frameMap[m][k]=int8(r.below(8));
    // geometrias y materiales
    for(auto& g:geos){ for(auto& b:g.b)b=uint8(r.next()); At<uint32>(g.b,8)=r.next(); }
    for(auto& m:mats){ for(auto& b:m.b)b=uint8(r.next()); m.b[7]=r.pct(55) ? 255 : uint8(r.below(255)); }
    for(auto& t:tex){ for(auto& b:t.b)b=uint8(r.next()); At<int32>(t.b,0x54)=int32(r.below(10)); }
    for(int m=0;m<4;++m)At<void*>(mats[m].b,0)=tex[m&1].b; At<void*>(mats[4].b,0)=tex[r.below(3)].b;
    for(int g=0;g<3;++g){ const int n=int(r.below(4)); for(int k=0;k<n;++k)W.gMats[g][k]=int8(r.below(4)); }
    // model infos de los atomics
    for(auto& m:miA){ for(auto& b:m.b)b=uint8(r.next()); const uint32 id=r.below(24); At<uint16>(m.b,0x12)=uint16((id<<10)|(r.pct(30)?0x100:0)|(r.next()&0xFF)); }
    // atomics
    for(int a=0;a<NA;++a){ auto* ab=atoms[a].b; for(size_t k=0;k<sizeof(ab[0])*0x90;++k)ab[k]=uint8(r.next()); ab[0]=1; ab[1]=0; }
    for(int a=0;a<6;++a){
        const int f=int(r.below(8)); for(int k=0;k<4;++k)if(W.fObjs[f][k]<0){ W.fObjs[f][k]=int8(a); break; }
        At<void*>(atoms[a].b,4)=frames[f].b; At<void*>(atoms[a].b,0x3C)=r.pct(80) ? static_cast<void*>(clump[0].b) : nullptr;
        const int g=int(r.below(3)); At<void*>(atoms[a].b,0x18)=geos[g].b; W.aGeo[a]=int8(g);
        int32 id=int32(r.next()&0xFFFF); if(r.pct(40))id|=0x800; else id&=~0x800; if(r.pct(10))id|=int32(r.next()&0x7FFF0000u);
        W.aId[a]=id; W.aMi[a]=(r.pct(75)||(id&0x800)) ? int8(r.below(4)) : int8(-1);
        W.aModelIdx[a]=r.pickI({-1,-1,0,15,int32(r.below(20000)),int32(int16(r.next())),int32(r.below(300))}); }
    { int n=0; for(int a=0;a<6;++a)if(r.pct(75))W.cAtoms[n++]=int8(a); }
    for(int a=6;a<10;++a){ const int fr=(a<8)?(8+(a-6)):int(r.below(8)); At<void*>(atoms[a].b,4)=frames[fr].b; At<void*>(atoms[a].b,0x18)=geos[r.below(3)].b; At<void*>(atoms[a].b,0x3C)=nullptr; W.aId[a]=int32(r.next()&0xFFFF); W.aMi[a]=-1; }
    W.clumpAlpha=int32(Alpha(r));
    // model info del vehiculo + argumento
    for(auto& m:miBuf){ for(auto& b:m.b)b=uint8(r.next()); At<void*>(m.b,0)=vtMi.data(); At<void*>(m.b,0x14)=colM.b; At<void*>(m.b,0x1C)=clump[1].b; At<void*>(m.b,0x5C)=vstruct.b;
        At<void*>(m.b,0x24)=r.pct(75) ? static_cast<void*>(mats[4].b) : nullptr; }
    for(auto& b:vstruct.b)b=uint8(r.next()); At<int32>(vstruct.b,0x2B0)=r.pickI({-1,0,1,-5,int32(r.next()),5});
    for(auto& b:colM.b)b=uint8(r.next());
    for(auto& b:posn.b)b=uint8(r.next()); for(int k=0;k<7;++k)At<float>(posn.b,4*k)=r.uni(-2.f,2.f);
    for(auto& c:plateText)c=char('A'+r.below(26)); plateText[7]=0; W.hasPlateText=int32(r.pct(50));
    for(auto& v:outInt)v=int32(r.next()|1u);
    AtA<void*>(0xA9B0C8+4*idx)=miBuf[0].b;
    AtA<uint8>(0xA9B0B0)=uint8(r.pct(10));
    AtA<uint32>(0xC1CB58)=r.pct(50) ? uint32(uintptr_t(clump[0].b)) : r.next();
}
inline void SetModelIndexGlobals(Rng& r){ for(int k=0;k<5;++k)AtA<uint16>(0x8CD760+4*k)=uint16(1+r.below(20000)); }

template<class R,class... A> inline R CallThis(uintptr_t addr,void* self,A... a){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(addr)(self,a...); }
template<class R,class... A> inline R CallC(uintptr_t addr,A... a){ return reinterpret_cast<R(__cdecl*)(A...)>(addr)(a...); }

struct Res { json j; std::vector<uint8> vehBytes; };

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cvehc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cvehc-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cvehc");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    InitModule();
    std::string only; { std::ifstream f(dir/"cvehc-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,26> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,26> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);hooks[f]->State(false);}

    // vtables falsas
    FillTraps(vtVeh,std::make_index_sequence<64>{}); vtVeh[48]=reinterpret_cast<void*>(&V_Setup);
    FillTraps(vtMi,std::make_index_sequence<64>{}); vtMi[11]=reinterpret_cast<void*>(&V_CreateInstance);
    if(named.empty()){
        Nm(veh.data(),veh.size(),"veh"); Nm(hnd.data(),hnd.size(),"hnd"); Nm(clump.data(),sizeof(clump),"clump"); Nm(frames.data(),sizeof(frames),"frames"); Nm(atoms.data(),sizeof(atoms),"atoms");
        Nm(geos.data(),sizeof(geos),"geos"); Nm(mats.data(),sizeof(mats),"mats"); Nm(tex.data(),sizeof(tex),"tex"); Nm(miBuf.data(),sizeof(miBuf),"mi"); Nm(miA.data(),sizeof(miA),"miA");
        Nm(vstruct.b,sizeof(vstruct),"vstruct"); Nm(colM.b,sizeof(colM),"colM"); Nm(posn.b,sizeof(posn),"posn"); Nm(plateText.data(),plateText.size(),"plate"); Nm(outInt.data(),sizeof(outInt),"out");
        Nm(reinterpret_cast<void*>(kGlobBase),kGlobSize,"gSpecial"); Nm(reinterpret_cast<void*>(0xB6BC90),0x10,"AudioEngine"); }

    // nombres de callbacks (EXE y DLL)
    cbs.push_back({"SetVisObj",0x6D2690,Addr(static_cast<RwObject*(*)(RwObject*,void*)>(&SetVehicleAtomicVisibilityCB))});
    cbs.push_back({"SetVisFrame",0x6D26D0,Addr(static_cast<RwFrame*(*)(RwFrame*,void*)>(&SetVehicleAtomicVisibilityCB))});
    cbs.push_back({"RemoveObj",0x6D33B0,Addr(static_cast<RwObject*(*)(RwObject*,void*)>(&RemoveObjectsCB))});
    cbs.push_back({"RemoveFrame",0x6D3420,Addr(static_cast<RwFrame*(*)(RwFrame*,void*)>(&RemoveObjectsCB))});
    cbs.push_back({"CopyObj",0x6D3450,Addr(&CopyObjectsCB)});
    cbs.push_back({"RemoveAllUpg",0x6D34D0,Addr(&RemoveAllUpgradesCB)});
    cbs.push_back({"RemoveUpgCB",0x6D3300,Addr(&RemoveUpgradeCB)});
    cbs.push_back({"FindUpgCB",0x6D3370,Addr(&FindUpgradeCB)});
    cbs.push_back({"FindReplCB",0x6D3490,Addr(&FindReplacementUpgradeCB)});
    cbs.push_back({"SetCompAlphaCB",0x6D2950,0});
    cbs.push_back({"HasAlphaCB",0x4C77C0,Addr(&CVehicleModelInfo::HasAlphaMaterialCB)});
    cbs.push_back({"RenderHiDetail",0x733240,Addr(&CVisibilityPlugins::RenderVehicleHiDetailCB)});
    cbs.push_back({"RenderHiDetailAlpha",0x733F80,Addr(&CVisibilityPlugins::RenderVehicleHiDetailAlphaCB)});

    // disposicion de las estructuras falsas frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      auto* fr=reinterpret_cast<RwFrame*>(frames[0].b); auto* at=reinterpret_cast<RpAtomic*>(atoms[0].b); auto* ge=reinterpret_cast<RpGeometry*>(geos[0].b);
      auto* ma=reinterpret_cast<RpMaterial*>(mats[0].b); auto* te=reinterpret_cast<RwTexture*>(tex[0].b); auto* vmi=reinterpret_cast<CVehicleModelInfo*>(miBuf[0].b);
      auto* up=reinterpret_cast<UpgradePosnDesc*>(posn.b);
      Chk("RwFrame.object.parent",Off(fr,&fr->object.parent),4); Chk("RwFrame.modelling",Off(fr,&fr->modelling),0x10); Chk("sizeof(RwFrame)<=0xB0",uint32(sizeof(RwFrame)<=0xB0),1);
      Chk("RpAtomic.object.object.parent",Off(at,&at->object.object.parent),4); Chk("RpAtomic.geometry",Off(at,&at->geometry),0x18); Chk("RpAtomic.clump",Off(at,&at->clump),0x3C);
      Chk("sizeof(RpAtomic)<=0x90",uint32(sizeof(RpAtomic)<=0x90),1);
      Chk("RpGeometry.flags",Off(ge,&ge->flags),8); Chk("RpMaterial.color",Off(ma,&ma->color),4); Chk("RwTexture.refCount",Off(te,&te->refCount),0x54);
      Chk("RwMatrix.flags",uint32(offsetof(RwMatrix,flags)),0xC); Chk("RwMatrix.pos",uint32(offsetof(RwMatrix,pos)),0x30);
      Chk("CVehicleModelInfo.m_pPlateMaterial",Off(vmi,&vmi->m_pPlateMaterial),0x24); Chk("CVehicleModelInfo.m_nPlateType",Off(vmi,&vmi->m_nPlateType),0x31);
      Chk("CVehicleModelInfo.m_pVehicleStruct",Off(vmi,&vmi->m_pVehicleStruct),0x5C); Chk("CVehicleModelInfo.m_anRemapTxds",Off(vmi,&vmi->m_anRemapTxds),0x2FA);
      { vmi->m_pVehicleStruct=reinterpret_cast<decltype(vmi->m_pVehicleStruct)>(vstruct.b); Chk("m_aUpgrades[15].m_nParentComponentId",Off(vstruct.b,&vmi->m_pVehicleStruct->m_aUpgrades[15].m_nParentComponentId),0x2B0); }
      Chk("UpgradePosnDesc.m_qRotation",Off(up,&up->m_qRotation),0xC); Chk("UpgradePosnDesc.m_nParentComponentId",Off(up,&up->m_nParentComponentId),0x1C);
      Chk("sizeof(CVehicle)",uint32(sizeof(CVehicle)),uint32(kVehSize)); Chk("sizeof(CColModel)",uint32(sizeof(CColModel)),0x30); Chk("sizeof(tHydraulicData)",uint32(sizeof(tHydraulicData)),0x28);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){ Snap(0xA9B0C8+4*15,16); Snap(0x8CD760,0x14); Snap(0xA9B0B0,4); Snap(kGlobBase,kGlobSize); }

    // dobles instalados durante todo el banco (nadie mas corre entre casos)
    Patches P;
    // CVisibilityPlugins
    P.Add(0x732370,S_GetAtomicId);             P.Add(Dll(static_cast<int32(*)(RpAtomic*)>(&CVisibilityPlugins::GetAtomicId)),S_GetAtomicId);
    P.Add(0x732260,S_GetModelInfo);            P.Add(Dll(static_cast<CBaseModelInfo*(*)(RpAtomic*)>(&CVisibilityPlugins::GetModelInfo)),S_GetModelInfo);
    P.Add(0x732250,S_GetModelInfoIndex);       P.Add(Dll(&CVisibilityPlugins::GetModelInfoIndex),S_GetModelInfoIndex);
    P.Add(0x732A20,S_GetFrameHierarchyId);     P.Add(Dll(&CVisibilityPlugins::GetFrameHierarchyId),S_GetFrameHierarchyId);
    P.Add(0x732A00,S_SetFrameHierarchyId);     P.Add(Dll(&CVisibilityPlugins::SetFrameHierarchyId),S_SetFrameHierarchyId);
    P.Add(0x732290,S_SetAtomicId);             P.Add(Dll(&CVisibilityPlugins::SetAtomicId),S_SetAtomicId);
    P.Add(0x7322B0,S_SetAtomicFlag);           P.Add(Dll(&CVisibilityPlugins::SetAtomicFlag),S_SetAtomicFlag);
    P.Add(0x7328A0,S_SetAtomicRenderCallback); P.Add(Dll(&CVisibilityPlugins::SetAtomicRenderCallback),S_SetAtomicRenderCallback);
    P.Add(0x732B20,S_GetClumpAlpha);           P.Add(Dll(&CVisibilityPlugins::GetClumpAlpha),S_GetClumpAlpha);
    P.Add(0x732B00,S_SetClumpAlpha);           P.Add(Dll(&CVisibilityPlugins::SetClumpAlpha),S_SetClumpAlpha);
    // model infos
    P.Add(0x4C88F0,S_SetRenderPipelinesCB);    P.Add(Dll(&CVehicleModelInfo::SetRenderPipelinesCB),S_SetRenderPipelinesCB);
    P.Add(0x4C4BA0,S_AddRef);                  P.Add(Dll(&CBaseModelInfo::AddRef),S_AddRef);
    P.Add(0x4C4BB0,S_RemoveRef);               P.Add(Dll(&CBaseModelInfo::RemoveRef),S_RemoveRef);
    P.Add(0x4C53C0,S_GetFrameFromId);          P.Add(Dll(&CClumpModelInfo::GetFrameFromId),S_GetFrameFromId);
    P.Add(0x4C86B0,S_GetNumRemaps);            P.Add(Dll(&CVehicleModelInfo::GetNumRemaps),S_GetNumRemaps);
    P.Add(0x4C8970,S_GetPlateText);            P.Add(Dll(&CVehicleModelInfo::GetCustomCarPlateText),S_GetPlateText);
    P.Add(0x4C8980,S_SetPlateText);            P.Add(Dll(&CVehicleModelInfo::SetCustomCarPlateText),S_SetPlateText);
    P.Add(0x6FDEA0,S_CreatePlate);             P.Add(Dll(&CCustomCarPlateMgr::CreatePlateTexture),S_CreatePlate);
    P.Add(0x731A30,S_TxdRemoveRef);            P.Add(Dll(&CTxdStore::RemoveRef),S_TxdRemoveRef);
    // automovil, audio, entidad, colision, cuaternion
    P.Add(0x6A3EA0,S_Nitrous);                 P.Add(Dll(&CAutomobile::NitrousControl),S_Nitrous);
    P.Add(0x506FA0,S_Bass);                    P.Add(Dll(&CAudioEngine::SetRadioBassSetting),S_Bass);
    P.Add(0x571B70,S_RegRef);                  P.Add(Dll(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),S_RegRef);
    P.Add(0x4185A0,S_RemoveTrianglePlanes);    // CCollision::RemoveTrianglePlanes(CColModel*): el envoltorio de la DLL llama a esta direccion
    P.Add(0x40F810,S_AllocateData);            P.Add(Dll(static_cast<void(CColModel::*)()>(&CColModel::AllocateData)),S_AllocateData);
    P.Add(0x40F7C0,S_ColAssign);               P.Add(Dll(static_cast<CColModel&(CColModel::*)(const CColModel&)>(&CColModel::operator=)),S_ColAssign);
    P.Add(0x59C080,S_QuatGet);                 P.Add(Dll(static_cast<void(CQuaternion::*)(RwMatrix*) const>(&CQuaternion::Get)),S_QuatGet);
    P.Add(0x6E3290,S_AddVehicleUpgrade);       // CVehicle::AddVehicleUpgrade: el envoltorio de la DLL llama a esta direccion
    // RenderWare (los envoltorios de la DLL llaman a las direcciones del EXE)
    P.Add(0x74A4C0,S_ClumpRemoveAtomic); P.Add(0x749DC0,S_AtomicDestroy); P.Add(0x7F05A0,S_FrameDestroy); P.Add(0x749E60,S_AtomicClone); P.Add(0x74A490,S_ClumpAddAtomic);
    P.Add(0x74BF20,S_AtomicSetFrame); P.Add(0x7F0B00,S_FrameAddChild); P.Add(0x7F18A0,S_MatrixUpdate); P.Add(0x7F3820,S_TextureDestroy);
    P.Add(0x749B70,S_ClumpForAllAtomics); P.Add(0x7F1200,S_FrameForAllObjects); P.Add(0x7F0DC0,S_FrameForAllChildren); P.Add(0x74C790,S_GeometryForAllMaterials);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->Res {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC0E5u; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                RestoreRegions();
                const int32 idx=int32(15+r.below(4));
                BuildScene(r,idx); SetModelIndexGlobals(r);
                hooks[f]->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                json out={{"switches",switches}};
                void* VP=veh.data(); uint8* vb=veh.data();
                switch(f){
                case 0: { // ClearVehicleUpgradeFlags(arg0, modId)
                    const int32 modId=r.pickI({15,16,17,16,17,15,0,1,14,18,-1,255,0x10010,int32(r.below(40))});
                    At<uint32>(vb,0x38C)=(At<uint32>(vb,0x38C)&~0xA0000u)|(r.pct(60)?0x20000u:0)|(r.pct(50)?0x80000u:0);
                    At<uint8>(vb,0x1BE)=uint8(r.pickI({0,1,2,3,0x80,0xFF})); At<uint8>(vb,0x1D3)=uint8(r.pickI({0,0,0,1,2,3,0xFF}));
                    At<uint8>(vb,0x42E)=uint8((At<uint8>(vb,0x42E)&~0x10)|(r.pct(60)?0x10:0)); At<uint32>(vb,0x590)=uint32(r.pickI({0,0,0,1,2,5,9}));
                    At<int8>(vb,0x48B)=int8(r.pickI({-1,-1,0,1,2,3,-2,-128}));
                    for(int k=0;k<4;++k)AtA<uint32>(0xC1CC08+4*k)=r.pct(40) ? 0u : r.pct(50) ? uint32(uintptr_t(vb)) : r.next();
                    const int32 arg0=int32(r.next());
                    out["ret"]=CallThis<bool>(fn.address,VP,arg0,modId); break; }
                case 1: { // SetVehicleUpgradeFlags(upgradeModelIndex, modId, int& result)
                    const int32 modId=r.pickI({15,16,17,16,17,15,0,1,14,18,-1,255,0x10010,int32(r.below(40))});
                    const int32 upg=r.pickI({int32(AtA<uint16>(0x8CD764)),int32(AtA<uint16>(0x8CD768)),int32(AtA<uint16>(0x8CD760)),int32(r.below(20000)),int32(r.next())});
                    At<uint32>(vb,0x38C)=(At<uint32>(vb,0x38C)&~0xA0000u)|(r.pct(50)?0x20000u:0)|(r.pct(50)?0x80000u:0);
                    At<uint8>(vb,0x1BE)=uint8(r.pickI({0,1,2,3,0x80,0xFF})); At<uint8>(vb,0x1D3)=uint8(r.pickI({0,0,0,1,2,3,0xFF}));
                    At<uint8>(vb,0x42E)=uint8((At<uint8>(vb,0x42E)&~0x10)|(r.pct(50)?0x10:0)); At<uint32>(vb,0x590)=uint32(r.pickI({0,0,0,1,2,5,9}));
                    out["ret"]=CallThis<bool>(fn.address,VP,upg,modId,reinterpret_cast<int32*>(outInt.data())); out["result"]=outInt[0]; break; }
                case 2: { // GetUpgrade(upgradeId)
                    const int32 id=r.pickI({15,16,17,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,18,-1,int32(r.below(32))});
                    At<uint32>(vb,0x38C)=(At<uint32>(vb,0x38C)&~0xA0000u)|(r.pct(50)?0x20000u:0)|(r.pct(50)?0x80000u:0); At<uint8>(vb,0x42E)=uint8((At<uint8>(vb,0x42E)&~0x10)|(r.pct(50)?0x10:0));
                    out["ret"]=CallThis<int32>(fn.address,VP,id); break; }
                case 3: { // RemoveUpgrade(upgradeId)
                    const int32 id=r.pickI({15,16,17,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,18,-1,int32(r.below(24))});
                    CallThis<void>(fn.address,VP,id); break; }
                case 4: CallThis<void>(fn.address,VP); break;   // RemoveAllUpgrades
                case 5: { // GetReplacementUpgrade(nodeId)
                    const int32 id=int32(r.below(300))-5; out["ret"]=CallThis<int32>(fn.address,VP,id); break; }
                case 6: { // RemoveReplacementUpgrade(frameId)
                    CallThis<void>(fn.address,VP,int32(r.below(300))-5); break; }
                case 7: { // CreateUpgradeAtomic(mi, upgradePosn, parentComponent, isDamaged)
                    void* parent=frames[r.below(8)].b; const bool dmg=r.pct(50);
                    out["ret"]=Name(uintptr_t(CallThis<RpAtomic*>(fn.address,VP,static_cast<CBaseModelInfo*>(static_cast<void*>(miBuf[1].b)),static_cast<const UpgradePosnDesc*>(static_cast<void*>(posn.b)),static_cast<RwFrame*>(parent),dmg))); break; }
                case 8: { // CreateReplacementAtomic(mi, parentFrame, flags, isDamaged, isWheel)
                    void* parent=frames[r.below(8)].b; const bool dmg=r.pct(50), wheel=r.pct(50); const uint32 flags=r.pct(80) ? (r.next()&0xFFFF) : r.next();
                    out["ret"]=Name(uintptr_t(CallThis<RpAtomic*>(fn.address,VP,static_cast<CBaseModelInfo*>(static_cast<void*>(miBuf[1].b)),static_cast<RwFrame*>(parent),flags,dmg,wheel))); break; }
                case 9: { // CustomCarPlate_TextureCreate(model)
                    out["ret"]=CallThis<bool>(fn.address,VP,static_cast<CVehicleModelInfo*>(static_cast<void*>(miBuf[1].b))); break; }
                case 10: { // CustomCarPlate_TextureDestroy
                    At<void*>(vb,0x588)=r.pct(70) ? static_cast<void*>(tex[1].b) : nullptr; CallThis<void>(fn.address,VP); break; }
                case 11: { // GetRemapIndex
                    W.numRemaps=r.pickI({-2,0,1,2,3,4,4,5,8,40}); auto* rm=reinterpret_cast<int16*>(miBuf[0].b+0x2FA);
                    for(int k=0;k<4;++k)rm[k]=int16(r.pickI({-1,0,1,2,3,100,int32(r.below(60))}));
                    At<int16>(vb,0x598)=r.pct(60) ? rm[r.below(6)] : int16(r.next());
                    out["ret"]=CallThis<int32>(fn.address,VP); break; }
                case 12: { // SetRemap(remapIndex)
                    auto* rm=reinterpret_cast<int16*>(miBuf[0].b+0x2FA); for(int k=0;k<4;++k)rm[k]=int16(r.pickI({-1,0,1,2,3,100,int32(r.below(60))}));
                    At<int16>(vb,0x598)=r.pct(50) ? rm[r.below(4)] : int16(r.pickI({-1,-1,0,5,int32(r.next())}));
                    int32 ri; const uint32 pk=r.below(10);
                    if(pk<3)ri=-1; else if(pk<7)ri=int32(r.below(4)); else if(pk<9)ri=int32(r.below(601))-300; else ri=int32((r.next()&0xFFFF0000u)|(r.next()%300));
                    CallThis<void>(fn.address,VP,ri); break; }
                case 13: { // GetVehicleAppearance
                    const uint32 pk=r.below(8); uint32 mf=r.next()&0x00FFFFFFu; const uint32 single=uint32(r.pickI({0,0x1000000,0x2000000,0x4000000,0x8000000}));
                    mf|= pk<4 ? single : (r.below(16)<<24); mf|= (r.next()&0xF0000000u);
                    At<uint32>(hnd.data(),0xCC)=mf; out["ret"]=CallThis<int32>(fn.address,VP); break; }
                case 14: { // SetComponentVisibility(component, state)
                    RwFrame* c=r.pct(10) ? nullptr : reinterpret_cast<RwFrame*>(frames[r.below(8)].b); CallThis<void>(fn.address,VP,c,uint32(r.below(3))); break; }
                case 15: { // SetComponentAtomicAlpha(atomic, alpha) (estatica)
                    RpAtomic* a=reinterpret_cast<RpAtomic*>(atoms[r.below(6)].b); CallC<void>(fn.address,a,int32(Alpha(r))); break; }
                case 16: CallThis<void>(fn.address,VP); break;   // UpdateClumpAlpha
                case 17: { // SetVehicleAtomicVisibilityCB (Object)
                    RwObject* o=reinterpret_cast<RwObject*>(atoms[r.below(6)].b); void* d=reinterpret_cast<void*>(uintptr_t(uint32(r.pickI({0,1,2,3,4,0x10000,-1,0x7FFFFFFF,int32(r.below(8))}))));
                    out["ret"]=Name(uintptr_t(CallC<RwObject*>(fn.address,o,d))); break; }
                case 18: { // SetVehicleAtomicVisibilityCB (Frame)
                    RwFrame* fr=reinterpret_cast<RwFrame*>(frames[r.below(8)].b); void* d=reinterpret_cast<void*>(uintptr_t(uint32(r.pickI({0,1,2,3,4,0x10000,-1,int32(r.below(8))}))));
                    out["ret"]=Name(uintptr_t(CallC<RwFrame*>(fn.address,fr,d))); break; }
                case 19: { // RemoveObjectsCB (Object): atomic (tipo 1) u otro tipo de objeto
                    const int a=int(r.below(6)); if(r.pct(15))atoms[a].b[0]=uint8(r.pickI({0,2,3,0xFF,int32(r.below(256))}));
                    out["ret"]=Name(uintptr_t(CallC<RwObject*>(fn.address,reinterpret_cast<RwObject*>(atoms[a].b),static_cast<void*>(outInt.data())))); break; }
                case 20: { // RemoveObjectsCB (Frame)
                    out["ret"]=Name(uintptr_t(CallC<RwFrame*>(fn.address,reinterpret_cast<RwFrame*>(frames[r.below(8)].b),static_cast<void*>(outInt.data())))); break; }
                case 21: { // CopyObjectsCB(object, frame)
                    const int a=int(r.below(6)); if(r.pct(15))atoms[a].b[0]=uint8(r.pickI({0,2,3,0xFF}));
                    out["ret"]=Name(uintptr_t(CallC<RwObject*>(fn.address,reinterpret_cast<RwObject*>(atoms[a].b),static_cast<void*>(frames[r.below(8)].b)))); break; }
                case 22: { // RemoveAllUpgradesCB(atomic, data)
                    out["ret"]=Name(uintptr_t(CallC<RpAtomic*>(fn.address,reinterpret_cast<RpAtomic*>(atoms[r.below(6)].b),static_cast<void*>(nullptr)))); break; }
                case 23: { // SetupUpgradesAfterLoad
                    auto* up=reinterpret_cast<int16*>(vb+0x43A); for(int k=0;k<15;++k)up[k]=r.pct(45) ? int16(-1) : int16(r.pickI({0,400,401,1000,19999,-2,int32(int16(r.next()))}));
                    CallThis<void>(fn.address,VP); break; }
                case 24: { // GetSpecialColModel
                    const int32 pk=int32(r.below(10)); const int8 sidx=int8(pk<2 ? -1 : pk<7 ? int32(r.below(4)) : pk<8 ? -128 : int32(r.below(124))+4);
                    At<int8>(vb,0x48B)=sidx;
                    for(int k=0;k<4;++k)AtA<uint32>(0xC1CC08+4*k)=r.pct(40) ? 0u : r.pct(10) ? uint32(uintptr_t(vb)) : (0x1000u+r.below(0x1000)*4);
                    if(sidx>=0&&sidx<4&&r.pct(45))AtA<uint32>(0xC1CC08+4*sidx)=uint32(uintptr_t(vb));
                    for(int k=0;k<0xC0;++k)AtA<uint8>(0xC1CC78+k)=uint8(r.next());
                    for(int k=0;k<4;++k)AtA<uint32>(0xC1CC78+0x30*k+0x2C)=r.pct(40) ? 0u : (0x100000u+r.below(0x1000)*16);
                    for(int k=0;k<0xA0;++k)AtA<uint8>(0xC1CB60+k)=uint8(r.next());
                    At<uint32>(vb,0x40)=r.next();
                    out["ret"]=CallThis<bool>(fn.address,VP); break; }
                case 25: { // SetTransmissionRotation(component, angleL, angleR, wheelPos, isFront)
                    RwFrame* c=r.pct(8) ? nullptr : reinterpret_cast<RwFrame*>(frames[r.below(8)].b);
                    const bool big=r.pct(10); const float s=big ? 50.f : 3.f;
                    const float aL=r.pct(8)?0.f:r.uni(-s,s), aR=r.pct(8)?0.f:r.uni(-s,s), wx=r.pct(8)?0.f:r.uni(-s,s), wz=r.uni(-s,s);
                    const float wy=(c&&r.pct(8)) ? At<float>(c,0x44) : r.uni(-s,s);       // wheelPos.y igual a la Y del marco (division/atan2 con 0)
                    CallThis<void>(fn.address,VP,c,aL,aR,wx,wy,wz,uint8(r.pct(50))); break; }
                }
                out["veh"]=Fnv(vb,kVehSize); out["guard"]=Fnv(vb+kVehSize,kGuard);
                out["frames"]=Fnv(frames.data(),sizeof(frames)); out["atoms"]=Fnv(atoms.data(),sizeof(atoms)); out["geos"]=Fnv(geos.data(),sizeof(geos)); out["mats"]=Fnv(mats.data(),sizeof(mats));
                out["tex"]=Fnv(tex.data(),sizeof(tex)); out["mi"]=Fnv(miBuf.data(),sizeof(miBuf)); out["miA"]=Fnv(miA.data(),sizeof(miA)); out["clump"]=Fnv(clump.data(),sizeof(clump));
                out["vstruct"]=Fnv(vstruct.b,sizeof(vstruct)); out["out"]=Fnv(outInt.data(),sizeof(outInt)); out["world"]=Fnv(&W,sizeof(W));
                out["globals"]=Fnv(reinterpret_cast<void*>(kGlobBase),kGlobSize); out["damagedFlag"]=AtA<uint8>(0xA9B0B0);
                out["trace"]=trace;
                Res res; res.j=std::move(out); res.vehBytes=ReversingProbe::Bytes(vb,kVehSize); return res;
            };
            const auto original=run(false), candidate=run(true);
            RestoreRegions();
            const bool ok=original.j==candidate.j&&original.j["switches"]==true&&candidate.j["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original.j)},{"candidate",Summary(candidate.j)},{"events",original.j["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.j.items())if(candidate.j[k]!=v)diff.push_back(k);
                json vd=json::array(); for(size_t k=0;k<kVehSize&&vd.size()<24;++k)if(original.vehBytes[k]!=candidate.vehBytes[k])vd.push_back({k,original.vehBytes[k],candidate.vehBytes[k]});
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"veh_diff",vd},
                       {"original",failures<=2 ? original.j:json(nullptr)},{"candidate",failures<=2 ? candidate.j:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",RegionsRestored()}});
}
}
