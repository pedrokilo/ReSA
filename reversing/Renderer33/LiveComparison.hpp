#pragma once
// Banco dentro del motor: CRenderer (renderizador, lista de 33: 33 funciones). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas e identicos. Nunca se renderiza ni se tocan sectores/entidades reales:
// - entidades falsas (vtable trazada: CreateRwObject, DeleteRwObject, PreRender, Render, SetupLighting,
//   RemoveLighting) en un bloque fijo; model infos falsas (vtable: AsAtomicModelInfoPtr, GetModelType, GetTimeInfo)
//   colocadas en ms_modelInfoPtrs para 7 ids especiales (0, 430, 432, 437, 453, 454, 460) y 10 ids libres;
// - listas de sectores, sectores repetidos y listas LOD falsas (un bloque de 8x8 sectores, los 256 sectores
//   repetidos y las 900 listas LOD se vacian y se rellenan). Los nodos son de pools de nodos FALSOS y los punteros de
//   los pools de entidades y de nodos se sustituyen por pools falsos que cubren las entidades y nodos falsos (la DLL de
//   depuracion valida cada nodo y su Item contra esos pools); nada mas usa los pools durante el caso;
// - RwCamera, marco, matriz, mundo, clumps y listas alpha (enlaces) falsos; TheCamera y Scene se modifican y restauran.
// Se simulan y trazan (EXE + cuerpo de la DLL; los cuerpos se resuelven antes de parchear y nunca se parchea un stub):
// RwRenderStateSet (puntero de RwEngineInstance), RwCameraBegin/EndUpdate, RwV3dTransformPoints, CWorldScan::ScanWorld,
// luces (DeActivateDirectional, SetAmbientColours, WorldReplaceNormalLightsWithScorched,
// SetLightColoursForPedsCarsAndObjects, CPointLights), CVisibilityPlugins, CPostEffects, CCarFXRenderer, CVehicle
// (render de ocupantes, SetupRender, ResetAfterRender), CPhysical::GetLightingFromCol, CHeli, CShadows, CClock,
// CEntity::GetIsOnScreen/IsEntityOccluded, CStreaming (RequestModel, StartRenderEntities), CWorld::ClearScanCodes,
// CCamera (CalculateGroundHeight, GetLookDirection), CCullZones, CMirrors, COcclusion y FindPlayerPed/Vehicle.
// La traza es un array fijo (sin reservas durante la llamada) que se convierte a json despues.
// Fuera de los fixtures (desviaciones NOTSA o lecturas sin inicializar del EXE):
// - PreRender: el NOTSA de la DLL tambien acepta CVisibilityPlugins::RenderEntity de la DLL como callback; los enlaces
//   falsos solo usan 0x732B40 y otros valores.
// - ScanSectorList/ScanBigBuildingList: la distancia inicial es basura de pila en el EXE; las entidades que devuelven
//   VISIBLE sin escribir distancia (model info no atomica, atomica con m_bDontStream, o de vehiculo en LOD) llevan
//   m_bDistanceFade (y m_bDrawLast en LOD) a 0 para que AddEntityToRenderList no use esa distancia.
// - RequestObjectsInDirection: los flags y rellenos de la matriz local (sin inicializar en el EXE) no se comparan.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Renderer.h"
#include "VisibilityPlugins.h"
#include "PostEffects.h"
#include "PointLights.h"
#include "CarFXRenderer.h"
#include "Shadows.h"
#include "Clock.h"
#include "Streaming.h"
#include "World.h"
#include "CullZones.h"
#include "Mirrors.h"
#include "Occlusion.h"
#include "Camera.h"
#include "Entity/Vehicle/Heli.h"
#include "../ProbeRuntime.hpp"

