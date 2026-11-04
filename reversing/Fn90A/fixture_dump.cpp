// Vuelca las filas `input` completas del banco Fn90A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py. Reproduce lo que LiveComparison.hpp anade a la entrada
// (base de la arena, imagen previa de la ventana, argumentos y ECX de entrada) con una base de arena ficticia alineada a 32.
#include <cstdio>
#include "Fixtures.hpp"
using namespace F90Fix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
static std::string Tag(u32 base, u32 v) {
    if (v >= base && v < base + kArena) { const long long d = (long long)v - (long long)(base + kGuard); return std::string("\"A") + (d < 0 ? "-" : "+") + std::to_string(d < 0 ? -d : d) + "\""; }
    return std::to_string(v);
}
int main() {
    constexpr u32 kBase = 0x01230000u;
    constexpr unsigned nargs[kFunctions] = {1, 4, 0, 0, 1, 0, 1, 2, 0, 0};
    constexpr bool isThis[kFunctions] = {false, true, true, true, false, true, true, true, true, true};
    for (unsigned fn = 0; fn < kFunctions; ++fn) for (unsigned n = 0; n < kCases; ++n) {
        const C c = MakeC(fn, n);
        u8 win[kWin];
        FillWindow(win, c.fill, c.seed * 13u + 2u);
        if (c.pokeSize == 2) { const u16 v = u16(c.poke); std::memcpy(win + c.pokeOff, &v, 2); }
        else if (c.pokeSize == 4) std::memcpy(win + c.pokeOff, &c.poke, 4);
        u32 args[4]{};
        switch (fn) {
        case 0: args[0] = c.a[0]; break;
        case 1: for (unsigned k = 0; k < 4; ++k) args[k] = c.a[k]; break;
        case 4: args[0] = (((kBase + u32(kGuard) + c.blk - kAnimBase) >> 5) & 0x07FFFFFFu) | (c.hi << 27); break;
        case 6: args[0] = c.a[0]; break;
        case 7: args[0] = c.a[0]; args[1] = kBase + u32(kGuard) + c.pOff; break;
        default: break;
        }
        const u32 seedEcx = isThis[fn] ? kBase + u32(kGuard) + c.off : c.ecx;
        std::string j = JsonC(c);
        j.pop_back();
        j += ",\"base\":" + std::to_string(kBase) + ",\"pre\":\"" + Hex(win, kWin) + "\",\"args\":[";
        for (unsigned k = 0; k < nargs[fn]; ++k) { if (k) j += ','; j += Tag(kBase, args[k]); }
        j += "],\"ecx_in\":" + Tag(kBase, seedEcx) + "}";
        std::printf("%s\n", j.c_str());
    }
    return 0;
}
