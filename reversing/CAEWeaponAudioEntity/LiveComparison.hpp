#pragma once
#include <fstream>
#include <functional>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/Audio/Entities/AEWeaponAudioEntity.h"
#include "../../source/game_sa/Audio/Managers/AESoundManager.h"
#include "../../source/game_sa/Audio/Hardware/AEAudioHardware.h"
#include "../../source/game_sa/Audio/AudioEngine.h"
#include "../../source/game_sa/Audio/AEAudioUtility.h"
#include "../../source/game_sa/Entity/Vehicle/Vehicle.h"
#include "../../source/game_sa/References.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"

// Banco local de las 20 entradas de CAEWeaponAudioEntity (0x503450..0x504F80,
// 0x5DE990, 0x507560). Alterna los hooks ya instalados por InjectHooksMain.cpp
// (NO llama a InjectHooks(): duplicaría el registro y dispararía HookCategory.h:60).
//
// Lado original: los 20 hooks de la clase deshabilitados (camino 100 % ensamblador
// original, incluidas las llamadas internas entre funciones de la clase).
// Lado candidato: los 18 hooks normales habilitados. Constructor/Destructor, que
// están bloqueados y deshabilitados en el ASI normal, se prueban aparte (fase L).
//
// Cada lado usa su propio fixture (entidad de audio, entidad física sintética,
// sonido sintético, ranuras devueltas) construido con los mismos bytes.
//
// Dependencias controladas (iguales en ambos lados, dirección GTA y del DLL):
//  - CAESoundManager::RequestNewSound: registra el CAESound pedido (banco, sonido,
//    volumen, atenuación, velocidad, doppler, flags, evento, variable cliente,
//    posición, entidad...) y devuelve una ranura propia. No suena nada.
//  - CancelSoundsOwnedByAudioEntity, AreSoundsOfThisEventPlayingForThisEntity,
//    AreSoundsPlayingInBankSlot, CancelSoundsInBankSlot: registran y devuelven el
//    valor fijado por el caso.
//  - CAEAudioHardware::IsSoundBankLoaded (valor por caso) y LoadSoundBank (solo se
//    registra; no carga bancos).
//  - CAudioEngine::IsLoadingTuneActive (valor por caso).
//  - CAEAudioUtility::ResolveProbability y GetRandomNumberInRange<float> (valores
//    por caso; se registran sus argumentos).
//  - CTimer::m_snTimeInMilliseconds y CGame::currArea: se fijan por caso y se
//    restauran.
// Dependencias reales: CAESound::Initialise/StopSoundAndForget/Register...,
// CEntity::RegisterReference/CleanUpOldReference (sobre la entidad sintética; se
// limpian después de cada lado), CAEAudioEnvironment::GetPositionRelativeToCamera
// (cámara real) y GetDistanceAttenuation, tabla de volúmenes por evento.
namespace WeaponAudioLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", msg ? msg : ""}});
    ExitProcess(91);
}

template<class M> uintptr_t MemberAddress(M m) {
    static_assert(sizeof(M) == sizeof(uintptr_t));
    uintptr_t a{};
    std::memcpy(&a, &m, sizeof(a));
    return a;
}
template<class T> T Rd(const void* base, size_t off) { T v; std::memcpy(&v, static_cast<const uint8*>(base) + off, sizeof(T)); return v; }
template<class T> void Wr(void* base, size_t off, T v) { std::memcpy(static_cast<uint8*>(base) + off, &v, sizeof(T)); }

// ---------------------------------------------------------------- entorno por caso
struct Env {
    bool   wpn{true}, col{true}, veh{true}, saw{true}; // bancos cargados
    bool   tune{};                                       // IsLoadingTuneActive
    int16  eventPlaying{};                               // AreSoundsOfThisEventPlayingForThisEntity
    int16  slotPlaying{};                                // AreSoundsPlayingInBankSlot
    std::vector<bool> probs{false};                      // ResolveProbability, en orden
    float  rnd{0.0137f};                                 // GetRandomNumberInRange<float>
    uint32 now{100000};
    int32  area{0};
    json Describe() const {
        return {{"wpn", wpn}, {"col", col}, {"veh", veh}, {"saw", saw}, {"tune", tune}, {"eventPlaying", eventPlaying},
                {"slotPlaying", slotPlaying}, {"probs", probs}, {"rnd", rnd}, {"now", now}, {"area", area}};
    }
};

// Estado inicial de los campos de la clase (desplazamientos del EXE).
struct Fields {
    uint8  spin{}, fire{}, planeIdx{}, mgState{3}, csState{4};
    uint32 flame{}, spray{}, ext{}, mg{}, cs{}, gun{};
    int    gasSound{}; // 0 nulo, 1 sonido sintético S
    json Describe() const {
        return {{"spin", spin}, {"fire", fire}, {"planeIdx", planeIdx}, {"mgState", mgState}, {"csState", csState}, {"flame", flame},
                {"spray", spray}, {"ext", ext}, {"mg", mg}, {"cs", cs}, {"gun", gun}, {"gasSound", gasSound}};
    }
};

enum class PhysKind { None, Ped, Car, Plane };

// Sonido sintético para UpdateParameters / StopFlameThrowerIdleGasLoop.
struct SoundInit {
    int32 event{-1};
    float volume{0.f}, speed{1.f}, cv{-1.f};
    json Describe() const { return {{"event", event}, {"volume", volume}, {"speed", speed}, {"cv", cv}}; }
};

constexpr size_t kEntSize = 0xB0;   // incluye +0xA0/+0xA4 que escribe 0x5DE990
constexpr size_t kPhysSize = 0xA10; // >= sizeof(CPlane)
constexpr int kSlots = 16;

struct Fixture {
    alignas(16) std::array<uint8, kEntSize> ent{};
    alignas(16) std::array<uint8, kPhysSize> phys{};
    alignas(16) std::array<uint8, sizeof(CAESound)> sound{};
    alignas(16) std::array<std::array<uint8, sizeof(CAESound)>, kSlots> slots{};
    std::vector<uint8> entBefore;

