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

// Banco par a par Fn220A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 29 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedNamedGlobalInstall) y V (GetType de CPedShelterAttractor). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas
// propias (A, B, C y D con guardas de 0x40).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F) o solo en la direccion del EXE cuando la DLL los llama por ella
// (FE: todos los de la STL, que el codigo de 220A llama por direccion fija como el EXE); los demas (operator new 0x82119A y delete 0x8214BD del CRT) llevan el falso solo en la direccion del EXE (ByExeAddress).
// Falsos: new/delete, vector::_Xlen (0x5ECC30, 0x5ECCB0; no vuelven: salida no local), fill_n, envoltorio de copia, fill_n que devuelve el final, fill y copy_backward de cada instancia, vector::_Insert_n / insert que llaman los
// push_back y los insert de un elemento (FE), el constructor base de CPedAttractor 0x5EDFB0 (en la DLL, el constructor real de C++ que llaman los de las clases) y CPedAttractor::Destructor 0x5EC410 (en la DLL, la llamada a Destructor()).
// Colaboradores que corren REALES (con el hook apagado): uninitialized_copy 0x5EAEA0 / 0x5EAEE0, size() 0x5E9850 y, en los destructores con borrado, el cuerpo del destructor de la clase (0x5EE530 ...).
// Las vtables que escriben los constructores se comparan como clase (la del EXE o una del ASI); los cuerpos de destructor escriben la del EXE tanto en el EXE como en la DLL (el falso de 0x5EC410 la anota).
// Memoria global del juego tocada (restaurada tras cada lado): marca del banco 0xA9B0B0 y puntero del pool de atractores 0xB744C4.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (punteros como etiqueta de arena), si la funcion termino con la salida no local y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn220a-crash.txt (junto al EXE).
namespace F220ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn220a-crash.txt";
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

