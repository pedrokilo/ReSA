#pragma once
// Fixtures del banco Vehicle66 (lote 66): CVehicle::RemoveVehicleUpgrade (0x6DF930), AddUpgrade (0x6DFA20), AddVehicleUpgrade (0x6E3290),
// PossiblyDropFreeFallBombForPlayer (0x6E07E0) y ProcessSirenAndHorn (0x6E0950). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y un
// programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   RemoveVehicleUpgrade              0x6DF930  thiscall(int32 upgradeModelIndex) ret 4
//   AddUpgrade                        0x6DFA20  thiscall(int32 modelIndex, int32 upgradeIndex) ret 8
//   AddVehicleUpgrade                 0x6E3290  thiscall(int32 modelId) -> EAX = mejora sustituida (lo lee el llamador 0x4985DA) ret 4
//   PossiblyDropFreeFallBombForPlayer 0x6E07E0  thiscall(eOrdnanceType type, bool arg1) ret 8
//   ProcessSirenAndHorn               0x6E0950  thiscall(bool arg0) ret 4
//
// El caso es un vector `v` de 40 palabras cuyo significado depende de la funcion (ver MakeXxx) y los valores iniciales de EAX/EDX/pila (ECX = this).
// El vehiculo, su model info, su CVehicleStructure, los model infos de mejoras (instalados en ms_modelInfoPtrs[16..19]), la manejabilidad, el conductor y la matriz FALSOS viven en
// una arena propia con guardas; los colaboradores se sustituyen por falsos que registran sus argumentos en orden.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace V66Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 5;
constexpr u32 kRemove = 0, kAddUp = 1, kAddVeh = 2, kDrop = 3, kSiren = 4;
constexpr u32 kMaxEvents = 24;
constexpr u32 kVWords = 40;
constexpr u32 kEvArgs = 8;

// Disposicion de la arena (bytes desde su base). Suma a mano: 0x40 guarda + 0x5C0 vehiculo + 0x80 model info del vehiculo + 0x2C0 estructura del vehiculo + 4 x 0x40 model infos de mejoras
// + 0xE0 manejabilidad + 0x5A0 conductor + 0x60 matriz + 0x20 clump (RwObject de tipo rpCLUMP: GetRpClump() de la DLL comprueba el tipo) + 0x40 guarda = 0x1180.
constexpr u32 oG0 = 0x000, oVeh = 0x040, oVMi = 0x600, oVSt = 0x680, oU0 = 0x940, oHnd = 0xA40, oPed = 0xB20, oMat = 0x10C0, oClump = 0x1120, oG1 = 0x1140;
constexpr u32 kArenaSize = 0x1180;
constexpr u32 kArenaWords = kArenaSize / 4;
static_assert(oVeh == oG0 + 0x40 && oVMi == oVeh + 0x5C0 && oVSt == oVMi + 0x80 && oU0 == oVSt + 0x2C0 && oHnd == oU0 + 4 * 0x40 && oPed == oHnd + 0xE0 && oMat == oPed + 0x5A0 &&
              oClump == oMat + 0x60 && oG1 == oClump + 0x20 && kArenaSize == oG1 + 0x40, "arena");
// Campos (desplazamientos del EXE, verificados con el desensamblado)
constexpr u32 kVPos = 0x04, kVMatrix = 0x14, kVClump = 0x18, kVModel = 0x22, kVStatus = 0x36, kVHandling = 0x384, kVFlags38C = 0x38C, kVFlags42D = 0x42D, kVUpgrades = 0x43A,
              kVWheelScale = 0x458, kVAlarm = 0x45C, kVDriver = 0x460, kVOrdn = 0x4A9, kVTime1 = 0x4FC, kVTime2 = 0x500, kVHorn = 0x514, kVSubType = 0x594;
