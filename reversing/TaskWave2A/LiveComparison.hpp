#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1/TaskFinal2, partida cargada via FastLoader): deja COMPLETAS las clases
//   CTaskComplexKillCriminal (8): Constructor 0x68BE70, Destructor 0x68BF30, CreateSubTask 0x68C050, FindNextCriminalToKill 0x68C3C0,
//     ChangeTarget 0x68C6E0, Clone (vtable 0x870A00 hueco 1 -> 0x68CE50), MakeAbortable (hueco 6 -> 0x68DAD0), CreateNextSubTask (hueco 8 -> 0x68E4F0);
//   CTaskComplexKillPedOnFootArmed (8): Constructor 0x621190, Destructor 0x621250, IsPedInLeaderFiringLine 0x621300,
//     LineOfSightClearForAttack 0x621500, Clone (vtable 0x86D918 hueco 1 -> 0x6234C0), MakeAbortable (hueco 6 -> 0x6212B0),
//     CreateFirstSubTask (hueco 9 -> 0x62BF00), ControlSubTask (hueco 10 -> 0x62CCE0);
//   CTaskSimplePlayerOnFoot (4 de 10): Constructor 0x685750, Destructor 0x6857D0, Clone (vtable 0x8708EC hueco 1 -> 0x68AFF0), MakeAbortable (hueco 6 -> 0x6857E0).
// Fuera del banco (CTaskSimplePlayerOnFoot): ProcessPed 0x688810, ProcessPlayerWeapon 0x6859A0, PlayIdleAnimations 0x6872C0,
// PlayerControlZeldaWeapon 0x687C20, PlayerControlDucked 0x687F30 y PlayerControlZelda 0x6883D0 (ver README.md).
// Original (hooks de la clase desactivados) frente a candidato con fixtures identicos y deterministas. Peatones, vehiculos, inteligencia,
// grupo, datos de jugador, tareas falsas (vtable trazada), eventos falsos (vtable trazada) y objetos de tarea viven en buffers propios con
// desplazamientos crudos del EXE. Colaboradores simulados y trazados (mismo stub en la direccion del EXE y en el simbolo de la DLL, solo en
// el hilo del banco): rand 0x821B1E y CGeneral::GetRandomNumber, CPed::SetCurrentWeapon(tipo) 0x5E6280, SetPedDefaultDecisionMaker 0x5E06E0,
// CCarCtrl::JoinCarWithRoadSystem 0x42F5A0, CPed::Say 0x5EFFE0, CPedGroups::GetPedsGroup 0x5F7E80, CWorld::GetIsLineOfSightClear 0x56A490,
// CPed::GetTransformedBonePosition 0x5E01C0, CCover::FindAndReserveCoverPoint 0x6992B0 / FindCoordinatesCoverPoint 0x699570,
// CPed::ReleaseCoverPoint 0x5E0270, CPedIntelligence::SetTaskDuckSecondary 0x601230, CTaskSimpleDuck::SetDuckTimer 0x692530 / ControlDuckMove 0x6923F0,
// CTaskSimpleUseGun::ControlGunMove 0x61E0C0, CTaskComplexKillPedOnFootArmed::CreateSubTask 0x626FC0 (validada aparte; devuelve una tarea falsa marcada),
// CCamera::Using1stPersonWeaponMode 0x50BFF0 / ClearPlayerWeaponMode 0x50AB10, CWeaponEffects::ClearCrossHair 0x742C60, CAnimManager::BlendAnimation 0x4D4610.
// Se ejecutan de verdad: IsPlayer, RegisterReference/CleanUpOldReference, CCopPed::AddCriminalToKill/SetPartner, CPedIntelligence::FindTaskByType/GetTaskDuck/
// GetTaskUseGun, CTaskManager::GetTaskSecondary, CPedGroupMembership::GetLeader, CWeapon::IsTypeMelee, CWeaponInfo, CVector::Normalise, CTimer, el pool de tareas
// (las tareas nuevas se detectan en el pool y se borran) y los constructores/destructores de las subtareas creadas.
// Globales tocados temporalmente y restaurados: CGame::currArea (0xB72914) y CLoadMonitor::m_eProcLevel (0xB7298C) para LineOfSightClearForAttack.
// Se compara: retorno, memoria completa del objeto probado (y del segundo objeto), tareas nuevas del pool (tipo + palabras sin vptr), memoria de las
// tareas falsas, huella de todos los buffers (referencias enmascaradas), listas de referencias antes y despues del borrado, traza completa (orden y numero
// de llamadas virtuales, rand y colaboradores), palabra de control x87 y pila x87 vacia. El x87 corre a 24 bits como el juego.
#include <fstream>
#include <unordered_map>
#include <map>
#include <float.h>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexKillCriminal.h"
#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskSimplePlayerOnFoot.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGunControl.h"
#include "Entity/Ped/CopPed.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "Cover.h"
#include "CarCtrl.h"
#include "World.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace TaskWave2ALive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const std::string& s,uint64 h=1469598103934665603ull){ for(unsigned char c:s){h^=c;h*=1099511628211ull;} return h; }
// Hilo del banco: otros hilos del juego (audio, streaming) llaman a funciones parcheadas; no se trazan ni tocan el estado compartido.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

constexpr int FAKES=10, QUIET=4;
constexpr size_t FK_TYPE=0x38, FK_ABORT=0x3C, FK_SUB=0x40, FK_SIZE=0x60;   // configuracion por objeto dentro de cada tarea falsa
// CPed (EXE)
constexpr size_t P_AREA=0x2F, P_FLAGS=0x46C, P_CREATED=0x484, P_INTEL=0x47C, P_PDATA=0x480, P_HEALTH=0x540, P_TIMEWE=0x54C, P_VEH=0x58C, P_TYPE=0x598, P_WEAPONS=0x5A0,
                 P_WSLOT=0x718, P_TARGETED=0x71C, P_COVER=0x788, P_REMOVAL=0x790, P_ATTACHED=0xFC, P_ANIMGROUP=0x4D4, P_CLUMP=0x18, P_MOVESPEED=0x44;
constexpr size_t COP_PARTNER=0x7A8, COP_CRIMS=0x7AC, COP_DRIVER=0x7C0;                                                 // CCopPed
constexpr size_t V_MATRIX=0x14, V_STATUS=0x36, V_AP_STYLE=0x3B9, V_AP_MISSION=0x3BA, V_AP_SPEED=0x3D0, V_DRIVER=0x460, V_FLAGS=0x42C, V_EXT=0x4A6, V_TYPE=0x590; // CVehicle
constexpr size_t I_PRIMARY=0x4, I_SECONDARY=0x18;      // CPedIntelligence: m_TaskMgr en +4 (primarias +4, secundarias +0x18)
constexpr size_t D_BLEND=0x14, D_FLAGS=0x34;           // CPlayerPedData
constexpr uint32 TASK_FINAL_MARK=0xFAB0;               // tipo de las tareas falsas "rapidas" (no trazadas)

