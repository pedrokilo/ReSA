#pragma once
// Banco dentro del motor: ayudantes x87 con que Plane.cpp sustituye su ensamblador (X87Math.cpp: Plane...Cpp, mas los ya existentes que reutiliza).
// El "original" es la secuencia EXACTA de instrucciones del EXE de CPlane (adaptador de ensamblador DENTRO del banco; _CIpow 0x822130, _CIasin 0x821E70 y
// _ftol2 0x821B40 se llaman por su direccion en el EXE) y el candidato es el ayudante de la DLL llamado directamente. Sin hooks ni colaboradores simulados
// (la "funcion de roll" es una rutina local del banco que deja el fpatan en ST0 sin redondear, como CPlaceable::GetRoll del EXE).
// Complementa a reversing/Plane*355 (pares de motor con fixtures finitos): aqui NaN con distinta carga/signo, sNaN, +-inf, +-0, subnormales, +-1e30/1e-30, FLT_MAX.
// Nota: los adaptadores solo usan EAX/ECX/EDX; ESP y EBX/ESI/EDI/EBP se miden antes y despues de cada secuencia.
#include <fstream>
#include <float.h>
#include <bit>
#include "Plane.h"
#include "X87Math.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppPlaneX87Stress1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
constexpr int kCases = 1024;
inline const std::array<Fn, 46> fns{{
    {"Plane","MassThreshold",0,0,-1,kCases},       {"Plane","HealthAfterDamage",0,0,-1,kCases},    {"Plane","SelectCloser",0,0,-1,kCases},
    {"Plane","ControlBlend",0,0,-1,kCases},        {"Plane","ForwardSpeed",0,0,-1,kCases},         {"Plane","InputSkid",0,0,-1,kCases},
    {"Plane","GroundSteer",0,0,-1,kCases},         {"Plane","NozzleChange",0,0,-1,kCases},         {"Plane","GearAdvance",0,0,-1,kCases},
    {"Plane","SpeedBelow",0,0,-1,kCases},          {"Plane","SpeedCap",0,0,-1,kCases},             {"Plane","GearResistance",0,0,-1,kCases},
    {"Plane","WaveAdvance",0,0,-1,kCases},         {"Plane","DamageSpeed",0,0,-1,kCases},          {"Plane","DamageNoise",0,0,-1,kCases},
    {"Plane","PropellerWave",0,0,-1,kCases},       {"Plane","FlightDecay",0,0,-1,kCases},          {"Plane","FlightProductAdd",0,0,-1,kCases},
    {"Plane","FloatBlend",0,0,-1,kCases},          {"Plane","DistanceBelow",0,0,-1,kCases},        {"Plane","RollCompare",0,0,-1,kCases},
    {"Plane","CeilingTarget",0,0,-1,kCases},       {"Plane","CeilingAcceleration",0,0,-1,kCases}, {"Plane","FlightBound",0,0,-1,kCases},
    {"Plane","SmokeParticleVelocity",0,0,-1,kCases},{"Plane","VelocityNoise",0,0,-1,kCases},       {"Plane","PositionNoise",0,0,-1,kCases},
    {"Plane","PropellerTarget",0,0,-1,kCases},     {"Plane","SpeedSquaredAbove",0,0,-1,kCases},    {"Plane","WheelCompression",0,0,-1,kCases},
    {"Plane","WheelHeight",0,0,-1,kCases},         {"Plane","AndromadaWheelRotation",0,0,-1,kCases},{"Plane","PropAngle",0,0,-1,kCases},
    {"Plane","GearFactors",0,0,-1,kCases},         {"Plane","SurfaceAngle",0,0,-1,kCases},         {"Plane","SubtractPosition",0,0,-1,kCases},
    {"Plane","FinOffset",0,0,-1,kCases},           {"Plane","PropRotation",0,0,-1,kCases},         {"Plane","GearRotation",0,0,-1,kCases},
    {"Plane","NozzleAngle",0,0,-1,kCases},         {"Plane","DustTime",0,0,-1,kCases},             {"Plane","SumVector3",0,0,-1,kCases},
    {"Plane","StreakAlpha",0,0,-1,kCases},         {"Plane","SmokeVelocity",0,0,-1,kCases},       {"Plane","Negate",0,0,-1,kCases},
    {"Plane","HoverTilt",0,0,-1,kCases},
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
enum Kind { Angle, Unit, Pos, Unit01, Free, Small, Safe };
// Argumento j del fixture: 3 de cada 8 fixtures solo patrones; el resto uniforme del dominio con ~25% de patrones. `Safe` excluye inf y magnitudes
// que harian infinito el bucle de PropAngle (el EXE tampoco termina con ellas): solo ceros, NaN y valores moderados.
inline float Gen(Kind kind, uint32 fixture, uint32 j) {
    Rng r(fixture*32u+j+1u);
    const bool onlyPatterns = (fixture%8u)<3u;
    if(kind==Safe) {
        if(r.next()%6u==0u) { static const uint32 safe[]{0x00000000u,0x80000000u,0x7FC00000u,0xFFC12345u,0x7FA00001u,0x00000001u,0x3F800000u,0xBF800000u}; return std::bit_cast<float>(safe[r.next()%8u]); }
        return r.uni(-50.f,50.f);
    }
    if(onlyPatterns || r.next()%4u==0u) return std::bit_cast<float>(kPatterns[r.next()%kPatterns.size()]);
    switch(kind) {
        case Angle: return r.uni(-4.f,4.f);
        case Unit: return r.uni(-1.2f,1.2f);
        case Pos: return r.uni(0.1f,3.f);
        case Unit01: return r.uni(0.f,1.f);
        case Small: return r.uni(-3.f,3.f);
        default: return r.uni(-100.f,100.f);
    }
}
inline int32 GenInt(uint32 fixture, uint32 j, int32 lo, int32 hi) {
    Rng r(fixture*32u+j+777u);
    switch(r.next()%8u) { case 0: return lo; case 1: return hi; case 2: return 0<lo?lo:(0>hi?hi:0); default: return lo+int32(r.next()%uint32(hi-lo+1)); }
}
inline uint32 GenUnsigned(uint32 fixture, uint32 j) {
    Rng r(fixture*32u+j+999u);
    static const uint32 edge[]{0u,1u,0x7FFFFFFFu,0x80000000u,0x80000001u,0xFFFFFFFFu,0x01000001u,0xFFFFFF00u};
    return r.next()%3u==0u ? edge[r.next()%8u] : r.next();
}

struct Args { float f[32]; int32 i[4]; uint32 u[2]; double d[1]; };
struct Out { uint32 w[8]; int n; };
inline void PutF(Out& o, float v) { o.w[o.n++] = Bits(v); }
inline void PutU(Out& o, uint32 v) { o.w[o.n++] = v; }

// --- Secuencias exactas del EXE (adaptadores de ensamblador) ---
inline bool gStack{true}, gRegs{true};
#define PL_BEGIN uintptr_t sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{}; \
    __asm { mov sp0,esp } __asm { mov b0,ebx } __asm { mov s0,esi } __asm { mov d0,edi } __asm { mov p0,ebp }
#define PL_END __asm { mov sp1,esp } __asm { mov b1,ebx } __asm { mov s1,esi } __asm { mov d1,edi } __asm { mov p1,ebp } \
    gStack&=(sp0==sp1); gRegs&=(b0==b1&&s0==s1&&d0==d1&&p0==p1);

// CPlane::VehicleDamage 0x6CC575..0x6CC5B8 (umbral por masa) y 0x6CC642..0x6CC6CE (vida)
__declspec(noinline) inline void ExeMassThreshold(const Args& a, Out& o){
    float r; const float mass=a.f[0], scale=a.f[1], threshold=a.f[2], one=1.f; const int minimumOne=a.i[0];
    PL_BEGIN
    __asm {
        fld mass
        fdiv scale
        cmp minimumOne,0
        je mt_multiply
        fld one
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne mt_multiply
        fstp st(0)
        fld one
    mt_multiply:
        fmul threshold
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeHealthAfterDamage(const Args& a, Out& o){
    float r; const float health=a.f[0], damage=a.f[1], factor=a.f[2];
    PL_BEGIN __asm { fld damage } __asm { fmul factor } __asm { fsubr health } __asm { fstp r } PL_END PutF(o,r); }
// 0x6CC7E3..0x6CC839 (delta + CVector::SquaredMagnitude 0x406DA0 + comparacion con closest²)
__declspec(noinline) inline void ExeSelectCloser(const Args& a, Out& o){
    float closest=a.f[6]; const float n0=a.f[0], n1=a.f[1], n2=a.f[2], r0=a.f[3], r1=a.f[4], r2=a.f[5]; float v[3]; uint16 status;
    PL_BEGIN
    __asm {
        fld n0
        fsub r0
        fstp v[0]
        fld n1
        fsub r1
        fstp v[4]
        fld n2
        fsub r2
        fstp v[8]
        lea ecx,v
        fld dword ptr [ecx+8]
        fld dword ptr [ecx+4]
        fld dword ptr [ecx]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fld closest
        fmul closest
        fld st(1)
        fcompp
        fnstsw ax
        mov status,ax
        test ah,5
        jp sc_not_closer
        fsqrt
        fstp closest
        jmp sc_done
    sc_not_closer:
        fstp st(0)
    sc_done:
    }
    PL_END PutF(o,closest); PutU(o,(status&0x500)==0x100); }
// ProcessControlInputs 0x6CADD0
__declspec(noinline) inline void ExeControlBlend(const Args& a, Out& o){
    float r; const float oldValue=a.f[0], scale=a.f[1], step=a.f[2], factor=a.f[3]; const int input=a.i[0];
    PL_BEGIN __asm { fild input } __asm { fmul scale } __asm { fsub oldValue } __asm { fmul step } __asm { fmul factor } __asm { fadd oldValue } __asm { fstp r } PL_END PutF(o,r); }
__declspec(noinline) inline void ExeForwardSpeed(const Args& a, Out& o){
    float r; const float s[3]{a.f[0],a.f[1],a.f[2]}, f[3]{a.f[3],a.f[4],a.f[5]};
    PL_BEGIN
    __asm {
        lea eax,s
        lea edx,f
        fld dword ptr [eax+8]
        fmul dword ptr [edx+8]
        fld dword ptr [eax+4]
        fmul dword ptr [edx+4]
        faddp st(1),st
        fld dword ptr [eax]
        fmul dword ptr [edx]
        faddp st(1),st
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeInputSkid(const Args& a, Out& o){
    float r; const float oldValue=a.f[0], target=a.f[1], step=a.f[2], factor=a.f[3], lower=std::bit_cast<float>(0xBF800000u), upper=1.f;
    PL_BEGIN
    __asm {
        fld target
        fsub oldValue
        fmul step
        fmul factor
        fadd oldValue
        fcom lower
        fnstsw ax
        test ah,41h
        jne sk_lower
        fcom upper
        fnstsw ax
        test ah,5h
        jp sk_upper
        jmp sk_store
    sk_lower:
        fcom lower
        fnstsw ax
        test ah,41h
        je sk_store
        fstp st(0)
        fld lower
        jmp sk_store
    sk_upper:
        fstp st(0)
        fld upper
    sk_store:
        fstp r
    }
    PL_END PutF(o,r); }
// 0x6CB260..0x6CB2A1
__declspec(noinline) inline void ExeGroundSteer(const Args& a, Out& o){
    float r; const float speed=a.f[0], degrees=a.f[1], skid=a.f[2], maximum=a.f[3], radians=a.f[4], one=1.f;
    PL_BEGIN
    __asm {
        fld one
        fsub speed
        fld maximum
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne gs_angle
        fstp st(0)
        fld maximum
    gs_angle:
        fld degrees
        fmul radians
        fmul skid
        fmulp st(1),st
        fchs
        fstp r
    }
    PL_END PutF(o,r); }
// 0x6CB55C..0x6CB570 (_ftol2 del EXE en 0x821B40; el llamador toma el int16 bajo, aqui se compara el entero de 32 bits)
__declspec(noinline) inline void ExeNozzleChange(const Args& a, Out& o){
    int result; const float step=a.f[0], rate=a.f[1], scale=a.f[2]; const int axis=a.i[0];
    PL_BEGIN
    __asm {
        fild axis
        fmul step
        fmul rate
        fmul scale
        mov eax,0x821B40
        call eax
        mov result,eax
    }
    PL_END PutU(o,uint32(result)); }
__declspec(noinline) inline void ExeGearAdvance(const Args& a, Out& o){
    float gear=a.f[0]; const float step=a.f[1], factor=a.f[2], limit=a.f[3]; uint16 status;
    PL_BEGIN
    __asm {
        lea edx,gear
        fld step
        fmul factor
        fadd dword ptr [edx]
        fst dword ptr [edx]
        fcomp limit
        fnstsw ax
        mov status,ax
    }
    PL_END PutF(o,gear); PutU(o,!(status&0x100)); }
__declspec(noinline) inline void ExeSpeedBelow(const Args& a, Out& o){
    const float s[3]{a.f[0],a.f[1],a.f[2]}; const float limit=a.f[3]; uint16 status;
    PL_BEGIN
    __asm {
        lea edx,s
        fld dword ptr [edx+8]
        fld dword ptr [edx+4]
        fld dword ptr [edx]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fcomp limit
        fnstsw ax
        mov status,ax
    }
    PL_END PutU(o,(status&0x500)==0x100); }
// 0x6CB742..0x6CB7A1
__declspec(noinline) inline void ExeSpeedCap(const Args& a, Out& o){
    float s[3]{a.f[0],a.f[1],a.f[2]}; const float limit=a.f[3];
    PL_BEGIN
    __asm {
        lea eax,s
        fld dword ptr [eax+8]
        fld dword ptr [eax+4]
        fld dword ptr [eax]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fcom limit
        fnstsw ax
        test ah,41h
        jne cap_none
        fdivr limit
        lea eax,s
        fld st(0)
        fmul dword ptr [eax]
        fstp dword ptr [eax]
        fld st(0)
        fmul dword ptr [eax+4]
        fstp dword ptr [eax+4]
        fmul dword ptr [eax+8]
        fstp dword ptr [eax+8]
        jmp cap_done
    cap_none:
        fstp st(0)
    cap_done:
    }
    PL_END PutF(o,s[0]); PutF(o,s[1]); PutF(o,s[2]); }
__declspec(noinline) inline void ExeGearResistance(const Args& a, Out& o){
    float r; const float drag=a.f[0], divisor=a.f[1], factor=a.f[2];
    PL_BEGIN __asm { fld drag } __asm { fdiv divisor } __asm { fmul factor } __asm { fstp r } PL_END PutF(o,r); }
// ProcessFlyingCarStuff 0x6CB7C0
__declspec(noinline) inline void ExeWaveAdvance(const Args& a, Out& o){
    const float spread=a.f[0], one=1.f, step=a.f[1], randomScale=a.f[2], timeScale=a.f[3], countScale=a.f[4], unsignedAdjustment=4294967296.f; const double rangeBase=1.0; const int random=a.i[0];
    float upper, lower; uint32 result;
    PL_BEGIN
    __asm {
        fld spread
        fadd rangeBase
        fstp upper
        fld one
        fsub spread
        fstp lower
        fild random
        fmul randomScale
        fld upper
        fsub lower
        fmul st,st(1)
        fadd lower
        fld step
        fmul timeScale
        fmul countScale
        mov eax,0x821B40
        call eax
        mov result,eax
        fild result
        cmp result,0
        jge wv_unsigned
        fadd unsignedAdjustment
    wv_unsigned:
        fmulp st(1),st
        mov eax,0x821B40
        call eax
        fstp st(0)
        mov result,eax
    }
    PL_END PutU(o,result); }
__declspec(noinline) inline void ExeDamageSpeed(const Args& a, Out& o){
    float r; const float s[3]{a.f[0],a.f[1],a.f[2]}; const float scale=std::bit_cast<float>(0x40400000u), one=1.f;
    PL_BEGIN
    __asm {
        lea edx,s
        fld dword ptr [edx+8]
        fld dword ptr [edx+4]
        fld dword ptr [edx]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fmul scale
        fcom one
        fnstsw ax
        test ah,41h
        jne ds_store
        fstp st(0)
        fld one
    ds_store:
        fstp r
    }
    PL_END PutF(o,r); }
// 0x6CBAF4..0x6CBB3C: negativo = float(-amplitude) (fchs; fstp), luego el ruido
__declspec(noinline) inline void ExeDamageNoise(const Args& a, Out& o){
    float r; const float oldValue=a.f[0], amplitude=a.f[1], squaredDamage=a.f[2], step=a.f[3], speed=a.f[4], randomScale=a.f[5]; float negative; const int random=a.i[0];
    PL_BEGIN
    __asm {
        fld amplitude
        fchs
        fstp negative
        fild random
        fmul randomScale
        fld amplitude
        fsub negative
        fmulp st(1),st
        fadd negative
        fmul squaredDamage
        fmul step
        fmul speed
        fadd oldValue
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExePropellerWave(const Args& a, Out& o){
    float r; const float oldValue=a.f[0], upper=a.f[1], lower=a.f[2], step=a.f[3], randomScale=a.f[4], circle=a.f[5], amount=a.f[6], unsignedAdjustment=4294967296.f, one=1.f;
    const int random=a.i[0], damage=a.i[1]; const uint32 wave=a.u[0], period=a.u[1];
    PL_BEGIN
    __asm {
        fild random
        fmul randomScale
        fild damage
        fild wave
        cmp wave,0
        jge pw_unsigned_wave
        fadd unsignedAdjustment
    pw_unsigned_wave:
        fmul circle
        fild period
        cmp period,0
        jge pw_unsigned_period
        fadd unsignedAdjustment
    pw_unsigned_period:
        fdivp st(1),st
        fsin
        fsub one
        fld upper
        fsub lower
        fmul st,st(3)
        fadd lower
        fmulp st(1),st
        fmul st,st(1)
        fmul st,st(1)
        fmul step
        fmul amount
        fadd oldValue
        fstp r
        fstp st(0)
        fstp st(0)
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeFlightDecay(const Args& a, Out& o){
    float r, decay; const float scale=a.f[1], one=1.f, oldValue=a.f[0]; const int damage=a.i[0], storeFactor=a.i[1];
    PL_BEGIN
    __asm {
        fild damage
        fmul scale
        cmp storeFactor,0
        je fd_extended
        fstp decay
        fld decay
    fd_extended:
        fsubr one
        fmul oldValue
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeFlightProductAdd(const Args& a, Out& o){
    float r; const float value=a.f[0], x=a.f[1], y=a.f[2]; const int subtract=a.i[0];
    PL_BEGIN
    __asm {
        fld x
        fmul y
        cmp subtract,0
        je fp_add
        fsubr value
        jmp fp_store
    fp_add:
        fadd value
    fp_store:
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeFloatBlend(const Args& a, Out& o){
    float r; const float value=a.f[0], target=a.f[1], step=a.f[2], factor=a.f[3];
    PL_BEGIN __asm { fld target } __asm { fsub value } __asm { fmul step } __asm { fmul factor } __asm { fadd value } __asm { fstp r } PL_END PutF(o,r); }
__declspec(noinline) inline void ExeDistanceBelow(const Args& a, Out& o){
    const float p[3]{a.f[0],a.f[1],a.f[2]}, q[3]{a.f[3],a.f[4],a.f[5]}; const float threshold=a.f[6]; uint16 comparison;
    PL_BEGIN
    __asm {
        lea eax,p
        lea edx,q
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fcomp threshold
        fstp st(0)
        fnstsw ax
        fstp st(0)
        fstp st(0)
        mov comparison,ax
    }
    PL_END PutU(o,(comparison&0x500)==0x100); }
// Rutina local con la forma de CPlaceable::GetRoll del EXE (0x420B30): fpatan sin redondear en ST0 (ECX = datos {y, x}).
__declspec(naked) inline float __fastcall RollFn(const void* data, void*) {
    __asm {
        fld dword ptr [ecx+4]
        fld dword ptr [ecx]
        fpatan
        ret
    }
}
// 0x6CB92F..0x6CB962
__declspec(noinline) inline void ExeRollCompare(const Args& a, Out& o){
    const float data[2]{a.f[0],a.f[1]}; const float threshold=a.f[2]; const int greater=a.i[0]; uint16 comparison;
    PL_BEGIN
    __asm {
        lea ecx,data
        call RollFn
        fabs
        fcomp threshold
        fnstsw ax
        mov comparison,ax
    }
    PL_END PutU(o,greater ? !(comparison&0x4100) : (comparison&0x500)==0x100); }
// 0x6CC265..0x6CC28B (ST0 = altura devuelta por 0x6D2600)
__declspec(noinline) inline void ExeCeilingTarget(const Args& a, Out& o){
    float r; const float height=a.f[0], target=a.f[1], scale=a.f[2], zero=0.f;
    PL_BEGIN
    __asm {
        fld height
        fmul scale
        fld zero
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne ct_multiply
        fstp st(0)
        fld zero
    ct_multiply:
        fmul target
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeCeilingAcceleration(const Args& a, Out& o){
    float r; const float height=a.f[0], acceleration=a.f[1], adjust=a.f[2], lower=std::bit_cast<float>(0xBF800000u);
    PL_BEGIN
    __asm {
        fld height
        fmul adjust
        fsubr acceleration
        fld lower
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne ca_store
        fstp st(0)
        fld lower
    ca_store:
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeFlightBound(const Args& a, Out& o){
    float r; const float spread=a.f[0], one=1.f; const double extendedOne=1.0; const int subtract=a.i[0];
    PL_BEGIN
    __asm {
        cmp subtract,0
        jne fb_subtract
        fld spread
        fadd extendedOne
        jmp fb_store
    fb_subtract:
        fld one
        fsub spread
    fb_store:
        fstp r
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeSmokeParticleVelocity(const Args& a, Out& o){
    float vout[3]; float roundedZ; const float s[3]{a.f[0],a.f[1],a.f[2]}; const float quarter=a.f[3], multiplier=a.f[4];
    PL_BEGIN
    __asm {
        lea edx,s
        lea eax,vout
        fld dword ptr [edx]
        fmul quarter
        fmul multiplier
        fstp dword ptr [eax]
        fld dword ptr [edx+4]
        fmul quarter
        fmul multiplier
        fstp dword ptr [eax+4]
        fld dword ptr [edx+8]
        fmul quarter
        fstp roundedZ
        fld roundedZ
        fmul multiplier
        fstp dword ptr [eax+8]
    }
    PL_END PutF(o,vout[0]); PutF(o,vout[1]); PutF(o,vout[2]); }
__declspec(noinline) inline void ExeVelocityNoise(const Args& a, Out& o){
    float r; const float value=a.f[0], randomScale=a.f[1], spread=a.f[2], base=a.f[3]; const int random=a.i[0];
    PL_BEGIN __asm { fild random } __asm { fmul randomScale } __asm { fmul spread } __asm { fadd base } __asm { fmul value } __asm { fstp r } PL_END PutF(o,r); }
__declspec(noinline) inline void ExePositionNoise(const Args& a, Out& o){
    float r, negative; const float value=a.f[0], strength=a.f[1], randomScale=a.f[2]; const int random=a.i[0];
    PL_BEGIN
    __asm {
        fld strength
        fchs
        fstp negative
        fild random
        fmul randomScale
        fld strength
        fsub negative
        fmul st,st(1)
        fadd negative
        fadd value
        fstp r
        fstp st(0)
    }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExePropellerTarget(const Args& a, Out& o){
    const float acceleration=a.f[0], standard=a.f[1], maximum=a.f[2], minimum=a.f[3], zero=0.f; float result=standard;
    PL_BEGIN
    __asm {
        fld acceleration
        fcomp zero
        fnstsw ax
        test ah,41h
        jne pt_negative
        fld maximum
        fsub standard
        jmp pt_blend
    pt_negative:
        fld acceleration
        fcomp zero
        fnstsw ax
        test ah,5
        jp pt_done
        fld standard
        fsub minimum
    pt_blend:
        fmul acceleration
        fadd standard
        fstp result
    pt_done:
    }
    PL_END PutF(o,result); }
__declspec(noinline) inline void ExeSpeedSquaredAbove(const Args& a, Out& o){
    const float s[3]{a.f[0],a.f[1],a.f[2]}; const double threshold=a.d[0]; uint16 comparison;
    PL_BEGIN
    __asm {
        lea edx,s
        fld dword ptr [edx+8]
        fld dword ptr [edx+4]
        fld dword ptr [edx]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fcomp threshold
        fstp st(0)
        fnstsw ax
        fstp st(0)
        fstp st(0)
        mov comparison,ax
    }
    PL_END PutU(o,!(comparison&0x4100)); }

// CPlane::PreRender 0x6C94A0
// 0x6C9532..0x6C955F
__declspec(noinline) inline void ExeWheelCompression(const Args& a, Out& o){
    float r; const float spring=a.f[0], line=a.f[1], compression=a.f[2], one=1.f;
    PL_BEGIN
    __asm {
        fld spring
        fdiv line
        fsubr one
        fld compression
        fsub st,st(1)
        fld one
        fsub st,st(2)
        fdivp st(1),st
        fstp r
        fstp st(0)
    }
    PL_END PutF(o,r); }
// 0x6C9564..0x6C95B6 (altura de rueda)
__declspec(noinline) inline void ExeWheelHeight(const Args& a, Out& o){
    float height=a.f[4]; const float posZ=a.f[0], lowerLimit=a.f[1], compression=a.f[2], spring=a.f[3], zero=0.f, smooth=std::bit_cast<float>(0x3F400000u); const int snap=a.i[0];
    PL_BEGIN
    __asm {
        fld posZ
        fadd lowerLimit
        fld compression
        fcomp zero
        fnstsw ax
        test ah,41h
        jne wh_compare
        fld compression
        fmul spring
        fsubp st(1),st
    wh_compare:
        fcom height
        fnstsw ax
        test ah,41h
        je wh_store
        cmp snap,0
        jne wh_store
        fsub height
        fmul smooth
        fadd height
    wh_store:
        fstp height
    }
    PL_END PutF(o,height); }
// 0x6C95C4..0x6C9652 (Andromada): _CIasin del EXE por su direccion
__declspec(noinline) inline void ExeAndromadaWheelRotation(const Args& a, Out& o){
    float r, tmp; const float rotation=a.f[0], compression=a.f[1], n0=a.f[2], n1=a.f[3], n2=a.f[4], f0=a.f[5], f1=a.f[6], f2=a.f[7];
    const float one=1.f, minusOne=-1.f, k095=std::bit_cast<float>(0x3F733333u);
    PL_BEGIN
    __asm {
        fld compression
        fcomp one
        fnstsw ax
        test ah,5
        jp ar_scale
        fld f2
        fmul n2
        fld n0
        fmul f0
        faddp st(1),st
        fld f1
        fmul n1
        faddp st(1),st
        fld minusOne
        fcomp st(1)
        fnstsw ax
        test ah,41h
        je ar_minus
        fld one
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp ar_middle
        fstp st(0)
        mov tmp,0x3F800000
        fld tmp
        mov eax,0x821E70
        call eax
        fchs
        jmp ar_store
    ar_minus:
        fstp st(0)
        mov tmp,0xBF800000
        fld tmp
        mov eax,0x821E70
        call eax
        fchs
        jmp ar_store
    ar_middle:
        fstp tmp
        fld tmp
        mov eax,0x821E70
        call eax
        fchs
        jmp ar_store
    ar_scale:
        fld rotation
        fmul k095
    ar_store:
        fstp r
    }
    PL_END PutF(o,r); }
// 0x6C96A5..0x6C96E3
__declspec(noinline) inline void ExePropAngle(const Args& a, Out& o){
    float angle=a.f[2]; const float timeStep=a.f[0], propSpeed=a.f[1], circle=a.f[3];
    PL_BEGIN
    __asm {
        fld timeStep
        fmul propSpeed
        fadd angle
        fst angle
        fcomp circle
        fnstsw ax
        test ah,41h
        jne pa_done
        fld angle
    pa_loop:
        fsub circle
        fcom circle
        fnstsw ax
        test ah,41h
        je pa_loop
        fstp angle
    pa_done:
    }
    PL_END PutF(o,angle); }
// 0x6C987C..0x6C98AE
__declspec(noinline) inline void ExeGearFactors(const Args& a, Out& o){
    float r1, r2; const float gear=a.f[0], half=a.f[1], third=a.f[2], one=1.f;
    PL_BEGIN
    __asm {
        fld gear
        fabs
        fsubr one
        fmul half
        fadd one
        fstp r1
        fld gear
        fabs
        fsubr one
        fmul third
        fstp r2
    }
    PL_END PutF(o,r1); PutF(o,r2); }
// 0x6C97AB.. (op 0), 0x6C9956.. (1: + offset), 0x6C9A65.. (2: - offset), 0x6C9BE3.. (3: fchs)
__declspec(noinline) inline void ExeSurfaceAngle(const Args& a, Out& o){
    float r; const float amplitude=a.f[0], radians=a.f[1], control=a.f[2], addend=a.f[3]; const int operation=a.i[0];
    PL_BEGIN
    if(operation==0)      { __asm { fld amplitude } __asm { fmul radians } __asm { fmul control } __asm { fstp r } }
    else if(operation==1) { __asm { fld amplitude } __asm { fmul radians } __asm { fmul control } __asm { fadd addend } __asm { fstp r } }
    else if(operation==2) { __asm { fld amplitude } __asm { fmul radians } __asm { fmul control } __asm { fsub addend } __asm { fstp r } }
    else                  { __asm { fld amplitude } __asm { fmul radians } __asm { fmul control } __asm { fchs } __asm { fstp r } }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeSubtractPosition(const Args& a, Out& o){
    float v[3]{a.f[0],a.f[1],a.f[2]}; const float p[3]{a.f[3],a.f[4],a.f[5]};
    PL_BEGIN
    __asm {
        lea eax,v
        lea edx,p
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fstp dword ptr [eax]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fstp dword ptr [eax+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fstp dword ptr [eax+8]
    }
    PL_END PutF(o,v[0]); PutF(o,v[1]); PutF(o,v[2]); }
__declspec(noinline) inline void ExeFinOffset(const Args& a, Out& o){
    float r; const float finScale=a.f[0], boxMaxY=a.f[1], y=a.f[2], factor=a.f[3], one=1.f;
    PL_BEGIN __asm { fld one } __asm { fsub finScale } __asm { fmul boxMaxY } __asm { fmul factor } __asm { fadd y } __asm { fstp r } PL_END PutF(o,r); }
__declspec(noinline) inline void ExePropRotation(const Args& a, Out& o){
    float r; const float direction=a.f[0], propAngle=a.f[1]; const int doubled=a.i[0];
    PL_BEGIN
    if(doubled) { __asm { fld direction } __asm { fmul propAngle } __asm { fadd st(0),st } __asm { fstp r } }
    else        { __asm { fld direction } __asm { fmul propAngle } __asm { fchs } __asm { fstp r } }
    PL_END PutF(o,r); }
__declspec(noinline) inline void ExeGearRotation(const Args& a, Out& o){
    float r; const float gear=a.f[0], value=a.f[1];
    PL_BEGIN __asm { fld gear } __asm { fabs } __asm { fmul value } __asm { fstp r } PL_END PutF(o,r); }
// 0x6C9F40..0x6C9F52 (fild; fmul; fidiv; fstp)
__declspec(noinline) inline void ExeNozzleAngle(const Args& a, Out& o){
    float r; const float halfPi=a.f[0]; const int misc=a.i[0], limit=a.i[1];
    PL_BEGIN __asm { fild misc } __asm { fmul halfPi } __asm { fidiv limit } __asm { fstp r } PL_END PutF(o,r); }
__declspec(noinline) inline void ExeDustTime(const Args& a, Out& o){
    float r; const float acceleration=a.f[0], one=a.f[1], half=a.f[2];
    PL_BEGIN __asm { fld acceleration } __asm { fadd one } __asm { fmul half } __asm { fstp r } PL_END PutF(o,r); }
__declspec(noinline) inline void ExeSumVector3(const Args& a, Out& o){
    float vout[3]; const float p[3]{a.f[0],a.f[1],a.f[2]}, q[3]{a.f[3],a.f[4],a.f[5]};
    PL_BEGIN
    __asm {
        lea eax,p
        lea edx,q
        lea ecx,vout
        fld dword ptr [eax+8]
        fadd dword ptr [edx+8]
        fld dword ptr [eax+4]
        fadd dword ptr [edx+4]
        fld dword ptr [eax]
        fadd dword ptr [edx]
        fstp dword ptr [ecx]
        fstp dword ptr [ecx+4]
        fstp dword ptr [ecx+8]
    }
    PL_END PutF(o,vout[0]); PutF(o,vout[1]); PutF(o,vout[2]); }
// 0x6CA768..0x6CA7B4 (_ftol2 del EXE; el llamador usa el byte bajo, aqui se compara el entero de 32 bits)
__declspec(noinline) inline void ExeStreakAlpha(const Args& a, Out& o){
    uint32 result; const float s[3]{a.f[0],a.f[1],a.f[2]}, u[3]{a.f[3],a.f[4],a.f[5]}; const float attack=a.f[6], scale=a.f[7], bias=a.f[8], zero=0.f;
    PL_BEGIN
    __asm {
        lea eax,s
        lea edx,u
        fld dword ptr [eax+8]
        fmul dword ptr [edx+8]
        fld dword ptr [eax+4]
        fmul dword ptr [edx+4]
        faddp st(1),st
        fld dword ptr [edx]
        fmul dword ptr [eax]
        faddp st(1),st
        fmul attack
        fmul scale
        fabs
        fsub bias
        fld zero
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne sa_convert
        fstp st(0)
        fld zero
    sa_convert:
        mov eax,0x821B40
        call eax
        mov result,eax
    }
    PL_END PutU(o,result); }
__declspec(noinline) inline void ExeSmokeVelocity(const Args& a, Out& o){
    float vout[3]; const float s[3]{a.f[0],a.f[1],a.f[2]}; const float scale=a.f[3], zOffset=a.f[4]; const int subtract=a.i[0];
    PL_BEGIN
    __asm {
        lea edx,s
        lea eax,vout
        fld dword ptr [edx+8]
        fmul scale
        fld dword ptr [edx+4]
        fmul scale
        fld dword ptr [edx]
        fmul scale
        fstp dword ptr [eax]
        fstp dword ptr [eax+4]
        cmp subtract,0
        je sv_store
        fsub zOffset
    sv_store:
        fstp dword ptr [eax+8]
    }
    PL_END PutF(o,vout[0]); PutF(o,vout[1]); PutF(o,vout[2]); }
__declspec(noinline) inline void ExeNegate(const Args& a, Out& o){
    float r; const float x=a.f[0];
    PL_BEGIN __asm { fld x } __asm { fchs } __asm { fstp r } PL_END PutF(o,r); }
// 0x6CA334..0x6CA633 (Vortex): secuencia generada del desensamblado; [esi+...] -> variables locales, [esp+...] -> temporales locales
__declspec(noinline) inline void ExeHoverTilt(const Args& a, Out& o){
    const float rgtx=a.f[0], rgty=a.f[1], rgtz=a.f[2], fwdx=a.f[3], fwdy=a.f[4], fwdz=a.f[5], velx=a.f[6], vely=a.f[7], velz=a.f[8];
    const float w0=a.f[9], w1=a.f[10], w2=a.f[11], w3=a.f[12], wz=a.f[13], wsize=a.f[14], timeStep=a.f[15];
    float oRz=a.f[16], oFz=a.f[17], oUz=a.f[18], oUx=0.f, oUy=0.f, t1c=0.f, t10=0.f;
    float lines[32]{}; lines[1]=a.f[19]; lines[9]=a.f[20]; lines[24]=a.f[21]; lines[8]=a.f[22];
    PL_BEGIN
    __asm {
    fld dword ptr fwdz
    fmul dword ptr velz
    fld dword ptr fwdy
    fmul dword ptr vely
    faddp st(1), st
    fld dword ptr fwdx
    fmul dword ptr velx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
    fcomp dword ptr ds:[0x8D3414]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca368
    fld dword ptr ds:[0x8D3414]
    jmp hv_6ca384
  hv_6ca368:
    fld dword ptr velz
    fmul dword ptr fwdz
    fld dword ptr vely
    fmul dword ptr fwdy
    faddp st(1), st
    fld dword ptr velx
    fmul dword ptr fwdx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
  hv_6ca384:
    fld dword ptr ds:[0x8D3414]
    fchs
    fst dword ptr t1c
    fcomp st(1)
    fnstsw ax
    fstp st(0)
    test ah, 0x41
    jne hv_6ca3a8
    mov edx, dword ptr t1c
    mov dword ptr oUy, edx
    jmp hv_6ca402
  hv_6ca3a8:
    fld dword ptr velz
    fmul dword ptr fwdz
    fld dword ptr vely
    fmul dword ptr fwdy
    faddp st(1), st
    fld dword ptr velx
    fmul dword ptr fwdx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
    fcomp dword ptr ds:[0x8D3414]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca3df
    mov eax, dword ptr ds:[0x8D3414]
    mov dword ptr oUy, eax
    jmp hv_6ca402
  hv_6ca3df:
    fld dword ptr velz
    fmul dword ptr fwdz
    fld dword ptr vely
    fmul dword ptr fwdy
    faddp st(1), st
    fld dword ptr velx
    fmul dword ptr fwdx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
    fstp dword ptr oUy
  hv_6ca402:
    fld dword ptr velz
    fmul dword ptr rgtz
    fld dword ptr vely
    fmul dword ptr rgty
    faddp st(1), st
    fld dword ptr rgtx
    fmul dword ptr velx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
    fcomp dword ptr ds:[0x8D3414]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca432
    fld dword ptr ds:[0x8D3414]
    jmp hv_6ca44d
  hv_6ca432:
    fld dword ptr velz
    fmul dword ptr rgtz
    fld dword ptr vely
    fmul dword ptr rgty
    faddp st(1), st
    fld dword ptr rgtx
    fmul dword ptr velx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
  hv_6ca44d:
    fld dword ptr t1c
    fcomp st(1)
    fnstsw ax
    fstp st(0)
    test ah, 0x41
    jne hv_6ca469
    mov ecx, dword ptr t1c
    mov dword ptr oUx, ecx
    jmp hv_6ca4c2
  hv_6ca469:
    fld dword ptr velz
    fmul dword ptr rgtz
    fld dword ptr vely
    fmul dword ptr rgty
    faddp st(1), st
    fld dword ptr velx
    fmul dword ptr rgtx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
    fcomp dword ptr ds:[0x8D3414]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca4a0
    mov edx, dword ptr ds:[0x8D3414]
    mov dword ptr oUx, edx
    jmp hv_6ca4c2
  hv_6ca4a0:
    fld dword ptr velz
    fmul dword ptr rgtz
    fld dword ptr vely
    fmul dword ptr rgty
    faddp st(1), st
    fld dword ptr velx
    fmul dword ptr rgtx
    faddp st(1), st
    fmul dword ptr ds:[0x8D3418]
    fstp dword ptr oUx
  hv_6ca4c2:
    fld dword ptr ds:[0x8D3410]
    fld dword ptr timeStep
    mov eax, 0x822130
    call eax
    fld dword ptr w2
    fsub dword ptr w3
    lea eax, lines
    fld dword ptr w0
    fsub dword ptr w1
    faddp st(1), st
    fmul dword ptr ds:[0x858B8C]
    fld dword ptr [eax+4]
    fsub dword ptr [eax+36]
    fdivp st(1), st
    fcom dword ptr ds:[0x8D340C]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca515
    fstp st(0)
    fld dword ptr ds:[0x8D340C]
    jmp hv_6ca532
  hv_6ca515:
    fld dword ptr ds:[0x8D340C]
    fchs
    fstp dword ptr t10
    fcom dword ptr t10
    fnstsw ax
    test ah, 5
    jp hv_6ca532
    fstp st(0)
    fld dword ptr t10
  hv_6ca532:
    fld dword ptr ds:[0x858624]
    fsub st, st(2)
    fstp dword ptr t1c
    fld dword ptr oFz
    fmul st, st(2)
    fld dword ptr t1c
    fmul st, st(2)
    faddp st(1), st
    fstp dword ptr oFz
    lea eax, lines
    fstp st(0)
    fld dword ptr w3
    fsub dword ptr w1
    fld dword ptr w2
    fsub dword ptr w0
    faddp st(1), st
    fmul dword ptr ds:[0x858B8C]
    fld dword ptr [eax+96]
    fsub dword ptr [eax+32]
    fdivp st(1), st
    fcom dword ptr ds:[0x8D3408]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca59a
    fstp st(0)
    fld dword ptr ds:[0x8D3408]
    jmp hv_6ca5b7
  hv_6ca59a:
    fld dword ptr ds:[0x8D3408]
    fchs
    fstp dword ptr t10
    fcom dword ptr t10
    fnstsw ax
    test ah, 5
    jp hv_6ca5b7
    fstp st(0)
    fld dword ptr t10
  hv_6ca5b7:
    fld dword ptr oRz
    fmul st, st(2)
    fld dword ptr t1c
    fmul st, st(2)
    faddp st(1), st
    fstp dword ptr oRz
    fstp st(0)
    fld dword ptr w3
    fadd dword ptr w1
    fadd dword ptr w2
    fadd dword ptr w0
    fmul dword ptr ds:[0x858C84]
    fsub dword ptr wz
    fld dword ptr wsize
    fmul dword ptr ds:[0x858B8C]
    fdivp st(1), st
    fsubr dword ptr ds:[0x858624]
    fcom dword ptr ds:[0x8D3404]
    fnstsw ax
    test ah, 0x41
    jne hv_6ca613
    fstp st(0)
    fld dword ptr ds:[0x8D3404]
  hv_6ca613:
    fld dword ptr oUz
    fmul st, st(2)
    fld dword ptr t1c
    fmul st, st(2)
    faddp st(1), st
    fstp dword ptr oUz
    fstp st(0)
    fstp st(0)
    }
    PL_END PutF(o,oFz); PutF(o,oRz); PutF(o,oUz); PutF(o,oUx); PutF(o,oUy); }

using ExeFn = void(*)(const Args&, Out&);
inline const std::array<ExeFn,46> exeFns{
    ExeMassThreshold,ExeHealthAfterDamage,ExeSelectCloser,ExeControlBlend,ExeForwardSpeed,ExeInputSkid,ExeGroundSteer,ExeNozzleChange,ExeGearAdvance,
    ExeSpeedBelow,ExeSpeedCap,ExeGearResistance,ExeWaveAdvance,ExeDamageSpeed,ExeDamageNoise,ExePropellerWave,ExeFlightDecay,ExeFlightProductAdd,
    ExeFloatBlend,ExeDistanceBelow,ExeRollCompare,ExeCeilingTarget,ExeCeilingAcceleration,ExeFlightBound,ExeSmokeParticleVelocity,ExeVelocityNoise,
    ExePositionNoise,ExePropellerTarget,ExeSpeedSquaredAbove,ExeWheelCompression,ExeWheelHeight,ExeAndromadaWheelRotation,ExePropAngle,ExeGearFactors,
    ExeSurfaceAngle,ExeSubtractPosition,ExeFinOffset,ExePropRotation,ExeGearRotation,ExeNozzleAngle,ExeDustTime,ExeSumVector3,ExeStreakAlpha,
    ExeSmokeVelocity,ExeNegate,ExeHoverTilt};

// --- Candidatos: los ayudantes de X87Math llamados directamente ---
inline void Candidate(size_t f, const Args& a, Out& o) {
    switch(f) {
    case 0: PutF(o,PlaneMassThresholdCpp(a.f[0],a.f[1],a.f[2],a.i[0]!=0)); break;
    case 1: PutF(o,PlaneHealthAfterDamageCpp(a.f[0],a.f[1],a.f[2])); break;
    case 2: { float closest=a.f[6]; const float node[3]{a.f[0],a.f[1],a.f[2]}, rel[3]{a.f[3],a.f[4],a.f[5]}; const bool nearer=PlaneSelectCloserCpp(node,rel,&closest); PutF(o,closest); PutU(o,nearer); break; }
    case 3: PutF(o,PlaneControlBlendCpp(a.f[0],a.i[0],a.f[1],a.f[2],a.f[3])); break;
    case 4: { const float s[3]{a.f[0],a.f[1],a.f[2]}, d[3]{a.f[3],a.f[4],a.f[5]}; PutF(o,PlaneForwardSpeedCpp(s,d)); break; }
    case 5: PutF(o,PlaneInputSkidCpp(a.f[0],a.f[1],a.f[2],a.f[3])); break;
    case 6: PutF(o,PlaneGroundSteerCpp(a.f[0],a.f[1],a.f[2],a.f[3],a.f[4])); break;
    case 7: PutU(o,uint32(PlaneNozzleChangeCpp(a.i[0],a.f[0],a.f[1],a.f[2]))); break;
    case 8: { float gear=a.f[0]; const bool reached=PlaneGearAdvanceCpp(&gear,a.f[1],a.f[2],a.f[3]); PutF(o,gear); PutU(o,reached); break; }
    case 9: { const float s[3]{a.f[0],a.f[1],a.f[2]}; PutU(o,PlaneSpeedBelowCpp(s,a.f[3])); break; }
    case 10: { float s[3]{a.f[0],a.f[1],a.f[2]}; PlaneSpeedCapCpp(s,a.f[3]); PutF(o,s[0]); PutF(o,s[1]); PutF(o,s[2]); break; }
    case 11: PutF(o,PlaneGearResistanceCpp(a.f[0],a.f[1],a.f[2])); break;
    case 12: PutU(o,PlaneWaveAdvanceCpp(a.f[0],a.f[1],a.i[0],a.f[2],a.f[3],a.f[4])); break;
    case 13: { const float s[3]{a.f[0],a.f[1],a.f[2]}; PutF(o,PlaneDamageSpeedCpp(s)); break; }
    case 14: PutF(o,PlaneDamageNoiseCpp(a.f[0],a.f[1],a.i[0],a.f[2],a.f[3],a.f[4],a.f[5])); break;
    case 15: PutF(o,PlanePropellerWaveCpp(a.f[0],a.f[1],a.f[2],a.i[0],a.i[1],a.u[0],a.u[1],a.f[3],a.f[4],a.f[5],a.f[6])); break;
    case 16: PutF(o,PlaneFlightDecayCpp(a.f[0],a.i[0],a.f[1],a.i[1]!=0)); break;
    case 17: PutF(o,PlaneFlightProductAddCpp(a.f[0],a.f[1],a.f[2],a.i[0]!=0)); break;
    case 18: PutF(o,PlaneFloatBlendCpp(a.f[0],a.f[1],a.f[2],a.f[3])); break;
    case 19: { const float p[3]{a.f[0],a.f[1],a.f[2]}, q[3]{a.f[3],a.f[4],a.f[5]}; PutU(o,PlaneDistanceBelowCpp(p,q,a.f[6])); break; }
    case 20: { const float data[2]{a.f[0],a.f[1]}; PutU(o,PlaneRollCompareCpp(RollFn,data,a.f[2],a.i[0]!=0)); break; }
    case 21: PutF(o,PlaneCeilingTargetCpp(a.f[0],a.f[1],a.f[2])); break;
    case 22: PutF(o,PlaneCeilingAccelerationCpp(a.f[0],a.f[1],a.f[2])); break;
    case 23: PutF(o,PlaneFlightBoundCpp(a.f[0],a.i[0]!=0)); break;
    case 24: { float v[3]; const float s[3]{a.f[0],a.f[1],a.f[2]}; PlaneSmokeParticleVelocityCpp(v,s,a.f[3],a.f[4]); PutF(o,v[0]); PutF(o,v[1]); PutF(o,v[2]); break; }
    case 25: PutF(o,PlaneVelocityNoiseCpp(a.f[0],a.i[0],a.f[1],a.f[2],a.f[3])); break;
    case 26: PutF(o,PlanePositionNoiseCpp(a.f[0],a.f[1],a.i[0],a.f[2])); break;
    case 27: PutF(o,PlanePropellerTargetCpp(a.f[0],a.f[1],a.f[2],a.f[3])); break;
    case 28: { const float s[3]{a.f[0],a.f[1],a.f[2]}; PutU(o,PlaneSpeedSquaredAboveCpp(s,a.d[0])); break; }
    case 29: PutF(o,PlaneWheelCompressionCpp(a.f[0],a.f[1],a.f[2])); break;
    case 30: PutF(o,PlaneWheelHeightCpp(a.f[0],a.f[1],a.f[2],a.f[3],a.f[4],a.i[0]!=0)); break;
    case 31: { const float n[3]{a.f[2],a.f[3],a.f[4]}, d[3]{a.f[5],a.f[6],a.f[7]}; PutF(o,PlaneAndromadaWheelRotationCpp(a.f[0],a.f[1],n,d)); break; }
    case 32: PutF(o,PlanePropAngleCpp(a.f[0],a.f[1],a.f[2],a.f[3])); break;
    case 33: { float scale, offset; PlaneGearFactorsCpp(a.f[0],a.f[1],a.f[2],&scale,&offset); PutF(o,scale); PutF(o,offset); break; }
    case 34: PutF(o,PlaneSurfaceAngleCpp(a.f[0],a.f[1],a.f[2],a.f[3],a.i[0])); break;
    case 35: { float v[3]{a.f[0],a.f[1],a.f[2]}; const float p[3]{a.f[3],a.f[4],a.f[5]}; PlaneSubtractPositionCpp(v,p); PutF(o,v[0]); PutF(o,v[1]); PutF(o,v[2]); break; }
    case 36: PutF(o,PlaneFinOffsetCpp(a.f[0],a.f[1],a.f[2],a.f[3])); break;
    case 37: PutF(o,PlanePropRotationCpp(a.f[0],a.f[1],a.i[0]!=0)); break;
    case 38: PutF(o,PlaneGearRotationCpp(a.f[0],a.f[1])); break;
    case 39: PutF(o,PlaneNozzleAngleCpp(a.i[0],a.f[0],a.i[1])); break;
    case 40: PutF(o,PlaneDustTimeCpp(a.f[0],a.f[1],a.f[2])); break;
    case 41: { float v[3]; const float p[3]{a.f[0],a.f[1],a.f[2]}, q[3]{a.f[3],a.f[4],a.f[5]}; PlaneSumVector3Cpp(p,q,v); PutF(o,v[0]); PutF(o,v[1]); PutF(o,v[2]); break; }
    case 42: { const float s[3]{a.f[0],a.f[1],a.f[2]}, u[3]{a.f[3],a.f[4],a.f[5]}; PutU(o,PlaneStreakAlphaCpp(s,u,a.f[6],a.f[7],a.f[8])); break; }
    case 43: { float v[3]; const float s[3]{a.f[0],a.f[1],a.f[2]}; PlaneSmokeVelocityCpp(s,a.f[3],a.f[4],a.i[0]!=0,v); PutF(o,v[0]); PutF(o,v[1]); PutF(o,v[2]); break; }
    case 44: PutF(o,PlaneNegateCpp(a.f[0])); break;
    default: {
        float rz=a.f[16], fz=a.f[17], uz=a.f[18], ux=0.f, uy=0.f; float lines[32]{}; lines[1]=a.f[19]; lines[9]=a.f[20]; lines[24]=a.f[21]; lines[8]=a.f[22];
        const float right[3]{a.f[0],a.f[1],a.f[2]}, forward[3]{a.f[3],a.f[4],a.f[5]}, velocity[3]{a.f[6],a.f[7],a.f[8]}, wheels[4]{a.f[9],a.f[10],a.f[11],a.f[12]};
        PlaneHoverTiltCpp(&rz,&fz,&ux,&uy,&uz,right,forward,velocity,wheels,lines,a.f[13],a.f[14],a.f[15]);
        PutF(o,fz); PutF(o,rz); PutF(o,uz); PutF(o,ux); PutF(o,uy); break; }
    }
}

// --- Argumentos por funcion ---
inline Args MakeArgs(size_t f, uint32 x) {
    Args a{};
    auto F=[&](int j, Kind k){ a.f[j]=Gen(k,x,uint32(j)); };
    switch(f) {
    case 0: F(0,Free); F(1,Pos); F(2,Free); a.i[0]=GenInt(x,0,0,1); if(x%4==0)a.f[1]=10000.f; break;
    case 1: F(0,Free); F(1,Free); F(2,Pos); break;
    case 2: for(int j=0;j<6;++j)F(j,Small); F(6,Pos); break;
    case 3: F(0,Unit); F(1,Pos); F(2,Pos); F(3,Pos); a.i[0]=GenInt(x,0,-128,128); if(x%4==0)a.f[1]=std::bit_cast<float>(0x3C000000u); break;
    case 4: for(int j=0;j<6;++j)F(j,Small); break;
    case 5: F(0,Unit); F(1,Unit); F(2,Pos); F(3,Pos); break;
    case 6: F(0,Small); F(1,Pos); F(2,Unit); F(3,Pos); F(4,Pos); break;
    case 7: a.i[0]=GenInt(x,0,-128,128); F(0,Pos); F(1,Free); F(2,Pos); break;
    case 8: F(0,Unit); F(1,Pos); F(2,Pos); F(3,Unit01); break;
    case 9: for(int j=0;j<3;++j)F(j,Small); F(3,Pos); break;
    case 10: for(int j=0;j<3;++j)F(j,Free); F(3,Pos); break;
    case 11: F(0,Free); F(1,Pos); F(2,Pos); break;
    case 12: F(0,Unit01); F(1,Pos); a.i[0]=GenInt(x,0,0,32767); F(2,Pos); F(3,Pos); F(4,Free); break;
    case 13: for(int j=0;j<3;++j)F(j,Free); break;
    case 14: F(0,Unit); F(1,Pos); a.i[0]=GenInt(x,0,0,32767); F(2,Pos); F(3,Pos); F(4,Pos); F(5,Pos); break;
    case 15: F(0,Unit); F(1,Pos); F(2,Unit01); a.i[0]=GenInt(x,0,0,32767); a.i[1]=GenInt(x,1,-3,5); a.u[0]=GenUnsigned(x,0); a.u[1]=GenUnsigned(x,1); F(3,Pos); F(4,Pos); F(5,Pos); F(6,Pos); break;
    case 16: F(0,Unit); a.i[0]=GenInt(x,0,-3,5); F(1,Pos); a.i[1]=GenInt(x,1,0,1); break;
    case 17: F(0,Unit); F(1,Pos); F(2,Small); a.i[0]=GenInt(x,0,0,1); break;
    case 18: F(0,Free); F(1,Free); F(2,Pos); F(3,Pos); break;
    case 19: for(int j=0;j<6;++j)F(j,Free); F(6,Pos); break;
    case 20: F(0,Small); F(1,Small); F(2,Angle); a.i[0]=GenInt(x,0,0,1); break;
    case 21: F(0,Small); F(1,Unit); F(2,Pos); break;
    case 22: F(0,Small); F(1,Unit); F(2,Pos); break;
    case 23: F(0,Unit01); a.i[0]=GenInt(x,0,0,1); break;
    case 24: for(int j=0;j<3;++j)F(j,Free); F(3,Pos); F(4,Pos); break;
    case 25: F(0,Free); a.i[0]=GenInt(x,0,0,32767); F(1,Pos); F(2,Pos); F(3,Pos); break;
    case 26: F(0,Free); F(1,Pos); a.i[0]=GenInt(x,0,0,32767); F(2,Pos); break;
    case 27: F(0,Unit); F(1,Pos); F(2,Pos); F(3,Pos); break;
    case 28: for(int j=0;j<3;++j)F(j,Small); { const uint32 pat=kPatterns[Rng(x*5u+1u).next()%kPatterns.size()]; a.d[0]=(x%4==0)?double(std::bit_cast<float>(pat)):double(Rng(x*7u+3u).uni(0.f,0.01f)); } break;
    case 29: F(0,Pos); F(1,Pos); F(2,Unit01); break;
    case 30: F(0,Small); F(1,Small); F(2,Unit); F(3,Pos); F(4,Small); a.i[0]=GenInt(x,0,0,1); break;
    case 31: F(0,Small); F(1,Unit01); for(int j=2;j<8;++j)F(j,Unit); break;
    case 32: a.f[0]=Gen(Safe,x,0); a.f[1]=Gen(Safe,x,1); a.f[2]=Gen(Safe,x,2); a.f[3]=(x%3==0)?std::bit_cast<float>(0x40C90FDBu):Gen(Safe,x,3)<0.f?6.283185f:6.283185f; break;
    case 33: F(0,Unit); a.f[1]=0.5f; a.f[2]=std::bit_cast<float>(0x3E99999Au); if(x%4!=0){F(1,Pos);F(2,Pos);} break;
    case 34: F(0,Free); F(1,Pos); F(2,Unit); F(3,Unit); a.i[0]=GenInt(x,0,0,3); break;
    case 35: for(int j=0;j<6;++j)F(j,Small); break;
    case 36: F(0,Pos); F(1,Small); F(2,Small); F(3,Pos); break;
    case 37: F(0,Unit); F(1,Free); a.i[0]=GenInt(x,0,0,1); break;
    case 38: F(0,Unit); F(1,Small); break;
    case 39: a.i[0]=GenInt(x,0,0,65535); F(0,Pos); a.i[1]=GenInt(x,1,-5000,5000); break;
    case 40: F(0,Unit); a.f[1]=1.f; a.f[2]=0.5f; if(x%4==1){F(1,Pos);F(2,Pos);} break;
    case 41: for(int j=0;j<6;++j)F(j,Free); break;
    case 42: for(int j=0;j<6;++j)F(j,Small); F(6,Pos); F(7,Free); F(8,Free); break;
    case 43: for(int j=0;j<3;++j)F(j,Free); F(3,Pos); F(4,Pos); a.i[0]=GenInt(x,0,0,1); break;
    case 44: F(0,Free); break;
    default: for(int j=0;j<9;++j)F(j,Unit); for(int j=9;j<13;++j)F(j,Small); F(13,Small); F(14,Pos); F(15,Pos); F(16,Unit); F(17,Unit); F(18,Unit); F(19,Pos); F(20,Free); F(21,Pos); F(22,Free); break;
    }
    return a;
}

using ErrnoFn = int*(__cdecl*)();

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppplanex87stress1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppplanex87stress1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppplanex87stress1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    // Direcciones de la DLL (llamada directa) y del EXE vigiladas
    const std::array<uintptr_t,46> dllEntries{
        std::bit_cast<uintptr_t>(&PlaneMassThresholdCpp), std::bit_cast<uintptr_t>(&PlaneHealthAfterDamageCpp), std::bit_cast<uintptr_t>(&PlaneSelectCloserCpp),
        std::bit_cast<uintptr_t>(&PlaneControlBlendCpp), std::bit_cast<uintptr_t>(&PlaneForwardSpeedCpp), std::bit_cast<uintptr_t>(&PlaneInputSkidCpp),
        std::bit_cast<uintptr_t>(&PlaneGroundSteerCpp), std::bit_cast<uintptr_t>(&PlaneNozzleChangeCpp), std::bit_cast<uintptr_t>(&PlaneGearAdvanceCpp),
        std::bit_cast<uintptr_t>(&PlaneSpeedBelowCpp), std::bit_cast<uintptr_t>(&PlaneSpeedCapCpp), std::bit_cast<uintptr_t>(&PlaneGearResistanceCpp),
        std::bit_cast<uintptr_t>(&PlaneWaveAdvanceCpp), std::bit_cast<uintptr_t>(&PlaneDamageSpeedCpp), std::bit_cast<uintptr_t>(&PlaneDamageNoiseCpp),
        std::bit_cast<uintptr_t>(&PlanePropellerWaveCpp), std::bit_cast<uintptr_t>(&PlaneFlightDecayCpp), std::bit_cast<uintptr_t>(&PlaneFlightProductAddCpp),
        std::bit_cast<uintptr_t>(&PlaneFloatBlendCpp), std::bit_cast<uintptr_t>(&PlaneDistanceBelowCpp), std::bit_cast<uintptr_t>(&PlaneRollCompareCpp),
        std::bit_cast<uintptr_t>(&PlaneCeilingTargetCpp), std::bit_cast<uintptr_t>(&PlaneCeilingAccelerationCpp), std::bit_cast<uintptr_t>(&PlaneFlightBoundCpp),
        std::bit_cast<uintptr_t>(&PlaneSmokeParticleVelocityCpp), std::bit_cast<uintptr_t>(&PlaneVelocityNoiseCpp), std::bit_cast<uintptr_t>(&PlanePositionNoiseCpp),
        std::bit_cast<uintptr_t>(&PlanePropellerTargetCpp), std::bit_cast<uintptr_t>(&PlaneSpeedSquaredAboveCpp), std::bit_cast<uintptr_t>(&PlaneWheelCompressionCpp),
        std::bit_cast<uintptr_t>(&PlaneWheelHeightCpp), std::bit_cast<uintptr_t>(&PlaneAndromadaWheelRotationCpp), std::bit_cast<uintptr_t>(&PlanePropAngleCpp),
        std::bit_cast<uintptr_t>(&PlaneGearFactorsCpp), std::bit_cast<uintptr_t>(&PlaneSurfaceAngleCpp), std::bit_cast<uintptr_t>(&PlaneSubtractPositionCpp),
        std::bit_cast<uintptr_t>(&PlaneFinOffsetCpp), std::bit_cast<uintptr_t>(&PlanePropRotationCpp), std::bit_cast<uintptr_t>(&PlaneGearRotationCpp),
        std::bit_cast<uintptr_t>(&PlaneNozzleAngleCpp), std::bit_cast<uintptr_t>(&PlaneDustTimeCpp), std::bit_cast<uintptr_t>(&PlaneSumVector3Cpp),
        std::bit_cast<uintptr_t>(&PlaneStreakAlphaCpp), std::bit_cast<uintptr_t>(&PlaneSmokeVelocityCpp), std::bit_cast<uintptr_t>(&PlaneNegateCpp),
        std::bit_cast<uintptr_t>(&PlaneHoverTiltCpp)};
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
    Write({{"event","begin"},{"functions",int(fns.size())},{"pairs",int(fns.size())*kCases},{"registered_hook",false},{"direct_dll_call",dllOutside},
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
        for(unsigned fixture=0;fixture<unsigned(kCases);++fixture,++pair) {
            const Args a=MakeArgs(f,fixture);
            auto run=[&](bool candidate) -> json {
                restore();
                gStack=true; gRegs=true;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                Out out{};
                if(candidate) Candidate(f,a,out); else exeFns[f](a,out);
                const unsigned fpAfter=_controlfp(0,0);
                const bool stackOK=gStack, regsOK=gRegs;
                restore();
                const bool protectedOK=(fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                const bool routeOK = candidate ? dllEntries[f]>=0x1000000 : true;
                json bitsRow=json::array(); for(int k=0;k<out.n;++k) bitsRow.push_back(Hex(out.w[k]));
                return json{{"route_ok",routeOK},{"stack_restored",stackOK},{"nonvolatile_registers_preserved",regsOK},
                            {"protected_state_unchanged",protectedOK},{"bits",bitsRow}};
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            json args=json::array(); for(float v:a.f) args.push_back(Hex(Bits(v))); for(int32 v:a.i) args.push_back(Hex(uint32(v))); for(uint32 v:a.u) args.push_back(Hex(v));
            args.push_back(Hex(std::bit_cast<uint64>(a.d[0])>>32)); args.push_back(Hex(uint32(std::bit_cast<uint64>(a.d[0]))));
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},{"args",args},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",kCases},{"failures",failures}});
    }
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
