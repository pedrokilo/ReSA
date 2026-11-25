// Banco oraculo librw (sin ejecutar el juego): compara las rutinas HAnim de ReSA con las equivalentes de librw (MIT, aap) y detecta lecturas de pila sin inicializar.
// Se enlaza con nuestro.cpp (nuestro codigo extraido por extraer.py) y con librw.lib (D:\Original\librw-test\build_null\src\librw.lib, plataforma NULL).
// Layout de fotograma (igual en ambos): [0] y [1] = punteros de cabecera (clave anterior / tiempo en el fotograma clave), [2..5] = cuaternion x,y,z,w, [6..8] = traslacion.
//   Fotograma clave: [0]=prev, [1]=time. Fotograma interpolado: [0]=keyFrame1, [1]=keyFrame2.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdint>
#include <vector>
#include <string>
#include "rw.h"

extern "C" {
void n_interp(float* out, float* in1, float* in2, float t);
void n_blend(float* out, float* in1, float* in2, float a);
void n_apply(float* m, const float* f);
void n_add(float* o, const float* a, const float* b);
void n_mulrecip(float* f, const float* s);
int n_update(int numNodes, const int* nodeFlags, const float* frames, float* outMats, unsigned hflags, int unused);
}

// ---------------- utilidades ----------------
static uint64_t rng_s = 0x9E3779B97F4A7C15ull;
static uint32_t rnd() { rng_s ^= rng_s << 13; rng_s ^= rng_s >> 7; rng_s ^= rng_s << 17; return uint32_t(rng_s >> 16); }
static float frand(float a, float b) { return a + (b - a) * (rnd() / 4294967296.0f); }
static void randUnitQuat(float* q) {
    float x = frand(-1, 1), y = frand(-1, 1), z = frand(-1, 1), w = frand(-1, 1);
    float n = std::sqrt(x*x + y*y + z*z + w*w); if (n < 1e-3f) { x = 0; y = 0; z = 0; w = 1; n = 1; }
    q[0] = x / n; q[1] = y / n; q[2] = z / n; q[3] = w / n;
}
static bool isnanf(float f) { return f != f; }
static bool bad(float f) { return isnanf(f) || std::fabs(f) > 1e7f; }

static void __declspec(noinline) smash(unsigned char v) {
    volatile unsigned char buf[12288];
    for (size_t i = 0; i < sizeof buf; ++i) buf[i] = v;
}
static const unsigned char kFills[] = { 0x00, 0xCC, 0xFF, 0x5A, 0xA5 };

static int g_fail = 0;
static void report(const char* nombre, long total, long difieren, double maxd, const char* nota = "") {
    printf("%-58s casos=%-7ld con_diferencia=%-6ld max|d|=%-12.6g %s\n", nombre, total, difieren, maxd, nota);
}

