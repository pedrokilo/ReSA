#pragma once
// Banco dentro del motor (BORRADOR SIN EJECUTAR): CVehicle::SetTransmissionRotation (EXE 0x6DBBB0, thiscall, ret 0x1C).
// El original es el codigo del EXE (hook RH apagado) y el candidato la funcion de la DLL (hook activo), llamados por la MISMA
// entrada con los mismos argumentos y marcos identicos. CMatrix::SetRotateX (0x59B060) y CMatrix::RotateY (0x59B2C0) se sustituyen en las DOS entradas
// (EXE y DLL) por el mismo Fake, que anota los bits del angulo: asi se compara en bits el angulo que calcula cada lado (fpatan sin redondear,
// -atan, PI, |x|+|x|) con estres de NaN con distinta carga/signo, sNaN, +-inf, +-0, subnormales, +-1e30/1e-30 y FLT_MAX, sin depender de CMatrix.
// El resto (constructor de CMatrix, suma de la posicion, UpdateRW, destructor) lo ejecuta cada lado con el suyo y se compara el marco entero.
#include <fstream>
#include <float.h>
#include <bit>
#include "Entity/Vehicle/Vehicle.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace CppVehicleTransmission1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{
    {"CVehicle","SetTransmissionRotation",0x6DBBB0,0,-1,1024},
}};

inline json trace;
inline void __fastcall FakeSetRotateX(CMatrix*, void*, float angle) { trace.push_back(json::array({"SetRotateX",std::bit_cast<uint32>(angle)})); }
inline void __fastcall FakeRotateY(CMatrix*, void*, float angle)    { trace.push_back(json::array({"RotateY",std::bit_cast<uint32>(angle)})); }

constexpr unsigned kFrame = 16 + 0x90 + 16;                 // RwFrame (modelling en +0x10) con guardas
alignas(16) inline std::array<uint8,kFrame> frame{};
alignas(16) inline std::array<uint8,64> vehicleDummy{};     // la funcion no lee this

