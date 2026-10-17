#pragma once
// Banco dentro del motor: ayudantes x87 con que Bmx.cpp sustituye su ensamblador (X87Math.cpp: TrigSimpleSinCpp/CosCpp/SinMulCpp, TrigAtan2Cpp,
// PowScaleFloatCpp, BmxSinScaledCpp, BmxCosScaledCpp, BmxSinTimes2Cpp, BmxChassisZCpp, BmxLeanTermCpp, BmxLeanTermAddCpp, BmxNegAsinCpp).
// El "original" es la secuencia EXACTA de instrucciones del EXE de CBmx (adaptador de ensamblador DENTRO del banco; _CIpow 0x822130 y
// _CIasin 0x821E70 se llaman por su direccion en el EXE) y el candidato es el ayudante de la DLL llamado directamente. Sin hooks ni colaboradores simulados.
// Complementa a reversing/Misc61b (pares de motor con fixtures uniformes): aqui NaN con distinta carga/signo, sNaN, +-inf, +-0, subnormales, +-1e30/1e-30, FLT_MAX.
#include <fstream>
#include <float.h>
#include <bit>
#include "Entity/Vehicle/Bmx.h"
#include "X87Math.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppBmxX87Stress1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,12> fns{{
    {"Bmx","Sin",0,0,-1,2048},          {"Bmx","Cos",0,0,-1,2048},
    {"Bmx","SinScaled",0,0,-1,2048},    {"Bmx","CosScaled",0,0,-1,2048},
    {"Bmx","SinMul",0,0,-1,2048},       {"Bmx","SinTimes2",0,0,-1,2048},
    {"Bmx","ChassisZ",0,0,-1,2048},     {"Bmx","LeanTerm",0,0,-1,2048},
    {"Bmx","LeanTermAdd",0,0,-1,2048},  {"Bmx","Atan2",0,0,-1,2048},
    {"Bmx","PowScale",0,0,-1,2048},     {"Bmx","NegAsin",0,0,-1,2048},
}};

inline std::string Hex(uint32 v) { static const char* d="0123456789abcdef"; std::string s(8,'0'); for(int i=0;i<8;++i) s[7-i]=d[(v>>(4*i))&15]; return s; }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
struct Rng {
    uint32 s;
    explicit Rng(uint32 seed):s(seed*2654435761u+0x9E3779B9u){ if(!s)s=1; for(int i=0;i<4;++i)next(); }
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float lo,float hi){ return lo+(hi-lo)*(float(next()>>8)/float(1<<24)); }
};
// Estres: ceros, subnormales, extremos, infinitos, NaN (silenciosos y senalizantes, cargas y signos distintos), constantes del EXE y vecinos de +-1.
inline const std::array<uint32,44> kPatterns{
    0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x00800000u,0x80800000u,
    0x7F7FFFFFu,0xFF7FFFFFu,0x7F800000u,0xFF800000u,0x7FC00000u,0xFFC00000u,0x7FC12345u,0xFFC0ABCDu,0x7FA00001u,0xFFA55555u,
    0x7F800001u,0x3F000000u,0xBF000000u,0x3F3504F3u,0x40490FDBu,0xC0490FDBu,0x3FC90FDBu,0x40C90FDBu,0x4B800000u,0x5F000000u,
    0xDF000000u,0x1E3CE508u,0x7149F2CAu,0x0DA24260u,0xF149F2CAu,0x3F800001u,0x3F7FFFFFu,0xBF800001u,0xBF7FFFFFu,0x3C23D70Au,
    0x3E99999Au,0x3F4CCCCDu,0x3CA3D70Au,0x3C8EFA35u};
enum Kind { Angle, Unit, Pos, Unit01, Free, NegSmall };
// Argumento j del fixture: 3 de cada 8 fixtures solo patrones; el resto uniforme del dominio con ~25% de patrones.
inline float Gen(Kind kind, uint32 fixture, uint32 j) {
    Rng r(fixture*16u+j+1u);
    const bool onlyPatterns = (fixture%8u)<3u;
    if(onlyPatterns || r.next()%4u==0u) return std::bit_cast<float>(kPatterns[r.next()%kPatterns.size()]);
    switch(kind) {
        case Angle: return r.uni(-4.f,4.f);
        case Unit: return r.uni(-1.2f,1.2f);
        case Pos: return r.uni(0.1f,3.f);
        case Unit01: return r.uni(0.f,1.f);
        case NegSmall: return r.uni(-1.2f,-0.2f);
        default: return r.uni(-100.f,100.f);
    }
}

