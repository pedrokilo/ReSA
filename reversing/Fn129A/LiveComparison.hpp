#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn129A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Catorce funciones (hook S = RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall: enlace de la entrada hacia la DLL; la 1 es V = RH_ScopedVMTInstall), cada una con 16384 pares
// EXE/DLL (ver Fixtures.hpp para la lista, los datos de cada caso y el esquema de familias).
// El banco invoca la direccion del EXE con objetos sinteticos (arenas propias, `this` en un desplazamiento alineado, desalineado o al borde de la ventana).
// Falsos de los colaboradores con hook RH (apagado durante el banco): el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab). Colaboradores sin hook que la DLL llama por la
// direccion del EXE (operator delete 0x8214BD y delete[] 0x8213AE, RenderWare 0x7FB020/0x7FB230/0x7F35D0): un solo parche. Colaboradores REALES: se ejecutan tal cual (0x821D30, 0x56CDE0).
// Se observan: los bytes cambiados de las arenas y de las dos regiones globales, los eventos de los falsos (colaborador, argumentos como etiquetas de arena o crudos), el valor de retorno (EAX como
// etiqueta o crudo, AL) y el estado x87 completo (el oraculo comprueba ST0 de 80 bits en la funcion 2).

namespace F129ALive {
using namespace F129AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x463370, 0x4B25E0, 0x4DA510, 0x41BDD0, 0x4E0380, 0x56CE30, 0x4EE9A0, 0x40F700, 0x6C4130, 0x4049C0, 0x43AA40, 0x705480, 0x70F890, 0x40F030};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F129AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(int64 off = 0) const { return uintptr_t(m.data()) + kGuard + size_t(off); }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Pattern(u32 kind, u32 seed) { if (kind == 0) Fill(seed); else memset(m.data(), kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), N); }
    void Snap() { r = m; }
    void Put32(int64 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(int64 off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(int64 off, u8 v) { m[size_t(int64(kGuard) + off)] = v; }
    u32 Get32(int64 off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kObjWin> objA;          // 0: objeto (this)
inline Arena<kAWin> aA;              // 1: arena secundaria (matrices, vectores, camara, evento, punteros)
inline Arena<kBWin> bA;              // 2: arena terciaria (rasters)
inline Arena<kSlotWin> sA;           // 3: slots de bancos de sonido (funcion 4)

// ================================================================================================ falsos de los colaboradores
// Datos del caso en curso para los falsos
inline u32 gTypeThis{}, gTypeOther{};
inline uintptr_t gVtThis[4]{}, gVtOther[4]{};
inline uintptr_t gCamera{};
inline u32 gNewRaster{};
inline DWORD gBenchThread{};

// Funcion 1: ranura 1 (GetEventType) de las vtables falsas de this y del otro evento; evento [1 = this / 2 = otro, objeto (etiqueta), tipo devuelto]
inline u32 __fastcall FkTypeThis(void* self, void*) {
    Ev& e = PushEv(1, 2);
    e.v[0] = Tag(self);
    e.v[1] = gTypeThis;
    return gTypeThis;
}
inline u32 __fastcall FkTypeOther(void* self, void*) {
    Ev& e = PushEv(2, 2);
    e.v[0] = Tag(self);
    e.v[1] = gTypeOther;
    return gTypeOther;
}
// Funciones 7 y 9: colaboradores con un puntero (cdecl o thiscall): [direccion del EXE, puntero (etiqueta o crudo)]. No leen la memoria apuntada.
template<u32 ADDR> inline void __cdecl FkPtrC(void* p) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(p);
}
template<u32 ADDR> inline void __fastcall FkPtrT(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
}
// Funcion 11 (RenderWare): se anota ademas el frameBuffer de la camara (+0x60) en el momento de la llamada
inline u32 CameraFb() { return BitsOf(reinterpret_cast<const void*>(gCamera + 0x60)); }
inline void __cdecl FkRasterDestroy(void* r) {
    Ev& e = PushEv(0x7FB020, 2);
    e.v[0] = Tag(r);
    e.v[1] = Tag(uintptr_t(CameraFb()));
}
inline void* __cdecl FkRasterCreate(int w, int h, int depth, int flags) {
    Ev& e = PushEv(0x7FB230, 5);
    e.v[0] = u32(w); e.v[1] = u32(h); e.v[2] = u32(depth); e.v[3] = u32(flags);
    e.v[4] = Tag(uintptr_t(CameraFb()));
    return reinterpret_cast<void*>(uintptr_t(gNewRaster));
}
inline void* __cdecl FkTextureSetRaster(void* tex, void* r) {
    Ev& e = PushEv(0x7F35D0, 3);
    e.v[0] = Tag(tex);
    e.v[1] = Tag(r);
    e.v[2] = Tag(uintptr_t(CameraFb()));
    return tex;
}

inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// Puntero de un caso segun su modo: 0 nulo, 1 arena (+ desplazamiento), 2 crudo
template<class A> inline u32 PtrOf(const A& arena, u32 mode, u32 raw, int64 off) { return mode == 0 ? 0u : mode == 1 ? u32(arena.Obj(off)) : raw; }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Pattern(c.a[2], sd++); aA.Fill(sd++); bA.Fill(sd++);
        if (fn == 4) sA.Fill(sd++);
        const int64 off = c.a[0];
        uintptr_t self = 0;
        u32 args[2]{};
        u32 argCount = 0, popBytes = 0;
        std::array<uint8, kConversationsBytes> convBefore{};
        switch (fn) {
        case 0: {
            self = objA.Obj(off);
            for (int i = 0; i < 6; ++i) objA.Put32(off - 8 + 4 * i, c.a[6 + i]);
            uintptr_t src;
            if (c.a[12] == 0) { aA.Put32(c.a[13], c.a[15]); src = aA.Obj(c.a[13]); }
            else src = uintptr_t(int64(self) + int32(c.a[13]));
            args[0] = u32(src); argCount = 1;
            break;
        }
        case 1:
            self = objA.Obj(off);
            gVtThis[1] = uintptr_t(&FkTypeThis); gVtOther[1] = uintptr_t(&FkTypeOther);
            gTypeThis = c.a[6]; gTypeOther = c.a[7];
            objA.Put32(off, u32(uintptr_t(gVtThis)));
            aA.Put32(c.a[8], u32(uintptr_t(gVtOther)));
            args[0] = u32(aA.Obj(c.a[8])); argCount = 1;
            break;
        case 2:
            *reinterpret_cast<i32*>(kSurfaceTableAddr + (c.a[6] & 0xFFu) * 16u) = i32(c.a[0]);
            args[0] = c.a[6]; argCount = 1; popBytes = 4;
            break;
        case 3:
            self = objA.Obj(off);
            objA.Put8(off + 0x428, u8(c.a[6])); objA.Put8(off + 0x42D, u8(c.a[7]));
            args[0] = c.a[8]; argCount = 1;
            break;
        case 4: {
            self = objA.Obj(off);
            objA.Put32(off, u32(sA.Obj(int64(kSlotBase))));
            objA.Put8(off + 0x14, u8(c.a[9]));
            const int64 slot = int16(c.a[7] & 0xFFFFu);
            const int64 sound = c.a[8] & 0xFFFFu;
            sA.Put32(int64(kSlotBase) + slot * int64(kSlotSize) + (sound + 2) * 12, c.a[6]);
            args[0] = c.a[8]; args[1] = c.a[7]; argCount = 2;
            break;
        }
        case 5:
            self = objA.Obj(off);
            for (u32 e = 0; e < 16; ++e) {
                const bool active = ((c.a[9] >> e) & 1) != 0;
                objA.Put32(off + 8 * e, active ? u32((e + 1) * c.a[6]) : 0u);
                objA.Put8(off + 8 * e + 4, active ? u8((e + 3) * c.a[7]) : 0);
                objA.Put8(off + 8 * e + 5, active ? u8((e + 5) * c.a[8]) : 0);
            }
            break;
        case 6:
            self = objA.Obj(off);
            objA.Put32(off, c.a[6]); objA.Put32(off + 4, c.a[7]);
            objA.Put8(off + 0x808, u8(c.a[8])); objA.Put8(off + 0x809, u8(c.a[9]));
            objA.Put16(off + 0x80A, u16(c.a[10])); objA.Put16(off + 0x80C, u16(c.a[11]));
            objA.Put32(off + 0x810, c.a[12]); objA.Put32(off + 0x818, c.a[13]);
            break;
        case 7:
            self = objA.Obj(off);
            objA.Put8(off + 0x29, u8(c.a[6]));
            objA.Put32(off + 0x2C, PtrOf(aA, c.a[7], c.a[8], c.a[9]));
            break;
        case 8:
            self = objA.Obj(off);
            objA.Put32(off + 0x14, u32(aA.Obj(c.a[7])));
            for (int k = 0; k < 11; ++k) aA.Put32(int64(c.a[7]) + 4 * k, c.a[8]);
            args[0] = c.a[6]; argCount = 1;
            break;
        case 9:
            self = objA.Obj(off);
            objA.Put32(off, c.a[6]); objA.Put32(off + 4, c.a[7]); objA.Put32(off + 8, c.a[8]); objA.Put32(off + 0xC, c.a[9]); objA.Put8(off + 0x10, u8(c.a[10]));
            break;
        case 10: {
            uint8* conv = reinterpret_cast<uint8*>(kConversationsAddr);
            const u32 j = c.a[7], dup = c.a[8];
            for (u32 e = 0; e < 14; ++e) {
                uint8* ent = conv + 0x1C * e;
                FillWords(ent, 0x1C, c.seed + 77u * (e + 1));
                u32 key = Rand(c.seed + e, 55);
                if (key == c.a[0]) key ^= 1u;
                if (e == j || (dup != 14 && e >= dup)) key = c.a[0];
                memcpy(ent + 8, &key, 4);
                ent[0x18] = u8(c.a[9] + e * 37u);
            }
            memcpy(convBefore.data(), conv, kConversationsBytes);
            args[0] = c.a[0]; args[1] = c.a[6]; argCount = 2; popBytes = 8;
            break;
        }
        case 11: {
            self = objA.Obj(off);
            gCamera = aA.Obj(c.a[6]);
            objA.Put32(off, u32(gCamera));
            objA.Put32(off + 4, c.a[11] == 0 ? 0u : c.a[11] == 1 ? u32(aA.Obj(0x10)) : c.a[12]);
            aA.Put32(int64(c.a[6]) + 0x60, u32(bA.Obj(c.a[7])));
            bA.Put32(int64(c.a[7]) + 0xC, c.a[8]);
            gNewRaster = PtrOf(bA, c.a[9], c.a[10], c.a[13]);
            break;
        }
        case 12:
            self = aA.Obj(c.a[8]);
            for (int k = 0; k < 3; ++k) aA.Put32(int64(c.a[8]) + 4 * k, c.a[9]);
            for (int k = 0; k < 3; ++k) aA.Put32(int64(c.a[6]) + 4 * k, c.a[10 + k]);
            for (int k = 0; k < 3; ++k) aA.Put32(int64(c.a[7]) + 4 * k, c.a[13 + k]);
            args[0] = u32(aA.Obj(c.a[6])); args[1] = u32(aA.Obj(c.a[7])); argCount = 2;
            break;
        default:   // 13
            self = objA.Obj(off);
            objA.Put32(off, c.a[6]); objA.Put32(off + 4, c.a[7]);
            for (int k = 2; k < 12; ++k) objA.Put32(off + 4 * k, c.a[6]);
            break;
        }
        const u32 ecxSeed = (kHasThis[fn] || fn == 12) ? u32(self) : c.ecx;
        objA.Snap(); aA.Snap(); bA.Snap(); if (fn == 4) sA.Snap();
        TagClear(); TagAdd(objA.m.data(), objA.m.size()); TagAdd(aA.m.data(), aA.m.size()); TagAdd(bA.m.data(), bA.m.size()); TagAdd(sA.m.data(), sA.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, args, argCount, c.eax, ecxSeed, c.edx, c.stack, popBytes);
        });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        json fb = nullptr;
        if (fn == 11) {
            // el frameBuffer de la camara es un puntero a una arena (direccion desconocida para el oraculo): se anota como etiqueta y se excluye de la lista de bytes
            const int64 at = int64(c.a[6]) + 0x60;
            fb = Tag(uintptr_t(aA.Get32(at)));
            for (int k = 0; k < 4; ++k) aA.m[size_t(int64(kGuard) + at + k)] = aA.r[size_t(int64(kGuard) + at + k)];
        }
        add(0, objA.Changes()); add(1, aA.Changes()); add(2, bA.Changes()); if (fn == 4) add(3, sA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (fn) {
        case 4: obs["ret"] = eax; break;                                                              // valor crudo
        case 0: case 5: case 6: case 13: obs["ret"] = Tag(uintptr_t(eax)); break;                     // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;
        }
        obs["al"] = fn == 1 ? json(eax & 0xFFu) : json(nullptr);
        obs["fb"] = fb;
        if (fn == 10) {
            json g = json::array();
            const uint8* conv = reinterpret_cast<const uint8*>(kConversationsAddr);
            for (size_t i = 0; i < kConversationsBytes; ++i) if (conv[i] != convBefore[i]) g.push_back(json::array({int64(i), conv[i]}));
            obs["gchanges"] = g;
        } else obs["gchanges"] = nullptr;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", kCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
        }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

