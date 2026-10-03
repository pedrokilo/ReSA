#pragma once
// Banco dentro del motor: efectos, objetos procedurales y tareas de interior (lista de 8):
// Fx_c, ProcObjectMan_c, CTaskInteriorBeInHouse, CTaskInteriorBeInOffice y CTaskInteriorUseInfo (40 funciones).
// Original (hooks del lote desactivados) frente a candidato con fixtures deterministas. Nunca se crean sistemas de
// efectos ni objetos reales: CreateFxSystem/PlayAndKill, FxRwMatrixCreate/Destroy, Im3D y los estados de render se
// simulan; el gestor de objetos procedurales es un buffer propio (la lista global 0xBBC8B8 se cambia por una vacia
// y se restaura) con entidades falsas de vtable trazada; las tareas usan peaton, inteligencia, interiores y subtareas
// falsos. Se simulan y trazan: rand/GetRandomNumber, la camara (matriz/posicion propias durante la llamada),
// fpRenderStateGet/Set, RwIm3DTransform/RenderPrimitive/End, CFileMgr::OpenFile/CloseFile, CFileLoader::LoadLine,
// ProcSurfaceInfo_c::Init/Exit/AddObjects, CWorld::Remove, InteriorGroup_c::FindInteriorInfo/GetPed,
// InteriorManager_c::AreAnimsLoaded y CPed::Say. Todo global tocado se guarda y se restaura.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/Fx/Fx.h"
#include "../../source/game_sa/Fx/FxManager.h"
#include "../../source/game_sa/Fx/FxSystem.h"
#include "../../source/game_sa/Plant/ProcObjectMan.h"
#include "../../source/game_sa/Plant/ProcSurfaceInfo.h"
#include "../../source/game_sa/Plant/PlantLocTri.h"
#include "../../source/game_sa/Interior/InteriorInfo_t.h"
#include "../../source/game_sa/Interior/Interior_c.h"
#include "../../source/game_sa/Interior/InteriorGroup_c.h"
#include "../../source/game_sa/Interior/InteriorManager_c.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.h"
#include "../../source/game_sa/Pools/TaskPool.h"
#include "../../source/game_sa/FileMgr.h"
#include "../../source/game_sa/FileLoader.h"
#include "../ProbeRuntime.hpp"

namespace Fx8Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hash(const void* p,size_t n){ return std::to_string(Fnv(p,n)); }
inline uint32 Str(const char* s){ return s ? uint32(Fnv(s,strlen(s))) : 0u; }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
// cuerpo real de una funcion de la DLL (salta la tabla ILT)
inline uintptr_t Body(uintptr_t p){ while(*reinterpret_cast<const uint8*>(p)==0xE9)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }
template<class T> T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }

