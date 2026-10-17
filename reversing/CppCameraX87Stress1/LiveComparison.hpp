#pragma once
// Banco dentro del motor (ESCRITO SIN COMPILAR NI EJECUTAR): ayudantes x87 con que Camera.cpp sustituye su ensamblador (X87Math.cpp, prefijo Camera: CameraElapsedSecondsCpp,
// CameraShakePercentCpp, CameraCutSceneElapsedCpp, CameraTimePlusCpp, CameraAimHorizontalCpp, CameraAimVerticalCpp, CameraQuickAimPitchCpp, CameraNearClipPedCpp,
// CameraSphereInFrustumCpp, CameraLinearBlendCpp, CameraFovLerpRatioCpp, CameraPhaseCpp, CameraEaseCpp, CameraAdvanceCpp, CameraAdvanceTowardCpp, CameraFovAdvanceCpp,
// CameraShakeForceCpp, CameraAccumulateDistanceCpp, CameraAverageDistanceCpp, CameraDotYZXCpp, CameraFrustumTermsCpp, CameraFrontNormCpp, CameraCineyRotatedCpp,
// CameraAtanPlusCpp, CameraAtanMinusCpp, CameraHeadingMinusCpp, CameraTruncateProductCpp).
// El "original" es la secuencia EXACTA de instrucciones del EXE (adaptador de ensamblador DENTRO del banco, desensamblado de gta-sa.exe 0x50A97E..0x52CB10, 0x420C40,
// 0x514970, 0x514D60, 0x5150E0, 0x515236, 0x51F5DD, 0x52A87A...; _ftol2 se llama por su direccion 0x821B40) y el candidato es el ayudante de la DLL llamado directamente.
// GetATanOfXY / GetHeading se sustituyen en AMBOS lados por la misma funcion del banco (nakeds TestAtan / TestHeading que devuelven ST0 sin redondear).
// Complementa a reversing/CameraFinal4, CameraProcess355 y compania (fixtures moderados, sin NaN/inf/subnormales): aqui NaN de carga y signo distintos, sNaN, +-inf, +-0,
// subnormales, +-1e30/1e-30, FLT_MAX y relojes alrededor de 2^24 y 2^31.
#include <fstream>
#include <float.h>
#include <bit>
#include <array>
#include "Camera.h"
#include "X87Math.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppCameraX87Stress1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,32> fns{{
    {"Camera","ElapsedSeconds",0,0,-1,2048},       {"Camera","ShakePercent",0,0,-1,2048},
    {"Camera","CutSceneElapsed",0,0,-1,2048},      {"Camera","TimePlus",0,0,-1,2048},
    {"Camera","AimHorizontal",0,0,-1,2048},        {"Camera","AimVertical",0,0,-1,2048},
    {"Camera","QuickAimPitch",0,0,-1,2048},        {"Camera","NearClipPed",0,0,-1,2048},
    {"Camera","SphereInFrustum",0,0,-1,2048},      {"Camera","LinearBlend",0,0,-1,2048},
    {"Camera","FovLerpRatio",0,0,-1,2048},         {"Camera","Phase",0,0,-1,2048},
    {"Camera","Ease",0,0,-1,2048},                 {"Camera","Advance0",0,0,-1,2048},
    {"Camera","Advance1",0,0,-1,2048},             {"Camera","Advance2",0,0,-1,2048},
    {"Camera","Advance3",0,0,-1,2048},             {"Camera","Advance4",0,0,-1,2048},
    {"Camera","AdvanceToward1",0,0,-1,2048},       {"Camera","AdvanceToward4",0,0,-1,2048},
    {"Camera","FovAdvance",0,0,-1,2048},           {"Camera","ShakeForce",0,0,-1,2048},
    {"Camera","AccumulateDistance",0,0,-1,2048},   {"Camera","AverageDistance",0,0,-1,2048},
    {"Camera","DotYZX",0,0,-1,2048},               {"Camera","FrustumTerms",0,0,-1,2048},
    {"Camera","FrontNorm",0,0,-1,2048},            {"Camera","CineyRotated",0,0,-1,2048},
    {"Camera","AtanPlus",0,0,-1,2048},             {"Camera","AtanMinus",0,0,-1,2048},
    {"Camera","HeadingMinus",0,0,-1,2048},         {"Camera","TruncateProduct",0,0,-1,2048},
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
inline const std::array<uint32,48> kPatterns{
    0x00000000u,0x80000000u,0x3F800000u,0xBF800000u,0x00000001u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x00800000u,0x80800000u,
    0x7F7FFFFFu,0xFF7FFFFFu,0x7F800000u,0xFF800000u,0x7FC00000u,0xFFC00000u,0x7FC12345u,0xFFC0ABCDu,0x7FA00001u,0xFFA55555u,
    0x7F800001u,0x3F000000u,0xBF000000u,0x3F3504F3u,0x40490FDBu,0xC0490FDBu,0x3FC90FDBu,0x40C90FDBu,0x4B800000u,0x5F000000u,
    0xDF000000u,0x1E3CE508u,0x7149F2CAu,0x0DA24260u,0xF149F2CAu,0x3F800001u,0x3F7FFFFFu,0xBF800001u,0xBF7FFFFFu,0x3C23D70Au,
    0x3E99999Au,0x3F4CCCCDu,0x3CA3D70Au,0x3C8EFA35u,0x3A83126Fu,0x4F800000u,0x43340000u,0x43870000u};
// Relojes y duraciones sin signo: 0, 1, alrededor de 2^24, 2^31 y 2^32, y valores tipicos de partida.
inline const std::array<uint32,20> kTimes{
    0u,1u,2u,1350u,600u,5000u,600000u,0x00FFFFFFu,0x01000000u,0x01000001u,0x7FFFFFFFu,0x80000000u,0x80000001u,0xF0000001u,0xFFFFFFFFu,0xFFFFF000u,0xC0000000u,0x7FFFFF01u,0x80000100u,0x12345678u};
