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

// Banco par a par Fn219A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 30 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F) o solo en la direccion del EXE cuando la DLL los llama por ella
// (FE); los demas (CRT) llevan el falso solo en la direccion del EXE (ByExeAddress). Falsos: operator new 0x82119A y delete 0x8214BD (CRT), _CxxThrowException 0x822D5D (no vuelve: salida no local), assign 0x4F3F60 y
// runtime_error(const string&) 0x4F4090 (vector::_Xlen), y los colaboradores de los insert: fill_n, envoltorio de copia, fill_n que devuelve el final, fill, copy_backward y vector::_Xlen de cada instancia. Los
// colaboradores que corren REALES (con el hook apagado): uninitialized_copy 0x5EAE10 / 0x5EAE40 / 0x5EAE70, size() 0x5E9800, los fill_n de las funciones fill_n-que-devuelve-el-final y FxSphere_c::FxSphere_c 0x4A9F70.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena; el recuento de RemoveItem en crudo), si la funcion termino con la salida no local y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).


// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn219a-crash.txt (junto al EXE).
namespace F219ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn219a-crash.txt";
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

namespace F219ALive {
using namespace F219AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F219AFix::Arena(WIN);
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
inline u32 gRet{};                   // EAX que devuelven los falsos de los envoltorios de copia y de fill_n que devuelve el final
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

// operator new (0x82119A): cdecl, un tamano; evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// operator delete (0x8214BD): cdecl, un puntero CRUDO (no se desreferencia); evento [direccion, puntero (etiqueta)].
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(p));
}
// vector::_Xlen de las instancias (thiscall sin argumentos): evento [direccion, ECX (etiqueta)]; salida no local.
template<u32 ADDR> [[noreturn]] inline void __fastcall FkVecXlen(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(self));
    NoReturnExit();
}
// _CxxThrowException (0x822D5D, stdcall (objeto, ThrowInfo)): evento [direccion, objeto (etiqueta), ThrowInfo, vtabla del objeto]; salida no local.
[[noreturn]] inline void __stdcall FkThrow(void* object, u32 info) {
    Ev& e = PushEv(0x822D5D, 3);
    DumpTag(e, 0, uintptr_t(object)); e.v[1] = info; e.v[2] = BitsOf(object);
    NoReturnExit();
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
// fill (cdecl (first, last, &valor)): evento [direccion, first, last, valor (WORDS palabras, deref)].
template<u32 ADDR, unsigned WORDS> inline void __cdecl FkFill(void* first, void* last, const u32* value) {
    Ev& e = PushEv(ADDR, 2 + WORDS);
    DumpTag(e, 0, uintptr_t(first)); DumpTag(e, 1, uintptr_t(last));
    for (unsigned i = 0; i < WORDS; ++i) e.v[2 + i] = value[i];
}
// copy_backward (cdecl (out, first, last, finDestino)): evento [direccion, out, first, last, finDestino] (etiquetas); no escribe.
template<u32 ADDR> inline void __cdecl FkCopyBack(void* out, void* first, void* last, void* destEnd) {
    Ev& e = PushEv(ADDR, 4);
    DumpTag(e, 0, uintptr_t(out)); DumpTag(e, 1, uintptr_t(first)); DumpTag(e, 2, uintptr_t(last)); DumpTag(e, 3, uintptr_t(destEnd));
}
// uninitialized_fill_n (cdecl (dest, n, &valor, ...)): evento [direccion, dest, n, valor (WORDS palabras, deref)].
template<u32 ADDR, unsigned WORDS> inline void __cdecl FkFillN(void* dest, u32 n, const u32* value) {
    Ev& e = PushEv(ADDR, 2 + WORDS);
    DumpTag(e, 0, uintptr_t(dest)); e.v[1] = n;
    for (unsigned i = 0; i < WORDS; ++i) e.v[2 + i] = value[i];
}
// envoltorio de uninitialized_copy (thiscall (first, last, dest), `ret 0Ch`): evento [direccion, ECX, first, last, dest]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCopyWrap(void* self, void*, void* first, void* last, void* dest) {
    Ev& e = PushEv(ADDR, 4);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(first)); DumpTag(e, 2, uintptr_t(last)); DumpTag(e, 3, uintptr_t(dest));
    return gRet;
}
// fill_n que devuelve el final (thiscall (dest, n, &valor), `ret 0Ch`): evento [direccion, ECX, dest, n, valor (WORDS palabras, deref)]; devuelve gRet.
template<u32 ADDR, unsigned WORDS> inline u32 __fastcall FkFillNEnd(void* self, void*, void* dest, u32 n, const u32* value) {
    Ev& e = PushEv(ADDR, 3 + WORDS);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(dest)); e.v[2] = n;
    for (unsigned i = 0; i < WORDS; ++i) e.v[3 + i] = value[i];
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

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gNew = 0; gRet = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    // modo de puntero de RemoveItem / RemoveHead: 0 nulo, 1 nodo en C, 2 nodo en D, 3 la lista, 4 el propio elemento, 5 this + 4 (RemoveItem) o this + 8 (RemoveHead), 6 this + 8
    const auto Mode = [&](u32 mode, u32 off, u32 item, bool removeItem) -> u32 {
        switch (mode) {
        case 0: return 0u;
        case 1: return u32(arenaC.Obj(off));
        case 2: return u32(arenaD.Obj(off));
        case 3: return thisA;
        case 4: return item;
        case 5: return thisA + (removeItem ? 4u : 8u);
        default: return thisA + 8u;
        }
    };
    switch (kKind[c.fn]) {
    case kEmpty:
    case kListCtor:
    case kFrustum:
    case kXlen:
        break;
    case kRemoveItem: {
        const u32 item = a[4] == 1 ? thisA : (a[4] == 0 ? u32(arenaB.Obj(a[5])) : u32(arenaC.Obj(a[5])));
        const u32 nxt = Mode(a[6], a[7], item, true), prv = Mode(a[8], a[9], item, true);
        arenaA.Put32(o + 8, a[10]);
        if (a[4] == 1) { arenaA.Put32(o + 4, nxt); arenaA.Put32(o, prv); }
        else if (a[4] == 0) { arenaB.Put32(a[5] + 4, nxt); arenaB.Put32(a[5], prv); }
        else { arenaC.Put32(a[5] + 4, nxt); arenaC.Put32(a[5], prv); }
        call.args[0] = item; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kRemoveHead: {
        const u32 sc = a[4];
        const u32 node = u32(arenaB.Obj(a[5]));
        u32 head, tail;
        if (sc == 0) { head = 0; tail = a[8]; }
        else if (sc == 4) { head = a[10]; tail = a[10]; }
        else if (sc == 1) { head = node; tail = node; }
        else { head = node; tail = a[8]; if (tail == head) tail ^= 4u; }
        arenaA.Put32(o, head); arenaA.Put32(o + 4, tail); arenaA.Put32(o + 8, a[9]);
        if (sc == 2 || sc == 3) arenaB.Put32(a[5] + 4, Mode(sc == 3 ? a[6] : 0u, a[7], head, false));
        break;
    }
    case kGetOffset: {
        const u32 len = a[4], base = a[5], sep = a[6], flags = a[7], brk = a[8];
        const auto Node = [&](u32 i) { return u32(arenaB.Obj(base + sep * i)); };
        u32 head = len ? Node(0) : 0u, tail = len ? Node(len - 1) : 0u;
        if (flags & 1u) head = 0;
        if (flags & 2u) tail = 0;
        if (flags & 32u) { head = a[11]; tail = a[12]; }
        arenaA.Put32(o, head); arenaA.Put32(o + 4, tail);
        for (u32 i = 0; i < len; ++i) {
            u32 nx = i + 1 < len ? Node(i + 1) : ((flags & 4u) ? Node(0) : 0u);
            u32 pv = i > 0 ? Node(i - 1) : ((flags & 4u) ? Node(len - 1) : 0u);
            if (i == brk % len) {
                if (flags & 8u) nx = 0;
                if (flags & 16u) pv = 0;
            }
            arenaB.Put32(base + sep * i, pv);
            arenaB.Put32(base + sep * i + 4, nx);
        }
        call.args[0] = a[9]; call.args[1] = a[10]; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kVecTidy: {
        const u32 first = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[6]);
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, a[7]); arenaA.Put32(o + 0xC, a[8]);
        break;
    }
    case kFillNEnd4:
    case kFillNEnd20:
    case kFillNEnd12: {
        const u32 words = kKind[c.fn] == kFillNEnd4 ? 1u : (kKind[c.fn] == kFillNEnd20 ? 5u : 3u);
        const u32 vals[5] = {a[6], a[11], a[12], a[13], a[14]};
        for (u32 i = 0; i < words; ++i) arenaB.Put32(a[5] + 4 * i, vals[i]);
        const u32 valptr = u32(arenaB.Obj(a[5]));
        const u32 dest = a[7] == 0 ? thisA : (a[7] == 1 ? 0u : (a[7] == 2 ? a[8] : valptr + 4 * a[10]));
        call.args[0] = dest; call.args[1] = a[4]; call.args[2] = valptr; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kErase: {
        const u32 first = u32(arenaB.Obj(a[5])), last = first + 4 * a[4], end = first + 4 * (a[4] + a[9]);
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, end);
        const u32 where = first + 4 * a[6];
        const u32 mode = a[7];
        const u32 out = mode == 0 ? u32(arenaC.Obj(a[8])) : (mode == 1 ? thisA + 8 : (mode == 2 ? first + 4 * a[8] : (mode == 3 ? thisA + 4 : thisA + 0xC)));
        call.args[0] = out; call.args[1] = where; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kInsert4: {
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
    case kInsert20: {
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        const u32 last = first == 0 ? 0u : first + 20 * a[6];
        const u32 end = first == 0 ? a[10] : first + 20 * a[5];
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, end);
        for (u32 i = 0; i < 5; ++i) arenaC.Put32(a[15] + 4 * i, a[10 + i]);
        const u32 pos = first == 0 ? 0u : first + 20 * a[8];
        gNew = u32(arenaD.Obj(a[16])); gRet = a[17];
        call.args[0] = pos; call.args[1] = a[9]; call.args[2] = u32(arenaC.Obj(a[15])); call.argCount = 3; call.popBytes = 0;
        break;
    }
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kListCtor: case kRemoveHead: case kGetOffset: case kFrustum: case kFillNEnd4: case kFillNEnd20: case kFillNEnd12: case kErase: return rTag;   // puntero como etiqueta de arena
    case kRemoveItem: return rRaw;                                                                                                                  // recuento (entero)
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
        case rRaw: obs["ret"] = eax; break;                  // entero (recuento)
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN219A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN219A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn219a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn219a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn219a"); }
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
    // Colaborador con hook RH en la DLL que corre REAL (sin falso) en ambos lados: su hook queda apagado (ruta de colaborador) y el EXE y la DLL (que lo llama por la direccion del EXE) ejecutan los bytes originales.
    auto RealOff = [&](uintptr_t exe) -> Pair {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return Pair{exe, 0, false};
    };
    env.reals[6] = {RealOff(0x4A9F70)};   // FxSphere_c::Constructor (`mov eax,ecx; mov [eax+10h],0; ret`): el EXE lo llama por su direccion; la DLL llama a su funcion (con el hook apagado salta a los bytes del EXE)
    for (unsigned f = 7; f <= 15; ++f) env.stubs[f] = {DelEv()};   // vaciados de vector (_Tidy): operator delete
    env.reals[16] = {RealOff(0x5EB2A0)};   // fill_n (stride 4) real
    env.reals[17] = {RealOff(0x5EB2E0)};   // fill_n de 20 bytes real
    env.reals[18] = {RealOff(0x5EB310)};
    env.reals[19] = {RealOff(0x5EB340)};   // fill_n de 12 bytes real
    env.reals[20] = {RealOff(0x5EB390)};
    for (unsigned f = 22; f <= 26; ++f) env.stubs[f] = {F(0x4F3F60, &FkAssignChars), F(0x4F4090, &FkFromString), ByExeAddress(0x822D5D, &FkThrow)};   // vector::_Xlen: lanza length_error
    env.stubs[27] = {NewEv(), DelEv(), FE(0x5EB2A0, &FkFillN<0x5EB2A0, 1>), FE(0x5EB930, &FkCopyWrap<0x5EB930>), FE(0x5EC070, &FkFillNEnd<0x5EC070, 1>), FE(0x5EAAA0, &FkFill<0x5EAAA0, 1>),
                     FE(0x5EB140, &FkCopyBack<0x5EB140>), FE(0x5ECAB0, &FkVecXlen<0x5ECAB0>)};
    env.reals[27] = {RealOff(0x5EAE10)};   // uninitialized_copy (stride 4) real
    env.stubs[28] = {NewEv(), DelEv(), FE(0x5EB2E0, &FkFillN<0x5EB2E0, 5>), FE(0x5EB960, &FkCopyWrap<0x5EB960>), FE(0x5EC0E0, &FkFillNEnd<0x5EC0E0, 5>), FE(0x5EB170, &FkCopyBack<0x5EB170>),
                     FE(0x5EAAC0, &FkFill<0x5EAAC0, 5>), FE(0x5ECB30, &FkVecXlen<0x5ECB30>)};
    env.reals[28] = {RealOff(0x5EAE40), RealOff(0x5E9800)};   // uninitialized_copy de 20 bytes y size() reales
    env.stubs[29] = {NewEv(), DelEv(), FE(0x5EB310, &FkFillN<0x5EB310, 1>), FE(0x5EB990, &FkCopyWrap<0x5EB990>), FE(0x5EC110, &FkFillNEnd<0x5EC110, 1>), FE(0x5EAAF0, &FkFill<0x5EAAF0, 1>),
                     FE(0x5EB1B0, &FkCopyBack<0x5EB1B0>), FE(0x5ECBB0, &FkVecXlen<0x5ECBB0>)};
    env.reals[29] = {RealOff(0x5EAE70)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 2u, 3u, 4u, 5u, 6u, 16u, 17u, 18u, 19u, 20u, 21u}) dllReal[f] = "toda la funcion (sin colaboradores falseados)";
    for (unsigned f : {6u, 16u, 17u, 18u, 19u, 20u}) dllReal[f] = "toda la funcion; el colaborador (con hook apagado) corre real: el EXE por su direccion y la DLL por su funcion o por la direccion del EXE";
    for (unsigned f : {27u, 28u, 29u}) dllReal[f] = "toda la funcion salvo los falsos; uninitialized_copy (y size() en 0x5ED1B0) con el hook apagado corren reales";
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