constexpr u32 kMiPlateMat = 0x24, kMiPlateType = 0x31, kMiWheelFront = 0x40, kMiStruct = 0x5C, kMiFlags = 0x12;
constexpr u32 kStUpgrades = 0xB4, kDescParent = 0x1C, kHndFlags = 0xD0, kPedType = 0x598, kMatFwd = 0x10, kMatPos = 0x30;
constexpr u32 kVehModelSlot = 15, kFirstUpgradeSlot = 16;
constexpr u32 kPadHist = 0xB73569, kPadHistSize = 0x100, kPads0 = 0xB73458, kTimeMs = 0xB7CB84, kOffroadWheel = 0x8CD758, kModelInfoPtrs = 0xA9B0C8;
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

constexpr u32 kFamilies = 8;

// Bits de float "sanos": sin denormales ni exponentes diminutos (el juego corre con MXCSR FTZ; el x87 del EXE conservaria el denormal): exponente 0 solo para el cero.
inline u32 SaneFloat(u32 bits) {
    const u32 exp = (bits >> 23) & 0xFF;
    if (exp == 0) return bits & 0x80000000u;                 // +-0
    if (exp < 0x30) return (bits & 0x807FFFFFu) | (0x30u << 23);
    return bits;
}
constexpr u32 kQNaN = 0x7FC00000u;
// Clases: 0 cero (+-), 1 entero pequeno, 2 normal moderado, 3 patron aleatorio sano (puede ser enorme), 4 infinito, 5 NaN silencioso
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
inline u32 AnyFloat(Gen& g) {
    const u32 r = g.P(100);
    return FloatOf(g, r < 8 ? 0u : r < 28 ? 1u : r < 70 ? 2u : r < 88 ? 3u : r < 94 ? 4u : 5u);
}
inline u32 Ptrish(Gen& g) {   // clump / marco / material: nulo, pequeno o patron alto (nunca dentro de la arena)
    switch (g.P(4)) {
    case 0: return 0u;
    case 1: return g.P(0x1000);
    default: return 0xA0000000u | (g.R() & 0x0FFFFFFFu);
    }
}
inline u32 NonNullPtrish(Gen& g) { const u32 p = Ptrish(g); return p ? p : 0xA0000040u | (g.R() & 0x0FFFFF00u); }
// Valor de retorno de un bool de un colaborador: solo cuenta AL (el EXE hace `test al,al`); los demas bits son basura
inline u32 BoolRet(Gen& g, bool value) { return (g.R() & 0xFFFFFF00u) | (value ? (1u + g.P(255)) : 0u); }

// ---- RemoveVehicleUpgrade (0x6DF930)
// v[0] = upgradeModelIndex (16..19: model info de mejora U0..U3); v[1..4] = palabra +0x12 de los model infos U0..U3 (bit 8 VehDummy, bits 10..14 CarMod); v[5] = retorno de
// ClearVehicleUpgradeFlags (AL); v[6] = retorno de FindOtherUpgrade (EAX; el EXE usa AX con signo); v[7] = m_fWheelScale inicial; v[8..22] = las 15 ranuras m_anUpgrades (int16);
// v[23] = clump (+0x18): 0 nulo, 1 el clump falso.
constexpr u32 kRmV0 = 0, kRmFlags = 1, kRmClear = 5, kRmLinked = 6, kRmScale = 7, kRmSlots = 8, kRmClump = 23;
// ---- AddUpgrade (0x6DFA20)
// v[0] = modelIndex (16..19); v[1] = upgradeIndex (0..14); v[2] = material de la matricula (+0x24 del model info del vehiculo); v[3] = tipo de matricula (+0x31, byte); v[4..18] = padres de las 15
// descripciones m_aUpgrades (+0x1C de cada una); v[19] = clump (+0x18): 0 nulo, 1 el clump falso; v[20] = retorno de GetFrameFromId; v[21], v[22] = retornos de CreateUpgradeAtomic.
constexpr u32 kAuModel = 0, kAuIndex = 1, kAuPlateMat = 2, kAuPlateType = 3, kAuParents = 4, kAuClump = 19, kAuFrame = 20, kAuRet1 = 21, kAuRet2 = 22;
// ---- AddVehicleUpgrade (0x6E3290)
// v[0] = modelId (16..19); v[1..4] = palabras +0x12 de U0..U3; v[5] = retorno de SetVehicleUpgradeFlags (AL); v[6] = 1: ese falso escribe v[7] en su salida; v[7] = valor escrito;
// v[8] = retorno de FindOtherUpgrade; v[9] = retorno de GetReplacementUpgrade; v[10] = retorno de GetUpgrade; v[11] = m_fWheelSizeFront (+0x40 del model info del vehiculo);
// v[12] = m_nVehicleSubType (+0x594); v[13] = MI_OFFROAD_WHEEL (palabra global 0x8CD758); v[14] = banderas de manejabilidad (+0xD0); v[15] = banderas del vehiculo (+0x38C);
// v[16] = m_fWheelScale inicial; v[17..31] = las 15 ranuras m_anUpgrades (int16).
constexpr u32 kAvModel = 0, kAvFlags = 1, kAvSetRet = 5, kAvSetWrites = 6, kAvSetValue = 7, kAvLinked = 8, kAvGetRepl = 9, kAvGetUp = 10, kAvWheelFront = 11, kAvSubType = 12,
              kAvOffroad = 13, kAvHnd = 14, kAvFlags38C = 15, kAvScale = 16, kAvSlots = 17;
