// Banco FUERA DEL JUEGO de CAutomobile::PreRender (0x6AAB50).
//
// Un proceso propio mapea el gta-sa.exe canonico en 0x400000 (solo lectura del archivo), sustituye por falsos los colaboradores y ejecuta el MISMO caso dos veces:
//   - original: los bytes del EXE en 0x6AAB50 (se ejecutan de verdad, incluidas las rutinas matematicas y de matrices que no se sustituyen);
//   - candidato: la fuente C++ de CAutomobile::PreRender extraida de source/game_sa/Entity/Vehicle/Automobile.cpp (extract.inc, generado por build_offgame.py).
// Ambos lados llaman a los mismos falsos (misma funcion nucleo; el EXE entra por un parche JMP con la convencion de llamada exacta del EXE) y se comparan:
// memoria completa (objeto con guardas, marcos, matrices, ColModel, manejo, modelo, camara, globales), traza de colaboradores, ABI (ESP, EBX/ESI/EDI/EBP, DF,
// x87 normalizado, palabra de control). Los registros EAX/ECX/EDX se registran pero no se exigen (void; ningun llamador del EXE los lee).
//
// Uso: harness.exe [casos] [primera_semilla] [ruta_exe]    (por defecto 8192 casos, semilla 0, D:\Grand Theft Auto San Andreas\gta-sa.exe)
#include "StdInc.h"
#include "Automobile.h"
#include "X87Math.h"
#include "Shadows.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <map>

CCamera& TheCamera = *reinterpret_cast<CCamera*>(0xB6F028);

namespace H {
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

constexpr uintptr_t kExeBase = 0x400000;
constexpr uintptr_t kExeEnd = 0xCB1000;
constexpr size_t kGuard = 0x2000;
constexpr size_t kCarSize = 0x988;
constexpr size_t kFrames = 28;       // 25 nodos + marco de reposo del montacargas + 2 de reserva
constexpr size_t kFrameSize = 0x60;

// ------------------------------------------------------------------------------------------------ imagen del EXE
inline bool LoadExe(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) { printf("No se puede abrir %s\n", path); return false; }
    fseek(f, 0, SEEK_END);
    const long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::vector<u8> file(size);
    fread(file.data(), 1, size, f);
    fclose(f);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(file.data());
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(file.data() + dos->e_lfanew);
    if (nt->OptionalHeader.ImageBase != kExeBase) { printf("ImageBase inesperada\n"); return false; }
    // run_offgame.py reserva la zona antes de que el cargador de Windows mapee nada alli; aqui solo se confirma (MEM_COMMIT) y, si no estaba reservada, se intenta reservarla
    void* base = VirtualAlloc(reinterpret_cast<void*>(kExeBase), kExeEnd - kExeBase, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (base != reinterpret_cast<void*>(kExeBase)) base = VirtualAlloc(reinterpret_cast<void*>(kExeBase), kExeEnd - kExeBase, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (base != reinterpret_cast<void*>(kExeBase)) {
        printf("No se pudo reservar 0x400000 (%p, error %lu)\n", base, GetLastError());
        for (uintptr_t a = kExeBase; a < kExeEnd;) {
            MEMORY_BASIC_INFORMATION mbi{};
            if (!VirtualQuery(reinterpret_cast<void*>(a), &mbi, sizeof(mbi))) break;
            printf("  %p +%zx state=%lx type=%lx alloc=%p\n", mbi.BaseAddress, mbi.RegionSize, mbi.State, mbi.Type, mbi.AllocationBase);
            a = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        }
        return false;
    }
    const auto* sec = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const size_t n = std::min<size_t>(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize);
        memcpy(reinterpret_cast<u8*>(kExeBase) + sec[i].VirtualAddress, file.data() + sec[i].PointerToRawData, n);
    }
    // El juego arranca con la CRT inicializada: _CIpow del EXE (0x822130) toma la ruta SSE2 (Intel LIBM, 0x829B80) cuando 0xC9C400 vale 1 (PowEquivalence);
    // sin ejecutar el arranque de la CRT esa bandera esta a 0 y el EXE usaria la ruta x87 de 24 bits.
    *reinterpret_cast<u32*>(0xC9C400) = 1;
    return true;
}

inline void PatchJmp(uintptr_t at, const void* target) {
    auto* p = reinterpret_cast<u8*>(at);
    p[0] = 0xE9;
    *reinterpret_cast<int32_t*>(p + 1) = int32_t(reinterpret_cast<uintptr_t>(target) - (at + 5));
}

// ------------------------------------------------------------------------------------------------ memoria del caso
struct Arena { const char* name; u8* base; size_t size; };
alignas(16) inline std::array<u8, kGuard + kCarSize + kGuard> carBuf{};
alignas(16) inline std::array<u8, 0x200> towedBuf{};
alignas(16) inline std::array<u8, 0x100> handlingBuf{};
alignas(16) inline std::array<u8, 0x80> carMatrixBuf{};
alignas(16) inline std::array<u8, 0x80> cameraMatrixBuf{};
alignas(16) inline std::array<u8, kFrames * kFrameSize> framesBuf{};
alignas(16) inline std::array<u8, 0x100> modelInfoBuf{};
alignas(16) inline std::array<u8, 0x100> forkliftInfoBuf{};
alignas(16) inline std::array<u8, 0x40> clumpBuf{};
alignas(16) inline std::array<u8, 0x80> colModelBuf{};
alignas(16) inline std::array<u8, 0x80> colDataBuf{};
alignas(16) inline std::array<u8, 0x200> linesBuf{};
alignas(16) inline std::array<void*, 96> fakeVTable{};
alignas(16) inline std::array<u8, 0x40> otherVehicleBuf{};
inline std::array<u8, 0x200> cameraBlockSnapshot{};   // TheCamera (0xB6F028..)
constexpr uintptr_t kCameraBase = 0xB6F028;
constexpr size_t kCameraBytes = 0x200;

inline u8* Car() { return carBuf.data() + kGuard; }
inline u8* Frame(size_t i) { return framesBuf.data() + i * kFrameSize; }
inline const std::array<Arena, 14>& Arenas() {
    static const std::array<Arena, 14> a{{
        {"car", carBuf.data(), carBuf.size()}, {"towed", towedBuf.data(), towedBuf.size()}, {"handling", handlingBuf.data(), handlingBuf.size()},
        {"carMatrix", carMatrixBuf.data(), carMatrixBuf.size()}, {"cameraMatrix", cameraMatrixBuf.data(), cameraMatrixBuf.size()},
        {"frames", framesBuf.data(), framesBuf.size()}, {"modelInfo", modelInfoBuf.data(), modelInfoBuf.size()},
        {"forkliftInfo", forkliftInfoBuf.data(), forkliftInfoBuf.size()}, {"clump", clumpBuf.data(), clumpBuf.size()},
        {"colModel", colModelBuf.data(), colModelBuf.size()}, {"colData", colDataBuf.data(), colDataBuf.size()}, {"lines", linesBuf.data(), linesBuf.size()},
        {"otherVehicle", otherVehicleBuf.data(), otherVehicleBuf.size()}, {"camera", reinterpret_cast<u8*>(kCameraBase), kCameraBytes},
    }};
    return a;
}

// Puntero normalizado para la traza (nulo, arena+desplazamiento, pila o fuera de las arenas)
inline u32 Tag(const void* p) {
    if (!p) return 0;
    const auto addr = reinterpret_cast<uintptr_t>(p);
    const auto& arenas = Arenas();
    for (unsigned i = 0; i < arenas.size(); ++i) {
        const auto base = reinterpret_cast<uintptr_t>(arenas[i].base);
        if (addr >= base && addr < base + arenas[i].size) {
            u32 off = u32(addr - base);
            if (i == 0) off -= u32(kGuard);           // el objeto se mide desde su inicio
            return ((i + 1) << 24) | (off & 0xFFFFFF);
        }
    }
    const auto stackHigh = uintptr_t(__readfsdword(4));
    const auto stackLow = uintptr_t(__readfsdword(8));
    if (addr >= stackLow && addr < stackHigh) return 0xFE000000u;
    return 0xFF000000u | (u32(addr) & 0xFFFFFF);
}

// ------------------------------------------------------------------------------------------------ generador
struct Rng {
    u64 s;
    explicit Rng(u64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0x1234567ull) { for (int i = 0; i < 6; ++i) Next(); }
    u32 Next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return u32((s >> 11) ^ (s >> 32)); }
    float Unit() { return float(Next() & 0xFFFFFF) / 16777216.0f; }
    float Range(float a, float b) { return a + (b - a) * Unit(); }
    bool Chance(u32 num, u32 den) { return Next() % den < num; }
    u32 Pick(u32 n) { return Next() % n; }
};
inline float Bits(u32 b) { float f; memcpy(&f, &b, 4); return f; }
inline u32 BitsOf(float f) { u32 b; memcpy(&b, &f, 4); return b; }
inline float WildFloat(Rng& r) {
    switch (r.Pick(20)) {
    case 0: return 0.0f;
    case 1: return -0.0f;
    case 2: return Bits(0x7F800000u);
    case 3: return Bits(0xFF800000u);
    case 4: return Bits(0x7FC00000u);       // NaN silencioso
    case 5: return FLT_MAX;
    case 6: return -FLT_MAX;
    case 7: return FLT_MIN;
    case 8: return Bits(0x00000010u);       // denormal
    case 9: return 1e30f;
    case 10: return -1e-30f;
    case 11: { u32 b = r.Next(); if (((b >> 23) & 0xFF) == 0xFF && (b & 0x7FFFFF)) b |= 0x400000; return Bits(b); }   // bits crudos (nunca sNaN)
    case 12: return 1.0f;
    case 13: return -1.0f;
    case 14: return 1e19f;
    case 15: return Bits(r.Next() & 0x7FFFFF);                // subnormal cualquiera
    case 16: return Bits(0x00800000u + (r.Next() & 0xFFFF)); // normal junto a FLT_MIN
    case 17: return Bits(0x80000000u | (r.Next() & 0x7FFFFF)); // subnormal negativo
    case 18: return Bits(0x7FC00000u | (r.Next() & 0x3FFFFF) | ((r.Next() & 1) << 31)); // NaN silencioso con carga y signo cualesquiera
    case 19: return Bits(0x7F800000u | ((r.Next() & 1) << 31));   // infinito
    default: return r.Range(-1000.f, 1000.f);
    }
}
struct Gen {
    Rng& r;
    bool wild;
    float F(float lo, float hi) { return wild && r.Chance(1, 10) ? WildFloat(r) : r.Range(lo, hi); }
};

