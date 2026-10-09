#pragma once
// Banco dentro del motor (flujo simple, como PadMouse1/AudioFinal1): CAERadioTrackManager (29 funciones) y CFont (32
// funciones) de GTA SA 1.0 US. Original (hook desactivado) frente a candidato (hook activo), por funcion.
//
// Seguridad: nunca se toca el gestor de radio real (AERadioTrackManager 0x8CB6F8) ni el audio: el objeto es un
// CAERadioTrackManager FALSO de 0x370 bytes con guardas, y todos los colaboradores que tocarian hardware, ficheros,
// textos, TXD o dibujo (AEAudioHardware, AudioEngine, CText, CFileMgr/CFileLoader, CTxdStore, CSprite2d, DrawRect/DrawWindow,
// RenderFontBuffer...) se sustituyen con el mismo stub en la direccion del EXE y en el simbolo de la DLL (EntryPatch),
// con traza de argumentos. Los globales tocados se guardan y se restauran por ejecucion y al final.
//
// - Body(): se calcula la direccion real (cadena de E9) de TODOS los simbolos de la DLL ANTES de crear ningun EntryPatch.
// - Los hilos ajenos (audio, streaming) no tocan la traza: los stubs solo registran en el hilo del banco (Mine()).
// - x87 a 24 bits durante cada llamada, como el juego.
// - Se enmascara el relleno: los objetos se rellenan con 0xCD y se compara el objeto completo (Reset/Initialise copian el
//   bloque de 60 bytes con rep movs en el EXE y con memcpy en la DLL, asi que el relleno copiado es el mismo).
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AERadioTrackManager.h"
#include "AEVehicleAudioEntity.h"
#include "AEAudioHardware.h"
#include "AudioEngine.h"
#include "AEAudioUtility.h"
#include "AEUserRadioTrackManager.h"
#include "GenericGameStorage.h"
#include "Font.h"
#include "Sprite.h"
#include "Sprite2d.h"
#include "TxdStore.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "Messages.h"
#include "../ProbeRuntime.hpp"

