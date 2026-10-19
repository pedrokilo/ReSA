// Herramienta SIN el juego: vuelca los 4608 `input` del banco FxInfoLoad28 (mismo formato que Core.hpp::InputOf) como JSON lineas, para fixture_mirror.py.
//   cl /nologo /EHsc /std:c++20 /O2 fixture_dump.cpp /Fe:fixture_dump.exe   (en un directorio temporal) ; fixture_dump.exe > casos.jsonl
#include "Fixtures.hpp"
#include <cstdio>
using namespace FxInfoLoad28Fix;
static void Esc(const std::string& s) {
    std::putchar('"');
    for (unsigned char c : s) {
        if (c == '"' || c == '\\') std::printf("\\%c", c);
        else if (c == '\n') std::printf("\\n");
        else if (c == '\r') std::printf("\\r");
        else if (c == '\t') std::printf("\\t");
        else if (c < 0x20) std::printf("\\u%04x", c);
        else std::putchar(c);
    }
    std::putchar('"');
}
int main() {
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        std::printf("{\"kind\":%u,\"sub\":%u,\"stream\":", f.kind, f.sub);
        Esc(f.stream);
        std::printf(",\"version\":%d,\"pre\":\"", f.version);
        for (u8 b : f.pre) std::printf("%02x", b);
        std::printf("\",\"junk_type\":%u,\"junk_b6\":%u,\"plan_body\":[", f.junkType, f.junkB6);
        for (size_t i = 0; i < f.plan.size(); ++i) std::printf("%s%u", i ? "," : "", f.plan[i].body);
        std::printf("],\"plan_retype\":[");
        for (size_t i = 0; i < f.plan.size(); ++i) std::printf("%s%u", i ? "," : "", f.plan[i].retype);
        std::printf("],\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"count\":%u}\n", f.eax, f.edx, f.stack, f.count);
    }
}
