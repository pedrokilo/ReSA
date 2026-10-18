#pragma once
// Fixtures del banco FileLoaderLoad2dEffect27: CFileLoader::Load2dEffect (0x5B7670, cdecl, `ret` normal, 1 argumento: const char* line).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco y
// RE-ANALIZA el texto de la linea con su propio modelo de sscanf; este archivo solo decide QUE lineas se prueban.
//
// 6144 casos = 12 grupos x 512. Grupo = n % 12: 0..10 = tipo 2DFX 0..10 (luz, particula, [2: no se lee], atractor, [4: no se lee], cartel, tragaperras, interior, 8, 9, 10);
// 11 = tipos fuera de rango (-1, 11, 12, 255, 256, 257, ..., y numeros que desbordan el int de 32 bits cuyo resultado es "otro" tipo en cualquier CRT razonable).
// Familia = (n / 12) % 16 (32 repeticiones de cada familia por grupo).
//
// REGLAS DE SEGURIDAD (el banco NO puede probar comportamiento indefinido que difiera por pila):
//  * Los cinco primeros campos (id x y z type) se leen SIEMPRE completos: si fallaran, `type` queda sin inicializar en las dos versiones.
//  * Toda variable local que el EXE guarda en la entrada sin inicializar (casi todas: ver README.md) llega siempre leida por sscanf. Excepciones deliberadas y documentadas:
//    el cartel (tipo 5) tiene por defecto -1 / 0x32 / bytes de tamano y dos enteros SIN inicializar (f184 y f180): las lineas truncadas conservan siempre >= esos campos, salvo las
//    familias con `mask` (bytes de la entrada cuyo valor es basura de pila en el EXE y se ponen a cero en la observacion de AMBOS lados; ver README.md).
//  * Nombres: ASCII imprimible sin comillas, limites del EXE (marco de 0x1CC): luz corona <= 90 / sombra <= 100; particula <= 100 (desborda a la entrada siguiente, dentro de la
//    arena); atractor <= 100; tragaperras <= 100; interior <= 32; ninguna cadena con bytes >= 0x80 (isspace con char negativo hace saltar asserts del CRT de depuracion de la DLL).
//  * La linea vive en una arena: linea, NUL, `tail`, NUL, ceros. Las lineas de luz sin comillas leen la cola (`tail`) como el EXE (que no comprueba el fin de la cadena).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace FileLoader2dFxFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kGroups = 12;
constexpr unsigned kPerGroup = 512;
constexpr unsigned kCases = kGroups * kPerGroup;   // 6144

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kEntry = 0x40;                       // tamano de un bloque C2dEffect
constexpr u32 kStoreSlots = 16;                    // entradas de la tienda falsa
constexpr u32 kMaxStoreK = 12;                     // el contador previo va de 0 a 12: k + 2 < kStoreSlots (los desbordes de particula/atractor caen en k+1 / k+2)
constexpr u32 kStoreGuard = 0x40;                  // guarda antes y despues
constexpr u32 kStoreAt = kStoreGuard;              // la "tienda" (u32 contador + entradas) empieza aqui
constexpr u32 kStoreArena = kStoreGuard + 4 + kStoreSlots * kEntry + kStoreGuard;
constexpr u32 kLineArena = 0x800;
constexpr u32 kLineAt = 0x40;
constexpr u32 kBlockGuard = 0x40;
constexpr u32 kBlockAt = kBlockGuard;              // bloque de Malloc(0x40) de la rama 7
constexpr u32 kBlockArena = kBlockGuard + 0x40 + kBlockGuard;
constexpr u32 kNumMiLabels = 16;                   // model infos falsos 0..14; 15 = nullptr
constexpr u32 kModelTable = 20000;                 // NUM_MODEL_INFOS

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

// Colision de claves con el oraculo: FNV-1a de 32 bits (nombres de textura / de TXD en la traza).
inline u32 Fnv32(const char* s) { u32 h = 2166136261u; for (; *s; ++s) { h ^= u8(*s); h *= 16777619u; } return h; }

// Bits de `crt` (informativos: lo que el GENERADOR quiso incluir; el oraculo reclasifica por su cuenta a partir del texto).
enum : u32 { kCrtHalfway = 1, kCrtIntOverflow = 2, kCrtExoticFloat = 4, kCrtExoticInt = 8, kCrtTypeOverflow = 16, kCrtMasked = 32, kCrtFailTail = 64 };

