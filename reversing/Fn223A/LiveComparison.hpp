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

// Banco par a par Fn223A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedNamedGlobalInstall, RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas
// propias (A, B, C y D con guardas de 0x40) y con regiones de memoria global del juego sembradas con datos aleatorios por caso (se guardan y se restauran).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F) o solo en la direccion del EXE cuando la DLL los llama por ella
// (FE). Las funciones de D3D9 (bloqueo y desbloqueo de rasters, desbloqueo de la paleta) llaman por la tabla virtual a objetos COM falsos del banco (dispositivo, texturas, superficies) que anotan cada llamada con
// sus argumentos y devuelven los HRESULT del caso; el EXE y la DLL llaman a los MISMOS objetos.
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena), los bytes cambiados de las regiones globales y el estado x87
// (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn223a-crash.txt (junto al EXE).

namespace F223ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn223a-crash.txt";
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


namespace F223ALive {
using namespace F223AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F223AFix::Arena(WIN);
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
inline u32 gRet{};                   // EAX que devuelven los falsos de Flush y de RwRasterRegisterPlugin
inline u32 gW0{}, gW1{};             // dwords que dejan los falsos en la memoria del objeto
inline u32 gCount{};                 // numero de llamada de los falsos del CDecision
inline DWORD gBenchThread{};
inline u32 gIdxRet{};                // indice que devuelve el falso de GetActualScriptThingIndex
inline u32 gHrGsl{}, gGslSel{}, gHrLock{}, gPitch{}, gBits{}, gHrCreate{}, gCreateSel{};   // comportamiento de los objetos COM falsos
inline std::array<u32, 8> gDesc{};   // D3DSURFACE_DESC que devuelve GetDesc

// Objetos COM falsos: el primer dword es la tabla virtual (las funciones llaman por [objeto][ranura]); los eventos anotan el id y no la direccion
struct FakeCom { void** vtbl; u32 id; };
inline std::array<FakeCom, kComCount> gCom{};
inline std::array<void*, 128> gVtDevice{}, gVtTexture{}, gVtCube{}, gVtSurface{};
inline u32 ComIdOf(const void* p) {
    for (u32 i = 0; i < kComCount; ++i) if (static_cast<const void*>(&gCom[i]) == p) return i;
    return u32(uintptr_t(p));
}
inline u32 ComAddr(u32 id) { return u32(uintptr_t(&gCom[id])); }
inline bool HrOk(u32 hr) { return i32(hr) >= 0; }

// Falsos de las funciones con hook (la direccion del EXE y el cuerpo de la DLL llevan el mismo)
// CEntryInfoList::Flush (0x536E10; thiscall sin pila): evento [direccion, ECX (etiqueta), dword de [ECX]]; deja gW0 en [ECX]; devuelve gRet.
inline u32 __fastcall FkFlush(void* self, void*) {
    Ev& e = PushEv(kEvFlush, 2); e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    memcpy(self, &gW0, 4);
    return gRet;
}
// CDecision::CDecision (0x6040C0) y CDecision::SetDefault (0x600530): thiscall sin pila; evento [direccion, ECX (etiqueta)]; dejan (base xor numero de llamada) en [ECX] y [ECX + 4].
inline u32 __fastcall FkDecCtor(void* self, void*) {
    Ev& e = PushEv(kEvDecCtor, 1); e.v[0] = Tag(self);
    const u32 w = gW0 ^ gCount++;
    memcpy(self, &w, 4);
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkDecSet(void* self, void*) {
    Ev& e = PushEv(kEvDecSet, 1); e.v[0] = Tag(self);
    const u32 w = gW1 ^ gCount++;
    memcpy(static_cast<u8*>(self) + 4, &w, 4);
    return u32(uintptr_t(self));
}
// CPhysical::AddToMovingList (0x542800; thiscall sin pila): evento [direccion, ECX (etiqueta)].
inline void __fastcall FkMoving(void* self, void*) {
    Ev& e = PushEv(kEvMoving, 1); e.v[0] = Tag(self);
}
// CScriptsForBrains::Init (0x46A8C0; thiscall sin pila): evento [direccion, ECX (etiqueta)]; deja gW0 en [ECX].
inline void __fastcall FkBrainsInit(void* self, void*) {
    Ev& e = PushEv(kEvBrainsInit, 1); e.v[0] = Tag(self);
    memcpy(self, &gW0, 4);
}
// CTask::Destructor (0x61A3A0; thiscall sin pila): evento [direccion, ECX (etiqueta)]; deja gW0 en [ECX].
inline u32 __fastcall FkTaskDtor(void* self, void*) {
    Ev& e = PushEv(kEvTaskDtor, 1); e.v[0] = Tag(self);
    memcpy(self, &gW0, 4);
    return u32(uintptr_t(self));
}
// CTask::operator delete (0x61A5B0; cdecl, un puntero): evento [direccion, argumento (etiqueta), dword de [argumento]]. No libera nada.
inline void __cdecl FkTaskDelete(void* p) {
    Ev& e = PushEv(kEvTaskDelete, 2); e.v[0] = Tag(p); e.v[1] = BitsOf(p);
}
// CTheScripts::GetActualScriptThingIndex (0x4839A0; cdecl, (referencia, tipo)): evento [direccion, referencia, tipo]; devuelve gIdxRet.
inline u32 __cdecl FkThingIdx(u32 handle, u32 type) {
    Ev& e = PushEv(kEvThingIdx, 2); e.v[0] = handle; e.v[1] = type;
    return gIdxRet;
}
// CEntity::CleanUpOldReference (0x571A00) y CEntity::RegisterReference (0x571B70): thiscall con un puntero de pila; evento [direccion, ECX (crudo), referencia (crudo)].
inline void __fastcall FkCleanUp(void* self, void*, void* ref) {
    Ev& e = PushEv(kEvCleanUp, 2); e.v[0] = u32(uintptr_t(self)); e.v[1] = u32(uintptr_t(ref));
}
inline void __fastcall FkRegRef(void* self, void*, void* ref) {
    Ev& e = PushEv(kEvRegRef, 2); e.v[0] = u32(uintptr_t(self)); e.v[1] = u32(uintptr_t(ref));
}
// CBaseModelInfo::Constructor (cuerpo, 0x4C4A60; thiscall sin pila; la DLL lo llama con EDX nulo): evento [direccion, ECX (etiqueta)]; deja gW0 en [ECX] y gW1 en [ECX + 0x34]; devuelve this.
inline u32 __fastcall FkBaseModel(void* self, void*) {
    Ev& e = PushEv(kEvBaseModel, 1); e.v[0] = Tag(self);
    memcpy(self, &gW0, 4); memcpy(static_cast<u8*>(self) + 0x34, &gW1, 4);
    return u32(uintptr_t(self));
}
// Colaboradores de RxD3D9ResEntryHeaderDestroy (cdecl): evento [direccion, argumentos...].
inline void __cdecl FkDynGeo(void* p) { Ev& e = PushEv(kEvDynGeo, 1); e.v[0] = Tag(p); }
inline void __cdecl FkIndexBuf(void* p) { Ev& e = PushEv(kEvIndexBuf, 1); e.v[0] = Tag(p); }
inline void __cdecl FkDecl(void* p) { Ev& e = PushEv(kEvDecl, 1); e.v[0] = Tag(p); }
inline void __cdecl FkVtxDestroy(u32 stride, u32 total, void* buffer, u32 offset) {
    Ev& e = PushEv(kEvVtxDestroy, 4); e.v[0] = stride; e.v[1] = total; e.v[2] = Tag(buffer); e.v[3] = offset;
}
inline void __cdecl FkDynVtx(void* p) { Ev& e = PushEv(kEvDynVtx, 1); e.v[0] = Tag(p); }
// RwRasterRegisterPlugin (0x7FB0B0; cdecl, cinco argumentos): evento [direccion, argumentos]; devuelve gRet.
inline u32 __cdecl FkRegPlugin(u32 size, u32 id, u32 ctor, u32 dtor, u32 copy) {
    Ev& e = PushEv(kEvRegPlugin, 5); e.v[0] = size; e.v[1] = id; e.v[2] = ctor; e.v[3] = dtor; e.v[4] = copy;
    return gRet;
}
// RwTextureRasterGenerateMipmaps (0x7F3CD0; cdecl, (raster, imagen)): evento [direccion, raster (etiqueta), imagen]; devuelve 1.
inline u32 __cdecl FkMipmaps(void* raster, void* image) {
    Ev& e = PushEv(kEvMipmaps, 2); e.v[0] = Tag(raster); e.v[1] = u32(uintptr_t(image));
    return 1;
}

// Metodos COM (stdcall). Evento [codigo, id del objeto, argumentos...]
inline constexpr u32 kEvCom = 0xC0A00000u;
inline u32 __stdcall FkSurfRelease(void* self) { Ev& e = PushEv(kEvCom + 1, 1); e.v[0] = ComIdOf(self); return 0; }
inline u32 __stdcall FkSurfGetDesc(void* self, u32* desc) {
    Ev& e = PushEv(kEvCom + 2, 2); e.v[0] = ComIdOf(self); e.v[1] = Tag(desc);
    memcpy(desc, gDesc.data(), 32);
    return 0;
}
inline u32 __stdcall FkSurfLockRect(void* self, u32* locked, const u32* rect, u32 flags) {
    Ev& e = PushEv(kEvCom + 3, 8); e.v[0] = ComIdOf(self); e.v[1] = Tag(locked); e.v[2] = rect ? 1u : 0u;
    for (u32 i = 0; i < 4; ++i) e.v[3 + i] = rect ? rect[i] : 0u;
    e.v[7] = flags;
    if (HrOk(gHrLock)) { locked[0] = gPitch; locked[1] = gBits; }
    return gHrLock;
}
inline u32 __stdcall FkSurfUnlockRect(void* self) { Ev& e = PushEv(kEvCom + 4, 1); e.v[0] = ComIdOf(self); return 0; }
inline u32 __stdcall FkTexGetSurfaceLevel(void* self, u32 level, void** out) {
    Ev& e = PushEv(kEvCom + 5, 3); e.v[0] = ComIdOf(self); e.v[1] = level; e.v[2] = Tag(out);
    if (HrOk(gHrGsl)) *out = &gCom[kComLevel0 + (gGslSel & 3)];
    return gHrGsl;
}
inline u32 __stdcall FkCubeGetFace(void* self, u32 face, u32 level, void** out) {
    Ev& e = PushEv(kEvCom + 6, 4); e.v[0] = ComIdOf(self); e.v[1] = face; e.v[2] = level; e.v[3] = Tag(out);
    if (HrOk(gHrGsl)) *out = &gCom[kComLevel0 + (gGslSel & 3)];
    return gHrGsl;
}
inline u32 __stdcall FkDevCreate(void* self, u32 w, u32 h, u32 fmt, u32 pool, void** out, void* shared) {
    Ev& e = PushEv(kEvCom + 7, 7); e.v[0] = ComIdOf(self); e.v[1] = w; e.v[2] = h; e.v[3] = fmt; e.v[4] = pool; e.v[5] = Tag(out); e.v[6] = u32(uintptr_t(shared));
    if (HrOk(gHrCreate)) *out = &gCom[kComOffscreen0 + (gCreateSel & 1)];
    return gHrCreate;
}
inline u32 __stdcall FkDevGetRtData(void* self, void* src, void* dst) {
    Ev& e = PushEv(kEvCom + 8, 3); e.v[0] = ComIdOf(self); e.v[1] = ComIdOf(src); e.v[2] = ComIdOf(dst);
    return 0;
}
inline u32 __stdcall FkDevUpdate(void* self, void* src, const u32* srcRect, void* dst, const u32* dstPoint) {
    Ev& e = PushEv(kEvCom + 9, 11); e.v[0] = ComIdOf(self); e.v[1] = ComIdOf(src); e.v[2] = srcRect ? 1u : 0u;
    for (u32 i = 0; i < 4; ++i) e.v[3 + i] = srcRect ? srcRect[i] : 0u;
    e.v[7] = ComIdOf(dst); e.v[8] = dstPoint ? 1u : 0u; e.v[9] = dstPoint ? dstPoint[0] : 0u; e.v[10] = dstPoint ? dstPoint[1] : 0u;
    return 0;
}
inline u32 __stdcall FkDevSetPalette(void* self, u32 number, const u8* entries) {
    Ev& e = PushEv(kEvCom + 10, 4); e.v[0] = ComIdOf(self); e.v[1] = number; e.v[2] = Tag(entries); e.v[3] = Fnv(entries, 0x400);
    return 0;
}
template<size_t N> inline void __cdecl FkTrap() { Ev& e = PushEv(kEvCom + 11, 1); e.v[0] = u32(N); }
template<size_t... I> inline void FillTraps(std::array<void*, 128>& v, std::index_sequence<I...>) { ((v[I] = reinterpret_cast<void*>(&FkTrap<I>)), ...); }
inline void InitCom() {
    for (auto* v : {&gVtDevice, &gVtTexture, &gVtCube, &gVtSurface}) FillTraps(*v, std::make_index_sequence<128>{});
    for (auto* v : {&gVtDevice, &gVtTexture, &gVtCube, &gVtSurface}) (*v)[2] = reinterpret_cast<void*>(&FkSurfRelease);
    gVtSurface[12] = reinterpret_cast<void*>(&FkSurfGetDesc); gVtSurface[13] = reinterpret_cast<void*>(&FkSurfLockRect); gVtSurface[14] = reinterpret_cast<void*>(&FkSurfUnlockRect);
    gVtTexture[18] = reinterpret_cast<void*>(&FkTexGetSurfaceLevel); gVtCube[18] = reinterpret_cast<void*>(&FkCubeGetFace);
    gVtDevice[30] = reinterpret_cast<void*>(&FkDevUpdate); gVtDevice[32] = reinterpret_cast<void*>(&FkDevGetRtData); gVtDevice[36] = reinterpret_cast<void*>(&FkDevCreate); gVtDevice[71] = reinterpret_cast<void*>(&FkDevSetPalette);
    for (u32 i = 0; i < kComCount; ++i) {
        gCom[i].id = i;
        gCom[i].vtbl = i == kComDevice ? gVtDevice.data() : (i == kComTexture ? gVtTexture.data() : (i == kComCube ? gVtCube.data() : gVtSurface.data()));
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
inline void PutG16(u32 at, u32 v) { memcpy(reinterpret_cast<void*>(uintptr_t(at)), &v, 2); }
inline void PutG8(u32 at, u32 v) { *reinterpret_cast<u8*>(uintptr_t(at)) = u8(v); }
// Prepara arenas y memoria global del caso; devuelve la llamada
inline u32 RegionSeed(const C& c, u32 region) { return c.a[90] + region * 17u; }
inline void FillRegion(const C& c, u32 region) { FillGlobal(kRegAt[region], kRegBytes[region], c.a[2], RegionSeed(c, region)); }
// Cadena de rasters de las funciones de D3D9: R0 (A + a[0]), R1 (B + a[5]), R2 (C + a[6]), R3 (D + a[7]); `depth` = a[4]; el ultimo es su propio padre
inline void SetupRasters(const C& c, bool fullChain) {
    const auto& a = c.a;
    Arena<kWin>* const ar[4]{&arenaA, &arenaB, &arenaC, &arenaD};
    const u32 off[4]{a[0], a[5], a[6], a[7]};
    const u32 X = a[8];
    const u32 depth = fullChain ? a[4] : 0u;
    for (u32 k = 0; k <= depth; ++k) ar[k]->Put32(off[k], k < depth ? u32(ar[k + 1]->Obj(off[k + 1])) : u32(ar[k]->Obj(off[k])));
    const u32 o = off[0];
    arenaA.Put32(o + 4, a[9]); arenaA.Put32(o + 8, a[46]); arenaA.Put32(o + 0xC, a[13]); arenaA.Put32(o + 0x10, a[14]); arenaA.Put32(o + 0x18, a[19]);
    arenaA.Put16(o + 0x1C, a[15]); arenaA.Put16(o + 0x1E, a[16]); arenaA.Put8(o + 0x20, a[10]); arenaA.Put8(o + 0x22, a[11]); arenaA.Put8(o + 0x23, a[12]);
    arenaA.Put32(o + 0x28, a[17]); arenaA.Put32(o + 0x2C, a[18]);
    const u32 e0 = o + X;
    arenaA.Put8(e0 + 9, a[23]); arenaA.Put8(e0 + 0xA, a[24]); arenaA.Put8(e0 + 0xB, a[25]);
    arenaA.Put32(e0 + 0xC, ComAddr(kComLevel0 + a[26]));   // superficie bloqueada: ids 5..10
    arenaA.Put32(e0 + 4, a[45] ? u32(arenaC.Obj(a[43])) : 0u);
    // extension del raster superior
    const u32 et = off[depth] + X;
    const bool cube = (a[21] & 0x0Fu) != 0;
    u32 texture = cube ? ComAddr(kComCube) : ComAddr(kComTexture);
    if ((a[10] & 7u) == 2) texture = a[22] != 0 ? ComAddr(kComSwapSurface) : 0xDEADC0DEu;
    ar[depth]->Put32(et + 0, texture); ar[depth]->Put8(et + 9, a[21]); ar[depth]->Put32(et + 0x1C, a[22]);
    PutG32(kExtOffsetAt, X);
    PutG32(kDevAt, ComAddr(kComDevice)); PutG32(kDevAt + 8, ComAddr(kComRenderSurface));
    gHrGsl = a[27]; gGslSel = a[28]; gHrLock = a[29]; gPitch = a[30]; gBits = a[31]; gHrCreate = a[32]; gCreateSel = a[33];
    for (u32 i = 0; i < 8; ++i) gDesc[i] = a[34 + i];
}
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind k = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gRet = 0; gW0 = 0; gW1 = 0; gCount = 0; gIdxRet = 0; gHrGsl = gGslSel = gHrLock = gPitch = gBits = gHrCreate = gCreateSel = 0; gDesc = {};
    for (const u32 r : regionsOf[c.fn]) if (r != 6) FillRegion(c, r);
    *reinterpret_cast<u8*>(kBenchMarker) = u8(kMarkerValue);   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    if (k != kLock && k != kUnlock && k != kPalette) arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const auto stdcall_ = [&](u32 n) { call.argCount = n; call.popBytes = 0; };
    const auto cdecl_ = [&](u32 n) { call.argCount = n; call.popBytes = 4 * n; };
    switch (k) {
    case kEntryDtor:
        gRet = a[4]; gW0 = a[5];
        stdcall_(0);
        break;
    case kPoolGetAt: {
        const i32 slot = i32(a[4]);
        const u32 objects = a[8] ? u32(arenaC.Obj(a[9])) : a[7];
        arenaA.Put32(o, objects); arenaA.Put32(o + 4, u32(arenaB.Obj(a[6])) - u32(slot));
        arenaB.Put8(a[6], a[5]);
        call.args[0] = (u32(slot) << 8) | (a[10] ? a[5] : a[11]); stdcall_(1);
        break;
    }
    case kPoolGetRef: {
        const i32 slot = i32(a[4]);
        const int64 diff = int64(slot) * 412 + (slot < 0 ? -int64(a[7]) : int64(a[7]));
        const u32 obj = a[8] ? u32(arenaC.Obj(a[9])) : a[9];
        arenaA.Put32(o, obj - u32(diff)); arenaA.Put32(o + 4, u32(arenaB.Obj(a[6])) - u32(slot));
        arenaB.Put8(a[6], a[5]);
        call.args[0] = obj; stdcall_(1);
        break;
    }
    case kDecision:
        gW0 = a[4]; gW1 = a[5];
        stdcall_(0);
        break;
    case kMissionRm: {
        memcpy(arenaA.m.data() + kGuard + o, c.s.data(), 0x258);
        arenaA.Put8(o + 0x258, a[11]);
        const u32 slot = a[6];
        const u32 sizes[3]{0xA18u, 0x7C4u, 0x19Cu};
        for (u32 i = 0; i < 3; ++i) {
            arenaB.Put32(a[12 + i], u32(arenaD.Obj(a[8])) - slot * sizes[i]);
            arenaB.Put32(a[12 + i] + 4, u32(arenaC.Obj(a[9])) - slot);
        }
        arenaC.Put8(a[9], a[7]);
        arenaD.Put32(a[8] + 0x1C, a[10]);
        PutG32(0xB74494, u32(arenaB.Obj(a[12]))); PutG32(0xB74490, u32(arenaB.Obj(a[13]))); PutG32(0xB7449C, u32(arenaB.Obj(a[14])));
        call.args[0] = a[4]; call.args[1] = a[5]; stdcall_(2);
        break;
    }
    case kSuppressed:
        for (u32 i = 0; i < 40; ++i) PutG32(kSuppAt + 4 * i, a[8 + i]);
        call.args[0] = a[4]; cdecl_(1);
        break;
    case kBrains:
        gW0 = a[4];
        stdcall_(0);
        break;
    case kCamera:
        PutG32(kTimerAt, a[4]);
        arenaA.Put32(o + 0xC60, a[5]); arenaA.Put32(o + 0xCC0, a[6]);
        stdcall_(0);
        break;
    case kSdd:
        gW0 = a[5];
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kBoxStub:
        for (u32 i = 0; i < 6; ++i) call.args[i] = a[4 + i];
        cdecl_(6);
        break;
    case kSlBetween: case kSlEntity: case kSlPoint: {
        gIdxRet = a[5];
        const i32 idx = i32(a[5]);
        if (idx >= 0 && idx < 8) {
            const u32 light = kLightsAt + 0x7Cu * u32(idx);
            PutG32(light + 0x14, a[7]); PutG32(light + 0x18, a[8]); PutG32(light + 0x48, a[6]);
        }
        call.args[0] = a[4];
        if (k == kSlBetween) { for (u32 i = 0; i < 7; ++i) call.args[1 + i] = a[9 + i]; cdecl_(8); }
        else if (k == kSlEntity) { call.args[1] = a[9]; call.args[2] = a[10]; cdecl_(3); }
        else { for (u32 i = 0; i < 4; ++i) call.args[1 + i] = a[9 + i]; cdecl_(5); }
        break;
    }
    case kPedMI:
        gW0 = a[4]; gW1 = a[5];
        stdcall_(0);
        break;
    case kHeader: {
        arenaA.Put32(o + 0xC, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        if (a[4]) { arenaB.Put8(a[5], a[6]); arenaB.Put32(a[5] + 0x18, a[7]); }
        arenaA.Put32(o + 0x20, a[8]); arenaA.Put32(o + 0x4C, a[9]); arenaA.Put32(o + 0x54, a[10]);
        for (u32 i = 0; i < 2; ++i) {
            const u32 b = 11 + 6 * i, base = o + 0x28 + 0x10 * i;
            arenaA.Put32(base, a[b]); arenaA.Put32(base + 4, a[b + 1]); arenaA.Put32(base + 8, a[b + 2]); arenaA.Put16(base + 0xC, a[b + 3]); arenaA.Put8(base + 0xE, a[b + 4]); arenaA.Put8(base + 0xF, a[b + 5]);
        }
        call.args[0] = thisA; cdecl_(1);
        break;
    }
    case kAttach:
        gRet = a[4];
        cdecl_(0);
        break;
    case kTexAlpha: {
        PutG32(kExtOffsetAt, a[6]);
        arenaA.Put32(o, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        if (a[4]) { arenaB.Put8(a[5] + a[6] + 8, a[7]); arenaB.Put8(a[5] + a[6] + 9, a[8]); }
        call.args[0] = thisA; cdecl_(1);
        break;
    }
    case kIsCube:
        PutG32(kExtOffsetAt, a[6]);
        arenaB.Put8(a[5] + a[6] + 9, a[7]);
        call.args[0] = u32(arenaB.Obj(a[5])); cdecl_(1);
        break;
    case kLock:
        SetupRasters(c, true);
        call.args[0] = u32(arenaC.Obj(a[42])); call.args[1] = thisA; call.args[2] = a[20]; cdecl_(3);
        break;
    case kUnlock:
        SetupRasters(c, true);
        call.args[0] = a[47]; call.args[1] = thisA; cdecl_(2);
        break;
    case kPalette:
        SetupRasters(c, false);
        arenaC.Put32(a[43] + 0x400, a[44]);
        call.args[0] = a[47]; call.args[1] = thisA; cdecl_(2);
        break;
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kPoolGetAt: case kDecision: case kBrains: case kSdd: case kPedMI: return rTag;   // puntero como etiqueta de arena
    case kPoolGetRef: case kCamera: case kAttach: case kTexAlpha: case kIsCube: case kLock: case kUnlock: case kPalette: return rRaw;   // EAX completo
    default: return rNone;
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
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN223A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN223A_CASES", buffer, sizeof(buffer)) > 0) {
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn223a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn223a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn223a"); }
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
    // Colaborador con hook RH que la DLL llama POR LA DIRECCION DEL EXE: el falso va solo en la direccion del EXE (el hook queda apagado)
    auto FE = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL que es funcion con hook (hook apagado): el EXE corre sus bytes y la DLL llama a su cuerpo
    auto R = [&](uintptr_t exe) -> Pair {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;
        return p;
    };
    env.stubs[0] = {F(kEvFlush, &FkFlush)};
    env.stubs[3] = {F(kEvDecCtor, &FkDecCtor), F(kEvDecSet, &FkDecSet)};
    env.stubs[4] = {F(kEvMoving, &FkMoving)};
    env.stubs[6] = {F(kEvBrainsInit, &FkBrainsInit)};
    env.stubs[9] = {F(kEvTaskDtor, &FkTaskDtor), F(kEvTaskDelete, &FkTaskDelete)};
    env.reals[9] = {R(0x4748F0)};
    env.stubs[11] = {F(kEvThingIdx, &FkThingIdx), F(kEvCleanUp, &FkCleanUp)};
    env.stubs[12] = {F(kEvThingIdx, &FkThingIdx), F(kEvCleanUp, &FkCleanUp), F(kEvRegRef, &FkRegRef)};
    env.stubs[13] = {F(kEvThingIdx, &FkThingIdx), F(kEvCleanUp, &FkCleanUp)};
    env.stubs[14] = {F(kEvBaseModel, &FkBaseModel)};
    env.stubs[15] = {FE(kEvDynGeo, &FkDynGeo), FE(kEvIndexBuf, &FkIndexBuf), FE(kEvDecl, &FkDecl), FE(kEvVtxDestroy, &FkVtxDestroy), FE(kEvDynVtx, &FkDynVtx)};
    env.stubs[16] = {F(kEvRegPlugin, &FkRegPlugin)};
    env.stubs[20] = {F(kEvMipmaps, &FkMipmaps)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    dllReal[1] = dllReal[2] = dllReal[5] = dllReal[7] = dllReal[8] = dllReal[10] = dllReal[17] = dllReal[18] = "toda la funcion (sin colaboradores)";
    dllReal[9] = "toda la funcion; CTask::Destructor 0x61A3A0 y CTask::operator delete 0x61A5B0 son falsos identicos; el cuerpo 0x4748F0 corre real (el EXE su salto, la DLL su cuerpo)";
    dllReal[11] = dllReal[12] = dllReal[13] = "toda la funcion (la x87 de 0x493360 incluida) salvo GetActualScriptThingIndex, CleanUpOldReference y RegisterReference (falsos identicos)";
    dllReal[15] = "toda la funcion; las cinco rutinas de liberacion que llama por su direccion del EXE (0x74E9E0, 0x730D30, 0x7FAC10, 0x7F56A0, 0x7F5AE0) son falsos solo en esa direccion";
    dllReal[16] = "toda la funcion salvo RwRasterRegisterPlugin (falso identico); la tabla de formatos 0xB4E7E0 se escribe de verdad";
    dllReal[19] = dllReal[20] = dllReal[21] = "toda la funcion; el dispositivo, las texturas y las superficies de D3D9 son objetos COM falsos del banco (tablas virtuales propias, los mismos en ambos lados)";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Memoria global del juego tocada (7 regiones; ver Fixtures.hpp): se guardan al empezar y se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (size_t r = 0; r < kRegAt.size(); ++r) regions.emplace_back(kRegAt[r], kRegBytes[r]);
    for (unsigned f = 0; f < kFunctions; ++f) {
        switch (kKind[f]) {
        case kAttach: case kTexAlpha: case kIsCube: regionsOf[f] = {0}; break;
        case kLock: case kUnlock: case kPalette: regionsOf[f] = {0, 1}; break;
        case kSuppressed: regionsOf[f] = {2}; break;
        case kSlBetween: case kSlEntity: case kSlPoint: regionsOf[f] = {3}; break;
        case kCamera: regionsOf[f] = {4}; break;
        case kMissionRm: regionsOf[f] = {5}; break;
        default: break;
        }
        regionsOf[f].push_back(6);   // la marca del banco (todas las funciones)
    }
    Run(env, ready, registered, bodies, collaborators);
}
}
