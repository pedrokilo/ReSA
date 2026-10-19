#pragma once
// Fixtures del banco CBDNMaterialSetup30: CCustomBuildingDNPipeline::CustomPipeMaterialSetup (0x5D7120, cdecl, `ret` normal, 2 argumentos: RpMaterial* material, void* data).
// Solo C++ estandar (sin tipos del juego): las incluyen LiveComparison.hpp/Core.hpp y fixture_dump.cpp (autoprueba cruzada con el oraculo Python).
//
// 5120 casos = 10 grupos x 512 (grupo = n % 10). El oraculo (fixture_oracle.py) trabaja SOLO con la fila `input` (imagenes previas en hexadecimal y etiquetas de puntero) y modela el
// desensamblado del EXE; este archivo solo decide QUE estados se prueban.
//
// Modelo de memoria (todas las arenas con guardas, comparadas enteras):
//   material  : ventana de 0x80 bytes. Palabra +0x10 = banderas. Palabra en +envOff = puntero al dato de entorno (plugin 0x8D12C4). Palabra en +mfxOff = puntero a los datos de
//               MatFX (plugin 0xC9AB74). Los desplazamientos de plugin se LEEN del juego en cada caso (se registran en `env_off` / `mfx_off`).
//   envA, envB: datos de entorno de 16 bytes: byte +4 = brillo, palabra +8 = puntero a textura (se comprueba solo contra 0, salvo el que se escribe).
//   tex0, tex1: texturas falsas de 0x60 bytes (el codigo escribe los bytes +0x50 y +0x51).
//   mfx       : datos de MatFX de 0x40 bytes: palabra +4 = textura del primer pase, palabra +0x30 = banderas de efectos (lo que lee RpMatFXMaterialGetEffects 0x812140).
// Etiquetas de puntero: 0 nulo, 1 envA, 2 envB, 3 tex0, 4 tex1, 5 mfx; cualquier otro valor es un puntero "basura" que NUNCA se desreferencia.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

namespace CBDNMatFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kGroups = 10;
constexpr unsigned kPerGroup = 512;
constexpr unsigned kCases = kGroups * kPerGroup;   // 5120

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kMatGuard = 0x40;
constexpr u32 kMatAt = kMatGuard;
constexpr u32 kMatWin = 0x80;
constexpr u32 kMatArena = kMatGuard + kMatWin + kMatGuard;            // 0x100
constexpr u32 kEnvGuard = 0x20;
constexpr u32 kEnvAt = kEnvGuard;
constexpr u32 kEnvSize = 0x10;
constexpr u32 kEnvArena = kEnvGuard + kEnvSize + kEnvGuard;           // 0x50
constexpr u32 kTexGuard = 0x20;
constexpr u32 kTexAt = kTexGuard;
constexpr u32 kTexSize = 0x60;
constexpr u32 kTexArena = kTexGuard + kTexSize + kTexGuard;           // 0xA0
constexpr u32 kMfxGuard = 0x20;
constexpr u32 kMfxAt = kMfxGuard;
constexpr u32 kMfxSize = 0x40;
constexpr u32 kMfxArena = kMfxGuard + kMfxSize + kMfxGuard;           // 0x80
constexpr u32 kGuardByte = 0xA7;                                      // relleno de las guardas

enum : u32 { kLblNull = 0, kLblEnvA = 1, kLblEnvB = 2, kLblTex0 = 3, kLblTex1 = 4, kLblMfx = 5 };
constexpr u32 kBogusBase = 0xDEAD0000u;                               // punteros basura (nunca desreferenciados)

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};

struct Fx {
    unsigned n{};
    int group{};
    u32 effects{};                 // palabra +0x30 de los datos de MatFX
    u32 mfxPresent{};              // 0: el puntero de MatFX del material es nulo (GetEffects devuelve 0 sin leer nada)
    u32 mfxTex{};                  // etiqueta de la textura del primer pase (0, 3 o 4)
    u32 envPre{};                  // etiqueta del puntero de entorno antes de la llamada (0, 1 o 2)
    u32 mode{};                    // comportamiento del falso de SetFxEnvTexture (ver Core.hpp / oraculo)
    u32 flagsWord{};               // modo 6: palabra que el falso escribe en +0x10
    u32 preFlags{};
    std::array<u8, kMatWin> mat{};         // imagen previa (sin las tres palabras especiales: se rellenan segun los desplazamientos)
    std::array<u8, kEnvSize> envA{}, envB{};   // imagen previa (palabra +8 a cero; su valor va en envATex / envBTex)
    u32 envATex{}, envBTex{};
    std::array<u8, kTexSize> tex0{}, tex1{};
    std::array<u8, kMfxSize> mfx{};        // palabras +4 y +0x30 a cero
    u32 data{};                    // segundo argumento (no se usa)
    u32 eax{}, ecx{}, edx{}, stack{};
};

