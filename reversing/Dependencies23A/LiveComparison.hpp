#pragma once
// Banco Dependencies23A: CVehicle::DoVehicleLights (0x6E1A60).
// Cada caso ejecuta la MISMA entrada del EXE con el hook RH apagado (bytes originales) y activo (version de la DLL), con los mismos colaboradores falsos instalados en la direccion del
// EXE y en la entrada de la DLL (EntryPatch): CCoronas::RegisterCorona (tipo, 0x6FC580), FindPlayerVehicle (0x56E0D0), CVehicle::GetVehicleLightsStatus (0x6D55C0),
// CDamageManager::GetLightStatus (0x6C2130), CVehicle::DoHeadLightEffect (0x6E0A50), CVehicle::DoTailLightEffect (0x6E1780), CVehicle::DoHeadLightReflection (0x6E1720) y
// CPointLights::AddLight (0x7000E0). CMatrix * CVector (0x59C890), CVector::NormaliseAndMag (0x59C970), _ftol2 (0x821B40) y el maximo 0x420800 son puros y no se sustituyen: el EXE
// los ejecuta tal cual y la DLL los reproduce en X87Math.cpp (VehLights*, VehHeadTail*). Ver README.md.
// Las dos copias en linea de DoHeadLightEffect(lightId = 1) que trae el EXE no llaman a 0x6E0A50: la DLL las ejecuta con VehicleHeadLightEffect (Vehicle.cpp), que no pasa por la entrada
// parcheada de DoHeadLightEffect, y sus RegisterCorona / FindPlayerVehicle van a los mismos falsos que los del EXE.
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
#include "PointLights.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

namespace Dependencies23ALive {
inline bool done{};
inline std::ofstream log;

// true (valor por defecto, no se relaja): el FNSAVE posterior a cada lado exige igualdad de la palabra de estado completa (TOP, C0..C3, IE/DE/ZE/OE/UE/PE) y del contenido crudo de los ocho
// registros ST, tambien de los que FTW marca vacios. Antes de cada llamada la pila se rellena con un residuo conocido y distinto por registro (ver BuildPrime): lo que cada lado deja o no
// escrito en los registros muertos queda a la vista. FIP, FCS, FDP, FDS y FOP no se comparan (cada lado ejecuta su ultima instruccion x87 en otra direccion).
inline constexpr bool kCompareDeadX87Registers = true;

struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline constexpr Fn fns[]{
    {"CVehicle", "DoVehicleLights", 0x6E1A60, 8192},
};

// Los desplazamientos que usa el EXE (0x6E1A60) y que la reconstruccion lee por nombre: si alguno no coincide el banco ni empieza.
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && sizeof(CMatrix) == 0x48 && sizeof(CMatrixLink) >= 0x48 && offsetof(CPlaceable, m_placement) == 0x04);
static_assert(offsetof(CPlaceable, m_placement) + offsetof(CSimpleTransform, m_fHeading) == 0x10);
static_assert(offsetof(CVehicle, m_nModelIndex) == 0x22 && offsetof(CVehicle, m_pHandlingData) == 0x384 && offsetof(CVehicle, m_pDriver) == 0x460 && offsetof(CVehicle, m_nAlarmState) == 0x45C);
static_assert(offsetof(CVehicle, m_BrakePedal) == 0x4A0 && offsetof(CVehicle, m_nVehicleType) == 0x590 && offsetof(CVehicle, m_nVehicleSubType) == 0x594 && offsetof(CAutomobile, m_damageManager) == 0x5A0);
static_assert(offsetof(CVehicle, m_nRenderLightsFlags) == 0x584 && offsetof(CAutomobile, m_fPropRotate) == 0x958 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44);
static_assert(offsetof(CVehicleModelInfo, m_pVehicleStruct) == 0x5C && offsetof(tHandlingData, m_nHandlingFlags) == 0xD0);
static_assert(offsetof(CCamera, m_nActiveCam) == 0x59 && offsetof(CCamera, m_fLODDistMultiplier) == 0xF0 && offsetof(CCamera, m_aCams) == 0x174 && sizeof(CCam) == 0x238);
static_assert(offsetof(CCam, m_nMode) == 0xC && sizeof(CCamera) == 0xD78);
static_assert(sizeof(CAutomobile) == Dependencies23AFix::kVehicleBytes && sizeof(CMatrixLink) == Dependencies23AFix::kMatrixBytes && sizeof(CVehicleModelInfo) == Dependencies23AFix::kModelInfoBytes);
static_assert(sizeof(CVehicleModelInfo::CVehicleStructure) == Dependencies23AFix::kVStructBytes && sizeof(tHandlingData) == Dependencies23AFix::kHandlingBytes && sizeof(CCamera) == Dependencies23AFix::kCameraBytes);

