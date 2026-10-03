#pragma once
// Banco dentro del motor: saltos, generadores de coches, muebles, criaturas y tareas de interior (lista de 7):
// CStuntJumpManager, CTheCarGenerators, FurnitureManager_c, WaterCreatureManager_c, CTaskInteriorGoToInfo,
// CTaskInteriorLieInBed y CTaskInteriorShopKeeper (49 funciones). Original (hooks del lote desactivados) frente a
// candidato con fixtures deterministas. Nunca se crean vehiculos/objetos reales: el pool de saltos vive en una arena
// propia (operator new/delete del EXE simulados sin reservas), las criaturas usan objetos falsos con vtable trazada,
// el gestor de muebles es un buffer propio y las tareas usan peaton/matriz/clump/inteligencia/animaciones falsos.
// Se simulan y trazan: rand/GetRandomNumber, el buffer de guardado (Save/LoadDataFromWorkBuffer), FindPlayerPed/
// Vehicle/Train, la apariencia del vehiculo, PickRandomPassenger, Say, CStats::IncrementStat, la camara (posicion
// fija, TakeControl, RestoreWithJumpCut, IsSphereVisible), el audio frontal, CText::Get, CMessages, CHud, el fichero
// furnitur.dat (OpenFile/LoadLine/CloseFile), CModelInfo::GetModelInfo, FurnitureGroup_c/FurnitureSubGroup_c,
// CCarGenerator::Process, CWorld::Remove, CWaterLevel::GetWaterDepth, WaterCreature_c::Init/Update,
// InteriorManager_c::AreAnimsLoaded/GetPedsInterior, InteriorGroup_c::FindInteriorInfo,
// RpAnimBlendClumpGetAssociation y CAnimManager::BlendAnimation. Todo global tocado se guarda y se restaura.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/StuntJumpManager.h"
#include "../../source/game_sa/TheCarGenerators.h"
#include "../../source/game_sa/CarGenerator.h"
#include "../../source/game_sa/Interior/FurnitureManager_c.h"
#include "../../source/game_sa/Interior/FurnitureGroup_c.h"
#include "../../source/game_sa/Interior/FurnitureSubGroup_c.h"
#include "../../source/game_sa/Interior/InteriorInfo_t.h"
#include "../../source/game_sa/Interior/Interior_c.h"
#include "../../source/game_sa/Interior/InteriorGroup_c.h"
#include "../../source/game_sa/Interior/InteriorManager_c.h"
#include "../../source/game_sa/WaterCreatureManager_c.h"
#include "../../source/game_sa/WaterCreature_c.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.h"
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.h"
#include "../../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.h"
#include "../../source/game_sa/Pools/TaskPool.h"
#include "../../source/game_sa/GenericGameStorage.h"
#include "../../source/game_sa/FileMgr.h"
#include "../../source/game_sa/FileLoader.h"
#include "../../source/game_sa/WaterLevel.h"
#include "../../source/game_sa/Animation/AnimManager.h"
#include "../ProbeRuntime.hpp"

namespace Mgr7Live {
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

// --- registro sin reservas (operator new/delete simulados) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- memoria de los fixtures (direcciones fijas: iguales en los dos lados) ---
alignas(16) inline std::array<uint8,0x8000> arena{}; inline size_t arenaUsed{};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdata{};
alignas(16) inline std::array<uint8,0x40> passenger{};
alignas(16) inline std::array<uint8,0x2078> fm{};
alignas(16) inline std::array<std::array<uint8,0x1C>,8> subGroups{};
alignas(16) inline std::array<uint8,64> furnitures{};
alignas(16) inline std::array<std::array<uint8,sizeof(CObject)>,24> objs{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> fakes{};
alignas(16) inline std::array<std::array<uint8,0x3C>,4> anims{};
alignas(16) inline std::array<uint8,sizeof(Interior_c)> interior{};
alignas(16) inline std::array<uint8,sizeof(Interior_c)> interior2{};
alignas(16) inline std::array<uint8,sizeof(InteriorGroup_c)> igroup{};
alignas(16) inline std::array<InteriorInfo_t,2> infos{};
alignas(16) inline std::array<uint8,8*0x1C> pathNodes{};
alignas(16) inline std::array<GxtChar,512> textBuf{};
inline CPed* Ped(){ return reinterpret_cast<CPed*>(ped.data()); }
inline CTask* Fake(int n){ return reinterpret_cast<CTask*>(fakes[n].data()); }

struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline std::unordered_map<uintptr_t,std::string> fnNames;
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    if(auto it=fnNames.find(v);it!=fnNames.end())return it->second;
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(v-uintptr_t(b.p));
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false){ json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }

// --- RNG ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand(){ trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }

// --- operator new/delete del EXE (la DLL los reenvia aqui) ---
inline void* __cdecl FakeNew(size_t n){ const size_t off=(arenaUsed+15)&~size_t(15); Push(1,uint32(n),uint32(off)); if(off+n>arena.size())return nullptr; arenaUsed=off+n; return arena.data()+off; }
inline void __cdecl FakeDelete(void* p){ Push(2,p ? uint32(intptr_t(p)-intptr_t(arena.data())) : 0xFFFFFFFFu); }

// --- buffer de guardado ---
// los buffers de pila del llamador (variables locales) no tienen direccion comparable entre EXE y DLL: se anotan como "stack"
inline json BufName(const void* d){ ULONG_PTR lo{}, hi{}; GetCurrentThreadStackLimits(&lo,&hi); const auto v=uintptr_t(d); if(v>=lo&&v<hi)return "stack"; return Name(v); }
inline std::vector<uint8> wbStream; inline size_t wbPos{};
inline bool __cdecl FakeSave(void* d,int32 n){ trace.push_back({"save",BufName(d),n,d ? Hash(d,size_t(n)) : std::string("null")}); return true; }
inline bool __cdecl FakeLoad(void* d,int32 n){ trace.push_back({"load",BufName(d),n});
    for(int32 k=0;k<n;++k){ const uint8 b=wbPos<wbStream.size() ? wbStream[wbPos] : uint8(wbPos*37u+11u); ++wbPos; if(d)static_cast<uint8*>(d)[k]=b; } return true; }

// --- jugador, vehiculo, saltos ---
inline bool vehRet{}; inline int appearance{}; inline bool passengerRet{}; inline uint32 textMask{}, textCalls{};
inline CPlayerPed* __cdecl FakeFindPed(int32 id){ trace.push_back({"find_ped",id}); return reinterpret_cast<CPlayerPed*>(ped.data()); }
inline CVehicle* __cdecl FakeFindVeh(int32 id,bool remote){ trace.push_back({"find_veh",id,remote}); return vehRet ? reinterpret_cast<CVehicle*>(veh.data()) : nullptr; }
inline int __fastcall FakeAppearance(void* v,void*){ trace.push_back({"appearance",Name(uintptr_t(v))}); return appearance; }
inline CPed* __fastcall FakePickPassenger(void* v,void*){ trace.push_back({"pick_passenger",Name(uintptr_t(v))}); return passengerRet ? reinterpret_cast<CPed*>(passenger.data()) : nullptr; }
inline int16 __fastcall FakeSay(void* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c){ trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),a,b,c}); return 0; }
inline void __cdecl FakeIncStat(uint32 stat,float v){ trace.push_back({"inc_stat",stat&0xFFFF,F(v)}); }
inline void __fastcall FakeFixedCam(void* c,void*,const CVector* s,const CVector* u){ trace.push_back({"cam_fixed",Name(uintptr_t(c)),F(s->x),F(s->y),F(s->z),F(u->x),F(u->y),F(u->z)}); }
inline void __fastcall FakeTakeControl(void* c,void*,void* e,int mode,int sw,int who){ trace.push_back({"cam_take",Name(uintptr_t(c)),Name(uintptr_t(e)),mode,sw,who}); }
inline void __fastcall FakeRestoreCam(void* c,void*){ trace.push_back({"cam_restore",Name(uintptr_t(c))}); }
inline void __fastcall FakeFrontend(void* ae,void*,int ev,float vol,float sp){ trace.push_back({"frontend",Name(uintptr_t(ae)),ev,F(vol),F(sp)}); }
inline const GxtChar* __fastcall FakeTextGet(void*,void*,const char* key){ const uint32 h=Str(key); trace.push_back({"text",h}); return (textMask>>(textCalls++%32))&1 ? textBuf.data()+(h%64)*4 : nullptr; }
inline void __cdecl FakeBigQ(const GxtChar* t,uint32 time,uint32 style){ trace.push_back({"big_q",Name(uintptr_t(t)),time,style&0xFFFF}); }
inline void __cdecl FakeHelp(const GxtChar* t,uint8 a,uint8 b,uint8 c){ trace.push_back({"help",Name(uintptr_t(t)),a,b,c}); }
inline void __cdecl FakeBigNum(const GxtChar* t,uint32 time,uint32 style,int32 n1,int32 n2,int32 n3,int32 n4,int32 n5,int32 n6){ trace.push_back({"big_num",Name(uintptr_t(t)),time,style&0xFFFF,n1,n2,n3,n4,n5,n6}); }