namespace RadioFont1Live {
using ReversingProbe::EntryPatch;
inline bool done{}; inline std::ofstream log; inline json trace;
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline void Tr(json&& j) { if (Mine()) trace.push_back(std::move(j)); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
inline uint64 Fnv(const void* p, size_t n, uint64 h = 1469598103934665603ull) { const auto* b = static_cast<const uint8*>(p); for (size_t k = 0; k < n; ++k) { h ^= b[k]; h *= 1099511628211ull; } return h; }
inline std::string Hex(const void* p, size_t n) { static const char* d = "0123456789ABCDEF"; std::string r; r.reserve(n * 2); const auto* b = static_cast<const uint8*>(p); for (size_t k = 0; k < n; ++k) { r.push_back(d[b[k] >> 4]); r.push_back(d[b[k] & 15]); } return r; }
// Texto seguro para JSON (solo ASCII imprimible; el resto se escribe como \xNN)
inline std::string Esc(const char* s, size_t n) { std::string r; r.reserve(n); char b[8]; for (size_t k = 0; k < n; ++k) { const uint8 c = uint8(s[k]); if (c >= 0x20 && c < 0x7F && c != '\\') r.push_back(char(c)); else { std::snprintf(b, sizeof(b), "\\x%02X", unsigned(c)); r += b; } } return r; }
inline std::string EscZ(const char* s, size_t cap = 600) { if (!s) return "<null>"; size_t n = 0; while (n < cap && s[n]) ++n; return Esc(s, n); }

// --- diseno de los objetos (si algo no coincide, el banco aborta con "layout") ---
static_assert(sizeof(CAERadioTrackManager) == 0x370);
static_assert(offsetof(CAERadioTrackManager, m_nMode) == 0x68);
static_assert(offsetof(CAERadioTrackManager, m_RequestedSettings) == 0x88);
static_assert(offsetof(CAERadioTrackManager, m_ActiveSettings) == 0xC4);
static_assert(offsetof(CAERadioTrackManager, m_aRadioState) == 0x100);
static_assert(offsetof(CAERadioTrackManager, field_368) == 0x368);
static_assert(offsetof(CAERadioTrackManager, m_nUserTrackPlayMode) == 0x36C);
static_assert(offsetof(CAERadioTrackManager, m_aListenTimes) == 0x18);
static_assert(offsetof(CAERadioTrackManager, m_nSavedTimeMs) == 0x58);
static_assert(offsetof(CAERadioTrackManager, m_nRetuneStartedTime) == 0x5C);
static_assert(offsetof(CAERadioTrackManager, m_HwClientHandle) == 0x64);
static_assert(offsetof(CAERadioTrackManager, m_iRadioStationMenuRequest) == 0x78);
static_assert(offsetof(tRadioSettings, StationID) == 0x25);
static_assert(offsetof(tRadioSettings, BassGain) == 0x28);
static_assert(offsetof(tRadioSettings, TrackTypes) == 0x2C);
static_assert(offsetof(tRadioSettings, TrackIndices) == 0x33);
static_assert(sizeof(tRadioState) == 0x2C);
static_assert(offsetof(tVehicleAudioSettings, BassSetting) == 0x06);
static_assert(offsetof(tVehicleAudioSettings, BassFactor) == 0x08);
static_assert(offsetof(tVehicleAudioSettings, RadioStation) == 0x1A);
static_assert(offsetof(tVehicleAudioSettings, RadioType) == 0x1B);
static_assert(offsetof(CAEUserRadioTrackManager, m_nUserTracksCount) == 0x06);
static_assert(sizeof(CFontChar) == 0x30);
static_assert(sizeof(tFontData) == 0xD2);

// --- generador pseudoaleatorio determinista ---
struct Rng {
    uint64 s;
    explicit Rng(uint64 seed) : s(seed * 0x9E3779B97F4A7C15ull + 0xD1B54A32D192ED03ull) { for (int k = 0; k < 4; ++k) u(); }
    uint32 u() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return uint32(s >> 16); }
    int32 r(int32 lo, int32 hi) { return lo + int32(u() % uint32(hi - lo + 1)); }
    bool p(uint32 percent) { return u() % 100 < percent; }
    template<class T> T pick(std::initializer_list<T> l) { return *(l.begin() + (u() % l.size())); }
    // flotantes sin sNaN (el x87 quieta las sNaN al copiar con fld/fstp; la DLL copia en bruto)
    float fl() { return pick<float>({0.f, -0.f, 1.f, -1.f, 0.5f, 0.75f, 2.f, 3.75f, 0.125f, 100.f, -37.5f, 640.f, 1e-20f, 1e20f, std::bit_cast<float>(0x7FC00000u), std::bit_cast<float>(0x7F800000u), std::bit_cast<float>(0xFF800000u), float(r(1, 2000)) / 64.f}); }
    float fin() { return pick<float>({0.f, 0.5f, 1.f, 1.25f, 2.f, 3.75f, 0.125f, 0.0625f, 10.f, 50.f, 111.5f, 320.f, float(r(1, 4000)) / 128.f, float(r(-300, 300)) / 16.f}); }
};

// --- regiones globales tocadas (se restauran por ejecucion) ---
struct Reg { const char* name; uintptr_t addr; size_t size; };
inline std::vector<Reg> regs;
inline std::vector<std::vector<uint8>> base;
inline void RestoreBase() { for (size_t k = 0; k < regs.size(); ++k) std::memcpy(reinterpret_cast<void*>(regs[k].addr), base[k].data(), regs[k].size); }

// --- objeto falso de radio con guardas ---
alignas(16) inline std::array<uint8, 16 + sizeof(CAERadioTrackManager) + 16> objBuf{};
inline CAERadioTrackManager* M() { return reinterpret_cast<CAERadioTrackManager*>(objBuf.data() + 16); }
// buffer de tVehicleAudioSettings falso (+ opciones de vehiculo de los stubs)
alignas(16) inline std::array<uint8, 64> optBuf{};
inline tVehicleAudioSettings* Opt() { return reinterpret_cast<tVehicleAudioSettings*>(optBuf.data()); }
inline int optMode{};       // 0: nullptr; 1: &optBuf
// aleatoriedad del juego (rand del EXE y CGeneral::GetRandomNumber)
inline uint32 rngSeed{}, rngCalls{};
inline int __cdecl FakeRand() { if (!Mine()) return 0; Tr(json::array({"rand"})); return int((rngSeed * 2654435761u + rngCalls++ * 40503u) >> 9) & 0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- estado de los stubs por ejecucion ---
inline std::array<float, 512> statTable{};
inline uint32 probBits{}, probIdx{}, queueBits{}, queueIdx{};
inline int userMode{}, ambRadio{}, ambTrack{}, ambOverride{}, vehActive{}, trackResult{}, vehPresent{};
inline int txdCalls{};
inline std::vector<std::string> fileLines; inline size_t fileCursor{};
inline uint32 loadCounter{};
inline std::array<char, 40> textBuf{};
inline FILESTREAM fileToken() { return reinterpret_cast<FILESTREAM>(uintptr_t(0x13579B)); }
inline bool IsSelf(const void* p) { return p == M(); }

// =========================== stubs de radio ===========================
inline float __cdecl SStat(eStats stat) { const int s = int(stat); const float v = (Mine() && s >= 0 && s < 512) ? statTable[size_t(s)] : 0.f; Tr(json::array({"stat", s, F(v)})); return v; }
inline uint8 __fastcall SUserMode(void*, void*) { const uint8 r = Mine() ? uint8(userMode) : uint8(0); Tr(json::array({"usermode", int(r)})); return r; }
inline void __fastcall SHwBass(void* self, void*, eBassSetting s, float g) { if (!Mine()) return; Tr(json::array({"hw_bass", uintptr_t(self) == 0xB5F8B8, int(uint8(s)), F(g)})); }
inline void __fastcall SHwFlags(void* self, void*, int16 ch, uint16 id, int16 flags) { if (!Mine()) return; Tr(json::array({"hw_flags", uintptr_t(self) == 0xB5F8B8, int(uint16(ch)), int(id), int(uint16(flags))})); }
inline void __fastcall SHwPlayback(void* self, void*) { if (!Mine()) return; Tr(json::array({"hw_playback", uintptr_t(self) == 0xB5F8B8})); }
inline void __fastcall SHwFreq(void* self, void*, int16 ch, uint16 slot, float f) { if (!Mine()) return; Tr(json::array({"hw_freq", uintptr_t(self) == 0xB5F8B8, int(uint16(ch)), int(slot), F(f)})); }
inline void __fastcall SVolumes(void* self, void*) { if (!Mine()) return; Tr(json::array({"volumes", IsSelf(self)})); }
inline tVehicleAudioSettings* __cdecl SVehSettings() { if (!Mine()) return nullptr; Tr(json::array({"veh_settings", optMode})); return optMode ? Opt() : nullptr; }
inline bool __cdecl SAmbRadio() { if (!Mine()) return false; const bool r = ambRadio != 0; Tr(json::array({"amb_radio", r})); return r; }
inline bool __cdecl SAmbTrack() { if (!Mine()) return false; const bool r = ambTrack != 0; Tr(json::array({"amb_track", r})); return r; }
inline bool __cdecl SAmbOverride() { if (!Mine()) return false; const bool r = ambOverride != 0; Tr(json::array({"amb_override", r})); return r; }
inline void __fastcall SStopAmb(void* self, void*, bool a) { if (!Mine()) return; Tr(json::array({"stop_amb", uintptr_t(self) == 0xB6BC90, a})); }
inline void __fastcall SReport(void* self, void*, eAudioEvents e, float v, float sp) { if (!Mine()) return; Tr(json::array({"report", uintptr_t(self) == 0xB6BC90, int(e), F(v), F(sp)})); }
inline void __fastcall SEngStop(void* self, void*, tVehicleAudioSettings* s, bool d) { if (!Mine()) return; Tr(json::array({"engine_stop", uintptr_t(self) == 0xB6BC90, s != nullptr, d})); }
inline void __fastcall SMgrStop(void* self, void*, tVehicleAudioSettings* s, bool d) { if (!Mine()) return; Tr(json::array({"mgr_stop", IsSelf(self), s != nullptr, d})); }
inline void __fastcall SStart(void* self, void*, eRadioID id, eBassSetting b, float g, bool skip) { if (!Mine()) return; Tr(json::array({"start", IsSelf(self), int(int8(id)), int(uint8(b)), F(g), skip})); }
inline bool __cdecl SVehActive() { if (!Mine()) return false; const bool r = vehActive != 0; Tr(json::array({"veh_active", r})); return r; }
inline bool __fastcall STrackRadio(void* self, void*, eRadioID id, bool skip) { if (!Mine()) return false; const bool r = trackResult != 0; Tr(json::array({"track_radio", IsSelf(self), int(int8(id)), skip, r})); if (r) static_cast<CAERadioTrackManager*>(self)->m_RequestedSettings.PlayTime = 777; return r; }
inline void __fastcall SChoose(void* self, void*, eRadioID id) { if (!Mine()) return; Tr(json::array({"choose", IsSelf(self), int(int8(id))})); static_cast<CAERadioTrackManager*>(self)->m_RequestedSettings.TrackQueue.data()[0] = 555; }
inline bool __fastcall SQueue(void* self, void*, eRadioID id, int8* count, int8 state, tRadioSettings& settings) {
    if (!Mine()) return false;
    const bool r = ((queueBits >> (queueIdx++ % 32)) & 1) != 0;
    Tr(json::array({"queue", IsSelf(self), int(int8(id)), int(*count), int(state), r}));
    if (r && *count >= 0 && *count < 5) {
        settings.TrackQueue.data()[*count] = 300 + int32(state);
        settings.TrackTypes.data()[*count] = state;
        settings.TrackIndices.data()[*count] = int8(40 + state);
        ++*count;
    }
    return r;
}
inline bool __cdecl SProb(float chance) { if (!Mine()) return false; const bool r = ((probBits >> (probIdx++ % 32)) & 1) != 0; Tr(json::array({"probability", F(chance), r})); return r; }
inline const GxtChar* __fastcall STextGet(void*, void*, const char* key) {
    if (!Mine()) return reinterpret_cast<const GxtChar*>("");
    Tr(json::array({"text_get", EscZ(key, 32)}));
    std::snprintf(textBuf.data(), textBuf.size(), "txt_%.30s", key);
    return reinterpret_cast<const GxtChar*>(textBuf.data());
}
inline CVehicle* __cdecl SFindVehicle(int32 player, bool remote) { if (!Mine()) return nullptr; Tr(json::array({"find_vehicle", player, remote, vehPresent})); return vehPresent ? reinterpret_cast<CVehicle*>(&optBuf[32]) : nullptr; }
inline bool __cdecl SSaveWb(void* p, int32 size) {
    if (!Mine()) return true;
    const uintptr_t a = reinterpret_cast<uintptr_t>(p);
    uint64 v = 0; std::memcpy(&v, p, size_t(size > 0 && size <= 8 ? size : 0));
    Tr(json::array({"save", a, size, uint64(v)})); return true;
}
inline bool __cdecl SLoadWb(void* p, int32 size) {
    if (!Mine()) return true;
    const uintptr_t a = reinterpret_cast<uintptr_t>(p);
    Tr(json::array({"load", a, size}));
    if (size > 0 && size <= 8) { uint8 tmp[8]; for (int k = 0; k < size; ++k) tmp[k] = uint8(loadCounter * 37u + uint32(k) * 11u + 3u); ++loadCounter; std::memcpy(p, tmp, size_t(size)); }
    return true;
}

// =========================== stubs de CFont ===========================
inline void __cdecl SEmit(float x, float y, const char* b, const char* e, float sp) {
    if (!Mine()) return;
    const size_t n = (e >= b && size_t(e - b) <= 600) ? size_t(e - b) : 0;
    Tr(json::array({"line", F(x), F(y), Esc(b, n), F(sp), e >= b}));
}
inline void __cdecl SDrawRect(const CRect& r, const CRGBA& c) { if (!Mine()) return; Tr(json::array({"draw_rect", Hex(&r, 16), Hex(&c, 4)})); }
inline void __fastcall SDrawWindow(void* self, void*, const CRect& r, const char* key, uint8 col, CRGBA back, bool unused, bool bg) { if (!Mine()) return; Tr(json::array({"draw_window", uintptr_t(self) == 0xBA6748, Hex(&r, 16), key != nullptr, int(col), Hex(&back, 4), unused, bg})); }
inline void __cdecl SInitSprite() { if (!Mine()) return; Tr(json::array({"init_sprite_buffer"})); }
inline void __cdecl SRenderBuf() { if (!Mine()) return; Tr(json::array({"render_font_buffer"})); }
inline void __cdecl SKeys(GxtChar* s) { if (!Mine()) return; Tr(json::array({"insert_keys", EscZ(reinterpret_cast<const char*>(s), 600)})); }
inline void __cdecl SPrint(float x, float y, const GxtChar* t) { if (!Mine()) return; Tr(json::array({"print", F(x), F(y), EscZ(reinterpret_cast<const char*>(t))})); }
inline void __cdecl SDraw() { if (!Mine()) return; Tr(json::array({"draw_fonts"})); }
inline int32 __cdecl SSetDir(const char* p) { if (!Mine()) return 0; Tr(json::array({"set_dir", EscZ(p, 64)})); return 0; }
inline FILESTREAM __cdecl SOpen(const char* p, const char* m) { if (!Mine()) return nullptr; Tr(json::array({"open", EscZ(p, 64), EscZ(m, 8)})); return fileToken(); }
inline int32 __cdecl SClose(FILESTREAM f) { if (!Mine()) return 0; Tr(json::array({"close", f == fileToken()})); return 0; }
inline char* __cdecl SLoadLine(FILESTREAM f) {
    if (!Mine()) return nullptr;
    char* r = fileCursor < fileLines.size() ? fileLines[fileCursor].data() : nullptr;
    Tr(json::array({"load_line", f == fileToken(), int(fileCursor), r ? EscZ(r, 200) : std::string("<eof>")}));
    if (r) ++fileCursor; return r;
}
inline int32 __cdecl SAddSlot(const char* n) { if (!Mine()) return -1; Tr(json::array({"add_slot", EscZ(n, 32)})); return 40 + txdCalls++; }
inline bool __cdecl SLoadTxd(int32 s, const char* f) { if (!Mine()) return true; Tr(json::array({"load_txd", s, EscZ(f, 64)})); return true; }
inline void __cdecl SAddRef(int32 s) { if (!Mine()) return; Tr(json::array({"add_ref", s})); }
inline void __cdecl SPushTxd() { if (!Mine()) return; Tr(json::array({"push_txd"})); }
inline void __cdecl SSetTxd(int32 s) { if (!Mine()) return; Tr(json::array({"set_txd", s})); }
inline void __cdecl SPopTxd() { if (!Mine()) return; Tr(json::array({"pop_txd"})); }
inline void __fastcall SSetTexture(CSprite2d* self, void*, const char* n, const char* m) { if (!Mine()) return; Tr(json::array({"set_texture", int((reinterpret_cast<intptr_t>(self) - 0xC71AD0) / 4), EscZ(n, 32), EscZ(m, 32)})); }
inline void __fastcall SSpriteDelete(CSprite2d* self, void*) { if (!Mine()) return; Tr(json::array({"sprite_delete", int((reinterpret_cast<intptr_t>(self) - 0xC71AD0) / 4)})); }
inline int32 __cdecl SFindSlot(const char* n) { if (!Mine()) return -1; Tr(json::array({"find_slot", EscZ(n, 32)})); return 7; }
inline void __cdecl SRemoveSlot(int32 s) { if (!Mine()) return; Tr(json::array({"remove_slot", s})); }
inline void __cdecl SLoadFontValues() { if (!Mine()) return; Tr(json::array({"load_font_values"})); }

// =========================== registro de sitios parcheables ===========================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}
// Sigue la cadena de E9 de la DLL (thunk incremental -> cuerpo) y NO entra en el EXE (lecciones 11 y 20).
inline uintptr_t Body(uintptr_t p) {
    for (int k = 0; k < 8 && *reinterpret_cast<const uint8*>(p) == 0xE9; ++k) { const uintptr_t t = p + 5 + *reinterpret_cast<const int32*>(p + 1); if (t >= 0x400000 && t < 0x1000000) break; p = t; }
    return p;
}
struct Access : CAERadioTrackManager {
    using CAERadioTrackManager::AddMusicTrackIndexToHistory; using CAERadioTrackManager::AddIdentIndexToHistory;
    using CAERadioTrackManager::AddAdvertIndexToHistory; using CAERadioTrackManager::AddDJBanterIndexToHistory;
    using CAERadioTrackManager::CheckForMissionStatsChanges; using CAERadioTrackManager::CheckForPause;
    using CAERadioTrackManager::CheckForStationRetuneDuringPause; using CAERadioTrackManager::ChooseTracksForStation;
    using CAERadioTrackManager::QueueUpTracksForStation; using CAERadioTrackManager::TrackRadioStation;
};
struct Site { const char* name; uintptr_t exe, dll, body, stub; };
enum SiteId {
    SITE_EMIT, SITE_DRAWRECT, SITE_DRAWWIN, SITE_INITSPR, SITE_RENDERBUF, SITE_KEYS, SITE_SETDIR, SITE_OPEN, SITE_CLOSE, SITE_LOADLINE, SITE_ADDSLOT, SITE_LOADTXD, SITE_ADDREF,
    SITE_PUSHTXD, SITE_SETTXD, SITE_POPTXD, SITE_FINDSLOT, SITE_REMSLOT, SITE_SETTEX, SITE_SPRDEL, SITE_LOADFV, SITE_PRINT, SITE_DRAWFONTS,
    SITE_STAT, SITE_USERMODE, SITE_HWBASS, SITE_HWFLAGS, SITE_HWPLAY, SITE_HWFREQ, SITE_VOLUMES, SITE_VEHSET, SITE_AMBRADIO, SITE_AMBTRACK, SITE_AMBOVR, SITE_STOPAMB, SITE_REPORT,
    SITE_ENGSTOP, SITE_MGRSTOP, SITE_START, SITE_VEHACTIVE, SITE_TRACKRADIO, SITE_CHOOSE, SITE_QUEUE, SITE_PROB, SITE_TEXTGET, SITE_FINDVEH, SITE_SAVEWB, SITE_LOADWB,
    SITE_RAND, SITE_RANDNUM, SITE__COUNT
};
inline std::array<Site, SITE__COUNT> sites{};
template<class T> inline uintptr_t A(T f) { return std::bit_cast<uintptr_t>(f); }
inline uintptr_t St(void* f) { return reinterpret_cast<uintptr_t>(f); }
inline void SetSite(SiteId id, const char* name, uintptr_t exe, uintptr_t dll, void* stub) {
    sites[size_t(id)] = Site{name, exe, dll, dll ? Body(dll) : 0, St(stub)};
}
// Calcula las direcciones (y Body) de TODOS los sitios antes de crear ningun parche.
inline void BuildSites() {
    SetSite(SITE_EMIT, "FontEmitLine", 0x719B40, 0, reinterpret_cast<void*>(&SEmit));
    SetSite(SITE_DRAWRECT, "CSprite2d::DrawRect", 0x727B60, A(static_cast<void(*)(const CRect&, const CRGBA&)>(&CSprite2d::DrawRect)), reinterpret_cast<void*>(&SDrawRect));
    SetSite(SITE_DRAWWIN, "CMenuManager::DrawWindow", 0x573EE0, A(&CMenuManager::DrawWindow), reinterpret_cast<void*>(&SDrawWindow));
    SetSite(SITE_INITSPR, "CSprite::InitSpriteBuffer", 0x70CFB0, A(&CSprite::InitSpriteBuffer), reinterpret_cast<void*>(&SInitSprite));
    SetSite(SITE_RENDERBUF, "CFont::RenderFontBuffer", 0x719840, A(&CFont::RenderFontBuffer), reinterpret_cast<void*>(&SRenderBuf));
    SetSite(SITE_KEYS, "CMessages::InsertPlayerControlKeysInString", 0x69E160, A(&CMessages::InsertPlayerControlKeysInString), reinterpret_cast<void*>(&SKeys));
    SetSite(SITE_SETDIR, "CFileMgr::SetDir", 0x5387D0, A(&CFileMgr::SetDir), reinterpret_cast<void*>(&SSetDir));
    SetSite(SITE_OPEN, "CFileMgr::OpenFile", 0x538900, A(&CFileMgr::OpenFile), reinterpret_cast<void*>(&SOpen));
    SetSite(SITE_CLOSE, "CFileMgr::CloseFile", 0x5389D0, A(&CFileMgr::CloseFile), reinterpret_cast<void*>(&SClose));
    SetSite(SITE_LOADLINE, "CFileLoader::LoadLine", 0x536F80, A(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine<FILESTREAM>)), reinterpret_cast<void*>(&SLoadLine));
    SetSite(SITE_ADDSLOT, "CTxdStore::AddTxdSlot", 0x731C80, A(&CTxdStore::AddTxdSlot), reinterpret_cast<void*>(&SAddSlot));
    SetSite(SITE_LOADTXD, "CTxdStore::LoadTxd", 0x7320B0, A(static_cast<bool(*)(int32, const char*)>(&CTxdStore::LoadTxd)), reinterpret_cast<void*>(&SLoadTxd));
    SetSite(SITE_ADDREF, "CTxdStore::AddRef", 0x731A00, A(&CTxdStore::AddRef), reinterpret_cast<void*>(&SAddRef));
    SetSite(SITE_PUSHTXD, "CTxdStore::PushCurrentTxd", 0x7316A0, A(&CTxdStore::PushCurrentTxd), reinterpret_cast<void*>(&SPushTxd));
    SetSite(SITE_SETTXD, "CTxdStore::SetCurrentTxd", 0x7319C0, A(&CTxdStore::SetCurrentTxd), reinterpret_cast<void*>(&SSetTxd));
    SetSite(SITE_POPTXD, "CTxdStore::PopCurrentTxd", 0x7316B0, A(&CTxdStore::PopCurrentTxd), reinterpret_cast<void*>(&SPopTxd));
    SetSite(SITE_FINDSLOT, "CTxdStore::FindTxdSlot", 0x731850, A(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)), reinterpret_cast<void*>(&SFindSlot));
    SetSite(SITE_REMSLOT, "CTxdStore::RemoveTxdSlot", 0x731CD0, A(&CTxdStore::RemoveTxdSlot), reinterpret_cast<void*>(&SRemoveSlot));
    SetSite(SITE_SETTEX, "CSprite2d::SetTexture", 0x7272B0, A(static_cast<void (CSprite2d::*)(Const char*, Const char*)>(&CSprite2d::SetTexture)), reinterpret_cast<void*>(&SSetTexture));
    SetSite(SITE_SPRDEL, "CSprite2d::Delete", 0x727240, A(&CSprite2d::Delete), reinterpret_cast<void*>(&SSpriteDelete));
    SetSite(SITE_LOADFV, "CFont::LoadFontValues", 0x7187C0, A(&CFont::LoadFontValues), reinterpret_cast<void*>(&SLoadFontValues));
    SetSite(SITE_PRINT, "CFont::PrintString", 0x71A700, A(&CFont::PrintString), reinterpret_cast<void*>(&SPrint));
    SetSite(SITE_DRAWFONTS, "CFont::DrawFonts", 0x71A210, A(&CFont::DrawFonts), reinterpret_cast<void*>(&SDraw));
    SetSite(SITE_STAT, "CStats::GetStatValue", 0x558E40, A(static_cast<float(*)(eStats)>(&CStats::GetStatValue)), reinterpret_cast<void*>(&SStat));
    SetSite(SITE_USERMODE, "CAEUserRadioTrackManager::GetUserTrackPlayMode", 0x4F3330, A(&CAEUserRadioTrackManager::GetUserTrackPlayMode), reinterpret_cast<void*>(&SUserMode));
    SetSite(SITE_HWBASS, "CAEAudioHardware::SetBassSetting", 0x4D94A0, A(&CAEAudioHardware::SetBassSetting), reinterpret_cast<void*>(&SHwBass));
    SetSite(SITE_HWFLAGS, "CAEAudioHardware::SetChannelFlags", 0x4D9500, A(&CAEAudioHardware::SetChannelFlags), reinterpret_cast<void*>(&SHwFlags));
    SetSite(SITE_HWPLAY, "CAEAudioHardware::StartTrackPlayback", 0x4D8F30, A(&CAEAudioHardware::StartTrackPlayback), reinterpret_cast<void*>(&SHwPlayback));
    SetSite(SITE_HWFREQ, "CAEAudioHardware::SetChannelFrequencyScalingFactor", 0x4D8960, A(&CAEAudioHardware::SetChannelFrequencyScalingFactor), reinterpret_cast<void*>(&SHwFreq));
    SetSite(SITE_VOLUMES, "CAERadioTrackManager::UpdateRadioVolumes", 0x4EA010, A(&CAERadioTrackManager::UpdateRadioVolumes), reinterpret_cast<void*>(&SVolumes));
    SetSite(SITE_VEHSET, "CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio", 0x4F4ED0, A(&CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio), reinterpret_cast<void*>(&SVehSettings));
    SetSite(SITE_AMBRADIO, "CAudioEngine::IsAmbienceRadioActive", 0x507280, A(&CAudioEngine::IsAmbienceRadioActive), reinterpret_cast<void*>(&SAmbRadio));
    SetSite(SITE_AMBTRACK, "CAudioEngine::IsAmbienceTrackActive", 0x507210, A(&CAudioEngine::IsAmbienceTrackActive), reinterpret_cast<void*>(&SAmbTrack));
    SetSite(SITE_AMBOVR, "CAudioEngine::DoesAmbienceTrackOverrideRadio", 0x507270, A(&CAudioEngine::DoesAmbienceTrackOverrideRadio), reinterpret_cast<void*>(&SAmbOverride));
    SetSite(SITE_STOPAMB, "CAudioEngine::StopAmbienceTrack", 0x507220, A(&CAudioEngine::StopAmbienceTrack), reinterpret_cast<void*>(&SStopAmb));
    SetSite(SITE_REPORT, "CAudioEngine::ReportFrontendAudioEvent", 0x506EA0, A(&CAudioEngine::ReportFrontendAudioEvent), reinterpret_cast<void*>(&SReport));
    SetSite(SITE_ENGSTOP, "CAudioEngine::StopRadio", 0x506F70, A(&CAudioEngine::StopRadio), reinterpret_cast<void*>(&SEngStop));
    SetSite(SITE_MGRSTOP, "CAERadioTrackManager::StopRadio", 0x4E9820, A(&CAERadioTrackManager::StopRadio), reinterpret_cast<void*>(&SMgrStop));
    SetSite(SITE_START, "CAERadioTrackManager::StartRadio", 0x4EB3C0, A(static_cast<void (CAERadioTrackManager::*)(eRadioID, eBassSetting, float, bool)>(&CAERadioTrackManager::StartRadio)), reinterpret_cast<void*>(&SStart));
    SetSite(SITE_VEHACTIVE, "CAERadioTrackManager::IsVehicleRadioActive", 0x4E9800, A(&CAERadioTrackManager::IsVehicleRadioActive), reinterpret_cast<void*>(&SVehActive));
    SetSite(SITE_TRACKRADIO, "CAERadioTrackManager::TrackRadioStation", 0x4EAC30, A(&Access::TrackRadioStation), reinterpret_cast<void*>(&STrackRadio));
    SetSite(SITE_CHOOSE, "CAERadioTrackManager::ChooseTracksForStation", 0x4EB180, A(&Access::ChooseTracksForStation), reinterpret_cast<void*>(&SChoose));
    SetSite(SITE_QUEUE, "CAERadioTrackManager::QueueUpTracksForStation", 0x4EA670, A(&Access::QueueUpTracksForStation), reinterpret_cast<void*>(&SQueue));
    SetSite(SITE_PROB, "CAEAudioUtility::ResolveProbability", 0x4D9C80, A(&CAEAudioUtility::ResolveProbability), reinterpret_cast<void*>(&SProb));
    SetSite(SITE_TEXTGET, "CText::Get", 0x6A0050, A(&CText::Get), reinterpret_cast<void*>(&STextGet));
    SetSite(SITE_FINDVEH, "FindPlayerVehicle", 0x56E0D0, A(static_cast<CVehicle*(*)(int32, bool)>(&FindPlayerVehicle)), reinterpret_cast<void*>(&SFindVehicle));
    SetSite(SITE_SAVEWB, "CGenericGameStorage::SaveDataToWorkBuffer", 0x5D1270, A(static_cast<bool(*)(void*, int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)), reinterpret_cast<void*>(&SSaveWb));
    SetSite(SITE_LOADWB, "CGenericGameStorage::LoadDataFromWorkBuffer", 0x5D1300, A(static_cast<bool(*)(void*, int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)), reinterpret_cast<void*>(&SLoadWb));
    SetSite(SITE_RAND, "rand", 0x821B1E, 0, reinterpret_cast<void*>(&FakeRand));
    SetSite(SITE_RANDNUM, "CGeneral::GetRandomNumber", 0, A(&CGeneral::GetRandomNumber), reinterpret_cast<void*>(&FakeRandomNumber));
}
inline void PatchSites(std::vector<std::unique_ptr<EntryPatch>>& out, const std::vector<SiteId>& ids) {
    for (const SiteId id : ids) {
        const Site& s = sites[size_t(id)];
        if (s.exe) out.push_back(std::make_unique<EntryPatch>(s.exe, reinterpret_cast<void*>(s.stub)));
        if (s.dll) out.push_back(std::make_unique<EntryPatch>(s.dll, reinterpret_cast<void*>(s.stub)));
        if (s.dll && s.body != s.dll) out.push_back(std::make_unique<EntryPatch>(s.body, reinterpret_cast<void*>(s.stub)));
    }
}

