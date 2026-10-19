#pragma once
// Fixtures del banco CameraSoundDist30: CCamera::UpdateSoundDistances en 0x515BD0 (thiscall, ECX = this, sin argumentos, `ret`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
// Reparto: familia = n % 16 (256 casos por familia, 4096 en total).
//
// Una sola arena: [guarda 0x40][CCamera 0xD78][guarda 0x40][entidad falsa 0x40][guarda 0x40][CMatrix 0x48][guarda 0x40] = 0xF00 bytes.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace CameraSoundDist30Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFamilies = 16;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ disposicion de memoria (guarda 0x40 entre bloques)
constexpr u32 kGuard = 0x40;
constexpr u32 kObjOffset = kGuard;                                   // 0x040
constexpr u32 kObjBytes = 0xD78;                                     // sizeof(CCamera)
constexpr u32 kEntityOffset = kObjOffset + kObjBytes + kGuard;       // 0xDF8
constexpr u32 kEntityBytes = 0x40;                                   // solo se lee el byte +0x36 (m_nType)
constexpr u32 kMatrixOffset = kEntityOffset + kEntityBytes + kGuard; // 0xE78
constexpr u32 kMatrixBytes = 0x48;                                   // sizeof(CMatrix); solo se lee la posicion en +0x30
constexpr u32 kArena = kMatrixOffset + kMatrixBytes + kGuard;        // 0xF00
static_assert(kEntityOffset == 0xDF8 && kMatrixOffset == 0xE78 && kArena == 0xF00, "arena: guarda 0x40 + camara 0xD78 + guarda + entidad 0x40 + guarda + matriz 0x48 + guarda");
// campos de CCamera (desplazamientos del EXE)
constexpr u32 kOffPlacePos = 0x04, kOffMatrix = 0x14, kOffActive = 0x59, kOffCams = 0x174, kCamStride = 0x238, kOffCamMode = 0x0C;
constexpr u32 kOffSoundUp = 0x158, kOffSoundNew = 0x15C, kOffSoundOld = 0x160, kWrittenBytes = 12;   // se escribe 0x158..0x164
constexpr u32 kOffTarget = 0x958, kOffForward = 0x984;
constexpr u32 kEntityTypeOffset = 0x36;
constexpr u32 kMatrixPosOffset = 0x30;
constexpr u32 kFrameCounterAddress = 0xB7CB4C;

// ------------------------------------------------------------------------------------------------ valores notables
inline u32 Quiet(u32 b) {   // nunca sNaN: el x87 los silencia al cargar con reglas que el oraculo no modela
    if ((b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) && !(b & 0x400000u)) b |= 0x400000u;
    return b;
}
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F3504F3u, 0xBF3504F3u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x41A00000u, 0xC1A00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00002u, 0x7FE00000u, 0xFFE00000u, 0x7FC7FFFEu};
inline constexpr std::array<u32, 6> kZeroOne{0x00000000u, 0x80000000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u};
// Modos de camara que consultan el objetivo (0x515BD0..0x515C35), en el orden del EXE.
inline constexpr std::array<u32, 13> kListModes{0x10, 0x07, 0x27, 0x28, 0x34, 0x2A, 0x2B, 0x29, 0x2D, 0x2E, 0x22, 0x08, 0x33};
// Modos que NO estan en la lista (valores de 16 bits).
inline constexpr std::array<u32, 20> kOtherModes{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x09, 0x0F, 0x11, 0x15, 0x21, 0x23, 0x26, 0x2C, 0x2F, 0x32, 0x35, 0x40, 0x7FFF};
inline constexpr std::array<u32, 8> kFrames{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFCu, 0x0000000Bu, 0x0000000Cu, 0x00000001u};

inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Componente "con mezcla": NaN, infinito, ceros, subnormal, enorme, especial o normal.
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 7) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Pick(kZeroOne, c, k + 100) & 0x80000000u;
    case 3: return (Rand(c, k + 100) & 0x807FFFFFu);
    case 4: return RandFloat(c, k + 100, 100, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    default: return RandFloat(c, k + 100, -6, 1);
    }
}

struct F {
    u32 family;
    u32 mode;           // palabra completa de CCam::m_nMode de la camara activa (la funcion compara solo los 16 bits bajos)
    u32 active;         // m_nActiveCam (0..2)
    u32 etype;          // byte +0x36 de la entidad objetivo (m_nType en los 3 bits bajos)
    u32 fwd[3];         // vector hacia delante de m_mCameraMatrix (+0x984)
    u32 hasMatrix;      // m_matrix distinto de nulo
    u32 matPos[3];      // posicion de la matriz (+0x30)
    u32 placePos[3];    // posicion propia (+4)
    u32 frame;          // CTimer::m_FrameCounter
    u32 old158, old15C, old160;
    u32 hit, hitZ, retHigh;   // colaborador: resultado (0/1), z del punto de choque, 24 bits altos de EAX
    u32 eax, edx, stack, headSeed, entSeed, matSeed;
    u32 otherModes[2];  // palabras de modo de las camaras no activas (decoy)
};