struct Fx {
    unsigned n{};
    int group{}, family{};
    std::string line, tail;
    i32 modelId{};
    u32 miLabel{}, storeK{};
    std::array<u8, kEntry> preEntry{};
    std::array<u8, 2 * kEntry> preOver{};       // entradas k+1 y k+2
    i32 findResult{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u64 mask{};                                 // bit i = el byte i de la entrada es basura de pila en el EXE: se pone a 0 en la observacion
    u32 crt{};
};

// ------------------------------------------------------------------------------------------------ generador
struct Gen {
    u32 c{}, k{};
    explicit Gen(u32 seed) : c(seed) {}
    u32 operator()(u32 m) { return m ? Rand(c, k++) % m : (++k, 0u); }
    u32 Raw() { return Rand(c, k++); }
    bool One(u32 den) { return (*this)(den) == 0; }
};

// ---- tokens enteros
inline constexpr const char* kIntEdge[] = {
    "0", "1", "2", "3", "7", "8", "15", "16", "17", "31", "32", "63", "64", "100", "127", "128", "129", "200", "254", "255", "256", "257", "258", "511", "512", "513", "1000", "4095", "4096",
    "32767", "32768", "32769", "65535", "65536", "65537", "131071", "131072", "16777215", "16777216", "2147483646", "2147483647",
    "-1", "-2", "-3", "-8", "-127", "-128", "-129", "-255", "-256", "-257", "-32767", "-32768", "-32769", "-65535", "-65536", "-65537", "-2147483647", "-2147483648",
    "+0", "+1", "+5", "+255", "+256", "007", "0255", "00000255", "-0", "-007", "0000000000256"};
// Desbordan el int de 32 bits (el resultado depende del CRT: envoltura, saturacion...). Se usan donde un valor distinto sigue estando definido en ambos lados.
inline constexpr const char* kIntOverflow[] = {
    "2147483648", "2147483649", "2147483650", "3000000000", "4294967295", "4294967296", "4294967297", "4294967551", "4294967552", "8589934591", "10000000000", "99999999999",
    "-2147483649", "-2147483650", "-3000000000", "-4294967295", "-4294967296", "-4294967297", "-99999999999", "18446744073709551616", "184467440737095516150"};
// Para el campo `type`: cada uno cae en "tipo desconocido" tanto con envoltura de 32 bits como con saturacion.
inline constexpr const char* kTypeOverflow[] = {"2147483648", "2147483649", "3000000000", "4294967295", "10000000000", "99999999999", "-2147483649", "-3000000000"};
// Tokens que NO son enteros completos: %d lee un prefijo o falla; despues sscanf se detiene (las variables restantes conservan sus valores por defecto).
inline constexpr const char* kExoticInt[] = {"3.7", "0x1F", "1e3", "+", "-", "abc", "--5", "+-5", "5abc", "0x", "08", "9.", "1,2", ".5", "e5", "#7", "%d", "-+3", "3-", "0.0"};

// ---- tokens decimales
inline constexpr const char* kFloatPlain[] = {
    "0", "1", "-1", "2", "0.5", "-0.5", ".5", "-.5", "5.", "-5.", "+2.5", "+.25", "-0", "0.0", "-0.0", "123.456", "-123.456", "1e3", "2.5E-3", "1.5e+2", "12345.678", "0.1", "0.2", "0.3", "1e-5",
    "100000", "-123456.789", "3.14159", "1000000", "16777216", "16777217", "8388607.5", "0.0001", "255.5", "255.99", "256", "65536.25", "3.4e10", "1e-10", "-2.5e-7", "7.0", "12.5", "-99.9",
    "1e30", "-1e30", "1e-30", "123456789", "-123456789", "0.000001", "4096", "-4096.5", "0.75", "-0.125", "1.0", "1e0", "1E1", "00012.5", "0.5e1", "9e-9"};
// Aproximaciones exactas, justo a mitad y justo por encima/debajo de la mitad entre dos floats (la conversion decimal->float distingue redondeo correcto de doble redondeo):
// triplas {mitad exacta, mitad + 1e-n, mitad - 1e-n}. Precalculadas con aritmetica exacta (Fraction/Decimal).
inline constexpr const char* kHalfway[] = {
    "1.000000059604644775390625", "1.00000005960464477539062500000000000000000000000001", "1.00000005960464477539062499999999999999999999999999",
    "3.00000011920928955078125", "3.0000001192092895507812500000000000000000000000001", "3.0000001192092895507812499999999999999999999999999",
    "0.1000000052154064178466796875", "0.100000005215406417846679687500000000000000000000000001", "0.100000005215406417846679687499999999999999999999999999",
    "100.000003814697265625", "100.00000381469726562500000000000000000000000001", "100.00000381469726562499999999999999999999999999",
    "255.50000762939453125", "255.5000076293945312500000000000000000000000001", "255.5000076293945312499999999999999999999999999",
    "12345.60009765625", "12345.6000976562500000000000000000000000001", "12345.6000976562499999999999999999999999999",
    "16777217", "16777217.00000000000000000000000001", "16777216.99999999999999999999999999",
    "10000000512", "10000000512.00000000000000000000000001", "10000000511.99999999999999999999999999",
    "0.30000002682209014892578125", "0.3000000268220901489257812500000000000000000000000001", "0.3000000268220901489257812499999999999999999999999999",
    "65536.00390625", "65536.0039062500000000000000000000000001", "65536.0039062499999999999999999999999999"};
// Valores de tamano del cartel (_ftol2 -> byte bajo): fronteras 0/1, 255/256, enteros grandes, 2^63, negativos.
inline constexpr const char* kSizeEdge[] = {
    "0", "0.5", "0.99999", "0.99999994", "0.999999999", "1", "1.5", "2", "127.5", "128", "254.9", "255", "255.5", "255.99999", "255.999993", "255.9999999999", "256", "256.5", "257", "300", "511.9", "512",
    "1000", "65535", "65536", "65537.5", "1e5", "16777216", "16777217", "2147483648", "4294967296", "4294967297", "4294967808", "1e10", "3e9", "1e15", "9.2e18", "9.3e18", "1e19", "1e20", "1e30", "2.55e2",
    "-0.5", "-0.99999", "-1", "-1.5", "-127.5", "-128", "-255", "-255.5", "-256", "-257", "-1e10", "-3e9", "-1e19", "-1e30", "-0", "+255.9"};
// Reales que pueden analizarse de forma distinta (o fallar) segun el CRT: solo en campos donde un fallo deja un estado definido (cartel, ver familias con `mask`).
inline constexpr const char* kExoticFloat[] = {
    "inf", "-inf", "+inf", "Inf", "INF", "infinity", "Infinity", "nan", "-nan", "NaN", "NAN", "nan(123)", "1e39", "-1e39", "1e400", "-1e400", "1e-50", "1e-400", "0x1p3", "0x10", "0X1.8p1", "0x.8",
    "1#INF", "1.#QNAN", "-1.#IND", "1e", "1e+", "1e-", ".", "-.", "+.e5", "1,5", "1d5", "1f", "--1", "1.5.5", "i", "n", "e"};

constexpr u32 kIdTable[] = {0, 1, 2, 7, 100, 1000, 4095, 10000, 18000, 19998, 19999};
inline constexpr const char* kTail1 = "\"tail_sh\" 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19";
inline constexpr const char* kTail2 = "\"tail_cor\" \"tail_sh\" 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19";

template <std::size_t N> inline std::string Pick(Gen& g, const char* const (&a)[N]) { return a[g(u32(N))]; }

inline std::string Num(long long v) { return std::to_string(v); }

// Entero normal: casi siempre un valor de byte, a veces un borde.
inline std::string IntTok(Gen& g, unsigned mode) {
    switch (mode) {
    case 0: return Num(g(256));
    case 1: return Pick(g, kIntEdge);
    case 2: return Num(static_cast<long long>(g.Raw() % 2000001u) - 1000000);
    case 3: return Num(g(2) ? g(256) : g(70000));
    default: return g(3) == 0 ? Pick(g, kIntEdge) : Num(g(256));
    }
}
inline std::string DecimalTok(Gen& g) {   // "-123.456" aleatorio, sin notacion cientifica
    std::string s;
    if (g(3) == 0) s += '-';
    s += Num(g(g(2) ? 10000u : 100u));
    if (g(4) != 0) { s += '.'; s += Num(g(1000)); }
    return s;
}
// Real normal. mode: 0 tabla plana, 1 decimal aleatorio, 2 entero, 3 mezcla
inline std::string FloatTok(Gen& g, unsigned mode) {
    switch (mode) {
    case 0: return Pick(g, kFloatPlain);
    case 1: return DecimalTok(g);
    case 2: return Num(static_cast<long long>(g.Raw() % 20001u) - 10000);
    default: return g(2) ? Pick(g, kFloatPlain) : DecimalTok(g);
    }
}
inline std::string Name(Gen& g, unsigned minLen, unsigned maxLen, unsigned style = 0) {
    static const char alnum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    static const char punct[] = "!#$%&'()*+,-./:;<=>?@[\\]^`{|}~";
    const unsigned len = minLen + g(maxLen - minLen + 1);
    std::string s;
    for (unsigned i = 0; i < len; ++i) {
        if (style == 1 && g(3) == 0) s += punct[g(u32(sizeof(punct) - 1))];
        else if (style == 2) s += char('0' + g(10));
        else if (style == 3) { static const char fmt[] = "%sdf%d.+-"; s += fmt[g(u32(sizeof(fmt) - 1))]; }
        else s += alnum[g(u32(sizeof(alnum) - 1))];
    }
    if (s.empty() && minLen > 0) s = "a";
    return s;
}
// Nombre dentro de comillas (puede llevar espacios).
inline std::string QName(Gen& g, unsigned minLen, unsigned maxLen, unsigned style = 0) {
    std::string s = Name(g, minLen, maxLen, style == 1 ? 1 : 0);
    if (style == 1) for (auto& ch : s) if (g(5) == 0) ch = ' ';
    return s;
}
inline std::string Sep(Gen& g, bool random) {
    if (!random) return " ";
    switch (g(8)) { case 0: return "  "; case 1: return "\t"; case 2: return " \t "; case 3: return "   "; case 4: return "\t\t"; default: return " "; }
}
inline std::string Join(Gen& g, const std::vector<std::string>& t, bool randomSep) {
    std::string s;
    for (std::size_t i = 0; i < t.size(); ++i) { if (i) s += Sep(g, randomSep); s += t[i]; }
    if (randomSep && g(4) == 0) s += Sep(g, true);    // espacio final
    return s;
}
inline std::string Pad(Gen& g, unsigned count) {     // basura tras la linea: nunca se lee
    std::string s;
    for (unsigned i = 0; i < count; ++i) s += (g(3) == 0 ? " " : Name(g, 1, 6));
    return s;
}

// ------------------------------------------------------------------------------------------------ estado previo
inline void FillPre(Fx& f, Gen& g) {
    switch (g(8)) {
    case 0: f.preEntry.fill(0x00); break;
    case 1: f.preEntry.fill(0xFF); break;
    case 2: for (u32 i = 0; i < kEntry; ++i) f.preEntry[i] = u8(i * 7u + f.n * 13u + 1u); break;   // bytes distintos por posicion y por caso
    case 3: f.preEntry.fill(u8(0x11 + (f.n & 0xEE))); break;
    default: FillWords(f.preEntry.data(), kEntry, f.n * 977u + 31u); break;
    }
    FillWords(f.preOver.data(), 2 * kEntry, f.n * 1237u + 91u);
}

// ------------------------------------------------------------------------------------------------ grupos
// Prefijo comun: id x y z type. `fam` solo decide el sabor del id y de las coordenadas.
inline void Prefix(Gen& g, Fx& f, std::vector<std::string>& t, const std::string& typeTok, unsigned coordMode) {
    std::string id = Num(f.modelId);
    switch (f.family % 4 == 3 ? g(6) : 9u) { case 0: id = "+" + id; break; case 1: id = std::string(g(4) + 1, '0') + id; break; default: break; }
    t.push_back(id);
    for (int i = 0; i < 3; ++i) {
        if (coordMode == 99) { t.push_back(Pick(g, kHalfway)); f.crt |= kCrtHalfway; }
        else t.push_back(FloatTok(g, coordMode));
    }
    t.push_back(typeTok);
}
inline std::string TypeTok(Gen& g, const Fx& f, int type) {
    std::string s = Num(type);
    if (f.family % 5 == 4) switch (g(4)) { case 0: s = "+" + s; break; case 1: s = "0" + s; break; case 2: s = "00" + s; break; default: break; }
    return s;
}

// ---- grupo 0: luz
inline void BuildLight(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 0), fam == 11 ? 99u : (fam == 9 ? 3u : 1u));
    auto byteTok = [&] { return fam == 8 ? Pick(g, kIntEdge) : (fam == 12 ? (g(2) ? Pick(g, kIntOverflow) : IntTok(g, 1)) : IntTok(g, g(5) == 0 ? 1 : 0)); };
    if (fam == 12) f.crt |= kCrtIntOverflow;
    for (int i = 0; i < 4; ++i) t.push_back(byteTok());   // r g b a
    std::string corona = QName(g, 1, 20), shadow = QName(g, 1, 20);
    bool pairA = true, pairB = true;
    f.tail = kTail1;
    switch (fam) {
    case 1: corona.clear(); shadow.clear(); break;
    case 2: corona.clear(); shadow = QName(g, 60, 100); break;
    case 3: corona = QName(g, 60, 90); break;
    case 4: pairB = false; break;
    case 5: pairA = pairB = false; f.tail = kTail2; f.crt |= kCrtFailTail; break;
    case 13: break;
    case 14: corona = QName(g, 1, 40, 1); shadow = QName(g, 1, 40, 1); break;
    case 15: if (g(2)) corona.clear(); if (g(2)) shadow = QName(g, 20, 100, g(2)); break;
    default: break;
    }
    if (!pairB && pairA) f.crt |= kCrtFailTail;
    std::vector<std::string> nums;
    for (int i = 0; i < 4; ++i) nums.push_back(fam == 11 ? Pick(g, kHalfway) : FloatTok(g, fam == 9 ? 3u : 0u));
    if (fam == 11) f.crt |= kCrtHalfway;
    for (int i = 0; i < 9; ++i) {
        std::string v;
        if (i == 4) {   // flags: el bit 4 manda
            switch (fam) {
            case 7: { static const char* const fl[] = {"0", "1", "2", "3", "4", "5", "6", "7", "255", "256", "260", "261", "1028", "65535", "65536", "65540", "-1", "-4", "-5", "-253", "4294967295", "32772", "32768"}; v = fl[g(u32(std::size(fl)))]; break; }
            case 12: v = g(2) ? Pick(g, kIntOverflow) : IntTok(g, 1); break;
            default: v = g(3) == 0 ? Num(4 + 8 * g(30)) : IntTok(g, 0); break;
            }
        } else if (fam == 10 || fam == 8) v = Pick(g, kIntEdge);
        else if (fam == 12) v = g(3) ? IntTok(g, 0) : Pick(g, kIntOverflow);
        else v = IntTok(g, g(6) == 0 ? 1 : 0);
        nums.push_back(v);
    }
    const bool rs = fam == 6 || (fam == 15 && g(2));
    std::string line = Join(g, t, rs);
    line += Sep(g, rs);
    if (pairA) line += "\"" + corona + "\"";
    if (fam == 13 && pairA && pairB) { /* comillas pegadas: "a""b" */ } else if (pairA && pairB) line += Sep(g, rs);
    if (pairB) line += "\"" + shadow + "\"";
    line += (fam == 13 && g(2)) ? "" : Sep(g, rs);
    line += Join(g, nums, rs);
    f.line = line;
}

