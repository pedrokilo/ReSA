#pragma once
// Fixtures del banco FxInfoLoad28: FxInfoManager_c::Load (0x5C0B70, thiscall, 2 argumentos en pila (FILESTREAM file, int32 version), `ret 8`).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y fixture_dump.cpp (herramienta de comprobacion SIN el juego). El oraculo (fixture_oracle.py)
// trabaja con la fila `input` que registra el banco (texto del flujo, version, objeto previo, plan de los falsos) y RE-ANALIZA el texto con su propio modelo de fgets/sscanf,
// asi que no depende del generador: este archivo solo decide QUE flujos se prueban.
//
// 4608 casos = 32 clases x 144. Clase = n % 32 (ver Make). El flujo es un texto continuo que el falso de ReadLine sirve con la semantica de fgets(buf, 256, f): hasta
// 255 caracteres o hasta '\n' inclusive (las lineas de >255 caracteres se parten y el resto se lee en la lectura siguiente).
//
// REGLAS DE SEGURIDAD (el banco NO puede probar comportamiento indefinido que difiera por pila):
//  * La primera palabra de toda linea que el manager analice como NOMBRE (ReadLine + sscanf("%s")) existe y mide <= 100 caracteres (el buffer `field` del EXE mide 128 bytes y
//    un sscanf("%s") sobre una linea en blanco dejaria `field` con basura de pila). Las lineas de NUM_INFOS y de TIMEMODEPRT SI pueden estar en blanco o ser ilegibles: en ese caso
//    sscanf no escribe y se conservan m_nNumInfos / el valor por defecto 1 (comportamiento definido).
//  * El numero de infos leido se mantiene <= 320 (la arena de punteros tiene 320 ranuras) y los enteros de NUM_INFOS / TIMEMODEPRT caben en int32 o son tokens de los que sscanf
//    solo lee un prefijo decimal (el sscanf es el MISMO del EXE en ambos lados; el oraculo marca como desconocido el byte si el valor desborda int32).
//  * El flujo lleva al final lineas de relleno ("FILLER n") para que ninguna lectura llegue a EOF (fgets devolveria NULL dejando el buffer sin tocar = basura de pila).
//  * Los trozos sobrantes de las lineas largas miden < 100 caracteres (los lee como nombre la lectura siguiente).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace FxInfoLoad28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kKinds = 32;
constexpr unsigned kPerKind = 144;
constexpr unsigned kCases = kKinds * kPerKind;     // 4608

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kMgrSize = 0x14;                     // sizeof(FxInfoManager_c)
constexpr u32 kGuard = 0x40;
constexpr u32 kMgrArena = kGuard + 0x20 + kGuard;  // objeto en +kGuard (0x20 bytes reservados; los 0x14 primeros son el objeto)
constexpr u32 kMaxSlots = 320;                     // ranuras de punteros de la arena de m_pInfos
constexpr u32 kPtrArena = kGuard + kMaxSlots * 4 + kGuard;
constexpr u32 kInfoSize = 0x18;                    // tamano de cada FxInfo en la arena FX
constexpr u32 kPoolInfos = 330;                    // objetos FxInfo falsos (kMaxSlots + holgura)
constexpr u32 kStreamArena = 0x10000;              // texto del flujo + NUL + ceros
constexpr u32 kStreamMax = kStreamArena - 0x40;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 777u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

