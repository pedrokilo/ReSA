#pragma once
// Banco dentro del motor: busqueda de entidad y sentarse en escritorio (lista de 9, 36 funciones).
// CTaskComplexSeekEntityStandard, CTaskComplexSeekEntityRadiusAngleOffset y
// CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> (constructor, destructor, Clone, GetTaskType, MakeAbortable,
// CreateSubTask, CreateNextSubTask, CreateFirstSubTask, ControlSubTask) y CTaskInteriorSitAtDesk (constructor,
// destructor, Clone, GetTaskType, MakeAbortable, ProcessPed, FinishAnimCB, StartRandomLoopAnim, StartRandomOneOffAnim).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones (el 3 hace de jugador con datos de jugador falsos), 2 vehiculos (vtable falsa con trampas, el tipo decide
// si son barcos), tareas falsas (vtable trazada), asociaciones de animacion falsas e InteriorInfo_t falso.
// Se simulan y trazan: ComputeEntitySeekPos de los tres calculadores, CPedGroups::AreInSameGroup,
// CPedGroupMembership::IsFollower, FindPlayerPed, CPed::Say, CPed::AttachPedToEntity, CPed::DettachPedFromEntity,
// CPedPlacement::FindZCoorForPed (firmas distintas en EXE y DLL), CTaskComplexFollowNodeRoute::SetTarget,
// CTaskSimpleStandStill::ProcessPed, CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation,
// InteriorManager_c::AreAnimsLoaded y el RNG.
// Pools de tareas, rutas de puntos y rutas de nodos: m_LastFreeSlot y estados se restauran y los slots libres se rellenan
// con 0xCD en cada ejecucion. El reloj y el contador de fotogramas se guardan y se restauran.
// Se compara la memoria del objeto (sin vptr; las vtables de calculador se traducen a nombres), los cambios de todos los
// buffers de fixtures (punteros traducidos a nombres), las tareas y rutas nuevas, las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h"
#include "Tasks/TaskTypes/TaskComplexFollowNodeRoute.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Interior/InteriorManager_c.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Animation/AnimManager.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "PedPlacement.h"
#include "PedGeometryAnalyser.h"
#include "NodeRoute.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "Pools/NodeRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks9dLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=4, NVEH=2, FAKES=3, NANIM=4;
// tareas falsas: los campos de control quedan por encima de 0x60 (GoToPoint escribe hasta +0x24)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CLONE=0x6C, FK_CALLS=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_HEADING=0x10, E_MATRIX=0x14, E_RWOBJ=0x18, E_SEED=0x20, E_TYPE=0x36;  // CEntity
constexpr size_t PH_FLAGS=0x40, PH_ATTACHED=0xFC;                                                  // CPhysical
constexpr size_t P_FLAGS=0x46C, P_PLAYERDATA=0x480, P_MOVESTATE=0x534, P_ROT=0x558, P_AIMROT=0x55C,
                 P_STANDING=0x568, P_CONTACT=0x584, P_VEHICLE=0x58C, P_PEDTYPE=0x598, P_WEAPONS=0x5A0,
                 P_ACTIVESLOT=0x718;                                                               // CPed
