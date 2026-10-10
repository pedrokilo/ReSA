#pragma once
// Casos de las funciones de estado puro de CTheScripts: tabla de switch, modelos bloqueados/suprimidos, LODs conectados, grupos de animaciones, ids de "cosas de script"
// (esferas, sistemas de efectos, checkpoints, focos), lista de scripts y utilidades. Cada caso rellena el fixture con la semilla del caso, ejecuta la funcion
// llamando a la direccion del EXE (con el hook desactivado es el codigo del EXE; con el hook activo salta a la DLL) y devuelve el retorno, el hash de toda la
// zona de estado y fragmentos hexadecimales legibles de lo que se espera que cambie.
#include "Stubs.hpp"

namespace ScriptsFinal3Live {

constexpr uintptr_t SWITCH_TABLE = 0xA43CF8, SWITCH_COUNT = 0xA43F50, SWITCH_DEFAULT_ADDR = 0xA43F54, SWITCH_DEFAULT_EXISTS = 0xA43F58, SWITCH_VALUE = 0xA43F5C, SWITCH_READ = 0xA43F60;
constexpr uintptr_t BLOCKED_MODELS = 0xA448F0, SUPPRESSED_MODELS = 0xA44940, CONNECT_LODS = 0xA44800, ANIM_GROUPS = 0xA44850;
constexpr uintptr_t SPHERES = 0xA91268, EFFECTS = 0xA44110, CHECKPOINTS = 0xA44070, SEQUENCES = 0xA43F68, NUM_SEARCHLIGHTS = 0xA90830, NUM_CHECKPOINTS = 0xA44068;
constexpr uintptr_t BUILDING_SWAPS = 0xA44A30, INVISIBLE = 0xA449E0, USED_OBJECTS = 0xA44B70, NUM_USED_OBJECTS = 0xA44B6C, MULTI_SCRIPT = 0xA444C8;
constexpr uintptr_t FIRES = 0xB71F80, PEDGROUP_IDS = 0xC098D0, PEDGROUP_ACTIVE = 0xC098E0, DM_IDS = 0xC0AFF4, DM_ACTIVE = 0xC0B01C, FX2D_IDS = 0xC3A120, FX2D_ACTIVE = 0xC3A1A0;
constexpr uintptr_t PENDING_IDLE = 0xA8B428, PENDING_ACTIVE = 0xA8B42C;

inline void FillRandom(uintptr_t addr, size_t n, uint32& s) { auto* p = reinterpret_cast<uint8*>(addr); for (size_t i = 0; i < n; ++i) p[i] = uint8(Rnd(s)); }
// Hash FNV por bloques de 256 bytes de la zona de estado (localiza que parte difiere: indice * 256 + 0xA43C78)
inline json BlockHashes() { json b = json::array(); for (uintptr_t a = SCR_BASE; a < SCR_END; a += 256) b.push_back(uint32(Fnv(reinterpret_cast<void*>(a), std::min<uintptr_t>(256, SCR_END - a)))); return b; }
inline json Base(uint64 ret) { return json{ {"ret", ret}, {"region", RegionHash()} }; }
inline void Diag(json& o, const char* key, uintptr_t addr, size_t n) { o[key] = HexAt(addr, n); }
// Identificadores de 16 bits con casos limite (contador de ids de "cosas de script": 0xFFFE / 0xFFFF reinician en 1)
inline uint16 PickId(uint32& s) { static constexpr uint16 t[] = { 0, 1, 2, 0x7FFE, 0x7FFF, 0x8000, 0x8001, 0xFFFC, 0xFFFD, 0xFFFE, 0xFFFF }; return Rnd(s) % 3 == 0 ? uint16(Rnd(s)) : t[Rnd(s) % std::size(t)]; }

// =====================================================================================================================
// Tabla de switch
// =====================================================================================================================
inline json CaseAddSwitch(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1001);
    FillRandom(SWITCH_TABLE, 75 * 8, s);
    Mem<uint16>(SWITCH_COUNT) = uint16(Rnd(s) % 74);
    const int32 v = int32(Rnd(s)), l = int32(Rnd(s));
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32, int32)>(addr)(v, l);
        json o = Base(0); Diag(o, "table", SWITCH_TABLE, 75 * 8); Diag(o, "count", SWITCH_COUNT, 2); return o;
    });
}
inline json CaseReinitSwitch(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1002);
    FillRandom(SWITCH_COUNT, 0x18, s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        json o = Base(0); Diag(o, "switch", SWITCH_COUNT, 0x18); return o;
    });
}
inline json CaseUseSwitch(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1003);
    FillRandom(SWITCH_TABLE, 75 * 8, s);
    const uint32 n = 1 + Rnd(s) % 75;                              // el EXE trabaja con bytes: n = 0 no es un dominio real
    int32 v = int32(Rnd(s) % 2000) - 1000;
    for (uint32 i = 0; i < n; ++i) { v += int32(Rnd(s) % 4); Mem<int32>(SWITCH_TABLE + 8 * i) = v; Mem<int32>(SWITCH_TABLE + 8 * i + 4) = int32(Rnd(s) % 90000); }
    Mem<uint16>(SWITCH_COUNT) = uint16(n);
    Mem<int32>(SWITCH_DEFAULT_ADDR) = int32(Rnd(s) % 90000) + 1; Mem<uint8>(SWITCH_DEFAULT_EXISTS) = uint8(Rnd(s) & 1); Mem<uint16>(SWITCH_READ) = uint16(Rnd(s) % 5);
    const uint32 mode = Rnd(s) % 5;
    int32 query = mode == 0 ? Mem<int32>(SWITCH_TABLE + 8 * (Rnd(s) % n)) : mode == 1 ? Mem<int32>(SWITCH_TABLE) - 1 - int32(Rnd(s) % 3) : mode == 2 ? Mem<int32>(SWITCH_TABLE + 8 * (n - 1)) + 1 + int32(Rnd(s) % 3)
                  : int32(Rnd(s) % 3000) - 1000;
    Mem<int32>(SWITCH_VALUE) = query;
    return Run(hook, cand, addr, [&] {
        int32 target = 0x5A5A5A5A;
        reinterpret_cast<void(__cdecl*)(int32*)>(addr)(&target);
        json o = Base(uint32(target)); Diag(o, "switch", SWITCH_COUNT, 0x18); return o;
    });
}

