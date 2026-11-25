// FnD3DX9C: banco de la matematica propia (D3DX9NativeMath) contra las exportaciones de d3dx9_43.dll. Compilar con build_banco.cmd (x86).
// Uso: banco_matematica.exe [pares=200000] [semilla]. Compara bit a bit; un NaN contra otro NaN se cuenta aparte (solo difiere la carga util).
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <random>
#include <cmath>
#include "../../source/game_sa/RenderWare/rw/D3DX9NativeMath.h"

using F3 = void*(__stdcall*)(void*, const void*, const void*);
using F2 = void*(__stdcall*)(void*, const void*);
using FH = float*(__stdcall*)(float*, const uint16_t*, uint32_t);

static std::mt19937 rng(12345);
static float Special() {
    static const uint32_t pool[] = {0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x7F7FFFFF, 0xFF7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FA00000, 0x7F800001, 0x3F000000, 0x40000000, 0x33800000, 0x4B000000};
    uint32_t b = pool[rng() % (sizeof(pool) / sizeof(pool[0]))]; float f; memcpy(&f, &b, 4); return f;
}
static float Rand(int mode) {
    if (mode == 1) return Special();
    if (mode == 2 && (rng() % 4 == 0)) return Special();
    if (mode == 3) { uint32_t b = rng(); float f; memcpy(&f, &b, 4); return f; }
    return (float)((double)(int32_t)rng() / 2147483648.0 * (mode == 4 ? 1e-3 : (mode == 5 ? 1e6 : 100.0)));
}
struct Stat { uint64_t n = 0, dif = 0, nan = 0; };
static bool Same(const float* a, const float* b, int n, bool& onlyNan) {
    onlyNan = true; bool same = true;
    for (int i = 0; i < n; ++i) {
        uint32_t x, y; memcpy(&x, a + i, 4); memcpy(&y, b + i, 4);
        if (x != y) { same = false; if (!(std::isnan(a[i]) && std::isnan(b[i]))) onlyNan = false; }
    }
    return same;
}
int main(int argc, char** argv) {
    int pairs = argc > 1 ? atoi(argv[1]) : 200000;
    if (argc > 2) rng.seed(atoi(argv[2]));
    HMODULE m = LoadLibraryExA("d3dx9_43.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!m) { printf("no se pudo cargar d3dx9_43.dll\n"); return 2; }
    auto dTC = (F3)GetProcAddress(m, "D3DXVec3TransformCoord");
    auto dTN = (F3)GetProcAddress(m, "D3DXVec3TransformNormal");
    auto dMM = (F3)GetProcAddress(m, "D3DXMatrixMultiply");
    auto dMT = (F2)GetProcAddress(m, "D3DXMatrixTranspose");
    auto dH = (FH)GetProcAddress(m, "D3DXFloat16To32Array");
    Stat sTC[6], sTN[6], sMM[6], sMT[6];
    for (int mode = 0; mode < 6; ++mode) {
        for (int i = 0; i < pairs; ++i) {
            alignas(16) float a[16], b[16], v[4], o1[16], o2[16]; bool nan;
            for (auto& x : a) x = Rand(mode); for (auto& x : b) x = Rand(mode); for (auto& x : v) x = Rand(mode);
            memset(o1, 0, sizeof o1); memset(o2, 0, sizeof o2);
            D3DX9Native::Vec3TransformCoord(o1, v, a); dTC(o2, v, a); sTC[mode].n++; if (!Same(o1, o2, 3, nan)) { sTC[mode].dif++; if (nan) sTC[mode].nan++; }
            memset(o1, 0, sizeof o1); memset(o2, 0, sizeof o2);
            D3DX9Native::Vec3TransformNormal(o1, v, a); dTN(o2, v, a); sTN[mode].n++; if (!Same(o1, o2, 3, nan)) { sTN[mode].dif++; if (nan) sTN[mode].nan++; }
            D3DX9Native::MatrixMultiply(o1, a, b); dMM(o2, a, b); sMM[mode].n++; if (!Same(o1, o2, 16, nan)) { sMM[mode].dif++; if (nan) sMM[mode].nan++; }
            D3DX9Native::MatrixTranspose(o1, a); dMT(o2, a); sMT[mode].n++; if (!Same(o1, o2, 16, nan)) { sMT[mode].dif++; if (nan) sMT[mode].nan++; }
            // aliasing: out == a
            float c1[16], c2[16]; memcpy(c1, a, 64); memcpy(c2, a, 64);
            D3DX9Native::MatrixMultiply(c1, c1, b); dMM(c2, c2, b); sMM[mode].n++; if (!Same(c1, c2, 16, nan)) { sMM[mode].dif++; if (nan) sMM[mode].nan++; }
        }
    }
    const char* nm[6] = {"rango +-100", "especiales", "mixto", "bits aleatorios", "pequenos 1e-3", "grandes 1e6"};
    int fail = 0;
    auto rep = [&](const char* f, Stat* s) { for (int k = 0; k < 6; ++k) { printf("%-24s %-16s pares=%llu dif=%llu (solo carga util NaN=%llu)\n", f, nm[k], s[k].n, s[k].dif, s[k].nan); fail += (int)(s[k].dif - s[k].nan); } };
    rep("Vec3TransformCoord", sTC); rep("Vec3TransformNormal", sTN); rep("MatrixMultiply", sMM); rep("MatrixTranspose", sMT);
    // Float16To32Array: los 65536 valores, en bloques de 1..7 para cubrir bucles de cuatro y resto
    uint64_t hn = 0, hd = 0, hn2 = 0;
    static uint16_t in[65536]; static float o1[65536], o2[65536];
    for (int i = 0; i < 65536; ++i) in[i] = (uint16_t)i;
    for (int cnt : {65536, 1, 2, 3, 5, 7}) {
        int total = cnt == 65536 ? 65536 : 4096;
        for (int s = 0; s + cnt <= total; s += cnt) {
            memset(o1, 0, sizeof o1); memset(o2, 0, sizeof o2);
            D3DX9Native::Float16To32Array(o1, in + s, cnt); dH(o2, in + s, cnt);
            for (int k = 0; k < cnt; ++k) { hn++; if (memcmp(o1 + k, o2 + k, 4)) { if (std::isnan(o1[k]) && std::isnan(o2[k])) hn2++; else { hd++; if (hd < 10) printf("half 0x%04X nativo=%08X dll=%08X\n", in[s + k], *(uint32_t*)(o1 + k), *(uint32_t*)(o2 + k)); } } }
        }
    }
    printf("%-24s %-16s valores=%llu dif=%llu (solo carga util NaN=%llu)\n", "Float16To32Array", "exhaustivo", hn, hd, hn2);
    fail += (int)hd;
    printf(fail ? "RESULTADO: HAY DIFERENCIAS (%d)\n" : "RESULTADO: 0 diferencias de bits (salvo carga util NaN)\n", fail);
    return fail ? 1 : 0;
}
