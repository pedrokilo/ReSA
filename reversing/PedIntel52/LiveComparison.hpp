#pragma once
// Banco dentro del motor: CPedIntelligence (lista de 34, 52 funciones). Original (hooks de la clase desactivados)
// frente a candidato con fixtures deterministas e identicos en las dos ejecuciones:
// - Dos inteligencias en buffers propios (A con el peaton 0, B con el peaton 1), seis peatones falsos con offsets
//   crudos del EXE (matriz, banderas, inteligencia, datos de jugador, conocidos, estado, salud, vehiculo, tipo,
//   armas), matrices, vehiculo, objeto, clump/frame falsos, asociaciones de animacion y jerarquias falsas.
// - Tareas falsas (tipo, subtarea y retornos en el propio objeto; vtable trazada: borrado, Clone, MakeAbortable,
//   huecos 7 y 8), clones sacados de un array fijo, eventos falsos (borrado y GetSourceEntity trazados) y grupos
//   falsos (evento actual en +0x34).
// - Se simulan y trazan: CLoadMonitor::StartTimer/EndTimer, CPedGroups::GetPedsGroup,
//   CPedGroupIntelligence::SetScriptCommandTask/ComputeDefaultTasks, CEventGroup::Add/HasScriptCommandOfTaskType/
//   Flush, CEventHandler::HandleEvents/FlushImmediately, CTaskManager::ManageTasks, CPedScriptedTaskRecord::Process,
//   CTaskComplexWander::GetWanderTaskByPedType, CTaskSimpleUseGun::ClearAnim, CTaskSimpleDuck::ProcessPed,
//   CPed::GetBonePosition, CPhysical::GetLightingTotal, IKChainManager_c::IsLooking/LookAt, CEntity::GetIsOnScreen,
//   CEntity::UpdateRwFrame, CWorld::FindObjectsInRange, rand/CGeneral::GetRandomNumber, RpAnimBlendClumpGet-
//   FirstAssociation/RpAnimBlendGetNextAssociation, CBike::FixHandsToBars, los escaneres (vehiculos, entidades,
//   eventos, atascos, colisiones, Clear de escaneres y atractores) y el registro de relaciones con el jugador.
//   Todos en la direccion del EXE y en el cuerpo de la DLL, resueltos antes de parchear (nunca sobre un stub).
// - CTask::IsTaskPtr de la DLL devuelve cierto (las tareas falsas no estan en el pool).
// - Las tareas reales creadas (pato, conducir, quieto, jugador a pie) salen del pool de tareas: m_LastFreeSlot se
//   restaura y los huecos libres se rellenan con 0xCD en cada ejecucion; las que quedan vivas se vuelcan (tipo en vez
//   del vptr) y se liberan del pool.
// - Se compara la inteligencia (punteros traducidos; vptrs de grupo de eventos y escaneres como "vt" en constructor y
//   destructor), peatones, tareas, clones, eventos, grupos, vehiculo, objeto, frame, la traza (array fijo, sin
//   reservas durante la llamada), los eventos anadidos, el reloj y el retorno.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroupIntelligence.h"
#include "PlayerRelationshipRecorder.h"
#include "LoadMonitor.h"
#include "Ragdoll/IKChainManager.h"
#include "World.h"
#include "AttractorScanner.h"
#include "Events/EventGroup.h"
#include "Events/EventHandler.h"
#include "Events/EventScanner.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Entity/Vehicle/Bike.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace PedIntel52Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T=uint32> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }

// ======================= objetos falsos =======================
constexpr int NPED=6, NT=16, NTC=16, NEV=8, NGRP=2, NAS=6, NADD=4, NFRESH=4;
constexpr size_t PEDSZ=0x7A0, INTEL=0x294, INTEL_BUF=0x2B8, TSZ=0x60, EVSZ=0x40, GRPSZ=0x40, MATSZ=0x48, VEHSZ=0x900, OBJSZ=0x180,
                 ASZ=0x40, HSZ=0x20, SLOT=0x80;
// CPed (offsets crudos del EXE)
constexpr size_t P_POS=0x4, P_MATRIX=0x14, P_RW=0x18, P_TYPEB=0x36, P_PHYS=0x40, P_DMG_INT=0xD8, P_DMG_ENT=0xDC, P_IMPACT=0xE0,
                 P_F1=0x46C, P_F2=0x470, P_F3=0x474, P_F4=0x478, P_INTEL=0x47C, P_PDATA=0x480, P_CREATED=0x484, P_ACQ=0x4E0,
                 P_STATE=0x530, P_MOVE=0x534, P_HEALTH=0x540, P_VEH=0x58C, P_PTYPE=0x598, P_WEAP=0x5A0, P_WSLOT=0x718;
// CPedIntelligence
constexpr size_t I_PRIM=0x4, I_SEC=0x18, I_HIST=0x38, I_DM=0xB4, I_NSCAN=0xC4, I_PEDENT=0x130, I_EFFECT=0x1B8, I_CNT2=0x274,
                 I_CNT=0x278, I_LASTPOS=0x27C, I_INTEREST=0x288;
