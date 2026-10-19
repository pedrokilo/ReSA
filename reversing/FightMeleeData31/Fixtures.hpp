#pragma once
// Fixtures del banco FightMeleeData31: CTaskSimpleFight::LoadMeleeData (0x5BEDC0, estatica, cdecl sin argumentos, retorno void). Solo C++ estandar (sin tipos del juego):
// las incluye LiveComparison.hpp y tambien un programa autonomo de calibracion (fixture_mirror.py).
//
// La funcion inicializa 14 filas de la tabla de combos (0xC170D0, 0x88 bytes por fila), los siete vectores de niveles (0xC177D0) y lee DATA\melee.dat con CFileMgr::OpenFile (0x538900),
// CFileLoader::LoadLine (0x536F80) y CFileMgr::CloseFile (0x5389D0); interpreta cada linea segun los bloques START_COMBO / START_LEVELS ... END_COMBO y el contador de lineas del bloque
// (sscanf del EXE, 0x8220AD) y busca el grupo de animacion por nombre con CAnimManager::GetAnimGroupName (0x4D3A20) sobre el contador 0xB4EA28. El banco SUSTITUYE las cuatro
// funciones de ficheros/nombres por falsos identicos en ambos lados: el fichero es una lista de lineas fijada por el caso.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace FightMeleeData31Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4608;   // 16 familias x 288

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ region observada (memoria global del EXE)
// Region = [0xC17090, 0xC17940): guarda delantera 0x40 + [0xC170D0, 0xC17900) (tabla de 12 filas, filas 12 y 13 que pisan los niveles, siete vectores y lo que hay hasta el final de la
// ventana: el CColModel estatico de FightStrike) + guarda trasera 0x40. 0x40 + 0x830 + 0x40 = 0x8B0. Se rellena con un patron pseudoaleatorio antes de cada lado y se restaura al terminar.
constexpr u32 kRegionBase = 0xC17090u;
constexpr u32 kRegionOff = 0x40, kRegionBytes = 0x830, kRegion = 0x8B0;
constexpr u32 kTableAt = 0xC170D0u, kLevelsAt = 0xC177D0u;       // direcciones del EXE
constexpr u32 kGroupCountAddress = 0xB4EA28u;                    // CAnimManager::ms_numAnimAssocDefinitions (uint32)
constexpr u32 kMaxGroups = 6;                                    // nombres de grupo que sabe dar el falso de GetAnimGroupName
constexpr u32 kMaxEvents = 1024;

// Un caso = lista de lineas del fichero + nombres de grupo + contador de grupos
struct C {
    u32 family = 0, numGroups = 0, prefillSeed = 0, eax = 0, ecx = 0, edx = 0, stack = 0;
    std::vector<std::string> groups;
    std::vector<std::string> lines;
};

// ------------------------------------------------------------------------------------------------ generador de lineas
struct Gen {
    u32 n, k = 100;
    u32 R() { return Rand(n, k++); }
    u32 Pick(u32 m) { return R() % m; }
    bool Chance(u32 num, u32 den) { return Pick(den) < num; }
    template <std::size_t N> const char* Choose(const char* const (&tab)[N]) { return tab[Pick(u32(N))]; }

