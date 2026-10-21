#pragma once
// Fixtures del banco Helpers65A (lote 65): cinco ayudantes pequenos del EXE que el codigo fuente llamaba por direccion, 4096 casos cada uno. Solo C++ estandar (sin tipos del
// juego): las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   0  FloorFloat (CHud, floorf del CRT)              0x406CE0  cdecl(float), ST0 = double            fld dword; fstp qword; call floor del CRT del EXE (0x8219F0)
//   1  GetAnimHierarchyFromFrameCallback (RwHelper)   0x734A70  cdecl(RwFrame*, void*), EAX           callback de RwFrameForAllChildren: guarda la jerarquia en *data o recorre los hijos
//   2  CDecisionMaker::MakeDecision                   0x606060  thiscall, 9 args de pila, ret 24h     this[eventIndex].MakeDecision(...) sobre CDecision (0x6040D0)
//   3  CDecisionMakerTypes::ReadEventIndices          0x5BB9F0  stdcall(int32*, const char*), ret 8   lee data\decision\<archivo> y rellena los 96 indices de eventos
//   4  UserTracksLegacyVector::PushBack               0x4F4620  thiscall, 1 arg de pila, ret 4        push_back del vector VC6 de punteros
//
// Toda la memoria que tocan las funciones esta en una arena de 4 KB (zonas por funcion, ver kZ*). El caso se describe por su `input` (JSON); la imagen previa de la arena es
// basura determinista (Garbage) salvo los campos del caso, para que el oraculo pueda reconstruirla.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace H65Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;
using i16 = std::int16_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 5;
constexpr u32 kFloor = 0, kCallback = 1, kDecisionMaker = 2, kReadEvents = 3, kPushBack = 4;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

// ---- arena (4 KB). Cada zona es multiplo de 16; todo lo que no es campo del caso es basura que NUNCA debe cambiar salvo escritura prevista.
constexpr u32 kArenaBytes = 0x1000;
constexpr u32 kArenaWords = kArenaBytes / 4;                 // 1024
constexpr u32 kFrames = 0x100;                               // F1: 10 marcos falsos de 16 bytes (0x100..0x19F)
constexpr u32 kFrameStride = 0x10;
constexpr u32 kNodes = 10;
constexpr u32 kFound = 0x1A0;                                // F1: palabra *data
constexpr u32 kThisZone = 0x200;                             // F2: "this" (nunca se desreferencia)
constexpr u32 kTaskSlot = 0x240;                             // F2: palabra con el int16 de la tarea (en los 16 bits bajos)
constexpr u32 kFacialSlot = 0x244;                           // F2: palabra con el int16 facial
constexpr u32 kIndices = 0x400;                              // F3: 96 enteros (0x400..0x57F); guarda previa 0x300..0x3FF, posterior 0x580..0x97F
constexpr u32 kIndexCount = 96;
constexpr u32 kNameZone = 0x980;                             // F3: nombre del archivo (hasta 64 bytes: 0x980..0x9BF)
constexpr u32 kNameBytes = 64;
constexpr u32 kVector = 0xA00;                               // F4: vector VC6 {allocator, first, last, end} (16 bytes)
constexpr u32 kValueSlot = 0xA20;                            // F4: palabra con el puntero a empujar
constexpr int kIdxMin = -64, kIdxMax = 350;                  // indices que caen dentro de la arena: 0x400 + 4*(-64) = 0x300; 0x400 + 4*350 = 0x978
static_assert(kIndices + 4 * kIdxMax + 4 <= kNameZone && kIndices - 4 * (-kIdxMin) == 0x300, "limites de indice");
static_assert(kNameZone + kNameBytes <= kVector && kValueSlot + 4 <= kArenaBytes && kFrames + kNodes * kFrameStride <= kFound, "zonas");
constexpr u32 kMaxEvents = 64;