struct Gen {
    u32 c{}, k{};
    explicit Gen(u32 seed) : c(seed) {}
    u32 operator()(u32 m) { return m ? Rand(c, k++) % m : (++k, 0u); }
    u32 Raw() { return Rand(c, k++); }
    bool One(u32 den) { return (*this)(den) == 0; }
};

inline constexpr u32 kEffectsOther[] = {0, 1, 3, 4, 5, 6, 7, 8, 0x100, 0x102, 0x10002, 0x80000002u, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFDu, 0x20, 0x12, 0x0A, 0x82};
inline constexpr u32 kShin[] = {0, 1, 2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF};
inline constexpr u32 kFlagsPre[] = {0, 1, 7, 8, 0xF, 0xFFFFFFF8u, 0xFFFFFFFFu, 0x80000000u, 0x100, 0x55555555u, 0xAAAAAAAAu};
inline constexpr u32 kFlagsWord[] = {0, 1, 7, 8, 0xFFFFFFFFu, 0xFFFFFFF8u, 0x80000001u, 0x12345679u, 0xCAFEF00Du};

inline u32 TexPtrLabel(Gen& g) {         // contenido de la palabra +8 de un dato de entorno
    switch (g(8)) {
    case 0: case 1: return kLblNull;
    case 2: return kLblTex0;
    case 3: return kLblTex1;
    default: return kBogusBase + (g.Raw() & 0xFFFCu);
    }
}

inline Fx Make(unsigned n) {
    Fx f;
    f.n = n;
    f.group = int(n % kGroups);
    Gen g(n * 2654435761u + 77u);
    // ---- valores por defecto
    f.effects = 2;
    f.mfxPresent = 1;
    f.mfxTex = g(4) == 0 ? kLblNull : (g(2) ? kLblTex0 : kLblTex1);
    f.envPre = g(4) == 0 ? kLblNull : (g(2) ? kLblEnvA : kLblEnvB);
    f.mode = g(7);
    f.flagsWord = kFlagsWord[g(sizeof(kFlagsWord) / sizeof(u32))];
    f.preFlags = g(3) == 0 ? g.Raw() : kFlagsPre[g(sizeof(kFlagsPre) / sizeof(u32))];
    FillWords(f.mat.data(), f.mat.size(), n * 40503u + 1u);
    FillWords(f.envA.data(), f.envA.size(), n * 40503u + 2u);
    FillWords(f.envB.data(), f.envB.size(), n * 40503u + 3u);
    FillWords(f.tex0.data(), f.tex0.size(), n * 40503u + 4u);
    FillWords(f.tex1.data(), f.tex1.size(), n * 40503u + 5u);
    FillWords(f.mfx.data(), f.mfx.size(), n * 40503u + 6u);
    for (u32 i = 8; i < 12; ++i) { f.envA[i] = 0; f.envB[i] = 0; }                            // la palabra +8 va aparte (etiqueta)
    for (u32 i = 4; i < 8; ++i) f.mfx[i] = 0;                                                  // textura del primer pase: aparte
    for (u32 i = 0x30; i < 0x34; ++i) f.mfx[i] = 0;                                            // efectos: aparte
    f.envA[4] = g(3) == 0 ? u8(g.Raw()) : u8(kShin[g(sizeof(kShin) / sizeof(u32))]);
    f.envB[4] = g(3) == 0 ? u8(g.Raw()) : u8(kShin[g(sizeof(kShin) / sizeof(u32))]);
    f.envATex = TexPtrLabel(g);
    f.envBTex = TexPtrLabel(g);
    f.data = g.Raw();
    f.eax = g.Raw(); f.ecx = g.Raw(); f.edx = g.Raw();
    f.stack = kStackSeeds[g(8)];
    // ---- grupos
    switch (f.group) {
    case 0: f.mode = 1; f.mfxTex = kLblTex0 + g(2); break;                                     // pide el dato y le pone la textura
    case 1: f.mode = 0; f.envPre = kLblEnvA + g(2); f.mfxTex = kLblTex0 + g(2); break;           // la ranura ya tiene el dato (el real no duplica)
    case 2: f.mode = g(2) ? 2 : 5; break;                                                        // sin dato / el falso devuelve nulo
    case 3: f.effects = kEffectsOther[g(sizeof(kEffectsOther) / sizeof(u32))]; break;            // casi nunca 2
    case 4: f.mfxTex = kLblNull; break;                                                          // textura nula: no se escriben bytes de textura
    case 5: f.envA[4] = 0; f.envB[4] = 0; f.mode = 1 + g(3); f.mfxTex = kLblTex0 + g(2); break;   // brillo 0: el bit 0 queda a 0
    case 6: f.mfxPresent = g(2); f.effects = g(2) ? 0 : g.Raw(); break;                           // sin MatFX o efectos 0
    case 7: f.mode = 6; break;                                                                   // el falso escribe las banderas: la mascara 0xFFFFFFF8
    case 8: f.effects = g(2) ? kEffectsOther[g(sizeof(kEffectsOther) / sizeof(u32))] : (g.Raw() & ~2u) | (g(2) ? 2u : 0u); break;   // casi 2
    default: break;                                                                              // 9: todo aleatorio (mode ya aleatorio)
    }
    if (f.effects != 2 && f.mfxPresent == 0 && g(2)) f.effects = 0;
    if (f.mfxPresent == 0 && f.effects == 2) f.mfxPresent = 1;                                    // efectos == 2 implica datos de MatFX
    return f;
}

