#pragma once
// Banco Dependencies22B: CVehicle::DoHeadLightEffect (0x6E0A50) y CVehicle::DoTailLightEffect (0x6E1780).
// Cada caso ejecuta la MISMA entrada del EXE con el hook RH apagado (bytes originales) y activo (version de la DLL), con los mismos colaboradores falsos
// instalados en la direccion del EXE y en la entrada de la DLL (EntryPatch): CCoronas::RegisterCorona (tipo, 0x6FC580), FindPlayerVehicle (0x56E0D0) y
// CDamageManager::GetPanelStatus (0x6C2180). CMatrix * CVector (0x59C890), CVector::NormaliseAndMag (0x59C970) y _ftol2 (0x821B40) son puras y no se sustituyen:
// el EXE las ejecuta tal cual (el hook de CVector esta comentado en InjectHooksMain.cpp) y la DLL las reproduce en X87Math.cpp (VehHeadTail*). Ver README.md.
#include <array>
#include <bit>
#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Models/VehicleModelInfo.h"
#include "tHandlingData.h"
#include "DamageManager.h"
#include "Camera.h"
#include "Cam.h"
#include "Coronas.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

namespace Dependencies22BLive {
inline bool done{};
inline std::ofstream log;

struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline constexpr Fn fns[]{
    {"CVehicle", "DoHeadLightEffect", 0x6E0A50, 4096},
    {"CVehicle", "DoTailLightEffect", 0x6E1780, 4096},
};
enum FnIndex { kHead, kTail };

// Los desplazamientos que usa el EXE (0x6E0A50 / 0x6E1780) y que la reconstruccion lee por nombre: si alguno no coincide el banco ni empieza.
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && sizeof(CMatrix) == 0x48 && sizeof(CMatrixLink) >= 0x48);
static_assert(offsetof(CPlaceable, m_placement) + offsetof(CSimpleTransform, m_fHeading) == 0x10);
static_assert(offsetof(CVehicle, m_nModelIndex) == 0x22 && offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(CVehicle, m_pDriver) == 0x460);
static_assert(offsetof(CVehicle, m_BrakePedal) == 0x4A0 && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CAutomobile, m_damageManager) == 0x5A0);
static_assert(offsetof(CVehicleModelInfo, m_pVehicleStruct) == 0x5C && offsetof(tHandlingData, m_nHandlingFlags) == 0xD0);
static_assert(offsetof(CCamera, m_nActiveCam) == 0x59 && offsetof(CCamera, m_fLODDistMultiplier) == 0xF0 && offsetof(CCamera, m_aCams) == 0x174 && sizeof(CCam) == 0x238);
static_assert(offsetof(CCam, m_nMode) == 0xC && sizeof(CCamera) == 0xD78);
static_assert(sizeof(CAutomobile) >= 0x5A0 + sizeof(CDamageManager));
static_assert(sizeof(CAutomobile) == Dependencies22BFix::kVehicleBytes && sizeof(CMatrixLink) == Dependencies22BFix::kMatrixBytes && sizeof(CVehicleModelInfo) == Dependencies22BFix::kModelInfoBytes);
static_assert(sizeof(CVehicleModelInfo::CVehicleStructure) == Dependencies22BFix::kVStructBytes && sizeof(tHandlingData) == Dependencies22BFix::kHandlingBytes && sizeof(CCamera) == Dependencies22BFix::kCameraBytes);

// ------------------------------------------------------------------ rutas y modulos
inline uintptr_t Resolve(uintptr_t p) { for (int i = 0; i < 16 && *reinterpret_cast<uint8*>(p) == 0xE9; ++i) p += 5 + *reinterpret_cast<int32*>(p + 1); return p; }
inline HMODULE ModuleOf(uintptr_t p) { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(p), &m); return m; }
inline void ModuleMarker() {}