// ---- grupo 1: particula
inline void BuildParticle(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 1), fam == 9 ? 3u : (fam == 15 ? g(4) : 0u));
    std::string name;
    switch (fam) {
    case 0: name = Name(g, 1, 7); break;
    case 1: name = Name(g, 7, 8); break;
    case 2: name = Name(g, 9, 20); break;
    case 3: name = Name(g, 21, 46); break;
    case 4: name = Name(g, 47, 47); break;
    case 5: name = Name(g, 48, 62); break;
    case 6: name = Name(g, 63, 100); break;
    case 7: name.clear(); break;                                   // sin nombre: la entrada conserva su estado previo desde +0x10
    case 10: name = Name(g, 1, 40, 1); break;
    case 11: { static const char* const nm[] = {"1.5", "0x10", "-7", "1e3", "+5", "007", "3.14", "nan", "inf", "0"}; name = nm[g(10)]; break; }
    case 12: name = Name(g, 2, 20, 3); break;
    case 13: name = Name(g, 1, 1); break;
    case 15: name = g(6) ? Name(g, 1, g(2) ? 30u : 100u, g(3) == 0 ? 1u : 0u) : std::string(); break;
    default: name = Name(g, 1, 24); break;
    }
    if (!name.empty()) t.push_back(name);
    if (fam == 8) { t.push_back(Name(g, 1, 8)); t.push_back(Num(g(1000))); t.push_back("\"q\""); }
    f.line = Join(g, t, fam == 9 || (fam == 15 && g(2)));
    if (fam == 14) f.line = "\t \t" + f.line;
}

