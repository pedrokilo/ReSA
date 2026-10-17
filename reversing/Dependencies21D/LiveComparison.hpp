#pragma once
// Banco Dependencies21D: CTaskSimpleGangDriveBy::Constructor (0x6217D0), PlayerTarget (0x621960), AITarget (0x627600) y AimGun (0x628350).
// Cada caso ejecuta la MISMA entrada del EXE con el hook RH apagado (bytes originales) y activo (version de la DLL), con los mismos colaboradores
// falsos instalados en la direccion del EXE y en la entrada de la DLL (EntryPatch). Ver README.md.
#include <array>
#include <bit>
#include <cstring>
#include <fstream>
#include <functional>
#include <memory>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "TaskSimpleGangDriveBy.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Animation/AnimBlendAssociation.h"
#include "WeaponInfo.h"
#include "PlayerInfo.h"
#include "PlayerPedData.h"
#include "Camera.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "GameLogic.h"
#include "General.h"
#include "PedIK.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace Dependencies21DLive {
inline bool done{};
inline std::ofstream log;

struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline constexpr Fn fns[]{
    {"CTaskSimpleGangDriveBy", "Constructor",  0x6217D0, 1024},
    {"CTaskSimpleGangDriveBy", "PlayerTarget", 0x621960, 2048},
    {"CTaskSimpleGangDriveBy", "AITarget",     0x627600, 4096},
    {"CTaskSimpleGangDriveBy", "AimGun",       0x628350, 4096},
};
enum FnIndex { kCtor, kPlayerTarget, kAiTarget, kAimGun };

// ------------------------------------------------------------------ rutas y modulos
inline uintptr_t Resolve(uintptr_t p) { for (int i = 0; i < 16 && *reinterpret_cast<uint8*>(p) == 0xE9; ++i) p += 5 + *reinterpret_cast<int32*>(p + 1); return p; }
inline HMODULE ModuleOf(uintptr_t p) { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(p), &m); return m; }
inline void ModuleMarker() {}

// ------------------------------------------------------------------ estado del coma flotante (como DependenciesTaskSprint16)
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

// ------------------------------------------------------------------ generador y arena
struct Rng {
    uint64 s{88172645463325252ull};
    explicit Rng(uint64 seed = 1) : s(88172645463325252ull ^ (seed * 0x9E3779B97F4A7C15ull)) { for (int i = 0; i < 4; ++i) Next(); }
    uint32 Next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return uint32(s >> 16); }
    uint32 N(uint32 n) { return Next() % n; }
};
inline Rng rng;
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline float FromBits(uint32 b) { return std::bit_cast<float>(b); }
inline float FRange(float lo, float hi) { return lo + (hi - lo) * (float(rng.N(1000001)) / 1000000.0f); }
inline constexpr uint32 kPool[]{
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x40490FDB, 0xC0490FDB, 0x40C90FDB, 0x3FC90FDB, 0x3F490FDB, 0xBF490FDB, 0x4016CBE4, 0xC016CBE4,
    0x7149F2CA, 0xF149F2CA, 0x7F7FFFFF, 0xFF7FFFFF, 0x0DA24260, 0x8DA24260, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x7F800000, 0xFF800000,
    0x7FC00000, 0xFFC00000, 0x7FC12345, 0xFFC54321, 0x7FA00000, 0xFFA00001, 0x7F800001, 0x41A00000, 0x42C80000, 0x3E860A92, 0xBE860A92, 0x40384E89, 0xC0384E89,
    0x3FA78D36, 0xBFA78D36, 0x3FEA9280, 0xBFEA9280, 0x3F333333, 0x358637BD, 0x3F7FFFFF, 0x3F800001, 0xBF7FFFFF, 0xBF800001, 0x3F800010, 0x4B000000, 0x5F000000};
// ~22% valores de estres (NaN con carga distinta y senalizantes, +-inf, +-0, subnormales, +-1e30, FLT_MAX, umbrales +-1 ulp), resto normales acotados.
inline uint32 RndBits(int range) {
    if (rng.N(100) < 22) {
        const uint32 b = kPool[rng.N(sizeof(kPool) / sizeof(kPool[0]))];
        return rng.N(4) == 0 ? b + uint32(int(rng.N(5)) - 2) : b;
    }
    return Bits((float(rng.N(2000001)) / 1000000.0f - 1.0f) * float(range));
}

