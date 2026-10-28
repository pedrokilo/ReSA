#pragma once
// Fixtures de la funcion 1 del banco PlayerPed79: CPlayerPed::FindWeaponLockOnTarget (EXE 0x60DC50, thiscall sin argumentos, `ret`, devuelve bool en AL).
// Todos los floats viajan como patrones de 32 bits. Solo se usa un patron de NaN (0xFFC00000, el "indefinido" de la x87): asi la seleccion de NaN de SSE y de la x87 no puede diferir.
#include "Fixtures.hpp"

namespace Pp79Fix {
constexpr unsigned kCases1 = 20480;       // 16 familias de 1024 (n % 16) + 4 de 1024 (16..19)
constexpr unsigned kPeds = 5, kVehs = 3, kObjs = 3, kBlds = 4, kBldList = 6;
constexpr unsigned kSites1 = 18;

constexpr std::size_t kPedWin = 0x800, kVehWin = 0xA20, kObjWin = 0x1A0, kBldWin = 0x40, kMatWin = 0x40, kInfoWin = 0x40, kHdrWin = 0x20, kFlagsWin = 0x10;
constexpr std::size_t kPedsWin = kPeds * 0x7C4, kVehsWin = kVehs * 0xA18, kObjsWin = kObjs * 0x19C, kBldsWin = kBlds * 0x40;

struct EntC {                   // una entidad del pool: posicion, matriz y atributos
    u32 pr{}, x{}, y{}, z{}, mt{};
    u32 a{}, b{}, c{}, d{}, e{}, f{};   // atributos segun el tipo (ver MakeC1)
};
struct C1 {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    // jugador
    u32 px{}, py{}, pz{}, pmt{}, fx{}, fy{}, fz{}, hd{};   // posicion, matriz (presente), vector hacia delante, rumbo de m_placement
    u32 slot{}, wtype{}, ptype{}, tgt{}, rng{}, rngInfo2{}, skill{};
    u32 tx{}, ty{}, tz{}, tmt{};
    u32 cam{}, twoPlayer{};
    // tablas de los falsos
    std::array<u32, 12> atan{}, mult{}, mag{}, evalSet{}, evalPrio{};
    std::array<u32, 3> norm{};
    u32 between{}, limitR{}, lr1{}, ud1{}, lr2{}, ud2{}, los{}, coop{};
    std::array<u32, 12> reach{};
    u32 bldCount{};
    std::array<u32, 15> bldList{};
    std::array<EntC, kPeds> ped{};
    std::array<EntC, kVehs> veh{};
    std::array<EntC, kObjs> obj{};
    std::array<EntC, kBlds> bld{};
    u32 pedSize{}, vehSize{}, objSize{};
    // mutaciones
    u32 mutSite{}, mutKind{}, mutRange{}, mutSlot{}, mutWtype{};
};

inline u32 Pick(u32 n, u32 k, std::initializer_list<u32> v) { return *(v.begin() + Rand(n, k) % v.size()); }
inline constexpr u32 kNanBits = 0xFFC00000u, kInfBits = 0x7F800000u, kNInfBits = 0xFF800000u;
// Coordenadas / magnitudes: normales (con tres decimales), ceros, grandes, y de vez en cuando infinitos y NaN
inline u32 Coord(u32 n, u32 k, u32 specials = 20) {
    const u32 r = Rand(n, k) % 100;
    if (r < specials / 4) return Pick(n, k + 1, {kInfBits, kNInfBits});
    if (r < specials / 2) return kNanBits;
    if (r < specials) return Pick(n, k + 2, {0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x461C4000u /*10000*/, 0xC61C4000u, 0x49742400u /*1e6*/, 0x358637BDu /*1e-6*/});
    return B(float(i32(Rand(n, k + 3) % 200001) - 100000) / 1000.0f);
}
inline u32 Angle(u32 n, u32 k) {   // resultados de los falsos de atan: moderados (los bucles de normalizacion del EXE no terminan con valores enormes)
    const u32 r = Rand(n, k) % 100;
    if (r < 6) return Pick(n, k + 1, {0u, 0x80000000u, 0x40490FDBu /*pi*/, 0xC0490FDBu, 0x40C90FDBu /*2pi*/, 0xC0C90FDBu, 0x3FC90FDBu, 0xBFC90FDBu, 0x3F000000u, 0x40490FDAu, 0x40490FDCu, 0xC0490FDAu, 0xC0490FDCu});
    if (r < 10) return kNanBits;
    return B(float(i32(Rand(n, k + 2) % 40001) - 20000) / 2000.0f);   // -10 .. 10 rad
}
inline u32 Range(u32 n, u32 k) {
    const u32 r = Rand(n, k) % 100;
    if (r < 3) return Pick(n, k + 1, {kInfBits, kNanBits, 0u, 0x80000000u});
    return B(float(Rand(n, k + 2) % 20001) / 100.0f);   // 0 .. 200
}
inline u32 Stick(u32 n, u32 k) {   // retorno de GetPedWalk*: int16 en AX con basura en la mitad alta de EAX
    const u32 r = Rand(n, k) % 10;
    i32 v;
    if (r < 3) v = i32(Rand(n, k + 1) % 121) - 60;
    else if (r == 3) v = Pick(n, k + 2, {60u, u32(-60), 61u, u32(-61), 59u, u32(-59), 0u, 32767u, u32(-32768), u32(-32767)});
    else v = i32(Rand(n, k + 3) % 256) - 128;
    return (u32(v) & 0xFFFFu) | (Rand(n, k + 4) % 3 == 0 ? Rand(n, k + 5) << 16 : 0u);
}
inline void FillEnt(EntC& e, u32 n, u32 k, bool nanOk) {
    e.pr = Rand(n, k) % 5 != 0;
    e.x = Coord(n, k + 1, nanOk ? 12 : 3); e.y = Coord(n, k + 2, nanOk ? 12 : 3); e.z = Coord(n, k + 3, nanOk ? 12 : 3);
    e.mt = Rand(n, k + 4) % 2;
}

inline C1 BaseDefaults1(unsigned n) {
    C1 c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7000);
    c.eax = SeedReg(n, 7001); c.edx = SeedReg(n, 7002); c.stack = StackSeed(n);
    c.px = Coord(n, 7010, 6); c.py = Coord(n, 7011, 6); c.pz = Coord(n, 7012, 6); c.pmt = Rand(n, 7013) % 2;
    c.fx = Coord(n, 7014, 6); c.fy = Coord(n, 7015, 6); c.fz = Coord(n, 7016, 6); c.hd = Angle(n, 7017);
    c.slot = Rand(n, 7018) % 5 == 0 ? u32(i32(Rand(n, 7019) % 13) - 3) : Rand(n, 7019) % 13; c.slot &= 0xFFu;
    c.wtype = Rand(n, 7020) % 4 == 0 ? 41u : (Rand(n, 7021) % 47);
    c.ptype = Rand(n, 7022) % 2;
    c.tgt = 0;
    c.rng = Range(n, 7023); c.rngInfo2 = Range(n, 7024); c.skill = Rand(n, 7025) % 4;
    c.tx = Coord(n, 7026, 6); c.ty = Coord(n, 7027, 6); c.tz = Coord(n, 7028, 6); c.tmt = Rand(n, 7029) % 2;
    c.cam = Angle(n, 7030); c.twoPlayer = Rand(n, 7031) % 3 == 0;
    for (unsigned i = 0; i < c.atan.size(); ++i) { c.atan[i] = Angle(n, 7040 + i); c.mult[i] = Range(n, 7060 + i); c.mag[i] = Range(n, 7080 + i); c.evalSet[i] = Rand(n, 7100 + i) % 3 == 0; c.evalPrio[i] = Angle(n, 7120 + i); c.reach[i] = Rand(n, 7140 + i) % 3 != 0; }
    for (unsigned i = 0; i < 3; ++i) c.norm[i] = Coord(n, 7160 + i, 4);
    c.between = Angle(n, 7170); c.limitR = Angle(n, 7171);
    c.lr1 = Stick(n, 7172); c.ud1 = Stick(n, 7176); c.lr2 = Stick(n, 7180); c.ud2 = Stick(n, 7184);
    c.los = Rand(n, 7188) % 3 == 0; c.coop = Rand(n, 7189) % 2;
    c.bldCount = Rand(n, 7190) % 7;
    for (unsigned i = 0; i < 15; ++i) c.bldList[i] = Rand(n, 7200 + i) % kBlds;
    for (unsigned i = 0; i < kPeds; ++i) {
        EntC& e = c.ped[i]; FillEnt(e, n, 7300 + i * 16, true);
        e.a = Rand(n, 7300 + i * 16 + 5) % 6 == 0 ? Pick(n, 7300 + i * 16 + 6, {0x36u, 0x37u, 0x35u, 0x38u}) : Rand(n, 7300 + i * 16 + 7) % 54;   // estado
        e.b = Rand(n, 7300 + i * 16 + 8) % 6 == 0;                                                                                                   // en vehiculo
        e.c = Rand(n, 7300 + i * 16 + 9) % 5;                                                                                                       // 0: sin vehiculo, 1..3: vehiculo i-1, 4: nulo
        e.d = Rand(n, 7300 + i * 16 + 10) % 8 == 0;                                                                                                  // nunca objetivo
        e.e = Rand(n, 7300 + i * 16 + 11) % 3 ? Rand(n, 7300 + i * 16 + 12) % 2 : Pick(n, 7300 + i * 16 + 13, {2u, 3u, 8u, 0x100u});               // tipo
        e.f = Rand(n, 7300 + i * 16 + 14) % 6 == 0;                                                                                                  // mismo grupo
    }
    for (unsigned i = 0; i < kVehs; ++i) {
        EntC& e = c.veh[i]; FillEnt(e, n, 7500 + i * 16, true);
        e.a = Rand(n, 7500 + i * 16 + 5) % 4 == 0 ? 0x20000000u | Rand(n, 7500 + i * 16 + 6) : Rand(n, 7500 + i * 16 + 7) & ~0x20000000u;   // +0x40
        e.b = Rand(n, 7500 + i * 16 + 8) % 5 == 0 ? 10u : Rand(n, 7500 + i * 16 + 9) % 10;                                                       // subtipo +0x594
        e.c = Rand(n, 7500 + i * 16 + 10) % 3 == 0 ? 9u : Rand(n, 7500 + i * 16 + 11) % 9;                                                       // tipo +0x590
        e.d = Rand(n, 7500 + i * 16 + 12) & 0xFFu;                                                                                              // byte +0x42D
    }
    for (unsigned i = 0; i < kObjs; ++i) {
        EntC& e = c.obj[i]; FillEnt(e, n, 7700 + i * 16, true);
        e.a = Rand(n, 7700 + i * 16 + 5) % 3 != 0;                    // CanBeTargetted
        e.b = Rand(n, 7700 + i * 16 + 6) & 0xFFu;                     // byte +0x140
        e.c = Rand(n, 7700 + i * 16 + 7) % 4 != 0;                    // objeto RW
    }
    for (unsigned i = 0; i < kBlds; ++i) {
        EntC& e = c.bld[i]; FillEnt(e, n, 7900 + i * 16, false);
        e.pr = 1;
        e.a = Rand(n, 7900 + i * 16 + 5) % 4 != 0;                    // IsTag
        e.b = Rand(n, 7900 + i * 16 + 6) % 4 == 0 ? Pick(n, 7900 + i * 16 + 7, {254u, 255u, 0u, 1u}) : Rand(n, 7900 + i * 16 + 8) & 0xFFu;   // alfa
    }
    c.pedSize = Rand(n, 8000) % (kPeds + 1); c.vehSize = Rand(n, 8001) % (kVehs + 1); c.objSize = Rand(n, 8002) % (kObjs + 1);
    return c;
}

