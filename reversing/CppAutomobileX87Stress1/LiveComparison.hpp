#pragma once
// Banco dentro del motor: ayudantes x87 con que Automobile.cpp sustituye su ensamblador (X87Math.cpp: AutomobileTimeMsPlusFloatCpp, AutomobileWheelSpinAudioCpp,
// AutomobileSteerFactorCpp, AutomobileHydraulicForceCpp, AutomobileTireDecayCpp, AutomobileHydraulicStickCpp, AutomobileBonnetAngVelCpp, AutomobileCarRollDblCpp,
// PlaceableRollDblCpp [GetCarPitch], CnvDecayPowCpp [G4PowToFloat] y PowScaleFloatCpp [G3MouseSteerDecay]).
// El "original" es la secuencia EXACTA de instrucciones del EXE (adaptador de ensamblador DENTRO del banco; _CIpow 0x822130, _CIasin 0x821E70 y _ftol2 0x821B40 se
// llaman por su direccion en el EXE) y el candidato es el ayudante de la DLL llamado directamente. Sin hooks ni colaboradores simulados.
// Complementa a reversing/AutomobileFinal4 (pares de motor con dominio finito y razonable): aqui NaN con distinta carga/signo, sNaN, +-inf, +-0, subnormales,
// +-1e30/1e-30, FLT_MAX, enteros de 32 bits con y sin bit alto. GetCarRoll/GetCarPitch comparan los 80 bits de ST0 (fstp tbyte) porque el EXE devuelve ST0 sin redondear.
#include <fstream>
#include <float.h>
#include <bit>
#include "Entity/Vehicle/Automobile.h"
#include "X87Math.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppAutomobileX87Stress1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,11> fns{{
    {"Automobile","TimeMs",0,0,-1,2048},        {"Automobile","WheelSpin",0,0,-1,2048},
    {"Automobile","SteerFactor",0,0,-1,2048},   {"Automobile","HydraulicForce",0,0,-1,2048},
    {"Automobile","TireDecay",0,0,-1,2048},     {"Automobile","HydraulicStick",0,0,-1,2048},
    {"Automobile","BonnetAngVel",0,0,-1,2048},  {"Automobile","CarRoll",0,0,-1,2048},
    {"Automobile","CarPitch",0,0,-1,2048},      {"Automobile","PowToFloat",0,0,-1,2048},
    {"Automobile","MouseSteer",0,0,-1,2048},
}};

inline std::string Hex(uint32 v) { static const char* d="0123456789abcdef"; std::string s(8,'0'); for(int i=0;i<8;++i) s[7-i]=d[(v>>(4*i))&15]; return s; }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline float FromBits(uint32 v) { return std::bit_cast<float>(v); }
struct Rng {
    uint32 s;
    explicit Rng(uint32 seed):s(seed*2654435761u+0x9E3779B9u){ if(!s)s=1; for(int i=0;i<4;++i)next(); }
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float lo,float hi){ return lo+(hi-lo)*(float(next()>>8)/float(1<<24)); }
};
// Estres: ceros, subnormales, extremos, infinitos, NaN (silenciosos y senalizantes, cargas y signos distintos), constantes del EXE y vecinos de +-1.
inline const std::array<uint32,48> kPatterns{
    0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x00800000u,0x80800000u,
    0x7F7FFFFFu,0xFF7FFFFFu,0x7F800000u,0xFF800000u,0x7FC00000u,0xFFC00000u,0x7FC12345u,0xFFC0ABCDu,0x7FA00001u,0xFFA55555u,
    0x7F800001u,0x3F000000u,0xBF000000u,0x3F3504F3u,0x40490FDBu,0xC0490FDBu,0x3FC90FDBu,0x40C90FDBu,0x4B800000u,0x5F000000u,
    0xDF000000u,0x1E3CE508u,0x7149F2CAu,0x0DA24260u,0xF149F2CAu,0x3F800001u,0x3F7FFFFFu,0xBF800001u,0xBF7FFFFFu,0x3C23D70Au,
    0x3E99999Au,0x3F4CCCCDu,0x3CA3D70Au,0x3C8EFA35u,0x461C4000u,0x457A0000u,0x33800000u,0x2B8CBCCCu};
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
// Valor entero de 32 bits (tiempo en ms): patrones de borde (con y sin bit alto) o uniforme.
inline uint32 GenTime(uint32 fixture) {
    static const std::array<uint32,12> edge{0u,1u,0x7FFFFFFFu,0x80000000u,0x80000001u,0xFFFFFFFFu,0xFFFFFFF0u,0x01000000u,0x00FFFFFFu,0x7FFFFF80u,0x00001000u,0xDEADBEEFu};
    Rng r(fixture*16u+1u);
    if((fixture%8u)<3u) return edge[r.next()%edge.size()];
    return r.next();
}

