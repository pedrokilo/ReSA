#pragma once
// Banco dentro del motor (flujo viejo, simple, estilo Small2/PadMouse1/GeoFinal1): CamHud1 = CHud (24 funciones) + CCam (12 funciones).
// Original (hook desactivado) frente a candidato (hook activo) con el mismo fixture determinista por caso. Se ejecuta dentro del juego
// completo (partida cargada) y se engancha en CGame::Process con probe_build.py. Ningun singleton real se usa salvo las zonas globales
// listadas en 'globals' (estaticas de CHud, CTimer, RsGlobal, Players, CPad, TheCamera...), que se guardan al empezar y se restauran
// tras cada ejecucion y al final. Objetos falsos: peds/vehiculos/entidades/camaras en buffers con guardas de 16 bytes.
// Colaboradores simulados con EntryPatch (direccion del EXE + simbolo de la DLL + su cuerpo) y trazados con argumentos; solo traza
// el hilo del banco (los hilos ajenos no tocan el registro). Dibujo real (RenderWare) y E/S nunca se ejecutan.
// Fuera del banco (ver README): CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise, Shutdown, CCam::Process_Rocket, Process_1stPerson.
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Hud.h"
#include "Cam.h"
#include "Camera.h"
#include "Font.h"
#include "Sprite2d.h"
#include "Messages.h"
#include "Pad.h"
#include "World.h"
#include "WaterLevel.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Shadows.h"
#include "HandShaker.h"
#include "IdleCam.h"
#include "General.h"
#include "Scripts/TheScripts.h"
#include "../ProbeRuntime.hpp"

namespace CamHud1Live {
using Hook = ReversibleHooks::ReversibleHook::Base;
inline bool done{}; inline std::ofstream log;
// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas (rand, etc.); sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline uint32 P(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline float Flt(uint32 b) { return std::bit_cast<float>(b); }
inline uint32 Rnd(uint32& s) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s; }
inline uint32 Seed(unsigned i, uint32 salt) { uint32 s = i * 2654435761u + salt * 0x9E3779B9u + 1u; if (!s) s = 1; for (int k = 0; k < 4; ++k) Rnd(s); return s; }
template<class T> inline T& At(uint8* base, size_t off) { return *reinterpret_cast<T*>(base + off); }
template<class T> inline T& Mem(uintptr_t address) { return *reinterpret_cast<T*>(address); }
inline std::string Hex(const void* p, size_t n) {
    static const char* d = "0123456789abcdef"; std::string s(n * 2, '0'); auto* b = static_cast<const uint8*>(p);
    for (size_t i = 0; i < n; ++i) { s[2 * i] = d[b[i] >> 4]; s[2 * i + 1] = d[b[i] & 15]; }
    return s;
}
inline std::string HexAt(uintptr_t address, size_t n) { return Hex(reinterpret_cast<const void*>(address), n); }

// Buffer con guardas de 16 bytes a cada lado (se comparan enteros: detectan escrituras fuera de rango).
template<size_t N> struct Buf {
    alignas(16) std::array<uint8, N + 32> raw{};
    uint8* p() { return raw.data() + 16; }
    void Fill(uint8 v) { raw.fill(v); }
    std::string Dump() const { return Hex(raw.data(), raw.size()); }
};

// Floats de los fixtures: moderados (el x87 conserva el exponente extendido y SSE no) y sin NaN/Inf de senalizacion.
inline float PickNear(uint32& s) { static constexpr float t[] = { 0.f, 0.5f, -0.5f, 1.f, -1.f, 2.f, -2.f, 3.f, -3.f, 1.5f, -1.5f }; return t[Rnd(s) % std::size(t)]; }
inline float PickRand(uint32& s) { const uint32 r = Rnd(s); return std::bit_cast<float>((r & 0x80000000u) | ((110u + Rnd(s) % 30u) << 23) | (r & 0x7FFFFFu)); }
inline float PickM(uint32& s) { return Rnd(s) % 100 < 45 ? PickNear(s) : PickRand(s); }
// Unitario aleatorio (para vectores normales)
inline float PickUnit(uint32& s) { return float(int32(Rnd(s) % 2001) - 1000) / 1000.f; }

// --- registro de trazas sin reservas ---
struct Rec { uint32 tag; uint32 n; uint32 a[20]; };
inline std::array<Rec, 1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t, std::initializer_list<uint32> v = {}) {
    if (!Mine() || nrec >= recs.size()) return;
    auto& r = recs[nrec++]; r = {}; r.tag = t; for (auto x : v) if (r.n < 20) r.a[r.n++] = x;
}
inline json Recs() { json r = json::array(); for (size_t i = 0; i < nrec; ++i) { json e = json::array({ recs[i].tag }); for (uint32 k = 0; k < recs[i].n; ++k) e.push_back(recs[i].a[k]); r.push_back(e); } return r; }
// Texto GXT terminado en NUL -> huella (el puntero cambia entre original y candidato solo si el contenido difiere)
inline uint32 TextHash(const void* p) {
    if (!p) return 0;
    auto* b = static_cast<const uint8*>(p); size_t n = 0; while (n < 600 && b[n]) ++n;
    return uint32(Fnv(b, n)) ^ uint32(n * 0x01000193u);
}

enum Tag : uint32 {
    T_BG = 1, T_SCALE, T_SCALELANG, T_ORIENT, T_CENTRE, T_PROP, T_EDGE, T_DROPCOLOR, T_STYLE, T_COLOR, T_PRINT, T_PRINTBOTTOM, T_RJWRAP,
    T_JUSTIFY, T_SLANT, T_WRAPX, T_BGCOLOR, T_DROPPOS, T_BAR, T_SPRITEDRAW, T_WEAPSKILL, T_BRIEF, T_FADING, T_FADESTATUS, T_SCRIPTSPRITES,
    T_WATER, T_CONECAST, T_NEARCLIP, T_SHADOW, T_FINDTARGET, T_TELEPORT, T_RAND, T_VTABLE
};

// --- estado de los colaboradores simulados (lo fija el fixture del caso) ---
inline uint32 fixSkill{}; inline bool fixFading{}; inline int32 fixFadeStatus{};
inline bool fixWaterHit{}; inline float fixWaterLevel{};
inline bool fixConeHit{}; inline std::array<float, 3> fixConeOut{}; inline float fixConeDist{};
inline void* fixTarget{}; inline int32 rngCalls{}; inline uint32 rngSeed{};