// =====================================================================================================================
// Modelos bloqueados por el script / suprimidos
// =====================================================================================================================
inline void FillModels(uintptr_t addr, uint32 n, uint32& s, bool allowFree) {
    for (uint32 i = 0; i < n; ++i) Mem<int32>(addr + 4 * i) = (allowFree && Rnd(s) % 3 == 0) ? -1 : int32(400 + Rnd(s) % 12);
}
inline json CaseAddBlocked(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1004);
    FillModels(BLOCKED_MODELS, 20, s, true); FillModels(SUPPRESSED_MODELS, 40, s, true);
    Mem<int32>(BLOCKED_MODELS + 4 * (Rnd(s) % 20)) = -1;          // siempre queda un hueco (el EXE escribiria fuera del array si estuviera lleno)
    const int32 model = Rnd(s) % 4 == 0 ? int32(Rnd(s) % 600) : int32(400 + Rnd(s) % 12);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32)>(addr)(model);
        json o = Base(0); Diag(o, "blocked", BLOCKED_MODELS, 80); Diag(o, "suppressed", SUPPRESSED_MODELS, 160); return o;
    });
}
inline json CaseClearSuppressed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1005); FillRandom(BLOCKED_MODELS, 0x100, s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "models", BLOCKED_MODELS, 0x100); return o; });
}
inline json CaseHasModel(uintptr_t table, uint32 n, unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1006);
    FillModels(table, n, s, true);
    const int32 model = Rnd(s) % 3 == 0 ? -1 : int32(400 + Rnd(s) % 14);
    return Run(hook, cand, addr, [&] {
        const bool r = reinterpret_cast<bool(__cdecl*)(int32)>(addr)(model);
        return Base(r ? 1 : 0);
    });
}
inline json CaseHasSuppressed(unsigned I, bool cand, Hook* hook, uintptr_t addr) { return CaseHasModel(SUPPRESSED_MODELS, 40, I, cand, hook, addr); }
inline json CaseHasBlocked(unsigned I, bool cand, Hook* hook, uintptr_t addr) { return CaseHasModel(BLOCKED_MODELS, 20, I, cand, hook, addr); }