enum Kind { Angle, Unit, Pos, Unit01, Free, Fov, Coord, Small };
// Argumento j del fixture: 3 de cada 8 fixtures solo patrones; el resto uniforme del dominio con ~25% de patrones.
inline float Gen(Kind kind, uint32 fixture, uint32 j) {
    Rng r(fixture*32u+j+1u);
    const bool onlyPatterns = (fixture%8u)<3u;
    if(onlyPatterns || r.next()%4u==0u) return std::bit_cast<float>(kPatterns[r.next()%kPatterns.size()]);
    switch(kind) {
        case Angle: return r.uni(-4.f,4.f);
        case Unit: return r.uni(-1.2f,1.2f);
        case Pos: return r.uni(0.1f,3.f);
        case Unit01: return r.uni(0.f,1.f);
        case Fov: return r.uni(5.f,170.f);
        case Coord: return r.uni(-300.f,300.f);
        case Small: return r.uni(-2.f,2.f);
        default: return r.uni(-100.f,100.f);
    }
}
inline uint32 GenTime(uint32 fixture, uint32 j) {
    Rng r(fixture*32u+j+101u);
    return r.next()%3u==0u ? r.next() : kTimes[r.next()%kTimes.size()];
}
inline int32 GenInt(uint32 fixture, uint32 j) {
    static constexpr int32 t[]{0,1,-1,2,4,5,100,640,480,1920,1080,-640,0x7FFFFFFF,int32(0x80000000),-2};
    Rng r(fixture*32u+j+201u);
    return t[r.next()%std::size(t)];
}

struct A { float f[24]{}; uint32 u[3]{}; int32 i[2]{}; uint32 flag{}; };
struct Out { uint32 v[4]{}; };
inline Out O1(float x) { Out o; o.v[0]=Bits(x); return o; }
inline Out O1(uint32 x) { Out o; o.v[0]=x; return o; }
inline Out O2(float x,float y) { Out o; o.v[0]=Bits(x); o.v[1]=Bits(y); return o; }
inline Out O3(const float* p) { Out o; for(int k=0;k<3;++k) o.v[k]=Bits(p[k]); return o; }
inline Out O4(const float* p) { Out o; for(int k=0;k<4;++k) o.v[k]=Bits(p[k]); return o; }

// --- Funciones del banco que sustituyen a CGeneral::GetATanOfXY / CPlaceable::GetHeading en los dos lados: devuelven ST0 sin redondear a float ---
inline double gHeadingValue{};
__declspec(naked) inline float __cdecl TestAtan(float, float) { __asm { fld dword ptr [esp+4]
    fld dword ptr [esp+8]
    fpatan
    ret } }
__declspec(naked) inline float __fastcall TestHeading(const void*, void*) { __asm { fld qword ptr [gHeadingValue]
    ret } }

// --- Medida de ABI por adaptador: ESP y registros no volatiles antes y despues ---
inline bool gStack{true}, gRegs{true};
#define CAM_BEGIN uintptr_t sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{}; \
    __asm { mov sp0,esp } __asm { mov b0,ebx } __asm { mov s0,esi } __asm { mov d0,edi } __asm { mov p0,ebp }
#define CAM_END __asm { mov sp1,esp } __asm { mov b1,ebx } __asm { mov s1,esi } __asm { mov d1,edi } __asm { mov p1,ebp } \
    gStack&=(sp0==sp1); gRegs&=(b0==b1&&s0==s1&&d0==d1&&p0==p1);
inline uint16 Top() { uint16 v{}; __asm { fnstsw v } return uint16(v&0x3800); }

// =====================================================================================================================
// Secuencias exactas del EXE (los registros usados son solo EAX, ECX y EDX)
// =====================================================================================================================
inline constexpr float kFourGig = 4294967296.0f;
inline constexpr float kHalf = 0.5f, kOne = 1.0f, kZero = 0.0f;
inline const float kToRad = std::bit_cast<float>(0x3C8EFA35u);   // 0x8595EC
inline const float kPi = std::bit_cast<float>(0x40490FDBu);      // 0x858CB8
inline const float kMilli = std::bit_cast<float>(0x3A83126Fu);   // 0x858CDC
inline const float kHundredthF = std::bit_cast<float>(0x3C23D70Au); // 0x858C58
inline const float kFrustumFactor = std::bit_cast<float>(0x3C0EF921u); // 0x8631D8
inline const float kDecay = std::bit_cast<float>(0x3992CCF7u);   // 0x863280
inline const double kHundredthD = 0.01, kOneD = 1.0;             // 0x85A308, 0x85A310

// 0x50AA91..0x50AA9D
__declspec(noinline) inline Out ExeElapsed(const A& a) {
    float r{}; const uint32 e=a.u[0]; int32 tmp{}; const float fourGig=kFourGig, milli=kMilli;
    CAM_BEGIN
    __asm { mov eax,e
            mov tmp,eax
            fild tmp
            test eax,eax
            jge positive
            fadd fourGig
        positive:
            fmul milli
            fstp r }
    CAM_END return O1(r); }
// 0x50AA72..0x50AA8D
__declspec(noinline) inline Out ExeShakePercent(const A& a) {
    float r{}; const float dist=a.f[0]; const double k1=kHundredthD, k2=kOneD;
    CAM_BEGIN __asm { fld dist
                      fmul k1
                      fsubr k2
                      fstp r } CAM_END return O1(r); }
// 0x50C088..0x50C0AA
__declspec(noinline) inline Out ExeCutScene(const A& a) {
    float r{}; const float percent=a.f[0]; const int32 finish=int32(a.u[0]); const float fourGig=kFourGig, hundredth=kHundredthF;
    CAM_BEGIN __asm { fld percent
                      fmul hundredth
                      fild finish
                      cmp finish,0
                      jge positive
                      fadd fourGig
                  positive:
                      fmulp st(1),st(0)
                      fstp r } CAM_END return O1(r); }