// Colaboradores de CFont (todos cdecl, estaticos). Los bool/int8/int16 solo cuentan por su ancho real.
inline void __cdecl SBg(uint32 a, uint32 b) { if (!Mine()) return; Push(T_BG, { a & 0xFF, b & 0xFF }); }
inline void __cdecl SScale(uint32 a, uint32 b) { if (!Mine()) return; Push(T_SCALE, { a, b }); }
inline void __cdecl SScaleLang(uint32 a, uint32 b) { if (!Mine()) return; Push(T_SCALELANG, { a, b }); }
inline void __cdecl SOrient(uint32 a) { if (!Mine()) return; Push(T_ORIENT, { a & 0xFF }); }
inline void __cdecl SCentre(uint32 a) { if (!Mine()) return; Push(T_CENTRE, { a }); }
inline void __cdecl SProp(uint32 a) { if (!Mine()) return; Push(T_PROP, { a & 0xFF }); }
inline void __cdecl SEdge(uint32 a) { if (!Mine()) return; Push(T_EDGE, { a & 0xFF }); }
inline void __cdecl SDropColor(uint32 a) { if (!Mine()) return; Push(T_DROPCOLOR, { a }); }
inline void __cdecl SStyle(uint32 a) { if (!Mine()) return; Push(T_STYLE, { a & 0xFF }); }
inline void __cdecl SColor(uint32 a) { if (!Mine()) return; Push(T_COLOR, { a }); }
inline void __cdecl SPrint(uint32 x, uint32 y, const void* text) { if (!Mine()) return; Push(T_PRINT, { x, y, TextHash(text) }); }
inline void __cdecl SPrintBottom(uint32 x, uint32 y, const void* text) { if (!Mine()) return; Push(T_PRINTBOTTOM, { x, y, TextHash(text) }); }
inline void __cdecl SRjWrap(uint32 a) { if (!Mine()) return; Push(T_RJWRAP, { a }); }
inline void __cdecl SJustify(uint32 a) { if (!Mine()) return; Push(T_JUSTIFY, { a & 0xFF }); }
inline void __cdecl SSlant(uint32 a) { if (!Mine()) return; Push(T_SLANT, { a }); }
inline void __cdecl SWrapx(uint32 a) { if (!Mine()) return; Push(T_WRAPX, { a }); }
inline void __cdecl SBgColor(uint32 a) { if (!Mine()) return; Push(T_BGCOLOR, { a }); }
inline void __cdecl SDropPos(uint32 a) { if (!Mine()) return; Push(T_DROPPOS, { a & 0xFFFF }); }
// CSprite2d::DrawBarChart (cdecl, 10 dwords)
inline void __cdecl SBar(uint32 x, uint32 y, uint32 w, uint32 h, uint32 progress, uint32 a, uint32 b, uint32 c, uint32 fore, uint32 back) {
    if (!Mine()) return; Push(T_BAR, { x, y, w & 0xFFFF, h & 0xFF, progress, a & 0xFF, b & 0xFF, c & 0xFF, fore, back });
}
// CSprite2d::Draw(const CRect&, const CRGBA&) (miembro: thiscall con dos argumentos)
inline void __fastcall SSpriteDraw(void* self, void*, const uint32* rect, const uint32* color) {
    if (!Mine()) return; Push(T_SPRITEDRAW, { P(self) - 0xBAB1FCu, rect[0], rect[1], rect[2], rect[3], *color });
}
// CPed::GetWeaponSkill() (miembro sin argumentos; el EXE devuelve el byte en AL y luego empuja EAX completo)
inline uint32 __fastcall SWeaponSkill(void* self, void*) { if (!Mine()) return 1; Push(T_WEAPSKILL, { P(self) }); return fixSkill; }
inline void __cdecl SBrief(const void* text, uint32 n1, uint32 n2, uint32 n3, uint32 n4, uint32 n5, uint32 n6, const void* str) {
    if (!Mine()) return; Push(T_BRIEF, { TextHash(text), n1, n2, n3, n4, n5, n6, TextHash(str) });
}
inline uint32 __fastcall SFading(void* self, void*) { if (!Mine()) return 0; Push(T_FADING, { P(self) }); return fixFading ? 1u : 0u; }
inline uint32 __fastcall SFadeStatus(void* self, void*) { if (!Mine()) return 0; Push(T_FADESTATUS, { P(self) }); return uint32(fixFadeStatus); }
inline void __cdecl SScriptSprites(uint32 a) { if (!Mine()) return; Push(T_SCRIPTSPRITES, { a & 0xFF }); }
// CWaterLevel::GetWaterLevel(x, y, z, float* out, bool, CVector*): el DLL la recibe por referencia (mismo diseno en pila)
inline uint32 __cdecl SWater(uint32 x, uint32 y, uint32 z, float* out, uint32 touching, const void* normals) {
    if (!Mine()) return 0; Push(T_WATER, { x, y, z, touching & 0xFF, P(normals) });
    if (fixWaterHit) *out = fixWaterLevel; return fixWaterHit ? 1u : 0u;
}
inline uint32 __fastcall SCone(void* self, void*, const uint32* pos, const uint32* look, float* out, uint32 rad, uint32 minDist, float* outDist) {
    if (!Mine()) return 0; Push(T_CONECAST, { P(self), pos[0], pos[1], pos[2], look[0], look[1], look[2], rad, minDist, Mem<uint32>(0xB7CD68) });
    if (fixConeHit) { out[0] = fixConeOut[0]; out[1] = fixConeOut[1]; out[2] = fixConeOut[2]; *outDist = fixConeDist; } return fixConeHit ? 1u : 0u;
}
inline uint32 __cdecl SNearClip(uint32 camera, uint32 value) { if (!Mine()) return camera; Push(T_NEARCLIP, { camera, value }); return camera; }
inline void __cdecl SShadow(uint32 type, uint32 tex, const uint32* pos, uint32 f0, uint32 f1, uint32 f2, uint32 f3, uint32 intensity, uint32 r, uint32 g, uint32 b,
                            uint32 z, uint32 water, uint32 scale, uint32 realtime, uint32 buildings) {
    if (!Mine()) return;
    Push(T_SHADOW, { type & 0xFF, tex, pos[0], pos[1], pos[2], f0, f1, f2, f3, intensity & 0xFFFF, r & 0xFF, g & 0xFF, b & 0xFF, z, water & 0xFF, scale, realtime, buildings & 0xFF });
}
inline uint32 __cdecl SFindTarget(uint32 x, uint32 y, uint32 range, uint32 px, uint32 py, uint32 pz, uint32 outX, uint32 outY) {
    if (!Mine()) return 0; Push(T_FINDTARGET, { x, y, range, px, py, pz, outX, outY }); return P(fixTarget);
}
inline int __cdecl FakeRand() { if (!Mine()) return int((GetTickCount() * 2654435761u) >> 9) & 0x7FFF; return int((rngSeed * 2654435761u + uint32(rngCalls++) * 40503u) >> 9) & 0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// Teleport virtual de un vehiculo falso: thiscall(this, CVector por valor, bool)
inline void __fastcall STeleport(void* self, void*, uint32 x, uint32 y, uint32 z, uint32 clear) { if (!Mine()) return; Push(T_TELEPORT, { P(self), x, y, z, clear & 0xFF }); }
inline void __fastcall SUnexpectedVirtual(void* self, void*) { if (!Mine()) return; Push(T_VTABLE, { P(self) }); }

// --- zonas globales que el banco toca (se guardan al empezar y se restauran tras cada ejecucion) ---
constexpr uintptr_t HUD_A = 0xBAA388, HUD_A_SIZE = 0xE5C;        // estaticas de CHud hasta OddJob2On
constexpr uintptr_t HUD_B = 0xBAB220, HUD_B_SIZE = 12;           // posY y bandera de inicializacion de DrawBustedWastedMessage + BAB228/9
constexpr uintptr_t PLAYERS = 0xB7CD98, PLAYERS_SIZE = 0x320, PLAYER_FOCUS = 0xB7CD74;
constexpr uintptr_t GSTRING = 0xB71670, GSTRING_SIZE = 352, GXTSTRING = 0xC1B100, GXTSTRING_SIZE = 552;
constexpr uintptr_t INTRO_LINES = 0xA913E8, INTRO_LINES_SIZE = 96 * 0x44;
constexpr uintptr_t CAM_BLUR = 0xB6F0A0, CAM_BLUR_SIZE = 0x30;
constexpr uintptr_t PADS = 0xB73458, PADS_SIZE = 616;
constexpr uintptr_t HANDSHAKER = 0xB6ECA0, HANDSHAKER_SIZE = 0x94;
constexpr uintptr_t IDLE_CAM = 0xB6FDA0, IDLE_CAM_SIZE = 0x9C;
constexpr uintptr_t DW_CINEY = 0xB6FE8C, DW_CINEY_SIZE = 0x30;
struct Zone { uintptr_t address; size_t size; };
inline const std::vector<Zone> globals = {
    {HUD_A, HUD_A_SIZE}, {HUD_B, HUD_B_SIZE}, {0x8D0934, 8}, {PLAYERS, PLAYERS_SIZE}, {PLAYER_FOCUS, 1}, {GSTRING, GSTRING_SIZE}, {GXTSTRING, GXTSTRING_SIZE},
    {INTRO_LINES, INTRO_LINES_SIZE}, {CAM_BLUR, CAM_BLUR_SIZE}, {PADS, PADS_SIZE}, {HANDSHAKER, HANDSHAKER_SIZE}, {IDLE_CAM, IDLE_CAM_SIZE}, {DW_CINEY, DW_CINEY_SIZE},
    {0xB7CB5C, 4}, {0xB7CB4C, 4}, {0xB7CB49, 1}, {0xB7CB84, 4},                  // CTimer: paso, contador de fotogramas, pausa, milisegundos
    {0xC17044, 8},                                                               // RsGlobal.maximumWidth/Height
    {0xA43088, 1}, {0xB5F851, 2}, {0x96C014, 8},                                 // Replay::Mode, CutsceneMgr running/processing, Garages::MessageIDString
    {0xA444A0, 1}, {0xA444A4, 1}, {0xA44489, 1},                                 // bDisplayHud, bPlayerIsOffTheMap, bDrawOddJobTitleBeforeFade
    {0x96A7CC, 4}, {0xB72914, 4}, {0x96AB90, 1}, {0xC0BC68, 4}, {0x96A704, 2},   // exitEnterState, currArea, bGangWarsActive, zona actual, Darkel
    {0xB6F05E, 1}, {0xB6F158, 4}, {0xB6F18C, 4}, {0xB6EC2C, 1}, {0xB7CD68, 4},   // TheCamera (FOV forzado, temporizador), colision fija, pIgnoreEntity
    {0x8CC380, 16}, {0xB6EC24, 8}, {0xB6FD18, 1}, {0xB6EC08, 8}, {0x8CCB9C, 4},  // globales de CCam::Init / CacheLast / Finalise
    {0xB6FFE4, 4}, {0xB7295A, 1}, {0xC403F4, 4}, {0xC1703C, 4}, {0xB7C508, 12},  // editor, sombras, camara RW, colores del agua
    {0xB6EC18, 8}, {0xB6EC20, 1},                                                // aceleraciones de raton, gbFirstPersonRunThisFrame
    {0xC3EFA4, 4}, {0x8CCD14, 4}, {0xB7004C, 4}, {0xB6F980, 4},                  // relacion de aspecto, estaticas de 1stPerson
    {0xBAB1F0, 12},
};
inline std::vector<std::vector<uint8>> savedGlobals;
inline bool savedAllowFixedCollision{};
inline void SaveGlobals() { savedAllowFixedCollision = gAllowScriptedFixedCameraCollision; savedGlobals.clear(); for (auto& z : globals) savedGlobals.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(z.address), z.size)); }
inline void RestoreGlobals() { gAllowScriptedFixedCameraCollision = savedAllowFixedCollision; for (size_t j = 0; j < globals.size(); ++j) std::memcpy(reinterpret_cast<void*>(globals[j].address), savedGlobals[j].data(), savedGlobals[j].size()); }

// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a) { for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) a += 5 + *reinterpret_cast<int32*>(a + 1); return a; }
inline Hook* FindHook(const char* cls, const char* fn) {
    Hook* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}

// Ejecuta un caso: activa/desactiva el hook, fija el x87 a 24 bits como el juego y recoge la traza.
template<class F> inline json Run(Hook* hook, bool cand, uintptr_t addr, F&& body) {
    nrec = 0; rngCalls = 0;
    hook->State(cand);
    const bool switches = ReversingProbe::JumpsToDll(addr) == cand;   // despues de State(), no antes
    const unsigned savedFP = _controlfp(0, 0);
    _controlfp(_PC_24, _MCW_PC);
    json out = body();
    const unsigned fp = _controlfp(0, 0);
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    out["switches"] = switches; out["fp"] = fp; out["trace"] = Recs();
    return out;
}

// Rellena un texto GXT terminado en NUL de longitud len en un buffer de cap bytes (caracteres 0x20..0x7D: nunca '~', que activa etiquetas).
inline void FillText(uint8* dst, size_t cap, size_t len, uint32& s) {
    for (size_t i = 0; i < cap; ++i) dst[i] = uint8(0x20 + Rnd(s) % 0x5E);
    if (len < cap) dst[len] = 0;
}
// Pone la zona de CHud en un estado conocido: 'pattern' = byte de relleno (o aleatorio si pattern < 0)
inline void FillHud(int pattern, uint32& s) {
    auto* a = reinterpret_cast<uint8*>(HUD_A);
    for (size_t i = 0; i < HUD_A_SIZE; ++i) a[i] = pattern >= 0 ? uint8(pattern) : uint8(Rnd(s));
    auto* b = reinterpret_cast<uint8*>(HUD_B);
    for (size_t i = 0; i < HUD_B_SIZE; ++i) b[i] = pattern >= 0 ? uint8(pattern) : uint8(Rnd(s));
}
inline json HudSnap() {
    return json{ {"a", HexAt(HUD_A, HUD_A_SIZE)}, {"b", HexAt(HUD_B, HUD_B_SIZE)}, {"pager", HexAt(0x8D0934, 8)}, {"gs", HexAt(GSTRING, GSTRING_SIZE)}, {"gx", HexAt(GXTSTRING, GXTSTRING_SIZE)} };
}
// Condiciones comunes de SetHelpMessage*: mensaje grande 4, garaje, repeticion, cinematica
inline void FixHelpGuards(uint32& s, bool allowBlock) {
    Mem<uint8>(0xBAAEC0) = (allowBlock && Rnd(s) % 100 < 8) ? uint8(1 + Rnd(s) % 200) : uint8(0);
    Mem<uint8>(0x96C014) = (allowBlock && Rnd(s) % 100 < 8) ? uint8('A' + Rnd(s) % 26) : uint8(0);
    Mem<uint8>(0xA43088) = (allowBlock && Rnd(s) % 100 < 8) ? uint8(1) : uint8(Rnd(s) % 100 < 20 ? 2 : 0);
    Mem<uint8>(0xB5F851) = (allowBlock && Rnd(s) % 100 < 8) ? uint8(1 + Rnd(s) % 3) : uint8(0);
    Mem<uint8>(0xB5F852) = (allowBlock && Rnd(s) % 100 < 8) ? uint8(1) : uint8(0);
}
inline void FixScreen(uint32& s) {
    static constexpr int32 w[] = { 640, 800, 1024, 1280, 1366, 1600, 1920, 1921 }, h[] = { 480, 600, 768, 720, 768, 900, 1080, 1081 };
    const auto k = Rnd(s) % std::size(w); Mem<int32>(0xC17044) = w[k]; Mem<int32>(0xC17048) = h[k];
}
inline void FixTimer(uint32& s) {
    static constexpr float ts[] = { 0.f, 0.5f, 1.f, 1.5f, 2.3f, 5.f, 10.f, 12.5f, 50.f, 130.f };
    Mem<float>(0xB7CB5C) = ts[Rnd(s) % std::size(ts)];
    Mem<uint32>(0xB7CB4C) = Rnd(s) % 64; Mem<uint32>(0xB7CB84) = 5000u + Rnd(s) % 600000u; Mem<uint8>(0xB7CB49) = 0;
}

