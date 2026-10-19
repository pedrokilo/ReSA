#pragma once
// Fixtures del banco GaragesAddOne30: CGarages::AddOne (0x4471E0, cdecl, 12 dwords de argumentos, `ret` normal, el indice del garaje vuelve en EAX).
//   pila (tras la direccion de retorno): +04 x1, +08 y1, +0C z1, +10 frontX, +14 frontY, +18 x2, +1C y2, +20 z2 (floats), +24 type (BYTE), +28 a10 (no se lee), +2C name (char*), +30 flags (solo el byte bajo).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// Memoria: el banco SUSTITUYE el array CGarages::aGarages (0x96C048, 50 ranuras de 0xD8 bytes = 0x2A30 bytes) y CGarages::NumGarages (0x96C024) por los del caso (se guardan los reales y se restauran
// al final). Todas las ranuras valen `fill`, salvo la ranura `index` (= NumGarages), que lleva `entry` (0xD8 bytes aleatorios): AddOne solo debe tocar los campos que el oraculo modela.
// El nombre vive en una arena de 32 bytes (guarda 0xCC, el nombre en el desplazamiento 8 y un 0 de terminacion); AddOne copia como mucho 7 caracteres con strncpy.
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>

namespace GaragesAddOne30Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kKinds = 16;
constexpr unsigned kPerKind = 256;
constexpr unsigned kCases = kKinds * kPerKind;          // 4096
constexpr u32 kEntryBytes = 0xD8;                       // sizeof(CGarage)
constexpr u32 kMaxGarages = 50;
constexpr u32 kArrayBytes = kMaxGarages * kEntryBytes;  // 0x2A30
constexpr u32 kArrayAddress = 0x96C048;
constexpr u32 kNumAddress = 0x96C024;
constexpr u32 kNameArena = 32;
constexpr u32 kNameOffset = 8;
constexpr u32 kMaxName = 14;                            // 8 + 14 + 1 (terminador) <= 32
static_assert(kArrayBytes == 0x2A30 && kEntryBytes * kMaxGarages == 10800, "tamano del array de garajes");
static_assert(kNameOffset + kMaxName + 1 <= kNameArena, "la arena del nombre cabe");

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
constexpr u32 Fb(float f) { return std::bit_cast<u32>(f); }

// ------------------------------------------------------------------------------------------------ valores notables (bits de float32)
inline constexpr u32 kSpecial[]{
    0u, 0x80000000u, Fb(1.0f), Fb(-1.0f), Fb(0.5f), Fb(-0.5f), Fb(2.0f), Fb(0.1f), Fb(0.3f), Fb(100.0f), Fb(-250.5f),
    0x00000001u /* menor subnormal */, 0x80000001u, 0x007FFFFFu, 0x00800000u /* menor normal */, Fb(1.0e-30f), Fb(1.0e-20f), Fb(-1.0e-20f), Fb(1.0e-10f),
    Fb(1.0e10f), Fb(1.0e19f), Fb(1.8e19f) /* cuadrado desborda float */, Fb(-1.8e19f), Fb(3.0e19f), Fb(1.0e30f), Fb(3.0e38f), Fb(-3.0e38f), 0x7F7FFFFFu, 0xFF7FFFFFu,
    Fb(16777216.0f), Fb(16777218.0f), Fb(-16777216.0f), Fb(8388608.5f), Fb(1.0e8f), Fb(1.0e-7f),
    0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC12345u, 0xFFC00001u, 0x7F800001u /* sNaN */, 0x7FA00000u, 0xFF800001u, 0xFFBFFFFFu,
};
inline constexpr unsigned kSpecialCount = std::size(kSpecial);
inline constexpr u32 kNans[]{0x7FC00000u, 0xFFC00000u, 0x7FC12345u, 0xFFC00001u, 0x7F800001u, 0x7FA00000u, 0xFF800001u, 0x7FC00002u, 0xFFC12345u, 0x7FFFFFFFu};
inline constexpr u32 kInfs[]{0x7F800000u, 0xFF800000u};
inline constexpr u32 kHuge[]{Fb(1.0e19f), Fb(-1.0e19f), Fb(1.8e19f), Fb(-1.8e19f), Fb(3.0e19f), Fb(1.0e30f), Fb(-1.0e30f), Fb(3.0e38f), Fb(-3.0e38f), 0x7F7FFFFFu, 0xFF7FFFFFu, Fb(1.7e38f)};
inline constexpr u32 kTiny[]{0x00000001u, 0x80000001u, 0x00000003u, 0x007FFFFFu, 0x00800000u, 0x80800000u, Fb(1.0e-30f), Fb(-1.0e-30f), Fb(1.0e-20f), Fb(-1.0e-20f), Fb(1.0e-10f), Fb(3.0e-20f), Fb(7.0e-39f)};
inline constexpr u32 kZeros[]{0u, 0x80000000u, Fb(1.0f), Fb(-1.0f), Fb(0.5f)};

