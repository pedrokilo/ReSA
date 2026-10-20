// ------------------------------------------------------------------------------------------------ CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents
// Incluido dentro de namespace EventScanners61Fix desde Fixtures.hpp.
constexpr unsigned kCasesVpc = 4096;   // 16 familias x 256
constexpr u32 kVsOff = 0x20, kVsBytes = 0xC, kVsArena = 0x4C;              // escaner (this): 0x20 + 0xC + 0x20
constexpr u32 kVpedOff = 0x40, kVpedBytes = 0x500, kVpedArena = 0x580;     // peaton: 0x40 + 0x500 + 0x40
constexpr u32 kVmatOff = 0x20, kVmatBytes = 0x48, kVmatArena = 0x88;       // matriz (CMatrix): 0x20 + 0x48 + 0x20
constexpr u32 kVvehOff = 0x40, kVvehBytes = 0x5A0, kVvehArena = 0x620;     // vehiculo: 0x40 + 0x5A0 + 0x40
constexpr u32 kVintOff = 0x40, kVintBytes = 0x130, kVintArena = 0x1B0;     // inteligencia: 0x40 + 0x130 + 0x40
constexpr u32 kVtaskOff = 0x20, kVtaskBytes = 0x20, kVtaskArena = 0x60;    // tarea GoTo: 0x20 + 0x20 + 0x20
constexpr u32 kVcolOff = 0x20, kVcolBytes = 0x18, kVcolArena = 0x58;       // modelo de colision (caja): 0x20 + 0x18 + 0x20
// desplazamientos del EXE
constexpr u32 kPlacePos = 4, kMatrixAt = 0x14, kMatPos = 0x30, kMatUp = 0x20, kPedFlags3At = 0x474, kVehTypeAt = 0x594, kIntClosestVeh = 0x120, kTaskTargetAt = 0xC;
constexpr u32 kTimeGlobal = 0xB7CB84;

struct V {
    u32 family;
    u32 now, tStart, tInterval, tStarted, tStopped;     // reloj del juego y temporizador (bytes crudos de started/stopped)
    u32 taskNull, isGoto, vehNull, pedMatNull, vehType, flags3;
    u32 vehPos[3], vm[9], mn[3], mx[3], pedPos[3], pedMatPos[3], vehPlace[3], tp[3];   // posiciones y matriz del vehiculo (right, forward, up)
    u32 clear, losWrite, losDist, move;
    u32 eax, edx, stack;
};

inline float RandF(u32 n, u32 k, float lo, float hi) { return lo + (hi - lo) * float(Rand(n, k) & 0xFFFFFF) * (1.0f / 16777216.0f); }
inline constexpr u32 kSpecialBits[]{0x7FC00000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x3F000001u, 0x3EFFFFFFu};
inline u32 SpecialBits(u32 n, u32 k) { return kSpecialBits[Rand(n, k) % std::size(kSpecialBits)]; }
// ejes (right, forward, up) de una rotacion desde tres angulos
inline void RotAxes(u32 n, u32 k, float out[9]) {
    const float yaw = RandF(n, k, -3.14159f, 3.14159f), pitch = RandF(n, k + 1, -1.2f, 1.2f), roll = RandF(n, k + 2, -1.2f, 1.2f);
    const float cy = std::cos(yaw), sy = std::sin(yaw), cp = std::cos(pitch), sp = std::sin(pitch), cr = std::cos(roll), sr = std::sin(roll);
    out[0] = cy * cr + sy * sp * sr; out[1] = sy * cr - cy * sp * sr; out[2] = -cp * sr;
    out[3] = -sy * cp;               out[4] = cy * cp;                out[5] = sp;
    out[6] = cy * sr - sy * sp * cr; out[7] = sy * sr + cy * sp * cr; out[8] = cp * cr;
}