// ------------------------------------------------------------------ rutas y modulos
inline uintptr_t Resolve(uintptr_t p) { for (int i = 0; i < 16 && *reinterpret_cast<uint8*>(p) == 0xE9; ++i) p += 5 + *reinterpret_cast<int32*>(p + 1); return p; }
inline HMODULE ModuleOf(uintptr_t p) { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(p), &m); return m; }
inline void ModuleMarker() {}

// ------------------------------------------------------------------ estado del coma flotante (como Dependencies21D)
struct Fpu { std::array<uint8, 108> state; uint32 mxcsr; bool operator==(const Fpu&) const = default; };
inline Fpu CaptureFP() { Fpu f{}; auto* p = f.state.data(); auto* m = &f.mxcsr; __asm { mov eax, p } __asm { fnsave [eax] } __asm { frstor [eax] } __asm { mov eax, m } __asm { stmxcsr [eax] } return f; }
inline void RestoreFP(const Fpu& f) { const auto* p = f.state.data(); const auto m = f.mxcsr; __asm { mov eax, p } __asm { frstor [eax] } __asm { ldmxcsr m } }
// Imagen inicial de cada llamada: la x87 del juego con el FSW elegido (TOP y C0..C3 y PE iniciales, como en Dependencies21D/E), los ocho registros "validos" (FTW = 0) con un residuo distinto cada
// uno y la mascara de excepciones del juego. CallTwo la carga con FRSTOR y la vacia con ocho FSTP ST(0): la pila queda vacia, TOP igual y cada registro conserva su residuo.
alignas(16) inline std::array<uint8, 108> primeImage{};
inline constexpr uint16 kFpStatus[4]{0x0000, uint16(0x4700 | (3 << 11)), uint16(0x0100 | 0x0020 | (5 << 11)), uint16(0x0400 | (7 << 11))};
inline void BuildPrime(const Fpu& game, unsigned variant) {
    primeImage = game.state;
    const uint16 status = kFpStatus[variant & 3], tags = 0;
    std::memcpy(primeImage.data() + 4, &status, 2);
    std::memcpy(primeImage.data() + 8, &tags, 2);
    for (unsigned i = 0; i < 8; ++i) {
        uint8* r = primeImage.data() + 28 + 10 * i;
        for (unsigned k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k);
        r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40;   // significando normalizado, exponente 2^1 + i/256: nunca es un valor especial
    }
}
inline std::string HexString(const uint8* data, size_t size) {
    constexpr char digits[] = "0123456789abcdef";
    std::string text; text.reserve(size * 2);
    for (size_t i = 0; i < size; ++i) { text += digits[data[i] >> 4]; text += digits[data[i] & 15]; }
    return text;
}
inline std::function<bool()> pumpStateCheck; inline bool pumpStateOK = true; inline bool writeFPActive{}; inline Fpu writeFP{};
struct PumpObserverGuard { ~PumpObserverGuard() { pumpStateCheck = {}; writeFPActive = false; } };
inline void Write(const json& r) {
    const auto enteringFP = CaptureFP(); const auto& baseline = writeFPActive ? writeFP : enteringFP; RestoreFP(baseline);
    if (pumpStateCheck) pumpStateOK &= pumpStateCheck(); RestoreFP(baseline); ReversingProbe::PumpMessages(); RestoreFP(baseline); if (pumpStateCheck) pumpStateOK &= pumpStateCheck(); RestoreFP(baseline);
    auto record = r; if (record.value("event", "") == "complete" && !pumpStateOK) { record["event"] = "error"; record["state_restored"] = false; }
    if (pumpStateCheck) record["pump_state_intact"] = pumpStateOK; log << record.dump() << '\n'; log.flush(); RestoreFP(baseline);
}