// =====================================================================================================================
// LODs conectados y grupos de animaciones de personajes
// =====================================================================================================================
inline json CaseInitConnectAll(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1007); FillRandom(CONNECT_LODS, 80, s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "lods", CONNECT_LODS, 80); return o; });
}
inline json CaseInitConnectOne(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1008); FillRandom(CONNECT_LODS, 80, s); const uint32 idx = Rnd(s) % 10;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(uint16)>(addr)(uint16(idx)); json o = Base(0); Diag(o, "lods", CONNECT_LODS, 80); return o; });
}
inline json CaseInitAnimAll(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1009); FillRandom(ANIM_GROUPS, 160, s);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "groups", ANIM_GROUPS, 160); return o; });
}
inline json CaseInitAnimOne(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x100A); FillRandom(ANIM_GROUPS, 160, s); const uint32 idx = Rnd(s) % 8;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(uint16)>(addr)(uint16(idx)); json o = Base(0); Diag(o, "groups", ANIM_GROUPS, 160); return o; });
}
inline json CaseAddAnim(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x100B);
    static const char* names[] = { "AAA", "BBB", "CCC", "DDDD", "gangs", "man", "" , "0123456789ABCDE" };
    for (uint32 i = 0; i < 8; ++i) {
        uint8* g = reinterpret_cast<uint8*>(ANIM_GROUPS + 20 * i);
        for (uint32 k = 0; k < 20; ++k) g[k] = uint8(Rnd(s));
        g[19] = 0;                                                  // los nombres sin hueco libre siempre terminan en NUL dentro de la entrada
        if (Rnd(s) % 3 == 0) At<int32>(g, 0) = -1;
        else { At<int32>(g, 0) = int32(100 + Rnd(s) % 4); std::strcpy(reinterpret_cast<char*>(g + 4), names[Rnd(s) % std::size(names)]); }
    }
    const int32 model = Rnd(s) % 5 == 0 ? -1 + int32(Rnd(s) % 2) * 105 : int32(100 + Rnd(s) % 4);
    const char* name = names[Rnd(s) % std::size(names)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32, const char*)>(addr)(model, name);
        json o = Base(0); Diag(o, "groups", ANIM_GROUPS, 160); return o;
    });
}

// Objetos del pool falso de objetos: referencia de la ranura i y conexiones aleatorias
inline json CaseAddConnectedLod(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x100C); ClearAllFakes(); BuildRanges(); InstallPools(s, 80);
    for (uint32 i = 0; i < 10; ++i) {
        auto* e = reinterpret_cast<int32*>(CONNECT_LODS + 8 * i);
        if (Rnd(s) % 3 == 0) { e[0] = -1; e[1] = Rnd(s) % 2 ? -1 : int32(Rnd(s)); }
        else { e[0] = PoolRef(objPool, Rnd(s) % N_OBJ); e[1] = PoolRef(objPool, Rnd(s) % N_OBJ); }
    }
    const uint32 a = Rnd(s) % N_OBJ, b = Rnd(s) % N_OBJ;
    if (Rnd(s) % 3 == 0) { auto* e = reinterpret_cast<int32*>(CONNECT_LODS + 8 * (Rnd(s) % 10)); e[0] = PoolRef(objPool, a); e[1] = PoolRef(objPool, b); }   // el par ya existe
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(void*, void*)>(addr)(ObjAt(a), ObjAt(b));
        json o = Base(0); Diag(o, "lods", CONNECT_LODS, 80); return o;
    });
}
inline void FillLodWorld(uint32& s, bool allValid) {
    ClearAllFakes(); BuildRanges(); InstallPools(s, 85);
    for (uint32 i = 0; i < 10; ++i) {
        auto* e = reinterpret_cast<int32*>(CONNECT_LODS + 8 * i);
        if (!allValid && Rnd(s) % 4 == 0) { e[0] = -1; e[1] = Rnd(s) % 2 ? -1 : PoolRef(objPool, Rnd(s) % N_OBJ); }
        else {
            uint32 a, b; do { a = Rnd(s) % N_OBJ; } while (objPool.states[a] & 0x80); do { b = Rnd(s) % N_OBJ; } while (objPool.states[b] & 0x80);
            e[0] = PoolRef(objPool, a); e[1] = PoolRef(objPool, b);
        }
    }
    for (uint32 i = 0; i < N_OBJ; ++i) { At<uint32>(ObjAt(i), 0x30) = Rnd(s); At<uint8>(ObjAt(i), 0x34) = uint8(Rnd(s)); }
}
inline uint32 LodWorldHash() { return uint32(Fnv(objStore.data(), objStore.size())); }
inline json CaseDoScriptSetup(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x100D);
    // al menos una ranura usada por pool (el bucle de relleno de FillLodWorld necesita ranuras ocupadas)
    do { FillLodWorld(s, false); } while (false);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        json o = Base(0); o["objects"] = LodWorldHash(); Diag(o, "lods", CONNECT_LODS, 80); return o;
    });
}
inline json CaseConnectLods(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x100E);
    FillLodWorld(s, true);
    uint32 a, b; do { a = Rnd(s) % N_OBJ; } while (objPool.states[a] & 0x80); do { b = Rnd(s) % N_OBJ; } while (objPool.states[b] & 0x80);
    const int32 r1 = PoolRef(objPool, a), r2 = PoolRef(objPool, b);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32, int32)>(addr)(r1, r2);
        json o = Base(0); o["objects"] = LodWorldHash(); return o;
    });
}

