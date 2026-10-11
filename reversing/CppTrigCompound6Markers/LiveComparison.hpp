#pragma once
// Banco dentro del motor: C3dMarker (AddMarker, DeleteMarkerObject, IsZCoordinateUpToDate,
// SetZCoordinateIfNotUpToDate, UpdateZCoordinate) y C3dMarkers (flechas, marcadores de usuario,
// Load/Save, ForceRender, Update, PlaceMarker, PlaceMarkerSet, PlaceMarkerCone, User3dMarkerAtomicCB).
// Original (hooks del lote desactivados) frente a candidato con los arrays globales y un marcador
// propio aleatorizados por caso. RenderWare (clonar/crear/destruir atomic y frame, render callback,
// RwFrameUpdateObjects, RpClumpForAllAtomics), el colisionado, el agua, el suelo, el centro del
// jugador, el bufer de guardado y el tiempo se simulan y se trazan. Se compara la memoria
// (punteros traducidos, flotantes bit a bit), el retorno y la traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "3dMarker.h"
#include "3dMarkers.h"
#include "VisibilityPlugins.h"
#include "WaterLevel.h"
#include "Collision/ColStore.h"
#include "GenericGameStorage.h"
#include "TxdStore.h"
#include "FileMgr.h"
#include "../ProbeRuntime.hpp"

namespace CppTrigCompound6MarkersLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
constexpr int N=32;
alignas(16) inline std::array<uint8,sizeof(C3dMarker)> marker{};
alignas(16) inline std::array<std::array<uint8,0x80>,N+2> atomics{};   // atomics falsos (+4 frame, +0x18 geometria)
alignas(16) inline std::array<std::array<uint8,0x60>,N+2> frames{};    // frames falsos (+0x10 matriz RW)
alignas(16) inline std::array<std::array<uint8,0x40>,N+1> rwmats{};    // matrices RW sueltas para m_pAttachMatrix
alignas(16) inline std::array<uint8,0x60> geometry{}, material{};
alignas(16) inline std::array<void*,4> matList{};
alignas(16) inline std::array<std::array<uint8,0x20>,7> clumps{};
inline CVector centre{}, argPos{}; inline bool losHit{}, waterFound{}, colLoaded{}, groundFound{}; inline float losZ{}, waterZ{}, groundZ{};
inline int cloneNext{}, frameNext{}; inline std::vector<uint8> loadStream; inline size_t loadPos{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const std::string& nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return nm+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(marker.data(),marker.size(),"marker"))return *s;
    for(size_t k=0;k<atomics.size();++k)if(auto s=in(atomics[k].data(),atomics[k].size(),"atomic"+std::to_string(k)))return *s;
    for(size_t k=0;k<frames.size();++k)if(auto s=in(frames[k].data(),frames[k].size(),"frame"+std::to_string(k)))return *s;
    for(size_t k=0;k<rwmats.size();++k)if(auto s=in(rwmats[k].data(),rwmats[k].size(),"rwmat"+std::to_string(k)))return *s;
    for(size_t k=0;k<clumps.size();++k)if(auto s=in(clumps[k].data(),clumps[k].size(),"clump"+std::to_string(k)))return *s;
    if(auto s=in(geometry.data(),geometry.size(),"geometry"))return *s; if(auto s=in(material.data(),material.size(),"material"))return *s;
    if(auto s=in(matList.data(),sizeof(matList),"matlist"))return *s; if(auto s=in(&centre,sizeof(centre),"centre"))return *s;
    if(auto s=in(&C3dMarkers::m_aMarkerArray,sizeof(C3dMarkers::m_aMarkerArray),"array"))return *s;
    return v;
}
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }

// RenderWare y demas dependencias
inline RpClump* __cdecl ForAllAtomics(RpClump* c,RpAtomicCallBack cb,void* data) {
    int k=-1; for(size_t j=0;j<clumps.size();++j)if(uintptr_t(c)==uintptr_t(clumps[j].data()))k=int(j);
    const bool removeRefs=uintptr_t(cb)==0x7226D0||uintptr_t(cb)==std::bit_cast<uintptr_t>(&RemoveRefsCB);
    trace.push_back({"clump_for_all_atomics",Name(uintptr_t(c)),removeRefs}); if(k>=0&&!removeRefs)cb(reinterpret_cast<RpAtomic*>(atomics[N+(k%2)].data()),data); return c; }
