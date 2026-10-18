#pragma once
// Escenas, falsos y compuertas del banco Dependencies25D, compartidos por LiveComparison.hpp (dentro del juego) y offline/main.cpp (bytes del EXE en un proceso hijo).
// Requiere antes: <windows.h>, <vector>, <string>, <cstring>, <cstdint> y `json` (nlohmann). Sin dependencias del juego.
//
// Mundo falso: un arena propio (kMem bytes + 16 de guarda a cada lado) donde viven el motor RenderWare falso (RwGlobals con la tabla stdFunc), las regiones de los modulos
// (textura, raster), los objetos COM falsos (bufer de vertices, sombreadores, dispositivo, superficies) y los nodos de la lista de buferes dinamicos. Los seis punteros
// globales del motor que leen o escriben las funciones (0xC97B24, 0xC97B4C, 0xC980D8, 0xC97B8C, 0xC9808C, 0xC97C28) se apuntan al arena durante cada llamada y se
// restauran justo despues (antes de escribir en el registro). Los colaboradores del EXE que estas funciones llaman por direccion (0x8084A0, 0x8088E0, 0x8019B0, 0x4C9AB0)
// se sustituyen con EntryPatch por falsos trazados. Ningun objeto real del motor se toca.
#include "Fixtures.hpp"
namespace Dependencies25DScene {
using namespace Dependencies25DFix;
using u8 = std::uint8_t;

inline constexpr std::size_t kMem = 0x300, kGuard = 16;
inline constexpr unsigned kDefaultDevice = 0xE0;   // objeto de dispositivo falso por omision (fuera de la ventana registrada)
alignas(16) inline std::array<u8, kMem + 2 * kGuard> arenaStore{};
inline u8* ArenaMem() { return arenaStore.data() + kGuard; }
inline u32 Addr(unsigned off) { return u32(reinterpret_cast<std::uintptr_t>(ArenaMem()) + off); }
inline u32 Enc(u32 p) { const u32 b = Addr(0); return (p >= b && p < b + kMem) ? p - b : p; }
inline void W32(unsigned off, u32 v) { std::memcpy(ArenaMem() + off, &v, 4); }
inline u32 R32(unsigned off) { u32 v{}; std::memcpy(&v, ArenaMem() + off, 4); return v; }
inline std::string HexOf(const void* p, std::size_t n) {
    constexpr char digits[] = "0123456789abcdef";
    const auto* b = static_cast<const u8*>(p);
    std::string out; out.reserve(n * 2);
    for (std::size_t i = 0; i < n; ++i) { out += digits[b[i] >> 4]; out += digits[b[i] & 15]; }
    return out;
}
inline std::uint64_t Fnv(const void* p, std::size_t n, std::uint64_t h = 1469598103934665603ull) {
    const auto* b = static_cast<const u8*>(p);
    for (std::size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; }
    return h;
}

// ---- globales del motor
inline constexpr std::uintptr_t kEngineInstance = 0xC97B24, kTextureOffset = 0xC97B4C, kRasterOffset = 0xC980D8, kDynamicList = 0xC97B8C, kDepthSurface = 0xC9808C, kDevicePtr = 0xC97C28;
struct EngineGlobals { u32 eng, tex, ras, dyn, ds, dev; };
inline u32& GlobalWord(std::uintptr_t a) { return *reinterpret_cast<u32*>(a); }
inline EngineGlobals ReadGlobals() { return {GlobalWord(kEngineInstance), GlobalWord(kTextureOffset), GlobalWord(kRasterOffset), GlobalWord(kDynamicList), GlobalWord(kDepthSurface), GlobalWord(kDevicePtr)}; }
inline void WriteGlobals(const EngineGlobals& g) { GlobalWord(kEngineInstance) = g.eng; GlobalWord(kTextureOffset) = g.tex; GlobalWord(kRasterOffset) = g.ras; GlobalWord(kDynamicList) = g.dyn; GlobalWord(kDepthSurface) = g.ds; GlobalWord(kDevicePtr) = g.dev; }
inline bool GlobalsEqual(const EngineGlobals& a, const EngineGlobals& b) { return std::memcmp(&a, &b, sizeof(a)) == 0; }
inline json GlobalsJson(const EngineGlobals& g) { return {{"eng", Enc(g.eng)}, {"tex", g.tex}, {"ras", g.ras}, {"dyn", Enc(g.dyn)}, {"ds", g.ds}, {"dev", Enc(g.dev)}}; }

// ---- estado de los falsos
struct Behaviour { u32 hr{}, ret{}, token{}, rv{}, cur{}; unsigned mode{}, mut{}, wmode{}; };
inline Behaviour Beh;
inline std::vector<u32> traceLog;
inline unsigned trapCount{};
inline void* fakeVtbl[128];

// ---- falsos cdecl (colaboradores del EXE y funciones estandar de RW)
inline int __cdecl StubAddPlugin(void* reg, int size, u32 id, void* c, void* d, void* cp) {                      // 0x8084A0 _rwPluginRegistryAddPlugin
    traceLog.insert(traceLog.end(), {1u, Enc(u32(reinterpret_cast<std::uintptr_t>(reg))), u32(size), id, Enc(u32(reinterpret_cast<std::uintptr_t>(c))), Enc(u32(reinterpret_cast<std::uintptr_t>(d))), Enc(u32(reinterpret_cast<std::uintptr_t>(cp)))});
    return int(Beh.ret);
}
inline int __cdecl StubAddStream(void* reg, u32 id, void* r, void* w, void* g) {                                  // 0x8088E0 _rwPluginRegistryAddPluginStream
    traceLog.insert(traceLog.end(), {2u, Enc(u32(reinterpret_cast<std::uintptr_t>(reg))), id, Enc(u32(reinterpret_cast<std::uintptr_t>(r))), Enc(u32(reinterpret_cast<std::uintptr_t>(w))), Enc(u32(reinterpret_cast<std::uintptr_t>(g)))});
    return int(Beh.ret);
}
inline void* __cdecl StubFree(int a1, int a2, int a3, int a4, void* a5, u32 a6) {                                  // 0x8019B0 RwFreeListCreateAndPreallocateSpace
    traceLog.insert(traceLog.end(), {3u, u32(a1), u32(a2), u32(a3), u32(a4), Enc(u32(reinterpret_cast<std::uintptr_t>(a5))), a6});
    return reinterpret_cast<void*>(std::uintptr_t(Beh.ret));
}
inline int __cdecl StubDeviceReg() {                                                                              // 0x4C9AB0 registro del plugin de raster D3D9
    traceLog.push_back(4u);
    return int(Beh.ret);
}
inline void* __cdecl StubGen(void* raster, void* image) {                                                         // callback de generacion de mipmaps
    traceLog.insert(traceLog.end(), {10u, Enc(u32(reinterpret_cast<std::uintptr_t>(raster))), Enc(u32(reinterpret_cast<std::uintptr_t>(image)))});
    return reinterpret_cast<void*>(std::uintptr_t(Beh.rv));
}
inline int __cdecl StdLock(void* out, void* raster, int mode) {                                                    // stdFunc[rwSTANDARDRASTERLOCKPALETTE]
    const u32 init = *static_cast<u32*>(out);
    traceLog.insert(traceLog.end(), {30u, Enc(u32(reinterpret_cast<std::uintptr_t>(raster))), u32(mode), init});
    if (Beh.wmode == 0 || (Beh.wmode == 2 && Beh.ret != 0)) *static_cast<u32*>(out) = Beh.token;
    return int(Beh.ret);
}
inline int __cdecl StdRgb(void* out, void* in, int format) {                                                       // stdFunc[rwSTANDARDRGBTOPIXEL]
    const u32 init = *static_cast<u32*>(out), rgba = *static_cast<u32*>(in);
    traceLog.insert(traceLog.end(), {31u, Enc(u32(reinterpret_cast<std::uintptr_t>(in))), u32(format), init, rgba});
    if (Beh.wmode == 0 || (Beh.wmode == 2 && Beh.ret != 0)) *static_cast<u32*>(out) = (rgba ^ u32(format)) + Beh.token;
    return int(Beh.ret);
}
inline int __cdecl StdTrap() { ++trapCount; return 0; }
// ---- falsos COM (stdcall; this primero)
inline u32 __stdcall ComTrap() { ++trapCount; return 0x7FFFFFFFu; }
inline u32 __stdcall ComRelease(void* self) {                 // slot 2 IUnknown::Release
    traceLog.insert(traceLog.end(), {21u, Enc(u32(reinterpret_cast<std::uintptr_t>(self)))});
    --*reinterpret_cast<u32*>(static_cast<u8*>(self) + 4);
    return Beh.hr;
}
inline u32 __stdcall ComUnlock(void* self) {                  // slot 12 IDirect3DVertexBuffer9::Unlock
    traceLog.insert(traceLog.end(), {20u, Enc(u32(reinterpret_cast<std::uintptr_t>(self)))});
    ++*reinterpret_cast<u32*>(static_cast<u8*>(self) + 4);
    return Beh.hr;
}
inline u32 __stdcall ComSetDepth(void* self, void* surface) {  // slot 39 IDirect3DDevice9::SetDepthStencilSurface
    traceLog.insert(traceLog.end(), {22u, Enc(u32(reinterpret_cast<std::uintptr_t>(self))), Enc(u32(reinterpret_cast<std::uintptr_t>(surface))), GlobalWord(kDepthSurface)});
    ++*reinterpret_cast<u32*>(static_cast<u8*>(self) + 4);
    if (Beh.mut == 1) GlobalWord(kDepthSurface) = 0xBAD0BAD0u;
    if (Beh.mut == 2) GlobalWord(kDevicePtr) = 0u;
    return Beh.hr;
}
inline u32 __stdcall ComCreatePixel(void* self, const void* func, void** out) {   // slot 106 IDirect3DDevice9::CreatePixelShader
    traceLog.insert(traceLog.end(), {23u, Enc(u32(reinterpret_cast<std::uintptr_t>(self))), Enc(u32(reinterpret_cast<std::uintptr_t>(func))), Enc(u32(reinterpret_cast<std::uintptr_t>(out)))});
    ++*reinterpret_cast<u32*>(static_cast<u8*>(self) + 4);
    if (Beh.mode == 1 || (Beh.mode == 0 && int(Beh.hr) >= 0)) *reinterpret_cast<u32*>(out) = Beh.token;
    return Beh.hr;
}
inline void InitVtable() {
    for (auto& slot : fakeVtbl) slot = reinterpret_cast<void*>(&ComTrap);
    fakeVtbl[2] = reinterpret_cast<void*>(&ComRelease);
    fakeVtbl[12] = reinterpret_cast<void*>(&ComUnlock);
    fakeVtbl[39] = reinterpret_cast<void*>(&ComSetDepth);
    fakeVtbl[106] = reinterpret_cast<void*>(&ComCreatePixel);
}

// ---- escena
struct CaseScene {
    u32 args[6]{};
    Seeds seeds{};
    std::vector<std::pair<std::uintptr_t, void*>> patches;   // (direccion del EXE, falso)
    unsigned winLo{0x100}, winLen{0x100};
    EngineGlobals g{};
    json input;
};
// Arena con basura determinista por caso, motor falso con stdFunc (todas las ranuras a StdTrap salvo las dos que usan las funciones) y globales por omision.
inline void BuildCommon(unsigned c, CaseScene& s, unsigned eng) {
    arenaStore.fill(0xA5);
    for (unsigned i = 0; i < kMem / 4; ++i) W32(i * 4, Rand(c, 1000 + i));
    for (unsigned i = 0; i < 29; ++i) W32(eng + 0x48 + 4 * i, u32(reinterpret_cast<std::uintptr_t>(&StdTrap)));
    W32(eng + 0x48 + 4 * 2, u32(reinterpret_cast<std::uintptr_t>(&StdRgb)));
    W32(eng + 0x48 + 4 * 23, u32(reinterpret_cast<std::uintptr_t>(&StdLock)));
    W32(kDefaultDevice, u32(reinterpret_cast<std::uintptr_t>(fakeVtbl))); W32(kDefaultDevice + 4, 0);
    s.g = {Addr(eng), 0x100u - eng, 0x140u - eng, 0u, Rand(c, 950), Addr(kDefaultDevice)};
    s.seeds = SeedsOf(c);
    Beh = Behaviour{};
    for (auto& a : s.args) a = 0xC0DE0000u;   // basura conocida en los argumentos de pila que la funcion no usa
}
inline void ComObject(unsigned off, u32 counter) { W32(off, u32(reinterpret_cast<std::uintptr_t>(fakeVtbl))); W32(off + 4, counter); }

using BuildFn = void (*)(unsigned, CaseScene&);
// Texturas: RwTextureGetMipmapping / GetAutoMipmapping / RasterGenerateMipmaps
inline void BuildTexCommon(unsigned c, CaseScene& s, const TexFx& f) {
    BuildCommon(c, s, f.eng);
    const unsigned tb = f.texBase;
    W32(tb + 0x1C, f.mip); W32(tb + 0x20, f.autom); W32(tb + 0x2C, f.cb);
    W32(tb + 0x28, u32(reinterpret_cast<std::uintptr_t>(&StdTrap))); W32(tb + 0x30, u32(reinterpret_cast<std::uintptr_t>(&StdTrap)));
    s.g.tex = tb - f.eng;
    s.winLo = 0x100; s.winLen = 0xC0;
}
inline void BuildMip(unsigned c, CaseScene& s) {
    const auto f = TexCase(c); BuildTexCommon(c, s, f);
    s.input = {{"case", c}, {"eng", f.eng}, {"tex_base", f.texBase}, {"mip", f.mip}, {"auto", f.autom}, {"cb", f.cb}};
}
inline void BuildGenerate(unsigned c, CaseScene& s) {
    auto f = TexCase(c); f.cb = u32(reinterpret_cast<std::uintptr_t>(&StubGen));
    BuildTexCommon(c, s, f);
    s.args[0] = f.raster; s.args[1] = f.image; Beh.rv = f.rv;
    s.input = {{"case", c}, {"eng", f.eng}, {"tex_base", f.texBase}, {"raster", f.raster}, {"image", f.image}, {"rv", f.rv}};
}
inline void BuildRaster(unsigned c, CaseScene& s) {
    const auto f = RasCase(c);
    BuildCommon(c, s, f.eng);
    for (unsigned i = 0; i < 10; ++i) W32(f.rasBase + 4 * i, Rand(c, 200 + i));
    W32(f.rasBase + 0x28, u32(f.sp)); W32(f.rasBase + 0x2C, Rand(c, 210));
    s.g.ras = f.rasBase - f.eng; s.winLo = 0x100; s.winLen = 0xC0;
    s.input = {{"case", c}, {"eng", f.eng}, {"ras_base", f.rasBase}, {"sp", f.sp}};
}
// Registro de plugins: cinco argumentos (textura, diccionario, raster) o cuatro (flujo de textura)
inline void BuildPlugin(unsigned c, CaseScene& s) {
    const auto f = PlugCase(c);
    BuildCommon(c, s, 0x10); s.winLen = 8;
    s.args[0] = f.size; s.args[1] = f.id; s.args[2] = f.f1; s.args[3] = f.f2; s.args[4] = f.f3; Beh.ret = f.ret;
    s.patches = {{0x8084A0, reinterpret_cast<void*>(&StubAddPlugin)}};
    s.input = {{"case", c}, {"size", f.size}, {"id", f.id}, {"f1", f.f1}, {"f2", f.f2}, {"f3", f.f3}, {"ret", f.ret}};
}
inline void BuildStream(unsigned c, CaseScene& s) {
    const auto f = PlugCase(c);
    BuildCommon(c, s, 0x10); s.winLen = 8;
    s.args[0] = f.id; s.args[1] = f.f1; s.args[2] = f.f2; s.args[3] = f.f3; Beh.ret = f.ret;
    s.patches = {{0x8088E0, reinterpret_cast<void*>(&StubAddStream)}};
    s.input = {{"case", c}, {"id", f.id}, {"f1", f.f1}, {"f2", f.f2}, {"f3", f.f3}, {"ret", f.ret}};
}
// Lista de buferes de vertices dinamicos
inline unsigned DynNodeOffset(const DynFx& f, unsigned i) { return kDynBase + kDynStride * DynSlot(f, i); }
inline void BuildDynDestroy(unsigned c, CaseScene& s) {
    const auto f = DynCase(c);
    BuildCommon(c, s, 0x10); s.winLo = 0x100; s.winLen = 0xC0;
    for (unsigned i = 0; i < f.n; ++i) {
        const unsigned at = DynNodeOffset(f, i);
        W32(at + 0x00, Rand(c, 30 + i)); W32(at + 0x04, 0xAAAA0000u + i); W32(at + 0x08, kVbPool[f.vbSel[i]]);
        W32(at + 0x0C, 0xBBBB0000u + i); W32(at + 0x10, i + 1 < f.n ? Addr(DynNodeOffset(f, i + 1)) : 0u);
    }
    s.g.dyn = f.n ? Addr(DynNodeOffset(f, 0)) : 0u;
    s.args[0] = f.arg;
    s.input = {{"case", c}, {"n", f.n}, {"mul", f.mul}, {"arg_sel", f.argSel}, {"vb_sel", std::vector<unsigned>(f.vbSel, f.vbSel + 6)}, {"arg", f.arg}};
}
// Objetos COM: Unlock y Release de sombreadores
inline void BuildObj(unsigned c, CaseScene& s, unsigned slotKind) {
    const auto f = ObjCase(c);
    BuildCommon(c, s, 0x10); s.winLo = 0x100; s.winLen = 0x30;
    ComObject(0x100, f.cnt0); ComObject(0x120, f.cnt1);
    s.args[0] = Addr(f.sel ? 0x120 : 0x100); Beh.hr = f.hr; (void)slotKind;
    s.input = {{"case", c}, {"sel", f.sel}, {"hr", f.hr}, {"cnt0", f.cnt0}, {"cnt1", f.cnt1}};
}
inline void BuildUnlock(unsigned c, CaseScene& s) { BuildObj(c, s, 12); }
inline void BuildRelease(unsigned c, CaseScene& s) { BuildObj(c, s, 2); }
inline void BuildDepth(unsigned c, CaseScene& s) {
    const auto f = DsCase(c);
    BuildCommon(c, s, 0x10); s.winLo = 0x100; s.winLen = 0x10;
    ComObject(0x100, Rand(c, 1));
    s.g.dev = Addr(0x100); s.g.ds = f.cur; s.args[0] = f.arg; Beh.mut = f.mut; Beh.hr = f.hr;
    s.input = {{"case", c}, {"cur", f.cur}, {"arg", f.arg}, {"mut", f.mut}, {"hr", f.hr}};
}
inline void BuildPixel(unsigned c, CaseScene& s) {
    const auto f = PsCase(c);
    BuildCommon(c, s, 0x10); s.winLo = 0x100; s.winLen = 0x40;
    ComObject(0x100, Rand(c, 3));
    for (unsigned i = 0; i < 4; ++i) W32(0x120 + 4 * i, 0xCDCDCDCDu);
    s.g.dev = Addr(0x100); s.args[0] = f.func; s.args[1] = Addr(0x120 + 4 * f.slot); Beh.mode = f.mode; Beh.token = f.token; Beh.hr = f.hr;
    s.input = {{"case", c}, {"func", f.func}, {"token", f.token}, {"mode", f.mode}, {"slot", f.slot}, {"hr", f.hr}};
}
// Funciones estandar con ranura de salida compartida
inline void BuildLock(unsigned c, CaseScene& s) {
    const auto f = StdCase(c);
    BuildCommon(c, s, f.eng); s.winLo = 0x100; s.winLen = 8;
    s.args[0] = f.a; s.args[1] = f.b; Beh.ret = f.ret; Beh.wmode = f.wmode; Beh.token = f.token;
    s.input = {{"case", c}, {"eng", f.eng}, {"raster", f.a}, {"mode", f.b}, {"ret", f.ret}, {"wmode", f.wmode}, {"token", f.token}};
}
inline void BuildRgba(unsigned c, CaseScene& s) {
    const auto f = StdCase(c);
    BuildCommon(c, s, f.eng); s.winLo = 0x100; s.winLen = 0x10;
    W32(0x100, f.rgba);
    s.args[0] = Addr(0x100); s.args[1] = f.b; Beh.ret = f.ret; Beh.wmode = f.wmode; Beh.token = f.token;
    s.input = {{"case", c}, {"eng", f.eng}, {"format", f.b}, {"rgba", f.rgba}, {"ret", f.ret}, {"wmode", f.wmode}, {"token", f.token}};
}
inline void BuildFree(unsigned c, CaseScene& s) {
    const auto f = FreeCase(c);
    BuildCommon(c, s, 0x10); s.winLen = 8;
    for (unsigned i = 0; i < 6; ++i) s.args[i] = f.a[i];
    Beh.ret = f.ret; s.patches = {{0x8019B0, reinterpret_cast<void*>(&StubFree)}};
    s.input = {{"case", c}, {"a", std::vector<u32>(f.a, f.a + 6)}, {"ret", f.ret}};
}
inline void BuildDevReg(unsigned c, CaseScene& s) {
    const auto f = DevCase(c);
    BuildCommon(c, s, 0x10); s.winLen = 8;
    Beh.ret = f.ret; s.patches = {{0x4C9AB0, reinterpret_cast<void*>(&StubDeviceReg)}};
    s.input = {{"case", c}, {"ret", f.ret}};
}
// _rwObjectHasFrameReleaseFrame: objeto RwObjectHasFrame en 0x130 (object{type,flags,parent}, lFrame{next,prev}, sync) y dos nodos de enlace vecinos
inline void BuildFrame(unsigned c, CaseScene& s) {
    const auto f = FrameCase(c);
    BuildCommon(c, s, 0x10); s.winLo = 0x100; s.winLen = 0x50;
    const unsigned P = kFramePrev, N = kFrameNext, O = kFrameObj;
    W32(P, f.junk[0]); W32(P + 4, f.junk[1]); W32(N, f.junk[2]); W32(N + 4, f.junk[3]);
    W32(O + 0, 0x00000012u); W32(O + 4, f.parent); W32(O + 0x10, f.junk[4]);
    u32 prev = Addr(P), next = Addr(N);
    switch (f.topo) {
        case 1: next = Addr(P); break;
        case 2: prev = Addr(O + 8); break;
        case 3: prev = Addr(O + 0xC); break;
        case 4: next = Addr(O + 8); break;
        case 5: next = Addr(O + 4); break;
        case 6: prev = Addr(O + 0); break;
        case 7: next = Addr(O + 0xC); break;
        default: break;
    }
    W32(O + 8, next); W32(O + 0xC, prev);
    s.args[0] = Addr(O);
    s.input = {{"case", c}, {"topo", f.topo}, {"parent", f.parent}, {"junk", std::vector<u32>(f.junk, f.junk + 6)}};
}

struct FnSpec {
    const char* name; const char* cls; const char* fn;
    std::uintptr_t entry; bool voidFn; BuildFn build;
};
inline const std::array<FnSpec, 20> kSpecs{{
    {"RwCore::RwTextureGetMipmapping", "RwCore", "RwTextureGetMipmapping", 0x7F3550, false, BuildMip},
    {"RwCore::RwTextureGetAutoMipmapping", "RwCore", "RwTextureGetAutoMipmapping", 0x7F3580, false, BuildMip},
    {"RwCore::RwTextureRegisterPlugin", "RwCore", "RwTextureRegisterPlugin", 0x7F3BB0, false, BuildPlugin},
    {"RwCore::RwTexDictionaryRegisterPlugin", "RwCore", "RwTexDictionaryRegisterPlugin", 0x7F3C10, false, BuildPlugin},
    {"RwCore::RwTextureRasterGenerateMipmaps", "RwCore", "RwTextureRasterGenerateMipmaps", 0x7F3CD0, false, BuildGenerate},
    {"RwCore::RwD3D9DynamicVertexBufferDestroy", "RwCore", "RwD3D9DynamicVertexBufferDestroy", 0x7F5AE0, true, BuildDynDestroy},
    {"RwCore::RwD3D9DynamicVertexBufferUnlock", "RwCore", "RwD3D9DynamicVertexBufferUnlock", 0x7F5C90, false, BuildUnlock},
    {"RwCore::_rwD3D9SetDepthStencilSurface", "RwCore", "_rwD3D9SetDepthStencilSurface", 0x7F5EF0, true, BuildDepth},
    {"RwCore::_rwDeviceRegisterPlugin", "RwCore", "_rwDeviceRegisterPlugin", 0x7F5F60, false, BuildDevReg},
    {"RwCore::RwD3D9DeleteVertexShader", "RwCore", "RwD3D9DeleteVertexShader", 0x7FAC90, true, BuildRelease},
    {"RwCore::RwD3D9CreatePixelShader", "RwCore", "RwD3D9CreatePixelShader", 0x7FACC0, false, BuildPixel},
    {"RwCore::RwD3D9DeletePixelShader", "RwCore", "RwD3D9DeletePixelShader", 0x7FACF0, true, BuildRelease},
    {"RwCore::RwRasterGetCurrentContext", "RwCore", "RwRasterGetCurrentContext", 0x7FAE60, false, BuildRaster},
    {"RwCore::RwRasterRegisterPlugin", "RwCore", "RwRasterRegisterPlugin", 0x7FB0B0, false, BuildPlugin},
    {"RwCore::RwRasterLockPalette", "RwCore", "RwRasterLockPalette", 0x7FB0E0, false, BuildLock},
    {"RwPlCore::RwFreeListCreate", "RwPlCore", "RwFreeListCreate", 0x801980, false, BuildFree},
    {"RwPlCore::RwFreeListCreateAndPreallocateSpace", "RwPlCore", "RwFreeListCreateAndPreallocateSpace", 0x801B70, false, BuildFree},
    {"RwCore::RwRGBAToPixel", "RwCore", "RwRGBAToPixel", 0x803740, false, BuildRgba},
    {"RwCore::RwTextureRegisterPluginStream", "RwCore", "RwTextureRegisterPluginStream", 0x804550, false, BuildStream},
    {"RwCore::_rwObjectHasFrameReleaseFrame", "RwCore", "_rwObjectHasFrameReleaseFrame", 0x804F40, true, BuildFrame},
}};

// ---- compuerta cdecl con seis argumentos de pila (las funciones con menos argumentos ignoran el resto); EAX/ECX/EDX con semillas por caso
struct Reg { u32 eax, ecx, edx; bool stack, nonvolatile; };
inline Reg GateCdecl6(std::uintptr_t entry, const u32* a, const Seeds& seeds) {
    u32 before{}, after{}, ra{}, rc{}, rd{}, b0{}, s0{}, i0{}, p0{}, b1{}, s1{}, i1{}, p1{};
    const u32 a0 = a[0], a1 = a[1], a2 = a[2], a3 = a[3], a4 = a[4], a5 = a[5], seedA = seeds.eax, seedC = seeds.ecx, seedD = seeds.edx;
    __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
    __asm {push a5} __asm {push a4} __asm {push a3} __asm {push a2} __asm {push a1} __asm {push a0}
    __asm {mov eax,seedA} __asm {mov ecx,seedC} __asm {mov edx,seedD} __asm {call entry}
    __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {add esp,24} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
    return {ra, rc, rd, before == after, b0 == b1 && s0 == s1 && i0 == i1 && p0 == p1};
}

// Estado observable de un lado: ventana del arena, "fuera de la ventana sin cambios", traza, trampas y globales (antes de restaurarlos).
struct Snap { std::string window; std::uint64_t outside; std::uint64_t full; EngineGlobals g; };
inline std::uint64_t OutsideHash(const CaseScene& s) {
    std::uint64_t h = Fnv(arenaStore.data(), kGuard + s.winLo);
    h = Fnv(ArenaMem() + s.winLo + s.winLen, kMem - s.winLo - s.winLen, h);
    return Fnv(arenaStore.data() + kGuard + kMem, kGuard, h);
}
inline Snap Snapshot(const CaseScene& s) {
    return {HexOf(ArenaMem() + s.winLo, s.winLen), OutsideHash(s), Fnv(arenaStore.data(), arenaStore.size()), ReadGlobals()};
}
inline json ObserveJson(const CaseScene& s, const Snap& before, const Reg& reg, bool voidFn, const EngineGlobals& after) {
    json out = {{"after", HexOf(ArenaMem() + s.winLo, s.winLen)}, {"outside_unchanged", OutsideHash(s) == before.outside}, {"trace", traceLog}, {"traps", trapCount},
                {"globals", GlobalsJson(after)}, {"before_full_hash", before.full}};
    if (!voidFn) out["eax"] = reg.eax;
    return out;
}
}  // namespace Dependencies25DScene