// =========================== llamadas ===========================
struct Fp24 { unsigned saved; Fp24() : saved(_controlfp(0, 0)) { _controlfp(_PC_24, _MCW_PC); } ~Fp24() { _controlfp(saved, _MCW_PC | _MCW_RC); } };
template<class R, class... Args> inline R CallThis(uintptr_t addr, Args... a) { Fp24 fp; return reinterpret_cast<R(__thiscall*)(void*, Args...)>(addr)(M(), a...); }
template<class R, class... Args> inline R CallStd(uintptr_t addr, Args... a) { Fp24 fp; return reinterpret_cast<R(__cdecl*)(Args...)>(addr)(a...); }

// =========================== fixtures de radio ===========================
inline void FillSettings(tRadioSettings& s, Rng& g) {
    std::memset(&s, 0xCD, sizeof(s));
    for (auto& v : s.TrackQueue) v = g.p(30) ? -1 : g.r(0, 400);
    s.CurrTrackID = g.r(-1, 300); s.PrevTrackID = g.r(-1, 300); s.PlayTime = g.pick<int32>({-4, -1, 0, 5, 7000, 300000, g.r(-5, 300000)}); s.TrackLengthMs = g.r(0, 300000);
    s.TrackFlags = int8(g.r(0, 3)); s.StationID = static_cast<eRadioID>(g.r(-1, 13));
    s.BassSetting = static_cast<eBassSetting>(g.r(0, 2)); s.BassGain = g.fl();
    for (auto& v : s.TrackTypes) v = int8(g.r(0, 7));
    s.CurrTrackType = int8(g.r(0, 7)); s.PrevTrackType = int8(g.r(0, 7));
    for (auto& v : s.TrackIndices) v = int8(g.r(-1, 60));
    s.CurrTrackIdx = int8(g.r(-1, 60)); s.PrevTrackIdx = int8(g.r(-1, 60));
}
inline void FillRadio(Rng& g) {
    auto* m = M();
    objBuf.fill(0xCD);
    m->m_bInitialised = g.p(50); m->m_bDisplayStationName = g.p(50); m->m_prev = char(g.r(0, 3)); m->m_bEnabledInPauseMode = g.p(50);
    m->m_bBassEnhance = g.p(50); m->m_bPauseMode = g.p(50); m->m_bRetuneJustStarted = g.p(50); m->m_bRadioAutoSelect = g.p(50);
    for (auto& v : m->m_nTracksInARow) v = uint8(g.r(0, 200));
    m->m_nSavedGameClockDays = uint8(g.pick<int>({0, 1, 5, 30, 0x7F, 0x80, 0xFF, 31}));
    m->m_nSavedGameClockHours = uint8(g.pick<int>({0, 1, 5, 23, 0x7F, 0x80, 0xFF}));
    for (auto& v : m->m_aListenTimes) v = g.r(0, 100000);
    m->m_nTimeRadioStationRetuned = g.u(); m->m_nTimeToDisplayRadioName = g.u(); m->m_nSavedTimeMs = g.u(); m->m_nRetuneStartedTime = g.u(); m->field_60 = g.u();
    m->m_HwClientHandle = g.pick<int32>({0, 1, 5, 0x7FFF, 0x8000, 0xFFFF, 0x10000, -1, -32768, 123456});
    m->m_nMode = static_cast<eRadioTrackMode>(g.r(0, 7));
    m->m_nStationsListed = g.pick<int32>({0, 0, 0, 1, -1, 2, -3, 13, -14, 100});
    m->m_nStationsListDown = g.pick<int32>({0, 0, 1, -1, 5});
    m->m_nSavedRadioStationId = g.pick<int32>({-1, 0, 1, 3, 5, 12, 13, 14, 200});
    m->m_iRadioStationMenuRequest = g.pick<int32>({-5, -1, 0, 3, 12, 13, 14, 300, 7});
    m->m_iRadioStationScriptRequest = g.pick<int32>({-1, -1, 0, 5, 13});
    m->m_f80 = g.fl(); m->m_f84 = g.fl();
    FillSettings(m->m_RequestedSettings, g); FillSettings(m->m_ActiveSettings, g);
    for (auto& s : m->m_aRadioState) {
        for (auto& e : s.m_aElapsed) e = g.pick<int32>({0, 100, 7000, 50000, -1});
        s.m_iTimeInPauseModeInMs = g.pick<int32>({-1, 0, 500, 100000});
        s.m_iTimeInMs = g.pick<int32>({-1, -5, 0, 100, 50000, 0x7FFFFFFF});
        s.m_iTrackPlayTime = g.pick<int32>({-1, 0, 1234, 100000});
        for (auto& q : s.m_aTrackQueue) q = g.r(-1, 500);
        for (auto& t : s.m_aTrackTypes) t = int8(g.r(0, 7));
        s.m_nGameClockDays = int8(g.pick<int>({-1, 0, 3, 31, 127, -128}));
        s.m_nGameClockHours = int8(g.pick<int>({-1, 0, 3, 23, 127, -128}));
    }
    m->field_368 = g.u(); m->m_nUserTrackPlayMode = uint8(g.r(0, 2));
}
inline void RandomRegion(uintptr_t addr, size_t n, Rng& g) { auto* p = reinterpret_cast<uint8*>(addr); for (size_t k = 0; k < n; ++k) p[k] = uint8(g.u()); }
inline void FillStatics(Rng& g) { RandomRegion(0xB61D78, 0xEFC, g); }
inline void FillTimer(Rng& g) {
    StaticRef<uint8>(0xB7CB48) = uint8(g.p(25)); StaticRef<uint8>(0xB7CB49) = uint8(g.p(25));
    StaticRef<uint32>(0xB7CB84) = g.pick<uint32>({0u, 1000u, 100000u, 0xFFFFFF00u, g.u()});
    StaticRef<uint32>(0xB7CB7C) = g.pick<uint32>({0u, 700u, 2000u, 123456u, g.u()});
}
inline void FillClock(Rng& g) {
    StaticRef<uint8>(0xB70153) = uint8(g.r(0, 23)); StaticRef<uint8>(0xB70154) = uint8(g.r(0, 31)); StaticRef<uint8>(0xB70155) = uint8(g.r(0, 11));
}

