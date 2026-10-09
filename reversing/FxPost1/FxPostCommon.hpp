#pragma once
// FxPost1: utilidades comunes (copiadas del modelo GfxWave2/GeoFinal1). Compartidas por FxPart.hpp y PostPart.hpp.
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeRuntime.hpp"

namespace FxPost1Live {
using Hook = ReversibleHooks::ReversibleHook::Base;
inline bool done{}; inline std::ofstream log;
// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline uint32 Fnv32(const void* p, size_t n) { const auto h = Fnv(p, n); return uint32(h ^ (h >> 32)); }
inline uint32 P(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline float FromBits(uint32 b) { return std::bit_cast<float>(b); }
inline uint32 Rnd(uint32& s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
inline uint32 Seed(unsigned i, uint32 salt) { uint32 s = i * 2654435761u + salt * 0x9E3779B9u + 1u; if (!s) s = 1; for (int k = 0; k < 4; ++k) Rnd(s); return s; }
template<class T> inline T& At(uint8* base, size_t off) { return *reinterpret_cast<T*>(base + off); }
inline uint32 U(int32 v) { return uint32(v); }
template<class T, size_t N> inline const T& Pick(uint32& s, const T (&t)[N]) { return t[Rnd(s) % N]; }
inline int RgI(uint32& s, int lo, int hi) { return lo + int(Rnd(s) % uint32(hi - lo + 1)); }
// Convierte un NaN de senalizacion en NaN silencioso (el x87 los silencia al copiar con fld/fstp; el EXE y la DLL copian crudo aqui, pero nunca se generan sNaN).
inline uint32 QuietNaN(uint32 v) { if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x007FFFFFu)) v |= 0x00400000u; return v; }
inline bool Chance(uint32& s, unsigned percent) { return Rnd(s) % 100 < percent; }

// Buffer con guardas de 16 bytes a cada lado (se comparan enteros: detectan escrituras fuera de rango).
template<size_t N> struct Buf {
    alignas(16) std::array<uint8, N + 32> raw{};
    uint8* p() { return raw.data() + 16; }
    const uint8* p() const { return raw.data() + 16; }
    void Fill(uint8 v) { raw.fill(v); }
    std::vector<uint8> Bytes() const { return { raw.begin(), raw.end() }; }
};
inline json Arr(const uint8* p, size_t n) { json a = json::array(); for (size_t i = 0; i < n; ++i) a.push_back(p[i]); return a; }

// Floats de los fixtures. Se evitan magnitudes que desborden/infradesborden al elevar al cuadrado: el x87 conserva el
// exponente extendido y SSE no. Hay inf y NaN silencioso (nunca NaN de senalizacion).
inline float PickRand(uint32& s) { const uint32 r = Rnd(s); return std::bit_cast<float>((r & 0x80000000u) | ((100u + Rnd(s) % 50u) << 23) | (r & 0x7FFFFFu)); }
inline float PickSmall(uint32& s) { static constexpr float t[] = { 0.f, 0.5f, -0.5f, 1.f, -1.f, 2.f, -2.f, 3.f, -3.f, 1.5f, -1.5f, 10.25f, -17.75f, 100.125f, 0.0625f, -0.001f, 640.f, 448.f }; return t[Rnd(s) % std::size(t)]; }
// Con inf y NaN silencioso (un solo NaN por operacion: x87 y SSE eligen distinta carga util si hay dos).
inline float PickF(uint32& s) {
    static constexpr float t[] = { 0.f, 1.f, -1.f, 0.5f, -0.5f, 2.5f, -2.5f, 0.75f, -0.25f, 1000.125f, -1000.125f, 123456.7f, -123456.7f, 1e10f, -3e14f, 1e-3f, -1e-3f, 7.f, -7.f, 0.1f,
        std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), std::bit_cast<float>(0x7FC00000u) };
    if (Rnd(s) % 100 < 35) return PickRand(s);
    return t[Rnd(s) % std::size(t)];
}
inline float PickFinite(uint32& s) { float v; do { v = PickF(s); } while (!std::isfinite(v)); return v; }

