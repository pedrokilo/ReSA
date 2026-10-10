#pragma once
// Casos: Process, ProcessAllSearchLights, ProcessWaitingForScriptBrainArray, lectura del SCM (ReadObjectNames, UpdateObjectIndices, ReadMultiScript),
// Init, RemoveScriptTextureDictionary, Save, Load y ScriptAttachAnimGroupToCharModel. Load/Save solo usan el flujo en memoria (stubs de 0x5D1270/0x5D1300):
// no se toca ningun fichero de guardado.
#include "Cases2.hpp"

namespace ScriptsFinal3Live {

inline Hook* gSaveHook{};

// ---- ProcessAllSearchLights
inline json CaseProcessSearchLights(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3001); ClearAllFakes(); BuildRanges();
    FillRandom(SEARCHLIGHTS, 8 * 0x7C, s);
    static constexpr float speeds[] = { 0.f, 0.1f, 0.5f, 1.f, 5.f, 10.f, 100.f, -1.f };
    for (uint32 i = 0; i < 8; ++i) {
        uint8* l = reinterpret_cast<uint8*>(SEARCHLIGHTS) + 0x7C * i;
        l[0] = Rnd(s) % 10 < 7; l[1] &= 1; l[2] &= 1;
        l[3] = uint8((Rnd(s) % 7) | (Rnd(s) % 8 ? 0x80 : 0));
        for (size_t off : { 0x08u, 0x0Cu, 0x10u, 0x14u, 0x18u, 0x1Cu, 0x20u, 0x24u, 0x28u, 0x2Cu, 0x30u, 0x34u, 0x38u, 0x3Cu }) At<float>(l, off) = Rnd(s) % 6 == 0 ? PickNear(s) : PickWorld(s);
        At<float>(l, 0x40) = Rnd(s) % 3 ? speeds[Rnd(s) % std::size(speeds)] : float(Rnd(s) % 800) / 16.f;
        At<uint32>(l, 0x44) = 0; At<uint32>(l, 0x4C) = 0;
        uint8* fol = ObjAt(i); EntInit(fol, 0x40, 4, 0, (Rnd(s) & 1) ? Mat(32 + i) : nullptr);
        if (At<uint32>(fol, 0x14)) RandMatrix(Mat(32 + i), s); else SetVec(fol, 4, PickWorld(s), PickWorld(s), PickWorld(s));
        At<uint32>(l, 0x48) = P(fol);
        const bool hasBulb = Rnd(s) % 5 != 0;
        uint8* bulb = EntAt(2 * i); uint8* hous = EntAt(2 * i + 1);
        EntInit(bulb, 0x40, 4, 0, Mat(2 * i)); EntInit(hous, 0x40, 4, 0, Mat(2 * i + 1));
        RandMatrix(Mat(2 * i), s); RandMatrix(Mat(2 * i + 1), s);
        if (Rnd(s) & 1) MakeRwBacked(bulb);
        if (Rnd(s) & 1) MakeRwBacked(hous);
        At<uint32>(l, 0x54) = hasBulb ? P(bulb) : 0u; At<uint32>(l, 0x50) = hasBulb ? P(hous) : 0u;
    }
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        json o = Base(0); Diag(o, "lights", SEARCHLIGHTS, 8 * 0x7C); o["mats"] = Fnv(matStore.data(), matStore.size()); o["ents"] = Fnv(entStore.data(), entStore.size()); return o;
    });
}

