#pragma once
// Grupo G2, parte C: OpenDoor, ProcessSwingingDoor, SpawnFlyingComponent, RemoveBonnetInPedCollision,
// dmgDrawCarCollidingParticles y SetTowLink. Incluido por CasesG2.hpp.
namespace AutomobileFinal4Live {

inline float G2SmallF(Ctx& c, float range) { return c.chance(8) ? 0.f : c.uni(-range, range); }
inline void G2Vec3(Ctx& c, uint8* p, size_t off, float range) { for (size_t k = 0; k < 3; ++k) At<float>(p, off + 4 * k) = G2SmallF(c, range); }

// ---------------------------------------------------------------------------------------------------------------------
// OpenDoor (vtable 0x871120 hueco 27, thiscall ret 0x14)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunOpenDoor(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    const int32 node = int32(c.below(25));
    G2FillNodes(c, car, 7, c.chance(8) ? 0u : (1u << node));        // a veces el nodo es nulo (el EXE sale sin hacer nada)
    G2FillDamage(c, car); G2FillDoors(c, car);
    const uint32 door = c.below(6);
    void* ped = c.chance(2) ? static_cast<void*>(g2b.ped) : nullptr;
    const float ratio = c.pick<float>({0.f, 0.f, 1.f, 0.5f, 1.3f, c.uni(0.f, 1.f), c.uni(0.f, 1.f), 0.9999999f});
    const uint32 sound = c.chance(3) ? 0u : 1u;
    { auto P = MakePatches(0, 2, G2_AUDF | G2_FRALL | G2_EVENT); CallThis<void>(c.a, car, ped, node, door, ratio, sound); }
    c.out["node"] = node; c.out["door"] = door; c.out["ratio"] = F(ratio); c.out["sound"] = sound;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// ProcessSwingingDoor (EXE 0x6A9D70, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSwingingDoor(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel(), c.uni(0.05f, 1.2f));
    auto* car = Cr(0);
    const int32 node = int32(c.below(25));
    const uint32 door = c.chance(3) ? 0u : c.below(6);
    G2FillNodes(c, car, 8, c.chance(10) ? 0u : (1u << node));
    G2FillDamage(c, car); G2FillDoors(c, car);
    At<uint8>(car, G2_O_DMG + 9 + door) = uint8(G2PickDamageState(c));
    At<float>(car, G2_O_DMGINT) = c.pick<float>({0.f, 50.f, 100.f, 100.0001f, 150.f, 300.f, c.uni(0.f, 400.f)});
    G2SetStatus(car, c.pick<uint32>({0u, 0u, 3u, 3u, 2u, 4u, 5u}));
    At<uint32>(car, G2_O_DOORLOCK) = c.below(10);
    G2Vec3(c, car, G2_O_MOVE, 1.5f); G2Vec3(c, car, G2_O_TURN, 0.5f); G2Vec3(c, car, G2_O_MOVEF, 2.f); G2Vec3(c, car, G2_O_TURNF, 2.f);
    // el capo (puerta 0) con mas frecuencia en los estados que activan sus ramas
    if (door == 0) {
        auto* b = car + G2_O_DOORS;
        if (c.chance(2)) At<uint16>(b, 8) = uint16((At<uint16>(b, 8) & 0xFFF0u) | 1u);                // eje Y
        if (c.chance(2)) At<uint8>(b, 0xB) = 1;                                                       // DOOR_HIT_MAX_END
    }
    g2ProcRet = c.chance(2); g2ImpactRet = c.chance(2);
    g2SpawnRet = c.chance(5) ? nullptr : static_cast<void*>(g2b.obj[0]);
    { auto P = MakePatches(G_RNG, 2, G2_DOORP | G2_SPAWN | G2_AUDP | G2_AUDF | G2_COMPVIS | G2_FORCE); CallThis<void>(c.a, car, node, door); }
    c.out["node"] = node; c.out["door"] = door;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// SpawnFlyingComponent (EXE 0x6A8580, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSpawn(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel(), c.uni(0.05f, 3.0f));
    auto* car = Cr(0);
    At<int16>(car, 0x598) = c.pick<int16>({int16(-1), int16(0), int16(5), int16(300)});     // m_nPreviousRemapTxd
    At<void*>(car, 0x59C) = c.chance(2) ? static_cast<void*>(w.misc + 0x40) : nullptr;       // m_pRemapTexture
    At<uint8>(car, G2_O_COLORS) = uint8(c.next()); At<uint8>(car, G2_O_COLORS + 1) = uint8(c.next());
    // model info falso de los modelos TEMPCOL (374..379) con modelo de colision propio
    c.FillMi(1, 0);
    for (int m = 374; m <= 379; ++m) c.InstallMi(m, 1);
    const int32 node = c.chance(5) ? 18 : int32(c.below(25));
    const uint32 type = c.chance(6) ? c.below(8) : c.below(5);
    G2FillNodes(c, car, 9, 1u << node);
    if (c.chance(10)) At<void*>(car, G2_O_NODES + 4 * size_t(node)) = nullptr;                 // nodo nulo
    g2CurAtomic = c.chance(10) ? nullptr : static_cast<void*>(w.at[0]);                       // atomo del marco
    AtA<uint16>(0xBB4A70) = c.pick<uint16>({0, 5, 9, 10, 11, 15, 19, 20, 21, 60, 149, 150, 200});
    AtA<uint32>(0xB7449C) = uint32(uintptr_t(g2b.pool));
    g2PoolFull = c.chance(14);
    G2SetStatus(car, c.chance(3) ? 5u : c.pick<uint32>({0u, 2u, 3u, 4u}));
    { uint8 f = At<uint8>(car, G2_O_FLAGS); f = c.chance(2) ? uint8(f | 0x80u) : uint8(f & 0x7Fu); At<uint8>(car, G2_O_FLAGS) = f; }
    g2OnScreen = c.chance(2);
    At<uint32>(car, G2_O_PHYS) = c.chance(2) ? 0x20000000u : 0u;
    G2Vec3(c, car, G2_O_MOVE, 1.5f); G2Vec3(c, car, G2_O_TURN, 0.8f);
    if (c.chance(3)) At<float>(car, G2_O_MOVE + 8) = c.uni(-0.5f, 0.f);                        // z <= 0 (ramas de la velocidad vertical)
    AtA<uint32>(0xB7CB84) = c.pick<uint32>({0u, 1000u, 20000u, 16777219u, 100000000u, 0x7FFFFFFFu, 0x80000001u, 0xFFFFFF00u, c.next() & 0x7FFFFFFFu, c.next()});
    // camara falsa
    if (c.chance(2)) { AtA<uint32>(0xB6F03C) = uint32(uintptr_t(g2b.cam)); std::memset(g2b.cam, 0, sizeof(g2b.cam)); At<float>(g2b.cam, 0x00) = 1.f; At<float>(g2b.cam, 0x14) = 1.f; At<float>(g2b.cam, 0x28) = 1.f;
                       for (size_t k = 0; k < 3; ++k) At<float>(g2b.cam, 0x30 + 4 * k) = c.uni(-30.f, 30.f); }
    else { AtA<uint32>(0xB6F03C) = 0u; for (size_t k = 0; k < 3; ++k) AtA<float>(0xB6F02C + 4 * k) = c.uni(-30.f, 30.f); }
    g2ColRet = c.pick<int32>({-1, 0, 1, 5});
    for (size_t k = 0; k < 3; ++k) g2AttachPos[k] = c.uni(-30.f, 30.f);
    void* ret{};
    { auto P = MakePatches(0, 2, G2_FRALL | G2_OBJNEW | G2_OBJFN | G2_RWOBJ | G2_FORCE | G2_PROCCOL | G2_WORLDADD | G2_ONSCREEN); ret = CallThis<void*>(c.a, car, node, type); }
    c.ret = Id(ret);
    c.out["node"] = node; c.out["type"] = type; c.out["ntemp"] = AtA<uint16>(0xBB4A70);
    c.out["obj0"] = c.Bytes(g2b.obj[0], 0x17C); c.out["objmat0"] = c.Bytes(g2b.objMat[0], 0x60); c.out["pool11"] = g2b.pool[0x11];
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// RemoveBonnetInPedCollision (EXE 0x6AA200, thiscall sin argumentos)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunRemoveBonnet(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 6, c.chance(5) ? 0u : (1u << 16));
    G2FillDamage(c, car); G2FillDoors(c, car);
    At<uint8>(car, G2_O_DMG + 9) = uint8(c.pick<uint32>({0u, 1u, 1u, 1u, 2u, 3u, 3u, 3u, 4u}));       // estado del capo (puerta 0)
    auto* b = car + G2_O_DOORS;
    const float open = c.uni(0.5f, 1.5f);
    At<float>(b, 0x0) = open;
    const float t = open * 0.4f;
    At<float>(b, 0xC) = c.pick<float>({t, std::nextafter(t, 10.f), std::nextafter(t, -10.f), open * 0.39f, open * 0.8f, 0.f, open});
    g2SpawnRet = G2SpawnObj(c);
    void* ret{};
    { auto P = MakePatches(0, 2, G2_SPAWN | G2_AUDP | G2_COMPVIS); ret = CallThis<void*>(c.a, car); }
    c.ret = Id(ret);
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// dmgDrawCarCollidingParticles (EXE 0x6A6DC0, thiscall ret 0xC)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunDmgParticles(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel(), c.uni(0.05f, 1.2f));
    auto* car = Cr(0);
    const uint32 mag = c.below(6);
    const float scale = mag == 0 ? 0.f : mag == 1 ? 0.01f : mag == 2 ? 0.1f : mag == 3 ? 0.3f : mag == 4 ? 1.f : 4.f;
    for (size_t k = 0; k < 3; ++k) At<float>(car, G2_O_MOVE + 4 * k) = scale == 0.f ? 0.f : c.uni(-1.f, 1.f) * scale;
    AtA<float>(0xB7CB5C) = c.uni(0.1f, 3.f);                                         // CTimer::ms_fTimeStep
    At<uint8>(car, G2_O_COLORS) = uint8(c.below(128));                              // color primario
    At<float>(car, G2_O_BRIGHT) = c.uni(0.f, 1.5f);                                 // brillo de la superficie de contacto
    if (c.chance(4)) At<void*>(car, 0x14) = nullptr;                                // sin matriz (usa la posicion de la placa)
    g2OnScreen = c.chance(4) ? false : true;
    float* pos = reinterpret_cast<float*>(g2b.vec[0]);
    for (size_t k = 0; k < 3; ++k) pos[k] = c.uni(-60.f, 60.f);
    const float force = c.pick<float>({-30.f, 0.f, 5.f, 50.f, 127.9f, 255.f, 400.f, c.uni(-100.f, 600.f), 99.9f, 100.f});
    const uint32 weapon = c.chance(4) ? 0u : c.chance(4) ? 14u : c.below(60);
    { auto P = MakePatches(0, 2, G2_FX | G2_ONSCREEN); CallThis<void>(c.a, car, pos, force, weapon); }
    c.out["force"] = F(force); c.out["weapon"] = weapon;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// SetTowLink (vtable 0x871120 hueco 61, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSetTowLink(Ctx& c) {
    G2Begin();
    const int modelThis = c.carModel();
    int modelTractor = c.pick({525, 531, 525, 531, 0});
    if (modelTractor == 0) { do { modelTractor = c.carModel(); } while (modelTractor == modelThis); }
    G2Car(c, 0, modelThis, c.uni(0.05f, 1.0f));
    G2Car(c, 1, modelTractor, c.uni(0.05f, 1.0f));
    auto* car = Cr(0); auto* tractor = Cr(1);
    SetVt(59, FnPtr(&S2_VtHitch)); SetVt(60, FnPtr(&S2_VtBar));
    G2SetStatus(car, c.pick<uint32>({3u, 10u, 4u, 2u, 2u, 0u, 1u, 5u, 8u, 12u, 3u}));
    if (!c.chance(6)) { At<void*>(car, G2_O_TOWING) = nullptr; } else { At<void*>(car, G2_O_TOWING) = tractor; }
    At<void*>(tractor, G2_O_TOWED) = nullptr;
    At<uint16>(tractor, G2_O_MISCANGLE) = uint16(c.next());
    if (c.chance(6)) { At<void*>(car, 0x14) = nullptr; At<float>(car, 0x10) = c.uni(-3.f, 3.f); }              // sin matriz: usa rumbo y posicion de la placa
    if (c.chance(6)) { At<void*>(tractor, 0x14) = nullptr; At<float>(tractor, 0x10) = c.uni(-3.f, 3.f); }
    AtA<uint32>(0xB7CB84) = c.next() & 0x7FFFFFFFu;
    g2HitchRet = !c.chance(6); g2BarRet = !c.chance(6);
    for (size_t k = 0; k < 3; ++k) { g2HitchVec[k] = c.uni(-5.f, 5.f); g2BarVec[k] = c.uni(-5.f, 5.f); }
    void* trac = c.chance(10) ? nullptr : static_cast<void*>(tractor);
    const uint32 place = c.chance(2) ? 1u : 0u;
    uint8 ret{};
    { auto P = MakePatches(0, 2, G2_TOW | G2_PLACE); ret = CallThis<uint8>(c.a, car, trac, place); }
    c.ret = ret;
    c.out["place"] = place; c.out["towing"] = Id(At<void*>(car, G2_O_TOWING)); c.out["towed"] = Id(At<void*>(tractor, G2_O_TOWED));
    G2Finish(c);
    return true;
}

} // namespace