// Familias (n % 16, 256 casos):
//  0  temporizador sin arrancar (se arranca con 500 ms y no vence)        1  temporizador detenido (se reinicia el inicio) y no vence
//  2  sin vencer por 1 ms / intervalos con la suma que da la vuelta        3  vence exacto (now == inicio + intervalo) y poco mas
//  4  suma de 32 bits (inicio + intervalo) que da la vuelta, vencida o no  5  vencido, sin tarea (GetSimplestActiveTask nulo)
//  6  vencido, tarea que no es GoTo                                        7  vencido, tarea GoTo, sin vehiculo cercano
//  8  fuera de la caja por arriba (cond. 1 falla)                          9  fuera de la caja por abajo (cond. 2 falla)
//  10 dentro de la caja, distancia al limite (5 / 10 si es tren / bandera de salida de coche)   11 dentro y a distancia: la linea de vision esta despejada
//  12 dentro, linea no despejada y la distancia devuelta <= 0.5 (o no escrita / NaN)   13 todo cumplido: evento WalkIntoVehicle
//  14 especiales (NaN/inf/-0 en posiciones, matriz y caja)                 15 mezcla al azar
inline V MakeV(u32 n) {
    V v{};
    const u32 f = n % 16;
    v.family = f;
    v.eax = Rand(n, 70); v.edx = Rand(n, 71);
    v.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    v.now = Rand(n, 1);
    constexpr u32 kBoolBytes[]{0, 1, 2, 0x80, 0xFF, 0x7F, 0x10};
    v.tStarted = Rand(n, 2) % 3 ? 1u : kBoolBytes[Rand(n, 3) % std::size(kBoolBytes)];
    v.tStopped = Rand(n, 4) % 3 ? 0u : kBoolBytes[Rand(n, 5) % std::size(kBoolBytes)];
    v.tInterval = Rand(n, 6) % 2 ? 500u : (Rand(n, 7) % 4 ? Rand(n, 8) % 2000u : Rand(n, 9));
    v.tStart = v.now - v.tInterval - (Rand(n, 10) % 4 ? Rand(n, 11) % 3000u : 0u);   // por defecto: vencido (now >= start + interval)
    v.taskNull = 0; v.isGoto = 1; v.vehNull = 0; v.pedMatNull = Rand(n, 12) % 3 == 0; v.vehType = Rand(n, 13) % 12; v.flags3 = Rand(n, 14) & 0xFFu;
    if (v.vehType == 6 && Rand(n, 15) % 2) v.vehType = Rand(n, 16) % 6;
    if (f >= 10 && f <= 13 && Rand(n, 17) % 3 == 0) v.vehType = 6;   // trenes (limite 10)
    v.clear = Rand(n, 17) % 2 ? 0u : (Rand(n, 18) % 3 ? 0u : (Rand(n, 19) & 0xFFFFFF00u));   // AL = 0 (falso); basura en los 24 bits altos a veces
    v.losWrite = Rand(n, 20) % 4 != 0;
    v.losDist = F(RandF(n, 21, 0.0f, 3.0f));
    v.move = Rand(n, 22) % 8;
    // temporizador
    switch (f) {
    case 0: v.tStarted = 0; break;
    case 1: v.tStarted = 1; v.tStopped = kBoolBytes[1 + Rand(n, 24) % 6]; v.tStart = v.now - 100u; v.tInterval = Rand(n, 28) % 2 ? 500u : Rand(n, 29); break;
    case 2: v.tStarted = 1; v.tStopped = 0; v.tStart = v.now - v.tInterval + 1u + (Rand(n, 24) % 3 == 0 ? Rand(n, 25) % 100 : 0u);
            if (Rand(n, 30) % 4 == 0) { v.tInterval = 0xFFFFFFFFu - Rand(n, 31) % 1000u; v.tStart = v.now - v.tInterval + 1u; }
            break;   // now = inicio + intervalo - 1 (+ algo)
    case 3: v.tStarted = 1; v.tStopped = 0; v.tStart = v.now - v.tInterval - (Rand(n, 24) % 2 ? 0u : Rand(n, 25) % 5u); break;   // vence exacto
    case 4: v.tStarted = 1; v.tStopped = 0; v.tStart = 0xFFFFFF00u + Rand(n, 24) % 0x100u; v.tInterval = 0x80u + Rand(n, 25) % 0x400u;
            v.now = (Rand(n, 26) % 2) ? v.tStart + v.tInterval : v.tStart + v.tInterval - 1u - Rand(n, 27) % 4u; break;
    default: v.tStarted = 1; v.tStopped = 0; break;   // vencido y arrancado (el temporizador siempre vence en las familias profundas)
    }
    // hasta donde llega
    switch (f) {
    case 5: v.taskNull = 1; break;
    case 6: v.isGoto = 0; if (Rand(n, 33) % 3 == 0) v.isGoto = 0x100u; break;     // AL = 0 con basura en el resto
    case 7: v.vehNull = 1; break;
    default: break;
    }
    if (f == 15) { v.taskNull = Rand(n, 34) % 8 == 0; v.isGoto = Rand(n, 35) % 6 != 0; v.vehNull = Rand(n, 36) % 8 == 0; }
    if (f == 15 || f == 6) { if (v.isGoto == 1 && Rand(n, 37) % 4 == 0) v.isGoto = (Rand(n, 38) | 0x1u); }
    if (f == 15 && Rand(n, 39) % 6 == 0) { v.tStarted = Rand(n, 3) % 3 ? 1u : kBoolBytes[Rand(n, 5) % std::size(kBoolBytes)]; v.tStopped = kBoolBytes[Rand(n, 4) % std::size(kBoolBytes)]; v.tStart = Rand(n, 10); }
    // geometria: el peaton se coloca en el sistema local del vehiculo
    float axes[9];
    RotAxes(n, 40, axes);
    if (Rand(n, 43) % 8 == 0) { const float ident[9]{1, 0, 0, 0, 1, 0, 0, 0, 1}; for (int i = 0; i < 9; ++i) axes[i] = ident[i]; }   // sin rotacion
    for (int i = 0; i < 9; ++i) v.vm[i] = F(axes[i]);
    const float vx = RandF(n, 44, -3000.0f, 3000.0f), vy = RandF(n, 45, -3000.0f, 3000.0f), vz = RandF(n, 46, -50.0f, 200.0f);
    v.vehPos[0] = F(vx); v.vehPos[1] = F(vy); v.vehPos[2] = F(vz);
    const float hx = RandF(n, 47, 0.6f, 1.3f), hy = RandF(n, 48, 1.5f, 4.0f), hz = RandF(n, 49, 0.4f, 1.2f);
    const float bminz = -hz * RandF(n, 50, 0.6f, 1.2f), bmaxz = hz * RandF(n, 51, 0.8f, 1.6f);
    v.mn[0] = F(-hx); v.mn[1] = F(-hy); v.mn[2] = F(bminz);
    v.mx[0] = F(hx);  v.mx[1] = F(hy);  v.mx[2] = F(bmaxz);
    // coordenadas locales del peaton
    float lx = RandF(n, 52, -hx - 1.0f, hx + 1.0f), ly = RandF(n, 53, -hy - 1.0f, hy + 1.0f);
    float lz = RandF(n, 54, bminz - 0.45f, bmaxz + 0.45f);               // dentro de la losa (min - 0.5, max + 0.5)
    const float limit = v.vehType == 6 ? 10.0f : 5.0f;
    const auto scaleTo = [&](float target) {
        const float len = std::sqrt(lx * lx + ly * ly + lz * lz);
        if (len > 1e-6f) { const float k = target / len; lx *= k; ly *= k; lz *= k; }
    };
    switch (f) {
    case 8: lz = bmaxz + 0.5f + RandF(n, 55, 0.0001f, 5.0f); if (Rand(n, 56) % 4 == 0) lz = bmaxz + 0.5f; break;      // por encima (cond. 1)
    case 9: lz = bminz - 0.5f - RandF(n, 55, 0.0001f, 5.0f); if (Rand(n, 56) % 4 == 0) lz = bminz - 0.5f; break;      // por debajo (cond. 2)
    case 10: {
        // distancia (en el espacio local) alrededor del limite 5 / 10 (tren), con el peaton dentro de la losa
        lz = RandF(n, 55, bminz - 0.45f, bmaxz + 0.45f);
        float tgt = (v.vehType == 6 ? 10.0f : 5.0f) + float(int(Rand(n, 60) % 3) - 1) * RandF(n, 61, 0.0f, 0.02f);
        if (Rand(n, 62) % 4 == 0) tgt = limit;
        if (Rand(n, 63) % 3 == 0) tgt = limit * (1.0f + float(int(Rand(n, 64) % 5) - 2) * 1e-6f);
        if (Rand(n, 65) % 5 == 0) tgt = (v.vehType == 6 ? 5.0f : 10.0f);   // el limite del otro tipo de vehiculo
        const float hh = std::sqrt(std::max(tgt * tgt - lz * lz, 0.0f));
        const float ang = RandF(n, 66, 0.0f, 6.2831853f);
        lx = hh * std::cos(ang); ly = hh * std::sin(ang);
        break;
    }
    case 11: case 12: case 13: case 14: case 15: {
        lx = RandF(n, 57, -1.0f, 1.0f); ly = RandF(n, 58, -1.0f, 1.0f);
        if (f == 15 && Rand(n, 59) % 3 == 0) { lx = RandF(n, 60, -20.0f, 20.0f); ly = RandF(n, 61, -20.0f, 20.0f); lz = RandF(n, 62, bminz - 3.0f, bmaxz + 3.0f); break; }
        const float want = RandF(n, 63, 0.1f, limit * 0.95f);
        lz = RandF(n, 64, bminz - 0.45f, bmaxz + 0.45f);
        const float h = std::sqrt(lx * lx + ly * ly);
        if (h > 1e-6f) { const float room = want * want - lz * lz; const float need = room > 0.0f ? std::sqrt(room) : 0.0f; lx *= need / h; ly *= need / h; }
        break;
    }
    default: break;
    }
    // posicion del peaton en el mundo: P = pos + local (tres ejes) en float
    const float wx = vx + (axes[0] * lx + axes[3] * ly + axes[6] * lz);
    const float wy = vy + (axes[1] * lx + axes[4] * ly + axes[7] * lz);
    const float wz = vz + (axes[2] * lx + axes[5] * ly + axes[8] * lz);
    v.pedPos[0] = F(wx); v.pedPos[1] = F(wy); v.pedPos[2] = F(wz);
    // la posicion que NO se usa (placement si el peaton tiene matriz) es basura
    for (int i = 0; i < 3; ++i) { v.pedMatPos[i] = Rand(n, 80 + i); v.vehPlace[i] = Rand(n, 83 + i); v.tp[i] = Rand(n, 86 + i); }
    // bandera "acaba de dejar un coche" (bit 3 del byte +0x474)
    if (f >= 10 && f <= 13) { v.flags3 = (v.flags3 & ~8u) | (Rand(n, 66) % 4 == 0 ? 8u : 0u); }
    // linea de vision y distancia
    switch (f) {
    case 11: v.clear = 1u | (Rand(n, 67) % 3 == 0 ? (Rand(n, 68) & 0xFFFFFF00u) : 0u); if (Rand(n, 69) % 6 == 0) v.clear = 0x80u; break;   // despejada: no hay evento
    case 12: {
        v.clear = 0u;
        const u32 k = Rand(n, 72) % 6;
        v.losWrite = k != 0;
        v.losDist = k == 1 ? F(0.5f) : k == 2 ? F(0.49999997f) : k == 3 ? 0x7FC00000u : k == 4 ? F(RandF(n, 73, 0.0f, 0.5f)) : F(-RandF(n, 73, 0.0f, 5.0f));
        break;
    }
    case 13: v.clear = (Rand(n, 74) % 4 == 0) ? (Rand(n, 75) & 0xFFFFFF00u) : 0u; v.losWrite = 1; v.losDist = F(0.5f + (Rand(n, 76) % 5 == 0 ? 0.00000006f : RandF(n, 77, 0.01f, 40.0f))); break;
    default: break;
    }
    // especiales: NaN/inf/-0 en posiciones, matriz y caja
    if (f == 14 || (f == 15 && Rand(n, 90) % 4 == 0)) {
        const u32 which = Rand(n, 91) % 7;
        const u32 idx = Rand(n, 92);
        switch (which) {
        case 0: v.pedPos[idx % 3] = SpecialBits(n, 93); break;
        case 1: v.vehPos[idx % 3] = SpecialBits(n, 93); break;
        case 2: v.vm[idx % 9] = SpecialBits(n, 93); break;
        case 3: v.mn[idx % 3] = SpecialBits(n, 93); break;
        case 4: v.mx[idx % 3] = SpecialBits(n, 93); break;
        case 5: v.losDist = SpecialBits(n, 93); v.clear = 0; v.losWrite = 1; break;
        default: v.tp[idx % 3] = SpecialBits(n, 93); break;
        }
    }
    return v;
}
