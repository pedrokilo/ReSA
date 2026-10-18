#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <vector>

// Fixtures deterministas del banco Dependencies26A (CAEAudioHardware::PlaySound 0x4D86B0). Solo datos: no dependen del juego ni del proyecto (las usan tambien el ensayo sin juego de offline/ y
// fixture_oracle.py, que NO las regenera: lee del registro la entrada completa serializada de cada caso). 16 familias x 128 casos = 2048 pares.
//
// PlaySound es `thiscall` con siete argumentos de pila (`ret 0x1C`); el llamador del EXE (0x4F07F8) empuja palabras de 32 bits ya extendidas, pero el EXE solo lee las palabras bajas
// (movsx/movzx/cx/bp/bl/bx), de modo que las fixtures ensucian los 16 bits altos de los seis primeros argumentos en una parte de los casos (es lo que el EXE tolera).
namespace Dependencies26AFix {
using u8 = std::uint8_t; using u16 = std::uint16_t; using u32 = std::uint32_t; using u64 = std::uint64_t; using s32 = std::int32_t;

inline constexpr unsigned kFamilies = 16;
inline constexpr unsigned kCasesPerFamily = 128;
inline constexpr unsigned kCases = kFamilies * kCasesPerFamily;
inline constexpr const char* kFamilyNames[kFamilies]{
    "valido sin porcentaje", "porcentaje", "canal negativo", "ranura fuera de rango", "sin buffer", "indice > 64", "SetAudioBuffer falla", "indice 63 y 64",
    "posicion extrema", "velocidad especial", "16 bits altos sucios", "flags especiales", "suma de 32 bits (envoltura)", "buffer/bucle/tasa extremos", "porcentaje extremo y x87", "mezcla"
};

// Disposicion del CAEAudioHardware falso (desplazamientos desde su inicio; el EXE los usa tal cual, ver README.md)
inline constexpr u32 kArena = 0x24000;          // la mayor lectura (0x90 + 2 * 0x7FFF) y la mayor escritura (6 + 2 * 0x10040) caben con holgura
inline constexpr u32 kFlagsAt = 0x06, kNumChannelsInSlotAt = 0x90, kBankLoaderAt = 0xD98, kN3dAt = 0xDA8, kChannelTableAt = 0xE64;
inline constexpr u32 kChannelFlagsAt = 0x28;    // CAEAudioChannel::m_nFlags
inline constexpr u32 kChannelCount = 65;        // 64 de la tabla + el que hay tras ella (primer dword de gBeatInfo, indice 64)

struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x2026ull) {}
    u64 Next() { u64 z = (s += 0x9E3779B97F4A7C15ull); z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; return z ^ (z >> 31); }
    u32 U32() { return u32(Next() >> 16); }
    u32 Below(u32 n) { return n ? u32(Next() % n) : 0; }
    bool Chance(u32 percent) { return Below(100) < percent; }
    template<class T, size_t N> T Pick(const T (&a)[N]) { return a[Below(u32(N))]; }
};

// Argumentos de pila de PlaySound, en orden: canal, ranura del canal, sonido, ranura de banco, posicion, flags, velocidad (bits de float)
struct Case {
    u32 n{}, family{};
    u32 args[7]{};
    u32 nslot{};        // palabra m_anNumChannelsInSlot[canal] (solo si el canal es >= 0)
    u32 buf{}, size{}, rate{}, loop{}, setret{}, length{}, n3d{}, fsw{}, fill{};
};

inline constexpr u32 kFsw[8]{0x0000, 0x0020, 0x003F, 0x4100, 0x4500, 0x4700, 0x0004, 0x0220};
inline constexpr u32 kSpeed[]{
    0x3F800000, 0x00000000, 0x80000000, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FC12345, 0x7FA00001, 0xFFBFFFFF, 0x00000001, 0x007FFFFF, 0x80000001, 0x7E967699, 0xFE967699, 0x7F7FFFFF,
    0xFF7FFFFF, 0x3F000000, 0x40000000, 0x3DCCCCCD, 0x42C80000, 0xBF800000, 0x4B000000, 0x5D5E0B6B};
inline constexpr u32 kSounds[]{0, 1, 2, 3, 7, 0x7F, 0x80, 0xFF, 0x100, 0x1FF, 0x3E7, 0x7FFF, 0x8000, 0xFFFE, 0xFFFF};
inline constexpr u32 kBanks[]{0, 1, 2, 5, 17, 40, 58, 59, 60, 0x7FFF, 0x8000, 0xFFFF, 0xFFFE};
inline constexpr u32 kLoops[]{0xFFFFFFFF, 0, 1, 2, 0x7FFF, 0x8000, 0xFFFF, 0x10000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFE, 0xFFFF8000, 0x00018000, 0x12345678, 100, 4000};
inline constexpr u32 kRates[]{0, 1, 8000, 11025, 22050, 32000, 44100, 48000, 0x7FFF, 0x8000, 0xFFFE, 0xFFFF};
inline constexpr u32 kSizes[]{0, 1, 2, 0x100, 0x2000, 0x5DC0, 0xFFFF, 0x10000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFE, 0xFFFFFFFF, 0x123456};
inline constexpr u32 kBufs[]{1, 2, 4, 0x10000, 0x00401000, 0x00858C58, 0x7FFFFFFF, 0x80000000, 0xA5A5A5A5, 0xDEADBEEF, 0xFFFFFFFC, 0xFFFFFFFF};
inline constexpr u32 kLengths[]{0, 1, 2, 3, 99, 100, 101, 1000, 4000, 32767, 32768, 32769, 65535, 65536, 65537, 600000, 0x00FFFFFF, 0x01000000, 0x01000001, 0x0123456F, 0x40000001, 0x7FFFFFFE, 0x7FFFFFFF,
                                0xFFFFFFFF, 0xFFFFFFFE, 0xFFFF8000, 0xFFFF0000, 0xFFFE7960, 0x80000000, 0x80000001, 0xC0000000};
