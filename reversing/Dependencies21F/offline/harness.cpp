// Banco nativo (fuera del juego): bytes reales del EXE (0x7305E0 y 0x730740) frente a la reconstruccion C++ del parche, con globales falsos
// mapeados en las mismas direcciones y objetos COM falsos. Solo para comprobar la reconstruccion antes del banco en el juego.
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
using uint32 = uint32_t;
using int32 = int32_t;
using uint8 = uint8_t;
#define VALIDATE_SIZE(a, b) static_assert(sizeof(a) == b)
#include "D3DResourceSystem.h"
#include "body.inc"

bool&                D3DResourceSystem::UseD3DResourceBuffering = *reinterpret_cast<bool*>(0x8D6084);
uint32&              D3DResourceSystem::NumTextureBuffers       = *reinterpret_cast<uint32*>(0xC87C60);
D3DTextureBuffer&    D3DResourceSystem::TextureBuffer           = *reinterpret_cast<D3DTextureBuffer*>(0xC87C68);
D3DTextureBuffer (&D3DResourceSystem::TextureBuffers)[17]       = *reinterpret_cast<D3DTextureBuffer(*)[17]>(0xC87C68);
D3DIndexDataBuffer (&D3DResourceSystem::IndexDataBuffers)[16]   = *reinterpret_cast<D3DIndexDataBuffer(*)[16]>(0xC87E48);
D3DIndexDataBuffer&  D3DResourceSystem::IndexDataBuffer         = *reinterpret_cast<D3DIndexDataBuffer*>(0xC87FC8);
uint32&              D3DResourceSystem::FreeTextureBufferIndex  = *reinterpret_cast<uint32*>(0xC87FE0);
uint32&              D3DResourceSystem::FreeIndexBufferIndex    = *reinterpret_cast<uint32*>(0xC87FE4);

static constexpr uintptr_t G0 = 0xC87C60, G1 = 0xC87FE8;

struct Fake { const void* vtbl; uint32 id; uint32 releases; };
static Fake pool[600];
struct TraceEntry { uint32 id; uint32 hash; };
static std::vector<TraceEntry> trace;
static int releaseNo, mutateAt, mutateN, mutateIdx, mutateIdxIb;
static uint32 Fnv(const void* p, size_t n) { uint32 h = 2166136261u; const uint8* b = (const uint8*)p; for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 16777619u; } return h; }
static unsigned long __stdcall FakeQI(Fake*) { printf("QI llamado\n"); exit(2); }
static unsigned long __stdcall FakeAddRef(Fake*) { printf("AddRef llamado\n"); exit(2); }
static unsigned long __stdcall FakeRelease(Fake* f) {
    ++f->releases; ++releaseNo;
    trace.push_back({f->id, Fnv((void*)G0, G1 - G0)});
    if (mutateAt && releaseNo == mutateAt) {
        if (mutateN) *(uint32*)0xC87C60 = mutateN;
        if (mutateIdx >= 0) *(uint32*)0xC87FE0 = mutateIdx;
        if (mutateIdxIb >= 0) *(uint32*)0xC87FE4 = mutateIdxIb;
    }
    return 0x1234 + f->id;
}
static const void* vt[3] = { (void*)FakeQI, (void*)FakeAddRef, (void*)FakeRelease };

static uint32 rngState;
static uint32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 17; rngState ^= rngState << 5; return rngState; }
static uint32 Rn(uint32 n) { return Rnd() % n; }

static void* texArr[17][64];
static void* ibArr[17][64];
static void* genArr[64];

struct Snapshot {
    std::vector<uint8> bytes;
    bool operator==(const Snapshot& o) const { return bytes == o.bytes; }
};
static Snapshot Take() {
    Snapshot s; auto add = [&](const void* p, size_t n) { s.bytes.insert(s.bytes.end(), (const uint8*)p, (const uint8*)p + n); };
    add((void*)G0, G1 - G0); add(texArr, sizeof texArr); add(ibArr, sizeof ibArr); add(genArr, sizeof genArr); add(pool, sizeof pool);
    return s;
}
static void Restore(const Snapshot& s) {
    size_t at = 0; auto get = [&](void* p, size_t n) { memcpy(p, s.bytes.data() + at, n); at += n; };
    get((void*)G0, G1 - G0); get(texArr, sizeof texArr); get(ibArr, sizeof ibArr); get(genArr, sizeof genArr); get(pool, sizeof pool);
}

static uint32 Extreme() { static const uint32 e[] = { 0, 1, 2, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFE, 100, 200 }; return e[Rn(9)]; }