// ---- grupos 2 y 4 (tipo que no se lee) y 11 (fuera de rango): despues del type no se lee nada
inline std::string ExtraTail(Gen& g, unsigned style) {
    switch (style) {
    case 0: return "";
    case 1: return " " + Pad(g, 3);
    case 2: return " " + Pad(g, 30);
    case 3: return "\t \t" + Pad(g, 4);
    case 4: return " " + Pad(g, 60) + Pad(g, 60);          // linea larga
    case 5: return " \"quoted tail\" 1 2 3";
    case 6: return " %s %d %f";
    case 7: return " " + std::string(g(40) + 1, 'x');
    default: return " " + Num(g(100)) + " " + FloatTok(g, 0) + " " + Name(g, 1, 5);
    }
}
inline void BuildPlainType(Gen& g, Fx& f, int type, const std::string& typeTokOverride) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    unsigned coord = fam % 6;                      // 0..5
    if (fam == 3) coord = 99;
    Prefix(g, f, t, typeTokOverride.empty() ? TypeTok(g, f, type) : typeTokOverride, coord == 4 ? 3u : (coord == 5 ? 2u : (coord == 99 ? 99u : coord)));
    if (fam == 5) for (int i = 1; i <= 3; ++i) t[std::size_t(i)] = g(2) ? "1e30" : (g(2) ? "-1e30" : "1e-30");
    if (fam == 4) for (int i = 1; i <= 3; ++i) t[std::size_t(i)] = g(2) ? Pick(g, kFloatPlain) : (g(2) ? "1.5e+2" : "2.5E-3");
    f.line = Join(g, t, fam == 7 || fam == 15);
    f.line += ExtraTail(g, fam == 11 ? 8u : (fam == 12 ? 4u : (fam % 4 == 0 ? 0u : unsigned(fam % 9))));
    if (fam == 13) f.line = "\t" + f.line;
}

