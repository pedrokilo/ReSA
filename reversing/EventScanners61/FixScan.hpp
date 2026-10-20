// ------------------------------------------------------------------------------------------------ CEventScanner::ScanForEvents (0x607E30)
// Incluido dentro de namespace EventScanners61Fix desde Fixtures.hpp.
constexpr unsigned kCasesScan = 4096;   // 16 familias x 256
constexpr u32 kEsOff = 0x20, kEsBytes = 0xD4, kEsArena = 0x114;            // CEventScanner (this): 0x20 + 0xD4 + 0x20
constexpr u32 kSpedOff = 0x40, kSpedBytes = 0x740, kSpedArena = 0x7C0;     // peaton: 0x40 + 0x740 + 0x40
constexpr u32 kSintOff = 0x40, kSintBytes = 0x180, kSintArena = 0x1C0;     // inteligencia: 0x40 + 0x180 + 0x40
constexpr u32 kSentOff = 0x20, kSentBytes = 0x40, kSentArena = 0x80;       // entidad (contacto): 0x20 + 0x40 + 0x20
constexpr u32 kStgtOff = 0x40, kStgtBytes = 0x5A0, kStgtArena = 0x620;     // peaton objetivo de la tarea de matar: 0x40 + 0x5A0 + 0x40
constexpr u32 kSktOff = 0x20, kSktBytes = 0x20, kSktArena = 0x60;          // tarea de matar: 0x20 + 0x20 + 0x20
constexpr u32 kSvehOff = 0x40, kSvehBytes = 0x5A0, kSvehArena = 0x620;     // vehiculo: 0x40 + 0x5A0 + 0x40
constexpr u32 kSplOff = 0x40, kSplBytes = 0x5A0, kSplArena = 0x620;        // jugador (FindPlayerPed): 0x40 + 0x5A0 + 0x40
constexpr u32 kSfireOff = 0x20, kSfireBytes = 0x28, kSfireArena = 0x68;    // fuego: 0x20 + 0x28 + 0x20
constexpr u32 kStaskOff = 0x20, kStaskBytes = 0x20, kStaskArena = 0x60;    // tarea activa (vtable falsa): 0x20 + 0x20 + 0x20
constexpr u32 kSevOff = 0x20, kSevBytes = 0x20, kSevArena = 0x60;          // evento actual del historial (vtable falsa): 0x20 + 0x20 + 0x20
// desplazamientos del EXE
constexpr u32 kPedFireAt = 0x730, kPedContactAt = 0x584, kEntModelAt = 0x22, kEntAreaAt = 0x2F, kVehTypeAt2 = 0x590, kKillTargetAt = 0x10, kFireStartedAt = 0x14, kIntPedClosestAt = 0x170;
constexpr u32 kEscalatorG1 = 0x8CD674, kEscalatorG2 = 0x8CD678, kDeadTimerGlobal = 0xC0B038;

struct S {
    u32 family;
    u32 now, nextScan, deadTimer;
    u32 flags;                                           // dword +0x46C del peaton
    u32 isInAir, hasCur, curType;                        // IsInAir (EAX crudo), historial: hay evento actual y su tipo
    u32 hasFire, creatorKind, isPlayer, hasTask, abort;
    u32 find[4], targetNull, tgtContactNull, tgtArea, pedContactNull, pedArea, modelIndex, g1, g2;
    u32 vehNull, vehType, plContactIsVeh;
    u32 healthBits, isAlive, hasEvent, closestKind;
    u32 eax, edx, stack;
};

inline constexpr u32 kEscModels[]{0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, 0x1234u, 0xFF00u, 0x00FFu, 0x8001u, 0x0100u};
inline constexpr u32 kAreas[]{0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0xFEu, 13u, 0x81u};
inline constexpr u32 kCurTypes[]{33u, 33u, 34u, 32u, 0u, 9u, 58u, 0xFFFFFFFFu, 0x21u + 0x100u};

inline u32 RawBool(u32 n, u32 k, bool value) {   // EAX de un bool devuelto: AL (cero o no) y, a veces, basura en los 24 bits altos
    u32 al = value ? (Rand(n, k) % 3 == 0 ? (Rand(n, k + 1) | 1u) & 0xFFu : 1u) : 0u;
    if (value && al == 0) al = 1;
    const u32 hi = Rand(n, k + 2) % 3 == 0 ? (Rand(n, k + 3) & 0xFFFFFF00u) : 0u;
    return hi | al;
}