// 0x50D254..0x50D273
__declspec(noinline) inline Out ExeTimePlus(const A& a) {
    float startValue{}, endValue{}; const int32 now=int32(a.u[0]); const float plus=a.f[0], fourGig=kFourGig;
    CAM_BEGIN __asm { fild now
                      cmp now,0
                      jge positive
                      fadd fourGig
                  positive:
                      fst startValue
                      fadd plus
                      fstp endValue } CAM_END return O2(startValue,endValue); }
// 0x514970..0x5149B6
__declspec(noinline) inline Out ExeAimHorizontal(const A& a) {
    float r{}; const float fov=a.f[0], camX=a.f[1], half=kHalf, toRad=kToRad;
    CAM_BEGIN __asm { fld fov
                      fmul half
                      fmul toRad
                      fptan
                      fstp st(0)
                      fld camX
                      fsub half
                      fadd st(0),st(0)
                      fmul st(0),st(1)
                      fstp r
                      fstp st(0) } CAM_END return O1(r); }
// 0x514970..0x514A21 (la parte vertical queda en ST0 en el EXE; aqui se guarda con fstp)
__declspec(noinline) inline Out ExeAimVertical(const A& a) {
    float r{}; const float fov=a.f[0], cy=a.f[1], aspect=a.f[2], half=kHalf, toRad=kToRad, one=kOne;
    CAM_BEGIN __asm { fld fov
                      fmul half
                      fmul toRad
                      fptan
                      fstp st(0)
                      fld half
                      fsub cy
                      fadd st(0),st(0)
                      fld one
                      fdiv aspect
                      fmulp st(1),st(0)
                      fmulp st(1),st(0)
                      fstp r } CAM_END return O1(r); }
// 0x50AD40..0x50AD8D
__declspec(noinline) inline Out ExeQuickAim(const A& a) {
    float r{}; const float fov=a.f[0], vertical=a.f[1], cy=a.f[2], aspect=a.f[3], half=kHalf, toRad=kToRad, one=kOne;
    CAM_BEGIN __asm { fld fov
                      fmul half
                      fmul toRad
                      fptan
                      fstp st(0)
                      fld half
                      fsub cy
                      fadd st(0),st(0)
                      fmulp st(1),st(0)
                      fld one
                      fdiv aspect
                      fmulp st(1),st(0)
                      fld1
                      fpatan
                      fadd vertical
                      fchs
                      fstp r } CAM_END return O1(r); }
// 0x50CB90..0x50CBC9
__declspec(noinline) inline Out ExeNearClip(const A& a) {
    float clip{}; const float arg=a.f[0], base=a.f[1], denominator=a.f[2], factor=a.f[3], maximum=a.f[4];
    CAM_BEGIN __asm { fld base
                      fld arg
                      fsqrt
                      fdiv denominator
                      fmul factor
                      fld maximum
                      fsub st(0),st(2)
                      fmulp st(1),st(0)
                      fadd st(0),st(1)
                      fst clip
                      fcomp st(1)
                      fnstsw ax
                      test ah,5
                      jp keepClip
                      fstp clip
                      jmp clipDone
                  keepClip:
                      fstp st(0)
                  clipDone: } CAM_END return O1(clip); }
// 0x420C70..0x420D3C (con el punto ya transformado: normals en ECX, punto en EDX)
__declspec(noinline) inline Out ExeSphere(const A& a) {
    uint32 result{}; const float radius=a.f[3], nearL=a.f[16], farL=a.f[17]; const float* point=&a.f[0]; const float* normals=&a.f[4];
    CAM_BEGIN __asm { mov ecx,normals
                      mov edx,point
                      fld dword ptr [edx+4]
                      fadd radius
                      fcomp nearL
                      fnstsw ax
                      test ah,5
                      jnp sphereReject
                      fld dword ptr [edx+4]
                      fsub radius
                      fcomp farL
                      fnstsw ax
                      test ah,41h
                      je sphereReject
                      fld dword ptr [edx]
                      fmul dword ptr [ecx]
                      fld dword ptr [edx+4]
                      fmul dword ptr [ecx+4]
                      faddp st(1),st
                      fcomp radius
                      fnstsw ax
                      test ah,41h
                      je sphereReject
                      fld dword ptr [edx]
                      fmul dword ptr [ecx+12]
                      fld dword ptr [edx+4]
                      fmul dword ptr [ecx+16]
                      faddp st(1),st
                      fcomp radius
                      fnstsw ax
                      test ah,41h
                      je sphereReject
                      fld dword ptr [edx+8]
                      fmul dword ptr [ecx+32]
                      fld dword ptr [edx+4]
                      fmul dword ptr [ecx+28]
                      faddp st(1),st
                      fcomp radius
                      fnstsw ax
                      test ah,41h
                      je sphereReject
                      fld dword ptr [edx+8]
                      fmul dword ptr [ecx+44]
                      fld dword ptr [edx+4]
                      fmul dword ptr [ecx+40]
                      faddp st(1),st
                      fcomp radius
                      fnstsw ax
                      test ah,41h
                      jne sphereAccept
                  sphereReject:
                      mov result,0
                      jmp sphereDone
                  sphereAccept:
                      mov result,1
                  sphereDone: } CAM_END return O1(result); }
// 0x50D510..0x50D578
__declspec(noinline) inline Out ExeLinear(const A& a) {
    float value{}; const float from=a.f[0], to=a.f[1], ratio=a.f[2], degrees=180.0f, startAngle=270.0f, radians=kToRad, one=kOne, half=kHalf;
    CAM_BEGIN
    if(a.flag) {
        __asm { fld ratio
                fmul degrees
                fsubr startAngle
                fmul radians
                fsin
                fadd one
                fmul half
                fld to
                fsub from
                fmulp st(1),st
                fadd from
                fstp value }
    } else {
        __asm { fld to
                fsub from
                fmul ratio
                fadd from
                fstp value }
    }
    CAM_END return O1(value); }