    // Escalares en texto. Solo decimales de <= 9 cifras significativas en rango normal (el sscanf del EXE y el de la DLL redondean igual): el banco usa SIEMPRE el del EXE, pero el oraculo
    // solo modela estos.
    std::string Float() {
        static const char* const fixed[] = {"0", "1", "2", "10", "24", "100", "-1", "-30", "0.5", "1.25", "24.0", "33.5", "-0.75", "0.001", "123.456", "999.99", "5.", ".25", "0.0", "-0.0", "1e2", "2.5e-1", "3.75", "60", "90.5",
                                            "-100.125", "7", "15", "45.5", "0.1", "0.3333333", "12345.678", "99999", "1e-3", "-5e1", "20", "30"};
        const u32 r = Pick(10);
        if (r < 6) return Choose(fixed);
        std::string s;
        if (Chance(1, 5)) s += '-';
        s += std::to_string(Pick(100000));
        if (Chance(3, 4)) { s += '.'; s += std::to_string(Pick(10000)); }
        return s;
    }
    std::string PosFloat() {
        static const char* const fixed[] = {"1", "2", "10", "24", "100", "0.5", "1.25", "24.0", "33.5", "0.001", "123.456", "999.99", "5.", ".25", "3.75", "60", "90.5", "0.1", "0.3333333", "7", "15", "45.5", "1e-3", "1e2"};
        return Choose(fixed);
    }
    std::string Int() {
        static const char* const fixed[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-1", "255", "256", "257", "1000", "-1000", "65535", "65536", "2147483647", "-2147483648", "-2", "100", "12", "40", "-128", "128", "127"};
        const u32 r = Pick(10);
        if (r < 8) return Choose(fixed);
        return (Chance(1, 4) ? std::string("-") : std::string()) + std::to_string(Pick(1000000));
    }
    std::string SoundInt() {            // 1..8 validos; el resto (0, negativos, grandes) dan 0x3E
        static const char* const fixed[] = {"1", "2", "3", "4", "5", "6", "7", "8", "0", "9", "-1", "2147483647", "-2147483648", "10", "1000"};
        return Pick(10) < 7 ? std::to_string(1 + Pick(8)) : std::string(Choose(fixed));
    }
    std::string Hex() {
        static const char* const fixed[] = {"0", "1", "FF", "ffff", "10000", "1A2B", "FFFFFFFF", "deadbeef", "4", "400", "401", "8000", "7FFF", "FFFF0004", "12345678", "0F0F", "a"};
        return Choose(fixed);
    }
    std::string Level() {
        static const char* const fixed[] = {"HIT", "LOW", "GROUND", "BLOCK", "H", "L", "G", "B", "X", "hit", "123", "HL", "LL", "GL", "Zero", "Hx", "Bz", "Gg", "Lq", "-", "zzz", "BLOCKED", "HIGH"};
        return Choose(fixed);
    }
    std::string Word() {
        static const char* const fixed[] = {"NAME", "RANGE", "HIT1", "HIT2", "HIT3", "HIT4", "HIT5", "BLOCK", "FLAGS", "ANIM", "X", "level", "STAGE", "a1", "b_2"};
        return Choose(fixed);
    }
    std::string Ws() { static const char* const fixed[] = {" ", " ", " ", "  ", "\t", " \t "}; return Choose(fixed); }
};

// Constructor de escenarios. Solo emite lineas truncadas o con basura numerica cuando el valor "obsoleto" que usara el EXE (los escalares de sscanf son huecos de pila que persisten
// entre lineas; antes de la primera lectura completa contienen basura) ya lo ha definido una linea completa anterior.
struct Builder {
    Gen g;
    C c;
    bool seenHit = false, seenWord2 = false, seenRaw28 = false, seenX3c = false;
    explicit Builder(u32 n) : g{n} {}

    void Add(const std::string& s) { c.lines.push_back(s); }
    void Noise(u32 num, u32 den) {             // comentarios y lineas vacias (no cuentan: '#' o primer caracter NUL)
        while (g.Chance(num, den)) {
            switch (g.Pick(4)) {
            case 0: Add("# " + g.Word()); break;
            case 1: Add(""); break;
            case 2: Add("#"); break;
            default: Add("#" + g.Word() + " " + g.Float()); break;
            }
        }
    }
    std::string Group() {
        if (c.groups.empty() || g.Chance(1, 7)) {
            static const char* const unknown[] = {"UnknownGrp", "walk_civi_x", "FIGHT", "zz", "a"};
            return g.Choose(unknown);
        }
        return c.groups[g.Pick(u32(c.groups.size()))];
    }
    std::string Level() { return g.Level(); }