struct Fx {
    unsigned n{};
    u32 kind{};
    u32 in[8]{};                       // x1, y1, z1, frontX, frontY, x2, y2, z2 (bits)
    u32 type{}, a10{}, flags{};        // tal como se empujan (type y flags: solo cuenta el byte bajo / los tres bits bajos)
    std::string name;                  // bytes no nulos, 0..kMaxName
    u32 index{};                       // NumGarages antes de la llamada
    u8 fill{};
    std::array<u8, kEntryBytes> entry{};
    u32 eax{}, ecx{}, edx{}, stack{};
};

inline u32 Typical(u32 c, u32 k) { return Fb(float(int(Rand(c, k) % 6000001u) - 3000000) / 1000.0f); }
inline u32 Lattice(u32 c, u32 k, u32 n) { return Fb(float(Rand(c, k) % n)); }
inline u32 Special(u32 c, u32 k) { return kSpecial[Rand(c, k) % kSpecialCount]; }
inline u32 PickNan(u32 c, u32 k) { return kNans[Rand(c, k) % std::size(kNans)]; }
inline u32 PickFrom(const u32* a, std::size_t n, u32 c, u32 k) { return a[Rand(c, k) % n]; }
// coordenadas: 0 x1, 1 y1, 3 frontX, 4 frontY, 5 x2, 6 y2; z: 2 y 7
inline constexpr unsigned kCoord[6]{0, 1, 3, 4, 5, 6};

