#pragma once
// Fixtures del banco Fn87B (lote 87B): CPlayerPed::FindNextWeaponLockOnTarget (EXE 0x60E530, thiscall, 2 argumentos en pila, `ret 8`, bool en AL).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp. Todos los floats viajan como patrones de 32 bits.
// Solo se usa un patron de NaN (0xFFC00000, el "indefinido" de la x87): asi la seleccion de NaN de SSE y de la x87 no puede diferir.
// Reparto: n < 16384: familia = n % 16 (1024 casos por familia); despues las familias 16..19 de 1024 casos (ver MakeCase).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace Fn87BFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;
constexpr unsigned kCases = kBaseCases + 4 * 1024;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;
constexpr unsigned kSites = 14;                      // sitios de mutacion (llamadas a colaboradores)
constexpr unsigned kPeds = 5, kVehs = 3, kObjs = 3;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPlayerWin = 0x800, kDataWin = 0x100, kMatWin = 0x40, kEntWin = 0x40, kInfoWin = 0x40, kHdrWin = 0x20, kFlagsWin = 0x10, kIntelWin = 0x100, kGrpWin = 0x40, kEvWin = 0x40;
constexpr std::size_t kPedsWin = kPeds * 0x7C4, kVehsWin = kVehs * 0xA18, kObjsWin = kObjs * 0x19C;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 87021u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

struct Js {   // JSON minimo
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& U(const std::string& k, u32 v) { return U(k.c_str(), v); }
    std::string End() const { return s + "}"; }
};

inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
inline u32 Pick(u32 n, u32 k, std::initializer_list<u32> v) { return *(v.begin() + Rand(n, k) % v.size()); }
inline constexpr u32 kNanBits = 0xFFC00000u, kInfBits = 0x7F800000u, kNInfBits = 0xFF800000u;

// Coordenadas: normales (tres decimales), ceros, grandes y de vez en cuando infinitos y NaN
inline u32 Coord(u32 n, u32 k, u32 specials = 20) {
    const u32 r = Rand(n, k) % 100;
    if (r < specials / 4) return Pick(n, k + 1, {kInfBits, kNInfBits});
    if (r < specials / 2) return kNanBits;
    if (r < specials) return Pick(n, k + 2, {0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x461C4000u, 0xC61C4000u, 0x49742400u, 0x358637BDu});
    return B(float(i32(Rand(n, k + 3) % 200001) - 100000) / 1000.0f);
}
inline u32 Angle(u32 n, u32 k) {   // resultados de los falsos de atan: moderados, de vez en cuando NaN
    const u32 r = Rand(n, k) % 100;
    if (r < 6) return Pick(n, k + 1, {0u, 0x80000000u, 0x40490FDBu, 0xC0490FDBu, 0x40C90FDBu, 0xC0C90FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x3F000000u});
    if (r < 10) return kNanBits;
    return B(float(i32(Rand(n, k + 2) % 40001) - 20000) / 2000.0f);
}
inline u32 Range(u32 n, u32 k) {   // alcances y multiplicadores
    const u32 r = Rand(n, k) % 100;
    if (r < 3) return Pick(n, k + 1, {kInfBits, kNanBits, 0u, 0x80000000u});
    return B(float(Rand(n, k + 2) % 20001) / 100.0f);
}