// ---- grupo 3: atractor  (id x y z 3 v34 f10 f14 f18 f1c f20 f24 f28 f2c f30 v35 v36 name)
inline void BuildAttractor(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 3), fam == 6 ? 99u : 0u);
    t.push_back(fam == 1 ? Pick(g, kIntEdge) : (fam == 7 ? Pick(g, kIntOverflow) : (fam == 12 ? (g(2) ? "+" + Num(g(256)) : "00" + Num(g(256))) : IntTok(g, fam == 8 ? 2u : 0u))));
    for (int i = 0; i < 9; ++i) {
        if (fam == 6) { t.push_back(Pick(g, kHalfway)); f.crt |= kCrtHalfway; }
        else if (fam == 5) t.push_back(Pick(g, kFloatPlain));
        else if (fam == 11) { static const char* const big[] = {"1e30", "-1e30", "1e-30", "123456789", "-123456789"}; t.push_back(big[g(5)]); }
        else t.push_back(FloatTok(g, fam == 15 ? 3u : 1u));
    }
    if (fam == 7) f.crt |= kCrtIntOverflow;
    for (int i = 0; i < 2; ++i) t.push_back(fam == 1 ? Pick(g, kIntEdge) : (fam == 7 ? Pick(g, kIntOverflow) : (fam == 8 ? Num(-long(g(300))) : IntTok(g, fam == 14 ? 4u : 0u))));
    std::string name;
    switch (fam) {
    case 2: name = Name(g, 1, 7); break;
    case 3: name = Name(g, 8, 30); break;
    case 4: name = Name(g, 31, 100); break;
    case 10: name = Name(g, 1, 40, 1); break;
    case 15: name = Name(g, 1, g(2) ? 12u : 100u); break;
    default: name = Name(g, 1, 14); break;
    }
    t.push_back(name);
    if (fam == 13) { t.push_back(Num(g(100))); t.push_back(Name(g, 1, 5)); }
    f.line = Join(g, t, fam == 9 || fam == 15);
}

