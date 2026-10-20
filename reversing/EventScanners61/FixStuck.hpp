// ------------------------------------------------------------------------------------------------ CPedStuckChecker::TestPedStuck (0x602C00)
// Incluido dentro de namespace EventScanners61Fix desde Fixtures.hpp.
constexpr unsigned kCasesStuck = 4096;   // 16 familias x 256
constexpr u32 kKcOff = 0x20, kKcBytes = 0x10, kKcArena = 0x50;             // CPedStuckChecker (this): 0x20 + 0x10 + 0x20
constexpr u32 kKpedOff = 0x40, kKpedBytes = 0x580, kKpedArena = 0x600;     // peaton: 0x40 + 0x580 + 0x40
constexpr u32 kKmatOff = 0x20, kKmatBytes = 0x48, kKmatArena = 0x88;       // matriz del peaton: 0x20 + 0x48 + 0x20
constexpr u32 kKentOff = 0x20, kKentBytes = 0x40, kKentArena = 0x80;       // entidades (dano / colisionada): 0x20 + 0x40 + 0x20
constexpr u32 kKwatOff = 0x20, kKwatBytes = 0x10, kKwatArena = 0x50;       // CEventInWater falso: 0x20 + 0x10 + 0x20
constexpr u32 kKgrpOff = 0x20, kKgrpBytes = 0x20, kKgrpArena = 0x60;       // grupo de eventos (solo su direccion): 0x20 + 0x20 + 0x20
constexpr u32 kKintOff = 0x20, kKintBytes = 0x20, kKintArena = 0x60;       // inteligencia (solo su direccion): 0x20 + 0x20 + 0x20
// desplazamientos del EXE
constexpr u32 kEntFlagsAt = 0x1C, kEntSeedAt = 0x20, kEntTypeAt = 0x36, kPhysAttachedAt = 0xFC, kPedStateAt = 0x530, kPedFlags2At = 0x470, kPhysDmgIntAt = 0xD8, kPhysDmgEntAt = 0xDC,
              kPhysImpactZAt = 0xE8, kPhysNumCollAt = 0xB9, kPhysCollEntAt = 0xBC, kPedCurRotAt = 0x558, kPedAimRotAt = 0x55C;
constexpr u32 kFrameGlobal = 0xB7CB4C, kStepGlobal = 0xB7CB5C;

struct K {
    u32 family;
    u32 entFlags, attachedNull, pedState, flags46C, flags470, dmgKind, dmgBits, impactZBits, numColl, collKind, seed, pedMatNull;
    u32 pos[3], placeOther[3], lastPt[3];
    u32 isPlayer, swim, jet, climb, radius, state;
    u32 tsBits, frame;
    u32 hit[3], cp[3][3], los[2], waterKind, accelBits, angleBits, limitedBits;
    u32 eax, edx, stack;
};

inline constexpr float kTsValues[]{0.0f, 0.00001f, 0.001f, 0.01f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f, 5.0f, 100.0f, -1.0f};
inline constexpr u32 kTsSpecial[]{0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x80000000u, 0xBF800000u};
inline constexpr u32 kRadiusPool[]{0u, 1u, 2u, 3u, 12u, 13u, 14u, 25u, 26u, 27u, 50u, 100u, 200u, 5000u, 9999u, 10000u, 10001u, 20000u, 65534u, 65535u};
inline constexpr u32 kPedStates[]{0u, 1u, 2u, 10u, 53u, 54u, 55u, 56u, 77u, 0xFFFFFFFFu};

