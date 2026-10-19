// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py.
#include <cstdio>
#include "Fixtures.hpp"
using namespace FightMeleeData31Fix;
static void Str(const std::string& s) {
    std::putchar('"');
    for (unsigned char ch : s) {
        if (ch == '"' || ch == '\\') { std::putchar('\\'); std::putchar(ch); }
        else if (ch == '\t') std::fputs("\\t", stdout);
        else if (ch < 0x20) std::printf("\\u%04x", ch);
        else std::putchar(ch);
    }
    std::putchar('"');
}
static unsigned long long Fnv64(const void* p, size_t n) { unsigned long long h = 1469598103934665603ull; const auto* b = static_cast<const unsigned char*>(p); for (size_t i = 0; i < n; ++i) h = (h ^ b[i]) * 1099511628211ull; return h; }
int main() {
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::array<u8, kRegion> pre{};
        FillWords(pre.data(), kRegion, c.prefillSeed);
        std::printf("{\"family\":%u,\"num_groups\":%u,\"groups\":[", c.family, c.numGroups);
        for (size_t i = 0; i < c.groups.size(); ++i) { if (i) std::putchar(','); Str(c.groups[i]); }
        std::fputs("],\"lines\":[", stdout);
        for (size_t i = 0; i < c.lines.size(); ++i) { if (i) std::putchar(','); Str(c.lines[i]); }
        std::printf("],\"prefill_seed\":%u,\"prefill_hash\":\"%016llx\",\"eax\":%u,\"ecx\":%u,\"edx\":%u,\"stack_seed\":%u,\"region_base\":%u,\"region_off\":%u,\"region_bytes\":%u}\n",
                    c.prefillSeed, Fnv64(pre.data(), kRegion), c.eax, c.ecx, c.edx, c.stack, kRegionBase, kRegionOff, kRegionBytes);
    }
    std::printf("{\"sizes\":{\"region_off\":%u,\"region_bytes\":%u,\"region\":%u,\"max_groups\":%u,\"max_events\":%u}}\n", kRegionOff, kRegionBytes, kRegion, kMaxGroups, kMaxEvents);
    return 0;
}