constexpr size_t V_TYPE=0x590;                                                                     // CVehicle
constexpr size_t A_DELTA=0x1C, A_SPEED=0x24, A_ID=0x2C;                                            // CAnimBlendAssociation
constexpr uintptr_t TIMER=0xB7CB84, FRAMES=0xB7CB4C, GROUPS=0xC09920, INTERIOR_MAN=0xBAF670; constexpr size_t GROUPS_SIZE=8*0x2D4;
constexpr uintptr_t VT_STD=0x859DF8, VT_RAD=0x85A388, VT_XY=0x86FBB4, VT_SIT=0x87035C;
constexpr uintptr_t CV_STD=0x859DC8, CV_RAD=0x85A384, CV_XY=0x86F8F8, CV_BASE=0x859DC4;
constexpr uintptr_t CB_DEFAULT=0x4D1390, CB_FINISH=0x676190;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,NANIM> anims{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,0x40> playerData{};
alignas(16) inline std::array<uint8,0x20> info{};
alignas(16) inline std::array<uint8,0x40> event{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CAnimBlendAssociation* Anim(int n) { return reinterpret_cast<CAnimBlendAssociation*>(anims[n].data()); }

// pools (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80, PROUTE_SLOT=sizeof(CPointRoute), NROUTE_SLOT=sizeof(CNodeRoute);
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PRouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* NRouteHdr() { return reinterpret_cast<uint8*>(GetNodeRoutePool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline int SlotIndex(uint8* h,size_t slot,uintptr_t v) {
    const auto s=uintptr_t(Storage(h)); if(v<s||v>=s+size_t(Cap(h))*slot||(v-s)%slot)return -1; return int((v-s)/slot);
}
inline bool IsPoolTask(uintptr_t v) { const int k=SlotIndex(TaskHdr(),TASK_SLOT,v); return k>=0&&!(States(TaskHdr())[k]&0x80); }
inline std::vector<bool> Occ(uint8* h) { std::vector<bool> r(size_t(Cap(h))); for(size_t k=0;k<r.size();++k)r[k]=!(States(h)[k]&0x80); return r; }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart) {
    Last(h)=last;
    if(Occ(h)==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}

inline std::array<void*,12> fakeVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t dllStdVt{}, dllRadVt{}, dllXyVt{}, dllFinishCb{}, dllDefaultCb{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(v==CV_STD||v==dllStdVt)return "std_calc_vt"; if(v==CV_RAD||v==dllRadVt)return "rad_calc_vt"; if(v==CV_XY||v==dllXyVt)return "xy_calc_vt";
    if(v==CV_BASE)return "base_calc_vt";
    if(v==CB_FINISH||v==dllFinishCb)return "finish_cb"; if(v==CB_DEFAULT||v==dllDefaultCb)return "default_cb";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NANIM;++n)if(auto s=in(anims[n].data(),anims[n].size(),"anim"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(info.data(),info.size(),"info"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(reinterpret_cast<void*>(GROUPS),GROUPS_SIZE,"groups"))return *s;
    if(v==INTERIOR_MAN)return "interior_man";
    if(const int k=SlotIndex(PRouteHdr(),PROUTE_SLOT,v);k>=0)return "proute#"+std::to_string(k);
    if(const int k=SlotIndex(NRouteHdr(),NROUTE_SLOT,v);k>=0)return "nroute#"+std::to_string(k);
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<TASK_SLOT/4;++k)words.push_back(Name(w[k]));
    return {{"type",type},{"words",words}};
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()}); b.push_back({"pmat#"+std::to_string(n),pedMats[n].data(),pedMats[n].size()}); }
    for(int n=0;n<NVEH;++n){ b.push_back({"veh#"+std::to_string(n),vehs[n].data(),vehs[n].size()}); b.push_back({"vmat#"+std::to_string(n),vehMats[n].data(),vehMats[n].size()}); }
    for(int n=0;n<FAKES;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    for(int n=0;n<NANIM;++n)b.push_back({"anim#"+std::to_string(n),anims[n].data(),anims[n].size()});
    b.push_back({"clump",clump.data(),clump.size()}); b.push_back({"pdata",playerData.data(),playerData.size()});
    b.push_back({"info",info.data(),info.size()}); b.push_back({"event",event.data(),event.size()});
    return b;
}
inline std::vector<std::vector<uint8>> Snapshot() { std::vector<std::vector<uint8>> s; for(auto& b:Buffers())s.push_back(ReversingProbe::Bytes(b.p,b.n)); return s; }
inline json Changes(const std::vector<std::vector<uint8>>& before) {
    json r=json::array(); const auto bufs=Buffers();
    for(size_t k=0;k<bufs.size();++k)for(size_t w=0;w+4<=bufs[k].n;w+=4)
        if(std::memcmp(bufs[k].p+w,before[k].data()+w,4))r.push_back({bufs[k].name,w,Name(At<uint32>(bufs[k].p,w))});
    return r;
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, calcCalls{}, blendCalls{}, sameGroupMask{}, sameGroupCalls{}, followerMask{}, followerCalls{},
              loadedMask{}, loadedCalls{};
inline uint8 findZRet{}, calcMode{}; inline uintptr_t assocRet{};

// tareas falsas: MakeAbortable responde segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CALLS)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k%32))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::array<void*,sizeof...(N)>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return 0;
}
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(Ped(3)); }
inline bool __cdecl FakeSameGroup(const CPed* a,const CPed* b) { const bool r=(sameGroupMask>>(sameGroupCalls++%32))&1; trace.push_back({"same_group",Name(a),Name(b),r}); return r; }
inline bool __fastcall FakeIsFollower(void* m,void*,const CPed* p) { const bool r=(followerMask>>(followerCalls++%32))&1; trace.push_back({"is_follower",Name(m),Name(p),r}); return r; }
inline void __fastcall FakeDettach(CPed* p,void*) { trace.push_back({"dettach_ped",Name(p)}); }
// el EXE empuja el angulo como int de _ftol2 y el DLL como uint16: solo se trazan 16 bits
inline CEntity* __fastcall FakeAttach(CPed* p,void*,CEntity* e,CVector off,uint32 heading,float f,int32 weapon) {
    trace.push_back({"attach_ped",Name(p),Name(e),Vec(off),heading&0xFFFF,F(f),weapon}); return e;
}
inline float NewZ(const CVector& p) { return p.z+1.25f; }
inline bool __cdecl FakeFindZExe(CVector* p) { trace.push_back({"find_z_for_ped",Vec(*p),findZRet}); if(findZRet)p->z=NewZ(*p); return findZRet!=0; }
inline std::pair<CVector,bool> __cdecl FakeFindZDll(CVector p) { trace.push_back({"find_z_for_ped",Vec(p),findZRet}); if(findZRet)p.z=NewZ(p); return {p,findZRet!=0}; }
inline void __fastcall FakeSetTarget(void* t,void*,CPed* p,const CVector* v,float tol,float slow,float height,uint32 force) {
    trace.push_back({"set_target",Name(t),Name(p),Vec(*v),F(tol),F(slow),F(height),force&0xFF});
}
// tarea local en la pila: solo se trazan sus campos
inline bool __fastcall FakeStandStillProcess(CTask* t,void*,CPed* p) {
    trace.push_back({"stand_still_process",Name(p),At<int32>(t,8),At<uint8>(t,0x18),At<uint8>(t,0x19),F(At<float>(t,0x1C))}); return false;
}
// posicion buscada: modo 0 = la del peaton (distancia 0), resto alrededor del peaton
inline CVector PredictSeek(const CPed* seeker,uint32 c) {
    const CVector pos=const_cast<CPed*>(seeker)->GetPosition();
    if(calcMode==0)return pos;
    return CVector{pos.x+float(int((seed+c*7)%61))*0.5f-15.f+0.125f*float(c),pos.y+float(int((seed/3+c*5)%53))*0.5f-13.f,pos.z+float(int(seed%7))-3.f};
}
inline void __fastcall FakeCompute(void* calc,void*,const CPed* seeker,const CEntity* target,CVector* out) {
    trace.push_back({"compute_seek_pos",Name(calc),Name(seeker),Name(target)}); *out=PredictSeek(seeker,calcCalls++);
}
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 group,int32 id,float delta) {
    auto* a=Anim(1+int(blendCalls++%3)); At<int16>(a,A_ID)=int16(id);
    trace.push_back({"blend_animation",Name(c),group,id,F(delta),Name(a)}); return a;
}
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) { trace.push_back({"get_association",Name(c),id,Name(assocRet)}); return reinterpret_cast<CAnimBlendAssociation*>(assocRet); }
inline bool __fastcall FakeAnimsLoaded(void* m,void*,int32 b) { const bool r=(loadedMask>>(loadedCalls++%32))&1; trace.push_back({"anims_loaded",Name(m),b,r}); return r; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=36;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexSeekEntityStandard","Clone",0,0x859DF8,1,256},
    {"CTaskComplexSeekEntityStandard","Constructor",0x46AC10,0,-1,256},
    {"CTaskComplexSeekEntityStandard","ControlSubTask",0,0x859DF8,10,1024},
    {"CTaskComplexSeekEntityStandard","CreateFirstSubTask",0,0x859DF8,9,512},
    {"CTaskComplexSeekEntityStandard","CreateNextSubTask",0,0x859DF8,8,512},
    {"CTaskComplexSeekEntityStandard","CreateSubTask",0x46F1E0,0,-1,512},
    {"CTaskComplexSeekEntityStandard","Destructor",0x46AD70,0,-1,128},
    {"CTaskComplexSeekEntityStandard","GetTaskType",0,0x859DF8,4,64},
    {"CTaskComplexSeekEntityStandard","MakeAbortable",0,0x859DF8,6,256},
    {"CTaskComplexSeekEntityRadiusAngleOffset","Clone",0,0x85A388,1,256},
    {"CTaskComplexSeekEntityRadiusAngleOffset","Constructor",0x493730,0,-1,256},
    {"CTaskComplexSeekEntityRadiusAngleOffset","ControlSubTask",0,0x85A388,10,1024},
    {"CTaskComplexSeekEntityRadiusAngleOffset","CreateFirstSubTask",0,0x85A388,9,512},
    {"CTaskComplexSeekEntityRadiusAngleOffset","CreateNextSubTask",0,0x85A388,8,512},
    {"CTaskComplexSeekEntityRadiusAngleOffset","CreateSubTask",0x496270,0,-1,512},
    {"CTaskComplexSeekEntityRadiusAngleOffset","Destructor",0x493890,0,-1,128},
    {"CTaskComplexSeekEntityRadiusAngleOffset","GetTaskType",0,0x85A388,4,64},
    {"CTaskComplexSeekEntityRadiusAngleOffset","MakeAbortable",0,0x85A388,6,256},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","Clone",0,0x86FBB4,1,256},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","Constructor",0x661DC0,0,-1,256},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","ControlSubTask",0,0x86FBB4,10,1024},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","CreateFirstSubTask",0,0x86FBB4,9,512},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","CreateNextSubTask",0,0x86FBB4,8,512},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","CreateSubTask",0x665270,0,-1,512},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","Destructor",0x661F30,0,-1,128},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","GetTaskType",0,0x86FBB4,4,64},
    {"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","MakeAbortable",0,0x86FBB4,6,256},
    {"CTaskInteriorSitAtDesk","Clone",0,0x87035C,1,128},
    {"CTaskInteriorSitAtDesk","Constructor",0x676010,0,-1,128},
    {"CTaskInteriorSitAtDesk","Destructor",0x676080,0,-1,128},
    {"CTaskInteriorSitAtDesk","FinishAnimCB",0x676190,0,-1,256},
    {"CTaskInteriorSitAtDesk","GetTaskType",0,0x87035C,4,64},
    {"CTaskInteriorSitAtDesk","MakeAbortable",0,0x87035C,6,256},
    {"CTaskInteriorSitAtDesk","ProcessPed",0,0x87035C,7,1024},
    {"CTaskInteriorSitAtDesk","StartRandomLoopAnim",0x677780,0,-1,512},
    {"CTaskInteriorSitAtDesk","StartRandomOneOffAnim",0x677880,0,-1,512},
}};
// tamano, vtable del calculador del EXE, desplazamiento del estado de movimiento y de los flags
struct SeekInfo { uintptr_t vt; size_t size; uintptr_t calcVt; size_t calcData, ms, flags; };
inline SeekInfo Seek(const std::string& cls) {
    if(cls=="CTaskComplexSeekEntityStandard")return {VT_STD,0x4C,CV_STD,0,0x44,0x48};
    if(cls=="CTaskComplexSeekEntityRadiusAngleOffset")return {VT_RAD,0x54,CV_RAD,8,0x4C,0x50};
    return {VT_XY,0x58,CV_XY,0xC,0x50,0x54};
}

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks9d-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks9d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks9d");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks9d-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllCompStd=OurVirtual("CEntitySeekPosCalculatorStandard","ComputeEntitySeekPos"),
                    dllCompRad=OurVirtual("CEntitySeekPosCalculatorRadiusAngleOffset","ComputeEntitySeekPos"),
                    dllCompXy=OurVirtual("CEntitySeekPosCalculatorXYOffset","ComputeEntitySeekPos"),
                    dllStandStill=OurVirtual("CTaskSimpleStandStill","ProcessPed");
    if(!dllCompStd||!dllCompRad||!dllCompXy||!dllStandStill){Write({{"event","error"},{"reason","virtual hook not found"},{"function","ComputeEntitySeekPos/CTaskSimpleStandStill::ProcessPed"}});return;}
    { CEntitySeekPosCalculatorStandard a{}; CEntitySeekPosCalculatorRadiusAngleOffset b{}; CEntitySeekPosCalculatorXYOffset c{};
      dllStdVt=*reinterpret_cast<const uintptr_t*>(&a); dllRadVt=*reinterpret_cast<const uintptr_t*>(&b); dllXyVt=*reinterpret_cast<const uintptr_t*>(&c); }
    dllFinishCb=std::bit_cast<uintptr_t>(&CTaskInteriorSitAtDesk::FinishAnimCB); dllDefaultCb=std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB);
    FillTraps(fakeVtbl,std::make_integer_sequence<int,12>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    const auto occTaskStart=Occ(TaskHdr()), occPRStart=Occ(PRouteHdr()), occNRStart=Occ(NRouteHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr()), lp0=Last(PRouteHdr()), ln0=Last(NRouteHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    const std::vector<uint8> stPR(States(PRouteHdr()),States(PRouteHdr())+Cap(PRouteHdr()));
    const std::vector<uint8> stNR(States(NRouteHdr()),States(NRouteHdr())+Cap(NRouteHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); ResetPool(PRouteHdr(),PROUTE_SLOT,lp0,stPR,occPRStart); ResetPool(NRouteHdr(),NROUTE_SLOT,ln0,stNR,occNRStart); };
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{FRAMES,4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch s2(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FakeFindPlayerPed), s2e(0x56E210,FakeFindPlayerPed);
        EntryPatch s3e(0x5F7F40,FakeSameGroup); // CPedGroups::AreInSameGroup del DLL llama al EXE
        EntryPatch s4(Body(MemberAddress(&CPedGroupMembership::IsFollower)),FakeIsFollower), s4e(0x5F6A10,FakeIsFollower);
        EntryPatch s5(Body(MemberAddress(&CPed::DettachPedFromEntity)),FakeDettach), s5e(0x5E7EC0,FakeDettach);
        EntryPatch s6(Body(MemberAddress(&CPed::AttachPedToEntity)),FakeAttach), s6e(0x5E7CB0,FakeAttach);
        EntryPatch s7(Body(bit_cast<uintptr_t>(&CPedPlacement::FindZCoorForPed)),FakeFindZDll), s7e(0x616920,FakeFindZExe);
        EntryPatch s8(Body(MemberAddress(&CTaskComplexFollowNodeRoute::SetTarget)),FakeSetTarget), s8e(0x671750,FakeSetTarget);
        EntryPatch s9(Body(dllStandStill),FakeStandStillProcess), s9e(0x62F370,FakeStandStillProcess);
        EntryPatch c1(Body(dllCompStd),FakeCompute), c1e(0x46AF20,FakeCompute);
        EntryPatch c2(Body(dllCompRad),FakeCompute), c2e(0x6946F0,FakeCompute);
        EntryPatch c3(Body(dllCompXy),FakeCompute), c3e(0x6946A0,FakeCompute);
        EntryPatch a1(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FakeBlend), a1e(0x4D4610,FakeBlend);
        EntryPatch a2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FakeGetAssoc), a2e(0x4D68B0,FakeGetAssoc);
        EntryPatch a3(Body(MemberAddress(&InteriorManager_c::AreAnimsLoaded)),FakeAnimsLoaded), a3e(0x5980F0,FakeAnimsLoaded);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool sit=cls=="CTaskInteriorSitAtDesk"; const SeekInfo si=Seek(cls);
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // slots libres con 0xCD: los bytes que los constructores no escriben quedan iguales en ambas ejecuciones
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x9D9Du; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=calcCalls=blendCalls=sameGroupCalls=followerCalls=loadedCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    At<uint32>(reinterpret_cast<void*>(FRAMES),0)=next();
                    // peatones: matriz (casi siempre), clump, semilla, estado de movimiento, tipo, armas
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        p[E_TYPE]=3; At<void*>(p,E_RWOBJ)=clump.data();
                        if(n==0||next()%5){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; }
                        else { At<CVector>(p,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; At<float>(p,E_HEADING)=uni(-3.f,3.f); }
                        At<uint16>(p,E_SEED)=uint16(next()); At<int32>(p,P_MOVESTATE)=int32(next()%8);
                        { static const int32 types[]{0,1,4,5,7,0}; At<int32>(p,P_PEDTYPE)=types[next()%6]; }
                        At<int8>(p,P_ACTIVESLOT)=int8(next()%13); for(int w=0;w<13;++w)At<int32>(p,P_WEAPONS+0x1C*w)=int32(next()%47);
                        At<float>(p,P_ROT)=uni(-3.2f,3.2f); At<float>(p,P_AIMROT)=uni(-3.2f,3.2f);
                    }
                    At<void*>(Ped(3),P_PLAYERDATA)=playerData.data();
                    std::memset(playerData.data(),0,playerData.size()); At<int32>(playerData.data(),0x38)=int32(next()%8);
                    // vehiculos: vtable falsa con trampas, siempre con matriz (el EXE invierte la del barco sin comprobarla)
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data();
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{5,5,0,9,5,3}; At<int32>(v,V_TYPE)=types[next()%6]; }
                    }
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0; At<uint32>(k,8)=0;
                        for(size_t w=0x10;w<0x24;w+=4)At<float>(k,w)=uni(-50.f,50.f);
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE+n; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CALLS)=0; At<uint32>(k,FK_CLONE)=0; }
                    for(int n=0;n<NANIM;++n){ auto* a=anims[n].data(); for(size_t b=0;b<anims[n].size();++b)a[b]=uint8(next());
                        At<uint32>(a,0)=0; At<float>(a,A_DELTA)=uni(-8.f,8.f); At<float>(a,A_SPEED)=uni(0.f,2.f); At<float>(a,0x18)=uni(0.f,1.f); At<float>(a,0x20)=uni(0.f,3.f); At<float>(a,0x28)=uni(0.f,1.f); }
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;
                    for(auto& b:info)b=uint8(next()); std::memset(event.data(),0,event.size());
                    std::memset(object.data(),0xCD,object.size());
                    sameGroupMask=next(); followerMask=next(); loadedMask=next(); findZRet=uint8(next()%2); calcMode=uint8(next()%6==0 ? 0 : 1+next()%3);
                    assocRet=next()%2 ? uintptr_t(anims[3].data()) : 0;

                    auto* o=object.data(); auto* p0=Ped(0);
                    auto setObj=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto inVeh=[&](CPed* p,bool on){ auto& fl=At<uint32>(p,P_FLAGS); fl=on ? (fl|0x100u) : (fl&~0x100u); };
                    auto timer=[&](size_t off){ At<uint32>(o,off)=now-next()%5000; At<int32>(o,off+4)=next()%8==0 ? -1 : int32(next()%5000); o[off+8]=uint8(next()%4!=0); o[off+9]=uint8(next()%8==0); };
                    auto interval=[&]{ const uint32 m=next()%8; return m==0 ? -1 : m==1 ? -int32(2+next()%100) : int32(next()%5000); };
                    auto pickEnt=[&]()->CEntity*{ switch(next()%6){ case 0: return nullptr; case 1: return Veh(1); case 2: return Ped(1); default: return Ped(2); } };
                    auto distVal=[&]{ const uint32 m=next()%10; return m==0 ? 0.f : m==1 ? qnan : uni(0.f,20.f); };
                    // CTaskComplexSeekEntity: +0x0C entidad, +0x10/+0x14 intervalos, +0x18 radio, +0x1C radio de estado, +0x20 minimo,
                    // +0x24 altura, +0x28/+0x34 temporizadores, +0x40 calculador (vtable del EXE y datos), estado de movimiento y flags
                    auto setSeek=[&](CEntity* ent){
                        setObj(si.vt,si.size); At<CEntity*>(o,0xC)=ent; At<int32>(o,0x10)=interval(); At<int32>(o,0x14)=interval();
                        At<float>(o,0x18)=distVal(); At<float>(o,0x1C)=uni(0.f,8.f); At<float>(o,0x20)=distVal(); At<float>(o,0x24)=uni(0.f,4.f);
                        timer(0x28); timer(0x34);
                        At<uintptr_t>(o,0x40)=si.calcVt; for(size_t w=0;w<si.calcData;w+=4)At<float>(o,0x44+w)=uni(-6.f,6.f);
                        At<int32>(o,si.ms)=int32(next()%8);
                    };
                    // CTaskInteriorSitAtDesk: +8 duracion, +0xC info, +0x10 instantaneo, +0x14 animacion, +0x18 anterior,
                    // +0x1C/+0x28 temporizadores, +0x34 actualizar posicion, +0x35 terminada, +0x36 abortando
                    auto setSit=[&]{
                        setObj(VT_SIT,0x38); At<int32>(o,8)=next()%8==0 ? -int32(next()%100) : int32(next()%20000); At<void*>(o,0xC)=info.data();
                        o[0x10]=uint8(next()%2); At<void*>(o,0x14)=next()%4 ? (void*)anims[0].data() : nullptr;
                        { static const int32 prev[]{-1,-1,0x148,0x14D,0x150,0x14E,0x149,0x14A,0x14C,5}; At<int32>(o,0x18)=prev[next()%10]; }
                        timer(0x1C); timer(0x28); o[0x34]=uint8(next()%2); o[0x35]=uint8(next()%4==0); o[0x36]=uint8(next()%3==0);
                        { static const int16 ids[]{0x148,0x148,0x149,0x14A,0x14B,0x14C,0x14D,0x14E,0x14F,0x150,5,-1}; At<int16>(anims[0].data(),A_ID)=ids[next()%12]; }
                        // la posicion del escritorio a veces casi coincide con la del peaton (paso menor que 0.02)
                        const CVector pp=p0->GetPosition(); const uint32 m=next()%4;
                        At<CVector>(info.data(),4)=m==0 ? CVector{pp.x+uni(-0.01f,0.01f),pp.y+uni(-0.01f,0.01f),pp.z} : CVector{pp.x+uni(-3.f,3.f),pp.y+uni(-3.f,3.f),pp.z+uni(-1.f,1.f)};
                        At<CVector>(info.data(),0x10)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f};
                    };

                    const auto occT=Occ(TaskHdr()), occP=Occ(PRouteHdr()), occN=Occ(NRouteHdr());
                    // preparacion especifica antes de la instantanea
                    CEntity* ctorEnt{}; int32 ctorA{}, ctorB{}; float ctorF[3]{}; uint32 ctorT{}, ctorFace{}; int32 subType{}; int32 prio{}; const CEvent* ev{}; float blendArg{};
                    if(!sit) {
                        if(nm=="Constructor"){
                            ctorEnt=pickEnt(); ctorA=interval(); ctorB=interval(); ctorF[0]=distVal(); ctorF[1]=uni(0.f,8.f); ctorF[2]=uni(0.f,4.f);
                            ctorT=next()%2; ctorFace=next()%2;
                            o[si.flags]=uint8(next()); // bits altos que el constructor conserva
                        } else {
                            setSeek(nm=="ControlSubTask"||nm=="CreateFirstSubTask" ? pickEnt() : (next()%8==0 ? nullptr : pickEnt()));
                            At<CTask*>(o,8)=nm=="Destructor"&&next()%3==0 ? nullptr : Fake(0);
                            if(nm=="Destructor"||nm=="Clone")Reg(At<CEntity*>(o,0xC),o+0xC);
                            if(nm=="MakeAbortable"){ prio=int32(next()%4); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                            if(nm=="CreateSubTask"){
                                static const int32 types[]{TASK_FINISHED,TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
                                    TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_TIRED,TASK_SIMPLE_PAUSE,1000};
                                subType=types[next()%11]; At<CVehicle*>(p0,P_VEHICLE)=Veh(0);
                                if(!At<CEntity*>(o,0xC))At<CEntity*>(o,0xC)=Ped(2);
                            }
                            if(nm=="CreateNextSubTask"){
                                static const int32 types[]{TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_TURN_TO_FACE_ENTITY,TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,
                                    TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_STAND_STILL,
                                    TASK_SIMPLE_TIRED,TASK_SIMPLE_PAUSE};
                                At<int32>(fakes[0].data(),FK_TYPE)=types[next()%11];
                                inVeh(p0,next()%2); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr;
                                At<void*>(p0,PH_ATTACHED)=next()%2 ? (void*)Veh(1) : nullptr;
                            }
                            if(nm=="CreateFirstSubTask"){ inVeh(p0,next()%3==0); At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(0) : nullptr; }
                            if(nm=="ControlSubTask"){
                                { static const int32 types[]{TASK_COMPLEX_FOLLOW_NODE_ROUTE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_FOLLOW_NODE_ROUTE,
                                      TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE};
                                  At<int32>(fakes[0].data(),FK_TYPE)=types[next()%6]; }
                                fakes[0][0x24]=uint8(next()); o[si.flags]=uint8(next());
                                // GoToPoint sin cambios: el destino falso ya es el punto que devolvera el calculador
                                if(next()%4==0){ At<CVector>(fakes[0].data(),0x10)=PredictSeek(p0,0); At<float>(fakes[0].data(),0x1C)=At<float>(o,0x18); At<float>(fakes[0].data(),0x20)=At<float>(o,0x1C); }
                                // barco comun: el objetivo y el buscador pisan o tocan el vehiculo 0
                                auto* tp=peds[2].data();
                                if(next()%2){ const bool standing=next()%2; At<CEntity*>(tp,standing ? P_STANDING : P_CONTACT)=Veh(0);
                                    if(next()%4==0)At<CEntity*>(tp,standing ? P_CONTACT : P_STANDING)=next()%2 ? (CEntity*)Veh(1) : (CEntity*)Ped(1);
                                    At<CEntity*>(p0,next()%2 ? P_STANDING : P_CONTACT)=next()%5 ? (CEntity*)Veh(0) : (CEntity*)Veh(1);
                                    if(next()%3==0)At<CEntity*>(p0,P_CONTACT)=Veh(1);
                                } else { auto pick=[&]()->CEntity*{ switch(next()%4){ case 0: return nullptr; case 1: return Veh(0); case 2: return Veh(1); default: return Ped(1); } };
                                    At<CEntity*>(tp,P_STANDING)=pick(); At<CEntity*>(tp,P_CONTACT)=pick(); At<CEntity*>(p0,P_STANDING)=pick(); At<CEntity*>(p0,P_CONTACT)=pick(); }
                                if(next()%4==0)At<uint32>(tp,PH_FLAGS)|=0x100u;
                                At<void*>(p0,PH_ATTACHED)=next()%3==0 ? (void*)Veh(next()%2) : nullptr;
                                // frase: a veces la semilla y el contador de fotogramas dan multiplo de 0x4000
                                if(next()%2)At<uint32>(reinterpret_cast<void*>(FRAMES),0)=0x4000u*(next()%64)-At<uint16>(p0,E_SEED);
                                At<int32>(p0,P_MOVESTATE)=int32(next()%3 ? 5+next()%3 : next()%8);
                            }
                        }
                    } else {
                        if(nm!="Constructor")setSit();
                        if(nm=="Constructor"){ ctorA=next()%8==0 ? -int32(next()%100) : int32(next()%20000); ctorT=next()%2; }
                        if(nm=="MakeAbortable"){ prio=int32(next()%4); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                        if(nm=="FinishAnimCB"){ At<void*>(o,0x14)=anims[0].data(); } // el DLL comprueba que la animacion es la de la tarea
                        if(nm=="StartRandomLoopAnim"){ static const float b[]{4.f,1000.f,8.f}; blendArg=next()%4 ? b[next()%3] : uni(0.f,100.f); }
                    }
                    const auto before=Snapshot();

                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; CTask* r{}; bool dead=false;
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callDtor=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); dead=true; };
                    auto callAbort=[&]{ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev); };
                    if(nm=="GetTaskType"){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }
                    else if(nm=="Clone"){ r=callClone(); ret=Name(r); }
                    else if(nm=="Destructor"){ callDtor(); }
                    else if(nm=="MakeAbortable"){ ret=callAbort(); }
                    else if(!sit) {
                        if(nm=="Constructor"){
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,int32,int32,float,float,float,uint32,uint32)>(target)(o,ctorEnt,ctorA,ctorB,ctorF[0],ctorF[1],ctorF[2],ctorT,ctorFace));
                        } else if(nm=="CreateSubTask"){
                            r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,subType,p0); ret=Name(r);
                        } else { // CreateFirstSubTask, CreateNextSubTask, ControlSubTask
                            r=callTask(p0); ret=Name(r);
                        }
                    } else {
                        if(nm=="Constructor"){ ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,void*,uint32)>(target)(o,ctorA,info.data(),ctorT)); }
                        else if(nm=="FinishAnimCB"){ reinterpret_cast<void(__cdecl*)(CAnimBlendAssociation*,void*)>(target)(Anim(0),o); }
                        else if(nm=="StartRandomLoopAnim"){ reinterpret_cast<void(__thiscall*)(void*,CPed*,float)>(target)(o,p0,blendArg); }
                        else if(nm=="StartRandomOneOffAnim"){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0); }
                        else { ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,p0); } // ProcessPed
                    }
                    for(auto* h:hooks)h->State(false);

                    // tareas nuevas del pool (raiz: el retorno)
                    std::vector<CTask*> roots;
                    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                      if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r); }
                    // rutas nuevas de ambos pools (antes de borrar las tareas, que las liberan)
                    { json routes=json::array();
                      for(size_t k=0;k<occP.size();++k)if(!occP[k]&&!(States(PRouteHdr())[k]&0x80))routes.push_back({{"proute",k},{"words",Region(Storage(PRouteHdr())+k*PROUTE_SLOT,PROUTE_SLOT)}});
                      for(size_t k=0;k<occN.size();++k)if(!occN[k]&&!(States(NRouteHdr())[k]&0x80))routes.push_back({{"nroute",k},{"words",Region(Storage(NRouteHdr())+k*NROUTE_SLOT,NROUTE_SLOT)}});
                      out["routes"]=routes; }
                    // objeto sin vptr; tras el destructor tampoco se compara la vptr del calculador destruido (+0x40)
                    { json obj=Region(o,sit ? 0x38 : si.size,true); if(dead&&!sit)obj[0x40/4-1]="dead_calc_vt"; out["object"]=obj; }
                    if(nm=="Constructor")out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    // limpieza: tareas nuevas alcanzables (raices), rutas sobrantes y referencias; despues los pools deben quedar como antes
                    const size_t traceBefore=trace.size();
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls;
                    out["clean"]=Occ(TaskHdr())==occT&&Occ(PRouteHdr())==occP&&Occ(NRouteHdr())==occN;
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
    CleanRefs(); restoreGlobals(); resetPools();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occ(TaskHdr())==occTaskStart&&Occ(PRouteHdr())==occPRStart&&Occ(NRouteHdr())==occNRStart&&freeRefs()==savedFree&&globalsIntact()
                        &&Last(TaskHdr())==lt0&&Last(PRouteHdr())==lp0&&Last(NRouteHdr())==ln0;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