// =====================================================================================================================
// Ids de cosas de script
// =====================================================================================================================
// Fija un fixture aleatorio para el tipo `type` y devuelve el indice elegido
inline uint32 SeedThing(uint32 type, uint32& s, uint32& storedId, bool& used) {
    storedId = PickId(s); used = Rnd(s) % 4 != 0;
    uint32 idx = 0;
    switch (type) {
    case 0: { idx = Rnd(s) % 16; FillRandom(SPHERES, 16 * 0x18, s); auto* e = reinterpret_cast<uint8*>(SPHERES + 0x18 * idx); e[0] = used; At<uint16>(e, 2) = uint16(storedId); for (uint32 i = 0; i < 16; ++i) reinterpret_cast<uint8*>(SPHERES)[0x18 * i] &= 1; break; }
    case 1: { idx = Rnd(s) % 32; FillRandom(EFFECTS, 32 * 8, s); auto* e = reinterpret_cast<uint8*>(EFFECTS + 8 * idx); e[0] = used; At<uint16>(e, 2) = uint16(storedId); for (uint32 i = 0; i < 32; ++i) reinterpret_cast<uint8*>(EFFECTS)[8 * i] &= 1; break; }
    case 2: { idx = Rnd(s) % 8; FillRandom(SEARCHLIGHTS, 8 * 0x7C, s); auto* e = reinterpret_cast<uint8*>(SEARCHLIGHTS + 0x7C * idx); e[0] = used; At<uint16>(e, 4) = uint16(storedId); for (uint32 i = 0; i < 8; ++i) reinterpret_cast<uint8*>(SEARCHLIGHTS)[0x7C * i] &= 1; break; }
    case 3: { idx = Rnd(s) % 20; FillRandom(CHECKPOINTS, 20 * 8, s); auto* e = reinterpret_cast<uint8*>(CHECKPOINTS + 8 * idx); e[0] = used; At<uint16>(e, 2) = uint16(storedId); for (uint32 i = 0; i < 20; ++i) reinterpret_cast<uint8*>(CHECKPOINTS)[8 * i] &= 1; break; }
    case 4: { idx = Rnd(s) % 64; FillRandom(SEQUENCES, 64 * 4, s); auto* e = reinterpret_cast<uint8*>(SEQUENCES + 4 * idx); e[0] = used; At<uint16>(e, 2) = uint16(storedId); for (uint32 i = 0; i < 64; ++i) reinterpret_cast<uint8*>(SEQUENCES)[4 * i] &= 1; break; }
    case 5: { idx = Rnd(s) % 60; FillRandom(FIRES, 60 * 0x28, s); auto* e = reinterpret_cast<uint8*>(FIRES + 0x28 * idx); e[0] = uint8(Rnd(s)); if (used) e[0] |= 2; else e[0] &= ~2; At<uint16>(e, 2) = uint16(storedId); break; }
    case 6: { idx = Rnd(s) % 64; FillRandom(FX2D_IDS, 0xC0, s); for (uint32 i = 0; i < 64; ++i) reinterpret_cast<uint8*>(FX2D_ACTIVE)[i] &= 1; Mem<uint16>(FX2D_IDS + 2 * idx) = uint16(storedId); Mem<uint8>(FX2D_ACTIVE + idx) = used; break; }
    case 7: { idx = Rnd(s) % 20; FillRandom(DM_IDS, 0x40, s); for (uint32 i = 0; i < 20; ++i) reinterpret_cast<uint8*>(DM_ACTIVE)[i] &= 1; Mem<uint16>(DM_IDS + 2 * idx) = uint16(storedId); Mem<uint8>(DM_ACTIVE + idx) = used; break; }
    default: { idx = Rnd(s) % 8; FillRandom(PEDGROUP_IDS, 0x18, s); for (uint32 i = 0; i < 8; ++i) reinterpret_cast<uint8*>(PEDGROUP_ACTIVE)[i] &= 1; Mem<uint16>(PEDGROUP_IDS + 2 * idx) = uint16(storedId); Mem<uint8>(PEDGROUP_ACTIVE + idx) = used; break; }
    }
    return idx;
}
inline json CaseGetActual(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x100F);
    const uint32 type = I % 10 == 9 ? 9 + Rnd(s) % 3 : I % 9;                    // 0..8 y algun tipo fuera de rango (el EXE devuelve -1)
    uint32 stored = 0; bool used = false;
    const uint32 idx = type <= 8 ? SeedThing(type, s, stored, used) : 0;
    uint32 id = Rnd(s) % 3 ? stored : PickId(s);
    int32 ref = int32((id << 16) | idx);
    if (Rnd(s) % 12 == 0) ref = -1;
    return Run(hook, cand, addr, [&] {
        const int32 r = reinterpret_cast<int32(__cdecl*)(int32, uint32)>(addr)(ref, type);
        return Base(uint32(r));
    });
}
inline json CaseGetNewUnique(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1010);
    const uint32 type = I % 10 == 9 ? 9 + Rnd(s) % 3 : I % 9;
    uint32 stored = 0; bool used = false;
    const uint32 idx = type <= 8 ? SeedThing(type, s, stored, used) : 0;
    return Run(hook, cand, addr, [&] {
        const int32 r = reinterpret_cast<int32(__cdecl*)(int32, uint32)>(addr)(int32(idx), type);
        json o = Base(uint32(r));
        Diag(o, "spheres", SPHERES, 16 * 0x18); Diag(o, "effects", EFFECTS, 256); Diag(o, "lights", SEARCHLIGHTS, 8 * 0x7C); Diag(o, "cps", CHECKPOINTS, 160); Diag(o, "seq", SEQUENCES, 256);
        Diag(o, "fires", FIRES, 60 * 0x28); Diag(o, "fx2d", FX2D_IDS, 0xC0); Diag(o, "dm", DM_IDS, 0x40); Diag(o, "groups", PEDGROUP_IDS, 0x18);
        return o;
    });
}
inline json CaseGetUnique(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1011);
    FillRandom(PEDGROUP_IDS, 0x18, s);
    const uint32 type = Rnd(s) % 4 == 0 ? Rnd(s) % 12 : 8; const int32 group = int32(Rnd(s) % 8);
    return Run(hook, cand, addr, [&] { return Base(uint32(reinterpret_cast<int32(__cdecl*)(int32, uint32)>(addr)(group, type))); });
}

