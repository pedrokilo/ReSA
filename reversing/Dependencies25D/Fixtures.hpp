#pragma once
// Fixtures deterministas del banco Dependencies25D (20 funciones de biblioteca RenderWare / controlador D3D9). Solo C++ estandar (sin dependencias del juego):
// fixture_oracle.py repite exactamente estas formulas y tablas. Todos los valores float/enteros se dan como patrones de bits. 1024 casos por funcion.
#include <array>
#include <cstdint>
namespace Dependencies25DFix {
using u32 = std::uint32_t;
constexpr unsigned kCases = 1024;

// Mezcla (finalizador de murmur3 de 32 bits) y generador por (caso, clave).
constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }

// 32 enteros de estres (0, 1, limites de signo, mascaras, patrones tipicos de basura, bits de float: NaN, inf, FLT_MAX, 1.0f).
inline constexpr std::array<u32, 32> kSpecial{
    0x00000000, 0x00000001, 0x00000002, 0x0000007F, 0x000000FF, 0x00000100, 0x0000FFFF, 0x00010000,
    0x00FF0000, 0x01010101, 0x7FFFFFFF, 0x80000000, 0x80000001, 0xFFFFFFFF, 0xFFFFFFFE, 0xDEADBEEF,
    0xCDCDCDCD, 0xA5A5A5A5, 0x5A5A5A5A, 0x12345678, 0x7F7FFFFF, 0xFF800000, 0x7FC00001, 0x3F800000,
    0x00800000, 0x00000003, 0x0000000F, 0x40000000, 0x0FFFFFFF, 0x10000000, 0xC0000000, 0x00008000};
constexpr u32 Special(u32 c, u32 k) { return kSpecial[Rand(c, k) % 32]; }
// 16 HRESULT (exito, S_FALSE, positivos, limites de signo, E_FAIL, D3DERR_INVALIDCALL, E_INVALIDARG, E_OUTOFMEMORY...)
inline constexpr std::array<u32, 16> kHr{
    0x00000000, 0x00000001, 0x00000002, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0x80004005, 0x8876086C,
    0x88760868, 0x00010000, 0x80070057, 0x8007000E, 0x80004001, 0x8000FFFF, 0xFFFF0000, 0xFFFFFFFE};
// valores devueltos por las funciones estandar de RW (RwBool de 32 bits)
inline constexpr std::array<u32, 8> kRet8{0x00000000, 0x00000001, 0x00000002, 0xFFFFFFFF, 0x80000000, 0x00000100, 0x0000FF00, 0x7FFFFFFF};

// Posiciones en el arena (desplazamientos desde Mem()): el motor falso, las regiones de modulo y la ventana registrada [0x100, 0x200).
inline constexpr std::array<unsigned, 2> kEng{0x10, 0x20};
inline constexpr std::array<unsigned, 3> kTexBase{0x100, 0x140, 0x180};
inline constexpr std::array<unsigned, 3> kRasBase{0x120, 0x150, 0x180};

struct Seeds { u32 eax, ecx, edx; };
constexpr Seeds SeedsOf(unsigned c) { return {Rand(c, 900), Rand(c, 901), Rand(c, 902)}; }

// ---- RwTextureGetMipmapping (0x7F3550), RwTextureGetAutoMipmapping (0x7F3580), RwTextureRasterGenerateMipmaps (0x7F3CD0)
struct TexFx { unsigned eng, texBase; u32 mip, autom, cb, raster, image, rv; };
constexpr TexFx TexCase(unsigned c) {
    TexFx f{};
    f.eng = kEng[c % 2]; f.texBase = kTexBase[c / 2 % 3];
    f.mip = Special(c, 1); f.autom = Special(c, 2); f.cb = Special(c, 3);
    f.raster = Rand(c, 4); f.image = Rand(c, 5);
    f.rv = (c % 4 == 0) ? 0u : Special(c, 6);   // valor que devuelve el callback de generacion (la funcion devuelve rv != 0)
    return f;
}
// ---- RwRasterGetCurrentContext (0x7FAE60)
struct RasFx { unsigned eng, rasBase; int sp; };
constexpr RasFx RasCase(unsigned c) { return {kEng[c % 2], kRasBase[c / 2 % 3], int(c / 6 % 18) - 4}; }

// ---- RwTextureRegisterPlugin / RwTexDictionaryRegisterPlugin / RwRasterRegisterPlugin (cinco argumentos) y RwTextureRegisterPluginStream (cuatro)
struct PlugFx { u32 size, id, f1, f2, f3, ret; };
constexpr PlugFx PlugCase(unsigned c) { return {Special(c, 1), Special(c, 2), Rand(c, 3), Rand(c, 4), Rand(c, 5), Special(c, 6)}; }