alignas(16) inline std::array<uint8,sizeof(CCopPed)> pedP{}, pedN{};
alignas(16) inline std::array<uint8,sizeof(CPed)> pedK{}, pedL{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,3> pedX{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehA{}, vehB{}, tgt{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,10> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,4> intels{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdata{};
alignas(16) inline std::array<uint8,0x300> grp{};      // CPedGroup falso: lider en +0x28 (CPedGroupMembership +8, lider +0x20)
alignas(16) inline std::array<uint8,0x40> cover{};
alignas(16) inline std::array<uint8,0x40> fakeClump = []{ std::array<uint8,0x40> a{}; a[0]=2; return a; }();
alignas(16) inline std::array<std::array<uint8,FK_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,FK_SIZE>,QUIET> quiet{};
alignas(16) inline std::array<uint8,0x80> event{};
alignas(16) inline std::array<uint8,0x100> object{}, object2{};
enum { M_P, M_K, M_N, M_X0, M_X1, M_X2, M_A, M_B, M_T, M_L };
enum { I_P, I_K, I_N, I_L };
inline CPed* PedP(){ return reinterpret_cast<CPed*>(pedP.data()); }
inline CPed* PedK(){ return reinterpret_cast<CPed*>(pedK.data()); }
inline CPed* PedN(){ return reinterpret_cast<CPed*>(pedN.data()); }
inline CPed* PedL(){ return reinterpret_cast<CPed*>(pedL.data()); }
inline CPed* PedX(int n){ return reinterpret_cast<CPed*>(pedX[n].data()); }
inline CTask* Fake(int n){ return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CTask* Quiet(int n){ return reinterpret_cast<CTask*>(quiet[n].data()); }
template<class T=uint32> T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

struct Buf { const char* name; uint8* p; size_t n; bool ent; };
inline std::vector<Buf> bufs;
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(const auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.name)+"+"+std::to_string(v-uintptr_t(b.p));
    for(int n=0;n<FAKES;++n)if(v>=uintptr_t(fakes[n].data())&&v<uintptr_t(fakes[n].data())+FK_SIZE)return "fk#"+std::to_string(n)+"+"+std::to_string(v-uintptr_t(fakes[n].data()));
    for(int n=0;n<QUIET;++n)if(v>=uintptr_t(quiet[n].data())&&v<uintptr_t(quiet[n].data())+FK_SIZE)return "q#"+std::to_string(n)+"+"+std::to_string(v-uintptr_t(quiet[n].data()));
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
inline std::string EntHash(const Buf& b) {
    std::vector<uint8> copy(b.p,b.p+b.n); const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(b.p)->m_pReferences)-uintptr_t(b.p);
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline void CleanRefs() {
    for(const auto& b:bufs)if(b.ent){ auto* e=reinterpret_cast<CEntity*>(b.p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); }
}
inline uint16 FpuTag() {   // pila x87 vacia = 0xFFFF; fnstenv enmascara las excepciones, fldenv restaura el entorno tal cual
    uint8 env[28]{};
    __asm { lea eax, env
            fnstenv [eax]
            fldenv [eax] }
    uint16 tag; std::memcpy(&tag,env+8,2); return tag;
}

// configuracion por caso de las dependencias simuladas
inline int eventType{}, eventPrio{}; inline uintptr_t eventSource{}; inline std::array<int,8> randQ{}; inline unsigned randIdx{};
inline std::array<bool,4> losQ{}; inline unsigned losIdx{}; inline bool fcRet{}, camFirst{}; inline CVector fcCoords{};
inline uintptr_t coverRet{}, groupRet{}, groupPed{};

// tareas falsas: cada objeto lleva su tipo, su respuesta a MakeAbortable y su subtarea
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { if(Mine())trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FSub(CTask* t,void*) { if(Mine())trace.push_back({"get_sub_task",Name(uintptr_t(t))}); return *reinterpret_cast<CTask**>(uintptr_t(t)+FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { if(Mine())trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { if(Mine())trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return *reinterpret_cast<uint8*>(uintptr_t(t)+FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(CTask*,void*) { if(Mine())trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// tareas falsas "silenciosas" (las que la inteligencia busca por tipo: el numero de lecturas de GetTaskType no es comparable entre EXE y DLL)
inline void* __fastcall QDtor(CTask* t,void*,unsigned) { return t; }
inline CTask* __fastcall QSub(CTask* t,void*) { return *reinterpret_cast<CTask**>(uintptr_t(t)+FK_SUB); }
inline int __fastcall QType(CTask* t,void*) { return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
template<int N> void* __fastcall QTrap(CTask*,void*) { if(Mine())trace.push_back({"quiet_unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> quietVtbl{};
template<int... N> void FillQTraps(std::integer_sequence<int,N...>) { ((quietVtbl[N]=reinterpret_cast<void*>(&QTrap<N>)),...); }
// evento falso (GetEventType hueco 1, GetEventPriority hueco 2, GetSourceEntity hueco 10)
inline int __fastcall EType(void* e,void*) { if(Mine())trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
inline int __fastcall EPrio(void* e,void*) { if(Mine())trace.push_back({"event_get_priority",Name(uintptr_t(e))}); return eventPrio; }
inline uintptr_t __fastcall ESource(void* e,void*) { if(Mine())trace.push_back({"event_get_source",Name(uintptr_t(e))}); return eventSource; }
template<int N> void* __fastcall ETrap(void*,void*) { if(Mine())trace.push_back({"event_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { if(!Mine())return 0x1234; const int v=randQ[randIdx++%8]; trace.push_back({"rand",v}); return v; }
inline uint16 __cdecl FakeRandU16() { return uint16(FakeRand()); }
inline void __fastcall SetWeaponStub(CPed* p,void*,int w) { if(Mine())trace.push_back({"set_current_weapon",Name(uintptr_t(p)),w}); }
inline void __fastcall DefaultDMStub(CPed* p,void*) { if(Mine())trace.push_back({"set_default_decision_maker",Name(uintptr_t(p))}); }
inline void __cdecl JoinStub(CVehicle* v) { if(Mine())trace.push_back({"join_car_with_road_system",Name(uintptr_t(v))}); }
inline int16 __fastcall SayStub(CPed* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c) {
    if(Mine())trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),a,b,c}); return 0; }
inline CPedGroup* __cdecl GroupStub(const CPed* p) {
    if(!Mine())return nullptr; trace.push_back({"get_peds_group",Name(uintptr_t(p))});
    return uintptr_t(p)==groupPed ? reinterpret_cast<CPedGroup*>(groupRet) : nullptr; }
inline bool __cdecl LosStub(const CVector* a,const CVector* b,bool bl,bool v,bool pd,bool ob,bool du,bool st,bool cam) {
    if(!Mine())return false; const bool r=losQ[losIdx++%4];
    trace.push_back({"line_of_sight",F(a->x),F(a->y),F(a->z),F(b->x),F(b->y),F(b->z),int(bl)|int(v)<<1|int(pd)<<2|int(ob)<<3|int(du)<<4|int(st)<<5|int(cam)<<6,r}); return r; }
inline void __fastcall BoneStub(CPed* p,void*,RwV3d* out,uint32 bone,bool upd) {
    if(!Mine())return; trace.push_back({"bone_position",Name(uintptr_t(p)),F(out->x),F(out->y),F(out->z),bone,upd});
    const auto& pos=p->GetPosition(); out->x=pos.x+0.1f; out->y=pos.y+float(bone); out->z=pos.z+0.7f; }
inline CCoverPoint* __cdecl FindCoverStub(CPed* p,const CVector* pos,bool attack) {
    if(!Mine())return nullptr; trace.push_back({"find_cover",Name(uintptr_t(p)),F(pos->x),F(pos->y),F(pos->z),attack}); return reinterpret_cast<CCoverPoint*>(coverRet); }
inline bool __cdecl CoverCoordsStub(const CCoverPoint* cp,CPed* p,const CVector* pos,CVector* out) {
    if(!Mine())return false; trace.push_back({"cover_coordinates",Name(uintptr_t(cp)),Name(uintptr_t(p)),F(pos->x),F(pos->y),F(pos->z),fcRet});
    if(fcRet)*out=fcCoords; return fcRet; }
inline void __fastcall ReleaseCoverStub(CPed* p,void*) { if(!Mine())return; trace.push_back({"release_cover",Name(uintptr_t(p))}); At(p,P_COVER)=0; }
inline void __fastcall DuckSecondaryStub(void* i,void*,uint32 len) { if(Mine())trace.push_back({"set_task_duck_secondary",Name(uintptr_t(i)),len&0xFFFF}); }
inline void __fastcall DuckTimerStub(void* t,void*,uint32 len) { if(Mine())trace.push_back({"set_duck_timer",Name(uintptr_t(t)),len&0xFFFF}); }
inline bool __fastcall DuckMoveStub(void* t,void*,float x,float y) { if(Mine())trace.push_back({"control_duck_move",Name(uintptr_t(t)),F(x),F(y)}); return true; }
inline bool __fastcall GunMoveStub(void* t,void*,const CVector2D* v) { if(Mine())trace.push_back({"control_gun_move",Name(uintptr_t(t)),F(v->x),F(v->y)}); return true; }
inline CTask* __fastcall KaSubStub(void* self,void*,int type,CPed* p) { if(!Mine())return nullptr; trace.push_back({"ka_create_sub_task",Name(uintptr_t(self)),type,Name(uintptr_t(p))}); return Fake(FAKES-1); }
inline bool __fastcall Cam1stStub(void*,void*) { if(Mine())trace.push_back({"camera_using_1st_person",camFirst}); return camFirst; }
inline void __fastcall CamClearStub(void*,void*) { if(Mine())trace.push_back({"camera_clear_player_weapon_mode"}); }
inline void __cdecl CrossHairStub(uint32 id) { if(Mine())trace.push_back({"clear_cross_hair",id}); }
inline void* __cdecl BlendStub(RpClump* c,int group,int anim,float blend) { if(Mine())trace.push_back({"blend_animation",uintptr_t(c),group,anim,F(blend)}); return nullptr; }

// ProcessPed de CTaskSimplePlayerOnFoot solo reparte el trabajo: las cinco funciones grandes se simulan y se traza cual se llama y con que argumentos.
// EXE: estaticas con `ret 4` (ecx = tarea, sin usar) y miembros thiscall; DLL: estaticas cdecl y miembros thiscall.
inline void __fastcall PfDuckedExe(void*,void*,CPed* p) { if(Mine())trace.push_back({"pf_control_ducked",Name(uintptr_t(p))}); }
inline void __cdecl PfDuckedDll(CPed* p) { if(Mine())trace.push_back({"pf_control_ducked",Name(uintptr_t(p))}); }
inline void __fastcall PfZeldaWeaponExe(void*,void*,CPed* p) { if(Mine())trace.push_back({"pf_control_zelda_weapon",Name(uintptr_t(p))}); }
inline void __cdecl PfZeldaWeaponDll(CPed* p) { if(Mine())trace.push_back({"pf_control_zelda_weapon",Name(uintptr_t(p))}); }
inline void __fastcall PfFighter(void* self,void*,CPed* p) { if(Mine())trace.push_back({"pf_control_fighter",Name(uintptr_t(self)),Name(uintptr_t(p))}); }
inline int __fastcall PfZelda(void* self,void*,CPed* p,bool avoid) { if(Mine())trace.push_back({"pf_control_zelda",Name(uintptr_t(self)),Name(uintptr_t(p)),avoid}); return 0; }
inline void __fastcall PfWeapon(void* self,void*,CPed* p) { if(Mine())trace.push_back({"pf_process_player_weapon",Name(uintptr_t(self)),Name(uintptr_t(p))}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j)a+=5+*reinterpret_cast<int32*>(a+1); return a; }
template<class T> uintptr_t Addr(T fn){ static_assert(sizeof(T)==sizeof(uintptr_t)); return std::bit_cast<uintptr_t>(fn); }

enum Kind { KC_CTOR, KC_DTOR, KC_CLONE, KC_CREATE, KC_FINDNEXT, KC_CHANGE, KC_ABORT, KC_NEXT,
            KA_CTOR, KA_DTOR, KA_CLONE, KA_ABORT, KA_LEADER, KA_LOS, KA_FIRST, KA_CONTROL,
            PF_CTOR, PF_DTOR, PF_CLONE, PF_ABORT, PF_PROCESS };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=21;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexKillCriminal","Constructor",0x68BE70,0,-1,256},
    {"CTaskComplexKillCriminal","Destructor",0x68BF30,0,-1,256},
    {"CTaskComplexKillCriminal","Clone",0,0x870A00,1,128},
    {"CTaskComplexKillCriminal","CreateSubTask",0x68C050,0,-1,1024},
    {"CTaskComplexKillCriminal","FindNextCriminalToKill",0x68C3C0,0,-1,1024},
    {"CTaskComplexKillCriminal","ChangeTarget",0x68C6E0,0,-1,1024},
    {"CTaskComplexKillCriminal","MakeAbortable",0,0x870A00,6,1024},
    {"CTaskComplexKillCriminal","CreateNextSubTask",0,0x870A00,8,1024},
    {"CTaskComplexKillPedOnFootArmed","Constructor",0x621190,0,-1,128},
    {"CTaskComplexKillPedOnFootArmed","Destructor",0x621250,0,-1,128},
    {"CTaskComplexKillPedOnFootArmed","Clone",0,0x86D918,1,128},
    {"CTaskComplexKillPedOnFootArmed","MakeAbortable",0,0x86D918,6,256},
    {"CTaskComplexKillPedOnFootArmed","IsPedInLeaderFiringLine",0x621300,0,-1,1024},
    {"CTaskComplexKillPedOnFootArmed","LineOfSightClearForAttack",0x621500,0,-1,1024},
    {"CTaskComplexKillPedOnFootArmed","CreateFirstSubTask",0,0x86D918,9,1024},
    {"CTaskComplexKillPedOnFootArmed","ControlSubTask",0,0x86D918,10,1024},
    {"CTaskSimplePlayerOnFoot","Constructor",0x685750,0,-1,64},
    {"CTaskSimplePlayerOnFoot","Destructor",0x6857D0,0,-1,64},
    {"CTaskSimplePlayerOnFoot","Clone",0,0x8708EC,1,64},
    {"CTaskSimplePlayerOnFoot","MakeAbortable",0,0x8708EC,6,1024},
    {"CTaskSimplePlayerOnFoot","ProcessPed",0,0x8708EC,7,1024},
}};
constexpr std::array<Kind,NFNS> KINDS{KC_CTOR,KC_DTOR,KC_CLONE,KC_CREATE,KC_FINDNEXT,KC_CHANGE,KC_ABORT,KC_NEXT,
                                      KA_CTOR,KA_DTOR,KA_CLONE,KA_ABORT,KA_LEADER,KA_LOS,KA_FIRST,KA_CONTROL,
                                      PF_CTOR,PF_DTOR,PF_CLONE,PF_ABORT,PF_PROCESS};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"taskwave2a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"taskwave2a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("taskwave2a");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"taskwave2a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // registro de buffers (nombre estable para las trazas)
    bufs.clear();
    auto reg=[&](const char* n,auto& a,bool ent){ bufs.push_back({n,a.data(),a.size(),ent}); };
    reg("pedP",pedP,true); reg("pedK",pedK,true); reg("pedN",pedN,true); reg("pedL",pedL,true);
    static const char* xn[3]{"pedX0","pedX1","pedX2"}; for(int n=0;n<3;++n)reg(xn[n],pedX[n],true);
    reg("vehA",vehA,true); reg("vehB",vehB,true); reg("tgt",tgt,true);
    static const char* mn[10]{"mat0","mat1","mat2","mat3","mat4","mat5","mat6","mat7","mat8","mat9"}; for(int n=0;n<10;++n)reg(mn[n],mats[n],false);
    static const char* in[4]{"intel0","intel1","intel2","intel3"}; for(int n=0;n<4;++n)reg(in[n],intels[n],false);
    reg("pdata",pdata,false); reg("grp",grp,false); reg("cover",cover,false); reg("event",event,false); reg("object",object,false); reg("object2",object2,false);

    // Comprobacion de la disposicion de miembros publicos frente a los desplazamientos del EXE usados por los fixtures.
    auto off=[](const void* base,const void* member){ return uintptr_t(member)-uintptr_t(base); };
    {
        auto* cop=reinterpret_cast<CCopPed*>(pedP.data()); auto* ka=reinterpret_cast<CTaskComplexKillPedOnFootArmed*>(object.data());
        auto* pf=reinterpret_cast<CTaskSimplePlayerOnFoot*>(object.data()); auto* veh=reinterpret_cast<CVehicle*>(vehA.data());
        const bool layout=off(cop,&cop->m_pCopPartner)==COP_PARTNER && off(cop,&cop->m_apCriminalsToKill)==COP_CRIMS && off(cop,&cop->m_isTheDriver)==COP_DRIVER &&
            off(cop,&cop->m_matrix)==V_MATRIX && off(cop,&cop->m_fHealth)==P_HEALTH && off(cop,&cop->m_nPedType)==P_TYPE && off(cop,&cop->m_pVehicle)==P_VEH &&
            off(cop,&cop->m_pTargetedObject)==P_TARGETED && off(cop,&cop->m_pCoverPoint)==P_COVER && off(cop,&cop->m_nTimeTillWeNeedThisPed)==P_TIMEWE && off(cop,&cop->m_nMoveState)==0x534 &&
            off(veh,&veh->m_pDriver)==V_DRIVER &&
            off(ka,&ka->m_target)==0xC && off(ka,&ka->m_shootTimer)==0x10 && off(ka,&ka->m_lastAttackTime)==0x14 && off(ka,&ka->m_lastRollTime)==0x18 &&
            off(ka,&ka->m_duckingConditions)==0x1C && off(ka,&ka->m_lengthOfDuck)==0x20 && off(ka,&ka->m_duckChancePerc)==0x24 && off(ka,&ka->m_lastDuckTime)==0x28 &&
            off(ka,&ka->m_lastStrafeTime)==0x2C && off(ka,&ka->m_competence)==0x30 && off(ka,&ka->m_bStrafeBack)==0x32 && off(ka,&ka->m_bShotFiredByPlayer)==0x33 &&
            off(ka,&ka->m_newTarget)==0x34 && off(ka,&ka->m_aimImmediate)==0x35 && off(ka,&ka->m_losBlockedOurPos)==0x38 && off(ka,&ka->m_losBlockedTargetPos)==0x44 &&
            off(ka,&ka->m_losBlockedTime)==0x50 && off(ka,&ka->m_losClearTime)==0x54 && off(ka,&ka->m_needToMoveInCloserTime)==0x58 &&
            off(pf,&pf->m_PlayerIdlesAnimBlockId)==8 && off(pf,&pf->m_nFrameCounter)==0xC && off(pf,&pf->m_nTimer)==0x10 && sizeof(CTaskSimplePlayerOnFoot)==0x1C &&
            sizeof(CTaskComplexKillPedOnFootArmed)==0x5C;
        Write({{"event","layout"},{"ok",layout}});
        if(!layout){Write({{"event","error"},{"reason","layout mismatch between DLL members and EXE offsets"}});return;}
    }
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillQTraps(std::make_integer_sequence<int,12>{});
    quietVtbl[0]=reinterpret_cast<void*>(&QDtor); quietVtbl[2]=reinterpret_cast<void*>(&QSub); quietVtbl[3]=reinterpret_cast<void*>(&FSimple); quietVtbl[4]=reinterpret_cast<void*>(&QType);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio); eventVtbl[10]=reinterpret_cast<void*>(&ESource);

    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 savedLastFree=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC);
    auto& currArea=*reinterpret_cast<int32*>(0xB72914); auto& procLevel=*reinterpret_cast<int32*>(0xB7298C); const int32 savedArea=currArea, savedProc=procLevel;
    const unsigned savedFP=_controlfp(0,0);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    // sitios parcheables: direccion del EXE y simbolo de la DLL (0 = no existe / no se parchea)
    struct Site { uintptr_t exeAddr, dll; void* stub; void* dllStub; };   // dllStub = otro stub para la DLL si la convencion difiere (0 = el mismo)
    const std::array<Site,26> sites{{
        {0x821B1E,0,reinterpret_cast<void*>(&FakeRand)},
        {0,Addr(&CGeneral::GetRandomNumber),reinterpret_cast<void*>(&FakeRandU16)},
        {0x5E6280,Addr(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),reinterpret_cast<void*>(&SetWeaponStub)},
        {0x5E06E0,Addr(&CPed::SetPedDefaultDecisionMaker),reinterpret_cast<void*>(&DefaultDMStub)},
        {0x42F5A0,Addr(&CCarCtrl::JoinCarWithRoadSystem),reinterpret_cast<void*>(&JoinStub)},
        {0x5EFFE0,Addr(&CPed::Say),reinterpret_cast<void*>(&SayStub)},
        {0x5F7E80,Addr(&CPedGroups::GetPedsGroup),reinterpret_cast<void*>(&GroupStub)},
        {0x56A490,Addr(&CWorld::GetIsLineOfSightClear),reinterpret_cast<void*>(&LosStub)},
        {0x5E01C0,Addr(&CPed::GetTransformedBonePosition),reinterpret_cast<void*>(&BoneStub)},
        {0x6992B0,Addr(&CCover::FindAndReserveCoverPoint),reinterpret_cast<void*>(&FindCoverStub)},
        {0x699570,Addr(&CCover::FindCoordinatesCoverPoint),reinterpret_cast<void*>(&CoverCoordsStub)},
        {0x5E0270,Addr(&CPed::ReleaseCoverPoint),reinterpret_cast<void*>(&ReleaseCoverStub)},
        {0x601230,Addr(&CPedIntelligence::SetTaskDuckSecondary),reinterpret_cast<void*>(&DuckSecondaryStub)},
        {0x692530,Addr(&CTaskSimpleDuck::SetDuckTimer),reinterpret_cast<void*>(&DuckTimerStub)},
        {0x6923F0,Addr(&CTaskSimpleDuck::ControlDuckMove),reinterpret_cast<void*>(&DuckMoveStub)},
        {0x61E0C0,Addr(&CTaskSimpleUseGun::ControlGunMove),reinterpret_cast<void*>(&GunMoveStub)},
        {0x626FC0,Addr(&CTaskComplexKillPedOnFootArmed::CreateSubTask),reinterpret_cast<void*>(&KaSubStub)},
        {0x50BFF0,Addr(&CCamera::Using1stPersonWeaponMode),reinterpret_cast<void*>(&Cam1stStub)},
        {0x50AB10,Addr(&CCamera::ClearPlayerWeaponMode),reinterpret_cast<void*>(&CamClearStub)},
        {0x742C60,Addr(&CWeaponEffects::ClearCrossHair),reinterpret_cast<void*>(&CrossHairStub)},
        {0x4D4610,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),reinterpret_cast<void*>(&BlendStub)},
        {0x687F30,Addr(&CTaskSimplePlayerOnFoot::PlayerControlDucked),reinterpret_cast<void*>(&PfDuckedExe),reinterpret_cast<void*>(&PfDuckedDll)},
        {0x687C20,Addr(&CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon),reinterpret_cast<void*>(&PfZeldaWeaponExe),reinterpret_cast<void*>(&PfZeldaWeaponDll)},
        {0x687530,Addr(&CTaskSimplePlayerOnFoot::PlayerControlFighter),reinterpret_cast<void*>(&PfFighter)},
        {0x6883D0,Addr(&CTaskSimplePlayerOnFoot::PlayerControlZelda),reinterpret_cast<void*>(&PfZelda)},
        {0x6859A0,Addr(&CTaskSimplePlayerOnFoot::ProcessPlayerWeapon),reinterpret_cast<void*>(&PfWeapon)}}};
    // bytes iniciales de las entradas parcheables, para comprobar la restauracion
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& s:sites)for(auto a:{s.exeAddr,s.dll,s.dll ? Body(s.dll) : uintptr_t(0)})if(a)entryBefore.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),8)});
    for(auto& fn:fns)if(fn.address)entryBefore.push_back({fn.address,ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address),8)});
    std::vector<std::pair<uintptr_t,uintptr_t>> vtBefore;
    for(auto& fn:fns)if(fn.vt)vtBefore.push_back({fn.vt+fn.slot*4,*reinterpret_cast<uintptr_t*>(fn.vt+fn.slot*4)});
    Write({{"event","begin"},{"fp_control",savedFP},{"hooks",NFNS}});

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        /* El cuerpo real se calcula ANTES de parchear ninguna entrada: tras parchear s.dll la cadena de E9 pasa por el propio stub y
           parchear "Body(s.dll)" sobreescribiria la entrada del stub con un salto a si mismo (bucle infinito). */
        std::vector<std::pair<uintptr_t,void*>> targets;
        for(auto& s:sites){ if(s.exeAddr)targets.push_back({s.exeAddr,s.stub}); if(s.dll){ const uintptr_t body=Body(s.dll); targets.push_back({s.dll,s.dllStub ? s.dllStub : s.stub}); if(body!=s.dll)targets.push_back({body,s.dllStub ? s.dllStub : s.stub}); } }
        for(auto& [at,stub]:targets)patches.push_back(std::make_unique<EntryPatch>(at,stub));

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm; const Kind kind=KINDS[f];
            if(!only.empty()&&only!=full)continue;
            std::map<std::string,int> hist;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F2Au; if(!st)st=1; trace=json::array(); randIdx=0; losIdx=0;
                    for(auto& q:randQ){ uint32 v=next()&0x7FFF; const uint32 m=next()%4; if(m==0)v&=~0x3Fu; else if(m==1)v&=~0xFu; q=int(v); }
                    for(auto& q:losQ)q=next()%2;
                    CleanRefs();
                    for(const auto& b:bufs)std::memset(b.p,0,b.n);
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FK_SIZE); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data();
                        At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_STAND_STILL; At<uint8>(fakes[n].data(),FK_ABORT)=uint8(next()%2); }
                    for(int n=0;n<QUIET;++n){ std::memset(quiet[n].data(),0,FK_SIZE); *reinterpret_cast<void**>(quiet[n].data())=quietVtbl.data(); At<int32>(quiet[n].data(),FK_TYPE)=TASK_SIMPLE_STAND_STILL; }
                    for(auto& b:event)b=uint8(next()%4==0 ? next() : 0); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    eventType=0; eventPrio=0; eventSource=0; fcRet=false; fcCoords=CVector{0.f,0.f,0.f}; coverRet=0; groupRet=0; groupPed=0; camFirst=false;
                    currArea=savedArea; procLevel=savedProc;

                    auto* P=PedP(); auto* K=PedK(); auto* N=PedN(); auto* L=PedL(); CPed* X[3]{PedX(0),PedX(1),PedX(2)};
                    auto* VA=reinterpret_cast<CVehicle*>(vehA.data()); auto* VB=reinterpret_cast<CVehicle*>(vehB.data());
                    auto mk=[&](CPed* e,int mi,int ii,const CVector& pos,int pedType,float health,bool useMat){
                        e->SetType(ENTITY_TYPE_PED);
                        if(useMat){ CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=pos; e->m_matrix=reinterpret_cast<CMatrixLink*>(mats[mi].data()); std::memcpy(mats[mi].data(),&m,sizeof(CMatrix)); }
                        else e->SetPosn(pos);
                        At(e,P_INTEL)=uint32(uintptr_t(intels[ii].data())); At<float>(e,P_HEALTH)=health; At<int32>(e,P_TYPE)=pedType; };
                    auto hp=[&]()->float{ switch(next()%10){ case 0: return 0.f; case 1: return -uni(0.1f,50.f); default: return uni(1.f,200.f); } };
                    auto place=[&](CEntity* e,int mi,const CVector& pos,int type){
                        e->SetType(eEntityType(type));
                        if(next()%2){ CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=pos; e->m_matrix=reinterpret_cast<CMatrixLink*>(mats[mi].data()); std::memcpy(mats[mi].data(),&m,sizeof(CMatrix)); }
                        else e->SetPosn(pos); };
                    auto vehicle=[&](CVehicle* v,int mi,const CVector& pos){
                        place(v,mi,pos,ENTITY_TYPE_VEHICLE);
                        At<uint8>(v,V_STATUS)=uint8((At<uint8>(v,V_STATUS)&7)|((next()%6)<<3));
                        At<int32>(v,V_TYPE)= next()%3==0 ? VEHICLE_TYPE_BIKE : int(next()%9);
                        At<uint8>(v,V_AP_STYLE)=uint8(next()); At<uint8>(v,V_AP_MISSION)=uint8(next()%4==0 ? 54+next()%2 : next()); At<uint8>(v,V_AP_SPEED)=uint8(next());
                        At<uint8>(v,V_FLAGS)=uint8(next()); At<uint8>(v,V_FLAGS+1)=uint8(next()); At<uint16>(v,V_EXT)=uint16(next()); };
                    auto setpos=[&](CEntity* e,const CVector& v){ e->m_matrix=nullptr; e->SetPosn(v); };   // sin matriz: la posicion vive en el placement
                    auto reg2=[&](CEntity* target,void* slot){ if(target)target->RegisterReference(reinterpret_cast<CEntity**>(slot)); };
                    int fk=0;
                    auto chain=[&](int len,int lastType)->CTask* {
                        CTask* head=nullptr; CTask* prev=nullptr;
                        for(int k=0;k<len&&fk<FAKES-1;++k){ CTask* t=Fake(fk++);
                            static const int mids[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_SIMPLE_FIGHT};
                            At<int32>(t,FK_TYPE)=(k==len-1) ? lastType : mids[next()%5];
                            if(prev)At<CTask*>(prev,FK_SUB)=t; else head=t; prev=t; }
                        return head; };
                    static const int ktypes[]{4,5,4,5,7,8,20,21,22,6,18,19,23,24,25,31,0,1,2,16};
                    auto weapon=[&](CPed* e,int type,int slot){ At<int8>(e,P_WSLOT)=int8(slot); At<int32>(e,P_WEAPONS+slot*0x1C)=type; };
                    static const int wtypes[]{0,1,4,9,16,18,22,23,24,25,27,29,30,31,33,34,35,37,39,41};

                    // mundo comun: cop P (con vehiculo A), criminal K (vehiculo B), companero N, extras X0..X2
                    auto world=[&](bool dead){
                        const CVector pp{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,20.f)};
                        mk(P,M_P,I_P,pp,PED_TYPE_COP,dead ? hp() : uni(1.f,200.f),next()%2);
                        mk(K,M_K,I_K,CVector{pp.x+uni(-40.f,40.f),pp.y+uni(-40.f,40.f),pp.z+uni(-3.f,3.f)},ktypes[next()%20],hp(),next()%2);
                        mk(N,M_N,I_N,CVector{pp.x+uni(-8.f,8.f),pp.y+uni(-8.f,8.f),pp.z},PED_TYPE_COP,hp(),next()%2);
                        for(int n=0;n<3;++n)mk(X[n],M_X0+n,I_L,CVector{pp.x+uni(-40.f,40.f),pp.y+uni(-40.f,40.f),pp.z},ktypes[next()%14],hp(),next()%2);
                        At<uint8>(K,P_CREATED)= next()%6==0 ? 2 : uint8(next()%3); At<uint8>(N,P_CREATED)=uint8(next()%3);
                        vehicle(VA,M_A,CVector{pp.x+uni(-6.f,6.f),pp.y+uni(-6.f,6.f),pp.z}); vehicle(VB,M_B,CVector{pp.x+uni(-40.f,40.f),pp.y+uni(-40.f,40.f),pp.z});
                        At<uint32>(P,P_FLAGS)=(next()&~0x100u)|(next()%2 ? 0x100u : 0u); At<uint32>(K,P_FLAGS)=(next()&~0x100u)|(next()%2 ? 0x100u : 0u);
                        At<uint32>(N,P_FLAGS)=(next()&~0x100u)|(next()%2 ? 0x100u : 0u);
                        At(P,P_VEH)= next()%4 ? uint32(uintptr_t(vehA.data())) : 0u; At(K,P_VEH)= next()%2 ? uint32(uintptr_t(vehB.data())) : 0u; At(N,P_VEH)=uint32(uintptr_t(vehA.data()));
                        At(VA,V_DRIVER)= next()%2 ? uint32(uintptr_t(P)) : 0u;
                        At<uint8>(P,COP_DRIVER)=uint8(next()%2); At<uint8>(N,COP_DRIVER)=uint8(next()%2);
                        // lista de criminales del cop y companero (referencias registradas en los huecos)
                        CPed* pool[]{nullptr,nullptr,K,X[0],X[1],X[2]};
                        for(int s=0;s<5;++s){ CPed* c=pool[next()%6]; At<CPed*>(P,COP_CRIMS+4*s)=c; reg2(c,static_cast<uint8*>(static_cast<void*>(P))+COP_CRIMS+4*s); }
                        if(next()%3){ At<CPed*>(P,COP_PARTNER)=N; reg2(N,static_cast<uint8*>(static_cast<void*>(P))+COP_PARTNER); }
                        for(int s=0;s<5;++s){ CPed* c=pool[next()%6]; At<CPed*>(N,COP_CRIMS+4*s)=c; reg2(c,static_cast<uint8*>(static_cast<void*>(N))+COP_CRIMS+4*s); }
                        weapon(P,wtypes[next()%20],int(next()%13)); weapon(K,wtypes[next()%20],int(next()%13)); };

                    std::memset(object.data(),0xCD,object.size()); std::memset(object2.data(),0xCD,object2.size());
                    uint8* const o=object.data(); auto W=[&](size_t offs)->uint32&{return *reinterpret_cast<uint32*>(o+offs);};
                    const uintptr_t* vt=fn.vt ? reinterpret_cast<uintptr_t*>(fn.vt) : nullptr;
                    uintptr_t vtObject=0; int prio=0; const void* evArg=nullptr; CPed* pedArg=P; CPed* pedArg2=nullptr; int ttArg=0; bool boolArg=false;
                    uint32 a0=0,a1=0,a2=0,a3=0; int8 a4=0; bool callOnObject2=false;

                    switch(kind){
                    case KC_CTOR: {
                        world(false); pedArg= next()%6==0 ? nullptr : K; boolArg=next()%2; break; }
                    case KC_DTOR: {
                        vtObject=0x870A00; world(false);
                        W(8)=next()%3==0 ? 0u : uint32(uintptr_t(chain(1+int(next()%3),TASK_COMPLEX_KILL_PED_ON_FOOT)));
                        W(0xC)= next()%5==0 ? 0u : uint32(uintptr_t(K)); W(0x10)= next()%4==0 ? 0u : uint32(uintptr_t(P));
                        reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC); reg2(reinterpret_cast<CEntity*>(W(0x10)),o+0x10);
                        *reinterpret_cast<float*>(o+0x14)=uni(-1.f,4.f); for(size_t k=0x18;k<0x20;++k)o[k]=uint8(next());
                        // vehiculo del cop: conductor, estado simple/no simple, banderas de sirena
                        break; }
                    case KC_CLONE: {
                        vtObject=0x870A00; world(false);
                        W(8)=0; W(0xC)= next()%5==0 ? 0u : uint32(uintptr_t(K)); W(0x10)=uint32(uintptr_t(P));
                        reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC); reg2(P,o+0x10);
                        *reinterpret_cast<float*>(o+0x14)=uni(-1.f,4.f); for(size_t k=0x18;k<0x20;++k)o[k]=uint8(next()); break; }
                    case KC_CREATE: {
                        vtObject=0x870A00; world(false);
                        static const int tts[]{700,701,702,703,704,705,706,708,709,710,724,1000,1022,1302,0,699,1,1001,1021,1023,2000,725,723};
                        ttArg=tts[next()%23]; boolArg=next()%2;
                        if(ttArg==724&&!At<uint32>(P,P_VEH)) At(P,P_VEH)=uint32(uintptr_t(vehA.data()));
                        if(next()%5==0&&ttArg!=724) At(K,P_VEH)=0u;
                        W(8)= next()%3==0 ? 0u : uint32(uintptr_t(Fake(0))); At<int32>(Fake(0),FK_TYPE)= next()%2 ? ttArg : ttArg+int(next()%3)-1;
                        W(0xC)=(ttArg==724||next()%6) ? uint32(uintptr_t(K)) : 0u; W(0x10)=uint32(uintptr_t(P));
                        reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC); reg2(P,o+0x10);
                        for(size_t k=0x14;k<0x20;++k)o[k]=uint8(next()); pedArg=P; break; }
                    case KC_FINDNEXT: {
                        vtObject=0x870A00; world(true);
                        CPed* cands[]{nullptr,K,X[0],X[1],X[2],N};
                        W(0xC)=uint32(uintptr_t(cands[next()%6])); reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC); W(0x10)=uint32(uintptr_t(P)); reg2(P,o+0x10);
                        if(next()%4==0){ // posiciones coincidentes / empates
                            const auto pos=P->GetPosition(); for(int n=0;n<3;++n)setpos(X[n],pos); setpos(K,pos); }
                        if(next()%6==0)for(int n=0;n<3;++n)At<float>(X[n],P_HEALTH)=uni(1.f,50.f);
                        boolArg=next()%2; pedArg=P; break; }
                    case KC_CHANGE: {
                        vtObject=0x870A00; world(true);
                        CPed* cands[]{nullptr,K,X[0],X[1],X[2],N,P};
                        // newTarget: nulo, igual al criminal actual, listado, no listado
                        CPed* cur=cands[next()%5]; pedArg2=cands[next()%7];
                        if(next()%4==0)pedArg2=cur;
                        W(0xC)=uint32(uintptr_t(cur)); reg2(cur,o+0xC); W(0x10)=uint32(uintptr_t(P)); reg2(P,o+0x10);
                        W(8)= next()%3==0 ? 0u : uint32(uintptr_t(Fake(0))); At<int32>(Fake(0),FK_TYPE)= next()%2 ? TASK_COMPLEX_KILL_PED_ON_FOOT : TASK_SIMPLE_PAUSE;
                        // companero en vehiculo con su tarea KillCriminal real (objeto 2)
                        if(At<uint32>(P,COP_PARTNER)&&next()%2){
                            At<uint32>(N,P_FLAGS)|=0x100u; At<CTask*>(intels[I_N].data(),I_PRIMARY+4*3)=reinterpret_cast<CTask*>(object2.data());
                            *reinterpret_cast<uintptr_t*>(object2.data())=0x870A00; At<uint32>(object2.data(),4)=0; At<uint32>(object2.data(),8)=0;
                            At<uint32>(object2.data(),0xC)= next()%2 ? uint32(uintptr_t(X[0])) : 0u; reg2(reinterpret_cast<CEntity*>(At<uint32>(object2.data(),0xC)),object2.data()+0xC);
                            At<uint32>(object2.data(),0x10)=uint32(uintptr_t(N)); reg2(N,object2.data()+0x10);
                            At<uint32>(object2.data(),0x14)=0x40400000u; At<uint8>(object2.data(),0x19)=1; At<uint8>(object2.data(),0x1A)=uint8(next()%2);
                            At<CPed*>(N,COP_PARTNER)=nullptr; At<uint8>(N,COP_DRIVER)=uint8(next()%2);
                        } else if(At<uint32>(P,COP_PARTNER)) At<uint32>(N,P_FLAGS)|= next()%2 ? 0x100u : 0u;
                        pedArg=pedArg2; break; }
                    case KC_ABORT: {
                        vtObject=0x870A00; world(true);
                        // la ranura 0 del cop siempre ocupada por un ped vivo; la ultima libre (AddCriminalToKill nunca devuelve 0 ni lista llena)
                        At<float>(X[2],P_HEALTH)=uni(1.f,100.f); At<uint32>(X[2],P_FLAGS)&=~0x100u; At<int32>(X[2],P_TYPE)=4; At<uint8>(X[2],P_CREATED)=0;
                        for(int s=0;s<5;++s)if(At<CPed*>(P,COP_CRIMS+4*s))CEntity::ClearReference(At<CPed*>(P,COP_CRIMS+4*s));
                        At<CPed*>(P,COP_CRIMS)=X[2]; reg2(X[2],static_cast<uint8*>(static_cast<void*>(P))+COP_CRIMS);
                        CPed* l1[]{nullptr,K,X[0],X[1]};
                        for(int s=1;s<4;++s){ CPed* c=l1[next()%4]; At<CPed*>(P,COP_CRIMS+4*s)=c; reg2(c,static_cast<uint8*>(static_cast<void*>(P))+COP_CRIMS+4*s); }
                        place(reinterpret_cast<CEntity*>(tgt.data()),M_T,CVector{uni(-30.f,30.f),uni(-30.f,30.f),0.f},ENTITY_TYPE_VEHICLE);
                        const uintptr_t srcs[]{0,uintptr_t(K),uintptr_t(X[0]),uintptr_t(X[1]),uintptr_t(N),uintptr_t(tgt.data()),uintptr_t(K),uintptr_t(X[1])};
                        eventSource=srcs[next()%8];
                        static const int types[]{EVENT_DAMAGE,EVENT_SHOT_FIRED,EVENT_GUN_AIMED_AT,EVENT_ACQUAINTANCE_PED_HATE,EVENT_VEHICLE_DAMAGE_WEAPON,EVENT_VEHICLE_DAMAGE_COLLISION,
                                                 EVENT_DAMAGE,EVENT_VEHICLE_DAMAGE_WEAPON,0,5,66,EVENT_IN_WATER};
                        eventType=types[next()%12]; eventPrio=int(next()%100);
                        W(8)=uint32(uintptr_t(Fake(0))); At<int32>(Fake(0),FK_TYPE)=TASK_COMPLEX_KILL_PED_ON_FOOT;
                        CPed* cur= next()%5==0 ? nullptr : K; W(0xC)=uint32(uintptr_t(cur)); reg2(cur,o+0xC);
                        if(next()%6){ W(0x10)=uint32(uintptr_t(P)); reg2(P,o+0x10); } else W(0x10)=0u;   // sin cop: no dejar 0xCDCDCDCD (el EXE lo desreferencia)
                        // distancia criminal-peaton en la frontera de 25 (d^2 = 625) o alrededor
                        if(cur&&next()%3==0){ const auto pp=P->GetPosition(); static const float ds[]{25.f,24.9999f,25.0001f,24.f,26.f,0.f};
                            setpos(cur,CVector{pp.x+ds[next()%6],pp.y,pp.z}); }
                        static const int ps[]{0,1,1,2,1,2,0,1}; prio=ps[next()%8]; evArg= next()%8==0 ? nullptr : event.data();
                        At<int32>(event.data(),4)=int32(next()%100); pedArg=P; break; }
                    case KC_NEXT: {
                        vtObject=0x870A00; world(true);
                        static const int sts[]{TASK_SIMPLE_GANG_DRIVEBY,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_ENTER_CAR_AS_DRIVER,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_LEAVE_CAR,
                                               TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_CAR_DRIVE_MISSION,TASK_FINISHED,TASK_SIMPLE_PAUSE,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_ENTER_CAR_AS_DRIVER};
                        const int stt=sts[next()%11]; W(8)=uint32(uintptr_t(Fake(0))); At<int32>(Fake(0),FK_TYPE)=stt;
                        CPed* cands[]{K,K,K,X[0],nullptr};
                        CPed* cur=cands[next()%5]; if(stt==TASK_SIMPLE_GANG_DRIVEBY&&!cur)cur=K;
                        W(0xC)=uint32(uintptr_t(cur)); reg2(cur,o+0xC); W(0x10)=uint32(uintptr_t(P)); reg2(P,o+0x10);
                        *reinterpret_cast<float*>(o+0x14)=3.f; o[0x18]=uint8(next()%2); o[0x19]=uint8(next()%2); o[0x1A]=uint8(next()%2); for(size_t k=0x1B;k<0x20;++k)o[k]=uint8(next());
                        if(next()%3==0){ const auto pp=P->GetPosition(); static const float ds[]{25.f,24.9999f,25.0001f,24.f,26.f}; // frontera de 25 al coche o al peaton
                            if(cur){ setpos(cur,CVector{pp.x+ds[next()%5],pp.y,pp.z}); } }
                        if(next()%4==0&&cur){ const auto vp=VA->GetPosition(); static const float ds[]{25.f,24.9999f,25.0001f}; setpos(cur,CVector{vp.x+ds[next()%3],vp.y,vp.z}); }
                        pedArg=P; break; }
                    case KA_CTOR: {
                        world(false); pedArg= next()%5==0 ? nullptr : K; a0=next()%8; a1=next()%5000; a2=next()%101; a4=int8(int(next()%9)-3); break; }
                    case KA_DTOR: {
                        vtObject=0x86D918; world(false);
                        W(0xC)= next()%4==0 ? 0u : uint32(uintptr_t(K)); reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC);
                        W(8)=next()%2 ? 0u : uint32(uintptr_t(chain(1+int(next()%2),TASK_SIMPLE_PAUSE))); for(size_t k=0x10;k<0x5C;k+=4)W(k)=next(); break; }
                    case KA_CLONE: {
                        vtObject=0x86D918; world(false);
                        W(0xC)= next()%4==0 ? 0u : uint32(uintptr_t(K)); reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC);
                        for(size_t k=0x10;k<0x5C;k+=4)W(k)=next(); break; }
                    case KA_ABORT: {
                        vtObject=0x86D918; world(false);
                        W(8)=uint32(uintptr_t(Fake(0))); At<int32>(Fake(0),FK_TYPE)=TASK_SIMPLE_GUN_CTRL;
                        W(0xC)= next()%5==0 ? 0u : uint32(uintptr_t(K)); reg2(reinterpret_cast<CEntity*>(W(0xC)),o+0xC);
                        for(size_t k=0x10;k<0x5C;k+=4)W(k)=next();
                        eventType= next()%3 ? EVENT_GUN_AIMED_AT : int(next()%80); eventPrio=int(next()%100);
                        CPed* aim[]{K,X[0],nullptr,P}; At<CPed*>(event.data(),0x14)=aim[next()%4];
                        static const int ps[]{1,1,1,2,0,3,-1,2}; prio=ps[next()%8]; evArg= next()%6==0 ? nullptr : event.data(); pedArg=P; break; }
                    case KA_LEADER: {
                        vtObject=0x86D918; world(false);
                        At<int32>(P,P_TYPE)=ktypes[next()%14]; mk(L,M_L,I_L,CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)},next()%5 ? 0 : 1,50.f,next()%2);
                        place(reinterpret_cast<CEntity*>(tgt.data()),M_T,CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,10.f)},next()%2 ? ENTITY_TYPE_PED : ENTITY_TYPE_VEHICLE);
                        const auto lp=L->GetPosition(); const auto tp=reinterpret_cast<CEntity*>(tgt.data())->GetPosition();
                        // peaton cerca del rayo lider->objetivo: proyeccion t (puede ser negativa) y desviacion lateral alrededor de 2
                        CVector dir=tp-lp; dir.z=0.f; dir.Normalise(); const CVector lat{-dir.y,dir.x,0.f};
                        static const float lats[]{0.f,1.f,1.9999f,2.f,2.0001f,3.f,0.5f,-1.5f};
                        static const float ts[]{5.f,9.f,9.99f,10.f,10.01f,-1.f,0.f,20.f,3.f};
                        const float lt=lats[next()%8], tt=next()%3 ? ts[next()%9] : uni(-5.f,15.f);
                        setpos(P,CVector{lp.x+dir.x*tt+lat.x*lt,lp.y+dir.y*tt+lat.y*lt,lp.z+uni(-2.f,2.f)});
                        At<uint32>(L,P_TARGETED)= next()%6==0 ? 0u : uint32(uintptr_t(tgt.data())); reg2(reinterpret_cast<CEntity*>(At<uint32>(L,P_TARGETED)),static_cast<uint8*>(static_cast<void*>(L))+P_TARGETED);
                        weapon(L,wtypes[next()%20],int(next()%13));
                        At<uint32>(grp.data(),0x28)= next()%6==0 ? 0u : uint32(uintptr_t(L));
                        groupPed= next()%6==0 ? 0 : uintptr_t(P); groupRet=uintptr_t(grp.data());
                        W(8)=0; W(0xC)=uint32(uintptr_t(K)); pedArg=P; break; }
                    case KA_LOS: {
                        vtObject=0x86D918; world(false);
                        At<int32>(P,P_TYPE)=ktypes[next()%14]; At<int32>(K,P_TYPE)= next()%3==0 ? 0 : ktypes[next()%14];
                        const uint32 now=CTimer::GetTimeInMS();
                        P->SetAreaCode(next()%4==0 ? AREA_CODE_1 : AREA_CODE_NORMAL_WORLD);
                        currArea= next()%3==0 ? 1 : 0; procLevel=int32(next()%4); At<uint8>(P,P_CREATED)= next()%3==0 ? 2 : uint8(next()%3);
                        if(next()%3==0)At<uint32>(P,P_FLAGS)|=0x4000000u; else At<uint32>(P,P_FLAGS)&=~0x4000000u;
                        W(0xC)=uint32(uintptr_t(K)); reg2(K,o+0xC);
                        static const uint32 deltas[]{0,1,2000,4999,5000,5001,7000,9999,10000,10001,20000};
                        W(0x54)= next()%3==0 ? 0u : now-deltas[next()%11]; W(0x50)= next()%3==0 ? 0u : now-deltas[next()%11];
                        const auto pp=P->GetPosition(), kp=K->GetPosition();
                        static const float mv[]{0.f,1.f,1.999f,2.f,2.001f,2.9f,3.f,3.001f,5.f};
                        auto jig=[&](CVector c){ return CVector{c.x+mv[next()%9]*float(int(next()%3)-1),c.y+mv[next()%9]*float(int(next()%3)-1),c.z}; };
                        const CVector sOur=next()%2 ? jig(pp) : pp, sTgt=next()%2 ? jig(kp) : kp;
                        *reinterpret_cast<CVector*>(o+0x38)=sOur; *reinterpret_cast<CVector*>(o+0x44)=sTgt; At<uint32>(VB,0x1C)=next();
                        if(At<uint32>(K,P_FLAGS)&0x100u&&!At<uint32>(K,P_VEH))At(K,P_VEH)=uint32(uintptr_t(vehB.data()));
                        pedArg=P; break; }
                    case KA_FIRST: case KA_CONTROL: {
                        vtObject=0x86D918; world(false);
                        At<int32>(P,P_TYPE)=ktypes[next()%14]; At<int32>(K,P_TYPE)= next()%4==0 ? 0 : ktypes[next()%14]; At<float>(K,P_HEALTH)= next()%8==0 ? hp() : uni(1.f,100.f);
                        P->SetAreaCode(AREA_CODE_NORMAL_WORLD); At<uint32>(P,P_FLAGS)&=~0x4600000u;
                        if(next()%3==0)At<uint32>(P,P_FLAGS)|=0x200000u; if(next()%4==0)At<uint32>(P,P_FLAGS)|=0x400000u; if(next()%4==0)At<uint32>(P,P_FLAGS)|=0x4000000u;
                        const uint32 now=CTimer::GetTimeInMS();
                        // distancia: alrededor de las fronteras del rango del arma (peaton en el origen, objetivo en +x)
                        const float range=reinterpret_cast<CPed*>(P)->GetActiveWeapon().GetWeaponInfo(P).m_fTargetRange;
                        static const float frac[]{0.5f,0.25f,1.f,0.8f,0.6f,0.f,0.1f};
                        float d;
                        switch(next()%9){ case 0: d=4.f; break; case 1: d=std::nextafter(4.f,100.f); break; case 2: d=std::nextafter(4.f,0.f); break; case 3: d=6.f; break; case 4: d=std::nextafter(6.f,100.f); break;
                                          case 5: d=30.f; break; case 6: d=std::nextafter(30.f,100.f); break; case 7: d=range*frac[next()%7]; break; default: d=uni(0.5f,40.f); }
                        const CVector pp{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,10.f)};
                        setpos(P,pp); setpos(K,CVector{pp.x+d,pp.y,pp.z});
                        W(0xC)=uint32(uintptr_t(K)); reg2(K,o+0xC); W(0x1C)=next()&7; W(0x20)=next()%3000; W(0x24)=next()%101; W(0x28)=now-(next()%3 ? next()%6000 : 2000u+next()%3);
                        o[0x30]=uint8(int8(int(next()%4)-1)); o[0x31]=uint8(next()%5); o[0x32]=uint8(next()%2); o[0x33]=uint8(next()%2); o[0x34]=uint8(next()%8==0); o[0x35]=uint8(next()%2);
                        W(0x10)=now+uint32(int(next()%5)-2); W(0x14)=now-(next()%3 ? next()%4000 : 2500u+next()%3-1); W(0x18)=now-(next()%3 ? next()%6000 : 3000u+next()%3-1);
                        W(0x2C)=now+uint32(int(next()%5)-2); W(0x50)=0; W(0x54)=0; W(0x58)=0;
                        coverRet= next()%2 ? uintptr_t(cover.data()) : 0; fcRet=true; fcCoords=CVector{pp.x+uni(-5.f,5.f),pp.y+uni(-5.f,5.f),pp.z};
                        if(kind==KA_FIRST){
                            if(next()%2)At<uint32>(P,P_COVER)=uint32(uintptr_t(cover.data()));
                        } else {
                            static const int sts[]{TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_GUN_CTRL,TASK_COMPLEX_SEEK_ENTITY,TASK_SIMPLE_PAUSE};
                            W(8)=uint32(uintptr_t(Fake(0))); At<int32>(Fake(0),FK_TYPE)=sts[next()%5]; At<uint8>(fakes[0].data(),0x2E)=uint8(next()%8); At<uint32>(fakes[0].data(),0x34)=next();
                            if(next()%2){ At<CTask*>(intels[I_P].data(),I_SECONDARY)=Quiet(0); At<int32>(Quiet(0),FK_TYPE)=TASK_SIMPLE_USE_GUN; }
                            if(next()%2){ At<CTask*>(intels[I_P].data(),I_SECONDARY+4)=Quiet(1); At<int32>(Quiet(1),FK_TYPE)=TASK_SIMPLE_DUCK; At<uint8>(Quiet(1),0x1B)=uint8(next()%2); }
                            At<uint32>(K,0x40)= next()%5==0 ? 0x100u : 0u;   // bSubmergedInWater del objetivo (CPhysical +0x40, bit 8)
                            *reinterpret_cast<CVector*>(static_cast<uint8*>(static_cast<void*>(K))+P_MOVESPEED)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f};
                            CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=pp; P->m_matrix=reinterpret_cast<CMatrixLink*>(mats[M_P].data()); std::memcpy(mats[M_P].data(),&m,sizeof(CMatrix));
                            setpos(K,CVector{pp.x+d,pp.y,pp.z});
                            groupPed= next()%3==0 ? uintptr_t(P) : 0; groupRet=uintptr_t(grp.data()); At<uint32>(grp.data(),0x28)= next()%2 ? uint32(uintptr_t(L)) : 0u;
                            mk(L,M_L,I_L,CVector{pp.x+uni(-5.f,5.f),pp.y+uni(-5.f,5.f),pp.z},0,50.f,false); At<uint32>(L,P_TARGETED)=uint32(uintptr_t(tgt.data()));
                            place(reinterpret_cast<CEntity*>(tgt.data()),M_T,CVector{pp.x+uni(-8.f,8.f),pp.y+uni(-8.f,8.f),pp.z},ENTITY_TYPE_PED); weapon(L,wtypes[next()%20],int(next()%13));
                        }
                        pedArg=P; break; }
                    case PF_PROCESS: {
                        vtObject=0x8708EC; world(false);
                        static const int pt[]{0,0,0,1,3,4,5};
                        At<int32>(P,P_TYPE)=pt[next()%7]; At<int32>(P,0x534)=int(next()%9);   // m_nMoveState (PEDMOVE_WALK = 4)
                        if(next()%3==0)At<uint32>(P,P_FLAGS)|=0x4000000u; else At<uint32>(P,P_FLAGS)&=~0x4000000u;
                        weapon(P,next()%2 ? 9 : wtypes[next()%20],int(next()%13));
                        static const int ats[]{TASK_SIMPLE_FIGHT,TASK_SIMPLE_USE_GUN,TASK_SIMPLE_PAUSE,0};
                        const int at=ats[next()%4];
                        if(at){ At<CTask*>(intels[I_P].data(),I_SECONDARY)=Quiet(0); At<int32>(Quiet(0),FK_TYPE)=at;
                            if(at==TASK_SIMPLE_FIGHT)At<uint8>(Quiet(0),0x25)= next()%2 ? 4 : uint8(next()%9);
                            else if(at==TASK_SIMPLE_USE_GUN){ At<uint32>(Quiet(0),0x30)= next()%5==0 ? 0u : uint32(uintptr_t(cover.data())); At<uint32>(cover.data(),0x18)=next(); } }   // cover hace de CWeaponInfo falso (banderas en +0x18)
                        W(0xC)=next(); pedArg=P; break; }
                    case PF_CTOR: { world(false); break; }
                    case PF_DTOR: { vtObject=0x8708EC; for(size_t k=4;k<0x1C;k+=4)W(k)=next(); break; }
                    case PF_CLONE: { vtObject=0x8708EC; for(size_t k=4;k<0x1C;k+=4)W(k)=next(); break; }
                    case PF_ABORT: {
                        vtObject=0x8708EC; world(false);
                        P->SetType(ENTITY_TYPE_PED); At(P,P_PDATA)=uint32(uintptr_t(pdata.data())); At<float>(pdata.data(),D_BLEND)=uni(0.f,3.f); At<uint32>(pdata.data(),D_FLAGS)=next()%3==0 ? 8u : next();
                        At<uint32>(P,P_CLUMP)=uint32(uintptr_t(fakeClump.data()));   // clump falso con tipo rpCLUMP (2) en +0: GetRpClump de la DLL lo comprueba
                         At<uint32>(P,P_ANIMGROUP)=next()%40; At<uint32>(P,P_ATTACHED)= next()%4==0 ? uint32(uintptr_t(VA)) : 0u;
                        At<uint32>(P,P_TARGETED)= next()%2 ? uint32(uintptr_t(K)) : 0u; reg2(reinterpret_cast<CEntity*>(At<uint32>(P,P_TARGETED)),static_cast<uint8*>(static_cast<void*>(P))+P_TARGETED);
                        camFirst=next()%3==0; At<int32>(P,P_TYPE)=int(next()%3);
                        CTask* sec= next()%4==0 ? nullptr : Fake(0);
                        if(sec)At<int32>(sec,FK_TYPE)= next()%3==0 ? 1018 /*TASK_SIMPLE_THROW_PROJECTILE*/ : next()%2 ? TASK_SIMPLE_FIGHT : TASK_SIMPLE_PAUSE;
                        At<CTask*>(intels[I_P].data(),I_SECONDARY)=sec;
                        // evento: tipo, prioridad y banderas de dano (+0x25 caida, +0x40 vida a cero, +0x0C al grupo, +0x1C arma)
                        static const int types[]{EVENT_DAMAGE,EVENT_DAMAGE,EVENT_IN_WATER,EVENT_SHOT_FIRED,0,EVENT_DAMAGE,EVENT_DAMAGE,5};
                        eventType=types[next()%8]; static const int prs[]{0,30,60,61,62,100,-4}; eventPrio=prs[next()%7];
                        At<uint8>(event.data(),0x25)=uint8(next()%2 ? 2 : next()%2); At<uint8>(event.data(),0x40)=uint8(next()%2); At<uint8>(event.data(),0x0C)=uint8(next()%2);
                        static const int wts[]{0,22,46,47,48,51,52,53,54,9}; At<int32>(event.data(),0x1C)=wts[next()%10];
                        static const int ps[]{0,1,1,2,1,2,3,-1}; prio=ps[next()%8]; evArg= next()%6==0 ? nullptr : event.data(); pedArg=P; break; }
                    }

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t target= fn.vt ? vt[fn.slot] : fn.address;
                    // despues de State(), no antes
                    const bool switches= fn.vt ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    if(vtObject)*reinterpret_cast<uintptr_t*>(o)=vtObject;
                    void* self=o; uintptr_t rawRet=0; json ret=nullptr;
                    uint16 fpuTag=0xFFFF; unsigned fpAfter=0;
                    {
                        _controlfp(_PC_24,_MCW_PC);                                   // x87 a 24 bits como el juego
                        switch(kind){
                        case KC_CTOR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,bool)>(target)(self,pedArg,boolArg)); break;
                        case KA_CTOR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,uint32,uint32,uint32,int8)>(target)(self,pedArg,a0,a1,a2,a4)); break;
                        case PF_CTOR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(self)); break;
                        case KC_DTOR: case KA_DTOR: case PF_DTOR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(self)); break;
                        case KC_CLONE: case KA_CLONE: case PF_CLONE: rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self)); break;
                        case KC_CREATE: rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int,CPed*,bool)>(target)(self,ttArg,pedArg,boolArg)); break;
                        case KC_FINDNEXT: rawRet=uintptr_t(reinterpret_cast<CPed*(__thiscall*)(void*,CPed*,bool)>(target)(self,pedArg,boolArg)); break;
                        case KC_CHANGE: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(self,pedArg); break;
                        case KC_ABORT: case KA_ABORT: case PF_ABORT: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(self,pedArg,prio,evArg); break;
                        case KC_NEXT: case KA_FIRST: case KA_CONTROL: rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,pedArg)); break;
                        case KA_LEADER: case KA_LOS: case PF_PROCESS: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(self,pedArg); break;
                        }
                        fpuTag=FpuTag(); fpAfter=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                    }
                    for(auto* h:hooks)h->State(false);
                    currArea=savedArea; procLevel=savedProc;
                    (void)a3; (void)callOnObject2; (void)pedArg2; (void)a0;
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null())ret=Name(rawRet);
                    // Constructores: el EXE y la DLL escriben su propia vptr -> se compara el tipo. Destructores: la vptr final (vtable de la base
                    // del EXE o de la DLL) y el retorno (EAX sin definir) no son comparables -> se enmascaran.
                    const bool isCtorK=kind==KC_CTOR||kind==KA_CTOR||kind==PF_CTOR, isDtorK=kind==KC_DTOR||kind==KA_DTOR||kind==PF_DTOR;
                    json typeOrVptr=isDtorK ? json(nullptr) : isCtorK ? json(int(reinterpret_cast<CTask*>(o)->GetTaskType())) : json(At<uint32>(o,0));
                    if(isDtorK)ret=nullptr;
                    json tasks=json::array(); for(auto* t:fresh){ tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x70,true)}}); }
                    const json objectWords=Region(o,object.size(),true);   // el objeto probado completo
                    auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    json refs=json::array(); for(const auto& b:bufs)if(b.ent)refs.push_back(refList(reinterpret_cast<CEntity*>(b.p)));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),FK_SIZE,true));
                    for(int n=0;n<QUIET;++n)fakeWords.push_back(Region(quiet[n].data(),FK_SIZE,true));
                    json ents=json::array(); for(const auto& b:bufs)ents.push_back(b.ent ? EntHash(b) : (b.p==object.data() ? std::string("-") : Hash(b.p,b.n)));
                    json r={{"ret",ret},{"vptr",typeOrVptr},{"object",objectWords},{"object2",Region(object2.data(),object2.size(),false)},{"tasks",tasks},{"fakes",fakeWords},{"refs",refs},{"buffers",ents},
                            {"fp",fpAfter},{"fpu_tag",fpuTag},{"curr_area",currArea},{"proc_level",procLevel}};
                    // borrado de las tareas creadas
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    json refsAfter=json::array(); for(const auto& b:bufs)if(b.ent)refsAfter.push_back(refList(reinterpret_cast<CEntity*>(b.p)));
                    r["refs_after"]=refsAfter;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["rng"]=randIdx; r["clean"]=clean; r["switches"]=switches;
                    CleanRefs();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true&&original["fpu_tag"]==0xFFFF;
                passed&=ok; failures+=!ok;
                hist[original["ret"].dump()]++;
                auto digest=[](const json& o){ return Fnv(o.dump()); };
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                             {"original",{{"ret",original["ret"]},{"trace",original["trace"]},{"state_fnv",digest(original)}}},{"candidate",{{"ret",candidate["ret"]},{"trace",candidate["trace"]},{"state_fnv",digest(candidate)}}},
                             {"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"return_histogram",hist}});
        }
        patches.clear();   // restaura las entradas
    }
    CleanRefs();
    *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC)=savedLastFree; currArea=savedArea; procLevel=savedProc;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&_controlfp(0,0)==savedFP&&currArea==savedArea&&procLevel==savedProc;
    for(auto& [a,b]:entryBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(a),8);
    for(auto& [a,v]:vtBefore)restored&=*reinterpret_cast<uintptr_t*>(a)==v;   // huecos de vtable como estaban al empezar
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