// ---- grupo 5: cartel de carretera
// tokens tras `type` (33): a5 (dword directo), sx sy sz (float -> _ftol2 -> byte), f30 (float directo), a10 (+0x15) a11 (+0x2E) y 26 enteros a12..a37.
inline void BuildRoadsign(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 5), fam == 14 ? 3u : 0u);
    std::vector<std::string> ct;
    ct.push_back(fam == 5 ? Pick(g, kIntEdge) : IntTok(g, 0));                       // a5
    for (int i = 0; i < 3; ++i) ct.push_back(fam == 3 || fam == 7 || fam == 15 ? (fam == 7 ? Pick(g, kHalfway) : Pick(g, kSizeEdge)) : (fam == 14 ? (g(2) ? "2.55e2" : FloatTok(g, 3u)) : std::to_string(g(300)) + (g(3) ? "" : ".5")));
    ct.push_back(fam == 6 || fam == 14 ? FloatTok(g, 3u) : FloatTok(g, 1u));        // f30
    if (fam == 7) {   // redondeo decimal -> float en los tamanos: tokens a un paso de una frontera entera
        static const char* const nearTok[] = {"255.99999", "255.999993", "255.9999999999", "0.99999999999999999999", "0.9999999999", "1.0000000001", "16777216.5", "16777217.00000000000000000001", "65535.99999999", "65535.9999999999999",
                                           "4294967295.9999999999", "4294967296.0000000001", "-0.9999999999", "-255.9999999999", "-256.0000000001", "127.99999999999", "128.00000000001", "254.99999999999"};
        for (int i = 1; i <= 3; ++i) if (g(2)) ct[std::size_t(i)] = nearTok[g(u32(std::size(nearTok)))];
        f.crt |= kCrtHalfway;
    }
    for (int i = 0; i < 28; ++i) {
        std::string v;
        switch (fam) {
        case 4: v = Pick(g, kIntEdge); break;
        case 8: v = g(3) == 0 ? Pick(g, kIntOverflow) : IntTok(g, 0); f.crt |= kCrtIntOverflow; break;
        case 15: v = g(3) == 0 ? Pick(g, kIntEdge) : IntTok(g, 0); break;
        default: v = IntTok(g, g(5) == 0 ? 1 : 0); break;
        }
        ct.push_back(v);
    }
    // familias con tokens exoticos
    switch (fam) {
    case 9: { const unsigned p = 7 + g(26); ct[p] = Pick(g, kExoticInt); f.crt |= kCrtExoticInt; break; }                    // posicion a12..a37: los restantes son -1 por defecto en ambos lados
    case 10: ct[4] = Pick(g, kExoticFloat); f.mask = (1ull << 0x15) | (1ull << 0x2E); f.crt |= kCrtExoticFloat | kCrtMasked; break;
    case 11: ct[1] = Pick(g, kExoticFloat); f.mask = (1ull << 0x12) | (1ull << 0x13) | (1ull << 0x14) | (1ull << 0x15) | (1ull << 0x2E); f.crt |= kCrtExoticFloat | kCrtMasked; break;
    case 12: { static const char* const bad[] = {"abc", "4294967296", "-", "0x10", "1e3", "3.7", "+", "99999999999", "2147483648"}; ct[0] = bad[g(u32(std::size(bad)))];
               f.mask = (1ull << 0x12) | (1ull << 0x13) | (1ull << 0x14) | (1ull << 0x15) | (1ull << 0x2E); f.crt |= kCrtExoticInt | kCrtMasked; break; }
    default: break;
    }
    // truncado: se conservan siempre a5 sx sy sz f30 a10 a11 (7 tokens); el resto es opcional (por defecto -1, y 0x32 en a13)
    std::size_t keep = ct.size();
    if (fam == 1) keep = 7 + g(27);
    else if (fam == 2) keep = 7 + g(2);
    else if (fam == 15 && g(2)) keep = 7 + g(27);
    if (fam == 9 || fam == 10 || fam == 11 || fam == 12) keep = ct.size();   // la exotica debe quedar dentro de la linea
    ct.resize(keep);
    for (auto& s : ct) t.push_back(s);
    f.line = Join(g, t, fam == 13 || fam == 15);
    if (fam == 13) f.line += "  " + Pad(g, 2);                                 // espacios dobles como el formato del EXE
}

