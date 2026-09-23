#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Conversations.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace ConversationsLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline int Report(int type,char* msg,int*) { if(type!=_CRT_ASSERT)return FALSE; Write({{"event","assertion"},{"message",msg ? msg:""}}); ExitProcess(91); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"conversations-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"conversations-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; auto report=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CConversations")for(auto& h:c.Items())if(h->Name()=="DoneSettingUpConversation")hook=h.get();});
    if(!hook){Write({{"event","error"}});_CrtSetReportHook(report);return;}
    auto* globals=reinterpret_cast<uint8*>(0x9691C4); constexpr size_t size=0x969A20-0x9691C4;
    auto saved=ReversingProbe::Bytes(globals,size); const auto savedPlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false); bool passed=true; int pair=0;
    using C=CConversations; auto entry=reinterpret_cast<void(__cdecl*)(bool)>(0x43ADB0);
    for(int i=0;i<2048 && passed;++i) {
        alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> a{},b{}; auto* pa=reinterpret_cast<CPlayerPed*>(a.data()); auto* pb=reinterpret_cast<CPlayerPed*>(b.data());
        const auto initial=[&](CPed* p) {
            std::memset(globals,0x5A,size);
            C::m_AwkwardSayStatus=C::eAwkwardSayStatus::PLAYING; C::m_SettingUpConversation=true; C::m_SettingUpConversationNumNodes=i%13; C::m_SettingUpConversationPed=p;
            for(int n=0;n<14;++n) C::m_Conversations[n].m_pPed=n<i%14 ? player:nullptr;
            for(int n=0;n<50;++n) { auto& node=C::m_Nodes[n]; node.m_Name[0]=((i/13)%4==0 || (n+i)%5==0) ? 'O':0; node.m_Name[1]=0; }
            for(int n=0;n<12;++n) {
                auto& t=C::m_aTempNodes[n]; sprintf_s(t.m_Name,"N%d",(i&16) ? n%4:n);
                sprintf_s(t.m_NameNodeYes,"%c%d",(i&32) ? 'n':'N',(n+1)%12);
                sprintf_s(t.m_NameNodeNo,"N%d",(n+3)%12); if(i&64)t.m_NameNodeNo[0]=0;
                t.m_FinalSlot=(n+i)%50; t.m_Speech=i+n; t.m_SpeechY=i-n; t.m_SpeechN=-i-n;
            }
        };
        auto run=[&](CPed* p) {
            initial(p);entry(i%2);
            auto conversations=C::m_Conversations; for(auto& c:conversations)if(c.m_pPed==p)c.m_pPed=reinterpret_cast<CPed*>(1);
            json out={{"conversations",ReversingProbe::Bytes(conversations.data(),sizeof(conversations))},{"nodes",ReversingProbe::Bytes(C::m_Nodes.data(),sizeof(C::m_Nodes))},{"temp",ReversingProbe::Bytes(C::m_aTempNodes.data(),sizeof(C::m_aTempNodes))},{"count",C::m_SettingUpConversationNumNodes},{"setting",C::m_SettingUpConversation},{"awkward",int(C::m_AwkwardSayStatus)},{"ped",C::m_SettingUpConversationPed==p},{"reference",p->m_pReferences!=nullptr}};
            auto& slot=C::m_Conversations[i%14]; p->CleanUpOldReference(slot.m_pPed); return out;
        };
        hook->State(false);auto start=ConversationsProbe::calls;auto original=run(pa);auto middle=ConversationsProbe::calls;
        hook->State(true);auto candidate=run(pb);auto end=ConversationsProbe::calls;
        const bool intact=std::ranges::all_of(a,[](uint8 x){return !x;}) && std::ranges::all_of(b,[](uint8 x){return !x;});
        passed=original==candidate&&intact&&middle==start&&end==middle+1;
        Write({{"event","comparison"},{"pair",pair++},{"original",original},{"candidate",candidate},{"equal",passed},{"inputs_restored",intact},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    std::memcpy(globals,saved.data(),size);hook->State(enabled);hook->LockState(locked);
    const bool restored=ReversingProbe::Bytes(globals,size)==saved&&ReversingProbe::Bytes(player,sizeof(CPlayerPed))==savedPlayer;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