// ---- esferas, sistemas de efectos, checkpoints
inline json CaseAddEffect(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1012);
    FillRandom(EFFECTS, 256, s); for (uint32 i = 0; i < 32; ++i) reinterpret_cast<uint8*>(EFFECTS)[8 * i] = (Rnd(s) % 3 != 0) ? 1 : 0;
    reinterpret_cast<uint8*>(EFFECTS)[8 * (Rnd(s) % 32)] = 0;     // al menos un hueco
    const uint32 fxp = Rnd(s);
    return Run(hook, cand, addr, [&] {
        const uint32 r = reinterpret_cast<uint32(__cdecl*)(uint32)>(addr)(fxp);
        json o = Base(r); Diag(o, "effects", EFFECTS, 256); return o;
    });
}
inline json CaseRemoveEffect(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1013); uint32 stored; bool used; const uint32 idx = SeedThing(1, s, stored, used);
    const int32 ref = Rnd(s) % 4 ? int32((stored << 16) | idx) : int32((PickId(s) << 16) | (Rnd(s) % 32));
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32)>(addr)(ref);
        json o = Base(0); Diag(o, "effects", EFFECTS, 256); return o;
    });
}
inline json CaseAddSphere(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1014);
    FillRandom(SPHERES, 16 * 0x18, s); for (uint32 i = 0; i < 16; ++i) reinterpret_cast<uint8*>(SPHERES)[0x18 * i] = (Rnd(s) % 3 != 0) ? 1 : 0;
    reinterpret_cast<uint8*>(SPHERES)[0x18 * (Rnd(s) % 16)] = 0;
    const uint32 id = Rnd(s) % 1000; const float x = PickWorld(s), y = PickWorld(s), z = PickWorld(s), r = float(Rnd(s) % 400) / 8.f;
    return Run(hook, cand, addr, [&] {
        const uint32 ret = reinterpret_cast<uint32(__cdecl*)(uint32, float, float, float, float)>(addr)(id, x, y, z, r);
        json o = Base(ret); Diag(o, "spheres", SPHERES, 16 * 0x18); return o;
    });
}
inline json CaseRemoveSphere(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1015); uint32 stored; bool used; const uint32 idx = SeedThing(0, s, stored, used);
    const int32 ref = Rnd(s) % 4 ? int32((stored << 16) | idx) : int32((PickId(s) << 16) | (Rnd(s) % 16));
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(int32)>(addr)(ref); json o = Base(0); Diag(o, "spheres", SPHERES, 16 * 0x18); return o; });
}
inline json CaseAddCheckpoint(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1016);
    ClearAllFakes(); BuildRanges();
    FillRandom(CHECKPOINTS, 160, s); for (uint32 i = 0; i < 20; ++i) reinterpret_cast<uint8*>(CHECKPOINTS)[8 * i] = (Rnd(s) % 3 != 0) ? 1 : 0;
    reinterpret_cast<uint8*>(CHECKPOINTS)[8 * (Rnd(s) % 20)] = 0;
    Mem<uint16>(NUM_CHECKPOINTS) = uint16(Rnd(s) % 20);
    const float v[7] = { PickWorld(s), PickWorld(s), PickWorld(s), PickWorld(s), PickWorld(s), PickWorld(s), float(Rnd(s) % 200) / 4.f }; const uint32 type = Rnd(s) % 9;
    return Run(hook, cand, addr, [&] {
        const uint32 ret = reinterpret_cast<uint32(__cdecl*)(float, float, float, float, float, float, float, uint32)>(addr)(v[0], v[1], v[2], v[3], v[4], v[5], v[6], type);
        json o = Base(ret); Diag(o, "cps", CHECKPOINTS, 160); Diag(o, "count", NUM_CHECKPOINTS, 2); return o;
    });
}
inline json CaseRemoveCheckpoint(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1017); ClearAllFakes(); BuildRanges();
    uint32 stored; bool used; const uint32 idx = SeedThing(3, s, stored, used);
    Mem<uint16>(NUM_CHECKPOINTS) = uint16(1 + Rnd(s) % 20);
    At<uint32>(reinterpret_cast<uint8*>(CHECKPOINTS), 8 * idx + 4) = Rnd(s) % 3 ? P(checkpointStore.data()) : 0u;
    At<uint16>(checkpointStore.data(), 0) = uint16(Rnd(s) % 9); At<uint32>(checkpointStore.data(), 4) = Rnd(s);
    const int32 ref = Rnd(s) % 4 ? int32((stored << 16) | idx) : int32((PickId(s) << 16) | (Rnd(s) % 20));
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(int32)>(addr)(ref); json o = Base(0); Diag(o, "cps", CHECKPOINTS, 160); Diag(o, "count", NUM_CHECKPOINTS, 2); return o; });
}