struct EntC {                   // una entidad del pool: posicion, matriz y atributos
    u32 pr{}, x{}, y{}, z{}, mt{};
    u32 a{}, b{}, c{}, d{}, e{}, f{}, g{}, t{};
    // peaton: a estado, b en vehiculo, c vehiculo (0 ninguno, 1..3 vehiculo i-1, 4 nulo), d nunca objetivo, e tipo de peaton, f mismo grupo, g LOS bloqueada, t byte de tipo (+0x36)
    // vehiculo: a banderas +0x40, b subtipo +0x594, c tipo +0x590, d byte +0x42D, t byte de tipo
    // objeto: a CanBeTargetted, b byte +0x140, c objeto RW (+0x18), t byte de tipo
};
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 fx{}, fy{};                                   // vector hacia delante de la matriz del jugador (+0x10, +0x14)
    u32 slot{}, wtype{}, tgt{}, rng{}, skill{}, twoPlayer{};
    u32 cur{}, cx{}, cy{}, cz{}, cmt{}, arg1{};      // cur: 0 nulo, 1 entidad propia, 2..6 peaton i-2 del pool
    u32 camMt{}, camX{}, camY{}, camZ{}, cam9AC{}, cam9B0{};
    u32 fire{}, hasGroup{}, coop{}, sameAfter{};   // sameAfter: respuesta de AreInSameGroup(mejor, yo) en el aviso (la del bucle de peatones es la del peaton)
    std::array<u32, 12> atan{}, mult{}, evalSet{}, evalPrio{}, reach{};
    std::array<EntC, kPeds> ped{};
    std::array<EntC, kVehs> veh{};
    std::array<EntC, kObjs> obj{};
    u32 pedSize{}, vehSize{}, objSize{};
    u32 mutSite{}, mutKind{}, mutRange{}, mutSlot{}, mutWtype{};
};

inline void FillEnt(EntC& e, u32 n, u32 k, u32 specials) {
    e.pr = Rand(n, k) % 5 != 0;
    e.x = Coord(n, k + 1, specials); e.y = Coord(n, k + 2, specials); e.z = Coord(n, k + 3, specials);
    e.mt = Rand(n, k + 4) % 2;
}
inline u32 TypeByte(u32 n, u32 k, u32 type, u32 wrongPercent = 12, u32 forbidden = 8) {   // byte +0x36: tipo en los 3 bits bajos y estado en los altos
    const u32 hi = (Rand(n, k) & 0x1Fu) << 3;
    u32 t = type;
    if (Rand(n, k + 1) % 100 < wrongPercent) { t = Rand(n, k + 2) % 8; if (t == type || t == forbidden) t = (type + 1) & 7u; }
    return t | hi;
}

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 9000);
    c.eax = SeedReg(n, 9001); c.edx = SeedReg(n, 9002); c.stack = StackSeed(n);
    c.fx = Coord(n, 9010, 6); c.fy = Coord(n, 9011, 6);
    c.slot = Rand(n, 9018) % 5 == 0 ? u32(i32(Rand(n, 9019) % 13) - 3) : Rand(n, 9019) % 13; c.slot &= 0xFFu;
    c.wtype = Rand(n, 9020) % 47;
    c.tgt = Rand(n, 9021) % 3 == 0;
    c.rng = Range(n, 9023); c.skill = Rand(n, 9025) % 4; c.twoPlayer = Rand(n, 9031) % 3 == 0 ? Pick(n, 9032, {1u, 1u, 0x100u, 0x200u, 0x101u, 0xFF00u, 0x1234u, 0xFFu}) : 0u;
    c.cur = Rand(n, 9034) % 3 == 0 ? 0u : (Rand(n, 9035) % 6 == 0 ? 2u + Rand(n, 9036) % kPeds : 1u);
    c.cx = Coord(n, 9037, 6); c.cy = Coord(n, 9038, 6); c.cz = Coord(n, 9039, 6); c.cmt = Rand(n, 9040) % 2;
    c.arg1 = Rand(n, 9041) % 4 == 0 ? Rand(n, 9042) : Rand(n, 9043) % 2;
    c.camMt = Rand(n, 9044) % 2; c.camX = Coord(n, 9045, 6); c.camY = Coord(n, 9046, 6); c.camZ = Coord(n, 9047, 6);
    c.cam9AC = Coord(n, 9048, 8); c.cam9B0 = Coord(n, 9049, 8);
    c.fire = Rand(n, 9050) % 4 == 0 ? 0u : Pick(n, 9051, {1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 0x100u, 0x80000000u});
    c.hasGroup = Rand(n, 9052) % 2; c.coop = Rand(n, 9053) % 2; c.sameAfter = Rand(n, 9054) % 4 == 0 ? (Rand(n, 9055) % 2 ? 1u : 0x5A5A5A01u) : 0u;
    for (unsigned i = 0; i < c.atan.size(); ++i) { c.atan[i] = Angle(n, 9060 + i); c.mult[i] = Range(n, 9080 + i); c.evalSet[i] = Rand(n, 9100 + i) % 3 == 0; c.evalPrio[i] = Angle(n, 9120 + i); c.reach[i] = Rand(n, 9140 + i) % 3 != 0; }
    for (unsigned i = 0; i < kPeds; ++i) {
        EntC& e = c.ped[i]; const u32 k = 9300 + i * 16; FillEnt(e, n, k, 12);
        e.a = Rand(n, k + 5) % 6 == 0 ? Pick(n, k + 6, {0x36u, 0x37u, 0x35u, 0x38u}) : Rand(n, k + 7) % 54;
        e.b = Rand(n, k + 8) % 6 == 0;
        e.c = Rand(n, k + 9) % 5;
        e.d = Rand(n, k + 10) % 8 == 0;
        e.e = Rand(n, k + 11) % 3 ? Rand(n, k + 12) % 2 : Pick(n, k + 13, {2u, 3u, 8u, 0x100u});
        e.f = Rand(n, k + 14) % 6 == 0;
        e.g = Rand(n, k + 15) % 5 == 0;
        e.t = TypeByte(n, k + 100, 3);
    }
    for (unsigned i = 0; i < kVehs; ++i) {
        EntC& e = c.veh[i]; const u32 k = 9500 + i * 16; FillEnt(e, n, k, 12);
        e.a = Rand(n, k + 5) % 4 == 0 ? 0x20000000u | Rand(n, k + 6) : Rand(n, k + 7) & ~0x20000000u;
        e.b = Rand(n, k + 8) % 5 == 0 ? 10u : Rand(n, k + 9) % 10;
        e.c = Rand(n, k + 10) % 3 == 0 ? 9u : Rand(n, k + 11) % 9;
        e.d = Rand(n, k + 12) & 0xFFu;
        e.t = TypeByte(n, k + 100, 2);
    }
    for (unsigned i = 0; i < kObjs; ++i) {
        EntC& e = c.obj[i]; const u32 k = 9700 + i * 16; FillEnt(e, n, k, 12);
        e.a = Rand(n, k + 5) % 3 != 0;
        e.b = Rand(n, k + 6) & 0xFFu;
        e.c = Rand(n, k + 7) % 4 != 0;
        e.t = TypeByte(n, k + 100, 4, 12, 3);   // un objeto con tipo peaton leeria la inteligencia en +0x47C, fuera de su hueco de la arena (el stride 0x19C es menor)
    }
    c.pedSize = Rand(n, 9900) % (kPeds + 1); c.vehSize = Rand(n, 9901) % (kVehs + 1); c.objSize = Rand(n, 9902) % (kObjs + 1);
    return c;
}

