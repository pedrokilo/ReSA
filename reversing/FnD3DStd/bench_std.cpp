// Banco D3DSTD: los 12 manejadores de raster de la tabla de funciones estandar del dispositivo D3D9 (0x4CA4E0, 0x4CAE40/60/80, 0x4CB4C0/E0, 0x4CB520, 0x4CBCB0, 0x4CBD40/50, 0x4CD4D0, 0x4CD820) con sus
// ayudantes usercall (0x4CAD90, 0x4CAEA0, 0x4CAAE0, 0x4CA5A0) del EXE original contra source/game_sa/RenderWare/D3D9RasterStandards.cpp (compilado aqui tal cual, con cabeceras minimas de shim/), fuera del juego.
// Metodo FnP3: el gta_sa.exe original (solo lectura) se mapea en 0x400000 en este proceso de 32 bits. Se desvian a falsos del banco, tanto en el EXE (salto en la entrada de la copia mapeada) como para el C++, todas las
// funciones de RenderWare que estas rutinas llaman (estados de render, Im2D, bloqueo de rasters, flujos, texturas, formatos) y los metodos COM de IDirect3DDevice9 y de las texturas/superficies (vtables falsas).
// Cada falso escribe en la traza su marca y sus argumentos (valores, no direcciones de pila) y devuelve resultados deterministas por hash de sus argumentos y de la semilla del caso, asi que cada rama del C++ y del
// EXE ve las mismas respuestas. Tras cada caso con el mismo estado inicial se compara: valor devuelto, la arena completa (rasters, extensiones, objetos COM, texturas), el bufer de pixeles y el de la paleta, el flujo
// escrito, los globales (destino actual) y la traza completa.
// Uso: bench_std.exe <ruta del EXE> [--casos N] [--semilla S] [--solo <manejador>]   (devuelve 0 si todo coincide)
#include "StdInc.h"
#include "D3DResourceSystem.h"
#include "D3D9RasterFormat.h"
#include "D3D9RasterDriver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <map>