// tarea falsa: datos de la tarea en +4..+0x3F, control en +0x40..
constexpr size_t T_TYPE=0x40, T_SUB=0x44, T_SIMPLE=0x48, T_ABORT=0x49, T_RET7=0x4A, T_RET8=0x4B;
// evento falso
constexpr size_t E_TYPE=0x30, E_SRC=0x34;
alignas(16) inline std::array<uint8,NPED*PEDSZ> peds{};
alignas(16) inline std::array<uint8,INTEL_BUF> intelA{}, intelB{};
alignas(16) inline std::array<uint8,NT*TSZ> tasks{};
alignas(16) inline std::array<uint8,NTC*TSZ> clones{};
alignas(16) inline std::array<uint8,NEV*EVSZ> evs{};
alignas(16) inline std::array<uint8,NGRP*GRPSZ> grps{};
alignas(16) inline std::array<uint8,NPED*MATSZ> mats{};
alignas(16) inline std::array<uint8,VEHSZ> veh{};
alignas(16) inline std::array<uint8,OBJSZ> obj{};
alignas(16) inline std::array<uint8,0x40> pdata{}, clump{};
alignas(16) inline std::array<uint8,0x60> frame{};
alignas(16) inline std::array<uint8,NAS*ASZ> assocs{};
alignas(16) inline std::array<uint8,NAS*HSZ> hiers{};
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleDuck)+0x10> duckProbe{};
alignas(16) inline std::array<uint8,sizeof(CPedScanner)+0x10> scanProbe{};
inline uint8* Pd(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline CPed* Ped(int k) { return reinterpret_cast<CPed*>(Pd(k)); }
inline uint8* Tk(int k) { return tasks.data()+size_t(k)*TSZ; }
inline uint8* Ev(int k) { return evs.data()+size_t(k)*EVSZ; }
inline uint8* Gp(int k) { return grps.data()+size_t(k)*GRPSZ; }
inline uint8* Mat(int k) { return mats.data()+size_t(k)*MATSZ; }
inline uint8* As(int k) { return assocs.data()+size_t(k)*ASZ; }
inline uint8* Hi(int k) { return hiers.data()+size_t(k)*HSZ; }

// pool de tareas (CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }

struct Rg { const void* p; size_t n; size_t stride; const char* nm; };
inline const std::array<Rg,15> regions{{
    {peds.data(),peds.size(),PEDSZ,"ped"},{intelA.data(),intelA.size(),0,"intelA"},{intelB.data(),intelB.size(),0,"intelB"},
    {tasks.data(),tasks.size(),TSZ,"task"},{clones.data(),clones.size(),TSZ,"clone"},{evs.data(),evs.size(),EVSZ,"ev"},
    {grps.data(),grps.size(),GRPSZ,"grp"},{mats.data(),mats.size(),MATSZ,"mat"},{veh.data(),veh.size(),0,"veh"},
    {obj.data(),obj.size(),0,"obj"},{pdata.data(),pdata.size(),0,"pdata"},{clump.data(),clump.size(),0,"clump"},
    {frame.data(),frame.size(),0,"frame"},{assocs.data(),assocs.size(),ASZ,"assoc"},{hiers.data(),hiers.size(),HSZ,"hier"}}};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    for(auto& r:regions){ const auto b=uintptr_t(r.p); if(v<b||v>=b+r.n)continue; const auto o=v-b;
        if(r.stride)return std::string(r.nm)+"#"+std::to_string(o/r.stride)+"+"+std::to_string(o%r.stride);
        return std::string(r.nm)+"+"+std::to_string(o); }
    { const auto s=uintptr_t(PoolStorage()); if(v>=s&&v<s+size_t(PoolCap())*SLOT)return "pool#"+std::to_string((v-s)/SLOT)+"+"+std::to_string((v-s)%SLOT); }
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 { C_TDTOR, C_TCLONE, C_TABORT, C_TSLOT7, C_TSLOT8, C_TTRAP, C_EDTOR, C_ESRC, C_ETRAP,
    C_LM_START, C_LM_END, C_PGRP, C_SCRIPTTASK, C_DEFTASKS, C_EVADD, C_HASSC, C_EGFLUSH, C_HANDLE, C_EHFLUSH, C_MANAGE,
    C_STRPROC, C_WANDER, C_CLEARANIM, C_DUCKPROC, C_BONE, C_LIGHT, C_ISLOOK, C_ONSCREEN, C_LOOKAT, C_LOOKAT2, C_FINDOBJ,
    C_FINDOBJ2, C_RAND, C_FIRSTAS, C_NEXTAS, C_FIXHANDS, C_RWFRAME, C_SCANVEH, C_SCANENT, C_SCANEV, C_STUCK, C_COLL,
    C_RECREL, C_CLRREL, C_SCLEAR, C_ACLEAR };
inline constexpr const char* CODE_NAMES[]{"task_delete","task_clone","task_make_abortable","task_slot7","task_slot8","task_trap",
    "ev_delete","ev_source","ev_trap","lm_start","lm_end","peds_group","set_script_command_task","compute_default_tasks",
    "event_group_add","has_script_command","event_group_flush","handle_events","event_handler_flush_immediately","manage_tasks",
    "scripted_task_record_process","wander_task","use_gun_clear_anim","duck_process_ped","bone_position","lighting_total",
    "ik_is_looking","is_on_screen","ik_look_at","ik_look_at_2","find_objects_in_range","find_objects_in_range_2","rand",
    "first_association","next_association","fix_hands_to_bars","update_rw_frame","scan_vehicles","scan_entities","scan_events",
    "test_ped_stuck","scan_collision_events","record_relationship","clear_relationship","entity_scanner_clear","attractor_scanner_clear"};
struct TR { uint32 code,a,b,c,d; };
inline std::array<TR,512> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 code,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0) { if(ntr<int(tr.size()))tr[ntr++]={code,a,b,c,d}; else trOverflow=true; }
inline json TraceJson() { json r=json::array(); for(int k=0;k<ntr;++k){ const auto& e=tr[k]; r.push_back({CODE_NAMES[e.code],Name(e.a),Name(e.b),Name(e.c),Name(e.d)}); } return r; }

// estado simulado de cada ejecucion
inline uint32 maskA{}, callsA{}; inline bool BitA() { return (maskA>>(callsA++%32))&1; }
inline std::array<uint32,16> randSeq{}; inline uint32 callsR{};
inline std::array<uint8*,NPED> pedGroup{};
inline int nTc{};
inline float lightVal{}; inline CVector bonePos{}; inline uint8* wanderRet{};
inline int16 findCount{}; inline std::array<CEntity*,10> findList{};
inline int firstAssoc{}; inline std::array<int,NAS> nextAssoc{};
// eventos anadidos al grupo (CEventGroup::Add): campos y copia de la tarea real del pool
struct AddRec { uint32 group,type,timeActive,valid,primary,task,affects,validArg,taskType; bool dumped; std::array<uint8,SLOT> bytes; };
inline std::array<AddRec,NADD> adds{}; inline int nAdds{};

