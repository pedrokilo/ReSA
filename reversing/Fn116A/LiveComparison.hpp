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
#include "Collision/BoundingBox.h"
#include "Collision/ColBox.h"
#include "Pad.h"
#include "StuckCarCheck.h"
#include "Events/EventDamage.h"
#include "Garage.h"
#include "OnscreenTimer.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"


// Banco par a par Fn116A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dieciocho funciones (hook S), cada una con 16384 pares EXE/DLL (ver Fixtures.hpp para la ABI de cada una):
//   0 CBoundingBox::IsPointWithin 0x49C300   1 CColBox::Set 0x40EE70   2 CColBox::operator= 0x40EEC0   3 tColLighting::GetCurrentLighting 0x59F0C0   4 CPad::sub_540530 0x540530
//   5 FightHitLevel 0x5BD360   6 FightHitSound 0x5BD3B0   7 approxEqual 0x4EEA80   8 ConvertPedNode2BoneTag 0x4D58A0   9 CStuckCarCheck::ResetArrayElement 0x463970
//   10 CEventDamage::ComputeHeadShot 0x4ADD60   11 FindCutsceneAudioTrackId 0x5AFA50   12 CSimpleTransform::Invert 0x54EF90   13 TransformVectors 0x54EE30
//   14 CGarages::IsPointWithinAnyGarage 0x448990   15 CGarages::IsPointInAGarageCameraZone 0x449BA0   16 IsTeleportPointValid 0x43EA90   17 COnscreenTimer::Init 0x44CBC0
// Colaboradores (con hook RH): 0x448740 (CGarage::IsPointInsideGarage(punto), funcion 14), 0x4487D0 (IsPointInsideGarage(punto, radio), funcion 15), 0x569E20 (CWorld::TestSphereAgainstWorld) y 0x56A490
// (CWorld::GetIsLineOfSightClear) (funcion 16). Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (los colaboradores tienen hook RH, apagado durante el banco). Registran sus argumentos EN ORDEN.
// NO se sustituyen (corren de verdad en ambos lados): toda la logica de las funciones (incluidos fsin/fcos de Invert y TransformVectors, y la tabla de nombres del EXE de FindCutsceneAudioTrackId).
// Memoria real del juego (se rellena y se restaura tras cada lado): el balance dia/noche 0x8D12C0 y la tabla de garajes 0x96C048 (50 x 0xD8 bytes; solo se cambia el byte de tipo +0x4C).

namespace F116ALayout {
static_assert(offsetof(CBoundingBox, m_vecMin) == 0 && offsetof(CBoundingBox, m_vecMax) == 0xC && sizeof(CBoundingBox) == 0x18, "CBoundingBox");
static_assert(offsetof(CColBox, m_Surface) == 0x18 && sizeof(CColBox) == 0x1C, "CColBox");
static_assert(offsetof(CPad, Mode) == 0x10A && offsetof(CPad, NewState) == 0 && offsetof(CPad, OldState) == 0x30, "CPad");
static_assert(sizeof(tStuckCar) == 0x24 && sizeof(CStuckCarCheck) == 0x240, "CStuckCarCheck");
static_assert(offsetof(CEventDamage, m_weaponType) == 0x1C && offsetof(CEventDamage, m_pedPieceType) == 0x20 && offsetof(CEventDamage, m_damageResponse) + offsetof(CPedDamageResponse, m_bForceDeath) == 0x41, "CEventDamage");
static_assert(offsetof(CGarage, m_nType) == 0x4C && sizeof(CGarage) == 0xD8, "CGarage");
static_assert(sizeof(COnscreenTimer) == 0x154, "COnscreenTimer");
}