static std::vector<uint32_t> g_log;
static void L(uint32_t a) { g_log.push_back(a); }
static void L(uint32_t a, uint32_t b) { L(a); L(b); }
static void L(uint32_t a, uint32_t b, uint32_t c) { L(a); L(b); L(c); }
static void L(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { L(a); L(b); L(c); L(d); }

// ---------------------------------------------------------------- arena y semilla
static uint8_t g_arena[1 << 18]; static size_t g_arenaUsed;
static void* arena_alloc(size_t n, uint8_t fill = 0) { n = (n + 15) & ~15u; void* p = g_arena + g_arenaUsed; g_arenaUsed += n; if (g_arenaUsed > sizeof g_arena) { printf("arena agotada\n"); ExitProcess(4); } memset(p, fill, n); return p; }
static uint32_t id_of(const void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t g_hseed;
static uint32_t H(uint32_t a, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0) {
    uint64_t x = g_hseed * 0x9E3779B97F4A7C15ull; for (uint32_t v : {a, b, c, d, e, f}) { x ^= v + 0x9E3779B9u + (x << 6) + (x >> 2); x *= 0xFF51AFD7ED558CCDull; } return (uint32_t)(x >> 32);
}
static std::mt19937_64 g_caseRng;
static uint32_t cr(uint32_t n) { return n ? (uint32_t)(g_caseRng() % n) : 0; }
template<typename T> static T& G(uintptr_t a) { return *(T*)a; }

static uint8_t g_pix[0x10000];
static uint8_t g_pal[0x440];
static std::vector<uint8_t> g_in; static size_t g_inPos; static std::vector<uint8_t> g_out;
static uint32_t g_expectD3D; static uint32_t g_streamVersion;
constexpr int32_t kExtOff = 0x34;
static uint8_t g_fakeStream[16], g_fakeSurfaceRender[16], g_fakeSurfaceDepth[16];

// ---------------------------------------------------------------- COM falsos
static void* g_texVtbl[200], *g_cubeVtbl[200], *g_surfVtbl[200], *g_devVtbl[200];
static HRESULT __stdcall Unexpected() { L(0xBAD); return (HRESULT)0x80004005; }
static void* new_obj(void** vtbl) { void** o = (void**)arena_alloc(32); o[0] = vtbl; return o; }
static void* new_tex(bool cube) { return new_obj(cube ? g_cubeVtbl : g_texVtbl); }
static ULONG __stdcall Obj_Release(void* self) { L(0xC02, id_of(self)); return 0; }
static UINT __stdcall Tex_GetLevelCount(void* self) { L(0xC0D, id_of(self)); return 1 + H(0xC0D, id_of(self)) % 9; }
static HRESULT __stdcall Tex_GetLevelDesc(void* self, UINT level, D3DSURFACE_DESC* d) {
    L(0xC11, id_of(self), level);
    memset(d, 0, sizeof *d); const uint32_t h = H(0xC11, id_of(self), level);
    d->Format = (D3DFORMAT)(h & 0xFF); d->Type = D3DRTYPE_TEXTURE; d->Usage = h >> 8 & 0xFFF; d->Pool = (D3DPOOL)(h % 5 == 0 ? 1 + (h >> 4) % 3 : 0);
    d->Width = 1 + (h >> 12) % 16; d->Height = 1 + (h >> 16) % 8; return 0;
}
static HRESULT __stdcall Tex_GetSurfaceLevel(void* self, UINT level, void** pp) { L(0xC12, id_of(self), level); *pp = new_obj(g_surfVtbl); L(id_of(*pp)); return 0; }
static HRESULT __stdcall Cube_GetCubeMapSurface(void* self, UINT face, UINT level, void** pp) { L(0xC13, id_of(self), face, level); *pp = new_obj(g_surfVtbl); L(id_of(*pp)); return 0; }
static void LogRect(const RECT* r) { if (r) L(r->left, r->top, r->right, r->bottom); else L(0xFFFFFFFF); }
static HRESULT __stdcall Dev_CreateCubeTexture(void*, UINT edge, UINT levels, DWORD usage, UINT fmt, UINT pool, void** pp, void* shared) {
    L(0xD19, edge, levels, usage); L(fmt, pool, id_of(shared));
    if (H(0xD19, edge, levels, usage, fmt) % 8 == 0) { L(0); return (HRESULT)0x8876086A; }
    *pp = new_tex(true); L(1, id_of(*pp)); return 0;
}
static HRESULT __stdcall Dev_UpdateSurface(void*, void* s, const RECT* sr, void* d, const POINT* p) { L(0xD1E, id_of(s)); LogRect(sr); L(id_of(d)); if (p) L(p->x, p->y); else L(0xFFFFFFFF); return (HRESULT)(H(0xD1E, id_of(s)) % 6 == 0 ? 0x80004005 : 0); }
static HRESULT __stdcall Dev_GetRenderTargetData(void*, void* s, void* d) { L(0xD20, id_of(s), id_of(d)); return (HRESULT)(H(0xD20, id_of(s)) % 6 == 0 ? 0x80004005 : 0); }
static HRESULT __stdcall Dev_StretchRect(void*, void* s, const RECT* sr, void* d, const RECT* dr, DWORD filter) { L(0xD22, id_of(s)); LogRect(sr); L(id_of(d)); LogRect(dr); L(filter); return (HRESULT)(H(0xD22, id_of(s)) % 6 == 0 ? 0x80004005 : 0); }
static HRESULT __stdcall Dev_ColorFill(void*, void* s, const RECT* r, DWORD color) { L(0xD23, id_of(s)); LogRect(r); L(color); return 0; }
static HRESULT __stdcall Dev_Clear(void*, DWORD count, const RECT* rects, DWORD flags, DWORD color, float z, DWORD stencil) { L(0xD2B, count); LogRect(rects); L(flags, color); uint32_t zb; memcpy(&zb, &z, 4); L(zb, stencil); return 0; }
static HRESULT __stdcall Dev_SetViewport(void*, const D3DVIEWPORT9* v) { L(0xD2F, v->X, v->Y, v->Width); L(v->Height); uint32_t a, b; memcpy(&a, &v->MinZ, 4); memcpy(&b, &v->MaxZ, 4); L(a, b); return 0; }
static void* g_devObj;

// ---------------------------------------------------------------- falsos de RenderWare (mismos para el C++ y para el EXE)
RwBool _rwD3D9PixelToRGB(RwUInt8* color, const RwUInt32* pixel, RwUInt32 format) { L(0xA01, *pixel, format); const uint32_t h = H(0xA01, *pixel, format); memcpy(color, &h, 4); return 1; }
RwBool _rwD3D9RGBToPixel(RwUInt32* pixel, const RwUInt8* color, RwUInt32 format) { uint32_t c; memcpy(&c, color, 4); L(0xA02, c, format); *pixel = H(0xA02, c, format); return 1; }
RwBool _rwD3D9SetRenderTarget(RwUInt32 index, void* surface) { L(0xA03, index, id_of(surface)); return 1; }
void _rwD3D9SetDepthStencilSurface(void* surface) { L(0xA04, id_of(surface)); }
RwBool RwD3D9SetRenderTarget(RwUInt32 index, RwRaster* raster) { L(0xA05, index, id_of(raster)); return H(0xA05, id_of(raster)) % 4 != 0; }
RwBool _rwD3D9RestoreDevice() { L(0xA06); return H(0xA06, (uint32_t)g_log.size()) % 5 != 0; }
RwBool _rwD3D9RWGetRenderState(RwInt32 state, void* value) { L(0xA07, state); *(uint32_t*)value = H(0xA07, state); return 1; }
void RwD3D9GetRenderState(RwUInt32 state, void* value) { L(0xA08, state); *(uint32_t*)value = H(0xA08, state); }
void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value) { L(0xA09, state, value); }
RwBool _rwD3D9RWSetRenderState(RwInt32 state, void* value) { L(0xA0A, state, id_of(value)); return 1; }
RwBool _rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage) { L(0xA0B, id_of(raster), stage); return 1; }
RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) { L(0xA0C, enable); return 1; }
RwBool _rwD3D9RenderStateSrcBlend(RwInt32 blend) { L(0xA0D, blend); return 1; }
RwBool _rwD3D9RenderStateDestBlend(RwInt32 blend) { L(0xA0E, blend); return 1; }
RwBool _rwD3D9Im2DRenderPrimitive(RwInt32 primType, void* vertices, RwInt32 numVertices) { L(0xA0F, primType, numVertices); const uint32_t* v = (const uint32_t*)vertices; for (int i = 0; i < 7 * 4; i++) L(v[i]); return 1; }
RwBool D3D9RasterLockCpp(RwUInt8** pixelsOut, RwRaster* raster, RwInt32 lockMode) {
    L(0xA10, id_of(raster), lockMode); if (H(0xA10, id_of(raster)) % 7 == 0) return 0;
    raster->stride = 4 * (1 + H(0xA11, id_of(raster)) % 40); L(raster->stride); *pixelsOut = g_pix; return 1;
}
RwBool D3D9RasterUnlockCpp(void* pixels, RwRaster* raster) { L(0xA12, id_of(pixels), id_of(raster)); return 1; }
// bloqueo con nivel (lectura y escritura de texturas nativas)
RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 mode) {
    L(0xB01, id_of(raster), level, mode); if (H(0xB01, id_of(raster), level) % 13 == 0) return nullptr;
    uint8_t* ext = (uint8_t*)raster + kExtOff; *(int32_t*)(ext + 0x10) = 4 * (1 + H(0xB02, id_of(raster), level) % 40); L(*(int32_t*)(ext + 0x10)); return g_pix;
}
RwRaster* RwRasterUnlock(RwRaster* raster) { L(0xB03, id_of(raster)); return raster; }
RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 mode) { L(0xB04, id_of(raster), mode); return g_pal; }
RwRaster* RwRasterUnlockPalette(RwRaster* raster) { L(0xB05, id_of(raster)); return raster; }
RwInt32 RwRasterGetNumLevels(RwRaster* raster) { L(0xB06, id_of(raster)); return 1 + H(0xB06, id_of(raster)) % 4; }
static RwRaster* make_raster(RwInt32 w, RwInt32 h, RwInt32 depth, RwInt32 flags, int32_t d3dFormat) {
    auto* r = (RwRaster*)arena_alloc(0x34 + 0x24 + 0x10);
    uint32_t hh = H(0xB07, w, h, depth, flags);
    r->parent = r; r->width = (hh % 11 == 0) ? w + 1 : w; r->height = (hh % 13 == 0) ? h + 1 : h; r->depth = depth; r->cType = (uint8_t)(flags & 0xF); r->cFlags = (uint8_t)(flags & 0xF0);
    r->cFormat = (uint8_t)((flags >> 8) & 0xFF); uint8_t* ext = (uint8_t*)r + kExtOff;
    *(uint32_t*)(ext + 0x18) = (hh % 9 == 0) ? (uint32_t)d3dFormat + 1 : (uint32_t)d3dFormat; ext[0xA] = (uint8_t)((hh >> 8) % 3 == 0 ? 0x10 | ((hh >> 12) & 1) : (hh >> 12) & 1);
    L(id_of(r)); return r;
}
RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags) { L(0xB08, width, height, depth); L(flags); if (H(0xB08, width, height, flags) % 14 == 0) return nullptr; return make_raster(width, height, depth, flags, (int32_t)g_expectD3D); }
RwBool RwRasterDestroy(RwRaster* raster) { L(0xB09, id_of(raster)); return 1; }
RwTexture* RwTextureCreate(RwRaster* raster) { L(0xB0A, id_of(raster)); if (H(0xB0A, id_of(raster)) % 14 == 0) return nullptr; auto* t = (RwTexture*)arena_alloc(sizeof(RwTexture)); t->raster = raster; t->filterAddressing = H(0xB0B, id_of(raster)); L(id_of(t), t->filterAddressing); return t; }
RwTexture* RwTextureSetName(RwTexture* t, const RwChar* name) { L(0xB0C, id_of(t)); for (int i = 0; i < 32; i += 4) L(*(const uint32_t*)(name + i)); memcpy(t->name, name, 32); return t; }
RwTexture* RwTextureSetMaskName(RwTexture* t, const RwChar* name) { L(0xB0D, id_of(t)); for (int i = 0; i < 32; i += 4) L(*(const uint32_t*)(name + i)); memcpy(t->mask, name, 32); return t; }
// flujos
RwUInt32 RwStreamRead(RwStream* s, void* buf, RwUInt32 len) {
    L(0xC01, id_of(s), len); if (H(0xC01, len, (uint32_t)g_inPos) % 30 == 0) { L(0); return len ? len - 1 : 0; }
    size_t n = std::min<size_t>(len, g_in.size() - g_inPos); if (len > sizeof g_pix) n = 0;
    if (buf) memcpy(buf, g_in.data() + g_inPos, n); g_inPos += n; L((uint32_t)n); return (RwUInt32)n;
}
RwStream* RwStreamWrite(RwStream* s, const void* buf, RwUInt32 len) {
    L(0xC03, id_of(s), len); if (H(0xC03, len, (uint32_t)g_out.size()) % 25 == 0) return nullptr;
    const uint8_t* b = (const uint8_t*)buf; if (len > sizeof g_pix) len = sizeof g_pix; g_out.insert(g_out.end(), b, b + len); return s;
}
RwBool RwStreamFindChunk(RwStream* s, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut) {
    L(0xC02, id_of(s), type); if (H(0xC02) % 12 == 0) return 0; *lengthOut = 0x100 + H(0xC04) % 0x400; *versionOut = g_streamVersion; L(*lengthOut, *versionOut); return 1;
}
RwStream* _rwStreamWriteVersionedChunkHeader(RwStream* s, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 build) { L(0xC05, id_of(s), type, size); L(version, build); if (H(0xC05) % 20 == 0) return nullptr; return s; }
// clases del proyecto que llaman estas rutinas
RwBool D3D9RasterFormat::GetRasterMemorySize(uint32* outSize, RwRaster** inRaster) { L(0xD01, id_of(*inRaster)); *outSize = 0x100 + H(0xD01) % 0x4000; L(*outSize); return 1; }
RwRaster* D3D9RasterFormat::CreateRasterFromD3DFormat(RwInt32 w, RwInt32 h, RwInt32 fmt, RwUInt32 flags) { L(0xD02, w, h, fmt); L(flags); if (H(0xD02, w, h, fmt) % 14 == 0) return nullptr; return make_raster(w, h, 16 + 16 * (H(0xD03) & 1), (RwInt32)flags | (int32_t)((H(0xD04) & 0xFF) << 8), fmt); }
int32 RwD3D9RasterDriver::CheckTextureFormatCpp(uint32 f) { L(0xD04, f); return H(0xD04, f) % 5 != 0; }
int32 RwD3D9RasterDriver::CheckTextureFormatUsage400Cpp(uint32 f) { L(0xD05, f); return H(0xD05, f) % 2; }
int32 RwD3D9RasterDriver::CheckCubeFormatUsage400Cpp(uint32 f) { L(0xD06, f); return H(0xD06, f) % 2; }
int32 D3DResourceSystem::CreateTexture(int32 w, int32 h, int32 levels, uint32 format, void** pp) { L(0xD07, w, h, levels); L(format); if (H(0xD07, w, h, format) % 8 == 0) { L(0); return (int32)0x8876086A; } *pp = new_tex(false); L(1, id_of(*pp)); return 0; }
void D3DResourceSystem::DestroyTexture(void*) {}
// los del EXE llaman a las versiones por nombre de funcion libre; las clases se desvian aparte (ver main)
static RwBool __cdecl ThunkCheckTexture(uint32 f) { return RwD3D9RasterDriver::CheckTextureFormatCpp(f); }
static RwBool __cdecl ThunkCheckTextureUsage(uint32 f) { return RwD3D9RasterDriver::CheckTextureFormatUsage400Cpp(f); }
static RwBool __cdecl ThunkCheckCube(uint32 f) { return RwD3D9RasterDriver::CheckCubeFormatUsage400Cpp(f); }
static RwBool __cdecl ThunkMemSize(uint32* o, RwRaster** r) { return D3D9RasterFormat::GetRasterMemorySize(o, r); }
static RwRaster* __cdecl ThunkCreateFromD3D(RwInt32 w, RwInt32 h, RwInt32 f, RwUInt32 fl) { return D3D9RasterFormat::CreateRasterFromD3DFormat(w, h, f, fl); }
static int32 __cdecl ThunkCreateTexture(int32 w, int32 h, int32 l, uint32 f, void** pp) { return D3DResourceSystem::CreateTexture(w, h, l, f, pp); }

