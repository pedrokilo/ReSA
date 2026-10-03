#pragma once
// Banco dentro del motor: datos de armas, plantillas de efectos y estar en grupo (lista de 10; 30 funciones).
// CWeaponInfo (FindWeaponFireType, FindWeaponType, GetCrouchReloadAnimationID, GetSkillStatIndex, GetTargetHeadRange,
// GetWeaponInfo, GetWeaponReloadTime, Initialise, LoadWeaponData, Shutdown), FxSystemBP_c (constructor, destructor,
// operator new, Load, Unload, Update, Render, FreePrtFromSystem, GetBoundingSphere, SetBoundingSphere) y
// CTaskComplexBeInGroup (constructor, destructor, Clone, GetTaskType, MakeAbortable, CreateNextSubTask,
// CreateFirstSubTask, ControlSubTask, MonitorMainGroupTask, MonitorSecondaryGroupTask).
// Original (hooks del grupo desactivados, codigo del EXE) frente a candidato con fixtures identicos.
// Se simulan y trazan: CFileMgr::OpenFile/ReadLine/CloseFile (lineas generadas de weapon.dat y de effects.fxp),
// FxMemoryPool_c::GetMem (arena propia), FxEmitterBP_c::Load/LoadTextures/IsFxInfoPresent, las primitivas de los
// blueprints (vtable falsa), CPedGroupIntelligence::GetTaskMain/GetTaskSecondary/GetTaskSecondarySlot/
// ReportFinishedTask, CTask::IsTaskPtr y las tareas falsas (vtable trazada). Peatones e inteligencias falsas.
// La tabla real de armas (aWeaponInfo), los desplazamientos de apuntado, CFileLoader::ms_line, los punteros de
// model info usados y el pool de tareas se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "WeaponInfo.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "AnimManager.h"
#include "ModelInfo.h"
#include "FxSystemBP.h"
#include "FxEmitterBP.h"
#include "FxPrimBP.h"
#include "FxMemoryPool.h"
#include "FxManager.h"
#include "Tasks/Task.h"
#include "TaskComplexBeInGroup.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace Data10Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- direcciones crudas del EXE ---
constexpr uintptr_t WI_BASE=0xC8AAB8, AO_BASE=0xC8A8A8, MS_LINE=0xB71848, FX_POOL=0xA9AF2C, PEDGROUPS=0xC09920, GRP_INTEL=0xC09950;
constexpr size_t WI_SIZE=0x70, WI_COUNT=80, AO_SIZE=0x18, AO_COUNT=21, PEDGROUP_SIZE=0x2D4;
constexpr uintptr_t VT_BIG=0x86E22C, VT_EMITTER_EXE=0x85A788;
constexpr size_t P_INTEL=0x47C, E_TYPE=0x36, I_TM=0x4, TM_SECONDARY=0x14, TM_PED=0x2C;
// tareas falsas: campos de control por encima de 0x50
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C;
constexpr int NPEDS=2, FAKES=6, NPRIMS=8;

// --- buffers de los fixtures ---
alignas(16) inline std::array<uint8,0x80> wiObj{};                 // CWeaponInfo falso
alignas(16) inline std::array<std::array<uint8,0x40>,20> fakeModels{};
inline const std::array<int32,20> modelSet{321,322,323,324,325,326,327,328,329,330,346,347,348,349,350,351,352,353,354,355};
alignas(16) inline std::array<uint8,0x40> bpObj{};                  // FxSystemBP_c
alignas(16) inline std::array<uint8,0x1000> arena{};                // FxMemoryPool_c::GetMem
inline size_t arenaPos{};
alignas(16) inline std::array<std::array<uint8,0x10>,NPRIMS> prims{};
alignas(16) inline std::array<void*,256> primPtrs{};
alignas(16) inline std::array<uint8,0x40> camBuf{}, sysBuf{}, centerBuf{};
alignas(16) inline std::array<uint8,0x40> object{};                 // CTaskComplexBeInGroup
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x20> eventArg{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// --- pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot) ---
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
inline int32& PoolLast() { return At<int32>(PoolHdr(),0xC); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}
inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }

inline std::array<void*,16> fakeVtbl{}, primVtbl{};
inline uintptr_t dllEmitterVt{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(primVtbl.data()))return "prim_vt";
    if(v==VT_EMITTER_EXE||(dllEmitterVt&&v==dllEmitterVt))return "emitter_vt";
    if(v==FX_POOL)return "fx_pool";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NPRIMS;++n)if(auto s=in(prims[n].data(),prims[n].size(),"prim"))return *s+"#"+std::to_string(n);
    if(auto s=in(primPtrs.data(),sizeof(primPtrs),"prims"))return *s; if(auto s=in(arena.data(),arena.size(),"arena"))return *s;
    if(auto s=in(bpObj.data(),bpObj.size(),"bp"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(camBuf.data(),camBuf.size(),"camera"))return *s; if(auto s=in(sysBuf.data(),sysBuf.size(),"system"))return *s;
    if(auto s=in(eventArg.data(),eventArg.size(),"event_arg"))return *s; if(auto s=in(wiObj.data(),wiObj.size(),"wi_obj"))return *s;
    if(v>=PEDGROUPS&&v<PEDGROUPS+8*PEDGROUP_SIZE)return "pedgroups+"+std::to_string(v-PEDGROUPS);
    if(v>=WI_BASE&&v<WI_BASE+WI_SIZE*WI_COUNT)return "wi+"+std::to_string(v-WI_BASE);
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); return r; }
inline json SerTask(const void* p,size_t bytes=TASK_SLOT) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// --- archivos (weapon.dat / effects.fxp) ---
inline std::vector<std::string> fileLines; inline size_t fileLine{};
inline FILESTREAM __cdecl FakeOpenFile(const char* path,const char* mode){ trace.push_back({"open_file",std::string(path),std::string(mode)}); return reinterpret_cast<FILESTREAM>(0x7A10); }
// devuelve 1/0 en todo eax: los llamadores del EXE prueban eax y los de la DLL al
inline int32 __cdecl FakeReadLine(FILESTREAM f,char* buf,int32 n){
    if(fileLine>=fileLines.size()){ trace.push_back({"read_line_eof",uint32(uintptr_t(f)),n}); return 0; }
    const auto& l=fileLines[fileLine]; const size_t m=std::min(l.size(),size_t(std::max(n,1)-1)); std::memcpy(buf,l.data(),m); buf[m]=0;
    trace.push_back({"read_line",uint32(uintptr_t(f)),fileLine++,n}); return 1;
}
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close_file",uint32(uintptr_t(f))}); return 0; }

