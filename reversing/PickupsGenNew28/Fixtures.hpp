#pragma once
// Fixtures del banco PickupsGenNew28: CPickups::GenerateNewOne (0x456F20, cdecl, `ret` normal, 9 dwords de argumentos).
//   pila (tras la direccion de retorno): +04 x, +08 y, +0C z (floats), +10 modelId, +14 pickupType (BYTE), +18 ammo, +1C moneyPerDay, +20 isEmpty (BYTE), +24 message (char*)
//   devuelve en EAX el handle (referencia << 16 | indice) o -1.
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// Tabla de pickups CPickups::aPickUps (0x9788C0): 620 ranuras de 0x20 bytes. Se sustituye entera por la del caso (se guarda la real y se restaura al final):
//   todas las ranuras son "relleno" (todo 0xA5 salvo el tipo en +0x1C) y encima se colocan ranuras explicitas (indice + 32 bytes) que fijan el escenario:
//   libres (tipo 0), de dinero (tipo 8), con tiempo limite (tipo 4/5), y otras no sustituibles.
//   El campo m_pObject (+4) se escribe/observa como ETIQUETA: 0, 0x0B100001..0x0B100004 (objetos falsos del banco, con destructor virtual falso) o un valor crudo (0xA5A5A5A5,
//   0xDEADBEEF...) que nunca se desreferencia. Las ranuras de tipo 8/4/5 llevan SIEMPRE 0 o una etiqueta valida (podrian ser sustituidas).
// Busqueda de hueco del EXE: 1) tipo 0: desde el final si pickupType es 13 u 11 o isEmpty(byte) != 0, si no desde el principio; 2) primer tipo 8; 3) primer tipo 4 o 5.
//   En 2 y 3 se llama a CWorld::Remove(obj) y al destructor virtual con borrado antes de rellenar. Sin hueco: -1 y nada se escribe.
// Los bytes de pickupType e isEmpty llegan en dwords con basura en los bytes altos (el EXE solo lee el byte).
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace PickupsGen28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kKinds = 16;
constexpr unsigned kPerKind = 384;
constexpr unsigned kCases = kKinds * kPerKind;       // 6144
constexpr u32 kSlots = 620;
constexpr u32 kSlotBytes = 0x20;
constexpr u32 kTableBytes = kSlots * kSlotBytes;
constexpr u32 kTableAddress = 0x9788C0;
constexpr u32 kTimeAddress = 0xB7CB84;               // CTimer::m_snTimeInMilliseconds
constexpr u32 kLabelBase = 0x0B100000u;              // etiqueta del objeto falso k (1..4) = kLabelBase + k
constexpr unsigned kNumObjs = 4;
constexpr u32 kMsgArena = 64;

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
inline u32 Fnv32(const char* s) { u32 h = 2166136261u; for (; *s; ++s) { h ^= u8(*s); h *= 16777619u; } return h; }   // FNV-1a de 32 bits (mensaje en la traza)