inline constexpr size_t Align16(size_t n) { return (n + 15) & ~size_t(15); }
inline constexpr size_t kGuard = 16;
// Una sola arena con guardas de 16 bytes (0xA5) entre objetos; todos los tamanos son los del EXE (ped 0x7A0, vehiculo 0xA20, entidad 0xA40...).
struct Layout {
    static constexpr size_t task = kGuard, taskSize = Align16(0x44);
    static constexpr size_t ped = task + taskSize + kGuard, pedSize = Align16(0x7A0);
    static constexpr size_t veh = ped + pedSize + kGuard, vehSize = Align16(0xA20);
    static constexpr size_t data = veh + vehSize + kGuard, dataSize = Align16(sizeof(CPlayerPedData));
    static constexpr size_t info = data + dataSize + kGuard, infoSize = Align16(sizeof(CPlayerInfo));
    static constexpr size_t tgt = info + infoSize + kGuard, tgtSize = Align16(0xA40);
    static constexpr size_t wi = tgt + tgtSize + kGuard, wiSize = Align16(0x80);
    static constexpr size_t assoc = wi + wiSize + kGuard, assocSize = Align16(0x40);
    static constexpr size_t matPed = assoc + assocSize + kGuard, matSize = Align16(0x48);
    static constexpr size_t matVeh = matPed + matSize + kGuard;
    static constexpr size_t matTgt = matVeh + matSize + kGuard;
    static constexpr size_t matAlloc = matTgt + matSize + kGuard;
    static constexpr size_t vecOut = matAlloc + matSize + kGuard, vecSize = Align16(0x40);
    static constexpr size_t total = vecOut + vecSize + kGuard;
};
static_assert(sizeof(CPed) <= 0x7A0 && sizeof(CPlayerPedData) == 0xAC && sizeof(CPlayerInfo) == 0x190 && sizeof(CTaskSimpleGangDriveBy) == 0x44);
alignas(16) inline std::array<uint8, Layout::total> arena{};
inline uint8* At(size_t offset) { return arena.data() + offset; }
inline uint8* Task() { return At(Layout::task); }
inline uint8* Ped() { return At(Layout::ped); }
inline uint8* Veh() { return At(Layout::veh); }
inline uint8* Data() { return At(Layout::data); }
inline uint8* Info() { return At(Layout::info); }
inline uint8* Tgt() { return At(Layout::tgt); }
inline uint8* Wi() { return At(Layout::wi); }
inline uint8* Assoc() { return At(Layout::assoc); }
inline uint8* MatPed() { return At(Layout::matPed); }
inline uint8* MatVeh() { return At(Layout::matVeh); }
inline uint8* MatTgt() { return At(Layout::matTgt); }
inline uint8* MatAlloc() { return At(Layout::matAlloc); }
inline uint8* VecOut() { return At(Layout::vecOut); }
inline CCamera& Camera() { return TheCamera; }
template <class T> inline void Put(void* base, size_t off, T v) { std::memcpy(static_cast<uint8*>(base) + off, &v, sizeof(v)); }
template <class T> inline T Get(const void* base, size_t off) { T v; std::memcpy(&v, static_cast<const uint8*>(base) + off, sizeof(v)); return v; }
inline void PutF(void* base, size_t off, uint32 bits) { Put<uint32>(base, off, bits); }
inline void FillRandom(void* p, size_t n) { auto* b = static_cast<uint8*>(p); for (size_t i = 0; i < n; ++i) b[i] = uint8(rng.Next()); }
inline uint32 Pick(std::initializer_list<uint32> l) { return *(l.begin() + rng.N(uint32(l.size()))); }

inline json Ptr(const void* p) {
    const auto u = reinterpret_cast<uintptr_t>(p);
    if (!u) return nullptr;
#define TAG(name, off, size) if (u >= uintptr_t(arena.data() + off) && u < uintptr_t(arena.data() + off + size)) return json::array({name, u - uintptr_t(arena.data() + off)});
    TAG("task", Layout::task, Layout::taskSize) TAG("ped", Layout::ped, Layout::pedSize) TAG("veh", Layout::veh, Layout::vehSize) TAG("data", Layout::data, Layout::dataSize)
    TAG("info", Layout::info, Layout::infoSize) TAG("tgt", Layout::tgt, Layout::tgtSize) TAG("wi", Layout::wi, Layout::wiSize) TAG("assoc", Layout::assoc, Layout::assocSize)
    TAG("matPed", Layout::matPed, Layout::matSize) TAG("matVeh", Layout::matVeh, Layout::matSize) TAG("matTgt", Layout::matTgt, Layout::matSize)
    TAG("matAlloc", Layout::matAlloc, Layout::matSize) TAG("vecOut", Layout::vecOut, Layout::vecSize)
#undef TAG
    if (u >= reinterpret_cast<uintptr_t>(&TheCamera) && u < reinterpret_cast<uintptr_t>(&TheCamera) + sizeof(CCamera)) return json::array({"camera", u - reinterpret_cast<uintptr_t>(&TheCamera)});
    { // la pila del hilo puede caer dentro de 0x400000..0x1000000: sus locales (p. ej. la salida de GetBonePosition) cambian de direccion entre el EXE y la DLL
        const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
        if (u >= reinterpret_cast<uintptr_t>(tib->StackLimit) && u < reinterpret_cast<uintptr_t>(tib->StackBase)) return "local";
    }
    if (u >= 0x400000 && u < 0x1000000) return json::array({"exe", u});
    return "local"; // pila del llamador: la direccion cambia entre ejecuciones
}

// ------------------------------------------------------------------ configuracion de los colaboradores falsos
struct Cfg {
    bool isPlayer{}, coop{}, los{};
    uint32 twoVec[3]{}, srcOut[3]{}, tgtOut[3]{}, bone[2][3]{}, radian[4]{};
    int radianCalls{};
};
inline Cfg G;
inline json trace = json::array();
inline int* ExeErrno() { return reinterpret_cast<int*(__cdecl*)()>(0x82B181)(); }