#include "D3D9RasterStandards.h"
#include "D3D9RasterStandards.cpp"

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

// ---------------------------------------------------------------- construccion de escenarios
static RwRaster* mk_obj_raster(RwRaster* parent, uint32_t typeBias) {
    auto* r = (RwRaster*)arena_alloc(0x34 + 0x24 + 0x10);
    for (size_t i = 0; i < 0x34 + 0x24; i++) ((uint8_t*)r)[i] = (uint8_t)g_caseRng();
    static const uint8_t types[] = {0, 0, 0, 1, 2, 2, 2, 3, 4, 4, 4, 5, 5, 5, 6, 7};
    r->cType = types[cr(16)]; if (typeBias != 0xFF && cr(3)) r->cType = (uint8_t)typeBias;
    static const int32_t depths[] = {8, 16, 32, 32, 16, 24, 0, 8};
    r->depth = depths[cr(8)]; r->width = 1 + cr(48); r->height = 1 + cr(48);
    r->nOffsetX = (int16_t)(cr(40) - 8); r->nOffsetY = (int16_t)(cr(40) - 8);
    r->parent = parent ? parent : r; r->cpPixels = nullptr; r->palette = nullptr; r->stride = 4 * (1 + cr(40));
    uint8_t* ext = (uint8_t*)r + kExtOff; const bool cube = cr(6) == 0;
    ext[9] = cube ? (uint8_t)(1 | (cr(6) << 4)) : 0; ext[8] = (uint8_t)cr(2); ext[0xA] = (uint8_t)((cr(3) == 0 ? 1 : 0) | (cr(3) == 0 ? 0x10 : 0) | (cr(8) == 0 ? 0xA0 : 0));
    ext[0xB] = (uint8_t)cr(8); *(int32_t*)(ext + 0x10) = 4 * (1 + cr(40)); *(uint32_t*)(ext + 0x18) = cr(100);
    *(void**)(ext + 4) = g_pal; *(void**)(ext + 0x1C) = cr(2) ? (void*)(uintptr_t)(0x66660000 + cr(16)) : nullptr;
    *(void**)ext = new_tex(cube); *(void**)(ext + 0xC) = nullptr;
    return r;
}
static RwRaster* mk_chain() { // raster con cadena de padres de longitud 0..2
    const uint32_t depth = cr(4);
    RwRaster* root = mk_obj_raster(nullptr, 0xFF);
    RwRaster* cur = root;
    for (uint32_t i = 0; i < depth && i < 2; i++) { RwRaster* c = mk_obj_raster(cur, 0xFF); cur = c; }
    return cur;
}