// ---------------- lado librw ----------------
struct LKey { void* prev; float time; rw::Quat q; rw::V3d t; };
static void lib_interp(float* out, const float* in1, const float* in2, float t) {
    const LKey* a = (const LKey*)in1; const LKey* b = (const LKey*)in2;
    float al = (t - a->time) / (b->time - a->time);
    rw::V3d tt = rw::lerp(a->t, b->t, al);
    rw::Quat q = rw::slerp(a->q, b->q, al);
    out[2] = q.x; out[3] = q.y; out[4] = q.z; out[5] = q.w; out[6] = tt.x; out[7] = tt.y; out[8] = tt.z;
}
static void lib_apply(float* m16, const float* f) {
    rw::Matrix m; memset(&m, 0, sizeof m);
    rw::Quat q = rw::makeQuat(f[5], f[2], f[3], f[4]);
    m.rotate(q, rw::COMBINEREPLACE);
    m.pos = rw::makeV3d(f[6], f[7], f[8]);
    m.optimize();
    memcpy(m16, &m, sizeof(float) * 16);
}
static void libApplyCB(void* r, void* frame) {
    rw::Matrix* m = (rw::Matrix*)r; const float* f = (const float*)frame;
    m->rotate(rw::makeQuat(f[5], f[2], f[3], f[4]), rw::COMBINEREPLACE);
    m->pos = rw::makeV3d(f[6], f[7], f[8]);
}
static void lib_update(int n, const int* nodeFlags, const float* frames, float* outMats) {
    const int keySize = 36;
    rw::AnimInterpolator* ai = (rw::AnimInterpolator*)calloc(1, sizeof(rw::AnimInterpolator) + n * keySize);
    ai->numNodes = n; ai->currentInterpKeyFrameSize = keySize; ai->applyCB = libApplyCB;
    memcpy(ai->getFrames(), frames, n * keySize);
    rw::HAnimHierarchy h; memset(&h, 0, sizeof h);
    h.numNodes = n; h.interpolator = ai;
    h.matrices = (rw::Matrix*)_aligned_malloc((n + 1) * sizeof(rw::Matrix), 16); memset(h.matrices, 0, (n + 1) * sizeof(rw::Matrix));
    rw::HAnimNodeInfo* ni = (rw::HAnimNodeInfo*)calloc(n, sizeof(rw::HAnimNodeInfo));
    for (int i = 0; i < n; ++i) { ni[i].flags = nodeFlags[i]; ni[i].index = i; }
    h.nodeInfo = ni;
    if (getenv("BANCO_DEBUG")) printf("   lib: n=%d flags0=%d keySize=%d\n", n, nodeFlags[0], keySize);
    h.updateMatrices();
    if (getenv("BANCO_DEBUG")) printf("   lib: ok\n");
    memcpy(outMats, h.matrices, n * 64);
    _aligned_free(h.matrices); free(ni); free(ai);
}

#include <windows.h>
static int seh_update(int n, const int* fl, const float* fr, float* o) {
    __try { return n_update(n, fl, fr, o, 0, 0); }
    __except (printf("   EXCEPCION 0x%08lx en 0x%p, dir=0x%p\n", GetExceptionCode(), GetExceptionInformation()->ExceptionRecord->ExceptionAddress, (void*)GetExceptionInformation()->ExceptionRecord->ExceptionInformation[1]), EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

// ---------------- generacion de entradas ----------------
struct Pair { float a[9], b[9]; float t; const char* cat; };
static void mkKey(float* k, float time, const float* q, const float* tr) {
    memset(k, 0, 36); k[1] = time; k[2] = q[0]; k[3] = q[1]; k[4] = q[2]; k[5] = q[3]; k[6] = tr[0]; k[7] = tr[1]; k[8] = tr[2];
}
static std::vector<Pair> makePairs() {
    std::vector<Pair> v;
    auto add = [&](const char* cat, const float* qa, const float* qb, float ta, float tb, float t) {
        Pair p; float t1[3] = { frand(-2,2), frand(-2,2), frand(-2,2) }, t2[3] = { frand(-2,2), frand(-2,2), frand(-2,2) };
        mkKey(p.a, ta, qa, t1); mkKey(p.b, tb, qb, t2); p.t = t; p.cat = cat; v.push_back(p);
    };
    for (int i = 0; i < 20000; ++i) { float a[4], b[4]; randUnitQuat(a); randUnitQuat(b); float ta = frand(0, 2), tb = ta + frand(0.01f, 1); add("unitarios aleatorios (dot de cualquier signo)", a, b, ta, tb, frand(ta, tb)); }
    for (int i = 0; i < 4000; ++i) { float a[4], b[4]; randUnitQuat(a); memcpy(b, a, 16); for (int k = 0; k < 4; ++k) b[k] += frand(-0.01f, 0.01f); float n = std::sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]+b[3]*b[3]); for (int k = 0; k < 4; ++k) b[k] /= n; add("casi iguales (dot > 0.999)", a, b, 0, 1, frand(0, 1)); }
    for (int i = 0; i < 4000; ++i) { float a[4], b[4]; randUnitQuat(a); for (int k = 0; k < 4; ++k) b[k] = -a[k] + frand(-0.01f, 0.01f); float n = std::sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]+b[3]*b[3]); for (int k = 0; k < 4; ++k) b[k] /= n; add("casi opuestos (dot < -0.999)", a, b, 0, 1, frand(0, 1)); }
    for (int i = 0; i < 4000; ++i) { float a[4], b[4]; randUnitQuat(a); randUnitQuat(b); float s1 = frand(0.2f, 3), s2 = frand(0.2f, 3); for (int k = 0; k < 4; ++k) { a[k] *= s1; b[k] *= s2; } add("NO normalizados (norma 0.2..3)", a, b, 0, 1, frand(0, 1)); }
    for (int i = 0; i < 1000; ++i) { float a[4], b[4]; randUnitQuat(a); randUnitQuat(b); add("tiempos de clave IGUALES (t2 == t1)", a, b, 0.5f, 0.5f, 0.5f); }
    for (int i = 0; i < 1000; ++i) { float a[4], b[4]; randUnitQuat(a); randUnitQuat(b); add("tiempo fuera de [t1,t2] (extrapolacion)", a, b, 0, 1, frand(1.0f, 2.0f)); }
    for (int i = 0; i < 500; ++i) { float a[4] = { 0,0,0,0 }, b[4]; randUnitQuat(b); add("un cuaternion CERO", a, b, 0, 1, frand(0, 1)); }
    for (int i = 0; i < 500; ++i) { float a[4] = { 0,0,0,0 }, b[4] = { 0,0,0,0 }; add("dos cuaterniones CERO", a, b, 0, 1, frand(0, 1)); }
    for (int i = 0; i < 500; ++i) { float a[4]; randUnitQuat(a); add("un solo fotograma (clave1 == clave2)", a, a, 0, 1, frand(0, 1)); }
    return v;
}

