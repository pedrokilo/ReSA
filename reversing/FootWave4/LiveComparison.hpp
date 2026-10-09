#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1/TaskWave2A, partida cargada via FastLoader): deja COMPLETA la clase CTaskSimplePlayerOnFoot con sus
// 5 funciones pendientes: PlayIdleAnimations 0x6872C0, PlayerControlZeldaWeapon 0x687C20, PlayerControlDucked 0x687F30, PlayerControlZelda 0x6883D0
// y ProcessPlayerWeapon 0x6859A0 (las otras 5 de la clase las cubrio TaskWave2A: Constructor, Destructor, Clone, MakeAbortable y ProcessPed).
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo: fuente de la DLL) con fixtures identicos y deterministas.
// Jugador, objetivos, grupo, inteligencias, datos de jugador, pad, armas, asociaciones de animacion, bloques/grupos de animacion y tareas viven en
// buffers propios con desplazamientos crudos del EXE. Colaboradores simulados y trazados (mismo comportamiento en la direccion del EXE y en el
// simbolo de la DLL, solo en el hilo del banco): rand 0x821B1E y CGeneral::GetRandomNumber, CPad (todas las consultas), CCamera (Using1stPersonWeaponMode,
// ClearPlayerWeaponMode, SetNewPlayerWeaponMode, UpdateAimingCoors), CWeaponEffects::ClearCrossHair, CPed::Say/GetWeaponSkill/SetPedState,
// CPlayerPed (GetPadFromPlayer, ClearWeaponTarget, Clear3rdPersonMouseTarget, PlayerHasJustAttackedSomeone, DoesTargetHaveToBeBroken,
// PedCanBeTargettedVehicleWise, FindWeaponLockOnTarget, FindNextWeaponLockOnTarget, Compute3rdPersonMouseTarget, ControlButtonSprint, SetRealMoveAnim),
// CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation/GetFirstAssociation/GetNextAssociation, CAnimBlendAssocGroup::GetAnimation, CStreaming
// (RequestModel, SetModelIsDeletable), CStats::GetStatValue, CGameLogic (IsPlayerUse2PlayerControls, IsPlayerAllowedToGoInThisDirection),
// CLocalisation::KickingWhenDown, CPedIntelligence (SetTaskDuckSecondary, ClearTaskDuckSecondary, TestForStealthKill, IsInSeeingRange,
// GetActivePrimaryTask), CTaskManager (SetTask, SetTaskSecondary), CTaskSimpleUseGun (RequirePistolWhip, ControlGun, PlayerPassiveControlGun, AbortIK,
// ControlGunMove), CTaskSimpleFight::ControlFight, CTaskSimpleThrowProjectile::ControlThrow, CTaskSimpleDuck (CanPedDuck, IsTaskInUseByOtherTasks,
// ControlDuckMove), CPedGroups (GetPedsGroup, AreInSameGroup), CPedGroupIntelligence::AddEvent, CEventGroup::Add, CEventDamage::AffectsPed,
// CPedDamageResponseCalculator::ComputeDamageResponse, CCrime::ReportCrime, CAEPedWeaponAudioEntity::AddAudioEvent, CWeapon (Fire, IsTypeMelee),
// IKChainManager_c (IsLooking, GetLookAtEntity, AbortLookAt, LookAt), SurfaceInfos_c::CantSprintOn.
// Se ejecutan de verdad (puros o validados): CPedIntelligence::GetTaskUseGun/GetTaskFighting/GetTaskDuck/GetTaskThrow/GetTaskHold, CTaskManager::
// GetTaskSecondary/GetActiveTask/GetSimplestActiveTask, CWeaponInfo::GetWeaponInfo/GetCrouchReloadAnimationID, CGeneral::GetRadianAngleBetweenPoints/
// LimitRadianAngle, CGameLogic::IsCoopGameGoingOn, CPlaceable::GetForward, CVector, los constructores/destructores de las tareas creadas (UseGun,
// Fight, StealthKill, ThrowProjectile, ComplexJump, UseGoggles) y los objetos de pila CPedDamageResponseCalculator / CEventDamage / CEventGunAimedAt /
// CEventGroupEvent (solo constructor y destructor).
// Globales tocados temporalmente y restaurados tras cada ejecucion: CWorld::Players[0/1].m_pPed, gLastTouchTimeDelta, gLastRandomNumberForIdleAnimationID,
// gbUnknown_8D2FE8, gDuckAnimBlendData, campos de TheCamera (modo y frente de las 3 camaras, camara activa, orientacion, pantalla ancha, raton en tercera
// persona, salto desde primera persona), 4 bloques de CAnimManager::ms_aAnimBlocks y el puntero CAnimManager::ms_aAnimAssocGroups (se intercambia por
// una tabla falsa).
// Se compara: retorno, memoria completa del objeto probado, tareas nuevas del pool (tipo + campos fijados por los argumentos del constructor), memoria
// de las tareas falsas, huella de todos los buffers (referencias enmascaradas), listas de referencias antes y despues del borrado, traza completa (orden y
// numero de llamadas, argumentos y retornos de todos los colaboradores), huella de los globales tocados, palabra de control x87 y pila x87 vacia.
// El x87 corre a 24 bits como el juego.
#include <fstream>
#include <unordered_map>
#include <map>
#include <float.h>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "TaskSimplePlayerOnFoot.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGunControl.h"
#include "TaskSimpleFight.h"
#include "TaskSimpleThrowProjectile.h"
#include "TaskSimpleHoldEntity.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "World.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace FootWave4Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const std::string& s,uint64 h=1469598103934665603ull){ for(unsigned char c:s){h^=c;h*=1099511628211ull;} return h; }
// Hilo del banco: otros hilos del juego (audio, streaming) llaman a funciones parcheadas; no se trazan ni tocan el estado compartido.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

constexpr int FAKES=4, QUIET=6;
constexpr size_t FK_TYPE=0x50, FK_ABORT=0x54, FK_SUB=0x58, FK_SIZE=0x60;   // configuracion por objeto dentro de cada tarea falsa
// CPed / CPlayerPed (EXE)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_CONTACT=0xBA, P_ATTACHED=0xFC, P_ANIMGROUP=0x4D4, P_FLAGS=0x46C, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C,
                 P_PDATA=0x480, P_IKFLAGS=0x528, P_PEDSTATE=0x530, P_MOVESTATE=0x534, P_SWIM=0x538, P_AIMROT=0x55C, P_STANDING=0x568, P_TYPE=0x598, P_WEAPONS=0x5A0,
                 P_WSLOT=0x718, P_ACCURACY=0x71A, P_TARGETED=0x71C, P_FIRE=0x730, P_MOUSE3=0x79C;
constexpr size_t I_PRIMARY=0x4, I_SECONDARY=0x18, I_EVENTS=0x68;        // CPedIntelligence: m_TaskMgr en +4 (primarias +4, secundarias +0x18), grupo de eventos en +0x68
constexpr size_t D_FIGHT=0xC, D_BLEND=0x14, D_CHOSEN=0x20, D_ATTACK=0x2C, D_FLAGS=0x34, D_PITCH=0x54, D_SPRINTOFF=0x84, D_NOWCHANGE=0x85, D_HSTIME=0x94, D_HSTARGET=0x98;
constexpr size_t PAD_WALK=0x2A, PAD_DISABLE=0x10E;
constexpr uintptr_t G_PLAYERS0=0xB7CD98, G_PLAYERS1=0xB7CF28, G_IDLEBLOCKS=0xB5D4A0, G_ASSOCGROUPS=0xB4EA34, G_NUMDEFS=0xB4EA28, G_NUMANIMS=0xB4EA2C,
                    G_NUMBLOCKS=0xB4EA30, G_ANIMS=0xB4EA40, G_CAM=0xB6F028, G_CAMS=0xB6F1A8, G_MOUSE3=0xB6EC2E, G_JUMPED=0xB6F048, G_WIDE=0xB6F065,
                    G_ACTIVECAM=0xB6F081, G_ORIENT=0xB6F178, G_LASTRAND=0x8D2FEC, G_LASTTOUCH=0xC19664, G_DUCKBLEND=0x8D2FF0, G_UNKNOWN=0x8D2FE8;
constexpr size_t CAM_SIZE=0x238, CAM_FRONT=0x184;

alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> plBuf{};
alignas(16) inline std::array<uint8,sizeof(CPed)> tpBuf{}, opBuf{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> tvBuf{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,4> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,3> intels{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdata{};
alignas(16) inline std::array<uint8,sizeof(CPad)> padBuf{};
alignas(16) inline std::array<uint8,0x300> grp{};                      // CPedGroup falso: m_groupIntelligence en +0x30
alignas(16) inline std::array<uint8,0x100> winfo{};                    // CWeaponInfo falso de las tareas UseGun falsas (banderas en +0x18)
alignas(16) inline std::array<uint8,0x40> fakeClump = []{ std::array<uint8,0x40> a{}; a[0]=2; return a; }();   // tipo rpCLUMP en +0
alignas(16) inline std::array<std::array<uint8,0x40>,6> assocs{};      // CAnimBlendAssociation falsas
alignas(16) inline std::array<std::array<uint8,0x20>,2> sassocs{};     // CAnimBlendStaticAssociation falsas
alignas(16) inline std::array<std::array<uint8,0x20>,4> fblocks{};     // CAnimBlock falsos
inline std::vector<uint8> fgroups;                                     // tabla de CAnimBlendAssocGroup falsos (0x14 por grupo)
alignas(16) inline std::array<std::array<uint8,FK_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,FK_SIZE>,QUIET> quiet{};
alignas(16) inline std::array<uint8,0x100> object{};
enum { M_PL, M_TP, M_OP, M_TV };
enum { I_PL, I_TP, I_OP };
inline CPed* Pl(){ return reinterpret_cast<CPed*>(plBuf.data()); }
inline CPed* Tp(){ return reinterpret_cast<CPed*>(tpBuf.data()); }
inline CPed* Op(){ return reinterpret_cast<CPed*>(opBuf.data()); }
inline CEntity* Tv(){ return reinterpret_cast<CEntity*>(tvBuf.data()); }
inline CTask* Fake(int n){ return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CTask* Quiet(int n){ return reinterpret_cast<CTask*>(quiet[n].data()); }
template<class T=uint32> T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> T& Gl(uintptr_t a){ return *reinterpret_cast<T*>(a); }

struct Buf { const char* name; uint8* p; size_t n; bool ent; };
inline std::vector<Buf> bufs;
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(const auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.name)+"+"+std::to_string(v-uintptr_t(b.p));
    if(v>=uintptr_t(fakeClump.data())&&v<uintptr_t(fakeClump.data())+fakeClump.size())return "clump+"+std::to_string(v-uintptr_t(fakeClump.data()));
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

// ---- configuracion por caso de los colaboradores simulados
enum { PB_DUCK, PB_SPRINT, PB_JUMP, PB_EXIT, PB_TARGET, PB_ENTER, PB_SHIFTL, PB_SHIFTR, PB_COUNT };
struct Cfg {
    int padB[PB_COUNT]{};
    int walkUD{}, walkLR{}, walkUDp{}, walkLRp{}, aimLR{}, aimUD{}, getWeapon{}, meleeJD[2]{}, getMelee{}, skill{1}, assocCount{};
    bool weaponJD{}, use2P{}, camFirst{}, canDuck{}, inUse{}, pistolWhip{}, kicking{}, canVeh{}, hasToBreak{}, sameGroup{}, affects{}, seeing{}, lockOn{}, justAtt{}, surfCant{},
         ikLooking{}, fireRet{}, sameHier{}, stealth[2]{}, allow[4]{};
    unsigned allowIdx{}, blendCount{};
    uint32 touchDelta{};
    float stat{}, sprint{};
    uintptr_t group{}, groupPed{}, activePrimary{}, lookEnt{}, assocKP{}, assocOther{};
};
inline Cfg cfg;
inline std::array<int,8> randQ{}; inline unsigned randIdx{};

// tareas falsas con vtable trazada: cada objeto lleva su tipo, su respuesta a MakeAbortable y su subtarea
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

// ---- dependencias simuladas (comunes a EXE y DLL salvo indicacion)
inline int __cdecl FakeRand() { if(!Mine())return 0x1234; const int v=randQ[randIdx++%8]; trace.push_back({"rand",v}); return v; }
inline uint16 __cdecl FakeRandU16() { return uint16(FakeRand()); }
// CPad (this = pad falso)
inline const char* const padBN[PB_COUNT]{"pad_duck_just_down","pad_get_sprint","pad_jump_just_down","pad_exit_vehicle_just_down","pad_get_target","pad_get_enter_targeting",
                                         "pad_shift_target_left","pad_shift_target_right"};
template<int ID> bool __fastcall PadB(void*,void*) { if(!Mine())return false; trace.push_back({padBN[ID]}); return cfg.padB[ID]!=0; }
inline int16 __fastcall PadUD(void*,void*) { if(!Mine())return 0; trace.push_back({"pad_walk_up_down"}); return int16(cfg.walkUD); }
inline int16 __fastcall PadLR(void*,void*) { if(!Mine())return 0; trace.push_back({"pad_walk_left_right"}); return int16(cfg.walkLR); }
inline int16 __fastcall PadUDp(void*,void*,CPed* p) { if(!Mine())return 0; trace.push_back({"pad_walk_up_down_ped",Name(uintptr_t(p))}); return int16(cfg.walkUDp); }
inline int16 __fastcall PadLRp(void*,void*,CPed* p) { if(!Mine())return 0; trace.push_back({"pad_walk_left_right_ped",Name(uintptr_t(p))}); return int16(cfg.walkLRp); }
inline void __fastcall PadSetTouched(void*,void*) { if(Mine())trace.push_back({"pad_set_touched"}); }
inline uint32 __fastcall PadTouchDelta(void*,void*) { if(!Mine())return 0; trace.push_back({"pad_touched_time_delta"}); return cfg.touchDelta; }
inline bool __fastcall PadWeaponJD(void*,void*,CPed* p) { if(!Mine())return false; trace.push_back({"pad_weapon_just_down",Name(uintptr_t(p))}); return cfg.weaponJD; }
inline uint8 __fastcall PadMeleeJD(void*,void*,uint32 a) { if(!Mine())return 0; trace.push_back({"pad_melee_attack_just_down",a&0xFF}); return uint8(cfg.meleeJD[a&1]); }
inline bool __fastcall PadGetMelee(void*,void*,uint32 a) { if(!Mine())return false; trace.push_back({"pad_get_melee_attack",a&0xFF}); return cfg.getMelee!=0; }
inline int32 __fastcall PadGetWeapon(void*,void*,CPed* p) { if(!Mine())return 0; trace.push_back({"pad_get_weapon",Name(uintptr_t(p))}); return cfg.getWeapon; }
inline int16 __fastcall PadAimLR(void*,void*,CPed* p) { if(!Mine())return 0; trace.push_back({"pad_aim_weapon_left_right",Name(uintptr_t(p))}); return int16(cfg.aimLR); }
inline int16 __fastcall PadAimUD(void*,void*,CPed* p) { if(!Mine())return 0; trace.push_back({"pad_aim_weapon_up_down",Name(uintptr_t(p))}); return int16(cfg.aimUD); }
// CCamera (this = TheCamera)
inline bool __fastcall Cam1stStub(void*,void*) { if(!Mine())return false; trace.push_back({"camera_using_1st_person",cfg.camFirst}); return cfg.camFirst; }
inline void __fastcall CamClearStub(void*,void*) { if(Mine())trace.push_back({"camera_clear_player_weapon_mode"}); }
inline void __fastcall CamModeStub(void*,void*,int mode,int a,int b) { if(Mine())trace.push_back({"camera_set_new_player_weapon_mode",mode&0xFFFF,a&0xFFFF,b&0xFFFF}); }
inline void __fastcall CamAimStub(void*,void*,const CVector* v) { if(Mine())trace.push_back({"camera_update_aiming_coors",F(v->x),F(v->y),F(v->z)}); }
inline void __cdecl CrossHairStub(uint32 id) { if(Mine())trace.push_back({"clear_cross_hair",id}); }
// CPed
inline int16 __fastcall SayStub(CPed* p,void*,int ctx,uint32 delay,float prob,bool a,bool b,bool c) {
    if(Mine())trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),a,b,c}); return 0; }
inline int __fastcall SkillStub(CPed* p,void*) { if(!Mine())return 1; trace.push_back({"get_weapon_skill",Name(uintptr_t(p))}); return cfg.skill; }
inline void __fastcall PedStateStub(CPed* p,void*,int st) { if(Mine())trace.push_back({"set_ped_state",Name(uintptr_t(p)),st}); }
// CPlayerPed
inline void* __fastcall GetPadStub(CPed* p,void*) { if(!Mine())return nullptr; trace.push_back({"get_pad_from_player",Name(uintptr_t(p))}); return padBuf.data(); }
inline void __fastcall PlClearTarget(CPed* p,void*) { if(Mine())trace.push_back({"clear_weapon_target",Name(uintptr_t(p))}); }
inline void __fastcall PlClear3rd(CPed* p,void*) { if(Mine())trace.push_back({"clear_3rd_person_mouse_target",Name(uintptr_t(p))}); }
inline bool __fastcall PlFindLock(CPed* p,void*) { if(!Mine())return false; trace.push_back({"find_weapon_lock_on_target",Name(uintptr_t(p))}); return cfg.lockOn; }
inline bool __fastcall PlFindNext(CPed* p,void*,CEntity* e,uint32 b) { if(!Mine())return false; trace.push_back({"find_next_weapon_lock_on_target",Name(uintptr_t(p)),Name(uintptr_t(e)),b&0xFF}); return cfg.lockOn; }
inline void __fastcall PlCompute3rd(CPed* p,void*,uint32 b) { if(Mine())trace.push_back({"compute_3rd_person_mouse_target",Name(uintptr_t(p)),b&0xFF}); }
inline bool __fastcall PlJustAttacked(CPed* p,void*) { if(!Mine())return false; trace.push_back({"player_has_just_attacked_someone",Name(uintptr_t(p))}); return cfg.justAtt; }
inline bool __fastcall PlHasToBreak(CPed* p,void*,CEntity* e,void* w) { if(!Mine())return false; trace.push_back({"does_target_have_to_be_broken",Name(uintptr_t(p)),Name(uintptr_t(e)),Name(uintptr_t(w))}); return cfg.hasToBreak; }
inline bool __cdecl PlCanVeh(CPed* p) { if(!Mine())return false; trace.push_back({"ped_can_be_targetted_vehicle_wise",Name(uintptr_t(p))}); return cfg.canVeh; }
inline float __fastcall PlSprint(CPed* p,void*,uint32 type) { if(!Mine())return 0.f; trace.push_back({"control_button_sprint",Name(uintptr_t(p)),type}); return cfg.sprint; }
inline void __fastcall PlRealMove(CPed* p,void*) { if(Mine())trace.push_back({"set_real_move_anim",Name(uintptr_t(p))}); }
// animacion
alignas(4) inline std::array<uint8,4> assocSlot{};
inline void* AssocPtr(int k) { return assocs[k].data(); }
inline void* __cdecl BlendStub(RpClump* c,int group,int anim,float blend) {
    if(!Mine())return nullptr; trace.push_back({"blend_animation",Name(uintptr_t(c)),group,anim,F(blend)}); return AssocPtr(int(cfg.blendCount++%2)); }
inline void* __cdecl AssocGetStub(RpClump* c,uint32 id) {
    if(!Mine())return nullptr; trace.push_back({"get_association",Name(uintptr_t(c)),id}); return reinterpret_cast<void*>(id==347 ? cfg.assocKP : cfg.assocOther); }
inline void* __cdecl AssocFirstStub(RpClump* c) {
    if(!Mine())return nullptr; trace.push_back({"first_association",Name(uintptr_t(c))}); return cfg.assocCount>0 ? AssocPtr(2) : nullptr; }
inline void* __cdecl AssocNextStub(void* a) {
    if(!Mine())return nullptr; trace.push_back({"next_association",Name(uintptr_t(a))});
    for(int k=2;k<2+cfg.assocCount-1;++k)if(a==AssocPtr(k))return AssocPtr(k+1);
    return nullptr; }
inline void* __fastcall GetAnimStub(void* group,void*,uint32 id) {
    if(!Mine())return nullptr; const auto base=Gl<uintptr_t>(G_ASSOCGROUPS);
    trace.push_back({"assoc_group_get_animation",int32((uintptr_t(group)-base)/0x14),id}); return sassocs[id==ANIM_ID_RUN ? 0 : 1].data(); }
inline void __cdecl ReqModelStub(int id,uint32 flags) { if(Mine())trace.push_back({"request_model",id,flags}); }
inline void __cdecl DeletableStub(int id) { if(Mine())trace.push_back({"set_model_is_deletable",id}); }
inline float __cdecl StatStub(int stat) { if(!Mine())return 0.f; trace.push_back({"get_stat_value",stat}); return cfg.stat; }
inline bool __fastcall CantSprintStub(void*,void*,uint32 id) { if(!Mine())return false; trace.push_back({"cant_sprint_on",id&0xFF}); return cfg.surfCant; }
// logica de juego
inline bool __cdecl Use2PStub(CPed* p) { if(!Mine())return false; trace.push_back({"is_player_use_2_player_controls",Name(uintptr_t(p))}); return cfg.use2P; }
inline bool __cdecl AllowDirStub(CPed* p,float x,float y,float z,float lim) {
    if(!Mine())return true; const bool r=cfg.allow[cfg.allowIdx++%4]; trace.push_back({"is_player_allowed_to_go_in_this_direction",Name(uintptr_t(p)),F(x),F(y),F(z),F(lim),r}); return r; }
inline bool __cdecl KickingStub() { if(!Mine())return false; trace.push_back({"kicking_when_down"}); return cfg.kicking; }
// inteligencia y gestor de tareas
inline void __fastcall DuckSecondaryStub(void* i,void*,uint32 len) { if(Mine())trace.push_back({"set_task_duck_secondary",Name(uintptr_t(i)),len&0xFFFF}); }
inline void __fastcall DuckClearStub(void* i,void*) { if(Mine())trace.push_back({"clear_task_duck_secondary",Name(uintptr_t(i))}); }
inline bool __fastcall StealthStub(void* i,void*,CPed* t,uint32 b) { if(!Mine())return false; trace.push_back({"test_for_stealth_kill",Name(uintptr_t(i)),Name(uintptr_t(t)),b&0xFF}); return cfg.stealth[b&1]; }
inline bool __fastcall SeeingStub(void* i,void*,const CVector* v) { if(!Mine())return false; trace.push_back({"is_in_seeing_range",Name(uintptr_t(i)),F(v->x),F(v->y),F(v->z)}); return cfg.seeing; }
inline void* __fastcall ActivePrimaryStub(void* i,void*) { if(!Mine())return nullptr; trace.push_back({"get_active_primary_task",Name(uintptr_t(i))}); return reinterpret_cast<void*>(cfg.activePrimary); }
inline void __fastcall SetTaskStub(void* tm,void*,CTask* t,int idx,uint32 b) {
    if(Mine())trace.push_back({"set_task",Name(uintptr_t(tm)),t ? int(t->GetTaskType()) : -1,idx,b&0xFF}); }
inline void __fastcall SetTaskSecStub(void* tm,void*,CTask* t,int idx) {
    if(Mine())trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),t ? int(t->GetTaskType()) : -1,idx}); }
