#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/WorldWave2): ultimas funciones pendientes de CRoadBlocks (Init, GenerateRoadBlockPedsForCar)
// y de CRunningScript (GivePedScriptedTask-OG, SetCharCoordinates). Original (hook de la funcion desactivado, se ejecuta el codigo del EXE)
// frente a candidato (hook activo), llamando SIEMPRE a la direccion del EXE, sobre objetos falsos en memoria propia y con la traza de toda
// colaboracion externa (direccion del EXE y cuerpo de la DLL parcheados con el mismo stub). Los stubs solo actuan en el hilo del banco.
//
// CRoadBlocks::Init (0x461100): lee data/paths/roadblox.dat. NO se abre el fichero real: CFileMgr::OpenFile/Read/CloseFile (0x538900/0x538950/
// 0x5389D0) se doblan con un fichero en memoria (recuento + 325 CNodeAddress); las llamadas de otros hilos se reenvian a fopen/fread/fclose.
// Se compara el bloque global 0xA43438..0xA43C78 (InOrOut, NumRoadBlocks, GenerateDynamicRoadBlocks, RoadBlockNodes, aScriptRoadBlocks) y la traza.
//
// CRoadBlocks::GenerateRoadBlockPedsForCar (0x461170): vehiculo, jugador, peds, matrices, inteligencia, colmodels y tareas son buferes falsos.
// Doblados: CPed::operator new (0x5E4720), CCopPed/CCivilianPed ctor (0x5DDC60/0x5DDB70, simbolos exportados de la DLL), CTask::operator new
// (0x61A5A0), CTaskManager::SetTask (0x681AF0), CEventGroup::Add (0x4AB420), CEntity::GetColModel (0x535300), CStreaming::GetDefaultCopCarModel
// (0x407C50), CGangs::ChooseGangPedModel (0x5DE5A0), CMatrix::SetRotate (0x59B120), CWorld::ProcessVerticalLine (0x5674E0), CWorld::Add (0x563220),
// CVisibilityPlugins::SetClumpAlpha (0x732B00), CEntity::RegisterReference (0x571B70), CPed::GiveDelayedWeapon (0x5E89B0), CPed::SetCurrentWeapon
// tipo/ranura (0x5E6280/0x5E61F0), FindPlayerPed (0x56E210), rand (0x821B1E) y CGeneral::GetRandomNumber. Se ejecutan de verdad (sobre buferes falsos)
// los constructores de tareas, CEventScriptCommand, CPedPlacement::FindZCoorForPed (0x616920 / DLL) y GetWeaponInfo. Globales tocados y restaurados:
// puntero de zona (0xC0BC68), CGangs::Gang (0xC091F0), estados de carga de los modelos 285..287, ranuras 596..599 de ms_modelInfoPtrs.
//
// CRunningScript::GivePedScriptedTask-OG (0x465C20): script falso (0x400 bytes), pool de peds falso (puntero 0xB74490 sustituido), tareas falsas con
// vtable trazada, grupo falso. Doblados: CPedGroups::GetPedsGroup (0x5F7E80), CPed::IsPlayer (0x5DF8F0), CScriptedBrainTaskStore::SetTask (0x635720),
// CPedScriptedTaskRecord::GetVacantSlot (0x608540), CPedScriptedTaskRecordData::Set/SetAsGroupTask/SetAsAttractorScriptTask (0x608390/0x6084A0/0x608440),
// CPedGroupIntelligence::SetScriptCommandTask/GetTaskScriptCommand (0x5F8560/0x5F8690), CTaskComplexSequence::AddTask (0x632D10), CEventGroup::Add (0x4AB420).
// CEventScriptCommand se construye y destruye de verdad.
//
// CRunningScript::SetCharCoordinates (0x464DC0): ped, vehiculo y grupo falsos; doblados CWorld::FindGroundZForCoord (0x569660), CEntity::GetDistance
// FromCentreOfMassToBaseOfModel (0x536BE0), CTheScripts::ClearSpaceForMissionEntity (0x486B00), CPedGroups::GetPedsGroup (0x5F7E80), CPedGroupMembership::
// IsLeader (0x5F69C0), CPedGroup::Teleport (0x5F7AD0) y la virtual Teleport (ranura 14) de la vtable falsa.
//
// Rellenos: la comparacion usa hash de los campos reales; las tareas se comparan sin su vptr (el EXE pone el vptr del EXE y la DLL el suyo).
// No se asigna memoria de pools reales; todo el estado global tocado se guarda y se restaura (state_restored).
#include <fstream>
#include <set>
#include <cmath>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "RoadBlocks.h"
#include "FileMgr.h"
#include "Scripts/RunningScript.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskComplexWanderCop.h"
#include "Tasks/TaskTypes/TaskComplexKillPedOnFoot.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGroup.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Ped/CivilianPed.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedIntelligence.h"
#include "PedPlacement.h"
#include "Gangs.h"
#include "GangInfo.h"
#include "PopCycle.h"
#include "ZoneInfo.h"
#include "WeaponInfo.h"
#include "StreamingInfo.h"
#include "VisibilityPlugins.h"
#include "Collision/ColModel.h"
#include "Models/BaseModelInfo.h"
#include "../ProbeRuntime.hpp"