inline u32 Garbage(u32 n, u32 k) {   // basura determinista (dos bits altos a 1, nunca 0xFFFFFFFF): la misma formula en fixture_oracle.py
    u32 v = 0xC0000000u | (Rand(n + 0x6A09E667u, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}
inline u32 ImageSeed(u32 fn, u32 n) { return n + 0x500u + fn * 7919u; }

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

constexpr u32 kFamilies = 12;

// ------------------------------------------------------------------------------------------------ 0 FloorFloat
// Fake de 0x8219F0 (floor del CRT, cdecl, un double = dos palabras de pila): devuelve en ST0 el double segun el modo y la semilla.
inline u64 FloorRetBits(u32 mode, u32 seed, u32 lo, u32 hi) {
    const u64 arg = (u64(hi) << 32) | lo;
    const u64 r = (u64(Rand(seed, 1)) << 32) | Rand(seed, 2);
    u64 v = 0;
    switch (mode) {
    case 0: v = 0; break;                                                              // +0.0
    case 1: v = 0x8000000000000000ull; break;                                          // -0.0
    case 2: v = arg; break;                                                            // el propio argumento
    case 3: v = arg ^ (r & 0x000FFFFFFFFFFFFFull); break;                              // argumento con la mantisa revuelta
    case 4: { u64 e = 1 + (Rand(seed, 3) % 2046); v = (r & 0x8000000000000000ull) | (e << 52) | (r & 0x000FFFFFFFFFFFFFull); break; }   // normal
    case 5: v = (r & 0x8000000000000000ull) | (r & 0x000FFFFFFFFFFFFFull) | 1ull; break;                                              // denormal
    case 6: v = (r & 0x8000000000000000ull) | 0x7FF0000000000000ull; break;                                                          // +-inf
    default: v = (r & 0x8000000000000000ull) | 0x7FF8000000000000ull | (r & 0x0007FFFFFFFFFFFFull); break;                            // QNaN con carga
    }
    if (((v >> 52) & 0x7FF) == 0x7FF && (v & 0x000FFFFFFFFFFFFFull)) v |= 0x0008000000000000ull;   // sin NaN senalizantes (fld los silencia)
    return v;
}
struct SpecFloor {
    u32 fam{}, x{}, mode{}, seed{}, stack{}, eax{}, ecx{}, edx{};
};
inline SpecFloor MakeFloor(u32 n) {
    SpecFloor s;
    Gen g(n + kFloor * 0x20000u);
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    s.fam = n % kFamilies;
    s.seed = g.R();
    s.mode = g.P(8);
    const u32 sign = g.C(50) ? 0x80000000u : 0u;
    static const u32 border[]{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x4AFFFFFFu, 0x4B000000u, 0x4B000001u,
                              0x4B800000u, 0x4F000000u, 0x5F000000u, 0x5F800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x00800000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x80000000u, 0x00000000u,
                              0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0xFFFFFFFFu, 0x3FC00000u, 0xC0000000u};
    switch (s.fam) {
    case 0: s.x = sign | ((1 + g.P(254)) << 23) | (g.R() & 0x7FFFFFu); break;                       // normal cualquiera
    case 1: s.x = sign | ((100 + g.P(50)) << 23) | (g.R() & 0x7FFFFFu); break;                      // |x| < 2^23 con parte fraccionaria
    case 2: {                                                                                       // enteros pequenos (float(k) exacto)
        const int k = int(g.P(2001)) - 1000;
        float f = float(k);
        std::memcpy(&s.x, &f, 4);
        break;
    }
    case 3: { float f = float(int(g.P(4001)) - 2000) / 4.0f; std::memcpy(&s.x, &f, 4); break; }     // cuartos
    case 4: s.x = (g.C(50) ? 0x7F800000u : 0u) | sign; break;                                       // +-0, +-inf
    case 5: s.x = sign | (1 + g.P(0x7FFFFFu)); break;                                               // denormales
    case 6: s.x = sign | 0x7F800000u | (1 + g.P(0x7FFFFFu)); break;                                 // NaN (silenciosos y senalizantes)
    case 7: s.x = sign | ((150 + g.P(105)) << 23) | (g.R() & 0x7FFFFFu); break;                     // >= 2^23 (enteros) hasta inf/NaN incluidos
    case 8: s.x = border[g.P(sizeof(border) / sizeof(border[0]))]; break;                           // fronteras
    case 9: s.x = g.R(); break;                                                                     // bits cualesquiera
    case 10: { float f = float(int(g.P(65536)) - 32768); u32 b; std::memcpy(&b, &f, 4); s.x = b + (g.C(50) ? 1u : u32(-1)); break; }   // un ulp de un entero
    default: s.x = sign | ((1 + g.P(254)) << 23) | (g.R() & 0x7FFFFFu); break;
    }
    return s;
}
inline u64 WidenFloat(u32 x) {   // fld dword; fstp qword: exacto, NaN silenciados
    const u64 sign = u64(x >> 31) << 63;
    const u32 e = (x >> 23) & 0xFF, f = x & 0x7FFFFFu;
    if (e == 255) return sign | 0x7FF0000000000000ull | (f ? (u64(f) << 29) | 0x0008000000000000ull : 0);
    if (e == 0) {
        if (!f) return sign;
        unsigned bl = 0;
        while ((f >> bl) != 0) ++bl;                    // bit_length
        const u64 exp = u64(int(bl) - 1 - 149 + 1023);
        const u64 mant = (u64(f) << (52 - (bl - 1))) & 0xFFFFFFFFFFFFFull;
        return sign | (exp << 52) | mant;
    }
    return sign | (u64(e - 127 + 1023) << 52) | (u64(f) << 29);
}

// ------------------------------------------------------------------------------------------------ 1 GetAnimHierarchyFromFrameCallback
struct SpecCallback {
    u32 fam{}, n{}, root{}, found0{}, rg{}, stack{}, eax{}, ecx{}, edx{};
    int parent[kNodes]{};
    u32 hier[kNodes]{};
};
inline SpecCallback MakeCallback(u32 n) {
    SpecCallback s;
    Gen g(n + kCallback * 0x20000u);
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    s.fam = n % kFamilies;
    s.n = 2 + g.P(kNodes - 1);                                   // 2..10 nodos
    for (u32 i = 0; i < kNodes; ++i) { s.parent[i] = -1; s.hier[i] = 0; }
    for (u32 i = 1; i < s.n; ++i) {
        switch (s.fam % 3) {
        case 0: s.parent[i] = int(i) - 1; break;                  // cadena
        case 1: s.parent[i] = 0; break;                           // estrella
        default: s.parent[i] = int(g.P(i)); break;                // arbol aleatorio (padre de indice menor)
        }
    }
    auto value = [&]() -> u32 {
        const u32 pick = g.P(8);
        return pick == 0 ? 1u : pick == 1 ? 0xFFFFFFFFu : pick == 2 ? 0x80000000u : g.R() | 1u;
    };
    switch (s.fam) {
    case 3: case 4: break;                                       // ninguna jerarquia: recorrido completo
    case 5: s.hier[s.fam == 5 ? g.P(s.n) : 0] = value(); break;  // una sola, en un nodo cualquiera
    case 6: s.hier[s.n - 1] = value(); break;                    // solo la del ultimo nodo
    case 7: s.hier[0] = value(); break;                          // la raiz
    default: for (u32 i = 0; i < s.n; ++i) if (g.C(25)) s.hier[i] = value(); break;
    }
    s.root = (s.fam == 7 || s.fam == 3) ? 0u : g.P(s.n);
    if (s.fam == 8) s.root = 0;
    s.found0 = g.C(30) ? 0u : (g.C(50) ? 0xFFFFFFFFu : Garbage(n, 77));
    s.rg = g.C(30) ? 0u : (g.C(30) ? 0xFFFFFFFFu : g.R());
    return s;
}

// ------------------------------------------------------------------------------------------------ 2 CDecisionMaker::MakeDecision
struct SpecDecision {
    u32 fam{}, wmode{}, stack{}, eax{}, ecx{}, edx{};
    i32 idx{}, src{}, a1{}, a2{}, a3{}, seek{};
    u32 veh{}, t0{}, f0{};
    i32 wt{}, wf{};
};
inline SpecDecision MakeDecision(u32 n) {
    SpecDecision s;
    Gen g(n + kDecisionMaker * 0x20000u);
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    s.fam = n % kFamilies;
    auto task = [&]() -> i32 {
        const u32 p = g.P(6);
        return p == 0 ? -1 : p == 1 ? 200 : p == 2 ? i32(g.P(1000)) : p == 3 ? i32(g.R()) : p == 4 ? 0 : i32(g.P(300));
    };
    switch (s.fam) {
    case 0: s.idx = i32(g.P(41)); break;                         // 0..40
    case 1: s.idx = 0; break;
    case 2: s.idx = 40; break;
    case 3: s.idx = -1 - i32(g.P(100)); break;                   // negativos
    case 4: s.idx = i32(0x7FFFFFFF - g.P(3)); break;
    case 5: s.idx = i32(g.R()); break;                           // cualquier valor de 32 bits
    case 6: s.idx = i32(0x04444444u + g.P(0x100)); break;        // idx * 0x3C da la vuelta
    case 7: s.idx = i32(0x80000000u + g.P(3)); break;
    default: s.idx = i32(g.P(41)); break;
    }
    s.src = g.C(60) ? i32(g.P(12)) : (g.C(50) ? -i32(g.P(5)) : i32(g.R()));
    static const u32 veh[]{0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0x80u, 0xFFFFFFFFu, 0xFFFFFF00u, 0xDEADBE00u, 0xDEADBE01u};
    s.veh = g.C(70) ? veh[g.P(sizeof(veh) / sizeof(veh[0]))] : g.R();
    s.a1 = task(); s.a2 = task(); s.a3 = task(); s.seek = task();
    s.t0 = Garbage(n, 10); s.f0 = Garbage(n, 11);
    s.wmode = g.P(4);
    s.wt = g.C(40) ? 200 : (g.C(30) ? -1 : i32(g.P(65536)) - 32768);
    s.wf = g.C(40) ? -1 : (g.C(30) ? 200 : i32(g.P(65536)) - 32768);
    return s;
}

// ------------------------------------------------------------------------------------------------ 3 CDecisionMakerTypes::ReadEventIndices
struct Line {
    std::string text;     // contenido (sin NUL); "" = primer caracter 0
    bool write{true};     // el falso de sscanf escribe el indice
    i32 idx{};
    u32 ret{};            // retorno del falso de sscanf
};
struct SpecRead {
    u32 fam{}, tok{}, stack{}, eax{}, ecx{}, edx{};
    std::string name;
    std::vector<Line> lines;
};
inline SpecRead MakeRead(u32 n) {
    SpecRead s;
    Gen g(n + kReadEvents * 0x20000u);
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    s.fam = n % kFamilies;
    s.tok = g.C(15) ? 0u : (0x5000u + g.P(0x1000));
    // nombre del archivo
    switch (g.P(5)) {
    case 0: s.name = "PedEvent.txt"; break;
    case 1: s.name = ""; break;
    case 2: s.name = "x"; break;
    default: {
        const u32 len = 1 + g.P(40);
        for (u32 i = 0; i < len; ++i) s.name += char("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_. "[g.P(65)]);
        break;
    }
    }
    // lineas
    u32 count = 0;
    switch (s.fam) {
    case 0: count = 0; break;
    case 1: count = 1; break;
    case 2: count = 1 + g.P(4); break;
    case 3: count = 8 + g.P(7); break;
    default: count = g.P(15); break;
    }
    auto idxFor = [&]() -> i32 {
        switch (s.fam) {
        case 4: return -i32(g.P(65));                                     // -64..0
        case 5: return 96 + i32(g.P(255));                                // 96..350
        case 6: return i32(g.P(96));
        case 7: return g.C(50) ? i32(g.P(96)) : (g.C(50) ? -i32(g.P(65)) : 96 + i32(g.P(255)));
        default: return g.C(90) ? i32(g.P(96)) : (g.C(50) ? -i32(g.P(65)) : 96 + i32(g.P(255)));
        }
    };
    bool seenParsed = false;
    for (u32 i = 0; i < count; ++i) {
        Line l;
        const u32 kind = g.P(10);
        l.ret = g.C(50) ? 2u : (g.C(50) ? 1u : (g.C(50) ? 0u : 0xFFFFFFFFu));
        l.idx = idxFor();
        l.write = g.C(88);
        if (kind <= 4 || (s.fam == 8 && kind <= 6)) {                     // linea normal "EVENTO n\n"
            l.text = "EVENT_" + std::to_string(g.P(100)) + " " + std::to_string(l.idx) + "\n";
        } else if (kind == 5) {
            l.text = "\n";                                                  // linea vacia: se salta (0x0A)
            l.write = false;
        } else if (kind == 6) {
            l.text = "";                                                    // primer caracter 0: se salta
            l.write = false;
        } else if (kind == 7) {
            static const char* junk[]{" 5\n", "#comment\n", "\r\n", "7 x\n", "\t9\n", "A\n", "-1\n"};
            l.text = junk[g.P(7)];
        } else if (kind == 8) {
            l.text = "LAST_EVENT " + std::to_string(l.idx);                 // sin salto de linea final
        } else {
            l.text = "L_" + std::string(g.P(250), 'q') + "\n";              // linea larga (<= 253 caracteres)
            if (l.text.size() > 254) l.text.resize(254);
        }
        const bool parsed = !l.text.empty() && l.text[0] != '\n';
        if (parsed && !seenParsed) { l.write = true; seenParsed = true; }   // el indice previo seria basura de pila del EXE: la primera linea analizada siempre lo escribe
        s.lines.push_back(l);
    }
    return s;
}

// ------------------------------------------------------------------------------------------------ 4 UserTracksLegacyVector::PushBack
struct SpecPush {
    u32 fam{}, alloc{}, first{}, last{}, end{}, val{}, fk{}, fv{}, stack{}, eax{}, ecx{}, edx{};
};
inline SpecPush MakePush(u32 n) {
    SpecPush s;
    Gen g(n + kPushBack * 0x20000u);
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    s.fam = n % kFamilies;
    s.alloc = g.R();
    s.val = g.C(10) ? 0u : g.R();
    s.fk = 0; s.fv = g.R();
    auto base = [&]() -> u32 { return 0x00100000u + (g.R() & 0x7FFFFFFCu); };   // puntero "razonable", alineado, que nunca cae dentro de la arena de la DLL a la vez que 0x400000-0x1000000
    auto aligned = [&](bool a) -> u32 { return a ? 0u : g.P(4); };
    switch (s.fam) {
    case 0: s.first = 0; s.last = g.R(); s.end = g.R(); break;                                  // sin almacenamiento
    case 1: s.first = base(); s.last = s.first; s.end = s.first + 4 * (1 + g.P(100)); break;      // vacio con sitio
    case 2: { const u32 sz = g.P(50), cap = sz + 1 + g.P(50); s.first = base(); s.last = s.first + 4 * sz; s.end = s.first + 4 * cap; break; }   // con sitio
    case 3: { const u32 sz = g.P(100); s.first = base(); s.last = s.first + 4 * sz; s.end = s.last; break; }                                 // lleno
    case 4: { const u32 cap = g.P(50); s.first = base(); s.end = s.first + 4 * cap; s.last = s.end + 4 * (1 + g.P(20)); break; }             // last pasa de end
    case 5: { const u32 d = 4 * (1 + g.P(60)); s.first = base() + 0x100u; s.last = s.first - d; s.end = g.C(50) ? s.first + 4 * g.P(30) : s.first - d / 2; break; }   // last < first
    case 6: s.first = 0xFFFFFF00u + 4 * g.P(60); s.last = g.P(0x400) & ~3u; s.end = g.C(50) ? (g.P(0x800) & ~3u) : s.first + 4 * g.P(30); break;   // da la vuelta
    case 7: s.first = g.R() | 1u; s.last = g.R(); s.end = g.R(); break;                         // todo aleatorio
    case 8: { const u32 sz = g.P(40); s.first = base(); s.last = s.first + 4 * sz; s.end = s.first + 4 * (sz + 1 + g.P(40)); s.fk = 1 + g.P(3); break; }   // con sitio + reentrada
    case 9: { const u32 sz = g.P(40); s.first = base(); s.last = s.first + 4 * sz; s.end = s.last; s.fk = 1 + g.P(3); break; }   // lleno + reentrada en insert
    case 10: s.first = base() + aligned(false); s.last = s.first + g.P(400); s.end = s.first + g.P(400); break;   // diferencias no multiplos de 4
    default: s.first = g.C(30) ? 1u + g.P(8) : base(); s.last = s.first + 4 * g.P(5); s.end = s.first + 4 * g.P(8); break;
    }
    return s;
}

// ------------------------------------------------------------------------------------------------ JSON de los casos
inline std::string U(u32 v) { return std::to_string(v); }
inline std::string I(i32 v) { return std::to_string(v); }
inline std::string Q(const std::string& s) {   // cadena JSON: solo ASCII imprimible mas \n \r \t
    std::string o = "\"";
    for (unsigned char c : s) {
        if (c == '"' || c == '\\') { o += '\\'; o += char(c); }
        else if (c == '\n') o += "\\n";
        else if (c == '\r') o += "\\r";
        else if (c == '\t') o += "\\t";
        else o += char(c);
    }
    return o + "\"";
}
inline std::string Regs(u32 stack, u32 eax, u32 ecx, u32 edx) { return "\"regs\":[" + U(stack) + "," + U(eax) + "," + U(ecx) + "," + U(edx) + "]"; }
inline std::string JsonFloor(const SpecFloor& s) {
    return "{\"fn\":0,\"fam\":" + U(s.fam) + ",\"x\":" + U(s.x) + ",\"mode\":" + U(s.mode) + ",\"seed\":" + U(s.seed) + "," + Regs(s.stack, s.eax, s.ecx, s.edx) + "}";
}
inline std::string JsonCallback(const SpecCallback& s) {
    std::string p = "[", h = "[";
    for (u32 i = 0; i < kNodes; ++i) { p += (i ? "," : "") + I(s.parent[i]); h += (i ? "," : "") + U(s.hier[i]); }
    return "{\"fn\":1,\"fam\":" + U(s.fam) + ",\"n\":" + U(s.n) + ",\"root\":" + U(s.root) + ",\"found0\":" + U(s.found0) + ",\"rg\":" + U(s.rg) + ",\"parent\":" + p + "],\"hier\":" + h + "]," +
           Regs(s.stack, s.eax, s.ecx, s.edx) + "}";
}
inline std::string JsonDecision(const SpecDecision& s) {
    return "{\"fn\":2,\"fam\":" + U(s.fam) + ",\"idx\":" + I(s.idx) + ",\"src\":" + I(s.src) + ",\"veh\":" + U(s.veh) + ",\"a\":[" + I(s.a1) + "," + I(s.a2) + "," + I(s.a3) + "," + I(s.seek) +
           "],\"t0\":" + U(s.t0) + ",\"f0\":" + U(s.f0) + ",\"wmode\":" + U(s.wmode) + ",\"w\":[" + I(s.wt) + "," + I(s.wf) + "]," + Regs(s.stack, s.eax, s.ecx, s.edx) + "}";
}
inline std::string JsonRead(const SpecRead& s) {
    std::string l = "[";
    for (std::size_t i = 0; i < s.lines.size(); ++i)
        l += (i ? "," : "") + std::string("{\"t\":") + Q(s.lines[i].text) + ",\"w\":" + (s.lines[i].write ? "1" : "0") + ",\"i\":" + I(s.lines[i].idx) + ",\"r\":" + U(s.lines[i].ret) + "}";
    return "{\"fn\":3,\"fam\":" + U(s.fam) + ",\"name\":" + Q(s.name) + ",\"tok\":" + U(s.tok) + ",\"lines\":" + l + "]," + Regs(s.stack, s.eax, s.ecx, s.edx) + "}";
}
inline std::string JsonPush(const SpecPush& s) {
    return "{\"fn\":4,\"fam\":" + U(s.fam) + ",\"alloc\":" + U(s.alloc) + ",\"first\":" + U(s.first) + ",\"last\":" + U(s.last) + ",\"end\":" + U(s.end) + ",\"val\":" + U(s.val) + ",\"fk\":" +
           U(s.fk) + ",\"fv\":" + U(s.fv) + "," + Regs(s.stack, s.eax, s.ecx, s.edx) + "}";
}
inline std::string JsonCase(u32 fn, u32 n) {
    switch (fn) {
    case kFloor: return JsonFloor(MakeFloor(n));
    case kCallback: return JsonCallback(MakeCallback(n));
    case kDecisionMaker: return JsonDecision(MakeDecision(n));
    case kReadEvents: return JsonRead(MakeRead(n));
    default: return JsonPush(MakePush(n));
    }
}

// Imagen previa de la arena: basura salvo los campos del caso.
inline void PutName(std::array<u32, kArenaWords>& img, const std::string& name) {
    u8 bytes[kNameBytes]{};
    std::memcpy(bytes, name.data(), name.size());
    const u32 words = u32(name.size() + 1 + 3) / 4;
    for (u32 w = 0; w < words; ++w) std::memcpy(&img[kNameZone / 4 + w], bytes + 4 * w, 4);
}
inline void BuildImage(u32 fn, u32 n, std::array<u32, kArenaWords>& img) {
    for (u32 w = 0; w < kArenaWords; ++w) img[w] = Garbage(ImageSeed(fn, n), w);
    switch (fn) {
    case kCallback: { const auto s = MakeCallback(n); img[kFound / 4] = s.found0; break; }
    case kDecisionMaker: { const auto s = MakeDecision(n); img[kTaskSlot / 4] = s.t0; img[kFacialSlot / 4] = s.f0; break; }
    case kReadEvents: { const auto s = MakeRead(n); PutName(img, s.name); break; }
    case kPushBack: {
        const auto s = MakePush(n);
        img[kVector / 4] = s.alloc; img[kVector / 4 + 1] = s.first; img[kVector / 4 + 2] = s.last; img[kVector / 4 + 3] = s.end; img[kValueSlot / 4] = s.val;
        break;
    }
    default: break;
    }
}
}  // namespace H65Fix