using namespace Dependencies23AFix;
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
inline unsigned headCalls{}, tailCalls{};
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
// thiscall sin argumentos (ret): devuelve AL. El EXE y la DLL lo llaman con ECX = el vehiculo.
inline uint32 __fastcall FLightsStatus(CVehicle* self, void*) {
    trace.push_back({"get_vehicle_lights_status", Ptr(self)});
    return G.lightsStatus;
}
// thiscall con un argumento (ret 4): el indice de luz es un byte (mov cl, [esp+4]) aunque el EXE empuja un dword; devuelve EAX completo (0..3).
inline uint32 __fastcall FGetLightStatus(CDamageManager* self, void*, uint32 light) {
    trace.push_back({"get_light_status", Ptr(self), light & 0xFF});
    return G.lightStatus[light & 3];
}
// DoHeadLightEffect(lightId, matrix, isRight, disabledOrAlarm) y DoTailLightEffect(lightId, matrix, isRight, disabledOrAlarm, unused, staticEmission): thiscall; los bytes son uint8 en las dos
// funciones (los llamadores empujan dwords con basura en los bytes altos); `unused` es un dword (los flags de DoVehicleLights). Devuelven AL = 0/1.
inline uint32 __fastcall FHead(CVehicle* self, void*, int32 lightId, uint32 matrix, uint32 isRight, uint32 disabled) {
    trace.push_back({"head_effect", Ptr(self), lightId, Ptr(reinterpret_cast<void*>(uintptr_t(matrix))), isRight & 0xFF, disabled & 0xFF});
    return G.headRet[(headCalls++) & 1];
}
inline uint32 __fastcall FTail(CVehicle* self, void*, int32 lightId, uint32 matrix, uint32 isRight, uint32 disabled, uint32 unused, uint32 staticEmission) {
    trace.push_back({"tail_effect", Ptr(self), lightId, Ptr(reinterpret_cast<void*>(uintptr_t(matrix))), isRight & 0xFF, disabled & 0xFF, unused, staticEmission & 0xFF});
    return G.tailRet[(tailCalls++) & 7];
}
// DoHeadLightReflection(matrix, flags, left, right): el EXE lee flags y los dos booleanos como bytes (test byte [esp+8], 1; mov al, [esp+0xC]; [esp+0x10]); empuja dwords de pila sin inicializar.
// Se registra el dword completo de flags (lo empuja ebp = los flags del llamador) y los bytes de left / right.
inline void __fastcall FReflection(CVehicle* self, void*, uint32 matrix, uint32 flags, uint32 left, uint32 right) {
    trace.push_back({"head_light_reflection", Ptr(self), Ptr(reinterpret_cast<void*>(uintptr_t(matrix))), flags, left & 0xFF, right & 0xFF});
}
// CPointLights::AddLight (cdecl, 14 dwords con los dos CVector por valor): los tres argumentos pequenos (tipo, niebla, sombras) son bytes en la funcion.
inline void __cdecl FAddLight(uint32 type, uint32 px, uint32 py, uint32 pz, uint32 dx, uint32 dy, uint32 dz, uint32 radius, uint32 r, uint32 g, uint32 b, uint32 fog, uint32 shadows, uint32 entity) {
    trace.push_back({"add_light", type & 0xFF, px, py, pz, dx, dy, dz, radius, r, g, b, fog & 0xFF, shadows & 0xFF, Ptr(reinterpret_cast<void*>(uintptr_t(entity)))});
}

// ------------------------------------------------------------------ llamada con captura de ABI y del FNSAVE (adaptador __asm solo en el banco)
struct Frame {
    uint32 espBefore{}, espAfter{}, ebxBefore{}, ebxAfter{}, esiBefore{}, esiAfter{}, ediBefore{}, ediAfter{}, ebpBefore{}, ebpAfter{}, sehBefore{}, sehAfter{};
    std::array<uint8, 108> imageAfter{};
};
inline unsigned FpWord(const std::array<uint8, 108>& e, int field) { uint16 v; std::memcpy(&v, e.data() + field, 2); return v; }
__declspec(noinline) inline Frame CallTwo(uintptr_t entry, void* self, uint32 a1, uint32 a2) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}; uint8* after = r.imageAfter.data(); const uint8* prime = primeImage.data();
    __asm {
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a2
        push a1
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, after
        fnsave [eax]
        frstor [eax]
    }
    r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}