// =========================== fixtures de CFont ===========================
inline void FillFont(Rng& g) {
    // bloque de estado 0xC71A54..0xC71AA0 con campos coherentes
    RandomRegion(0xC71A54, 0x4C, g);
    CFont::PS2Symbol = static_cast<eExtraFontSymbol>(g.p(70) ? 0 : g.r(1, 14));
    CFont::m_bNewLine = g.p(15);
    CFont::m_Color = CRGBA(uint8(g.u()), uint8(g.u()), uint8(g.u()), uint8(g.u()));
    CFont::m_Scale = CVector2D(g.fin(), g.fin());
    CFont::m_fSlant = g.p(60) ? 0.f : g.fin() / 4.f;
    CFont::m_fSlantRefPoint = CVector2D(g.fin() * 10.f, g.fin() * 10.f);
    CFont::m_bFontJustify = g.p(30); CFont::m_bFontCentreAlign = g.p(30); CFont::m_bFontRightAlign = g.p(30);
    CFont::m_bFontBackground = g.p(40); CFont::m_bEnlargeBackgroundBox = g.p(50); CFont::m_bFontPropOn = g.p(70); CFont::m_bFontIsBlip = g.p(10);
    CFont::m_fFontAlpha = g.pick<float>({255.f, 255.f, 254.5f, 128.f, 0.f, 1.f, 300.f});
    CFont::m_FontBackgroundColor = CRGBA(uint8(g.u()), uint8(g.u()), uint8(g.u()), uint8(g.u()));
    CFont::m_fWrapx = g.pick<float>({300.f, 640.f, 100.f, 55.5f, 20.f}); CFont::m_fFontCentreSize = g.pick<float>({300.f, 200.f, 100.f, 55.5f});
    CFont::m_fRightJustifyWrap = g.pick<float>({0.f, 20.f, 100.f, 55.5f});
    CFont::m_FontTextureId = uint8(g.r(0, 1)); CFont::m_FontStyle = uint8(g.r(0, 3)); CFont::m_nFontShadow = uint8(g.r(0, 3));
    CFont::m_FontDropColor = CRGBA(uint8(g.u()), uint8(g.u()), uint8(g.u()), uint8(g.u()));
    CFont::m_nFontOutlineSize = uint8(g.pick<int>({0, 1, 2, 255, 254, 128, 127}));
    CFont::m_nFontOutline = uint8(g.r(0, 2));
    // RenderState (CFontChar)
    CFontChar& rs = CFont::RenderState;
    RandomRegion(0xC71AA0, sizeof(CFontChar), g);
    rs.m_bContainImages = g.p(30); rs.m_nFontStyle = uint8(g.r(0, 3)); rs.m_wFontTexture = uint16(g.pick<int>({0, 1, 0xFFFF, 0}));
    *reinterpret_cast<uint8*>(&rs.m_bPropOn) = uint8(g.pick<int>({0, 1, 1, 2}));
    rs.m_fWidth = g.fin(); rs.m_fHeight = g.fin(); rs.m_fSlant = g.p(50) ? 0.f : g.fin();
    // tabla de fuentes
    RandomRegion(0xC718B0, 0x1A4, g);
}
// Texto GXT bien formado (cada '~' abre una etiqueta que se cierra); sin bytes 0x00.
inline std::string MakeText(Rng& g, bool tags, int maxWords) {
    static const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;!?-+*/'()%&=<>@[]_";
    std::string s; const int words = g.r(0, maxWords);
    for (int w = 0; w < words; ++w) {
        if (tags && g.p(25)) {
            s.push_back('~');
            int n = g.r(0, 3);
            if (n == 0) s.push_back('~');
            else {
                static const char* first = "<>ABCDGHJKMNOPQRSTUVWXYabcdghjkmnopqrstuvwxylzEFIL1=?@ ";
                const char c0 = first[g.u() % std::strlen(first)];
                /* El EXE (0x71A338 y 0x71A33D) retrocede el cursor 3 bytes tras una etiqueta de simbolo PS2 (<>ACDJKMOQTUVXacdjkmoqtuvx,
                   tabla 0x7191F4 -> manejadores que escriben 0xC71A54) o de salto de linea (Nn, escribe 0xC71A55): solo es coherente con etiquetas de UN caracter (~X~).
                   Con etiquetas largas el cursor cae a mitad de etiqueta y el bucle de ProcessCurrentString puede no terminar nunca (cuelgue del banco). */
                if (std::strchr("<>ACDJKMOQTUVXacdjkmoqtuvxNn", c0)) n = 1;
                s.push_back(c0); for (int k = 1; k < n; ++k) s.push_back(letters[g.u() % std::strlen(letters)]); s.push_back('~');
            }
        }
        const int len = g.r(1, 8);
        for (int k = 0; k < len; ++k) { if (g.p(4)) s.push_back(char(g.r(0xA0, 0xFF))); else s.push_back(letters[g.u() % std::strlen(letters)]); }
        if (tags && g.p(10)) { s += "~n~"; }
        s.append(size_t(g.r(1, 2)), ' ');
    }
    if (tags && g.p(20)) { s += "~r~"; }
    if (g.p(30) && !s.empty() && s.back() == ' ') s.pop_back();
    return s;
}

