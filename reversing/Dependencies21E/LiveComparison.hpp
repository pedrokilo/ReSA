#pragma once
// Banco Dependencies21E: 0x610DB0 (CPopulation::RandomIntInLimitRange), 0x72D6D0 (CZoneDef::GetCenter) y 0x63A380 (CTaskComplexEnterCar::GetCameraStickModifier).
// Cada par llama a la MISMA entrada del EXE con el hook RH apagado (original) y activo (candidato: la DLL) con el mismo fixture, el mismo estado x87 inicial y los mismos
// colaboradores falsos, y compara en bits: valor de retorno, memoria tocada (con bordes de proteccion), ESP, registros no volatiles, cadena SEH (FS:0) y estado x87
// (FCW, FSW completa con TOP, C0..C3 y banderas pegajosas, FTW y los registros ST con datos). Ver README.md.
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <vector>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Population.h"
#include "CullZones.h"
#include "General.h"
#include "Pad.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "Collision/ColModel.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "CutsceneMgr.h"
#include "World.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

namespace Dependencies21ELive {
namespace fx = Dependencies21EFixtures;

inline bool done{};
inline std::ofstream log;
inline constexpr unsigned kCases1 = fx::kCases1, kCases2 = fx::kCases2, kCases3 = fx::kCases3;
struct Fn { const char* cls; const char* name; uintptr_t address; int reserved, unused, count; };
inline constexpr Fn fns[]{
    {"CPopulation", "RandomIntInLimitRange", 0x610DB0, 0, -1, int(kCases1)},
    {"CZoneDef", "GetCenter", 0x72D6D0, 0, -1, int(kCases2)},
    {"CTaskComplexEnterCar", "GetCameraStickModifier", 0x63A380, 0, -1, int(kCases3)},
};
// true (valor por defecto, no se relaja): ademas de FCW, FSW completa (TOP, C0..C3, IE/DE/ZE/OE/UE/PE), FTW y los registros con datos, se exige igualdad del contenido crudo de los ocho
// registros ST, tambien de los que FTW marca vacios. Cada llamada empieza con un residuo conocido y distinto en cada registro (BaseFp + los ocho FSTP de RawCall), asi que se ve tanto
// lo que un lado escribe y el otro no como lo que escribe con otro valor. FIP, FCS, FDP, FDS y FOP no se comparan (cada lado ejecuta su ultima instruccion x87 en otra direccion):
// van en fp_difference como informacion.
inline constexpr bool kCompareDeadX87Registers = true;

inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {
    MEMORY_BASIC_INFORMATION info{};
    return VirtualQuery(reinterpret_cast<void*>(address), &info, sizeof(info)) == sizeof(info) ? uintptr_t(info.AllocationBase) : 0;
}
inline uintptr_t Resolve(uintptr_t address) {
    for (unsigned depth = 0; depth < 16 && *reinterpret_cast<uint8*>(address) == 0xE9; ++depth) address += 5 + *reinterpret_cast<int32*>(address + 1);
    return address;
}
inline std::string Hex(const void* data, size_t size) {
    constexpr char digits[] = "0123456789abcdef";
    const auto* bytes = static_cast<const uint8*>(data);
    std::string text;
    text.reserve(size * 2);
    for (size_t i = 0; i < size; ++i) { text += digits[bytes[i] >> 4]; text += digits[bytes[i] & 15]; }
    return text;
}
inline uint16 W16(const uint8* p) { uint16 v; std::memcpy(&v, p, 2); return v; }
inline uint32 W32(const uint8* p) { uint32 v; std::memcpy(&v, p, 4); return v; }
inline void Put32(void* p, uint32 value) { std::memcpy(p, &value, 4); }
inline void Put16(void* p, uint16 value) { std::memcpy(p, &value, 2); }

// ---------------- estado de coma flotante del juego ----------------
using FpImage = std::array<uint8, 108>;
inline void SaveFp(void* image) { __asm { mov eax, image } __asm { fnsave [eax] } __asm { frstor [eax] } }
inline void LoadFp(const void* image) { __asm { mov eax, image } __asm { frstor [eax] } }
inline uint32 ReadMxcsr() { uint32 value{}; __asm { stmxcsr value } return value; }
inline void SetMxcsr(uint32 value) { __asm { ldmxcsr value } }
alignas(16) inline FpImage gameFp{};
inline uint32 gameMxcsr{};
inline bool gameFpActive{};
inline void RestoreFloatingState() { if (gameFpActive) { LoadFp(gameFp.data()); SetMxcsr(gameMxcsr); } }
inline bool FloatingStateRestored() {
    alignas(16) FpImage now{};
    SaveFp(now.data());
    return now == gameFp && ReadMxcsr() == gameMxcsr;
}
// Estado inicial de cada caso: el del juego con la pila vacia y FSW = el del fixture (TOP, C0..C3 y banderas pegajosas).
// Los ocho registros salen "validos" (FTW = 0) con un residuo distinto cada uno; RawCall los vacia con ocho FSTP ST(0) justo despues del FRSTOR: pila vacia, TOP igual y cada registro con su residuo.
inline FpImage BaseFp(unsigned status) {
    FpImage image = gameFp;
    Put16(image.data() + 4, fx::kFpStatus[status]);
    Put16(image.data() + 8, 0x0000);
    for (unsigned i = 0; i < 8; ++i) {
        uint8* r = image.data() + 28 + 10 * i;
        for (unsigned k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k);
        r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40;   // significando normalizado: nunca es un valor especial
    }
    return image;
}
inline void Write(const json& row) {
    RestoreFloatingState();
    ReversingProbe::PumpMessages();
    log << row.dump() << '\n';
    log.flush();
    RestoreFloatingState();
}

// ---------------- llamada cruda con captura completa ----------------
// Variables globales (no locales): el ensamblador toca ESP y los registros no volatiles (EBP se conserva tal cual: modificarlo da el aviso C4731, un error con /WX). Los valores 0A5123456h/0C0DE5921h/0B1234567h de entrada permiten ver que nadie lee ECX/EDX/EAX de entrada.
inline uint32 gEbp0{}, gFn{}, gEcx{}, gEdx{}, gEax{}, gCount{}, gSavedEsp{}, gEsp0{}, gEsp1{}, gFs0{}, gFs1{};
inline uint32 gArgs[8]{};
inline uint32 gOutEax{}, gOutEcx{}, gOutEdx{}, gOutEbx{}, gOutEsi{}, gOutEdi{}, gOutEbp{};
alignas(16) inline uint8 gBaseFp[108]{};
alignas(16) inline uint8 gOutFp[108]{};
__declspec(noinline) inline void RawCall() {
    __asm {
        push ebx
        push esi
        push edi
        mov gSavedEsp, esp
        mov eax, fs:[0]
        mov gFs0, eax
        mov ebx, 0B0B0B0Bh
        mov esi, 51515151h
        mov edi, 0D1D1D1D1h
        mov gEbp0, ebp
        frstor gBaseFp
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov gEsp0, esp
        mov ecx, gCount
    pushing:
        test ecx, ecx
        jz going
        push dword ptr gArgs[ecx * 4 - 4]
        dec ecx
        jmp pushing
    going:
        mov ecx, gEcx
        mov edx, gEdx
        mov eax, gEax
        call gFn
        mov gOutEax, eax
        mov gOutEcx, ecx
        mov gOutEdx, edx
        mov gEsp1, esp
        mov gOutEbx, ebx
        mov gOutEsi, esi
        mov gOutEdi, edi
        mov gOutEbp, ebp
        mov eax, fs:[0]
        mov gFs1, eax
        fnsave gOutFp
        mov esp, gSavedEsp
        pop edi
        pop esi
        pop ebx
    }
}
struct CallResult {
    uint32 eax, ecx, edx;
    bool stack, nonvolatile, seh;
    FpImage fp;
};
inline CallResult Call(uintptr_t entry, uint32 ecx, uint32 edx, uint32 eax, std::initializer_list<uint32> args, const FpImage& base, bool calleeCleans) {
    gFn = uint32(entry); gEcx = ecx; gEdx = edx; gEax = eax; gCount = uint32(args.size());
    unsigned n = 0;
    for (const uint32 a : args) gArgs[n++] = a;
    std::memcpy(gBaseFp, base.data(), 108);
    RawCall();
    CallResult r{};
    r.eax = gOutEax; r.ecx = gOutEcx; r.edx = gOutEdx;
    r.stack = calleeCleans ? gEsp1 == gEsp0 : gEsp1 == gEsp0 - 4 * gCount;
    r.nonvolatile = gOutEbx == 0x0B0B0B0Bu && gOutEsi == 0x51515151u && gOutEdi == 0xD1D1D1D1u && gOutEbp == gEbp0;
    r.seh = gFs0 == gFs1;
    std::memcpy(r.fp.data(), gOutFp, 108);
    RestoreFloatingState();
    return r;
}
// FNSAVE: +0 FCW, +4 FSW, +8 FTW (2 bits por registro fisico, 3 = vacio), +12 FIP, +16 FCS|FOP, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes, orden de pila).
inline json FpState(const FpImage& f) {
    const uint16 fsw = W16(f.data() + 4), ftw = W16(f.data() + 8);
    const unsigned top = (fsw >> 11) & 7;
    json valid = json::array();
    for (unsigned st = 0; st < 8; ++st)
        if (((ftw >> (2 * ((top + st) & 7))) & 3) != 3) valid.push_back({{"st", st}, {"raw", Hex(f.data() + 28 + 10 * st, 10)}});
    json state = {{"fcw", W16(f.data())}, {"fsw", fsw}, {"ftw", ftw}, {"top", top}, {"valid_registers", valid}};
    if (kCompareDeadX87Registers) state["raw_registers"] = Hex(f.data() + 28, 80);
    return state;
}
inline bool DeadRegistersEqual(const FpImage& a, const FpImage& b) {
    const uint16 ftw = W16(a.data() + 8), fsw = W16(a.data() + 4);
    const unsigned top = (fsw >> 11) & 7;
    for (unsigned st = 0; st < 8; ++st)
        if (((ftw >> (2 * ((top + st) & 7))) & 3) == 3 && std::memcmp(a.data() + 28 + 10 * st, b.data() + 28 + 10 * st, 10) != 0) return false;
    return true;
}
inline json FpDifference(const FpImage& a, const FpImage& b) {
    json out = json::array();
    const struct { const char* name; size_t at, size; } fields[]{{"fcw", 0, 2}, {"fsw", 4, 2}, {"ftw", 8, 2}, {"fip", 12, 4}, {"fcs_fop", 16, 4}, {"fdp", 20, 4}, {"fds", 24, 4}};
    for (const auto& field : fields) if (std::memcmp(a.data() + field.at, b.data() + field.at, field.size) != 0) out.push_back(field.name);
    if (!DeadRegistersEqual(a, b)) out.push_back("dead_registers");
    return out;
}

// ---------------- bloques de memoria con bordes ----------------
template<size_t N>
struct Arena {
    alignas(16) std::array<uint8, N + 32> bytes{};
    uint8* Data() { return bytes.data() + 16; }
    const uint8* Data() const { return bytes.data() + 16; }
    void Fill(uint8 guard, uint8 body) { std::memset(bytes.data(), guard, bytes.size()); std::memset(Data(), body, N); }
    bool GuardsIntact(uint8 guard) const {
        for (size_t i = 0; i < 16; ++i) if (bytes[i] != guard || bytes[N + 16 + i] != guard) return false;
        return true;
    }
    uintptr_t Address() const { return uintptr_t(Data()); }
};
inline uint64_t Fnv1a(const std::string& text) {
    uint64_t h = 1469598103934665603ull;
    for (const unsigned char c : text) { h ^= c; h *= 1099511628211ull; }
    return h;
}
struct Block {
    uintptr_t address;
    std::vector<uint8> saved;
    Block(uintptr_t a, size_t n) : address(a), saved(n) { std::memcpy(saved.data(), reinterpret_cast<void*>(a), n); }
    void Restore() const {
        DWORD old{}, ignored{};
        VirtualProtect(reinterpret_cast<void*>(address), saved.size(), PAGE_EXECUTE_READWRITE, &old);
        std::memcpy(reinterpret_cast<void*>(address), saved.data(), saved.size());
        VirtualProtect(reinterpret_cast<void*>(address), saved.size(), old, &ignored);
    }
    bool Restored() const { return std::memcmp(reinterpret_cast<void*>(address), saved.data(), saved.size()) == 0; }
};
inline void Unprotect(uintptr_t address, size_t size) { DWORD old{}; VirtualProtect(reinterpret_cast<void*>(address), size, PAGE_EXECUTE_READWRITE, &old); }

// ---------------- colaboradores falsos ----------------
inline std::vector<std::array<uint32, 3>> trace;     // {tipo, objeto, argumento} en orden de llamada
inline bool trapHit{};
enum TraceKind : uint32 { kTraceTaskType = 1, kTraceAimLeftRight, kTraceAimUpDown, kTraceAccelerate, kTraceBrake, kTraceColModel, kTraceRandom };
inline int16 scriptAimLeftRight{}, scriptAimUpDown{}, scriptAccelerate{}, scriptBrake{};
inline int32 scriptSubTaskType{};
inline uintptr_t scriptColModel{};
inline uint32 scriptRandom{};
inline unsigned randomCalls{};
inline void* __fastcall FakeGetSubTask(void* self, void*) { return *reinterpret_cast<void**>(static_cast<uint8*>(self) + 8); }
inline int32 __fastcall FakeGetTaskType(void* self, void*) { trace.push_back({kTraceTaskType, uint32(uintptr_t(self)), 0}); return scriptSubTaskType; }
inline int32 __fastcall FakeTrap(void*, void*) { trapHit = true; return 0; }
inline int16 __fastcall FakeAimLeftRight(void* pad, void*, void* ped) { trace.push_back({kTraceAimLeftRight, uint32(uintptr_t(pad)), uint32(uintptr_t(ped))}); return scriptAimLeftRight; }
inline int16 __fastcall FakeAimUpDown(void* pad, void*, void* ped) { trace.push_back({kTraceAimUpDown, uint32(uintptr_t(pad)), uint32(uintptr_t(ped))}); return scriptAimUpDown; }
inline int16 __fastcall FakeAccelerate(void* pad, void*) { trace.push_back({kTraceAccelerate, uint32(uintptr_t(pad)), 0}); return scriptAccelerate; }
inline int16 __fastcall FakeBrake(void* pad, void*) { trace.push_back({kTraceBrake, uint32(uintptr_t(pad)), 0}); return scriptBrake; }
inline void* __fastcall FakeColModel(void* car, void*) { trace.push_back({kTraceColModel, uint32(uintptr_t(car)), 0}); return reinterpret_cast<void*>(scriptColModel); }
inline int32 __cdecl FakeExeRand() { ++randomCalls; return int32(scriptRandom); }
inline uint16 __cdecl FakeDllRand() { ++randomCalls; return uint16(scriptRandom); }

// Parche doble de un colaborador: direccion del EXE (que llama el original) y entrada de la DLL (que llama el candidato; Resolve salta los trampolines del enlace incremental).
// Ambos saltan a la misma funcion falsa, asi que las dos rutas ven la misma traza.
class Collaborator {
    uintptr_t exe_, dll_;
    std::vector<uint8> exeBefore_, dllBefore_;
    std::unique_ptr<ReversingProbe::EntryPatch> exePatch_, dllPatch_;
public:
    template<class F>
    Collaborator(uintptr_t exe, uintptr_t dllEntry, F fake)
        : exe_(exe), dll_(Resolve(dllEntry)), exeBefore_(ReversingProbe::Bytes(reinterpret_cast<void*>(exe), 16)), dllBefore_(ReversingProbe::Bytes(reinterpret_cast<void*>(dll_), 16)) {
        exePatch_ = std::make_unique<ReversingProbe::EntryPatch>(exe_, fake);
        dllPatch_ = std::make_unique<ReversingProbe::EntryPatch>(dll_, fake);
    }
    // Ambas entradas saltan al mismo destino y la de la DLL es de este modulo (no un colaborador del EXE sin reconstruir).
    bool Active() const {
        return *reinterpret_cast<uint8*>(exe_) == 0xE9 && *reinterpret_cast<uint8*>(dll_) == 0xE9 && Resolve(exe_) == Resolve(dll_) && ModuleOf(dll_) == ModuleOf(uintptr_t(&ModuleMarker));
    }
    void Release() { exePatch_.reset(); dllPatch_.reset(); }
    bool BytesRestored() const {
        return exeBefore_ == ReversingProbe::Bytes(reinterpret_cast<void*>(exe_), 16) && dllBefore_ == ReversingProbe::Bytes(reinterpret_cast<void*>(dll_), 16);
    }
    json Metadata() const { return {{"exe", exe_}, {"dll", dll_}, {"exe_bytes", Hex(exeBefore_.data(), 16)}, {"dll_bytes", Hex(dllBefore_.data(), 16)}, {"active", Active()}}; }
};

// Registro compacto: las cadenas de mas de 512 caracteres (volcados de memoria) se reducen a longitud + FNV-1a de 64 bits cuando el par es igual; la igualdad se decide siempre
// sobre los bytes completos.
inline json Slim(json value) {
    if (value.is_string() && value.get_ref<const std::string&>().size() > 512) {
        const std::string text = value.get<std::string>();
        return json{{"bytes", text.size() / 2}, {"fnv1a64", Fnv1a(text)}};
    }
    if (value.is_object() || value.is_array()) for (auto& item : value) item = Slim(item);
    return value;
}

struct Observed {
    json gated;     // lo que se compara entre original y candidato (y debe cumplir las compuertas)
    json info;      // informacion por lado: no se compara (residuo de ECX/EDX que ningun llamador lee, registros x87 vacios)
    FpImage fp{};
};
inline constexpr const char* kGates[] = {"route_ok", "collaborators_ok", "stack_restored", "nonvolatile_preserved", "seh_restored", "fixture_contract", "memory_restored", "floating_state_restored"};

inline void Update() {
    if (done) return;
    char exePath[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    const auto dir = std::filesystem::path(exePath).parent_path();
    if (!std::filesystem::exists(dir / "dependencies21e-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "dependencies21e-runtime.jsonl");
        Write({{"event", "waiting_for_player"}});
        ReversingProbe::InstallFailureCapture("dependencies21e");
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    static_assert(sizeof(CVehicle) == 0x5A0 && sizeof(CPed) == 0x79C && sizeof(CColModel) == 0x30 && sizeof(CTaskComplexEnterCar) == 0x50);

    SaveFp(gameFp.data());
    gameMxcsr = ReadMxcsr();
    gameFpActive = true;
    constexpr uintptr_t kPads = 0xB73458, kPadSize = 0x134, kUpperLimit = 0x86C6C8, kLowerLimit = 0x86C6C4, kCameraLimit = 0x8D2ECC;
    std::vector<Block> globals;
    globals.emplace_back(kLowerLimit, 8);
    globals.emplace_back(kCameraLimit, 4);
    globals.emplace_back(kPads, 2 * kPadSize);
    DWORD rdataProtection{};
    {
        MEMORY_BASIC_INFORMATION info{};
        VirtualQuery(reinterpret_cast<void*>(kLowerLimit), &info, sizeof(info));
        rdataProtection = info.Protect;
        Unprotect(kLowerLimit, 8);
    }
    auto restoreGlobals = [&] { for (const auto& b : globals) b.Restore(); RestoreFloatingState(); };
    auto globalsRestored = [&] { return std::ranges::all_of(globals, [](const auto& b) { return b.Restored(); }) && FloatingStateRestored(); };

    // Pruebas de ruta de los tres destinos (como DependenciesTaskSprint16): original = la entrada del EXE intacta; candidato = salto hacia la DLL y cuerpo de este modulo.
    OriginalDependencies499::HookRoute route1("CPopulation", "RandomIntInLimitRange", 0x610DB0, &CPopulation::RandomIntInLimitRange);
    OriginalDependencies499::HookRoute route2("CZoneDef", "GetCenter", 0x72D6D0, &CZoneDef::GetCenter);
    OriginalDependencies499::HookRoute route3("CTaskComplexEnterCar", "GetCameraStickModifier", 0x63A380, &CTaskComplexEnterCar::GetCameraStickModifier);
    const auto off1 = ReversingProbe::Bytes(reinterpret_cast<void*>(0x610DB0), 64);
    const auto off2 = ReversingProbe::Bytes(reinterpret_cast<void*>(0x72D6D0), 64);
    const auto off3 = ReversingProbe::Bytes(reinterpret_cast<void*>(0x63A380), 64);
    auto routeProof = [&](uintptr_t entry, const OriginalDependencies499::HookRoute& route, const std::vector<uint8>& original, bool candidate) {
        if (ReversingProbe::JumpsToDll(entry) != candidate) return false;
        if (candidate) return Resolve(entry) == route.Body() && ModuleOf(route.Body()) == ModuleOf(uintptr_t(&ModuleMarker));
        return Resolve(entry) == entry && ModuleOf(entry) == uintptr_t(GetModuleHandleA(nullptr)) && original == ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 64);
    };
    const bool routesReady = route1.Registered() && route2.Registered() && route3.Registered()
        && route1.Invoke(true, [&] { return routeProof(0x610DB0, route1, off1, true); }) && routeProof(0x610DB0, route1, off1, false)
        && route2.Invoke(true, [&] { return routeProof(0x72D6D0, route2, off2, true); }) && routeProof(0x72D6D0, route2, off2, false)
        && route3.Invoke(true, [&] { return routeProof(0x63A380, route3, off3, true); }) && routeProof(0x63A380, route3, off3, false);

    // Colaboradores falsos: RNG (rand del EXE en 0x821B1E, devuelve int; CGeneral::GetRandomNumber de la DLL, devuelve uint16), las cinco funciones de pad / colision (parche doble en la
    // direccion del EXE y en la entrada de la DLL) y las tablas virtuales falsas de las tareas.
    const auto exeRandBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(0x821B1E), 16);
    const uintptr_t dllRandEntry = Resolve(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber));
    const auto dllRandBefore = ReversingProbe::Bytes(reinterpret_cast<void*>(dllRandEntry), 16);
    auto exeRand = std::make_unique<ReversingProbe::EntryPatch>(0x821B1E, &FakeExeRand);
    auto dllRand = std::make_unique<ReversingProbe::EntryPatch>(dllRandEntry, &FakeDllRand);
    Collaborator aimLeftRight(0x541040, std::bit_cast<uintptr_t>(&CPad::AimWeaponLeftRight), &FakeAimLeftRight);
    Collaborator aimUpDown(0x5410C0, std::bit_cast<uintptr_t>(&CPad::AimWeaponUpDown), &FakeAimUpDown);
    Collaborator accelerate(0x5403F0, std::bit_cast<uintptr_t>(&CPad::GetAccelerate), &FakeAccelerate);
    Collaborator brake(0x540080, std::bit_cast<uintptr_t>(&CPad::GetBrake), &FakeBrake);
    Collaborator colModel(0x535300, std::bit_cast<uintptr_t>(&CEntity::GetColModel), &FakeColModel);
    auto padCollaboratorsOk = [&] { return aimLeftRight.Active() && aimUpDown.Active() && accelerate.Active() && brake.Active() && colModel.Active(); };
    auto randomCollaboratorOk = [&] {
        return *reinterpret_cast<uint8*>(0x821B1E) == 0xE9 && *reinterpret_cast<uint8*>(dllRandEntry) == 0xE9 && ModuleOf(dllRandEntry) == ModuleOf(uintptr_t(&ModuleMarker)) && ModuleOf(Resolve(0x821B1E)) == ModuleOf(uintptr_t(&ModuleMarker));
    };

    // Tablas virtuales falsas: CTask::GetSubTask (ranura 2) devuelve [tarea + 8]; GetTaskType (ranura 4) de la subtarea es el valor del fixture; cualquier otra ranura activa la trampa.
    static void* vtTask[16];
    static void* vtSub[16];
    for (int i = 0; i < 16; ++i) { vtTask[i] = reinterpret_cast<void*>(&FakeTrap); vtSub[i] = reinterpret_cast<void*>(&FakeTrap); }
    vtTask[2] = reinterpret_cast<void*>(&FakeGetSubTask);
    vtSub[4] = reinterpret_cast<void*>(&FakeGetTaskType);

    json collaborators = {{"random_exe", 0x821B1E}, {"random_dll", dllRandEntry}, {"random_dll_bytes", Hex(dllRandBefore.data(), 16)}};
    collaborators["aim_left_right"] = aimLeftRight.Metadata(); collaborators["aim_up_down"] = aimUpDown.Metadata(); collaborators["accelerate"] = accelerate.Metadata();
    collaborators["brake"] = brake.Metadata(); collaborators["col_model"] = colModel.Metadata();
    const bool ready = routesReady && padCollaboratorsOk() && randomCollaboratorOk();
    Write({{"event", "begin"}, {"registered_hook", route1.Registered() && route2.Registered() && route3.Registered()}, {"ready", ready}, {"routes_ready", routesReady}, {"collaborators_stubbed", true},
           {"collaborators", collaborators}, {"functions", 3}, {"pairs", kCases1 + kCases2 + kCases3}, {"cases", json{{"CPopulation::RandomIntInLimitRange", kCases1}, {"CZoneDef::GetCenter", kCases2}, {"CTaskComplexEnterCar::GetCameraStickModifier", kCases3}}},
           {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"initial_fp", Hex(gameFp.data(), 108)}, {"initial_mxcsr", gameMxcsr}, {"rdata_protection", rdataProtection},
           {"bodies", json{{"RandomIntInLimitRange", route1.Body()}, {"GetCenter", route2.Body()}, {"GetCameraStickModifier", route3.Body()}}}, {"asi_module", ModuleOf(uintptr_t(&ModuleMarker))},
           {"original_module", uintptr_t(GetModuleHandleA(nullptr))}, {"scope", "tres destinos puros; colaboradores falsos identicos en las dos rutas; fixtures de dominio legal (eleccion 0..3, coche con matriz)"}});

    unsigned pairs = 0, failures = 0;
    bool passed = ready;
    std::array<unsigned, 3> perFunction{}, perFunctionFailures{};
    auto finish = [&](unsigned function, unsigned caseIndex, const Observed& a, const Observed& b) {
        bool ok = a.gated == b.gated;
        for (const char* gate : kGates) ok &= a.gated.value(gate, false) == true && b.gated.value(gate, false) == true;
        json fpDifference = FpDifference(a.fp, b.fp);
        passed &= ok;
        failures += !ok;
        perFunction[function]++;
        perFunctionFailures[function] += !ok;
        Write({{"event", "comparison"}, {"function", std::string(fns[function].cls) + "::" + fns[function].name}, {"case", caseIndex}, {"pair", pairs++}, {"original", ok ? Slim(a.gated) : a.gated},
               {"candidate", ok ? Slim(b.gated) : b.gated}, {"info_original", a.info}, {"info_candidate", b.info}, {"fp_difference", fpDifference}, {"equal", ok}, {"accepted", ok}});
    };
    auto summarize = [&](unsigned function) {
        Write({{"event", "function_summary"}, {"function", std::string(fns[function].cls) + "::" + fns[function].name}, {"cases", perFunction[function]}, {"failures", perFunctionFailures[function]}});
    };

    // ======================= 0x610DB0 =======================
    for (unsigned i = 0; i < kCases1 && ready; ++i) {
        const fx::Fx1 f = fx::MakeFx1(i);
        const FpImage base = BaseFp(f.fp);
        const json input = {{"lower", f.lower}, {"upper", f.upper}, {"random", f.random}, {"fp", f.fp}};
        auto invoke = [&](bool candidate) {
            restoreGlobals();
            Put32(reinterpret_cast<void*>(kLowerLimit), uint32(f.lower));
            Put32(reinterpret_cast<void*>(kUpperLimit), uint32(f.upper));
            scriptRandom = f.random; randomCalls = 0; trapHit = false;
            bool routes = false;
            CallResult r{};
            route1.Invoke(candidate, [&] {
                routes = routeProof(0x610DB0, route1, off1, candidate);
                r = Call(0x610DB0, 0xB1234567u, 0xC0DE5921u, 0xA5123456u, {}, base, false);
                routes &= routeProof(0x610DB0, route1, off1, candidate);
                return 0;
            });
            const bool unchanged = W32(reinterpret_cast<uint8*>(kLowerLimit)) == uint32(f.lower) && W32(reinterpret_cast<uint8*>(kUpperLimit)) == uint32(f.upper);
            Observed o;
            o.gated = {{"input", input}, {"route_ok", routes}, {"collaborators_ok", randomCollaboratorOk() && !trapHit}, {"eax", r.eax}, {"rand_calls", randomCalls},
                       {"stack_restored", r.stack}, {"nonvolatile_preserved", r.nonvolatile}, {"seh_restored", r.seh}, {"fp", FpState(r.fp)}, {"fixture_contract", unchanged}};
            o.info = {{"ecx", r.ecx}, {"edx", r.edx}};
            o.fp = r.fp;
            restoreGlobals();
            o.gated["memory_restored"] = globalsRestored();
            o.gated["floating_state_restored"] = FloatingStateRestored();
            return o;
        };
        const Observed a = invoke(false), b = invoke(true);
        finish(0, i, a, b);
    }
    summarize(0);

    // ======================= 0x72D6D0 =======================
    for (unsigned i = 0; i < kCases2 && ready; ++i) {
        const fx::Fx2 f = fx::MakeFx2(i);
        const FpImage base = BaseFp(f.fp);
        json zone = json::array();
        for (int k = 0; k < 8; ++k) zone.push_back(f.zone[k]);
        const json input = {{"zone", zone}, {"fp", f.fp}};
        auto invoke = [&](bool candidate) {
            restoreGlobals();
            Arena<16> zoneBlock;
            Arena<12> outBlock;
            zoneBlock.Fill(0xA5, 0);
            outBlock.Fill(0xA6, 0);
            for (int k = 0; k < 8; ++k) Put16(zoneBlock.Data() + 2 * k, uint16(f.zone[k]));
            Put32(outBlock.Data(), 0x7FC0DEADu); Put32(outBlock.Data() + 4, 0xFFC0BEEFu); Put32(outBlock.Data() + 8, 0x7F800001u);
            const auto zoneInput = std::vector<uint8>(zoneBlock.Data(), zoneBlock.Data() + 16);
            bool routes = false;
            CallResult r{};
            route2.Invoke(candidate, [&] {
                routes = routeProof(0x72D6D0, route2, off2, candidate);
                r = Call(0x72D6D0, uint32(zoneBlock.Address()), 0xC0DE5921u, 0xA5123456u, {uint32(outBlock.Address())}, base, true);
                routes &= routeProof(0x72D6D0, route2, off2, candidate);
                return 0;
            });
            const bool contract = std::vector<uint8>(zoneBlock.Data(), zoneBlock.Data() + 16) == zoneInput && zoneBlock.GuardsIntact(0xA5) && outBlock.GuardsIntact(0xA6);
            Observed o;
            o.gated = {{"input", input}, {"route_ok", routes}, {"collaborators_ok", true}, {"eax_is_out", r.eax == uint32(outBlock.Address())}, {"out", Hex(outBlock.Data(), 12)},
                       {"zone", Hex(zoneBlock.Data(), 16)}, {"stack_restored", r.stack}, {"nonvolatile_preserved", r.nonvolatile}, {"seh_restored", r.seh}, {"fp", FpState(r.fp)},
                       {"fixture_contract", contract}};
            o.info = {{"ecx", r.ecx}, {"edx", r.edx}};
            o.fp = r.fp;
            restoreGlobals();
            o.gated["memory_restored"] = globalsRestored();
            o.gated["floating_state_restored"] = FloatingStateRestored();
            return o;
        };
        const Observed a = invoke(false), b = invoke(true);
        finish(1, i, a, b);
    }
    summarize(1);

    // ======================= 0x63A380 =======================
    static Arena<0x50> task;
    static Arena<0x20> sub;
    static Arena<sizeof(CVehicle)> car;
    static Arena<0x48> matrix;
    static Arena<sizeof(CPed)> ped;
    static Arena<0x48> pedMatrix;
    static Arena<0x30> col;
    static Arena<16> outs;
    auto roleOf = [&](uint32 p) -> std::string {
        if (p == 0) return "null";
        if (p == uint32(sub.Address())) return "sub";
        if (p == uint32(task.Address())) return "task";
        if (p == uint32(car.Address())) return "car";
        if (p == uint32(ped.Address())) return "ped";
        if (p == uint32(col.Address())) return "col";
        if (p == kPads) return "pad0";
        if (p == kPads + kPadSize) return "pad1";
        return Hex(&p, 4);
    };
    constexpr const char* kTraceNames[] = {"", "task_type", "aim_left_right", "aim_up_down", "accelerate", "brake", "col_model", "random"};
    for (unsigned i = 0; i < kCases3 && ready; ++i) {
        const fx::Fx3 f = fx::MakeFx3(i);
        const FpImage base = BaseFp(f.fp);
        json input = {{"case", i}, {"pass_", f.pass}, {"gate", f.gate}, {"sticks", f.sticks}, {"choice0", f.choice0}, {"ped_profile", f.pedProfile}, {"fp", f.fp}, {"car_null", f.carNull},
                      {"car_subtype", f.carSubtype}, {"sub_null", f.subNull}, {"sub_kind", f.subKind}, {"ped_type", f.pedType}, {"ped_matrix", f.pedMatrix}, {"aim_lr", f.aimLR},
                      {"aim_ud", f.aimUD}, {"accel", f.accel}, {"brake", f.brake}, {"up", json::array({f.up[0], f.up[1]})}, {"right", json::array({f.right[0], f.right[1], f.right[2]})},
                      {"car_pos", json::array({f.carPos[0], f.carPos[1], f.carPos[2]})}, {"ped_pos", json::array({f.pedPos[0], f.pedPos[1], f.pedPos[2]})}, {"distance", f.distance},
                      {"min_y", f.minY}, {"horizontal", f.horizontal}, {"vertical", f.vertical}, {"alpha", f.alpha}, {"beta", f.beta}, {"limit", f.limit}};
        auto setup = [&] {
            restoreGlobals();
            task.Fill(0xA1, 0xA5); sub.Fill(0xA2, 0xA6); car.Fill(0xA3, 0xA7); matrix.Fill(0xA4, 0xA8); ped.Fill(0xA5, 0xA9); pedMatrix.Fill(0xA6, 0xAA); col.Fill(0xA7, 0xAB); outs.Fill(0xA8, 0);
            Put32(task.Data(), uint32(uintptr_t(vtTask)));
            Put32(task.Data() + 8, f.subNull ? 0u : uint32(sub.Address()));
            Put32(task.Data() + 0xC, f.carNull ? 0u : uint32(car.Address()));
            task.Data()[0x29] = uint8(f.choice0);
            Put32(sub.Data(), uint32(uintptr_t(vtSub)));
            Put32(car.Data() + 0x594, uint32(f.carSubtype));
            Put32(car.Data() + 0x14, uint32(matrix.Address()));
            Put32(car.Data() + 0x10, 0x3EBD70A4u);
            for (unsigned k = 0; k < 3; ++k) {
                Put32(car.Data() + 4 + 4 * k, f.carPos[k]); Put32(matrix.Data() + 0x30 + 4 * k, f.carPos[k]); Put32(matrix.Data() + 4 * k, f.right[k]);
                Put32(ped.Data() + 4 + 4 * k, f.pedPos[k]); Put32(pedMatrix.Data() + 0x30 + 4 * k, f.pedPos[k]);
            }
            Put32(matrix.Data() + 0x10, f.up[0]); Put32(matrix.Data() + 0x14, f.up[1]);
            Put32(ped.Data() + 0x598, f.pedType);
            Put32(ped.Data() + 0x14, f.pedMatrix ? uint32(pedMatrix.Address()) : 0u);
            Put32(col.Data() + 4, f.minY);
            Put32(outs.Data(), f.vertical); Put32(outs.Data() + 4, f.horizontal); Put32(outs.Data() + 8, f.alpha); Put32(outs.Data() + 12, f.beta);
            scriptAimLeftRight = f.aimLR; scriptAimUpDown = f.aimUD; scriptAccelerate = f.accel; scriptBrake = f.brake;
            scriptSubTaskType = f.subKind; scriptColModel = col.Address();
            Put32(reinterpret_cast<void*>(kCameraLimit), f.limit);
            trace.clear(); trapHit = false;
        };
        auto invoke = [&](bool candidate) {
            setup();
            const std::array<std::pair<const uint8*, size_t>, 7> bodies{{{task.Data(), 0x50}, {sub.Data(), 0x20}, {car.Data(), sizeof(CVehicle)}, {matrix.Data(), 0x48}, {ped.Data(), sizeof(CPed)}, {pedMatrix.Data(), 0x48}, {col.Data(), 0x30}}};
            std::array<std::vector<uint8>, 7> before;
            for (size_t k = 0; k < bodies.size(); ++k) before[k].assign(bodies[k].first, bodies[k].first + bodies[k].second);
            bool routes = false;
            CallResult r{};
            route3.Invoke(candidate, [&] {
                routes = routeProof(0x63A380, route3, off3, candidate);
                r = Call(0x63A380, uint32(task.Address()), 0xC0DE5921u, 0xA5123456u,
                         {uint32(ped.Address()), f.distance, uint32(outs.Address()), uint32(outs.Address() + 4), uint32(outs.Address() + 8), uint32(outs.Address() + 12)}, base, true);
                routes &= routeProof(0x63A380, route3, off3, candidate);
                return 0;
            });
            // contrato de escritura: solo cambian el byte +0x29 de la tarea y los cuatro floats de salida; nada mas (ni el coche, ni el peaton, ni la subtarea, ni el pad real)
            bool contract = true;
            for (size_t k = 0; k < bodies.size(); ++k) {
                std::vector<uint8> after(bodies[k].first, bodies[k].first + bodies[k].second);
                if (k == 0) { after[0x29] = before[0][0x29]; }
                contract &= after == before[k];
            }
            contract &= task.GuardsIntact(0xA1) && sub.GuardsIntact(0xA2) && car.GuardsIntact(0xA3) && matrix.GuardsIntact(0xA4) && ped.GuardsIntact(0xA5) && pedMatrix.GuardsIntact(0xA6)
                     && col.GuardsIntact(0xA7) && outs.GuardsIntact(0xA8);
            json events = json::array();
            for (const auto& e : trace) events.push_back({kTraceNames[e[0]], roleOf(e[1]), roleOf(e[2])});
            Observed o;
            o.gated = {{"input", input}, {"route_ok", routes}, {"collaborators_ok", padCollaboratorsOk() && !trapHit}, {"vertical", W32(outs.Data())}, {"horizontal", W32(outs.Data() + 4)},
                       {"alpha_stick", W32(outs.Data() + 8)}, {"beta_stick", W32(outs.Data() + 12)}, {"choice", task.Data()[0x29]}, {"trace", events},
                       {"task", Hex(task.Data(), 0x50)}, {"sub", Hex(sub.Data(), 0x20)}, {"car", Hex(car.Data(), sizeof(CVehicle))}, {"matrix", Hex(matrix.Data(), 0x48)},
                       {"ped", Hex(ped.Data(), sizeof(CPed))}, {"ped_matrix", Hex(pedMatrix.Data(), 0x48)}, {"col", Hex(col.Data(), 0x30)},
                       {"pads", Hex(reinterpret_cast<void*>(kPads), 2 * kPadSize)}, {"camera_limit", W32(reinterpret_cast<uint8*>(kCameraLimit))},
                       {"stack_restored", r.stack}, {"nonvolatile_preserved", r.nonvolatile}, {"seh_restored", r.seh}, {"fp", FpState(r.fp)}, {"fixture_contract", contract}};
            o.info = {{"eax", r.eax}, {"ecx", r.ecx}, {"edx", r.edx}};
            o.fp = r.fp;
            restoreGlobals();
            o.gated["memory_restored"] = globalsRestored();
            o.gated["floating_state_restored"] = FloatingStateRestored();
            return o;
        };
        const Observed a = invoke(false), b = invoke(true);
        finish(2, i, a, b);
    }
    summarize(2);

    // ======================= cierre =======================
    for (auto* c : {&aimLeftRight, &aimUpDown, &accelerate, &brake, &colModel}) c->Release();
    exeRand.reset();
    dllRand.reset();
    restoreGlobals();
    VirtualProtect(reinterpret_cast<void*>(kLowerLimit), 8, rdataProtection, &rdataProtection);
    bool stateRestored = globalsRestored();
    stateRestored &= route1.Restore() && route2.Restore() && route3.Restore();
    stateRestored &= exeRandBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(0x821B1E), 16) && dllRandBefore == ReversingProbe::Bytes(reinterpret_cast<void*>(dllRandEntry), 16);
    stateRestored &= aimLeftRight.BytesRestored() && aimUpDown.BytesRestored() && accelerate.BytesRestored() && brake.BytesRestored() && colModel.BytesRestored();
    Write({{"event", "global_state_final"}, {"state_restored", stateRestored}, {"floating_state", Hex(gameFp.data(), 108)}, {"mxcsr", ReadMxcsr()}});
    Write({{"event", passed && stateRestored && pairs == kCases1 + kCases2 + kCases3 ? "complete" : "error"}, {"pairs", pairs}, {"failures", failures}, {"state_restored", stateRestored},
           {"floating_state_restored", FloatingStateRestored()}, {"per_function", json{{"CPopulation::RandomIntInLimitRange", perFunction[0]}, {"CZoneDef::GetCenter", perFunction[1]},
           {"CTaskComplexEnterCar::GetCameraStickModifier", perFunction[2]}}}});
    RestoreFloatingState();
    gameFpActive = false;
}
}  // namespace Dependencies21ELive
