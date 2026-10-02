#pragma once
// Banco dentro del motor: tareas complejas, repartidores y peatones (lista de 52, 32 funciones).
// CTaskComplexEnterCarAsDriver/AsPassenger, CTaskComplexGoToPointShooting y CTaskComplexLeaveCarAndDie (constructores,
// Clone, GetTaskType, CreateFirstSubTask), CTaskAllocator y CTaskAllocatorPlayerCommandAttack, CCivilianPed (constructor,
// destructor con borrado y ProcessControl), CCoverPoint, CPedPlacement, CPedToPlayerConversations::Clear/EndConversation
// y CSetPiece (TryToGenerateCopPed/CopCar y Update).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// peatones, jugador, vehiculos, coches y polis generados, peaton civil, grupo, inteligencia de grupo, model infos y
// modelos de colision falsos y tareas falsas (vtable trazada). Se simulan y trazan: CTaskComplexLeaveCar::
// CreateFirstSubTask, CTaskAllocatorAttack::AllocateTasks, CPedList (BuildListFromGroup_NoLeader, RemovePeds*,
// BuildListOfPedsOfPedType), CFormation (DistributeDestinations_PedsToAttack, ReturnTargetPedForPed),
// CPedGroupIntelligence::SetTask (se clona y serializa la tarea), CPed::CPed/~CPed/SetModelIndex/ProcessControl/
// operator new/operator delete, CAEPedSpeechAudioEntity::Initialise/ReleasePlayerConversation, CWeapon::Update,
// CPed::EnablePedSpeech, IKChainManager_c::IsLooking/AbortLookAt, CWorld (FindObjectsKindaColliding,
// TestSphereAgainstWorld, ProcessVerticalLine, FindGroundZForCoord, Add, Remove), CCollision::ProcessColModels,
// FindPlayerPed/Vehicle/Coors/Speed, CPlayerPed::GetWantedLevel, CStreaming::GetDefaultCopCarModel,
// CVehicle::operator new, el constructor de CAutomobile y de CCopPed, CVehicle::ChangeLawEnforcerState,
// CCarAI::AddPoliceCarOccupants, CTaskManager::SetTask (EXE) / ChangeTaskInSlot (DLL) y el RNG.
// CTimer::m_snTimeInMilliseconds, los globales de CPedToPlayerConversations, las entradas de ms_modelInfoPtrs usadas,
// CFormation::m_DestinationPeds y los puntos de colision estaticos 0xC102A0 se guardan y se restauran en cada ejecucion.
// Se compara la memoria de los fixtures (punteros traducidos a nombres), las tareas nuevas del pool, las referencias,
// los globales tocados, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexGoToPointShooting.h"
#include "TaskComplexLeaveCarAndDie.h"
#include "TaskComplexLeaveCar.h"
#include "Tasks/Allocators/TaskAllocator.h"
#include "Tasks/Allocators/TaskAllocatorAttack.h"
#include "Tasks/Allocators/TaskAllocatorPlayerCommandAttack.h"
#include "Tasks/TaskManager.h"
#include "Entity/Ped/CivilianPed.h"
#include "Entity/Ped/CopPed.h"
#include "Entity/Vehicle/Automobile.h"
#include "CoverPoint.h"
#include "PedPlacement.h"
#include "Conversations/PedToPlayerConversations.h"
#include "SetPiece.h"
#include "PedList.h"
#include "Formation.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "Ragdoll/IKChainManager.h"
#include "Audio/entities/AEPedSpeechAudioEntity.h"
#include "Collision/Collision.h"
#include "CarAI.h"
#include "Streaming.h"
#include "World.h"
#include "Weapon.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace PTasks52Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

// acceso a CTaskManager::ChangeTaskInSlot (privada): la instanciacion explicita ignora el control de acceso
template<class Tag,typename Tag::type M> struct Rob { friend typename Tag::type Get(Tag) { return M; } };
struct ChangeTaskTag { using type=void(CTaskManager::*)(CTask*&,CTask*); friend type Get(ChangeTaskTag); };
template struct Rob<ChangeTaskTag,&CTaskManager::ChangeTaskInSlot>;

constexpr int NPEDS=8, NVEH=3, FAKES=4;
constexpr size_t FK_CLONE=0x34, FK_TYPE=0x38;
// desplazamientos crudos del EXE
constexpr size_t E_FLAGS=0x1C, E_MATRIX=0x14, E_MODEL=0x22, E_TYPE=0x36;                 // CEntity
constexpr size_t P_FLAGS2=0x46C, P_FLAGS3=0x470, P_INTEL=0x47C, P_CREATED=0x484, P_STATE=0x530, P_NEED=0x54C,
                 P_VEHICLE=0x58C, P_PEDTYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718, P_SPEECH=0x294;  // CPed