namespace F116ALive {
using namespace F116AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x49C300, 0x40EE70, 0x40EEC0, 0x59F0C0, 0x540530, 0x5BD360, 0x5BD3B0, 0x4EEA80, 0x4D58A0, 0x463970, 0x4ADD60, 0x5AFA50, 0x54EF90, 0x54EE30, 0x448990, 0x449BA0, 0x43EA90, 0x44CBC0};
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F116AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(int64 off = 0) const { return uintptr_t(int64(m.data()) + int64(kGuard) + off); }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(int64 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(int64 off, u32 v) { const u16 w = u16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(int64 off, u32 v) { m[size_t(int64(kGuard) + off)] = uint8(v); }
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
inline Arena<kObjWin> objA;          // 0: objeto principal (caja, pad, lista de coches, evento, temporizador, transformacion destino)
inline Arena<kA1Win> a1A;            // 1: punto / sup / base / origen
inline Arena<kA2Win> a2A;            // 2: inf / destino
inline Arena<kInWin> inA;            // 3: vectores de entrada de TransformVectors
inline Arena<kOutWin> outA;          // 4: vectores de salida de TransformVectors
inline Arena<kStrWin> strA;          // 5: cadenas

// Estado del caso en curso para los falsos
struct State { const C* c{}; };
inline State g;

// Eventos: 50 IsPointInsideGarage(punto) 0x448740 [indice, x, y, z]; 51 IsPointInsideGarage(punto, radio) 0x4487D0 [indice, x, y, z, radio]; 52 CWorld::TestSphereAgainstWorld 0x569E20 [x, y, z, radio, ignorar, 6 booleanos];
// 53 CWorld::GetIsLineOfSightClear 0x56A490 [etiqueta origen, etiqueta destino, origen xyz, destino xyz, 7 booleanos]
inline u32 GarageIndex(void* self) { return u32((Addr(self) - kGarageBase) / kGarageSize); }
// Los bucles del EXE guardan el puntero del garaje en EDX (0x448990) y en ECX (0x449BA0) a traves de la llamada: el colaborador real los conserva, asi que los falsos van envueltos en un
// adaptador que guarda ECX y EDX (el codigo C++ de un falso los clobbera). Solo del banco.
inline u32 __cdecl FkGaragePointC(void* self, u32 x, u32 y, u32 z) {
    const u32 idx = GarageIndex(self);
    Ev& e = PushEv(50, 4); e.v[0] = idx; e.v[1] = x; e.v[2] = y; e.v[3] = z;
    const u64 mask = u64(g.c->a[13]) | (u64(g.c->a[14]) << 32);
    return u32((mask >> idx) & 1u) | (g.c->a[18] & 0xFFFFFF00u);
}
inline u32 __cdecl FkGarageRadiusC(void* self, u32 x, u32 y, u32 z, u32 radius) {
    const u32 idx = GarageIndex(self);
    Ev& e = PushEv(51, 5); e.v[0] = idx; e.v[1] = x; e.v[2] = y; e.v[3] = z; e.v[4] = radius;
    const u64 mask = u64(g.c->a[13]) | (u64(g.c->a[14]) << 32);
    return u32((mask >> idx) & 1u) | (g.c->a[18] & 0xFFFFFF00u);
}
// 0x448740 (thiscall, ECX = garaje, x, y, z en la pila, `ret 0Ch`)
__declspec(naked) inline void FkGaragePoint() {
    __asm {
        push edx
        push ecx
        push dword ptr [esp+0x14]
        push dword ptr [esp+0x14]
        push dword ptr [esp+0x14]
        push ecx
        call FkGaragePointC
        add esp, 16
        pop ecx
        pop edx
        ret 0Ch
    }
}
// 0x4487D0 (thiscall, ECX = garaje, x, y, z y radio en la pila, `ret 10h`)
__declspec(naked) inline void FkGarageRadius() {
    __asm {
        push edx
        push ecx
        push dword ptr [esp+0x18]
        push dword ptr [esp+0x18]
        push dword ptr [esp+0x18]
        push dword ptr [esp+0x18]
        push ecx
        call FkGarageRadiusC
        add esp, 20
        pop ecx
        pop edx
        ret 10h
    }
}
inline u32 __cdecl FkTestSphere(u32 x, u32 y, u32 z, u32 radius, u32 ignore, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5, u32 b6) {   // 0x569E20
    Ev& e = PushEv(52, 11);
    e.v[0] = x; e.v[1] = y; e.v[2] = z; e.v[3] = radius; e.v[4] = ignore; e.v[5] = b1; e.v[6] = b2; e.v[7] = b3; e.v[8] = b4; e.v[9] = b5; e.v[10] = b6;
    return g.c->a[6];
}
inline u32 __cdecl FkLineOfSight(const u32* origin, const u32* target, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5, u32 b6, u32 b7) {   // 0x56A490
    Ev& e = PushEv(53, 15);
    e.v[0] = Tag(origin); e.v[1] = Tag(target);
    for (int k = 0; k < 3; ++k) { e.v[2 + k] = origin[k]; e.v[5 + k] = target[k]; }
    e.v[8] = b1; e.v[9] = b2; e.v[10] = b3; e.v[11] = b4; e.v[12] = b5; e.v[13] = b6; e.v[14] = b7;
    return g.c->a[7];
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

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        objA.Fill(sd++); a1A.Fill(sd++); a2A.Fill(sd++); inA.Fill(sd++); outA.Fill(sd++); strA.Fill(sd++);
        TagClear();
        TagAdd(objA.m.data(), objA.m.size());   // 0
        TagAdd(a1A.m.data(), a1A.m.size());     // 1
        TagAdd(a2A.m.data(), a2A.m.size());     // 2
        TagAdd(inA.m.data(), inA.m.size());     // 3
        TagAdd(outA.m.data(), outA.m.size());   // 4
        TagAdd(strA.m.data(), strA.m.size());   // 5
        g = State{}; g.c = &c; nev = 0;
        std::vector<u32> args;
        u32 ecx = u32(objA.Obj()), popBytes = 0;
        switch (fn) {
        case 0:   // CBoundingBox::IsPointWithin(&punto)
            for (unsigned k = 0; k < 6; ++k) objA.Put32(4 * k, c.a[k]);
            for (unsigned k = 0; k < 3; ++k) a1A.Put32(4 * k, c.a[6 + k]);
            args = { u32(a1A.Obj()) };
            break;
        case 1: { // CColBox::Set(&sup, &inf, material, pieza, iluminacion)
            for (unsigned k = 0; k < 3; ++k) { a1A.Put32(4 * k, c.a[k]); a2A.Put32(4 * k, c.a[3 + k]); }
            const u32 sup = (c.a[9] & 1) ? u32(objA.Obj(int32(c.a[10]))) : u32(a1A.Obj());
            const u32 inf = (c.a[9] & 2) ? u32(objA.Obj(int32(c.a[11]))) : u32(a2A.Obj());
            args = { sup, inf, c.a[6], c.a[7], c.a[8] };
            break;
        }
        case 2:   // CColBox::operator=(&origen)
            args = { c.a[0] ? u32(objA.Obj(int32(c.a[1]))) : u32(a1A.Obj()) };
            break;
        case 3:   // tColLighting::GetCurrentLighting(iluminacion, factor)
            G32(kBalanceAddr) = c.a[2];
            args = { c.a[0], c.a[1] };
            popBytes = 8;
            ecx = c.seed;
            break;
        case 4:   // CPad::sub_540530()
            objA.Put16(0x10A, c.a[0]); objA.Put16(0x10, c.a[1]); objA.Put16(0x12, c.a[2]); objA.Put16(0x1A, c.a[3]);
            objA.Put16(0x40, c.a[4]); objA.Put16(0x42, c.a[5]); objA.Put16(0x4A, c.a[6]);
            break;
        case 5:   // FightHitLevel(char*)
            for (unsigned k = 0; k < 4; ++k) strA.Put8(c.a[4] + k, c.a[k]);
            args = { u32(strA.Obj(c.a[4])) };
            popBytes = 4;
            ecx = c.seed;
            break;
        case 6:   // FightHitSound(n)
            args = { c.a[0] };
            popBytes = 4;
            ecx = c.seed;
            break;
        case 7:   // approxEqual(f1, f2, epsilon)
            args = { c.a[0], c.a[1], c.a[2] };
            popBytes = 12;
            ecx = c.seed;
            break;
        case 8:   // ConvertPedNode2BoneTag(nodo)
            args = { c.a[0] };
            popBytes = 4;
            ecx = c.seed;
            break;
        case 9:   // CStuckCarCheck::ResetArrayElement(indice)
            args = { c.a[0] };
            break;
        case 10:  // CEventDamage::ComputeHeadShot(&bool)
            objA.Put32(0x1C, c.a[0]); objA.Put32(0x20, c.a[1]); objA.Put8(0x41, c.a[2]);
            a1A.Put8(0, c.a[3]);
            args = { u32(a1A.Obj()) };
            break;
        case 11:  // FindCutsceneAudioTrackId(char*)
            for (unsigned k = 0; k < 6; ++k) strA.Put32(4 * k, c.a[4 + k]);
            args = { u32(strA.Obj()) };
            popBytes = 4;
            ecx = c.seed;
            break;
        case 12: { // CSimpleTransform::Invert(&base)
            if (c.a[4]) { for (unsigned k = 0; k < 4; ++k) objA.Put32(4 * k, c.a[k]); args = { u32(objA.Obj()) }; }
            else { for (unsigned k = 0; k < 4; ++k) a1A.Put32(4 * k, c.a[k]); args = { u32(a1A.Obj()) }; }
            break;
        }
        case 13: { // TransformVectors(salida, cuenta, &transformacion, entrada)
            a1A.Put32(0xC, c.a[1]);
            for (unsigned k = 0; k < 18; ++k) inA.Put32(4 * k, c.a[4 + k]);
            args = { c.a[2] ? u32(inA.Obj()) : u32(outA.Obj()), c.a[0], u32(a1A.Obj()), u32(inA.Obj()) };
            popBytes = 16;
            ecx = c.seed;
            break;
        }
        case 14: case 15: { // garajes
            for (unsigned k = 0; k < kGarageCount; ++k) *reinterpret_cast<uint8*>(kGarageBase + k * kGarageSize + 0x4C) = uint8(c.a[k / 4] >> (8 * (k % 4)));
            for (unsigned k = 0; k < 3; ++k) a1A.Put32(4 * k, c.a[15 + k]);
            if (fn == 14) { args = { u32(a1A.Obj()) }; popBytes = 4; }
            else { args = { c.a[15], c.a[16], c.a[17] }; popBytes = 12; }
            ecx = c.seed;
            break;
        }
        case 16:  // IsTeleportPointValid(&origen, &destino)
            for (unsigned k = 0; k < 3; ++k) { a1A.Put32(4 * k, c.a[k]); a2A.Put32(4 * k, c.a[3 + k]); }
            args = { u32(a1A.Obj()), u32(a2A.Obj()) };
            popBytes = 8;
            ecx = c.seed;
            break;
        default:  // COnscreenTimer::Init()
            break;
        }
        objA.Snap(); a1A.Snap(); a2A.Snap(); inA.Snap(); outA.Snap(); strA.Snap();
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        json obs;
        json ch = json::array();
        auto add = [&](unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); };
        add(0, objA.Changes()); add(1, a1A.Changes()); add(2, a2A.Changes()); add(3, inA.Changes()); add(4, outA.Changes()); add(5, strA.Changes());
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        // valor devuelto (lo que lee algun llamador): AL (bool), AX (id), EAX completo (enteros y this) o ST0 (funcion 3, comparada en la imagen x87)
        const u32 eax = sides[s].reg.eax;
        switch (fn) {
        case 0: case 4: case 5: case 7: case 14: case 15: case 16: obs["ret"] = eax & 0xFFu; break;
        case 2: obs["ret"] = Tag(uintptr_t(eax)); break;
        case 6: case 8: obs["ret"] = eax; break;
        case 11: obs["ret"] = eax & 0xFFFFu; break;
        default: obs["ret"] = nullptr; break;
        }
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
    if (!std::filesystem::exists(dir / "fn116a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn116a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn116a"); }
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
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    for (unsigned f = 0; f < kFunctions; ++f) env.stubs[f] = {};
    // La DLL no llama a los cuerpos de los hooks de IsPointInsideGarage (envoltorios con registros del EXE) sino a los metodos de CGarage: se parchean tambien, solo en la DLL.
    {
        using PointFn = bool (CGarage::*)(CVector);
        using RadiusFn = bool (CGarage::*)(CVector, float);
        static_assert(sizeof(PointFn) == sizeof(uintptr_t) && sizeof(RadiusFn) == sizeof(uintptr_t));
        const PointFn pointFn = &CGarage::IsPointInsideGarage;
        const RadiusFn radiusFn = &CGarage::IsPointInsideGarage;
        uintptr_t pointAddr{}, radiusAddr{};
        memcpy(&pointAddr, &pointFn, sizeof(pointAddr));
        memcpy(&radiusAddr, &radiusFn, sizeof(radiusAddr));
        env.dllOnly.push_back(S(Pair{0, Body(pointAddr), true}, reinterpret_cast<void*>(&FkGaragePoint)));
        env.dllOnly.push_back(S(Pair{0, Body(radiusAddr), true}, reinterpret_cast<void*>(&FkGarageRadius)));
    }
    env.stubs[14] = {F(0x448740, &FkGaragePoint)};
    env.stubs[15] = {F(0x4487D0, &FkGarageRadius)};
    env.stubs[16] = {F(0x569E20, &FkTestSphere), F(0x56A490, &FkLineOfSight)};
    const char* dllReal[kFunctions] = {
        "seis comparaciones ordenadas del punto con la caja (x87 del EXE frente a las comparaciones de la DLL)",
        "copia de sup, inf y los tres bytes de superficie",
        "copia de los dos vectores, la palabra de material y pieza y el byte de iluminacion",
        "mezcla dia/noche con el ayudante x87 ColLightingCurrentCpp (ST0 sin guardar como float)",
        "lectura del modo y de los estados del pad",
        "seleccion por el primer caracter",
        "tabla de sonidos con comparacion sin signo",
        "resta redondeada a 24 bits con exponente ampliado, valor absoluto y comparacion ordenada (ApproxEqualCpp)",
        "tabla de nodos del ped a tags de hueso",
        "escritura de los campos de la entrada de coche",
        "clasificacion por arma y comprobacion de pieza",
        "busqueda por nombre sin distinguir mayusculas en la tabla de la DLL (la del EXE es otra copia)",
        "fcos/fsin sin redondear y productos a 24 bits (SimpleTransformInvertCpp) y negacion de z y rumbo",
        "fcos/fsin redondeados a float y el bucle x87 (CommonTransformVectorsCpp)",
        "recorrido de la tabla de garajes por tipo; IsPointInsideGarage es un falso identico",
        "recorrido de la tabla de garajes por tipo; IsPointInsideGarage con radio es un falso identico",
        "TestSphereAgainstWorld y GetIsLineOfSightClear son falsos identicos",
        "escritura de los campos del reloj y de los cuatro contadores"};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