struct TypeName { u16 type; const char* name; };
// Tabla del EXE (0x86AFC4..0x86B2A8), en el orden de las comparaciones de 0x5C0C00..0x5C15D8.
inline constexpr TypeName kTypes[32] = {
    {0x1001, "FX_INFO_EMRATE_DATA:"}, {0x1004, "FX_INFO_EMSIZE_DATA:"}, {0x1008, "FX_INFO_EMSPEED_DATA:"}, {0x1010, "FX_INFO_EMDIR_DATA:"},
    {0x1020, "FX_INFO_EMANGLE_DATA:"}, {0x1040, "FX_INFO_EMLIFE_DATA:"}, {0x1080, "FX_INFO_EMPOS_DATA:"}, {0x1100, "FX_INFO_EMWEATHER_DATA:"},
    {0x1200, "FX_INFO_EMROTATION_DATA:"},
    {0x2001, "FX_INFO_NOISE_DATA:"}, {0x2002, "FX_INFO_FORCE_DATA:"}, {0x2004, "FX_INFO_FRICTION_DATA:"}, {0x2008, "FX_INFO_ATTRACTPT_DATA:"},
    {0x2010, "FX_INFO_ATTRACTLINE_DATA:"}, {0x2020, "FX_INFO_GROUNDCOLLIDE_DATA:"}, {0x2040, "FX_INFO_WIND_DATA:"}, {0x2080, "FX_INFO_JITTER_DATA:"},
    {0x2100, "FX_INFO_ROTSPEED_DATA:"}, {0x2200, "FX_INFO_FLOAT_DATA:"}, {0x2400, "FX_INFO_UNDERWATER_DATA:"},
    {0x4001, "FX_INFO_COLOUR_DATA:"}, {0x4002, "FX_INFO_SIZE_DATA:"}, {0x4004, "FX_INFO_SPRITERECT_DATA:"}, {0x4008, "FX_INFO_HEATHAZE_DATA:"},
    {0x4010, "FX_INFO_TRAIL_DATA:"}, {0x4020, "FX_INFO_FLAT_DATA:"}, {0x4040, "FX_INFO_DIR_DATA:"}, {0x4080, "FX_INFO_ANIMTEX_DATA:"},
    {0x4100, "FX_INFO_COLOURRANGE_DATA:"}, {0x4200, "FX_INFO_SELFLIT_DATA:"}, {0x4400, "FX_INFO_COLOURBRIGHT_DATA:"},
    {0x8001, "FX_INFO_SMOKE_DATA:"},
};
constexpr bool ReadsTimeMode(u16 type) { return (type & 0xF000) > 0x1000; }

struct Plan { u8 body{}; u16 retype{}; };            // por ORDINAL de creacion (AddFxInfo): lineas que consume el Load falso y tipo con el que lo deja (0 = no lo toca)

struct Fx {
    unsigned n{}, kind{}, sub{};
    std::string stream;
    i32 version{};
    std::array<u8, kMgrSize> pre{};
    u16 junkType{};                  // tipo del FxInfo "basura" al que apuntan las ranuras de m_pInfos sin inicializar (nombres desconocidos)
    u8 junkB6{};
    std::vector<Plan> plan;
    u32 eax{}, edx{}, stack{};
    unsigned count{};                // numero de entradas generadas (informativo)
};

struct Rng {
    u32 c, k{};
    explicit Rng(u32 seed) : c(seed) {}
    u32 next() { return Rand(c, k++); }
    u32 below(u32 m) { return m ? next() % m : 0; }
    bool chance(u32 num, u32 den) { return below(den) < num; }
    template <class T, std::size_t N> const T& pick(const T (&a)[N]) { return a[below(u32(N))]; }
};

struct Entry {
    int typeIdx{};                   // -1 = nombre desconocido
    std::string nameLine;            // linea del nombre (con '\n')
    std::string timeText;            // linea de TIMEMODEPRT (con '\n')
    unsigned body{};
    u16 retype{};
    std::string closing{"-\n"};
    std::vector<std::string> bodyLines;
};

inline std::string Fmt(const char* f, long long a) { char b[256]; std::snprintf(b, sizeof b, f, a); return b; }
inline std::string LongLine(const std::string& base, unsigned total) {      // rellena con 'x' (sin espacios) hasta `total` caracteres + '\n'
    // Relleno con palabras cortas (una palabra de >127 caracteres desbordaria `field` si una lectura desplazada la analizara como nombre); acaba en 'x' para que el resto no sea una linea en blanco.
    std::string s = base;
    while (s.size() + 1 < total) s += (s.size() % 7 == 6) ? ' ' : 'x';
    s += 'x';
    return s + "\n";
}

