#pragma once
#include "PedToPlayerConversations.h"
#include "PedClothesDesc.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventScriptCommand.h"
#include "../ProbeSupport.hpp"

namespace P2PProbe {
using C = CPedToPlayerConversations;
inline uint32 calls{};
inline bool active{};
struct Spec {
    std::string name;
    uint32 state{}, time{60000}, start{60000}, last{};
    int topic{1}, type{PED_TYPE_CIVMALE}, seed{256}, index{-1}, slot{0};
    int vehicleClass{VEHICLE_CLASS_RICHFAMILY}, random{99}, fallback{0}, gate{};
    float respect{900}, muscle{600}, fat{0}, appeal{900}, budget{10}, wind{0}, distance{2}, carDistance{3}, health{1000};
    int hour{12};
    bool positive{true}, participant{true}, inVehicle{}, hasCar{true}, request{true}, chat{true}, hair{true}, tattoo{true};
    bool yes{}, no{}, talking{}, lookPlayer{true}, lookPed{true}, frontPlayer{true}, frontPed{true}, emptySlots{};
};
inline Spec spec;
inline CPlayerPed* player{};
inline CPed* ped{};
inline CVehicle* car{};
inline json trace = json::array();
inline int Id(const CEntity* e) { return !e ? 0 : e==player ? 1 : e==ped ? 2 : e==car ? 3 : 4; }
inline void Record(const char* name, json args = json::array()) { trace.push_back({name,args}); }
inline CPlayerPed* Player(int32) { return player; }
inline CVehicle* Vehicle(int32,bool) { return spec.inVehicle ? car : nullptr; }
inline CVector Coors(int32) { return player->GetPosition(); }
inline eWantedLevel Wanted(CPed*) { Record("wanted"); return static_cast<eWantedLevel>(spec.gate==1); }
inline bool Conversation() { Record("conversation"); return spec.gate==2; }
inline bool Ready(CPed* p,bool arg) { Record("ready",{Id(p),arg}); return spec.gate!=(p==player ? 3 : 4); }
inline bool OnScreen(CPed* p) { Record("screen",{Id(p)}); return spec.gate!=5; }
inline bool Topic(CPed* p,int16 topic) { Record("topic",{Id(p),topic}); return spec.chat; }
inline int32 RandomRange(int32 a,int32 b) { Record("range",{a,b}); return b==7 || b==10 ? spec.topic : spec.random; }
inline int32 Random() { Record("random"); return spec.fallback; }
inline float Stat(eStats s) {
    Record("stat",{int(s)});
    switch(s) {
    case STAT_CLOTHES_RESPECT:return spec.respect;
    case STAT_MUSCLE:return spec.muscle;
    case STAT_FAT:return spec.fat;
    case STAT_SEX_APPEAL:return spec.appeal;
    case STAT_HAIRDRESSING_BUDGET:return spec.budget;
    default:std::abort();
    }
}
inline bool Hair(CPedClothesDesc*,int32 arg) { Record("hair",{arg});return spec.hair; }
inline bool Tattoo(CPedClothesDesc*) { Record("tattoo");return spec.tattoo; }
inline bool Request(CPed* p) { Record("request",{Id(p)});return spec.request; }
inline void Release() { Record("release"); }
inline int16 Say(CPed* p,eGlobalSpeechContext ctx,uint32 delay=0,float probability=1.f,bool silence=false,bool audible=false,bool frontend=false) {
    Record("say",{Id(p),int(ctx),delay,probability,silence,audible,frontend,int(C::m_State),C::m_bPositiveOpening});return 7;
}
inline void Enable(CPed* p) { Record("enable",{Id(p)}); }
inline void Disable(CPed* p,bool stop) { Record("disable",{Id(p),stop}); }
inline bool Talking(CPed* p) { Record("talking",{Id(p)});return spec.talking; }
inline bool Looking(CPed* p) { Record("looking",{Id(p)});return p==player ? spec.lookPlayer : spec.lookPed; }
inline void Abort(CPed* p,int32 ms) { Record("abort",{Id(p),ms}); }
inline void LookAt(const char* name,CPed* p,CEntity* e,int32 time,eBoneTag32 bone,CVector* offset,bool torso,float speed,int32 blend,int32 priority,bool force) {
    Record("lookat",{name,Id(p),Id(e),time,int(bone),offset!=nullptr,torso,speed,blend,priority,force});
}
inline bool Yes() { Record("yes");return spec.yes; }
inline bool No() { Record("no");return spec.no; }
inline CEvent* AddEvent(CEventGroup*,CEvent* event,bool valid) {
    auto* cmd=static_cast<CEventScriptCommand*>(event);
    auto* seq=static_cast<CTaskComplexSequence*>(cmd->m_task);
    auto* stand=static_cast<CTaskSimpleStandStill*>(seq->m_Tasks[0]);
    auto* kill=static_cast<CTaskComplexKillPedOnFoot*>(seq->m_Tasks[1]);
    Record("event",{int(cmd->GetEventType()),cmd->m_primaryTaskIndex,cmd->m_affectsDeadPeds,valid,int(seq->GetTaskType()),int(stand->GetTaskType()),stand->m_nTime,stand->m_bLooped,stand->m_bUseAnimIdleStance,stand->m_fBlendData,int(kill->GetTaskType()),Id(kill->m_target),kill->m_time,kill->m_pedFlags,kill->m_actionDelay,kill->m_actionChance,kill->m_nCompetence});
    return nullptr;
}
// Adaptadores ABI de las dependencias del EXE. La candidata usa los mismos
// proveedores deterministas; ambas Update se ejecutan completas dentro del motor.
inline eWantedLevel __fastcall WantedABI(CPed* p,void*) { return Wanted(p); }
inline bool __fastcall ReadyABI(CPed* p,void*,bool a) { return Ready(p,a); }
inline bool __fastcall ScreenABI(CPed* p,void*) { return OnScreen(p); }
inline bool __fastcall TopicABI(CAEPedSpeechAudioEntity* p,void*,int16 a) { return Topic(reinterpret_cast<CPed*>(reinterpret_cast<uint8*>(p)-0x294),a); }
inline bool __fastcall HairABI(CPedClothesDesc* p,void*,int32 a) { return Hair(p,a); }
inline bool __fastcall TattooABI(CPedClothesDesc* p,void*) { return Tattoo(p); }
inline int16 __fastcall SayABI(CPed* p,void*,eGlobalSpeechContext a,uint32 b,float c,bool d,bool e,bool f) { return Say(p,a,b,c,d,e,f); }
inline void __fastcall EnableABI(CPed* p,void*) { Enable(p); }
inline void __fastcall DisableABI(CPed* p,void*,bool a) { Disable(p,a); }
inline bool __fastcall TalkingABI(CPed* p,void*) { return Talking(p); }
inline bool __fastcall LookingABI(void*,void*,CPed* p) { return Looking(p); }
inline void __fastcall AbortABI(void*,void*,CPed* p,int32 a) { Abort(p,a); }
inline void __fastcall LookAtABI(void*,void*,const char* n,CPed* p,CEntity* e,int32 t,eBoneTag32 b,CVector* o,bool u,float s,int32 l,int32 r,bool f) { LookAt(n,p,e,t,b,o,u,s,l,r,f); }
inline bool __fastcall YesABI(void*,void*) { return Yes(); }
inline bool __fastcall NoABI(void*,void*) { return No(); }
inline CEvent* __fastcall EventABI(CEventGroup* g,void*,CEvent* e,bool b) { return AddEvent(g,e,b); }
}
