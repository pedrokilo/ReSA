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
#include "WindModifiers.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par HeliFly75 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CHeli::ProcessFlyingCarStuff (0x6C4E60, hook VIRTUAL, ranura 68 de la vtable 0x871680; thiscall sin argumentos, void). Original = bytes del EXE con el hook V apagado; candidato = DLL con SOLO ese hook
// encendido (la entrada del EXE salta al cuerpo de la DLL y la ranura de la vtable del EXE apunta a el). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con hook RH, apagado
// durante el banco): CVehicle::FlyingControl 0x6D85F0, CMatrix::Attach 0x59BD10 (la DLL llama a su propio cuerpo: no hay hook RH), RwFrameForAllObjects 0x7F1200 y _rpAtomicResyncInterpolatedSphere 0x7491F0 (sin hook; las
// envolturas de la DLL llaman a la direccion del EXE), CVehicle::DoBladeCollision 0x6E2E50, CWindModifiers::RegisterOne 0x72C8B0 y CAEVehicleAudioEntity::AddAudioEvent 0x4F6420.
// Multiply3x3 (0x59C790) NO se sustituye: el EXE ejecuta la suya y la DLL calcula con Heli75DotCpp (lo que se compara es el resultado: la decision de disparar el sonido de las aspas). El destructor de CMatrix
// (0x59ACD0) tampoco: con la matriz sin propietario no hace nada observable.
// El falso de RwFrameForAllObjects no ejecuta la funcion de retorno (0x6A0750 en el EXE; GetCurrentAtomicObjectCB en la DLL, que el falso reconoce): escribe el atomico (o nulo). El falso de
// _rpAtomicResyncInterpolatedSphere escribe un radio nuevo en el atomico (el EXE lee el radio DESPUES de la llamada). Los globales (paso del temporizador, camara) se fijan por caso y se restauran.

namespace Hf75Layout {
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CHeli) == Hf75Fix::kHeliWin, "la arena del helicoptero es un CHeli completo");
static_assert(sizeof(CMatrix) == 0x48, "CMatrix mide 0x48");
static_assert(offsetof(CAutomobile, m_fHeliRotorSpeed) == 0x84C && offsetof(CAutomobile, m_wheelRotation) == 0x828 && offsetof(CAutomobile, m_fPropRotate) == 0x958, "CAutomobile: rotor +0x84C, giro de ruedas +0x828, +0x958");
static_assert(offsetof(CAutomobile, m_aCarNodes) + 12 * sizeof(void*) == 0x678, "nodo 12 en +0x678");
static_assert(offsetof(CVehicle, m_vehicleAudio) == 0x138 && offsetof(CVehicle, m_pHandlingData) == 0x384, "CVehicle: audio +0x138, manejo +0x384");
static_assert(offsetof(CHeli, m_fLeftRightSkid) == 0x98C && offsetof(CHeli, m_fAccelerationBreakStatus) == 0x998, "CHeli: controles en +0x98C..+0x998");
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == 0x44, "velocidad en +0x44");
static_assert(offsetof(RpAtomic, boundingSphere) + offsetof(RwSphere, radius) == 0x28 && offsetof(RpAtomic, interpolator) == 0x4C, "RpAtomic: radio en +0x28 y banderas del interpolador en +0x4C");
static_assert(offsetof(RwFrame, modelling) == 0x10, "RwFrame: matriz de modelado en +0x10");
inline std::string why;
inline bool LayoutOK() {
    why.clear();
    auto chk = [&](const char* name, bool cond) { if (!cond) { why += name; why += ';'; } };
    chk("camera", uintptr_t(&TheCamera) == 0xB6F028);
    chk("step", uintptr_t(&CTimer::ms_fTimeStep) == 0xB7CB5C);
    chk("camera_matrix_ptr", uintptr_t(&TheCamera.m_matrix) == 0xB6F03C && uintptr_t(&TheCamera.m_placement) == 0xB6F02C);
    chk("class_size_matrix", sizeof(CMatrixLink) >= 0x48);
    return why.empty();
}
}

