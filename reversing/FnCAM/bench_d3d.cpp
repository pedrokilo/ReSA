// Banco CAM-D3D: las funciones estandar CAMERAENDUPDATE (0x7F98D0), RASTERSHOWRASTER (0x7F99B0) e IMAGEFINDRASTERFORMAT (0x7FFF00) del EXE original contra su reconstruccion C++ de rwcore.cpp, fuera del juego.
// Metodo FnP3/FnD3DStd: el gta_sa.exe se mapea en 0x400000 y se llama por su direccion; el texto C++ (helper COM, extension D3D9 del raster, D3D9RestoreAfterReset y las tres funciones) se extrae tal cual con
// extract_d3d.py a d3d_func.inc. El estado global es el del propio EXE (mismas direcciones: dispositivo 0xC97C28, escena 0xC97C54, parametros de presentacion 0xC9C040, superficies, luces, camara actual 0xC9BCC0...).
// Falsos con traza para el EXE y para el C++: el dispositivo IDirect3DDevice9 (TestCooperativeLevel, Reset, Present, StretchRect, GetRenderTarget, GetDepthStencilSurface, BeginScene, EndScene, SetLight, LightEnable),
// la cadena de intercambio (Present), las superficies y la textura (Release, GetSurfaceLevel / GetCubeMapSurface), y por direccion las funciones de las que dependen: 0x7F7F70 (liberar memoria de video), 0x4CC970 (lista
// de rasters), 0x7F58D0, 0x7FD100, 0x7FB480, 0x80E020, 0x7F5800, 0x7FED70, 0x4CC580, 0x7FFFF0, _rwerror 0x8088D0, RwErrorSet 0x808820 y el callback de restauracion (0xC980B0). Se compara el valor devuelto, el estado global
// observable, la memoria de los objetos de prueba y la traza completa (incluidos los argumentos de cada llamada COM).
// Uso: bench_d3d.exe <ruta del EXE> [--casos N] [--semilla S] [--solo K]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <vector>
#include <random>
#include <algorithm>
#include <map>

typedef int8_t int8; typedef int16_t int16; typedef int32_t int32; typedef uint8_t uint8; typedef uint16_t uint16; typedef uint32_t uint32;
typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef uint8_t RwUInt8; typedef int16_t RwInt16; typedef uint16_t RwUInt16; typedef int32_t RwBool;
#define TRUE 1
#define FALSE 0
template<typename T> inline T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<typename T> static T& G(uintptr_t a) { return *(T*)a; }

struct RwRaster {
    RwRaster* parent; RwUInt8* cpPixels; RwUInt8* palette; RwInt32 width, height, depth, stride; RwInt16 nOffsetX, nOffsetY;
    RwUInt8 cType, cFlags, privateFlags, cFormat; RwUInt8* originalPixels; RwInt32 originalWidth, originalHeight, originalStride;
};
static_assert(sizeof(RwRaster) == 0x34 && offsetof(RwRaster, cType) == 0x20);
struct RwCamera { RwUInt8 pad[0x60]; RwRaster* frameBuffer; RwUInt8 pad2[0x40]; };
struct RwImage { RwInt32 flags; RwInt32 width; RwInt32 height; RwInt32 depth; RwInt32 stride; RwUInt8* cpPixels; void* palette; };
struct RwError { RwInt32 pluginID; RwInt32 errorCode; };
static_assert(offsetof(RwImage, width) == 4 && offsetof(RwImage, height) == 8);