// 0x516500..0x51653B
__declspec(noinline) inline Out ExeFovLerp(const A& a) {
    float ratio{}; int32 running{}; const int32 now=int32(a.u[0]); const float endTime=a.f[0], startTime=a.f[1], fourGig=kFourGig;
    CAM_BEGIN __asm { mov running,0
                      fild now
                      cmp now,0
                      jge positive
                      fadd fourGig
                  positive:
                      fcom endTime
                      fnstsw ax
                      test ah,41h
                      jp notRunning
                      fsub startTime
                      fld endTime
                      fsub startTime
                      fdivp st(1),st(0)
                      fstp ratio
                      mov running,1
                      jmp lerpDone
                  notRunning:
                      fstp st(0)
                  lerpDone: }
    CAM_END return O2(float(running), running ? ratio : 0.f); }
// 0x52BA9D..0x52BAC9 (fase = E / D)
__declspec(noinline) inline Out ExePhase(const A& a) {
    float r{}; int32 numerator=int32(a.u[0]), denominator=int32(a.u[1]); const float fourGig=kFourGig;
    CAM_BEGIN __asm { fild numerator
                      cmp numerator,0
                      jge positive1
                      fadd fourGig
                  positive1:
                      fild denominator
                      cmp denominator,0
                      jge positive2
                      fadd fourGig
                  positive2:
                      fdivp st(1),st(0)
                      fstp r } CAM_END return O1(r); }
// 0x52BAD9..0x52BB4D (objetivo) y 0x52BBC6..0x52BC0E (alcance): ease = 0.5 - cos(x * pi) * 0.5
__declspec(noinline) inline Out ExeEase(const A& a) {
    float r{}; const float phase=a.f[0], moving=a.f[1], catchup=a.f[2], zero=kZero, one=kOne, pi=kPi, half=kHalf;
    CAM_BEGIN
    if(a.flag) {
        __asm { fld phase
                fld moving
                fcomp zero
                fnstsw ax
                test ah,44h
                jp movingNonZero
                fstp st(0)
                fld zero
                jmp common1
            movingNonZero:
                fsubr moving
                fdiv moving
            common1:
                fmul pi
                fcos
                fmul half
                fsubr half
                fstp r }
    } else {
        __asm { fld phase
                fld catchup
                fcomp zero
                fnstsw ax
                test ah,44h
                jp catchupNonZero
                fstp st(0)
                fld one
                jmp common2
            catchupNonZero:
                fsub moving
                fdiv catchup
            common2:
                fmul pi
                fcos
                fmul half
                fsubr half
                fstp r }
    }
    CAM_END return O1(r); }
// 0x52BB4D..0x52BCD5 y 0x52BD29..0x52BE80 (transcripcion del propio Camera.cpp anterior, validada por CameraProcess355 contra el EXE)
__declspec(noinline) inline Out ExeAdvance(const A& a, int mode) {
    float out[3]{}; const float ease=a.f[0]; const float* speed=&a.f[1]; const float* base=&a.f[4];
    const double coefficient=double(ease); const float stored=ease; const double spilledCoefficient=mode==0?coefficient:double(stored); float product{};
    float* result=out;
    CAM_BEGIN
    if(mode==0||mode==2){ __asm { fld coefficient } } else { __asm { fld stored } }
    __asm { mov eax,speed
            mov edx,base
            mov ecx,result
            fmul dword ptr [eax] }
    if(mode==1||mode==4){ __asm { fstp product
                                  fld product } }
    __asm { mov eax,speed
            mov edx,base
            mov ecx,result
            fadd dword ptr [edx]
            fstp dword ptr [ecx] }
    if(mode==0||mode==4){
        __asm { mov eax,speed
                mov edx,base
                mov ecx,result
                fld spilledCoefficient
                fmul dword ptr [eax+4]
                fstp product
                fld product
                fadd dword ptr [edx+4]
                fstp dword ptr [ecx+4]
                fld spilledCoefficient
                fmul dword ptr [eax+8]
                fstp product
                fld product
                fadd dword ptr [edx+8]
                fstp dword ptr [ecx+8] }
    } else {
        __asm { mov eax,speed
                mov edx,base
                mov ecx,result
                fld stored
                fmul dword ptr [eax+4]
                fadd dword ptr [edx+4]
                fstp dword ptr [ecx+4]
                fld stored
                fmul dword ptr [eax+8] }
        if(mode!=1){ __asm { fstp product
                             fld product } }
        __asm { mov edx,base
                mov ecx,result
                fadd dword ptr [edx+8]
                fstp dword ptr [ecx+8] }
    }
    CAM_END return O3(out); }
// 0x52BC51..0x52BCD5 (modo 1: restas de x e y sin guardar, la de z guardada) y 0x40FE60/0x40FEC0/0x40FE30 (modo 4: restas, productos y sumas guardadas)
__declspec(noinline) inline Out ExeToward(const A& a, int mode) {
    float out[3]{}; const float ease=a.f[0]; const float* dest=&a.f[1]; const float* base=&a.f[4]; float* result=out; float t0{}, t1{}, t2{};
    CAM_BEGIN
    if(mode==4) {
        __asm { mov eax,dest
                mov edx,base
                fld dword ptr [eax+8]
                fsub dword ptr [edx+8]
                fld dword ptr [eax+4]
                fsub dword ptr [edx+4]
                fld dword ptr [eax]
                fsub dword ptr [edx]
                fstp t0
                fstp t1
                fstp t2
                fld ease
                fmul t0
                fstp t0
                fld ease
                fmul t1
                fstp t1
                fld ease
                fmul t2
                fstp t2
                mov ecx,result
                fld t0
                fadd dword ptr [edx]
                fstp dword ptr [ecx]
                fld t1
                fadd dword ptr [edx+4]
                fstp dword ptr [ecx+4]
                fld t2
                fadd dword ptr [edx+8]
                fstp dword ptr [ecx+8] }
    } else {
        __asm { mov eax,dest
                mov edx,base
                mov ecx,result
                fld dword ptr [eax]
                fsub dword ptr [edx]
                fld dword ptr [eax+4]
                fsub dword ptr [edx+4]
                fld dword ptr [eax+8]
                fsub dword ptr [edx+8]
                fstp t2
                fxch st(1)
                fmul ease
                fstp t0
                fmul ease
                fld t2
                fmul ease
                fld t0
                fadd dword ptr [edx]
                fstp dword ptr [ecx]
                fxch st(1)
                fadd dword ptr [edx+4]
                fstp dword ptr [ecx+4]
                fadd dword ptr [edx+8]
                fstp dword ptr [ecx+8] }
    }
    CAM_END return O3(out); }
