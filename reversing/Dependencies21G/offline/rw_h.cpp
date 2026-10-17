// Arnes aislado: ejecuta los bytes REALES del EXE (reubicados) frente a la reconstruccion C++ de
// GetFirstTexture (0x734940) y RpGeometryReplaceOldMaterialWithNewMaterial (0x734DE0).
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <vector>
#include <random>
#include "exebytes.inc"

struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwTexture { void* raster; void* dict; RwLLLink lInDictionary; char name[32]; char mask[32]; uint32_t filterAddressing; int32_t refCount; };
struct RwTexDictionary { uint8_t object[8]; RwLLLink link; RwLLLink lInInstance; };
struct RpMaterial { void* texture; uint32_t color; void* pipeline; float surf[3]; int16_t refCount; int16_t pad; };
struct RpMaterialList { RpMaterial** materials; int32_t numMaterials; int32_t space; };
struct RpMesh { void* indices; uint32_t numIndices; RpMaterial* material; };
struct RpMeshHeader { uint32_t flags; uint16_t numMeshes; uint16_t serialNum; uint32_t totalIndicesInMesh; uint32_t firstMeshOffset; };
struct RpGeometry { uint8_t head[0x20]; RpMaterialList matList; uint8_t mid[0x54 - 0x2C]; RpMeshHeader* mesh; uint8_t tail[0x20]; };
static_assert(offsetof(RpGeometry, matList) == 0x20 && offsetof(RpGeometry, mesh) == 0x54 && offsetof(RpMaterial, refCount) == 0x18 && offsetof(RwTexture, lInDictionary) == 8);

// ---- reconstruccion C++ (identica a la del parche) ----
RwTexture* GetFirstTextureCpp(RwTexDictionary* txd) {
    auto* const sentinel = &txd->link;
    auto* const first = sentinel->next;
    if (first == sentinel) return nullptr;
    return reinterpret_cast<RwTexture*>(reinterpret_cast<uint8_t*>(first) - offsetof(RwTexture, lInDictionary));
}
static int g_trace[64]; static int g_nTrace;
static uint8_t* g_findEntry; static uint8_t* g_destroyEntry;
static int32_t __cdecl FindIdx(const RpMaterialList* l, const RpMaterial* m) { return ((int32_t(__cdecl*)(const RpMaterialList*, const RpMaterial*))g_findEntry)(l, m); }
static int32_t __cdecl DestroyMat(RpMaterial* m) { return ((int32_t(__cdecl*)(RpMaterial*))g_destroyEntry)(m); }
bool ReplaceCpp(RpGeometry* geometry, RpMaterial* oldMaterial, RpMaterial* newMaterial) {
    RpMeshHeader* const header = geometry->mesh;
    bool replaced = false;
    RpMesh* mesh = reinterpret_cast<RpMesh*>(header + 1);
    for (uint32_t count = header->numMeshes; count > 0; --count, ++mesh) {
        if (mesh->material != oldMaterial) continue;
        const int32_t index = FindIdx(&geometry->matList, mesh->material);
        DestroyMat(mesh->material);
        geometry->matList.materials[index] = newMaterial;
        mesh->material = newMaterial;
        ++newMaterial->refCount;
        replaced = true;
    }
    return replaced;
}

