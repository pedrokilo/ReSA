#pragma once
// Banco dentro del motor: eventos de bici y de script, registros de tareas de script, sombras en tiempo real,
// rectangulos y coches atascados (lista de 8; 48 funciones de 6 clases).
// CEventKnockOffBike (los dos constructores, AffectsPed, ReportCriminalEvent, From, SetPedOutCar, CalcForcesAndAnims,
// SetPedSafePosition), CEventScriptCommand (constructor, GetEventType, GetEventPriority, Clone, AffectsPed,
// TakesPriorityOver, IsValid, CloneScriptTask), CPedScriptedTaskRecordData (constructor, AssociateWithEvent/Task, los dos
// Set, SetAsGroupTask, SetAsAttractorScriptTask, Flush), CRealTimeShadow (constructor, destructor, Create, Destroy,
// DrawBorderAroundTexture, GetShadowRwTexture, SetLightProperties, Update), CRect (IsFlipped, Restrict, Resize, los dos
// IsPointInside, SetFromCenter, GetCenter, StretchToPoint) y CStuckCarCheck (Init, AddCarToCheck, AttemptToWarpVehicle,
// ClearStuckFlagForCar, HasCarBeenStuckForAWhile, IsCarInStuckCarArray, Process, RemoveCarFromCheck).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: peatones y vehiculos falsos
// (vtable de trampas; TestCollision del peaton trazado), tareas y eventos falsos (vtable propia), objetos RW falsos
// (luz, marcos, objeto RW, camaras y rasters) y un pool de vehiculos falso (cabecera, estados con byte de guarda en [-1]
// y 5 huecos de 0xA18) con un modelo falso (entrada 400 de ms_modelInfoPtrs sustituida durante la ejecucion).
// Se simulan y trazan: FindPlayerWanted, CWanted::RegisterCrime, CPhysical::ApplyMoveForce (las dos),
// CGeneral::GetRandomNumberInRange<float>, CGeneral::LimitRadianAngle, CMatrix::SetRotateZOnly,
// CPed::SetPedPositionInCar/PositionPedOutOfCollision, CWorld::GetIsLineOfSightClear/FindMissionEntitiesIntersectingCube,
// CTaskSimpleCarSetPedOut::ProcessPed (la tarea de la pila se serializa en el momento), CCarEnterExit::RemoveCarSitAnim,
// CCamera::IsSphereVisible (3 args), CCarCtrl::SetCoordsOfScriptCar, CUpsideDownCarCheck::IsCarUpsideDown,
// CEntity::GetBoundCentre, CPathFind::FindNodeCoorsForScript (las dos)/FindNthNodeClosestToCoors/
// FindNodeOrientationForCarPlacement, CShadowCamera (Destroy, Create, SetLight, SetCenter, Update x2, RasterResample,
// RasterBlur, DrawBlurryRaster2, DrawOutlineBorder), las llamadas RW de luces y marcos (RpLightCreate/Destroy/SetColor,
// RwFrameCreate/Destroy/Rotate, _rwObjectHasFrameSetFrame, RwV3dTransformPoints) y el RNG.
// Estado global que se toca y se restaura en cada ejecucion y al final: el pool de eventos, el reloj, el puntero del pool
// de vehiculos, CWorld::pIgnoreEntity, TheCamera.m_bMirrorActive y CModelInfo::ms_modelInfoPtrs[400].
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Events/EventKnockOffBike.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "RealTimeShadow.h"
#include "ShadowCamera.h"
#include "StuckCarCheck.h"
#include "UpsideDownCarCheck.h"
#include "PathFind.h"
#include "CarCtrl.h"
#include "CarEnterExit.h"
#include "TaskSimpleCarSetPedOut.h"
#include "Wanted.h"
#include "General.h"
#include "Pools/Pools.h"
#include "Pools/EventPool.h"
#include "Pools/TaskPool.h"
#include "Entity/Vehicle/Automobile.h"
#include "../ProbeRuntime.hpp"

namespace Data8bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_RWOBJ=0x18, E_MODEL=0x22, E_STATUS=0x36;                       // CEntity
constexpr size_t PH_MOVE=0x44, PH_FLAGS=0x40, PH_MASS=0x8C, PH_IGNORED=0x128;                                  // CPhysical
constexpr size_t P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_STATE=0x530, P_HEALTH=0x540, P_CURROT=0x558,
                 P_AIMROT=0x55C, P_STANDING=0x568, P_TYPE=0x598;                                                // CPed
constexpr size_t V_DRIVER=0x460, V_PASSENGERS=0x464, V_TYPE=0x590, V_SUBTYPE=0x594, V_AUTOPILOT=0x390,
                 V_BIKE_LEANMAT=0x5C8, V_BIKE_LEAN=0x648, V_WHEELS_BIKE=0x804, V_WHEELS_AUTO=0x960;              // CVehicle