template<class T> inline void Put(u8* base, size_t off, T v) { memcpy(base + off, &v, sizeof(T)); }
template<class T> inline T Get(const u8* base, size_t off) { T v; memcpy(&v, base + off, sizeof(T)); return v; }

// ------------------------------------------------------------------------------------------------ traza y falsos
inline std::vector<u32> trace;
inline u64 fakeState = 1;
inline unsigned idCounts[64]{};
inline u32 FakeNext() { fakeState ^= fakeState << 13; fakeState ^= fakeState >> 7; fakeState ^= fakeState << 17; return u32(fakeState >> 16); }
inline float FakeFloat() {
    Rng r(FakeNext());
    if (r.Chance(1, 8)) return WildFloat(r);
    return r.Range(-3.f, 3.f);
}
inline void FakeVec(float* v) { v[0] = FakeFloat(); v[1] = FakeFloat(); v[2] = FakeFloat(); }
inline void T(u32 v) { trace.push_back(v); }
inline void TF(float f) { trace.push_back(BitsOf(f)); }
inline void TV(const float* v) { TF(v[0]); TF(v[1]); TF(v[2]); }
enum Id : u32 {
    kBasePreRender = 1, kDoHover, kDoBurst, kTowHitch, kTowBar, kColModel, kAddSingle, kCamNoRain, kPlayerNoRain, kWaterSplash, kExhaust, kDamagedParticles,
    kCorona, kLight, kAppearance, kVehicleLights, kFindPlayer, kLookingForward, kShadow, kSetCompRot, kWheelRot, kUpdateWheelMatrix, kSwingingDoor, kPanel,
    kDoor, kWheelPosn, kFrameFromId, kSetTransRot, kBadVirtual, kIdCount
};
inline std::vector<std::pair<size_t, u32>> callStarts;
inline void Begin(Id id) { callStarts.emplace_back(trace.size(), u32(id)); T(id); ++idCounts[id]; }

// ---- nucleos (las usan EXE y DLL)
inline void C_BasePreRender(const void* self) { Begin(kBasePreRender); T(Tag(self)); }
inline void C_DoHover(const void* self) { Begin(kDoHover); T(Tag(self)); }
inline void C_DoBurst(const void* self) { Begin(kDoBurst); T(Tag(self)); }
inline bool C_TowHitch(const void* self, float* out, bool check, const void* veh) { Begin(kTowHitch); T(Tag(self)); T(check); T(Tag(veh)); FakeVec(out); return (FakeNext() & 3) != 0; }
inline bool C_TowBar(const void* self, float* out, bool check, const void* veh) { Begin(kTowBar); T(Tag(self)); T(check); T(Tag(veh)); FakeVec(out); return (FakeNext() & 3) != 0; }
inline void* C_ColModel(const void* self) { Begin(kColModel); T(Tag(self)); return colModelBuf.data(); }
inline bool C_AddSingle(const void* self, u32 state, u32 a1, float a2, float a3, const void* cp, const float* pos, float a6, int a7, u32 surf, const u8* blood, u32 a10) {
    Begin(kAddSingle); T(Tag(self)); T(state); T(a1); TF(a2); TF(a3); T(Tag(cp)); TV(reinterpret_cast<const float*>(cp)); T(Tag(pos)); TV(pos); TF(a6); T(a7); T(surf); T(Tag(blood)); T(*blood); T(a10);
    return (FakeNext() & 1) != 0;
}
inline bool C_CamNoRain() { Begin(kCamNoRain); return (FakeNext() & 3) == 0; }
inline bool C_PlayerNoRain() { Begin(kPlayerNoRain); return (FakeNext() & 3) == 0; }
inline void C_Water(const void* self) { Begin(kWaterSplash); T(Tag(self)); }
inline void C_Exhaust(const void* self) { Begin(kExhaust); T(Tag(self)); }
inline void C_Damaged(const void* self) { Begin(kDamagedParticles); T(Tag(self)); }
inline void C_Corona(u32 id, const void* attach, u8 r, u8 g, u8 b, u8 intensity, const float* pos, float size, float range, u32 ctype, u32 flare, u32 refl, u32 los, u32 trail,
                     float normal, bool neon, float pull, bool fullBright, float fade, bool below, bool whiteCore) {
    Begin(kCorona); T(id - u32(reinterpret_cast<uintptr_t>(Car()))); T(Tag(attach)); T(r); T(g); T(b); T(intensity); T(Tag(pos)); TV(pos); TF(size); TF(range);
    T(ctype & 0xFF); T(flare & 0xFF); T(refl & 0xFF); T(los & 0xFF); T(trail & 0xFF); TF(normal); T(neon); TF(pull); T(fullBright); TF(fade); T(below); T(whiteCore);
}
inline void C_Light(u32 type, const float* point, const float* dir, float radius, float r, float g, float b, u32 fog, u32 gen, const void* entity) {
    Begin(kLight); T(type & 0xFF); TV(point); TV(dir); TF(radius); TF(r); TF(g); TF(b); T(fog & 0xFF); T(gen & 0xFF); T(Tag(entity));
}
inline u32 C_Appearance(const void* self) { Begin(kAppearance); T(Tag(self)); const u32 v = FakeNext() % 8; return v > 5 ? 3 : v; }
inline void C_VehicleLights(const void* self, const void* matrix, u32 flags) { Begin(kVehicleLights); T(Tag(self)); T(Tag(matrix)); T(flags); }
inline void* C_FindPlayer(int a, bool b) { Begin(kFindPlayer); T(a); T(b); const u32 v = FakeNext() % 4; return v == 0 ? nullptr : v == 1 ? static_cast<void*>(otherVehicleBuf.data()) : static_cast<void*>(Car()); }
inline bool C_Looking(const void* self) { Begin(kLookingForward); T(Tag(self)); return (FakeNext() & 1) != 0; }
inline void C_Shadow(const void* veh, int type) { Begin(kShadow); T(Tag(veh)); T(type); }
inline void C_SetCompRot(const void* self, const void* frame, int axis, float angle, bool reset) { Begin(kSetCompRot); T(Tag(self)); T(Tag(frame)); T(axis); TF(angle); T(reset); }
inline float C_WheelRot(const void* self, u32 state, const float* a, const float* b, float radius) { Begin(kWheelRot); T(Tag(self)); T(state); TV(a); TV(b); TF(radius); return FakeFloat(); }
inline void C_UpdateWheel(const void* self, int node, int flags) { Begin(kUpdateWheelMatrix); T(Tag(self)); T(node); T(flags); }
inline void C_SwingDoor(const void* self, int node, int door) { Begin(kSwingingDoor); T(Tag(self)); T(node); T(door); }
inline void C_Panel(const void* panel, const void* veh, const void* frame, const float* a, const float* b, float c, float d) {
    Begin(kPanel); T(Tag(panel)); T(Tag(veh)); T(Tag(frame)); TV(a); TV(b); TF(c); TF(d);
}
inline bool C_Door(const void* door, const void* veh, const float* a, const float* b, const float* c) { Begin(kDoor); T(Tag(door)); T(Tag(veh)); TV(a); TV(b); TV(c); return (FakeNext() & 1) != 0; }
inline void C_WheelPosn(const void* mi, int wheel, float* out, bool local) { Begin(kWheelPosn); T(Tag(mi)); T(wheel); T(local); FakeVec(out); }
inline void* C_FrameFromId(const void* clump, int id) { Begin(kFrameFromId); T(Tag(clump)); T(id); return Frame(26); }
inline void C_SetTransRot(const void* self, const void* frame, float l, float r, const float* pos, bool front) { Begin(kSetTransRot); T(Tag(self)); T(Tag(frame)); TF(l); TF(r); TV(pos); T(front); }
inline void* C_BadVirtual() { Begin(kBadVirtual); return nullptr; }