// Flags de nodo validos: 2 = apilar, 1 = desapilar (solo con algo apilado), 0 = nada; se vacia la pila al final.
static void genFlags(int* fl, int N) {
    int depth = 0;
    for (int i = 0; i < N; ++i) { fl[i] = 0; int r = rnd() % 4; if (r == 1 && i < N - 4) { fl[i] = 2; depth++; } else if (r == 2 && depth > 0) { fl[i] = 1; depth--; } }
    // las pilas sobrantes quedan sin desapilar (inofensivo; un POP sin PUSH previo si seria un error de datos)
}

// ---------------- pruebas ----------------
static void testInterpVsLibrw() {
    auto pairs = makePairs();
    std::vector<std::string> cats; std::vector<long> tot, dif, nanOurs, nanLib; std::vector<double> mx;
    for (auto& p : pairs) {
        size_t ci = 0; for (; ci < cats.size(); ++ci) if (cats[ci] == p.cat) break;
        if (ci == cats.size()) { cats.push_back(p.cat); tot.push_back(0); dif.push_back(0); nanOurs.push_back(0); nanLib.push_back(0); mx.push_back(0); }
        float ours_in1[9], ours_in2[9]; memcpy(ours_in1, p.a, 36); memcpy(ours_in2, p.b, 36);
        float oo[9] = {}, lo[9] = {}; float om[16], lm[16];
        n_interp(oo, ours_in1, ours_in2, p.t);
        lib_interp(lo, p.a, p.b, p.t);
        // se comparan las MATRICES de rotacion resultantes (q y -q son la misma rotacion) y la traslacion
        n_apply(om, oo); lib_apply(lm, lo);
        bool bo = false, bl = false; for (int k = 0; k < 16; ++k) { if (k == 3 || k == 7 || k == 11 || k == 15) continue; bo |= bad(om[k]); bl |= bad(lm[k]); }
        tot[ci]++; if (bo) nanOurs[ci]++; if (bl) nanLib[ci]++;
        if (bo || bl) { if (bo != bl) dif[ci]++; continue; }
        double d = 0; for (int k = 0; k < 16; ++k) { if (k == 3 || k == 7 || k == 11 || k == 15) continue; d = std::fmax(d, std::fabs(om[k] - lm[k])); }
        if (d > mx[ci]) mx[ci] = d; if (d > 1e-4) dif[ci]++;
    }
    printf("\n== A. Interpolacion de fotogramas (nuestro HAnimKeyFrameInterpolateCpp frente a librw slerp/lerp; matrices, tolerancia 1e-4) ==\n");
    for (size_t i = 0; i < cats.size(); ++i) {
        char buf[200]; snprintf(buf, sizeof buf, "  %s", cats[i].c_str());
        char nota[120]; snprintf(nota, sizeof nota, "[basura/NaN: nuestro=%ld librw=%ld]", nanOurs[i], nanLib[i]);
        report(buf, tot[i], dif[i], mx[i], nota);
    }
}