inline RpAtomic* __cdecl AtomicClone(RpAtomic* a) { trace.push_back({"atomic_clone",Name(uintptr_t(a))}); auto* r=atomics[cloneNext%N].data(); ++cloneNext; *reinterpret_cast<void**>(r+0x18)=geometry.data(); return reinterpret_cast<RpAtomic*>(r); }
inline RwFrame* __cdecl FrameCreate() { trace.push_back({"frame_create"}); auto* f=frames[frameNext%N].data(); ++frameNext; return reinterpret_cast<RwFrame*>(f); }
inline RpAtomic* __cdecl AtomicSetFrame(RpAtomic* a,RwFrame* f) { trace.push_back({"atomic_set_frame",Name(uintptr_t(a)),Name(uintptr_t(f))}); *reinterpret_cast<RwFrame**>(reinterpret_cast<uint8*>(a)+4)=f; return a; }
inline void __cdecl SetRenderCB(RpAtomic* a,RpAtomicCallBackRender cb) { trace.push_back({"set_render_cb",Name(uintptr_t(a)),uintptr_t(cb)}); }
inline RwBool __cdecl AtomicDestroy(RpAtomic* a) { trace.push_back({"atomic_destroy",Name(uintptr_t(a))}); return TRUE; }
inline RwBool __cdecl FrameDestroy(RwFrame* f) { trace.push_back({"frame_destroy",Name(uintptr_t(f))}); return TRUE; }
inline RwFrame* __cdecl FrameUpdate(RwFrame* f) { trace.push_back({"frame_update_objects",Name(uintptr_t(f))}); return f; }
inline const CVector* __cdecl Centre(int32 id) { trace.push_back({"player_centre",id,_control87(0,0)&_MCW_PC}); return &centre; }
inline bool __cdecl LineOfSight(const CVector& a,const CVector& b,CColPoint& cp,CEntity*& e,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7,bool b8) {
    trace.push_back({"line_of_sight",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),b1,b2,b3,b4,b5,b6,b7,b8});
    if(losHit){cp.m_vecPoint=CVector{b.x,b.y,losZ};e=nullptr;} return losHit; }