// =========================== tabla de funciones ===========================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::vector<Fn> fns{
    {"CAERadioTrackManager","AddAdvertIndexToHistory",0x4E9760,0,-1,512},
    {"CAERadioTrackManager","AddDJBanterIndexToHistory",0x4E97B0,0,-1,512},
    {"CAERadioTrackManager","AddIdentIndexToHistory",0x4E9720,0,-1,512},
    {"CAERadioTrackManager","AddMusicTrackIndexToHistory",0x4E96C0,0,-1,512},
    {"CAERadioTrackManager","CheckForMissionStatsChanges",0x4E8410,0,-1,1024},
    {"CAERadioTrackManager","CheckForPause",0x4EA590,0,-1,512},
    {"CAERadioTrackManager","CheckForStationRetuneDuringPause",0x4EB890,0,-1,512},
    {"CAERadioTrackManager","ChooseTracksForStation",0x4EB180,0,-1,512},
    {"CAERadioTrackManager","DisplayRadioStationName",0x4E9E50,0,-1,512},
    {"CAERadioTrackManager","GetCurrentRadioStationID",0x4E83F0,0,-1,128},
    {"CAERadioTrackManager","GetRadioStationListenTimes",0x4E83E0,0,-1,64},
    {"CAERadioTrackManager","GetRadioStationName",0x4E9E10,0,-1,128},
    {"CAERadioTrackManager","GetRadioStationNameKey",0x4E8380,0,-1,128},
    {"CAERadioTrackManager","HasRadioRetuneJustStarted",0x4E8370,0,-1,64},
    {"CAERadioTrackManager","Initialise",0x5B9390,0,-1,256},
    {"CAERadioTrackManager","InitialiseRadioStationID",0x4E8330,0,-1,128},
    {"CAERadioTrackManager","IsRadioOn",0x4E8350,0,-1,256},
    {"CAERadioTrackManager","Load",0x5D40E0,0,-1,128},
    {"CAERadioTrackManager","PlayRadioAnnouncement",0x4E8400,0,-1,64},
    {"CAERadioTrackManager","Reset",0x4E7F80,0,-1,256},
    {"CAERadioTrackManager","ResetStatistics",0x4E8200,0,-1,128},
    {"CAERadioTrackManager","RetuneRadio",0x4E8290,0,-1,256},
    {"CAERadioTrackManager","Save",0x5D3EE0,0,-1,128},
    {"CAERadioTrackManager","SetBassEnhanceOnOff",0x4E9DB0,0,-1,256},
    {"CAERadioTrackManager","SetBassSetting",0x4E82F0,0,-1,256},
    {"CAERadioTrackManager","SetRadioAutoRetuneOnOff",0x4E82E0,0,-1,64},
    {"CAERadioTrackManager","StartRadio-manual",0x4EB3C0,0,-1,1024},
    {"CAERadioTrackManager","StartRadio-with-settings",0x4EB550,0,-1,512},
    {"CAERadioTrackManager","StartTrackPlayback",0x4EA640,0,-1,128},
    {"CFont","DrawFonts",0x71A210,0,-1,64},
    {"CFont","GetCharacterSize",0x719750,0,-1,512},
    {"CFont","GetLetterIdPropValue",0x718770,0,-1,512},
    {"CFont","GetNumberLines",0x71A5E0,0,-1,256},
    {"CFont","GetStringWidth",0x71A0E0,0,-1,512},
    {"CFont","GetTextRect",0x71A620,0,-1,256},
    {"CFont","InitPerFrame",0x719800,0,-1,64},
    {"CFont","Initialise",0x5BA690,0,-1,64},
    {"CFont","LoadFontValues",0x7187C0,0,-1,128},
    {"CFont","ParseToken",0x718F00,0,-1,512},
    {"CFont","PrintString",0x71A700,0,-1,256},
    {"CFont","PrintStringFromBottom",0x71A820,0,-1,256},
    {"CFont","ProcessStringToDisplay",0x71A600,0,-1,256},
    {"CFont","SetAlphaFade",0x719500,0,-1,64},
    {"CFont","SetBackground",0x7195C0,0,-1,64},
    {"CFont","SetBackgroundColor",0x7195E0,0,-1,64},
    {"CFont","SetCentreSize",0x7194E0,0,-1,64},
    {"CFont","SetColor",0x719430,0,-1,256},
    {"CFont","SetDropColor",0x719510,0,-1,256},
    {"CFont","SetDropShadowPosition",0x719570,0,-1,64},
    {"CFont","SetEdge",0x719590,0,-1,64},
    {"CFont","SetFontStyle",0x719490,0,-1,128},
    {"CFont","SetJustify",0x719600,0,-1,64},
    {"CFont","SetOrientation",0x719610,0,-1,128},
    {"CFont","SetProportional",0x7195B0,0,-1,64},
    {"CFont","SetRightJustifyWrap",0x7194F0,0,-1,64},
    {"CFont","SetScale",0x719380,0,-1,128},
    {"CFont","SetScaleForCurrentLanguage",0x7193A0,0,-1,128},
    {"CFont","SetSlant",0x719420,0,-1,64},
    {"CFont","SetSlantRefPoint",0x719400,0,-1,64},
    {"CFont","SetWrapx",0x7194D0,0,-1,64},
    {"CFont","Shutdown",0x7189B0,0,-1,64},
};

// Sitios que cada funcion necesita parchear (todos los demas colaboradores se ejecutan de verdad).
inline std::vector<SiteId> Needs(const Fn& f) {
    const std::string n = f.name; const bool radio = std::string(f.cls) == "CAERadioTrackManager";
    if (radio) {
        if (n == "CheckForMissionStatsChanges") return {SITE_STAT};
        if (n == "Reset") return {SITE_RAND, SITE_RANDNUM};
        if (n == "Initialise") return {SITE_RAND, SITE_RANDNUM, SITE_USERMODE};
        if (n == "GetRadioStationName") return {SITE_TEXTGET};
        if (n == "SetBassSetting" || n == "SetBassEnhanceOnOff") return {SITE_HWBASS};
        if (n == "StartTrackPlayback") return {SITE_HWFLAGS, SITE_HWPLAY, SITE_VOLUMES};
        if (n == "CheckForPause") return {SITE_VEHSET, SITE_AMBRADIO, SITE_MGRSTOP, SITE_REPORT, SITE_HWFREQ};
        if (n == "CheckForStationRetuneDuringPause") return {SITE_REPORT, SITE_ENGSTOP, SITE_START};
        if (n == "ChooseTracksForStation") return {SITE_PROB, SITE_QUEUE};
        if (n == "DisplayRadioStationName") return {SITE_FINDVEH, SITE_VEHACTIVE, SITE_TEXTGET, SITE_PRINT, SITE_DRAWFONTS};
        if (n == "StartRadio-manual") return {SITE_AMBTRACK, SITE_AMBOVR, SITE_REPORT, SITE_STOPAMB, SITE_TRACKRADIO, SITE_CHOOSE, SITE_RAND, SITE_RANDNUM};
        if (n == "StartRadio-with-settings") return {SITE_START, SITE_REPORT};
        if (n == "Save") return {SITE_SAVEWB};
        if (n == "Load") return {SITE_LOADWB};
        return {};
    }
    if (n == "DrawFonts") return {SITE_RENDERBUF};
    if (n == "InitPerFrame") return {SITE_INITSPR};
    if (n == "GetStringWidth") return {SITE_KEYS};
    if (n == "GetNumberLines" || n == "ProcessStringToDisplay" || n == "GetTextRect") return {SITE_EMIT, SITE_KEYS};
    if (n == "PrintString" || n == "PrintStringFromBottom") return {SITE_EMIT, SITE_KEYS, SITE_DRAWRECT, SITE_DRAWWIN};
    if (n == "Initialise") return {SITE_ADDSLOT, SITE_LOADTXD, SITE_ADDREF, SITE_PUSHTXD, SITE_SETTXD, SITE_POPTXD, SITE_SETTEX, SITE_LOADFV};
    if (n == "LoadFontValues") return {SITE_SETDIR, SITE_OPEN, SITE_CLOSE, SITE_LOADLINE};
    if (n == "Shutdown") return {SITE_SPRDEL, SITE_FINDSLOT, SITE_REMSLOT};
    return {};
}