// ---- PossiblyDropFreeFallBombForPlayer (0x6E07E0)
// v[0] = type; v[1] = arg1 (dword; solo cuenta el byte bajo); v[2] = CTimer::m_snTimeInMilliseconds; v[3] = +0x4FC; v[4] = +0x500; v[5] = retorno de GetPlaneOrdnanceRateOfFire;
// v[6..8] = posicion de la ordenanza (bits de float); v[9] = 1 si hay matriz; v[10..12] = posicion de la matriz (+0x30); v[13..15] = posicion del placeable (+4); v[16] = byte +0x4A9;
// v[17] = conductor (0 ninguno, 1 presente); v[18] = tipo de ped (+0x598); v[19] = retorno de AddProjectile.
constexpr u32 kDrType = 0, kDrArg1 = 1, kDrTime = 2, kDrT1 = 3, kDrT2 = 4, kDrRate = 5, kDrOrd = 6, kDrHasMat = 9, kDrMatPos = 10, kDrPlPos = 13, kDrByte = 16, kDrDriver = 17,
              kDrPedType = 18, kDrProj = 19;
// ---- ProcessSirenAndHorn (0x6E0950)
// v[0] = arg0 (dword; solo cuenta el byte bajo); v[1] = retorno de UsesSiren (AL); v[2] = iCurrHornHistory (0..255); v[3..7] = bHornHistory[0..4]; v[8] = m_nAlarmState; v[9] = byte +0x36;
// v[10] = retorno de GetHorn (AL); v[11] = byte +0x42D; v[12] = m_HornCounter inicial; el resto del historial (bytes 6..255 tras bHornHistory[0]) sale de HornFill.
constexpr u32 kSiArg0 = 0, kSiSiren = 1, kSiIndex = 2, kSiHist = 3, kSiAlarm = 8, kSiStatus = 9, kSiHorn = 10, kSiFlags = 11, kSiCounter = 12;
inline u32 HornFill(u32 n, u32 i) {   // byte i (6..255) de la region [0xB73569, +0x100): la mitad son ceros
    const u32 r = Rand(n + 0x44000u, 500u + i);
    return (r & 1) ? 0u : (1u + ((r >> 8) % 255u));
}

inline u32 SlotWord(Gen& g) {   // contenido de una ranura m_anUpgrades (int16 con signo): -1, 16..19, 0, o un valor cualquiera
    switch (g.P(8)) {
    case 0: case 1: case 2: return 0xFFFFu;
    case 3: case 4: case 5: return 16u + g.P(4);
    case 6: return g.P(2) ? 0u : 0x8000u;
    default: return g.R() & 0xFFFFu;
    }
}
inline u32 InfoFlags(Gen& g, u32 carMod, bool veh) { return (g.R() & 0x82FFu) | (carMod << 10) | (veh ? 0x100u : 0u); }   // bits 0..7, 9 y 15 aleatorios