// tareas
inline bool __cdecl PistolWhipStub(CPed* p,CEntity* e) { if(!Mine())return false; trace.push_back({"require_pistol_whip",Name(uintptr_t(p)),Name(uintptr_t(e))}); return cfg.pistolWhip; }
inline bool __fastcall ControlGunStub(void* t,void*,CPed* p,CEntity* e,int cmd) { if(!Mine())return true; trace.push_back({"control_gun",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(e)),int(int8(cmd))}); return true; }
inline bool __fastcall PassiveGunStub(void* t,void*) { if(!Mine())return true; trace.push_back({"player_passive_control_gun",Name(uintptr_t(t))}); return true; }
inline void __fastcall AbortIkStub(void* t,void*,CPed* p) { if(Mine())trace.push_back({"abort_ik",Name(uintptr_t(t)),Name(uintptr_t(p))}); }
inline bool __fastcall GunMoveStub(void* t,void*,const CVector2D* v) { if(Mine())trace.push_back({"control_gun_move",Name(uintptr_t(t)),F(v->x),F(v->y)}); return true; }
inline bool __fastcall ControlFightStub(void* t,void*,CEntity* e,uint32 cmd) { if(!Mine())return true; trace.push_back({"control_fight",Name(uintptr_t(t)),Name(uintptr_t(e)),cmd&0xFF}); return true; }
inline bool __fastcall ControlThrowStub(void* t,void*,uint32 b,CEntity* e,const CVector* v) { if(!Mine())return true; trace.push_back({"control_throw",Name(uintptr_t(t)),b&0xFF,Name(uintptr_t(e)),v ? 1 : 0}); return true; }
inline bool __cdecl CanDuckStub(CPed* p) { if(!Mine())return false; trace.push_back({"can_ped_duck",Name(uintptr_t(p))}); return cfg.canDuck; }
inline bool __fastcall InUseStub(void* t,void*) { if(!Mine())return false; trace.push_back({"is_task_in_use_by_other_tasks",Name(uintptr_t(t))}); return cfg.inUse; }
inline bool __fastcall DuckMoveStub(void* t,void*,float x,float y) { if(Mine())trace.push_back({"control_duck_move",Name(uintptr_t(t)),F(x),F(y)}); return true; }
// grupos, eventos, crimenes y audio
inline CPedGroup* __cdecl GroupStub(const CPed* p) {
    if(!Mine())return nullptr; trace.push_back({"get_peds_group",Name(uintptr_t(p))});
    return uintptr_t(p)==cfg.groupPed ? reinterpret_cast<CPedGroup*>(cfg.group) : nullptr; }
inline bool __cdecl SameGroupStub(const CPed* a,const CPed* b) { if(!Mine())return false; trace.push_back({"are_in_same_group",Name(uintptr_t(a)),Name(uintptr_t(b))}); return cfg.sameGroup; }
inline bool __fastcall AddEventStub(void* gi,void*,void* ev) { if(!Mine())return true; trace.push_back({"group_intelligence_add_event",Name(uintptr_t(gi)),ev ? 1 : 0}); return true; }
inline void* __fastcall EventGroupAddStub(void* g,void*,void* ev,uint32 b) { if(Mine())trace.push_back({"event_group_add",Name(uintptr_t(g)),ev ? 1 : 0,b&0xFF}); return nullptr; }
inline bool __fastcall AffectsStub(void* ev,void*,CPed* p) { if(!Mine())return false; trace.push_back({"event_damage_affects_ped",Name(uintptr_t(p))}); return cfg.affects; }
inline void __fastcall ComputeRespStub(void* calc,void*,CPed* p,void* resp,uint32 b) { if(Mine())trace.push_back({"compute_damage_response",Name(uintptr_t(p)),resp ? 1 : 0,b&0xFF}); }
inline void __cdecl ReportCrimeStub(int type,CEntity* victim,CPed* who) { if(Mine())trace.push_back({"report_crime",type,Name(uintptr_t(victim)),Name(uintptr_t(who))}); }
inline void __fastcall AudioEventStub(void* ae,void*,int ev) { if(Mine())trace.push_back({"add_audio_event",Name(uintptr_t(ae)),ev}); }
inline bool __fastcall WeaponFireStub(void* w,void*,CEntity* by,CVector* a,CVector* b,CEntity* c,CVector* d,CVector* e) {
    if(!Mine())return false;
    auto v=[](const CVector* p)->json{ return p ? json::array({F(p->x),F(p->y),F(p->z)}) : json(nullptr); };
    trace.push_back({"weapon_fire",Name(uintptr_t(w)),Name(uintptr_t(by)),v(a),v(b),Name(uintptr_t(c)),v(d),v(e)}); return cfg.fireRet; }
inline bool __fastcall IsMeleeStub(void* w,void*) {
    if(!Mine())return false; const auto type=*reinterpret_cast<int32*>(w); trace.push_back({"weapon_is_type_melee",Name(uintptr_t(w))});
    return CWeaponInfo::GetWeaponInfo(eWeaponType(type),eWeaponSkill::STD)->m_nWeaponFire==WEAPON_FIRE_MELEE; }
// IK
inline bool __fastcall IkLookingStub(void*,void*,CPed* p) { if(!Mine())return false; trace.push_back({"ik_is_looking",Name(uintptr_t(p))}); return cfg.ikLooking; }
inline void* __fastcall IkLookEntStub(void*,void*,CPed* p) { if(!Mine())return nullptr; trace.push_back({"ik_get_look_at_entity",Name(uintptr_t(p))}); return reinterpret_cast<void*>(cfg.lookEnt); }
inline void __fastcall IkAbortStub(void*,void*,CPed* p,uint32 t) { if(Mine())trace.push_back({"ik_abort_look_at",Name(uintptr_t(p)),t}); }
inline void __fastcall IkLookAtStub(void*,void*,const char* why,CPed* ped,CEntity* ent,int time,int bone,CVector* off,uint32 torso,float speed,int blend,int prio,uint32 force) {
    if(Mine())trace.push_back({"ik_look_at",std::string(why),Name(uintptr_t(ped)),Name(uintptr_t(ent)),time,bone,off ? 1 : 0,torso&0xFF,F(speed),blend,prio,force&0xFF}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Direcciones del hook: ours = la funcion de la DLL, gta = la entrada del EXE (0 si no es un hook simple o virtual).
inline uintptr_t HookOurs(ReversibleHooks::ReversibleHook::Base* h,uintptr_t* gta=nullptr) {
    using namespace ReversibleHooks::ReversibleHook;
    if(!h)return 0;
    if(h->Type()==Base::HookType::Simple){ auto* s=static_cast<Simple*>(h); if(gta)*gta=uintptr_t(s->GetHookGTAAddress()); return uintptr_t(s->GetHookOurAddress()); }
    if(h->Type()==Base::HookType::Virtual){ auto* v=static_cast<Virtual*>(h); if(gta)*gta=uintptr_t(v->GetHookGTAAddress()); return uintptr_t(v->GetHookOurAddress()); }
    return 0;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo) sin entrar nunca en el EXE.
inline uintptr_t Body(uintptr_t a){
    for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const uintptr_t t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; }
    return a;
}
template<class T> uintptr_t Addr(T fn){ static_assert(sizeof(T)==sizeof(uintptr_t)); return std::bit_cast<uintptr_t>(fn); }

enum Kind { K_IDLE, K_ZELDAWEAPON, K_DUCKED, K_ZELDA, K_WEAPON };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=5;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimplePlayerOnFoot","PlayIdleAnimations",0x6872C0,0,-1,1024},
    {"CTaskSimplePlayerOnFoot","PlayerControlZeldaWeapon",0x687C20,0,-1,1024},
    {"CTaskSimplePlayerOnFoot","PlayerControlDucked",0x687F30,0,-1,1024},
    {"CTaskSimplePlayerOnFoot","PlayerControlZelda",0x6883D0,0,-1,1024},
    {"CTaskSimplePlayerOnFoot","ProcessPlayerWeapon",0x6859A0,0,-1,1024},
}};
constexpr std::array<Kind,NFNS> KINDS{K_IDLE,K_ZELDAWEAPON,K_DUCKED,K_ZELDA,K_WEAPON};

