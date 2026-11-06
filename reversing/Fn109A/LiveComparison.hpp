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
#include "SetPiece.h"
#include "Cam.h"
#include "Events/EventDamage.h"
#include "VehicleScanner.h"
#include "PedScanner.h"
#include "PedGeometryAnalyser.h"
#include "PedClothesDesc.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn109A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dieciseis funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la ABI de cada una).
// Colaboradores: CPointRoute::operator new llama a CPointRoutePool::New (0x41B5B0, con hook RH): corre de verdad en ambos lados (en el EXE el original, con el hook apagado; en la DLL su cuerpo).
// SetTextureAndModel llama a CClothes::GetTextureDependency (0x5A7EA0, hook RH): real en ambos lados (conserva ECX/EDX, de lo que depende el EXE). La version por nombre llama ademas a
// CKeyGen::GetUppercaseKey (0x53CF30, hook RH): falso identico en la direccion del EXE y en el cuerpo de la DLL (registra el puntero etiquetado y el FNV del texto).
// Memoria real del juego (se rellena y se restaura tras cada lado): las 14 palabras de IsLampPost y el puntero del pool de CPointRoute (0xB744B0, apuntado a un pool propio en la arena).

namespace F109ALayout {
static_assert(offsetof(CSetPiece, m_SpawnCoord1) == 0xC && offsetof(CSetPiece, m_SpawnCoord2) == 0x10 && offsetof(CSetPiece, m_TargetCoord1) == 0x14 && offsetof(CSetPiece, m_TargetCoord2) == 0x18 && sizeof(CSetPiece) == 0x20, "CSetPiece");
static_assert(offsetof(CCam, m_fHorizontalAngle) == 0xBC && offsetof(CCam, m_pCamTargetEntity) == 0x21C, "CCam");
static_assert(offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_aWeapons) == 0x5A0 && sizeof(CWeapon) == 0x1C && offsetof(CWeapon, m_IsFirstPersonWeaponModeSelected) == 0x14, "CPed");
static_assert(offsetof(CEventDamage, m_bAddToEventGroup) == 0xC && offsetof(CEventDamage, m_damageResponse) + offsetof(CPedDamageResponse, m_bHealthZero) == 0x40, "CEventDamage");
static_assert(offsetof(CPedClothesDesc, m_anModelKeys) == 0 && offsetof(CPedClothesDesc, m_anTextureKeys) == 0x28 && sizeof(CPointRoute) == 0x64, "CPedClothesDesc");
}