// Colaboradores falsos; los mismos para la direccion del EXE y la entrada de la DLL. Los argumentos float se reciben como bits (el EXE los empuja con mov enteros).
inline bool __fastcall FIsPlayer(CPed* ped, void*) { trace.push_back({"is_player", Ptr(ped), G.isPlayer}); return G.isPlayer; }
inline bool __cdecl FIsCoop() { trace.push_back({"is_coop", G.coop}); return G.coop; }
inline void __fastcall FSetMode(CCamera* cam, void*, int mode, int a, int b) { trace.push_back({"set_new_player_weapon_mode", Ptr(cam), mode, a & 0xFFFF, b & 0xFFFF}); }
inline CEntity* __fastcall FTwoPlayer(CCam* cam, void*, CVector* v) {
    trace.push_back({"two_player_aim", Ptr(cam), Bits(v->x), Bits(v->y), Bits(v->z)});
    v->x = FromBits(G.twoVec[0]); v->y = FromBits(G.twoVec[1]); v->z = FromBits(G.twoVec[2]);
    return nullptr;
}
inline CPlayerInfo* __fastcall FPlayerInfo(CPlayerPed* pp, void*) { trace.push_back({"player_info", Ptr(pp)}); return reinterpret_cast<CPlayerInfo*>(Info()); }
inline void __fastcall FFind3rd(CCamera* cam, void*, uint32 range, CVector muzzle, CVector* src, CVector* tgt) {
    trace.push_back({"find_3rd_person_cam_target_vector", Ptr(cam), range, Bits(muzzle.x), Bits(muzzle.y), Bits(muzzle.z), Ptr(src), Ptr(tgt)});
    std::memcpy(src, G.srcOut, 12); std::memcpy(tgt, G.tgtOut, 12);
}
inline void __fastcall FBone(CPed* ped, void*, CVector* out, int bone, int upd) {
    trace.push_back({"get_bone_position", Ptr(ped), Ptr(out), bone & 0xFFFF, upd & 0xFF, Bits(out->x), Bits(out->y), Bits(out->z)});
    std::memcpy(out, G.bone[(bone & 0xFFFF) == 3 ? 0 : 1], 12);
}
inline float __cdecl FRadian(uint32 x1, uint32 y1, uint32 x2, uint32 y2) {
    trace.push_back({"radian_angle", x1, y1, x2, y2});
    return FromBits(G.radian[(G.radianCalls++) & 3]);
}
inline bool __fastcall FLos(CTaskSimpleGangDriveBy* task, void*, CPed* ped, CVector* v) {
    trace.push_back({"line_of_sight", Ptr(task), Ptr(ped), Bits(v->x), Bits(v->y), Bits(v->z)});
    return G.los;
}
inline void __fastcall FAlloc(CPlaceable* pl, void*) { trace.push_back({"allocate_matrix", Ptr(pl)}); *reinterpret_cast<void**>(reinterpret_cast<uint8*>(pl) + 0x14) = MatAlloc(); }
inline void __fastcall FUpdate(CSimpleTransform* tr, void*, CMatrix* m) { trace.push_back({"update_matrix", Ptr(tr), Ptr(m)}); }
inline bool __fastcall FPointGun(CPedIK* ik, void*, uint32 a, uint32 d, int flag, uint32 n) {
    trace.push_back({"point_gun_in_direction", Ptr(ik), a, d, flag & 0xFF, n});
    return true;
}
inline void __fastcall FRegisterReference(CEntity* entity, void*, CEntity** slot) { trace.push_back({"register_reference", Ptr(entity), Ptr(slot)}); }

