// Vuelca las filas `input` de los 4096 casos (una por linea, JSON) precedidas de las constantes, sin el juego: calibra la cobertura de run_local.py y alimenta la autoprueba cruzada del oraculo.
// Compilar: cl /nologo /std:c++20 /EHsc /I. fixture_dump.cpp && fixture_dump.exe > inputs.jsonl
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    std::puts(GaragesImpound30Fix::ConstantsLine().c_str());
    for (unsigned n = 0; n < GaragesImpound30Fix::kCases; ++n) std::puts(GaragesImpound30Fix::InputLine(GaragesImpound30Fix::Make(n)).c_str());
    return 0;
}
