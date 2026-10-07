#pragma once
// Banco dentro del motor: CEntity, grupo B (ciclo de vida, referencias, estado y colision; 28 funciones):
// Constructor, Destructor, CleanUpOldReference, RegisterReference, ResolveReferences, PruneReferences, IsEntityPointerValid,
// FlagToDestroyWhenNextProcessed, SetModelIndex, SetModelIndexNoCreate, CreateRwObject, DeleteRwObject, SetIsStatic, GetIsStatic,
// GetIsTouching (entidad y esfera), GetIsTypePhysical, DoesNotCollideWithFlyers, TreatAsPlayerForCollisions, ProcessCollision,
// ProcessControl, ProcessShift, TestCollision, Teleport, SpecialEntityCalcCollisionSteps, SpecialEntityPreCollisionStuff,
// FindTriggerPointCoors y SetupBigBuilding.
// Original (hooks del grupo desactivados) frente a candidato con fixtures identicos sobre entidades FALSAS: buferes del tamano
// de CEntity (mas colchon hasta 0x1C0 para los campos de CPhysical/CObject que lee el codigo) rellenados con 0xCD (constructor)
// o con bytes deterministas; nunca entidades reales ni pools reales. Las virtuales se llaman por la vtable del EXE de CEntity
// (0x863928, huecos comprobados volcando la vtable: 4 SetIsStatic, 5 SetModelIndex, 6 SetModelIndexNoCreate, 7 CreateRwObject,
// 8 DeleteRwObject, 10 ProcessControl, 11 ProcessCollision, 12 ProcessShift, 13 TestCollision, 14 Teleport, 15 SpecialEntityPre
// CollisionStuff, 16 SpecialEntityCalcCollisionSteps, 21 FlagToDestroyWhenNextProcessed). Las llamadas virtuales que hace el
// codigo probado van por una vtable FALSA de la entidad y otra de los model infos, con trazas y trampas (`*_inesperada`).
// Se simulan y trazan (direccion del EXE + simbolo/cuerpo de la DLL, sin reservas de memoria dentro de los stubs):
//   rand/GetRandomNumber, CEntity::HasPreRenderEffects/CreateEffects/DestroyEffects/RemoveEscalatorsForEntity/DeleteRwObject/
//   ResolveReferences, CPlaceable::RemoveMatrix/AllocateMatrix, CSimpleTransform::UpdateRwMatrix/UpdateMatrix,
//   CMatrix::UpdateRwMatrix, CBaseModelInfo::AddRef/RemoveRef/Get2dEffect, CTagManager::AddTag/IsTag/ResetAlpha,
//   CCustomBuildingDNPipeline::PreRenderUpdate, CCustomBuildingRenderer::IsCBPCPipelineAttached, CStreaming::AddEntity/
//   RemoveEntity, RpAtomicDestroy/RwFrameDestroy/RpClumpDestroy/RpClumpForAllAtomics/RpSkinGeometryGetSkin/GetFirstAtomic,
//   RpAnimBlendClumpGetFirstAssociation, CAnimBlendAssociation::SetCurrentTime, CPhysical::AddToMovingList, la lista de
//   cuerpos en movimiento (AddItem/DeleteItem de CWorld::ms_listMovingEntityPtrs), CEntity::TransformFromObjectSpace y las cinco
//   IsXPointerValid. En las referencias (RegisterReference y compania) se ejecuta de verdad todo el codigo sobre nodos CReference
//   propios (lista de libres falsa) y pools falsos de peatones, vehiculos y objetos (los punteros reales 0xB74490/94/9C solo
//   se cambian durante la llamada). El constructor y el destructor ejecutan de verdad CPlaceable (EXE y DLL).
// Se guardan y restauran: model infos 19990..19992, gBuildings, ms_bCreateDamagedVersion, numMatrices, CReferences::pEmptyList,
// los cuatro punteros de pools de entidades y la cabeza de la lista de cuerpos en movimiento.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "TagManager.h"
#include "CustomBuildingDNPipeline.h"
#include "CustomBuildingRenderer.h"
#include "../ProbeRuntime.hpp"

namespace CEntBLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; } };
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

// --- modulo de la DLL y busqueda de cuerpos (como CPedD) ---
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

// ======================= objetos falsos =======================
constexpr size_t EN=0x1C0;                                  // bufer de cada entidad falsa (CEntity mide 0x38; el resto cubre CPhysical/CObject)
constexpr size_t NN=64, NC=12, PN=4, VN=3, ON=3, NFX=4;     // nodos CReference, celdas de puntero, huecos de pools, efectos 2d
constexpr size_t PED_STRIDE=0x7C4, VEH_STRIDE=0xA18, OBJ_STRIDE=0x19C;
constexpr int MI0=19990, MI1=19991, MI2=19992;
constexpr uintptr_t G_VT=0x863928, G_PEDPOOL=0xB74490, G_VEHPOOL=0xB74494, G_BLDPOOL=0xB74498, G_OBJPOOL=0xB7449C,
                    G_BUILDINGS=0xB71804, G_DAMAGED=0xA9B0B0, G_NUMMAT=0xB74238, G_EMPTYREF=0xBA1768, G_MOVLIST=0xB9ACC8, G_MIPTRS=0xA9B0C8;