constexpr size_t FK_TYPE=0x38, FK_PRIO=0x3C;                                                                    // tarea/evento falsos
constexpr uintptr_t VT_KO=0x85B2E0, VT_SC=0x85B378;
constexpr uintptr_t TIMER=0xB7CB84, VEHPOOL=0xB74494, IGNORE_ENT=0xB7CD68, MIRROR=0xB6F998, MODELPTR400=0xA9B0C8+4*400;
constexpr size_t EVENT_SLOT=0x44, TASK_SLOT=0x80, POOL_SLOT=0xA18;
constexpr int NPEDS=6, NVEH=3, FAKES=6, NPOOL=4;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> evs{};
alignas(16) inline std::array<uint8,0x260> object{};
alignas(16) inline std::array<uint8,0x60> src{};
alignas(16) inline std::array<uint8,0x40> refEv{}, pt{}, wanted{}, mi{}, col{}, light{}, rwobj{}, cvec{};
alignas(16) inline std::array<uint8,0x60> frameA{}, frameB{};
alignas(16) inline std::array<uint8,0x80> camA{}, camB{};
alignas(16) inline std::array<uint8,0x10> rast0{}, rast1{}, rast2{}, rast3{}, texA{}, texB{};
// pool de vehiculos falso: el hueco 0 queda delante del almacenamiento (lo usa el EXE si el estado [-1] coincide)
alignas(16) inline std::array<uint8,POOL_SLOT*(NPOOL+1)> poolStorage{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPOOL+1> poolMats{};
alignas(16) inline std::array<uint8,0x20> poolStates{}, poolHdr{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uint8* PoolVeh(int n) { return poolStorage.data()+POOL_SLOT*size_t(n); }  // n=0: hueco de guarda; 1..NPOOL: indices 0..NPOOL-1

inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }

inline std::array<void*,16> taskVtbl{};
inline std::array<void*,20> eventVtbl{};
inline std::array<void*,80> vehVtbl{}, pedVtbl{};
inline uintptr_t stackLo{}, stackHi{};
struct Buf { const uint8* p; size_t n; const char* name; int idx; };
inline std::vector<Buf> bufs;
inline void InitBufs() {
    bufs.clear();
    for(int n=0;n<NPEDS;++n){ bufs.push_back({peds[n].data(),peds[n].size(),"ped",n}); bufs.push_back({pedMats[n].data(),pedMats[n].size(),"pmat",n}); }
    for(int n=0;n<NVEH;++n){ bufs.push_back({vehs[n].data(),vehs[n].size(),"veh",n}); bufs.push_back({vehMats[n].data(),vehMats[n].size(),"vmat",n}); }
    for(int n=0;n<FAKES;++n)bufs.push_back({fakes[n].data(),fakes[n].size(),"fk",n});
    for(int n=0;n<2;++n)bufs.push_back({evs[n].data(),evs[n].size(),"ev",n});
    for(int n=0;n<=NPOOL;++n)bufs.push_back({poolMats[n].data(),poolMats[n].size(),"poolmat",n});
#define D8_B(b,nm) bufs.push_back({b.data(),b.size(),nm,-1});
    D8_B(object,"object") D8_B(src,"src") D8_B(refEv,"refEv") D8_B(pt,"pt") D8_B(wanted,"wanted") D8_B(mi,"mi") D8_B(col,"col")
    D8_B(light,"light") D8_B(rwobj,"rwobj") D8_B(cvec,"cvec") D8_B(frameA,"frameA") D8_B(frameB,"frameB") D8_B(camA,"camA") D8_B(camB,"camB")
    D8_B(rast0,"rast0") D8_B(rast1,"rast1") D8_B(rast2,"rast2") D8_B(rast3,"rast3") D8_B(texA,"texA") D8_B(texB,"texB")
    D8_B(poolStorage,"poolveh") D8_B(poolStates,"poolstates") D8_B(poolHdr,"poolhdr")
#undef D8_B
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v>=stackLo&&v<stackHi)return "stack";
    if(v==uintptr_t(taskVtbl.data()))return "task_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(pedVtbl.data()))return "ped_vt";
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n){ std::string s=std::string(b.name)+"+"+std::to_string(v-uintptr_t(b.p)); if(b.idx>=0)s+="#"+std::to_string(b.idx); return s; }
    { const auto es=uintptr_t(EvStorage()); if(v>=es&&v<es+EvCap()*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,size_t from=0) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=from/4;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(const void* e) { json l=json::array(); for(auto* r=static_cast<const CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

// configuracion por caso de las dependencias simuladas: cada mascara se consume bit a bit con su contador
inline uint32 seed{}, rngCalls{};
inline uint32 mVis{}, cVis{}, mFind{}, cFind{}, mUpside{}, cUpside{}, mFound{}, cFound{}, mCol{}, cCol{}, mLos{}, cLos{}, mPosOut{}, cPosOut{},
              mCam{}, cCam{}, mClone{}, cClone{}, mLight{}, cLight{}, cRange{};
inline bool Bit(uint32 mask,uint32& calls) { return (mask>>(calls++%32))&1; }
inline uintptr_t wantedRet{};

// tareas falsas: GetTaskType (hueco 4) y Clone (hueco 1) trazados
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"get_task_type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline void* __fastcall FClone(CTask* t,void*) { trace.push_back({"task_clone",Name(t)}); return Bit(mClone,cClone) ? (void*)Fake(5) : nullptr; }
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(t)}); return nullptr; }
// evento de referencia de TakesPriorityOver: tipo y prioridad trazados
inline int __fastcall EType(void* e,void*) { trace.push_back({"ref_event_type",Name(e)}); return At<int32>(e,FK_TYPE); }
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"ref_event_priority",Name(e)}); return At<int32>(e,FK_PRIO); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
// peatones: TestCollision (hueco 13) trazado; el resto son trampas
inline bool __fastcall FTestCol(void* p,void*,uint32 applySpeed) {
    const CVector pos=static_cast<CPed*>(p)->GetPosition();
    trace.push_back({"test_collision",Name(p),applySpeed&0xFF,F(pos.x),F(pos.y),F(pos.z),Name(At<uintptr_t>(p,PH_IGNORED))}); return Bit(mCol,cCol);
}
template<int N> void* __fastcall VTrap(void* v,void*) { trace.push_back({"veh_unexpected_slot",N,Name(v)}); return nullptr; }
template<int N> void* __fastcall PTrap(void* p,void*) { trace.push_back({"ped_unexpected_slot",N,Name(p)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) {
    ((taskVtbl[N%16]=reinterpret_cast<void*>(&FTrap<N%16>)),...); ((eventVtbl[N%20]=reinterpret_cast<void*>(&ETrap<N%20>)),...);
    ((vehVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...);
}

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline float __cdecl FakeRandRange(float a,float b) {
    trace.push_back({"random_in_range",F(a),F(b)}); const uint32 k=(seed*2246822519u+cRange++*3266489917u)>>16;
    return a+(b-a)*(float(k%1000)/1000.f);
}
// buscados y crimenes
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wantedRet); }
inline void __fastcall FakeRegisterCrime(void* w,void*,int32 type,const CVector* pos,uint32 id,uint32 dontCare) {
    trace.push_back({"register_crime",Name(w),type,F(pos->x),F(pos->y),F(pos->z),Name(id),dontCare&0xFF});
}
// fisica, angulos y posiciones
inline void __fastcall FakeMoveForce(void* p,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(p),F(x),F(y),F(z)}); }
inline float __cdecl FakeLimitRadian(float a) { trace.push_back({"limit_radian_angle",F(a)}); return a*0.5f; }
inline void __fastcall FakeRotZ(void* m,void*,float a) { trace.push_back({"set_rotate_z_only",Name(m),F(a)}); }
inline void __fastcall FakePosInCar(void* p,void*) { trace.push_back({"set_ped_position_in_car",Name(p)}); }
inline bool __fastcall FakePosOut(void* p,void*,int32 door,void* v,uint32 findClosest) {
    trace.push_back({"position_ped_out_of_collision",Name(p),door,Name(v),findClosest&0xFF}); return Bit(mPosOut,cPosOut);
}
inline bool __cdecl FakeLos(const CVector& a,const CVector& b,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7) {
    trace.push_back({"line_of_sight",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),b1,b2,b3,b4,b5,b6,b7,Name(At<uintptr_t>(reinterpret_cast<void*>(IGNORE_ENT),0))});
    return Bit(mLos,cLos);
}
// SetPedOutCar: la tarea de la pila se serializa (sin vptr; bytes 0x16/0x17 sin escribir enmascarados)
inline bool __fastcall FakeProcessPed(void* task,void*,CPed* p) {
    trace.push_back({"set_ped_out_process_ped",Name(task),Name(p),Name(At<uintptr_t>(task,4)),Name(At<uintptr_t>(task,8)),At<uint32>(task,0xC),
                     At<uint32>(task,0x10),At<uint32>(task,0x14)&0xFFFFu});
    return true;
}
inline void __cdecl FakeRemoveSit(const CPed* p) { trace.push_back({"remove_car_sit_anim",Name(p)}); }
// coches atascados: camara, mision, coordenadas, coches volcados, caminos y centro de la esfera
inline bool __fastcall FakeSphereVis(void* cam,void*,const CVector* c,float r,void* m) {
    trace.push_back({"is_sphere_visible",Name(cam),F(c->x),F(c->y),F(c->z),F(r),Name(m)}); return Bit(mVis,cVis);
}
inline void __cdecl FakeFindMission(const CVector& a,const CVector& b,int16* count,int16 maxCount,CEntity** out,bool v,bool p,bool o) {
    trace.push_back({"find_mission_entities",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),Name(count),maxCount,Name(out),v,p,o});
    *count=int16(Bit(mFind,cFind) ? 1+cFind%3 : 0);
}
inline void __cdecl FakeSetCoords(void* v,float x,float y,float z,uint8 a,uint8 b) { trace.push_back({"set_coords_of_script_car",Name(v),F(x),F(y),F(z),a,b}); }
inline bool __fastcall FakeUpside(void* self,void*,const void* v) { trace.push_back({"is_car_upside_down",Name(self),Name(v)}); return Bit(mUpside,cUpside); }
inline CVector* __fastcall FakeBoundCentre(void* e,void*,CVector* out) {
    trace.push_back({"get_bound_centre",Name(e),Name(out)}); const float k=float((uintptr_t(e)>>4)&0xFF);
    *out=CVector{k*0.5f+1.f,k*-0.25f,k+0.125f}; return out;
}
inline CVector* __fastcall FakeCoorsTwo(void* pf,void*,CVector* out,uint32 a,uint32 b,float* heading,bool* found) {
    trace.push_back({"find_node_coors_two",Name(pf),Name(out),a,b,Name(heading),Name(found)});
    *found=Bit(mFound,cFound); *heading=float(cFound*37%360); *out=CVector{float(a&0xFFF),float(b&0xFFF),float(cFound)}; return out;
}
inline CVector* __fastcall FakeCoorsLinked(void* pf,void*,CVector* out,uint32 a,bool* found) {
    trace.push_back({"find_node_coors_linked",Name(pf),Name(out),a,Name(found)});
    *found=Bit(mFound,cFound); *out=CVector{float(a&0xFFF),float(a>>20),float(cFound)*2.f}; return out;
}
inline uint32* __fastcall FakeNth(void* pf,void*,uint32* out,float x,float y,float z,uint32 type,float maxd,uint32 lowTraffic,uint32 unk,int32 nth,uint32 boats,uint32 interior,void* outNode) {
    trace.push_back({"find_nth_node",Name(pf),Name(out),F(x),F(y),F(z),type&0xFF,F(maxd),lowTraffic&0xFF,unk&0xFF,nth,boats&0xFF,interior&0xFF,Name(outNode)});
    *out=0x10000u*uint32(nth+1)+uint32(nth*7); return out;
}
inline float __fastcall FakeOrient(void* pf,void*,uint32 addr) { trace.push_back({"node_orientation",Name(pf),addr}); return float(addr%720)*0.5f; }
// sombras: CShadowCamera y RW
inline void __fastcall FakeCamDestroy(void* c,void*) { trace.push_back({"shadow_camera_destroy",Name(c)}); }
inline void* __fastcall FakeCamCreate(void* c,void*,int32 size) { trace.push_back({"shadow_camera_create",Name(c),size}); return Bit(mCam,cCam) ? (void*)camB.data() : nullptr; }
inline void __fastcall FakeCamSetLight(void* c,void*,void* l) { trace.push_back({"shadow_camera_set_light",Name(c),Name(l)}); }
inline void __fastcall FakeCamSetCenter(void* c,void*,const CVector* v) { trace.push_back({"shadow_camera_set_center",Name(c),Name(v),F(v->x),F(v->y),F(v->z)}); }
inline void* __fastcall FakeCamUpdateClump(void* c,void*,void* o) { trace.push_back({"shadow_camera_update_clump",Name(c),Name(o)}); return camB.data(); }
inline void* __fastcall FakeCamUpdateAtomic(void* c,void*,void* o) { trace.push_back({"shadow_camera_update_atomic",Name(c),Name(o)}); return camB.data(); }
inline void* __fastcall FakeResample(void* c,void*,void* r) { trace.push_back({"raster_resample",Name(c),Name(r)}); return rast2.data(); }
inline void* __fastcall FakeBlur(void* c,void*,void* r,int32 n) { trace.push_back({"raster_blur",Name(c),Name(r),n}); return rast3.data(); }
inline void* __fastcall FakeBlurry2(void* c,void*,void* r) { trace.push_back({"draw_blurry_raster2",Name(c),Name(r)}); return r; }
inline void* __fastcall FakeOutline(void* c,void*,const uint32* color) { trace.push_back({"draw_outline_border",Name(c),*color}); return rast1.data(); }
inline void* __cdecl FakeLightCreate(int32 type) { trace.push_back({"rp_light_create",type}); return Bit(mLight,cLight) ? (void*)light.data() : nullptr; }
inline void* __cdecl FakeLightSetColor(void* l,const uint32* c) { trace.push_back({"rp_light_set_color",Name(l),c[0],c[1],c[2],c[3]}); return l; }
inline void* __cdecl FakeFrameCreate() { trace.push_back({"rw_frame_create"}); return frameB.data(); }
inline void __cdecl FakeSetFrame(void* o,void* f) { trace.push_back({"rw_object_set_frame",Name(o),Name(f)}); At<void*>(o,4)=f; }
inline int __cdecl FakeFrameDestroy(void* f) { trace.push_back({"rw_frame_destroy",Name(f)}); return 1; }
inline int __cdecl FakeLightDestroy(void* l) { trace.push_back({"rp_light_destroy",Name(l)}); return 1; }
inline void* __cdecl FakeFrameRotate(void* f,const uint32* axis,float angle,int32 comb) { trace.push_back({"rw_frame_rotate",Name(f),axis[0],axis[1],axis[2],F(angle),comb}); return f; }
inline float* __cdecl FakeTransformPoints(float* out,const float* in,int32 n,const float* m) {
    trace.push_back({"rw_v3d_transform_points",Name(out),Name(in),n,Name(m),F(in[0]),F(in[1]),F(in[2])});
    out[0]=in[0]+m[12]; out[1]=in[1]+m[13]; out[2]=in[2]+m[14]; return out;
}

