// Vuelca las filas `input` de los 5120 casos de Fixtures.hpp (una por linea, JSON) para calibrar/validar fixture_oracle.py SIN el juego.
// Compilar y ejecutar (estandar, sin tipos del juego):  cl /nologo /EHsc /std:c++20 fixture_dump.cpp && fixture_dump.exe > casos.jsonl
#include <cstdio>
#include <string>
#include "Fixtures.hpp"
using namespace RTShadowFix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
int main() {
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        std::printf("{\"case\":%u,\"group\":%d,\"family\":%d,\"obj_present\":%s,\"object\":\"%s\",\"parent_label\":%u,\"frame_pos\":[", n, f.group, f.family, f.objPresent ? "true" : "false",
                    Hex(f.object.data(), f.object.size()).c_str(), f.parentLabel);
        for (size_t i = 0; i < f.framePos.size(); ++i) std::printf("%s%u", i ? "," : "", f.framePos[i]);
        std::printf("],\"pre\":\"%s\",\"owner_seed\":%u,\"guard_seed\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u}\n", Hex(f.pre.data(), f.pre.size()).c_str(), f.ownerSeed, f.guardSeed, f.eax, f.edx, f.stack);
    }
}