static void reset_globals() {
    G<int32_t>(0xB4E9E0) = kExtOff;
    G<void*>(0xC97C28) = &g_devObj; G<void*>(0xC97C30) = g_fakeSurfaceRender; G<void*>(0xC97C2C) = g_fakeSurfaceDepth;
    G<uint32_t>(0xC9BFF4) = (uint32_t)g_caseRng() & (cr(2) ? 0x3300300u : 0x1000100u) ; G<uint32_t>(0xC9BF44) = cr(4) * 0x10000u;
}

enum Kind { K_SETTEX, K_CONTEXT, K_SUB, K_CLEARRECT, K_CLEAR, K_RENDER, K_SCALED, K_FAST, K_LOCKPAL, K_READ, K_WRITE, K_MIPS, K_COUNT };
static const char* kNames[K_COUNT] = {"0x4CBD40 SetTextureRaster", "0x4CB520 SetRasterContext", "0x4CBD50 SubRaster", "0x4CB4C0 ClearRect", "0x4CB4E0 Clear", "0x4CAE40 Render", "0x4CAE80 RenderScaled", "0x4CAE60 RenderFast", "0x4CA4E0 LockPalette", "0x4CD820 NativeTextureRead", "0x4CD4D0 NativeTextureWrite", "0x4CBCB0 GetMipLevels"};
static const uintptr_t kExeAddr[K_COUNT] = {0x4CBD40, 0x4CB520, 0x4CBD50, 0x4CB4C0, 0x4CB4E0, 0x4CAE40, 0x4CAE80, 0x4CAE60, 0x4CA4E0, 0x4CD820, 0x4CD4D0, 0x4CBCB0};
typedef int32_t(__cdecl* StdFn)(void*, void*, int32_t);
static const StdFn kOurFn[K_COUNT] = {
    (StdFn)&D3D9RasterStandards::SetTextureRaster, (StdFn)&D3D9RasterStandards::SetRasterContext, (StdFn)&D3D9RasterStandards::RasterSubRaster, (StdFn)&D3D9RasterStandards::RasterClearRect, (StdFn)&D3D9RasterStandards::RasterClear,
    (StdFn)&D3D9RasterStandards::RasterRender, (StdFn)&D3D9RasterStandards::RasterRenderScaled, (StdFn)&D3D9RasterStandards::RasterRenderFast, (StdFn)&D3D9RasterStandards::RasterLockPalette,
    (StdFn)&D3D9RasterStandards::NativeTextureRead, (StdFn)&D3D9RasterStandards::NativeTextureWrite, (StdFn)&D3D9RasterStandards::RasterGetMipLevels };

