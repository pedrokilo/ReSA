// Pruebas de _rwTextureDefaultRead (0x7F40F0) y _rwTextureDefaultMipmapGeneration (0x7F5140) con colaboradores simulados (sin dispositivo).
struct RwImage { RwInt32 flags, width, height, depth, stride; RwUInt8* cpPixels; RwRGBA* palette; };
struct RwRaster { RwRaster* parent; RwUInt8* cpPixels; RwUInt8* palette; RwInt32 width, height, depth, stride; RwInt16 nOffsetX, nOffsetY; RwUInt8 cType, cFlags, cPrivateFlags, cFormat; RwUInt8 pad[0x10]; };
static_assert(offsetof(RwRaster, cFormat) == 0x23 && offsetof(RwRaster, height) == 0x10);
static_assert(offsetof(RwImage, depth) == 0xC && offsetof(RwImage, palette) == 0x18);
struct TextureGlobalsLayout {
    RwLLLink texDictList; RwFreeList* textureFreeList; RwFreeList* texDictFreeList; RwTexDictionary* currentTexDict;
    void* readCallBack; void* findCallBack; RwBool mipmapping; RwBool autoMipmapping; RwUInt8 pad1[8]; void* mipmapGenerationCallBack;
    RwBool (*mipmapNameCallBack)(RwChar*, RwChar*, RwUInt8, RwInt32);
};
static TextureGlobalsLayout& TextureGlobals() { return *reinterpret_cast<TextureGlobalsLayout*>(reinterpret_cast<uint8_t*>(RwEngineInstance) + StaticRef<RwInt32>(0xC97B4C)); }
static RwPluginRegistry& TextureTKList = StaticRef<RwPluginRegistry>(0x8E23CC);
constexpr uint32_t kTextureReadDefaultMaskSource = 0xC97904;
static int g_cFormat, g_levels, g_imgFlags, g_texW;
static char* __cdecl M_strncpy(char* d, const char* s, size_t n) { return strncpy(d, s, n); }
static size_t __cdecl M_strlen(const char* s) { return strlen(s); }
static RwImage* newImage(RwInt32 w, RwInt32 h, RwInt32 d) { RwImage* i = (RwImage*)bump(sizeof(RwImage)); i->width = w; i->height = h; i->depth = d; return i; }
static RwImage* __cdecl M_ReadImage(const char* name, const char* mask, RwUInt32 type, RwInt32* w, RwInt32* h, RwInt32* d, RwInt32* fl) {
    L("readimage '%s' '%s' type=%x w=%d h=%d", name, mask, type, *w, *h);
    if (failNow()) return nullptr;
    *w = g_texW; *h = g_texW; *d = (g_cFormat & 0x60) ? 8 : 32; *fl = g_imgFlags; return newImage(*w, *h, *d);
}
static RwRaster* __cdecl M_RasterCreate(RwInt32 w, RwInt32 h, RwInt32 d, RwInt32 fl) {
    RwRaster* r = (RwRaster*)bump(sizeof(RwRaster)); memset(r, 0, sizeof *r); r->width = w; r->height = h; r->depth = d; r->cFormat = (RwUInt8)g_cFormat; L("rastercreate %d %d %d %x -> %p", w, h, d, fl, r); return failNow() ? nullptr : r;
}
static void __cdecl M_RasterDestroy(RwRaster* r) { L("rasterdestroy %p", r); }
static void __cdecl M_ImgDestroy(RwImage* i) { if (i->palette) { L("imgdestroy %p (con paleta)", i); i->palette = nullptr; } else L("imgdestroy %p", i); }
static RwImage* __cdecl M_Gamma(RwImage* i) { L("gamma %p", i); return i; }
static RwRaster* __cdecl M_SetFromImage(RwRaster* r, RwImage* i) { L("setfromimage %p %p", r, i); return failNow() ? nullptr : r; }
static RwBool __cdecl M_MakePalette(void* pal, const RwImage* keep, RwImage** imgs, RwInt32 n, RwInt32 bits) { L("makepalette keep=%p n=%d bits=%d", keep, n, bits); for (int k = 0; k < n && k < 12; ++k) L(" img %p", imgs[k]); return failNow() ? 0 : 1; }
static RwTexture* __cdecl M_ReadMipmapped(const char* name, const char* mask) { L("readmipmapped '%s' '%s'", name, mask ? mask : "(null)"); return (RwTexture*)0x4242; }
static RwTexture* __cdecl M_SetMaskName(RwTexture* t, const char* m) { L("setmaskname %p %p", t, m); return t; }
static RwImage* __cdecl M_ImgCreate(RwInt32 w, RwInt32 h, RwInt32 d) { RwImage* i = newImage(w, h, d); L("imgcreate %d %d %d -> %p", w, h, d, i); return failNow() ? nullptr : i; }
static RwImage* __cdecl M_ImgAlloc(RwImage* i) { L("imgalloc %p", i); return failNow() ? nullptr : i; }
static RwImage* __cdecl M_ImgFromRaster(RwImage* i, RwRaster* r) { L("imgfromraster %p %p", i, r); return i; }
static RwImage* __cdecl M_ImgCopy(RwImage* d, const RwImage* s) { L("imgcopy %p %p", d, s); return d; }
static RwInt32 __cdecl M_NumLevels(RwRaster* r) { L("numlevels -> %d", g_levels); return g_levels; }
static RwUInt8* __cdecl M_Lock(RwRaster* r, RwUInt8 level, RwUInt8 mode) {
    L("lock %p level=%d mode=%d", r, level, mode); if (failNow()) return nullptr;
    if (mode == 2 && level > 0) { r->width = (r->width + 1) / 2; r->height = (r->height + 1) / 2; } return (RwUInt8*)0x1234;
}
static void* __cdecl M_Unlock(RwRaster* r) { L("unlock %p", r); return r; }
static RwImage* __cdecl M_Resample(const RwImage* s, RwInt32 w, RwInt32 h) { RwImage* i = newImage(w, h, 32); L("resample %p %d %d -> %p", s, w, h, i); return failNow() ? nullptr : i; }
static RwBool g_nameCbCalls;
static RwBool __cdecl M_NameCb(RwChar* n, RwChar* m, RwUInt8 lvl, RwInt32 fmt) { L("namecb '%s' '%s' %d %d", n, m, lvl, fmt); return 1; }
#define _rwTextureReadImage M_ReadImage
#define RwRasterCreate M_RasterCreate
#define RwRasterDestroy M_RasterDestroy
#define RwImageDestroy M_ImgDestroy
#define RwImageGammaCorrect M_Gamma
#define RwRasterSetFromImage M_SetFromImage
#define _rwTextureMakePalette M_MakePalette
#define _rwTextureReadMipmapped M_ReadMipmapped
#define RwTextureSetMaskName M_SetMaskName
#define RwImageCreate M_ImgCreate
#define RwImageAllocatePixels M_ImgAlloc
#define RwImageSetFromRaster M_ImgFromRaster
#define RwImageCopy M_ImgCopy
#define RwRasterGetNumLevels M_NumLevels
#define RwRasterLock M_Lock
#define RwRasterUnlock M_Unlock
#define RwImageCreateResample M_Resample
#define rwstrncpy M_strncpy
#define rwstrlen M_strlen
namespace {
#include "core_texrw.inc"
}
static void installTexMocks() {
    *(void**)(ENG + 0xFC) = (void*)M_strncpy; *(void**)(ENG + 0x120) = (void*)M_strlen;
    patch(0x7F4690, (void*)M_ReadImage); patch(0x7FB230, (void*)M_RasterCreate); patch(0x7FB020, (void*)M_RasterDestroy); patch(0x802740, (void*)M_ImgDestroy);
    patch(0x803E30, (void*)M_Gamma); patch(0x804290, (void*)M_SetFromImage); patch(0x7F44C0, (void*)M_MakePalette); patch(0x7F4AE0, (void*)M_ReadMipmapped);
    patch(0x7F3910, (void*)M_SetMaskName); patch(0x8026E0, (void*)M_ImgCreate); patch(0x8027A0, (void*)M_ImgAlloc); patch(0x804250, (void*)M_ImgFromRaster);
    patch(0x803950, (void*)M_ImgCopy); patch(0x7FB160, (void*)M_NumLevels); patch(0x7FB2D0, (void*)M_Lock); patch(0x7FAEC0, (void*)M_Unlock); patch(0x80CD10, (void*)M_Resample);
}
static void testTexRead(int n, uint32_t seed0) {
    StaticRef<RwInt32>(0xC97B4C) = 0x400;
    static const char* names[] = { "wall", "", "tex_0123456789abcdef0123456789abcdef", "a" };
    std::string longName(300, 'x');
    for (int t = 0; t < n; ++t) {
        const char* nm = nullptr; const char* mk = nullptr; int failAt = (t % 3 == 0) ? 0 : (int)(t % 9);
        auto setup = [&]() {
            TextureGlobalsLayout& g = TextureGlobals(); memset(&g, 0, sizeof g);
            g.textureFreeList = M_FLCreate(0x58, 16, 4, 0x30006); g.mipmapping = (R(4) == 0); g.mipmapNameCallBack = R(2) ? M_NameCb : nullptr;
            static const int fm[] = { 0, 0x10, 0x20, 0x40, 0x60, 0x30, 0x50, 0x05 }; g_cFormat = fm[R(8)]; g_texW = 16 << R(3); g_imgFlags = R(2) ? 0x1000 : 0x8004;
            nm = R(8) == 0 ? longName.c_str() : names[R(4)]; mk = R(3) == 0 ? nullptr : (R(8) == 0 ? longName.c_str() : names[R(4)]);
        };
        auto fin = [&](RwTexture* r) { return (uintptr_t)r; };
        pair("TextureDefaultRead", 9000 + t + seed0, failAt, setup,
            [&]() { return fin(((RwTexture*(__cdecl*)(const char*, const char*))0x7F40F0)(nm, mk)); },
            [&]() { return fin(_rwTextureDefaultRead(nm, mk)); });
    }
}
static void testMipGen(int n, uint32_t seed0) {
    StaticRef<RwInt32>(0xC97B4C) = 0x400;
    for (int t = 0; t < n; ++t) {
        RwRaster* ras = nullptr; RwImage* img = nullptr; int failAt = (t % 3 == 0) ? 0 : (int)(t % 14);
        auto setup = [&]() {
            static const int fm[] = { 0, 0x10, 0x20, 0x40, 0x60, 0x30, 0x50, 0x05 }; g_cFormat = fm[R(8)]; g_levels = R(5) + (R(8) == 0 ? 0 : 1);
            ras = (RwRaster*)lowAlloc(sizeof(RwRaster)); memset(ras, 0, sizeof *ras); ras->width = 8 << R(3); ras->height = 8 << R(3); ras->depth = 32; ras->cFormat = (RwUInt8)g_cFormat;
            int k = R(3); img = (k == 0) ? nullptr : (RwImage*)lowAlloc(sizeof(RwImage)); if (img) { memset(img, 0, sizeof *img); img->width = ras->width; img->height = ras->height; img->depth = (k == 1) ? 32 : 8; }
        };
        auto fin = [&](RwRaster* r) { L("cformat=%02x w=%d h=%d", ras->cFormat, ras->width, ras->height); return (uintptr_t)r; };
        pair("TextureMipmapGeneration", 12000 + t + seed0, failAt, setup,
            [&]() { return fin(((RwRaster*(__cdecl*)(RwRaster*, RwImage*))0x7F5140)(ras, img)); },
            [&]() { return fin(_rwTextureDefaultMipmapGeneration(ras, img)); });
    }
}