// ------------------------------------------------------------------ estado del coma flotante (como Dependencies21D)
struct Fpu { std::array<uint8, 108> state; uint32 mxcsr; bool operator==(const Fpu&) const = default; };
inline Fpu CaptureFP() { Fpu f{}; auto* p = f.state.data(); auto* m = &f.mxcsr; __asm { mov eax, p } __asm { fnsave [eax] } __asm { frstor [eax] } __asm { mov eax, m } __asm { stmxcsr [eax] } return f; }
inline void RestoreFP(const Fpu& f) { const auto* p = f.state.data(); const auto m = f.mxcsr; __asm { mov eax, p } __asm { frstor [eax] } __asm { ldmxcsr m } }
inline std::function<bool()> pumpStateCheck; inline bool pumpStateOK = true; inline bool writeFPActive{}; inline Fpu writeFP{};
struct PumpObserverGuard { ~PumpObserverGuard() { pumpStateCheck = {}; writeFPActive = false; } };
inline void Write(const json& r) {
    const auto enteringFP = CaptureFP(); const auto& baseline = writeFPActive ? writeFP : enteringFP; RestoreFP(baseline);
    if (pumpStateCheck) pumpStateOK &= pumpStateCheck(); RestoreFP(baseline); ReversingProbe::PumpMessages(); RestoreFP(baseline); if (pumpStateCheck) pumpStateOK &= pumpStateCheck(); RestoreFP(baseline);
    auto record = r; if (record.value("event", "") == "complete" && !pumpStateOK) { record["event"] = "error"; record["state_restored"] = false; }
    if (pumpStateCheck) record["pump_state_intact"] = pumpStateOK; log << record.dump() << '\n'; log.flush(); RestoreFP(baseline);
}

using namespace Dependencies22BFix;
// ms_modelInfoPtrs del EXE (0xA9B0C8): el modelo del caso se desvia a un CVehicleModelInfo propio y se restaura al terminar el par.
inline uint32* ModelSlot(uint32 model) { return reinterpret_cast<uint32*>(0xA9B0C8) + model; }
inline constexpr uint32 kModelSlots = 640;   // las fixtures usan indices de modelo 0..610: se comprueba que todas las ranuras vuelven a su valor

inline json Ptr(const void* p) {
    const auto u = reinterpret_cast<uintptr_t>(p);
    if (!u) return nullptr;
#define TAG(name, off, size) if (u >= uintptr_t(arena.data() + off) && u < uintptr_t(arena.data() + off + size)) return json::array({name, u - uintptr_t(arena.data() + off)});
    TAG("veh", Layout::veh, Layout::vehSize) TAG("matVeh", Layout::matVeh, Layout::matSize) TAG("matArg", Layout::matArg, Layout::matSize) TAG("matCam", Layout::matCam, Layout::matSize)
    TAG("modelInfo", Layout::modelInfo, Layout::modelInfoSize) TAG("vstruct", Layout::vstruct, Layout::vstructSize) TAG("handling", Layout::handling, Layout::handlingSize)
    TAG("driver", Layout::driver, Layout::driverSize) TAG("other", Layout::other, Layout::otherSize)
#undef TAG
    if (u >= reinterpret_cast<uintptr_t>(&TheCamera) && u < reinterpret_cast<uintptr_t>(&TheCamera) + sizeof(CCamera)) return json::array({"camera", u - reinterpret_cast<uintptr_t>(&TheCamera)});
    { // la pila del hilo puede caer dentro de 0x400000..0x1000000: sus locales (el vector que se pasa a RegisterCorona) cambian de direccion entre el EXE y la DLL
        const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
        if (u >= reinterpret_cast<uintptr_t>(tib->StackLimit) && u < reinterpret_cast<uintptr_t>(tib->StackBase)) return "local";
    }
    if (u >= 0x400000 && u < 0x1000000) return json::array({"exe", u});
    return "local";
}