// =====================================================================================================================
// Focos de busqueda (con referencias a entidades falsas)
// =====================================================================================================================
inline void FillSearchLights(uint32& s, bool leaveFree) {
    ClearAllFakes(); BuildRanges();
    FillRandom(SEARCHLIGHTS, 8 * 0x7C, s);
    for (uint32 i = 0; i < 8; ++i) {
        uint8* e = reinterpret_cast<uint8*>(SEARCHLIGHTS) + 0x7C * i;
        e[0] = (Rnd(s) % 3 != 0) ? 1 : 0; e[1] &= 1; e[2] &= 1;
        for (size_t off : { 0x44u, 0x48u, 0x4Cu, 0x50u, 0x54u }) At<uint32>(e, off) = Rnd(s) % 3 == 0 ? 0u : P(EntAt(Rnd(s) % N_ENT));
        for (size_t off : { 0x08u, 0x0Cu, 0x10u, 0x14u, 0x18u, 0x1Cu, 0x20u, 0x24u, 0x28u, 0x2Cu, 0x30u, 0x34u, 0x38u, 0x3Cu, 0x40u, 0x58u, 0x5Cu, 0x60u, 0x64u, 0x68u, 0x6Cu, 0x70u, 0x74u, 0x78u }) At<float>(e, off) = PickWorld(s);
        At<uint16>(e, 4) = PickId(s);
    }
    if (leaveFree) reinterpret_cast<uint8*>(SEARCHLIGHTS)[0x7C * (Rnd(s) % 8)] = 0;
    Mem<uint16>(NUM_SEARCHLIGHTS) = uint16(1 + Rnd(s) % 8);
}
inline json LightsOut(uint64 ret) { json o = Base(ret); Diag(o, "lights", SEARCHLIGHTS, 8 * 0x7C); Diag(o, "count", NUM_SEARCHLIGHTS, 2); return o; }
inline json CaseAddSearchLight(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1018); FillSearchLights(s, true);
    const float v[8] = { PickWorld(s), PickWorld(s), PickWorld(s), PickWorld(s), PickWorld(s), PickWorld(s), float(Rnd(s) % 400) / 8.f, float(Rnd(s) % 400) / 8.f };
    void* ent = Rnd(s) % 5 == 0 ? nullptr : EntAt(Rnd(s) % N_ENT);
    return Run(hook, cand, addr, [&] {
        const uint32 r = reinterpret_cast<uint32(__cdecl*)(float, float, float, void*, float, float, float, float, float)>(addr)(v[0], v[1], v[2], ent, v[3], v[4], v[5], v[6], v[7]);
        return LightsOut(r);
    });
}
inline json CaseRemoveSearchLight(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1019); FillSearchLights(s, false);
    const uint32 idx = Rnd(s) % 8; reinterpret_cast<uint8*>(SEARCHLIGHTS)[0x7C * idx] = 1;
    const uint32 stored = At<uint16>(reinterpret_cast<uint8*>(SEARCHLIGHTS) + 0x7C * idx, 4);
    const int32 ref = Rnd(s) % 4 ? int32((stored << 16) | idx) : int32((PickId(s) << 16) | (Rnd(s) % 8));
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(int32)>(addr)(ref); return LightsOut(0); });
}
inline json CaseAttachSearchLight(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x101A); FillSearchLights(s, false);
    const uint32 idx = Rnd(s) % 8; reinterpret_cast<uint8*>(SEARCHLIGHTS)[0x7C * idx] = 1;
    const uint32 stored = At<uint16>(reinterpret_cast<uint8*>(SEARCHLIGHTS) + 0x7C * idx, 4);
    const int32 ref = Rnd(s) % 4 ? int32((stored << 16) | idx) : int32((PickId(s) << 16) | (Rnd(s) % 8));
    void* t = ObjAt(Rnd(s) % N_OBJ); void* h = ObjAt(Rnd(s) % N_OBJ); void* b = ObjAt(Rnd(s) % N_OBJ);
    const float ox = PickWorld(s), oy = PickWorld(s), oz = PickWorld(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32, void*, void*, void*, float, float, float)>(addr)(ref, t, h, b, ox, oy, oz);
        return LightsOut(0);
    });
}