    void* E() { return ent.data(); }
    CPhysical* P() { return reinterpret_cast<CPhysical*>(phys.data()); }
    CAESound* S() { return reinterpret_cast<CAESound*>(sound.data()); }

    void Setup(const Fields& f, PhysKind kind, const CVector& pos, const SoundInit& si) {
        ent.fill(0xA5);
        phys.fill(0);
        sound.fill(0);
        for (auto& s : slots) s.fill(0);
        Wr<uint32>(E(), 0, 0x862E68);              // vtable GTA de CAEWeaponAudioEntity
        Wr<uint32>(E(), 4, 0);                     // CAEAudioEntity::m_Entity
        ::new (ent.data() + 8) CAESound();         // m_tempSound
        Wr<uint8>(E(), 0x7C, f.spin); Wr<uint8>(E(), 0x7D, f.fire); Wr<uint8>(E(), 0x7E, f.planeIdx);
        Wr<uint8>(E(), 0x7F, f.mgState); Wr<uint8>(E(), 0x80, f.csState);
        Wr<uint32>(E(), 0x84, f.flame); Wr<uint32>(E(), 0x88, f.spray); Wr<uint32>(E(), 0x8C, f.ext);
        Wr<uint32>(E(), 0x90, f.mg); Wr<uint32>(E(), 0x94, f.cs); Wr<uint32>(E(), 0x98, f.gun);
        Wr<uint32>(E(), 0x9C, f.gasSound == 1 ? uint32(uintptr_t(S())) : 0u);

        if (kind != PhysKind::None) {
            auto* e = reinterpret_cast<CEntity*>(phys.data());
            e->SetType(kind == PhysKind::Ped ? ENTITY_TYPE_PED : ENTITY_TYPE_VEHICLE);
            e->m_pReferences = nullptr;
            Wr<CVector>(phys.data(), 4, pos);      // CPlaceable::m_placement.m_vPosn
            Wr<uint32>(phys.data(), 0x14, 0);      // m_matrix nulo
            if (kind != PhysKind::Ped) {
                reinterpret_cast<CVehicle*>(phys.data())->m_nVehicleSubType = kind == PhysKind::Plane ? VEHICLE_TYPE_PLANE : VEHICLE_TYPE_AUTOMOBILE;
            }
        }

        ::new (sound.data()) CAESound();
        Wr<uint32>(S(), 4, uint32(uintptr_t(E())));
        Wr<uint32>(S(), 8, kind != PhysKind::None ? uint32(uintptr_t(P())) : 0u); // puntero crudo, sin registrar
        Wr<int32>(S(), 0xC, si.event);
        Wr<float>(S(), 0x10, si.cv);
        Wr<float>(S(), 0x14, si.volume);
        Wr<float>(S(), 0x1C, si.speed);
        entBefore = ReversingProbe::Bytes(E(), kEntSize);
    }

    json Norm(uint32 v) {
        if (v == 0) return 0;
        if (v == uint32(uintptr_t(E()))) return "E";
        if (v == uint32(uintptr_t(E())) + 8) return "T";
        if (v == uint32(uintptr_t(P()))) return "P";
        if (v == uint32(uintptr_t(S()))) return "S";
        for (int i = 0; i < kSlots; ++i) if (v == uint32(uintptr_t(slots[i].data()))) return "R" + std::to_string(i);
        return v;
    }

    json Sound(const void* p) {
        json f;
        f["bank"] = Rd<int16>(p, 0); f["sfx"] = Rd<int16>(p, 2);
        f["audioEntity"] = Norm(Rd<uint32>(p, 4)); f["phys"] = Norm(Rd<uint32>(p, 8));
        f["event"] = Rd<int32>(p, 0xC); f["cv"] = Rd<float>(p, 0x10); f["volume"] = Rd<float>(p, 0x14);
        f["rollOff"] = Rd<float>(p, 0x18); f["speed"] = Rd<float>(p, 0x1C); f["speedVariance"] = Rd<float>(p, 0x20);
        f["pos"] = {Rd<float>(p, 0x24), Rd<float>(p, 0x28), Rd<float>(p, 0x2C)};
        f["prevPos"] = {Rd<float>(p, 0x30), Rd<float>(p, 0x34), Rd<float>(p, 0x38)};
        f["frame"] = Rd<int32>(p, 0x3C); f["t0"] = Rd<int32>(p, 0x40); f["t1"] = Rd<int32>(p, 0x44);
        f["camDist"] = {Rd<float>(p, 0x48), Rd<float>(p, 0x4C)}; f["doppler"] = Rd<float>(p, 0x50);
        f["frameDelay"] = Rd<uint8>(p, 0x54); f["flags"] = Rd<uint16>(p, 0x56); f["inUse"] = Rd<uint16>(p, 0x58);
        f["playTime"] = Rd<int16>(p, 0x5C); f["physPlaying"] = Rd<int16>(p, 0x5E); f["listenerVolume"] = Rd<float>(p, 0x60);
        f["listenerSpeed"] = Rd<float>(p, 0x64); f["stopped"] = Rd<int16>(p, 0x68); f["headroom"] = Rd<float>(p, 0x6C);
        f["length"] = Rd<int16>(p, 0x70);
        return f; // excluidos: +0x5A (m_IsAudioHardwareAware) y relleno, que el original no copia
    }

    // Bytes comparados de la entidad: cabecera (vtable, m_Entity) y campos propios
    // 0x7C..0xB0. m_tempSound (+0x08..+0x7C) es memoria de trabajo: el original lo usa
    // para construir cada sonido y la reconstrucción usa un CAESound local.
    json Entity() {
        json f;
        f["vtbl"] = Rd<uint32>(E(), 0); f["entity"] = Norm(Rd<uint32>(E(), 4));
        json own = json::array();
        for (size_t o = 0x7C; o < kEntSize; o += 4) {
            const uint32 v = Rd<uint32>(E(), o);
            own.push_back(o == 0x9C ? Norm(v) : json(v));
        }
        f["own"] = own;
        return f;
    }

