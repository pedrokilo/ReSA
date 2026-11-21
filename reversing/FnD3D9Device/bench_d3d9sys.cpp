// Banco fuera del juego del lote D3D9DEV: compara la funcion 0x7F5F70 (`D3D9System`, despachador del sistema del dispositivo D3D9) del EXE original, mapeado en memoria
// (solo lectura, sin ejecutar el juego), con su reconstruccion en C++ (bloque extraido de rwcore.cpp por extract_bench.py). IDirect3D9, IDirect3DDevice9 y las superficies son
// simulacros con registro de llamadas (ranura, argumentos, resultado); las funciones auxiliares pesadas del EXE (lista de rasters, Im2D, estados, gestor de buferes, cache de matrices,
// parada, estandares, parametros de presentacion, _rwMatrixSetMultFn y _rwVectorSetMultFn) se sustituyen por registradores en el EXE (las usan las dos ejecuciones, el EXE por su direccion
// y el C++ por los adaptadores de abajo). Siguen reales: deteccion de CPU, 0x857404 (x87), recuento y enumeracion de modos (0x7F7540) y niveles de multimuestreo (0x7F6BF0).
// Cada escenario genera un mundo aleatorio (adaptadores, capacidades, modos, fallos) y una secuencia de peticiones; se ejecuta con el EXE y despues con el C++ desde el mismo estado
// (datos del EXE restaurados, arena determinista) y se comparan: valor devuelto, bytes de salida, registro de llamadas COM/auxiliares y huella de los datos del EXE tras cada peticion.
// Sin NOTSA_WINDOWED_MODE: el C++ sigue el camino original (fallo de CreateDevice -> FALSE); el camino NOTSA (BeforeCreateDevice/AfterCreateDevice, exit(10)) se revisa aparte.
#include "common.h"
#include <d3d9.h>
#include <xmmintrin.h>
#include <cstddef>

typedef uint32_t uint32; typedef int32_t int32; typedef uint16_t uint16; typedef uint8_t uint8;
typedef int32_t RwInt32; typedef uint32_t RwUInt32; typedef int32_t RwBool; typedef char RwChar;
typedef void (__cdecl* rwMatrixMultFn)(void*, const void*, const void*);
typedef void* (__cdecl* rwVectorMultFn)(void*, const void*, const void*);
typedef void* (__cdecl* rwVectorsMultFn)(void*, const void*, int, const void*);
template<class T> static inline T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }

// ---------------- motor falso (RwEngineInstance + 0xF8 rwstrcpy, +0x134/0x138/0x13C asignador)
static u8* g_engine;
static std::map<void*, u32> g_sizes;
static void* __cdecl rw_malloc(u32 size, u32 hint) { void* p = ar_alloc_fill(size ? size : 1); g_sizes[p] = size; L(0xA0000000u ^ size); L(hint); return p; }
static void __cdecl rw_free(void* p) { L(0xF0000000u); L((u32)p); }
static void* __cdecl rw_realloc(void* p, u32 size, u32 hint) { void* q = ar_alloc_fill(size); if (p) memcpy(q, p, std::min(size, g_sizes[p])); g_sizes[q] = size; L(0xB0000000u ^ size); L(hint); return q; }
static char* __cdecl rw_strcpy(char* d, const char* s) { L(0x57C0u); L(fnv(s, strlen(s) + 1)); return strcpy(d, s); }
#define RwFree(p) rw_free((p))
#define rwstrcpy(d, s) rw_strcpy((d), (s))

// ---------------- simulacros de COM
static const int NSLOT = 140;
struct Mock { void** vtbl; u32 kind; };
enum { K_D3D = 1, K_DEV = 2, K_SURF = 3 };
static void* g_vt_d3d[NSLOT]; static void* g_vt_dev[NSLOT]; static void* g_vt_surf[NSLOT];
static Mock g_d3d_obj = { g_vt_d3d, K_D3D }, g_dev_obj = { g_vt_dev, K_DEV }, g_surf_obj = { g_vt_surf, K_SURF };