// =====================================================================================================================
// CHud: setters y estado puro
// =====================================================================================================================
using CaseFn = json (*)(unsigned, bool, Hook*, uintptr_t);

inline json CaseHelpDisplayed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2001); FillHud(-1, s);
    static constexpr int32 v[] = { 0, 1, 2, -1, 0x100, 0x7FFFFFFF, int32(0x80000000) };
    Mem<int32>(0xBAA474) = v[I % std::size(v)];
    return Run(hook, cand, addr, [&] {
        const uint32 r = reinterpret_cast<uint32(__cdecl*)()>(addr)();
        return json{ {"ret", r & 0xFF}, {"hud", HudSnap()} };
    });
}

inline json CaseGetYPos(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2002); FixScreen(s);
    const uint8 id = uint8(Rnd(s) % 2);
    static constexpr uint8 maxHealth[] = { 0, 1, 99, 100, 101, 102, 150, 255 };
    Mem<uint8>(PLAYERS + id * 0x190 + 0x14F) = maxHealth[Rnd(s) % std::size(maxHealth)];
    const float pos = PickM(s); const int8 offset = int8(Rnd(s) % 256);
    return Run(hook, cand, addr, [&] {
        const float r = reinterpret_cast<float(__cdecl*)(uint8, float, int8)>(addr)(id, pos, offset);
        return json{ {"ret", Bits(r)} };
    });
}

inline Buf<640> srcText;
inline json CaseSetMessage(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2003); FillHud(0xA5, s);
    static constexpr size_t lens[] = { 0, 1, 2, 17, 200, 398, 399, 400, 401, 450 };
    srcText.Fill(0xEE); const size_t len = lens[I % std::size(lens)]; FillText(srcText.p(), 600, len, s);
    const bool isNull = (I % 13) == 12;
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(const uint8*)>(addr)(isNull ? nullptr : srcText.p());
        return json{ {"hud", HudSnap()}, {"src", srcText.Dump()} };
    });
}

inline Buf<256> bigText;
inline json CaseSetBig(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2004); FillHud(-1, s);
    const uint16 style = uint16(I % 7);
    // filas de texto de los siete estilos con longitudes variadas
    for (int row = 0; row < 7; ++row) {
        FillText(reinterpret_cast<uint8*>(0xBAACC0) + row * 128, 128, Rnd(s) % 3 == 0 ? 0 : Rnd(s) % 127, s);
        FillText(reinterpret_cast<uint8*>(0xBAA940) + row * 128, 128, Rnd(s) % 3 == 0 ? 0 : Rnd(s) % 127, s);
    }
    static constexpr uint32 xs[] = { 0x00000000u, 0x00000000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xC0200000u, 0x7FC00000u };
    for (int k = 0; k < 7; ++k) Mem<uint32>(0xBAA3DC + k * 4) = xs[Rnd(s) % std::size(xs)];
    if (I % 5 != 4) Mem<uint32>(0xBAA3DC + style * 4) = (I % 17 == 16) ? 0x3F800000u : 0u;   // la mayoria de los casos entran
    Mem<float>(0xBAA398) = 123.5f; Mem<uint16>(0xBAB1E0) = 3;
    static constexpr size_t lens[] = { 0, 1, 5, 20, 126, 127, 128, 129, 150 };
    bigText.Fill(0xEE); const size_t len = lens[(I / 7) % std::size(lens)]; FillText(bigText.p(), 200, len, s);
    // el texto "anterior" del estilo: igual, con un caracter distinto, mas corto o aleatorio
    uint8* last = reinterpret_cast<uint8*>(0xBAA940) + style * 128;
    switch ((I / 63) % 4) {
    case 1: for (size_t k = 0; k < 128; ++k) last[k] = bigText.p()[k]; last[std::min<size_t>(len, 127)] = 0; break;
    case 2: for (size_t k = 0; k < 128; ++k) last[k] = bigText.p()[k]; last[std::min<size_t>(len, 127)] = 0; if (len > 3) last[len / 2] ^= 1; break;
    case 3: for (size_t k = 0; k < 128; ++k) last[k] = bigText.p()[k]; last[len / 2 < 127 ? len / 2 : 127] = 0; break;
    default: break;
    }
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint8*, uint16)>(addr)(bigText.p(), style);
        return json{ {"hud", HudSnap()}, {"msg", bigText.Dump()} };
    });
}

inline json CaseSetVehicleName(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2005); FillHud(-1, s); const uint32 ptr = Rnd(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint32)>(addr)(ptr);
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseSetZoneName(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2006); FillHud(-1, s); const uint32 ptr = Rnd(s);
    static constexpr int32 areas[] = { 0, 0, 1, 2, 18 };
    Mem<int32>(0xB72914) = areas[Rnd(s) % std::size(areas)];
    Mem<uint32>(0xBAA930) = Rnd(s) % 3 == 0 ? Rnd(s) % 6 : 0;
    const bool immediately = (I & 1) != 0;
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint32, bool)>(addr)(ptr, immediately);
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseResetWasted(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2007); FillHud(-1, s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseGetRid(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2008); FillHud(-1, s);
    static constexpr uint32 xs[] = { 0x00000000u, 0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7FC00000u };
    for (int k = 0; k < 7; ++k) Mem<uint32>(0xBAA3DC + k * 4) = xs[Rnd(s) % std::size(xs)];
    const bool flag = (I & 1) != 0;
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(bool)>(addr)(flag);
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseReInit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x2009); FillHud(I % 2 ? 0xA5 : -1, s);
    for (int k = 0; k < 8; ++k) Mem<uint8>(0x8D0934 + k) = uint8(Rnd(s));
    const uint8 focus = uint8(Rnd(s) % 2); Mem<uint8>(PLAYER_FOCUS) = focus;
    Mem<uint32>(PLAYERS + focus * 0x190 + 0xBC) = Rnd(s); Mem<uint32>(PLAYERS + focus * 0x190 + 0xE4) = Rnd(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

// SetHelpMessage / WithNumber / StatUpdate
inline Buf<640> helpText;
inline json CaseSetHelp(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x200A); FillHud(-1, s); FixHelpGuards(s, true);
    Mem<int32>(0xBAA474) = (Rnd(s) & 1) ? 1 : 0;
    static constexpr size_t lens[] = { 0, 1, 10, 50, 200, 398, 399, 400, 450 };
    helpText.Fill(0xEE); FillText(helpText.p(), 600, lens[Rnd(s) % std::size(lens)], s);
    const bool isNull = (I % 11) == 10; const bool quick = (Rnd(s) & 1), permanent = (Rnd(s) & 1), brief = (Rnd(s) & 1);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(const uint8*, bool, bool, bool)>(addr)(isNull ? nullptr : helpText.p(), quick, permanent, brief);
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseSetHelpNumber(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x200B); FillHud(-1, s); FixHelpGuards(s, true);
    static constexpr size_t lens[] = { 0, 3, 10, 50, 200, 300 };
    helpText.Fill(0xEE); const size_t len = lens[Rnd(s) % std::size(lens)]; FillText(helpText.p(), 600, len, s);
    if (len >= 3 && (Rnd(s) & 1)) { const size_t at = Rnd(s) % (len - 2); helpText.p()[at] = '~'; helpText.p()[at + 1] = '1'; helpText.p()[at + 2] = '~'; }
    static constexpr int32 numbers[] = { 0, 1, -1, 7, 42, 1000, 123456, -987654, 2147483647 };
    const int32 number = numbers[Rnd(s) % std::size(numbers)]; const bool quick = (Rnd(s) & 1), permanent = (Rnd(s) & 1);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(const uint8*, int32, bool, bool)>(addr)(helpText.p(), number, quick, permanent);
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseSetHelpStat(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x200C); FillHud(-1, s); FixHelpGuards(s, true);
    Mem<int32>(0xBAA474) = (Rnd(s) % 4 == 0) ? 1 : 0;
    for (size_t k = 0; k < GSTRING_SIZE; ++k) Mem<uint8>(GSTRING + k) = uint8(Rnd(s));
    const uint8 state = uint8(Rnd(s) % 3); const uint16 statId = uint16(Rnd(s));
    const float diff = PickM(s);
    static constexpr float maxima[] = { 0.f, 1000.f, 500.7f, -3.5f, 65535.f, 70000.5f, 1e6f, 40000.f, 32768.f };
    const float maximum = maxima[Rnd(s) % std::size(maxima)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint8, uint16, float, float)>(addr)(state, statId, diff, maximum);
        return json{ {"hud", HudSnap()} };
    });
}

// DrawFadeState
inline json CaseFade(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x200D); FillHud(0, s); FixTimer(s);
    static constexpr uint32 offsets[4][3] = { {0xBAA414, 0xBAA418, 0xBAA41C}, {0xBAA434, 0xBAA438, 0xBAA43C}, {0xBAA424, 0xBAA428, 0xBAA42C}, {0xBAA404, 0xBAA408, 0xBAA40C} };
    static constexpr uint32 states[] = { 0, 1, 2, 3, 4, 5, 5, 2, 1, 3, 6 };
    static constexpr int32 fades[] = { -1500, -5, 0, 1, 500, 999, 1000, 1001, 2000, 3000, 4000 };
    static constexpr uint32 timers[] = { 0, 5, 3000, 9990, 10000, 10001, 20000, 100 };
    for (auto& e : offsets) { Mem<uint32>(e[0]) = states[Rnd(s) % std::size(states)]; Mem<int32>(e[1]) = fades[Rnd(s) % std::size(fades)]; Mem<uint32>(e[2]) = timers[Rnd(s) % std::size(timers)]; }
    const int32 element = int32(I % 6); const int32 force = int32((I / 6) % 3 == 0 ? 1 : 0);
    return Run(hook, cand, addr, [&] {
        const float r = reinterpret_cast<float(__cdecl*)(int32, int32)>(addr)(element, force);
        return json{ {"ret", Bits(r)}, {"hud", HudSnap()} };
    });
}