inline Fx Make(unsigned n) {
    Fx f;
    const u32 c = n;
    f.n = n;
    f.kind = n % kKinds;
    const u32 round = n / kKinds;                    // 0..255
    for (unsigned i = 0; i < 8; ++i) f.in[i] = Typical(c, 1 + i);
    f.type = Rand(c, 20) % 3 == 0 ? Rand(c, 21) : (Rand(c, 22) % 0x30u);
    f.a10 = Rand(c, 23);
    f.flags = Rand(c, 24) % 2 ? (Rand(c, 25) & 7u) : Rand(c, 26);
    f.index = Rand(c, 27) % 4 == 0 ? Rand(c, 28) % kMaxGarages : Rand(c, 29) % 20u;
    f.fill = u8(0x90 + Rand(c, 30) % 0x60);
    FillWords(f.entry.data(), kEntryBytes, Rand(c, 31));
    f.eax = Rand(c, 32); f.ecx = Rand(c, 33); f.edx = Rand(c, 34);
    f.stack = Rand(c, 35) % 2 ? kStackSeeds[Rand(c, 36) % kStackSeeds.size()] : Rand(c, 37);
    const u32 len = Rand(c, 38) % 4 == 0 ? Rand(c, 39) % (kMaxName + 1) : (3 + Rand(c, 40) % 6);
    for (u32 i = 0; i < len; ++i) {
        u32 b = Rand(c, 41 + i) % 255u + 1u;           // 1..255, nunca 0
        if (Rand(c, 60 + i) % 2) b = 0x41u + Rand(c, 70 + i) % 26u;
        f.name += char(b);
    }
    switch (f.kind) {
    case 0:                                          // tipicos
        break;
    case 1:                                          // retícula pequena: empates y todos los ordenes de los limites
        for (const unsigned i : kCoord) f.in[i] = Lattice(c, 80 + i, 4);
        break;
    case 2:                                          // ceros con signo y unidades
        for (const unsigned i : kCoord) f.in[i] = PickFrom(kZeros, std::size(kZeros), c, 80 + i);
        break;
    case 3:                                          // NaN en algunas coordenadas (al menos una)
        for (const unsigned i : kCoord) if (Rand(c, 80 + i) % 3 == 0) f.in[i] = PickNan(c, 90 + i);
        f.in[kCoord[Rand(c, 99) % 6]] = PickNan(c, 100);
        break;
    case 4:                                          // infinitos (al menos uno)
        for (const unsigned i : kCoord) if (Rand(c, 80 + i) % 3 == 0) f.in[i] = PickFrom(kInfs, 2, c, 90 + i);
        f.in[kCoord[Rand(c, 99) % 6]] = PickFrom(kInfs, 2, c, 100);
        break;
    case 5:                                          // enormes: sumas y cuadrados desbordan float pero no la x87
        for (const unsigned i : kCoord) f.in[i] = Rand(c, 80 + i) % 4 == 0 ? Typical(c, 90 + i) : PickFrom(kHuge, std::size(kHuge), c, 100 + i);
        break;
    case 6:                                          // diminutos y subnormales: los cuadrados desbordan por abajo en float pero no en la x87
        for (const unsigned i : kCoord) f.in[i] = Rand(c, 80 + i) % 5 == 0 ? Special(c, 90 + i) : PickFrom(kTiny, std::size(kTiny), c, 100 + i);
        break;
    case 7:                                          // direcciones degeneradas (longitud cero): x/0, 0/0
        f.in[3] = f.in[0]; f.in[4] = f.in[1];
        if (Rand(c, 80) % 2) { f.in[5] = f.in[0]; f.in[6] = f.in[1]; }
        if (Rand(c, 81) % 3 == 0) f.in[3] = Rand(c, 82) % 2 ? Fb(-0.0f) : Fb(0.0f);
        if (Rand(c, 83) % 3 == 0) f.in[0] = Rand(c, 84) % 2 ? Fb(0.0f) : Fb(-0.0f);
        if (Rand(c, 85) % 4 == 0) f.in[Rand(c, 86) % 2 ? 4 : 6] = Special(c, 87);
        break;
    case 8: {                                        // redondeo a 24 bits: sumas de magnitudes muy distintas y casi empates
        const float base = float(int(Rand(c, 80) % 40000000u) + 1000000) * (Rand(c, 81) % 2 ? 1.0f : -1.0f);
        for (const unsigned i : kCoord) {
            const u32 r = Rand(c, 82 + i) % 4;
            f.in[i] = r == 0 ? Fb(base) : r == 1 ? Fb(base + float(int(Rand(c, 90 + i) % 9) - 4)) : r == 2 ? Fb(float(int(Rand(c, 100 + i) % 2001) - 1000) / 1000.0f) : Special(c, 110 + i);
        }
        break;
    }
    case 9:                                          // type / flags / a10 con basura en los bytes altos; sweep de los ocho valores de flags
        f.type = (round & 0xFFu) | (Rand(c, 120) & 0xFFFFFF00u);
        f.flags = (round & 7u) | (Rand(c, 121) % 2 ? (Rand(c, 122) & 0xFFFFFFF8u) : 0u);
        f.a10 = Rand(c, 123) % 3 ? Rand(c, 124) : 0xFFFFFFFFu;
        break;
    case 10: {                                       // nombres: longitudes 0..14, bytes altos, exactamente 7 y 8
        const u32 want = round % (kMaxName + 1);
        f.name.clear();
        for (u32 i = 0; i < want; ++i) f.name += char(Rand(c, 130 + i) % 3 == 0 ? 0x80u + Rand(c, 150 + i) % 0x7Fu : 0x30u + Rand(c, 170 + i) % 0x4Au);
        break;
    }
    case 11: {                                       // indices frontera y ranura vieja distinta (bytes de flags previos)
        static constexpr u32 idx[]{0u, 1u, 2u, 24u, 25u, 47u, 48u, 49u};
        f.index = idx[round % std::size(idx)];
        f.fill = u8(Rand(c, 130));
        break;
    }
    case 12:                                         // bits crudos en las coordenadas (cualquier patron)
        for (const unsigned i : kCoord) if (Rand(c, 80 + i) % 3 != 0) f.in[i] = Rand(c, 90 + i);
        break;
    case 13:                                         // retícula con NaN: empates y comparaciones no ordenadas
        for (const unsigned i : kCoord) f.in[i] = Rand(c, 80 + i) % 4 == 0 ? PickNan(c, 90 + i) : Lattice(c, 100 + i, 3);
        break;
    case 14: {                                       // garajes reales: rectangulo alineado o girado
        const float x1 = float(int(Rand(c, 80) % 6000001u) - 3000000) / 1000.0f, y1 = float(int(Rand(c, 81) % 6000001u) - 3000000) / 1000.0f;
        const float w = float(Rand(c, 82) % 20000u) / 1000.0f + 1.0f, h = float(Rand(c, 83) % 20000u) / 1000.0f + 1.0f;
        f.in[0] = Fb(x1); f.in[1] = Fb(y1);
        if (Rand(c, 84) % 2) { f.in[3] = Fb(x1 + w); f.in[4] = Fb(y1); f.in[5] = Fb(x1); f.in[6] = Fb(y1 + h); }
        else { const float a = float(int(Rand(c, 85) % 10001u) - 5000) / 1000.0f, b = float(int(Rand(c, 86) % 10001u) - 5000) / 1000.0f;
               f.in[3] = Fb(x1 + a); f.in[4] = Fb(y1 + b); f.in[5] = Fb(x1 - b); f.in[6] = Fb(y1 + a); }
        break;
    }
    case 15:                                         // z1 / z2 especiales (se copian en bruto), coordenadas tipicas
        f.in[2] = Special(c, 80); f.in[7] = Special(c, 81);
        if (Rand(c, 82) % 4 == 0) f.in[2] = PickNan(c, 83);
        if (Rand(c, 84) % 4 == 0) f.in[7] = PickNan(c, 85);
        break;
    }
    if (f.name.size() > kMaxName) f.name.resize(kMaxName);
    return f;
}