// ---- grupo 6: tragaperras  (id x y z 6 f10 f14 f18 f1c f20 f24 f28 v2c v2e name v2f)
inline void BuildSlot(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 6), fam == 8 ? 99u : 0u);
    for (int i = 0; i < 7; ++i) {
        if (fam == 8) { t.push_back(Pick(g, kHalfway)); f.crt |= kCrtHalfway; }
        else if (fam == 7) t.push_back(Pick(g, kFloatPlain));
        else t.push_back(FloatTok(g, fam == 15 ? 3u : 1u));
    }
    std::string v2c, v2e, v2f;
    switch (fam) {
    case 5: { static const char* const w[] = {"0", "65535", "65536", "65537", "-1", "-32768", "32768", "70000", "255", "256", "131071", "4660", "-65536"}; v2c = w[g(13)]; v2e = IntTok(g, 0); v2f = IntTok(g, 0); break; }
    case 6: v2c = IntTok(g, 0); v2e = Pick(g, kIntEdge); v2f = Pick(g, kIntEdge); break;
    case 9: v2c = Pick(g, kIntOverflow); v2e = Pick(g, kIntOverflow); v2f = Pick(g, kIntOverflow); f.crt |= kCrtIntOverflow; break;
    case 13: v2c = Num(-long(g(70000))); v2e = Num(-long(g(300))); v2f = Num(-long(g(300))); break;
    case 14: v2c = "+" + Num(g(70000)); v2e = "00" + Num(g(256)); v2f = "+" + Num(g(256)); break;
    default: v2c = IntTok(g, g(4) == 0 ? 1 : 3); v2e = IntTok(g, 0); v2f = IntTok(g, 0); break;
    }
    t.push_back(v2c); t.push_back(v2e);
    std::string name;
    switch (fam) {
    case 1: name = Name(g, 1, 7); break;
    case 2: name = Name(g, 8, 8); break;
    case 3: name = Name(g, 9, 30); break;
    case 4: name = Name(g, 31, 100); break;
    case 10: name = Name(g, 1, 30, 1); break;
    case 15: name = Name(g, 1, g(2) ? 10u : 100u); break;
    default: name = Name(g, 1, 12); break;
    }
    t.push_back(name);
    t.push_back(v2f);
    if (fam == 12) { t.push_back(Name(g, 1, 6)); t.push_back(Num(g(50))); }
    f.line = Join(g, t, fam == 11 || fam == 15);
}

// ---- grupo 7: interior  (id x y z 7 f1b8 f1b0 f1b4 f1a8 f1ac f1a4 [n0 [n1 [n2 [n3]]]])
inline void BuildInterior(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 7), 0u);
    for (int i = 0; i < 5; ++i) {
        if (fam == 9) { t.push_back(Pick(g, kHalfway)); f.crt |= kCrtHalfway; }
        else if (fam == 8) t.push_back(Pick(g, kFloatPlain));
        else t.push_back(FloatTok(g, 1u));
    }
    switch (fam) {
    case 7: { static const char* const w[] = {"0", "65535", "65536", "65537", "-1", "-32768", "32768", "70000", "255", "256", "131071", "4660"}; t.push_back(w[g(12)]); break; }
    case 10: t.push_back(Pick(g, kIntOverflow)); f.crt |= kCrtIntOverflow; break;
    default: t.push_back(IntTok(g, g(4) == 0 ? 1 : 3)); break;
    }
    unsigned nn = 4;
    switch (fam) {
    case 1: nn = 0; break;
    case 2: nn = 1; break;
    case 3: nn = 2; break;
    case 4: nn = 3; break;
    case 14: nn = 5; break;
    case 15: nn = g(6); break;
    default: break;
    }
    for (unsigned i = 0; i < nn; ++i) {
        switch (fam) {
        case 5: t.push_back(Name(g, 15, 17)); break;
        case 6: t.push_back(Name(g, 18, 32)); break;
        case 11: t.push_back(Name(g, 1, 20, 1)); break;
        case 13: t.push_back(g(2) ? Name(g, 1, 1) : Name(g, 1, 4, 2)); break;
        case 15: t.push_back(Name(g, 1, g(2) ? 16u : 32u)); break;
        default: t.push_back(Name(g, 1, 14)); break;
        }
    }
    f.line = Join(g, t, fam == 12 || fam == 15);
}

