#pragma once
// Banco dentro del motor: clases sueltas del lote misc61a (CScriptsForBrains, CPedGroup, CWeather y CRopes; 42 funciones).
// Original (hooks de la clase desactivados) frente a candidato con fixtures deterministas. Nunca se tocan los singletons
// reales: los scripts de cerebros, los grupos y los peatones son buferes propios; los datos estaticos reales que usan CRopes
// (el array de cuerdas) y CWeather (variables, tablas de estado, buferes temporales, camara, reloj, replay, time cycle) se
// guardan y se restauran en cada ejecucion y al final.
// Se simulan y trazan (solo mientras corre el banco): rand (0x821B1E + CGeneral::GetRandomNumber), operator new[] (solo durante
// RenderRainStreaks), operator new/delete de eventos, estados de render (fpRenderStateSet) e Im3D (direcciones del EXE),
// CCullZones::CamNoRain/PlayerNoRain, FindPlayerPed/FindPlayerVehicle/FindPlayerCoors, CCamera::GetLookingLRBFirstPerson,
// CVehicle::CarHasRoof, CWorld::ProcessVerticalLine, FxSystem_c::AddParticle, CAEWeatherAudioEntity::AddAudioEvent/Service,
// CPad::StartShake, CWeather::AddRain y UpdateInTunnelness (solo en las pruebas de Update), CPed::Say,
// CPed::PositionAnyPedOutOfCollision, CPedIntelligence::FindTaskByType/IsInACarOrEnteringOne, CPedGroupIntelligence::
// AddEvent/SetTask/Flush/Process, CPedGroupMembership::RemoveMember/Process, CFormation::*, CStreaming::RequestModel,
// CTheScripts::AddToWaitingForScriptBrainArray, CScriptsForBrains::StartNewStreamedScriptBrain, CRope::Remove/Update/Render/
// CreateHookObjectForRope y CEntity::RegisterReference. Las vtables de los peatones, tareas y eventos falsos tienen trampas
// para cualquier ranura no prevista.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ScriptsForBrains.h"
#include "PedGroup.h"
#include "PedGroups.h"
#include "PedList.h"
#include "Formation.h"
#include "Weather.h"
#include "Ropes.h"
#include "Rope.h"
#include "CullZones.h"
#include "Pad.h"
#include "Streaming.h"
#include "Replay.h"
#include "PedIntelligence.h"
#include "EventGroupEvent.h"
#include "EventPlayerCommandToGroup.h"
#include "TaskSimpleGoToPoint.h"
#include "FxSystem.h"
#include "FxPrtMult.h"
#include "AEWeatherAudioEntity.h"
#include "../ProbeRuntime.hpp"

namespace Misc61aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }   // salta los saltos de la tabla de enlace incremental
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- constantes del EXE ---
constexpr int NPED=10; constexpr size_t PED_STRIDE=0x7C4;
constexpr size_t P_POS=0x4, P_MAT=0x14, P_AREA=0x2F, P_FLAGS=0x46C, P_SBFLAGS=0x474, P_INTEL=0x47C, P_CREATED=0x484, P_STATE=0x530, P_VEH=0x58C, P_TYPE=0x598, P_SBIDX=0x794;
constexpr uintptr_t TEMP_VERTS=0xC4D958, TEMP_IDX=0xC4B958, TEMP_NV=0xC4B950, TEMP_NI=0xC4B954;
constexpr uintptr_t ROPES=0xB768B8; constexpr size_t ROPE_SIZE=0x328, NROPES=8;
constexpr uintptr_t SCM_INFOS=0x964DF8; constexpr size_t SCM_COUNT=80, INFO_SIZE=20; // CStreaming::ms_aInfoForModel[26230 ...]
constexpr uintptr_t MODEL_PTRS=0xA9B0C8; constexpr int MI_BASE=19990, MI_COUNT=10;      // ranuras no usadas de ms_modelInfoPtrs

// --- buferes de fixtures (estaticos: misma direccion en original y candidato) ---
alignas(16) inline std::array<uint8,NPED*PED_STRIDE> pedStore{};
alignas(16) inline std::array<std::array<uint8,0x60>,NPED> pedMats{};
alignas(16) inline std::array<std::array<uint8,0x100>,NPED> intels{};
alignas(16) inline std::array<std::array<uint8,0x48>,8> tasks{};
alignas(16) inline std::array<std::array<uint8,0x80>,4> vehs{};
alignas(16) inline std::array<uint8,0x2D4> groupBuf{};
alignas(16) inline std::array<uint8,0x20> gEvent{}, gInner{};
alignas(16) inline std::array<uint8,PED_STRIDE> playerPed{};
alignas(16) inline std::array<uint8,0x100> playerIntel{};
alignas(16) inline std::array<uint8,0x1A0> object{};
alignas(16) inline std::array<uint8,0x60> objMat{};
alignas(16) inline std::array<uint8,0x578> scripts{};
alignas(16) inline std::array<char,32> nameBuf{};
alignas(16) inline std::array<uint8,0x48> camMat{};
alignas(16) inline std::array<uint8,0x100> fxSplash{}, fxSand{};
alignas(16) inline std::array<std::array<uint8,0x100>,4> ents{};      // entidades / fisicos falsos de las cuerdas
alignas(16) inline std::array<std::array<uint8,0x40>,MI_COUNT> modelInfos{};
alignas(16) inline std::array<std::array<uint8,0x40>,MI_COUNT> colModels{};
alignas(16) inline std::array<int32,32> streakX{}, streakY{}, streakZ{};
alignas(16) inline std::array<uint8,32> streakS{};
alignas(16) inline std::array<std::array<uint8,0x80>,4> newBufs{};
alignas(16) inline std::array<uint8,0x100> evArena{};
alignas(16) inline std::array<void*,128> pedVtbl{}, taskVtbl{}, evVtbl{};
alignas(16) inline std::array<uint8,0x40> fakeTaskNew{};
inline uint8* PedBuf(int n) { return pedStore.data()+size_t(n)*PED_STRIDE; }
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(PedBuf(n)); }

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n){ return v>=uintptr_t(b)&&v<uintptr_t(b)+n; };
    auto rel=[&](const void* b){ return std::to_string(v-uintptr_t(b)); };
    if(in(pedStore.data(),pedStore.size())){ const auto o=v-uintptr_t(pedStore.data()); return "ped#"+std::to_string(o/PED_STRIDE)+"+"+std::to_string(o%PED_STRIDE); }
    for(int n=0;n<NPED;++n){ if(in(pedMats[n].data(),pedMats[n].size()))return "pmat#"+std::to_string(n)+"+"+rel(pedMats[n].data());
                             if(in(intels[n].data(),intels[n].size()))return "intel#"+std::to_string(n)+"+"+rel(intels[n].data()); }
    for(size_t n=0;n<tasks.size();++n)if(in(tasks[n].data(),tasks[n].size()))return "task#"+std::to_string(n)+"+"+rel(tasks[n].data());
    for(size_t n=0;n<vehs.size();++n)if(in(vehs[n].data(),vehs[n].size()))return "veh#"+std::to_string(n)+"+"+rel(vehs[n].data());
    for(size_t n=0;n<ents.size();++n)if(in(ents[n].data(),ents[n].size()))return "ent#"+std::to_string(n)+"+"+rel(ents[n].data());
    for(size_t n=0;n<newBufs.size();++n)if(in(newBufs[n].data(),newBufs[n].size()))return "newbuf#"+std::to_string(n)+"+"+rel(newBufs[n].data());
    const std::pair<const void*,size_t> bufs[]{{groupBuf.data(),groupBuf.size()},{gEvent.data(),gEvent.size()},{gInner.data(),gInner.size()},{playerPed.data(),playerPed.size()},
        {playerIntel.data(),playerIntel.size()},{object.data(),object.size()},{objMat.data(),objMat.size()},{scripts.data(),scripts.size()},{camMat.data(),camMat.size()},
        {fxSplash.data(),fxSplash.size()},{fxSand.data(),fxSand.size()},{streakX.data(),sizeof(streakX)},{streakY.data(),sizeof(streakY)},{streakZ.data(),sizeof(streakZ)},
        {streakS.data(),streakS.size()},{evArena.data(),evArena.size()},{fakeTaskNew.data(),fakeTaskNew.size()},{pedVtbl.data(),sizeof(pedVtbl)},{taskVtbl.data(),sizeof(taskVtbl)},{evVtbl.data(),sizeof(evVtbl)}};
    static const char* names[]{"group","gevent","ginner","player","pintel","object","omat","scripts","cammat","fxsplash","fxsand","sx","sy","sz","ss","evarena","tasknew","pvt","tvt","evt"};
    for(size_t k=0;k<std::size(bufs);++k)if(in(bufs[k].first,bufs[k].second))return std::string(names[k])+"+"+rel(bufs[k].first);
    if(v>=ROPES&&v<ROPES+NROPES*ROPE_SIZE)return "rope#"+std::to_string((v-ROPES)/ROPE_SIZE)+"+"+std::to_string((v-ROPES)%ROPE_SIZE);
    // La pila puede caer en el rango del EXE: se comprueba antes (su direccion difiere entre EXE y DLL)
    const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack";
    if(v>=0x400000&&v<0x1000000)return v;
    return "ext";
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json Words(uintptr_t a,size_t n) { json r=json::array(); const auto* w=reinterpret_cast<const uint32*>(a); for(size_t k=0;k<n/4;++k)r.push_back(w[k]); return r; }