inline u32 PlaceFrame(u32 c, u32 phase) { return 12u * (Rand(c, 8) % 0x1000000u) + phase; }

inline F MakeF(unsigned n) {
    F f{};
    const u32 c = n;
    f.family = n % kFamilies;
    f.active = Rand(c, 1) % 3;
    f.mode = kOtherModes[Rand(c, 2) % kOtherModes.size()];
    f.etype = (Rand(c, 3) & 1) ? 3u : (Rand(c, 4) & 0xFFu);
    for (u32 i = 0; i < 3; ++i) f.fwd[i] = RandFloat(c, 10 + i, -4, 1);
    f.hasMatrix = Rand(c, 6) & 1;
    for (u32 i = 0; i < 3; ++i) { f.matPos[i] = RandFloat(c, 14 + i, -3, 10); f.placePos[i] = RandFloat(c, 17 + i, -3, 10); }
    u32 phase = Rand(c, 7) % 12;
    f.old158 = RandFloat(c, 20, -6, 5); f.old15C = RandFloat(c, 21, -6, 5); f.old160 = RandFloat(c, 22, -6, 5);
    f.hit = Rand(c, 9) & 1; f.hitZ = RandFloat(c, 23, -3, 10); f.retHigh = Rand(c, 24) & 0xFFFFFFu;
    f.eax = Rand(c, 25); f.edx = Rand(c, 26); f.stack = StackSeed(n);
    f.headSeed = Rand(c, 27); f.entSeed = Rand(c, 28); f.matSeed = Rand(c, 29);
    f.otherModes[0] = Rand(c, 30); f.otherModes[1] = Rand(c, 31);
    const u32 listMode = kListModes[Rand(c, 32) % kListModes.size()];
    switch (f.family) {
    case 0: phase = 0; f.hit = 1; break;                                                      // tipico, rayo con choque
    case 1: phase = 0; f.hit = 0; break;                                                      // tipico, rayo sin choque (20)
    case 2: phase = 1 + Rand(c, 7) % 11; break;                                               // solo mezcla (fases 1..11)
    case 3: f.mode = listMode; f.etype = 3u | (Rand(c, 4) & 0xF8u); break;                    // modo de la lista y objetivo ped (bits altos libres): escala 0,5
    case 4: f.mode = listMode; { constexpr u32 lows[7]{0, 1, 2, 4, 5, 6, 7}; f.etype = (Rand(c, 4) & 0xF8u) | lows[Rand(c, 5) % 7u]; } break;   // modo de la lista, objetivo no ped
    case 5: {                                                                                  // palabra de modo con bits altos / cercana a la lista: compara solo 16 bits
        f.etype = 3u | (Rand(c, 4) & 0xF8u);
        switch (Rand(c, 33) % 4) {
        case 0: f.mode = listMode | ((1u + Rand(c, 34) % 0xFFFFu) << 16); break;                 // en la lista (16 bits bajos) con basura arriba: escala 0,5
        case 1: f.mode = (listMode ^ (1u << (8 + Rand(c, 34) % 8))); break;                       // un bit alto de los 16 distinto: fuera de la lista
        case 2: f.mode = (listMode + 1u) | ((Rand(c, 34) & 1u) << 16); break;                     // vecino
        default: f.mode = (listMode - 1u) | (Rand(c, 34) & 0xFFFF0000u); break;
        }
        break;
    }
    case 6: for (u32 i = 0; i < 3; ++i) f.fwd[i] = MixComponent(c, 40 + i * 7); if (Rand(c, 35) & 1) { f.mode = listMode; f.etype = 3u; } break;       // forward con mezcla
    case 7: for (u32 i = 0; i < 3; ++i) { f.matPos[i] = MixComponent(c, 50 + i * 7); f.placePos[i] = MixComponent(c, 70 + i * 7); } break;           // posicion con mezcla
    case 8: {                                                                                  // exponente extendido: forward*5 se pasa de FLT_MAX pero la suma con la posicion (de signo opuesto) no
        const bool sameSign = (Rand(c, 36) % 4) == 0;
        for (u32 i = 0; i < 3; ++i) {
            const u32 sg = Rand(c, 44 + i) & 0x80000000u;
            if (i == 2) {   // z: el EXE guarda forward.z * escala como float antes de sumar la posicion, asi que aqui no hay exponente extendido
                f.fwd[i] = RandFloat(c, 40 + i, 0, 1) ; f.matPos[i] = RandFloat(c, 47 + i, -2, 6);
            } else if (Rand(c, 51 + i) % 4) {   // fwd en [2^126, 1,25 * 2^126), pos en [2^127, 2^128) de signo opuesto: 5 * fwd > FLT_MAX y la suma es finita
                f.fwd[i] = sg | (253u << 23) | (Rand(c, 40 + i) & 0x1FFFFFu);
                f.matPos[i] = (sg ^ 0x80000000u) | (254u << 23) | (Rand(c, 47 + i) & 0x7FFFFFu);
            } else {
                f.fwd[i] = (RandFloat(c, 40 + i, 123, 127) & 0x7FFFFFFFu) | sg;
                f.matPos[i] = (RandFloat(c, 47 + i, 125, 127) & 0x7FFFFFFFu) | (sameSign ? sg : (sg ^ 0x80000000u));
            }
            f.placePos[i] = f.matPos[i];
        }
        if (Rand(c, 37) % 4 == 0) { f.mode = listMode; f.etype = 3u; }
        break;
    }
    case 9:                                                                                    // subnormales y diminutos
        for (u32 i = 0; i < 3; ++i) {
            f.fwd[i] = (Rand(c, 40 + i) & 1) ? (Rand(c, 44 + i) & 0x807FFFFFu) : RandFloat(c, 47 + i, -126, -90);
            f.matPos[i] = (Rand(c, 50 + i) & 1) ? (Rand(c, 54 + i) & 0x807FFFFFu) : RandFloat(c, 57 + i, -126, -90);
            f.placePos[i] = (Rand(c, 60 + i) & 1) ? (Rand(c, 64 + i) & 0x807FFFFFu) : RandFloat(c, 67 + i, -126, -90);
        }
        if (Rand(c, 37) & 1) { f.mode = listMode; f.etype = 3u; }
        break;
    case 10: f.old15C = MixComponent(c, 40); f.old160 = MixComponent(c, 47); f.old158 = MixComponent(c, 54); if (Rand(c, 35) % 3) phase = 1 + Rand(c, 7) % 11; break;   // valores previos con mezcla
    case 11: {                                                                                 // mezcla con exponente extendido (t > 1: (1 - t) < 0)
        phase = 6 + Rand(c, 7) % 6;
        const u32 p = RandFloat(c, 40, 124, 127) & 0x7FFFFFFFu, sg = Rand(c, 41) & 0x80000000u;
        f.old160 = p | sg;
        f.old15C = (Rand(c, 42) & 1) ? f.old160 : ((p + (Rand(c, 43) % 5) - 2u) | sg);
        break;
    }
    case 12: phase = 0; f.hit = 1; f.hitZ = MixComponent(c, 40); for (u32 i = 0; i < 3; ++i) if (Rand(c, 44 + i) % 4 == 0) f.matPos[i] = f.placePos[i] = MixComponent(c, 50 + i * 7); break;   // z del choque con mezcla
    case 13: f.frame = (Rand(c, 38) & 1) ? Pick(kFrames, c, 39) : (Rand(c, 8) | ((Rand(c, 38) & 2) ? 0x80000000u : 0u)); break;     // contador extremo
    case 14: {                                                                                 // camara activa y decoys: las otras camaras llevan modos de la lista
        f.active = (n / 16) % 3; f.mode = (Rand(c, 33) & 1) ? listMode : kOtherModes[Rand(c, 2) % kOtherModes.size()];
        f.otherModes[0] = kListModes[Rand(c, 34) % kListModes.size()]; f.otherModes[1] = kListModes[Rand(c, 35) % kListModes.size()];
        f.etype = (f.mode == listMode) ? ((Rand(c, 36) & 1) ? 3u : 4u) : 3u;
        break;
    }
    default:                                                                                   // 15: bits crudos (nunca sNaN), modo aleatorio
        for (u32 i = 0; i < 3; ++i) { f.fwd[i] = Quiet(Rand(c, 40 + i)); f.matPos[i] = Quiet(Rand(c, 44 + i)); f.placePos[i] = Quiet(Rand(c, 48 + i)); }
        f.old158 = Quiet(Rand(c, 52)); f.old15C = Quiet(Rand(c, 53)); f.old160 = Quiet(Rand(c, 54)); f.hitZ = Quiet(Rand(c, 55));
        f.mode = Rand(c, 56); if ((Rand(c, 57) & 1)) f.mode = (f.mode & 0xFFFF0000u) | listMode;
        break;
    }
    if (f.family != 13) f.frame = PlaceFrame(c, phase);
    // la entidad objetivo es siempre valida y solo se lee con un modo de la lista; si el modo no esta en la lista el EXE no la toca
    return f;
}
}