    // Retira todas las referencias que queden registradas sobre la entidad sintética
    // (p. ej. m_tempSound del lado original) y devuelve cuántas había.
    int CleanRefs() {
        auto* e = reinterpret_cast<CEntity*>(phys.data());
        int n = 0;
        while (e->m_pReferences) {
            auto* r = e->m_pReferences;
            e->m_pReferences = r->m_pNext;
            r->m_pNext = CReferences::pEmptyList;
            CReferences::pEmptyList = r;
            r->m_ppEntity = nullptr;
            ++n;
        }
        return n;
    }
};

// ---------------------------------------------------------------- dependencias controladas
struct Ctx {
    Fixture* fx{};
    Env      env;
    json     events = json::array();
    int      slot{};
    size_t   prob{};
};
inline Ctx* ctx{};
inline unsigned foreignCalls{};

struct SoundManagerFake {
    CAESound* RequestNewSound(CAESound* s) {
        if (!ctx) { ++foreignCalls; return nullptr; }
        // No se registra la dirección del CAESound recibido: el original usa
        // m_tempSound y la reconstrucción un CAESound local (memoria de trabajo).
        ctx->events.push_back({{"t", "sound"}, {"s", ctx->fx->Sound(s)}});
        if (ctx->slot >= kSlots) return nullptr;
        return reinterpret_cast<CAESound*>(ctx->fx->slots[ctx->slot++].data());
    }
    void CancelSoundsOwnedByAudioEntity(CAEAudioEntity* e, bool full) {
        if (!ctx) { ++foreignCalls; return; }
        ctx->events.push_back({{"t", "cancelOwned"}, {"e", ctx->fx->Norm(uint32(uintptr_t(e)))}, {"full", full}});
    }
    int16 AreSoundsOfThisEventPlayingForThisEntity(int16 ev, CAEAudioEntity* e) {
        if (!ctx) { ++foreignCalls; return 0; }
        ctx->events.push_back({{"t", "eventPlaying"}, {"ev", ev}, {"e", ctx->fx->Norm(uint32(uintptr_t(e)))}});
        return ctx->env.eventPlaying;
    }
    int16 AreSoundsPlayingInBankSlot(int16 slot) {
        if (!ctx) { ++foreignCalls; return 0; }
        ctx->events.push_back({{"t", "slotPlaying"}, {"slot", slot}});
        return ctx->env.slotPlaying;
    }
    void CancelSoundsInBankSlot(int16 slot, bool full) {
        if (!ctx) { ++foreignCalls; return; }
        ctx->events.push_back({{"t", "cancelSlot"}, {"slot", slot}, {"full", full}});
    }
};
struct HardwareFake {
    bool IsSoundBankLoaded(int16 bank, int16 slot) {
        if (!ctx) { ++foreignCalls; return true; }
        ctx->events.push_back({{"t", "isLoaded"}, {"bank", bank}, {"slot", slot}});
        const auto& e = ctx->env;
        if (bank == 0x8F && slot == 5) return e.wpn;
        if (bank == 0x27 && slot == 2) return e.col;
        if (bank == 0x8A && slot == 0x13) return e.veh;
        if (bank == 0x24 && slot == 0x28) return e.saw;
        return false;
    }
    void LoadSoundBank(int16 bank, int16 slot) {
        if (!ctx) { ++foreignCalls; return; }
        ctx->events.push_back({{"t", "load"}, {"bank", bank}, {"slot", slot}});
    }
};
struct EngineFake {
    bool IsLoadingTuneActive() {
        if (!ctx) { ++foreignCalls; return false; }
        ctx->events.push_back({{"t", "tune"}});
        return ctx->env.tune;
    }
};
inline bool __cdecl FakeProbability(float p) {
    if (!ctx) { ++foreignCalls; return false; }
    const bool r = ctx->env.probs.empty() ? false : bool(ctx->env.probs[std::min(ctx->prob, ctx->env.probs.size() - 1)]);
    ++ctx->prob;
    ctx->events.push_back({{"t", "prob"}, {"p", p}, {"r", r}});
    return r;
}
inline float __cdecl FakeRandom(float a, float b) {
    if (!ctx) { ++foreignCalls; return a; }
    ctx->events.push_back({{"t", "rand"}, {"min", a}, {"max", b}});
    return ctx->env.rnd;
}

// ---------------------------------------------------------------- comparación
// Enteros y cadenas exactos; números en coma flotante iguales o con diferencia
// <= 1e-5 * max(1, |x|). Se informa la diferencia máxima.
inline bool Similar(const json& a, const json& b, double& maxDiff, bool& exact) {
    if (a.is_number_float() || b.is_number_float()) {
        if (!a.is_number() || !b.is_number()) return false;
        const double x = a.get<double>(), y = b.get<double>();
        if (std::isnan(x) || std::isnan(y)) { if (std::isnan(x) != std::isnan(y)) return false; return true; }
        if (x == y) return true;
        exact = false;
        const double d = std::fabs(x - y);
        maxDiff = std::max(maxDiff, d);
        return d <= 1e-5 * std::max(1.0, std::fabs(x));
    }
    if (a.type() != b.type()) return false;
    if (a.is_array()) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) if (!Similar(a[i], b[i], maxDiff, exact)) return false;
        return true;
    }
    if (a.is_object()) {
        if (a.size() != b.size()) return false;
        for (auto it = a.begin(); it != a.end(); ++it) {
            if (!b.contains(it.key()) || !Similar(it.value(), b[it.key()], maxDiff, exact)) return false;
        }
        return true;
    }
    return a == b;
}

// ---------------------------------------------------------------- llamadas a las direcciones GTA
using ThisFn0 = void(__thiscall*)(void*);
inline void Call0(uint32 a, void* e) { reinterpret_cast<ThisFn0>(a)(e); }

struct Case {
    int fn{};
    json desc;
    Env env;
    Fields fields;
    PhysKind phys{PhysKind::Ped};
    float dist{3.f};
    SoundInit sound;
    std::function<void(Fixture&)> call;
};