    // ---- una linea de datos del combo segun su posicion (0..8); `broken` pide truncada/garabateada si es seguro
    std::string Slot(u32 i, bool broken, bool f1c) {
        switch (i) {
        case 0: {
            if (broken && seenWord2 && g.Chance(1, 2)) return g.Word();               // sin segundo token: word2 obsoleto
            const std::string s = g.Word() + g.Ws() + Group();
            seenWord2 = true;
            return s;
        }
        case 1: {
            if (broken && seenRaw28) { return g.Chance(1, 2) ? g.Word() : g.Word() + " abc"; }
            seenRaw28 = true;
            return g.Word() + g.Ws() + g.Float();
        }
        case 2: case 3: case 4: case 5: case 6: {
            if (broken && seenHit) {
                const u32 kind = g.Pick(3);
                std::string toks[9] = {g.Word(), g.Float(), g.Float(), g.Float(), g.Level(), g.Int(), g.SoundInt(), g.SoundInt(), g.Float()};
                if (kind == 0) {                                                    // cortada tras `cut` tokens
                    const u32 cut = 1 + g.Pick(8);
                    std::string s;
                    for (u32 t = 0; t < cut; ++t) s += (t ? g.Ws() : std::string()) + toks[t];
                    return s;
                }
                if (kind == 1) {                                                    // un token numerico sustituido por basura (la lectura se detiene alli)
                    const u32 bad = 1 + g.Pick(3);
                    toks[bad] = "xyz";
                    std::string s = toks[0];
                    for (u32 t = 1; t < 9; ++t) s += g.Ws() + toks[t];
                    return s;
                }
                const u32 badInt = 5 + g.Pick(3);                                   // un entero con basura
                toks[badInt] = "-";
                std::string s = toks[0];
                for (u32 t = 1; t < 9; ++t) s += g.Ws() + toks[t];
                return s;
            }
            std::string s = g.Word();
            for (int t = 0; t < 3; ++t) s += g.Ws() + g.Float();
            s += g.Ws() + Level();
            s += g.Ws() + g.Int();
            s += g.Ws() + g.SoundInt();
            s += g.Ws() + g.SoundInt();
            if (f1c) s += g.Ws() + g.Float();
            seenHit = true;
            seenWord2 = true;
            return s;
        }
        case 7: {
            if (broken && seenHit && g.Chance(1, 2)) return g.Chance(1, 2) ? g.Word() + " " + g.Float() : g.Word() + " xyz";
            return g.Word() + g.Ws() + g.Float() + g.Ws() + g.Float();
        }
        case 8: {
            if (broken && seenX3c && g.Chance(1, 2)) return g.Word() + " zz";
            seenX3c = true;
            return g.Word() + g.Ws() + g.Hex();
        }
        default: return g.Word() + " " + g.Float();                                 // linea de relleno: contador >= 9, ignorada
        }
    }
    void StartCombo() {
        static const char* const ok[] = {"START_COMBO", "START_COMBO", "START_COMBO", "START_COMBO 1", "START_COMBOS", "START_COMBO_X\t2"};
        Add(g.Choose(ok));
    }
    void EndCombo() {
        static const char* const ok[] = {"END_COMBO", "END_COMBO", "END_COMBO", "END_COMBO extra", "END_COMBOS"};
        Add(g.Choose(ok));
    }
    // count: lineas de datos (9 = completo); noise: densidad de comentarios; brokenPct: probabilidad (en %) de linea defectuosa
    void Combo(u32 count, u32 noiseNum, u32 brokenPct, bool f1cMode, bool close = true) {
        StartCombo();
        for (u32 i = 0; i < count; ++i) {
            Noise(noiseNum, 10);
            Add(Slot(i, g.Pick(100) < brokenPct, f1cMode ? g.Chance(2, 3) : g.Chance(1, 2)));
        }
        Noise(noiseNum, 10);
        if (close) EndCombo();
    }
    void Levels(u32 count, u32 noiseNum) {
        static const char* const st[] = {"START_LEVELS", "START_LEVELS", "START_LEVELS 7", "START_LEVELSX"};
        Add(g.Choose(st));
        for (u32 i = 0; i < count; ++i) {
            Noise(noiseNum, 10);
            switch (g.Pick(6)) {
            case 0: Add(g.Word() + " " + g.Float()); break;                           // solo x: y y z conservan lo anterior (residuo 0.75f, 0 o el vector previo)
            case 1: Add(g.Word() + " " + g.Float() + " " + g.Float()); break;
            case 2: Add(g.Word() + " xyz"); break;                                    // nada se lee
            default: Add(g.Word() + g.Ws() + g.Float() + g.Ws() + g.Float() + g.Ws() + g.Float()); break;
            }
        }
        EndCombo();
    }
    void Junk(u32 count) {                      // fuera de bloques: ignorado (o directivas que casi coinciden)
        static const char* const lines[] = {"MELEE_DATA", "END_LEVELS", "foo bar 12", "start_combo", "Start_Levels", "START_COMB", "START_LEVEL", "END_COMBO", "END_COMBO extra", "END_MELEE_DAT", "end_melee_data",
                                            "0 1 2 3", "NAME group", "STARTCOMBO", "X START_COMBO"};
        for (u32 i = 0; i < count; ++i) Add(g.Choose(lines));
    }
    void EndMeleeData() {
        static const char* const ok[] = {"END_MELEE_DATA", "END_MELEE_DATA", "END_MELEE_DATA x", "END_MELEE_DATAS"};
        Add(g.Choose(ok));
    }
    void Groups(u32 count) {
        static const char* const pool[] = {"walk_civi", "FIGHTA", "FIGHTB", "FIGHTC", "KO_SHOT", "MELEE_X", "a", "b", "FIGHTA", "ped"};
        c.groups.clear();
        for (u32 i = 0; i < count; ++i) c.groups.push_back(g.Choose(pool));
        c.numGroups = count;
    }
};

inline C MakeC(u32 n) {
    Builder b(n);
    C& c = b.c;
    const u32 f = n % 16;
    c.family = f;
    c.prefillSeed = Rand(n, 70);
    c.eax = Rand(n, 20);
    c.ecx = Rand(n, 21);
    c.edx = Rand(n, 22);
    c.stack = StackSeed(n % 13 < 8 ? n % 13 : n);
    b.Groups(1 + Rand(n, 71) % kMaxGroups);
    const u32 r = Rand(n, 72);
    if (r % 11 == 0) c.numGroups = 0;
    else if (r % 11 == 1) c.numGroups = 0xFFFFFFFFu;
    else if (r % 11 == 2) c.numGroups = 0x80000000u;
    else if (r % 11 == 3) c.numGroups = 1;
    else if (r % 11 == 4 && c.groups.size() > 2) c.numGroups = 2;
    const u32 combos = 1 + Rand(n, 73) % 12;
    auto full = [&](u32 count) { for (u32 i = 0; i < count; ++i) { b.Noise(1, 10); b.Combo(9, 0, 0, false); } };
    switch (f) {
    case 0:
        b.Noise(2, 10); full(combos); b.Noise(2, 10);
        if (b.g.Chance(2, 3)) { b.Levels(7, 0); }
        b.EndMeleeData();
        break;
    case 1:
        full(combos);
        if (b.g.Chance(1, 2)) b.Junk(2);
        break;
    case 2:
        b.Noise(5, 10);
        for (u32 i = 0; i < combos; ++i) { b.Noise(5, 10); b.Combo(9, 5, 0, b.g.Chance(1, 2)); }
        b.Levels(b.g.Pick(8), 5);
        b.Noise(5, 10);
        b.EndMeleeData();
        break;
    case 3:
        b.Junk(1 + b.g.Pick(5));
        for (u32 i = 0; i < combos; ++i) { b.Combo(9, 1, 0, false); b.Junk(b.g.Pick(4)); }
        b.Levels(7, 0);
        b.Junk(2);
        b.EndMeleeData();
        break;
    case 4:
        b.Combo(9, 1, 0, false);                           // la primera completa define los escalares obsoletos
        for (u32 i = 1; i < combos + 1; ++i) b.Combo(9, 1, 55, b.g.Chance(1, 2));
        b.EndMeleeData();
        break;
    case 5:
        for (u32 i = 0; i < 1 + combos / 2; ++i) b.Combo(9, 0, 15, false);
        b.EndMeleeData();
        break;
    case 6:
        for (u32 i = 0; i < 1 + combos / 2; ++i) b.Combo(9, 0, 10, false);
        b.EndMeleeData();
        break;
    case 7:
        for (u32 i = 0; i < 1 + combos / 2; ++i) b.Combo(9, 0, 10, true);
        b.EndMeleeData();
        break;
    case 8: {
        b.Combo(9, 0, 0, false);
        for (u32 i = 0; i < combos; ++i) {
            const u32 k = b.g.Pick(4);
            b.Combo(k == 0 ? 10 + b.g.Pick(6) : k == 1 ? 1 + b.g.Pick(8) : k == 2 ? b.g.Pick(2) : 9, 2, 10, b.g.Chance(1, 2));
            if (b.g.Chance(1, 3)) b.EndCombo();                // END_COMBO suelto: fuera de bloque no hace nada
        }
        b.EndMeleeData();
        break;
    }
    case 9: {
        if (b.g.Chance(1, 2)) b.Combo(9, 0, 0, false);
        b.Levels(b.g.Pick(10), 2);
        if (b.g.Chance(1, 2)) b.Levels(b.g.Pick(4), 0);
        if (b.g.Chance(1, 2)) b.Combo(9, 0, 0, false);
        b.EndMeleeData();
        break;
    }
    case 10: {
        const u32 many = 13 + b.g.Pick(3);                 // 13..15 combos: las filas 12..14 pisan la tabla de niveles y el CColModel
        b.Combo(9, 0, 0, false);
        for (u32 i = 1; i < many; ++i) b.Combo(9, 0, 5, false);
        if (b.g.Chance(1, 2)) b.Levels(7, 0);
        b.EndMeleeData();
        break;
    }
    case 11: {
        const u32 k = b.g.Pick(4);
        for (u32 i = 0; i < 1 + combos / 3; ++i) {
            b.StartCombo();
            std::string grp = k == 0 ? c.groups.front() : k == 1 ? c.groups.back() : k == 2 ? std::string("UnknownGrp") : c.groups[b.g.Pick(u32(c.groups.size()))];
            b.Add(b.g.Word() + " " + grp);
            b.seenWord2 = true;
            for (u32 s = 1; s < 9; ++s) b.Add(b.Slot(s, false, false));
            b.EndCombo();
        }
        b.EndMeleeData();
        break;
    }
    case 12: {
        b.Combo(9, 0, 0, false);
        for (u32 i = 0; i < 1 + combos / 2; ++i) {                // como mucho 2 combos cerrados por vuelta: 1 + 6 * 2 = 13 filas
            const u32 k = b.g.Pick(4);
            if (k == 0) { b.StartCombo(); b.Add(b.Slot(0, false, false)); b.StartCombo(); b.Add("START_LEVELS"); for (u32 s = 1; s < 9; ++s) b.Add(b.Slot(s, false, false)); b.EndCombo(); }
            else if (k == 1) { b.Combo(b.g.Pick(12), 1, 0, false, false); b.StartCombo(); b.EndCombo(); }
            else if (k == 2) { b.Levels(2, 0); b.Combo(9, 0, 0, false); b.Add("START_LEVELS"); b.EndCombo(); }
            else { b.Add("START_LEVELS"); b.Add("START_COMBO"); b.Add("a 1 2 3"); b.EndCombo(); b.Combo(9, 0, 0, false); }
        }
        b.EndMeleeData();
        break;
    }
    case 13: {
        const u32 k = b.g.Pick(5);
        if (k == 0) {}                                     // fichero vacio: LoadLine devuelve nulo de entrada
        else if (k == 1) { b.EndMeleeData(); b.Combo(9, 0, 0, false); }
        else if (k == 2) { b.Noise(8, 10); b.Add("#"); }
        else if (k == 3) { b.Noise(8, 10); b.EndMeleeData(); b.Junk(2); }
        else { b.Junk(3); }
        break;
    }
    default: {                                             // 14..15: mezcla
        b.Combo(9, 0, 0, false);
        const u32 steps = 2 + b.g.Pick(7);
        for (u32 i = 0; i < steps; ++i) {
            switch (b.g.Pick(9)) {
            case 0: b.Combo(9, 3, 25, b.g.Chance(1, 2)); break;
            case 1: b.Combo(1 + b.g.Pick(14), 3, 25, b.g.Chance(1, 2)); break;
            case 2: b.Levels(b.g.Pick(10), 2); break;
            case 3: b.Junk(1 + b.g.Pick(3)); break;
            case 4: b.Noise(6, 10); break;
            case 5: b.Combo(9, 0, 0, false, false); break;
            case 6: b.EndCombo(); break;
            case 7: b.StartCombo(); break;
            default: b.Junk(1); break;
            }
        }
        if (b.g.Chance(2, 3)) b.EndMeleeData();
        break;
    }
    }
    return c;
}
}  // namespace FightMeleeData31Fix