// --- Secuencias exactas del EXE (adaptadores de ensamblador) ---
inline bool gStack{true}, gRegs{true};
// Cada adaptador mide ESP y los registros no volatiles antes y despues (los llamados _CIpow/_CIasin/_ftol2 del EXE no deben tocarlos).
#define AUT_BEGIN uintptr_t sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{}; \
    __asm { mov sp0,esp } __asm { mov b0,ebx } __asm { mov s0,esi } __asm { mov d0,edi } __asm { mov p0,ebp }
#define AUT_END __asm { mov sp1,esp } __asm { mov b1,ebx } __asm { mov s1,esi } __asm { mov d1,edi } __asm { mov p1,ebp } \
    gStack&=(sp0==sp1); gRegs&=(b0==b1&&s0==s1&&d0==d1&&p0==p1);
using Words = std::array<uint32,4>;
// 0x6A884D..0x6A886E: fild tiempo; si es negativo fadd 2^32 (0x858C54); fadd addend; call _ftol2 (0x821B40); se usa EAX
__declspec(noinline) inline Words ExeTimeMs(uint32 ms,float addend){
    int32 t=int32(ms); const float wrap=4294967296.0f; uint32 r;
    AUT_BEGIN
    __asm { fild dword ptr t }
    __asm { cmp t,0 }
    __asm { jge no_wrap }
    __asm { fadd wrap }
    __asm { no_wrap: }
    __asm { fadd addend }
    __asm { mov eax,0x821B40 }
    __asm { call eax }
    __asm { mov r,eax }
    AUT_END
    return Words{r,0,0,0}; }
// 0x6B2DCD..0x6B2DEE
__declspec(noinline) inline Words ExeWheelSpin(float base,float ts,float audio,float rate){
    float r; AUT_BEGIN __asm { fld base } __asm { fld ts } __asm { mov eax,0x822130 } __asm { call eax } __asm { fld1 } __asm { fsub st,st(1) } __asm { fmul rate }
    __asm { fxch st(1) } __asm { fmul audio } __asm { faddp st(1),st } __asm { fstp r } AUT_END return Words{Bits(r),0,0,0}; }
// 0x6B2989..0x6B2A34: _CIasin; fld steerLock; fmul degToRad; fdivp; si forceOne o 1.0 < ST0 (fld 1.0; fcomp st(1); test ah,5; jp) ST0 = 1.0; fmul steer; fstp
__declspec(noinline) inline Words ExeSteer(float x,float steerLock,float deg,float steer,int forceOne){
    float r; const float one=1.0f;
    AUT_BEGIN
    __asm { fld x } __asm { mov eax,0x821E70 } __asm { call eax } __asm { fld steerLock } __asm { fmul deg } __asm { fdivp st(1),st }
    __asm { cmp forceOne,0 }
    __asm { jne set_one }
    __asm { fld one } __asm { fcomp st(1) } __asm { fnstsw ax } __asm { test ah,5 } __asm { jp keep }
    __asm { set_one: }
    __asm { fstp st(0) } __asm { fld one }
    __asm { keep: }
    __asm { fmul steer } __asm { fstp r }
    AUT_END return Words{Bits(r),0,0,0}; }
