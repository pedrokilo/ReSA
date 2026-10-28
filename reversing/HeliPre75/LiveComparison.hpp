#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "Camera.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "Shadows.h"
#include "Clock.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeliPre75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::PreRender (0x6C5420, hook VIRTUAL, ranura 17 de la vtable 0x871680; thiscall sin argumentos, void). Original = bytes del EXE con el hook V apagado; candidato = DLL con SOLO ese hook
// encendido (la entrada del EXE salta al cuerpo de la DLL y la ranura de la vtable del EXE apunta a el). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con hook RH,
// apagado durante el banco): CVehicle::PreRender 0x6D6480 (virtual), CClock::GetIsTimeInRange 0x52CEE0, operator*(CMatrix, CVector) 0x59C890, CHeli::AddHeliSearchLight 0x6C45B0, CEntity::GetColModel 0x535300,
// CVehicleModelInfo::GetWheelPosn 0x4C7D20, CAutomobile::UpdateWheelMatrix 0x6AA290 y DoHeliDustEffect 0x6B0690, CMatrix::Attach 0x59BD10 / SetRotateZ 0x59B0E0 / SetRotateX 0x59B060 / UpdateRW 0x59BBB0 (la DLL llama a
// sus cuerpos: CMatrix::InjectHooks esta desactivado) y CShadows::StoreShadowForVehicle 0x70BDA0. La llamada virtual del EXE a la ranura 52 de la vtable del objeto (CAutomobile::DoBurstAndSoftGroundRatios) pasa por una
// vtable propia del banco (la arena del helicoptero apunta a ella): cada ranura es un falso que registra la llamada. El destructor de CMatrix (0x59ACD0) no se sustituye (matriz sin propietario: sin efecto).

namespace Hp75Layout {
inline constexpr uintptr_t kModelTableAddr = 0xA9B0C8;
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CHeli) == Hp75Fix::kHeliWin, "la arena del helicoptero es un CHeli completo");
static_assert(sizeof(CMatrix) == 0x48, "CMatrix mide 0x48");
static_assert(offsetof(CAutomobile, m_fHeliRotorSpeed) == 0x84C && offsetof(CAutomobile, m_wheelPosition) == 0x838 && offsetof(CAutomobile, m_fWheelsSuspensionCompression) == 0x7D4, "CAutomobile: rotor, posicion de ruedas, compresion");
static_assert(offsetof(CAutomobile, m_aSuspensionSpringLength) == 0x878 && offsetof(CAutomobile, m_aSuspensionLineLength) == 0x888, "CAutomobile: muelles y lineas");
static_assert(offsetof(CAutomobile, m_aCarNodes) + 12 * sizeof(void*) == 0x678, "nodo 12 en +0x678");
static_assert(offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(CVehicle, m_nHandlingFlagsIntValue) == 0x38C, "CVehicle: manejo +0x384 y banderas +0x38C");
static_assert(offsetof(CHeli, m_fRotorZ) == 0x9A0 && offsetof(CHeli, m_fSecondRotorZ) == 0x9A4 && offsetof(CHeli, m_vecSearchLightTarget) == 0x9F4 && offsetof(CHeli, m_fSearchLightIntensity) == 0xA00 && offsetof(CHeli, m_bSearchLightEnabled) == 0xA11, "CHeli: giros y foco");
static_assert(offsetof(RwFrame, modelling) == 0x10, "RwFrame: matriz de modelado en +0x10");
inline std::string why;
inline bool slot52Hit{};
inline void __fastcall Slot52Probe(void*, void*) { slot52Hit = true; }
inline std::array<uint32, 96> probeTable{};
inline bool LayoutOK() {
    alignas(16) static std::array<uint8, 0xA40> mem{};
    why.clear();
    auto* h = reinterpret_cast<CHeli*>(mem.data());
    auto chk = [&](const char* name, bool cond) { if (!cond) { why += name; why += ';'; } };
    auto one = [&](const char* name, auto set, size_t byte, uint8 value) {
        mem.fill(0); set();
        size_t other = 0; for (size_t i = 0x400; i < 0x880; ++i) other += mem[i] != 0;
        chk(name, mem[byte] == value && other == 1);
    };
    one("colProcessed", [&] { h->vehicleFlags.bVehicleColProcessed = true; }, 0x42B, 0x01);
    // La llamada virtual DoBurstAndSoftGroundRatios() de la DLL pasa por la ranura 52 de la vtable del objeto
    mem.fill(0);
    probeTable.fill(0);
    probeTable[52] = static_cast<uint32>(reinterpret_cast<uintptr_t>(&Slot52Probe));
    const uint32 tablePtr = static_cast<uint32>(reinterpret_cast<uintptr_t>(probeTable.data()));
    memcpy(mem.data(), &tablePtr, 4);
    slot52Hit = false;
    h->DoBurstAndSoftGroundRatios();
    chk("vtable_slot_52", slot52Hit);
    chk("timestep", uintptr_t(&CTimer::ms_fTimeStep) == 0xB7CB5C);
    chk("model_table", uintptr_t(&CModelInfo::ms_modelInfoPtrs[0]) == kModelTableAddr);
    return why.empty();
}
}

