#pragma once
#include <fstream>
#include <new>
#include "TaskComplexWanderStandard.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
#include <reversiblehooks/ReversibleHook/Virtual.h>
namespace RequestedWanderRemainingLive {
using namespace ReversibleHooks::ReversibleHook;
inline bool done{};
inline CTaskComplexWanderStandard* expectedSelf{};
inline CPed* expectedPed{};
inline int fixture{};
inline json trace;
inline bool Helper(const char* name,CTaskComplexWanderStandard* self,CPed* ped,int bit) {
    const bool result=(fixture&(1<<bit))!=0;
    trace.push_back({{"helper",name},{"self_matches",self==expectedSelf},{"ped_matches",ped==expectedPed},{"returned",result}});
    if(result)self->m_nMinNextScanTime=CTimer::GetTimeInMS()+100000u;
    return result;
}
inline bool __fastcall Gang(CTaskComplexWanderStandard* self,void*,CPed* ped){return Helper("gang",self,ped,2);}
inline bool __fastcall Chat(CTaskComplexWanderStandard* self,void*,CPed* ped){return Helper("chat",self,ped,3);}
inline bool __fastcall Cars(CTaskComplexWanderStandard* self,void*,CPed* ped){return Helper("cars",self,ped,4);}
struct Range{const void* address;std::vector<uint8> bytes;};
inline void Update() {
    if(done)return;done=true;
    char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);
    const auto dir=std::filesystem::path(executable).parent_path();
    if(!std::filesystem::exists(dir/"requested-wander-remaining-runtime.start"))return;
    std::ofstream log(dir/"requested-wander-remaining-runtime.jsonl");
    auto write=[&](const json& row){log<<row.dump()<<'\n';log.flush();};
    const auto flags=SmallPendingSupport::SnapshotHooks();std::vector<Range> entries,tables;
    Simple* constructor{};Virtual* scan{};
    for(const auto& state:flags){
        void* gta{};void* ours{};
        if(state.hook->Type()==Base::HookType::Simple){auto*s=static_cast<Simple*>(state.hook);gta=s->GetHookGTAAddress();ours=s->GetHookOurAddress();}
        else if(state.hook->Type()==Base::HookType::Virtual){auto*v=static_cast<Virtual*>(state.hook);gta=v->GetHookGTAAddress();ours=v->GetHookOurAddress();}
        if(gta)entries.push_back({gta,ReversingProbe::Bytes(gta,52)});
        if(ours)entries.push_back({ours,ReversingProbe::Bytes(ours,52)});
        if(state.hook->Name()=="Constructor"&&gta==reinterpret_cast<void*>(0x48E4F0))constructor=static_cast<Simple*>(state.hook);
        if(state.hook->Name()=="ScanForStuff"&&gta==reinterpret_cast<void*>(0x672600))scan=static_cast<Virtual*>(state.hook);
    }
    if(!constructor||!scan){write({{"event","error"},{"reason","missing exact hook identities"}});return;}
    HMODULE module{};
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(constructor->GetHookOurAddress()),&module);
    struct Table{const char* symbol;uintptr_t original;size_t slots;};
    const Table definitions[]{
        {"??_7CTask@@6B@",0x86D48C,7},{"??_7CTaskComplex@@6B@",0x86D4CC,11},
        {"??_7CTaskComplexWander@@6B@",0x86FE84,15},{"??_7CTaskComplexWanderStandard@@6B@",0x85A200,15}};
    uintptr_t ourStandard{};
    for(const auto& table:definitions){
        const auto ours=reinterpret_cast<uintptr_t>(GetProcAddress(module,table.symbol));
        if(!ours){write({{"event","error"},{"reason","missing named vtable export"},{"symbol",table.symbol}});return;}
        tables.push_back({reinterpret_cast<void*>(table.original),ReversingProbe::Bytes(reinterpret_cast<void*>(table.original),table.slots*4)});
        tables.push_back({reinterpret_cast<void*>(ours),ReversingProbe::Bytes(reinterpret_cast<void*>(ours),table.slots*4)});
        if(table.original==0x85A200)ourStandard=ours;
    }
    write({{"event","fixture"},{"native_standard_vptr",0x85A200},{"dll_standard_vptr",ourStandard},{"cped_bytes",sizeof(CPed)},{"helper_scope","isolated boolean short-circuit and next-minimum side effect; opaque CPed token"}});
    const auto clockBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB7C),12);
    const auto radiusBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x86FCB0),4);
    const uint32 radiusBits=*reinterpret_cast<const uint32*>(0x86FCB0);
    if(radiusBits!=0x3F000000u){write({{"event","error"},{"reason","canonical radius fixture not .5"},{"radius_bits",radiusBits}});return;}
    for(const auto& state:flags){if(state.hook->Name()=="NOTSA_WinMain")continue;state.hook->LockState(false);state.hook->State(false);}
    constructor->LockState(false);scan->LockState(false);
    scan->State(false);
    const auto scanOriginal=ReversingProbe::Bytes(scan->GetHookGTAAddress(),5);
    scan->State(true);
    const auto scanCandidate=ReversingProbe::Bytes(scan->GetHookOurAddress(),5);
    scan->State(false);
    auto entryGate=[&](bool candidate){
        const auto gta=ReversingProbe::Bytes(scan->GetHookGTAAddress(),5),dll=ReversingProbe::Bytes(scan->GetHookOurAddress(),5);
        const auto from=candidate?scan->GetHookGTAAddress():scan->GetHookOurAddress();
        const auto to=candidate?scan->GetHookOurAddress():scan->GetHookGTAAddress();
        std::array<uint8,5> jump{0xE9};const int32 delta=int32(reinterpret_cast<uintptr_t>(to)-reinterpret_cast<uintptr_t>(from)-5);
        std::memcpy(jump.data()+1,&delta,4);
        const auto& redirected=candidate?gta:dll;
        return (candidate?dll==scanCandidate:gta==scanOriginal)&&std::equal(jump.begin(),jump.end(),redirected.begin());
    };
    alignas(CTaskComplexWanderStandard)std::array<uint8,sizeof(CTaskComplexWanderStandard)> left{},right{};
    bool passed=true;int pair{},constructorFailures{},scanFailures{};
    for(int n=0;n<2048;++n){
        const uint8 poison=uint8(1+(n*37)%255);left.fill(poison);right=left;
        const eMoveState move=static_cast<eMoveState>(n%8);const uint8 direction=uint8(n/8);const bool sensible=((n/8)&1)!=0;
        json results[2];
        for(int side=0;side<2;++side){
            auto& bytes=side?right:left;auto*self=reinterpret_cast<CTaskComplexWanderStandard*>(bytes.data());
            constructor->State(side!=0);const bool gate=SmallPendingSupport::ExactSwitch(*constructor,side!=0);
            auto* returned=reinterpret_cast<CTaskComplexWanderStandard*(__thiscall*)(CTaskComplexWanderStandard*,eMoveState,uint8,bool)>(0x48E4F0)(self,move,direction,sensible);
            const uintptr_t vptr=*reinterpret_cast<uintptr_t*>(bytes.data());
            results[side]={{"bytes",ReversingProbe::Bytes(bytes.data(),bytes.size())},{"returned_this",returned==self},{"vptr",vptr},{"named_vptr_matches",vptr==(side?ourStandard:0x85A200)},{"gate",gate}};
        }
        const bool payload=std::equal(left.begin()+4,left.end(),right.begin()+4);
        const bool equal=payload&&results[0]["returned_this"]==true&&results[1]["returned_this"]==true&&results[0]["named_vptr_matches"]==true&&results[1]["named_vptr_matches"]==true&&results[0]["gate"]==true&&results[1]["gate"]==true;
        passed&=equal;constructorFailures+=!equal;
        write({{"event","comparison"},{"function","CTaskComplexWanderStandard::Constructor"},{"pair",pair++},{"seed",n},{"poison",poison},{"equal",equal},{"accepted",equal},{"raw_equal",left==right},{"payload_equal",payload},{"original",results[0]},{"candidate",results[1]},{"pointer_mapping","only named Standard vptr; all other bytes exact"}});
    }
    constructor->State(false);
    const std::array<uintptr_t,6> helpers{0x670100,std::bit_cast<uintptr_t>(&CTaskComplexWanderStandard::LookForGangMembers),0x66FDA0,std::bit_cast<uintptr_t>(&CTaskComplexWanderStandard::LookForChatPartners),0x66AFD0,std::bit_cast<uintptr_t>(&CTaskComplexWanderStandard::LookForSexyCars)};
    std::array<std::vector<uint8>,6> helperBefore;for(size_t i=0;i<6;++i)helperBefore[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[i]),5);
    alignas(CPed)std::array<uint8,sizeof(CPed)> pedStorage{};const auto pedBefore=pedStorage;expectedPed=reinterpret_cast<CPed*>(pedStorage.data());
    int initialStarts{},paused{},minimumBlocked{},expired{},waiting{},gangOnly{},chatLast{},carsLast{},wrapped{};
    {
        ReversingProbe::EntryPatch g0(helpers[0],Gang),g1(helpers[1],Gang),c0(helpers[2],Chat),c1(helpers[3],Chat),s0(helpers[4],Cars),s1(helpers[5],Cars);
        constexpr uint32 nowValues[]{0,1,49,50,51,0xFFFFFFE0u,0xFFFFFFFFu,0x80000000u};
        constexpr int32 intervals[]{0,1,49,50,51,-1,INT32_MAX,INT32_MIN};
        for(int n=0;n<4096;++n){
            const uint32 now=nowValues[(n/32)%8];const int32 interval=intervals[(n/256)%8];
            const uint32 starts[]{now,now-1u,now-49u,now-50u,now-51u,0,0xFFFFFFFFu,0x80000000u};
            const uint32 start=starts[(n/7)%8],minimum=(n/43)%3==0?now:(n/43)%3==1?now-1u:now+1u;
            const bool started=(n&1)!=0,stopped=(n&2)!=0;
            uint32 expectedStart=start;int32 expectedInterval=interval;bool expectedStarted=started,expectedStopped=stopped;uint32 expectedMinimum=minimum;
            json expectedTrace=json::array();
            if(!started){expectedStart=now;expectedInterval=50;expectedStarted=true;++initialStarts;}
            const bool blocked=now<minimum;
            bool isExpired=false;
            if(blocked)++minimumBlocked;
            else {
                if(expectedStopped){expectedStart=now;expectedStopped=false;++paused;}
                const uint32 end=expectedStart+uint32(expectedInterval);wrapped+=end<expectedStart;
                if(now>=end){isExpired=true;++expired;expectedStart=now;expectedInterval=50;expectedStarted=true;
                    for(int helper=0;helper<3;++helper){const bool ret=(n&(1<<(helper+2)))!=0;const char*name=helper==0?"gang":helper==1?"chat":"cars";expectedTrace.push_back({{"helper",name},{"self_matches",true},{"ped_matches",true},{"returned",ret}});if(ret)expectedMinimum=now+100000u;if(ret||helper==2)break;}
                    if(expectedTrace.size()==1)++gangOnly;else if(expectedTrace.size()==2)++chatLast;else ++carsLast;
                }else ++waiting;
            }
            json results[2];
            for(int side=0;side<2;++side){
                auto& bytes=side?right:left;bytes.fill(uint8(1+n%255));auto*self=::new(bytes.data())CTaskComplexWanderStandard{PEDMOVE_WALK,uint8(n),false};
                *reinterpret_cast<uintptr_t*>(bytes.data())=side?ourStandard:0x85A200;
                self->m_TaskTimer.m_nStartTime=start;self->m_TaskTimer.m_nInterval=interval;self->m_TaskTimer.m_bStarted=started;self->m_TaskTimer.m_bStopped=stopped;self->m_nMinNextScanTime=minimum;
                CTimer::m_snTimeInMilliseconds=now;fixture=n;expectedSelf=self;trace=json::array();
                scan->State(side!=0);
                const bool tableGate=(*reinterpret_cast<void***>(bytes.data()))[12]==(side?scan->GetHookOurAddress():scan->GetHookGTAAddress());
                const bool exactEntry=entryGate(side!=0);
                using Fn=void(__thiscall*)(CTaskComplexWanderStandard*,CPed*);
                if(n%3==0)reinterpret_cast<Fn>(0x672600)(self,expectedPed);else reinterpret_cast<Fn>((*reinterpret_cast<void***>(bytes.data()))[12])(self,expectedPed);
                const bool named=*reinterpret_cast<uintptr_t*>(bytes.data())==(side?ourStandard:0x85A200);
                const bool model=self->m_TaskTimer.m_nStartTime==expectedStart&&self->m_TaskTimer.m_nInterval==expectedInterval&&self->m_TaskTimer.m_bStarted==expectedStarted&&self->m_TaskTimer.m_bStopped==expectedStopped&&self->m_nMinNextScanTime==expectedMinimum&&trace==expectedTrace;
                results[side]={{"bytes",ReversingProbe::Bytes(bytes.data(),bytes.size())},{"trace",trace},{"named_vptr_matches",named},{"table_gate",tableGate},{"entry_gate",exactEntry},{"model_matches",model},{"clock_unchanged",CTimer::GetTimeInMS()==now}};
            }
            const bool payload=std::equal(left.begin()+4,left.end(),right.begin()+4);
            const bool equal=payload&&results[0]["named_vptr_matches"]==true&&results[1]["named_vptr_matches"]==true&&results[0]["table_gate"]==true&&results[1]["table_gate"]==true&&results[0]["entry_gate"]==true&&results[1]["entry_gate"]==true&&results[0]["model_matches"]==true&&results[1]["model_matches"]==true&&results[0]["clock_unchanged"]==true&&results[1]["clock_unchanged"]==true;
            passed&=equal;scanFailures+=!equal;
            write({{"event","comparison"},{"function","CTaskComplexWanderStandard::ScanForStuff"},{"pair",pair++},{"seed",n},{"route",n%3==0?"entry":"named_vtable_slot12"},{"now",now},{"interval",interval},{"expired",isExpired},{"minimum_blocked",blocked},{"equal",equal},{"accepted",equal},{"raw_equal",left==right},{"payload_equal",payload},{"expected_trace",expectedTrace},{"original",results[0]},{"candidate",results[1]},{"pointer_mapping","only named Standard vptr; no field or padding filtering"}});
        }
    }
    bool helpersRestored=true;for(size_t i=0;i<6;++i)helpersRestored&=helperBefore[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(helpers[i]),5);
    std::memcpy(reinterpret_cast<void*>(0xB7CB7C),clockBefore.data(),clockBefore.size());
    for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
    bool entriesRestored=true,tablesRestored=true;
    for(const auto&r:entries)entriesRestored&=r.bytes==ReversingProbe::Bytes(r.address,r.bytes.size());
    for(const auto&r:tables)tablesRestored&=r.bytes==ReversingProbe::Bytes(r.address,r.bytes.size());
    const bool pedUnchanged=pedStorage==pedBefore;
    const bool globalsRestored=clockBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB7C),12)&&radiusBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x86FCB0),4);
    const bool restored=SmallPendingSupport::HooksUnchanged(flags)&&entriesRestored&&tablesRestored&&helpersRestored&&globalsRestored&&pedUnchanged;
    const bool coverage=initialStarts>0&&paused>0&&minimumBlocked>0&&expired>0&&waiting>0&&gangOnly>0&&chatLast>0&&carsLast>0&&wrapped>0;
    write({{"event","function_summary"},{"function","CTaskComplexWanderStandard::Constructor"},{"cases",2048},{"failures",constructorFailures}});
    write({{"event","function_summary"},{"function","CTaskComplexWanderStandard::ScanForStuff"},{"cases",4096},{"failures",scanFailures},{"initial_starts",initialStarts},{"paused",paused},{"minimum_blocked",minimumBlocked},{"expired",expired},{"waiting",waiting},{"gang_only",gangOnly},{"chat_last",chatLast},{"cars_last",carsLast},{"wraparound",wrapped},{"coverage",coverage}});
    write({{"event",passed&&restored&&coverage?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"entries_restored",entriesRestored},{"tables_restored",tablesRestored},{"helpers_restored",helpersRestored},{"globals_restored",globalsRestored},{"ped_fixture_unchanged",pedUnchanged},{"coverage",coverage}});
}
}