constexpr u32 Fb(float f) { return std::bit_cast<u32>(f); }

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr u32 kSpecialCoords[]{
    0u, 0x80000000u /* -0.0 */, Fb(0.1f), Fb(0.124f), Fb(0.125f), Fb(-0.125f), Fb(-0.5f), Fb(1.0f), Fb(4095.875f) /* 32767/8 */, Fb(4096.0f) /* 32768 */, Fb(-4096.0f), Fb(-4096.125f),
    Fb(8191.875f) /* 65535 */, Fb(8192.0f) /* 65536 */, Fb(1.0e6f), Fb(-1.0e6f), Fb(1.0e10f), Fb(1.1e18f), Fb(1.2e18f) /* x8 >= 2^63 */, Fb(-1.2e18f), Fb(3.0e37f), Fb(5.0e37f) /* x8 desborda a inf */,
    0x7F7FFFFFu /* FLT_MAX */, 0xFF7FFFFFu, 0x7F800000u /* +inf */, 0xFF800000u /* -inf */, 0x7FC00000u /* NaN */, 0xFFC00000u, 0x7FC12345u, 0x00000001u /* denormal */, 0x007FFFFFu, 0x80000001u,
};
inline constexpr u32 kSpecialModels[]{0u, 1u, 2u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0x10001u, 0xFFFF0000u, 0x80000000u, 0xFFFFFFFFu, 4000u, 19999u, 20000u, 374u};
inline constexpr u32 kSpecialAmmo[]{0u, 1u, 5u, 100u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFF00u, 500u};
inline constexpr u32 kSpecialNow[]{0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFF8000u, 0xFFFFE000u /* now+30000 desborda */, 0xFFFFFA24u /* now+1500 justo desborda */, 0xFFFFFA23u, 100000u, 3600000u};
inline constexpr u32 kTextRets[]{0u, 1u, 2u, 3u, 4u, 7u, 8u, 0xFu, 0x10u, 0xFFu, 0x100u, 0xFFFFFFFFu, 0xFFFFFFF9u};
inline constexpr u32 kRefValues[]{0u, 1u, 2u, 0x7FFEu, 0x7FFFu, 0x8000u, 0xFFFAu, 0xFFFBu, 0xFFFCu, 0xFFFDu, 0xFFFEu, 0xFFFFu};
inline constexpr u32 kEmptyBytes[]{0u, 1u, 2u, 3u, 0x7Fu, 0x80u, 0xFFu, 0x55u};
inline constexpr u32 kFillerTypes[]{1u, 2u, 3u, 6u, 7u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 0x40u, 0x7Fu, 0x80u, 0xFFu};   // nunca 0, 4, 5 ni 8
inline constexpr u32 kPlainTypes[]{1u, 2u, 3u, 6u, 7u, 9u, 10u, 12u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 0x40u, 0xFFu, 0u, 4u, 5u, 8u};   // sin 11 ni 13 (activan la busqueda por el final)
inline constexpr u32 kBoundaryIdx[]{0u, 1u, 2u, 8u, 9u, 10u, 11u, 19u, 20u, 21u, 309u, 310u, 311u, 599u, 600u, 608u, 609u, 610u, 611u, 617u, 618u, 619u};

struct SlotFx {
    u32 index{};
    std::array<u8, kSlotBytes> bytes{};
};
struct Fx {
    unsigned n{};
    u32 kind{};
    u32 x{}, y{}, z{};                 // bits de float
    u32 model{}, ptype{}, ammo{}, money{}, empty{};   // dwords tal como se empujan (ptype e empty: solo cuenta el byte bajo)
    bool msgNull{true};
    std::string msg;
    u32 textRet{}, visibleRet{}, giveObj{}, now{};   // lo que devuelven los falsos FindTextIndexForString / IsVisible / GiveUsAPickUpObject (etiqueta de objeto), y la hora
    u32 fillerType{};
    std::vector<SlotFx> slots;
    u32 eax{}, ecx{}, edx{}, stack{};
};