// --- vtables falsas (trampas para toda ranura no prevista) ---
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_virtual",N,Name(uintptr_t(self))}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline void __fastcall FTeleport(CPed* p,void*,CVector pos,bool reset) { trace.push_back({"teleport",Name(p),F(pos.x),F(pos.y),F(pos.z),reset}); }
inline void* __fastcall FSub(CTask* t,void*) { trace.push_back({"sub_task",Name(t)}); return At<void*>(t,0x10); }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"task_type",Name(t)}); return At<int32>(t,0x14); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable",Name(t),Name(p),prio,Name(e)}); return true; }
inline int __fastcall EType(CEvent* e,void*) { trace.push_back({"event_type",Name(e)}); return At<int32>(e,0x18); }

// --- configuracion por caso de las dependencias simuladas ---
inline uint32 noRainMask{}, noRainCalls{};
inline bool __cdecl FakeCamNoRain() { trace.push_back({"cam_no_rain"}); return (noRainMask>>(noRainCalls++%32))&1; }
inline bool __cdecl FakePlayerNoRain() { trace.push_back({"player_no_rain"}); return (noRainMask>>(noRainCalls++%32))&1; }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerPed.data()); }
inline void* fakeVehResult{}; inline CVector playerCoors{};
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote) { trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(fakeVehResult); }
inline CVector* __cdecl FakeFindPlayerCoors(CVector* out,int32 id) { trace.push_back({"find_player_coors",id}); *out=playerCoors; return out; }
inline bool lookingLRB{}, carHasRoof{};
inline bool __fastcall FakeLookingLRB(void* self,void*) { trace.push_back({"looking_lrb",Name(self)}); return lookingLRB; }
inline bool __fastcall FakeCarHasRoof(void* self,void*) { trace.push_back({"car_has_roof",Name(self)}); return carHasRoof; }
inline bool pvlHit{}; inline float pvlZ{};
inline bool __cdecl FakePVL(const CVector& o,float d,CColPoint& cp,CEntity*& e,bool b,bool v,bool p,bool ob,bool du,bool see,CStoredCollPoly* poly) {
    trace.push_back({"process_vertical_line",F(o.x),F(o.y),F(o.z),F(d),b,v,p,ob,du,see,poly!=nullptr});
    if(pvlHit){ cp.m_vecPoint=CVector{o.x,o.y,pvlZ}; e=nullptr; } return pvlHit; }
inline void __fastcall FakeAddParticle(void* self,void*,const CVector* pos,const CVector* vel,float ts,const FxPrtMult_c* m,float rotZ,float lm,float lml,bool local) {
    trace.push_back({"add_particle",Name(self),json::array({F(pos->x),F(pos->y),F(pos->z)}),json::array({F(vel->x),F(vel->y),F(vel->z)}),F(ts),
        json::array({F(At<float>((void*)m,0)),F(At<float>((void*)m,4)),F(At<float>((void*)m,8)),F(At<float>((void*)m,0xC)),F(At<float>((void*)m,0x10)),F(At<float>((void*)m,0x14)),F(At<float>((void*)m,0x18))}),
        F(rotZ),F(lm),F(lml),local}); }
