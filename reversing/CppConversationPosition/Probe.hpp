#pragma once
// Proveedores de dependencias derivados de ../ConversationForPed/Probe.hpp (solo los de posicion).
// Se instalan con EntryPatch tanto en la direccion del EXE como en la funcion de la DLL; ya no
// hay instrumentacion textual del cuerpo candidato.
#include "ConversationForPed.h"
#include "Conversations.h"
#include "../ProbeSupport.hpp"

namespace CppConversationPositionProbe {
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
inline int Id(const CEntity* p) { return !p ? 0 : p==player ? 1 : p==ped ? 2 : 3; }
inline void Record(const char* name,json args=json::array()) {
    trace.push_back({name,args,{conversation->m_CurrentNode,int(conversation->m_Status),conversation->m_LastChange,conversation->m_LastTimeWeWereCloseEnough}});
}
inline CPlayerPed* Player(int32) { return player; }
inline CVector Coors(int32) { return player->GetPosition(); }
inline bool Ready(CPed* p,bool random) { Record("ready",{Id(p),random}); return p==player ? spec.playerReady : spec.pedReady; }
inline int32 Members() { Record("followers"); return spec.followers; }
inline bool __fastcall ReadyABI(CPed* p,void*,bool random) { return Ready(p,random); }
inline int32 __fastcall MembersABI(void*,void*) { return Members(); }
}
