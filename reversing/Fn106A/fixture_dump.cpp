// Vuelca las filas `input` del banco Fn106A (Fixtures.hpp tal cual) para la autoprueba sin juego: fixture_selfcheck.py. Para las funciones 2..5 anade lo que el banco registra como `pre`/`src`/`buf_pre`
// (imagen previa de las arenas, reconstruida con las mismas semillas y el mismo relleno que LiveComparison.hpp).
#include <cstdio>
#include "Fixtures.hpp"
using namespace F106AFix;
static std::string Hex(const u8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { o += d[p[i] >> 4]; o += d[p[i] & 15]; } return o; }
template <size_t WIN> struct Ar { std::array<u8, Arena(WIN)> m{}; void Fill(u32 s) { FillWords(m.data(), m.size(), s); } void Put32(size_t off, u32 v) { std::memcpy(m.data() + kGuard + off, &v, 4); } void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); } };
int main() {
    for (unsigned fn = 0; fn < kFunctions; ++fn)
        for (unsigned n = 0; n < kCases; ++n) {
            const C c = MakeC(fn, n);
            std::string js = JsonC(c);
            u32 sd = c.seed * 13u + 60u;
            Ar<kTaskWin> task; Ar<kEventWin> ev; Ar<kEntWin> ea, eb, vx; Ar<kStrWin> str; Ar<kGlobalsWin> gl;
            task.Fill(sd++); ev.Fill(sd++); ea.Fill(sd++); eb.Fill(sd++); str.Fill(sd++); gl.Fill(sd++); vx.Fill(sd++);
            std::string extra;
            if (fn == 2) extra = ",\"ent_a\":0,\"pre\":\"" + Hex(task.m.data() + kGuard, 0x18) + "\"";
            if (fn == 5) { task.Put32(0, c.a[0]); extra = ",\"pre\":\"" + Hex(task.m.data() + kGuard, 0x20) + "\""; }
            if (fn == 4) { ev.Put32(0, c.a[2]); ev.Put32(0x38, 0); ev.Put32(0x30, 0); extra = ",\"pre\":\"" + Hex(ev.m.data() + kGuard, 0x3C) + "\""; }
            if (fn == 3) {
                const auto bytes = StrBytes(c);
                for (size_t i = 0; i < bytes.size(); ++i) str.Put8(i, bytes[i]);
                if (c.a[1] != 1) str.Put8(bytes.size(), 0);
                std::array<u8, 256> buf{}; FillWords(buf.data(), 256, sd);
                extra = ",\"src\":\"" + Hex(str.m.data() + kGuard, std::min<size_t>(kStrWin, size_t(c.a[2] & 0xFFu) + 258)) + "\",\"buf_pre\":\"" + Hex(buf.data(), 256) + "\"";
            }
            js.pop_back();
            std::printf("%s%s}\n", js.c_str(), extra.c_str());
        }
    return 0;
}
