// Vuelca las filas `input` de las 16384 fixtures de Fixtures.hpp (tal cual, compilado con cl) y las constantes de arena, para fixture_selfcheck.py. Una linea JSON por caso:
// la primera linea es `layout`. Las claves coinciden con las que registra Core.hpp (CtorInput). La instancia del singleton (0xC0B030) toma un valor nominal.
#include <cstdio>
#include <string>
#include "Fixtures.hpp"
using namespace H67Fix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
constexpr u32 kNominalInstance = 0x1234ABCDu;
int main() {
    std::printf("{\"layout\":{\"arena\":%u,\"obj_bytes\":%u,\"region_addr\":%u,\"region_bytes\":%u,\"cases\":%u}}\n", kArena, kObjBytes, kRegionAddr, kRegionBytes, kCases);
    for (unsigned n = 0; n < kCases; ++n) {
        const Case f = Make(n);
        u8 region[kRegionBytes];
        BuildRegionPre(f, kNominalInstance, region);
        std::printf("{\"func\":0,\"n\":%u,\"family\":%u,\"obj_off\":%u,\"mode\":%u,\"seed\":%u,\"gmode\":%u,\"gseed\":%u,\"region_pre\":\"%s\",\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
            f.n, f.family, f.objOff, f.mode, f.seedArena, f.gmode, f.gseed, Hex(region, kRegionBytes).c_str(), f.eax, f.edx, f.stack);
    }
}