namespace Renderer33Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
template<class T> inline T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; const auto* b=static_cast<const uint8*>(p); for(size_t k=0;k<n;++k){h^=b[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// --- direcciones del EXE ---
constexpr uintptr_t MI_PTRS=0xA9B0C8, STREAM_INFO=0x8E4CC0, SCAN_CODE=0xB7CD78, CURR_AREA=0xB72914;
constexpr uintptr_t REN_BEGIN=0xB745C0, REN_END=0xB76898, LOD_SCALES=0x8CD800, PC_SCRATCH=0xC8E0C8, LOD_LIST_BASE=0xC8E0E0, LOD_DONT_BASE=0xC900C8;
constexpr uintptr_t CAMERA=0xB6F028, SCENE=0xC17038, ALPHA=0xC88120, ALPHA_HEAD_NEXT=0xC88130, ALPHA_TAIL=0xC88134, UW_HEAD_NEXT=0xC88188, UW_TAIL=0xC8818C;
constexpr uintptr_t POSTFX_NV=0xC402B8, MIRROR_TYPE=0xC7C724, FIRST_PERSON=0xB6EC20, REPLAY_MODE=0xA43088, DISABLE_STREAMING=0x9654B0, NUM_REQUESTED=0x8E4CB8;
constexpr uintptr_t LOD_DIST_MULT=0xB7C548, PLAYER0_PED=0xB7CD98, ENGINE=0xC97B24, POOLS=0xB74484, SECTORS=0xB7D0B8, REPEAT=0xB992B8, LOD_LISTS=0xB99EB8;
// globales de CRenderer
constexpr uintptr_t R_TUNNELS=0xB745C0, R_OUTSIDE=0xB745C1, R_REQ_FLAGS=0xB745C4, R_LOD_DONT=0xB745CC, R_LOD=0xB745D0, R_FPV=0xB745D4, R_INVIS=0xB745D8,
    R_SUPERLODS=0xB74830, R_LODS=0xB748F8, R_ENTS=0xB75898, R_N_SUPERLODS=0xB76838, R_N_INVIS=0xB7683C, R_N_LODS=0xB76840, R_N_ENTS=0xB76844,
    R_FARCLIP=0xB76848, R_HEADING=0xB7684C, R_PRIORITY=0xB76850, R_SKY=0xB76851, R_GPOUT=0xB76854, R_CAMPOS=0xB76870, R_LASTFWD=0xB7687C, R_LASTPOS=0xB76888;
// desplazamientos dentro de TheCamera
constexpr size_t C_POS=0x04, C_HEADING=0x10, C_MATRIX=0x14, C_TUNNEL_BIG=0x22, C_ACTIVE=0x59, C_LODMULT=0xF0, C_CAM_MODE=0x180, C_CAM_DIR=0x19C, C_RWCAM=0x954, C_FWD=0x984;
constexpr size_t CAM_STRIDE=0x238, CAMERA_SIZE=0xD78;
// entidad
constexpr size_t E_POS=0x04, E_HEADING=0x10, E_MATRIX=0x14, E_RW=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_SCAN=0x2C, E_AREA=0x2F, E_LOD=0x30, E_NLOD=0x34, E_NLODR=0x35, E_TYPE=0x36;
constexpr size_t P_FLAGS=0x40, P_DYNLIGHT=0x130, PED_STATE=0x530, PED_WEAPONS=0x5A0, PED_SLOT=0x718, V_HANDLING=0x384, V_FLAGS5=0x42D, V_TYPE=0x590, B_FLAGS=0x614;
// model info
constexpr size_t M_ALPHA=0x0C, M_FLAGS=0x12, M_COL=0x14, M_DRAW=0x18, M_RW=0x1C, M_TIME=0x20, M_TYPE=0x28, M_HASTIME=0x29, M_ATOMIC=0x2A;

constexpr int NENT=24, NMI=17, NCLUMP=8, NSNODE=192, NDNODE=192, NLINK=12, NOUT=192;
constexpr size_t ENT_SIZE=0xA20, ARENA=size_t(NENT)*ENT_SIZE, POOL_CAP=ARENA/0x38+1;
constexpr uint32 QNAN=0x7FC00000u;
constexpr int SEC_LO=56, SEC_HI=63;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,ARENA> arena{};
alignas(16) inline std::array<std::array<uint8,0x48>,NENT+2> mats{};
alignas(16) inline std::array<std::array<uint8,0x40>,NMI> mis{};
alignas(16) inline std::array<std::array<uint8,0x30>,NMI> cols{};
alignas(16) inline std::array<std::array<uint8,0x20>,NCLUMP> clumps{};
alignas(16) inline std::array<std::array<uint8,0xE0>,4> handlings{};
alignas(16) inline std::array<uint8,0xA0> rwCam{}, sceneCam{};
alignas(16) inline std::array<uint8,0x60> camFrame{};
alignas(16) inline std::array<uint8,0x40> xformMat{}, rpWorld{};
alignas(16) inline std::array<uint32,NSNODE*2> sNodes{};
alignas(16) inline std::array<uint32,NDNODE*3> dNodes{};
alignas(16) inline std::array<std::array<uint8,0x14>,NLINK> links{};
alignas(16) inline std::array<void*,NOUT> outEnts{};
alignas(16) inline std::array<uint32,2> plist{};
alignas(16) inline std::array<std::array<uint8,0x14>,7> poolHdrs{};
alignas(16) inline std::array<uint8,POOL_CAP> entStates{};
alignas(16) inline std::array<uint8,NSNODE> sStates{};
alignas(16) inline std::array<uint8,NDNODE> dStates{};
inline std::array<void*,64> evt{}, mvt{};
inline std::array<int32,NMI> ids{};
inline std::array<bool,NENT> onScreen{}, occluded{}, lightRet{}, createGives{};
inline std::array<uint8,NENT> insertRet{};
inline uint8* Ent(int k){ return arena.data()+size_t(k)*ENT_SIZE; }
inline int EntIdx(const void* p){ const auto u=uintptr_t(p), b=uintptr_t(arena.data()); return u>=b&&u<b+ARENA ? int((u-b)/ENT_SIZE) : -1; }

// --- identificadores sin reservas: region+desplazamiento, pila u otro ---
struct Region { uintptr_t a; size_t n; };
inline std::array<Region,32> regions{}; inline size_t nRegions{};
inline void Reg(const void* p,size_t n){ regions[nRegions++]={uintptr_t(p),n}; }
inline bool OnStack(const void* p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=tib->StackLimit&&p<tib->StackBase; }
inline uint32 IdU(const void* p){ if(!p) return 0xFFFFFFFFu; const auto u=uintptr_t(p);
    for(size_t k=0;k<nRegions;++k) if(u>=regions[k].a&&u<regions[k].a+regions[k].n) return uint32((k+1)<<16|(u-regions[k].a));
    return OnStack(p) ? 0xEEEEEEEEu : 0xDDDDDDDDu; }

// --- registro fijo (sin reservas dentro de los stubs) ---
struct Ev { const char* k; uint32 n; std::array<uint32,56> v; };
inline std::array<Ev,192> evs{}; inline uint32 nev{};
template<class... T> inline void Push(const char* k,T... a){ if(nev<evs.size()){ auto& e=evs[nev]; e.k=k; e.n=0; ((e.v[e.n++]=uint32(a)),...); } ++nev; }
inline void PushArr(const char* k,const uint32* a,uint32 n){ if(nev<evs.size()){ auto& e=evs[nev]; e.k=k; e.n=std::min<uint32>(n,56); for(uint32 i=0;i<e.n;++i)e.v[i]=a[i]; } ++nev; }
inline json Events(){ json t=json::array(); for(uint32 i=0;i<std::min<uint32>(nev,uint32(evs.size()));++i){ json e=json::array({evs[i].k}); for(uint32 j=0;j<evs[i].n;++j)e.push_back(evs[i].v[j]); t.push_back(e); }
    if(nev>evs.size())t.push_back({"overflow",nev}); return t; }

// --- estado de los stubs (fijado por el fixture) ---
inline bool visionFx{}; inline float genLight{}, genTotal{}, colLight{}; inline void* fakePlayerPed{}; inline void* fakePlayerVeh{};
inline uint32 timeMask{}; inline std::array<float,4> groundZ{}; inline int32 lookDir{}, tunnelAttr{}; inline void* focusEnt{};
inline std::array<uintptr_t,10> scanFns{};

// --- stubs: render y luces ---
inline int32 __cdecl FakeRss(int32 st,void* v){ Push("rs_set",st,uint32(uintptr_t(v))); return 1; }
inline void __cdecl FakeDeactDir(){ Push("deactivate_directional"); }
inline void __cdecl FakeSetAmbient(){ Push("set_ambient_colours"); }
inline void __cdecl FakeRenderFading(){ Push("render_fading_entities"); }
inline void __cdecl FakeRenderFadingUw(){ Push("render_fading_underwater_entities"); }
inline bool __cdecl FakeVisionFx(){ Push("is_vision_fx_active"); return visionFx; }
inline void __cdecl FakeStoreDnb(){ Push("filter_fx_store_dnb"); }
inline void __cdecl FakeRestoreDnb(){ Push("filter_fx_restore_dnb"); }
inline void __cdecl FakeNvLights(){ Push("night_vision_set_lights"); }
inline void __cdecl FakeIrLights(){ Push("infrared_set_lights_default"); }
inline void __cdecl FakeSetupVehVars(void* c){ Push("setup_vehicle_variables",IdU(c)); }
inline void __cdecl FakeInitAlphaAtomics(){ Push("init_alpha_atomic_list"); }
inline void __cdecl FakeRenderAlphaAtomics(){ Push("render_alpha_atomics",focusEnt ? At<uint32>(focusEnt,E_FLAGS) : 0u); }
inline void __cdecl FakeInitAlphaEntities(){ Push("init_alpha_entity_list"); }
inline bool __cdecl FakeInsertSorted(void* e,float d){ Push("insert_sorted",IdU(e),F(d)); const int k=EntIdx(e); return k>=0&&(insertRet[k]&1); }
inline bool __cdecl FakeInsertUw(void* e,float d){ Push("insert_underwater",IdU(e),F(d)); const int k=EntIdx(e); return k>=0&&(insertRet[k]&2); }
inline int32 __cdecl FakeClumpAlpha(void* c){ Push("get_clump_alpha",IdU(c)); return c ? At<int32>(c,4) : 255; }
inline void __fastcall FakeRenderDriver(void* v,void*){ Push("render_driver_and_passengers",IdU(v)); }
inline void __fastcall FakeSetupRender(void* v,void*){ Push("vehicle_setup_render",IdU(v)); }
inline void __fastcall FakeResetAfterRender(void* v,void*){ Push("vehicle_reset_after_render",IdU(v)); }
inline void __cdecl FakeRemoveLights(){ Push("remove_lights_affecting_object"); }
inline float __cdecl FakeGenLights(const CVector* p,float* total,void* e){ Push("generate_lights",IdU(p),F(p->x),F(p->y),F(p->z),IdU(total),IdU(e)); *total=genTotal; return genLight; }
inline void __cdecl FakeEnvMapMult(float m){ Push("set_fx_env_map_light_mult",F(m)); }
inline void __cdecl FakeScorched(void* w,float l){ Push("replace_lights_with_scorched",IdU(w),F(l)); }
inline void __cdecl FakeLightColours(float m){ Push("set_light_colours_peds_cars_objects",F(m)); }
inline float __fastcall FakeLightFromCol(void* e,void*,bool b){ Push("get_lighting_from_col",IdU(e),b); return colLight; }
inline void __cdecl FakeHeliPreRender(){ Push("special_heli_prerender"); }
inline void __cdecl FakeExtraShadows(){ Push("render_extra_player_shadows"); }
inline void* __cdecl FakeCamEnd(void* c){ Push("rw_camera_end_update",IdU(c),c ? F(At<float>(c,0x90)) : 0u); return c; }
inline void* __cdecl FakeCamBegin(void* c){ Push("rw_camera_begin_update",IdU(c),c ? F(At<float>(c,0x90)) : 0u); return c; }
// --- stubs: mundo, streaming, camara ---
inline void* __cdecl FakeFindPlayerPed(int32 id){ Push("find_player_ped",id); return fakePlayerPed; }
inline void* __cdecl FakeFindPlayerVehicle(int32 id,bool r){ Push("find_player_vehicle",id,r); return fakePlayerVeh; }
inline bool __cdecl FakeTimeInRange(uint8 a,uint8 b){ Push("time_in_range",a,b); return ((timeMask>>((a*3u+b)&31u))&1u)!=0; }
inline bool __fastcall FakeOnScreen(void* e,void*){ Push("on_screen",IdU(e)); const int k=EntIdx(e); return k>=0&&onScreen[k]; }
inline bool __fastcall FakeOccluded(void* e,void*){ Push("occluded",IdU(e)); const int k=EntIdx(e); return k>=0&&occluded[k]; }
inline void __cdecl FakeRequestModel(int32 m,int32 f){ Push("request_model",m,f); }
inline void __cdecl FakeStartRenderEntities(){ Push("start_render_entities"); }
inline void __cdecl FakeClearScanCodes(){ Push("clear_scan_codes"); }
inline void __cdecl FakeWorldScan(const float* pts,int32 n,void* fn){
    uint32 a[16]{}; uint32 c=0; uint32 fid=99; for(uint32 k=0;k<scanFns.size();++k)if(scanFns[k]&&uintptr_t(fn)==scanFns[k])fid=k%5;
    a[c++]=uint32(n); a[c++]=fid; for(int32 k=0;k<n&&k<6;++k){ a[c++]=F(pts[k*2]); a[c++]=F(pts[k*2+1]); } PushArr("world_scan",a,c); }
inline void* __cdecl FakeTransform(float* out,const float* in,int32 n,const float* m){
    uint32 a[56]{}; uint32 c=0; a[c++]=uint32(n); a[c++]=IdU(m); a[c++]=out==in;
    for(int k:{0,1,2,4,5,6,8,9,10,12,13,14}) a[c++]=F(m[k]); // flags y rellenos fuera (sin inicializar en RequestObjectsInDirection)
    for(int32 k=0;k<n&&k<13;++k){ a[c++]=F(in[k*3]); a[c++]=F(in[k*3+1]); a[c++]=F(in[k*3+2]); }
    PushArr("rw_v3d_transform_points",a,std::min<uint32>(c,56u));
    for(int32 k=0;k<n;++k){ out[k*3]=in[k*3]+m[12]; out[k*3+1]=in[k*3+1]+m[13]; out[k*3+2]=in[k*3+2]+m[14]; }
    return out; }
inline float __fastcall FakeGroundHeight(void* cam,void*,int32 type){ Push("calculate_ground_height",IdU(cam),type); return groundZ[uint32(type)&3u]; }
inline int32 __fastcall FakeLookDir(void* cam,void*){ Push("get_look_direction",IdU(cam)); return lookDir; }
struct V3 { float x,y,z; };
inline int32 __cdecl FakeTunnelAttr(V3 p){ Push("find_tunnel_attributes",F(p.x),F(p.y),F(p.z)); return tunnelAttr; }
inline void __cdecl FakeMirrorsBefore(){ Push("mirrors_before_construct_render_list"); }
inline void __cdecl FakeOcclusionBefore(){ Push("occlusion_process_before_rendering"); }
// --- vtables falsas ---
inline void __fastcall FakeCreateRw(void* e,void*){ Push("create_rw_object",IdU(e)); const int k=EntIdx(e); if(k>=0&&createGives[k])At<void*>(e,E_RW)=clumps[size_t(k)%NCLUMP].data(); }
inline void __fastcall FakeDeleteRw(void* e,void*){ Push("delete_rw_object",IdU(e)); At<void*>(e,E_RW)=nullptr; }
inline void __fastcall FakePreRender(void* e,void*){ Push("pre_render",IdU(e),At<uint32>(e,E_FLAGS)); }
inline void __fastcall FakeRender(void* e,void*){ Push("render",IdU(e),At<uint32>(e,E_FLAGS)); }
inline bool __fastcall FakeSetupLighting(void* e,void*){ Push("setup_lighting",IdU(e)); const int k=EntIdx(e); return k>=0&&lightRet[k]; }
inline void __fastcall FakeRemoveLighting(void* e,void*,bool b){ Push("remove_lighting",IdU(e),b); }
template<int N> void __fastcall EntTrap(void* e,void*){ Push("entity_vt_trap",N,IdU(e)); }
inline void* __fastcall MiAsAtomic(void* m,void*){ return At<uint8>(m,M_ATOMIC) ? m : nullptr; }
inline uint32 __fastcall MiType(void* m,void*){ return At<uint8>(m,M_TYPE); }
// GetRwModelType (slot 9): solo lo llama el assert de CBaseModelInfo::GetRwObject en la DLL de depuracion; sin traza
inline uint32 __fastcall MiRwType(void* m,void*){ auto* rw=At<uint8*>(m,M_RW); return rw ? rw[0] : 0u; }
inline void* __fastcall MiTimeInfo(void* m,void*){ return At<uint8>(m,M_HASTIME) ? static_cast<uint8*>(m)+M_TIME : nullptr; }
template<int N> void* __fastcall MiTrap(void* m,void*){ Push("mi_vt_trap",N,IdU(m)); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((evt[N]=reinterpret_cast<void*>(&EntTrap<N>)),...); ((mvt[N]=reinterpret_cast<void*>(&MiTrap<N>)),...); }

// ======================= hooks y parches =======================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
// cuerpo de una funcion de la DLL: sigue los saltos del ILT mientras no vuelvan al EXE
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class M> inline uintptr_t MAddr(M m) { static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=33;
inline const std::array<Fn,NFNS> fns{{
    {"CRenderer","RenderFadingInEntities",0x5531E0,0,-1,64},{"CRenderer","RenderFadingInUnderwaterEntities",0x553220,0,-1,64},
    {"CRenderer","RenderOneRoad",0x553230,0,-1,256},{"CRenderer","RenderOneNonRoad",0x553260,0,-1,1024},
    {"CRenderer","RemoveVehiclePedLights",0x553390,0,-1,128},{"CRenderer","AddEntityToRenderList",0x5534B0,0,-1,1024},
    {"CRenderer","GetLodRenderListBase",0x5536D0,0,-1,64},{"CRenderer","GetLodDontRenderListBase",0x5536E0,0,-1,64},
    {"CRenderer","ResetLodRenderLists",0x5536F0,0,-1,64},{"CRenderer","AddToLodRenderList",0x553710,0,-1,256},
    {"CRenderer","AddToLodDontRenderList",0x553740,0,-1,256},{"CRenderer","ProcessLodRenderLists",0x553770,0,-1,1024},
    {"CRenderer","PreRender",0x553910,0,-1,512},{"CRenderer","RenderRoads",0x553A10,0,-1,512},
    {"CRenderer","RenderEverythingBarRoads",0x553AA0,0,-1,1024},{"CRenderer","RenderFirstPersonVehicle",0x553D00,0,-1,256},
    {"CRenderer","SetupLightingForEntity",0x553E40,0,-1,256},{"CRenderer","SetupMapEntityVisibility",0x553F60,0,-1,1024},
    {"CRenderer","SetupEntityVisibility",0x554230,0,-1,1024},{"CRenderer","SetupBigBuildingVisibility",0x554650,0,-1,1024},
    {"CRenderer","SetupScanLists",0x553540,0,-1,512},{"CRenderer","ScanSectorList_ListModels",0x5535D0,0,-1,512},
    {"CRenderer","ScanSectorList_ListModelsVisible",0x553650,0,-1,512},{"CRenderer","ScanSectorList",0x554840,0,-1,1024},
    {"CRenderer","ScanBigBuildingList",0x554B10,0,-1,1024},{"CRenderer","ShouldModelBeStreamed",0x554EB0,0,-1,1024},
    {"CRenderer","ScanPtrList_RequestModels<CPtrListSingleLink<CPhysical*>>",0x555680,0,-1,512},{"CRenderer","ConstructRenderList",0x5556E0,0,-1,512},
    {"CRenderer","ScanSectorList_RequestModels",0x555900,0,-1,512},{"CRenderer","ScanWorld",0x554FE0,0,-1,512},
    {"CRenderer","GetObjectsInFrustum",0x554C60,0,-1,512},{"CRenderer","RequestObjectsInFrustum",0x555960,0,-1,512},
    {"CRenderer","RequestObjectsInDirection",0x555CB0,0,-1,512},
}};

struct Saved { uintptr_t at; size_t n; std::vector<uint8> v; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"renderer33-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"renderer33-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("renderer33");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"renderer33-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // vtables falsas
    FillTraps(std::make_integer_sequence<int,64>{});
    evt[7]=reinterpret_cast<void*>(&FakeCreateRw); evt[8]=reinterpret_cast<void*>(&FakeDeleteRw); evt[17]=reinterpret_cast<void*>(&FakePreRender);
    evt[18]=reinterpret_cast<void*>(&FakeRender); evt[19]=reinterpret_cast<void*>(&FakeSetupLighting); evt[20]=reinterpret_cast<void*>(&FakeRemoveLighting);
    mvt[1]=reinterpret_cast<void*>(&MiAsAtomic); mvt[4]=reinterpret_cast<void*>(&MiType); mvt[5]=reinterpret_cast<void*>(&MiTimeInfo); mvt[9]=reinterpret_cast<void*>(&MiRwType);
    if(!nRegions){ Reg(arena.data(),arena.size()); Reg(mats.data(),sizeof(mats)); Reg(mis.data(),sizeof(mis)); Reg(cols.data(),sizeof(cols)); Reg(clumps.data(),sizeof(clumps));
        Reg(handlings.data(),sizeof(handlings)); Reg(rwCam.data(),rwCam.size()); Reg(sceneCam.data(),sceneCam.size()); Reg(camFrame.data(),camFrame.size());
        Reg(xformMat.data(),xformMat.size()); Reg(rpWorld.data(),rpWorld.size()); Reg(links.data(),sizeof(links)); Reg(outEnts.data(),sizeof(outEnts));
        Reg(reinterpret_cast<void*>(PC_SCRATCH),0x4000); Reg(reinterpret_cast<void*>(REN_BEGIN),REN_END-REN_BEGIN); Reg(reinterpret_cast<void*>(CAMERA),CAMERA_SIZE); }
    // callbacks de CWorldScan::ScanWorld: direccion del EXE o simbolo de la DLL (con hooks son equivalentes)
    scanFns={0x554840,0x554B10,0x5535D0,0x553650,0x555900,uintptr_t(&CRenderer::ScanSectorList),uintptr_t(&CRenderer::ScanBigBuildingList),
             uintptr_t(&CRenderer::ScanSectorList_ListModels),uintptr_t(&CRenderer::ScanSectorList_ListModelsVisible),uintptr_t(&CRenderer::ScanSectorList_RequestModels)};

    // ids de model info: 7 especiales y 10 huecos libres de ms_modelInfoPtrs
    { static const int32 special[7]{0,430,432,437,453,454,460}; int c=0; for(int32 s:special)ids[c++]=s;
      for(int32 m=1000;m<20000&&c<NMI;++m)if(!AtA<void*>(MI_PTRS+size_t(m)*4))ids[c++]=m;
      if(c<NMI){Write({{"event","error"},{"reason","not enough free model info slots"}});return;} }
    const uintptr_t engine=AtA<uintptr_t>(ENGINE);

    // estado global guardado
    std::vector<Saved> saved;
    auto Save=[&](uintptr_t a,size_t n){ saved.push_back({a,n,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); };
    Save(REN_BEGIN,REN_END-REN_BEGIN); Save(LOD_SCALES,0x18); Save(PC_SCRATCH,0x4000); Save(CAMERA,CAMERA_SIZE); Save(SCENE,8); Save(ALPHA,0xB0);
    Save(POSTFX_NV,2); Save(MIRROR_TYPE,4); Save(FIRST_PERSON,1); Save(REPLAY_MODE,1); Save(DISABLE_STREAMING,1); Save(NUM_REQUESTED,4);
    Save(LOD_DIST_MULT,4); Save(PLAYER0_PED,4); Save(SCAN_CODE,2); Save(CURR_AREA,4); Save(POOLS,0x20); Save(REPEAT,0xC00); Save(LOD_LISTS,0xE10);
    Save(engine+0x20,4);
    for(int y=SEC_LO;y<=SEC_HI;++y)Save(SECTORS+(size_t(y)*120+SEC_LO)*8,size_t(SEC_HI-SEC_LO+1)*8);
    for(int32 id:ids){ Save(MI_PTRS+size_t(id)*4,4); Save(STREAM_INFO+size_t(id)*20,20); }
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.at),s.v.data(),s.n); };
    auto AllRestored=[&]{ for(auto& s:saved)if(std::memcmp(reinterpret_cast<void*>(s.at),s.v.data(),s.n))return false; return true; };

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    add(0x735C70,uintptr_t(&DeActivateDirectional),&FakeDeactDir);
    add(0x735D30,uintptr_t(static_cast<void(*)()>(&SetAmbientColours)),&FakeSetAmbient);
    add(0x7357E0,uintptr_t(&WorldReplaceNormalLightsWithScorched),&FakeScorched);
    add(0x735D90,uintptr_t(&SetLightColoursForPedsCarsAndObjects),&FakeLightColours);
    add(0x733F10,uintptr_t(&CVisibilityPlugins::RenderFadingEntities),&FakeRenderFading);
    add(0x7337D0,uintptr_t(&CVisibilityPlugins::RenderFadingUnderwaterEntities),&FakeRenderFadingUw);
    add(0x733160,uintptr_t(&CVisibilityPlugins::SetupVehicleVariables),&FakeSetupVehVars);
    add(0x734530,uintptr_t(&CVisibilityPlugins::InitAlphaAtomicList),&FakeInitAlphaAtomics);
    add(0x733E90,uintptr_t(&CVisibilityPlugins::RenderAlphaAtomics),&FakeRenderAlphaAtomics);
    add(0x734540,uintptr_t(&CVisibilityPlugins::InitAlphaEntityList),&FakeInitAlphaEntities);
    add(0x734570,uintptr_t(&CVisibilityPlugins::InsertEntityIntoSortedList),&FakeInsertSorted);
    add(0x733D90,uintptr_t(&CVisibilityPlugins::InsertEntityIntoUnderwaterList),&FakeInsertUw);
    add(0x732B20,uintptr_t(&CVisibilityPlugins::GetClumpAlpha),&FakeClumpAlpha);
    add(0x7034F0,uintptr_t(&CPostEffects::IsVisionFXActive),&FakeVisionFx);
    add(0x7034B0,uintptr_t(&CPostEffects::FilterFX_StoreAndSetDayNightBalance),&FakeStoreDnb);
    add(0x7034D0,uintptr_t(&CPostEffects::FilterFX_RestoreDayNightBalance),&FakeRestoreDnb);
    add(0x7012E0,uintptr_t(&CPostEffects::NightVisionSetLights),&FakeNvLights);
    add(0x701430,uintptr_t(&CPostEffects::InfraredVisionSetLightsForDefaultObjects),&FakeIrLights);
    add(0x6D3D60,MAddr(&CVehicle::RenderDriverAndPassengers),&FakeRenderDriver);
    add(0x6D64F0,MAddr(&CVehicle::SetupRender),&FakeSetupRender);
    add(0x6D0E20,MAddr(&CVehicle::ResetAfterRender),&FakeResetAfterRender);
    add(0x6FFFE0,uintptr_t(&CPointLights::RemoveLightsAffectingObject),&FakeRemoveLights);
    add(0x6FFBB0,uintptr_t(&CPointLights::GenerateLightsAffectingObject),&FakeGenLights);
    add(0x5D5BA0,uintptr_t(&CCarFXRenderer::SetFxEnvMapLightMult),&FakeEnvMapMult);
    add(0x5447B0,MAddr(&CPhysical::GetLightingFromCol),&FakeLightFromCol);
    add(0x6C4750,uintptr_t(&CHeli::SpecialHeliPreRender),&FakeHeliPreRender);
    add(0x707FA0,uintptr_t(&CShadows::RenderExtraPlayerShadows),&FakeExtraShadows);
    add(0x7EE180,uintptr_t(&RwCameraEndUpdate),&FakeCamEnd);
    add(0x7EE190,uintptr_t(&RwCameraBeginUpdate),&FakeCamBegin);
    add(0x7EDD90,0,&FakeTransform);   // RwV3dTransformPoints de la DLL es un envoltorio del EXE
    add(0x72CAE0,0,&FakeWorldScan);   // CWorldScan::ScanWorld de la DLL es un envoltorio del EXE
    add(0x56E210,uintptr_t(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),&FakeFindPlayerPed);
    add(0x56E0D0,uintptr_t(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),&FakeFindPlayerVehicle);
    add(0x52CEE0,uintptr_t(&CClock::GetIsTimeInRange),&FakeTimeInRange);
    add(0x534540,MAddr(&CEntity::GetIsOnScreen),&FakeOnScreen);
    add(0x71FAE0,MAddr(&CEntity::IsEntityOccluded),&FakeOccluded);
    add(0x4087E0,uintptr_t(&CStreaming::RequestModel),&FakeRequestModel);
    add(0x4096C0,uintptr_t(&CStreaming::StartRenderEntities),&FakeStartRenderEntities);
    add(0x563470,uintptr_t(&CWorld::ClearScanCodes),&FakeClearScanCodes);
    add(0x514B80,MAddr(&CCamera::CalculateGroundHeight),&FakeGroundHeight);
    add(0x50AE90,MAddr(&CCamera::GetLookDirection),&FakeLookDir);
    add(0x72D9F0,uintptr_t(&CCullZones::FindTunnelAttributesForCoors),&FakeTunnelAttr);
    add(0x726DF0,uintptr_t(&CMirrors::BeforeConstructRenderList),&FakeMirrorsBefore);
    add(0x7201C0,uintptr_t(&COcclusion::ProcessBeforeRendering),&FakeOcclusionBefore);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        auto* const scratch=reinterpret_cast<uint8*>(PC_SCRATCH);
        auto* const cam=reinterpret_cast<uint8*>(CAMERA);
        auto& scanCode=AtA<uint16>(SCAN_CODE); auto& currArea=AtA<int32>(CURR_AREA);
        auto SectorAt=[](int x,int y){ return reinterpret_cast<uint32*>(SECTORS+(size_t(y)*120+size_t(x))*8); };
        auto RepeatAt=[](int x,int y){ return reinterpret_cast<uint32*>(REPEAT+(size_t(uint32(y)&15u)*16+(uint32(x)&15u))*12); };
        auto LodListAt=[](int x,int y){ return reinterpret_cast<uint32*>(LOD_LISTS+(size_t(y)*30+size_t(x))*4); };

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto uni=[&](float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; };
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3333u; if(!st)st=1; nev=0; focusEnt=nullptr;
                    const uintptr_t a=fn.address;
                    json out; json ret=nullptr;

                    // ---------- estado comun ----------
                    currArea=rnd(4) ? 0 : int32(rnd(19));
                    scanCode=rnd(16) ? uint16(1+rnd(0xFFF0)) : 0xFFFF;
                    AtA<void*>(engine+0x20)=reinterpret_cast<void*>(&FakeRss);
                    // pools falsos: entidades (los 5 punteros) y nodos simples/dobles
                    entStates.fill(0); sStates.fill(0); dStates.fill(0);
                    auto Hdr=[&](std::array<uint8,0x14>& h,void* storage,void* states,uint32 cap){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=storage; At<void*>(h.data(),4)=states; At<uint32>(h.data(),8)=cap; At<int32>(h.data(),0xC)=-1; };
                    Hdr(poolHdrs[0],sNodes.data(),sStates.data(),NSNODE); Hdr(poolHdrs[1],dNodes.data(),dStates.data(),NDNODE);
                    for(int k=2;k<7;++k)Hdr(poolHdrs[k],arena.data(),entStates.data(),uint32(POOL_CAP));
                    AtA<void*>(0xB74484)=poolHdrs[0].data(); AtA<void*>(0xB74488)=poolHdrs[1].data();
                    for(int k=0;k<5;++k)AtA<void*>(0xB74490+size_t(k)*4)=poolHdrs[2+k].data();
                    // TheCamera, RwCamera y Scene
                    CVector camPos{uni(-200.f,200.f),uni(-200.f,200.f),uni(-10.f,120.f)};
                    std::memset(rwCam.data(),0,rwCam.size()); At<void*>(rwCam.data(),4)=camFrame.data();
                    At<float>(rwCam.data(),0x68)=uni(0.3f,1.2f); At<float>(rwCam.data(),0x6C)=uni(0.3f,1.0f);
                    At<float>(rwCam.data(),0x84)=rnd(4) ? uni(100.f,2000.f) : 300.f; At<float>(rwCam.data(),0x90)=uni(-5.f,5.f);
                    std::memcpy(sceneCam.data(),rwCam.data(),rwCam.size()); At<float>(sceneCam.data(),0x90)=uni(-5.f,5.f);
                    auto RandMatrix=[&](uint8* m,const CVector& pos,size_t pad){ std::memset(m,0,pad); auto* v=reinterpret_cast<float*>(m);
                        const float h=uni(-3.2f,3.2f), p=uni(-1.2f,1.2f), ch=std::cos(h), sh=std::sin(h), cp=std::cos(p), sp=std::sin(p);
                        v[0]=ch; v[1]=sh; v[2]=0.f; v[4]=-sh*cp; v[5]=ch*cp; v[6]=sp; v[8]=sh*sp; v[9]=-ch*sp; v[10]=cp; v[12]=pos.x; v[13]=pos.y; v[14]=pos.z;
                        if(rnd(8)==0)v[10]=rnd(2) ? 0.f : -0.f; };
                    RandMatrix(camFrame.data()+0x10,camPos,0x40); std::memset(camFrame.data(),0,0x10);
                    RandMatrix(mats[NENT].data(),camPos,0x48);
                    At<void*>(cam,C_RWCAM)=rwCam.data();
                    At<void*>(cam,C_MATRIX)=rnd(4) ? mats[NENT].data() : nullptr; At<CVector>(cam,C_POS)=camPos+CVector{uni(-1.f,1.f),0.f,0.f}; At<float>(cam,C_HEADING)=uni(-3.2f,3.2f);
                    At<float>(cam,C_LODMULT)=rnd(3) ? uni(0.5f,2.f) : 1.f; At<uint8>(cam,C_TUNNEL_BIG)=uint8(rnd(6)==0);
                    At<uint8>(cam,C_ACTIVE)=uint8(rnd(3));
                    for(int c=0;c<3;++c){ static const uint16 modes[]{14,16,16,4,18,46}; At<uint16>(cam,C_CAM_MODE+size_t(c)*CAM_STRIDE)=modes[rnd(6)]; At<uint32>(cam,C_CAM_DIR+size_t(c)*CAM_STRIDE)=rnd(4); }
                    { const float h=uni(-3.2f,3.2f); At<CVector>(cam,C_FWD)=CVector{-std::sin(h),std::cos(h),uni(-0.3f,0.3f)}; }
                    AtA<void*>(SCENE)=rpWorld.data(); AtA<void*>(SCENE+4)=sceneCam.data();
                    // globales de CRenderer
                    AtA<CVector>(R_CAMPOS)=camPos; AtA<float>(R_HEADING)=uni(-3.2f,3.2f); AtA<float>(R_FARCLIP)=rnd(3) ? uni(100.f,1500.f) : 300.f;
                    AtA<uint8>(R_SKY)=uint8(rnd(5)==0); AtA<uint8>(R_PRIORITY)=uint8(rnd(2)); AtA<uint8>(R_TUNNELS)=uint8(rnd(2)); AtA<uint8>(R_OUTSIDE)=uint8(rnd(4)!=0);
                    AtA<float>(LOD_SCALES+4)=rnd(2) ? 1.f : uni(1.f,2.2f); AtA<uint32>(R_REQ_FLAGS)=next()&0x3F;
                    AtA<CVector>(R_LASTPOS)=camPos+CVector{uni(-6.f,6.f),uni(-6.f,6.f),uni(-2.f,2.f)}; AtA<CVector>(R_LASTFWD)=At<CVector>(cam,C_FWD)+CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),0.f};
                    AtA<int32>(R_N_LODS)=int32(rnd(6)); AtA<int32>(R_N_ENTS)=int32(rnd(6)); AtA<int32>(R_N_SUPERLODS)=int32(rnd(3)); AtA<int32>(R_N_INVIS)=int32(rnd(5));
                    AtA<void*>(R_LOD)=reinterpret_cast<void*>(LOD_LIST_BASE+8*rnd(4)); AtA<void*>(R_LOD_DONT)=reinterpret_cast<void*>(LOD_DONT_BASE+8*rnd(4));
                    AtA<void*>(R_FPV)=nullptr; AtA<void*>(R_GPOUT)=outEnts.data(); outEnts.fill(nullptr);
                    // otras globales
                    visionFx=rnd(3)==0; AtA<uint8>(POSTFX_NV)=uint8(rnd(2)); AtA<uint8>(POSTFX_NV+1)=uint8(rnd(2));
                    AtA<int32>(MIRROR_TYPE)=rnd(3) ? 0 : int32(1+rnd(3)); AtA<uint8>(FIRST_PERSON)=uint8(rnd(2)); AtA<uint8>(REPLAY_MODE)=uint8(rnd(4)==0 ? 1 : rnd(5)==0 ? 2 : 0);
                    AtA<uint8>(DISABLE_STREAMING)=uint8(rnd(6)==0); AtA<int32>(NUM_REQUESTED)=int32(rnd(4))-1; AtA<float>(LOD_DIST_MULT)=rnd(3) ? 1.f : uni(0.5f,2.f);
                    genLight=uni(0.f,2.f); genTotal=uni(0.f,1.f); colLight=uni(0.f,1.f); timeMask=next(); lookDir=int32(rnd(5)); tunnelAttr=0;
                    for(auto& g:groundZ)g=camPos.z-uni(-20.f,120.f);
                    // clumps, handling y model infos
                    for(size_t k=0;k<clumps.size();++k){ auto* c=clumps[k].data(); std::memset(c,0,0x20); c[0]=2; At<int32>(c,4)=rnd(3) ? 255 : int32(rnd(255)); }
                    for(auto& h:handlings){ std::memset(h.data(),0,h.size()); At<uint32>(h.data(),0xCC)=next(); }
                    for(int k=0;k<NMI;++k){ auto* m=mis[k].data(); std::memset(m,0,0x40); At<void*>(m,0)=mvt.data();
                        m[M_ALPHA]=uint8(rnd(3) ? 255 : rnd(255)); At<uint16>(m,M_FLAGS)=uint16(next());
                        if(rnd(3)==0){ static const uint16 glass[]{0x2000,0x2800,0x1800,0x3000}; At<uint16>(m,M_FLAGS)=uint16((At<uint16>(m,M_FLAGS)&~0x7800)|glass[rnd(4)]); }
                        auto* c=cols[k].data(); std::memset(c,0,0x30); At<float>(c,0x24)=rnd(32) ? uni(0.f,80.f) : std::bit_cast<float>(QNAN);
                        At<void*>(m,M_COL)=c; static const float dd[]{20.f,150.f,300.f,320.f};
                        At<float>(m,M_DRAW)=rnd(3) ? uni(0.f,700.f) : dd[rnd(4)];
                        At<void*>(m,M_RW)=rnd(4) ? static_cast<void*>(c) : nullptr;
                        m[M_TIME]=uint8(rnd(24)); m[M_TIME+1]=uint8(rnd(24)); At<int16>(m,M_TIME+2)=int16(rnd(3) ? -1 : ids[rnd(NMI)]);
                        static const uint8 types[]{1,1,1,3,3,4,5,6,7}; uint8 type=types[rnd(9)]; if(k>=1&&k<=6&&rnd(3))type=6;
                        m[M_TYPE]=type; m[M_ATOMIC]=uint8(type==1||type==3); m[M_HASTIME]=uint8(type==3||rnd(6)==0);
                        AtA<void*>(MI_PTRS+size_t(ids[k])*4)=m;
                        static const uint8 ls[]{0,1,1,2,3}; AtA<uint8>(STREAM_INFO+size_t(ids[k])*20+0x10)=ls[rnd(5)]; }
                    auto MiOf=[&](const uint8* e)->uint8*{ const uint16 id=At<uint16>(const_cast<uint8*>(e),E_MODEL); for(int k=0;k<NMI;++k)if(ids[k]==id)return mis[k].data(); return nullptr; };
                    // entidades: 0..23 con tipos variados
                    auto Near=[&](float r){ return camPos+CVector{uni(-r,r),uni(-r,r),uni(-20.f,20.f)}; };
                    auto InitEnt=[&](int k,uint8 type){ uint8* e=Ent(k); std::memset(e,0,ENT_SIZE); At<void*>(e,0)=evt.data();
                        const CVector pos=rnd(3) ? Near(120.f) : Near(600.f); At<CVector>(e,E_POS)=pos; At<float>(e,E_HEADING)=uni(-3.2f,3.2f);
                        if(rnd(2)){ RandMatrix(mats[k].data(),rnd(4) ? pos : pos+CVector{uni(-3.f,3.f),uni(-3.f,3.f),0.f},0x48); At<void*>(e,E_MATRIX)=mats[k].data(); }
                        At<void*>(e,E_RW)=rnd(4) ? clumps[size_t(k)%NCLUMP].data() : nullptr;
                        At<uint32>(e,E_FLAGS)=next(); At<uint16>(e,E_MODEL)=uint16(ids[rnd(NMI)]);
                        At<uint16>(e,E_SCAN)=rnd(5)==0 ? scanCode : uint16(next());
                        At<uint8>(e,E_AREA)=uint8(rnd(4) ? currArea : rnd(3)==0 ? 13 : rnd(19));
                        At<void*>(e,E_LOD)=rnd(3)==0 ? Ent(int(rnd(NENT))) : nullptr;
                        static const uint8 nl[]{0,0,1,2,3,4,200}; const uint8 children=nl[rnd(7)]; At<uint8>(e,E_NLOD)=children;
                        static const int8 nr[]{0,0,1,2,3,-128,-1,5}; At<int8>(e,E_NLODR)=rnd(5)==0 ? int8(children) : nr[rnd(8)];
                        At<uint8>(e,E_TYPE)=uint8((next()&0xF8)|type);
                        At<uint32>(e,P_FLAGS)=next(); At<float>(e,P_DYNLIGHT)=uni(0.f,1.f);
                        At<int32>(e,PED_STATE)=rnd(4)==0 ? 50 : int32(rnd(60));
                        At<void*>(e,V_HANDLING)=handlings[size_t(k)%4].data(); At<uint8>(e,V_FLAGS5)=uint8(next());
                        static const int32 vt[]{0,0,5,5,9,3,6}; At<int32>(e,V_TYPE)=vt[rnd(7)]; At<uint8>(e,B_FLAGS)=uint8(next());
                        onScreen[k]=rnd(4)!=0; occluded[k]=rnd(4)==0; lightRet[k]=rnd(2)!=0; createGives[k]=rnd(3)!=0; insertRet[k]=uint8(next()); };
                    for(int k=0;k<NENT;++k){ static const uint8 types[]{1,1,2,2,3,4,4,5}; InitEnt(k,types[rnd(8)]); }
                    auto Pick=[&]{ return Ent(int(rnd(NENT))); };
                    fakePlayerPed=nullptr; fakePlayerVeh=rnd(4)==0 ? Pick() : nullptr;
                    // listas de entidades visibles (punteros a entidades falsas)
                    for(int k=0;k<8;++k){ AtA<void*>(R_LODS+size_t(k)*4)=Pick(); AtA<void*>(R_ENTS+size_t(k)*4)=Pick(); AtA<void*>(R_INVIS+size_t(k)*4)=Pick(); AtA<void*>(R_SUPERLODS+size_t(k)*4)=Pick(); }
                    // listas del mundo vacias (bloque de sectores, sectores repetidos y listas LOD)
                    for(int y=SEC_LO;y<=SEC_HI;++y)std::memset(SectorAt(SEC_LO,y),0,size_t(SEC_HI-SEC_LO+1)*8);
                    std::memset(reinterpret_cast<void*>(REPEAT),0,0xC00); std::memset(reinterpret_cast<void*>(LOD_LISTS),0,0xE10);
                    uint32 nS=0, nD=0;
                    auto AddS=[&](uint32* head,void* item){ if(nS>=NSNODE)return; uint32* n=&sNodes[nS++*2]; n[0]=uint32(uintptr_t(item)); n[1]=*head; *head=uint32(uintptr_t(n)); };
                    auto AddD=[&](uint32* head,void* item){ if(nD>=NDNODE)return; uint32* n=&dNodes[nD++*3]; n[0]=uint32(uintptr_t(item)); n[1]=*head; n[2]=0;
                        if(*head)reinterpret_cast<uint32*>(uintptr_t(*head))[2]=uint32(uintptr_t(n)); *head=uint32(uintptr_t(n)); };
                    auto InRange=[&](int x,int y){ return x>=0&&y>=0&&x<120&&y<120; };
                    auto FillSector=[&](int x,int y){
                        if(InRange(x,y)){ uint32* s=SectorAt(x,y); for(uint32 k=rnd(6);k;--k)AddS(&s[0],Pick()); for(uint32 k=rnd(4);k;--k)AddD(&s[1],Pick()); }
                        uint32* r=RepeatAt(x,y); for(int l=0;l<3;++l)for(uint32 k=rnd(4);k;--k)AddD(&r[l],Pick()); };
                    auto PickSector=[&]()->std::pair<int,int>{ static const int outs[]{-1,-2,-16,-17,120,121,136,-100};
                        auto c=[&]{ return rnd(5) ? SEC_LO+int(rnd(SEC_HI-SEC_LO+1)) : outs[rnd(8)]; }; return {c(),c()}; };
                    // la distancia inicial de ScanSectorList/ScanBigBuildingList es basura de pila en el EXE (ver cabecera)
                    auto MaskStaleDistance=[&](bool lod){ for(int k=0;k<NENT;++k){ uint8* e=Ent(k); const uint8* m=MiOf(e); if(!m)continue; const uint8 t=m[M_TYPE];
                        if(lod){ if(t==6)At<uint32>(e,E_FLAGS)&=~0xC000u; }
                        else if((t!=1&&t!=3&&t!=4&&t!=5)||(t==1&&(At<uint32>(e,E_FLAGS)&0x80000u)))At<uint32>(e,E_FLAGS)&=~0x8000u; } };
                    auto RandDist=[&]{ switch(rnd(12)){case 0:return 0.f;case 1:return std::bit_cast<float>(QNAN);case 2:return uni(280.f,320.f);case 3:return uni(0.f,30.f);default:return uni(0.f,900.f);} };

                    // ---------- fixture por funcion (identico en las dos ejecuciones) ----------
                    std::function<void()> call;
                    auto C0=[&]{ return [&,a]{ reinterpret_cast<void(__cdecl*)()>(a)(); }; };
                    if(nm=="RenderFadingInEntities"||nm=="RenderFadingInUnderwaterEntities"||nm=="ResetLodRenderLists"||nm=="RenderRoads"||nm=="RenderEverythingBarRoads"||nm=="ConstructRenderList"||nm=="ScanWorld"){
                        if(nm=="RenderRoads"||nm=="RenderEverythingBarRoads"){ AtA<int32>(R_N_ENTS)=int32(rnd(9)); AtA<int32>(R_N_LODS)=int32(rnd(5)); }
                        if(nm=="ConstructRenderList"){ static const int32 attrs[]{0,0x80,0x800,0x880,0x1,0x804,0x84,0xFFFF};
                            tunnelAttr=attrs[rnd(8)]; fakePlayerPed=rnd(6) ? Pick() : nullptr; fakePlayerVeh=rnd(2) ? Pick() : nullptr;
                            if(fakePlayerPed&&rnd(3))At<uint8>(fakePlayerPed,E_AREA)=0;
                            if(rnd(2)){ auto* cm=At<uint8*>(cam,C_MATRIX); auto& cp=cm ? At<CVector>(cm,0x30) : At<CVector>(cam,C_POS); cp.z=rnd(4) ? uni(60.f,300.f) : 80.f; }
                            if(fakePlayerPed&&rnd(2)){ auto* em=At<uint8*>(fakePlayerPed,E_MATRIX); (em ? At<CVector>(em,0x30) : At<CVector>(fakePlayerPed,E_POS)).z=groundZ[0]+uni(30.f,80.f); } }
                        call=C0(); }
                    else if(nm=="RenderOneRoad"||nm=="RenderOneNonRoad"||nm=="RemoveVehiclePedLights"){ uint8* e=Pick(); focusEnt=e;
                        call=[&,a,e]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(e); }; }
                    else if(nm=="AddEntityToRenderList"||nm=="AddToLodRenderList"||nm=="AddToLodDontRenderList"){ uint8* e=Pick(); const float d=RandDist();
                        AtA<int32>(R_N_LODS)=int32(rnd(990)); AtA<int32>(R_N_ENTS)=int32(rnd(990));
                        call=[&,a,e,d]{ reinterpret_cast<void(__cdecl*)(void*,float)>(a)(e,d); }; }
                    else if(nm=="GetLodRenderListBase"||nm=="GetLodDontRenderListBase"){ call=[&,a]{ ret=IdU(reinterpret_cast<void*(__cdecl*)()>(a)()); }; }
                    else if(nm=="ProcessLodRenderLists"){ const uint32 n=rnd(18);
                        for(uint32 k=0;k<n;++k){ auto* en=reinterpret_cast<uint8*>(LOD_LIST_BASE+k*8); At<void*>(en,0)=rnd(6) ? Pick() : nullptr; At<float>(en,4)=RandDist(); }
                        AtA<void*>(R_LOD)=reinterpret_cast<void*>(LOD_LIST_BASE+n*8); call=C0(); }
                    else if(nm=="PreRender"){ AtA<int32>(R_N_LODS)=int32(rnd(7)); AtA<int32>(R_N_ENTS)=int32(rnd(7)); AtA<int32>(R_N_SUPERLODS)=int32(rnd(4)); AtA<int32>(R_N_INVIS)=int32(rnd(5));
                        // listas alpha (entidades y bajo el agua) con enlaces falsos entre la cabeza y la cola reales
                        size_t li=0; for(uintptr_t head:{ALPHA_HEAD_NEXT,UW_HEAD_NEXT}){ const uintptr_t tail=head==ALPHA_HEAD_NEXT ? ALPHA_TAIL : UW_TAIL;
                            uintptr_t prev=head; for(uint32 k=rnd(6);k&&li<links.size();--k){ auto* l=links[li++].data(); std::memset(l,0,0x14); At<void*>(l,0)=Pick();
                                static const uint32 cbs[]{0x732B40,0x732B40,0x733000,0x734000}; At<uint32>(l,4)=cbs[rnd(4)]; At<float>(l,8)=uni(0.f,100.f);
                                AtA<void*>(prev)=l; prev=uintptr_t(l)+0x10; }
                            AtA<uintptr_t>(prev)=tail; }
                        call=C0(); }
                    else if(nm=="RenderFirstPersonVehicle"){ AtA<void*>(R_FPV)=rnd(6) ? Pick() : nullptr; uint8* ped=Pick(); AtA<void*>(PLAYER0_PED)=ped;
                        At<int8>(ped,PED_SLOT)=int8(rnd(13)); for(int s=0;s<13;++s)At<uint32>(ped,PED_WEAPONS+size_t(s)*0x1C)=rnd(3)==0 ? 28u : rnd(47);
                        call=C0(); }
                    else if(nm=="SetupLightingForEntity"){ uint8* e=Pick(); call=[&,a,e]{ ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(e); }; }
                    else if(nm=="SetupMapEntityVisibility"){ uint8* e=Pick(); uint8* m=rnd(4) ? MiOf(e) : mis[rnd(NMI)].data(); const float d=RandDist(); const bool tr=rnd(4)!=0;
                        if(!m)m=mis[0].data();
                        call=[&,a,e,m,d,tr]{ ret=reinterpret_cast<int32(__cdecl*)(void*,void*,float,bool)>(a)(e,m,d,tr); }; }
                    else if(nm=="SetupEntityVisibility"||nm=="SetupBigBuildingVisibility"){ uint8* e=Pick();
                        if(rnd(3)==0){ fakePlayerVeh=e; At<uint8>(e,E_TYPE)=uint8((At<uint8>(e,E_TYPE)&0xF8)|2); } else fakePlayerVeh=rnd(2) ? Pick() : nullptr;
                        if(rnd(3)==0){ static const uint16 sp[]{430,432,437,453,454,460,0}; At<uint16>(e,E_MODEL)=sp[rnd(7)]; }
                        call=[&,a,e]{ float d=-1234.f; const int32 r=reinterpret_cast<int32(__cdecl*)(void*,float*)>(a)(e,&d); ret={r,F(d)}; }; }
                    else if(nm=="SetupScanLists"){ const auto sec=PickSector(); const int x=sec.first, y=sec.second; call=[&,a,x,y]{ reinterpret_cast<void(__cdecl*)(int32,int32)>(a)(x,y); }; }
                    else if(nm=="ScanSectorList_ListModels"||nm=="ScanSectorList_ListModelsVisible"||nm=="ScanSectorList"||nm=="ScanSectorList_RequestModels"){
                        const auto sec=PickSector(); const int x=sec.first, y=sec.second; FillSector(x,y); if(rnd(3)==0){ const auto sec2=PickSector(); FillSector(sec2.first,sec2.second); }
                        if(nm=="ScanSectorList"){ MaskStaleDistance(false); AtA<CVector>(R_CAMPOS)=CVector{float((x-60)*50+25)+uni(-150.f,150.f),float((y-60)*50+25)+uni(-150.f,150.f),camPos.z}; }
                        call=[&,a,x,y]{ reinterpret_cast<void(__cdecl*)(int32,int32)>(a)(x,y); }; }
                    else if(nm=="ScanBigBuildingList"){ static const int outs[]{-1,30,31,-30};
                        auto c=[&]{ return rnd(8) ? int(rnd(30)) : outs[rnd(4)]; }; const int x=c(), y=c();
                        if(x>=0&&y>=0&&x<30&&y<30)for(uint32 k=rnd(8);k;--k)AddS(LodListAt(x,y),Pick());
                        MaskStaleDistance(true);
                        AtA<CVector>(R_CAMPOS)=CVector{float((x-15)*200+100)+uni(-400.f,400.f),float((y-15)*200+100)+uni(-400.f,400.f),camPos.z};
                        for(int k=0;k<NENT;++k){ uint8* e=Ent(k); auto* em=At<uint8*>(e,E_MATRIX); (em ? At<CVector>(em,0x30) : At<CVector>(e,E_POS))=AtA<CVector>(R_CAMPOS)+CVector{uni(-700.f,700.f),uni(-700.f,700.f),uni(-20.f,20.f)}; }
                        call=[&,a,x,y]{ reinterpret_cast<void(__cdecl*)(int32,int32)>(a)(x,y); }; }
                    else if(nm=="ShouldModelBeStreamed"){ uint8* e=Pick(); static CVector pt; auto* em=At<uint8*>(e,E_MATRIX); const CVector ep=em ? At<CVector>(em,0x30) : At<CVector>(e,E_POS);
                        pt=rnd(4) ? ep+CVector{uni(-400.f,400.f),uni(-400.f,400.f),uni(-30.f,30.f)} : camPos; const float fc=rnd(16) ? uni(0.f,600.f) : std::bit_cast<float>(QNAN);
                        call=[&,a,e,fc]{ ret=reinterpret_cast<bool(__cdecl*)(void*,const CVector*,float)>(a)(e,&pt,fc); }; }
                    else if(nm=="ScanPtrList_RequestModels<CPtrListSingleLink<CPhysical*>>"){ plist.fill(0); for(uint32 k=rnd(10);k;--k)AddS(&plist[0],Pick());
                        call=[&,a]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(plist.data()); }; }
                    else if(nm=="GetObjectsInFrustum"){ const float fp=rnd(4) ? uni(10.f,600.f) : 300.f; uint8* m=nullptr;
                        if(rnd(2)){ RandMatrix(xformMat.data(),camPos+CVector{uni(-50.f,50.f),uni(-50.f,50.f),0.f},0x40); m=xformMat.data(); }
                        call=[&,a,fp,m]{ ret=reinterpret_cast<int32(__cdecl*)(void*,float,void*)>(a)(outEnts.data(),fp,m); }; }
                    else if(nm=="RequestObjectsInFrustum"){ uint8* m=nullptr; const int32 fl=int32(next()&0x3F);
                        if(rnd(3)){ RandMatrix(xformMat.data(),camPos+CVector{uni(-50.f,50.f),uni(-50.f,50.f),0.f},0x40); At<uint32>(xformMat.data(),0xC)=next(); m=xformMat.data();
                            if(rnd(16)==0)At<float>(xformMat.data(),0x28)=std::bit_cast<float>(QNAN); }
                        call=[&,a,m,fl]{ reinterpret_cast<void(__cdecl*)(void*,int32)>(a)(m,fl); }; }
                    else if(nm=="RequestObjectsInDirection"){ static CVector pos; pos=camPos+CVector{uni(-500.f,500.f),uni(-500.f,500.f),uni(-20.f,60.f)};
                        const float ang=rnd(8) ? uni(-7.f,7.f) : float(rnd(8))*0.785398f; const int32 fl=rnd(2) ? 0x20 : int32(next()&0x3F);
                        call=[&,a,ang,fl]{ reinterpret_cast<void(__cdecl*)(const CVector*,float,int32)>(a)(&pos,ang,fl); }; }

                    // ---------- llamada ----------
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    nev=0;
                    if(call)call();

                    // ---------- resultado ----------
                    out["switches"]=switches; out["ret"]=ret; out["trace"]=Events();
                    out["counts"]={AtA<int32>(R_N_LODS),AtA<int32>(R_N_ENTS),AtA<int32>(R_N_SUPERLODS),AtA<int32>(R_N_INVIS),IdU(AtA<void*>(R_LOD)),IdU(AtA<void*>(R_LOD_DONT)),IdU(AtA<void*>(R_FPV)),IdU(AtA<void*>(R_GPOUT))};
                    out["globals"]={F(AtA<float>(R_CAMPOS)),F(AtA<float>(R_CAMPOS+4)),F(AtA<float>(R_CAMPOS+8)),F(AtA<float>(R_HEADING)),F(AtA<float>(R_FARCLIP)),AtA<uint8>(R_PRIORITY),AtA<uint8>(R_SKY),
                                    AtA<uint8>(R_TUNNELS),AtA<uint8>(R_OUTSIDE),AtA<uint32>(R_REQ_FLAGS),AtA<uint8>(0xB745C8),AtA<uint32>(0xB76894),F(AtA<float>(LOD_SCALES)),F(AtA<float>(LOD_SCALES+4)),scanCode};
                    out["ren"]=Hash(reinterpret_cast<void*>(REN_BEGIN),REN_END-REN_BEGIN); out["scratch"]=Hash(scratch,0x4000);
                    out["ents"]=Hash(arena.data(),arena.size()); out["mis"]=Hash(mis.data(),sizeof(mis)); out["clumps"]=Hash(clumps.data(),sizeof(clumps));
                    out["cams"]={Hash(rwCam.data(),rwCam.size()),Hash(sceneCam.data(),sceneCam.size()),Hash(cam,CAMERA_SIZE)};
                    out["world"]={Hash(reinterpret_cast<void*>(REPEAT),0xC00),Hash(reinterpret_cast<void*>(LOD_LISTS),0xE10),Hash(sNodes.data(),sizeof(sNodes)),Hash(dNodes.data(),sizeof(dNodes))};
                    json outs=json::array(); for(auto* p:outEnts){ if(!p)break; outs.push_back(IdU(p)); } out["out_ents"]=outs;
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",AllRestored()}});
}
}
