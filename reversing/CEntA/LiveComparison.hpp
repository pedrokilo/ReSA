#pragma once
// Banco dentro del motor: CEntity, grupo A (matrices, transformaciones, limites y visibilidad; 26 funciones):
// AttachToRwObject, DetachFromRwObject, BuildWindSockMatrix, CalculateBBProjection, ModifyMatrixForBannerInWind,
// ModifyMatrixForCrane, ModifyMatrixForTreeInWind, GetBoundCentre (ref y vec), GetBoundRect (virtual, vtable 0x863928 ranura 9),
// GetColModel, GetDistanceFromCentreOfMassToBaseOfModel, GetModellingMatrix, TransformFromObjectSpace (ptr y ref), UpdateRwFrame,
// UpdateRwMatrix, UpdateRpHAnim, UpdateAnim, Add (rect; virtual, ranura 1), Remove (virtual, ranura 3), LivesInThisNonOverlapSector,
// GetIsBoundingBoxOnScreen, GetIsOnScreen, IsEntityOccluded e IsVisible.
// Original (hooks del grupo desactivados) frente a candidato (DLL) con fixtures identicos y comparacion bit a bit.
// Entidades FALSAS: bloques de 0xA20 bytes (tipo, banderas, semilla, indice de modelo, matriz o posicion compacta, objeto RenderWare,
// enlace de streaming, lista de movimiento, tipo de objeto +0x13C e indice de colision especial +0x48B) con una vtable FALSA cuyas ranuras
// usadas (SetIsStatic y GetBoundRect) son stubs trazados y el resto aborta. Model info FALSO (indice 19990: banderas, tipo de modelo por
// la ranura 4 de su vtable y modelo de colision) y modelos de colision especiales de vehiculo (4 primeras entradas de 0xC1CC78).
// RenderWare FALSO: clump (tipo 2) y atomico (tipo 1) con su marco (matriz de modelado en +0x10), geometria, piel y jerarquia.
// Mundo FALSO para Add/Remove: las listas de sectores, de repeticion y LOD se vacian y se rellenan con pools de nodos LOCALES (simple y doble)
// y pools de entidades FALSOS (cubren el bloque de entidades: la DLL de depuracion valida nodos e Items contra esos pools).
// Se simulan y trazan (en el EXE y en la DLL): CPlaceable::AllocateMatrix/GetMatrix (matriz del "pool" falsa), RwFrameUpdateObjects,
// GetFirstAtomic, RpSkinGeometryGetSkin, GetAnimHierarchyFromSkinClump, RpHAnimHierarchyUpdateMatrices, CBaseModelInfo::AddRef/RemoveRef,
// CStreaming::AddEntity/RemoveEntity, CEntity::CreateEffects/DestroyEffects, CPhysical::AddToMovingList, CCamera::IsSphereVisible,
// RpAnimBlendClumpGetFirstAssociation/UpdateAnimations y CWindModifiers::FindWindModifier. CalcScreenCoors (las dos) y los dos metodos de
// CActiveOccluder son funciones PURAS (dependen solo de sus argumentos): el EXE las evalua perezosamente y la DLL tambien tras la reescritura.
// CVector, CMatrix, CSimpleTransform, TransformVectors y TransformPoint corren de verdad.
// Estado global tocado: se guarda y se restaura antes de cada ejecucion y al final (punteros de pools, sectores, repeticion, LOD, lista de
// movimiento, entrada de ms_modelInfoPtrs, camara, viento, reloj y pausa, oclusores, colisiones especiales, numBBFailed).
#include <fstream>
#include <functional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity.h"
#include "Physical.h"
#include "Ped.h"
#include "Building.h"
#include "Dummy.h"
#include "CutsceneMgr.h"
#include "Animation/AnimManager.h"
#include "Object.h"
#include "Vehicle.h"
#include "References.h"
#include "World.h"
#include "Camera.h"
#include "Timer.h"
#include "Weather.h"
#include "WindModifiers.h"
#include "Occlusion.h"
#include "ActiveOccluder.h"
#include "Streaming.h"
#include "RwHelper.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace CEntALive {
inline bool done{}; inline std::ofstream log; inline json trace; inline DWORD mainThread{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class V=uint32> inline V& At(void* base,size_t off) { return *reinterpret_cast<V*>(static_cast<uint8*>(base)+off); }
template<class V=uint32> inline V& AtA(uintptr_t addr) { return *reinterpret_cast<V*>(addr); }
// Sigue los saltos de la ILT de la DLL sin entrar en la imagen del EXE (como CPedE)
inline uintptr_t Body(uintptr_t p) {
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p;
}
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline std::string Hex80(const uint8* p) { std::string s; for(size_t k=10;k-->0;)s+=std::format("{:02x}",p[k]); return s; }

// --- direcciones del EXE ---
constexpr uintptr_t VT_ENT=0x863928, MI_PTRS=0xA9B0C8, POOLS=0xB74484, SECTORS=0xB7D0B8, REPEAT=0xB992B8, LODL=0xB99EB8, MOVING_LIST=0xB9ACC8,
    CAM_MIRROR=0xB6F998, CAM_FRUSTUM=0xB6FB7C, CAM_MAT_MAIN=0xB6FA74, CAM_MAT_MIRROR=0xB6FABC, BBFAILED=0xB71808, WIND=0xC812F0, WINDDIR=0xC813E0,
    PAUSE=0xB7CB48, STEPS=0xB7CB58, TIMEMS=0xB7CB84, OCC_COUNT=0xC73CC0, OCC_ARR=0xC78610, SPECIAL_COL=0xC1CC78;
constexpr size_t SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12, LOD_SIZE=30*30*4, CAM_FRUSTUM_SIZE=0x80, OCC_SIZE=4*0xAC, SPECIAL_SIZE=4*0x30, POOLS_SIZE=0x20;
constexpr int MODEL=19990;
constexpr size_t ES=0xA20; constexpr int NE=4, NDBL=8192, NSGL=8192;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,ES*NE> entMem{};
inline uint8* E(int k) { return entMem.data()+size_t(k)*ES; }
inline std::array<uint8,ES*NE/0x38+1> entStates{};
alignas(16) inline std::array<std::array<uint8,0x60>,NE+2> mats{}; // 0..NE-1 entidades, NE = matriz que devuelve AllocateMatrix
alignas(16) inline std::array<uint8,0x100> frameBuf{}, clumpBuf{}, atomObj{}, atomBuf{}, geomBuf{}, hierBuf{}, skinBuf{}, miBuf{}, linkBuf{}, assocBuf{}, outBuf{};
alignas(16) inline std::array<uint8,0x30> colBuf{};
alignas(16) inline std::array<uint8,NDBL*12> dblStore{}; inline std::array<uint8,NDBL> dblStates{};
alignas(16) inline std::array<uint8,NSGL*8> sglStore{}; inline std::array<uint8,NSGL> sglStates{};
alignas(16) inline std::array<uint8,0x20> dblHdr{}, sglHdr{}, dblImg{}, sglImg{};
alignas(16) inline std::array<uint8,0x14> vehHdr{}, pedHdr{}, objHdr{}, bldHdr{}, dumHdr{};
inline std::array<void*,64> fvt{}; inline std::array<void*,32> miVt{};
template<size_t N> std::array<uint8,N> MaskMat(const std::array<uint8,N>& m,size_t base) { auto c=m; for(size_t o:{size_t(0xC),size_t(0x1C),size_t(0x2C),size_t(0x3C)})std::memset(c.data()+base+o,0,4); return c; }

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    struct R { uintptr_t base; size_t size, stride; const char* tag; };
    const R regions[]{
        {uintptr_t(entMem.data()),entMem.size(),ES,"e"},{uintptr_t(mats.data()),sizeof(mats),sizeof(mats[0]),"m"},
        {uintptr_t(frameBuf.data()),frameBuf.size(),frameBuf.size(),"frame"},{uintptr_t(clumpBuf.data()),clumpBuf.size(),clumpBuf.size(),"clump"},
        {uintptr_t(atomObj.data()),atomObj.size(),atomObj.size(),"atomobj"},{uintptr_t(atomBuf.data()),atomBuf.size(),atomBuf.size(),"atom"},
        {uintptr_t(geomBuf.data()),geomBuf.size(),geomBuf.size(),"geom"},{uintptr_t(hierBuf.data()),hierBuf.size(),hierBuf.size(),"hier"},
        {uintptr_t(skinBuf.data()),skinBuf.size(),skinBuf.size(),"skin"},{uintptr_t(miBuf.data()),miBuf.size(),miBuf.size(),"mi"},
        {uintptr_t(linkBuf.data()),linkBuf.size(),linkBuf.size(),"link"},{uintptr_t(assocBuf.data()),assocBuf.size(),assocBuf.size(),"assoc"},
        {uintptr_t(outBuf.data()),outBuf.size(),outBuf.size(),"out"},{uintptr_t(colBuf.data()),colBuf.size(),colBuf.size(),"col"},
        {uintptr_t(dblStore.data()),dblStore.size(),12,"dn"},{uintptr_t(sglStore.data()),sglStore.size(),8,"sn"},
        {REPEAT,REPEAT_SIZE,12,"rs"},{SECTORS,SECTORS_SIZE,8,"sec"},{LODL,LOD_SIZE,4,"lod"},{MOVING_LIST,4,4,"moving"},
        {SPECIAL_COL,SPECIAL_SIZE,0x30,"special"},{OCC_ARR,OCC_SIZE,0xAC,"occ"}};
    for(auto& r:regions)if(u>=r.base&&u<r.base+r.size)return std::format("{}#{}+{:x}",r.tag,(u-r.base)/r.stride,(u-r.base)%r.stride);
    const auto* tib=reinterpret_cast<NT_TIB*>(NtCurrentTeb()); if(u>=uintptr_t(tib->StackLimit)&&u<uintptr_t(tib->StackBase))return "stack";
    return std::format("{:08x}",u);
}
inline json V3(const CVector* v) { return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }

