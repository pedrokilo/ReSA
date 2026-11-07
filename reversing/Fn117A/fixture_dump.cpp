// Vuelca las filas `input` del banco Fn117A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py. Anade lo que el banco registra como `pre` (imagen previa del evento,
// reconstruida con las mismas semillas y el mismo relleno que LiveComparison.hpp).
#include <cstdio>
#include "Fixtures.hpp"
using namespace F117AFix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
template <size_t WIN> struct Ar { std::array<u8, Arena(WIN)> m{}; void Fill(u32 s) { FillWords(m.data(), m.size(), s); } void Put32(size_t off, u32 v) { std::memcpy(m.data() + kGuard + off, &v, 4); } };
int main() {
    for (unsigned fn = 0; fn < kFunctions; ++fn)
        for (unsigned n = 0; n < kCases; ++n) {
            const C c = MakeC(fn, n);
            const Spec& sp = kSpecs[fn];
            std::string js = JsonC(c);
            u32 sd = c.seed * 13u + 60u;
            Ar<kEventWin> ev; Ar<kEntWin> ea, eb;
            ev.Fill(sd++); ea.Fill(sd++); eb.Fill(sd++);
            if (sp.kind == 0) {
                ev.Put32(0, c.a[2]);
                ev.Put32(sp.off[0], c.a[0] == 0 ? 0x10000000u + (1u << 20) + u32(kGuard) : (c.a[0] == 1 ? 0u : 0x00BADC00u));   // la etiqueta no importa: el oraculo solo usa `pre` para saber que bytes cambian; el banco real escribe la direccion de la arena
                if (sp.refs == 2) ev.Put32(sp.off[1], c.a[1] == 0 ? 0x10000000u + (2u << 20) + u32(kGuard) : (c.a[1] == 1 ? 0u : 0x00BADC40u));
            } else {
                ev.Put32(0, kPreVtable);
            }
            std::string extra = ",\"pre\":\"" + Hex(ev.m.data() + kGuard, 0x40) + "\"";
            js.pop_back();
            std::printf("%s%s}\n", js.c_str(), extra.c_str());
        }
    return 0;
}
