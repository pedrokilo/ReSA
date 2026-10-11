#pragma once
// Banco dentro del motor: evitar peatones al deambular, trepar y agacharse (lista de 15, 45 funciones).
// CTaskComplexAvoidOtherPedWhileWandering (15), CTaskSimpleClimb (15) y CTaskSimpleDuck (15).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 4 peatones con vtable falsa (SetMoveAnim trazado), matriz, inteligencia y tareas falsas en el gestor de tareas
// (GetSubTask/GetTaskType/MakeAbortable trazados); 6 entidades (2 edificios, 2 objetos, 2 vehiculos) con matriz y
// modelo real con colision (solo se lee su radio); asociaciones de animacion y jerarquia falsas; mando falso.
// Mundo aislado: se guardan/vacian todas las cabezas 120x120 y repeat 16x16; los fixtures 59..61 se
// sustituyen por cadenas de nodos propias (se restauran al final de cada ejecucion) y CCollision::ProcessColModels se
// simula con un guion de puntos de contacto (punto, normal, superficie, pieza) por llamada.
// Se simulan y trazan: RNG, FindPlayerPed, CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation,
// CAnimBlendAssociation::SetFinishCallback/SetDeleteCallback, CPed::SetMoveState/Say/GetWeaponSkill,
// CPhysical::ApplyMoveForce (las dos sobrecargas)/GetSpeed, CPlaceable::SetOrientation, CWorld::ProcessVerticalLine,
// CEntity::GetIsOnScreen/GetBoundCentre/GetColModel, IKChainManager_c (IsLooking/AbortLookAt/GetLookAtEntity/LookAt),
// CTaskManager::GetTaskSecondary, CPedIntelligence::GetTaskSwim/FindTaskByType/GetTaskUseGun,
// CTaskSimpleUseGun::ClearAnim, CTaskSimpleGoToPoint::UpdatePoint, CPedGroups::AreInSameGroup,
// CPedGeometryAnalyser::ComputeRouteRoundSphere (0x5F1890), CTaskComplexGangFollower::CalculateOffsetPosition (0x65ED40),
// CEventGroup::Add, CStats::GetFatAndMuscleModifier, CCollision::ProcessColModels, SurfaceInfos_c::CanClimb,
// CPlayerPed::GetPadFromPlayer y CPad::GetExitVehicle.
// Se fijan y restauran: reloj, ms_fTimeStep, ms_nCurrentScanCode, CWorld::m_aTempColPts, la matriz estatica del EXE
// (0xC195E8 y su guarda), el estatico de ControlSubTask (0xC18D24/28), CTaskSimpleGoTo::ms_fLookAtThresholdDotProduct,
// Pads[0].DisablePlayerControls, las listas de sectores tocadas y los 4 modelos de colision de trepar (CreateColModel y
// Shutdown trabajan sobre copias: se crean, se serializan, se liberan y se restaura el original).
// Mascaras documentadas: los bits 1-7 del byte de flags y los campos de sombras (0x20..0x2F) de CCollisionData (AllocateData no los escribe), el ultimo byte de la superficie (m_nLight) de las esferas creadas (ninguno de los dos lo
// escribe) y la esfera local de ComputeDetourTarget (en el EXE sin inicializar) solo se traza sin ese byte.
// Fuera del banco: la rama FIX_BUGS del paso fino de CTaskSimpleClimb::ProcessPed (limite de velocidad 0.2): los casos
// usan un paso de tiempo >= 1.6, con el que el desplazamiento fino (< 0.317) nunca supera 0.2.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventGroup.h"
#include "Collision/Collision.h"
#include "Collision/ColModel.h"
#include "SurfaceInfos_c.h"
#include "Stats.h"
#include "Pad.h"
#include "World.h"
#include "Sector.h"
#include "RepeatSector.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../ProbeRuntime.hpp"

namespace CppTrigClimbLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_HEADING=0x10, E_MATRIX=0x14, E_CLUMP=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_SCAN=0x2C, E_TYPE=0x36; // CEntity
constexpr size_t PH_FLAGS=0x40, PH_SPEED=0x44, PH_MASS=0x8C, PH_IGNORED=0x128;                                         // CPhysical
constexpr size_t P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PDATA=0x480,
                 P_ANIMGROUP=0x4D4, P_MOVESTATE=0x534, P_HEALTH=0x540, P_CURROT=0x558, P_AIMROT=0x55C,
                 P_WEAPONS=0x5A0, P_PEDTYPE=0x598, P_SLOT=0x718;                                                       // CPed
constexpr size_t V_SUBTYPE=0x594;                                                                                     // CVehicle
constexpr size_t I_TASKMGR=4, I_STATIC=0x274;                                                                         // CPedIntelligence
constexpr size_t A_HIER=0x14, A_AMOUNT=0x18, A_DELTA=0x1C, A_TIME=0x20, A_SPEED=0x24, A_STEP=0x28, A_ID=0x2C, A_FLAGS=0x2E; // asociacion
constexpr uintptr_t ADDR_TIME=0xB7CB84, ADDR_STEP=0xB7CB5C, ADDR_SCAN=0xB7CD78, ADDR_TEMPCOL=0xB9ACD0, ADDR_LOOK_THR=0xC18D48,
                    ADDR_IKMAN=0xC15448, ADDR_STATMAT=0xC195E8, ADDR_STATMAT_GUARD=0xC19630, ADDR_AVOID_STATIC=0xC18D24,
                    ADDR_CLIMBCM=0xC19518, EXE_DEFAULT_CB=0x4D1390, EXE_DUCK_CB=0x692550, EXE_CLIMB_CB=0x67A380;
constexpr uintptr_t VT_AVOID=0x86FEC4, VT_CLIMB=0x87059C, VT_DUCK=0x870BC4;
constexpr int NPEDS=4, NENTS=6, NFAKES=6, NCELL=3, SECTOR0=59, NSCRIPT=8;
constexpr size_t FK_SIZE=0x40, FK_TYPE=0x30, FK_SUB=0x34, FK_ABORT=0x38, FK_CNT=0x3C, TEMPCOL_SIZE=32*sizeof(CColPoint);

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,0x6A0>,NENTS> ents{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NENTS> entMats{};
alignas(16) inline std::array<std::array<uint8,FK_SIZE>,NFAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,3> anims{};
alignas(16) inline std::array<uint8,0x40> hier{}, clump{}, eventBuf{};
alignas(16) inline std::array<uint8,0x200> playerData{};
alignas(16) inline std::array<uint8,sizeof(CPad)> pad{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,sizeof(CColModel)> fakeCm{};
alignas(16) inline std::array<uint8,sizeof(CCollisionData)> fakeCd{};
alignas(16) inline std::array<uint8,0x40> vec{};                       // vectores, angulos y superficies de salida
alignas(16) inline std::array<uint8,0x80> pedArrays{};                 // dos arrays de 16 peatones
alignas(16) inline std::array<uint8,0x400> nodes{};                    // nodos de las listas falsas
alignas(16) inline std::array<uint8,0x10> listHead{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CEntity* Ent(int n) { return reinterpret_cast<CEntity*>(ents[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CAnimBlendAssociation* Anim(int n) { return reinterpret_cast<CAnimBlendAssociation*>(anims[n].data()); }
inline std::array<void*,64> pedVtbl{}, entVtbl{};
inline std::array<void*,16> fakeVtbl{}, eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_DUCK_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleDuck::DeleteDuckAnimCB))return "DeleteDuckAnimCB";
    if(v==EXE_CLIMB_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleClimb::DeleteAnimCB))return "ClimbDeleteAnimCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(v==VT_AVOID)return "vt_avoid"; if(v==VT_CLIMB)return "vt_climb"; if(v==VT_DUCK)return "vt_duck";
    for(int k=0;k<4;++k)if(v==ADDR_CLIMBCM+0x30*uintptr_t(k))return "climb_cm"+std::to_string(k);
    if(v==ADDR_TEMPCOL)return "temp_col_pts";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+0x80)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const std::string& nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return nm+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ const auto s=std::to_string(n);
        if(auto r=in(peds[n].data(),peds[n].size(),"ped"+s))return *r; if(auto r=in(pedMats[n].data(),pedMats[n].size(),"pmat"+s))return *r;
        if(auto r=in(intels[n].data(),intels[n].size(),"intel"+s))return *r; }
    for(int n=0;n<NENTS;++n){ const auto s=std::to_string(n);
        if(auto r=in(ents[n].data(),ents[n].size(),"ent"+s))return *r; if(auto r=in(entMats[n].data(),entMats[n].size(),"emat"+s))return *r; }
    for(int n=0;n<NFAKES;++n)if(auto r=in(fakes[n].data(),fakes[n].size(),"fk"+std::to_string(n)))return *r;
    for(int n=0;n<3;++n)if(auto r=in(anims[n].data(),anims[n].size(),"anim"+std::to_string(n)))return *r;
    if(auto r=in(hier.data(),hier.size(),"hier"))return *r; if(auto r=in(clump.data(),clump.size(),"clump"))return *r;
    if(auto r=in(eventBuf.data(),eventBuf.size(),"event"))return *r; if(auto r=in(playerData.data(),playerData.size(),"pdata"))return *r;
    if(auto r=in(pad.data(),pad.size(),"pad"))return *r; if(auto r=in(object.data(),object.size(),"obj"))return *r;
    if(auto r=in(fakeCm.data(),fakeCm.size(),"fakecm"))return *r; if(auto r=in(fakeCd.data(),fakeCd.size(),"fakecd"))return *r;
    if(auto r=in(vec.data(),vec.size(),"vec"))return *r; if(auto r=in(pedArrays.data(),pedArrays.size(),"parr"))return *r;
    if(auto r=in(nodes.data(),nodes.size(),"node"))return *r; if(auto r=in(listHead.data(),listHead.size(),"list"))return *r;
    if(auto r=in(pedVtbl.data(),sizeof(pedVtbl),"pedvt"))return *r; if(auto r=in(entVtbl.data(),sizeof(entVtbl),"entvt"))return *r;
    if(auto r=in(fakeVtbl.data(),sizeof(fakeVtbl),"fakevt"))return *r; if(auto r=in(eventVtbl.data(),sizeof(eventVtbl),"eventvt"))return *r;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }
inline json Sphere(const void* sp) { return {F(At<float>(sp,0)),F(At<float>(sp,4)),F(At<float>(sp,8)),F(At<float>(sp,0xC)),At<uint8>(sp,0x10),At<uint8>(sp,0x11),At<uint8>(sp,0x12)}; } // sin m_nLight
inline json Mat(const void* m) { json r=json::array(); for(size_t o:{0u,4u,8u,0x10u,0x14u,0x18u,0x20u,0x24u,0x28u,0x30u,0x34u,0x38u})r.push_back(F(At<float>(m,o))); return r; }

// buffers comparados palabra a palabra (traducidas) antes/despues
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ const auto s=std::to_string(n); b.push_back({"ped"+s,peds[n].data(),peds[n].size()}); b.push_back({"pmat"+s,pedMats[n].data(),pedMats[n].size()}); b.push_back({"intel"+s,intels[n].data(),intels[n].size()}); }
    for(int n=0;n<NENTS;++n){ const auto s=std::to_string(n); b.push_back({"ent"+s,ents[n].data(),ents[n].size()}); b.push_back({"emat"+s,entMats[n].data(),entMats[n].size()}); }
    for(int n=0;n<NFAKES;++n)b.push_back({"fk"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    for(int n=0;n<3;++n)b.push_back({"anim"+std::to_string(n),anims[n].data(),anims[n].size()});
    b.push_back({"hier",hier.data(),hier.size()}); b.push_back({"event",eventBuf.data(),eventBuf.size()}); b.push_back({"pdata",playerData.data(),playerData.size()});
    b.push_back({"pad",pad.data(),pad.size()}); b.push_back({"fakecm",fakeCm.data(),fakeCm.size()}); b.push_back({"fakecd",fakeCd.data(),fakeCd.size()});
    b.push_back({"vec",vec.data(),vec.size()}); b.push_back({"parr",pedArrays.data(),pedArrays.size()}); b.push_back({"node",nodes.data(),nodes.size()});
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
inline uint32 seed{}, rngCalls{}, blendCalls{}, colCalls{};
inline uint32 boolMask{}, boolCnt{};
inline bool B() { return (boolMask>>(boolCnt++&31))&1; }
inline uint32 assocMask{}, surfMask{}, sameGroupMask{};
inline uintptr_t playerRet{}, lookEntRet{}, swimRet{}, findTaskRet{}, useGunRet{};
inline std::array<std::array<uintptr_t,6>,NPEDS> secondaryRet{};
inline bool onScreenRet{}, lookingRet{}, vlineRet{}, routeRet{}, exitRet{};
inline float colZ{}, fatRet{}; inline int32 skillRet{};
inline CVector routeA{}, routeB{}, offsetRet{}, speedAdd{};
inline std::array<CVector,NENTS> boundCentres{};
struct ColScript { int32 count; CVector point, normal; uint8 surfB, pieceA; };
inline std::array<ColScript,NSCRIPT> colScript{};

template<int N> void* __fastcall PTrap(void* s,void*) { trace.push_back({"ped_unexpected_slot",N,Name(s)}); return nullptr; }
template<int N> void* __fastcall ETrap(void* s,void*) { trace.push_back({"ent_unexpected_slot",N,Name(s)}); return nullptr; }
template<int N> void* __fastcall FTrap(void* s,void*) { trace.push_back({"fake_unexpected_slot",N,Name(s)}); return nullptr; }
template<int N> void* __fastcall VTrap(void* s,void*) { trace.push_back({"event_unexpected_slot",N,Name(s)}); return nullptr; }
template<size_t S,class T,T... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<T,N...>,int kind) {
    ((vt[N]=kind==0 ? reinterpret_cast<void*>(&PTrap<N>) : kind==1 ? reinterpret_cast<void*>(&ETrap<N>) : kind==2 ? reinterpret_cast<void*>(&FTrap<N>) : reinterpret_cast<void*>(&VTrap<N>)),...);
}
inline void __fastcall PSetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(p)}); }
// tareas falsas: GetSubTask, GetTaskType y MakeAbortable trazados (campos de control en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FSub(CTask* t,void*) { trace.push_back({"get_sub_task",Name(t)}); return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FIsSimple(CTask* t,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"get_task_type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
// evento falso: tipo (1), prioridad (2) y entidad origen (10)
inline int eventType{}, eventPrio{}; inline uintptr_t eventSource{};
inline int __fastcall EvType(void* e,void*) { trace.push_back({"event_get_type",Name(e)}); return eventType; }
inline int __fastcall EvPrio(void* e,void*) { trace.push_back({"event_get_priority",Name(e)}); return eventPrio; }
inline CEntity* __fastcall EvSource(void* e,void*) { trace.push_back({"event_get_source",Name(e)}); return reinterpret_cast<CEntity*>(eventSource); }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FFindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline CAnimBlendAssociation* __cdecl FBlend(RpClump* c,int32 group,int32 id,float delta) {
    trace.push_back({"blend_animation",Name(c),group,id,F(delta)}); return Anim(1+int(blendCalls++%2)); // nunca nula: el EXE la desreferencia
}
inline CAnimBlendAssociation* __cdecl FGetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(c),id}); return (assocMask>>(id%32))&1 ? Anim(0) : nullptr; }
inline void __fastcall FSetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(a),Name(cb),Name(data)}); }
inline void __fastcall FSetDeleteCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_delete_callback",Name(a),Name(cb),Name(data)}); }
inline void __fastcall FSetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(p),s}); }
inline int16 __fastcall FSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 c) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),a&0xFF,b&0xFF,c&0xFF}); return -1;
}
inline int __fastcall FWeaponSkill(CPed* p,void*) { trace.push_back({"get_weapon_skill",Name(p)}); return skillRet; }
inline void __fastcall FMoveForce(CPhysical* p,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(p),F(x),F(y),F(z)}); }
inline CVector* __fastcall FGetSpeed(CPhysical* p,void*,CVector* out,float x,float y,float z) {
    trace.push_back({"get_speed",Name(p),F(x),F(y),F(z)}); *out=CVector{x*0.01f+speedAdd.x,y*0.01f+speedAdd.y,z*0.01f+speedAdd.z}; return out;
}
inline void __fastcall FSetOrientation(CPlaceable* p,void*,float x,float y,float z) { trace.push_back({"set_orientation",Name(p),F(x),F(y),F(z)}); }
inline bool __cdecl FVLine(const CVector& o,float dist,CColPoint& cp,CEntity*& e,bool b,bool v,bool pd,bool ob,bool du,bool see,CStoredCollPoly* poly) {
    trace.push_back({"process_vertical_line",Vec(o),F(dist),Name(&cp),b,v,pd,ob,du,see,poly!=nullptr});
    if(vlineRet){ cp.m_vecPoint=CVector{o.x,o.y,colZ}; e=Ent(0); }
    return vlineRet;
}
inline bool __fastcall FOnScreen(CEntity* e,void*) { trace.push_back({"get_is_on_screen",Name(e)}); return onScreenRet; }
inline CVector* __fastcall FBoundCentre(CEntity* e,void*,CVector* out) {
    trace.push_back({"get_bound_centre",Name(e)});
    int idx=0; for(int n=0;n<NENTS;++n)if(e==Ent(n))idx=n; *out=boundCentres[idx]; return out;
}
inline CColModel* __fastcall FGetColModel(CEntity* e,void*) { trace.push_back({"get_col_model",Name(e)}); return reinterpret_cast<CColModel*>(fakeCm.data()); }
inline bool __fastcall FIsLooking(void* m,void*,CPed* p) { trace.push_back({"ik_is_looking",Name(m),Name(p)}); return lookingRet; }
inline void __fastcall FAbortLook(void* m,void*,CPed* p,uint32 t) { trace.push_back({"ik_abort_look_at",Name(m),Name(p),t}); }
inline CEntity* __fastcall FLookEnt(void* m,void*,CPed* p) { trace.push_back({"ik_get_look_at_entity",Name(m),Name(p)}); return reinterpret_cast<CEntity*>(lookEntRet); }
inline void __fastcall FLookAt(void* m,void*,const char* why,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&why&&why[k];++k)s+=why[k];
    trace.push_back({"ik_look_at",Name(m),s,Name(p),Name(e),time,bone,off ? Vec(*off) : json(nullptr),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline CTask* __fastcall FSecondary(void* tm,void*,int32 idx) {
    int n=0; for(int k=0;k<NPEDS;++k)if(tm==intels[k].data()+I_TASKMGR)n=k;
    trace.push_back({"get_task_secondary",Name(tm),idx}); return idx>=0&&idx<6 ? reinterpret_cast<CTask*>(secondaryRet[n][idx]) : nullptr;
}
inline void* __fastcall FTaskSwim(void* i,void*) { trace.push_back({"get_task_swim",Name(i)}); return reinterpret_cast<void*>(swimRet); }
inline CTask* __fastcall FFindTask(void* i,void*,int32 type) { trace.push_back({"find_task_by_type",Name(i),type}); return reinterpret_cast<CTask*>(findTaskRet); }
inline void* __fastcall FUseGun(void* i,void*) { trace.push_back({"get_task_use_gun",Name(i)}); return reinterpret_cast<void*>(useGunRet); }
inline void __fastcall FClearAnim(void* t,void*,CPed* p) { trace.push_back({"use_gun_clear_anim",Name(t),Name(p)}); }
inline void __fastcall FUpdatePoint(void* t,void*,const CVector* p,float r,uint32 b) { trace.push_back({"update_point",Name(t),Vec(*p),F(r),b&0xFF}); }
inline bool __cdecl FSameGroup(const CPed* a,const CPed* b) { const bool r=B(); trace.push_back({"same_group",Name(a),Name(b),r}); return r; }
inline bool __cdecl FRoute(const CPed& p,const CColSphere& sp,const CVector& a,const CVector& b,CVector& o1,CVector& o2) {
    trace.push_back({"route_round_sphere",Name(&p),Sphere(&sp),Vec(a),Vec(b),Name(&o1),Name(&o2),routeRet});
    if(routeRet){ o1=routeA; o2=routeB; } return routeRet;
}
inline CVector* __fastcall FOffsetPos(void* t,void*,CVector* out) { trace.push_back({"gang_offset_position",Name(t)}); *out=offsetRet; return out; }
inline CEvent* __fastcall FEventAdd(void* g,void*,CEvent* e,uint32 valid) {
    trace.push_back({"event_group_add",int(e->GetEventType()),Name(At<uint32>(e,0x14)),F(At<float>(e,0x18)),At<uint32>(e,0x1C),Vec(At<CVector>(e,0x20)),valid&0xFF}); return e;
}
inline float __cdecl FFat(int32 mod) { trace.push_back({"fat_and_muscle_modifier",mod}); return fatRet; }
inline int32 __cdecl FProcessColModels(const CMatrix& ma,CColModel& ca,const CMatrix& mb,CColModel& cb,CColPoint* pts,CColPoint* lines,float* depths,bool arg7) {
    const auto& s=colScript[colCalls++%NSCRIPT];
    trace.push_back({"process_col_models",Mat(&ma),Name(&ca),Name(&mb),Mat(&mb),Name(&cb),Name(pts),Name(lines),Name(depths),arg7,s.count});
    if(s.count>0){ auto& cp=pts[0]; cp.m_vecPoint=s.point; cp.m_vecNormal=s.normal; cp.m_nSurfaceTypeB=eSurfaceType(s.surfB); cp.m_nPieceTypeA=s.pieceA; }
    return s.count;
}
inline bool __fastcall FCanClimb(void* si,void*,uint32 id) { const bool r=(surfMask>>((id&0xFF)%32))&1; trace.push_back({"can_climb",id&0xFF,r}); return r; }
inline CPad* __fastcall FPadFromPlayer(void* p,void*) { trace.push_back({"pad_from_player",Name(p)}); return reinterpret_cast<CPad*>(pad.data()); }
inline bool __fastcall FExitVehicle(void* p,void*) { trace.push_back({"pad_exit_vehicle",Name(p),exitRet}); return exitRet; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexAvoidOtherPedWhileWandering","Clone",0,0x86FEC4,1,256},
    {"CTaskComplexAvoidOtherPedWhileWandering","ComputeAvoidSphere",0x672080,0,-1,512},
    {"CTaskComplexAvoidOtherPedWhileWandering","ComputeDetourTarget",0x672180,0,-1,256},
    {"CTaskComplexAvoidOtherPedWhileWandering","ComputeRouteRoundSphere",0x66A7B0,0,-1,128},
    {"CTaskComplexAvoidOtherPedWhileWandering","ComputeSphere",0x66A320,0,-1,512},
    {"CTaskComplexAvoidOtherPedWhileWandering","Constructor",0x66A100,0,-1,128},
    {"CTaskComplexAvoidOtherPedWhileWandering","ControlSubTask",0,0x86FEC4,10,1024},
    {"CTaskComplexAvoidOtherPedWhileWandering","CreateFirstSubTask",0,0x86FEC4,9,256},
    {"CTaskComplexAvoidOtherPedWhileWandering","CreateNextSubTask",0,0x86FEC4,8,256},
    {"CTaskComplexAvoidOtherPedWhileWandering","Destructor",0x66A1D0,0,-1,128},
    {"CTaskComplexAvoidOtherPedWhileWandering","GetTaskType",0,0x86FEC4,4,64},
    {"CTaskComplexAvoidOtherPedWhileWandering","MakeAbortable",0,0x86FEC4,6,256},
    {"CTaskComplexAvoidOtherPedWhileWandering","NearbyPedsInSphere",0x671FE0,0,-1,512},
    {"CTaskComplexAvoidOtherPedWhileWandering","QuitIK",0x66A230,0,-1,128},
    {"CTaskComplexAvoidOtherPedWhileWandering","SetUpIK",0x66A850,0,-1,1024},
    {"CTaskSimpleClimb","Constructor",0x67A110,0,-1,128},
    {"CTaskSimpleClimb","CreateColModel",0x67A890,0,-1,64},
    {"CTaskSimpleClimb","DeleteAnimCB",0x67A380,0,-1,64},
    {"CTaskSimpleClimb","GetCameraStickModifier",0x67A5D0,0,-1,512},
    {"CTaskSimpleClimb","GetCameraTargetPos",0x67A390,0,-1,512},
    {"CTaskSimpleClimb","MakeAbortable",0,0x87059C,6,256},
    {"CTaskSimpleClimb","ProcessPed",0,0x87059C,7,2048},
    {"CTaskSimpleClimb","ScanToGrab",0x67FD30,0,-1,1024},
    {"CTaskSimpleClimb","ScanToGrabSectorList<CPtrListSingleLink<CPhysical*>>",0x67DE10,0,-1,1024},
    {"CTaskSimpleClimb","Shutdown",0x67A250,0,-1,64},
    {"CTaskSimpleClimb","StartAnim",0x67DBE0,0,-1,512},
    {"CTaskSimpleClimb","StartSpeech",0x67A320,0,-1,128},
    {"CTaskSimpleClimb","TestForClimb",0x6803A0,0,-1,1024},
    {"CTaskSimpleClimb","TestForStandUp",0x680570,0,-1,256},
    {"CTaskSimpleClimb","TestForVault",0x6804D0,0,-1,256},
    {"CTaskSimpleDuck","CanPedDuck",0x692610,0,-1,256},
    {"CTaskSimpleDuck","Clone",0,0x870BC4,1,128},
    {"CTaskSimpleDuck","Constructor",0x691FC0,0,-1,128},
    {"CTaskSimpleDuck","ControlDuckMove",0x6923F0,0,-1,512},
    {"CTaskSimpleDuck","DeleteDuckAnimCB",0x692550,0,-1,256},
    {"CTaskSimpleDuck","Destructor",0x692030,0,-1,256},
    {"CTaskSimpleDuck","ForceStopMove",0x6924B0,0,-1,64},
    {"CTaskSimpleDuck","GetTaskType",0,0x870BC4,4,64},
    {"CTaskSimpleDuck","IsTaskInUseByOtherTasks",0x61C3D0,0,-1,256},
    {"CTaskSimpleDuck","MakeAbortable",0,0x870BC4,6,1024},
    {"CTaskSimpleDuck","ProcessPed",0,0x870BC4,7,2048},
    {"CTaskSimpleDuck","RestartTask",0x692390,0,-1,128},
    {"CTaskSimpleDuck","SetControlType",0x6924F0,0,-1,128},
    {"CTaskSimpleDuck","SetDuckTimer",0x692530,0,-1,128},
    {"CTaskSimpleDuck","SetMoveAnim",0x6939F0,0,-1,1024},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& e:ents)clean(e.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }
inline json AllRefs() {
    json r=json::array(); auto list=[](void* p){ json l=json::array(); for(auto* x=static_cast<CEntity*>(p)->m_pReferences;x;x=x->m_pNext)l.push_back(Name(uintptr_t(x->m_ppEntity))); return l; };
    for(auto& p:peds)r.push_back(list(p.data())); for(auto& e:ents)r.push_back(list(e.data())); return r;
}
// modelos de colision de trepar: serializacion sin direcciones (punteros relativos a su CCollisionData)
inline json SerColModel(const void* cmp) {
    json r=json::array(); for(size_t o=0;o<0x2C;o+=4)r.push_back(At<uint32>(cmp,o));
    const auto* cd=At<const uint8*>(cmp,0x2C); if(!cd){ r.push_back(nullptr); return r; }
    json d={At<uint16>(cd,0),At<uint16>(cd,2),At<uint16>(cd,4),At<uint8>(cd,6),At<uint8>(cd,7)&1}; // AllocateData (0x40F870) solo escribe el bit 0 del byte de flags: el resto queda con lo que traiga CMemoryMgr::Malloc
    // Como el original: AllocateData escribe los punteros 0x8..0x1C; los campos de sombras (0x20..0x2F) quedan con lo que traiga Malloc
    for(size_t o=8;o<0x20;o+=4){ const auto p=At<uintptr_t>(cd,o); d.push_back(p==0 ? json(nullptr) : json(int64(p)-int64(uintptr_t(cd)))); }
    const auto* sp=At<const uint8*>(cd,8); json s=json::array(); if(sp)for(uint16 k=0;k<At<uint16>(cd,0);++k)s.push_back(Sphere(sp+k*0x14));
    r.push_back(d); r.push_back(s); return r;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpptrigclimb-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpptrigclimb-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpptrigclimb");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"cpptrigclimb-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // modelo con colision cargada (solo se lee su radio)
    int16 colModel=-1; for(int32 m=400;m<612&&colModel<0;++m)if(auto* mi=CModelInfo::GetModelInfo(m);mi&&mi->GetColModel())colModel=int16(m);
    if(colModel<0){Write({{"event","error"},{"reason","no model with collision"}});return;}
    // los modelos de trepar los crea el propio juego la primera vez (ScanToGrab): aqui deben existir ya
    auto* climbCms=reinterpret_cast<uint8*>(ADDR_CLIMBCM);
    if(!At<void*>(climbCms,0x2C)){ Write({{"event","note"},{"reason","climb col models were not created yet: created once with the game's own CreateColModel (0x67A890)"}});
        reinterpret_cast<bool(__cdecl*)()>(0x67A890)(); }
    FillTraps(pedVtbl,std::make_integer_sequence<int,64>{},0); FillTraps(entVtbl,std::make_integer_sequence<int,64>{},1);
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{},2); FillTraps(eventVtbl,std::make_integer_sequence<int,16>{},3);
    pedVtbl[23]=reinterpret_cast<void*>(&PSetMoveAnim);
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FIsSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&EvType); eventVtbl[2]=reinterpret_cast<void*>(&EvPrio); eventVtbl[10]=reinterpret_cast<void*>(&EvSource);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: regiones guardadas y restauradas en cada ejecucion y al final
    std::vector<std::pair<uintptr_t,size_t>> regions{{ADDR_TIME,4},{ADDR_STEP,4},{ADDR_SCAN,2},{ADDR_TEMPCOL,TEMPCOL_SIZE},{ADDR_LOOK_THR,4},
        {ADDR_STATMAT,0x48},{ADDR_STATMAT_GUARD,4},{ADDR_AVOID_STATIC,8},{ADDR_CLIMBCM,0xC0},
        {uintptr_t(&CPad::GetPad(0)->DisablePlayerControls),2},{uintptr_t(&CWorld::ms_aRepeatSectors[0][0]),sizeof(CRepeatSector)*16*16}};
    // Preserve ALL regular heads; the traversal may reach beyond the fixture's nine cells.
    regions.push_back({uintptr_t(&CWorld::ms_aSectors[0][0]),sizeof(CWorld::ms_aSectors)});
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    // Scope restoration performs only copies of already saved globals.
    // It must outlive every isolated side, including exception unwinding.
    struct GlobalsRestoreGuard {
        const std::vector<std::pair<uintptr_t,size_t>>& regions;
        const std::vector<std::vector<uint8>>& saved;
        ~GlobalsRestoreGuard() noexcept {
            for(size_t k=0;k<regions.size();++k) {
                std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second);
            }
        }
    };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool registered=true; for(size_t f=0;f<fns.size();++f)if((std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleClimb::ProcessPed")||(std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleClimb::TestForClimb")||(std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleClimb::TestForStandUp")||(std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleClimb::TestForVault")||(std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleClimb::GetCameraStickModifier")||(std::string(fns[f].cls)+"::"+fns[f].name=="CTaskSimpleClimb::GetCameraTargetPos"))registered&=enabled[f];
    Write({{"event","begin"},{"registered_hook",registered},{"functions",6},{"pairs",4608},{"collaborators_stubbed",true},{"world_isolation",{{"regular_sectors",120*120},{"repeat_sectors",16*16},{"fixture_sectors",NCELL*NCELL},{"heads_only",true}}}});
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FFindPlayer), p1e(0x56E210,FFindPlayer);
        EntryPatch a1(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FBlend), a1e(0x4D4610,FBlend);
        EntryPatch a2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FGetAssoc), a2e(0x4D68B0,FGetAssoc);
        EntryPatch a3(Body(MemberAddress(&CAnimBlendAssociation::SetFinishCallback)),FSetFinishCb), a3e(0x4CEBE0,FSetFinishCb);
        EntryPatch a4(Body(MemberAddress(&CAnimBlendAssociation::SetDeleteCallback)),FSetDeleteCb), a4e(0x4CEBC0,FSetDeleteCb);
        EntryPatch s1(Body(MemberAddress(&CPed::SetMoveState)),FSetMoveState), s1e(0x5DEC00,FSetMoveState);
        EntryPatch s2(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FSay), s2e(0x5EFFE0,FSay);
        EntryPatch s3(Body(MemberAddress(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill))),FWeaponSkill), s3e(0x5E6580,FWeaponSkill);
        EntryPatch s4(Body(MemberAddress(static_cast<void(CPhysical::*)(float,float,float)>(&CPhysical::ApplyMoveForce))),FMoveForce), s4e(0x4ABBA0,FMoveForce);
        EntryPatch s5(Body(MemberAddress(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce))),FMoveForce), s5e(0x5429F0,FMoveForce);
        EntryPatch s6(Body(MemberAddress(&CPhysical::GetSpeed)),FGetSpeed), s6e(0x542CE0,FGetSpeed);
        EntryPatch s7(Body(MemberAddress(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation))),FSetOrientation), s7e(0x439A80,FSetOrientation);
        EntryPatch w1(Body(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine)),FVLine), w1e(0x5674E0,FVLine);
        EntryPatch e1(Body(MemberAddress(&CEntity::GetIsOnScreen)),FOnScreen), e1e(0x534540,FOnScreen);
        EntryPatch e2(Body(MemberAddress(static_cast<CVector(CEntity::*)() const>(&CEntity::GetBoundCentre))),FBoundCentre), e2e(0x534250,FBoundCentre);
        EntryPatch e3(Body(MemberAddress(&CEntity::GetColModel)),FGetColModel), e3e(0x535300,FGetColModel);
        EntryPatch k1(Body(MemberAddress(&IKChainManager_c::IsLooking)),FIsLooking), k1e(0x6181A0,FIsLooking);
        EntryPatch k2(Body(MemberAddress(&IKChainManager_c::AbortLookAt)),FAbortLook), k2e(0x618280,FAbortLook);
        EntryPatch k3(Body(MemberAddress(&IKChainManager_c::GetLookAtEntity)),FLookEnt), k3e(0x6181D0,FLookEnt);
        EntryPatch k4(Body(MemberAddress(&IKChainManager_c::LookAt)),FLookAt), k4e(0x618970,FLookAt);
        EntryPatch t1(Body(MemberAddress(static_cast<CTask*(CTaskManager::*)(eSecondaryTask)>(&CTaskManager::GetTaskSecondary))),FSecondary), t1e(0x681810,FSecondary);
        EntryPatch t2(Body(MemberAddress(&CPedIntelligence::GetTaskSwim)),FTaskSwim), t2e(0x601070,FTaskSwim);
        EntryPatch t3(Body(MemberAddress(&CPedIntelligence::FindTaskByType)),FFindTask), t3e(0x600EE0,FFindTask);
        EntryPatch t4(Body(MemberAddress(&CPedIntelligence::GetTaskUseGun)),FUseGun), t4e(0x600F70,FUseGun);
        EntryPatch t5(Body(MemberAddress(&CTaskSimpleUseGun::ClearAnim)),FClearAnim), t5e(0x61E190,FClearAnim);
        EntryPatch t6(Body(MemberAddress(&CTaskSimpleGoToPoint::UpdatePoint)),FUpdatePoint), t6e(0x645700,FUpdatePoint);
        EntryPatch g1(Body(bit_cast<uintptr_t>(&CPedGroups::AreInSameGroup)),FSameGroup), g1e(0x5F7F40,FSameGroup);
        EntryPatch g2(0x5F1890,FRoute);       // el simbolo del DLL llama a esta direccion del EXE
        EntryPatch g3(0x65ED40,FOffsetPos);   // idem (CalculateOffsetPosition)
        EntryPatch g4(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FEventAdd), g4e(0x4AB420,FEventAdd);
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier)),FFat), c1e(0x559AF0,FFat);
        EntryPatch c2(Body(bit_cast<uintptr_t>(&CCollision::ProcessColModels)),FProcessColModels), c2e(0x4185C0,FProcessColModels);
        EntryPatch c3(Body(MemberAddress(&SurfaceInfos_c::CanClimb)),FCanClimb), c3e(0x55EA10,FCanClimb);
        EntryPatch c4(Body(MemberAddress(&CPlayerPed::GetPadFromPlayer)),FPadFromPlayer), c4e(0x609560,FPadFromPlayer);
        EntryPatch c5(Body(MemberAddress(&CPad::GetExitVehicle)),FExitVehicle), c5e(0x5400D0,FExitVehicle);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(full!="CTaskSimpleClimb::ProcessPed"&&full!="CTaskSimpleClimb::TestForClimb"&&full!="CTaskSimpleClimb::TestForStandUp"&&full!="CTaskSimpleClimb::TestForVault"&&full!="CTaskSimpleClimb::GetCameraStickModifier"&&full!="CTaskSimpleClimb::GetCameraTargetPos")continue;
            if(!only.empty()&&only!=full)continue;
            const bool avoid=cls=="CTaskComplexAvoidOtherPedWhileWandering", climb=cls=="CTaskSimpleClimb", duck=cls=="CTaskSimpleDuck";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    // pool de tareas: m_LastFreeSlot restaurado y huecos libres con 0xCD
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1515u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=blendCalls=colCalls=boolCnt=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    GlobalsRestoreGuard restoreOnExit{regions,saved};
                    // Head-only isolation: do not traverse, destroy, or mutate real nodes/entities.
                    // CSector contains two pointer heads; CRepeatSector contains three.
                    static_assert(sizeof(CSector)==8&&sizeof(CRepeatSector)==12);
                    std::memset(&CWorld::ms_aSectors[0][0],0,sizeof(CWorld::ms_aSectors));
                    std::memset(&CWorld::ms_aRepeatSectors[0][0],0,sizeof(CWorld::ms_aRepeatSectors));
                    At<uint32>(reinterpret_cast<void*>(ADDR_TIME),0)=100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(ADDR_TIME),0);
                    // paso >= 1.6 en ProcessPed de trepar (rama FIX_BUGS fuera del banco)
                    At<float>(reinterpret_cast<void*>(ADDR_STEP),0)=(climb&&nm=="ProcessPed") ? (next()%3 ? 1.6f+float(next()%3)*0.2f : uni(1.6f,3.f)) : (next()%3 ? 1.f : uni(0.5f,3.f));
                    At<uint16>(reinterpret_cast<void*>(ADDR_SCAN),0)=uint16(1000+next()%1000);
                    const uint16 scanNext=uint16(At<uint16>(reinterpret_cast<void*>(ADDR_SCAN),0)+1);
                    At<float>(reinterpret_cast<void*>(ADDR_LOOK_THR),0)=next()%4 ? uni(-1.f,1.f) : 0.70710677f;
                    CPad::GetPad(0)->DisablePlayerControls=next()%2 ? 0 : uint16(1+next()%3);
                    boolMask=next(); assocMask=next(); surfMask=next();
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP
                    // peatones
                    const CVector base{next()%8==0 ? (next()%2 ? uni(0.5f,2.f) : uni(48.f,49.5f)) : uni(5.f,45.f),next()%8==0 ? (next()%2 ? uni(0.5f,2.f) : uni(48.f,49.5f)) : uni(5.f,45.f),uni(0.f,10.f)};
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3; At<void*>(p,E_CLUMP)=clump.data(); At<void*>(p,P_INTEL)=intels[n].data();
                        At<void*>(p,E_MATRIX)=pedMats[n].data();
                        CMatrix m{}; m.SetRotate(next()%2 ? 0.f : uni(-0.3f,0.3f),next()%2 ? 0.f : uni(-0.3f,0.3f),next()%6==0 ? 0.f : uni(-3.2f,3.2f));
                        m.GetPosition()=n==0 ? base : CVector{base.x+uni(-4.f,4.f),base.y+uni(-4.f,4.f),base.z+(next()%5==0 ? (next()%2 ? 3.f : -3.f) : uni(-3.5f,3.5f))};
                        std::memcpy(pedMats[n].data(),&m,sizeof(CMatrix));
                        At<uint32>(p,E_FLAGS)=next(); At<uint32>(p,P_FLAGS1)=next(); At<uint32>(p,P_FLAGS2)=next(); At<uint32>(p,P_FLAGS3)=next(); At<uint32>(p,P_FLAGS4)=next();
                        { static const int32 types[]{0,1,4,5,6,7}; At<int32>(p,P_PEDTYPE)=types[next()%6]; }
                        At<int32>(p,P_MOVESTATE)=int32(next()%8); At<int32>(p,P_ANIMGROUP)=int32(next()%118);
                        { const uint32 h=next()%6; At<float>(p,P_HEALTH)=h==0 ? 0.f : h==1 ? 1.f : h==2 ? 0.99f : h==3 ? qnan : uni(1.f,100.f); }
                        At<float>(p,PH_MASS)=uni(40.f,160.f); At<float>(p,P_CURROT)=uni(-3.2f,3.2f); At<float>(p,P_AIMROT)=uni(-3.2f,3.2f);
                        for(int k=0;k<3;++k)At<float>(p,PH_SPEED+4*k)=next()%5==0 ? 0.f : uni(-0.1f,0.1f);
                        At<void*>(p,PH_IGNORED)=next()%3==0 ? static_cast<void*>(Ent(int(next()%NENTS))) : nullptr;
                        { static const int32 wt[]{0,1,4,16,22,24,25,28,29,30,31,33,35,41,42,43,46}; At<uint8>(p,P_SLOT)=uint8(next()%13);
                          for(int s=0;s<13;++s)At<int32>(p,P_WEAPONS+0x1C*size_t(s))=wt[next()%std::size(wt)]; }
                        At<int32>(intels[n].data(),I_STATIC)=next()%3==0 ? 28+int32(next()%6) : int32(next()%40);
                        for(int k=0;k<6;++k)secondaryRet[n][k]=next()%4==0 ? uintptr_t(fakes[size_t(next()%NFAKES)].data()) : 0;
                    }
                    At<void*>(Ped(0),P_PDATA)=next()%3 ? static_cast<void*>(playerData.data()) : nullptr;
                    for(auto& b:playerData)b=uint8(next()); At<float>(playerData.data(),0x14)=uni(0.f,3.f);
                    for(auto& b:pad)b=uint8(next()); At<uint16>(pad.data(),0x10E)=next()%2 ? 0 : uint16(next());
                    // tareas falsas (algunas en el gestor de tareas de los peatones 0 y 1)
                    for(int n=0;n<NFAKES;++n){ auto* k=fakes[n].data(); for(auto& b:fakes[n])b=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<void*>(k,4)=nullptr; At<void*>(k,8)=nullptr;
                        { static const int32 ty[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_DUCK,TASK_SIMPLE_PAUSE,TASK_COMPLEX_AVOID_ENTITY,
                                                  TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING,TASK_COMPLEX_WANDER};
                          At<int32>(k,FK_TYPE)=ty[next()%std::size(ty)]; }
                        At<void*>(k,FK_SUB)=n<NFAKES-1&&next()%3==0 ? static_cast<void*>(fakes[size_t(n+1)].data()) : nullptr; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; }
                    for(int n=0;n<2;++n)for(int k=0;k<5;++k)At<void*>(intels[n].data(),I_TASKMGR+4*size_t(k))=next()%4==0 ? static_cast<void*>(fakes[size_t(next()%NFAKES)].data()) : nullptr;
                    // gang follower falso: fk3 con lider en +0x10
                    At<CPed*>(fakes[3].data(),0x10)=next()%3 ? Ped(int(2+next()%2)) : nullptr;
                    // escaner de peatones del peaton 0
                    { auto& sc=reinterpret_cast<CPedIntelligence*>(intels[0].data())->GetPedScanner().m_apEntities;
                      for(auto& e:sc){ const uint32 m=next()%5; e=m==0 ? nullptr : reinterpret_cast<CEntity*>(Ped(int(1+(m-1)%3))); } }
                    // asociaciones de animacion y jerarquia
                    for(int n=0;n<3;++n){ auto* a=anims[n].data(); for(auto& b:anims[n])b=uint8(next());
                        static const float amounts[]{1.f,1.f,0.99f,0.995f,0.5f,0.f,-0.f,qnan};
                        static const float deltas[]{-4.f,0.f,-0.f,1.f,-8.f,4.f,qnan};
                        At<float>(a,A_AMOUNT)=amounts[next()%std::size(amounts)]; At<float>(a,A_DELTA)=deltas[next()%std::size(deltas)];
                        At<void*>(a,A_HIER)=hier.data(); At<float>(a,A_TIME)=next()%4==0 ? 1.5f : uni(0.f,2.f); At<float>(a,A_SPEED)=uni(0.f,2.f); At<float>(a,A_STEP)=next()%3==0 ? 0.f : uni(0.f,0.6f);
                        { static const int16 ids[]{55,56,57,58,59,141,3,10,121,128,129,130,131,132,133,134};
                          At<int16>(a,A_ID)=next()%8 ? ids[next()%std::size(ids)] : int16(next()%300); }
                        At<uint16>(a,A_FLAGS)=uint16(next()); }
                    for(auto& b:hier)b=uint8(next()); At<float>(hier.data(),0x10)=next()%3==0 ? 1.5f : uni(0.f,2.f);
                    // entidades: 0-1 edificios, 2-3 objetos, 4-5 vehiculos
                    for(int n=0;n<NENTS;++n){
                        auto* e=ents[n].data(); std::memset(e,0,ents[n].size()); std::memset(entMats[n].data(),0,entMats[n].size());
                        At<void*>(e,0)=entVtbl.data(); e[E_TYPE]=uint8(n<2 ? 1 : n<4 ? 4 : 2); At<int16>(e,E_MODEL)=colModel;
                        At<uint32>(e,E_FLAGS)=(next()%6 ? 1u : 0u)|(next()%2 ? 4u : 0u)|(next()%5==0 ? 0x40000u : 0u);
                        At<uint32>(e,PH_FLAGS)=(next()%2 ? 4u : 0u)|(next()%4==0 ? 0x40u : 0u);
                        { const uint32 m=next()%4; const float s=m==0 ? 0.f : m==1 ? 0.1f : uni(0.f,0.12f); At<CVector>(e,PH_SPEED)=m==1 ? CVector{0.06f,0.08f,0.f} : CVector{s*uni(-1.f,1.f),s*uni(-1.f,1.f),s*uni(-1.f,1.f)}; }
                        { static const int32 sub[]{0,5,6,6,9,3}; At<int32>(e,V_SUBTYPE)=n>=4 ? sub[next()%std::size(sub)] : 0; }
                        At<uint16>(e,E_SCAN)=next()%4==0 ? scanNext : uint16(next());
                        // sin matriz solo donde nadie la asigna (ScanToGrab/ScanToGrabSectorList/ProcessPed la crearian en el pool de matrices real)
                        if(n<2&&(nm=="GetCameraStickModifier"||nm=="GetCameraTargetPos")&&next()%4==0){ At<CVector>(e,E_POS)=CVector{base.x+uni(-3.f,3.f),base.y+uni(-3.f,3.f),base.z+uni(-1.f,3.f)}; At<float>(e,E_HEADING)=uni(-3.2f,3.2f); }
                        else { At<void*>(e,E_MATRIX)=entMats[n].data(); CMatrix m{}; m.SetRotate(0.f,0.f,next()%5==0 ? 0.f : uni(-3.2f,3.2f));
                            m.GetPosition()=CVector{base.x+uni(-3.f,3.f),base.y+uni(-3.f,3.f),base.z+uni(-1.f,3.f)}; std::memcpy(entMats[n].data(),&m,sizeof(CMatrix)); }
                        boundCentres[n]=CVector{base.x+uni(-4.f,4.f),base.y+uni(-3.f,5.f),base.z+uni(-1.f,4.f)};
                        if(next()%6==0)boundCentres[n].x+=40.f; // lejos
                    }
                    // modelo de colision falso (GetColModel) con 0..20 esferas
                    std::memset(fakeCm.data(),0,fakeCm.size()); std::memset(fakeCd.data(),0,fakeCd.size());
                    At<void*>(fakeCm.data(),0x2C)=fakeCd.data(); At<uint16>(fakeCd.data(),0)=uint16(next()%21);
                    // mundo falso: listas de los sectores 59..61 y de sus sectores repetidos
                    { std::memset(nodes.data(),0,nodes.size()); size_t used=0;
                      auto chain=[&](std::initializer_list<int> pool,bool dbl)->void*{ void* head=nullptr; void* prev=nullptr;
                          for(int e:pool){ if(next()%3==0)continue; auto* nd=nodes.data()+used; used+=dbl ? 12 : 8; if(used>nodes.size())break;
                              At<void*>(nd,0)=Ent(e); At<void*>(nd,4)=nullptr; if(dbl)At<void*>(nd,8)=prev; if(prev)At<void*>(prev,4)=nd; else head=nd; prev=nd; }
                          return head; };
                      for(int y=SECTOR0;y<SECTOR0+NCELL;++y)for(int x=SECTOR0;x<SECTOR0+NCELL;++x){
                          auto& sec=CWorld::GetSector(x,y); At<void*>(&sec,0)=chain({0,1},false); At<void*>(&sec,4)=nullptr;
                          auto& rep=CWorld::ms_aRepeatSectors[size_t(y&15)][size_t(x&15)]; At<void*>(&rep,0)=chain({4,5},true); At<void*>(&rep,4)=nullptr; At<void*>(&rep,8)=chain({2,3},true); }
                      At<void*>(listHead.data(),0)=chain({0,2,4,1,3,5},false); }
                    // guion de puntos de contacto
                    for(auto& s:colScript){
                        { const uint32 m=next()%6; s.count=m==0 ? 0 : m==1 ? 2 : 1; }
                        const auto& pp=Ped(0)->GetPosition(); const auto& fw=Ped(0)->GetMatrix().GetForward();
                        s.point=CVector{pp.x+fw.x*uni(0.f,2.f)+uni(-0.5f,0.5f),pp.y+fw.y*uni(0.f,2.f)+uni(-0.5f,0.5f),pp.z+uni(-0.5f,3.f)};
                        { const uint32 m=next()%7; s.normal=m==0 ? CVector{0.f,0.f,1.f} : m==1 ? CVector{0.05f,-0.05f,1.f} : m==2 ? CVector{qnan,0.f,1.f}
                            : m==3 ? CVector{-fw.x,-fw.y,0.f} : m==4 ? CVector{fw.x,fw.y,0.f} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; }
                        s.surfB=uint8(next()%179);
                        { const uint32 m=next()%4; s.pieceA=uint8(m==0 ? next()%4 : m==1 ? (next()%2 ? 4 : 10) : next()%22); }
                    }
                    // respuestas
                    { const uint32 m=next()%4; playerRet=m==0 ? 0 : m==1 ? uintptr_t(Ped(0)) : uintptr_t(Ped(1)); }
                    onScreenRet=next()%5!=0; lookingRet=next()%2; lookEntRet=next()%4 ? 0 : uintptr_t(Ent(2));
                    swimRet=next()%4==0 ? uintptr_t(fakes[4].data()) : 0; findTaskRet=next()%3 ? uintptr_t(fakes[3].data()) : 0;
                    useGunRet=next()%2 ? uintptr_t(fakes[5].data()) : 0;
                    vlineRet=next()%2; colZ=base.z-(next()%3 ? uni(0.f,4.f) : 1.f); routeRet=next()%3!=0; exitRet=next()%5==0;
                    routeA=CVector{base.x+uni(-3.f,3.f),base.y+uni(-3.f,3.f),base.z}; routeB=CVector{base.x+uni(-3.f,3.f),base.y+uni(-3.f,3.f),base.z};
                    offsetRet=CVector{uni(-2.f,2.f),uni(-2.f,2.f),0.f}; speedAdd=CVector{uni(-0.05f,0.05f),uni(-0.05f,0.05f),0.f};
                    { static const float fats[]{0.f,0.5f,1.f,1.5f,2.f,2.7f}; fatRet=fats[next()%std::size(fats)]; } skillRet=int32(next()%3);
                    // evento falso
                    for(auto& b:eventBuf)b=uint8(next()); At<void*>(eventBuf.data(),0)=eventVtbl.data();
                    { static const int ty[]{EVENT_SHOT_FIRED_WHIZZED_BY,EVENT_SHOT_FIRED_WHIZZED_BY,EVENT_DAMAGE,EVENT_SCRIPT_COMMAND}; eventType=ty[next()%4]; }
                    At<int16>(eventBuf.data(),0xE)=next()%2 ? int16(TASK_SIMPLE_DUCK_WHILE_SHOTS_WHIZZING) : int16(next()%500);
                    eventPrio=next()%3==0 ? 71 : int(next()%100); eventSource=next()%3 ? uintptr_t(Ped(1)) : 0;
                    const int prio=int(next()%3); const CEvent* ev=next()%4 ? reinterpret_cast<const CEvent*>(eventBuf.data()) : nullptr;
                    for(auto& b:vec)b=uint8(next()); std::memset(pedArrays.data(),0,pedArrays.size());

                    // objeto bajo prueba
                    auto* o=object.data(); std::memset(o,0xCD,object.size());
                    CPed* const pd=Ped(0);
                    auto randVec=[&]{ return CVector{base.x+uni(-6.f,6.f),base.y+uni(-6.f,6.f),base.z+uni(-2.f,2.f)}; };
                    if(avoid){
                        At<uintptr_t>(o,0)=VT_AVOID; At<void*>(o,4)=nullptr; At<void*>(o,8)=Fake(0);
                        { const uint32 m=next()%8; At<CPed*>(o,0xC)=m==0 ? nullptr : m==1 ? Ped(2) : Ped(1); } Reg(At<CPed*>(o,0xC),o+0xC);
                        At<CVector>(o,0x10)=randVec(); At<CVector>(o,0x1C)=next()%5==0 ? CVector{base.x+uni(-2.f,2.f),base.y+uni(-2.f,2.f),base.z} : randVec();
                        At<CVector>(o,0x28)=randVec(); At<CVector>(o,0x34)=randVec();
                        for(size_t t:{0x40u,0x4Cu}){ At<uint32>(o,t)=now-next()%3000; At<int32>(o,t+4)=next()%6==0 ? -1 : int32(next()%2500); o[t+8]=uint8(next()%3!=0); o[t+9]=uint8(next()%4==0); }
                        At<int32>(o,0x58)=int32(4+next()%4); o[0x5C]=uint8(next());
                        if(next()%3==0)At<void*>(o,4)=Fake(int(next()%NFAKES));
                    } else if(climb){
                        At<uintptr_t>(o,0)=VT_CLIMB; At<void*>(o,4)=nullptr;
                        o[8]=uint8(next()%5==0); o[9]=uint8(next()%3==0); o[0xA]=uint8(next()%2); o[0xB]=uint8(next()%3==0); o[0xC]=uint8(next()%4==0);
                        o[0xD]=uint8(next()%7); o[0xE]=uint8(next()%7); o[0xF]=uint8(next()%179); o[0x10]=uint8(next()%3==0 ? 0xFF : next()%3==0 ? 9 : next()%25);
                        At<float>(o,0x14)=uni(-3.2f,3.2f);
                        { const uint32 m=next()%8; At<CEntity*>(o,0x24)=m==0 ? nullptr : Ent(int(next()%NENTS)); } Reg(At<CEntity*>(o,0x24),o+0x24);
                        auto* ce=At<CEntity*>(o,0x24);
                        At<CVector>(o,0x18)=ce&&ce->GetIsTypePhysical() ? CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(0.f,2.f)} : randVec();
                        At<uint16>(o,0x28)=uint16(next()%3==0 ? 900+next()%200 : next()%600);
                        { const uint32 m=next()%5; At<void*>(o,0x2C)=m==0 ? nullptr : static_cast<void*>(Anim(int(m%3))); }
                    } else {
                        At<uintptr_t>(o,0)=VT_DUCK; At<void*>(o,4)=nullptr;
                        At<uint32>(o,8)=now-next()%5000; At<uint16>(o,0xC)=uint16(next()%3==0 ? 0 : next()%5000);
                        { const uint32 m=next()%5; At<int16>(o,0xE)=m==0 ? -1 : m==1 ? 0 : int16(next()%3000); }
                        { const uint32 m=next()%4; At<void*>(o,0x10)=m==0 ? nullptr : static_cast<void*>(Anim(int(m%3))); }
                        { const uint32 m=next()%3; At<void*>(o,0x14)=m==0 ? nullptr : static_cast<void*>(Anim(int(1+m%2))); }
                        o[0x18]=uint8(next()%4==0); o[0x19]=uint8(next()%4==0); o[0x1A]=uint8(next()%2); o[0x1B]=uint8(next()%2);
                        { static const float mx[]{0.f,0.f,1.f,-1.f,0.5f,-0.3f}; static const float my[]{0.f,0.f,0.5f,-0.5f,0.05f,1.f,-1.f};
                          At<float>(o,0x1C)=mx[next()%std::size(mx)]; At<float>(o,0x20)=next()%8==0 ? qnan : my[next()%std::size(my)]; }
                        o[0x24]=uint8(next()%5); o[0x25]=uint8(next()%3==0 ? 0 : next()%3==0 ? 1 : next()%8);
                        // tareas que pueden devolver GetSimplestActiveTask / GetTaskSecondary(DUCK): esta tarea, otra de agacharse o ninguna
                        if(nm=="ProcessPed"){
                            { const uint32 m=next()%4; At<void*>(intels[0].data(),I_TASKMGR)=m==0 ? nullptr : m==1 ? static_cast<void*>(o) : static_cast<void*>(Fake(1)); At<void*>(Fake(1),FK_SUB)=nullptr; }
                            { const uint32 m=next()%4; secondaryRet[0][1]=m==0 ? 0 : m==1 ? uintptr_t(o) : uintptr_t(Fake(2)); }
                            At<int32>(Fake(1),FK_TYPE)=next()%2 ? TASK_SIMPLE_DUCK : TASK_SIMPLE_PAUSE; At<int32>(Fake(2),FK_TYPE)=next()%2 ? TASK_SIMPLE_DUCK : TASK_SIMPLE_PAUSE;
                            // la otra tarea de agacharse se aborta con AbortBecauseOfOtherDuck: buffer con forma de CTaskSimpleDuck
                            for(int k:{1,2}){ At<void*>(Fake(k),0x10)=next()%2 ? static_cast<void*>(Anim(0)) : nullptr; At<void*>(Fake(k),0x14)=next()%2 ? static_cast<void*>(Anim(2)) : nullptr; }
                        }
                    }

                    // argumentos por funcion (antes de la instantanea)
                    std::array<int32,8> ai{}; std::array<float,4> af{}; std::array<void*,4> ap{};
                    if(avoid){
                        if(nm=="Constructor"){ ap[0]=next()%4 ? static_cast<void*>(Ped(1)) : nullptr; At<CVector>(vec.data(),0)=randVec(); ai[0]=int32(next()%8); }
                        if(nm=="ComputeSphere"||nm=="NearbyPedsInSphere"){
                            auto* a=reinterpret_cast<CPed**>(pedArrays.data()); auto* b=a+16;
                            for(int k=0;k<16;++k){ const uint32 m=next()%5; a[k]=m<2 ? nullptr : Ped(int(m-1)); }
                            for(int k=0;k<16;++k){ const uint32 m=next()%6; b[k]=m<3 ? nullptr : Ped(int(m-2)); }
                            if(nm=="ComputeSphere"&&next()%2)a[0]=Ped(1); // al menos un peaton (el EXE divide por el numero)
                            if(nm=="ComputeSphere"&&!std::any_of(a,a+16,[](CPed* x){return x!=nullptr;}))a[next()%16]=Ped(3);
                            if(nm=="NearbyPedsInSphere"&&next()%4==0)for(int k=0;k<16;++k)b[k]=Ped(1+k%3); // sin hueco libre
                            auto* sp=vec.data(); At<CVector>(sp,0)=CVector{base.x+uni(-2.f,2.f),base.y+uni(-2.f,2.f),base.z+uni(-2.f,2.f)}; At<float>(sp,0xC)=uni(0.5f,4.f);
                        }
                        if(nm=="ComputeAvoidSphere"||nm=="ComputeDetourTarget"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask"||nm=="ComputeRouteRoundSphere")
                            if(!At<CPed*>(o,0xC)&&nm!="CreateFirstSubTask"&&nm!="CreateNextSubTask"){ At<CPed*>(o,0xC)=Ped(1); Reg(Ped(1),o+0xC); }
                        if(nm=="ComputeRouteRoundSphere"){ auto* sp=vec.data(); At<CVector>(sp,0)=randVec(); At<float>(sp,0xC)=uni(0.5f,4.f); sp[0x10]=uint8(next()); sp[0x11]=uint8(next()); sp[0x12]=uint8(next()); }
                        if(nm=="ComputeAvoidSphere")std::memset(vec.data(),0xCD,0x14);
                        if(nm=="ComputeSphere")std::memset(vec.data()+0x20,0xCD,0x14);
                        if(nm=="ControlSubTask"||nm=="CreateNextSubTask"||nm=="MakeAbortable"){
                            static const int32 ty[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_PAUSE};
                            At<int32>(Fake(0),FK_TYPE)=ty[next()%std::size(ty)]; At<void*>(Fake(0),FK_SUB)=nullptr; }
                        if(nm=="ControlSubTask"&&At<CPed*>(o,0xC)){
                            // ramas de salida: distancia 2D cerca de 1 y de sqrt(7.25), altura 3 exacta, frentes paralelos
                            auto* other=At<CPed*>(o,0xC); auto& op=At<CVector>(At<void*>(other,E_MATRIX),0x30); const auto pp=pd->GetPosition();
                            const uint32 m=next()%8;
                            if(m==0)op=CVector{pp.x+1.f,pp.y,pp.z}; else if(m==1)op=CVector{pp.x+std::sqrt(7.25f),pp.y,pp.z+uni(-1.f,1.f)};
                            else if(m==2)op=CVector{pp.x+uni(-1.f,1.f),pp.y+uni(-1.f,1.f),pp.z+3.f}; else if(m==3)op=CVector{pp.x+uni(-2.f,2.f),pp.y+uni(-2.f,2.f),pp.z};
                            if(next()%3==0)std::memcpy(At<void*>(other,E_MATRIX),At<void*>(pd,E_MATRIX),0x30);
                        }
                    } else if(climb){
                        if(nm=="Constructor"){ { const uint32 m=next()%4; ap[0]=m==0 ? nullptr : static_cast<void*>(Ent(int(next()%NENTS))); }
                            At<CVector>(vec.data(),0)=randVec(); af[0]=uni(-3.2f,3.2f); ai[0]=int32(next()%179); ai[1]=int32(next()%7); ai[2]=int32(next()%2); }
                        if(nm=="DeleteAnimCB"){ ap[0]=next()%3 ? static_cast<void*>(Anim(int(next()%3))) : nullptr; }
                        if(nm=="GetCameraStickModifier"){ ap[0]=static_cast<void*>(next()%4==0 ? static_cast<void*>(Ped(1)) : static_cast<void*>(Ent(int(next()%NENTS))));
                            if(At<CEntity*>(o,0x24)==nullptr){ At<CEntity*>(o,0x24)=Ent(0); Reg(Ent(0),o+0x24); }
                            for(int k=0;k<4;++k)At<float>(vec.data(),4*k)=uni(-3.5f,3.5f); if(next()%4==0)At<float>(vec.data(),0)=next()%2 ? -0.1f : -0.6f; }
                        if(nm=="GetCameraTargetPos"||nm=="ProcessPed"||nm=="TestForVault"||nm=="MakeAbortable"){ if(At<CEntity*>(o,0x24)==nullptr&&(nm!="ProcessPed"||next()%4)){ At<CEntity*>(o,0x24)=Ent(int(next()%NENTS)); Reg(At<CEntity*>(o,0x24),o+0x24); } }
                        if(nm=="StartAnim"||nm=="ProcessPed"){ if(o[0xD]==2&&o[0xE]!=0&&!At<void*>(o,0x2C)&&next()%2)At<void*>(o,0x2C)=Anim(0); }
                        // ProcessPed desreferencia la animacion si la altura de posicion es > 0: sin animacion solo con alturas que la crean
                        if(nm=="ProcessPed"&&o[0xD]==0&&!At<void*>(o,0x2C))At<void*>(o,0x2C)=Anim(0);
                        if(nm=="ScanToGrab"||nm=="ScanToGrabSectorList<CPtrListSingleLink<CPhysical*>>"||nm=="TestForClimb"){
                            At<CVector>(vec.data(),0)=randVec(); At<float>(vec.data(),0xC)=uni(-3.2f,3.2f); At<int32>(vec.data(),0x10)=int32(next()%179);
                            ai[0]=int32(next()%2); ai[1]=int32(next()%3==0); ai[2]=int32(next()%3==0); if(ai[1]&&ai[2]&&next()%2)ai[2]=0;
                            At<CVector>(vec.data(),0x20)=randVec(); ap[0]=next()%2 ? static_cast<void*>(vec.data()+0x20) : nullptr; }
                        if(nm=="TestForStandUp"||nm=="TestForVault"){ At<CVector>(vec.data(),0)=randVec(); af[0]=uni(-3.2f,3.2f); }
                        if(nm=="CreateColModel"||nm=="Shutdown"){ /* sin argumentos */ }
                    } else {
                        if(nm=="Constructor"){ ai[0]=int32(next()%5); ai[1]=int32(next()%3==0 ? 0 : next()%5000); { const uint32 m=next()%4; ai[2]=m==0 ? -1 : m==1 ? 0 : int32(next()%3000); } }
                        if(nm=="ControlDuckMove"){ static const float vals[]{0.f,0.f,1.f,-1.f,0.9f,-0.9f,0.95f,0.1f,-0.1f,0.05f,0.5f,-0.5f}; af[0]=vals[next()%std::size(vals)]; af[1]=next()%10==0 ? qnan : vals[next()%std::size(vals)];
                            if(next()%3==0)af[0]=uni(-1.f,1.f); if(next()%3==0)af[1]=At<float>(o,0x20)+uni(-0.2f,0.2f); }
                        if(nm=="DeleteDuckAnimCB"){ ap[0]=next()%6 ? static_cast<void*>(Anim(int(next()%3))) : nullptr; ap[1]=next()%6 ? static_cast<void*>(o) : nullptr; }
                        if(nm=="SetControlType")ai[0]=int32(next()%5);
                        if(nm=="SetDuckTimer")ai[0]=int32(next()%60000);
                        if(nm=="SetMoveAnim"&&next()%3==0){ static const float my[]{0.f,0.3f,-0.3f,0.7f,-0.7f,0.59f}; At<float>(o,0x20)=my[next()%std::size(my)]; At<float>(o,0x1C)=next()%2 ? 0.f : At<float>(o,0x1C); }
                        if(nm=="ProcessPed"&&next()%3==0)At<uint32>(o,8)=now-At<uint16>(o,0xC)+uint32(next()%3)-1u; // fin exacto del temporizador
                    }
                    const auto before=Snapshot(); const auto occ=Occupancy();
                    std::vector<uint8> cmSaved; // CreateColModel / Shutdown: copias de los 4 modelos
                    if(nm=="CreateColModel"||nm=="Shutdown"){ cmSaved=ReversingProbe::Bytes(climbCms,0xC0); for(int k=0;k<4;++k)At<void*>(climbCms+0x30*k,0x2C)=nullptr;
                        if(nm=="Shutdown")reinterpret_cast<bool(__cdecl*)()>(0x67A890)(); } // se crean antes (codigo del EXE salvo con el hook activo)

                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json ret=nullptr; uintptr_t rawRet{}; bool isPtr=false; json extra=nullptr;
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor";
                    auto callTask=[&]{ isPtr=true; rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,pd)); };
                    if(avoid){
                        if(isCtor){ std::memset(o,0xCD,0x60); isPtr=true; rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,const CVector*,int32)>(target)(o,ap[0],&At<CVector>(vec.data(),0),ai[0])); }
                        else if(isDtor)reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Clone"){ isPtr=true; rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(target)(o,pd,prio,ev);
                        else if(nm=="ControlSubTask"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask")callTask();
                        else if(nm=="QuitIK"||nm=="SetUpIK")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="ComputeSphere"){ isPtr=true; rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*)>(target)(o,vec.data()+0x20,pedArrays.data())); }
                        else if(nm=="NearbyPedsInSphere")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,const void*,void*,void*)>(target)(o,pd,vec.data(),pedArrays.data(),pedArrays.data()+0x40);
                        else if(nm=="ComputeAvoidSphere")reinterpret_cast<void(__thiscall*)(void*,CPed*,void*)>(target)(o,pd,vec.data());
                        else if(nm=="ComputeRouteRoundSphere")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,void*)>(target)(o,pd,vec.data());
                        else if(nm=="ComputeDetourTarget")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    } else if(climb){
                        if(isCtor){ std::memset(o,0xCD,0x30); isPtr=true;
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,const CVector*,float,int32,int32,int32)>(target)(o,ap[0],&At<CVector>(vec.data(),0),af[0],ai[0],ai[1],ai[2])); }
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(target)(o,pd,prio,ev);
                        else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="StartAnim"||nm=="StartSpeech")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="DeleteAnimCB")reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(ap[0],o);
                        else if(nm=="GetCameraStickModifier"){ auto* v=reinterpret_cast<float*>(vec.data());
                            reinterpret_cast<void(__thiscall*)(void*,void*,float*,float*,float*,float*)>(target)(o,ap[0],v,v+1,v+2,v+3); }
                        else if(nm=="GetCameraTargetPos")reinterpret_cast<void(__thiscall*)(void*,CPed*,CVector*)>(target)(o,pd,&At<CVector>(vec.data(),0x30));
                        else if(nm=="ScanToGrab"){ isPtr=true; rawRet=uintptr_t(reinterpret_cast<CEntity*(__cdecl*)(CPed*,void*,void*,void*,uint32,uint32,uint32,void*)>(target)(
                                pd,vec.data(),vec.data()+0xC,vec.data()+0x10,uint32(ai[0]),uint32(ai[1]),uint32(ai[2]),ap[0])); }
                        else if(nm=="ScanToGrabSectorList<CPtrListSingleLink<CPhysical*>>"){ isPtr=true; rawRet=uintptr_t(reinterpret_cast<CEntity*(__cdecl*)(void*,CPed*,void*,void*,void*,uint32,uint32,uint32)>(target)(
                                listHead.data(),pd,vec.data(),vec.data()+0xC,vec.data()+0x10,uint32(ai[0]),uint32(ai[1]),uint32(ai[2]))); }
                        else if(nm=="TestForClimb"){ isPtr=true; rawRet=uintptr_t(reinterpret_cast<CEntity*(__cdecl*)(CPed*,void*,void*,void*,uint32)>(target)(pd,vec.data(),vec.data()+0xC,vec.data()+0x10,uint32(ai[0]))); }
                        else if(nm=="TestForStandUp"||nm=="TestForVault")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,const CVector*,float)>(target)(o,pd,&At<CVector>(vec.data(),0),af[0]);
                        else if(nm=="CreateColModel")ret=reinterpret_cast<bool(__cdecl*)()>(target)();
                        else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(target)();
                    } else {
                        if(isCtor){ std::memset(o,0xCD,0x28); isPtr=true; rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,int32)>(target)(o,ai[0],ai[1],ai[2])); }
                        else if(isDtor)reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Clone"){ isPtr=true; rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const CEvent*)>(target)(o,pd,prio,ev);
                        else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="CanPedDuck")ret=reinterpret_cast<bool(__cdecl*)(CPed*)>(target)(pd);
                        else if(nm=="ControlDuckMove")ret=reinterpret_cast<bool(__thiscall*)(void*,float,float)>(target)(o,af[0],af[1]);
                        else if(nm=="DeleteDuckAnimCB")reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(ap[0],ap[1]);
                        else if(nm=="ForceStopMove")reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else if(nm=="IsTaskInUseByOtherTasks")ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o);
                        else if(nm=="RestartTask"||nm=="SetMoveAnim")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd); // RestartTask: el EXE devuelve this, el DLL nada (no se compara)
                        else if(nm=="SetControlType")ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(target)(o,ai[0]);
                        else if(nm=="SetDuckTimer")reinterpret_cast<void(__thiscall*)(void*,int32)>(target)(o,ai[0]);
                    }
                    setState(false);

                    if(nm=="CreateColModel"||nm=="Shutdown"){
                        json models=json::array(); for(int k=0;k<4;++k)models.push_back(SerColModel(climbCms+0x30*k)); extra=models;
                        for(int k=0;k<4;++k){ auto* cm=reinterpret_cast<CColModel*>(climbCms+0x30*k); if(cm->m_pColData)cm->RemoveCollisionVolumes(); }
                        std::memcpy(climbCms,cmSaved.data(),cmSaved.size());
                    }
                    std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(isPtr)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    const size_t objSize=avoid ? 0x60 : climb ? 0x30 : 0x28;
                    json objectWords=Region(o,objSize,isCtor||isDtor);
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // tipo en vez del vptr
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"changes",Changes(before)},{"refs",AllRefs()},{"extra",extra},
                            {"scan_code",At<uint16>(reinterpret_cast<void*>(ADDR_SCAN),0)},{"switches",switches}};
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["rng"]=rngCalls; r["clean"]=Occupancy()==occ;
                    freshIds.clear(); CleanRefs(); restoreGlobals();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",original},{"candidate",candidate}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
