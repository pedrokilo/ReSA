#pragma once
// Fixtures del banco ReplUpgrade72 (lote 72): CVehicle::AddReplacementUpgrade (0x6D3830), thiscall(int32 modelIndex, int32 nodeId) ret 8, sin valor de retorno.
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
// El caso es un vector `v` de kVWords palabras (ver abajo) y los valores iniciales de EAX/EDX/pila (ECX = this). El vehiculo, su model info, los model infos de mejoras
// (instalados en ranuras NULAS de ms_modelInfoPtrs elegidas en `begin`), la manejabilidad, los marcos y los atomos FALSOS viven en una arena propia con guardas; los colaboradores se sustituyen por falsos
// que registran sus argumentos en orden.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace R72Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFns = 1;
constexpr u32 kMaxEvents = 32;
constexpr u32 kVWords = 24;
constexpr u32 kEvArgs = 8;
constexpr u32 kFamilies = 16;

// Disposicion de la arena (bytes desde su base): guarda 0x40 + vehiculo 0x5C0 + model info del vehiculo 0x80 + 4 x 0x40 model infos de mejoras + 0xE0 manejabilidad
// + 3 marcos de 0x60 (F0 = marco del componente, F1 = marco del atomo de rueda nuevo, FA = marco original del atomo creado) + 3 atomos de 0x40 (AT0 = CreateInstance,
// AT1 = rueda, AT2 = danado) + 0x20 clump (RwObject de tipo rpCLUMP) + 0x40 guarda.
constexpr u32 oG0 = 0x000, oVeh = 0x040, oVMi = 0x600, oU0 = 0x680, oHnd = 0x780, oF0 = 0x860, oF1 = 0x8C0, oFA = 0x920, oAT0 = 0x980, oAT1 = 0x9C0, oAT2 = 0xA00, oClump = 0xA40, oG1 = 0xA60;
constexpr u32 kArenaSize = 0xAA0;
constexpr u32 kArenaWords = kArenaSize / 4;
static_assert(oVeh == oG0 + 0x40 && oVMi == oVeh + 0x5C0 && oU0 == oVMi + 0x80 && oHnd == oU0 + 4 * 0x40 && oF0 == oHnd + 0xE0 && oF1 == oF0 + 0x60 && oFA == oF1 + 0x60 &&
              oAT0 == oFA + 0x60 && oAT1 == oAT0 + 0x40 && oAT2 == oAT1 + 0x40 && oClump == oAT2 + 0x40 && oG1 == oClump + 0x20 && kArenaSize == oG1 + 0x40, "arena");
// Campos (desplazamientos del EXE, verificados con el desensamblado de 0x6D3830)
constexpr u32 kVClump = 0x18, kVModel = 0x22, kVHandling = 0x384;
constexpr u32 kMiPlateMat = 0x24, kMiPlateType = 0x31, kHndFlags = 0xCC;
constexpr u32 kAtParent = 0x04, kAtGeometry = 0x18, kFrPosX = 0x40, kFrMatrix = 0x10;
constexpr u32 kVehModelSlot = 15, kFirstUpgradeSlot = 16;
constexpr u32 kModelInfoPtrs = 0xA9B0C8, kDamagedFlag = 0xA9B0B0;
constexpr u32 kNumModelInfos = 20000;   // CModelInfo::NUM_MODEL_INFOS: la tabla ms_modelInfoPtrs (0xA9B0C8, 4 bytes por entrada) acaba en 0xACE948; el EXE no valida el indice (0x6D3875/0x6D3971)
// Ranuras de la tabla (modelo 26/10/2026b): el banco NUNCA toca una ranura con model info real. En `begin` se recorre la tabla (20000 entradas) y se eligen kSel ranuras NULAS
// (SelectNullSlots); el caso usa ORDINALES 0..kSel-1 y Realize los sustituye por el indice real. Ordinales: 0..7 las nulas mas bajas, 8..15 las mas altas, 16..47 repartidas
// (32), 48..63 un bloque de 16 consecutivas (zona 3). Todas dentro de la tabla; ninguna lectura/escritura fuera de ella.
constexpr u32 kSel = 64;
constexpr u32 ZoneOf(u32 ord) { return ord < 8 ? 0u : ord < 16 ? 1u : ord < 48 ? 2u : 3u; }
inline bool SelectNullSlots(const std::vector<u8>& isNull, std::array<u32, kSel>& sel) {
    std::vector<u32> nl;
    for (u32 i = 0; i < kNumModelInfos; ++i) if (isNull[i]) nl.push_back(i);
    const size_t n = nl.size();
    if (n < 4 * kSel) return false;
    std::vector<u8> used(kNumModelInfos, 0);
    auto take = [&](size_t pos) {
        for (size_t k = 0; k < n; ++k) { const u32 v = nl[(pos + k) % n]; if (!used[v]) { used[v] = 1; return v; } }
        return 0xFFFFFFFFu;
    };
    // bloque de 16 consecutivas en el medio de la racha nula mas larga (si la racha es menor, se completa con las siguientes nulas)
    size_t bestStart = 0, bestLen = 0;
    for (size_t a = 0; a < n;) { size_t b = a; while (b + 1 < n && nl[b + 1] == nl[b] + 1) ++b; if (b - a + 1 > bestLen) { bestLen = b - a + 1; bestStart = a; } a = b + 1; }
    const size_t runFirst = bestStart + (bestLen > 16 ? (bestLen - 16) / 2 : 0);
    for (u32 i = 0; i < 16; ++i) sel[48 + i] = take(runFirst + i);
    for (u32 i = 0; i < 8; ++i) sel[i] = take(i);
    for (u32 i = 0; i < 8; ++i) sel[8 + i] = take(n - 1 - i);
    for (u32 i = 0; i < 32; ++i) sel[16 + i] = take((2 * i + 1) * n / 64);
    for (u32 i = 0; i < kSel; ++i) if (sel[i] >= kNumModelInfos) return false;
    return true;
}
constexpr u64 kLabel = 0x100000000ull;   // marca de "puntero a la arena": kLabel + desplazamiento

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