// RenderArmorBar / RenderBreathBar / RenderHealthBar
inline std::array<Buf<0x600>, 2> fakeBarPed; inline Buf<0x100> fakeBarData;
inline json CaseBar(unsigned I, bool cand, Hook* hook, uintptr_t addr, int which) {
    uint32 s = Seed(I, 0x3000 + which); FillHud(0, s); FixScreen(s); FixTimer(s);
    static constexpr int16 items[] = { -1, 3, 4, 10, 8, 0, -1 };
    Mem<int16>(0xBAB1DC) = items[Rnd(s) % std::size(items)];
    const int32 id = int32(Rnd(s) % 2);
    for (auto& ped : fakeBarPed) ped.Fill(0xCD);
    fakeBarData.Fill(0xCD);
    uint8* ped = fakeBarPed[id].p();
    static constexpr float armours[] = { 0.f, 0.5f, 1.f, 1.00001f, 1.5f, 50.f, 100.f, 250.5f, -3.f, 7.25f };
    static constexpr float healths[] = { 0.f, 9.5f, 10.f, 10.9f, 50.f, 100.f, 176.f, 255.9f, 65540.f, -4.f, 9.99f, 65546.f };
    static constexpr float breaths[] = { 0.f, 5.f, 39.99f, 40.f, 78.f, 1000.f, 12.5f };
    At<float>(ped, 0x548) = armours[Rnd(s) % std::size(armours)]; At<float>(ped, 0x540) = healths[Rnd(s) % std::size(healths)];
    At<uint32>(ped, 0x480) = P(fakeBarData.p()); At<float>(fakeBarData.p(), 0x44) = breaths[Rnd(s) % std::size(breaths)];
    Mem<uint32>(PLAYERS + id * 0x190) = P(ped);
    static constexpr uint8 maxima[] = { 1, 50, 100, 150, 176, 255 };
    Mem<uint8>(PLAYERS + id * 0x190 + 0x14F) = maxima[Rnd(s) % std::size(maxima)]; Mem<uint8>(PLAYERS + id * 0x190 + 0x150) = maxima[Rnd(s) % std::size(maxima)];
    const int32 x = int32(Rnd(s) % 400), y = int32(Rnd(s) % 300);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(int32, int32, int32)>(addr)(id, x, y);
        return json{ {"ped", fakeBarPed[id].Dump()}, {"data", fakeBarData.Dump()} };
    });
}
inline json CaseArmor(unsigned I, bool c, Hook* h, uintptr_t a) { return CaseBar(I, c, h, a, 0); }
inline json CaseBreath(unsigned I, bool c, Hook* h, uintptr_t a) { return CaseBar(I, c, h, a, 1); }
inline json CaseHealth(unsigned I, bool c, Hook* h, uintptr_t a) { return CaseBar(I, c, h, a, 2); }

// DrawAmmo
inline Buf<0x800> fakeAmmoPed;
inline json CaseAmmo(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x3010); FillHud(0, s);
    for (size_t k = 0; k < GSTRING_SIZE; ++k) Mem<uint8>(GSTRING + k) = uint8(Rnd(s));
    for (size_t k = 0; k < GXTSTRING_SIZE; ++k) Mem<uint8>(GXTSTRING + k) = uint8(Rnd(s));
    fakeAmmoPed.Fill(0xCD); uint8* ped = fakeAmmoPed.p();
    const int8 slot = int8(Rnd(s) % 13); At<int8>(ped, 0x718) = slot;
    static constexpr uint32 types[] = { 0, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 16, 17, 18, 39, 40, 41, 42, 43, 44, 45, 46, 10, 11, 12, 13, 14, 15, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    static constexpr uint32 clips[] = { 0, 1, 7, 10, 30, 45, 99, 250, 1000 }, totals[] = { 0, 1, 5, 40, 500, 9000, 9998, 9999, 10000, 10001, 10050, 12345, 20000, 70000 };
    uint8* weapon = ped + 0x5A0 + int32(slot) * 0x1C;
    At<uint32>(weapon, 0) = types[Rnd(s) % std::size(types)]; At<uint32>(weapon, 8) = clips[Rnd(s) % std::size(clips)]; At<uint32>(weapon, 0xC) = totals[Rnd(s) % std::size(totals)];
    if (Rnd(s) % 4 == 0) At<uint32>(weapon, 8) = At<uint32>(weapon, 0xC) + (Rnd(s) % 3);     // clip >= total (resta negativa/cero)
    static constexpr uint16 frenzy[] = { 0, 0, 0, 1, 2, 4 }; Mem<uint16>(0x96A704) = frenzy[Rnd(s) % std::size(frenzy)];
    fixSkill = Rnd(s) % 4;
    FixScreen(s); static constexpr float alphas[] = { 0.f, 100.5f, 255.f, 300.7f, 17.9f, 254.99f };
    const float alpha = alphas[Rnd(s) % std::size(alphas)]; const int32 x = int32(Rnd(s) % 1920), y = int32(Rnd(s) % 1080);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(uint8*, int32, int32, float)>(addr)(ped, x, y, alpha);
        return json{ {"hud", HudSnap()}, {"ped", fakeAmmoPed.Dump()} };
    });
}

// =====================================================================================================================
// CHud: funciones de dibujo con CFont/CSprite2d simulados
// =====================================================================================================================
inline Buf<64> zoneTextA, zoneTextB; inline Buf<0x40> fakeZoneInfo;
inline void FillNames(uint32& s) {
    zoneTextA.Fill(0xEE); zoneTextB.Fill(0xEE);
    FillText(zoneTextA.p(), 40, 3 + Rnd(s) % 30, s); FillText(zoneTextB.p(), 40, 3 + Rnd(s) % 30, s);
}
inline uint32 PickName(uint32& s) { const uint32 r = Rnd(s) % 5; return r == 0 ? 0u : r < 3 ? P(zoneTextA.p()) : P(zoneTextB.p()); }
inline float PickBigX(uint32& s) { static constexpr uint32 b[] = { 0u, 0u, 0x3F800000u, 0x42C80000u, 0x80000000u, 0x7FC00000u }; return Flt(b[Rnd(s) % std::size(b)]); }

inline json CaseBusted(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4001); FillHud(-1, s); FixScreen(s); FixTimer(s);
    uint8* row = reinterpret_cast<uint8*>(0xBAACC0) + 2 * 128;
    FillText(row, 128, Rnd(s) % 4 == 0 ? 0 : 1 + Rnd(s) % 120, s);
    Mem<float>(0xBAA3E4) = PickBigX(s);
    static constexpr float alphas[] = { 0.f, 100.f, 250.f, 254.9f, 255.f, 255.5f, 400.f, -3.f };
    Mem<float>(0xBAA3AC) = alphas[Rnd(s) % std::size(alphas)];
    Mem<uint32>(0xBAA448) = Rnd(s) % 5; Mem<uint32>(0xBAA930) = Rnd(s) % 5;
    static constexpr uint32 guards[] = { 0, 1, 2, 3, 0x100, 0xFFFFFFFEu };
    Mem<uint32>(0xBAB224) = guards[Rnd(s) % std::size(guards)]; Mem<float>(0xBAB220) = PickM(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseAreaName(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4002); FillHud(0, s); FixScreen(s); FixTimer(s); FillNames(s);
    Mem<uint32>(0xBAB1D8) = PickName(s); if (Rnd(s) % 3 == 0) Mem<uint32>(0xBAB1D4) = Mem<uint32>(0xBAB1D8); else Mem<uint32>(0xBAB1D4) = PickName(s);
    Mem<uint32>(0xBAB1D0) = PickName(s);
    static constexpr uint32 zoneStates[] = { 0, 1, 2, 3, 4, 5, 1, 2, 3, 4 };
    Mem<uint32>(0xBAA930) = zoneStates[Rnd(s) % std::size(zoneStates)];
    static constexpr int32 fades[] = { -100, -1, 0, 1, 300, 999, 1000, 1001, 2000 }; Mem<int32>(0xBAA934) = fades[Rnd(s) % std::size(fades)];
    static constexpr uint32 timers[] = { 0, 100, 2999, 3000, 3001, 10000 }; Mem<uint32>(0xBAA938) = timers[Rnd(s) % std::size(timers)];
    Mem<uint32>(0xBAA448) = Rnd(s) % 5;
    if (Rnd(s) % 5 == 0) Mem<uint8>(0xBAB040) = uint8(1 + Rnd(s) % 200);
    Mem<uint32>(0xBAA3E0) = Rnd(s) % 6 == 0 ? 0x3F800000u : 0u; Mem<uint32>(0xBAA3E4) = Rnd(s) % 6 == 0 ? 0x3F800000u : 0u;
    Mem<uint8>(0xA444A4) = uint8(Rnd(s) & 1); Mem<uint8>(0xA444A0) = uint8(Rnd(s) & 1);
    static constexpr int32 exits[] = { 0, 0, 1, 2, 3 }; Mem<int32>(0x96A7CC) = exits[Rnd(s) % std::size(exits)];
    Mem<uint8>(0x96AB90) = uint8(Rnd(s) & 1);
    fakeZoneInfo.Fill(0xCD); for (int k = 0; k < 3; ++k) At<uint8>(fakeZoneInfo.p(), 0x0B + k) = (Rnd(s) % 5 == 0) ? uint8(0) : uint8(1 + Rnd(s) % 255);
    Mem<uint32>(0xC0BC68) = (Rnd(s) % 6 == 0) ? 0u : P(fakeZoneInfo.p());
    fixFading = (Rnd(s) % 4 == 0); fixFadeStatus = int32(Rnd(s) % 4);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseVehicleName(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4003); FillHud(0, s); FixScreen(s); FixTimer(s); FillNames(s);
    Mem<uint32>(0xBAA458) = PickName(s); Mem<uint32>(0xBAA454) = (Rnd(s) % 3 == 0) ? Mem<uint32>(0xBAA458) : PickName(s); Mem<uint32>(0xBAA444) = PickName(s);
    Mem<uint32>(0xBAA448) = Rnd(s) % 5;      // el EXE deja el alfa sin inicializar fuera de 0..4
    static constexpr int32 fades[] = { -100, -1, 0, 1, 300, 999, 1000, 1001, 2000 }; Mem<int32>(0xBAA44C) = fades[Rnd(s) % std::size(fades)];
    static constexpr uint32 timers[] = { 0, 100, 2999, 3000, 3001, 10000 }; Mem<uint32>(0xBAA450) = timers[Rnd(s) % std::size(timers)];
    Mem<uint32>(0xBAA930) = Rnd(s) % 5;
    if (Rnd(s) % 5 == 0) Mem<uint8>(0xBAB040) = uint8(1 + Rnd(s) % 200);
    Mem<uint8>(0xA444A0) = uint8(Rnd(s) & 1);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseMissionTitle(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4004); FillHud(0, s); FixScreen(s); FixTimer(s); FixHelpGuards(s, false);
    uint8* row = reinterpret_cast<uint8*>(0xBAACC0) + 1 * 128;
    FillText(row, 128, Rnd(s) % 5 == 0 ? 0 : 1 + Rnd(s) % 100, s);
    static constexpr uint32 xs[] = { 0u, 0u, 0x3F800000u, 0x42C80000u, 0x42EFC000u, 0x42F00000u, 0x43000000u, 0x7FC00000u };
    Mem<uint32>(0xBAA3E0) = xs[Rnd(s) % std::size(xs)];
    const float w = float(Mem<int32>(0xC17044));
    const float inUse[] = { -60.f, 0.f, w - 21.f, w - 20.f, w - 19.f, 300.f, 7000.f };
    Mem<float>(0xBAA3C4) = inUse[Rnd(s) % std::size(inUse)];
    static constexpr float alphas[] = { 0.f, 0.4f, 5.f, 100.f, 255.f, 300.f, 12.25f };
    Mem<float>(0xBAA3A8) = alphas[Rnd(s) % std::size(alphas)];
    Mem<uint32>(0xBAA930) = Rnd(s) % 5; Mem<int32>(0xBAA934) = int32(Rnd(s) % 2000);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseOddJob(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4005); FillHud(0, s); FixScreen(s); FixTimer(s);
    for (int rowIndex : { 1, 3, 4, 5, 6 }) FillText(reinterpret_cast<uint8*>(0xBAACC0) + rowIndex * 128, 128, Rnd(s) % 3 == 0 ? 0 : 1 + Rnd(s) % 100, s);
    const bool immediately = (Rnd(s) & 1) != 0; Mem<uint8>(0xA44489) = (Rnd(s) % 4 == 0) ? uint8(!immediately) : uint8(immediately);
    static constexpr float offTimers[] = { -5.f, 0.f, 3.5f, 1000.f, 5000.f }; Mem<float>(0xBAA398) = offTimers[Rnd(s) % std::size(offTimers)];
    static constexpr float xOffsets[] = { -400.f, -380.1f, -379.f, 0.f, 1.9f, 2.f, 2.1f, 35.f, 240.f, 380.f, -60.f };
    Mem<float>(0xBAA39C) = xOffsets[Rnd(s) % std::size(xOffsets)];
    static constexpr uint16 ons[] = { 0, 1, 2, 3, 4, 0xFFFF }; Mem<uint16>(0xBAB1E0) = ons[Rnd(s) % std::size(ons)];
    static constexpr uint16 timers[] = { 0, 1499, 1500, 1501, 65000, 32768 }; Mem<uint16>(0xBAA3A0) = timers[Rnd(s) % std::size(timers)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(bool)>(addr)(immediately);
        return json{ {"hud", HudSnap()} };
    });
}