// =========================== ejecucion de cada funcion de radio ===========================
inline json ExecRadio(const std::string& n, int /*i*/, Rng& g) {
    json out = json::object();
    const auto id14 = [&]() { return g.r(0, 13); };
    if (n == "AddMusicTrackIndexToHistory") {
        FillRadio(g); FillStatics(g);
        const int id = id14(); auto& h = CAERadioTrackManager::m_nMusicTrackIndexHistory[id];
        const int idx = g.p(30) ? int(h.indices[0]) : g.pick<int>({-3, -1, 0, 1, 5, 60, 126, 127, 128, 200, 255, g.r(-5, 125)});
        CallThis<void>(0x4E96C0, int32(id), int32(idx));
    } else if (n == "AddIdentIndexToHistory") {
        FillRadio(g); FillStatics(g);
        const int id = id14(); auto& h = CAERadioTrackManager::m_nIdentIndexHistory[id];
        const int32 idx = g.p(30) ? h.indices[0] : g.pick<int32>({-1, 0, 1, 5, 127, 128, 200, 255, 256, 1000, -100000, 0x7FFFFFFF, g.r(-5, 300)});
        CallThis<void>(0x4E9720, int32(id), idx);
    } else if (n == "AddAdvertIndexToHistory") {
        FillRadio(g); FillStatics(g);
        const int id = id14(); auto& h = CAERadioTrackManager::m_nAdvertIndexHistory[id];
        const int32 idx = g.p(30) ? h.indices[0] : g.pick<int32>({-1, 0, 1, 5, 127, 128, 200, 255, 256, 1000, -100000, 0x7FFFFFFF, g.r(-5, 300)});
        CallThis<void>(0x4E9760, int32(id), idx);
    } else if (n == "AddDJBanterIndexToHistory") {
        FillRadio(g); FillStatics(g);
        const int id = id14(); auto& h = CAERadioTrackManager::m_nDJBanterIndexHistory[id];
        const int32 idx = g.p(30) ? h.indices[0] : g.pick<int32>({-1, 0, 1, 5, 127, 128, 200, 255, 256, 1000, -100000, 0x7FFFFFFF, g.r(-5, 300)});
        CallThis<void>(0x4E97B0, int32(id), idx);
    } else if (n == "CheckForMissionStatsChanges") {
        FillStatics(g); FillClock(g);
        for (uintptr_t a = 0xB62C58; a <= 0xB62C73; ++a) *reinterpret_cast<uint8*>(a) = uint8(g.pick<int>({0, 1, 2, 0xFF, 0x7F, 0x80, 3, 0}));
        *reinterpret_cast<uint8*>(0xB62C73) = uint8(g.pick<int>({1, 2, 3, 3, 3}));
        *reinterpret_cast<uint8*>(0xB62C58) = uint8(g.pick<int>({0, 1, 24, 168, 255, 100, 50, 200}));
        *reinterpret_cast<uint8*>(0xB62C5A) = uint8(g.pick<int>({0, 5, 10, 20, 0xFF, 0x80, 31, 0x7F}));
        *reinterpret_cast<uint8*>(0xB62C59) = uint8(g.pick<int>({0, 5, 12, 23, 0xFF, 0x80, 0x7F}));
        statTable.fill(0.f);
        static const int stats[] = {181, 327, 328, 329, 318, 320, 330, 331, 332, 333, 311, 323, 308, 310, 335, 336, 337, 302, 338, 339, 340, 341, 342};
        for (const int s : stats) statTable[size_t(s)] = g.pick<float>({0.f, 1.f, 2.f, 3.f, 0.5f, 1.5f, 2.5f, 255.f, 256.f, -1.f, -0.5f, 1e10f, std::bit_cast<float>(0x7FC00000u), 127.f, 128.f, 129.f, 1.99f});
        CallStd<void>(0x4E8410);
    } else if (n == "CheckForPause") {
        FillRadio(g); FillTimer(g);
        optMode = g.r(0, 1); Opt()->RadioType = static_cast<eAERadioType>(g.pick<int>({-1, 0, 1, 2, 3, 4})); ambRadio = g.r(0, 1);
        CallThis<void>(0x4EA590);
    } else if (n == "CheckForStationRetuneDuringPause") {
        FillRadio(g); FillTimer(g);
        if (g.p(25)) M()->m_ActiveSettings.StationID = RADIO_EMERGENCY_AA; else if (g.p(30)) M()->m_ActiveSettings.StationID = RADIO_OFF;
        if (g.p(40)) M()->m_nRetuneStartedTime = StaticRef<uint32>(0xB7CB7C) - uint32(g.pick<int>({0, 699, 700, 701, 2000}));
        CallThis<void>(0x4EB890);
    } else if (n == "ChooseTracksForStation") {
        FillRadio(g); FillStatics(g);
        probBits = g.u(); probIdx = 0; queueBits = g.u() | (g.u() & g.u()); queueIdx = 0;
        *reinterpret_cast<uint8*>(0xBA67F8) = uint8(g.r(0, 2));
        CallThis<void>(0x4EB180, g.pick<int32>({0, 0, 1, 5, 12, 12, 13, 7, 3}));
    } else if (n == "DisplayRadioStationName") {
        FillRadio(g); FillTimer(g); FillFont(g); FillStatics(g);
        vehPresent = g.p(80); vehActive = g.p(60);
        StaticRef<uint8>(0xB6F065) = uint8(g.p(15));
        *reinterpret_cast<uint32*>(0xA43088) = g.p(15) ? 1u : 0u;
        StaticRef<int32>(0xC17044) = g.pick<int32>({640, 800, 1024, 1025, 1279, 1920, 1921, 3, 2});
        StaticRef<int32>(0xC17048) = g.pick<int32>({480, 600, 768, 769, 1080, 1081});
        const uint32 now = StaticRef<uint32>(0xB7CB84);
        M()->m_nTimeToDisplayRadioName = now + uint32(g.pick<int>({-10, 0, 1, 5, 3000, 2500}));
        M()->m_RequestedSettings.StationID = static_cast<eRadioID>(g.pick<int>({-1, 0, 1, 5, 11, 12, 13}));
        M()->m_nStationsListed = g.pick<int32>({0, 0, 1, -1, 2, -3, 13, -14, 14, 100, -13});
        CallThis<void>(0x4E9E50);
    } else if (n == "GetCurrentRadioStationID") {
        FillRadio(g);
        out["ret"] = int(CallThis<uint8>(0x4E83F0));
    } else if (n == "GetRadioStationListenTimes") {
        FillRadio(g);
        const uintptr_t r = CallThis<uintptr_t>(0x4E83E0);
        out["ret"] = int64(r) - int64(reinterpret_cast<uintptr_t>(M()));
    } else if (n == "GetRadioStationName") {
        FillRadio(g);
        const int32 id = g.pick<int32>({-3, -1, 0, 1, 2, 5, 11, 12, 13, 4, 7});
        const GxtChar* r = CallThis<const GxtChar*>(0x4E9E10, id);
        out["ret"] = r ? EscZ(reinterpret_cast<const char*>(r), 64) : std::string("<null>");
    } else if (n == "GetRadioStationNameKey") {
        FillRadio(g);
        char key[16]; std::memset(key, 0x5A, sizeof(key));
        CallThis<void>(0x4E8380, g.pick<int32>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}), static_cast<char*>(key));
        out["key"] = Hex(key, sizeof(key));
    } else if (n == "HasRadioRetuneJustStarted") {
        FillRadio(g);
        out["ret"] = int(CallThis<uint8>(0x4E8370));
    } else if (n == "Initialise") {
        FillRadio(g); FillStatics(g);
        rngSeed = g.u(); rngCalls = 0; userMode = g.r(0, 3);
        out["ret"] = int(CallThis<uint8>(0x5B9390, g.pick<int32>({0, 1, 5, 0x7FFF, 0xFFFF, 0x10000, -1, 123456})));
    } else if (n == "InitialiseRadioStationID") {
        FillRadio(g);
        CallThis<void>(0x4E8330, g.pick<int32>({-1, 0, 1, 5, 12, 13, 14, g.r(-1, 13)}));
    } else if (n == "IsRadioOn") {
        FillRadio(g);
        if (g.p(30)) M()->m_nMode = eRadioTrackMode::RADIO_STOPPED;
        if (g.p(40)) { M()->m_bInitialised = false; M()->m_nStationsListed = 0; if (g.p(50)) M()->m_nStationsListDown = 0; }
        out["ret"] = int(CallThis<uint8>(0x4E8350));
    } else if (n == "Load") {
        FillStatics(g); loadCounter = uint32(g.r(0, 200));
        CallStd<void>(0x5D40E0);
    } else if (n == "PlayRadioAnnouncement") {
        FillRadio(g);
        CallThis<void>(0x4E8400, g.u());
    } else if (n == "Reset") {
        FillRadio(g); FillStatics(g);
        rngSeed = g.u(); rngCalls = 0;
        CallThis<void>(0x4E7F80);
    } else if (n == "ResetStatistics") {
        FillStatics(g);
        CallStd<void>(0x4E8200);
    } else if (n == "RetuneRadio") {
        FillRadio(g); FillTimer(g);
        *reinterpret_cast<uint16*>(0xB6B976) = uint16(g.pick<int>({0, 1, 5, 12, 0xFFFF}));
        CallThis<void>(0x4E8290, g.pick<int32>({-1, 0, 1, 5, 11, 12, 12, 13, 14, 100}));
    } else if (n == "Save") {
        FillStatics(g);
        CallStd<void>(0x5D3EE0);
    } else if (n == "SetBassEnhanceOnOff") {
        FillRadio(g);
        if (g.p(50)) M()->m_nMode = eRadioTrackMode::RADIO_PLAYING;
        CallThis<void>(0x4E9DB0, bool(g.p(50)));
    } else if (n == "SetBassSetting") {
        FillRadio(g);
        const int32 setting = g.pick<int32>({0, 1, 2, -1, 5, 3});
        const float gain = g.fl();
        CallThis<void>(0x4E82F0, setting, gain);
    } else if (n == "SetRadioAutoRetuneOnOff") {
        FillRadio(g);
        CallThis<void>(0x4E82E0, bool(g.p(50)));
    } else if (n == "StartRadio-manual") {
        FillRadio(g); FillTimer(g);
        ambTrack = g.p(40); ambOverride = g.p(40); trackResult = g.p(50); rngSeed = g.u(); rngCalls = 0;
        const int32 id = g.pick<int32>({0, 1, 2, 5, 11, 12, 13, 14, 50, 127, g.r(0, 13)});
        if (g.p(40)) M()->m_ActiveSettings.StationID = static_cast<eRadioID>(std::min<int32>(id, 13));
        if (g.p(30)) M()->m_aRadioState[size_t(std::min<int32>(id, 13))].m_iTimeInMs = -1;
        const int32 bass = g.pick<int32>({0, 1, 2});
        const float gain = g.fl();
        const bool skip = g.p(50);
        CallThis<void>(0x4EB3C0, id, bass, gain, skip);
    } else if (n == "StartRadio-with-settings") {
        FillRadio(g); FillTimer(g); FillClock(g);
        *reinterpret_cast<uint32*>(0xA43088) = g.p(15) ? 1u : 0u;
        optBuf.fill(0);
        Opt()->BassSetting = static_cast<eBassSetting>(g.pick<int>({0, 1, 2}));
        Opt()->BassFactor = g.fl();
        Opt()->RadioStation = static_cast<eRadioID>(int8(g.pick<int>({-1, 0, 3, 5, 12, 13, 200})));
        Opt()->RadioType = static_cast<eAERadioType>(g.pick<int>({-1, 0, 0, 0, 1, 2, 3, 4}));
        const uint32 now = StaticRef<uint32>(0xB7CB84);
        M()->m_nSavedTimeMs = now - uint32(g.pick<int>({0, 100, 59000, 60000, 61000, -5}));
        const int cd = StaticRef<uint8>(0xB70154);
        M()->m_nSavedGameClockDays = uint8(g.pick<int>({cd, cd, cd - 1, 0xFF, 0x80, 3}));
        M()->m_nSavedGameClockHours = uint8(g.pick<int>({0, 3, 12, 22, 23, 0xFF, 0x80}));
        CallThis<void>(0x4EB550, static_cast<const tVehicleAudioSettings*>(Opt()));
    } else if (n == "StartTrackPlayback") {
        FillRadio(g);
        CallThis<void>(0x4EA640);
    } else {
        out["error"] = "funcion de radio desconocida";
    }
    return out;
}

