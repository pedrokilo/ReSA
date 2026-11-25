// Banco RwHelper (sustitucion CRT/RenderWare/x87): pares EXE original vs C++ propio, fuera del juego.
//   0x734970 y 0x734ED0 (callbacks de RpClumpGetBoundingSphere), 0x734FC0 (RpClumpGetBoundingSphere) y 0x730E50 (TexDictDestroyTextureCallback).
// El codigo C++ que se compara es el REAL de source/game_sa/RwHelper.cpp y rwtexdict.cpp: gen_slice_rwhelper.py lo extrae a un .inc que se incluye aqui tal cual
// (compilado con las opciones del proyecto en Debug: /Od, SSE2, /RTC1), junto con X87Math.cpp compilado con sus opciones por archivo (/O2 /Oi /arch:IA32 /fp:fast).
// El gta_sa.exe original (solo lectura) se mapea en 0x400000 y se llama por su direccion (mismo metodo que bench_p3). Los colaboradores (resincronizar la esfera, LTM del marco,
// RwV3dTransformPoints, RwMatrixInvert, RpClumpGetNumAtomics, RpClumpForAllAtomics, RwTextureDestroy) son los mismos falsos deterministas en los dos lados: en la copia mapeada
// del EXE se parchean las llamadas a sus direcciones (0x7491F0, 0x7F0990, 0x7EDD90, 0x7F2070, 0x7498E0, 0x749B70, 0x7F3820). Cada falso registra su llamada en una traza.
// Por caso: copia identica de la escena para cada lado; se comparan el valor devuelto, todos los bytes de la escena, la traza de colaboradores y el global 0x8D60BC.
// La x87 va a 24 bits (precision que fija el juego). Uso: bench_rwhelper.exe <ruta del EXE> [--casos N] [--semilla S] [--solo cb1|cb2|sphere|tex]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <bit>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

using uint8 = uint8_t;
using uint32 = uint32_t;
using int32 = int32_t;
using RwInt32 = int32_t;
using RwReal = float;
using RwBool = int32_t;
#define UNUSED(x) (void)(x)

// ---- tipos minimos con la disposicion del EXE (desplazamientos comprobados en el desensamblado de 0x734970 / 0x734ED0 / 0x734FC0) ----
struct RwV3d { float x, y, z; };
struct RwSphere { RwV3d center; float radius; };
struct RwMatrix { RwV3d right; uint32_t flags; RwV3d up; uint32_t pad1; RwV3d at; uint32_t pad2; RwV3d pos; uint32_t pad3; };
struct RwObject { uint8 type, subType, flags, privateFlags; void* parent; };
struct RwFrame { RwObject object; uint32_t link[2]; RwMatrix modelling; RwMatrix ltm; uint32_t tail[4]; };
struct RpClump;
struct RpAtomic {
    uint8 pad0[0x1C];
    RwSphere boundingSphere;       // +0x1C centro, +0x28 radio
    uint8 pad1[0x3C - 0x2C];
    RpClump* clump;                // +0x3C
    uint8 pad2[0x4C - 0x40];
    struct { uint8 flags; uint8 pad[3]; } interpolator; // +0x4C
    uint8 pad3[0x20];
};
struct RpClump { RwObject object; /* +4 parent (marco) */ int32_t numAtomics; RpAtomic* atomics[8]; };
struct RwTexture { int32_t id; };
typedef RpAtomic* (*RpAtomicCallBack)(RpAtomic*, void*);
static_assert(offsetof(RpAtomic, boundingSphere) == 0x1C && offsetof(RpAtomic, clump) == 0x3C && offsetof(RpAtomic, interpolator) == 0x4C, "disposicion de RpAtomic");
static_assert(offsetof(RwFrame, modelling) == 0x10 && offsetof(RwObject, parent) == 4, "disposicion de RwFrame/RwObject");

namespace DatosExe::Data { inline uint8_t& v8D60BC = *reinterpret_cast<uint8_t*>(0x8D60BC); }

// ---- falsos deterministas (mismos en los dos lados) y traza ----
static std::vector<uint64_t> g_trace;
static void tr(uint64_t a, uint64_t b = 0, uint64_t c = 0, uint64_t d = 0) {
    g_trace.push_back(a); g_trace.push_back(b); g_trace.push_back(c); g_trace.push_back(d);
}
// Los punteros se registran como desplazamiento respecto a la base de la escena activa (cada lado tiene su copia).
static uint8_t* g_base = nullptr;
// Los punteros a la pila del codigo bajo prueba (locales distintos en el EXE y en el C++) se anotan como "pila"; los de la escena, por su desplazamiento.
static uint64_t off(const void* p) {
    if (!p) return 0xFFFFFFFFull;
    const uint8_t* q = (const uint8_t*)p;
    return (q >= g_base && q < g_base + 0x800) ? (uint64_t)(q - g_base) : 0xFFFFFFFEull;
}
static uint32_t fb(float v) { uint32_t b; memcpy(&b, &v, 4); return b; }

