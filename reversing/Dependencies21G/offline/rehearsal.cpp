// Ensayo del banco: mismas fixtures y arenas que reversing/Dependencies21G, pero ejecutando los bytes reales del EXE (reubicados)
// frente a la reconstruccion C++ en un proceso aparte (sin el juego). Colaboradores con los mismos stubs trazados del banco.
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <vector>
#include <string>
#include "../Arenas.hpp"
#include "exebytes.inc"
using namespace Dependencies21GFix;

struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwTexture { void* raster; void* dict; RwLLLink lInDictionary; char name[32]; char mask[32]; uint32_t filterAddressing; int32_t refCount; };
struct RwTexDictionary { uint8_t object[8]; RwLLLink link; RwLLLink lInInstance; };
struct RpMaterial { void* texture; uint32_t color; void* pipeline; float surf[3]; int16_t refCount; int16_t pad; };
struct RpMaterialList { RpMaterial** materials; int32_t numMaterials; int32_t space; };
struct RpMesh { void* indices; uint32_t numIndices; RpMaterial* material; };
struct RpMeshHeader { uint32_t flags; uint16_t numMeshes; uint16_t serialNum; uint32_t totalIndicesInMesh; uint32_t firstMeshOffset; };
struct RpGeometry { uint8_t head[0x20]; RpMaterialList matList; uint8_t mid[0x54 - 0x2C]; RpMeshHeader* mesh; uint8_t tail[0x20]; };

// ---- candidato C++ (identico al del parche) ----
RwTexture* GetFirstTextureCpp(RwTexDictionary* txd) {
    auto* const sentinel = &txd->link;
    auto* const first = sentinel->next;
    if (first == sentinel) return nullptr;
    return reinterpret_cast<RwTexture*>(reinterpret_cast<uint8_t*>(first) - offsetof(RwTexture, lInDictionary));
}
static std::vector<uint32_t> g_trace; static const uint8_t* g_base;
static int32_t __cdecl FindStub(const RpMaterialList* l, const RpMaterial* m) {
    g_trace.push_back(1); g_trace.push_back((uint32_t)((const uint8_t*)l - g_base)); g_trace.push_back((uint32_t)((const uint8_t*)m - g_base));
    int32_t n = l->numMaterials; if (n <= 0) return n - 1;
    for (int32_t i = n - 1; i >= 0; --i) if (l->materials[i] == m) return i;
    return -1;
}
static int32_t __cdecl DestroyStub(RpMaterial* m) {
    g_trace.push_back(2); g_trace.push_back((uint32_t)((const uint8_t*)m - g_base)); g_trace.push_back(0);
    if ((uint16_t)m->refCount != 1) m->refCount = (int16_t)(m->refCount - 1);
    return 1;
}
bool ReplaceCpp(RpGeometry* geometry, RpMaterial* oldMaterial, RpMaterial* newMaterial) {
    RpMeshHeader* const header = geometry->mesh;
    bool replaced = false;
    RpMesh* mesh = reinterpret_cast<RpMesh*>(header + 1);
    for (uint32_t count = header->numMeshes; count > 0; --count, ++mesh) {
        if (mesh->material != oldMaterial) continue;
        const int32_t index = FindStub(&geometry->matList, mesh->material);
        DestroyStub(mesh->material);
        geometry->matList.materials[index] = newMaterial;
        mesh->material = newMaterial;
        ++newMaterial->refCount;
        replaced = true;
    }
    return replaced;
}