struct Spec {
    u32 fn{};
    u32 family{};
    std::array<u32, kVWords> v{};
    u32 stack{}, eax{}, edx{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

inline u32 SaneFloat(u32 bits) {   // sin denormales (el juego corre con MXCSR FTZ; el x87 del EXE conservaria el denormal)
    const u32 exp = (bits >> 23) & 0xFF;
    if (exp == 0) return bits & 0x80000000u;
    if (exp < 0x30) return (bits & 0x807FFFFFu) | (0x30u << 23);
    return bits;
}
constexpr u32 kQNaN = 0x7FC00000u;
// Clases: 0 cero (+-), 1 entero pequeno, 2 normal moderado, 3 patron aleatorio sano (puede ser enorme: x * -2 desborda), 4 infinito, 5 NaN silencioso
inline u32 FloatOf(Gen& g, u32 cls) {
    const u32 sign = g.C(50) ? 0x80000000u : 0u;
    switch (cls) {
    case 0: return sign;
    case 1: { const float f = float(int(g.P(2001)) - 1000); u32 b; std::memcpy(&b, &f, 4); return b; }
    case 2: { const float f = (float(int(g.P(2000001)) - 1000000)) / 64.0f; u32 b; std::memcpy(&b, &f, 4); return b; }
    case 3: return SaneFloat(g.R());
    case 4: return sign | 0x7F800000u;
    default: return sign | kQNaN | (g.R() & 0x1FFFFFu);
    }
}
// Valores especiales de x del componente (lote 72b; el juego corre con MXCSR FTZ y el EXE usa la x87): desnormales (mitad baja: el doble sigue siendo desnormal y mulss+FTZ lo vaciaria;
// mitad alta: el doble es normal), limites 1 / 0x3FFFFF / 0x400000 / 0x7FFFFF, NaN senalizadores (bit 22 limpio, carga no nula) y silenciosos con cargas distintas, ceros con signo,
// infinitos, normales minimos y maximos (el doble desborda).
inline u32 SpecialX(Gen& g) {
    const u32 sign = g.C(50) ? 0x80000000u : 0u;
    switch (g.P(14)) {
    case 0: return sign | (1u + g.P(0x7FFFFF));
    case 1: return sign | (1u + g.P(0x3FFFFF));
    case 2: return sign | (0x400000u + g.P(0x400000));
    case 3: { static constexpr u32 e[]{1u, 2u, 0x3FFFFFu, 0x400000u, 0x400001u, 0x7FFFFFu}; return sign | e[g.P(6)]; }
    case 4: return sign | 0x7F800000u | (1u + g.P(0x3FFFFF));
    case 5: { static constexpr u32 e[]{1u, 2u, 0x200000u, 0x3FFFFFu}; return sign | 0x7F800000u | e[g.P(4)]; }
    case 6: return sign | kQNaN | (g.R() & 0x3FFFFFu);
    case 7: { static constexpr u32 e[]{0u, 1u, 0x200000u, 0x3FFFFFu}; return sign | kQNaN | e[g.P(4)]; }
    case 8: return sign;
    case 9: return sign | 0x7F800000u;
    case 10: return sign | (0x00800000u + g.P(32));
    case 11: return sign | (0x7F7FFFFFu - g.P(32));
    case 12: return sign | (0x3F000000u + g.P(0x01000000));
    default: return sign | (0x00FFFFFFu & g.R()) | 0x00800000u;
    }
}
inline bool IsFiniteNonZero(u32 b) { const u32 e = (b >> 23) & 0xFF; return e != 0xFF && (b & 0x7FFFFFFFu) != 0; }
inline u32 Ptrish(Gen& g) {   // clump / marco / material / geometria: nulo, pequeno o patron alto (nunca dentro de la arena)
    switch (g.P(4)) {
    case 0: return 0u;
    case 1: return g.P(0x1000);
    default: return 0xA0000000u | (g.R() & 0x0FFFFFFFu);
    }
}
inline u32 NonNullPtrish(Gen& g) { const u32 p = Ptrish(g); return p ? p : 0xA0000040u | (g.R() & 0x0FFFFF00u); }

// v[0] = modelIndex (16..19: model info de mejora U0..U3); v[1] = nodeId; v[2] = retorno de GetFrameFromId: 0 nulo, 1 el marco F0 de la arena, 2 el valor v[3] (puntero ajeno);
// v[3] = puntero ajeno; v[4] = material de la matricula (+0x24 del model info del vehiculo); v[5] = tipo de matricula (+0x31, byte); v[6] = banderas de la manejabilidad (+0xCC);
// v[7] = clump (+0x18): 0 nulo, 1 el clump falso; v[8] = 1: el falso de RwFrameForAllObjects(RemoveObjectsCB) escribe v[9] en `data`; v[10] = 1: el de ForAllChildren escribe v[11];
// v[12] = marco original del atomo creado (AT0 +4); v[13] = geometria del atomo (AT0 +0x18); v[14] = byte que escribe el falso de RpGeometryForAllMaterials en `data` (hay material con alfa);
// v[15] = retorno de AsDamageAtomicModelInfoPtr (EAX completo); v[16] = x del marco del componente (F0 +0x40, bits de float); v[17] = ms_bCreateDamagedVersion inicial (byte);
// v[18] = indice (int16 de [this+0x22]) del model info del vehiculo en ms_modelInfoPtrs; v[0] = modelIndex. En MakeSpec son ORDINALES 0..kSel-1; Realize los sustituye por el indice real de la ranura
// nula elegida y rellena v[19] = selector del model info falso de la mejora (U0..U3 de la arena = ordinal & 3), v[20] / v[21] = zona (ZoneOf) del ordinal de modelIndex / del vehiculo; v[0] != v[18].
constexpr u32 kModel = 0, kNode = 1, kFrameKind = 2, kFrameOther = 3, kPlateMat = 4, kPlateType = 5, kHnd = 6, kClumpOn = 7, kObjWrites = 8, kObjValue = 9, kChWrites = 10, kChValue = 11,
              kOldFrame = 12, kGeom = 13, kAlphaByte = 14, kDamageRet = 15, kPosX = 16, kDmgInit = 17, kVehIdx = 18, kUSel = 19, kZoneM = 20, kZoneV = 21;

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + 0x5000u);
    s.family = n % kFamilies;
    s.stack = StackSeed(n);
    s.eax = g.R(); s.edx = g.R();
    const u32 fam = s.family, sel = fam & 7;
    s.v[kModel] = kFirstUpgradeSlot + g.P(4);   // (el pool ampliado de indices se elige al final, con tiradas nuevas, para no alterar las demas)
    u32 node;
    switch (sel) {
    case 0: case 1: node = 0x13; break;
    case 2: node = 0xC; break;
    case 3: node = 0xD; break;
    case 4: node = g.P(0x30); break;
    case 5: node = g.C(50) ? 0x12 : 0x14; break;
    case 6: node = g.C(50) ? (g.C(50) ? 0xB : 0xE) : g.R(); break;
    default: node = g.C(60) ? 0x13 : g.P(0x30); break;
    }
    s.v[kNode] = node;
    u32 hnd = g.R();
    if (sel == 0) hnd |= 0x2000u;
    else if (sel == 1) hnd &= ~0x2000u;
    else if (sel == 7) hnd = g.C(50) ? (hnd | 0x2000u) : (hnd & ~0x2000u);
    s.v[kHnd] = hnd;
    const bool wheel = node == 0x13 && (hnd & 0x2000u);
    const u32 fk = g.P(10);
    s.v[kFrameKind] = wheel ? 1u : (fk < 2 ? 0u : (fk < 8 ? 1u : 2u));
    s.v[kFrameOther] = NonNullPtrish(g);
    s.v[kPlateMat] = Ptrish(g);
    s.v[kPlateType] = g.C(50) ? g.P(256) : g.R();   // el EXE lee un byte (+0x31): el resto de la palabra es relleno
    s.v[kClumpOn] = g.C(15) ? 0u : 1u;
    const u32 ow = g.P(4);
    s.v[kObjWrites] = ow < 2 ? 1u : 0u;
    s.v[kObjValue] = g.C(30) ? (g.P(8)) : (g.C(20) ? 0xFFFFFFFFu : g.R());
    const u32 cw = g.P(4);
    s.v[kChWrites] = cw < 2 ? 1u : 0u;
    s.v[kChValue] = g.C(30) ? (g.P(8)) : (g.C(20) ? 0xFFFFFFFFu : g.R());
    s.v[kOldFrame] = NonNullPtrish(g);   // el atomo creado siempre trae marco (RwFrameDestroy de la DLL exige no nulo)
    s.v[kGeom] = NonNullPtrish(g);
    const u32 al = g.P(10);
    s.v[kAlphaByte] = (g.R() & 0xFFFFFF00u) | (al < 5 ? 0u : (al < 8 ? 1u : 1u + g.P(255)));
    const u32 dm = g.P(10);
    s.v[kDamageRet] = dm < 5 ? 0u : (dm < 8 ? (0xA0000000u | (g.R() & 0x0FFFFFFFu)) : (dm < 9 ? 1u : (g.R() | 1u)));
    if (g.C(5)) s.v[kDamageRet] = g.C(50) ? 0x100u : 0x80000000u;   // solo bits altos: el EXE prueba EAX completo
    const u32 cls = fam >= 8 ? (g.C(60) ? 4u + g.P(2) : g.P(4)) : (g.C(10) ? 0u : g.C(30) ? 1u : g.C(60) ? 2u : 3u);
    s.v[kPosX] = FloatOf(g, cls);
    s.v[kDmgInit] = g.C(40) ? 0u : (g.C(50) ? 1u : g.P(256));
    // ---- ampliacion 26/10/2026 (tiradas nuevas, despues de todas las anteriores)
    // (a) ordinales de ranuras nulas (ver SelectNullSlots): zona 0 (bajas) 20 %, 1 (altas) 20 %, 2 (repartidas) 35 %, 3 (bloque consecutivo) 25 %
    auto pick = [&]() { const u32 z = g.P(100); return z < 20 ? g.P(8) : z < 40 ? 8u + g.P(8) : z < 75 ? 16u + g.P(32) : 48u + g.P(16); };
    const u32 mo = pick();
    u32 vo = pick();
    if (vo == mo) vo = (mo + 1u + g.P(kSel - 1)) % kSel;
    s.v[kModel] = mo;
    s.v[kVehIdx] = vo;
    // (b) x del componente: valores especiales (desnormales, SNaN, ...) en parte de los casos de la rueda con x finita no nula; los demas conservan sus clases y sus minimos
    const bool wheelCase = node == 0x13 && (hnd & 0x2000u);
    if (wheelCase && IsFiniteNonZero(s.v[kPosX]) && g.C(85)) s.v[kPosX] = SpecialX(g);
    return s;
}

// Sustituye los ordinales por los indices reales de las ranuras nulas elegidas y rellena v[19..21].
inline void Realize(Spec& s, const std::array<u32, kSel>& sel) {
    const u32 mo = s.v[kModel], vo = s.v[kVehIdx];
    s.v[kUSel] = mo & 3u;
    s.v[kZoneM] = ZoneOf(mo);
    s.v[kZoneV] = ZoneOf(vo);
    s.v[kModel] = sel[mo];
    s.v[kVehIdx] = sel[vo];
}

// Relleno inicial de la arena: palabras con los bits 31 y 30 puestos (nunca coinciden con una direccion de la arena); los campos de la funcion se escriben encima.
inline u32 FillWord(u32 fn, u32 n, u32 i) { return 0xC0000000u | (Rand(n + fn * 0x20000u + 0x777u, 1000u + i) >> 2); }

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"v\":[";
    for (u32 i = 0; i < kVWords; ++i) o += (i ? "," : "") + U(s.v[i]);
    o += "],\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.edx) + "]}";
    return o;
}
}  // namespace R72Fix