inline json CaseScriptText(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4006); FillHud(0, s); FixScreen(s); FixTimer(s);
    uint8* lines = reinterpret_cast<uint8*>(INTRO_LINES); std::memset(lines, 0, INTRO_LINES_SIZE);
    const bool beforeFade = (Rnd(s) & 1) != 0;
    static constexpr const char* keys[] = { "FEC_TSK", "ABCDEF", "NOTFND1", "Z", "FEC_TSK" };
    const int active = 1 + int(Rnd(s) % 4);
    for (int k = 0; k < active; ++k) {
        uint8* line = lines + (Rnd(s) % 96) * 0x44;
        At<float>(line, 0x00) = PickNear(s) * 0.5f + 0.6f; At<float>(line, 0x04) = PickNear(s) * 0.5f + 1.1f;
        At<uint32>(line, 0x08) = Rnd(s);                                    // color
        At<uint8>(line, 0x0C) = uint8(Rnd(s) & 1); At<uint8>(line, 0x0D) = uint8(Rnd(s) & 1); At<uint8>(line, 0x0E) = uint8(Rnd(s) & 1);
        At<float>(line, 0x10) = PickM(s); At<float>(line, 0x14) = PickM(s);   // WrapX, CentreSize
        At<uint32>(line, 0x18) = Rnd(s); At<uint8>(line, 0x1C) = uint8(Rnd(s) & 1);   // BgColor, IsProportional
        for (int b = 0; b < 4; ++b) At<uint8>(line, 0x1D + b) = uint8(Rnd(s));         // DropShadowColor (no alineado)
        At<int8>(line, 0x21) = int8(Rnd(s) % 6); At<int8>(line, 0x22) = int8(Rnd(s) % 4 == 0 ? 0 : 1 + Rnd(s) % 3);   // DropShadow, TextEdge
        At<uint8>(line, 0x23) = (Rnd(s) % 5 == 0) ? uint8(!beforeFade) : uint8(beforeFade);   // IsDrawBeforeFade
        At<uint8>(line, 0x24) = uint8(Rnd(s) & 1);                                              // HasRightJustify
        At<int32>(line, 0x28) = int32(Rnd(s) % 4);                                              // FontStyle
        At<float>(line, 0x2C) = PickM(s); At<float>(line, 0x30) = PickM(s);                   // Pos
        const char* key = keys[Rnd(s) % std::size(keys)]; std::memset(line + 0x34, 0, 8); std::memcpy(line + 0x34, key, std::strlen(key));
        At<int32>(line, 0x3C) = int32(Rnd(s) % 1000); At<int32>(line, 0x40) = int32(Rnd(s) % 1000) - 5;
    }
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(bool)>(addr)(beforeFade);
        return json{ {"hud", HudSnap()}, {"lines", HexAt(INTRO_LINES, INTRO_LINES_SIZE)} };
    });
}

inline json CaseTripSkip(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x4007); FillHud(0, s); FixScreen(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)()>(addr)();
        return json{ {"hud", HudSnap()} };
    });
}

// =====================================================================================================================
// CCam
// =====================================================================================================================
inline Buf<0x238> camBuf;
inline void SetVec(uint8* base, size_t off, float x, float y, float z) { At<float>(base, off) = x; At<float>(base, off + 4) = y; At<float>(base, off + 8) = z; }
inline void RandVec(uint8* base, size_t off, uint32& s) { SetVec(base, off, PickM(s), PickM(s), PickM(s)); }
inline void RandUnit(uint8* base, size_t off, uint32& s) { SetVec(base, off, PickUnit(s), PickUnit(s), PickUnit(s)); }
inline std::array<uint32, 64> fakeVtable;
inline Buf<0x800> fakePlayerPed, fakeSecondPed; inline Buf<0x700> fakeVehicle, fakeVehicleB; inline Buf<0x400> fakeEntity;
inline Buf<0x48> fakeMatrix; inline Buf<0x100> fakeRwCamera;

inline json CaseWell(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5001); FixTimer(s);
    static Buf<16> cell; cell.Fill(0xCD);
    const bool angle = (I & 1) != 0;
    const float target = angle ? PickNear(s) * 3.f : PickM(s);
    const float value = angle ? PickNear(s) * 3.f + float(int32(Rnd(s) % 5) - 2) * 6.2831855f : PickM(s);
    At<float>(cell.p(), 0) = value; At<float>(cell.p(), 4) = PickM(s);
    static constexpr float tops[] = { 0.05f, 0.1f, 0.5f, 1.f, -0.5f, 2.f }, steps[] = { 0.01f, 0.05f, 0.2f, 1.f, 3.f };
    const float top = tops[Rnd(s) % std::size(tops)], step = steps[Rnd(s) % std::size(steps)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__cdecl*)(float, float*, float*, float, float, bool)>(addr)(target, &At<float>(cell.p(), 0), &At<float>(cell.p(), 4), top, step, angle);
        return json{ {"cell", cell.Dump()} };
    });
}

inline json CaseBump(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5002); camBuf.Fill(0xCD); Mem<uint32>(0xB7CB84) = Rnd(s);
    const float horizontal = PickM(s), vertical = PickM(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, float, float)>(addr)(camBuf.p(), horizontal, vertical);
        return json{ {"cam", camBuf.Dump()} };
    });
}

inline json CaseVectorsReady(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5003); camBuf.Fill(0xCD); uint8* cam = camBuf.p();
    switch (I % 7) {
    case 1: SetVec(cam, 0x190, 0.f, 0.f, 0.f); break;
    case 2: SetVec(cam, 0x190, 0.f, 0.f, 5.f); break;
    case 3: SetVec(cam, 0x190, 0.f, 0.f, -0.5f); break;
    case 4: SetVec(cam, 0x190, 1e-3f, 0.f, 0.f); break;
    case 5: SetVec(cam, 0x190, -0.f, 0.f, 1.f); break;
    default: RandVec(cam, 0x190, s); break;
    }
    RandVec(cam, 0x1B4, s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*)>(addr)(cam);
        return json{ {"cam", camBuf.Dump()} };
    });
}

// Constructor (0x517730) e Init (0x50E490): el objeto se rellena con 0xCD y se compara entero; ademas CIdleCam::Reset(true) toca gIdleCam y el pad 0.
inline void FixInitGlobals(uint32& s) {
    for (size_t k = 0; k < IDLE_CAM_SIZE; ++k) Mem<uint8>(IDLE_CAM + k) = uint8(Rnd(s));
    for (size_t k = 0; k < 16; ++k) Mem<uint8>(0x8CC380 + k) = uint8(Rnd(s));
    for (size_t k = 0; k < 8; ++k) Mem<uint8>(0xB6EC24 + k) = uint8(Rnd(s));
    Mem<uint8>(0xB6FD18) = uint8(Rnd(s)); Mem<uint32>(PADS + 0x120) = Rnd(s); Mem<uint32>(0xB7CB84) = Rnd(s);
}
inline json InitSnap() {
    return json{ {"cam", camBuf.Dump()}, {"idle", HexAt(IDLE_CAM, IDLE_CAM_SIZE)}, {"g1", HexAt(0x8CC380, 16)}, {"g2", HexAt(0xB6EC24, 8)}, {"g3", HexAt(0xB6FD18, 1)}, {"pad", HexAt(PADS + 0x120, 4)} };
}
inline json CaseConstructor(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5004); camBuf.Fill(0xCD); FixInitGlobals(s);
    return Run(hook, cand, addr, [&] {
        void* const result = reinterpret_cast<void*(__thiscall*)(void*)>(addr)(camBuf.p());
        json o = InitSnap(); o["ret"] = P(result); return o;
    });
}
inline json CaseInit(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5005); camBuf.Fill(0xCD); FixInitGlobals(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*)>(addr)(camBuf.p());
        return InitSnap();
    });
}