// ---- adaptadores con la convencion del EXE (thiscall = fastcall con EDX ficticio; el que limpia la pila coincide)
#define EXE_THIS(name) static auto __fastcall name
inline void __fastcall X_BasePreRender(void* self, int) { C_BasePreRender(self); }
inline void __fastcall X_DoHover(void* self, int) { C_DoHover(self); }
inline void __fastcall X_DoBurst(void* self, int) { C_DoBurst(self); }
inline bool __fastcall X_TowHitch(void* self, int, float* out, bool check, void* veh) { return C_TowHitch(self, out, check, veh); }
inline bool __fastcall X_TowBar(void* self, int, float* out, bool check, void* veh) { return C_TowBar(self, out, check, veh); }
inline void* __fastcall X_ColModel(void* self, int) { return C_ColModel(self); }
inline bool __fastcall X_AddSingle(void* self, int, u32 state, u32 a1, float a2, float a3, void* cp, float* pos, float a6, int a7, u32 surf, u8* blood, u32 a10) {
    return C_AddSingle(self, state, a1, a2, a3, cp, pos, a6, a7, surf, blood, a10);
}
inline bool __cdecl X_CamNoRain() { return C_CamNoRain(); }
inline bool __cdecl X_PlayerNoRain() { return C_PlayerNoRain(); }
inline void __fastcall X_Water(void* self, int) { C_Water(self); }
inline void __fastcall X_Exhaust(void* self, int) { C_Exhaust(self); }
inline void __fastcall X_Damaged(void* self, int) { C_Damaged(self); }
inline void __cdecl X_Corona(u32 id, void* attach, u8 r, u8 g, u8 b, u8 intensity, const float* pos, float size, float range, u32 ctype, u8 flare, u8 refl, u8 los, u8 trail,
                             float normal, bool neon, float pull, bool fullBright, float fade, bool below, bool whiteCore) {
    C_Corona(id, attach, r, g, b, intensity, pos, size, range, ctype, flare, refl, los, trail, normal, neon, pull, fullBright, fade, below, whiteCore);
}
inline void __cdecl X_Light(u32 type, float px, float py, float pz, float dx, float dy, float dz, float radius, float r, float g, float b, u32 fog, u32 gen, void* entity) {
    const float p[3]{px, py, pz}, d[3]{dx, dy, dz};
    C_Light(type, p, d, radius, r, g, b, fog, gen, entity);
}
inline u32 __fastcall X_Appearance(void* self, int) { return C_Appearance(self); }
inline void __fastcall X_VehicleLights(void* self, int, void* matrix, u32 flags) { C_VehicleLights(self, matrix, flags); }
inline void* __cdecl X_FindPlayer(int a, bool b) { return C_FindPlayer(a, b); }
inline bool __fastcall X_Looking(void* self, int) { return C_Looking(self); }
inline void __cdecl X_Shadow(void* veh, int type) { C_Shadow(veh, type); }
inline void __fastcall X_SetCompRot(void* self, int, void* frame, int axis, float angle, bool reset) { C_SetCompRot(self, frame, axis, angle, reset); }
inline float __fastcall X_WheelRot(void* self, int, u32 state, float* a, float* b, float radius) { return C_WheelRot(self, state, a, b, radius); }
inline void __fastcall X_UpdateWheel(void* self, int, int node, int flags) { C_UpdateWheel(self, node, flags); }
inline void __fastcall X_SwingDoor(void* self, int, int node, int door) { C_SwingDoor(self, node, door); }
inline void __fastcall X_Panel(void* panel, int, void* veh, void* frame, float ax, float ay, float az, float bx, float by, float bz, float c, float d) {
    const float a[3]{ax, ay, az}, b[3]{bx, by, bz};
    C_Panel(panel, veh, frame, a, b, c, d);
}
inline bool __fastcall X_Door(void* door, int, void* veh, float* a, float* b, float* c) { return C_Door(door, veh, a, b, c); }
inline void __fastcall X_WheelPosn(void* mi, int, int wheel, float* out, bool local) { C_WheelPosn(mi, wheel, out, local); }
inline void* __cdecl X_FrameFromId(void* clump, int id) { return C_FrameFromId(clump, id); }
inline void __fastcall X_SetTransRot(void* self, int, void* frame, float l, float r, float px, float py, float pz, bool front) {
    const float p[3]{px, py, pz};
    C_SetTransRot(self, frame, l, r, p, front);
}
inline int* __cdecl X_Errno() { static int e{}; return &e; }
inline void __fastcall X_BadVirtual(void*, int) { C_BadVirtual(); }

inline void InstallFakes() {
    struct { uintptr_t at; const void* fn; } patches[]{
        {0x6D6480, (const void*)&X_BasePreRender}, {0x535300, (const void*)&X_ColModel}, {0x6DE880, (const void*)&X_AddSingle},
        {0x72DDB0, (const void*)&X_CamNoRain}, {0x72DDC0, (const void*)&X_PlayerNoRain}, {0x6DDF60, (const void*)&X_Water}, {0x6DE240, (const void*)&X_Exhaust},
        {0x6D2A80, (const void*)&X_Damaged}, {0x6FC580, (const void*)&X_Corona}, {0x7000E0, (const void*)&X_Light}, {0x6D1080, (const void*)&X_Appearance},
        {0x6E1A60, (const void*)&X_VehicleLights}, {0x56E0D0, (const void*)&X_FindPlayer}, {0x50AED0, (const void*)&X_Looking}, {0x70BDA0, (const void*)&X_Shadow},
        {0x6DBA30, (const void*)&X_SetCompRot}, {0x6D1230, (const void*)&X_WheelRot}, {0x6AA290, (const void*)&X_UpdateWheel}, {0x6A9D70, (const void*)&X_SwingDoor},
        {0x6F49A0, (const void*)&X_Panel}, {0x6F4040, (const void*)&X_Door}, {0x4C7D20, (const void*)&X_WheelPosn}, {0x4C53C0, (const void*)&X_FrameFromId},
        {0x6DBBB0, (const void*)&X_SetTransRot}, {0x82B181, (const void*)&X_Errno},
    };
    for (const auto& p : patches) PatchJmp(p.at, p.fn);
    for (auto& e : fakeVTable) e = (void*)&X_BadVirtual;
    fakeVTable[52] = (void*)&X_DoBurst;     // +0xD0
    fakeVTable[59] = (void*)&X_TowHitch;    // +0xEC
    fakeVTable[60] = (void*)&X_TowBar;      // +0xF0
    fakeVTable[69] = (void*)&X_DoHover;     // +0x114
}
}  // namespace H

