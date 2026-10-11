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

namespace DependenciesVehicleStats10Live {
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
    {"CStats","UpdateFatAndMuscleStats",0x55C470,0,-1,2048},
    {"CStats","UpdateStatsWhenWeaponHit",0x55CEB0,0,-1,2048},
    {"CStats","UpdateStatsWhenSwimming",0x55C990,0,-1,2048}
};
struct Result {uint32 eax,ecx,edx,arg0,arg1;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry,bool twoArgs,uint32 arg0,uint32 arg1) {
    uint32 a,c,d,x=arg0,y=arg1,before,after,b0,s0,i0,p0,b1,s1,i1,p1;
    __asm {
        mov before,esp
        mov b0,ebx
        mov s0,esi
        mov i0,edi
        mov p0,ebp
        cmp twoArgs,0
        je oneArg
        push arg1
    oneArg:
        push arg0
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
        mov eax,[esp]
        mov x,eax
        add esp,4
        cmp twoArgs,0
        je clean
        mov eax,[esp]
        mov y,eax
        add esp,4
    clean:
        mov after,esp
    }
    return {a,c,d,x,y,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};
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
inline uint32 ReadInt(uintptr_t at){uint32 v{};memcpy(&v,reinterpret_cast<void*>(at),4);return v;}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesvehiclestats10-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesvehiclestats10-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesvehiclestats10");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    const std::array<uintptr_t,3> symbols{uintptr_t(&CStats::UpdateFatAndMuscleStats),uintptr_t(&CStats::UpdateStatsWhenWeaponHit),uintptr_t(&CStats::UpdateStatsWhenSwimming)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;
    for(unsigned i=0;i<3;++i)for(auto at:{fns[i].address,symbols[i],Resolve(symbols[i])})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute r0("CStats",fns[0].name,fns[0].address,&CStats::UpdateFatAndMuscleStats);
    OriginalDependencies499::HookRoute r1("CStats",fns[1].name,fns[1].address,&CStats::UpdateStatsWhenWeaponHit);
    OriginalDependencies499::HookRoute r2("CStats",fns[2].name,fns[2].address,&CStats::UpdateStatsWhenSwimming);
    OriginalDependencies499::HookRoute* routes[]{&r0,&r1,&r2};
    std::array<std::vector<uint8>,3> offCode;
    for(unsigned i=0;i<3;++i)offCode[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    auto* info=reinterpret_cast<uint8*>(&CWorld::Players[0]);auto* pad=CPad::GetPad(0);
    std::vector<Block> blocks;
    blocks.emplace_back(0xB78200,0x1338);
    blocks.emplace_back(0xBAA460,0x4D0);
    blocks.emplace_back(0xB71510,0x2C0);
    blocks.emplace_back(reinterpret_cast<uintptr_t>(info),sizeof(CPlayerInfo));
    blocks.emplace_back(uintptr_t(&CWorld::Players[1]),sizeof(CPlayerInfo));
    blocks.emplace_back(reinterpret_cast<uintptr_t>(&pad->JustOutOfFrontEnd),sizeof(pad->JustOutOfFrontEnd));
    for(uintptr_t a:{uintptr_t(0x8CDE56),uintptr_t(0xB6F065),uintptr_t(0xB5F852),uintptr_t(0xBA82E3),uintptr_t(0xB7CD74),uintptr_t(0x96918C),uintptr_t(0x96C014),uintptr_t(0xBAACC0+4*128),uintptr_t(0xA43088)})blocks.emplace_back(a,1);
    blocks.emplace_back(0xC1AEB8,32);
    blocks.emplace_back(0xB7CB5C,4); // CTimer timestep, read by both complete bodies.
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
    auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
    std::array<HelperRoute,11> helpers{HelperRoute(0x53FB70),HelperRoute(0x588B50),HelperRoute(0x558E40),HelperRoute(0x6A0050),HelperRoute(0x588BE0),HelperRoute(0x559760),HelperRoute(0x821B40),HelperRoute(0x55C180),HelperRoute(0x55B980),HelperRoute(0x743CD0),HelperRoute(0x441390)};
    auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
    bool ready=pad&&helpersOK();for(auto* route:routes)ready&=route->Registered()&&ModuleOf(route->Body())==ModuleOf(uintptr_t(&ModuleMarker));
    // The two direct game collaborators must execute reconstructed DLL bodies on both sides.
    for(const auto& h:helpers){const auto m=h.Metadata();if(m["entry"]==0x55C180||m["entry"]==0x55B980||m["entry"]==0x743CD0||m["entry"]==0x441390)ready&=m["registered"]==true&&m["reconstructed"]==true&&m["enabled"]==true;}
    json metadata=json::array();for(const auto& h:helpers)metadata.push_back(h.Metadata());
    Write({{"event","begin"},{"registered_hook",ready},{"helper_routes",metadata},{"helper_routes_ok",helpersOK()},{"floating_state_restored",FloatingStateRestored()},{"functions",3},{"pairs",6144},{"collaborators_stubbed",false}});
    const uint32 counters[]{0,1,999,1000,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF};
    const uint32 limits[]{0xBF800000,0,0x3F800000,0x3F7FFFFF,0x3F800001,0x49800000,0x4A800000,0x7F800000,0x7FC01234};
    const uint32 scalarBits[]{0xBF800000,0,0x3F800000,0x41C80000,0x447A0000,0x7FC01234};
    const uint32 skillBits[]{0,0x3F800000,0x4479FFFF,0x447A0000,0x447A0001,0xBF800000,0x7FC01234,0x7F800000};
    const float steps[]{0.f,0.01f,0.5f,1.f,1.999f,3.f,8.f,-1.f};
    const auto* loadedPed=FindPlayerPed();
    bool passed=ready,nestedRestored=true;int pair=0;
    for(int fn=0;fn<3&&ready;++fn){int failures=0;
        std::unique_ptr<HelperHookRoute> nested;
        if(fn==2){nested=std::make_unique<HelperHookRoute>("CStats","UpdateFatAndMuscleStats",0x55C470,reinterpret_cast<void*>(&CStats::UpdateFatAndMuscleStats));ready&=nested->Check();Write({{"event","nested_helper_begin"},{"function","CStats::UpdateStatsWhenSwimming"},{"route",nested->Metadata("CStats","UpdateFatAndMuscleStats")}});}
        if(!ready){passed=false;break;}
        for(int scenario=0;scenario<2048;++scenario){const unsigned profile=scenario%16,value=scenario/16;
            auto invoke=[&](bool candidate){
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                memset(reinterpret_cast<void*>(0xBAA460),0,0x4D0);
                memset(CStats::StatTypesInt.data(),0,sizeof(CStats::StatTypesInt));
                memset(CStats::StatTypesFloat.data(),0,sizeof(CStats::StatTypesFloat));
                CStats::TotalNumStatMessages=0;CStats::bStatUpdateMessageDisplayed=false;CStats::bShowUpdateStats=true;
                *reinterpret_cast<uint8*>(0xB6F065)=0;*reinterpret_cast<uint8*>(0xB5F852)=0;*reinterpret_cast<uint8*>(0xBA82E3)=0;
                *reinterpret_cast<uint8*>(0xB7CD74)=0;*reinterpret_cast<uint8*>(0x96918C)=0;*reinterpret_cast<uint8*>(0x96C014)=0;
                *reinterpret_cast<uint8*>(0xBAACC0+4*128)=0;*reinterpret_cast<uint8*>(0xA43088)=0;info[0x117]=0;pad->JustOutOfFrontEnd=0;
                CWorld::Players[0].m_pPed=const_cast<CPlayerPed*>(loadedPed);CWorld::Players[1].m_pPed=(profile&1)?const_cast<CPlayerPed*>(loadedPed):nullptr;
                const uint32 fatCounter=counters[value%8],maxHealthCounter=counters[(value/8)%8];
                const uint32 fatLimit=limits[(value/4)%9],maxHealthLimit=limits[(value/7)%9];
                const uint32 fatBits=scalarBits[(value/11+profile/4)%6],muscleBits=scalarBits[(value/13)%6];
                const uint32 fatLoss=scalarBits[(value/3)%6],muscleLoss=scalarBits[(value/5)%6];
                const float muscleIncrement=float(value%3)*25.f,maxHealthIncrement=float((value/3)%3)*25.f,step=steps[(value/3)%8];
                const uint32 maxHealthBits=skillBits[(profile/2+value/13)%8];
                PutInt(0xB794FC,fatCounter);PutInt(0xB7950C,maxHealthCounter);PutInt(0xB78FB8,fatLimit);PutInt(0xB78FC4,maxHealthLimit);
                PutInt(0xB793D4,fatBits);PutInt(0xB793DC,muscleBits);PutInt(0xB78F6C,fatLoss);PutInt(0xB78F70,muscleLoss);PutFloat(0xB78F34,muscleIncrement);PutFloat(0xB78F3C,maxHealthIncrement);PutInt(0xB793E0,maxHealthBits);
                *reinterpret_cast<uint8*>(0xB79534)=uint8(profile%4);PutFloat(0xB7CB5C,step);
                uint32 arg0=counters[(value/3)%8],arg1=0;
                json input={{"profile",profile},{"value",value},{"fat_counter",fatCounter},{"max_health_counter",maxHealthCounter},{"fat_limit_bits",fatLimit},{"max_health_limit_bits",maxHealthLimit},{"fat_bits",fatBits},{"muscle_bits",muscleBits},{"fat_loss_bits",fatLoss},{"muscle_loss_bits",muscleLoss},{"muscle_increment_bits",std::bit_cast<uint32>(muscleIncrement)},{"max_health_increment_bits",std::bit_cast<uint32>(maxHealthIncrement)},{"max_health_bits",maxHealthBits},{"flag",profile%4},{"step_bits",std::bit_cast<uint32>(step)}};
                if(fn==1){
                    arg0=22+value%11;const uint32 slot=arg0==32?6:arg0-22,stat=69+slot;
                    const uint32 skill=skillBits[(value/11)%8],count=counters[(value/7)%8],limit=limits[(value/9)%9];
                    const uint32 last=(profile&2)?slot:(slot+1)%10;const float increment=float(value%3)*25.f;
                    PutInt(0xB79380+stat*4,skill);PutInt(0xB78F40+slot*4,std::bit_cast<uint32>(increment));PutInt(0xB78FC8+slot*4,limit);PutInt(0xB79514,last);PutInt(0xB79504,count);
                    input["weapon"]=arg0;input["stat"]=stat;input["slot"]=slot;input["skill_bits"]=skill;input["weapon_counter"]=count;input["weapon_limit_bits"]=limit;input["last_slot"]=last;input["weapon_increment_bits"]=std::bit_cast<uint32>(increment);input["coop"]=bool(profile&1);
                }
                if(fn==2){
                    arg0=(0xA55A5A00u^(value<<8))|(profile&1);arg1=(0x5AA5A500u^(value<<8))|((profile>>1)&1);
                    const uint32 stamina=counters[(value/8)%8],lung=counters[(value/16)%8];
                    const uint32 staminaLimit=limits[(value/4)%9],lungLimit=limits[(value/7)%9];
                    const uint32 rateNormal=scalarBits[(value/3)%6],rateFast=scalarBits[(value/5)%6];
                    const float increment=float(value%3)*25.f;
                    PutInt(0xB794E4,stamina);PutInt(0xB794E8,lung);PutInt(0xB78F98,staminaLimit);PutInt(0xB78FC0,lungLimit);PutInt(0xB78F80,rateNormal);PutInt(0xB78F84,rateFast);PutFloat(0xB78F14,increment);PutFloat(0xB78F38,increment);
                    input["stamina_counter"]=stamina;input["lung_counter"]=lung;input["stamina_limit_bits"]=staminaLimit;input["lung_limit_bits"]=lungLimit;input["normal_rate_bits"]=rateNormal;input["fast_rate_bits"]=rateFast;input["swim_increment_bits"]=std::bit_cast<uint32>(increment);
                }
                input["arg0"]=arg0;input["arg1"]=arg1;
                bool routeOk=false,helperOk=helpersOK(),nestedOK=fn!=2||nested->Check();
                const auto result=routes[fn]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;return Gate(fns[fn].address,fn==2,arg0,arg1);});helperOk&=helpersOK();nestedOK&=fn!=2||nested->Check();
                json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
                json out={{"input",input},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"argument0_after",result.arg0},{"argument1_after",result.arg1},{"fat_counter_after",ReadInt(0xB794FC)},{"max_health_counter_after",ReadInt(0xB7950C)},{"fat_bits_after",ReadInt(0xB793D4)},{"muscle_bits_after",ReadInt(0xB793DC)},{"flag_after",*reinterpret_cast<uint8*>(0xB79534)},{"weapon_counter_after",ReadInt(0xB79504)},{"last_slot_after",ReadInt(0xB79514)},{"stamina_counter_after",ReadInt(0xB794E4)},{"lung_counter_after",ReadInt(0xB794E8)},{"state",state},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOk},{"helper_route_ok",helperOk},{"nested_helper_route_ok",nestedOK},{"arguments_unchanged",result.arg0==arg0&&result.arg1==arg1}};
                if(fn==2)out["nested_helper"]=nested->Metadata("CStats","UpdateFatAndMuscleStats");
                restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
            };
            const auto original=invoke(false),candidate=invoke(true);bool ok=original==candidate;
            for(const char* gate:{"stack_restored","nonvolatile_preserved","route_ok","helper_route_ok","nested_helper_route_ok","arguments_unchanged","globals_restored","floating_state_restored"})ok&=original[gate]==true&&candidate[gate]==true;
            passed&=ok;failures+=!ok;
            Write({{"event","comparison"},{"function",std::string("CStats::")+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});
        }
        if(nested){const bool ok=nested->Restore();nestedRestored&=ok;}
        Write({{"nested_helper_restored",nestedRestored},{"event","function_summary"},{"function",std::string("CStats::")+fns[fn].name},{"cases",2048},{"failures",failures}});
    }
    restore();bool stateRestored=restored()&&helpersOK()&&nestedRestored;for(unsigned i=0;i<3;++i)stateRestored&=offCode[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    for(auto* route:routes){const bool ok=route->Restore();stateRestored&=ok;}
    for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pair},{"nested_helper_restored",nestedRestored},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"helper_route_ok",helpersOK()}});
    RestoreFloatingState();savedFPActive=false;
}
}
