#pragma once
// Banco dentro del motor (escrito sin compilar ni ejecutar): ayudantes x87 con que Cam.cpp sustituye su ensamblador (X87Math.cpp: Cam*Cpp y los reutilizados
// TrigSimpleSinCpp/CosCpp, TrigAtan2Cpp, Atan2MinusCpp, IntRatioCpp, CnvDecayPowCpp, CnvAsinFloatCpp).
// El "original" es el bloque __asm EXACTO que Cam.cpp tenia antes de la conversion (adaptador de ensamblador DENTRO del banco; _CIpow 0x822130 y _CIasin 0x821E70 se
// llaman por su direccion en el EXE) y el candidato es el ayudante de la DLL llamado directamente. Sin hooks ni colaboradores simulados.
// Complementa a los bancos Cam*355 (pares de motor con fixtures uniformes): aqui NaN con distinta carga/signo, sNaN, +-inf, +-0, subnormales, +-1e30/1e-30, FLT_MAX.
// No cubre CamMouseFinalDistanceCpp (llama a RwCameraSetNearClipPlane con la camara real; sigue en CamMouse355) ni los dos splines de ruta (DependenciesWorldWaterPath6).
// CamShootingFovCpp y CamFollowCarBetaCorrectionCpp necesitan punteros/desplazamientos validos en el buffer: Fixup() los coloca.
#include <fstream>
#include <float.h>
#include <bit>
#include <cstring>
#include <initializer_list>
#include <string_view>
#include "Cam.h"
#include "General.h"
#include "X87Math.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppCamX87Stress1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline constexpr int kCases = 512;
inline const std::array<Fn,84> fns{{
    {"Cam","CamTwoPlayerAimTanCpp",0,0,-1,kCases},
    {"Cam","CamLookBehindPedSideCpp",0,0,-1,kCases},
    {"Cam","CamLookSideOrbitCpp",0,0,-1,kCases},
    {"Cam","CamRotInFrontCarOrbitCpp",0,0,-1,kCases},
    {"Cam","CamVelocityInverseStepCpp",0,0,-1,kCases},
    {"Cam","CamVehicleTargetSpeedCpp",0,0,-1,kCases},
    {"Cam","CamVehicleSmoothedSpeedCpp",0,0,-1,kCases},
    {"Cam","CamPowerComplementCpp",0,0,-1,kCases},
    {"Cam","CamDeadWaveCpp",0,0,-1,kCases},
    {"Cam","CamDeadUnsignedElapsedCpp",0,0,-1,kCases},
    {"Cam","CamDeadCosineCpp",0,0,-1,kCases},
    {"Cam","CamFirstPersonPedFrontCpp",0,0,-1,kCases},
    {"Cam","CamAimOffsetsCpp",0,0,-1,kCases},
    {"Cam","CamAimTargetAnglesCpp",0,0,-1,kCases},
    {"Cam","CamAimProjectionCpp",0,0,-1,kCases},
    {"Cam","CamAimMeleeBlendCpp",0,0,-1,kCases},
    {"Cam","CamAimMouseAnglesCpp",0,0,-1,kCases},
    {"Cam","CamAimVehicleResetPitchCpp",0,0,-1,kCases},
    {"Cam","CamAttachedUpCpp",0,0,-1,kCases},
    {"Cam","CamDirectionFromAnglesCpp",0,0,-1,kCases},
    {"Cam","CamShootingSourceZCpp",0,0,-1,kCases},
    {"Cam","CamShootingOrbitCpp",0,0,-1,kCases},
    {"Cam","CamShootingTiltCpp",0,0,-1,kCases},
    {"Cam","CamShootingFovCpp",0,0,-1,kCases},
    {"Cam","CamStoredHeadingMinusHalfPiCpp",0,0,-1,kCases},
    {"Cam","CamTwoCarsBlendWeightCpp",0,0,-1,kCases},
    {"Cam","CamTwoCarsBlendCpp",0,0,-1,kCases},
    {"Cam","CamTwoCarsSourcesCpp",0,0,-1,kCases},
    {"Cam","CamEaseCpp",0,0,-1,kCases},
    {"Cam","CamBirdyLampHeightCpp",0,0,-1,kCases},
    {"Cam","CamManFovCpp",0,0,-1,kCases},
    {"Cam","CamHeliInitialFovCpp",0,0,-1,kCases},
    {"Cam","CamHeliReturnFovCpp",0,0,-1,kCases},
    {"Cam","CamHeliDistanceFovCpp",0,0,-1,kCases},
    {"Cam","CamPlaneOrbitWaveCpp",0,0,-1,kCases},
    {"Cam","CamPlaneSpotterRandomCpp",0,0,-1,kCases},
    {"Cam","CamFlyByUpCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarBetaBlendCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarHistoryCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarFinalHistoryCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarBetaCorrectionCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarActualDistanceCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarAutoAlphaCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarAlphaComplementCpp",0,0,-1,kCases},
    {"Cam","CamFollowCarAlphaBlendCpp",0,0,-1,kCases},
    {"Cam","CamMouseChangesStickCpp",0,0,-1,kCases},
    {"Cam","CamMouseChangesMouseCpp",0,0,-1,kCases},
    {"Cam","CamMouseRepeatedRadiusCpp",0,0,-1,kCases},
    {"Cam","CamMousePerpendicularDistanceCpp",0,0,-1,kCases},
    {"Cam","CamMouseSphereCenterCpp",0,0,-1,kCases},
    {"Cam","CamMouseInitialSourceCpp",0,0,-1,kCases},
    {"Cam","CamMouseFirstHitDistanceCpp",0,0,-1,kCases},
    {"Cam","CamMousePullSourceCpp",0,0,-1,kCases},
    {"Cam","CamMouseRecenterDeltaCpp",0,0,-1,kCases},
    {"Cam","CamFollowMouseDistanceCpp",0,0,-1,kCases},
    {"Cam","CamFollowMouseRadiusCpp",0,0,-1,kCases},
    {"Cam","CamRound4ScaleCpp",0,0,-1,kCases},
    {"Cam","CamRound4FinishCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedAutoAlphaCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedPitchCorrectionCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedDistanceCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedHistoryPitchCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedHistoryBaseCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedResetHistoryCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedHistoryCpp",0,0,-1,kCases},
    {"Cam","CamFollowPedHistoryHeadingCpp",0,0,-1,kCases},
    {"Cam","CamM16BlendCpp",0,0,-1,kCases},
    {"Cam","CamM16BumpCpp",0,0,-1,kCases},
    {"Cam","CamM16StickInputsCpp",0,0,-1,kCases},
    {"Cam","CamM16MouseAnglesCpp",0,0,-1,kCases},
    {"Cam","CamM16SideRayCpp",0,0,-1,kCases},
    {"Cam","CamRocketFrontCpp",0,0,-1,kCases},
    {"Cam","CamRocketHeadingCpp",0,0,-1,kCases},
    {"Cam","CamWheelUpCpp",0,0,-1,kCases},
    {"Cam","Reuse_TrigSimpleSinCpp",0,0,-1,kCases},
    {"Cam","Reuse_TrigSimpleCosCpp",0,0,-1,kCases},
    {"Cam","Reuse_Atan2MinusCpp",0,0,-1,kCases},
    {"Cam","Reuse_TrigAtan2Cpp",0,0,-1,kCases},
    {"Cam","Reuse_Atan2MinusCpp_neg",0,0,-1,kCases},
    {"Cam","Reuse_CnvDecayPowCpp",0,0,-1,kCases},
    {"Cam","Reuse_Atan2MinusCpp_const",0,0,-1,kCases},
    {"Cam","Reuse_CnvDecayPowCpp_const",0,0,-1,kCases},
    {"Cam","Reuse_IntRatioCpp",0,0,-1,kCases},
    {"Cam","Reuse_CnvAsinFloatCpp",0,0,-1,kCases},
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
// Argumento j del fixture: 3 de cada 8 fixtures solo patrones; el resto uniforme (escala 0.3, 1.5, 4, 100 o 3000) con ~25% de patrones.
inline float GenF(uint32 fixture, uint32 j) {
    Rng r(fixture*32u+j+1u);
    const bool onlyPatterns = (fixture%8u)<3u;
    if(onlyPatterns || r.next()%4u==0u) return std::bit_cast<float>(kPatterns[r.next()%kPatterns.size()]);
    static constexpr float scales[]{0.3f,1.5f,4.f,100.f,3000.f};
    const float s = scales[r.next()%5u];
    return r.uni(-s,s);
}
inline int GenI(uint32 fixture, uint32 j) {
    Rng r(fixture*32u+j+777u);
    static constexpr int common[]{0,1,-1,2,5,10,100,1000,32767,-1000,0x7FFFFFFF,int(0x80000000u)};
    switch(r.next()%3u) {
        case 0: return common[r.next()%12u];
        case 1: return int(r.next()%5000u);
        default: return int(r.next());
    }
}
struct Fx {
    float fv[24]; int iv[24]; float out[24]; unsigned adr[24];
    alignas(16) uint8 buf[8][0x800];
};
// spec: f = float, i = int, o = float de salida (entra con valor aleatorio), u = buffer de 2 KB con floats de estres, r = desplazamiento de fila (0,28,56,84),
// p = puntero a CGeneral::GetATanOfXY.
inline void Fill(Fx& f, const char* spec, uint32 fixture) {
    std::memset(&f,0,sizeof(f));
    int buffer = 0;
    for(uint32 j=0; spec[j]; ++j) {
        switch(spec[j]) {
            case 'f': f.fv[j]=GenF(fixture,j); break;
            case 'i': f.iv[j]=GenI(fixture,j); break;
            case 'o': f.out[j]=GenF(fixture,j); break;
            case 'r': f.adr[j]=28u*(Rng(fixture*32u+j+99u).next()%4u); break;
            case 'p': f.adr[j]=static_cast<unsigned>(std::bit_cast<uintptr_t>(&CGeneral::GetATanOfXY)); break;
            default: {
                auto* words = reinterpret_cast<float*>(f.buf[buffer]);
                for(uint32 w=0; w<0x800/4; ++w) words[w]=GenF(fixture*3u+uint32(buffer),j*997u+w);
                f.adr[j]=static_cast<unsigned>(reinterpret_cast<uintptr_t>(f.buf[buffer]));
                ++buffer; break;
            }
        }
    }
}
// Campos del buffer que el EXE trata como puntero o como desplazamiento de fila (no admiten datos arbitrarios).
// fptan con |x| >= 2^63 (finito) pone C2 y NO empuja 1.0: el original deja la pila x87 desequilibrada (comportamiento de la CPU, fuera de
// dominio: fov*escala jamas llega ahi). Esos fixtures no son validos para el par; se sustituyen los factores por 1.0f hasta que el
// argumento (producto en double, sin desbordar) quede por debajo de 2^63. NaN e infinitos se conservan (ahi fptan si empuja).
// El test de finito usa el exponente en bits (el banco puede compilarse con /fp:fast).
inline void KeepTanArgInRange(Fx& f, std::initializer_list<int> idx, double constant) {
    for(int i:idx) {
        double p=constant; for(int k:idx) p*=double(f.fv[k]);
        const uint64_t pb=std::bit_cast<uint64_t>(p);
        const bool finite=((pb>>52)&0x7FFu)!=0x7FFu;
        if(!(finite && (pb<<1) >= (std::bit_cast<uint64_t>(9223372036854775808.0)<<1))) return;
        f.fv[i]=1.0f;
    }
}
inline void Fixup(Fx& f, const char* name, uint32 fixture) {
    const std::string_view n{name};
    if(n=="CamTwoPlayerAimTanCpp") KeepTanArgInRange(f,{0,1},1.0);
    else if(n=="CamMouseRepeatedRadiusCpp"||n=="CamFollowMouseRadiusCpp") KeepTanArgInRange(f,{0,1,2},1.0);
    else if(n=="CamAimOffsetsCpp") KeepTanArgInRange(f,{0},double(*reinterpret_cast<const float*>(0x858B8C))*double(*reinterpret_cast<const float*>(0x8595EC)));
    if(n=="CamShootingFovCpp") { // vehiculo = buf[1]: +0x14 puntero a la matriz (buf[7]), +0x594 subclase
        auto* matrix = reinterpret_cast<float*>(f.buf[7]);
        for(uint32 w=0; w<0x800/4; ++w) matrix[w]=GenF(fixture*5u+3u,w);
        *reinterpret_cast<unsigned*>(f.buf[1]+0x14)=static_cast<unsigned>(reinterpret_cast<uintptr_t>(f.buf[7]));
        static constexpr unsigned subclasses[]{0,9,3,5,1,2};
        *reinterpret_cast<unsigned*>(f.buf[1]+0x594)=subclasses[Rng(fixture+5u).next()%6u];
    } else if(n=="CamFollowCarBetaCorrectionCpp") { // scratch = buf[2]: +0x34 desplazamiento de fila (estado*28)
        static constexpr unsigned rows[]{0,28,56,84};
        *reinterpret_cast<unsigned*>(f.buf[2]+0x34)=rows[Rng(fixture+7u).next()%4u];
    }
}
inline uint64_t Digest(const void* data,size_t n){ uint64_t h=14695981039346656037ull; const auto* p=static_cast<const uint8*>(data); for(size_t i=0;i<n;++i) h=(h^p[i])*1099511628211ull; return h; }
inline uint16 Top(){ uint16 v{}; __asm{ fnstsw v } return v&0x3800; }

inline bool gStack{true}, gRegs{true};
// Cada adaptador mide ESP antes y despues. Los registros no volatiles (EBX/ESI/EDI/EBP) se miden FUERA del adaptador, en el bucle de
// Update() alrededor de la llamada: el compilador guarda/restaura en el prologo/epilogo los que el __asm del original toca (como en
// Cam.cpp), de modo que dentro del adaptador (antes del epilogo) siempre parecen modificados y daban un falso 512/512.
#define CAMB_BEGIN uintptr_t sp0{},sp1{}; \
    __asm { mov sp0,esp }
#define CAMB_END __asm { mov sp1,esp } \
    gStack&=(sp0==sp1);

// --- Bloques __asm originales de Cam.cpp (adaptadores) y llamadas a la DLL ---
#pragma warning(push)
#pragma warning(disable: 4731) // ebp modificado por el ensamblador en linea (adaptadores que usan EBP como registro)

// CamTwoPlayerAimTanCpp  (Cam.cpp antes de la conversion: bloque de la linea 430)
__declspec(noinline) inline void Exe0(Fx& f) {
    float fov = f.fv[0];
    float fovScale = f.fv[1];
    float aspect = f.fv[2];
    float tanOverAspect = f.out[3];
    float tanRounded = f.out[4];
    CAMB_BEGIN
        __asm {
            fld   dword ptr [fov]
            fmul  dword ptr [fovScale]
            fptan
            fstp  st(0)
            fld   dword ptr [aspect]
            fdivr st(0), st(1)
            fstp  dword ptr [tanOverAspect]
            fstp  dword ptr [tanRounded]
        }
    CAMB_END
    f.out[3] = tanOverAspect;
    f.out[4] = tanRounded;
}
__declspec(noinline) inline void Dll0(Fx& f) { CAMB_BEGIN CamTwoPlayerAimTanCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3], &f.out[4]); CAMB_END }
// CamLookBehindPedSideCpp  (Cam.cpp antes de la conversion: bloque de la linea 542)
__declspec(noinline) inline void Exe1(Fx& f) {
    float heading = f.fv[0];
    float x = f.out[1];
    float y = f.out[2];
    CAMB_BEGIN
        __asm { fld heading
            fcos
            fchs
            fstp x
            fld heading
            fsin
            fchs
            fstp y
        }
    CAMB_END
    f.out[1] = x;
    f.out[2] = y;
}
__declspec(noinline) inline void Dll1(Fx& f) { CAMB_BEGIN CamLookBehindPedSideCpp(f.fv[0], &f.out[1], &f.out[2]); CAMB_END }
// CamLookSideOrbitCpp  (Cam.cpp antes de la conversion: bloque de la linea 574)
__declspec(noinline) inline void Exe2(Fx& f) {
    float directionY = f.fv[0];
    float directionX = f.fv[1];
    unsigned atanFunction = f.adr[2];
    float sign = f.fv[3];
    float halfPi = f.fv[4];
    float distance = f.fv[5];
    float targetX = f.fv[6];
    float outX = f.out[7];
    float targetY = f.fv[8];
    float outY = f.out[9];
    CAMB_BEGIN
    __asm {
        push directionY
        push directionX
        call atanFunction
        add esp,8
        fld sign
        fmul halfPi
        faddp st(1),st(0)
        fld st(0)
        fcos
        fmul distance
        fadd targetX
        fstp outX
        fsin
        fmul distance
        fadd targetY
        fstp outY
    }
    CAMB_END
    f.out[7] = outX;
    f.out[9] = outY;
}
__declspec(noinline) inline void Dll2(Fx& f) { CAMB_BEGIN CamLookSideOrbitCpp(f.fv[0], f.fv[1], f.adr[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], &f.out[7], f.fv[8], &f.out[9]); CAMB_END }
// CamRotInFrontCarOrbitCpp  (Cam.cpp antes de la conversion: bloque de la linea 666)
__declspec(noinline) inline void Exe3(Fx& f) {
    float angle = f.fv[0];
    float distance = f.fv[1];
    float baseX = f.fv[2];
    float resultX = f.out[3];
    float baseY = f.fv[4];
    float resultY = f.out[5];
    CAMB_BEGIN
        __asm { fld angle
            fcos
            fmul distance
            fadd baseX
            fstp resultX
            fld angle
            fsin
            fmul distance
            fadd baseY
            fstp resultY
        }
    CAMB_END
    f.out[3] = resultX;
    f.out[5] = resultY;
}
__declspec(noinline) inline void Dll3(Fx& f) { CAMB_BEGIN CamRotInFrontCarOrbitCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3], f.fv[4], &f.out[5]); CAMB_END }
// CamVelocityInverseStepCpp  (Cam.cpp antes de la conversion: bloque de la linea 718)
__declspec(noinline) inline void Exe4(Fx& f) {
    float one = f.fv[0];
    float denominator = f.fv[1];
    float reciprocal = f.out[2];
    CAMB_BEGIN
    __asm {
        fld one
        fdiv denominator
        fstp reciprocal
    }
    CAMB_END
    f.out[2] = reciprocal;
}
__declspec(noinline) inline void Dll4(Fx& f) { CAMB_BEGIN CamVelocityInverseStepCpp(f.fv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamVehicleTargetSpeedCpp  (Cam.cpp antes de la conversion: bloque de la linea 729)
__declspec(noinline) inline void Exe5(Fx& f) {
    float forwardY = f.out[0];
    float forwardX = f.out[1];
    float zero = f.fv[2];
    float one = f.fv[3];
    float velocityX = f.fv[4];
    float velocityY = f.fv[5];
    float positiveScale = f.fv[6];
    float negativeScale = f.fv[7];
    float half = f.fv[8];
    float result = f.out[9];
    CAMB_BEGIN
    __asm {
        fld forwardY
        fmul forwardY
        fld forwardX
        fmul forwardX
        faddp st(1),st(0)
        fsqrt
        fcom zero
        fnstsw ax
        test ah,44h
        jnp camVehicleSkipNormalise
        fdivr one
        fld forwardX
        fmul st(0),st(1)
        fstp forwardX
        fmul forwardY
        fstp forwardY
        jmp camVehicleProducts
    camVehicleSkipNormalise:
        fstp st(0)
    camVehicleProducts:
        fld forwardX
        fmul velocityX
        fld forwardY
        fmul velocityY
        fld st(0)
        fadd st(0),st(2)
        fcomp zero
        fld st(0)
        fmul st(0),st(1)
        fld st(2)
        fmul st(0),st(3)
        fnstsw ax
        faddp st(1),st(0)
        test ah,41h
        fsqrt
        fstp st(2)
        fstp st(0)
        jne camVehicleNegative
        fmul positiveScale
        fld one
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp camVehicleStore
        fstp st(0)
        fld one
        jmp camVehicleStore
    camVehicleNegative:
        fmul negativeScale
        fld half
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp camVehicleNegate
        fstp st(0)
        fld half
    camVehicleNegate:
        fchs
    camVehicleStore:
        fstp result
    }
    CAMB_END
    f.out[0] = forwardY;
    f.out[1] = forwardX;
    f.out[9] = result;
}
__declspec(noinline) inline void Dll5(Fx& f) { CAMB_BEGIN CamVehicleTargetSpeedCpp(&f.out[0], &f.out[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], f.fv[7], f.fv[8], &f.out[9]); CAMB_END }
// CamVehicleSmoothedSpeedCpp  (Cam.cpp antes de la conversion: bloque de la linea 796)
__declspec(noinline) inline void Exe6(Fx& f) {
    float target = f.fv[0];
    float targetWeight = f.fv[1];
    float previous = f.fv[2];
    float previousWeight = f.fv[3];
    float result = f.out[4];
    CAMB_BEGIN
    __asm {
        fld target
        fmul targetWeight
        fld previous
        fmul previousWeight
        faddp st(1),st(0)
        fstp result
    }
    CAMB_END
    f.out[4] = result;
}
__declspec(noinline) inline void Dll6(Fx& f) { CAMB_BEGIN CamVehicleSmoothedSpeedCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], &f.out[4]); CAMB_END }
// CamPowerComplementCpp  (Cam.cpp antes de la conversion: bloque de la linea 810)
__declspec(noinline) inline void Exe7(Fx& f) {
    float base = f.fv[0];
    float step = f.fv[1];
    float one = f.fv[2];
    float result = f.out[3];
    CAMB_BEGIN
    __asm {
        fld base
        fld step
        mov eax,00822130h
        call eax
        fsubr one
        fstp result
    }
    CAMB_END
    f.out[3] = result;
}
__declspec(noinline) inline void Dll7(Fx& f) { CAMB_BEGIN CamPowerComplementCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3]); CAMB_END }
// CamDeadWaveCpp  (Cam.cpp antes de la conversion: bloque de la linea 1028)
__declspec(noinline) inline void Exe8(Fx& f) {
    float elapsed = f.fv[0];
    float period = f.fv[1];
    float capped = f.fv[2];
    float factor = f.fv[3];
    float sx = f.fv[4];
    float amplitude = f.fv[5];
    float sy = f.fv[6];
    float scaledY = f.out[7];
    float y = f.out[8];
    float tx = f.fv[9];
    float x = f.out[10];
    CAMB_BEGIN
    __asm {
        fld elapsed
        fdiv period
        fsin
        fld capped
        fmul factor
        fmulp st(1),st(0)
        fld sx
        fmul amplitude
        fld sy
        fmul amplitude
        fstp scaledY
        fmul st(0),st(1)
        fld scaledY
        fmul st(0),st(2)
        fstp y
        fadd tx
        fstp x
        fstp st(0)
    }
    CAMB_END
    f.out[7] = scaledY;
    f.out[8] = y;
    f.out[10] = x;
}
__declspec(noinline) inline void Dll8(Fx& f) { CAMB_BEGIN CamDeadWaveCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], &f.out[7], &f.out[8], f.fv[9], &f.out[10]); CAMB_END }
// CamDeadUnsignedElapsedCpp  (Cam.cpp antes de la conversion: bloque de la linea 1071)
__declspec(noinline) inline void Exe9(Fx& f) {
    int elapsedInt = f.iv[0];
    float unsignedCorrection = f.fv[1];
    float elapsed = f.out[2];
    CAMB_BEGIN
        __asm { fild elapsedInt
            cmp elapsedInt,0
            jge dead_elapsed_positive
            fadd unsignedCorrection
        dead_elapsed_positive:
            fstp elapsed
        }
    CAMB_END
    f.out[2] = elapsed;
}
__declspec(noinline) inline void Dll9(Fx& f) { CAMB_BEGIN CamDeadUnsignedElapsedCpp(f.iv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamDeadCosineCpp  (Cam.cpp antes de la conversion: bloque de la linea 1084)
__declspec(noinline) inline void Exe10(Fx& f) {
    float elapsed = f.fv[0];
    float period = f.fv[1];
    float cosine = f.out[2];
    CAMB_BEGIN
        __asm { fld elapsed
            fdiv period
            fcos
            fstp cosine
        }
    CAMB_END
    f.out[2] = cosine;
}
__declspec(noinline) inline void Dll10(Fx& f) { CAMB_BEGIN CamDeadCosineCpp(f.fv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamFirstPersonPedFrontCpp  (Cam.cpp antes de la conversion: bloque de la linea 1173)
__declspec(noinline) inline void Exe11(Fx& f) {
    float pitch = f.fv[0];
    float yaw = f.fv[1];
    float sineY = f.out[2];
    float sineZ = f.out[3];
    float cosineX = f.out[4];
    CAMB_BEGIN
        __asm {
            fld pitch
            fcos
            fld yaw
            fcos
            fmul st, st(1)
            fld yaw
            fsin
            fmul st, st(2)
            fstp sineY
            fld pitch
            fsin
            fstp sineZ
            fmul dword ptr ds:[00858B3Ch]
            fstp cosineX
            fstp st(0)
        }
    CAMB_END
    f.out[2] = sineY;
    f.out[3] = sineZ;
    f.out[4] = cosineX;
}
__declspec(noinline) inline void Dll11(Fx& f) { CAMB_BEGIN CamFirstPersonPedFrontCpp(f.fv[0], f.fv[1], &f.out[2], &f.out[3], &f.out[4]); CAMB_END }
// CamAimOffsetsCpp  (Cam.cpp antes de la conversion: bloque de la linea 1409)
__declspec(noinline) inline void Exe12(Fx& f) {
    float fov = f.fv[0];
    float vResult = f.out[1];
    float hResult = f.out[2];
    CAMB_BEGIN
    __asm {
        fld fov
        fmul dword ptr ds:[00858B8Ch]
        fmul dword ptr ds:[008595ECh]
        fptan
        fstp st(0)
        fld dword ptr ds:[00B6EC14h]
        fsub dword ptr ds:[00858B8Ch]
        fadd st(0),st
        fmul st,st(1)
        fld1
        fpatan
        fstp vResult
        fld dword ptr ds:[00858B8Ch]
        fsub dword ptr ds:[00B6EC10h]
        fadd st(0),st
        fld dword ptr ds:[00858624h]
        fdiv dword ptr ds:[00C3EFA4h]
        fmulp st(1),st
        fmulp st(1),st
        fld1
        fpatan
        fstp hResult
    }
    CAMB_END
    f.out[1] = vResult;
    f.out[2] = hResult;
}
__declspec(noinline) inline void Dll12(Fx& f) { CAMB_BEGIN CamAimOffsetsCpp(f.fv[0], &f.out[1], &f.out[2]); CAMB_END }
// CamAimTargetAnglesCpp  (Cam.cpp antes de la conversion: bloque de la linea 1439)
__declspec(noinline) inline void Exe13(Fx& f) {
    float tx = f.fv[0];
    float fx = f.fv[1];
    float ty = f.fv[2];
    float fy = f.fv[3];
    float tz = f.fv[4];
    float fz = f.fv[5];
    float dz = f.out[6];
    float hResult = f.out[7];
    float fResult = f.out[8];
    float pResult = f.out[9];
    CAMB_BEGIN
    __asm {
        fld tx
        fsub fx
        fld ty
        fsub fy
        fld tz
        fsub fz
        fstp dz
        fld st(1)
        fchs
        fld st(1)
        fpatan
        fsub dword ptr ds:[00858FE4h]
        fstp hResult
        fld st(1)
        fmul st,st(2)
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fstp fResult
        fstp st(0)
        fstp st(0)
        fld dz
        fld fResult
        fsqrt
        fxch st(1)
        fxch st(1)
        fpatan
        fstp pResult
    }
    CAMB_END
    f.out[6] = dz;
    f.out[7] = hResult;
    f.out[8] = fResult;
    f.out[9] = pResult;
}
__declspec(noinline) inline void Dll13(Fx& f) { CAMB_BEGIN CamAimTargetAnglesCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], &f.out[6], &f.out[7], &f.out[8], &f.out[9]); CAMB_END }
// CamAimProjectionCpp  (Cam.cpp antes de la conversion: bloque de la linea 1478)
__declspec(noinline) inline void Exe14(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned scratch = f.adr[1];
    unsigned rowOffset = f.adr[2];
    CAMB_BEGIN
    __asm {
        push ebp
        mov esi,cam
        mov edi,scratch
        mov eax,rowOffset
        mov ebp,eax
        fld dword ptr [ebp+008CC4C0h]
        fld dword ptr [esi+000000ACh]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne aim_projection_00522955
        fld dword ptr [ebp+008CC4C8h]
        fmul dword ptr [esi+000000ACh]
        fld dword ptr ds:[00858FE4h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp aim_projection_00522951
        fstp st(0)
        fld dword ptr ds:[00858FE4h]
    aim_projection_00522951:
        fcos
        jmp aim_projection_0052295D
    aim_projection_00522955:
        fld dword ptr [esi+000000ACh]
        fcos
    aim_projection_0052295D:
        fmul dword ptr [ebp+008CC4C4h]
        lea ebp,[esi+00000190h]
        mov edx,ebp
        faddp st(1),st
        lea ebx,[esi+0000019Ch]
        fld dword ptr [esi+000000ACh]
        fcos
        fld dword ptr [esi+000000BCh]
        fcos
        fmul st,st(1)
        fchs
        fstp dword ptr [edi+30h]
        mov eax,dword ptr [edi+30h]
        fld dword ptr [esi+000000BCh]
        mov dword ptr [edx],eax
        fsin
        fmul st,st(1)
        fchs
        fstp dword ptr [edi+34h]
        mov ecx,dword ptr [edi+34h]
        mov dword ptr [edx+4],ecx
        fstp st(0)
        mov ecx,ebx
        fld dword ptr [esi+000000ACh]
        fsin
        fstp dword ptr [edi+38h]
        mov eax,dword ptr [edi+38h]
        fld st(0)
        mov dword ptr [edx+8],eax
        fmul dword ptr [ebp]
        fld st(1)
        fmul dword ptr [ebp+4]
        fstp dword ptr [edi+58h]
        fxch st(1)
        fmul dword ptr [ebp+8]
        fstp dword ptr [edi+5Ch]
        fld dword ptr [edi+3Ch]
        fsub st,st(1)
        fstp dword ptr [edi+30h]
        mov edx,dword ptr [edi+30h]
        mov dword ptr [ecx],edx
        fstp st(0)
        fld dword ptr [edi+40h]
        fsub dword ptr [edi+58h]
        fstp dword ptr [edi+34h]
        fld dword ptr [edi+44h]
        mov eax,dword ptr [edi+34h]
        fsub dword ptr [edi+5Ch]
        mov dword ptr [ecx+4],eax
        lea eax,[edi+3Ch]
        fstp dword ptr [edi+3Ch]
        mov edx,dword ptr [edi+3Ch]
        mov dword ptr [ecx+8],edx
        pop ebp
    }
    CAMB_END
}
__declspec(noinline) inline void Dll14(Fx& f) { CAMB_BEGIN CamAimProjectionCpp(f.adr[0], f.adr[1], f.adr[2]); CAMB_END }
// CamAimMeleeBlendCpp  (Cam.cpp antes de la conversion: bloque de la linea 1568)
__declspec(noinline) inline void Exe15(Fx& f) {
    unsigned scratch = f.adr[0];
    CAMB_BEGIN
    __asm {
        mov ebx,scratch
        fld dword ptr ds:[00862F30h]
        fld dword ptr ds:[00B7CB5Ch]
        mov eax,00822130h
        call eax
        fld dword ptr ds:[00858624h]
        fsub st,st(1)
        fld dword ptr [ebx+20h]
        fmul st,st(1)
        fld dword ptr ds:[00B70100h]
        fmul st,st(3)
        faddp st(1),st
        fstp dword ptr ds:[00B70100h]
        fld dword ptr [ebx+14h]
        fmul st,st(1)
        fld dword ptr ds:[00B700FCh]
        fmul st,st(3)
        faddp st(1),st
        fstp dword ptr ds:[00B700FCh]
        fmul dword ptr [ebx+28h]
        fld dword ptr ds:[00B700F4h]
        fmul st,st(2)
        faddp st(1),st
        fstp dword ptr ds:[00B700F4h]
        fstp st(0)
    }
    CAMB_END
}
__declspec(noinline) inline void Dll15(Fx& f) { CAMB_BEGIN CamAimMeleeBlendCpp(f.adr[0]); CAMB_END }
// CamAimMouseAnglesCpp  (Cam.cpp antes de la conversion: bloque de la linea 1597)
__declspec(noinline) inline void Exe16(Fx& f) {
    unsigned cam = f.adr[0];
    CAMB_BEGIN
    __asm {
        mov eax,cam
        fld dword ptr [eax+0B4h]
        fmul dword ptr ds:[0085A5C4h]
        fld dword ptr ds:[00B73428h]
        fmul dword ptr ds:[00858B90h]
        fmul st(0),st(1)
        fmul dword ptr ds:[00B6EC18h]
        fld dword ptr ds:[00B73424h]
        fmul dword ptr ds:[008631B0h]
        fmul st(0),st(2)
        fmul dword ptr ds:[00B6EC1Ch]
        fadd dword ptr [eax+0BCh]
        fstp dword ptr [eax+0BCh]
        fadd dword ptr [eax+0ACh]
        fstp dword ptr [eax+0ACh]
        fstp st(0)
    }
    CAMB_END
}
__declspec(noinline) inline void Dll16(Fx& f) { CAMB_BEGIN CamAimMouseAnglesCpp(f.adr[0]); CAMB_END }
// CamAimVehicleResetPitchCpp  (Cam.cpp antes de la conversion: bloque de la linea 1619)
__declspec(noinline) inline void Exe17(Fx& f) {
    float clampedZ = f.fv[0];
    float initialAlpha = f.fv[1];
    float result = f.out[2];
    CAMB_BEGIN
    __asm {
        fld clampedZ
        mov eax,00821E70h
        call eax
        fadd initialAlpha
        fstp result
    }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll17(Fx& f) { CAMB_BEGIN CamAimVehicleResetPitchCpp(f.fv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamAttachedUpCpp  (Cam.cpp antes de la conversion: bloque de la linea 1860)
__declspec(noinline) inline void Exe18(Fx& f) {
    float angle = f.fv[0];
    float sideX = f.fv[1];
    float sinX = f.out[2];
    float sideY = f.fv[3];
    float sinY = f.out[4];
    float sideZ = f.fv[5];
    float sinZ = f.out[6];
    float upX = f.fv[7];
    float cosX = f.out[8];
    float upY = f.fv[9];
    float cosY = f.out[10];
    float upZ = f.fv[11];
    float cosZ = f.out[12];
    CAMB_BEGIN
    __asm { fld angle
        fsin
        fld sideX
        fmul st(0),st(1)
        fstp sinX
        fld sideY
        fmul st(0),st(1)
        fstp sinY
        fmul sideZ
        fstp sinZ
        fld angle
        fcos
        fld upX
        fmul st(0),st(1)
        fstp cosX
        fld upY
        fmul st(0),st(1)
        fstp cosY
        fmul upZ
        fstp cosZ
    }
    CAMB_END
    f.out[2] = sinX;
    f.out[4] = sinY;
    f.out[6] = sinZ;
    f.out[8] = cosX;
    f.out[10] = cosY;
    f.out[12] = cosZ;
}
__declspec(noinline) inline void Dll18(Fx& f) { CAMB_BEGIN CamAttachedUpCpp(f.fv[0], f.fv[1], &f.out[2], f.fv[3], &f.out[4], f.fv[5], &f.out[6], f.fv[7], &f.out[8], f.fv[9], &f.out[10], f.fv[11], &f.out[12]); CAMB_END }
// CamDirectionFromAnglesCpp  (Cam.cpp antes de la conversion: bloque de la linea 1914)
__declspec(noinline) inline void Exe19(Fx& f) {
    float alpha = f.fv[0];
    float beta = f.fv[1];
    float directionX = f.out[2];
    float directionY = f.out[3];
    float directionZ = f.out[4];
    CAMB_BEGIN
    __asm {
        fld alpha
        fcos
        fld beta
        fcos
        fmul st(0),st(1)
        fchs
        fstp directionX
        fld beta
        fsin
        fmulp st(1),st(0)
        fchs
        fstp directionY
        fld alpha
        fsin
        fstp directionZ
    }
    CAMB_END
    f.out[2] = directionX;
    f.out[3] = directionY;
    f.out[4] = directionZ;
}
__declspec(noinline) inline void Dll19(Fx& f) { CAMB_BEGIN CamDirectionFromAnglesCpp(f.fv[0], f.fv[1], &f.out[2], &f.out[3], &f.out[4]); CAMB_END }
// CamShootingSourceZCpp  (Cam.cpp antes de la conversion: bloque de la linea 2070)
__declspec(noinline) inline void Exe20(Fx& f) {
    float alpha = f.fv[0];
    float radius = f.fv[1];
    float targetZ = f.fv[2];
    float result = f.out[3];
    CAMB_BEGIN
    __asm {
        fld alpha
        fsin
        fmul radius
        fsubr targetZ
        fstp result
    }
    CAMB_END
    f.out[3] = result;
}
__declspec(noinline) inline void Dll20(Fx& f) { CAMB_BEGIN CamShootingSourceZCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3]); CAMB_END }
// CamShootingOrbitCpp  (Cam.cpp antes de la conversion: bloque de la linea 2086)
__declspec(noinline) inline void Exe21(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned source = f.adr[1];
    unsigned scratch = f.adr[2];
    CAMB_BEGIN
    __asm {
        mov esi,cam
        mov edi,source
        mov ebx,scratch
        fld dword ptr [edi]
        fsub dword ptr [ebx+1Ch]
        lea eax,[esi+00000190h]
        fld dword ptr [esi+000001A0h]
        fsub dword ptr [ebx+20h]
        fld dword ptr [ebx+28h]
        fsin
        fld st(0)
        fld dword ptr [ebx+28h]
        fcos
        fstp dword ptr [ebx+10h]
        fmul st,st(2)
        fld dword ptr [ebx+10h]
        fmul st,st(4)
        faddp st(1),st
        fstp dword ptr [ebx+30h]
        fld dword ptr [ebx+10h]
        mov dword ptr [ebx+10h],eax
        fmul st,st(2)
        fxch st(1)
        fmul st,st(3)
        fsubp st(1),st
        fstp st(2)
        fstp st(0)
        fld dword ptr [ebx+30h]
        fadd dword ptr [ebx+1Ch]
        fstp dword ptr [edi]
        fadd dword ptr [ebx+20h]
        fstp dword ptr [esi+000001A0h]
    }
    CAMB_END
}
__declspec(noinline) inline void Dll21(Fx& f) { CAMB_BEGIN CamShootingOrbitCpp(f.adr[0], f.adr[1], f.adr[2]); CAMB_END }
// CamShootingTiltCpp  (Cam.cpp antes de la conversion: bloque de la linea 2125)
__declspec(noinline) inline void Exe22(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned vehicle = f.adr[1];
    unsigned scratch = f.adr[2];
    CAMB_BEGIN
    __asm {
        mov esi,cam
        mov ebx,vehicle
        mov edi,scratch
        fld dword ptr [ebx+4Ch]
        fld dword ptr [ebx+48h]
        fld dword ptr [ebx+44h]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fcomp dword ptr ds:[00858FC4h]
        fstp st(0)
        fnstsw ax
        fstp st(0)
        test ah,41h
        fstp st(0)
        jne shooting_tilt_end
        fld dword ptr [ebx+48h]
        mov eax,dword ptr [ebx+00000594h]
        cmp eax,3
        fld dword ptr [ebx+44h]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fstp st(2)
        fstp st(0)
        fsqrt
        fld dword ptr [ebx+4Ch]
        fxch st(1)
        fpatan
        fld dword ptr ds:[00862F7Ch]
        jne shooting_tilt_00519F81
        fstp st(0)
        fld dword ptr ds:[00862F88h]
    shooting_tilt_00519F81:
        fld dword ptr [ebx+4Ch]
        fld dword ptr [ebx+48h]
        fld dword ptr [ebx+44h]
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
        fsqrt
        fsub dword ptr ds:[00858C58h]
        fmulp st(1),st
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp shooting_tilt_00519FC1
        fstp st(0)
        fld dword ptr ds:[00858624h]
    shooting_tilt_00519FC1:
        fmulp st(1),st
        fadd dword ptr [edi+34h]
        fstp dword ptr [edi+34h]
        fld dword ptr ds:[00862F78h]
        fld dword ptr ds:[00B7CB5Ch]
        mov eax,000822130h
        call eax
        fld dword ptr ds:[00858624h]
        fsub st,st(1)
        fmul dword ptr [edi+34h]
        fxch st(1)
        fmul dword ptr [esi+000000ACh]
        faddp st(1),st
        fstp dword ptr [esi+000000ACh]
    shooting_tilt_end:
    }
    CAMB_END
}
__declspec(noinline) inline void Dll22(Fx& f) { CAMB_BEGIN CamShootingTiltCpp(f.adr[0], f.adr[1], f.adr[2]); CAMB_END }
// CamShootingFovCpp  (Cam.cpp antes de la conversion: bloque de la linea 2211)
__declspec(noinline) inline void Exe23(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned vehicle = f.adr[1];
    CAMB_BEGIN
    __asm {
        mov esi,cam
        mov ebx,vehicle
        mov eax,dword ptr [ebx+594h]
        test eax,eax
        je shooting_fov_005198DB
        cmp eax,9
        jne shooting_fov_0051992F
    shooting_fov_005198DB:
        mov ecx,dword ptr [ebx+14h]
        fld dword ptr [ebx+4Ch]
        fmul dword ptr [ecx+18h]
        fld dword ptr [ebx+48h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fld dword ptr [ebx+44h]
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fcomp dword ptr ds:[008CC540h]
        fnstsw ax
        test ah,41h
        jne shooting_fov_0051992F
        fld dword ptr [ebx+4Ch]
        fmul dword ptr [ecx+18h]
        fld dword ptr [ebx+48h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fld dword ptr [ecx+10h]
        fmul dword ptr [ebx+44h]
        faddp st(1),st
        fsub dword ptr ds:[008CC540h]
        fmul dword ptr ds:[00B7CB5Ch]
        fadd dword ptr [esi+000000B4h]
        fstp dword ptr [esi+000000B4h]
    shooting_fov_0051992F:
        fld dword ptr [esi+000000B4h]
        fcomp dword ptr ds:[00858CE0h]
        fnstsw ax
        test ah,41h
        jne shooting_fov_0051996F
        fld dword ptr ds:[008CC544h]
        fld dword ptr ds:[00B7CB5Ch]
        mov eax,000822130h
        call eax
        fld dword ptr [esi+000000B4h]
        fsub dword ptr ds:[00858CE0h]
        fmul st,st(1)
        fadd dword ptr ds:[00858CE0h]
        fstp dword ptr [esi+000000B4h]
        fstp st(0)
    shooting_fov_0051996F:
        fld dword ptr [esi+000000B4h]
        fcomp dword ptr ds:[00858628h]
        fnstsw ax
        test ah,41h
        jne shooting_fov_0051998E
        mov dword ptr [esi+000000B4h],42C80000h
        jmp shooting_fov_end
    shooting_fov_0051998E:
        fld dword ptr [esi+000000B4h]
        fcomp dword ptr ds:[00858CE0h]
        fnstsw ax
        test ah,5
        jp shooting_fov_end
        mov dword ptr [esi+000000B4h],428C0000h
    shooting_fov_end:
    }
    CAMB_END
}
__declspec(noinline) inline void Dll23(Fx& f) { CAMB_BEGIN CamShootingFovCpp(f.adr[0], f.adr[1]); CAMB_END }
// CamStoredHeadingMinusHalfPiCpp  (Cam.cpp antes de la conversion: bloque de la linea 2299)
__declspec(noinline) inline void Exe24(Fx& f) {
    float storedHeading = f.fv[0];
    float vehicleHeading = f.out[1];
    CAMB_BEGIN
        __asm {
            fld storedHeading
            fsub dword ptr ds:[00858FE4h]
            fstp vehicleHeading
        }
    CAMB_END
    f.out[1] = vehicleHeading;
}
__declspec(noinline) inline void Dll24(Fx& f) { CAMB_BEGIN CamStoredHeadingMinusHalfPiCpp(f.fv[0], &f.out[1]); CAMB_END }
// CamTwoCarsBlendWeightCpp  (Cam.cpp antes de la conversion: bloque de la linea 2405)
__declspec(noinline) inline void Exe25(Fx& f) {
    float blend = f.fv[0];
    float sine = f.out[1];
    float weight = f.out[2];
    CAMB_BEGIN
    __asm {
        fld blend
        fmul dword ptr ds:[00858CB8h]
        fld st(0)
        fsin
        fstp sine
        fcos
        fadd dword ptr ds:[00858624h]
        fmul dword ptr ds:[00858B8Ch]
        fstp weight
    }
    CAMB_END
    f.out[1] = sine;
    f.out[2] = weight;
}
__declspec(noinline) inline void Dll25(Fx& f) { CAMB_BEGIN CamTwoCarsBlendWeightCpp(f.fv[0], &f.out[1], &f.out[2]); CAMB_END }
// CamTwoCarsBlendCpp  (Cam.cpp antes de la conversion: bloque de la linea 2420)
__declspec(noinline) inline void Exe26(Fx& f) {
    unsigned scratch = f.adr[0];
    unsigned cam = f.adr[1];
    unsigned firstPos = f.adr[2];
    unsigned secondPos = f.adr[3];
    CAMB_BEGIN
    __asm {
        mov esi,scratch
        mov ebx,cam
        mov eax,firstPos
        mov ecx,secondPos
        fld dword ptr [eax] // 0051379F
        fsub dword ptr [ecx] // 005137A2
        fld dword ptr [eax+4] // 005137A4
        fsub dword ptr [ecx+4] // 005137A7
        fld dword ptr [eax+8] // 005137AA
        fsub dword ptr [ecx+8] // 005137AD
        fld st(2) // 005137B0
        fmulp st(3),st // 005137B2
        fld st(0) // 005137B4
        fmul st,st(1) // 005137B6
        faddp st(3),st // 005137B8
        fld st(1) // 005137BA
        fmul st,st(2) // 005137BC
        faddp st(3),st // 005137BE
        fxch st(2) // 005137C0
        fsqrt // 005137C2
        fstp st(2) // 005137C4
        fstp st(0) // 005137C6
        fstp dword ptr [esi+1Ch] // 005137C8
        fld dword ptr [esi+48h] // 005137CC
        fmul dword ptr [esi+64h] // 005137D0
        fst dword ptr [esi+3Ch] // 005137D4
        fld dword ptr [esi+48h] // 005137D8
        fmul dword ptr [esi+68h] // 005137DC
        fld st(0) // 005137E0
        fld dword ptr [esi+48h] // 005137E2
        fmul dword ptr [esi+6Ch] // 005137E6
        fst dword ptr [esi+00000094h] // 005137EA
        fld dword ptr [esi+3Ch] // 005137F1
        fmul dword ptr [esi+1Ch] // 005137F5
        fstp dword ptr [esi+10h] // 005137F9
        fxch st(1) // 005137FD
        fmul dword ptr [esi+1Ch] // 005137FF
        fstp dword ptr [esi+14h] // 00513803
        fmul dword ptr [esi+1Ch] // 00513807
        fld dword ptr [esi+10h] // 0051380B
        fmul dword ptr ds:[00858F34h] // 0051380F
        fld dword ptr [esi+14h] // 00513815
        fmul dword ptr ds:[00858F34h] // 00513819
        fstp dword ptr [esi+40h] // 0051381F
        fxch st(1) // 00513823
        fmul dword ptr ds:[00858F34h] // 00513825
        fstp dword ptr [esi+44h] // 0051382B
        fld dword ptr ds:[00858624h] // 0051382F
        fsub dword ptr [esi+2Ch] // 00513835
        fst dword ptr [esi+1Ch] // 00513839
        fld dword ptr [esi+30h] // 0051383D
        fmul st,st(1) // 00513841
        fld st(1) // 00513843
        fmul dword ptr [esi+34h] // 00513845
        fstp dword ptr [esi+5Ch] // 00513849
        fxch st(1) // 0051384D
        fmul dword ptr [esi+38h] // 0051384F
        fstp dword ptr [esi+60h] // 00513853
        fld dword ptr [esi+2Ch] // 00513857
        fmul dword ptr [esi+20h] // 0051385B
        fld dword ptr [esi+2Ch] // 0051385F
        fmul dword ptr [esi+24h] // 00513863
        fstp dword ptr [esi+34h] // 00513867
        fld dword ptr [esi+2Ch] // 0051386B
        fmul dword ptr [esi+28h] // 0051386F
        fstp dword ptr [esi+38h] // 00513873
        fadd st,st(1) // 00513877
        fstp dword ptr [esi+10h] // 00513879
        fstp st(0) // 0051387D
        fld dword ptr [esi+34h] // 0051387F
        fadd dword ptr [esi+5Ch] // 00513883
        fstp dword ptr [esi+14h] // 00513887
        fld dword ptr [esi+38h] // 0051388B
        lea eax,[ebx+0000019Ch] // 0051388F
        fadd dword ptr [esi+60h] // 00513895
        fstp dword ptr [esi+18h] // 00513899
        fld dword ptr [esi+10h] // 0051389D
        fsub st,st(1) // 005138A1
        fstp dword ptr [esi+20h] // 005138A3
        mov ecx,dword ptr [esi+20h] // 005138A7
        mov dword ptr [eax],ecx // 005138AB
        fstp st(0) // 005138AD
        fld dword ptr [esi+14h] // 005138AF
        fsub dword ptr [esi+40h] // 005138B3
        fstp dword ptr [esi+24h] // 005138B7
        fld dword ptr [esi+18h] // 005138BB
        mov edx,dword ptr [esi+24h] // 005138BF
        fsub dword ptr [esi+44h] // 005138C3
        mov dword ptr [eax+4],edx // 005138C7
        fstp dword ptr [esi+28h] // 005138CA
        fxch st(1) // 005138CE
        mov ecx,dword ptr [esi+28h] // 005138D0
        fstp dword ptr [esi+10h] // 005138D4
        mov dword ptr [eax+8],ecx // 005138D8
        fld dword ptr [esi+00000094h] // 005138DB
        fld dword ptr [esi+1Ch] // 005138E2
        fld dword ptr [esi+00000088h] // 005138E6
        fmul st,st(1) // 005138ED
        fld dword ptr [esi+0000008Ch] // 005138EF
        fmul st,st(2) // 005138F6
        fstp dword ptr [esi+5Ch] // 005138F8
        fld dword ptr [esi+00000090h] // 005138FC
        fmul st,st(2) // 00513903
        fstp dword ptr [esi+60h] // 00513905
        fld dword ptr [esi+7Ch] // 00513909
        fmul dword ptr [esi+2Ch] // 0051390D
        fld dword ptr [esi+00000080h] // 00513911
        fmul dword ptr [esi+2Ch] // 00513918
        fstp dword ptr [esi+40h] // 0051391C
        fld dword ptr [esi+00000084h] // 00513920
        fmul dword ptr [esi+2Ch] // 00513927
        fstp dword ptr [esi+44h] // 0051392B
        fadd st,st(1) // 0051392F
        fstp st(2) // 00513931
        fstp st(0) // 00513933
        fld dword ptr [esi+40h] // 00513935
        fadd dword ptr [esi+5Ch] // 00513939
        fld dword ptr [esi+44h] // 0051393D
        fadd dword ptr [esi+60h] // 00513941
        fstp dword ptr [esi+60h] // 00513945
        fld dword ptr ds:[00858624h] // 00513949
        fsub dword ptr [esi+48h] // 0051394F
        fstp dword ptr [esi+1Ch] // 00513953
        fxch st(1) // 00513957
        fmul dword ptr [esi+1Ch] // 00513959
        fstp dword ptr [esi+3Ch] // 0051395D
        fmul dword ptr [esi+1Ch] // 00513961
        fld dword ptr [esi+60h] // 00513965
        fmul dword ptr [esi+1Ch] // 00513969
        fstp dword ptr [esi+44h] // 0051396D
        fld dword ptr [esi+3Ch] // 00513971
        fadd dword ptr [esi+10h] // 00513975
        fstp dword ptr [esi+10h] // 00513979
        fadd st,st(2) // 0051397D
        fstp dword ptr [esi+14h] // 0051397F
        fld dword ptr [esi+44h] // 00513983
        fadd st,st(1) // 00513987
        mov eax,dword ptr [esi+10h] // 00513989
        fstp dword ptr [esi+18h] // 0051398D
        mov ecx,dword ptr [esi+14h] // 00513991
        lea edi,[ebx+00000190h] // 00513995
        fstp st(0) // 0051399B
        mov edx,edi // 0051399D
        fstp st(0) // 0051399F
        mov dword ptr [edx],eax // 005139A1
        mov eax,dword ptr [esi+18h] // 005139A3
        mov dword ptr [edx+4],ecx // 005139A7
        mov dword ptr [edx+8],eax // 005139AC
    }
    CAMB_END
}
__declspec(noinline) inline void Dll26(Fx& f) { CAMB_BEGIN CamTwoCarsBlendCpp(f.adr[0], f.adr[1], f.adr[2], f.adr[3]); CAMB_END }
// CamTwoCarsSourcesCpp  (Cam.cpp antes de la conversion: bloque de la linea 2577)
__declspec(noinline) inline void Exe27(Fx& f) {
    float firstZ = f.fv[0];
    float firstHeight = f.fv[1];
    float liftedFirst = f.out[2];
    float secondZ = f.fv[3];
    float secondHeight = f.fv[4];
    float liftedSecond = f.out[5];
    float betweenX = f.fv[6];
    float scaledX = f.out[7];
    float firstX = f.fv[8];
    float fx = f.out[9];
    float secondX = f.fv[10];
    float sx = f.out[11];
    float betweenY = f.fv[12];
    float firstY = f.fv[13];
    float fy = f.out[14];
    float secondY = f.fv[15];
    float sy = f.out[16];
    float betweenZ = f.fv[17];
    float scaledZ = f.out[18];
    float fz = f.out[19];
    float sz = f.out[20];
    CAMB_BEGIN
    __asm {
        fld firstZ
        fadd firstHeight
        fadd dword ptr ds:[00858624h]
        fstp liftedFirst
        fld secondZ
        fadd secondHeight
        fadd dword ptr ds:[00858624h]
        fstp liftedSecond
        fld betweenX
        fmul dword ptr ds:[00858B44h]
        fstp scaledX
        fld firstX
        fsub scaledX
        fstp fx
        fld scaledX
        fadd secondX
        fstp sx
        fld betweenY
        fmul dword ptr ds:[00858B44h]
        fld firstY
        fsub st,st(1)
        fstp fy
        fadd secondY
        fstp sy
        fld betweenZ
        fmul dword ptr ds:[00858B44h]
        fst scaledZ
        fsubr liftedFirst
        fstp fz
        fld scaledZ
        fadd liftedSecond
        fstp sz
    }
    CAMB_END
    f.out[2] = liftedFirst;
    f.out[5] = liftedSecond;
    f.out[7] = scaledX;
    f.out[9] = fx;
    f.out[11] = sx;
    f.out[14] = fy;
    f.out[16] = sy;
    f.out[18] = scaledZ;
    f.out[19] = fz;
    f.out[20] = sz;
}
__declspec(noinline) inline void Dll27(Fx& f) { CAMB_BEGIN CamTwoCarsSourcesCpp(f.fv[0], f.fv[1], &f.out[2], f.fv[3], f.fv[4], &f.out[5], f.fv[6], &f.out[7], f.fv[8], &f.out[9], f.fv[10], &f.out[11], f.fv[12], f.fv[13], &f.out[14], f.fv[15], &f.out[16], f.fv[17], &f.out[18], &f.out[19], &f.out[20]); CAMB_END }
// CamEaseCpp  (Cam.cpp antes de la conversion: bloque de la linea 2721)
__declspec(noinline) inline void Exe28(Fx& f) {
    float ratio = f.fv[0];
    float degrees180 = f.fv[1];
    float degrees270 = f.fv[2];
    float radians = f.fv[3];
    float one = f.fv[4];
    float half = f.fv[5];
    float to = f.fv[6];
    float from = f.fv[7];
    float value = f.out[8];
    CAMB_BEGIN
    __asm {
        fld ratio
        fmul degrees180
        fsubr degrees270
        fmul radians
        fsin
        fadd one
        fmul half
        fld to
        fsub from
        fmulp st(1), st(0)
        fadd from
        fstp value
    }
    CAMB_END
    f.out[8] = value;
}
__declspec(noinline) inline void Dll28(Fx& f) { CAMB_BEGIN CamEaseCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], f.fv[7], &f.out[8]); CAMB_END }
// CamBirdyLampHeightCpp  (Cam.cpp antes de la conversion: bloque de la linea 2741)
__declspec(noinline) inline void Exe29(Fx& f) {
    int randomValue = f.iv[0];
    float randomScale = f.fv[1];
    float height = f.fv[2];
    float one = f.fv[3];
    float pointZ = f.fv[4];
    float result = f.out[5];
    CAMB_BEGIN
    __asm {
        fild randomValue
        fmul randomScale
        fld height
        fsub one
        fmul st(0),st(1)
        fadd one
        fsubr pointZ
        fstp result
        fstp st(0)
    }
    CAMB_END
    f.out[5] = result;
}
__declspec(noinline) inline void Dll29(Fx& f) { CAMB_BEGIN CamBirdyLampHeightCpp(f.iv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], &f.out[5]); CAMB_END }
// CamManFovCpp  (Cam.cpp antes de la conversion: bloque de la linea 2846)
__declspec(noinline) inline void Exe30(Fx& f) {
    unsigned target = f.adr[0];
    unsigned source = f.adr[1];
    float distanceScale = f.fv[2];
    float zero = f.fv[3];
    float one = f.fv[4];
    float degrees180 = f.fv[5];
    float degrees270 = f.fv[6];
    float radians = f.fv[7];
    float half = f.fv[8];
    float final = f.fv[9];
    float initial = f.fv[10];
    float fov = f.out[11];
    float progress = f.fv[12];
    float timeScale = f.fv[13];
    float early = f.fv[14];
    CAMB_BEGIN
    __asm {
        mov eax,target
        mov edx,source
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fld st(2)
        fmulp st(3),st(0)
        fld st(0)
        fmul st(0),st(1)
        faddp st(3),st(0)
        fld st(1)
        fmul st(0),st(2)
        faddp st(3),st(0)
        fxch st(2)
        fsqrt
        fstp st(2)
        fstp st(0)
        fdiv distanceScale
        fcom zero
        fnstsw ax
        test ah,5
        jp manDistanceUpper
        fstp st(0)
        fld zero
        jmp manDistanceEase
    manDistanceUpper:
        fcom one
        fnstsw ax
        test ah,41h
        jne manDistanceEase
        fstp st(0)
        fld one
    manDistanceEase:
        fmul degrees180
        fsubr degrees270
        fmul radians
        fsin
        fadd one
        fmul half
        fld final
        fsub initial
        fmulp st(1),st(0)
        fadd initial
        fstp fov
        fld progress
        fcomp timeScale
        fnstsw ax
        test ah,5
        jp manFovDone
        fld progress
        fdiv timeScale
        fcom zero
        fnstsw ax
        test ah,5
        jp manTimeUpper
        fstp st(0)
        fld zero
        jmp manTimeEase
    manTimeUpper:
        fcom one
        fnstsw ax
        test ah,41h
        jne manTimeEase
        fstp st(0)
        fld one
    manTimeEase:
        fmul degrees180
        fsubr degrees270
        fmul radians
        fsin
        fadd one
        fmul half
        fld fov
        fsub early
        fmulp st(1),st(0)
        fadd early
        fstp fov
    manFovDone:
    }
    CAMB_END
    f.out[11] = fov;
}
__declspec(noinline) inline void Dll30(Fx& f) { CAMB_BEGIN CamManFovCpp(f.adr[0], f.adr[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], f.fv[7], f.fv[8], f.fv[9], f.fv[10], &f.out[11], f.fv[12], f.fv[13], f.fv[14]); CAMB_END }
// CamHeliInitialFovCpp  (Cam.cpp antes de la conversion: bloque de la linea 2984)
__declspec(noinline) inline void Exe31(Fx& f) {
    float one = f.fv[0];
    float duration = f.fv[1];
    float progress = f.fv[2];
    float degrees180 = f.fv[3];
    float degrees270 = f.fv[4];
    float radians = f.fv[5];
    float half = f.fv[6];
    float to = f.fv[7];
    float from = f.fv[8];
    float result = f.out[9];
    CAMB_BEGIN
    __asm {
        fld one
        fdiv duration
        fmul progress
        fmul degrees180
        fsubr degrees270
        fmul radians
        fsin
        fadd one
        fmul half
        fld to
        fsub from
        fmulp st(1), st(0)
        fadd from
        fstp result
    }
    CAMB_END
    f.out[9] = result;
}
__declspec(noinline) inline void Dll31(Fx& f) { CAMB_BEGIN CamHeliInitialFovCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], f.fv[7], f.fv[8], &f.out[9]); CAMB_END }
// CamHeliReturnFovCpp  (Cam.cpp antes de la conversion: bloque de la linea 3008)
__declspec(noinline) inline void Exe32(Fx& f) {
    int start = f.iv[0];
    int now = f.iv[1];
    int end = f.iv[2];
    float zero = f.fv[3];
    float one = f.fv[4];
    float degrees = f.fv[5];
    float phase = f.fv[6];
    float radians = f.fv[7];
    float half = f.fv[8];
    float to = f.fv[9];
    float from = f.fv[10];
    float result = f.out[11];
    CAMB_BEGIN
    __asm {
        fild start
        fild now
        fsub st(0),st(1)
        fild end
        fsub st(0),st(2)
        fdivp st(1),st(0)
        fstp st(1)
        fcom zero
        fnstsw ax
        test ah,5
        jp heliReturnUpper355
        fstp st(0)
        fld zero
        jmp heliReturnEase355
    heliReturnUpper355:
        fcom one
        fnstsw ax
        test ah,41h
        jne heliReturnEase355
        fstp st(0)
        fld one
    heliReturnEase355:
        fmul degrees
        fsubr phase
        fmul radians
        fsin
        fadd one
        fmul half
        fld to
        fsub from
        fmulp st(1),st(0)
        fadd from
        fstp result
    }
    CAMB_END
    f.out[11] = result;
}
__declspec(noinline) inline void Dll32(Fx& f) { CAMB_BEGIN CamHeliReturnFovCpp(f.iv[0], f.iv[1], f.iv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], f.fv[7], f.fv[8], f.fv[9], f.fv[10], &f.out[11]); CAMB_END }
// CamHeliDistanceFovCpp  (Cam.cpp antes de la conversion: bloque de la linea 3050)
__declspec(noinline) inline void Exe33(Fx& f) {
    float to = f.fv[0];
    int initial = f.iv[1];
    float one = f.fv[2];
    float duration = f.fv[3];
    float progress = f.fv[4];
    float degrees = f.fv[5];
    float phase = f.fv[6];
    float radians = f.fv[7];
    float half = f.fv[8];
    float from = f.fv[9];
    float zero = f.fv[10];
    unsigned camera = f.adr[11];
    unsigned dest = f.adr[12];
    float distance = f.out[13];
    float nearDistance = f.fv[14];
    float farDistance = f.fv[15];
    float reduction = f.fv[16];
    float result = f.out[17];
    CAMB_BEGIN
    __asm {
        fld to
        cmp initial,0
        je heliDistanceMeasure355
        fstp st(0)
        fld one
        fdiv duration
        fmul progress
        fmul degrees
        fsubr phase
        fmul radians
        fsin
        fadd one
        fmul half
        fld to
        fsub from
        fmulp st(1),st(0)
        fadd from
    heliDistanceMeasure355:
        fld zero
        mov eax,camera
        mov edx,dest
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
        fst distance
        fcomp nearDistance
        fnstsw ax
        test ah,41h
        jne heliDistanceSubtract355
        fstp st(0)
        fld distance
        fsub nearDistance
        fld farDistance
        fsub nearDistance
        fdivp st(1),st(0)
        fcom zero
        fnstsw ax
        test ah,5
        jp heliDistanceUpper355
        fstp st(0)
        fld zero
        jmp heliDistanceEase355
    heliDistanceUpper355:
        fcom one
        fnstsw ax
        test ah,41h
        jne heliDistanceEase355
        fstp st(0)
        fld one
    heliDistanceEase355:
        fmul degrees
        fsubr phase
        fmul radians
        fsin
        fadd one
        fmul half
        fmul reduction
    heliDistanceSubtract355:
        fxch st(1)
        fsub st(0),st(1)
        fstp result
        fstp st(0)
    }
    CAMB_END
    f.out[13] = distance;
    f.out[17] = result;
}
__declspec(noinline) inline void Dll33(Fx& f) { CAMB_BEGIN CamHeliDistanceFovCpp(f.fv[0], f.iv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], f.fv[7], f.fv[8], f.fv[9], f.fv[10], f.adr[11], f.adr[12], &f.out[13], f.fv[14], f.fv[15], f.fv[16], &f.out[17]); CAMB_END }
// CamPlaneOrbitWaveCpp  (Cam.cpp antes de la conversion: bloque de la linea 3243)
__declspec(noinline) inline void Exe34(Fx& f) {
    int start = f.iv[0];
    int now = f.iv[1];
    int end = f.iv[2];
    int cycles = f.iv[3];
    float degrees = f.fv[4];
    float radians = f.fv[5];
    float componentX = f.fv[6];
    float resultX = f.out[7];
    float componentY = f.fv[8];
    float resultY = f.out[9];
    float componentZ = f.fv[10];
    float resultZ = f.out[11];
    CAMB_BEGIN
    __asm {
        fild start
        fild now
        fsub st(0),st(1)
        fild end
        fsub st(0),st(2)
        fdivp st(1),st(0)
        fild cycles
        fmulp st(1),st(0)
        fmul degrees
        fmul radians
        fsin
        fstp st(1)
        fld componentX
        fmul st(0),st(1)
        fstp resultX
        fld componentY
        fmul st(0),st(1)
        fstp resultY
        fmul componentZ
        fstp resultZ
    }
    CAMB_END
    f.out[7] = resultX;
    f.out[9] = resultY;
    f.out[11] = resultZ;
}
__declspec(noinline) inline void Dll34(Fx& f) { CAMB_BEGIN CamPlaneOrbitWaveCpp(f.iv[0], f.iv[1], f.iv[2], f.iv[3], f.fv[4], f.fv[5], f.fv[6], &f.out[7], f.fv[8], &f.out[9], f.fv[10], &f.out[11]); CAMB_END }
// CamPlaneSpotterRandomCpp  (Cam.cpp antes de la conversion: bloque de la linea 3383)
__declspec(noinline) inline void Exe35(Fx& f) {
    int randomValue = f.iv[0];
    float randomScale = f.fv[1];
    float range = f.fv[2];
    float halfRange = f.fv[3];
    float origin = f.fv[4];
    float result = f.out[5];
    CAMB_BEGIN
    __asm {
        fild randomValue
        fmul randomScale
        fld range
        fsub halfRange
        fmul st(0),st(1)
        fadd halfRange
        fadd origin
        fstp result
        fstp st(0)
    }
    CAMB_END
    f.out[5] = result;
}
__declspec(noinline) inline void Dll35(Fx& f) { CAMB_BEGIN CamPlaneSpotterRandomCpp(f.iv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], &f.out[5]); CAMB_END }
// CamFlyByUpCpp  (Cam.cpp antes de la conversion: bloque de la linea 3644)
__declspec(noinline) inline void Exe36(Fx& f) {
    float roll = f.fv[0];
    float radians = f.fv[1];
    float halfPi = f.fv[2];
    float upX = f.out[3];
    float upZ = f.out[4];
    CAMB_BEGIN
    __asm { fld roll
        fmul radians
        fadd halfPi
        fld st(0)
        fcos
        fstp upX
        fsin
        fstp upZ
    }
    CAMB_END
    f.out[3] = upX;
    f.out[4] = upZ;
}
__declspec(noinline) inline void Dll36(Fx& f) { CAMB_BEGIN CamFlyByUpCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3], &f.out[4]); CAMB_END }
// CamFollowCarBetaBlendCpp  (Cam.cpp antes de la conversion: bloque de la linea 3663)
__declspec(noinline) inline void Exe37(Fx& f) {
    float betaInput = f.fv[0];
    float betaCorrection = f.fv[1];
    float maximumSpeed = f.fv[2];
    float negativeMaximum = f.out[3];
    float smoothing = f.fv[4];
    float previous = f.fv[5];
    float result = f.out[6];
    CAMB_BEGIN
    __asm {
        fld betaInput
        fadd betaCorrection
        fcom maximumSpeed
        fnstsw ax
        test ah,41h
        jne car_beta_lower355
        fstp st(0)
        fld maximumSpeed
        jmp car_beta_blend355
    car_beta_lower355:
        fld maximumSpeed
        fchs
        fstp negativeMaximum
        fcom negativeMaximum
        fnstsw ax
        test ah,5
        jp car_beta_blend355
        fstp st(0)
        fld negativeMaximum
    car_beta_blend355:
        fld dword ptr ds:[00858624h]
        fsub smoothing
        fmulp st(1),st
        fld smoothing
        fmul previous
        faddp st(1),st
        fst result
        fabs
        fcomp dword ptr ds:[008CCEBCh]
        fnstsw ax
        test ah,5
        jp car_beta_done355
        mov result,0
    car_beta_done355:
    }
    CAMB_END
    f.out[3] = negativeMaximum;
    f.out[6] = result;
}
__declspec(noinline) inline void Dll37(Fx& f) { CAMB_BEGIN CamFollowCarBetaBlendCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3], f.fv[4], f.fv[5], &f.out[6]); CAMB_END }
// CamFollowCarHistoryCpp  (Cam.cpp antes de la conversion: bloque de la linea 3708)
__declspec(noinline) inline void Exe38(Fx& f) {
    float radius = f.fv[0];
    float frontX = f.fv[1];
    float frontY = f.fv[2];
    float frontZ = f.fv[3];
    float scaledZ = f.out[4];
    float focalX = f.fv[5];
    float outputX = f.out[6];
    float focalY = f.fv[7];
    float outputY = f.out[8];
    float focalZ = f.fv[9];
    float outputZ = f.out[10];
    CAMB_BEGIN
    __asm {
        fld radius
        fmul frontX
        fld radius
        fmul frontY
        fld radius
        fmul frontZ
        fstp scaledZ
        fld focalX
        fsub st,st(2)
        fstp outputX
        fld focalY
        fsub st,st(1)
        fstp outputY
        fstp st(0)
        fstp st(0)
        fld focalZ
        fsub scaledZ
        fstp outputZ
    }
    CAMB_END
    f.out[4] = scaledZ;
    f.out[6] = outputX;
    f.out[8] = outputY;
    f.out[10] = outputZ;
}
__declspec(noinline) inline void Dll38(Fx& f) { CAMB_BEGIN CamFollowCarHistoryCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], &f.out[4], f.fv[5], &f.out[6], f.fv[7], &f.out[8], f.fv[9], &f.out[10]); CAMB_END }
// CamFollowCarFinalHistoryCpp  (Cam.cpp antes de la conversion: bloque de la linea 3742)
__declspec(noinline) inline void Exe39(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned scratch = f.adr[1];
    CAMB_BEGIN
    __asm {
        mov esi,cam
        mov edi,scratch
        lea eax,[esi+1D8h]
        fld dword ptr [esi+000000BCh]
        fcos
        fmul dword ptr [edi+0000008Ch]
        fchs
        fst dword ptr [edi+6Ch]
        fld dword ptr [esi+000000BCh]
        fsin
        fmul dword ptr [edi+0000008Ch]
        fchs
        fld st(0)
        fld dword ptr [edi+14h]
        fsin
        fst dword ptr [edi+0000008Ch]
        fld dword ptr [edi+6Ch]
        fmul dword ptr [edi+2Ch]
        fstp dword ptr [edi+6Ch]
        fxch st(1)
        fmul dword ptr [edi+2Ch]
        fstp dword ptr [edi+70h]
        fmul dword ptr [edi+2Ch]
        fld dword ptr [edi+48h]
        fsub dword ptr [edi+6Ch]
        fstp dword ptr [edi+54h]
        mov ecx,dword ptr [edi+54h]
        fld dword ptr [edi+4Ch]
        mov dword ptr [eax],ecx
        fsub dword ptr [edi+70h]
        fstp dword ptr [edi+58h]
        fld dword ptr [edi+50h]
        mov edx,dword ptr [edi+58h]
        fsub st,st(1)
        mov dword ptr [eax+4],edx
        fstp dword ptr [edi+5Ch]
        mov ecx,dword ptr [edi+5Ch]
        fstp st(0)
        mov dword ptr [eax+8],ecx
        fxch st(1)
        fstp dword ptr [edi+6Ch]
        fld dword ptr [edi+0000008Ch]
        fld dword ptr [edi+6Ch]
        fmul dword ptr [edi+7Ch]
        fstp dword ptr [edi+6Ch]
        fxch st(1)
        fmul dword ptr [edi+7Ch]
        fstp dword ptr [edi+70h]
        fmul dword ptr [edi+7Ch]
        fld dword ptr [edi+48h]
        fsub dword ptr [edi+6Ch]
        fstp dword ptr [edi+54h]
        mov edx,dword ptr [edi+54h]
        fld dword ptr [edi+4Ch]
        mov dword ptr [esi+000001E4h],edx
        fsub dword ptr [edi+70h]
        fstp dword ptr [edi+58h]
        fld dword ptr [edi+50h]
        mov eax,dword ptr [edi+58h]
        fsub st,st(1)
        mov dword ptr [esi+000001E8h],eax
        mov eax,dword ptr [edi+24h]
        fstp dword ptr [edi+5Ch]
        mov ecx,dword ptr [edi+5Ch]
        mov dword ptr [esi+000001ECh],ecx
        fstp st(0)
    }
    CAMB_END
}
__declspec(noinline) inline void Dll39(Fx& f) { CAMB_BEGIN CamFollowCarFinalHistoryCpp(f.adr[0], f.adr[1]); CAMB_END }
// CamFollowCarBetaCorrectionCpp  (Cam.cpp antes de la conversion: bloque de la linea 3819)
__declspec(noinline) inline void Exe40(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned vehicle = f.adr[1];
    unsigned scratch = f.adr[2];
    unsigned front = f.adr[3];
    CAMB_BEGIN
    __asm {
        push ebp
        mov esi,cam
        mov edi,vehicle
        mov ecx,scratch
        mov eax,front
        mov ebp,eax
        fld dword ptr [ebp]
        fchs
        fld dword ptr [esi+00000194h]
        fpatan
        fsub dword ptr ds:[00858FE4h]
        fcom dword ptr ds:[00858CC0h]
        fnstsw ax
        test ah,5
        jp follow_car_beta_00524E56
        fadd dword ptr ds:[00858CBCh]
    follow_car_beta_00524E56:
        fld dword ptr [edi+48h]
        fld dword ptr [edi+44h]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fsqrt
        fstp st(2)
        fstp st(0)
        fcomp dword ptr ds:[00858B38h]
        fnstsw ax
        test ah,41h
        jne follow_car_beta_00524E8B
        fld dword ptr [edi+44h]
        fchs
        fld dword ptr [edi+48h]
        fpatan
        fsub dword ptr ds:[00858FE4h]
        jmp follow_car_beta_00524E8D
    follow_car_beta_00524E8B:
        fld st(0)
    follow_car_beta_00524E8D:
        fld st(1)
        fadd dword ptr ds:[00858CB8h]
        fld st(1)
        fcompp
        fnstsw ax
        test ah,41h
        jne follow_car_beta_00524EA8
        fsub dword ptr ds:[00858CBCh]
        jmp follow_car_beta_00524EC1
    follow_car_beta_00524EA8:
        fld st(1)
        fsub dword ptr ds:[00858CB8h]
        fld st(1)
        fcompp
        fnstsw ax
        test ah,5
        jp follow_car_beta_00524EC1
        fadd dword ptr ds:[00858CBCh]
    follow_car_beta_00524EC1:
        mov ebx,dword ptr [ecx+34h]
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr [ebx+008CC628h]
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr [ebx+008CC62Ch]
        fstp dword ptr [ecx+18h]
        fld dword ptr [edi+4Ch]
        fmul dword ptr [ebp+8]
        fld dword ptr [edi+48h]
        fmul dword ptr [ebp+4]
        faddp st(1),st
        fld dword ptr [edi+44h]
        fmul dword ptr [ebp]
        faddp st(1),st
        fld st(0)
        fmul dword ptr [ebp]
        fld st(1)
        fmul dword ptr [ebp+4]
        fstp dword ptr [ecx+70h]
        fxch st(1)
        fmul dword ptr [ebp+8]
        fstp dword ptr [ecx+74h]
        fsubr dword ptr [edi+44h]
        fld dword ptr [edi+48h]
        fsub dword ptr [ecx+70h]
        fld dword ptr [edi+4Ch]
        fsub dword ptr [ecx+74h]
        fld st(2)
        fmul st,st(3)
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fsqrt
        fstp dword ptr [ecx+0000008Ch]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fmul dword ptr [ecx+0000008Ch]
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp follow_car_beta_00524F5C
        fstp st(0)
        fld dword ptr ds:[00858624h]
    follow_car_beta_00524F5C:
        fxch st(1)
        fsub st,st(2)
        fmulp st(1),st
        fcom dword ptr [ecx+18h]
        fnstsw ax
        test ah,41h
        jne follow_car_beta_00524F75
        fstp st(0)
        fld dword ptr [ecx+18h]
        jmp follow_car_beta_00524F99
    follow_car_beta_00524F75:
        fld dword ptr [ecx+18h]
        fchs
        fstp dword ptr [ecx+0000008Ch]
        fcom dword ptr [ecx+0000008Ch]
        fnstsw ax
        test ah,5
        jp follow_car_beta_00524F99
        fstp st(0)
        fld dword ptr [ecx+0000008Ch]
    follow_car_beta_00524F99:
        faddp st(1),st
        fld dword ptr [esi+000000BCh]
        fadd dword ptr ds:[00858CB8h]
        fld st(1)
        fcompp
        fnstsw ax
        test ah,41h
        jne follow_car_beta_00524FBA
        fsub dword ptr ds:[00858CBCh]
        jmp follow_car_beta_00524FD7
    follow_car_beta_00524FBA:
        fld dword ptr [esi+000000BCh]
        fsub dword ptr ds:[00858CB8h]
        fld st(1)
        fcompp
        fnstsw ax
        test ah,5
        jp follow_car_beta_00524FD7
        fadd dword ptr ds:[00858CBCh]
    follow_car_beta_00524FD7:
        fld dword ptr ds:[00858624h]
        fcomp dword ptr ds:[00B7CB5Ch]
        fnstsw ax
        test ah,41h
        jne follow_car_beta_00524FF2
        fld dword ptr ds:[00858624h]
        jmp follow_car_beta_00524FF8
    follow_car_beta_00524FF2:
        fld dword ptr ds:[00B7CB5Ch]
    follow_car_beta_00524FF8:
        fxch st(1)
        fsub dword ptr [esi+000000BCh]
        fdiv st,st(1)
        fstp dword ptr [ecx+44h]
        fstp st(0)
        pop ebp
    }
    CAMB_END
}
__declspec(noinline) inline void Dll40(Fx& f) { CAMB_BEGIN CamFollowCarBetaCorrectionCpp(f.adr[0], f.adr[1], f.adr[2], f.adr[3]); CAMB_END }
// CamFollowCarActualDistanceCpp  (Cam.cpp antes de la conversion: bloque de la linea 3992)
__declspec(noinline) inline void Exe41(Fx& f) {
    unsigned origin = f.adr[0];
    unsigned previous = f.adr[1];
    float result = f.out[2];
    CAMB_BEGIN
    __asm {
        mov eax,origin
        mov edx,previous
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fld st(2)
        fmulp st(3),st(0)
        fld st(0)
        fmul st(0),st(1)
        faddp st(3),st(0)
        fld st(1)
        fmul st(0),st(2)
        faddp st(3),st(0)
        fxch st(2)
        fsqrt
        fstp st(2)
        fstp st(0)
        fstp result
    }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll41(Fx& f) { CAMB_BEGIN CamFollowCarActualDistanceCpp(f.adr[0], f.adr[1], &f.out[2]); CAMB_END }
// CamFollowCarAutoAlphaCpp  (Cam.cpp antes de la conversion: bloque de la linea 4020)
__declspec(noinline) inline void Exe42(Fx& f) {
    float base = f.fv[0];
    float step = f.fv[1];
    float wanted = f.fv[2];
    float previous = f.fv[3];
    float result = f.out[4];
    float maximum = f.fv[5];
    CAMB_BEGIN
    __asm {
        fld base
        fld step
        mov eax,00822130h
        call eax
        fsubr dword ptr ds:[00858624h]
        fld wanted
        fsub previous
        fmulp st(1),st
        fst result
        fcomp maximum
        fnstsw ax
        test ah,41h
        jne car_auto_lower355
        mov eax,maximum
        mov result,eax
        jmp car_auto_done355
    car_auto_lower355:
        fld maximum
        fchs
        fld result
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp car_auto_keep355
        fstp result
        jmp car_auto_done355
    car_auto_keep355:
        fstp st(0)
    car_auto_done355:
    }
    CAMB_END
    f.out[4] = result;
}
__declspec(noinline) inline void Dll42(Fx& f) { CAMB_BEGIN CamFollowCarAutoAlphaCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], &f.out[4], f.fv[5]); CAMB_END }
// CamFollowCarAlphaComplementCpp  (Cam.cpp antes de la conversion: bloque de la linea 4058)
__declspec(noinline) inline void Exe43(Fx& f) {
    float smoothing = f.fv[0];
    float complement = f.out[1];
    CAMB_BEGIN
    __asm {
        fld dword ptr ds:[00858624h]
        fsub smoothing
        fstp complement
    }
    CAMB_END
    f.out[1] = complement;
}
__declspec(noinline) inline void Dll43(Fx& f) { CAMB_BEGIN CamFollowCarAlphaComplementCpp(f.fv[0], &f.out[1]); CAMB_END }
// CamFollowCarAlphaBlendCpp  (Cam.cpp antes de la conversion: bloque de la linea 4064)
__declspec(noinline) inline void Exe44(Fx& f) {
    unsigned cam = f.adr[0];
    unsigned scratch = f.adr[1];
    CAMB_BEGIN
    __asm {
        mov esi,cam
        mov edi,scratch
        fld dword ptr [edi+0000008Ch]
        fmul dword ptr [edi+10h]
        fld dword ptr [edi+3Ch]
        fmul dword ptr [esi+000000B0h]
        faddp st(1),st
        fst dword ptr [edi+0000008Ch]
        fstp dword ptr [esi+000000B0h]
        fld dword ptr [edi+10h]
        fcomp dword ptr ds:[00858B50h]
        fld dword ptr [edi+18h]
        fnstsw ax
        test ah,41h
        jne car_alpha_005259E0
        fmul dword ptr ds:[00858B8Ch]
    car_alpha_005259E0:
        fld dword ptr [edi+0000008Ch]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne car_alpha_005259F8
        fstp dword ptr [esi+000000B0h]
        jmp car_alpha_00525A14
    car_alpha_005259F8:
        fchs
        fld dword ptr [edi+0000008Ch]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp car_alpha_00525A12
        fstp dword ptr [esi+000000B0h]
        jmp car_alpha_00525A14
    car_alpha_00525A12:
        fstp st(0)
    car_alpha_00525A14:
        fld dword ptr [esi+000000B0h]
        fabs
        fcomp dword ptr ds:[008CCEB4h]
        fnstsw ax
        test ah,5
        jp car_alpha_end
        mov dword ptr [esi+000000B0h],0
    car_alpha_end:
    }
    CAMB_END
}
__declspec(noinline) inline void Dll44(Fx& f) { CAMB_BEGIN CamFollowCarAlphaBlendCpp(f.adr[0], f.adr[1]); CAMB_END }
// CamMouseChangesStickCpp  (Cam.cpp antes de la conversion: bloque de la linea 4452)
__declspec(noinline) inline void Exe45(Fx& f) {
    float inputY = f.fv[0];
    float fov = f.fv[1];
    float scaleFactor = f.fv[2];
    float stickBeta = f.fv[3];
    float step = f.fv[4];
    float stickScale = f.fv[5];
    float inputX = f.fv[6];
    float betaResult = f.out[7];
    float stickAlpha = f.fv[8];
    float alphaResult = f.out[9];
    CAMB_BEGIN
        __asm {
            fld inputY
            fld fov
            fmul scaleFactor
            fld stickBeta
            fmul st(0),st(1)
            fmul step
            fmul stickScale
            fmul inputX
            fstp betaResult
            fmul stickAlpha
            fmul step
            fmul st(0),st(1)
            fmul stickScale
            fstp alphaResult
            fstp st(0)
        }
    CAMB_END
    f.out[7] = betaResult;
    f.out[9] = alphaResult;
}
__declspec(noinline) inline void Dll45(Fx& f) { CAMB_BEGIN CamMouseChangesStickCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], &f.out[7], f.fv[8], &f.out[9]); CAMB_END }
// CamMouseChangesMouseCpp  (Cam.cpp antes de la conversion: bloque de la linea 4470)
__declspec(noinline) inline void Exe46(Fx& f) {
    float inputX = f.fv[0];
    float mouseBeta = f.fv[1];
    float inputY = f.fv[2];
    float mouseAlpha = f.fv[3];
    float fov = f.fv[4];
    float scaleFactor = f.fv[5];
    float betaSensitivity = f.fv[6];
    float betaResult = f.out[7];
    float alphaSensitivity = f.fv[8];
    float alphaResult = f.out[9];
    CAMB_BEGIN
        __asm {
            fld inputX
            fmul mouseBeta
            fld inputY
            fmul mouseAlpha
            fld fov
            fmul scaleFactor
            fld betaSensitivity
            fmul st(0),st(1)
            fmul st(0),st(3)
            fstp betaResult
            fmul st(0),st(1)
            fmul alphaSensitivity
            fstp alphaResult
            fstp st(0)
            fstp st(0)
        }
    CAMB_END
    f.out[7] = betaResult;
    f.out[9] = alphaResult;
}
__declspec(noinline) inline void Dll46(Fx& f) { CAMB_BEGIN CamMouseChangesMouseCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], f.fv[6], &f.out[7], f.fv[8], &f.out[9]); CAMB_END }
// CamMouseRepeatedRadiusCpp  (Cam.cpp antes de la conversion: bloque de la linea 4493)
__declspec(noinline) inline void Exe47(Fx& f) {
    float fov = f.fv[0];
    float radians = f.fv[1];
    float half = f.fv[2];
    float aspect = f.fv[3];
    float nearPlane = f.fv[4];
    float factor = f.fv[5];
    float result = f.out[6];
    CAMB_BEGIN
    __asm {
        fld fov
        fmul radians
        fmul half
        fptan
        fstp st(0)
        fmul aspect
        fmul nearPlane
        fmul factor
        fstp result
    }
    CAMB_END
    f.out[6] = result;
}
__declspec(noinline) inline void Dll47(Fx& f) { CAMB_BEGIN CamMouseRepeatedRadiusCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], f.fv[4], f.fv[5], &f.out[6]); CAMB_END }
// CamMousePerpendicularDistanceCpp  (Cam.cpp antes de la conversion: bloque de la linea 4511)
__declspec(noinline) inline void Exe48(Fx& f) {
    float one = f.fv[0];
    float radiusFactor = f.fv[1];
    float inverse = f.out[2];
    unsigned collision = f.adr[3];
    unsigned source = f.adr[4];
    unsigned front = f.adr[5];
    float zDifference = f.out[6];
    float yProjection = f.out[7];
    float zProjection = f.out[8];
    float xPerpendicular = f.out[9];
    float result = f.out[10];
    CAMB_BEGIN
    __asm {
        fld one
        fdiv radiusFactor
        fstp inverse
        mov eax,collision
        mov edx,source
        mov ecx,front
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fst zDifference
        fmul dword ptr [ecx+8]
        fld st(1)
        fmul dword ptr [ecx+4]
        faddp st(1),st(0)
        fld st(2)
        fmul dword ptr [ecx]
        faddp st(1),st(0)
        fld st(0)
        fmul dword ptr [ecx]
        fld st(1)
        fmul dword ptr [ecx+4]
        fstp yProjection
        fxch st(1)
        fmul dword ptr [ecx+8]
        fstp zProjection
        fxch st(2)
        fsub st(0),st(2)
        fstp xPerpendicular
        fstp st(1)
        fsub yProjection
        fld zDifference
        fsub zProjection
        fld st(0)
        fmul st(0),st(1)
        fld st(2)
        fmul st(0),st(3)
        faddp st(1),st(0)
        fld xPerpendicular
        fmul xPerpendicular
        faddp st(1),st(0)
        fsqrt
        fstp st(2)
        fstp st(0)
        fmul inverse
        fstp result
    }
    CAMB_END
    f.out[2] = inverse;
    f.out[6] = zDifference;
    f.out[7] = yProjection;
    f.out[8] = zProjection;
    f.out[9] = xPerpendicular;
    f.out[10] = result;
}
__declspec(noinline) inline void Dll48(Fx& f) { CAMB_BEGIN CamMousePerpendicularDistanceCpp(f.fv[0], f.fv[1], &f.out[2], f.adr[3], f.adr[4], f.adr[5], &f.out[6], &f.out[7], &f.out[8], &f.out[9], &f.out[10]); CAMB_END }
// CamMouseSphereCenterCpp  (Cam.cpp antes de la conversion: bloque de la linea 4565)
__declspec(noinline) inline void Exe49(Fx& f) {
    unsigned front = f.adr[0];
    unsigned source = f.adr[1];
    unsigned result = f.adr[2];
    float nearPlane = f.fv[3];
    float zProduct = f.out[4];
    CAMB_BEGIN
    __asm {
        mov eax,front
        mov edx,source
        mov ecx,result
        fld nearPlane
        fmul dword ptr [eax]
        fld nearPlane
        fmul dword ptr [eax+4]
        fld nearPlane
        fmul dword ptr [eax+8]
        fstp zProduct
        fxch st(1)
        fadd dword ptr [edx]
        fstp dword ptr [ecx]
        fadd dword ptr [edx+4]
        fstp dword ptr [ecx+4]
        fld zProduct
        fadd dword ptr [edx+8]
        fstp dword ptr [ecx+8]
    }
    CAMB_END
    f.out[4] = zProduct;
}
__declspec(noinline) inline void Dll49(Fx& f) { CAMB_BEGIN CamMouseSphereCenterCpp(f.adr[0], f.adr[1], f.adr[2], f.fv[3], &f.out[4]); CAMB_END }
// CamMouseInitialSourceCpp  (Cam.cpp antes de la conversion: bloque de la linea 4589)
__declspec(noinline) inline void Exe50(Fx& f) {
    unsigned target = f.adr[0];
    unsigned front = f.adr[1];
    unsigned result = f.adr[2];
    float distance = f.fv[3];
    float zProduct = f.out[4];
    CAMB_BEGIN
    __asm {
        mov eax,target
        mov edx,front
        mov ecx,result
        fld distance
        fmul dword ptr [edx]
        fld distance
        fmul dword ptr [edx+4]
        fld distance
        fmul dword ptr [edx+8]
        fstp zProduct
        fld dword ptr [eax]
        fsub st(0),st(2)
        fstp dword ptr [ecx]
        fld dword ptr [eax+4]
        fsub st(0),st(1)
        fstp dword ptr [ecx+4]
        fstp st(0)
        fstp st(0)
        fld dword ptr [eax+8]
        fsub zProduct
        fstp dword ptr [ecx+8]
    }
    CAMB_END
    f.out[4] = zProduct;
}
__declspec(noinline) inline void Dll50(Fx& f) { CAMB_BEGIN CamMouseInitialSourceCpp(f.adr[0], f.adr[1], f.adr[2], f.fv[3], &f.out[4]); CAMB_END }
// CamMouseFirstHitDistanceCpp  (Cam.cpp antes de la conversion: bloque de la linea 4615)
__declspec(noinline) inline void Exe51(Fx& f) {
    unsigned target = f.adr[0];
    unsigned hit = f.adr[1];
    float result = f.out[2];
    CAMB_BEGIN
    __asm {
        mov eax,target
        mov edx,hit
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
        fstp result
    }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll51(Fx& f) { CAMB_BEGIN CamMouseFirstHitDistanceCpp(f.adr[0], f.adr[1], &f.out[2]); CAMB_END }
// CamMousePullSourceCpp  (Cam.cpp antes de la conversion: bloque de la linea 4643)
__declspec(noinline) inline void Exe52(Fx& f) {
    unsigned target = f.adr[0];
    unsigned source = f.adr[1];
    float zDifference = f.out[2];
    float factor = f.fv[3];
    float xProduct = f.out[4];
    CAMB_BEGIN
    __asm {
        mov eax,target
        mov edx,source
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fstp zDifference
        fxch st(1)
        fmul factor
        fstp xProduct
        fmul factor
        fld zDifference
        fmul factor
        fld xProduct
        fadd dword ptr [edx]
        fstp dword ptr [edx]
        fxch st(1)
        fadd dword ptr [edx+4]
        fstp dword ptr [edx+4]
        fadd dword ptr [edx+8]
        fstp dword ptr [edx+8]
    }
    CAMB_END
    f.out[2] = zDifference;
    f.out[4] = xProduct;
}
__declspec(noinline) inline void Dll52(Fx& f) { CAMB_BEGIN CamMousePullSourceCpp(f.adr[0], f.adr[1], &f.out[2], f.fv[3], &f.out[4]); CAMB_END }
// CamMouseRecenterDeltaCpp  (Cam.cpp antes de la conversion: bloque de la linea 4682)
__declspec(noinline) inline void Exe53(Fx& f) {
    float orientation = f.fv[0];
    float beta = f.fv[1];
    float result = f.out[2];
    CAMB_BEGIN
        __asm {
            fld orientation
            fadd dword ptr ds:[00858CB8h]
            fsub beta
            fstp result
        }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll53(Fx& f) { CAMB_BEGIN CamMouseRecenterDeltaCpp(f.fv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamFollowMouseDistanceCpp  (Cam.cpp antes de la conversion: bloque de la linea 4724)
__declspec(noinline) inline void Exe54(Fx& f) {
    float alpha = f.fv[0];
    float alphaMultiplier = f.fv[1];
    float distance = f.out[2];
    CAMB_BEGIN
    __asm {
        fld alpha
        fldz
        fcomip st(0),st(1)
        jae follow_mouse_nonpositive_alpha
        fmul alphaMultiplier
        fcomp dword ptr ds:[00858FE4h]
        fnstsw ax
        test ah,41h
        jne follow_mouse_positive_bounded
        fld dword ptr ds:[00858FE4h]
        jmp follow_mouse_cos_distance
    follow_mouse_positive_bounded:
        fld alpha
        fmul alphaMultiplier
        jmp follow_mouse_cos_distance
    follow_mouse_nonpositive_alpha:
    follow_mouse_cos_distance:
        fcos
        fmul dword ptr ds:[008CC7C4h]
        fadd dword ptr ds:[008CC7C0h]
        fstp distance
    }
    CAMB_END
    f.out[2] = distance;
}
__declspec(noinline) inline void Dll54(Fx& f) { CAMB_BEGIN CamFollowMouseDistanceCpp(f.fv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamFollowMouseRadiusCpp  (Cam.cpp antes de la conversion: bloque de la linea 4795)
__declspec(noinline) inline void Exe55(Fx& f) {
    float fov = f.fv[0];
    float radians = f.fv[1];
    float half = f.fv[2];
    float factor = f.fv[3];
    float radiusFactor = f.out[4];
    CAMB_BEGIN
    __asm {
        fld fov
        fmul radians
        fmul half
        fptan
        fstp st(0)
        fmul dword ptr ds:[00C3EFA4h]
        fmul factor
        fstp radiusFactor
    }
    CAMB_END
    f.out[4] = radiusFactor;
}
__declspec(noinline) inline void Dll55(Fx& f) { CAMB_BEGIN CamFollowMouseRadiusCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], &f.out[4]); CAMB_END }
// CamRound4ScaleCpp  (Cam.cpp antes de la conversion: bloque de la linea 4844)
__declspec(noinline) inline void Exe56(Fx& f) {
    int five = f.iv[0];
    float value = f.fv[1];
    float scaled = f.out[2];
    CAMB_BEGIN
    __asm {
        fld qword ptr ds:[00859EA8h]
        fild five
        mov eax,00822130h
        call eax
        fmul value
        fld value
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp camera_round_positive
        fsub dword ptr ds:[00858C80h]
        jmp camera_round_fraction
    camera_round_positive:
        fadd dword ptr ds:[00858C80h]
    camera_round_fraction:
        fmul dword ptr ds:[00858B1Ch]
        fstp scaled
    }
    CAMB_END
    f.out[2] = scaled;
}
__declspec(noinline) inline void Dll56(Fx& f) { CAMB_BEGIN CamRound4ScaleCpp(f.iv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamRound4FinishCpp  (Cam.cpp antes de la conversion: bloque de la linea 4864)
__declspec(noinline) inline void Exe57(Fx& f) {
    float integral = f.fv[0];
    int four = f.iv[1];
    float result = f.out[2];
    CAMB_BEGIN
    __asm {
        fld integral
        fld qword ptr ds:[00859EA8h]
        fild four
        mov eax,00822130h
        call eax
        fdivp st(1),st
        fstp result
    }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll57(Fx& f) { CAMB_BEGIN CamRound4FinishCpp(f.fv[0], f.iv[1], &f.out[2]); CAMB_END }
// CamFollowPedAutoAlphaCpp  (Cam.cpp antes de la conversion: bloque de la linea 4893)
__declspec(noinline) inline void Exe58(Fx& f) {
    float base = f.fv[0];
    float step = f.fv[1];
    float wanted = f.fv[2];
    float previous = f.fv[3];
    float result = f.out[4];
    float maximum = f.fv[5];
    CAMB_BEGIN
    __asm {
        fld base
        fld step
        mov eax,00822130h
        call eax
        fsubr dword ptr ds:[00858624h]
        fld wanted
        fsub previous
        fmulp st(1),st
        fst result
        fcomp maximum
        fnstsw ax
        test ah,41h
        jne ped_auto_lower355
        mov eax,maximum
        mov result,eax
        jmp ped_auto_done355
    ped_auto_lower355:
        fld maximum
        fchs
        fld result
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp ped_auto_keep355
        fstp result
        jmp ped_auto_done355
    ped_auto_keep355:
        fstp st(0)
    ped_auto_done355:
    }
    CAMB_END
    f.out[4] = result;
}
__declspec(noinline) inline void Dll58(Fx& f) { CAMB_BEGIN CamFollowPedAutoAlphaCpp(f.fv[0], f.fv[1], f.fv[2], f.fv[3], &f.out[4], f.fv[5]); CAMB_END }
// CamFollowPedPitchCorrectionCpp  (Cam.cpp antes de la conversion: bloque de la linea 4929)
__declspec(noinline) inline void Exe59(Fx& f) {
    float wantedPitch = f.fv[0];
    float followWeight = f.fv[1];
    float followMaximum = f.fv[2];
    float maximum = f.out[3];
    float taskWeight = f.fv[4];
    float historyPitch = f.fv[5];
    float negativeMaximum = f.out[6];
    float verticalOffset = f.fv[7];
    float result = f.out[8];
    CAMB_BEGIN
    __asm {
        fld wantedPitch
        fld dword ptr ds:[008CCE84h]
        fmul followWeight
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp pedPitchWeight355
        fstp st(0)
        fld dword ptr ds:[00858624h]
    pedPitchWeight355:
        fld dword ptr ds:[008CCE80h]
        fmul followMaximum
        fstp maximum
        fld taskWeight
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jnp pedPitchProduct355
        fmul taskWeight
        fld maximum
        fmul taskWeight
        fstp maximum
    pedPitchProduct355:
        fxch st(1)
        fsub historyPitch
        fmulp st(1),st(0)
        fcom maximum
        fnstsw ax
        test ah,41h
        jne pedPitchLower355
        fstp st(0)
        fld maximum
        jmp pedPitchSum355
    pedPitchLower355:
        fld maximum
        fchs
        fstp negativeMaximum
        fcom negativeMaximum
        fnstsw ax
        test ah,5
        jp pedPitchSum355
        fstp st(0)
        fld negativeMaximum
    pedPitchSum355:
        fld historyPitch
        fadd st(0),st(1)
        fsub verticalOffset
        fstp result
        fstp st(0)
    }
    CAMB_END
    f.out[3] = maximum;
    f.out[6] = negativeMaximum;
    f.out[8] = result;
}
__declspec(noinline) inline void Dll59(Fx& f) { CAMB_BEGIN CamFollowPedPitchCorrectionCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3], f.fv[4], f.fv[5], &f.out[6], f.fv[7], &f.out[8]); CAMB_END }
// CamFollowPedDistanceCpp  (Cam.cpp antes de la conversion: bloque de la linea 4986)
__declspec(noinline) inline void Exe60(Fx& f) {
    unsigned origin = f.adr[0];
    unsigned previous = f.adr[1];
    float actual = f.out[2];
    float distance = f.out[3];
    float rowMinimum = f.fv[4];
    float minimumDistance = f.fv[5];
    CAMB_BEGIN
    __asm {
        mov eax,origin
        mov edx,previous
        fld dword ptr [eax]
        fsub dword ptr [edx]
        fld dword ptr [eax+4]
        fsub dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub dword ptr [edx+8]
        fld st(2)
        fmulp st(3),st(0)
        fld st(0)
        fmul st(0),st(1)
        faddp st(3),st(0)
        fld st(1)
        fmul st(0),st(2)
        faddp st(3),st(0)
        fxch st(2)
        fsqrt
        fstp st(2)
        fstp st(0)
        fst actual
        fcomp distance
        fnstsw ax
        test ah,5
        jp pedDistanceDone355
        fld distance
        fcomp rowMinimum
        fnstsw ax
        test ah,41h
        jne pedDistanceDone355
        fld minimumDistance
        fcomp actual
        fnstsw ax
        test ah,41h
        jne pedDistanceActual355
        mov eax,minimumDistance
        mov distance,eax
        jmp pedDistanceDone355
    pedDistanceActual355:
        mov eax,actual
        mov distance,eax
    pedDistanceDone355:
    }
    CAMB_END
    f.out[2] = actual;
    f.out[3] = distance;
}
__declspec(noinline) inline void Dll60(Fx& f) { CAMB_BEGIN CamFollowPedDistanceCpp(f.adr[0], f.adr[1], &f.out[2], &f.out[3], f.fv[4], f.fv[5]); CAMB_END }
// CamFollowPedHistoryPitchCpp  (Cam.cpp antes de la conversion: bloque de la linea 5035)
__declspec(noinline) inline void Exe61(Fx& f) {
    float separation = f.fv[0];
    float maximum = f.out[1];
    float step = f.fv[2];
    float historyPitch = f.out[3];
    CAMB_BEGIN
    __asm {
        fld separation
        fsub dword ptr ds:[008CCE90h]
        fmul dword ptr ds:[00858F08h]
        fld dword ptr ds:[00858CB8h]
        fsub dword ptr ds:[008CCE90h]
        fdivp st(1),st(0)
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp pedHistoryMaximum355
        fstp st(0)
        fld dword ptr ds:[00858624h]
    pedHistoryMaximum355:
        fld dword ptr ds:[00858FE4h]
        fsub dword ptr ds:[008CCE8Ch]
        fmul st(0),st(1)
        fsubr dword ptr ds:[00858FE4h]
        fstp maximum
        fstp st(0)
        fld dword ptr ds:[008CCE88h]
        fld step
        mov eax,00822130h
        call eax
        fld historyPitch
        fcomp maximum
        fnstsw ax
        test ah,41h
        jne pedHistoryNegative355
        fld dword ptr ds:[00858624h]
        fsub st(0),st(1)
        fmul maximum
        fxch st(1)
        fmul historyPitch
        faddp st(1),st(0)
        fstp historyPitch
        jmp pedHistoryDone355
    pedHistoryNegative355:
        fld maximum
        fchs
        fcomp historyPitch
        fnstsw ax
        test ah,41h
        jne pedHistoryUnchanged355
        fld historyPitch
        fmul st(0),st(1)
        fld dword ptr ds:[00858624h]
        fsub st(0),st(2)
        fmul maximum
        fsubp st(1),st(0)
        fstp historyPitch
    pedHistoryUnchanged355:
        fstp st(0)
    pedHistoryDone355:
    }
    CAMB_END
    f.out[1] = maximum;
    f.out[3] = historyPitch;
}
__declspec(noinline) inline void Dll61(Fx& f) { CAMB_BEGIN CamFollowPedHistoryPitchCpp(f.fv[0], &f.out[1], f.fv[2], &f.out[3]); CAMB_END }
// CamFollowPedHistoryBaseCpp  (Cam.cpp antes de la conversion: bloque de la linea 5096)
__declspec(noinline) inline void Exe62(Fx& f) {
    float step = f.fv[0];
    float speed = f.fv[1];
    float wanted = f.fv[2];
    float result = f.out[3];
    CAMB_BEGIN
    __asm {
        fld step
        fmul speed
        fadd wanted
        fstp result
    }
    CAMB_END
    f.out[3] = result;
}
__declspec(noinline) inline void Dll62(Fx& f) { CAMB_BEGIN CamFollowPedHistoryBaseCpp(f.fv[0], f.fv[1], f.fv[2], &f.out[3]); CAMB_END }
// CamFollowPedResetHistoryCpp  (Cam.cpp antes de la conversion: bloque de la linea 5108)
__declspec(noinline) inline void Exe63(Fx& f) {
    unsigned origin = f.adr[0];
    unsigned direction = f.adr[1];
    unsigned firstOut = f.adr[2];
    float firstDistance = f.fv[3];
    float zProduct = f.out[4];
    unsigned secondOut = f.adr[5];
    float secondDistance = f.fv[6];
    CAMB_BEGIN
    __asm {
        mov eax,origin
        mov ecx,direction
        mov edx,firstOut
        fld firstDistance
        fmul dword ptr [ecx]
        fld firstDistance
        fmul dword ptr [ecx+4]
        fld firstDistance
        fmul dword ptr [ecx+8]
        fstp zProduct
        fld dword ptr [eax]
        fsub st(0),st(2)
        fstp dword ptr [edx]
        fld dword ptr [eax+4]
        fsub st(0),st(1)
        fstp dword ptr [edx+4]
        fstp st(0)
        fstp st(0)
        fld dword ptr [eax+8]
        fsub zProduct
        fstp dword ptr [edx+8]
        mov edx,secondOut
        fld secondDistance
        fmul dword ptr [ecx]
        fld secondDistance
        fmul dword ptr [ecx+4]
        fld secondDistance
        fmul dword ptr [ecx+8]
        fstp zProduct
        fld dword ptr [eax]
        fsub st(0),st(2)
        fstp dword ptr [edx]
        fld dword ptr [eax+4]
        fsub st(0),st(1)
        fstp dword ptr [edx+4]
        fstp st(0)
        fstp st(0)
        fld dword ptr [eax+8]
        fsub zProduct
        fstp dword ptr [edx+8]
    }
    CAMB_END
    f.out[4] = zProduct;
}
__declspec(noinline) inline void Dll63(Fx& f) { CAMB_BEGIN CamFollowPedResetHistoryCpp(f.adr[0], f.adr[1], f.adr[2], f.fv[3], &f.out[4], f.adr[5], f.fv[6]); CAMB_END }
// CamFollowPedHistoryCpp  (Cam.cpp antes de la conversion: bloque de la linea 5155)
__declspec(noinline) inline void Exe64(Fx& f) {
    float historyDelta = f.fv[0];
    float verticalOffset = f.fv[1];
    float angle = f.out[2];
    float cosAlpha = f.out[3];
    float beta = f.fv[4];
    float xDirection = f.out[5];
    float zDirection = f.out[6];
    float historyDistance = f.fv[7];
    float xProduct = f.out[8];
    float yProduct = f.out[9];
    unsigned origin = f.adr[10];
    unsigned firstOut = f.adr[11];
    float distance = f.fv[12];
    unsigned secondOut = f.adr[13];
    CAMB_BEGIN
    __asm {
        fld historyDelta
        fadd verticalOffset
        fst angle
        fcos
        fstp cosAlpha
        fld beta
        fcos
        fmul cosAlpha
        fchs
        fst xDirection
        fld beta
        fsin
        fmul cosAlpha
        fchs
        fld st(0)
        fld angle
        fsin
        fst zDirection
        fld xDirection
        fmul historyDistance
        fstp xProduct
        fxch st(1)
        fmul historyDistance
        fstp yProduct
        fmul historyDistance
        mov eax,origin
        mov edx,firstOut
        fld dword ptr [eax]
        fsub xProduct
        fstp dword ptr [edx]
        fld dword ptr [eax+4]
        fsub yProduct
        fstp dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub st,st(1)
        fstp dword ptr [edx+8]
        fstp st(0)
        fxch st(1)
        fstp xDirection
        fld zDirection
        fld xDirection
        fmul distance
        fstp xProduct
        fxch st(1)
        fmul distance
        fstp yProduct
        fmul distance
        mov edx,secondOut
        fld dword ptr [eax]
        fsub xProduct
        fstp dword ptr [edx]
        fld dword ptr [eax+4]
        fsub yProduct
        fstp dword ptr [edx+4]
        fld dword ptr [eax+8]
        fsub st,st(1)
        fstp dword ptr [edx+8]
        fstp st(0)
    }
    CAMB_END
    f.out[2] = angle;
    f.out[3] = cosAlpha;
    f.out[5] = xDirection;
    f.out[6] = zDirection;
    f.out[8] = xProduct;
    f.out[9] = yProduct;
}
__declspec(noinline) inline void Dll64(Fx& f) { CAMB_BEGIN CamFollowPedHistoryCpp(f.fv[0], f.fv[1], &f.out[2], &f.out[3], f.fv[4], &f.out[5], &f.out[6], f.fv[7], &f.out[8], &f.out[9], f.adr[10], f.adr[11], f.fv[12], f.adr[13]); CAMB_END }
// CamFollowPedHistoryHeadingCpp  (Cam.cpp antes de la conversion: bloque de la linea 5222)
__declspec(noinline) inline void Exe65(Fx& f) {
    float frontX = f.fv[0];
    float frontY = f.fv[1];
    float result = f.out[2];
    CAMB_BEGIN
    __asm {
        fld frontX
        fchs
        fld frontY
        fpatan
        fsub dword ptr ds:[00858FE4h]
        fst result
        fcomp dword ptr ds:[00858CC0h]
        fnstsw ax
        test ah,5
        jp ped_history_heading_done355
        fld result
        fadd dword ptr ds:[00858CBCh]
        fstp result
    ped_history_heading_done355:
    }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll65(Fx& f) { CAMB_BEGIN CamFollowPedHistoryHeadingCpp(f.fv[0], f.fv[1], &f.out[2]); CAMB_END }
// CamM16BlendCpp  (Cam.cpp antes de la conversion: bloque de la linea 5470)
__declspec(noinline) inline void Exe66(Fx& f) {
    float smoothing = f.fv[0];
    float complement = f.out[1];
    float betaInput = f.fv[2];
    unsigned betaSpeed = f.adr[3];
    unsigned alphaSpeed = f.adr[4];
    float alphaInput = f.fv[5];
    float alphaRounded = f.out[6];
    unsigned betaAngle = f.adr[7];
    unsigned alphaAngle = f.adr[8];
    CAMB_BEGIN
    __asm {
        fld dword ptr ds:[00858624h]
        fsub smoothing
        fst complement
        fmul betaInput
        mov eax,betaSpeed
        fld smoothing
        fmul dword ptr [eax]
        faddp st(1),st
        fst dword ptr [eax]
        mov eax,alphaSpeed
        fld smoothing
        fmul dword ptr [eax]
        fld complement
        fmul alphaInput
        faddp st(1),st
        fst alphaRounded
        fstp dword ptr [eax]
        fld alphaRounded
        fxch st(1)
        mov eax,betaAngle
        fadd dword ptr [eax]
        fstp dword ptr [eax]
        mov eax,alphaAngle
        fadd dword ptr [eax]
        fstp dword ptr [eax]
    }
    CAMB_END
    f.out[1] = complement;
    f.out[6] = alphaRounded;
}
__declspec(noinline) inline void Dll66(Fx& f) { CAMB_BEGIN CamM16BlendCpp(f.fv[0], &f.out[1], f.fv[2], f.adr[3], f.adr[4], f.fv[5], &f.out[6], f.adr[7], f.adr[8]); CAMB_END }
// CamM16BumpCpp  (Cam.cpp antes de la conversion: bloque de la linea 5502)
__declspec(noinline) inline void Exe67(Fx& f) {
    int signedElapsed = f.iv[0];
    unsigned horz = f.adr[1];
    unsigned beta = f.adr[2];
    unsigned vert = f.adr[3];
    unsigned alpha = f.adr[4];
    float damping = f.fv[5];
    float step = f.fv[6];
    CAMB_BEGIN
    __asm {
        fild signedElapsed
        cmp signedElapsed,0
        jge m16_elapsed_positive355
        fadd dword ptr ds:[00858C54h]
    m16_elapsed_positive355:
        fidiv dword ptr ds:[008CC474h]
        fmul dword ptr ds:[00858CBCh]
        fcos
        fld dword ptr ds:[008CC480h]
        fmul st,st(1)
        mov eax,horz
        fmul dword ptr [eax]
        mov eax,beta
        fadd dword ptr [eax]
        fstp dword ptr [eax]
        mov eax,vert
        fmul dword ptr [eax]
        fmul dword ptr ds:[008CC480h]
        mov eax,alpha
        fadd dword ptr [eax]
        fstp dword ptr [eax]
        fld damping
        fld step
        mov eax,00822130h
        call eax
        mov eax,horz
        fmul dword ptr [eax]
        fstp dword ptr [eax]
        fld damping
        fld step
        mov eax,00822130h
        call eax
        mov eax,vert
        fmul dword ptr [eax]
        fstp dword ptr [eax]
    }
    CAMB_END
}
__declspec(noinline) inline void Dll67(Fx& f) { CAMB_BEGIN CamM16BumpCpp(f.iv[0], f.adr[1], f.adr[2], f.adr[3], f.adr[4], f.fv[5], f.fv[6]); CAMB_END }
// CamM16StickInputsCpp  (Cam.cpp antes de la conversion: bloque de la linea 5543)
__declspec(noinline) inline void Exe68(Fx& f) {
    float vertical = f.fv[0];
    int attached = f.iv[1];
    float horizontal = f.fv[2];
    float fov = f.fv[3];
    float step = f.fv[4];
    float betaResult = f.out[5];
    float alphaResult = f.out[6];
    float horizontalSign = f.out[7];
    CAMB_BEGIN
    __asm {
        fld vertical
        cmp attached,0
        je m16_unattached_input355
        fld horizontal
        fmul dword ptr ds:[00858B88h]
        fld fov
        fmul dword ptr ds:[0085A5C4h]
        fld st(1)
        fabs
        fld st(1)
        fmul dword ptr ds:[00858CECh]
        fmulp st(1),st
        fmul step
        fmul st,st(2)
        fstp betaResult
        fld st(2)
        fmul dword ptr ds:[00858B88h]
        fld st(0)
        fabs
        fxch st(2)
        fmul dword ptr ds:[008631C0h]
        fmulp st(2),st
        fxch st(1)
        fmul step
        fmul st,st(1)
        fstp alphaResult
        fstp st(0)
        fstp st(0)
        jmp m16_input_cleanup355
    m16_unattached_input355:
        fld dword ptr ds:[00858624h]
        fld horizontal
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp m16_positive_h355
        mov horizontalSign,0BF800000h
    m16_positive_h355:
        fld st(1)
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp m16_positive_v355
        fstp st(0)
        fld dword ptr ds:[00858C1Ch]
    m16_positive_v355:
        fld fov
        fmul dword ptr ds:[0085A5C4h]
        fld horizontal
        fmul horizontal
        fmul dword ptr ds:[00858FC4h]
        fld st(1)
        fmul dword ptr ds:[00863100h]
        fmulp st(1),st
        fmul step
        fmul horizontalSign
        fstp betaResult
        fld st(2)
        fmul st,st(3)
        fmul dword ptr ds:[008630FCh]
        fxch st(1)
        fmul dword ptr ds:[00859B50h]
        fmulp st(1),st
        fmul step
        fmul st,st(1)
        fstp alphaResult
        fstp st(0)
    m16_input_cleanup355:
        fstp st(0)
    }
    CAMB_END
    f.out[5] = betaResult;
    f.out[6] = alphaResult;
    f.out[7] = horizontalSign;
}
__declspec(noinline) inline void Dll68(Fx& f) { CAMB_BEGIN CamM16StickInputsCpp(f.fv[0], f.iv[1], f.fv[2], f.fv[3], f.fv[4], &f.out[5], &f.out[6], &f.out[7]); CAMB_END }
// CamM16MouseAnglesCpp  (Cam.cpp antes de la conversion: bloque de la linea 5619)
__declspec(noinline) inline void Exe69(Fx& f) {
    float horizontal = f.fv[0];
    float fov = f.fv[1];
    float scale = f.out[2];
    float vertical = f.fv[3];
    unsigned beta = f.adr[4];
    unsigned alpha = f.adr[5];
    CAMB_BEGIN
    __asm {
        fld horizontal
        fld fov
        fmul dword ptr ds:[0085A5C4h]
        fst scale
        fmul dword ptr ds:[00B6EC1Ch]
        fmulp st(1),st
        fld scale
        fmul dword ptr ds:[00B6EC18h]
        fmul vertical
        fxch st(1)
        mov eax,beta
        fadd dword ptr [eax]
        fstp dword ptr [eax]
        mov eax,alpha
        fadd dword ptr [eax]
        fstp dword ptr [eax]
    }
    CAMB_END
    f.out[2] = scale;
}
__declspec(noinline) inline void Dll69(Fx& f) { CAMB_BEGIN CamM16MouseAnglesCpp(f.fv[0], f.fv[1], &f.out[2], f.fv[3], f.adr[4], f.adr[5]); CAMB_END }
// CamM16SideRayCpp  (Cam.cpp antes de la conversion: bloque de la linea 5642)
__declspec(noinline) inline void Exe70(Fx& f) {
    float beta = f.fv[0];
    int positive = f.iv[1];
    float alpha = f.fv[2];
    float xDirection = f.out[3];
    float yDirection = f.out[4];
    float yProduct = f.out[5];
    float zProduct = f.out[6];
    unsigned origin = f.adr[7];
    unsigned outputVector = f.adr[8];
    CAMB_BEGIN
    __asm {
        fld beta
        cmp positive,0
        je m16_side_negative355
        fadd dword ptr ds:[008631B8h]
        jmp m16_side_alpha355
    m16_side_negative355:
        fsub dword ptr ds:[008631B8h]
    m16_side_alpha355:
        fld alpha
        fsub dword ptr ds:[008631B4h]
        fld st(0)
        fcos
        fld st(2)
        fcos
        fmul st,st(1)
        fstp xDirection
        fxch st(2)
        fsin
        fmul st,st(2)
        fstp yDirection
        fstp st(1)
        fsin
        fld xDirection
        fmul dword ptr ds:[00858B3Ch]
        fld yDirection
        fmul dword ptr ds:[00858B3Ch]
        fstp yProduct
        fxch st(1)
        fmul dword ptr ds:[00858B3Ch]
        fstp zProduct
        mov eax,origin
        mov edx,outputVector
        fadd dword ptr [eax]
        fstp dword ptr [edx]
        fld yProduct
        fadd dword ptr [eax+4]
        fstp dword ptr [edx+4]
        fld zProduct
        fadd dword ptr [eax+8]
        fstp dword ptr [edx+8]
    }
    CAMB_END
    f.out[3] = xDirection;
    f.out[4] = yDirection;
    f.out[5] = yProduct;
    f.out[6] = zProduct;
}
__declspec(noinline) inline void Dll70(Fx& f) { CAMB_BEGIN CamM16SideRayCpp(f.fv[0], f.iv[1], f.fv[2], &f.out[3], &f.out[4], &f.out[5], &f.out[6], f.adr[7], f.adr[8]); CAMB_END }
// CamRocketFrontCpp  (Cam.cpp antes de la conversion: bloque de la linea 5920)
__declspec(noinline) inline void Exe71(Fx& f) {
    float verticalAngle = f.fv[0];
    float horizontalAngle = f.fv[1];
    float frontX = f.out[2];
    float frontY = f.out[3];
    float frontZ = f.out[4];
    CAMB_BEGIN
        __asm {
            fld  dword ptr [verticalAngle]
            fcos
            fld  dword ptr [horizontalAngle]
            fcos
            fmul st(0), st(1)
            fchs
            fstp dword ptr [frontX]
            fld  dword ptr [horizontalAngle]
            fsin
            fmul st(0), st(1)
            fchs
            fstp dword ptr [frontY]
            fstp st(0)
            fld  dword ptr [verticalAngle]
            fsin
            fstp dword ptr [frontZ]
        }
    CAMB_END
    f.out[2] = frontX;
    f.out[3] = frontY;
    f.out[4] = frontZ;
}
__declspec(noinline) inline void Dll71(Fx& f) { CAMB_BEGIN CamRocketFrontCpp(f.fv[0], f.fv[1], &f.out[2], &f.out[3], &f.out[4]); CAMB_END }
// CamRocketHeadingCpp  (Cam.cpp antes de la conversion: bloque de la linea 5948)
__declspec(noinline) inline void Exe72(Fx& f) {
    float atanY = f.fv[0];
    float atanX = f.fv[1];
    unsigned atanFn = f.adr[2];
    float halfPi = f.fv[3];
    float heading = f.out[4];
    CAMB_BEGIN
            __asm {
                push dword ptr [atanY]
                push dword ptr [atanX]
                call dword ptr [atanFn]
                add  esp, 8
                fsub dword ptr [halfPi]
                fstp dword ptr [heading]
            }
    CAMB_END
    f.out[4] = heading;
}
__declspec(noinline) inline void Dll72(Fx& f) { CAMB_BEGIN CamRocketHeadingCpp(f.fv[0], f.fv[1], f.adr[2], f.fv[3], &f.out[4]); CAMB_END }
// CamWheelUpCpp  (Cam.cpp antes de la conversion: bloque de la linea 6091)
__declspec(noinline) inline void Exe73(Fx& f) {
    int phase = f.iv[0];
    float angleScale = f.fv[1];
    float amount = f.fv[2];
    float sx = f.fv[3];
    float lateralX = f.out[4];
    float sy = f.fv[5];
    float lateralY = f.out[6];
    float sz = f.fv[7];
    float ux = f.fv[8];
    float uy = f.fv[9];
    float verticalY = f.out[10];
    float uz = f.fv[11];
    float verticalZ = f.out[12];
    float outputX = f.out[13];
    float outputY = f.out[14];
    float outputZ = f.out[15];
    CAMB_BEGIN
    __asm {
        fild phase
        fmul angleScale
        fcos
        fmul amount
        fld st(0)
        fsin
        fld sx
        fmul st, st(1)
        fstp lateralX
        fld sy
        fmul st, st(1)
        fstp lateralY
        fmul sz
        fxch st(1)
        fcos
        fld ux
        fmul st, st(1)
        fld uy
        fmul st, st(2)
        fstp verticalY
        fld uz
        fmul st, st(2)
        fstp verticalZ
        fadd lateralX
        fstp outputX
        fstp st(0)
        fld verticalY
        fadd lateralY
        fstp outputY
        fld verticalZ
        fadd st, st(1)
        fstp outputZ
        fstp st(0)
    }
    CAMB_END
    f.out[4] = lateralX;
    f.out[6] = lateralY;
    f.out[10] = verticalY;
    f.out[12] = verticalZ;
    f.out[13] = outputX;
    f.out[14] = outputY;
    f.out[15] = outputZ;
}
__declspec(noinline) inline void Dll73(Fx& f) { CAMB_BEGIN CamWheelUpCpp(f.iv[0], f.fv[1], f.fv[2], f.fv[3], &f.out[4], f.fv[5], &f.out[6], f.fv[7], f.fv[8], f.fv[9], &f.out[10], f.fv[11], &f.out[12], &f.out[13], &f.out[14], &f.out[15]); CAMB_END }
// Reuse_TrigSimpleSinCpp  (Cam.cpp antes de la conversion: bloque de la linea 62)
__declspec(noinline) inline void Exe74(Fx& f) {
    float angle = f.fv[0];
    float result = f.out[1];
    CAMB_BEGIN
    __asm {
        fld  dword ptr [angle]
        fsin
        fstp dword ptr [result]
    }
    CAMB_END
    f.out[1] = result;
}
__declspec(noinline) inline void Dll74(Fx& f) { CAMB_BEGIN f.out[1] = TrigSimpleSinCpp(f.fv[0]); CAMB_END }
// Reuse_TrigSimpleCosCpp  (Cam.cpp antes de la conversion: bloque de la linea 71)
__declspec(noinline) inline void Exe75(Fx& f) {
    float angle = f.fv[0];
    float result = f.out[1];
    CAMB_BEGIN
    __asm {
        fld  dword ptr [angle]
        fcos
        fstp dword ptr [result]
    }
    CAMB_END
    f.out[1] = result;
}
__declspec(noinline) inline void Dll75(Fx& f) { CAMB_BEGIN f.out[1] = TrigSimpleCosCpp(f.fv[0]); CAMB_END }
// Reuse_Atan2MinusCpp  (Cam.cpp antes de la conversion: bloque de la linea 643)
__declspec(noinline) inline void Exe76(Fx& f) {
    float negativeX = f.fv[0];
    float velocityY = f.fv[1];
    float halfPi = f.fv[2];
    float orientation = f.out[3];
    CAMB_BEGIN
        __asm { fld negativeX
            fld velocityY
            fpatan
            fsub halfPi
            fstp orientation
        }
    CAMB_END
    f.out[3] = orientation;
}
__declspec(noinline) inline void Dll76(Fx& f) { CAMB_BEGIN f.out[3] = Atan2MinusCpp(f.fv[0], f.fv[1], f.fv[2]); CAMB_END }
// Reuse_TrigAtan2Cpp  (Cam.cpp antes de la conversion: bloque de la linea 1201)
__declspec(noinline) inline void Exe77(Fx& f) {
    float headingInputX = f.fv[0];
    float headingInputY = f.fv[1];
    float heading = f.out[2];
    CAMB_BEGIN
        __asm {
            fld headingInputX
            fld headingInputY
            fpatan
            fstp heading
        }
    CAMB_END
    f.out[2] = heading;
}
__declspec(noinline) inline void Dll77(Fx& f) { CAMB_BEGIN f.out[2] = TrigAtan2Cpp(f.fv[0], f.fv[1]); CAMB_END }
// Reuse_Atan2MinusCpp_neg  (Cam.cpp antes de la conversion: bloque de la linea 1396)
__declspec(noinline) inline void Exe78(Fx& f) {
    float frontX = f.fv[0];
    float frontY = f.fv[1];
    float headingOffset = f.fv[2];
    float result = f.out[3];
    CAMB_BEGIN
    __asm {
        fld frontX
        fchs
        fld frontY
        fpatan
        fsub headingOffset
        fstp result
    }
    CAMB_END
    f.out[3] = result;
}
__declspec(noinline) inline void Dll78(Fx& f) { CAMB_BEGIN f.out[3] = Atan2MinusCpp(-f.fv[0], f.fv[1], f.fv[2]); CAMB_END }
// Reuse_CnvDecayPowCpp  (Cam.cpp antes de la conversion: bloque de la linea 1888)
__declspec(noinline) inline void Exe79(Fx& f) {
    float base = f.fv[0];
    float step = f.fv[1];
    float result = f.out[2];
    CAMB_BEGIN
    __asm {
        fld base
        fld step
        mov eax,00822130h
        call eax
        fstp result
    }
    CAMB_END
    f.out[2] = result;
}
__declspec(noinline) inline void Dll79(Fx& f) { CAMB_BEGIN f.out[2] = CnvDecayPowCpp(f.fv[0], f.fv[1]); CAMB_END }
// Reuse_Atan2MinusCpp_const  (Cam.cpp antes de la conversion: bloque de la linea 2002)
__declspec(noinline) inline void Exe80(Fx& f) {
    float headingX = f.fv[0];
    float headingY = f.fv[1];
    float heading = f.out[2];
    CAMB_BEGIN
            __asm {
                fld headingX
                fld headingY
                fpatan
                fsub dword ptr ds:[00858FE4h]
                fstp heading
            }
    CAMB_END
    f.out[2] = heading;
}
__declspec(noinline) inline void Dll80(Fx& f) { CAMB_BEGIN f.out[2] = Atan2MinusCpp(f.fv[0], f.fv[1], std::bit_cast<float>(*reinterpret_cast<const uint32*>(0x858FE4))); CAMB_END }
// Reuse_CnvDecayPowCpp_const  (Cam.cpp antes de la conversion: bloque de la linea 2026)
__declspec(noinline) inline void Exe81(Fx& f) {
    float smoothing = f.out[0];
    CAMB_BEGIN
    __asm {
        fld dword ptr ds:[008CC5E0h]
        fld dword ptr ds:[00B7CB5Ch]
        mov eax,00822130h
        call eax
        fstp smoothing
    }
    CAMB_END
    f.out[0] = smoothing;
}
__declspec(noinline) inline void Dll81(Fx& f) { CAMB_BEGIN f.out[0] = CnvDecayPowCpp(std::bit_cast<float>(*reinterpret_cast<const uint32*>(0x8CC5E0)), std::bit_cast<float>(*reinterpret_cast<const uint32*>(0xB7CB5C))); CAMB_END }
// Reuse_IntRatioCpp  (Cam.cpp antes de la conversion: bloque de la linea 2962)
__declspec(noinline) inline void Exe82(Fx& f) {
    int elapsed = f.iv[0];
    int duration = f.iv[1];
    float progress = f.out[2];
    CAMB_BEGIN
    __asm { fild elapsed
        fidiv duration
        fstp progress
    }
    CAMB_END
    f.out[2] = progress;
}
__declspec(noinline) inline void Dll82(Fx& f) { CAMB_BEGIN f.out[2] = IntRatioCpp(f.iv[0], f.iv[1]); CAMB_END }
// Reuse_CnvAsinFloatCpp  (Cam.cpp antes de la conversion: bloque de la linea 5437)
__declspec(noinline) inline void Exe83(Fx& f) {
    float input = f.fv[0];
    float result = f.out[1];
    CAMB_BEGIN
    __asm {
        fld input
        mov eax, 00821E70h
        call eax
        fstp result
    }
    CAMB_END
    f.out[1] = result;
}
__declspec(noinline) inline void Dll83(Fx& f) { CAMB_BEGIN f.out[1] = CnvAsinFloatCpp(f.fv[0]); CAMB_END }

#pragma warning(pop)
using StepFn = void(*)(Fx&);
struct Entry { const char* name; const char* spec; StepFn exe; StepFn dll; uintptr_t dllEntry; uint32 globals[3]; };
inline const std::array<Entry,84> entries{{
    Entry{"CamTwoPlayerAimTanCpp","fffoo",&Exe0,&Dll0,std::bit_cast<uintptr_t>(&CamTwoPlayerAimTanCpp),{0,0,0}},
    Entry{"CamLookBehindPedSideCpp","foo",&Exe1,&Dll1,std::bit_cast<uintptr_t>(&CamLookBehindPedSideCpp),{0,0,0}},
    Entry{"CamLookSideOrbitCpp","ffpffffofo",&Exe2,&Dll2,std::bit_cast<uintptr_t>(&CamLookSideOrbitCpp),{0,0,0}},
    Entry{"CamRotInFrontCarOrbitCpp","fffofo",&Exe3,&Dll3,std::bit_cast<uintptr_t>(&CamRotInFrontCarOrbitCpp),{0,0,0}},
    Entry{"CamVelocityInverseStepCpp","ffo",&Exe4,&Dll4,std::bit_cast<uintptr_t>(&CamVelocityInverseStepCpp),{0,0,0}},
    Entry{"CamVehicleTargetSpeedCpp","oofffffffo",&Exe5,&Dll5,std::bit_cast<uintptr_t>(&CamVehicleTargetSpeedCpp),{0,0,0}},
    Entry{"CamVehicleSmoothedSpeedCpp","ffffo",&Exe6,&Dll6,std::bit_cast<uintptr_t>(&CamVehicleSmoothedSpeedCpp),{0,0,0}},
    Entry{"CamPowerComplementCpp","fffo",&Exe7,&Dll7,std::bit_cast<uintptr_t>(&CamPowerComplementCpp),{0,0,0}},
    Entry{"CamDeadWaveCpp","fffffffoofo",&Exe8,&Dll8,std::bit_cast<uintptr_t>(&CamDeadWaveCpp),{0,0,0}},
    Entry{"CamDeadUnsignedElapsedCpp","ifo",&Exe9,&Dll9,std::bit_cast<uintptr_t>(&CamDeadUnsignedElapsedCpp),{0,0,0}},
    Entry{"CamDeadCosineCpp","ffo",&Exe10,&Dll10,std::bit_cast<uintptr_t>(&CamDeadCosineCpp),{0,0,0}},
    Entry{"CamFirstPersonPedFrontCpp","ffooo",&Exe11,&Dll11,std::bit_cast<uintptr_t>(&CamFirstPersonPedFrontCpp),{0,0,0}},
    Entry{"CamAimOffsetsCpp","foo",&Exe12,&Dll12,std::bit_cast<uintptr_t>(&CamAimOffsetsCpp),{0,0,0}},
    Entry{"CamAimTargetAnglesCpp","ffffffoooo",&Exe13,&Dll13,std::bit_cast<uintptr_t>(&CamAimTargetAnglesCpp),{0,0,0}},
    Entry{"CamAimProjectionCpp","uur",&Exe14,&Dll14,std::bit_cast<uintptr_t>(&CamAimProjectionCpp),{0,0,0}},
    Entry{"CamAimMeleeBlendCpp","u",&Exe15,&Dll15,std::bit_cast<uintptr_t>(&CamAimMeleeBlendCpp),{0xB70100,0xB700FC,0xB700F4}},
    Entry{"CamAimMouseAnglesCpp","u",&Exe16,&Dll16,std::bit_cast<uintptr_t>(&CamAimMouseAnglesCpp),{0,0,0}},
    Entry{"CamAimVehicleResetPitchCpp","ffo",&Exe17,&Dll17,std::bit_cast<uintptr_t>(&CamAimVehicleResetPitchCpp),{0,0,0}},
    Entry{"CamAttachedUpCpp","ffofofofofofo",&Exe18,&Dll18,std::bit_cast<uintptr_t>(&CamAttachedUpCpp),{0,0,0}},
    Entry{"CamDirectionFromAnglesCpp","ffooo",&Exe19,&Dll19,std::bit_cast<uintptr_t>(&CamDirectionFromAnglesCpp),{0,0,0}},
    Entry{"CamShootingSourceZCpp","fffo",&Exe20,&Dll20,std::bit_cast<uintptr_t>(&CamShootingSourceZCpp),{0,0,0}},
    Entry{"CamShootingOrbitCpp","uuu",&Exe21,&Dll21,std::bit_cast<uintptr_t>(&CamShootingOrbitCpp),{0,0,0}},
    Entry{"CamShootingTiltCpp","uuu",&Exe22,&Dll22,std::bit_cast<uintptr_t>(&CamShootingTiltCpp),{0,0,0}},
    Entry{"CamShootingFovCpp","uu",&Exe23,&Dll23,std::bit_cast<uintptr_t>(&CamShootingFovCpp),{0,0,0}},
    Entry{"CamStoredHeadingMinusHalfPiCpp","fo",&Exe24,&Dll24,std::bit_cast<uintptr_t>(&CamStoredHeadingMinusHalfPiCpp),{0,0,0}},
    Entry{"CamTwoCarsBlendWeightCpp","foo",&Exe25,&Dll25,std::bit_cast<uintptr_t>(&CamTwoCarsBlendWeightCpp),{0,0,0}},
    Entry{"CamTwoCarsBlendCpp","uuuu",&Exe26,&Dll26,std::bit_cast<uintptr_t>(&CamTwoCarsBlendCpp),{0,0,0}},
    Entry{"CamTwoCarsSourcesCpp","ffoffofofofoffofofooo",&Exe27,&Dll27,std::bit_cast<uintptr_t>(&CamTwoCarsSourcesCpp),{0,0,0}},
    Entry{"CamEaseCpp","ffffffffo",&Exe28,&Dll28,std::bit_cast<uintptr_t>(&CamEaseCpp),{0,0,0}},
    Entry{"CamBirdyLampHeightCpp","iffffo",&Exe29,&Dll29,std::bit_cast<uintptr_t>(&CamBirdyLampHeightCpp),{0,0,0}},
    Entry{"CamManFovCpp","uufffffffffofff",&Exe30,&Dll30,std::bit_cast<uintptr_t>(&CamManFovCpp),{0,0,0}},
    Entry{"CamHeliInitialFovCpp","fffffffffo",&Exe31,&Dll31,std::bit_cast<uintptr_t>(&CamHeliInitialFovCpp),{0,0,0}},
    Entry{"CamHeliReturnFovCpp","iiiffffffffo",&Exe32,&Dll32,std::bit_cast<uintptr_t>(&CamHeliReturnFovCpp),{0,0,0}},
    Entry{"CamHeliDistanceFovCpp","fifffffffffuuofffo",&Exe33,&Dll33,std::bit_cast<uintptr_t>(&CamHeliDistanceFovCpp),{0,0,0}},
    Entry{"CamPlaneOrbitWaveCpp","iiiifffofofo",&Exe34,&Dll34,std::bit_cast<uintptr_t>(&CamPlaneOrbitWaveCpp),{0,0,0}},
    Entry{"CamPlaneSpotterRandomCpp","iffffo",&Exe35,&Dll35,std::bit_cast<uintptr_t>(&CamPlaneSpotterRandomCpp),{0,0,0}},
    Entry{"CamFlyByUpCpp","fffoo",&Exe36,&Dll36,std::bit_cast<uintptr_t>(&CamFlyByUpCpp),{0,0,0}},
    Entry{"CamFollowCarBetaBlendCpp","fffoffo",&Exe37,&Dll37,std::bit_cast<uintptr_t>(&CamFollowCarBetaBlendCpp),{0,0,0}},
    Entry{"CamFollowCarHistoryCpp","ffffofofofo",&Exe38,&Dll38,std::bit_cast<uintptr_t>(&CamFollowCarHistoryCpp),{0,0,0}},
    Entry{"CamFollowCarFinalHistoryCpp","uu",&Exe39,&Dll39,std::bit_cast<uintptr_t>(&CamFollowCarFinalHistoryCpp),{0,0,0}},
    Entry{"CamFollowCarBetaCorrectionCpp","uuuu",&Exe40,&Dll40,std::bit_cast<uintptr_t>(&CamFollowCarBetaCorrectionCpp),{0,0,0}},
    Entry{"CamFollowCarActualDistanceCpp","uuo",&Exe41,&Dll41,std::bit_cast<uintptr_t>(&CamFollowCarActualDistanceCpp),{0,0,0}},
    Entry{"CamFollowCarAutoAlphaCpp","ffffof",&Exe42,&Dll42,std::bit_cast<uintptr_t>(&CamFollowCarAutoAlphaCpp),{0,0,0}},
    Entry{"CamFollowCarAlphaComplementCpp","fo",&Exe43,&Dll43,std::bit_cast<uintptr_t>(&CamFollowCarAlphaComplementCpp),{0,0,0}},
    Entry{"CamFollowCarAlphaBlendCpp","uu",&Exe44,&Dll44,std::bit_cast<uintptr_t>(&CamFollowCarAlphaBlendCpp),{0,0,0}},
    Entry{"CamMouseChangesStickCpp","fffffffofo",&Exe45,&Dll45,std::bit_cast<uintptr_t>(&CamMouseChangesStickCpp),{0,0,0}},
    Entry{"CamMouseChangesMouseCpp","fffffffofo",&Exe46,&Dll46,std::bit_cast<uintptr_t>(&CamMouseChangesMouseCpp),{0,0,0}},
    Entry{"CamMouseRepeatedRadiusCpp","ffffffo",&Exe47,&Dll47,std::bit_cast<uintptr_t>(&CamMouseRepeatedRadiusCpp),{0,0,0}},
    Entry{"CamMousePerpendicularDistanceCpp","ffouuuooooo",&Exe48,&Dll48,std::bit_cast<uintptr_t>(&CamMousePerpendicularDistanceCpp),{0,0,0}},
    Entry{"CamMouseSphereCenterCpp","uuufo",&Exe49,&Dll49,std::bit_cast<uintptr_t>(&CamMouseSphereCenterCpp),{0,0,0}},
    Entry{"CamMouseInitialSourceCpp","uuufo",&Exe50,&Dll50,std::bit_cast<uintptr_t>(&CamMouseInitialSourceCpp),{0,0,0}},
    Entry{"CamMouseFirstHitDistanceCpp","uuo",&Exe51,&Dll51,std::bit_cast<uintptr_t>(&CamMouseFirstHitDistanceCpp),{0,0,0}},
    Entry{"CamMousePullSourceCpp","uuofo",&Exe52,&Dll52,std::bit_cast<uintptr_t>(&CamMousePullSourceCpp),{0,0,0}},
    Entry{"CamMouseRecenterDeltaCpp","ffo",&Exe53,&Dll53,std::bit_cast<uintptr_t>(&CamMouseRecenterDeltaCpp),{0,0,0}},
    Entry{"CamFollowMouseDistanceCpp","ffo",&Exe54,&Dll54,std::bit_cast<uintptr_t>(&CamFollowMouseDistanceCpp),{0,0,0}},
    Entry{"CamFollowMouseRadiusCpp","ffffo",&Exe55,&Dll55,std::bit_cast<uintptr_t>(&CamFollowMouseRadiusCpp),{0,0,0}},
    Entry{"CamRound4ScaleCpp","ifo",&Exe56,&Dll56,std::bit_cast<uintptr_t>(&CamRound4ScaleCpp),{0,0,0}},
    Entry{"CamRound4FinishCpp","fio",&Exe57,&Dll57,std::bit_cast<uintptr_t>(&CamRound4FinishCpp),{0,0,0}},
    Entry{"CamFollowPedAutoAlphaCpp","ffffof",&Exe58,&Dll58,std::bit_cast<uintptr_t>(&CamFollowPedAutoAlphaCpp),{0,0,0}},
    Entry{"CamFollowPedPitchCorrectionCpp","fffoffofo",&Exe59,&Dll59,std::bit_cast<uintptr_t>(&CamFollowPedPitchCorrectionCpp),{0,0,0}},
    Entry{"CamFollowPedDistanceCpp","uuooff",&Exe60,&Dll60,std::bit_cast<uintptr_t>(&CamFollowPedDistanceCpp),{0,0,0}},
    Entry{"CamFollowPedHistoryPitchCpp","fofo",&Exe61,&Dll61,std::bit_cast<uintptr_t>(&CamFollowPedHistoryPitchCpp),{0,0,0}},
    Entry{"CamFollowPedHistoryBaseCpp","fffo",&Exe62,&Dll62,std::bit_cast<uintptr_t>(&CamFollowPedHistoryBaseCpp),{0,0,0}},
    Entry{"CamFollowPedResetHistoryCpp","uuufouf",&Exe63,&Dll63,std::bit_cast<uintptr_t>(&CamFollowPedResetHistoryCpp),{0,0,0}},
    Entry{"CamFollowPedHistoryCpp","ffoofoofoouufu",&Exe64,&Dll64,std::bit_cast<uintptr_t>(&CamFollowPedHistoryCpp),{0,0,0}},
    Entry{"CamFollowPedHistoryHeadingCpp","ffo",&Exe65,&Dll65,std::bit_cast<uintptr_t>(&CamFollowPedHistoryHeadingCpp),{0,0,0}},
    Entry{"CamM16BlendCpp","fofuufouu",&Exe66,&Dll66,std::bit_cast<uintptr_t>(&CamM16BlendCpp),{0,0,0}},
    Entry{"CamM16BumpCpp","iuuuuff",&Exe67,&Dll67,std::bit_cast<uintptr_t>(&CamM16BumpCpp),{0,0,0}},
    Entry{"CamM16StickInputsCpp","fifffooo",&Exe68,&Dll68,std::bit_cast<uintptr_t>(&CamM16StickInputsCpp),{0,0,0}},
    Entry{"CamM16MouseAnglesCpp","ffofuu",&Exe69,&Dll69,std::bit_cast<uintptr_t>(&CamM16MouseAnglesCpp),{0,0,0}},
    Entry{"CamM16SideRayCpp","fifoooouu",&Exe70,&Dll70,std::bit_cast<uintptr_t>(&CamM16SideRayCpp),{0,0,0}},
    Entry{"CamRocketFrontCpp","ffooo",&Exe71,&Dll71,std::bit_cast<uintptr_t>(&CamRocketFrontCpp),{0,0,0}},
    Entry{"CamRocketHeadingCpp","ffpfo",&Exe72,&Dll72,std::bit_cast<uintptr_t>(&CamRocketHeadingCpp),{0,0,0}},
    Entry{"CamWheelUpCpp","ifffofofffofoooo",&Exe73,&Dll73,std::bit_cast<uintptr_t>(&CamWheelUpCpp),{0,0,0}},
    Entry{"Reuse_TrigSimpleSinCpp","fo",&Exe74,&Dll74,std::bit_cast<uintptr_t>(&TrigSimpleSinCpp),{0,0,0}},
    Entry{"Reuse_TrigSimpleCosCpp","fo",&Exe75,&Dll75,std::bit_cast<uintptr_t>(&TrigSimpleCosCpp),{0,0,0}},
    Entry{"Reuse_Atan2MinusCpp","fffo",&Exe76,&Dll76,std::bit_cast<uintptr_t>(&Atan2MinusCpp),{0,0,0}},
    Entry{"Reuse_TrigAtan2Cpp","ffo",&Exe77,&Dll77,std::bit_cast<uintptr_t>(&TrigAtan2Cpp),{0,0,0}},
    Entry{"Reuse_Atan2MinusCpp_neg","fffo",&Exe78,&Dll78,std::bit_cast<uintptr_t>(&Atan2MinusCpp),{0,0,0}},
    Entry{"Reuse_CnvDecayPowCpp","ffo",&Exe79,&Dll79,std::bit_cast<uintptr_t>(&CnvDecayPowCpp),{0,0,0}},
    Entry{"Reuse_Atan2MinusCpp_const","ffo",&Exe80,&Dll80,std::bit_cast<uintptr_t>(&Atan2MinusCpp),{0,0,0}},
    Entry{"Reuse_CnvDecayPowCpp_const","o",&Exe81,&Dll81,std::bit_cast<uintptr_t>(&CnvDecayPowCpp),{0,0,0}},
    Entry{"Reuse_IntRatioCpp","iio",&Exe82,&Dll82,std::bit_cast<uintptr_t>(&IntRatioCpp),{0,0,0}},
    Entry{"Reuse_CnvAsinFloatCpp","fo",&Exe83,&Dll83,std::bit_cast<uintptr_t>(&CnvAsinFloatCpp),{0,0,0}},
}};

using ErrnoFn = int*(__cdecl*)();

// Llama a fn(fx) midiendo EBX/ESI/EDI/EBP justo antes y despues de la llamada, sin codigo del llamador en medio (el codigo de Debug usa ESI para las
// comprobaciones de pila /RTC1 y falsearia la medida). out[0..3] = antes, out[4..7] = despues. naked: sin prologo que guarde ni cambie registros; __asm no vale en una lambda.
__declspec(naked) inline void __cdecl CallStepMeasured(StepFn fn, Fx* fx, uintptr_t* out) {
    __asm {
        mov eax,[esp+12]
        mov [eax],ebx
        mov [eax+4],esi
        mov [eax+8],edi
        mov [eax+12],ebp
        push dword ptr [esp+8]
        call dword ptr [esp+8]
        add esp,4
        mov eax,[esp+12]
        mov [eax+16],ebx
        mov [eax+20],esi
        mov [eax+24],edi
        mov [eax+28],ebp
        ret
    }
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppcamx87stress1-runtime.start"))return;
    if(!log.is_open()) {
        log.open(dir/"cppcamx87stress1-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("cppcamx87stress1");
        Write({{"event","waiting_for_player"}});
    }
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;

    const std::array<uintptr_t,3> exeWatched{0x822130,0x821E70,0x82B181};
    const bool exeUnhooked = !ReversingProbe::JumpsToDll(0x822130) && !ReversingProbe::JumpsToDll(0x821E70);
    bool dllOutside = true; for(const auto& e:entries) dllOutside &= e.dllEntry>=0x1000000;
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> watched;
    for(auto at:exeWatched) watched.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    for(const auto& e:entries) watched.push_back({e.dllEntry,ReversingProbe::Bytes(reinterpret_cast<void*>(e.dllEntry),16)});
    const unsigned savedFP=_controlfp(0,0);
    int* const exeErrno=reinterpret_cast<ErrnoFn>(0x82B181)();
    const int savedErrno=*exeErrno;
    auto restore=[&]{ _clearfp(); _controlfp(savedFP,_MCW_PC|_MCW_RC); *exeErrno=savedErrno; };
    auto restored=[&]{
        bool ok = _controlfp(0,0)==savedFP && *exeErrno==savedErrno;
        for(auto& [at,bytes]:watched) ok &= bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    Write({{"event","begin"},{"functions",int(entries.size())},{"pairs",int(entries.size())*kCases},{"registered_hook",false},{"direct_dll_call",dllOutside},
           {"exe_unhooked",exeUnhooked},{"collaborators_stubbed",false},{"fp_control",savedFP}});
    if(!(dllOutside&&exeUnhooked)) {
        restore(); const bool stateOK=restored();
        Write({{"event","error"},{"reason","ayudantes de la DLL no directos o EXE hookeado"},{"state_restored",stateOK}});
        return;
    }

    unsigned pair=0; bool passed=true, allRestored=true;
    for(size_t f=0;f<entries.size();++f) {
        int failures=0;
        const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<unsigned(kCases);++fixture,++pair) {
            auto run=[&](bool candidate) -> json {
                restore();
                gStack=true; gRegs=true;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                Fx fx; Fill(fx,entries[f].spec,fixture); Fixup(fx,entries[f].name,fixture);
                // Estado global que el bloque lee y escribe (CamAimMeleeBlendCpp: 0xB70100, 0xB700FC, 0xB700F4): valores del fixture y restauracion tras cada lado.
                uint32 savedGlobals[3]{};
                for(int k=0;k<3;++k) if(entries[f].globals[k]) { savedGlobals[k]=*reinterpret_cast<uint32*>(entries[f].globals[k]); *reinterpret_cast<uint32*>(entries[f].globals[k])=Bits(GenF(fixture,40u+uint32(k))); }
                const uint16 topBefore=Top();
                uintptr_t regs[8]{};
                CallStepMeasured(candidate ? entries[f].dll : entries[f].exe,&fx,regs);
                gRegs&=(regs[0]==regs[4]&&regs[1]==regs[5]&&regs[2]==regs[6]&&regs[3]==regs[7]);
                const uint16 topAfter=Top();
                const unsigned fpAfter=_controlfp(0,0);
                const bool stackOK=gStack, regsOK=gRegs;
                json globals=json::array();
                for(int k=0;k<3;++k) if(entries[f].globals[k]) { globals.push_back(Hex(*reinterpret_cast<uint32*>(entries[f].globals[k]))); *reinterpret_cast<uint32*>(entries[f].globals[k])=savedGlobals[k]; }
                restore();
                const bool protectedOK=(fpAfter&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);
                json outs=json::array(); for(float v:fx.out) outs.push_back(Hex(Bits(v)));
                return json{{"route_ok",candidate ? entries[f].dllEntry>=0x1000000 : true},{"stack_restored",stackOK},{"nonvolatile_registers_preserved",regsOK},
                            {"protected_state_unchanged",protectedOK},{"x87_stack_balanced",topBefore==topAfter},{"outputs",outs},{"globals",globals},{"buffers",Hex(uint32(Digest(fx.buf,sizeof(fx.buf))))+Hex(uint32(Digest(fx.buf,sizeof(fx.buf))>>32))}};
            };
            const auto original=run(false), candidate=run(true);
            const bool stateOK=restored(), equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["protected_state_unchanged"]==true&&original["x87_stack_balanced"]==true;
            failures+=!accepted; passed&=accepted; allRestored&=stateOK;
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},{"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",kCases},{"failures",failures}});
    }
    restore(); allRestored&=restored();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