static void testApplyVsLibrw() {
    long tot = 0, dif = 0; double mx = 0; long totNU = 0, difNU = 0; double mxNU = 0;
    for (int i = 0; i < 20000; ++i) {
        float f[9] = {}; randUnitQuat(f + 2); f[6] = frand(-5, 5); f[7] = frand(-5, 5); f[8] = frand(-5, 5);
        float om[16], lm[16]; n_apply(om, f); lib_apply(lm, f);
        double d = 0; for (int k = 0; k < 16; ++k) { if (k == 3 || k == 7 || k == 11 || k == 15) continue; d = std::fmax(d, std::fabs(om[k] - lm[k])); }
        tot++; mx = std::fmax(mx, d); if (d > 1e-5) dif++;
    }
    for (int i = 0; i < 5000; ++i) {
        float f[9] = {}; randUnitQuat(f + 2); float s = frand(0.3f, 2); for (int k = 2; k < 6; ++k) f[k] *= s; f[6] = f[7] = f[8] = 0;
        float om[16], lm[16]; n_apply(om, f); lib_apply(lm, f);
        double d = 0; for (int k = 0; k < 16; ++k) { if (k == 3 || k == 7 || k == 11 || k == 15) continue; d = std::fmax(d, std::fabs(om[k] - lm[k])); }
        totNU++; mxNU = std::fmax(mxNU, d); if (d > 1e-5) difNU++;
    }
    printf("\n== B. Cuaternion a matriz (nuestro HAnimKeyFrameApplyCpp frente a librw makeRotation) ==\n");
    report("  unitarios, tolerancia 1e-5", tot, dif, mx);
    report("  no unitarios (ambos usan 2*(a+b) sin normalizar), tolerancia 1e-5", totNU, difNU, mxNU);
}

// Control: una funcion con una lectura de pila sin inicializar de verdad; el metodo diferencial debe detectarla.
#pragma optimize("", off)
static __declspec(noinline) void controlUninit(float* out) { float local[4]; out[0] = local[0] + 1.0f; out[1] = 2.0f; }
#pragma optimize("", on)

template <size_t N, class F>
static long stackDiff(F fn, float* outFirst /*N floats de la primera pasada*/) {
    long diffs = 0; float ref[N] = {}; bool first = true;
    for (unsigned char fill : kFills) {
        float out[N]; memset(out, 0, sizeof out);
        smash(fill); fn(out);
        if (first) { memcpy(ref, out, sizeof out); first = false; }
        else if (memcmp(ref, out, sizeof out) != 0) diffs++;
    }
    if (outFirst) memcpy(outFirst, ref, sizeof ref);
    return diffs;
}

