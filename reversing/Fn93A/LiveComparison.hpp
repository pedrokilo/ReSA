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
#include "PathFind.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn93A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Funcion 0 (hook S): CCarPathLink::OneWayLaneOffset 0x44DB00 (thiscall: ECX = enlace; sin argumentos; float en ST0). Observa el valor de ST0 (10 bytes de la imagen FNSAVE).
// Funcion 1 (hook S): CPathFind::RemoveBadStartNode 0x44E4F0 (thiscall: ECX = this; posicion (3 dwords), nodos y contador en pila, `ret 14h`; void). `this` es una arena con m_pPathNodes en +0x804;
// CPathNode::GetPosition (0x420A10, hook activo) corre de verdad en ambos lados.
// Funcion 2 (hook S): CPathFind::StoreInteriorNodeLink 0x450FB0 (cdecl, 6 argumentos, sin this: usa las matrices globales de ThePaths 0x96F854..: el banco apunta las ranuras que usa a arenas y las restaura).
// Funcion 3 (hook S): CPathFind::CalcDistToAnyConnectingLinks 0x44F190 (thiscall: ECX = this; nodo y posicion en pila, `ret 10h`; float en ST0). Falso: CCollision::DistToLineSqr 0x412850 (cdecl, tres punteros;
// devuelve el resultado k del caso; registra sus argumentos). Los CPathNode::GetPosition corren de verdad.
// Funcion 4 (hook S): CPathFind::FindNodeClosestInRegion 0x44F2C0 (thiscall: ECX = this; 10 argumentos en pila, `ret 2Ch`; void). Falso: CalcDistToAnyConnectingLinks 0x44F190 (thiscall, nodo y posicion por valor en pila,
// `ret 10h`; devuelve el resultado k del caso; registra this, el nodo y la posicion).
// Colaboradores sustituidos por falsos identicos en la direccion del EXE y, como tienen hook RH en la DLL (apagado durante el banco), en su cuerpo del ASI.

namespace F93ALayout {
static_assert(offsetof(CPathFind, m_pPathNodes) == 0x804 && offsetof(CPathFind, m_pNaviNodes) == 0x924 && offsetof(CPathFind, m_pNodeLinks) == 0xA44 && offsetof(CPathFind, m_pLinkLengths) == 0xB64
              && offsetof(CPathFind, m_pPathIntersections) == 0xC84 && offsetof(CPathFind, m_anNumNodes) == 0xFA4 && offsetof(CPathFind, m_anNumVehicleNodes) == 0x10C4, "campos de CPathFind");
static_assert(sizeof(CPathNode) == 0x1C && sizeof(CCarPathLink) == 0xE, "tamanos de nodo y enlace");
}

namespace F93ALive {
using namespace F93AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x44DB00, 0x44E4F0, 0x450FB0, 0x44F190, 0x44F2C0};
inline constexpr uintptr_t kPathNodesAt = 0x96F854, kNodeLinksAt = 0x96FA94, kLinkLengthsAt = 0x96FBB4, kIntersectionsAt = 0x96FCD4;   // ThePaths (0x96F050) + 0x804, 0xA44, 0xB64, 0xC84
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uintptr_t Addr(auto p) { return reinterpret_cast<uintptr_t>(p); }

// ================================================================================================ arenas
template<size_t WIN> struct Arena {
    static constexpr size_t N = F93AFix::Arena(WIN);
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj(int64 off = 0) const { return uintptr_t(int64(uintptr_t(m.data())) + int64(kGuard) + off); }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(int64 off, u32 v) { memcpy(m.data() + ptrdiff_t(int64(kGuard) + off), &v, 4); }
    void Put16(int64 off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + ptrdiff_t(int64(kGuard) + off), &w, 2); }
    void Put8(int64 off, u32 v) { m[size_t(int64(kGuard) + off)] = uint8(v); }
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kLinkWin> linkA;
inline Arena<kThis1Win> this1A;
inline Arena<kNodesWin> nodesA;
inline Arena<kCountWin> countA;
inline std::array<Arena<kBankWin>, kBanks> bankA;
inline Arena<kLinks2Win> links2A;
inline Arena<kBytes2Win> lens2A, ints2A;
inline Arena<kCtrWin> ctrA;
inline Arena<kThis3Win> this3A;
inline Arena<kNodeWin> node3A;
inline Arena<kLinkArrWin> linkArrA;
inline Arena<kThis4Win> this4A;
inline Arena<kOutWin> outA, distA;

