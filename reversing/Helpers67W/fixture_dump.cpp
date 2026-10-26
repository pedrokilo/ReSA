// Vuelca las filas `input` de las 16384 fixtures de Fixtures.hpp (tal cual, compilado con cl) y las constantes de la region, para fixture_selfcheck.py. Una linea JSON por caso:
// la primera linea es `layout`. Las claves coinciden con las que registra Core.hpp (CaseInput).
#include <cstdio>
#include <string>
#include <vector>
#include "Fixtures.hpp"
using namespace H67Fix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
int main() {
    std::printf("{\"layout\":{\"region_address\":%u,\"region_bytes\":%u,\"idx_off\":%u,\"vtx_off\":%u,\"vertex_bytes\":%u,\"guard_bytes\":%u,\"cases\":%u}}\n",
        kRegionAddress, kRegionBytes, kIdxOff, kVtxOff, kVertexBytes, kGuardBytes, kCases);
    static u8 ref[kRegionBytes];
    for (unsigned n = 0; n < kCases; ++n) {
        const Case f = MakeCase(n);
        BuildRegionRef(f, ref);
        std::printf("{\"func\":0,\"n\":%u,\"family\":%u,\"x\":%d,\"y\":%d,\"min_x\":%u,\"max_x\":%u,\"min_y\":%u,\"max_y\":%u,\"vtx\":%u,\"idx\":%u,", f.n, f.family, f.x, f.y, f.minX, f.maxX, f.minY, f.maxY, f.vtx, f.idx);
        std::printf("\"pre_vertices\":\"%s\",\"pre_indices\":\"%s\",\"mode\":%u,\"seed\":%u,\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u}\n",
            Hex(ref + kVtxOff + f.vtx * kVertexBytes, 4 * kVertexBytes).c_str(), Hex(ref + kIdxOff + 2 * f.idx, 12).c_str(), f.mode, f.seedRegion, f.eax, f.ecx, f.edx, f.stack);
    }
}