inline void FixRwCamera(uint32& s) {
    fakeRwCamera.Fill(0xCD); At<float>(fakeRwCamera.p(), 0x80) = PickM(s); Mem<uint32>(0xC1703C) = P(fakeRwCamera.p());
}
inline json CaseCacheLast(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5006); camBuf.Fill(0xCD); uint8* cam = camBuf.p();
    RandVec(cam, 0x190, s); RandVec(cam, 0x1B4, s); RandVec(cam, 0x19C, s); At<float>(cam, 0xB4) = PickM(s);
    for (size_t k = 0; k < DW_CINEY_SIZE; ++k) Mem<uint8>(DW_CINEY + k) = uint8(Rnd(s));
    for (size_t k = 0; k < 8; ++k) Mem<uint8>(0xB6EC08 + k) = uint8(Rnd(s));
    FixRwCamera(s);
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*)>(addr)(cam);
        return json{ {"cam", camBuf.Dump()}, {"dw", HexAt(DW_CINEY, DW_CINEY_SIZE)}, {"clip", HexAt(0xB6EC08, 8)}, {"rw", fakeRwCamera.Dump()} };
    });
}

inline Buf<16> vecSrc, vecDest;
inline json CaseFinalise(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5007); camBuf.Fill(0xCD); uint8* cam = camBuf.p(); FixTimer(s); rngSeed = Rnd(s);
    RandVec(cam, 0x190, s); RandVec(cam, 0x1B4, s); RandVec(cam, 0x19C, s); At<float>(cam, 0xB4) = PickM(s);
    vecSrc.Fill(0xCD); vecDest.Fill(0xCD); RandVec(vecSrc.p(), 0, s); RandVec(vecDest.p(), 0, s);
    if (I % 19 == 18) std::memcpy(vecDest.p(), vecSrc.p(), 12);                         // direccion nula
    if (I % 23 == 22) SetVec(vecDest.p(), 0, At<float>(vecSrc.p(), 0), At<float>(vecSrc.p(), 4), At<float>(vecSrc.p(), 8) + 5.f);   // vertical: x = y = 0
    for (size_t k = 0; k < DW_CINEY_SIZE; ++k) Mem<uint8>(DW_CINEY + k) = uint8(Rnd(s));
    for (size_t k = 0; k < 8; ++k) Mem<uint8>(0xB6EC08 + k) = uint8(Rnd(s));
    Mem<uint32>(0x8CCB9C) = Rnd(s);
    // CHandShaker: vectores moderados, matriz de resultado a cero, rangos y frecuencia coherentes
    std::memset(reinterpret_cast<void*>(HANDSHAKER), 0, HANDSHAKER_SIZE);
    for (int k = 0; k < 15; ++k) Mem<float>(HANDSHAKER + k * 4) = PickNear(s) * 0.05f;
    for (int k = 3; k < 6; ++k) Mem<float>(HANDSHAKER + k * 4) = 0.05f + 0.01f * float(Rnd(s) % 6);   // limites (divisores): nunca 0
    Mem<float>(HANDSHAKER + 0x84) = 0.5f + float(Rnd(s) % 5) * 0.1f; Mem<float>(HANDSHAKER + 0x88) = 1.5f + float(Rnd(s) % 5) * 0.1f;
    Mem<int32>(HANDSHAKER + 0x8C) = int32(5 + Rnd(s) % 30); Mem<float>(HANDSHAKER + 0x90) = 0.0005f * float(1 + Rnd(s) % 6);
    FixRwCamera(s);
    const float roll = PickNear(s) * 0.7f, fov = 70.f + PickNear(s) * 20.f, nearClip = 0.1f * float(1 + Rnd(s) % 5);
    static constexpr float shakes[] = { 0.f, 0.5f, 1.f, 2.f, 3.5f }; const float shake = shakes[Rnd(s) % std::size(shakes)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, const float*, const float*, float, float, float, float)>(addr)(cam, reinterpret_cast<const float*>(vecSrc.p()), reinterpret_cast<const float*>(vecDest.p()), roll, fov, nearClip, shake);
        return json{ {"cam", camBuf.Dump()}, {"dw", HexAt(DW_CINEY, DW_CINEY_SIZE)}, {"clip", HexAt(0xB6EC08, 8)}, {"rw", fakeRwCamera.Dump()},
                     {"frame", Mem<uint32>(0x8CCB9C)}, {"shaker", HexAt(HANDSHAKER, HANDSHAKER_SIZE)}, {"rng", rngCalls}, {"src", vecSrc.Dump()}, {"dest", vecDest.Dump()} };
    });
}

// GetCoreDataForDWCineyCamMode (0x517130): entidad falsa (tipo vehiculo) con o sin matriz y con el modelo del jugador (tiene ColModel real)
inline uint16 coreModel{};
inline Buf<0x400> coreOut;
inline json CaseCore(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5008); camBuf.Fill(0xCD); uint8* cam = camBuf.p();
    fakeEntity.Fill(0); fakeMatrix.Fill(0xCD); uint8* e = fakeEntity.p();
    const bool hasMatrix = (Rnd(s) % 10) >= 3;
    At<uint8>(e, 0x36) = 2; At<uint16>(e, 0x22) = coreModel;
    SetVec(e, 0x04, PickM(s), PickM(s), PickM(s)); At<float>(e, 0x10) = PickNear(s) * 3.f;
    if (hasMatrix) { At<uint32>(e, 0x14) = P(fakeMatrix.p()); for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u, 0x30u, 0x34u, 0x38u }) At<float>(fakeMatrix.p(), off) = PickM(s); At<uint32>(fakeMatrix.p(), 0x40) = 0; At<uint32>(fakeMatrix.p(), 0x44) = 0; }
    RandVec(e, 0x44, s); RandVec(e, 0x50, s);
    At<uint32>(cam, 0x21C) = P(e);
    for (size_t k = 0; k < DW_CINEY_SIZE; ++k) Mem<uint8>(DW_CINEY + k) = uint8(Rnd(s));
    coreOut.Fill(0xCD); uint8* o = coreOut.p();
    // salidas consecutivas: entidad*(0) vehiculo*(4) dest(8) src(20) up(32) right(44) fwd(56) vel(68) speed(80) angVel(84) angSpeed(96) colSphere(100)
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, void*, void*, void*, void*, void*, void*, void*, void*, void*, void*, void*, void*)>(addr)(
            cam, o + 0, o + 4, o + 8, o + 20, o + 32, o + 44, o + 56, o + 68, o + 80, o + 84, o + 96, o + 100);
        return json{ {"out", coreOut.Dump()}, {"cam", camBuf.Dump()}, {"entity", fakeEntity.Dump()} };
    });
}

// Get_TwoPlayer_AimVector (0x513E40)
inline Buf<0x100> fakeTargetEntity; inline Buf<16> aimOut;
inline void FixPeds(uint32& s, bool withVehicles) {
    fakePlayerPed.Fill(0); fakeSecondPed.Fill(0); fakeVehicle.Fill(0); fakeVehicleB.Fill(0);
    static constexpr uint32 types[] = { 0, 22, 23, 24, 25, 29, 30, 31, 34, 16, 4, 46, 37, 38 };
    for (Buf<0x800>* ped : { &fakePlayerPed, &fakeSecondPed }) {
        uint8* p = ped->p(); const int8 slot = int8(Rnd(s) % 13); At<int8>(p, 0x718) = slot; At<uint32>(p, 0x5A0 + int32(slot) * 0x1C) = types[Rnd(s) % std::size(types)];
        RandVec(p, 0x04, s);
        if (Rnd(s) % 4 == 0) { At<uint32>(p, 0x14) = P(fakeMatrix.p()); }
    }
    if (withVehicles) {
        if (Rnd(s) % 2) { At<uint32>(fakePlayerPed.p(), 0x58C) = P(fakeVehicle.p()); At<uint32>(fakeVehicle.p(), 0x460) = (Rnd(s) % 2) ? P(fakePlayerPed.p()) : P(fakeSecondPed.p()); RandVec(fakeVehicle.p(), 0x04, s); }
        if (Rnd(s) % 2) { At<uint32>(fakeSecondPed.p(), 0x58C) = P(fakeVehicleB.p()); At<uint32>(fakeVehicleB.p(), 0x460) = P(fakeSecondPed.p()); RandVec(fakeVehicleB.p(), 0x04, s); }
    }
    for (int k = 0; k < 12; ++k) At<uint8>(fakeMatrix.p(), k) = 0;
    Mem<uint32>(PLAYERS) = P(fakePlayerPed.p()); Mem<uint32>(PLAYERS + 0x190) = P(fakeSecondPed.p());
}
inline json CaseAim(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x5009); camBuf.Fill(0xCD); uint8* cam = camBuf.p();
    fakeMatrix.Fill(0xCD); fakeTargetEntity.Fill(0); FixPeds(s, true);
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u }) At<float>(fakeMatrix.p(), off) = PickM(s);
    At<uint32>(fakeMatrix.p(), 0x40) = 0; At<uint32>(fakeMatrix.p(), 0x44) = 0; SetVec(fakeMatrix.p(), 0x30, PickM(s), PickM(s), PickM(s));
    RandVec(fakeTargetEntity.p(), 0x04, s);
    fixTarget = (Rnd(s) % 3 == 0) ? static_cast<void*>(fakeTargetEntity.p()) : nullptr; fixSkill = Rnd(s) % 4;
    RandUnit(cam, 0x190, s); RandUnit(cam, 0x1B4, s); RandVec(cam, 0x19C, s);
    At<float>(cam, 0xB4) = 30.f + float(Rnd(s) % 800) / 10.f; At<float>(cam, 0x108) = PickNear(s) * 0.5f; At<float>(cam, 0x10C) = PickNear(s) * 0.5f;
    static constexpr float aspects[] = { 1.3333334f, 1.6f, 1.7777778f, 2.39f, 1.f }; Mem<float>(0xC3EFA4) = aspects[Rnd(s) % std::size(aspects)];
    aimOut.Fill(0xCD);
    return Run(hook, cand, addr, [&] {
        const uint32 r = reinterpret_cast<uint32(__thiscall*)(void*, void*)>(addr)(cam, aimOut.p());
        return json{ {"ret", r}, {"out", aimOut.Dump()}, {"cam", camBuf.Dump()} };
    });
}

