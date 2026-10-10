#pragma once
// Caso de CPickups::Update (0x458DE0). Recorre el array REAL de pickups (0x9788C0, 620 x 0x20 bytes) con un fixture determinista; la zona 0x978610..0x97D690
// se guarda y se restaura en cada ejecucion (el banco corre dentro de un solo fotograma). Los objetos de los pickups son objetos FALSOS con tabla virtual
// falsa cuya ranura 0 (destructor con borrado) se traza. Los colaboradores (visibilidad, creacion de objeto, mundo, pad, tareas, CPickup::Update) son dobles.
#include "HudCases.hpp"

namespace HudCamFinal2Live {

constexpr size_t kFakeObjs = 64, kFakeObjSize = 0x1A0;
alignas(16) inline std::array<std::array<uint8, kFakeObjSize>, kFakeObjs> fakeObj{};
inline std::array<void*, 16> objVt{};
inline uint32 ObjIdx(const void* e) {
    if (!e) return 0xFFFFu;
    const auto a = uintptr_t(e), b = uintptr_t(fakeObj.data());
    return (a >= b && a < b + sizeof(fakeObj)) ? uint32((a - b) / kFakeObjSize) : 0xFFFEu;
}
inline uint32 PickIdx(const void* self) { return uint32((uintptr_t(self) - PICKUPS) / 0x20); }
inline std::array<uint8, 620> fixVisible{};
inline uint32 fixCollect{}, fixTargetBtn{}, fixTask701{}, fixTask1600{}, fixUpdSeed{};
inline Buf<0x100> intelA, intelB, taskObj;
inline Buf<0x40> pickPedA, pickPedB; inline Buf<0x20> pickVehA, pickVehB;
inline Buf<0x800> pickPedAFull, pickPedBFull;

inline void* __fastcall SObjDtor(void* self, void*, uint32 flag) { if (Mine()) Push(T_OBJDELETE, { ObjIdx(self), flag & 0xFF }); return self; }   // vtable[0]: "push 1; call [vt]" -> ret 4
inline uint32 __fastcall SIsVisible(void* self, void*) {
    if (!Mine()) return 0; const uint32 idx = PickIdx(self); const uint32 v = idx < 620 ? fixVisible[idx] : 0u; Push(T_ISVISIBLE, { idx, v }); return v;
}
// CPickup::IsVisible (0x454C70) solo toca EAX/ECX: el EXE sigue usando EDX (=pickup, 0x458E3D lea edx,[esi-4] ... 0x458E62 mov ecx,edx) tras la llamada.
__declspec(naked) inline uint32 SIsVisibleExe() { __asm { push edx
    call SIsVisible
    pop edx
    ret } }
// CPickup::GiveUsAPickUpObject(CObject*&, int32) (miembro, 2 argumentos: ret 8)
inline void __fastcall SGiveObj(void* self, void*, void** pp, int32 slot) {
    if (!Mine()) return; const uint32 idx = PickIdx(self);
    const uint32 h = (idx * 2654435761u) ^ fixUpdSeed; const bool give = (h >> 7) % 4 != 0;
    Push(T_GIVEOBJ, { idx, uint32(slot) });
    *pp = give ? static_cast<void*>(fakeObj[(h >> 11) % kFakeObjs].data()) : nullptr;
}
inline void __cdecl SWorldAdd(void* e) { if (Mine()) Push(T_WORLDADD, { ObjIdx(e) }); }
inline void __cdecl SWorldRemove(void* e) { if (Mine()) Push(T_WORLDREMOVE, { ObjIdx(e) }); }
inline uint32 __fastcall SCollectJust(void*, void*) { if (!Mine()) return 0; Push(T_COLLECTJUST); return fixCollect; }
inline uint32 __fastcall SGetTarget(void*, void*) { if (!Mine()) return 0; Push(T_GETTARGET); return fixTargetBtn; }
// CPedIntelligence::FindTaskByType(eTaskType) (miembro, 1 argumento)
inline void* __fastcall SFindTask(void* self, void*, int32 id) {
    if (!Mine()) return nullptr;
    const uint32 who = self == intelA.p() ? 0u : self == intelB.p() ? 1u : 2u;
    Push(T_FINDTASK, { who, uint32(id) });
    return ((id == 701 && fixTask701) || (id == 1600 && fixTask1600)) ? static_cast<void*>(taskObj.p()) : nullptr;
}
// CPickup::Update(CPlayerPed*, CVehicle*, int32) (miembro, 3 argumentos: ret 0xC). Puede cambiar el tipo o la visibilidad del pickup.
inline uint32 __fastcall SPickupUpdate(void* self, void*, void* ped, void* veh, int32 playerId) {
    if (!Mine()) return 0;
    const uint32 idx = PickIdx(self);
    const uint32 h = ((idx + 1) * 2246822519u) ^ (uint32(playerId) * 3266489917u) ^ fixUpdSeed;
    const uint32 ret = (h >> 9) & 1u, effect = (h >> 13) & 7u;
    auto* p = reinterpret_cast<uint8*>(self);
    if (effect == 1) p[0x1C] = 0;                          // el pickup deja de existir
    else if (effect == 2) p[0x1D] &= 0xF7;                 // deja de ser visible
    const uint32 pedId = ped == fixPed[0] ? 0u : ped == fixPed[1] ? 1u : 2u;
    const uint32 vehId = !veh ? 0u : veh == fixVeh[0] ? 1u : veh == fixVeh[1] ? 2u : 3u;
    Push(T_PICKUPUPDATE, { idx, pedId, vehId, uint32(playerId), ret, effect });
    return ret;
}

inline std::vector<uint8> PickBefore() { return ReversingProbe::Bytes(reinterpret_cast<void*>(PICKUPS), 620 * 0x20); }
inline json PickState(const std::vector<uint8>& before) {
    json o = json::object();
    o["pickups"] = Fnv(reinterpret_cast<void*>(PICKUPS), 620 * 0x20);
    o["globals"] = Fnv(reinterpret_cast<void*>(PICK_REGION), PICKUPS - PICK_REGION);
    o["tail"] = Fnv(reinterpret_cast<void*>(PICKUPS + 620 * 0x20), PICK_REGION + PICK_REGION_SIZE - (PICKUPS + 620 * 0x20));
    json ch = json::array(); const auto after = ReversingProbe::Bytes(reinterpret_cast<void*>(PICKUPS), 620 * 0x20);
    for (int k = 0; k < 620 && ch.size() < 12; ++k) if (std::memcmp(after.data() + k * 0x20, before.data() + k * 0x20, 0x20) != 0) ch.push_back(json::array({ k, Hex(after.data() + k * 0x20, 0x20) }));
    o["changed_first12"] = ch;
    o["collect_buffer"] = Mem<int32>(0x97D644); o["on_weapon_pickup"] = Mem<int32>(0x97D640); o["collected_index"] = Mem<uint16>(0x978624);
    return o;
}

// =====================================================================================================================
// CPickups::Update (0x458DE0)
// =====================================================================================================================
inline json CaseUpdate(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x8001);
    fixUpdSeed = Rnd(s);
    // el contador de fotogramas recorre los 32 sub-rangos del primer bucle y los 6 del segundo
    Mem<uint32>(0xB7CB4C) = (I % 4 == 0) ? I : Rnd(s) % 5000u;
    Mem<uint8>(0xA43088) = (Rnd(s) % 12 == 0) ? 1 : 0;                  // CReplay::Mode
    for (auto& o : fakeObj) { std::memset(o.data(), 0, o.size()); At<void*>(o.data(), 0) = objVt.data(); }
    auto* base = reinterpret_cast<uint8*>(PICKUPS);
    for (int k = 0; k < 620; ++k) {
        uint8* p = base + k * 0x20;
        for (int b = 0; b < 0x20; ++b) p[b] = uint8(Rnd(s) >> 7);
        At<float>(p, 0) = PickNear(s) * 10.f;
        At<uint8>(p, 0x1C) = (Rnd(s) % 100 < 72) ? uint8(1 + Rnd(s) % 22) : uint8(0);           // tipo (0 = PICKUP_NONE)
        At<uint8>(p, 0x1D) = uint8(Rnd(s));                                                       // banderas: bit 0 bDisabled, bit 3 bVisible, ...
        At<void*>(p, 4) = (Rnd(s) % 100 < 35) ? static_cast<void*>(fakeObj[Rnd(s) % kFakeObjs].data()) : nullptr;
        fixVisible[size_t(k)] = (Rnd(s) % 100 < 60) ? 1 : 0;
    }
    Mem<uint16>(0x978624) = uint16(Rnd(s) % 20);                                              // indice de recogidos (siempre < 20)
    for (int k = 0; k < 20; ++k) Mem<uint32>(0x978628 + 4u * uint32(k)) = Rnd(s);
    static constexpr int32 buffers[] = { 0, 1, 2, 5, 6, 7, -1, 100 }; Mem<int32>(0x97D644) = buffers[Rnd(s) % std::size(buffers)];
    static constexpr int32 onPickup[] = { 0, 0, 1, 2, 50, -3 };        Mem<int32>(0x97D640) = onPickup[Rnd(s) % std::size(onPickup)];
    fixCollect = Rnd(s) & 1; fixTargetBtn = Rnd(s) & 1; fixTask701 = (Rnd(s) % 4 == 0); fixTask1600 = (Rnd(s) % 4 == 0);
    // jugadores: Players[0].m_pPed siempre existe (el EXE lo desreferencia); Players[1].m_pPed a veces
    pickPedAFull.Fill(0); pickPedBFull.Fill(0); pickVehA.Fill(0); pickVehB.Fill(0);
    At<uint32>(pickPedAFull.p(), 0x47C) = P(intelA.p()); At<uint32>(pickPedBFull.p(), 0x47C) = P(intelB.p());
    Mem<uint32>(PLAYERS) = P(pickPedAFull.p());
    Mem<uint32>(PLAYERS + 0x190) = (Rnd(s) % 3 == 0) ? 0u : P(pickPedBFull.p());
    Mem<uint8>(PLAYER_FOCUS) = (Mem<uint32>(PLAYERS + 0x190) && (Rnd(s) & 1)) ? 1 : 0;
    fixPed[0] = pickPedAFull.p(); fixPed[1] = Mem<uint32>(PLAYERS + 0x190) ? pickPedBFull.p() : nullptr;
    fixVeh[0] = (Rnd(s) % 3 == 0) ? pickVehA.p() : nullptr; fixVeh[1] = (Rnd(s) % 3 == 0) ? pickVehB.p() : nullptr;
    const auto before = PickBefore();
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return PickState(before);
    });
}