inline std::vector<bool> TPOccupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline std::vector<bool> EvOccupancy() { std::vector<bool> r(EvCap()); for(size_t k=0;k<r.size();++k)r[k]=!(EvStates()[k]&0x80); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; uintptr_t p=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CEventKnockOffBike","AffectsPed",0,0x85B2E0,5,512},
    {"CEventKnockOffBike","CalcForcesAndAnims",0x4B0020,0,-1,1024},
    {"CEventKnockOffBike","Constructor-first",0x4AFCF0,0,-1,256},
    {"CEventKnockOffBike","Constructor-second",0x4AFC70,0,-1,128},
    {"CEventKnockOffBike","From",0x4AFDD0,0,-1,256},
    {"CEventKnockOffBike","ReportCriminalEvent",0,0x85B2E0,8,256},
    {"CEventKnockOffBike","SetPedOutCar",0x4AFF60,0,-1,256},
    {"CEventKnockOffBike","SetPedSafePosition",0x4B4AC0,0,-1,1024},
    {"CEventScriptCommand","AffectsPed",0,0x85B378,5,256},
    {"CEventScriptCommand","Clone",0,0x85B378,4,256},
    {"CEventScriptCommand","CloneScriptTask",0,0x85B378,16,128},
    {"CEventScriptCommand","Constructor",0x4B0A00,0,-1,128},
    {"CEventScriptCommand","GetEventPriority",0,0x85B378,2,512},
    {"CEventScriptCommand","GetEventType",0,0x85B378,1,64},
    {"CEventScriptCommand","IsValid",0,0x85B378,14,256},
    {"CEventScriptCommand","TakesPriorityOver",0,0x85B378,11,512},
    {"CPedScriptedTaskRecordData","AssociateWithEvent",0x608500,0,-1,128},
    {"CPedScriptedTaskRecordData","AssociateWithTask",0x608520,0,-1,128},
    {"CPedScriptedTaskRecordData","Constructor",0x608330,0,-1,64},
    {"CPedScriptedTaskRecordData","Flush",0x608350,0,-1,256},
    {"CPedScriptedTaskRecordData","Set-CEventScriptCommand",0x608390,0,-1,256},
    {"CPedScriptedTaskRecordData","Set-CTask",0x6083E0,0,-1,256},
    {"CPedScriptedTaskRecordData","SetAsAttractorScriptTask",0x608440,0,-1,256},
    {"CPedScriptedTaskRecordData","SetAsGroupTask",0x6084A0,0,-1,256},
    {"CRealTimeShadow","Constructor",0x706410,0,-1,64},
    {"CRealTimeShadow","Create",0x706460,0,-1,512},
    {"CRealTimeShadow","Destroy",0x705990,0,-1,256},
    {"CRealTimeShadow","Destructor",0x706440,0,-1,256},
    {"CRealTimeShadow","DrawBorderAroundTexture",0x705A00,0,-1,128},
    {"CRealTimeShadow","GetShadowRwTexture",0x7059F0,0,-1,128},
    {"CRealTimeShadow","SetLightProperties",0x705900,0,-1,256},
    {"CRealTimeShadow","Update",0x706600,0,-1,512},
    {"CRect","GetCenter-",0x43E050,0,-1,256},
    {"CRect","IsFlipped",0x404190,0,-1,512},
    {"CRect","IsPointInside-",0x404290,0,-1,1024},
    {"CRect","IsPointInside-Tolerance",0x4042D0,0,-1,1024},
    {"CRect","Resize",0x404260,0,-1,256},
    {"CRect","Restrict",0x404200,0,-1,512},
    {"CRect","SetFromCenter",0x43E020,0,-1,256},
    {"CRect","StretchToPoint",0x5327F0,0,-1,512},
    {"CStuckCarCheck","AddCarToCheck",0x465970,0,-1,512},
    {"CStuckCarCheck","AttemptToWarpVehicle",0x463A60,0,-1,512},
    {"CStuckCarCheck","ClearStuckFlagForCar",0x463C40,0,-1,256},
    {"CStuckCarCheck","HasCarBeenStuckForAWhile",0x463C00,0,-1,256},
    {"CStuckCarCheck","Init",0x4639E0,0,-1,64},
    {"CStuckCarCheck","IsCarInStuckCarArray",0x463C70,0,-1,256},
    {"CStuckCarCheck","Process",0x465680,0,-1,1024},
    {"CStuckCarCheck","RemoveCarFromCheck",0x463B80,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"data8b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"data8b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("data8b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"data8b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    InitBufs();
    FillTraps(std::make_integer_sequence<int,80>{});
    taskVtbl[1]=reinterpret_cast<void*>(&FClone); taskVtbl[4]=reinterpret_cast<void*>(&FType);
    eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    pedVtbl[13]=reinterpret_cast<void*>(&FTestCol);
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{VEHPOOL,4},{IGNORE_ENT,4},{MIRROR,1},{MODELPTR400,4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto restoreEvents=[&]{ std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size());
                            std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size()); };
    const auto evOccStart=EvOccupancy(), tpOccStart=TPOccupancy();
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch r2(bit_cast<uintptr_t>(static_cast<float(*)(float,float)>(&CGeneral::GetRandomNumberInRange<float>)),FakeRandRange), r2e(0x41BD90,FakeRandRange);
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FakeFindPlayerWanted), w1e(0x56E230,FakeFindPlayerWanted);
        EntryPatch w2(bit_cast<uintptr_t>(&CWanted::RegisterCrime),FakeRegisterCrime), w2e(0x562410,FakeRegisterCrime);
        EntryPatch f1(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(float,float,float)>(&CPhysical::ApplyMoveForce)),FakeMoveForce), f1e(0x4ABBA0,FakeMoveForce);
        EntryPatch f2(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeMoveForce), f2e(0x5429F0,FakeMoveForce);
        EntryPatch a1(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),FakeLimitRadian), a1e(0x53CB50,FakeLimitRadian);
        EntryPatch a2(bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly),FakeRotZ), a2e(0x59B020,FakeRotZ);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetPedPositionInCar),FakePosInCar), p1e(0x5DF910,FakePosInCar);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::PositionPedOutOfCollision),FakePosOut), p2e(0x5E0820,FakePosOut);
        EntryPatch l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLos), l1e(0x56A490,FakeLos);
        EntryPatch t1(OurVirtual("CTaskSimpleCarSetPedOut","ProcessPed"),FakeProcessPed), t1e(0x647D10,FakeProcessPed);   // la entrada de la DLL es un envoltorio que llama al EXE
        EntryPatch t2(bit_cast<uintptr_t>(&CCarEnterExit::RemoveCarSitAnim),FakeRemoveSit), t2e(0x64F680,FakeRemoveSit);
        EntryPatch c1e(0x420C40,FakeSphereVis);    // IsSphereVisible de 3 args: envoltorio en la DLL
        EntryPatch c2(bit_cast<uintptr_t>(&CWorld::FindMissionEntitiesIntersectingCube),FakeFindMission), c2e(0x569240,FakeFindMission);
        EntryPatch c3e(0x4342A0,FakeSetCoords);    // SetCoordsOfScriptCar: envoltorio en la DLL
        EntryPatch c4(bit_cast<uintptr_t>(static_cast<bool(CUpsideDownCarCheck::*)(const CVehicle*)>(&CUpsideDownCarCheck::IsCarUpsideDown)),FakeUpside), c4e(0x463830,FakeUpside);
        EntryPatch c5(bit_cast<uintptr_t>(static_cast<CVector(CEntity::*)() const>(&CEntity::GetBoundCentre)),FakeBoundCentre), c5e(0x534250,FakeBoundCentre);
        EntryPatch n1(bit_cast<uintptr_t>(static_cast<CVector(CPathFind::*)(CNodeAddress,CNodeAddress,float&,bool*)>(&CPathFind::FindNodeCoorsForScript)),FakeCoorsTwo), n1e(0x450780,FakeCoorsTwo);
        EntryPatch n2(bit_cast<uintptr_t>(static_cast<CVector(CPathFind::*)(CNodeAddress,bool*)>(&CPathFind::FindNodeCoorsForScript)),FakeCoorsLinked), n2e(0x4505E0,FakeCoorsLinked);
        EntryPatch n3(bit_cast<uintptr_t>(&CPathFind::FindNthNodeClosestToCoors),FakeNth), n3e(0x44F8C0,FakeNth);
        EntryPatch n4e(0x450320,FakeOrient);       // FindNodeOrientationForCarPlacement: envoltorio en la DLL
        EntryPatch s1(bit_cast<uintptr_t>(&CShadowCamera::Destroy),FakeCamDestroy), s1e(0x705400,FakeCamDestroy);
        EntryPatch s2(bit_cast<uintptr_t>(&CShadowCamera::Create),FakeCamCreate), s2e(0x705B60,FakeCamCreate);
        EntryPatch s3(bit_cast<uintptr_t>(&CShadowCamera::SetLight),FakeCamSetLight), s3e(0x705520,FakeCamSetLight);
        EntryPatch s4(bit_cast<uintptr_t>(&CShadowCamera::SetCenter),FakeCamSetCenter), s4e(0x705590,FakeCamSetCenter);
        EntryPatch s5(bit_cast<uintptr_t>(static_cast<RwCamera*(CShadowCamera::*)(RpClump*)>(&CShadowCamera::Update)),FakeCamUpdateClump), s5e(0x705BF0,FakeCamUpdateClump);
        EntryPatch s6(bit_cast<uintptr_t>(static_cast<RwCamera*(CShadowCamera::*)(RpAtomic*)>(&CShadowCamera::Update)),FakeCamUpdateAtomic), s6e(0x705C80,FakeCamUpdateAtomic);
        EntryPatch s7(bit_cast<uintptr_t>(&CShadowCamera::RasterResample),FakeResample), s7e(0x706070,FakeResample);
        EntryPatch s8(bit_cast<uintptr_t>(&CShadowCamera::RasterBlur),FakeBlur), s8e(0x706170,FakeBlur);
        EntryPatch s9(bit_cast<uintptr_t>(&CShadowCamera::DrawBlurryRaster2),FakeBlurry2), s9e(0x706330,FakeBlurry2);
        EntryPatch s10(bit_cast<uintptr_t>(&CShadowCamera::DrawOutlineBorder),FakeOutline), s10e(0x705790,FakeOutline);
        // RW: los simbolos de la DLL son envoltorios que llaman al EXE
        EntryPatch w3e(0x752110,FakeLightCreate), w4e(0x751A90,FakeLightSetColor), w5e(0x7F0410,FakeFrameCreate), w6e(0x804EF0,FakeSetFrame),
                   w7e(0x7F05A0,FakeFrameDestroy), w8e(0x7520D0,FakeLightDestroy), w9e(0x7F1010,FakeFrameRotate), w10e(0x7EDD90,FakeTransformPoints);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isCtor=nm.rfind("Constructor",0)==0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l){return *(l.begin()+next()%l.size());};
                // float aleatorio sin NaN de senalizacion (el x87 los silenciaria al copiarlos)
                auto rndF=[&]{ uint32 v=next(); if((v&0x7F800000u)==0x7F800000u)v|=0x00400000u; return v; };
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8B8Bu; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    cVis=cFind=cUpside=cFound=cCol=cLos=cPosOut=cCam=cClone=cLight=cRange=0;
                    CleanRefs(); restoreGlobals(); restoreEvents();
                    for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    const auto evOcc=EvOccupancy(), tpOcc=TPOccupancy();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    const uintptr_t P0=uintptr_t(Ped(0)), P1=uintptr_t(Ped(1)), P2=uintptr_t(Ped(2)), P3=uintptr_t(Ped(3)), V0=uintptr_t(Veh(0)), V1=uintptr_t(Veh(1)), V2=uintptr_t(Veh(2));
                    // peatones: matriz siempre presente salvo en ped#5
                    const CVector center=vec(30.f);
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_STATUS]=3;
                        const CVector pos=center+vec(10.f);
                        if(n!=5){ auto* m=pedMats[n].data(); At<void*>(p,E_MATRIX)=m; for(size_t k=0;k<0x30;k+=4)At<float>(m,k)=uni(-1.f,1.f); At<CVector>(m,0x30)=pos; }
                        else { At<CVector>(p,E_POS)=pos; At<float>(p,0x10)=uni(-3.f,3.f); }
                        At<uint32>(p,P_TYPE)=uint32(pick({6,6,0,1,4,7,20}));
                        At<uint32>(p,P_STATE)=uint32(pick({1,1,1,0x36,0x37,0,7}));
                        At<uint32>(p,P_FLAGS1)=next(); At<uint32>(p,P_FLAGS2)=next(); At<uint32>(p,P_FLAGS3)=next();
                        At<float>(p,PH_MASS)=uni(40.f,150.f); At<CVector>(p,PH_MOVE)=vec(0.5f);
                        At<float>(p,P_HEALTH)=std::bit_cast<float>(uint32(pick({F(100.f),F(100.f),F(0.f),F(-1.f),F(-0.f),0x7FC00000u,F(1.f)})));
                    }
                    // vehiculos: matriz siempre presente salvo en veh#2
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_STATUS]=uint8(2|(pick({4,4,12,0,5,12})<<3));
                        if(n!=2){ auto* m=vehMats[n].data(); At<void*>(v,E_MATRIX)=m; for(size_t k=0;k<0x30;k+=4)At<float>(m,k)=uni(-1.f,1.f); At<CVector>(m,0x30)=center+vec(10.f); }
                        else { At<CVector>(v,E_POS)=center+vec(10.f); At<float>(v,0x10)=uni(-3.f,3.f); }
                        At<uint32>(v,V_TYPE)=uint32(pick({0,9,9,0,5,3}));
                        At<uint32>(v,V_SUBTYPE)=uint32(pick({10,9,0,10,5}));
                        At<uintptr_t>(v,V_DRIVER)=next()%3 ? pick({P0,P1,P2,P3}) : 0;
                        for(int k=0;k<8;++k)At<uintptr_t>(v,V_PASSENGERS+4*k)=next()%3==0 ? pick({P0,P1,P2}) : 0;
                        At<float>(v,PH_MASS)=uni(100.f,2000.f); At<CVector>(v,PH_MOVE)=vec(0.5f);
                        v[V_WHEELS_AUTO]=uint8(next()%5); v[V_WHEELS_BIKE]=uint8(next()%3);
                        At<float>(v,V_BIKE_LEAN)=uni(-1.f,1.f); v[V_BIKE_LEANMAT]=uint8(next());
                    }
                    // tareas y eventos falsos
                    static const int32 ttypes[]{0x191,0x191,0xD4,0x518,0x640,0xD9,0xCB,0xC8,0x190};
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[n])b=uint8(next()); At<void*>(fakes[n].data(),0)=taskVtbl.data(); At<void*>(fakes[n].data(),4)=nullptr;
                        At<int32>(fakes[n].data(),FK_TYPE)=ttypes[next()%std::size(ttypes)]; }
                    for(auto& e:evs){ for(auto& b:e)b=uint8(next()); At<void*>(e.data(),0)=eventVtbl.data(); }
                    std::memset(refEv.data(),0,refEv.size()); std::memset(wanted.data(),0,wanted.size());
                    wantedRet=uintptr_t(wanted.data());
                    mVis=next(); mFind=next(); mUpside=next(); mFound=next(); mCol=next(); mLos=next(); mPosOut=next(); mCam=next(); mClone=next(); mLight=next();
                    void* const o=object.data();
                    json out=json::object(); json ret=nullptr; uintptr_t rawRet=0; bool retIsPtr=false;
                    auto callTarget=[&](auto&& body){
                        for(auto* h:hooks)h->State(candidate);
                        volatile uint8 marker{}; stackHi=uintptr_t(&marker)+0x100; stackLo=stackHi-0x20000;
                        body(fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address);
                        for(auto* h:hooks)h->State(false);
                    };
                    bool switches{};
                    { for(auto* h:hooks)h->State(candidate);
                      const uintptr_t t=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                      switches=fn.slot>=0 ? ((t>=0x400000&&t<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                      for(auto* h:hooks)h->State(false); }

                    // ---------------------------------------------------------------- CEventKnockOffBike
                    if(cls=="CEventKnockOffBike") {
                        CPed* const p=Ped(0);
                        if(isCtor){
                            std::memset(o,0xCD,0x40);
                            if(nm=="Constructor-first"){
                                void* veh=reinterpret_cast<void*>(pick({V0,V1,0,V2})); auto* ms=src.data(); auto* iv=src.data()+0x10;
                                for(size_t k=0;k<0x20;k+=4)At<uint32>(src.data(),k)=rndF();
                                const uint32 dmg=rndF(), a6=rndF(), type=(next()&~0xFFu)|uint32(pick({0,49,53,54,55,0x80})), dir=(next()&~0xFFu)|uint32(next()%5);
                                const int32 tm=int32(next()); void* ped=reinterpret_cast<void*>(pick({P1,P2,0,P0}));
                                const uint32 vic=(next()&~0xFFu)|(next()%2), frc=(next()&~0xFFu)|(next()%2);
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*,void*,uint32,uint32,uint32,uint32,int32,void*,uint32,uint32)>(t)(o,veh,ms,iv,dmg,a6,type,dir,tm,ped,vic,frc)); });
                            } else {
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); });
                            }
                            retIsPtr=true;
                            json words=Region(o,0x3C,4); words.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType())); out["object"]=words;
                        } else {
                            for(size_t b=0;b<0x40;++b)object[b]=uint8(next());
                            At<uintptr_t>(o,0)=VT_KO; At<int32>(o,4)=int32(next()%5)-2;
                            for(size_t k=0xC;k<0x2C;k+=4)At<uint32>(o,k)=rndF();
                            At<uintptr_t>(o,0x38)=pick({V0,V0,V1,V1,V2}); At<uintptr_t>(o,0x30)=pick({P1,P2,0,0,P3});
                            At<uint8>(o,0x34)=uint8(next()%8==0 ? pick({4,0xFF,0x80,7}) : next()%4);
                            At<uint8>(o,0x36)=uint8(next()%8==0 ? pick({1,50,0x80,0xFF,0x37}) : pick({0,49,53,54,55,49,54}));
                            At<uint8>(o,0x37)=uint8(next()%4==0 ? next() : pick({8,9,10,11}));
                            auto* veh=At<uint8*>(o,0x38);
                            if(nm=="AffectsPed"){
                                if(next()%2)At<uintptr_t>(o,0x38)=pick({V0,V1,0});
                                if(auto* v=At<uint8*>(o,0x38); v&&next()%2){ const int k=int(next()%4); At<uintptr_t>(v,k==0 ? V_DRIVER : V_PASSENGERS+4*(k-1))=P0; }
                                At<uint32>(p,P_FLAGS3)=(next()&~0x18000000u)|(uint32(next()%4)<<27);
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,p); });
                            } else if(nm=="ReportCriminalEvent"){
                                wantedRet=next()%4 ? uintptr_t(wanted.data()) : uintptr_t(wanted.data()+4);
                                callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(t)(o,p); });
                            } else if(nm=="From"){
                                for(auto& b:src)b=uint8(next()); for(size_t k=0xC;k<0x2C;k+=4)At<uint32>(src.data(),k)=rndF();
                                At<uintptr_t>(src.data(),0x38)=pick({V0,V1,0}); At<uintptr_t>(src.data(),0x30)=pick({P1,P2,0});
                                callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*)>(t)(o,src.data()); });
                                out["src"]=Region(src.data(),0x40);
                            } else if(nm=="SetPedOutCar"){
                                if(next()%2){ const int k=int(next()%4); At<uintptr_t>(veh,k==0 ? V_DRIVER : V_PASSENGERS+4*(k-1))=P0; }
                                callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(t)(o,p); });
                            } else if(nm=="CalcForcesAndAnims"){
                                At<uintptr_t>(o,0x38)=pick({V0,V1}); veh=At<uint8*>(o,0x38);
                                // velocidad del vehiculo alrededor de |v|^2 = 0.09 (con los limites exactos y NaN)
                                const uint32 m=next()%8;
                                if(m==0)At<CVector>(veh,PH_MOVE)=CVector{0.3f,0.f,0.f};
                                else if(m==1)At<CVector>(veh,PH_MOVE)=CVector{0.f,std::bit_cast<float>(0x3E99999Au),0.f};
                                else if(m==2)At<uint32>(veh,PH_MOVE+4*(next()%3))=0x7FC00000u;
                                else if(m==3)At<CVector>(veh,PH_MOVE)=vec(0.2f);
                                else At<CVector>(veh,PH_MOVE)=vec(0.4f);
                                if(next()%8==0)At<float>(veh,PH_MASS)=std::bit_cast<float>(uint32(pick({0u,F(-1.f),0x7FC00000u})));
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__thiscall*)(void*,CPed*)>(t)(o,p); });
                            } else { // SetPedSafePosition
                                At<uintptr_t>(o,0x38)=pick({V0,V1}); veh=At<uint8*>(o,0x38);
                                At<uint8>(o,0x35)=uint8(next());
                                auto* vm=At<uint8*>(veh,E_MATRIX);
                                At<uint32>(vm,0x28)=next()%3 ? F(uni(-1.f,1.f)) : uint32(pick({0u,0x80000000u,0x7FC00000u,F(-1.f),F(1.f)}));
                                if(next()%8==0){ At<float>(vm,0x10)=0.f; At<float>(vm,0x14)=next()%2 ? 0.f : -0.f; }
                                if(next()%2){ auto* sp=peds[1].data(); At<uintptr_t>(p,P_STANDING)=uintptr_t(sp); Reg(sp,peds[0].data()+P_STANDING); }
                                At<uintptr_t>(reinterpret_cast<void*>(IGNORE_ENT),0)=pick({0,P3});
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,p); });
                                out["ignore"]=Name(At<uintptr_t>(reinterpret_cast<void*>(IGNORE_ENT),0));
                                out["vehicle"]=Region(veh,0x700);
                            }
                            out["object"]=Region(o,0x40,4);
                            out["ped"]=Region(p,sizeof(CPed)); out["pmat"]=Region(pedMats[0].data(),0x48);
                        }
                        out["refs"]=AllRefs(); CleanRefs();
                    }
                    // ---------------------------------------------------------------- CEventScriptCommand
                    else if(cls=="CEventScriptCommand") {
                        CPed* const p=Ped(0);
                        if(isCtor){
                            std::memset(o,0xCD,0x40);
                            const int32 idx=int32(next()%6)-1; void* task=reinterpret_cast<void*>(pick({uintptr_t(Fake(1)),0,uintptr_t(Fake(2))})); const uint32 dead=(next()&~0xFFu)|(next()%2);
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,void*,uint32)>(t)(o,idx,task,dead)); });
                            retIsPtr=true;
                            json words=Region(o,0x18,4); words.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType())); out["object"]=words;
                        } else {
                            for(size_t b=0;b<0x40;++b)object[b]=uint8(next());
                            At<uintptr_t>(o,0)=VT_SC; At<int32>(o,4)=int32(next()%6)-2; At<uint8>(o,8)=uint8(next()%3==0);
                            At<int32>(o,0xC)=int32(next()%5)-1; At<uintptr_t>(o,0x10)=pick({uintptr_t(Fake(1)),uintptr_t(Fake(2)),uintptr_t(Fake(3)),0});
                            At<uint8>(o,0x14)=uint8(next()%3==0);
                            if(nm=="AffectsPed"){ callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,p); }); }
                            else if(nm=="IsValid"){ CPed* q=next()%2 ? p : nullptr; callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,q); }); }
                            else if(nm=="GetEventType"){ callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(t)(o); }); }
                            else if(nm=="GetEventPriority"){ callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(t)(o); }); }
                            else if(nm=="TakesPriorityOver"){
                                auto* r=refEv.data(); At<void*>(r,0)=eventVtbl.data(); At<int32>(r,FK_TYPE)=int32(pick({9,10,10,9,0x20,3,11}));
                                At<int32>(r,FK_PRIO)=int32(next()%3==0 ? pick({53,71,75}) : 40+next()%45);
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(t)(o,r); });
                            } else if(nm=="CloneScriptTask"){ callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true; }
                            else { callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true; }
                            out["object"]=Region(o,0x18,4);
                        }
                        // clon nuevo del pool de eventos: tipo + bytes; despues se libera
                        json clone=nullptr; void* fresh=nullptr;
                        { const auto now2=EvOccupancy(); for(size_t k=0;k<now2.size();++k)if(now2[k]&&!evOcc[k]){ fresh=EvStorage()+k*EVENT_SLOT; break; } }
                        if(fresh)clone={{"at",Name(fresh)},{"type",int(reinterpret_cast<CEvent*>(fresh)->GetEventType())},{"words",Region(fresh,EVENT_SLOT,4)}};
                        out["clone"]=clone;
                        if(fresh)GetEventPool()->Delete(static_cast<CEvent*>(fresh));
                    }
                    // ---------------------------------------------------------------- CPedScriptedTaskRecordData
                    else if(cls=="CPedScriptedTaskRecordData") {
                        if(isCtor)std::memset(o,0xCD,0x20);
                        else {
                            for(size_t b=0;b<0x20;++b)object[b]=uint8(next());
                            At<int32>(o,0)=int32(next()%3 ? 0x600+next()%8 : uint32(-1));
                            At<uintptr_t>(o,4)=pick({uintptr_t(evs[0].data()),0,uintptr_t(evs[1].data())});
                            At<uintptr_t>(o,8)=pick({uintptr_t(Fake(1)),0,uintptr_t(Fake(2))});
                            At<uintptr_t>(o,0xC)=pick({P1,0,P2,P0}); Reg(At<void*>(o,0xC),static_cast<uint8*>(o)+0xC);
                            At<int32>(o,0x10)=int32(next()%8)-1;
                        }
                        void* const ped=reinterpret_cast<void*>(pick({P0,P1,P3}));
                        const int32 op=int32(next()%3 ? 0x600+next()%16 : next());
                        void* const ev=reinterpret_cast<void*>(pick({uintptr_t(evs[0].data()),uintptr_t(evs[1].data()),0}));
                        void* const task=reinterpret_cast<void*>(pick({uintptr_t(Fake(1)),uintptr_t(Fake(3)),0}));
                        if(isCtor){ callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true; }
                        else if(nm=="AssociateWithEvent")callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*)>(t)(o,ev); });
                        else if(nm=="AssociateWithTask")callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*)>(t)(o,task); });
                        else if(nm=="Flush")callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                        else if(nm=="Set-CEventScriptCommand")callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*,int32,void*)>(t)(o,ped,op,ev); });
                        else callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*,int32,void*)>(t)(o,ped,op,task); });
                        out["object"]=Region(o,0x20); out["refs"]=AllRefs(); CleanRefs();
                    }
                    // ---------------------------------------------------------------- CRealTimeShadow
                    else if(cls=="CRealTimeShadow") {
                        // objetos RW falsos: luz (+4 marco), objeto RW del dueno (tipo en +0, marco en +4), camaras (+0x60 raster)
                        for(auto* b:{&light,&rwobj})for(auto& x:*b)x=uint8(next());
                        for(auto* b:{&frameA,&frameB})for(size_t k=0;k<b->size();k+=4)At<uint32>(b->data(),k)=rndF();
                        std::memset(camA.data(),0,camA.size()); std::memset(camB.data(),0,camB.size());
                        At<void*>(camA.data(),0x60)=rast0.data(); At<void*>(camB.data(),0x60)=rast1.data();
                        At<void*>(light.data(),4)=next()%5 ? (void*)frameA.data() : nullptr;
                        if(isCtor)std::memset(o,0xCD,0x50);
                        else {
                            for(size_t b=0;b<0x50;++b)object[b]=uint8(next());
                            for(size_t k=0x2C;k<0x4C;k+=4)At<uint32>(o,k)=rndF();
                            At<uintptr_t>(o,0)=pick({uintptr_t(Ped(4)),uintptr_t(Ped(4)),0}); At<uint8>(o,4)=uint8(next()%2); At<uint8>(o,5)=uint8(next());
                            At<uintptr_t>(o,8)=uintptr_t(camA.data()); At<uintptr_t>(o,0xC)=uintptr_t(texA.data());
                            At<uintptr_t>(o,0x14)=next()%2 ? uintptr_t(camB.data()) : 0; At<uintptr_t>(o,0x18)=next()%2 ? uintptr_t(texB.data()) : 0;
                            At<uint8>(o,0x10)=uint8(next()%2); At<uint32>(o,0x1C)=uint32(pick({0,0,1,3,0x80000000u}));
                            At<uint8>(o,0x20)=uint8(next()%2); At<int32>(o,0x24)=int32(pick({1,2,1,2,0,3,uint32(-1)}));
                            At<uintptr_t>(o,0x28)=next()%4 ? uintptr_t(light.data()) : 0;
                        }
                        if(nm=="Constructor"){ callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true; }
                        else if(nm=="Destroy"||nm=="Destructor"){
                            // el EXE destruye siempre el marco de la luz: con luz, marco presente
                            if(At<uintptr_t>(o,0x28))At<void*>(light.data(),4)=frameA.data();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                        } else if(nm=="Create"){
                            const uint32 blurred=(next()&~0xFFu)|(next()%2), more=(next()&~0xFFu)|(next()%2); const int32 passes=int32(pick({0,1,3,5,0x80000000u}));
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,int32,uint32)>(t)(o,blurred,passes,more); });
                        } else if(nm=="DrawBorderAroundTexture"){
                            At<uint32>(cvec.data(),0)=next();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,const void*)>(t)(o,cvec.data()); });
                        } else if(nm=="GetShadowRwTexture"){
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true;
                        } else if(nm=="SetLightProperties"){
                            At<uintptr_t>(o,0x28)=uintptr_t(light.data());
                            const uint32 az=next()%4 ? F(uni(-360.f,360.f)) : uint32(pick({0u,0x80000000u,0x7FC00000u,F(90.f)})), el=rndF(), doSet=(next()&~0xFFu)|(next()%2);
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,uint32,uint32,uint32)>(t)(o,az,el,doSet)); }); retIsPtr=true;
                        } else { // Update: dueno con objeto RW del tipo indicado y raster de la camara presente
                            auto* owner=peds[4].data(); At<uintptr_t>(o,0)=uintptr_t(owner); At<void*>(owner,E_RWOBJ)=rwobj.data();
                            const int32 type=At<int32>(o,0x24); rwobj[0]=uint8(type==1||type==2 ? type : 1+next()%2); At<void*>(rwobj.data(),4)=frameA.data();
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true;
                        }
                        out["object"]=Region(o,0x50); out["light"]=Region(light.data(),0x10); out["rwobj"]=Region(rwobj.data(),0x10);
                    }
                    // ---------------------------------------------------------------- CRect
                    else if(cls=="CRect") {
                        // valores: normales, iguales a otro borde, ceros con signo, infinitos y un solo NaN silencioso (0x7FC00000;
                        // con dos NaN distintos el x87 y el SSE pueden propagar uno u otro)
                        static const uint32 sp[]{0u,0x80000000u,0x7FC00000u,0x7FC00000u,0x7F800000u,0xFF800000u,0x49742400u,0xC9742400u,0x3F800000u};
                        std::array<uint32,4> r{};
                        for(auto& x:r)x=next()%5 ? F(uni(-100.f,100.f)) : sp[next()%std::size(sp)];
                        if(next()%4==0)r[next()%4]=r[next()%4];
                        if(next()%3==0){ if(std::bit_cast<float>(r[0])>std::bit_cast<float>(r[2]))std::swap(r[0],r[2]); if(std::bit_cast<float>(r[3])>std::bit_cast<float>(r[1]))std::swap(r[1],r[3]); }
                        for(size_t b=0;b<0x20;++b)object[b]=uint8(next());
                        for(int k=0;k<4;++k)At<uint32>(o,4*k)=r[k];
                        auto coord=[&](bool xAxis)->uint32{ const uint32 m=next()%6;
                            if(m<2)return xAxis ? r[m==0 ? 0 : 2] : r[m==0 ? 1 : 3];
                            if(m==2)return sp[next()%std::size(sp)];
                            return F(uni(-120.f,120.f)); };
                        if(nm=="IsFlipped"){ callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(t)(o); }); }
                        else if(nm=="Restrict"){ for(int k=0;k<4;++k)At<uint32>(pt.data(),4*k)=next()%3 ? coord(k%2==0) : r[k];
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,const void*)>(t)(o,pt.data()); }); }
                        else if(nm=="Resize"){ const uint32 x=next()%4 ? F(uni(-10.f,10.f)) : sp[next()%std::size(sp)], y=next()%4 ? F(uni(-10.f,10.f)) : sp[next()%std::size(sp)];
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(t)(o,x,y); }); }
                        else if(nm=="IsPointInside-"){ At<uint32>(pt.data(),0)=coord(true); At<uint32>(pt.data(),4)=coord(false);
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,const void*)>(t)(o,pt.data()); }); }
                        else if(nm=="IsPointInside-Tolerance"){ At<uint32>(pt.data(),0)=coord(true); At<uint32>(pt.data(),4)=coord(false);
                            const uint32 tol=next()%4 ? F(uni(-5.f,5.f)) : sp[next()%std::size(sp)];
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,const void*,uint32)>(t)(o,pt.data(),tol); }); }
                        else if(nm=="SetFromCenter"){ const uint32 x=coord(true), y=coord(false), sz=next()%4 ? F(uni(-10.f,50.f)) : sp[next()%std::size(sp)];
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,uint32)>(t)(o,x,y,sz); }); }
                        else if(nm=="GetCenter-"){ At<uint32>(pt.data(),0)=next(); At<uint32>(pt.data(),4)=next();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(t)(o,pt.data(),pt.data()+4); }); }
                        else { const uint32 x=coord(true), y=coord(false);
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(t)(o,x,y); }); }
                        out["object"]=Region(o,0x20); out["pt"]=Region(pt.data(),0x10);
                    }
                    // ---------------------------------------------------------------- CStuckCarCheck
                    else {
                        // pool falso: estados [-1..15] (el byte de guarda es poolStates[7]), huecos 0xA18
                        auto* states=poolStates.data()+8; std::memset(poolStates.data(),0x80,poolStates.size());
                        for(int k=0;k<NPOOL;++k)states[k]=uint8(next()%4==0 ? 0x80|next() : next()&0x7F);
                        states[-1]=uint8(next()%4==0 ? 0xFF : next()%2 ? 0x80 : 0x3A);
                        std::memset(poolHdr.data(),0,poolHdr.size()); At<void*>(poolHdr.data(),0)=PoolVeh(1); At<void*>(poolHdr.data(),4)=states; At<int32>(poolHdr.data(),8)=NPOOL; At<int32>(poolHdr.data(),0xC)=-1;
                        // modelo falso 400: +0x14 colision con el radio en +0x24
                        for(auto* b:{&mi,&col})for(auto& x:*b)x=uint8(next()); At<void*>(mi.data(),0x14)=col.data(); At<uint32>(col.data(),0x24)=F(uni(0.5f,8.f));
                        At<void*>(reinterpret_cast<void*>(MODELPTR400),0)=mi.data();
                        At<uint8>(reinterpret_cast<void*>(MIRROR),0)=uint8(next()%3==0);
                        for(int k=0;k<=NPOOL;++k){ auto* v=PoolVeh(k); std::memset(v,0,POOL_SLOT); std::memset(poolMats[k].data(),0,poolMats[k].size());
                            At<void*>(v,0)=vehVtbl.data(); v[E_STATUS]=2; At<int16>(v,E_MODEL)=400;
                            if(next()%4){ At<void*>(v,E_MATRIX)=poolMats[k].data(); At<CVector>(poolMats[k].data(),0x30)=center+vec(20.f); }
                            else At<CVector>(v,E_POS)=center+vec(20.f);
                            At<uint32>(v,PH_FLAGS)=next(); At<uintptr_t>(v,V_DRIVER)=next()%4 ? P1 : 0;
                            At<uint32>(v,V_AUTOPILOT)=next(); At<uint32>(v,V_AUTOPILOT+4)=next(); }
                        auto handle=[&]()->int32{
                            const int32 idx=int32(next()%(NPOOL+1))-1; const uint32 m=next()%5;
                            const uint8 low=m<3 ? states[idx] : uint8(next());
                            return m==4 ? int32(pick({uint32(-1),0,uint32(-2),0x100})) : int32((uint32(idx)<<8)|low);
                        };
                        // 16 huecos de 0x24: manejador, ultima posicion, ultima comprobacion, radio, intervalo, 5 banderas, nodos
                        for(size_t b=0;b<0x240;++b)object[b]=uint8(next());
                        for(int k=0;k<16;++k){ auto* c=static_cast<uint8*>(o)+0x24*k;
                            At<int32>(c,0)=next()%4==0 ? int32(pick({uint32(-1),uint32(-1),uint32(-5)})) : handle();
                            At<CVector>(c,4)=center+vec(20.f);
                            At<uint32>(c,0x10)=now-uint32(pick({0,500,1000,1001,999,5000,0x80000000u})); At<uint32>(c,0x14)=next()%4 ? F(uni(0.f,6.f)) : uint32(pick({0u,0x7FC00000u,F(1e6f)}));
                            At<int32>(c,0x18)=int32(pick({1000,0,999,uint32(-1),3000}));
                            for(int b=0;b<5;++b)c[0x1C+b]=uint8(next()%2);
                            c[0x21]=uint8(int8(next()%9)-3);
                        }
                        // posiciones de los vehiculos del pool cerca de la ultima posicion de algun hueco
                        for(int k=1;k<=NPOOL;++k)if(next()%2){ const CVector lp=At<CVector>(static_cast<uint8*>(o)+0x24*(next()%16),4)+vec(1.f);
                            if(auto* m=At<uint8*>(PoolVeh(k),E_MATRIX))At<CVector>(m,0x30)=lp; else At<CVector>(PoolVeh(k),E_POS)=lp; }
                        At<void*>(reinterpret_cast<void*>(VEHPOOL),0)=poolHdr.data();
                        auto anyHandle=[&]()->int32{ return next()%2 ? At<int32>(static_cast<uint8*>(o)+0x24*(next()%16),0) : handle(); };
                        if(nm=="Init")callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                        else if(nm=="AddCarToCheck"){
                            if(next()%3==0)for(int k=0;k<16;++k)if(next()%2)At<int32>(static_cast<uint8*>(o)+0x24*k,0)=handle();
                            const int32 h=handle(); const uint32 rad=rndF(); const uint32 tm=next();
                            const uint32 b1=(next()&~0xFFu)|(next()%2), b2=(next()&~0xFFu)|(next()%2), b3=(next()&~0xFFu)|(next()%2), b4=(next()&~0xFFu)|(next()%2), nodes=next();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,int32,uint32,uint32,uint32,uint32,uint32,uint32,uint32)>(t)(o,h,rad,tm,b1,b2,b3,b4,nodes); });
                        } else if(nm=="AttemptToWarpVehicle"){
                            void* v=Veh(int(next()%NVEH)); At<CVector>(cvec.data(),0)=center+vec(50.f);
                            const uint32 ori=next()%4 ? F(uni(-360.f,720.f)) : uint32(pick({0u,0x80000000u,0x7FC00000u}));
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*,void*,uint32)>(t)(o,v,cvec.data(),ori); });
                            out["vehicles"]={Region(vehs[0].data(),0x20),Region(vehs[1].data(),0x20),Region(vehs[2].data(),0x20)};
                            out["cvec"]=Region(cvec.data(),0x10);
                        } else if(nm=="ClearStuckFlagForCar"||nm=="RemoveCarFromCheck"){ const int32 h=anyHandle();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,int32)>(t)(o,h); }); }
                        else if(nm=="HasCarBeenStuckForAWhile"||nm=="IsCarInStuckCarArray"){
                            if(next()%2){ const int a=int(next()%16), b=int(next()%16); At<int32>(static_cast<uint8*>(o)+0x24*b,0)=At<int32>(static_cast<uint8*>(o)+0x24*a,0); }
                            const int32 h=anyHandle(); callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(t)(o,h); }); }
                        else { // Process sobre el pool falso
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                            json pv=json::array(); for(int k=0;k<=NPOOL;++k)pv.push_back({Region(PoolVeh(k),0x20),Region(poolMats[k].data(),0x48)}); out["pool"]=pv;
                        }
                        restoreGlobals();
                        out["object"]=Region(o,0x240);
                    }
                    if(retIsPtr)ret=Name(rawRet);
                    out["ret"]=ret; out["trace"]=trace; out["switches"]=switches; out["rng"]=rngCalls;
                    out["clean"]=EvOccupancy()==evOcc&&TPOccupancy()==tpOcc;
                    CleanRefs(); restoreGlobals(); restoreEvents();
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
    CleanRefs(); restoreGlobals(); restoreEvents();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=EvOccupancy()==evOccStart&&TPOccupancy()==tpOccStart&&freeRefs()==savedFree&&globalsIntact()
        &&std::memcmp(EvHdr(),savedEvHdr.data(),savedEvHdr.size())==0;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