inline bool __cdecl WaterNoWaves(CVector p,float* out,float*,float*) { trace.push_back({"water_no_waves",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)}); if(waterFound&&out)*out=waterZ; return waterFound; }
inline bool __cdecl ColLoaded(const CVector& p,int area) { trace.push_back({"collision_loaded",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),area}); return colLoaded; }
inline float __cdecl GroundZ(CVector p,bool* found,CEntity** e) { trace.push_back({"ground_z",std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z),found!=nullptr,e!=nullptr}); if(found)*found=groundFound; return groundZ; }
inline bool __cdecl LoadWB(void* p,int32 n) { trace.push_back({"load_work_buffer",Name(uintptr_t(p)),n}); for(int32 k=0;k<n;++k)static_cast<uint8*>(p)[k]=loadStream[(loadPos++)%loadStream.size()]; return true; }
inline bool __cdecl SaveWB(void* p,int32 n) { trace.push_back({"save_work_buffer",Name(uintptr_t(p)),ReversingProbe::Bytes(p,n)}); return true; }
inline RwBool __cdecl StateSet(RwRenderState st,void* v) { trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline RwBool __cdecl StateGet(RwRenderState st,void* v) { trace.push_back({"render_state_get",int(st)}); *static_cast<uint32*>(v)=0x1000u+uint32(st); return TRUE; }
inline RwFrame* __cdecl FrameIdentity(RwFrame* f) { trace.push_back({"frame_set_identity",Name(uintptr_t(f))}); return f; }
inline RwFrame* __cdecl FrameTranslate(RwFrame* f,const RwV3d* v,int op) { trace.push_back({"frame_translate",Name(uintptr_t(f)),std::bit_cast<uint32>(v->x),std::bit_cast<uint32>(v->y),std::bit_cast<uint32>(v->z),op}); return f; }
inline RpClump* __cdecl ClumpRender(RpClump* c) { trace.push_back({"clump_render",Name(uintptr_t(c)),ReversingProbe::Bytes(&C3dMarkers::m_user3dMarkerColor,4)}); return c; }
// render, luces, camara, modelos y TXD
inline RpAtomic* __cdecl AtomicRenderCB(RpAtomic* a) { trace.push_back({"atomic_render",Name(uintptr_t(a)),ReversingProbe::Bytes(material.data()+4,4)}); return a; }
inline void __cdecl BrightColours(float b) { trace.push_back({"bright_colours",std::bit_cast<uint32>(b)}); }
inline void __cdecl ResetColours() { trace.push_back({"reset_colours"}); }
inline void __cdecl ActivateDir() { trace.push_back({"activate_directional"}); }
inline void __cdecl SetAmbient(RwRGBAReal* c) { trace.push_back({"set_ambient",uintptr_t(c)}); }
inline void __cdecl SetDirectional(RwRGBAReal* c) { trace.push_back({"set_directional",uintptr_t(c)}); }
inline bool __fastcall SphereVisible(void* cam,void*,const CVector& c,float r,RwMatrix* m) { trace.push_back({"sphere_visible",std::bit_cast<uint32>(c.x),std::bit_cast<uint32>(c.y),std::bit_cast<uint32>(r),uintptr_t(m)}); return (std::bit_cast<uint32>(c.x)>>7)%3!=0; }
alignas(16) inline std::array<uint8,0x40> fakeModel{}; inline std::array<void*,24> modelVtbl{}; inline int instNext{}, clumpNext{};
inline void* __fastcall CreateInstance(void* mi,void*) { trace.push_back({"create_instance"}); auto* a=atomics[(instNext++)%N].data(); return a; }
template<int K> void* __fastcall ModelTrap(void*,void*) { trace.push_back({"model_unexpected_slot",K}); return nullptr; }
template<int... K> void FillModelTraps(std::integer_sequence<int,K...>) { ((modelVtbl[K]=reinterpret_cast<void*>(&ModelTrap<K>)),...); }
inline CBaseModelInfo* __cdecl GetModelByName(const char* n,int32* id) { trace.push_back({"get_model_info",std::string(n)}); if(id)*id=1234+int32(strlen(n)); return reinterpret_cast<CBaseModelInfo*>(fakeModel.data()); }
inline void __fastcall ModelAddRef(void*,void*) { trace.push_back({"model_add_ref"}); }
inline bool __cdecl ReqModel(int32 id,int32 f) { trace.push_back({"request_model",id,f}); return true; }
inline void __cdecl LoadAllReq(bool b) { trace.push_back({"load_all_requested",b}); }
inline RpClump* __cdecl ClumpCreate() { trace.push_back({"clump_create"}); auto* c=clumps[(clumpNext++)%7].data(); std::memset(c,0,0x20); return reinterpret_cast<RpClump*>(c); }
inline RpClump* __cdecl ClumpAddAtomic(RpClump* c,RpAtomic* a) { trace.push_back({"clump_add_atomic",Name(uintptr_t(c)),Name(uintptr_t(a))}); return c; }
inline RwBool __cdecl ClumpDestroy(RpClump* c) { trace.push_back({"clump_destroy",Name(uintptr_t(c))}); return TRUE; }
inline int32 __cdecl FindTxd(const char* n) { trace.push_back({"find_txd",std::string(n)}); return 7; }
inline void __cdecl PushTxd() { trace.push_back({"push_txd"}); }
inline void __cdecl PopTxd() { trace.push_back({"pop_txd"}); }
inline void __cdecl SetTxd(int32 i) { trace.push_back({"set_txd",i}); }
inline void __cdecl ChangeDir(const char* d) { trace.push_back({"change_dir",std::string(d)}); }
inline C3dMarker* __cdecl FakePlace(uint32 id,int type,CVector* pos,float size,uint8 r,uint8 g,uint8 b,uint8 a,uint16 pp,float pf,int16 rr,CVector normal,bool z) {
    trace.push_back({"place_marker",id,type,Name(uintptr_t(pos)),std::bit_cast<uint32>(size),r,g,b,a,pp,std::bit_cast<uint32>(pf),rr,std::bit_cast<uint32>(normal.x),std::bit_cast<uint32>(normal.y),std::bit_cast<uint32>(normal.z),z});
    return nullptr; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,26> fns{{
    {"C3dMarker","AddMarker",0x722230,512},{"C3dMarker","DeleteMarkerObject",0x722390,128},{"C3dMarker","IsZCoordinateUpToDate",0x7226A0,1024},
    {"C3dMarker","SetZCoordinateIfNotUpToDate",0x724E10,1024},{"C3dMarker","UpdateZCoordinate",0x724D40,2048},
    {"C3dMarkers","DirectionArrowFindFirstFreeSlot",0x721120,256},{"C3dMarkers","DirectionArrowSet",0x721140,512},{"C3dMarkers","DirectionArrowsInit",0x721100,64},
    {"C3dMarkers","ForceRender",0x722870,32},{"C3dMarkers","User3dMarkerDelete",0x721090,256},{"C3dMarkers","User3dMarkerDeleteAll",0x7210B0,64},
    {"C3dMarkers","User3dMarkerFindFirstFreeSlot",0x720FB0,256},{"C3dMarkers","User3dMarkerSet",0x720FD0,512},{"C3dMarkers","LoadUser3dMarkers",0x5D42E0,64},
    {"C3dMarkers","SaveUser3dMarkers",0x5D4300,64},{"C3dMarkers","User3dMarkerAtomicCB",0x7210D0,64},{"C3dMarkers","Update",0x7227B0,256},
    {"C3dMarkers","PlaceMarkerSet",0x725BA0,512},{"C3dMarkers","PlaceMarkerCone",0x726D40,1024},{"C3dMarkers","PlaceMarker",0x725120,4096},
    {"C3dMarker","Render",0x7223D0,2048},{"C3dMarkers","User3dMarkersDraw",0x723240,512},{"C3dMarkers","DirectionArrowsDraw",0x7211F0,1024},
    {"C3dMarkers","Render",0x725040,512},{"C3dMarkers","Init",0x724E40,64},{"C3dMarkers","Shutdown",0x722710,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpptrigcompound6markers-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpptrigcompound6markers-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpptrigcompound6markers");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,26> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // estado global que se toca: se guarda y se restaura
    struct Blob { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{&C3dMarkers::m_aMarkerArray,sizeof(C3dMarkers::m_aMarkerArray)},{&C3dMarkers::ms_directionArrows,sizeof(C3dMarkers::ms_directionArrows)},
        {&C3dMarkers::ms_user3dMarkers,sizeof(C3dMarkers::ms_user3dMarkers)},{&C3dMarkers::m_pRpClumpArray,sizeof(C3dMarkers::m_pRpClumpArray)},
        {&C3dMarkers::m_angleDiamondDeg,4},{&C3dMarkers::NumActiveMarkers,4},{&C3dMarkers::IgnoreRenderLimit,1},{&C3dMarkers::m_user3dMarkerColor,4},{&C3dMarkers::m_colDiamond,1},{reinterpret_cast<void*>(0xC4028C),12},{reinterpret_cast<void*>(0xC4029C),4},{reinterpret_cast<void*>(0x8D5E44),1}};
    for(auto& b:blobs)b.saved.assign(static_cast<uint8*>(b.p),static_cast<uint8*>(b.p)+b.n);
    { const unsigned pc=_control87(0,0)&_MCW_PC; Write({{"event","info"},{"precision_bits",pc==_PC_24 ? 24 : pc==_PC_53 ? 53 : 64}}); }
    const auto savedTime=CTimer::m_snTimeInMilliseconds; const auto savedStep=CTimer::ms_fTimeStep;
    std::array<bool,26> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(0x749B70,ForAllAtomics), d2(0x749E60,AtomicClone), d3(0x7F0410,FrameCreate), d4(0x74BF20,AtomicSetFrame), d5(0x749DC0,AtomicDestroy), d6(0x7F05A0,FrameDestroy), d7(0x7F0910,FrameUpdate);
        EntryPatch d8(bit_cast<uintptr_t>(&CVisibilityPlugins::SetAtomicRenderCallback),SetRenderCB), d8e(0x7328A0,SetRenderCB);
        EntryPatch d9(bit_cast<uintptr_t>(&FindPlayerCentreOfWorld),Centre), d9e(0x56E250,Centre);
        EntryPatch d10(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),LineOfSight), d10e(0x56BA00,LineOfSight);
        EntryPatch d11(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),WaterNoWaves), d11e(0x6E8580,WaterNoWaves);
        EntryPatch d12(bit_cast<uintptr_t>(&CColStore::HasCollisionLoaded),ColLoaded), d12e(0x410CE0,ColLoaded);
        EntryPatch d13(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),GroundZ), d13e(0x5696C0,GroundZ);
        EntryPatch d14(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),LoadWB), d14e(0x5D1300,LoadWB);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; auto& fpGet=RWSRCGLOBAL(dOpenDevice).fpRenderStateGet; const auto savedSet=fpSet; const auto savedGet=fpGet;
        fpSet=StateSet; fpGet=StateGet;
        EntryPatch d16(0x7F10B0,FrameIdentity), d17(0x7F0E30,FrameTranslate), d18(0x749B20,ClumpRender);
        EntryPatch e1(bit_cast<uintptr_t>(&SetBrightMarkerColours),BrightColours), e1e(0x735BD0,BrightColours);
        EntryPatch e2(bit_cast<uintptr_t>(&ReSetAmbientAndDirectionalColours),ResetColours), e2e(0x735C40,ResetColours);
        EntryPatch e3(bit_cast<uintptr_t>(&ActivateDirectional),ActivateDir), e3e(0x735C80,ActivateDir);
        EntryPatch e4(bit_cast<uintptr_t>(static_cast<void(*)(RwRGBAReal*)>(&SetAmbientColours)),SetAmbient), e4e(0x735D50,SetAmbient);
        EntryPatch e5(bit_cast<uintptr_t>(&SetDirectionalColours),SetDirectional), e5e(0x735D70,SetDirectional);
        EntryPatch e6(bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible)),SphereVisible), e6e(0x420C40,SphereVisible);
        EntryPatch e7(bit_cast<uintptr_t>(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo)),GetModelByName), e7e(0x4C5940,GetModelByName);
        EntryPatch e8(bit_cast<uintptr_t>(&CBaseModelInfo::AddRef),ModelAddRef), e8e(0x4C4BA0,ModelAddRef);
        EntryPatch e9(bit_cast<uintptr_t>(&CStreaming::RequestModel),ReqModel), e9e(0x4087E0,ReqModel);
        EntryPatch e10(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),LoadAllReq), e10e(0x40EA10,LoadAllReq);
        EntryPatch e11(0x74A290,ClumpCreate), e12(0x74A490,ClumpAddAtomic), e13(0x74A310,ClumpDestroy);
        EntryPatch e14(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FindTxd), e14e(0x731850,FindTxd);
        EntryPatch e15(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),PushTxd), e15e(0x7316A0,PushTxd);
        EntryPatch e16(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),PopTxd), e16e(0x7316B0,PopTxd);
        EntryPatch e17(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),SetTxd), e17e(0x7319C0,SetTxd);
        EntryPatch e18(bit_cast<uintptr_t>(&CFileMgr::ChangeDir),ChangeDir), e18e(0x538730,ChangeDir);
        FillModelTraps(std::make_integer_sequence<int,24>{}); modelVtbl[11]=reinterpret_cast<void*>(&CreateInstance); *reinterpret_cast<void**>(fakeModel.data())=modelVtbl.data();
        EntryPatch d15(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),SaveWB), d15e(0x5D1270,SaveWB);
        std::string only; if(std::ifstream of(dir/"cpptrigcompound6markers-only.txt"); of)std::getline(of,only); // depuracion: limitar a una funcion
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; if(!only.empty()&&only!=fn.name)continue;
            if((std::string(fn.cls)+"::"+fn.name)!="C3dMarkers::PlaceMarker"&&(std::string(fn.cls)+"::"+fn.name)!="C3dMarkers::User3dMarkersDraw")continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3D3Du; trace=json::array(); cloneNext=0; frameNext=0; loadPos=0;
                    CTimer::m_snTimeInMilliseconds=100000+next()%50000; CTimer::ms_fTimeStep=uni(0.2f,3.f);
                    instNext=0; clumpNext=0; *reinterpret_cast<uint8*>(0x8D5E44)=uint8(next()); // alfa guardado de C3dMarker::Render
                    for(auto& a:atomics){ std::memset(a.data(),0,a.size()); *reinterpret_cast<void**>(a.data()+0x18)=geometry.data(); *reinterpret_cast<void**>(a.data()+0x48)=reinterpret_cast<void*>(&AtomicRenderCB); *reinterpret_cast<float*>(a.data()+0x28)=uni(0.5f,4.f); /* radio de la esfera envolvente */ }
                    for(size_t k=0;k<atomics.size();++k)*reinterpret_cast<void**>(atomics[k].data()+4)=frames[k%frames.size()].data();
                    for(auto& fr:frames){ for(auto& b:fr)b=uint8(next()); auto* m=reinterpret_cast<RwMatrix*>(fr.data()+0x10); *m=RwMatrix{}; m->right={1,0,0}; m->up={0,1,0}; m->at={0,0,1}; m->pos={uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)}; m->flags=3; }
                    for(auto& m:rwmats){ std::memset(m.data(),0,m.size()); auto* rm=reinterpret_cast<RwMatrix*>(m.data()); rm->right={1,0,0}; rm->up={0,1,0}; rm->at={0,0,1}; rm->pos={uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)}; rm->flags=next()%4; }
                    for(auto& b:geometry)b=uint8(next()); *reinterpret_cast<void**>(geometry.data()+0x20)=matList.data(); matList[0]=material.data();
                    for(auto& b:material)b=uint8(next());
                    for(size_t k=0;k<clumps.size();++k){ std::memset(clumps[k].data(),0,clumps[k].size()); *reinterpret_cast<void**>(clumps[k].data()+4)=frames[N+(k%2)].data(); }
                    for(size_t k=0;k<C3dMarkers::m_pRpClumpArray.size();++k)C3dMarkers::m_pRpClumpArray[k]=(next()%6||std::string(fn.name).ends_with("Draw")||std::string(fn.name)=="Render") ? reinterpret_cast<RpClump*>(clumps[k].data()) : nullptr;
                    if(next()%3==0){ C3dMarkers::m_pRpClumpArray[6]=C3dMarkers::m_pRpClumpArray[5]; C3dMarkers::m_pRpClumpArray[0]=C3dMarkers::m_pRpClumpArray[5]; } // tipos que comparten clump
                    centre=CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,20.f)};
                    losHit=next()%2; losZ=uni(0.f,30.f); waterFound=next()%2; waterZ=uni(-5.f,25.f); colLoaded=next()%4!=0; groundFound=next()%3!=0; groundZ=uni(-5.f,25.f);
                    loadStream.resize(0x1C*5+7); for(auto& b:loadStream)b=uint8(next());
                    C3dMarkers::m_angleDiamondDeg=uni(0.f,720.f); C3dMarkers::NumActiveMarkers=next()%40; C3dMarkers::IgnoreRenderLimit=next()%2;
                    C3dMarkers::m_user3dMarkerColor=CRGBA(uint8(next()),uint8(next()),uint8(next()),uint8(next())); C3dMarkers::m_colDiamond=uint8(next());
                    for(auto& a:C3dMarkers::ms_directionArrows){ auto* raw=reinterpret_cast<uint8*>(&a); for(size_t b=0;b<sizeof(a);++b)raw[b]=uint8(next()); a.m_bIsUsed=next()%3==0;
                        // valores realistas: posicion, tamano y direccion (no bytes aleatorios)
                        a.m_vecPosition=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)}; a.m_fSize=uni(0.2f,4.f);
                        a.m_normal=next()%4==0 ? CVector{0.f,0.f,1.f} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; }
                    for(auto& u:C3dMarkers::ms_user3dMarkers){ auto* raw=reinterpret_cast<uint8*>(&u); for(size_t b=0;b<sizeof(u);++b)raw[b]=uint8(next()); u.m_bIsUsed=next()%3==0;
                        u.m_vecPosition=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)}; }
                    // marcadores: posiciones cercanas al centro para que haya de todo
                    auto fillMarker=[&](C3dMarker& m,int k){ auto* raw=reinterpret_cast<uint8*>(&m); for(size_t b=0;b<sizeof(m);++b)raw[b]=uint8(next());
                        CMatrix mat{}; mat.SetRotate(0.f,0.f,uni(-3.f,3.f)); mat.GetPosition()=CVector{centre.x+uni(-40.f,40.f),centre.y+uni(-40.f,40.f),uni(0.f,20.f)};
                        std::memcpy(&m.m_Mat,&mat,sizeof(CMatrix)); // incluye m_pAttachMatrix=null
                        *reinterpret_cast<RwMatrix**>(reinterpret_cast<uint8*>(&m.m_Mat)+0x40)=next()%3 ? reinterpret_cast<RwMatrix*>(rwmats[k].data()) : nullptr;
                        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(&m.m_Mat)+0x44)=0;
                        m.m_Atomic=next()%5 ? reinterpret_cast<RpAtomic*>(atomics[k].data()) : nullptr; m.m_Material=reinterpret_cast<RpMaterial*>(material.data());
                        if(next()%4==0)m.m_Normal=CVector{0.f,0.f,1.f}; else if(next()%6==0)m.m_Normal=CVector{0.f,0.f,-1.f};
                        { const uint32 t=next()%9; m.m_Type=t<7 ? e3dMarkerType(t) : MARKER3D_NA; }
                        if(m.m_Type!=MARKER3D_NA||m.m_ID)m.m_Atomic=reinterpret_cast<RpAtomic*>(atomics[k].data());
                        if(m.m_Atomic)*reinterpret_cast<RwMatrix**>(reinterpret_cast<uint8*>(&m.m_Mat)+0x40)=reinterpret_cast<RwMatrix*>(rwmats[k].data()); // con atomic siempre hay matriz adjunta
                        m.m_IsInUse=m.m_Atomic && next()%2; m.m_IsActive=next()%2; // el juego asume atomic en los marcadores en uso m.m_ID=next()%3==0 ? 0 : 1+next()%12;
                        m.m_PulseFraction=uni(0.f,1.f); m.m_StdSize=uni(0.5f,4.f); m.m_Size=uni(0.5f,4.f); m.m_Brightness=uni(0.f,1.f); m.m_DistToCam2D=uni(0.f,200.f);
                        m.m_Normal=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        const auto& pos=m.m_Mat.GetPosition();
                        if(next()%2){ m.m_LastMapReadX=uint16(int32(pos.x)); m.m_LastMapReadY=next()%4 ? uint16(int32(pos.y)) : uint16(next()); } else { m.m_LastMapReadX=uint16(next()); m.m_LastMapReadY=uint16(next()); }
                        m.m_LastMapReadResultZ=uni(0.f,10.f); m.m_RoofHeight=next()%2 ? 65535.f : uni(0.f,30.f);
                        m.m_LastPosition=next()%3 ? CVector{} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        m.m_StartTime=next()%100000; m.m_OnScreenTestTime=CTimer::m_snTimeInMilliseconds-next()%4000; };
                    for(int k=0;k<N;++k)fillMarker(C3dMarkers::m_aMarkerArray[k],k);
                    fillMarker(*reinterpret_cast<C3dMarker*>(marker.data()),N);
                    std::array<std::string,N> pre{}; for(int k=0;k<N;++k)pre[k]=Hash(&C3dMarkers::m_aMarkerArray[k],sizeof(C3dMarker));
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; json ret=nullptr; void* self=marker.data();
                    // argumentos comunes
                    argPos=CVector{centre.x+uni(-30.f,30.f),centre.y+uni(-30.f,30.f),uni(0.f,20.f)};
                    if(next()%4==0){ const auto& mp=C3dMarkers::m_aMarkerArray[next()%N].m_Mat.GetPosition(); argPos=CVector{mp.x,mp.y,argPos.z}; }
                    const uint32 argId=1+next()%12; const int argType=next()%10==0 ? 7+int(next()%3) : int(next()%7);
                    const float argSize=uni(0.3f,5.f); const uint8 cr=uint8(next()),cg=uint8(next()),cb=uint8(next()),ca=uint8(next()); const uint16 pp=uint16(next()); const float pf=uni(0.f,1.f);
                    const int16 rr=int16(next()%3==0 ? 0 : int(next()%20)-10); const CVector normal{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const bool zc=next()%2;
                    if(nm=="AddMarker")ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,int,float,uint8,uint8,uint8,uint8,uint16,float,int16)>(fn.address)(self,argId,int(next()%7),argSize,cr,cg,cb,ca,pp,pf,rr);
                    else if(nm=="DeleteMarkerObject"){ reinterpret_cast<C3dMarker*>(self)->m_Atomic=reinterpret_cast<RpAtomic*>(atomics[N].data()); reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self); }
                    else if(nm=="IsZCoordinateUpToDate")ret=reinterpret_cast<bool(__thiscall*)(void*)>(fn.address)(self);
                    else if(nm=="SetZCoordinateIfNotUpToDate")reinterpret_cast<void(__thiscall*)(void*,float)>(fn.address)(self,uni(-10.f,30.f));
                    else if(nm=="UpdateZCoordinate"){ const auto& mp=reinterpret_cast<C3dMarker*>(self)->m_Mat.GetPosition(); CVector p=next()%2 ? CVector{mp.x+uni(-120.f,120.f),mp.y+uni(-120.f,120.f),uni(0.f,9.f)} : argPos;
                        reinterpret_cast<void(__thiscall*)(void*,CVector,float)>(fn.address)(self,p,uni(0.f,30.f)); }
                    else if(nm=="DirectionArrowFindFirstFreeSlot"||nm=="User3dMarkerFindFirstFreeSlot")ret=reinterpret_cast<int32(__cdecl*)()>(fn.address)();
                    else if(nm=="DirectionArrowSet")reinterpret_cast<void(__cdecl*)(CVector,float,int32,int32,int32,int32,float,float,float)>(fn.address)(argPos,argSize,int32(next()),int32(next()),int32(next()),int32(next()),normal.x,normal.y,normal.z);
                    else if(nm=="DirectionArrowsInit"||nm=="User3dMarkerDeleteAll"||nm=="Update")reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    else if(nm=="ForceRender")reinterpret_cast<void(__cdecl*)(bool)>(fn.address)(next()%2);
                    else if(nm=="User3dMarkerDelete")reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(int32(next()%9)-2);
                    else if(nm=="User3dMarkerSet")ret=reinterpret_cast<int32(__cdecl*)(float,float,float,int32)>(fn.address)(argPos.x,argPos.y,argPos.z,int32(next()%15));
                    else if(nm=="User3dMarkersDraw"||nm=="DirectionArrowsDraw"||nm=="Init"||nm=="Shutdown"||(nm=="Render"&&std::string(fn.cls)=="C3dMarkers"))reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    else if(nm=="Render"){ auto* mk=reinterpret_cast<C3dMarker*>(self); if(next()%6==0)mk->m_Atomic=nullptr; reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self); }
                    else if(nm=="LoadUser3dMarkers"||nm=="SaveUser3dMarkers")ret=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
                    else if(nm=="User3dMarkerAtomicCB")ret=Name(uintptr_t(reinterpret_cast<RpAtomic*(__cdecl*)(RpAtomic*,void*)>(fn.address)(reinterpret_cast<RpAtomic*>(atomics[N].data()),nullptr)));
                    else if(nm=="PlaceMarkerSet"||nm=="PlaceMarkerCone"){
                        EntryPatch pl(bit_cast<uintptr_t>(&C3dMarkers::PlaceMarker),FakePlace), ple(0x725120,FakePlace);
                        if(nm=="PlaceMarkerSet")reinterpret_cast<void(__cdecl*)(uint32,int,CVector&,float,uint8,uint8,uint8,uint8,uint16,float,int16)>(fn.address)(argId,argType,argPos,argSize,cr,cg,cb,ca,pp,pf,rr);
                        else { if(next()%3==0)argPos=TheCamera.GetPosition()+CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            reinterpret_cast<void(__cdecl*)(uint32,CVector&,float,uint8,uint8,uint8,uint8,uint16,float,int16,bool)>(fn.address)(argId,argPos,argSize,cr,cg,cb,ca,pp,pf,rr,next()%2); } }
                    else { auto* r=reinterpret_cast<C3dMarker*(__cdecl*)(uint32,int,CVector&,float,uint8,uint8,uint8,uint8,uint16,float,int16,CVector,bool)>(fn.address)(argId,argType,argPos,argSize,cr,cg,cb,ca,pp,pf,rr,normal,zc); ret=Name(uintptr_t(r)); }
                    json r=json::object(); r["ret"]=ret; r["trace"]=trace; r["switches"]=switches; r["marker"]=Region(marker.data(),marker.size());
                    { json ch=json::array(); for(int k=0;k<N;++k)if(Hash(&C3dMarkers::m_aMarkerArray[k],sizeof(C3dMarker))!=pre[k])ch.push_back({k,Region(&C3dMarkers::m_aMarkerArray[k],sizeof(C3dMarker))}); r["array"]=ch; }
                    r["arrows"]=ReversingProbe::Bytes(&C3dMarkers::ms_directionArrows,sizeof(C3dMarkers::ms_directionArrows));
                    r["user"]=ReversingProbe::Bytes(&C3dMarkers::ms_user3dMarkers,sizeof(C3dMarkers::ms_user3dMarkers));
                    r["saved_alpha"]=*reinterpret_cast<uint8*>(0x8D5E44); r["globals"]=json::array({std::bit_cast<uint32>(float(C3dMarkers::m_angleDiamondDeg)),uint32(C3dMarkers::NumActiveMarkers),bool(C3dMarkers::IgnoreRenderLimit),ReversingProbe::Bytes(&C3dMarkers::m_user3dMarkerColor,4),int(C3dMarkers::m_colDiamond)});
                    r["centre"]=json::array({std::bit_cast<uint32>(centre.x),std::bit_cast<uint32>(centre.y),std::bit_cast<uint32>(centre.z)}); r["pos"]=json::array({std::bit_cast<uint32>(argPos.x),std::bit_cast<uint32>(argPos.y),std::bit_cast<uint32>(argPos.z)});
                    r["geometry"]=Hash(geometry.data(),geometry.size()); r["material"]=Hash(material.data(),material.size());
                    r["rwmats"]=Hash(rwmats.data(),sizeof(rwmats));
                    r["atomics"]=Hash(atomics.data(),sizeof(atomics)); r["frames"]=Hash(frames.data(),sizeof(frames)); r["clumpframes"]=json::array({ReversingProbe::Bytes(frames[N].data(),0x60),ReversingProbe::Bytes(frames[N+1].data(),0x60)});
                    return r;
                };
                if(std::string(fn.name)=="PlaceMarker"||std::string(fn.name)=="User3dMarkersDraw")Write({{"event","progress"},{"case",i}});
                if(std::string(fn.name)=="Render")Write({{"event","progress"},{"case",i}});
                json original, candidate;
                try { original=run(false); } catch(const std::exception& e){ Write({{"event","error"},{"reason","exception original"},{"what",e.what()},{"case",i}}); return; }
                if(std::string(fn.name)=="Render")Write({{"event","progress_original_done"}});
                try { candidate=run(true); } catch(const std::exception& e){ Write({{"event","error"},{"reason","exception candidate"},{"what",e.what()},{"case",i}}); return; }
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",original},{"candidate",candidate}}); }
            }
            Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
        fpSet=savedSet; fpGet=savedGet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(auto& b:blobs)std::memcpy(b.p,b.saved.data(),b.n);
    CTimer::m_snTimeInMilliseconds=savedTime; CTimer::ms_fTimeStep=savedStep;
    bool restored=true; for(auto& b:blobs)restored&=std::memcmp(b.p,b.saved.data(),b.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