// Casilla de la IAT de un modulo para una funcion importada por nombre (0 si no esta)
inline uintptr_t* FindImportSlot(uintptr_t module, const char* func) {
    const auto* base = reinterpret_cast<const uint8*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    const auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress) return nullptr;
    for (const auto* d = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; ++d) {
        const auto* names = reinterpret_cast<const IMAGE_THUNK_DATA32*>(base + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* slots = reinterpret_cast<IMAGE_THUNK_DATA32*>(const_cast<uint8*>(base) + d->FirstThunk);
        for (; names->u1.AddressOfData; ++names, ++slots) {
            if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG32) continue;
            const auto* by = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + names->u1.AddressOfData);
            if (!std::strcmp(reinterpret_cast<const char*>(by->Name), func)) return reinterpret_cast<uintptr_t*>(&slots->u1.Function);
        }
    }
    return nullptr;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn129a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn129a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn129a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado (el Virtual apaga tambien el salto de la entrada del EXE).
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    auto Real = [](uintptr_t exe) { return Pair{exe, 0, false}; };
    env.stubs[0] = {};
    env.stubs[1] = {};
    env.stubs[2] = {};
    env.stubs[3] = {};
    env.stubs[4] = {};
    env.stubs[5] = {};                          env.reals[5] = {Real(0x821D30), Real(0x56CDE0)};
    env.stubs[6] = {};
    env.stubs[7] = {F(0x416400, &FkPtrC<0x416400>), F(0x72F430, &FkPtrC<0x72F430>), F(0x40F070, &FkPtrT<0x40F070>), F(0x8214BD, &FkPtrC<0x8214BD>)};
    env.stubs[8] = {};
    env.stubs[9] = {F(0x8213AE, &FkPtrC<0x8213AE>)};
    env.stubs[10] = {};
    env.stubs[11] = {F(0x7FB020, &FkRasterDestroy), F(0x7FB230, &FkRasterCreate), F(0x7F35D0, &FkTextureSetRaster)};
    env.stubs[12] = {};
    env.stubs[13] = {};
    const char* dllReal[kFunctions] = {
        "toda la funcion: cuatro copias de byte con carga y escritura alternadas (volatile)",
        "toda la funcion: tipo del otro evento (ranura 1 de la vtable falsa del otro) y despues el propio (ranura 1 de la vtable falsa de this); devuelve propio != otro",
        "toda la funcion: fild de la tabla * 0.01f en la x87 y 20 * log10 con un solo redondeo (IntTimesFloatCpp + AudioLog10Times20Cpp, X87Math)",
        "toda la funcion: bit 4 del byte +0x428 segun el motor roto (bit 1 de +0x42D) y el bit 0 del argumento",
        "toda la funcion: -1 sin inicializar; si no m_BankSlots[int16 slot].Sounds[uint16 sonido].LoopStartOffset sin comprobar indices",
        "toda la funcion: pone a cero m_pPed, m_bIsTreated y m_bIsRevived de las 16 entradas; el EXE llama al iterador CRT 0x821D30 y a 0x56CDE0 (reales)",
        "toda la funcion: el constructor reconstruido de CAESmoothFadeThread (campos +0, +4, +0x808, +0x809, +0x80A, +0x80C, +0x810 y +0x818)",
        "toda la funcion salvo CCollision::RemoveTrianglePlanes (0x416400), CMemoryMgr::Free (0x72F430), CCollisionData::RemoveCollisionVolumes (0x40F070), todos con hook (falsos identicos), y operator delete (0x8214BD, falso)",
        "toda la funcion: sin y cos del angulo (TrigSimpleSinCpp / TrigSimpleCosCpp de X87Math) y la matriz de rotacion alrededor de Z",
        "toda la funcion salvo delete[] (0x8213AE, falso): libera +0 y +4 si la capacidad es positiva y el byte +0x10 no es cero, y pone a cero +0, +4, +8 y +0xC",
        "toda la funcion: pone el byte m_Enabled de la primera conversacion con ese peaton (CConversations::FindConversationForPed)",
        "toda la funcion salvo RwRasterDestroy (0x7FB020), RwRasterCreate (0x7FB230) y RwTextureSetRaster (0x7F35D0), falsos",
        "toda la funcion: producto vectorial en la x87 de 24 bits con cada componente guardada antes de la siguiente (CrossProductInPlaceExe)",
        "toda la funcion: el constructor reconstruido de CCollisionData (bits bajos del byte +7 apagados, el resto a cero)"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