// ---- cargador de los bytes del EXE ----
static uint8_t* g_getFirst; static uint8_t* g_replace;
static uint8_t* Alloc(size_t n) { return (uint8_t*)VirtualAlloc(nullptr, n, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); }
static void PatchRel(uint8_t* at, uint8_t* target) { *(int32_t*)(at + 1) = (int32_t)(target - (at + 5)); }
static void Setup() {
    // GetFirstTexture: bloque [0x734930 callback][0x734940 funcion] y ForAllTextures (0x7F3730)
    uint8_t* blk = Alloc(0x1000);
    memcpy(blk, kGetFirst, sizeof(kGetFirst));             // 0x734930 -> blk
    uint8_t* forall = blk + 0x200; memcpy(forall, kForAllTex, sizeof(kForAllTex));
    uint8_t* fn = blk + 0x10;                               // 0x734940
    // fn+0x13: push 0x734930 ; fn+0x18: call 0x7f3730
    if (*(uint32_t*)(fn + 0x0b) != 0x734930) { /* localizar */ }
    int patched = 0;
    for (size_t i = 0; i + 5 <= 0x40; ++i) { if (blk[i] == 0x68 && *(uint32_t*)(blk + i + 1) == 0x734930) { *(uint32_t*)(blk + i + 1) = (uint32_t)blk; ++patched; } if (blk[i] == 0xE8 && (uint32_t)(0x734930 + i + 5 + *(int32_t*)(blk + i + 1)) == 0x7F3730) { PatchRel(blk + i, forall); ++patched; } }
    g_getFirst = fn;
    // Replace: [0x734DE0], FindIdx (0x74E420), Destroy (0x74DA20)
    uint8_t* r = Alloc(0x1000);
    memcpy(r, kReplace, sizeof(kReplace));
    g_findEntry = r + 0x200; memcpy(g_findEntry, kFindIdx, sizeof(kFindIdx));
    g_destroyEntry = r + 0x300; memcpy(g_destroyEntry, kMatDestroy, sizeof(kMatDestroy));
    for (size_t i = 0; i + 5 <= sizeof(kReplace); ++i) if (r[i] == 0xE8) { uint32_t tgt = 0x734DE0 + i + 5 + *(int32_t*)(r + i + 1); if (tgt == 0x74E420) { PatchRel(r + i, g_findEntry); ++patched; } else if (tgt == 0x74DA20) { PatchRel(r + i, g_destroyEntry); ++patched; } }
    g_replace = r;
    printf("saltos reubicados: %d\n", patched);
}