// ---- RwD3D9DynamicVertexBufferDestroy (0x7F5AE0)
inline constexpr std::array<u32, 4> kVbPool{0x11110000, 0x22220000, 0x33330000, 0x00000000};
inline constexpr unsigned kDynStride = 0x1C, kDynBase = 0x100;
struct DynFx { unsigned n, mul, argSel; unsigned vbSel[6]; u32 arg; };
constexpr DynFx DynCase(unsigned c) {
    DynFx f{};
    f.n = c % 7; f.mul = c / 7 % 6 + 1; f.argSel = c / 42 % 6;
    for (unsigned i = 0; i < 6; ++i) f.vbSel[i] = Rand(c, 10 + i) % 4;
    f.arg = f.argSel < 4 ? kVbPool[f.argSel] : (f.argSel == 4 ? 0x44440000u : 0xFFFFFFFFu);
    return f;
}
constexpr unsigned DynSlot(const DynFx& f, unsigned i) { return (i * f.mul) % 7; }

// ---- COM: RwD3D9DynamicVertexBufferUnlock (0x7F5C90), RwD3D9DeleteVertexShader (0x7FAC90), RwD3D9DeletePixelShader (0x7FACF0)
struct ObjFx { unsigned sel; u32 hr; u32 cnt0, cnt1; };   // sel: 0 -> objeto en 0x100, 1 -> objeto en 0x120
constexpr ObjFx ObjCase(unsigned c) { return {c / 16 % 2, kHr[c % 16], Rand(c, 1), Rand(c, 2)}; }
// ---- _rwD3D9SetDepthStencilSurface (0x7F5EF0)
inline constexpr std::array<u32, 4> kSurface{0x00000000, 0x00A00010, 0x00B00020, 0xFFFFFFFF};
struct DsFx { u32 cur, arg; unsigned mut; u32 hr; };
constexpr DsFx DsCase(unsigned c) { return {kSurface[c % 4], kSurface[c / 4 % 4], c / 16 % 3, kHr[c / 48 % 16]}; }
// ---- RwD3D9CreatePixelShader (0x7FACC0)
struct PsFx { u32 func, token; unsigned mode, slot; u32 hr; };
constexpr PsFx PsCase(unsigned c) { return {(c % 7 == 0) ? 0u : Rand(c, 1), Rand(c, 2), c / 16 % 3, c / 48 % 4, kHr[c % 16]}; }

// ---- RwRasterLockPalette (0x7FB0E0), RwRGBAToPixel (0x803740): funciones estandar de RW con ranura de salida compartida
struct StdFx { unsigned eng; u32 a, b, ret, wmode, token, rgba; };
constexpr StdFx StdCase(unsigned c) { return {kEng[c / 24 % 2], Rand(c, 1), Special(c, 2), kRet8[c % 8], c / 8 % 3, Rand(c, 3), Rand(c, 4)}; }

// ---- _rwFreeListCreate -> RwFreeListCreate (0x801980, 4 argumentos) y RwFreeListCreateAndPreallocateSpace (0x801B70, 6 argumentos)
struct FreeFx { u32 a[6]; u32 ret; };
constexpr FreeFx FreeCase(unsigned c) {
    FreeFx f{};
    for (unsigned i = 0; i < 4; ++i) f.a[i] = Special(c, 1 + i);
    f.a[4] = Rand(c, 5); f.a[5] = Rand(c, 6); f.ret = Rand(c, 7);
    return f;
}
// ---- _rwDeviceRegisterPlugin (0x7F5F60)
struct DevFx { u32 ret; };
constexpr DevFx DevCase(unsigned c) { return {Special(c, 1)}; }

// ---- _rwObjectHasFrameReleaseFrame (0x804F40)
inline constexpr unsigned kFramePrev = 0x100, kFrameNext = 0x110, kFrameObj = 0x130;
struct FrameFx { unsigned topo; u32 parent; u32 junk[6]; };
constexpr FrameFx FrameCase(unsigned c) {
    FrameFx f{};
    f.topo = c % 8;
    const unsigned sel = c / 8 % 5;
    f.parent = sel == 0 ? 0u : sel == 1 ? 1u : sel == 2 ? 0xFFFFFFFFu : sel == 3 ? 0x80000000u : 0x12345678u;
    for (unsigned i = 0; i < 6; ++i) f.junk[i] = Rand(c, 40 + i);
    return f;
}
}  // namespace Dependencies25DFix
