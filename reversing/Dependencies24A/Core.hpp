#pragma once
// Nucleo comun del banco Dependencies24A (Win::__MainWndProc 0x747EB0): lo comparten el banco dentro del juego (LiveComparison.hpp) y el ensayo sin juego (offline/main.cpp).
// Contiene el estado x87, la compuerta stdcall de 4 argumentos, los colaboradores falsos (funciones del juego, objetos COM falsos y API de Win32 por IAT), la preparacion de las
// globales del EXE que lee o escribe el procedimiento y la instantanea de la memoria tocada. Requiere los tipos del proyecto (StdInc.h) y nlohmann::json.
#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

namespace Dependencies24ALive {
using namespace Dependencies24AFix;

// Los diez bytes de cada registro x87 VACIO se comparan enteros (FNSAVE completo salvo FIP). No se relaja nunca.
inline constexpr bool kCompareDeadX87Registers = true;

inline std::string Hex(const void* a, size_t n) { constexpr char d[] = "0123456789abcdef"; const auto* b = static_cast<const uint8*>(a); std::string o; o.reserve(n * 2); for (size_t i = 0; i < n; ++i) { o += d[b[i] >> 4]; o += d[b[i] & 15]; } return o; }
inline uint64 Fnv(const void* a, size_t n) { uint64 h = 1469598103934665603ull; const auto* b = static_cast<const uint8*>(a); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline std::vector<uint8> BytesAt(uintptr_t at, size_t n) { const auto* p = reinterpret_cast<const uint8*>(at); return {p, p + n}; }

// ---- estado x87 / SSE ----
inline void SaveFP(void* p) { __asm { mov eax, p } __asm { fnsave [eax] } __asm { frstor [eax] } }
inline void RestoreFP(void* p) { __asm { mov eax, p } __asm { frstor [eax] } }
inline uint32 ReadMXCSR() { uint32 v{}; __asm { stmxcsr v } return v; }
inline void SetMXCSR(uint32 v) { __asm { ldmxcsr v } }
alignas(16) inline std::array<uint8, 108> savedFP{};
inline uint32 savedMXCSR{};
inline bool savedFPActive{};
inline void RestoreFloatingState() { if (savedFPActive) { RestoreFP(savedFP.data()); SetMXCSR(savedMXCSR); } }
inline bool FloatingStateRestored() { alignas(16) std::array<uint8, 108> c{}; SaveFP(c.data()); return c == savedFP && ReadMXCSR() == savedMXCSR; }

// ---- FNSAVE: +0 FCW, +4 FSW, +8 FTW, +12 FIP, +16 FCS|FOP, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes cada uno) ----
using Fp = std::array<uint8, 108>;
inline uint32 W(const Fp& f, size_t at, size_t n = 4) { uint32 v{}; memcpy(&v, f.data() + at, n); return v; }
inline bool Dead(const Fp& f, unsigned st) { const uint32 ftw = W(f, 8, 2), top = (W(f, 4, 2) >> 11) & 7; return ((ftw >> (2 * ((top + st) & 7))) & 3) == 3; }
inline json FpDifference(const Fp& a, const Fp& b) {
    json o = json::array();
    const struct { const char* n; size_t at, sz; } fl[]{{"fcw", 0, 2}, {"fsw", 4, 2}, {"ftw", 8, 2}, {"fcs_fop", 16, 4}, {"fdp", 20, 4}, {"fds", 24, 4}};
    for (const auto& f : fl) if (memcmp(a.data() + f.at, b.data() + f.at, f.sz)) o.push_back(f.n);
    for (unsigned st = 0; st < 8; ++st) { const bool dead = Dead(a, st); if ((!dead || kCompareDeadX87Registers) && memcmp(a.data() + 28 + 10 * st, b.data() + 28 + 10 * st, 10)) o.push_back("st" + std::to_string(st) + (dead ? "(dead)" : "(valid)")); }
    return o;
}
inline bool DeadEqual(const Fp& a, const Fp& b) { for (unsigned st = 0; st < 8; ++st) if (Dead(a, st) && memcmp(a.data() + 28 + 10 * st, b.data() + 28 + 10 * st, 10)) return false; return true; }

// ---- estado x87 inicial determinista: FCW 0x7F (24 bits, redondeo al par, excepciones enmascaradas), FSW del caso (TOP = 0), ocho residuos; MXCSR 0x1F80 ----
alignas(16) inline uint8 gPrime[108]{};
inline void MakePrime(uint32 fsw) {
    std::memset(gPrime, 0, 108); gPrime[0] = 0x7F; const uint16 s = uint16(fsw); std::memcpy(gPrime + 4, &s, 2);
    for (int i = 0; i < 8; ++i) { uint8* r = gPrime + 28 + 10 * i; for (int k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k); r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40; }
}

// ---- compuerta: WNDPROC __stdcall (HWND, UINT, WPARAM, LPARAM), ret 0x10, LRESULT en EAX ----
struct Gate { bool stack{}, nonvolatile{}; uint32 mxcsr{}, eax{}; alignas(16) std::array<uint8, 108> img{}; };
inline Gate GateStd4(uintptr_t entry, uint32 argHwnd, uint32 argMsg, uint32 argWparam, uint32 argLparam) {
    Gate g{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, rv{}, mx0 = 0x1F80, mx1{}; uint8* img = g.img.data(); const uint8* prime = gPrime;
    __asm {
        ldmxcsr mx0
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        push argLparam
        push argWparam
        push argMsg
        push argHwnd
        mov ecx, 0x1CE5D00D
        mov edx, 0x2DE5C0DE
        mov eax, 0xA5123456
        call entry
        mov rv, eax
        mov sp1, esp
        stmxcsr mx1
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, img
        fnsave [eax]
        frstor [eax]
    }
    g.stack = sp0 == sp1; g.nonvolatile = b0 == b1 && s0 == s1 && d0 == d1 && p0 == p1; g.mxcsr = mx1; g.eax = rv; return g;
}

// ---- hilo principal (los demas hilos van a la API real) ----
inline DWORD gMainThread{};
inline bool OnMain() { return GetCurrentThreadId() == gMainThread; }

// ---- traza de colaboradores (palabras de longitud fija por tipo) ----
inline std::vector<uint32> trace;
inline void T(std::initializer_list<uint32> v) { trace.insert(trace.end(), v); }
enum Ev : uint32 {
    EvRsEvent = 1, EvRsKey = 2, EvTranslateKey = 3, EvTranslateShift = 4, EvPauseSounds = 5, EvResumeSounds = 6, EvPadClear = 7, EvBackBuf = 8, EvShutdown = 9, EvWaitState = 10, EvUpdateWindow = 11,
    EvGraphNotify = 12, EvStreaming = 13, EvLetter = 14, EvCheckDvd = 15, EvNoDisk = 16, EvSkipCutscene = 17, EvGetDevice = 18, EvSetGamma = 19, EvGetState = 20, EvRun = 21, EvPauseMedia = 22, EvTrap = 23,
    EvClipCursor = 30, EvPostQuit = 31, EvSetCursor = 32, EvShowCursor = 33, EvSetCapture = 34, EvReleaseCapture = 35, EvGetPlacement = 36, EvSendMessage = 37, EvSetTimer = 38, EvSetFocus = 39,
    EvSetWindowPos = 40, EvDefWindowProc = 41, EvOutputDebug = 42, EvMax = 43
};
inline constexpr std::array<int, EvMax> MakeTraceLen() {
    std::array<int, EvMax> l{};
    l[EvRsEvent] = 6; l[EvRsKey] = 3; l[EvTranslateKey] = 3; l[EvTranslateShift] = 2; l[EvPauseSounds] = 2; l[EvResumeSounds] = 2; l[EvPadClear] = 4; l[EvBackBuf] = 1; l[EvShutdown] = 1; l[EvWaitState] = 2;
    l[EvUpdateWindow] = 1; l[EvGraphNotify] = 1; l[EvStreaming] = 1; l[EvLetter] = 1; l[EvCheckDvd] = 2; l[EvNoDisk] = 2; l[EvSkipCutscene] = 2; l[EvGetDevice] = 1; l[EvSetGamma] = 5; l[EvGetState] = 4;
    l[EvRun] = 2; l[EvPauseMedia] = 2; l[EvTrap] = 1; l[EvClipCursor] = 2; l[EvPostQuit] = 2; l[EvSetCursor] = 2; l[EvShowCursor] = 2; l[EvSetCapture] = 2; l[EvReleaseCapture] = 1; l[EvGetPlacement] = 2;
    l[EvSendMessage] = 5; l[EvSetTimer] = 6; l[EvSetFocus] = 2; l[EvSetWindowPos] = 8; l[EvDefWindowProc] = 5; l[EvOutputDebug] = 3;
    return l;
}
inline constexpr std::array<int, EvMax> kTraceLen = MakeTraceLen();
inline json TraceJson(const std::vector<uint32>& t, bool full) {
    json counts = json::object(); bool ok = true; size_t i = 0;
    while (i < t.size()) { const uint32 k = t[i]; if (k == 0 || k >= EvMax || kTraceLen[k] == 0 || i + kTraceLen[k] > t.size()) { ok = false; break; } counts[std::to_string(k)] = counts.value(std::to_string(k), 0) + 1; i += kTraceLen[k]; }
    json o = {{"len", t.size()}, {"hash", Fnv(t.data(), t.size() * 4)}, {"counts", counts}, {"well_formed", ok}};
    if (full) o["words"] = t;
    return o;
}

// ---- globales del EXE ----
inline uint32& D32(uintptr_t a) { return *reinterpret_cast<uint32*>(a); }
inline uint8& D8(uintptr_t a) { return *reinterpret_cast<uint8*>(a); }
namespace Addr {
inline constexpr uintptr_t kForeground = 0x8D621C, kCodePause = 0xB7CB48, kFem = 0xBA6748, kFrameLimit = 0xC1704C, kPs = 0xC17054, kGameState = 0xC8D4C0, kGammaChanged = 0xC920C8, kWindowed = 0xC920CC,
    kMediaControl = 0xC920D8, kRwInit = 0xC920E8, kRejected = 0xC9246C, kAudioHw = 0xB5F8B8, kCutsceneRunning = 0xB5F851, kPads = 0xB73458, kSavedGamma = 0xC8D4C8, kGammaTable = 0xC8DAC8;
}
struct Window { uintptr_t at; size_t n; };
// Ventanas de memoria que se guardan, se comparan y se restauran alrededor de cada lado (todo lo que el procedimiento y sus falsos pueden escribir, con margen)
inline constexpr std::array<Window, 9> kWindows{{{0x8D6218, 12}, {0xB7CB44, 12}, {0xBA6740, 0x120}, {0xC17040, 0x28}, {0xC8D4BC, 12}, {0xC920C4, 0x2C}, {0xC92468, 12}, {0xB5F8B4, 8}, {0xB5F84C, 12}}};
inline std::vector<uint8> SnapshotWindows() { std::vector<uint8> v; for (const auto& w : kWindows) { const auto b = BytesAt(w.at, w.n); v.insert(v.end(), b.begin(), b.end()); } return v; }
inline void RestoreWindows(const std::vector<uint8>& s) { size_t o = 0; for (const auto& w : kWindows) { std::memcpy(reinterpret_cast<void*>(w.at), s.data() + o, w.n); o += w.n; } }

// ---- arena (lParam apunta aqui), estructura de plataforma falsa, dispositivo D3D9 y control multimedia COM falsos ----
alignas(4096) inline uint8 gArena[4096]{};
alignas(8) inline uint8 gPs[0x28]{};
inline constexpr size_t kRectAt = 0x100, kDevAt = 0x200;
inline Case G{};
inline unsigned checkCalls{}, noDiskCalls{};
struct FakeCom { void** vtbl; };
inline void* gDeviceVtbl[128]{};
inline void* gMediaVtbl[16]{};
inline FakeCom gDevice{gDeviceVtbl}, gMedia{gMediaVtbl};

// colaboradores del juego (los mismos en el EXE y en la DLL; reemplazan a la funcion original por EntryPatch)
inline int __cdecl FRsEvent(int event, void* param) {
    if (event == 0) { auto* r = static_cast<int32*>(param); T({EvRsEvent, uint32(event), uint32(r[0]), uint32(r[1]), uint32(r[2]), uint32(r[3])}); if (G.camMode & 1) r[2] = int32(G.camW); if (G.camMode & 2) r[3] = int32(G.camH); }
    else T({EvRsEvent, uint32(event), uint32(uintptr_t(param)), D32(Addr::kForeground), 0u, 0u}); // ForegroundApp en este instante: fija el orden de las escrituras
    return int(G.rsRet);
}
inline int __cdecl FRsKey(int event, void* ck) { T({EvRsKey, uint32(event), *static_cast<uint32*>(ck)}); return int(G.rsRet); }
// Devuelve como el EXE (`mov al, 1` / `xor al, al` sobre un EAX sucio): solo cuenta AL; los 24 bits altos son basura que el procedimiento NO debe mirar
inline int __cdecl FTranslateKey(uint32* ck, uint32 lParam, uint32 vk) { T({EvTranslateKey, lParam, vk}); *ck = G.translateCk; return int(0x7A5B2C00u | (G.translateRet ? 1u : 0u)); }
inline int __cdecl FTranslateShift(uint32* ck) { T({EvTranslateShift, *ck}); return 1; } // el EXE empuja un segundo argumento que la funcion no lee (cdecl: el llamador lo limpia)
inline void __cdecl FPauseSounds() { T({EvPauseSounds, D32(Addr::kForeground)}); }
inline void __cdecl FResumeSounds() { T({EvResumeSounds, D32(Addr::kForeground)}); }
inline void __fastcall FPadClear(void* self, void*, uint8 a, uint8 b) { const uintptr_t d = uintptr_t(self) - Addr::kPads; T({EvPadClear, (d % 0x134 == 0 && d / 0x134 < 2) ? uint32(d / 0x134) : 0xFFFFu, a, b}); }
inline void __cdecl FBackBuf() { T({EvBackBuf}); }
inline int __cdecl FShutdown() { T({EvShutdown}); return int(G.rsRet); }
inline void __cdecl FWaitState(int state) { T({EvWaitState, uint32(state)}); }
inline long __cdecl FUpdateWindow() { T({EvUpdateWindow}); return long(G.rsRet); }
inline long __cdecl FGraphNotify() { T({EvGraphNotify}); return long(G.rsRet); }
inline bool __fastcall FStreaming(void*, void*) { T({EvStreaming}); return G.streaming != 0; }
inline char __fastcall FLetter(void*, void*) { T({EvLetter}); return char(G.letter); }
inline bool __fastcall FCheckDvd(void*, void*) { ++checkCalls; T({EvCheckDvd, checkCalls}); return checkCalls >= 8 || ((G.checkMask >> (checkCalls - 1)) & 1) != 0; }
inline void __fastcall FNoDisk(void*, void*) { ++noDiskCalls; T({EvNoDisk, noDiskCalls}); if (noDiskCalls == G.quitAt || noDiskCalls >= 9) D8(Addr::kFem + 0x58) = 1; }
inline void __cdecl FSkipCutscene() { T({EvSkipCutscene, D8(Addr::kCodePause)}); } // CodePause ya vale lo que escribio el procedimiento antes de la llamada
inline void* __cdecl FGetDevice() { T({EvGetDevice}); return G.deviceNull ? nullptr : &gDevice; }
// metodos COM falsos (stdcall, this primero)
inline long __stdcall FSetGammaRamp(void* self, uint32 swapChain, uint32 flags, void* ramp) {
    const uintptr_t r = uintptr_t(ramp); T({EvSetGamma, self == &gDevice, swapChain, flags, r == Addr::kSavedGamma ? 1u : r == Addr::kGammaTable ? 2u : 0xFFFFu}); return 0;
}
inline long __stdcall FGetState(void* self, long ms, long* state) {
    T({EvGetState, self == &gMedia, uint32(ms), uint32(*state)});
    if (G.getStateWrites) *state = long(G.getStateValue);
    if (G.nullifyMedia) D32(Addr::kMediaControl) = 0;
    return long(G.getStateHr);
}
inline long __stdcall FMediaRun(void* self) { T({EvRun, self == &gMedia}); return long(G.runHr); }
inline long __stdcall FMediaPause(void* self) { T({EvPauseMedia, self == &gMedia}); return long(G.pauseHr); }
inline long __stdcall FTrap() { T({EvTrap}); return 0; }
inline void InitFakes() {
    for (auto& e : gDeviceVtbl) e = reinterpret_cast<void*>(&FTrap);
    for (auto& e : gMediaVtbl) e = reinterpret_cast<void*>(&FTrap);
    gDeviceVtbl[21] = reinterpret_cast<void*>(&FSetGammaRamp);
    gMediaVtbl[7] = reinterpret_cast<void*>(&FMediaRun); gMediaVtbl[8] = reinterpret_cast<void*>(&FMediaPause); gMediaVtbl[10] = reinterpret_cast<void*>(&FGetState);
}

// ---- API de Win32 por IAT (solo el hilo principal; los demas hilos van a la API real) ----
inline FARPROC Real(const char* dll, const char* fn) { const HMODULE m = GetModuleHandleA(dll); return m ? GetProcAddress(m, fn) : nullptr; }
// Direccion REAL de la API (por GetProcAddress, no por la IAT parcheada): los demas hilos no deben pasar por los falsos ni recursar
template<class F> inline F RealFn(const char* dll, const char* fn) { return reinterpret_cast<F>(Real(dll, fn)); }
inline BOOL __stdcall FClipCursor(const RECT* r) { if (!OnMain()) return RealFn<BOOL(__stdcall*)(const RECT*)>("user32.dll", "ClipCursor")(r); T({EvClipCursor, uint32(uintptr_t(r))}); return 1; }
inline void __stdcall FPostQuitMessage(int c) { if (!OnMain()) return RealFn<void(__stdcall*)(int)>("user32.dll", "PostQuitMessage")(c); T({EvPostQuit, uint32(c)}); }
inline HCURSOR __stdcall FSetCursor(HCURSOR h) { if (!OnMain()) return RealFn<HCURSOR(__stdcall*)(HCURSOR)>("user32.dll", "SetCursor")(h); T({EvSetCursor, uint32(uintptr_t(h))}); return nullptr; }
inline int __stdcall FShowCursor(BOOL b) { if (!OnMain()) return RealFn<int(__stdcall*)(BOOL)>("user32.dll", "ShowCursor")(b); T({EvShowCursor, uint32(b)}); return int(G.showRet); }
inline HWND __stdcall FSetCapture(HWND h) { if (!OnMain()) return RealFn<HWND(__stdcall*)(HWND)>("user32.dll", "SetCapture")(h); T({EvSetCapture, uint32(uintptr_t(h))}); return nullptr; }
inline BOOL __stdcall FReleaseCapture() { if (!OnMain()) return RealFn<BOOL(__stdcall*)()>("user32.dll", "ReleaseCapture")(); T({EvReleaseCapture}); return 1; }
inline BOOL __stdcall FGetWindowPlacement(HWND h, WINDOWPLACEMENT* p) { if (!OnMain()) return RealFn<BOOL(__stdcall*)(HWND, WINDOWPLACEMENT*)>("user32.dll", "GetWindowPlacement")(h, p); T({EvGetPlacement, uint32(uintptr_t(h))}); p->showCmd = G.showCmd; return 1; }
inline LRESULT __stdcall FSendMessageA(HWND h, UINT m, WPARAM w, LPARAM l) { if (!OnMain()) return RealFn<LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM)>("user32.dll", "SendMessageA")(h, m, w, l); T({EvSendMessage, uint32(uintptr_t(h)), m, uint32(w), uint32(l)}); return 0; }
inline UINT_PTR __stdcall FSetTimer(HWND h, UINT_PTR id, UINT ms, TIMERPROC proc) { if (!OnMain()) return RealFn<UINT_PTR(__stdcall*)(HWND, UINT_PTR, UINT, TIMERPROC)>("user32.dll", "SetTimer")(h, id, ms, proc); T({EvSetTimer, uint32(uintptr_t(h)), uint32(id), ms, uint32(uintptr_t(proc)), D32(Addr::kForeground)}); return 1; }
inline HWND __stdcall FSetFocus(HWND h) { if (!OnMain()) return RealFn<HWND(__stdcall*)(HWND)>("user32.dll", "SetFocus")(h); T({EvSetFocus, uint32(uintptr_t(h))}); return nullptr; }
inline BOOL __stdcall FSetWindowPos(HWND h, HWND after, int x, int y, int cx, int cy, UINT flags) { if (!OnMain()) return RealFn<BOOL(__stdcall*)(HWND, HWND, int, int, int, int, UINT)>("user32.dll", "SetWindowPos")(h, after, x, y, cx, cy, flags); T({EvSetWindowPos, uint32(uintptr_t(h)), uint32(uintptr_t(after)), uint32(x), uint32(y), uint32(cx), uint32(cy), flags}); return 1; }
inline LRESULT __stdcall FDefWindowProcA(HWND h, UINT m, WPARAM w, LPARAM l) { if (!OnMain()) return RealFn<LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM)>("user32.dll", "DefWindowProcA")(h, m, w, l); T({EvDefWindowProc, uint32(uintptr_t(h)), m, uint32(w), uint32(l)}); return LRESULT(G.defRet); }
inline void __stdcall FOutputDebugStringA(const char* s) { if (!OnMain()) return RealFn<void(__stdcall*)(const char*)>("kernel32.dll", "OutputDebugStringA")(s); const size_t n = strlen(s); T({EvOutputDebug, uint32(n), uint32(Fnv(s, n))}); }

// Asignacion nombre -> falso -> direccion de la ranura en la tabla de importacion del EXE (para el ensayo sin juego; en el juego se busca por valor)
struct ApiFake { const char* dll; const char* name; void* fake; uintptr_t exeSlot; };
inline std::array<ApiFake, 13> ApiFakes() {
    return {{{"user32.dll", "ReleaseCapture", reinterpret_cast<void*>(&FReleaseCapture), 0x858288}, {"user32.dll", "GetWindowPlacement", reinterpret_cast<void*>(&FGetWindowPlacement), 0x85828C},
             {"user32.dll", "SetTimer", reinterpret_cast<void*>(&FSetTimer), 0x858290}, {"user32.dll", "ClipCursor", reinterpret_cast<void*>(&FClipCursor), 0x858294},
             {"user32.dll", "PostQuitMessage", reinterpret_cast<void*>(&FPostQuitMessage), 0x858298}, {"user32.dll", "SetCursor", reinterpret_cast<void*>(&FSetCursor), 0x85829C},
             {"user32.dll", "SetCapture", reinterpret_cast<void*>(&FSetCapture), 0x8582A0}, {"user32.dll", "DefWindowProcA", reinterpret_cast<void*>(&FDefWindowProcA), 0x8582A4},
             {"user32.dll", "SetFocus", reinterpret_cast<void*>(&FSetFocus), 0x8582D8}, {"user32.dll", "SendMessageA", reinterpret_cast<void*>(&FSendMessageA), 0x8582DC},
             {"user32.dll", "SetWindowPos", reinterpret_cast<void*>(&FSetWindowPos), 0x8582E0}, {"user32.dll", "ShowCursor", reinterpret_cast<void*>(&FShowCursor), 0x8582EC},
             {"kernel32.dll", "OutputDebugStringA", reinterpret_cast<void*>(&FOutputDebugStringA), 0x858118}}};
}
inline HMODULE SelfModule() { HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(&SelfModule), &m); return m; }
struct IatPatch {
    std::vector<std::pair<uintptr_t*, uintptr_t>> slots;
    static void Set(uintptr_t* t, uintptr_t v) { DWORD o{}; VirtualProtect(t, 4, PAGE_READWRITE, &o); *t = v; VirtualProtect(t, 4, o, &o); }
    // `modules`: modulos cuyas tablas se parchean (el EXE y la DLL en el juego; el ejecutable del ensayo sin juego)
    IatPatch(std::initializer_list<HMODULE> modules, const char* dll, const char* fn, void* fake) {
        std::vector<uintptr_t> reals;
        for (const char* d : {dll, "kernelbase.dll", "user32.dll", "win32u.dll"}) if (const auto r = Real(d, fn)) reals.push_back(uintptr_t(r));
        for (HMODULE m : modules) {
            auto* base = reinterpret_cast<uint8*>(m); auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if (!dir.VirtualAddress) continue;
            for (auto* d = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + dir.VirtualAddress); d->Name; ++d)
                for (auto* t = reinterpret_cast<uintptr_t*>(base + d->FirstThunk); *t; ++t)
                    if (std::ranges::find(reals, *t) != reals.end()) { slots.push_back({t, *t}); Set(t, uintptr_t(fake)); }
        }
    }
    ~IatPatch() { for (auto it = slots.rbegin(); it != slots.rend(); ++it) Set(it->first, it->second); }
};

