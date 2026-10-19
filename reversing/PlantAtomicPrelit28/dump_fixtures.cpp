// Herramienta SIN juego: vuelca los 4096 fixtures de Fixtures.hpp (el generador C++ real) como JSON, una linea por caso, con hashes del relleno para que
// `python -I fixture_oracle.py --check-dump <salida>` contraste el generador C++ con la reimplementacion Python y calcule la cobertura.
// Compilacion (consola x86 de VS, no es el proyecto):  cl /nologo /EHsc /std:c++20 /O2 /Fe:dump_fixtures.exe dump_fixtures.cpp
// Uso:  dump_fixtures.exe > fixtures.jsonl
#include <cstdio>
#include <vector>
#include "Fixtures.hpp"
using namespace PlantPrelitFix;
static void H(const char* name, const std::vector<u8>& b) { std::printf(",\"%s\":\"%016llx\"", name, (unsigned long long)Fnv64(b.data(), b.size())); }
int main() {
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        std::printf("{\"n\":%u,\"group\":%d,\"family\":%d,\"mode\":%u,\"flags\":%u,\"num_v\":%d,\"num_t\":%d,\"old_seed\":%u,\"data_seed\":%u,\"new_seed\":%u,\"new_flags\":%u,\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u",
                    f.n, f.group, f.family, f.mode, f.flags, f.numV, f.numT, f.oldSeed, f.dataSeed, f.newSeed, f.newFlags, f.eax, f.ecx, f.edx, f.stack);
        std::vector<u8> a(kArena), o(kOldArena), v(size_t(f.numV) * 12 + 1), t(size_t(f.numV) * 8 + 1), r(size_t(f.numT) * 8 + 1);
        FillWords(a.data(), a.size(), f.newSeed); H("fill_arena", a);
        FillWords(o.data(), o.size(), f.oldSeed); H("fill_old", o);
        v.resize(size_t(f.numV) * 12); t.resize(size_t(f.numV) * 8); r.resize(size_t(f.numT) * 8);
        FillData(v.data(), v.size(), f.dataSeed, f.mode, 0); H("data_verts", v);
        FillData(t.data(), t.size(), f.dataSeed, f.mode, 1); H("data_tex", t);
        FillData(r.data(), r.size(), f.dataSeed, f.mode, 2); H("data_tri", r);
        std::printf("}\n");
    }
    return 0;
}