// --- Secuencias exactas del EXE (adaptadores de ensamblador) ---
inline bool gStack{true}, gRegs{true};
// Cada adaptador mide ESP y los registros no volatiles antes y despues (los llamados _CIpow/_CIasin del EXE no deben tocarlos).
#define BMX_BEGIN uintptr_t sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{}; \
    __asm { mov sp0,esp } __asm { mov b0,ebx } __asm { mov s0,esi } __asm { mov d0,edi } __asm { mov p0,ebp }
#define BMX_END __asm { mov sp1,esp } __asm { mov b1,ebx } __asm { mov s1,esi } __asm { mov d1,edi } __asm { mov p1,ebp } \
    gStack&=(sp0==sp1); gRegs&=(b0==b1&&s0==s1&&d0==d1&&p0==p1);
__declspec(noinline) inline float ExeSin(float x){ float r; BMX_BEGIN __asm { fld x } __asm { fsin } __asm { fstp r } BMX_END return r; }
__declspec(noinline) inline float ExeCos(float x){ float r; BMX_BEGIN __asm { fld x } __asm { fcos } __asm { fstp r } BMX_END return r; }
// 0x6C0E0A..0x6C0E34: fld angulo; fmul 0x8595EC; fsin / fcos
__declspec(noinline) inline float ExeSinScaled(float x,float k){ float r; BMX_BEGIN __asm { fld x } __asm { fmul k } __asm { fsin } __asm { fstp r } BMX_END return r; }
__declspec(noinline) inline float ExeCosScaled(float x,float k){ float r; BMX_BEGIN __asm { fld x } __asm { fmul k } __asm { fcos } __asm { fstp r } BMX_END return r; }
// 0x6C10B7..0x6C10D0: fld; fsin; fmul k; fstp
__declspec(noinline) inline float ExeSinMul(float x,float k){ float r; BMX_BEGIN __asm { fld x } __asm { fsin } __asm { fmul k } __asm { fstp r } BMX_END return r; }
// 0x6C0A21..0x6C0A31: fsin; fmul minZ; fmul 0.8f
__declspec(noinline) inline float ExeSinTimes2(float x,float k1,float k2){ float r; BMX_BEGIN __asm { fld x } __asm { fsin } __asm { fmul k1 } __asm { fmul k2 } __asm { fstp r } BMX_END return r; }
// 0x6C1251..0x6C128C
__declspec(noinline) inline float ExeChassisZ(float lean,float minZ,float zOff){
    float r; const float nine=std::bit_cast<float>(0x3F666666u);
    BMX_BEGIN __asm { fld lean } __asm { fcos } __asm { fld1 } __asm { fsub st,st(1) } __asm { fmul minZ } __asm { fmul nine } __asm { fadd zOff } __asm { fstp r } __asm { fstp st(0) } BMX_END return r; }
// 0x6BFA9C..0x6BFAB8 y 0x6BFB0B..0x6BFB2F
__declspec(noinline) inline float ExeLeanTerm(float cur,float total,float start,float blend,float mult){
    float r; const float twoPi=std::bit_cast<float>(0x40C90FDBu);
    BMX_BEGIN __asm { fld cur } __asm { fdiv total } __asm { fmul twoPi } __asm { fadd start } __asm { fsin } __asm { fmul blend } __asm { fmul mult } __asm { fstp r } BMX_END return r; }
__declspec(noinline) inline float ExeLeanTermAdd(float cur,float total,float start,float blend,float mult,float addend){
    float r; const float twoPi=std::bit_cast<float>(0x40C90FDBu);
    BMX_BEGIN __asm { fld cur } __asm { fdiv total } __asm { fmul twoPi } __asm { fadd start } __asm { fsin } __asm { fmul blend } __asm { fmul mult } __asm { fadd addend } __asm { fstp r } BMX_END return r; }