// --- generador de los stubs (mismo estado al empezar cada ejecucion) ---
inline uint32 sst{};
inline uint32 SNext() { sst^=sst<<13; sst^=sst>>17; sst^=sst<<5; return sst; }
inline uint32 Mix(uint32 a,uint32 b) { uint32 h=a*2654435761u^(b+0x9E3779B9u+(a<<6)+(a>>2)); h^=h>>15; h*=2246822519u; h^=h>>13; return h; }
inline void Tr(json j) { if(GetCurrentThreadId()==mainThread)trace.push_back(std::move(j)); }
// configuracion por caso de los stubs
inline int miType{5}; inline bool atomOn{}, skinOn{}, assocOn{};
inline uint32 sphPct{50}, sphSeed{}, scrSeed{}, occSeed{}, occAreaPct{90}, occBehindPct{90}, windSeed{};
inline float rc[4]{}; // rectangulo inyectado en orden de memoria: izquierda(+0), borde +4, derecha(+8), borde +0xC

// --- dependencias simuladas ---
inline void __fastcall AllocStub(uint8* e,void*) { Tr({"allocate_matrix",Name(e)}); At<void*>(e,0x14)=mats[NE].data(); }
inline CMatrix* __fastcall GetMatrixStub(uint8* e,void*) {
    if(!At<void*>(e,0x14)){ AllocStub(e,nullptr); reinterpret_cast<CPlaceable*>(e)->m_placement.UpdateMatrix(reinterpret_cast<CMatrix*>(mats[NE].data())); }
    return reinterpret_cast<CMatrix*>(At<void*>(e,0x14));
}
inline RwFrame* __cdecl FrameUpdStub(RwFrame* f) { Tr({"frame_update_objects",Name(f)}); return f; }
inline RpAtomic* __cdecl FirstAtomicStub(RpClump* c) { Tr({"get_first_atomic",Name(c)}); return atomOn ? reinterpret_cast<RpAtomic*>(atomBuf.data()) : nullptr; }
inline void* __cdecl SkinStub(void* g) { Tr({"get_skin",Name(g)}); return skinOn ? skinBuf.data() : nullptr; }
inline RpHAnimHierarchy* __cdecl HierStub(RpClump* c) { Tr({"get_anim_hierarchy_from_skin_clump",Name(c)}); return reinterpret_cast<RpHAnimHierarchy*>(hierBuf.data()); }
inline int __cdecl HierUpdStub(void* h) { Tr({"hanim_update_matrices",Name(h)}); return 1; }
inline void __fastcall AddRefStub(uint8* mi,void*) { Tr({"model_add_ref",Name(mi)}); }
inline void __fastcall RemoveRefStub(uint8* mi,void*) { Tr({"model_remove_ref",Name(mi)}); }
inline void* __cdecl StreamAddStub(CEntity* e) { Tr({"streaming_add_entity",Name(e)}); return linkBuf.data(); }
inline void __cdecl StreamRemoveStub(void* l) { Tr({"streaming_remove_entity",Name(l)}); }
inline void __fastcall CreateFxStub(uint8* e,void*) { Tr({"create_effects",Name(e)}); }
inline void __fastcall DestroyFxStub(uint8* e,void*) { Tr({"destroy_effects",Name(e)}); }
// la funcion real sale sin hacer nada si ya hay enlace o si la entidad espera colision; el EXE solo la llama sin enlace, la DLL siempre
inline void __fastcall MovingListStub(uint8* e,void*) {
    if(!At<void*>(e,0xB4)&&!(At<uint32>(e,0x1C)&0x40000)){ Tr({"add_to_moving_list",Name(e)}); At<void*>(e,0xB4)=linkBuf.data()+0x40; }
}
inline bool __fastcall SphereStub(void*,void*,const CVector* o,float r,void* m) {
    const char* which=uintptr_t(m)==CAM_MAT_MAIN ? "main" : uintptr_t(m)==CAM_MAT_MIRROR ? "mirror" : "other";
    Tr({"is_sphere_visible",V3(o),F(r),which});
    return (Mix(sphSeed,F(o->x)^F(r)^uint32(uintptr_t(m)))%100)<sphPct;
}
inline void* __cdecl FirstAssocStub(RpClump* c) { Tr({"first_association",Name(c)}); return assocOn ? assocBuf.data() : nullptr; }
inline void __cdecl UpdateAnimsStub(RpClump* c,float step,uint32 onScreen) { Tr({"update_animations",Name(c),F(step),onScreen&0xFF}); }
inline bool __cdecl WindModStub(CVector pos,float* x,float* y) {
    Tr({"find_wind_modifier",V3(&pos),F(*x),F(*y)});
    if(Mix(windSeed,F(pos.x)^F(pos.y))&1u){ *x=*x*1.5f+0.001f; *y=*y*0.5f-0.002f; return true; }
    return false;
}
// funciones puras de pantalla y oclusion: dependen solo de sus argumentos (el orden de evaluacion no importa)
inline uint32 PtHash(const CVector& v,uint32 seed) { return Mix(Mix(F(v.x),F(v.y)),Mix(F(v.z),seed)); }
inline bool __cdecl ScreenStub4(const CVector& in,CVector& out,float& sx,float& sy) {
    const uint32 h=PtHash(in,scrSeed); out.x=float(int(h%640)); out.y=float(int((h>>10)%480)); out.z=float(int((h>>20)%260)-30);
    sx=0.25f*float(1+(h>>3)%16); sy=0.25f*float(1+(h>>7)%16); return ((h>>5)%8)!=0;
}
inline bool __cdecl ScreenStub2(const CVector& in,CVector& out) {
    const uint32 h=PtHash(in,scrSeed); out.x=float(int(h%640)); out.y=float(int((h>>10)%480)); out.z=float(int((h>>20)%260)-30); return ((h>>5)%8)!=0;
}
inline int OccIndex(const void* o) { return int((uintptr_t(o)-OCC_ARR)/0xAC); }
inline bool __fastcall OccAreaStub(void* o,void*,float x,float y,float r) { return (Mix(Mix(occSeed,uint32(OccIndex(o))),Mix(Mix(F(x),F(y)),F(r)))%100)<occAreaPct; }
inline bool __fastcall OccBehindStub(void* o,void*,float x,float y,float z,float r) {
    return (Mix(Mix(occSeed^0x55u,uint32(OccIndex(o))),Mix(Mix(F(x),F(y)),Mix(F(z),F(r))))%100)<occBehindPct;
}