// =========================== ejecucion de cada funcion de CFont ===========================
inline void MakeFontFile(Rng& g) {
    fileLines.clear(); fileCursor = 0;
    const auto num = [&]() { return g.pick<int>({0, 1, 2, 5, 7, 16, 31, 100, 127, 128, 200, 255, 256, 300, -1, -5, g.r(0, 255)}); };
    if (g.p(60)) fileLines.push_back("# comentario");
    const int sections = g.r(1, 6);
    for (int s = 0; s < sections; ++s) {
        if (g.p(30)) fileLines.push_back("");
        if (g.p(30)) fileLines.push_back("# nota");
        switch (g.r(0, 5)) {
        case 0: fileLines.push_back("[TOTAL_FONTS]"); fileLines.push_back(std::to_string(g.r(1, 3))); break;
        case 1: fileLines.push_back("[FONT_ID]"); fileLines.push_back(std::to_string(g.r(0, 1))); break;
        case 2: fileLines.push_back("[REPLACEMENT_SPACE_CHAR]"); fileLines.push_back(std::to_string(num())); break;
        case 3: {
            fileLines.push_back("[PROP]");
            for (int row = 0; row < 26; ++row) { std::string l; for (int c = 0; c < 8; ++c) { if (c) l += "  "; l += std::to_string(num()); } fileLines.push_back(l); }
            break; }
        case 4: fileLines.push_back("[UNPROP]"); fileLines.push_back(std::to_string(num())); break;
        default: fileLines.push_back("[SOMETHING_ELSE]"); fileLines.push_back("1 2 3"); break;
        }
    }
}
inline json ExecFont(const std::string& n, int i, Rng& g) {
    json out = json::object();
    const auto block = [&]() { RandomRegion(0xC71A54, 0x4C, g); RandomRegion(0xC71AA0, sizeof(CFontChar), g); };
    const auto text = [&](bool tags, int words) { std::string t = MakeText(g, tags, words); if (g.p(6)) t.clear(); return t; };
    if (n == "SetScale") { block(); const float w = g.fl(), h = g.fl(); CallStd<void>(0x719380, w, h); }
    else if (n == "SetScaleForCurrentLanguage") { block(); *reinterpret_cast<uint8*>(0xBA67CC) = uint8(g.pick<int>({0, 1, 2, 3, 4, 5, 6, 0xFF, 0x80, 200})); const float w = g.fl(), h = g.fl(); CallStd<void>(0x7193A0, w, h); }
    else if (n == "SetSlantRefPoint") { block(); const float x = g.fl(), y = g.fl(); CallStd<void>(0x719400, x, y); }
    else if (n == "SetSlant") { block(); CallStd<void>(0x719420, g.fl()); }
    else if (n == "SetColor") { block(); CFont::m_fFontAlpha = g.pick<float>({255.f, 254.f, 128.f, 0.f, 1.f, 300.f, std::bit_cast<float>(0x7FC00000u), -5.f, 127.5f, std::bit_cast<float>(0x7F800000u), 255.5f}); CallStd<void>(0x719430, g.u()); }
    else if (n == "SetFontStyle") { block(); CallStd<void>(0x719490, g.pick<int32>({0, 1, 2, 3, 4, 5, 100, 200, 255, g.r(0, 3)})); }
    else if (n == "SetWrapx") { block(); CallStd<void>(0x7194D0, g.fl()); }
    else if (n == "SetCentreSize") { block(); CallStd<void>(0x7194E0, g.fl()); }
    else if (n == "SetRightJustifyWrap") { block(); CallStd<void>(0x7194F0, g.fl()); }
    else if (n == "SetAlphaFade") { block(); CallStd<void>(0x719500, g.fl()); }
    else if (n == "SetDropColor") { block(); CFont::m_fFontAlpha = g.pick<float>({255.f, 254.f, 128.f, 0.f, 1.f, 300.f, std::bit_cast<float>(0x7FC00000u), -5.f, 127.5f, 0.5f, 255.5f}); CallStd<void>(0x719510, g.u()); }
    else if (n == "SetDropShadowPosition") { block(); CallStd<void>(0x719570, g.pick<int32>({0, 1, 2, 3, -1, 255, 256, 257, 0x7FFF, -32768, g.r(-300, 300)})); }
    else if (n == "SetEdge") { block(); CallStd<void>(0x719590, g.pick<int32>({0, 1, 2, -1, 127, -128, 100, g.r(-128, 127)})); }
    else if (n == "SetProportional") { block(); CallStd<void>(0x7195B0, bool(g.p(50))); }
    else if (n == "SetBackground") { block(); const bool a = g.p(50), b = g.p(50); CallStd<void>(0x7195C0, a, b); }
    else if (n == "SetBackgroundColor") { block(); CallStd<void>(0x7195E0, g.u()); }
    else if (n == "SetJustify") { block(); CallStd<void>(0x719600, bool(g.p(50))); }
    else if (n == "SetOrientation") { block(); CallStd<void>(0x719610, g.pick<int32>({-1, 0, 1, 2, 3, 100, 255, -128})); }
    else if (n == "GetCharacterSize") {
        FillFont(g);
        const uint8 letter = uint8(g.pick<int>({63, 145, 155, 156, 0, 255, 32, g.r(0, 255), g.r(0, 255)}));
        out["ret"] = F(CallStd<float>(0x719750, letter));
    } else if (n == "GetLetterIdPropValue") {
        FillFont(g);
        const uint8 letter = uint8(g.pick<int>({63, 0, 255, 100, g.r(0, 255), g.r(0, 255)}));
        out["ret"] = F(CallStd<float>(0x718770, letter));
    } else if (n == "GetStringWidth") {
        FillFont(g);
        char buf[1024]; std::memset(buf, 0, sizeof(buf));
        std::string t;
        if (g.p(8)) { t = MakeText(g, false, 90); while (t.size() < 420) t += "abc defgh "; t.resize(size_t(g.r(400, 480))); }
        else t = text(true, 10);
        std::memcpy(buf, t.c_str(), t.size() + 1);
        const bool full = g.p(50), script = g.p(15);
        out["ret"] = F(CallStd<float, const GxtChar*, bool, bool>(0x71A0E0, reinterpret_cast<const GxtChar*>(buf), full, script));
    } else if (n == "GetNumberLines" || n == "ProcessStringToDisplay") {
        FillFont(g); RandomRegion(reinterpret_cast<uintptr_t>(&gString[0]), 16, g);
        char buf[1024]; std::memset(buf, 0, sizeof(buf));
        const std::string t = text(true, 12); std::memcpy(buf, t.c_str(), t.size() + 1);
        const float x = g.fin() * 40.f, y = g.fin() * 40.f;
        out["ret"] = int(CallStd<int16, float, float, const GxtChar*>(n == "GetNumberLines" ? 0x71A5E0 : 0x71A600, x, y, reinterpret_cast<const GxtChar*>(buf)));
    } else if (n == "GetTextRect") {
        FillFont(g); RandomRegion(reinterpret_cast<uintptr_t>(&gString[0]), 16, g);
        char buf[1024]; std::memset(buf, 0, sizeof(buf));
        const std::string t = text(true, 12); std::memcpy(buf, t.c_str(), t.size() + 1);
        CRect rect; std::memset(&rect, 0xEE, sizeof(rect));
        const float x = g.fin() * 40.f, y = g.fin() * 40.f;
        CallStd<void, CRect*, float, float, const GxtChar*>(0x71A620, &rect, x, y, reinterpret_cast<const GxtChar*>(buf));
        out["rect"] = Hex(&rect, 16);
    } else if (n == "PrintString" || n == "PrintStringFromBottom") {
        FillFont(g); RandomRegion(reinterpret_cast<uintptr_t>(&gString[0]), 16, g);
        char buf[1024]; std::memset(buf, 0, sizeof(buf));
        std::string t = text(true, 12); if (g.p(8)) t = "*" + t;
        std::memcpy(buf, t.c_str(), t.size() + 1);
        const float x = g.fin() * 40.f, y = g.fin() * 40.f;
        CallStd<void, float, float, const GxtChar*>(n == "PrintString" ? 0x71A700 : 0x71A820, x, y, reinterpret_cast<const GxtChar*>(buf));
    } else if (n == "InitPerFrame") {
        FillFont(g);
        CallStd<void>(0x719800);
    } else if (n == "DrawFonts") {
        FillFont(g);
        CallStd<void>(0x71A210);
    } else if (n == "ParseToken") {
        FillFont(g);
        char buf[96]; std::memset(buf, 0, sizeof(buf));
        std::string t = "~"; const int blen = g.r(0, 4);
        static const char* firstChars = "<>ABCDGHJKMNOPQRSTUVWXYabcdghjkmnopqrstuvwxylzEFIL1=?@ [`{";
        if (blen == 0) t += "~";
        else { t.push_back(firstChars[g.u() % std::strlen(firstChars)]); for (int k = 1; k < blen; ++k) t.push_back(char(g.r(0x20, 0x7D))); t += "~"; }
        const int tail = g.r(0, 6);
        for (int k = 0; k < tail; ++k) t.push_back(char(g.r(0x21, 0x7D)));
        std::memcpy(buf, t.c_str(), t.size() + 1);
        CRGBA color(uint8(g.u()), uint8(g.u()), uint8(g.u()), uint8(g.u()));
        char tag = 0x77; const bool useTag = g.p(60);
        const bool blip = g.p(40);
        char* r = CallStd<char*, char*, CRGBA&, bool, char*>(0x718F00, static_cast<char*>(buf), color, blip, useTag ? &tag : nullptr);
        out["ret"] = int64(r - buf); out["color"] = Hex(&color, 4); out["tag"] = int(uint8(tag));
    } else if (n == "LoadFontValues") {
        RandomRegion(0xC718B0, 0x1A4, g);
        MakeFontFile(g);
        CallStd<void>(0x7187C0);
        out["cursor"] = int(fileCursor);
    } else if (n == "Initialise") {
        FillFont(g); txdCalls = 0;
        StaticRef<int32>(0xC17044) = g.pick<int32>({640, 800, 1024, 1025, 1920, 3});
        CallStd<void>(0x5BA690);
    } else if (n == "Shutdown") {
        FillFont(g);
        CallStd<void>(0x7189B0);
    } else {
        out["error"] = "funcion de CFont desconocida";
    }
    (void)i;
    return out;
}