// Estado del caso en curso para los falsos
struct State { const C* c{}; unsigned calls{}; };
inline State g;
inline constexpr u32 kTgtArea[2] = {3u, 30u};     // areas destino de los enlaces de la funcion 3

// Eventos: 1 (funcion 3) CCollision::DistToLineSqr 0x412850 [inicio.xyz, fin.xyz, punto.xyz]; 1 (funcion 4) CalcDistToAnyConnectingLinks 0x44F190 [this, nodo, pos.xyz].
inline float __cdecl FkDistToLine(const float* a, const float* b, const float* p) {                    // 0x412850 (cdecl, tres punteros)
    Ev& e = PushEv(1, 9); memcpy(&e.v[0], a, 12); memcpy(&e.v[3], b, 12); memcpy(&e.v[6], p, 12);
    const u32 k = g.calls++;
    return F(g.c->dist[k < 16 ? k : 15]);
}
inline float __fastcall FkCalcDist(void* self, void*, void* node, float x, float y, float z) {       // 0x44F190 (ret 10h)
    Ev& e = PushEv(1, 5); e.v[0] = Tag(self); e.v[1] = Tag(node); e.v[2] = BitsOf(&x); e.v[3] = BitsOf(&y); e.v[4] = BitsOf(&z);
    const u32 k = g.calls++;
    return F(g.c->dist[k < 16 ? k : 15]);
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

inline void PutNodes(const C& c, unsigned nodes) {
    for (unsigned b = 0; b < kBanks; ++b)
        for (unsigned i = 0; i < nodes; ++i) {
            bankA[b].Put16(int64(i) * 0x1C + 8, u32(c.np[b][i][0]));
            bankA[b].Put16(int64(i) * 0x1C + 10, u32(c.np[b][i][1]));
            bankA[b].Put16(int64(i) * 0x1C + 12, u32(c.np[b][i][2]));
        }
}
inline void TagBanks() { for (unsigned b = 0; b < kBanks; ++b) TagAdd(bankA[b].m.data(), bankA[b].m.size()); }
inline void FillBanks(u32& sd) { for (unsigned b = 0; b < kBanks; ++b) bankA[b].Fill(sd++); }
inline void SnapBanks() { for (unsigned b = 0; b < kBanks; ++b) bankA[b].Snap(); }
inline void AddBanks(json& ch, unsigned first) {
    for (unsigned b = 0; b < kBanks; ++b) for (const auto& e : bankA[b].Changes()) ch.push_back(json::array({first + b, e[0], e[1]}));
}
inline void AddChanges(json& ch, unsigned idx, const json& list) { for (const auto& e : list) ch.push_back(json::array({idx, e[0], e[1]})); }
inline u32 BankPtr(unsigned b, bool loaded) { return loaded ? u32(bankA[b].Obj()) : 0u; }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        u32 sd = c.seed * 13u + 50u;
        TagClear();
        std::vector<u32> args;
        u32 ecx = 0, popBytes = 0;
        json ch = json::array();
        json obs;
        switch (fn) {
        case 0: {   // OneWayLaneOffset
            linkA.Fill(sd++);
            TagAdd(linkA.m.data(), linkA.m.size());
            linkA.Put8(0xA, c.width); linkA.Put8(0xB, c.lanes);
            linkA.Snap();
            ecx = u32(linkA.Obj());
            break;
        }
        case 1: {   // RemoveBadStartNode
            this1A.Fill(sd++); nodesA.Fill(sd++); countA.Fill(sd++); FillBanks(sd);
            TagAdd(this1A.m.data(), this1A.m.size()); TagAdd(nodesA.m.data(), nodesA.m.size()); TagAdd(countA.m.data(), countA.m.size()); TagBanks();
            PutNodes(c, 8);
            this1A.Put32(0x804 + 4 * int64(c.areaOf[0]), BankPtr(c.bank0, c.loaded0));
            this1A.Put32(0x804 + 4 * int64(c.areaOf[1]), BankPtr(c.bank1, c.loaded1));
            for (unsigned k = 0; k < 16; ++k) nodesA.Put32(4 * int64(k), (c.areaOf[k] & 0xFFFFu) | (c.idOf[k] << 16));
            countA.Put32(0, c.cnt);
            this1A.Snap(); nodesA.Snap(); countA.Snap(); SnapBanks();
            args = {c.pos[0], c.pos[1], c.pos[2], u32(nodesA.Obj()), u32(countA.Obj())};
            ecx = u32(this1A.Obj());
            break;
        }
        case 2: {   // StoreInteriorNodeLink
            FillBanks(sd); links2A.Fill(sd++); lens2A.Fill(sd++); ints2A.Fill(sd++); ctrA.Fill(sd++);
            TagBanks(); TagAdd(links2A.m.data(), links2A.m.size()); TagAdd(lens2A.m.data(), lens2A.m.size()); TagAdd(ints2A.m.data(), ints2A.m.size()); TagAdd(ctrA.m.data(), ctrA.m.size());
            PutNodes(c, 8);
            const u32 idx5 = 0x40 + c.a5;
            G32(kPathNodesAt + 4 * uintptr_t(c.a1)) = BankPtr(c.bankA, true);
            G32(kPathNodesAt + 4 * uintptr_t(c.a3)) = BankPtr(c.bankB, true);
            G32(kPathNodesAt + 4 * uintptr_t(idx5)) = BankPtr(c.bankF, true);
            G32(kNodeLinksAt + 4 * uintptr_t(idx5)) = u32(links2A.Obj());
            G32(kLinkLengthsAt + 4 * uintptr_t(idx5)) = u32(lens2A.Obj());
            G32(kIntersectionsAt + 4 * uintptr_t(idx5)) = u32(ints2A.Obj());
            // banderas iniciales del nodo A en el area de la ranura idx5 (su banco es el ultimo asignado: bankF), indice con signo
            const int64 ia = int64(std::int8_t(c.a2 & 0xFFu));
            bankA[c.bankF].Put8(ia * 0x1C + 0x18, c.flagInit);
            ctrA.Put32(0, c.ctr);
            SnapBanks(); links2A.Snap(); lens2A.Snap(); ints2A.Snap(); ctrA.Snap();
            args = {c.a1, c.a2, c.a3, c.a4, c.a5, u32(ctrA.Obj())};
            popBytes = 24;
            ecx = 0x12345678u;
            break;
        }
        case 3: {   // CalcDistToAnyConnectingLinks
            this3A.Fill(sd++); node3A.Fill(sd++); linkArrA.Fill(sd++); FillBanks(sd);
            TagAdd(this3A.m.data(), this3A.m.size()); TagAdd(node3A.m.data(), node3A.m.size()); TagAdd(linkArrA.m.data(), linkArrA.m.size()); TagBanks();
            PutNodes(c, 8);
            const int64 base = int64(std::int16_t(c.base & 0xFFFFu));
            this3A.Put32(0xA44 + 4 * int64(c.narea), u32(linkArrA.Obj()));
            this3A.Put32(0x804 + 4 * int64(kTgtArea[0]), BankPtr(c.areaBank[0], c.loadedArea[0]));
            this3A.Put32(0x804 + 4 * int64(kTgtArea[1]), BankPtr(c.areaBank[1], c.loadedArea[1]));
            // el nodo consultado: posicion = nodo 7 del banco 2; base de enlaces, area y byte de banderas
            node3A.Put16(8, u32(c.np[2][7][0])); node3A.Put16(10, u32(c.np[2][7][1])); node3A.Put16(12, u32(c.np[2][7][2]));
            node3A.Put16(0x10, c.base); node3A.Put16(0x12, c.narea); node3A.Put8(0x18, c.nflags);
            for (unsigned k = 0; k < 16; ++k) linkArrA.Put32(4 * (base + int64(k)), kTgtArea[c.lkArea[k] & 1] | (c.lkNode[k] << 16));
            this3A.Snap(); node3A.Snap(); linkArrA.Snap(); SnapBanks();
            args = {u32(node3A.Obj()), c.pos[0], c.pos[1], c.pos[2]};
            ecx = u32(this3A.Obj());
            break;
        }
        default: {  // FindNodeClosestInRegion
            this4A.Fill(sd++); outA.Fill(sd++); distA.Fill(sd++); FillBanks(sd);
            TagAdd(this4A.m.data(), this4A.m.size()); TagAdd(outA.m.data(), outA.m.size()); TagAdd(distA.m.data(), distA.m.size()); TagBanks();
            PutNodes(c, kBankNodes);
            for (unsigned b = 0; b < kBanks; ++b)
                for (unsigned i = 0; i < kBankNodes; ++i) { bankA[b].Put8(int64(i) * 0x1C + 0x18, c.fl18[i]); bankA[b].Put8(int64(i) * 0x1C + 0x19, c.fl19[i]); }
            this4A.Put32(0x804 + 4 * int64(c.area4), BankPtr(c.areaBank[0], c.loadedArea[0]));
            this4A.Put32(0xFA4 + 4 * int64(c.area4), c.numNodes);
            this4A.Put32(0x10C4 + 4 * int64(c.area4), c.numVeh);
            outA.Put32(0, c.outInit);
            distA.Put32(0, c.outDist);
            this4A.Snap(); outA.Snap(); distA.Snap(); SnapBanks();
            args = {u32(outA.Obj()), c.area4, c.pos[0], c.pos[1], c.pos[2], c.ntype, u32(distA.Obj()), c.flLow, c.flUnk, c.flBoat, c.flUnused};
            ecx = u32(this4A.Obj());
            break;
        }
        }
        g = State{}; g.c = &c; nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) { return Gate(entry, args.data(), u32(args.size()), c.eax, ecx, c.edx, c.stack, popBytes); });
        switch (fn) {
        case 0: AddChanges(ch, 0, linkA.Changes()); break;
        case 1: AddChanges(ch, 0, this1A.Changes()); AddChanges(ch, 1, nodesA.Changes()); AddChanges(ch, 2, countA.Changes()); AddBanks(ch, 3); break;
        case 2: AddBanks(ch, 0); AddChanges(ch, 3, links2A.Changes()); AddChanges(ch, 4, lens2A.Changes()); AddChanges(ch, 5, ints2A.Changes()); AddChanges(ch, 6, ctrA.Changes()); break;
        case 3: AddChanges(ch, 0, this3A.Changes()); AddChanges(ch, 1, node3A.Changes()); AddChanges(ch, 2, linkArrA.Changes()); AddBanks(ch, 3); break;
        default: AddChanges(ch, 0, this4A.Changes()); AddChanges(ch, 1, outA.Changes()); AddChanges(ch, 2, distA.Changes()); AddBanks(ch, 3); break;
        }
        obs["changes"] = ch;
        obs["events"] = EvsJson();
        if (fn == 0 || fn == 3) obs["st0"] = HexBytes(sides[s].fp.data() + 28, 10);   // ST0 de la imagen FNSAVE (valor de retorno)
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
    for (const auto& s : env.dllOnly) ready &= s.pair.dll != 0 && ModuleOf(s.pair.dll) == env.asi;
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
    if (!std::filesystem::exists(dir / "fn93a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn93a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn93a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
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
    env.stubs[3] = {F(0x412850, &FkDistToLine)};
    env.stubs[4] = {F(0x44F190, &FkCalcDist)};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"CPathNode::GetPosition 0x420A10 (hook activo en ambos lados) y la aritmetica de la funcion"})},
                                 {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    for (unsigned f = 0; f < kFunctions; ++f) env.watches.emplace_back(kEntries[f], 16);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