inline constexpr u32 kPercents[]{1, 2, 3, 7, 10, 33, 50, 99, 100, 101, 255, 256, 999, 1000, 1001, 4096, 10000, 12345, 32766, 32767};
inline constexpr u32 kFlagPool[]{0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x7FFF, 0xFFFF, 0x0208, 0x0209, 0x8208, 0x82C9, 0x01F7};

inline u32 Hi(Rng& r) { return r.U32() & 0xFFFF0000u; }
inline u32 Sext16(u32 v) { return u32(s32(std::int16_t(v & 0xFFFF))); }

// Rellena el caso con un camino "valido" (llega hasta Play) y deja que cada familia lo rompa o lo estire.
inline Case MakeCase(u32 n) {
    Case c{}; c.n = n; c.family = n % kFamilies;
    Rng r(0xA6E00000ull + n * 7919ull + 26);
    const u32 f = c.family;
    c.fill = u32(r.Next()) | 1u; c.fsw = kFsw[r.Below(8)];

    u32 nsl = 1 + r.Below(8);
    u32 slot = r.Below(nsl);
    u32 ch = r.Below(56);
    if (ch + slot > 64) ch = 64 - slot;
    c.nslot = nsl;
    c.args[0] = ch; c.args[1] = slot;
    c.args[2] = r.Chance(60) ? r.Below(40) : r.Pick(kSounds);
    c.args[3] = r.Chance(70) ? r.Below(60) : r.Pick(kBanks);
    c.args[4] = r.Chance(30) ? 0 : r.Chance(60) ? r.Below(60000) : r.Below(32768);
    c.args[5] = r.Chance(50) ? (r.U32() & 0x03F7) : r.Pick(kFlagPool);
    c.args[5] &= ~8u;                                   // por defecto sin porcentaje
    c.args[6] = r.Chance(60) ? 0x3F800000u + (r.U32() & 0x7FFFFF) : r.Pick(kSpeed);
    c.buf = r.Chance(50) ? 0x10000000u + 4u * r.Below(1u << 20) : r.Pick(kBufs);
    c.size = r.Chance(50) ? 0x100u + r.Below(1u << 20) : r.Pick(kSizes);
    c.rate = r.Chance(50) ? r.Pick(kRates) : r.Below(0x10000);
    c.loop = r.Chance(40) ? 0xFFFFFFFFu : r.Pick(kLoops);
    c.setret = 1;
    c.length = r.Chance(50) ? 1 + r.Below(300000) : r.Pick(kLengths);
    c.n3d = r.Chance(40) ? 0 : r.Chance(70) ? 1 + r.Below(2) : (r.U32() & 0xFF);

    switch (f) {
    case 0: // camino valido, sin porcentaje
        break;
    case 1: // porcentaje: bit 3 y posicion > 0
        c.args[5] |= 8u;
        c.args[4] = r.Chance(50) ? r.Pick(kPercents) : 1 + r.Below(32767);
        if (r.Chance(60)) c.length = 1 + r.Below(600000);
        if (r.Chance(8)) c.length = 0;
        break;
    case 2: // canal negativo (bit 15 de la palabra baja)
        c.args[0] = 0x8000u | (r.U32() & 0x7FFF);
        if (r.Chance(30)) c.args[0] = 0xFFFF;
        if (r.Chance(10)) c.args[0] = 0x8000;
        break;
    case 3: { // ranura >= numero de canales de la ranura (cmp cx, [..]; jae): nsl, nsl + 1, 0xFFFF, 0xFFFE, 0x8000, mayor; con la cuenta a cero tambien
        if (r.Chance(15)) c.nslot = 0;
        const u32 k = r.Below(6), base = c.nslot;
        c.args[1] = (k == 0 ? base : k == 1 ? base + 1 : k == 2 ? 0xFFFFu : k == 3 ? 0xFFFEu : k == 4 ? 0x8000u : base + r.Below(200)) & 0xFFFFu;
        break; }
    case 4: // GetSoundBuffer devuelve nulo
        c.buf = 0;
        break;
    case 5: { // indice (ranura + canal) truncado a 16 bits > 0x40
        const u32 k = r.Below(5);
        const u32 idx = k == 0 ? 65 : k == 1 ? 66 : k == 2 ? 0x7FFF : k == 3 ? 100 + r.Below(500) : 65 + r.Below(1000);
        const u32 chn = r.Below(std::min<u32>(idx, 0x200) + 1);
        c.args[0] = chn; c.args[1] = idx - chn; c.nslot = c.args[1] + 1 + r.Below(3);
        if (c.nslot > 0xFFFF) c.nslot = 0xFFFF;
        break; }
    case 6: // SetAudioBuffer devuelve falso
        c.setret = 0;
        break;
    case 7: { // indice 64 (lee el primer dword de gBeatInfo) o 63
        const u32 idx = r.Chance(60) ? 64 : 63;
        const u32 chn = r.Below(idx + 1);
        c.args[0] = chn; c.args[1] = idx - chn; c.nslot = c.args[1] + 1 + r.Below(4);
        if (r.Chance(30)) c.args[4] = 1 + r.Below(5000);
        break; }
    case 8: { // posicion extrema con y sin porcentaje: negativa, cero, == longitud, +-1, mayor que la longitud (recorte con signo de 16 bits)
        const u32 k = r.Below(9);
        const bool pct = r.Chance(30);
        if (pct) c.args[5] |= 8u;
        c.length = r.Chance(50) ? r.Pick(kLengths) : 1 + r.Below(70000);
        const u32 len16 = c.length & 0xFFFF;
        c.args[4] = k == 0 ? (0x8000u | r.Below(0x8000)) : k == 1 ? 0 : k == 2 ? len16 : k == 3 ? (len16 + 1) & 0xFFFF : k == 4 ? (len16 - 1) & 0xFFFF : k == 5 ? 0x7FFF : k == 6 ? 0x8000 : k == 7 ? 0xFFFF : r.Below(0x10000);
        break; }
    case 9: // velocidad: NaN (carga y senalizante), infinitos, ceros, subnormales, limites
        c.args[6] = r.Chance(85) ? r.Pick(kSpeed) : r.U32();
        break;
    case 10: // los 16 bits altos de los seis primeros argumentos sucios
        break;
    case 11: // flags: bit 9 (0x200) y bit 3 (0x8) con y sin posicion, signo, todos los bits
        c.args[5] = r.Pick(kFlagPool) | (r.Chance(50) ? 8u : 0u);
        if (r.Chance(40)) c.args[4] = 0;
        break;
    case 12: { // suma de 32 bits distinta de la de 16: canal grande + ranura grande que dan 0x10000 + indice (<= 0x40)
        const u32 idx = r.Below(65);
        const u32 chn = 0x4000 + r.Below(0x4000);                 // 0x4000..0x7FFF
        const u32 slotv = 0x10000 + idx - chn;                     // 0x8001 + idx .. 0xC000 + idx
        c.args[0] = chn; c.args[1] = slotv & 0xFFFF; c.nslot = std::min<u32>(0xFFFF, (slotv & 0xFFFF) + 1 + r.Below(100));
        break; }
    case 13: // buffer, tamano, tasa y desplazamiento del bucle extremos
        c.buf = r.Pick(kBufs); c.size = r.Pick(kSizes); c.rate = r.Chance(70) ? r.Pick(kRates) : r.U32(); c.loop = r.Chance(80) ? r.Pick(kLoops) : r.U32();
        break;
    case 14: { // porcentaje con longitudes que hacen inexactos los productos (PE), negativas y enormes; FSW inicial variado
        c.args[5] |= 8u;
        c.args[4] = r.Chance(50) ? r.Pick(kPercents) : 1 + r.Below(32767);
        c.length = r.Chance(50) ? r.Pick(kLengths) : u32(s32(r.U32()));
        if (r.Chance(30)) { // porcentaje y longitud con pocos bits: los dos productos son exactos (la bandera PE no se levanta)
            c.args[4] = 1u << r.Below(15);
            c.length = r.Chance(15) ? 0u : r.Chance(60) ? (1u << r.Below(31)) : u32(-s32(1u << r.Below(31)));
        }
        c.fsw = kFsw[(n / kFamilies) % 8];
        break; }
    default: { // 15: mezcla libre
        const u32 k = r.Below(12);
        if (k == 0) c.args[0] = 0x8000u | r.Below(0x8000);
        else if (k == 1) c.args[1] = c.nslot + r.Below(3);
        else if (k == 2) c.buf = 0;
        else if (k == 3) c.setret = 0;
        else if (k == 4) { c.args[1] = 60 + r.Below(10); c.args[0] = r.Below(10); c.nslot = 0xFFFF; }
        else if (k >= 5 && k <= 7) { c.args[5] |= 8u; c.args[4] = 1 + r.Below(32767); }
        c.args[6] = r.Chance(30) ? r.Pick(kSpeed) : c.args[6];
        break; }
    }
    // Basura en los 16 bits altos de los seis primeros argumentos: siempre en la familia 10, a veces en las demas
    const bool dirty = f == 10 || (f != 2 && r.Chance(20)) || (f == 2 && r.Chance(50));
    if (dirty) for (int i = 0; i < 6; ++i) if (f == 10 || r.Chance(70)) c.args[i] = (c.args[i] & 0xFFFFu) | Hi(r);
    return c;
}
}
