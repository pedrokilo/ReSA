// Vuelca las filas `input` de los 6144 casos (una por linea, JSON) sin el juego: para calibrar la cobertura de run_local.py y probar el oraculo.
// Compilar: cl /nologo /std:c++20 /EHsc /I. fixture_dump.cpp && fixture_dump.exe > inputs.jsonl
#include <cstdio>
#include "Fixtures.hpp"
int main() {
    for (unsigned n = 0; n < PickupsGen28Fix::kCases; ++n) std::puts(PickupsGen28Fix::InputLine(PickupsGen28Fix::Make(n)).c_str());
    return 0;
}