inline void Invoke(void* p) { (*static_cast<std::function<void()>*>(p))(); }
inline bool SehInvoke(void* p, unsigned* code) {
    __try { Invoke(p); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { *code = GetExceptionCode(); return false; }
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(std::string_view cat, std::string_view name) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() == cat) {
            for (auto& h : c.Items()) if (h->Name() == name) hook = h.get();
        }
    });
    return hook;
}
struct HookGuard {
    ReversibleHooks::ReversibleHook::Base* h;
    bool enabled, locked;
    explicit HookGuard(ReversibleHooks::ReversibleHook::Base* hook) : h(hook), enabled(hook->Hooked()), locked(hook->Locked()) { h->LockState(false); }
    void Restore() { h->State(enabled); h->LockState(locked); }
    bool Restored() const { return h->Hooked() == enabled && h->Locked() == locked; }
};

inline const char* kNames[WeaponAudioProbe::Count] = {
    "Constructor", "Destructor", "Initialise", "Terminate", "Reset", "WeaponFire", "WeaponReload",
    "PlayGunSounds", "PlayGoggleSound", "ReportStealthKill", "ReportChainsawEvent",
    "PlayChainsawStopSound", "PlayWeaponLoopSound", "PlayMiniGunFireSounds", "PlayMiniGunStopSound",
    "PlayFlameThrowerSounds", "PlayFlameThrowerIdleGasLoop", "StopFlameThrowerIdleGasLoop",
    "PlayCameraSound", "UpdateParameters",
};