// =====================================================================================================================
// Intercambio de edificios, invisibilidad, espera de cerebros de script
// =====================================================================================================================
inline json CaseAddBuildingSwap(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x101B); ClearAllFakes(); BuildRanges();
    for (uint32 i = 0; i < 25; ++i) {
        auto* e = reinterpret_cast<int32*>(BUILDING_SWAPS + 12 * i);
        if (Rnd(s) % 3 == 0) { e[0] = 0; e[1] = -1; e[2] = -1; } else { e[0] = int32(P(BldAt(Rnd(s) % N_BLD))); e[1] = int32(Rnd(s) % 20000); e[2] = int32(Rnd(s) % 20000); }
    }
    const uint32 bi = Rnd(s) % N_BLD; uint8* b = BldAt(bi); At<uint8>(b, 0x2E) = Rnd(s) % 4 == 0 ? uint8(1 + Rnd(s) % 5) : 0;
    const int32 oldM = int32(Rnd(s) % 20000), newM = Rnd(s) % 3 == 0 ? Mem<int32>(BUILDING_SWAPS + 8) : int32(Rnd(s) % 20000);
    if (Rnd(s) % 2) { Mem<int32>(BUILDING_SWAPS + 12 * (Rnd(s) % 25)) = int32(P(b)); }
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(void*, int32, int32)>(addr)(b, oldM, newM);
        json o = Base(0); Diag(o, "swaps", BUILDING_SWAPS, 300); return o;
    });
}
inline json CaseUndoBuildingSwaps(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x101C); ClearAllFakes(); BuildRanges();
    for (uint32 i = 0; i < 25; ++i) {
        auto* e = reinterpret_cast<int32*>(BUILDING_SWAPS + 12 * i);
        if (Rnd(s) % 2 == 0) { e[0] = 0; e[1] = int32(Rnd(s)); e[2] = int32(Rnd(s)); } else { e[0] = int32(P(BldAt(Rnd(s) % N_BLD))); e[1] = int32(Rnd(s) % 20000); e[2] = int32(Rnd(s) % 20000); }
    }
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)()>(addr)(); json o = Base(0); Diag(o, "swaps", BUILDING_SWAPS, 300); return o; });
}
inline json CaseAddInvisibility(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x101D); ClearAllFakes(); BuildRanges();
    for (uint32 i = 0; i < 20; ++i) Mem<uint32>(INVISIBLE + 4 * i) = Rnd(s) % 3 == 0 ? 0u : P(EntAt(Rnd(s) % N_ENT));
    if (Rnd(s) % 5 == 0) for (uint32 i = 0; i < 20; ++i) Mem<uint32>(INVISIBLE + 4 * i) = P(EntAt(i));   // lleno
    uint8* e = EntAt(Rnd(s) % N_ENT); At<uint8>(e, 0x2E) = Rnd(s) % 5 == 0 ? 3 : 0;
    if (Rnd(s) % 2) Mem<uint32>(INVISIBLE + 4 * (Rnd(s) % 20)) = P(e);
    const bool visible = Rnd(s) & 1;
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*, bool)>(addr)(e, visible); json o = Base(0); Diag(o, "array", INVISIBLE, 80); return o; });
}
inline json CaseUndoInvisibility(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x101E); ClearAllFakes(); BuildRanges();
    for (uint32 i = 0; i < N_ENT; ++i) At<uint32>(EntAt(i), 0x1C) = Rnd(s);
    for (uint32 i = 0; i < 20; ++i) Mem<uint32>(INVISIBLE + 4 * i) = Rnd(s) % 3 == 0 ? 0u : P(EntAt(Rnd(s) % N_ENT));
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        json o = Base(0); Diag(o, "array", INVISIBLE, 80); o["ents"] = uint32(Fnv(entStore.data(), entStore.size())); return o;
    });
}
inline void FillWaiting(uint32& s) {
    ClearAllFakes(); BuildRanges();
    for (uint32 i = 0; i < 150; ++i) {
        Mem<uint32>(WAITING + 8 * i) = Rnd(s) % 3 == 0 ? 0u : P(EntAt(Rnd(s) % N_ENT));
        Mem<int16>(WAITING + 8 * i + 4) = Rnd(s) % 3 == 0 ? int16(-1) : int16(Rnd(s) % 70);
        Mem<int16>(WAITING + 8 * i + 6) = int16(Rnd(s));
    }
}
inline json CaseAddWaiting(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x101F); FillWaiting(s);
    if (Rnd(s) % 4 == 0) for (uint32 i = 0; i < 150; ++i) Mem<uint32>(WAITING + 8 * i) = P(EntAt(i));    // lleno (no hay hueco: la funcion no hace nada)
    void* e = Rnd(s) % 3 ? EntAt(Rnd(s) % N_ENT) : EntAt(Mem<uint32>(WAITING + 8 * (Rnd(s) % 150)) ? 0 : 3);
    if (Rnd(s) % 3 == 0) Mem<uint32>(WAITING + 8 * (Rnd(s) % 150)) = P(e);
    const int16 idx = int16(Rnd(s) % 70);
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*, int16)>(addr)(e, idx); json o = Base(0); Diag(o, "waiting", WAITING, 1200); return o; });
}
inline json CaseRemoveWaiting(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x1020); FillWaiting(s);
    void* e = EntAt(Rnd(s) % N_ENT); const int16 idx = int16(Rnd(s) % 3 == 0 ? -1 : int32(Rnd(s) % 70));
    for (uint32 k = 0; k < 4; ++k) { const uint32 slot = Rnd(s) % 150; Mem<uint32>(WAITING + 8 * slot) = P(e); Mem<int16>(WAITING + 8 * slot + 4) = Rnd(s) & 1 ? idx : int16(Rnd(s) % 70); }
    return Run(hook, cand, addr, [&] { reinterpret_cast<void(__cdecl*)(void*, int16)>(addr)(e, idx); json o = Base(0); Diag(o, "waiting", WAITING, 1200); return o; });
}
}