// 0x6B30E0..0x6B3182
__declspec(noinline) inline Words ExeHydForce(float tsf,float ts,float base,float factor,float ux,float uy,float uz,float mass){
    float f0,f1,f2,s38,s40,z4c; const float minusOne=-1.0f;
    AUT_BEGIN
    __asm { fld base } __asm { fld ts } __asm { mov eax,0x822130 } __asm { call eax }
    __asm { fld tsf } __asm { fmul tsf } __asm { fmul factor } __asm { fld1 } __asm { faddp st(1),st } __asm { fdivp st(1),st }
    __asm { fld ts } __asm { mov eax,0x822130 } __asm { call eax } __asm { fmul tsf } __asm { fsub tsf }
    __asm { fld ux } __asm { fmul minusOne } __asm { fld uy } __asm { fmul minusOne } __asm { fld uz } __asm { fmul minusOne } __asm { fstp z4c }
    __asm { fld st(2) } __asm { fmul st(0),st(2) } __asm { fstp s38 }
    __asm { fld st(2) } __asm { fmul st(0),st(1) } __asm { fstp st(2) } __asm { fstp st(0) }
    __asm { fxch st(1) } __asm { fmul z4c } __asm { fstp s40 }
    __asm { fld s38 } __asm { fmul mass } __asm { fstp f0 }
    __asm { fmul mass } __asm { fstp f1 }
    __asm { fld s40 } __asm { fmul mass } __asm { fstp f2 }
    AUT_END return Words{Bits(f0),Bits(f1),Bits(f2),0}; }
// 0x6A5CE7..0x6A5D0F
__declspec(noinline) inline Words ExeTire(float base,float ts,float temp){
    float r; const float one=1.0f;
    AUT_BEGIN __asm { fld base } __asm { fld ts } __asm { mov eax,0x822130 } __asm { call eax } __asm { fmul temp } __asm { fadd one } __asm { fstp r } AUT_END return Words{Bits(r),0,0,0}; }
// 0x6A0E9D..0x6A0EBF
__declspec(noinline) inline Words ExeStick(float lr,float ud,float jump,float qpi){
    float rc,rs; AUT_BEGIN
    __asm { fld jump } __asm { fld lr } __asm { fld ud } __asm { fpatan } __asm { fadd qpi } __asm { fld st(0) } __asm { fcos } __asm { fmul st(0),st(2) } __asm { fstp rc }
    __asm { fsin } __asm { fmul st(0),st(1) } __asm { fstp rs } __asm { fstp st(0) }
    AUT_END return Words{Bits(rc),Bits(rs),0,0}; }
// 0x6A9F12..0x6A9F51
__declspec(noinline) inline Words ExeBonnet(float angle,float bias,float mulK,float fx,float fy,float fz,float vx,float vy,float vz,float angVel){
    float r; AUT_BEGIN
    __asm { fld angle } __asm { fsin } __asm { fadd bias } __asm { fmul mulK }
    __asm { fld st(0) } __asm { fmul fz } __asm { fld st(1) } __asm { fmul fy } __asm { fxch st(2) } __asm { fmul fx } __asm { fxch st(2) }
    __asm { fmul vy } __asm { fxch st(1) } __asm { fmul vz } __asm { faddp st(1),st } __asm { fxch st(1) } __asm { fmul vx } __asm { faddp st(1),st }
    __asm { fadd angVel } __asm { fstp r }
    AUT_END return Words{Bits(r),0,0,0}; }
