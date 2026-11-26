// Banco de PadAnalog (parte pura, sin el juego): compara PadAnalog.h con la rutina de lectura de GInputSA.asi evaluada instruccion a instruccion (gen_oracle.py) y comprueba que
// la tuberia con la configuracion por defecto reproduce el codigo actual de CPad::ProcessPad. Ademas imprime las tablas de la curva de sensibilidad.
// Compilar (x86, como el juego): cl /nologo /EHsc /O2 /arch:IA32 /I..\..\source\game_sa bench_core.cpp ; uso: bench_core oracle_vectores.txt
#include "PadAnalog.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

using namespace PadAnalog;

static int g_fail = 0, g_total = 0, g_off1 = 0;
#define CHECK(cond, ...) do { g_total++; if (!(cond)) { g_fail++; if (g_fail < 30) { printf("FALLO: "); printf(__VA_ARGS__); printf("\n"); } } } while (0)

static float BitsToFloat(const std::string& hex) {
    uint32_t u = (uint32_t)strtoul(hex.c_str(), nullptr, 16);
    // el generador escribe struct.pack('<f').hex(): bytes little-endian
    uint32_t v = ((u & 0xff) << 24) | ((u & 0xff00) << 8) | ((u >> 8) & 0xff00) | ((u >> 24) & 0xff);
    float f; memcpy(&f, &v, 4); return f;
}

static void TestOracle(const char* path) {
    std::ifstream in(path);
    std::string line;
    int nS = 0, nF = 0, nI = 0, exactS = 0, offByOne = 0, worse = 0;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        char kind; ss >> kind;
        if (kind == 'S') {
            int dzl, dzr, sl, sr, flags, lx, ly, rx, ry, lt, rt; char bar; int o[6];
            ss >> dzl >> dzr >> sl >> sr >> flags >> lx >> ly >> rx >> ry >> lt >> rt >> bar >> o[0] >> o[1] >> o[2] >> o[3] >> o[4] >> o[5];
            StickParams L, R;
            L.deadzone = DeadzoneFromPercent(dzl); R.deadzone = DeadzoneFromPercent(dzr);
            L.sensitivity = SensitivityFromPercent(sl); R.sensitivity = SensitivityFromPercent(sr);
            L.invertX = flags & 1; L.invertY = (flags >> 1) & 1; L.swapAxes = (flags >> 2) & 1;
            R.invertX = (flags >> 3) & 1; R.invertY = (flags >> 4) & 1; R.swapAxes = (flags >> 5) & 1;
            const StickOut a = ConvertStick(lx * kXInputScale, ly * kXInputScale, L, false);
            const StickOut b = ConvertStick(rx * kXInputScale, ry * kXInputScale, R, false);
            const int got[6] = { a.x, a.y, b.x, b.y, TriggerValue(lt), TriggerValue(rt) };
            bool exact = true;
            for (int k = 0; k < 6; k++) {
                const int d = got[k] - o[k];
                if (d != 0) { exact = false; }
                if (d > 1 || d < -1) { worse++; CHECK(false, "S linea '%s' campo %d: C++ %d, .asi %d", line.c_str(), k, got[k], o[k]); }
            }
            if (exact) { exactS++; } else { offByOne++; }
            nS++;
        } else if (kind == 'F') {
            int val; std::string bb, tb; char bar; int o[4];
            ss >> val >> bb >> tb >> bar >> o[0] >> o[1] >> o[2] >> o[3];
            const float b = BitsToFloat(bb), t = BitsToFloat(tb);
            const float factor = FaceButtonsFactor(val);
            const int got[4] = { FaceButtonValue(b, factor), FaceButtonValue(b, factor), FaceButtonValue(b, factor), ScpTriggerValue(t) };
            for (int k = 0; k < 4; k++) { CHECK(got[k] == o[k], "F linea '%s' campo %d: C++ %d, .asi %d", line.c_str(), k, got[k], o[k]); }
            nF++;
        } else if (kind == 'I') {
            std::string bb; char bar; int o; ss >> bb >> bar >> o;
            CHECK(FaceButtonValue(BitsToFloat(bb), FaceButtonsFactor(0)) == o, "I linea '%s': C++ %d, .asi %d", line.c_str(), FaceButtonValue(BitsToFloat(bb), FaceButtonsFactor(0)), o);
            nI++;
        }
    }
    printf("vectores del .asi: S=%d (idénticos %d, difieren en 1: %d, peor: %d) F=%d I=%d\n", nS, exactS, offByOne, worse, nF, nI);
    g_off1 = offByOne;
}

// Tuberia por defecto == codigo actual (lambda UpdateJoyStickPosition de CPad::ProcessPad)
static void TestLegacyEquivalence() {
    std::mt19937 rng(7);
    PadParams cfg; // por defecto: zona muerta -1 (actual), sensibilidad 1, sin inversiones
    long n = 0, bad = 0;
    for (int flags = 0; flags < 64; flags++) {
        for (int v = -2000; v <= 2000; v++) {
            StickInput in;
            const float fv = (float)((double)v * 0.0005f); // NormalizePadAxis
            const int w = (int)(rng() % 4001) - 2000;
            const float fw = (float)((double)w * 0.0005f);
            in.legacyLX = fv; in.legacyLY = -fw; in.legacyRX = fw; in.legacyRY = -fv; // Y con el signo del juego (Axis(.., true))
            in.menuInvertX1 = flags & 1; in.menuInvertY1 = (flags >> 1) & 1; in.menuSwap1 = (flags >> 2) & 1;
            in.menuInvertX2 = (flags >> 3) & 1; in.menuInvertY2 = (flags >> 4) & 1; in.menuSwap2 = (flags >> 5) & 1;
            const PadSticksOut a = LegacySticks(in), b = ComputeSticks(cfg, in);
            n++;
            if (a.left.x != b.left.x || a.left.y != b.left.y || a.right.x != b.right.x || a.right.y != b.right.y) { bad++; if (bad < 10) printf("  distinto: flags=%d v=%d w=%d legacy=(%d,%d,%d,%d) nueva=(%d,%d,%d,%d)\n", flags, v, w, a.left.x, a.left.y, a.right.x, a.right.y, b.left.x, b.left.y, b.right.x, b.right.y); }
        }
    }
    CHECK(bad == 0, "tuberia por defecto distinta del codigo actual en %ld de %ld", bad, n);
    printf("equivalencia con el codigo actual (64 combinaciones de inversion/permutacion del menu x 4001 valores): %ld casos, %ld distintos\n", n, bad);
}