extern "C" {
RpAtomic* __cdecl _rpAtomicResyncInterpolatedSphere(RpAtomic* a) {
    tr(1, off(a), fb(a->boundingSphere.center.x), a->interpolator.flags);
    a->boundingSphere.center.x = a->boundingSphere.center.x * 1.0009765625f + 0.125f;
    a->boundingSphere.radius += 0.25f;
    if (fb(a->boundingSphere.center.y) & 1) a->interpolator.flags &= ~2; // a veces limpia el indicador (se relee de memoria en el codigo bajo prueba)
    return a;
}
RwMatrix* __cdecl RwFrameGetLTM(RwFrame* f) { tr(2, off(f)); return &f->ltm; }
RwV3d* __cdecl RwV3dTransformPoints(RwV3d* out, const RwV3d* in, RwInt32 n, const RwMatrix* m) {
    tr(3, off(out), off(in), (uint64_t)n << 32 | off(m));
    for (RwInt32 i = 0; i < n; i++) {
        const RwV3d p = in[i];
        out[i].x = p.x * m->right.x + p.y * m->up.x + p.z * m->at.x + m->pos.x;
        out[i].y = p.x * m->right.y + p.y * m->up.y + p.z * m->at.y + m->pos.y;
        out[i].z = p.x * m->right.z + p.y * m->up.z + p.z * m->at.z + m->pos.z;
    }
    return out;
}
RwMatrix* __cdecl RwMatrixInvert(RwMatrix* inv, const RwMatrix* m) {
    tr(4, off(inv), off(m));
    inv->right = { m->right.x, m->up.x, m->at.x }; inv->up = { m->right.y, m->up.y, m->at.y }; inv->at = { m->right.z, m->up.z, m->at.z };
    inv->pos = { -(m->pos.x * m->right.x + m->pos.y * m->right.y + m->pos.z * m->right.z), -(m->pos.x * m->up.x + m->pos.y * m->up.y + m->pos.z * m->up.z), -(m->pos.x * m->at.x + m->pos.y * m->at.y + m->pos.z * m->at.z) };
    inv->flags = inv->pad1 = inv->pad2 = inv->pad3 = 0;
    return inv;
}
RwInt32 __cdecl RpClumpGetNumAtomics(RpClump* c) { tr(5, off(c)); return c->numAtomics; }
RpClump* __cdecl RpClumpForAllAtomics(RpClump* c, RpAtomicCallBack cb, void* data) {
    tr(6, off(c), off(data));
    for (int32_t i = 0; i < c->numAtomics; i++) {
        if (!cb(c->atomics[i], data)) break;
    }
    return c;
}
RwTexture* __cdecl RwTextureDestroy(RwTexture* t) { tr(7, off(t)); return t; }
}

#include "X87Math.h"
#include "slice.inc"
// Los callbacks son `static` en la rama real; el banco toma sus direcciones aqui.
static RpAtomicCallBack g_cb1 = RpClumpBoundingSphereCenterCallback;
static RpAtomicCallBack g_cb2 = RpClumpBoundingSphereRadiusCallback;

