#pragma once
// GfxWave2: utilidades comunes (copiadas de GeoFinal1). Compartidas por ClothesPart.hpp, ShadowsPart.hpp y WaterPart.hpp.
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeRuntime.hpp"

namespace GfxWave2Live {
using Hook = ReversibleHooks::ReversibleHook::Base;
inline bool done{}; inline std::ofstream log;
// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline uint32 P(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline uint32 Rnd(uint32& s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
inline uint32 Seed(unsigned i, uint32 salt) { uint32 s = i * 2654435761u + salt * 0x9E3779B9u + 1u; if (!s) s = 1; for (int k = 0; k < 4; ++k) Rnd(s); return s; }
template<class T> inline T& At(uint8* base, size_t off) { return *reinterpret_cast<T*>(base + off); }

// Buffer con guardas de 16 bytes a cada lado (se comparan enteros: detectan escrituras fuera de rango).
template<size_t N> struct Buf {
    alignas(16) std::array<uint8, N + 32> raw{};
    uint8* p() { return raw.data() + 16; }
    void Fill(uint8 v) { raw.fill(v); }
    std::vector<uint8> Bytes() const { return { raw.begin(), raw.end() }; }
};

// Floats de los fixtures. Se evitan magnitudes que desborden/infradesborden al elevar al cuadrado: el x87 conserva el
// exponente extendido y SSE no. Hay inf y NaN silencioso (nunca NaN de senalizacion).
inline float PickNear(uint32& s) { static constexpr float t[] = { 0.f, 0.5f, -0.5f, 1.f, -1.f, 2.f, -2.f, 3.f, -3.f, 1.5f, -1.5f }; return t[Rnd(s) % std::size(t)]; }
inline float PickRand(uint32& s) { const uint32 r = Rnd(s); return std::bit_cast<float>((r & 0x80000000u) | ((100u + Rnd(s) % 50u) << 23) | (r & 0x7FFFFFu)); }
inline float PickF(uint32& s) {
    static constexpr float t[] = { 0.f, 1.f, -1.f, 0.5f, -0.5f, 2.5f, -2.5f, 0.75f, -0.25f, 1000.125f, -1000.125f, 123456.7f, -123456.7f, 1e10f, -3e14f, 1e-3f, -1e-3f, 7.f, -7.f, 0.1f,
        std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::bit_cast<float>(0x7FC00000u) };
    if (Rnd(s) % 100 < 35) return PickRand(s);
    return t[Rnd(s) % std::size(t)];
}

// --- registro de trazas sin reservas (valido mientras operator new/delete estan sustituidos) ---
struct Rec { uint32 tag; uint32 n; uint32 a[8]; };
inline std::array<Rec, 2048> recs{}; inline size_t nrec{};
inline void Push(uint32 t, std::initializer_list<uint32> v = {}) {
    if (!Mine() || nrec >= recs.size()) return;
    auto& r = recs[nrec++]; r = {}; r.tag = t; for (auto x : v) if (r.n < 8) r.a[r.n++] = x;
}
inline json Recs() { json r = json::array(); for (size_t i = 0; i < nrec; ++i) { json e = json::array({ recs[i].tag }); for (uint32 k = 0; k < recs[i].n; ++k) e.push_back(recs[i].a[k]); r.push_back(e); } return r; }

inline Hook* FindHook(const char* cls, const char* fn) {
    Hook* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a) { for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) a += 5 + *reinterpret_cast<int32*>(a + 1); return a; }
// Constructor de la DLL (no se puede tomar la direccion de un constructor): primera llamada a la DLL que sigue a la del
// operator new correspondiente dentro de la funcion (copiado de Misc8).
inline uintptr_t FindCtorCall(uintptr_t fn, uintptr_t newFn) {
    HMODULE mod{}; if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(fn), &mod)) return 0;
    const auto modBase = uintptr_t(mod); const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase + reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew);
    const uintptr_t modEnd = modBase + nt->OptionalHeader.SizeOfImage;
    auto FollowIn = [&](uintptr_t q) { for (int k = 0; k < 4 && q >= modBase && q < modEnd && *reinterpret_cast<const uint8*>(q) == 0xE9; ++k) q = q + 5 + *reinterpret_cast<const int32*>(q + 1); return q; };
    fn = FollowIn(fn); const auto nf = FollowIn(newFn); bool seen = false;
    for (uintptr_t p = fn; p < fn + 0x1200; ++p) {
        if (*reinterpret_cast<const uint8*>(p) != 0xE8) continue;
        const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1);
        if (!seen) { if (t >= modBase && t < modEnd && FollowIn(t) == nf) { seen = true; p += 4; } continue; }
        if (t < modBase || t >= modEnd) continue;
        if (*reinterpret_cast<const uint8*>(t) != 0xE9 && (t & 15)) continue;
        return t;
    }
    return 0;
}

// Parche de entrada: direccion del EXE + simbolo de la DLL (+ cuerpo si el simbolo es un thunk E9).
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
struct Stub { const char* name; uintptr_t exeAddr, dll, stub; };
// Cada parte devuelve: sus stubs, los rangos globales que toca (se guardan/restauran por ejecucion) y su despachador de casos.
struct GlobalRange { uintptr_t addr; size_t size; };
}
