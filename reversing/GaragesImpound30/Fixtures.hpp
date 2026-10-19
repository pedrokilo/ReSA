#pragma once
// Fixtures del banco GaragesImpound30: CGarages::StoreCarInNearestImpoundingGarage (0x44A3C0, cdecl, 1 dword de argumento = CVehicle*, `ret` normal, void).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_dump.cpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco.
//
// Memoria que el banco SUSTITUYE por la del caso (se guarda la real y se restaura al final):
//   * CGarages::aGarages (0x96C048, 50 x 0xD8) y CGarages::NumGarages (0x96C024): las `n_garages` primeras ranuras llevan (tipo en +0x4C, x en +0x00, y en +0x04) y bytes aleatorios en el resto.
//   * CGarages::aCarsInSafeHouse (0x96ABD8, 20 refugios x 4 ranuras x 0x40 bytes = 0x1400): todo vale `fill` salvo los refugios que referencian los falsos de FindSafeHouseIndexForGarageType
//     (hasta tres), que llevan 4 ranuras aleatorias con el modelo (+0x12, uint16) elegido por el caso.
//   * El "vehiculo" es un bloque de 0x40 bytes de la arena (offsets +04..+0F posicion, +14 puntero a la matriz) y la "matriz" otro bloque de 0x40 (posicion en +30..+3B). Si veh_matrix == 0 el
//     puntero vale 0 y la funcion lee la posicion del propio vehiculo; si no, la de la matriz. La otra posicion lleva un valor distinto (`veh_other`).
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): CGarages::FindSafeHouseIndexForGarageType (0x4489F0; devuelve find[tipo - 0x21],
// conserva ECX/EDX como el EXE) y CStoredCar::StoreCar (0x449760; anota la ranura y el vehiculo, escribe en la ranura 0xC0DE0000 | indice-de-ranura en +0 y 0x7777 en +0x12).
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace GaragesImpound30Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kKinds = 16;
constexpr unsigned kPerKind = 256;
constexpr unsigned kCases = kKinds * kPerKind;           // 4096
constexpr u32 kEntryBytes = 0xD8;                        // sizeof(CGarage)
constexpr u32 kMaxGarages = 50;
constexpr u32 kGarageBytes = kMaxGarages * kEntryBytes;  // 0x2A30
constexpr u32 kGarageAddress = 0x96C048;
constexpr u32 kNumAddress = 0x96C024;
constexpr u32 kSafehouses = 20;
constexpr u32 kSlotsPerHouse = 4;
constexpr u32 kSlotBytes = 0x40;                         // sizeof(CStoredCar)
constexpr u32 kHouseBytes = kSlotsPerHouse * kSlotBytes; // 0x100
constexpr u32 kTableBytes = kSafehouses * kHouseBytes;   // 0x1400
constexpr u32 kTableAddress = 0x96ABD8;
constexpr u32 kChunks = kSafehouses * kSlotsPerHouse;    // 80
constexpr u32 kVehicleBytes = 0x40;
constexpr u32 kMatrixBytes = 0x40;
static_assert(kGarageBytes == 0x2A30 && kTableBytes == 0x1400 && kChunks == 80, "tamanos de las tablas");

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
constexpr u32 kInitialMin = 0x47C34FF3u;                 // distancia inicial (99999.898...)

inline constexpr u32 kSpecial[]{
    0u, 0x80000000u, Fb(1.0f), Fb(-1.0f), Fb(0.5f), Fb(100.0f), Fb(-250.5f), Fb(1.0e-30f), Fb(-1.0e-30f), Fb(1.0e-20f), Fb(1.0e10f), Fb(1.8e19f), Fb(-1.8e19f), Fb(3.0e19f),
    Fb(3.0e38f), Fb(-3.0e38f), 0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u,
    0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC12345u, 0x7F800001u, 0xFF800001u,
};
inline constexpr u32 kNans[]{0x7FC00000u, 0xFFC00000u, 0x7FC12345u, 0xFFC00001u, 0x7F800001u, 0x7FA00000u, 0xFF800001u, 0x7FFFFFFFu};
inline constexpr u32 kInfs[]{0x7F800000u, 0xFF800000u};
inline constexpr u32 kModels[]{0u, 1u, 2u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x00FFu, 0x0100u, 400u, 411u};   // modelo (uint16 en +0x12): >0 sin signo
inline constexpr u32 kTypes[]{0x21u, 0x22u, 0x23u};

struct GarageFx { u32 type{}, x{}, y{}; };
struct HouseFx { u32 index{}; std::array<u8, kHouseBytes> bytes{}; };
struct Fx {
    unsigned n{};
    u32 kind{};
    std::vector<GarageFx> garages;
    u32 vehMatrix{};                       // 0: sin matriz (posicion del vehiculo); 1: con matriz (posicion de la matriz)
    u32 vpos[3]{}, vother[3]{};            // posicion leida / posicion no usada (bits)
    u32 find[3]{};                         // refugio devuelto por el falso para los tipos 0x21, 0x22, 0x23 (0..19)
    u8 fill{};
    std::vector<HouseFx> houses;
    u32 eax{}, ecx{}, edx{}, stack{};
};

inline u32 Typical(u32 c, u32 k) { return Fb(float(int(Rand(c, k) % 8000001u) - 4000000) / 1000.0f); }       // +-4000 con resolucion 0.001
inline u32 Special(u32 c, u32 k) { return kSpecial[Rand(c, k) % std::size(kSpecial)]; }
inline u32 Lattice(u32 c, u32 k, u32 n) { return Fb(float(Rand(c, k) % n)); }
inline u32 ImpoundType(u32 c, u32 k) { return kTypes[Rand(c, k) % 3]; }
inline u32 AnyType(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 4;
    if (r == 0) return ImpoundType(c, k + 1);
    if (r == 1) { static constexpr u32 edge[]{0x1Fu, 0x20u, 0x21u, 0x23u, 0x24u, 0x25u, 0xA1u, 0xA3u, 0x00u, 0xFFu, 0x42u, 0x2Cu}; return edge[Rand(c, k + 1) % std::size(edge)]; }
    return Rand(c, k + 2) % 0x2Eu;
}

inline void AddHouse(Fx& f, u32 c, u32 k, u32 index, const u32* models /* 4 */) {
    for (const auto& h : f.houses) if (h.index == index) return;
    HouseFx h;
    h.index = index;
    FillWords(h.bytes.data(), kHouseBytes, Rand(c, k));
    for (u32 s = 0; s < kSlotsPerHouse; ++s) {
        const u16 m = u16(models[s]);
        std::memcpy(h.bytes.data() + s * kSlotBytes + 0x12, &m, 2);
    }
    f.houses.push_back(h);
}
// modelos de las cuatro ranuras: mask bit s = ocupada (modelo "bueno" aleatorio >0) o libre (0)
inline void Occupancy(u32 c, u32 k, u32 mask, u32* models) {
    for (u32 s = 0; s < 4; ++s) {
        if (mask & (1u << s)) { u32 m = kModels[Rand(c, k + s) % std::size(kModels)]; if (m == 0) m = 1 + Rand(c, k + 8 + s) % 0xFFFFu; models[s] = m; }
        else models[s] = 0;
    }
}

inline Fx Make(unsigned n) {
    Fx f;
    const u32 c = n;
    f.n = n;
    f.kind = n % kKinds;
    const u32 round = n / kKinds;                    // 0..255
    f.vehMatrix = Rand(c, 1) % 2;
    for (unsigned i = 0; i < 3; ++i) { f.vpos[i] = Typical(c, 2 + i); f.vother[i] = Typical(c, 6 + i); }
    f.fill = u8(0x90 + Rand(c, 9) % 0x60);
    f.eax = Rand(c, 10); f.ecx = Rand(c, 11); f.edx = Rand(c, 12);
    f.stack = Rand(c, 13) % 2 ? kStackSeeds[Rand(c, 14) % kStackSeeds.size()] : Rand(c, 15);
    for (u32 i = 0; i < 3; ++i) f.find[i] = Rand(c, 16 + i) % 4 == 0 ? (Rand(c, 20 + i) % 2 ? 0u : kSafehouses - 1) : Rand(c, 24 + i) % kSafehouses;
    u32 count = 1 + Rand(c, 30) % 8;
    auto addGarage = [&](u32 type, u32 x, u32 y) { f.garages.push_back({type, x, y}); };
    auto typicalGarages = [&](u32 total, u32 pImpound) {
        for (u32 i = 0; i < total; ++i) addGarage(Rand(c, 40 + i) % 100 < pImpound ? ImpoundType(c, 140 + i) : AnyType(c, 240 + i), Typical(c, 340 + 2 * i), Typical(c, 341 + 2 * i));
    };
    u32 occ = Rand(c, 31) % 16;
    u32 models[4]{};
    switch (f.kind) {
    case 0:                                          // tipicos: pocos garajes, varios de embargo
        typicalGarages(count, 50);
        break;
    case 1:                                          // sin garajes de embargo
        for (u32 i = 0; i < count; ++i) { u32 t = AnyType(c, 40 + i); if (t >= 0x21 && t <= 0x23) t = 0x20; addGarage(t, Typical(c, 340 + 2 * i), Typical(c, 341 + 2 * i)); }
        break;
    case 2:                                          // NumGarages = 0
        break;
    case 3:                                          // muchos garajes (hasta 50), casi todos de embargo
        count = 20 + Rand(c, 32) % 31;
        typicalGarages(count, 80);
        break;
    case 4: {                                        // alrededor de la distancia inicial 99999.9: unos dentro y otros fuera
        count = 1 + Rand(c, 32) % 6;
        const float d = 99999.0f + float(Rand(c, 33) % 2001u) / 1000.0f;
        for (u32 i = 0; i < count; ++i) {
            const float dd = d + float(int(Rand(c, 34 + i) % 5) - 2) * 0.0078125f * float(Rand(c, 50 + i) % 3);
            addGarage(ImpoundType(c, 140 + i), Fb(dd), Fb(0.0f));
        }
        f.vpos[0] = Fb(0.0f); f.vpos[1] = Fb(0.0f); f.vehMatrix = Rand(c, 60) % 2;
        break;
    }
    case 5:                                          // NaN en las posiciones de garaje o vehiculo
        typicalGarages(count, 85);
        for (auto& g : f.garages) { if (Rand(c, 70) % 3 == 0) g.x = kNans[Rand(c, 71) % std::size(kNans)]; if (Rand(c, 72) % 3 == 0) g.y = kNans[Rand(c, 73) % std::size(kNans)]; }
        if (Rand(c, 74) % 3 == 0) f.vpos[0] = kNans[Rand(c, 75) % std::size(kNans)];
        if (Rand(c, 76) % 4 == 0) f.vpos[1] = kNans[Rand(c, 77) % std::size(kNans)];
        break;
    case 6:                                          // infinitos y enormes (cuadrados y diferencias que desbordan float pero no la x87)
        typicalGarages(count, 85);
        for (auto& g : f.garages) {
            if (Rand(c, 70) % 2) g.x = Rand(c, 78) % 4 == 0 ? kInfs[Rand(c, 79) % 2] : Special(c, 71);
            if (Rand(c, 72) % 2) g.y = Rand(c, 80) % 4 == 0 ? kInfs[Rand(c, 81) % 2] : Special(c, 73);
        }
        if (Rand(c, 74) % 2) f.vpos[0] = Rand(c, 82) % 4 == 0 ? kInfs[Rand(c, 83) % 2] : Special(c, 75);
        if (Rand(c, 76) % 2) f.vpos[1] = Rand(c, 84) % 4 == 0 ? kInfs[Rand(c, 85) % 2] : Special(c, 77);
        break;
    case 7:                                          // diminutos: los cuadrados desbordan por abajo en float pero no en la x87; el primero gana por poco
        for (u32 i = 0; i < count; ++i) {
            static constexpr float tiny[]{1.0e-30f, 2.0e-30f, 1.0e-20f, 3.0e-20f, 1.0e-25f, 1.0e-35f, 1.0e-10f, 0.0f, 1.0e-40f};
            const float t = tiny[Rand(c, 34 + i) % std::size(tiny)] * float(int(Rand(c, 50 + i) % 5) - 2);
            addGarage(ImpoundType(c, 140 + i), Fb(t), Fb(tiny[Rand(c, 60 + i) % std::size(tiny)]));
        }
        f.vpos[0] = Rand(c, 70) % 2 ? Fb(0.0f) : Fb(1.0e-30f); f.vpos[1] = Rand(c, 71) % 2 ? Fb(-0.0f) : Fb(0.0f);
        break;
    case 8:                                          // ocupacion de las tres primeras ranuras: los 16 patrones (la cuarta no cuenta)
        typicalGarages(1 + Rand(c, 32) % 3, 100);
        occ = round % 16;
        break;
    case 9:                                          // tres ranuras ocupadas: desplazamiento 1->0 y 2->1
        typicalGarages(1 + Rand(c, 32) % 3, 100);
        occ = 7 | ((Rand(c, 33) % 2) << 3);
        break;
    case 10:                                         // con y sin matriz: posiciones distintas
        typicalGarages(count, 100);
        f.vehMatrix = round % 2;
        break;
    case 11:                                         // los tres tipos de embargo y refugios 0..19 devueltos por el falso
        for (u32 i = 0; i < 3 + count; ++i) addGarage(kTypes[i % 3], Typical(c, 340 + 2 * i), Typical(c, 341 + 2 * i));
        f.find[0] = round % kSafehouses; f.find[1] = (round + 7) % kSafehouses; f.find[2] = (round / 20 + 13) % kSafehouses;
        break;
    case 12:                                         // tipos frontera (0x20/0x21/0x23/0x24...), todos a la misma distancia
        for (u32 i = 0; i < 2 + count; ++i) addGarage(AnyType(c, 40 + i), Fb(float(int(Rand(c, 34 + i) % 7) - 3)), Fb(0.0f));
        f.vpos[0] = Fb(0.0f); f.vpos[1] = Fb(0.0f);
        break;
    case 13:                                         // empates de distancia y ceros con signo: gana el primero
        for (u32 i = 0; i < 2 + count; ++i) addGarage(ImpoundType(c, 140 + i), Lattice(c, 34 + i, 3), Lattice(c, 50 + i, 3));
        f.vpos[0] = Lattice(c, 70, 3); f.vpos[1] = Lattice(c, 71, 3);
        if (Rand(c, 72) % 4 == 0) { f.vpos[0] = Fb(-0.0f); f.garages[0].x = Fb(0.0f); }
        break;
    case 14:                                         // bits crudos en las posiciones
        typicalGarages(count, 90);
        for (auto& g : f.garages) { if (Rand(c, 70) % 2) g.x = Rand(c, 71); if (Rand(c, 72) % 2) g.y = Rand(c, 73); }
        if (Rand(c, 74) % 2) f.vpos[0] = Rand(c, 75);
        if (Rand(c, 76) % 2) f.vpos[1] = Rand(c, 77);
        break;
    case 15: {                                       // el ganador esta al final / es la ranura 49 / el unico de embargo es el ultimo
        const u32 total = (round % 4 == 0) ? kMaxGarages : 2 + Rand(c, 32) % 10;
        for (u32 i = 0; i < total; ++i) addGarage(i + 1 == total ? ImpoundType(c, 140 + i) : (Rand(c, 40 + i) % 3 ? 0x20u : AnyType(c, 240 + i)), Typical(c, 340 + 2 * i), Typical(c, 341 + 2 * i));
        if (total > 1) { f.garages.back().x = f.vpos[0]; f.garages.back().y = f.vpos[1]; if (f.garages.back().type < 0x21) f.garages.back().type = 0x22; }
        break;
    }
    }
    // refugios: los tres que pueden devolver los falsos (hasta tres distintos)
    Occupancy(c, 400, occ, models);
    AddHouse(f, c, 410, f.find[0], models);
    if (f.kind != 8 && f.kind != 9) Occupancy(c, 420, Rand(c, 430) % 16, models);
    AddHouse(f, c, 440, f.find[1], models);
    if (f.kind != 8 && f.kind != 9) Occupancy(c, 450, Rand(c, 460) % 16, models);
    AddHouse(f, c, 470, f.find[2], models);
    if (f.kind == 8 || f.kind == 9) {   // un solo refugio relevante: los tres tipos apuntan al mismo
        f.find[1] = f.find[0]; f.find[2] = f.find[0];
        f.houses.resize(1);
    }
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
    std::string o = "{\"kind\":" + std::to_string(f.kind) + ",\"garages\":[";
    for (std::size_t i = 0; i < f.garages.size(); ++i) {
        if (i) o += ',';
        o += "[" + std::to_string(f.garages[i].type) + "," + std::to_string(f.garages[i].x) + "," + std::to_string(f.garages[i].y) + "]";
    }
    o += "],\"veh_matrix\":" + std::to_string(f.vehMatrix) + ",\"veh_pos\":[" + std::to_string(f.vpos[0]) + "," + std::to_string(f.vpos[1]) + "," + std::to_string(f.vpos[2])
        + "],\"veh_other\":[" + std::to_string(f.vother[0]) + "," + std::to_string(f.vother[1]) + "," + std::to_string(f.vother[2]) + "],\"find\":[" + std::to_string(f.find[0]) + ","
        + std::to_string(f.find[1]) + "," + std::to_string(f.find[2]) + "],\"fill\":" + std::to_string(f.fill) + ",\"houses\":[";
    for (std::size_t i = 0; i < f.houses.size(); ++i) {
        if (i) o += ',';
        o += "[" + std::to_string(f.houses[i].index) + ",\"" + HexOf(f.houses[i].bytes.data(), kHouseBytes) + "\"]";
    }
    o += "],\"eax\":" + std::to_string(f.eax) + ",\"ecx\":" + std::to_string(f.ecx) + ",\"edx\":" + std::to_string(f.edx) + ",\"stack_seed\":" + std::to_string(f.stack) + "}";
    return o;
}
// Constantes que el oraculo (fixture_oracle.py) debe repetir EXACTAMENTE; la autoprueba cruzada las compara con este volcado.
inline std::string ConstantsLine() {
    return "{\"entry_bytes\":" + std::to_string(kEntryBytes) + ",\"max_garages\":" + std::to_string(kMaxGarages) + ",\"garage_bytes\":" + std::to_string(kGarageBytes)
        + ",\"garage_address\":" + std::to_string(kGarageAddress) + ",\"num_address\":" + std::to_string(kNumAddress) + ",\"safehouses\":" + std::to_string(kSafehouses)
        + ",\"slots_per_house\":" + std::to_string(kSlotsPerHouse) + ",\"slot_bytes\":" + std::to_string(kSlotBytes) + ",\"house_bytes\":" + std::to_string(kHouseBytes)
        + ",\"table_bytes\":" + std::to_string(kTableBytes) + ",\"table_address\":" + std::to_string(kTableAddress) + ",\"chunks\":" + std::to_string(kChunks)
        + ",\"vehicle_bytes\":" + std::to_string(kVehicleBytes) + ",\"matrix_bytes\":" + std::to_string(kMatrixBytes) + ",\"initial_min\":" + std::to_string(kInitialMin)
        + ",\"cases\":" + std::to_string(kCases) + "}";
}
}