// 0x52BE00..0x52BE8E (suma) y 0x52C159..0x52C1C3 (diferencia)
__declspec(noinline) inline Out ExeFovAdvance(const A& a) {
    float r{}; const float base=a.f[0], operand=a.f[1], coefficient=a.f[2];
    CAM_BEGIN
    if(a.flag) { __asm { fld operand
                         fsub base
                         fmul coefficient
                         fadd base
                         fstp r } }
    else { __asm { fld operand
                   fmul coefficient
                   fadd base
                   fstp r } }
    CAM_END return O1(r); }
// 0x52C7E8..0x52C800
__declspec(noinline) inline Out ExeShakeForce(const A& a) {
    float r{}; const float force=a.f[0]; const int32 elapsed=int32(a.u[0]); const float fourGig=kFourGig, decay=kDecay;
    CAM_BEGIN __asm { fild elapsed
                      cmp elapsed,0
                      jge positive
                      fadd fourGig
                  positive:
                      fmul decay
                      fsubr force
                      fstp r } CAM_END return O1(r); }
// 0x52CA93..0x52CAF0 (el resultado se devuelve como double)
__declspec(noinline) inline Out ExeAccumulate(const A& a) {
    double value{}; const float* current=&a.f[0]; const float* previous=&a.f[3]; const float sum=a.f[6];
    CAM_BEGIN __asm { mov eax,current
                      mov edx,previous
                      fld dword ptr [eax]
                      fsub dword ptr [edx]
                      fld dword ptr [eax+4]
                      fsub dword ptr [edx+4]
                      fld dword ptr [eax+8]
                      fsub dword ptr [edx+8]
                      fld st(0)
                      fmul st(0),st(1)
                      fld st(2)
                      fmul st(0),st(3)
                      faddp st(1),st(0)
                      fld st(3)
                      fmul st(0),st(4)
                      faddp st(1),st(0)
                      fsqrt
                      fstp st(3)
                      fstp st(0)
                      fstp st(0)
                      fadd sum
                      fstp value } CAM_END
    Out o; const uint64 bits=std::bit_cast<uint64>(value); o.v[0]=uint32(bits); o.v[1]=uint32(bits>>32); return o; }
// 0x52CAF8..0x52CB0A
__declspec(noinline) inline Out ExeAverage(const A& a) {
    float r{}; const double sum=double(a.f[0]); const int32 frames=a.i[0];
    CAM_BEGIN __asm { fld sum
                      fild frames
                      fdivp st(1),st(0)
                      fstp r } CAM_END return O1(r); }
// 0x514EA5..0x514ED7
__declspec(noinline) inline Out ExeDot(const A& a) {
    float r{}; const float* n=&a.f[0]; const float* p=&a.f[3];
    CAM_BEGIN __asm { mov eax,n
                      mov edx,p
                      fld dword ptr [eax+4]
                      fmul dword ptr [edx+4]
                      fld dword ptr [eax+8]
                      fmul dword ptr [edx+8]
                      faddp st(1),st
                      fld dword ptr [eax]
                      fmul dword ptr [edx]
                      faddp st(1),st
                      fstp r } CAM_END return O1(r); }
// 0x514D63..0x514E4C
__declspec(noinline) inline Out ExeTerms(const A& a) {
    float t[4]{}; const float fov=a.f[0], factor=kFrustumFactor; const int32 height=a.i[0], width=a.i[1]; float ratio{}, cosine{}, sine{}, verticalCosine{}, verticalSine{};
    CAM_BEGIN __asm { fld fov
                      fmul factor
                      fld st(0)
                      fcos
                      fstp cosine
                      fsin
                      fstp sine
                      fild height
                      fidiv width
                      fst ratio
                      fmul cosine
                      fstp verticalCosine
                      fld ratio
                      fmul sine
                      fchs
                      fstp verticalSine } CAM_END
    t[0]=cosine; t[1]=sine; t[2]=verticalCosine; t[3]=verticalSine; return O4(t); }
// 0x51518E..0x5151E5
__declspec(noinline) inline Out ExeFrontNorm(const A& a) {
    float nx{}, ny{}; const float x=a.f[0], y=a.f[1], one=kOne;
    CAM_BEGIN __asm { fld x
                      fld y
                      fld st(0)
                      fmul st,st(1)
                      fld st(2)
                      fmul st,st(3)
                      faddp st(1),st
                      fsqrt
                      ftst
                      fnstsw ax
                      test ah,44h
                      jp normalise
                      fstp st(0)
                      fld one
                      fstp nx
                      fld y
                      fstp ny
                      jmp normDone
                  normalise:
                      fdivr one
                      fld st(0)
                      fmul st,st(3)
                      fstp nx
                      fmul st,st(1)
                      fstp ny
                  normDone:
                      fstp st(0)
                      fstp st(0) } CAM_END return O2(nx,ny); }
// 0x51F5DD..0x51F617
__declspec(noinline) inline Out ExeCiney(const A& a) {
    float outX{}, outY{}, sine{}; const float inputX=a.f[0], inputY=a.f[1], rotation=a.f[2]; const uintptr_t fn=std::bit_cast<uintptr_t>(&TestAtan);
    CAM_BEGIN __asm { push inputY
                      push inputX
                      call fn
                      add esp,8
                      fadd rotation
                      fld st(0)
                      fcos
                      fxch st(1)
                      fsin
                      fstp sine
                      fadd inputX
                      fstp outX
                      fld sine
                      fadd inputY
                      fstp outY } CAM_END return O2(outX,outY); }
