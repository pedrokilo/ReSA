#pragma once
#include <fstream>
#include <float.h>
#include "Quaternion.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesMathLeaf3Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot,cases; };
inline const std::array<Fn,2> fns{{
    {"CQuaternion","Set-matrix",0x59C3E0,0,-1,1024},
    {"CQuaternion","Set-axis-angle",0x59C600,0,-1,1024},
}};
alignas(16) inline std::array<uint8,96> input{};
alignas(16) inline std::array<uint8,48> output{};
struct ABIResult { bool stack,registers; };
inline ABIResult CallRaw(uintptr_t callAddress,void* self,const std::array<uint32,2>& args,unsigned arity) {
    const auto* argPointer=args.data();
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
        mov ecx,self
        call eax
        // Original ret4/ret8 and reconstructed thiscall both pop their arguments.
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter,ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter};
}
inline void Put(uint8* p,float value) { std::memcpy(p,&value,4); }
inline float Get(const uint8* p) { float value;std::memcpy(&value,p,4);return value; }
inline std::array<float,3> Axis(unsigned i) {
    const std::array<std::array<float,3>,16> axes{{
        {1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1},
        {1,1,0},{1,-1,0},{1,0,1},{1,0,-1},{0,1,1},{0,1,-1},
        {1,1,1},{-1,1,1},{1,-1,1},{1,1,-1}
    }};
    auto axis=axes[i];const float length=std::sqrt(axis[0]*axis[0]+axis[1]*axis[1]+axis[2]*axis[2]);
    for(auto& value:axis)value/=length;
    return axis;
}
inline unsigned MatrixFixture(uint8* matrix,unsigned profile,unsigned variant) {
    constexpr float pi=3.14159265358979323846f;
    const unsigned axisIndex=profile<5 ? 4 : profile<9 ? 0 : profile<13 ? 2 : profile==13 ? 12 : profile==14 ? 6 : 8;
    const unsigned step=profile<5 ? profile : profile<9 ? profile-4 : profile<13 ? profile-8 : 4;
    const auto axis=Axis(axisIndex);const float angle=step*(pi/4),c=std::cos(angle),s=std::sin(angle),t=1-c;
    const float x=axis[0],y=axis[1],z=axis[2];
    // Row-vector RW matrix, with coherent rotations and nonzero off-diagonals.
    const std::array<float,9> elements{t*x*x+c,t*x*y+s*z,t*x*z-s*y,
        t*x*y-s*z,t*y*y+c,t*y*z+s*x,t*x*z+s*y,t*y*z-s*x,t*z*z+c};
    for(unsigned row=0;row<3;++row)for(unsigned col=0;col<3;++col)Put(matrix+row*16+col*4,elements[row*3+col]);
    Put(matrix+48,float(int(variant)-8));Put(matrix+52,float(variant)*0.25f);Put(matrix+56,-float(variant)*2);
    const float xx=elements[0],yy=elements[4],zz=elements[8];
    if(yy+xx+zz>=0)return 0;
    if(xx-yy-zz>=0)return 1;
    if(yy-xx-zz>=0)return 2;
    return 3;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesmathleaf3-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"dependenciesmathleaf3-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("dependenciesmathleaf3");Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    using MatrixMember=void(CQuaternion::*)(const RwMatrix&);
    using AxisMember=void(CQuaternion::*)(RwV3d*,float);
    const auto matrixMember=static_cast<MatrixMember>(&CQuaternion::Set);
    const auto axisMember=static_cast<AxisMember>(&CQuaternion::Set);
    const std::array<uintptr_t,2> symbols{reinterpret_cast<uintptr_t>(FunctionToVoidPtr(matrixMember)),reinterpret_cast<uintptr_t>(FunctionToVoidPtr(axisMember))};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> finalEntries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f]})
        finalEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute matrixRoute("CQuaternion","Set-matrix",0x59C3E0,FunctionToVoidPtr(matrixMember));
    OriginalDependencies499::HookRoute axisRoute("CQuaternion","Set-axis-angle",0x59C600,FunctionToVoidPtr(axisMember));
    std::array<OriginalDependencies499::HookRoute*,2> routes{&matrixRoute,&axisRoute};
    const auto savedInput=input;const auto savedOutput=output;const unsigned savedFP=_controlfp(0,0);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(size_t f=0;f<fns.size();++f)for(auto at:{fns[f].address,symbols[f],routes[f]->Body()})
        entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    auto restore=[&] { input=savedInput;output=savedOutput;_controlfp(savedFP,_MCW_PC|_MCW_RC); };
    auto restored=[&] {
        bool ok=input==savedInput&&output==savedOutput&&_controlfp(0,0)==savedFP;
        for(auto& [at,bytes]:entries)ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    const bool allRegistered=matrixRoute.Registered()&&axisRoute.Registered();
    Write({{"event","begin"},{"functions",2},{"pairs",2048},{"registered_hook",allRegistered},{"collaborators_stubbed",false}});
    if(!allRegistered) {
        restore();bool stateOK=restored();for(auto* route:routes)stateOK&=route->Restore();
        Write({{"event","error"},{"reason","Aplicar propuesta y registrar los dos hooks selectivos"},{"state_restored",stateOK}});return;
    }
    constexpr float pi=3.14159265358979323846f;
    const std::array<float,16> angles{0.0f,-0.0f,1e-8f,-1e-8f,.25f,-.25f,pi/2,-pi/2,pi,-pi,
        2*pi,-2*pi,3*pi,-3*pi,std::nextafter(pi,0.0f),std::nextafter(pi,4.0f)};
    unsigned pair=0;bool passed=true,allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0;const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<1024;++fixture,++pair) {
            auto run=[&](bool candidate) {
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                for(unsigned i=0;i<input.size();++i)input[i]=uint8(fixture*29+i*11);
                for(unsigned i=0;i<output.size();++i)output[i]=uint8(fixture*7+i*19);
                auto* source=input.data()+16;auto* target=output.data()+16;
                const unsigned profile=fixture%16,variant=(fixture/16)%16,layout=fixture/256;
                unsigned branch=0;
                if(f==0)branch=MatrixFixture(source,profile,variant);
                else { const auto axis=Axis(profile);for(unsigned i=0;i<3;++i)Put(source+i*4,axis[i]); }
                if(layout)target=source+(layout-1)*(f==0 ? 16 : 4);
                const auto beforeInput=input;const auto beforeOutput=output;
                const std::array<uint32,2> args{uint32(uintptr_t(source)),std::bit_cast<uint32>(angles[variant])};
                bool routeOK=false;ABIResult abi{};
                routes[f]->Invoke(candidate,[&] {
                    routeOK=ReversingProbe::JumpsToDll(fns[f].address)==candidate;
                    abi=CallRaw(fns[f].address,target,args,f==0 ? 1u:2u);return true;
                });
                auto expectedInput=beforeInput,maskedInput=input;
                auto expectedOutput=beforeOutput,maskedOutput=output;
                for(unsigned i=0;i<16;++i) {
                    if(layout)expectedInput[unsigned(target-input.data())+i]=maskedInput[unsigned(target-input.data())+i]=0;
                    else expectedOutput[16+i]=maskedOutput[16+i]=0;
                }
                bool finite=true;for(unsigned i=0;i<4;++i)finite&=std::isfinite(Get(target+i*4));
                const bool protectedOK=expectedInput==maskedInput&&expectedOutput==maskedOutput;
                json result{{"route_ok",routeOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                    {"protected_state_unchanged",protectedOK},{"finite_output",finite},{"layout",layout},{"matrix_branch",branch},
                    {"input",input},{"output",output}};
                restore();return result;
            };
            const auto original=run(false),candidate=run(true);
            const bool stateOK=restored(),equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true&&original["finite_output"]==true;
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