inline std::string HexOf(const u8* p, std::size_t n) {
    constexpr char d[] = "0123456789abcdef";
    std::string o;
    o.reserve(n * 2);
    for (std::size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; }
    return o;
}
// Fila `input` en JSON (texto), identica para el banco (Core.hpp la parsea) y para fixture_dump.cpp (calibracion sin juego).
inline std::string InputLine(const Fx& f) {
    std::string o = "{\"kind\":" + std::to_string(f.kind) + ",\"in\":[";
    for (unsigned i = 0; i < 8; ++i) { if (i) o += ','; o += std::to_string(f.in[i]); }
    o += "],\"type\":" + std::to_string(f.type) + ",\"a10\":" + std::to_string(f.a10) + ",\"flags\":" + std::to_string(f.flags)
        + ",\"name\":\"" + HexOf(reinterpret_cast<const u8*>(f.name.data()), f.name.size()) + "\",\"index\":" + std::to_string(f.index) + ",\"fill\":" + std::to_string(f.fill)
        + ",\"entry\":\"" + HexOf(f.entry.data(), kEntryBytes) + "\",\"eax\":" + std::to_string(f.eax) + ",\"ecx\":" + std::to_string(f.ecx) + ",\"edx\":" + std::to_string(f.edx)
        + ",\"stack_seed\":" + std::to_string(f.stack) + "}";
    return o;
}
// Constantes que el oraculo (fixture_oracle.py) debe repetir EXACTAMENTE; la autoprueba cruzada las compara con este volcado.
inline std::string ConstantsLine() {
    return "{\"entry_bytes\":" + std::to_string(kEntryBytes) + ",\"max_garages\":" + std::to_string(kMaxGarages) + ",\"array_bytes\":" + std::to_string(kArrayBytes)
        + ",\"array_address\":" + std::to_string(kArrayAddress) + ",\"num_address\":" + std::to_string(kNumAddress) + ",\"name_arena\":" + std::to_string(kNameArena)
        + ",\"name_offset\":" + std::to_string(kNameOffset) + ",\"cases\":" + std::to_string(kCases) + "}";
}
}