// ---- ProcessWaitingForScriptBrainArray
inline json CaseProcessWaiting(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3002); ClearAllFakes(); BuildRanges();
    for (uint32 i = 0; i < 70; ++i) {
        uint8* b = reinterpret_cast<uint8*>(0xA90CF0) + 0x14 * i;
        for (uint32 k = 0; k < 0x14; ++k) b[k] = uint8(Rnd(s));
        At<int16>(b, 0) = int16(Rnd(s) % 80); At<int8>(b, 2) = int8(Rnd(s) % 6 == 0 ? int32(Rnd(s) % 12) - 6 : int32(Rnd(s) % 5));
    }
    for (uint32 i = 0; i < 80; ++i) Mem<uint8>(STREAM_INFO_LOAD + 0x10 + 20 * i) = uint8(Rnd(s) & 1);
    fx.inRange = Rnd(s) & 1; for (auto& c : fx.centre) c = PickWorld(s);
    const uint32 focus = Rnd(s) & 1; Mem<uint8>(PLAYER_FOCUS) = uint8(focus);
    Mem<uint32>(PLAYERS + 0x190 * focus) = (Rnd(s) % 8) ? P(PedAt(0)) : 0u;
    for (uint32 i = 0; i < 150; ++i) {
        const uint32 brain = Rnd(s) % 70; const int8 type = At<int8>(reinterpret_cast<uint8*>(0xA90CF0) + 0x14 * brain, 2);
        uint8* e = nullptr;
        if (Rnd(s) % 4 == 0) {
            if (type == 0 || type == 3) { e = PedAt(Rnd(s) % N_PED); At<uint8>(e, 0x794) = uint8(Rnd(s) % 70); }
            else if (type == 1 || type == 4) { e = ObjAt(Rnd(s) % N_OBJ); At<uint32>(e, 0x140) = Rnd(s); At<uint8>(e, 0x168) = uint8(Rnd(s) % 70); }
            else e = EntAt(Rnd(s) % N_ENT);
        }
        Mem<uint32>(WAITING + 8 * i) = P(e); Mem<int16>(WAITING + 8 * i + 4) = int16(brain); Mem<int16>(WAITING + 8 * i + 6) = int16(Rnd(s));
    }
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return Base(0); });
}

// ---- Process
inline json CaseProcess(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3003); ClearAllFakes(); BuildRanges(); InstallPools(s, 75);
    static constexpr float ts[] = { 0.f, 0.5f, 1.f, 1.5f, 2.3f, 5.f, 10.f, 12.5f, 50.f, 130.f };
    Mem<float>(0xB7CB58) = ts[Rnd(s) % std::size(ts)];                   // Process lee ms_fTimeStepNonClipped (0x46A010 fld [0B7CB58h])
    Mem<float>(0xB7CB5C) = Mem<float>(0xB7CB58);
    Mem<uint8>(0xA43088) = Rnd(s) % 10 == 0 ? 1 : 0;
    BuildScripts(s, Rnd(s) % 7, false);
    FillRandom(0xA48960, 0x100, s);
    FillRandom(TEXT_LINES, 96 * 0x44, s); FillRandom(RECTS, 128 * 0x3C, s);
    Mem<uint8>(0xA44B67) = uint8(Rnd(s) % 3); Mem<uint16>(0xA44B68) = 0; Mem<uint16>(0xA44B5C) = 0;
    Mem<uint8>(0xA444B0) = uint8(Rnd(s) % 4 == 0 ? Rnd(s) : Rnd(s) % 4);
    fx.killScript = 0xFFFFFFFFu; fx.killTarget = 0xFFFFFFFFu;
    if (Mem<uint32>(PENDING_ACTIVE) && Rnd(s) % 2) {
        auto* first = reinterpret_cast<uint8*>(Mem<uint32>(PENDING_ACTIVE));
        fx.killScript = ScriptIdx(first); fx.killTarget = At<uint32>(first, 0) ? ScriptIdx(reinterpret_cast<void*>(At<uint32>(first, 0))) : 0xFFFFFFFFu;
    }
    for (uint32 i = 0; i < N_PED; ++i) { uint8* p = PedAt(i); At<uint8>(p, 0x484) = uint8(Rnd(s) % 4); At<uint32>(p, 0x47C) = P(IntelAt(i)); }
    const uint32 localsBefore = Mem<uint32>(0xA489E0), tsBits = Mem<uint32>(0xB7CB58);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        json o = Base(0); Diag(o, "counters", 0xA447F4, 6); Diag(o, "locals", 0xA489E0, 8);
        o["ts_bits"] = tsBits; o["timerA_delta"] = int32(Mem<uint32>(0xA489E0) - localsBefore); o["blocks"] = BlockHashes(); return o;
    });
}