// ---------------------------------------------------------------- casos
inline std::vector<Case> BuildCases(float flameTarget) {
    using namespace WeaponAudioProbe;
    std::vector<Case> cs;
    const auto bankVariants = [](Env base) {
        std::vector<std::pair<std::string, Env>> v;
        Env a = base; v.emplace_back("loaded", a);
        Env b = base; b.wpn = false; b.tune = false; v.emplace_back("notLoaded", b);
        Env c = base; c.wpn = false; c.tune = true; v.emplace_back("notLoadedTune", c);
        return v;
    };

    // Initialise
    for (bool tune : {false, true})
        for (int pat = 0; pat < 2; ++pat) {
            Case c{.fn = Initialise};
            c.env.tune = tune;
            c.fields.mgState = uint8(pat); c.fields.csState = uint8(pat + 1);
            c.desc = {{"tune", tune}, {"pattern", pat}};
            c.call = [](Fixture& f) { Call0(0x503450, f.E()); };
            cs.push_back(c);
        }
    // Terminate / Reset
    for (int pat = 0; pat < 2; ++pat) {
        Case c{.fn = Terminate};
        c.fields.flame = 11u * pat; c.desc = {{"pattern", pat}};
        c.call = [](Fixture& f) { Call0(0x503480, f.E()); };
        cs.push_back(c);
    }
    for (int pat = 0; pat < 3; ++pat) {
        Case c{.fn = Reset};
        c.fields = {.spin = 1, .fire = 1, .planeIdx = 1, .mgState = uint8(1 + pat % 2), .csState = uint8(pat + 1),
                    .flame = 101u + pat, .spray = 202, .ext = 303, .mg = 404, .cs = 505, .gun = 606, .gasSound = pat == 2 ? 1 : 0};
        c.desc = {{"pattern", pat}};
        c.call = [](Fixture& f) { Call0(0x503490, f.E()); };
        cs.push_back(c);
    }
    // StopFlameThrowerIdleGasLoop
    for (int gas : {0, 1})
        for (int ev : {-1, 2}) {
            Case c{.fn = StopFlameThrowerIdleGasLoop};
            c.fields.gasSound = gas; c.sound.event = ev;
            c.desc = {{"gas", gas}, {"soundEvent", ev}};
            c.call = [](Fixture& f) { Call0(0x5034E0, f.E()); };
            cs.push_back(c);
        }
    // PlayGoggleSound
    for (int16 sfx : {int16(64), int16(10)})
        for (int ev : {0x91, 0x92})
            for (auto& [bn, env] : bankVariants({}))
                for (bool pr : {false, true}) {
                    Case c{.fn = PlayGoggleSound, .env = env};
                    c.env.probs = {pr};
                    c.desc = {{"sfx", sfx}, {"ev", ev}, {"bank", bn}, {"prob", pr}};
                    c.call = [sfx, ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int16, int32)>(0x503500)(f.E(), sfx, ev); };
                    cs.push_back(c);
                }
    // WeaponReload
    for (int type = 0; type <= 47; ++type)
        for (int ev : {0x92, 0x93}) {
            Case c{.fn = WeaponReload};
            c.desc = {{"type", type}, {"ev", ev}};
            c.call = [type, ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x503690)(f.E(), type, f.P(), ev); };
            cs.push_back(c);
        }
    for (int type = 22; type <= 34; ++type) {
        Case c{.fn = WeaponReload};
        c.desc = {{"type", type}, {"ev", 0x91}};
        c.call = [type](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x503690)(f.E(), type, f.P(), 0x91); };
        cs.push_back(c);
    }
    for (auto& [bn, env] : bankVariants({}))
        for (int ev : {0x92, 0x93}) {
            Case c{.fn = WeaponReload, .env = env};
            c.desc = {{"type", 22}, {"ev", ev}, {"bank", bn}, {"nullEntity", false}};
            c.call = [ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x503690)(f.E(), 22, f.P(), ev); };
            cs.push_back(c);
        }
    {
        Case c{.fn = WeaponReload, .phys = PhysKind::None};
        c.desc = {{"type", 22}, {"ev", 0x92}, {"nullEntity", true}};
        c.call = [](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x503690)(f.E(), 22, nullptr, 0x92); };
        cs.push_back(c);
    }
    // PlayFlameThrowerIdleGasLoop
    for (int gas : {0, 1})
        for (PhysKind k : {PhysKind::Ped, PhysKind::Plane}) {
            Case c{.fn = PlayFlameThrowerIdleGasLoop, .phys = k};
            c.fields.gasSound = gas;
            c.desc = {{"gas", gas}, {"phys", int(k)}};
            c.call = [](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*)>(0x503870)(f.E(), f.P()); };
            cs.push_back(c);
        }
    // ReportChainsawEvent
    for (int ev : {0x99, 0x9A, 0x9B, 0x91})
        for (uint8 st = 0; st <= 4; ++st)
            for (int16 playing : {int16(0), int16(1)}) {
                Case c{.fn = ReportChainsawEvent};
                c.fields.csState = st; c.fields.cs = 7; c.env.eventPlaying = playing;
                c.desc = {{"ev", ev}, {"state", st}, {"playing", playing}};
                c.call = [ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int32)>(0x503910)(f.E(), f.P(), ev); };
                cs.push_back(c);
            }
    for (int16 sp : {int16(0), int16(1)})
        for (bool tune : {false, true}) {
            Case c{.fn = ReportChainsawEvent};
            c.env.saw = false; c.env.slotPlaying = sp; c.env.tune = tune; c.fields.csState = 0;
            c.desc = {{"ev", 0x9A}, {"sawLoaded", false}, {"slotPlaying", sp}, {"tune", tune}};
            c.call = [](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int32)>(0x503910)(f.E(), f.P(), 0x9A); };
            cs.push_back(c);
        }
    // ReportStealthKill
    for (int type : {4, 0, 8})
        for (auto& [bn, env0] : bankVariants({}))
            for (bool col : {true, false})
                for (uint32 now : {100000u, 0x80000123u}) {
                    Case c{.fn = ReportStealthKill, .env = env0};
                    c.env.col = col; c.env.now = now;
                    c.desc = {{"type", type}, {"bank", bn}, {"col", col}, {"now", now}};
                    c.call = [type](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x503B20)(f.E(), type, f.P(), 0x91); };
                    cs.push_back(c);
                }
    // PlayGunSounds
    struct Sfx { const char* name; int16 d, s, l, r, t; float primary, tail; };
    const Sfx sets[] = {
        {"pistol", 0x34, 0x35, 6, 7, 8, 1.41421f, 1.f},
        {"sawnoff", 0x49, 0x4A, 0x15, 0x16, 0x17, 0.7937f, 0.93f},
        {"partial", -1, -1, 6, -1, 8, 1.f, 1.f},
        {"detonator", 0x31, -1, -1, -1, -1, 1.f, 1.f},
    };
    const float dists[] = {0.f, 3.f, 5.f, 5.5f, 8.f, 11.5f, 12.f, 40.f};
    int gunIdx = 0;
    for (int ev : {0x91, 0x95, 0x96, 0x98})
        for (float d : dists)
            for (const auto& s : sets)
                for (uint8 pIdx : {uint8(0), uint8(1)})
                    for (bool pr : {false, true})
                        for (float off : {0.f, 30.f}) {
                            if (ev != 0x95 && pIdx == 1) continue;
                            Case c{.fn = PlayGunSounds, .dist = d};
                            c.env.probs = {pr};
                            c.env.rnd = -0.02f + 0.01f * float(gunIdx++ % 5);
                            c.fields.planeIdx = pIdx;
                            c.desc = {{"ev", ev}, {"dist", d}, {"sfx", s.name}, {"planeIdx", pIdx}, {"prob", pr}, {"volumeOffset", off}};
                            c.call = [s, ev, off](Fixture& f) {
                                reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int16, int16, int16, int16, int16, int32, float, float, float)>(0x503CE0)(
                                    f.E(), f.P(), s.d, s.s, s.l, s.r, s.t, ev, off, s.primary, s.tail);
                            };
                            cs.push_back(c);
                        }
    for (uint32 lastGun : {100000u - 24u, 100000u - 25u, 100001u})
        for (auto& [bn, env] : bankVariants({})) {
            Case c{.fn = PlayGunSounds, .env = env};
            c.fields.gun = lastGun;
            c.desc = {{"ev", 0x91}, {"sfx", "pistol"}, {"lastGun", lastGun}, {"bank", bn}};
            c.call = [](Fixture& f) {
                reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int16, int16, int16, int16, int16, int32, float, float, float)>(0x503CE0)(
                    f.E(), f.P(), 0x34, 0x35, 6, 7, 8, 0x91, 0.f, 1.41421f, 1.f);
            };
            cs.push_back(c);
        }
    // PlayFlameThrowerSounds
    for (int16 dry : {int16(83), int16(-1)})
        for (int16 flame : {int16(26), int16(-1)})
            for (auto& [bn, env0] : bankVariants({}))
                for (bool veh : {true, false})
                    for (int v = 0; v < 2; ++v) {
                        Case c{.fn = PlayFlameThrowerSounds, .env = env0};
                        c.env.veh = veh;
                        const float off = v ? 0.f : -14.f, sp = v ? 0.7937f : 1.f;
                        c.desc = {{"dry", dry}, {"flame", flame}, {"bank", bn}, {"veh", veh}, {"off", off}, {"speed", sp}};
                        c.call = [dry, flame, off, sp](Fixture& f) {
                            reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int32, int32, int32, float, float)>(0x504470)(f.E(), f.P(), dry, flame, 0x91, off, sp);
                        };
                        cs.push_back(c);
                    }
    // PlayWeaponLoopSound
    for (int16 sfx : {int16(28), int16(9)})
        for (int cat : {3, 4})
            for (auto& [bn, env] : bankVariants({}))
                for (int v = 0; v < 2; ++v) {
                    Case c{.fn = PlayWeaponLoopSound, .env = env};
                    const float off = v ? 2.5f : -20.f, sp = v ? 0.7937f : 1.f;
                    c.desc = {{"sfx", sfx}, {"cat", cat}, {"bank", bn}, {"off", off}, {"speed", sp}};
                    c.call = [sfx, cat, off, sp](Fixture& f) {
                        reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int16, int32, float, float, int32)>(0x504610)(f.E(), f.P(), sfx, 0x91, off, sp, cat);
                    };
                    cs.push_back(c);
                }
    // PlayCameraSound
    for (float aud : {-14.f, 0.f, 3.5f})
        for (int ev : {0x91, 0x92})
            for (auto& [bn, env] : bankVariants({})) {
                Case c{.fn = PlayCameraSound, .env = env};
                c.desc = {{"aud", aud}, {"ev", ev}, {"bank", bn}};
                c.call = [aud, ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int32, float)>(0x5046F0)(f.E(), f.P(), ev, aud); };
                cs.push_back(c);
            }
    // PlayMiniGunFireSounds
    for (int ev : {0x91, 0x92, 0x95, 0x96, 0x97, 0x98})
        for (uint8 spin : {uint8(0), uint8(1)})
            for (uint8 fire : {uint8(0), uint8(1)})
                for (uint8 st = 0; st <= 3; ++st)
                    for (auto& [bn, env] : bankVariants({})) {
                        Case c{.fn = PlayMiniGunFireSounds, .env = env};
                        c.fields.spin = spin; c.fields.fire = fire; c.fields.mgState = st; c.fields.mg = 5;
                        c.desc = {{"ev", ev}, {"spin", spin}, {"fire", fire}, {"state", st}, {"bank", bn}};
                        c.call = [ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*, int32)>(0x5047C0)(f.E(), f.P(), ev); };
                        cs.push_back(c);
                    }
    // PlayMiniGunStopSound
    for (PhysKind k : {PhysKind::None, PhysKind::Ped, PhysKind::Car, PhysKind::Plane})
        for (uint8 st = 0; st <= 3; ++st)
            for (auto& [bn, env] : bankVariants({})) {
                Case c{.fn = PlayMiniGunStopSound, .env = env, .phys = k};
                c.fields.mgState = st;
                c.desc = {{"phys", int(k)}, {"state", st}, {"bank", bn}};
                c.call = [k](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*)>(0x504960)(f.E(), k == PhysKind::None ? nullptr : f.P()); };
                cs.push_back(c);
            }
    // PlayChainsawStopSound
    for (PhysKind k : {PhysKind::None, PhysKind::Ped})
        for (bool saw : {true, false})
            for (uint8 st = 0; st <= 4; ++st) {
                Case c{.fn = PlayChainsawStopSound, .phys = k};
                c.env.saw = saw; c.fields.csState = st;
                c.desc = {{"phys", int(k)}, {"saw", saw}, {"state", st}};
                c.call = [k](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CPhysical*)>(0x504AA0)(f.E(), k == PhysKind::None ? nullptr : f.P()); };
                cs.push_back(c);
            }
    // UpdateParameters
    const uint32 now = 100000;
    for (int16 pp : {int16(-1), int16(-2)})
        for (int ev : {8, 3})
            for (uint8 mg : {uint8(1), uint8(2)})
                for (int gas : {0, 1}) {
                    Case c{.fn = UpdateParameters};
                    c.fields.mgState = mg; c.fields.gasSound = gas; c.sound.event = ev;
                    c.desc = {{"pp", pp}, {"ev", ev}, {"mg", mg}, {"gasIsSound", gas}};
                    c.call = [pp](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CAESound*, int16)>(0x504B70)(f.E(), f.S(), pp); };
                    cs.push_back(c);
                }
    for (int ev = -1; ev <= 15; ++ev)
        for (int16 pp : {int16(0), int16(1001)})
            for (int expired = 0; expired < 3; ++expired)
                for (int s = 0; s < 5; ++s)
                    for (int v = 0; v < 3; ++v)
                        for (int32 area : {0, 1}) {
                            if (area == 1 && ev != 1) continue;
                            Case c{.fn = UpdateParameters};
                            c.env.area = area; c.env.now = now;
                            const uint32 last = expired == 0 ? now - 300 : expired == 1 ? now - 350 : now - 401;
                            c.fields = {.spin = 1, .fire = 1, .mgState = uint8(s % 4), .csState = uint8(s),
                                        .flame = last, .spray = last, .ext = last, .mg = last, .cs = last, .gun = 0};
                            const float vols[3] = {-30.f, flameTarget + 1.f, 0.5f};
                            const float speeds[3] = {0.84f, 0.9f, 1.2f};
                            const float cvs[3] = {float(now - 821), float(now - 820), float(now - 2201)};
                            c.sound = {.event = ev, .volume = vols[v], .speed = speeds[v], .cv = cvs[v]};
                            c.desc = {{"ev", ev}, {"pp", pp}, {"expired", expired}, {"state", s}, {"variant", v}, {"area", area}};
                            c.call = [pp](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CAESound*, int16)>(0x504B70)(f.E(), f.S(), pp); };
                            cs.push_back(c);
                        }
    for (float vol : {flameTarget - 1.f, flameTarget - 2.f, flameTarget - 2.5f, flameTarget})
        for (int ev : {2})
            {
                Case c{.fn = UpdateParameters};
                c.fields.flame = now;
                c.sound = {.event = ev, .volume = vol};
                c.desc = {{"ev", ev}, {"pp", 0}, {"flameVolume", vol}};
                c.call = [](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, CAESound*, int16)>(0x504B70)(f.E(), f.S(), 0); };
                cs.push_back(c);
            }
    // WeaponFire
    for (int type = 0; type <= 50; ++type)
        for (int ev : {0x91, 0x95, 0x96, 0x97}) {
            Case c{.fn = WeaponFire};
            const bool hot = (type + ev) % 2;
            c.fields.flame = hot ? 99999u : 0u; c.fields.spray = hot ? 99999u : 0u; c.fields.ext = hot ? 99999u : 0u;
            c.desc = {{"type", type}, {"ev", ev}, {"lastTimesSet", hot}};
            c.call = [type, ev](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x504F80)(f.E(), type, f.P(), ev); };
            cs.push_back(c);
        }
    for (int type : {22, 37}) {
        Case c{.fn = WeaponFire, .phys = PhysKind::None};
        c.desc = {{"type", type}, {"ev", 0x91}, {"nullEntity", true}};
        c.call = [type](Fixture& f) { reinterpret_cast<void(__thiscall*)(void*, int32, CPhysical*, int32)>(0x504F80)(f.E(), type, nullptr, 0x91); };
        cs.push_back(c);
    }
    return cs;
}