inline u32 HiGarbage(u32 c, u32 k) { return Rand(c, k) % 2 ? (Rand(c, k + 1) & 0xFFFFFF00u) : 0u; }
inline u32 CoordBits(u32 c, u32 k) {
    if (Rand(c, k) % 3 == 0) return kSpecialCoords[Rand(c, k + 1) % std::size(kSpecialCoords)];
    const float f = float(int(Rand(c, k + 2) % 8000001u) - 4000000) / 1000.0f;
    return Fb(f);
}
inline u32 PickIdx(u32 c, u32 k) { return Rand(c, k) % 2 ? kBoundaryIdx[Rand(c, k + 1) % std::size(kBoundaryIdx)] : Rand(c, k + 2) % kSlots; }
inline bool Used(const Fx& f, u32 i) { for (const auto& s : f.slots) if (s.index == i) return true; return false; }
inline u32 FreshIdx(const Fx& f, u32 c, u32 k) { u32 i = PickIdx(c, k); for (u32 t = 0; Used(f, i); ++t) i = (i + 1 + t) % kSlots; return i; }
// objMode: 0 = nulo, 1 = etiqueta de objeto falso, 2 = valor crudo (solo para ranuras que no se pueden sustituir)
inline void AddSlot(Fx& f, u32 c, u32 k, u32 idx, u32 type, u32 objMode) {
    SlotFx s;
    s.index = idx;
    FillWords(s.bytes.data(), kSlotBytes, Rand(c, k));
    s.bytes[0x1C] = u8(type);
    u32 obj = 0;
    if (objMode == 1) obj = kLabelBase + 1 + Rand(c, k + 1) % kNumObjs;
    else if (objMode == 2) obj = Rand(c, k + 2) % 2 ? 0xA5A5A5A5u : 0xDEADBEEFu;
    std::memcpy(s.bytes.data() + 4, &obj, 4);
    f.slots.push_back(s);
}
inline void Put16(Fx& f, std::size_t slotPos, u32 off, u32 v) { f.slots[slotPos].bytes[off] = u8(v); f.slots[slotPos].bytes[off + 1] = u8(v >> 8); }
// Rellena el escenario: nFree libres (tipo 0), nMoney (8), nT4 (4), nT5 (5), nOther (no sustituibles). nullObjs: los sustituibles llevan objeto nulo.
inline void Scenario(Fx& f, u32 c, u32 k0, u32 nFree, u32 nMoney, u32 nT4, u32 nT5, u32 nOther, bool nullObjs) {
    u32 k = k0;
    for (u32 i = 0; i < nFree; ++i, k += 4) { const u32 idx = FreshIdx(f, c, k); AddSlot(f, c, k + 1, idx, 0, Rand(c, k + 2) % 3); }
    for (u32 i = 0; i < nMoney; ++i, k += 4) { const u32 idx = FreshIdx(f, c, k); AddSlot(f, c, k + 1, idx, 8, nullObjs ? 0u : (Rand(c, k + 2) % 5 == 0 ? 0u : 1u)); }
    for (u32 i = 0; i < nT4; ++i, k += 4) { const u32 idx = FreshIdx(f, c, k); AddSlot(f, c, k + 1, idx, 4, nullObjs ? 0u : (Rand(c, k + 2) % 5 == 0 ? 0u : 1u)); }
    for (u32 i = 0; i < nT5; ++i, k += 4) { const u32 idx = FreshIdx(f, c, k); AddSlot(f, c, k + 1, idx, 5, nullObjs ? 0u : (Rand(c, k + 2) % 5 == 0 ? 0u : 1u)); }
    for (u32 i = 0; i < nOther; ++i, k += 4) { const u32 idx = FreshIdx(f, c, k); AddSlot(f, c, k + 1, idx, kFillerTypes[Rand(c, k + 2) % std::size(kFillerTypes)], Rand(c, k + 3) % 3); }
}

inline void PickMessage(Fx& f, u32 c) {
    static const char* const fixed[]{"PROP_3", "PROP_4", "prop_3", "PROP_3 ", "PROP_5", "", "FESZ_CA", "PROP_"};
    const u32 r = Rand(c, 70) % 8;
    f.msgNull = false;
    if (r == 0) { f.msgNull = true; f.msg.clear(); }
    else if (r <= 4) f.msg = fixed[Rand(c, 71) % std::size(fixed)];
    else {
        const u32 len = 1 + Rand(c, 72) % 14;
        f.msg.clear();
        for (u32 i = 0; i < len; ++i) f.msg += char(0x21 + Rand(c, 73 + i) % 0x5D);   // ASCII imprimible sin espacios
    }
}