static uint8_t* Alloc(size_t n) { return (uint8_t*)VirtualAlloc(nullptr, n, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); }
static void PatchRel(uint8_t* at, const void* target) { *(int32_t*)(at + 1) = (int32_t)((const uint8_t*)target - (at + 5)); }
static uint8_t *g_getFirst, *g_replace;
static void Setup() {
    uint8_t* blk = Alloc(0x1000); memcpy(blk, kGetFirst, sizeof(kGetFirst));
    uint8_t* forall = blk + 0x200; memcpy(forall, kForAllTex, sizeof(kForAllTex));
    for (size_t i = 0; i + 5 <= 0x40; ++i) {
        if (blk[i] == 0x68 && *(uint32_t*)(blk + i + 1) == 0x734930) *(uint32_t*)(blk + i + 1) = (uint32_t)blk;
        if (blk[i] == 0xE8 && (uint32_t)(0x734930 + i + 5 + *(int32_t*)(blk + i + 1)) == 0x7F3730) PatchRel(blk + i, forall);
    }
    g_getFirst = blk + 0x10;
    uint8_t* r = Alloc(0x1000); memcpy(r, kReplace, sizeof(kReplace));
    int n = 0;
    for (size_t i = 0; i + 5 <= sizeof(kReplace); ++i) if (r[i] == 0xE8) { uint32_t t = 0x734DE0 + i + 5 + *(int32_t*)(r + i + 1); if (t == 0x74E420) { PatchRel(r + i, (const void*)&FindStub); ++n; } else if (t == 0x74DA20) { PatchRel(r + i, (const void*)&DestroyStub); ++n; } }
    g_replace = r; printf("llamadas a colaboradores redirigidas a stubs: %d\n", n);
}
static uint8_t* s_fn; static void *s_a1, *s_a2, *s_a3; static uint32_t s_res, s_ok;
static uint32_t __declspec(noinline) CallExe1(void* a1, bool& nv) {
    s_fn = g_getFirst; s_a1 = a1; s_ok = 1;
    __asm { push ebx
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
    bad: mov s_ok, 0
    ok: pop ebp
        pop edi
        pop esi
        pop ebx }
    nv = s_ok != 0; return s_res;
}
static uint32_t __declspec(noinline) CallExe3(void* a1, void* a2, void* a3, bool& nv) {
    s_fn = g_replace; s_a1 = a1; s_a2 = a2; s_a3 = a3; s_ok = 1;
    __asm { push ebx
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
    bad: mov s_ok, 0
    ok: pop ebp
        pop edi
        pop esi
        pop ebx }
    nv = s_ok != 0; return s_res;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    FILE* dump = fopen("rehearsal.out", "w");
    Setup();
    alignas(16) static uint8_t arena[0x800];
    long bad = 0, withFirst = 0;
    // ---- GetFirstTexture: 1024 casos del banco ----
    for (unsigned c = 0; c < kFirstTextureCases; ++c) {
        auto fx = FirstTexture(c);
        BuildFirstTexture(arena, fx); std::vector<uint8_t> init(arena, arena + kTexArena);
        bool nv; uint32_t e = CallExe1(arena + DictOffset(fx), nv); std::vector<uint8_t> afterE(arena, arena + kTexArena);
        BuildFirstTexture(arena, fx);
        uint32_t k = (uint32_t)GetFirstTextureCpp((RwTexDictionary*)(arena + DictOffset(fx))); std::vector<uint8_t> afterC(arena, arena + kTexArena);
        long expect = ExpectedFirstTexture(fx);
        uint32_t want = expect < 0 ? 0 : (uint32_t)(arena + expect);
        if (e != k || e != want || afterE != afterC || afterE != init || !nv) { if (bad++ < 5) printf("FT DIF c=%u exe=%x cpp=%x want=%x nv=%d\n", c, e, k, want, nv); }
        if (e) ++withFirst;
        fprintf(dump, "F %u %u %ld ", c, (uint32_t)arena, e ? (long)(e - (uint32_t)arena) : -1L); for (uint8_t b : afterC) fprintf(dump, "%02x", b); fprintf(dump, "\n");
    }
    printf("GetFirstTexture: %u casos, no nulos=%ld, difs=%ld\n", kFirstTextureCases, withFirst, bad);
    // ---- Replace: 1536 casos del banco ----
    long rbad = 0, replaced = 0, absent = 0, minus1 = 0, destroyLast = 0, eventsTotal = 0;
    for (unsigned c = 0; c < kReplaceCases; ++c) {
        auto fx = Replace(c);
        auto* geo = (RpGeometry*)(arena + kRepGeometry);
        auto* oldM = (RpMaterial*)(arena + PoolOffset(fx.oldMat)); auto* newM = (RpMaterial*)(arena + PoolOffset(fx.newMat));
        g_base = arena;
        BuildReplace(arena, fx); g_trace.clear();
        bool nv; uint32_t e = CallExe3(geo, oldM, newM, nv) & 0xFF; std::vector<uint8_t> afterE(arena, arena + kRepArena); auto tE = g_trace;
        BuildReplace(arena, fx); g_trace.clear();
        uint32_t k = ReplaceCpp(geo, oldM, newM) ? 1 : 0; std::vector<uint8_t> afterC(arena, arena + kRepArena); auto tC = g_trace;
        if (e != k || afterE != afterC || tE != tC || !nv) { if (rbad++ < 5) printf("RP DIF c=%u exe=%u cpp=%u mem=%d trace=%d nv=%d\n", c, e, k, afterE == afterC, tE == tC, nv); }
        fprintf(dump, "R %u %u %u ", c, (uint32_t)arena, k); for (uint8_t b : afterC) fprintf(dump, "%02x", b); fprintf(dump, " "); for (uint32_t t : tC) fprintf(dump, "%u,", t); fprintf(dump, "\n");
        if (e) ++replaced; if (fx.absent) ++absent; eventsTotal += (long)tE.size() / 3;
        for (size_t q = 0; q + 2 < tE.size(); q += 3) if (tE[q] == 1 && (int32_t)0 == 0) {}
    }
    printf("Replace: %u casos, con reemplazo=%ld, old ausente=%ld, eventos=%ld, difs=%ld\n", kReplaceCases, replaced, absent, eventsTotal, rbad);
    return 0;
}