struct World {
    u32 adapters;                 // IDirect3D9::GetAdapterCount
    bool capsFail[4];             // GetDeviceCaps falla en el adaptador
    u32 modeCount[4][3];          // modos por adaptador y formato (0x17, 0x16, 0x23)
    u32 desktopFormat[4];
    u32 salt;                     // semilla de las respuestas derivadas
    bool createFail;              // CreateDevice falla
    u32 texMem;
    u32 clientW, clientH;
    bool d3dFail;                 // Direct3DCreate9 devuelve NULL
    bool surfFail;                // GetRenderTarget/GetDepthStencilSurface devuelven un HRESULT negativo
} W;
static u32 hsh(u32 a, u32 b = 0, u32 c = 0, u32 d = 0) { u32 h = W.salt * 2654435761u ^ 0x9E3779B9u; for (u32 v : { a, b, c, d }) { h ^= v + 0x7F4A7C15u + (h << 6) + (h >> 2); h *= 0x85EBCA6Bu; h ^= h >> 13; } return h; }
static u32 fmtIndex(u32 f) { return f == 0x17 ? 0 : f == 0x16 ? 1 : f == 0x23 ? 2 : 9; }
static u32 P(const void* p) { u32 v = (u32)p; return (v >= 0x401000 && v < 0xCB1000) || (v >= (u32)g_ar && v < (u32)g_ar + ARENA) ? v : 0xBADBAD; }

// IDirect3D9
static ULONG __stdcall d3d_Release(Mock*) { L(0xD302); return 0; }
static UINT __stdcall d3d_GetAdapterCount(Mock*) { L(0xD304); return W.adapters; }
static HRESULT __stdcall d3d_GetAdapterIdentifier(Mock*, UINT a, DWORD fl, D3DADAPTER_IDENTIFIER9* id) {
    L(0xD305); L(a); L(fl); L(P(id)); memset(id, 0, sizeof(*id)); sprintf(id->Description, "ADAPTER-%u-%08x", a, hsh(a)); sprintf(id->Driver, "drv%u", a); return (hsh(a, 5) % 7 == 0) ? E_FAIL : 0; }
static UINT __stdcall d3d_GetAdapterModeCount(Mock*, UINT a, D3DFORMAT f) { L(0xD306); L(a); L(f); u32 i = fmtIndex(f); return (a < 4 && i < 3) ? W.modeCount[a][i] : 0; }
static HRESULT __stdcall d3d_EnumAdapterModes(Mock*, UINT a, D3DFORMAT f, UINT idx, D3DDISPLAYMODE* m) {
    L(0xD307); L(a); L(f); L(idx); L(P(m)); u32 i = fmtIndex(f); if (a >= 4 || i >= 3 || idx >= W.modeCount[a][i]) return D3DERR_INVALIDCALL;
    u32 h = hsh(a, f, idx % 5); m->Width = 320 + (h % 7) * 160; m->Height = 240 + ((h >> 8) % 6) * 120; m->RefreshRate = 50 + ((h >> 16) % 4) * 25; m->Format = f; return 0; }
static HRESULT __stdcall d3d_GetAdapterDisplayMode(Mock*, UINT a, D3DDISPLAYMODE* m) {
    L(0xD308); L(a); L(P(m)); u32 h = hsh(a, 8); m->Width = 800 + (h % 4) * 320; m->Height = 600 + ((h >> 4) % 4) * 200; m->RefreshRate = 60 + (h >> 20) % 3 * 15; m->Format = (D3DFORMAT)W.desktopFormat[a & 3]; return 0; }
static HRESULT __stdcall d3d_CheckDeviceType(Mock*, UINT a, D3DDEVTYPE t, D3DFORMAT af, D3DFORMAT bf, BOOL w) { L(0xD309); L(a); L(t); L(af); L(bf); L(w); return hsh(a, t, af, bf) % 3 ? 0 : D3DERR_NOTAVAILABLE; }
static HRESULT __stdcall d3d_CheckDeviceFormat(Mock*, UINT a, D3DDEVTYPE t, D3DFORMAT af, DWORD u, D3DRESOURCETYPE r, D3DFORMAT c) { L(0xD30A); L(a); L(t); L(af); L(u); L(r); L(c); return hsh(af, u, c) % 2 ? 0 : D3DERR_NOTAVAILABLE; }
static HRESULT __stdcall d3d_CheckDeviceMultiSampleType(Mock*, UINT a, D3DDEVTYPE t, D3DFORMAT f, BOOL w, D3DMULTISAMPLE_TYPE ms, DWORD* q) {
    L(0xD30B); L(a); L(t); L(f); L(w); L(ms); L(P(q)); if (hsh(ms, w, f) % 3 == 0) return D3DERR_NOTAVAILABLE; if (q) *q = hsh(ms, 11) % 9; return 0; }