// --- efectos ---
inline void* __fastcall FakeGetMem(void* pool,void*,int32 size,int32 align){
    if(align>0)arenaPos=(arenaPos+size_t(align)-1)/size_t(align)*size_t(align);
    void* r=arena.data()+std::min(arenaPos,arena.size()-0x100);
    trace.push_back({"get_mem",Name(pool),size,align,Name(r)});
    if(size>0)arenaPos=std::min(arenaPos+size_t(size),arena.size()-0x100);
    return r;
}
inline uintptr_t texBase{}; inline uint32 texCounter{}; inline uint32 infoMask{};
inline json TexOff(const void* t){ if(!texBase)texBase=uintptr_t(t); return int32(uintptr_t(t)-texBase); }
inline bool __fastcall FakeEmitterLoad(void* self,void*,FILESTREAM f,int32 version,char (*tex)[32]){
    trace.push_back({"emitter_load",Name(self),uint32(uintptr_t(f)),version,TexOff(tex)});
    for(int k=0;k<4;++k)snprintf(tex[k],32,"tex%u_%d",texCounter,k); ++texCounter; return true;
}
inline bool __fastcall FakeEmitterTextures(void* self,void*,char (*tex)[32],int32 version){
    std::string s0(tex[0],strnlen(tex[0],32)), s3(tex[3],strnlen(tex[3],32));
    trace.push_back({"emitter_textures",Name(self),TexOff(tex),s0,s3,version}); return true;
}
inline uint32 infoCalls{};
inline bool __fastcall FakeIsFxInfoPresent(const void* self,void*,int32 type){
    const bool r=(infoMask>>(infoCalls++%32))&1; trace.push_back({"is_fx_info_present",Name(self),type,r}); return r;
}
inline void __fastcall PUpdate(void* p,void*,float t){ trace.push_back({"prim_update",Name(p),F(t)}); }
inline void __fastcall PRender(void* p,void*,RwCamera* c,uint32 a2,float d,uint32 hh){ trace.push_back({"prim_render",Name(p),Name(c),a2,F(d),hh&0xFF}); }
inline bool __fastcall PFree(void* p,void*,FxSystem_c* s){ const bool r=At<uint8>(p,4)!=0; trace.push_back({"prim_free",Name(p),Name(s),r}); return r; }