// ------------------------------------------------------------------ colaboradores falsos
inline json trace = json::array();
// Registra el id como desplazamiento respecto del vehiculo (el id de la corona es una direccion del propio vehiculo).
inline json CoronaId(uint32 id) { return int64(int32(id - uint32(uintptr_t(Veh())))); }
// CCoronas::RegisterCorona (tipo): el EXE (0x6FC580) y la DLL pasan los mismos 21 dwords de pila; r, g, b y alfa son bytes (el original los lee con mov al, [esp+..]: solo cuenta el byte bajo),
// el resto de los enteros se registran como dwords completos (constantes en el EXE; zero-extendidos por la DLL).
inline void __cdecl FCorona(uint32 id, uint32 attach, uint32 r, uint32 g, uint32 b, uint32 a, const float* pos, uint32 size, uint32 range, uint32 type, uint32 flare, uint32 refl, uint32 los,
                            uint32 trail, uint32 angle, uint32 neon, uint32 pull, uint32 fullBright, uint32 fade, uint32 below, uint32 white) {
    trace.push_back({"register_corona", CoronaId(id), Ptr(reinterpret_cast<void*>(uintptr_t(attach))), r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), size, range, type, flare, refl, los, trail,
                     angle, neon, pull, fullBright, fade, below, white});
}
inline CVehicle* __cdecl FFindPlayerVehicle(int32 playerId, int32 includeRemote) {
    trace.push_back({"find_player_vehicle", playerId, includeRemote & 0xFF});
    return G.findVehicle == 0 ? nullptr : G.findVehicle == 1 ? reinterpret_cast<CVehicle*>(Veh()) : reinterpret_cast<CVehicle*>(Other());
}
inline uint32 __fastcall FPanelStatus(CDamageManager* self, void*, uint32 panel) {
    trace.push_back({"get_panel_status", Ptr(self), panel & 0xFF});
    return G.panel;
}

// ------------------------------------------------------------------ llamada con captura de ABI y x87 (adaptadores __asm solo en el banco)
struct Frame {
    uint32 eax{}, espBefore{}, espAfter{}, ebxBefore{}, ebxAfter{}, esiBefore{}, esiAfter{}, ediBefore{}, ediAfter{}, ebpBefore{}, ebpAfter{}, sehBefore{}, sehAfter{};
    std::array<uint8, 28> envBefore{}, envAfter{};
};
inline unsigned EnvWord(const std::array<uint8, 28>& e, int field) { uint16 v; std::memcpy(&v, e.data() + field, 2); return v; }
__declspec(noinline) inline Frame CallFour(uintptr_t entry, void* self, uint32 a1, uint32 a2, uint32 a3, uint32 a4) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}, rv{}; uint8* e0 = r.envBefore.data(); uint8* e1 = r.envAfter.data();
    __asm {
        mov eax, e0
        fnstenv [eax]
        fldenv [eax]
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a4
        push a3
        push a2
        push a1
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov rv, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, e1
        fnstenv [eax]
        fldenv [eax]
    }
    r.eax = rv; r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}
__declspec(noinline) inline Frame CallSix(uintptr_t entry, void* self, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}, rv{}; uint8* e0 = r.envBefore.data(); uint8* e1 = r.envAfter.data();
    __asm {
        mov eax, e0
        fnstenv [eax]
        fldenv [eax]
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a6
        push a5
        push a4
        push a3
        push a2
        push a1
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov rv, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, e1
        fnstenv [eax]
        fldenv [eax]
    }
    r.eax = rv; r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}

// ------------------------------------------------------------------ captura / restauracion
inline std::vector<uint8> Capture() {
    std::vector<uint8> v(arena.begin(), arena.end());
    v.insert(v.end(), CamRaw(), CamRaw() + sizeof(CCamera));
    return v;
}
inline void Restore(const std::vector<uint8>& v) {
    std::memcpy(arena.data(), v.data(), arena.size());
    std::memcpy(&TheCamera, v.data() + arena.size(), sizeof(CCamera));
}
inline json Diff(const std::vector<uint8>& before, const std::vector<uint8>& after) {
    json runs = json::array();
    static const char* hex = "0123456789abcdef";
    for (size_t i = 0; i < before.size();) {
        if (before[i] == after[i]) { ++i; continue; }
        size_t j = i; std::string bytes;
        while (j < before.size() && before[j] != after[j]) { bytes += hex[after[j] >> 4]; bytes += hex[after[j] & 15]; ++j; }
        runs.push_back(json::array({i, bytes})); i = j;
    }
    return runs;
}
inline bool GuardsOK() {
    for (size_t g : {size_t(0), Layout::veh + Layout::vehSize, Layout::matVeh + Layout::matSize, Layout::matArg + Layout::matSize, Layout::matCam + Layout::matSize, Layout::modelInfo + Layout::modelInfoSize,
                     Layout::vstruct + Layout::vstructSize, Layout::handling + Layout::handlingSize, Layout::driver + Layout::driverSize, Layout::other + Layout::otherSize})
        for (size_t i = 0; i < kGuard; ++i) if (arena[g + i] != 0xA5) return false;
    return true;
}

