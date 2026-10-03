#pragma once
#include <fstream>
#include <crtdbg.h>
#include "PlayerPed.h"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
namespace SprintLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { { static DWORD pumpLast=0; const DWORD pumpNow=GetTickCount(); if(pumpNow-pumpLast>=250){ pumpLast=pumpNow; MSG pumpMsg; PeekMessageA(&pumpMsg,nullptr,0,0,PM_NOREMOVE); } } log << row.dump() << '\n'; log.flush(); }
inline int Report(int type,char* message,int*) { if(type!=_CRT_ASSERT)return FALSE; Write({{"event","assertion"},{"message",message?message:""}});ExitProcess(91); }
inline bool Select(bool enabled) {
    bool found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
        if(category.Name()!="CPlayerPed")return;
        for(auto& hook:category.Items()) if(hook->Name()=="GetButtonSprintResults") {category.SetItemEnabled(hook,enabled);found=hook->Hooked()==enabled;}
    });return found;
}
struct Result { uint32 bits; bool edx; };
inline Result Call(CPlayerPed* player,int sprintMode) {
    uint32 animation[16]{};
    uint32 edxAfter{};
    const auto entry=(void*)0x60A820;
    __asm {
        mov ecx,player
        push sprintMode
        lea edx,animation
        call entry
        mov edxAfter,edx
        fstp dword ptr [edx + 24h]
    }
    return {animation[9],edxAfter==(uint32)animation};
}
inline void Update() {
    if(done)return;
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);
    const auto directory=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(directory/"sprint-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(directory/"sprint-runtime.jsonl",std::ios::trunc);Write({{"event","waiting_for_player"}});_CrtSetReportHook(Report);}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;
    done=true;
    Write({{"event","player_ready"},{"startup_candidate_calls",SprintProbe::calls}});
    auto& speed=player->GetPlayerData()->m_fMoveSpeed;
    const auto savedSpeed=speed;
    int pair{};
    for(int index=0;index<1016;++index) {
        const uint32 special[]={0,0x80000000,0x7f800000,0xff800000,0x7fc00000,0xffc00000,0x40a00000,0x409fffff,0x40a00001,1,0x80000001,0x7f7fffff,0xff7fffff,0x3f800000,0xbf800000,0x00800000};
        if(index<16)memcpy(&speed,&special[index],4);else speed=(index-116)*0.1f;
        uint32 speedBits;memcpy(&speedBits,&speed,4);
        for(int mode=0;mode<4;++mode) {
            const auto before=SprintProbe::calls;
            if(!Select(false)){speed=savedSpeed;Write({{"event","error"},{"reason","original selection failed"}});return;}
            const auto original=Call(player,mode);const auto middle=SprintProbe::calls;
            if(!Select(true)){speed=savedSpeed;Write({{"event","error"},{"reason","candidate selection failed"}});return;}
            const auto candidate=Call(player,mode);const auto after=SprintProbe::calls;
            const bool equal=original.bits==candidate.bits&&original.edx&&candidate.edx&&middle==before&&after==middle+1;
            Write({{"event","comparison"},{"pair",pair++},{"mode",mode},{"speed_bits",speedBits},{"original_bits",original.bits},{"candidate_bits",candidate.bits},{"edx_preserved",original.edx&&candidate.edx},{"equal",equal},{"original_candidate_calls",middle-before},{"candidate_calls",after-middle}});
            if(!equal){speed=savedSpeed;Write({{"event","error"},{"reason","output or EDX mismatch"}});return;}
        }
    }
    speed=savedSpeed;
    Write({{"event","complete"},{"pairs",pair},{"speed_restored",speed==savedSpeed}});
}
}