inline C1 MakeC1(unsigned n) {
    C1 c = BaseDefaults1(n);
    unsigned fam = c.family;
    if (n >= kBaseCases) fam = 16 + (n - kBaseCases) / 1024;
    c.family = fam;
    const bool coordsSane = true; (void)coordsSane;
    auto calm = [&] {   // finitos y moderados en todo el escenario
        auto fix = [&](u32& v, u32 k) { if (!std::isfinite(F(v)) || std::fabs(F(v)) > 1.0e5f) v = B(float(i32(Rand(n, k) % 20001) - 10000) / 100.0f); };
        fix(c.px, 8100); fix(c.py, 8101); fix(c.pz, 8102); fix(c.fx, 8103); fix(c.fy, 8104); fix(c.fz, 8105); fix(c.tx, 8106); fix(c.ty, 8107); fix(c.tz, 8108);
        for (unsigned i = 0; i < kPeds; ++i) { fix(c.ped[i].x, 8110 + i * 3); fix(c.ped[i].y, 8111 + i * 3); fix(c.ped[i].z, 8112 + i * 3); }
        for (unsigned i = 0; i < kVehs; ++i) { fix(c.veh[i].x, 8130 + i * 3); fix(c.veh[i].y, 8131 + i * 3); fix(c.veh[i].z, 8132 + i * 3); }
        for (unsigned i = 0; i < kObjs; ++i) { fix(c.obj[i].x, 8140 + i * 3); fix(c.obj[i].y, 8141 + i * 3); fix(c.obj[i].z, 8142 + i * 3); }
        for (unsigned i = 0; i < kBlds; ++i) { fix(c.bld[i].x, 8150 + i * 3); fix(c.bld[i].y, 8151 + i * 3); fix(c.bld[i].z, 8152 + i * 3); }
        for (auto& v : c.atan) if (!std::isfinite(F(v))) v = 0x3F000000u;
    };
    auto noWorld = [&] { c.pedSize = c.vehSize = c.objSize = 0; c.bldCount = 0; };
    auto nonSpray = [&] { if (c.wtype == 41) c.wtype = 30; };
    auto nearPeds = [&] {   // peatones cerca del jugador y delante
        for (unsigned i = 0; i < kPeds; ++i) {
            c.ped[i].x = B(float(i32(Rand(n, 8200 + i) % 2001) - 1000) / 100.0f); c.ped[i].y = B(float(i32(Rand(n, 8210 + i) % 2001) - 1000) / 100.0f); c.ped[i].z = B(float(i32(Rand(n, 8220 + i) % 401) - 200) / 100.0f);
            c.ped[i].pr = 1;
        }
    };
    auto stickOff = [&] { c.lr1 = c.ud1 = 0; };
    switch (fam) {
    case 0: c.tgt = 1; c.wtype &= ~0u; break;                                               // objetivo ya fijado, dentro o fuera de alcance
    case 1: c.tgt = 1; c.mult[0] = Range(n, 8300); c.rng = Range(n, 8301); break;
    case 2: c.tgt = 1; c.mult[0] = Pick(n, 8302, {kInfBits, kNanBits, 0u, 0x3F800000u, 0x80000000u}); c.rng = Pick(n, 8303, {kInfBits, kNanBits, 0u, 0x42C80000u}); c.tx = Coord(n, 8304, 30); c.ty = Coord(n, 8305, 30); c.px = Coord(n, 8306, 30); break;
    case 3: c.tgt = 0; noWorld(); nonSpray(); stickOff(); c.ptype = Rand(n, 8307) % 2; break;     // nada que evaluar -> false
    case 4: nonSpray(); c.pedSize = 1 + Rand(n, 8308) % kPeds; c.vehSize = c.objSize = 0; c.bldCount = 0; nearPeds(); calm(); break;
    case 5: nonSpray(); c.pedSize = 1 + Rand(n, 8309) % kPeds; for (unsigned i = 0; i < kPeds; ++i) { c.ped[i].b = Rand(n, 8310 + i) % 2; c.ped[i].c = Rand(n, 8320 + i) % 5; c.ped[i].a = Rand(n, 8330 + i) % 54; } nearPeds(); c.vehSize = c.objSize = 0; c.bldCount = 0; calm(); break;
    case 6: nonSpray(); c.pedSize = 1 + Rand(n, 8340) % kPeds; nearPeds(); c.vehSize = c.objSize = 0; c.bldCount = 0; for (auto& v : c.atan) v = Pick(n, 8341, {0x40490FDBu, 0xC0490FDBu, 0x40C90FDBu, 0x3F800000u, 0x4048F5C3u, 0xC048F5C3u, 0x3E800000u, 0x40000000u, 0x00000000u, 0x41200000u}); calm(); break;
    case 7: nonSpray(); c.pedSize = Rand(n, 8342) % (kPeds + 1); nearPeds(); c.vehSize = c.objSize = 0; c.bldCount = 0; calm(); break;
    case 8: nonSpray(); c.ptype = Rand(n, 8343) % 2; c.twoPlayer = Rand(n, 8344) % 2; c.pedSize = 1 + Rand(n, 8345) % kPeds; nearPeds(); c.vehSize = c.objSize = 0; c.bldCount = 0; calm(); break;
    case 9: nonSpray(); c.objSize = 1 + Rand(n, 8346) % kObjs; c.pedSize = c.vehSize = 0; c.bldCount = 0; calm(); break;
    case 10: nonSpray(); c.vehSize = 1 + Rand(n, 8347) % kVehs; c.pedSize = c.objSize = 0; c.bldCount = 0; c.coop = Rand(n, 8348) % 4 != 0; calm(); break;
    case 11: c.wtype = 41; c.bldCount = 1 + Rand(n, 8349) % kBldList; c.pedSize = c.vehSize = c.objSize = 0; c.pmt = 1; c.rng = B(float(10 + Rand(n, 8350) % 190)); c.los = Rand(n, 8351) % 4 == 0; calm();
            for (unsigned i = 0; i < kBlds; ++i) { c.bld[i].a = 1; c.bld[i].b = Rand(n, 8352 + i) % 3 ? Rand(n, 8356 + i) % 255 : 255; c.bld[i].x = B(F(c.px) + float(i32(Rand(n, 8360 + i) % 401) - 200) / 10.0f); c.bld[i].y = B(F(c.py) + float(i32(Rand(n, 8364 + i) % 401) - 200) / 10.0f); c.bld[i].z = B(F(c.pz) + float(i32(Rand(n, 8368 + i) % 41) - 20) / 10.0f); } break;
    case 12: c.wtype = 41; c.bldCount = 1 + Rand(n, 8372) % kBldList; c.pedSize = c.vehSize = c.objSize = 0; c.pmt = 1; c.los = 1; c.tgt = 0; calm(); break;
    case 13: nonSpray(); c.pedSize = 1 + Rand(n, 8373) % kPeds; c.objSize = Rand(n, 8374) % (kObjs + 1); c.vehSize = Rand(n, 8375) % (kVehs + 1); nearPeds(); c.bldCount = 0; for (auto& v : c.evalSet) v = Rand(n, 8376) % 2; calm(); break;
    case 14: c.mutSite = 1 + Rand(n, 8377) % kSites1; c.mutKind = 1 + Rand(n, 8378) % 15; c.mutRange = Range(n, 8379); c.mutSlot = Rand(n, 8380) % 13; c.mutWtype = Rand(n, 8381) % 2 ? 41u : Rand(n, 8382) % 47;
            c.tgt = Rand(n, 8383) % 3 == 0; c.pedSize = 1 + Rand(n, 8384) % kPeds; c.objSize = Rand(n, 8385) % (kObjs + 1); c.vehSize = Rand(n, 8386) % (kVehs + 1); nearPeds(); c.bldCount = Rand(n, 8387) % 4; calm(); break;
    default: if (Rand(n, 8388) % 2) calm(); break;
    }
    if (fam >= 16) {
        const unsigned j = (n - kBaseCases) % 1024;
        if (fam == 16) {   // barrido del bote de spray: etiquetas, alfa, alcance, rumbo
            c.wtype = 41; c.tgt = 0; c.pmt = 1; c.pedSize = c.vehSize = c.objSize = 0; c.bldCount = 1 + j % kBldList; c.los = (j / 6) % 4 == 0;
            calm();
            c.rng = B(float(5 + (j * 7) % 120));
            for (unsigned i = 0; i < kBlds; ++i) { c.bld[i].a = (j + i) % 5 != 0; c.bld[i].b = (j * 3 + i * 17) % 7 == 0 ? 255 : (j * 31 + i * 13) % 255;
                c.bld[i].x = B(F(c.px) + float(i32((j * 13 + i * 71) % 301) - 150) / 10.0f); c.bld[i].y = B(F(c.py) + float(i32((j * 29 + i * 53) % 301) - 150) / 10.0f); c.bld[i].z = B(F(c.pz) + float(i32((j * 7 + i * 3) % 41) - 20) / 10.0f); c.bld[i].mt = (j + i) % 2; }
            if (j % 11 == 0) { c.fx = kNanBits; }
            if (j % 13 == 0) { c.fx = Coord(n, 8400, 0); c.fy = Coord(n, 8401, 0); }
        } else if (fam == 17) {   // extremos de coma flotante en el objetivo fijado y en los peatones
            c.tgt = j % 2; c.pedSize = (j / 2) % (kPeds + 1); c.objSize = c.vehSize = 0; c.bldCount = 0; if (c.wtype == 41) c.wtype = 12;
            c.tx = Coord(n, 8402, 40); c.ty = Coord(n, 8403, 40); c.px = Coord(n, 8404, 40); c.py = Coord(n, 8405, 40); c.mult[0] = Range(n, 8406); c.rng = Range(n, 8407);
            for (unsigned i = 0; i < kPeds; ++i) { c.ped[i].x = Coord(n, 8410 + i, 40); c.ped[i].y = Coord(n, 8420 + i, 40); c.ped[i].z = Coord(n, 8430 + i, 40); c.ped[i].pr = 1; }
            c.fx = Coord(n, 8440, 30); c.fy = Coord(n, 8441, 30); c.fz = Coord(n, 8442, 30);
            for (auto& v : c.atan) if (!std::isfinite(F(v))) v = 0x3F000000u;   // el bucle de normalizacion no termina con infinitos
        } else if (fam == 18) {   // barrido de valores del stick
            if (c.wtype == 41) c.wtype = 12; c.tgt = 0; c.pedSize = (j / 16) % (kPeds + 1); c.objSize = c.vehSize = 0; c.bldCount = 0; nearPeds(); calm();
            static constexpr u32 sv[]{0u, 1u, 59u, 60u, 61u, 0xFFFFFFC4u /*-60*/, 0xFFFFFFC3u, 0xFFFFFFC5u, 0x7FFFu, 0x8000u, 0xFFFF8000u, 0x12348000u, 0x1234FFFFu, 0xABCD003Cu, 0xABCD003Du, 0xFFFF003Cu};
            c.lr1 = sv[j % 16]; c.ud1 = sv[(j / 16) % 16]; c.lr2 = sv[(j / 7) % 16]; c.ud2 = sv[(j / 5) % 16];
        } else {   // 19: ranuras de arma, tipos de peaton / jugador y pools de tamanos distintos
            c.slot = u32(i32(j % 16) - 3) & 0xFFu; c.wtype = (j / 16) % 2 ? 41u : (j / 4) % 47; c.ptype = (j / 8) % 2; c.tgt = (j / 32) % 2 ? 1u : 0u;
            c.pedSize = (j / 3) % (kPeds + 1); c.objSize = (j / 5) % (kObjs + 1); c.vehSize = (j / 7) % (kVehs + 1); c.bldCount = (j / 11) % 7; c.pmt = 1; c.coop = j % 2;
            calm();
        }
    }
    // el vector hacia delante y la matriz del jugador: sin matriz la funcion sin objetivo se cae (el EXE lee la matriz sin comprobarla)
    if (!c.tgt || (c.mutKind & 1)) c.pmt = 1;   // la mutacion de bit 1 puede quitar el objetivo en mitad de la funcion
    if (c.ptype > 1) c.ptype = 0;
    return c;
}