namespace F220ALive {
using namespace F220AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F220AFix::Arena(WIN);
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
inline u32 gNew{};                   // lo que devuelven los falsos de operator new
inline u32 gRet{};                   // EAX que devuelven los falsos de los envoltorios de copia, de fill_n que devuelve el final y del cuerpo 0x5EC410
inline u32 gDtorWrite{};             // dword que el falso de 0x5EC410 deja en [this]
inline bool gModFirst{};             // el falso de insert cambia el inicio del vector ([this + 4])
inline u32 gNewFirst{};
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
// vector::_Insert_n (0x5ED750, 0x5EDA50, 0x5ECF10, 0x5ED4B0, 0x5ED1B0; thiscall (pos, n, &valor), `ret 0Ch`): evento [direccion, ECX, pos, n, &valor (etiqueta), palabras del valor]; si gModFirst deja gNewFirst en [ECX + 4].
template<u32 ADDR, unsigned WORDS> inline void __fastcall FkInsN(void* self, void*, void* pos, u32 n, const u32* value) {
    Ev& e = PushEv(ADDR, 4 + WORDS);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(pos)); e.v[2] = n; DumpTag(e, 3, uintptr_t(value));
    for (unsigned i = 0; i < WORDS; ++i) e.v[4 + i] = value[i];
    if (gModFirst) memcpy(static_cast<u8*>(self) + 4, &gNewFirst, 4);
}
// vector::insert de un elemento (0x5EDD90, 0x5EDE50; thiscall (salida, pos, &valor), `ret 0Ch`; devuelve la salida): evento [direccion, ECX, salida, pos, &valor (etiquetas), palabras del valor].
template<u32 ADDR, unsigned WORDS> inline u32 __fastcall FkIns1(void* self, void*, void* out, void* pos, const u32* value) {
    Ev& e = PushEv(ADDR, 4 + WORDS);
    DumpTag(e, 0, uintptr_t(self)); DumpTag(e, 1, uintptr_t(out)); DumpTag(e, 2, uintptr_t(pos)); DumpTag(e, 3, uintptr_t(value));
    for (unsigned i = 0; i < WORDS; ++i) e.v[4 + i] = value[i];
    return u32(uintptr_t(out));
}
// CPedAttractor::CPedAttractor (0x5EDFB0; thiscall, once dwords de pila, `ret 2Ch`): evento [direccion, ECX (etiqueta), los once argumentos (crudos)]; devuelve this; no escribe nada.
inline u32 __fastcall FkBaseCtor(void* self, void*, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    Ev& e = PushEv(0x5EDFB0, 12);
    DumpTag(e, 0, uintptr_t(self));
    e.v[1] = a0; e.v[2] = a1; e.v[3] = a2; e.v[4] = a3; e.v[5] = a4; e.v[6] = a5; e.v[7] = a6; e.v[8] = a7; e.v[9] = a8; e.v[10] = a9; e.v[11] = a10;
    return u32(uintptr_t(self));
}
// Cuerpo sin argumentos de pila (thiscall; 0x5EC410): evento [direccion, ECX (etiqueta), dword que tenia [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    DumpTag(e, 0, uintptr_t(self));
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
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
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};
// Dwords de vtable que la funcion escribe (arena, desplazamiento, vtable del EXE): se omiten de los cambios y se anotan como clase ("vt" si es la del EXE o una de la DLL)
struct VptrSlot { unsigned arena; u32 off; u32 exeVt; };
inline std::vector<VptrSlot> vptrSlots;

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind k = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(IsStl(k) ? 0u : a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);   // las funciones de la STL copian de la arena B: siempre datos aleatorios (otros rellenos harian invisibles las copias)
    gNew = 0; gRet = 0; gDtorWrite = 0; gModFirst = false; gNewFirst = 0;
    vptrSlots.clear();
    *reinterpret_cast<u8*>(kBenchMarker) = u8(kMarkerValue);   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const auto stdcall_ = [&](u32 n) { call.argCount = n; call.popBytes = 0; };
    switch (k) {
    case kInsN12:
    case kInsN4: {
        const u32 S = StrideOf(k), words = S / 4;
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        const u32 last = first == 0 ? 0u : first + S * a[6];
        const u32 end = first == 0 ? a[18] : first + S * a[5];
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, end);
        for (u32 i = 0; i < words; ++i) arenaC.Put32(a[15] + 4 * i, a[10 + i]);
        const u32 pos = first == 0 ? 0u : first + S * a[8];
        gNew = u32(arenaD.Obj(a[16])); gRet = a[17];
        call.args[0] = pos; call.args[1] = a[9]; call.args[2] = u32(arenaC.Obj(a[15])); stdcall_(3);
        break;
    }
    case kIns20:
    case kIns12: {
        const u32 S = StrideOf(k), words = S / 4;
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        const u32 last = first == 0 ? a[22] : first + S * a[6];
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, a[21]);
        for (u32 i = 0; i < words; ++i) arenaC.Put32(a[15] + 4 * i, a[10 + i]);
        const u32 pos = a[9] == 1 ? a[19] : (first == 0 ? 0u : first + S * a[8]);
        const u32 out = a[16] == 0 ? u32(arenaC.Obj(a[17])) : (a[16] == 1 ? thisA + 4 : (a[16] == 2 ? thisA + 8 : thisA + 0xC));
        gModFirst = a[18] != 0; gNewFirst = a[20];
        call.args[0] = out; call.args[1] = pos; call.args[2] = u32(arenaC.Obj(a[15])); stdcall_(3);
        break;
    }
    case kPb4:
    case kPb20:
    case kPb12: {
        const u32 S = StrideOf(k), words = S / 4;
        const u32 first = a[4] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        const u32 last = first == 0 ? a[19] : first + S * a[6];
        const u32 end = first == 0 ? a[18] : first + S * a[5];
        arenaA.Put32(o + 4, first); arenaA.Put32(o + 8, last); arenaA.Put32(o + 0xC, end);
        for (u32 i = 0; i < words; ++i) arenaC.Put32(a[15] + 4 * i, a[10 + i]);
        call.args[0] = u32(arenaC.Obj(a[15])); stdcall_(1);
        break;
    }
    case kMgr:
    case kType:
        break;
    case kACtor:
        vptrSlots.push_back({0, o, kClsVt[kCls[c.fn]]});
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; stdcall_(3);
        break;
    case kASdd: {
        gRet = a[5]; gDtorWrite = a[6];
        const i32 idx = i32(a[9]);
        const int64 diff = int64(idx) * kAttractorSize + a[10];
        const u32 pool = a[7];
        arenaB.Put32(pool + 0, u32(int64(thisA) - diff)); arenaB.Put32(pool + 4, u32(arenaD.Obj(a[8]))); arenaB.Put32(pool + 8, 0x40); arenaB.Put32(pool + 0xC, a[11]);
        arenaD.m[kGuard + a[8] + u32(idx)] &= 0x7Fu;   // el hueco esta usado
        *reinterpret_cast<u32*>(kAttractorPool) = u32(arenaB.Obj(pool));
        call.args[0] = a[4]; stdcall_(1);
        break;
    }
    case kABody:
        gRet = a[5]; gDtorWrite = a[6];
        break;
    }
    return call;
}

