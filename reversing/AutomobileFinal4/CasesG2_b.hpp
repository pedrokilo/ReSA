#pragma once
// Grupo G2, parte B: PopDoor, PopPanel, PopBoot, CloseBoot, PopBootUsingPhysics, CloseAllDoors, SetBumperDamage, SetPanelDamage y
// SetDoorDamage. Incluido por CasesG2.hpp.
namespace AutomobileFinal4Live {

// Nodo para PopDoor/PopPanel: con sesgo hacia los nodos especiales (0 = ninguno, 1 = chasis, 5 y 6 = ruedas delanteras)
inline int32 G2PickNode(Ctx& c) {
    const uint32 m = c.below(8);
    if (m == 0) return 0; if (m == 1) return 1; if (m == 2) return c.chance(2) ? 5 : 6;
    return int32(c.below(25));
}
// Objeto falso que devuelve el doble de SpawnFlyingComponent (o nulo)
inline void* G2SpawnObj(Ctx& c) { return c.chance(4) ? nullptr : static_cast<void*>(g2b.obj[0]); }

// ---------------------------------------------------------------------------------------------------------------------
// PopDoor (EXE 0x6ADEF0, thiscall ret 0xC)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunPopDoor(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 5);
    G2FillDamage(c, car);
    g2SpawnRet = G2SpawnObj(c);
    const int32 node = G2PickNode(c);
    const uint32 door = c.below(6);
    const uint32 show = c.chance(3) ? 0u : 1u;
    { auto P = MakePatches(0, 2, G2_SPAWN | G2_AUDP | G2_COMPVIS); CallThis<void>(c.a, car, node, door, show); }
    c.out["node"] = node; c.out["door"] = door; c.out["show"] = show;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// PopPanel (EXE 0x6ADF80, thiscall ret 0xC)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunPopPanel(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 5);
    G2FillDamage(c, car);
    g2SpawnRet = G2SpawnObj(c);
    const int32 node = G2PickNode(c);
    const uint32 panel = c.below(7);
    const uint32 show = c.chance(3) ? 0u : 1u;
    { auto P = MakePatches(0, 2, G2_SPAWN | G2_COMPVIS); CallThis<void>(c.a, car, node, panel, show); }
    c.out["node"] = node; c.out["panel"] = panel; c.out["show"] = show;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// PopBoot (EXE 0x6AF910) y CloseBoot (EXE 0x6AFA20): ambas leen m_aCarNodes[17] (CAR_BOOT) sin comprobar que exista
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunPopBoot(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 5, 1u << 17);
    G2FillDamage(c, car);
    G2FillDoors(c, car);
    CallThis<void>(c.a, car);
    G2Finish(c);
    return true;
}
inline bool G2RunCloseBoot(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillNodes(c, car, 5, 1u << 17);
    G2FillDamage(c, car);
    G2FillDoors(c, car);
    CallThis<void>(c.a, car);
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// PopBootUsingPhysics (EXE 0x6A44D0): estados 0..4 del maletero
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunPopBootPhysics(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    G2FillDamage(c, car);
    G2FillDoors(c, car);
    At<uint8>(car, G2_O_DMG + 9 + 1) = uint8(c.i % 5);          // estado de la puerta 1 (maletero): recorre 0..4
    CallThis<void>(c.a, car);
    c.out["boot_status"] = At<uint8>(car, G2_O_DMG + 10);
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// CloseAllDoors (EXE 0x6A4520): llama por la vtable a IsDoorMissing (hueco 38) y OpenDoor (hueco 27): se sustituyen por dobles
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunCloseAllDoors(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    SetVt(38, FnPtr(&S2_VtDoorMissing)); SetVt(27, FnPtr(&S2_VtOpenDoor));
    static constexpr int kDoors[]{1, 2, 4};
    const int numDoors = kDoors[c.i % 3];
    G2SetModelInfo(c, 0, G2DamageMask(c), numDoors);
    g2Aux[48] = uint8(c.next());
    CallThis<void>(c.a, car);
    c.out["numDoors"] = numDoors;
    G2Finish(c);
    return true;
}

// Estado del panel p (nibble 4 bits del dword 0x14 del gestor de dano)
inline void G2SetPanelStatus(uint8* car, uint32 panel, uint32 st) {
    uint32 v = At<uint32>(car, G2_O_DMG + 0x14); v = (v & ~(0xFu << (4 * panel))) | (st << (4 * panel)); At<uint32>(car, G2_O_DMG + 0x14) = v; }
// Estado estiloso: sesgado hacia 1, 2 y 3
inline uint32 G2PickDamageState(Ctx& c) { return c.pick<uint32>({0u, 1u, 1u, 2u, 2u, 2u, 3u, 3u, 4u}); }

// ---------------------------------------------------------------------------------------------------------------------
// SetBumperDamage (EXE 0x6B1350, thiscall ret 8) y SetPanelDamage (EXE 0x6B1480, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline void G2PanelCommon(Ctx& c, uint8* car, uint32 panel, int32& node) {
    node = int32(CDamageManager::GetCarNodeIndexFromPanel(static_cast<ePanels>(panel)));
    G2FillNodes(c, car, 4, node >= 0 ? (1u << node) : 0u);
    G2FillDamage(c, car);
    G2SetPanelStatus(car, panel, G2PickDamageState(c));
    G2SetModelInfo(c, 0, G2DamageMask(c), 4);
    G2HdBit(0, 0xCC, 28, c.chance(3));                    // m_bBouncePanels
    G2SyncHandling(car, 0);
    G2FillDoors(c, car); G2FillChassis(c, car);
    G2FillPanels(c, car, node);
    const uint32 m = c.below(6);
    if (m <= 1) G2ChassisLowWord(car, 0xFFFF);            // el cuarto hueco libre
    else if (m == 2 && node >= 0) G2ChassisLowWord(car, uint16(node));
    if (c.chance(3)) At<uint16>(car, G2_O_PANELS + 0x20) = 0xFFFF;
    g2SpawnRet = G2SpawnObj(c);
}
inline bool G2RunSetBumperDamage(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    const uint32 panel = c.chance(2) ? 5u + c.below(2) : c.pick<uint32>({0u, 1u, 4u, 5u, 6u});   // paneles 2 y 3 no tienen nodo (-1): el EXE leyo fuera y std::array aborta en la DLL
    int32 node{};
    G2PanelCommon(c, car, panel, node);
    const uint32 withoutFx = c.chance(2) ? 1u : 0u;
    { auto P = MakePatches(G_RNG, 2, G2_SPAWN | G2_COMPVIS); CallThis<void>(c.a, car, panel, withoutFx); }
    c.out["panel"] = panel; c.out["node"] = node; c.out["withoutFx"] = withoutFx;
    G2Finish(c);
    return true;
}
inline bool G2RunSetPanelDamage(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    const uint32 panel = c.chance(3) ? 4u : c.pick<uint32>({0u, 1u, 4u, 5u, 6u});
    int32 node{};
    G2PanelCommon(c, car, panel, node);
    const uint32 glass = c.chance(2) ? 1u : 0u;
    { auto P = MakePatches(G_RNG, 2, G2_SPAWN | G2_COMPVIS | G2_AUDF | G2_GLASS); CallThis<void>(c.a, car, panel, glass); }
    c.out["panel"] = panel; c.out["node"] = node; c.out["glass"] = glass;
    G2Finish(c);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// SetDoorDamage (EXE 0x6B1600, thiscall ret 8)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G2RunSetDoorDamage(Ctx& c) {
    G2Begin();
    G2Car(c, 0, c.carModel());
    auto* car = Cr(0);
    const uint32 door = c.below(6);
    const int32 node = int32(CDamageManager::GetCarNodeIndexFromDoor(static_cast<eDoors>(door)));
    G2FillNodes(c, car, 4, node >= 0 ? (1u << node) : 0u);
    G2FillDamage(c, car);
    At<uint8>(car, G2_O_DMG + 9 + door) = uint8(G2PickDamageState(c));
    G2SetModelInfo(c, 0, G2DamageMask(c), 4);
    G2HdBit(0, 0xCC, 7, c.chance(2));                     // m_bNoswingBoot (bit 7 del byte 0xCC)
    G2SyncHandling(car, 0);
    G2FillDoors(c, car);
    At<uint32>(car, G2_O_DOORLOCK) = c.below(10);          // CanDoorsBeDamaged: 0, 1 y 7 si; el resto no
    g2SpawnRet = G2SpawnObj(c);
    const uint32 withoutFx = c.chance(2) ? 1u : 0u;
    { auto P = MakePatches(0, 2, G2_SPAWN | G2_AUDF | G2_AUDP | G2_COMPVIS | G2_FRALL); CallThis<void>(c.a, car, door, withoutFx); }
    c.out["door"] = door; c.out["node"] = node; c.out["withoutFx"] = withoutFx;
    G2Finish(c);
    return true;
}

} // namespace