inline json Exec(const Fn& f, size_t fi, int i) {
    Rng g(uint64(i) * 1000003ull + uint64(fi) * 7919ull + 17);
    optBuf.fill(0);
    return std::string(f.cls) == "CFont" ? ExecFont(f.name, i, g) : ExecRadio(f.name, i, g);
}

// =========================== bucle principal ===========================
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "radiofont1-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "radiofont1-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("radiofont1"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "radiofont1-only.txt"); if (f) std::getline(f, only); }

    // Comprobaciones de diseno en tiempo de ejecucion (direcciones de los globales que usa el EXE)
    const bool layout =
        reinterpret_cast<uintptr_t>(&AEUserRadioTrackManager) == 0xB6B970 && reinterpret_cast<uintptr_t>(&AEUserRadioTrackManager.m_nUserTracksCount) == 0xB6B976 &&
        reinterpret_cast<uintptr_t>(&TheCamera.m_bWideScreenOn) == 0xB6F065 && reinterpret_cast<uintptr_t>(&CTimer::m_UserPause) == 0xB7CB49 &&
        reinterpret_cast<uintptr_t>(&CTimer::m_CodePause) == 0xB7CB48 && reinterpret_cast<uintptr_t>(&AudioEngine) == 0xB6BC90 &&
        reinterpret_cast<uintptr_t>(&AEAudioHardware) == 0xB5F8B8 && reinterpret_cast<uintptr_t>(&FrontEndMenuManager) == 0xBA6748 &&
        reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_nPrefsLanguage) == 0xBA67CC && reinterpret_cast<uintptr_t>(&RsGlobal.maximumWidth) == 0xC17044 &&
        reinterpret_cast<uintptr_t>(&RsGlobal.maximumHeight) == 0xC17048 && reinterpret_cast<uintptr_t>(&CClock::ms_nGameClockMonth) == 0xB70155 &&
        reinterpret_cast<uintptr_t>(&CFont::RenderState) == 0xC71AA0 && reinterpret_cast<uintptr_t>(&gString[0]) == 0xB71670 &&
        reinterpret_cast<uintptr_t>(&CAERadioTrackManager::m_nSpecialDJBanterPending) == 0xB62C73 && reinterpret_cast<uintptr_t>(&CAERadioTrackManager::m_nMusicTrackIndexHistory) == 0xB62B40;

    BuildSites();   // Body() de TODOS los sitios antes de crear ningun EntryPatch
    bool sitesOk = true; json siteInfo = json::array();
    for (const auto& s : sites) {
        const bool ok = (s.exe == 0 || (s.exe >= 0x401000 && s.exe < 0x1000000)) && (s.dll == 0 || (s.dll >= 0x1000000 && s.body >= 0x1000000)) && s.stub >= 0x1000000;
        sitesOk &= ok; if (!ok) siteInfo.push_back({{"name", s.name}, {"exe", s.exe}, {"dll", s.dll}, {"body", s.body}});
    }

    regs = {
        {"radio_stats", 0xB61D78, 0xEFC}, {"timer", 0xB7CB48, 0x40}, {"clock", 0xB70150, 0x10}, {"replay", 0xA43088, 4},
        {"widescreen", 0xB6F065, 1}, {"font", 0xC716A8, 0x500}, {"language", 0xBA67CC, 1}, {"radiomode", 0xBA67F8, 1},
        {"screen", 0xC17044, 8}, {"usertracks", 0xB6B976, 2}, {"gstring", reinterpret_cast<uintptr_t>(&gString[0]), sizeof(gString)},
    };
    base.clear(); for (const auto& r : regs) base.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(r.addr), r.size));
    // bytes iniciales de todas las entradas parcheables (comprobacion de restauracion)
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (const auto& s : sites) for (const uintptr_t a : {s.exe, s.dll, s.body}) if (a) entryBefore.push_back({a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8)});
    for (const auto& f : fns) entryBefore.push_back({f.address, ReversingProbe::Bytes(reinterpret_cast<void*>(f.address), 8)});
    const unsigned savedFP = _controlfp(0, 0);
    Write({{"event", "begin"}, {"layout", layout}, {"sites_ok", sitesOk}, {"bad_sites", siteInfo}, {"fp_control", savedFP}, {"object", uintptr_t(M())}});

    bool passed = layout && sitesOk; int pair = 0;
    if (passed) {
        for (size_t fi = 0; fi < fns.size(); ++fi) {
            const auto& f = fns[fi]; const std::string full = std::string(f.cls) + "::" + f.name;
            if (!only.empty() && only != full) continue;
            auto* hook = FindHook(f.cls, f.name);
            if (!hook) { Write({{"event", "error"}, {"reason", "hook not found"}, {"function", full}}); passed = false; continue; }
            const bool wasEnabled = hook->Hooked(), wasLocked = hook->Locked(); hook->LockState(false);
            std::vector<std::unique_ptr<EntryPatch>> patches; PatchSites(patches, Needs(f));
            int failures = 0;
            for (int i = 0; i < f.cases; ++i) {
                auto run = [&](bool candidate, bool verbose) -> json {
                    RestoreBase(); trace = json::array(); objBuf.fill(0xCD);
                    hook->State(candidate);
                    const bool switches = ReversingProbe::JumpsToDll(f.address) == candidate;   // despues de State()
                    json out = Exec(f, fi, i);
                    json st = json::array(); for (const auto& r : regs) st.push_back(Fnv(reinterpret_cast<void*>(r.addr), r.size));
                    out["state"] = st; out["trace"] = trace; out["switches"] = switches;
                    out["obj"] = Fnv(M(), sizeof(CAERadioTrackManager));
                    bool guards = true; for (size_t k = 0; k < 16; ++k) guards &= objBuf[k] == 0xCD && objBuf[16 + sizeof(CAERadioTrackManager) + k] == 0xCD;
                    out["guards"] = guards;
                    if (verbose) {
                        out["obj_bytes"] = Hex(M(), sizeof(CAERadioTrackManager));
                        json rb = json::object(); for (const auto& r : regs) rb[r.name] = Hex(reinterpret_cast<void*>(r.addr), r.size);
                        out["region_bytes"] = rb;
                    }
                    return out;
                };
                const json original = run(false, false), candidate = run(true, false);
                const bool ok = original == candidate && original["switches"] == true && original["guards"] == true && !original.contains("error");
                failures += !ok; passed &= ok;
                if (ok) {
                    const auto compact = [](const json& o) {
                        const std::string stDump = o.at("state").dump();
                        json c = {{"trace", o.at("trace")}, {"fnv", Fnv(stDump.data(), stDump.size(), o.at("obj").get<uint64>())}};
                        if (o.contains("ret")) c["ret"] = o.at("ret");
                        if (o.contains("key")) c["key"] = o.at("key");
                        if (o.contains("rect")) c["rect"] = o.at("rect");
                        if (o.contains("color")) c["color"] = o.at("color");
                        if (o.contains("tag")) c["tag"] = o.at("tag");
                        if (o.contains("cursor")) c["cursor"] = o.at("cursor");
                        return c;
                    };
                    Write({{"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true}, {"original", compact(original)}, {"candidate", compact(candidate)}, {"events", original["trace"].size()}});
                } else {
                    json diff = json::array(); for (auto& [k, v] : original.items()) if (!candidate.contains(k) || candidate[k] != v) diff.push_back(k);
                    const bool detail = failures <= 2;
                    const json vo = detail ? run(false, true) : json(nullptr), vc = detail ? run(true, true) : json(nullptr);
                    Write({{"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff}, {"original", detail ? vo : json(nullptr)}, {"candidate", detail ? vc : json(nullptr)}});
                }
            }
            patches.clear();
            hook->State(wasEnabled); hook->LockState(wasLocked);
            Write({{"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", f.cases}});
        }
    }
    RestoreBase();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t k = 0; k < regs.size(); ++k) restored &= base[k] == ReversingProbe::Bytes(reinterpret_cast<void*>(regs[k].addr), regs[k].size);
    for (auto& [a, b] : entryBefore) { const auto now = ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8); if (b != now) restored = false; }
    Write({{"event", passed ? "complete" : "error"}, {"pairs", pair}, {"state_restored", restored}});
}
}
