#pragma once
#include <fstream>
#include "CarCtrl.h"
#include "Vehicle.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesCarCtrl4Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot,cases; };
inline const std::array<Fn,2> fns{{
    {"CCarCtrl","SwitchVehicleToRealPhysics",0x423FC0,0,-1,1024},
    {"CCarCtrl","UpdateCarCount",0x424000,0,-1,1024},
}};
static_assert(offsetof(CVehicle,m_autoPilot)==0x390);
static_assert(offsetof(CAutoPilot,m_nTimeToStartMission)==0x1C);
static_assert(offsetof(CAutoPilot,m_nTimeSwitchedToRealPhysics)==0x20);
static_assert(offsetof(CAutoPilot,m_nTempAction)==0x2B);
static_assert(offsetof(CVehicle,m_nCreatedBy)==0x4A4);
static_assert(offsetof(CVehicle,vehicleFlags)==0x428);
static_assert(offsetof(CPhysical,m_nFakePhysics)==0xB8);
static_assert(sizeof(CEntity)==0x38); // Native packed type/status byte is at 0x36 (protected in C++).
inline const std::array<std::pair<uintptr_t,size_t>,2> globals{{
    {0x969084,0x34}, // surrounding pointers/guards and all five counters
    {0xB7CB84,4},   // millisecond timer
}};
alignas(16) inline std::array<uint8,16+sizeof(CVehicle)+16> vehicle{};
struct ABIResult { bool stack,registers; };
inline ABIResult CallRaw(uintptr_t callAddress,const std::array<uint32,2>& args,unsigned arity) {
    const auto* argPointer=args.data();
    const unsigned argBytes=arity*4;
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov eax,callAddress
        mov ecx,argPointer
        mov edx,arity
    pushArgument:
        push dword ptr [ecx + edx*4 - 4]
        dec edx
        jnz pushArgument
        call eax
        add esp,argBytes
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter,ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter};
}
inline uint32 Load(const uint8* bytes) { uint32 value;std::memcpy(&value,bytes,4);return value; }
inline void Store(uint8* bytes,uint32 value) { std::memcpy(bytes,&value,4); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciescarctrl4-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"dependenciescarctrl4-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("dependenciescarctrl4");Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    const std::array<uintptr_t,2> symbols{reinterpret_cast<uintptr_t>(&CCarCtrl::SwitchVehicleToRealPhysics),
        reinterpret_cast<uintptr_t>(&CCarCtrl::UpdateCarCount)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> finalEntries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f]})
        finalEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute physicsRoute("CCarCtrl","SwitchVehicleToRealPhysics",0x423FC0,&CCarCtrl::SwitchVehicleToRealPhysics);
    OriginalDependencies499::HookRoute countRoute("CCarCtrl","UpdateCarCount",0x424000,&CCarCtrl::UpdateCarCount);
    std::array<OriginalDependencies499::HookRoute*,2> routes{&physicsRoute,&countRoute};
    const auto savedVehicle=vehicle;
    std::array<std::vector<uint8>,2> saved;
    for(size_t j=0;j<globals.size();++j)saved[j]=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f],routes[f]->Body()})
        entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    auto restore=[&] {
        vehicle=savedVehicle;
        for(size_t j=0;j<globals.size();++j)std::memcpy(reinterpret_cast<void*>(globals[j].first),saved[j].data(),saved[j].size());
    };
    auto restored=[&] {
        bool ok=vehicle==savedVehicle;
        for(size_t j=0;j<globals.size();++j)ok&=saved[j]==ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
        for(auto& [at,bytes]:entries)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    const bool allRegistered=physicsRoute.Registered()&&countRoute.Registered();
    Write({{"event","begin"},{"functions",2},{"pairs",2048},{"registered_hook",allRegistered},{"collaborators_stubbed",false}});
    if(!allRegistered) {
        restore();bool stateOK=restored();for(auto* route:routes)stateOK&=route->Restore();
        Write({{"event","error"},{"reason","Aplicar propuesta y registrar los dos hooks"},{"state_restored",stateOK}});return;
    }
    const std::array<uint32,4> times{0,1999,0xFFFFF830u,0xFFFFFFFFu};
    const std::array<uint8,8> created{1,2,3,4,0,5,128,255};
    const std::array<uint8,8> decrease{0,1,2,17,127,128,254,255};
    const std::array<uint32,8> seeds{0,1,2,0xFFFFFFFFu,0x80000000u,0x7FFFFFFFu,0xFFFFFFFEu,0x12345678u};
    unsigned pair=0;bool passed=true,allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0;const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<1024;++fixture,++pair) {
            auto run=[&](bool candidate) {
                restore();
                for(size_t i=0;i<vehicle.size();++i)vehicle[i]=uint8(fixture*29+i*11);
                auto* body=vehicle.data()+16;
                auto* counters=reinterpret_cast<uint8*>(0x969094);
                const unsigned byIndex=fixture%8,decIndex=(fixture/8)%8,law=(fixture/64)%2,seedIndex=fixture/128;
                body[0x36]=uint8(fixture%256);
                body[0x4A4]=created[byIndex];body[0x428]=uint8((body[0x428]&0xFE)|law);
                for(unsigned i=0;i<5;++i)Store(counters+i*4,seeds[seedIndex==7 ? (i+7)%8 : seedIndex]);
                StaticRef<uint32>(0xB7CB84)=times[fixture/256];
                const auto beforeVehicle=vehicle;
                const auto beforeGlobals=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[0].first),globals[0].second);
                const uint32 beforeTime=StaticRef<uint32>(0xB7CB84);
                auto expectedVehicle=beforeVehicle;auto expectedGlobals=beforeGlobals;
                if(f==0) {
                    expectedVehicle[16+0x36]=uint8((body[0x36]&7)|0x18);
                    expectedVehicle[16+0x3BB]=0;expectedVehicle[16+0xB8]=0;
                    Store(expectedVehicle.data()+16+0x3AC,beforeTime+2000u);
                    Store(expectedVehicle.data()+16+0x3B0,beforeTime);
                } else {
                    const unsigned by=created[byIndex];const bool down=decrease[decIndex]!=0;
                    const auto updateCounter=[&](unsigned slot,bool minus,bool saturate) {
                        auto* dst=expectedGlobals.data()+0x10+slot*4;
                        uint32 value=Load(dst);value+=minus ? 0xFFFFFFFFu:1u;
                        if(saturate&&(value&0x80000000u))value=0;
                        Store(dst,value);
                    };
                    if(by>=1&&by<=4) {
                        if(by==1) {
                            if(law)updateCounter(1,down,down);
                            updateCounter(0,down,down);
                        } else {
                            if(by==2&&!down&&law) {
                                expectedVehicle[16+0x428]&=0xFE;updateCounter(1,true,false);
                            }
                            updateCounter(by==2 ? 2u:by,down,down);
                        }
                    }
                }
                const std::array<uint32,2> args{uint32(uintptr_t(body)),decrease[decIndex]};
                bool routeOK=false;ABIResult abi{};
                routes[f]->Invoke(candidate,[&] {
                    routeOK=ReversingProbe::JumpsToDll(fns[f].address)==candidate;
                    abi=CallRaw(fns[f].address,args,f==0 ? 1u:2u);return true;
                });
                const auto afterGlobals=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[0].first),globals[0].second);
                const uint32 afterTime=StaticRef<uint32>(0xB7CB84);
                bool guardsOK=std::equal(vehicle.begin(),vehicle.begin()+16,beforeVehicle.begin())
                    &&std::equal(vehicle.end()-16,vehicle.end(),beforeVehicle.end()-16)
                    &&std::equal(afterGlobals.begin(),afterGlobals.begin()+0x10,beforeGlobals.begin())
                    &&std::equal(afterGlobals.begin()+0x24,afterGlobals.end(),beforeGlobals.begin()+0x24)&&beforeTime==afterTime;
                const bool expectedOK=vehicle==expectedVehicle&&afterGlobals==expectedGlobals&&beforeTime==afterTime;
                json result{{"route_ok",routeOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                    {"protected_state_unchanged",guardsOK},{"expected_effects_correct",expectedOK},{"vehicle",vehicle},
                    {"globals",afterGlobals},{"time",afterTime},{"created_by",created[byIndex]},{"decrease",decrease[decIndex]},
                    {"law_enforcer",law},{"seed_profile",seedIndex},{"initial_status",beforeVehicle[16+0x36]},
                    {"timer_profile",fixture/256}};
                restore();return result;
            };
            const auto original=run(false),candidate=run(true);
            const bool stateOK=restored(),equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true&&original["expected_effects_correct"]==true;
            failures+=!accepted;passed&=accepted;allRestored&=stateOK;
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},{"equal",equal},{"accepted",accepted},
                {"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",1024},{"failures",failures}});
    }
    restore();allRestored&=restored();for(auto* route:routes)allRestored&=route->Restore();
    for(auto& [at,bytes]:finalEntries)allRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