static HRESULT __stdcall d3d_CheckDepthStencilMatch(Mock*, UINT a, D3DDEVTYPE t, D3DFORMAT af, D3DFORMAT rf, D3DFORMAT df) { L(0xD30C); L(a); L(t); L(af); L(rf); L(df); return hsh(af, rf, df) % 2 ? 0 : D3DERR_NOTAVAILABLE; }
static HRESULT __stdcall d3d_GetDeviceCaps(Mock*, UINT a, D3DDEVTYPE t, D3DCAPS9* c) {
    L(0xD30E); L(a); L(t); L(P(c)); if (a >= 4 || W.capsFail[a]) return D3DERR_INVALIDCALL;
    u8* b = (u8*)c; for (size_t i = 0; i < sizeof(*c); i++) b[i] = (u8)(hsh(a, (u32)i) >> 7);
    D3DCAPS9* k = c; k->DevCaps = (hsh(a, 99) & ~(D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_PUREDEVICE)) | ((hsh(a, 100) & 1) ? D3DDEVCAPS_HWTRANSFORMANDLIGHT : 0) | ((hsh(a, 101) & 1) ? D3DDEVCAPS_PUREDEVICE : 0);
    k->MaxTextureWidth = 256u << (hsh(a, 102) % 5); k->MaxTextureHeight = 256u << (hsh(a, 103) % 5); k->DeclTypes = 0x1FF | (hsh(a, 104) & 0xF00); return 0; }
static HRESULT __stdcall d3d_CreateDevice(Mock*, UINT a, D3DDEVTYPE t, HWND hw, DWORD fl, D3DPRESENT_PARAMETERS* pp, void** dev) {
    L(0xD310); L(a); L(t); L((u32)hw); L(fl); L(P(pp)); L(P(dev)); L(fnv(pp, sizeof(*pp))); L(fnv((void*)0xC9BF00, sizeof(D3DCAPS9)));
    if (W.createFail) return D3DERR_NOTAVAILABLE; *dev = &g_dev_obj; return 0; }
// IDirect3DDevice9
static ULONG __stdcall dev_Release(Mock*) { L(0xDE02); return 0; }
static UINT __stdcall dev_GetAvailableTextureMem(Mock*) { L(0xDE04); return W.texMem; }
static HRESULT __stdcall dev_GetRenderTarget(Mock*, DWORD i, void** s) { L(0xDE26); L(i); L(P(s)); if (W.surfFail) return D3DERR_INVALIDCALL; *s = &g_surf_obj; return 0; }
static HRESULT __stdcall dev_GetDepthStencilSurface(Mock*, void** s) { L(0xDE28); L(P(s)); if (W.surfFail) return D3DERR_NOTFOUND; *s = &g_surf_obj; return 0; }
static ULONG __stdcall surf_Release(Mock*) { L(0x5F02); return 0; }
static void unexpected() { printf("ranura COM inesperada\n"); fflush(stdout); ExitProcess(3); }
// API de Windows y de D3D que el EXE llama por su tabla de importaciones (parcheada) y el C++ por las macros de abajo
static void* __stdcall mock_Direct3DCreate9(UINT v) { L(0x3D9C); L(v); return W.d3dFail ? nullptr : &g_d3d_obj; }
static BOOL __stdcall mock_ShowWindow(HWND h, int c) { L(0x5A0A); L((u32)h); L(c); return 1; }
static BOOL __stdcall mock_GetClientRect(HWND h, RECT* r) { L(0x6C1E); L((u32)h); L(P(r)); r->left = 0; r->top = 0; r->right = W.clientW; r->bottom = W.clientH; return 1; }
#define Direct3DCreate9 mock_Direct3DCreate9
#define ShowWindow mock_ShowWindow
#define GetClientRect mock_GetClientRect