struct SideResult {
    json snap;
    unsigned calls[WeaponAudioProbe::Count]{};
    unsigned total{};
    int refsLeft{};
    bool ok{};
    unsigned code{};
};

inline SideResult RunSide(Fixture& fx, const Case& c, const CVector& camPos) {
    SideResult r;
    const CVector pos = camPos + CVector{c.dist, 0.f, 0.f};
    fx.Setup(c.fields, c.phys, pos, c.sound);
    Ctx cx; cx.fx = &fx; cx.env = c.env;
    ctx = &cx;
    const uint32 savedTime = CTimer::m_snTimeInMilliseconds;
    auto& rawArea = *reinterpret_cast<int32*>(0xB72914); // CGame::currArea
    const int32 savedArea = rawArea;
    CTimer::m_snTimeInMilliseconds = c.env.now;
    rawArea = c.env.area;
    unsigned before[WeaponAudioProbe::Count];
    std::memcpy(before, WeaponAudioProbe::calls, sizeof(before));
    std::function<void()> fn = [&] { c.call(fx); };
    r.ok = SehInvoke(&fn, &r.code);
    for (int i = 0; i < WeaponAudioProbe::Count; ++i) { r.calls[i] = WeaponAudioProbe::calls[i] - before[i]; r.total += r.calls[i]; }
    CTimer::m_snTimeInMilliseconds = savedTime;
    rawArea = savedArea;
    ctx = nullptr;
    if (r.ok) {
        r.snap = {{"events", cx.events}, {"entity", fx.Entity()}, {"sound", fx.Sound(fx.S())}};
    }
    // Restaura referencias: m_tempSound (lado original) y el sonido sintético.
    reinterpret_cast<CAESound*>(fx.ent.data() + 8)->m_PhysicalEntity = nullptr;
    r.refsLeft = fx.CleanRefs();
    return r;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "weaponaudio-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "weaponaudio-runtime.jsonl");
        Write({{"event", "waiting_for_player"}});
    }
    auto* player = FindPlayerPed();
    if (!player || !player->m_matrix || CCutsceneMgr::IsRunning()) return;
    done = true;
    auto report = _CrtSetReportHook(Report);

    std::vector<std::unique_ptr<HookGuard>> guards;
    ReversibleHooks::ReversibleHook::Base* hooks[WeaponAudioProbe::Count]{};
    json missing = json::array(), before = json::array();
    for (int i = 0; i < WeaponAudioProbe::Count; ++i) {
        hooks[i] = FindHook("CAEWeaponAudioEntity", kNames[i]);
        if (!hooks[i]) { missing.push_back(kNames[i]); continue; }
        before.push_back({{"name", kNames[i]}, {"hooked", hooks[i]->Hooked()}, {"locked", hooks[i]->Locked()}});
        guards.push_back(std::make_unique<HookGuard>(hooks[i]));
    }
    if (!missing.empty()) {
        Write({{"event", "error"}, {"reason", "missing hook"}, {"missing", missing}});
        for (auto& g : guards) g->Restore();
        _CrtSetReportHook(report);
        return;
    }
    const auto livePlayer = ReversingProbe::Bytes(player, sizeof(CPlayerPed));
    const CVector camPos = TheCamera.GetPosition();
    const uint32 timeBefore = CTimer::m_snTimeInMilliseconds;
    const int32 areaBefore = *reinterpret_cast<int32*>(0xB72914);
    const auto refsEmptyBefore = CReferences::ListSize(CReferences::pEmptyList);
    // Tabla de volúmenes por evento (puntero en 0xBD00F8), igual que lee el original.
    const float flameTarget = float((*reinterpret_cast<int8**>(0xBD00F8))[AE_WEAPON_FIRE]) - 14.f;
    Write({{"event", "player_ready"}, {"independent_inputs", true}, {"hooks_before", before},
           {"camera", {camPos.x, camPos.y, camPos.z}}, {"references_free", refsEmptyBefore}, {"flame_target", flameTarget}});

    const auto SetSide = [&](bool candidate) {
        for (int i = WeaponAudioProbe::Initialise; i < WeaponAudioProbe::Count; ++i) hooks[i]->State(candidate);
        hooks[WeaponAudioProbe::Constructor]->State(false);
        hooks[WeaponAudioProbe::Destructor]->State(false);
    };

    bool passed = true;
    int pairs = 0;
    double maxDiff = 0;
    int inexact = 0;
    std::map<std::string, int> perFn;
    {
        ReversingProbe::EntryPatch reqGta(0x4EFB10, reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::RequestNewSound)));
        ReversingProbe::EntryPatch reqDll(MemberAddress(&CAESoundManager::RequestNewSound), reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::RequestNewSound)));
        ReversingProbe::EntryPatch cancelGta(0x4EFCD0, reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::CancelSoundsOwnedByAudioEntity)));
        ReversingProbe::EntryPatch cancelDll(MemberAddress(&CAESoundManager::CancelSoundsOwnedByAudioEntity), reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::CancelSoundsOwnedByAudioEntity)));
        ReversingProbe::EntryPatch evGta(0x4EF570, reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::AreSoundsOfThisEventPlayingForThisEntity)));
        ReversingProbe::EntryPatch evDll(MemberAddress(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity), reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::AreSoundsOfThisEventPlayingForThisEntity)));
        ReversingProbe::EntryPatch slotGta(0x4EF520, reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::AreSoundsPlayingInBankSlot)));
        ReversingProbe::EntryPatch slotDll(MemberAddress(&CAESoundManager::AreSoundsPlayingInBankSlot), reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::AreSoundsPlayingInBankSlot)));
        ReversingProbe::EntryPatch cslotGta(0x4EFC60, reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::CancelSoundsInBankSlot)));
        ReversingProbe::EntryPatch cslotDll(MemberAddress(&CAESoundManager::CancelSoundsInBankSlot), reinterpret_cast<void*>(MemberAddress(&SoundManagerFake::CancelSoundsInBankSlot)));
        ReversingProbe::EntryPatch isLGta(0x4D88C0, reinterpret_cast<void*>(MemberAddress(&HardwareFake::IsSoundBankLoaded)));
        ReversingProbe::EntryPatch isLDll(MemberAddress(&CAEAudioHardware::IsSoundBankLoaded), reinterpret_cast<void*>(MemberAddress(&HardwareFake::IsSoundBankLoaded)));
        ReversingProbe::EntryPatch loadGta(0x4D88A0, reinterpret_cast<void*>(MemberAddress(&HardwareFake::LoadSoundBank)));
        ReversingProbe::EntryPatch loadDll(MemberAddress(&CAEAudioHardware::LoadSoundBank), reinterpret_cast<void*>(MemberAddress(&HardwareFake::LoadSoundBank)));
        ReversingProbe::EntryPatch tuneGta(0x506D90, reinterpret_cast<void*>(MemberAddress(&EngineFake::IsLoadingTuneActive)));
        ReversingProbe::EntryPatch tuneDll(MemberAddress(&CAudioEngine::IsLoadingTuneActive), reinterpret_cast<void*>(MemberAddress(&EngineFake::IsLoadingTuneActive)));
        ReversingProbe::EntryPatch probGta(0x4D9C80, FakeProbability);
        ReversingProbe::EntryPatch probDll(std::bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability), FakeProbability);
        ReversingProbe::EntryPatch randGta(0x4D9C50, FakeRandom);
        ReversingProbe::EntryPatch randDll(std::bit_cast<uintptr_t>(static_cast<float (*)(float, float)>(&CAEAudioUtility::GetRandomNumberInRange<float>)), FakeRandom);

        const auto cases = BuildCases(flameTarget);
        auto a = std::make_unique<Fixture>();
        auto b = std::make_unique<Fixture>();
        for (size_t i = 0; i < cases.size(); ++i) {
            const auto& c = cases[i];
            SetSide(false);
            const auto o = RunSide(*a, c, camPos);
            SetSide(true);
            const auto k = RunSide(*b, c, camPos);
            SetSide(false);
            bool exact = true;
            double d = 0;
            const bool equal = o.ok && k.ok && Similar(o.snap, k.snap, d, exact);
            const bool counted = o.total == 0 && k.calls[c.fn] >= 1;
            const bool ok = equal && counted;
            if (!exact) ++inexact;
            maxDiff = std::max(maxDiff, d);
            passed = passed && ok;
            ++perFn[kNames[c.fn]];
            json rec = {{"event", "comparison"}, {"phase", "pairs"}, {"pair", pairs}, {"fn", kNames[c.fn]}, {"case", c.desc}, {"env", c.env.Describe()},
                        {"fields", c.fields.Describe()}, {"phys", int(c.phys)}, {"dist", c.dist}, {"sound_init", c.sound.Describe()},
                        {"original", o.snap}, {"equal", equal}, {"exact", exact}, {"max_diff", d},
                        {"original_candidate_calls", o.total}, {"candidate_calls", k.calls[c.fn]}, {"candidate_total_calls", k.total},
                        {"refs_left", {o.refsLeft, k.refsLeft}}, {"passed", ok}};
            if (!equal) rec["candidate"] = k.snap;
            if (!o.ok || !k.ok) rec["seh"] = {o.ok, o.code, k.ok, k.code};
            Write(rec);
            ++pairs;
        }

        // ------------------------------------------------ fase L: entradas bloqueadas
        // 0x5DE990 (ctor de CAEPedWeaponAudioEntity) y 0x507560 (ctor de
        // CAEWeaponAudioEntity) frente a Constructor()/Destructor(). Se esperan
        // diferencias: documentan por qué siguen bloqueadas.
        for (int which : {WeaponAudioProbe::Constructor, WeaponAudioProbe::Destructor}) {
            const uint32 addr = which == WeaponAudioProbe::Constructor ? 0x5DE990 : 0x507560;
            json sides[2];
            unsigned counts[2]{};
            for (int side = 0; side < 2; ++side) {
                auto& fx = side ? *b : *a;
                // Campos distintos de cero y vtable/m_Entity ficticios para que se vea
                // qué escribe realmente cada lado.
                fx.Setup({.spin = 1, .fire = 1, .planeIdx = 1, .mgState = 2, .csState = 2, .flame = 11, .spray = 22, .ext = 33,
                          .mg = 44, .cs = 55, .gun = 66, .gasSound = 1}, PhysKind::Ped, camPos, {});
                Wr<uint32>(fx.E(), 0, 0x11111111u);
                Wr<uint32>(fx.E(), 4, 0x22222222u);
                hooks[which]->State(side == 1);
                const unsigned c0 = WeaponAudioProbe::calls[which];
                std::function<void()> fn = [&] { Call0(addr, fx.E()); };
                unsigned code{};
                const bool ok = SehInvoke(&fn, &code);
                counts[side] = WeaponAudioProbe::calls[which] - c0;
                json bytes = json::array();
                for (size_t o = 0; o < kEntSize; o += 4) bytes.push_back(o == 0 ? json(Rd<uint32>(fx.E(), o)) : fx.Norm(Rd<uint32>(fx.E(), o)));
                sides[side] = {{"ok", ok}, {"code", code}, {"dwords", bytes}};
                hooks[which]->State(false);
                reinterpret_cast<CAESound*>(fx.ent.data() + 8)->m_PhysicalEntity = nullptr;
                fx.CleanRefs();
            }
            Write({{"event", "locked_comparison"}, {"fn", kNames[which]}, {"address", addr}, {"original", sides[0]}, {"candidate", sides[1]},
                   {"equal", sides[0] == sides[1]}, {"original_candidate_calls", counts[0]}, {"candidate_calls", counts[1]}});
        }
    }

    for (auto& g : guards) g->Restore();
    bool hooksRestored = true;
    for (auto& g : guards) hooksRestored = hooksRestored && g->Restored();
    const bool restored = hooksRestored && CTimer::m_snTimeInMilliseconds == timeBefore && *reinterpret_cast<int32*>(0xB72914) == areaBefore
        && CReferences::ListSize(CReferences::pEmptyList) == refsEmptyBefore
        && ReversingProbe::Bytes(player, sizeof(CPlayerPed)) == livePlayer;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs", pairs}, {"per_function", perFn}, {"inexact_pairs", inexact},
           {"max_float_diff", maxDiff}, {"foreign_calls", foreignCalls}, {"state_restored", restored}, {"hooks_restored", hooksRestored},
           {"references_free_after", CReferences::ListSize(CReferences::pEmptyList)}});
    _CrtSetReportHook(report);
}
}