struct Args { void* a; void* b; int32_t c; size_t slotOffset; };
static Args setup(Kind kind, uint64_t seed) {
    g_caseRng.seed(seed); g_hseed = (uint32_t)seed * 2654435761u; g_log.clear(); g_arenaUsed = 0x100; memset(g_arena, 0, 0x100);
    memset(g_pix, 0, sizeof g_pix); memset(g_fakeStream, 0, sizeof g_fakeStream);
    for (auto& b : g_pal) b = (uint8_t)g_caseRng();
    if (cr(3) == 0) { for (int i = 0; i < 0x400; i += 4) { g_pal[i] = (uint8_t)(i / 4 % 3); g_pal[i + 1] = (uint8_t)(i / 4 % 5); g_pal[i + 2] = (uint8_t)(i / 4 % 7); g_pal[i + 3] = 255; } }
    g_out.clear(); g_in.clear(); g_inPos = 0; g_expectD3D = cr(60); { const uint32_t vsel = cr(12); g_streamVersion = vsel == 0 ? 0x33FFFu : vsel == 1 ? 0x34000u : vsel == 2 ? 0x36003u : vsel == 3 ? 0x36004u : (0x34000 + cr(0x2004)); }
    reset_globals();
    Args a{nullptr, nullptr, 0, 0};
    uint32_t* slot = (uint32_t*)arena_alloc(64); a.slotOffset = (size_t)((uint8_t*)slot - g_arena);
    switch (kind) {
    case K_SETTEX: a.a = slot; a.b = (void*)(uintptr_t)g_caseRng(); break;
    case K_CONTEXT: a.b = mk_chain(); break;
    case K_SUB: a.a = mk_obj_raster(nullptr, 0xFF); a.b = mk_obj_raster(nullptr, 0xFF); break;
    case K_CLEARRECT: case K_CLEAR: {
        G<RwRaster*>(0xB4E9E4) = mk_chain();
        int32_t* rect = (int32_t*)arena_alloc(16); rect[0] = cr(12); rect[1] = cr(12); rect[2] = cr(12); rect[3] = cr(12);
        a.b = rect; a.c = (int32_t)(cr(3) ? (uint32_t)g_caseRng() : (cr(2) ? 0u : 0xFFFFFFFFu));
        if (kind == K_CLEAR) a.b = nullptr;
        // el raster destino tiene a menudo ancho y alto pequenos coherentes con el rectangulo
        break; }
    case K_RENDER: case K_SCALED: case K_FAST: {
        G<RwRaster*>(0xB4E9E4) = mk_chain();
        a.a = cr(40) == 0 ? nullptr : mk_chain();
        int32_t* pos = (int32_t*)arena_alloc(16); pos[0] = cr(60) - 10; pos[1] = cr(60) - 10; pos[2] = 1 + cr(60); pos[3] = 1 + cr(60); a.b = pos; break; }
    case K_LOCKPAL: a.a = slot; a.b = mk_chain(); a.c = (int32_t)cr(4); break;
    case K_MIPS: a.a = slot; a.b = mk_chain(); break;
    case K_WRITE: {
        RwRaster* r = mk_chain(); auto* t = (RwTexture*)arena_alloc(sizeof(RwTexture)); t->raster = r;
        for (int i = 0; i < 32; i++) { t->name[i] = (char)g_caseRng(); t->mask[i] = (char)g_caseRng(); } t->filterAddressing = (uint32_t)g_caseRng();
        a.a = g_fakeStream; a.b = t; a.c = (int32_t)cr(1000); break; }
    case K_READ: {
        // flujo con las dos cabeceras, paleta, y niveles
        auto put = [&](const void* p, size_t n) { const uint8_t* b = (const uint8_t*)p; g_in.insert(g_in.end(), b, b + n); };
        uint32_t plat = cr(10) == 0 ? 8 : 9; put(&plat, 4);
        uint32_t fa = (uint32_t)g_caseRng(); put(&fa, 4);
        uint8_t names[64]; for (auto& b : names) b = (uint8_t)g_caseRng(); put(names, 64);
        uint32_t rf = (cr(2) ? (cr(0x100) << 8) : 0) | (cr(3) == 0 ? 0x9000u : 0u) | (cr(5) == 0 ? 0x4000u : 0u) | (cr(5) == 0 ? 0x2000u : 0u) | (cr(14) == 0 ? 0x10000u : 0u) | (cr(6) == 0 ? 0x8000u : 0u);
        put(&rf, 4);
        uint32_t d3d = g_expectD3D; put(&d3d, 4);
        uint16_t w = (uint16_t)(1 + cr(16)), h = (uint16_t)(1 + cr(16)); put(&w, 2); put(&h, 2);
        uint8_t depth = (uint8_t)(cr(3) ? 32 : 16); uint8_t numLevels = (uint8_t)cr(4); static const uint8_t types[] = {0, 4, 4, 5, 2, 0};
        uint8_t type = types[cr(6)]; uint8_t fl = (uint8_t)((cr(3) == 0 ? 8 : 0) | (cr(4) == 0 ? 2 : 0) | (cr(4) == 0 ? 4 : 0) | (cr(2) ? 1 : 0));
        put(&depth, 1); put(&numLevels, 1); put(&type, 1); put(&fl, 1);
        for (int i = 0; i < 0x800; i++) { uint8_t v = (uint8_t)(i & 3 ? (cr(40) == 0 ? g_caseRng() : 0) : (cr(4) == 0 ? 8 : 16)); g_in.push_back(v); }
        a.a = g_fakeStream; a.b = arena_alloc(8); a.c = 0; break; }
    default: break;
    }
    return a;
}