// ======================= vtables falsas =======================
template<int N> void* __fastcall TTrap(void* self,void*) { T(C_TTRAP,uint32(N),U(self)); return nullptr; }
template<int N> void* __fastcall ETrap(void* self,void*) { T(C_ETRAP,uint32(N),U(self)); return nullptr; }
inline std::array<void*,16> taskVt{}, evVt{};
template<int... N> void FillTT(std::integer_sequence<int,N...>) { ((taskVt[N]=reinterpret_cast<void*>(&TTrap<N>)),...); }
template<int... N> void FillET(std::integer_sequence<int,N...>) { ((evVt[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
inline void* __fastcall TDtor(uint8* self,void*,uint32 flag) { T(C_TDTOR,U(self),flag); return self; }
inline void* __fastcall TClone(uint8* self,void*) { uint8* r=nullptr; if(nTc<NTC){ r=clones.data()+size_t(nTc++)*TSZ; std::memcpy(r,self,TSZ); } T(C_TCLONE,U(self),U(r)); return r; }
inline void* __fastcall TSub(uint8* self,void*) { return At<void*>(self,T_SUB); }
inline bool __fastcall TSimple(uint8* self,void*) { return At<uint8>(self,T_SIMPLE)!=0; }
inline int32 __fastcall TType(uint8* self,void*) { return At<int32>(self,T_TYPE); }
inline bool __fastcall TAbort(uint8* self,void*,CPed* ped,int32 prio,const void* ev) { T(C_TABORT,U(self),U(ped),uint32(prio),U(ev)); return At<uint8>(self,T_ABORT)&1; }
// hueco 7: ProcessPed (simple) / CreateNextSubTask (compleja); hueco 8: SetPedPosition / CreateFirstSubTask
inline uint32 __fastcall TSlot7(uint8* self,void*,CPed* ped) { T(C_TSLOT7,U(self),U(ped)); return At<uint8>(self,T_SIMPLE) ? uint32(At<uint8>(self,T_RET7)&1) : 0u; }
inline uint32 __fastcall TSlot8(uint8* self,void*,CPed* ped) { T(C_TSLOT8,U(self),U(ped)); return At<uint8>(self,T_SIMPLE) ? uint32(At<uint8>(self,T_RET8)&1) : 0u; }
inline void* __fastcall EDtor(uint8* self,void*,uint32 flag) { T(C_EDTOR,U(self),flag); return self; }
inline int32 __fastcall EType(uint8* self,void*) { return At<int32>(self,E_TYPE); }
inline void* __fastcall ESrc(uint8* self,void*) { T(C_ESRC,U(self)); return At<void*>(self,E_SRC); }

// ======================= stubs de dependencias =======================
inline void __fastcall LmStart(void* self,void*,int32 idx) { T(C_LM_START,U(self),uint32(idx)); }
inline void __fastcall LmEnd(void* self,void*,int32 idx) { T(C_LM_END,U(self),uint32(idx)); }
inline CPedGroup* __cdecl PedsGroup(const CPed* ped) { uint8* r=nullptr; for(int k=0;k<NPED;++k)if(reinterpret_cast<const uint8*>(ped)==Pd(k))r=pedGroup[k]; T(C_PGRP,U(ped),U(r)); return reinterpret_cast<CPedGroup*>(r); }
inline void __fastcall ScriptTask(void* self,void*,CPed* ped,const CTask* task) { T(C_SCRIPTTASK,U(self),U(ped),U(task)); }
inline void __fastcall DefTasks(void* self,void*,CPed* ped) { T(C_DEFTASKS,U(self),U(ped)); }
inline CEvent* __fastcall EvAdd(void* g,void*,CEvent* e,bool valid) {
    auto* b=reinterpret_cast<uint8*>(e); // bytes 9-0xB y 0x15-0x17 del evento en pila sin inicializar: no se trazan
    if(nAdds<NADD){ auto& a=adds[nAdds++]; a={}; a.group=U(g); a.type=uint32(e->GetEventType()); a.timeActive=At<uint32>(b,4); a.valid=b[8];
        a.primary=At<uint32>(b,0xC); a.task=At<uint32>(b,0x10); a.affects=b[0x14]; a.validArg=valid;
        const auto t=uintptr_t(a.task), s=uintptr_t(PoolStorage());
        if(t>=s&&t<s+size_t(PoolCap())*SLOT&&(t-s)%SLOT==0){ a.dumped=true; std::memcpy(a.bytes.data(),reinterpret_cast<void*>(t),SLOT); a.taskType=uint32(reinterpret_cast<CTask*>(t)->GetTaskType()); } }
    T(C_EVADD,U(g)); return nullptr;   // el evento esta en la pila del llamador: su direccion no se traza
}
inline bool __fastcall HasSC(void* g,void*,int32 type) { const bool r=BitA(); T(C_HASSC,U(g),uint32(type),r); return r; }
inline void __fastcall EgFlush(void* g,void*,bool flag) { T(C_EGFLUSH,U(g),flag); }
inline void __fastcall Handle(void* eh,void*) { T(C_HANDLE,U(eh)); }
inline void __fastcall EhFlush(void* eh,void*) { T(C_EHFLUSH,U(eh)); }
inline void __fastcall Manage(void* tm,void*) { T(C_MANAGE,U(tm)); }
inline void __cdecl StrProc() { T(C_STRPROC); }
inline CTask* __cdecl Wander(CPed* ped) { T(C_WANDER,U(ped),U(wanderRet)); return reinterpret_cast<CTask*>(wanderRet); }
inline void __fastcall ClearAnim(void* gun,void*,CPed* ped) { T(C_CLEARANIM,U(gun),U(ped)); }
inline bool __fastcall DuckProc(void* t,void*,CPed* ped) { T(C_DUCKPROC,U(t),U(ped)); return false; }
// exe 0x5E4280 (out, hueso, actualizar) y la version de la DLL que devuelve CVector (puntero oculto primero)
inline CVector* __fastcall Bone(CPed* p,void*,CVector* out,int32 bone,uint32 upd) { T(C_BONE,U(p),uint32(bone),upd&0xFF); *out=bonePos; return out; }
inline float __fastcall Light(void* p,void*) { T(C_LIGHT,U(p)); return lightVal; }
inline bool __fastcall IsLooking(void* m,void*,CPed* ped) { const bool r=BitA(); T(C_ISLOOK,U(m),U(ped),r); return r; }
inline bool __fastcall OnScreen(void* e,void*) { const bool r=BitA(); T(C_ONSCREEN,U(e),r); return r; }
inline void __fastcall LookAt(void* m,void*,const char* purpose,CPed* ped,CEntity* ent,int32 time,int32 bone,CVector* offset,bool torso,float speed,int32 blend,int32 prio,bool force) {
    uint32 h=2166136261u; for(const char* c=purpose;c&&*c;++c){h^=uint8(*c);h*=16777619u;}   // el literal del EXE y el de la DLL estan en sitios distintos
    T(C_LOOKAT,U(m),h,U(ped),U(ent)); T(C_LOOKAT2,uint32(time),uint32(bone),offset ? F(offset->x)^F(offset->y)*3u^F(offset->z)*7u : 0xFFFFFFFFu,(torso ? 1u : 0u)|(force ? 2u : 0u));
    T(C_LOOKAT2,F(speed),uint32(blend),uint32(prio));
}
inline void __cdecl FindObj(const CVector& pt,float radius,bool b2D,int16* outCount,int16 maxCount,CEntity** out,bool b,bool v,bool p,bool o,bool d) {
    // contador y lista en la pila del llamador: no se trazan sus direcciones
    T(C_FINDOBJ,F(pt.x),F(pt.y),F(pt.z),F(radius)); T(C_FINDOBJ2,b2D,uint32(maxCount),uint32(*outCount)&0xFFFF,(b?1u:0u)|(v?2u:0u)|(p?4u:0u)|(o?8u:0u)|(d?16u:0u));
    for(int k=0;k<findCount&&k<maxCount;++k)out[k]=findList[k];
    *outCount=findCount;
}
inline int __cdecl Rand() { const int r=int(randSeq[callsR%16]); ++callsR; T(C_RAND,uint32(r)); return r; }
inline CAnimBlendAssociation* __cdecl FirstAs(RpClump* c,uint32 flags) { uint8* r=firstAssoc>=0 ? As(firstAssoc) : nullptr; T(C_FIRSTAS,U(c),flags,U(r)); return reinterpret_cast<CAnimBlendAssociation*>(r); }
inline CAnimBlendAssociation* __cdecl NextAs(CAnimBlendAssociation* a) { uint8* r=nullptr; for(int k=0;k<NAS;++k)if(reinterpret_cast<uint8*>(a)==As(k)&&nextAssoc[k]>=0)r=As(nextAssoc[k]); T(C_NEXTAS,U(a),U(r)); return reinterpret_cast<CAnimBlendAssociation*>(r); }
inline void __fastcall FixHands(void* bike,void*,CPed* ped) { T(C_FIXHANDS,U(bike),U(ped)); }
inline void __fastcall RwFrameUpd(void* e,void*) { T(C_RWFRAME,U(e)); }
inline void __fastcall ScanVeh(void* s,void*,const CPed* ped) { T(C_SCANVEH,U(s),U(ped)); }
inline void __fastcall ScanEnt(void* s,void*,int32 list,const CPed* ped) { T(C_SCANENT,U(s),uint32(list),U(ped)); }
inline void __fastcall ScanEv(void* s,void*,CPed* ped) { T(C_SCANEV,U(s),U(ped)); }
inline bool __fastcall Stuck(void* s,void*,CPed* ped,void* g) { const bool r=BitA(); T(C_STUCK,U(s),U(ped),U(g),r); return r; }
inline void __fastcall Coll(void* s,void*,CPed* ped,void* g) { T(C_COLL,U(s),U(ped),U(g)); }
inline void __fastcall RecRel(void* rec,void*,const CPed* ped) { T(C_RECREL,U(rec),U(ped)); }
inline void __fastcall ClrRel(void* rec,void*,const CPed* ped) { T(C_CLRREL,U(rec),U(ped)); }
inline void __fastcall SClear(void* s,void*) { T(C_SCLEAR,U(s)); }
inline void __fastcall AClear(void* s,void*) { T(C_ACLEAR,U(s)); }
inline bool __cdecl AlwaysTaskPtr(CTask*) { return true; }

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
// CEntityScanner::Clear es protegido: se nombra a traves de una clase derivada (no virtual, puntero directo)
struct ScanAccess : CEntityScanner { static uintptr_t ClearAddr() { return MAddr(&ScanAccess::Clear); } };

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=52;
inline const std::array<Fn,NFNS> fns{{
    {"CPedIntelligence","Constructor",0x607140,0,-1,128},
    {"CPedIntelligence","Destructor",0x607300,0,-1,256},
    {"CPedIntelligence","GetPedEntities",0x4893E0,0,-1,64},
    {"CPedIntelligence","SetPedDecisionMakerType",0x600B50,0,-1,256},
    {"CPedIntelligence","SetPedDecisionMakerTypeInGroup",0x600BB0,0,-1,64},
    {"CPedIntelligence","RestorePedDecisionMakerType",0x600BC0,0,-1,128},
    {"CPedIntelligence","SetHearingRange",0x600BE0,0,-1,64},
    {"CPedIntelligence","SetSeeingRange",0x600BF0,0,-1,64},
    {"CPedIntelligence","IsInSeeingRange",0x600C60,0,-1,512},
    {"CPedIntelligence","FindRespectedFriendInInformRange",0x600CF0,0,-1,512},
    {"CPedIntelligence","IsRespondingToEvent",0x600DB0,0,-1,256},
    {"CPedIntelligence","AddTaskPhysResponse",0x600DC0,0,-1,128},
    {"CPedIntelligence","AddTaskEventResponseTemp",0x600DE0,0,-1,128},
    {"CPedIntelligence","AddTaskEventResponseNonTemp",0x600E00,0,-1,128},
    {"CPedIntelligence","AddTaskPrimaryMaybeInGroup",0x600E20,0,-1,256},
    {"CPedIntelligence","FindTaskByType",0x600EE0,0,-1,256},
    {"CPedIntelligence","GetTaskFighting",0x600F30,0,-1,128},
    {"CPedIntelligence","GetTaskUseGun",0x600F70,0,-1,128},
    {"CPedIntelligence","GetTaskThrow",0x600FB0,0,-1,128},
    {"CPedIntelligence","GetTaskHold",0x600FF0,0,-1,256},
    {"CPedIntelligence","GetTaskSwim",0x601070,0,-1,128},
    {"CPedIntelligence","GetTaskDuck",0x6010A0,0,-1,256},
    {"CPedIntelligence","GetTaskJetPack",0x601110,0,-1,128},
    {"CPedIntelligence","GetTaskInAir",0x601150,0,-1,128},
    {"CPedIntelligence","GetTaskClimb",0x601180,0,-1,128},
    {"CPedIntelligence","GetUsingParachute",0x6011B0,0,-1,512},
    {"CPedIntelligence","SetTaskDuckSecondary",0x601230,0,-1,256},
    {"CPedIntelligence","ClearTaskDuckSecondary",0x601390,0,-1,256},
    {"CPedIntelligence","ClearTasks",0x601420,0,-1,512},
    {"CPedIntelligence","FlushImmediately",0x601640,0,-1,512},
    {"CPedIntelligence","GetEffectInUse",0x6018D0,0,-1,64},
    {"CPedIntelligence","SetEffectInUse",0x6018E0,0,-1,64},
    {"CPedIntelligence","ProcessAfterProcCol",0x6018F0,0,-1,256},
    {"CPedIntelligence","ProcessAfterPreRender",0x6019B0,0,-1,256},
    {"CPedIntelligence","ProcessEventHandler",0x601BB0,0,-1,64},
    {"CPedIntelligence","IsFriendlyWith",0x601BC0,0,-1,256},
    {"CPedIntelligence","IsThreatenedBy",0x601C30,0,-1,256},
    {"CPedIntelligence","Respects",0x601C90,0,-1,256},
    {"CPedIntelligence","IsInACarOrEnteringOne",0x601CC0,0,-1,256},
    {"CPedIntelligence","AreFriends",0x601D10,0,-1,256},
    {"CPedIntelligence","GetMoveStateFromGoToTask",0x601D70,0,-1,128},
    {"CPedIntelligence","FlushIntelligence",0x601DA0,0,-1,128},
    {"CPedIntelligence","TestForStealthKill",0x601E00,0,-1,1024},
    {"CPedIntelligence","RecordEventForScript",0x602050,0,-1,256},
    {"CPedIntelligence","IsInterestingEntity",0x6020A0,0,-1,128},
    {"CPedIntelligence","LookAtInterestingEntities",0x6020D0,0,-1,512},
    {"CPedIntelligence","IsPedGoingForCarDoor",0x602350,0,-1,256},
    {"CPedIntelligence","CanSeeEntityWithLights",0x605550,0,-1,1024},
    {"CPedIntelligence","ProcessStaticCounter",0x605650,0,-1,1024},
    {"CPedIntelligence","ProcessFirst",0x6073A0,0,-1,512},
    {"CPedIntelligence","Process",0x608260,0,-1,256},
    {"CPedIntelligence","GetActivePrimaryTask",0x4B85B0,0,-1,128},
}};

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int k=0;k<NPED;++k)clean(Pd(k)); clean(veh.data()); clean(obj.data());
}
inline constexpr int32 TYPES[]{TASK_SIMPLE_FIGHT,TASK_SIMPLE_USE_GUN,TASK_SIMPLE_THROW_PROJECTILE,TASK_SIMPLE_HOLD_ENTITY,
    TASK_SIMPLE_PICKUP_ENTITY,TASK_SIMPLE_PUTDOWN_ENTITY,TASK_SIMPLE_SWIM,TASK_SIMPLE_DUCK,TASK_SIMPLE_JETPACK,TASK_SIMPLE_IN_AIR,
    TASK_SIMPLE_CLIMB,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_SIMPLE_CAR_DRIVE,
    TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,
    TASK_COMPLEX_BE_IN_GROUP,TASK_COMPLEX_FACIAL,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_GANG_DRIVEBY,200,999};
constexpr int NTYPES=int(std::size(TYPES));

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pedintel52-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pedintel52-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pedintel52");}
    if(!FindPlayerPed() || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"pedintel52-only.txt"); if(f)std::getline(f,only); }
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(auto& fn:fns){ auto* h=FindHook(fn.cls,fn.name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fn.cls)+"::"+fn.name}});return;} hooks.push_back(h); }
    // vtables falsas
    FillTT(std::make_integer_sequence<int,16>{}); FillET(std::make_integer_sequence<int,16>{});
    taskVt[0]=reinterpret_cast<void*>(&TDtor); taskVt[1]=reinterpret_cast<void*>(&TClone); taskVt[2]=reinterpret_cast<void*>(&TSub);
    taskVt[3]=reinterpret_cast<void*>(&TSimple); taskVt[4]=reinterpret_cast<void*>(&TType); taskVt[6]=reinterpret_cast<void*>(&TAbort);
    taskVt[7]=reinterpret_cast<void*>(&TSlot7); taskVt[8]=reinterpret_cast<void*>(&TSlot8);
    evVt[0]=reinterpret_cast<void*>(&EDtor); evVt[1]=reinterpret_cast<void*>(&EType); evVt[10]=reinterpret_cast<void*>(&ESrc);
    // cuerpos de la DLL que solo se alcanzan por vtable: CTaskSimpleDuck::ProcessPed (hueco 7) y
    // CEntityScanner::ScanForEntitiesInRange (hueco 0), leidos de objetos construidos por la DLL en buffers propios
    const uintptr_t dllDuckProcess=(*reinterpret_cast<uintptr_t**>(::new (duckProbe.data()) CTaskSimpleDuck(DUCK_STANDALONE,0)))[7];
    const uintptr_t dllScanEntities=(*reinterpret_cast<uintptr_t**>(::new (scanProbe.data()) CPedScanner()))[0];
    // indice del bloque "parachute" y numero de bloques (solo lectura)
    // ms_aAnimBlocks (0xB5D4A0, 32 bytes por bloque, nombre en +0) y ms_numAnimBlocks (0xB4EA30) son privados: se leen por direccion
    const int32 numBlocks=int32(std::min<uint32>(*reinterpret_cast<const uint32*>(uintptr_t(0xB4EA30)),NUM_ANIM_BLOCKS)); int32 parachuteBlock=-1;
    for(int32 k=0;k<numBlocks;++k)if(_stricmp(reinterpret_cast<const char*>(uintptr_t(0xB5D4A0)+size_t(k)*0x20),"parachute")==0){parachuteBlock=k;break;}
    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,void* stub){ if(exeAddr)plan.push_back({exeAddr,stub}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,stub}); } };
    auto V=[](auto f){ return reinterpret_cast<void*>(f); };
    add(0x53D050,MAddr(&CLoadMonitor::StartTimer),V(&LmStart));
    add(0x53D070,MAddr(&CLoadMonitor::EndTimer),V(&LmEnd));
    add(0x5F7E80,uintptr_t(&CPedGroups::GetPedsGroup),V(&PedsGroup));
    add(0x5F8560,MAddr(&CPedGroupIntelligence::SetScriptCommandTask),V(&ScriptTask));
    add(0x5F88D0,MAddr(&CPedGroupIntelligence::ComputeDefaultTasks),V(&DefTasks));
    add(0x4AB420,MAddr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),V(&EvAdd));
    add(0x4AB840,MAddr(&CEventGroup::HasScriptCommandOfTaskType),V(&HasSC));
    add(0x4AB370,MAddr(&CEventGroup::Flush),V(&EgFlush));
    add(0x4C3F10,MAddr(&CEventHandler::HandleEvents),V(&Handle));
    add(0x4C3820,MAddr(&CEventHandler::FlushImmediately),V(&EhFlush));
    add(0x681C10,MAddr(&CTaskManager::ManageTasks),V(&Manage));
    add(0x6085E0,uintptr_t(&CPedScriptedTaskRecord::Process),V(&StrProc));
    add(0x673D00,uintptr_t(&CTaskComplexWander::GetWanderTaskByPedType),V(&Wander));
    add(0x61E190,MAddr(&CTaskSimpleUseGun::ClearAnim),V(&ClearAnim));
    plan.push_back({Body(dllDuckProcess),V(&DuckProc)});   // entrada de la vtable (DLL o EXE): se parchea su destino
    add(0x5E4280,MAddr(static_cast<CVector(CPed::*)(eBoneTag,bool)>(&CPed::GetBonePosition)),V(&Bone));
    add(0,MAddr(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),V(&Bone));
    add(0x544850,MAddr(&CPhysical::GetLightingTotal),V(&Light));
    add(0x6181A0,MAddr(&IKChainManager_c::IsLooking),V(&IsLooking));
    add(0x618970,MAddr(&IKChainManager_c::LookAt),V(&LookAt));
    add(0x534540,MAddr(&CEntity::GetIsOnScreen),V(&OnScreen));
    add(0x532B00,MAddr(&CEntity::UpdateRwFrame),V(&RwFrameUpd));
    add(0x564A20,uintptr_t(&CWorld::FindObjectsInRange),V(&FindObj));
    add(0x821B1E,uintptr_t(&CGeneral::GetRandomNumber),V(&Rand));
    add(0x4D6A70,uintptr_t(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),V(&FirstAs));
    add(0x4D6AB0,uintptr_t(static_cast<CAnimBlendAssociation*(*)(CAnimBlendAssociation*)>(&RpAnimBlendGetNextAssociation)),V(&NextAs));
    add(0x6B7F90,MAddr(&CBike::FixHandsToBars),V(&FixHands));
    add(0x5FFE60,MAddr(&CVehicleScanner::ScanForVehiclesInRange),V(&ScanVeh));
    add(0x5FFA20,0,V(&ScanEnt)); plan.push_back({Body(dllScanEntities),V(&ScanEnt)});
    add(0x607E30,MAddr(&CEventScanner::ScanForEvents),V(&ScanEv));
    add(0x602C00,MAddr(&CPedStuckChecker::TestPedStuck),V(&Stuck));
    add(0x604500,MAddr(&CCollisionEventScanner::ScanForCollisionEvents),V(&Coll));
    add(0x61A1D0,MAddr(&CPlayerRelationshipRecorder::RecordRelationshipWithPlayer),V(&RecRel));
    add(0x61A150,MAddr(&CPlayerRelationshipRecorder::ClearRelationshipWithPlayer),V(&ClrRel));
    add(0x5FF9D0,ScanAccess::ClearAddr(),V(&SClear));
    add(0x5FFF90,MAddr(&CAttractorScanner::Clear),V(&AClear));
    add(0,uintptr_t(&CTask::IsTaskPtr),V(&AlwaysTaskPtr));
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }
    const auto occupancyStart=Occupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lf0=*reinterpret_cast<int32*>(PoolHdr()+0xC);
    auto& timeMs=CTimer::m_snTimeInMilliseconds; const uint32 savedTime=timeMs;
    std::vector<bool> enabled(hooks.size()),locked(hooks.size());
    for(size_t f=0;f<hooks.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);hooks[f]->State(false);}
    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del pool de tareas del EXE busca desde m_LastFreeSlot+1: mismo slot en las dos ejecuciones;
                    // los huecos libres se rellenan con 0xCD
                    { *reinterpret_cast<int32*>(PoolHdr()+0xC)=lf0; auto* stor=PoolStorage(); auto* sb=PoolStates(); const int32 cap=PoolCap();
                      for(int32 k=0;k<cap;++k)if(sb[k]&0x80)std::memset(stor+size_t(k)*SLOT,0xCD,SLOT); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5252u; if(!st)st=1;
                    ntr=0; trOverflow=false; callsA=0; callsR=0; nTc=0; nAdds=0;
                    auto fillRand=[&](uint8* p,size_t n){ for(size_t k=0;k+4<=n;k+=4)At<uint32>(p,k)=next(); };
                    const bool randomSimple=nm=="ProcessAfterProcCol"||nm=="ProcessAfterPreRender";
                    // ---- mundo ----
                    for(int k=0;k<NPED;++k){ uint8* m=Mat(k); fillRand(m,MATSZ);
                        for(int c=0;c<3;++c){ At<float>(m,0x0+4*c)=uni(-1.f,1.f); At<float>(m,0x10+4*c)=uni(-1.f,1.f); At<float>(m,0x20+4*c)=uni(-1.f,1.f); }
                        At<float>(m,0x30)=uni(-6.f,6.f); At<float>(m,0x34)=uni(-6.f,6.f); At<float>(m,0x38)=uni(-2.f,2.f); At<uint32>(m,0x40)=0; At<uint32>(m,0x44)=0; }
                    std::memset(veh.data(),0,veh.size()); veh[0x36]=uint8((next()&0xF8)|2); At<int32>(veh.data(),0x590)=next()%3 ? 9 : int32(next()%11);
                    veh[0x7B4]=uint8(next()); veh[0x7B5]=uint8(next());
                    std::memset(obj.data(),0,obj.size()); obj[0x36]=uint8((next()&0xF8)|(next()%4 ? 4u : 3u)); At(obj.data(),0x1C)=next(); obj[0x13C]=uint8(next());
                    fillRand(pdata.data(),pdata.size());
                    std::memset(clump.data(),0,clump.size()); clump[0]=2; At<void*>(clump.data(),4)=frame.data();   // rpCLUMP; padre = frame
                    fillRand(frame.data(),frame.size());
                    for(int k=0;k<NPED;++k){ uint8* p=Pd(k); std::memset(p,0,PEDSZ);
                        p[P_TYPEB]=uint8((next()&0xF8)|3); At<void*>(p,P_MATRIX)=Mat(k);
                        for(int c=0;c<3;++c)At<float>(p,P_POS+4*c)=uni(-6.f,6.f);
                        At(p,P_PHYS)=next(); At(p,P_F1)=next(); At(p,P_F2)=next(); At(p,P_F3)=next(); At(p,P_F4)=next();
                        At<void*>(p,P_INTEL)=k==0 ? intelA.data() : k==1 ? intelB.data() : next()%2 ? intelA.data() : intelB.data();
                        At<void*>(p,P_PDATA)=next()%3==0 ? pdata.data() : nullptr;
                        p[P_CREATED]=uint8(next()%4==0 ? next()%5 : 1+next()%2);
                        for(int c=0;c<5;++c){ const uint32 m=next()%5; At(p,P_ACQ+4*c)=m==0 ? 0u : m==1 ? next() : m==2 ? next()&next() : m==3 ? next()|next() : 0xFFFFFFFFu; }
                        { const uint32 m=next()%6; At(p,P_STATE)=m==0 ? 0x36u : m==1 ? 0x37u : m==2 ? 0u : 1u; }
                        At<int32>(p,P_MOVE)=int32(next()%8); At<float>(p,P_HEALTH)=next()%5==0 ? uni(-2.f,1.5f) : uni(0.f,200.f);
                        At<void*>(p,P_VEH)=next()%2 ? veh.data() : nullptr;
                        { const uint32 m=next()%4; At<int32>(p,P_PTYPE)=int32(m==0 ? next()%2 : m==1 ? 7+next()%10 : m==2 ? next()%32 : 4+next()%3); }
                        for(int w=0;w<13;++w){ static const uint32 wt[]{0,18,46,46,22,31}; uint8* wp=p+P_WEAP+size_t(w)*0x1C; fillRand(wp,0x1C); At(wp,0)=wt[next()%6]; At(wp,0x18)=0; }
                        p[P_WSLOT]=uint8(next()%13);
                        At<float>(p,P_DMG_INT)=next()%3 ? uni(0.1f,5.f) : (next()%2 ? 0.f : -1.f);
                        { const uint32 m=next()%5; At<void*>(p,P_DMG_ENT)=m==0 ? nullptr : m==1 ? static_cast<void*>(veh.data()) : m==2 ? static_cast<void*>(obj.data()) : static_cast<void*>(Pd(int(next()%NPED))); if(m==4)At<void*>(p,P_DMG_ENT)=nullptr; }
                        for(int c=0;c<3;++c)At<float>(p,P_IMPACT+4*c)=uni(-3.f,3.f);
                    }
                    for(int n=0;n<NT;++n){ uint8* t=Tk(n); fillRand(t,TSZ); At<void*>(t,0)=taskVt.data();
                        At<void*>(t,4)=next()%3 ? nullptr : static_cast<void*>(Tk(int(next()%NT)));
                        { const uint32 m=next()%5; At<uint32>(t,8)=m==0 ? 0u : m==1 ? U(obj.data()) : m==2 ? U(Pd(int(next()%NPED))) : m==3 ? U(veh.data()) : next()%9; }
                        if(nm=="FlushImmediately"&&At<uint32>(t,8)<16)At<uint32>(t,8)=next()%2 ? U(obj.data()) : 0u;   // entidad a sostener
                        At<uint32>(t,0xC)=next()%3 ? U(veh.data()) : 0u;
                        { const uint32 m=next()%4; At<uint32>(t,0x10)=m==0 ? U(Pd(0)) : m==1 ? U(Pd(1)) : m==2 ? 0u : next(); }
                        { const uint32 m=next()%4; t[0x24]=uint8(m==0 ? 2 : m==1 ? 4 : m==2 ? 0 : next()); }
                        At<int32>(t,T_TYPE)=TYPES[next()%NTYPES];
                        At<void*>(t,T_SUB)=n<NT-1&&next()%4==0 ? static_cast<void*>(Tk(n+1+int(next()%uint32(NT-1-n)))) : nullptr;
                        t[T_SIMPLE]=uint8(randomSimple ? next()%3!=0 : 1); t[T_ABORT]=uint8(next()); t[T_RET7]=uint8(next()); t[T_RET8]=uint8(next()); }
                    std::memset(clones.data(),0xCD,clones.size());
                    for(int e=0;e<NEV;++e){ uint8* ev=Ev(e); fillRand(ev,EVSZ); At<void*>(ev,0)=evVt.data();
                        static const int32 et[]{9,31,32,0,5}; At<int32>(ev,E_TYPE)=et[next()%5];
                        const uint32 m=next()%4; At<uint32>(ev,E_SRC)=m==0 ? U(Pd(0)) : m==1 ? U(Pd(1)) : m==2 ? U(veh.data()) : 0u; }
                    for(int g=0;g<NGRP;++g){ fillRand(Gp(g),GRPSZ); At<uint32>(Gp(g),0x34)=next()%3 ? U(Ev(int(next()%NEV))) : 0u; }
                    for(int k=0;k<NPED;++k)pedGroup[k]=next()%3 ? nullptr : Gp(int(next()%NGRP));
                    for(int a=0;a<NAS;++a){ fillRand(As(a),ASZ); fillRand(Hi(a),HSZ); At<void*>(As(a),0x14)=Hi(a);
                        At<int32>(Hi(a),0xC)=parachuteBlock>=0&&next()%3==0 ? parachuteBlock : int32(next()%uint32(std::max(numBlocks,1)));
                        nextAssoc[a]=a<NAS-1&&next()%3 ? a+1+int(next()%uint32(NAS-1-a)) : -1; }
                    firstAssoc=next()%5 ? int(next()%NAS) : -1;
                    auto anyTask=[&]()->uint8*{ return next()%5<2 ? nullptr : Tk(int(next()%NT)); };
                    for(int which=0;which<2;++which){ uint8* I=which ? intelB.data() : intelA.data(); CPed* owner=Ped(which); fillRand(I,INTEL_BUF);
                        At<CPed*>(I,0)=owner;
                        for(int k=0;k<5;++k)At<uint8*>(I,I_PRIM+4*k)=anyTask();
                        for(int k=0;k<6;++k)At<uint8*>(I,I_SEC+4*k)=anyTask();
                        At<CPed*>(I,0x30)=owner; At<CPed*>(I,0x34)=owner;
                        At<uint8*>(I,I_HIST)=next()%2 ? nullptr : Tk(int(next()%NT));
                        for(int k=1;k<4;++k)At<uint8*>(I,I_HIST+4*k)=next()%3 ? Ev(int(next()%NEV)) : nullptr;
                        At<int32>(I,I_DM)=int32(next()%12)-1; At<int32>(I,I_DM+4)=int32(next()%12)-1;
                        At<float>(I,0xBC)=uni(0.f,40.f); At<float>(I,0xC0)=uni(0.f,40.f);
                        { const uint32 m=next()%6; At<uint32>(I,I_NSCAN)=m==0 ? next() : m==1 ? 0xFFFFFFFFu : next()%20; }
                        At<float>(I,0xC8)=uni(0.f,15.f);
                        for(int k=0;k<16;++k)At<uint8*>(I,I_PEDENT+4*k)=next()%3 ? Pd(1+int(next()%(NPED-1))) : nullptr;
                        { static const int32 cs[]{-3,-2,-1,0,1,3,4,5,6,0x7FFFFFFF}; At<int32>(I,I_CNT2)=int32(next()%8); At<int32>(I,I_CNT)=cs[next()%10]; }
                        for(int c=0;c<3;++c)At<float>(I,I_LASTPOS+4*c)=uni(-6.f,6.f);
                        for(int k=0;k<3;++k){ const uint32 m=next()%5; At<uint32>(I,I_INTEREST+4*k)=m==0 ? U(Pd(int(next()%NPED))) : m==1 ? U(veh.data()) : m==2 ? U(obj.data()) : 0u; }
                    }
                    maskA=next();
                    for(auto& r:randSeq){ const uint32 m=next()%4; r=m==0 ? 16384u+next()%320 : m==1 ? 0u : m==2 ? 0x7FFFu : next()%0x8000; }
                    lightVal=next()%4==0 ? uni(0.25f,1.f) : uni(0.f,0.32f); if(next()%16==0)lightVal=std::numeric_limits<float>::quiet_NaN();
                    timeMs=next();
                    wanderRet=next()%4 ? Tk(int(next()%NT)) : nullptr;
                    findCount=int16(next()%6==0 ? 0 : next()%11); for(auto& e:findList){ const uint32 m=next()%5;
                        e=reinterpret_cast<CEntity*>(m==0 ? static_cast<void*>(veh.data()) : m==1 ? static_cast<void*>(obj.data()) : static_cast<void*>(Pd(int(next()%NPED)))); }
                    // ---- ajustes por funcion y argumentos ----
                    uint8* const A0=intelA.data(); uint8* const p0=Pd(0); uint8* const p1=Pd(1);
                    auto pedOf=[&](uint32 m)->CPed*{ return Ped(int(m%NPED)); };
                    if(nm=="ClearTasks"&&(At(p0,P_F1)&0x100)&&At<void*>(p0,P_VEH)&&!At<void*>(A0,I_PRIM+16))At<uint8*>(A0,I_PRIM+16)=Tk(int(next()%NT)); // el EXE no comprueba la tarea por defecto
                    if(nm=="GetUsingParachute"){ uint8* w=p0+P_WEAP+size_t(p0[P_WSLOT])*0x1C; if(next()%4)At(w,0)=46; if(next()%3)At(p0,P_F1)&=~1u; if(next()%3)At(p0,P_PHYS)&=~0x100u; At<void*>(p0,P_RW)=clump.data(); }
                    if(nm=="ProcessAfterProcCol")At<void*>(p0,P_RW)=next()%2 ? clump.data() : nullptr;
                    if(nm=="ProcessAfterPreRender"){ uint8* w=p0+P_WEAP+size_t(p0[P_WSLOT])*0x1C; if(next()%3==0)At(w,0)=18; }   // molotov sin FX: el camino del FX no se prueba
                    if(nm=="TestForStealthKill"){ if(next()%4)At(p1,P_F1)&=~0x100u; if(next()%3)At(p1,P_F1)&=~0x4000000u; if(next()%4)At<float>(p1,P_HEALTH)=uni(1.f,100.f);
                        if(next()%3)At<int32>(p1,P_MOVE)=int32(next()%6);
                        uint8* m1=Mat(1); uint8* m0=Mat(0); for(int c=0;c<3;++c)At<float>(m1,0x30+4*c)=At<float>(m0,0x30+4*c)+uni(-2.5f,2.5f); }
                    if(nm=="LookAtInterestingEntities"||nm=="Process"){ if(next()%4)At(p0,P_F3)&=~0x400000u; maskA=next()%2 ? next()&~1u&~4u : next(); }
                    if(nm=="ProcessStaticCounter"&&next()%5==0)At<void*>(p0,P_INTEL)=intelB.data();
                    if(nm=="CanSeeEntityWithLights"){ if(next()%3)At<int32>(p1,P_PTYPE)=int32(next()%2);
                        uint8* m1=Mat(1); uint8* m0=Mat(0); const float sc=next()%2 ? 1.f : 15.f; for(int c=0;c<3;++c)At<float>(m1,0x30+4*c)=At<float>(m0,0x30+4*c)+uni(-sc,sc); }
                    bonePos=CVector{uni(-6.f,6.f),uni(-6.f,6.f),At<float>(Mat(1),0x38)+uni(-1.f,1.f)};
                    const CVector posArg{At<float>(Mat(0),0x30)+uni(-40.f,40.f),At<float>(Mat(0),0x34)+uni(-40.f,40.f),At<float>(Mat(0),0x38)+uni(-10.f,10.f)};
                    const uint32 argA=next(), argB=next();
                    void* const self=A0; const uintptr_t Ad=fn.address;
                    const auto occ=Occupancy();
                    // Destructor: inteligencia construida por el constructor del EXE (hooks apagados) y tareas/eventos falsos
                    if(nm=="Destructor"){ std::memset(A0,0xCD,INTEL_BUF); reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(uintptr_t(0x607140))(self,Ped(0));
                        for(int k=0;k<5;++k)At<uint8*>(A0,I_PRIM+4*k)=anyTask(); for(int k=0;k<6;++k)At<uint8*>(A0,I_SEC+4*k)=anyTask();
                        for(int k=1;k<4;++k)At<uint8*>(A0,I_HIST+4*k)=next()%2 ? Ev(k+2*int(next()%2)) : nullptr;
                        ntr=0; callsA=0; callsR=0; }
                    json ret=nullptr; bool switches=false;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(Ad)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    using VF=void(__thiscall*)(void*); using BF=bool(__thiscall*)(void*); using PF=void*(__thiscall*)(void*);
                    if(nm=="Constructor"){ std::memset(A0,0xCD,INTEL_BUF); CPed* ped=Ped(int(argA%2));
                        on(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(Ad)(self,ped))); off(); }
                    else if(nm=="Destructor"||nm=="ClearTaskDuckSecondary"||nm=="ProcessAfterProcCol"||nm=="ProcessAfterPreRender"||nm=="ProcessEventHandler"
                          ||nm=="FlushIntelligence"||nm=="LookAtInterestingEntities"||nm=="ProcessStaticCounter"||nm=="ProcessFirst"||nm=="Process"
                          ||nm=="RestorePedDecisionMakerType"){ on(); reinterpret_cast<VF>(Ad)(self); off(); }
                    else if(nm=="GetPedEntities"||nm=="GetTaskFighting"||nm=="GetTaskUseGun"||nm=="GetTaskThrow"||nm=="GetTaskSwim"||nm=="GetTaskJetPack"
                          ||nm=="GetTaskInAir"||nm=="GetTaskClimb"||nm=="GetEffectInUse"||nm=="GetActivePrimaryTask"||nm=="IsInACarOrEnteringOne"){
                        on(); ret=Name(uintptr_t(reinterpret_cast<PF>(Ad)(self))); off(); }
                    else if(nm=="FindRespectedFriendInInformRange"||nm=="GetUsingParachute"||nm=="IsPedGoingForCarDoor"){ on(); ret=reinterpret_cast<BF>(Ad)(self); off(); }
                    else if(nm=="GetMoveStateFromGoToTask"){ on(); ret=reinterpret_cast<int32(__thiscall*)(void*)>(Ad)(self); off(); }
                    else if(nm=="SetPedDecisionMakerType"||nm=="SetPedDecisionMakerTypeInGroup"){ const int32 t=argA%5==0 ? 0 : int32(argA%12)-1;
                        on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(Ad)(self,t); off(); }
                    else if(nm=="SetHearingRange"||nm=="SetSeeingRange"){ on(); reinterpret_cast<void(__thiscall*)(void*,float)>(Ad)(self,uni(0.f,60.f)); off(); }
                    else if(nm=="SetEffectInUse"){ void* e=argA%4==0 ? nullptr : reinterpret_cast<void*>(uintptr_t(argB)); on(); reinterpret_cast<void(__thiscall*)(void*,void*)>(Ad)(self,e); off(); }
                    else if(nm=="IsInSeeingRange"){ on(); ret=reinterpret_cast<bool(__thiscall*)(void*,const CVector*)>(Ad)(self,&posArg); off(); }
                    else if(nm=="IsRespondingToEvent"){ static const int32 et[]{-1,9,31,32,0,5}; const int32 t=et[argA%6];
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(Ad)(self,t); off(); }
                    else if(nm=="AddTaskPhysResponse"||nm=="AddTaskEventResponseTemp"||nm=="AddTaskEventResponseNonTemp"){
                        const int slot=nm=="AddTaskPhysResponse" ? 0 : nm=="AddTaskEventResponseTemp" ? 1 : 2;
                        uint8* t=argA%6==0 ? At<uint8*>(A0,I_PRIM+4*slot) : anyTask();
                        on(); reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(Ad)(self,t,argB); off(); }
                    else if(nm=="AddTaskPrimaryMaybeInGroup"){ uint8* t=argA%8==0 ? nullptr : Tk(int(argA%NT));
                        on(); reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(Ad)(self,t,argB%3==0 ? argB : argB%2); off(); }
                    else if(nm=="FindTaskByType"){ on(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(Ad)(self,TYPES[argA%NTYPES]))); off(); }
                    else if(nm=="GetTaskHold"||nm=="GetTaskDuck"){ on(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool)>(Ad)(self,argA%2))); off(); }
                    else if(nm=="SetTaskDuckSecondary"){ on(); reinterpret_cast<void(__thiscall*)(void*,uint32)>(Ad)(self,argA); off(); }
                    else if(nm=="ClearTasks"){ on(); reinterpret_cast<void(__thiscall*)(void*,bool,bool)>(Ad)(self,argA%3!=0,argB%3!=0); off(); }
                    else if(nm=="FlushImmediately"){ on(); reinterpret_cast<void(__thiscall*)(void*,bool)>(Ad)(self,argA%2); off(); }
                    else if(nm=="IsFriendlyWith"||nm=="IsThreatenedBy"||nm=="Respects"){ CPed* other=pedOf(1+argA%(NPED-1));
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(Ad)(self,other); off(); }
                    else if(nm=="AreFriends"){ CPed* a=Ped(int(argA%2)); CPed* b=argB%4==0 ? a : pedOf(argB);
                        on(); ret=reinterpret_cast<bool(__cdecl*)(CPed*,CPed*)>(Ad)(a,b); off(); }
                    else if(nm=="TestForStealthKill"){ CPed* target=argA%8==0 ? Ped(0) : Ped(1);
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,bool)>(Ad)(self,target,argB%4==0); off(); }
                    else if(nm=="RecordEventForScript"){ static const int32 ids[]{0,32,5,9,31}; const int32 id=argA%6==5 ? int32(argA%80) : ids[argA%5];
                        const int32 prio=argB%5==0 ? -1 : argB%5==1 ? 300 : int32(argB%256);
                        on(); reinterpret_cast<void(__thiscall*)(void*,int32,int32)>(Ad)(self,id,prio); off(); }
                    else if(nm=="IsInterestingEntity"){ const uint32 m=argA%4; void* e=m==0 ? reinterpret_cast<void*>(uintptr_t(At<uint32>(A0,I_INTEREST+4*(argB%3)))) : m==1 ? nullptr : m==2 ? static_cast<void*>(Pd(int(argB%NPED))) : static_cast<void*>(veh.data());
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(Ad)(self,e); off(); }
                    else if(nm=="CanSeeEntityWithLights"){ const uint32 m=argA%8; void* e=m<5 ? static_cast<void*>(p1) : m==5 ? static_cast<void*>(veh.data()) : m==6 ? static_cast<void*>(obj.data()) : static_cast<void*>(p0);
                        on(); const float r=reinterpret_cast<float(__thiscall*)(void*,void*,int32)>(Ad)(self,e,int32(argB%2)); off(); ret=F(r); }
                    // ---- tareas reales nuevas en el pool: volcado (tipo en vez de vptr) y liberacion ----
                    json fresh=json::array();
                    { const auto now=Occupancy(); auto* stor=PoolStorage();
                      for(size_t k=0;k<now.size();++k)if(now[k]&&!occ[k]){ uint8* s=stor+k*SLOT; std::array<uint8,SLOT> b{}; std::memcpy(b.data(),s,SLOT);
                          At<int32>(b.data(),0)=int32(reinterpret_cast<CTask*>(s)->GetTaskType()); json d=Region(b.data(),SLOT); d.push_back(k);
                          fresh.push_back(d); GetTaskPool()->Delete(reinterpret_cast<CTask*>(s)); } }
                    CleanRefs();
                    json addsJ=json::array(); for(int k=0;k<nAdds;++k){ const auto& a=adds[k];
                        json d={Name(a.group),a.type,a.timeActive,a.valid,a.primary,Name(a.task),a.affects,a.validArg};
                        if(a.dumped){ auto b=a.bytes; At<uint32>(b.data(),0)=a.taskType; d.push_back(Region(b.data(),SLOT)); }
                        addsJ.push_back(d); }
                    json intelAJ=Region(intelA.data(),INTEL_BUF);
                    if(nm=="Constructor"||nm=="Destructor")for(size_t o:{size_t(0x68),size_t(0xD4),size_t(0x124)})intelAJ[o/4]="vt";   // vptr del EXE o de la DLL
                    json pedsJ=json::array(); for(int k=0;k<NPED;++k)pedsJ.push_back(Hash(Pd(k),PEDSZ));
                    json r={{"ret",ret},{"switches",switches},{"intelA",intelAJ},{"intelB",Region(intelB.data(),INTEL_BUF)},{"peds",pedsJ},
                            {"p0_flags",{At(p0,P_F1),At(p0,P_F2),At(p0,P_F3),At(p0,P_F4),At(p0,P_MOVE)}},
                            {"tasks",Region(tasks.data(),tasks.size())},{"clones",Region(clones.data(),size_t(nTc)*TSZ)},{"evs",Hash(evs.data(),evs.size())},
                            {"grps",Hash(grps.data(),grps.size())},{"veh",Hash(veh.data(),veh.size())},{"obj",Hash(obj.data(),obj.size())},
                            {"pdata",Region(pdata.data(),pdata.size())},{"frame",Region(frame.data(),frame.size())},{"mats",Hash(mats.data(),mats.size())},
                            {"trace",TraceJson()},{"overflow",trOverflow},{"adds",addsJ},{"fresh",fresh},{"time",timeMs},{"rand_calls",callsR},{"bits",callsA}};
                    r["clean"]=Occupancy()==occ;
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true&&original["overflow"]==false;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    *reinterpret_cast<int32*>(PoolHdr()+0xC)=lf0; timeMs=savedTime;
    for(size_t f=0;f<hooks.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&timeMs==savedTime&&freeRefs()==savedFree}});
}
}