// ---------------- sustitutos registradores de las funciones auxiliares del EXE (se parchean en el EXE: los dos lados los llaman)
static void __cdecl stub_SetPresent(const void* mode, u32 flags, u32 fmt) {
    const u32* m = (const u32*)mode; L(0x5E77); L(P(mode)); L(m[0]); L(m[1]); L(m[2]); L(m[3]); L(flags); L(fmt);
    u32* pp = (u32*)0xC9C040; for (int i = 0; i < 14; i++) pp[i] = (i + 1) * 0x01010101u ^ (flags * 7 + fmt) ^ (m[0] + i);
}
static void __cdecl stub_RasterList() { L(0x4CC1); }
static RwBool __cdecl stub_Im2D() { L(0x1A2D); return 1; }
static void __cdecl stub_RenderState() { L(0x5E57); }
static RwBool __cdecl stub_VBOpen() { L(0xB0B0); return 1; }
static void __cdecl stub_TransformReset() { L(0x7C0E); }
static RwBool __cdecl stub_Stop(void* o, void* io, RwInt32 in) { L(0x5707); L(P(o)); L(P(io)); L(in); StaticRef<void*>(0xC97C28) = nullptr; return 1; }
static RwBool __cdecl stub_Standards(void* o, void* io, RwInt32 in) { L(0x57D0); L(P(o)); L(P(io)); L(in); return 1; }
static RwBool __cdecl stub_MatrixMult(void* f) { L(0x3A7F); L((u32)f); return 1; }
static RwBool __cdecl stub_VectorMult(void* a, void* b, void* c, void* d) { L(0x3EC7); L((u32)a); L((u32)b); L((u32)c); L((u32)d); return 1; }

// adaptadores que usa el C++ (llaman al EXE por su direccion, igual que las funciones propias del proyecto cuando no estan enganchadas)
RwInt32 _rwD3D9CPUDetectMMX() { return ((RwInt32(__cdecl*)())0x80DBE0)(); }
RwInt32 _rwD3D9CPUDetectSSE() { return ((RwInt32(__cdecl*)())0x80DC40)(); }
RwInt32 _rwD3D9CPUDetectSSE2() { return ((RwInt32(__cdecl*)())0x80DCA0)(); }
RwInt32 _rwD3D9CPUDetect3DNow() { return ((RwInt32(__cdecl*)())0x80DD00)(); }
void _rwD3D9RenderStateOpen() { ((void(__cdecl*)())0x7FCAC0)(); }
RwBool _rwD3D9Im2DRenderOpen() { return ((RwBool(__cdecl*)())0x7FB480)(); }
RwBool _rwMatrixSetMultFn(rwMatrixMultFn f) { return ((RwBool(__cdecl*)(void*))0x7F12C0)((void*)f); }
RwBool _rwVectorSetMultFn(rwVectorMultFn a, rwVectorsMultFn b, rwVectorMultFn c, rwVectorsMultFn d) { return ((RwBool(__cdecl*)(void*, void*, void*, void*))0x7ED5F0)((void*)a, (void*)b, (void*)c, (void*)d); }
void _rwD3D9FindMSLevels() { ((void(__cdecl*)())0x7F6BF0)(); }
void _rwD3D9EnumerateDisplayModes() { ((void(__cdecl*)())0x7F7540)(); }
void _rwD3D9SetPresentParameters(const void* m, RwUInt32 f, RwUInt32 a) { ((void(__cdecl*)(const void*, u32, u32))0x7F6CB0)(m, f, a); }
void _rwD3D9StateCacheReset() { ((void(__cdecl*)())0x7F6B00)(); }
void _rwD3D9TransformCacheReset() { ((void(__cdecl*)())0x7F6B40)(); }
RwBool _rwD3D9VertexBufferManagerOpen() { return ((RwBool(__cdecl*)())0x7F5CB0)(); }
RwBool _rwD3D9DeviceSystemStop(void* o, void* io, RwInt32 in) { return ((RwBool(__cdecl*)(void*, void*, RwInt32))0x7F7100)(o, io, in); }
RwBool _rwD3D9DeviceSystemStandards(void* o, void* io, RwInt32 in) { return ((RwBool(__cdecl*)(void*, void*, RwInt32))0x7F72E0)(o, io, in); }