inline C MakeCase(unsigned n) {
    C c = BaseDefaults(n);
    unsigned fam = c.family;
    if (n >= kBaseCases) fam = 16 + (n - kBaseCases) / 1024;
    c.family = fam;
    auto calm = [&] {   // finitos y moderados en todo el escenario
        auto fix = [&](u32& v, u32 k) { if (!std::isfinite(F(v)) || std::fabs(F(v)) > 1.0e5f) v = B(float(i32(Rand(n, k) % 20001) - 10000) / 100.0f); };
        fix(c.fx, 10100); fix(c.fy, 10101); fix(c.cx, 10102); fix(c.cy, 10103); fix(c.cz, 10104); fix(c.camX, 10105); fix(c.camY, 10106); fix(c.camZ, 10107); fix(c.cam9AC, 10108); fix(c.cam9B0, 10109);
        for (unsigned i = 0; i < kPeds; ++i) { fix(c.ped[i].x, 10110 + i * 3); fix(c.ped[i].y, 10111 + i * 3); fix(c.ped[i].z, 10112 + i * 3); }
        for (unsigned i = 0; i < kVehs; ++i) { fix(c.veh[i].x, 10130 + i * 3); fix(c.veh[i].y, 10131 + i * 3); fix(c.veh[i].z, 10132 + i * 3); }
        for (unsigned i = 0; i < kObjs; ++i) { fix(c.obj[i].x, 10140 + i * 3); fix(c.obj[i].y, 10141 + i * 3); fix(c.obj[i].z, 10142 + i * 3); }
    };
    auto onlyPeds = [&](unsigned minSize) { c.pedSize = minSize + Rand(n, 10200) % (kPeds + 1 - minSize); c.vehSize = c.objSize = 0; };
    auto openPeds = [&] {   // peatones validos (sin banderas que los salten) para llegar a la evaluacion
        for (unsigned i = 0; i < kPeds; ++i) { EntC& e = c.ped[i]; e.pr = 1; e.a = 1 + Rand(n, 10210 + i) % 30; e.b = 0; e.c = 0; e.d = 0; e.e = 2 + Rand(n, 10220 + i) % 6; e.f = 0; e.g = 0; e.t = 3 | ((Rand(n, 10230 + i) & 0x1Fu) << 3); }
        c.twoPlayer = 0;
        for (auto& v : c.reach) v = 1;
    };
    switch (fam) {
    case 0: c.pedSize = c.vehSize = c.objSize = 0; break;                                                         // mundo vacio: false
    case 1: onlyPeds(1); openPeds(); calm(); break;
    case 2: onlyPeds(1); for (unsigned i = 0; i < kPeds; ++i) { c.ped[i].a = Rand(n, 10240 + i) % 54; c.ped[i].b = Rand(n, 10250 + i) % 2; c.ped[i].c = Rand(n, 10260 + i) % 5; } calm(); break;
    case 3: c.pedSize = c.vehSize = 0; c.objSize = 1 + Rand(n, 10270) % kObjs; calm(); break;
    case 4: c.pedSize = c.objSize = 0; c.vehSize = 1 + Rand(n, 10271) % kVehs; c.coop = Rand(n, 10272) % 4 != 0; calm(); break;
    case 5: c.pedSize = 1 + Rand(n, 10273) % kPeds; c.vehSize = 1 + Rand(n, 10274) % kVehs; c.objSize = 1 + Rand(n, 10275) % kObjs; c.coop = 1; openPeds(); calm(); break;
    case 6: c.cur = 1; onlyPeds(1); openPeds(); calm(); break;                                                   // objetivo actual propio: rumbo respecto a la camara
    case 7: c.cur = 2 + Rand(n, 10276) % kPeds; onlyPeds(1); openPeds(); calm(); break;                          // el objetivo actual es un peaton del pool: se salta
    case 8: onlyPeds(1); openPeds(); c.fire = 1 + Rand(n, 10277) % 5; for (unsigned i = 0; i < 12; ++i) c.evalSet[i] = Rand(n, 10280 + i) % 2; calm(); break;   // evento sobre un peaton
    case 9: c.pedSize = 0; c.objSize = 1 + Rand(n, 10292) % kObjs; c.vehSize = Rand(n, 10293) % (kVehs + 1); c.coop = 1; for (unsigned i = 0; i < 12; ++i) c.evalSet[i] = Rand(n, 10294 + i) % 2; calm(); break;   // mejor objeto o vehiculo
    case 10: onlyPeds(1); openPeds(); c.fire = 0; for (unsigned i = 0; i < 12; ++i) c.evalSet[i] = Rand(n, 10310 + i) % 2; calm(); break;   // cuerpo a cuerpo: sin evento
    case 11: onlyPeds(1); openPeds(); for (auto& e : c.ped) e.t = (3u + 1u + Rand(n, 10330) % 6) & 7u; for (unsigned i = 0; i < 12; ++i) c.evalSet[i] = Rand(n, 10340 + i) % 2; c.fire = 1; calm(); break;   // el mejor no es de tipo peaton
    case 12: c.pedSize = 0; c.objSize = 1 + Rand(n, 10352) % kObjs; c.vehSize = Rand(n, 10353) % (kVehs + 1); c.coop = 1; c.cur = Rand(n, 10354) % 2; calm(); break;   // pool de peatones vacio: el multiplicador se pide con el objetivo actual
    case 13: onlyPeds(1); c.objSize = 1 + Rand(n, 10355) % kObjs; c.vehSize = 1 + Rand(n, 10356) % kVehs; c.coop = 1; c.ped[0].pr = Rand(n, 10357) % 2; for (unsigned i = 1; i < kPeds; ++i) c.ped[i].pr = 1; calm(); break;   // hueco 0 libre u ocupado
    case 14: c.mutSite = 1 + Rand(n, 10358) % kSites; c.mutKind = 1 + Rand(n, 10359) % 15; c.mutRange = Range(n, 10360); c.mutSlot = Rand(n, 10361) % 13; c.mutWtype = Rand(n, 10362) % 47;
             c.pedSize = 1 + Rand(n, 10363) % kPeds; c.objSize = Rand(n, 10364) % (kObjs + 1); c.vehSize = Rand(n, 10365) % (kVehs + 1); c.coop = 1; openPeds(); calm(); break;
    default: if (Rand(n, 10366) % 2) calm(); break;
    }
    if (fam >= 16) {
        const unsigned j = (n - kBaseCases) % 1024;
        if (fam == 16) {   // barrido de mutaciones: sitio (14) x tipo (15) x 4 escenarios
            const unsigned v = j / (kSites * 15);
            c.mutSite = 1 + j % kSites; c.mutKind = 1 + (j / kSites) % 15;
            c.mutRange = Range(n, 10400); c.mutSlot = Rand(n, 10401) % 13; c.mutWtype = Rand(n, 10402) % 47;
            c.pedSize = 2 + v % 3; c.objSize = 1 + v % 2; c.vehSize = 1 + (v / 2) % 2; c.coop = 1; c.tgt = v % 2; c.cur = v % 3 == 2 ? 1 : (v % 3);
            openPeds(); calm();
            for (unsigned i = 0; i < 12; ++i) c.evalSet[i] = (i + v) % 3 == 0;
            c.fire = 1 + v % 3; c.hasGroup = (v / 2) % 2;
            if (v % 2) c.ped[0].f = 0;
        } else if (fam == 17) {   // extremos de coma flotante en posiciones, camara, multiplicadores y alcance
            c.pedSize = 1 + j % kPeds; c.objSize = (j / 5) % (kObjs + 1); c.vehSize = (j / 3) % (kVehs + 1); c.coop = 1; openPeds();
            c.cx = Coord(n, 10410, 40); c.cy = Coord(n, 10411, 40); c.camX = Coord(n, 10412, 40); c.camY = Coord(n, 10413, 40); c.cam9AC = Coord(n, 10414, 40); c.cam9B0 = Coord(n, 10415, 40);
            c.fx = Coord(n, 10416, 40); c.fy = Coord(n, 10417, 40); c.rng = Range(n, 10418); c.cur = j % 2;
            for (unsigned i = 0; i < kPeds; ++i) { c.ped[i].x = Coord(n, 10420 + i, 40); c.ped[i].y = Coord(n, 10430 + i, 40); c.ped[i].z = Coord(n, 10440 + i, 40); }
            for (unsigned i = 0; i < kObjs; ++i) { c.obj[i].x = Coord(n, 10450 + i, 40); c.obj[i].y = Coord(n, 10460 + i, 40); c.obj[i].z = Coord(n, 10470 + i, 40); }
            for (unsigned i = 0; i < 12; ++i) { c.mult[i] = Pick(n, 10480 + i, {kInfBits, kNanBits, 0u, 0x80000000u, 0x3F800000u, 0x42C80000u, Range(n, 10492 + i)}); c.evalSet[i] = Rand(n, 10500 + i) % 2; }
            c.fire = 1;
        } else if (fam == 18) {   // el booleano de la pila con basura en los bits altos, la ranura y el tipo del arma
            c.arg1 = Pick(n, 10510, {0u, 1u, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000000u, 0x12345600u, 0x12345601u, 0x45u, 0xFFu}) ;
            c.slot = u32(i32(j % 16) - 3) & 0xFFu; c.wtype = (j / 16) % 47; c.cur = (j / 32) % 3; c.pedSize = 1 + (j / 7) % kPeds; c.objSize = (j / 11) % (kObjs + 1); c.vehSize = (j / 13) % (kVehs + 1); c.coop = j % 2;
            openPeds(); for (unsigned i = 0; i < 12; ++i) c.evalSet[i] = Rand(n, 10520 + i) % 2; c.fire = 1 + (j / 3) % 4; calm();
        } else {   // 19: tamanos de los pools, huecos libres y matrices
            c.pedSize = j % (kPeds + 1); c.objSize = (j / 6) % (kObjs + 1); c.vehSize = (j / 24) % (kVehs + 1); c.coop = (j / 3) % 2; c.camMt = (j / 5) % 2; c.cmt = (j / 7) % 2; c.cur = (j / 2) % 3;
            for (unsigned i = 0; i < kPeds; ++i) { c.ped[i].pr = (j / (4 + i)) % 3 != 0; c.ped[i].mt = (j / (3 + i)) % 2; }
            for (unsigned i = 0; i < kObjs; ++i) { c.obj[i].pr = (j / (5 + i)) % 3 != 0; c.obj[i].mt = (j / (2 + i)) % 2; }
            for (unsigned i = 0; i < kVehs; ++i) { c.veh[i].pr = (j / (6 + i)) % 3 != 0; c.veh[i].mt = (j / (4 + i)) % 2; }
            calm();
        }
    }
    c.mutKind &= 15;
    return c;
}