// --- registro de trazas sin reservas (valido mientras se parchean funciones del asignador) ---
struct Rec { uint32 tag; uint32 n; uint32 a[8]; };
inline std::array<Rec, 6144> recs{}; inline size_t nrec{};
inline void Push(uint32 t, std::initializer_list<uint32> v = {}) {
    if (!Mine() || nrec >= recs.size()) return;
    auto& r = recs[nrec++]; r = {}; r.tag = t; for (auto x : v) if (r.n < 8) r.a[r.n++] = x;
}
inline json Recs() { json r = json::array(); for (size_t i = 0; i < nrec; ++i) { json e = json::array({ recs[i].tag }); for (uint32 k = 0; k < recs[i].n; ++k) e.push_back(recs[i].a[k]); r.push_back(e); } if (nrec >= recs.size()) r.push_back("TRACE_OVERFLOW"); return r; }

inline Hook* FindHook(const char* cls, const char* fn) {
    Hook* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo). Regla 20: no entra en el EXE.
inline uintptr_t Body(uintptr_t a) {
    for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) {
        const uintptr_t t = a + 5 + *reinterpret_cast<int32*>(a + 1);
        if (t >= 0x400000 && t < 0x1000000) break;
        a = t;
    }
    return a;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
struct GlobalRange { uintptr_t addr; size_t size; };

// Unidad de control x87: 24 bits durante la llamada (como el motor), nrec a cero.
struct Ctl {
    unsigned savedFp;
    Ctl() { savedFp = _controlfp(0, 0); _controlfp(_PC_24, _MCW_PC); nrec = 0; }
    ~Ctl() { _controlfp(savedFp, _MCW_PC); }
};

// Parches locales de colaboradores: entrada del EXE + simbolo de la DLL (+ cuerpo si es un thunk E9).
// Regla 11: todos los Body() se calculan ANTES de crear ningun EntryPatch.
struct IsoSite { uintptr_t exe; uintptr_t dll; void* stub; };
struct Iso {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    explicit Iso(const std::vector<IsoSite>& l) {
        std::vector<uintptr_t> bodies; for (auto& s : l) bodies.push_back(s.dll ? Body(s.dll) : 0);
        size_t k = 0;
        for (auto& s : l) {
            if (s.exe) p.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.exe, s.stub));
            if (s.dll) {
                p.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.dll, s.stub));
                if (bodies[k] != s.dll) p.push_back(std::make_unique<ReversingProbe::EntryPatch>(bodies[k], s.stub));
            }
            ++k;
        }
    }
};
template<class F> inline void* SP(F f) { return reinterpret_cast<void*>(f); }
template<class F> inline uintptr_t DA(F f) { return std::bit_cast<uintptr_t>(f); }

// Copia/restauracion de rangos de memoria global.
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> SaveRanges(const std::vector<GlobalRange>& g) { std::vector<Saved> v; for (auto& r : g) v.push_back({ r.addr, ReversingProbe::Bytes(reinterpret_cast<void*>(r.addr), r.size) }); return v; }
inline void RestoreRanges(const std::vector<Saved>& v) { for (auto& r : v) std::memcpy(reinterpret_cast<void*>(r.addr), r.bytes.data(), r.bytes.size()); }
inline bool SameRanges(const std::vector<Saved>& v) { for (auto& r : v) if (ReversingProbe::Bytes(reinterpret_cast<void*>(r.addr), r.bytes.size()) != r.bytes) return false; return true; }
inline json HashRanges(const std::vector<GlobalRange>& g) { json h = json::array(); for (auto& r : g) h.push_back(Fnv32(reinterpret_cast<void*>(r.addr), r.size)); return h; }
}