// ------------------------------------------------------------------------------------------------ lado DLL: definiciones de los miembros que usa la fuente extraida
// Las rutinas de matrices, GetSpeed y GetWheelStatus se resuelven en el EXE (las mismas que ejecuta el lado original); el resto son los falsos del banco.
using namespace H;
void CVehicle::PreRender() { C_BasePreRender(this); }
CColModel* CEntity::GetColModel() const { return static_cast<CColModel*>(C_ColModel(this)); }
bool CVehicle::AddSingleWheelParticles(tWheelState a, uint32 b, float c, float d, CColPoint* e, CVector* f, float g, int32 h, uint32 i, bool* j, uint32 k) {
    return C_AddSingle(this, u32(a), b, c, d, e, &f->x, g, h, i, reinterpret_cast<const u8*>(j), k);
}
void CVehicle::AddWaterSplashParticles() { C_Water(this); }
void CVehicle::AddExhaustParticles() { C_Exhaust(this); }
void CVehicle::AddDamagedVehicleParticles() { C_Damaged(this); }
eVehicleAppearance CVehicle::GetVehicleAppearance() const { return static_cast<eVehicleAppearance>(C_Appearance(this)); }
void CVehicle::DoVehicleLights(CMatrix& matrix, eVehicleLightsFlags flags) { C_VehicleLights(this, &matrix, u32(flags)); }
void CVehicle::SetComponentRotation(RwFrame* component, eRotationAxis axis, float angle, bool reset) { C_SetCompRot(this, component, int(axis), angle, reset); }
float CVehicle::ProcessWheelRotation(tWheelState state, const CVector& a, const CVector& b, float radius) { return C_WheelRot(this, u32(state), &a.x, &b.x, radius); }
void CVehicle::SetTransmissionRotation(RwFrame* component, float angleL, float angleR, CVector wheelPos, bool isFront) { C_SetTransRot(this, component, angleL, angleR, &wheelPos.x, isFront); }
void CAutomobile::UpdateWheelMatrix(int32 node, int32 flags) { C_UpdateWheel(this, node, flags); }
void CAutomobile::ProcessSwingingDoor(eCarNodes node, eDoors door) { C_SwingDoor(this, int(node), int(door)); }
void CAutomobile::DoBurstAndSoftGroundRatios() { C_DoBurst(this); }
void CAutomobile::DoHoverSuspensionRatios() { C_DoHover(this); }
bool CVehicle::GetTowHitchPos(CVector& out, bool check, CVehicle* veh) { return C_TowHitch(this, &out.x, check, veh); }
bool CVehicle::GetTowBarPos(CVector& out, bool check, CVehicle* veh) { return C_TowBar(this, &out.x, check, veh); }
void CBouncingPanel::ProcessPanel(CVehicle* veh, RwFrame* frame, CVector a, CVector b, float c, float d) { C_Panel(this, veh, frame, &a.x, &b.x, c, d); }
bool CDoor::Process(CVehicle* veh, Const CVector& a, Const CVector& b, Const CVector& c) { return C_Door(this, veh, &a.x, &b.x, &c.x); }
void CVehicleModelInfo::GetWheelPosn(int32 wheel, CVector& out, bool local) const { C_WheelPosn(this, wheel, &out.x, local); }
RwFrame* CClumpModelInfo::GetFrameFromId(RpClump* clump, int32 id) { return static_cast<RwFrame*>(C_FrameFromId(clump, id)); }
void CCoronas::RegisterCorona(uint32 id, CEntity* attach, uint8 r, uint8 g, uint8 b, uint8 intensity, const CVector& pos, float size, float range, eCoronaType ctype,
                              eCoronaFlareType flare, eCoronaReflType refl, eCoronaLOSCheck los, eCoronaTrail trail, float normal, bool neon, float pull, bool fullBright,
                              float fade, bool below, bool whiteCore) {
    C_Corona(id, attach, r, g, b, intensity, &pos.x, size, range, u32(ctype), u32(flare), u32(refl), u32(los), u32(trail), normal, neon, pull, fullBright, fade, below, whiteCore);
}
void CPointLights::AddLight(uint8 type, CVector point, CVector dir, float radius, float r, float g, float b, uint8 fog, bool gen, CEntity* entity) {
    C_Light(type, &point.x, &dir.x, radius, r, g, b, fog, gen, entity);
}
void CShadows::StoreShadowForVehicle(CVehicle* veh, VEH_SHD_TYPE type) { C_Shadow(veh, int(type)); }
bool CCullZones::CamNoRain() { return C_CamNoRain(); }
bool CCullZones::PlayerNoRain() { return C_PlayerNoRain(); }
CVehicle* FindPlayerVehicle(int32 a, bool b) { return static_cast<CVehicle*>(C_FindPlayer(a, b)); }
bool CCamera::GetLookingForwardFirstPerson() const { return C_Looking(this); }
// ---- EXE nativo
CVector CPhysical::GetSpeed(CVector point) {
    CVector out;
    reinterpret_cast<CVector*(__thiscall*)(CPhysical*, CVector*, float, float, float)>(0x542CE0)(this, &out, point.x, point.y, point.z);
    return out;
}
eCarWheelStatus CDamageManager::GetWheelStatus(eCarWheel wheel) const {
    return static_cast<eCarWheelStatus>(reinterpret_cast<u32(__thiscall*)(const CDamageManager*, int)>(0x6C21B0)(this, int(wheel)));
}
void CMatrix::Attach(RwMatrix* m, bool owns) { reinterpret_cast<void(__thiscall*)(CMatrix*, RwMatrix*, bool)>(0x59BD10)(this, m, owns); }
void CMatrix::UpdateRW() { reinterpret_cast<void(__thiscall*)(CMatrix*)>(0x59BBB0)(this); }
void CMatrix::SetRotate(float x, float y, float z) { reinterpret_cast<void(__thiscall*)(CMatrix*, float, float, float)>(0x59B120)(this, x, y, z); }
void CMatrix::RotateX(float a) { reinterpret_cast<void(__thiscall*)(CMatrix*, float)>(0x59B1E0)(this, a); }
void CMatrix::SetTranslateOnly(CVector t) { reinterpret_cast<void(__thiscall*)(CMatrix*, float, float, float)>(0x59AF80)(this, t.x, t.y, t.z); }
CMatrix::~CMatrix() { reinterpret_cast<void(__thiscall*)(CMatrix*)>(0x59ACD0)(this); }

// ------------------------------------------------------------------------------------------------ fuente candidata (extraida de Automobile.cpp)
#include "extract.inc"