// 0x6A6010 y 0x6A6050: ST0 final almacenado con fstp tbyte (80 bits)
inline Words Tbyte(const uint8* b){ Words w{}; std::memcpy(w.data(),b,10); return w; }
__declspec(noinline) inline Words ExeRoll(const float* v,float upZ){
    const float rx=v[0],ry=v[1],rz=v[2]; const float zero=0.0f,minusOne=-1.0f,toDegrees=57.2957763671875f; uint8 outBuf[16]{};
    AUT_BEGIN
    __asm { fld ry } __asm { fld rx } __asm { fld st(0) } __asm { fmul st(0),st(1) } __asm { fld st(2) } __asm { fmul st(0),st(3) } __asm { faddp st(1),st(0) }
    __asm { fsqrt } __asm { fstp st(2) } __asm { fstp st(0) } __asm { fld upZ } __asm { fcomp zero } __asm { fnstsw ax } __asm { test ah,5 } __asm { jp keep_r }
    __asm { fmul minusOne }
    __asm { keep_r: }
    __asm { fld rz } __asm { fxch st(1) } __asm { fpatan } __asm { fmul toDegrees } __asm { fstp tbyte ptr outBuf }
    AUT_END return Tbyte(outBuf); }
__declspec(noinline) inline Words ExePitch(const float* v,float upZ){
    const float fx=v[0],fy=v[1],fz=v[2]; const float zero=0.0f,minusOne=-1.0f; uint8 outBuf[16]{};
    AUT_BEGIN
    __asm { fld fy } __asm { fld fx } __asm { fld st(0) } __asm { fmul st(0),st(1) } __asm { fld st(2) } __asm { fmul st(0),st(3) } __asm { faddp st(1),st(0) }
    __asm { fsqrt } __asm { fstp st(2) } __asm { fstp st(0) } __asm { fld upZ } __asm { fcomp zero } __asm { fnstsw ax } __asm { test ah,5 } __asm { jp keep_p }
    __asm { fmul minusOne }
    __asm { keep_p: }
    __asm { fld fz } __asm { fxch st(1) } __asm { fpatan } __asm { fstp tbyte ptr outBuf }
    AUT_END return Tbyte(outBuf); }
// Candidatos de roll/pitch: llamada directa al ayudante de la DLL (devuelve ST0), que se guarda con fstp tbyte para comparar los 80 bits
__declspec(noinline) inline Words CandRoll(const float* v,float upZ){
    uint8 outBuf[16]{}; const uintptr_t fn=std::bit_cast<uintptr_t>(&AutomobileCarRollDblCpp);
    AUT_BEGIN __asm { push dword ptr upZ } __asm { push dword ptr v } __asm { mov eax,fn } __asm { call eax } __asm { add esp,8 } __asm { fstp tbyte ptr outBuf } AUT_END
    return Tbyte(outBuf); }
__declspec(noinline) inline Words CandPitch(const float* v,float upZ){
    uint8 outBuf[16]{}; const uintptr_t fn=std::bit_cast<uintptr_t>(&PlaceableRollDblCpp);
    AUT_BEGIN __asm { push dword ptr upZ } __asm { push dword ptr v } __asm { mov eax,fn } __asm { call eax } __asm { add esp,8 } __asm { fstp tbyte ptr outBuf } AUT_END
    return Tbyte(outBuf); }
// 0x6A8D3F..0x6A8D54: fld base; fld exp; call _CIpow; fstp
__declspec(noinline) inline Words ExePowToFloat(float base,float e){
    float r; AUT_BEGIN __asm { fld base } __asm { fld e } __asm { mov eax,0x822130 } __asm { call eax } __asm { fstp r } AUT_END return Words{Bits(r),0,0,0}; }
// 0x6AD8D1..0x6AD8E8: fld 0.975f; fld ts; call _CIpow; fmul raw; fstp
__declspec(noinline) inline Words ExeMouse(float base,float ts,float raw){
    float r; AUT_BEGIN __asm { fld base } __asm { fld ts } __asm { mov eax,0x822130 } __asm { call eax } __asm { fmul raw } __asm { fstp r } AUT_END return Words{Bits(r),0,0,0}; }