// --- generadores de coches ---
inline bool trainRet{};
inline void* __cdecl FakeFindTrain(int32 id){ trace.push_back({"find_train",id}); return trainRet ? veh.data() : nullptr; }
inline void __fastcall FakeCarGenProcess(void* g,void*){ trace.push_back({"cargen_process",int32((intptr_t(g)-intptr_t(0xC27AD0))/0x20)}); }

// --- muebles ---
inline uint32 SgIndex(const void* p){ return uint32((intptr_t(p)-intptr_t(subGroups.data()))/0x1C); }
inline Furniture_c* __fastcall FakeSgGetFurniture(void* self,void*,int16 id,uint8 w){ trace.push_back({"sg_get",SgIndex(self),id,w}); return reinterpret_cast<Furniture_c*>(furnitures.data()+SgIndex(self)%8*4); }
inline int32 __fastcall FakeSgGetRandomId(void* self,void*,uint8 w){ trace.push_back({"sg_rand",SgIndex(self),w}); return int32(SgIndex(self)*1000+w); }
inline int32 GrpIndex(const void* g){ return int32((intptr_t(g)-intptr_t(fm.data()))); }
inline bool __fastcall FakeAddSubGroup(void* g,void*,int32 id,int32 a,int32 b,int32 c,int32 d,uint8 w,uint8 t,uint8 s){ trace.push_back({"add_subgroup",GrpIndex(g),id,a,b,c,d,w,t,s}); return true; }
inline bool __fastcall FakeAddFurniture(void* g,void*,int32 sub,uint16 m,int16 id,uint8 a,uint8 b,uint8 c){ trace.push_back({"add_furniture",GrpIndex(g),sub,m,id,a,b,c}); return true; }
inline void __fastcall FakeLoadFurniture(void* self,void*){ trace.push_back({"load_furniture",Name(uintptr_t(self))}); }
inline std::array<std::array<char,96>,24> lines{}; inline size_t nlines{}, linePos{};
inline FILESTREAM __cdecl FakeOpen(const char* path,const char* mode){ trace.push_back({"open",Str(path),Str(mode)}); return reinterpret_cast<FILESTREAM>(0x5157); }
inline char* __cdecl FakeLoadLine(FILESTREAM f){ trace.push_back({"load_line",uintptr_t(f)}); return linePos<nlines ? lines[linePos++].data() : nullptr; }
inline int32 __cdecl FakeClose(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }
inline CBaseModelInfo* __cdecl FakeGetModelInfo(const char* name,int32* idx){ const uint32 h=Str(name); trace.push_back({"model_info",h,idx!=nullptr});
    if(h%5==0)return nullptr; if(idx)*idx=int32(h%20000); return reinterpret_cast<CBaseModelInfo*>(furnitures.data()); }

// --- criaturas ---
inline uint32 sphereMask{}, sphereCalls{}, wdMask{}, wdCalls{}, initMask{}, initCalls{}; inline float wDepth{}, wLevel{};
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Name(uintptr_t(e))}); }
inline void* __fastcall FakeObjDtor(void* o,void*,uint32 f){ trace.push_back({"obj_delete",Name(uintptr_t(o)),f}); return o; }
template<int N> void* __fastcall ObjTrap(void*,void*){ trace.push_back({"obj_unexpected_slot",N}); return nullptr; }
inline std::array<void*,64> objVtbl{};
template<int... N> void FillObjTraps(std::integer_sequence<int,N...>){ ((objVtbl[N]=reinterpret_cast<void*>(&ObjTrap<N>)),...); }
inline bool __fastcall FakeSphere(void* c,void*,const CVector& p,float r){ trace.push_back({"sphere",Name(uintptr_t(c)),F(p.x),F(p.y),F(p.z),F(r)}); return (sphereMask>>(sphereCalls++%32))&1; }
inline bool __cdecl FakeWaterDepth(const CVector& p,float* d,float* l,float* g){ trace.push_back({"water_depth",F(p.x),F(p.y),F(p.z),d!=nullptr,l!=nullptr,g!=nullptr});
    const bool r=(wdMask>>(wdCalls++%32))&1; if(r){ if(d)*d=wDepth; if(l)*l=wLevel; } return r; }
inline bool __fastcall FakeCreatureInit(void* c,void*,int32 type,CVector* pos,void* parent,float lvl,float dep){
    trace.push_back({"creature_init",Name(uintptr_t(c)),type,F(pos->x),F(pos->y),F(pos->z),Name(uintptr_t(parent)),F(lvl),F(dep)}); return (initMask>>(initCalls++%32))&1; }
inline void __fastcall FakeCreatureUpdate(void* c,void*,float dt){ trace.push_back({"creature_update",Name(uintptr_t(c)),F(dt)}); }