namespace H {
// ------------------------------------------------------------------------------------------------ generacion del caso
inline const std::vector<int> kModels{ 400, 407, 416, 420, 424, 427, 432, 434, 438, 441, 443, 471, 477, 486, 490, 495, 524, 525, 530, 531, 532, 539, 544, 568, 596, 597, 598, 599, 601, 503, -2 };

struct Case { unsigned n{}; u64 seed{}; bool wild{}; int model{}; u32 eax{}, ecx{}, edx{}, stack{}; u64 fakeSeed{}; };

inline Case MakeCase(unsigned n, unsigned first) {
    const unsigned k = n + first;
    Rng r(0xC0DEull * (k + 1));
    Case c;
    c.n = k;
    c.seed = r.Next() ^ (u64(r.Next()) << 32);
    c.wild = (k % 5) >= 3;
    c.model = kModels[k % kModels.size()];
    c.eax = r.Next(); c.edx = r.Next(); c.stack = r.Next();
    c.fakeSeed = r.Next() | (u64(r.Next()) << 32);
    if (c.fakeSeed == 0) c.fakeSeed = 1;
    return c;
}

inline void FillMatrix(u8* m, Rng& r, bool wild, bool unitary) {
    Gen g{r, wild};
    if (unitary && !wild) {
        const float yaw = r.Range(-3.14f, 3.14f), pitch = r.Range(-0.5f, 0.5f), roll = r.Range(-0.5f, 0.5f);
        const float cy = cosf(yaw), sy = sinf(yaw), cp = cosf(pitch), sp = sinf(pitch), cr = cosf(roll), sr = sinf(roll);
        const float right[3]{cy * cr - sy * sp * sr, sy * cr + cy * sp * sr, -cp * sr};
        const float fwd[3]{-sy * cp, cy * cp, sp};
        const float up[3]{cy * sr + sy * sp * cr, sy * sr - cy * sp * cr, cp * cr};
        memcpy(m + 0x00, right, 12); memcpy(m + 0x10, fwd, 12); memcpy(m + 0x20, up, 12);
    } else {
        for (int i = 0; i < 3; ++i) { Put<float>(m, 0x00 + 4 * i, g.F(-1.f, 1.f)); Put<float>(m, 0x10 + 4 * i, g.F(-1.f, 1.f)); Put<float>(m, 0x20 + 4 * i, g.F(-1.f, 1.f)); }
    }
    for (int i = 0; i < 3; ++i) Put<float>(m, 0x30 + 4 * i, g.F(-60.f, 60.f));
    Put<u32>(m, 0x0C, r.Next()); Put<u32>(m, 0x1C, r.Next()); Put<u32>(m, 0x2C, r.Next()); Put<u32>(m, 0x3C, r.Next());
    Put<u32>(m, 0x40, 0); Put<u32>(m, 0x44, 0);   // sin matriz asociada
}

inline void ApplyWorld(const Case& c) {
    Rng r(c.seed);
    Gen g{r, c.wild};
    const bool wild = c.wild;
    u8* const car = Car();
    // ---- relleno general y guardas
    memset(carBuf.data(), 0xC3, carBuf.size());
    for (size_t off = 0; off < kCarSize; off += 4) Put<float>(car, off, r.Range(-4.f, 4.f));
    memset(towedBuf.data(), 0x6B, towedBuf.size()); memset(otherVehicleBuf.data(), 0x7C, otherVehicleBuf.size());
    memset(handlingBuf.data(), 0, handlingBuf.size());
    for (size_t off = 0; off < 0xD0; off += 4) Put<float>(handlingBuf.data(), off, r.Range(-3.f, 3.f));
    memset(clumpBuf.data(), 0, clumpBuf.size()); Put<u8>(clumpBuf.data(), 0, 2);   // RwObject.type = rpCLUMP
    // ---- entidad
    Put<void*>(car, 0x00, fakeVTable.data());
    Put<void*>(car, 0x14, carMatrixBuf.data());
    Put<u16>(car, 0x22, u16(int16_t(c.model)));
    const u32 statusPick = r.Pick(10);
    const u8 status = statusPick < 2 ? 0 : statusPick < 3 ? 1 : statusPick < 6 ? 2 : statusPick < 8 ? 3 : statusPick == 8 ? 5 : 4;
    Put<u8>(car, 0x36, u8((status << 3) | 2));
    Put<float>(car, 0x04, g.F(-50.f, 50.f)); Put<float>(car, 0x08, g.F(-50.f, 50.f)); Put<float>(car, 0x0C, g.F(-50.f, 50.f)); Put<float>(car, 0x10, g.F(-3.f, 3.f));
    // ---- fisica
    const bool still = r.Chance(1, 6);
    for (int i = 0; i < 3; ++i) {
        Put<float>(car, 0x44 + 4 * i, still ? 0.f : g.F(-3.f, 3.f));
        Put<float>(car, 0x50 + 4 * i, g.F(-1.f, 1.f));
        Put<float>(car, 0x5C + 4 * i, g.F(-1.f, 1.f));
        Put<float>(car, 0x68 + 4 * i, g.F(-1.f, 1.f));
    }
    // ---- vehiculo
    Put<void*>(car, 0x384, handlingBuf.data());
    u32 hflags = r.Next();
    if (r.Chance(1, 3)) hflags |= 0x20; else hflags &= ~0x20u;
    if (r.Chance(1, 3)) hflags |= 0x40; else hflags &= ~0x40u;
    if (r.Chance(1, 2)) hflags |= 0x30000; else if (r.Chance(1, 2)) hflags &= ~0x20000u;
    Put<u32>(car, 0x38C, hflags);
    u32 mflags = r.Next();
    if (r.Chance(1, 2)) mflags |= 0x1000; else mflags &= ~0x1000u;
    if (r.Chance(1, 5)) mflags |= 0x02000000; else mflags &= ~0x02000000u;
    Put<u32>(handlingBuf.data(), 0xCC, mflags);
    Put<float>(handlingBuf.data(), 0xB8, g.F(-0.5f, 0.5f));
    u32 vf0 = r.Next(), vf1 = r.Next();
    auto setBit = [&](u32& w, unsigned bit, bool on) { if (on) w |= 1u << bit; else w &= ~(1u << bit); };
    setBit(vf0, 4, r.Chance(1, 2));     // bEngineOn
    setBit(vf0, 24, r.Chance(2, 3));    // bVehicleColProcessed
    setBit(vf0, 30, r.Chance(1, 8));    // bIsDrowning
    setBit(vf1, 7, r.Chance(1, 8));     // bIsRCVehicle
    setBit(vf1, 15, r.Chance(3, 4));    // bSirenOrAlarm
    setBit(vf1, 23, r.Chance(1, 8));    // bDisableParticles
    Put<u32>(car, 0x428, vf0); Put<u32>(car, 0x42C, vf1);
    Put<u8>(car, 0x486, r.Chance(1, 5) ? u8(r.Next()) : 0); Put<u8>(car, 0x487, r.Chance(1, 5) ? u8(r.Next()) : 0);
    Put<float>(car, 0x494, g.F(-0.8f, 0.8f)); Put<float>(car, 0x498, r.Chance(1, 2) ? 0.f : g.F(-0.8f, 0.8f)); Put<float>(car, 0x49C, r.Chance(1, 4) ? 0.f : g.F(-1.f, 1.f));
    Put<void*>(car, 0x4C8, r.Chance(1, 2) ? static_cast<void*>(towedBuf.data()) : nullptr);
    Put<void*>(towedBuf.data(), 0, fakeVTable.data());
    Put<void*>(otherVehicleBuf.data(), 0, fakeVTable.data());
    static const int kTypes[]{0, 0, 0, 1, 2, 9, 3};
    Put<int32_t>(car, 0x590, kTypes[r.Pick(7)]); Put<int32_t>(car, 0x594, kTypes[r.Pick(7)]);
    // ---- danos (bytes aleatorios; GetWheelStatus del EXE los lee)
    for (size_t off = 0x5A0; off < 0x5A0 + 0x18; ++off) Put<u8>(car, off, u8(r.Next()));
    // ---- nodos
    for (int i = 0; i < 25; ++i) Put<void*>(car, 0x648 + 4 * i, Frame(i));
    auto maybeNull = [&](int i, u32 num, u32 den) { if (r.Chance(num, den)) Put<void*>(car, 0x648 + 4 * i, nullptr); };
    maybeNull(3, 1, 3); maybeNull(6, 1, 3); maybeNull(1, 1, 8); maybeNull(22, 1, 6); maybeNull(23, 1, 6);
    // el EXE desreferencia sin comprobar el nodo 20 (cosechadora) y el 21 (grua sin remolque con panel 21): en esos modelos no puede ser nulo
    if (c.model != 532) maybeNull(20, 1, 6);
    if (c.model != 525) maybeNull(21, 1, 8);
    // ---- paneles, cadera oscilante
    for (int i = 0; i < 3; ++i) {
        const u32 pick = r.Pick(8);
        const int16_t id = pick == 0 ? -1 : pick == 1 ? int16_t(0x8000) : pick == 2 ? 21 : int16_t(r.Pick(25));
        Put<int16_t>(car, 0x6AC + 0x20 * i, id); Put<u16>(car, 0x6AE + 0x20 * i, u16(r.Pick(3)));
    }
    if (c.model == 525 && r.Chance(1, 2)) Put<int16_t>(car, 0x6AC, 21);
    Put<float>(car, 0x70C, g.F(-1.f, 1.f)); Put<float>(car, 0x710, g.F(-1.f, 1.f)); Put<u16>(car, 0x714, u16(r.Next()));
    Put<u8>(car, 0x716, u8(r.Pick(3)));
    Put<u8>(car, 0x717, r.Chance(2, 3) ? 1 : u8(r.Pick(5)));
    Put<float>(car, 0x718, g.F(-1.f, 1.f));
    // ---- ruedas
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) Put<float>(car, 0x724 + 0x2C * i + 4 * j, g.F(-2.f, 2.f));
        Put<u8>(car, 0x724 + 0x2C * i + 0x23, u8(r.Next()));
        const float spring = g.F(0.15f, 0.45f);
        Put<float>(car, 0x878 + 4 * i, spring);
        Put<float>(car, 0x888 + 4 * i, wild && r.Chance(1, 8) ? WildFloat(r) : spring + r.Range(0.02f, 0.3f));
        Put<float>(car, 0x7D4 + 4 * i, r.Chance(1, 3) ? g.F(0.5f, 1.3f) : r.Chance(1, 2) ? 1.0f : g.F(0.f, 1.f));
        Put<float>(car, 0x7E4 + 4 * i, g.F(0.f, 1.2f));
        Put<float>(car, 0x7F4 + 4 * i, r.Chance(1, 4) ? 0.f : g.F(0.f, 3.f));
        Put<u32>(car, 0x810 + 4 * i, r.Pick(7));
        Put<u8>(car, 0x820 + i, r.Chance(1, 2) ? u8(r.Pick(3)) : 0); Put<u8>(car, 0x824 + i, r.Chance(1, 2) ? u8(r.Pick(3)) : 0);
        Put<float>(car, 0x828 + 4 * i, g.F(-7.f, 7.f)); Put<float>(car, 0x838 + 4 * i, g.F(-0.5f, 0.6f)); Put<float>(car, 0x848 + 4 * i, g.F(-5.f, 5.f));
        Put<u32>(car, 0x968 + 4 * i, r.Chance(1, 4) ? 2 : r.Pick(4));
    }
    Put<u8>(car, 0x868, u8(r.Next())); Put<u16>(car, 0x86C, r.Chance(1, 4) ? u16(r.Next()) : u16(r.Pick(5000)));
    for (int i = 0; i < 3; ++i) { Put<float>(car, 0x8B4 + 4 * i, g.F(-1.f, 1.f)); Put<float>(car, 0x8C0 + 4 * i, g.F(-1.f, 1.f)); }
    Put<float>(car, 0x94C, g.F(-1.f, 1.f)); Put<float>(car, 0x950, g.F(-1.f, 1.f)); Put<float>(car, 0x958, r.Chance(1, 3) ? g.F(5.f, 8.f) : g.F(-7.f, 7.f));
    // ---- matrices y marcos
    memset(carMatrixBuf.data(), 0, carMatrixBuf.size()); FillMatrix(carMatrixBuf.data(), r, wild, true);
    memset(cameraMatrixBuf.data(), 0, cameraMatrixBuf.size()); FillMatrix(cameraMatrixBuf.data(), r, wild, true);
    memset(framesBuf.data(), 0x5D, framesBuf.size());
    for (size_t i = 0; i < kFrames; ++i) FillMatrix(Frame(i) + 0x10, r, wild, false);
    // ---- modelo, colision
    memset(modelInfoBuf.data(), 0, modelInfoBuf.size()); memset(forkliftInfoBuf.data(), 0, forkliftInfoBuf.size());
    for (size_t off = 0; off < 0x80; off += 4) Put<float>(modelInfoBuf.data(), off, r.Range(-2.f, 2.f));
    Put<float>(modelInfoBuf.data(), 0x40, g.F(0.15f, 0.6f)); Put<float>(modelInfoBuf.data(), 0x44, g.F(0.15f, 0.6f));
    memcpy(forkliftInfoBuf.data(), modelInfoBuf.data(), 0x80);
    Put<void*>(forkliftInfoBuf.data(), 0x1C, clumpBuf.data());
    memset(colModelBuf.data(), 0, colModelBuf.size()); Put<void*>(colModelBuf.data(), 0x2C, colDataBuf.data());
    memset(colDataBuf.data(), 0, colDataBuf.size()); Put<void*>(colDataBuf.data(), 0x10, linesBuf.data());
    memset(linesBuf.data(), 0x33, linesBuf.size());
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) { Put<float>(linesBuf.data(), 0x20 * i + 4 * j, g.F(-1.5f, 1.5f)); Put<float>(linesBuf.data(), 0x20 * i + 0x10 + 4 * j, g.F(-1.5f, 1.5f)); }
        Put<float>(linesBuf.data(), 0x20 * i + 8, g.F(0.f, 0.6f));
    }
    // ---- tabla de modelos y camara
    auto* const table = reinterpret_cast<void**>(0xA9B0C8);
    for (int m = 380; m < 640; ++m) table[m] = modelInfoBuf.data();
    table[-2] = modelInfoBuf.data(); table[530] = forkliftInfoBuf.data(); table[503] = modelInfoBuf.data();
    auto* const cam = reinterpret_cast<u8*>(kCameraBase);
    Put<float>(cam, 0x04, g.F(-50.f, 50.f)); Put<float>(cam, 0x08, g.F(-50.f, 50.f)); Put<float>(cam, 0x0C, g.F(-50.f, 50.f)); Put<float>(cam, 0x10, g.F(-3.f, 3.f));
    Put<void*>(cam, 0x14, r.Chance(3, 4) ? static_cast<void*>(cameraMatrixBuf.data()) : nullptr);
    // ---- globales (temporizador, paso, brillo de coronas, LOD, area, lluvia, truco de coches flotantes, divisor de velocidad)
    auto pf = [](uintptr_t at, float v) { *reinterpret_cast<float*>(at) = v; };
    *reinterpret_cast<u32*>(0xB7CB84) = r.Chance(1, 8) ? r.Next() : r.Next() & 0xFFFFF;
    pf(0xB7CB5C, wild && r.Chance(1, 5) ? WildFloat(r) : r.Chance(1, 8) ? 0.f : r.Range(0.1f, 3.f));
    pf(0xB7C4E4, wild && r.Chance(1, 4) ? WildFloat(r) : r.Chance(1, 8) ? 0.f : r.Range(0.f, 3.f));
    pf(0xB6F118, wild && r.Chance(1, 5) ? WildFloat(r) : r.Range(0.5f, 2.f));
    *reinterpret_cast<int32_t*>(0xB72914) = r.Chance(4, 5) ? 0 : int32_t(r.Pick(30));
    pf(0xC81324, r.Chance(1, 3) ? r.Range(0.f, 0.02f) : r.Range(0.f, 1.f));
    *reinterpret_cast<u8*>(0x969152) = r.Chance(1, 4) ? 1 : 0;
    pf(0xC1BFF0, wild && r.Chance(1, 6) ? WildFloat(r) : r.Range(0.01f, 0.2f));

    // ---- familia de frontera (1 de cada 11 casos): matrices alineadas con los ejes (productos escalares exactos 0, +-1) y velocidades que dan EXACTAMENTE los umbrales
    //      de la funcion (|v| = 20 de la lluvia, v = 130 del escape, 0.1 de la cosechadora, 0 del Rancher) y +-1 ulp alrededor; lluvia = 0.02f exacto
    if (c.n % 11 == 0) {
        const auto axisMatrix = [&](u8* m, unsigned quarter) {
            static const float cs[4]{1.f, 0.f, -1.f, 0.f}, sn[4]{0.f, 1.f, 0.f, -1.f};
            const float co = cs[quarter & 3], si = sn[quarter & 3];
            const float right[3]{co, si, 0.f}, fwd[3]{-si, co, 0.f}, up[3]{0.f, 0.f, 1.f};
            memcpy(m + 0x00, right, 12); memcpy(m + 0x10, fwd, 12); memcpy(m + 0x20, up, 12);
        };
        const unsigned quarter = r.Pick(4);
        axisMatrix(carMatrixBuf.data(), quarter);
        axisMatrix(cameraMatrixBuf.data(), r.Pick(4));
        const float divisors[3]{1.f, 0.5f, 0.125f};
        const float d = divisors[r.Pick(3)];
        pf(0xC1BFF0, d);
        static const float speeds[]{20.f, -20.f, 130.f, 0.1f, 0.f, 19.999998f, 20.000002f, 129.99998f, 130.00002f, 0.099999994f, 0.10000001f};
        const float v = speeds[r.Pick(sizeof(speeds) / sizeof(speeds[0]))] * d;
        const float* fwd = reinterpret_cast<const float*>(carMatrixBuf.data() + 0x10);
        Put<float>(car, 0x44, fwd[0] * v); Put<float>(car, 0x48, fwd[1] * v); Put<float>(car, 0x4C, 0.f);
        if (r.Chance(1, 2)) pf(0xC81324, 0.02f);
        // sin giro de la camara en la frontera: el producto escalar de la camara y el coche es 0 o +-1
        if (r.Chance(1, 2)) Put<void*>(cam, 0x14, cameraMatrixBuf.data());
        Put<float>(car, 0x494, r.Chance(1, 2) ? 0.f : g.F(-0.8f, 0.8f));
    }
}