inline constexpr i32 kVersions[] = {1, 2, 109, 100, 7, 0x7FFFFFFF};
inline constexpr i32 kVersionEdges[] = {0, 1, -1, 2, 0x7FFFFFFF, (i32)0x80000000u, 100, 109, 0x3FFFFFFF, -100};
inline constexpr const char* kTimeTexts[] = {
    "TIMEMODEPRT: 1\n", "TIMEMODEPRT: 0\n", "TIMEMODEPRT: 2\n", "TIMEMODEPRT: -1\n", "TIMEMODEPRT: 255\n", "TIMEMODEPRT: 256\n", "TIMEMODEPRT: 257\n", "TIMEMODEPRT: 65536\n",
    "TIMEMODEPRT: -256\n", "TIMEMODEPRT: 128\n", "TIMEMODEPRT: 2147483647\n", "TIMEMODEPRT: -2147483648\n", "TIMEMODEPRT: +3\n", "TIMEMODEPRT: 007\n", "TIMEMODEPRT: 0x10\n",
    "TIMEMODEPRT: abc\n", "TIMEMODEPRT:\n", "TIMEMODEPRT:5\n", "\n", "   \n", "TIMEMODEPRT:   9   \n", "TIMEMODEPRT:\t42\n", "FOO: 3\n", "TIMEMODEPRT: 3.9\n",
    "TIMEMODEPRT: 1e3\n", "TIMEMODEPRT: -0\n", "TIMEMODEPRT: 12abc\n", "TIMEMODEPRT: 4294967296\n", "TIMEMODEPRT: 300\n", "TIMEMODEPRT: 1\r\n", "TIMEMODEPRT: - 4\n", "x y z\n",
    "TIMEMODEPRT: 99999999999\n", "TIMEMODEPRT: 3000000000\n", "TIMEMODEPRT: -3\n", "TIMEMODEPRT: 1 2 3\n",
};
inline constexpr unsigned kNumTimeTexts = sizeof(kTimeTexts) / sizeof(kTimeTexts[0]);
inline constexpr u16 kRetypes[] = {0x1000, 0x2000, 0x4000, 0x8000, 0x3000, 0xC000, 0xFFFF, 0x0001, 0x1001, 0x2001, 0x4001, 0x8001, 0x5000, 0xA000, 0x6000, 0xF000, 0x0000, 0x1234};
inline constexpr u16 kJunkTypes[] = {0x0000, 0x1001, 0x2001, 0x4001, 0x8001, 0x3000, 0xC000, 0xFFFF, 0x2400, 0x4400, 0x1200, 0x0F00};
inline constexpr const char* kUnknownNames[] = {
    "FX_INFO_FOO_DATA:\n", "FX_INFO_EMRATE_DATA\n", "FX_INFO_EMRATE_DATA::\n", "fx_info_emrate_data:\n", "FX_INFO_ROTATE_DATA:\n", "FX_INFO_TEXCOORDS_DATA:\n", "FX_INFO_ROTATEOFFSET_DATA:\n",
    "X\n", "FX_INFO_\n", "FX_INFO_SMOKE_DATA:x\n", "FX_INFO_EMRATE_DATA:\x01\n", "NUM_INFOS:\n", "9\n", "FX_INFO_COLOUR_DATA: \n", "FILLER\n", "-\n", ":\n", "FX_INFO_NOISE_DATA\n",
    "FX_INFO_EMSIZE_DATA;\n", "_FX_INFO_FLAT_DATA:\n"};
inline constexpr unsigned kNumUnknown = sizeof(kUnknownNames) / sizeof(kUnknownNames[0]);