inline void MakeRemove(Spec& s, Gen& g) {
    const u32 fam = s.family;
    const u32 u = g.P(4);
    s.v[kRmV0] = kFirstUpgradeSlot + u;
    bool veh[4];
    u32 car[4];
    for (u32 i = 0; i < 4; ++i) {
        car[i] = g.C(30) ? 2u : g.P(32);
        veh[i] = g.C(50);
    }
    switch (fam) {   // la mejora principal
    case 0: veh[u] = false; break;
    case 1: veh[u] = true; car[u] = 2; break;
    case 2: veh[u] = true; car[u] = g.P(32); break;
    case 3: veh[u] = false; car[u] = 2; break;
    default: break;
    }
    for (u32 i = 0; i < 4; ++i) s.v[kRmFlags + i] = InfoFlags(g, car[i], veh[i]);
    s.v[kRmClear] = BoolRet(g, fam == 7 ? g.C(60) : g.C(15));
    // FindOtherUpgrade devuelve un int16 en AX (EAX con basura arriba): -1 o la mejora enlazada (16..19)
    const u32 lk = g.P(6);
    s.v[kRmLinked] = (lk < 2 ? 0xFFFFu : (kFirstUpgradeSlot + g.P(4))) | (g.C(40) ? (g.R() & 0xFFFF0000u) : 0u);
    s.v[kRmScale] = AnyFloat(g);
    const u32 target = kFirstUpgradeSlot + u;
    for (u32 i = 0; i < 15; ++i) {
        u32 w = SlotWord(g);
        if (fam >= 4 && g.C(35)) w = target;                    // varias coincidencias con la mejora retirada
        else if (fam == 3 && g.C(15)) w = target;
        s.v[kRmSlots + i] = w;
    }
    s.v[kRmClump] = g.C(25) ? 0u : 1u;   // 0 nulo, 1 el clump falso de la arena
}

inline void MakeAddUpgrade(Spec& s, Gen& g) {
    const u32 fam = s.family;
    const u32 idxTab[8] = {0, 1, 2, 6, 12, 3, 9, 14};
    s.v[kAuModel] = kFirstUpgradeSlot + g.P(4);
    const u32 idx = (g.C(65)) ? idxTab[fam] : (g.C(50) ? idxTab[g.P(5)] : g.P(15));
    s.v[kAuIndex] = idx;
    s.v[kAuPlateMat] = Ptrish(g);
    s.v[kAuPlateType] = g.C(50) ? g.P(256) : (g.R() & 0xFFFFFFFFu);   // el EXE lee un byte (+0x31): el resto de la palabra es relleno
    for (u32 i = 0; i < 15; ++i) {
        u32 p;
        switch (g.P(6)) {
        case 0: case 1: p = 0xFFFFFFFFu; break;
        case 2: p = 1u; break;
        case 3: p = g.P(8); break;
        case 4: p = g.R(); break;
        default: p = 0xFFFFFFFFu - g.P(3); break;
        }
        s.v[kAuParents + i] = p;
    }
    // el alterno del indice con mas frecuencia distinto de -1 y el padre propio fuera de 1 para ejercer ambas ramas
    const u32 alt[15] = {3, 4, 5, 0, 0, 0, 7, 0, 0, 0, 0, 0, 13, 0, 0};
    if (alt[idx]) {
        if (g.C(55)) s.v[kAuParents + alt[idx]] = g.P(6);          // alterno valido
        else if (g.C(50)) s.v[kAuParents + alt[idx]] = 0xFFFFFFFFu; // alterno -1: se queda con el propio
    }
    if (g.C(25)) s.v[kAuParents + idx] = 1;                         // padre 1: vuelve tras la primera copia
    else if (g.C(15)) s.v[kAuParents + idx] = 0xFFFFFFFFu;
    s.v[kAuClump] = g.C(25) ? 0u : 1u;
    s.v[kAuFrame] = g.C(25) ? 0u : NonNullPtrish(g);
    s.v[kAuRet1] = g.R();
    s.v[kAuRet2] = g.R();
}