inline void RegisterPickupSites() {
    using std::bit_cast;
    AddSite("CPickup::IsVisible", 0x454C70, bit_cast<uintptr_t>(&CPickup::IsVisible), (&SIsVisibleExe), (&SIsVisible));
    AddSite("CPickup::GiveUsAPickUpObject", 0x4567E0, bit_cast<uintptr_t>(&CPickup::GiveUsAPickUpObject), (&SGiveObj));
    AddSite("CWorld::Add", 0x563220, bit_cast<uintptr_t>(&CWorld::Add), (&SWorldAdd));
    AddSite("CWorld::Remove", 0x563280, bit_cast<uintptr_t>(&CWorld::Remove), (&SWorldRemove));
    AddSite("CPad::CollectPickupJustDown", 0x540A70, bit_cast<uintptr_t>(&CPad::CollectPickupJustDown), (&SCollectJust));
    AddSite("CPad::GetTarget", 0x540670, bit_cast<uintptr_t>(&CPad::GetTarget), (&SGetTarget));
    AddSite("CPedIntelligence::FindTaskByType", 0x600EE0, bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType), (&SFindTask));
    AddSite("CPickup::Update", 0x457410, bit_cast<uintptr_t>(static_cast<bool(CPickup::*)(CPlayerPed*, CVehicle*, int32)>(&CPickup::Update)), (&SPickupUpdate));
}
inline std::unique_ptr<Patches> PickupPatches(const std::string& name) {
    if (name == "Update") return Use({ "FindPlayerPed", "FindPlayerVehicle", "CPickup::IsVisible", "CPickup::GiveUsAPickUpObject", "CWorld::Add", "CWorld::Remove", "CPad::CollectPickupJustDown",
        "CPad::GetTarget", "CPedIntelligence::FindTaskByType", "CPickup::Update" });
    return nullptr;
}
}
