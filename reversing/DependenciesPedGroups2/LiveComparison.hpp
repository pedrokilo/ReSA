#pragma once
#include <fstream>
#include "PedGroups.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesPedGroups2Live {
inline bool done{};
inline std::ofstream log;
struct Fn { const char* cls; const char* name; uintptr_t address,vtable; int slot,cases; };
inline constexpr Fn fns[]{
{"CPedGroups","IsGroupLeader",0x5F7E40,0,-1,4096},
{"CPedGroups","GetGroupId",0x5F7EE0,0,-1,256},
{"CPedGroups","IsInPlayersGroup",0x5F7F10,0,-1,512},
{"CPedGroups","AreInSameGroup",0x5F7F40,0,-1,8192}
};
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log<<row.dump()<<'\n';log.flush(); }
// Cdecl call gate: ESP and callee-saved register sentinels, all checked before logging.
__declspec(noinline) inline uint32 Call(uintptr_t address,uintptr_t arg0,uintptr_t arg1,bool two,bool boolean,bool& abi) {
    uint32 result{},espBefore{},espAfter{},rB{},rS{},rD{};
    __asm {
        push ebx
        push esi
        push edi
        mov espBefore,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        cmp two,0
        je oneArg
        push arg1
    oneArg:
        push arg0
        call address
        mov result,eax
        add esp,4
        cmp two,0
        je cleaned
        add esp,4
    cleaned:
        mov espAfter,esp
        mov rB,ebx
        mov rS,esi
        mov rD,edi
        pop edi
        pop esi
        pop ebx
    }
    abi=espBefore==espAfter&&rB==0x13579BDF&&rS==0x2468ACE0&&rD==0x10293847;
    return boolean ? result&255 : result;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"depspedgroups2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"depspedgroups2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("depspedgroups2");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,4> routes;
    routes[0]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","IsGroupLeader",0x5F7E40,&CPedGroups::IsGroupLeader);
    routes[1]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","GetGroupId",0x5F7EE0,&CPedGroups::GetGroupId);
    routes[2]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","IsInPlayersGroup",0x5F7F10,&CPedGroups::IsInPlayersGroup);
    routes[3]=std::make_unique<OriginalDependencies499::HookRoute>("CPedGroups","AreInSameGroup",0x5F7F40,&CPedGroups::AreInSameGroup);
    const auto savedActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,sizeof(CPedGroups::ms_activeGroups));
    const auto savedGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
    bool passed=true,registered=true;for(auto& r:routes)registered&=r->Registered();passed&=registered;
    Write({{"event","begin"},{"known",passed},{"registered_hook",registered},{"layout",sizeof(CPedGroup)==0x2D4},{"scope","four_queries_real_RH_no_credit_collaborators"}});
    alignas(16) std::array<uint8,0x800> pedBytes{};
    const uintptr_t p1=reinterpret_cast<uintptr_t>(pedBytes.data()),p2=0x12345000;
    uint32 pair=0;
    for(unsigned fn=0;fn<4;++fn) {
        unsigned failures=0;
        for(unsigned i=0;i<unsigned(fns[fn].cases);++i) {
            auto run=[&](bool candidate) {
                std::memset(&CPedGroups::ms_groups,0xA7,sizeof(CPedGroups::ms_groups));pedBytes.fill(0);
                unsigned mask=i&255,mode=(i>>8)&31,group=(i>>8)&7,member=i&7;
                for(unsigned g=0;g<8;++g) {
                    CPedGroups::ms_activeGroups[g]=(mask&(1u<<g))?char((i&1)?0x80:1):0;
                    for(unsigned m=0;m<8;++m) {
                        uintptr_t token{};
                        if(fn==0 && m==7 && g==group && !(mode&8))token=p1;
                        if(fn==2 && m==member && !(i&128))token=p1;
                        if(fn==3) {
                            if(m==member && (g==group || (mode&8)))token=p1;
                            if(m==((member+1)&7) && g==((mode&16)?((group+1)&7):group))token=p2;
                        }
                        std::memcpy(reinterpret_cast<uint8*>(&CPedGroups::ms_groups[g])+0xC+m*4,&token,4);
                    }
                }
                uintptr_t arg0=p1,arg1=p2;
                if(fn==0 && (mode&8))arg0=0;
                if(fn==1) { unsigned which=i%16;arg0=which<8?uintptr_t(&CPedGroups::ms_groups[which]):which==8?0:uintptr_t(&CPedGroups::ms_groups[(which-9)%8])+1; }
                if(fn==2) { uintptr_t playerData=(i&256)?0x23456000:0;std::memcpy(pedBytes.data()+0x480,&playerData,4); }
                if(fn==3) { if(mode&1)arg0=0;if(mode&2)arg1=0;if(mode&4)arg1=arg0; }
                const auto beforeGroups=ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups));
                const auto beforeActive=ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,sizeof(CPedGroups::ms_activeGroups));
                const auto beforePed=pedBytes;
                bool abi{},route{};uint32 ret=routes[fn]->Invoke(candidate,[&]{route=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;return Call(fns[fn].address,arg0,arg1,fn==3,fn!=1,abi);});
                return json{{"return",ret},{"route_ok",route},{"stack_restored",abi},{"registers_preserved",abi},{"ped_unchanged",beforePed==pedBytes},{"groups_unchanged",beforeGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,sizeof(CPedGroups::ms_groups))},{"active_unchanged",beforeActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,sizeof(CPedGroups::ms_activeGroups))}};
            };
            const auto original=run(false),candidate=run(true);
            bool equal=passed&&original==candidate;for(auto key:{"route_ok","stack_restored","registers_preserved","ped_unchanged","groups_unchanged","active_unchanged"})equal&=original[key]==true;
            failures+=!equal;
            Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CPedGroups::")+fns[fn].name},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
        }
        passed&=!failures;Write({{"event","function_summary"},{"function",std::string("CPedGroups::")+fns[fn].name},{"cases",fns[fn].cases},{"failures",failures}});
    }
    std::memcpy(&CPedGroups::ms_groups,savedGroups.data(),savedGroups.size());std::memcpy(&CPedGroups::ms_activeGroups,savedActive.data(),savedActive.size());
    bool restored=savedGroups==ReversingProbe::Bytes(&CPedGroups::ms_groups,savedGroups.size())&&savedActive==ReversingProbe::Bytes(&CPedGroups::ms_activeGroups,savedActive.size());for(auto& r:routes)restored&=r->Restore();
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