// Process_Cam_TwoPlayer_Separate_Cars_TopDown (0x513BE0)
inline json CaseTopDown(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x500A); camBuf.Fill(0xCD); uint8* cam = camBuf.p(); FixTimer(s);
    fakeMatrix.Fill(0xCD); FixPeds(s, true);
    for (size_t off : { 0x00u, 0x04u, 0x08u, 0x10u, 0x14u, 0x18u, 0x20u, 0x24u, 0x28u }) At<float>(fakeMatrix.p(), off) = PickM(s);
    At<uint32>(fakeMatrix.p(), 0x40) = 0; At<uint32>(fakeMatrix.p(), 0x44) = 0; SetVec(fakeMatrix.p(), 0x30, PickM(s), PickM(s), PickM(s));
    static constexpr float mults[] = { 0.f, 0.45f, 0.449f, 0.5f, 0.75f, 0.7499f, 0.75001f, 1.f, 1.2f, -0.3f };
    At<float>(cam, 0xD8) = mults[Rnd(s) % std::size(mults)];
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*)>(addr)(cam);
        return json{ {"cam", camBuf.Dump()} };
    });
}

// Process_Editor (0x50F3F0)
inline json CaseEditor(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x500B); camBuf.Fill(0xCD); uint8* cam = camBuf.p(); FixRwCamera(s);
    fakeEntity.Fill(0); fakeMatrix.Fill(0); FixPeds(s, false);
    At<uint8>(cam, 0x0A) = uint8(Rnd(s) % 3 == 0); At<uint32>(cam, 0x21C) = (Rnd(s) % 3) ? P(fakeEntity.p()) : 0u;
    RandVec(fakeEntity.p(), 0x04, s);
    static constexpr float angles[] = { -1.6f, -1.5621f, -1.5f, 0.f, 1.5f, 1.5621f, 1.6f, 0.3f, -0.8f };
    At<float>(cam, 0xAC) = angles[Rnd(s) % std::size(angles)]; At<float>(cam, 0xBC) = PickNear(s);
    static constexpr float coords[] = { 0.f, 100.f, 796.f, 2749.9f, 2750.5f, 3100.5f, -2749.9f, -2750.5f, -3300.7f, 60.f, 5500.f };
    SetVec(cam, 0x19C, coords[Rnd(s) % std::size(coords)], coords[Rnd(s) % std::size(coords)], (Rnd(s) % 3) ? PickM(s) * 50.f : -449.5f - float(Rnd(s) % 10));
    uint8* pad = reinterpret_cast<uint8*>(PADS + 0x134); std::memset(pad, 0, 0x60);
    for (int half = 0; half < 2; ++half) {   // NewState (0) y OldState (0x30)
        uint8* st = pad + half * 0x30;
        At<int16>(st, 0x00) = int16(int32(Rnd(s) % 257) - 128); At<int16>(st, 0x02) = int16(int32(Rnd(s) % 257) - 128);
        for (size_t off : { 0x0Eu, 0x1Cu, 0x20u, 0x24u }) At<uint16>(st, off) = (Rnd(s) % 3 == 0) ? uint16(255) : uint16(0);
    }
    static constexpr float looks[] = { -80.f, -70.5f, -69.95f, -0.1f, 0.f, 0.05f, 69.95f, 75.f }; Mem<float>(0xB6FFE4) = looks[Rnd(s) % std::size(looks)];
    Mem<uint8>(0xB7295A) = uint8(Rnd(s) & 1); Mem<uint32>(0xC403F4) = Rnd(s);
    // jugador en foco: ped falso, a veces dentro de un vehiculo falso con tabla virtual falsa (Teleport = ranura 14)
    for (auto& v : fakeVtable) v = reinterpret_cast<uint32>(&SUnexpectedVirtual); fakeVtable[14] = reinterpret_cast<uint32>(&STeleport);
    Mem<uint8>(PLAYER_FOCUS) = 0; Mem<uint32>(PLAYERS) = P(fakePlayerPed.p());
    if (Rnd(s) % 3 == 0) { At<uint32>(fakePlayerPed.p(), 0x46C) = 0x100; At<uint32>(fakePlayerPed.p(), 0x58C) = P(fakeVehicle.p()); At<uint32>(fakeVehicle.p(), 0) = P(fakeVtable.data()); }
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, const float*, float, float, float)>(addr)(cam, reinterpret_cast<const float*>(fakeMatrix.p()), 0.f, 0.f, 0.f);
        return json{ {"cam", camBuf.Dump()}, {"entity", fakeEntity.Dump()}, {"look", Mem<float>(0xB6FFE4)}, {"ped", fakePlayerPed.Dump()}, {"rw", fakeRwCamera.Dump()} };
    });
}

// Process_Fixed (0x51D470)
inline json CaseFixed(unsigned I, bool cand, Hook* hook, uintptr_t addr) {
    uint32 s = Seed(I, 0x500C); camBuf.Fill(0xCD); uint8* cam = camBuf.p(); FixRwCamera(s);
    fakeMatrix.Fill(0); FixPeds(s, false);
    static constexpr uint32 dirs[] = { 3, 3, 0, 5, 3 }; At<uint32>(cam, 0x28) = dirs[Rnd(s) % std::size(dirs)];
    RandVec(cam, 0x160, s); RandVec(cam, 0x16C, s);
    if (Rnd(s) % 7 == 0) SetVec(cam, 0x16C, 0.f, 0.f, -1.f);
    vecDest.Fill(0xCD); RandVec(vecDest.p(), 0, s);
    if (Rnd(s) % 11 == 0) std::memcpy(vecDest.p(), cam + 0x160, 12);
    Mem<uint8>(0xB6F05E) = (Rnd(s) % 3 == 0) ? uint8(1) : uint8(Rnd(s) % 2 ? 2 : 0); Mem<float>(0xB6F158) = 40.f + float(Rnd(s) % 60);
    fixWaterHit = (Rnd(s) % 2) != 0; fixWaterLevel = At<float>(cam, 0x160 + 8) + PickNear(s) * 2.f;
    Mem<float>(0xB7C508) = float(Rnd(s) % 400) / 10.f; Mem<float>(0xB7C50C) = float(Rnd(s) % 400) / 10.f; Mem<float>(0xB7C510) = float(Rnd(s) % 400) / 10.f;
    Mem<uint8>(0xB6EC2C) = uint8(Rnd(s) % 3 == 0); /* el EXE lee 0xB6EC2C; la DLL lee su propia variable estatica (TheScripts.h): se igualan */ gAllowScriptedFixedCameraCollision = Mem<uint8>(0xB6EC2C) != 0; fixConeHit = (Rnd(s) & 1) != 0; fixConeDist = PickNear(s); SetVec(reinterpret_cast<uint8*>(fixConeOut.data()), 0, PickM(s), PickM(s), PickM(s));
    Mem<uint32>(0xB7CD68) = 0x1234u + I;
    Mem<uint8>(PLAYER_FOCUS) = 0; Mem<uint32>(PLAYERS) = P(fakePlayerPed.p());
    if (Rnd(s) % 3 == 0) { At<uint32>(fakePlayerPed.p(), 0x46C) = 0x100; At<uint32>(fakePlayerPed.p(), 0x58C) = P(fakeVehicle.p()); }
    return Run(hook, cand, addr, [&] {
        reinterpret_cast<void(__thiscall*)(void*, const float*, float, float, float)>(addr)(cam, reinterpret_cast<const float*>(vecDest.p()), 0.f, 0.f, 0.f);
        return json{ {"cam", camBuf.Dump()}, {"blur", HexAt(CAM_BLUR, CAM_BLUR_SIZE)}, {"ignore", Mem<uint32>(0xB7CD68)}, {"dest", vecDest.Dump()} };
    });
}

