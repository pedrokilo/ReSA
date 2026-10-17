#pragma once
// Banco Dependencies21C: CTaskSimpleFight::FightSetUpCol (0x61D5F0), FightHitCar (0x61D0B0) y FightStrike (0x6240B0).
// Cada par ejecuta la MISMA entrada del EXE dos veces: con el hook RH apagado (codigo original del EXE) y con el hook activo (version de la DLL).
// Los colaboradores se sustituyen por falsos con EntryPatch en la direccion del EXE y en la entrada de la DLL (Body() salta los thunks) para que las dos
// rutas vean exactamente los mismos resultados y dejen la misma traza (llamadas, argumentos en bits, orden). Se comparan: traza completa, memoria
// tocada, ESP, EBX/ESI/EDI/EBP, cabecera SEH, TOP/FCW/FTW del x87 y control del MXCSR.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "TaskSimpleFight.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Object/Object.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventVehicleDamageWeapon.h"
#include "Fx/Fx.h"
#include "Crime.h"
#include "Glass.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "Models/ModelInfo.h"
#include "Models/PedModelInfo.h"
#include "Collision/Collision.h"
#include "PedIntelligence.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace Dependencies21CLive {
inline bool done{};
inline std::ofstream log;
inline json trace = json::array();
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }

struct Fn { const char* cls; const char* name; uintptr_t address; int vt, slot, cases; };
inline const Fn fns[]{
    {"CTaskSimpleFight","FightSetUpCol",0x61D5F0,0,-1,1536},
    {"CTaskSimpleFight","FightHitCar",0x61D0B0,0,-1,1536},
    {"CTaskSimpleFight","FightStrike",0x6240B0,0,-1,1536}
};

// ---------------------------------------------------------------- utilidades
template<class T> inline T Get(const void* p, size_t o) { T v; std::memcpy(&v, static_cast<const uint8*>(p) + o, sizeof(T)); return v; }
template<class T> inline void Put(void* p, size_t o, T v) { std::memcpy(static_cast<uint8*>(p) + o, &v, sizeof(T)); }
inline uint32 Lcg(uint32& s) { s = s * 1664525u + 1013904223u; return s; }
inline uint32 Rnd(uint32& s, uint32 n) { return (Lcg(s) >> 8) % n; }
inline uintptr_t Body(uintptr_t p) { for (int i = 0; i < 16 && *reinterpret_cast<uint8*>(p) == 0xE9; ++i) p += 5 + *reinterpret_cast<int32*>(p + 1); return p; }
inline uintptr_t Addr(auto member) { return Body(std::bit_cast<uintptr_t>(member)); }

// Regiones con nombre: las trazas no llevan direcciones absolutas sino (nombre, desplazamiento).
struct Region { const char* name; uintptr_t base; size_t size; };
inline std::vector<Region> regions;
inline void Reg(const char* name, const void* base, size_t size) { regions.push_back({name, uintptr_t(base), size}); }
inline json Ptr(uintptr_t p) {
    if (!p) return nullptr;
    for (const auto& r : regions) if (p >= r.base && p < r.base + r.size) return json::array({r.name, p - r.base});
    auto* tib = reinterpret_cast<NT_TIB*>(NtCurrentTeb());
    if (p >= uintptr_t(tib->StackLimit) && p < uintptr_t(tib->StackBase)) return "stack";
    return p;
}
inline json Ptr(const void* p) { return Ptr(uintptr_t(p)); }
inline json Vec3(const void* p) { return json::array({Get<uint32>(p, 0), Get<uint32>(p, 4), Get<uint32>(p, 8)}); }
inline json Words(const void* p, size_t n) { json j = json::array(); for (size_t i = 0; i < n / 4; ++i) j.push_back(Get<uint32>(p, i * 4)); return j; }
inline json BytesJson(const std::vector<uint8>& b) { json j = json::array(); for (size_t i = 0; i + 3 < b.size(); i += 4) j.push_back(Get<uint32>(b.data(), i)); return j; }
inline std::vector<uint8> Snapshot(const void* p, size_t n) { return ReversingProbe::Bytes(p, n); }

// Identidad de las vtables de los eventos temporales (la del EXE o la de la DLL).
inline json VtableName(uint32 v) {
    static const std::array<std::pair<const char*, uint32>, 2> canonical{{{"CEventVehicleDamageWeapon", 0x86D640}, {"CEventSoundQuiet", 0x86C2B0}}};
    static const std::array<uint32, 2> ours{
        uint32(uintptr_t(ReversibleHooks::detail::GetVTableAddress("CEventVehicleDamageWeapon"))),
        uint32(uintptr_t(ReversibleHooks::detail::GetVTableAddress("CEventSoundQuiet")))};
    for (size_t i = 0; i < canonical.size(); ++i) if (v == canonical[i].second || v == ours[i]) return canonical[i].first;
    return json::array({"vtable?", v});
}

// ---------------------------------------------------------------- x87 / ABI
struct Abi {
    uint32 eax{};
    bool stack{}, nonvolatile{}, seh{}, fpuBalanced{}, fpuControl{}, mxcsrControl{};
    uint16 cw{}, tag{};
    bool operator==(const Abi&) const = default;
};
inline Abi MakeAbi(uint32 sp0, uint32 sp1, uint32 b0, uint32 b1, uint32 s0, uint32 s1, uint32 d0, uint32 d1, uint32 h0, uint32 h1, const uint8* e0, const uint8* e1, uint32 m0, uint32 m1, uint32 eaxOut) {
    Abi r;
    r.eax = eaxOut;
    r.stack = sp0 == sp1;
    r.nonvolatile = b0 == b1 && s0 == s1 && d0 == d1;
    r.seh = h0 == h1;
    const uint16 sw0 = Get<uint16>(e0, 4), sw1 = Get<uint16>(e1, 4);
    r.fpuBalanced = (sw0 & 0x3800) == (sw1 & 0x3800) && Get<uint16>(e0, 8) == Get<uint16>(e1, 8);   // TOP y palabra de etiquetas
    r.fpuControl = Get<uint16>(e0, 0) == Get<uint16>(e1, 0);                                       // FCW
    r.mxcsrControl = (m0 & ~0x3Fu) == (m1 & ~0x3Fu);                                               // control del MXCSR (los indicadores acumulados no son contrato)
    r.cw = Get<uint16>(e1, 0);
    r.tag = Get<uint16>(e1, 8);
    return r;
}
inline json AbiJson(const Abi& a) {
    return {{"stack_restored", a.stack}, {"nonvolatile_preserved", a.nonvolatile}, {"seh_restored", a.seh}, {"fpu_top_tag_balanced", a.fpuBalanced},
            {"fpu_control_preserved", a.fpuControl}, {"mxcsr_control_preserved", a.mxcsrControl}, {"fcw_after", a.cw}, {"ftw_after", a.tag}};
}

// thiscall (this, radius) ret 4
inline Abi CallSetUpCol(uintptr_t entry, void* self, uint32 radiusBits) {
    uint32 sp0, sp1, b0, b1, s0, s1, d0, d1, h0, h1, m0, m1, eaxOut;
    alignas(4) uint8 e0[32]{}, e1[32]{};
    __asm {
        lea eax, e0
        fnstenv [eax]
        fldenv [eax]
        stmxcsr m0
        mov ebx, 0B1B1B1B1h
        mov esi, 05151A1A1h
        mov edi, 0D1D1E1E1h
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov eax, fs:[0]
        mov h0, eax
        push radiusBits
        mov ecx, self
        mov edx, 05AFEC0DEh
        mov eax, 0A5123456h
        call entry
        mov eaxOut, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov eax, fs:[0]
        mov h1, eax
        lea eax, e1
        fnstenv [eax]
        fldenv [eax]
        stmxcsr m1
    }
    auto r = MakeAbi(sp0, sp1, b0, b1, s0, s1, d0, d1, h0, h1, e0, e1, m0, m1, eaxOut);
    r.nonvolatile = r.nonvolatile && b1 == 0xB1B1B1B1u && s1 == 0x5151A1A1u && d1 == 0xD1D1E1E1u;
    return r;
}

// thiscall (this, ped, vehicle, posn1, posn2, componente, superficie) ret 0x18
inline Abi CallHitCar(uintptr_t entry, void* self, void* ped, void* vehicle, void* posn1, void* posn2, uint32 component, uint32 surface) {
    uint32 sp0, sp1, b0, b1, s0, s1, d0, d1, h0, h1, m0, m1, eaxOut;
    alignas(4) uint8 e0[32]{}, e1[32]{};
    __asm {
        lea eax, e0
        fnstenv [eax]
        fldenv [eax]
        stmxcsr m0
        mov ebx, 0B1B1B1B1h
        mov esi, 05151A1A1h
        mov edi, 0D1D1E1E1h
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov eax, fs:[0]
        mov h0, eax
        push surface
        push component
        push posn2
        push posn1
        push vehicle
        push ped
        mov ecx, self
        mov edx, 05AFEC0DEh
        mov eax, 0A5123456h
        call entry
        mov eaxOut, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov eax, fs:[0]
        mov h1, eax
        lea eax, e1
        fnstenv [eax]
        fldenv [eax]
        stmxcsr m1
    }
    auto r = MakeAbi(sp0, sp1, b0, b1, s0, s1, d0, d1, h0, h1, e0, e1, m0, m1, eaxOut);
    r.nonvolatile = r.nonvolatile && b1 == 0xB1B1B1B1u && s1 == 0x5151A1A1u && d1 == 0xD1D1E1E1u;
    return r;
}

// thiscall (this, ped, posn) ret 8
inline Abi CallStrike(uintptr_t entry, void* self, void* ped, void* position) {
    uint32 sp0, sp1, b0, b1, s0, s1, d0, d1, h0, h1, m0, m1, eaxOut;
    alignas(4) uint8 e0[32]{}, e1[32]{};
    __asm {
        lea eax, e0
        fnstenv [eax]
        fldenv [eax]
        stmxcsr m0
        mov ebx, 0B1B1B1B1h
        mov esi, 05151A1A1h
        mov edi, 0D1D1E1E1h
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov eax, fs:[0]
        mov h0, eax
        push position
        push ped
        mov ecx, self
        mov edx, 05AFEC0DEh
        mov eax, 0A5123456h
        call entry
        mov eaxOut, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov eax, fs:[0]
        mov h1, eax
        lea eax, e1
        fnstenv [eax]
        fldenv [eax]
        stmxcsr m1
    }
    auto r = MakeAbi(sp0, sp1, b0, b1, s0, s1, d0, d1, h0, h1, e0, e1, m0, m1, eaxOut);
    r.nonvolatile = r.nonvolatile && b1 == 0xB1B1B1B1u && s1 == 0x5151A1A1u && d1 == 0xD1D1E1E1u;
    return r;
}