constexpr size_t V_AUTOPILOT=0x390, V_FLAGS=0x428, V_NEED=0x4D0, V_TYPE=0x590, V_SUBTYPE=0x594, V_MOVE=0x44; // CVehicle
constexpr size_t G_MEMBERS=0xC, I_PAIRS=0xC;                                              // CPedGroup, CPedGroupIntelligence
constexpr uintptr_t MI_PTRS=0xA9B0C8, TIMER=0xB7CB84, P2P_STATE=0x969A20, P2P_PED=0x9691C0, P2P_LAST=0x9691B4;
constexpr uintptr_t DEST_PEDS=0xC1A458, COLPOINTS=0xC102A0, IKMAN=0xC15448, CIV_VT=0x86C0A8;
constexpr int16 MI_PED=7, MI_VEH=400; constexpr int MI_COP0=596;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> player{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,2> cars{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,2> carMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CCopPed)>,2> cops{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,2> copMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,2> copIntel{};
alignas(16) inline std::array<uint8,sizeof(CCivilianPed)> civ{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x40> outEntities{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,sizeof(CPedGroupIntelligence)> intel{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> mis{}, cols{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uintptr_t dllCivVt{};

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}

inline std::array<void*,12> fakeVtbl{};
inline std::array<void*,80> vehVtbl{};
inline std::array<void*,32> pedVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==CIV_VT||(dllCivVt&&v==dllCivVt))return "civ_vt";
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(pedVtbl.data()))return "ped_vt";
    if(v==COLPOINTS)return "colpoints_static"; if(v==DEST_PEDS)return "dest_peds"; if(v==IKMAN)return "ik_man";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<2;++n){ if(auto s=in(cars[n].data(),cars[n].size(),"car"))return *s+"#"+std::to_string(n); if(auto s=in(carMats[n].data(),carMats[n].size(),"cmat"))return *s+"#"+std::to_string(n);
                          if(auto s=in(cops[n].data(),cops[n].size(),"cop"))return *s+"#"+std::to_string(n); if(auto s=in(copMats[n].data(),copMats[n].size(),"copmat"))return *s+"#"+std::to_string(n);
                          if(auto s=in(copIntel[n].data(),copIntel[n].size(),"copintel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<3;++n){ if(auto s=in(mis[n].data(),mis[n].size(),"mi"))return *s+"#"+std::to_string(n); if(auto s=in(cols[n].data(),cols[n].size(),"col"))return *s+"#"+std::to_string(n); }
    if(auto s=in(player.data(),player.size(),"player"))return *s; if(auto s=in(civ.data(),civ.size(),"civ"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(outEntities.data(),outEntities.size(),"out"))return *s;
    if(auto s=in(group.data(),group.size(),"group"))return *s; if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
// entidad: hash sin el nodo de referencias (sale de la lista libre global; su direccion no es comparable)
inline std::string EntHash(const void* p,size_t n) {
    std::vector<uint8> copy(static_cast<const uint8*>(p),static_cast<const uint8*>(p)+n);
    const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(p)->m_pReferences)-uintptr_t(p); std::memset(copy.data()+off,0,4); return Hash(copy.data(),n);
}

// tarea serializada sin vptr (se compara el tipo); los bytes 0x39..0x3B de CTaskComplexSequence son relleno
inline json Word(int type,size_t off,uint32 v,int depth);
inline json SerTask(const void* p,int depth) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<TASK_SLOT/4;++k)words.push_back(Word(type,k*4,w[k],depth));
    return {{"type",type},{"words",words}};
}
inline json Word(int type,size_t off,uint32 v,int depth) {
    if(type==TASK_COMPLEX_SEQUENCE&&off==0x38)return v&0xFF;
    if(depth>=0&&depth<3&&IsPoolTask(v)&&!freshIds.contains(v))return SerTask(reinterpret_cast<const void*>(uintptr_t(v)),depth+1);
    return Name(v);
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline uint32 listCount{}, removeMask{}, targetCalls{}; inline std::array<int,4> listPeds{}; inline std::array<uintptr_t,8> targetRet{};
inline uint32 postpone{};
inline std::array<int16,4> fokcCount{}; inline uint32 fokcCalls{}; inline uintptr_t sphereRet{}; inline int32 colRet{};
inline uint32 lookMask{}, lookCalls{};
inline int32 copModel{}; inline std::array<uint8,4> vertHit{}; inline std::array<float,4> vertZ{}, groundZ{}; inline uint32 vertCalls{}, groundCalls{};
inline float heightRet{}; inline uint32 wanted{}; inline uintptr_t playerVeh{}; inline CVector playerPos{}, playerSpeed{};
inline uint32 carNewCalls{}, copNewCalls{}, copFlags{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
template<int Tag,int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",Tag,N,Name(self)}); return nullptr; }
template<int Tag,size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<Tag,N>)),...); }
// vehiculos y polis generados
inline void* __fastcall FVehDelete(void* self,void*,uint32 f) { trace.push_back({"veh_delete",Name(self),f}); return self; }
inline float __fastcall FHeight(void* self,void*) { trace.push_back({"height_above_road",Name(self)}); return heightRet; }
inline void* __fastcall FPedDelete(void* self,void*,uint32 f) { trace.push_back({"ped_delete",Name(self),f}); return self; }

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// CTaskComplexLeaveCarAndDie
inline CTask* __fastcall FakeLeaveFirst(void* self,void*,CPed* p) { trace.push_back({"leave_car_first",Name(self),Name(p),At<int32>(p,P_STATE)}); return Fake(1); }
// CTaskAllocatorPlayerCommandAttack
inline json ListContents(const uint8* l) { json r=json::array(); const uint32 n=At<uint32>(l,0); r.push_back(n); for(uint32 k=0;k<n&&k<30;++k)r.push_back(Name(At<uint32>(l,4+4*k))); return r; }
inline void __fastcall FakeAttackAlloc(void* self,void*,void* in) { trace.push_back({"attack_allocate",Name(self),Name(in)}); }
// la lista vive en la pila (direccion no comparable): solo se traza su contenido
inline void __fastcall FakeBuildList(uint8* l,void*,void* membership) {
    trace.push_back({"build_list",Name(membership)}); At<uint32>(l,0)=listCount; for(uint32 k=0;k<listCount;++k)At<CPed*>(l,4+4*k)=Ped(listPeds[k]);
}
inline void __fastcall FakeRemoveNoListen(uint8* l,void*) { trace.push_back({"remove_dont_listen",ListContents(l)}); if((removeMask&1)&&At<uint32>(l,0))--At<uint32>(l,0); }
inline void __fastcall FakeRemoveAttacking(uint8* l,void*,int32 type) {
    trace.push_back({"remove_attacking",type,ListContents(l)});
    auto& n=At<uint32>(l,0); if((removeMask&2)&&n>1){ for(uint32 k=1;k<n;++k)At<uint32>(l,4*k)=At<uint32>(l,4+4*k); --n; }
}
inline void __fastcall FakeBuildOfType(uint8* l,void*,int32 type) { trace.push_back({"build_of_type",Name(l),type}); }
inline void __cdecl FakeDistribute(const uint8* l) { trace.push_back({"distribute",ListContents(l)}); }
inline void __cdecl FakeReturnTarget(CPed* p,CPed** out) { trace.push_back({"return_target",Name(p)}); *out=reinterpret_cast<CPed*>(targetRet[targetCalls++%8]); }
// la tarea llega en la pila (bytes sin escribir no comparables): se clona, se serializa y se borra el clon
inline void __fastcall FakeGroupSetTask(void* in,void*,CPed* ped,const CTask* task,void* pairs,int32 slot,uint32 force) {
    json r={"group_set_task",Name(in),Name(ped),Name(pairs),slot,uint8(force)};
    if(task){ CTask* c=task->Clone(); r.push_back(SerTask(c,0)); delete c; }
    trace.push_back(r);
}
// CCivilianPed
inline void* __fastcall FakePedCtor(uint8* self,void*,int32 type) { trace.push_back({"ped_ctor",Name(self),type}); return self; }
inline void __fastcall FakeSetModel(void* self,void*,uint32 idx) { trace.push_back({"set_model_index",Name(self),idx}); }
inline void __fastcall FakeSpeechInit(void* self,void*,void* e) { trace.push_back({"speech_initialise",Name(self),Name(e)}); }
inline void __fastcall FakePedDtor(void* self,void*) { trace.push_back({"ped_dtor",Name(self),Name(At<uint32>(self,0))}); }
inline void __cdecl FakePedOpDelete(void* p) { trace.push_back({"ped_operator_delete",Name(p)}); }
inline void __fastcall FakePedProcess(uint8* self,void*) {
    trace.push_back({"ped_process_control",Name(self)});
    auto& f=At<uint32>(self,E_FLAGS); if(postpone==1)f|=0x40u; else if(postpone==2)f&=~0x40u;
}
inline void __fastcall FakeWeaponUpdate(void* w,void*,CPed* p) { trace.push_back({"weapon_update",Name(w),Name(p)}); }
// CCoverPoint / CPedPlacement
inline void __cdecl FakeFokc(const CVector& p,float r,bool b2D,int16* cnt,int16 maxCount,CEntity** out,bool bld,bool veh,bool pd,bool obj,bool dum) {
    trace.push_back({"fokc",F(p.x),F(p.y),F(p.z),F(r),b2D,maxCount,Name(out),bld,veh,pd,obj,dum}); *cnt=fokcCount[fokcCalls++%4];
}
inline CEntity* __cdecl FakeSphere(CVector c,float r,CEntity* ign,bool b,bool v,bool p,bool o,bool d,bool cam) {
    trace.push_back({"test_sphere",F(c.x),F(c.y),F(c.z),F(r),Name(ign),b,v,p,o,d,cam}); return reinterpret_cast<CEntity*>(sphereRet);
}
inline int32 __cdecl FakeColModels(const CMatrix& ma,CColModel& ca,const CMatrix& mb,CColModel& cb,void* sp,CColPoint* lp,float* md,bool a7) {
    trace.push_back({"process_col_models",Name(&ma),Name(&ca),Name(&mb),Name(&cb),Name(sp),Name(lp),Name(md),a7}); return colRet;
}
// CPedToPlayerConversations
inline void __cdecl FakeRelease() { trace.push_back({"release_player_conversation"}); }
inline void __fastcall FakeEnableSpeech(void* p,void*) { trace.push_back({"enable_ped_speech",Name(p)}); }
inline bool __fastcall FakeIsLooking(void* ik,void*,CPed* p) { const bool r=(lookMask>>(lookCalls++%32))&1; trace.push_back({"is_looking",Name(ik),Name(p),r}); return r; }
inline void __fastcall FakeAbortLook(void* ik,void*,CPed* p,uint32 t) { trace.push_back({"abort_look_at",Name(ik),Name(p),t}); }
// jugador
inline CPlayerPed* __cdecl FakeFindPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(player.data()); }
inline CVehicle* __cdecl FakeFindVeh(int32 id,bool b) { trace.push_back({"find_player_vehicle",id,b}); return reinterpret_cast<CVehicle*>(playerVeh); }
// sin traza: el registro de depuracion NOTSA de TryToGenerateCop* tambien lee la posicion del jugador
inline CVector __cdecl FakeCoors(int32) { return playerPos; }
inline CVector& __cdecl FakeSpeed(int32) { return playerSpeed; }
inline uint32 __fastcall FakeWanted(void* p,void*) { trace.push_back({"wanted",Name(p)}); return wanted; }
// CSetPiece
inline int32 __cdecl FakeCopModel(uint32 ignore) { trace.push_back({"default_cop_car_model",uint8(ignore)}); return copModel; }
inline void* __cdecl FakeVehNew(unsigned n) { trace.push_back({"vehicle_new",n}); return cars[carNewCalls++%2].data(); }
inline void* __fastcall FakeAutoCtor(uint8* self,void*,int32 model,int32 createdBy,uint32 setup) {
    trace.push_back({"automobile_ctor",Name(self),model,createdBy,uint8(setup)});
    const int k=self==cars[1].data() ? 1 : 0; std::memset(self,0,sizeof(CAutomobile)); std::memset(carMats[k].data(),0xAB,carMats[k].size());
    At<void*>(self,0)=vehVtbl.data(); At<void*>(self,E_MATRIX)=carMats[k].data(); At<int16>(self,E_MODEL)=int16(model); self[E_TYPE]=2;
    return self;
}
inline bool __cdecl FakeVert(const CVector& o,float d,CColPoint& cp,CEntity*& e,bool b,bool v,bool p,bool ob,bool du,bool see,void* poly) {
    const uint32 k=vertCalls++%4; trace.push_back({"vertical_line",F(o.x),F(o.y),F(o.z),F(d),b,v,p,ob,du,see,Name(poly),vertHit[k]});
    if(vertHit[k])At<float>(&cp,8)=vertZ[k]; return vertHit[k]!=0;
}
inline void __fastcall FakeLawEnforcer(void* self,void*,uint32 b) { trace.push_back({"law_enforcer_state",Name(self),uint8(b)}); }
inline void __cdecl FakeWorldAdd(CEntity* e) { trace.push_back({"world_add",Name(e)}); }
inline void __cdecl FakeWorldRemove(CEntity* e) { trace.push_back({"world_remove",Name(e)}); }
inline void* __cdecl FakePedNew(unsigned n) { trace.push_back({"ped_new",n}); return cops[copNewCalls++%2].data(); }
inline void* __fastcall FakeCopCtor(uint8* self,void*,uint32 type) {
    trace.push_back({"cop_ctor",Name(self),type});
    const int k=self==cops[1].data() ? 1 : 0; std::memset(self,0,sizeof(CCopPed)); std::memset(copMats[k].data(),0xAB,copMats[k].size()); std::memset(copIntel[k].data(),0,copIntel[k].size());
    At<void*>(self,0)=pedVtbl.data(); At<void*>(self,E_MATRIX)=copMats[k].data(); At<void*>(self,P_INTEL)=copIntel[k].data();
    At<uint32>(self,P_FLAGS3)=copFlags; At<uint32>(self,P_NEED)=0xA5A5A5A5u; self[E_TYPE]=3;
    return self;
}
inline float __cdecl FakeGroundZ(float x,float y) { trace.push_back({"ground_z",F(x),F(y)}); return groundZ[groundCalls++%4]; }
inline void __cdecl FakePoliceOccupants(CVehicle* v,bool b) { trace.push_back({"police_occupants",Name(v),b}); }
// CTaskManager::SetTask (EXE, ret 0xC) y CTaskManager::ChangeTaskInSlot (DLL, en linea en SetTask): se serializa y se borra
inline void SetTaskImpl(uint8* tm,int32 slot,CTask* task) { trace.push_back({"set_task",Name(tm),slot,task ? SerTask(task,0) : json(nullptr)}); delete task; }
inline void __fastcall FakeExeSetTask(uint8* tm,void*,CTask* task,int32 slot,uint32) { SetTaskImpl(tm,slot,task); }
inline void __fastcall FakeDllChangeTask(uint8* tm,void*,CTask** ref,CTask* task) { SetTaskImpl(tm,int32((uintptr_t(ref)-uintptr_t(tm))/4),task); }
inline uintptr_t LocalAddress(HMODULE module,const char* name) { auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name)); if(p&&*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1); return p; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=32;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexEnterCarAsDriver","Clone",0,0x86EAAC,1,256},
    {"CTaskComplexEnterCarAsDriver","Constructor",0x6402F0,0,-1,256},
    {"CTaskComplexEnterCarAsDriver","GetTaskType",0,0x86EAAC,4,64},
    {"CTaskComplexEnterCarAsPassenger","Clone",0,0x86EADC,1,256},
    {"CTaskComplexEnterCarAsPassenger","Constructor",0x640340,0,-1,256},
    {"CTaskComplexEnterCarAsPassenger","GetTaskType",0,0x86EADC,4,64},
    {"CTaskComplexGoToPointShooting","Clone",0,0x86FE2C,1,512},
    {"CTaskComplexGoToPointShooting","Constructor",0x668C70,0,-1,512},
    {"CTaskComplexGoToPointShooting","GetTaskType",0,0x86FE2C,4,64},
    {"CTaskComplexLeaveCarAndDie","Clone",0,0x86DD00,1,256},
    {"CTaskComplexLeaveCarAndDie","CreateFirstSubTask",0,0x86DD00,9,256},
    {"CTaskComplexLeaveCarAndDie","GetTaskType",0,0x86DD00,4,64},
    {"CTaskAllocator","Abort",0,0x870E30,4,64},
    {"CTaskAllocator","IsFinished",0,0x870E30,3,256},
    {"CTaskAllocator","ProcessGroup",0,0x870E30,2,64},
    {"CTaskAllocatorPlayerCommandAttack","AllocateTasks",0,0x870E78,1,1024},
    {"CTaskAllocatorPlayerCommandAttack","GetType",0,0x870E78,5,64},
    {"CTaskAllocatorPlayerCommandAttack","ProcessGroup",0,0x870E78,2,512},
    {"CCivilianPed","Constructor",0x5DDB70,0,-1,256},
    {"CCivilianPed","Destructor",0x5DDC30,0,-1,256},
    {"CCivilianPed","ProcessControl",0,0x86C0A8,10,512},
    {"CCoverPoint","CanAccommodateAnotherPed",0x698E70,0,-1,256},
    {"CCoverPoint","ReleaseCoverPointForPed",0x698EF0,0,-1,512},
    {"CCoverPoint","ReserveCoverPointForPed",0x698EB0,0,-1,512},
    {"CPedPlacement","IsPositionClearForPed-OG",0x616860,0,-1,512},
    {"CPedPlacement","IsPositionClearOfCars-Ped",0x616A40,0,-1,512},
    {"CPedPlacement","IsPositionClearOfCars-Pos",0x6168E0,0,-1,256},
    {"CPedToPlayerConversations","Clear",0x43AAE0,0,-1,128},
    {"CPedToPlayerConversations","EndConversation",0x43AB10,0,-1,512},
    {"CSetPiece","TryToGenerateCopCar",0x4998A0,0,-1,512},
    {"CSetPiece","TryToGenerateCopPed",0x499690,0,-1,512},
    {"CSetPiece","Update",0x499A80,0,-1,1024},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data()); for(auto& c:cars)clean(c.data()); for(auto& c:cops)clean(c.data());
    clean(player.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ptasks52-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ptasks52-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ptasks52");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ptasks52-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    HMODULE module{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    const std::array<std::pair<const char*,uintptr_t>,10> exports{{
        {"?CreateFirstSubTask@CTaskComplexLeaveCar@@UAEPAVCTask@@PAVCPed@@@Z",0},
        {"?AllocateTasks@CTaskAllocatorAttack@@UAEXPAVCPedGroupIntelligence@@@Z",0},
        {"??0CPed@@QAE@W4ePedType@@@Z",0},
        {"?SetModelIndex@CPed@@UAEXI@Z",0},
        {"?Initialise@CAEPedSpeechAudioEntity@@QAEXPAVCEntity@@@Z",0},
        {"??1CPed@@UAE@XZ",0},
        {"??3CPed@@SAXPAX@Z",0},
        {"?ProcessControl@CPed@@UAEXXZ",0},
        {"??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z",0},
        {"??0CCopPed@@QAE@I@Z",0},
    }};
    std::array<uintptr_t,10> ex{}; json missing=json::array();
    for(size_t k=0;k<exports.size();++k)if(!(ex[k]=LocalAddress(module,exports[k].first)))missing.push_back(exports[k].first);
    dllCivVt=reinterpret_cast<uintptr_t>(GetProcAddress(module,"??_7CCivilianPed@@6B@"));
    if(!missing.empty()){Write({{"event","error"},{"reason","missing dll export"},{"exports",missing}});return;}
    FillTraps<0>(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps<1>(vehVtbl,std::make_integer_sequence<int,80>{}); FillTraps<2>(pedVtbl,std::make_integer_sequence<int,32>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    vehVtbl[0]=reinterpret_cast<void*>(&FVehDelete); vehVtbl[0xD4/4]=reinterpret_cast<void*>(&FHeight); pedVtbl[0]=reinterpret_cast<void*>(&FPedDelete);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{P2P_STATE,4},{P2P_PED,4},{P2P_LAST,4},{DEST_PEDS,sizeof(CPedList)},{COLPOINTS,32*sizeof(CColPoint)},
                                                    {MI_PTRS+4*MI_PED,4},{MI_PTRS+4*MI_VEH,4}};
    for(int k=0;k<4;++k)regions.push_back({MI_PTRS+4*(MI_COP0+k),4});
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch t1(ex[0],FakeLeaveFirst), t1e(0x641FC0,FakeLeaveFirst);
        EntryPatch a1(ex[1],FakeAttackAlloc), a1e(0x69C3F0,FakeAttackAlloc);
        EntryPatch a2(bit_cast<uintptr_t>(&CPedList::BuildListFromGroup_NoLeader),FakeBuildList), a2e(0x699DD0,FakeBuildList);
        EntryPatch a3(bit_cast<uintptr_t>(&CPedList::RemovePedsThatDontListenToPlayer),FakeRemoveNoListen), a3e(0x69A420,FakeRemoveNoListen);
        EntryPatch a4(bit_cast<uintptr_t>(&CPedList::RemovePedsAttackingPedType),FakeRemoveAttacking), a4e(0x69A450,FakeRemoveAttacking);
        EntryPatch a5(bit_cast<uintptr_t>(&CPedList::BuildListOfPedsOfPedType),FakeBuildOfType), a5e(0x69A3B0,FakeBuildOfType);
        EntryPatch a6(bit_cast<uintptr_t>(&CFormation::DistributeDestinations_PedsToAttack),FakeDistribute), a6e(0x69B700,FakeDistribute);
        EntryPatch a7(bit_cast<uintptr_t>(&CFormation::ReturnTargetPedForPed),FakeReturnTarget), a7e(0x699F50,FakeReturnTarget);
        EntryPatch a8(bit_cast<uintptr_t>(&CPedGroupIntelligence::SetTask),FakeGroupSetTask), a8e(0x5F7540,FakeGroupSetTask);
        EntryPatch c1(ex[2],FakePedCtor), c1e(0x5E8030,FakePedCtor);
        EntryPatch c2(ex[3],FakeSetModel), c2e(0x5E4880,FakeSetModel);
        EntryPatch c3(ex[4],FakeSpeechInit), c3e(0x4E68D0,FakeSpeechInit);
        EntryPatch c4(ex[5],FakePedDtor), c4e(0x5E8620,FakePedDtor);
        EntryPatch c5(ex[6],FakePedOpDelete), c5e(0x5E4760,FakePedOpDelete);
        EntryPatch c6(ex[7],FakePedProcess), c6e(0x5E8CD0,FakePedProcess);
        EntryPatch c7(bit_cast<uintptr_t>(&CWeapon::Update),FakeWeaponUpdate), c7e(0x73DB40,FakeWeaponUpdate);
        EntryPatch w1(bit_cast<uintptr_t>(&CWorld::FindObjectsKindaColliding),FakeFokc), w1e(0x568B80,FakeFokc);
        EntryPatch w2(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),FakeSphere), w2e(0x569E20,FakeSphere);
        EntryPatch w3(bit_cast<uintptr_t>(&CCollision::ProcessColModels),FakeColModels), w3e(0x4185C0,FakeColModels);
        EntryPatch w4(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVert), w4e(0x5674E0,FakeVert);
        EntryPatch w5(bit_cast<uintptr_t>(&CWorld::FindGroundZForCoord),FakeGroundZ), w5e(0x569660,FakeGroundZ);
        EntryPatch w6(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), w6e(0x563220,FakeWorldAdd);
        EntryPatch w7(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), w7e(0x563280,FakeWorldRemove);
        EntryPatch v1(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::ReleasePlayerConversation),FakeRelease), v1e(0x4E3960,FakeRelease);
        EntryPatch v2(bit_cast<uintptr_t>(&CPed::EnablePedSpeech),FakeEnableSpeech), v2e(0x5EFF70,FakeEnableSpeech);
        EntryPatch v3(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),FakeIsLooking), v3e(0x6181A0,FakeIsLooking);
        EntryPatch v4(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),FakeAbortLook), v4e(0x618280,FakeAbortLook);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPed), p1e(0x56E210,FakeFindPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindVeh), p2e(0x56E0D0,FakeFindVeh);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), p3e(0x56E010,FakeCoors);
        EntryPatch p4(bit_cast<uintptr_t>(static_cast<CVector&(*)(int32)>(&FindPlayerSpeed)),FakeSpeed), p4e(0x56E090,FakeSpeed);
        EntryPatch p5(bit_cast<uintptr_t>(&CPlayerPed::GetWantedLevel),FakeWanted), p5e(0x41BE60,FakeWanted);
        EntryPatch s1(bit_cast<uintptr_t>(&CStreaming::GetDefaultCopCarModel),FakeCopModel), s1e(0x407C50,FakeCopModel);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CVehicle::operator new)),FakeVehNew), s2e(0x6E2D50,FakeVehNew);
        EntryPatch s3(ex[8],FakeAutoCtor), s3e(0x6B0A90,FakeAutoCtor);
        EntryPatch s4(bit_cast<uintptr_t>(&CVehicle::ChangeLawEnforcerState),FakeLawEnforcer), s4e(0x6D2330,FakeLawEnforcer);
        EntryPatch s5(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CPed::operator new)),FakePedNew), s5e(0x5E4720,FakePedNew);
        EntryPatch s6(ex[9],FakeCopCtor), s6e(0x5DDC60,FakeCopCtor);
        EntryPatch s7(bit_cast<uintptr_t>(&CCarAI::AddPoliceCarOccupants),FakePoliceOccupants), s7e(0x41C070,FakePoliceOccupants);
        EntryPatch s8(bit_cast<uintptr_t>(Get(ChangeTaskTag{})),FakeDllChangeTask), s8e(0x681AF0,FakeExeSetTask);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // slots libres con 0xCD: los bytes que el constructor no escribe quedan iguales en ambas ejecuciones
                    { auto& lf=At<int32>(PoolHdr(),0xC); static const int32 lf0=lf; lf=lf0;
                      for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5252u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    targetCalls=0; fokcCalls=0; lookCalls=0; vertCalls=0; groundCalls=0; carNewCalls=0; copNewCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    const uint32 now=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=now;
                    // peatones, jugador y vehiculos
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        p[E_TYPE]=3; At<int16>(p,E_MODEL)=MI_PED;
                        if(next()%3){ At<void*>(p,E_MATRIX)=pedMats[n].data(); At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; }
                        else At<CVector>(p,4)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{0,1,4,5,7,8,6}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                        { static const int32 states[]{1,1,54,55,0}; At<int32>(p,P_STATE)=states[next()%5]; }
                    }
                    std::memset(player.data(),0,player.size()); player[E_TYPE]=3;
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        v[E_TYPE]=2; At<int16>(v,E_MODEL)=MI_VEH; At<void*>(v,E_MATRIX)=vehMats[n].data();
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                    }
                    for(auto& c:cars)std::memset(c.data(),0,c.size()); for(auto& c:cops)std::memset(c.data(),0,c.size());
                    for(auto& m:carMats)std::memset(m.data(),0,m.size()); for(auto& m:copMats)std::memset(m.data(),0,m.size()); for(auto& m:copIntel)std::memset(m.data(),0,m.size());
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; }
                    // model infos falsos (m_pColModel en +0x14) y modelos de colision (radio de la esfera en +0x24)
                    for(int n=0;n<3;++n){ std::memset(mis[n].data(),0,0x40); std::memset(cols[n].data(),0,0x40); At<void*>(mis[n].data(),0x14)=cols[n].data(); At<float>(cols[n].data(),0x24)=uni(0.5f,6.f); }
                    At<void*>(reinterpret_cast<void*>(MI_PTRS),4*MI_PED)=mis[0].data(); At<void*>(reinterpret_cast<void*>(MI_PTRS),4*MI_VEH)=mis[1].data();
                    for(int k=0;k<4;++k)At<void*>(reinterpret_cast<void*>(MI_PTRS),4*(MI_COP0+k))=mis[2].data();
                    std::memset(object.data(),0,object.size()); std::memset(outEntities.data(),0xEE,outEntities.size());
                    std::memset(group.data(),0,group.size()); std::memset(intel.data(),0,intel.size()); std::memset(civ.data(),0,civ.size());

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches},{"now",now}}; json ret=nullptr; std::vector<CTask*> roots;
                    auto* o=object.data();
                    auto collectFresh=[&](CTask* r){
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t,-1)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    auto pickEntity=[&]()->CEntity*{ switch(next()%4){ case 0: return nullptr; case 1: return Veh(0); default: return Ped(1); } };
                    auto setPiece=[&]{
                        // CSetPiece: tiempo, rectangulo alrededor del jugador, salidas/destinos y tipo
                        playerPos=CVector{next()%3 ? uni(-100.f,100.f) : float(int(next()%800)-400)*0.25f, next()%3 ? uni(-100.f,100.f) : float(int(next()%800)-400)*0.25f, uni(0.f,20.f)};
                        playerSpeed=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        { static const int32 dts[]{0,40001,40000,39999,100000,1000,-5}; const uint32 k=next()%8; At<int32>(o,0)=k==7 ? 0 : int32(now-uint32(dts[k])); }
                        auto q=[&](float v){ return int16(std::clamp(v*4.f,-32000.f,32000.f)); };
                        const uint32 m=next()%5; // 0 dentro, 1-3 con un lado fuera/limite, 4 aleatorio
                        At<int16>(o,4)=q(playerPos.x-(m==1 ? -1.f : uni(0.f,20.f))); At<int16>(o,8)=q(playerPos.x+(m==2 ? -1.f : uni(0.f,20.f)));
                        At<int16>(o,6)=q(playerPos.y-(m==3 ? -1.f : uni(0.f,20.f))); At<int16>(o,0xA)=q(playerPos.y+uni(0.f,20.f));
                        if(m==4)for(size_t b=4;b<0xC;b+=2)At<int16>(o,b)=int16(next());
                        if(next()%6==0){ At<int16>(o,4)=q(playerPos.x); At<int16>(o,8)=q(playerPos.x); }
                        for(size_t b=0xC;b<0x1C;b+=2)At<int16>(o,b)=q((b&2 ? playerPos.y : playerPos.x)+uni(-60.f,60.f));
                        if(next()%10==0){ At<int16>(o,0x14)=At<int16>(o,0xC); At<int16>(o,0x16)=At<int16>(o,0xE); }
                        o[0x1C]=uint8(next()%10); o[0x1D]=uint8(next()); o[0x1E]=uint8(next()); o[0x1F]=uint8(next());
                        wanted=next()%4; playerVeh=next()%2 ? uintptr_t(Veh(2)) : 0;
                        copModel=next()%6==0 ? -1 : MI_COP0+int32(next()%4);
                        for(int k=0;k<4;++k){ vertHit[k]=uint8(next()%3!=0); vertZ[k]=uni(-10.f,40.f); groundZ[k]=uni(-10.f,40.f); static const int16 cs[]{0,0,0,1,2,16}; fokcCount[k]=cs[next()%6]; }
                        heightRet=uni(0.2f,2.f); copFlags=next();
                    };

                    if(nm=="GetTaskType"||nm=="GetType") {
                        At<uintptr_t>(o,0)=fn.vt; for(size_t b=4;b<0x40;++b)o[b]=uint8(next());
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                    }
                    else if(nm=="Clone") {
                        At<uintptr_t>(o,0)=fn.vt; for(size_t b=4;b<0x60;++b)o[b]=uint8(next()); At<uint32>(o,4)=0; At<uint32>(o,8)=0;
                        if(cls=="CTaskComplexGoToPointShooting"){
                            At<int32>(o,0xC)=int32(next()%8); At<CEntity*>(o,0x10)=pickEntity();
                            At<CVector>(o,0x14)=CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,30.f)};
                            const bool cd=next()%4==0; const float cdf=std::bit_cast<float>(0xCDCDCDCDu);
                            At<CVector>(o,0x20)=cd ? CVector{cdf,cdf,next()%3 ? cdf : 1.f} : CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,30.f)};
                            At<float>(o,0x2C)=cd&&next()%3 ? cdf : uni(0.f,4.f); At<float>(o,0x30)=cd&&next()%3 ? cdf : uni(0.f,8.f);
                        } else {
                            At<CVehicle*>(o,0xC)=next()%5 ? Veh(0) : nullptr;
                            if(cls=="CTaskComplexLeaveCarAndDie")At<CTask*>(o,0x1C)=nullptr; // m_pTaskUtilityLineUpPedWithCar
                            else At<void*>(o,0x38)=nullptr;                                  // m_LineUpUtility
                        }
                        auto* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o);
                        collectFresh(r); ret=Name(r);
                        out["refs"]=json::array({RefList(Veh(0)),RefList(Ped(1))});
                    }
                    else if(cls=="CTaskComplexEnterCarAsDriver"||cls=="CTaskComplexEnterCarAsPassenger") {
                        std::memset(o,0xCD,0x50); CVehicle* v=next()%5 ? Veh(0) : nullptr; void* r{};
                        if(cls=="CTaskComplexEnterCarAsDriver") r=reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v);
                        else { const int32 seat=int32(next()%10)-1; const uint32 carry=next()%2; out["args"]={seat,carry};
                               r=reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,uint32)>(target)(o,v,seat,carry); }
                        for(auto* h:hooks)h->State(false);
                        ret=Name(r); out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType(); out["object"]=Region(o,0x50,true); out["refs"]=RefList(Veh(0));
                    }
                    else if(cls=="CTaskComplexGoToPointShooting") {
                        std::memset(o,0xCD,0x38);
                        const float cdf=std::bit_cast<float>(0xCDCDCDCDu); const uint32 mode=next()%6; // 0-1 igual al hueco 0xCD (total o parcial)
                        const int32 ms=int32(next()%8); CEntity* e=pickEntity();
                        CVector mp=mode<2 ? CVector{cdf,cdf,mode==0||next()%2 ? cdf : 2.f} : CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,30.f)};
                        const CVector aim{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,30.f)};
                        const float rad=mode==0||(mode==1&&next()%2) ? cdf : uni(0.f,4.f), slow=mode==0||(mode==1&&next()%2) ? cdf : uni(0.f,8.f);
                        auto* r=reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,CEntity*,CVector,float,float)>(target)(o,ms,&mp,e,aim,rad,slow);
                        for(auto* h:hooks)h->State(false);
                        ret=Name(r); out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType(); out["object"]=Region(o,0x38,true);
                        out["refs"]=json::array({RefList(Veh(0)),RefList(Ped(1))});
                    }
                    else if(cls=="CTaskComplexLeaveCarAndDie") { // CreateFirstSubTask
                        At<uintptr_t>(o,0)=fn.vt; for(size_t b=4;b<0x34;++b)o[b]=uint8(next());
                        auto* p=Ped(0); { static const int32 states[]{1,54,55,0,50}; At<int32>(p,P_STATE)=states[next()%5]; }
                        auto* r=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,p);
                        ret=Name(r); out["ped_state"]=At<int32>(p,P_STATE); out["object"]=Region(o,0x34,true);
                    }
                    else if(cls=="CTaskAllocator") {
                        At<uintptr_t>(o,0)=fn.vt; for(size_t b=4;b<0x20;++b)o[b]=uint8(next());
                        const uint32 mode=next()%4;
                        for(int k=0;k<8;++k){ auto* pp=intel.data()+I_PAIRS+0x14*k; At<CPed*>(pp,0)=Ped(k);
                            At<CTask*>(pp,4)=mode==0 ? nullptr : (next()%(mode+2)==0 ? Fake(k%FAKES) : nullptr); At<int32>(pp,8)=int32(next()%8)-1; }
                        if(nm=="Abort") reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else if(nm=="IsFinished") ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(target)(o,intel.data());
                        else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,intel.data()));
                        out["object"]=Region(o,0x20,true);
                    }
                    else if(cls=="CTaskAllocatorPlayerCommandAttack") {
                        At<uintptr_t>(o,0)=fn.vt; At<CPed*>(o,4)=next()%4 ? Ped(5) : nullptr; At<int32>(o,8)=int32(next()%10)-1; At<CPed*>(o,0xC)=next()%2 ? Ped(6) : nullptr;
                        At<uint32>(o,0x10)=now-next()%6000; { static const int32 iv[]{3000,0,-1,1000,5000,100}; At<int32>(o,0x14)=iv[next()%6]; }
                        o[0x18]=uint8(next()%3!=0); o[0x19]=uint8(next()%4==0); o[0x1A]=uint8(next()); o[0x1B]=uint8(next());
                        { static const int32 types[]{-1,-1,-5,0,1,4,6,7,8,20}; At<int32>(o,0x1C)=types[next()%10]; }
                        // inteligencia: grupo falso (lider = miembro 7 = ped#7) y pares principales
                        At<void*>(intel.data(),0)=group.data();
                        for(int k=0;k<8;++k)At<CPed*>(group.data(),G_MEMBERS+4*k)=next()%3 ? Ped(k) : nullptr;
                        At<CPed*>(group.data(),G_MEMBERS+28)=next()%4 ? Ped(7) : nullptr;
                        { static const int32 lt[]{0,1,4,7}; At<int32>(Ped(7),P_PEDTYPE)=lt[next()%4]; }
                        for(int k=0;k<8;++k){ auto* pp=intel.data()+I_PAIRS+0x14*k; At<CPed*>(pp,0)=Ped(k); At<CTask*>(pp,4)=next()%3==0 ? Fake(k%FAKES) : nullptr; At<int32>(pp,8)=-1; }
                        for(int k=0;k<5;++k){ auto* p=Ped(k); At<uint32>(p,P_FLAGS2)=next()%3==0 ? (next()|0x100u) : (next()&~0x100u); At<CVehicle*>(p,P_VEHICLE)=next()%3 ? Veh(0) : nullptr; }
                        listCount=next()%5; for(auto& lp:listPeds)lp=int(next()%5); removeMask=next()%4;
                        for(auto& t:targetRet){ const uint32 m=next()%4; t=m==0 ? 0 : m==1 ? uintptr_t(Ped(6)) : uintptr_t(Ped(5)); }
                        if(nm=="AllocateTasks") reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(o,intel.data());
                        else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,intel.data()));
                        for(auto* h:hooks)h->State(false);
                        out["object"]=Region(o,0x20,true); out["intel"]=Region(intel.data(),0x30);
                        out["refs"]=json::array({RefList(Veh(0)),RefList(Ped(5)),RefList(Ped(6))});
                    }
                    else if(cls=="CCivilianPed") {
                        auto* c=civ.data();
                        if(nm=="Constructor"){
                            std::memset(c,0xCD,civ.size()); const int32 type=int32(next()%24); const uint32 model=next()%300;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(target)(c,type,model));
                        } else if(nm=="Destructor"){
                            for(size_t b=0;b<civ.size();++b)c[b]=uint8(next()); At<uintptr_t>(c,0)=CIV_VT; reinterpret_cast<CEntity*>(c)->m_pReferences=nullptr;
                            static const uint32 flags[]{0,1,2,3,0x101}; const uint32 fl=flags[next()%5]; out["flags"]=fl;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(c,fl));
                        } else {
                            At<uintptr_t>(c,0)=CIV_VT; c[E_TYPE]=3; At<uint32>(c,E_FLAGS)=next(); c[P_CREATED]=uint8(next()%5);
                            { static const int32 states[]{1,54,55,0,55}; At<int32>(c,P_STATE)=states[next()%5]; }
                            c[P_SLOT]=uint8(next()%13); postpone=next()%3;
                            reinterpret_cast<void(__thiscall*)(void*)>(target)(c);
                            out["flags"]=At<uint32>(c,E_FLAGS);
                        }
                        for(auto* h:hooks)h->State(false);
                        out["vt"]=Name(At<uint32>(c,0)); out["civ"]=Region(c,civ.size(),true);
                        { std::vector<uint8> copy(civ.begin(),civ.end()); std::memset(copy.data(),0,4); out["hash"]=Hash(copy.data(),copy.size()); }
                        std::memset(c,0,civ.size()); // el constructor simulado deja m_pReferences con 0xCD
                    }
                    else if(cls=="CCoverPoint") {
                        static const int8 types[]{-1,0,1,2,3,3,2,4}; o[0]=uint8(types[next()%8]); o[1]=uint8(next()%3); o[2]=uint8(next());
                        At<CVector>(o,4)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; At<CEntity*>(o,0x10)=nullptr;
                        auto pick=[&]()->CPed*{ const uint32 m=next()%5; return m==0 ? nullptr : Ped(int(m-1)); };
                        CPed* s0=next()%3 ? pick() : nullptr; CPed* s1=next()%2 ? pick() : nullptr;
                        // FIX_BUGS (CanAccommodateAnotherPed, VEHICLE): el banco no entra en el camino con el segundo hueco ocupado
                        if(nm=="CanAccommodateAnotherPed"&&int8(o[0])==2)s1=nullptr;
                        At<CPed*>(o,0x14)=s0; At<CPed*>(o,0x18)=s1;
                        if(s0)static_cast<CEntity*>(s0)->RegisterReference(reinterpret_cast<CEntity**>(o+0x14));
                        if(s1)static_cast<CEntity*>(s1)->RegisterReference(reinterpret_cast<CEntity**>(o+0x18));
                        if(nm=="CanAccommodateAnotherPed") ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o);
                        else { const uint32 m=next()%4; CPed* arg=m==0 ? s0 : m==1 ? s1 : pick(); out["arg"]=Name(arg);
                               reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,arg); }
                        out["object"]=Region(o,0x1C); json refs=json::array(); for(int n=0;n<4;++n)refs.push_back(RefList(Ped(n))); out["refs"]=refs;
                    }
                    else if(cls=="CPedPlacement") {
                        for(int k=0;k<4;++k){ static const int16 cs[]{0,0,1,2}; fokcCount[k]=cs[next()%4]; }
                        if(nm=="IsPositionClearForPed-OG"){
                            const CVector pos{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,30.f)};
                            const float radius=next()%3==0 ? -1.f : next()%5==0 ? 0.f : uni(0.1f,3.f);
                            static const int32 maxs[]{-1,-1,1,2,3,4}; const int32 mx=maxs[next()%6];
                            CEntity** outE=next()%3 ? reinterpret_cast<CEntity**>(outEntities.data()) : nullptr;
                            const bool bv=next()%2, bp=next()%2, bo=next()%2;
                            ret=reinterpret_cast<bool(__cdecl*)(const CVector&,float,int32,CEntity**,bool,bool,bool)>(target)(pos,radius,mx,outE,bv,bp,bo);
                        } else {
                            const uint32 m=next()%4; sphereRet=m==0 ? 0 : m==3 ? uintptr_t(Ped(2)) : uintptr_t(Veh(0));
                            if(nm=="IsPositionClearOfCars-Pos"){
                                const CVector pos{uni(-100.f,100.f),uni(-100.f,100.f),uni(-10.f,30.f)};
                                ret=Name(reinterpret_cast<void*(__cdecl*)(const CVector*)>(target)(&pos));
                            } else {
                                if(sphereRet==uintptr_t(Ped(2)))sphereRet=uintptr_t(Veh(0));
                                auto* v=vehs[0].data(); { static const int32 sub[]{0,0,1,3,5,9}; At<int32>(v,V_SUBTYPE)=sub[next()%6]; At<int32>(v,V_TYPE)=sub[next()%6]; }
                                At<uint32>(v,V_FLAGS)=next(); colRet=next()%3==0 ? 0 : int32(next()%4);
                                ret=Name(reinterpret_cast<void*(__cdecl*)(const CPed*)>(target)(Ped(0)));
                            }
                        }
                    }
                    else if(cls=="CPedToPlayerConversations") {
                        { static const int32 states[]{0,1,2,3,4,-1}; At<int32>(reinterpret_cast<void*>(P2P_STATE),0)=states[next()%6]; }
                        At<CPed*>(reinterpret_cast<void*>(P2P_PED),0)=next()%3 ? Ped(3) : nullptr; At<uint32>(reinterpret_cast<void*>(P2P_LAST),0)=next();
                        lookMask=next();
                        reinterpret_cast<void(__cdecl*)()>(target)();
                        out["state"]=At<int32>(reinterpret_cast<void*>(P2P_STATE),0); out["ped"]=Name(At<uint32>(reinterpret_cast<void*>(P2P_PED),0));
                        out["last"]=At<uint32>(reinterpret_cast<void*>(P2P_LAST),0);
                    }
                    else if(cls=="CSetPiece") {
                        setPiece();
                        if(nm=="TryToGenerateCopPed"){
                            const float x=uni(-200.f,200.f), y=uni(-200.f,200.f);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,float,float)>(target)(o,x,y));
                        } else if(nm=="TryToGenerateCopCar"){
                            const float x=uni(-200.f,200.f), y=uni(-200.f,200.f); float tx=uni(-200.f,200.f), ty=uni(-200.f,200.f);
                            if(next()%8==0){ tx=x; ty=y; } // direccion nula: el original pone x = 1
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,float,float,float,float)>(target)(o,x,y,tx,ty));
                        } else reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        for(auto* h:hooks)h->State(false);
                        out["setpiece"]=Region(o,0x20);
                        json cs=json::array(), ps=json::array();
                        for(int k=0;k<2;++k){
                            auto* c=cars[k].data();
                            cs.push_back({{"hash",EntHash(c,cars[k].size())},{"mat",Region(carMats[k].data(),0x40)},{"type",c[E_TYPE]},{"ap",Region(c+V_AUTOPILOT,0x98)},{"move",Region(c+V_MOVE,12)},{"need",At<uint32>(c,V_NEED)}});
                            auto* p=cops[k].data();
                            ps.push_back({{"hash",EntHash(p,cops[k].size())},{"mat",Region(copMats[k].data(),0x40)},{"flags",At<uint32>(p,P_FLAGS3)},{"need",At<uint32>(p,P_NEED)},{"intel",Hash(copIntel[k].data(),copIntel[k].size())}});
                        }
                        out["cars"]=cs; out["cops"]=ps;
                    }
                    for(auto* h:hooks)h->State(false);
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls; out["clean"]=Occupancy()==occ;
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
    CleanRefs(); restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