static void TestPieces() {
    // zonas muertas
    CHECK(ApplyDeadzone(0.239, 0.24) == 0.0, "dentro de la zona muerta");
    CHECK(std::fabs(ApplyDeadzone(1.0, 0.24) - 1.0) < 1e-12, "extremo = 1");
    CHECK(std::fabs(ApplyDeadzone(-1.0, 0.24) + 1.0) < 1e-12, "extremo = -1");
    CHECK(DeadzoneFromPercent(90) == 0.75F, "tope 0,75");
    CHECK(SensitivityFromPercent(300) == 2.0F, "tope 2,0");
    // curva
    for (double x : { -1.0, -0.5, 0.0, 0.3, 0.5, 1.0 }) {
        CHECK(ApplyCurve(x, 1.0) == x, "sens 100 es lineal exacta (x=%g)", x);
        CHECK(std::fabs(ApplyCurve(x, 0.0) - x * x * x) < 1e-15, "sens 0 es x^3");
        CHECK(std::fabs(ApplyCurve(x, 2.0) - std::cbrt(x)) < 1e-12, "sens 200 es raiz cubica");
    }
    // triggers
    CHECK(TriggerValue(30) == 0 && TriggerValue(31) == 1 && TriggerValue(255) == 254, "gatillos");
    // vibracion
    CHECK(MotorFromShakeFreq(0) == 0 && MotorFromShakeFreq(255) == 65535 && MotorFromShakeFreq(128) == 128 * 257, "motor");
    CHECK(ShakeDurAfterFrame(100, 1.0F) == 80 && ShakeDurAfterFrame(100, 0.5F) == 90 && ShakeDurAfterFrame(10, 1.0F) == -10, "duracion");
    // southpaw / invertir / permutar con valores concretos
    PadParams c; c.left.deadzone = DeadzoneFromPercent(0); c.right.deadzone = DeadzoneFromPercent(0);
    StickInput in; in.rawLX = 16384; in.rawLY = 32767; in.rawRX = -32768; in.rawRY = 8192;
    PadSticksOut o = ComputeSticks(c, in);
    CHECK(o.left.x == 63 && o.left.y == -127 && o.right.x == -127 && o.right.y == -31, "base (%d,%d,%d,%d)", o.left.x, o.left.y, o.right.x, o.right.y);
    c.southpaw = true; o = ComputeSticks(c, in);
    CHECK(o.left.x == -127 && o.left.y == -31 && o.right.x == 63 && o.right.y == -127, "southpaw");
    c.southpaw = false; c.left.swapAxes = true; o = ComputeSticks(c, in);
    CHECK(o.left.x == -127 && o.left.y == 63, "swap izq (%d,%d)", o.left.x, o.left.y);
    c.left.swapAxes = false; c.left.invertX = true; c.left.invertY = true; o = ComputeSticks(c, in);
    CHECK(o.left.x == -63 && o.left.y == 127, "invertir izq");
    // la curva solo toca el eje X salvo con SensitivityBothAxes
    c = PadParams{}; c.left.deadzone = 0.0F; c.left.sensitivity = 0.0F; in.rawLX = 16384; in.rawLY = 16384; o = ComputeSticks(c, in);
    CHECK(o.left.x == 15 && o.left.y == -63, "curva solo en X (%d,%d)", o.left.x, o.left.y);
    c.curveBothAxes = true; o = ComputeSticks(c, in);
    CHECK(o.left.x == 15 && o.left.y == -15, "curva en ambos ejes (%d,%d)", o.left.x, o.left.y);
}

static void PrintCurveTable() {
    printf("\nTabla de la curva de sensibilidad (valor normalizado de entrada tras la zona muerta -> salida en [-127,127]; solo eje X):\n");
    printf("%-10s", "entrada");
    const int sens[] = { 0, 25, 50, 75, 100, 125, 150, 175, 200 };
    for (int s : sens) { printf(" s=%-4d", s); }
    printf("\n");
    for (double x : { 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 }) {
        printf("%-10.2f", x);
        for (int s : sens) {
            StickParams p; p.deadzone = 0.0F; p.sensitivity = SensitivityFromPercent(s); p.invertY = true;
            printf(" %-6d", (int)ConvertStick(x * 32767.0 / 32767.0, 0.0, p, false).x);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    TestPieces();
    if (argc > 1) { TestOracle(argv[1]); }
    TestLegacyEquivalence();
    PrintCurveTable();
    printf("\nBANCO: %d comprobaciones, %d fallos\n", g_total, g_fail);
    return g_fail ? 1 : 0;
}