// Escenario por familia (kind = n % 16; 384 casos de cada una). Todas las familias varian coordenadas, modelo, ammo, dinero, hora, mensaje, textos, visibilidad y objeto devuelto.
inline Fx Make(unsigned n) {
    Fx f;
    const u32 c = n;
    f.n = n;
    f.kind = n % kKinds;
    const u32 round = n / kKinds;
    f.x = CoordBits(c, 1); f.y = CoordBits(c, 5); f.z = CoordBits(c, 9);
    f.model = Rand(c, 13) % 4 == 0 ? kSpecialModels[Rand(c, 14) % std::size(kSpecialModels)] : Rand(c, 15) % 20000u;
    f.ammo = Rand(c, 16) % 4 == 0 ? kSpecialAmmo[Rand(c, 17) % std::size(kSpecialAmmo)] : Rand(c, 18) % 1000u;
    f.money = Rand(c, 19) % 4 == 0 ? kSpecialAmmo[Rand(c, 20) % std::size(kSpecialAmmo)] : Rand(c, 21) % 5000u;
    f.now = Rand(c, 22) % 3 == 0 ? kSpecialNow[Rand(c, 23) % std::size(kSpecialNow)] : Rand(c, 24) % 20000000u;
    f.textRet = Rand(c, 25) % 3 == 0 ? kTextRets[Rand(c, 26) % std::size(kTextRets)] : Rand(c, 27) % 4;
    f.visibleRet = (Rand(c, 28) % 2) | (Rand(c, 29) & 0xFFFFFF00u);
    f.giveObj = Rand(c, 30) % 3 == 0 ? 0u : kLabelBase + 1 + Rand(c, 31) % kNumObjs;
    f.fillerType = kFillerTypes[Rand(c, 32) % std::size(kFillerTypes)];
    f.eax = Rand(c, 33); f.ecx = Rand(c, 34); f.edx = Rand(c, 35);
    f.stack = Rand(c, 36) % 2 ? kStackSeeds[Rand(c, 37) % kStackSeeds.size()] : Rand(c, 38);
    PickMessage(f, c);
    u32 ptype = kPlainTypes[Rand(c, 40) % std::size(kPlainTypes)];
    u32 empty = 0;
    const u32 free1 = 1 + Rand(c, 41) % 5, tl = 1 + Rand(c, 42) % 4;
    switch (f.kind) {
    case 0: Scenario(f, c, 100, free1, Rand(c, 43) % 3, Rand(c, 44) % 2, Rand(c, 45) % 2, Rand(c, 46) % 4, false); break;                  // hueco libre, busqueda desde el principio
    case 1: ptype = 13; Scenario(f, c, 100, free1, Rand(c, 43) % 3, Rand(c, 44) % 2, Rand(c, 45) % 2, Rand(c, 46) % 4, false); break;       // FLOATINGPACKAGE: desde el final
    case 2: ptype = 11; Scenario(f, c, 100, free1, Rand(c, 43) % 3, Rand(c, 44) % 2, Rand(c, 45) % 2, Rand(c, 46) % 4, false); break;       // NAUTICAL_MINE_INACTIVE: desde el final
    case 3: empty = kEmptyBytes[1 + Rand(c, 47) % (std::size(kEmptyBytes) - 1)]; Scenario(f, c, 100, free1, Rand(c, 43) % 3, Rand(c, 44) % 2, Rand(c, 45) % 2, Rand(c, 46) % 4, false); break;   // isEmpty != 0: desde el final
    case 4: empty = 0; Scenario(f, c, 100, free1, 0, 0, 0, Rand(c, 46) % 4, false); break;                                                    // isEmpty = 0 con basura en los bytes altos
    case 5: Scenario(f, c, 100, 0, tl, Rand(c, 44) % 3, Rand(c, 45) % 3, Rand(c, 46) % 3, false); break;                                     // sin libres: sustituye el primer dinero (con objeto)
    case 6: Scenario(f, c, 100, 0, tl, Rand(c, 44) % 3, Rand(c, 45) % 3, Rand(c, 46) % 3, true); break;                                       // sin libres: dinero con objeto nulo
    case 7: Scenario(f, c, 100, 0, 0, tl, Rand(c, 45) % 3, Rand(c, 46) % 3, false); break;                                                    // sin libres ni dinero: tiempo limite
    case 8: Scenario(f, c, 100, 0, 0, Rand(c, 44) % 3, 1 + Rand(c, 45) % 3, Rand(c, 46) % 3, Rand(c, 47) % 4 == 0); break;
    case 9: Scenario(f, c, 100, 0, 1 + Rand(c, 43) % 2, 1 + Rand(c, 44) % 2, 1 + Rand(c, 45) % 2, Rand(c, 46) % 3, false); break;           // dinero y tiempo limite: gana el dinero
    case 10: Scenario(f, c, 100, 0, 0, 0, 0, Rand(c, 46) % 7, false); break;                                                                 // nada sustituible: -1
    case 11: {                                                                                                                                // busqueda por el final fallida y paso a dinero / tiempo limite
        const u32 w = Rand(c, 48) % 3;
        if (w == 0) ptype = 13; else if (w == 1) ptype = 11; else empty = kEmptyBytes[1 + Rand(c, 47) % (std::size(kEmptyBytes) - 1)];
        Scenario(f, c, 100, 0, Rand(c, 43) % 3, Rand(c, 44) % 3, Rand(c, 45) % 3, Rand(c, 46) % 4, Rand(c, 49) % 4 == 0);
        break;
    }
    case 12: ptype = round & 0xFF; if (Rand(c, 50) % 8 == 0) ptype = 0xA0 + (Rand(c, 51) % 0x60); Scenario(f, c, 100, 1 + Rand(c, 41) % 2, 0, 0, 0, Rand(c, 46) % 3, false); break;   // barrido de todos los tipos
    case 13: empty = kEmptyBytes[round % std::size(kEmptyBytes)]; Scenario(f, c, 100, 1 + Rand(c, 41) % 3, 0, 0, 0, Rand(c, 46) % 3, false);
             f.visibleRet = (round / 8) % 2 | (Rand(c, 29) & 0xFFFFFF00u); f.giveObj = ((round / 16) % 5 == 0) ? 0u : kLabelBase + 1 + (round / 16) % kNumObjs; f.textRet = kTextRets[(round / 4) % std::size(kTextRets)]; break;   // banderas / texto / visibilidad / objeto
    case 14: Scenario(f, c, 100, 1 + Rand(c, 41) % 2, 0, 0, 0, Rand(c, 46) % 3, false);
             for (std::size_t i = 0; i < f.slots.size(); ++i) if (f.slots[i].bytes[0x1C] == 0) { Put16(f, i, 0x1A, kRefValues[(round + i) % std::size(kRefValues)]); break; } break;   // indice de referencia
    case 15: Scenario(f, c, 100, 1 + Rand(c, 41) % 2, 0, 0, 0, Rand(c, 46) % 3, false);
             f.x = kSpecialCoords[Rand(c, 52) % std::size(kSpecialCoords)]; f.y = kSpecialCoords[Rand(c, 53) % std::size(kSpecialCoords)]; f.z = kSpecialCoords[Rand(c, 54) % std::size(kSpecialCoords)];
             f.model = kSpecialModels[Rand(c, 55) % std::size(kSpecialModels)]; f.ammo = kSpecialAmmo[Rand(c, 56) % std::size(kSpecialAmmo)]; f.money = kSpecialAmmo[Rand(c, 57) % std::size(kSpecialAmmo)]; break;
    }
    f.ptype = ptype | HiGarbage(c, 60);
    f.empty = empty | HiGarbage(c, 62);
    return f;
}