namespace Hp75Live {
using namespace Hp75Fix;
inline constexpr uintptr_t kEntry = 0x6C5420;
inline constexpr uintptr_t kVtable = 0x871680;
inline constexpr unsigned kSlot = 17;
inline uint32& G32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }

// ================================================================================================ arenas
template<size_t N, size_t G = kGuard> struct Arena {
    alignas(16) std::array<uint8, N> m{}, r{};
    uintptr_t Obj() const { return uintptr_t(m.data()) + G; }
    void Fill(u32 seed) { FillWords(m.data(), N, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + G + off, &v, 4); }
    void Put16(size_t off, u32 v) { const uint16 w = uint16(v); memcpy(m.data() + G + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[G + off] = uint8(v); }
    json Changes() const { return ArenaChanges(m.data(), r.data(), N, G); }
    void Tagged() const { TagAdd(m.data(), N); }
};
inline Arena<kArena(kHeliWin)> heliA;
inline Arena<kArena(kMatWin)> matA;
inline Arena<kArena(kHandWin)> handA;
inline std::array<Arena<kArena(kFrameWin)>, 4> frameA;

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

// ================================================================================================ PreRender (0x6C5420)
struct State { const C* c{}; };
inline State g;
// Eventos: 1 CVehicle::PreRender(this); 2 CClock::GetIsTimeInRange(desde, hasta); 3 operator*(matriz, x, y, z); 4 AddHeliSearchLight(origen xyz, objetivo xyz, radio, potencia, corona, bandera, sombra);
// 5 GetColModel(this); 6 ranura 52 (this); 7 GetWheelPosn(info del modelo, rueda, local); 8 UpdateWheelMatrix(this, nodo, banderas); 9 DoHeliDustEffect(this, a, b);
// 10 CMatrix::Attach(matriz local, RwMatrix*, propietaria); 11 SetRotateZ(matriz local, angulo); 12 SetRotateX(matriz local, angulo); 13 UpdateRW(matriz local, RwMatrix* unido);
// 14 StoreShadowForVehicle(vehiculo, tipo); 15 llamada a otra ranura de la vtable (no debe ocurrir). Los punteros dentro de una arena se etiquetan; las matrices locales (pila) se marcan.
inline u32 StackMark(const void* p) { return Tag(p) == u32(uintptr_t(p)) ? 0xF00D0001u : Tag(p); }
inline u32 ModelMarker(u32 p) { return p == reinterpret_cast<const u32*>(kModelTable)[static_cast<int16>(g.c->model)] ? 0xA0D00001u : p; }
inline void __fastcall FakeBase(void* self, void*) { Ev& e = PushEv(1, 1); e.v[0] = Tag(self); }
inline u32 __cdecl FakeTimeRange(u32 from, u32 to) { Ev& e = PushEv(2, 2); e.v[0] = from & 0xFFu; e.v[1] = to & 0xFFu; return g.c->timeRet; }
inline u32* __cdecl FakeMulVec(u32* out, void* m, u32* v) {
    Ev& e = PushEv(3, 4); e.v[0] = Tag(m); e.v[1] = v[0]; e.v[2] = v[1]; e.v[3] = v[2];
    for (int i = 0; i < 3; ++i) out[i] = g.c->origin[i];
    return out;
}
inline void __cdecl FakeLight(u32* origin, u32* target, u32 radius, u32 power, u32 corona, u32 flag, u32 shadow) {
    Ev& e = PushEv(4, 11);
    for (int i = 0; i < 3; ++i) { e.v[i] = origin[i]; e.v[3 + i] = target[i]; }
    e.v[6] = radius; e.v[7] = power; e.v[8] = Tag(corona); e.v[9] = flag & 0xFFu; e.v[10] = shadow & 0xFFu;
}
inline u32 __fastcall FakeCol(void* self, void*) { Ev& e = PushEv(5, 1); e.v[0] = Tag(self); return 0x12345678u; }
inline void __fastcall FakeSlot52(void* self, void*) { Ev& e = PushEv(6, 1); e.v[0] = Tag(self); }
inline void __fastcall FakeOtherSlot(void* self, void*) { Ev& e = PushEv(15, 1); e.v[0] = Tag(self); }
inline void __fastcall FakeWheelPosn(void* self, void*, u32 wheel, u32* out, u32 local) {
    Ev& e = PushEv(7, 3); e.v[0] = ModelMarker(u32(uintptr_t(self))); e.v[1] = wheel; e.v[2] = local & 0xFFu;
    for (int i = 0; i < 3; ++i) out[i] = wheel < 4 ? g.c->wheel[wheel][i] : 0u;
}
inline void __fastcall FakeWheelMat(void* self, void*, u32 node, u32 flags) { Ev& e = PushEv(8, 3); e.v[0] = Tag(self); e.v[1] = node; e.v[2] = flags; }
inline void __fastcall FakeDust(void* self, void*, u32 a, u32 b) { Ev& e = PushEv(9, 3); e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; }
inline void __fastcall FakeAttach(void* self, void*, void* rw, u32 own) {
    Ev& e = PushEv(10, 3); e.v[0] = StackMark(self); e.v[1] = Tag(rw); e.v[2] = own & 0xFFu;
    u8* m = static_cast<u8*>(self);
    memcpy(m, rw, 0x40);                                   // UpdateMatrix: right/up/at/pos (y el relleno)
    *reinterpret_cast<void**>(m + 0x40) = rw;
    *reinterpret_cast<u32*>(m + 0x44) = (own & 0xFFu) ? 1u : 0u;
}
// Rotaciones: matriz deterministica a partir del angulo (los bits del angulo van tal cual) y posicion a cero, como las reales
inline void __fastcall FakeRotZ(void* self, void*, u32 angle) {
    Ev& e = PushEv(11, 2); e.v[0] = StackMark(self); e.v[1] = angle;
    u32* m = static_cast<u32*>(self);
    m[0] = angle; m[1] = 0; m[2] = 0; m[4] = 0; m[5] = angle; m[6] = 0; m[8] = 0; m[9] = 0; m[10] = angle; m[12] = 0; m[13] = 0; m[14] = 0;
}
inline void __fastcall FakeRotX(void* self, void*, u32 angle) {
    Ev& e = PushEv(12, 2); e.v[0] = StackMark(self); e.v[1] = angle;
    u32* m = static_cast<u32*>(self);
    m[0] = 0x3F800000u; m[1] = 0; m[2] = 0; m[4] = 0; m[5] = angle; m[6] = angle; m[8] = 0; m[9] = angle; m[10] = angle; m[12] = 0; m[13] = 0; m[14] = 0;
}
inline void __fastcall FakeUpdateRW(void* self, void*) {
    u8* m = static_cast<u8*>(self);
    u8* rw = *reinterpret_cast<u8**>(m + 0x40);
    Ev& e = PushEv(13, 2); e.v[0] = StackMark(self); e.v[1] = Tag(rw);
    if (rw) for (const unsigned w : {0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 10u, 12u, 13u, 14u}) memcpy(rw + w * 4, m + w * 4, 4);
}
inline void __cdecl FakeShadow(u32 vehicle, u32 type) { Ev& e = PushEv(14, 2); e.v[0] = Tag(vehicle); e.v[1] = type; }

// vtable propia: la ranura 52 es el falso de DoBurstAndSoftGroundRatios; las demas, un falso que lo registra
inline std::array<uint32, 96> fakeVtable{};
inline void BuildVtable() {
    for (auto& s : fakeVtable) s = static_cast<uint32>(reinterpret_cast<uintptr_t>(&FakeOtherSlot));
    fakeVtable[52] = static_cast<uint32>(reinterpret_cast<uintptr_t>(&FakeSlot52));
}

inline void CasePre(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        heliA.Fill(c.seed * 13u + 1u); matA.Fill(c.seed * 13u + 2u); handA.Fill(c.seed * 13u + 3u);
        for (int k = 0; k < 4; ++k) frameA[k].Fill(c.seed * 13u + 4u + k);
        heliA.Put32(0, static_cast<u32>(reinterpret_cast<uintptr_t>(fakeVtable.data())));
        heliA.Put32(0x14, c.matPresent ? u32(matA.Obj()) : 0u);
        heliA.Put16(0x22, c.model); heliA.Put32(0x40, c.flags40); heliA.Put32(0x384, u32(handA.Obj())); heliA.Put32(0x38C, c.flags38c); heliA.Put8(0x42B, c.b42b);
        handA.Put32(0xB8, c.handB8);
        heliA.Put8(0xA11, c.a11); heliA.Put32(0xA00, c.intens);
        for (int i = 0; i < 3; ++i) heliA.Put32(0x9F4 + 4 * i, c.tgt[i]);
        for (int i = 0; i < 4; ++i) {
            heliA.Put32(0x7D4 + 4 * i, c.comp[i]); heliA.Put32(0x878 + 4 * i, c.spr[i]); heliA.Put32(0x888 + 4 * i, c.line[i]); heliA.Put32(0x838 + 4 * i, c.wp[i]);
        }
        heliA.Put32(0x84C, c.rotor); heliA.Put32(0x9A0, c.a0); heliA.Put32(0x9A4, c.a1);
        for (int k = 0; k < 4; ++k) {
            heliA.Put32(0x678 + 4 * k, c.nodePresent[k] ? u32(frameA[k].Obj()) : 0u);
            for (int i = 0; i < 16; ++i) frameA[k].Put32(0x10 + 4 * i, c.frame[k][i]);
        }
        heliA.Snap(); matA.Snap(); handA.Snap(); for (auto& f : frameA) f.Snap();
        TagClear(); heliA.Tagged(); matA.Tagged(); handA.Tagged(); for (auto& f : frameA) f.Tagged();
        G32(kStepAddr) = c.step;
        g.c = &c; nev = 0;
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, nullptr, 0, c.eax, u32(heliA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json globals = json::object();
        globals["step"] = G32(kStepAddr); globals["dust"] = G32(kDustAddr);
        obs["globals"] = globals;
        obs["changes"] = json::array({heliA.Changes(), matA.Changes(), handA.Changes(), frameA[0].Changes(), frameA[1].Changes(), frameA[2].Changes(), frameA[3].Changes()});
        obs["events"] = EvsJson();
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[0], n, input, rows, sides);
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
    const bool layout = Hp75Layout::LayoutOK();
    ready &= layout;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases}, {"layout_ok", layout}, {"layout_why", Hp75Layout::why},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CasePre(env, totals, c);
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
// Colaborador con hook: se crea su ruta (el hook queda apagado) y se toma el cuerpo de la DLL; sin hook (dll = 0) la DLL llama a la direccion del EXE.
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
// Colaborador de la DLL sin hook RH propio (CMatrix::InjectHooks esta desactivado): la DLL llama a su cuerpo; el EXE conserva su codigo.
inline Pair MakeDllCollab(uintptr_t exe, uintptr_t dllBody) { return Pair{exe, dllBody, false}; }
template<class M> inline uintptr_t MemberBody(M member) { return Body(std::bit_cast<uintptr_t>(member)); }

inline ReversibleHooks::ReversibleHook::Base* FindHookByName(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "helipre75-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "helipre75-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("helipre75"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot = kVtable + 4 * kSlot;
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta VIRTUAL: el cuerpo del candidato es el destino vivo del hook V. Se construye ANTES que nada; el constructor deja el hook apagado.
    {
        auto* hook = FindHookByName("CHeli", "PreRender");
        uintptr_t body = 0;
        if (hook && !strcmp(hook->Symbol(), "V")) body = uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready = false;
        env.route[0].reset(new HookRoute("CHeli", "PreRender", kEntry, body));
        registered[kFn[0]] = env.route[0]->Registered(); bodies[kFn[0]] = env.route[0]->Body() != 0;
        ready &= env.route[0]->Registered() && env.route[0]->Body() && ModuleOf(env.route[0]->Body()) == env.asi;
        ownHooks[kFn[0]] = hook ? json({{"class", "CHeli"}, {"name", "PreRender"}, {"address", kEntry}, {"type", hook->Symbol()}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    BuildVtable();
    // CVehicle::PreRender es VIRTUAL con hook V: el cuerpo de la DLL es el destino vivo del hook (la llamada calificada de la DLL va directa a el)
    Pair pBase{0x6D6480, 0, false};
    if (auto* hook = FindHookByName("CVehicle", "PreRender"); hook && !strcmp(hook->Symbol(), "V")) {
        const uintptr_t body = uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        env.collab.emplace_back(new HookRoute("CVehicle", "PreRender", 0x6D6480, body));
        hooksFound.push_back({{"exe", 0x6D6480}, {"class", "CVehicle"}, {"name", "PreRender"}});
        pBase.dll = Body(body); pBase.hooked = true;
    }
    const Pair pTime = MakeCollab(env, 0x52CEE0, hooksFound);
    Pair pMul = MakeCollab(env, 0x59C890, hooksFound);
#pragma warning(push)
#pragma warning(disable: 4996)
    if (!pMul.hooked) pMul = MakeDllCollab(0x59C890, Body(std::bit_cast<uintptr_t>(static_cast<CVector(*)(const CMatrix&, const CVector&)>(&operator*))));
#pragma warning(pop)
    const Pair pLight = MakeCollab(env, 0x6C45B0, hooksFound), pCol = MakeCollab(env, 0x535300, hooksFound), pWheelPosn = MakeCollab(env, 0x4C7D20, hooksFound);
    const Pair pWheelMat = MakeCollab(env, 0x6AA290, hooksFound);
    Pair pDust = MakeCollab(env, 0x6B0690, hooksFound);   // DoHeliDustEffect no tiene hook RH: la DLL llama a su cuerpo
    if (!pDust.hooked) pDust = MakeDllCollab(0x6B0690, MemberBody(&CAutomobile::DoHeliDustEffect));
    Pair pAttach = MakeCollab(env, 0x59BD10, hooksFound);
    if (!pAttach.hooked) pAttach = MakeDllCollab(0x59BD10, MemberBody(&CMatrix::Attach));
    Pair pRotZ = MakeCollab(env, 0x59B0E0, hooksFound);
    if (!pRotZ.hooked) pRotZ = MakeDllCollab(0x59B0E0, MemberBody(&CMatrix::SetRotateZ));
    Pair pRotX = MakeCollab(env, 0x59B060, hooksFound);
    if (!pRotX.hooked) pRotX = MakeDllCollab(0x59B060, MemberBody(&CMatrix::SetRotateX));
    Pair pUpdate = MakeCollab(env, 0x59BBB0, hooksFound);
    if (!pUpdate.hooked) pUpdate = MakeDllCollab(0x59BBB0, MemberBody(&CMatrix::UpdateRW));
    const Pair pShadow = MakeCollab(env, 0x70BDA0, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pBase, reinterpret_cast<void*>(&FakeBase)), S(pTime, reinterpret_cast<void*>(&FakeTimeRange)), S(pMul, reinterpret_cast<void*>(&FakeMulVec)), S(pLight, reinterpret_cast<void*>(&FakeLight)),
        S(pCol, reinterpret_cast<void*>(&FakeCol)), S(pWheelPosn, reinterpret_cast<void*>(&FakeWheelPosn)), S(pWheelMat, reinterpret_cast<void*>(&FakeWheelMat)), S(pDust, reinterpret_cast<void*>(&FakeDust)),
        S(pAttach, reinterpret_cast<void*>(&FakeAttach)), S(pRotZ, reinterpret_cast<void*>(&FakeRotZ)), S(pRotX, reinterpret_cast<void*>(&FakeRotX)), S(pUpdate, reinterpret_cast<void*>(&FakeUpdateRW)),
        S(pShadow, reinterpret_cast<void*>(&FakeShadow))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: la ranura 52 de la vtable propia, DoBurstAndSoftGroundRatios, es un falso del banco"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16); env.watches.emplace_back(env.vtSlot, 4);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