static std::vector<uint32_t> observe(int32_t ret, const Args& a) {
    std::vector<uint32_t> o; o.push_back((uint32_t)ret);
    const uint32_t* p = (const uint32_t*)g_arena; for (size_t i = 0; i < g_arenaUsed / 4; i++) o.push_back(p[i]);
    for (size_t i = 0; i < sizeof g_pix; i += 4) o.push_back(*(uint32_t*)(g_pix + i));
    for (size_t i = 0; i < sizeof g_pal; i += 4) o.push_back(*(uint32_t*)(g_pal + i));
    o.push_back((uint32_t)g_out.size()); for (size_t i = 0; i < g_out.size(); i++) o.push_back(g_out[i]);
    o.push_back((uint32_t)g_inPos); o.push_back(G<uint32_t>(0xB4E9E4));
    (void)a;
    o.insert(o.end(), g_log.begin(), g_log.end());
    return o;
}

static LONG WINAPI Crash(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en %p (acceso %p) eax=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress,
           e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr, e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
    fflush(stdout); ExitProcess(3); return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(Crash);
    if (argc < 2) { printf("uso: bench_std.exe <gta_sa.exe> [--casos N] [--semilla S] [--solo K]\n"); return 2; }
    uint64_t n = 100000, seed = 1; int only = -1;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = strtoull(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) only = atoi(argv[++i]); }
    setvbuf(stdout, nullptr, _IONBF, 0);
    unsigned int cw; _controlfp_s(&cw, _PC_24, _MCW_PC);
    if (!map_exe(argv[1])) return 2;
    for (auto* v : {g_texVtbl, g_cubeVtbl, g_surfVtbl, g_devVtbl}) for (int i = 0; i < 200; i++) v[i] = (void*)&Unexpected;
    for (auto* v : {g_texVtbl, g_cubeVtbl, g_surfVtbl}) v[2] = (void*)&Obj_Release;
    for (auto* v : {g_texVtbl, g_cubeVtbl}) { v[13] = (void*)&Tex_GetLevelCount; v[17] = (void*)&Tex_GetLevelDesc; }
    g_texVtbl[18] = (void*)&Tex_GetSurfaceLevel; g_cubeVtbl[18] = (void*)&Cube_GetCubeMapSurface;
    g_devVtbl[25] = (void*)&Dev_CreateCubeTexture; g_devVtbl[30] = (void*)&Dev_UpdateSurface; g_devVtbl[32] = (void*)&Dev_GetRenderTargetData; g_devVtbl[34] = (void*)&Dev_StretchRect;
    g_devVtbl[35] = (void*)&Dev_ColorFill; g_devVtbl[43] = (void*)&Dev_Clear; g_devVtbl[47] = (void*)&Dev_SetViewport; g_devObj = g_devVtbl;
    // desvios del EXE hacia los mismos falsos que usa el C++
    divert(0x7FF070, (void*)&_rwD3D9PixelToRGB); divert(0x7FEE20, (void*)&_rwD3D9RGBToPixel); divert(0x7F5F20, (void*)&_rwD3D9SetRenderTarget); divert(0x7F5EF0, (void*)&_rwD3D9SetDepthStencilSurface);
    divert(0x7F9E90, (void*)&RwD3D9SetRenderTarget); divert(0x7F8300, (void*)&_rwD3D9RestoreDevice); divert(0x7FD810, (void*)&_rwD3D9RWGetRenderState); divert(0x7FC320, (void*)&RwD3D9GetRenderState);
    divert(0x7FC2D0, (void*)&RwD3D9SetRenderState); divert(0x7FE420, (void*)&_rwD3D9RWSetRenderState); divert(0x7FDCD0, (void*)&_rwD3D9RWSetRasterStage); divert(0x7FE0A0, (void*)&_rwD3D9RenderStateVertexAlphaEnable);
    divert(0x7FDC30, (void*)&_rwD3D9RenderStateSrcBlend); divert(0x7FDC80, (void*)&_rwD3D9RenderStateDestBlend); divert(0x7FBB00, (void*)&_rwD3D9Im2DRenderPrimitive);
    divert(0x4C9F90, (void*)&D3D9RasterLockCpp); divert(0x4CA290, (void*)&D3D9RasterUnlockCpp);
    divert(0x7FB2D0, (void*)&RwRasterLock); divert(0x7FAEC0, (void*)&RwRasterUnlock); divert(0x7FB0E0, (void*)&RwRasterLockPalette); divert(0x7FAFF0, (void*)&RwRasterUnlockPalette); divert(0x7FB160, (void*)&RwRasterGetNumLevels);
    divert(0x7FB230, (void*)&RwRasterCreate); divert(0x7FB020, (void*)&RwRasterDestroy); divert(0x7F37C0, (void*)&RwTextureCreate); divert(0x7F38A0, (void*)&RwTextureSetName); divert(0x7F3910, (void*)&RwTextureSetMaskName);
    divert(0x7EC9D0, (void*)&RwStreamRead); divert(0x7ECB30, (void*)&RwStreamWrite); divert(0x7ED2D0, (void*)&RwStreamFindChunk); divert(0x7ED270, (void*)&_rwStreamWriteVersionedChunkHeader);
    divert(0x4CBDE0, (void*)&ThunkCheckTexture); divert(0x4CBF80, (void*)&ThunkCheckTextureUsage); divert(0x4CC020, (void*)&ThunkCheckCube); divert(0x4CD360, (void*)&ThunkMemSize); divert(0x4CD050, (void*)&ThunkCreateFromD3D); divert(0x730510, (void*)&ThunkCreateTexture);

    uint64_t fails = 0;
    for (int k = 0; k < K_COUNT; k++) {
        if (only >= 0 && only != k) continue;
        uint64_t kFails = 0, okRet = 0; std::map<uint32_t, uint64_t> cov;
        for (uint64_t it = 0; it < n; it++) {
            const uint64_t cs = seed * 1000003 + it * 16 + (uint64_t)k;
            Args a = setup((Kind)k, cs);
            const int32_t ra = ((StdFn)kExeAddr[k])(a.a, a.b, a.c);
            const std::vector<uint32_t> oa = observe(ra, a);
            const std::vector<uint32_t> la = g_log;
            Args a2 = setup((Kind)k, cs); (void)a2;
            const int32_t rb = kOurFn[k](a2.a, a2.b, a2.c);
            const std::vector<uint32_t> ob = observe(rb, a2);
            if (ra) okRet++;
            for (uint32_t t : {0xA01u, 0xA02u, 0xA05u, 0xA0Fu, 0xA10u, 0xC11u, 0xC12u, 0xC13u, 0xD19u, 0xD1Eu, 0xD20u, 0xD22u, 0xD23u, 0xD2Bu, 0xD2Fu, 0xB08u, 0xD02u, 0xD07u, 0xA12u, 0xB01u, 0xB0Au, 0xB0Cu, 0xC05u, 0xB09u, 0xB05u}) if (std::find(g_log.begin(), g_log.end(), t) != g_log.end()) cov[t]++;
            if (oa != ob) {
                kFails++;
                if (kFails <= 3) {
                    printf("DIFERENCIA %s caso=%llu: exe ret=%d (%zu valores) ours ret=%d (%zu valores)\n", kNames[k], (unsigned long long)cs, ra, oa.size(), rb, ob.size());
                    for (size_t i = 0; i < std::min(oa.size(), ob.size()); i++) if (oa[i] != ob[i]) { printf("  primer valor distinto en [%zu]: exe=%08x ours=%08x\n", i, oa[i], ob[i]); break; }
                }
            }
            (void)la;
        }
        printf("%-28s %llu casos (devuelven != 0: %llu), %llu diferencias; cobertura de marcas:", kNames[k], (unsigned long long)n, (unsigned long long)okRet, (unsigned long long)kFails);
        for (auto& kv : cov) printf(" %x=%llu", kv.first, (unsigned long long)kv.second);
        printf("\n"); fails += kFails;
    }
    printf("RESULTADO: %s (%llu diferencias)\n", fails ? "HAY DIFERENCIAS" : "SIN DIFERENCIAS", (unsigned long long)fails);
    return fails ? 1 : 0;
}