enum RetKind { rNone, rTag, rRaw };
inline RetKind RetOf(Kind k) {
    switch (k) {
    case kIns20: case kIns12: case kMgr: case kACtor: case kASdd: return rTag;   // puntero como etiqueta de arena
    case kType: return rRaw;                                                  // valor crudo
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
        std::array<std::vector<u32>, 4> skip;
        json vptrs = json::array();
        for (const auto& v : vptrSlots) {
            skip[v.arena].push_back(v.off);
            u32 val{}; memcpy(&val, ArenaAt(v.arena).m.data() + kGuard + v.off, 4);
            const bool ok = val == v.exeVt || ModuleOf(val) == env.asi;
            vptrs.push_back(json::array({v.arena, v.off, ok ? json("vt") : json(val)}));
        }
        obs["changes"] = {{"A", arenaA.Changes(skip[0])}, {"B", arenaB.Changes(skip[1])}, {"C", arenaC.Changes(skip[2])}, {"D", arenaD.Changes(skip[3])}};
        obs["vptrs"] = vptrs;
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
// Casos por funcion: kCases salvo que la variable de entorno FN220A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN220A_CASES", buffer, sizeof(buffer)) > 0) {
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

// Constructor base de CPedAttractor: la funcion de la DLL (hook S sobre la envoltura `Constructor`) llama a std::construct_at, que llama al constructor real; los constructores de los atractores llaman a ese ultimo.
// Se busca por la cadena de llamadas de la envoltura (hasta cuatro niveles dentro del ASI): la primera funcion que escribe una vtable del ASI (`mov dword ptr [reg], imm32` con el inmediato en el ASI) es el constructor.
inline bool HasVptrStore(uintptr_t fn, uintptr_t asi) {
    const uint8* q = reinterpret_cast<const uint8*>(fn);
    for (size_t i = 0; i + 6 <= 0x400; ++i) {
        if (q[i] == 0xC7 && (q[i + 1] <= 0x03 || q[i + 1] == 0x06 || q[i + 1] == 0x07)) {
            const uint32 imm = *reinterpret_cast<const uint32*>(q + i + 2);
            if (ModuleOf(imm) == asi && imm >= 0x10000) return true;
        }
    }
    return false;
}
inline uintptr_t FindCtorChain(uintptr_t fn, unsigned depth, uintptr_t asi) {
    if (!fn || depth > 4 || ModuleOf(fn) != asi) return 0;
    if (HasVptrStore(fn, asi)) return fn;
    const uint8* q = reinterpret_cast<const uint8*>(fn);
    for (size_t i = 0; i + 5 <= 0x300; ++i) {
        if (q[i] == 0xE8) {
            const uintptr_t raw = fn + i + 5 + *reinterpret_cast<const int32*>(q + i + 1);
            if (ModuleOf(raw) != asi || IsBadReadPtr(reinterpret_cast<const void*>(raw), 8)) continue;
            const uintptr_t target = Body(raw);
            if (ModuleOf(target) == asi && target != fn && !IsBadReadPtr(reinterpret_cast<const void*>(target), 0x400)) {
                if (const uintptr_t r = FindCtorChain(target, depth + 1, asi)) return r;
            }
        }
    }
    return 0;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn220a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn220a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn220a"); }
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
    // Constructor base (CPedAttractor::Constructor 0x5EDFB0, hook S sobre la envoltura): el falso va en la entrada del EXE (hook apagado) y en el constructor real de C++ que llaman los de las clases derivadas de la DLL
    auto FC = [&](uintptr_t exe, auto* fake) {
        Pair p = Collab(exe);
        const uintptr_t ctor = p.dll ? FindCtorChain(p.dll, 0, env.asi) : 0;
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
    // Colaborador REAL que es funcion con hook (hook apagado): el EXE corre sus bytes y la DLL llama a su cuerpo
    auto R = [&](uintptr_t exe) -> Pair {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;
        return p;
    };
    // vector::_Insert_n de 12 y de 4 bytes (instancia C): nueva, delete, falsos de la STL con la direccion de cada instancia, vector::_Xlen y la copia (real) con size() (real, solo la de 12 bytes)
    env.stubs[0] = {NewEv(), DelEv(), FE(0x5EB340, &FkFillN<0x5EB340, 3>), FE(0x5EB9D0, &FkCopyWrap<0x5EB9D0>), FE(0x5EC140, &FkFillNEnd<0x5EC140, 3>), FE(0x5EB220, &FkCopyBack<0x5EB220>),
                    FE(0x5EB1E0, &FkFill<0x5EB1E0, 3>), FE(0x5ECC30, &FkVecXlen<0x5ECC30>)};
    env.reals[0] = {RealOff(0x5EAEA0), RealOff(0x5E9850)};
    env.stubs[1] = {NewEv(), DelEv(), FE(0x5EB390, &FkFillN<0x5EB390, 1>), FE(0x5EBA00, &FkCopyWrap<0x5EBA00>), FE(0x5EC1B0, &FkFillNEnd<0x5EC1B0, 1>), FE(0x5EB260, &FkCopyBack<0x5EB260>),
                    FE(0x5EAB10, &FkFill<0x5EAB10, 1>), FE(0x5ECCB0, &FkVecXlen<0x5ECCB0>)};
    env.reals[1] = {RealOff(0x5EAEE0)};
    env.stubs[2] = {FE(0x5ED1B0, &FkInsN<0x5ED1B0, 5>)};
    env.stubs[3] = {FE(0x5ED750, &FkInsN<0x5ED750, 3>)};
    env.stubs[5] = {FE(0x5EB2A0, &FkFillN<0x5EB2A0, 1>), FE(0x5ECF10, &FkInsN<0x5ECF10, 1>)};
    env.stubs[6] = {FE(0x5EB2E0, &FkFillN<0x5EB2E0, 5>), FE(0x5EDD90, &FkIns1<0x5EDD90, 5>)};
    env.stubs[7] = {FE(0x5EB310, &FkFillN<0x5EB310, 1>), FE(0x5ED4B0, &FkInsN<0x5ED4B0, 1>)};
    env.stubs[8] = {FE(0x5EB340, &FkFillN<0x5EB340, 3>), FE(0x5EDE50, &FkIns1<0x5EDE50, 3>)};
    env.stubs[9] = {FE(0x5EB390, &FkFillN<0x5EB390, 1>), FE(0x5EDA50, &FkInsN<0x5EDA50, 1>)};
    // atractores: constructor base falso, cuerpo del destructor base (0x5EC410) falso; el cuerpo del destructor de la clase es REAL en los destructores con borrado
    for (unsigned f = 0; f < kFunctions; ++f) {
        switch (kKind[f]) {
        case kACtor: env.stubs[f] = {FC(0x5EDFB0, &FkBaseCtor)}; break;
        case kASdd: env.stubs[f] = {F(0x5EC410, &FkBody<0x5EC410>)}; env.reals[f] = {R(kEntry[f + 1])}; break;
        case kABody: env.stubs[f] = {F(0x5EC410, &FkBody<0x5EC410>)}; break;
        default: break;
        }
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u}) dllReal[f] = "toda la funcion salvo los falsos; uninitialized_copy (y size() en 0x5ED750) con el hook apagado corren reales en ambos lados (la DLL los llama por la direccion del EXE)";
    for (unsigned f : {4u, 23u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        if (kKind[f] == kASdd) dllReal[f] = "toda la funcion; el cuerpo del destructor de la clase corre real (el EXE sus bytes, la DLL su cuerpo), CPedAttractor::Destructor 0x5EC410 es falso identico y el borrado del pool de atractores (EXE: en linea; DLL: CPedAttractor::operator delete) corre real sobre el pool sintetico";
        if (kKind[f] == kABody) dllReal[f] = "toda la funcion; CPedAttractor::Destructor 0x5EC410 es falso identico (en la DLL, la llamada a CPedAttractor::Destructor)";
        if (kKind[f] == kACtor) dllReal[f] = "toda la funcion; el constructor base 0x5EDFB0 es falso (en la DLL, el constructor real de C++ que llama el de la clase)";
    }
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
    // Memoria global del juego tocada: marca del banco (todas las funciones) y puntero del pool de atractores (destructores con borrado); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kAttractorPool, 4);
    const size_t poolRegion = regions.size() - 1;
    regions.emplace_back(kBenchMarker, 1);
    const size_t markerRegion = regions.size() - 1;
    for (unsigned f = 0; f < kFunctions; ++f) {
        if (kKind[f] == kASdd) regionsOf[f].push_back(poolRegion);
        regionsOf[f].push_back(markerRegion);
    }
    Run(env, ready, registered, bodies, collaborators);
}
}