inline void MakeAddVehicle(Spec& s, Gen& g) {
    const u32 fam = s.family;
    const u32 m = g.P(4);
    s.v[kAvModel] = kFirstUpgradeSlot + m;
    bool veh[4];
    u32 car[4];
    for (u32 i = 0; i < 4; ++i) {
        car[i] = g.C(35) ? 2u : g.P(32);
        veh[i] = g.C(50);
    }
    switch (fam) {
    case 0: veh[m] = false; break;
    case 1: veh[m] = true; car[m] = 2; break;
    case 2: veh[m] = true; car[m] = g.P(32); break;
    case 3: veh[m] = false; car[m] = g.P(15); break;
    default: break;
    }
    for (u32 i = 0; i < 4; ++i) s.v[kAvFlags + i] = InfoFlags(g, car[i], veh[i]);
    s.v[kAvSetRet] = BoolRet(g, fam == 7 ? true : g.C(12));
    const bool writes = g.C(40);
    s.v[kAvSetWrites] = writes ? 1u : 0u;
    s.v[kAvSetValue] = g.C(50) ? (kFirstUpgradeSlot + g.P(4)) : (g.C(50) ? 0xFFFFFFFFu : g.R());
    const u32 lk = g.P(6);
    s.v[kAvLinked] = lk < 2 ? (0xFFFFu | (g.C(40) ? (g.R() & 0xFFFF0000u) : 0u)) : ((kFirstUpgradeSlot + g.P(4)) | (g.C(30) ? (g.R() & 0xFFFF0000u) : 0u));
    for (u32 i = 0; i < 15; ++i) s.v[kAvSlots + i] = SlotWord(g);
    // la mejora sustituida: -1, una de las ranuras o cualquiera; con fam >= 4 se fuerza a coincidir con alguna ranura
    u32 repl;
    switch (g.P(5)) {
    case 0: repl = 0xFFFFFFFFu; break;
    case 1: repl = kFirstUpgradeSlot + g.P(4); break;
    case 2: repl = s.v[kAvSlots + g.P(15)] & 0xFFFFu; repl = (repl & 0x8000u) ? (repl | 0xFFFF0000u) : repl; break;
    case 3: repl = g.R(); break;
    default: repl = 0xFFFFFFFFu; break;
    }
    s.v[kAvGetRepl] = repl;
    s.v[kAvGetUp] = (g.C(40)) ? s.v[kAvGetRepl] : (g.C(50) ? 0xFFFFFFFFu : (kFirstUpgradeSlot + g.P(4)));
    if (fam >= 5 && g.C(70)) {   // asegura al menos una ranura igual a la sustituida
        const u32 w = s.v[kAvGetUp] & 0xFFFFu;
        s.v[kAvSlots + g.P(15)] = w;
        if (g.C(50)) s.v[kAvSlots + g.P(15)] = w;
    }
    if (g.C(30) && fam >= 5) for (u32 i = 0; i < 15; ++i) s.v[kAvSlots + i] = g.C(70) ? (kFirstUpgradeSlot + g.P(4)) : 0xFFFFu;   // sin hueco o casi
    if (fam == 6 && g.C(60)) {   // sin hueco ni mejora sustituida coincidente: no se guarda nada
        for (u32 i = 0; i < 15; ++i) s.v[kAvSlots + i] = kFirstUpgradeSlot + g.P(4);
        s.v[kAvGetRepl] = s.v[kAvGetUp] = s.v[kAvSetValue] = 0x12345u;
    }
    s.v[kAvWheelFront] = AnyFloat(g);
    s.v[kAvSubType] = g.C(55) ? 0u : (g.C(60) ? 1u + g.P(7) : g.R());
    const u32 modelId = s.v[kAvModel];
    s.v[kAvOffroad] = g.C(40) ? (modelId | (g.C(10) ? 0x10000u : 0u)) : (g.C(50) ? kFirstUpgradeSlot + g.P(4) : (g.R() & 0xFFFFu));
    s.v[kAvHnd] = g.R();
    if (g.C(40)) s.v[kAvHnd] |= 0x100000u;
    else if (g.C(60)) s.v[kAvHnd] &= ~0x100000u;
    s.v[kAvFlags38C] = g.R();
    s.v[kAvScale] = AnyFloat(g);
}