static void Build(bool tex) {
    memset((void*)G0, 0xA5, G1 - G0);
    for (auto& a : texArr) for (auto& p : a) p = (void*)0xDEAD0000;
    for (auto& a : ibArr) for (auto& p : a) p = (void*)0xDEAD0000;
    for (auto& p : genArr) p = (void*)0xDEAD0000;
    for (unsigned i = 0; i < 600; ++i) { pool[i].vtbl = vt; pool[i].id = i; pool[i].releases = 0; }
    uint32 next = 0;
    auto obj = [&]() -> void* { uint32 r = Rn(20); if (r < 3) return nullptr; if (r < 5 && next) return &pool[Rn(next)]; return &pool[next++ % 600]; };
    uint32 n = 1 + Rn(17); if (Rn(8) == 0) n = 17; if (Rn(10) == 0) n = 1;
    *(uint32*)0xC87C60 = n; *(uint32*)0xC87C64 = 0x55AA55AA;
    *(uint32*)0xC87FE0 = Rn(4) == 0 ? Rn(17) : Rn(n);
    *(uint32*)0xC87FE4 = Rn(4) == 0 ? Rn(17) : Rn(16);
    unsigned fill = Rn(4);
    auto fillBuf = [&](uint32* num, uint32* size, void* arr, int numOff) {
        uint32 sz = fill == 0 ? 0 : Rn(fill == 1 ? 3 : 20);
        if (Rn(6) == 0) sz = 0;
        *size = sz; *num = Rn(3) == 0 ? Extreme() : sz + Rn(4);
        void** a = (void**)arr; for (uint32 i = 0; i < sz; ++i) a[i] = obj();
    };
    for (int i = 0; i < 17; ++i) {
        D3DTextureBuffer* b = (D3DTextureBuffer*)(0xC87C68 + i * 0x1C);
        b->m_nFormat = Rnd(); b->m_nWidth = Rnd(); b->m_nLevels = -1; b->m_nCapcacity = 64;
        fillBuf(&b->m_nNumTexturesInBuffer, &b->m_nSize, texArr[i], 0); b->m_apTextures = (void**)texArr[i];
    }
    for (int i = 0; i < 16; ++i) {
        D3DIndexDataBuffer* b = (D3DIndexDataBuffer*)(0xC87E48 + i * 0x18);
        b->m_nFormat = Rnd(); b->field_4 = 7; b->m_nCapcacity = 64;
        fillBuf(&b->m_nNumDatasInBuffer, &b->m_nSize, ibArr[i], 0); b->m_apIndexData = (void**)ibArr[i];
    }
    { D3DIndexDataBuffer* b = (D3DIndexDataBuffer*)0xC87FC8; b->m_nFormat = Rnd(); b->field_4 = 7; b->m_nCapcacity = 64;
      fill = Rn(4); fillBuf(&b->m_nNumDatasInBuffer, &b->m_nSize, genArr, 0); b->m_apIndexData = (void**)genArr; }
    releaseNo = 0; trace.clear();
    mutateAt = 0; mutateN = 0; mutateIdx = -1; mutateIdxIb = -1;
    if (Rn(3) == 0) {
        mutateAt = 1 + Rn(5);
        if (Rn(2)) mutateN = 1 + Rn(17);
        if (Rn(2)) mutateIdx = Rn(17);
        if (Rn(2)) mutateIdxIb = Rn(17);
    }
}

typedef void(__cdecl* Fn)(uint32);
int main() {
    void* g = VirtualAlloc((void*)0xC80000, 0x10000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (g != (void*)0xC80000) { printf("no se pudo mapear 0xC87000 (%p)\n", g); return 1; }
    VirtualAlloc((void*)0x8D0000, 0x10000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    uint8* code = (uint8*)VirtualAlloc(nullptr, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    auto load = [&](const char* f, size_t at) { FILE* fp = fopen(f, "rb"); size_t n = fread(code + at, 1, 0x400, fp); fclose(fp); return n; };
    load("exe_tex.bin", 0); load("exe_ib.bin", 0x200);
    Fn exeFn[2] = { (Fn)(code), (Fn)(code + 0x200) };
    Fn dllFn[2] = { &D3DResourceSystem::TidyUpD3DTextures, &D3DResourceSystem::TidyUpD3DIndexBuffers };
    const char* names[2] = { "TidyUpD3DTextures", "TidyUpD3DIndexBuffers" };
    static const uint32 counts[] = { 0, 1, 2, 3, 5, 100, 200, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF };
    int fails = 0; unsigned long total = 0;
    long relCount[2] = {0, 0}, exhaust[2] = {0, 0}, mut[2] = {0, 0};
    for (int f = 0; f < 2; ++f) {
        for (unsigned s = 0; s < 200000; ++s) {
            rngState = 0x9E3779B9u * (s + 1) + f * 77;
            uint32 count = counts[Rn(10)]; if (Rn(3) == 0) count = Rn(40);
            Build(f == 0);
            Snapshot init = Take(); int m0 = mutateAt, mN = mutateN, mI = mutateIdx, mIb = mutateIdxIb;
            exeFn[f](count);
            Snapshot a = Take(); auto ta = trace; int ra = releaseNo;
            Restore(init); trace.clear(); releaseNo = 0; mutateAt = m0; mutateN = mN; mutateIdx = mI; mutateIdxIb = mIb;
            dllFn[f](count);
            Snapshot b = Take();
            bool same = a == b && ta.size() == trace.size() && (ta.empty() || !memcmp(ta.data(), trace.data(), ta.size() * sizeof(TraceEntry)));
            ++total; relCount[f] += ra; if (ra < 0 || (uint32)ra < count) ++exhaust[f]; if (m0 && ra >= m0) ++mut[f];
            if (!same) { if (++fails < 10) printf("DIFERENCIA %s caso %u count=%u releases exe=%zu dll=%zu mem=%d\n", names[f], s, count, ta.size(), trace.size(), a == b); }
        }
        printf("%s: liberaciones totales=%ld, casos que agotan=%ld, con mutacion aplicada=%ld\n", names[f], relCount[f], exhaust[f], mut[f]);
    }
    printf("casos=%lu diferencias=%d\n", total, fails);
    return fails != 0;
}