// --- vtables FALSAS ---
template<int K> void __fastcall VTrap(void* self,void*) {
    Write({{"event","error"},{"reason","unexpected virtual call"},{"slot",K},{"self",Name(self)}}); log.flush(); ExitProcess(95);
}
template<int... K> std::array<void*,sizeof...(K)> TrapTable(std::integer_sequence<int,K...>) { return {reinterpret_cast<void*>(&VTrap<K>)...}; }
inline void __fastcall VSetIsStatic(uint8* e,void*,uint32 s) { Tr({"set_is_static",Name(e),s&0xFF}); }
inline float* __fastcall VGetBoundRect(uint8* e,void*,float* out) { Tr({"bound_rect",Name(e)}); out[0]=rc[0]; out[1]=rc[1]; out[2]=rc[2]; out[3]=rc[3]; return out; }
inline uint32 __fastcall MiGetType(uint8* mi,void*) { Tr({"get_model_type",Name(mi)}); return uint32(miType); }

// --- llamadas que devuelven un float en ST0 (se guardan los 80 bits) ---
inline std::string CallThisF0(uintptr_t fnAddr,void* self) {
    uint8 raw[10]{};
    __asm { mov ecx, self
            mov eax, fnAddr
            call eax
            fstp tbyte ptr raw }
    return Hex80(raw);
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=26;
inline const std::array<Fn,NFNS> fns{{
    {"CEntity","AttachToRwObject",0x533ED0,0,-1,768},{"CEntity","DetachFromRwObject",0x533FB0,0,-1,512},
    {"CEntity","BuildWindSockMatrix",0x532DB0,0,-1,512},{"CEntity","CalculateBBProjection",0x535340,0,-1,1024},
    {"CEntity","ModifyMatrixForBannerInWind",0x535040,0,-1,768},{"CEntity","ModifyMatrixForCrane",0x533170,0,-1,512},
    {"CEntity","ModifyMatrixForTreeInWind",0x534E90,0,-1,1024},{"CEntity","GetBoundCentre-ref",0x534290,0,-1,512},
    {"CEntity","GetBoundCentre-vec",0x534250,0,-1,512},{"CEntity","GetBoundRect",0,0x863928,9,512},
    {"CEntity","GetColModel",0x535300,0,-1,256},{"CEntity","GetDistanceFromCentreOfMassToBaseOfModel",0x536BE0,0,-1,256},
    {"CEntity","GetModellingMatrix",0x46A2D0,0,-1,128},{"CEntity","TransformFromObjectSpace-ptr",0x533560,0,-1,768},
    {"CEntity","TransformFromObjectSpace-ref",0x5334F0,0,-1,768},{"CEntity","UpdateRwFrame",0x532B00,0,-1,128},
    {"CEntity","UpdateRwMatrix",0x446F90,0,-1,512},{"CEntity","UpdateRpHAnim",0x532B20,0,-1,256},
    {"CEntity","UpdateAnim",0x535F00,0,-1,768},{"CEntity","Add-rect",0,0x863928,1,1024},
    {"CEntity","Remove",0,0x863928,3,1024},{"CEntity","LivesInThisNonOverlapSector",0x533050,0,-1,1024},
    {"CEntity","GetIsBoundingBoxOnScreen",0x5345D0,0,-1,1024},{"CEntity","GetIsOnScreen",0x534540,0,-1,512},
    {"CEntity","IsEntityOccluded",0x71FAE0,0,-1,1024},{"CEntity","IsVisible",0x536BC0,0,-1,256},
}};

// comprobacion de la disposicion de campos de la DLL frente al EXE (solo direcciones, sin leer nada)
inline std::string LayoutErrors() {
    std::string why;
    auto* e=reinterpret_cast<CEntity*>(E(0)); auto* ph=reinterpret_cast<CPhysical*>(E(0)); auto* ob=reinterpret_cast<CObject*>(E(0)); auto* ve=reinterpret_cast<CVehicle*>(E(0));
    auto* mi=reinterpret_cast<CBaseModelInfo*>(miBuf.data()); auto* cm=reinterpret_cast<CColModel*>(colBuf.data());
    auto chk=[&](const char* nm,const void* addr,const void* base,size_t off){ if(uintptr_t(addr)-uintptr_t(base)!=off){ why+=nm; why+=' '; } };
    auto chkAbs=[&](const char* nm,const void* addr,uintptr_t want){ if(uintptr_t(addr)!=want){ why+=nm; why+=' '; } };
    chk("m_placement",&e->m_placement,e,4); chk("m_matrix",&e->m_matrix,e,0x14); chk("m_nFlags",&e->m_nFlags,e,0x1C); chk("m_nRandomSeed",&e->m_nRandomSeed,e,0x20);
    chk("m_nModelIndex",&e->m_nModelIndex,e,0x22); chk("m_pReferences",&e->m_pReferences,e,0x24);
    chk("m_pMovingList",&ph->m_pMovingList,ph,0xB4); chk("m_nObjectType",&ob->m_nObjectType,ob,0x13C);
    chk("m_vehicleSpecialColIndex",&ve->m_vehicleSpecialColIndex,ve,0x48B);
    chk("mi.m_nFlags",&mi->m_nFlags,mi,0x12); chk("mi.m_pColModel",&mi->m_pColModel,mi,0x14);
    chk("cm.min",&cm->m_boundBox.m_vecMin,cm,0); chk("cm.max",&cm->m_boundBox.m_vecMax,cm,0xC); chk("cm.center",&cm->m_boundSphere.m_vecCenter,cm,0x18);
    chk("cm.radius",&cm->m_boundSphere.m_fRadius,cm,0x24);
    chkAbs("cam.mirror",&TheCamera.m_bMirrorActive,CAM_MIRROR); chkAbs("cam.normals",&TheCamera.m_avecFrustumWorldNormals,CAM_FRUSTUM);
    chkAbs("cam.normals_mirror",&TheCamera.m_avecFrustumWorldNormals_Mirror,CAM_FRUSTUM+0x30); chkAbs("cam.offsets",&TheCamera.m_fFrustumPlaneOffsets,CAM_FRUSTUM+0x60);
    chkAbs("cam.offsets_mirror",&TheCamera.m_fFrustumPlaneOffsets_Mirror,CAM_FRUSTUM+0x70);
    chkAbs("numBBFailed",&numBBFailed,BBFAILED); chkAbs("wind",&CWeather::Wind,WIND); chkAbs("wind_dir",&CWeather::WindDir,WINDDIR);
    chkAbs("occ_count",&COcclusion::NumActiveOccluders,OCC_COUNT); chkAbs("occ_array",&COcclusion::ActiveOccluders,OCC_ARR);
    chkAbs("special_col",&CVehicle::m_aSpecialColModel,SPECIAL_COL); chkAbs("sectors",&CWorld::ms_aSectors,SECTORS); chkAbs("repeat",&CWorld::ms_aRepeatSectors,REPEAT);
    chkAbs("lod",&CWorld::ms_aLodPtrLists,LODL); chkAbs("moving",&CWorld::ms_listMovingEntityPtrs,MOVING_LIST);
    chkAbs("time_ms",&CTimer::m_snTimeInMilliseconds,TIMEMS); chkAbs("time_step",&CTimer::ms_fTimeStep,STEPS+4); chkAbs("time_step_nc",&CTimer::ms_fTimeStepNonClipped,STEPS);
    if(sizeof(CActiveOccluder)!=0xAC){ why+="sizeof(CActiveOccluder) "; }
    if(sizeof(CMatrixLink)>mats[0].size()){ why+="sizeof(CMatrixLink) "; }
    auto* oc=reinterpret_cast<CActiveOccluder*>(OCC_ARR); chk("occ.m_DistToCam",&oc->m_DistToCam,oc,0x78);
    return why;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"centa-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"centa-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("centa");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainThread=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"centa-only.txt"); if(f)std::getline(f,only); }
    if(auto bad=LayoutErrors(); !bad.empty()){Write({{"event","error"},{"reason","layout mismatch"},{"fields",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };

    // --- tablas falsas ---
    { const auto traps=TrapTable(std::make_integer_sequence<int,64>{}); for(int k=0;k<64;++k)fvt[k]=traps[k];
      fvt[4]=reinterpret_cast<void*>(&VSetIsStatic); fvt[9]=reinterpret_cast<void*>(&VGetBoundRect);
      const auto mtraps=TrapTable(std::make_integer_sequence<int,32>{}); for(int k=0;k<32;++k)miVt[k]=mtraps[k];
      miVt[4]=reinterpret_cast<void*>(&MiGetType); }

    // --- estado global guardado ---
    struct Blob { uintptr_t at; std::vector<uint8> data; };
    std::vector<Blob> blobs;
    auto Save=[&](uintptr_t at,size_t n){ blobs.push_back({at,std::vector<uint8>(reinterpret_cast<uint8*>(at),reinterpret_cast<uint8*>(at)+n)}); };
    Save(MI_PTRS+size_t(MODEL)*4,4); Save(POOLS,POOLS_SIZE); Save(SECTORS,SECTORS_SIZE); Save(REPEAT,REPEAT_SIZE); Save(LODL,LOD_SIZE); Save(MOVING_LIST,4);
    Save(CAM_MIRROR,1); Save(CAM_FRUSTUM,CAM_FRUSTUM_SIZE); Save(BBFAILED,4); Save(WIND,4); Save(WINDDIR,12); Save(PAUSE,2); Save(STEPS,8); Save(TIMEMS,4);
    Save(OCC_COUNT,4); Save(OCC_ARR,OCC_SIZE); Save(SPECIAL_COL,SPECIAL_SIZE);
    auto RestoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size()); };
    auto Restored=[&]{ bool ok=true; for(auto& b:blobs)ok&=std::memcmp(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size())==0; return ok; };

    // pools de nodos locales: cabecera construida una sola vez; pools de entidades falsos que cubren el bloque de entidades
    new (dblHdr.data()) CPtrNodeDoubleLinkPool(NDBL,dblStore.data(),dblStates.data()); dblImg=dblHdr;
    new (sglHdr.data()) CPtrNodeSingleLinkPool(NSGL,sglStore.data(),sglStates.data()); sglImg=sglHdr;
    auto PoolHdr=[&](std::array<uint8,0x14>& h,size_t stride){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=entMem.data(); At<void*>(h.data(),4)=entStates.data();
                                                             At<int32>(h.data(),8)=int32(entMem.size()/stride); At<int32>(h.data(),0xC)=-1; };
    // la capacidad se calcula con el paso mas pequeno (0x38): cubre todo entMem sea cual sea el tamano real del tipo almacenado
    PoolHdr(vehHdr,0x38); PoolHdr(pedHdr,0x38); PoolHdr(objHdr,0x38); PoolHdr(bldHdr,0x38); PoolHdr(dumHdr,0x38);
    auto* const poolPtrs=reinterpret_cast<void**>(POOLS); // 0 simple, 1 doble, 2 entry, 3 peatones, 4 vehiculos, 5 edificios, 6 objetos, 7 dummies

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches; std::vector<uintptr_t> seen;
        auto add=[&](uintptr_t exeAddr,auto fake,uintptr_t dllSym){
            for(uintptr_t at:{exeAddr,dllSym ? Body(dllSym) : uintptr_t(0)}) { if(!at||std::find(seen.begin(),seen.end(),at)!=seen.end())continue; seen.push_back(at); patches.push_back(std::make_unique<EntryPatch>(at,fake)); }
        };
        add(0x54F560,AllocStub,bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix)); add(0x411990,GetMatrixStub,bit_cast<uintptr_t>(&CPlaceable::GetMatrix));
        add(0x7F0910,FrameUpdStub,0); add(0x734820,FirstAtomicStub,bit_cast<uintptr_t>(&GetFirstAtomic)); add(0x7C7550,SkinStub,0);
        add(0x734A40,HierStub,bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump)); add(0x7C51D0,HierUpdStub,0);
        add(0x4C4BA0,AddRefStub,bit_cast<uintptr_t>(&CBaseModelInfo::AddRef)); add(0x4C4BB0,RemoveRefStub,bit_cast<uintptr_t>(&CBaseModelInfo::RemoveRef));
        add(0x409650,StreamAddStub,bit_cast<uintptr_t>(&CStreaming::AddEntity)); add(0x409710,StreamRemoveStub,bit_cast<uintptr_t>(&CStreaming::RemoveEntity));
        add(0x533790,CreateFxStub,bit_cast<uintptr_t>(&CEntity::CreateEffects)); add(0x533BF0,DestroyFxStub,bit_cast<uintptr_t>(&CEntity::DestroyEffects));
        add(0x542800,MovingListStub,bit_cast<uintptr_t>(&CPhysical::AddToMovingList));
        add(0x420C40,SphereStub,bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible)));
        add(0x4D15E0,FirstAssocStub,bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*)>(&RpAnimBlendClumpGetFirstAssociation)));
        add(0x4D34F0,UpdateAnimsStub,bit_cast<uintptr_t>(&RpAnimBlendClumpUpdateAnimations));
        add(0x72C950,WindModStub,bit_cast<uintptr_t>(&CWindModifiers::FindWindModifier));
        add(0x71DA00,ScreenStub4,bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CVector&,float&,float&)>(&CalcScreenCoors)));
        add(0x71DAB0,ScreenStub2,bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CVector&)>(&CalcScreenCoors)));
        add(0x71E580,OccAreaStub,bit_cast<uintptr_t>(&CActiveOccluder::IsPointWithinOcclusionArea));
        add(0x71FA40,OccBehindStub,bit_cast<uintptr_t>(&CActiveOccluder::IsPointBehindOccluder));

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                // valor "interesante": 0, +-1, pequeno, o uniforme
                auto sp=[&](float lo,float hi){ const uint32 m=next()%10; return m==0 ? 0.f : m==1 ? 1.f : m==2 ? -1.f : m==3 ? uni(-0.01f,0.01f) : uni(lo,hi); };
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3A3Au; if(!st)st=1; sst=st^0xA5A5A5A5u; if(!sst)sst=7;
                    trace=json::array();
                    // ---------- mundo y pools (identicos en las dos ejecuciones) ----------
                    RestoreAll();
                    std::memcpy(dblHdr.data(),dblImg.data(),dblImg.size()); dblStore.fill(0xFD); dblStates.fill(0x80);
                    std::memcpy(sglHdr.data(),sglImg.data(),sglImg.size()); sglStore.fill(0xFD); sglStates.fill(0x80);
                    entStates.fill(0);
                    poolPtrs[0]=sglHdr.data(); poolPtrs[1]=dblHdr.data(); poolPtrs[3]=pedHdr.data(); poolPtrs[4]=vehHdr.data(); poolPtrs[5]=bldHdr.data(); poolPtrs[6]=objHdr.data(); poolPtrs[7]=dumHdr.data();
                    std::memset(reinterpret_cast<void*>(SECTORS),0,SECTORS_SIZE); std::memset(reinterpret_cast<void*>(REPEAT),0,REPEAT_SIZE); std::memset(reinterpret_cast<void*>(LODL),0,LOD_SIZE);
                    AtA<void*>(MOVING_LIST)=nullptr;
                    // ---------- estado global por caso ----------
                    AtA<uint8>(PAUSE)=0; AtA<uint8>(PAUSE+1)=0; if(next()%12==0){ if(next()%2)AtA<uint8>(PAUSE)=1; else AtA<uint8>(PAUSE+1)=1; }
                    AtA<uint32>(TIMEMS)=next()%8==0 ? uint32(next()&0xFFFF) : next();
                    AtA<float>(STEPS)=next()%4 ? uni(0.05f,4.f) : 1.f; AtA<float>(STEPS+4)=next()%4 ? uni(0.05f,4.f) : 1.f;
                    { static const float winds[]{0.f,0.05f,0.099999994f,0.1f,0.19999999f,0.2f,0.3f,0.49999997f,0.5f,0.7f,0.79999995f,0.8f,1.f,1.2f,-0.1f};
                      AtA<float>(WIND)=next()%2 ? winds[next()%std::size(winds)] : uni(0.f,1.3f); }
                    AtA<float>(WINDDIR)=sp(-3.f,3.f); AtA<float>(WINDDIR+4)=sp(-3.f,3.f); AtA<float>(WINDDIR+8)=sp(-3.f,3.f);
                    AtA<uint8>(CAM_MIRROR)=uint8(next()%3==0);
                    miType=(next()%4==0) ? int(1+next()%8) : 5;
                    atomOn=next()%4!=0; skinOn=next()%4!=0; assocOn=next()%4!=0;
                    sphPct=next()%5==0 ? uint32(next()%101) : uint32(next()%2 ? 0 : 100); sphSeed=next(); scrSeed=next(); occSeed=next(); windSeed=next();
                    occAreaPct=next()%4==0 ? uint32(next()%101) : 92; occBehindPct=next()%4==0 ? uint32(next()%101) : 92;
                    AtA<int32>(BBFAILED)=int32(next()%1000);
                    // ---------- buffers ----------
                    entMem.fill(0xCD);
                    for(auto& m:mats){ std::memset(m.data(),0,m.size()); }
                    for(auto* b:{&frameBuf,&clumpBuf,&atomObj,&atomBuf,&geomBuf,&hierBuf,&skinBuf,&miBuf,&linkBuf,&assocBuf,&outBuf})for(auto& x:*b)x=uint8(next());
                    clumpBuf[0]=2; clumpBuf[1]=0; At<void*>(clumpBuf.data(),4)=frameBuf.data();     // rpCLUMP con su marco
                    atomObj[0]=1; atomObj[1]=0; At<void*>(atomObj.data(),4)=frameBuf.data();        // rpATOMIC con su marco
                    At<void*>(atomBuf.data(),0x18)=geomBuf.data();
                    // marco: matriz de modelado en +0x10
                    for(size_t k=0;k<12;++k)At<float>(frameBuf.data(),0x10+size_t(k/3)*0x10+size_t(k%3)*4)=uni(-2.f,2.f);
                    // modelo de colision falso (+0 min, +0xC max, +0x18 centro, +0x24 radio)
                    std::memset(colBuf.data(),0,colBuf.size());
                    { CVector mn{-uni(0.05f,6.f),-uni(0.05f,6.f),-uni(0.05f,4.f)}, mx{uni(0.05f,6.f),uni(0.05f,6.f),uni(0.05f,4.f)};
                      const uint32 shape=next()%8;
                      if(shape==0){ mx=CVector{-mn.x,-mn.y,-mn.z}; }               // simetrico: empates en max(-min, max)
                      else if(shape==1){ mn=CVector{-uni(0.f,80.f),-uni(0.f,80.f),-uni(0.f,40.f)}; mx=CVector{uni(0.f,80.f),uni(0.f,80.f),uni(0.f,40.f)}; } // cajas grandes
                      else if(shape==2){ mx.x=mn.x; }
                      else if(shape==3){ mn.z=0.f; }
                      At<CVector>(colBuf.data(),0)=mn; At<CVector>(colBuf.data(),0xC)=mx;
                      At<CVector>(colBuf.data(),0x18)=CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)}; At<float>(colBuf.data(),0x24)=uni(0.3f,15.f); }
                    // modelos de colision especiales de vehiculo (primeras 4 entradas reales): solo se leen los limites
                    for(int k=0;k<4;++k){ uint8* sc=reinterpret_cast<uint8*>(SPECIAL_COL)+k*0x30; At<CVector>(sc,0)=CVector{-uni(0.1f,5.f),-uni(0.1f,5.f),-uni(0.1f,3.f)}; At<CVector>(sc,0xC)=CVector{uni(0.1f,5.f),uni(0.1f,5.f),uni(0.1f,3.f)}; }
                    // modelo FALSO en el indice del entity
                    At<void*>(miBuf.data(),0)=miVt.data(); AtA<void*>(MI_PTRS+size_t(MODEL)*4)=miBuf.data(); At<void*>(miBuf.data(),0x14)=colBuf.data();
                    // marco de camara (normales de frustum, desplazamientos y espejo)
                    for(size_t k=0;k<8;++k)At<float>(reinterpret_cast<void*>(CAM_FRUSTUM),k*4)=0.f;
                    for(int c=0;c<2;++c){ // normales principales (+0x00) y de espejo (+0x30), desplazamientos (+0x60) y de espejo (+0x70)
                        CVector n{sp(-1.f,1.f),sp(-1.f,1.f),sp(-1.f,1.f)}; CVector nm2{sp(-1.f,1.f),sp(-1.f,1.f),sp(-1.f,1.f)};
                        if(next()%6==0)n.x=-0.f; if(next()%6==0)nm2.y=-0.f;
                        At<CVector>(reinterpret_cast<void*>(CAM_FRUSTUM),size_t(c)*12)=n; At<CVector>(reinterpret_cast<void*>(CAM_FRUSTUM),0x30+size_t(c)*12)=nm2;
                        At<float>(reinterpret_cast<void*>(CAM_FRUSTUM),0x60+size_t(c)*4)=sp(-30.f,30.f); At<float>(reinterpret_cast<void*>(CAM_FRUSTUM),0x70+size_t(c)*4)=sp(-30.f,30.f); }
                    // oclusores activos (solo las 4 primeras entradas se tocan)
                    { const uint32 no=next()%5; AtA<uint32>(OCC_COUNT)=no<4 ? no : 4;
                      for(uint32 k=0;k<4;++k){ uint8* o=reinterpret_cast<uint8*>(OCC_ARR)+k*0xAC; std::memset(o,0,0xAC);
                          const uint32 m=next()%6; At<uint16>(o,0x78)= m==0 ? uint16(0x8000+next()%0x100) : m==1 ? uint16(0xFFFF-next()%8) : uint16(next()%200); } }

                    // ---------- entidades ----------
                    auto FillMat=[&](void* m,const CVector& pos){
                        auto unit=[&]{ CVector v{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float l=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); return l<0.1f ? CVector{1.f,0.f,0.f} : CVector{v.x/l,v.y/l,v.z/l}; };
                        auto cross=[&](const CVector& a,const CVector& b){ return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; };
                        CVector r{1.f,0.f,0.f}, f{0.f,1.f,0.f}, u{0.f,0.f,1.f};
                        const uint32 kind=next()%5;
                        if(kind==0){ const float h=uni(-3.2f,3.2f), c=std::cos(h), s=std::sin(h); r=CVector{c,s,0.f}; f=CVector{-s,c,0.f}; }
                        else if(kind!=1){ r=unit(); CVector u0=unit(); f=cross(u0,r); const float l=std::sqrt(f.x*f.x+f.y*f.y+f.z*f.z); f=l<0.05f ? CVector{0.f,1.f,0.f} : CVector{f.x/l,f.y/l,f.z/l}; u=cross(r,f); }
                        if(next()%3==0){ const float sx=uni(0.2f,3.f), sy=uni(0.2f,3.f), sz=uni(0.2f,3.f); r=CVector{r.x*sx,r.y*sx,r.z*sx}; f=CVector{f.x*sy,f.y*sy,f.z*sy}; u=CVector{u.x*sz,u.y*sz,u.z*sz}; }
                        if(next()%11==0){ u.x=0.f; u.y=0.f; }  // eje Z sin componente XY: normalizacion degenerada
                        std::memset(m,0,0x48);
                        At<CVector>(m,0)=r; At<CVector>(m,0x10)=f; At<CVector>(m,0x20)=u; At<CVector>(m,0x30)=pos;
                        for(size_t o:{size_t(0xC),size_t(0x1C),size_t(0x2C),size_t(0x3C)})At<uint32>(m,o)=next();
                    };
                    auto RandType=[&]{ return int(1+next()%5); };
                    // inicializa la entidad k: tipo, banderas, posicion compacta o matriz, modelo, enlaces
                    auto InitEnt=[&](int k,int type,bool hasMat){
                        uint8* e=E(k); std::memset(e,0xCD,ES);
                        At<void*>(e,0)=fvt.data(); At<void*>(e,0x14)=nullptr; At<void*>(e,0x18)=nullptr;
                        At<uint32>(e,0x1C)=next(); At<uint16>(e,0x20)=uint16(next()); At<int16>(e,0x22)=int16(MODEL); At<void*>(e,0x24)=nullptr;
                        At<void*>(e,0x28)=next()%2 ? linkBuf.data()+0x20 : nullptr; At<uint8>(e,0x36)=uint8(type|((next()%32)<<3));
                        At<void*>(e,0xB4)=nullptr; At<int8>(e,0x48B)=int8(-1); At<uint8>(e,0x13C)=uint8(next()%2 ? 4 : next()%8);
                        const float range=next()%4==0 ? 3000.f : 300.f;
                        const CVector p{uni(-range,range),uni(-range,range),uni(-20.f,120.f)};
                        FillMat(mats[size_t(k)].data(),p);
                        if(hasMat)At<void*>(e,0x14)=mats[size_t(k)].data();
                        else { At<CVector>(e,4)=p; At<float>(e,0x10)=next()%5==0 ? 0.f : uni(-7.f,7.f); }
                    };
                    FillMat(mats[NE].data(),CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-20.f,120.f)}); FillMat(mats[NE+1].data(),CVector{0.f,0.f,0.f});
                    const bool hasMat=next()%3!=0;
                    int type=RandType();
                    InitEnt(0,type,hasMat);
                    for(int k=1;k<NE;++k)InitEnt(k,RandType(),true);
                    uint8* const self=E(0);
                    // banderas: visible (0x80), BIG (0x100), fuera de pantalla (0x20000), no actualizar jerarquia (0x800000), esperando colision (0x40000)
                    auto SetFlag=[&](uint32 mask,uint32 chance){ At<uint32>(self,0x1C)=(At<uint32>(self,0x1C)&~mask)|(next()%100<chance ? mask : 0u); };
                    // informacion del modelo: banderas (carretera 0x100, tipo especial 0x7800) y tipo (miType)
                    auto SetMi=[&](bool road,uint32 special){ At<uint16>(miBuf.data(),0x12)=uint16((next()&0x80FF)|(road ? 0x100u : 0u)|(special<<11)); };
                    SetMi(next()%3==0,next()%16);
                    auto Rwobj=[&](uint32 pClump,uint32 pAtom){ const uint32 m=next()%100; At<void*>(self,0x18)= m<pClump ? static_cast<void*>(clumpBuf.data()) : m<pClump+pAtom ? static_cast<void*>(atomObj.data()) : nullptr; };
                    SetFlag(0x80,85);

                    // ---------- fixture por funcion ----------
                    uintptr_t target=fn.address; // las virtuales leen la ranura DESPUES de h->State(candidate)
                    json ret=nullptr; std::string f80; bool compareOut=false;
                    std::function<void()> call;
                    // rectangulo inyectado (orden de memoria: +0 izquierda, +4 borde "top", +8 derecha, +0xC borde "bottom")
                    auto GenRect=[&](bool lod){
                        auto Sx=[](float v){ return int(std::floor(v*0.02f+60.f)); };
                        const uint32 m=next()%8;
                        static const float edges[]{-3000.f,-2999.99f,-2950.f,-2900.f,2950.f,2999.f,3000.f,2999.9f,-3000.01f,-3100.f,3100.f,0.f,50.f,-50.f,100.f,2900.f,2999.99f,3000.01f};
                        auto Edge=[&]{ float v=next()%3==0 ? edges[next()%std::size(edges)] : float(int(next()%121)*50-3000); const uint32 j=next()%4; if(j==0)v=std::nextafter(v,1e9f); else if(j==1)v=std::nextafter(v,-1e9f); return v; };
                        if(m==0){ rc[0]=Edge(); rc[3]=Edge(); rc[2]=Edge(); rc[1]=Edge(); }
                        else if(m==1){ const float cx=Edge(), cy=Edge(), hx=float(1<<(next()%7)), hy=float(1<<(next()%7)); rc[0]=cx-hx; rc[2]=cx+hx; rc[3]=cy-hy; rc[1]=cy+hy; }
                        else { const float cx=uni(-3200.f,3200.f), cy=uni(-3200.f,3200.f), hx=next()%8==0 ? uni(50.f,600.f) : uni(0.5f,150.f), hy=next()%8==0 ? uni(50.f,600.f) : uni(0.5f,150.f);
                               rc[0]=cx-hx; rc[2]=cx+hx; rc[3]=cy-hy; rc[1]=cy+hy; }
                        if(lod){ rc[0]=std::clamp(rc[0],-3600.f,2990.f); rc[3]=std::clamp(rc[3],-3600.f,2990.f); if(rc[1]==3000.f)rc[1]=2999.5f; }
                        // maximo ~700 celdas por caso (los pools de nodos son pequenos)
                        auto Cells=[&]{ const long long w=static_cast<long long>(Sx(std::clamp(rc[2],-3600.f,3600.f)))-Sx(std::clamp(rc[0],-3600.f,3600.f))+1, h=static_cast<long long>(Sx(std::clamp(rc[1],-3600.f,3600.f)))-Sx(std::clamp(rc[3],-3600.f,3600.f))+1; return (w>0&&h>0) ? w*h : 0ll; };
                        for(int t=0;t<8&&Cells()>700;++t){ const float cx=(rc[0]+rc[2])*0.5f, cy=(rc[1]+rc[3])*0.5f, hx=(rc[2]-rc[0])*0.25f, hy=(rc[1]-rc[3])*0.25f; rc[0]=cx-hx; rc[2]=cx+hx; rc[3]=cy-hy; rc[1]=cy+hy; }
                    };
                    // rellena las listas de las celdas cubiertas con nodos de la entidad y de otras (para Remove)
                    auto PopulateWorld=[&](bool lod,bool ownerPresent){
                        auto Sx=[](float v){ return int(std::floor(v*0.02f+60.f)); };
                        float l=rc[0], t=rc[1], r=rc[2], b=rc[3];
                        if(l<-3000.f)l=-3000.f; if(r>=3000.f)r=2999.f; if(b<-3000.f)b=-3000.f; if(t>3000.f)t=2999.f;
                        uint8* others[]{E(1),E(2),E(3)};
                        auto Pick=[&]()->uint8*{ const uint32 m=next()%6; if(m<2&&ownerPresent)return self; if(m<5)return others[next()%3]; return nullptr; };
                        if(lod){
                            const int x0=int(l*0.005f+15.f), x1=int(r*0.005f+15.f), y0=int(b*0.005f+15.f), y1=int(t*0.005f+15.f);
                            for(int y=std::max(y0,0);y<=std::min(y1,29);++y)for(int x=std::max(x0,0);x<=std::min(x1,29);++x){
                                auto& list=*(reinterpret_cast<CPtrListSingleLink<CEntity*>*>(LODL)+(y*30+x));
                                for(int n=0;n<3;++n){ uint8* e=Pick(); if(e)list.AddItem(reinterpret_cast<CEntity*>(e)); } }
                        } else {
                            const int x0=Sx(l), x1=Sx(r), y0=Sx(b), y1=Sx(t);
                            for(int y=std::max(y0,0);y<=std::min(y1,125);++y)for(int x=std::max(x0,0);x<=std::min(x1,125);++x){
                                const int cx=std::clamp(x,0,119), cy=std::clamp(y,0,119);
                                for(int n=0;n<3;++n){ uint8* e=Pick(); if(!e)continue;
                                    switch(At<uint8>(self,0x36)&7){
                                    case ENTITY_TYPE_BUILDING: CWorld::GetSector(cx,cy).Buildings.AddItem(reinterpret_cast<CBuilding*>(e)); break;
                                    case ENTITY_TYPE_DUMMY:    CWorld::GetSector(cx,cy).Dummies.AddItem(reinterpret_cast<CDummy*>(e)); break;
                                    case ENTITY_TYPE_VEHICLE:  CWorld::ms_aRepeatSectors[y&15][x&15].Vehicles.AddItem(reinterpret_cast<CVehicle*>(e)); break;
                                    case ENTITY_TYPE_PED:      CWorld::ms_aRepeatSectors[y&15][x&15].Peds.AddItem(reinterpret_cast<CPed*>(e)); break;
                                    default:                   CWorld::ms_aRepeatSectors[y&15][x&15].Objects.AddItem(reinterpret_cast<CObject*>(e)); break;
                                    } } }
                        }
                    };
                    const std::string nmf=nm;
                    if(nmf=="AttachToRwObject"){
                        SetFlag(0x80,85); SetMi(next()%2==0,next()%16);
                        RwObject* obj=next()%7==0 ? nullptr : (next()%3==0 ? reinterpret_cast<RwObject*>(atomObj.data()) : reinterpret_cast<RwObject*>(clumpBuf.data()));
                        const uint32 upd=next()%2;
                        if(type==ENTITY_TYPE_OBJECT){ if(next()%2)At<void*>(self,0xB4)=linkBuf.data()+0x50; SetFlag(0x40000,25); }
                        // la lista de entidades en movimiento ya tiene nodos de otras entidades fisicas a veces
                        for(int n=int(next()%3);n>0;--n)CWorld::ms_listMovingEntityPtrs.AddItem(reinterpret_cast<CPhysical*>(E(1+int(next()%3))));
                        call=[&,obj,upd]{ reinterpret_cast<void(__thiscall*)(CEntity*,RwObject*,uint32)>(target)(reinterpret_cast<CEntity*>(self),obj,upd); };
                    }
                    else if(nmf=="DetachFromRwObject"){
                        Rwobj(85,0); SetMi(next()%2==0,next()%16);
                        for(int n=int(next()%3);n>0;--n)CWorld::ms_listMovingEntityPtrs.AddItem(reinterpret_cast<CPhysical*>(E(1+int(next()%3))));
                        if(next()%2)CWorld::ms_listMovingEntityPtrs.AddItem(reinterpret_cast<CPhysical*>(self));
                        for(int n=int(next()%2);n>0;--n)CWorld::ms_listMovingEntityPtrs.AddItem(reinterpret_cast<CPhysical*>(E(1+int(next()%3))));
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="BuildWindSockMatrix"){
                        Rwobj(85,0);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="CalculateBBProjection"){
                        compareOut=true; for(auto& x:outBuf)x=0xCD;
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*,CVector*,CVector*,CVector*,CVector*)>(target)(reinterpret_cast<CEntity*>(self),
                            reinterpret_cast<CVector*>(outBuf.data()),reinterpret_cast<CVector*>(outBuf.data()+0x10),reinterpret_cast<CVector*>(outBuf.data()+0x20),reinterpret_cast<CVector*>(outBuf.data()+0x30)); };
                    }
                    else if(nmf=="ModifyMatrixForBannerInWind"||nmf=="ModifyMatrixForCrane"||nmf=="ModifyMatrixForTreeInWind"){
                        Rwobj(nmf=="ModifyMatrixForBannerInWind" ? 100 : 85,0);
                        if(nmf=="ModifyMatrixForTreeInWind"){ SetMi(false,next()%2 ? 2u : next()%4); }
                        if(nmf=="ModifyMatrixForBannerInWind"){ // posicion con valores grandes y de signo variable (trunc(y + x) de 64 bits)
                            const CVector p{uni(-20000.f,20000.f),uni(-20000.f,20000.f),uni(-20.f,120.f)};
                            if(hasMat)At<CVector>(mats[0].data(),0x30)=p; else At<CVector>(self,4)=p; }
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="GetBoundCentre-ref"||nmf=="GetBoundCentre-vec"){
                        compareOut=true; for(auto& x:outBuf)x=0xCD;
                        if(nmf=="GetBoundCentre-ref") call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*,CVector*)>(target)(reinterpret_cast<CEntity*>(self),reinterpret_cast<CVector*>(outBuf.data())); };
                        else call=[&]{ ret=Name(reinterpret_cast<void*>(reinterpret_cast<CVector*(__thiscall*)(CEntity*,CVector*)>(target)(reinterpret_cast<CEntity*>(self),reinterpret_cast<CVector*>(outBuf.data())))); };
                    }
                    else if(nmf=="GetBoundRect"){
                        compareOut=true; for(auto& x:outBuf)x=0xCD;
                        call=[&]{ ret=Name(reinterpret_cast<void*>(reinterpret_cast<CRect*(__thiscall*)(CEntity*,CRect*)>(target)(reinterpret_cast<CEntity*>(self),reinterpret_cast<CRect*>(outBuf.data())))); };
                    }
                    else if(nmf=="GetColModel"||nmf=="GetDistanceFromCentreOfMassToBaseOfModel"){
                        if(next()%2){ At<uint8>(self,0x36)=uint8((At<uint8>(self,0x36)&~7)|ENTITY_TYPE_VEHICLE); type=ENTITY_TYPE_VEHICLE; }
                        { const uint32 m=next()%8; At<int8>(self,0x48B)=int8(m<3 ? -1 : m<7 ? int(next()%4) : int(next()%128)); if(nmf!="GetColModel"&&At<int8>(self,0x48B)>3)At<int8>(self,0x48B)=int8(next()%4); }
                        if(nmf=="GetColModel") call=[&]{ ret=Name(reinterpret_cast<void*>(reinterpret_cast<CColModel*(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)))); };
                        else call=[&]{ f80=CallThisF0(target,self); };
                    }
                    else if(nmf=="GetModellingMatrix"){
                        Rwobj(60,30);
                        call=[&]{ ret=Name(reinterpret_cast<void*>(reinterpret_cast<RwMatrix*(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)))); };
                    }
                    else if(nmf=="TransformFromObjectSpace-ptr"||nmf=="TransformFromObjectSpace-ref"){
                        compareOut=true; for(auto& x:outBuf)x=0xCD;
                        At<CVector>(outBuf.data(),0x20)=CVector{sp(-30.f,30.f),sp(-30.f,30.f),sp(-30.f,30.f)}; // vector de entrada
                        // el EXE de la variante "ref" deja en EAX basura (el ultimo float copiado) y no el puntero de retorno: solo se compara el de "ptr"
                        const bool cmpRet=nmf=="TransformFromObjectSpace-ptr";
                        call=[&,cmpRet]{ const auto* r=reinterpret_cast<CVector*(__thiscall*)(CEntity*,CVector*,const CVector*)>(target)(reinterpret_cast<CEntity*>(self),
                            reinterpret_cast<CVector*>(outBuf.data()),reinterpret_cast<const CVector*>(outBuf.data()+0x20)); if(cmpRet)ret=Name(r); };
                    }
                    else if(nmf=="UpdateRwFrame"){
                        Rwobj(50,30);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="UpdateRwMatrix"){
                        Rwobj(60,30);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="UpdateRpHAnim"){
                        At<void*>(self,0x18)=clumpBuf.data(); SetFlag(0x800000,30);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="UpdateAnim"){
                        Rwobj(75,12); SetFlag(0x20000,30); SetFlag(0x800000,50);
                        if(type!=ENTITY_TYPE_OBJECT&&next()%4==0){ At<uint8>(self,0x36)=uint8((At<uint8>(self,0x36)&~7)|ENTITY_TYPE_OBJECT); type=ENTITY_TYPE_OBJECT; }
                        if(type==ENTITY_TYPE_OBJECT)At<uint8>(self,0x13C)=uint8(next()%2 ? 4 : next()%8);
                        call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                    }
                    else if(nmf=="Add-rect"||nmf=="Remove"){
                        const bool lod=next()%4==0; SetFlag(0x100,0); if(lod)At<uint32>(self,0x1C)|=0x100u;
                        GenRect(lod);
                        if(nmf=="Remove")PopulateWorld(lod,next()%5!=0);
                        if(nmf=="Remove") call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self)); };
                        else call=[&]{ reinterpret_cast<void(__thiscall*)(CEntity*,const CRect*)>(target)(reinterpret_cast<CEntity*>(self),reinterpret_cast<const CRect*>(rc)); };
                        if(nmf=="Add-rect"){ PopulateWorld(lod,false); }
                    }
                    else if(nmf=="LivesInThisNonOverlapSector"){
                        GenRect(false);
                        auto Sx=[](float v){ return int(std::floor(v*0.02f+60.f)); };
                        const int cx=Sx((rc[2]+rc[0])*0.5f), cy=Sx((rc[1]+rc[3])*0.5f);
                        const int ax=next()%3==0 ? cx+int(next()%3)-1 : cx, ay=next()%3==0 ? cy+int(next()%3)-1 : cy;
                        const int bx=next()%7==0 ? int(next()%130) : ax, by=next()%7==0 ? int(next()%130) : ay;
                        call=[&,bx,by]{ ret=int(reinterpret_cast<uint32(__thiscall*)(CEntity*,int32,int32)>(target)(reinterpret_cast<CEntity*>(self),bx,by)&0xFF); };
                    }
                    else if(nmf=="GetIsBoundingBoxOnScreen"||nmf=="GetIsOnScreen"||nmf=="IsEntityOccluded"||nmf=="IsVisible"){
                        if(nmf=="IsVisible"){ Rwobj(85,10); SetFlag(0x80,80); }
                        call=[&]{ ret=int(reinterpret_cast<uint32(__thiscall*)(CEntity*)>(target)(reinterpret_cast<CEntity*>(self))&0xFF); };
                    }

                    // ---------- ejecucion ----------
                    setState(candidate);
                    bool switches{};
                    if(fn.slot>=0){ target=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=(target>=0x400000&&target<0x1000000)!=candidate; }
                    else switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    call();
                    setState(false);
                    auto Mask=[&](const std::array<uint8,0x100>& b,size_t base){ auto c=b; for(size_t o:{size_t(0xC),size_t(0x1C),size_t(0x2C),size_t(0x3C)})std::memset(c.data()+base+o,0,4); return c; };
                    json mh=json::array(); for(auto& m:mats)mh.push_back(Hash(MaskMat(m,0).data(),0x48));
                    json r={{"ret",ret},{"f80",f80},{"ent",Hash(E(0),ES)},{"ents",{Hash(E(1),ES),Hash(E(2),ES),Hash(E(3),ES)}},{"mats",mh},
                            {"frame",Hash(Mask(frameBuf,0x10).data(),frameBuf.size())},
                            {"bufs",{Hash(clumpBuf.data(),clumpBuf.size()),Hash(atomObj.data(),atomObj.size()),Hash(atomBuf.data(),atomBuf.size()),Hash(geomBuf.data(),geomBuf.size()),
                                     Hash(skinBuf.data(),skinBuf.size()),Hash(hierBuf.data(),hierBuf.size()),Hash(miBuf.data(),miBuf.size()),Hash(colBuf.data(),colBuf.size()),
                                     Hash(linkBuf.data(),linkBuf.size()),Hash(assocBuf.data(),assocBuf.size())}},
                            {"out",compareOut ? json(Hash(outBuf.data(),outBuf.size())) : json(nullptr)},
                            {"out_words",compareOut ? json(Hash(outBuf.data(),0x40)) : json(nullptr)},
                            {"world",{Hash(reinterpret_cast<void*>(SECTORS),SECTORS_SIZE),Hash(reinterpret_cast<void*>(REPEAT),REPEAT_SIZE),Hash(reinterpret_cast<void*>(LODL),LOD_SIZE),
                                      Hash(dblStore.data(),dblStore.size()),Hash(dblStates.data(),dblStates.size()),Hash(dblHdr.data(),dblHdr.size()),
                                      Hash(sglStore.data(),sglStore.size()),Hash(sglStates.data(),sglStates.size()),Hash(sglHdr.data(),sglHdr.size())}},
                            {"moving",Name(AtA<void*>(MOVING_LIST))},{"bb_failed",AtA<int32>(BBFAILED)},
                            {"special",Hash(reinterpret_cast<void*>(SPECIAL_COL),SPECIAL_SIZE)},{"occ",Hash(reinterpret_cast<void*>(OCC_ARR),OCC_SIZE)},
                            {"trace",trace},{"switches",switches}};
                    RestoreAll();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"].is_null() ? original["f80"] : original["ret"]},{"candidate",candidate["ret"].is_null() ? candidate["f80"] : candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    const bool intactBefore=Restored();
    RestoreAll();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},{"state_restored",Restored()}});
}
}