// Familias (n % 16, 256 casos):
//  0 reinicio: sin colision / sujeto a algo / estado 54 o 55 (DIE/DEAD)            1 de pie o estaba de pie
//  2 dano de una entidad con intensidad > 0 contra jugador / atasco (IsPlayer, bIsStuck)  3 nadando / jetpack / escalando
//  4 contador: 0, 1, 10000, 10001, 65535... (se reinicia el punto o se incrementa)      5 umbral 1: contador > 50/(4*paso) y contador*0.01 > |V|^2 (paso en bordes y especiales)
//  6 colisiona con mas de una entidad: contador > 50/(2*paso) y contador*0.004 > |V.z| 7 colisiona con una entidad edificio (una, nula, otro tipo)
//  8 sonda vertical: cada 8 fotogramas (id del peaton + contador)                        9 sonda vertical: golpea / no, bandera de cabeza, dano de peaton, se pone de pie
//  10 choque lateral: alturas f14 / f10 (5000, 5120, > 5000) y LOS                        11 choque lateral: lado 1 / 2 / -1 / 0, movimiento, fuerza, angulo
//  12 estado final: eventos (CEventInWater con aceleracion en los bordes de 1.0)           13..15 mezcla al azar
inline K MakeK(u32 n) {
    K k{};
    const u32 f = n % 16;
    k.family = f;
    k.eax = Rand(n, 70); k.edx = Rand(n, 71);
    k.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    // base: peaton "bueno" que llega a las sondas
    k.entFlags = 0x11u | (Rand(n, 1) & 0xFFFFFFEEu & ~0x100u);   // bUsesCollision + bIsStuck + ruido
    k.attachedNull = 1;
    k.pedState = 1;
    k.flags46C = Rand(n, 2) & ~3u;                  // de pie / estaba de pie = 0
    k.flags470 = Rand(n, 3) & ~0x200000u;           // sin bHeadStuckInCollision
    k.dmgKind = 0; k.dmgBits = F(RandF(n, 4, 0.0f, 100.0f)); k.impactZBits = F(RandF(n, 5, -2.0f, 2.0f));
    k.numColl = Rand(n, 6) % 4; k.collKind = 1; k.seed = Rand(n, 7) & 0xFF; k.pedMatNull = Rand(n, 8) % 3 == 0;
    const float px = RandF(n, 9, -300.0f, 300.0f), py = RandF(n, 10, -300.0f, 300.0f), pz = RandF(n, 11, -20.0f, 60.0f);
    k.pos[0] = F(px); k.pos[1] = F(py); k.pos[2] = F(pz);
    for (int i = 0; i < 3; ++i) { k.placeOther[i] = Rand(n, 12 + i); }
    k.isPlayer = 1; k.swim = k.jet = k.climb = 0;
    k.radius = 100; k.state = Rand(n, 15) % 3 ? 0u : (Rand(n, 16) % 4);
    k.tsBits = F(1.0f);
    k.frame = Rand(n, 17);
    k.hit[0] = k.hit[1] = k.hit[2] = 0;
    for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) k.cp[i][j] = Rand(n, 20 + 3 * i + j);
    for (int i = 0; i < 3; ++i) k.cp[i][2] = F(RandF(n, 30 + i, pz - 10.0f, pz + 10.0f));
    k.los[0] = k.los[1] = 0;
    k.waterKind = 0; k.accelBits = F(RandF(n, 40, 0.0f, 3.0f)); k.angleBits = F(RandF(n, 41, -7.0f, 7.0f)); k.limitedBits = F(RandF(n, 42, -3.2f, 3.2f));
    const auto setLast = [&](float vx, float vy, float vz) { k.lastPt[0] = F(px - vx); k.lastPt[1] = F(py - vy); k.lastPt[2] = F(pz - vz); };
    setLast(RandF(n, 43, -0.3f, 0.3f), RandF(n, 44, -0.3f, 0.3f), RandF(n, 45, -0.05f, 0.05f));
    const auto bump = [&](u32 salt) { return Rand(n, salt); };
    const auto pickTs = [&](u32 salt) {
        const u32 r = bump(salt) % 16;
        if (r == 15) k.tsBits = kTsSpecial[bump(salt + 1) % std::size(kTsSpecial)];
        else if (r >= 12) k.tsBits = F(kTsValues[bump(salt + 2) % std::size(kTsValues)]);
        else k.tsBits = F(1.0f);   // el caso normal
    };
    pickTs(50);
    switch (f) {
    case 0: {
        const u32 w = Rand(n, 53) % 4;
        if (w == 0) k.entFlags &= ~1u; else if (w == 1) k.attachedNull = 0; else if (w == 2) k.pedState = 54u + Rand(n, 54) % 2; else { k.pedState = kPedStates[Rand(n, 55) % std::size(kPedStates)]; if (Rand(n, 56) % 2) k.entFlags &= ~1u; }
        k.radius = kRadiusPool[Rand(n, 57) % std::size(kRadiusPool)]; k.state = Rand(n, 58) % 3;
        break;
    }
    case 1: k.flags46C |= 1u << (Rand(n, 53) % 2); if (Rand(n, 54) % 4 == 0) k.flags46C |= 3u; k.radius = kRadiusPool[Rand(n, 57) % std::size(kRadiusPool)]; break;
    case 2: {
        k.dmgKind = Rand(n, 53) % 3; k.dmgBits = Rand(n, 54) % 3 == 0 ? (Rand(n, 55) % 2 ? 0u : 0x80000000u) : (Rand(n, 56) % 8 == 0 ? 0x7FC00000u : F(RandF(n, 57, -2.0f, 100.0f)));
        k.isPlayer = Rand(n, 58) % 2 ? 1u : (Rand(n, 59) % 2 ? 0u : 0x100u);
        if (Rand(n, 60) % 2) k.entFlags &= ~0x10u;
        k.swim = Rand(n, 61) % 6 == 0; k.radius = kRadiusPool[Rand(n, 62) % std::size(kRadiusPool)];
        break;
    }
    case 3: k.swim = Rand(n, 53) % 2; k.jet = Rand(n, 54) % 2; k.climb = Rand(n, 55) % 2; if (!(k.swim | k.jet | k.climb)) k.climb = 1; k.dmgKind = Rand(n, 56) % 3; break;
    case 4: k.radius = kRadiusPool[Rand(n, 53) % std::size(kRadiusPool)]; break;
    case 5: {   // umbral 1
        k.radius = 14u + Rand(n, 53) % 300;
        if (Rand(n, 54) % 3 == 0) k.radius = kRadiusPool[Rand(n, 55) % std::size(kRadiusPool)];
        const float s = FromBits(k.tsBits) != FromBits(k.tsBits) ? 1.0f : (0.01f > FromBits(k.tsBits) ? 0.01f : FromBits(k.tsBits));
        const float lim = 50.0f / (4.0f * s);
        const float cnt = float(std::min<u32>(k.radius, 65534u) + 1u);
        if (Rand(n, 56) % 3 == 0) k.radius = u32(std::max(lim - 2.0f + float(Rand(n, 57) % 5), 0.0f));   // alrededor del borde
        const float c01 = cnt * 0.01f;
        const float target = (Rand(n, 58) % 2) ? c01 * (1.0f + float(int(Rand(n, 59) % 5) - 2) * 1e-6f) : c01 * RandF(n, 60, 0.0f, 2.0f);   // |V|^2 alrededor de contador*0.01
        const float len = std::sqrt(std::max(target, 0.0f));
        const float a1 = RandF(n, 61, 0.0f, 6.2831853f), a2 = RandF(n, 62, 0.0f, 3.1415926f);
        setLast(len * std::sin(a2) * std::cos(a1), len * std::sin(a2) * std::sin(a1), len * std::cos(a2));
        k.numColl = Rand(n, 63) % 3;
        break;
    }
    case 6: {   // mas de una entidad colisionada
        k.numColl = 2 + Rand(n, 53) % 4;
        k.radius = 26u + Rand(n, 54) % 400;
        if (Rand(n, 55) % 3 == 0) k.radius = kRadiusPool[Rand(n, 56) % std::size(kRadiusPool)];
        const float cnt = float(std::min<u32>(k.radius, 65534u) + 1u);
        const float vz = (Rand(n, 57) % 2) ? cnt * 0.004f * (1.0f + float(int(Rand(n, 58) % 5) - 2) * 1e-6f) : RandF(n, 59, -2.0f, 2.0f);
        setLast(RandF(n, 60, -50.0f, 50.0f), RandF(n, 61, -50.0f, 50.0f), Rand(n, 62) % 2 ? vz : -vz);
        break;
    }
    case 7: {   // una entidad colisionada
        k.numColl = Rand(n, 53) % 8 == 0 ? Rand(n, 54) % 2 : 1u;
        k.collKind = Rand(n, 55) % 5 == 0 ? Rand(n, 56) % 3 : 1u;
        k.radius = 26u + Rand(n, 57) % 400;
        const float cnt = float(k.radius + 1u);
        const float vz = (Rand(n, 58) % 2) ? cnt * 0.004f * (1.0f + float(int(Rand(n, 59) % 5) - 2) * 1e-6f) : RandF(n, 60, -2.0f, 2.0f);
        setLast(RandF(n, 61, -50.0f, 50.0f), RandF(n, 62, -50.0f, 50.0f), Rand(n, 63) % 2 ? vz : -vz);
        k.hit[0] = Rand(n, 64) % 2; k.hit[1] = Rand(n, 65) % 2; k.los[0] = Rand(n, 66) % 2; k.los[1] = Rand(n, 67) % 2;
        break;
    }
    case 8: k.radius = 14u + Rand(n, 53) % 300; setLast(RandF(n, 54, -0.05f, 0.05f), RandF(n, 55, -0.05f, 0.05f), 0.0f); k.tsBits = F(1.0f);
            k.frame = Rand(n, 56) % 3 ? (Rand(n, 57) & ~7u) + ((8u - (k.seed + 3u) % 8u) % 8u) : Rand(n, 58); k.hit[0] = Rand(n, 59) % 2; break;
    case 9: k.radius = 14u + Rand(n, 53) % 300; setLast(RandF(n, 54, -0.05f, 0.05f), RandF(n, 55, -0.05f, 0.05f), 0.0f); k.tsBits = F(1.0f);
            k.frame = (Rand(n, 56) & ~7u) + ((8u - (k.seed + 3u) % 8u) % 8u); k.hit[0] = Rand(n, 57) % 5 != 0;
            if (Rand(n, 58) % 3 == 0) k.flags470 |= 0x200000u;
            k.dmgKind = Rand(n, 59) % 3; k.dmgBits = Rand(n, 60) % 3 ? F(RandF(n, 61, 0.1f, 50.0f)) : 0u; k.impactZBits = F(Rand(n, 62) % 2 ? RandF(n, 63, -1.0f, 0.3f) : RandF(n, 64, 0.3000001f, 3.0f));
            if (Rand(n, 65) % 4 == 0) k.impactZBits = F(0.3f);
            { const float cz = (Rand(n, 66) % 3 == 0) ? pz - 1.0f + RandF(n, 67, -0.0001f, 0.0001f) : RandF(n, 68, pz - 6.0f, pz + 6.0f); k.cp[0][2] = F(cz); }
            if (Rand(n, 69) % 6 == 0) k.dmgKind = 1;
            break;
    case 10: case 11: {
        k.numColl = 1; k.collKind = 1; k.radius = 26u + Rand(n, 53) % 400; k.tsBits = F(1.0f);
        const float cnt = float(k.radius + 1u);
        setLast(RandF(n, 54, -30.0f, 30.0f), RandF(n, 55, -30.0f, 30.0f), cnt * 0.004f * RandF(n, 56, -0.9f, 0.9f));
        k.hit[0] = Rand(n, 57) % 4 != 0; k.hit[1] = Rand(n, 58) % 4 != 0; k.los[0] = Rand(n, 59) % 4 != 0; k.los[1] = Rand(n, 60) % 4 != 0;
        if (f == 10) {
            const float c1 = Rand(n, 61) % 5 == 0 ? RandF(n, 62, 4990.0f, 5100.0f) : RandF(n, 63, pz - 5.0f, pz + 5.0f);
            const float c2 = Rand(n, 64) % 5 == 0 ? RandF(n, 65, 4990.0f, 5100.0f) : RandF(n, 66, pz - 5.0f, pz + 5.0f);
            k.cp[0][2] = F(c1); k.cp[1][2] = F(c2);
            if (Rand(n, 67) % 5 == 0) { k.cp[0][2] = F(5000.0f); }
            if (Rand(n, 68) % 6 == 0) { k.cp[1][2] = F(5001.0f); }
            if (Rand(n, 69) % 9 == 0) { k.cp[0][2] = F(5000.5f); k.cp[1][2] = F(5200.0f); k.hit[0] = k.hit[1] = k.los[0] = k.los[1] = 1; }
        } else {
            // alturas relativas a la del peaton: lado 1, lado 2 o ninguno
            const u32 w = Rand(n, 70) % 4;
            const float below = pz - 1.0f - RandF(n, 71, 0.01f, 5.0f), above = pz - 1.0f + RandF(n, 72, 0.0f, 5.0f);
            k.hit[0] = k.hit[1] = k.los[0] = k.los[1] = 1;
            if (w == 0) { k.cp[0][2] = F(pz - 2.0f + RandF(n, 73, 0.0f, 0.99f)); k.cp[1][2] = F(RandF(n, 74, pz - 20.0f, pz + 20.0f)); }       // lado 1: P.z-1 < f14
            else if (w == 1) { k.cp[0][2] = F(below); k.cp[1][2] = F(above < 5001.0f ? above : pz); }                                        // lado 2
            else if (w == 2) { k.cp[0][2] = F(above); k.cp[1][2] = F(above + RandF(n, 75, 0.0f, 1.0f)); }
            else { k.cp[0][2] = F(RandF(n, 76, pz - 3.0f, pz + 3.0f)); k.cp[1][2] = F(RandF(n, 77, pz - 3.0f, pz + 3.0f)); }
            if (Rand(n, 78) % 6 == 0) { k.cp[0][2] = F(5000.0f + RandF(n, 79, 0.1f, 100.0f)); k.cp[1][2] = F(5000.0f + RandF(n, 80, 0.1f, 100.0f)); }
            if (Rand(n, 81) % 3 == 0) k.flags470 |= 0x200000u;
            k.dmgKind = Rand(n, 82) % 3; k.dmgBits = Rand(n, 83) % 3 ? F(RandF(n, 84, 0.1f, 50.0f)) : 0u; k.impactZBits = F(Rand(n, 85) % 2 ? RandF(n, 86, -1.0f, 0.3f) : RandF(n, 87, 0.3000001f, 3.0f));
        }
        break;
    }
    case 12: {
        k.state = 1u + Rand(n, 53) % 2; k.radius = Rand(n, 54) % 2 ? 50u : 3u; k.numColl = 0; k.frame = 1; k.seed = 0; k.tsBits = F(1.0f);
        setLast(5.0f, 5.0f, 5.0f);   // |V|^2 enorme: sin sonda; contador = 4 o 51 -> no supera 50/(4*1)
        k.radius = Rand(n, 55) % 2 ? 3u : 1u;
        k.waterKind = Rand(n, 56) % 3 != 0;
        { const u32 w = Rand(n, 57) % 7; k.accelBits = w == 0 ? F(1.0f) : w == 1 ? F(1.0000001f) : w == 2 ? F(0.99999994f) : w == 3 ? 0x7FC00000u : w == 4 ? F(-1.0f) : w == 5 ? F(RandF(n, 58, 1.1f, 50.0f)) : F(RandF(n, 59, 0.0f, 1.0f)); }
        k.flags46C |= 0u;
        break;
    }
    default: {   // 13..15: mezcla
        if (Rand(n, 53) % 6 == 0) { const u32 w = Rand(n, 54) % 4; if (w == 0) k.entFlags &= ~1u; else if (w == 1) k.attachedNull = 0; else if (w == 2) k.pedState = 54u + Rand(n, 55) % 2; else k.flags46C |= 1u; }
        k.dmgKind = Rand(n, 56) % 3; k.isPlayer = Rand(n, 57) % 4 != 0; if (Rand(n, 58) % 5 == 0) k.entFlags &= ~0x10u; k.swim = Rand(n, 59) % 12 == 0;
        k.radius = Rand(n, 60) % 2 ? 14u + Rand(n, 61) % 300 : kRadiusPool[Rand(n, 62) % std::size(kRadiusPool)];
        k.numColl = Rand(n, 63) % 4; k.collKind = Rand(n, 64) % 3;
        setLast(RandF(n, 65, -2.0f, 2.0f), RandF(n, 66, -2.0f, 2.0f), RandF(n, 67, -0.5f, 0.5f));
        k.frame = (Rand(n, 68) & ~7u) + ((8u - (k.seed + 3u) % 8u) % 8u);
        k.hit[0] = Rand(n, 69) % 2; k.hit[1] = Rand(n, 70) % 2; k.hit[2] = Rand(n, 71) % 2; k.los[0] = Rand(n, 72) % 2; k.los[1] = Rand(n, 73) % 2;
        k.waterKind = Rand(n, 74) % 4 == 0; if (Rand(n, 75) % 5 == 0) k.flags470 |= 0x200000u;
        if (Rand(n, 76) % 5 == 0) { k.cp[0][2] = F(5000.0f + RandF(n, 77, 0.0f, 5.0f)); k.cp[1][2] = F(RandF(n, 78, 4999.0f, 5300.0f)); }
        if (Rand(n, 79) % 20 == 0) { const u32 w = Rand(n, 80) % 3; k.pos[w] = SpecialBits(n, 81); }
        if (Rand(n, 82) % 20 == 0) { k.lastPt[Rand(n, 83) % 3] = SpecialBits(n, 84); }
        break;
    }
    }
    // los EAX de los bool devueltos por los falsos (AL, a veces con basura en el resto)
    for (int i = 0; i < 3; ++i) if (k.hit[i]) k.hit[i] = 1u | (Rand(n, 90 + i) % 3 == 0 ? (Rand(n, 93 + i) & 0xFFFFFF00u) : 0u); else if (Rand(n, 96 + i) % 4 == 0) k.hit[i] = Rand(n, 99 + i) & 0xFFFFFF00u;
    for (int i = 0; i < 2; ++i) if (k.los[i]) k.los[i] = 1u | (Rand(n, 100 + i) % 3 == 0 ? (Rand(n, 103 + i) & 0xFFFFFF00u) : 0u); else if (Rand(n, 106 + i) % 4 == 0) k.los[i] = Rand(n, 109 + i) & 0xFFFFFF00u;
    return k;
}