// =====================================================================================================================
// Tabla de funciones y bucle principal
// =====================================================================================================================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn, 36> fns{{
    {"CHud","HelpMessageDisplayed",0x588B50,0,-1,64},
    {"CHud","GetYPosBasedOnHealth",0x588B60,0,-1,512},
    {"CHud","SetMessage",0x588F60,0,-1,320},
    {"CHud","SetBigMessage",0x588FC0,0,-1,1024},
    {"CHud","SetVehicleName",0x588F50,0,-1,64},
    {"CHud","SetZoneName",0x588BB0,0,-1,256},
    {"CHud","ResetWastedText",0x589070,0,-1,64},
    {"CHud","GetRidOfAllHudMessages",0x588A50,0,-1,256},
    {"CHud","ReInitialise",0x588880,0,-1,128},
    {"CHud","SetHelpMessage",0x588BE0,0,-1,768},
    {"CHud","SetHelpMessageStatUpdate",0x588D40,0,-1,512},
    {"CHud","SetHelpMessageWithNumber",0x588E30,0,-1,512},
    {"CHud","DrawFadeState",0x58D580,0,-1,1024},
    {"CHud","RenderArmorBar",0x5890A0,0,-1,512},
    {"CHud","RenderBreathBar",0x589190,0,-1,512},
    {"CHud","RenderHealthBar",0x589270,0,-1,512},
    {"CHud","DrawAmmo",0x5893B0,0,-1,1024},
    {"CHud","DrawBustedWastedMessage",0x58CA50,0,-1,768},
    {"CHud","DrawAreaName",0x58AA50,0,-1,1024},
    {"CHud","DrawVehicleName",0x58AEA0,0,-1,1024},
    {"CHud","DrawMissionTitle",0x58D240,0,-1,768},
    {"CHud","DrawOddJobMessage",0x58CC80,0,-1,768},
    {"CHud","DrawScriptText",0x58C080,0,-1,512},
    {"CHud","DrawTripSkip",0x58A160,0,-1,128},
    {"CCam","WellBufferMe",0x509AE0,0,-1,1024},
    {"CCam","DoCamBump",0x50CB30,0,-1,128},
    {"CCam","GetVectorsReadyForRW",0x509CE0,0,-1,128},
    {"CCam","Constructor",0x517730,0,-1,64},
    {"CCam","Init",0x50E490,0,-1,64},
    {"CCam","CacheLastSettingsDWCineyCam",0x50D7A0,0,-1,128},
    {"CCam","Finalise_DW_CineyCams",0x50DD70,0,-1,512},
    {"CCam","GetCoreDataForDWCineyCamMode",0x517130,0,-1,256},
    {"CCam","Get_TwoPlayer_AimVector",0x513E40,0,-1,768},
    {"CCam","Process_Cam_TwoPlayer_Separate_Cars_TopDown",0x513BE0,0,-1,768},
    {"CCam","Process_Editor",0x50F3F0,0,-1,1024},
    {"CCam","Process_Fixed",0x51D470,0,-1,768},
}};
inline const std::array<CaseFn, 36> caseFns{{
    CaseHelpDisplayed, CaseGetYPos, CaseSetMessage, CaseSetBig, CaseSetVehicleName, CaseSetZoneName, CaseResetWasted, CaseGetRid, CaseReInit,
    CaseSetHelp, CaseSetHelpStat, CaseSetHelpNumber, CaseFade, CaseArmor, CaseBreath, CaseHealth, CaseAmmo, CaseBusted, CaseAreaName, CaseVehicleName,
    CaseMissionTitle, CaseOddJob, CaseScriptText, CaseTripSkip,
    CaseWell, CaseBump, CaseVectorsReady, CaseConstructor, CaseInit, CaseCacheLast, CaseFinalise, CaseCore, CaseAim, CaseTopDown, CaseEditor, CaseFixed,
}};

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "camhud1-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "camhud1-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("camhud1"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    coreModel = uint16(player->m_nModelIndex);
    std::string only; { std::ifstream f(dir / "camhud1-only.txt"); if (f) std::getline(f, only); }
    std::vector<Hook*> hooks(fns.size(), nullptr);
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }

    using std::bit_cast;
    // Colaboradores simulados: direccion del EXE, simbolo de la DLL (0 = sin simbolo propio, solo EXE) y stub (los stubs de EXE y DLL coinciden en pila)
    struct Site { const char* name; uintptr_t exeAddr, dll, stub; };
    const std::vector<Site> sites = {
        {"CFont::SetBackground", 0x7195C0, bit_cast<uintptr_t>(&CFont::SetBackground), bit_cast<uintptr_t>(&SBg)},
        {"CFont::SetScale", 0x719380, bit_cast<uintptr_t>(&CFont::SetScale), bit_cast<uintptr_t>(&SScale)},
        {"CFont::SetScaleForCurrentLanguage", 0x7193A0, bit_cast<uintptr_t>(&CFont::SetScaleForCurrentLanguage), bit_cast<uintptr_t>(&SScaleLang)},
        {"CFont::SetOrientation", 0x719610, bit_cast<uintptr_t>(&CFont::SetOrientation), bit_cast<uintptr_t>(&SOrient)},
        {"CFont::SetCentreSize", 0x7194E0, bit_cast<uintptr_t>(&CFont::SetCentreSize), bit_cast<uintptr_t>(&SCentre)},
        {"CFont::SetProportional", 0x7195B0, bit_cast<uintptr_t>(&CFont::SetProportional), bit_cast<uintptr_t>(&SProp)},
        {"CFont::SetEdge", 0x719590, bit_cast<uintptr_t>(&CFont::SetEdge), bit_cast<uintptr_t>(&SEdge)},
        {"CFont::SetDropColor", 0x719510, bit_cast<uintptr_t>(&CFont::SetDropColor), bit_cast<uintptr_t>(&SDropColor)},
        {"CFont::SetFontStyle", 0x719490, bit_cast<uintptr_t>(&CFont::SetFontStyle), bit_cast<uintptr_t>(&SStyle)},
        {"CFont::SetColor", 0x719430, bit_cast<uintptr_t>(&CFont::SetColor), bit_cast<uintptr_t>(&SColor)},
        {"CFont::PrintString", 0x71A700, bit_cast<uintptr_t>(&CFont::PrintString), bit_cast<uintptr_t>(&SPrint)},
        {"CFont::PrintStringFromBottom", 0x71A820, bit_cast<uintptr_t>(&CFont::PrintStringFromBottom), bit_cast<uintptr_t>(&SPrintBottom)},
        {"CFont::SetRightJustifyWrap", 0x7194F0, bit_cast<uintptr_t>(&CFont::SetRightJustifyWrap), bit_cast<uintptr_t>(&SRjWrap)},
        {"CFont::SetJustify", 0x719600, bit_cast<uintptr_t>(&CFont::SetJustify), bit_cast<uintptr_t>(&SJustify)},
        {"CFont::SetSlant", 0x719420, bit_cast<uintptr_t>(&CFont::SetSlant), bit_cast<uintptr_t>(&SSlant)},
        {"CFont::SetWrapx", 0x7194D0, bit_cast<uintptr_t>(&CFont::SetWrapx), bit_cast<uintptr_t>(&SWrapx)},
        {"CFont::SetBackgroundColor", 0x7195E0, bit_cast<uintptr_t>(&CFont::SetBackgroundColor), bit_cast<uintptr_t>(&SBgColor)},
        {"CFont::SetDropShadowPosition", 0x719570, bit_cast<uintptr_t>(&CFont::SetDropShadowPosition), bit_cast<uintptr_t>(&SDropPos)},
        {"CSprite2d::DrawBarChart", 0x728640, bit_cast<uintptr_t>(&CSprite2d::DrawBarChart), bit_cast<uintptr_t>(&SBar)},
        {"CSprite2d::Draw(CRect,CRGBA)", 0x728350, bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&, const CRGBA&)>(&CSprite2d::Draw)), bit_cast<uintptr_t>(&SSpriteDraw)},
        {"CPed::GetWeaponSkill", 0x5E6580, bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)), bit_cast<uintptr_t>(&SWeaponSkill)},
        {"CMessages::AddToPreviousBriefArray", 0x69DD50, bit_cast<uintptr_t>(&CMessages::AddToPreviousBriefArray), bit_cast<uintptr_t>(&SBrief)},
        {"CCamera::GetFading", 0x50ADE0, bit_cast<uintptr_t>(&CCamera::GetFading), bit_cast<uintptr_t>(&SFading)},
        {"CCamera::GetScreenFadeStatus", 0x50AE20, bit_cast<uintptr_t>(&CCamera::GetScreenFadeStatus), bit_cast<uintptr_t>(&SFadeStatus)},
        {"CTheScripts::DrawScriptSpritesAndRectangles", 0x464980, bit_cast<uintptr_t>(&CTheScripts::DrawScriptSpritesAndRectangles), bit_cast<uintptr_t>(&SScriptSprites)},
        {"CWaterLevel::GetWaterLevel", 0x6EB690, bit_cast<uintptr_t>(static_cast<bool(*)(float, float, float, float&, uint8, CVector*)>(&CWaterLevel::GetWaterLevel)), bit_cast<uintptr_t>(&SWater)},
        {"CCamera::ConeCastCollisionResolve", 0x51A5D0, bit_cast<uintptr_t>(&CCamera::ConeCastCollisionResolve), bit_cast<uintptr_t>(&SCone)},
        {"RwCameraSetNearClipPlane", 0x7EE1D0, 0, bit_cast<uintptr_t>(&SNearClip)},
        {"CShadows::StoreShadowToBeRendered(texture)", 0x707390, bit_cast<uintptr_t>(static_cast<void(*)(uint8, RwTexture*, const CVector&, float, float, float, float, int16, uint8, uint8, uint8, float, bool, float, CRealTimeShadow*, bool)>(&CShadows::StoreShadowToBeRendered)), bit_cast<uintptr_t>(&SShadow)},
        {"CWeapon::FindNearestTargetEntityWithScreenCoors", 0x73E240, bit_cast<uintptr_t>(&CWeapon::FindNearestTargetEntityWithScreenCoors), bit_cast<uintptr_t>(&SFindTarget)},
        {"CGeneral::GetRandomNumber", 0x821B1E, bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), bit_cast<uintptr_t>(&FakeRand)},
    };
    bool known = true; json siteInfo = json::array(); std::vector<uintptr_t> bodies;
    // El cuerpo se calcula ANTES de parchear ningun simbolo: tras parchear, el E9 del simbolo apunta al stub y Body() devolveria el propio stub.
    for (auto& s : sites) bodies.push_back(s.dll ? Body(s.dll) : 0);
    for (size_t k = 0; k < sites.size(); ++k) {
        const auto& s = sites[k];
        const bool ok = s.exeAddr >= 0x401000 && s.exeAddr < 0x1000000 && (s.dll == 0 || (s.dll >= 0x1000000 && bodies[k] >= 0x1000000));
        known &= ok; siteInfo.push_back({ {"name", s.name}, {"exe", s.exeAddr}, {"dll", s.dll}, {"body", bodies[k]}, {"ok", ok} });
    }
    // bytes iniciales de todas las entradas parcheables y de las funciones probadas, para comprobar la restauracion
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (size_t k = 0; k < sites.size(); ++k) for (auto a : { sites[k].exeAddr, sites[k].dll, bodies[k] }) if (a) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });
    SaveGlobals();
    for (size_t k = 0; k < fakeVtable.size(); ++k) fakeVtable[k] = 0;
    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP}, {"core_model", coreModel} });

    bool passed = known; int pair = 0;
    if (known) {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        for (size_t k = 0; k < sites.size(); ++k) {
            const auto& s = sites[k];
            patches.push_back(std::make_unique<EntryPatch>(s.exeAddr, reinterpret_cast<void*>(s.stub)));
            if (s.dll) {
                // En el sitio de rand el DLL devuelve uint16 y el EXE int: stubs distintos con el mismo contenido
                void* const dllStub = (s.exeAddr == 0x821B1E) ? reinterpret_cast<void*>(&FakeRandomNumber) : reinterpret_cast<void*>(s.stub);
                patches.push_back(std::make_unique<EntryPatch>(s.dll, dllStub));
                if (bodies[k] != s.dll) patches.push_back(std::make_unique<EntryPatch>(bodies[k], dllStub));
            }
        }
        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) continue;
            for (int i = 0; i < fn.cases; ++i) {
                json original = caseFns[f](unsigned(i), false, hooks[f], fn.address); RestoreGlobals();
                json candidate = caseFns[f](unsigned(i), true, hooks[f], fn.address); RestoreGlobals();
                const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
                const std::string od = original.dump(), cd = candidate.dump();
                passed &= ok; failures += !ok;
                if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                                {"original", {{"trace", original["trace"]}, {"state_fnv", Fnv(od.data(), od.size())}}},
                                {"candidate", {{"trace", candidate["trace"]}, {"state_fnv", Fnv(cd.data(), cd.size())}}},
                                {"events", original["trace"].size()} });
                else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                    Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                            {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
            }
            Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
        }
        patches.clear();   // restaura las entradas
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= savedGlobals[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].address), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored} });
}
}