static void testStackFill() {
    printf("\n== C. Pila rellena con 00/CC/FF/5A/A5 antes de cada llamada: la salida debe ser identica bit a bit (sin lecturas sin inicializar) ==\n");
    { long d = 0; for (int i = 0; i < 20; ++i) d += stackDiff<2>([&](float* o) { controlUninit(o); }, nullptr);
      printf("  CONTROL (lectura sin inicializar a proposito): diferencias detectadas=%ld  %s\n", d, d ? "[el metodo funciona]" : "[ATENCION: el metodo no detecta nada con este compilador/opt]"); }
    auto pairs = makePairs();
    struct Cnt { const char* n; long tot = 0, dif = 0; } c[6] = { {"HAnimKeyFrameInterpolateCpp"}, {"HAnimKeyFrameBlendCpp"}, {"HAnimKeyFrameApplyCpp"}, {"HAnimKeyFrameAddCpp"}, {"HAnimKeyFrameMulRecipCpp"}, {"RpHAnimHierarchyUpdateMatrices (32 nodos)"} };
    for (size_t pi = 0; pi < pairs.size(); pi += 3) {
        const Pair& p = pairs[pi];
        { float a[9], b[9]; c[0].tot++; c[0].dif += stackDiff<9>([&](float* o) { memcpy(a, p.a, 36); memcpy(b, p.b, 36); memset(o, 0, 36); n_interp(o, a, b, p.t); }, nullptr) != 0; }
        { c[1].tot++; c[1].dif += stackDiff<9>([&](float* o) { float a[9], b[9]; memcpy(a, p.a, 36); memcpy(b, p.b, 36); memset(o, 0, 36); n_blend(o, a, b, std::fmod(std::fabs(p.t), 1.0f)); }, nullptr) != 0; }
        { c[2].tot++; c[2].dif += stackDiff<16>([&](float* o) { n_apply(o, p.a); }, nullptr) != 0; }
        { c[3].tot++; c[3].dif += stackDiff<9>([&](float* o) { memcpy(o, p.a, 36); n_add(o, p.a, p.b); }, nullptr) != 0; }
        { c[4].tot++; c[4].dif += stackDiff<9>([&](float* o) { memcpy(o, p.a, 36); n_mulrecip(o, p.b); }, nullptr) != 0; }
    }
    for (int it = 0; it < 300; ++it) {
        const int N = 32; int fl[N]; float fr[N * 9];
        genFlags(fl, N);
        for (int i = 0; i < N; ++i) { float* f = fr + i * 9; memset(f, 0, 36); randUnitQuat(f + 2); f[6] = frand(-1, 1); f[7] = frand(-1, 1); f[8] = frand(-1, 1); }
        c[5].tot++;
        long diffs = 0; std::vector<float> ref(N * 16); bool first = true;
        for (unsigned char fill : kFills) { std::vector<float> o(N * 16, 0.0f); smash(fill); n_update(N, fl, fr, o.data(), 0, 0); if (first) { ref = o; first = false; } else if (memcmp(ref.data(), o.data(), N * 64) != 0) diffs++; }
        c[5].dif += diffs != 0;
    }
    for (auto& x : c) { char b[200]; snprintf(b, sizeof b, "  %s", x.n); report(b, x.tot, x.dif, 0); if (x.dif) g_fail++; }
}

static void testUpdateVsLibrw() {
    printf("\n== D. RpHAnimHierarchyUpdateMatrices (nuestro, extraido de rphanim.cpp) frente a HAnimHierarchy::updateMatrices de librw ==\n");
    long tot = 0, dif = 0; double mx = 0; long totFull = 0;
    for (int it = 0; it < 2000; ++it) {
        const int N = 2 + rnd() % 62; std::vector<int> fl(N, 0); std::vector<float> fr(N * 9);
        genFlags(fl.data(), N);
        for (int i = 0; i < N; ++i) { float* f = &fr[i * 9]; memset(f, 0, 36); randUnitQuat(f + 2); f[6] = frand(-1, 1); f[7] = frand(-1, 1); f[8] = frand(-1, 1); }
        std::vector<float> om(N * 16), lm(N * 16);
        if (getenv("BANCO_DEBUG")) { printf("  it=%d N=%d flags:", it, N); for (int i = 0; i < N; ++i) printf("%d", fl[i]); printf("\n"); }
        if (seh_update(N, fl.data(), fr.data(), om.data()) != 0) { printf("   n_update fallo con it=%d\n", it); return; }
        lib_update(N, fl.data(), fr.data(), lm.data());
        double d = 0; for (int i = 0; i < N; ++i) for (int k = 0; k < 16; ++k) { if (k == 3 || k == 7 || k == 11 || k == 15) continue; d = std::fmax(d, std::fabs(om[i*16+k] - lm[i*16+k])); }
        tot++; mx = std::fmax(mx, d); if (!(d < 1e-3)) dif++;
    }
    (void)totFull;
    report("  jerarquias aleatorias con push/pop balanceado (tolerancia 1e-3)", tot, dif, mx);
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    printf("Banco oraculo librw-HAnim (x86). Compilado: %s\n", sizeof(void*) == 4 ? "32 bits" : "64 bits");
    testInterpVsLibrw();
    testApplyVsLibrw();
    testUpdateVsLibrw();
    testStackFill();
    printf("\nResultado del banco (C): %s\n", g_fail ? "HAY lecturas de pila sin inicializar o salidas dependientes de la pila" : "sin diferencias dependientes de la pila");
    return g_fail ? 1 : 0;
}