// ---- preparacion del caso ----
inline uint32 LparamOf(const Case& c) { return c.lparamKind == 1 ? uint32(uintptr_t(gArena) + kRectAt) : c.lparamKind == 2 ? uint32(uintptr_t(gArena) + kDevAt) : c.lparam; }
inline void Apply(const Case& c) {
    std::memset(gArena, 0xA5, sizeof gArena); std::memcpy(gArena + kRectAt, c.rect, 16); std::memcpy(gArena + kDevAt, c.dev, 20);
    std::memset(gPs, 0, sizeof gPs); D32(uintptr_t(gPs)) = c.psWindow; D32(uintptr_t(gPs) + 8) = c.psFullScreen;
    D32(Addr::kForeground) = c.foregroundInit; D8(Addr::kCodePause) = uint8(c.codePause);
    D8(Addr::kFem + 0x33) = uint8(c.menuActivateNext); D8(Addr::kFem + 0x58) = uint8(c.quitNoDvd); D8(Addr::kFem + 0x5C) = uint8(c.menuActive); D8(Addr::kFem + 0xE9) = uint8(c.menuMainSwitch);
    D32(Addr::kFem + 0xE0) = c.mouseX; D32(Addr::kFem + 0xE4) = c.mouseY;
    D32(Addr::kFrameLimit) = c.frameLimit; D32(Addr::kPs) = uint32(uintptr_t(gPs)); D32(Addr::kGameState) = c.gameState;
    D8(Addr::kGammaChanged) = uint8(c.gammaChanged); D32(Addr::kWindowed) = c.windowedInit; D32(Addr::kMediaControl) = c.mediaNull ? 0u : uint32(uintptr_t(&gMedia));
    D32(Addr::kRwInit) = c.rwInit; D32(Addr::kRejected) = c.rejectedInit; D8(Addr::kAudioHw) = uint8(c.hwInit); D8(Addr::kCutsceneRunning) = uint8(c.cutsceneRunning);
    G = c; checkCalls = 0; noDiskCalls = 0; trace.clear();
}

