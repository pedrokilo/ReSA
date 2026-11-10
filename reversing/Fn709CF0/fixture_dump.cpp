// Vuelca las entradas efectivas de los 16384 casos de Fn709CF0 sin juego (para fixture_selfcheck.py): repite la preparacion de las arenas de LiveComparison.hpp (Setup/InputJson) sobre matrices normales.
#include <cstdio>
#include "Fixtures.hpp"
using namespace F709Fix;
struct Ar {
    std::array<u8, Arena(kWin)> m{};
    std::int64_t Off(std::int64_t o) const { return std::int64_t(kGuard) + o; }
    void Put(std::int64_t o, const void* p, std::size_t n) { std::memcpy(m.data() + Off(o), p, n); }
    u32 Get(std::int64_t o) const { u32 v; std::memcpy(&v, m.data() + Off(o), 4); return v; }
};
int main() {
    const unsigned cases = kCases;
    for (unsigned n = 0; n < cases; ++n) {
        const C c = MakeC(0, n);
        Ar A, B, Cc, D;
        FillKind(A.m.data(), A.m.size(), c.a[2], c.a[12]); FillKind(B.m.data(), B.m.size(), c.a[2], c.a[13]); FillKind(Cc.m.data(), Cc.m.size(), c.a[2], c.a[14]); FillKind(D.m.data(), D.m.size(), c.a[2], c.a[15]);
        const u32 kind = c.a[1];
        Ar& pa = A; Ar& na = (kind == 0 || kind == 3) ? B : A; Ar& ta = (kind == 0 || kind == 3) ? Cc : A;
        std::array<u8, kParamBytes> params{};
        FillWords(params.data(), params.size(), c.seed ^ 0x5151u);
        std::memcpy(params.data() + 0x00, c.at.data(), 12);
        std::memcpy(params.data() + 0x4C, &c.shadowDword, 4);
        std::memcpy(params.data() + 0x50, &c.fadeDword, 4);
        std::memcpy(params.data() + 0x54, &c.batchDword, 4);
        pa.Put(i32(c.a[0]), params.data(), params.size());
        ta.Put(i32(c.a[4]), c.tri.data(), 36);
        na.Put(i32(c.a[3]), c.normal.data(), 12);
        std::string s = JsonC(c);
        s.pop_back();
        s += ",\"effective\":{\"normal\":[" + std::to_string(na.Get(i32(c.a[3]))) + "," + std::to_string(na.Get(i32(c.a[3]) + 4)) + "," + std::to_string(na.Get(i32(c.a[3]) + 8)) + "],\"tri\":[";
        for (int i = 0; i < 9; ++i) s += (i ? "," : "") + std::to_string(ta.Get(i32(c.a[4]) + 4 * i));
        s += "],\"at\":[" + std::to_string(pa.Get(i32(c.a[0]))) + "," + std::to_string(pa.Get(i32(c.a[0]) + 4)) + "," + std::to_string(pa.Get(i32(c.a[0]) + 8)) + "]";
        s += ",\"shadow_dword\":" + std::to_string(pa.Get(i32(c.a[0]) + 0x4C)) + ",\"fade_dword\":" + std::to_string(pa.Get(i32(c.a[0]) + 0x50));
        // etiquetas: arena (A=0, B=1, C=2) y desplazamiento respecto a la ventana; las del banco incluyen la guarda (kGuard) y 0x10000000
        const auto tg = [&](int arena, std::int64_t off) { return std::to_string(0x10000000ll + (std::int64_t(arena) << 20) + std::int64_t(kGuard) + off); };
        s += ",\"tags\":[" + tg((kind == 0 || kind == 3) ? 1 : 0, i32(c.a[3])) + "," + tg((kind == 0 || kind == 3) ? 2 : 0, i32(c.a[4])) + "," + tg(0, i32(c.a[0])) + "]}}";
        std::puts(s.c_str());
    }
}