using ErrnoFn = int*(__cdecl*)();

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppautomobilex87stress1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppautomobilex87stress1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppautomobilex87stress1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    // Direcciones de la DLL (llamada directa) y del EXE vigiladas
    const std::array<uintptr_t,11> dllEntries{
        std::bit_cast<uintptr_t>(&AutomobileTimeMsPlusFloatCpp), std::bit_cast<uintptr_t>(&AutomobileWheelSpinAudioCpp), std::bit_cast<uintptr_t>(&AutomobileSteerFactorCpp),
        std::bit_cast<uintptr_t>(&AutomobileHydraulicForceCpp), std::bit_cast<uintptr_t>(&AutomobileTireDecayCpp), std::bit_cast<uintptr_t>(&AutomobileHydraulicStickCpp),
        std::bit_cast<uintptr_t>(&AutomobileBonnetAngVelCpp), std::bit_cast<uintptr_t>(&AutomobileCarRollDblCpp), std::bit_cast<uintptr_t>(&PlaceableRollDblCpp),
        std::bit_cast<uintptr_t>(&CnvDecayPowCpp), std::bit_cast<uintptr_t>(&PowScaleFloatCpp)};
    const std::array<uintptr_t,4> exeWatched{0x822130,0x821E70,0x821B40,0x82B181};
    const bool exeUnhooked = !ReversingProbe::JumpsToDll(0x822130) && !ReversingProbe::JumpsToDll(0x821E70) && !ReversingProbe::JumpsToDll(0x821B40);
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
            std::array<float,12> a{};
            uint32 timeBits{}; // el tiempo no pasa por float: un sNaN cambiaria de bits al cargarse en la x87
            const bool known=(fixture%4u)!=3u; // 3 de cada 4 fixtures usan las constantes reales del EXE
            switch(f) {
                case 0: timeBits=GenTime(fixture); a[1]=(fixture%4u==0u)?FromBits(0x461C4000u):(fixture%4u==1u)?FromBits(0x457A0000u):Gen(Free,fixture,1); break;
                case 1: a[0]=Gen(Unit01,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=Gen(Free,fixture,2); a[3]=Gen(Free,fixture,3); break;
                case 2: a[0]=Gen(Unit01,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=known?FromBits(0x3C8EFA35u):Gen(Pos,fixture,2); a[3]=Gen(Unit,fixture,3); a[4]=(fixture%3u==0u)?1.f:0.f; break;
                case 3: a[0]=Gen(Free,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=known?FromBits(0x3F7C28F6u):Gen(Unit01,fixture,2); a[3]=known?5.0f:Gen(Pos,fixture,3);
                        a[4]=Gen(Unit,fixture,4); a[5]=Gen(Unit,fixture,5); a[6]=Gen(Unit,fixture,6); a[7]=Gen(Pos,fixture,7); break;
                case 4: a[0]=known?FromBits(0x3F7EB852u):Gen(Unit01,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=Gen(Free,fixture,2); break;
                case 5: a[0]=Gen(Free,fixture,0); a[1]=Gen(Free,fixture,1); a[2]=Gen(Unit01,fixture,2); a[3]=known?FromBits(0xBF490FDBu):Gen(Angle,fixture,3); break;
                case 6: a[0]=Gen(Angle,fixture,0); a[1]=known?0.1f:Gen(Free,fixture,1); a[2]=known?0.05f:Gen(Free,fixture,2); a[3]=Gen(Unit,fixture,3); a[4]=Gen(Unit,fixture,4);
                        a[5]=Gen(Unit,fixture,5); a[6]=Gen(Free,fixture,6); a[7]=Gen(Free,fixture,7); a[8]=Gen(Free,fixture,8); a[9]=Gen(Free,fixture,9); break;
                case 7: case 8: a[0]=Gen(Unit,fixture,0); a[1]=Gen(Unit,fixture,1); a[2]=Gen(Unit,fixture,2); a[3]=Gen(Unit,fixture,3); break;
                case 9: a[0]=Gen(Unit01,fixture,0); a[1]=Gen(Pos,fixture,1); break;
                default: a[0]=Gen(Free,fixture,0); a[1]=Gen(Pos,fixture,1); a[2]=known?FromBits(0x3F79999Au):Gen(Unit01,fixture,2); break;
            }
            auto run=[&](bool candidate) -> json {
                restore();
                gStack=true; gRegs=true;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                Words r{};
                if(candidate) {
                    switch(f) {
                        case 0: r=Words{AutomobileTimeMsPlusFloatCpp(timeBits,a[1]),0,0,0}; break;
                        case 1: r=Words{Bits(AutomobileWheelSpinAudioCpp(a[0],a[1],a[2],a[3])),0,0,0}; break;
                        case 2: r=Words{Bits(AutomobileSteerFactorCpp(a[0],a[1],a[2],a[3],a[4]!=0.f)),0,0,0}; break;
                        case 3: { const float up[3]{a[4],a[5],a[6]}; float force[3]{}; AutomobileHydraulicForceCpp(a[0],a[1],a[2],a[3],up,a[7],force);
                                  r=Words{Bits(force[0]),Bits(force[1]),Bits(force[2]),0}; break; }
                        case 4: r=Words{Bits(AutomobileTireDecayCpp(a[0],a[1],a[2])),0,0,0}; break;
                        case 5: { float c=0.f,s=0.f; AutomobileHydraulicStickCpp(a[0],a[1],a[2],a[3],&c,&s); r=Words{Bits(c),Bits(s),0,0}; break; }
                        case 6: { const float fwd[3]{a[3],a[4],a[5]}, mv[3]{a[6],a[7],a[8]}; r=Words{Bits(AutomobileBonnetAngVelCpp(a[0],a[1],a[2],fwd,mv,a[9])),0,0,0}; break; }
                        case 7: { const float v[3]{a[0],a[1],a[2]}; r=CandRoll(v,a[3]); break; }
                        case 8: { const float v[3]{a[0],a[1],a[2]}; r=CandPitch(v,a[3]); break; }
                        case 9: r=Words{Bits(CnvDecayPowCpp(a[0],a[1])),0,0,0}; break;
                        default: { float v=a[0]; PowScaleFloatCpp(&v,a[2],a[1]); r=Words{Bits(v),0,0,0}; break; }
                    }
                } else {
                    switch(f) {
                        case 0: r=ExeTimeMs(timeBits,a[1]); break;
                        case 1: r=ExeWheelSpin(a[0],a[1],a[2],a[3]); break;
                        case 2: r=ExeSteer(a[0],a[1],a[2],a[3],a[4]!=0.f ? 1 : 0); break;
                        case 3: r=ExeHydForce(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]); break;
                        case 4: r=ExeTire(a[0],a[1],a[2]); break;
                        case 5: r=ExeStick(a[0],a[1],a[2],a[3]); break;
                        case 6: r=ExeBonnet(a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7],a[8],a[9]); break;
                        case 7: { const float v[3]{a[0],a[1],a[2]}; r=ExeRoll(v,a[3]); break; }
                        case 8: { const float v[3]{a[0],a[1],a[2]}; r=ExePitch(v,a[3]); break; }
                        case 9: r=ExePowToFloat(a[0],a[1]); break;
                        default: r=ExeMouse(a[2],a[1],a[0]); break;
                    }
                }
                const unsigned fpAfter=_controlfp(0,0);
                const bool stackOK=gStack, regsOK=gRegs;
                restore();
                const bool protectedOK=(fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                const bool routeOK = candidate ? dllEntries[f]>=0x1000000 : true;
                return json{{"route_ok",routeOK},{"stack_restored",stackOK},{"nonvolatile_registers_preserved",regsOK},
                            {"protected_state_unchanged",protectedOK},
                            {"bits",Hex(r[0])+Hex(r[1])+Hex(r[2])+Hex(r[3])}};
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            json args=json::array(); args.push_back(Hex(timeBits)); for(float v:a) args.push_back(Hex(Bits(v)));
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},{"args",args},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",2048},{"failures",failures}});
    }
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
