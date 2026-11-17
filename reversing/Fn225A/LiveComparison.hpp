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
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn225A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 8 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedNamedGlobalInstall, RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas
// propias (A, B, C y D con guardas de 0x40) y con regiones de memoria global del juego sembradas con datos aleatorios por caso (se guardan y se restauran).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F) o solo en la direccion del EXE cuando la DLL los llama por ella
// (FE). Las funciones de D3D9 (bloqueo y desbloqueo de rasters, desbloqueo de la paleta) llaman por la tabla virtual a objetos COM falsos del banco (dispositivo, texturas, superficies) que anotan cada llamada con
// sus argumentos y devuelven los HRESULT del caso; el EXE y la DLL llaman a los MISMOS objetos.
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena), los bytes cambiados de las regiones globales y el estado x87
// (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn225a-crash.txt (junto al EXE).

namespace F225ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn225a-crash.txt";
    char text[2048]{};
    const auto* c = p->ContextRecord;
    int n = wsprintfA(text, "av eip=%08X addr=%08X rw=%u eax=%08X ecx=%08X edx=%08X esi=%08X edi=%08X ebp=%08X esp=%08X\r\nstack:", unsigned(c->Eip), unsigned(p->ExceptionRecord->ExceptionInformation[1]), unsigned(p->ExceptionRecord->ExceptionInformation[0]),
        unsigned(c->Eax), unsigned(c->Ecx), unsigned(c->Edx), unsigned(c->Esi), unsigned(c->Edi), unsigned(c->Ebp), unsigned(c->Esp));
    const unsigned* sp = reinterpret_cast<const unsigned*>(c->Esp);
    for (int i = 0; i < 48 && !IsBadReadPtr(sp + i, 4); ++i) n += wsprintfA(text + n, " %08X", sp[i]);
    HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(c->Eip), &m);
    n += wsprintfA(text + n, "\r\nmodule=%08X\r\n", unsigned(reinterpret_cast<uintptr_t>(m)));
    HANDLE f = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) { DWORD w{}; WriteFile(f, text, DWORD(n), &w, nullptr); CloseHandle(f); }
    return EXCEPTION_CONTINUE_SEARCH;
}
inline const bool installed = [] { AddVectoredExceptionHandler(1, Handler); return true; }();
}