// Familias (n % 16, 256 casos):
//  0 hora <= m_nNextScanTime (borde: igual, una menos, una mas): no escanea             1 escanea sin eventos (de pie, sin fuego, sin objetivo ...)
//  2 en el aire (no de pie, bandera puesta): CEventInAir                                 3 no de pie sin bandera: IsInAir (AL, con basura) decide el evento
//  4 de pie con la bandera de aire: el evento actual del historial (nulo, tipo 33 u otro) la baja o no
//  5 en llamas: IsPlayer, tarea activa, MakeAbortable (AL, con basura) -> evento o GenerateDamageEvent
//  6 busqueda de la tarea de matar (llamada 1..4 o ninguna) y objetivo nulo / no                7 codigos de area de los contactos (iguales / distintos / nulos, bytes con signo)
//  8 escalera mecanica: indice de modelo con signo contra los dos globales (0xFFFF, 0x8000...)   9 en un barco: vehiculo, tipo, IsPlayer, contacto del jugador
//  10 peaton muerto en pie: salud (<= 0, -0, NaN, > 0), IsAlive, hora contra el temporizador (bordes y vuelta de 32 bits), HasEventOfType
//  11..15 mezcla al azar de todo (con varias condiciones a la vez)
inline S MakeS(u32 n) {
    S s{};
    const u32 f = n % 16;
    s.family = f;
    s.eax = Rand(n, 70); s.edx = Rand(n, 71);
    s.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    s.now = Rand(n, 1);
    if (Rand(n, 2) % 8 == 0) s.now = 0xFFFFF000u + Rand(n, 3) % 0x1000u;   // cerca de la vuelta de 32 bits
    s.nextScan = s.now - 1u - (Rand(n, 4) % 3 ? Rand(n, 5) % 5000u : 0u);   // por defecto: escanea (now > nextScan)
    s.deadTimer = s.now - 1u - (Rand(n, 6) % 3 ? Rand(n, 7) % 5000u : 0u);  // por defecto: vencido
    s.flags = Rand(n, 8);
    s.flags &= ~(1u | 0x100u | 0x200u);                                      // de pie, en vehiculo, en el aire: se fijan abajo
    s.isInAir = RawBool(n, 9, Rand(n, 12) % 2); s.hasCur = Rand(n, 13) % 3 != 0; s.curType = kCurTypes[Rand(n, 14) % std::size(kCurTypes)];
    s.hasFire = 0; s.creatorKind = Rand(n, 15) % 3; s.isPlayer = RawBool(n, 16, Rand(n, 19) % 2); s.hasTask = Rand(n, 20) % 4 != 0; s.abort = RawBool(n, 21, Rand(n, 24) % 2);
    for (int i = 0; i < 4; ++i) s.find[i] = 0;
    s.targetNull = Rand(n, 25) % 5 == 0; s.tgtContactNull = Rand(n, 26) % 4 == 0; s.tgtArea = kAreas[Rand(n, 27) % std::size(kAreas)];
    s.pedContactNull = Rand(n, 28) % 4 == 0; s.pedArea = Rand(n, 29) % 2 ? s.tgtArea : kAreas[Rand(n, 30) % std::size(kAreas)];
    s.modelIndex = kEscModels[Rand(n, 31) % std::size(kEscModels)];
    s.g1 = Rand(n, 32) % 3 ? kEscModels[Rand(n, 33) % std::size(kEscModels)] : (Rand(n, 34) & 0xFFFFu);
    s.g2 = Rand(n, 35) % 3 ? kEscModels[Rand(n, 36) % std::size(kEscModels)] : (Rand(n, 37) & 0xFFFFu);
    s.vehNull = Rand(n, 38) % 5 == 0; s.vehType = Rand(n, 39) % 2 ? 5u : (Rand(n, 40) % 12); s.plContactIsVeh = Rand(n, 41) % 2;
    s.healthBits = F(RandF(n, 42, 1.0f, 100.0f)); s.isAlive = RawBool(n, 43, Rand(n, 46) % 3 != 0); s.hasEvent = RawBool(n, 47, Rand(n, 50) % 2); s.closestKind = Rand(n, 51) % 3;
    // posibles semillas de las banderas
    const auto setStanding = [&](bool v) { s.flags = v ? (s.flags | 1u) : (s.flags & ~1u); };
    const auto setAir = [&](bool v) { s.flags = v ? (s.flags | 0x200u) : (s.flags & ~0x200u); };
    const auto setInVeh = [&](bool v) { s.flags = v ? (s.flags | 0x100u) : (s.flags & ~0x100u); };
    constexpr float kHealths[]{0.0f, -0.0f, -1.0f, -100.0f, 1e-30f, -1e-30f, 0.5f, 1.0f, 100.0f, 1000.0f};
    constexpr u32 kHealthBits[]{0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x80000001u, 0x00000001u};
    switch (f) {
    case 0: {   // no vence el temporizador de escaneo: now <= nextScan
        const u32 k = Rand(n, 52) % 4;
        s.nextScan = k == 0 ? s.now : k == 1 ? s.now + 1u : k == 2 ? s.now + Rand(n, 53) : 0xFFFFFFFFu;
        setStanding(Rand(n, 54) % 2); setAir(Rand(n, 55) % 2); s.hasFire = Rand(n, 56) % 2;
        s.healthBits = F(kHealths[Rand(n, 57) % std::size(kHealths)]);
        break;
    }
    case 1: setStanding(true); setAir(false); break;
    case 2: setStanding(false); setAir(true); break;
    case 3: setStanding(false); setAir(false); s.isInAir = RawBool(n, 9, Rand(n, 12) % 2); break;
    case 4: setStanding(true); setAir(true); break;
    case 5: setStanding(Rand(n, 58) % 2); setAir(false); s.hasFire = 1; s.hasTask = Rand(n, 20) % 5 != 0; break;
    case 6: setStanding(true); setAir(false); { const u32 k = Rand(n, 59) % 5; if (k < 4) s.find[k] = 1; if (Rand(n, 60) % 4 == 0) { for (int i = 0; i < 4; ++i) s.find[i] = Rand(n, 61 + i) % 2; } } break;
    case 7: setStanding(true); setAir(false); s.find[Rand(n, 59) % 4] = 1; s.targetNull = Rand(n, 65) % 6 == 0; if (Rand(n, 66) % 2) s.pedArea = s.tgtArea; break;
    case 8: setStanding(true); setAir(false); s.pedContactNull = Rand(n, 28) % 6 == 0;
            if (Rand(n, 67) % 2) s.g1 = s.modelIndex; else if (Rand(n, 68) % 2) s.g2 = s.modelIndex; if (Rand(n, 69) % 5 == 0) { s.g1 = 0xFFFFu; s.modelIndex = 0xFFFFu; } break;
    case 9: setStanding(true); setAir(false); setInVeh(Rand(n, 72) % 5 != 0); s.vehType = Rand(n, 73) % 3 ? 5u : (Rand(n, 74) % 12); s.plContactIsVeh = Rand(n, 75) % 3 != 0; break;
    case 10: setStanding(true); setAir(false); s.healthBits = Rand(n, 76) % 4 == 0 ? kHealthBits[Rand(n, 77) % std::size(kHealthBits)] : F(kHealths[Rand(n, 78) % std::size(kHealths)]);
             { const u32 k = Rand(n, 79) % 5; s.deadTimer = k == 0 ? s.now : k == 1 ? s.now - 1u : k == 2 ? s.now + 1u : k == 3 ? s.now - Rand(n, 80) % 100000u : Rand(n, 81); } break;
    default: {
        setStanding(Rand(n, 82) % 2); setAir(Rand(n, 83) % 2); setInVeh(Rand(n, 84) % 3 == 0); s.hasFire = Rand(n, 85) % 3 == 0;
        for (int i = 0; i < 4; ++i) s.find[i] = Rand(n, 86 + i) % 5 == 0;
        s.healthBits = Rand(n, 90) % 3 ? s.healthBits : F(kHealths[Rand(n, 91) % std::size(kHealths)]);
        if (Rand(n, 92) % 2) { const u32 k = Rand(n, 93) % 3; s.deadTimer = k == 0 ? s.now : k == 1 ? s.now - 1u : Rand(n, 94); }
        if (Rand(n, 95) % 4 == 0) { s.g1 = s.modelIndex; }
        if (Rand(n, 96) % 5 == 0) { s.nextScan = s.now + Rand(n, 97) % 3; }
        break;
    }
    }
    if (f == 5 || f == 6 || f == 7 || f == 8 || f == 9 || f == 10) { /* sin condiciones de aire ni fuego ajenas a la familia */ }
    if (f != 5 && f != 0 && f < 11) s.hasFire = 0;
    if (f != 10 && f != 0 && f < 11) s.healthBits = F(RandF(n, 42, 1.0f, 100.0f));   // vivo: sin evento de ahogamiento
    if (f != 9 && f < 11) setInVeh(false);
    if (f != 6 && f != 7 && f != 0 && f < 11) for (int i = 0; i < 4; ++i) s.find[i] = 0;
    if (f != 8 && f < 11 && f != 0) { s.g1 = (s.modelIndex + 1u) & 0xFFFFu; s.g2 = (s.modelIndex + 2u) & 0xFFFFu; }
    return s;
}