// ------------------------------------------------------------------------------------------------ x87, llamada con medicion de ABI
inline void SaveFP(void* p) { __asm { mov eax, p } __asm { fnsave [eax] } __asm { frstor [eax] } }
inline void RestoreFP(void* p) { __asm { mov eax, p } __asm { frstor [eax] } }
inline u32 ReadMXCSR() { u32 v{}; __asm { stmxcsr v } return v; }
alignas(16) inline std::array<u8, 108> savedFP{};
inline u32 savedMXCSR{};
inline void PrimeX87() {
    static const double stale[8]{1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    const double* p = stale;
    __asm { mov eax, p } __asm { fld qword ptr [eax] } __asm { fld qword ptr [eax+8] } __asm { fld qword ptr [eax+16] } __asm { fld qword ptr [eax+24] }
    __asm { fld qword ptr [eax+32] } __asm { fld qword ptr [eax+40] } __asm { fld qword ptr [eax+48] } __asm { fld qword ptr [eax+56] }
    __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) }
}
inline u32 FpWord(const std::array<u8, 108>& fp, size_t at) { u32 v{}; memcpy(&v, fp.data() + at, 4); return v; }
inline std::array<u8, 108> NormalizeFp(std::array<u8, 108> fp) {
    u16 fsw{}; memcpy(&fsw, fp.data() + 4, 2); fsw &= 0x3800; memcpy(fp.data() + 4, &fsw, 2);
    memset(fp.data() + 2, 0, 2); memset(fp.data() + 6, 0, 2); memset(fp.data() + 10, 0, 2); memset(fp.data() + 12, 0, 16);
    const u32 ftw = FpWord(fp, 8) & 0xFFFF, top = (FpWord(fp, 4) >> 11) & 7;
    for (unsigned st = 0; st < 8; ++st) if (((ftw >> (2 * ((top + st) & 7))) & 3) == 3) memset(fp.data() + 28 + 10 * st, 0, 10);
    return fp;
}
struct Reg { u32 eax, ecx, edx, eflags; bool stack, nonvolatile; };
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress, u32 seedEax, u32 seedEcx, u32 seedEdx, u32 stackSeed) {
    u32 espBefore{}, espAfter{}, outEax{}, outEcx{}, outEdx{}, outFlags{}, ebxAfter{}, esiAfter{}, ediAfter{}, ebpBefore{}, ebpAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore, ebp
        mov edx, stackSeed
        xor esi, esi
        mov eax, esp
        mov ecx, 64
    seedloop:
        sub eax, 4
        add esi, 9E3779B1h
        mov edi, edx
        xor edi, esi
        mov [eax], edi
        dec ecx
        jnz seedloop
        mov espBefore, esp
        mov ebx, 13579BDFh
        mov esi, 2468ACE0h
        mov edi, 10293847h
        mov eax, seedEax
        mov ecx, seedEcx
        mov edx, seedEdx
        call entryAddress
        mov outEax, eax
        mov outEcx, ecx
        mov outEdx, edx
        pushfd
        pop eax
        mov outFlags, eax
        mov espAfter, esp
        mov ebxAfter, ebx
        mov esiAfter, esi
        mov ediAfter, edi
        mov ebpAfter, ebp
        pop edi
        pop esi
        pop ebx
    }
    return {outEax, outEcx, outEdx, outFlags, espBefore == espAfter, ebxAfter == 0x13579BDFu && esiAfter == 0x2468ACE0u && ediAfter == 0x10293847u && ebpBefore == ebpAfter};
}
static void __fastcall DllEntry(CAutomobile* self, int) { self->CAutomobile::PreRender(); }