// --- grupos y tareas falsas ---
inline CTask* mainRet{}; inline CTask* secRet{}; inline int32 slotRet{}; inline uint32 reportMask{}, reportCalls{}, isTaskMask{};
inline CTask* __fastcall FakeGetTaskMain(void* gi,void*,CPed* p){ trace.push_back({"get_task_main",Name(gi),Name(p),Name(mainRet)}); return mainRet; }
inline CTask* __fastcall FakeGetTaskSecondary(void* gi,void*,CPed* p){ trace.push_back({"get_task_secondary",Name(gi),Name(p),Name(secRet)}); return secRet; }
inline int32 __fastcall FakeGetTaskSecondarySlot(void* gi,void*,CPed* p){ trace.push_back({"get_task_secondary_slot",Name(gi),Name(p),slotRet}); return slotRet; }
inline bool __fastcall FakeReport(void* gi,void*,const CPed* p,const CTask* t){ const bool r=(reportMask>>(reportCalls++%32))&1; trace.push_back({"report_finished_task",Name(gi),Name(p),Name(t),r}); return r; }
inline bool __cdecl FakeIsTaskPtr(CTask* t){ int n=-1; for(int k=0;k<FAKES;++k)if(t==Fake(k))n=k; const bool r=n>=0&&((isTaskMask>>n)&1); trace.push_back({"is_task_ptr",Name(t),r}); return r; }
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=30;
inline const std::array<Fn,NFNS> fns{{
    {"CWeaponInfo","FindWeaponFireType",0x5BCF30,0,-1,256},
    {"CWeaponInfo","FindWeaponType",0x743D10,0,-1,256},
    {"CWeaponInfo","GetCrouchReloadAnimationID",0x685700,0,-1,256},
    {"CWeaponInfo","GetSkillStatIndex",0x743CD0,0,-1,128},
    {"CWeaponInfo","GetTargetHeadRange",0x743D50,0,-1,512},
    {"CWeaponInfo","GetWeaponInfo-original",0x743C60,0,-1,512},
    {"CWeaponInfo","GetWeaponReloadTime",0x743D70,0,-1,512},
    {"CWeaponInfo","Initialise",0x5BF750,0,-1,32},
    {"CWeaponInfo","LoadWeaponData",0x5BE670,0,-1,512},
    {"CWeaponInfo","Shutdown",0x743C50,0,-1,16},
    {"FxSystemBP_c","Constructor",0x4AA0D0,0,-1,64},
    {"FxSystemBP_c","Destructor",0x4AA0F0,0,-1,64},
    {"FxSystemBP_c","FreePrtFromSystem",0x4AA1B0,0,-1,256},
    {"FxSystemBP_c","GetBoundingSphere",0x4AA1F0,0,-1,64},
    {"FxSystemBP_c","Load",0x5C05F0,0,-1,512},
    {"FxSystemBP_c","Render",0x4AA160,0,-1,256},
    {"FxSystemBP_c","SetBoundingSphere",0x4AA200,0,-1,256},
    {"FxSystemBP_c","Unload",0x4AA120,0,-1,16},
    {"FxSystemBP_c","Update",0x4AA130,0,-1,256},
    {"FxSystemBP_c","operator new",0x4AA100,0,-1,128},
    {"CTaskComplexBeInGroup","Clone",0x636BE0,0x86E22C,1,128},
    {"CTaskComplexBeInGroup","Constructor",0x632E50,0,-1,128},
    {"CTaskComplexBeInGroup","ControlSubTask",0x638AA0,0x86E22C,10,1024},
    {"CTaskComplexBeInGroup","CreateFirstSubTask",0x632FB0,0x86E22C,9,256},
    {"CTaskComplexBeInGroup","CreateNextSubTask",0x632F40,0x86E22C,8,256},
    {"CTaskComplexBeInGroup","Destructor",0x632EA0,0,-1,128},
    {"CTaskComplexBeInGroup","GetTaskType",0x632E90,0x86E22C,4,64},
    {"CTaskComplexBeInGroup","MakeAbortable",0x632EB0,0x86E22C,6,512},
    {"CTaskComplexBeInGroup","MonitorMainGroupTask",0x633010,0,-1,512},
    {"CTaskComplexBeInGroup","MonitorSecondaryGroupTask",0x6330B0,0,-1,1024},
}};

inline void CleanRefs() { for(auto& p:peds){ auto* e=reinterpret_cast<CEntity*>(p.data()); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); } }

