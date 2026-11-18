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

// Banco par a par FnF1A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 10 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias
// (A, B, C y D con guardas de 0x40). Colaboradores REALES con el hook de la DLL apagado: elementos y listas de los vectores (0x617850, 0x6E3E30, 0x4A8DB0..0x4A8DE0), CMatrix (0x59C050, 0x59C890, 0x59ACD0),
// TextBufferAppend (0x508930) y DecodeDxt5AlphaBlock (0x81F0C0). Falsos solo en la direccion del EXE (la DLL los llama por ella): 0x508B80, 0x508D30 y 0x509190, que anotan el texto acumulado.
// Memoria global del juego tocada (restaurada tras cada lado): Scene.m_pRwCamera (0xC1703C), a una camara falsa de la arena B.
// Se observan: los bytes cambiados de las cuatro arenas (en las descompresoras DXT, su cuenta y un FNV; con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria
// global, los eventos de los falsos, el valor devuelto y el estado x87. EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace FF1ALive {
using namespace FF1AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();
// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = FF1AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
    // Resumen de los cambios para las funciones que escriben muchos bytes: [cuenta, FNV-1a de (desplazamiento de 4 bytes LE, byte nuevo) de cada byte cambiado, en orden]
    json ChangeHash() const {
        u32 h = 2166136261u, count = 0;
        for (size_t i = 0; i < N; ++i) {
            if (m[i] == r[i]) continue;
            const u32 off = u32(int64(i) - int64(kGuard));
            for (int k = 0; k < 4; ++k) { h ^= (off >> (8 * k)) & 0xFFu; h *= 16777619u; }
            h ^= m[i]; h *= 16777619u; ++count;
        }
        return json::array({count, h});
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores de BugstarWriteScreenShotEntry
inline DWORD gBenchThread{};
struct TextRaw { const u8* data; u32 length; };
inline std::array<u8, 4096> gBlob{};
inline u32 gBlobLen{};
inline u32 TextHash(const void* text) { const auto* t = static_cast<const TextRaw*>(text); return t->data ? Fnv(t->data, t->length) : Fnv("", 0); }
inline u32 TextLen(const void* text) { return static_cast<const TextRaw*>(text)->length; }
// 0x508B80 (thiscall, un entero, `ret 4`): evento [direccion, ECX (etiqueta: la cadena local es de pila), entero, hash del texto acumulado, longitud]
inline void __fastcall Fk508B80(void* self, void*, u32 value) {
    Ev& e = PushEv(0x508B80, 4);
    e.v[0] = Tag(self); e.v[1] = value; e.v[2] = TextHash(self); e.v[3] = TextLen(self);
}
// 0x508D30 (thiscall, clave y tipo, `ret 8`): evento [direccion, ECX, hash de la clave, hash del tipo, hash del texto acumulado, longitud]
inline void __fastcall Fk508D30(void* self, void*, const char* key, const char* type) {
    Ev& e = PushEv(0x508D30, 5);
    e.v[0] = Tag(self); e.v[1] = FnvStr(key); e.v[2] = FnvStr(type); e.v[3] = TextHash(self); e.v[4] = TextLen(self);
}
// 0x509190 (thiscall, tres argumentos, `ret 0xC`): evento [direccion, ECX (etiqueta de arena), primer argumento, hash de los datos, longitud]; guarda una copia de los datos para el registro
inline void __fastcall Fk509190(void* self, void*, u32 zero, const u8* data, u32 length) {
    Ev& e = PushEv(0x509190, 4);
    e.v[0] = Tag(self); e.v[1] = zero; e.v[2] = data ? Fnv(data, length) : Fnv("", 0); e.v[3] = length;
    gBlobLen = length < gBlob.size() ? length : u32(gBlob.size());
    if (data && gBlobLen) memcpy(gBlob.data(), data, gBlobLen);
}

// ================================================================================================ cierre de un lado
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

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

inline void PutString(Arena<kWin>& ar, u32 off, u32 len, u32 seed) {
    for (u32 i = 0; i < len; ++i) ar.m[kGuard + off + i] = u8(1 + Rand(seed, i) % 255);
    ar.m[kGuard + off + len] = 0;
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gBlobLen = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kIk: case kWc: case kNop:
        break;
    case kCam:
        for (u32 i = 0; i < 16; ++i) arenaB.Put32(a[28] + 0x20 + 4 * i, a[11 + i]);
        arenaB.Put32(a[28] + 0x80, a[8]); arenaB.Put32(a[28] + 0x84, a[9]);
        arenaC.Put32(a[27], a[4]); arenaC.Put32(a[27] + 4, a[5]); arenaC.Put32(a[27] + 8, a[6]);
        *reinterpret_cast<u32*>(kScenePtr) = u32(arenaB.Obj(a[28]));
        call.args[0] = u32(arenaC.Obj(a[27])); call.args[1] = a[7]; call.args[2] = a[10]; call.argCount = 3; call.popBytes = 12;
        break;
    case kBug:
        PutString(arenaC, a[8], a[11], a[13]);
        PutString(arenaC, a[7], a[12], a[13] + 1);
        call.args = {a[4], a[5], a[6], u32(arenaC.Obj(a[7])), u32(arenaC.Obj(a[8])), u32(arenaD.Obj(a[9])), a[10], 0};
        call.argCount = 7; call.popBytes = 0;
        break;
    case kDxt1: case kDxt3: case kDxt5: {
        const i32 w = i32(a[4]), h = i32(a[5]);
        const i32 bpr = w / 4, bh = h / 4;
        const u32 bytes = kDxtBlockBytes[kKind[c.fn] - kDxt1];
        const u32 nBlocks = (bpr > 0 && bh > 0) ? u32(bpr * bh) : 0u;
        for (u32 j = 0; j < nBlocks; ++j) {
            if (!((a[8] >> (j % 32)) & 1u)) continue;
            u8* blk = arenaC.m.data() + kGuard + a[7] + j * bytes;
            u8* col = blk + (kKind[c.fn] == kDxt1 ? 0 : 8);
            col[2] = col[0]; col[3] = col[1];
            if (kKind[c.fn] == kDxt5) blk[1] = blk[0];
        }
        call.args = {a[4], a[5], u32(arenaB.Obj(a[6])), u32(arenaC.Obj(a[7])), 0, 0, 0, 0}; call.argCount = 4; call.popBytes = 16;
        break;
    }
    case kAlpha:
        if (a[8] & 1u) arenaC.m[kGuard + a[7] + 1] = arenaC.m[kGuard + a[7]];
        call.args = {u32(arenaB.Obj(a[6])), u32(arenaC.Obj(a[7])), a[4], a[5], 0, 0, 0, 0}; call.argCount = 4; call.popBytes = 16;
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    const bool hashed = kKind[fn] == kDxt1 || kKind[fn] == kDxt3 || kKind[fn] == kDxt5 || kKind[fn] == kAlpha;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        if (hashed) obs["chash"] = {{"A", arenaA.ChangeHash()}, {"B", arenaB.ChangeHash()}, {"C", arenaC.ChangeHash()}, {"D", arenaD.ChangeHash()}};
        else obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kIk: case kWc: obs["ret"] = Tag(uintptr_t(eax)); break;      // this como etiqueta de arena
        case kCam: obs["ret"] = eax & 0xFFu; break;                        // bool: solo AL
        case kDxt1: obs["ret"] = eax; break;
        default: obs["ret"] = nullptr; break;                              // void
        }
        if (kKind[fn] == kBug) obs["blob"] = HexBytes(gBlob.data(), gBlobLen);
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FNF1A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FNF1A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fnf1a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fnf1a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fnf1a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (0x508B80, 0x508D30 y 0x509190 de BugstarWriteScreenShotEntry): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook RH en la DLL (apagado durante el banco): el EXE corre sus bytes y la DLL su cuerpo
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    env.stubs[5] = {ByExeAddress(0x508B80, &Fk508B80), ByExeAddress(0x508D30, &Fk508D30), ByExeAddress(0x509190, &Fk509190)};
    for (unsigned f : {0u, 1u}) env.reals[f] = {Real(0x617850), Real(0x4A8DB0), Real(0x4A8DC0), Real(0x4A8DD0), Real(0x4A8DE0)};
    for (unsigned f : {2u, 3u}) env.reals[f] = {Real(0x6E3E30), Real(0x4A8DB0), Real(0x4A8DC0), Real(0x4A8DD0), Real(0x4A8DE0)};
    env.reals[4] = {Real(0x59C050), Real(0x59C890), Real(0x59ACD0)};
    env.reals[5] = {Real(0x508930)};
    env.reals[8] = {Real(0x81F0C0)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (los colaboradores con hook corren reales con el hook apagado)";
    dllReal[5] = "toda la funcion salvo 0x508B80, 0x508D30 y 0x509190 (falsos solo en la direccion del EXE: la DLL los llama por ella); TextBufferAppend (0x508930), sprintf, operator new y delete corren reales";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: Scene.m_pRwCamera (IsPointInCameraDepthRange); se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kScenePtr, 4); regionsOf[4] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