// 0x5152A9..0x5152BD (resta) y 0x515463..0x51546C (suma)
__declspec(noinline) inline Out ExeAtanAdd(const A& a, bool subtract) {
    float r{}; const float x=a.f[0], y=a.f[1], constant=a.f[2]; const uintptr_t fn=std::bit_cast<uintptr_t>(&TestAtan);
    CAM_BEGIN
    if(subtract) { __asm { push y
                           push x
                           call fn
                           add esp,8
                           fsub constant
                           fstp r } }
    else { __asm { push y
                   push x
                   call fn
                   add esp,8
                   fadd constant
                   fstp r } }
    CAM_END return O1(r); }
// 0x52A87A..0x52A892: call GetHeading (ST0 sin redondear); fsub pi/2
__declspec(noinline) inline Out ExeHeading(const A& a) {
    float r{}; const float subtrahend=a.f[0]; const uintptr_t fn=std::bit_cast<uintptr_t>(&TestHeading);
    CAM_BEGIN __asm { xor ecx,ecx
                      xor edx,edx
                      call fn
                      fsub subtrahend
                      fstp r } CAM_END return O1(r); }
// 0x52C8C2..0x52C8CC y 0x52CA0F..0x52CA1B: fld value; fmul multiplier; call _ftol2 (0x821B40); solo EAX
__declspec(noinline) inline Out ExeTruncate(const A& a) {
    int32 r{}; const float value=a.f[0], multiplier=a.f[1];
    CAM_BEGIN __asm { fld value
                      fmul multiplier
                      mov eax,0x821B40
                      call eax
                      mov r,eax } CAM_END return O1(uint32(r)); }