// Dos parches (EXE y DLL) con el mismo falso.
// IMPORTANTE: el cuerpo de la DLL se resuelve ANTES de parchear la entrada del EXE. Si la funcion de la DLL esta sin enganchar (categoria Audio,
// que el runner desactiva con --unhook), su cuerpo es `jmp EXE`: resuelto despues del primer parche, la cadena de E9 sigue por el salto recien
// escrito hasta el cuerpo del propio falso y el segundo EntryPatch lo parchea contra si mismo (bucle infinito: "GTA no responde"). `Addr()` ya
// resuelve la cadena en el sitio de la llamada, antes de construir el falso; aqui no se vuelve a resolver tras el primer parche.
struct Fake {
    const uintptr_t dllBody;
    ReversingProbe::EntryPatch exe, dll;
    template<class F> Fake(uintptr_t exeAddress, uintptr_t dllAddress, F fake) : dllBody(Body(dllAddress)), exe(exeAddress, fake), dll(dllBody, fake) {}
};
// Solo el EXE (colaboradores sin reconstruir).
struct FakeExe {
    ReversingProbe::EntryPatch exe;
    template<class F> FakeExe(uintptr_t exeAddress, F fake) : exe(exeAddress, fake) {}
};

// ---------------------------------------------------------------- valores de estres
// Floats: 0 con signo, subnormales, limites, grandes, infinitos, NaN silenciosos con distinta carga y senalizantes.
inline constexpr uint32 stressBits[]{
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x3F000000, 0x40A00000, 0x42C80000, 0x447A0000,
    0x7149F2CA, 0xF149F2CA, 0x0DA24260, 0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FC00001, 0xFFC12345, 0x7FFFFFFF, 0xFFFFFFFF,
    0x7F800001, 0xFF800001, 0x7FA00000, 0xFFBFFFFF, 0x7F8AAAAA, 0xFFA55555, 0x3DCCCCCD, 0x3E99999A, 0x3FC00000, 0x40490FDB, 0x00400000, 0x1F800000, 0x5F800000, 0x7E967699};
inline constexpr size_t stressCount = std::size(stressBits);
inline uint32 StressBits(uint32& s) { return stressBits[Rnd(s, uint32(stressCount))]; }
inline bool IsNan(uint32 b) { return (b & 0x7FFFFFFFu) > 0x7F800000u; }
inline bool IsSignalling(uint32 b) { return IsNan(b) && !(b & 0x00400000u); }
inline bool IsDenormal(uint32 b) { return (b & 0x7F800000u) == 0 && (b & 0x007FFFFFu) != 0; }
inline uint32 FloatBitsOf(float f) { return std::bit_cast<uint32>(f); }
inline float FloatOfBits(uint32 b) { return std::bit_cast<float>(b); }

// Contadores de cobertura (se alimentan de los fixtures y de las trazas del candidato).
inline std::map<std::string, int> coverage;
inline void Cover(const char* tag) { ++coverage[tag]; }

// ================================================================ comparacion comun
inline constexpr std::array<const char*, 8> gateNames{"route_ok", "stack_restored", "nonvolatile_preserved", "seh_restored", "fpu_top_tag_balanced", "fpu_control_preserved", "mxcsr_control_preserved", "guards_intact"};
inline bool GatesOk(const json& v) {
    for (const char* k : gateNames) if (!v.contains(k) || v[k] != true) return false;
    return v.contains("inputs_intact") && v["inputs_intact"] == true;
}
inline int pairCounter{};
inline bool memoryRestored{true}, hooksRestored{true};
inline bool allPassed{true};
inline bool Report(const Fn& fn, unsigned c, const json& original, const json& candidate, int& failures) {
    const bool equal = original == candidate && GatesOk(original) && GatesOk(candidate);
    allPassed &= equal;
    failures += !equal;
    Write({{"event", "comparison"}, {"function", std::string(fn.cls) + "::" + fn.name}, {"pair", pairCounter++}, {"case", c}, {"original", original}, {"candidate", candidate}, {"equal", equal}, {"accepted", equal}});
    return equal;
}

// ================================================================ FightSetUpCol (0x61D5F0)
// Memoria tocada: CColModel estatico 0xC17824 (0x30) + CCollisionData 0xC17854 + una CColSphere 0xC17884. Unico colaborador: CColSphere::Set (0x40FD10).
namespace SetUp {
inline constexpr uintptr_t base = 0xC17824;
inline constexpr size_t size = 0x80;
alignas(16) inline std::array<uint8, 0x28> task;

inline void __fastcall FakeSet(CColSphere* self, void*, uint32 radius, const CVector* center, uint32 material, uint32 piece, uint32 lighting) {
    trace.push_back({"sphere_set", Ptr(self), radius, Vec3(center), material & 0xFFu, piece & 0xFFu, lighting & 0xFFu});
}

inline uint32 RadiusBits(unsigned c) {
    const unsigned index = c % 64;
    if (index < stressCount) return stressBits[index];
    uint32 s = c * 40503u + 77u;
    return 0x3F000000u + (Lcg(s) >> 9) % 0x01000000u + ((index & 1) ? 0x80000000u : 0u);   // flotantes normales de ambos signos
}
inline void Prepare(unsigned c) {
    uint32 s = c * 2654435761u + 0x1234567u;
    auto* p = reinterpret_cast<uint8*>(base);
    for (size_t i = 0; i < size; ++i) p[i] = uint8(Lcg(s) >> 24);
    switch ((c / 64) % 3) {
    case 0: Put<uint32>(p, 0x2C, 0); break;                                              // sin inicializar: [0xC17850] == 0
    case 1: Put<uint32>(p, 0x2C, 0xC17854); Put<uint32>(p, 0x38, 0xC17884); Put<uint16>(p, 0x30, 1); break; // ya preparado
    default: Put<uint32>(p, 0x2C, 0x00ABCDEF | 1u); Put<uint32>(p, 0x38, 0x00123450u); break;           // puntero de datos ajeno: no se toca
    }
    for (auto& b : task) b = uint8(Lcg(s) >> 24);
}
inline json One(unsigned c, bool candidate, OriginalDependencies499::HookRoute& route) {
    Prepare(c);
    trace = json::array();
    const auto guardLow = Snapshot(reinterpret_cast<void*>(base - 0x40), 0x40), guardHigh = Snapshot(reinterpret_cast<void*>(base + size), 0x40);
    const auto taskBefore = task;
    Abi abi{};
    bool routeOk = false;
    route.Invoke(candidate, [&] {
        routeOk = ReversingProbe::JumpsToDll(0x61D5F0) == candidate;
        abi = CallSetUpCol(0x61D5F0, task.data(), RadiusBits(c));
        return 0;
    });
    json v = AbiJson(abi);
    v["route_ok"] = routeOk;
    v["trace"] = trace;
    v["memory"] = BytesJson(Snapshot(reinterpret_cast<void*>(base), size));
    v["guards_intact"] = guardLow == Snapshot(reinterpret_cast<void*>(base - 0x40), 0x40) && guardHigh == Snapshot(reinterpret_cast<void*>(base + size), 0x40);
    v["inputs_intact"] = taskBefore == task;
    return v;
}
inline void Run(int& failures) {
    const Fn& fn = fns[0];
    OriginalDependencies499::HookRoute route(fn.cls, fn.name, fn.address, FunctionToVoidPtr(&CTaskSimpleFight::FightSetUpCol));
    const auto saved = Snapshot(reinterpret_cast<void*>(base - 0x40), size + 0x80);
    regions.clear();
    Reg("static_col", reinterpret_cast<void*>(base), 0x30);
    Reg("static_col_data", reinterpret_cast<void*>(base + 0x30), 0x30);
    Reg("static_sphere", reinterpret_cast<void*>(0xC17884), 0x14);
    Reg("task", task.data(), task.size());
    if (!route.Registered()) { Write({{"event", "error"}, {"reason", "hook FightSetUpCol no registrado"}}); allPassed = false; return; }
    {
        Fake set(0x40FD10, Addr(&CColSphere::Set), FakeSet);
        for (unsigned c = 0; c < unsigned(fn.cases); ++c) {
            const auto original = One(c, false, route);
            const auto candidate = One(c, true, route);
            Report(fn, c, original, candidate, failures);
            if (candidate["trace"].size() == 1 && candidate["trace"][0][1] == json::array({"static_sphere", 0})) Cover("setup.initialised_sphere"); else Cover("setup.foreign_sphere");
            if (IsNan(RadiusBits(c))) Cover(IsSignalling(RadiusBits(c)) ? "setup.snan" : "setup.qnan");
            if (IsDenormal(RadiusBits(c))) Cover("setup.denormal");
        }
    }
    memcpy(reinterpret_cast<void*>(base - 0x40), saved.data(), saved.size());
    memoryRestored &= saved == Snapshot(reinterpret_cast<void*>(base - 0x40), size + 0x80);
    hooksRestored &= route.Restore();
}
}

// Palabras que cambian entre dos instantaneas: [[desplazamiento, valor nuevo], ...]
inline json ChangedWords(const std::vector<uint8>& before, const std::vector<uint8>& after) {
    json j = json::array();
    for (size_t i = 0; i + 3 < before.size() && i + 3 < after.size(); i += 4)
        if (Get<uint32>(before.data(), i) != Get<uint32>(after.data(), i)) j.push_back(json::array({i, Get<uint32>(after.data(), i)}));
    return j;
}