// ---- Lectura del SCM sintetico
inline uint32 BuildImage(uint32& s, uint32 maxObjects) {
    image.fill(0);
    for (uint32 i = 0; i < 0x6000; ++i) image[i] = uint8(Rnd(s));
    const uint32 count = Rnd(s) % (maxObjects + 1), numMission = Rnd(s) % 201;
    auto head = [&](uint32 at, uint32 next, uint8 idx) { image[at] = 2; image[at + 1] = 0; image[at + 2] = 1; At<uint32>(image.data(), at + 3) = next; image[at + 7] = idx; };
    const uint32 off2 = 0x100 + 0xC + 24 * count, off3 = off2 + 0x10 + 8 + 4 * numMission;
    head(0, 0x100, 's'); head(0x100, off2, 0); At<uint32>(image.data(), 0x108) = count;
    for (uint32 i = 0; i < count; ++i) {
        uint8* n = image.data() + 0x10C + 24 * i; const uint32 len = Rnd(s) % 20;
        for (uint32 k = 0; k < 24; ++k) n[k] = k < len ? uint8('A' + Rnd(s) % 26) : (k == len ? 0 : uint8(Rnd(s)));
    }
    head(off2, off3, 1);
    At<uint32>(image.data(), off2 + 8) = Rnd(s); At<uint32>(image.data(), off2 + 12) = Rnd(s);
    At<uint16>(image.data(), off2 + 16) = uint16(numMission); At<uint16>(image.data(), off2 + 18) = uint16(Rnd(s)); At<uint32>(image.data(), off2 + 20) = Rnd(s);
    for (uint32 i = 0; i < numMission; ++i) At<uint32>(image.data(), off2 + 24 + 4 * i) = Rnd(s);
    return off3 + 16;
}
inline void CopyImageToScriptSpace(uint32 size) { std::memcpy(reinterpret_cast<void*>(SCRIPT_SPACE), image.data(), size); }
inline json CaseReadObjectNames(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3004); FillRandom(USED_OBJECTS, 395 * 0x1C + 4, s); const uint32 sz = BuildImage(s, 395); CopyImageToScriptSpace(sz);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "count", NUM_USED_OBJECTS, 2); o["objs"] = Fnv(reinterpret_cast<void*>(USED_OBJECTS), 395 * 0x1C); return o; });
}
inline json CaseUpdateObjectIndices(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3005); FillRandom(USED_OBJECTS, 395 * 0x1C, s);
    for (uint32 i = 0; i < 395; ++i) { uint8* o = reinterpret_cast<uint8*>(USED_OBJECTS) + 0x1C * i; const uint32 len = Rnd(s) % 20; for (uint32 k = len; k < 24; ++k) o[k] = k == len ? 0 : uint8(Rnd(s)); for (uint32 k = 0; k < len; ++k) o[k] = uint8('A' + Rnd(s) % 26); }
    Mem<uint16>(NUM_USED_OBJECTS) = uint16(Rnd(s) % 8 == 0 ? Rnd(s) % 396 : Rnd(s) % 40);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); o["objs"] = Fnv(reinterpret_cast<void*>(USED_OBJECTS), 395 * 0x1C); return o; });
}
inline json CaseReadMulti(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3006); FillRandom(0xA444B4, 0x350, s); const uint32 sz = BuildImage(s, 40); CopyImageToScriptSpace(sz);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "info", 0xA444B4, 0x350); return o; });
}

// ---- TXD falso para RemoveScriptTextureDictionary
inline void InstallTxdPool(uint32& s) {
    txdStore.fill(0); txdDictStore.fill(0xAB);
    txdPool = { txdStore.data(), txdStates.data(), 8, 7, 0, 0, 0 };
    for (uint32 i = 0; i < 8; ++i) { txdStates[i] = (Rnd(s) % 3) ? uint8(Rnd(s) & 0x7F) : uint8(0x80); if (Rnd(s) & 1) At<uint32>(txdStore.data(), 12 * i) = P(txdDictStore.data()); }
    Mem<uint32>(0xC8800C) = P(&txdPool);
    // El EXE desreferencia el TxD aunque la ranura este libre (0x46925C/0x465A8C `cmp [ecx],0` con ecx = 0): solo ranuras usadas
    fx.txdSlot = 0xFFFFFFFFu;
    if (Rnd(s) % 4 != 0) for (uint32 k = 0, i = Rnd(s) % 8; k < 8; ++k, i = (i + 1) % 8) if (!(txdStates[i] & 0x80)) { fx.txdSlot = i; break; }
}
inline json CaseRemoveTxd(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3007); FillRandom(0xA94B68, 0x200, s); InstallTxdPool(s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return Base(0); });
}

// ---- Init
inline json CaseInit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3008); ClearAllFakes(); BuildRanges();
    FillRandom(SCR_BASE, SCR_END - SCR_BASE, s);                       // estado previo aleatorio: Init no debe tocar lo que el EXE no toca
    Mem<uint8>(0xB72910) = 0;
    const uint32 sz = BuildImage(s, 395); fx.imageSize = sz; fx.fileOk = 1;
    for (uint32 i = 0; i < N_ENT; ++i) {}
    InstallTxdPool(s); Mem<uint32>(0xC17044) = 320u + (Rnd(s) % 1700u);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); o["blocks"] = BlockHashes(); return o; });
}