// ------------------------------------------------------------------------------------------------ resultado de un lado
struct Side {
    Reg reg{};
    std::array<u8, 108> fp{};
    u32 mxcsr{};
    std::vector<u32> trace;
    std::vector<std::pair<size_t, u32>> calls;
    std::vector<u8> memory;
};
inline std::vector<u8> Snapshot() {
    std::vector<u8> out;
    for (const auto& a : Arenas()) out.insert(out.end(), a.base, a.base + a.size);
    // globales que la funcion puede escribir o que el banco fija
    for (const uintptr_t at : {0xB7CB84u, 0xB7CB5Cu, 0xB7C4E4u, 0xB6F118u, 0xB72914u, 0xC81324u, 0x969152u, 0xC1BFF0u}) out.insert(out.end(), reinterpret_cast<u8*>(at), reinterpret_cast<u8*>(at) + 4);
    return out;
}
inline const char* CallName(u32 id) {
    static const char* const names[kIdCount]{"", "BasePreRender", "DoHover", "DoBurst", "TowHitch", "TowBar", "ColModel", "AddSingleWheel", "CamNoRain", "PlayerNoRain", "WaterSplash",
        "Exhaust", "DamagedParticles", "Corona", "AddLight", "Appearance", "VehicleLights", "FindPlayerVehicle", "LookingForward", "StoreShadow", "SetComponentRotation",
        "ProcessWheelRotation", "UpdateWheelMatrix", "SwingingDoor", "ProcessPanel", "DoorProcess", "GetWheelPosn", "GetFrameFromId", "SetTransmissionRotation", "BadVirtual"};
    return id < kIdCount ? names[id] : "?";
}
inline Side RunSide(const Case& c, bool candidate) {
    ApplyWorld(c);
    trace.clear();
    callStarts.clear();
    fakeState = c.fakeSeed;
    PrimeX87();
    _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
    Side s;
    s.reg = Gate(candidate ? reinterpret_cast<uintptr_t>(&DllEntry) : 0x6AAB50u, c.eax, u32(reinterpret_cast<uintptr_t>(Car())), c.edx, c.stack);
    SaveFP(s.fp.data());
    s.mxcsr = ReadMXCSR() & 0xFFC0u;
    s.fp = NormalizeFp(s.fp);
    s.trace = trace;
    s.calls = callStarts;
    s.memory = Snapshot();
    // x87 limpio para el siguiente caso
    RestoreFP(savedFP.data());
    return s;
}
inline std::string HexBytes(const u8* p, size_t n) { std::string o; char b[4]; for (size_t i = 0; i < n; ++i) { sprintf(b, "%02x", p[i]); o += b; } return o; }
// Descripcion de la primera diferencia de memoria
inline std::string FirstMemoryDifference(const std::vector<u8>& a, const std::vector<u8>& b) {
    size_t pos = 0;
    for (const auto& ar : Arenas()) {
        for (size_t i = 0; i < ar.size; ++i) {
            if (a[pos + i] != b[pos + i]) {
                char buf[160];
                size_t off = i;
                if (ar.base == carBuf.data()) off = i >= kGuard ? i - kGuard : i;
                sprintf(buf, "%s+0x%zX original=%02x candidato=%02x%s", ar.name, off, a[pos + i], b[pos + i], (ar.base == carBuf.data() && (i < kGuard || i >= kGuard + kCarSize)) ? " (GUARDA)" : "");
                return buf;
            }
        }
        pos += ar.size;
    }
    return a.size() == b.size() ? "globales" : "tamano";
}
}  // namespace H

namespace H {
inline unsigned currentCase{};
inline int currentSide{-1};
inline LONG WINAPI CrashFilter(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION 0x%08lx en %p (caso %u, lado %s); acceso a %p\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, currentCase,
           currentSide == 0 ? "original/EXE" : currentSide == 1 ? "candidato/DLL" : "?", e->ExceptionRecord->NumberParameters > 1 ? reinterpret_cast<void*>(e->ExceptionRecord->ExceptionInformation[1]) : nullptr);
    printf("  EIP=%08lx ESP=%08lx EAX=%08lx ECX=%08lx EDX=%08lx ESI=%08lx EDI=%08lx EBP=%08lx\n", e->ContextRecord->Eip, e->ContextRecord->Esp, e->ContextRecord->Eax,
           e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi, e->ContextRecord->Ebp);
    fflush(stdout);
    ExitProcess(9);
    return EXCEPTION_CONTINUE_SEARCH;
}
}  // namespace H