// ================================================================ FightHitCar (0x61D0B0)
// Colaboradores falsos: GetStrikeDamage (0x61C740), anim de hueso (0x734A40/0x7C51A0/0x7C5120), Fx_c::AddSparks/AddPunchImpact (0x49F040/0x49F670),
// VehicleDamage (virtual +0E0h), CCrime::ReportCrime (0x532010), CEventGroup::Add (0x4AB420), CEntity::RegisterReference/CleanUpOldReference (0x571B70/0x571A00),
// CAEPedWeaponAudioEntity::AddAudioEvent (0x4E69F0) y CAEPedAudioEntity::AddAudioEvent (0x4E2BB0). Los eventos temporales (4B18D0/4B1990) y el resto corren de verdad.
namespace HitCar {
alignas(16) inline std::array<uint8, 0x28> task;
alignas(16) inline std::array<uint8, sizeof(CPed)> ped, driver;
alignas(16) inline std::array<std::array<uint8, sizeof(CPed)>, 8> passengers;
alignas(16) inline std::array<std::array<uint8, sizeof(CPedIntelligence)>, 9> intel;
alignas(16) inline std::array<uint8, sizeof(CAutomobile)> vehicle;
alignas(16) inline std::array<uint8, sizeof(tHandlingData)> handling;
alignas(16) inline std::array<uintptr_t, 64> vtable;
alignas(16) inline std::array<uint8, 16> pos1, pos2, clump, hierarchy;
alignas(16) inline std::array<uint8, 64 * 8> bones;

// Tabla de sonidos de golpe que lee el EXE: (combo * 0x22 + movimiento) * 4 + 0xC16F0C. Solo se rellena la tabla de combos (13 filas de 0x88 desde 0xC170D0, como el banco de
// FightStrike): lo anterior (0xC16E70..0xC170CF) son globales vivas del juego (CDraw/FadeTimer, ruta de guardado, Scene, RsGlobal...) que no se pueden pisar con basura.
// Los combos -1, 0 y 3 leen alli sin escribir: el valor es el mismo en la ruta original y en la candidata.
inline constexpr uintptr_t soundLow = 0xC170D0, soundSize = 13 * 0x88;

struct Params {
    uint32 healthBefore{}, healthAfter{}, damage{}, mass{}, swapType{};
    int32 slot{};
    uint32 weaponType{};
    bool chainsaw{};
    bool hasDriver{};
    uint32 maxPassengers{}, passengerMask{};
    int32 combo{}, move{}, boneIndex{};
    uint32 component{}, surface{};
};
inline Params Cur;
inline uint32 damageBits, massBits;

inline Params Decode(unsigned c) {
    uint32 s = c * 2654435761u + 0xC0FFEEu;
    Params p;
    static constexpr int32 slots[]{0, 1, 2, 3, 5, 12, 0xFF, 0, 4};
    p.slot = slots[Rnd(s, 9)];
    const uint32 w = Rnd(s, 8);
    p.weaponType = w < 3 ? 9u : w == 3 ? 0u : w == 4 ? 5u : Rnd(s, 47);
    p.chainsaw = p.weaponType == 9;
    p.healthBefore = (Rnd(s, 3) == 0) ? StressBits(s) : FloatBitsOf(1000.f - float(Rnd(s, 700)));
    const uint32 h = Rnd(s, 6);
    p.healthAfter = h == 0 ? p.healthBefore : h == 1 ? p.healthBefore - 1u : h == 2 ? p.healthBefore + 1u : (h == 3 ? FloatBitsOf(float(Rnd(s, 1000))) : StressBits(s));
    static constexpr std::array<std::pair<uint32, uint32>, 6> products{{
        {std::bit_cast<uint32>(3.0e20f), std::bit_cast<uint32>(1.0e20f)}, {std::bit_cast<uint32>(1.0e-20f), std::bit_cast<uint32>(1.5e-19f)},
        {std::bit_cast<uint32>(3.0e30f), std::bit_cast<uint32>(1.0e9f)}, {std::bit_cast<uint32>(1.0e-30f), std::bit_cast<uint32>(1.0e-30f)},
        {0x7F7FFFFFu, 0x3F800000u}, {0x00800000u, 0x3F000000u}}};
    if (Rnd(s, 4) == 0) { const auto& pr = products[Rnd(s, 6)]; p.mass = pr.first; p.damage = pr.second; }
    else { p.mass = Rnd(s, 3) == 0 ? StressBits(s) : FloatBitsOf(float(100 + Rnd(s, 4000))); p.damage = Rnd(s, 2) == 0 ? StressBits(s) : FloatBitsOf(float(Rnd(s, 400)) * 0.25f); }
    p.swapType = (Rnd(s, 4) == 0) ? (p.chainsaw ? 5u : 9u) : 0xFFFFFFFFu;
    p.hasDriver = Rnd(s, 4) != 0;
    p.maxPassengers = Rnd(s, 9);
    p.passengerMask = Lcg(s) >> 8;
    static constexpr int32 combos[]{-1, 0, 3, 4, 5, 7, 11, 15, 16, 4, 5};
    p.combo = combos[Rnd(s, 11)];
    p.move = int32(Rnd(s, 6)) - 1;
    p.boneIndex = Rnd(s, 8) == 0 ? -1 : int32(Rnd(s, 4));
    static constexpr uint32 components[]{0, 1, 0x7FFF, 0xFFFF, 0x8000, 0x00FF, 0x1234, 0xC3};
    p.component = (components[Rnd(s, 8)] & 0xFFFFu) | (Rnd(s, 2) ? 0xA5A50000u : 0u);
    static constexpr uint32 surfaces[]{0, 1, 0x7F, 0x80, 0xFF, 5, 0x44, 0x13};
    p.surface = surfaces[Rnd(s, 8)];
    return p;
}

inline float __fastcall FakeDamage(CTaskSimpleFight*, void*, CPed* who) { trace.push_back({"strike_damage", Ptr(who)}); return FloatOfBits(damageBits); }
inline void __fastcall FakeVehicleDamage(CVehicle* self, void*, uint32 intensity, uint32 component, CEntity* damager, CVector* coors, CVector* direction, uint32 weapon) {
    trace.push_back({"vehicle_damage", Ptr(self), intensity, component & 0xFFFFu, Ptr(damager), Ptr(coors), Vec3(coors), Ptr(direction), Vec3(direction), weapon});
    Put<uint32>(self, 0x4C0, Cur.healthAfter);
    if (Cur.swapType != 0xFFFFFFFFu) Put<uint32>(ped.data(), 0x5A0 + int32(int8(Cur.slot)) * 0x1C, Cur.swapType);   // el arma se relee tras VehicleDamage
}
inline void __cdecl FakeCrime(uint32 type, CEntity* victim, CPed* who) { trace.push_back({"crime", type, Ptr(victim), Ptr(who)}); }
inline CEvent* __fastcall FakeGroupAdd(CEventGroup* group, void*, CEvent* event, uint32 valid) {
    const auto* b = reinterpret_cast<const uint8*>(event);
    trace.push_back({"group_add", Ptr(group), VtableName(Get<uint32>(b, 0)), Get<uint32>(b, 4), b[8], b[0xC], Get<uint16>(b, 0xE), Get<uint16>(b, 0x10),
                     Ptr(Get<uint32>(b, 0x14)), Ptr(Get<uint32>(b, 0x18)), Get<uint32>(b, 0x1C), valid & 0xFFu});
    return event;
}
inline void __fastcall FakeRegister(CEntity* self, void*, CEntity** ref) { trace.push_back({"register", Ptr(self), Ptr(ref), Ptr(ref ? uintptr_t(*ref) : 0)}); }
inline void __fastcall FakeCleanUp(CEntity* self, void*, CEntity** ref) { trace.push_back({"cleanup", Ptr(self), Ptr(ref), Ptr(ref ? uintptr_t(*ref) : 0)}); }
inline void __fastcall FakeWeaponAudio(void* self, void*, int event) { trace.push_back({"weapon_audio", Ptr(self), event}); }
inline void __fastcall FakePedAudio(void* self, void*, int event, uint32 volume, uint32 speed, CPhysical* physical, uint32 surface, int a7, uint32 maxVolume) {
    trace.push_back({"ped_audio", Ptr(self), event, volume, speed, Ptr(physical), surface & 0xFFu, a7, maxVolume});
}
inline void __fastcall FakeSparks(void* self, void*, const CVector* origin, const CVector* direction, uint32 force, int amount, uint32 ax, uint32 ay, uint32 az, uint32 type, uint32 spread, uint32 life) {
    trace.push_back({"sparks", Ptr(self), Ptr(origin), Vec3(origin), Vec3(direction), force, amount, ax, ay, az, type & 0xFFu, spread, life});
}
inline void __fastcall FakePunch(void* self, void*, const CVector* a, const CVector* b, int amount) { trace.push_back({"punch_impact", Ptr(self), Ptr(a), Vec3(a), Ptr(b), Vec3(b), amount}); }
inline RpHAnimHierarchy* __cdecl FakeHierarchy(RpClump* c) { trace.push_back({"anim_hierarchy", Ptr(c)}); return reinterpret_cast<RpHAnimHierarchy*>(hierarchy.data()); }
inline int __cdecl FakeBoneIndex(RpHAnimHierarchy* h, int id) { trace.push_back({"bone_index", Ptr(h), id}); return Cur.boneIndex; }
inline RwMatrix* __cdecl FakeMatrices(RpHAnimHierarchy* h) { trace.push_back({"bone_matrices", Ptr(h)}); return reinterpret_cast<RwMatrix*>(bones.data() + 64); }

inline void Prepare(unsigned c) {
    Cur = Decode(c);
    damageBits = Cur.damage;
    massBits = Cur.mass;
    uint32 s = c * 40503u + 12345u;
    auto fill = [&](auto& a) { for (auto& b : a) b = uint8(Lcg(s) >> 24); };
    fill(task); fill(ped); fill(driver); for (auto& a : passengers) fill(a); for (auto& a : intel) fill(a); fill(vehicle); fill(handling); fill(pos1); fill(pos2); fill(clump); fill(hierarchy); fill(bones);
    for (auto& e : vtable) e = 0xDEAD0000u + uintptr_t(&e - vtable.data());
    vtable[56] = reinterpret_cast<uintptr_t>(&FakeVehicleDamage);
    // tarea: combo y movimiento (+24h/+25h)
    Put<int8>(task.data(), 0x24, int8(Cur.combo));
    Put<int8>(task.data(), 0x25, int8(Cur.move));
    // atacante: clump (+18h), ranura activa (+718h) y tipos de arma
    Put<uintptr_t>(ped.data(), 0x18, uintptr_t(clump.data()));
    Put<uint8>(ped.data(), 0x718, uint8(Cur.slot));
    for (int k = -1; k <= 12; ++k) Put<uint32>(ped.data(), 0x5A0 + k * 0x1C, Rnd(s, 47));
    Put<uint32>(ped.data(), 0x5A0 + int32(int8(Cur.slot)) * 0x1C, Cur.weaponType);
    // vehiculo
    Put<uintptr_t>(vehicle.data(), 0, uintptr_t(vtable.data()));
    Put<uintptr_t>(vehicle.data(), 0x384, uintptr_t(handling.data()));
    Put<uint32>(handling.data(), 4, Cur.mass);
    Put<uintptr_t>(vehicle.data(), 0x460, Cur.hasDriver ? uintptr_t(driver.data()) : 0);
    for (int i = 0; i < 8; ++i) Put<uintptr_t>(vehicle.data(), 0x464 + i * 4, (Cur.passengerMask >> i) & 1 ? uintptr_t(passengers[i].data()) : 0);
    Put<uint8>(vehicle.data(), 0x488, uint8(Cur.maxPassengers));
    Put<uint32>(vehicle.data(), 0x4C0, Cur.healthBefore);
    // ocupantes: cada uno con su inteligencia (grupo de eventos en +68h)
    Put<uintptr_t>(driver.data(), 0x47C, uintptr_t(intel[8].data()));
    for (int i = 0; i < 8; ++i) Put<uintptr_t>(passengers[i].data(), 0x47C, uintptr_t(intel[i].data()));
    // huesos: vector `at` (+20h) de cada matriz con valores de estres
    for (int m = 0; m < 8; ++m) for (int k = 0; k < 3; ++k) Put<uint32>(bones.data(), m * 64 + 0x20 + k * 4, StressBits(s));
    // posiciones: tres floats de estres cada una
    for (int k = 0; k < 3; ++k) { Put<uint32>(pos1.data(), k * 4, StressBits(s)); Put<uint32>(pos2.data(), k * 4, StressBits(s)); }
    // tabla de sonidos
    uint32 t = c * 7919u + 3u;
    auto* table = reinterpret_cast<uint8*>(soundLow);
    for (size_t i = 0; i < soundSize; ++i) table[i] = uint8(Lcg(t) >> 24);
}

inline json One(unsigned c, bool candidate, OriginalDependencies499::HookRoute& route) {
    Prepare(c);
    trace = json::array();
    const auto pedBefore = Snapshot(ped.data(), ped.size()), vehicleBefore = Snapshot(vehicle.data(), vehicle.size()), taskBefore = Snapshot(task.data(), task.size()),
               handlingBefore = Snapshot(handling.data(), handling.size()), pos1Before = Snapshot(pos1.data(), pos1.size()), pos2Before = Snapshot(pos2.data(), pos2.size()),
               driverBefore = Snapshot(driver.data(), driver.size()), bonesBefore = Snapshot(bones.data(), bones.size());
    std::array<std::vector<uint8>, 8> passengersBefore;
    for (size_t i = 0; i < 8; ++i) passengersBefore[i] = Snapshot(passengers[i].data(), passengers[i].size());
    const auto guard = Snapshot(reinterpret_cast<void*>(soundLow + soundSize), 0x40);
    Abi abi{};
    bool routeOk = false;
    route.Invoke(candidate, [&] {
        routeOk = ReversingProbe::JumpsToDll(0x61D0B0) == candidate;
        abi = CallHitCar(0x61D0B0, task.data(), ped.data(), vehicle.data(), pos1.data(), pos2.data(), Cur.component, Cur.surface);
        return 0;
    });
    json v = AbiJson(abi);
    v["route_ok"] = routeOk;
    v["trace"] = trace;
    v["changes"] = {{"ped", ChangedWords(pedBefore, Snapshot(ped.data(), ped.size()))}, {"vehicle", ChangedWords(vehicleBefore, Snapshot(vehicle.data(), vehicle.size()))},
                    {"driver", ChangedWords(driverBefore, Snapshot(driver.data(), driver.size()))}};
    json pass = json::array();
    for (size_t i = 0; i < 8; ++i) pass.push_back(ChangedWords(passengersBefore[i], Snapshot(passengers[i].data(), passengers[i].size())));
    v["changes"]["passengers"] = pass;
    v["guards_intact"] = guard == Snapshot(reinterpret_cast<void*>(soundLow + soundSize), 0x40);
    v["inputs_intact"] = taskBefore == Snapshot(task.data(), task.size()) && handlingBefore == Snapshot(handling.data(), handling.size()) && pos1Before == Snapshot(pos1.data(), pos1.size()) &&
                         pos2Before == Snapshot(pos2.data(), pos2.size()) && bonesBefore == Snapshot(bones.data(), bones.size());
    return v;
}

inline void Run(int& failures) {
    const Fn& fn = fns[1];
    OriginalDependencies499::HookRoute route(fn.cls, fn.name, fn.address, FunctionToVoidPtr(&CTaskSimpleFight::FightHitCar));
    if (!route.Registered()) { Write({{"event", "error"}, {"reason", "hook FightHitCar no registrado"}}); allPassed = false; return; }
    regions.clear();
    Reg("task", task.data(), task.size()); Reg("ped", ped.data(), ped.size()); Reg("driver", driver.data(), driver.size());
    for (size_t i = 0; i < 8; ++i) { static std::array<std::string, 8> names; names[i] = "passenger" + std::to_string(i); Reg(names[i].c_str(), passengers[i].data(), passengers[i].size()); }
    for (size_t i = 0; i < 9; ++i) { static std::array<std::string, 9> names; names[i] = "intel" + std::to_string(i); Reg(names[i].c_str(), intel[i].data(), intel[i].size()); }
    Reg("vehicle", vehicle.data(), vehicle.size()); Reg("handling", handling.data(), handling.size()); Reg("pos1", pos1.data(), pos1.size()); Reg("pos2", pos2.data(), pos2.size());
    Reg("clump", clump.data(), clump.size()); Reg("hierarchy", hierarchy.data(), hierarchy.size()); Reg("bones", bones.data(), bones.size()); Reg("g_fx", &g_fx, sizeof(Fx_c));
    const auto saved = Snapshot(reinterpret_cast<void*>(soundLow), soundSize + 0x40);
    {
        FakeExe damageFake(0x61C740, FakeDamage);
        Fake hier(0x734A40, reinterpret_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump), FakeHierarchy);
        Fake bone(0x7C51A0, reinterpret_cast<uintptr_t>(&RpHAnimIDGetIndex), FakeBoneIndex);
        Fake mats(0x7C5120, reinterpret_cast<uintptr_t>(&RpHAnimHierarchyGetMatrixArray), FakeMatrices);
        Fake sparks(0x49F040, Addr(&Fx_c::AddSparks), FakeSparks);
        Fake punch(0x49F670, Addr(&Fx_c::AddPunchImpact), FakePunch);
        Fake crime(0x532010, reinterpret_cast<uintptr_t>(&CCrime::ReportCrime), FakeCrime);
        Fake add(0x4AB420, Addr(static_cast<CEvent* (CEventGroup::*)(CEvent*, bool)>(&CEventGroup::Add)), FakeGroupAdd);
        Fake reg(0x571B70, Addr(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::RegisterReference)), FakeRegister);
        Fake clean(0x571A00, Addr(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)), FakeCleanUp);
        Fake waudio(0x4E69F0, Addr(&CAEPedWeaponAudioEntity::AddAudioEvent), FakeWeaponAudio);
        Fake paudio(0x4E2BB0, Addr(&CAEPedAudioEntity::AddAudioEvent), FakePedAudio);
        for (unsigned c = 0; c < unsigned(fn.cases); ++c) {
            const auto original = One(c, false, route);
            const auto candidate = One(c, true, route);
            Report(fn, c, original, candidate, failures);
            const Params q = Decode(c);
            Cover(q.chainsaw ? "hitcar.chainsaw" : "hitcar.other_weapon");
            if (q.hasDriver) Cover("hitcar.driver");
            if (q.maxPassengers && (q.passengerMask & ((1u << std::min(8u, q.maxPassengers)) - 1))) Cover("hitcar.passengers");
            if (q.swapType != 0xFFFFFFFFu) Cover("hitcar.weapon_swapped_after_damage");
            if (q.boneIndex < 0 && q.chainsaw) Cover("hitcar.bone_index_minus_one");
            const auto& tr = candidate["trace"];
            for (const auto& e : tr) if (e[0] == "register" && e[2] == json::array({"vehicle", 0x50C})) { Cover("hitcar.last_damage_registered"); break; }
            for (const auto& e : tr) if (e[0] == "weapon_audio") { Cover("hitcar.chainsaw_sound"); break; }
            if (IsNan(q.healthBefore) || IsNan(q.healthAfter)) Cover("hitcar.nan_health");
            if (IsNan(q.damage) || IsNan(q.mass)) Cover("hitcar.nan_damage_or_mass");
        }
    }
    memcpy(reinterpret_cast<void*>(soundLow), saved.data(), saved.size());
    memoryRestored &= saved == Snapshot(reinterpret_cast<void*>(soundLow), soundSize + 0x40);
    hooksRestored &= route.Restore();
}
}