inline std::string JsonC1(const C1& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("px", c.px).U("py", c.py).U("pz", c.pz).U("pmt", c.pmt).U("fx", c.fx).U("fy", c.fy).U("fz", c.fz).U("hd", c.hd);
    j.U("slot", c.slot).U("wtype", c.wtype).U("ptype", c.ptype).U("tgt", c.tgt).U("rng", c.rng).U("rng_info2", c.rngInfo2).U("skill", c.skill);
    j.U("tx", c.tx).U("ty", c.ty).U("tz", c.tz).U("tmt", c.tmt).U("cam", c.cam).U("two_player", c.twoPlayer);
    for (unsigned i = 0; i < 12; ++i) {
        const std::string s = std::to_string(i);
        j.U(("atan" + s).c_str(), c.atan[i]).U(("mult" + s).c_str(), c.mult[i]).U(("mag" + s).c_str(), c.mag[i]).U(("eval_set" + s).c_str(), c.evalSet[i]).U(("eval_prio" + s).c_str(), c.evalPrio[i]).U(("reach" + s).c_str(), c.reach[i]);
    }
    j.U("norm0", c.norm[0]).U("norm1", c.norm[1]).U("norm2", c.norm[2]).U("between", c.between).U("limit_r", c.limitR);
    j.U("lr1", c.lr1).U("ud1", c.ud1).U("lr2", c.lr2).U("ud2", c.ud2).U("los", c.los).U("coop", c.coop).U("bld_count", c.bldCount);
    for (unsigned i = 0; i < 15; ++i) j.U(("bld_list" + std::to_string(i)).c_str(), c.bldList[i]);
    auto ent = [&](const char* pfx, unsigned i, const EntC& e) {
        const std::string p = std::string(pfx) + std::to_string(i) + "_";
        j.U((p + "pr").c_str(), e.pr).U((p + "x").c_str(), e.x).U((p + "y").c_str(), e.y).U((p + "z").c_str(), e.z).U((p + "mt").c_str(), e.mt);
        j.U((p + "a").c_str(), e.a).U((p + "b").c_str(), e.b).U((p + "c").c_str(), e.c).U((p + "d").c_str(), e.d).U((p + "e").c_str(), e.e).U((p + "f").c_str(), e.f);
    };
    for (unsigned i = 0; i < kPeds; ++i) ent("ped", i, c.ped[i]);
    for (unsigned i = 0; i < kVehs; ++i) ent("veh", i, c.veh[i]);
    for (unsigned i = 0; i < kObjs; ++i) ent("obj", i, c.obj[i]);
    for (unsigned i = 0; i < kBlds; ++i) ent("bld", i, c.bld[i]);
    j.U("ped_size", c.pedSize).U("veh_size", c.vehSize).U("obj_size", c.objSize);
    j.U("mut_site", c.mutSite).U("mut_kind", c.mutKind).U("mut_range", c.mutRange).U("mut_slot", c.mutSlot).U("mut_wtype", c.mutWtype);
    return j.End();
}
}
