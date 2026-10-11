#pragma once
#include <fstream>
#include <float.h>
#include "Sprite2d.h"
#include "common.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesMathSprite2Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot,cases; };
inline const std::array<Fn,3> fns{{
    {"CSprite2d","OffsetTexCoordForBilinearFiltering",0x728150,0,-1,1024},
    {"Common","TransformPoint",0x54ECE0,0,-1,1024},
    {"Common","TransformPoints",0x54EEF0,0,-1,1024},
}};
inline const std::array<std::pair<uintptr_t,size_t>,2> globals{{
    {0xC80448,16+8*28+16+16}, // contadores, Z/reciprocal, los ocho vertices y guardas
    {0xB72920,4},            // CGame::m_pWorkingMatrix1
}};
alignas(16) inline std::array<uint8,16+16*12+16> input{},output{};
alignas(16) inline std::array<uint8,16+16+16> simple{};
alignas(16) inline std::array<uint8,16+0x48+16> matrix{};
alignas(16) inline std::array<uint8,16+64+16> working{};
struct ABIResult { bool stack,registers; };
inline ABIResult CallRaw(uintptr_t callAddress,const std::array<uint32,4>& args,unsigned arity) {
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
inline void PutFloat(uint8* p,float value) { std::memcpy(p,&value,4); }
inline void PutVector(uint8* p,float x,float y,float z) { PutFloat(p,x);PutFloat(p+4,y);PutFloat(p+8,z); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesmathsprite2-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"dependenciesmathsprite2-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("dependenciesmathsprite2");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    OriginalDependencies499::HookRoute spriteRoute("CSprite2d","OffsetTexCoordForBilinearFiltering",0x728150,&CSprite2d::OffsetTexCoordForBilinearFiltering);
    OriginalDependencies499::HookRoute pointRoute("Common","TransformPoint",0x54ECE0,&TransformPoint);
    OriginalDependencies499::HookRoute pointsRoute("Common","TransformPoints",0x54EEF0,&TransformPoints);
    std::array<OriginalDependencies499::HookRoute*,3> routes{&spriteRoute,&pointRoute,&pointsRoute};
    const std::array<uintptr_t,3> symbols{
        reinterpret_cast<uintptr_t>(&CSprite2d::OffsetTexCoordForBilinearFiltering),
        reinterpret_cast<uintptr_t>(&TransformPoint),reinterpret_cast<uintptr_t>(&TransformPoints)};
    std::array<std::vector<uint8>,2> saved;
    for(size_t j=0;j<globals.size();++j)
        saved[j]=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
    const auto savedInput=input,savedOutput=output;
    const auto savedSimple=simple;
    const auto savedMatrix=matrix;
    const auto savedWorking=working;
    const unsigned savedFP=_controlfp(0,0);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f],routes[f]->Body()})
        entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    auto restore=[&] {
        for(size_t j=0;j<globals.size();++j)
            std::memcpy(reinterpret_cast<void*>(globals[j].first),saved[j].data(),saved[j].size());
        input=savedInput;output=savedOutput;simple=savedSimple;matrix=savedMatrix;working=savedWorking;
        _controlfp(savedFP,_MCW_PC|_MCW_RC);
    };
    auto restored=[&] {
        bool ok=input==savedInput&&output==savedOutput&&simple==savedSimple&&matrix==savedMatrix&&working==savedWorking&&_controlfp(0,0)==savedFP;
        for(size_t j=0;j<globals.size();++j)
            ok&=saved[j]==ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
        for(auto& [at,bytes]:entries)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    const bool allRegistered=spriteRoute.Registered()&&pointRoute.Registered()&&pointsRoute.Registered();
    Write({{"event","begin"},{"functions",3},{"pairs",3072},{"registered_hook",allRegistered},
           {"collaborators_stubbed",false}});
    if(!allRegistered) {
        restore();bool stateOK=restored();for(auto* route:routes)stateOK&=route->Restore();
        Write({{"event","error"},{"reason","Aplicar source-proposal.patch y registrar los tres hooks antes del banco"},{"state_restored",stateOK}});
        return;
    }
    const std::array<float,16> dimensions{1,2,3,4,7,8,15,16,31,32,64,128,256,512,1920,4096};
    const std::array<int,8> counts{0,1,2,3,4,7,12,16};
    unsigned pair=0;bool passed=true,allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0;
        const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<1024;++fixture,++pair) {
            auto run=[&](bool candidate) {
                restore();
                for(size_t j=0;j<globals.size();++j)if(j==0) {
                    auto* region=reinterpret_cast<uint8*>(globals[j].first);
                    for(size_t k=0;k<globals[j].second;++k)region[k]=uint8(fixture*29+k*11);
                }
                input.fill(0xA7);output.fill(0xCD);simple.fill(0x5A);matrix.fill(0x6B);working.fill(0x91);
                auto* in=input.data()+16;
                auto* out=output.data()+16;
                for(unsigned i=0;i<16;++i)PutVector(in+i*12,float(int(fixture%17)-8)+i*0.375f,
                    float(int((fixture/8)%13)-6)-i*0.25f,float(int(fixture%7)-3)+i*0.125f);
                PutVector(simple.data()+16,float(int(fixture%11)-5),float(int((fixture/4)%9)-4),float(int(fixture%5)-2));
                PutFloat(simple.data()+28,float(int((fixture/8)%64)-32)*0.125f);
                const float rotation=float(int((fixture/8)%16)-8)*0.25f;
                const float cosine=std::cos(rotation),sine=std::sin(rotation);
                PutVector(matrix.data()+16,cosine,sine,0);
                PutVector(matrix.data()+32,-sine,cosine,0);
                PutVector(matrix.data()+48,0,0,1);
                PutVector(matrix.data()+64,float(int(fixture%11)-5),float(int(fixture%9)-4),float(int(fixture%7)-3));
                std::memset(matrix.data()+16+0x40,0,5); // no attached matrix; nunca ejecutar destructor
                StaticRef<uintptr_t>(0xB72920)=uintptr_t(working.data()+16);
                const uint32 initialFlags=0x20000u|(fixture&3);
                std::memcpy(working.data()+16+0x0C,&initialFlags,4);
                // Todos los ocho vértices de sprite tienen campos float coherentes.
                auto* vertices=reinterpret_cast<uint8*>(0xC80468);
                for(unsigned i=0;i<8;++i) {
                    PutFloat(vertices+i*28,20.0f+i*15);PutFloat(vertices+i*28+4,30.0f+i*9);
                    PutFloat(vertices+i*28+8,0.5f);PutFloat(vertices+i*28+12,1.0f);
                    PutFloat(vertices+i*28+20,float(int(fixture/256)-2)+i*0.125f);
                    PutFloat(vertices+i*28+24,float(int(fixture/256)-1)-i*0.25f);
                }
                std::array<uint32,4> args{};unsigned arity=0;
                const bool alias=f==1 ? (fixture&1)!=0 : (fixture/128)&1;
                if(f==0) {
                    args={std::bit_cast<uint32>(dimensions[fixture%16]),std::bit_cast<uint32>(dimensions[(fixture/16)%16]),0,0};arity=2;
                } else if(f==1) {
                    if(alias)out=in;
                    args={uint32(uintptr_t(out)),uint32(uintptr_t(simple.data()+16)),uint32(uintptr_t(in)),0};arity=3;
                } else {
                    if(alias)out=in;
                    args={uint32(uintptr_t(out)),uint32(counts[fixture%8]),uint32(uintptr_t(matrix.data()+16)),uint32(uintptr_t(in))};arity=4;
                }
                const auto beforeInput=input,beforeOutput=output;
                const auto beforeSimple=simple;
                const auto beforeMatrix=matrix;
                const auto beforeWorking=working;
                const auto beforeSprite=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[0].first),globals[0].second);
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                bool routeOK=false;
                ABIResult abi{};
                routes[f]->Invoke(candidate,[&] {
                    routeOK=ReversingProbe::JumpsToDll(fns[f].address)==candidate;
                    abi=CallRaw(fns[f].address,args,arity);return true;
                });
                const auto afterSprite=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[0].first),globals[0].second);
                bool protectedOK=simple==beforeSimple&&matrix==beforeMatrix;
                if(f==0) {
                    auto maskedBefore=beforeSprite,maskedAfter=afterSprite;
                    for(unsigned i=0;i<4;++i)for(unsigned off=0;off<8;++off) {
                        const unsigned at=0x20+i*28+20+off;maskedBefore[at]=maskedAfter[at]=0;
                    }
                    protectedOK&=maskedBefore==maskedAfter&&input==beforeInput&&output==beforeOutput&&working==beforeWorking;
                } else {
                    protectedOK&=beforeSprite==afterSprite;
                    const unsigned num=f==1 ? 1u : unsigned(counts[fixture%8]);
                    auto expectedInput=beforeInput,expectedOutput=beforeOutput;
                    auto maskedInput=input,maskedOutput=output;
                    for(unsigned off=0;off<num*12;++off) {
                        if(alias)maskedInput[16+off]=expectedInput[16+off]=0;
                        else maskedOutput[16+off]=expectedOutput[16+off]=0;
                    }
                    protectedOK&=expectedInput==maskedInput&&expectedOutput==maskedOutput;
                    if(f==1)protectedOK&=working==beforeWorking;
                    else protectedOK&=std::equal(working.begin(),working.begin()+16,beforeWorking.begin())
                        &&std::equal(working.end()-16,working.end(),beforeWorking.end()-16);
                }
                json result{{"route_ok",routeOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                            {"protected_state_unchanged",protectedOK},{"alias",alias},{"input",input},{"output",output},
                            {"simple",simple},{"matrix",matrix},{"working",working},{"sprite_state_guards",afterSprite}};
                restore();return result;
            };
            const auto original=run(false),candidate=run(true);
            const bool stateOK=restored(),equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted;passed&=accepted;allRestored&=stateOK;
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",1024},{"failures",failures}});
    }
    restore();allRestored&=restored();for(auto* route:routes)allRestored&=route->Restore();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