// 0x6C1235: fpatan(ST1 = y, ST0 = x)
__declspec(noinline) inline float ExeAtan2(float y,float x){ float r; BMX_BEGIN __asm { fld y } __asm { fld x } __asm { fpatan } __asm { fstp r } BMX_END return r; }
// 0x6C0016..0x6C002D: fld 0.97f; fld paso; call 0x822130 (_CIpow); fmul crank; fstp
__declspec(noinline) inline float ExePowScale(float base,float step,float x){
    float r; BMX_BEGIN __asm { fld base } __asm { fld step } __asm { mov eax,0x822130 } __asm { call eax } __asm { fmul x } __asm { fstp r } BMX_END return r; }
// 0x6C0FC6..0x6C0FDA: call 0x821E70 (_CIasin); fmul [0x858C1C] (-1.0f); fstp
__declspec(noinline) inline float ExeNegAsin(float x){
    float r; const float minusOne=-1.0f;
    BMX_BEGIN __asm { fld x } __asm { mov eax,0x821E70 } __asm { call eax } __asm { fmul minusOne } __asm { fstp r } BMX_END return r; }

using ErrnoFn = int*(__cdecl*)();

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppbmxx87stress1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppbmxx87stress1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppbmxx87stress1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    // Direcciones de la DLL (llamada directa) y del EXE vigiladas
    const std::array<uintptr_t,12> dllEntries{
        std::bit_cast<uintptr_t>(&TrigSimpleSinCpp), std::bit_cast<uintptr_t>(&TrigSimpleCosCpp), std::bit_cast<uintptr_t>(&BmxSinScaledCpp),
        std::bit_cast<uintptr_t>(&BmxCosScaledCpp), std::bit_cast<uintptr_t>(&TrigSimpleSinMulCpp), std::bit_cast<uintptr_t>(&BmxSinTimes2Cpp),
        std::bit_cast<uintptr_t>(&BmxChassisZCpp), std::bit_cast<uintptr_t>(&BmxLeanTermCpp), std::bit_cast<uintptr_t>(&BmxLeanTermAddCpp),
        std::bit_cast<uintptr_t>(&TrigAtan2Cpp), std::bit_cast<uintptr_t>(&PowScaleFloatCpp), std::bit_cast<uintptr_t>(&BmxNegAsinCpp)};
    const std::array<uintptr_t,3> exeWatched{0x822130,0x821E70,0x82B181};
    const bool exeUnhooked = !ReversingProbe::JumpsToDll(0x822130) && !ReversingProbe::JumpsToDll(0x821E70);
    bool dllOutside = true; for(auto a:dllEntries) dllOutside &= a>=0x1000000;
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for(auto at:exeWatched) entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    for(auto at:dllEntries) entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    const unsigned savedFP=_controlfp(0,0);
    int* const exeErrno=reinterpret_cast<ErrnoFn>(0x82B181)();
    const int savedErrno=*exeErrno;
    auto restore=[&]{ _clearfp(); _controlfp(savedFP,_MCW_PC|_MCW_RC); *exeErrno=savedErrno; };
    auto restored=[&]{
        bool ok = _controlfp(0,0)==savedFP && *exeErrno==savedErrno;
        for(auto& [at,bytes]:entries) ok &= bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    Write({{"event","begin"},{"functions",int(fns.size())},{"pairs",int(fns.size())*2048},{"registered_hook",false},{"direct_dll_call",dllOutside},
           {"exe_unhooked",exeUnhooked},{"collaborators_stubbed",false},{"fp_control",savedFP}});
    if(!(dllOutside&&exeUnhooked)) {
        restore(); const bool stateOK=restored();
        Write({{"event","error"},{"reason","ayudantes de la DLL no directos o EXE hookeado"},{"state_restored",stateOK}});
        return;
    }

    unsigned pair=0; bool passed=true, allRestored=true;
    for(size_t f=0;f<fns.size();++f) {
        int failures=0;
        const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<2048;++fixture,++pair) {
            std::array<float,6> a{};
            switch(f) {
                case 0: case 1: a[0]=Gen(Angle,fixture,0); break;
                case 2: case 3: a[0]=Gen(Free,fixture,0); a[1]=(fixture%4==0)?std::bit_cast<float>(0x3C8EFA35u):Gen(Free,fixture,1); break;
                case 4: a[0]=Gen(Angle,fixture,0); a[1]=(fixture%4==0)?std::bit_cast<float>(fixture%8==0?0x3CA3D70Au:0x3D23D70Au):Gen(Free,fixture,1); break;
                case 5: a[0]=Gen(Angle,fixture,0); a[1]=Gen(NegSmall,fixture,1); a[2]=(fixture%4==0)?std::bit_cast<float>(0x3F4CCCCDu):Gen(Free,fixture,2); break;
                case 6: a[0]=Gen(Angle,fixture,0); a[1]=Gen(NegSmall,fixture,1); a[2]=Gen(Free,fixture,2); break;
                case 7: case 8:
                    a[0]=Gen(Pos,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=(fixture%4==0)?std::bit_cast<float>(0x3FC90FDBu):Gen(Angle,fixture,2);
                    a[3]=Gen(Unit01,fixture,3); a[4]=(fixture%4==0)?std::bit_cast<float>(fixture%8==0?0x3E99999Au:0x3DB22D0Eu):Gen(Free,fixture,4); a[5]=Gen(Angle,fixture,5); break;
                case 9: a[0]=Gen(Free,fixture,0); a[1]=Gen(Free,fixture,1); break;
                case 10: a[0]=(fixture%4==0)?std::bit_cast<float>(0x3F7851ECu):Gen(Unit01,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=Gen(Free,fixture,2); break;
                default: a[0]=Gen(Unit,fixture,0); break;
            }
            auto run=[&](bool candidate) -> json {
                restore();
                gStack=true; gRegs=true;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                float r=0.f;
                if(candidate) {
                    switch(f) {
                        case 0: r=TrigSimpleSinCpp(a[0]); break;
                        case 1: r=TrigSimpleCosCpp(a[0]); break;
                        case 2: r=BmxSinScaledCpp(a[0],a[1]); break;
                        case 3: r=BmxCosScaledCpp(a[0],a[1]); break;
                        case 4: r=TrigSimpleSinMulCpp(a[0],a[1]); break;
                        case 5: r=BmxSinTimes2Cpp(a[0],a[1],a[2]); break;
                        case 6: r=BmxChassisZCpp(a[0],a[1],a[2]); break;
                        case 7: r=BmxLeanTermCpp(a[0],a[1],a[2],a[3],a[4]); break;
                        case 8: r=BmxLeanTermAddCpp(a[0],a[1],a[2],a[3],a[4],a[5]); break;
                        case 9: r=TrigAtan2Cpp(a[0],a[1]); break;
                        case 10: { float v=a[2]; PowScaleFloatCpp(&v,a[0],a[1]); r=v; break; }
                        default: r=BmxNegAsinCpp(a[0]); break;
                    }
                } else {
                    switch(f) {
                        case 0: r=ExeSin(a[0]); break;
                        case 1: r=ExeCos(a[0]); break;
                        case 2: r=ExeSinScaled(a[0],a[1]); break;
                        case 3: r=ExeCosScaled(a[0],a[1]); break;
                        case 4: r=ExeSinMul(a[0],a[1]); break;
                        case 5: r=ExeSinTimes2(a[0],a[1],a[2]); break;
                        case 6: r=ExeChassisZ(a[0],a[1],a[2]); break;
                        case 7: r=ExeLeanTerm(a[0],a[1],a[2],a[3],a[4]); break;
                        case 8: r=ExeLeanTermAdd(a[0],a[1],a[2],a[3],a[4],a[5]); break;
                        case 9: r=ExeAtan2(a[0],a[1]); break;
                        case 10: r=ExePowScale(a[0],a[1],a[2]); break;
                        default: r=ExeNegAsin(a[0]); break;
                    }
                }
                const unsigned fpAfter=_controlfp(0,0);
                const bool stackOK=gStack, regsOK=gRegs;
                restore();
                const bool protectedOK=(fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                const bool routeOK = candidate ? dllEntries[f]>=0x1000000 : true;
                return json{{"route_ok",routeOK},{"stack_restored",stackOK},{"nonvolatile_registers_preserved",regsOK},
                            {"protected_state_unchanged",protectedOK},{"bits",Hex(Bits(r))}};
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            json args=json::array(); for(float v:a) args.push_back(Hex(Bits(v)));
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},{"args",args},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",2048},{"failures",failures}});
    }
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