// desplazamientos crudos del EXE (CEntity)
constexpr size_t E_MATRIX=0x14, E_RW=0x18, E_FLAGS=0x1C, E_SEED=0x20, E_MODEL=0x22, E_REFS=0x24, E_LINK=0x28, E_LOD=0x30, E_NLC=0x34, E_INFO=0x36, E_MOVING=0xB4;
alignas(16) inline std::array<uint8,EN> entA{}, entB{}, entL{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> mis{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> cols{};
alignas(16) inline std::array<std::array<uint8,0x40>,NFX> fx{};
alignas(16) inline std::array<uint8,0x40> amiBuf{}, atomF{}, atomF2{}, clumpF{}, lodAtomF{}, lodClumpF{}, geomF{}, skinF{}, assocOwn{}, assocLod{}, linkA{}, linkNew{}, movNode{}, nodeNewBuf{}, outBuf{}, boolBuf{};
alignas(16) inline std::array<uint8,0x60> frameA{}, frameC{}, frameL{}, matA{};
inline std::array<uint32,NN*2> nodes{};
inline std::array<void*,NC> cells{};
alignas(16) inline std::array<uint8,PN*PED_STRIDE> pedSt{}; alignas(16) inline std::array<uint8,PN> pedStates{}; alignas(16) inline std::array<uint8,0x20> pedHdr{};
alignas(16) inline std::array<uint8,VN*VEH_STRIDE> vehSt{}; alignas(16) inline std::array<uint8,VN> vehStates{}; alignas(16) inline std::array<uint8,0x20> vehHdr{};
alignas(16) inline std::array<uint8,ON*OBJ_STRIDE> objSt{}; alignas(16) inline std::array<uint8,ON> objStates{}; alignas(16) inline std::array<uint8,0x20> objHdr{};
inline std::array<void*,24> entVt{}; inline std::array<void*,32> miVt{};

struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v; }
inline json Name(const void* p){ return Name(uintptr_t(p)); }

// --- registro de stubs sin reservas de memoria ni E/S ---
struct I { int32 v; }; struct U { uint32 v; }; struct Fl { float v; }; struct Pt { const void* p; }; struct Bo { bool v; };
struct Arg { uint8 kind; uint32 v; };
struct Rec { const char* tag; uint32 n; std::array<Arg,16> a; };
inline std::array<Rec,512> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Put(Rec& r,uint8 k,uint32 v){ if(r.n<r.a.size())r.a[r.n++]={k,v}; }
inline void Add(Rec& r,I x){ Put(r,0,uint32(x.v)); }
inline void Add(Rec& r,U x){ Put(r,1,x.v); }
inline void Add(Rec& r,Fl x){ Put(r,2,std::bit_cast<uint32>(x.v)); }
inline void Add(Rec& r,Pt x){ Put(r,3,uint32(uintptr_t(x.p))); }
inline void Add(Rec& r,Bo x){ Put(r,4,x.v ? 1u : 0u); }
template<class... A> void T(const char* tag,A... a){
    if(!Main())return; if(nrec>=recs.size()){ recOverflow=true; return; }
    Rec& r=recs[nrec++]; r.tag=tag; r.n=0; (Add(r,a),...); }
inline json RecsToJson(){
    json out=json::array();
    for(size_t i=0;i<nrec;++i){ const Rec& r=recs[i]; json j=json::array(); j.push_back(r.tag);
        for(uint32 k=0;k<r.n;++k){ const Arg& a=r.a[k];
            switch(a.kind){ case 0: j.push_back(int32(a.v)); break; case 1: case 2: j.push_back(a.v); break; case 3: j.push_back(Name(uintptr_t(a.v))); break;
                case 4: j.push_back(a.v!=0); break; default: j.push_back(a.v); } }
        out.push_back(std::move(j)); }
    if(recOverflow)out.push_back("overflow");
    return out; }

// --- estado de los dobles (se fija por caso) ---
inline uint32 randIdx{}; inline std::array<uint32,16> randTab{};
inline bool hasPreRet{}, tagRet{}, cbpcRet{}; inline std::array<bool,5> validRet{};
inline void *amiRet{}, *createRet{}, *firstAtomicRet{}, *skinRet{}, *assocLodRet{}, *assocOwnRet{};
inline uint32 miTypeRet{};
inline std::array<float,3> tfA{}, tfB{};
inline uintptr_t cbDll{};

// --- dobles ---
inline int __cdecl S_Rand(){ T("rand"); return int(randTab[(randIdx++)&15]); }
inline uint16 S_RandNum(){ return uint16(S_Rand()); }
// destructor
inline void __fastcall S_DelRw(void* e,void*){ T("CEntity::DeleteRwObject",Pt{e}); }
inline void __fastcall S_Resolve(void* e,void*){ T("CEntity::ResolveReferences",Pt{e}); }
inline void __fastcall S_RemoveMatrix(void* e,void*){ T("CPlaceable::RemoveMatrix",Pt{e}); }
// SetModelIndexNoCreate
inline bool __fastcall S_HasPre(void* e,void*){ T("CEntity::HasPreRenderEffects",Pt{e}); return hasPreRet; }
inline void __cdecl S_AddTag(void* e){ T("CTagManager::AddTag",Pt{e}); }
// CreateRwObject / DeleteRwObject
inline void __fastcall S_AddRef(void* m,void*){ T("CBaseModelInfo::AddRef",Pt{m}); }
inline void __fastcall S_RemoveRef(void* m,void*){ T("CBaseModelInfo::RemoveRef",Pt{m}); }
inline void __fastcall S_MatUpdateRw(void* m,void*,void* rw){ T("CMatrix::UpdateRwMatrix",Pt{m},Pt{rw}); }
inline void __fastcall S_SimpleUpdateRw(void* s,void*,void* rw){ T("CSimpleTransform::UpdateRwMatrix",Pt{s},Pt{rw}); }
inline bool __cdecl S_IsTag(void* e){ T("CTagManager::IsTag",Pt{e},Bo{tagRet}); return tagRet; }
inline void __cdecl S_ResetAlpha(void* e){ T("CTagManager::ResetAlpha",Pt{e}); }
inline void __cdecl S_DnPre(void* a,uint32 b){ T("CCustomBuildingDNPipeline::PreRenderUpdate",Pt{a},U{b&0xFFu}); }
inline bool __cdecl S_Cbpc(void* a){ T("CCustomBuildingRenderer::IsCBPCPipelineAttached",Pt{a},Bo{cbpcRet}); return cbpcRet; }
inline void* __cdecl S_FirstAtomic(void* c){ T("GetFirstAtomic",Pt{c}); return firstAtomicRet; }
inline void* __cdecl S_FirstAssoc(void* c){ T("RpAnimBlendClumpGetFirstAssociation",Pt{c}); return c==lodClumpF.data() ? assocLodRet : assocOwnRet; }
inline void __fastcall S_SetCurTime(void* a,void*,float t){ T("CAnimBlendAssociation::SetCurrentTime",Pt{a},Fl{t}); }
inline void* __cdecl S_StAdd(void* e){ T("CStreaming::AddEntity",Pt{e}); return linkNew.data(); }
inline void __cdecl S_StRemove(void* l){ T("CStreaming::RemoveEntity",Pt{l}); }
inline void __fastcall S_CreateFx(void* e,void*){ T("CEntity::CreateEffects",Pt{e}); }
inline void __fastcall S_DestroyFx(void* e,void*){ T("CEntity::DestroyEffects",Pt{e}); }
inline void __fastcall S_RemoveEsc(void* e,void*){ T("CEntity::RemoveEscalatorsForEntity",Pt{e}); }
inline int32 __cdecl S_AtomicDestroy(void* a){ T("RpAtomicDestroy",Pt{a}); return 1; }
inline int32 __cdecl S_FrameDestroy(void* f){ T("RwFrameDestroy",Pt{f}); return 1; }
inline int32 __cdecl S_ClumpDestroy(void* c){ T("RpClumpDestroy",Pt{c}); return 1; }
inline void* __cdecl S_Skin(void* g){ T("RpSkinGeometryGetSkin",Pt{g}); return skinRet; }
inline void* __cdecl S_ForAll(void* c,void* cb,void* d){ const uintptr_t k=uintptr_t(cb); const bool known=k==0x734B90||k==cbDll||FollowIn(k)==cbDll; T("RpClumpForAllAtomics",Pt{c},Bo{known},Pt{d}); return c; }
inline void __fastcall S_AddMoving(void* e,void*){ T("CPhysical::AddToMovingList",Pt{e}); At<void*>(e,E_MOVING)=movNode.data(); }
inline void* __cdecl S_NodeNew(uint32){ return nodeNewBuf.data(); }                       // reserva del nodo del EXE (sin traza ni estado comparado)
inline void __fastcall S_ExeListAdd(void* node,void*,void* list){ T("MovingList.AddItem",Pt{list},Pt{*reinterpret_cast<void**>(node)}); }
inline void* __fastcall S_DllListAdd(void* list,void*,void* item){ T("MovingList.AddItem",Pt{list},Pt{item}); return nullptr; }
inline void* __fastcall S_ListDelete(void* list,void*,void* item){ T("MovingList.DeleteItem",Pt{list},Pt{item}); return nullptr; }
// GetIsTouching / FindTriggerPointCoors
inline void* __fastcall S_Tfos(void* self,void*,void* out,const float* off){
    T("CEntity::TransformFromObjectSpace",Pt{self},U{F(off[0])},U{F(off[1])},U{F(off[2])});
    const auto& d=self==entA.data() ? tfA : tfB; float* o=static_cast<float*>(out); o[0]=off[0]+d[0]; o[1]=off[1]+d[1]; o[2]=off[2]+d[2]; return out; }
inline void* __fastcall S_Get2dFx(void* m,void*,int32 i){ T("CBaseModelInfo::Get2dEffect",Pt{m},I{i}); return i>=0&&i<int32(NFX) ? fx[size_t(i)].data() : nullptr; }
inline void __fastcall S_AllocMatrix(void* e,void*){ T("CPlaceable::AllocateMatrix",Pt{e}); At<void*>(e,E_MATRIX)=matA.data(); }
inline void __fastcall S_SimpleUpdateMatrix(void* s,void*,void* m){ T("CSimpleTransform::UpdateMatrix",Pt{s},Pt{m}); }
// IsEntityPointerValid
template<int K> bool __cdecl S_Valid(void* p){ T("IsPointerValid",I{K},Pt{p},Bo{validRet[K]}); return validRet[K]; }
// vtable falsa de la entidad (huecos 4, 6 y 7 con traza; el resto trampas) y de los model infos
template<int N> void __fastcall EntTrap(void* e,void*){ T("ent_virtual_inesperada",I{N},Pt{e}); }
template<int... N> void FillEntTraps(std::integer_sequence<int,N...>) { ((entVt[N]=reinterpret_cast<void*>(&EntTrap<N>)),...); }
inline void __fastcall EV4(void* e,void*,uint32 b){ T("ent.SetIsStatic",Pt{e},U{b&0xFFu}); }
inline void __fastcall EV6(void* e,void*,uint32 idx){ T("ent.SetModelIndexNoCreate",Pt{e},U{idx}); }
inline void __fastcall EV7(void* e,void*){ T("ent.CreateRwObject",Pt{e}); }
template<int N> void* __fastcall MiTrap(void* m,void*){ T("mi_virtual_inesperada",I{N},Pt{m}); return nullptr; }
template<int... N> void FillMiTraps(std::integer_sequence<int,N...>) { ((miVt[N]=reinterpret_cast<void*>(&MiTrap<N>)),...); }
inline void* __fastcall MiAsAtomic(void*,void*){ return amiRet; }                 // getter puro, sin traza (el EXE lo llama hasta tres veces)
inline uint32 __fastcall MiGetType(void* m,void*){ T("mi.GetModelType",Pt{m}); return miTypeRet; }
inline void* __fastcall MiCreateInst(void* m,void*){ T("mi.CreateInstance",Pt{m},U{*reinterpret_cast<uint8*>(G_DAMAGED)}); return createRet; }

// ======================= fixtures =======================
inline void RandFill(uint8* p,size_t n,Rng& r){ for(size_t k=0;k<n;++k)p[k]=uint8(r.next()>>5); }
inline void FillFloats(void* p,size_t n,Rng& r){ for(size_t k=0;k<n;++k)At<float>(p,k*4)=r.next()%5==0 ? 0.f : r.uni(-40.f,40.f); }
inline float Mixed(Rng& r){ static const float sc[]{0.01f,0.1f,1.f,10.f,100.f,1000.f}; return r.uni(-1.f,1.f)*sc[r.next()%std::size(sc)]; }
inline float NextUlp(float v,int n){ for(;n>0;--n)v=std::nextafter(v,std::numeric_limits<float>::infinity()); for(;n<0;++n)v=std::nextafter(v,-std::numeric_limits<float>::infinity()); return v; }
inline uint8* Mi(size_t k){ return mis[k].data(); }

struct Tracked { const void* p; size_t n; const char* name; };
inline std::vector<Tracked> tracked;
inline void InitTables(){
    tracked={{entB.data(),EN,"entB"},{entL.data(),EN,"entL"},{mis[0].data(),0x40,"mi0"},{mis[1].data(),0x40,"mi1"},{mis[2].data(),0x40,"mi2"},{cols[0].data(),0x40,"col0"},{cols[1].data(),0x40,"col1"},
        {fx[0].data(),0x40,"fx0"},{fx[1].data(),0x40,"fx1"},{fx[2].data(),0x40,"fx2"},{fx[3].data(),0x40,"fx3"},{amiBuf.data(),0x40,"ami"},{atomF.data(),0x40,"atom"},{atomF2.data(),0x40,"atom2"},
        {clumpF.data(),0x40,"clump"},{lodAtomF.data(),0x40,"lodAtom"},{lodClumpF.data(),0x40,"lodClump"},{geomF.data(),0x40,"geom"},{skinF.data(),0x40,"skin"},{assocOwn.data(),0x40,"assocOwn"},
        {assocLod.data(),0x40,"assocLod"},{linkA.data(),0x40,"linkA"},{linkNew.data(),0x40,"linkNew"},{movNode.data(),0x40,"movNode"},{outBuf.data(),0x40,"out"},{boolBuf.data(),0x40,"bools"},
        {frameA.data(),0x60,"frameA"},{frameC.data(),0x60,"frameC"},{frameL.data(),0x60,"frameL"},{matA.data(),0x60,"matA"},{nodes.data(),sizeof(nodes),"nodes"},{cells.data(),sizeof(cells),"cells"},
        {pedSt.data(),pedSt.size(),"pedSt"},{pedStates.data(),pedStates.size(),"pedStates"},{pedHdr.data(),pedHdr.size(),"pedHdr"},{vehSt.data(),vehSt.size(),"vehSt"},{vehStates.data(),vehStates.size(),"vehStates"},
        {vehHdr.data(),vehHdr.size(),"vehHdr"},{objSt.data(),objSt.size(),"objSt"},{objStates.data(),objStates.size(),"objStates"},{objHdr.data(),objHdr.size(),"objHdr"}};
    named={{entA.data(),EN,"entA"},{entB.data(),EN,"entB"},{entL.data(),EN,"entL"},{mis[0].data(),0x40,"mi0"},{mis[1].data(),0x40,"mi1"},{mis[2].data(),0x40,"mi2"},{cols[0].data(),0x40,"col0"},{cols[1].data(),0x40,"col1"},
        {fx[0].data(),0x40,"fx0"},{fx[1].data(),0x40,"fx1"},{fx[2].data(),0x40,"fx2"},{fx[3].data(),0x40,"fx3"},{amiBuf.data(),0x40,"ami"},{atomF.data(),0x40,"atom"},{atomF2.data(),0x40,"atom2"},
        {clumpF.data(),0x40,"clump"},{lodAtomF.data(),0x40,"lodAtom"},{lodClumpF.data(),0x40,"lodClump"},{geomF.data(),0x40,"geom"},{skinF.data(),0x40,"skin"},{assocOwn.data(),0x40,"assocOwn"},
        {assocLod.data(),0x40,"assocLod"},{linkA.data(),0x40,"linkA"},{linkNew.data(),0x40,"linkNew"},{movNode.data(),0x40,"movNode"},{nodeNewBuf.data(),0x40,"nodeNew"},{outBuf.data(),0x40,"out"},{boolBuf.data(),0x40,"bools"},
        {frameA.data(),0x60,"frameA"},{frameC.data(),0x60,"frameC"},{frameL.data(),0x60,"frameL"},{matA.data(),0x60,"matA"},{nodes.data(),sizeof(nodes),"nodes"},{cells.data(),sizeof(cells),"cells"},
        {pedSt.data(),pedSt.size(),"pedSt"},{vehSt.data(),vehSt.size(),"vehSt"},{objSt.data(),objSt.size(),"objSt"},{entVt.data(),sizeof(entVt),"ent_vt"},{miVt.data(),sizeof(miVt),"mi_vt"},
        {reinterpret_cast<void*>(G_MOVLIST),4,"moving_list"}};
}

// nodos CReference, celdas y pools falsos: listas por entidad, lista de libres y entidades de pool
struct RefPlan { void* argCell; };
inline uint32 BuildList(Rng& r,size_t& nn,uint32 cnt,bool freeList){
    uint32 head=0;
    for(uint32 j=0;j<cnt&&nn<NN;++j){ uint32* n=&nodes[(nn++)*2]; n[0]=head; n[1]=freeList ? 0u : uint32(uintptr_t(&cells[r.next()%NC])); head=uint32(uintptr_t(n)); }
    return head; }
inline uint8* PoolEnt(size_t kind,size_t k){ return kind==0 ? pedSt.data()+k*PED_STRIDE : kind==1 ? vehSt.data()+k*VEH_STRIDE : objSt.data()+k*OBJ_STRIDE; }
inline void PoolHeader(std::array<uint8,0x20>& h,void* st,void* states,size_t n){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=st; At<void*>(h.data(),4)=states; At<int32>(h.data(),8)=int32(n); At<int32>(h.data(),0xC)=-1; }
inline RefPlan BuildRefs(Rng& r,bool withFreeList,bool pools){
    for(auto& w:nodes)w=0xCDCDCDCDu;
    size_t nn=0;
    // celdas: la entidad probada, otra, entidades de pool, nulo o basura
    for(size_t k=0;k<NC;++k){
        const uint32 m=r.next()%20; void* v;
        if(m<8)v=entA.data(); else if(m<11)v=entB.data(); else if(m<15&&pools)v=PoolEnt(r.next()%3,r.next()%2==0 ? 0 : 1); else if(m<17)v=nullptr; else v=reinterpret_cast<void*>(0x5000u+uint32(k)*16u);
        cells[k]=v; }
    At<uint32>(entA.data(),E_REFS)=BuildList(r,nn,r.next()%9,false);
    At<uint32>(entB.data(),E_REFS)=BuildList(r,nn,r.next()%3,false);
    At<uint32>(reinterpret_cast<void*>(G_EMPTYREF),0)=(withFreeList&&r.next()%2) ? BuildList(r,nn,1+r.next()%6,true) : 0u;
    if(pools){
        for(size_t k=0;k<PN;++k){ RandFill(PoolEnt(0,k),PED_STRIDE,r); At<uint32>(PoolEnt(0,k),E_REFS)=BuildList(r,nn,r.next()%4,false); pedStates[k]=uint8(r.next()%3 ? (r.next()%128) : (0x80|(r.next()%128))); }
        for(size_t k=0;k<VN;++k){ RandFill(PoolEnt(1,k),VEH_STRIDE,r); At<uint32>(PoolEnt(1,k),E_REFS)=BuildList(r,nn,r.next()%4,false); vehStates[k]=uint8(r.next()%3 ? (r.next()%128) : (0x80|(r.next()%128))); }
        for(size_t k=0;k<ON;++k){ RandFill(PoolEnt(2,k),OBJ_STRIDE,r); At<uint32>(PoolEnt(2,k),E_REFS)=BuildList(r,nn,r.next()%4,false); objStates[k]=uint8(r.next()%3 ? (r.next()%128) : (0x80|(r.next()%128))); }
    }
    PoolHeader(pedHdr,pedSt.data(),pedStates.data(),PN); PoolHeader(vehHdr,vehSt.data(),vehStates.data(),VN); PoolHeader(objHdr,objSt.data(),objStates.data(),ON);
    // cualquier nodo que el recorrido de pools no pueda ver sigue siendo basura 0xCD; elegimos una celda de argumento
    return RefPlan{ &cells[r.next()%NC] };
}

// fixture comun
inline void Reset(Rng& r){
    nrec=0; recOverflow=false; randIdx=0;
    for(auto* p:{&entA,&entB,&entL})RandFill(p->data(),EN,r);
    for(auto& m:mis)RandFill(m.data(),0x40,r);
    for(auto& c:cols){ std::memset(c.data(),0,0x40); FillFloats(c.data(),9,r); At<float>(c.data(),0x24)=r.uni(0.1f,30.f); }
    for(auto& e:fx)RandFill(e.data(),0x40,r);
    for(auto* p:{&amiBuf,&atomF,&atomF2,&clumpF,&lodAtomF,&lodClumpF,&geomF,&skinF,&assocOwn,&assocLod,&linkA,&linkNew,&movNode,&nodeNewBuf,&outBuf,&boolBuf})RandFill(p->data(),0x40,r);
    for(auto* p:{&frameA,&frameC,&frameL,&matA})RandFill(p->data(),0x60,r);
    for(auto& w:nodes)w=0xCDCDCDCDu; for(auto& c:cells)c=nullptr;
    std::memset(pedSt.data(),0xCD,pedSt.size()); std::memset(vehSt.data(),0xCD,vehSt.size()); std::memset(objSt.data(),0xCD,objSt.size());
    std::memset(pedStates.data(),0x80,pedStates.size()); std::memset(vehStates.data(),0x80,vehStates.size()); std::memset(objStates.data(),0x80,objStates.size());
    std::memset(pedHdr.data(),0,pedHdr.size()); std::memset(vehHdr.data(),0,vehHdr.size()); std::memset(objHdr.data(),0,objHdr.size());
    for(auto* e:{&entA,&entB,&entL}){ At<void*>(e->data(),0)=entVt.data(); At<void*>(e->data(),E_MATRIX)=nullptr; At<void*>(e->data(),E_RW)=nullptr; At<void*>(e->data(),E_REFS)=nullptr; At<void*>(e->data(),E_LINK)=nullptr; At<void*>(e->data(),E_LOD)=nullptr; }
    // objetos de RenderWare falsos con el tipo correcto
    atomF[0]=1; atomF2[0]=1; lodAtomF[0]=1; clumpF[0]=2; lodClumpF[0]=2;
    At<void*>(atomF.data(),4)=frameA.data(); At<void*>(atomF2.data(),4)=frameA.data(); At<void*>(clumpF.data(),4)=frameC.data(); At<void*>(lodClumpF.data(),4)=frameL.data(); At<void*>(lodAtomF.data(),4)=frameL.data();
    At<void*>(atomF.data(),0x18)=geomF.data(); At<void*>(atomF2.data(),0x18)=geomF.data(); At<void*>(lodAtomF.data(),0x18)=geomF.data();
    // model infos falsos: vtable, colision, sin efectos 2d
    for(size_t k=0;k<mis.size();++k){ At<void*>(Mi(k),0)=miVt.data(); At<void*>(Mi(k),0x14)=cols[k%2].data(); At<uint8>(Mi(k),0xD)=0; CModelInfo::ms_modelInfoPtrs[MI0+int(k)]=reinterpret_cast<CBaseModelInfo*>(Mi(k)); }
    // globales
    *reinterpret_cast<int32*>(G_BUILDINGS)=int32(r.next()%500); *reinterpret_cast<uint8*>(G_DAMAGED)=uint8(r.next()%2);
    *reinterpret_cast<int32*>(G_NUMMAT)=int32(r.next()%900)+1; *reinterpret_cast<uint32*>(G_EMPTYREF)=0;
    for(auto& v:randTab){ static const uint32 sp[]{0,1,2,0x7FFF,0x3FFF,0x1234,0xFFFF,0x8000,0x10000,0x12345678}; v=r.next()%3 ? sp[r.next()%std::size(sp)] : r.next(); }
    hasPreRet=r.next()%2; tagRet=r.next()%2; cbpcRet=r.next()%2; for(auto& v:validRet)v=r.next()%2;
    amiRet=nullptr; createRet=nullptr; firstAtomicRet=nullptr; skinRet=nullptr; assocLodRet=nullptr; assocOwnRet=nullptr; miTypeRet=5;
    for(auto& v:tfA)v=r.uni(-200.f,200.f); for(auto& v:tfB)v=r.uni(-200.f,200.f);
}

inline json EntWords(const uint8* p){ json r=json::array(); for(size_t k=4;k<EN;k+=4)r.push_back(*reinterpret_cast<const uint32*>(p+k)); return r; }
inline json Globals(){
    return {*reinterpret_cast<int32*>(G_BUILDINGS),*reinterpret_cast<uint8*>(G_DAMAGED),*reinterpret_cast<int32*>(G_NUMMAT),Name(uintptr_t(*reinterpret_cast<uint32*>(G_EMPTYREF))),
            *reinterpret_cast<uint32*>(G_PEDPOOL),*reinterpret_cast<uint32*>(G_VEHPOOL),*reinterpret_cast<uint32*>(G_BLDPOOL),*reinterpret_cast<uint32*>(G_OBJPOOL),*reinterpret_cast<uint32*>(G_MOVLIST)}; }

// llamada por un thiscall generico: devuelve eax
using Th0=uint32(__thiscall*)(void*);

// ======================= ejecucion de un caso =======================
inline json Run(const std::string& nm,uintptr_t target,Rng& r,int i,json& out,bool candidate){
    Reset(r);
    uint8* const a=entA.data(); json ret=nullptr; uint32 expectVptr=0;
    uint32* const gEmpty=reinterpret_cast<uint32*>(G_EMPTYREF);
    auto withPools=[&](auto&& fn){
        void* const s0=*reinterpret_cast<void**>(G_PEDPOOL); void* const s1=*reinterpret_cast<void**>(G_VEHPOOL); void* const s2=*reinterpret_cast<void**>(G_OBJPOOL);
        *reinterpret_cast<void**>(G_PEDPOOL)=pedHdr.data(); *reinterpret_cast<void**>(G_VEHPOOL)=vehHdr.data(); *reinterpret_cast<void**>(G_OBJPOOL)=objHdr.data();
        fn();
        *reinterpret_cast<void**>(G_PEDPOOL)=s0; *reinterpret_cast<void**>(G_VEHPOOL)=s1; *reinterpret_cast<void**>(G_OBJPOOL)=s2; };
    auto TypeByte=[&](uint32 type){ a[E_INFO]=uint8(type|((r.next()%32)<<3)); };
    const auto Idx=[&]()->int{ return MI0+int(r.next()%3); };
    if(nm=="Constructor"){
        expectVptr=0x863928u; std::memset(a,0xCD,EN);
        auto* p=reinterpret_cast<void*(__thiscall*)(void*)>(target)(a); ret=Name(p); }
    else if(nm=="Destructor"){
        expectVptr=0x863C40u;
        At<void*>(a,E_MATRIX)=r.next()%2 ? static_cast<void*>(matA.data()) : nullptr;
        At<void*>(a,E_LOD)=r.next()%3 ? static_cast<void*>(entL.data()) : nullptr;
        reinterpret_cast<void(__thiscall*)(void*)>(target)(a); }
    else if(nm=="CleanUpOldReference-"){
        const RefPlan p=BuildRefs(r,true,false); void* arg=r.next()%5 ? p.argCell : static_cast<void*>(&cells[(r.next()%NC)]);
        out["arg"]=Name(arg); reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(a,arg); }
    else if(nm=="RegisterReference-"){
        { static const uint32 types[]{1,1,1,2,3,4,5,0,6,7}; TypeByte(types[r.next()%std::size(types)]); }
        { uint32& fl=At<uint32>(a,E_FLAGS); fl=(fl&~0x4400000u)|(r.next()%3==0 ? 0x400000u : 0u)|(r.next()%3==0 ? 0x4000000u : 0u); }
        a[0x2E]=r.next()%3==0 ? uint8(1+r.next()%200) : uint8(0);
        const RefPlan p=BuildRefs(r,true,true); void* arg=r.next()%4 ? p.argCell : static_cast<void*>(&cells[(r.next()%NC)]);
        out["arg"]=Name(arg);
        withPools([&]{ reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(a,arg); }); }
    else if(nm=="ResolveReferences"||nm=="PruneReferences"){
        BuildRefs(r,true,false); reinterpret_cast<void(__thiscall*)(void*)>(target)(a); }
    else if(nm=="IsEntityPointerValid"){
        a[E_INFO]=uint8((r.next()%8)|((r.next()%32)<<3)); void* const arg=r.next()%10==0 ? nullptr : static_cast<void*>(a);
        ret=reinterpret_cast<uint32(__cdecl*)(void*)>(target)(arg)&0xFFu; }
    else if(nm=="FlagToDestroyWhenNextProcessed"||nm=="ProcessCollision"||nm=="ProcessControl"||nm=="ProcessShift"){
        reinterpret_cast<void(__thiscall*)(void*)>(target)(a); }
    else if(nm=="TestCollision"){
        ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(target)(a,r.next()&1)&0xFFu; }
    else if(nm=="Teleport"){
        const float x=Mixed(r),y=Mixed(r),z=Mixed(r); reinterpret_cast<void(__thiscall*)(void*,float,float,float,uint32)>(target)(a,x,y,z,r.next()&1); }
    else if(nm=="SpecialEntityPreCollisionStuff"){
        for(size_t k=0;k<8;++k)boolBuf[k]=uint8(r.next()&1);
        reinterpret_cast<void(__thiscall*)(void*,void*,uint32,void*,void*,void*,void*)>(target)(a,entB.data(),r.next()&1,&boolBuf[0],&boolBuf[1],&boolBuf[2],&boolBuf[3]); }
    else if(nm=="SpecialEntityCalcCollisionSteps"){
        for(size_t k=0;k<8;++k)boolBuf[k]=uint8(r.next()&1);
        ret=reinterpret_cast<uint32(__thiscall*)(void*,void*,void*)>(target)(a,&boolBuf[0],&boolBuf[1])&0xFFu; }
    else if(nm=="SetIsStatic"){
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(a,r.next()&1); }
    else if(nm=="GetIsStatic"){
        { uint32& fl=At<uint32>(a,E_FLAGS); fl=(fl&~0x40004u)|(r.next()%2 ? 4u : 0u)|(r.next()%2 ? 0x40000u : 0u); if(i<4)fl=(fl&~0x40004u)|(i&1 ? 4u : 0u)|(i&2 ? 0x40000u : 0u); }
        ret=reinterpret_cast<Th0>(target)(a)&0xFFu; }
    else if(nm=="GetIsTypePhysical"||nm=="TreatAsPlayerForCollisions"){
        a[E_INFO]=i<256 ? uint8(i) : uint8(r.next()); ret=reinterpret_cast<Th0>(target)(a)&0xFFu; }
    else if(nm=="DoesNotCollideWithFlyers"){
        const int idx=Idx(); At<int16>(a,E_MODEL)=int16(idx);
        uint16 w=uint16(r.next()); if(r.next()%2){ const uint32 st=r.next()%3 ? (1u+r.next()%2) : (r.next()%16); w=uint16((w&~0x7800u)|(st<<11)); }
        At<uint16>(Mi(size_t(idx-MI0)),0x12)=w; ret=reinterpret_cast<Th0>(target)(a)&0xFFu; }
    else if(nm=="SetupBigBuilding"){
        const int idx=Idx(); At<int16>(a,E_MODEL)=int16(idx); reinterpret_cast<void(__thiscall*)(void*)>(target)(a); }
    else if(nm=="SetModelIndex"){
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(a,uint32(r.next()%20000)); }
    else if(nm=="SetModelIndexNoCreate"){
        const int idx=Idx(); At<uint16>(Mi(size_t(idx-MI0)),0x12)=uint16(r.next());
        { uint16& w=At<uint16>(Mi(size_t(idx-MI0)),0x12); w=uint16((w&~0x42u)|(r.next()%2 ? 2u : 0u)|(r.next()%2 ? 0x40u : 0u)); }
        amiRet=r.next()%3 ? static_cast<void*>(amiBuf.data()) : nullptr;
        { uint16& w=At<uint16>(amiBuf.data(),0x12); w=uint16(r.next()); if(r.next()%10<4)w=uint16((w&~0x7800u)|0x3000u); if(r.next()%2)w|=0x8000u; else w&=uint16(~0x8000u); }
        reinterpret_cast<void(__thiscall*)(void*,uint32)>(target)(a,uint32(idx)); }
    else if(nm=="CreateRwObject"){
        { static const uint32 types[]{1,1,2,3,4,4,5}; TypeByte(types[r.next()%std::size(types)]); }
        { uint32& fl=At<uint32>(a,E_FLAGS); fl=(fl&~0x280u)|(r.next()%8 ? 0x80u : 0u)|(r.next()%3==0 ? 0x200u : 0u); }
        const int idx=Idx(); At<int16>(a,E_MODEL)=int16(idx);
        { uint16& w=At<uint16>(Mi(size_t(idx-MI0)),0x12); w=uint16((w&~0x100u)|(r.next()%3 ? 0x100u : 0u)); }
        { const uint32 k=r.next()%8; createRet=k==0 ? nullptr : k<3 ? static_cast<void*>(atomF.data()) : static_cast<void*>(clumpF.data()); }
        At<void*>(a,E_RW)=reinterpret_cast<void*>(0x4321u);
        At<void*>(a,E_MATRIX)=r.next()%2 ? static_cast<void*>(matA.data()) : nullptr;
        if(r.next()%3){ At<void*>(a,E_LOD)=entL.data(); At<void*>(entL.data(),E_RW)=r.next()%3==0 ? nullptr : r.next()%2 ? static_cast<void*>(lodAtomF.data()) : static_cast<void*>(lodClumpF.data()); }
        assocLodRet=r.next()%3 ? static_cast<void*>(assocLod.data()) : nullptr; assocOwnRet=r.next()%3 ? static_cast<void*>(assocOwn.data()) : nullptr;
        At<float>(assocLod.data(),0x20)=r.uni(0.f,10.f); At<float>(assocOwn.data(),0x20)=r.uni(0.f,10.f);
        At<void*>(a,E_MOVING)=r.next()%2 ? nullptr : static_cast<void*>(movNode.data());
        firstAtomicRet=r.next()%4 ? static_cast<void*>(atomF2.data()) : nullptr;
        reinterpret_cast<void(__thiscall*)(void*)>(target)(a); }
    else if(nm=="DeleteRwObject"){
        { static const uint32 types[]{1,2,3,4,5,4,1}; TypeByte(types[r.next()%std::size(types)]); }
        const int idx=Idx(); At<int16>(a,E_MODEL)=int16(idx);
        { uint16& w=At<uint16>(Mi(size_t(idx-MI0)),0x12); w=uint16((w&~0x100u)|(r.next()%2 ? 0x100u : 0u)); }
        { const uint32 k=r.next()%6; At<void*>(a,E_RW)=k==0 ? nullptr : k<3 ? static_cast<void*>(atomF.data()) : static_cast<void*>(clumpF.data()); }
        At<void*>(atomF.data(),4)=r.next()%8 ? static_cast<void*>(frameA.data()) : nullptr;
        skinRet=r.next()%2 ? static_cast<void*>(skinF.data()) : nullptr; firstAtomicRet=r.next()%4 ? static_cast<void*>(atomF2.data()) : nullptr;
        At<void*>(a,E_LINK)=r.next()%4 ? static_cast<void*>(linkA.data()) : nullptr;
        miTypeRet=r.next()%4 ? 5u : r.next()%9;
        reinterpret_cast<void(__thiscall*)(void*)>(target)(a); }
    else if(nm=="GetIsTouching-ent"||nm=="GetIsTouching-vec"){
        const bool vec=nm=="GetIsTouching-vec";
        At<int16>(a,E_MODEL)=int16(MI0); const int idxB=r.next()%2 ? MI0 : MI1; At<int16>(entB.data(),E_MODEL)=int16(idxB);
        float* const cA=reinterpret_cast<float*>(cols[0].data()+0x18); float* const cB=reinterpret_cast<float*>(cols[size_t(idxB-MI0)%2].data()+0x18);
        for(int k=0;k<3;++k){ cA[k]=r.next()%3 ? Mixed(r) : 0.f; cB[k]=r.next()%3 ? Mixed(r) : 0.f; tfA[size_t(k)]=Mixed(r); tfB[size_t(k)]=Mixed(r); }
        const float rA0=At<float>(cols[0].data(),0x24); const float rB0=At<float>(cols[size_t(idxB-MI0)%2].data(),0x24);
        float other[3]; float pa[3]; for(int k=0;k<3;++k)pa[k]=cA[k]+tfA[size_t(k)];
        float radiusArg=0.f; float tie=0.f;
        if(vec){ for(int k=0;k<3;++k)other[k]=r.next()%4 ? pa[k]+Mixed(r)*0.5f : Mixed(r); }
        else { for(int k=0;k<3;++k)other[k]=cB[k]+tfB[size_t(k)]; }
        { const float dx=pa[0]-other[0],dy=pa[1]-other[1],dz=pa[2]-other[2];
          const float s=vec ? (dz*dz+dx*dx)+dy*dy : (dz*dz+dy*dy)+dx*dx; tie=std::sqrt(s); }
        const uint32 mode=r.next()%5;
        if(vec){ if(mode<3)At<float>(cols[0].data(),0x24)=0.f; radiusArg=mode<3 ? NextUlp(tie,int(r.next()%7)-3) : r.uni(-5.f,60.f); (void)rA0; }
        else { const float want=mode<3 ? NextUlp(tie,int(r.next()%7)-3) : r.uni(0.f,120.f); At<float>(cols[0].data(),0x24)=want*0.5f; At<float>(cols[size_t(idxB-MI0)%2].data(),0x24)=want*0.5f; (void)rB0; }
        out["radius"]=F(radiusArg);
        if(vec)ret=reinterpret_cast<uint32(__thiscall*)(void*,const void*,float)>(target)(a,other,radiusArg)&0xFFu;
        else ret=reinterpret_cast<uint32(__thiscall*)(void*,void*)>(target)(a,entB.data())&0xFFu; }
    else if(nm=="FindTriggerPointCoors"){
        At<int16>(a,E_MODEL)=int16(MI0); At<uint8>(Mi(0),0xD)=uint8(r.next()%(NFX+1));
        for(size_t k=0;k<NFX;++k){ At<uint8>(fx[k].data(),0xC)=r.next()%2 ? uint8(8) : uint8(r.next()%11); At<int32>(fx[k].data(),0x10)=int32(r.next()%6); FillFloats(fx[k].data(),3,r); for(int q=0;q<3;++q)At<float>(fx[k].data(),size_t(q)*4)=Mixed(r); }
        int32 index=int32(r.next()%7)-1; if(r.next()%10<6&&At<uint8>(Mi(0),0xD)>0)index=At<int32>(fx[r.next()%At<uint8>(Mi(0),0xD)].data(),0x10);
        At<void*>(a,E_MATRIX)=r.next()%2 ? static_cast<void*>(matA.data()) : nullptr;
        for(size_t k=0;k<16;++k)At<float>(matA.data(),k*4)=Mixed(r);
        std::memset(outBuf.data(),0xCD,0x40); out["index"]=index;
        auto* p=reinterpret_cast<void*(__thiscall*)(void*,void*,int32)>(target)(a,outBuf.data(),index); ret=Name(p); }
    // ---- recogida ----
    const uint32 vptr=At<uint32>(a,0);
    if(expectVptr)out["vptr"]=(candidate ? InDll(vptr) : vptr==expectVptr) ? json("CEntity") : json(vptr); else out["vptr"]=vptr;
    out["ent"]=EntWords(a);
    { json s=json::object(); for(auto& t:tracked)s[t.name]=Fnv(t.p,t.n); out["state"]=std::move(s); }
    out["globals"]=Globals();
    (void)gEmpty;
    return ret;
}

// ======================= tabla =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=28;
inline const std::array<Fn,NFN> fns{{
    {"CEntity","Constructor",0x532A90,0,-1,512},{"CEntity","Destructor",0x535E90,0,-1,512},{"CEntity","CleanUpOldReference-",0x571A00,0,-1,512},
    {"CEntity","RegisterReference-",0x571B70,0,-1,1024},{"CEntity","ResolveReferences",0x571A40,0,-1,512},{"CEntity","PruneReferences",0x571A90,0,-1,512},
    {"CEntity","IsEntityPointerValid",0x533310,0,-1,512},{"CEntity","FlagToDestroyWhenNextProcessed",0,0x863928,21,64},{"CEntity","SetModelIndex",0,0x863928,5,256},
    {"CEntity","SetModelIndexNoCreate",0,0x863928,6,1024},{"CEntity","CreateRwObject",0,0x863928,7,2048},{"CEntity","DeleteRwObject",0,0x863928,8,1024},
    {"CEntity","SetIsStatic",0,0x863928,4,128},{"CEntity","GetIsStatic",0x4633E0,0,-1,256},{"CEntity","GetIsTouching-ent",0x5343F0,0,-1,1024},
    {"CEntity","GetIsTouching-vec",0x5344B0,0,-1,1024},{"CEntity","GetIsTypePhysical",0x4DA030,0,-1,256},{"CEntity","DoesNotCollideWithFlyers",0x532D40,0,-1,512},
    {"CEntity","TreatAsPlayerForCollisions",0x541F70,0,-1,256},{"CEntity","ProcessCollision",0,0x863928,11,64},{"CEntity","ProcessControl",0,0x863928,10,64},
    {"CEntity","ProcessShift",0,0x863928,12,64},{"CEntity","TestCollision",0,0x863928,13,64},{"CEntity","Teleport",0,0x863928,14,128},
    {"CEntity","SpecialEntityCalcCollisionSteps",0,0x863928,16,64},{"CEntity","SpecialEntityPreCollisionStuff",0,0x863928,15,128},
    {"CEntity","FindTriggerPointCoors",0x533380,0,-1,1024},{"CEntity","SetupBigBuilding",0x533150,0,-1,256},
}};
// direccion del EXE que debe tener cada hueco de la vtable de CEntity (volcada con rd.py i863928...)
inline uintptr_t ExpectedSlot(int slot){
    switch(slot){ case 4: return 0x403E20; case 5: return 0x532AE0; case 6: return 0x533700; case 7: return 0x533D30; case 8: return 0x534030; case 10: return 0x403E40; case 11: return 0x403E50;
        case 12: return 0x403E60; case 13: return 0x403E70; case 14: return 0x403E80; case 15: return 0x403E90; case 16: return 0x403EA0; case 21: return 0x403EB0; default: return 0; } }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"centb-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"centb-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("centb");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"centb-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // cuerpos de la DLL de las virtuales que llama el codigo probado (el simbolo de una virtual es un thunk de vtable)
    const uintptr_t dllDelRw=DllBody("CEntity","DeleteRwObject");
    if(!dllDelRw)Write({{"event","warning"},{"reason","CEntity::DeleteRwObject body not found"}});
    cbDll=FollowIn(Addr(&AtomicRemoveAnimFromSkinCB));

    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto* e=reinterpret_cast<CEntity*>(entA.data()); auto* ph=reinterpret_cast<CPhysical*>(entA.data()); auto* col=reinterpret_cast<CColModel*>(cols[0].data()); auto* fxp=reinterpret_cast<C2dEffect*>(fx[0].data());
      auto* mi0=reinterpret_cast<CBaseModelInfo*>(Mi(0)); auto* as=reinterpret_cast<CAnimBlendAssociation*>(assocOwn.data()); auto* ref=reinterpret_cast<CReference*>(nodes.data());
      auto Off=[&](const void* m,const void* base){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      Chk("m_placement",Off(&e->m_placement,e),4); Chk("m_matrix",Off(&e->m_matrix,e),E_MATRIX); Chk("m_nFlags",Off(&e->m_nFlags,e),E_FLAGS); Chk("m_nRandomSeed",Off(&e->m_nRandomSeed,e),E_SEED);
      Chk("m_nModelIndex",Off(&e->m_nModelIndex,e),E_MODEL); Chk("m_pReferences",Off(&e->m_pReferences,e),E_REFS); Chk("m_pMovingList",Off(&ph->m_pMovingList,ph),E_MOVING);
      Chk("m_boundSphere.center",Off(&col->m_boundSphere.m_vecCenter,col),0x18); Chk("m_boundSphere.radius",Off(&col->m_boundSphere.m_fRadius,col),0x24);
      Chk("C2dEffect.m_Type",Off(&fxp->m_Type,fxp),0xC); Chk("C2dEffect.slotMachineIndex",Off(&fxp->slotMachineIndex,fxp),0x10); Chk("C2dEffect.m_Pos",Off(&fxp->m_Pos,fxp),0);
      Chk("mi.m_n2dfxCount",Off(&mi0->m_n2dfxCount,mi0),0xD); Chk("mi.m_nFlags",Off(&mi0->m_nFlags,mi0),0x12); Chk("mi.m_pColModel",Off(&mi0->m_pColModel,mi0),0x14); Chk("mi.m_nRefCount",Off(&mi0->m_nRefCount,mi0),8);
      Chk("assoc.m_CurrentTime",Off(&as->m_CurrentTime,as),0x20); Chk("CReference.m_ppEntity",Off(&ref->m_ppEntity,ref),4);
      Chk("sizeof(CEntity)",uint32(sizeof(CEntity)),0x38); Chk("sizeof(CReference)",uint32(sizeof(CReference)),8);
      Write({{"event","layout"},{"layout_ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    regions.clear();
    for(int k=0;k<3;++k)Snap(G_MIPTRS+size_t(MI0+k)*4,4);
    Snap(G_BUILDINGS,4); Snap(G_DAMAGED,1); Snap(G_NUMMAT,4); Snap(G_EMPTYREF,4); Snap(G_PEDPOOL,16); Snap(G_MOVLIST,4);

    FillEntTraps(std::make_integer_sequence<int,24>{}); entVt[4]=reinterpret_cast<void*>(&EV4); entVt[6]=reinterpret_cast<void*>(&EV6); entVt[7]=reinterpret_cast<void*>(&EV7);
    FillMiTraps(std::make_integer_sequence<int,32>{}); miVt[1]=reinterpret_cast<void*>(&MiAsAtomic); miVt[4]=reinterpret_cast<void*>(&MiGetType); miVt[11]=reinterpret_cast<void*>(&MiCreateInst);
    InitTables();

    bool passed=true; int pair=0;
    {
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            // parches propios de la funcion (nunca se sustituye la funcion probada ni sus propios cuerpos)
            Patches p; std::set<uintptr_t> stubSet;
            auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
                const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
                stubSet.insert(st); stubSet.insert(stb);
                auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
                if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
            if(nm=="Constructor"){ P(0x821B1E,0,S_Rand); P(0,Addr(&CGeneral::GetRandomNumber),S_RandNum); }
            else if(nm=="Destructor"){
                P(0x534030,dllDelRw,S_DelRw); P(0x571A40,Addr(&CEntity::ResolveReferences),S_Resolve); P(0x54F3B0,Addr(&CPlaceable::RemoveMatrix),S_RemoveMatrix); }
            else if(nm=="SetModelIndexNoCreate"){
                P(0x532B70,Addr(&CEntity::HasPreRenderEffects),S_HasPre); P(0x49CC90,Addr(&CTagManager::AddTag),S_AddTag); }
            else if(nm=="CreateRwObject"){
                P(0x4C4BA0,Addr(&CBaseModelInfo::AddRef),S_AddRef); P(0x59AD70,Addr(static_cast<void(CMatrix::*)(RwMatrix*) const>(&CMatrix::UpdateRwMatrix)),S_MatUpdateRw);
                P(0x54EF40,Addr(&CSimpleTransform::UpdateRwMatrix),S_SimpleUpdateRw); P(0x49CCE0,Addr(&CTagManager::IsTag),S_IsTag); P(0x49CD50,Addr(&CTagManager::ResetAlpha),S_ResetAlpha);
                P(0x5D7200,Addr(static_cast<void(*)(RpAtomic*,bool)>(&CCustomBuildingDNPipeline::PreRenderUpdate)),S_DnPre);
                P(0x5D7F40,Addr(&CCustomBuildingRenderer::IsCBPCPipelineAttached),S_Cbpc); P(0x734820,Addr(&GetFirstAtomic),S_FirstAtomic);
                P(0x4D15E0,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*)>(&RpAnimBlendClumpGetFirstAssociation)),S_FirstAssoc);
                P(0x4CEA80,Addr(&CAnimBlendAssociation::SetCurrentTime),S_SetCurTime); P(0x409650,Addr(&CStreaming::AddEntity),S_StAdd); P(0x533790,Addr(&CEntity::CreateEffects),S_CreateFx);
                P(0x542800,Addr(&CPhysical::AddToMovingList),S_AddMoving); P(0x5523C0,0,S_NodeNew); P(0x5329A0,0,S_ExeListAdd);
                P(0,Addr(&CPtrListDoubleLink<CPhysical*>::AddItem),S_DllListAdd); }
            else if(nm=="DeleteRwObject"){
                P(0x749DC0,Addr(&RpAtomicDestroy),S_AtomicDestroy); P(0x7F05A0,Addr(&RwFrameDestroy),S_FrameDestroy); P(0x74A310,Addr(&RpClumpDestroy),S_ClumpDestroy);
                P(0x734820,Addr(&GetFirstAtomic),S_FirstAtomic); P(0x7C7550,Addr(&RpSkinGeometryGetSkin),S_Skin); P(0x749B70,Addr(&RpClumpForAllAtomics),S_ForAll);
                P(0x4C4BB0,Addr(&CBaseModelInfo::RemoveRef),S_RemoveRef); P(0x409710,Addr(&CStreaming::RemoveEntity),S_StRemove); P(0x533BF0,Addr(&CEntity::DestroyEffects),S_DestroyFx);
                P(0x717900,Addr(&CEntity::RemoveEscalatorsForEntity),S_RemoveEsc); P(0x5336B0,Addr(&CPtrListDoubleLink<CPhysical*>::DeleteItem),S_ListDelete); }
            else if(nm=="GetIsTouching-ent"||nm=="GetIsTouching-vec"){
                P(0x5334F0,Addr(static_cast<CVector(CEntity::*)(const CVector&) const>(&CEntity::TransformFromObjectSpace)),S_Tfos);
                P(0,Addr(static_cast<CVector*(CEntity::*)(CVector&,const CVector&) const>(&CEntity::TransformFromObjectSpace)),S_Tfos); }
            else if(nm=="FindTriggerPointCoors"){
                P(0x4C4C70,Addr(&CBaseModelInfo::Get2dEffect),S_Get2dFx); P(0x54F560,Addr(&CPlaceable::AllocateMatrix),S_AllocMatrix);
                P(0x54F1B0,Addr(&CSimpleTransform::UpdateMatrix),S_SimpleUpdateMatrix); }
            else if(nm=="IsEntityPointerValid"){
                P(0x4040E0,Addr(&IsBuildingPointerValid),S_Valid<0>); P(0x6E38F0,Addr(&IsVehiclePointerValid),S_Valid<1>); P(0x5E8A30,Addr(&IsPedPointerValid),S_Valid<2>);
                P(0x5A2C20,Addr(&IsObjectPointerValid),S_Valid<3>); P(0x532730,Addr(&IsDummyPointerValid),S_Valid<4>); }
            const uintptr_t expect=ExpectedSlot(fn.slot);
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xCE47B1u}; if(!r.s)r.s=1;
                    RestoreRegions();
                    for(auto* h:hooks)h->State(candidate);
                    for(auto& e:p.v)e->Enable(true); // los parches de entrada se reafirman despues de fijar los hooks
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    json out; out["switches"]=fn.slot>=0 ? ((target<0x400000||target>=0x1000000)==candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    if(fn.slot>=0&&!candidate)out["slot_is_exe_function"]=(target==expect);
                    json ret=Run(nm,target,r,i,out,candidate);
                    trace=RecsToJson(); // el registro de los stubs se convierte a json fuera de la llamada
                    RestoreRegions();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                json original=run(false), candidate=run(true);
                const bool slotOk=fn.slot<0||original.value("slot_is_exe_function",false);
                original.erase("slot_is_exe_function");
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&slotOk;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array();
                    for(auto& [k,v]:original.items()){ if(candidate[k]==v)continue; diff.push_back(k);
                        if(k=="state")for(auto& [sk,sv]:v.items())if(candidate[k][sk]!=sv)diff.push_back("state."+sk); }
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