// ------------------------------------------------------------------ colaboradores: parches temporales en la direccion del EXE y en la entrada de la DLL
struct Fakes {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
    template <class F> void Both(uintptr_t exeEntry, uintptr_t dllEntry, F fake) {
        patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(exeEntry, fake));
        patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(Resolve(dllEntry), fake));
    }
    Fakes() {
        using std::bit_cast;
        using CoronaFn = void (*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, eCoronaType, eCoronaFlareType, eCoronaReflType, eCoronaLOSCheck, eCoronaTrail, float, bool, float, bool, float, bool, bool);
        Both(0x6FC580, bit_cast<uintptr_t>(static_cast<CoronaFn>(&CCoronas::RegisterCorona)), FCorona);
        Both(0x56E0D0, bit_cast<uintptr_t>(&FindPlayerVehicle), FFindPlayerVehicle);
        Both(0x6C2180, bit_cast<uintptr_t>(&CDamageManager::GetPanelStatus), FPanelStatus);
    }
};

// EAX completo solo lo lee el llamador como AL (los 10 llamadores de DoVehicleLights hacen `shl/xor/and` sobre AL, ver README.md); ECX/EDX/EAX alto no se leen.
inline json FrameJson(const Frame& f) {
    return {{"al", f.eax & 0xFF}, {"fcw", EnvWord(f.envAfter, 0)}, {"fsw_top", (EnvWord(f.envAfter, 4) >> 11) & 7}, {"ftw", EnvWord(f.envAfter, 8)}};
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies22b-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "dependencies22b-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("dependencies22b"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;

    camBase = reinterpret_cast<uint8*>(&TheCamera);
    const auto fp = CaptureFP(); writeFP = fp; writeFPActive = true; pumpStateOK = true; PumpObserverGuard pumpGuard;
    const auto cameraSaved = ReversingProbe::Bytes(&TheCamera, sizeof(CCamera));
    OriginalDependencies499::HookRoute routeHead("CVehicle", "DoHeadLightEffect", 0x6E0A50, FunctionToVoidPtr(&CVehicle::DoHeadLightEffect));
    OriginalDependencies499::HookRoute routeTail("CVehicle", "DoTailLightEffect", 0x6E1780, FunctionToVoidPtr(&CVehicle::DoTailLightEffect));
    OriginalDependencies499::HookRoute* route[2]{&routeHead, &routeTail};
    std::vector<uint8> mainOff[2];
    for (int i = 0; i < 2; ++i) mainOff[i] = ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address), 64);

    auto routeProof = [&](int f, bool candidate) {
        const auto entry = fns[f].address;
        return ReversingProbe::JumpsToDll(entry) == candidate
            && (candidate ? (Resolve(entry) == route[f]->Body() && ModuleOf(route[f]->Body()) == ModuleOf(uintptr_t(&ModuleMarker)))
                          : (Resolve(entry) == entry && ModuleOf(entry) == GetModuleHandleA(nullptr) && mainOff[f] == ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 64)));
    };
    bool routesReady = true;
    for (int f = 0; f < 2; ++f) routesReady &= route[f]->Registered() && route[f]->Invoke(true, [&] { return routeProof(f, true); }) && routeProof(f, false);

    // Contrato de la fixture: el bit 0x20 del byte 0x428 del vehiculo es bIsHandbrakeOn, y los tres colaboradores se resuelven a cuerpos de la DLL distintos del EXE.
    bool contract = true;
    {
        std::memset(arena.data(), 0, arena.size());
        auto* v = reinterpret_cast<CVehicle*>(Veh());
        Put<uint32>(Veh(), 0x428, 0x20); contract &= v->vehicleFlags.bIsHandbrakeOn == 1;
        Put<uint32>(Veh(), 0x428, ~0x20u); contract &= v->vehicleFlags.bIsHandbrakeOn == 0;
        Put<uint32>(Veh(), 0x428, 0);
    }
    using CoronaFn = void (*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, eCoronaType, eCoronaFlareType, eCoronaReflType, eCoronaLOSCheck, eCoronaTrail, float, bool, float, bool, float, bool, bool);
    const uintptr_t coronaBody = Resolve(std::bit_cast<uintptr_t>(static_cast<CoronaFn>(&CCoronas::RegisterCorona)));
    const uintptr_t findBody = Resolve(std::bit_cast<uintptr_t>(&FindPlayerVehicle));
    const uintptr_t panelBody = Resolve(std::bit_cast<uintptr_t>(&CDamageManager::GetPanelStatus));
    const bool bodiesOk = ModuleOf(coronaBody) == ModuleOf(uintptr_t(&ModuleMarker)) && ModuleOf(findBody) == ModuleOf(uintptr_t(&ModuleMarker)) && ModuleOf(panelBody) == ModuleOf(uintptr_t(&ModuleMarker));
    const auto slotSaved = [&](uint32 m) { return *ModelSlot(m); };
    uint32 slotsBefore[kModelSlots]{}; for (uint32 m = 0; m < kModelSlots; ++m) slotsBefore[m] = slotSaved(m);
    const auto input0 = Capture();
    const bool ready = routesReady && contract && bodiesOk && input0.size() == arena.size() + sizeof(CCamera);
    json collaborators = json::array({json::array({0x6FC580, coronaBody}), json::array({0x56E0D0, findBody}), json::array({0x6C2180, panelBody})});
    pumpStateCheck = [&] { return ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp; };
    RestoreFP(fp);
    json routesJson = json::array(); for (int f = 0; f < 2; ++f) routesJson.push_back({{"function", fns[f].name}, {"registered", route[f]->Registered()}, {"body", route[f]->Body()}, {"module_ok", ModuleOf(route[f]->Body()) == ModuleOf(uintptr_t(&ModuleMarker))}});
    Write({{"event", "begin"}, {"registered_hook", routesReady}, {"ready", ready}, {"target_routes_ready", routesReady}, {"fixture_contract", contract}, {"collaborator_bodies_ok", bodiesOk}, {"routes", routesJson},
           {"collaborators", collaborators}, {"probe_module", uintptr_t(ModuleOf(uintptr_t(&ModuleMarker)))}, {"arena_bytes", arena.size()}, {"camera_bytes", sizeof(CCamera)}, {"vehicle_bytes", sizeof(CAutomobile)},
           {"scope", "vehiculo, matrices, modelo, dummies y manejo en arena propia; camara real guardada y restaurada entera; ms_modelInfoPtrs desviado por caso y restaurado; "
                     "RegisterCorona, FindPlayerVehicle y GetPanelStatus falsos en EXE y DLL; 0x59C890, 0x59C970 y _ftol2 reales"}});
    if (!ready) { Write({{"event", "error"}, {"reason", "routes/fixture preflight failed"}, {"pairs", 0}, {"state_restored", true}}); return; }

    bool passed = true; unsigned total = 0;
    for (int f = 0; f < 2; ++f) {
        unsigned failures = 0;
        const bool tail = f == kTail;
        const std::string full = std::string(fns[f].cls) + "::" + fns[f].name;
        for (int n = 0; n < fns[f].cases; ++n) {
            fixRng = Rng(uint64(f + 1) * 1000003ull + uint64(n) * 7919ull + 17ull);
            const json profile = SetupCase(tail);
            const auto input = Capture();
            const Cfg cfg0 = G;
            const uint32 slotValue = *ModelSlot(modelIndex);
            *ModelSlot(modelIndex) = uint32(uintptr_t(ModelInfo()));
            std::unique_ptr<Fakes> fakes = std::make_unique<Fakes>();
            auto invoke = [&](bool candidate) -> json {
                Restore(input); G = cfg0; trace = json::array(); RestoreFP(fp);
                bool routesOk = false;
                const auto frame = route[f]->Invoke(candidate, [&] {
                    routesOk = routeProof(f, candidate); RestoreFP(fp);
                    void* matrix = (args.matrixIsVehicle && Get<void*>(Veh(), 0x14)) ? static_cast<void*>(MatVeh()) : static_cast<void*>(MatArg());
                    const uint32 isRight = args.isRight | (args.unused & 0xFFFFFF00u), disabled = args.disabledOrAlarm | ((args.unused * 2654435761u) & 0xFFFFFF00u);
                    Frame r = !tail ? CallFour(fns[f].address, Veh(), uint32(args.lightId), uint32(uintptr_t(matrix)), isRight, disabled)
                                    : CallSix(fns[f].address, Veh(), uint32(args.lightId), uint32(uintptr_t(matrix)), isRight, disabled, args.unused, args.staticEmission | ((args.unused * 3u) & 0xFFFFFF00u));
                    routesOk &= routeProof(f, candidate); return r; });
                const auto after = Capture();
                const bool guards = GuardsOK();
                json row = {
                    {"trace", trace}, {"changes", Diff(input, after)}, {"x87", FrameJson(frame)},
                    {"route_ok", routesOk}, {"stack_restored", frame.espBefore == frame.espAfter},
                    {"nonvolatile_preserved", frame.ebxBefore == frame.ebxAfter && frame.esiBefore == frame.esiAfter && frame.ediBefore == frame.ediAfter && frame.ebpBefore == frame.ebpAfter},
                    {"seh_restored", frame.sehBefore == frame.sehAfter},
                    {"fpu_top_balanced", ((EnvWord(frame.envBefore, 4) >> 11) & 7) == ((EnvWord(frame.envAfter, 4) >> 11) & 7) && EnvWord(frame.envAfter, 8) == 0xFFFF},
                    {"fpu_control_same", EnvWord(frame.envBefore, 0) == EnvWord(frame.envAfter, 0)},
                    {"guards_preserved", guards}, {"fixture_contract", guards && after.size() == input.size()}};
                Restore(input); G = cfg0; RestoreFP(fp);
                row["state_restored"] = Capture() == input && CaptureFP() == fp;
                row["floating_state_restored"] = CaptureFP() == fp;
                return row;
            };
            pumpStateCheck = [&] { return Capture() == input; };
            const auto original = invoke(false);
            const auto candidate = invoke(true);
            fakes.reset();
            *ModelSlot(modelIndex) = slotValue;
            std::memcpy(&TheCamera, cameraSaved.data(), sizeof(CCamera)); // la camara real vuelve a su valor antes de escribir el registro
            bool equal = original == candidate;
            for (const char* gate : {"route_ok", "stack_restored", "nonvolatile_preserved", "seh_restored", "fpu_top_balanced", "fpu_control_same", "guards_preserved", "fixture_contract", "state_restored", "floating_state_restored"})
                equal &= original.value(gate, false) && candidate.value(gate, false);
            passed &= equal; failures += !equal;
            pumpStateCheck = [&] { return ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp; };
            Write({{"event", "comparison"}, {"function", full}, {"pair", total}, {"case", n}, {"profile", profile}, {"original", original}, {"candidate", candidate}, {"equal", equal}, {"accepted", equal}});
            ++total;
        }
        Write({{"event", "function_summary"}, {"function", full}, {"cases", fns[f].cases}, {"failures", failures}});
    }
    std::memcpy(&TheCamera, cameraSaved.data(), sizeof(CCamera));
    bool slotsOk = true; for (uint32 m = 0; m < kModelSlots; ++m) slotsOk &= *ModelSlot(m) == slotsBefore[m];
    bool hooks = true; for (int f = 0; f < 2; ++f) hooks &= route[f]->Restore();
    const bool state = slotsOk && ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp;
    RestoreFP(fp);
    Write({{"event", passed && state && hooks ? "complete" : "error"}, {"pairs", total}, {"state_restored", state && hooks}, {"model_slots_restored", slotsOk}, {"floating_state_restored", CaptureFP() == fp}});
    RestoreFP(fp);
}
}