namespace H {
// Autoprueba de la CRT: _CIpow del EXE (0x822130, x87 con ST1 = base, ST0 = exponente) frente a ExeCompatiblePowCpp y _CIasin del EXE (0x821E70) frente a ExeAsinDoubleCpp
inline double ExePow(double base, double exponent) {
    double r;
    const double* pb = &base; const double* pe = &exponent; double* pr = &r;
    __asm {
        mov eax, pb
        fld qword ptr [eax]
        mov eax, pe
        fld qword ptr [eax]
        mov eax, 0x822130
        call eax
        mov eax, pr
        fstp qword ptr [eax]
    }
    return r;
}
inline double ExeAsin(double x) {
    double r;
    const double* px = &x; double* pr = &r;
    __asm {
        mov eax, px
        fld qword ptr [eax]
        mov eax, 0x821E70
        call eax
        mov eax, pr
        fstp qword ptr [eax]
    }
    return r;
}
inline void SelfTestMath(unsigned n) {
    Rng r(777);
    unsigned powBad = 0, asinBad = 0;
    for (unsigned i = 0; i < n; ++i) {
        _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM);
        const double t = double(r.Range(0.01f, 3.f));
        const double a = ExePow(double(0.95f), t), b = ExeCompatiblePowCpp(double(0.95f), t);
        if (memcmp(&a, &b, 8)) { if (powBad++ < 5) printf("  pow(0.95, %.9g): exe=%.17g dll=%.17g\n", t, a, b); }
        const double x = double(r.Range(-1.f, 1.f));
        const double c = ExeAsin(x), d = ExeAsinDoubleCpp(x);
        if (memcmp(&c, &d, 8)) { if (asinBad++ < 5) printf("  asin(%.9g): exe=%.17g dll=%.17g\n", x, c, d); }
        PrimeX87();
        RestoreFP(savedFP.data());
    }
    printf("autoprueba: pow %u/%u distintos, asin %u/%u distintos\n", powBad, n, asinBad, n);
}
}  // namespace H

int main(int argc, char** argv) {
    using namespace H;
    setvbuf(stdout, nullptr, _IONBF, 0);
    AddVectoredExceptionHandler(1, CrashFilter);
    const unsigned cases = argc > 1 ? unsigned(atoi(argv[1])) : 8192;
    const unsigned first = argc > 2 ? unsigned(atoi(argv[2])) : 0;
    const char* exe = argc > 3 ? argv[3] : "D:\\Grand Theft Auto San Andreas\\gta-sa.exe";
    if (!LoadExe(exe)) return 2;
    InstallFakes();
    SaveFP(savedFP.data());
    savedMXCSR = ReadMXCSR();
    if (argc > 4 && !strcmp(argv[4], "selftest")) { SelfTestMath(20000); return 0; }
    const int verboseCase = argc > 5 ? atoi(argv[5]) : -1;
    unsigned failures = 0, shown = 0;
    std::map<std::string, unsigned> causes;
    for (unsigned n = 0; n < cases; ++n) {
        const Case c = MakeCase(n, first);
        currentCase = c.n;
        currentSide = 0;
        Side a = RunSide(c, false);
        currentSide = 1;
        Side b = RunSide(c, true);
        currentSide = -1;
        std::vector<std::string> why;
        if (a.memory != b.memory) why.push_back("memoria: " + FirstMemoryDifference(a.memory, b.memory));
        if (a.trace != b.trace) {
            size_t i = 0;
            while (i < a.trace.size() && i < b.trace.size() && a.trace[i] == b.trace[i]) ++i;
            char buf[220];
            u32 callId = 0; size_t callAt = 0;
            for (const auto& cs : a.calls) if (cs.first <= i) { callId = cs.second; callAt = cs.first; }
            sprintf(buf, "traza: primera diferencia en %zu (llamada %s +%zu; orig %zu valores, cand %zu) orig=%08x cand=%08x", i, CallName(callId), i - callAt, a.trace.size(), b.trace.size(),
                    i < a.trace.size() ? a.trace[i] : 0xDEAD, i < b.trace.size() ? b.trace[i] : 0xDEAD);
            why.push_back(buf);
        }
        for (const Side* s : {&a, &b}) {
            const char* who = s == &a ? "orig" : "cand";
            if (!s->reg.stack) why.push_back(std::string("ESP ") + who);
            if (!s->reg.nonvolatile) why.push_back(std::string("no volatiles ") + who);
            if (s->reg.eflags & 0x400u) why.push_back(std::string("DF ") + who);
        }
        if (a.fp != b.fp) why.push_back("x87 normalizado");
        if (a.mxcsr != b.mxcsr) why.push_back("MXCSR");
        if (verboseCase >= 0 && c.n == unsigned(verboseCase)) {
            printf("--- detalle del caso %u: traza original (%zu) / candidato (%zu)\n", c.n, a.trace.size(), b.trace.size());
            ApplyWorld(c);
            { const float* fw = reinterpret_cast<const float*>(carMatrixBuf.data() + 0x10); const float* cp1 = reinterpret_cast<const float*>(Car() + 0x750); const float* cp3 = reinterpret_cast<const float*>(Car() + 0x7A8);
              printf("  fwd=(%.9g %.9g %.9g) [%08x %08x %08x] cp1=(%.9g %.9g %.9g) [%08x %08x %08x] cp3=(%.9g %.9g %.9g)\n", fw[0], fw[1], fw[2], BitsOf(fw[0]), BitsOf(fw[1]), BitsOf(fw[2]), cp1[0], cp1[1], cp1[2], BitsOf(cp1[0]), BitsOf(cp1[1]), BitsOf(cp1[2]), cp3[0], cp3[1], cp3[2]); }
            static const char* const callNames[kIdCount]{"", "BasePreRender", "DoHover", "DoBurst", "TowHitch", "TowBar", "ColModel", "AddSingleWheel", "CamNoRain", "PlayerNoRain", "WaterSplash",
                "Exhaust", "DamagedParticles", "Corona", "AddLight", "Appearance", "VehicleLights", "FindPlayerVehicle", "LookingForward", "StoreShadow", "SetComponentRotation",
                "ProcessWheelRotation", "UpdateWheelMatrix", "SwingingDoor", "ProcessPanel", "DoorProcess", "GetWheelPosn", "GetFrameFromId", "SetTransmissionRotation", "BadVirtual"};
            size_t next = 0;
            for (size_t i = 0; i < std::max(a.trace.size(), b.trace.size()); ++i) {
                const char* name = "";
                while (next < a.calls.size() && a.calls[next].first < i) ++next;
                if (next < a.calls.size() && a.calls[next].first == i) name = callNames[a.calls[next].second];
                printf("%4zu  %08x  %08x  %s%s\n", i, i < a.trace.size() ? a.trace[i] : 0u, i < b.trace.size() ? b.trace[i] : 0u, name,
                       i < a.trace.size() && i < b.trace.size() && a.trace[i] != b.trace[i] ? "   <---" : "");
            }
        }
        if (!why.empty()) {
            ++failures;
            for (const auto& w : why) {
                const size_t k = w.find("llamada ");
                causes[k == std::string::npos ? w.substr(0, 20) : "traza en " + w.substr(k + 8, w.find(' ', k + 8) - k - 8) + "  (modelo " + std::to_string(c.model) + ")"] += 1;
            }
            if (shown < 25) {
                ++shown;
                printf("FALLO caso %u (semilla de caso %llu, modelo %d, wild=%d)\n", c.n, c.seed, c.model, c.wild);
                for (const auto& w : why) printf("    %s\n", w.c_str());
            }
        }
    }
    printf("\nCasos: %u  fallos: %u\n", cases, failures);
    for (const auto& kv : causes) printf("  %5u  %s\n", kv.second, kv.first.c_str());
    static const char* const names[kIdCount]{"", "BasePreRender", "DoHover", "DoBurst", "TowHitch", "TowBar", "ColModel", "AddSingleWheel", "CamNoRain", "PlayerNoRain", "WaterSplash",
        "Exhaust", "DamagedParticles", "Corona", "AddLight", "Appearance", "VehicleLights", "FindPlayerVehicle", "LookingForward", "StoreShadow", "SetComponentRotation",
        "ProcessWheelRotation", "UpdateWheelMatrix", "SwingingDoor", "ProcessPanel", "DoorProcess", "GetWheelPosn", "GetFrameFromId", "SetTransmissionRotation", "BadVirtual"};
    printf("Cobertura (llamadas a colaboradores, ambos lados):\n");
    for (unsigned i = 1; i < kIdCount; ++i) printf("  %-24s %8u\n", names[i], idCounts[i]);
    return failures ? 1 : 0;
}
