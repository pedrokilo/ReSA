#pragma once
#include "ConversationForPed.h"
#include "Conversations.h"
#include "../ProbeSupport.hpp"

namespace CForPedProbe {
inline bool active{};
inline uint32 updateCalls{}, positionCalls{};
struct Spec {
    std::string name;
    bool positionOnly{}, random{}, enabled{true}, participant{true}, suppress{}, yes{}, no{};
    bool playerReady{true}, pedReady{true}, damaged{}, matrix{true};
    uint32 state{2}, time{60000}, last{59000}, close{59000}, damageTime{};
    uint32 precision{0xFFFFFFFFu};
    int followers{}, speech{42}, speechY{43}, speechN{44};
    int16 nodeYes{1}, nodeNo{2};
    CVector pos{0.f,2.f,0.f}, playerPos{0.f,0.f,0.f};
    CVector forward{0.f,-1.f,0.f}, playerForward{0.f,1.f,0.f}, speed{0.f,0.f,0.f};
};
inline Spec spec;
inline CPlayerPed* player{};
inline CPed* ped{};
inline CConversationForPed* conversation{};
inline json trace;
inline char translation[64]{};
inline int Id(const CEntity* p) { return !p ? 0 : p==player ? 1 : p==ped ? 2 : 3; }
inline void Record(const char* name,json args=json::array()) {
    trace.push_back({name,args,{conversation->m_CurrentNode,int(conversation->m_Status),conversation->m_LastChange,conversation->m_LastTimeWeWereCloseEnough}});
}
inline CPlayerPed* Player(int32) { return player; }
inline CVector Coors(int32) { return player->GetPosition(); }
inline bool Ready(CPed* p,bool random) { Record("ready",{Id(p),random}); return p==player ? spec.playerReady : spec.pedReady; }
inline int32 Members() { Record("followers"); return spec.followers; }
inline bool Yes() { Record("yes");return spec.yes; }
inline bool No() { Record("no");return spec.no; }
inline void ClearMessages() { Record("clear"); }
inline const GxtChar* Text(const char* key) {
    Record("text",{key});strcpy(translation,key);return reinterpret_cast<const GxtChar*>(translation);
}
inline void Message(const char* kind,const GxtChar* text,uint32 time,uint16 flags,bool brief) {
    Record(kind,{reinterpret_cast<const char*>(text),time,flags,brief});
}
inline void Jump(const GxtChar* text,uint32 time,uint16 flags,bool brief) { Message("jump",text,time,flags,brief); }
inline void Queue(const GxtChar* text,uint32 time,uint16 flags,bool brief) { Message("queue",text,time,flags,brief); }
inline int16 Say(CPed* p,eGlobalSpeechContext ctx,uint32 delay=0,float probability=1.f,bool silence=false,bool audible=false,bool frontend=false) {
    Record("say",{Id(p),int(ctx),delay,probability,silence,audible,frontend});return 17;
}
inline void Awkward(int32 speech,CPed* p) { Record("awkward",{speech,Id(p)}); }
inline bool __fastcall ReadyABI(CPed* p,void*,bool random) { return Ready(p,random); }
inline int32 __fastcall MembersABI(void*,void*) { return Members(); }
inline bool __fastcall YesABI(void*,void*) { return Yes(); }
inline bool __fastcall NoABI(void*,void*) { return No(); }
inline const GxtChar* __fastcall TextABI(void*,void*,const char* key) { return Text(key); }
inline int16 __fastcall SayABI(CPed* p,void*,eGlobalSpeechContext ctx,uint32 delay,float probability,bool silence,bool audible,bool frontend) { return Say(p,ctx,delay,probability,silence,audible,frontend); }
}