inline void __fastcall FakeAddAudio(void* self,void*,int32 ev) { trace.push_back({"add_audio_event",Name(self),ev}); }
inline void __fastcall FakeService(void* self,void*) { trace.push_back({"audio_service",Name(self)}); }
inline void __fastcall FakeStartShake(void* self,void*,int16 t,uint8 f,uint32 d) { trace.push_back({"start_shake",Name(self),t,f,d}); }
inline void __cdecl FakeAddRain() { trace.push_back({"add_rain"}); }
inline void __cdecl FakeUpdateTunnel() { trace.push_back({"update_in_tunnelness"}); }
// render / Im3D
inline RwBool __cdecl StateSet(RwRenderState st,void* v) { trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f) { const size_t k=std::min<RwUInt32>(n,2048);
    trace.push_back({"im3d_transform",intptr_t(v)-intptr_t(TEMP_VERTS),n,uintptr_t(m),f,Fnv(v,k*0x24)}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRenderIndexed(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n) { const size_t k=n<0 ? 0 : size_t(std::min<RwInt32>(n,4096));
    trace.push_back({"im3d_render_indexed",int(t),uintptr_t(idx),n,Fnv(idx,k*2)}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd() { trace.push_back({"im3d_end"}); return TRUE; }
// operator new[] (sin trazas con json: registro en un array fijo)
inline std::array<uint32,8> newLog{}; inline uint32 newCount{};
inline void* __cdecl FakeNewArr(size_t n) { const auto k=newCount++; if(k<newLog.size())newLog[k]=uint32(n); return newBufs[k%newBufs.size()].data(); }
// eventos (arena propia)
inline uint32 evNew{};
inline void* __cdecl FakeEventNew(unsigned n) { trace.push_back({"event_new",n}); return evArena.data()+size_t(evNew++%4)*0x40; }
inline void __cdecl FakeEventDelete(void* p) { trace.push_back({"event_delete",Name(p)}); }

// grupos de peatones
inline bool addEventRet{};
inline bool __fastcall FakeAddEvent(void* self,void*,CEvent* ev) {
    auto* ge=reinterpret_cast<CEventGroupEvent*>(ev); auto* in=reinterpret_cast<CEventPlayerCommandToGroup*>(ge->m_event);
    trace.push_back({"add_event",Name(self),int(ev->GetEventType()),Name(ge->m_ped),int(in->GetEventType()),int(in->m_command),Name(in->m_target)}); return addEventRet; }
inline int16 __fastcall FakeSay(CPed* p,void*,int32 ctx,uint32 delay,float prob,bool a,bool b,bool c) { trace.push_back({"say",Name(p),ctx,delay,F(prob),a,b,c}); return 0; }
inline bool __fastcall FakePosOut(CPed* p,void*) { trace.push_back({"position_out_of_collision",Name(p)}); return true; }
inline void __fastcall FakeRemoveMember(void* self,void*,int32 idx) { trace.push_back({"remove_member",Name(self),idx}); }
inline void __fastcall FakeIntelFlush(void* self,void*) { trace.push_back({"intelligence_flush",Name(self)}); }
inline void __fastcall FakeIntelProcess(void* self,void*) { trace.push_back({"intelligence_process",Name(self)}); }
inline void __fastcall FakeMemProcess(void* self,void*) { trace.push_back({"membership_process",Name(self)}); }
inline std::array<std::array<int,2>,NPED> findCfg{};          // por peaton: tarea de conductor (701) y de pasajero (700) -> indice de tarea falsa o -1
inline CTask* __fastcall FakeFindTask(void* self,void*,int32 type) {
    int n=-1; for(int k=0;k<NPED;++k)if(self==intels[k].data())n=k;
    trace.push_back({"find_task_by_type",Name(self),type});
    if(n<0||(type!=701&&type!=700))return nullptr; const int c=findCfg[n][type==700]; return c<0 ? nullptr : reinterpret_cast<CTask*>(tasks[c].data()); }
inline uint32 inCarMask{}, inCarCalls{};
inline void* __fastcall FakeInCar(void* self,void*) { trace.push_back({"is_in_car_or_entering",Name(self)}); return ((inCarMask>>(inCarCalls++%32))&1) ? vehs[0].data() : nullptr; }
inline uint32 destMask{}, destCalls{}, shrinkCfg{};
inline void __cdecl FakeGenGather(CPedList* l,CPed* ped) { trace.push_back({"generate_gather_destinations",Name(ped),l->m_count}); if(l->m_count>=shrinkCfg)l->m_count-=shrinkCfg; }
inline void __cdecl FakeGenGatherCar(CPedList* l,CVehicle* v) { trace.push_back({"generate_gather_destinations_around_car",Name(v),l->m_count}); }
inline void __cdecl FakeDistribute(CPedList* l) { json m=json::array(); for(uint32 k=0;k<std::min<uint32>(l->m_count,30);++k)m.push_back(Name(l->m_peds[k])); trace.push_back({"distribute_destinations",l->m_count,m}); }
inline bool __cdecl FakeReturnDest(CPed* ped,CVector* out) { const bool r=(destMask>>(destCalls++%32))&1; trace.push_back({"return_destination_for_ped",Name(ped),r});
    if(r)*out=CVector{float(destCalls)*1.5f,-2.25f*float(destCalls),7.5f}; return r; }
inline void __fastcall FakeSetTask(void* self,void*,CPed* ped,CTask* t,void* pairs,int32 slot,bool force) {
    auto* g=reinterpret_cast<CTaskSimpleGoToPoint*>(t);
    trace.push_back({"set_task",Name(self),Name(ped),Name(pairs),slot,force,int(g->GetTaskType()),int(g->m_moveState),json::array({F(g->m_vecTargetPoint.x),F(g->m_vecTargetPoint.y),F(g->m_vecTargetPoint.z)}),F(g->m_fRadius),
                     bool(g->gotoPointFlags.m_bMoveTowardsTargetPoint),bool(g->gotoPointFlags.m_b04)}); }

// scripts de cerebros, streaming, cuerdas
inline void __fastcall FakeStartNewBrain(void* self,void*,uint32 idx,CEntity* e,uint32 flag) { trace.push_back({"start_new_streamed_script_brain",Name(self),uint8(idx),Name(e),uint8(flag)}); }
inline void __cdecl FakeAddToWaiting(CEntity* e,int16 idx) { trace.push_back({"add_to_waiting_array",Name(e),idx}); }
inline void __cdecl FakeRequestModel(int32 model,int32 flags) { trace.push_back({"request_model",model,flags}); }
inline void __fastcall FakeRopeRemove(CRope* r,void*) { trace.push_back({"rope_remove",Name(r)}); }
inline void __fastcall FakeRopeUpdate(CRope* r,void*) { trace.push_back({"rope_update",Name(r)}); }
inline void __fastcall FakeRopeRender(CRope* r,void*) { trace.push_back({"rope_render",Name(r)}); }
inline void __fastcall FakeRopeHook(CRope* r,void*) { trace.push_back({"rope_create_hook_object",Name(r),Fnv(r,ROPE_SIZE)}); }
inline void __fastcall FakeRegRef(CEntity* e,void*,CEntity** pp) { trace.push_back({"register_reference",Name(e),Name(pp)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CScriptsForBrains","GetIndexOfScriptBrainWithThisName",0x46AA30,0,-1,512},
    {"CScriptsForBrains","HasAttractorScriptBrainWithThisNameLoaded",0x46AB20,0,-1,512},
    {"CScriptsForBrains","Init",0x46A8C0,0,-1,256},
    {"CScriptsForBrains","IsObjectWithinBrainActivationRange",0x46B3D0,0,-1,1024},
    {"CScriptsForBrains","StartAttractorScriptBrainWithThisName",0x46B390,0,-1,512},
    {"CScriptsForBrains","StartOrRequestNewStreamedScriptBrain",0x46CD80,0,-1,1024},
    {"CPedGroup","Constructor",0x5FC150,0,-1,256},
    {"CPedGroup","Destructor",0x5FC190,0,-1,256},
    {"CPedGroup","FindDistanceToFurthestMember",0x5FB010,0,-1,512},
    {"CPedGroup","FindDistanceToNearestMember",0x5FB0A0,0,-1,512},
    {"CPedGroup","Flush",0x5FB790,0,-1,256},
    {"CPedGroup","GetClosestGroupPed",0x5FACD0,0,-1,512},
    {"CPedGroup","IsAnyoneUsingCar",0x5F7DB0,0,-1,512},
    {"CPedGroup","PlayerGaveCommand_Attack",0x5F7CC0,0,-1,512},
    {"CPedGroup","PlayerGaveCommand_Gather",0x5FAB60,0,-1,512},
    {"CPedGroup","Process",0x5FC7E0,0,-1,256},
    {"CPedGroup","RemoveAllFollowers",0x5FB7D0,0,-1,256},
    {"CPedGroup","Teleport",0x5F7AD0,0,-1,768},
    {"CWeather","AddRain",0x72A9A0,0,-1,768},
    {"CWeather","AddSandStormParticles",0x72A820,0,-1,256},
    {"CWeather","FindWeatherTypesList",0x72A520,0,-1,256},
    {"CWeather","ForceWeather",0x72A4E0,0,-1,256},
    {"CWeather","ForceWeatherNow",0x72A4F0,0,-1,256},
    {"CWeather","ForecastWeather",0x72A590,0,-1,768},
    {"CWeather","Init",0x72A480,0,-1,256},
    {"CWeather","IsRainy",0x4ABF50,0,-1,256},
    {"CWeather","ReleaseWeather",0x72A510,0,-1,256},
    {"CWeather","RenderRainStreaks",0x72AF70,0,-1,768},
    {"CWeather","SetWeatherToAppropriateTypeNow",0x72A790,0,-1,512},
    {"CWeather","Update",0x72B850,0,-1,1536},
    {"CWeather","UpdateInTunnelness",0x72B630,0,-1,768},
    {"CRopes","CreateRopeForSwatPed",0x558D10,0,-1,512},
    {"CRopes","FindCoorsAlongRope",0x555E40,0,-1,768},
    {"CRopes","FindPickupHeight",0x556760,0,-1,256},
    {"CRopes","FindRope",0x556000,0,-1,512},
    {"CRopes","Init",0x555DC0,0,-1,256},
    {"CRopes","IsCarriedByRope",0x555F80,0,-1,512},
    {"CRopes","RegisterRope",0x556B40,0,-1,1024},
    {"CRopes","Render",0x556AE0,0,-1,256},
    {"CRopes","SetSpeedOfTopNode",0x555DF0,0,-1,512},
    {"CRopes","Shutdown",0x556B10,0,-1,256},
    {"CRopes","Update",0x558D70,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"misc61a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"misc61a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("misc61a");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"misc61a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables falsas
    FillTraps(pedVtbl,std::make_integer_sequence<int,128>{}); FillTraps(taskVtbl,std::make_integer_sequence<int,128>{}); FillTraps(evVtbl,std::make_integer_sequence<int,128>{});
    pedVtbl[14]=reinterpret_cast<void*>(&FTeleport);
    taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[4]=reinterpret_cast<void*>(&FType); taskVtbl[6]=reinterpret_cast<void*>(&FAbort);
    evVtbl[1]=reinterpret_cast<void*>(&EType);

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xC812A8,0xB8},{0xC813E0,0x80},                                  // CWeather (sin el CAEWeatherAudioEntity de 0xC81360)
            {0x8D5EAC,4},{0x8D6078,4},
            {0xB6F028,0x20},{0xB6F178,4},{0xB6F9AC,12},                       // TheCamera: posicion, heading, m_matrix; orientacion; frente de 0x984
            {0xB7CA50,0xC0},{0xB79E48,4},{0xB79FD0,4},{0xB7C540,2},          // time cycle: vectores al sol, FogReduction, valor guardado, palabra de niebla de agua
            {0xB70150,6},{0xB7CB48,1},{0xB7CB4C,4},{0xB7CB5C,4},{0xB7CB84,4},{0xB72914,4},
            {0xC7C714,1},{0xB5F851,1},{0xC87ABC,4},{0xA43088,1},
            {TEMP_NV,8},{TEMP_IDX,0x200},{TEMP_VERTS,0x24*80},
            {0xA9AE1C,0x18},                                                  // g_fx.m_Sand2 ... m_Splash
            {ROPES,NROPES*ROPE_SIZE},{0xB76898,4},{0xB781F8,4},
            {SCM_INFOS,SCM_COUNT*INFO_SIZE},{MODEL_PTRS+4*MI_BASE,4*MI_COUNT}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch n1(Body(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CEvent::operator new))),FakeEventNew), n1e(0x4B5620,FakeEventNew);
        EntryPatch n2(Body(bit_cast<uintptr_t>(static_cast<void(*)(void*)>(&CEvent::operator delete))),FakeEventDelete), n2e(0x4B5630,FakeEventDelete);
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRenderIndexed), i4(0x7EF520,FakeIm3DEnd);                 // RwIm3D* de la DLL son envoltorios
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CCullZones::CamNoRain)),FakeCamNoRain), c1e(0x72DDB0,FakeCamNoRain);
        EntryPatch c2(Body(bit_cast<uintptr_t>(&CCullZones::PlayerNoRain)),FakePlayerNoRain), c2e(0x72DDC0,FakePlayerNoRain);
        EntryPatch f1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FakeFindPlayerPed), f1e(0x56E210,FakeFindPlayerPed);
        EntryPatch f2(Body(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle))),FakeFindPlayerVehicle), f2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch f3(Body(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors))),FakeFindPlayerCoors), f3e(0x56E010,FakeFindPlayerCoors);
        EntryPatch w1(Body(MemberAddress(&CCamera::GetLookingLRBFirstPerson)),FakeLookingLRB), w1e(0x50AE60,FakeLookingLRB);
        EntryPatch w2(Body(MemberAddress(&CVehicle::CarHasRoof)),FakeCarHasRoof), w2e(0x6D25D0,FakeCarHasRoof);
        EntryPatch w3(Body(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine)),FakePVL), w3e(0x5674E0,FakePVL);
        EntryPatch w4(Body(MemberAddress(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle))),FakeAddParticle), w4e(0x4AA440,FakeAddParticle);
        EntryPatch w5(Body(MemberAddress(&CAEWeatherAudioEntity::AddAudioEvent)),FakeAddAudio), w5e(0x506800,FakeAddAudio);
        EntryPatch w6(Body(MemberAddress(&CAEWeatherAudioEntity::Service)),FakeService), w6e(0x5052F0,FakeService);
        EntryPatch w7(Body(MemberAddress(&CPad::StartShake)),FakeStartShake), w7e(0x53F920,FakeStartShake);
        EntryPatch g1(Body(MemberAddress(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent))),FakeAddEvent), g1e(0x5F7470,FakeAddEvent);
        EntryPatch g2(Body(MemberAddress(&CPed::Say)),FakeSay), g2e(0x5EFFE0,FakeSay);
        EntryPatch g3(Body(MemberAddress(&CPed::PositionAnyPedOutOfCollision)),FakePosOut), g3e(0x5E13C0,FakePosOut);
        EntryPatch g4(Body(MemberAddress(static_cast<void(CPedGroupMembership::*)(int32)>(&CPedGroupMembership::RemoveMember))),FakeRemoveMember), g4e(0x5F80D0,FakeRemoveMember);
        EntryPatch g5(Body(MemberAddress(&CPedGroupIntelligence::Flush)),FakeIntelFlush), g5e(0x5F7350,FakeIntelFlush);
        EntryPatch g6(Body(MemberAddress(&CPedGroupIntelligence::Process)),FakeIntelProcess), g6e(0x5FC4A0,FakeIntelProcess);
        EntryPatch g7(Body(MemberAddress(&CPedGroupMembership::Process)),FakeMemProcess), g7e(0x5FBA60,FakeMemProcess);
        EntryPatch g8(Body(MemberAddress(&CPedIntelligence::FindTaskByType)),FakeFindTask), g8e(0x600EE0,FakeFindTask);
        EntryPatch g9(Body(MemberAddress(&CPedIntelligence::IsInACarOrEnteringOne)),FakeInCar), g9e(0x601CC0,FakeInCar);
        EntryPatch ga(Body(MemberAddress(&CPedGroupIntelligence::SetTask)),FakeSetTask), gae(0x5F7540,FakeSetTask);
        EntryPatch gb(0x69A620,FakeGenGather), gc(0x69A770,FakeGenGatherCar), gd(0x69B240,FakeDistribute), ge(0x699FA0,FakeReturnDest);   // CFormation::* de la DLL llaman al EXE
        // CFormation::* ya es C++ en la DLL y PedGroup.cpp lo llama directo: tambien se intercepta la entrada de la DLL con el mismo Fake
        EntryPatch gbd(Body(bit_cast<uintptr_t>(&CFormation::GenerateGatherDestinations)),FakeGenGather), gcd(Body(bit_cast<uintptr_t>(&CFormation::GenerateGatherDestinations_AroundCar)),FakeGenGatherCar), gdd(Body(bit_cast<uintptr_t>(&CFormation::DistributeDestinations)),FakeDistribute), ged(Body(bit_cast<uintptr_t>(&CFormation::ReturnDestinationForPed)),FakeReturnDest);
        EntryPatch s1(0x46B270,FakeStartNewBrain);                                                                                        // la DLL lo llama por direccion del EXE
        EntryPatch s2(Body(bit_cast<uintptr_t>(&CTheScripts::AddToWaitingForScriptBrainArray)),FakeAddToWaiting), s2e(0x46AB60,FakeAddToWaiting);
        EntryPatch s3(Body(bit_cast<uintptr_t>(&CStreaming::RequestModel)),FakeRequestModel), s3e(0x4087E0,FakeRequestModel);
        EntryPatch o1(Body(MemberAddress(&CRope::Remove)),FakeRopeRemove), o1e(0x556780,FakeRopeRemove);
        EntryPatch o2(Body(MemberAddress(&CRope::Update)),FakeRopeUpdate), o2e(0x557530,FakeRopeUpdate);
        EntryPatch o3(Body(MemberAddress(&CRope::Render)),FakeRopeRender), o3e(0x556800,FakeRopeRender);
        EntryPatch o4(Body(MemberAddress(&CRope::CreateHookObjectForRope)),FakeRopeHook), o4e(0x556070,FakeRopeHook);
        EntryPatch o5(Body(MemberAddress(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference))),FakeRegRef), o5e(0x571B70,FakeRegRef);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;

        auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C); auto& camPos=*reinterpret_cast<CVector*>(0xB6F02C); auto& camHeading=*reinterpret_cast<float*>(0xB6F038);
        auto& camOrient=*reinterpret_cast<float*>(0xB6F178); auto& camFwd984=*reinterpret_cast<CVector*>(0xB6F9AC);
        auto& streakPosX=*reinterpret_cast<int32**>(0xC81420); auto& streakPosY=*reinterpret_cast<int32**>(0xC8141C); auto& streakPosZ=*reinterpret_cast<int32**>(0xC81418); auto& streakStr=*reinterpret_cast<uint8**>(0xC81414);
        auto& nv=*reinterpret_cast<int32*>(TEMP_NV); auto& ni=*reinterpret_cast<int32*>(TEMP_NI);
        auto& heatMinute=*reinterpret_cast<int32*>(0x8D6078);
        auto* ropes=reinterpret_cast<uint8*>(ROPES); auto& playerCrane=*reinterpret_cast<int32*>(0xB76898); auto& ropeCounter=*reinterpret_cast<uint32*>(0xB781F8);
        auto** modelPtrs=reinterpret_cast<void**>(MODEL_PTRS);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            std::vector<size_t> sameClass; for(size_t k=0;k<fns.size();++k)if(cls==fns[k].cls)sameClass.push_back(k);
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto pick=[&](std::initializer_list<uint32> l){return *(l.begin()+next()%l.size());};
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; };   // sin NaN
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto weightedUnit=[&]{ switch(next()%7){case 0:return 0.f;case 1:return 1.f;case 2:return uni(-0.3f,0.f);case 3:return uni(1.f,1.3f);default:return uni(0.f,1.f);} };
                const float NaN=std::numeric_limits<float>::quiet_NaN();
                // --- camara ---
                auto setCamera=[&](bool highZ){ const CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),highZ&&next()%4==0 ? uni(850.f,1000.f) : uni(-50.f,300.f)};
                    if(next()%4){ std::memset(camMat.data(),0,camMat.size()); for(int k=0;k<12;++k)At<float>(camMat.data(),k*4)=uni(-1.f,1.f); At<CVector>(camMat.data(),0x30)=p; camMatrix=camMat.data(); camPos=randVec(-10.f,10.f); }
                    else { camMatrix=nullptr; camPos=p; }
                    camHeading=uni(-7.f,7.f); camOrient=uni(-4.f,4.f); camFwd984=randVec(-1.f,1.f); return p; };
                // --- peatones y grupo ---
                auto buildPeds=[&](bool vt){
                    for(int n=0;n<NPED;++n){
                        auto* b=PedBuf(n); std::memset(b,0,PED_STRIDE); if(vt)At<void*>(b,0)=pedVtbl.data();
                        float x=uni(-60.f,60.f), y=uni(-60.f,60.f), z=uni(-10.f,30.f);
                        if(next()%50==0)(next()%2 ? x : z)=NaN;
                        if(next()%4==0){ std::memset(pedMats[n].data(),0,pedMats[n].size()); At<float>(pedMats[n].data(),0x30)=x; At<float>(pedMats[n].data(),0x34)=y; At<float>(pedMats[n].data(),0x38)=z; At<void*>(b,P_MAT)=pedMats[n].data(); }
                        else { At<float>(b,P_POS)=x; At<float>(b,P_POS+4)=y; At<float>(b,P_POS+8)=z; }
                        b[P_AREA]=uint8(pick({0,0,0,1,3}));
                        At<uint32>(b,P_FLAGS)=next()%3==0 ? 0x100u : 0u;
                        std::memset(intels[n].data(),0,intels[n].size()); At<void*>(b,P_INTEL)=intels[n].data();
                        b[P_CREATED]=uint8(pick({1,2,2,3}));
                        At<int32>(b,P_STATE)=int32(pick({1,1,1,1,0x36,0x37,4,0x29}));
                        At<void*>(b,P_VEH)=next()%3 ? vehs[next()%4].data() : nullptr;
                        At<int32>(b,P_TYPE)=int32(pick({0,1,4,4,4,6,7,8,9,12,16,17}));
                    } };
                auto fillGroup=[&](bool leaderOptional){
                    std::memset(groupBuf.data(),0,groupBuf.size()); At<void*>(groupBuf.data(),8)=groupBuf.data(); At<void*>(groupBuf.data(),0x30)=groupBuf.data();
                    std::array<int,NPED> perm{}; for(int k=0;k<NPED;++k)perm[k]=k; for(int k=NPED-1;k>0;--k)std::swap(perm[k],perm[next()%(k+1)]);
                    const uint32 mode=next()%5;
                    for(int k=0;k<8;++k){
                        bool on; int who=perm[k];
                        switch(mode){ case 0: on=next()%3!=0; break; case 1: on=next()%3!=0; who=int(next()%NPED); break; case 2: on=k<7||next()%2; break; case 3: on=next()%5==0; break; default: on=next()%2!=0; if(next()%4==0)who=perm[next()%8]; break; }
                        if(k==7&&leaderOptional&&next()%4==0)on=false;
                        At<void*>(groupBuf.data(),0xC+4*size_t(k))=on ? PedBuf(who) : nullptr;
                    }
                    At<float>(groupBuf.data(),0x2C)=next()%2 ? 60.f : uni(0.f,80.f); groupBuf[0x2D0]=uint8(next()%2); };
                auto slot=[&](int k)->CPed*&{ return At<CPed*>(groupBuf.data(),0xC+4*size_t(k)); };
                // --- el jugador falso de CWeather (area, inteligencia y arbol de tareas) ---
                auto buildPlayer=[&]{
                    std::memset(playerPed.data(),0,playerPed.size()); std::memset(playerIntel.data(),0,playerIntel.size());
                    playerPed[P_AREA]=next()%6==0 ? uint8(1+next()%5) : 0; At<void*>(playerPed.data(),P_INTEL)=playerIntel.data();
                    for(auto& t:tasks){ std::memset(t.data(),0,t.size()); At<void*>(t.data(),0)=taskVtbl.data(); At<void*>(t.data(),0x10)=nullptr; At<int32>(t.data(),0x14)=next()%3==0 ? 254 : int32(next()%400); }
                    for(int k=0;k<5;++k)At<void*>(playerIntel.data(),4+4*size_t(k))=next()%3==0 ? tasks[next()%8].data() : nullptr;   // m_aPrimaryTasks
                    for(int k=0;k<8;++k)if(next()%2)At<void*>(tasks[k].data(),0x10)=next()%2 ? tasks[next()%8].data() : nullptr;      // subtareas (puede haber ciclos: el EXE se colgaria)
                    // se rompen los ciclos: cada subtarea apunta solo a un indice mayor
                    for(int k=0;k<8;++k){ auto* s=At<uint8*>(tasks[k].data(),0x10); if(s){ int idx=-1; for(int m=0;m<8;++m)if(s==tasks[m].data())idx=m; if(idx<=k)At<void*>(tasks[k].data(),0x10)=nullptr; } } };
                auto setTimeGlobals=[&]{
                    CClock::ms_nGameClockSeconds=uint16(next()%60)|uint16(next()%4==0 ? 0x1200 : 0); CClock::ms_nGameClockMinutes=uint8(next()%60); CClock::ms_nGameClockHours=uint8(next()%24);
                    CTimer::ms_fTimeStep=next()%8==0 ? 0.f : next()%16==0 ? 100.f : uni(0.f,4.f); CTimer::m_snTimeInMilliseconds=next()%2 ? next() : (next()&~0x1FFFu)|(next()%800); CTimer::m_FrameCounter=next();
                    CTimer::m_CodePause=next()%8==0; CGame::currArea=eAreaCodes(next()%8==0 ? int32(1+next()%17) : 0); };
                auto weatherType=[&]()->int16{ static const int16 rainy[]{8,16}, sunny[]{1,3,14,5,10,18}, xs[]{0,2,13,6,11,17}, foggy[]{9,19}, cloudy[]{4,15,12,7};
                    switch(next()%8){ case 0: return rainy[next()%2]; case 1: return sunny[next()%6]; case 2: return xs[next()%6]; case 3: return foggy[next()%2]; case 4: return cloudy[next()%4]; case 5: return int16(next()%23); default: return int16(next()%23); } };
                auto weatherState=[&]{
                    CWeather::NewWeatherType=eWeatherType(weatherType()); CWeather::OldWeatherType=eWeatherType(weatherType());
                    CWeather::ForcedWeatherType=next()%3 ? WEATHER_UNDEFINED : eWeatherType(next()%23);
                    CWeather::WeatherRegion=eWeatherRegion(next()%8==0 ? int16(next()%7)-1 : int16(next()%5)); CWeather::WeatherTypeInList=next()%4==0 ? next()%0x7FFF0000u : next()%64;
                    CWeather::InterpolationValue=next()%8==0 ? (next()%2 ? 0.f : 1.f) : uni(0.f,1.f);
                    CWeather::Rain=weightedUnit(); CWeather::Sandstorm=weightedUnit(); CWeather::WetRoads=weightedUnit(); CWeather::CloudCoverage=weightedUnit(); CWeather::Foggyness=weightedUnit(); CWeather::Foggyness_SF=weightedUnit();
                    CWeather::ExtraSunnyness=weightedUnit(); CWeather::Rainbow=weightedUnit(); CWeather::SunGlare=weightedUnit(); CWeather::HeatHaze=weightedUnit(); CWeather::HeatHazeFXControl=weightedUnit();
                    CWeather::Wavyness=weightedUnit(); CWeather::HeadLightsSpectrum=weightedUnit(); CWeather::TrafficLightsBrightness=weightedUnit(); CWeather::InTunnelness=weightedUnit(); CWeather::WaterFogFXControl=weightedUnit();
                    CWeather::UnderWaterness=next()%4==0 ? uni(0.f,1.f) : next()%6==0 ? uni(-0.5f,0.f) : 0.f; CWeather::Wind=uni(0.f,1.6f); CWeather::WindClipped=weightedUnit(); CWeather::WindDir=randVec(-1.f,1.f);
                    CWeather::CurrentRainParticleStrength=next()%8==0 ? uint32(next()) : next()%4==0 ? 0u : next()%100; CWeather::StreamAfterRainTimer=next()%4==0 ? int32(next()%900)-20 : 0;
                    CWeather::StreamAfterRainFlag=next()%2; CWeather::RainFXAlpha=next()%4==0 ? 0.f : weightedUnit(); CWeather::LightningFlash=next()%2; CWeather::LightningBurst=next()%2;
                    CWeather::LightningStart=next()%2 ? CTimer::m_FrameCounter-next()%40 : next(); CWeather::LightningFlashLastChange=int32(CTimer::m_snTimeInMilliseconds-uint32(next()%200));
                    CWeather::LightningDuration=next()%30; CWeather::WhenToPlayLightningSound=next()%2 ? 0 : int32(CTimer::m_snTimeInMilliseconds+uint32(int32(next()%400)-200));
                    noRainMask=next()%3==0 ? next()&next() : next()%2 ? 0u : next(); noRainCalls=0; };

                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x61A1u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0; noRainCalls=0; inCarCalls=0; destCalls=0; evNew=0; newCount=0;
                    json out; json ret=nullptr; const uintptr_t a=fn.address;
                    const auto Switch=[&]{ for(size_t k:sameClass)hooks[k]->State(candidate); return ReversingProbe::JumpsToDll(a)==candidate; };

                    // ================= CScriptsForBrains =================
                    if(cls=="CScriptsForBrains"){
                        static const char* names[]{"AMB1","amb1","TEST","CJ","WRKCLO","ATM","bar_x","LONGNAME","Ab","aB","","test"};
                        auto fillScripts=[&]{
                            for(size_t k=0;k<scripts.size();++k)scripts[k]=uint8(next());
                            for(int n=0;n<70;++n){ auto* s=scripts.data()+size_t(n)*20;
                                At<int16>(s,0)=int16(next()%6==0 ? int32(next()%4)-3 : int32(next()%80)); s[2]=uint8(int8(pick({0xFF,0,1,2,3,4,5,5,5,5}))); s[3]=uint8(next()%4==0 ? 0xFF : next()%6);
                                s[4]=uint8(pick({0,1,1,2})); At<float>(s,8)=next()%6==0 ? 0.f : uni(0.f,20.f);
                                std::memset(s+0xC,0,8); const char* nmv=names[next()%std::size(names)]; std::memcpy(s+0xC,nmv,std::min<size_t>(strlen(nmv),8)); } };
                        auto setLoadStates=[&]{ for(size_t k=0;k<SCM_COUNT;++k)CStreaming::ms_aInfoForModel[SCMToModelId(int32(k))].m_LoadState=eStreamingLoadState(next()%3==0 ? 1 : next()%5); };
                        auto setName=[&](bool fromTable){ std::memset(nameBuf.data(),0,nameBuf.size()); const char* nmv=fromTable ? names[next()%std::size(names)] : "zzz"; if(fromTable&&next()%2){ std::string t=nmv; for(auto& c:t)c=char(next()%2 ? toupper(c) : tolower(c)); std::memcpy(nameBuf.data(),t.c_str(),t.size()); } else std::memcpy(nameBuf.data(),nmv,strlen(nmv)); };
                        void* self=scripts.data();
                        if(nm=="Init"){ for(size_t k=0;k<scripts.size();++k)scripts[k]=uint8(next()); out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="GetIndexOfScriptBrainWithThisName"){ fillScripts(); setName(next()%4); const int8 type=int8(pick({0xFF,0,1,2,3,4,5,5,5,6,100})); out["switches"]=Switch();
                            ret=int16(reinterpret_cast<int16(__thiscall*)(void*,const char*,int8)>(a)(self,nameBuf.data(),type)); }
                        else if(nm=="HasAttractorScriptBrainWithThisNameLoaded"){ fillScripts(); setLoadStates(); setName(next()%4); out["switches"]=Switch();
                            ret=reinterpret_cast<bool(__thiscall*)(void*,const char*)>(a)(self,nameBuf.data()); }
                        else if(nm=="IsObjectWithinBrainActivationRange"){ fillScripts(); std::memset(object.data(),0,object.size()); std::memset(objMat.data(),0,objMat.size());
                            const int idx=int(next()%70); At<int16>(object.data(),0x168)=int16(idx); auto* s=scripts.data()+size_t(idx)*20; if(next()%4==0)s[2]=1; else if(next()%5)s[2]=1;
                            CVector pos=next()%8==0 ? CVector{0.f,0.f,0.f} : randVec(-300.f,300.f);
                            if(next()%2){ At<float>(objMat.data(),0x30)=pos.x; At<float>(objMat.data(),0x34)=pos.y; At<float>(objMat.data(),0x38)=pos.z; At<void*>(object.data(),0x14)=objMat.data(); }
                            else { At<float>(object.data(),4)=pos.x; At<float>(object.data(),8)=pos.y; At<float>(object.data(),12)=pos.z; }
                            CVector point; const float radius=At<float>(s,8);
                            if(next()%8==0){ pos=CVector{0.f,0.f,0.f}; if(At<void*>(object.data(),0x14)){ At<float>(objMat.data(),0x30)=0.f; At<float>(objMat.data(),0x34)=0.f; At<float>(objMat.data(),0x38)=0.f; } else { At<float>(object.data(),4)=0.f; At<float>(object.data(),8)=0.f; At<float>(object.data(),12)=0.f; }
                                At<float>(s,8)=5.f; point=CVector{3.f,next()%2 ? 4.f : -4.f,0.f}; }
                            else { const CVector dir=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float dm=std::sqrt(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z)+0.001f;
                                static const float scale[]{0.f,1.f,0.999f,1.001f,0.5f,2.f,1.0000001f,0.9999999f}; const float d=radius*scale[next()%8];
                                point=CVector{pos.x+dir.x/dm*d,pos.y+dir.y/dm*d,pos.z+dir.z/dm*d}; }
                            out["switches"]=Switch(); ret=reinterpret_cast<bool(__thiscall*)(void*,void*,const CVector*)>(a)(self,object.data(),&point); }
                        else if(nm=="StartAttractorScriptBrainWithThisName"){ fillScripts(); setName(next()%4); buildPeds(false); At<uint32>(PedBuf(0),P_SBFLAGS)=pick({0,0x800000,0x1000000,0x1800000,0x2000000});
                            out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*,const char*,CPed*,bool)>(a)(self,nameBuf.data(),Ped(0),next()%2); }
                        else if(nm=="StartOrRequestNewStreamedScriptBrain"){ fillScripts(); setLoadStates(); buildPeds(false);
                            auto* e=PedBuf(0); for(size_t k=0;k<PED_STRIDE;k+=4)At<uint32>(e,k)=next(); const uint32 sb=next();
                            At<uint32>(e,P_SBFLAGS)=pick({0,0x800000,0x1000000,0x1800000,0x2000000,0xFFFFFFFFu,sb}); At<uint32>(e,0x140)=pick({0,0x100000,0x200000,0x300000,0xFFFFFFFFu,sb});
                            const int idx=int(next()%70); const int8 attach=int8(int(next()%9)-2); const bool add=next()%3!=0;
                            out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*,uint32,void*,int8,bool)>(a)(self,uint32(idx),e,attach,add); out["entity"]=Region(e,PED_STRIDE); }
                        out["scripts"]=Region(scripts.data(),scripts.size());
                    }
                    // ================= CPedGroup =================
                    else if(cls=="CPedGroup"){
                        void* self=groupBuf.data(); json refs=json::array();
                        auto cleanRefs=[&]{ for(int n=0;n<NPED;++n){ auto* e=reinterpret_cast<CEntity*>(Ped(n)); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); } };
                        if(nm=="Constructor"){ std::memset(groupBuf.data(),0xCD,groupBuf.size()); out["switches"]=Switch(); ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self))); }
                        else if(nm=="Destructor"){ buildPeds(false); fillGroup(false); out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="Flush"||nm=="Process"||nm=="RemoveAllFollowers"){ buildPeds(false); fillGroup(false); out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="FindDistanceToFurthestMember"){ buildPeds(false); fillGroup(true); out["switches"]=Switch(); ret=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(self)); }
                        else if(nm=="FindDistanceToNearestMember"){ buildPeds(false); fillGroup(true); CPed* outP=reinterpret_cast<CPed*>(0x12345678); const bool wantOut=next()%5!=0;
                            out["switches"]=Switch(); ret=F(reinterpret_cast<float(__thiscall*)(void*,CPed**)>(a)(self,wantOut ? &outP : nullptr)); out["out"]=Name(outP); }
                        else if(nm=="GetClosestGroupPed"){ buildPeds(false); fillGroup(true); float outF=-123.f; const bool wantOut=next()%5!=0; CPed* arg=next()%3==0 && slot(int(next()%8)) ? slot(int(next()%8)) : Ped(int(next()%NPED));
                            if(!arg)arg=Ped(0); out["switches"]=Switch(); ret=Name(uintptr_t(reinterpret_cast<CPed*(__thiscall*)(void*,CPed*,float*)>(a)(self,arg,wantOut ? &outF : nullptr))); out["dist"]=F(outF); }
                        else if(nm=="IsAnyoneUsingCar"){ buildPeds(false); fillGroup(false);
                            for(auto& t:tasks){ std::memset(t.data(),0,t.size()); At<void*>(t.data(),0xC)=next()%4==0 ? nullptr : vehs[next()%4].data(); }
                            for(int n=0;n<NPED;++n){ findCfg[n][0]=next()%3==0 ? int(next()%8) : -1; findCfg[n][1]=next()%3==0 ? int(next()%8) : -1; }
                            CVehicle* veh=next()%16==0 ? nullptr : reinterpret_cast<CVehicle*>(vehs[next()%4].data()); out["switches"]=Switch();
                            ret=reinterpret_cast<bool(__thiscall*)(void*,CVehicle*)>(a)(self,veh); }
                        else if(nm=="PlayerGaveCommand_Attack"){ buildPeds(false); fillGroup(false); cleanRefs(); addEventRet=next()%3!=0; std::memset(evArena.data(),0xCD,evArena.size());
                            CPed* pl=Ped(int(next()%NPED)); CPed* tg=next()%8==0 ? nullptr : Ped(int(next()%NPED)); out["switches"]=Switch();
                            reinterpret_cast<void(__thiscall*)(void*,CPed*,CPed*)>(a)(self,pl,tg);
                            for(int n=0;n<NPED;++n){ json l=json::array(); for(auto* r=reinterpret_cast<CEntity*>(Ped(n))->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); refs.push_back(l); } cleanRefs(); }
                        else if(nm=="PlayerGaveCommand_Gather"){ buildPeds(false); fillGroup(false); inCarMask=next(); destMask=next(); shrinkCfg=next()%3==0 ? 1+next()%3 : 0; CPed* pl=Ped(int(next()%NPED));
                            if(next()%2)At<void*>(PedBuf(0),P_VEH)=nullptr; out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*,CPed*)>(a)(self,pl); }
                        else if(nm=="Teleport"){ buildPeds(true); fillGroup(true);
                            // evento actual: sin evento, o con evento interno de tipo 77 (EVENT_LEADER_ENTRY_EXIT) u otro
                            std::memset(gEvent.data(),0,gEvent.size()); std::memset(gInner.data(),0,gInner.size()); At<void*>(gInner.data(),0)=evVtbl.data(); At<int32>(gInner.data(),0x18)=int32(pick({77,77,0,2,41,100}));
                            At<void*>(gEvent.data(),0x10)=gInner.data(); At<void*>(groupBuf.data(),0x34)=next()%3==0 ? nullptr : gEvent.data();
                            // tareas de respuesta de cada peaton (para AbortFirstPrimaryTaskIn)
                            for(auto& t:tasks){ std::memset(t.data(),0,t.size()); At<void*>(t.data(),0)=taskVtbl.data(); }
                            for(int n=0;n<NPED;++n)for(int k=0;k<3;++k)At<void*>(intels[n].data(),4+4*size_t(k))=next()%3==0 ? tasks[next()%8].data() : nullptr;
                            // sin seguidores que el EXE saque del vehiculo con CTaskSimpleCarSetPedOut (su ProcessPed real no se ejecuta sobre peatones falsos)
                            auto alive=[&](CPed* p){ const int32 s=At<int32>(p,P_STATE); return s!=0x36&&s!=0x37; };
                            CPed* leader=slot(7); if(leader&&!(At<uint32>(leader,P_FLAGS)&0x100u))
                                for(int k=0;k<7;++k)if(auto* f2=slot(k))if((At<uint32>(f2,P_FLAGS)&0x100u)&&At<uint8>(f2,P_CREATED)!=2&&alive(f2))At<uint32>(f2,P_FLAGS)&=~0x100u;
                            const CVector pos=randVec(-500.f,500.f); out["switches"]=Switch(); reinterpret_cast<void(__thiscall*)(void*,const CVector*)>(a)(self,&pos); }
                        out["group"]=Region(groupBuf.data(),groupBuf.size()); out["refs"]=refs;
                        json peds=json::array(); for(int n=0;n<NPED;++n)peds.push_back(Fnv(PedBuf(n),PED_STRIDE)); out["peds"]=peds; out["tasks"]=Fnv(tasks.data(),sizeof(tasks));
                    }
                    // ================= CWeather =================
                    else if(cls=="CWeather"){
                        weatherState(); setTimeGlobals(); setCamera(true); buildPlayer(); json extra;
                        CReplay::Mode=eReplayMode(next()%8==0 ? 1 : 0); CCutsceneMgr::ms_running=next()%10==0; CSpecialFX::bSnapShotActive=next()%6==0; CTimeCycle::m_FogReduction=next()%4==0 ? int32(next()%80) : int32(next()%65); CTimeCycle::m_CurrentStoredValue=next()%16;
                        for(auto& v:CTimeCycle::m_VectorToSun)v=randVec(-1.f,1.f);
                        CCullZones::CurrentFlags_Camera=next()%4!=0 ? int32(0x2000|(next()&0xFF)) : int32(next()&0xFFFF);
                        lookingLRB=next()%3==0; carHasRoof=next()%2; pvlHit=next()%4!=0; pvlZ=uni(-5.f,40.f); fakeVehResult=next()%2 ? vehs[0].data() : nullptr; playerCoors=randVec(-3000.f,3000.f);
                        if(next()%4==0)playerCoors=CVector{uni(-2000.f,2000.f),uni(-3000.f,3000.f),0.f};
                        g_fx.m_Splash=reinterpret_cast<FxSystem_c*>(fxSplash.data()); g_fx.m_Sand2=reinterpret_cast<FxSystem_c*>(fxSand.data());
                        im3dOk=next()%6!=0; fillWords(reinterpret_cast<void*>(TEMP_VERTS),0x24*80); fillWords(reinterpret_cast<void*>(TEMP_IDX),0x200); nv=int32(next()%2049); ni=int32(next()%4097);
                        auto rainArrays=[&](bool allocate){ if(allocate){ streakPosX=streakPosY=streakPosZ=nullptr; streakStr=nullptr; for(auto& b:newBufs)for(auto& x:b)x=0xAB; }
                            else { const CVector c=TheCamera.GetPosition(); for(int k=0;k<32;++k){ streakX[k]=int32(c.x)+int32(next()%21)-10; streakY[k]=int32(c.y)+int32(next()%21)-10; streakZ[k]=next()%6==0 ? int32(next()%7)-3 : int32(c.z)+int32(next()%13)-6; streakS[k]=next()%5==0 ? 0 : uint8(next()%120); }
                                streakPosX=streakX.data(); streakPosY=streakY.data(); streakPosZ=streakZ.data(); streakStr=streakS.data(); } };
                        auto weatherOut=[&]{ out["w1"]=Words(0xC812A8,0xB8); out["w2"]=Words(0xC813E0,0x80); out["w3"]=json::array({*reinterpret_cast<uint32*>(0x8D5EAC),*reinterpret_cast<uint32*>(0x8D6078)});
                            out["streaks"]=json::array({Name(uintptr_t(streakPosX)),Name(uintptr_t(streakPosY)),Name(uintptr_t(streakPosZ)),Name(uintptr_t(streakStr))});
                            out["streak_data"]=json::array({Fnv(streakX.data(),sizeof(streakX)),Fnv(streakY.data(),sizeof(streakY)),Fnv(streakZ.data(),sizeof(streakZ)),Fnv(streakS.data(),streakS.size()),Fnv(newBufs.data(),sizeof(newBufs))});
                            out["tempbuf"]=json::array({nv,ni,Fnv(reinterpret_cast<void*>(TEMP_IDX),0x200),Fnv(reinterpret_cast<void*>(TEMP_VERTS),0x24*80)}); };
                        if(nm=="Init"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="IsRainy"){ CWeather::Rain=next()%6==0 ? 0.2f : next()%12==0 ? NaN : uni(-0.1f,0.5f); out["switches"]=Switch(); ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                        else if(nm=="ReleaseWeather"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="ForceWeather"||nm=="ForceWeatherNow"){ const int32 t=next()%4==0 ? int32(next()%100)-50 : int32(next()%23); out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)(int32)>(a)(t); }
                        else if(nm=="FindWeatherTypesList"){ out["switches"]=Switch(); const auto* p=reinterpret_cast<const int8*(__cdecl*)()>(a)(); ret=Fnv(p,64); }
                        else if(nm=="ForecastWeather"){ const int32 t=next()%8==0 ? int32(next()%60)-30 : int32(next()%23); const int32 steps=next()%6==0 ? int32(next()%200)-20 : int32(next()%70);
                            out["switches"]=Switch(); ret=reinterpret_cast<bool(__cdecl*)(int32,int32)>(a)(t,steps); }
                        else if(nm=="SetWeatherToAppropriateTypeNow"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="UpdateInTunnelness"){ *reinterpret_cast<uint32*>(0xC8143C)=next()%4; At<CVector>(reinterpret_cast<void*>(0xC81430),0)=randVec(-3000.f,3000.f); At<CVector>(reinterpret_cast<void*>(0xC81424),0)=randVec(-3000.f,3000.f);
                            if(next()%2){ const CVector c{uni(-200.f,400.f),uni(-1400.f,-700.f),0.f}; if(camMatrix)At<CVector>(camMat.data(),0x30)=c; else camPos=c; }
                            out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="AddSandStormParticles"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="AddRain"){ if(next()%5)CWeather::Wind=uni(1.f,1.6f); CWeather::Rain=next()%5==0 ? 0.f : weightedUnit(); out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RenderRainStreaks"){ const bool alloc=next()%5==0; rainArrays(alloc); CWeather::Rain=next()%5==0 ? 0.f : uni(0.f,1.2f); CWeather::UnderWaterness=next()%8==0 ? 0.5f : 0.f; CGame::currArea=eAreaCodes(next()%12==0 ? 3 : 0);
                            if(next()%2)CWeather::CurrentRainParticleStrength=uint32(next()%90); CWeather::Wind=uni(0.f,1.6f); out["switches"]=Switch();
                            if(alloc){ EntryPatch na(0x821195,FakeNewArr); reinterpret_cast<void(__cdecl*)()>(a)(); json nl=json::array(); for(uint32 k=0;k<std::min<uint32>(newCount,8);++k)nl.push_back(newLog[k]); out["new_log"]=nl; out["new_count"]=newCount; }
                            else reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Update"){ rainArrays(false); heatMinute=next()%2 ? int32(CClock::ms_nGameClockMinutes) : int32(next()%60)-1; *reinterpret_cast<float*>(0xC81448)=weightedUnit(); *reinterpret_cast<float*>(0xC81444)=uni(0.f,1.5f); *reinterpret_cast<uint8*>(0xC81440)=next()%2;
                            *reinterpret_cast<uint16*>(0xB7C540)=uint16(pick({0,1,50,94,95,96,200,0xFFFF})); if(next()%2)CWeather::HeatHaze=1.f;
                            if(next()%4==0){ static const int16 hz[]{18,17,11,0}; CWeather::OldWeatherType=eWeatherType(hz[next()%4]); CWeather::NewWeatherType=eWeatherType(next()%2 ? hz[next()%4] : int16(weatherType())); }
                            if(next()%6==0)CWeather::ForcedWeatherType=eWeatherType(next()%23);
                            out["switches"]=Switch(); EntryPatch u1(Body(bit_cast<uintptr_t>(&CWeather::AddRain)),FakeAddRain), u1e(0x72A9A0,FakeAddRain), u2(Body(bit_cast<uintptr_t>(&CWeather::UpdateInTunnelness)),FakeUpdateTunnel), u2e(0x72B630,FakeUpdateTunnel);
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        weatherOut();
                    }
                    // ================= CRopes =================
                    else if(cls=="CRopes"){
                        auto fillRopes=[&]{
                            fillWords(ropes,NROPES*ROPE_SIZE); playerCrane=int32(next()%5); ropeCounter=next()%8==0 ? next() : next()%2 ? next()%8 : next()%300;
                            for(auto& e:ents){ std::memset(e.data(),0,e.size()); }
                            for(size_t n=0;n<NROPES;++n){ auto* r=ropes+n*ROPE_SIZE;
                                At<uint32>(r,0x300)=next()%3==0 ? uint32(100+next()%6) : next()%6; At<void*>(r,0x310)=next()%2 ? ents[next()%4].data() : nullptr; At<void*>(r,0x314)=next()%2 ? ents[next()%4].data() : nullptr;
                                At<void*>(r,0x318)=next()%2 ? ents[next()%4].data() : nullptr; At<uint32>(r,0x320)=next(); r[0x324]=uint8(next()%32); r[0x325]=uint8(next()%5==0 ? 0 : 1+next()%8); r[0x326]=uint8(next()); r[0x327]=uint8(next());
                                if(next()%4==0){ const auto it=next()%3; if(it==0)r[0x325]=0; } } };
                        auto ropeOut=[&]{ json h=json::array(); for(size_t n=0;n<NROPES;++n)h.push_back(Fnv(ropes+n*ROPE_SIZE,ROPE_SIZE)); out["ropes"]=h; out["crane"]=playerCrane; out["counter"]=ropeCounter; };
                        fillRopes();
                        const uint32 knownId=At<uint32>(ropes+size_t(next()%NROPES)*ROPE_SIZE,0x300);
                        if(nm=="Init"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Shutdown"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Update"){ CReplay::Mode=eReplayMode(next()%4==0 ? 1 : 0); out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Render"){ out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="FindRope"){ const uint32 id=next()%3==0 ? uint32(next()%200) : knownId; out["switches"]=Switch(); ret=reinterpret_cast<int32(__cdecl*)(uint32)>(a)(id); }
                        else if(nm=="IsCarriedByRope"){ void* e=next()%6==0 ? nullptr : next()%2 ? ents[next()%4].data() : At<void*>(ropes+size_t(next()%NROPES)*ROPE_SIZE,0x318); out["switches"]=Switch(); ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(e); }
                        else if(nm=="SetSpeedOfTopNode"){ const uint32 id=next()%4==0 ? uint32(next()%200) : knownId; const CVector v=randVec(-5.f,5.f); out["switches"]=Switch(); reinterpret_cast<void(__cdecl*)(uint32,CVector)>(a)(id,v); }
                        else if(nm=="FindPickupHeight"){ for(int k=0;k<MI_COUNT;++k){ std::memset(modelInfos[k].data(),0,0x40); std::memset(colModels[k].data(),0,0x40); At<void*>(modelInfos[k].data(),0x14)=colModels[k].data();
                                At<float>(colModels[k].data(),0x14)=uni(-5.f,50.f); modelPtrs[MI_BASE+k]=modelInfos[k].data(); }
                            const int k=int(next()%MI_COUNT); std::memset(ents[0].data(),0,0x100); At<uint16>(ents[0].data(),0x22)=uint16(MI_BASE+k); out["switches"]=Switch(); ret=F(reinterpret_cast<float(__cdecl*)(void*)>(a)(ents[0].data())); }
                        else if(nm=="FindCoorsAlongRope"){ CVector op{-1.f,-2.f,-3.f}, os{-4.f,-5.f,-6.f}; const uint32 id=next()%5==0 ? uint32(next()%200) : knownId;
                            static const float dists[]{0.f,1.f,0.999f,0.9990000128746033f,0.5f,-0.5f,2.f,0.9999f,0.001f,0.0322580645f};
                            const float d=next()%3==0 ? dists[next()%10] : uni(-0.3f,1.3f); const bool wantSpeed=next()%2; out["switches"]=Switch();
                            ret=reinterpret_cast<bool(__cdecl*)(uint32,float,CVector*,CVector*)>(a)(id,d,&op,wantSpeed ? &os : nullptr); out["pos"]=json::array({F(op.x),F(op.y),F(op.z)}); out["speed"]=json::array({F(os.x),F(os.y),F(os.z)}); }
                        else if(nm=="CreateRopeForSwatPed"){ const CVector p=randVec(-100.f,100.f); out["switches"]=Switch(); ret=reinterpret_cast<int32(__cdecl*)(const CVector*)>(a)(&p); }
                        else if(nm=="RegisterRope"){
                            const uint32 id=next()%3==0 ? uint32(next()%200) : next()%6==0 ? uint32(100+next()%6) : knownId; const uint32 type=next()%8==0 ? uint32(next()%12) : uint32(1+next()%8);
                            const CVector p=randVec(-500.f,500.f); const bool exp=next()%2; const uint8 segs=uint8(next()%32); const uint8 flags=uint8(next()%4==0 ? 0xFF : next()%2);
                            void* holder=next()%3==0 ? nullptr : ents[next()%4].data(); if(holder)At<uint8>(holder,0x36)=uint8(next()%2 ? 2 : next()%8|(next()%2 ? 0x38 : 0)); const uint32 tExp=next()%5==0 ? 0 : next()%20000;
                            if(next()%5==0)for(size_t n=0;n<NROPES;++n)ropes[n*ROPE_SIZE+0x325]=uint8(1+next()%8);                                  // sin huecos libres
                            if(next()%6==0)for(size_t n=0;n<NROPES;++n)ropes[n*ROPE_SIZE+0x325]=0;                                                    // todas libres
                            out["switches"]=Switch(); ret=reinterpret_cast<bool(__cdecl*)(uint32,uint32,CVector,bool,uint8,uint8,void*,uint32)>(a)(id,type,p,exp,segs,flags,holder,tExp);
                            out["holder"]=Fnv(ents.data(),sizeof(ents)); }
                        ropeOut();
                    }

                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls;
                    for(size_t k:sameClass)hooks[k]->State(false);
                    RestoreAll();
                    return out;
                };
                Write({{"event","case_start"},{"function",full},{"case",i}});
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
        fpSet=savedSet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