namespace RoadScriptWave3Live {
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

// ======================= estado compartido con los dobles =======================
inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline float RandF(Rng& r){ const uint32 k=r.below(100); if(k<6)return 0.f; if(k<10)return r.pick({1.f,-1.f,0.5f,90.f,89.99f,30.f,-30.f,1e-30f}); return r.uni(-300.f,300.f); }
inline void FillFloats(void* p,size_t from,size_t to,Rng& r){ for(size_t o=from;o+4<=to;o+=4)At<float>(p,o)=RandF(r); }

inline uint32 Hh(uint32 a,uint32 b=0){ uint32 x=(a+0x9E3779B9u)*2654435761u; x^=(b+0x7F4A7C15u)*40503u; x^=x>>15; x*=2246822519u; x^=x>>13; x*=3266489917u; x^=x>>16; return x; }
inline std::array<void*,80> entVt{};                 // vtable falsa de entidades (0 = destructor con borrado, 14 = Teleport)
inline std::array<void*,8> taskVt{};                 // vtable falsa de tareas (0 = destructor con borrado)
inline std::array<bool,8> script{};                  // resultados programados de los dobles
inline uint32 scN{};
inline Rng rr{1};                                    // generador de rand del caso
inline Rng pr{1};                                    // generador de los campos de los peds falsos
inline bool stealTask{}, evAddNull{};
inline void* grpPtr{};                               // grupo devuelto por GetPedsGroup
inline int32 vacantVal{};

// --- CRoadBlocks::GenerateRoadBlockPedsForCar ---
constexpr size_t kBig=0x800, kTaskSlot=0x60, kTaskSlots=6;
alignas(16) inline std::array<uint8,kBig> playerBuf{}, vehBuf{};
alignas(16) inline std::array<uint8,0x60> vehMat{}, vehCol{}, copModelBuf{}, copCol{};
alignas(16) inline std::array<std::array<uint8,kBig>,2> newPeds{};
alignas(16) inline std::array<std::array<uint8,0x60>,2> pedMats{};
alignas(16) inline std::array<std::array<uint8,0x400>,2> intelBuf{};
alignas(16) inline std::array<uint8,0x40> clumpBuf{};
alignas(16) inline std::array<uint8,kTaskSlot*kTaskSlots> taskArena{};
alignas(16) inline std::array<uint8,0x100> zoneBuf{};
alignas(16) inline std::array<uint8,0x40> evRetBuf{};
inline uint32 pedNewCount{}, taskNewCount{}, copModelIdx{};
inline bool pedHasMat[2]{};
inline std::array<int32,10> gangModel{};

// --- CRunningScript::GivePedScriptedTask-OG / SetCharCoordinates ---
constexpr size_t kPedStride=0x7C4, kPedSlots=6;
alignas(16) inline std::array<uint8,kPedSlots*kPedStride> gpStore{};
inline std::array<uint8,kPedSlots> gpFlags{};
alignas(16) inline std::array<uint8,0x14> gpPool{};
alignas(16) inline std::array<uint8,0x400> scrBuf{};
alignas(16) inline std::array<uint8,0x400> gpIntel{};
alignas(16) inline std::array<uint8,0x100> grpBuf{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> fTask{};
alignas(16) inline std::array<uint8,kBig> scPed{}, scVeh{};

// --- CRoadBlocks::Init ---
alignas(16) inline std::array<uint8,4+325*4> fileData{};
alignas(16) inline std::array<uint8,0x20> fileFake{};
inline size_t filePos{}; inline uint32 fileOpens{}, fileCloses{};

// ======================= dobles =======================
inline void __fastcall V_Dtor(void* self,void*,uint32 flags){ if(Mine())T("dtor",self,flags); }
inline void __fastcall V_Trap(void* self,void*){ if(Mine())T("virtual_inesperada",self); }
inline void __fastcall V_Teleport(void* self,void*,float x,float y,float z,uint32 f){ if(Mine())T("teleport",self,x,y,z,f&0xFF); }
inline void __fastcall S_Reg(void* self,void*,void** ref){ if(Mine())T("register_reference",self,ref); }
inline void __cdecl S_WorldAdd(void* e){ if(Mine())T("world_add",e); }
inline int __cdecl S_RandExe(){ if(!Mine())return 0; const int v=int(rr.next()&0x7FFF); T("rand",v); return v; }
inline uint16 __cdecl S_RandDll(){ if(!Mine())return 0; const uint16 v=uint16(rr.next()&0x7FFF); T("rand",int(v)); return v; }
inline void* __cdecl S_Player(int32){ return playerBuf.data(); }
// CFileMgr (Init): fichero en memoria; otros hilos usan el CRT real
inline void* __cdecl S_FOpen(const char* path,const char* mode){
    if(!Mine())return fopen(path,mode);
    trace.push_back(json::array({"fopen",std::string(path),std::string(mode)})); ++fileOpens; filePos=0; return fileFake.data(); }
inline size_t __cdecl S_FRead(void* file,void* buf,size_t size){
    if(!Mine()||file!=fileFake.data())return fread(buf,1,size,static_cast<FILE*>(file));
    T("fread",file==static_cast<void*>(fileFake.data()),uint32(size));
    const size_t n=std::min(size,fileData.size()-filePos); std::memcpy(buf,fileData.data()+filePos,n); filePos+=n; return n; }
inline int __cdecl S_FClose(void* file){
    if(!Mine()||file!=fileFake.data())return fclose(static_cast<FILE*>(file));
    T("fclose",file==static_cast<void*>(fileFake.data())); ++fileCloses; return 0; }
// GenerateRoadBlockPedsForCar
inline void InitFakePed(void* p){
    const uint32 k= p==newPeds[0].data() ? 0u : 1u;
    At<void*>(p,0)=entVt.data();
    for(size_t o=4;o<0x14;o+=4)At<float>(p,o)=RandF(pr);
    At<void*>(p,0x14)= pedHasMat[k] ? pedMats[k].data() : nullptr;
    At<void*>(p,0x18)=clumpBuf.data();
    At<uint32>(p,0x46C)=pr.next(); At<uint32>(p,0x470)=pr.next();
    At<void*>(p,0x47C)=intelBuf[k].data();
    At<uint32>(p,0x54C)=pr.next(); }
inline void* __cdecl S_PedNew(unsigned n){ if(!Mine())return nullptr; T("ped_new",uint32(n)); return newPeds[pedNewCount++ ? 1 : 0].data(); }
inline void* __fastcall S_CopCtor(void* self,void*,uint32 type){ if(Mine()){ T("cop_ctor",self,type); InitFakePed(self); } return self; }
inline void* __fastcall S_CivCtor(void* self,void*,uint32 pedType,uint32 model){ if(Mine()){ T("civilian_ctor",self,pedType,model); InitFakePed(self); } return self; }
inline void* __cdecl S_TaskNew(size_t n){ if(!Mine())return nullptr; T("task_new",uint32(n)); const size_t k= taskNewCount<kTaskSlots-1 ? taskNewCount : kTaskSlots-1; ++taskNewCount; return taskArena.data()+kTaskSlot*k; }
inline void __fastcall S_SetTask(void* self,void*,void* task,int32 slot,uint32 unused){ if(Mine())T("set_task",self,task,slot,unused&0xFF); }
inline CEvent* __fastcall S_EvAdd(void* self,void*,CEventScriptCommand* ev,uint32 valid){
    if(!Mine())return nullptr;
    T("event_add",self,ev->m_primaryTaskIndex,ev->m_task,uint32(ev->m_affectsDeadPeds),valid&0xFF);
    if(stealTask)ev->m_task=nullptr;   // el EXE/DLL real clonarian el evento; aqui se evita que el destructor libere una tarea del arena
    return evAddNull ? nullptr : reinterpret_cast<CEvent*>(evRetBuf.data()); }
inline void* __fastcall S_GetCol(void* self,void*){ if(!Mine())return nullptr; T("get_col_model",self); return vehCol.data(); }
inline int __cdecl S_CopCar(uint32 f){ if(Mine())T("default_cop_car",f&0xFF); return int(596u+copModelIdx); }
inline int __cdecl S_GangModel(int32 g){ if(Mine())T("choose_gang_ped_model",g); return gangModel[uint32(g)%10u]; }
inline void __fastcall S_SetRot(void* m,void*,float x,float y,float z){
    if(!Mine())return; T("set_rotate",m,x,y,z);
    const float c=0.25f+z*0.125f;
    At<float>(m,0)=c; At<float>(m,4)=x+0.5f; At<float>(m,8)=-c; At<float>(m,0x10)=y-0.5f; At<float>(m,0x14)=c*2.f; At<float>(m,0x18)=0.75f;
    At<float>(m,0x20)=-1.f; At<float>(m,0x24)=z; At<float>(m,0x28)=0.5f; At<float>(m,0x30)=0.f; At<float>(m,0x34)=0.f; At<float>(m,0x38)=0.f; }
inline bool __cdecl S_Pvl(const CVector* o,float dist,void* cp,void** ent,uint32 b0,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,void* poly){
    (void)ent; if(!Mine())return false;
    const bool res=script[scN++&7];
    T("process_vertical_line",o->x,o->y,o->z,dist,b0&0xFF,b1&0xFF,b2&0xFF,b3&0xFF,b4&0xFF,b5&0xFF,poly,res);
    if(res)At<float>(cp,8)=o->z-5.f+float(scN);
    return res; }
inline void __cdecl S_Alpha(void* clump,int32 a){ if(Mine())T("clump_alpha",clump,a); }
inline void __fastcall S_Give(void* self,void*,int32 w,uint32 ammo){ if(Mine())T("give_delayed_weapon",self,w,ammo); }
inline void __fastcall S_SetWT(void* self,void*,int32 w){ if(Mine())T("set_current_weapon_type",self,w); }
inline void __fastcall S_SetWS(void* self,void*,int32 slot){ if(Mine())T("set_current_weapon_slot",self,slot); }
// SetCharCoordinates
inline float __cdecl S_GroundZ(float x,float y){ if(Mine())T("find_ground_z",x,y); return -50.f+float(Hh(std::bit_cast<uint32>(x),std::bit_cast<uint32>(y))%20000u)*0.01f; }
inline float __fastcall S_Dist(void* self,void*){ if(Mine())T("dist_com_to_base",self); return 0.25f+float(Hh(self==scVeh.data() ? 1u : 2u,7u)%300u)*0.01f; }
inline void __cdecl S_Clear(const CVector* p,void* e){ if(Mine())T("clear_space",p->x,p->y,p->z,e); }
inline void* __cdecl S_GetGroup(void* ped){ if(!Mine())return nullptr; T("get_peds_group",ped); return grpPtr; }
inline bool __fastcall S_IsLeader(void* self,void*,void* ped){ if(!Mine())return false; const bool r=script[scN++&7]; T("is_leader",self,ped,r); return r; }
inline void __fastcall S_GrpTele(void* self,void*,const CVector* p){ if(Mine())T("group_teleport",self,p->x,p->y,p->z); }
// GivePedScriptedTask
inline bool __fastcall S_IsPlayer(void* self,void*){ if(!Mine())return false; const bool r=script[scN++&7]; T("is_player",self,r); return r; }
inline void* __cdecl S_Brain(void* ped,void* task){ if(!Mine())return nullptr; void* r= script[scN++&7] ? task : nullptr; T("brain_set_task",ped,task,r); return r; }
inline int __cdecl S_Vacant(){ if(Mine())T("vacant_slot"); return vacantVal; }
inline void __fastcall S_RecEv(void* self,void*,void* ped,int32 op,void* ev){ if(Mine())T("record_set_event",self,ped,op,ev); }
inline void __fastcall S_RecGroup(void* self,void*,void* ped,int32 op,void* task){ if(Mine())T("record_set_group",self,ped,op,task); }
inline void __fastcall S_RecAttr(void* self,void*,void* ped,int32 op,void* task){ if(Mine())T("record_set_attractor",self,ped,op,task); }
inline void __fastcall S_SetCmd(void* self,void*,void* ped,void* task){ if(Mine())T("group_set_script_command",self,ped,task); }
inline void* __fastcall S_GetCmd(void* self,void*,void* ped){ if(!Mine())return nullptr; void* r= script[scN++&7] ? fTask[2].data() : nullptr; T("group_get_script_command",self,ped,r); return r; }
inline bool __fastcall S_SeqAdd(void* self,void*,void* task){ if(Mine())T("sequence_add_task",self,task); return true; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,4> fns{{
    {"CRoadBlocks","Init",0x461100,0,-1,128},
    {"CRoadBlocks","GenerateRoadBlockPedsForCar",0x461170,0,-1,1024},
    {"CRunningScript","GivePedScriptedTask-OG",0x465C20,0,-1,1024},
    {"CRunningScript","SetCharCoordinates",0x464DC0,0,-1,1024},
}};

// ======================= fixtures =======================
constexpr uintptr_t kRbBase=0xA43438; constexpr size_t kRbSize=0xA43C78-0xA43438;
inline uint64 H(const void* p,size_t n){ return Fnv(p,n); }

// ---- CRoadBlocks::Init ----
inline json CaseInit(Rng& r){
    FillBytes(reinterpret_cast<void*>(kRbBase),kRbSize,r);
    const int32 n=r.pickI({0,1,2,17,100,300,324,325,int32(r.below(326))});
    FillBytes(fileData.data(),fileData.size(),r); At<int32>(fileData.data(),0)=n;
    fileOpens=fileCloses=0; filePos=0;
    reinterpret_cast<void(__cdecl*)()>(0x461100)();
    json o=json::object(); o["rb"]=H(reinterpret_cast<void*>(kRbBase),kRbSize); o["pos"]=uint32(filePos); o["opens"]=fileOpens; o["closes"]=fileCloses; o["n"]=n;
    return o; }

// ---- CRoadBlocks::GenerateRoadBlockPedsForCar ----
inline uint64 TaskHash(){ uint64 h=1469598103934665603ull; for(size_t k=0;k<kTaskSlots;++k)h=Fnv(taskArena.data()+k*kTaskSlot+4,kTaskSlot-4,h); return h; }
inline json CasePeds(Rng& r){
    FillBytes(newPeds[0].data(),kBig,r); FillBytes(newPeds[1].data(),kBig,r);
    std::memset(newPeds[0].data(),0xCD,kBig); std::memset(newPeds[1].data(),0xCD,kBig);
    std::memset(taskArena.data(),0xCD,taskArena.size());
    for(int k=0;k<2;++k){ FillBytes(pedMats[size_t(k)].data(),0x60,r); FillFloats(pedMats[size_t(k)].data(),0,0x60,r); FillBytes(intelBuf[size_t(k)].data(),0x400,r); pedHasMat[k]=r.pct(60); }
    FillBytes(clumpBuf.data(),clumpBuf.size(),r); clumpBuf[0]=2;                         // rpCLUMP
    FillBytes(playerBuf.data(),kBig,r); FillBytes(vehBuf.data(),kBig,r);
    FillBytes(vehMat.data(),0x60,r); FillFloats(vehMat.data(),0,0x60,r);
    for(size_t o=0;o<0x30;o+=4)if(o!=0xC)At<float>(vehMat.data(),o)=r.uni(-1.f,1.f);
    for(size_t o=0x30;o<0x3C;o+=4)At<float>(vehMat.data(),o)=r.uni(-500.f,500.f);
    const int32 models[]{427,433,490,599,596,597,402,0,595,520};
    At<void*>(vehBuf.data(),0)=entVt.data(); At<void*>(vehBuf.data(),0x14)=vehMat.data();
    At<int16>(vehBuf.data(),0x22)=int16(r.pct(70) ? models[r.below(4)] : models[r.below(10)]);
    At<uint8>(vehBuf.data(),0x4AE)=uint8(r.pickI({2,2,1,1,0,2}));
    FillBytes(vehCol.data(),0x60,r); At<float>(vehCol.data(),0x24)=r.pick({3.f,2.5f,5.f,4.2f,1.f,2.75f});
    FillBytes(copCol.data(),0x60,r); At<float>(copCol.data(),0x24)=r.pick({2.5f,3.f,2.4f,3.6f,2.75f});
    FillBytes(copModelBuf.data(),0x60,r); At<void*>(copModelBuf.data(),0x14)=copCol.data();
    copModelIdx=r.below(4);
    AtA<uint32>(0xA9B0C8+4*(596+copModelIdx))=uint32(reinterpret_cast<uintptr_t>(copModelBuf.data()));
    FillBytes(zoneBuf.data(),zoneBuf.size(),r); for(int i=0;i<10;++i)zoneBuf[size_t(i)]= r.pct(35) ? uint8(0) : uint8(1+r.below(200));
    AtA<uint32>(0xC0BC68)=uint32(reinterpret_cast<uintptr_t>(zoneBuf.data()));
    const int32 weapons[]{0,0,5,22,23,24,25,27,28,29,30,32,33,2};
    for(int g=0;g<10;++g){ auto* gi=reinterpret_cast<uint8*>(0xC091F0)+g*16; for(int k=0;k<3;++k)At<int32>(gi,4+4*size_t(k))=weapons[r.below(14)];
        gangModel[size_t(g)]= r.pct(40) ? -1 : int32(105+g+r.below(3)); }
    for(uint32 m=285;m<=287;++m)AtA<uint8>(0x8E4CD0+20*m)=uint8(r.pickI({1,1,1,0,2,3,4}));
    pedNewCount=taskNewCount=0; stealTask=true; evAddNull=r.pct(10);
    for(auto& b:script)b=r.pct(50); scN=0;
    rr=Rng{r.next()|1u}; pr=Rng{r.next()|1u};
    const bool cop=r.pct(55);
    const int32 pedType= cop ? int32(PED_TYPE_COP) : int32(PED_TYPE_GANG1)+int32(r.below(10));
    const int32 pos=int32(r.below(3));
    reinterpret_cast<void(__cdecl*)(void*,int32,int32)>(0x461170)(vehBuf.data(),pos,pedType);
    json o=json::object();
    o["peds"]=json::array({H(newPeds[0].data(),kBig),H(newPeds[1].data(),kBig)}); o["mats"]=json::array({H(pedMats[0].data(),0x60),H(pedMats[1].data(),0x60)});
    o["intel"]=json::array({H(intelBuf[0].data(),0x400),H(intelBuf[1].data(),0x400)});
    o["tasks"]=TaskHash(); o["task_n"]=taskNewCount; o["ped_n"]=pedNewCount; o["veh"]=H(vehBuf.data(),kBig); o["player"]=H(playerBuf.data(),kBig);
    o["vehmat"]=H(vehMat.data(),0x60); o["clump"]=H(clumpBuf.data(),clumpBuf.size()); o["rand_state"]=rr.s; o["pr_state"]=pr.s; o["sc"]=scN;
    return o; }

// ---- CRunningScript::GivePedScriptedTask-OG ----
inline json CaseGive(Rng& r){
    FillBytes(gpStore.data(),gpStore.size(),r); FillBytes(gpIntel.data(),gpIntel.size(),r); FillBytes(scrBuf.data(),scrBuf.size(),r); FillBytes(grpBuf.data(),grpBuf.size(),r);
    for(size_t s=0;s<kPedSlots;++s){ auto* p=gpStore.data()+s*kPedStride; At<void*>(p,0)=entVt.data(); At<void*>(p,0x47C)=gpIntel.data();
        uint32& f=At<uint32>(p,0x474); f= r.pct(40) ? (f|0x800000u) : (f&~0x800000u);
        gpFlags[s]=uint8(r.pct(80) ? r.below(128) : (0x80|r.below(128))); }
    gpFlags[0]=uint8(r.below(128));
    for(auto& t:fTask){ FillBytes(t.data(),t.size(),r); At<void*>(t.data(),0)=taskVt.data(); }
    std::memset(gpPool.data(),0,gpPool.size());
    At<void*>(gpPool.data(),0)=gpStore.data(); At<void*>(gpPool.data(),4)=gpFlags.data(); At<int32>(gpPool.data(),8)=int32(kPedSlots); At<int32>(gpPool.data(),0xC)=-1;
    AtA<uint32>(0xB74490)=uint32(reinterpret_cast<uintptr_t>(gpPool.data()));
    // ped principal: ranura usada
    std::array<uint32,kPedSlots> used{}; uint32 nu=0; for(uint32 s=0;s<kPedSlots;++s)if(!(gpFlags[s]&0x80))used[nu++]=s;
    const uint32 s0=used[r.below(nu)];
    const int32 handle= r.pct(12) ? -1 : int32((s0<<8)|gpFlags[s0]);
    // variable local 0 (otherPed): el mismo ped, otro, obsoleto o fuera de uso
    const uint32 sk=r.below(kPedSlots);
    int32 lv;
    switch(r.below(5)){ case 0: lv=int32((s0<<8)|gpFlags[s0]); break; case 1: lv=int32((sk<<8)|gpFlags[sk]); break; case 2: lv=int32((sk<<8)|((gpFlags[sk]+1u)&0xFF)); break;
        case 3: lv=int32((sk<<8)|r.below(256)); break; default: lv=int32((r.below(kPedSlots)<<8)|r.below(256)); }
    const int8 ext=int8(r.pickI({5,5,2,0,3,1,4,7,-1,5}));
    At<int8>(scrBuf.data(),0xC9)=ext; At<uint8>(scrBuf.data(),0xDC)=uint8(r.pct(35));
    At<int32>(scrBuf.data(),0x3C)=At<uint8>(scrBuf.data(),0xDC) ? int32((r.below(kPedSlots)<<8)|r.below(256)) : lv;
    AtA<int32>(0xA48960)=At<uint8>(scrBuf.data(),0xDC) ? lv : int32((r.below(kPedSlots)<<8)|r.below(256));
    AtA<int32>(0x8D2E98)=int32(r.below(4));
    grpPtr= r.pct(50) ? static_cast<void*>(grpBuf.data()) : nullptr;
    evAddNull=r.pct(15); stealTask=false; vacantVal=int32(r.below(8));
    for(auto& b:script)b=r.pct(50); scN=0;
    CTask* task=reinterpret_cast<CTask*>(fTask[0].data());
    const int32 op=r.pickI({0x605,0x812,0xA1A,0,1,-5,int32(r.next())});
    reinterpret_cast<void(__thiscall*)(void*,int32,void*,int32)>(0x465C20)(scrBuf.data(),handle,task,op);
    json o=json::object();
    o["store"]=H(gpStore.data(),gpStore.size()); o["intel"]=H(gpIntel.data(),gpIntel.size()); o["script"]=H(scrBuf.data(),scrBuf.size()); o["grp"]=H(grpBuf.data(),grpBuf.size());
    o["ft0"]=H(fTask[0].data(),0x40); o["ft1"]=H(fTask[1].data(),0x40); o["ft2"]=H(fTask[2].data(),0x40); o["evret"]=H(evRetBuf.data(),evRetBuf.size()); o["sc"]=scN;
    return o; }

// ---- CRunningScript::SetCharCoordinates ----
inline json CaseSetChar(Rng& r){
    FillBytes(scPed.data(),kBig,r); FillBytes(scVeh.data(),kBig,r); FillBytes(grpBuf.data(),grpBuf.size(),r); FillBytes(scrBuf.data(),scrBuf.size(),r);
    At<void*>(scPed.data(),0)=entVt.data(); At<void*>(scVeh.data(),0)=entVt.data();
    const bool inVeh=r.pct(45);
    uint32& fl=At<uint32>(scPed.data(),0x46C); fl= inVeh ? (fl|0x100u) : (fl&~0x100u);
    At<void*>(scPed.data(),0x58C)= (inVeh||r.pct(30)) ? static_cast<void*>(scVeh.data()) : nullptr;
    grpPtr= r.pct(60) ? static_cast<void*>(grpBuf.data()) : nullptr;
    for(auto& b:script)b=r.pct(50); scN=0;
    float x=r.pct(15) ? r.pick({0.f,-0.f,1.f,-1.f,1e10f,-1e10f,1e-30f}) : r.uni(-3000.f,3000.f);
    float y=r.pct(15) ? r.pick({0.f,-0.f,2.f,-2.f,1e10f,-1e10f,1e-30f}) : r.uni(-3000.f,3000.f);
    float z;
    switch(r.below(10)){ case 0: z=-100.f; break; case 1: z=std::bit_cast<float>(0xC2C80001u); break; case 2: z=std::bit_cast<float>(0xC2C7FFFFu); break; case 3: z=-0.f; break;
        case 4: z=std::bit_cast<float>(0x7FC00000u); break; case 5: z=r.uni(-500.f,-100.f); break; case 6: z=-1e30f; break; case 7: z=0.f; break; default: z=r.uni(-99.f,500.f); }
    const uint32 warp=r.below(2), off=r.below(2);
    reinterpret_cast<void(__thiscall*)(void*,void*,float,float,float,uint32,uint32)>(0x464DC0)(scrBuf.data(),scPed.data(),x,y,z,warp,off);
    json o=json::object(); o["ped"]=H(scPed.data(),kBig); o["veh"]=H(scVeh.data(),kBig); o["grp"]=H(grpBuf.data(),grpBuf.size()); o["sc"]=scN;
    return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"roadscriptwave3-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"roadscriptwave3-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("roadscriptwave3");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"roadscriptwave3-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,4> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ---
    struct B { const char* n; uintptr_t exe; uintptr_t dll; };
    std::vector<B> bodies;
    auto AddB=[&](const char* n,uintptr_t exeAddr,uintptr_t dll){ bodies.push_back({n,exeAddr,dll}); };
    AddB("fopen",0x538900,Body(&CFileMgr::OpenFile));
    AddB("fread",0x538950,Body(&CFileMgr::Read));
    AddB("fclose",0x5389D0,Body(&CFileMgr::CloseFile));
    AddB("reg",0x571B70,Body(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)));
    AddB("world_add",0x563220,Body(&CWorld::Add));
    AddB("rand",0x821B1E,Body(&CGeneral::GetRandomNumber));
    AddB("player",0x56E210,Body(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)));
    AddB("ped_new",0x5E4720,Body(static_cast<void*(*)(unsigned)>(&CPed::operator new)));
    AddB("cop_ctor",0x5DDC60,Exported("??0CCopPed@@QAE@I@Z"));
    AddB("civ_ctor",0x5DDB70,Exported("??0CCivilianPed@@QAE@W4ePedType@@I@Z"));
    AddB("task_new",0x61A5A0,Body(static_cast<void*(*)(size_t)>(&CTask::operator new)));
    AddB("set_task",0x681AF0,Body(&CTaskManager::SetTask));
    AddB("event_add",0x4AB420,Body(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)));
    AddB("get_col",0x535300,Body(&CEntity::GetColModel));
    AddB("cop_car",0x407C50,Body(&CStreaming::GetDefaultCopCarModel));
    AddB("gang_model",0x5DE5A0,Body(&CGangs::ChooseGangPedModel));
    AddB("set_rotate",0x59B120,Body(static_cast<void(CMatrix::*)(float,float,float)>(&CMatrix::SetRotate)));
    AddB("pvl",0x5674E0,Body(static_cast<bool(*)(const CVector&,float,CColPoint&,CEntity*&,bool,bool,bool,bool,bool,bool,CStoredCollPoly*)>(&CWorld::ProcessVerticalLine)));
    AddB("alpha",0x732B00,Body(&CVisibilityPlugins::SetClumpAlpha));
    AddB("give",0x5E89B0,Body(&CPed::GiveDelayedWeapon));
    AddB("set_wt",0x5E6280,Body(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)));
    AddB("set_ws",0x5E61F0,Body(static_cast<void(CPed::*)(int32)>(&CPed::SetCurrentWeapon)));
    AddB("ground_z",0x569660,Body(&CWorld::FindGroundZForCoord));
    AddB("dist",0x536BE0,Body(&CEntity::GetDistanceFromCentreOfMassToBaseOfModel));
    AddB("clear_space",0x486B00,Body(&CTheScripts::ClearSpaceForMissionEntity));
    AddB("get_group",0x5F7E80,Body(&CPedGroups::GetPedsGroup));
    AddB("is_leader",0x5F69C0,Body(&CPedGroupMembership::IsLeader));
    AddB("grp_teleport",0x5F7AD0,Body(&CPedGroup::Teleport));
    AddB("is_player",0x5DF8F0,Body(&CPed::IsPlayer));
    AddB("brain",0x635720,Body(&CScriptedBrainTaskStore::SetTask));
    AddB("vacant",0x608540,Body(&CPedScriptedTaskRecord::GetVacantSlot));
    AddB("rec_ev",0x608390,Body(static_cast<void(CPedScriptedTaskRecordData::*)(CPed*,int32,CEventScriptCommand*)>(&CPedScriptedTaskRecordData::Set)));
    AddB("rec_group",0x6084A0,Body(&CPedScriptedTaskRecordData::SetAsGroupTask));
    AddB("rec_attr",0x608440,Body(&CPedScriptedTaskRecordData::SetAsAttractorScriptTask));
    AddB("set_cmd",0x5F8560,Body(&CPedGroupIntelligence::SetScriptCommandTask));
    AddB("get_cmd",0x5F8690,Body(&CPedGroupIntelligence::GetTaskScriptCommand));
    AddB("seq_add",0x632D10,Body(static_cast<bool(CTaskComplexSequence::*)(CTask*)>(&CTaskComplexSequence::AddTask)));
    {   std::set<uintptr_t> all; bool bad=false; json miss=json::array();
        for(auto& b:bodies){ if(!b.dll){bad=true;miss.push_back(b.n);} else if(!all.insert(b.dll).second){bad=true;miss.push_back(std::string(b.n)+"(repetido)");} }
        if(bad){ Write({{"event","error"},{"reason","DLL body not found or overlapping"},{"which",miss}}); return; } }
    auto D=[&](const char* n)->uintptr_t{ for(auto& b:bodies)if(std::string(b.n)==n)return b.dll; return 0; };

    std::array<bool,4> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables falsas y nombres de buferes
    for(auto& v:entVt)v=reinterpret_cast<void*>(&V_Trap); entVt[0]=reinterpret_cast<void*>(&V_Dtor); entVt[14]=reinterpret_cast<void*>(&V_Teleport);
    for(auto& v:taskVt)v=reinterpret_cast<void*>(&V_Trap); taskVt[0]=reinterpret_cast<void*>(&V_Dtor);
    if(named.empty()){
        Nm(playerBuf.data(),kBig,"player"); Nm(vehBuf.data(),kBig,"vehicle"); Nm(vehMat.data(),0x60,"vehmat"); Nm(vehCol.data(),0x60,"vehcol");
        Nm(clumpBuf.data(),clumpBuf.size(),"clump"); Nm(evRetBuf.data(),evRetBuf.size(),"evret"); Nm(fileFake.data(),fileFake.size(),"file");
        for(size_t i=0;i<2;++i){ static const char* np[2]{"newped0","newped1"}; static const char* nm[2]{"pedmat0","pedmat1"}; static const char* ni[2]{"intel0","intel1"};
            Nm(newPeds[i].data(),kBig,np[i]); Nm(pedMats[i].data(),0x60,nm[i]); Nm(intelBuf[i].data(),0x400,ni[i]); }
        for(size_t i=0;i<kTaskSlots;++i){ static const char* nt[6]{"task0","task1","task2","task3","task4","task5"}; Nm(taskArena.data()+i*kTaskSlot,kTaskSlot,nt[i]); }
        for(size_t i=0;i<kPedSlots;++i){ static const char* ng[6]{"gped0","gped1","gped2","gped3","gped4","gped5"}; Nm(gpStore.data()+i*kPedStride,kPedStride,ng[i]); }
        for(size_t i=0;i<3;++i){ static const char* nf[3]{"ftask0","ftask1","ftask2"}; Nm(fTask[i].data(),0x40,nf[i]); }
        Nm(gpIntel.data(),gpIntel.size(),"gintel"); Nm(scrBuf.data(),scrBuf.size(),"script"); Nm(grpBuf.data(),grpBuf.size(),"group");
        Nm(scPed.data(),kBig,"scped"); Nm(scVeh.data(),kBig,"scveh");
        Nm(reinterpret_cast<void*>(0xC0B1E8),128*0x14,"records"); Nm(reinterpret_cast<void*>(0xC178F0),64*0x40,"sequences"); }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      alignas(16) static std::array<uint8,0x800> cb{};
      auto Zero=[&]{ std::memset(cb.data(),0,cb.size()); };
      auto OffOf=[&](const void* m){ return uint32(static_cast<const uint8*>(m)-cb.data()); };
      auto* P=reinterpret_cast<CPed*>(cb.data());
      Zero(); P->bInVehicle=true; Chk("CPed.bInVehicle",At<uint32>(cb.data(),0x46C),0x100);
      Zero(); P->bStayInSamePlace=true; Chk("CPed.bStayInSamePlace",At<uint32>(cb.data(),0x46C),0x200000);
      Zero(); P->bNotAllowedToDuck=true; Chk("CPed.bNotAllowedToDuck",At<uint32>(cb.data(),0x46C),0x1000000);
      Zero(); P->bCrouchWhenShooting=true; Chk("CPed.bCrouchWhenShooting",At<uint32>(cb.data(),0x46C),0x2000000);
      Zero(); P->bCullExtraFarAway=true; Chk("CPed.bCullExtraFarAway",At<uint32>(cb.data(),0x470),0x40);
      Zero(); P->bHasAScriptBrain=true; Chk("CPed.bHasAScriptBrain",At<uint32>(cb.data(),0x474),0x800000);
      Chk("CPed.m_pVehicle",OffOf(&P->m_pVehicle),0x58C); Chk("CPed.m_nTimeTillWeNeedThisPed",OffOf(&P->m_nTimeTillWeNeedThisPed),0x54C);
      Chk("CPed.m_matrix",OffOf(&P->m_matrix),0x14); Chk("CPed.m_placement.heading",OffOf(&P->m_placement.m_fHeading),0x10);
      Chk("CCopPed.m_bDontPursuit",OffOf(&reinterpret_cast<CCopPed*>(cb.data())->m_bDontPursuit),0x79C);
      Chk("CVehicle.m_nNumPedsForRoadBlock",OffOf(&reinterpret_cast<CVehicle*>(cb.data())->m_nNumPedsForRoadBlock),0x4AE);
      Chk("CColModel.radius",OffOf(&reinterpret_cast<CColModel*>(cb.data())->GetBoundingSphere().m_fRadius),0x24);
      Chk("CColPoint.point.z",OffOf(&reinterpret_cast<CColPoint*>(cb.data())->m_vecPoint.z),8);
      Zero(); At<void*>(cb.data(),0x14)=cb.data()+0x100; Chk("CBaseModelInfo.m_pColModel",uint32(reinterpret_cast<CBaseModelInfo*>(cb.data())->GetColModel()==reinterpret_cast<CColModel*>(cb.data()+0x100)),1);
      Chk("CZoneInfo.GangStrength",OffOf(&reinterpret_cast<CZoneInfo*>(cb.data())->GangStrength[0]),0);
      Chk("sizeof(CGangInfo)",uint32(sizeof(CGangInfo)),16); Chk("CGangInfo.weapons",OffOf(reinterpret_cast<CGangInfo*>(cb.data())->m_nGangWeapons.data()),4);
      Chk("CGangs::Gang",uint32(reinterpret_cast<uintptr_t>(&CGangs::Gang.data()[0])),0xC091F0);
      Chk("CStreamingInfo.m_LoadState",uint32(reinterpret_cast<uintptr_t>(&CStreaming::ms_aInfoForModel[0].m_LoadState)),0x8E4CD0);
      Chk("sizeof(CStreamingInfo)",uint32(sizeof(CStreamingInfo)),20);
      Chk("CRunningScript.m_LocalVars",OffOf(&reinterpret_cast<CRunningScript*>(cb.data())->m_LocalVars),0x3C);
      Chk("CRunningScript.m_ExternalType",OffOf(&reinterpret_cast<CRunningScript*>(cb.data())->m_ExternalType),0xC9);
      Chk("CRunningScript.m_ThisMustBeTheOnlyMissionRunning",OffOf(&reinterpret_cast<CRunningScript*>(cb.data())->m_ThisMustBeTheOnlyMissionRunning),0xDC);
      Chk("sizeof(CTaskComplexSequence)",uint32(sizeof(CTaskComplexSequence)),0x40); Chk("sizeof(CPedScriptedTaskRecordData)",uint32(sizeof(CPedScriptedTaskRecordData)),0x14);
      Chk("CPedGroup.membership",OffOf(&reinterpret_cast<CPedGroup*>(cb.data())->GetMembership()),8); Chk("CPedGroup.intelligence",OffOf(&reinterpret_cast<CPedGroup*>(cb.data())->GetIntelligence()),0x30);
      Chk("CPedIntelligence.taskMgr",OffOf(&reinterpret_cast<CPedIntelligence*>(cb.data())->m_TaskMgr),4); Chk("CPedIntelligence.eventGroup",OffOf(&reinterpret_cast<CPedIntelligence*>(cb.data())->m_eventGroup),0x68);
      Chk("CTaskSequences::ms_iActiveSequence",uint32(reinterpret_cast<uintptr_t>(&CTaskSequences::ms_iActiveSequence)),0x8D2E98);
      Chk("CPopCycle::m_pCurrZoneInfo",uint32(reinterpret_cast<uintptr_t>(&CPopCycle::m_pCurrZoneInfo)),0xC0BC68);
      Chk("LocalVariablesForCurrentMission",uint32(reinterpret_cast<uintptr_t>(&CTheScripts::LocalVariablesForCurrentMission)),0xA48960);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){
        Snap(kRbBase,kRbSize);
        Snap(0xC0BC68,4); Snap(0xC091F0,0xA0); Snap(0x8E4CC0+20*285,20*3); Snap(0xA9B0C8+4*596,16);
        Snap(0xB74490,4); Snap(0xA48960,0x100); Snap(0x8D2E98,4); }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3C7Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); scN=0;
                hooks[f]->State(candidate);
                const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                Patches P;
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                if(nm=="Init"){
                    P.Add(0x538900,S_FOpen); P.Add(D("fopen"),S_FOpen); P.Add(0x538950,S_FRead); P.Add(D("fread"),S_FRead); P.Add(0x5389D0,S_FClose); P.Add(D("fclose"),S_FClose);
                    out["state"]=CaseInit(r); }
                else if(nm=="GenerateRoadBlockPedsForCar"){
                    P.Add(0x571B70,S_Reg); P.Add(D("reg"),S_Reg); P.Add(0x563220,S_WorldAdd); P.Add(D("world_add"),S_WorldAdd);
                    P.Add(0x821B1E,S_RandExe); P.Add(D("rand"),S_RandDll); P.Add(0x56E210,S_Player); P.Add(D("player"),S_Player);
                    P.Add(0x5E4720,S_PedNew); P.Add(D("ped_new"),S_PedNew); P.Add(0x5DDC60,S_CopCtor); P.Add(D("cop_ctor"),S_CopCtor);
                    P.Add(0x5DDB70,S_CivCtor); P.Add(D("civ_ctor"),S_CivCtor); P.Add(0x61A5A0,S_TaskNew); P.Add(D("task_new"),S_TaskNew);
                    P.Add(0x681AF0,S_SetTask); P.Add(D("set_task"),S_SetTask); P.Add(0x4AB420,S_EvAdd); P.Add(D("event_add"),S_EvAdd);
                    P.Add(0x535300,S_GetCol); P.Add(D("get_col"),S_GetCol); P.Add(0x407C50,S_CopCar); P.Add(D("cop_car"),S_CopCar);
                    P.Add(0x5DE5A0,S_GangModel); P.Add(D("gang_model"),S_GangModel); P.Add(0x59B120,S_SetRot); P.Add(D("set_rotate"),S_SetRot);
                    P.Add(0x5674E0,S_Pvl); P.Add(D("pvl"),S_Pvl); P.Add(0x732B00,S_Alpha); P.Add(D("alpha"),S_Alpha);
                    P.Add(0x5E89B0,S_Give); P.Add(D("give"),S_Give); P.Add(0x5E6280,S_SetWT); P.Add(D("set_wt"),S_SetWT); P.Add(0x5E61F0,S_SetWS); P.Add(D("set_ws"),S_SetWS);
                    out["state"]=CasePeds(r); }
                else if(nm=="GivePedScriptedTask-OG"){
                    P.Add(0x5F7E80,S_GetGroup); P.Add(D("get_group"),S_GetGroup); P.Add(0x5DF8F0,S_IsPlayer); P.Add(D("is_player"),S_IsPlayer);
                    P.Add(0x635720,S_Brain); P.Add(D("brain"),S_Brain); P.Add(0x608540,S_Vacant); P.Add(D("vacant"),S_Vacant);
                    P.Add(0x608390,S_RecEv); P.Add(D("rec_ev"),S_RecEv); P.Add(0x6084A0,S_RecGroup); P.Add(D("rec_group"),S_RecGroup); P.Add(0x608440,S_RecAttr); P.Add(D("rec_attr"),S_RecAttr);
                    P.Add(0x5F8560,S_SetCmd); P.Add(D("set_cmd"),S_SetCmd); P.Add(0x5F8690,S_GetCmd); P.Add(D("get_cmd"),S_GetCmd);
                    P.Add(0x632D10,S_SeqAdd); P.Add(D("seq_add"),S_SeqAdd); P.Add(0x4AB420,S_EvAdd); P.Add(D("event_add"),S_EvAdd);
                    out["state"]=CaseGive(r); }
                else if(nm=="SetCharCoordinates"){
                    P.Add(0x569660,S_GroundZ); P.Add(D("ground_z"),S_GroundZ); P.Add(0x536BE0,S_Dist); P.Add(D("dist"),S_Dist);
                    P.Add(0x486B00,S_Clear); P.Add(D("clear_space"),S_Clear); P.Add(0x5F7E80,S_GetGroup); P.Add(D("get_group"),S_GetGroup);
                    P.Add(0x5F69C0,S_IsLeader); P.Add(D("is_leader"),S_IsLeader); P.Add(0x5F7AD0,S_GrpTele); P.Add(D("grp_teleport"),S_GrpTele);
                    out["state"]=CaseSetChar(r); }
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