// ---- Save / Load: estado coherente con pools falsos
inline void BuildSaveState(uint32& s) {
    ClearAllFakes(); BuildRanges(); InstallPools(s, 85); BuildVtables();
    FillRandom(SCR_BASE, SCR_END - SCR_BASE, s);
    Mem<uint8>(0xB72910) = 0;
    static constexpr uint32 totals[] = { 0, 1, 4000, 51199, 51200, 51201, 102400, 102401, 153600, 200000 };
    const uint32 total = Rnd(s) % 3 == 0 ? totals[Rnd(s) % std::size(totals)] : 8u + Rnd(s) % 120000u;
    uint8* ss = reinterpret_cast<uint8*>(SCRIPT_SPACE); ss[0] = 2; ss[1] = 0; ss[2] = 1; At<uint32>(ss, 3) = total; ss[7] = 's';
    BuildScripts(s, 1 + Rnd(s) % 6, true); // el EXE de Save desreferencia pActiveScripts (0x5D4F13): minimo un script activo
    for (uint32 i = 0; i < 25; ++i) {
        auto* e = reinterpret_cast<int32*>(BUILDING_SWAPS + 12 * i);
        e[0] = Rnd(s) % 3 ? 0 : int32(P(BldAt(Rnd(s) % N_BLD))); e[1] = int32(Rnd(s)); e[2] = int32(Rnd(s));
    }
    for (uint32 i = 0; i < 20; ++i) {
        const uint32 k = Rnd(s) % 4; uint8* e = nullptr;
        if (k == 1) { e = BldAt(Rnd(s) % N_BLD); At<uint8>(e, 0x36) = 1; } else if (k == 2) { e = ObjAt(Rnd(s) % N_OBJ); At<uint8>(e, 0x36) = 4; } else if (k == 3) { e = DumAt(Rnd(s) % N_DUM); At<uint8>(e, 0x36) = 5; }
        Mem<uint32>(INVISIBLE + 4 * i) = P(e);
    }
    for (uint32 i = 0; i < 8; ++i) { uint8* g = reinterpret_cast<uint8*>(ANIM_GROUPS) + 20 * i; At<int32>(g, 0) = -1; }
}
inline json SaveOut() { json o = Base(0); o["bld"] = Fnv(bldStore.data(), bldStore.size()); o["obj"] = Fnv(objStore.data(), objStore.size()); o["dum"] = Fnv(dumStore.data(), dumStore.size()); return o; }
inline json CaseSave(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3009); BuildSaveState(s); fx.recording = false;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); return SaveOut(); });
}
inline json CaseLoad(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x300A); BuildSaveState(s);
    // el flujo se graba con el Save del EXE (hook desactivado) sobre el estado coherente, y luego se carga sobre un estado aleatorio
    const bool saveWas = gSaveHook->Hooked(); gSaveHook->State(false);
    fx.recording = true; fx.savePos = 0; fx.loadPos = 0;
    nrec = 0; reinterpret_cast<void(__cdecl*)()>(0x5D4C40)(); fx.recording = false;
    gSaveHook->State(saveWas);
    const uint32 sz = BuildImage(s, 395); fx.imageSize = sz; fx.fileOk = 1; InstallTxdPool(s);
    FillRandom(0xA43C78 + 0x80, 0x10000, s);                           // ensucia una parte del estado antes de cargar
    Mem<uint8>(0xB72910) = 0;
    for (uint32 i = 0; i < N_BLD; ++i) At<uint32>(BldAt(i), 0x1C) = Rnd(s);
    for (uint32 i = 0; i < N_OBJ; ++i) At<uint32>(ObjAt(i), 0x1C) = Rnd(s);
    for (uint32 i = 0; i < N_DUM; ++i) At<uint32>(DumAt(i), 0x1C) = Rnd(s);
    return Run(hook, cand, addr, [&] { fx.loadPos = 0; reinterpret_cast<void(__cdecl*)()>(addr)(); return SaveOut(); });
}

// ---- ScriptAttachAnimGroupToCharModel
inline json CaseAttachAnim(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x300B); BuildVtables();
    InstallFakeModel(float(Rnd(s) % 100), 0, int32(Rnd(s) % 6) - 1);
    fx.animBlock = uint32(int32(Rnd(s) % 6) - 1);
    static const char* names[] = { "ped", "man", "gangs", "", "a" }; const char* name = names[Rnd(s) % std::size(names)];
    return Run(hook, cand, addr, [&] { return Base(reinterpret_cast<bool(__cdecl*)(int32, const char*)>(addr)(FAKE_MODEL, name) ? 1 : 0); });
}
}
