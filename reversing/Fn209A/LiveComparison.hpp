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

// Banco par a par Fn209A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 29 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall, CTaskSimpleSlideToCoord::Clone). El banco invoca la
// direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F) o solo en la direccion del EXE cuando la DLL los llama por ella
// (FE); los demas (CRT y plantillas sin hook) llevan el falso solo en la direccion del EXE (ByExeAddress). Falsos: operator new 0x82119A y delete 0x8214BD (CRT; la DLL los llama por esas direcciones),
// std::string::_Xran 0x8211FD, _Xlen 0x821255 y _CxxThrowException 0x822D5D (no vuelven: salida no local), _Copy 0x4F39C0, constructor 0x822D97, constructor de copia 0x822DE5 y cuerpo del destructor 0x822E2F de
// std::exception, y los colaboradores con hook: fill 0x4F3AE0, copy_backward 0x4F3B00, fill_n 0x4F3B30, envoltorio 0x4F3CE0, fill_n que devuelve el final 0x4F3D80, Grow 0x4F3BD0, erase 0x4F3DB0, assign 0x4F3E80
// y 0x4F3F60, runtime_error(const string&) 0x4F4090, cuerpos del destructor 0x4F3D10 y 0x4F4130, vector::_Xlen 0x4F41B0, destructor y operator delete de CPatrolRoute 0x6677A0 y 0x41B820, cuerpo de CTaskComplex
// 0x61A3D0, CTask::operator new 0x61A5A0 y los constructores de CTaskSimpleSlideToCoord 0x66C450 y 0x66C3E0. uninitialized_copy 0x4F3940 (sin hook) corre REAL en Insert. Las ranuras de la vtabla sintetica de
// la tarea (MakeAbortable de la subtarea, ranura 6, y CreateFirstSubTask, ranura 9) apuntan a falsos propios.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena; AL en las funciones cuyos llamadores solo lo leen), si la funcion termino con la salida no local y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).


// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn209a-crash.txt (junto al EXE).
namespace F209ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn209a-crash.txt";
    char text[2048]{};
    const auto* c = p->ContextRecord;
    int n = wsprintfA(text, "av eip=%08X addr=%08X rw=%u eax=%08X ecx=%08X edx=%08X esi=%08X edi=%08X ebp=%08X esp=%08X\r\nstack:", unsigned(c->Eip), unsigned(p->ExceptionRecord->ExceptionInformation[1]), unsigned(p->ExceptionRecord->ExceptionInformation[0]),
        unsigned(c->Eax), unsigned(c->Ecx), unsigned(c->Edx), unsigned(c->Esi), unsigned(c->Edi), unsigned(c->Ebp), unsigned(c->Esp));
    const unsigned* sp = reinterpret_cast<const unsigned*>(c->Esp);
    for (int i = 0; i < 48 && !IsBadReadPtr(sp + i, 4); ++i) n += wsprintfA(text + n, " %08X", sp[i]);
    HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(c->Eip), &m);
    n += wsprintfA(text + n, "\r\nmodule=%08X\r\n", unsigned(reinterpret_cast<uintptr_t>(m)));
    HANDLE f = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) { DWORD w{}; WriteFile(f, text, DWORD(n), &w, nullptr); CloseHandle(f); }
    return EXCEPTION_CONTINUE_SEARCH;
}
inline const bool installed = [] { AddVectoredExceptionHandler(1, Handler); return true; }();
}

namespace F209ALive {
using namespace F209AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F209AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// ================================================================================================ falsos de los colaboradores
inline u32 gNew{};                   // lo que devuelven los falsos de operator new
inline u32 gRet{};                   // EAX que devuelven los falsos de CopyWrap, MakeAbortable y CreateFirstSubTask de la tarea
inline u32 gGrow{};                  // EAX del falso de std::string::_Grow
inline DWORD gBenchThread{};

#pragma warning(push)
#pragma warning(disable : 4733 4731)
[[noreturn]] __declspec(noinline) inline void NoReturnExit() {
    gThrew = true;
    __asm {
        mov eax, gJmpSeh
        mov dword ptr fs:[0], eax
        mov ebp, gJmpEbp
        mov esp, gJmpEsp
        jmp dword ptr gJmpPc
    }
}
#pragma warning(pop)
inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }

// operator new (0x82119A, 0x61A5A0 de CTask): cdecl, un tamano; evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// operator delete (0x8214BD, 0x41B820 de CPatrolRoute): cdecl, un puntero CRUDO (no se desreferencia); evento [direccion, puntero (etiqueta)].
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(p));
}
// Funciones que no vuelven (_Xran 0x8211FD, _Xlen 0x821255): sin argumentos; evento [direccion]; salida no local.
template<u32 ADDR> [[noreturn]] inline void __cdecl FkNoReturn0() {
    PushEv(ADDR, 0);
    NoReturnExit();
}
// vector::_Xlen (0x4F41B0, thiscall sin argumentos): evento [direccion, ECX (etiqueta)]; salida no local.
[[noreturn]] inline void __fastcall FkVecXlen(void* self, void*) {
    Ev& e = PushEv(0x4F41B0, 1);
    DumpTag(e, 0, uintptr_t(self));
    NoReturnExit();
}
// _CxxThrowException (0x822D5D, stdcall (objeto, ThrowInfo)): evento [direccion, objeto (etiqueta), ThrowInfo, vtabla del objeto]; salida no local.
[[noreturn]] inline void __stdcall FkThrow(void* object, u32 info) {
    Ev& e = PushEv(0x822D5D, 3);
    DumpTag(e, 0, uintptr_t(object)); e.v[1] = info; e.v[2] = BitsOf(object);
    NoReturnExit();
}
// std::string::_Copy (0x4F39C0, thiscall (nuevaCapacidad, longitudAnterior), `ret 8`): evento [direccion, ECX (etiqueta), capacidad, longitud]; no cambia nada.
inline u32 __fastcall FkCopy(void* self, void*, u32 newSize, u32 oldLen) {
    Ev& e = PushEv(0x4F39C0, 3);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = newSize; e.v[2] = oldLen;
    return 0;
}
// fill (0x4F3AE0, cdecl (first, last, &valor)): evento [direccion, first, last, valor (deref)].
inline void __cdecl FkFill(void* first, void* last, const u32* value) {
    Ev& e = PushEv(0x4F3AE0, 3);
    DumpTag(e, 0, uintptr_t(first)); DumpTag(e, 1, uintptr_t(last)); e.v[2] = *value;
}
// copy_backward (0x4F3B00, cdecl (out, first, last, finDestino)): evento [direccion, out, first, last, finDestino] (etiquetas); no escribe.
inline void __cdecl FkCopyBack(void* out, void* first, void* last, void* destEnd) {
    Ev& e = PushEv(0x4F3B00, 4);
    DumpTag(e, 0, uintptr_t(out)); DumpTag(e, 1, uintptr_t(first)); DumpTag(e, 2, uintptr_t(last)); DumpTag(e, 3, uintptr_t(destEnd));
}
// uninitialized_fill_n (0x4F3B30, cdecl (dest, n, &valor, ...)): evento [direccion, dest, n, valor (deref)].
inline void __cdecl FkFillN(void* dest, u32 n, const u32* value) {
    Ev& e = PushEv(0x4F3B30, 3);
    DumpTag(e, 0, uintptr_t(dest)); e.v[1] = n; e.v[2] = *value;
}
// envoltorio de uninitialized_copy (0x4F3CE0, thiscall (first, last, dest), `ret 0Ch`): evento [direccion, ECX, first, last, dest]; devuelve gRet.
inline u32 __fastcall FkCopyWrap(void* self, void*, void* first, void* last, void* dest) {
    Ev& e = PushEv(0x4F3CE0, 4);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(first)); DumpTag(e, 2, uintptr_t(last)); DumpTag(e, 3, uintptr_t(dest));
    return gRet;
}
// fill_n que devuelve el final (0x4F3D80, thiscall (dest, n, &valor), `ret 0Ch`): evento [direccion, ECX, dest, n, valor (deref)].
inline u32 __fastcall FkFillNEnd(void* self, void*, void* dest, u32 n, const u32* value) {
    Ev& e = PushEv(0x4F3D80, 4);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(dest)); e.v[2] = n; e.v[3] = *value;
    return gRet;
}
// std::string::_Grow (0x4F3BD0, thiscall (nuevoTamano, vaciar), `ret 8`): evento [direccion, ECX, tamano, vaciar (byte bajo)]; devuelve gGrow.
inline u32 __fastcall FkGrow(void* self, void*, u32 newSize, u32 trim) {
    Ev& e = PushEv(0x4F3BD0, 3);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = newSize; e.v[2] = trim & 0xFFu;
    return gGrow;
}
// std::string::erase (0x4F3DB0, thiscall (pos, n), `ret 8`): evento [direccion, ECX, pos, n]; devuelve this.
inline u32 __fastcall FkErase(void* self, void*, u32 pos, u32 n) {
    Ev& e = PushEv(0x4F3DB0, 3);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = pos; e.v[2] = n;
    return u32(uintptr_t(self));
}
// std::string::assign(const string&, pos, n) (0x4F3E80, thiscall, `ret 0Ch`): evento [direccion, ECX, otro, pos, n]; devuelve this.
inline u32 __fastcall FkAssignStr(void* self, void*, void* str, u32 pos, u32 n) {
    Ev& e = PushEv(0x4F3E80, 4);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(str)); e.v[2] = pos; e.v[3] = n;
    return u32(uintptr_t(self));
}
// std::string::assign(const char*, n) (0x4F3F60, thiscall, `ret 8`): evento [direccion, ECX, texto, n, capacidad, longitud, primer byte del buffer interno] del string de ECX; devuelve this.
inline u32 __fastcall FkAssignChars(void* self, void*, void* text, u32 n) {
    Ev& e = PushEv(0x4F3F60, 6);
    const u8* s = static_cast<const u8*>(self);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(text)); e.v[2] = n; e.v[3] = BitsOf(s + 0x18); e.v[4] = BitsOf(s + 0x14); e.v[5] = s[4];
    return u32(uintptr_t(self));
}
// std::runtime_error(const string&) (0x4F4090, thiscall, `ret 4`): evento [direccion, ECX, mensaje, longitud y capacidad del mensaje]; devuelve this.
inline u32 __fastcall FkFromString(void* self, void*, void* message) {
    Ev& e = PushEv(0x4F4090, 4);
    const u8* m = static_cast<const u8*>(message);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(message)); e.v[2] = BitsOf(m + 0x14); e.v[3] = BitsOf(m + 0x18);
    return u32(uintptr_t(self));
}
// std::exception::exception() (0x822D97, thiscall): evento [direccion, ECX]; devuelve this. Constructor de copia (0x822DE5, `ret 4`): [direccion, ECX, otro].
inline u32 __fastcall FkExcCtor(void* self, void*) {
    Ev& e = PushEv(0x822D97, 1);
    DumpTag(e, 0, uintptr_t(self));
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkExcCopy(void* self, void*, void* other) {
    Ev& e = PushEv(0x822DE5, 2);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(other));
    return u32(uintptr_t(self));
}
// Funcion thiscall sin argumentos que no toca memoria (0x822E2F, 0x4F3D10, 0x4F4130, 0x6677A0, 0x61A3D0): evento [direccion, ECX]; devuelve this.
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(self));
    return u32(uintptr_t(self));
}
// Constructores de CTaskSimpleSlideToCoord (thiscall): largo 0x66C450 (nueve argumentos, `ret 24h`) y corto 0x66C3E0 (tres, `ret 0Ch`); devuelven this como los reales.
inline u32 __fastcall FkCtorLong(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9) {
    Ev& e = PushEv(0x66C450, 10);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, a1); e.v[2] = a2; e.v[3] = a3; DumpTag(e, 4, a4); DumpTag(e, 5, a5); e.v[6] = a6; e.v[7] = a7; e.v[8] = a8 & 0xFFu; e.v[9] = a9;
    return u32(uintptr_t(self));
}
inline u32 __fastcall FkCtorShort(void* self, void*, u32 a1, u32 a2, u32 a3) {
    Ev& e = PushEv(0x66C3E0, 4);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, a1); e.v[2] = a2; e.v[3] = a3;
    return u32(uintptr_t(self));
}
// Ranuras de la vtabla sintetica: MakeAbortable de la subtarea (ranura 6, thiscall (ped, prioridad, evento), `ret 0Ch`) y CreateFirstSubTask (ranura 9, thiscall (ped), `ret 4`); devuelven gRet.
inline u32 __fastcall FkSubAbort(void* self, void*, u32 ped, u32 prio, u32 event) {
    Ev& e = PushEv(0xF0000006u, 4);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, ped); e.v[2] = prio; e.v[3] = event;
    return gRet;
}
inline u32 __fastcall FkCreateFirst(void* self, void*, u32 ped) {
    Ev& e = PushEv(0xF0000009u, 2);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, ped);
    return gRet;
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
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

