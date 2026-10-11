#pragma once
#include <fstream>
#include "Stats.h"
#include "Hud.h"
#include "World.h"
#include "Pad.h"
#include "PlayerInfo.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperRoutes.hpp"

namespace DependenciesStatsUi8Live {
inline bool done{};
inline std::ofstream log;
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
struct Fn { const char* cls;const char* name;uintptr_t address;int reserved,unused,count; };
inline constexpr Fn fns[]{
    {"CStats","FindCriminalRatingString",0x55A210,0,-1,2048},
    {"CStats","UpdateStatsOnRespawn",0x55CFC0,0,-1,2048},
    {"CStats","UpdateStatsAddToHealth",0x55D030,0,-1,2048},
    {"CStats","CheckForStatsMessage",0x559760,0,-1,2048}
};
struct Result { uint32 eax,ecx,edx,arg;bool stack,nonvolatile; };
inline Result Gate(uintptr_t entry,bool withArg,uint32 argument) {
    uint32 a,c,d,argAfter=argument,before,after,b0,s0,i0,p0,b1,s1,i1,p1;
    __asm {
        mov before,esp
        mov b0,ebx
        mov s0,esi
        mov i0,edi
        mov p0,ebp
        cmp withArg,0
        je noArg
        push argument
    noArg:
        mov eax,0A5123456h
        mov ecx,137AFF01h
        mov edx,0C0DE5921h
        call entry
        mov a,eax
        mov c,ecx
        mov d,edx
        mov b1,ebx
        mov s1,esi
        mov i1,edi
        mov p1,ebp
        cmp withArg,0
        je clean
        mov eax,[esp]
        mov argAfter,eax
        add esp,4
    clean:
        mov after,esp
    }
    return {a,c,d,argAfter,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};
}
inline std::string Hex(const std::vector<uint8>& bytes) {
    constexpr char digits[]="0123456789abcdef";std::string out;out.reserve(bytes.size()*2);
    for(auto c:bytes){out+=digits[c>>4];out+=digits[c&15];}return out;
}
struct Block {
    uintptr_t address;std::vector<uint8> saved;
    Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}
    void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}
    bool Restored()const{return memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size())==0;}
    std::string Observe()const{std::vector<uint8> v(saved.size());memcpy(v.data(),reinterpret_cast<void*>(address),v.size());return Hex(v);}
};
inline void PutFloat(uintptr_t address,float value){memcpy(reinterpret_cast<void*>(address),&value,4);}
inline void PutInt(uintptr_t address,uint32 value){memcpy(reinterpret_cast<void*>(address),&value,4);}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesstatsui8-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesstatsui8-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesstatsui8");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    const std::array<uintptr_t,4> rawSymbols{uintptr_t(&CStats::FindCriminalRatingString),uintptr_t(&CStats::UpdateStatsOnRespawn),uintptr_t(&CStats::UpdateStatsAddToHealth),uintptr_t(&CStats::CheckForStatsMessage)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialTargetCode;
    for(unsigned i=0;i<4;++i)for(auto at:{fns[i].address,rawSymbols[i],Resolve(rawSymbols[i])})initialTargetCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute r0("CStats",fns[0].name,fns[0].address,&CStats::FindCriminalRatingString);
    OriginalDependencies499::HookRoute r1("CStats",fns[1].name,fns[1].address,&CStats::UpdateStatsOnRespawn);
    OriginalDependencies499::HookRoute r2("CStats",fns[2].name,fns[2].address,&CStats::UpdateStatsAddToHealth);
    OriginalDependencies499::HookRoute r3("CStats",fns[3].name,fns[3].address,&CStats::CheckForStatsMessage);
    OriginalDependencies499::HookRoute* routes[]{&r0,&r1,&r2,&r3};
    std::array<std::vector<uint8>,4> code;
    for(int i=0;i<4;++i)code[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);
    auto* info=reinterpret_cast<uint8*>(&CWorld::Players[0]);auto* pad=CPad::GetPad(0);
    std::vector<Block> blocks;
    blocks.emplace_back(0xB78200,0x1318); // Complete stats arrays, reactions, messages and counters.
    blocks.emplace_back(0xBAA460,0x4D0); // Complete help state and all three 400-byte buffers.
    blocks.emplace_back(0xB71510,0x2C0); // gString2 and gString, including padding.
    blocks.emplace_back(reinterpret_cast<uintptr_t>(info),sizeof(CPlayerInfo));
    blocks.emplace_back(reinterpret_cast<uintptr_t>(&pad->JustOutOfFrontEnd),sizeof(pad->JustOutOfFrontEnd));
    for(uintptr_t a:{uintptr_t(0x8CDE56),uintptr_t(0xB6F065),uintptr_t(0xB5F852),uintptr_t(0xBA82E3),uintptr_t(0xB7CD74),uintptr_t(0x96918C),uintptr_t(0x96C014),uintptr_t(0xBAACC0+4*128)})blocks.emplace_back(a,1);
    blocks.emplace_back(0xA43088,1);
    blocks.emplace_back(0xC1AEB8,32); // Real text fallback buffer; restored even if readiness fails.
    const auto control=_controlfp(0,0);SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
    auto restored=[&]{for(const auto& b:blocks)if(!b.Restored())return false;return _controlfp(0,0)==control&&FloatingStateRestored();};
    // Use a real loaded GXT key, taken from an original configured stat-message record.
    char key[8]{};
    for(int i=0;i<128;++i){auto* k=reinterpret_cast<char*>(0xB78208+i*16);if(k[0]&&memchr(k,0,8)){memcpy(key,k,8);break;}}
    auto* keyText=key[0]?reinterpret_cast<GxtChar*(__thiscall*)(void*,const char*)>(0x6A0050)(reinterpret_cast<void*>(0xC1B340),key):nullptr;
    bool ready=pad&&key[0]&&keyText&&keyText!=reinterpret_cast<GxtChar*>(0xC1AEB8)&&keyText[0];
    restore();
    for(auto* route:routes)ready&=route->Registered()&&ModuleOf(route->Body())==ModuleOf(uintptr_t(&ModuleMarker));
    std::array<HelperRoute,9> helperProofs{HelperRoute(0x53FB70),HelperRoute(0x588B50),HelperRoute(0x558E40),HelperRoute(0x6A0050),HelperRoute(0x588BE0),HelperRoute(0x559080),HelperRoute(0x821B40),HelperRoute(0x55C180),HelperRoute(0x55B980)};
    auto helpersOK=[&] {return std::ranges::all_of(helperProofs,[](const auto& h){return h.Check();});};
    ready&=helpersOK();json helperRoutes=json::array();for(const auto& helper:helperProofs)helperRoutes.push_back(helper.Metadata());
    Write({{"event","begin"},{"helper_routes_ok",helpersOK()},{"floating_state_restored",FloatingStateRestored()},{"registered_hook",ready},{"real_stat_message_key",std::string(key)},{"helper_routes",helperRoutes}});
    bool passed=ready;int pair=0;
    const int bands[]{-10000,-6001,-6000,-5999,-4001,-4000,-3999,-2001,-2000,-1999,-501,-500,-499,-1,0,20,50,75,100,120,150,200,240,270,300,335,370,400,450,500,550,600,610,650,700,850,1000,1005,1150,1300,1500,1700,2000,2100,2300,2500,2750,3000,3500,4000,5000,7500,10000,20000,30000,40000,50000,65000,80000,100000,150000,200000,300000,375000,500000,1000000};
    for(int fn=0;fn<4&&ready;++fn){int failures=0;
        for(int scenario=0;scenario<fns[fn].count;++scenario){
            auto invoke=[&](bool candidate){
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                memset(reinterpret_cast<void*>(0xBAA460),0,0x4D0);
                memset(CStats::StatTypesInt.data(),0,sizeof(CStats::StatTypesInt));
                memset(CStats::StatTypesFloat.data(),0,sizeof(CStats::StatTypesFloat));
                CStats::TotalNumStatMessages=0;CStats::bStatUpdateMessageDisplayed=false;CStats::bShowUpdateStats=true;
                *reinterpret_cast<uint8*>(0xB6F065)=0;*reinterpret_cast<uint8*>(0xB5F852)=0;*reinterpret_cast<uint8*>(0xBA82E3)=0;
                *reinterpret_cast<uint8*>(0xB7CD74)=0;*reinterpret_cast<uint8*>(0x96918C)=0;*reinterpret_cast<uint8*>(0x96C014)=0;
                *reinterpret_cast<uint8*>(0xBAACC0+4*128)=0;*reinterpret_cast<uint8*>(0xA43088)=0;info[0x117]=0;pad->JustOutOfFrontEnd=0;
                CWorld::Players[0].m_nMoney=0;CWorld::Players[0].m_nDisplayMoney=10000000+((scenario/(std::size(bands)*3))%3)-1;
                uint32 argument=uint32(scenario)*2654435761u;
                int expectedRating=0,observedRating=0;
                if(fn==0){
                    int rating=bands[scenario%std::size(bands)];if(rating>0)rating+=(scenario/std::size(bands))%3-1;
                    int cheated=rating<0?(-rating+9)/10:0;
                    CStats::StatTypesInt[177-120]=rating+cheated*10;CStats::StatTypesInt[137-120]=cheated;
                    *reinterpret_cast<uint8*>(0x96918C)=cheated!=0;
                    CStats::StatTypesInt[13]=(scenario/(std::size(bands)*3))%2?1001:0; // B79034: special 1150..1299 title.
                    const int flightTimes[]{0,36000000,39600000};CStats::StatTypesInt[49]=flightTimes[(scenario/7)%3]; // B790C4: special 375000..499999 title.
                    CStats::StatTypesInt[57]+=CStats::StatTypesInt[13]*3; // Compensate the real rating helper's busted penalty.
                    expectedRating=rating;observedRating=reinterpret_cast<int32(__cdecl*)()>(0x559080)();
                }else if(fn==1||fn==2){
                    const uint32 counters[]{0,1,2,3,9,10,11,0x7FFFFFFF,0x80000000,0xFFFFFFFF};
                    const float thresholds[]{-1.f,0.f,1.f,2.f,10.f,2147483648.f,4294967296.f,std::bit_cast<float>(0x7FC01234u),std::bit_cast<float>(0x7F800000u)};
                    const float health[]{0.f,500.f,999.f,1000.f,1001.f,std::bit_cast<float>(0x7FC01234u)};
                    PutInt(fn==1?0xB79508:0xB79510,counters[scenario%10]);
                    PutFloat(fn==1?0xB78FF4:0xB78FF8,thresholds[(scenario/10)%9]);
                    PutFloat(fn==1?0xB78F74:0xB78F3C,float((scenario/90)%3)*25.f);
                    CStats::StatTypesFloat[24]=health[(scenario/270)%6];
                    argument=counters[(scenario/5)%10];
                }else{
                    const int counts[]{0,1,2,5,127,128,129};CStats::TotalNumStatMessages=counts[(scenario/8)%7];
                    for(int i=0;i<128;++i){auto* m=reinterpret_cast<uint8*>(0xB78200+i*16);uint16 stat=uint16(i%2?177:24);memcpy(m,&stat,2);m[2]=uint8((scenario+i)%3==0);m[3]=uint8((scenario+i)%2);float threshold=float((scenario+i)%3)*50.f;if((scenario+i)%17==0)threshold=std::bit_cast<float>(0x7FC01234u);memcpy(m+4,&threshold,4);memcpy(m+8,key,8);}
                    CStats::StatTypesFloat[24]=50.f;CStats::StatTypesInt[57]=50;
                    switch(scenario%8){case 1:info[0x117]=1;break;case 2:CStats::bShowUpdateStats=false;break;case 3:*reinterpret_cast<uint8*>(0xB6F065)=1;break;case 4:*reinterpret_cast<uint8*>(0xB5F852)=1;break;case 5:CHud::m_nHelpMessageState=1;break;case 6:*reinterpret_cast<uint8*>(0xBA82E3)=1;break;default:break;}
                }
                bool routeOk{},helperOk=helpersOK()&&(fn==3||!ReversingProbe::JumpsToDll(0x559760));
                const auto result=routes[fn]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;return Gate(fns[fn].address,fn==2,argument);});helperOk&=helpersOK();
                json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
                bool returnOk=fn!=0||(result.eax!=0&&result.eax!=0xC1AEB8&&*reinterpret_cast<const char*>(result.eax));
                json value={{"helper_route_ok",helperOk},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"argument_after",result.arg},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOk},{"return_nonnull",returnOk},{"fixture_contract",fn!=0||observedRating==expectedRating},{"rating",observedRating},{"state",state}};
                if(fn==0) {value["display_money"]=CWorld::Players[0].m_nDisplayMoney;value["flight_ms"]=CStats::StatTypesInt[49];value["arrests"]=CStats::StatTypesInt[13];}
                if(fn==0&&returnOk)value["text"]=std::string(reinterpret_cast<const char*>(result.eax));
                restore();value["globals_restored"]=restored();value["floating_state_restored"]=FloatingStateRestored();return value;
            };
            const auto original=invoke(false),candidate=invoke(true);
            bool ok=original==candidate&&original["stack_restored"]==true&&original["nonvolatile_preserved"]==true&&original["route_ok"]==true&&original["return_nonnull"]==true&&original["fixture_contract"]==true&&original["globals_restored"]==true&&original["helper_route_ok"]==true&&original["floating_state_restored"]==true;
            passed&=ok;failures+=!ok;
            Write({{"event","comparison"},{"function",std::string("CStats::")+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});
        }
        Write({{"event","function_summary"},{"function",std::string("CStats::")+fns[fn].name},{"cases",fns[fn].count},{"failures",failures}});
    }
    restore();bool stateRestored=restored()&&helpersOK();for(int i=0;i<4;++i)stateRestored&=code[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),5);
    for(auto* route:routes){const bool r=route->Restore();stateRestored&=r;}
    for(auto& [address,bytes]:initialTargetCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(address),16);
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pair},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"helper_route_ok",helpersOK()}});
    RestoreFloatingState();savedFPActive=false;
}
}