namespace F109ALive {
using namespace F109AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4997A0, 0x4997E0, 0x499820, 0x499860, 0x509C50, 0x509DC0, 0x509A30, 0x509760, 0x4DC670, 0x4ABCA0, 0x5FFEE0, 0x5FFF20, 0x41B7C0, 0x41B7D0, 0x5A8050, 0x5A8080};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F109AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            const int64 off = int64(i) - int64(kGuard);
            if (m[i] != r[i]) out.push_back(json::array({off, m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> A0, A1, A2, A3, A4, A5, A6;   // 0 objeto principal, 1 ped / vector B, 2 salida, 3 cabecera del pool, 4 estados del pool, 5 objetos del pool, 6 cadenas

// Estado del caso en curso para los falsos
struct State { const C* c{}; };
inline State g;

// Eventos: 50 CKeyGen::GetUppercaseKey 0x53CF30 [puntero etiquetado, FNV del texto]
inline u32 __cdecl FkKey(const char* s) {
    Ev& e = PushEv(50, 2); e.v[0] = Tag(s); e.v[1] = FnvStr(s);
    return Mix(FnvStr(s) ^ 0x5A5A1234u);
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

inline void Put3(Arena<kWin>& arena, size_t off, const u32* p) { arena.Put32(off, p[0]); arena.Put32(off + 4, p[1]); arena.Put32(off + 8, p[2]); }
inline void FillString(Arena<kWin>& arena, size_t off, u32 len, u32 seed) {
    static constexpr char kChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    for (u32 i = 0; i < len; ++i) arena.Put8(off + i, uint8(kChars[Rand(seed, i) % (sizeof(kChars) - 1)]));
    arena.Put8(off + len, 0);
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        A0.Fill(sd++); A1.Fill(sd++); A2.Fill(sd++); A3.Fill(sd++); A4.Fill(sd++); A5.Fill(sd++); A6.Fill(sd++);
        TagClear();
        TagAdd(A0.m.data(), A0.m.size());   // 0
        TagAdd(A1.m.data(), A1.m.size());   // 1
        TagAdd(A2.m.data(), A2.m.size());   // 2
        TagAdd(A3.m.data(), A3.m.size());   // 3
        TagAdd(A4.m.data(), A4.m.size());   // 4
        TagAdd(A5.m.data(), A5.m.size());   // 5
        TagAdd(A6.m.data(), A6.m.size());   // 6
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(A0.Obj()), popBytes = 0;
        switch (fn) {
        case 0: case 1: case 2: case 3: {
            const size_t off = 0xC + 4 * fn;
            A0.Put16(off, c.a[0]); A0.Put16(off + 2, c.a[1]);
            args = { u32(A2.Obj()) };
            break;
        }
        case 4: A0.Put32(0xBC, c.a[0]); break;
        case 5: {
            A0.Put32(0x21C, c.a[0] == 0 ? u32(A1.Obj()) : 0u);
            A1.Put8(0x36, c.a[1]);
            A1.Put8(0x718, c.a[2]);
            const int slot = int(int8_t(uint8(c.a[2])));
            A1.Put8(size_t(0x5A0 + 0x14 + slot * 0x1C), c.a[3]);
            break;
        }
        case 6: {
            for (u32 k = 0; k < 14; ++k) { const uint16 w = uint16(c.a[k]); memcpy(reinterpret_cast<void*>(kLampWords[k]), &w, 2); }
            args = { c.a[14] }; popBytes = 4;
            break;
        }
        case 7: case 8:
            Put3(A0, 0, &c.a[0]); Put3(A1, 0, &c.a[3]);
            args = { u32(A0.Obj()), u32(A1.Obj()) }; popBytes = 8;
            break;
        case 9: A0.Put8(0x40, c.a[0]); A0.Put8(0x0C, c.a[1]); break;
        case 10: case 11: A0.Put32(0x4C, c.a[0]); break;
        case 12: case 13: {
            A3.Put32(0, u32(A5.Obj())); A3.Put32(4, u32(A4.Obj())); A3.Put32(8, c.a[0]); A3.Put32(0xC, c.a[1]);
            A3.Put8(0x10, c.a[20] & 0xFF); A3.Put8(0x11, (c.a[20] >> 8) & 0xFF);
            for (u32 k = 0; k < 16; ++k) A4.Put8(k, c.a[2 + k]);
            G32(kPoolPtrAddr) = u32(A3.Obj());
            if (fn == 12) args = { c.a[18] };
            else args = { u32(A5.Obj() + c.a[18] * 100u + c.a[19]) };
            popBytes = 4;
            break;
        }
        case 14: {
            A0.Put32(0, c.a[3]);   // el resto de las claves ya es basura de la arena
            args = { c.a[0], c.a[1], c.a[2] };
            break;
        }
        default: {
            FillString(A6, 0, c.a[3], c.a[5]); FillString(A6, 0x20, c.a[4], c.a[6]);
            if (c.a[0] == 2) A6.Put8(0, 0);
            if (c.a[1] == 2) A6.Put8(0x20, 0);
            args = { c.a[0] == 1 ? 0u : u32(A6.Obj(0)), c.a[1] == 1 ? 0u : u32(A6.Obj(0x20)), c.a[2] };
            break;
        }
        }
        A0.Snap(); A1.Snap(); A2.Snap(); A3.Snap(); A4.Snap(); A5.Snap(); A6.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, A0.Changes()); add(1, A1.Changes()); add(2, A2.Changes()); add(3, A3.Changes()); add(4, A4.Changes()); add(5, A5.Changes()); add(6, A6.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        if (fn <= 3 || fn == 12) obs["ret"] = Tag(eax);
        else if (fn == 10 || fn == 11) obs["ret"] = eax;
        else if (fn >= 5 && fn <= 9) obs["al"] = (eax & 0xFFu) != 0 ? 1 : 0;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
        for (const auto& s : env.stubs[f]) {
            if (s.pair.hooked) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
            else ready &= s.pair.dll == 0 || ModuleOf(s.pair.dll) == env.asi;
        }
        for (const auto& p : env.real[f]) ready &= p.dll != 0 && ModuleOf(p.dll) == env.asi;
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

// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn109a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn109a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn109a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name;
        const bool found = FindHook(kEntries[f], cat, name);
        if (found) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto C_ = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    // Colaboradores que corren de verdad (hook apagado en el EXE; en la DLL, su cuerpo)
    env.real[12] = {C_(0x41B5B0)};
    env.real[14] = {C_(0x5A7EA0)};
    env.real[15] = {C_(0x5A7EA0)};
    // Colaborador falseado: CKeyGen::GetUppercaseKey (falso identico en la direccion del EXE y en el cuerpo de la DLL)
    env.stubs[15] = {StubSpec{C_(0x53CF30), reinterpret_cast<void*>(&FkKey)}};
    const char* dllReal[kFunctions] = {
        "lectura de las dos palabras con signo y conversion a float del par (x * 0.25, y * 0.25), puntero de salida en EAX",
        "igual", "igual", "igual",
        "comparaciones con +-pi y resta/suma de 2pi sobre el float +0xBC",
        "tipo de entidad, ranura activa con signo y byte de bandera del arma",
        "comparacion con las 14 palabras de indice de modelo leidas sin signo",
        "comparacion exacta de tres componentes", "comparacion exacta de tres componentes",
        "dos bytes del evento", "dword +0x4C del escaner", "dword +0x4C del escaner",
        "CPointRoutePool::New real en ambos lados (0x41B5B0: el original en el EXE, el cuerpo propio en la DLL), sobre un pool de arena",
        "marcado de la ranura como vacia y cursor del pool (cuerpo propio de la DLL), sobre un pool de arena",
        "CClothes::GetTextureDependency real en ambos lados; escrituras de claves sin comprobar el rango de la parte",
        "CClothes::GetTextureDependency real en ambos lados y CKeyGen::GetUppercaseKey falso identico; escrituras de claves sin comprobar el rango"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& p : env.real[f]) {
            env.watches.emplace_back(p.exe, 16);
            real.push_back({{"exe", p.exe}, {"dll", p.dll != 0}, {"hooked", p.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