inline void PutStr(Arena<kWin>& self, u32 off, Arena<kWin>& blocks, const StrSpec& s) {
    self.Put32(off + 0x14, s.size);
    self.Put32(off + 0x18, s.res);
    if (s.mode == 1) {
        self.Put32(off + 4, u32(blocks.Obj(s.blk)));
        for (u32 i = 0; i < s.size; ++i) blocks.Put8(s.blk + i, StrChar(s.seed, i));
        blocks.Put8(s.blk + s.size, 0);
    } else if (s.mode == 2) {
        self.Put32(off + 4, s.raw);
    } else {
        for (u32 i = 0; i < s.size; ++i) self.Put8(off + 4 + i, StrChar(s.seed, i));
        self.Put8(off + 4 + s.size, 0);
    }
}
inline u32 StrData(const Arena<kWin>& arena, u32 off) { return arena.Get32(off + 0x18) >= 0x10 ? arena.Get32(off + 4) : u32(arena.Obj(off + 4)); }

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gNew = 0; gRet = 0; gGrow = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kFill: {
        arenaB.Put32(a[5], a[6]);
        call.args[0] = thisA; call.args[1] = thisA + 4 * a[4]; call.args[2] = u32(arenaB.Obj(a[5])); call.argCount = 3; call.popBytes = 12;
        break;
    }
    case kCopyBack: {
        const u32 src = u32(ArenaAt(a[6] == 0 ? 2 : 3).Obj(a[5]));
        call.args[0] = thisA; call.args[1] = src; call.args[2] = src + 4 * a[4]; call.args[3] = u32(arenaD.Obj(a[7])); call.argCount = 4; call.popBytes = 16;
        break;
    }
    case kFillN:
    case kFillNEnd: {
        arenaB.Put32(a[5], a[6]);
        const u32 dest = a[7] == 0 ? thisA : (a[7] == 1 ? 0u : a[8]);
        call.args[0] = dest; call.args[1] = a[4]; call.args[2] = u32(arenaB.Obj(a[5]));
        if (kKind[c.fn] == kFillN) { call.args[3] = a[8]; call.args[4] = a[9]; call.argCount = 5; call.popBytes = 20; }
        else { call.argCount = 3; call.popBytes = 0; }
        break;
    }
    case kCopyWrap: {
        const u32 src = u32(arenaC.Obj(a[5]));
        u32 dest = a[6] == 0 ? thisA : src + 4 * a[7];
        if (a[8] == 1) dest = 0;
        call.args[0] = src; call.args[1] = src + 4 * a[4]; call.args[2] = dest; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kVecTidy: {
        const u32 first = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[6]);
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, a[7]); arenaA.Put32(o + 0xC, a[8]);
        break;
    }
    case kInsert: {
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        const u32 last = first == 0 ? 0u : first + 4 * a[6];
        const u32 end = first == 0 ? a[10] : first + 4 * a[5];
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, end);
        arenaC.Put32(a[11], a[10]);
        const u32 pos = first == 0 ? 0u : first + 4 * a[8];
        gNew = u32(arenaD.Obj(a[12])); gRet = a[13];
        call.args[0] = pos; call.args[1] = a[9]; call.args[2] = u32(arenaC.Obj(a[11])); call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kStrTidy:
    case kGrow:
    case kErase:
        PutStr(arenaA, o, arenaB, GetSpec(c, 4));
        if (kKind[c.fn] == kGrow) { call.args[0] = a[10]; call.args[1] = a[11]; call.argCount = 2; call.popBytes = 0; }
        if (kKind[c.fn] == kErase) { call.args[0] = a[10]; call.args[1] = a[11]; call.argCount = 2; call.popBytes = 0; }
        break;
    case kAssignStr: {
        PutStr(arenaA, o, arenaB, GetSpec(c, 4));
        PutStr(arenaC, a[20], arenaD, GetSpec(c, 10));
        gGrow = a[19];
        call.args[0] = a[18] ? thisA : u32(arenaC.Obj(a[20])); call.args[1] = a[16]; call.args[2] = a[17]; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kAssignChars: {
        PutStr(arenaA, o, arenaB, GetSpec(c, 4));
        gGrow = a[13];
        const u32 data = StrData(arenaA, o), size = arenaA.Get32(o + 0x14);
        u32 text;
        switch (a[10]) {
        case 0: text = u32(arenaC.Obj(a[11])); break;
        case 1: text = data + a[11] % size; break;
        case 2: text = data + size; break;
        case 3: text = data - 1; break;
        default: text = data + size + 1; break;
        }
        call.args[0] = text; call.args[1] = a[12]; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kFromChars: {
        for (u32 i = 0; i < a[4]; ++i) arenaC.Put8(a[5] + i, StrChar(a[6], i));
        arenaC.Put8(a[5] + a[4], 0);
        call.args[0] = u32(arenaC.Obj(a[5])); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kRtDtorBody:
    case kRtWhat:
    case kRtScalarDel:
        arenaA.Put32(o, a[10]);
        PutStr(arenaA, o + 0xC, arenaB, GetSpec(c, 4));
        if (kKind[c.fn] == kRtScalarDel) { call.args[0] = a[11]; call.argCount = 1; call.popBytes = 0; }
        break;
    case kRtFromStr:
        PutStr(arenaC, a[10], arenaD, GetSpec(c, 4));
        call.args[0] = u32(arenaC.Obj(a[10])); call.argCount = 1; call.popBytes = 0;
        break;
    case kRtCopy:
        PutStr(arenaC, a[10] + 0xC, arenaD, GetSpec(c, 4));
        call.args[0] = u32(arenaC.Obj(a[10])); call.argCount = 1; call.popBytes = 0;
        break;
    case kXlen:
        break;
    case kHandbrake:
        arenaA.Put8(o + 0x428, a[4]);
        break;
    case kAlarm:
        arenaA.Put16(o + 0x45C, a[4]); arenaA.Put8(o + 0x36, a[5]);
        break;
    case kPatrolSD:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kAbort: {
        const u32 sub = u32(arenaB.Obj(a[4])), vt = u32(arenaC.Obj(a[5])), route = u32(arenaD.Obj(a[6])), ped = u32(arenaB.Obj(a[9])), mat = u32(arenaC.Obj(a[11]));
        arenaA.Put32(o + 8, sub); arenaA.Put32(o + 0x1C, route); arenaA.Put32(o + 0x20, a[8]);
        arenaD.Put32(a[6], a[7]);
        arenaB.Put32(a[4], vt);
        for (u32 k = 0; k < 10; ++k) arenaC.Put32(a[5] + 4 * k, 0xBAD00000u + k);
        arenaC.Put32(a[5] + 0x18, u32(uintptr_t(&FkSubAbort)));
        arenaC.Put32(a[5] + 0x24, u32(uintptr_t(&FkCreateFirst)));
        arenaB.Put32(a[9] + 0x14, a[10] ? mat : 0u);
        gRet = a[14];
        call.args[0] = ped; call.args[1] = a[12]; call.args[2] = a[13]; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kControl: {
        const u32 vt = u32(arenaC.Obj(a[5]));
        arenaA.Put32(o, vt); arenaA.Put32(o + 8, a[9]); arenaA.Put32(o + 0x20, a[8]);
        for (u32 k = 0; k < 10; ++k) arenaC.Put32(a[5] + 4 * k, 0xBAD00000u + k);
        arenaC.Put32(a[5] + 0x18, u32(uintptr_t(&FkSubAbort)));
        arenaC.Put32(a[5] + 0x24, u32(uintptr_t(&FkCreateFirst)));
        gRet = a[14];
        call.args[0] = a[10]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kFollowDtor: {
        const u32 route = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaD.Obj(a[5])) : a[6]);
        arenaA.Put32(o + 0x1C, route);
        break;
    }
    case kClone:
        arenaA.Put32(o + 0x78, a[4]);
        for (u32 j = 0; j < 3; ++j) arenaA.Put32(o + 0x64 + 4 * j, a[5 + j]);
        arenaA.Put32(o + 0x70, a[8]); arenaA.Put32(o + 0x74, a[9]); arenaA.Put32(o + 0x5C, a[10]); arenaA.Put32(o + 0x38, a[11]); arenaA.Put32(o + 0x40, a[12]); arenaA.Put8(o + 0xC, a[13]);
        gNew = a[14] == 0 ? 0u : u32(arenaD.Obj(a[15]));
        break;
    }
    return call;
}

enum RetKind { rNone, rTag, rAl };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kCopyWrap: case kFillNEnd: case kErase: case kAssignStr: case kAssignChars: case kFromChars: case kRtWhat: case kRtScalarDel: case kRtFromStr: case kRtCopy: case kPatrolSD: case kControl: case kClone: return rTag;
    case kGrow: case kHandbrake: case kAlarm: case kAbort: return rAl;
    default: return rNone;
    }
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
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
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(kKind[fn])) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case rAl: obs["ret"] = eax & 0xFFu; break;           // los llamadores del EXE solo leen AL
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN209A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN209A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn209a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn209a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn209a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
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
    // Un colaborador con hook RH que es tambien una funcion probada reutiliza su ruta (el hook ya esta apagado); si no, se crea una ruta de colaborador
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
    // Colaborador con hook RH que la DLL llama POR LA DIRECCION DEL EXE (plugin::Call): el falso va solo en la direccion del EXE (el hook queda apagado)
    auto FE = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Constructor de C++ (RH_ScopedOverloadedInstall(Constructor, ...)): el hook apunta a una envoltura `Constructor(...)` que llama al constructor real; `new T(...)` del codigo de la DLL llama a este ultimo, asi que el falso va
    // en el constructor real, cuya direccion sale de la envoltura (la primera llamada de la envoltura: el marco no tiene llamadas de comprobacion antes). El EXE lo llama por la direccion del EXE (falso en la entrada con el hook apagado).
    auto FC = [&](uintptr_t exe, auto* fake) {
        Pair p = Collab(exe);
        uintptr_t ctor = 0;
        for (uintptr_t a = p.dll; p.dll && a < p.dll + 0x200 && !ctor; ++a) {
            const uint8* q = reinterpret_cast<const uint8*>(a);
            if (q[0] == 0xE8) ctor = Body(a + 5 + *reinterpret_cast<const int32*>(a + 1));
        }
        hooksFound.push_back({{"ctor_wrapper", p.dll}, {"ctor", ctor}, {"wrapper_bytes", p.dll ? HexBytes(reinterpret_cast<void*>(p.dll), 96) : std::string()}});
        if (!p.hooked || !ctor || ModuleOf(ctor) != env.asi) ready = false;
        p.dll = ctor;
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto NewEv = [&] { return ByExeAddress(0x82119A, &FkNew<0x82119A>); };
    auto DelEv = [&] { return ByExeAddress(0x8214BD, &FkDelete<0x8214BD>); };
    env.stubs[5] = {DelEv()};
    env.stubs[6] = {DelEv()};
    env.stubs[7] = {NewEv(), DelEv(), FE(0x4F3B30, &FkFillN), FE(0x4F3CE0, &FkCopyWrap), FE(0x4F3D80, &FkFillNEnd), FE(0x4F3AE0, &FkFill), FE(0x4F3B00, &FkCopyBack), F(0x4F41B0, &FkVecXlen)};
    env.reals[7] = {Pair{0x4F3940, 0, false}};   // uninitialized_copy (sin hook): corre REAL en el EXE y en la DLL
    env.stubs[8] = {DelEv()};
    env.stubs[9] = {DelEv(), ByExeAddress(0x4F39C0, &FkCopy), ByExeAddress(0x821255, &FkNoReturn0<0x821255>)};
    env.stubs[10] = {ByExeAddress(0x8211FD, &FkNoReturn0<0x8211FD>)};
    env.stubs[11] = {ByExeAddress(0x8211FD, &FkNoReturn0<0x8211FD>), F(0x4F3BD0, &FkGrow), F(0x4F3DB0, &FkErase)};
    env.stubs[12] = {F(0x4F3BD0, &FkGrow), F(0x4F3E80, &FkAssignStr)};
    env.stubs[13] = {F(0x4F3F60, &FkAssignChars)};
    env.stubs[14] = {DelEv(), ByExeAddress(0x822E2F, &FkThis<0x822E2F>)};
    env.stubs[15] = {DelEv(), ByExeAddress(0x822E2F, &FkThis<0x822E2F>)};
    env.stubs[17] = {F(0x4F3D10, &FkThis<0x4F3D10>), DelEv()};
    env.stubs[18] = {F(0x4F4130, &FkThis<0x4F4130>), DelEv()};
    env.stubs[19] = {ByExeAddress(0x822D97, &FkExcCtor), F(0x4F3E80, &FkAssignStr)};
    env.stubs[20] = {ByExeAddress(0x822DE5, &FkExcCopy), F(0x4F3E80, &FkAssignStr)};
    env.stubs[21] = {F(0x4F3F60, &FkAssignChars), F(0x4F4090, &FkFromString), ByExeAddress(0x822D5D, &FkThrow)};
    env.stubs[24] = {F(0x6677A0, &FkThis<0x6677A0>), F(0x41B820, &FkDelete<0x41B820>)};
    env.stubs[27] = {F(0x41B820, &FkDelete<0x41B820>), F(0x61A3D0, &FkThis<0x61A3D0>)};
    env.stubs[28] = {F(0x61A5A0, &FkNew<0x61A5A0>), FC(0x66C450, &FkCtorLong), FC(0x66C3E0, &FkCtorShort)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 2u, 3u, 4u, 16u, 22u, 23u, 25u, 26u}) dllReal[f] = "toda la funcion (sin colaboradores falseados)";
    dllReal[7] = "toda la funcion salvo los falsos; uninitialized_copy 0x4F3940 (sin hook) corre real";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Ninguna memoria global del juego se toca (los colaboradores que lo harian estan falseados)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