struct ABIResult { bool stack, registers; };
// thiscall con siete argumentos de pila (ret 0x1C la limpia el llamado): args = {marco, L, R, wx, wy, wz, esFrontal}.
inline ABIResult CallThis(uintptr_t callAddress, uint32 self, const std::array<uint32,7>& args) {
    const auto* argPointer = args.data();
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov eax,callAddress
        mov edx,argPointer
        push dword ptr [edx + 24]
        push dword ptr [edx + 20]
        push dword ptr [edx + 16]
        push dword ptr [edx + 12]
        push dword ptr [edx + 8]
        push dword ptr [edx + 4]
        push dword ptr [edx]
        mov ecx,self
        call eax
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter, ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter};
}
inline void PutFloat(uint8* p,float value) { std::memcpy(p,&value,4); }
inline void PutU32(uint8* p,uint32 value) { std::memcpy(p,&value,4); }
inline std::string Hex(const uint8* p,size_t n) {
    static const char* d="0123456789abcdef"; std::string s; s.reserve(n*2);
    for(size_t i=0;i<n;++i){s.push_back(d[p[i]>>4]);s.push_back(d[p[i]&15]);} return s;
}
struct Rng {
    uint32 s;
    explicit Rng(uint32 seed):s(seed*2654435761u+0x9E3779B9u){ if(!s)s=1; for(int i=0;i<4;++i)next(); }
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float value(float scale){ return (float(int32(next()>>8)-(1<<23))/float(1<<23))*scale; }
};
// Valores de estres (bits): NaN con distinta carga y signo, sNaN, +-inf, +-0, subnormales, FLT_MAX, +-1e30, +-1e-30.
inline constexpr std::array<uint32,22> stress{
    0x7FC00000u,0xFFC00000u,0x7FC12345u,0xFFC0ABCDu,0x7FA00001u,0xFFA55555u,0x7F800000u,0xFF800000u,0x00000000u,0x80000000u,0x00000001u,
    0x80000001u,0x007FFFFFu,0x807FFFFFu,0x7F7FFFFFu,0xFF7FFFFFu,0x7149F2CAu,0xF149F2CAu,0x0DA24260u,0x8DA24260u,0x3F800000u,0xBF800000u};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppvehicletransmission1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppvehicletransmission1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppvehicletransmission1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    OriginalDependencies499::HookRoute route("CVehicle",fns[0].name,fns[0].address,&CVehicle::SetTransmissionRotation);
    const bool registered=route.Registered();
    const uintptr_t setRotateXDll=std::bit_cast<uintptr_t>(&CMatrix::SetRotateX), rotateYDll=std::bit_cast<uintptr_t>(&CMatrix::RotateY);
    const auto savedFrame=frame;
    const unsigned savedFP=_controlfp(0,0);
    auto restore=[&] { frame=savedFrame; _controlfp(savedFP,_MCW_PC|_MCW_RC); };
    Write({{"event","begin"},{"functions",1},{"pairs",1024},{"registered_hook",registered},{"collaborators_stubbed",true},
           {"stubbed",{"CMatrix::SetRotateX","CMatrix::RotateY"}},{"fp_control",savedFP}});
    if(!registered) { Write({{"event","error"},{"reason","SetTransmissionRotation no esta registrada"}}); return; }

    unsigned pair=0; bool passed=true, allRestored=true; int failures=0;
    const std::string name=std::string(fns[0].cls)+"::"+fns[0].name;
    for(unsigned fixture=0;fixture<1024;++fixture,++pair) {
        Rng rng(fixture+1);
        const float s=(fixture%8==7)?100.0f:3.0f;
        const unsigned profile=(fixture/4)%8;               // cuantas entradas se sustituyen por valores de estres: 0 ninguna .. 7 casi todas
        const unsigned mode=fixture%4;                      // 0: sin estres; 1: un valor; 2: dos; 3: perfil
        std::array<float,6> in{rng.value(s),rng.value(s),rng.value(s),rng.value(s),rng.value(s),rng.value(s)}; // L, R, wx, wy, wz, posY del marco
        if(fixture%16==5) in[0]=in[1]; if(fixture%16==6) in[3]=0.0f;  // L == R (y = 0) y wheelPos.y = 0 (x = posY - wy)
        unsigned count = mode==0 ? 0 : mode==1 ? 1 : mode==2 ? 2 : profile;
        for(unsigned k=0;k<count;++k) {
            const unsigned slot=rng.next()%6u;
            in[slot]=std::bit_cast<float>(stress[rng.next()%stress.size()]);
        }
        const bool isFront=(fixture/32)%2==1;
        auto build=[&] {
            for(unsigned k=0;k<kFrame;++k) frame[k]=uint8(0xC0u^(k*37u+fixture*11u));
            Rng r2(fixture+0x1000u);
            auto* m=frame.data()+16+0x10;                    // RwMatrix: right(+0) up(+16) at(+32) pos(+48), cada RwV3d con 4 bytes de relleno
            const float rot=r2.value(3.0f);
            const float c=std::cos(rot), sn=std::sin(rot);
            PutFloat(m+0,c);  PutFloat(m+4,sn);  PutFloat(m+8,0.0f);
            PutFloat(m+16,-sn); PutFloat(m+20,c); PutFloat(m+24,0.0f);
            PutFloat(m+32,0.0f); PutFloat(m+36,0.0f); PutFloat(m+40,1.0f);
            PutFloat(m+48,r2.value(s)); PutFloat(m+52,in[5]); PutFloat(m+56,r2.value(s));
        };
        auto run=[&](bool candidate) -> json {
            restore();
            build();
            trace=json::array();
            ReversingProbe::EntryPatch p1(0x59B060,FakeSetRotateX), p2(setRotateXDll,FakeSetRotateX), p3(0x59B2C0,FakeRotateY), p4(rotateYDll,FakeRotateY);
            const auto beforeFrame=frame;
            std::array<uint32,7> args{uint32(uintptr_t(frame.data()+16)),std::bit_cast<uint32>(in[0]),std::bit_cast<uint32>(in[1]),
                                      std::bit_cast<uint32>(in[2]),std::bit_cast<uint32>(in[3]),std::bit_cast<uint32>(in[4]),uint32(isFront)};
            _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
            ABIResult abi{};
            route.Invoke(candidate,[&]{ abi=CallThis(fns[0].address,uint32(uintptr_t(vehicleDummy.data())),args); return 0; });
            const unsigned fpAfter=_controlfp(0,0);
            _controlfp(savedFP,_MCW_PC|_MCW_RC);
            bool guards=true;                                 // las guardas del marco no cambian
            for(unsigned k=0;k<16;++k) guards&=frame[k]==beforeFrame[k]&&frame[kFrame-16+k]==beforeFrame[kFrame-16+k];
            const bool fpOK=(fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
            json result{{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},{"guards_unchanged",guards},{"fp_control_kept",fpOK},
                        {"trace",trace},{"frame",Hex(frame.data(),frame.size())}};
            restore();
            return result;
        };
        const auto original=run(false), candidate=run(true);
        bool stateOK=frame==savedFrame && _controlfp(0,0)==savedFP;
        const bool equal=original==candidate;
        const bool accepted=equal&&stateOK&&original["stack_restored"]==true&&original["nonvolatile_registers_preserved"]==true
            &&original["guards_unchanged"]==true&&original["fp_control_kept"]==true&&original["trace"].size()==2;
        failures+=!accepted; passed&=accepted; allRestored&=stateOK;
        Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},
               {"fixture",{{"mode",mode},{"profile",profile},{"is_front",isFront},{"inputs",{std::bit_cast<uint32>(in[0]),std::bit_cast<uint32>(in[1]),
                   std::bit_cast<uint32>(in[2]),std::bit_cast<uint32>(in[3]),std::bit_cast<uint32>(in[4]),std::bit_cast<uint32>(in[5])}}}},
               {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
    }
    Write({{"event","function_summary"},{"function",name},{"cases",1024},{"failures",failures}});
    restore(); allRestored&=route.Restore();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