// ================================================================ FightStrike (0x6240B0)
// Colaboradores falsos (EXE y DLL): GetWeaponInfo, FindPlayerPed, BreakGlassPhysically, CPed::IsPlayer/IsAlive, FindObjectsInRange, FightSetUpCol, GetBoundCentre,
// AnimatePedColModelSkinnedWorld, AllocateMatrix, CSimpleTransform::UpdateMatrix, CEntity::GetColModel, ProcessColModels, FightHitCar, GetEventGlobalGroup,
// CEventGroup::Add, RpAnimBlendClumpGetAssociation, RegisterReference/CleanUpOldReference; y solo en el EXE FightHitPed (0x61CBA0) y FightHitObj (0x61D400).
// Corren de verdad: CMatrix (copia, SetTranslateOnly, destructor) y CEventSoundQuiet (constructor/destructor, que usan los falsos de referencias).
namespace Strike {
inline constexpr int N = 16;
inline constexpr uintptr_t tableBase = 0xC170D0, tableSize = 13 * 0x88;
alignas(16) inline std::array<uint8, 0x28> task;
alignas(16) inline std::array<uint8, sizeof(CPed)> attacker;
alignas(16) inline std::array<uint8, sizeof(CMatrixLink)> attackerMatrix;
alignas(16) inline std::array<uint8, sizeof(CPedIntelligence)> intelligence;
alignas(16) inline std::array<std::array<uint8, sizeof(CPed)>, N> peds;
alignas(16) inline std::array<std::array<uint8, sizeof(CAutomobile)>, N> vehicles;
alignas(16) inline std::array<std::array<uint8, sizeof(CObject)>, N> objects;
alignas(16) inline std::array<std::array<uint8, sizeof(CMatrixLink)>, 2 * N + 1> matrices;   // 0..15 vehiculos, 16..31 objetos, 32 la que reparte AllocateMatrix
alignas(16) inline std::array<std::array<uint8, 16>, N + 1> clumps;                         // clump de cada ped (16 = atacante)
alignas(16) inline std::array<std::array<uint8, 0x30>, 8> modelInfos, modelCols;            // CBaseModelInfo (solo +14h) y su CColModel (radio de esfera en +24h)
alignas(16) inline std::array<std::array<uint8, 0x30>, 2 * N> entityCols;                   // CColModel devuelto por GetColModel (0..15 vehiculos, 16..31 objetos)
alignas(16) inline std::array<std::array<uint8, 0x30>, N> pedCols;                          // CColModel que devuelve AnimatePedColModelSkinnedWorld
alignas(16) inline std::array<std::array<uint8, 0x30>, N> pedColData;                       // CCollisionData: cuenta (+0) y puntero a esferas (+8)
alignas(16) inline std::array<std::array<uint8, 4 * 0x14>, N> pedSpheres;
alignas(16) inline std::array<uint8, sizeof(CAnimBlendAssociation)> anim, otherAnim;
alignas(16) inline std::array<uint8, 0x100> globalGroup;
alignas(16) inline std::array<uint8, 16> posn, weaponInfo;
inline std::array<int32, 8> modelSlot{};
inline std::array<CBaseModelInfo*, 8> savedModelPtr{};

struct Desc {
    int32 combo{}, move{}, slot{};
    uint32 weaponType{};
    bool hasAnim{}, attackerIsPlayer{}, noPlayer{}, playerIsAttacker{}, assocFound{};
    uint32 pedMask{}, vehMask{}, numObjects{};
    std::array<bool, N> alive{}, collision{}, inVehicle{};
    std::array<int, N> pedVehicle{};
    std::array<uint32, N> vehType{}, hitPedMode{};
    std::array<int, 2 * N> processCount{};
    std::array<std::array<uint32, 3>, 3 * N> centre{};      // [0..15] peds, [16..31] vehiculos, [32..47] objetos
    uint32 seed{};
};
inline Desc D;
inline unsigned caseIndex{};

inline int IndexIn(const auto& arrays, const void* p) { for (size_t i = 0; i < arrays.size(); ++i) if (p == arrays[i].data()) return int(i); return -1; }
inline int PedIx(const void* p) { return IndexIn(peds, p); }
inline int VehIx(const void* p) { return IndexIn(vehicles, p); }
inline int ObjIx(const void* p) { return IndexIn(objects, p); }

// ---- falsos ----
inline void* __cdecl FakeWeaponInfo(uint32 type, uint32 skill) { trace.push_back({"weapon_info", type, skill & 0xFFu}); return weaponInfo.data(); }
inline CPlayerPed* __cdecl FakePlayer(int id) {
    trace.push_back({"find_player", id});
    return reinterpret_cast<CPlayerPed*>(D.noPlayer ? nullptr : D.playerIsAttacker ? attacker.data() : peds[0].data());
}
inline void __cdecl FakeGlass(uint32 x, uint32 y, uint32 z, uint32 radius) { trace.push_back({"break_glass", x, y, z, radius}); }
inline bool __fastcall FakeIsPlayer(CPed* self, void*) { trace.push_back({"is_player", Ptr(self)}); return D.attackerIsPlayer; }
inline bool __fastcall FakeIsAlive(CPed* self, void*) { const int i = PedIx(self); trace.push_back({"is_alive", Ptr(self)}); return i >= 0 && D.alive[i]; }
inline void __cdecl FakeFind(const CVector* point, uint32 radius, uint32 b2D, int16* outCount, uint32 maxCount, CEntity** out, uint32 f1, uint32 f2, uint32 f3, uint32 f4, uint32 f5) {
    trace.push_back({"find_objects", Ptr(point), Vec3(point), radius, b2D & 0xFFu, maxCount & 0xFFFFu, f1 & 0xFFu, f2 & 0xFFu, f3 & 0xFFu, f4 & 0xFFu, f5 & 0xFFu, D.numObjects});
    *outCount = int16(D.numObjects);
    for (uint32 i = 0; i < D.numObjects && i < maxCount; ++i) out[i] = reinterpret_cast<CEntity*>(objects[i].data());
}
inline void __fastcall FakeSetUp(CTaskSimpleFight* self, void*, uint32 radius) { trace.push_back({"fight_setup", Ptr(self), radius}); }
inline CVector* __fastcall FakeCentre(CEntity* self, void*, CVector* out) {
    int k = PedIx(self), base = 0;
    if (k < 0) { k = VehIx(self); base = N; }
    if (k < 0) { k = ObjIx(self); base = 2 * N; }
    trace.push_back({"bound_centre", Ptr(self)});
    for (int a = 0; a < 3; ++a) Put<uint32>(out, a * 4, k < 0 ? 0u : D.centre[base + k][a]);
    return out;
}
inline CColModel* __fastcall FakeAnimate(void* self, void*, RpClump* clump) {
    int k = -1;
    for (int i = 0; i <= N; ++i) if (clump == reinterpret_cast<RpClump*>(clumps[i].data())) k = i;
    trace.push_back({"ped_col_model", Ptr(self), Ptr(clump)});
    return reinterpret_cast<CColModel*>(k >= 0 && k < N ? pedCols[k].data() : nullptr);
}
inline void __fastcall FakeAllocate(CPlaceable* self, void*) {
    trace.push_back({"allocate_matrix", Ptr(self)});
    Put<uintptr_t>(self, 0x14, uintptr_t(matrices[2 * N].data()));
}
inline void __fastcall FakeUpdateMatrix(void* self, void*, CMatrix* matrix) { trace.push_back({"update_matrix", Ptr(self), Ptr(matrix)}); }
inline CColModel* __fastcall FakeGetCol(CEntity* self, void*) {
    int k = VehIx(self), base = 0;
    if (k < 0) { k = ObjIx(self); base = N; }
    trace.push_back({"get_col_model", Ptr(self)});
    return reinterpret_cast<CColModel*>(k < 0 ? nullptr : entityCols[base + k].data());
}
inline int __cdecl FakeProcess(const CMatrix* matA, CColModel* colA, const CMatrix* matB, CColModel* colB, void* points, void* lines, void* maxTouch, uint32 flag) {
    json a = json::array();
    for (int off : {0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38}) a.push_back(Get<uint32>(matA, off));
    int k = -1;
    for (int i = 0; i < 2 * N; ++i) if (colB == reinterpret_cast<CColModel*>(entityCols[i].data())) k = i;
    trace.push_back({"process_col_models", a, Get<uint32>(matA, 0x40), Get<uint8>(matA, 0x44), Ptr(colA), Ptr(matB), Ptr(colB), Ptr(points), Ptr(lines), Ptr(maxTouch), flag & 0xFFu});
    const int count = k < 0 ? 0 : D.processCount[k];
    uint32 s = D.seed * 977u + uint32(k) * 131u + 7u;
    auto* pt = reinterpret_cast<uint8*>(points);
    for (int a2 = 0; a2 < 3; ++a2) { Put<uint32>(pt, a2 * 4, StressBits(s)); Put<uint32>(pt, 0x10 + a2 * 4, StressBits(s)); }
    pt[0x23] = uint8(Lcg(s) >> 24);   // superficie B
    pt[0x24] = uint8(Lcg(s) >> 24);   // pieza B
    return count;
}
inline CPed* __fastcall FakeHitPed(CTaskSimpleFight* self, void*, CPed* creator, CPed* victim, CVector* p1, CVector* p2, uint32 component) {
    trace.push_back({"hit_ped", Ptr(self), Ptr(creator), Ptr(victim), Ptr(p1), Vec3(p1), Ptr(p2), Vec3(p2), component & 0xFFFFu});
    const int i = PedIx(victim);
    switch (i >= 0 ? D.hitPedMode[i] : 0u) {
    case 1: return victim;
    case 2: return reinterpret_cast<CPed*>(attacker.data());
    case 3: return reinterpret_cast<CPed*>(peds[(i + 1) % N].data());
    default: return nullptr;
    }
}
inline void __fastcall FakeHitObj(CTaskSimpleFight* self, void*, CPed* creator, CEntity* object, CVector* p1, CVector* p2, uint32 component, uint32 surface) {
    trace.push_back({"hit_obj", Ptr(self), Ptr(creator), Ptr(object), Ptr(p1), Vec3(p1), Ptr(p2), Vec3(p2), component & 0xFFFFu, surface & 0xFFu});
}
inline void __fastcall FakeHitCar(CTaskSimpleFight* self, void*, CPed* creator, CVehicle* vehicle, CVector* p1, CVector* p2, uint32 component, uint32 surface) {
    trace.push_back({"hit_car", Ptr(self), Ptr(creator), Ptr(vehicle), Ptr(p1), Vec3(p1), Ptr(p2), Vec3(p2), component & 0xFFFFu, surface & 0xFFu});
}
inline void* __cdecl FakeGroup() { trace.push_back({"global_group"}); return globalGroup.data(); }
inline CEvent* __fastcall FakeAdd(CEventGroup* group, void*, CEvent* event, uint32 valid) {
    const auto* b = reinterpret_cast<const uint8*>(event);
    trace.push_back({"group_add", Ptr(group), VtableName(Get<uint32>(b, 0)), Get<uint32>(b, 4), b[8], b[0xC], Get<uint16>(b, 0xE), Get<uint16>(b, 0x10), Ptr(Get<uint32>(b, 0x14)),
                     Get<uint32>(b, 0x18), Get<uint32>(b, 0x1C), Vec3(b + 0x20), valid & 0xFFu});
    return event;
}
inline CAnimBlendAssociation* __cdecl FakeAssociation(RpClump* clump, uint32 id) {
    trace.push_back({"association", Ptr(clump), id});
    return reinterpret_cast<CAnimBlendAssociation*>(D.assocFound ? otherAnim.data() : nullptr);
}

inline float F(uint32 b) { return FloatOfBits(b); }
inline uint32 B(float f) { return FloatBitsOf(f); }

inline uint32 Radius(uint32& s) {
    static constexpr float common[]{0.25f, 0.5f, 1.f, 1.5f, 2.f, 3.f, 0.75f};
    const uint32 r = Rnd(s, 8);
    if (r < 5) return B(common[Rnd(s, 7)]);
    if (r < 7) return B(float(Rnd(s, 4000)) * 0.001f);
    return StressBits(s);
}
inline uint32 Coord(uint32& s) {
    static constexpr float common[]{0.f, 1.5f, -3.25f, 100.f, 1000.5f, -1000.f, 7.f, -0.5f};
    const uint32 r = Rnd(s, 16);
    if (r < 11) return B(common[Rnd(s, 8)]);
    if (r < 12) return B(1.0e30f);
    if (r < 13) return B(-1.0e-30f);
    return StressBits(s);
}


// Escenarios dirigidos (1 de cada 24 casos): garantizan las ramas poco probables al azar (avance del golpe, animacion de combo 7, golpes a vehiculo/objeto, sonido, cristales).
inline void Force(unsigned c) {
    const unsigned mode = c % 24;
    if (mode == 0 || mode > 6) return;
    auto* table = reinterpret_cast<uint8*>(tableBase);
    auto setCombo = [&](int combo, int move, uint8 level) {
        D.combo = combo; D.move = move;
        Put<int8>(task.data(), 0x24, int8(combo));
        Put<int8>(task.data(), 0x25, int8(move));
        auto* r = table + (combo - 4) * 0x88;
        Put<uint32>(r, 0x30 + move * 4, B(1.f));
        r[0x50 + move] = level;
    };
    for (int a = 0; a < 3; ++a) Put<uint32>(posn.data(), a * 4, 0);
    for (int a = 0; a < 3; ++a) Put<uint32>(attackerMatrix.data(), 0x10 + a * 4, B(a == 1 ? 1.f : 0.f));
    Put<uint32>(modelCols[0].data(), 0x24, B(1.f));
    for (int i = 0; i < N; ++i) { Put<uintptr_t>(intelligence.data(), 0x130 + i * 4, 0); Put<uintptr_t>(intelligence.data(), 0xE0 + i * 4, 0); }
    D.pedMask = D.vehMask = 0;
    D.numObjects = 0;
    D.attackerIsPlayer = D.noPlayer = D.playerIsAttacker = D.hasAnim = false;
    D.weaponType = 0;
    Put<uint32>(attacker.data(), 0x5A0 + int32(int8(D.slot)) * 0x1C, 0);
    Put<uintptr_t>(task.data(), 0x1C, 0);
    auto pedZero = [&](float sx, float sy) {
        auto* p0 = peds[0].data();
        Put<uint16>(p0, 0x22, uint16(modelSlot[0]));
        D.alive[0] = false; D.collision[0] = false; D.inVehicle[0] = false;
        Put<uint32>(p0, 0x1C, Get<uint32>(p0, 0x1C) & ~1u);
        Put<uint32>(p0, 0x46C, Get<uint32>(p0, 0x46C) & ~0x100u);
        Put<uintptr_t>(intelligence.data(), 0x130, uintptr_t(p0));
        D.pedMask = 1;
        D.centre[0] = {B(1.f), 0u, B(1000.f)};
        Put<uint16>(pedColData[0].data(), 0, 1);
        auto* sp = pedSpheres[0].data();
        Put<uint32>(sp, 0, B(sx)); Put<uint32>(sp, 4, B(sy)); Put<uint32>(sp, 8, 0); Put<uint32>(sp, 0xC, B(0.5f));
    };
    switch (mode) {
    case 1: setCombo(5, 2, 5); pedZero(0.f, 1.5f); break;                                     // avanza el punto y acierta en la segunda pasada
    case 2:                                                                                  // combo 7, movimiento 1: animacion
        setCombo(7, 1, 0); pedZero(0.5f, 0.f);
        D.hasAnim = true; Put<uintptr_t>(task.data(), 0x1C, uintptr_t(anim.data()));
        D.hitPedMode[0] = (c / 24) & 1; D.assocFound = ((c / 48) & 1) != 0;
        break;
    case 3:                                                                                  // golpe a vehiculo
        setCombo(5, 0, 0);
        Put<uint16>(vehicles[0].data(), 0x22, uint16(modelSlot[0])); D.vehType[0] = 0; Put<uint32>(vehicles[0].data(), 0x590, 0);
        Put<uintptr_t>(vehicles[0].data(), 0x14, ((c / 24) & 1) ? uintptr_t(matrices[0].data()) : 0);
        Put<uintptr_t>(intelligence.data(), 0xE0, uintptr_t(vehicles[0].data())); D.vehMask = 1;
        D.centre[N] = {B(1.f), 0u, 0u}; D.processCount[0] = 2;
        break;
    case 4:                                                                                  // golpe a objeto (el atacante es el jugador; el segundo objeto no usa colision)
        setCombo(5, 0, 0);
        D.attackerIsPlayer = true; D.numObjects = 2;
        for (int i = 0; i < 2; ++i) {
            auto* o = objects[i].data();
            Put<uint16>(o, 0x22, uint16(modelSlot[0]));
            Put<uint32>(o, 0x1C, i == 0 ? (Get<uint32>(o, 0x1C) | 1u) : (Get<uint32>(o, 0x1C) & ~1u));
            Put<uintptr_t>(o, 0x14, 0);
            D.centre[2 * N + i] = {B(1.f), 0u, 0u};
            D.processCount[N + i] = 1;
        }
        break;
    case 5: setCombo(5, 0, 0); D.attackerIsPlayer = true; break;                              // nadie golpeado: evento de sonido
    default: setCombo(5, 0, 0); D.attackerIsPlayer = true; D.playerIsAttacker = true; D.weaponType = 5;   // cristales
        Put<uint32>(attacker.data(), 0x5A0 + int32(int8(D.slot)) * 0x1C, 5);
        break;
    }
}

inline void Prepare(unsigned c) {
    caseIndex = c;
    D = Desc{};
    D.seed = c;
    uint32 s = c * 2654435761u + 0x51F15Eu;
    auto fill = [&](auto& a) { for (auto& b : a) b = uint8(Lcg(s) >> 24); };
    fill(task); fill(attacker); fill(attackerMatrix); fill(intelligence); for (auto& a : peds) fill(a); for (auto& a : vehicles) fill(a); for (auto& a : objects) fill(a);
    for (auto& a : matrices) fill(a); for (auto& a : clumps) fill(a); for (auto& a : modelInfos) fill(a); for (auto& a : modelCols) fill(a); for (auto& a : entityCols) fill(a);
    for (auto& a : pedCols) fill(a); for (auto& a : pedColData) fill(a); for (auto& a : pedSpheres) fill(a); fill(anim); fill(otherAnim); fill(globalGroup); fill(posn); fill(weaponInfo);

    // tabla de combos (filas 0..12) con relleno determinista; radios y niveles de la fila elegida
    uint32 t = c * 7919u + 11u;
    auto* table = reinterpret_cast<uint8*>(tableBase);
    for (size_t i = 0; i < tableSize; ++i) table[i] = uint8(Lcg(t) >> 24);
    static constexpr int32 combos[]{4, 5, 6, 7, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 3, 0, -1};
    D.combo = combos[Rnd(s, 18)];
    D.move = int32(Rnd(s, 6)) - 1;
    if (D.combo == 7 && Rnd(s, 2)) D.move = 1;
    const int row = D.combo - 4;
    static constexpr uint8 levels[]{0, 1, 3, 4, 5, 7, 0xFF, 4, 4, 5};
    uint32 hitRadius = 0;
    if (row >= 0 && row <= 12) {
        auto* r = table + row * 0x88;
        for (int k = 0; k < 7; ++k) Put<uint32>(r, 0x2C + k * 4, Radius(s));
        for (int k = 0; k < 6; ++k) r[0x4F + k] = levels[Rnd(s, 10)];
        hitRadius = Get<uint32>(r, 0x30 + D.move * 4);
    }
    // tarea
    Put<int8>(task.data(), 0x24, int8(D.combo));
    Put<int8>(task.data(), 0x25, int8(D.move));
    D.hasAnim = Rnd(s, 3) != 0;
    Put<uintptr_t>(task.data(), 0x1C, D.hasAnim ? uintptr_t(anim.data()) : 0);
    Put<uintptr_t>(task.data(), 0x18, 0);
    // atacante
    static constexpr int32 slots[]{0, 1, 3, 12, 0xFF, 0, 2};
    D.slot = slots[Rnd(s, 7)];
    D.weaponType = Rnd(s, 3) == 0 ? 0u : 1u + Rnd(s, 46);
    D.attackerIsPlayer = Rnd(s, 2) != 0;
    D.playerIsAttacker = Rnd(s, 3) != 0;
    D.noPlayer = Rnd(s, 8) == 0;
    D.assocFound = Rnd(s, 2) != 0;
    Put<uint8>(attacker.data(), 0x718, uint8(D.slot));
    for (int k = -1; k <= 12; ++k) Put<uint32>(attacker.data(), 0x5A0 + k * 0x1C, 1u + Rnd(s, 46));
    Put<uint32>(attacker.data(), 0x5A0 + int32(int8(D.slot)) * 0x1C, D.weaponType);
    Put<uintptr_t>(attacker.data(), 0x47C, uintptr_t(intelligence.data()));
    Put<uintptr_t>(attacker.data(), 0x14, uintptr_t(attackerMatrix.data()));
    Put<uintptr_t>(attacker.data(), 0x18, uintptr_t(clumps[N].data()));
    static constexpr float forwards[][3]{{0.f, 1.f, 0.f}, {1.f, 0.f, 0.f}, {0.6f, 0.8f, 0.f}, {0.f, 0.f, 1.f}, {-0.6f, 0.f, 0.8f}};
    const auto& fw = forwards[Rnd(s, 5)];
    for (int a = 0; a < 3; ++a) Put<uint32>(attackerMatrix.data(), 0x10 + a * 4, Rnd(s, 12) == 0 ? StressBits(s) : B(fw[a]));
    for (int a = 0; a < 3; ++a) { Put<uint32>(attackerMatrix.data(), a * 4, Coord(s)); Put<uint32>(attackerMatrix.data(), 0x20 + a * 4, Coord(s)); Put<uint32>(attackerMatrix.data(), 0x30 + a * 4, Coord(s)); }
    Put<uint32>(attackerMatrix.data(), 0x40, 0); Put<uint32>(attackerMatrix.data(), 0x44, 0);
    // posicion del golpe
    for (int a = 0; a < 3; ++a) Put<uint32>(posn.data(), a * 4, Coord(s));
    // modelos (las entradas reales de la tabla se rellenan en Run)
    std::array<float, 8> boundRadius{};
    for (int k = 0; k < 8; ++k) {
        const uint32 rb = Radius(s);
        Put<uintptr_t>(modelInfos[k].data(), 0x14, uintptr_t(modelCols[k].data()));
        Put<uint32>(modelCols[k].data(), 0x24, rb);
        boundRadius[k] = F(rb);
    }
    // candidatos
    D.pedMask = (Lcg(s) >> 8) & (Lcg(s) >> 11) & 0xFFFF;
    D.vehMask = (Lcg(s) >> 8) & (Lcg(s) >> 11) & 0xFFFF;
    D.numObjects = Rnd(s, 3) == 0 ? 0u : Rnd(s, 17);
    auto place = [&](int slotIndex, bool threeD, float bonus, int k) {
        // distancia al punto de golpe relativa al alcance aproximado
        const float reach = boundRadius[k] + F(hitRadius) + bonus;
        static constexpr float factors[]{0.25f, 0.9f, 0.99999994f, 1.f, 1.0000001f, 1.5f, 3.f, 0.5f};
        const float d = reach * factors[Rnd(s, 8)];
        for (int a = 0; a < 3; ++a) D.centre[slotIndex][a] = Get<uint32>(posn.data(), a * 4);
        if (Rnd(s, 10) == 0) { for (int a = 0; a < 3; ++a) D.centre[slotIndex][a] = Coord(s); return; }
        D.centre[slotIndex][0] = B(F(D.centre[slotIndex][0]) + d);
        if (threeD && Rnd(s, 2)) D.centre[slotIndex][2] = B(F(D.centre[slotIndex][2]) + d * 0.5f);
        if (!threeD) D.centre[slotIndex][2] = B(F(D.centre[slotIndex][2]) + 1000.f);    // el plano ignora la altura
    };
    Put<uintptr_t>(intelligence.data(), 0, 0);
    for (int i = 0; i < N; ++i) {
        // peds
        auto* p = peds[i].data();
        const int k = int(Rnd(s, 8));
        Put<uint16>(p, 0x22, uint16(modelSlot[k]));
        Put<uintptr_t>(p, 0x18, uintptr_t(clumps[i].data()));
        D.alive[i] = Rnd(s, 4) != 0;
        D.collision[i] = Rnd(s, 2) != 0;
        D.inVehicle[i] = Rnd(s, 4) == 0;
        D.pedVehicle[i] = Rnd(s, 5) == 0 ? -1 : int(Rnd(s, N));
        D.hitPedMode[i] = Rnd(s, 4);
        Put<uint32>(p, 0x1C, (Get<uint32>(p, 0x1C) & ~1u) | (D.collision[i] ? 1u : 0u));
        Put<uint32>(p, 0x46C, (Get<uint32>(p, 0x46C) & ~0x100u) | (D.inVehicle[i] ? 0x100u : 0u));
        Put<uintptr_t>(p, 0x58C, D.pedVehicle[i] < 0 ? 0 : uintptr_t(vehicles[D.pedVehicle[i]].data()));
        Put<uintptr_t>(intelligence.data(), 0x130 + i * 4, (D.pedMask >> i) & 1 ? uintptr_t(p) : 0);
        place(i, false, F(hitRadius) * 0.5f, k);
        // esferas del ped
        const uint32 n = Rnd(s, 5);
        Put<uint16>(pedColData[i].data(), 0, uint16(n));
        Put<uintptr_t>(pedColData[i].data(), 8, uintptr_t(pedSpheres[i].data()));
        Put<uintptr_t>(pedCols[i].data(), 0x2C, uintptr_t(pedColData[i].data()));
        for (uint32 j = 0; j < 4; ++j) {
            auto* sp = pedSpheres[i].data() + j * 0x14;
            const float rs = Rnd(s, 3) == 0 ? 0.25f : 0.5f;
            static constexpr float factors[]{0.1f, 0.9f, 0.99999994f, 1.f, 1.0000001f, 2.f, 4.f};
            const float d = (rs + F(hitRadius)) * factors[Rnd(s, 7)];
            Put<uint32>(sp, 0xC, B(rs));
            for (int a = 0; a < 3; ++a) Put<uint32>(sp, a * 4, Get<uint32>(posn.data(), a * 4));
            Put<uint32>(sp, Rnd(s, 3) * 4, B(F(Get<uint32>(sp, 0)) + d));
            if (Rnd(s, 14) == 0) for (int a = 0; a < 3; ++a) Put<uint32>(sp, a * 4, StressBits(s));
        }
        // vehiculos
        auto* v = vehicles[i].data();
        const int kv = int(Rnd(s, 8));
        Put<uint16>(v, 0x22, uint16(modelSlot[kv]));
        D.vehType[i] = Rnd(s, 2) ? 0u : (Rnd(s, 2) ? 9u : Rnd(s, 12));
        Put<uint32>(v, 0x590, D.vehType[i]);
        Put<uintptr_t>(v, 0x14, Rnd(s, 4) == 0 ? 0 : uintptr_t(matrices[i].data()));
        Put<uintptr_t>(intelligence.data(), 0xE0 + i * 4, (D.vehMask >> i) & 1 ? uintptr_t(v) : 0);
        place(N + i, true, 0.f, kv);
        D.processCount[i] = Rnd(s, 4) == 0 ? 0 : Rnd(s, 8) == 0 ? -1 : int(1 + Rnd(s, 3));
        // objetos
        auto* o = objects[i].data();
        const int ko = int(Rnd(s, 8));
        Put<uint16>(o, 0x22, uint16(modelSlot[ko]));
        Put<uint32>(o, 0x1C, (Get<uint32>(o, 0x1C) & ~1u) | (Rnd(s, 4) != 0 ? 1u : 0u));
        Put<uintptr_t>(o, 0x14, Rnd(s, 4) == 0 ? 0 : uintptr_t(matrices[N + i].data()));
        place(2 * N + i, true, 0.f, ko);
        D.processCount[N + i] = Rnd(s, 4) == 0 ? 0 : Rnd(s, 8) == 0 ? -1 : int(1 + Rnd(s, 3));
    }
    Put<uint32>(anim.data(), 0x1C, 0x3F800000u);
    Force(c);
}

inline json One(unsigned c, bool candidate, OriginalDependencies499::HookRoute& route) {
    Prepare(c);
    trace = json::array();
    std::vector<std::vector<uint8>> before;
    auto keep = [&](const auto& a) { before.push_back(Snapshot(a.data(), a.size())); };
    keep(task); keep(attacker); keep(attackerMatrix); keep(intelligence); keep(anim); keep(otherAnim); keep(posn);
    for (auto& a : peds) keep(a);
    for (auto& a : vehicles) keep(a);
    for (auto& a : objects) keep(a);
    for (auto& a : matrices) keep(a);
    // Estado inicial fijo de CWorld::m_aTempColPts[0]: el falso de ProcessColModels lo reescribe con valores que dependen solo del caso;
    // sin esto la pasada candidata (que corre justo despues de la original con el mismo caso) parte de los valores ya escritos por la original
    // y ChangedWords no ve cambios, aunque la memoria final sea identica.
    memset(reinterpret_cast<void*>(0xB9ACD0), 0x5A, 0x2C);
    const auto pointsBefore = Snapshot(reinterpret_cast<void*>(0xB9ACD0), 0x2C);
    const auto guardLow = Snapshot(reinterpret_cast<void*>(tableBase - 0x40), 0x40), guardHigh = Snapshot(reinterpret_cast<void*>(tableBase + tableSize), 0x40);
    Abi abi{};
    bool routeOk = false;
    route.Invoke(candidate, [&] {
        routeOk = ReversingProbe::JumpsToDll(0x6240B0) == candidate;
        abi = CallStrike(0x6240B0, task.data(), attacker.data(), posn.data());
        return 0;
    });
    json v = AbiJson(abi);
    v["route_ok"] = routeOk;
    v["trace"] = trace;
    size_t at = 0;
    auto changes = [&](const auto& a) { return ChangedWords(before[at++], Snapshot(a.data(), a.size())); };
    json ch = json::object();
    ch["task"] = changes(task); ch["attacker"] = changes(attacker); ch["attacker_matrix"] = changes(attackerMatrix); const auto intelChanges = changes(intelligence);
    ch["anim"] = changes(anim); ch["other_anim"] = changes(otherAnim); ch["posn"] = changes(posn);
    json pedCh = json::array(), vehCh = json::array(), objCh = json::array(), matCh = json::array();
    for (auto& a : peds) pedCh.push_back(changes(a));
    for (auto& a : vehicles) vehCh.push_back(changes(a));
    for (auto& a : objects) objCh.push_back(changes(a));
    for (auto& a : matrices) matCh.push_back(changes(a));
    ch["peds"] = pedCh; ch["vehicles"] = vehCh; ch["objects"] = objCh; ch["matrices"] = matCh;
    ch["temp_col_points"] = ChangedWords(pointsBefore, Snapshot(reinterpret_cast<void*>(0xB9ACD0), 0x2C));
    v["changes"] = ch;
    v["guards_intact"] = guardLow == Snapshot(reinterpret_cast<void*>(tableBase - 0x40), 0x40) && guardHigh == Snapshot(reinterpret_cast<void*>(tableBase + tableSize), 0x40);
    v["inputs_intact"] = intelChanges.empty();   // la inteligencia y las tablas del atacante solo se leen
    return v;
}

inline void Run(int& failures) {
    const Fn& fn = fns[2];
    OriginalDependencies499::HookRoute route(fn.cls, fn.name, fn.address, FunctionToVoidPtr(&CTaskSimpleFight::FightStrike));
    if (!route.Registered()) { Write({{"event", "error"}, {"reason", "hook FightStrike no registrado"}}); allPassed = false; return; }
    // entradas libres de la tabla de modelos (se restauran al terminar)
    {
        int found = 0;
        for (int i = int(std::size(CModelInfo::ms_modelInfoPtrs)) - 1; i >= 0 && found < 8; --i) if (!CModelInfo::ms_modelInfoPtrs[i]) modelSlot[found++] = i;
        if (found < 8) { Write({{"event", "error"}, {"reason", "sin entradas libres en la tabla de modelos"}}); allPassed = false; return; }
    }
    regions.clear();
    static std::array<std::string, 256> names;
    size_t nameIx = 0;
    auto reg = [&](const std::string& n, const void* p, size_t sz) { names[nameIx] = n; Reg(names[nameIx].c_str(), p, sz); ++nameIx; };
    reg("task", task.data(), task.size()); reg("attacker", attacker.data(), attacker.size()); reg("attacker_matrix", attackerMatrix.data(), attackerMatrix.size());
    reg("intelligence", intelligence.data(), intelligence.size()); reg("posn", posn.data(), posn.size()); reg("anim", anim.data(), anim.size()); reg("other_anim", otherAnim.data(), otherAnim.size());
    reg("global_group", globalGroup.data(), globalGroup.size()); reg("weapon_info", weaponInfo.data(), weaponInfo.size());
    for (int i = 0; i < N; ++i) {
        reg("ped" + std::to_string(i), peds[i].data(), peds[i].size()); reg("vehicle" + std::to_string(i), vehicles[i].data(), vehicles[i].size());
        reg("object" + std::to_string(i), objects[i].data(), objects[i].size()); reg("matrix" + std::to_string(i), matrices[i].data(), matrices[i].size());
        reg("matrix" + std::to_string(N + i), matrices[N + i].data(), matrices[N + i].size());
        reg("vehicle_col" + std::to_string(i), entityCols[i].data(), entityCols[i].size()); reg("object_col" + std::to_string(i), entityCols[N + i].data(), entityCols[N + i].size());
        reg("ped_col" + std::to_string(i), pedCols[i].data(), pedCols[i].size());
    }
    for (int i = 0; i <= N; ++i) reg("clump" + std::to_string(i), clumps[i].data(), clumps[i].size());
    for (int i = 0; i < 8; ++i) reg("model" + std::to_string(i), modelInfos[i].data(), modelInfos[i].size());
    reg("matrix" + std::to_string(2 * N), matrices[2 * N].data(), matrices[2 * N].size());
    reg("static_col", reinterpret_cast<void*>(0xC17824), 0x30); reg("temp_col_points", reinterpret_cast<void*>(0xB9ACD0), 32 * 0x2C);
    const auto savedTable = Snapshot(reinterpret_cast<void*>(tableBase - 0x40), tableSize + 0x80);
    const auto savedPoints = Snapshot(reinterpret_cast<void*>(0xB9ACD0), 0x2C);
    for (int k = 0; k < 8; ++k) { savedModelPtr[k] = CModelInfo::ms_modelInfoPtrs[modelSlot[k]]; CModelInfo::ms_modelInfoPtrs[modelSlot[k]] = reinterpret_cast<CBaseModelInfo*>(modelInfos[k].data()); }
    {
        FakeExe hitPed(0x61CBA0, FakeHitPed);
        FakeExe hitObj(0x61D400, FakeHitObj);
        Fake hitCar(0x61D0B0, Addr(&CTaskSimpleFight::FightHitCar), FakeHitCar);
        Fake setup(0x61D5F0, Addr(&CTaskSimpleFight::FightSetUpCol), FakeSetUp);
        Fake info(0x743C60, reinterpret_cast<uintptr_t>(static_cast<CWeaponInfo* (*)(eWeaponType, eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)), FakeWeaponInfo);
        Fake player(0x56E210, reinterpret_cast<uintptr_t>(&FindPlayerPed), FakePlayer);
        Fake glass(0x71CF50, reinterpret_cast<uintptr_t>(&CGlass::BreakGlassPhysically), FakeGlass);
        Fake isPlayer(0x5DF8F0, Addr(&CPed::IsPlayer), FakeIsPlayer);
        Fake isAlive(0x5E0170, Addr(&CPed::IsAlive), FakeIsAlive);
        Fake find(0x564A20, reinterpret_cast<uintptr_t>(&CWorld::FindObjectsInRange), FakeFind);
        Fake centre(0x534250, Addr(static_cast<CVector (CEntity::*)() const>(&CEntity::GetBoundCentre)), FakeCentre);
        Fake animate(0x4C7170, Addr(&CPedModelInfo::AnimatePedColModelSkinnedWorld), FakeAnimate);
        Fake allocate(0x54F560, Addr(&CPlaceable::AllocateMatrix), FakeAllocate);
        Fake update(0x54F1B0, Addr(&CSimpleTransform::UpdateMatrix), FakeUpdateMatrix);
        Fake getCol(0x535300, Addr(&CEntity::GetColModel), FakeGetCol);
        Fake process(0x4185C0, reinterpret_cast<uintptr_t>(&CCollision::ProcessColModels), FakeProcess);
        Fake group(0x4ABA50, reinterpret_cast<uintptr_t>(&GetEventGlobalGroup), FakeGroup);
        Fake add(0x4AB420, Addr(static_cast<CEvent* (CEventGroup::*)(CEvent*, bool)>(&CEventGroup::Add)), FakeAdd);
        Fake association(0x4D68B0, reinterpret_cast<uintptr_t>(static_cast<CAnimBlendAssociation* (*)(RpClump*, uint32)>(&RpAnimBlendClumpGetAssociation)), FakeAssociation);
        Fake referenceFake(0x571B70, Addr(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::RegisterReference)), HitCar::FakeRegister);
        Fake cleanFake(0x571A00, Addr(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)), HitCar::FakeCleanUp);
        for (unsigned c = 0; c < unsigned(fn.cases); ++c) {
            const auto original = One(c, false, route);
            const auto candidate = One(c, true, route);
            Report(fn, c, original, candidate, failures);
            // cobertura: trazas del candidato y fixtures
            std::map<std::string, int> seen;
            for (const auto& e : candidate["trace"]) ++seen[e[0].get<std::string>()];
            for (const char* tag : {"break_glass", "find_objects", "hit_ped", "hit_obj", "hit_car", "allocate_matrix", "update_matrix", "global_group", "group_add", "association", "is_alive", "process_col_models", "bound_centre", "ped_col_model"})
                if (seen[tag]) Cover((std::string("strike.") + tag).c_str());
            if (seen["process_col_models"] && seen["fight_setup"] == 1) Cover("strike.matrix_candidates");
            const auto changesPosn = candidate["changes"]["posn"];
            if (!changesPosn.empty()) Cover("strike.position_advanced");
            if (!candidate["changes"]["anim"].empty()) Cover("strike.anim_blended_out");
            if (!candidate["changes"]["attacker"].empty()) Cover("strike.hit_position_stored");
            if (seen["is_player"] == 2) Cover("strike.player_twice");
        }
    }
    for (int k = 0; k < 8; ++k) CModelInfo::ms_modelInfoPtrs[modelSlot[k]] = savedModelPtr[k];
    memcpy(reinterpret_cast<void*>(tableBase - 0x40), savedTable.data(), savedTable.size());
    memcpy(reinterpret_cast<void*>(0xB9ACD0), savedPoints.data(), savedPoints.size());
    memoryRestored &= savedTable == Snapshot(reinterpret_cast<void*>(tableBase - 0x40), tableSize + 0x80) && savedPoints == Snapshot(reinterpret_cast<void*>(0xB9ACD0), 0x2C);
    for (int k = 0; k < 8; ++k) memoryRestored &= CModelInfo::ms_modelInfoPtrs[modelSlot[k]] == savedModelPtr[k];
    hooksRestored &= route.Restore();
}
}