inline void MakeDrop(Spec& s, Gen& g) {
    const u32 fam = s.family;
    static const u32 types[6] = {1, 0, 2, 3, 0xFFFFFFFFu, 0x7FFFFFFFu};
    s.v[kDrType] = fam < 3 ? 1u : (g.C(60) ? types[g.P(6)] : (g.C(70) ? g.P(4) : g.R()));
    s.v[kDrArg1] = (g.R() & 0xFFFFFF00u) | ((fam & 1) ? 0u : (g.C(50) ? 1u : 1u + g.P(255)));   // fam impar: arg1 falso (con basura arriba)
    const u32 rate = g.C(15) ? 0u : g.C(10) ? 0xFFFFFFFFu : g.P(5000);
    s.v[kDrRate] = rate;
    s.v[kDrT1] = g.C(10) ? 0xFFFFFFFFu : (g.C(50) ? g.P(100000) : g.R());
    s.v[kDrT2] = g.C(10) ? 0xFFFFFFFFu : (g.C(50) ? g.P(100000) : g.R());
    const u32 last = (s.v[kDrType] == 1) ? s.v[kDrT1] : s.v[kDrT2];
    const u32 limit = rate + last;   // uint32 con desbordamiento, como el EXE
    u32 time;
    switch (g.P(8)) {
    case 0: time = limit; break;
    case 1: time = limit + 1; break;
    case 2: time = limit - 1; break;
    case 3: time = 0; break;
    case 4: time = 0xFFFFFFFFu; break;
    case 5: time = limit + g.P(5000); break;
    case 6: time = limit - g.P(5000); break;
    default: time = g.R(); break;
    }
    s.v[kDrTime] = time;
    const u32 nanMode = g.P(10);   // 0: NaN/inf solo en la ordenanza (por componente); 1: solo en la posicion; el resto, finitos
    for (u32 i = 0; i < 3; ++i) {
        u32 ord = FloatOf(g, g.C(10) ? 0u : g.C(30) ? 1u : g.C(60) ? 2u : 3u);
        u32 p0 = FloatOf(g, g.C(10) ? 0u : g.C(30) ? 1u : g.C(60) ? 2u : 3u);
        u32 p1 = FloatOf(g, g.C(10) ? 0u : g.C(30) ? 1u : g.C(60) ? 2u : 3u);
        if (nanMode == 0 && g.C(30)) ord = FloatOf(g, g.C(50) ? 4u : 5u);
        if (nanMode == 1 && g.C(30)) { const u32 c = g.C(50) ? 4u : 5u; p0 = FloatOf(g, c); p1 = FloatOf(g, c == 5 ? 4u : c); }
        if (g.C(5)) ord = FloatOf(g, 4u), p0 = FloatOf(g, 4u);   // inf +- inf (da el NaN indefinido)
        // dos NaN en una suma: el orden de operandos de x87 y de SSE difiere; nunca se generan juntos
        if ((ord & 0x7FFFFFFFu) > 0x7F800000u) { if ((p0 & 0x7FFFFFFFu) > 0x7F800000u) p0 = 0; if ((p1 & 0x7FFFFFFFu) > 0x7F800000u) p1 = 0; }
        s.v[kDrOrd + i] = ord;
        s.v[kDrMatPos + i] = p0;
        s.v[kDrPlPos + i] = p1;
    }
    s.v[kDrHasMat] = (fam == 6 || (fam == 7 && g.C(50))) ? 0u : 1u;
    s.v[kDrByte] = g.R() & 0xFFu;
    if (fam == 5) s.v[kDrByte] &= ~0x0Cu;
    else if (fam == 4) s.v[kDrByte] = (s.v[kDrByte] & ~0x0Cu) | (1u << 2 << g.P(2));   // 4 u 8
    s.v[kDrDriver] = g.C(75) ? 1u : 0u;
    s.v[kDrPedType] = g.C(70) ? g.P(2) : (g.C(50) ? 2u + g.P(6) : g.R());
    s.v[kDrProj] = g.R();
}