namespace F225ALive {
using namespace F225AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F225AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]; `skip` = desplazamientos (respecto a la ventana) de dwords de vtable, cuyos 4 bytes se omiten
    json Changes(const std::vector<u32>& skip = {}) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            if (m[i] == r[i]) continue;
            const int64 rel = int64(i) - int64(kGuard);
            bool masked = false;
            for (const u32 s : skip) masked |= rel >= int64(s) && rel < int64(s) + 4;
            if (!masked) out.push_back(json::array({rel, m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }
// ================================================================================================ falsos de los colaboradores
inline DWORD gBenchThread{};
inline std::array<u32, 8> gHr{};      // secuencia de HRESULT del caso (a[80..87])
inline u32 gHrN{};
inline u32 NextHr() { return gHr[(gHrN++) & 7u]; }
inline std::array<u32, 6> gChk{};     // mascaras de los comprobadores: 0 textura 0x4CBDE0, 1 objetivo de render 0x4CBE20, 2 textura de profundidad 0x4CBE80, 3 cubo 0x4CBF20, 4 generacion automatica 0x4CBF80, 5 superficie de profundidad 0x4CBD80
inline u32 gChkVar{}, gChkCalls{};
inline u32 ChkResult(unsigned which, u32 fmt) {
    const u32 n = gChkCalls++;
    if (!((gChk[which] >> (fmt & 31u)) & 1u)) return 0;
    static constexpr u32 nz[5]{1u, 2u, 0xFFFFFFFFu, 0x80000000u, 0x100u};
    return nz[(gChkVar + 7u * n) % 5u];
}
inline u32 gNewRaster{}, gImage{}, gRetDestroyRes{}, gRetRebuild{};
inline std::array<u32, 4> gRect{};
inline u32 gNumModes{};
inline std::array<std::array<u32, 4>, 16> gModes{};
inline u32 gLevels{}, gPitch0{}, gPitchStep{}, gHeight0{}, gHeightStep{}, gDescWidth{};
inline std::array<u32, 6> gDescHead{};

// Falsos de las funciones del EXE (la direccion del EXE y el cuerpo de la DLL llevan el mismo, o solo la direccion del EXE si la DLL la llama por ella)
// Comprobadores de formato (cdecl, un argumento): evento [direccion, formato]; el resultado sale de la mascara
template<unsigned W, u32 ADDR> inline u32 __cdecl FkCheck(u32 fmt) { Ev& e = PushEv(ADDR, 1); e.v[0] = fmt; return ChkResult(W, fmt); }
inline void __cdecl FkRasterPush(void* raster) { Ev& e = PushEv(kEvRasterPush, 1); e.v[0] = Tag(raster); }
inline u32 __cdecl FkRasterDestroyRes(u32 a, void* raster, u32 c) { Ev& e = PushEv(kEvRasterDestroyRes, 3); e.v[0] = a; e.v[1] = Tag(raster); e.v[2] = c; return gRetDestroyRes; }
inline u32 __cdecl FkRasterRebuild(u32 a, void* raster, u32 flags) { Ev& e = PushEv(kEvRasterRebuild, 3); e.v[0] = a; e.v[1] = Tag(raster); e.v[2] = flags; return gRetRebuild; }
inline u32 __cdecl FkRasterCreate(u32 w, u32 h, u32 d, u32 flags) { Ev& e = PushEv(kEvRasterCreate, 4); e.v[0] = w; e.v[1] = h; e.v[2] = d; e.v[3] = flags; return gNewRaster; }
inline u32 __cdecl FkRasterDestroy(void* raster) { Ev& e = PushEv(kEvRasterDestroy, 1); e.v[0] = Tag(raster); return 1; }
inline u32 __cdecl FkImageCreate(u32 w, u32 h, u32 d) { Ev& e = PushEv(kEvImageCreate, 3); e.v[0] = w; e.v[1] = h; e.v[2] = d; return gImage; }
inline u32 __cdecl FkImageAlloc(void* image) { Ev& e = PushEv(kEvImageAlloc, 1); e.v[0] = Tag(image); return u32(uintptr_t(image)); }
inline u32 __cdecl FkRasterToImage(void* image, void* parent, u32 z) { Ev& e = PushEv(kEvRasterToImage, 3); e.v[0] = Tag(image); e.v[1] = Tag(parent); e.v[2] = z; return u32(uintptr_t(image)); }
inline u32 __cdecl FkRasterFromImage(void* raster, void* image, u32 z) { Ev& e = PushEv(kEvRasterFromImage, 3); e.v[0] = Tag(raster); e.v[1] = Tag(image); e.v[2] = z; return u32(uintptr_t(raster)); }
inline u32 __cdecl FkImageDestroy(void* image) { Ev& e = PushEv(kEvImageDestroy, 1); e.v[0] = Tag(image); return 1; }

// Objetos COM falsos: el primer dword es la tabla virtual (las funciones llaman por [objeto][ranura]); los eventos anotan el id y no la direccion
struct FakeCom { void** vtbl; u32 id; };
inline std::array<FakeCom, kComCount> gCom{};
inline std::array<void*, 128> gVtDevice{}, gVtD3d{}, gVtTexture{}, gVtCube{}, gVtSurface{}, gVtSwap{};
inline u32 ComIdOf(const void* p) {
    for (u32 i = 0; i < kComCount; ++i) if (static_cast<const void*>(&gCom[i]) == p) return i;
    return u32(uintptr_t(p));
}
inline u32 ComAddr(u32 id) { return u32(uintptr_t(&gCom[id])); }
inline bool HrOk(u32 hr) { return i32(hr) >= 0; }

// Metodos COM (stdcall). Evento [codigo, id del objeto, argumentos...]
inline constexpr u32 kEvCom = 0xC0A00000u;
enum ComEv : u32 { kRelease = 1, kGetLevelCount, kGetLevelDesc, kLockRect, kUnlockRect, kLockRectCube, kUnlockRectCube, kCreateTexture, kCreateCube, kCreateDepth, kSetPalette, kCreateSwap, kGetBackBuffer,
                   kModeCount, kEnumModes, kCheckDeviceFormat, kD3dCreate, kGetClientRect, kTrap = 31 };
inline u32 __stdcall FkRelease(void* self) { Ev& e = PushEv(kEvCom + kRelease, 1); e.v[0] = ComIdOf(self); return 0; }
inline u32 __stdcall FkGetLevelCount(void* self) { Ev& e = PushEv(kEvCom + kGetLevelCount, 1); e.v[0] = ComIdOf(self); return gLevels; }
inline u32 __stdcall FkGetLevelDesc(void* self, u32 level, u32* desc) {
    Ev& e = PushEv(kEvCom + kGetLevelDesc, 3); e.v[0] = ComIdOf(self); e.v[1] = level; e.v[2] = Tag(desc);
    for (u32 i = 0; i < 6; ++i) desc[i] = gDescHead[i];
    desc[6] = gDescWidth; desc[7] = gHeight0 + level * gHeightStep;
    return 0;
}
inline u32 __stdcall FkLockRect(void* self, u32 level, u32* locked, const u32* rect, u32 flags) {
    Ev& e = PushEv(kEvCom + kLockRect, 5); e.v[0] = ComIdOf(self); e.v[1] = level; e.v[2] = Tag(locked); e.v[3] = rect ? 1u : 0u; e.v[4] = flags;
    locked[0] = gPitch0 + level * gPitchStep; locked[1] = 0xB10C0000u + level;
    return 0;
}
inline u32 __stdcall FkUnlockRect(void* self, u32 level) { Ev& e = PushEv(kEvCom + kUnlockRect, 2); e.v[0] = ComIdOf(self); e.v[1] = level; return 0; }
inline u32 __stdcall FkLockRectCube(void* self, u32 face, u32 level, u32* locked, const u32* rect, u32 flags) {
    Ev& e = PushEv(kEvCom + kLockRectCube, 6); e.v[0] = ComIdOf(self); e.v[1] = face; e.v[2] = level; e.v[3] = Tag(locked); e.v[4] = rect ? 1u : 0u; e.v[5] = flags;
    locked[0] = gPitch0 + level * gPitchStep; locked[1] = 0xB10C0000u + level;
    return 0;
}
inline u32 __stdcall FkUnlockRectCube(void* self, u32 face, u32 level) { Ev& e = PushEv(kEvCom + kUnlockRectCube, 3); e.v[0] = ComIdOf(self); e.v[1] = face; e.v[2] = level; return 0; }
inline u32 __stdcall FkDevCreateTexture(void* self, u32 w, u32 h, u32 levels, u32 usage, u32 fmt, u32 pool, u32* out, void* shared) {
    Ev& e = PushEv(kEvCom + kCreateTexture, 9); e.v[0] = ComIdOf(self); e.v[1] = w; e.v[2] = h; e.v[3] = levels; e.v[4] = usage; e.v[5] = fmt; e.v[6] = pool; e.v[7] = Tag(out); e.v[8] = u32(uintptr_t(shared));
    const u32 hr = NextHr();
    *out = HrOk(hr) ? ComAddr(kComTexture) : 0u;
    return hr;
}
inline u32 __stdcall FkDevCreateCube(void* self, u32 edge, u32 levels, u32 usage, u32 fmt, u32 pool, u32* out, void* shared) {
    Ev& e = PushEv(kEvCom + kCreateCube, 8); e.v[0] = ComIdOf(self); e.v[1] = edge; e.v[2] = levels; e.v[3] = usage; e.v[4] = fmt; e.v[5] = pool; e.v[6] = Tag(out); e.v[7] = u32(uintptr_t(shared));
    const u32 hr = NextHr();
    *out = HrOk(hr) ? ComAddr(kComCube) : 0u;
    return hr;
}
inline u32 __stdcall FkDevCreateDepth(void* self, u32 w, u32 h, u32 fmt, u32 ms, u32 msq, u32 discard, u32* out, void* shared) {
    Ev& e = PushEv(kEvCom + kCreateDepth, 9); e.v[0] = ComIdOf(self); e.v[1] = w; e.v[2] = h; e.v[3] = fmt; e.v[4] = ms; e.v[5] = msq; e.v[6] = discard; e.v[7] = Tag(out); e.v[8] = u32(uintptr_t(shared));
    const u32 hr = NextHr();
    *out = HrOk(hr) ? ComAddr(kComSurface0) : 0u;
    return hr;
}
inline u32 __stdcall FkDevSetPalette(void* self, u32 number, const u8* entries) {
    Ev& e = PushEv(kEvCom + kSetPalette, 4); e.v[0] = ComIdOf(self); e.v[1] = number; e.v[2] = Tag(entries); e.v[3] = Fnv(entries, 0x400);
    return NextHr();
}
inline u32 __stdcall FkDevCreateSwap(void* self, const u32* pp, u32* out) {
    Ev& e = PushEv(kEvCom + kCreateSwap, 17); e.v[0] = ComIdOf(self); e.v[1] = Tag(pp); e.v[2] = Tag(out);
    for (u32 i = 0; i < 14; ++i) e.v[3 + i] = pp[i];
    const u32 hr = NextHr();
    *out = HrOk(hr) ? ComAddr(kComSwap) : 0u;
    return hr;
}
inline u32 __stdcall FkSwapGetBackBuffer(void* self, u32 index, u32 type, u32* out) {
    Ev& e = PushEv(kEvCom + kGetBackBuffer, 4); e.v[0] = ComIdOf(self); e.v[1] = index; e.v[2] = type; e.v[3] = Tag(out);
    *out = ComAddr(kComSurface1);
    return NextHr();
}
inline u32 __stdcall FkD3dModeCount(void* self, u32 adapter, u32 fmt) { Ev& e = PushEv(kEvCom + kModeCount, 3); e.v[0] = ComIdOf(self); e.v[1] = adapter; e.v[2] = fmt; return gNumModes; }
inline u32 __stdcall FkD3dEnumModes(void* self, u32 adapter, u32 fmt, u32 index, u32* mode) {
    Ev& e = PushEv(kEvCom + kEnumModes, 5); e.v[0] = ComIdOf(self); e.v[1] = adapter; e.v[2] = fmt; e.v[3] = index; e.v[4] = Tag(mode);
    for (u32 i = 0; i < 4; ++i) mode[i] = index < 16 ? gModes[index][i] : 0u;     // D3DDISPLAYMODE: ancho, alto, frecuencia, formato
    return 0;
}
inline u32 __stdcall FkD3dCheckDeviceFormat(void* self, u32 adapter, u32 devType, u32 adFmt, u32 usage, u32 rtype, u32 fmt) {
    Ev& e = PushEv(kEvCom + kCheckDeviceFormat, 7); e.v[0] = ComIdOf(self); e.v[1] = adapter; e.v[2] = devType; e.v[3] = adFmt; e.v[4] = usage; e.v[5] = rtype; e.v[6] = fmt;
    return NextHr();
}
inline u32 __stdcall FkDirect3DCreate9(u32 sdk) { Ev& e = PushEv(kEvCom + kD3dCreate, 1); e.v[0] = sdk; return ComAddr(kComD3d); }
inline u32 __stdcall FkGetClientRect(void* hwnd, u32* rect) {
    Ev& e = PushEv(kEvCom + kGetClientRect, 2); e.v[0] = u32(uintptr_t(hwnd)); e.v[1] = Tag(rect);
    for (u32 i = 0; i < 4; ++i) rect[i] = gRect[i];
    return 1;
}
template<size_t N> inline void __cdecl FkTrap() { Ev& e = PushEv(kEvCom + kTrap, 1); e.v[0] = u32(N); }
template<size_t... I> inline void FillTraps(std::array<void*, 128>& v, std::index_sequence<I...>) { ((v[I] = reinterpret_cast<void*>(&FkTrap<I>)), ...); }
inline void InitCom() {
    for (auto* v : {&gVtDevice, &gVtD3d, &gVtTexture, &gVtCube, &gVtSurface, &gVtSwap}) FillTraps(*v, std::make_index_sequence<128>{});
    for (auto* v : {&gVtDevice, &gVtD3d, &gVtTexture, &gVtCube, &gVtSurface, &gVtSwap}) (*v)[2] = reinterpret_cast<void*>(&FkRelease);
    gVtDevice[13] = reinterpret_cast<void*>(&FkDevCreateSwap); gVtDevice[23] = reinterpret_cast<void*>(&FkDevCreateTexture); gVtDevice[25] = reinterpret_cast<void*>(&FkDevCreateCube);
    gVtDevice[29] = reinterpret_cast<void*>(&FkDevCreateDepth); gVtDevice[71] = reinterpret_cast<void*>(&FkDevSetPalette);
    gVtD3d[6] = reinterpret_cast<void*>(&FkD3dModeCount); gVtD3d[7] = reinterpret_cast<void*>(&FkD3dEnumModes); gVtD3d[10] = reinterpret_cast<void*>(&FkD3dCheckDeviceFormat);
    gVtTexture[13] = gVtCube[13] = reinterpret_cast<void*>(&FkGetLevelCount); gVtTexture[17] = gVtCube[17] = reinterpret_cast<void*>(&FkGetLevelDesc);
    gVtTexture[19] = reinterpret_cast<void*>(&FkLockRect); gVtTexture[20] = reinterpret_cast<void*>(&FkUnlockRect);
    gVtCube[19] = reinterpret_cast<void*>(&FkLockRectCube); gVtCube[20] = reinterpret_cast<void*>(&FkUnlockRectCube);
    gVtSwap[5] = reinterpret_cast<void*>(&FkSwapGetBackBuffer);
    for (u32 i = 0; i < kComCount; ++i) {
        gCom[i].id = i;
        gCom[i].vtbl = i == kComDevice ? gVtDevice.data() : (i == kComD3d ? gVtD3d.data() : (i == kComTexture ? gVtTexture.data() : (i == kComCube ? gVtCube.data() : (i == kComSwap ? gVtSwap.data() : gVtSurface.data()))));
    }
}

// ================================================================================================ cierre de un lado
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

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

// Memoria global del banco: se rellena la region entera de forma determinista y despues se escriben los campos del caso
inline void FillGlobal(u32 at, u32 bytes, u32 kind, u32 seed) {
    std::vector<u8> tmp(bytes);
    FillKind(tmp.data(), bytes, kind, seed);
    memcpy(reinterpret_cast<void*>(uintptr_t(at)), tmp.data(), bytes);
}
inline void PutG32(u32 at, u32 v) { memcpy(reinterpret_cast<void*>(uintptr_t(at)), &v, 4); }
inline void PutG8(u32 at, u32 v) { *reinterpret_cast<u8*>(uintptr_t(at)) = u8(v); }
inline u32 RegionSeed(const C& c, u32 region) { return c.a[90] + region * 17u; }
inline void FillRegion(const C& c, u32 region) { FillGlobal(kRegAt[region], kRegBytes[region], c.a[2], RegionSeed(c, region)); }
// Regiones que toca cada funcion (indices de kRegAt; la marca del banco, 7, siempre al final)
inline std::vector<size_t> RegionList(Kind k) {
    switch (k) {
    case kFind: case kSetFmt: return {0, 3, 4, 7};
    case kRestore: return {0, 1, 2, 3, 4, 7};
    case kCreate: return {0, 2, 7};
    case kConvert: return {0, 2, 3, 5, 7};
    case kMemSize: return {0, 7};
    default: return {6, 7};
    }
}
inline void SetChecks(const C& c, std::initializer_list<std::pair<unsigned, u32>> masks, u32 var) {
    gChk = {}; gChkCalls = 0; gChkVar = var;
    for (const auto& m : masks) gChk[m.first] = c.a[m.second];
}
// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind k = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gHr = {}; gHrN = 0; gChk = {}; gChkCalls = 0; gChkVar = 0; gNewRaster = gImage = gRetDestroyRes = gRetRebuild = 0; gRect = {}; gNumModes = 0; gModes = {};
    gLevels = gPitch0 = gPitchStep = gHeight0 = gHeightStep = gDescWidth = 0; gDescHead = {};
    for (u32 i = 0; i < 8; ++i) gHr[i] = a[80 + i];
    for (const u32 r : RegionList(k)) if (r != 7) FillRegion(c, r);
    *reinterpret_cast<u8*>(kBenchMarker) = u8(kMarkerValue);   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 X = a[3];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    const auto cdecl_ = [&](u32 n) { call.argCount = n; call.popBytes = 4 * n; };
    switch (k) {
    case kFind: case kSetFmt:
        PutG8(kCapsAt, a[6]); PutG32(kDispAt, a[7]); PutG32(kZAt, a[8]); PutG32(kPpAt + 8, a[9]);
        SetChecks(c, {{0, 10}, {1, 11}, {2, 12}, {3, 13}, {4, 14}, {5, 15}}, a[16]);
        if (k == kFind) { call.args[0] = a[4]; call.args[1] = a[5]; }
        else { call.args[0] = thisA; call.args[1] = a[5]; }
        cdecl_(2);
        break;
    case kRestore: {
        PutG32(kExtOffsetAt, X); PutG32(kZAt, a[5]); PutG32(kPpAt + 8, a[6]); PutG32(kDivXAt, a[7]); PutG32(kDivYAt, a[8]); PutG32(kPpAt, a[9]); PutG32(kPpAt + 4, a[10]);
        PutG32(kPpAt + 0x10, a[11]); PutG32(kPpAt + 0x14, a[12]); PutG32(kHwndAt, a[13]); PutG32(kRenderSurfAt, a[14]); PutG32(kDevAt, ComAddr(kComDevice));
        SetChecks(c, {{5, 15}}, a[16]);
        for (u32 i = 0; i < 4; ++i) gRect[i] = a[25 + i];
        const u32 count = a[4];
        Arena<kWin>* const ar[4]{&arenaA, &arenaB, &arenaC, &arenaD};
        for (u32 n = 0; n < count; ++n) {
            const u32 b = 32 + 14 * n;
            auto& R = *ar[n];
            auto& H = *ar[n + 1];   // arena que guarda la paleta y el nodo
            const u32 ro = a[b + 0];
            const u32 self = u32(R.Obj(ro));
            const u32 parent = a[b + 1] == 0 ? self : (a[b + 1] == 1 ? u32(ar[(n + 1) % 4]->Obj(0x10)) : a[b + 12]);
            R.Put32(ro, parent); R.Put32(ro + 0xC, a[b + 4]); R.Put32(ro + 0x10, a[b + 5]); R.Put8(ro + 0x20, a[b + 2]); R.Put8(ro + 0x23, a[b + 3]);
            const u32 e = ro + X;
            R.Put32(e + 0, a[b + 7] == 0 ? 0u : (a[b + 7] == 1 ? ComAddr(kComSurface0) : 0x0BADF00Du));
            R.Put32(e + 4, a[b + 8] ? u32(H.Obj(a[b + 13])) : 0u);
            R.Put8(e + 9, a[b + 9]); R.Put8(e + 0xA, a[b + 10]); R.Put32(e + 0x18, a[b + 6]); R.Put32(e + 0x1C, a[b + 11]); R.Put32(e + 0x20, a[b + 12]);
            H.Put32(a[74 + n], self);
            H.Put32(a[74 + n] + 4, n + 1 < count ? u32(ar[n + 2]->Obj(a[74 + n + 1])) : 0u);
        }
        PutG32(kListAt, count ? u32(arenaB.Obj(a[74])) : 0u);
        cdecl_(0);
        break;
    }
    case kCreate:
        PutG32(kExtOffsetAt, X); PutG32(kDevAt, ComAddr(kComDevice));
        SetChecks(c, {{0, 11}, {1, 12}, {2, 13}, {4, 14}}, a[16]);
        arenaA.Put32(o + 0xC, a[9]); arenaA.Put32(o + 0x10, a[10]);
        gNewRaster = a[8] ? 0u : thisA;
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; call.args[3] = a[7]; cdecl_(4);
        break;
    case kConvert: {
        PutG32(kExtOffsetAt, X); PutG32(kD3dAt, ComAddr(kComD3d)); PutG32(kAdapterAt, a[12]); PutG32(kDevTypeAt, a[13]); PutG32(kAdFmtAt, a[11]); PutG32(kDispAt, a[10]);
        SetChecks(c, {{3, 16}, {1, 17}}, a[18]);
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]);
        arenaA.Put32(o, a[6] == 0 ? thisA : (a[6] == 1 ? u32(arenaB.Obj(a[21])) : a[22]));
        arenaA.Put8(o + 0x20, a[7]); arenaA.Put8(o + X + 8, a[8]); arenaA.Put8(o + X + 9, a[9]);
        gImage = a[14] ? 0u : u32(arenaB.Obj(a[15]));
        gRetDestroyRes = a[19]; gRetRebuild = a[20];
        call.args[0] = thisA; cdecl_(1);
        break;
    }
    case kMemSize: {
        PutG32(kExtOffsetAt, X);
        arenaB.Put32(a[5], a[6] ? 0u : thisA);
        arenaA.Put8(o + 0x23, a[7]);
        const bool cube = (a[8] & 0x0Fu) != 0;
        arenaA.Put32(o + X, cube ? ComAddr(kComCube) : ComAddr(kComTexture));
        arenaA.Put8(o + X + 9, a[8]); arenaA.Put8(o + X + 0xA, a[9]);
        gLevels = a[11]; gPitch0 = a[12]; gPitchStep = a[13]; gHeight0 = a[14]; gHeightStep = a[15];
        for (u32 i = 0; i < 6; ++i) gDescHead[i] = a[16 + i];
        gDescWidth = a[22];
        call.args[0] = u32(arenaC.Obj(a[4])); call.args[1] = u32(arenaB.Obj(a[5])); cdecl_(2);
        break;
    }
    case kForeground:
        PutG32(kForeAt, a[4]);
        cdecl_(0);
        break;
    case kRefresh:
        PutG32(kSelAt, a[7]);
        gNumModes = a[8];
        for (u32 i = 0; i < 16; ++i) for (u32 j = 0; j < 4; ++j) gModes[i][j] = a[16 + 4 * i + j];
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; cdecl_(3);
        break;
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw, rLow8 };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kCreate: return rTag;                                                          // puntero como etiqueta de arena
    case kConvert: return rNone;
    case kForeground: return rLow8;                                                     // bool: solo AL (el EXE deja en EAX el dword completo con el byte bajo reemplazado)
    default: return rRaw;                                                               // EAX completo
    }
}
inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    {
        json fakes = json::array();
        for (u32 i = 0; i < kComCount; ++i) fakes.push_back(ComAddr(i));   // direcciones de los objetos COM falsos (el oraculo las necesita para los punteros que el banco escribe en memoria)
        input["fakes"] = fakes;
    }
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0; gThrew = false;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["vptrs"] = json::array();
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(kKind[fn])) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case rRaw: obs["ret"] = eax; break;                  // valor crudo
        case rLow8: obs["ret"] = eax & 0xFFu; break;         // solo AL
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN225A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN225A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
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

