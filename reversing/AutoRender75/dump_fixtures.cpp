// Volcador de fixtures (autoprueba cruzada): imprime una fila JSON por caso `{"case":n,"input":{...}}` con exactamente las filas `input` del banco.
//   cl /nologo /EHsc /O2 /std:c++20 /Fe:dump_fixtures.exe dump_fixtures.cpp ; dump_fixtures.exe > rows.jsonl ; python -I fixture_oracle.py --coverage rows.jsonl
#include <cstdio>
#include "Fixtures.hpp"
using namespace AutoRender75Fix;
int main() {
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"case\":%u,\"input\":%s}\n", n, JsonFR(MakeFR(n)).c_str());
    return 0;
}