// ---- carga del EXE y parches de llamadas ----
static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    return true;
}
static void patch_call(uintptr_t at, void* target) { const int32_t rel = (int32_t)((uintptr_t)target - (at + 5)); memcpy((void*)(at + 1), &rel, 4); }
static int patch_range(uintptr_t lo, uintptr_t hi) {
    struct M { uintptr_t addr; void* fn; } map[] = {
        {0x7491F0, (void*)_rpAtomicResyncInterpolatedSphere}, {0x7F0990, (void*)RwFrameGetLTM}, {0x7EDD90, (void*)RwV3dTransformPoints}, {0x7F2070, (void*)RwMatrixInvert},
        {0x7498E0, (void*)RpClumpGetNumAtomics}, {0x749B70, (void*)RpClumpForAllAtomics}, {0x7F3820, (void*)RwTextureDestroy}};
    int n = 0;
    // se recorre por instrucciones (capstone no esta aqui): las tres funciones conocidas solo tienen `call rel32` (E8) a esas direcciones; se aceptan solo los aciertos exactos
    for (uintptr_t p = lo; p < hi; p++) {
        if (*(uint8_t*)p != 0xE8) continue;
        const uintptr_t tgt = p + 5 + *(int32_t*)(p + 1);
        for (auto& m : map) if (tgt == m.addr) { patch_call(p, m.fn); n++; }
    }
    return n;
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static float bitsf(uint32_t b) { float x; memcpy(&x, &b, 4); return x; }
static void set_pc24() { unsigned c; _controlfp_s(&c, _PC_24, _MCW_PC); }

// float variado: mayormente realista; a veces cero con signo, NaN, infinito, subnormal
static float rfloat() {
    const uint32_t k = rnd(40);
    if (k == 0) return 0.0f; if (k == 1) return -0.0f; if (k == 2) return bitsf(0x7FC00000); if (k == 3) return bitsf(0x7F800000); if (k == 4) return bitsf(0xFF800000);
    if (k == 5) return bitsf((uint32_t)rng() & 0x807FFFFF);
    if (k == 6) return 1.0f;
    const int e = (int)rnd(40) - 12;
    return bitsf(((uint32_t)(127 + e) << 23) | ((uint32_t)rng() & 0x7FFFFF) | ((uint32_t)(rng() & 1) << 31));
}

// Escena: [clump][marco del clump][N atomos]; los atomos apuntan al clump. Bytes de relleno aleatorios (se comparan enteros).
struct Scene {
    alignas(16) uint8_t mem[0x800];
    RpClump* clump() { return (RpClump*)(mem + 0x000); }
    RwFrame* frame() { return (RwFrame*)(mem + 0x100); }
    RpAtomic* atom(int i) { return (RpAtomic*)(mem + 0x300 + i * 0x80); }
    RwSphere* sphere() { return (RwSphere*)(mem + 0x700); }
    RwV3d* sum() { return (RwV3d*)(mem + 0x720); }
    RwTexture* tex() { return (RwTexture*)(mem + 0x740); }
};
static void make_scene(Scene& s, int natoms) {
    for (auto& b : s.mem) b = (uint8_t)rng();
    RpClump* c = s.clump(); c->object.parent = s.frame(); c->numAtomics = natoms;
    for (int i = 0; i < 8; i++) c->atomics[i] = i < natoms ? s.atom(i) : nullptr;
    float* fm = (float*)&s.frame()->modelling;
    for (int i = 0; i < 32; i++) fm[i] = rfloat();
    for (int i = 0; i < natoms; i++) {
        RpAtomic* a = s.atom(i); a->clump = c;
        a->boundingSphere.center = { rfloat(), rfloat(), rfloat() }; a->boundingSphere.radius = rfloat();
        a->interpolator.flags = (uint8_t)(rnd(2) ? (rng() & 0xFF) | 2 : (rng() & 0xFD));
    }
    RwSphere* sp = s.sphere(); sp->center = { rfloat(), rfloat(), rfloat() }; sp->radius = rfloat();
    RwV3d* su = s.sum(); *su = { rfloat(), rfloat(), rfloat() };
}

static uint64_t hash_trace() { uint64_t h = 1469598103934665603ull; for (auto v : g_trace) { h ^= v; h *= 1099511628211ull; } return h; }

enum Kind { CB1, CB2, SPHERE, TEX };
static uint64_t run_case(Kind kind, uint64_t n, uint64_t& counts) {
    set_pc24();
    uint64_t fails = 0;
    for (uint64_t it = 0; it < n; it++) {
        const int natoms = 1 + (int)rnd(5);
        static Scene live; // ubicacion fija: los punteros internos son los mismos en las dos pasadas
        Scene base; make_scene(base, natoms);
        Scene a, b;
        auto reset = [&]() {
            live = base;
            RpClump* c = live.clump(); c->object.parent = live.frame();
            for (int i = 0; i < 8; i++) c->atomics[i] = i < natoms ? live.atom(i) : nullptr;
            for (int i = 0; i < natoms; i++) live.atom(i)->clump = c;
        };
        reset();
        const uint8_t useLtmVal = (uint8_t)(rnd(3) == 0 ? 0 : (rnd(2) ? 1 : (uint8_t)(rng() | 1)));
        const int which = (int)rnd(natoms);
        const bool useLtmArg = rnd(2) != 0;
        const int variant = rnd(10) == 0 ? 1 + (int)rnd(2) : 0; // 0 normal, 1 clump nulo, 2 esfera nula
        const uint8_t savedGlobal = *(uint8_t*)0x8D60BC;
        uint64_t exeRet = 0, ourRet = 0; uint64_t exeHash, ourHash; uint8_t exeG, ourG; std::vector<uint64_t> exeTrace, ourTrace;
        auto call = [&](Scene& s, bool exe) -> uint64_t {
            g_base = s.mem; g_trace.clear();
            *(uint8_t*)0x8D60BC = useLtmVal;
            uint64_t r = 0;
            switch (kind) {
            case CB1: { RpAtomicCallBack f = exe ? (RpAtomicCallBack)0x734970 : g_cb1; r = off(f(s.atom(which), s.sum())); break; }
            case CB2: { RpAtomicCallBack f = exe ? (RpAtomicCallBack)0x734ED0 : g_cb2; r = off(f(s.atom(which), s.sphere())); break; }
            case SPHERE: {
                typedef RpClump*(__cdecl* SFn)(RpClump*, RwSphere*, bool);
                SFn f = exe ? (SFn)0x734FC0 : (SFn)RpClumpGetBoundingSphere;
                bool ba; const uint8_t arg = (uint8_t)(useLtmArg ? 1 : 0); memcpy(&ba, &arg, 1);
                RpClump* clump = variant == 1 ? nullptr : s.clump();
                RwSphere* sph = variant == 2 ? nullptr : s.sphere();
                r = off(f(clump, sph, ba));
                break; }
            case TEX: { typedef RwTexture*(__cdecl* TFn)(RwTexture*, void*); TFn f = exe ? (TFn)0x730E50 : (TFn)TexDictDestroyTextureCallback; r = off(f(s.tex(), s.sum())); break; }
            }
            return r;
        };
        exeRet = call(live, true); exeTrace = g_trace; exeHash = hash_trace(); exeG = *(uint8_t*)0x8D60BC; a = live;
        reset();
        ourRet = call(live, false); ourTrace = g_trace; ourHash = hash_trace(); ourG = *(uint8_t*)0x8D60BC; b = live;
        *(uint8_t*)0x8D60BC = savedGlobal;
        counts++;
        const bool same = exeRet == ourRet && exeHash == ourHash && exeG == ourG && memcmp(a.mem, b.mem, sizeof a.mem) == 0;
        if (!same && fails++ < 8) {
            printf("DIFERENCIA caso %llu (atomos=%d global=%u): ret exe=%llx ours=%llx traza exe=%016llx ours=%016llx global exe=%u ours=%u", (unsigned long long)it, natoms, useLtmVal, (unsigned long long)exeRet, (unsigned long long)ourRet,
                   (unsigned long long)exeHash, (unsigned long long)ourHash, exeG, ourG);
            for (size_t i = 0; i < sizeof a.mem; i++) if (a.mem[i] != b.mem[i]) { printf(" primer byte distinto +0x%zx (exe %02x, ours %02x)", i, a.mem[i], b.mem[i]); break; }
            for (size_t i = 0; i < std::max(exeTrace.size(), ourTrace.size()); i += 4) {
                const bool eq = i + 3 < exeTrace.size() && i + 3 < ourTrace.size() && !memcmp(&exeTrace[i], &ourTrace[i], 32);
                if (eq) continue;
                printf("\n   traza[%zu] exe=", i / 4);
                if (i < exeTrace.size()) printf("(%llu,%llx,%llx,%llx)", (unsigned long long)exeTrace[i], (unsigned long long)exeTrace[i + 1], (unsigned long long)exeTrace[i + 2], (unsigned long long)exeTrace[i + 3]); else printf("fin");
                printf(" ours=");
                if (i < ourTrace.size()) printf("(%llu,%llx,%llx,%llx)", (unsigned long long)ourTrace[i], (unsigned long long)ourTrace[i + 1], (unsigned long long)ourTrace[i + 2], (unsigned long long)ourTrace[i + 3]); else printf("fin");
                break;
            }
            printf("\n");
        }
    }
    return fails;
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_rwhelper.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo cb1|cb2|sphere|tex]\n"); return 2; }
    setvbuf(stdout, nullptr, _IONBF, 0);
    uint64_t n = 200000, seed = 12345; std::string only;
    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10);
        else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10);
        else if (!strcmp(argv[i], "--solo") && i + 1 < argc) only = argv[++i];
    }
    rng.seed(seed);
    if (!map_exe(argv[1])) return 2;
    const int p1 = patch_range(0x734970, 0x734A03), p2 = patch_range(0x734ED0, 0x734FB4), p3 = patch_range(0x734FC0, 0x735140), p4 = patch_range(0x730E50, 0x730E60);
    printf("llamadas parcheadas: cb1=%d (esperadas 5) cb2=%d (esperadas 6) esfera=%d (esperadas 7) tex=%d (esperadas 1)\n", p1, p2, p3, p4);
    uint64_t total = 0, c;
    struct { const char* name; Kind k; } items[] = {{"cb1", CB1}, {"cb2", CB2}, {"sphere", SPHERE}, {"tex", TEX}};
    for (auto& it : items) {
        if (!only.empty() && only != it.name) continue;
        c = 0; const uint64_t f = run_case(it.k, n, c);
        printf("%s: %llu casos, %llu diferencias\n", it.name, (unsigned long long)c, (unsigned long long)f);
        total += f;
    }
    printf(total ? "RESULTADO: HAY DIFERENCIAS (%llu)\n" : "RESULTADO: SIN DIFERENCIAS\n", (unsigned long long)total);
    return total ? 1 : 0;
}