// ------------------------------------------------------------------ llamada con captura de ABI y x87 (adaptadores __asm solo en el banco)
struct Frame {
    uint32 eax{}, espBefore{}, espAfter{}, ebxBefore{}, ebxAfter{}, esiBefore{}, esiAfter{}, ediBefore{}, ediAfter{}, ebpBefore{}, ebpAfter{}, sehBefore{}, sehAfter{};
    std::array<uint8, 28> envBefore{}, envAfter{};
};
inline unsigned EnvWord(const std::array<uint8, 28>& e, int field) { uint16 v; std::memcpy(&v, e.data() + field, 2); return v; }
__declspec(noinline) inline Frame CallOne(uintptr_t entry, void* self, void* argument) {
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
        push argument
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

// ------------------------------------------------------------------ fixtures
// Estructura comun: la arena se rellena con 0xCD y se escriben los campos que leen las funciones (desplazamientos del EXE).
inline void SetupCommon(bool withVehMatrix, bool pedMatrix) {
    std::memset(arena.data(), 0xCD, arena.size());
    for (size_t g : {size_t(0), Layout::task + Layout::taskSize, Layout::ped + Layout::pedSize, Layout::veh + Layout::vehSize, Layout::data + Layout::dataSize, Layout::info + Layout::infoSize,
                     Layout::tgt + Layout::tgtSize, Layout::wi + Layout::wiSize, Layout::assoc + Layout::assocSize, Layout::matPed + Layout::matSize, Layout::matVeh + Layout::matSize,
                     Layout::matTgt + Layout::matSize, Layout::matAlloc + Layout::matSize, Layout::vecOut + Layout::vecSize})
        std::memset(arena.data() + g, 0xA5, kGuard);
    Put<void*>(Ped(), 0x58C, Veh());
    Put<void*>(Ped(), 0x480, nullptr);
    Put<void*>(Ped(), 0x14, pedMatrix ? static_cast<void*>(MatPed()) : nullptr);
    Put<uint8>(Ped(), 0x36, ENTITY_TYPE_PED);
    Put<uint8>(Veh(), 0x36, ENTITY_TYPE_VEHICLE);
    Put<void*>(Veh(), 0x14, withVehMatrix ? static_cast<void*>(MatVeh()) : nullptr);
    PutF(Veh(), 0x10, RndBits(4));
    PutF(Veh(), 0x4C0, RndBits(1000));
    Put<int>(Veh(), 0x590, 0);
    PutF(Veh(), 0x648, RndBits(2));
    FillRandom(MatPed(), 0x40); FillRandom(MatVeh(), 0x40); FillRandom(MatTgt(), 0x40);
    for (int i = 0; i < 3; ++i) { PutF(MatPed(), 0x30 + 4 * i, RndBits(60)); PutF(MatVeh(), 0x30 + 4 * i, RndBits(60)); PutF(MatTgt(), 0x30 + 4 * i, RndBits(60)); }
    for (int i = 0; i < 3; ++i) { PutF(MatVeh(), 0x10 + 4 * i, RndBits(2)); PutF(MatTgt(), 0x10 + 4 * i, RndBits(2)); PutF(MatPed(), 0x10 + 4 * i, RndBits(2)); }
    for (int i = 0; i < 3; ++i) { PutF(MatVeh(), 4 * i, RndBits(2)); PutF(MatTgt(), 4 * i, RndBits(2)); PutF(MatVeh(), 0x20 + 4 * i, RndBits(2)); PutF(MatTgt(), 0x20 + 4 * i, RndBits(2)); }
    for (int i = 0; i < 3; ++i) PutF(Ped(), 4 + 4 * i, RndBits(80));
    PutF(Ped(), 0x10, RndBits(4));
    FillRandom(Data(), Layout::dataSize); FillRandom(Info(), Layout::infoSize);
    Put<uint8>(Info(), 0x153, 1);
    for (auto* v : {G.twoVec, G.srcOut, G.tgtOut, G.bone[0], G.bone[1]}) for (int i = 0; i < 3; ++i) v[i] = RndBits(60);
    for (int i = 0; i < 4; ++i) G.radian[i] = rng.N(5) == 0 ? Pick({0x7FC00000, 0xFFC12345, 0x7F800000, 0x00000000, 0x80000000, 0x00000001, 0x3FC90FDB}) : Bits(float(int(rng.N(6283186)) - 3141593) / 1000000.0f);
    G.radianCalls = 0;
    G.coop = rng.N(2); G.los = rng.N(2); G.isPlayer = false;
}
inline void SetupTaskDefaults(uint32 style) {
    FillRandom(Task(), 0x44);
    Put<uint8>(Task(), 0x12, uint8(style));
    Put<uint8>(Task(), 0xA, uint8(rng.N(2)));
    Put<uint8>(Task(), 0xB, uint8(rng.N(2)));
    Put<uint8>(Task(), 0xC, uint8(rng.N(2)));
    Put<uint8>(Task(), 0xD, uint8(rng.N(2)));
    Put<uint8>(Task(), 0xF, uint8(rng.N(8)));
    Put<int32>(Task(), 0x24, int32(Pick({191, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xEE, 5, 0})));
}
inline uint8* ActiveCamBytes() { return reinterpret_cast<uint8*>(&TheCamera) + 0x174 + 0x238 * Get<uint8>(&TheCamera, 0x59); }
inline void SetCam(uint32 idx, uint32 mode) { Put<uint8>(&TheCamera, 0x59, uint8(idx)); Put<uint16>(ActiveCamBytes(), 0xC, uint16(mode)); }
// geometria tipica: que las ramas profundas se recorran con datos finitos y coherentes
inline void TypicalPed() {
    for (int i = 0; i < 3; ++i) { const uint32 b = Bits(FRange(-20, 20)); PutF(Ped(), 4 + 4 * i, b); PutF(MatPed(), 0x30 + 4 * i, b); }
    const float h = FRange(-3.14159f, 3.14159f);
    PutF(Veh(), 0x10, Bits(h));
    PutF(MatVeh(), 0x10, Bits(-std::sin(h))); PutF(MatVeh(), 0x14, Bits(std::cos(h))); PutF(MatVeh(), 0x18, Bits(FRange(-1.2f, 1.2f)));
    if (rng.N(10) == 0) PutF(MatVeh(), 0x18, Bits(FRange(-0.3f, 0.3f)));
}
inline json SetupPlayerTarget() {
    SetupCommon(rng.N(3) != 0, true);
    SetupTaskDefaults(rng.N(9));
    G.isPlayer = rng.N(4) != 0;
    const uint32 idx = rng.N(3);
    const uint32 mode = Pick({0x37, 0x37, 0x31, 0x31, 0x35, 0, 0x30, 0x41});
    SetCam(idx, mode);
    uint8* cam = ActiveCamBytes();
    for (int i = 0; i < 3; ++i) PutF(cam, 0x190 + 4 * i, rng.N(4) == 0 ? RndBits(4) : RndBits(2));
    Put<void*>(Ped(), 0x480, rng.N(3) ? static_cast<void*>(Data()) : nullptr);
    if (rng.N(4) != 0) {
        TypicalPed();
        const float t = FRange(-3.14159f, 3.14159f);
        PutF(cam, 0x190, Bits(-std::sin(t))); PutF(cam, 0x194, Bits(std::cos(t))); PutF(cam, 0x198, Bits(FRange(-1, 1)));
        G.twoVec[0] = Bits(-std::sin(t + 1.0f)); G.twoVec[1] = Bits(std::cos(t + 1.0f)); G.twoVec[2] = Bits(0.2f);
        if (rng.N(3) != 0) Put<uint16>(cam, 0xC, uint16(rng.N(2) ? 0x37 : 0x31));
    }
    return {{"cam_index", idx}, {"cam_mode", Get<uint16>(cam, 0xC)}, {"is_player", G.isPlayer}, {"coop", G.coop}, {"vehicle_matrix", Get<void*>(Veh(), 0x14) != nullptr}, {"player_data", Get<void*>(Ped(), 0x480) != nullptr}, {"seat_rhs", Get<uint8>(Task(), 0xA)}};
}
inline json SetupAiTarget() {
    SetupCommon(rng.N(4) != 0, rng.N(3) != 0);
    const uint32 style = rng.N(12) == 0 ? Pick({9, 0xFF, 0x7F, 0x80, 0xFE}) : rng.N(9);
    SetupTaskDefaults(style);
    G.isPlayer = false;
    Put<void*>(Task(), 0x30, Wi());
    PutF(Wi(), 8, rng.N(3) ? Bits(float(rng.N(300)) / 4.0f) : RndBits(100));
    PutF(Task(), 0x20, rng.N(3) ? Bits(float(rng.N(300)) / 4.0f) : RndBits(100));
    for (int i = 0; i < 3; ++i) PutF(Task(), 0x38 + 4 * i, RndBits(60));
    if (rng.N(8) == 0) for (int i = 0; i < 3; ++i) PutF(Task(), 0x38 + 4 * i, 0);
    const uint32 k = rng.N(5);
    if (k == 0) Put<void*>(Task(), 0x34, nullptr);
    else {
        Put<void*>(Task(), 0x34, Tgt());
        std::memset(Tgt(), 0xCD, Layout::tgtSize);
        Put<uint8>(Tgt(), 0x36, uint8(k == 1 ? ENTITY_TYPE_PED : k == 2 ? ENTITY_TYPE_VEHICLE : k == 3 ? ENTITY_TYPE_OBJECT : ENTITY_TYPE_PED));
        Put<void*>(Tgt(), 0x14, rng.N(3) ? static_cast<void*>(MatTgt()) : nullptr);
        for (int i = 0; i < 3; ++i) PutF(Tgt(), 4 + 4 * i, RndBits(60));
        PutF(Tgt(), 0x540, rng.N(3) ? Bits(float(rng.N(200))) : RndBits(100));
        PutF(Tgt(), 0x4C0, rng.N(3) ? Bits(float(rng.N(1000))) : RndBits(100));
    }
    if (rng.N(4) != 0) {
        TypicalPed();
        PutF(Wi(), 8, Bits(FRange(3, 40)));
        PutF(Task(), 0x20, Bits(FRange(20, 150)));
        if (rng.N(8) != 0) Put<uint8>(Task(), 0xD, 0);
        if (Get<void*>(Task(), 0x34)) {
            PutF(Tgt(), 0x540, Bits(FRange(1, 200))); PutF(Tgt(), 0x4C0, Bits(FRange(1, 1000)));
            for (int i = 0; i < 3; ++i) { PutF(Task(), 0x38 + 4 * i, Bits(FRange(-12, 12))); PutF(Tgt(), 4 + 4 * i, Bits(FRange(-20, 20))); PutF(MatTgt(), 0x30 + 4 * i, Bits(FRange(-20, 20))); }
            const float t = FRange(-3.14159f, 3.14159f);
            PutF(MatTgt(), 0x10, Bits(-std::sin(t))); PutF(MatTgt(), 0x14, Bits(std::cos(t))); PutF(MatTgt(), 0x18, 0); PutF(MatTgt(), 0x00, Bits(std::cos(t))); PutF(MatTgt(), 0x04, Bits(std::sin(t))); PutF(MatTgt(), 0x08, 0);
            PutF(MatTgt(), 0x20, 0); PutF(MatTgt(), 0x24, 0); PutF(MatTgt(), 0x28, Bits(1.0f));
        } else {
            for (int i = 0; i < 3; ++i) PutF(Task(), 0x38 + 4 * i, Bits(FRange(-25, 25)));
        }
        if (rng.N(2)) Put<int32>(Task(), 0x24, 191);
    }
    return {{"style", int(int8(Get<uint8>(Task(), 0x12)))}, {"target_kind", k}, {"target_matrix", k != 0 && Get<void*>(Tgt(), 0x14) != nullptr}, {"ped_matrix", Get<void*>(Ped(), 0x14) != nullptr}, {"vehicle_matrix", Get<void*>(Veh(), 0x14) != nullptr}, {"los", G.los}, {"reached_abort", Get<uint8>(Task(), 0xD)}};
}
inline json SetupAimGun() {
    SetupCommon(rng.N(5) != 0, rng.N(3) != 0);
    SetupTaskDefaults(rng.N(9));
    G.isPlayer = rng.N(3) == 0;
    const uint32 idx = rng.N(3);
    SetCam(idx, Pick({0x37, 0x31, 0x31, 0x35, 0}));
    uint8* cam = ActiveCamBytes();
    for (int i = 0; i < 3; ++i) PutF(cam, 0x19C + 4 * i, RndBits(60));
    Put<void*>(Task(), 0x2C, Assoc());
    Put<int16>(Assoc(), 0x2C, int16(Pick({0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 5, 0, 0x1E7})));
    PutF(Task(), 0x20, Bits(100.0f));
    for (int i = 0; i < 3; ++i) PutF(Task(), 0x38 + 4 * i, RndBits(60));
    if (rng.N(6) == 0) for (int i = 0; i < 3; ++i) PutF(Task(), 0x38 + 4 * i, Pick({0, 0x80000000}));
    const uint32 k = rng.N(5);
    if (k == 0) Put<void*>(Task(), 0x34, nullptr);
    else {
        Put<void*>(Task(), 0x34, Tgt());
        std::memset(Tgt(), 0xCD, Layout::tgtSize);
        Put<uint8>(Tgt(), 0x36, uint8(k == 1 ? ENTITY_TYPE_PED : k == 2 ? ENTITY_TYPE_VEHICLE : ENTITY_TYPE_OBJECT));
        Put<void*>(Tgt(), 0x14, rng.N(3) ? static_cast<void*>(MatTgt()) : nullptr);
        for (int i = 0; i < 3; ++i) PutF(Tgt(), 4 + 4 * i, RndBits(60));
    }
    const bool bike = rng.N(2) != 0;
    Put<int>(Veh(), 0x590, bike ? 9 : 0);
    const bool noVehicle = rng.N(12) == 0;
    Put<void*>(Ped(), 0x58C, noVehicle ? nullptr : static_cast<void*>(Veh()));
    if (bike && !Get<void*>(Veh(), 0x14)) Put<void*>(Veh(), 0x14, MatVeh()); // el EXE lee forward[2] sin comprobar: la rama de moto exige matriz
    if (rng.N(4) != 0) {
        TypicalPed();
        for (int i = 0; i < 3; ++i) PutF(cam, 0x19C + 4 * i, Bits(FRange(-20, 20)));
        PutF(Veh(), 0x648, Bits(FRange(-0.6f, 0.6f)));
        for (int i = 0; i < 3; ++i) PutF(Task(), 0x38 + 4 * i, Bits(FRange(-20, 20)));
        for (int i = 0; i < 3; ++i) { G.srcOut[i] = Bits(FRange(-30, 30)); G.tgtOut[i] = Bits(FRange(-30, 30)); G.bone[0][i] = Bits(FRange(-30, 30)); G.bone[1][i] = Bits(FRange(-30, 30)); }
        G.radian[0] = Bits(FRange(-3.14159f, 3.14159f)); G.radian[1] = Bits(FRange(-1.5f, 1.5f));
        if (Get<void*>(Task(), 0x34)) for (int i = 0; i < 3; ++i) PutF(Tgt(), 4 + 4 * i, Bits(FRange(-20, 20)));
    }
    return {{"player", G.isPlayer}, {"cam_mode", Get<uint16>(cam, 0xC)}, {"target_kind", k}, {"target_matrix", k != 0 && Get<void*>(Tgt(), 0x14) != nullptr}, {"bike", bike && !noVehicle}, {"vehicle", !noVehicle}, {"anim", Get<int16>(Assoc(), 0x2C)}, {"ped_matrix", Get<void*>(Ped(), 0x14) != nullptr}};
}
inline uint32 ctorArgs[6];
inline json SetupCtor() {
    SetupCommon(true, true);
    FillRandom(Task(), 0x44);
    const bool hasPos = rng.N(2) != 0, hasTarget = rng.N(3) != 0;
    for (int k = 0; k < 3; ++k) PutF(VecOut(), 4 * k, RndBits(60));
    ctorArgs[0] = hasTarget ? uint32(uintptr_t(Tgt())) : 0;
    ctorArgs[1] = hasPos ? uint32(uintptr_t(VecOut())) : 0;
    ctorArgs[2] = RndBits(100); ctorArgs[3] = rng.N(256); ctorArgs[4] = rng.N(9); ctorArgs[5] = rng.N(2);
    return {{"has_target", hasTarget}, {"has_position", hasPos}, {"style", ctorArgs[4]}, {"seat_rhs", ctorArgs[5]}};
}

// ------------------------------------------------------------------ captura / restauracion
inline std::vector<uint8> Capture() {
    std::vector<uint8> v(arena.begin(), arena.end());
    const auto* c = reinterpret_cast<const uint8*>(&TheCamera);
    v.insert(v.end(), c, c + sizeof(CCamera));
    return v;
}
inline void Restore(const std::vector<uint8>& v) {
    std::memcpy(arena.data(), v.data(), arena.size());
    std::memcpy(&TheCamera, v.data() + arena.size(), sizeof(CCamera));
}
inline json Diff(const std::vector<uint8>& before, const std::vector<uint8>& after, size_t ignoreBegin, size_t ignoreEnd) {
    json runs = json::array();
    static const char* hex = "0123456789abcdef";
    for (size_t i = 0; i < before.size();) {
        if (before[i] == after[i] || (i >= ignoreBegin && i < ignoreEnd)) { ++i; continue; }
        size_t j = i; std::string bytes;
        while (j < before.size() && before[j] != after[j] && !(j >= ignoreBegin && j < ignoreEnd)) { bytes += hex[after[j] >> 4]; bytes += hex[after[j] & 15]; ++j; }
        runs.push_back(json::array({i, bytes})); i = j;
    }
    return runs;
}
inline bool GuardsOK() {
    for (size_t g : {size_t(0), Layout::task + Layout::taskSize, Layout::ped + Layout::pedSize, Layout::veh + Layout::vehSize, Layout::data + Layout::dataSize, Layout::info + Layout::infoSize,
                     Layout::tgt + Layout::tgtSize, Layout::wi + Layout::wiSize, Layout::assoc + Layout::assocSize, Layout::matPed + Layout::matSize, Layout::matVeh + Layout::matSize,
                     Layout::matTgt + Layout::matSize, Layout::matAlloc + Layout::matSize, Layout::vecOut + Layout::vecSize})
        for (size_t i = 0; i < kGuard; ++i) if (arena[g + i] != 0xA5) return false;
    return true;
}

// ------------------------------------------------------------------ colaboradores: parches temporales en la direccion del EXE y en la entrada de la DLL
struct Fakes {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
    template <class F> void Both(uintptr_t exeEntry, uintptr_t dllEntry, F fake) {
        patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(exeEntry, fake));
        patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(dllEntry, fake));
    }
    template <class F> void ExeOnly(uintptr_t exeEntry, F fake) {
        patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(exeEntry, fake));
    }
    Fakes() {
        using std::bit_cast;
        Both(0x5DF8F0, bit_cast<uintptr_t>(&CPed::IsPlayer), FIsPlayer);
        Both(0x441390, bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn), FIsCoop);
        Both(0x50BFB0, bit_cast<uintptr_t>(&CCamera::SetNewPlayerWeaponMode), FSetMode);
        Both(0x513E40, bit_cast<uintptr_t>(&CCam::Get_TwoPlayer_AimVector), FTwoPlayer);
        Both(0x609FF0, bit_cast<uintptr_t>(&CPlayerPed::GetPlayerInfoForThisPlayerPed), FPlayerInfo);
        Both(0x514970, bit_cast<uintptr_t>(&CCamera::Find3rdPersonCamTargetVector), FFind3rd);
        Both(0x5E4280, bit_cast<uintptr_t>(static_cast<void (CPed::*)(CVector*, eBoneTag, bool)>(&CPed::GetBonePosition)), FBone);
        Both(0x53CBE0, bit_cast<uintptr_t>(static_cast<float (*)(float, float, float, float)>(&CGeneral::GetRadianAngleBetweenPoints)), FRadian);
        Both(0x54F560, bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix), FAlloc);
        Both(0x54F1B0, bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix), FUpdate);
        Both(0x5FDC00, bit_cast<uintptr_t>(&CPedIK::PointGunInDirection), FPointGun);
        Both(0x571B70, bit_cast<uintptr_t>(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::RegisterReference)), FRegisterReference);
        ExeOnly(0x621B10, FLos); // la DLL lo llama por direccion (plugin::CallMethodAndReturn<bool, 0x621B10>): un solo parche
    }
};