// Resumen de las tareas nuevas: tipo y campos fijados por los argumentos del constructor (los huecos sin escribir del EXE/DLL no se comparan).
inline json TaskSummary(CTask* t) {
    json w=json::array(); const int type=int(t->GetTaskType());
    auto word=[&](size_t o){ w.push_back(Name(At<uint32>(t,o))); };
    auto raw=[&](size_t o){ w.push_back(At<uint32>(t,o)); };
    switch(type){
    case TASK_SIMPLE_USE_GUN: w.push_back(At<uint8>(t,0xF)); word(0x1C); raw(0x20); raw(0x24); raw(0x28); w.push_back(At<uint16>(t,0x34)); w.push_back(At<uint8>(t,0x3B)); break;
    case TASK_SIMPLE_FIGHT: w.push_back(At<uint16>(t,0x10)); word(0x18); w.push_back(At<uint8>(t,0x26)); break;
    case TASK_SIMPLE_STEALTH_KILL: w.push_back(At<uint8>(t,0x8)); word(0xC); raw(0x10); break;
    case TASK_SIMPLE_THROW_PROJECTILE: word(0x10); raw(0x14); raw(0x18); raw(0x1C); raw(0x20); break;
    case TASK_COMPLEX_JUMP: raw(0xC); w.push_back(At<uint8>(t,0x10)); break;
    default: break;
    }
    return {{"type",type},{"fields",w}};
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"footwave4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"footwave4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("footwave4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"footwave4-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // direcciones de la DLL de las dos estaticas (cdecl en la DLL, `ret 4` en el EXE): el candidato las llama directamente
    std::array<uintptr_t,NFNS> dllAddr{};
    for(size_t f=0;f<fns.size();++f){ uintptr_t gta=0; const uintptr_t ours=HookOurs(hooks[f],&gta); if(!ours||gta!=fns[f].address){Write({{"event","error"},{"reason","hook address mismatch"},{"function",fns[f].name},{"gta",gta}});return;} dllAddr[f]=Body(ours); }

    // registro de buffers (nombre estable para las trazas)
    fgroups.assign((Gl<uint32>(G_NUMDEFS)+2)*0x14,0);
    bufs.clear();
    auto reg=[&](const char* n,auto& a,bool ent){ bufs.push_back({n,a.data(),a.size(),ent}); };
    reg("pl",plBuf,true); reg("tp",tpBuf,true); reg("op",opBuf,true); reg("tv",tvBuf,true);
    static const char* mn[4]{"mat0","mat1","mat2","mat3"}; for(int n=0;n<4;++n)reg(mn[n],mats[n],false);
    static const char* in[3]{"intel0","intel1","intel2"}; for(int n=0;n<3;++n)reg(in[n],intels[n],false);
    reg("pdata",pdata,false); reg("pad",padBuf,false); reg("grp",grp,false); reg("winfo",winfo,false); reg("object",object,false);
    static const char* an[6]{"as0","as1","as2","as3","as4","as5"}; for(int n=0;n<6;++n)reg(an[n],assocs[n],false);
    static const char* sn[2]{"sa0","sa1"}; for(int n=0;n<2;++n)reg(sn[n],sassocs[n],false);
    static const char* bn[4]{"blk0","blk1","blk2","blk3"}; for(int n=0;n<4;++n)reg(bn[n],fblocks[n],false);
    bufs.push_back({"groups",fgroups.data(),fgroups.size(),false});

    // Comprobacion de la disposicion de miembros y de las constantes frente a los desplazamientos y valores del EXE usados por los fixtures.
    std::vector<std::string> bad;
    auto off=[](const void* base,const void* member){ return uintptr_t(member)-uintptr_t(base); };
    {
        auto* pl=reinterpret_cast<CPlayerPed*>(plBuf.data()); auto* pd=reinterpret_cast<CPlayerPedData*>(pdata.data()); auto* pad=reinterpret_cast<CPad*>(padBuf.data());
        auto* task=reinterpret_cast<CTaskSimplePlayerOnFoot*>(object.data()); auto* ug=reinterpret_cast<CTaskSimpleUseGun*>(quiet[0].data());
        auto* duck=reinterpret_cast<CTaskSimpleDuck*>(quiet[1].data()); auto* hold=reinterpret_cast<CTaskSimpleHoldEntity*>(quiet[2].data());
        auto* g=reinterpret_cast<CPedGroup*>(grp.data()); auto* wi=reinterpret_cast<CWeaponInfo*>(winfo.data()); auto* cam=&TheCamera.m_aCams[0];
        auto* ass=reinterpret_cast<CAnimBlendAssociation*>(assocs[0].data()); auto* blk=reinterpret_cast<CAnimBlock*>(fblocks[0].data());
        auto chk=[&](bool c,const char* what){ if(!c)bad.push_back(what); };
        chk(off(pl,&pl->m_matrix)==P_MATRIX,"CPed::m_matrix"); At<uint32>(pl,P_CLUMP)=uint32(uintptr_t(fakeClump.data())); chk(uintptr_t(pl->GetRwObject())==uintptr_t(fakeClump.data()),"CEntity::m_pRwObject (+0x18)"); At<uint32>(pl,P_CLUMP)=0; chk(off(pl,&pl->m_nContactSurface)==P_CONTACT,"CPed::m_nContactSurface");
        chk(off(pl,&pl->m_pAttachedTo)==P_ATTACHED,"CPed::m_pAttachedTo"); chk(off(pl,&pl->m_nAnimGroup)==P_ANIMGROUP,"CPed::m_nAnimGroup"); chk(off(pl,&pl->m_pedIK.m_nFlags)==P_IKFLAGS,"CPed::m_pedIK");
        chk(off(pl,&pl->m_nPedState)==P_PEDSTATE,"CPed::m_nPedState"); chk(off(pl,&pl->m_nMoveState)==P_MOVESTATE,"CPed::m_nMoveState"); chk(off(pl,&pl->m_nSwimmingMoveState)==P_SWIM,"CPed::m_nSwimmingMoveState");
        chk(off(pl,&pl->m_fAimingRotation)==P_AIMROT,"CPed::m_fAimingRotation"); chk(off(pl,&pl->m_standingOnEntity)==P_STANDING,"CPed::m_standingOnEntity"); chk(off(pl,&pl->m_nPedType)==P_TYPE,"CPed::m_nPedType");
        chk(off(pl,&pl->m_aWeapons)==P_WEAPONS,"CPed::m_aWeapons"); chk(off(pl,&pl->m_nActiveWeaponSlot)==P_WSLOT,"CPed::m_nActiveWeaponSlot"); chk(off(pl,&pl->m_nWeaponAccuracy)==P_ACCURACY,"CPed::m_nWeaponAccuracy");
        chk(off(pl,&pl->m_pTargetedObject)==P_TARGETED,"CPed::m_pTargetedObject"); chk(off(pl,&pl->m_pFire)==P_FIRE,"CPed::m_pFire"); chk(off(pl,&pl->m_p3rdPersonMouseTarget)==P_MOUSE3,"CPlayerPed::m_p3rdPersonMouseTarget");
        chk(sizeof(CWeapon)==0x1C,"sizeof(CWeapon)"); chk(off(&pl->m_aWeapons.data()[0],&pl->m_aWeapons.data()[0].m_State)==4,"CWeapon::m_State"); chk(off(&pl->m_aWeapons.data()[0],&pl->m_aWeapons.data()[0].m_TimeForNextShotMs)==0x10,"CWeapon::m_TimeForNextShotMs");
        chk(off(pd,&pd->m_vecFightMovement)==D_FIGHT,"PlayerPedData::m_vecFightMovement"); chk(off(pd,&pd->m_fMoveBlendRatio)==D_BLEND,"PlayerPedData::m_fMoveBlendRatio"); chk(off(pd,&pd->m_nChosenWeapon)==D_CHOSEN,"PlayerPedData::m_nChosenWeapon");
        chk(off(pd,&pd->m_fAttackButtonCounter)==D_ATTACK,"PlayerPedData::m_fAttackButtonCounter"); chk(off(pd,&pd->m_nPlayerFlags)==D_FLAGS,"PlayerPedData::m_nPlayerFlags"); chk(off(pd,&pd->m_fLookPitch)==D_PITCH,"PlayerPedData::m_fLookPitch");
        chk(off(pd,&pd->m_bPlayerSprintDisabled)==D_SPRINTOFF,"PlayerPedData::m_bPlayerSprintDisabled"); chk(off(pd,&pd->m_bDontAllowWeaponChange)==D_NOWCHANGE,"PlayerPedData::m_bDontAllowWeaponChange");
        chk(off(pd,&pd->m_nFireHSMissilePressedTime)==D_HSTIME,"PlayerPedData::m_nFireHSMissilePressedTime"); chk(off(pd,&pd->m_LastHSMissileTarget)==D_HSTARGET,"PlayerPedData::m_LastHSMissileTarget");
        chk(off(pad,&pad->NewState.m_bPedWalk)==PAD_WALK,"CPad::NewState.m_bPedWalk"); chk(off(pad,&pad->DisablePlayerControls)==PAD_DISABLE,"CPad::DisablePlayerControls");
        chk(off(task,&task->m_PlayerIdlesAnimBlockId)==8&&off(task,&task->m_nFrameCounter)==0xC&&off(task,&task->m_nTimer)==0x10&&off(task,&task->m_pLookingAtEntity)==0x18&&sizeof(CTaskSimplePlayerOnFoot)==0x1C,"CTaskSimplePlayerOnFoot");
        chk(off(ug,&ug->m_WeaponInfo)==0x30,"CTaskSimpleUseGun::m_WeaponInfo"); chk(off(duck,&duck->m_bIsFinished)==0x18&&off(duck,&duck->m_bIsAborting)==0x19,"CTaskSimpleDuck flags");
        chk(off(hold,&hold->m_pAnimBlendAssociation)==0x38,"CTaskSimpleHoldEntity::m_pAnimBlendAssociation"); chk(off(g,&g->m_groupIntelligence)==0x30,"CPedGroup::m_groupIntelligence");
        chk(off(wi,&wi->m_eAnimGroup)==0x1C&&off(wi,&wi->flags)==0x18,"CWeaponInfo"); chk(sizeof(CCam)==CAM_SIZE&&sizeof(cam->m_nMode)==2&&off(cam,&cam->m_vecFront)-off(cam,&cam->m_nMode)==CAM_FRONT,"CCam");
        chk(uintptr_t(&TheCamera.m_aCams[0].m_nMode)==G_CAMS&&uintptr_t(&TheCamera.m_bWideScreenOn)==G_WIDE&&uintptr_t(&TheCamera.m_fOrientation)==G_ORIENT&&uintptr_t(&TheCamera.m_nActiveCam)==G_ACTIVECAM&&
            uintptr_t(&TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget)==G_JUMPED&&uintptr_t(&CCamera::m_bUseMouse3rdPerson)==G_MOUSE3,"TheCamera");
        chk(off(ass,&ass->m_BlendHier)==0x14&&off(ass,&ass->m_Flags)==0x2E&&off(ass,&ass->m_BlendAmount)==0x18&&off(ass,&ass->m_BlendDelta)==0x1C,"CAnimBlendAssociation"); chk(off(blk,&blk->IsLoaded)==0x10&&off(blk,&blk->FirstAnimIdx)==0x14&&off(blk,&blk->NumAnims)==0x18,"CAnimBlock");
        chk(eGunCommand::AIM==eGunCommand(1)&&eGunCommand::FIRE==eGunCommand(2)&&eGunCommand::RELOAD==eGunCommand(4)&&eGunCommand::PISTOLWHIP==eGunCommand(5)&&eGunCommand::END_NOW==eGunCommand(7),"eGunCommand");
        chk(TASK_SIMPLE_FIGHT==1016&&TASK_SIMPLE_USE_GUN==1017&&TASK_SIMPLE_THROW_PROJECTILE==1018&&TASK_SIMPLE_STEALTH_KILL==1027&&TASK_COMPLEX_JUMP==211&&TASK_COMPLEX_REACT_TO_GUN_AIMED_AT==601,"eTaskType");
        chk(PEDSTATE_DIE==54&&PEDSTATE_DEAD==55&&PEDSTATE_SNIPER_MODE==12&&PEDMOVE_STILL==1&&PEDMOVE_WALK==4&&PEDMOVE_RUN==6&&PEDMOVE_SPRINT==7,"ePedState/eMoveState");
        chk(WEAPON_CHAINSAW==9&&WEAPON_DETONATOR==40&&WEAPON_CAMERA==43&&WEAPON_INFRARED==45&&WEAPON_EXTINGUISHER==42&&WEAPON_RLAUNCHER==35&&WEAPON_RLAUNCHER_HS==36&&WEAPON_SNIPERRIFLE==34&&WEAPON_PISTOL_SILENCED==23,"eWeaponType");
        chk(MODE_SNIPER==7&&MODE_ROCKETLAUNCHER==8&&MODE_ROCKETLAUNCHER_HS==0x33&&MODE_M16_1STPERSON==0x22&&MODE_HELICANNON_1STPERSON==0x2D&&MODE_CAMERA==0x2E&&MODE_AIMWEAPON==0x35&&MODE_FOLLOWPED==4,"eCamMode");
        chk(WEAPON_FIRE_MELEE==0&&WEAPON_FIRE_INSTANT_HIT==1&&WEAPON_FIRE_PROJECTILE==2&&WEAPON_FIRE_AREA_EFFECT==3&&WEAPON_FIRE_CAMERA==4&&WEAPON_FIRE_USE==5,"eWeaponFire");
        chk(ANIM_ID_KILL_PARTIAL==0x15B&&ANIM_ID_RELOAD==0xE2&&ANIM_ID_RUN==1&&ANIM_ID_SPRINT==2&&ANIM_ID_GUNMOVE_FWD==0x32&&ANIM_ID_IDLE_ARMED==0xB&&ANIM_ID_IDLE_CHAT==0xC&&IFPToModelId(0)==0x63E7,"anim ids");
        chk(CTX_GLOBAL_PULL_GUN==0xB0&&CTX_GLOBAL_SINGING==0x150&&STAT_MANAGEMENT_ISSUES_MISSION_ACCOMPLISHED==0x12F&&CRIME_STAB_PED==18&&AE_WEAPON_STEALTH_KILL==0x9C&&PED_PIECE_TORSO==3&&BONE_HEAD==5&&BONE_UNKNOWN==-1&&STREAMING_GAME_REQUIRED==2&&SPRINT_GROUND==0&&TASK_SECONDARY_ATTACK==0&&TASK_SECONDARY_DUCK==1&&TASK_SECONDARY_PARTIAL_ANIM==4&&TASK_PRIMARY_PRIMARY==3,"ids varios");
        chk(Gl<uint32>(G_NUMBLOCKS)>=8&&Gl<uint32>(G_NUMDEFS)>=90&&Gl<int32>(G_NUMANIMS)>=64,"tablas de animacion reales demasiado pequenas");
        Write({{"event","layout"},{"ok",bad.empty()},{"bad",bad}});
        if(!bad.empty()){Write({{"event","error"},{"reason","layout mismatch between DLL members and EXE offsets"},{"bad",bad}});return;}
    }
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillQTraps(std::make_integer_sequence<int,12>{});
    quietVtbl[0]=reinterpret_cast<void*>(&QDtor); quietVtbl[2]=reinterpret_cast<void*>(&QSub); quietVtbl[3]=reinterpret_cast<void*>(&FSimple); quietVtbl[4]=reinterpret_cast<void*>(&QType);
    quietVtbl[6]=reinterpret_cast<void*>(&FAbort);   // MakeAbortable solo lo llama la funcion probada: se traza con sus argumentos

    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 savedLastFree=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC);
    const unsigned savedFP=_controlfp(0,0);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // globales tocados: instantanea inicial (se restaura tras cada ejecucion y al final)
    struct Range { uintptr_t a; size_t n; std::vector<uint8> saved; };
    std::vector<Range> ranges;
    auto addRange=[&](uintptr_t a,size_t n){ ranges.push_back({a,n,ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)}); };
    addRange(G_PLAYERS1,4); addRange(G_LASTRAND,4); addRange(G_LASTTOUCH,4); addRange(G_DUCKBLEND,4); addRange(G_UNKNOWN,1);
    addRange(G_MOUSE3,1); addRange(G_JUMPED,1); addRange(G_WIDE,1); addRange(G_ACTIVECAM,1); addRange(G_ORIENT,4); addRange(G_ASSOCGROUPS,4);
    for(int n=0;n<3;++n){ addRange(G_CAMS+n*CAM_SIZE,2); addRange(G_CAMS+n*CAM_SIZE+CAM_FRONT,12); }
    for(int n=0;n<4;++n)addRange(G_IDLEBLOCKS+n*0x20,0x20);
    const uintptr_t realGroups=Gl<uintptr_t>(G_ASSOCGROUPS);
    auto restoreGlobals=[&]{ for(auto& r:ranges)std::memcpy(reinterpret_cast<void*>(r.a),r.saved.data(),r.n); };
    auto globalsHash=[&]{ std::vector<uint8> all; for(auto& r:ranges){ auto b=ReversingProbe::Bytes(reinterpret_cast<void*>(r.a),r.n); all.insert(all.end(),b.begin(),b.end()); } return Hash(all.data(),all.size()); };
    (void)realGroups;

    // sitios parcheables: direccion del EXE y funcion de la DLL (hook -> GetHookOurAddress; si no hay hook, el simbolo dado). Un solo stub por sitio.
    struct Site { const char* cls; const char* fn; uintptr_t exeAddr; void* stub; uintptr_t fallback; };
    const std::vector<Site> sites{
        {nullptr,nullptr,0x821B1E,reinterpret_cast<void*>(&FakeRand),0},
        {nullptr,nullptr,0,reinterpret_cast<void*>(&FakeRandU16),Addr(&CGeneral::GetRandomNumber)},
        {"CPlayerPed","GetPadFromPlayer",0x609560,reinterpret_cast<void*>(&GetPadStub),0},
        {"CPad","SetTouched",0x53F200,reinterpret_cast<void*>(&PadSetTouched),0},
        {"CPad","GetTouchedTimeDelta",0x53F210,reinterpret_cast<void*>(&PadTouchDelta),0},
        {"CPad","GetPedWalkUpDown-",0x53FD30,reinterpret_cast<void*>(&PadUD),0},
        {"CPad","GetPedWalkLeftRight-",0x53FC90,reinterpret_cast<void*>(&PadLR),0},
        {"CPad","GetPedWalkUpDown-ped",0x540E20,reinterpret_cast<void*>(&PadUDp),0},
        {"CPad","GetPedWalkLeftRight-ped",0x540DC0,reinterpret_cast<void*>(&PadLRp),0},
        {"CPad","DuckJustDown",0x540720,reinterpret_cast<void*>(&PadB<PB_DUCK>),0},
        {"CPad","GetSprint",0x5407A0,reinterpret_cast<void*>(&PadB<PB_SPRINT>),0},
        {"CPad","JumpJustDown",0x540770,reinterpret_cast<void*>(&PadB<PB_JUMP>),0},
        {"CPad","ExitVehicleJustDown",0x540120,reinterpret_cast<void*>(&PadB<PB_EXIT>),0},
        {"CPad","GetTarget",0x540670,reinterpret_cast<void*>(&PadB<PB_TARGET>),0},
        {"CPad","GetEnterTargeting",0x5406B0,reinterpret_cast<void*>(&PadB<PB_ENTER>),0},
        {"CPad","ShiftTargetLeftJustDown",0x540850,reinterpret_cast<void*>(&PadB<PB_SHIFTL>),0},
        {"CPad","ShiftTargetRightJustDown",0x540880,reinterpret_cast<void*>(&PadB<PB_SHIFTR>),0},
        {"CPad","WeaponJustDown",0x540250,reinterpret_cast<void*>(&PadWeaponJD),0},
        {"CPad","MeleeAttackJustDown",0x540390,reinterpret_cast<void*>(&PadMeleeJD),0},
        {"CPad","GetMeleeAttack",0x540340,reinterpret_cast<void*>(&PadGetMelee),0},
        {"CPad","GetWeapon",0x540180,reinterpret_cast<void*>(&PadGetWeapon),0},
        {"CPad","AimWeaponLeftRight",0x541040,reinterpret_cast<void*>(&PadAimLR),0},
        {"CPad","AimWeaponUpDown",0x5410C0,reinterpret_cast<void*>(&PadAimUD),0},
        {"CCamera","Using1stPersonWeaponMode",0x50BFF0,reinterpret_cast<void*>(&Cam1stStub),0},
        {"CCamera","ClearPlayerWeaponMode",0x50AB10,reinterpret_cast<void*>(&CamClearStub),0},
        {"CCamera","SetNewPlayerWeaponMode",0x50BFB0,reinterpret_cast<void*>(&CamModeStub),0},
        {"CCamera","UpdateAimingCoors",0x50CB10,reinterpret_cast<void*>(&CamAimStub),0},
        {"CWeaponEffects","ClearCrossHair",0x742C60,reinterpret_cast<void*>(&CrossHairStub),0},
        {"CPed","Say",0x5EFFE0,reinterpret_cast<void*>(&SayStub),0},
        {"CPed","GetWeaponSkill-Current",0x5E6580,reinterpret_cast<void*>(&SkillStub),0},
        {"CPed","SetPedState",0x5E4500,reinterpret_cast<void*>(&PedStateStub),0},
        {"CPlayerPed","ClearWeaponTarget",0x609C80,reinterpret_cast<void*>(&PlClearTarget),0},
        {"CPlayerPed","Clear3rdPersonMouseTarget",0x609ED0,reinterpret_cast<void*>(&PlClear3rd),0},
        {"CPlayerPed","PlayerHasJustAttackedSomeone",0x60D5A0,reinterpret_cast<void*>(&PlJustAttacked),0},
        {"CPlayerPed","DoesTargetHaveToBeBroken",0x60C0C0,reinterpret_cast<void*>(&PlHasToBreak),0},
        {"CPlayerPed","PedCanBeTargettedVehicleWise",0x609D90,reinterpret_cast<void*>(&PlCanVeh),0},
        {nullptr,nullptr,0x60DC50,reinterpret_cast<void*>(&PlFindLock),Addr(&CPlayerPed::FindWeaponLockOnTarget)},
        {nullptr,nullptr,0x60E530,reinterpret_cast<void*>(&PlFindNext),Addr(&CPlayerPed::FindNextWeaponLockOnTarget)},
        {nullptr,nullptr,0x60B650,reinterpret_cast<void*>(&PlCompute3rd),Addr(&CPlayerPed::Compute3rdPersonMouseTarget)},
        {nullptr,nullptr,0x60A610,reinterpret_cast<void*>(&PlSprint),Addr(&CPlayerPed::ControlButtonSprint)},
        {nullptr,nullptr,0x60A9C0,reinterpret_cast<void*>(&PlRealMove),Addr(&CPlayerPed::SetRealMoveAnim)},
        {"CAnimManager","BlendAnimation-id",0x4D4610,reinterpret_cast<void*>(&BlendStub),0},
        {"RpAnimBlend","RpAnimBlendClumpGetAssociation-AnimId",0x4D68B0,reinterpret_cast<void*>(&AssocGetStub),0},
        {"RpAnimBlend","RpAnimBlendClumpGetFirstAssociation-",0x4D15E0,reinterpret_cast<void*>(&AssocFirstStub),0},
        {"RpAnimBlend","RpAnimBlendGetNextAssociation-Any",0x4D6AB0,reinterpret_cast<void*>(&AssocNextStub),0},
        {"CAnimBlendAssocGroup","GetAnimation-AnimationId",0x4CE090,reinterpret_cast<void*>(&GetAnimStub),0},
        {"CStreaming","RequestModel",0x4087E0,reinterpret_cast<void*>(&ReqModelStub),0},
        {"CStreaming","SetModelIsDeletable",0x409C10,reinterpret_cast<void*>(&DeletableStub),0},
        {"CStats","GetStatValue--OG",0x558E40,reinterpret_cast<void*>(&StatStub),0},
        {"SurfaceInfos_c","CantSprintOn",0x55E870,reinterpret_cast<void*>(&CantSprintStub),0},
        {"CGameLogic","IsPlayerUse2PlayerControls",0x442020,reinterpret_cast<void*>(&Use2PStub),0},
        {"CGameLogic","IsPlayerAllowedToGoInThisDirection",0x441E10,reinterpret_cast<void*>(&AllowDirStub),0},
        {"CLocalisation","KickingWhenDown",0x56D270,reinterpret_cast<void*>(&KickingStub),0},
        {"CPedIntelligence","SetTaskDuckSecondary",0x601230,reinterpret_cast<void*>(&DuckSecondaryStub),0},
        {"CPedIntelligence","ClearTaskDuckSecondary",0x601390,reinterpret_cast<void*>(&DuckClearStub),0},
        {"CPedIntelligence","TestForStealthKill",0x601E00,reinterpret_cast<void*>(&StealthStub),0},
        {"CPedIntelligence","IsInSeeingRange",0x600C60,reinterpret_cast<void*>(&SeeingStub),0},
        {"CPedIntelligence","GetActivePrimaryTask",0x4B85B0,reinterpret_cast<void*>(&ActivePrimaryStub),0},
        {"CTaskManager","SetTask",0x681AF0,reinterpret_cast<void*>(&SetTaskStub),0},
        {"CTaskManager","SetTaskSecondary",0x681B60,reinterpret_cast<void*>(&SetTaskSecStub),0},
        {"CTaskSimpleUseGun","RequirePistolWhip",0x61E200,reinterpret_cast<void*>(&PistolWhipStub),0},
        {"CTaskSimpleUseGun","ControlGun",0x61E040,reinterpret_cast<void*>(&ControlGunStub),0},
        {"CTaskSimpleUseGun","PlayerPassiveControlGun",0x61E0A0,reinterpret_cast<void*>(&PassiveGunStub),0},
        {"CTaskSimpleUseGun","AbortIK",0x61DFA0,reinterpret_cast<void*>(&AbortIkStub),0},
        {"CTaskSimpleUseGun","ControlGunMove",0x61E0C0,reinterpret_cast<void*>(&GunMoveStub),0},
        {nullptr,nullptr,0x61C5E0,reinterpret_cast<void*>(&ControlFightStub),Addr(&CTaskSimpleFight::ControlFight)},
        {"CTaskSimpleThrowProjectile","ControlThrow",0x61F810,reinterpret_cast<void*>(&ControlThrowStub),0},
        {"CTaskSimpleDuck","CanPedDuck",0x692610,reinterpret_cast<void*>(&CanDuckStub),0},
        {"CTaskSimpleDuck","IsTaskInUseByOtherTasks",0x61C3D0,reinterpret_cast<void*>(&InUseStub),0},
        {"CTaskSimpleDuck","ControlDuckMove",0x6923F0,reinterpret_cast<void*>(&DuckMoveStub),0},
        {nullptr,nullptr,0x5F7E80,reinterpret_cast<void*>(&GroupStub),Addr(&CPedGroups::GetPedsGroup)},
        {nullptr,nullptr,0x5F7F40,reinterpret_cast<void*>(&SameGroupStub),Addr(&CPedGroups::AreInSameGroup)},
        {"CPedGroupIntelligence","AddEvent-",0x5F7470,reinterpret_cast<void*>(&AddEventStub),0},
        {"CEventGroup","Add-",0x4AB420,reinterpret_cast<void*>(&EventGroupAddStub),0},
        {"CEventDamage","AffectsPed",0x4B35A0,reinterpret_cast<void*>(&AffectsStub),0},
        {"CPedDamageResponseCalculator","ComputeDamageResponse",0x4B5AC0,reinterpret_cast<void*>(&ComputeRespStub),0},
        {"CCrime","ReportCrime",0x532010,reinterpret_cast<void*>(&ReportCrimeStub),0},
        {"CAEPedWeaponAudioEntity","AddAudioEvent",0x4E69F0,reinterpret_cast<void*>(&AudioEventStub),0},
        {"CWeapon","Fire",0x742300,reinterpret_cast<void*>(&WeaponFireStub),0},
        {"CWeapon","IsTypeMelee",0x73B1C0,reinterpret_cast<void*>(&IsMeleeStub),0},
        {"IKChainManager_c","IsLooking",0x6181A0,reinterpret_cast<void*>(&IkLookingStub),0},
        {"IKChainManager_c","GetLookAtEntity",0x6181D0,reinterpret_cast<void*>(&IkLookEntStub),0},
        {"IKChainManager_c","AbortLookAt",0x618280,reinterpret_cast<void*>(&IkAbortStub),0},
        {"IKChainManager_c","LookAt",0x618970,reinterpret_cast<void*>(&IkLookAtStub),0},
    };
    // El cuerpo real se calcula ANTES de parchear ninguna entrada: tras parchear s.dll la cadena de E9 pasa por el propio stub y parchear "Body(s.dll)"
    // sobreescribiria la entrada del stub con un salto a si mismo (bucle infinito).
    std::vector<std::pair<uintptr_t,void*>> targets; std::vector<std::string> missing;
    auto addTarget=[&](uintptr_t at,void* stub){ if(!at)return; for(auto& [a,s]:targets)if(a==at){ if(s!=stub)missing.push_back("conflicto en "+std::to_string(at)); return; } targets.push_back({at,stub}); };
    for(auto& s:sites){
        uintptr_t dll=0;
        if(s.cls){ uintptr_t gta=0; auto* h=FindHook(s.cls,s.fn); dll=HookOurs(h,&gta);
            if(!h)missing.push_back(std::string("hook no encontrado: ")+s.cls+"::"+s.fn);
            else if(!dll||(s.exeAddr&&gta&&gta!=s.exeAddr))missing.push_back(std::string("direccion del hook distinta: ")+s.cls+"::"+s.fn); }
        if(!dll)dll=s.fallback;
        if(s.exeAddr)addTarget(s.exeAddr,s.stub);
        if(dll){ const uintptr_t body=Body(dll); addTarget(dll,s.stub); if(body!=dll)addTarget(body,s.stub); }
    }
    if(!missing.empty()){Write({{"event","error"},{"reason","sitios sin resolver"},{"missing",missing}});return;}
    // bytes iniciales de las entradas parcheables, para comprobar la restauracion
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& [a,s]:targets)entryBefore.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),8)});
    for(auto& fn:fns)entryBefore.push_back({fn.address,ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address),8)});
    Write({{"event","begin"},{"fp_control",savedFP},{"hooks",NFNS},{"sites",targets.size()}});

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        for(auto& [at,stub]:targets)patches.push_back(std::make_unique<EntryPatch>(at,stub));

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm; const Kind kind=KINDS[f];
            if(!only.empty()&&only!=full)continue;
            std::map<std::string,int> hist;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto R=[&](uint32 n){ return next()%n; };
                auto chance=[&](uint32 pct){ return next()%100<pct; };
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F2Au; if(!st)st=1; trace=json::array(); randIdx=0; cfg=Cfg{}; freshIds.clear();
                    CleanRefs();
                    for(const auto& b:bufs)std::memset(b.p,0,b.n);
                    for(auto& a:sassocs)std::memset(a.data(),0,a.size());
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FK_SIZE); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data();
                        At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_STAND_STILL; At<uint8>(fakes[n].data(),FK_ABORT)=uint8(next()%2); }
                    for(int n=0;n<QUIET;++n){ std::memset(quiet[n].data(),0,FK_SIZE); *reinterpret_cast<void**>(quiet[n].data())=quietVtbl.data(); At<int32>(quiet[n].data(),FK_TYPE)=TASK_SIMPLE_STAND_STILL; At<uint8>(quiet[n].data(),FK_ABORT)=uint8(next()%2); }
                    for(int k=0;k<8;++k){ const uint32 b=next()%4; randQ[k]=int(b*8192+next()%8192); }
                    randQ[6]=int(next()%8192); randQ[7]=int(8192+next()%8192);   // al menos dos tramos distintos de [0,4): el bucle de PlayIdleAnimations termina

                    auto* PL=Pl(); auto* TP=Tp(); auto* OP=Op(); auto* TV=Tv();
                    auto mat=[&](int mi,const CVector& pos)->CMatrixLink*{
                        CMatrix m{}; m.SetRotate(uni(-.5f,.5f),uni(-.5f,.5f),uni(-3.f,3.f)); m.GetPosition()=pos; std::memcpy(mats[mi].data(),&m,sizeof(CMatrix)); return reinterpret_cast<CMatrixLink*>(mats[mi].data()); };
                    auto place=[&](CEntity* e,int mi,const CVector& pos,int type,bool useMat){
                        e->SetType(eEntityType(type)); if(useMat)e->m_matrix=mat(mi,pos); else { e->m_matrix=nullptr; e->SetPosn(pos); } };
                    static const int WT[]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};
                    constexpr uint32 NWT=sizeof(WT)/sizeof(WT[0]);
                    auto weapon=[&](int slot,int type,int state,uint32 nextShot){
                        const size_t b=P_WEAPONS+size_t(slot)*0x1C; At<int32>(PL,b)=type; At<int32>(PL,b+4)=state; At<uint32>(PL,b+0x10)=nextShot; };
                    auto padWalk=[&]()->int{ static const int v[]{0,0,1,-1,10,-10,30,-30,50,51,-50,-51,60,-60,100,-100,127,-128,127,-128}; return chance(65) ? v[R(20)] : int(R(256))-128; };
                    auto angle=[&]()->float{ static const float v[]{0.f,0.5f,-0.5f,1.5707964f,-1.5707964f,3.1415927f,-3.1415927f,6.2831855f,-6.2831855f,100.f}; return chance(35) ? v[R(10)] : uni(-7.f,7.f); };
                    const float qnan=std::bit_cast<float>(0x7FC00000u);

                    // ---- mundo comun: jugador, objetivos, inteligencias, datos de jugador, pad, camaras
                    const CVector pp{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,20.f)};
                    place(PL,M_PL,pp,ENTITY_TYPE_PED,true);
                    At<uint32>(PL,P_INTEL)=uint32(uintptr_t(intels[I_PL].data())); At<uint32>(PL,P_PDATA)=uint32(uintptr_t(pdata.data())); At<uint32>(PL,P_CLUMP)=uint32(uintptr_t(fakeClump.data()));
                    At<int32>(PL,P_TYPE)=0; At<uint32>(PL,P_ANIMGROUP)=chance(50) ? 54u : (chance(50) ? (55u+R(2)) : R(60));
                    place(TP,M_TP,CVector{pp.x+uni(-10.f,10.f),pp.y+uni(-10.f,10.f),pp.z+uni(-2.f,2.f)},ENTITY_TYPE_PED,chance(60));
                    At<uint32>(TP,P_INTEL)=uint32(uintptr_t(intels[I_TP].data())); At<int32>(TP,P_TYPE)=int(R(4)); At<uint32>(TP,P_PEDSTATE)=chance(20) ? (54u+R(2)) : R(30);
                    At<uint32>(TP,P_FLAGS3)= chance(15) ? 0x100000u : 0u; At<uint32>(TP,P_FLAGS)= chance(15) ? 0x100u : 0u;
                    place(OP,M_OP,CVector{pp.x+uni(-10.f,10.f),pp.y+uni(-10.f,10.f),pp.z},ENTITY_TYPE_PED,false);
                    At<uint32>(OP,P_INTEL)=uint32(uintptr_t(intels[I_OP].data())); At<uint32>(OP,P_FLAGS)= chance(15) ? 0x100u : 0u;
                    place(TV,M_TV,CVector{pp.x+uni(-10.f,10.f),pp.y+uni(-10.f,10.f),pp.z},ENTITY_TYPE_VEHICLE,chance(50));
                    At<uint32>(TV,0x40)= chance(40) ? (4u|(chance(50) ? 8u : 0u)) : R(16);      // banderas fisicas (+0x40): bit 2 / bit 3
                    uintptr_t tgt=0; { const uint32 k=R(100); if(k<35)tgt=0; else if(k<70)tgt=uintptr_t(TP); else if(k<82)tgt=uintptr_t(TP); else tgt=uintptr_t(TV); }
                    At<uint32>(PL,P_TARGETED)=uint32(tgt);
                    At<uint32>(PL,P_MOUSE3)= chance(20) ? uint32(uintptr_t(OP)) : 0u;
                    At<uint32>(PL,P_ATTACHED)= chance(8) ? uint32(uintptr_t(TV)) : 0u;
                    At<uint32>(PL,P_STANDING)= chance(30) ? uint32(uintptr_t(TV)) : 0u;
                    At<uint32>(PL,P_FIRE)= chance(8) ? 1u : 0u;
                    At<uint8>(PL,P_CONTACT)=uint8(R(180)); At<uint32>(PL,P_MOVESTATE)=chance(10) ? 0x80000000u : R(9);
                    At<uint32>(PL,P_IKFLAGS)= chance(50) ? next() & ~8u : 0u;
                    for(int s=0;s<13;++s)weapon(s,WT[R(NWT)],int(R(5)),next());
                    { const int slot=int(R(13)); At<uint8>(PL,P_WSLOT)=uint8(slot); }
                    const int wtype=WT[R(NWT)]; const int slotNow=At<uint8>(PL,P_WSLOT);
                    weapon(slotNow,wtype,int(R(5)),CTimer::GetTimeInMS()+uint32(int(R(5))-2));
                    cfg.skill=(wtype>=22&&wtype<=32) ? int(R(4)) : 1;
                    // datos de jugador
                    At<float>(pdata.data(),D_BLEND)= chance(30) ? 0.f : uni(0.f,2.5f);
                    At<uint8>(pdata.data(),D_CHOSEN)= chance(75) ? uint8(slotNow) : uint8((slotNow+1+R(3))%13);
                    At<uint32>(pdata.data(),D_FLAGS)= (chance(30) ? 4u : 0u)|(chance(30) ? 8u : 0u);
                    At<float>(pdata.data(),D_PITCH)=uni(-1.f,1.f); At<uint8>(pdata.data(),D_SPRINTOFF)= chance(15); At<uint8>(pdata.data(),D_NOWCHANGE)=0;
                    At<float>(pdata.data(),D_ATTACK)=uni(0.f,5.f);
                    { static const float fy[]{-1.f,-0.5f,-0.50001f,-0.49999f,0.f,0.5f,1.f}; At<float>(pdata.data(),D_FIGHT+4)= chance(8) ? qnan : (chance(70) ? fy[R(7)] : uni(-1.f,1.f)); At<float>(pdata.data(),D_FIGHT)=uni(-1.f,1.f); }
                    // pad
                    At<int16>(padBuf.data(),PAD_WALK)= chance(25) ? 1 : 0; At<uint16>(padBuf.data(),PAD_DISABLE)= chance(10) ? 1 : 0;
                    for(int b=0;b<PB_COUNT;++b)cfg.padB[b]= chance(b==PB_TARGET ? 50 : (b==PB_SHIFTL||b==PB_SHIFTR||b==PB_ENTER ? 25 : 30));
                    cfg.walkUD=padWalk(); cfg.walkLR=padWalk(); cfg.walkUDp=chance(40) ? cfg.walkUD : padWalk(); cfg.walkLRp=chance(40) ? cfg.walkLR : padWalk();
                    cfg.aimLR=chance(30) ? 101+int(R(27)) : padWalk(); cfg.aimUD=chance(30) ? -101-int(R(27)) : padWalk();
                    cfg.getWeapon= chance(50) ? int(R(3)) : 0; cfg.meleeJD[0]=chance(50) ? int(R(5)) : 0; cfg.meleeJD[1]=chance(50) ? int(R(5)) : 0; cfg.getMelee=chance(40);
                    cfg.weaponJD=chance(40); cfg.use2P=chance(35); cfg.camFirst=chance(25); cfg.canDuck=chance(60); cfg.inUse=chance(40); cfg.pistolWhip=chance(15);
                    cfg.kicking=chance(40); cfg.canVeh=chance(75); cfg.hasToBreak=chance(15); cfg.sameGroup=chance(30); cfg.affects=chance(70); cfg.seeing=chance(60);
                    cfg.lockOn=chance(50); cfg.justAtt=chance(50); cfg.surfCant=chance(25); cfg.ikLooking=chance(50); cfg.fireRet=chance(50); cfg.sameHier=chance(35);
                    cfg.stealth[0]=chance(50); cfg.stealth[1]=chance(60); for(auto& a:cfg.allow)a=chance(70);
                    { static const float sv[]{0.f,1.f,0.9999f,1.0001f,1.5f,-1.f,0.f}; cfg.sprint= chance(8) ? qnan : sv[R(7)]; }
                    { static const float stv[]{0.f,1.f,-1.f,0.5f,-0.f}; cfg.stat= chance(8) ? qnan : stv[R(5)]; }
                    cfg.touchDelta=10000;
                    cfg.group=uintptr_t(grp.data()); cfg.groupPed= chance(50) ? uintptr_t(TP) : 0;
                    cfg.activePrimary= chance(40) ? uintptr_t(Fake(1)) : 0; At<int32>(Fake(1),FK_TYPE)= chance(50) ? TASK_COMPLEX_REACT_TO_GUN_AIMED_AT : TASK_SIMPLE_PAUSE;
                    // camaras (globales reales, restauradas al terminar la ejecucion)
                    static const uint32 cmodes[]{0,4,7,8,0x22,0x2D,0x2E,0x33,0x35,5,10,0x36};
                    for(int n=0;n<3;++n){ Gl<uint16>(G_CAMS+n*CAM_SIZE)=uint16(cmodes[R(12)]); Gl<float>(G_CAMS+n*CAM_SIZE+CAM_FRONT)=uni(-1.f,1.f); Gl<float>(G_CAMS+n*CAM_SIZE+CAM_FRONT+4)=chance(5) ? 0.f : uni(-1.f,1.f); Gl<float>(G_CAMS+n*CAM_SIZE+CAM_FRONT+8)=uni(-1.f,1.f); }
                    Gl<uint8>(G_ACTIVECAM)=uint8(R(3)); Gl<float>(G_ORIENT)=angle(); Gl<uint8>(G_MOUSE3)=chance(20); Gl<uint8>(G_JUMPED)=chance(15); Gl<uint8>(G_WIDE)=chance(10);
                    Gl<uint32>(G_PLAYERS1)= chance(12) ? uint32(uintptr_t(OP)) : 0u;
                    Gl<int32>(G_LASTRAND)=int(R(6))-1; Gl<uint32>(G_LASTTOUCH)=chance(50) ? 0u : R(30000); Gl<float>(G_DUCKBLEND)= chance(70) ? 4.f : uni(0.f,10.f); Gl<uint8>(G_UNKNOWN)= chance(80);
                    // grupos de animacion falsos (tabla intercambiada por el puntero real) y bloques falsos
                    for(int n=0;n<4;++n){ std::memset(fblocks[n].data(),0,0x20); fblocks[n][0x10]=uint8(chance(70)); }
                    for(uint32 g=0;g<Gl<uint32>(G_NUMDEFS);++g)At<uint32>(fgroups.data(),size_t(g)*0x14)= chance(20) ? 0u : uint32(uintptr_t(fblocks[R(4)].data()));
                    Gl<uintptr_t>(G_ASSOCGROUPS)=uintptr_t(fgroups.data());
                    // asociaciones falsas: 0/1 devueltas por BlendAnimation, 2..5 lista del clump, 4/5 tambien para GetAssociation
                    for(int n=0;n<6;++n){ std::memset(assocs[n].data(),0,0x40); static const float ba[]{-2.f,0.f,0.3f,0.5f,0.50001f,1.f,2.f}; At<float>(assocs[n].data(),0x18)= chance(10) ? qnan : ba[R(7)];
                        At<uint16>(assocs[n].data(),0x2E)=uint16(chance(50) ? 0x200u : 0u)|uint16(R(2)); At<float>(assocs[n].data(),0x1C)=uni(-1.f,1.f); }
                    cfg.assocKP= chance(50) ? uintptr_t(assocs[4].data()) : 0; cfg.assocOther= chance(40) ? uintptr_t(assocs[5].data()) : 0;
                    At<uint32>(sassocs[0].data(),0x10)=0xB4EA40u+0x18u*R(40); At<uint32>(sassocs[1].data(),0x10)= cfg.sameHier ? At<uint32>(sassocs[0].data(),0x10) : 0xB4EA40u+0x18u*(40u+R(40));
                    for(int n=0;n<2;++n)At<float>(sassocs[n].data(),0x18)=0.f;

                    // ---- tareas de la inteligencia del jugador (secundarias: 0 ataque, 1 agacharse, 4 parcial/sujetar; primarias en +4)
                    CTask* qAttack=Quiet(0); CTask* qDuck=Quiet(1); CTask* qHold=Quiet(2);
                    { const uint32 k=R(100);
                      if(k<72){ At<CTask*>(intels[I_PL].data(),I_SECONDARY)=qAttack; At<int32>(qAttack,FK_TYPE)= k<35 ? TASK_SIMPLE_USE_GUN : k<55 ? TASK_SIMPLE_FIGHT : k<65 ? TASK_SIMPLE_THROW_PROJECTILE : TASK_SIMPLE_PAUSE; } }
                    At<uint32>(qAttack,0x30)=uint32(uintptr_t(winfo.data()));
                    At<uint32>(winfo.data(),0x18)=next()&7u|(next()&0x1F8u);            // bCanAim, bAimWithArm, b1stPerson, ...
                    if(chance(55)){ At<CTask*>(intels[I_PL].data(),I_SECONDARY+4)=qDuck; At<int32>(qDuck,FK_TYPE)=TASK_SIMPLE_DUCK; At<int8>(qDuck,0x18)=int8(chance(20)); At<int8>(qDuck,0x19)=int8(chance(20)); }
                    if(chance(15)){ At<CTask*>(intels[I_PL].data(),I_SECONDARY+16)=qHold; At<int32>(qHold,FK_TYPE)=TASK_SIMPLE_HOLD_ENTITY; At<uint32>(qHold,0x38)= chance(50) ? uint32(uintptr_t(assocs[5].data())) : 0u; }
                    if(chance(25)){ At<CTask*>(intels[I_PL].data(),I_PRIMARY+4*3)=Quiet(3); At<int32>(Quiet(3),FK_TYPE)= chance(50) ? TASK_COMPLEX_JUMP : (chance(50) ? TASK_SIMPLE_PAUSE : TASK_SIMPLE_STAND_STILL); }   // silenciosa: la inteligencia real la recorre
                    cfg.assocCount=0;

                    // ---- fixture especifico de cada funcion (sobreescribe lo comun)
                    std::memset(object.data(),0xCD,object.size());
                    uint8* const o=object.data(); auto W=[&](size_t offs)->uint32&{return *reinterpret_cast<uint32*>(o+offs);};
                    W(0)=0x8708EC; W(8)=1+R(3); W(0xC)=CTimer::GetFrameCounter()+uint32(int(R(5))-3); W(0x10)=next(); W(0x14)=0; W(0x18)= chance(40) ? uint32(tgt ? tgt : uintptr_t(OP)) : 0u;
                    cfg.lookEnt= chance(50) ? uintptr_t(W(0x18)) : (chance(50) ? tgt : uintptr_t(OP));
                    bool avoid=false; CPed* pedArg=PL;
                    switch(kind){
                    case K_IDLE: {
                        const uint32 id=W(8); const int first=int(R(30)), num=int(R(12)); const uintptr_t blk=G_IDLEBLOCKS+id*0x20;
                        Gl<uint8>(blk+0x10)=uint8(chance(70)); Gl<int32>(blk+0x14)=first; Gl<uint32>(blk+0x18)=uint32(num);
                        static const uint32 deltas[]{0,1,9999,10000,10001,10002,15000,20000,20001,25000,30000,60000,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu};
                        cfg.touchDelta= chance(75) ? deltas[R(15)] : next()%70000;
                        Gl<uint32>(G_LASTTOUCH)= chance(55) ? uint32(cfg.touchDelta-20000u+uint32(int(R(5))-2)) : (chance(50) ? 0u : R(30000));
                        cfg.assocCount=int(R(4));
                        for(int n=0;n<cfg.assocCount;++n){ int hi=first+int(R(uint32(num+3)))-1; if(hi<0)hi=0; if(hi>Gl<int32>(G_NUMANIMS))hi=Gl<int32>(G_NUMANIMS);
                            At<uint32>(assocs[2+n].data(),0x14)=uint32(G_ANIMS+0x18u*uint32(hi)); At<uint16>(assocs[2+n].data(),0x2E)=uint16(chance(50) ? 0x200u : 0u); }
                        At<uint32>(PL,P_FLAGS)=(chance(10)?0x4000000u:0u)|(chance(20)?4u:0u)|(chance(20)?8u:0u); At<uint32>(PL,P_FLAGS4)= chance(10) ? 2u : 0u;
                        At<uint32>(PL,P_MOVESTATE)= chance(8) ? 0x80000000u : (chance(75) ? 1u : R(7));
                        At<uint32>(PL,P_ANIMGROUP)= chance(70) ? 54u+R(3) : R(60);
                        pedArg=PL; break; }
                    case K_ZELDAWEAPON: {
                        At<uint32>(PL,P_ATTACHED)= chance(8) ? uint32(uintptr_t(TV)) : 0u;
                        if(chance(8))At<CTask*>(intels[I_PL].data(),I_SECONDARY)=nullptr;
                        else { At<CTask*>(intels[I_PL].data(),I_SECONDARY)=qAttack; At<int32>(qAttack,FK_TYPE)=TASK_SIMPLE_USE_GUN; }
                        pedArg=PL; break; }
                    case K_DUCKED: {
                        if(chance(8))At<CTask*>(intels[I_PL].data(),I_SECONDARY+4)=nullptr;
                        else { At<CTask*>(intels[I_PL].data(),I_SECONDARY+4)=qDuck; At<int32>(qDuck,FK_TYPE)=TASK_SIMPLE_DUCK; At<int8>(qDuck,0x18)=int8(chance(12)); At<int8>(qDuck,0x19)=int8(chance(12)); }
                        if(chance(55)){ At<CTask*>(intels[I_PL].data(),I_SECONDARY)=qAttack; At<int32>(qAttack,FK_TYPE)=TASK_SIMPLE_USE_GUN; } else At<CTask*>(intels[I_PL].data(),I_SECONDARY)=nullptr;
                        pedArg=PL; break; }
                    case K_ZELDA: {
                        avoid=chance(20); cfg.padB[PB_JUMP]=chance(55); cfg.padB[PB_TARGET]=chance(30); cfg.padB[PB_DUCK]=chance(40);
                        if(chance(55)){ At<CTask*>(intels[I_PL].data(),I_PRIMARY+4*3)=Quiet(3); At<int32>(Quiet(3),FK_TYPE)= chance(40) ? TASK_COMPLEX_JUMP : TASK_SIMPLE_STAND_STILL; }
                        At<uint32>(PL,P_ATTACHED)= chance(5) ? uint32(uintptr_t(TV)) : 0u;
                        { const uint32 id=W(8); const uintptr_t blk=G_IDLEBLOCKS+id*0x20; Gl<uint8>(blk+0x10)=uint8(chance(70)); Gl<int32>(blk+0x14)=int(R(30)); Gl<uint32>(blk+0x18)=R(12); }
                        { static const uint32 deltas[]{0,1,9999,10000,10001,15000,20001,30000}; cfg.touchDelta=deltas[R(8)]; Gl<uint32>(G_LASTTOUCH)=chance(50) ? 0u : R(10000); }
                        cfg.assocCount=int(R(3)); for(int n=0;n<cfg.assocCount;++n)At<uint32>(assocs[2+n].data(),0x14)=G_ANIMS+0x18u*R(60);
                        At<uint32>(PL,P_FLAGS)=(chance(8)?0x4000000u:0u)|(chance(12)?0x200u:0u)|(chance(10)?4u:0u); At<uint32>(PL,P_FLAGS4)= chance(5) ? 2u : 0u;
                        { const float step=CTimer::GetTimeStep()*0.07f; static const float dv[]{0.f,1.f,-1.f,0.9999f,1.0001f,2.f,-2.f,0.5f}; const float ratio=chance(50) ? uni(0.f,1.f) : 0.f;
                          At<float>(pdata.data(),D_BLEND)= chance(10) ? qnan : ratio - step*dv[R(8)] + (chance(30) ? uni(-0.001f,0.001f) : 0.f); }
                        At<uint32>(PL,P_MOVESTATE)=R(9);
                        pedArg=PL; break; }
                    case K_WEAPON: {
                        pedArg=PL;
                        { static const float mb[]{0.f,1.5f,1.89999f,1.9f,1.90001f,2.f,0.5f}; At<float>(pdata.data(),D_BLEND)= chance(5) ? qnan : mb[R(7)]; }
                        At<uint32>(PL,P_MOVESTATE)= chance(15) ? 7u : (chance(50) ? 1u : R(9));
                        At<uint32>(PL,P_FLAGS)=(chance(15)?0x4000000u:0u)|(chance(25)?0x40u:0u);
                        At<uint32>(TP,P_FLAGS3)= chance(15) ? 0x100000u : 0u;
                        if(chance(30))At<uint32>(object.data(),0xC)=CTimer::GetFrameCounter()-2u-R(3);
                        // secundaria de ataque: sin tarea, UseGun, Fight, ThrowProjectile u otra
                        { const uint32 k=R(100); const int ty= k<30 ? TASK_SIMPLE_USE_GUN : k<50 ? TASK_SIMPLE_FIGHT : k<62 ? TASK_SIMPLE_THROW_PROJECTILE : k<68 ? TASK_SIMPLE_PAUSE : 0;
                          if(ty){ At<CTask*>(intels[I_PL].data(),I_SECONDARY)=qAttack; At<int32>(qAttack,FK_TYPE)=ty; } else At<CTask*>(intels[I_PL].data(),I_SECONDARY)=nullptr; }
                        if(chance(35)){ At<CTask*>(intels[I_PL].data(),I_SECONDARY+4)=qDuck; At<int32>(qDuck,FK_TYPE)=TASK_SIMPLE_DUCK; }
                        cfg.assocCount=int(R(3)); for(int n=0;n<cfg.assocCount;++n){ At<uint32>(assocs[2+n].data(),0x14)=G_ANIMS+0x18u*R(60); }
                        if(chance(20)){   // escenario de muerte sigilosa: arma cuerpo a cuerpo, objetivo ped, boton de apuntar, grupo de animacion cargado, sin tarea secundaria
                            static const int mel[]{0,1,2,3,4,5,6,7,8,9,15};
                            const int nt=mel[R(11)]; weapon(slotNow,nt,int(R(2)),0); cfg.skill=1;
                            At<uint32>(PL,P_TARGETED)=uint32(uintptr_t(TP)); At<float>(pdata.data(),D_BLEND)=chance(80) ? 0.5f : 1.9f; At<uint32>(PL,P_MOVESTATE)=chance(90) ? 1u : 7u;
                            At<uint8>(pdata.data(),D_CHOSEN)=uint8(slotNow);
                            At<CTask*>(intels[I_PL].data(),I_SECONDARY)=nullptr; At<uint32>(PL,P_FLAGS)&=~0x4000000u; if(chance(25))At<uint32>(PL,P_FLAGS)|=0x4000000u;
                            cfg.padB[PB_TARGET]=1; cfg.stealth[0]=chance(85); cfg.stealth[1]=chance(80); cfg.meleeJD[1]=chance(80) ? 1+int(R(4)) : 0; cfg.meleeJD[0]=cfg.meleeJD[1];
                            At<uint32>(TP,P_PEDSTATE)=R(30);
                            const uint32 ag=uint32(CWeaponInfo::GetWeaponInfo(eWeaponType(nt),eWeaponSkill::STD)->m_eAnimGroup);
                            if(ag&&ag<Gl<uint32>(G_NUMDEFS)){ fblocks[0][0x10]=1; At<uint32>(fgroups.data(),size_t(ag)*0x14)=uint32(uintptr_t(fblocks[0].data())); }
                            for(int n=0;n<2;++n)At<float>(assocs[n].data(),0x18)=chance(85) ? 1.f : 0.3f;
                        }
                        if(chance(8)){    // escenario de la camara de fotos: tiempo vencido y modo de camara activo = MODE_CAMERA
                            weapon(slotNow,43,0,CTimer::GetTimeInMS()-uint32(R(3))); cfg.skill=1; cfg.getWeapon=1; At<uint8>(pdata.data(),D_CHOSEN)=uint8(slotNow);
                            At<uint32>(PL,P_MOVESTATE)=R(7); At<uint32>(PL,P_FIRE)=0; Gl<uint16>(G_CAMS+size_t(Gl<uint8>(G_ACTIVECAM))*CAM_SIZE)= chance(85) ? 0x2E : 4;
                        }
                        break; }
                    }

                    // la tarea probada vive en `object`; el objetivo de la mirada se registra como referencia solo si lo escribe la funcion
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t target= fn.address;
                    // despues de State(), no antes
                    const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;
                    void* self=o; json ret=nullptr;
                    uint16 fpuTag=0xFFFF; unsigned fpAfter=0;
                    {
                        _controlfp(_PC_24,_MCW_PC);                                   // x87 a 24 bits como el juego
                        switch(kind){
                        case K_IDLE: case K_WEAPON: reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(self,pedArg); break;
                        case K_ZELDAWEAPON: case K_DUCKED:
                            // estaticas: `ret 4` en el EXE (ecx sin usar), cdecl en la DLL
                            if(candidate)reinterpret_cast<void(__cdecl*)(CPed*)>(dllAddr[f])(pedArg); else reinterpret_cast<void(__stdcall*)(CPed*)>(target)(pedArg);
                            break;
                        case K_ZELDA: ret=int32(reinterpret_cast<int32(__thiscall*)(void*,CPed*,bool)>(target)(self,pedArg,avoid)); break;
                        }
                        fpuTag=FpuTag(); fpAfter=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                    }
                    for(auto* h:hooks)h->State(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back(TaskSummary(t));
                    const json objectWords=Region(o,0x1C,true);                       // el objeto probado (CTaskSimplePlayerOnFoot mide 0x1C)
                    const json guard=Region(o+0x1C,object.size()-0x1C,false);         // y su relleno: ninguna funcion debe escribir fuera del objeto
                    auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    json refs=json::array(); for(const auto& b:bufs)if(b.ent)refs.push_back(refList(reinterpret_cast<CEntity*>(b.p)));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),FK_SIZE,true));
                    for(int n=0;n<QUIET;++n)fakeWords.push_back(Region(quiet[n].data(),FK_SIZE,true));
                    json ents=json::array(); for(const auto& b:bufs)ents.push_back(b.ent ? EntHash(b) : (b.p==object.data() ? std::string("-") : Hash(b.p,b.n)));
                    json r={{"ret",ret},{"object",objectWords},{"guard",guard},{"tasks",tasks},{"fakes",fakeWords},{"refs",refs},{"buffers",ents},{"globals",globalsHash()},
                            {"fp",fpAfter},{"fpu_tag",fpuTag}};
                    // borrado de las tareas creadas
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    json refsAfter=json::array(); for(const auto& b:bufs)if(b.ent)refsAfter.push_back(refList(reinterpret_cast<CEntity*>(b.p)));
                    r["refs_after"]=refsAfter;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["rng"]=randIdx; r["clean"]=clean; r["switches"]=switches;
                    restoreGlobals();
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
    restoreGlobals();
    *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC)=savedLastFree;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&_controlfp(0,0)==savedFP&&Gl<uintptr_t>(G_ASSOCGROUPS)==realGroups;
    for(auto& r:ranges)restored&=r.saved==ReversingProbe::Bytes(reinterpret_cast<void*>(r.a),r.n);
    for(auto& [a,b]:entryBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(a),8);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