inline std::string JsonCase(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("fx", c.fx).U("fy", c.fy).U("slot", c.slot).U("wtype", c.wtype).U("tgt", c.tgt).U("rng", c.rng).U("skill", c.skill).U("two_player", c.twoPlayer);
    j.U("cur", c.cur).U("cx", c.cx).U("cy", c.cy).U("cz", c.cz).U("cmt", c.cmt).U("arg1", c.arg1);
    j.U("cam_mt", c.camMt).U("cam_x", c.camX).U("cam_y", c.camY).U("cam_z", c.camZ).U("cam9ac", c.cam9AC).U("cam9b0", c.cam9B0);
    j.U("fire", c.fire).U("has_group", c.hasGroup).U("coop", c.coop).U("same_after", c.sameAfter);
    for (unsigned i = 0; i < 12; ++i) {
        const std::string s = std::to_string(i);
        j.U("atan" + s, c.atan[i]).U("mult" + s, c.mult[i]).U("eval_set" + s, c.evalSet[i]).U("eval_prio" + s, c.evalPrio[i]).U("reach" + s, c.reach[i]);
    }
    auto ent = [&](const char* pfx, unsigned i, const EntC& e) {
        const std::string p = std::string(pfx) + std::to_string(i) + "_";
        j.U(p + "pr", e.pr).U(p + "x", e.x).U(p + "y", e.y).U(p + "z", e.z).U(p + "mt", e.mt);
        j.U(p + "a", e.a).U(p + "b", e.b).U(p + "c", e.c).U(p + "d", e.d).U(p + "e", e.e).U(p + "f", e.f).U(p + "g", e.g).U(p + "t", e.t);
    };
    for (unsigned i = 0; i < kPeds; ++i) ent("ped", i, c.ped[i]);
    for (unsigned i = 0; i < kVehs; ++i) ent("veh", i, c.veh[i]);
    for (unsigned i = 0; i < kObjs; ++i) ent("obj", i, c.obj[i]);
    j.U("ped_size", c.pedSize).U("veh_size", c.vehSize).U("obj_size", c.objSize);
    j.U("mut_site", c.mutSite).U("mut_kind", c.mutKind).U("mut_range", c.mutRange).U("mut_slot", c.mutSlot).U("mut_wtype", c.mutWtype);
    return j.End();
}
}