// EAX solo lo lee algun llamador del constructor; en PlayerTarget/AITarget/AimGun (ret 4, sin valor) es residuo y no se compara (ver README).
inline json FrameJson(const Frame& f, bool withEax) {
    return {{"eax", withEax ? f.eax : 0u}, {"fcw", EnvWord(f.envAfter, 0)}, {"fsw_top", (EnvWord(f.envAfter, 4) >> 11) & 7}, {"ftw", EnvWord(f.envAfter, 8)}};
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies21d-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "dependencies21d-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("dependencies21d"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;

    const auto fp = CaptureFP(); writeFP = fp; writeFPActive = true; pumpStateOK = true; PumpObserverGuard pumpGuard;
    const auto cameraSaved = ReversingProbe::Bytes(&TheCamera, sizeof(CCamera));
    const int savedErrno = *ExeErrno();
    OriginalDependencies499::HookRoute routeCtor("CTaskSimpleGangDriveBy", "Constructor", 0x6217D0, FunctionToVoidPtr(&CTaskSimpleGangDriveBy::Constructor));
    OriginalDependencies499::HookRoute routePlayer("CTaskSimpleGangDriveBy", "PlayerTarget", 0x621960, FunctionToVoidPtr(&CTaskSimpleGangDriveBy::PlayerTarget));
    OriginalDependencies499::HookRoute routeAi("CTaskSimpleGangDriveBy", "AITarget", 0x627600, FunctionToVoidPtr(&CTaskSimpleGangDriveBy::AITarget));
    OriginalDependencies499::HookRoute routeAim("CTaskSimpleGangDriveBy", "AimGun", 0x628350, FunctionToVoidPtr(&CTaskSimpleGangDriveBy::AimGun));
    OriginalDependencies499::HookRoute* route[4]{&routeCtor, &routePlayer, &routeAi, &routeAim};
    std::vector<uint8> mainOff[4];
    for (int i = 0; i < 4; ++i) mainOff[i] = ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address), 64);

    auto routeProof = [&](int f, bool candidate) {
        const auto entry = fns[f].address;
        return ReversingProbe::JumpsToDll(entry) == candidate
            && (candidate ? (Resolve(entry) == route[f]->Body() && ModuleOf(route[f]->Body()) == ModuleOf(uintptr_t(&ModuleMarker)))
                          : (Resolve(entry) == entry && ModuleOf(entry) == GetModuleHandleA(nullptr) && mainOff[f] == ReversingProbe::Bytes(reinterpret_cast<void*>(entry), 64)));
    };
    bool routesReady = true;
    for (int f = 0; f < 4; ++f) routesReady &= route[f]->Registered() && route[f]->Invoke(true, [&] { return routeProof(f, true); }) && routeProof(f, false);

    Cfg cfgSaved = G;
    const auto idleCheck = [&] { return ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp; };
    pumpStateCheck = idleCheck;
    auto restored = [&](const std::vector<uint8>& input) { return Capture() == input && CaptureFP() == fp && *ExeErrno() == 0; };
    const bool ready = routesReady && Capture().size() == arena.size() + sizeof(CCamera);
    json routesJson = json::array(); for (int f = 0; f < 4; ++f) routesJson.push_back({{"function", fns[f].name}, {"registered", route[f]->Registered()}, {"body", route[f]->Body()}, {"module_ok", ModuleOf(route[f]->Body()) == ModuleOf(uintptr_t(&ModuleMarker))}});
    RestoreFP(fp);
    Write({{"event", "begin"}, {"registered_hook", routesReady}, {"ready", ready}, {"target_routes_ready", routesReady}, {"routes", routesJson}, {"probe_module", uintptr_t(ModuleOf(uintptr_t(&ModuleMarker)))},
           {"arena_bytes", arena.size()}, {"camera_bytes", sizeof(CCamera)}, {"scope", "tareas falsas en arena propia; colaboradores falsos en EXE y DLL; sin tareas, peds ni camara reales (la camara real se guarda y restaura completa)"}});
    if (!ready) { Write({{"event", "error"}, {"reason", "routes preflight failed"}, {"pairs", 0}, {"state_restored", true}}); return; }

    bool passed = true; unsigned total = 0;
    const auto setup = [&](int f) -> json { switch (f) { case kCtor: return SetupCtor(); case kPlayerTarget: return SetupPlayerTarget(); case kAiTarget: return SetupAiTarget(); default: return SetupAimGun(); } };
    for (int f = 0; f < 4; ++f) {
        unsigned failures = 0;
        const std::string full = std::string(fns[f].cls) + "::" + fns[f].name;
        for (int n = 0; n < fns[f].cases; ++n) {
            rng = Rng(uint64(f + 1) * 1000003ull + uint64(n) * 7919ull + 17ull);
            const json profile = setup(f);
            const auto input = Capture();
            const Cfg cfg0 = G;
            std::unique_ptr<Fakes> fakes = std::make_unique<Fakes>();
            auto invoke = [&](bool candidate) -> json {
                Restore(input); G = cfg0; G.radianCalls = 0; trace = json::array(); *ExeErrno() = 0; RestoreFP(fp);
                bool routesOk = false;
                const auto frame = route[f]->Invoke(candidate, [&] {
                    routesOk = routeProof(f, candidate); RestoreFP(fp);
                    Frame r = (f == kCtor) ? CallSix(fns[f].address, Task(), ctorArgs[0], ctorArgs[1], ctorArgs[2], ctorArgs[3], ctorArgs[4], ctorArgs[5])
                                           : CallOne(fns[f].address, Task(), Ped());
                    routesOk &= routeProof(f, candidate); return r; });
                const auto after = Capture();
                const int errnoAfter = *ExeErrno();
                const bool guards = GuardsOK();
                const uint32 vptr = Get<uint32>(Task(), 0);
                const bool vptrOk = f != kCtor || (candidate ? (vptr != 0 && ModuleOf(vptr) == ModuleOf(uintptr_t(&ModuleMarker))) : vptr == 0x86D944);
                json row = {
                    {"return", f == kCtor ? json(frame.eax) : json(0)}, {"trace", trace}, {"changes", f == kCtor ? Diff(input, after, Layout::task, Layout::task + 4) : Diff(input, after, 0, 0)}, {"errno", errnoAfter}, {"x87", FrameJson(frame, f == kCtor)},
                    {"task_after", {Get<uint8>(Task(), 0xB), Get<uint8>(Task(), 0xC), Get<uint8>(Task(), 0xD), Get<uint8>(Task(), 0xF), Get<int32>(Task(), 0x24)}},
                    {"route_ok", routesOk}, {"stack_restored", frame.espBefore == frame.espAfter},
                    {"nonvolatile_preserved", frame.ebxBefore == frame.ebxAfter && frame.esiBefore == frame.esiAfter && frame.ediBefore == frame.ediAfter && frame.ebpBefore == frame.ebpAfter},
                    {"seh_restored", frame.sehBefore == frame.sehAfter},
                    {"fpu_top_balanced", ((EnvWord(frame.envBefore, 4) >> 11) & 7) == ((EnvWord(frame.envAfter, 4) >> 11) & 7) && EnvWord(frame.envAfter, 8) == 0xFFFF},
                    {"fpu_control_same", EnvWord(frame.envBefore, 0) == EnvWord(frame.envAfter, 0)},
                    {"guards_preserved", guards}, {"vptr_ok", vptrOk}, {"fixture_contract", guards && after.size() == input.size()}};
                Restore(input); G = cfg0; *ExeErrno() = 0; RestoreFP(fp);
                row["state_restored"] = restored(input);
                row["floating_state_restored"] = CaptureFP() == fp;
                return row;
            };
            pumpStateCheck = [&] { return Capture() == input; };
            const auto original = invoke(false);
            const auto candidate = invoke(true);
            fakes.reset();
            bool equal = original == candidate;
            for (const char* gate : {"route_ok", "stack_restored", "nonvolatile_preserved", "seh_restored", "fpu_top_balanced", "fpu_control_same", "guards_preserved", "vptr_ok", "fixture_contract", "state_restored", "floating_state_restored"})
                equal &= original.value(gate, false) && candidate.value(gate, false);
            passed &= equal; failures += !equal;
            Write({{"event", "comparison"}, {"function", full}, {"pair", total}, {"case", n}, {"profile", profile}, {"original", original}, {"candidate", candidate}, {"equal", equal}, {"accepted", equal}});
            std::memcpy(&TheCamera, cameraSaved.data(), sizeof(CCamera)); // la camara real vuelve a su valor entre pares
            pumpStateCheck = idleCheck;
            ++total;
        }
        Write({{"event", "function_summary"}, {"function", full}, {"cases", fns[f].cases}, {"failures", failures}});
    }
    std::memcpy(&TheCamera, cameraSaved.data(), sizeof(CCamera));
    *ExeErrno() = savedErrno;
    G = cfgSaved;
    bool hooks = true; for (int f = 0; f < 4; ++f) hooks &= route[f]->Restore();
    const bool state = Capture().size() == arena.size() + sizeof(CCamera) && ReversingProbe::Bytes(&TheCamera, sizeof(CCamera)) == cameraSaved && CaptureFP() == fp;
    RestoreFP(fp);
    Write({{"event", passed && state && hooks ? "complete" : "error"}, {"pairs", total}, {"state_restored", state && hooks}, {"floating_state_restored", CaptureFP() == fp}});
    RestoreFP(fp);
}
}