// ================================================================ ejecucion
// Ramas que el banco exige haber recorrido (no se relaja ninguna): cada etiqueta se cuenta sobre los pares del candidato.
inline constexpr std::array<const char*, 33> requiredCoverage{
    "setup.initialised_sphere", "setup.foreign_sphere", "setup.snan", "setup.qnan", "setup.denormal",
    "hitcar.chainsaw", "hitcar.other_weapon", "hitcar.driver", "hitcar.passengers", "hitcar.weapon_swapped_after_damage", "hitcar.bone_index_minus_one",
    "hitcar.last_damage_registered", "hitcar.chainsaw_sound", "hitcar.nan_health", "hitcar.nan_damage_or_mass",
    "strike.break_glass", "strike.find_objects", "strike.hit_ped", "strike.hit_obj", "strike.hit_car", "strike.allocate_matrix", "strike.update_matrix",
    "strike.global_group", "strike.group_add", "strike.association", "strike.is_alive", "strike.process_col_models", "strike.bound_centre", "strike.ped_col_model",
    "strike.position_advanced", "strike.anim_blended_out", "strike.hit_position_stored", "strike.player_twice"};

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies21c-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "dependencies21c-runtime.jsonl");
        Write({{"event", "waiting_for_player"}});
        ReversingProbe::InstallFailureCapture("dependencies21c");
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    Write({{"event", "begin"}, {"functions", json::array({"CTaskSimpleFight::FightSetUpCol", "CTaskSimpleFight::FightHitCar", "CTaskSimpleFight::FightStrike"})},
           {"scope", "colaboradores falsos con EntryPatch en EXE y DLL; trazas, memoria, ESP, no volatiles, SEH y x87 (TOP/FTW/FCW); EAX/ECX/EDX no los lee ningun llamador"}});
    int failures[3]{};
    SetUp::Run(failures[0]);
    Write({{"event", "function_summary"}, {"function", "CTaskSimpleFight::FightSetUpCol"}, {"cases", fns[0].cases}, {"failures", failures[0]}});
    HitCar::Run(failures[1]);
    Write({{"event", "function_summary"}, {"function", "CTaskSimpleFight::FightHitCar"}, {"cases", fns[1].cases}, {"failures", failures[1]}});
    Strike::Run(failures[2]);
    Write({{"event", "function_summary"}, {"function", "CTaskSimpleFight::FightStrike"}, {"cases", fns[2].cases}, {"failures", failures[2]}});
    json cover = json::object();
    for (const char* tag : requiredCoverage) {
        cover[tag] = coverage[tag];
        if (!coverage[tag]) { allPassed = false; Write({{"event", "error"}, {"reason", "rama sin cubrir"}, {"branch", tag}}); }
    }
    const bool restored = memoryRestored && hooksRestored;
    Write({{"event", allPassed && restored ? "complete" : "error"}, {"pairs", pairCounter}, {"state_restored", restored}, {"memory_restored", memoryRestored}, {"hooks_restored", hooksRestored}, {"coverage", cover}});
}
}