// =====================================================================================================================
// Candidatos: ayudantes de X87Math llamados directamente
// =====================================================================================================================
inline Out DllCall(size_t f, const A& a) {
    switch(f) {
        case 0: return O1(CameraElapsedSecondsCpp(a.u[0]));
        case 1: return O1(CameraShakePercentCpp(a.f[0]));
        case 2: return O1(CameraCutSceneElapsedCpp(a.f[0],a.u[0]));
        case 3: { float s{}, e{}; CameraTimePlusCpp(a.u[0],a.f[0],&s,&e); return O2(s,e); }
        case 4: return O1(CameraAimHorizontalCpp(a.f[0],a.f[1]));
        case 5: return O1(CameraAimVerticalCpp(a.f[0],a.f[1],a.f[2]));
        case 6: return O1(CameraQuickAimPitchCpp(a.f[0],a.f[1],a.f[2],a.f[3]));
        case 7: return O1(CameraNearClipPedCpp(a.f[0],a.f[1],a.f[2],a.f[3],a.f[4]));
        case 8: return O1(uint32(CameraSphereInFrustumCpp(&a.f[0],a.f[3],&a.f[4],a.f[16],a.f[17])));
        case 9: return O1(CameraLinearBlendCpp(a.f[0],a.f[1],a.f[2],a.flag!=0));
        case 10: { float ratio{}; const bool running=CameraFovLerpRatioCpp(a.u[0],a.f[0],a.f[1],&ratio); return O2(float(running), running ? ratio : 0.f); }
        case 11: return O1(CameraPhaseCpp(a.u[0],a.u[1]));
        case 12: return O1(CameraEaseCpp(a.f[0],a.f[1],a.f[2],a.flag!=0));
        case 13: case 14: case 15: case 16: case 17: { float out[3]{}; CameraAdvanceCpp(out,&a.f[4],&a.f[1],a.f[0],int(f)-13); return O3(out); }
        case 18: { float out[3]{}; CameraAdvanceTowardCpp(out,&a.f[4],&a.f[1],a.f[0],1); return O3(out); }
        case 19: { float out[3]{}; CameraAdvanceTowardCpp(out,&a.f[4],&a.f[1],a.f[0],4); return O3(out); }
        case 20: return O1(CameraFovAdvanceCpp(a.f[0],a.f[1],a.f[2],a.flag!=0));
        case 21: return O1(CameraShakeForceCpp(a.f[0],a.u[0]));
        case 22: { const double v=CameraAccumulateDistanceCpp(&a.f[0],&a.f[3],a.f[6]); Out o; const uint64 bits=std::bit_cast<uint64>(v); o.v[0]=uint32(bits); o.v[1]=uint32(bits>>32); return o; }
        case 23: return O1(CameraAverageDistanceCpp(double(a.f[0]),a.i[0]));
        case 24: return O1(CameraDotYZXCpp(&a.f[0],&a.f[3]));
        case 25: { float t[4]{}; CameraFrustumTermsCpp(a.f[0],a.i[0],a.i[1],t); return O4(t); }
        case 26: { float nx{}, ny{}; CameraFrontNormCpp(a.f[0],a.f[1],&nx,&ny); return O2(nx,ny); }
        case 27: { float x{}, y{}; CameraCineyRotatedCpp(&TestAtan,a.f[0],a.f[1],a.f[2],&x,&y); return O2(x,y); }
        case 28: return O1(CameraAtanPlusCpp(&TestAtan,a.f[0],a.f[1],a.f[2]));
        case 29: return O1(CameraAtanMinusCpp(&TestAtan,a.f[0],a.f[1],a.f[2]));
        case 30: return O1(CameraHeadingMinusCpp(&TestHeading,nullptr,a.f[0]));
        default: return O1(uint32(CameraTruncateProductCpp(a.f[0],a.f[1])));
    }
}
inline Out ExeCall(size_t f, const A& a) {
    switch(f) {
        case 0: return ExeElapsed(a);       case 1: return ExeShakePercent(a);   case 2: return ExeCutScene(a);
        case 3: return ExeTimePlus(a);      case 4: return ExeAimHorizontal(a);  case 5: return ExeAimVertical(a);
        case 6: return ExeQuickAim(a);      case 7: return ExeNearClip(a);       case 8: return ExeSphere(a);
        case 9: return ExeLinear(a);        case 10: return ExeFovLerp(a);       case 11: return ExePhase(a);
        case 12: return ExeEase(a);
        case 13: case 14: case 15: case 16: case 17: return ExeAdvance(a,int(f)-13);
        case 18: return ExeToward(a,1);     case 19: return ExeToward(a,4);
        case 20: return ExeFovAdvance(a);   case 21: return ExeShakeForce(a);    case 22: return ExeAccumulate(a);
        case 23: return ExeAverage(a);      case 24: return ExeDot(a);           case 25: return ExeTerms(a);
        case 26: return ExeFrontNorm(a);    case 27: return ExeCiney(a);         case 28: return ExeAtanAdd(a,false);
        case 29: return ExeAtanAdd(a,true); case 30: return ExeHeading(a);       default: return ExeTruncate(a);
    }
}
// fptan con |x| >= 2^63 (finito) pone C2 y NO empuja 1.0: la pila x87 queda desequilibrada tambien en el EXE (comportamiento de la CPU, fuera de dominio:
// un fov real jamas llega ahi). AimHorizontal, AimVertical y QuickAimPitch calculan fptan(fov * 0.5f * kToRad); los fixtures con fov finito y de magnitud
// FLT_MAX o +-1e30 daban 82/2048 desequilibrios identicos en los dos lados. Esos fixtures no son validos para el par: se sustituye el fov por 1.0f.
// NaN e infinitos se conservan (ahi fptan si empuja). El test de finito usa el exponente en bits (el banco puede compilarse con /fp:fast).
inline void KeepFovTanArgInRange(float& fov) {
    const double argument = double(fov) * double(kHalf) * double(kToRad);
    const uint64 bits = std::bit_cast<uint64>(argument);
    const bool finite = ((bits >> 52) & 0x7FFu) != 0x7FFu;
    if(finite && (bits << 1) >= (std::bit_cast<uint64>(9223372036854775808.0) << 1)) fov = 1.0f;
}
// Argumentos por funcion (solo se rellenan los que usa la funcion; el resto queda a cero)
inline void Fill(size_t f, unsigned fx, A& a) {
    a.flag=fx&1u;
    switch(f) {
        case 0: case 21: a.u[0]=GenTime(fx,0); if(f==21) a.f[0]=Gen(Free,fx,0); break;
        case 1: a.f[0]=Gen(Free,fx,0); break;
        case 2: a.f[0]=Gen(Free,fx,0); a.u[0]=GenTime(fx,0); break;
        case 3: a.u[0]=GenTime(fx,0); a.f[0]=Gen(Free,fx,0); break;
        case 4: a.f[0]=Gen(Fov,fx,0); KeepFovTanArgInRange(a.f[0]); a.f[1]=Gen(Unit01,fx,1); break;
        case 5: a.f[0]=Gen(Fov,fx,0); KeepFovTanArgInRange(a.f[0]); a.f[1]=Gen(Unit01,fx,1); a.f[2]=Gen(Pos,fx,2); break;
        case 6: a.f[0]=Gen(Fov,fx,0); KeepFovTanArgInRange(a.f[0]); a.f[1]=Gen(Angle,fx,1); a.f[2]=Gen(Unit01,fx,2); a.f[3]=Gen(Pos,fx,3); break;
        case 7: for(int k=0;k<5;++k) a.f[k]=Gen(k==0?Pos:Free,fx,k); break;
        case 8: {
            if(fx%4u==1u) { // familia con aceptaciones: punto cercano, radio 2, near 0.5, far 40, normales unitarias del frustum
                Rng r(fx*7u+5u); const float s=0.70710677f; const float n[12]{s,-s,0,-s,-s,0,0,-s,-s,0,-s,s};
                a.f[0]=r.uni(-10.f,10.f); a.f[1]=r.uni(0.f,45.f); a.f[2]=r.uni(-10.f,10.f); a.f[3]=r.uni(0.f,3.f);
                for(int k=0;k<12;++k) a.f[4+k]=n[k]; a.f[16]=0.5f; a.f[17]=40.f;
            } else {
                for(int k=0;k<18;++k) a.f[k]=Gen(k<3?Coord:k==3?Pos:(k<16?Unit:Free),fx,k);
            }
            break; }
        case 9: a.f[0]=Gen(Free,fx,0); a.f[1]=Gen(Free,fx,1); a.f[2]=Gen(Unit01,fx,2); break;
        case 10: a.u[0]=GenTime(fx,0); a.f[0]=Gen(Free,fx,1); a.f[1]=Gen(Free,fx,2);
                 if(fx%3u==0u) { a.f[0]=float(int32(a.u[0]))+float(fx%50u); a.f[1]=float(int32(a.u[0]))-float(fx%70u); }
                 break;
        case 11: a.u[0]=GenTime(fx,0); a.u[1]=GenTime(fx,1); break;
        case 12: a.f[0]=Gen(Unit01,fx,0); a.f[1]=Gen(Unit01,fx,1); a.f[2]=Gen(Unit01,fx,2); break;
        case 13: case 14: case 15: case 16: case 17: case 18: case 19:
            a.f[0]=Gen(Unit01,fx,0); for(int k=1;k<7;++k) a.f[k]=Gen(Free,fx,k); break;
        case 20: a.f[0]=Gen(Free,fx,0); a.f[1]=Gen(Free,fx,1); a.f[2]=Gen(Unit01,fx,2); break;
        case 22: for(int k=0;k<7;++k) a.f[k]=Gen(Free,fx,k); break;
        case 23: a.f[0]=Gen(Free,fx,0); a.i[0]=GenInt(fx,0); break;
        case 24: for(int k=0;k<6;++k) a.f[k]=Gen(Unit,fx,k); break;
        case 25: a.f[0]=Gen(Fov,fx,0); a.i[0]=GenInt(fx,0); a.i[1]=GenInt(fx,1); break;
        case 26: a.f[0]=Gen(Unit,fx,0); a.f[1]=Gen(Unit,fx,1); break;
        case 27: a.f[0]=Gen(Free,fx,0); a.f[1]=Gen(Free,fx,1); a.f[2]=Gen(Angle,fx,2); break;
        case 28: case 29: a.f[0]=Gen(Free,fx,0); a.f[1]=Gen(Free,fx,1); a.f[2]=Gen(Angle,fx,2); break;
        case 30: a.f[0]=Gen(Angle,fx,0); gHeadingValue=atan2(double(Gen(Free,fx,1)),double(Gen(Free,fx,2))); if(fx%5u==0u) gHeadingValue=double(std::bit_cast<float>(kPatterns[fx%kPatterns.size()])); break;
        default: a.f[0]=Gen(Free,fx,0); a.f[1]=(fx%4u==0u)?std::bit_cast<float>(0xC37F0000u):Gen(Free,fx,1); break;
    }
}
inline uintptr_t DllEntry(size_t f) {
    switch(f) {
        case 0: return std::bit_cast<uintptr_t>(&CameraElapsedSecondsCpp);   case 1: return std::bit_cast<uintptr_t>(&CameraShakePercentCpp);
        case 2: return std::bit_cast<uintptr_t>(&CameraCutSceneElapsedCpp);  case 3: return std::bit_cast<uintptr_t>(&CameraTimePlusCpp);
        case 4: return std::bit_cast<uintptr_t>(&CameraAimHorizontalCpp);    case 5: return std::bit_cast<uintptr_t>(&CameraAimVerticalCpp);
        case 6: return std::bit_cast<uintptr_t>(&CameraQuickAimPitchCpp);    case 7: return std::bit_cast<uintptr_t>(&CameraNearClipPedCpp);
        case 8: return std::bit_cast<uintptr_t>(&CameraSphereInFrustumCpp);  case 9: return std::bit_cast<uintptr_t>(&CameraLinearBlendCpp);
        case 10: return std::bit_cast<uintptr_t>(&CameraFovLerpRatioCpp);    case 11: return std::bit_cast<uintptr_t>(&CameraPhaseCpp);
        case 12: return std::bit_cast<uintptr_t>(&CameraEaseCpp);
        case 13: case 14: case 15: case 16: case 17: return std::bit_cast<uintptr_t>(&CameraAdvanceCpp);
        case 18: case 19: return std::bit_cast<uintptr_t>(&CameraAdvanceTowardCpp);
        case 20: return std::bit_cast<uintptr_t>(&CameraFovAdvanceCpp);      case 21: return std::bit_cast<uintptr_t>(&CameraShakeForceCpp);
        case 22: return std::bit_cast<uintptr_t>(&CameraAccumulateDistanceCpp); case 23: return std::bit_cast<uintptr_t>(&CameraAverageDistanceCpp);
        case 24: return std::bit_cast<uintptr_t>(&CameraDotYZXCpp);          case 25: return std::bit_cast<uintptr_t>(&CameraFrustumTermsCpp);
        case 26: return std::bit_cast<uintptr_t>(&CameraFrontNormCpp);       case 27: return std::bit_cast<uintptr_t>(&CameraCineyRotatedCpp);
        case 28: return std::bit_cast<uintptr_t>(&CameraAtanPlusCpp);        case 29: return std::bit_cast<uintptr_t>(&CameraAtanMinusCpp);
        case 30: return std::bit_cast<uintptr_t>(&CameraHeadingMinusCpp);    default: return std::bit_cast<uintptr_t>(&CameraTruncateProductCpp);
    }
}