// Variantes de una linea de nombre conocido (todas las que siguen reconociendo el nombre).
inline std::string NameLine(Rng& r, int idx) {
    const std::string nm = kTypes[idx].name;
    switch (r.below(10)) {
    case 0: return nm + "\n";
    case 1: return nm + "\r\n";
    case 2: return "  " + nm + "\n";
    case 3: return "\t" + nm + " extra tokens 1 2 3\n";
    case 4: return nm + " # comment\n";
    case 5: return nm + "  \n";
    default: return nm + "\n";
    }
}

inline Fx Make(unsigned n) {
    Fx f;
    f.n = n;
    f.kind = n % kKinds;
    f.sub = n / kKinds;
    Rng r(n * 2654435761u + 99u);
    const unsigned kind = f.kind;
    FillWords(f.pre.data(), f.pre.size(), n * 40503u + 5u);
    switch (r.below(6)) {                    // objeto previo: patrones
    case 0: f.pre.fill(0x00); break;
    case 1: f.pre.fill(0xFF); break;
    default: break;
    }
    f.junkType = r.pick(kJunkTypes);
    f.junkB6 = u8(r.next());
    f.eax = r.next(); f.edx = r.next(); f.stack = r.next();
    f.version = r.pick(kVersions);

    std::vector<Entry> entries;
    std::string header = "FX_HEADER_LINE 12\n";
    std::string numLine;                     // vacio = estandar con el contador de `entries`
    bool numSet = false;
    bool alwaysTimeLine = false;             // escribe la linea de TIMEMODEPRT incluso cuando el EXE no la lee (version < 0.7)
    bool omitTimeLine = false;               // la omite incluso cuando el EXE la lee

    auto MakeKnown = [&](int idx) {
        Entry e;
        e.typeIdx = idx;
        e.nameLine = NameLine(r, idx);
        if (e.nameLine.back() != '\n') e.nameLine += "\n";
        e.timeText = kTimeTexts[r.below(kNumTimeTexts)];
        return e;
    };
    auto MakeUnknown = [&]() {
        Entry e;
        e.typeIdx = -1;
        e.nameLine = kUnknownNames[r.below(kNumUnknown)];
        return e;
    };
    auto RandIdx = [&]() { return int(r.below(32)); };
    auto IdxInClass = [&](unsigned cls) {    // 0 emision, 1 movimiento, 2 render
        if (cls == 0) return int(r.below(9));
        if (cls == 1) return 9 + int(r.below(11));
        return 20 + int(r.below(12));
    };

    switch (kind) {
    case 0: case 1: {                        // secuencias bien formadas aleatorias
        const unsigned cnt = r.below(13);
        for (unsigned i = 0; i < cnt; ++i) entries.push_back(MakeKnown(RandIdx()));
        f.version = kind == 0 ? r.pick(kVersions) : r.pick(kVersionEdges);
        break;
    }
    case 2: {                                // emision, movimiento y render en orden
        const unsigned e = r.below(5), m = r.below(5), p = r.below(5);
        for (unsigned i = 0; i < e; ++i) entries.push_back(MakeKnown(IdxInClass(0)));
        for (unsigned i = 0; i < m; ++i) entries.push_back(MakeKnown(IdxInClass(1)));
        for (unsigned i = 0; i < p; ++i) entries.push_back(MakeKnown(IdxInClass(2)));
        f.version = (r.below(2) ? 1 : 109);
        break;
    }
    case 3: {                                // fases desordenadas / ausentes
        const unsigned cnt = 1 + r.below(10);
        const unsigned mask = r.below(7) + 1;     // clases permitidas
        for (unsigned i = 0; i < cnt; ++i) {
            unsigned cls;
            do { cls = r.below(3); } while (!((mask >> cls) & 1));
            entries.push_back(MakeKnown(IdxInClass(cls)));
        }
        f.version = r.pick(kVersions);
        break;
    }
    case 4: {                                // valores de TIMEMODEPRT
        const unsigned cnt = 1 + r.below(4);
        for (unsigned i = 0; i < cnt; ++i) {
            auto e = MakeKnown(9 + int(r.below(23)));
            e.timeText = kTimeTexts[(f.sub * 7 + i * 13 + r.below(3)) % kNumTimeTexts];
            entries.push_back(e);
        }
        f.version = r.below(4) ? r.pick(kVersions) : 1;
        break;
    }
    case 5: {                                // formas de la linea NUM_INFOS
        struct Form { const char* fmt; int mode; };       // mode 0: lee N; 1: no lee (se conserva el valor previo); 2: lee 0 (literal)
        static const Form forms[] = {
            {"NUM_INFOS: %lld\n", 0}, {"NUM_INFOS:%lld\n", 1}, {"X %lld\n", 0}, {"  NUM_INFOS:\t%lld\n", 0}, {"NUM_INFOS: +%lld\n", 0}, {"NUM_INFOS: 00%lld\n", 0},
            {"NUM_INFOS:   %lld   garbage\n", 0}, {"NUM_INFOS: %lld.5\n", 0}, {"NUM_INFOS: 0x3\n", 2}, {"NUM_INFOS: abc%lld\n", 1}, {"NUM_INFOS:\n", 1}, {"\n", 1}, {"   \n", 1},
            {"NUM_INFOS: -\n", 1}, {"NUM_INFOS: %lld\r\n", 0}, {"NUM_INFOS: %lldabc\n", 0}, {"%lld\n", 1}, {"NUM_INFOS: - %lld\n", 1}, {"NUM_INFOS %lld\n", 0},
            {"NUM_INFOS: +\n", 1}, {"NUM_INFOS: %lld,5\n", 0}, {"A B\n", 1},
        };
        const Form& fm = forms[(f.sub + r.below(2)) % (sizeof(forms) / sizeof(forms[0]))];
        const unsigned N = r.below(7);
        numSet = true;
        if (fm.mode == 0) { numLine = Fmt(fm.fmt, N); for (unsigned i = 0; i < N; ++i) entries.push_back(MakeKnown(RandIdx())); }
        else if (fm.mode == 2) { numLine = fm.fmt; }
        else {
            numLine = Fmt(fm.fmt, N);
            const unsigned prev = r.below(7);
            std::memcpy(f.pre.data(), &prev, 4);                       // sscanf no escribe: se conserva el valor previo
            for (unsigned i = 0; i < prev; ++i) entries.push_back(MakeKnown(RandIdx()));
        }
        f.version = r.pick(kVersions);
        break;
    }
    case 6: case 7: {                        // contadores grandes: los offsets son bytes
        static const unsigned sizes[] = {127, 128, 129, 130, 200, 254, 255, 256, 257, 258, 300, 320};
        const unsigned cnt = sizes[(f.sub + r.below(3)) % (sizeof(sizes) / sizeof(sizes[0]))];
        static const unsigned firsts[] = {0, 1, 5, 126, 127, 128, 129, 200, 253, 254, 255, 256, 257, 300};
        unsigned fm = firsts[r.below(14)] % cnt, fr = firsts[r.below(14)] % cnt;
        if (kind == 7) std::swap(fm, fr);
        for (unsigned i = 0; i < cnt; ++i) {
            int idx = IdxInClass(0);
            if (i == fm) idx = IdxInClass(1);
            else if (i == fr) idx = IdxInClass(2);
            else if (i > fm && r.chance(1, 40)) idx = IdxInClass(1 + r.below(2));
            entries.push_back(MakeKnown(idx));
        }
        f.version = r.below(2) ? 1 : 0;
        break;
    }
    case 8: {                                // contador <= 0
        static const long long vals[] = {0, -1, -2, -5, -127, -128, -129, -255, -256, -257, -300, -2147483647LL, 0, -1000};
        const long long v = vals[(f.sub + r.below(2)) % (sizeof(vals) / sizeof(vals[0]))];
        numSet = true;
        numLine = (r.below(4) == 0 ? Fmt("NUM_INFOS: +%lld\n", v < 0 ? 0 : v) : Fmt("NUM_INFOS: %lld\n", v));
        f.version = r.pick(kVersions);
        break;
    }
    case 9: {                                // nombres desconocidos mezclados
        const unsigned cnt = 1 + r.below(8);
        for (unsigned i = 0; i < cnt; ++i) entries.push_back(r.chance(2, 5) ? MakeKnown(RandIdx()) : MakeUnknown());
        f.version = r.pick(kVersions);
        break;
    }
    case 10: {                               // barrido de los 32 tipos
        entries.push_back(MakeKnown(int(f.sub % 32)));
        static const i32 vs[] = {0, 1, 109, 1};
        f.version = vs[(f.sub / 32 + r.below(2)) % 4];
        entries.back().timeText = kTimeTexts[r.below(3)];
        break;
    }
    case 11: {                               // lineas de cuerpo y cambio de tipo en el Load
        const unsigned cnt = 1 + r.below(8);
        for (unsigned i = 0; i < cnt; ++i) {
            auto e = MakeKnown(RandIdx());
            e.body = r.below(4);
            e.retype = r.chance(2, 3) ? kRetypes[r.below(sizeof(kRetypes) / sizeof(kRetypes[0]))] : 0;
            entries.push_back(e);
        }
        f.version = r.pick(kVersions);
        break;
    }
    case 12: {                               // lineas de mas de 255 caracteres (fgets las parte)
        const unsigned cnt = 1 + r.below(5);
        for (unsigned i = 0; i < cnt; ++i) { auto e = MakeKnown(RandIdx()); e.body = r.below(3); entries.push_back(e); }
        f.version = r.pick(kVersions);
        const unsigned total = 256 + r.below(95);
        const unsigned which = r.below(6);
        const unsigned who = r.below(cnt);
        if (which == 0) header = LongLine("FX_HEADER_LINE", total);
        else if (which == 1) { numSet = true; numLine = LongLine(Fmt("NUM_INFOS: %lld ", (long long)cnt), total); }
        else if (which == 2) entries[who].nameLine = LongLine(entries[who].nameLine.substr(0, entries[who].nameLine.find_first_of("\r\n")) + " ", total);
        else if (which == 3) entries[who].timeText = LongLine("TIMEMODEPRT: 7 ", total);
        else if (which == 4) entries[who].closing = LongLine("closing_line", total);
        else { entries[who].body = 1 + r.below(2); entries[who].bodyLines.assign(entries[who].body, LongLine("body_line", total)); }
        break;
    }
    case 13: {                               // version y lineas de tiempo no coincidentes
        const unsigned cnt = 1 + r.below(5);
        for (unsigned i = 0; i < cnt; ++i) entries.push_back(MakeKnown(9 + int(r.below(23))));
        f.version = r.pick(kVersionEdges);
        alwaysTimeLine = r.below(2) == 0;
        omitTimeLine = !alwaysTimeLine && r.below(3) == 0;
        break;
    }
    case 14: {                               // version en los bordes del 0.7 y tipos con y sin linea de tiempo
        const unsigned cnt = 1 + r.below(6);
        for (unsigned i = 0; i < cnt; ++i) entries.push_back(MakeKnown(RandIdx()));
        f.version = r.pick(kVersionEdges);
        alwaysTimeLine = true;
        break;
    }
    case 15: {                               // estructura tipica de un efecto SA
        const unsigned e = 3 + r.below(6), m = r.below(5), p = 1 + r.below(5);
        for (unsigned i = 0; i < e; ++i) entries.push_back(MakeKnown(IdxInClass(0)));
        for (unsigned i = 0; i < m; ++i) entries.push_back(MakeKnown(IdxInClass(1)));
        for (unsigned i = 0; i < p; ++i) entries.push_back(MakeKnown(IdxInClass(2)));
        for (auto& en : entries) { en.timeText = r.below(2) ? "TIMEMODEPRT: 1\n" : "TIMEMODEPRT: 0\n"; en.body = r.below(3); }
        f.version = r.below(2) ? 109 : 1;
        break;
    }
    default: {                               // mezcla aleatoria
        const unsigned cnt = r.below(25);
        for (unsigned i = 0; i < cnt; ++i) {
            if (r.chance(15, 100)) { entries.push_back(MakeUnknown()); continue; }
            auto e = MakeKnown(RandIdx());
            e.body = r.chance(1, 3) ? r.below(3) : 0;
            e.retype = r.chance(1, 8) ? kRetypes[r.below(sizeof(kRetypes) / sizeof(kRetypes[0]))] : 0;
            if (r.chance(1, 20)) e.closing = LongLine("closing_line", 256 + r.below(80));
            entries.push_back(e);
        }
        f.version = r.chance(1, 4) ? r.pick(kVersionEdges) : r.pick(kVersions);
        if (r.chance(1, 8)) alwaysTimeLine = true;
        if (r.chance(1, 12)) omitTimeLine = true;
        if (r.chance(1, 10)) header = LongLine("FX_HEADER_LINE", 256 + r.below(80));
        break;
    }
    }
    // Cabecera de >255 caracteres: el resto se lee como linea NUM_INFOS (sin numero): sscanf no escribe y se conserva el valor previo -> debe ser el numero de entradas.
    if (header.size() >= 256) {
        const u32 cnt = u32(entries.size());
        std::memcpy(f.pre.data(), &cnt, 4);
    }
    // Sin protocolo exacto (lineas de tiempo no coincidentes o lineas partidas) una linea de tiempo en blanco podria caer en una lectura de NOMBRE (sscanf devolveria EOF y
    // `field` quedaria con basura de pila): se sustituye por una sin valor.
    {
        bool exact = !alwaysTimeLine && !omitTimeLine && header.size() < 256 && numLine.size() < 256;
        for (auto& e : entries) exact &= e.nameLine.size() < 256 && e.timeText.size() < 256 && e.closing.size() < 256;
        for (auto& e : entries) for (auto& b : e.bodyLines) exact &= b.size() < 256;
        if (!exact)
            for (auto& e : entries)
                if (e.timeText.find_first_not_of(" \t\r\n\v\f") == std::string::npos) e.timeText = "TIMEMODEPRT:\n";
    }

    // ---- ensamblado del flujo
    if (!numSet) numLine = Fmt("NUM_INFOS: %lld\n", (long long)entries.size());
    std::string s = header + numLine;
    unsigned ordinal = 0;
    for (auto& e : entries) {
        s += e.nameLine;
        if (e.typeIdx < 0) continue;                       // desconocido: el EXE no lee nada mas de esta entrada
        const bool eVersionReads = double(f.version) >= 0.7;
        const bool exeReads = ReadsTimeMode(kTypes[e.typeIdx].type) && eVersionReads;
        if ((exeReads && !omitTimeLine) || alwaysTimeLine) s += e.timeText;
        Plan p;
        p.body = u8(e.body);
        p.retype = e.retype;
        if (f.plan.size() <= ordinal) f.plan.resize(ordinal + 1);
        f.plan[ordinal] = p;
        ++ordinal;
        if (!e.bodyLines.empty()) { for (auto& b : e.bodyLines) s += b; }
        else for (unsigned b = 0; b < e.body; ++b) s += "body " + std::to_string(b) + " 1.5 7\n";
        s += e.closing;
    }
    f.count = unsigned(entries.size());
    const unsigned fill = 6 + 2 * unsigned(entries.size());
    for (unsigned i = 0; i < fill; ++i) s += "FILLER " + std::to_string(i) + "\n";
    f.stream = s;
    return f;
}
}  // namespace FxInfoLoad28Fix