// ---------------- codigo propio: declaraciones del proyecto (copiadas de rwcore.cpp por extract_bench.py) y la funcion
typedef uint32_t uint32;
namespace {
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
constexpr size_t kSlotRelease = 2;
constexpr size_t kSlotCheckDeviceMultiSampleType = 11;
void* (&D3D9CurrentRenderTargets)[4] = *reinterpret_cast<void*(*)[4]>(0xC98090);
auto& D3D9DirectObject              = StaticRef<void*>(0xC97C20);
auto& D3D9AdapterIndex              = StaticRef<uint32>(0xC97C24);
auto& D3D9DeviceType                = StaticRef<uint32>(0x8E2428);
auto& D3D9AdapterFormat             = StaticRef<uint32>(0xC9BEF0);
auto& D3D9FullScreenFlag            = StaticRef<uint32>(0xC9BEF8);
auto& CurrentDepthStencilSurface    = StaticRef<void*>(0xC9808C);
auto& D3D9Device                    = StaticRef<void*>(0xC97C28);
auto& D3D9RenderSurface             = StaticRef<void*>(0xC97C30);
struct RwFreeList;
}
#include "bloque_d3d9sys.inc"

// ---------------- banco
static u8* g_snap;                       // copia de los datos del EXE (0x8E0000..0xCB1000)
static const u32 DATA_LO = 0x8E0000, DATA_HI = 0xCB1000;
struct Req { int req; int in; };
struct Result { std::vector<u32> log; u32 finalHash; };

static void reset_state(u32 salt) {
    memcpy((void*)DATA_LO, g_snap, DATA_HI - DATA_LO);
    g_brk = 0; g_log.clear(); g_sizes.clear(); memset(g_ar, 0, 1 << 20);
    g_engine = (u8*)ar_alloc(0x200); memset(g_engine, 0, 0x200);
    *(void**)(g_engine + 0xF8) = (void*)rw_strcpy;
    *(void**)(g_engine + 0x134) = (void*)rw_malloc; *(void**)(g_engine + 0x138) = (void*)rw_free; *(void**)(g_engine + 0x13C) = (void*)rw_realloc;
    *(void**)0xC97B24 = g_engine;
    _fpreset(); _mm_setcsr(0x1F80);
    (void)salt;
}
static u32 state_hash(bool full = false) {
    u32 h = 2166136261u;
    if (full) h = fnv((void*)DATA_LO, DATA_HI - DATA_LO);
    else { h = fnv((void*)0x8E2400, 0x140, h); h = fnv((void*)0xC97B00, 0x700, h); h = fnv((void*)0xC9BC00, 0x500, h); h = fnv((void*)0xCAF000, 0x10, h); } u32 c = _mm_getcsr(); h = fnv(&c, 4, h); unsigned short cw; __asm { fnstcw cw } h = fnv(&cw, 2, h); return h; }

typedef RwBool(__cdecl* SysFn)(RwInt32, void*, void*, RwInt32);
static Result run(bool useCpp, const std::vector<Req>& seq, u32 salt, u32 hwnd, bool neg, Patch* np) {
    reset_state(salt);
    u8* outbuf = (u8*)ar_alloc(0x600); u32* io = (u32*)ar_alloc(16); *io = hwnd;
    if (neg && !useCpp) patch_on(*np);
    for (const Req& r : seq) {
        memset(outbuf, 0xA5, 0x600);
        L(0x7E00 + r.req); L(r.in);
        RwBool ret = useCpp ? _rwD3D9System(r.req, outbuf, io, r.in) : ((SysFn)0x7F5F70)(r.req, outbuf, io, r.in);
        L(0x7E57); L(ret); L(fnv(outbuf, 0x600)); L(state_hash());
    }
    if (neg && !useCpp) patch_off(*np);
    Result res; res.log = g_log; res.finalHash = state_hash(true); return res;
}