// ---------------------------------------------------------------- traza, arena y falsos
static std::vector<uint32_t> g_log;
static void L(uint32_t a) { g_log.push_back(a); if (getenv("BENCH_TRACE2")) printf("L %x\n", a); }
static void L(uint32_t a, uint32_t b) { L(a); L(b); }
static void L(uint32_t a, uint32_t b, uint32_t c) { L(a); L(b); L(c); }
static void L(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { L(a); L(b); L(c); L(d); }
static uint8_t g_arena[1 << 18]; static size_t g_arenaUsed;
static void* arena_alloc(size_t n, uint8_t fill = 0xCD) { n = (n + 15) & ~15u; void* p = g_arena + g_arenaUsed; g_arenaUsed += n; if (g_arenaUsed > sizeof g_arena) { printf("arena agotada\n"); ExitProcess(4); } memset(p, fill, n); return p; }
static uint32_t id_of(const void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t g_hseed;
static uint32_t H(uint32_t a, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) {
    uint64_t x = g_hseed * 0x9E3779B97F4A7C15ull; for (uint32_t v : {a, b, c, d}) { x ^= v + 0x9E3779B9u + (x << 6) + (x >> 2); x *= 0xFF51AFD7ED558CCDull; } return (uint32_t)(x >> 32);
}
static std::mt19937_64 g_caseRng;
static uint32_t cr(uint32_t n) { return n ? (uint32_t)(g_caseRng() % n) : 0; }
static uint32_t g_callSerial;
static uint32_t pickHr(uint32_t tag) { // resultados posibles de una llamada COM, por semilla y numero de llamada
    static const uint32_t hrs[] = { 0, 0, 0, 0x88760868u, 0x88760869u, 0x8876086Cu, 0x80004005u, 1u };
    return hrs[H(tag, g_callSerial++) % 8];
}
// objetos COM falsos (la tabla virtual se lee en cada llamada: vtbl[Slot])
static void* g_vtblDevice[120]; static void* g_vtblSwap[8]; static void* g_vtblSurface[8]; static void* g_vtblTexture[24];
static HRESULT __stdcall Unexpected() { L(0xBAD); return (HRESULT)0x80004005; }
static ULONG __stdcall Obj_Release(void* self) { L(0xD02, id_of(self)); return 0; }
static HRESULT __stdcall Dev_TestCoop(void* self) { const uint32_t hr = pickHr(0xD03); L(0xD03, id_of(self), hr); return (HRESULT)hr; }
static HRESULT __stdcall Dev_Reset(void* self, void* params) { const uint8_t* p = (const uint8_t*)params; L(0xD10, id_of(self), id_of(params)); L(*(uint32_t*)(p + 0x34), *(uint32_t*)(p + 0x20)); const uint32_t hr = pickHr(0xD10); L(hr); return (HRESULT)hr; }
static HRESULT __stdcall Dev_Present(void* self, void* a, void* b, void* c, void* d) { L(0xD11, id_of(self), id_of(a), id_of(b)); L(id_of(c), id_of(d)); const uint32_t hr = pickHr(0xD11); L(hr); return (HRESULT)hr; }
static HRESULT __stdcall Swap_Present(void* self, void* a, void* b, void* c, void* d, DWORD f) { L(0xD12, id_of(self), id_of(a), id_of(b)); L(id_of(c), id_of(d), f); const uint32_t hr = pickHr(0xD12); L(hr); return (HRESULT)hr; }
static HRESULT __stdcall Dev_StretchRect(void* self, void* src, const RECT* sr, void* dst, const RECT* dr, DWORD filter) {
    L(0xD13, id_of(self), id_of(src), id_of(dst)); L(filter, sr ? (uint32_t)sr->left : 0xFFFFFFFF, sr ? (uint32_t)sr->top : 0, sr ? (uint32_t)sr->right : 0); L(sr ? (uint32_t)sr->bottom : 0, id_of(sr) == id_of(dr) ? 1u : 0u, dr ? (uint32_t)dr->left : 0, dr ? (uint32_t)dr->bottom : 0);
    return (HRESULT)pickHr(0xD13);
}
static void* g_surfaces[32]; // objetos COM de superficie (primer dword = tabla virtual)
static HRESULT __stdcall Dev_GetRT(void* self, DWORD idx, void** out) { L(0xD14, id_of(self), idx); *out = &g_surfaces[H(0xD14, g_callSerial++) % 16]; L(id_of(*out)); return 0; }
static HRESULT __stdcall Dev_GetDS(void* self, void** out) { L(0xD15, id_of(self)); *out = &g_surfaces[16 + H(0xD15, g_callSerial++) % 16]; L(id_of(*out)); return 0; }
static HRESULT __stdcall Dev_BeginScene(void* self) { L(0xD16, id_of(self)); return (HRESULT)pickHr(0xD16); }
static HRESULT __stdcall Dev_EndScene(void* self) { L(0xD17, id_of(self)); return 0; }
static HRESULT __stdcall Dev_SetLight(void* self, DWORD idx, const void* light) { L(0xD18, id_of(self), idx, *(const uint32_t*)light); return 0; }
static HRESULT __stdcall Dev_LightEnable(void* self, DWORD idx, BOOL on) { const uint32_t hr = H(0xD19, idx) % 4 == 0 ? 0x80004005u : 0u; L(0xD19, id_of(self), idx, (uint32_t)on); L(hr); return (HRESULT)hr; }
static HRESULT __stdcall Tex_GetSurface(void* self, DWORD a, DWORD b, void** out) { L(0xD20, id_of(self), a, b); *out = &g_surfaces[H(0xD20, g_callSerial++) % 16]; L(id_of(*out)); return 0; }   // cubo: (cara, nivel, salida)
static HRESULT __stdcall Tex_GetSurface1(void* self, DWORD a, void** out) { L(0xD21, id_of(self), a); *out = &g_surfaces[H(0xD21, g_callSerial++) % 16]; L(id_of(*out)); return 0; }        // textura: (nivel, salida)
// funciones por direccion
static RwBool __cdecl FakeReleaseVideoMemory() { L(0xE60); return 1; }
static void __cdecl FakeForceDiscard() { L(0xE61); }
static RwBool __cdecl FakeRasterListRestore() { const uint32_t r = H(0xE62, g_callSerial++) % 5 != 0; L(0xE62, r); return r; }
static RwBool __cdecl FakeDynVbRestore() { const uint32_t r = H(0xE63, g_callSerial++) % 5 != 0; L(0xE63, r); return r; }
static void __cdecl FakeStateReset() { L(0xE64); }
static RwBool __cdecl FakeIm2DOpen() { const uint32_t r = H(0xE65, g_callSerial++) % 4 != 0; L(0xE65, r); return r; }
static RwBool __cdecl FakeIm3DOpen() { const uint32_t r = H(0xE66, g_callSerial++) % 4 != 0; L(0xE66, r); return r; }
static void __cdecl FakeCallback() { L(0xE67); }
static void __cdecl FakeCheckValidTextureSize(RwInt32* w, RwInt32* h, RwUInt32 f) { L(0xE68, (uint32_t)*w, (uint32_t)*h, f); *w = (*w + 3) & ~3; *h = *h > 8 ? *h & ~1 : *h; L((uint32_t)*w, (uint32_t)*h); }
static RwBool __cdecl FakeSetRasterFormat(RwRaster* r, RwUInt32 flags) { L(0xE69, id_of(r), flags, (uint32_t)r->width); L((uint32_t)r->height, (uint32_t)r->depth); return (RwBool)(H(0xE69, flags) % 3 != 0); }
static RwInt32 __cdecl FakeFindFormat(const RwImage* image) { const uint32_t f = (H(0xE6A, (uint32_t)image->width, (uint32_t)image->height) & 0xF00u) | (H(0xE6B, (uint32_t)image->depth) & 0x6000u); L(0xE6A, (uint32_t)image->width, f); return (RwInt32)f; }
static int __cdecl FakeRwError(int code, int arg) { UNREFERENCED_PARAMETER(arg); L(0xE10, (uint32_t)code); return code; }
static int __cdecl FakeRwErrorSet(RwError* e) { L(0xE11, (uint32_t)e->pluginID, (uint32_t)e->errorCode); return 0; }
static int _rwerror(int code) { return FakeRwError(code, 0); }
static void RwErrorSet(RwError* e) { FakeRwErrorSet(e); }
namespace {
void D3D9RaiseError(RwInt32 code) { RwError error; error.pluginID = 1; error.errorCode = _rwerror(code); RwErrorSet(&error); }
}
RwBool _rwD3D9DeviceReleaseVideoMemory() { return FakeReleaseVideoMemory(); }
void _rwD3D9DynamicVertexBufferManagerForceDiscard() { FakeForceDiscard(); }
RwBool _rwD3D9DynamicVertexBufferRestore() { return FakeDynVbRestore(); }
void _rwD3D9RenderStateReset() { FakeStateReset(); }
RwBool _rwD3D9Im2DRenderOpen() { return FakeIm2DOpen(); }
RwBool _rwD3D9Im3DRenderOpen() { return FakeIm3DOpen(); }
void _rwD3D9CheckValidTextureSize(RwInt32* w, RwInt32* h, RwUInt32 f) { FakeCheckValidTextureSize(w, h, f); }
RwInt32 _rwD3D9ImageFindRasterFormat(const RwImage* image) { return FakeFindFormat(image); }
struct D3D9RasterFormat {
    static RwBool VideoMemoryRasterListRestore() { return FakeRasterListRestore(); }
    static RwBool SetRasterFormat(RwRaster* r, uint32 flags) { return FakeSetRasterFormat(r, flags); }
};

#include "d3d_func.inc"

// ---------------------------------------------------------------- mapa del EXE
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
static void divert(uintptr_t exeAddr, void* target) { *(uint8_t*)exeAddr = 0xE9; *(int32_t*)(exeAddr + 1) = (int32_t)((uintptr_t)target - (exeAddr + 5)); }

// ---------------------------------------------------------------- estado por semilla
enum Kind { K_END, K_SHOW, K_FIND, K_COUNT };
struct Args { Kind kind; void* a0; void* a1; RwInt32 flags; };
static Args setup(uint64_t seed, Kind kind) {
    g_caseRng.seed(seed); g_hseed = (uint32_t)seed * 2654435761u; g_log.clear(); g_callSerial = 0; g_arenaUsed = 0x100; memset(g_arena, 0, 0x100);
    for (auto& s : g_vtblDevice) s = (void*)&Unexpected; for (auto& s : g_vtblSwap) s = (void*)&Unexpected; for (auto& s : g_vtblSurface) s = (void*)&Unexpected; for (auto& s : g_vtblTexture) s = (void*)&Unexpected;
    g_vtblDevice[2] = (void*)&Obj_Release; g_vtblDevice[3] = (void*)&Dev_TestCoop; g_vtblDevice[16] = (void*)&Dev_Reset; g_vtblDevice[17] = (void*)&Dev_Present; g_vtblDevice[34] = (void*)&Dev_StretchRect;
    g_vtblDevice[38] = (void*)&Dev_GetRT; g_vtblDevice[40] = (void*)&Dev_GetDS; g_vtblDevice[41] = (void*)&Dev_BeginScene; g_vtblDevice[42] = (void*)&Dev_EndScene; g_vtblDevice[51] = (void*)&Dev_SetLight; g_vtblDevice[53] = (void*)&Dev_LightEnable;
    g_vtblSwap[2] = (void*)&Obj_Release; g_vtblSwap[3] = (void*)&Swap_Present; g_vtblSurface[2] = (void*)&Obj_Release; g_vtblTexture[2] = (void*)&Obj_Release;
    // los objetos con tabla virtual: el primer dword es la tabla
    static void* devObj; static void* swapObj; static void* surfObj; static void* texObj; static void* cubeObj; static void* vtblCube[24];
    for (int i = 0; i < 24; i++) vtblCube[i] = g_vtblTexture[i]; g_vtblTexture[18] = (void*)&Tex_GetSurface1; vtblCube[18] = (void*)&Tex_GetSurface;
    for (auto& so : g_surfaces) so = g_vtblSurface;
    devObj = g_vtblDevice; swapObj = g_vtblSwap; surfObj = g_vtblSurface; texObj = g_vtblTexture; cubeObj = vtblCube;
    // estado global del EXE
    G<void*>(0xC97C28) = &devObj; G<void*>(0xC97C30) = &surfObj; G<void*>(0xC97C2C) = &surfObj; G<void*>(0xC97C20) = &devObj;
    G<RwInt32>(0xB4E9E0) = 0x40; G<RwInt32>(0xC97C54) = (RwInt32)cr(2); G<RwInt32>(0xC97C34) = (RwInt32)cr(2);
    uint32_t* present = (uint32_t*)0xC9C040; for (int i = 0; i < 14; i++) present[i] = 0x1000 + i; present[0x20 / 4] = cr(2);
    static const uint32_t ints[] = { 0, 1, 0x80000000u, 1, 0x80000000u, 2, 0 }; present[0x34 / 4] = ints[cr(7)];
    G<uint32_t>(0xC9BFF4) = cr(2) ? 0x1000100u : (cr(2) ? 0x100u : 0u);
    const uint32_t nl = cr(3) == 0 ? cr(4) : 0; G<RwInt32>(0xC98084) = (RwInt32)nl; auto* cache = (uint8_t*)arena_alloc(0x6C * (nl + 1)); for (uint32_t i = 0; i < nl; i++) { *(uint32_t*)(cache + i * 0x6C) = 0xA000 + i; *(uint32_t*)(cache + i * 0x6C + 0x68) = cr(2); } G<uint8_t*>(0xC98088) = cache;
    G<void*>(0xC980B0) = cr(2) ? (void*)&FakeCallback : nullptr;
    G<void*>(0xC9BCC0) = nullptr;
    Args a{ kind, nullptr, nullptr, 0 };
    // rasters: raster de imagen (hijo) y raster superior con extension
    auto* top = (RwRaster*)arena_alloc(0x80); top->parent = top; top->cType = cr(3) ? 5 : (RwUInt8)cr(8); top->width = (RwInt32)(1 + cr(512)); top->height = (RwInt32)(1 + cr(512));
    auto* ext = (uint32_t*)((uint8_t*)top + 0x40); const bool cube = cr(3) == 0;
    ext[0] = id_of(cube ? &cubeObj : &texObj); ((uint8_t*)ext)[9] = cube ? (uint8_t)(1 + cr(2) + (cr(6) << 4)) : (uint8_t)(cr(2) ? 0 : (cr(6) << 4)); ext[7] = cr(5) == 0 ? id_of(&swapObj) : 0;
    if (cr(8) == 0) top->cType = 5;
    auto* sub = (RwRaster*)arena_alloc(0x80); sub->parent = cr(4) == 0 ? sub : top; sub->nOffsetX = (RwInt16)((int)cr(40) - 20); sub->nOffsetY = (RwInt16)((int)cr(40) - 20); sub->width = (RwInt32)cr(300); sub->height = (RwInt32)cr(300);
    if (sub->parent == sub) { sub->cType = top->cType; memcpy((uint8_t*)sub + 0x40, ext, 0x24); }
    auto* cam = (RwCamera*)arena_alloc(sizeof(RwCamera)); cam->frameBuffer = sub;
    if (kind == K_END) { G<RwCamera*>(0xC9BCC0) = cam; }
    if (kind == K_SHOW) { a.a0 = sub; a.a1 = (void*)(uintptr_t)cr(100); a.flags = (RwInt32)cr(4); }
    if (kind == K_FIND) {
        auto* raster = (RwRaster*)arena_alloc(0x80); raster->width = 0x11; raster->height = 0x22; raster->depth = 0x33; auto* img = (RwImage*)arena_alloc(sizeof(RwImage));
        img->width = (RwInt32)(1 + cr(600)); img->height = (RwInt32)(1 + cr(600)); img->depth = (RwInt32)(cr(3) == 0 ? 32 : (cr(2) ? 8 : 4));
        a.a0 = raster; a.a1 = img; static const uint32_t fl[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 0x1000, 0x6000, 0x9000, 0xFFFF6FFF, 0x12345, 0xFFFFFFFFu, 0x80, 0x9004, 0x9005, 0x6000, 0 };
        a.flags = (RwInt32)(fl[cr(20)] + (cr(4) == 0 ? cr(256) : 0));
    }
    return a;
}
static RwBool call_exe(const Args& a) {
    switch (a.kind) {
    case K_END: return ((RwBool(__cdecl*)(void*, void*, RwInt32))0x7F98D0)(nullptr, nullptr, 0);
    case K_SHOW: return ((RwBool(__cdecl*)(void*, void*, RwInt32))0x7F99B0)(a.a0, a.a1, a.flags);
    default: return ((RwBool(__cdecl*)(void*, void*, RwInt32))0x7FFF00)(a.a0, a.a1, a.flags);
    }
}
static RwBool call_cpp(const Args& a) {
    switch (a.kind) {
    case K_END: return _rwD3D9CameraEndUpdate(nullptr, nullptr, 0);
    case K_SHOW: return _rwD3D9RasterShowRaster((RwRaster*)a.a0, a.a1, a.flags);
    default: return _rwD3D9StandardImageFindRasterFormat((RwRaster*)a.a0, (const RwImage*)a.a1, a.flags);
    }
}
static std::vector<uint32_t> observe(RwBool ret) {
    std::vector<uint32_t> o; o.push_back((uint32_t)ret);
    for (uintptr_t ad : {0xC97C54u, 0xC97C34u, 0xC9BCC0u, 0xC97C30u, 0xC97C2Cu}) o.push_back(G<uint32_t>(ad));
    const uint32_t* pp = (const uint32_t*)0xC9C040; for (int i = 0; i < 14; i++) o.push_back(pp[i]);
    const uint32_t* p = (const uint32_t*)g_arena; for (size_t i = 0; i < g_arenaUsed / 4; i++) o.push_back(p[i]);
    o.insert(o.end(), g_log.begin(), g_log.end());
    return o;
}
static LONG WINAPI Crash(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr);
    fflush(stdout); ExitProcess(3); return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(Crash);
    if (argc < 2) { printf("uso: bench_d3d.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo K]\n"); return 2; }
    uint64_t n = 100000, seed = 1; int only = -1;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) only = atoi(argv[++i]); }
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (!map_exe(argv[1])) return 2;
    unsigned int cw; _controlfp_s(&cw, _PC_24, _MCW_PC);
    divert(0x7F7F70, (void*)&FakeReleaseVideoMemory); divert(0x7F5800, (void*)&FakeForceDiscard); divert(0x4CC970, (void*)&FakeRasterListRestore); divert(0x7F58D0, (void*)&FakeDynVbRestore); divert(0x7FD100, (void*)&FakeStateReset);
    divert(0x7FB480, (void*)&FakeIm2DOpen); divert(0x80E020, (void*)&FakeIm3DOpen); divert(0x7FED70, (void*)&FakeCheckValidTextureSize); divert(0x4CC580, (void*)&FakeSetRasterFormat); divert(0x7FFFF0, (void*)&FakeFindFormat);
    divert(0x8088D0, (void*)&FakeRwError); divert(0x808820, (void*)&FakeRwErrorSet);
    static const char* names[] = { "0x7F98D0 CAMERAENDUPDATE", "0x7F99B0 RASTERSHOWRASTER", "0x7FFF00 IMAGEFINDRASTERFORMAT" };
    uint64_t fails[K_COUNT] = {}, runs[K_COUNT] = {}, truthy[K_COUNT] = {}; std::map<uint32_t, uint64_t> cov;
    for (uint64_t it = 0; it < n; it++) {
        const Kind kind = (Kind)(it % K_COUNT); if (only >= 0 && kind != (Kind)only) continue;
        const uint64_t cs = seed * 1000003 + it;
        Args a = setup(cs, kind); const RwBool ra = call_exe(a); const std::vector<uint32_t> oa = observe(ra);
        for (uint32_t t : {0xD10u, 0xD11u, 0xD12u, 0xD13u, 0xD03u, 0xD17u, 0xD18u, 0xD20u, 0xD21u, 0xE10u, 0xE60u, 0xE61u, 0xE67u, 0xE68u}) if (std::find(g_log.begin(), g_log.end(), t) != g_log.end()) cov[t]++;
        Args b = setup(cs, kind); const RwBool rb = call_cpp(b); const std::vector<uint32_t> ob = observe(rb);
        runs[kind]++; if (ra) truthy[kind]++;
        if (oa != ob) {
            fails[kind]++;
            if (fails[kind] <= 2) {
                printf("DIFERENCIA %s caso=%llu: exe ret=%08x (%zu valores) ours ret=%08x (%zu valores)\n", names[kind], (unsigned long long)cs, (unsigned)ra, oa.size(), (unsigned)rb, ob.size());
                for (size_t i = 0; i < std::min(oa.size(), ob.size()); i++) if (oa[i] != ob[i]) { printf("  primer valor distinto en [%zu]: exe=%08x ours=%08x\n", i, oa[i], ob[i]); break; }
            }
        }
    }
    uint64_t total = 0;
    for (int k = 0; k < K_COUNT; k++) { if (!runs[k]) continue; printf("%-32s %llu casos (TRUE: %llu), %llu diferencias\n", names[k], (unsigned long long)runs[k], (unsigned long long)truthy[k], (unsigned long long)fails[k]); total += fails[k]; }
    printf("cobertura de marcas:"); for (auto& kv : cov) printf(" %x=%llu", kv.first, (unsigned long long)kv.second);
    printf("\nRESULTADO: %s (%llu diferencias)\n", total ? "HAY DIFERENCIAS" : "SIN DIFERENCIAS", (unsigned long long)total);
    return total ? 1 : 0;
}