// ---- imagen de la ventana del material para unos desplazamientos de plugin dados (las tres palabras especiales quedan a cero)
inline std::array<u8, kMatWin> MatImage(const Fx& f, u32 envOff, u32 mfxOff) {
    auto m = f.mat;
    const u32 zero = 0;
    std::memcpy(m.data() + 0x10, &zero, 4);
    std::memcpy(m.data() + envOff, &zero, 4);
    std::memcpy(m.data() + mfxOff, &zero, 4);
    return m;
}

inline std::string Hex(const u8* p, std::size_t n) {
    constexpr char d[] = "0123456789abcdef";
    std::string o;
    o.reserve(n * 2);
    for (std::size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; }
    return o;
}

// Fila `input` (JSON). La usa Core.hpp (nlohmann) y fixture_dump.cpp: el mismo texto en ambos sitios.
inline std::string InputJson(const Fx& f, u32 envOff, u32 mfxOff) {
    const auto m = MatImage(f, envOff, mfxOff);
    std::string s = "{";
    char b[96];
    auto num = [&](const char* k, unsigned long long v, bool last = false) { std::snprintf(b, sizeof b, "\"%s\":%llu%s", k, v, last ? "" : ","); s += b; };
    auto hex = [&](const char* k, const u8* p, std::size_t len, bool last = false) { s += "\""; s += k; s += "\":\""; s += Hex(p, len); s += last ? "\"" : "\","; };
    num("group", unsigned(f.group)); num("n", f.n);
    num("env_off", envOff); num("mfx_off", mfxOff);
    num("effects", f.effects); num("mfx_present", f.mfxPresent); num("mfx_tex", f.mfxTex); num("env_pre", f.envPre);
    num("mode", f.mode); num("flags_word", f.flagsWord); num("pre_flags", f.preFlags);
    hex("mat", m.data(), m.size());
    hex("env_a", f.envA.data(), f.envA.size()); num("env_a_tex", f.envATex);
    hex("env_b", f.envB.data(), f.envB.size()); num("env_b_tex", f.envBTex);
    hex("tex0", f.tex0.data(), f.tex0.size()); hex("tex1", f.tex1.data(), f.tex1.size());
    hex("mfx", f.mfx.data(), f.mfx.size());
    num("data", f.data); num("eax", f.eax); num("ecx", f.ecx); num("edx", f.edx); num("stack_seed", f.stack, true);
    s += "}";
    return s;
}
}  // namespace CBDNMatFix