static LONG WINAPI crash_filter(EXCEPTION_POINTERS* e) {
    printf("EXCEPCION %08lx en EIP=%p (acceso a %p) EAX=%08lx ECX=%08lx EDX=%08lx ESP=%08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress,
        e->ExceptionRecord->NumberParameters > 1 ? (void*)e->ExceptionRecord->ExceptionInformation[1] : nullptr, e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esp);
    fflush(stdout); ExitProcess(9);
}
int main(int argc, char** argv) {
    SetUnhandledExceptionFilter(crash_filter);
    const char* exe = argc > 1 ? argv[1] : "E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\gta_sa.exe";
    int N = argc > 2 ? atoi(argv[2]) : 500; u32 seed = argc > 3 ? (u32)atoi(argv[3]) : 1;
    bool neg = argc > 4 && !strcmp(argv[4], "neg");
    if (!map_exe(exe)) return 2;
    g_ar = (u8*)VirtualAlloc(nullptr, ARENA, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    g_snap = (u8*)VirtualAlloc(nullptr, DATA_HI - DATA_LO, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    // tablas virtuales
    for (int i = 0; i < NSLOT; i++) g_vt_d3d[i] = g_vt_dev[i] = g_vt_surf[i] = (void*)unexpected;
    g_vt_d3d[2] = (void*)d3d_Release; g_vt_d3d[4] = (void*)d3d_GetAdapterCount; g_vt_d3d[5] = (void*)d3d_GetAdapterIdentifier; g_vt_d3d[6] = (void*)d3d_GetAdapterModeCount; g_vt_d3d[7] = (void*)d3d_EnumAdapterModes;
    g_vt_d3d[8] = (void*)d3d_GetAdapterDisplayMode; g_vt_d3d[9] = (void*)d3d_CheckDeviceType; g_vt_d3d[10] = (void*)d3d_CheckDeviceFormat; g_vt_d3d[11] = (void*)d3d_CheckDeviceMultiSampleType;
    g_vt_d3d[12] = (void*)d3d_CheckDepthStencilMatch; g_vt_d3d[14] = (void*)d3d_GetDeviceCaps; g_vt_d3d[16] = (void*)d3d_CreateDevice;
    g_vt_dev[2] = (void*)dev_Release; g_vt_dev[4] = (void*)dev_GetAvailableTextureMem; g_vt_dev[38] = (void*)dev_GetRenderTarget; g_vt_dev[40] = (void*)dev_GetDepthStencilSurface; g_vt_surf[2] = (void*)surf_Release;
    // IAT del EXE (importaciones sin resolver en el mapa): Direct3DCreate9, ShowWindow, GetClientRect
    *(void**)0x8E3ED4 = (void*)mock_Direct3DCreate9; *(void**)0x858278 = (void*)mock_ShowWindow; *(void**)0x858304 = (void*)mock_GetClientRect;
    // sustitutos registradores en el EXE
    static Patch stubs[] = { { 0x7F6CB0, (void*)stub_SetPresent }, { 0x4CC150, (void*)stub_RasterList }, { 0x7FB480, (void*)stub_Im2D }, { 0x7FCAC0, (void*)stub_RenderState }, { 0x7F5CB0, (void*)stub_VBOpen },
        { 0x7F6B40, (void*)stub_TransformReset }, { 0x7F7100, (void*)stub_Stop }, { 0x7F72E0, (void*)stub_Standards }, { 0x7F12C0, (void*)stub_MatrixMult }, { 0x7ED5F0, (void*)stub_VectorMult } };
    for (auto& s : stubs) patch_on(s);
    memcpy(g_snap, (void*)DATA_LO, DATA_HI - DATA_LO);
    static Patch negp = { 0x7F6BF0, nullptr };   // control negativo: _rwD3D9FindMSLevels -> xor eax,eax; ret (solo en la ejecucion del EXE)
    Rng rng(seed); R = &rng;
    int fails = 0, runs = 0; u64 requests = 0; u64 counts[24] = {}; u64 started = 0, openOk = 0;
    for (int it = 0; it < N; it++) {
        // mundo
        memset(&W, 0, sizeof(W)); W.salt = rng.next();
        bool openFail = rng.chance(15);
        W.adapters = openFail ? rng.below(2) : 1 + rng.below(3);
        for (int a = 0; a < 4; a++) { W.capsFail[a] = openFail ? true : (a == 0 ? false : rng.chance(30)); for (int f = 0; f < 3; f++) W.modeCount[a][f] = rng.chance(20) ? 0 : rng.below(7); }
        if (!openFail && rng.chance(25)) { W.capsFail[0] = true; W.capsFail[1] = false; W.adapters = std::max<u32>(W.adapters, 2); }
        static const u32 fmts[] = { 0x15, 0x16, 0x17, 0x18, 0x19, 0x23, 0x1A, 0x20, 0x32, 0x14 };
        for (int a = 0; a < 4; a++) W.desktopFormat[a] = fmts[rng.below(10)];
        W.createFail = rng.chance(20); W.d3dFail = openFail && rng.chance(50); W.texMem = rng.next(); W.clientW = rng.below(2000); W.clientH = rng.below(1200); W.surfFail = false;
        u32 hwnd = rng.chance(15) ? 0 : 0x10000 + rng.below(0xFFFF);
        // secuencia (con las precondiciones que exige el EXE para no desreferenciar punteros nulos)
        std::vector<Req> seq; seq.push_back({ 4, 0 });
        bool canOpen = !openFail && !W.capsFail[0] || (!openFail && W.adapters >= 2 && !W.capsFail[1]);
        seq.push_back({ 0, 0 });
        int n = 4 + rng.below(24); bool opened = canOpen, dev = false;
        if (!opened) { seq.push_back({ 1, 0 }); seq.push_back({ 0, 0 }); }
        for (int i = 0; i < n && opened; i++) {
            static const int pool[] = { 5, 5, 6, 6, 6, 7, 7, 2, 2, 8, 8, 10, 11, 12, 13, 14, 14, 15, 16, 16, 17, 17, 19, 22, 9, 18, 20, 21, 23, 40, 3, 1 };
            int rq = pool[rng.below(sizeof(pool) / sizeof(pool[0]))]; int in = (int)rng.below(9) - 2;
            if (rq == 12 && !(dev && !W.createFail)) continue;
            if (rq == 12 && W.createFail) continue;
            if (rq == 1) { seq.push_back({ 1, 0 }); seq.push_back({ 0, 0 }); dev = false; continue; }
            if (rq == 3) { seq.push_back({ 3, 0 }); dev = false; continue; }
            if (rq == 2) { seq.push_back({ 2, 0 }); dev = !W.createFail; continue; }
            if (rq == 8) in = rng.below(2);
            if (rq == 17) in = 0;
            seq.push_back({ rq, in });
        }
        if (getenv("V")) { printf("it=%d adapters=%u caps0=%d caps1=%d createFail=%d d3dFail=%d hwnd=%x seq:", it, W.adapters, W.capsFail[0], W.capsFail[1], W.createFail, W.d3dFail, hwnd); for (auto& q : seq) printf(" %d(%d)", q.req, q.in); printf("\n"); fflush(stdout); }
        Result a = run(false, seq, W.salt, hwnd, neg, &negp);
        Result b = run(true, seq, W.salt, hwnd, false, nullptr);
        runs++; requests += seq.size(); for (auto& r : seq) if (r.req >= 0 && r.req < 24) counts[r.req]++;
        if (canOpen) openOk++; if (dev) started++;
        bool same = a.log == b.log && a.finalHash == b.finalHash;
        if (!same) {
            fails++; size_t i = 0; while (i < a.log.size() && i < b.log.size() && a.log[i] == b.log[i]) i++;
            printf("it=%d difiere (%zu/%zu entradas; primera diferencia en %zu: %08x/%08x; hash %08x/%08x)\n", it, a.log.size(), b.log.size(), i,
                i < a.log.size() ? a.log[i] : 0, i < b.log.size() ? b.log[i] : 0, a.finalHash, b.finalHash);
            if (fails > 8) break;
        }
    }
    printf("escenarios=%d peticiones=%llu (Open correcto en %llu, dispositivo creado en %llu) diferencias=%d\n", runs, requests, openOk, started, fails);
    printf("peticiones por tipo:"); for (int i = 0; i < 24; i++) printf(" %d:%llu", i, counts[i]); printf("\n");
    return fails ? 1 : 0;
}