using ErrnoFn = int*(__cdecl*)();

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppcamerax87stress1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppcamerax87stress1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppcamerax87stress1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    // Direcciones de la DLL (llamada directa) y del EXE vigiladas
    bool dllOutside=true; for(size_t f=0;f<fns.size();++f) dllOutside&=DllEntry(f)>=0x1000000;
    const bool exeUnhooked=!ReversingProbe::JumpsToDll(0x821B40);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    entries.push_back({0x821B40,ReversingProbe::Bytes(reinterpret_cast<void*>(0x821B40),16)});
    for(size_t f=0;f<fns.size();++f) entries.push_back({DllEntry(f),ReversingProbe::Bytes(reinterpret_cast<void*>(DllEntry(f)),16)});
    const unsigned savedFP=_controlfp(0,0);
    const double savedHeading=gHeadingValue;
    auto restore=[&]{ _clearfp(); _controlfp(savedFP,_MCW_PC|_MCW_RC); };
    auto restored=[&]{
        bool ok=_controlfp(0,0)==savedFP;
        for(auto& [at,bytes]:entries) ok&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
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
            A a{}; Fill(f,fixture,a);
            auto run=[&](bool candidate) -> json {
                restore();
                gStack=true; gRegs=true;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                const uint16 topBefore=Top();
                const Out out=candidate ? DllCall(f,a) : ExeCall(f,a);
                const uint16 topAfter=Top();
                const unsigned fpAfter=_controlfp(0,0);
                const bool stackOK=gStack, regsOK=gRegs;
                restore();
                const bool protectedOK=(fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                const bool routeOK = candidate ? DllEntry(f)>=0x1000000 : true;
                return json{{"route_ok",routeOK},{"stack_restored",stackOK},{"nonvolatile_registers_preserved",regsOK},{"x87_stack_balanced",topBefore==topAfter},
                            {"protected_state_unchanged",protectedOK},{"bits",json::array({Hex(out.v[0]),Hex(out.v[1]),Hex(out.v[2]),Hex(out.v[3])})}};
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true&&original["x87_stack_balanced"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            json args=json::array(); for(int k=0;k<24;++k) args.push_back(Hex(Bits(a.f[k])));
            args.push_back(Hex(a.u[0])); args.push_back(Hex(a.u[1])); args.push_back(Hex(uint32(a.i[0]))); args.push_back(Hex(uint32(a.i[1]))); args.push_back(Hex(a.flag));
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},{"args",args},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",2048},{"failures",failures}});
    }
    gHeadingValue=savedHeading;
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
