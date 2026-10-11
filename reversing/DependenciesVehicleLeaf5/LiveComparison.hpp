#pragma once
#include <fstream>
#include <float.h>
#include "CarCtrl.h"
#include "PathFind.h"
#include "Physical.h"
#include "ScriptsForBrains.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperHookRoute.hpp"

namespace DependenciesVehicleLeaf5Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush(); }
struct Fn { const char* cls;const char* name;uintptr_t address,vt;int slot,cases; };
inline const std::array<Fn,4> fns{{
    {"CCarCtrl","FindSpeedMultiplierWithSpeedFromNodes",0x424130,0,-1,1024},
    {"CPathFind","AddRandomOffsetToNodePosition",0x44DA30,0,-1,2048},
    {"CPhysical","ApplyTurnForceComponents",0x65DA70,0,-1,1024},
    {"CScriptsForBrains","MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded",0x46AAE0,0,-1,1024},
}};
static_assert(offsetof(CPathFind,m_pPathNodes)==0x804);
static_assert(sizeof(CPathNode)==0x1C);
static_assert(offsetof(CPathNode,m_nPathWidth)==0x16);
static_assert(sizeof(CPhysical)==0x138);
static_assert(offsetof(CPhysical,m_vecTurnSpeed)==0x50);
static_assert(offsetof(CPhysical,m_fTurnMass)==0x90);
static_assert(sizeof(tScriptForBrains)==0x14);
static_assert(sizeof(CStreamedScripts::CStreamedScriptInfo)==0x20);
alignas(16) inline std::array<uint8,16+sizeof(CPathFind)+16> path{};
alignas(16) inline std::array<uint8,16+4*0x1C+16> nodeA{},nodeB{};
alignas(16) inline std::array<uint8,48> coordinates{};
alignas(16) inline std::array<uint8,16+sizeof(CPhysical)+16> physical{};
alignas(16) inline std::array<uint8,16+0x48+16> matrix{};
alignas(16) inline std::array<uint8,16+sizeof(CScriptsForBrains)+16> brains{};
alignas(16) inline std::array<uint8,48> nameBuffer{};
inline constexpr uintptr_t streamedBegin=0xA47B50;
inline constexpr size_t streamedSize=16+82*0x20+16;
struct ABIResult { bool stack,registers;uint32 floatBits; };
inline ABIResult CallRaw(uintptr_t callAddress,void* self,const std::array<uint32,6>& args,unsigned arity,bool calleePop,bool returnFloat) {
    const auto* argPointer=args.data();const unsigned callerBytes=calleePop ? 0:arity*4;
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    uint32 returnBits{};
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
        push dword ptr [ecx+edx*4-4]
        dec edx
        jnz pushArgument
        mov ecx,self
        call eax
        add esp,callerBytes
    }
    if(returnFloat) { __asm fstp dword ptr returnBits }
    __asm {
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter,ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter,returnBits};
}
inline void Put(uint8* p,uint32 v) { std::memcpy(p,&v,4); }
inline void Float(uint8* p,float v) { Put(p,std::bit_cast<uint32>(v)); }
inline void Vector(uint8* p,float x,float y,float z) { Float(p,x);Float(p+4,y);Float(p+8,z); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesvehicleleaf5-runtime.start"))return;
    if(!log.is_open()) {log.open(dir/"dependenciesvehicleleaf5-runtime.jsonl");ReversingProbe::InstallFailureCapture("dependenciesvehicleleaf5");Write({{"event","waiting_for_player"}});}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    const auto pathMember=&CPathFind::AddRandomOffsetToNodePosition;
    const auto forceMember=&CPhysical::ApplyTurnForceComponents;
    const auto brainMember=&CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded;
    const std::array<uintptr_t,4> symbols{reinterpret_cast<uintptr_t>(&CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes),
        reinterpret_cast<uintptr_t>(FunctionToVoidPtr(pathMember)),reinterpret_cast<uintptr_t>(FunctionToVoidPtr(forceMember)),
        reinterpret_cast<uintptr_t>(FunctionToVoidPtr(brainMember))};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> finalEntries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f]})finalEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute speedRoute("CCarCtrl","FindSpeedMultiplierWithSpeedFromNodes",0x424130,&CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes);
    OriginalDependencies499::HookRoute pathRoute("CPathFind","AddRandomOffsetToNodePosition",0x44DA30,FunctionToVoidPtr(pathMember));
    OriginalDependencies499::HookRoute forceRoute("CPhysical","ApplyTurnForceComponents",0x65DA70,FunctionToVoidPtr(forceMember));
    OriginalDependencies499::HookRoute brainRoute("CScriptsForBrains","MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded",0x46AAE0,FunctionToVoidPtr(brainMember));
    std::array<OriginalDependencies499::HookRoute*,4> routes{&speedRoute,&pathRoute,&forceRoute,&brainRoute};
    HelperHookRoute turnHelper("CPhysical","ApplyTurnForce",0x542A50,FunctionToVoidPtr(&CPhysical::ApplyTurnForce));
    HelperHookRoute indexHelper("CScriptsForBrains","GetIndexOfScriptBrainWithThisName",0x46AA30,
        FunctionToVoidPtr(&CScriptsForBrains::GetIndexOfScriptBrainWithThisName));
    const auto savedPath=path;const auto savedNodeA=nodeA;const auto savedNodeB=nodeB;const auto savedCoordinates=coordinates;
    const auto savedPhysical=physical;const auto savedMatrix=matrix;const auto savedBrains=brains;const auto savedName=nameBuffer;
    const auto savedStreamed=ReversingProbe::Bytes(reinterpret_cast<void*>(streamedBegin),streamedSize);const unsigned savedFP=_controlfp(0,0);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f],routes[f]->Body()})entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    auto restore=[&] {
        path=savedPath;nodeA=savedNodeA;nodeB=savedNodeB;coordinates=savedCoordinates;physical=savedPhysical;matrix=savedMatrix;brains=savedBrains;nameBuffer=savedName;
        std::memcpy(reinterpret_cast<void*>(streamedBegin),savedStreamed.data(),savedStreamed.size());_controlfp(savedFP,_MCW_PC|_MCW_RC);
    };
    auto restored=[&] {
        bool ok=turnHelper.Check()&&indexHelper.Check()&&path==savedPath&&nodeA==savedNodeA&&nodeB==savedNodeB&&coordinates==savedCoordinates&&physical==savedPhysical&&matrix==savedMatrix&&brains==savedBrains&&nameBuffer==savedName
            &&savedStreamed==ReversingProbe::Bytes(reinterpret_cast<void*>(streamedBegin),streamedSize)&&_controlfp(0,0)==savedFP;
        for(auto& [at,bytes]:entries)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);return ok;
    };
    const bool allRegistered=std::ranges::all_of(routes,[](auto* r){return r->Registered();})&&turnHelper.Registered()&&indexHelper.Registered();
    const bool helperRoutes=turnHelper.Check()&&indexHelper.Check();
    Write({{"event","begin"},{"functions",4},{"pairs",5120},{"registered_hook",allRegistered},{"collaborators_stubbed",false},
        {"helper_route_ok",helperRoutes},{"helpers",json::array({turnHelper.Metadata("CPhysical","ApplyTurnForce"),
            indexHelper.Metadata("CScriptsForBrains","GetIndexOfScriptBrainWithThisName")})}});
    if(!allRegistered||!helperRoutes) {restore();bool ok=restored();for(auto* r:routes)ok&=r->Restore();ok&=turnHelper.Restore();ok&=indexHelper.Restore();Write({{"event","error"},{"reason","Los cuatro hooks y dos colaboradores deben tener rutas registradas correctas"},{"state_restored",ok}});return;}
    unsigned pair=0;bool passed=true,allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0;const std::string label=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<unsigned(fns[f].cases);++fixture,++pair) {
            auto run=[&](bool candidate) {
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                path.fill(0xA7);nodeA.fill(0xD1);nodeB.fill(0xE2);coordinates.fill(0xB3);physical.fill(0xC4);matrix.fill(0xA5);brains.fill(0xD6);nameBuffer.fill(0xF7);
                void* self=nullptr;std::array<uint32,6> args{};unsigned arity=1;
                unsigned profile=fixture/256;unsigned byte=fixture%256;unsigned touchedScript=0;bool found=false;
                if(f==0)args[0]=(profile*0x55667700u)|byte;
                if(f==1) {
                    self=path.data()+16;auto* table=path.data()+16+0x804;
                    Put(table,uint32(uintptr_t(nodeA.data()+16)));Put(table+4,uint32(uintptr_t(nodeB.data()+16)));Put(table+71*4,uint32(uintptr_t(nodeB.data()+16)));
                    for(unsigned i=0;i<4;++i) {nodeA[16+i*0x1C+0x16]=uint8(byte);nodeB[16+i*0x1C+0x16]=uint8(255-byte);}
                    const unsigned node1=byte%4,node2=(byte/4)%4;uint32 first=node1<<16,second=(node2<<16)|1;
                    if(profile==1)std::swap(first,second);
                    if(profile==2)second=first;
                    if(profile==3) {second=(node2<<16)|71;nodeB[16+node2*0x1C+0x16]=0;}
                    if(profile==4)first=0x1234FFFFu;
                    if(profile==5)second=0x1234FFFFu;
                    if(profile==6) {first=(node1<<16)|2;Put(table+8,0);}
                    if(profile==7) {second=(node2<<16)|3;Put(table+12,0);}
                    Float(coordinates.data()+16,float(int(byte)-128)*.125f);Float(coordinates.data()+20,-float(byte)*.25f);
                    auto* x=coordinates.data()+16;auto* y=coordinates.data()+((profile&1)?16:20);
                    const uint16 random=uint16((profile&1 ? 0xA500u:0x7F00u)|byte);
                    args={first,second,uint32(random),uint32(uintptr_t(x)),uint32(uintptr_t(y)),0};arity=5;
                }
                if(f==2) {
                    self=physical.data()+16;auto* body=physical.data()+16;auto* m=matrix.data()+16;
                    Put(body+0x14,uint32(uintptr_t(m)));body[0x40]=uint8(((fixture/64)%8)<<4);
                    Float(body+0x8C,2.5f);Float(body+0x90,(fixture/512)?12.5f:.5f);
                    Vector(body+0x50,.5f,-.75f,1.25f);Vector(body+0xA4,.25f,-.5f,.75f);
                    const float angle=float(fixture%16)*.125f,c=std::cos(angle),s=std::sin(angle);
                    Vector(m,c,s,0);Vector(m+16,-s,c,0);Vector(m+32,0,0,1);Vector(m+48,2,3,4);
                    const unsigned force=fixture%8,point=(fixture/8)%8;
                    const std::array<std::array<float,3>,8> values{{{0,0,0},{1,0,0},{0,1,0},{0,0,1},{-1,2,3},{.25f,-.5f,.75f},{4,-2,1},{-3,-2,-1}}};
                    for(unsigned i=0;i<3;++i) {args[i]=std::bit_cast<uint32>(values[force][i]);args[3+i]=std::bit_cast<uint32>(values[point][i]);}arity=6;
                }
                if(f==3) {
                    self=brains.data()+16;auto* body=brains.data()+16;
                    for(unsigned i=0;i<70;++i) {body[i*20+2]=4;body[i*20]=0;body[i*20+1]=0;std::memset(body+i*20+12,0,8);std::memcpy(body+i*20+12,"OTHER",5);}
                    const auto record=[&](unsigned slot,unsigned script) {body[slot*20+2]=5;body[slot*20]=uint8(script);body[slot*20+1]=0;std::memset(body+slot*20+12,0,8);std::memcpy(body+slot*20+12,"wave",4);};
                    if(profile==0) {record(0,0);touchedScript=0;found=true;}
                    if(profile==1) {record(69,81);touchedScript=81;found=true;}
                    if(profile==2) {record(0,17);record(69,81);touchedScript=17;found=true;}
                    std::memcpy(nameBuffer.data()+16,(byte&1)?"WAVE":"wave",5);
                    for(unsigned i=0;i<82;++i)*reinterpret_cast<uint8*>(0xA47B64+i*32)=uint8(i*7+fixture);
                    *reinterpret_cast<uint8*>(0xA47B64+touchedScript*32)=uint8(byte);
                    args[0]=uint32(uintptr_t(nameBuffer.data()+16));
                }
                const auto beforePath=path;const auto beforeNodeA=nodeA;const auto beforeNodeB=nodeB;const auto beforeCoordinates=coordinates;
                const auto beforePhysical=physical;const auto beforeMatrix=matrix;const auto beforeBrains=brains;const auto beforeName=nameBuffer;
                const auto beforeStreamed=ReversingProbe::Bytes(reinterpret_cast<void*>(streamedBegin),streamedSize);
                bool routeOK=false,helperOK=false;ABIResult abi{};
                routes[f]->Invoke(candidate,[&] {
                    routeOK=ReversingProbe::JumpsToDll(fns[f].address)==candidate;
                    helperOK=turnHelper.Check()&&indexHelper.Check();
                    if(helperOK)abi=CallRaw(fns[f].address,self,args,arity,f!=0,f==0);
                    helperOK&=turnHelper.Check()&&indexHelper.Check();return true;
                });
                const auto afterStreamed=ReversingProbe::Bytes(reinterpret_cast<void*>(streamedBegin),streamedSize);
                bool expectedOK=true,protectedOK=path==beforePath&&nodeA==beforeNodeA&&nodeB==beforeNodeB&&matrix==beforeMatrix&&brains==beforeBrains&&nameBuffer==beforeName;
                auto maskPhysical=physical,maskBeforePhysical=beforePhysical;auto maskCoordinates=coordinates,maskBeforeCoordinates=beforeCoordinates;
                if(f==0) {
                    const uint32 expected=byte==255 ? 0x3F000000u:byte==0 ? 0x3F266666u:byte==2 ? 0x40133333u:0x3F800000u;
                    expectedOK=abi.floatBits==expected;
                }
                if(f==1&&profile<4) {
                    for(unsigned i=0;i<8;++i)maskCoordinates[16+i]=maskBeforeCoordinates[16+i]=0;
                    if(profile&1)expectedOK=std::equal(coordinates.begin()+20,coordinates.begin()+24,beforeCoordinates.begin()+20);
                }
                if(f==2) {
                    if(beforePhysical[16+0x40]&0x10)expectedOK&=physical==beforePhysical;
                    for(unsigned component=0;component<3;++component) {
                        float value;std::memcpy(&value,physical.data()+16+0x50+component*4,4);
                        expectedOK&=std::isfinite(value);
                    }
                    for(unsigned i=0;i<12;++i)maskPhysical[16+0x50+i]=maskBeforePhysical[16+0x50+i]=0;
                }
                auto expectedStreamed=beforeStreamed;
                if(f==3&&found) {auto& users=expectedStreamed[0x14+touchedScript*32];if(users)--users;}
                expectedOK&=expectedStreamed==afterStreamed;
                protectedOK&=maskPhysical==maskBeforePhysical&&maskCoordinates==maskBeforeCoordinates;
                json result{{"route_ok",routeOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                    {"helper_route_ok",helperOK},
                    {"protected_state_unchanged",protectedOK},{"expected_effects_correct",expectedOK},{"return_bits",abi.floatBits},
                    {"physical",physical},{"coordinates",coordinates},{"profile",profile},{"byte",byte}};
                if(f==3)result["streamed_state_guards"]=afterStreamed;
                restore();return result;
            };
            const auto original=run(false),candidate=run(true);const bool stateOK=restored(),equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["helper_route_ok"]==true&&original["stack_restored"]==true&&original["nonvolatile_registers_preserved"]==true
                &&original["protected_state_unchanged"]==true&&original["expected_effects_correct"]==true;
            failures+=!accepted;passed&=accepted;allRestored&=stateOK;
            Write({{"event","comparison"},{"function",label},{"pair",pair},{"case",fixture},{"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",label},{"cases",fns[f].cases},{"failures",failures}});
    }
    restore();allRestored&=restored();for(auto* r:routes)allRestored&=r->Restore();allRestored&=turnHelper.Restore();allRestored&=indexHelper.Restore();for(auto& [at,bytes]:finalEntries)allRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&allRestored?"complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
