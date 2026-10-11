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

namespace DependenciesVehicleStats9Live {
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
    {"CStats","UpdateStatsWhenDriving",0x55CAC0,0,-1,2048},
    {"CStats","UpdateStatsWhenFlying",0x55CC00,0,-1,2048}
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
alignas(16) inline std::array<uint8,0xA20> vehicleStorage{};
alignas(16) inline std::array<uint8,0x70> matrixStorage{};
inline uint32 ReadInt(uintptr_t at){uint32 v{};memcpy(&v,reinterpret_cast<void*>(at),4);return v;}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesvehiclestats9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesvehiclestats9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesvehiclestats9");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    const std::array<uintptr_t,2> symbols{uintptr_t(&CStats::UpdateStatsWhenDriving),uintptr_t(&CStats::UpdateStatsWhenFlying)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;
    for(unsigned i=0;i<2;++i)for(auto at:{fns[i].address,symbols[i],Resolve(symbols[i])})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute r0("CStats",fns[0].name,fns[0].address,&CStats::UpdateStatsWhenDriving);
    OriginalDependencies499::HookRoute r1("CStats",fns[1].name,fns[1].address,&CStats::UpdateStatsWhenFlying);
    OriginalDependencies499::HookRoute* routes[]{&r0,&r1};
    std::array<std::vector<uint8>,2> offCode;
    for(unsigned i=0;i<2;++i)offCode[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    auto* info=reinterpret_cast<uint8*>(&CWorld::Players[0]);auto* pad=CPad::GetPad(0);
    std::vector<Block> blocks;
    blocks.emplace_back(0xB78200,0x1318);
    blocks.emplace_back(0xBAA460,0x4D0);
    blocks.emplace_back(0xB71510,0x2C0);
    blocks.emplace_back(reinterpret_cast<uintptr_t>(info),sizeof(CPlayerInfo));
    blocks.emplace_back(reinterpret_cast<uintptr_t>(&pad->JustOutOfFrontEnd),sizeof(pad->JustOutOfFrontEnd));
    for(uintptr_t a:{uintptr_t(0x8CDE56),uintptr_t(0xB6F065),uintptr_t(0xB5F852),uintptr_t(0xBA82E3),uintptr_t(0xB7CD74),uintptr_t(0x96918C),uintptr_t(0x96C014),uintptr_t(0xBAACC0+4*128),uintptr_t(0xA43088)})blocks.emplace_back(a,1);
    blocks.emplace_back(0xC1AEB8,32);
    blocks.emplace_back(0xB7CB5C,4); // CTimer timestep, read by both complete bodies.
    blocks.emplace_back(uintptr_t(vehicleStorage.data()),vehicleStorage.size());
    blocks.emplace_back(uintptr_t(matrixStorage.data()),matrixStorage.size());
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
    auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
    std::array<HelperRoute,9> helpers{HelperRoute(0x53FB70),HelperRoute(0x588B50),HelperRoute(0x558E40),HelperRoute(0x6A0050),HelperRoute(0x588BE0),HelperRoute(0x559760),HelperRoute(0x821B40),HelperRoute(0x55C180),HelperRoute(0x55B980)};
    auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
    bool ready=pad&&helpersOK();for(auto* route:routes)ready&=route->Registered()&&ModuleOf(route->Body())==ModuleOf(uintptr_t(&ModuleMarker));
    // The two direct game collaborators must execute reconstructed DLL bodies on both sides.
    for(const auto& h:helpers){const auto m=h.Metadata();if(m["entry"]==0x55C180||m["entry"]==0x55B980)ready&=m["registered"]==true&&m["reconstructed"]==true&&m["enabled"]==true;}
    json metadata=json::array();for(const auto& h:helpers)metadata.push_back(h.Metadata());
    Write({{"event","begin"},{"registered_hook",ready},{"helper_routes",metadata},{"helper_routes_ok",helpersOK()},{"floating_state_restored",FloatingStateRestored()},{"functions",2},{"pairs",4096},{"collaborators_stubbed",false}});
    const uint32 counterValues[]{0,1,999,1000,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF};
    const uint32 speedBits[]{0,0x3DCCCCCD,0x3E4CCCCC,0x3E4CCCCD,0x3E4CCCCE,0x3EFFFFFF,0x3F000000,0x3F000001,0x3F4CCCCC,0x3F4CCCCD,0x3F4CCCCE,0x3FA66665,0x3FA66666,0x3FA66667,0xBF800000,0x7FC01234};
    const uint32 limitBits[]{0xBF800000,0,0x3F800000,0x3F7FFFFF,0x3F800001,0x49800000,0x4A800000,0x7F800000,0x7FC01234};
    const float steps[]{0.f,0.01f,0.5f,1.f,1.999f,3.f,8.f,-1.f};
    const uint32 upBits[]{0xBF800000,0,0x3F800000,0x7FC01234};
    bool passed=ready;int pair=0;
    for(int fn=0;fn<2&&ready;++fn){int failures=0;
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
                vehicleStorage.fill(value%2?0xA5:0x5A);matrixStorage.fill(value%2?0x5A:0xA5);
                const auto vehicle=uintptr_t(vehicleStorage.data()+16),matrix=uintptr_t(matrixStorage.data()+16);
                PutInt(vehicle+0x14,uint32(matrix));PutInt(vehicle+0x590,profile/8?5:0);
                *reinterpret_cast<uint8*>(vehicle+0x960)=uint8((profile/4)%2?1+value%4:0);
                PutInt(vehicle+0x44,speedBits[value%16]);PutFloat(vehicle+0x48,0.f);PutFloat(vehicle+0x4C,0.f);
                // Matrix basis is a real invertible rotation for finite up.z -1/0/1; NaN is an explicit numerical edge.
                for(unsigned off=0;off<0x40;off+=4)PutFloat(matrix+off,0.f);
                PutFloat(matrix,1.f);PutFloat(matrix+0x14,profile%4==1?0.f:float(profile%4==0?-1:1));
                PutFloat(matrix+0x18,profile%4==1?1.f:0.f);PutFloat(matrix+0x24,profile%4==1?-1.f:0.f);PutInt(matrix+0x28,upBits[profile%4]);
                const auto counter=counterValues[(value/16)%8];const auto limit=limitBits[(value/8)%9];const auto step=steps[(value/3)%8];const float inc=float(value%3)*25.f;
                PutInt(fn==0?0xB794EC:0xB794F0,counter);PutInt(fn==0?0xB78FA4:0xB78FA8,limit);PutFloat(fn==0?0xB78F20:0xB78F24,inc);PutFloat(0xB7CB5C,step);
                const auto beforeVehicle=ReversingProbe::Bytes(vehicleStorage.data(),vehicleStorage.size()),beforeMatrix=ReversingProbe::Bytes(matrixStorage.data(),matrixStorage.size());
                bool routeOk=false,helperOk=helpersOK();const auto result=routes[fn]->Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;return Gate(fns[fn].address,true,uint32(vehicle));});helperOk&=helpersOK();
                json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
                const uint32 finalCounter=ReadInt(fn==0?0xB794EC:0xB794F0);
                json input={{"profile",profile},{"value",value},{"subtype",profile/8?5:0},{"contact_wheels",(profile/4)%2?1+value%4:0},{"up_bits",upBits[profile%4]},{"speed_bits",speedBits[value%16]},{"counter",counter},{"limit_bits",limit},{"step_bits",std::bit_cast<uint32>(step)},{"increment_bits",std::bit_cast<uint32>(inc)}};
                json out={{"input",input},{"eax",result.eax},{"ecx",result.ecx},{"edx",result.edx},{"argument_after",result.arg},{"counter_after",finalCounter},{"state",state},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOk},{"helper_route_ok",helperOk},{"fixture_unchanged",beforeVehicle==ReversingProbe::Bytes(vehicleStorage.data(),vehicleStorage.size())&&beforeMatrix==ReversingProbe::Bytes(matrixStorage.data(),matrixStorage.size())}};
                restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
            };
            const auto original=invoke(false),candidate=invoke(true);bool ok=original==candidate;
            for(const char* gate:{"stack_restored","nonvolatile_preserved","route_ok","helper_route_ok","fixture_unchanged","globals_restored","floating_state_restored"})ok&=original[gate]==true&&candidate[gate]==true;
            passed&=ok;failures+=!ok;
            Write({{"event","comparison"},{"function",std::string("CStats::")+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});
        }
        Write({{"event","function_summary"},{"function",std::string("CStats::")+fns[fn].name},{"cases",2048},{"failures",failures}});
    }
    restore();bool stateRestored=restored()&&helpersOK();for(unsigned i=0;i<2;++i)stateRestored&=offCode[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    for(auto* route:routes){const bool ok=route->Restore();stateRestored&=ok;}
    for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pair},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"helper_route_ok",helpersOK()}});
    RestoreFloatingState();savedFPActive=false;
}
}