// Casilla de la IAT de un modulo para una funcion importada por nombre (la DLL llama a GetClientRect y a Direct3DCreate9 por su IAT, el EXE por la suya)
inline uintptr_t* FindImportSlot(uintptr_t module, const char* func) {
    const auto* base = reinterpret_cast<const uint8*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    const auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress) return nullptr;
    for (const auto* d = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; ++d) {
        const auto* names = reinterpret_cast<const IMAGE_THUNK_DATA32*>(base + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* slots = reinterpret_cast<IMAGE_THUNK_DATA32*>(const_cast<uint8*>(base) + d->FirstThunk);
        for (; names->u1.AddressOfData; ++names, ++slots) {
            if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG32) continue;
            const auto* by = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(base + names->u1.AddressOfData);
            if (!std::strcmp(reinterpret_cast<const char*>(by->Name), func)) return reinterpret_cast<uintptr_t*>(&slots->u1.Function);
        }
    }
    return nullptr;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn225a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn225a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn225a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    InitCom();
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) {
        for (unsigned f = 0; f < kFunctions; ++f) collaborators[kFn[f]] = {{"own_hook", ownHooks[kFn[f]]}, {"body", env.route[f] ? env.route[f]->Body() : 0}, {"body_module", env.route[f] ? ModuleOf(env.route[f]->Body()) : 0}};
        Run(env, false, registered, bodies, collaborators);
        return;
    }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    auto Collab = [&](uintptr_t exe) -> Pair {
        auto it = collabCache.find(exe);
        if (it != collabCache.end()) return it->second;
        Pair p{exe, 0, false};
        for (unsigned f = 0; f < kFunctions; ++f)
            if (kEntries[f] == exe && env.route[f]) { p.dll = Body(env.route[f]->Body()); p.hooked = true; hooksFound.push_back({{"exe", exe}, {"tested_function", kFn[f]}}); }
        if (!p.hooked) p = MakeCollab(env, exe, hooksFound);
        return collabCache.emplace(exe, p).first->second;
    };
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador que la DLL llama POR LA DIRECCION DEL EXE (con o sin hook RH, que queda apagado): el falso va solo en la direccion del EXE
    auto FE = [&](uintptr_t exe, auto* fake) {
        Collab(exe);
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL que es funcion con hook (hook apagado): el EXE corre sus bytes y la DLL llama a su cuerpo
    auto R = [&](uintptr_t exe) -> Pair {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;
        return p;
    };
    const auto ChkStubs = [&]() -> std::vector<StubSpec> {
        return {FE(kEvCheckTex, &FkCheck<0, kEvCheckTex>), FE(kEvCheckRt, &FkCheck<1, kEvCheckRt>), FE(kEvCheckDepthTex, &FkCheck<2, kEvCheckDepthTex>), FE(kEvCheckCubeRt, &FkCheck<3, kEvCheckCubeRt>),
                FE(kEvCheckAutoGen, &FkCheck<4, kEvCheckAutoGen>), FE(kEvCheckDepthSurf, &FkCheck<5, kEvCheckDepthSurf>)};
    };
    env.stubs[0] = ChkStubs();
    env.stubs[1] = ChkStubs();
    env.reals[1] = {R(0x4CC1D0)};
    env.stubs[2] = {FE(kEvCheckDepthSurf, &FkCheck<5, kEvCheckDepthSurf>)};
    env.stubs[3] = {F(kEvRasterCreate, &FkRasterCreate), F(kEvRasterDestroy, &FkRasterDestroy), FE(kEvCheckTex, &FkCheck<0, kEvCheckTex>), FE(kEvCheckRt, &FkCheck<1, kEvCheckRt>),
                    FE(kEvCheckDepthTex, &FkCheck<2, kEvCheckDepthTex>), FE(kEvCheckAutoGen, &FkCheck<4, kEvCheckAutoGen>), FE(kEvRasterPush, &FkRasterPush)};
    env.stubs[4] = {F(kEvImageCreate, &FkImageCreate), F(kEvImageAlloc, &FkImageAlloc), F(kEvImageDestroy, &FkImageDestroy), FE(kEvRasterToImage, &FkRasterToImage), FE(kEvRasterFromImage, &FkRasterFromImage),
                    FE(kEvRasterDestroyRes, &FkRasterDestroyRes), FE(kEvRasterRebuild, &FkRasterRebuild), FE(kEvCheckCubeRt, &FkCheck<3, kEvCheckCubeRt>), FE(kEvCheckRt, &FkCheck<1, kEvCheckRt>)};
    // IAT: GetClientRect (funcion 2) y Direct3DCreate9 (funcion 7): el EXE llama por su casilla (0x858304) y por el salto de 0x807C2B, la DLL por la suya
    uintptr_t* const exeGcr = reinterpret_cast<uintptr_t*>(0x858304);
    uintptr_t* const dllGcr = FindImportSlot(env.asi, "GetClientRect");
    const uint8* const d3dThunk = reinterpret_cast<const uint8*>(0x807C2B);
    uintptr_t* const exeD3d = d3dThunk[0] == 0xFF && d3dThunk[1] == 0x25 ? *reinterpret_cast<uintptr_t* const*>(d3dThunk + 2) : nullptr;
    uintptr_t* const dllD3d = FindImportSlot(env.asi, "Direct3DCreate9");
    json iatInfo = {{"gcr_exe_slot", uintptr_t(exeGcr)}, {"gcr_dll_slot_found", dllGcr != nullptr}, {"gcr_same", dllGcr && *dllGcr == *exeGcr}, {"d3d_exe_slot", uintptr_t(exeD3d)}, {"d3d_dll_slot_found", dllD3d != nullptr},
                    {"d3d_same", exeD3d && dllD3d && *dllD3d == *exeD3d}, {"d3d_thunk", HexBytes(d3dThunk, 6)}};
    if (!dllGcr || !*exeGcr || !*dllGcr || !exeD3d || !dllD3d || !*exeD3d || !*dllD3d) ready = false;
    else {
        env.iat[2] = {IatSpec{exeGcr, reinterpret_cast<void*>(&FkGetClientRect)}, IatSpec{dllGcr, reinterpret_cast<void*>(&FkGetClientRect)}};
        env.iat[7] = {IatSpec{exeD3d, reinterpret_cast<void*>(&FkDirect3DCreate9)}, IatSpec{dllD3d, reinterpret_cast<void*>(&FkDirect3DCreate9)}};
        env.watches.emplace_back(uintptr_t(exeGcr), 4); env.watches.emplace_back(uintptr_t(exeD3d), 4); env.watches.emplace_back(0x807C2B, 6);
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    dllReal[0] = "toda la funcion; los seis comprobadores de formato (0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF20, 0x4CBF80, 0x4CBD80) son falsos solo en la direccion del EXE (la DLL los llama por ella)";
    dllReal[1] = "toda la funcion; FindCorrectRasterFormat 0x4CC1D0 corre real (el EXE sus bytes, la DLL su cuerpo) con los seis comprobadores falsos solo en la direccion del EXE";
    dllReal[2] = "toda la funcion; el dispositivo, la cadena de intercambio y las superficies de D3D9 son objetos COM falsos del banco; GetClientRect (IAT del EXE 0x858304 y de la DLL) es falso; 0x4CBD80 falso solo en la direccion del EXE";
    dllReal[3] = "toda la funcion salvo RwRasterCreate 0x7FB230 y RwRasterDestroy 0x7FB020 (falsos identicos), 0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF80 y 0x4CB530 (falsos solo en la direccion del EXE); el dispositivo de D3D9 es un objeto COM falso";
    dllReal[4] = "toda la funcion salvo RwImageCreate 0x8026E0, RwImageAllocatePixels 0x8027A0 y RwImageDestroy 0x802740 (falsos identicos), 0x7FF270, 0x8001E0, 0x4CBB00, 0x4CCE60, 0x4CBF20 y 0x4CBE20 (falsos solo en la direccion del EXE); el ayudante usercall 0x4CC0C0 corre real en el EXE (su version local en la DLL) sobre un IDirect3D9 falso";
    dllReal[5] = "toda la funcion (sin colaboradores); la textura y el cubo de D3D9 son objetos COM falsos del banco";
    dllReal[6] = "toda la funcion (sin colaboradores)";
    dllReal[7] = "toda la funcion; Direct3DCreate9 (casilla de la IAT del EXE por el salto de 0x807C2B y de la DLL) devuelve un IDirect3D9 falso";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", (f == 2 || f == 7) ? iatInfo : json(nullptr)}};
    }
    // Memoria global del juego tocada (8 regiones; ver Fixtures.hpp): se guardan al empezar y se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (size_t r = 0; r < kRegAt.size(); ++r) regions.emplace_back(kRegAt[r], kRegBytes[r]);
    for (unsigned f = 0; f < kFunctions; ++f) for (const auto r : RegionList(kKind[f])) regionsOf[f].push_back(r);
    Run(env, ready, registered, bodies, collaborators);
}
}