struct SideResult { Gate gate; std::vector<uint32> trace; std::vector<uint8> windows, arena, ps; };
inline SideResult Capture(const Gate& g) {
    SideResult r; r.gate = g; r.trace = trace; r.windows = SnapshotWindows(); r.arena.assign(gArena, gArena + sizeof gArena); r.ps.assign(gPs, gPs + sizeof gPs); return r;
}
// Lado -> JSON (lo que debe coincidir bit a bit entre el EXE y la DLL)
inline json SideJson(const Case& c, const SideResult& s) {
    json o = {{"input", {{"route", c.route}, {"case", c.n}, {"msg", c.msg}, {"wparam", c.wparam}, {"lparam_kind", c.lparamKind}, {"lparam", c.lparamKind ? 0u : c.lparam}, {"fsw", c.fsw}, {"hwnd", c.hwnd},
                        {"game_state", c.gameState}, {"rw_init", c.rwInit}, {"frame_limit", c.frameLimit}}},
              {"trace", TraceJson(s.trace, false)},
              {"observed", {{"eax", s.gate.eax}, {"windows_hash", Fnv(s.windows.data(), s.windows.size())}, {"arena_hash", Fnv(s.arena.data(), s.arena.size())}, {"ps_hash", Fnv(s.ps.data(), s.ps.size())}, {"mxcsr", s.gate.mxcsr & 0x3F}}}};
    Fp fp = s.gate.img; memset(fp.data() + 12, 0, 4);   // FIP: cada lado ejecuta su ultima instruccion x87 en otra direccion (el modulo se valida aparte)
    o["post_floating_state"] = Hex(fp.data(), 108);
    return o;
}
inline uint32 FipOf(const Gate& g) { Fp f = g.img; return W(f, 12); }
} // namespace Dependencies24ALive