// --- memoria de los fixtures (direcciones fijas: iguales en los dos lados) ---
constexpr uintptr_t TEMPBUF=0xC4D958; constexpr size_t TEMPBUF_N=TOTAL_TEMP_BUFFER_3DVERTICES*0x24;
constexpr uintptr_t GFX=0xA9AE00; constexpr size_t GFX_N=0x70;
constexpr uintptr_t PROC_LIST=0xBBC8B8;                                       // g_procObjMan.m_ObjectsList
constexpr size_t PM_INFOS=8, PM_ITEMS=0x2408, PM_LIST=0x4C08, PSI_N=0x48, ITEM_N=0x14;
alignas(16) inline std::array<uint8,sizeof(ProcObjectMan_c)> pm{};
alignas(16) inline std::array<std::array<uint8,0x40>,16> ents{};
alignas(16) inline std::array<uint8,0x60> tri{}, tri2{};
alignas(16) inline std::array<uint8,0x48> camMat{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> fxSys{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> rasters{};
alignas(16) inline std::array<uint8,0x48> ltm{};
alignas(16) inline std::array<uint8,0x40> fxMat{};
alignas(16) inline std::array<uint8,0x40> vecs{};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,0x600> keeper{};
alignas(16) inline std::array<uint8,0x40> keeperIntel{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> fakes{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,sizeof(Interior_c)> interior{};
alignas(16) inline std::array<uint8,sizeof(Interior_c)> interior2{};
alignas(16) inline std::array<uint8,sizeof(InteriorGroup_c)> igroup{};
alignas(16) inline std::array<InteriorInfo_t,4> infos{};
alignas(16) inline std::array<uint8,0x40> collEnt{};
inline CPed* Ped(){ return reinterpret_cast<CPed*>(ped.data()); }
inline CTask* Fake(int n){ return reinterpret_cast<CTask*>(fakes[n].data()); }

struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(v-uintptr_t(b.p));
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false){ json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json Vp(const void* p){ const auto* f=static_cast<const float*>(p); return json::array({F(f[0]),F(f[1]),F(f[2])}); }
// los buffers de pila del llamador no tienen direccion comparable entre EXE y DLL: se anotan como "stack"
inline json StackName(const void* d){ ULONG_PTR lo{}, hi{}; GetCurrentThreadStackLimits(&lo,&hi); const auto v=uintptr_t(d); if(v>=lo&&v<hi)return "stack"; return Name(v); }

// --- RNG ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand(){ trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }

// --- efectos ---
inline uint32 fxMask{}, fxCalls{};
inline FxSystem_c* __fastcall FakeCreateFx(void* self,void*,const char* name,const CVector& p,RwMatrix* m,bool b){
    const uint32 c=fxCalls++; const bool r=(fxMask>>(c%32))&1; trace.push_back({"create_fx",Name(uintptr_t(self)),std::string(name ? name : ""),F(p.x),F(p.y),F(p.z),Name(uintptr_t(m)),b,r});
    if(!r)return nullptr; return reinterpret_cast<FxSystem_c*>(fxSys[c%2].data()); }
inline void __fastcall FakePlayAndKill(void* s,void*){ trace.push_back({"play_and_kill",Name(uintptr_t(s))}); }
inline void* rsGetValue{};
inline RwBool __cdecl FakeStateGet(RwRenderState st,void* v){ trace.push_back({"render_state_get",int(st),StackName(v)}); if(v)*static_cast<void**>(v)=rsGetValue; return TRUE; }
inline RwBool __cdecl FakeStateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),Name(uintptr_t(v))}); return TRUE; }
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){
    trace.push_back({"im3d_transform",Name(uintptr_t(v)),n,Name(uintptr_t(m)),f,Hash(reinterpret_cast<void*>(TEMPBUF),TEMPBUF_N)}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DPrim(RwPrimitiveType t){ trace.push_back({"im3d_render",int(t)}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
inline RwMatrix* __fastcall FakeMatCreate(void* self,void*){ trace.push_back({"fx_matrix_create",Name(uintptr_t(self))}); return reinterpret_cast<RwMatrix*>(fxMat.data()); }
inline void __fastcall FakeMatDestroy(void* self,void*,RwMatrix* m){ trace.push_back({"fx_matrix_destroy",Name(uintptr_t(self)),Name(uintptr_t(m))}); }

// --- objetos procedurales ---
inline uint32 PsiIndex(const void* p){ return uint32((intptr_t(p)-intptr_t(pm.data())-intptr_t(PM_INFOS))/intptr_t(PSI_N)); }
inline bool __fastcall FakePsiInit(void* self,void*,const char* st,const char* mn,float spacing,float minDist,int32 minRot,int32 maxRot,
                                   float minScale,float maxScale,float minScaleZ,float maxScaleZ,float zMin,float zMax,uint32 align,uint32 useGrid){
    trace.push_back({"psi_init",PsiIndex(self),std::string(st),std::string(mn),F(spacing),F(minDist),minRot,maxRot,F(minScale),F(maxScale),F(minScaleZ),F(maxScaleZ),
                     F(zMin),F(zMax),align&0xFF,useGrid&0xFF}); return true; }
inline void __fastcall FakePsiExit(void* self,void*){ trace.push_back({"psi_exit",PsiIndex(self)}); }
inline std::array<int32,8> addRets{}; inline uint32 addCalls{};
inline int32 __fastcall FakeAddObjects(void* self,void*,CPlantLocTri* p){ trace.push_back({"psi_add_objects",PsiIndex(self),Name(uintptr_t(p))}); return addRets[addCalls++%8]; }
inline uint32 loadCalls{};
inline void __fastcall FakeLoadData(void* self,void*){ trace.push_back({"load_data_file",Name(uintptr_t(self)),At<int32>(self,4),At<int32>(self,0)}); ++loadCalls; }
inline std::array<std::array<char,160>,32> lines{}; inline size_t nlines{}, linePos{};
inline FILESTREAM __cdecl FakeOpen(const char* path,const char* mode){ trace.push_back({"open",std::string(path),std::string(mode)}); return reinterpret_cast<FILESTREAM>(0x5157); }
inline char* __cdecl FakeLoadLine(FILESTREAM f){ trace.push_back({"load_line",uintptr_t(f)}); return linePos<nlines ? lines[linePos++].data() : nullptr; }
inline int32 __cdecl FakeClose(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Name(uintptr_t(e))}); }
inline void* __fastcall FakeEntDtor(void* o,void*,uint32 f){ trace.push_back({"ent_delete",Name(uintptr_t(o)),f}); return o; }
inline void __fastcall FakeEntDeleteRw(void* o,void*){ trace.push_back({"ent_delete_rw",Name(uintptr_t(o))}); }
template<int N> void* __fastcall EntTrap(void*,void*){ trace.push_back({"ent_unexpected_slot",N}); return nullptr; }
inline std::array<void*,64> entVtbl{};
template<int... N> void FillEntTraps(std::integer_sequence<int,N...>){ ((entVtbl[N]=reinterpret_cast<void*>(&EntTrap<N>)),...); }

// --- tareas ---
constexpr size_t FK_TYPE=0x38;
inline bool abortRet{};
inline CTask* __fastcall FSub(CTask*,void*){ return nullptr; }
inline bool __fastcall FSimple(CTask*,void*){ return false; }
inline int __fastcall FType(CTask* t,void*){ trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline void* __fastcall FDtor(CTask* t,void*,unsigned f){ trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e){ trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return abortRet; }
template<int N> void* __fastcall FTrap(CTask*,void*){ trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>){ ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline uint32 findMask{}, findCalls{}, animsMask{}, animsCalls{};
inline bool __fastcall FakeFindInfo(void* g,void*,int32 type,InteriorInfo_t** info,Interior_c** in){
    const uint32 c=findCalls++; const bool r=c>=6||((findMask>>(c%32))&1); trace.push_back({"find_info",Name(uintptr_t(g)),type,r});
    if(r){ *info=&infos[(uint32(type)+c)%4]; *in=reinterpret_cast<Interior_c*>(c%2 ? interior2.data() : interior.data()); } return r; }
inline CPed* __fastcall FakeGetPed(void* g,void*,int32 idx){ trace.push_back({"get_ped",Name(uintptr_t(g)),idx}); return reinterpret_cast<CPed*>(keeper.data()); }
inline bool __fastcall FakeAnimsLoaded(void* m,void*,int32 b){ trace.push_back({"anims_loaded",Name(uintptr_t(m)),b}); return (animsMask>>(animsCalls++%32))&1; }
inline int16 __fastcall FakeSay(void* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c){ trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),a,b,c}); return 0; }

inline std::vector<bool> Occupancy(){ auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// llamadas __thiscall con medida de la pila (una firma con otro numero de argumentos dejaria esp desplazado)
inline int32 CallThis1(uintptr_t fn,void* self,const void* arg,int32* espDelta){
    uint32 before{}, after{}; int32 r{};
    __asm {
        mov before, esp
        push arg
        mov ecx, self
        call fn
        mov r, eax
        mov after, esp
        mov esp, before
    }
    *espDelta=int32(before)-int32(after); return r;
}
inline int32 CallThis2(uintptr_t fn,void* self,const void* a1,const void* a2,int32* espDelta){
    uint32 before{}, after{}; int32 r{};
    __asm {
        mov before, esp
        push a2
        push a1
        mov ecx, self
        call fn
        mov r, eax
        mov after, esp
        mov esp, before
    }
    *espDelta=int32(before)-int32(after); return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,40> fns{{
    {"Fx_c","TriggerWaterSplash",0x4A1070,0,-1,512},{"Fx_c","TriggerBulletSplash",0x4A10E0,0,-1,512},{"Fx_c","TriggerFootSplash",0x4A1150,0,-1,512},
    {"Fx_c","RenderBegin",0x4A13B0,0,-1,256},{"Fx_c","RenderAddTri_",0x4A1410,0,-1,1024},{"Fx_c","RenderEnd",0x4A1600,0,-1,256},
    {"Fx_c","RotateVecIntoVec",0x4A1660,0,-1,1024},{"Fx_c","RotateVecAboutVec",0x4A1780,0,-1,1024},
    {"ProcObjectMan_c","Init",0x5A3EA0,0,-1,64},{"ProcObjectMan_c","Update",0x5A3110,0,-1,64},{"ProcObjectMan_c","Exit",0x5A3EE0,0,-1,256},
    {"ProcObjectMan_c","LoadDataFile",0x5A3140,0,-1,512},{"ProcObjectMan_c","GetEntityFromPool",0x5A3120,0,-1,256},
    {"ProcObjectMan_c","ReturnEntityToPool",0x5A3130,0,-1,256},{"ProcObjectMan_c","ProcessTriangleAdded",0x5A3F20,0,-1,512},
    {"ProcObjectMan_c","ProcessTriangleRemoved",0x5A3F70,0,-1,1024},
    {"CTaskInteriorBeInHouse","Constructor",0x674FC0,0x8701E8,-1,128},{"CTaskInteriorBeInHouse","Destructor",0x675000,0x8701E8,-1,128},
    {"CTaskInteriorBeInHouse","GetInfoForPedToUse",0x6750A0,0x8701E8,-1,1024},{"CTaskInteriorBeInHouse","Clone",0,0x8701E8,1,128},
    {"CTaskInteriorBeInHouse","GetTaskType",0,0x8701E8,4,64},{"CTaskInteriorBeInHouse","CreateNextSubTask",0,0x8701E8,8,512},
    {"CTaskInteriorBeInHouse","CreateFirstSubTask",0,0x8701E8,9,512},{"CTaskInteriorBeInHouse","ControlSubTask",0,0x8701E8,10,128},
    {"CTaskInteriorBeInOffice","Constructor",0x675220,0x870218,-1,128},{"CTaskInteriorBeInOffice","Destructor",0x675260,0x870218,-1,128},
    {"CTaskInteriorBeInOffice","GetInfoForPedToUse",0x675300,0x870218,-1,1024},{"CTaskInteriorBeInOffice","Clone",0,0x870218,1,128},
    {"CTaskInteriorBeInOffice","GetTaskType",0,0x870218,4,64},{"CTaskInteriorBeInOffice","CreateNextSubTask",0,0x870218,8,512},
    {"CTaskInteriorBeInOffice","CreateFirstSubTask",0,0x870218,9,512},{"CTaskInteriorBeInOffice","ControlSubTask",0,0x870218,10,128},
    {"CTaskInteriorUseInfo","Constructor",0x675A50,0x8702E8,-1,128},{"CTaskInteriorUseInfo","Destructor",0x675A90,0x8702E8,-1,128},
    {"CTaskInteriorUseInfo","Clone",0,0x8702E8,1,128},{"CTaskInteriorUseInfo","GetTaskType",0,0x8702E8,4,64},
    {"CTaskInteriorUseInfo","MakeAbortable",0,0x8702E8,6,256},{"CTaskInteriorUseInfo","CreateNextSubTask",0,0x8702E8,8,1024},
    {"CTaskInteriorUseInfo","CreateFirstSubTask",0,0x8702E8,9,256},{"CTaskInteriorUseInfo","ControlSubTask",0,0x8702E8,10,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fx8-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fx8-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fx8");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fx8-only.txt"); if(f)std::getline(f,only); }
    const auto& all=fns;
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(all.size());
    for(size_t f=0;f<all.size();++f)if(!(hooks[f]=FindHook(all[f].cls,all[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(all[f].cls)+"::"+all[f].name}});return;}
    std::vector<uint8> enabled(all.size()),locked(all.size());
    for(size_t f=0;f<all.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillEntTraps(std::make_integer_sequence<int,64>{}); entVtbl[0]=reinterpret_cast<void*>(&FakeEntDtor); entVtbl[8]=reinterpret_cast<void*>(&FakeEntDeleteRw);
    bufs={{pm.data(),pm.size(),"pm"},{ents.data(),sizeof(ents),"ent"},{tri.data(),tri.size(),"tri"},{tri2.data(),tri2.size(),"tri2"},{camMat.data(),camMat.size(),"cam_mat"},
          {fxSys.data(),sizeof(fxSys),"fx_sys"},{rasters.data(),sizeof(rasters),"raster"},{ltm.data(),ltm.size(),"ltm"},{fxMat.data(),fxMat.size(),"fx_mat"},
          {vecs.data(),vecs.size(),"vecs"},{ped.data(),ped.size(),"ped"},{intel.data(),intel.size(),"intel"},{keeper.data(),keeper.size(),"keeper"},
          {keeperIntel.data(),keeperIntel.size(),"keeper_intel"},{clump.data(),clump.size(),"clump"},{fakes.data(),sizeof(fakes),"fake"},{object.data(),object.size(),"object"},
          {interior.data(),interior.size(),"interior"},{interior2.data(),interior2.size(),"interior2"},{igroup.data(),igroup.size(),"igroup"},
          {infos.data(),sizeof(infos),"info"},{collEnt.data(),collEnt.size(),"coll_ent"},{reinterpret_cast<void*>(TEMPBUF),TEMPBUF_N,"tempbuf"},
          {reinterpret_cast<void*>(GFX),GFX_N,"gfx"},{&g_fxMan,sizeof(FxManager_c),"fxman"},{&g_interiorMan,sizeof(InteriorManager_c),"intman"},
          {reinterpret_cast<void*>(PROC_LIST),0xC,"proc_list"}};

    // estado global guardado
    auto* gfx=reinterpret_cast<uint8*>(GFX); const std::vector<uint8> savedGfx(gfx,gfx+GFX_N);
    auto* tbuf=reinterpret_cast<uint8*>(TEMPBUF); const std::vector<uint8> savedTbuf(tbuf,tbuf+TEMPBUF_N);
    auto* plist=reinterpret_cast<uint8*>(PROC_LIST); const std::vector<uint8> savedPlist(plist,plist+0xC);
    auto& camMatrix=G<void*>(0xB6F03C); void* const savedCamMatrix=camMatrix;
    auto& camPlace=G<CVector>(0xB6F02C); const CVector savedCamPlace=camPlace;
    auto& tempObjs=G<uint16>(0xBB4A70); const uint16 savedTempObjs=tempObjs;
    auto& fpGet=RWSRCGLOBAL(dOpenDevice).fpRenderStateGet; const auto savedGet=fpGet;
    auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet;
    const auto occupancyStart=Occupancy();
    auto stateOk=[&]{ return std::memcmp(gfx,savedGfx.data(),GFX_N)==0&&std::memcmp(tbuf,savedTbuf.data(),TEMPBUF_N)==0&&std::memcmp(plist,savedPlist.data(),0xC)==0
        &&camMatrix==savedCamMatrix&&std::memcmp(&camPlace,&savedCamPlace,sizeof(CVector))==0&&tempObjs==savedTempObjs&&fpGet==savedGet&&fpSet==savedSet&&Occupancy()==occupancyStart; };
    auto restore=[&]{ std::memcpy(gfx,savedGfx.data(),GFX_N); std::memcpy(tbuf,savedTbuf.data(),TEMPBUF_N); std::memcpy(plist,savedPlist.data(),0xC);
        camMatrix=savedCamMatrix; camPlace=savedCamPlace; tempObjs=savedTempObjs; fpGet=savedGet; fpSet=savedSet; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<all.size();++f) {
            const auto& fn=all[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xF8F8u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    fxCalls=0; addCalls=0; loadCalls=0; findCalls=0; animsCalls=0; freshIds.clear();
                    for(auto* h:hooks)h->State(candidate);
                    bool switches;
                    if(fn.slot>=0){ const uintptr_t t=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=(t>=0x400000&&t<0x1000000)!=candidate; }
                    else switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="Fx_c") {
                        auto* fx=gfx;
                        if(nm.rfind("Trigger",0)==0){
                            // camara propia: con matriz (posicion en +0x30) o solo con la posicion de m_placement
                            const CVector c{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,300.f)};
                            std::memset(camMat.data(),0,camMat.size()); std::memcpy(camMat.data()+0x30,&c,sizeof(c));
                            if(next()%4){ camMatrix=camMat.data(); camPlace=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-10.f,10.f)}; } else { camMatrix=nullptr; camPlace=c; }
                            const float d=next()%3==0 ? uni(0.f,24.f) : next()%2 ? uni(24.f,26.f) : uni(26.f,200.f);
                            CVector dir{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; dir.Normalise();
                            CVector p{c.x+dir.x*d,c.y+dir.y*d,c.z+dir.z*d};
                            const uint32 k=next()%16;
                            if(k==0)p=CVector{c.x+25.f,c.y,c.z};                                     // exactamente 625
                            else if(k==1)p=CVector{c.x,c.y-25.f,c.z};
                            else if(k==2)(&p.x)[next()%3]=std::numeric_limits<float>::quiet_NaN();  // NaN: el EXE crea el efecto
                            else if(k==3)(&p.x)[next()%3]=std::numeric_limits<float>::infinity();
                            fxMask=next();
                            std::memcpy(vecs.data(),&p,sizeof(p));
                            EntryPatch c1(Body(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(Const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem))),FakeCreateFx), c1e(0x4A9BE0,FakeCreateFx);
                            EntryPatch c2(Body(MemberAddress(&FxSystem_c::PlayAndKill)),FakePlayAndKill), c2e(0x4AA3D0,FakePlayAndKill);
                            int32 d2{}; CallThis1(a,gfx,vecs.data(),&d2); out["esp"]=d2;
                            out["pos"]=Vp(vecs.data()); }
                        else if(nm=="RenderBegin"){
                            for(size_t k=0x58;k<GFX_N;++k)fx[k]=uint8(next());
                            void* r=next()%5 ? rasters[next()%2].data() : nullptr; void* m=next()%5 ? ltm.data() : nullptr; const uint32 fl=next()%3 ? next()%8 : next();
                            rsGetValue=next()%2 ? r : (next()%2 ? rasters[next()%2].data() : nullptr);
                            fpGet=FakeStateGet; fpSet=FakeStateSet;
                            reinterpret_cast<void(__cdecl*)(void*,void*,uint32)>(a)(r,m,fl);
                            fpGet=savedGet; fpSet=savedSet; }
                        else if(nm=="RenderAddTri_"){
                            for(size_t k=0x50;k<0x58;++k)fx[k]=uint8(next());
                            { const uint32 m=next()%8; At<int32>(fx,0x58)=m==0 ? int32(next()) : m<4 ? int32(2040+next()%8) : int32(next()%2045); }
                            { const uint32 m=next()%8; At<int32>(fx,0x5C)=m==0 ? int32(0x80000000u+next()%4) : m==1 ? -1 : m<4 ? int32(4092+next()%6) : int32(next()%4000); }
                            At<uint32>(fx,0x60)=next()%8; At<void*>(fx,0x64)=next()%3 ? rasters[0].data() : nullptr; At<void*>(fx,0x68)=next()%5 ? ltm.data() : nullptr;
                            At<uint8*>(fx,0x6C)=tbuf+(next()%2045)*0x24; im3dOk=next()%3!=0;
                            float fv[15]; for(int k=0;k<9;++k)fv[k]=uni(-200.f,200.f); for(int k=9;k<15;++k)fv[k]=uni(-1.f,2.f);
                            int32 cv[12]; for(auto& c:cv)c=next()%4 ? int32(next()%256) : int32(next());
                            EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF6B0,FakeIm3DPrim), i3(0x7EF520,FakeIm3DEnd);
                            reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,
                                int32,int32,int32,int32,int32,int32,int32,int32,int32,int32,int32,int32)>(a)(
                                fv[0],fv[1],fv[2],fv[3],fv[4],fv[5],fv[6],fv[7],fv[8],fv[9],fv[10],fv[11],fv[12],fv[13],fv[14],
                                cv[0],cv[1],cv[2],cv[3],cv[4],cv[5],cv[6],cv[7],cv[8],cv[9],cv[10],cv[11]); }
                        else if(nm=="RenderEnd"){
                            for(size_t k=0x58;k<GFX_N;++k)fx[k]=uint8(next());
                            { const uint32 m=next()%8; At<int32>(fx,0x5C)=m<2 ? 0 : m==2 ? -int32(1+next()%3) : int32(next()%700); }
                            At<void*>(fx,0x68)=next()%5 ? ltm.data() : nullptr; At<uint8*>(fx,0x6C)=tbuf+(next()%2045)*0x24; im3dOk=next()%3!=0;
                            EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF6B0,FakeIm3DPrim), i3(0x7EF520,FakeIm3DEnd);
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else { // RotateVecIntoVec / RotateVecAboutVec
                            auto* v=reinterpret_cast<float*>(vecs.data()); // [0..2] res, [4..6] vec, [8..10] eje/alineacion
                            for(int k=0;k<16;++k)v[k]=uni(-1.f,1.f);
                            for(int k=4;k<7;++k)v[k]*=next()%4 ? 1.f : uni(1.f,300.f);
                            if(nm=="RotateVecIntoVec"){
                                // alineacion lejos de la referencia (0,4243; 0,5657; 0,7071) y de cero
                                CVector al{v[8],v[9],v[10]}; if(al.Magnitude()<0.2f)al=CVector{0.f,0.f,1.f};
                                CVector cr=CrossProduct(al,CVector{0.4243f,0.5657f,0.7071f}); if(cr.Magnitude()<0.05f*al.Magnitude())al=CVector{al.z,-al.x,al.y*0.5f+0.3f};
                                if(next()%2)al.Normalise(); v[8]=al.x; v[9]=al.y; v[10]=al.z;
                                fxMat.fill(0xCD);
                                float* res=next()%4==0 ? v+4 : v;
                                EntryPatch m1(Body(MemberAddress(&FxManager_c::FxRwMatrixCreate)),FakeMatCreate), m1e(0x4A9440,FakeMatCreate);
                                EntryPatch m2(Body(MemberAddress(&FxManager_c::FxRwMatrixDestroy)),FakeMatDestroy), m2e(0x4A9460,FakeMatDestroy);
                                reinterpret_cast<void(__cdecl*)(void*,const void*,const void*)>(a)(res,v+4,v+8);
                                out["fx_mat"]=Region(fxMat.data(),fxMat.size()); }
                            else {
                                CVector ax{v[8],v[9],v[10]}; if(next()%3&&ax.Magnitude()>0.01f)ax.Normalise(); v[8]=ax.x; v[9]=ax.y; v[10]=ax.z;
                                const uint32 m=next()%8; const float ang=m==0 ? uni(-1000.f,1000.f) : m==1 ? float(int32(next()%17)-8)*std::bit_cast<float>(0x3FC90FDBu) : uni(-7.f,7.f);
                                const uint32 al=next()%8; float* res=al==0 ? v+4 : al==1 ? v+8 : v;
                                reinterpret_cast<void(__cdecl*)(void*,const void*,const void*,float)>(a)(res,v+4,v+8,ang); }
                            out["vecs"]=Region(vecs.data(),0x30); }
                        out["gfx"]=Region(gfx+0x50,0x20); if(nm=="RenderAddTri_")out["tempbuf"]=Hash(tbuf,TEMPBUF_N);
                    }
                    else if(cls=="ProcObjectMan_c") {
                        for(auto& b:pm)b=uint8(next());
                        auto* P=pm.data();
                        auto item=[&](int k){ return P+PM_ITEMS+k*ITEM_N; };
                        auto info=[&](int k){ return P+PM_INFOS+k*PSI_N; };
                        // lista enlazada de elementos [idx...] dentro de una cabecera (head, tail, count)
                        auto build=[&](uint8* hdr,const std::vector<int>& idx){ uint8* prev=nullptr; uint8* head=nullptr;
                            for(int k:idx){ uint8* it=item(k); At<uint8*>(it,0)=prev; At<uint8*>(it,4)=nullptr; if(prev)At<uint8*>(prev,4)=it; else head=it; prev=it; }
                            At<uint8*>(hdr,0)=head; At<uint8*>(hdr,4)=prev; At<uint32>(hdr,8)=uint32(idx.size()); };
                        auto distinct=[&](int n,std::vector<int>& taken){ std::vector<int> r; while(int(r.size())<n){ const int k=int(next()%512); if(std::find(taken.begin(),taken.end(),k)==taken.end()){taken.push_back(k);r.push_back(k);} } return r; };
                        std::vector<int> used;
                        if(nm=="Init"){ build(P+PM_LIST,{});
                            EntryPatch l1(Body(MemberAddress(&ProcObjectMan_c::LoadDataFile)),FakeLoadData), l1e(0x5A3140,FakeLoadData);
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*)>(a)(P))); }
                        else if(nm=="Update")reinterpret_cast<void(__thiscall*)(void*)>(a)(P);
                        else if(nm=="Exit"){ At<int32>(P,4)=next()%8==0 ? -int32(next()%5) : next()%6==0 ? 128 : int32(next()%129);
                            EntryPatch e1(Body(MemberAddress(&ProcSurfaceInfo_c::Exit)),FakePsiExit), e1e(0x5A3270,FakePsiExit);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(P); }
                        else if(nm=="LoadDataFile"){
                            static const char* surfs[]{"P_GRASS_SHORT","P_GRASS_LONG","P_SAND","SAND_DEEP","P_FLOWERBED","DEFAULT","UNKNOWN_SURF"};
                            static const char* objsN[]{"procobj_tree1","sm_bush_large_1","grassplant","rock_1","veg_fern","x"};
                            auto q=[&](int lo,int hi){ return float(lo*8+int32(next()%uint32((hi-lo)*8+1)))/8.f; };
                            At<int32>(P,4)=int32(next()%40); nlines=1+next()%30; linePos=0;
                            for(size_t k=0;k<nlines;++k){ char* l=lines[k].data(); const uint32 kind=next()%10;
                                if(kind==0)snprintf(l,160,"# %s comment",surfs[next()%7]);
                                else if(kind==1)l[0]=0;
                                else snprintf(l,160,"%s %s %.3f %.3f %d %d %.3f %.3f %.3f %.3f %.3f %.3f %d %d",surfs[next()%7],objsN[next()%6],q(0,20),q(0,200),
                                    int(next()%360)-20,int(next()%400),q(0,3),q(0,3),q(0,3),q(0,3),q(-2,1),q(-1,2),
                                    int(next()%6==0 ? next()%300 : next()%2),int(next()%6==0 ? next()%300 : next()%2)); }
                            EntryPatch o1(Body(bit_cast<uintptr_t>(&CFileMgr::OpenFile)),FakeOpen), o1e(0x538900,FakeOpen);
                            EntryPatch o2(Body(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine))),FakeLoadLine), o2e(0x536F80,FakeLoadLine);
                            EntryPatch o3(Body(bit_cast<uintptr_t>(&CFileMgr::CloseFile)),FakeClose), o3e(0x5389D0,FakeClose);
                            EntryPatch s1(Body(MemberAddress(&ProcSurfaceInfo_c::Init)),FakePsiInit), s1e(0x5A2EB0,FakePsiInit);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(P); }
                        else if(nm=="GetEntityFromPool"){ build(P+PM_LIST,distinct(int(next()%7),used));
                            ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(P))); }
                        else if(nm=="ReturnEntityToPool"){ build(P+PM_LIST,distinct(int(next()%7),used)); const int k=distinct(1,used)[0];
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(P,item(k)); }
                        else if(nm=="ProcessTriangleAdded"){
                            At<int32>(P,4)=next()%8==0 ? -int32(next()%3) : int32(next()%41);
                            for(int k=0;k<128;++k)info(k)[0]=uint8(next()%6==0 ? next() : next()%5);
                            std::memset(tri.data(),0,tri.size()); tri[0x46]=uint8(next()%5);
                            for(auto& r:addRets){ const uint32 m=next()%6; r=m<3 ? int32(next()%4) : m<5 ? int32(100+next()%200) : int32(next()%1000); }
                            EntryPatch s1(Body(MemberAddress(&ProcSurfaceInfo_c::AddObjects)),FakeAddObjects), s1e(0x5A3850,FakeAddObjects);
                            ret=int(reinterpret_cast<uint32(__thiscall*)(void*,void*)>(a)(P,tri.data())&0xFF); }
                        else { // ProcessTriangleRemoved
                            const int n=next()%8==0 ? -int32(next()%3) : int32(next()%9); At<int32>(P,4)=n;
                            std::memset(tri.data(),0,tri.size()); tri[0x46]=uint8(next()%3);
                            for(int e=0;e<16;++e){ auto& o=ents[e]; std::memset(o.data(),0,o.size()); At<void*>(o.data(),0)=entVtbl.data();
                                o[0x36]=uint8((next()%3==0 ? 2+next()%2 : 4)|((next()%4)<<3)); }
                            int entNext=0;
                            for(int k=0;k<std::max(n,0)+1&&k<12;++k){ uint8* in=info(k); in[0]=uint8(next()%3);
                                const auto idx=distinct(int(next()%5),used); build(in+0x3C,idx);
                                for(int it:idx){ uint8* p=item(it); At<void*>(p,8)=ents[entNext++%16].data(); At<void*>(p,0xC)=next()%2 ? (void*)tri.data() : (void*)tri2.data();
                                    p[0x10]=uint8(next()%3==0 ? next() : next()%2); } }
                            std::memset(plist,0,0xC); tempObjs=uint16(next()%50);
                            EntryPatch w1(Body(bit_cast<uintptr_t>(&CWorld::Remove)),FakeWorldRemove), w1e(0x563280,FakeWorldRemove);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(P,tri.data());
                            out["global_list"]=Region(plist,0xC); out["temp_objs"]=tempObjs; out["ents"]=Hash(ents.data(),sizeof(ents)); }
                        out["pm"]=Hash(pm.data(),pm.size()); out["pm_head"]=Region(P,8); out["pm_list"]=Region(P+PM_LIST,0xC);
                        json lists=json::array(); for(int k=0;k<12;++k)lists.push_back(Region(info(k)+0x3C,0xC)); out["psi_lists"]=lists;
                    }
                    else { // tareas de interior
                        // el New del pool de tareas del EXE busca desde m_LastFreeSlot+1: se restaura y los slots libres se rellenan con 0xCD
                        { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                          auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* st=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                          for(int32 k=0;k<cap;++k)if(st[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                        const auto occ=Occupancy();
                        std::vector<CTask*> fresh;
                        std::memset(ped.data(),0,ped.size()); std::memset(intel.data(),0,intel.size()); std::memset(clump.data(),0,clump.size()); clump[0]=2;
                        auto* p=Ped(); p->SetType(ENTITY_TYPE_PED); At<void*>(ped.data(),0x18)=clump.data(); At<void*>(ped.data(),0x47C)=intel.data();
                        { static const int32 types[]{0,1,4,5,6,7,8,10,12,15,16,17,18,19,20,-1,7,16,6,17}; At<int32>(ped.data(),0x598)=next()%8==0 ? int32(next()) : types[next()%20]; }
                        At<uint32>(ped.data(),0x474)=next()%2 ? next() : 0; At<void*>(ped.data(),0x128)=next()%2 ? (void*)fakes[1].data() : nullptr;
                        for(int n=0;n<2;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); }
                        At<int32>(fakes[0].data(),FK_TYPE)=next()%5 ? TASK_INTERIOR_GOTO_INFO : (next()%2 ? TASK_SIMPLE_STAND_STILL : TASK_INTERIOR_SHOPKEEPER);
                        At<int32>(fakes[1].data(),FK_TYPE)=next()%4 ? TASK_INTERIOR_SHOPKEEPER : TASK_SIMPLE_STAND_STILL;
                        std::memset(keeper.data(),0,keeper.size()); std::memset(keeperIntel.data(),0,keeperIntel.size());
                        At<void*>(keeper.data(),0x47C)=keeperIntel.data(); At<void*>(keeperIntel.data(),0x14)=fakes[1].data();
                        for(auto& in:infos){ in.Type=static_cast<eInteriorInfoType>(next()%12); in.IsInUse=next()%2; in.Pos=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                            in.Dir=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f}; in.EntityIgnoredCollision=next()%2 ? reinterpret_cast<CEntity*>(collEnt.data()) : nullptr; }
                        for(auto& b:interior)b=uint8(next()); for(auto& b:interior2)b=uint8(next()); for(auto& b:igroup)b=uint8(next());
                        At<void*>(interior.data(),0xC)=igroup.data(); At<void*>(interior2.data(),0xC)=igroup.data();
                        findMask=next()%4==0 ? 0u : next()%3==0 ? 0xFFFFFFFFu : next(); animsMask=next()%4 ? 0xFFFFFFFFu : next(); abortRet=next()%2;
                        std::memset(object.data(),0xCD,object.size()); auto* o=object.data();
                        uintptr_t rawRet=0;
                        EntryPatch e2(Body(MemberAddress(&InteriorManager_c::AreAnimsLoaded)),FakeAnimsLoaded), e2e(0x5980F0,FakeAnimsLoaded);
                        EntryPatch e3(Body(MemberAddress(&InteriorGroup_c::FindInteriorInfo)),FakeFindInfo), e3e(0x594970,FakeFindInfo);
                        EntryPatch e4(Body(MemberAddress(&InteriorGroup_c::GetPed)),FakeGetPed), e4e(0x594B90,FakeGetPed);
                        EntryPatch e5(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), e5e(0x5EFFE0,FakeSay);
                        size_t sz=0x18; int32 dur=0x5A5A5A5A;
                        const auto pickInfo=[&]{ return next()%6 ? &infos[next()%4] : nullptr; };
                        if(cls=="CTaskInteriorUseInfo"){ sz=0x1C;
                            At<uintptr_t>(o,0)=fn.vt; At<CTask*>(o,4)=nullptr; At<CTask*>(o,8)=next()%8 ? Fake(0) : nullptr;
                            At<InteriorInfo_t*>(o,0xC)=pickInfo(); At<void*>(o,0x10)=next()%2 ? interior.data() : interior2.data();
                            At<int32>(o,0x14)=next()%6==0 ? -1 : int32(next()%40000); o[0x18]=uint8(next()%3==0);
                            // el EXE desreferencia la info (MakeAbortable) y la subtarea (MakeAbortable, CreateNextSubTask con info)
                            if(nm=="MakeAbortable"){ if(!At<InteriorInfo_t*>(o,0xC))At<InteriorInfo_t*>(o,0xC)=&infos[0]; At<CTask*>(o,8)=Fake(0); }
                            if(nm=="CreateNextSubTask")At<CTask*>(o,8)=Fake(0);
                            if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                                rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,InteriorInfo_t*,void*,int32,uint8)>(a)(o,pickInfo(),next()%2 ? interior.data() : nullptr,
                                    next()%6==0 ? -1 : int32(next()%40000),uint8(next()%3 ? next()%2 : next()))); }
                            else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(a)(o);
                            else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(a)(o);
                            else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(a)(o));
                            else if(nm=="MakeAbortable"){ const int prio=int(next()%3); void* ev=next()%2 ? (void*)fakes[1].data() : nullptr;
                                ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,CPed*,int,const void*)>(a)(o,p,prio,ev))); }
                            else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(a)(o,p)); }
                        else { // BeInHouse / BeInOffice
                            At<uintptr_t>(o,0)=fn.vt; At<CTask*>(o,4)=nullptr; At<CTask*>(o,8)=next()%8 ? Fake(0) : nullptr;
                            At<void*>(o,0xC)=igroup.data(); At<void*>(o,0x10)=next()%3 ? (void*)interior.data() : nullptr; At<InteriorInfo_t*>(o,0x14)=pickInfo();
                            if(nm=="Constructor"){ std::memset(o,0xCD,object.size()); rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(o,next()%5 ? igroup.data() : nullptr)); }
                            else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(a)(o);
                            else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(a)(o);
                            else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(a)(o));
                            else if(nm=="GetInfoForPedToUse"){ int32 d2{};
                                if(cls=="CTaskInteriorBeInHouse")CallThis2(a,o,p,&dur,&d2); else CallThis1(a,o,&dur,&d2);
                                out["esp"]=d2; ret=dur; }
                            else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(a)(o,p)); }
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool();
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        if(ret.is_null()&&nm!="Destructor")ret=Name(rawRet);
                        json tasks=json::array();
                        for(auto* t:fresh){ json w=Region(t,0x60,true); const int type=t->GetTaskType();
                            if(type==TASK_INTERIOR_LIE_IN_BED)w[3]=json(Name(At<uint32>(t,0x10)&0xFFFFFF00u)); // +0x10: lado derecho solo escrito con FIX_BUGS
                            tasks.push_back({{"type",type},{"words",w}}); }
                        json objWords=Region(o,sz,true);
                        if(nm=="Constructor")objWords.push_back(reinterpret_cast<CTask*>(o)->GetTaskType());
                        out["object"]=objWords; out["tasks"]=tasks; out["ped"]=Hash(ped.data(),ped.size()); out["ped_flags"]=At<uint32>(ped.data(),0x474);
                        out["ped_coll"]=Name(At<uintptr_t>(ped.data(),0x128)); out["intel"]=Hash(intel.data(),intel.size());
                        out["infos"]=json::array({infos[0].IsInUse,infos[1].IsInUse,infos[2].IsInUse,infos[3].IsInUse}); out["fakes"]=Region(fakes.data(),sizeof(fakes),true);
                        // se borra solo la tarea devuelta (una secuencia es duena de su subtarea)
                        if(rawRet&&freshIds.count(rawRet))delete reinterpret_cast<CTask*>(rawRet);
                        out["clean"]=Occupancy()==occ;
                    }
                    for(auto* h:hooks)h->State(false);
                    restore();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&(!original.contains("clean")||original["clean"]==true);
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<all.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restore();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateOk()}});
}
}