// ------------------------------------------------------------------ captura / restauracion (arena, camara real y los tres globales del EXE)
inline std::vector<uint8> Capture() {
    std::vector<uint8> v(arena.begin(), arena.end());
    v.insert(v.end(), CamRaw(), CamRaw() + sizeof(CCamera));
    for (uintptr_t a : {kTimeMs, kTimeStep, kForceOff}) v.insert(v.end(), reinterpret_cast<uint8*>(a), reinterpret_cast<uint8*>(a) + 4);
    return v;
}
inline void Restore(const std::vector<uint8>& v) {
    std::memcpy(arena.data(), v.data(), arena.size());
    std::memcpy(&TheCamera, v.data() + arena.size(), sizeof(CCamera));
    size_t at = arena.size() + sizeof(CCamera);
    for (uintptr_t a : {kTimeMs, kTimeStep, kForceOff}) { std::memcpy(reinterpret_cast<void*>(a), v.data() + at, 4); at += 4; }
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
        Both(0x6D55C0, bit_cast<uintptr_t>(&CVehicle::GetVehicleLightsStatus), FLightsStatus);
        Both(0x6C2130, bit_cast<uintptr_t>(&CDamageManager::GetLightStatus), FGetLightStatus);
        Both(0x6E0A50, bit_cast<uintptr_t>(&CVehicle::DoHeadLightEffect), FHead);
        Both(0x6E1780, bit_cast<uintptr_t>(&CVehicle::DoTailLightEffect), FTail);
        Both(0x6E1720, bit_cast<uintptr_t>(&CVehicle::DoHeadLightReflection), FReflection);
        Both(0x7000E0, bit_cast<uintptr_t>(&CPointLights::AddLight), FAddLight);
    }
};