inline const char* kWeaponNames[]{"UNARMED","BRASSKNUCKLE","GOLFCLUB","NIGHTSTICK","KNIFE","BASEBALLBAT","SHOVEL","POOLCUE","KATANA","CHAINSAW","DILDO1",
    "DILDO2","VIBE1","VIBE2","FLOWERS","CANE","GRENADE","TEARGAS","MOLOTOV","ROCKET","ROCKET_HS","FREEFALL_BOMB","PISTOL","PISTOL_SILENCED","DESERT_EAGLE",
    "SHOTGUN","SAWNOFF","SPAS12","MICRO_UZI","MP5","AK47","M4","TEC9","COUNTRYRIFLE","SNIPERRIFLE","RLAUNCHER","RLAUNCHER_HS","FTHROWER","MINIGUN",
    "SATCHEL_CHARGE","DETONATOR","SPRAYCAN","EXTINGUISHER","CAMERA","NIGHTVISION","INFRARED","PARACHUTE","","ARMOUR"};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"data10-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"data10-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("data10");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"data10-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(primVtbl,std::make_integer_sequence<int,16>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    primVtbl[4]=reinterpret_cast<void*>(&PUpdate); primVtbl[5]=reinterpret_cast<void*>(&PRender); primVtbl[6]=reinterpret_cast<void*>(&PFree);
    // vtable del FxEmitterBP_c de la DLL (el constructor del EXE puede saltar a la DLL): emisor de prueba en un buffer propio
    { alignas(16) static std::array<uint8,0x48> probe{}; new (probe.data()) FxEmitterBP_c(); dllEmitterVt=At<uintptr_t>(probe.data(),0); }

    // nombres de grupos de animacion (solo lectura)
    const int32 ng=*reinterpret_cast<const int32*>(0xB4EA28); // CAnimManager::ms_numAnimAssocDefinitions (privado)
    std::vector<std::string> groupNames; for(int32 g=0;g<ng;++g)groupNames.emplace_back(CAnimManager::GetAnimGroupName(AssocGroupId(g)));
    auto firstIndex=[&](const std::string& s){ for(int32 g=0;g<ng;++g)if(groupNames[g]==s)return g; return ng; };
    std::vector<std::string> aimGroups; for(int32 g=11;g<=31&&g<ng;++g)if(firstIndex(groupNames[g])==g)aimGroups.push_back(groupNames[g]);
    // grupo desconocido en '%': se escribe en AO_BASE + (ng - 11) * 0x18; solo si cae dentro de las tablas guardadas
    const bool unknownAimOk=ng>=11&&AO_BASE+size_t(ng-11)*AO_SIZE+AO_SIZE<=WI_BASE+WI_SIZE*WI_COUNT;

    // estado global tocado
    auto* wiTable=reinterpret_cast<uint8*>(WI_BASE); auto* aoTable=reinterpret_cast<uint8*>(AO_BASE); auto* msLine=reinterpret_cast<uint8*>(MS_LINE);
    const std::vector<uint8> savedWi(wiTable,wiTable+WI_SIZE*WI_COUNT), savedAo(aoTable,aoTable+AO_SIZE*AO_COUNT), savedLine(msLine,msLine+512);
    auto& modelPtrs=CModelInfo::ms_modelInfoPtrs; std::array<CBaseModelInfo*,20> savedModels{}; for(size_t k=0;k<modelSet.size();++k)savedModels[k]=modelPtrs[modelSet[k]];
    auto restoreGlobals=[&]{ std::memcpy(wiTable,savedWi.data(),savedWi.size()); std::memcpy(aoTable,savedAo.data(),savedAo.size()); std::memcpy(msLine,savedLine.data(),512);
                             for(size_t k=0;k<modelSet.size();++k)modelPtrs[modelSet[k]]=savedModels[k]; };
    auto globalsIntact=[&]{ bool ok=!std::memcmp(wiTable,savedWi.data(),savedWi.size())&&!std::memcmp(aoTable,savedAo.data(),savedAo.size())&&!std::memcmp(msLine,savedLine.data(),512);
                            for(size_t k=0;k<modelSet.size();++k)ok&=modelPtrs[modelSet[k]]==savedModels[k]; return ok; };
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lf0=PoolLast();
    const std::vector<uint8> statesStart(PoolStates(),PoolStates()+PoolCap());
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), f1e(0x538900,FakeOpenFile);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::ReadLine),FakeReadLine), f2e(0x5389B0,FakeReadLine);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), f3e(0x5389D0,FakeCloseFile);
        EntryPatch m1(bit_cast<uintptr_t>(&FxMemoryPool_c::GetMem),FakeGetMem), m1e(0x4A9CA0,FakeGetMem);
        EntryPatch x1(bit_cast<uintptr_t>(&FxEmitterBP_c::IsFxInfoPresent),FakeIsFxInfoPresent), x1e(0x4A24D0,FakeIsFxInfoPresent);
        // Load (ranura 1) y LoadTextures (ranura 2) del emisor: destinos de la vtable del EXE y de la DLL, y las funciones originales
        std::vector<std::unique_ptr<EntryPatch>> emitterPatches; std::vector<uintptr_t> patched;
        auto patchOnce=[&](uintptr_t at,auto fn){ if(!at||std::find(patched.begin(),patched.end(),at)!=patched.end())return; patched.push_back(at); emitterPatches.push_back(std::make_unique<EntryPatch>(at,fn)); };
        patchOnce(reinterpret_cast<uintptr_t*>(VT_EMITTER_EXE)[1],FakeEmitterLoad); patchOnce(reinterpret_cast<uintptr_t*>(dllEmitterVt)[1],FakeEmitterLoad); patchOnce(0x5C25F0,FakeEmitterLoad);
        patchOnce(reinterpret_cast<uintptr_t*>(VT_EMITTER_EXE)[2],FakeEmitterTextures); patchOnce(reinterpret_cast<uintptr_t*>(dllEmitterVt)[2],FakeEmitterTextures); patchOnce(0x5C0A30,FakeEmitterTextures);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroupIntelligence::GetTaskMain),FakeGetTaskMain), g1e(0x5F85A0,FakeGetTaskMain);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGroupIntelligence::GetTaskSecondary),FakeGetTaskSecondary), g2e(0x5F8620,FakeGetTaskSecondary);
        EntryPatch g3(bit_cast<uintptr_t>(&CPedGroupIntelligence::GetTaskSecondarySlot),FakeGetTaskSecondarySlot), g3e(0x5F8650,FakeGetTaskSecondarySlot);
        EntryPatch g4(bit_cast<uintptr_t>(static_cast<bool(CPedGroupIntelligence::*)(const CPed*,const CTask*)>(&CPedGroupIntelligence::ReportFinishedTask)),FakeReport), g4e(0x5F86F0,FakeReport);
        EntryPatch t1(bit_cast<uintptr_t>(&CTask::IsTaskPtr),FakeIsTaskPtr), t1e(0x61A4B0,FakeIsTaskPtr);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                // decimales exactos en binario (k/8): las dos CRT convierten igual
                auto fx=[&](int lo,int hi)->std::string{ const int k=lo*8+int(next()%uint32((hi-lo)*8+1)); char b[48]; snprintf(b,sizeof(b),"%.3f",k/8.0); return b; };
                auto I=[&](int64 v){ return std::to_string(v); };
                auto run=[&](bool candidate)->json {
                    PoolLast()=lf0;
                    if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xDA7Au; if(!st)st=1; trace=json::array(); freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    fileLines.clear(); fileLine=0; arenaPos=0; std::memset(arena.data(),0xCD,arena.size()); texBase=0; texCounter=0; infoCalls=0; reportCalls=0;

                    const auto occ=Occupancy();
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    json out=json::object(); json ret=nullptr; std::vector<CTask*> roots;
                    auto setState=[&](bool c){ for(auto* h:hooks)h->State(c); };
                    auto switched=[&]{ const uintptr_t t=fn.slot>=0 ? vt[fn.slot] : fn.address;
                                       return fn.slot>=0 ? ((t>=0x400000&&t<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate); };

                    if(cls=="CWeaponInfo") {
                        auto* w=wiObj.data();
                        auto wname=[&]()->std::string{ const uint32 m=next()%10; std::string s=kWeaponNames[next()%49]; if(s.empty())s="PISTOL";
                            if(m==0)for(auto& c:s)c=char(tolower(c)); else if(m==1)s="BOGUS_WEAPON"; return s; };
                        auto wtOf=[&](const std::string& s){ for(int k=0;k<49;++k)if(!_stricmp(kWeaponNames[k],s.c_str()))return k; return 0; };
                        auto fire=[&]()->std::string{ static const char* n[]{"MELEE","INSTANT_HIT","PROJECTILE","AREA_EFFECT","CAMERA","USE","melee","USE2","NONE"}; return n[next()%9]; };
                        auto mid=[&]()->std::string{ const uint32 m=next()%6; return m==0 ? "-1" : m==1 ? "0" : I(modelSet[next()%modelSet.size()]); };
                        auto grp=[&]()->std::string{ switch(next()%10){ case 0: return "null"; case 1: return "nullgun"; case 2: return "NOGROUP_XYZ";
                            case 3: { auto s=groupNames[next()%size_t(ng)]; for(auto& c:s)c=char(toupper(c)); return s; }
                            case 4: case 5: if(!aimGroups.empty())return aimGroups[next()%aimGroups.size()]; [[fallthrough]];
                            default: return groupNames[next()%size_t(ng)]; } };
                        auto genLines=[&](int count){
                            for(int k=0;k<count;++k){
                                std::string l; const std::string lead=next()%6==0 ? "  " : "";
                                switch(next()%12){
                                case 0: case 1: case 2: case 3: { // arma de fuego
                                    const std::string n=wname(); const int wt=wtOf(n); const bool skilled=wt>=22&&wt<=32;
                                    l="$ "+n+" "+fire()+" "+fx(0,200)+" "+fx(0,200)+" "+mid()+" "+mid()+" "+I(int(next()%14)-1)+" "+grp()+" "+I(next()%70001)+" "+I(next()%70001)
                                     +" "+fx(-2,2)+" "+fx(-2,2)+" "+fx(-2,2)+" "+I(skilled ? next()%4 : next()%10)+" "+I(next()%1000)+" "+fx(0,4)+" "+fx(0,4);
                                    for(int a=0;a<6;++a)l+=" "+I(next()%90);
                                    l+=" "+I(next()%120); { char h[16]; snprintf(h,sizeof(h)," %X",next()); l+=h; }
                                    const uint32 opt=next()%5; for(uint32 a=0;a<opt;++a)l+=" "+fx(-50,50);
                                    break; }
                                case 4: case 5: { // cuerpo a cuerpo (0xA3)
                                    static const char* combos[]{"UNARMED","BBALLBAT","KNIFE","GOLFCLUB","SWORD","CHAINSAW","DILDO","FLOWERS","unarmed","BOGUS"};
                                    l=std::string("\xA3 ")+wname()+" "+fire()+" "+fx(0,20)+" "+fx(0,20)+" "+mid()+" "+mid()+" "+I(int(next()%14)-1)+" "+combos[next()%10]+" "+I(next()%300);
                                    { char h[16]; snprintf(h,sizeof(h)," %X ",next()); l+=h; } l+=grp(); break; }
                                case 6: case 7: { // desplazamientos de apuntado
                                    std::string g=aimGroups.empty()||(unknownAimOk&&next()%8==0) ? std::string("NOGROUP_XYZ") : aimGroups[next()%aimGroups.size()];
                                    if(g=="NOGROUP_XYZ"&&!unknownAimOk){ l="# sin grupos"; break; }
                                    l="% "+g+" "+fx(-2,2)+" "+fx(-2,2)+" "+fx(-2,2)+" "+fx(-2,2);
                                    for(int a=0;a<4;++a)l+=" "+I(int64(next()%70100)-100);
                                    break; }
                                case 8: l=next()%2 ? "# comentario $ PISTOL" : "#ENDWEAPONDATA"; break;
                                case 9: l=next()%2 ? "" : "   "; break;
                                case 10: l=next()%3==0 ? "ENDWEAPONDATA" : next()%2 ? "ENDWEAPONDATAX" : "XENDWEAPONDATA"; break;
                                default: l="otra linea cualquiera"; break;
                                }
                                fileLines.push_back(lead+l);
                            }
                        };
                        auto prepareTables=[&]{
                            for(size_t e=0;e<WI_COUNT;++e)if(next()%4==0)for(size_t b=0;b<WI_SIZE;++b)wiTable[e*WI_SIZE+b]=uint8(next());
                            for(size_t b=0;b<AO_SIZE*AO_COUNT;++b)if(next()%3==0)aoTable[b]=uint8(next());
                            for(size_t k=0;k<modelSet.size();++k){ std::memset(fakeModels[k].data(),0xCD,fakeModels[k].size()); modelPtrs[modelSet[k]]=reinterpret_cast<CBaseModelInfo*>(fakeModels[k].data()); }
                        };
                        auto tablesOut=[&](const std::vector<uint64>& before){
                            json h=json::array(), changed=json::object();
                            for(size_t e=0;e<WI_COUNT;++e){ const uint64 x=Fnv(wiTable+e*WI_SIZE,WI_SIZE); h.push_back(x); if(x!=before[e])changed[std::to_string(e)]=ReversingProbe::Bytes(wiTable+e*WI_SIZE,WI_SIZE); }
                            out["wi_hash"]=h; out["wi_changed"]=changed; out["ao"]=ReversingProbe::Bytes(aoTable,AO_SIZE*AO_COUNT);
                            json mo=json::array(); for(auto& m:fakeModels)mo.push_back(At<uint32>(m.data(),0x24)); out["models"]=mo;
                        };
                        auto hashes=[&]{ std::vector<uint64> b(WI_COUNT); for(size_t e=0;e<WI_COUNT;++e)b[e]=Fnv(wiTable+e*WI_SIZE,WI_SIZE); return b; };
                        for(auto& b:wiObj)b=uint8(next());

                        if(nm=="FindWeaponFireType"||nm=="FindWeaponType"){
                            std::string s;
                            if(nm=="FindWeaponFireType"){ static const char* n[]{"MELEE","INSTANT_HIT","PROJECTILE","AREA_EFFECT","CAMERA","USE","melee","USE ","US","USEX","","INSTANT","PROJECTILES","camera","AREA_EFFECT2","MELE"};
                                s=n[next()%16]; }
                            else { s=kWeaponNames[next()%49]; const uint32 m=next()%6; if(m==0)for(auto& c:s)c=char(tolower(c)); else if(m==1&&!s.empty())s[next()%s.size()]=char('a'+next()%26); else if(m==2)s+="X"; }
                            if(next()%8==0){ s.clear(); const uint32 n=next()%8; for(uint32 k=0;k<n;++k)s+=char('A'+next()%27); }
                            setState(candidate); out["switches"]=switched();
                            ret=reinterpret_cast<int32(__cdecl*)(const char*)>(target)(s.c_str()); out["arg"]=s;
                        } else if(nm=="GetCrouchReloadAnimationID"){
                            At<uint32>(w,0x18)=next()%2 ? (next()&0x3000u)|(next()&~0x3000u) : next();
                            setState(candidate); out["switches"]=switched();
                            ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(w);
                        } else if(nm=="GetSkillStatIndex"){
                            const int32 wt=int32(next()%54)-3;
                            setState(candidate); out["switches"]=switched();
                            // eStats es uint16 en la DLL: se comparan los 16 bits bajos
                            ret=reinterpret_cast<int32(__cdecl*)(int32)>(target)(wt)&0xFFFF; out["arg"]=wt;
                        } else if(nm=="GetTargetHeadRange"){
                            const uint32 m=next()%6; At<int32>(w,0x30)=m==0 ? int32(next()) : m==1 ? -int32(next()%5) : int32(next()%4);
                            const uint32 r=next()%8; At<float>(w,8)=r==0 ? 0.f : r==1 ? -uni(0.f,100.f) : r==2 ? uni(0.f,1e6f) : uni(0.f,200.f);
                            setState(candidate); out["switches"]=switched();
                            ret=F(reinterpret_cast<float(__thiscall*)(void*)>(target)(w)); out["skill"]=At<int32>(w,0x30); out["range"]=F(At<float>(w,8));
                        } else if(nm=="GetWeaponInfo-original"){
                            int32 wt; switch(next()%6){ case 0: wt=int32(next()%120)-20; break; case 1: wt=int32(0x10000u+next()%60); break; case 2: wt=int32(0xFFFF0000u|(next()%60)); break; default: wt=int32(next()%47); }
                            const uint32 sk=next()%5==0 ? next()%256 : next()%4;
                            setState(candidate); out["switches"]=switched();
                            const auto p=uintptr_t(reinterpret_cast<void*(__cdecl*)(int32,uint32)>(target)(wt,sk));
                            ret=json::array({int32(p-WI_BASE)/int32(WI_SIZE),int32(p-WI_BASE)%int32(WI_SIZE)}); out["arg"]=json::array({wt,sk});
                        } else if(nm=="GetWeaponReloadTime"){
                            At<uint32>(w,0x18)=next()%2 ? (next()&~0x9800u)|(next()%3==0 ? 0x1000u : 0u)|(next()%2 ? 0x800u : 0u)|(next()%3==0 ? 0x8000u : 0u) : next();
                            At<int16>(w,0x6C)=int16(next()%6==0 ? int32(next()%5)-2 : int32(next()%21));
                            for(size_t e=0;e<AO_COUNT;++e)for(int k=0;k<4;++k){ const uint32 m=next()%4; At<int16>(aoTable,e*AO_SIZE+0x10+2*k)=int16(m==0 ? int32(next()%65536) : m==1 ? 300+int32(next()%5) : int32(next()%700)); }
                            setState(candidate); out["switches"]=switched();
                            ret=reinterpret_cast<uint32(__thiscall*)(void*)>(target)(w);
                        } else if(nm=="Shutdown"){
                            const auto before=hashes();
                            setState(candidate); out["switches"]=switched();
                            reinterpret_cast<void(__cdecl*)()>(target)(); tablesOut(before);
                        } else if(nm=="Initialise"){
                            prepareTables(); genLines(int(next()%4)); const auto before=hashes();
                            setState(candidate); out["switches"]=switched();
                            reinterpret_cast<void(__cdecl*)()>(target)(); setState(false); tablesOut(before);
                        } else { // LoadWeaponData
                            prepareTables(); genLines(1+int(next()%10)); const auto before=hashes();
                            out["lines"]=fileLines.size();
                            setState(candidate); out["switches"]=switched();
                            reinterpret_cast<void(__cdecl*)()>(target)(); setState(false); tablesOut(before);
                        }
                        setState(false);
                    }
                    else if(cls=="FxSystemBP_c") {
                        auto* bp=bpObj.data(); for(auto& b:bpObj)b=uint8(next());
                        for(int n=0;n<NPRIMS;++n){ std::memset(prims[n].data(),0,prims[n].size()); At<void*>(prims[n].data(),0)=primVtbl.data(); prims[n][4]=uint8(next()%4==0); }
                        for(size_t k=0;k<primPtrs.size();++k)primPtrs[k]=prims[k%NPRIMS].data();
                        infoMask=next();
                        auto setPrims=[&]{ const uint32 m=next()%8; bp[0x1B]=uint8(m==0 ? 128+next()%128 : m==1 ? 0 : next()%(NPRIMS+1)); At<void*>(bp,0x1C)=primPtrs.data(); };
                        if(nm=="Constructor"){ std::memset(bp,0xCD,bpObj.size()); setState(candidate); out["switches"]=switched(); ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(bp)); }
                        else if(nm=="Destructor"){ setState(candidate); out["switches"]=switched(); reinterpret_cast<void(__thiscall*)(void*)>(target)(bp); }
                        else if(nm=="Unload"){ setState(candidate); out["switches"]=switched(); reinterpret_cast<void(__thiscall*)(void*)>(target)(bp); }
                        else if(nm=="operator new"){ const uint32 size=next()%4 ? 0x24 : next()%0x100; arenaPos=next()%16;
                            setState(candidate); out["switches"]=switched(); ret=Name(reinterpret_cast<void*(__cdecl*)(uint32)>(target)(size)); }
                        else if(nm=="GetBoundingSphere"){ if(next()%3==0)At<void*>(bp,0x20)=nullptr; setState(candidate); out["switches"]=switched(); ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(target)(bp)); }
                        else if(nm=="SetBoundingSphere"){
                            At<void*>(bp,0x20)=next()%3==0 ? static_cast<void*>(sysBuf.data()) : nullptr;
                            CVector c{uni(-1000.f,1000.f),uni(-1000.f,1000.f),uni(-100.f,100.f)};
                            float r; switch(next()%8){ case 0: r=0.f; break; case 1: r=next()%2 ? -0.f : 1e-6f; break; case 2: r=qnan; break; case 3: r=-uni(0.f,10.f); break; case 4: r=1e-30f; break; default: r=uni(0.f,50.f); }
                            arenaPos=next()%16;
                            setState(candidate); out["switches"]=switched();
                            reinterpret_cast<void(__thiscall*)(void*,const CVector*,float)>(target)(bp,&c,r);
                            out["arena"]=Region(arena.data(),0x40);
                        }
                        else if(nm=="Update"){ setPrims(); const float t=next()%4 ? uni(0.f,0.1f) : uni(-1.f,10.f);
                            setState(candidate); out["switches"]=switched(); reinterpret_cast<void(__thiscall*)(void*,float)>(target)(bp,t); }
                        else if(nm=="Render"){ setPrims(); const float d=uni(0.f,1.f); const bool hh=next()%2;
                            setState(candidate); out["switches"]=switched(); reinterpret_cast<void(__thiscall*)(void*,RwCamera*,float,bool)>(target)(bp,reinterpret_cast<RwCamera*>(camBuf.data()),d,hh); }
                        else if(nm=="FreePrtFromSystem"){ setPrims();
                            setState(candidate); out["switches"]=switched(); ret=reinterpret_cast<bool(__thiscall*)(void*,FxSystem_c*)>(target)(bp,reinterpret_cast<FxSystem_c*>(sysBuf.data())); }
                        else { // Load
                            const int32 version=100+int32(next()%11);
                            static const char* names[]{"prt_spark","PRT_BLOOD","water_splash","explosion_large","smoke30lit","fire_car","camflash"};
                            fileLines.push_back("FX_SYSTEM_DATA:");
                            if(version>100)fileLines.push_back("FILENAME: X:\\SA\\FxTools\\data\\effects\\gta_pc\\"+std::string(names[next()%7])+".fxs");
                            fileLines.push_back(std::string("NAME: ")+names[next()%7]);
                            fileLines.push_back("LENGTH: "+fx(0,20));
                            if(version>=106){ fileLines.push_back("LOOPINTERVALMIN: "+fx(0,10)); fileLines.push_back("LENGTH: "+fx(0,10)); }
                            fileLines.push_back("PLAYMODE: "+I(next()%300));
                            fileLines.push_back("CULLDIST: "+fx(next()%8==0 ? -20 : 0,300));
                            if(version>103){ const uint32 m=next()%5; fileLines.push_back("BOUNDINGSPHERE: "+fx(-50,50)+" "+fx(-50,50)+" "+fx(-50,50)+" "+(m==0 ? std::string("0.000") : m==1 ? fx(-10,0) : fx(0,100))); }
                            const uint32 np=next()%10==0 ? 128+next()%128 : next()%8;
                            fileLines.push_back("NUM_PRIMS: "+I(np));
                            if(np<128)for(uint32 k=0;k<np;++k){ fileLines.push_back("FX_PRIM_EMITTER_DATA:"); fileLines.push_back("FX_PRIM_BASE_DATA:"); }
                            if(version>=108)fileLines.push_back("OMITTEXTURES: "+I(next()%2));
                            if(version>=109)fileLines.push_back("TXDNAME: NOTXDSET");
                            fileLines.push_back("FX_SYSTEM_DATA:"); fileLines.push_back("106");
                            arenaPos=next()%16;
                            setState(candidate); out["switches"]=switched();
                            ret=reinterpret_cast<bool(__thiscall*)(void*,const char*,FILESTREAM,int32)>(target)(bp,"effects.fxp",reinterpret_cast<FILESTREAM>(0x7A11),version);
                            setState(false);
                            out["arena"]=Region(arena.data(),0x400); out["version"]=version; out["read"]=fileLine;
                        }
                        setState(false);
                        out["bp"]=Region(bp,0x24);
                    }
                    else if(cls=="CTaskComplexBeInGroup") {
                        auto* o=object.data();
                        static const int32 types[]{TASK_SIMPLE_PAUSE,TASK_COMPLEX_WANDER,TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_NONE};
                        for(int n=0;n<FAKES;++n){ auto* fk=fakes[n].data(); std::memset(fk,0,FAKE_SIZE); At<void*>(fk,0)=fakeVtbl.data(); At<int32>(fk,FK_TYPE)=types[next()%6];
                            fk[FK_ABORT]=uint8(next()%3!=0); At<CTask*>(fk,FK_CLONE)=next()%4 ? Fake(int(next()%FAKES)) : nullptr; }
                        for(int n=0;n<NPEDS;++n){ auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                            p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(intels[n].data(),I_TM+TM_PED)=p; }
                        // ranuras secundarias del peaton: tareas falsas o vacias; primaria 4 (= secundaria -1 en el EXE) vacia
                        for(int s=0;s<6;++s)At<CTask*>(intels[0].data(),I_TM+TM_SECONDARY+4*s)=next()%3==0 ? Fake(int(next()%FAKES)) : nullptr;
                        auto pick=[&](uint32 nullOdds){ return next()%nullOdds==0 ? nullptr : Fake(int(next()%FAKES)); };
                        mainRet=pick(3); secRet=pick(3); slotRet=int32(next()%7)-1; reportMask=next(); isTaskMask=next();
                        // GetTaskSecondarySlot devuelve -1 solo cuando no hay tarea secundaria (SetTaskSecondary(-1) aborta en la DLL)
                        if(slotRet<0)secRet=nullptr;
                        std::memset(o,0xCD,object.size());
                        const int32 gid=int32(next()%8);
                        At<uintptr_t>(o,0)=VT_BIG; At<uint32>(o,4)=0; At<CTask*>(o,8)=Fake(0); At<int32>(o,0xC)=gid; o[0x10]=uint8(next()%2);
                        At<CPed*>(o,0x14)=next()%2 ? Ped(1) : nullptr;
                        At<CTask*>(o,0x18)=next()%3==0&&mainRet ? mainRet : pick(2);
                        At<int32>(o,0x1C)=next()%2&&At<CTask*>(o,0x18) ? At<int32>(At<CTask*>(o,0x18),FK_TYPE) : types[next()%6];
                        At<CTask*>(o,0x20)=next()%3==0 ? secRet : pick(2);
                        At<int32>(o,0x24)=next()%4==0 ? slotRet : int32(next()%7)-1;
                        auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                        auto collectFresh=[&](CTask* r){
                            setState(false);
                            auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                            for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                            json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                            if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                        };
                        if(nm=="Constructor"){ const bool leader=next()%2; const int32 g=next()%6==0 ? int32(next()) : gid;
                            setState(candidate); out["switches"]=switched();
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,bool)>(target)(o,g,leader)); setState(false);
                            out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType(); }
                        else if(nm=="Destructor"){ At<CTask*>(o,8)=next()%3 ? Fake(0) : nullptr; setState(candidate); out["switches"]=switched(); reinterpret_cast<void(__thiscall*)(void*)>(target)(o); }
                        else if(nm=="Clone"){ setState(candidate); out["switches"]=switched(); auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); collectFresh(r); ret=Name(r); }
                        else if(nm=="GetTaskType"){ setState(candidate); out["switches"]=switched(); ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }
                        else if(nm=="MakeAbortable"){ const int32 prio=next()%3 ? 2 : int32(next()%4); const CEvent* ev=next()%2 ? reinterpret_cast<const CEvent*>(eventArg.data()) : nullptr;
                            setState(candidate); out["switches"]=switched();
                            ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,Ped(0),prio,ev); }
                        else if(nm=="CreateNextSubTask"){ if(next()%6==0)At<CTask*>(o,8)=nullptr; setState(candidate); out["switches"]=switched(); ret=Name(callTask(Ped(0))); }
                        else if(nm=="MonitorSecondaryGroupTask"){ setState(candidate); out["switches"]=switched(); reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,Ped(0)); }
                        else { setState(candidate); out["switches"]=switched(); ret=Name(callTask(Ped(0))); } // CreateFirstSubTask, ControlSubTask, MonitorMainGroupTask
                        setState(false);
                        out["object"]=Region(o,0x28,true);
                        json fk=json::array(); for(auto& x:fakes)fk.push_back(Region(x.data()+FK_ABORT,0x10)); out["fakes"]=fk;
                        out["task_manager"]=Region(intels[0].data()+I_TM,0x30);
                        out["refs"]=AllRefs();
                    }
                    setState(false);
                    // limpieza: tareas nuevas, referencias; el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace;
                    out["clean"]=Occupancy()==occ;
                    restoreGlobals();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreGlobals(); PoolLast()=lf0;
    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    if(restored)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