// --- tareas ---
constexpr size_t FK_TYPE=0x38;
inline CTask* __fastcall FSub(CTask*,void*){ return nullptr; }
inline bool __fastcall FSimple(CTask*,void*){ return false; }
inline int __fastcall FType(CTask* t,void*){ trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline void* __fastcall FDtor(CTask* t,void*,unsigned f){ trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
template<int N> void* __fastcall FTrap(CTask*,void*){ trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>){ ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline uintptr_t pedsInteriorRet{}; inline uint32 animsMask{}, animsCalls{}; inline uintptr_t assocRet{}; inline uint32 blendCalls{};
inline void* __fastcall FakePedsInterior(void* m,void*,const CPed* p){ trace.push_back({"peds_interior",Name(uintptr_t(m)),Name(uintptr_t(p))}); return reinterpret_cast<void*>(pedsInteriorRet); }
inline bool __fastcall FakeAnimsLoaded(void* m,void*,int32 b){ trace.push_back({"anims_loaded",Name(uintptr_t(m)),b}); return (animsMask>>(animsCalls++%32))&1; }
inline bool __fastcall FakeFindInfo(void* g,void*,int32 type,InteriorInfo_t** info,Interior_c** in){ trace.push_back({"find_info",Name(uintptr_t(g)),type});
    *info=&infos[1]; *in=reinterpret_cast<Interior_c*>(interior2.data()); return true; }
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id){ trace.push_back({"get_assoc",Name(uintptr_t(c)),id}); return reinterpret_cast<CAnimBlendAssociation*>(assocRet); }
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 grp,int32 id,float delta){ trace.push_back({"blend",Name(uintptr_t(c)),grp,id,F(delta)});
    auto* a=anims[2+blendCalls++%2].data(); At<int16>(a,0x2C)=int16(id); return reinterpret_cast<CAnimBlendAssociation*>(a); }

inline std::vector<bool> Occupancy(){ auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// llamada __thiscall de un argumento con medida de la pila (un candidato cdecl dejaria esp desplazado)
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

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,49> fns{{
    {"CStuntJumpManager","Init",0x49CA50,0,-1,64},{"CStuntJumpManager","Shutdown",0x49CBC0,0,-1,128},{"CStuntJumpManager","ShutdownForRestart",0x49CB10,0,-1,128},
    {"CStuntJumpManager","Save",0x5D5570,0,-1,128},{"CStuntJumpManager","Load",0x5D5920,0,-1,256},{"CStuntJumpManager","AddOne",0x49CB40,0,-1,512},
    {"CStuntJumpManager","Update",0x49C490,0,-1,1024},
    {"CTheCarGenerators","CreateCarGenerator",0x6F31A0,0,-1,512},{"CTheCarGenerators","Get",0x479D60,0,-1,128},{"CTheCarGenerators","Init",0x6F3270,0,-1,64},
    {"CTheCarGenerators","Load",0x5D39B0,0,-1,256},{"CTheCarGenerators","Process",0x6F3F40,0,-1,512},{"CTheCarGenerators","RemoveCarGenerators",0x6F3240,0,-1,256},
    {"CTheCarGenerators","Save",0x5D38C0,0,-1,256},
    {"FurnitureManager_c","Exit",0x5911A0,0,-1,256},{"FurnitureManager_c","GetFurniture",0x5911E0,0,-1,512},{"FurnitureManager_c","GetGroupId",0x5BFB40,0,-1,256},
    {"FurnitureManager_c","GetRandomId",0x591220,0,-1,512},{"FurnitureManager_c","GetSubGroupId",0x5BFBF0,0,-1,512},{"FurnitureManager_c","Init",0x5C04A0,0,-1,64},
    {"FurnitureManager_c","LoadFurniture",0x5C0280,0,-1,512},
    {"WaterCreatureManager_c","CanAddWaterCreatureAtPos",0x6E4510,0,-1,1024},{"WaterCreatureManager_c","Exit",0x6E3FD0,0,-1,256},
    {"WaterCreatureManager_c","GetRandomWaterCreatureId",0x6E4040,0,-1,512},{"WaterCreatureManager_c","Init",0x6E3F90,0,-1,64},
    {"WaterCreatureManager_c","TryToExitGroup",0x6E45B0,0,-1,512},{"WaterCreatureManager_c","TryToFreeUpWaterCreatures",0x6E40E0,0,-1,512},
    {"WaterCreatureManager_c","Update",0x6E4F10,0,-1,1024},
    {"CTaskInteriorGoToInfo","Constructor",0x6757D0,0x8702BC,-1,128},{"CTaskInteriorGoToInfo","Destructor",0x675810,0x8702BC,-1,64},
    {"CTaskInteriorGoToInfo","Clone",0,0x8702BC,1,128},{"CTaskInteriorGoToInfo","GetTaskType",0,0x8702BC,4,64},
    {"CTaskInteriorGoToInfo","CreateNextSubTask",0,0x8702BC,8,512},{"CTaskInteriorGoToInfo","CreateFirstSubTask",0,0x8702BC,9,512},
    {"CTaskInteriorGoToInfo","ControlSubTask",0,0x8702BC,10,128},
    {"CTaskInteriorLieInBed","Constructor",0x675E20,0x870338,-1,128},{"CTaskInteriorLieInBed","Destructor",0x675E90,0x870338,-1,128},
    {"CTaskInteriorLieInBed","FinishAnimCB",0x675FC0,0x870338,-1,512},{"CTaskInteriorLieInBed","Clone",0,0x870338,1,128},
    {"CTaskInteriorLieInBed","GetTaskType",0,0x870338,4,64},{"CTaskInteriorLieInBed","MakeAbortable",0,0x870338,6,256},
    {"CTaskInteriorLieInBed","ProcessPed",0,0x870338,7,1024},
    {"CTaskInteriorShopKeeper","Constructor",0x675660,0x870290,-1,128},{"CTaskInteriorShopKeeper","Destructor",0x6756A0,0x870290,-1,128},
    {"CTaskInteriorShopKeeper","Clone",0,0x870290,1,128},{"CTaskInteriorShopKeeper","GetTaskType",0,0x870290,4,64},
    {"CTaskInteriorShopKeeper","CreateNextSubTask",0,0x870290,8,128},{"CTaskInteriorShopKeeper","CreateFirstSubTask",0,0x870290,9,256},
    {"CTaskInteriorShopKeeper","ControlSubTask",0,0x870290,10,128},
}};

constexpr uintptr_t SJ_BEGIN=0xA9A888; constexpr size_t SJ_N=0x1C;           // mp_poolStuntJumps .. m_iNumCompleted
constexpr uintptr_t CG_BEGIN=0xC279D0; constexpr size_t CG_N=0xC2B950-0xC279D0; // contadores, matriculas y CarGeneratorArray
constexpr uintptr_t WC_BEGIN=0xC1DF30; constexpr size_t WC_N=0x1A1C;          // g_waterCreatureMan
constexpr size_t SJ_STORE=0x20, SJ_STATES=0x4420, SJ_END=0x4520;              // pool falso de saltos dentro de la arena

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"mgr7-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"mgr7-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("mgr7");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"mgr7-only.txt"); if(f)std::getline(f,only); }
    const auto& all=fns;
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(all.size());
    for(size_t f=0;f<all.size();++f)if(!(hooks[f]=FindHook(all[f].cls,all[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(all[f].cls)+"::"+all[f].name}});return;}
    std::vector<uint8> enabled(all.size()),locked(all.size());
    for(size_t f=0;f<all.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    FillObjTraps(std::make_integer_sequence<int,64>{}); objVtbl[0]=reinterpret_cast<void*>(&FakeObjDtor);
    bufs={{arena.data(),arena.size(),"arena"},{ped.data(),ped.size(),"ped"},{mat.data(),mat.size(),"mat"},{veh.data(),veh.size(),"veh"},{clump.data(),clump.size(),"clump"},
          {intel.data(),intel.size(),"intel"},{pdata.data(),pdata.size(),"pdata"},{passenger.data(),passenger.size(),"passenger"},{fm.data(),fm.size(),"fm"},
          {subGroups.data(),sizeof(subGroups),"sg"},{furnitures.data(),furnitures.size(),"furn"},{objs.data(),sizeof(objs),"obj"},{object.data(),object.size(),"object"},
          {fakes.data(),sizeof(fakes),"fake"},{anims.data(),sizeof(anims),"anim"},{interior.data(),interior.size(),"interior"},{interior2.data(),interior2.size(),"interior2"},
          {igroup.data(),igroup.size(),"igroup"},{infos.data(),sizeof(infos),"info"},{pathNodes.data(),pathNodes.size(),"node"},{textBuf.data(),sizeof(textBuf),"text"},
          {reinterpret_cast<void*>(WC_BEGIN),WC_N,"wcman"},{reinterpret_cast<void*>(CG_BEGIN),CG_N,"cargen"},{&TheCamera,sizeof(CCamera),"camera"},
          {&AudioEngine,sizeof(CAudioEngine),"audio"},{&g_interiorMan,sizeof(InteriorManager_c),"intman"}};
    for(uintptr_t a:{uintptr_t(0x675FC0),uintptr_t(&CTaskInteriorLieInBed::FinishAnimCB),Body(uintptr_t(&CTaskInteriorLieInBed::FinishAnimCB))})fnNames[a]="finish_cb";
    for(uintptr_t a:{uintptr_t(0x4D1390),uintptr_t(&CDefaultAnimCallback::DefaultAnimCB),Body(uintptr_t(&CDefaultAnimCallback::DefaultAnimCB))})fnNames[a]="default_cb";

    // estado global guardado
    auto* sj=reinterpret_cast<uint8*>(SJ_BEGIN); const std::vector<uint8> savedSj(sj,sj+SJ_N);
    auto* cg=reinterpret_cast<uint8*>(CG_BEGIN); const std::vector<uint8> savedCg(cg,cg+CG_N);
    auto* wc=reinterpret_cast<uint8*>(WC_BEGIN); const std::vector<uint8> savedWc(wc,wc+WC_N);
    auto* pinfo=reinterpret_cast<uint8*>(&CWorld::Players[CWorld::PlayerInFocus]); const std::vector<uint8> savedPinfo(pinfo,pinfo+sizeof(CPlayerInfo));
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); const float savedStep=timeStep;
    auto& timeScale=*reinterpret_cast<float*>(0xB7CB64); const float savedScale=timeScale;
    auto& replay=*reinterpret_cast<uint8*>(0xA43088); const uint8 savedReplay=replay;
    auto& cutscene=*reinterpret_cast<uint8*>(0xB5F852); const uint8 savedCutscene=cutscene;
    auto& fmCounters=*reinterpret_cast<uint64*>(0xBAB378); const uint64 savedFmCounters=fmCounters;
    auto& tempObjs=*reinterpret_cast<uint16*>(0xBB4A70); const uint16 savedTempObjs=tempObjs;
    const auto occupancyStart=Occupancy();
    auto stateOk=[&]{ return std::memcmp(sj,savedSj.data(),SJ_N)==0&&std::memcmp(cg,savedCg.data(),CG_N)==0&&std::memcmp(wc,savedWc.data(),WC_N)==0
        &&std::memcmp(pinfo,savedPinfo.data(),savedPinfo.size())==0&&timeMs==savedTime&&F(timeStep)==F(savedStep)&&F(timeScale)==F(savedScale)&&replay==savedReplay
        &&cutscene==savedCutscene&&fmCounters==savedFmCounters&&tempObjs==savedTempObjs&&Occupancy()==occupancyStart; };
    auto restore=[&]{ std::memcpy(sj,savedSj.data(),SJ_N); std::memcpy(cg,savedCg.data(),CG_N); std::memcpy(wc,savedWc.data(),WC_N); std::memcpy(pinfo,savedPinfo.data(),savedPinfo.size());
        timeMs=savedTime; timeStep=savedStep; timeScale=savedScale; replay=savedReplay; cutscene=savedCutscene; fmCounters=savedFmCounters; tempObjs=savedTempObjs; };
    const CVector cam=TheCamera.GetPosition();

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
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7A77u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; nrec=0;
                    textCalls=0; sphereCalls=0; wdCalls=0; initCalls=0; animsCalls=0; blendCalls=0; wbPos=0; wbStream.clear(); freshIds.clear();
                    // el New del pool de tareas del EXE busca desde m_LastFreeSlot+1: se restaura y los slots libres se rellenan con 0xCD
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* st=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(st[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    bool switches;
                    if(fn.slot>=0){ const uintptr_t t=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=(t>=0x400000&&t<0x1000000)!=candidate; }
                    else switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    std::vector<CTask*> fresh;

                    if(cls=="CStuntJumpManager") {
                        // pool falso dentro de la arena
                        arena.fill(0xCD); arenaUsed=SJ_END;
                        auto* hdr=arena.data(); auto* store=arena.data()+SJ_STORE; auto* states=arena.data()+SJ_STATES;
                        const CVector vpos{uni(-500.f,500.f),uni(-500.f,500.f),uni(0.f,60.f)};
                        auto box=[&](uint8* b,bool inside){ float* v=reinterpret_cast<float*>(b);
                            for(int k=0;k<3;++k){ const float c=(&vpos.x)[k]; const float lo=inside ? c-uni(0.f,20.f) : c+uni(-30.f,30.f); v[k]=lo; v[3+k]=lo+uni(0.f,40.f); } };
                        const int32 size=next()%6==0 ? int32(next()%257) : 256;
                        for(int k=0;k<256;++k){ uint8* j=store+k*0x44; box(j,next()%4==0); box(j+0x18,next()%3==0);
                            for(int c=0;c<3;++c)At<float>(j,0x30+4*c)=uni(-500.f,500.f); At<int32>(j,0x3C)=int32(next()%20000); j[0x40]=uint8(next()%3 ? next()%2 : next()); j[0x41]=uint8(next()%3 ? next()%2 : next());
                            j[0x42]=uint8(next()); j[0x43]=uint8(next());
                            states[k]=uint8(next()%3==0 ? 0x80|(next()&0x7F) : next()&0x7F); }
                        if(next()%5==0)for(int k=0;k<256;++k)states[k]|=0x80; // pool lleno de huecos libres
                        if(next()%6==0)for(int k=0;k<256;++k)states[k]&=0x7F; // pool lleno
                        At<uint8*>(hdr,0)=store; At<uint8*>(hdr,4)=states; At<int32>(hdr,8)=size; At<int32>(hdr,0xC)=next()%5==0 ? -1 : int32(next()%257)-1;
                        hdr[0x10]=uint8(next()%4 ? 1 : 0); hdr[0x11]=uint8(next());
                        At<uint8*>(sj,0)=next()%10==0 ? nullptr : hdr;
                        At<uint8*>(sj,4)=next()%5==0 ? nullptr : store+(next()%256)*0x44;
                        sj[8]=uint8(next()%4!=0); sj[9]=uint8(next()%2); sj[10]=uint8(next()); sj[11]=uint8(next());
                        { const uint32 m=next()%6; At<uint32>(sj,0xC)=m==0 ? next() : m==1 ? 300+next()%3-1 : m==2 ? 1000+next()%3-1 : m==3 ? 100+next()%3-1 : next()%2000; }
                        At<uint32>(sj,0x10)=next()%4;
                        At<int32>(sj,0x14)=int32(next()%300); At<uint32>(sj,0x18)=next()%3==0 ? uint32(At<int32>(sj,0x14))-1 : next()%300;
                        if(nm=="Init"){ arena.fill(0xCD); arenaUsed=0; At<uint8*>(sj,0)=reinterpret_cast<uint8*>(0x1234);
                            EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete);
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Shutdown"){ EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete);
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="ShutdownForRestart"){ At<uint8*>(sj,0)=hdr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Save"){ At<uint8*>(sj,0)=hdr; At<int32>(hdr,8)=256;
                            EntryPatch s1(Body(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer))),FakeSave), s1e(0x5D1270,FakeSave);
                            ret=int(uint8(reinterpret_cast<uint32(__cdecl*)()>(a)())); }
                        else if(nm=="Load"){ At<uint8*>(sj,0)=hdr; At<int32>(hdr,8)=256;
                            const int32 cnt=next()%8==0 ? int32(next()%300) : int32(next()%40); wbStream.resize(4); std::memcpy(wbStream.data(),&cnt,4);
                            for(int32 k=0;k<cnt*0x44;++k)wbStream.push_back(uint8(next()));
                            for(int32 k=0;k<cnt;++k)for(int c=0;c<15;++c){ const float fv=uni(-500.f,500.f); std::memcpy(wbStream.data()+4+k*0x44+c*4,&fv,4); }
                            EntryPatch l1(Body(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer))),FakeLoad), l1e(0x5D1300,FakeLoad);
                            ret=int(uint8(reinterpret_cast<uint32(__cdecl*)()>(a)())); }
                        else if(nm=="AddOne"){ alignas(4) std::array<float,6> s{},e{}; for(auto& v:s)v=uni(-500.f,500.f); for(auto& v:e)v=uni(-500.f,500.f);
                            const CVector cp{uni(-500.f,500.f),uni(-500.f,500.f),uni(0.f,100.f)};
                            ret=reinterpret_cast<int32(__cdecl*)(const void*,const void*,const CVector*,int32)>(a)(s.data(),e.data(),&cp,int32(next())); }
                        else { // Update
                            std::memset(ped.data(),0,ped.size()); std::memset(veh.data(),0,veh.size()); std::memset(passenger.data(),0,passenger.size());
                            Ped()->SetType(ENTITY_TYPE_PED); At<CVehicle*>(ped.data(),0x58C)=next()%8 ? reinterpret_cast<CVehicle*>(veh.data()) : nullptr;
                            if(next()%5)At<uint32>(ped.data(),0x46C)|=0x100;
                            auto* v=reinterpret_cast<CVehicle*>(veh.data()); v->SetType(ENTITY_TYPE_VEHICLE); v->SetPosn(vpos);
                            veh[0xB9]=uint8(next()%3 ? 0 : 1+next()%3); At<void*>(veh.data(),0x460)=next()%6 ? (void*)ped.data() : (void*)passenger.data();
                            veh[0x36]=uint8(((next()%5==0 ? next()%8 : 1)<<3)|(next()&7)); if(next()%10==0)veh[0x42B]|=0x40; if(next()%10==0)At<uint32>(veh.data(),0x40)|=0x100;
                            { const float s=next()%4 ? uni(0.3f,1.5f) : uni(0.f,0.45f); At<float>(veh.data(),0x44)=s*uni(-1.f,1.f); At<float>(veh.data(),0x48)=s*uni(-1.f,1.f); At<float>(veh.data(),0x4C)=uni(-0.2f,0.2f);
                              if(next()%10==0){ At<float>(veh.data(),0x44)=0.4f; At<float>(veh.data(),0x48)=0.f; At<float>(veh.data(),0x4C)=0.f; } }
                            pinfo[0xDC]=uint8(next()%5 ? 0 : 1+next()%3); At<int32>(pinfo,0xB8)=int32(next()%100000);
                            appearance=int(next()%7==0 ? next()%6 : 1+next()%2); vehRet=next()%5!=0; passengerRet=next()%3!=0; textMask=next();
                            replay=uint8(next()%12==0 ? 1 : 0); timeStep=next()%8==0 ? 0.f : uni(0.2f,3.f);
                            // el EXE desreferencia sin comprobar el vehiculo (estado 1) y mp_Active (estado 2 con premio): fixtures fuera de esos caminos
                            At<uint8*>(sj,0)=next()%12==0 ? nullptr : hdr;
                            if(At<uint32>(sj,0x10)==1)At<CVehicle*>(ped.data(),0x58C)=reinterpret_cast<CVehicle*>(veh.data());
                            if(At<uint32>(sj,0x10)==2&&!At<uint8*>(sj,4))At<uint8*>(sj,4)=store+(next()%256)*0x44;
                            EntryPatch p1(Body(bit_cast<uintptr_t>(&FindPlayerPed)),FakeFindPed), p1e(0x56E210,FakeFindPed);
                            EntryPatch p2(Body(bit_cast<uintptr_t>(&FindPlayerVehicle)),FakeFindVeh), p2e(0x56E0D0,FakeFindVeh);
                            EntryPatch p3(Body(MemberAddress(&CVehicle::GetVehicleAppearance)),FakeAppearance), p3e(0x6D1080,FakeAppearance);
                            EntryPatch p4(Body(MemberAddress(&CVehicle::PickRandomPassenger)),FakePickPassenger), p4e(0x6D2A10,FakePickPassenger);
                            EntryPatch p5(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), p5e(0x5EFFE0,FakeSay);
                            EntryPatch p6(Body(bit_cast<uintptr_t>(&CStats::IncrementStat)),FakeIncStat), p6e(0x55C180,FakeIncStat);
                            EntryPatch p7(Body(MemberAddress(&CCamera::SetCamPositionForFixedMode)),FakeFixedCam), p7e(0x50BEC0,FakeFixedCam);
                            EntryPatch p8(Body(MemberAddress(&CCamera::TakeControl)),FakeTakeControl), p8e(0x50C7C0,FakeTakeControl);
                            EntryPatch p9(Body(MemberAddress(&CCamera::RestoreWithJumpCut)),FakeRestoreCam), p9e(0x50BAB0,FakeRestoreCam);
                            EntryPatch pa(Body(MemberAddress(&CAudioEngine::ReportFrontendAudioEvent)),FakeFrontend), pae(0x506EA0,FakeFrontend);
                            EntryPatch pb(Body(MemberAddress(&CText::Get)),FakeTextGet), pbe(0x6A0050,FakeTextGet);
                            EntryPatch pc(Body(bit_cast<uintptr_t>(&CMessages::AddBigMessageQ)),FakeBigQ), pce(0x69F370,FakeBigQ);
                            EntryPatch pd(Body(bit_cast<uintptr_t>(&CHud::SetHelpMessage)),FakeHelp), pde(0x588BE0,FakeHelp);
                            EntryPatch pe(Body(bit_cast<uintptr_t>(&CMessages::AddBigMessageWithNumber)),FakeBigNum), pee(0x69E5F0,FakeBigNum);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["ped"]=Hash(ped.data(),ped.size()); out["veh"]=Hash(veh.data(),veh.size());
                            out["player"]=json::array({pinfo[0xDC],At<int32>(pinfo,0xB8)}); out["scale"]=F(timeScale); }
                        out["sj"]=Region(sj,SJ_N); out["hdr"]=Region(arena.data(),0x14); out["hdr_tail"]=arena[0x10]|(arena[0x11]<<8);
                        out["arena"]=Hash(arena.data(),arena.size()); out["states"]=ReversingProbe::Bytes(states,256); out["recs"]=Recs();
                        if(nm=="Init")out["new_states"]=ReversingProbe::Bytes(arena.data()+0x4420,0x110);
                    }
                    else if(cls=="CTheCarGenerators") {
                        for(size_t k=0;k<CG_N;++k)cg[k]=uint8(next());
                        for(int k=0;k<500;++k){ uint8* g=cg+(0xC27AD0-CG_BEGIN)+k*0x20; g[0x1D]=uint8(next()%3 ? next()%2 : next()); g[0x1C]=uint8(next()%3 ? 0 : next()%4); }
                        if(next()%6==0)for(int k=0;k<500;++k)cg[(0xC27AD0-CG_BEGIN)+k*0x20+0x1D]=1;
                        At<uint32>(cg,4)=next()%600;
                        if(nm=="CreateCarGenerator"){ const int32 model=int32(next()%5==0 ? int32(next()%800)-1 : next()%6==0 ? -1 : 400+int32(next()%231));
                            ret=reinterpret_cast<int32(__cdecl*)(CVector,float,int32,int16,int16,uint8,uint8,uint8,uint16,uint16,uint8,bool)>(a)(
                                CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)},uni(0.f,360.f),model,int16(next()),int16(next()),uint8(next()),uint8(next()),uint8(next()),
                                uint16(next()),uint16(next()),uint8(next()%4),next()%2!=0); }
                        else if(nm=="Get"){ const uint16 idx=uint16(next()%5==0 ? next() : next()%500); ret=int32(intptr_t(reinterpret_cast<void*(__cdecl*)(uint16)>(a)(idx))-intptr_t(0xC27AD0)); }
                        else if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="RemoveCarGenerators")reinterpret_cast<void(__cdecl*)(uint8)>(a)(uint8(next()%4==0 ? next() : next()%4));
                        else if(nm=="Process"){ cg[1]=uint8(next()%3 ? next()%4 : next()); cg[0]=uint8(next()%3 ? next()%3 : next()); trainRet=next()%8==0; cutscene=uint8(next()%8==0); replay=uint8(next()%10==0);
                            EntryPatch t1(Body(bit_cast<uintptr_t>(&FindPlayerTrain)),FakeFindTrain), t1e(0x56E160,FakeFindTrain);
                            EntryPatch g1(Body(MemberAddress(&CCarGenerator::Process)),FakeCarGenProcess), g1e(0x6F3E90,FakeCarGenProcess);
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Save"){ EntryPatch s1(Body(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer))),FakeSave), s1e(0x5D1270,FakeSave);
                            ret=int(uint8(reinterpret_cast<uint32(__cdecl*)()>(a)())); }
                        else { // Load: flujo con la secuencia esperada
                            auto put=[&](const void* p,size_t n){ const auto* b=static_cast<const uint8*>(p); wbStream.insert(wbStream.end(),b,b+n); };
                            const int32 num=int32(next()%8==0 ? next()%600 : next()%40); put(&num,4); const uint8 pc=uint8(next()), ge=uint8(next()); put(&pc,1); put(&ge,1);
                            for(int32 k=0;k<num;++k){ const uint16 idx=uint16(next()%6==0 ? 490+next()%30 : next()%500); put(&idx,2); if(idx<500)for(int c=0;c<32;++c)wbStream.push_back(uint8(next())); }
                            for(int c=0;c<4+15*16;++c)wbStream.push_back(uint8(next()));
                            EntryPatch l1(Body(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer))),FakeLoad), l1e(0x5D1300,FakeLoad);
                            ret=int(uint8(reinterpret_cast<uint32(__cdecl*)()>(a)())); }
                        out["cg_head"]=ReversingProbe::Bytes(cg,0x100); out["cg"]=Hash(cg,CG_N);
                    }
                    else if(cls=="FurnitureManager_c") {
                        for(auto& b:fm)b=uint8(next()); for(auto& g:subGroups)for(auto& b:g)b=uint8(next());
                        auto P=[&](void* base,size_t off)->uint8*&{ return *reinterpret_cast<uint8**>(static_cast<uint8*>(base)+off); };
                        // grupos con listas de subgrupos falsos (sin compartir nodos entre grupos)
                        std::array<int,8> order{0,1,2,3,4,5,6,7}; for(int k=7;k>0;--k)std::swap(order[k],order[next()%(k+1)]);
                        int used=0;
                        for(int g=0;g<9;++g){ uint8* grp=fm.data()+g*0xC; const int n=used<8 ? int(next()%std::min(4,9-used)) : 0; uint8* prev=nullptr; uint8* head=nullptr;
                            for(int k=0;k<n;++k){ uint8* s=subGroups[order[used++]].data(); P(s,0)=prev; P(s,4)=nullptr; if(prev)P(prev,4)=s; else head=s; prev=s; At<int32>(s,8)=int32(next()%5);
                                P(s,0xC)=next()%2 ? furnitures.data() : nullptr; P(s,0x10)=P(s,0xC); At<uint32>(s,0x14)=P(s,0xC) ? 1 : 0; }
                            P(grp,0)=head; P(grp,4)=prev; At<uint32>(grp,8)=uint32(n); }
                        P(fm.data(),0x206C)=next()%2 ? fm.data()+0x6C : nullptr; P(fm.data(),0x2070)=P(fm.data(),0x206C); At<uint32>(fm.data(),0x2074)=P(fm.data(),0x206C) ? 1 : 0;
                        if(nm=="GetGroupId"||nm=="GetSubGroupId"){
                            static const char* names[]{"IT_SHOP","IT_OFFICE","IT_LOUNGE","IT_BEDROOM","IT_KITCHEN","IT_MISC","IT_SHOPX","it_shop","IT_","",
                                "SHOP_UNIT1_L","SHOP_UNIT1_R","SHOP_UNIT1_M","SHOP_UNIT2_L","SHOP_UNIT2_R","SHOP_UNIT2_M","SHOP_UNIT3_L","SHOP_UNIT3_R","SHOP_UNIT3_M","SHOP_UNIT4_L",
                                "SHOP_UNIT4_R","SHOP_UNIT4_M","SHOP_CHECKOUTS","SHOP_BASKETS","SHOP_LIGHTS","OFFICE_DESKS","OFFICE_CHAIRS","OFFICE_WATERCOOLERS","OFFICE_FILINGCABS",
                                "LOUNGE_SOFAS","LOUNGE_CHAIRS","LOUNGE_TVSTANDS","LOUNGE_TVS","LOUNGE_TABLES","LOUNGE_LAMPS","LOUNGE_HIFI_TABLES","LOUNGE_VIDEOS","LOUNGE_HIFIS",
                                "LOUNGE_CONSOLES","BEDROOM_BEDS","BEDROOM_TABLES","BEDROOM_DRAWERS","BEDROOM_WARDROBES","KITCHEN_UNIT_L","KITCHEN_UNIT_M","KITCHEN_UNIT_R",
                                "KITCHEN_UNIT_SINKS","KITCHEN_UNIT_FRIDGES","KITCHEN_UNIT_OVENS","KITCHEN_UNIT_WASHERS","KITCHEN_UNIT_CORNERS","KITCHEN_MICROWAVES","KITCHEN_TOASTERS",
                                "KITCHEN_TVS","MISC_PLANTS","MISC_LIGHTS","MISC_RUGS","MISC_PLATES","MISC_FOOD","MISC_MAGAZINES","MISC_CLOTHES","MISC_CLOTHESX","KITCHEN_TV","SHOP_UNIT"};
                            constexpr size_t NN=std::size(names); char buf[32]{};
                            if(next()%6==0){ const int len=int(next()%12); for(int k=0;k<len;++k)buf[k]=char('A'+next()%27); } else std::strcpy(buf,names[next()%NN]);
                            int32 d{}; ret=CallThis1(a,fm.data(),buf,&d); out["esp"]=d; out["name"]=std::string(buf); }
                        else if(nm=="GetFurniture"||nm=="GetRandomId"){ const int32 g=int32(next()%9), id=int32(next()%6);
                            EntryPatch s1(Body(MemberAddress(&FurnitureSubGroup_c::GetFurniture)),FakeSgGetFurniture), s1e(0x590EE0,FakeSgGetFurniture);
                            EntryPatch s2(Body(MemberAddress(&FurnitureSubGroup_c::GetRandomId)),FakeSgGetRandomId), s2e(0x590FD0,FakeSgGetRandomId);
                            if(nm=="GetFurniture")ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,int16,uint8)>(a)(fm.data(),g,id,int16(next()),uint8(next()))));
                            else ret=reinterpret_cast<int32(__thiscall*)(void*,int32,int32,uint8)>(a)(fm.data(),g,id,uint8(next())); }
                        else if(nm=="Exit")reinterpret_cast<void(__thiscall*)(void*)>(a)(fm.data());
                        else if(nm=="Init"){ P(fm.data(),0x206C)=nullptr; P(fm.data(),0x2070)=nullptr; At<uint32>(fm.data(),0x2074)=0; fmCounters=(uint64(next())<<32)|next();
                            EntryPatch l1(Body(MemberAddress(&FurnitureManager_c::LoadFurniture)),FakeLoadFurniture), l1e(0x5C0280,FakeLoadFurniture);
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*)>(a)(fm.data()))); out["counters"]=std::to_string(fmCounters); fmCounters=savedFmCounters; }
                        else { // LoadFurniture: lineas completas (sscanf nunca deja campos sin escribir); ITEM solo tras un SUBGROUP
                            static const char* groups[]{"IT_SHOP","IT_OFFICE","IT_LOUNGE","IT_BEDROOM","IT_KITCHEN","IT_MISC","IT_NOPE"};
                            static const char* subs[]{"SHOP_UNIT1_L","SHOP_LIGHTS","OFFICE_DESKS","LOUNGE_TVS","BEDROOM_BEDS","KITCHEN_TVS","MISC_FOOD","MISC_CLOTHES","BAD_SUB"};
                            static const char* models[]{"chair1","sofa","tv_unit","bed_a","kitchen_l","plant2","rug","lamp3","toaster","fridge"};
                            nlines=1+next()%23; linePos=0; bool haveSub=false;
                            for(size_t k=0;k<nlines;++k){ char* l=lines[k].data(); const uint32 kind=k==0 ? 0 : next()%8;
                                if(kind==0||kind==5)snprintf(l,96,"GROUP: %s",groups[next()%7]);
                                else if(kind==1||kind==2||((kind==3||kind==4)&&!haveSub)){ snprintf(l,96,"SUBGROUP: %s %d %d %d %d %d %d %d",subs[next()%9],int(next()%5),int(next()%5),int(next()%9),int(next()%9),int(next()%2),int(next()%2),int(next()%2)); haveSub=true; }
                                else if(kind==3||kind==4)snprintf(l,96,"ITEM: %s %d %d %d %d",models[next()%10],int(next()%40)-2,int(next()%300),int(next()%300),int(next()%400));
                                else if(kind==6)snprintf(l,96,"%s",next()%2 ? "# comment" : "");
                                else snprintf(l,96,"%s %d",next()%2 ? "UNKNOWN:" : "group:",int(next()%9)); }
                            EntryPatch o1(Body(bit_cast<uintptr_t>(&CFileMgr::OpenFile)),FakeOpen), o1e(0x538900,FakeOpen);
                            EntryPatch o2(Body(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine))),FakeLoadLine), o2e(0x536F80,FakeLoadLine);
                            EntryPatch o3(Body(bit_cast<uintptr_t>(&CFileMgr::CloseFile)),FakeClose), o3e(0x5389D0,FakeClose);
                            EntryPatch m1(Body(bit_cast<uintptr_t>(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo))),FakeGetModelInfo), m1e(0x4C5940,FakeGetModelInfo);
                            EntryPatch g1(Body(MemberAddress(&FurnitureGroup_c::AddSubGroup)),FakeAddSubGroup), g1e(0x5910E0,FakeAddSubGroup);
                            EntryPatch g2(Body(MemberAddress(&FurnitureGroup_c::AddFurniture)),FakeAddFurniture), g2e(0x5C0230,FakeAddFurniture);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(fm.data()); }
                        out["fm"]=Hash(fm.data(),fm.size()); out["fm_groups"]=Region(fm.data(),0x6C); out["fm_list"]=Region(fm.data()+0x206C,0xC);
                        out["sg"]=Hash(subGroups.data(),sizeof(subGroups));
                    }
                    else if(cls=="WaterCreatureManager_c") {
                        auto* m=wc; for(size_t k=0;k<WC_N;++k)m[k]=uint8(next());
                        auto item=[&](int k){ return m+k*0x34; };
                        // objetos falsos
                        for(int k=0;k<24;++k){ auto& o=objs[k]; std::memset(o.data(),0,o.size()); At<void*>(o.data(),0)=objVtbl.data(); o[0x36]=uint8(2<<3);
                            At<float>(o.data(),4)=cam.x+uni(-80.f,80.f); At<float>(o.data(),8)=cam.y+uni(-80.f,80.f); At<float>(o.data(),0xC)=cam.z+uni(-20.f,20.f);
                            if(next()%2)At<uint32>(o.data(),0x1C)|=0x20000; }
                        // listas: k en [0,nl) activas y [nl,nl+np) libres
                        const int nl=int(next()%6==0 ? 0 : next()%20), np=int(next()%5==0 ? 128-nl : next()%12);
                        auto build=[&](size_t off,int from,int n){ uint8* prev=nullptr; uint8* head=nullptr;
                            for(int k=from;k<from+n;++k){ uint8* it=item(k); At<uint8*>(it,0)=prev; At<uint8*>(it,4)=nullptr; if(prev)At<uint8*>(prev,4)=it; else head=it; prev=it; }
                            At<uint8*>(m,off)=head; At<uint8*>(m,off+4)=prev; At<uint32>(m,off+8)=uint32(n); };
                        build(0x1A0C,0,nl); build(0x1A00,nl,np);
                        // los elementos libres que Update pasa a la lista (Init simulado) se borran por m_bShouldBeDeleted: objeto falso valido y sin lider
                        for(int k=nl;k<128;++k){ uint8* it=item(k); At<void*>(it,8)=objs[k%24].data(); it[0xD]=uint8(next()%4==0); At<uint8*>(it,0x1C)=nullptr; }
                        for(int k=0;k<nl;++k){ uint8* it=item(k); At<void*>(it,8)=objs[k%24].data(); it[0xC]=uint8(next()%7); it[0xD]=uint8(next()%4==0);
                            At<uint8*>(it,0x1C)=(k>0&&next()%2) ? item(int(next()%k)) : nullptr; }
                        if(nm=="Init"){ build(0x1A00,0,0); reinterpret_cast<uint8(__thiscall*)(void*)>(a)(m); }
                        else if(nm=="Exit"||nm=="TryToFreeUpWaterCreatures"||nm=="Update"){
                            EntryPatch w1(Body(bit_cast<uintptr_t>(&CWorld::Remove)),FakeWorldRemove), w1e(0x563280,FakeWorldRemove);
                            if(nm=="Exit")reinterpret_cast<void(__thiscall*)(void*)>(a)(m);
                            else if(nm=="TryToFreeUpWaterCreatures"){ static const int32 ns[]{-1,0,1,2,3,5,100}; ret=reinterpret_cast<int32(__thiscall*)(void*,int32)>(a)(m,ns[next()%7]); }
                            else { std::memset(ped.data(),0,ped.size()); std::memset(pdata.data(),0,pdata.size()); At<void*>(ped.data(),0x480)=pdata.data();
                                pdata[0x8E]=uint8(next()%4 ? 51+next()%50 : next()%52);
                                timeMs=savedTime; At<int32>(m,0x1A18)=int32(savedTime)-int32(next()%4 ? 1001+next()%3000 : next()%1002); if(next()%10==0)At<int32>(m,0x1A18)=int32(next());
                                sphereMask=next()&next(); wdMask=next()|next(); initMask=next()|next()|next(); wDepth=next()%4 ? uni(4.6f,30.f) : uni(0.f,5.f); if(next()%10==0)wDepth=4.5f; wLevel=uni(-5.f,10.f);
                                EntryPatch p1(Body(bit_cast<uintptr_t>(&FindPlayerPed)),FakeFindPed), p1e(0x56E210,FakeFindPed);
                                EntryPatch s1(Body(MemberAddress(static_cast<bool(CCamera::*)(const CVector&,float)>(&CCamera::IsSphereVisible))),FakeSphere), s1e(0x420D40,FakeSphere);
                                EntryPatch d1(Body(bit_cast<uintptr_t>(&CWaterLevel::GetWaterDepth)),FakeWaterDepth), d1e(0x6EA960,FakeWaterDepth);
                                EntryPatch c1(Body(MemberAddress(&WaterCreature_c::Init)),FakeCreatureInit), c1e(0x6E4160,FakeCreatureInit);
                                EntryPatch c2(Body(MemberAddress(&WaterCreature_c::Update)),FakeCreatureUpdate), c2e(0x6E4670,FakeCreatureUpdate);
                                reinterpret_cast<void(__thiscall*)(void*,float)>(a)(m,uni(0.f,3.f)); timeMs=savedTime; } }
                        else if(nm=="GetRandomWaterCreatureId")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(m);
                        else if(nm=="CanAddWaterCreatureAtPos"){ const int32 id=int32(next()%7); CVector p{cam.x+uni(-80.f,80.f),cam.y+uni(-80.f,80.f),cam.z+uni(-20.f,20.f)};
                            if(nl&&next()%2){ const auto* o=objs[next()%std::min(nl,24)].data(); p=CVector{At<float>((void*)o,4)+uni(-8.f,8.f),At<float>((void*)o,8)+uni(-8.f,8.f),uni(-5.f,5.f)}; }
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,int32,CVector)>(a)(m,id,p))); }
                        else { // TryToExitGroup
                            for(int k=0;k<nl;++k){ auto* o=objs[k%24].data(); const float r=next()%3 ? uni(50.f,70.f) : uni(0.f,100.f); const float ang=uni(-3.f,3.f);
                                At<float>(o,4)=cam.x+r*std::cos(ang); At<float>(o,8)=cam.y+r*std::sin(ang); At<float>(o,0xC)=cam.z; }
                            void* wcArg=nl ? item(int(next()%nl)) : item(0); if(!nl){ At<uint8*>(item(0),0x1C)=nullptr; }
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(m,wcArg); }
                        out["wc"]=Hash(m,WC_N); out["wc_lists"]=Region(m+0x1A00,0x1C); out["temp_objs"]=tempObjs;
                        json heads=json::array(); for(int k=0;k<std::min(nl+2,128);++k)heads.push_back(Region(item(k),0x20)); out["items"]=heads;
                        out["objs"]=Hash(objs.data(),sizeof(objs));
                    }
                    else { // tareas de interior
                        // peaton con matriz, clump e inteligencia falsos
                        std::memset(ped.data(),0,ped.size()); std::memset(mat.data(),0,mat.size()); std::memset(intel.data(),0,intel.size());
                        std::memset(clump.data(),0,clump.size()); clump[0]=2;
                        auto* p=Ped(); p->SetType(ENTITY_TYPE_PED); At<void*>(ped.data(),0x18)=clump.data(); At<void*>(ped.data(),0x47C)=intel.data();
                        const bool withMatrix=cls=="CTaskInteriorLieInBed"||next()%4!=0;
                        if(withMatrix){ p->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix mm{}; mm.SetRotate(0.f,0.f,uni(-3.f,3.f)); mm.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; std::memcpy(static_cast<CMatrix*>(p->m_matrix),&mm,sizeof(CMatrix)); }
                        else p->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                        p->m_fCurrentRotation=uni(-3.2f,3.2f); p->m_fAimingRotation=uni(-3.2f,3.2f);
                        for(int n=0;n<2;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); }
                        { static const int types[]{TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_ACHIEVE_HEADING}; At<int32>(fakes[0].data(),FK_TYPE)=types[next()%4]; }
                        for(auto& in:infos){ in.Type=static_cast<eInteriorInfoType>(next()%12); in.IsInUse=next()%2; const auto& pp=p->GetPosition();
                            in.Pos=next()%3 ? CVector{pp.x+uni(-0.3f,0.3f),pp.y+uni(-0.3f,0.3f),pp.z+uni(-1.f,1.f)} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                            if(next()%10==0)in.Pos=CVector{pp.x+0.2f,pp.y,pp.z};
                            in.Dir=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f}; in.EntityIgnoredCollision=nullptr; }
                        for(auto& b:interior)b=uint8(next()); for(auto& b:interior2)b=uint8(next()); for(auto& b:igroup)b=uint8(next()); for(auto& b:pathNodes)b=uint8(next());
                        const uint16 area=uint16(next()%64), nodeId=uint16(next()%8); At<uint16>(interior.data(),0x3F0)=area; At<uint16>(interior.data(),0x3F2)=nodeId;
                        for(auto& an:anims){ std::memset(an.data(),0,an.size()); for(int c=0x18;c<0x2C;c+=4)At<float>(an.data(),size_t(c))=uni(-2.f,2.f); }
                        timeMs=savedTime+next()%5000;
                        auto& nodeSlot=ThePaths.m_pPathNodes[area]; CPathNode* const savedNodes=nodeSlot; nodeSlot=reinterpret_cast<CPathNode*>(pathNodes.data());
                        std::memset(object.data(),0xCD,object.size()); auto* o=object.data();
                        const int prio=int(next()%3);
                        uintptr_t rawRet=0;
                        EntryPatch e1(Body(MemberAddress(&InteriorManager_c::GetPedsInterior)),FakePedsInterior), e1e(0x598620,FakePedsInterior);
                        EntryPatch e2(Body(MemberAddress(&InteriorManager_c::AreAnimsLoaded)),FakeAnimsLoaded), e2e(0x5980F0,FakeAnimsLoaded);
                        EntryPatch e3(Body(MemberAddress(&InteriorGroup_c::FindInteriorInfo)),FakeFindInfo), e3e(0x594970,FakeFindInfo);
                        EntryPatch e4(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FakeGetAssoc), e4e(0x4D68B0,FakeGetAssoc);
                        EntryPatch e5(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FakeBlend), e5e(0x4D4610,FakeBlend);
                        pedsInteriorRet=next()%2 ? uintptr_t(interior.data()) : uintptr_t(interior2.data()); // el EXE lee el nodo del interior del peaton sin comprobar nulo
                        animsMask=next()%4 ? 0xFFFFFFFFu : next(); assocRet=next()%3 ? uintptr_t(anims[1].data()) : 0;
                        size_t sz=0x18;
                        if(cls=="CTaskInteriorGoToInfo"){ sz=0x18;
                            At<uintptr_t>(o,0)=fn.vt; At<CTask*>(o,4)=nullptr; At<CTask*>(o,8)=next()%8 ? Fake(0) : nullptr;
                            At<InteriorInfo_t*>(o,0xC)=next()%8 ? &infos[0] : nullptr; At<void*>(o,0x10)=interior.data(); o[0x14]=uint8(next()%3==0);
                            if(nm=="CreateNextSubTask"&&!At<CTask*>(o,8))At<CTask*>(o,8)=Fake(0);
                            if(nm=="Constructor"){ std::memset(o,0xCD,object.size()); rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,InteriorInfo_t*,void*,uint8)>(a)(o,next()%4 ? &infos[0] : nullptr,interior.data(),uint8(next()%3 ? next()%2 : next()))); }
                            else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(a)(o);
                            else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(a)(o);
                            else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(a)(o));
                            else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(a)(o,p)); }
                        else if(cls=="CTaskInteriorShopKeeper"){ sz=0x20;
                            At<uintptr_t>(o,0)=fn.vt; At<CTask*>(o,4)=nullptr; At<CTask*>(o,8)=next()%8 ? Fake(0) : nullptr;
                            o[0xC]=uint8(next()%2); At<void*>(o,0x10)=igroup.data(); o[0x14]=uint8(next()%2); At<InteriorInfo_t*>(o,0x18)=next()%2 ? &infos[0] : nullptr; At<void*>(o,0x1C)=next()%2 ? interior.data() : nullptr;
                            // la UseInfo creada se borra con el destructor de la DLL, que no comprueba m_IntInfo nulo (el EXE si, 0x675A93): fuera de ese camino
                            if(nm=="CreateNextSubTask")At<InteriorInfo_t*>(o,0x18)=&infos[0];
                            if(nm=="Constructor"){ std::memset(o,0xCD,object.size()); rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,uint8)>(a)(o,igroup.data(),uint8(next()%3 ? next()%2 : next()))); }
                            else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(a)(o);
                            else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(a)(o);
                            else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(a)(o));
                            else rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(a)(o,p)); }
                        else { sz=0x30; // LieInBed
                            const int32 base=next()%2 ? 0x154 : 0x151;
                            At<uintptr_t>(o,0)=fn.vt; At<CTask*>(o,4)=nullptr; At<int32>(o,8)=next()%6==0 ? -int32(next()%100) : int32(next()%20000);
                            At<InteriorInfo_t*>(o,0xC)=&infos[0]; o[0x10]=uint8(next()%2); o[0x11]=uint8(next()%2);
                            { const uint32 k=next()%5; At<void*>(o,0x14)=k==0 ? nullptr : (void*)anims[0].data(); }
                            { static const int32 off[]{-1,0,1,2,7}; const int32 v=off[next()%5]; At<int32>(o,0x18)=v<0 ? -1 : base+v; }
                            At<uint32>(o,0x1C)=savedTime+next()%3000; At<int32>(o,0x20)=int32(next()%6000); o[0x24]=uint8(next()%2); o[0x25]=uint8(next()%3==0);
                            o[0x28]=uint8(next()%2); o[0x29]=uint8(next()%3==0); o[0x2A]=uint8(next()%3==0); At<int32>(o,0x2C)=base;
                            { static const int32 off[]{0,1,2,1,2,5}; At<int16>(anims[0].data(),0x2C)=int16(base+off[next()%6]); At<int16>(anims[1].data(),0x2C)=int16(base+2); }
                            if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                                rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,InteriorInfo_t*,uint8,uint8)>(a)(o,int32(next()%20000),&infos[0],uint8(next()%3 ? next()%2 : next()),uint8(next()%3 ? next()%2 : next()))); }
                            else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(a)(o);
                            else if(nm=="FinishAnimCB"){ auto* an=anims[next()%2].data(); { static const int32 off[]{0,1,2,3,-1}; At<int16>(an,0x2C)=int16(base+off[next()%5]); }
                                reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(an,o); }
                            else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(a)(o);
                            else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(a)(o));
                            else if(nm=="MakeAbortable")ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,CPed*,int,const void*)>(a)(o,p,prio,nullptr)));
                            else ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,CPed*)>(a)(o,p))); }
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool();
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        if(ret.is_null()&&nm!="Destructor"&&nm!="FinishAnimCB")ret=Name(rawRet);
                        json tasks=json::array();
                        for(auto* t:fresh){ json w=Region(t,0x60,true); const int type=t->GetTaskType();
                            if(type==TASK_COMPLEX_FOLLOW_NODE_ROUTE){ auto* r=static_cast<CTaskComplexFollowNodeRoute*>(t);
                                w[(uintptr_t(&r->m_NodeRoute)-uintptr_t(t))/4-1]="route"; w[(uintptr_t(&r->m_PtRoute)-uintptr_t(t))/4-1]="route"; }
                            if(type==TASK_INTERIOR_LIE_IN_BED)w[3]=json(Name(At<uint32>(t,0x10)&0xFFFFFF00u)); // +0x10: lado derecho solo escrito con FIX_BUGS
                            tasks.push_back({{"type",type},{"words",w}}); }
                        json objWords=Region(o,sz,true);
                        if(nm=="Constructor"){ objWords.push_back(reinterpret_cast<CTask*>(o)->GetTaskType()); if(cls=="CTaskInteriorLieInBed")objWords[3]=json(Name(At<uint32>(o,0x10)&0xFFFFFF00u)); }
                        const auto& pp=p->GetPosition();
                        out["object"]=objWords; out["tasks"]=tasks; out["anims"]=json::array({Region(anims[0].data(),0x3C),Region(anims[1].data(),0x3C),Region(anims[2].data(),0x3C),Region(anims[3].data(),0x3C)});
                        out["ped"]=json::array({Hash(ped.data(),ped.size()),Hash(mat.data(),mat.size()),F(pp.x),F(pp.y),F(pp.z),F(p->m_fCurrentRotation),F(p->m_fAimingRotation),
                                                F(p->m_vecAnimMovingShiftLocal.x),F(p->m_vecAnimMovingShiftLocal.y)});
                        out["intel"]=Hash(intel.data(),intel.size()); out["infos"]=json::array({infos[0].IsInUse,infos[1].IsInUse}); out["fakes"]=Region(fakes[0].data(),0x40,true);
                        nodeSlot=savedNodes;
                        for(auto* t:fresh)delete t;
                        out["clean"]=Occupancy()==occ;
                        timeMs=savedTime;
                    }
                    for(auto* h:hooks)h->State(false);
                    restore();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                // el banco corre de forma sincrona: se consulta la cola (sin sacar mensajes) para que Windows no marque la ventana como colgada
                { static ULONGLONG lastPump=GetTickCount64(); if(GetTickCount64()-lastPump>1000){ MSG msg{}; PeekMessageA(&msg,nullptr,0,0,PM_NOREMOVE); lastPump=GetTickCount64(); } }
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