inline std::string HexOf(const u8* p, std::size_t n) {
    constexpr char d[] = "0123456789abcdef";
    std::string o;
    o.reserve(n * 2);
    for (std::size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; }
    return o;
}
// Fila `input` en JSON (texto), identica para el banco (Core.hpp la parsea) y para fixture_dump.cpp (calibracion sin juego). El mensaje es ASCII imprimible.
inline std::string InputLine(const Fx& f) {
    std::string m = "null";
    if (!f.msgNull) {
        m = "\"";
        for (const char ch : f.msg) { if (ch == '"' || ch == '\\') m += '\\'; m += ch; }
        m += "\"";
    }
    std::string o = "{\"kind\":" + std::to_string(f.kind) + ",\"x\":" + std::to_string(f.x) + ",\"y\":" + std::to_string(f.y) + ",\"z\":" + std::to_string(f.z)
        + ",\"model\":" + std::to_string(f.model) + ",\"ptype\":" + std::to_string(f.ptype) + ",\"ammo\":" + std::to_string(f.ammo) + ",\"money\":" + std::to_string(f.money)
        + ",\"empty\":" + std::to_string(f.empty) + ",\"msg\":" + m + ",\"text_ret\":" + std::to_string(f.textRet) + ",\"visible_ret\":" + std::to_string(f.visibleRet)
        + ",\"give_obj\":" + std::to_string(f.giveObj) + ",\"now\":" + std::to_string(f.now) + ",\"filler_type\":" + std::to_string(f.fillerType) + ",\"slots\":[";
    for (std::size_t i = 0; i < f.slots.size(); ++i) {
        if (i) o += ',';
        o += "[" + std::to_string(f.slots[i].index) + ",\"" + HexOf(f.slots[i].bytes.data(), kSlotBytes) + "\"]";
    }
    o += "],\"eax\":" + std::to_string(f.eax) + ",\"ecx\":" + std::to_string(f.ecx) + ",\"edx\":" + std::to_string(f.edx) + ",\"stack_seed\":" + std::to_string(f.stack) + "}";
    return o;
}
}
