// Volcador de fixtures (autoprueba cruzada): imprime una fila JSON por caso `{"fn":0,"case":n,"input":{...}}` con exactamente las filas `input` del banco.
//   cl /nologo /EHsc /O2 /std:c++20 /Fe:dump_fixtures.exe dump_fixtures.cpp   (x86) ; dump_fixtures.exe > rows.jsonl ; python -I fixture_oracle.py --selftest rows.jsonl
#include <cstdio>
#include "Fixtures.hpp"
using namespace AutoPreRender60Fix;
int main() {
    for (unsigned n = 0; n < kCases; ++n) std::printf("{\"fn\":0,\"case\":%u,\"input\":%s}\n", n, JsonCase(MakeCase(n)).c_str());
    return 0;
}