// ---- grupos 8, 9, 10
inline void BuildType8(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 8), 0u);
    std::string v;
    switch (fam) {
    case 0: v = IntTok(g, 0); break;
    case 1: case 2: v = Pick(g, kIntEdge); break;
    case 3: v = Num(static_cast<long long>(g.Raw()) - 2147483648ll); break;
    case 4: v = Pick(g, kIntOverflow); f.crt |= kCrtIntOverflow; break;
    case 5: { static const char* const w[] = {"3.9", "1e3", "0x10", "7abc", "-3.9", "0.5", "255.5", "1,5", "12#", "5."}; v = w[g(10)]; break; }   // %d lee un prefijo (valido) en ambos CRT
    case 6: v = "+" + Num(g(70000)); break;
    case 7: v = "00" + Num(g(70000)); break;
    case 8: v = Num(-long(g(70000))); break;
    default: v = IntTok(g, 4); break;
    }
    t.push_back(v);
    if (fam == 9) { t.push_back(Name(g, 1, 6)); t.push_back(Num(g(10))); }
    f.line = Join(g, t, fam == 10 || fam == 15);
}
inline void BuildType9(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 9), 0u);
    for (int i = 0; i < 2; ++i) {
        if (fam == 4) { t.push_back(Pick(g, kHalfway)); f.crt |= kCrtHalfway; }
        else if (fam == 5) t.push_back(Pick(g, kFloatPlain));
        else t.push_back(FloatTok(g, 1u));
    }
    switch (fam) {
    case 1: case 2: t.push_back(Pick(g, kIntEdge)); break;
    case 3: t.push_back(Num(-long(g(300)))); break;
    case 6: t.push_back(Pick(g, kIntOverflow)); f.crt |= kCrtIntOverflow; break;
    case 7: t.push_back("+" + Num(g(256))); break;
    default: t.push_back(IntTok(g, 0)); break;
    }
    f.line = Join(g, t, fam == 8 || fam == 15);
}
inline void BuildType10(Gen& g, Fx& f) {
    const unsigned fam = unsigned(f.family);
    std::vector<std::string> t;
    Prefix(g, f, t, TypeTok(g, f, 10), 0u);
    for (int i = 0; i < 9; ++i) {
        if (fam == 5) { t.push_back(Pick(g, kHalfway)); f.crt |= kCrtHalfway; }
        else if (fam == 6) t.push_back(Pick(g, kFloatPlain));
        else t.push_back(FloatTok(g, 1u));
    }
    std::string v;
    switch (fam) {
    case 0: v = "0"; break;
    case 1: v = "1"; break;
    case 2: v = Pick(g, kIntEdge); break;
    case 3: { static const char* const w[] = {"256", "65536", "16777216", "-2147483648", "2147483647", "-1", "0.5", "0.9", "1.9", "-0.5", "0x10", "00", "-0", "+0", "000000000001"}; v = w[g(15)]; break; }
    case 4: v = Pick(g, kIntOverflow); f.crt |= kCrtIntOverflow; break;   // 4294967296 vale 0 con envoltura de 32 bits y != 0 con saturacion
    default: v = IntTok(g, 4); break;
    }
    t.push_back(v);
    f.line = Join(g, t, fam == 7 || fam == 15);
}

// ---- grupo 11: tipos fuera de rango. n/12 recorre la lista (44 entradas).
inline const std::vector<std::string>& OutOfRangeTypes() {
    static const std::vector<std::string> v = {"-1", "11", "12", "13", "15", "16", "31", "32", "63", "64", "127", "128", "129", "254", "255", "256", "257", "258", "383", "384", "511", "512", "513", "1000", "1023", "1024",
        "4095", "4096", "65535", "65536", "65537", "65538", "16777216", "16777217", "2147483647", "-2147483648", "-2", "-100", "-255", "-256", "-257", "-65536", "-65535", "+11"};
    return v;
}
inline void BuildOutOfRange(Gen& g, Fx& f) {
    const auto& types = OutOfRangeTypes();
    const unsigned idx = (f.n / kGroups) % u32(types.size() + std::size(kTypeOverflow));
    std::string tok;
    if (idx < types.size()) tok = types[idx];
    else { tok = kTypeOverflow[idx - types.size()]; f.crt |= kCrtTypeOverflow | kCrtIntOverflow; }
    BuildPlainType(g, f, -1, tok);
}

inline Fx Make(unsigned n) {
    Fx f;
    f.n = n;
    f.group = int(n % kGroups);
    f.family = int((n / kGroups) % 16);
    Gen g(Mix(n * 2654435761u + 0x2D2Fu));
    f.modelId = g(3) == 0 ? i32(g(kModelTable)) : i32(kIdTable[g(u32(std::size(kIdTable)))]);
    f.miLabel = g(kNumMiLabels);
    f.storeK = g(kMaxStoreK + 1);
    f.findResult = i32(g(130)) - 10;
    f.eax = g.Raw(); f.ecx = g.Raw(); f.edx = g.Raw();
    f.stack = n % 5 == 0 ? kStackSeeds[(n / 5) % 8] : g.Raw();
    FillPre(f, g);
    f.tail = kTail1;
    switch (f.group) {
    case 0: BuildLight(g, f); break;
    case 1: BuildParticle(g, f); break;
    case 2: BuildPlainType(g, f, 2, std::string()); break;
    case 3: BuildAttractor(g, f); break;
    case 4: BuildPlainType(g, f, 4, std::string()); break;
    case 5: BuildRoadsign(g, f); break;
    case 6: BuildSlot(g, f); break;
    case 7: BuildInterior(g, f); break;
    case 8: BuildType8(g, f); break;
    case 9: BuildType9(g, f); break;
    case 10: BuildType10(g, f); break;
    default: BuildOutOfRange(g, f); break;
    }
    return f;
}
}   // namespace FileLoader2dFxFix