// EAX completo, ECX y EDX no se comparan: DoVehicleLights no devuelve nada y ningun llamador los lee (ver README.md y g23a-audit.txt); si se lee, solo ESP, EBX/ESI/EDI/EBP y FS:[0].
inline json FrameJson(const Frame& f) {
    json x87 = {{"fcw", FpWord(f.imageAfter, 0)}, {"fsw", FpWord(f.imageAfter, 4)}, {"fsw_top", (FpWord(f.imageAfter, 4) >> 11) & 7}, {"ftw", FpWord(f.imageAfter, 8)}};
    if (kCompareDeadX87Registers) x87["raw_registers"] = HexString(f.imageAfter.data() + 28, 80);   // ST0..ST7 crudos (10 bytes cada uno), vacios incluidos
    return x87;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies23a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "dependencies23a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("dependencies23a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;

    camBase = reinterpret_cast<uint8*>(&TheCamera);
    const auto fp = CaptureFP(); writeFP = fp; writeFPActive = true; pumpStateOK = true; PumpObserverGuard pumpGuard;
    const auto cameraSaved = ReversingProbe::Bytes(&TheCamera, sizeof(CCamera));
    OriginalDependencies499::HookRoute route("CVehicle", "DoVehicleLights", 0x6E1A60, FunctionToVoidPtr(&CVehicle::DoVehicleLights));
    const std::vector<uint8> mainOff = ReversingProbe::Bytes(reinterpret_cast<void*>(0x6E1A60), 64);
    auto routeProof = [&](bool candidate) {
        const auto entry = fns[0].address;
        return ReversingProbe::JumpsToDll(entry) == candidate
            && (candidate ? (Resolve(entry) == route.Body() && ModuleOf(route.Body()) == ModuleOf(uintptr_t(&ModuleMarker)))
                          : (Resolve(entry) == entry && ModuleOf(entry) == GetModuleHandleA(nullptr) && mainOff == ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 64)));
    };
    const bool routesReady = route.Registered() && route.Invoke(true, [&] { return routeProof(true); }) && routeProof(false);

    // Contrato de la fixture: los bits que lee el EXE son los de los campos con nombre de la clase.
    bool contract = &CTimer::m_snTimeInMilliseconds == reinterpret_cast<uint32*>(kTimeMs) && &CTimer::ms_fTimeStep == reinterpret_cast<float*>(kTimeStep) && &CVehicle::ms_forceVehicleLightsOff == reinterpret_cast<bool*>(kForceOff);
    {
        std::memset(arena.data(), 0, arena.size());
        auto* v = reinterpret_cast<CAutomobile*>(Veh());
        Put<uint32>(Veh(), Off::flags, 0x10); contract &= v->vehicleFlags.bEngineOn == 1 && v->vehicleFlags.bIsHandbrakeOn == 0 && v->vehicleFlags.bLightsOn == 0;
        Put<uint32>(Veh(), Off::flags, 0x20); contract &= v->vehicleFlags.bEngineOn == 0 && v->vehicleFlags.bIsHandbrakeOn == 1 && v->vehicleFlags.bLightsOn == 0;
        Put<uint32>(Veh(), Off::flags, 0x40); contract &= v->vehicleFlags.bEngineOn == 0 && v->vehicleFlags.bIsHandbrakeOn == 0 && v->vehicleFlags.bLightsOn == 1;
        Put<uint32>(Veh(), Off::flags, 0);
        Put<uint8>(Veh(), Off::overrideByte, uint8(2 << 3)); contract &= v->m_nOverrideLights == 2;
        Put<uint8>(Veh(), Off::overrideByte, uint8(1 << 3 | 5)); contract &= v->m_nOverrideLights == 1;
        Put<uint8>(Veh(), Off::overrideByte, 0);
        Put<uint8>(Veh(), Off::statusByte, uint8(5 << 3 | 3)); contract &= v->GetStatus() == STATUS_WRECKED;
        Put<uint8>(Veh(), Off::statusByte, uint8(4 << 3)); contract &= v->GetStatus() == STATUS_ABANDONED;
        Put<uint8>(Veh(), Off::statusByte, 0);
        Put<uint8>(Veh(), Off::renderLights, 0x0A); contract &= !v->m_renderLights.m_bRightFront && v->m_renderLights.m_bLeftFront && !v->m_renderLights.m_bRightRear && v->m_renderLights.m_bLeftRear;
        Put<uint8>(Veh(), Off::renderLights, 0x05); contract &= v->m_renderLights.m_bRightFront && !v->m_renderLights.m_bLeftFront && v->m_renderLights.m_bRightRear && !v->m_renderLights.m_bLeftRear;
        Put<uint8>(Veh(), Off::renderLights, 0);
        Put<uint32>(Veh(), Off::vehicleType, 0); contract &= v->IsAutomobile();
        Put<uint32>(Veh(), Off::subType, 6); contract &= v->IsSubTrain();
        Put<uint16>(Veh(), Off::alarm, 0xFFFF); Put<uint8>(Veh(), Off::statusByte, uint8(3 << 3)); contract &= v->CanUpdateHornCounter();
        Put<uint16>(Veh(), Off::alarm, 7); contract &= !v->CanUpdateHornCounter();
        Put<uint16>(Veh(), Off::alarm, 0);
    }
    using CoronaFn = void (*)(uint32, CEntity*, uint8, uint8, uint8, uint8, const CVector&, float, float, eCoronaType, eCoronaFlareType, eCoronaReflType, eCoronaLOSCheck, eCoronaTrail, float, bool, float, bool, float, bool, bool);
    const uintptr_t bodies[]{Resolve(std::bit_cast<uintptr_t>(static_cast<CoronaFn>(&CCoronas::RegisterCorona))), Resolve(std::bit_cast<uintptr_t>(&FindPlayerVehicle)),
                             Resolve(std::bit_cast<uintptr_t>(&CVehicle::GetVehicleLightsStatus)), Resolve(std::bit_cast<uintptr_t>(&CDamageManager::GetLightStatus)),
                             Resolve(std::bit_cast<uintptr_t>(&CVehicle::DoHeadLightEffect)), Resolve(std::bit_cast<uintptr_t>(&CVehicle::DoTailLightEffect)),
                             Resolve(std::bit_cast<uintptr_t>(&CVehicle::DoHeadLightReflection)), Resolve(std::bit_cast<uintptr_t>(&CPointLights::AddLight))};
    constexpr uintptr_t exeEntries[]{0x6FC580, 0x56E0D0, 0x6D55C0, 0x6C2130, 0x6E0A50, 0x6E1780, 0x6E1720, 0x7000E0};
    bool bodiesOk = true; json collaborators = json::array();
    for (size_t i = 0; i < std::size(bodies); ++i) { bodiesOk &= ModuleOf(bodies[i]) == ModuleOf(uintptr_t(&ModuleMarker)); collaborators.push_back(json::array({exeEntries[i], bodies[i]})); }
    uint32 slotsBefore[kModelSlots]{}; for (uint32 m = 0; m < kModelSlots; ++m) slotsBefore[m] = *ModelSlot(m);
    const auto input0 = Capture();
    const bool ready = routesReady && contract && bodiesOk && input0.size() == arena.size() + sizeof(CCamera) + 12;
    pumpStateCheck = [&] { return ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp; };
    RestoreFP(fp);
    json routesJson = json::array({{{"function", fns[0].name}, {"registered", route.Registered()}, {"body", route.Body()}, {"module_ok", ModuleOf(route.Body()) == ModuleOf(uintptr_t(&ModuleMarker))}}});
    Write({{"event", "begin"}, {"registered_hook", routesReady}, {"ready", ready}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"target_routes_ready", routesReady}, {"fixture_contract", contract},
           {"collaborator_bodies_ok", bodiesOk}, {"routes", routesJson}, {"collaborators", collaborators}, {"probe_module", uintptr_t(ModuleOf(uintptr_t(&ModuleMarker)))}, {"arena_bytes", arena.size()},
           {"camera_bytes", sizeof(CCamera)}, {"vehicle_bytes", sizeof(CAutomobile)}, {"scope", "vehiculo, matrices, modelo, dummies y manejo en arena propia; camara real guardada y restaurada entera; "
           "ms_modelInfoPtrs desviado por caso y restaurado; tiempo, paso de tiempo y ms_forceVehicleLightsOff guardados y restaurados; RegisterCorona, FindPlayerVehicle, GetVehicleLightsStatus, GetLightStatus, "
           "DoHeadLightEffect, DoTailLightEffect, DoHeadLightReflection y AddLight falsos en EXE y DLL; 0x59C890, 0x59C970, _ftol2 y 0x420800 reales"}});
    if (!ready) { Write({{"event", "error"}, {"reason", "routes/fixture preflight failed"}, {"pairs", 0}, {"state_restored", true}}); return; }

    bool passed = true; unsigned total = 0;
    const std::string full = std::string(fns[0].cls) + "::" + fns[0].name;
    unsigned failures = 0;
    for (int n = 0; n < fns[0].cases; ++n) {
        fixRng = Rng(uint64(1) * 1000003ull + uint64(n) * 7919ull + 17ull);
        const json profile = SetupCase();
        const auto input = Capture();
        const Cfg cfg0 = G;
        const uint32 slotValue = *ModelSlot(modelIndex);
        *ModelSlot(modelIndex) = uint32(uintptr_t(ModelInfo()));
        std::unique_ptr<Fakes> fakes = std::make_unique<Fakes>();
        auto invoke = [&](bool candidate) -> json {
            Restore(input); G = cfg0; trace = json::array(); headCalls = tailCalls = 0; RestoreFP(fp);
            BuildPrime(fp, unsigned(n));
            bool routesOk = false;
            const auto frame = route.Invoke(candidate, [&] {
                routesOk = routeProof(candidate); RestoreFP(fp);
                void* matrix = (args.matrixIsVehicle && Get<void*>(Veh(), Off::matrix)) ? static_cast<void*>(MatVeh()) : static_cast<void*>(MatArg());
                Frame r = CallTwo(fns[0].address, Veh(), uint32(uintptr_t(matrix)), args.flags);
                routesOk &= routeProof(candidate); return r; });
            const auto after = Capture();
            const bool guards = GuardsOK();
            json row = {
                {"trace", trace}, {"changes", Diff(input, after)}, {"x87", FrameJson(frame)},
                {"route_ok", routesOk}, {"stack_restored", frame.espBefore == frame.espAfter},
                {"nonvolatile_preserved", frame.ebxBefore == frame.ebxAfter && frame.esiBefore == frame.esiAfter && frame.ediBefore == frame.ediAfter && frame.ebpBefore == frame.ebpAfter},
                {"seh_restored", frame.sehBefore == frame.sehAfter},
                {"fpu_top_balanced", ((FpWord(primeImage, 4) >> 11) & 7) == ((FpWord(frame.imageAfter, 4) >> 11) & 7) && FpWord(frame.imageAfter, 8) == 0xFFFF},
                {"fpu_control_same", FpWord(primeImage, 0) == FpWord(frame.imageAfter, 0)},
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
    Write({{"event", "function_summary"}, {"function", full}, {"cases", fns[0].cases}, {"failures", failures}});
    std::memcpy(&TheCamera, cameraSaved.data(), sizeof(CCamera));
    bool slotsOk = true; for (uint32 m = 0; m < kModelSlots; ++m) slotsOk &= *ModelSlot(m) == slotsBefore[m];
    const bool hooks = route.Restore();
    const bool state = slotsOk && ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp;
    RestoreFP(fp);
    Write({{"event", passed && state && hooks ? "complete" : "error"}, {"pairs", total}, {"state_restored", state && hooks}, {"model_slots_restored", slotsOk}, {"floating_state_restored", CaptureFP() == fp}});
    RestoreFP(fp);
}
}