struct Regs { uint32_t eax, ebx, esi, edi, ebp, esp0, esp1; };
static Regs g_regs;
static bool g_nvGet;
static uint8_t* s_fn; static void *s_a1, *s_a2, *s_a3; static uint32_t s_res, s_ok;
static uint32_t __declspec(noinline) CallExeGetFirst(void* txd) {
    s_fn = g_getFirst; s_a1 = txd; s_ok = 1;
    __asm {
        push ebx
        push esi
        push edi
        push ebp
        mov ebx, 0x11111111
        mov esi, 0x22222222
        mov edi, 0x33333333
        mov ebp, 0x44444444
        push s_a1
        call s_fn
        add esp, 4
        mov s_res, eax
        cmp ebx, 0x11111111
        jne bad
        cmp esi, 0x22222222
        jne bad
        cmp edi, 0x33333333
        jne bad
        cmp ebp, 0x44444444
        jne bad
        jmp ok
    bad:
        mov s_ok, 0
    ok:
        pop ebp
        pop edi
        pop esi
        pop ebx
    }
    g_nvGet = s_ok != 0;
    return s_res;
}
static bool __declspec(noinline) CallExeReplace(void* g, void* o, void* n, bool& nonvol) {
    s_fn = g_replace; s_a1 = g; s_a2 = o; s_a3 = n; s_ok = 1;
    __asm {
        push ebx
        push esi
        push edi
        push ebp
        mov ebx, 0x11111111
        mov esi, 0x22222222
        mov edi, 0x33333333
        mov ebp, 0x44444444
        push s_a3
        push s_a2
        push s_a1
        call s_fn
        add esp, 12
        mov s_res, eax
        cmp ebx, 0x11111111
        jne bad
        cmp esi, 0x22222222
        jne bad
        cmp edi, 0x33333333
        jne bad
        cmp ebp, 0x44444444
        jne bad
        jmp ok
    bad:
        mov s_ok, 0
    ok:
        pop ebp
        pop edi
        pop esi
        pop ebx
    }
    nonvol = s_ok != 0;
    return (s_res & 0xFF) != 0;
}

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    Setup();
    std::mt19937 rng(777);
    long N = argc > 1 ? atol(argv[1]) : 100000;
    // ---------- GetFirstTexture ----------
    long gBad = 0;
    for (long i = 0; i < N; ++i) {
        int n = rng() % 8;
        std::vector<uint8_t> arenaA(0x1000), arenaB;
        for (auto& b : arenaA) b = (uint8_t)rng();
        auto* txd = (RwTexDictionary*)(arenaA.data() + 0x10);
        std::vector<RwTexture*> tex;
        for (int k = 0; k < n; ++k) tex.push_back((RwTexture*)(arenaA.data() + 0x100 + k * 0x80));
        auto* sent = &txd->link;
        RwLLLink* prev = sent;
        for (auto* t : tex) { prev->next = &t->lInDictionary; t->lInDictionary.prev = prev; prev = &t->lInDictionary; }
        prev->next = sent; sent->prev = prev;
        if (n == 0) { sent->next = sent; sent->prev = sent; }
        arenaB = arenaA;   // las copias en otra direccion: comparar offsets
        uint32_t exe = CallExeGetFirst(txd);
        RwTexture* cpp = GetFirstTextureCpp(txd);
        bool same = (exe == (uint32_t)cpp) && arenaA == arenaB && g_nvGet;
        if (!same) { if (gBad++ < 5) printf("GetFirstTexture DIF i=%ld n=%d exe=%08x cpp=%p\n", i, n, exe, (void*)cpp); }
    }
    printf("GetFirstTexture: casos=%ld difs=%ld\n", N, gBad);
    // ---------- Replace ----------
    long rBad = 0, replacedCases = 0;
    for (long i = 0; i < N; ++i) {
        // dos arenas identicas en direcciones distintas (offsets iguales); se normalizan punteros comparando por offset
        const size_t SZ = 0x1000;
        std::vector<uint8_t> A(SZ + 0x100), B(SZ + 0x100);
        auto build = [&](std::vector<uint8_t>& arena, uint32_t seed, RpGeometry*& geo, RpMaterial*& oldM, RpMaterial*& newM) {
            std::mt19937 r(seed);
            for (size_t q = 0; q < arena.size(); ++q) arena[q] = (q % 4 == 3) ? 0xA5 : (uint8_t)r();
            uint8_t* base = arena.data() + 0x40;
            geo = (RpGeometry*)base;                                   // 0x00..0x7F
            int nMat = 1 + r() % 6, nMesh = r() % 7;
            RpMaterial** list = (RpMaterial**)(base + 0x200);          // lista de materiales (con 16 bytes de relleno antes)
            std::vector<RpMaterial*> mats;
            for (int k = 0; k < 10; ++k) { auto* m = (RpMaterial*)(base + 0x400 + k * 0x40); m->refCount = (int16_t)(2 + r() % 5); mats.push_back(m); }
            if (r() % 9 == 0) mats[1]->refCount = (int16_t)0x7FFF; if (r() % 9 == 0) mats[2]->refCount = (int16_t)-1; if (r() % 11 == 0) mats[3]->refCount = 1;
            geo->matList.materials = list; geo->matList.numMaterials = nMat; geo->matList.space = nMat;
            for (int k = 0; k < nMat; ++k) list[k] = mats[r() % 5];
            auto* hdr = (RpMeshHeader*)(base + 0x800); geo->mesh = hdr;
            hdr->numMeshes = (uint16_t)nMesh; hdr->firstMeshOffset = (r() % 3 == 0) ? 0x30 : 0;   // el EXE lo ignora
            auto* meshes = (RpMesh*)(hdr + 1);
            for (int k = 0; k < nMesh; ++k) { meshes[k].indices = (void*)(uintptr_t)(r() | 0xA5000000u); meshes[k].numIndices &= 0x0000FFFF; meshes[k].numIndices = r() & 0xFFFF; meshes[k].material = mats[r() % 7]; }
            oldM = mats[r() % 7];
            switch (r() % 5) { case 0: newM = oldM; break; case 1: newM = mats[7]; break; case 2: newM = mats[8]; break; default: newM = mats[r() % 10]; }
            // el material viejo debe tener refCount >= 2 (el camino de destruccion real libera memoria del motor)
            for (auto* m : mats) { if (m == oldM && m->refCount < 2 && nMesh) m->refCount = 2; }
            if (r() % 13 == 0 && nMesh) { list[0] = mats[9]; }
            // el viejo debe estar en la lista salvo en algunos casos (indice -1 escribe en list[-1])
        };
        RpGeometry *gA, *gB; RpMaterial *oA, *oB, *nA, *nB;
        uint32_t seed = (uint32_t)rng();
        build(A, seed, gA, oA, nA); build(B, seed, gB, oB, nB);
        bool inList = false; for (int k = 0; k < gA->matList.numMaterials; ++k) if (gA->matList.materials[k] == oA) inList = true;
        // para que la destruccion de refCount==1 no libere nada, se exige refCount>=2 en cada malla que coincida
        bool safe = true; int matches = 0;
        { auto* hdr = gA->mesh; auto* ms = (RpMesh*)(hdr + 1); for (int k = 0; k < hdr->numMeshes; ++k) if (ms[k].material == oA) { ++matches; } }
        if (matches && (int)oA->refCount < matches + 1) { oA->refCount = (int16_t)(matches + 1); oB->refCount = oA->refCount; }
        if (!safe) { continue; }
        if (!inList) { bool anyMatch = false; { auto* hdr = gA->mesh; auto* ms = (RpMesh*)(hdr + 1); for (int k = 0; k < hdr->numMeshes; ++k) if (ms[k].material == oA) anyMatch = true; } (void)anyMatch; }
        bool nvA = false;
        bool rA = CallExeReplace(gA, oA, nA, nvA);
        bool rB = ReplaceCpp(gB, oB, nB);
        // normalizar punteros: restar la base de cada arena
        auto norm = [&](std::vector<uint8_t>& arena) { std::vector<uint8_t> c = arena; uint32_t lo = (uint32_t)arena.data(), hi = lo + (uint32_t)arena.size(); for (size_t k = 0; k + 4 <= c.size(); ++k) { uint32_t v = *(uint32_t*)&c[k]; if (v >= lo && v < hi) {} } return c; };
        // comparacion por offsets: reemplazar punteros dentro de la arena por (puntero - base)
        auto rel = [&](std::vector<uint8_t>& arena) { std::vector<uint8_t> c = arena; uint32_t lo = (uint32_t)arena.data(); for (size_t k = 0; k + 4 <= c.size(); k += 4) { uint32_t v = *(uint32_t*)&c[k]; if (v >= lo && v < lo + arena.size()) *(uint32_t*)&c[k] = v - lo; } return c; };
        (void)norm;
        bool same = rA == rB && nvA && rel(A) == rel(B);
        if (rA) ++replacedCases;
        if (!same && rBad < 3) { auto ra = rel(A), rb = rel(B); for (size_t k = 0; k < ra.size(); ++k) if (ra[k] != rb[k]) { printf("  off=%zx exe=%02x cpp=%02x\n", k, ra[k], rb[k]); } }
        if (!same) { if (rBad++ < 6) printf("Replace DIF i=%ld exeRet=%d cppRet=%d nonvol=%d memEq=%d inList=%d\n", i, rA, rB, nvA, rel(A) == rel(B), inList); }
    }
    printf("Replace: casos=%ld con_reemplazo=%ld difs=%ld\n", N, replacedCases, rBad);
    return 0;
}