inline void MakeSiren(Spec& s, Gen& g) {
    const u32 fam = s.family;
    s.v[kSiArg0] = (g.R() & 0xFFFFFF00u) | (g.C(50) ? 0u : 1u + g.P(255));
    s.v[kSiSiren] = BoolRet(g, fam < 5 ? true : g.C(20));
    const u32 idxPick = g.P(8);
    s.v[kSiIndex] = idxPick < 5 ? g.P(5) : (idxPick == 5 ? 5u : (idxPick == 6 ? 255u : g.P(256)));
    for (u32 i = 0; i < 5; ++i) s.v[kSiHist + i] = g.C(55) ? 1u : (g.C(40) ? 0u : 1u + g.P(255));
    // fuerza los patrones que deciden m_HornCounter (indices i, i+4, i+1, i+3 modulo 5, sobre indices < 5)
    if (fam < 5 && s.v[kSiIndex] < 5) {
        const u32 i = s.v[kSiIndex];
        auto set = [&](u32 k, bool on) { s.v[kSiHist + (k % 5)] = on ? 1u + g.P(255) : 0u; };
        switch (fam) {
        case 0: set(i, true);  set(i + 4, true);  set(i + 3, true);  break;   // contador 1
        case 1: set(i, true);  set(i + 4, false); break;
        case 2: set(i, false); set(i + 4, true);  set(i + 1, false); break;   // conmuta
        case 3: set(i, false); set(i + 4, true);  set(i + 1, true);  break;
        default: set(i, true); set(i + 4, true);  set(i + 3, false); break;
        }
    }
    u32 alarm;
    switch (g.P(5)) {
    case 0: alarm = 0u; break;
    case 1: alarm = 0xFFFFu; break;
    case 2: alarm = 1u + g.P(0xFFFE); break;
    case 3: alarm = g.R() & 0xFFFFu; break;
    default: alarm = 0u; break;
    }
    s.v[kSiAlarm] = alarm | (g.C(30) ? (g.R() & 0xFFFF0000u) : 0u);
    const u32 st = g.R() & 0xFFu;
    s.v[kSiStatus] = g.C(35) ? ((st & 0x07u) | 0x28u) : (g.C(40) ? (st & ~0xF8u) | (g.P(32) << 3) : st);
    s.v[kSiHorn] = BoolRet(g, g.C(50));
    s.v[kSiFlags] = g.R() & 0xFFu;
    s.v[kSiCounter] = g.C(50) ? g.P(3) : g.R();
}

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + fn * 0x20000u);
    s.family = n % kFamilies;
    s.stack = StackSeed(n);
    s.eax = g.R(); s.edx = g.R();
    switch (fn) {
    case kRemove: MakeRemove(s, g); break;
    case kAddUp: MakeAddUpgrade(s, g); break;
    case kAddVeh: MakeAddVehicle(s, g); break;
    case kDrop: MakeDrop(s, g); break;
    default: MakeSiren(s, g); break;
    }
    return s;
}

// Relleno inicial de la arena: palabras con los bits 31 y 30 puestos (nunca coinciden con una direccion de la arena); los campos de cada funcion se escriben encima.
inline u32 FillWord(u32 fn, u32 n, u32 i) { return 0xC0000000u | (Rand(n + fn * 0x20000u + 0x777u, 1000u + i) >> 2); }

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"v\":[";
    for (u32 i = 0; i < kVWords; ++i) o += (i ? "," : "") + U(s.v[i]);
    o += "],\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.edx) + "]}";
    return o;
}
}  // namespace V66Fix
