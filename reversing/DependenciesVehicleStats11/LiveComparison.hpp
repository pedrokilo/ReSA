#pragma once
#include <fstream>
#include "Stats.h"
#include "CutsceneMgr.h"
#include "Hud.h"
#include "World.h"
#include "Pad.h"
#include "PlayerInfo.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Bmx.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperRoutes.hpp"

namespace DependenciesVehicleStats11Live {
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
    {"CStats","UpdateStatsWhenCycling",0x55C780,0,-1,2048},
    {"CVehicle","IsValidModForVehicle",0x49B010,0,-1,2048}
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

inline uint32 ReadInt(uintptr_t a){uint32 v;memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutWord(uintptr_t a,int16 v){memcpy(reinterpret_cast<void*>(a),&v,2);}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesvehiclestats11-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesvehiclestats11-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesvehiclestats11");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    const std::array<uintptr_t,2> symbols{uintptr_t(&CStats::UpdateStatsWhenCycling),uintptr_t(&IsValidModForVehicle)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;
    for(unsigned i=0;i<2;++i)for(auto at:{fns[i].address,symbols[i],Resolve(symbols[i])})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute r0("CStats",fns[0].name,fns[0].address,&CStats::UpdateStatsWhenCycling);
    OriginalDependencies499::HookRoute r1("CVehicle",fns[1].name,fns[1].address,&IsValidModForVehicle);
    OriginalDependencies499::HookRoute* routes[]{&r0,&r1};
    std::array<std::vector<uint8>,2> offCode;
    for(unsigned i=0;i<2;++i)offCode[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    alignas(16)std::array<uint8,0xA20> vehicle{};
    alignas(16)std::array<uint8,0x340> carModel{},upgradeModel{};
    const uintptr_t veh=uintptr_t(vehicle.data()+16),car=uintptr_t(carModel.data()+16),upgrade=uintptr_t(upgradeModel.data()+16);
    std::array<int,2> slots{-1,-1};unsigned found=0;
    for(int id=19999;id>=19000&&found<2;--id)if(!ReadInt(0xA9B0C8+4*id))slots[found++]=id;
    auto* info=reinterpret_cast<uint8*>(&CWorld::Players[0]);auto* pad=CPad::GetPad(0);
    std::vector<Block> blocks;
    blocks.emplace_back(0xB78200,0x1338);blocks.emplace_back(0xBAA460,0x4D0);blocks.emplace_back(0xB71510,0x2C0);
    blocks.emplace_back(uintptr_t(info),sizeof(CPlayerInfo));blocks.emplace_back(uintptr_t(&CWorld::Players[1]),sizeof(CPlayerInfo));
    if(pad)blocks.emplace_back(uintptr_t(&pad->JustOutOfFrontEnd),sizeof(pad->JustOutOfFrontEnd));
    for(uintptr_t a:{uintptr_t(0x8CDE56),uintptr_t(0xB6F065),uintptr_t(0xB5F852),uintptr_t(0xBA82E3),uintptr_t(0xB7CD74),uintptr_t(0x96918C),uintptr_t(0x96C014),uintptr_t(0xBAACC0+4*128),uintptr_t(0xA43088)})blocks.emplace_back(a,1);
    blocks.emplace_back(0xC1AEB8,32);blocks.emplace_back(0xB7CB5C,4);blocks.emplace_back(0xC9C400,4);
    blocks.emplace_back(0xB4E3E8,0xA0);
    if(found==2)for(int id:slots)blocks.emplace_back(0xA9B0C8+4*id,4);
    blocks.emplace_back(uintptr_t(vehicle.data()),vehicle.size());blocks.emplace_back(uintptr_t(carModel.data()),carModel.size());blocks.emplace_back(uintptr_t(upgradeModel.data()),upgradeModel.size());
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
    auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
    std::array<HelperRoute,13> helpers{HelperRoute(0x53FB70),HelperRoute(0x588B50),HelperRoute(0x558E40),HelperRoute(0x6A0050),HelperRoute(0x588BE0),HelperRoute(0x559760),HelperRoute(0x821B40),HelperRoute(0x823820),HelperRoute(0x55C470),HelperRoute(0x55C180),HelperRoute(0x55B980),HelperRoute(0x4C8740),HelperRoute(0x4C8750)};
    auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
    bool ready=pad&&found==2&&helpersOK();for(auto* route:routes)ready&=route->Registered()&&ModuleOf(route->Body())==ModuleOf(uintptr_t(&ModuleMarker));
    for(const auto& h:helpers){const auto m=h.Metadata();const auto at=m["entry"].get<uintptr_t>();if(at==0x55C470||at==0x55C180||at==0x55B980||at==0x4C8740||at==0x4C8750)ready&=m["registered"]==true&&m["reconstructed"]==true&&m["enabled"]==true;}
    json metadata=json::array();for(const auto& h:helpers)metadata.push_back(h.Metadata());
    json descriptors=json::array();for(const auto& b:blocks)descriptors.push_back({{"address",b.address},{"bytes",b.saved.size()}});
    Write({{"event","begin"},{"registered_hook",ready},{"helper_routes",metadata},{"helper_routes_ok",helpersOK()},{"floating_state_restored",FloatingStateRestored()},{"functions",2},{"pairs",4096},{"unused_model_slots",slots},{"unused_model_slots_proven",found==2},{"snapshot_blocks",blocks.size()},{"snapshot_layout",descriptors},{"global_cpp_function_owner","CVehicle::IsValidModForVehicle"},{"collaborators_stubbed",false},{"exe_wheel_collaborators_during_call",true}});
    const uint32 counters[]{0,1,999,1000,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF};
    const uint32 limits[]{0xBF800000,0,0x3F800000,0x3F7FFFFF,0x3F800001,0x49800000,0x4A800000,0x7F800000,0x7FC01234};
    const uint32 speeds[]{0,0x3D4CCCCC,0x3D4CCCCD,0x3D4CCCCE,0x3E4CCCCC,0x3E4CCCCD,0x3E4CCCCE,0x3F800000,0xBF800000,0x7FC01234,0x3E800000,0x3D800000,0x3C800000,0x3F000000,0x40000000,0x00000001};
    const uint32 pedals[]{0,0xBF800000,0x3E4CCCCD,0x7FC01234};
    const float steps[]{0.f,0.01f,0.5f,1.f,1.999f,3.f,8.f,-1.f};
    auto* loadedPed=FindPlayerPed();bool passed=ready;int pair=0;
    for(int fn=0;fn<2&&ready;++fn){int failures=0;
        for(int scenario=0;scenario<2048;++scenario){const unsigned profile=scenario%16,value=scenario/16;
            auto invoke=[&](bool candidate){
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                memset(vehicle.data(),0xA5,vehicle.size());memset(reinterpret_cast<void*>(veh),0,0xA00);
                memset(carModel.data(),0x5A,carModel.size());memset(reinterpret_cast<void*>(car),0,0x320);
                memset(upgradeModel.data(),0xC3,upgradeModel.size());memset(reinterpret_cast<void*>(upgrade),0,0x320);
                uint32 arg0=0,arg1=uint32(veh);json input={{"profile",profile},{"value",value}};
                if(fn==0){
                    memset(reinterpret_cast<void*>(0xBAA460),0,0x4D0);memset(CStats::StatTypesInt.data(),0,sizeof(CStats::StatTypesInt));memset(CStats::StatTypesFloat.data(),0,sizeof(CStats::StatTypesFloat));
                    CStats::TotalNumStatMessages=0;CStats::bStatUpdateMessageDisplayed=false;CStats::bShowUpdateStats=true;
                    for(uintptr_t a:{uintptr_t(0xB6F065),uintptr_t(0xB5F852),uintptr_t(0xBA82E3),uintptr_t(0xB7CD74),uintptr_t(0x96918C),uintptr_t(0x96C014),uintptr_t(0xBAACC0+4*128),uintptr_t(0xA43088)})*reinterpret_cast<uint8*>(a)=0;
                    info[0x117]=0;pad->JustOutOfFrontEnd=0;CWorld::Players[0].m_pPed=loadedPed;CWorld::Players[1].m_pPed=nullptr;
                    arg0=(0xA55A5A00u^(value<<8))|(profile&1);
                    const uint32 pedal=pedals[(profile/2)%4],speed=speeds[value%16],contact=profile<8?0:1+value%2;
                    const uint32 stamina=counters[value%8],skill=counters[(value/8)%8],staminaLimit=limits[(value/4)%9],skillLimit=limits[(value/7)%9];
                    const float step=steps[(value/3)%8],increment=float(value%3)*25.f;
                    PutInt(veh+0x49C,pedal);PutInt(veh+0x44,speed);*reinterpret_cast<uint8*>(veh+0x804)=uint8(contact);
                    PutInt(0xB794DC,stamina);PutInt(0xB794E0,skill);PutInt(0xB78F94,staminaLimit);PutInt(0xB78FAC,skillLimit);
                    PutFloat(0xB78F78,1.f);PutFloat(0xB78F7C,3.f);PutFloat(0xB78F10,increment);PutFloat(0xB78F28,increment);PutFloat(0xB7CB5C,step);
                    PutInt(0xB794FC,counters[(value/5)%8]);PutInt(0xB7950C,counters[(value/11)%8]);PutFloat(0xB78FB8,1.f);PutFloat(0xB78FC4,1.f);PutFloat(0xB793D4,25.f);PutFloat(0xB793DC,25.f);PutFloat(0xB78F6C,1.f);PutFloat(0xB78F70,1.f);PutFloat(0xB78F34,increment);PutFloat(0xB78F3C,increment);*reinterpret_cast<uint8*>(0xB79534)=uint8(profile%4);
                    input.update({{"pedal_bits",pedal},{"speed_bits",speed},{"contact",contact},{"stamina_counter",stamina},{"skill_counter",skill},{"stamina_limit_bits",staminaLimit},{"skill_limit_bits",skillLimit},{"step_bits",std::bit_cast<uint32>(step)},{"increment_bits",std::bit_cast<uint32>(increment)}});
                }else{
                    arg0=uint32(slots[1]);PutWord(veh+0x22,int16(slots[0]));PutInt(0xA9B0C8+4*slots[0],uint32(car));PutInt(0xA9B0C8+4*slots[1],uint32(upgrade));
                    const unsigned group=profile%4,count=profile==0?0:1+value%15;
                    const uint16 flags=profile<4?0x900:profile>=8&&profile<=11?0x4400:profile==12?0x1100:profile==13?0x800:profile==14?0x4500:0;
                    PutWord(upgrade+0x12,int16(flags));*reinterpret_cast<uint8*>(car+0x4F)=uint8(group);
                    *reinterpret_cast<uint8*>(veh+0x1D3)=profile==8?1:0;*reinterpret_cast<uint8*>(veh+0x1BE)=profile==11?0:1;*reinterpret_cast<uint8*>(veh+0x42E)=profile==10?0x10:0;
                    json wheels=json::array(),upgrades=json::array();const int wheelHit=profile==1?0:profile==2?int(count)-1:-1;
                    const int normalHit=profile==4||profile==12?0:profile==5||profile==13?8:profile==6||profile==14?17:-1;
                    for(int g=0;g<4;++g){PutWord(0xB4E470+2*g,int16(g==int(group)?count:0));for(int n=0;n<15;++n)PutWord(0xB4E3F8+2*(15*g+n),int16(g==int(group)&&n==wheelHit?arg0:1000+g*15+n));}
                    for(int n=0;n<15;++n)wheels.push_back(*reinterpret_cast<int16*>(0xB4E3F8+2*(15*group+n)));
                    for(int n=0;n<18;++n){const int16 id=n==normalHit?int16(arg0):int16(-1);PutWord(car+0x2D6+2*n,id);upgrades.push_back(id);}
                    input.update({{"flags",flags},{"group",group},{"count",count},{"vehicle_model",slots[0]},{"candidate_model",slots[1]},{"wheel_hit",wheelHit},{"normal_hit",normalHit},{"nitro_blocked",profile==8},{"vehicle_type",profile==11?0:1},{"nitro_flag",profile==10},{"wheels",wheels},{"upgrades",upgrades}});
                }
                input["arg0"]=arg0;input["arg1"]=arg1;
                const auto vehicleBefore=blocks[blocks.size()-3].Observe(),carBefore=blocks[blocks.size()-2].Observe(),upgradeBefore=blocks.back().Observe();
                std::vector<std::string> prepared;for(const auto& b:blocks)prepared.push_back(b.Observe());
                bool routeOK=false,helperOK=helpersOK();const auto result=routes[fn]->Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;
                    if(fn==1){helpers[11].UseExeCode(true);helpers[12].UseExeCode(true);}   // 0x4C8740 / 0x4C8750 con su codigo del EXE durante la llamada
                    const auto gate=Gate(fns[fn].address,true,arg0,arg1);
                    if(fn==1){helpers[11].UseExeCode(false);helpers[12].UseExeCode(false);}
                    return gate;});helperOK&=helpersOK();
                json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
                const bool fixtureReadonly=vehicleBefore==blocks[blocks.size()-3].Observe()&&carBefore==blocks[blocks.size()-2].Observe()&&upgradeBefore==blocks.back().Observe();
                bool allReadonly=true;if(fn==1)for(unsigned n=0;n<blocks.size();++n)allReadonly&=prepared[n]==blocks[n].Observe();
                json out={{"input",input},{"eax",result.eax},{"boolean_result",result.eax&255},{"ecx",result.ecx},{"edx",result.edx},{"argument0_after",result.arg0},{"argument1_after",result.arg1},{"stamina_after",ReadInt(0xB794DC)},{"skill_after",ReadInt(0xB794E0)},{"state",state},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"argument0_unchanged",result.arg0==arg0},{"fixture_readonly",fixtureReadonly},{"mod_globals_readonly",allReadonly}};
                restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
            };
            const auto original=invoke(false),candidate=invoke(true);bool ok=original==candidate;
            for(const char* gate:{"stack_restored","nonvolatile_preserved","route_ok","helper_route_ok","argument0_unchanged","fixture_readonly","mod_globals_readonly","globals_restored","floating_state_restored"})ok&=original[gate]==true&&candidate[gate]==true;
            passed&=ok;failures+=!ok;
            Write({{"event","comparison"},{"function",std::string(fns[fn].cls)+"::"+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});
        }
        Write({{"event","function_summary"},{"function",std::string(fns[fn].cls)+"::"+fns[fn].name},{"cases",2048},{"failures",failures}});
    }
    restore();bool stateRestored=restored()&&helpersOK();for(unsigned i=0;i<2;++i)stateRestored&=offCode[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    for(auto* route:routes)stateRestored&=route->Restore();for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pair},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"helper_route_ok",helpersOK()}});
    RestoreFloatingState();savedFPActive=false;
}
}
