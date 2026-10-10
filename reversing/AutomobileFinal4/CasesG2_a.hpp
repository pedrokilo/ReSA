#pragma once
// Grupo G2, parte A: ayudantes de fixtures comunes + Constructor, Destructor, SetupSuspensionLines, Fix, SetupDamageAfterLoad,
// FixTyre, FixDoor, FixPanel y SetTotalDamage. Incluido por CasesG2.hpp (despues de la infraestructura y los dobles).
namespace AutomobileFinal4Live {

// ---------------------------------------------------------------------------------------------------------------------
// Ayudantes comunes de fixtures
// ---------------------------------------------------------------------------------------------------------------------
inline float G2NegZero() { return std::bit_cast<float>(0x80000000u); }

// Posicion de un marco: a veces +0.0 o -0.0 (distintos al SUMAR 0.0f), si no un finito razonable
inline float G2Pos(Ctx& c, float range = 40.f) {
    const uint32 k = c.below(24);
    if (k == 0) return 0.f;
    if (k == 1) return G2NegZero();
    return c.uni(-range, range);
}

// Marco falso i: matriz de modelado y LTM con rotacion aleatoria, raiz = el propio marco, sin objetos
inline void G2MakeFrame(Ctx& c, int i, float tilt = 1.0f) {
    auto* f = w.fr[i]; std::memset(f, 0, FR_SIZE);
    double r[9]; c.RandRot(r, tilt);
    const float x = G2Pos(c), y = G2Pos(c), z = G2Pos(c);
    Ctx::SetMatrix(f + 0x10, r, x, y, z);   // RwFrameGetMatrix
    Ctx::SetMatrix(f + 0x50, r, x, y, z);   // LTM
    At<void*>(f, 0xA0) = f;                 // raiz del marco
}

// Estado de la entidad: (estado << 3) | tipo vehiculo (2)
inline void G2SetStatus(uint8* car, uint32 st) { At<uint8>(car, G2_O_STATUS) = uint8((st << 3) | 2u); }

// Pone el bit 'bit' del dword 'off' del handling falso k a 'on'
inline void G2HdBit(int k, size_t off, int bit, bool on) {
    uint32 v = At<uint32>(w.hd[k], off); v = on ? (v | (1u << bit)) : (v & ~(1u << bit)); At<uint32>(w.hd[k], off) = v; }
// Despues de tocar el handling falso: recoge sus banderas en el coche
inline void G2SyncHandling(uint8* car, int k) { At<uint32>(car, G2_O_HFLAGS) = At<uint32>(w.hd[k], 0xD0); }

// m_aCarNodes[0..24]: marco i (siempre w.fr[i]) o nulo (1 de cada nullOneIn; 0 = nunca nulo; los bits de alwaysMask nunca son nulos)
inline void G2FillNodes(Ctx& c, uint8* car, uint32 nullOneIn, uint32 alwaysMask = 0) {
    for (int i = 0; i < 25; ++i) {
        const bool present = ((alwaysMask >> i) & 1u) || !(nullOneIn && c.chance(nullOneIn));
        if (present) { G2MakeFrame(c, i); At<void*>(car, G2_O_NODES + 4 * size_t(i)) = w.fr[i]; }
        else At<void*>(car, G2_O_NODES + 4 * size_t(i)) = nullptr;
    }
}

// Gestor de dano valido (estados 0..4 de puertas y paneles, ruedas 0..4)
inline void G2FillDamage(Ctx& c, uint8* car) {
    auto* d = car + G2_O_DMG;
    At<float>(d, 0) = c.uni(0.f, 1.f);
    At<uint8>(d, 4) = uint8(c.below(251));
    for (size_t k = 0; k < 4; ++k) At<uint8>(d, 5 + k) = uint8(c.below(5));
    for (size_t k = 0; k < 6; ++k) At<uint8>(d, 9 + k) = uint8(c.below(5));
    At<uint8>(d, 0xF) = uint8(c.next());
    At<uint32>(d, 0x10) = c.next() & 0x55u;
    uint32 panels = 0; for (int k = 0; k < 7; ++k) panels |= c.below(5) << (4 * k);
    At<uint32>(d, 0x14) = panels;
}

// Una puerta en 'p' (CDoor de 0x18 bytes): angulos finitos, eje 0..2, estado 0..4
inline void G2FillOneDoor(Ctx& c, uint8* p) {
    At<float>(p, 0x0) = c.chance(6) ? c.pick({1.2566371f, -1.2566371f, 1.5707964f, 0.5f}) : c.uni(-2.f, 2.f);
    At<float>(p, 0x4) = c.chance(3) ? 0.f : c.uni(-0.5f, 0.5f);
    At<uint16>(p, 0x8) = uint16(c.below(6) | uint32(c.pick<int>({0, 0x10, 0x20, 0x40, 0x80, 0x100, 0x30})));
    At<uint8>(p, 0xA) = uint8(c.below(3));
    At<uint8>(p, 0xB) = uint8(c.below(5));
    const uint32 m = c.below(4);
    At<float>(p, 0xC) = m == 0 ? At<float>(p, 0x4) : m == 1 ? At<float>(p, 0x0) : c.uni(-1.6f, 1.6f);
    At<float>(p, 0x10) = c.chance(3) ? 0.f : c.uni(-1.6f, 1.6f);
    At<float>(p, 0x14) = c.chance(3) ? 0.f : c.uni(-1.f, 1.f);
}
// Las 6 puertas m_doors[0..5]
inline void G2FillDoors(Ctx& c, uint8* car) { for (size_t d = 0; d < 6; ++d) G2FillOneDoor(c, car + G2_O_DOORS + 0x18 * d); }
// El chasis oscilante m_swingingChassis (0x70C), detras de los 3 paneles rebotantes
inline void G2FillChassis(Ctx& c, uint8* car) { G2FillOneDoor(c, car + G2_O_CHASSIS); }

// Paneles rebotantes (3): frameId = 0xFFFF, el nodo 'matchNode' o un nodo cualquiera; el resto de campos aleatorios finitos
inline void G2FillPanels(Ctx& c, uint8* car, int matchNode = -1) {
    for (int p = 0; p < 3; ++p) {
        auto* q = car + G2_O_PANELS + 0x20 * size_t(p);
        const uint32 m = c.below(5);
        uint16 id = m <= 1 ? uint16(0xFFFF) : (m == 2 && matchNode >= 0) ? uint16(matchNode) : uint16(c.below(25));
        At<uint16>(q, 0) = id;
        At<uint16>(q, 2) = uint16(c.below(3));
        At<float>(q, 4) = c.uni(-0.6f, 0.6f);
        for (size_t o = 8; o < 0x20; o += 4) At<float>(q, o) = c.uni(-1.f, 1.f);
    }
}

// Bits bajos de m_swingingChassis.m_openAngle = valor (para que el cuarto hueco de 0x20 bytes que recorre el EXE coincida o este libre)
inline void G2ChassisLowWord(uint8* car, uint16 low) {
    uint32 v = At<uint32>(car, G2_O_CHASSIS); v = (v & 0xFFFF0000u) | low; At<uint32>(car, G2_O_CHASSIS) = v; }

// Model info y estructura de vehiculo falsos k: puertas, tamanos de rueda y mascara de componentes danables
inline void G2SetModelInfo(Ctx& c, int k, uint32 damageMask, int numDoors) {
    auto* mi = w.mi[k];
    At<uint8>(mi, 0x4C) = uint8(numDoors);
    At<float>(mi, 0x40) = c.uni(0.2f, 0.9f); At<float>(mi, 0x44) = c.uni(0.2f, 0.9f);
    At<uint32>(w.vs[k], 0x310) = damageMask;
}
// Mascara de componentes danables: casi todos, ninguno o aleatoria
inline uint32 G2DamageMask(Ctx& c) {
    const uint32 m = c.below(8);
    if (m == 0) return 0u; if (m == 1) return 0x01FFFFFFu; if (m == 2) return c.next() & 0x01FFFFFFu;
    return (c.next() | c.next()) & 0x01FFFFFFu | 0x00FFFF00u;
}

// Coche k coherente para este grupo: base + clump del coche con tipo 2 + banderas de estado
inline CAutomobile* G2Car(Ctx& c, int k, int model, float tilt = 0.3f) {
    auto* v = c.BuildBase(k, model, tilt);
    At<void*>(Cr(k), 0x18) = w.clump[0]; w.clump[0][0] = 2;
    return v;
}

// Despues de cada llamada: resumen comun de dos hashes de los buferes propios
inline void G2Finish(Ctx& c) { c.Hash("g2b", &g2b, sizeof(g2b)); }

// ---------------------------------------------------------------------------------------------------------------------
// Constructor (EXE 0x6B0A90, thiscall ret 0xC)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunCtor(Ctx& c) {
    G2Begin();
    static constexpr int kSpecial[]{432, 525, 531, 409, 419, 598, 544, 431, 403, 485, 441, 571};
    const int model = c.i < int(std::size(kSpecial)) * 2 ? kSpecial[c.i % int(std::size(kSpecial))] : c.carModel();
    int hid = int(c.below(210));
    if (c.chance(8)) hid = c.pick({0, 1, 162, 185, 186, 208, 209});
    c.FillMi(0, hid); c.InstallMi(model, 0);
    auto* car = Cr(0); std::memset(car, 0xCD, CAR_SIZE);
    for (int n = 0; n < 25; ++n) { g2Aux[size_t(n)] = c.chance(5) ? 0u : 1u; G2MakeFrame(c, n); }
    if (model == 525) g2Aux[21] = c.chance(4) ? 0u : 1u;
    if (model == 531) g2Aux[17] = c.chance(4) ? 0u : 1u;
    for (size_t n = 40; n < 44; ++n) g2Aux[n] = uint8(c.next());
    g2FirstObj = w.at[0];
    const uint32 createdBy = c.below(5);
    const uint32 setup = c.chance(2) ? 1u : 0u;
    AtA<uint8>(0xC1BFD0) = uint8(c.chance(3) ? 1 : 0);                 // m_sAllTaxiLights
    void* ret{};
    { auto P = MakePatches(G_RNG, 2, G2_CTOR | G2_SUSPSTUB | G2_GETFIRST); ret = CallThis<void*>(c.a, car, model, createdBy, setup); }
    const uint32 vp = At<uint32>(car, 0);
    const int kind = vp == 0x871120u ? 1 : (G2InDll(vp) ? 2 : 0);
    c.out["ret_is_this"] = ret == car;
    c.out["vptr_ok"] = c.candidate ? kind == 2 : kind == 1;     // original: vtable del EXE; candidato: vtable de la DLL
    At<uint32>(car, 0) = 0;                                      // se enmascara el vptr (distinto por diseno)
    c.out["setup"] = setup; c.out["model"] = model;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// Destructor (EXE 0x6A61E0, thiscall)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunDtor(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    auto fx = [&](size_t off) { At<void*>(car, off) = c.chance(2) ? static_cast<void*>(w.misc + 0x20 + 8 * (off & 3)) : nullptr; };
    fx(G2_O_FIRE); fx(G2_O_NITRO); fx(G2_O_NITRO + 4);
    { auto P = MakePatches(0, 2, G2_KILL | G2_DTOR); CallThis<void>(c.a, car); }
    const uint32 vp = At<uint32>(car, 0);
    const int kind = vp == 0x871120u ? 1 : (G2InDll(vp) ? 2 : 0);
    c.out["vptr_ok"] = c.candidate ? kind == 2 : kind == 1;
    At<uint32>(car, 0) = 0;
    c.out["fire"] = Id(At<void*>(car, G2_O_FIRE)); c.out["nitro0"] = Id(At<void*>(car, G2_O_NITRO)); c.out["nitro1"] = Id(At<void*>(car, G2_O_NITRO + 4));
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// SetupSuspensionLines (vtable 0x871120 hueco 48)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSetupSusp(Ctx& c) {
    G2Begin();
    const uint32 sel = c.below(10);
    const int model = sel == 0 ? 432 : sel == 1 ? 441 : sel == 2 ? 571 : c.carModel();
    G2Car(c, 0, model);
    auto* car = Cr(0);
    auto* hd = w.hd[0];
    At<float>(hd, 0xB8) = c.uni(0.05f, 0.5f);          // limite superior de la suspension
    At<float>(hd, 0xBC) = c.uni(-0.5f, -0.02f);        // limite inferior
    At<float>(hd, 0xAC) = c.uni(0.05f, 3.f);           // nivel de fuerza
    G2HdBit(0, 0xCC, 30, c.chance(2));                  // m_bForceGroundClearance
    At<int32>(car, G2_O_VSUB) = c.chance(5) ? 2 : 0;    // subtipo: 2 = quad
    auto* mi = w.mi[0]; At<float>(mi, 0x40) = c.uni(0.2f, 0.8f); At<float>(mi, 0x44) = c.uni(0.2f, 0.8f);
    // modelo de colision y sus datos
    auto* cm = w.colm[0]; auto* cd = w.cdata[0];
    std::memset(cm, 0, COLM_SIZE);
    for (size_t o = 0; o < 3; ++o) { At<float>(cm, 4 * o) = c.uni(-3.f, 0.5f); At<float>(cm, 0xC + 4 * o) = c.uni(-0.5f, 3.f); }
    if (c.chance(3)) At<float>(cm, 8) = c.uni(-0.2f, 1.f);                      // min.z a veces por encima de la linea (sin cambio)
    for (size_t o = 0; o < 3; ++o) At<float>(cm, 0x18 + 4 * o) = c.uni(-1.f, 1.f);
    At<float>(cm, 0x24) = c.chance(3) ? c.uni(0.f, 1.f) : c.uni(2.f, 5.f);     // radio de la esfera
    At<void*>(cm, 0x2C) = cd;
    std::memset(cd, 0, CDATA_SIZE);
    const uint16 nsph = uint16(c.below(9));
    At<uint16>(cd, 0) = nsph; At<void*>(cd, 8) = g2b.sph;
    for (uint16 s = 0; s < 16; ++s) {
        auto* sp = g2b.sph[s];
        At<float>(sp, 0) = c.uni(-1.5f, 1.5f); At<float>(sp, 4) = c.uni(-2.f, 2.f); At<float>(sp, 8) = c.uni(-1.f, 1.f);
        At<float>(sp, 0xC) = c.chance(3) ? c.uni(0.05f, 0.4f) : c.uni(0.4f, 1.3f);
        At<uint32>(sp, 0x10) = c.next();
    }
    const bool haveLines = !c.chance(3);
    At<void*>(cd, 0x10) = haveLines ? static_cast<void*>(w.lines[0]) : nullptr;
    At<uint8>(cd, 6) = uint8(haveLines ? (model == 432 ? 12 : 4) : c.below(13));
    for (size_t o = 0; o < sizeof(w.lines[0]); o += 4) At<float>(w.lines[0], o) = c.uni(-1.f, 1.f);
    // posiciones de rueda que entrega el doble de GetWheelPosn
    for (int k = 0; k < 4; ++k) {
        const float sx = (k < 2) ? -1.f : 1.f;
        g2WheelPos[k][0] = c.chance(10) ? (c.chance(2) ? 0.f : G2NegZero()) : sx * c.uni(0.5f, 1.3f);
        g2WheelPos[k][1] = (k & 1) ? -c.uni(0.8f, 1.7f) : c.uni(0.8f, 1.7f);
        g2WheelPos[k][2] = c.uni(-0.7f, 0.3f);
    }
    { auto P = MakePatches(0, 2, G2_WHEELPOSN | G2_MALLOC); CallThis<void>(c.a, car); }
    c.out["lines_ptr"] = Id(At<void*>(cd, 0x10)); c.out["nlines"] = At<uint8>(cd, 6);
    c.Hash("sph", g2b.sph, sizeof(g2b.sph));
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// Fix (vtable hueco 50)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunFix(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2HdBit(0, 0xCC, 8, c.chance(3));                    // m_bNoDoors (banderas de modelo, bit 8)
    G2SyncHandling(car, 0);
    G2FillNodes(c, car, 6);
    At<uint8>(car, 0x42A) = uint8(c.next());             // m_bIsDamaged es el bit 0
    G2FillDamage(c, car); G2FillDoors(c, car); G2FillChassis(c, car); G2FillPanels(c, car);
    if (c.chance(3)) G2ChassisLowWord(car, uint16(c.below(26)));
    { auto P = MakePatches(0, 2, G2_CLUMPALL); CallThis<void>(c.a, car); }
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// SetupDamageAfterLoad (vtable hueco 51): las tres funciones de dano se sustituyen por dobles trazados
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSetupDamage(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 3);
    { auto P = MakePatches(0, 2, G2_DMGSUB); CallThis<void>(c.a, car); }
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// FixTyre (EXE 0x6A3580, thiscall ret 4)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunFixTyre(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillDamage(c, car);
    const uint32 wheel = c.below(4);
    CallThis<void>(c.a, car, wheel);
    c.out["wheel"] = wheel;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// FixDoor (EXE 0x6A35A0, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunFixDoor(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2HdBit(0, 0xCC, 8, c.chance(3));
    G2SyncHandling(car, 0);
    G2FillNodes(c, car, 5);
    G2FillDamage(c, car); G2FillDoors(c, car);
    const int32 node = int32(c.below(25));
    const uint32 door = c.below(6);
    { auto P = MakePatches(0, 2, G2_COMPVIS); CallThis<void>(c.a, car, node, door); }
    c.out["node"] = node; c.out["door"] = door;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// FixPanel (EXE 0x6A3670, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunFixPanel(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 5);
    G2FillDamage(c, car);
    G2FillDoors(c, car);
    G2FillChassis(c, car);
    const int32 node = int32(c.below(25));
    const uint32 panel = c.below(7);
    G2FillPanels(c, car, node);
    if (c.chance(4)) G2ChassisLowWord(car, uint16(node));        // el cuarto hueco (m_swingingChassis) tambien coincide
    { auto P = MakePatches(0, 2, G2_COMPVIS); CallThis<void>(c.a, car, node, panel); }
    c.out["node"] = node; c.out["panel"] = panel;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// SetTotalDamage (EXE 0x6A27F0, thiscall ret 4)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSetTotalDamage(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 5);
    G2FillDamage(c, car);
    G2SetModelInfo(c, 0, G2DamageMask(c), 4);
    const uint32 randomness = c.i % 2 == 0 ? 1u : (c.chance(2) ? 1u : 0u);
    { auto P = MakePatches(G_RNG, 2, G2_COMPVIS); CallThis<void>(c.a, car, randomness); }
    c.out["randomness"] = randomness;
    G2Finish(c);
    return true;
}

} // namespace