namespace Hf75Live {
using namespace Hf75Fix;
inline constexpr uintptr_t kEntry = 0x6C4E60;
inline constexpr uintptr_t kVtable = 0x871680;
inline constexpr unsigned kSlot = 68;
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
inline Arena<kArena(kMatWin)> matA, camA;
inline Arena<kArena(kFrameWin)> frameA;
inline Arena<kArena(kAtomWin)> atomA;
inline Arena<kArena(kHandWin)> handA;

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

// ================================================================================================ ProcessFlyingCarStuff (0x6C4E60)
struct State { const C* c{}; };
inline State g;
// Eventos: 1 FlyingControl(this, modelo, a, b, c, d); 2 CMatrix::Attach(matriz local, RwMatrix*, propietaria); 3 RwFrameForAllObjects(marco, funcion de retorno esperada?, destino*);
// 4 _rpAtomicResyncInterpolatedSphere(atomico); 5 DoBladeCollision(this, x, y, z, matriz, tipo, radio, escala); 6 CWindModifiers::RegisterOne(x, y, z, activo, potencia);
// 7 AddAudioEvent(audio, evento, volumen). Los punteros dentro de una arena se etiquetan; las matrices locales (pila) se marcan.
inline u32 StackMark(const void* p) { return Tag(p) == u32(uintptr_t(p)) ? 0xF00D0001u : Tag(p); }
inline uintptr_t gCallbackDll{}, gCallbackThunk{};   // GetCurrentAtomicObjectCB de la DLL: cuerpo y enlace incremental (la direccion que recibe el falso es la del enlace)
inline void __fastcall FakeFlying(void* self, void*, u32 model, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(1, 6); e.v[0] = Tag(self); e.v[1] = model; e.v[2] = a; e.v[3] = b; e.v[4] = c; e.v[5] = d;
}
inline void __fastcall FakeAttach(void* self, void*, void* rw, u32 own) {
    Ev& e = PushEv(2, 3); e.v[0] = StackMark(self); e.v[1] = Tag(rw); e.v[2] = own & 0xFFu;
    u8* m = static_cast<u8*>(self);
    memcpy(m, rw, 0x40);                                   // UpdateMatrix: right/up/at/pos (y el relleno)
    *reinterpret_cast<void**>(m + 0x40) = rw;
    *reinterpret_cast<u32*>(m + 0x44) = (own & 0xFFu) ? 1u : 0u;
}
inline u32 __cdecl FakeFrames(u32 frame, u32 callback, u32* data) {
    Ev& e = PushEv(3, 3); e.v[0] = Tag(frame); e.v[1] = (callback == 0x6A0750u || callback == gCallbackDll || callback == gCallbackThunk) ? 0xCB000001u : callback; e.v[2] = StackMark(data);
    *data = g.c->atomic ? u32(atomA.Obj()) : 0u;
    return frame;
}
inline void __cdecl FakeResync(u32 atomic) {
    Ev& e = PushEv(4, 1); e.v[0] = Tag(atomic);
    memcpy(reinterpret_cast<u8*>(uintptr_t(atomic)) + 0x28, &g.c->radiusAfter, 4);   // el radio nuevo
}
inline u32 __fastcall FakeBlade(void* self, void*, u32 x, u32 y, u32 z, u32 mat, u32 type, u32 radius, u32 scale) {
    Ev& e = PushEv(5, 8); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; e.v[4] = Tag(mat); e.v[5] = type & 0xFFFFu; e.v[6] = radius; e.v[7] = scale;
    return 0u;
}
inline void __cdecl FakeWind(u32 x, u32 y, u32 z, u32 active, u32 power) {
    Ev& e = PushEv(6, 5); e.v[0] = x; e.v[1] = y; e.v[2] = z; e.v[3] = active; e.v[4] = power;
}
inline void __fastcall FakeAudio(void* self, void*, u32 event, u32 volume) {
    Ev& e = PushEv(7, 3); e.v[0] = Tag(self); e.v[1] = event; e.v[2] = volume;
}

inline void CaseFly(Env& env, Totals& t, unsigned n) {
    const C c = MakeC(n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        heliA.Fill(c.seed * 13u + 1u); matA.Fill(c.seed * 13u + 2u); camA.Fill(c.seed * 13u + 3u); frameA.Fill(c.seed * 13u + 4u); atomA.Fill(c.seed * 13u + 5u); handA.Fill(c.seed * 13u + 6u);
        heliA.Put32(0x14, c.matPresent ? u32(matA.Obj()) : 0u);
        for (int i = 0; i < 3; ++i) {
            if (c.matPresent) matA.Put32(0x30 + 4 * i, c.pos[i]); else heliA.Put32(4 + 4 * i, c.pos[i]);
        }
        for (int i = 0; i < 12; ++i) if (i % 4 != 3) matA.Put32(4 * i, c.mat[i]);   // right, forward y up (los rellenos quedan con el relleno aleatorio)
        heliA.Put8(0x36, c.status); heliA.Put16(0x22, c.model); heliA.Put32(0x40, c.flags40); heliA.Put8(0x428, c.b428); heliA.Put8(0x42C, c.b42c); heliA.Put8(0x960, c.b960); heliA.Put8(0xB8, c.bB8);
        heliA.Put32(0x384, u32(handA.Obj())); handA.Put32(0xCC, c.handFlags);
        heliA.Put32(0x84C, c.rotor);
        for (int i = 0; i < 4; ++i) heliA.Put32(0x98C + 4 * i, c.ctl[i]);
        for (int i = 0; i < 3; ++i) heliA.Put32(0x44 + 4 * i, c.vel[i]);
        heliA.Put32(0x958, c.prop); heliA.Put32(0x82C, c.wheelRot);
        heliA.Put32(0x678, c.nodePresent ? u32(frameA.Obj()) : 0u);
        for (int i = 0; i < 16; ++i) frameA.Put32(0x10 + 4 * i, c.frame[i]);
        atomA.Put8(0x4C, c.atomFlags); atomA.Put32(0x28, c.radius);
        for (int i = 0; i < 3; ++i) if (c.camMat) camA.Put32(0x30 + 4 * i, c.camPos[i]);
        heliA.Snap(); matA.Snap(); camA.Snap(); frameA.Snap(); atomA.Snap(); handA.Snap();
        TagClear(); heliA.Tagged(); matA.Tagged(); camA.Tagged(); frameA.Tagged(); atomA.Tagged(); handA.Tagged();
        G32(kStepAddr) = c.step;
        G32(kCamMatPtrAddr) = c.camMat ? u32(camA.Obj()) : 0u;
        // la posicion que no se usa recibe otro valor (si el candidato leyera la equivocada, la comparacion lo vera)
        for (int i = 0; i < 3; ++i) G32(kCamPosAddr + 4 * i) = c.camMat ? Rand(c.seed, 900 + i) : c.camPos[i];
        g.c = &c; nev = 0;
        sides[s] = Execute(env, 0, kEntry, candidate, [&](uintptr_t entry) { return Gate(entry, nullptr, 0, c.eax, u32(heliA.Obj()), c.edx, c.stack, 0); });
        json obs;
        json globals = json::object();
        globals["step"] = G32(kStepAddr); globals["cam_ptr"] = G32(kCamMatPtrAddr);
        globals["cam_pos"] = json::array({G32(kCamPosAddr), G32(kCamPosAddr + 4), G32(kCamPosAddr + 8)});
        obs["globals"] = globals;
        obs["changes"] = json::array({heliA.Changes(), matA.Changes(), camA.Changes(), frameA.Changes(), atomA.Changes(), handA.Changes()});
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
    const bool layout = Hf75Layout::LayoutOK();
    ready &= layout;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = kCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * kCases}, {"cases", cases}, {"layout_ok", layout}, {"layout_why", Hf75Layout::why},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < kCases; ++c) CaseFly(env, totals, c);
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
    if (!std::filesystem::exists(dir / "helifly75-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "helifly75-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("helifly75"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot = kVtable + 4 * kSlot;
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta VIRTUAL: el cuerpo del candidato es el destino vivo del hook V. Se construye ANTES que nada; el constructor deja el hook apagado.
    {
        auto* hook = FindHookByName("CHeli", "ProcessFlyingCarStuff");
        uintptr_t body = 0;
        if (hook && !strcmp(hook->Symbol(), "V")) body = uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        else ready = false;
        env.route[0].reset(new HookRoute("CHeli", "ProcessFlyingCarStuff", kEntry, body));
        registered[kFn[0]] = env.route[0]->Registered(); bodies[kFn[0]] = env.route[0]->Body() != 0;
        ready &= env.route[0]->Registered() && env.route[0]->Body() && ModuleOf(env.route[0]->Body()) == env.asi;
        ownHooks[kFn[0]] = hook ? json({{"class", "CHeli"}, {"name", "ProcessFlyingCarStuff"}, {"address", kEntry}, {"type", hook->Symbol()}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    gCallbackThunk = reinterpret_cast<uintptr_t>(&GetCurrentAtomicObjectCB);
    gCallbackDll = Body(gCallbackThunk);
    const Pair pFly = MakeCollab(env, 0x6D85F0, hooksFound);
    Pair pAttach = MakeCollab(env, 0x59BD10, hooksFound);
    if (!pAttach.hooked) pAttach = MakeDllCollab(0x59BD10, MemberBody(&CMatrix::Attach));
    const Pair pFrames = MakeCollab(env, 0x7F1200, hooksFound);   // sin hook: la envoltura de la DLL llama a la direccion del EXE
    const Pair pResync = MakeCollab(env, 0x7491F0, hooksFound);   // sin hook: la envoltura de la DLL llama a la direccion del EXE
    const Pair pBlade = MakeCollab(env, 0x6E2E50, hooksFound), pWind = MakeCollab(env, 0x72C8B0, hooksFound), pAudio = MakeCollab(env, 0x4F6420, hooksFound);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    env.stubs[0] = {S(pFly, reinterpret_cast<void*>(&FakeFlying)), S(pAttach, reinterpret_cast<void*>(&FakeAttach)), S(pFrames, reinterpret_cast<void*>(&FakeFrames)), S(pResync, reinterpret_cast<void*>(&FakeResync)),
        S(pBlade, reinterpret_cast<void*>(&FakeBlade)), S(pWind, reinterpret_cast<void*>(&FakeWind)), S(pAudio, reinterpret_cast<void*>(&FakeAudio))};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array();
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({"ninguno: GetCurrentAtomicObjectCB (0x6A0750) no se ejecuta, el falso de RwFrameForAllObjects escribe su resultado; Multiply3x3 (0x59C790) y ~CMatrix (0x59ACD0) se ejecutan reales en el EXE"})}, {"stubbed", stubbed}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry, 16); env.watches.emplace_back(env.vtSlot, 4);
    SaveRegions();
    Run(env, ready, registered, bodies, collaborators);
}
}
