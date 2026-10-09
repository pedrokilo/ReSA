#pragma once
// Banco AudioWave2 (estilo Small2/PadMouse1/AudioFinal1, dentro del juego completo con partida cargada).
// Original (hook desactivado, codigo del EXE) frente a candidato (hook activado, codigo de la DLL) para 54 funciones de
// CAEScriptAudioEntity (8), CAEPoliceScannerAudioEntity (13), CAEPedAudioEntity (15) y CAEStreamingChannel (18).
//
// Seguridad: todo se ejecuta sobre objetos FALSOS en memoria propia (entidades de 0x21C/0x7C/0x15C bytes, un
// CAEStreamingChannel falso de 0x60098 bytes, un ped/fisico falsos, sonidos falsos de 0x74 bytes). Nunca se toca AEAudioHardware,
// AESoundManager, AudioEngine ni AESmoothFadeThread reales: sus funciones (y las de CAESound / CAETwinLoopSoundEntity) se sustituyen con
// EntryPatch (direccion del EXE + simbolo de la DLL, cuerpos calculados ANTES de crear ningun parche) por dobles trazados.
// DirectSound / IDirectSoundFXParamEq son objetos COM falsos con vtable propia (nada llega a dsound.dll salvo CoInitialize, que
// tambien se sustituye durante CAEStreamingChannel::Initialise). Los dobles ignoran los hilos ajenos al del banco (Mine()).
// El x87 se pone a 24 bits durante cada llamada, como en el juego.
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AEAudioUtility.h"
#include "Audio/AESmoothFadeThread.h"
#include "Audio/AEStreamThread.h"
#include "Audio/AudioEngine.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Hardware/AEStreamingChannel.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Entities/AEScriptAudioEntity.h"
#include "Audio/Entities/AEPoliceScannerAudioEntity.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Audio/Entities/AETwinLoopSoundEntity.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();

namespace AudioWave2Live {
inline bool done{}; inline std::ofstream log; inline json trace; inline std::string rawSnd;   // bytes crudos de cada CAESound pedido (solo diagnostico, se vuelcan si hay diferencia)
inline uint32 sc{}, seq{}, positive{};
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId() == owner; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
inline uint64 Hash(const void* p, size_t n) { uint64 h = 1469598103934665603ull; auto* b = static_cast<const uint8*>(p); for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; } return h; }
inline uint32 H(uint32 a) { a ^= a >> 16; a *= 0x7FEB352Du; a ^= a >> 15; a *= 0x846CA68Bu; a ^= a >> 16; return a; }
inline uint32 Rn() { return H(sc * 2654435761u + (seq++) * 40503u + 12345u); }
template<class T = uint32> inline T& At(void* base, size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base) + off); }
template<class T = uint32> inline T& G(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
inline uint64 Digest(size_t objSize); inline uint64 preDigest{}; inline size_t curSz{}; inline bool lastChanged{};
template<class R = void, class... A> inline R Th(uintptr_t a, void* self, A... x) { preDigest = Digest(curSz); return reinterpret_cast<R(__thiscall*)(void*, A...)>(a)(self, x...); }
inline void T(const json& j) { trace.push_back(j); }

// ---------------- memoria falsa ----------------
constexpr size_t OBJ_MAX = 0x60098;
alignas(16) inline std::array<uint8, OBJ_MAX + 64> obj{};        // entidad / canal bajo prueba
alignas(16) inline std::array<uint8, 0x400> phys{};              // CPhysical falso (placement en +4, m_matrix en +0x14)
alignas(16) inline std::array<uint8, 0x60> mat{};                // matriz falsa (posicion en +0x30)
alignas(16) inline std::array<uint8, 0x800> fakePed{}, otherPed{};
alignas(16) inline std::array<uint8, 8 * 0x74> snd{};           // 8 CAESound falsos
alignas(16) inline std::array<uint8, 0x60000> destination{}, source{};
constexpr uintptr_t SND_STRIDE = 0x74;
inline uint8* Snd(size_t k) { return snd.data() + k * SND_STRIDE; }

struct Reg { const void* p; size_t n; uint32 tag; };
inline std::array<Reg, 8> regs{};
inline uint32 Rel(const void* p) {
    const auto u = uintptr_t(p); if (!u) return 0;
    for (auto& r : regs) { const auto b = uintptr_t(r.p); if (r.p && u >= b && u < b + r.n) return r.tag + uint32(u - b); }
    return 0xEEEEEEEEu;
}
inline void InitRegs() {
    regs = {{ { obj.data(), obj.size(), 0x01000000 }, { phys.data(), phys.size(), 0x02000000 }, { fakePed.data(), fakePed.size(), 0x04000000 },
              { snd.data(), snd.size(), 0x08000000 }, { otherPed.data(), otherPed.size(), 0x10000000 }, { destination.data(), destination.size(), 0x20000000 },
              { source.data(), source.size(), 0x40000000 }, { mat.data(), mat.size(), 0x30000000 } }};
}
inline void RawAppend(const void* s) { static const char* hx = "0123456789abcdef"; rawSnd += '|'; for (size_t i = 0; i < SND_STRIDE; ++i) { const uint8 v = static_cast<const uint8*>(s)[i]; rawSnd += hx[v >> 4]; rawSnd += hx[v & 15]; } }
inline uint64 SoundDigest(const void* s) {   // enmascara relleno (+0x55, +0x6A..0x6B, +0x72..0x73), camara/tiempos (+0x40..0x4F) y m_IsAudioHardwareAware (+0x5A)
    uint8 b[SND_STRIDE]; std::memcpy(b, s, SND_STRIDE); b[0x55] = 0; std::memset(b + 0x40, 0, 0x10); std::memset(b + 0x5A, 0, 2); std::memset(b + 0x6A, 0, 2); std::memset(b + 0x72, 0, 2); return Hash(b, SND_STRIDE);
}

// ---------------- dobles del juego (EXE y DLL comparten estos cuerpos) ----------------
inline bool __fastcall SbLoaded(void*, void*, int32 bank, int32 slot) { if (!Mine()) return false; ++positive; const bool r = (Rn() & 4) != 0; T(json::array({ "loaded", bank & 0xFFFF, slot & 0xFFFF, r })); return r; }
inline void __fastcall SbLoad(void*, void*, int32 bank, int32 slot) { if (!Mine()) return; ++positive; T(json::array({ "load_bank", bank & 0xFFFF, slot & 0xFFFF })); }
inline void __fastcall MgrCancelSlot(void*, void*, int32 slot, int32 full) { if (!Mine()) return; ++positive; T(json::array({ "cancel_slot", slot & 0xFFFF, full & 0xFF })); }
inline int16 __fastcall MgrPlayingSlot(void*, void*, int32 slot) { if (!Mine()) return 0; ++positive; const int16 r = int16(Rn() % 4); T(json::array({ "playing_slot", slot & 0xFFFF, r })); return r; }
inline int16 __fastcall MgrEventPlaying(void*, void*, int32 ev, void* ent) { if (!Mine()) return 0; ++positive; const int16 r = int16((Rn() >> 3) % 3 == 0); T(json::array({ "event_playing", ev & 0xFFFF, Rel(ent), r })); return r; }
inline int16 __fastcall MgrEventPlayingPhys(void*, void*, int32 ev, void* ent, void* ph) { if (!Mine()) return 0; ++positive; const int16 r = int16((Rn() >> 3) % 3 == 0); T(json::array({ "event_playing_phys", ev & 0xFFFF, Rel(ent), Rel(ph), r })); return r; }
inline void __fastcall MgrCancelOwned(void*, void*, void* ent, int32 full) { if (!Mine()) return; ++positive; T(json::array({ "cancel_owned", Rel(ent), full & 0xFF })); }
inline CAESound* __fastcall MgrRequest(void*, void*, CAESound* s) {
    if (!Mine()) return nullptr; ++positive; const uint32 r = Rn(); RawAppend(s); T(json::array({ "request", SoundDigest(s), s == reinterpret_cast<CAESound*>(Snd(0)) }));
    return (r % 5 == 0) ? nullptr : reinterpret_cast<CAESound*>(Snd(1 + r % 3));
}
inline void __fastcall SndRegister(CAESound* self, void*, void* ent) { if (!Mine()) return; ++positive; T(json::array({ "register", Rel(ent), Rel(self) })); }
inline void __fastcall SndStopForget(CAESound* self, void*) { if (!Mine()) return; ++positive; T(json::array({ "stop_forget", Rel(self) })); }
inline void __fastcall SndPosition(CAESound* s, void*, CVector p) { if (!Mine()) return; s->m_CurrPos = p; }
inline int32 __cdecl RndRange(int32 lo, int32 hi) { if (!Mine()) return lo; ++positive; const uint32 span = hi >= lo ? uint32(hi - lo + 1) : 1u; const int32 r = lo + int32(Rn() % span); T(json::array({ "random", lo, hi, r })); return r; }
inline uintptr_t playerChoice{};
inline CPlayerPed* __cdecl FindPlayer(int32 id) { if (!Mine()) return nullptr; ++positive; T(json::array({ "find_player", id })); return reinterpret_cast<CPlayerPed*>(playerChoice); }
inline void __fastcall Frontend(void*, void*, int32 ev, float vol, float speed) { if (!Mine()) return; ++positive; T(json::array({ "frontend", ev, F(vol), F(speed) })); }
inline bool __fastcall TuneActive(void*, void*) { if (!Mine()) return false; ++positive; const bool r = (Rn() & 1) != 0; T(json::array({ "tune_active", r })); return r; }
inline bool __fastcall Fade(void*, void*, IDirectSoundBuffer* b, float vol, int16 time, bool stop) { if (!Mine()) return false; ++positive; const bool r = (Rn() & 1) != 0; T(json::array({ "fade", Rel(b) ? 1u : 0u, F(vol), time, stop, r })); return r; }
inline void __fastcall CancelFadeStub(void*, void*, IDirectSoundBuffer*) { if (!Mine()) return; ++positive; T(json::array({ "cancel_fade" })); }
inline void __fastcall SetOrig(CAEAudioChannel* self, void*, uint32 rate) { if (!Mine()) return; ++positive; T(json::array({ "set_original_frequency", rate })); self->m_nFrequency = rate; self->m_nOriginalFrequency = rate; }
inline void __fastcall SetFreq(CAEAudioChannel* self, void*, uint32 rate) { if (!Mine()) return; ++positive; T(json::array({ "set_frequency", rate })); self->m_nFrequency = rate; }
inline uint64 nowMs{};
inline uint64 __cdecl NowStub() { return nowMs; }
// CAETwinLoopSoundEntity
inline void __fastcall TwinInit(void* self, void*, int32 slot, int32 a, int32 b, void* parent, int32 mn, int32 mx, int32 pa, int32 pb) { if (!Mine()) return; ++positive; T(json::array({ "twin_init", slot & 0xFFFF, a & 0xFFFF, b & 0xFFFF, Rel(parent), mn & 0xFFFF, mx & 0xFFFF, pa & 0xFFFF, pb & 0xFFFF })); }
inline void __fastcall TwinPlay(void* self, void*, float x, float y, float z, float vol, float speed, float roll, float dop, int32 flags) { if (!Mine()) return; ++positive; T(json::array({ "twin_play", F(x), F(y), F(z), F(vol), F(speed), F(roll), F(dop), flags & 0xFFFF })); }
inline void __fastcall TwinUpdate(void* self, void*, float x, float y, float z, float vol, float speed) { if (!Mine()) return; ++positive; T(json::array({ "twin_update", F(x), F(y), F(z), F(vol), F(speed) })); }
inline void __fastcall TwinStop(void* self, void*) { if (!Mine()) return; ++positive; T(json::array({ "twin_stop" })); }
// funciones de las propias clases que otras funciones del banco llaman (solo se sustituyen donde hace falta)
inline void __fastcall Process(void*, void*, int32 ev, CVector* p, void* ph, float vol, float speed) { if (!Mine()) return; ++positive; T(json::array({ "process", ev, F(p->x), F(p->y), F(p->z), Rel(ph), F(vol), F(speed) })); }
inline void __fastcall PlayResident(void*, void*, int32 slot, int32 bank, int32 sfx, int32 ev, CVector* p, void* ph, float vol, float speed, int32 playPosn, float maxDist) {
    if (!Mine()) return; ++positive; T(json::array({ "play_resident", slot, bank, sfx, ev, F(p->x), F(p->y), F(p->z), Rel(ph), F(vol), F(speed), playPosn & 0xFFFF, F(maxDist) })); }
inline void __fastcall PlayLoadedStub(void*, void*) { if (!Mine()) return; ++positive; T(json::array({ "play_loaded" })); }
inline void __fastcall HFoot(void*, void*, int32 ev, float vol, float speed, int32 surf) { if (!Mine()) return; ++positive; T(json::array({ "h_footstep", ev, F(vol), F(speed), surf })); }
inline void __fastcall HSkate(void*, void*, int32 ev, float vol, float speed) { if (!Mine()) return; ++positive; T(json::array({ "h_skate", ev, F(vol), F(speed) })); }
inline void __fastcall HLand(void*, void*, int32 ev) { if (!Mine()) return; ++positive; T(json::array({ "h_landing", ev })); }
inline void __fastcall HSwing(void*, void*, int32 ev, int32 a3, uint32 vol) { if (!Mine()) return; ++positive; T(json::array({ "h_swing", ev, a3, vol })); }
inline void __fastcall HHit(void*, void*, int32 ev, void* ph, int32 surf, float vol, uint32 maxVol) { if (!Mine()) return; ++positive; T(json::array({ "h_hit", ev, Rel(ph), surf & 0xFF, F(vol), maxVol })); }
inline void __fastcall HJack(void*, void*, int32 ev) { if (!Mine()) return; ++positive; T(json::array({ "h_jacked", ev })); }
inline void __fastcall HSplash(void*, void*, int32 ev) { if (!Mine()) return; ++positive; T(json::array({ "h_splash", ev })); }
inline void __fastcall HWake(void*, void*, int32 ev) { if (!Mine()) return; ++positive; T(json::array({ "h_wake", ev })); }
inline HRESULT __stdcall CoInitStub(void*) { if (Mine()) { ++positive; T(json::array({ "co_initialize" })); } return S_OK; }

// ---------------- COM falso (vtables crudas) ----------------
inline std::array<uintptr_t, 32> comVT{}; inline std::array<uintptr_t, 8> eqVT{}, devVT{}, decVT{};
inline uintptr_t comObject{}, silObject{}, eqObject{}, devObject{}, decA[2]{}, decB[2]{};
inline int createCount{};
inline uint32 ComId(void* s) { return uintptr_t(s) == uintptr_t(&comObject) ? 0 : uintptr_t(s) == uintptr_t(&silObject) ? 1 : 2; }
inline HRESULT __stdcall ComRelease(void* s) { if (Mine()) { ++positive; T(json::array({ "release", ComId(s) })); } return 0; }
inline HRESULT __stdcall ComStop(void* s) { if (Mine()) { ++positive; T(json::array({ "stop", ComId(s) })); } return S_OK; }
inline HRESULT __stdcall ComPlay(void* s, DWORD a, DWORD b, DWORD flags) { if (Mine()) { ++positive; T(json::array({ "play", ComId(s), a, b, flags })); } return S_OK; }
inline HRESULT __stdcall ComSetVolume(void* s, LONG v) { if (Mine()) { ++positive; T(json::array({ "set_volume", ComId(s), v })); } return S_OK; }
inline HRESULT __stdcall ComSetPosition(void* s, DWORD v) { if (Mine()) { ++positive; T(json::array({ "set_position", ComId(s), v })); } return S_OK; }
inline uint32 cursorValue{};
inline HRESULT __stdcall ComGetPosition(void*, DWORD* cur, DWORD* wr) { if (Mine()) { ++positive; T(json::array({ "get_position", cur != nullptr, wr != nullptr })); } if (cur) *cur = cursorValue; if (wr) *wr = 0; return S_OK; }
inline uint32 lockSize{};
inline HRESULT __stdcall ComLock(void* s, DWORD off, DWORD bytes, void** p1, DWORD* n1, void** p2, DWORD* n2, DWORD flags) {
    if (Mine()) { ++positive; T(json::array({ "lock", ComId(s), off, bytes, flags, p2 != nullptr, n2 != nullptr })); }
    if (sc % 7 == 0 && ComId(s) == 0) return E_FAIL;
    *p1 = destination.data(); *n1 = lockSize; return S_OK; }
inline HRESULT __stdcall ComUnlock(void* s, void* p, DWORD n, void* p2, DWORD n2) { if (Mine()) { ++positive; T(json::array({ "unlock", ComId(s), Rel(p), n, p2 != nullptr, n2, Hash(destination.data(), 0x8000) })); } return S_OK; }
inline int fxResult{};   // 0 = ok, 1 = fallo
inline HRESULT __stdcall ComSetFX(void* s, DWORD n, void* desc, DWORD* res) {
    if (Mine()) { ++positive; json d = json::array(); if (desc) { const auto* b = static_cast<const uint8*>(desc); for (DWORD i = 0; i < n; ++i) d.push_back(Hash(b + i * 32, 32)); }
        T(json::array({ "set_fx", n, d, res != nullptr })); }
    return fxResult ? E_FAIL : S_OK; }
inline int eqFail{};
inline HRESULT __stdcall ComGetObject(void* s, const GUID* g, DWORD idx, const GUID* iid, void** out) {
    if (Mine()) { ++positive; T(json::array({ "get_object", Hash(g, 16), idx, Hash(iid, 16) })); }
    if (eqFail && int(idx) == eqFail - 1) return E_FAIL;
    *out = &eqObject; return S_OK; }
inline HRESULT __stdcall EqSetAll(void*, const DSFXParamEq* p) { if (Mine()) { ++positive; T(json::array({ "eq_set", F(p->fCenter), F(p->fBandwidth), F(p->fGain) })); } return S_OK; }
inline HRESULT __stdcall DevCreate(void*, const DSBUFFERDESC* d, IDirectSoundBuffer** out, IUnknown*) {
    ++createCount;
    if (Mine()) { ++positive;
        T(json::array({ "create_buffer", d->dwSize, d->dwFlags, d->dwBufferBytes, d->dwReserved, Hash(&d->guid3DAlgorithm, 16),
                        d->lpwfxFormat ? Rel(d->lpwfxFormat) : 0u, d->lpwfxFormat ? Hash(d->lpwfxFormat, 16) : 0ull, d->lpwfxFormat ? uint32(d->lpwfxFormat->cbSize) : 0u })); }
    if (createCount == 1 && sc % 5 == 0) return E_FAIL;     // el buffer de silencio (2.a llamada) siempre se crea
    *out = reinterpret_cast<IDirectSoundBuffer*>(createCount == 1 ? &comObject : &silObject); return S_OK; }
// decodificador falso (vtable de 8 entradas como CAEStreamingDecoder del EXE)
inline uint32 DecId(void* s) { return uintptr_t(s) == uintptr_t(decA) ? 0 : uintptr_t(s) == uintptr_t(decB) ? 1 : 2; }
inline int fillCalls{};
inline uint32 __fastcall DecFill(void* s, void*, void* buf, uint32 size) {
    if (Mine()) { ++positive; T(json::array({ "dec_fill", DecId(s), Rel(buf), size })); } else return 0;
    uint32 w = (fillCalls++ == 0) ? uint32((uint64(size) * (sc % 4)) / 3) : size; if (w > size) w = size;
    if (sc % 11 == 0 && DecId(s) == 1 && fillCalls == 1) w = 0;
    auto* b = static_cast<uint8*>(buf); for (uint32 i = 0; i < w; ++i) b[i] = uint8(i * 17 + sc); return w; }
inline int32 __fastcall DecLen(void* s, void*) { if (Mine()) { ++positive; T(json::array({ "dec_length", DecId(s) })); } return 1000 + int32(sc % 50); }
inline int32 __fastcall DecPlay(void* s, void*) { if (Mine()) { ++positive; T(json::array({ "dec_playtime", DecId(s) })); } return 77 + int32(sc % 40); }
inline void __fastcall DecCursor(void* s, void*, uint32 pos) { if (Mine()) { ++positive; T(json::array({ "dec_cursor", DecId(s), pos })); } }
inline int32 __fastcall DecRate(void* s, void*) { if (Mine()) { ++positive; T(json::array({ "dec_rate", DecId(s) })); } return DecId(s) == 0 ? 48000 : (sc % 3 == 0 ? 44100 : 48000); }
inline void* __fastcall DecDtor(void* s, void*, uint32 flags) { if (Mine()) { ++positive; T(json::array({ "dec_destroy", DecId(s), flags })); } return s; }
inline int32 __fastcall DecId32(void* s, void*) { if (Mine()) { ++positive; T(json::array({ "dec_stream_id", DecId(s) })); } return 900 + int32(sc % 13) + int32(DecId(s)); }
inline void BuildCom() {
    comVT.fill(reinterpret_cast<uintptr_t>(&ComStop));
    comVT[2] = reinterpret_cast<uintptr_t>(&ComRelease); comVT[4] = reinterpret_cast<uintptr_t>(&ComGetPosition); comVT[11] = reinterpret_cast<uintptr_t>(&ComLock);
    comVT[12] = reinterpret_cast<uintptr_t>(&ComPlay); comVT[13] = reinterpret_cast<uintptr_t>(&ComSetPosition); comVT[15] = reinterpret_cast<uintptr_t>(&ComSetVolume);
    comVT[18] = reinterpret_cast<uintptr_t>(&ComStop); comVT[19] = reinterpret_cast<uintptr_t>(&ComUnlock); comVT[21] = reinterpret_cast<uintptr_t>(&ComSetFX); comVT[23] = reinterpret_cast<uintptr_t>(&ComGetObject);
    comObject = silObject = uintptr_t(comVT.data());
    eqVT.fill(reinterpret_cast<uintptr_t>(&ComRelease)); eqVT[3] = reinterpret_cast<uintptr_t>(&EqSetAll); eqObject = uintptr_t(eqVT.data());
    devVT.fill(reinterpret_cast<uintptr_t>(&ComRelease)); devVT[3] = reinterpret_cast<uintptr_t>(&DevCreate); devObject = uintptr_t(devVT.data());
    decVT.fill(reinterpret_cast<uintptr_t>(&DecRate));
    decVT[1] = reinterpret_cast<uintptr_t>(&DecFill); decVT[2] = reinterpret_cast<uintptr_t>(&DecLen); decVT[3] = reinterpret_cast<uintptr_t>(&DecPlay); decVT[4] = reinterpret_cast<uintptr_t>(&DecCursor);
    decVT[5] = reinterpret_cast<uintptr_t>(&DecRate); decVT[6] = reinterpret_cast<uintptr_t>(&DecDtor); decVT[7] = reinterpret_cast<uintptr_t>(&DecId32);
    decA[0] = decB[0] = uintptr_t(decVT.data());
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls, const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if (c.Name() == cls) for (auto& h : c.Items()) if (h->Name() == fn) r = h.get(); });
    return r;
}
// Sigue los saltos de la tabla ILT hasta el cuerpo real, pero NUNCA entra en el EXE: con el hook desactivado la DLL escribe en su
// propio inicio un 'jmp EXE' (Simple.cpp, m_LibHookContent) y seguirlo devolveria la direccion del EXE.
inline uintptr_t Body(uintptr_t a) { for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++j) { const uintptr_t t = a + 5 + *reinterpret_cast<int32*>(a + 1); if (t >= 0x400000 && t < 0x1000000) break; a = t; } return a; }

// ---------------- tabla de sitios parcheables ----------------
enum Site : int {
    S_LOADED, S_LOAD, S_CANCEL, S_PLAYINGSLOT, S_EVENTPLAY, S_EVENTPLAYPHYS, S_CANCELOWNED, S_REQUEST, S_REG, S_STOP, S_POS, S_RANDOM, S_FINDPED,
    S_FRONTEND, S_TUNE, S_FADE, S_CANCELFADE, S_SETORIG, S_SETFREQ, S_NOW, S_TWININIT, S_TWINPLAY, S_TWINUPD, S_TWINSTOP, S_PERMANENT_END,
    S_PROCESS = S_PERMANENT_END, S_PLAYRES, S_PLAYLOADED, S_HFOOT, S_HSKATE, S_HLAND, S_HSWING, S_HHIT, S_HJACK, S_HSPLASH, S_HWAKE, S_COUNT
};
struct SiteInfo { const char* name; uintptr_t exe, dll, body, stub; };
inline std::array<SiteInfo, S_COUNT> sites{};
template<class P> inline uintptr_t A(P p) { return std::bit_cast<uintptr_t>(p); }
inline void BuildSites() {
    // Los cuerpos se calculan con el hook del sitio ACTIVADO (si existe): desactivado, el inicio de la funcion de la DLL es un 'jmp EXE'
    // y no hay cuerpo propio que parchear. Despues se devuelve el hook a su estado y bloqueo originales.
    auto set = [&](int i, const char* n, uintptr_t exe, uintptr_t dll, uintptr_t stub, const char* cls = nullptr, const char* hook = nullptr) {
        auto* h = cls ? FindHook(cls, hook ? hook : n) : nullptr;
        const bool was = h && h->Hooked(), lk = h && h->Locked();
        if (h) { h->LockState(false); h->State(true); }
        sites[i] = { n, exe, dll, Body(dll), stub };
        if (h) { h->State(was); h->LockState(lk); }
    };
    auto U = [](auto f) { return reinterpret_cast<uintptr_t>(f); };
    set(S_LOADED, "IsSoundBankLoaded", 0x4D88C0, A(&CAEAudioHardware::IsSoundBankLoaded), U(&SbLoaded), "CAEAudioHardware");
    set(S_LOAD, "LoadSoundBank", 0x4D88A0, A(&CAEAudioHardware::LoadSoundBank), U(&SbLoad), "CAEAudioHardware");
    set(S_CANCEL, "CancelSoundsInBankSlot", 0x4EFC60, A(&CAESoundManager::CancelSoundsInBankSlot), U(&MgrCancelSlot), "CAESoundManager");
    set(S_PLAYINGSLOT, "AreSoundsPlayingInBankSlot", 0x4EF520, A(&CAESoundManager::AreSoundsPlayingInBankSlot), U(&MgrPlayingSlot), "CAESoundManager");
    set(S_EVENTPLAY, "AreSoundsOfThisEventPlayingForThisEntity", 0x4EF570, A(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity), U(&MgrEventPlaying), "CAESoundManager");
    set(S_EVENTPLAYPHYS, "AreSoundsOfThisEventPlayingForThisEntityAndPhysical", 0x4EF5D0, A(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical), U(&MgrEventPlayingPhys), "CAESoundManager");
    set(S_CANCELOWNED, "CancelSoundsOwnedByAudioEntity", 0x4EFCD0, A(&CAESoundManager::CancelSoundsOwnedByAudioEntity), U(&MgrCancelOwned), "CAESoundManager");
    set(S_REQUEST, "RequestNewSound", 0x4EFB10, A(&CAESoundManager::RequestNewSound), U(&MgrRequest), "CAESoundManager");
    set(S_REG, "RegisterWithPhysicalEntity", 0x4EF820, A(&CAESound::RegisterWithPhysicalEntity), U(&SndRegister), "CAESound");
    set(S_STOP, "StopSoundAndForget", 0x4EF850, A(&CAESound::StopSoundAndForget), U(&SndStopForget), "CAESound");
    set(S_POS, "SetPosition", 0x4EF880, A(&CAESound::SetPosition), U(&SndPosition), "CAESound");
    set(S_RANDOM, "GetRandomNumberInRange", 0x4D9C10, A(&CAEAudioUtility::GetRandomNumberInRange<int32>), U(&RndRange), "CAEAudioUtility", "GetRandomNumberInRange<int32>-int");
    set(S_FINDPED, "FindPlayerPed", 0x56E210, A(&FindPlayerPed), U(&FindPlayer));
    set(S_FRONTEND, "ReportFrontendAudioEvent", 0x506EA0, A(&CAudioEngine::ReportFrontendAudioEvent), U(&Frontend), "CAudioEngine");
    set(S_TUNE, "IsLoadingTuneActive", 0x506D90, A(&CAudioEngine::IsLoadingTuneActive), U(&TuneActive), "CAudioEngine");
    set(S_FADE, "RequestFade", 0x4EEAA0, A(&CAESmoothFadeThread::RequestFade), U(&Fade), "CAESmoothFadeThread");
    set(S_CANCELFADE, "CancelFade", 0x4EEA50, A(&CAESmoothFadeThread::CancelFade), U(&CancelFadeStub), "CAESmoothFadeThread");
    set(S_SETORIG, "SetOriginalFrequency", 0x4D7A70, A(&CAEAudioChannel::SetOriginalFrequency), U(&SetOrig), "CAEAudioChannel");
    set(S_SETFREQ, "SetFrequency", 0x4D7A50, A(&CAEAudioChannel::SetFrequency), U(&SetFreq), "CAEAudioChannel");
    set(S_NOW, "GetCurrentTimeInMS", 0x4D9E80, A(&CAEAudioUtility::GetCurrentTimeInMS), U(&NowStub), "CAEAudioUtility");
    set(S_TWININIT, "Twin::Initialise", 0x4F28A0, A(&CAETwinLoopSoundEntity::Initialise), U(&TwinInit), "CAETwinLoopSoundEntity", "Initialise");
    set(S_TWINPLAY, "Twin::PlayTwinLoopSound", 0x4F2CB0, A(&CAETwinLoopSoundEntity::PlayTwinLoopSound), U(&TwinPlay), "CAETwinLoopSoundEntity", "PlayTwinLoopSound");
    set(S_TWINUPD, "Twin::UpdateTwinLoopSound", 0x4F29A0, A(&CAETwinLoopSoundEntity::UpdateTwinLoopSound), U(&TwinUpdate), "CAETwinLoopSoundEntity", "UpdateTwinLoopSound");
    set(S_TWINSTOP, "Twin::StopSoundAndForget", 0x4F2960, A(&CAETwinLoopSoundEntity::StopSoundAndForget), U(&TwinStop), "CAETwinLoopSoundEntity", "StopSoundAndForget");
    set(S_PROCESS, "ProcessMissionAudioEvent", 0x4ECCF0, A(&CAEScriptAudioEntity::ProcessMissionAudioEvent), U(&Process), "CAEScriptAudioEntity");
    set(S_PLAYRES, "PlayResidentSoundEvent", 0x4EC550, A(&CAEScriptAudioEntity::PlayResidentSoundEvent), U(&PlayResident), "CAEScriptAudioEntity");
    set(S_PLAYLOADED, "PlayLoadedDialogue", 0x4E6F60, A(&CAEPoliceScannerAudioEntity::PlayLoadedDialogue), U(&PlayLoadedStub), "CAEPoliceScannerAudioEntity");
    set(S_HFOOT, "HandleFootstepEvent", 0x4E13A0, A(&CAEPedAudioEntity::HandleFootstepEvent), U(&HFoot), "CAEPedAudioEntity");
    set(S_HSKATE, "HandleSkateEvent", 0x4E17E0, A(&CAEPedAudioEntity::HandleSkateEvent), U(&HSkate), "CAEPedAudioEntity");
    set(S_HLAND, "HandleLandingEvent", 0x4E18E0, A(&CAEPedAudioEntity::HandleLandingEvent), U(&HLand), "CAEPedAudioEntity");
    set(S_HSWING, "HandlePedSwing", 0x4E1A40, A(&CAEPedAudioEntity::HandlePedSwing), U(&HSwing), "CAEPedAudioEntity");
    set(S_HHIT, "HandlePedHit", 0x4E1CC0, A(&CAEPedAudioEntity::HandlePedHit), U(&HHit), "CAEPedAudioEntity");
    set(S_HJACK, "HandlePedJacked", 0x4E2350, A(&CAEPedAudioEntity::HandlePedJacked), U(&HJack), "CAEPedAudioEntity");
    set(S_HSPLASH, "HandleSwimSplash", 0x4E26A0, A(&CAEPedAudioEntity::HandleSwimSplash), U(&HSplash), "CAEPedAudioEntity");
    set(S_HWAKE, "HandleSwimWake", 0x4E2790, A(&CAEPedAudioEntity::HandleSwimWake), U(&HWake), "CAEPedAudioEntity");
}
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;
    void Add(int i) {
        const auto& s = sites[i];
        v.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.exe, reinterpret_cast<void*>(s.stub)));
        v.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.dll, reinterpret_cast<void*>(s.stub)));
        if (s.body != s.dll) v.push_back(std::make_unique<ReversingProbe::EntryPatch>(s.body, reinterpret_cast<void*>(s.stub)));
    }
};

// ---------------- tabla de funciones ----------------
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn, 54> fns{{
    {"CAEScriptAudioEntity","Constructor",0x5074D0,0,-1,128},
    {"CAEScriptAudioEntity","Initialise",0x5B9B60,0,-1,128},
    {"CAEScriptAudioEntity","Reset",0x4EC150,0,-1,256},
    {"CAEScriptAudioEntity","GetMissionAudioEvent",0x4EC020,0,-1,128},
    {"CAEScriptAudioEntity","Service",0x4EC900,0,-1,256},
    {"CAEScriptAudioEntity","ReportMissionAudioEvent-1",0x4EE960,0,-1,256},
    {"CAEScriptAudioEntity","ReportMissionAudioEvent-2",0x4EE940,0,-1,256},
    {"CAEScriptAudioEntity","PlayResidentSoundEvent",0x4EC550,0,-1,1024},
    {"CAEPoliceScannerAudioEntity","Constructor",0x56DA00,0,-1,128},
    {"CAEPoliceScannerAudioEntity","Destructor",0x4E6E00,0,-1,512},
    {"CAEPoliceScannerAudioEntity","StaticInitialise",0x5B9C30,0,-1,128},
    {"CAEPoliceScannerAudioEntity","Reset",0x4E6E90,0,-1,512},
    {"CAEPoliceScannerAudioEntity","EnableScanner",0x4E6DB0,0,-1,64},
    {"CAEPoliceScannerAudioEntity","DisableScanner",0x4E71B0,0,-1,512},
    {"CAEPoliceScannerAudioEntity","StopScanner",0x4E6DC0,0,-1,512},
    {"CAEPoliceScannerAudioEntity","FinishedPlayingScannerDialogue",0x4E6C30,0,-1,512},
    {"CAEPoliceScannerAudioEntity","PrepSlots",0x4E6BC0,0,-1,512},
    {"CAEPoliceScannerAudioEntity","PopulateScannerDialogueLists",0x4E6B60,0,-1,512},
    {"CAEPoliceScannerAudioEntity","CanWePlayNewScannerDialogue",0x4E6C00,0,-1,512},
    {"CAEPoliceScannerAudioEntity","PlayPoliceScannerDialogue",0x4E6ED0,0,-1,512},
    {"CAEPoliceScannerAudioEntity","UpdateParameters",0,0x85F368,0,512},
    {"CAEPedAudioEntity","Initialise",0x4E0E80,0,-1,128},
    {"CAEPedAudioEntity","StaticInitialise",0x5B98A0,0,-1,64},
    {"CAEPedAudioEntity","Terminate",0x4E1360,0,-1,256},
    {"CAEPedAudioEntity","AddAudioEvent",0x4E2BB0,0,-1,1024},
    {"CAEPedAudioEntity","TurnOnJetPack",0x4E28A0,0,-1,256},
    {"CAEPedAudioEntity","TurnOffJetPack",0x4E2A70,0,-1,128},
    {"CAEPedAudioEntity","StopJetPackSound",0x4E1120,0,-1,256},
    {"CAEPedAudioEntity","UpdateJetPack",0x4E0EE0,0,-1,1024},
    {"CAEPedAudioEntity","PlayWindRush",0x4E1170,0,-1,64},
    {"CAEPedAudioEntity","HandleFootstepEvent",0x4E13A0,0,-1,1024},
    {"CAEPedAudioEntity","HandleSkateEvent",0x4E17E0,0,-1,512},
    {"CAEPedAudioEntity","HandleLandingEvent",0x4E18E0,0,-1,512},
    {"CAEPedAudioEntity","HandleSwimSplash",0x4E26A0,0,-1,256},
    {"CAEPedAudioEntity","HandleSwimWake",0x4E2790,0,-1,256},
    {"CAEPedAudioEntity","PlayShirtFlap",0x4E2A90,0,-1,256},
    {"CAEStreamingChannel","AddFX",0x4F1AE0,0,-1,256},
    {"CAEStreamingChannel","RemoveFX",0x4F1C20,0,-1,128},
    {"CAEStreamingChannel","SetBassEQ",0x4F1F30,0,-1,512},
    {"CAEStreamingChannel","Initialise",0x4F22F0,0,-1,128},
    {"CAEStreamingChannel","InitialiseSilence",0x4F1C70,0,-1,64},
    {"CAEStreamingChannel","Destructor",0x4F2200,0,-1,256},
    {"CAEStreamingChannel","Pause",0x4F2170,0,-1,128},
    {"CAEStreamingChannel","Stop-class",0x4F1A90,0,-1,128},
    {"CAEStreamingChannel","Stop-virtual",0,0x85F3F0,7,128},
    {"CAEStreamingChannel","SetReady",0x4F1FF0,0,-1,256},
    {"CAEStreamingChannel","SetNextStream",0x4F1DE0,0,-1,128},
    {"CAEStreamingChannel","GetActiveTrackID",0x4F1A40,0,-1,64},
    {"CAEStreamingChannel","GetPlayingTrackID",0x4F1A60,0,-1,128},
    {"CAEStreamingChannel","FillBuffer",0x4F1E20,0,-1,512},
    {"CAEStreamingChannel","UpdatePlayTime",0x4F18A0,0,-1,1024},
    {"CAEStreamingChannel","IsSoundPlaying",0,0x85F3F0,2,128},
    {"CAEStreamingChannel","Play",0,0x85F3F0,5,128},
    {"CAEStreamingChannel","SetFrequencyScalingFactor",0,0x85F3F0,8,512},
}};
// Sitios dinamicos que necesita cada funcion (los permanentes siempre estan puestos)
inline std::vector<int> Dyn(const std::string& n) {
    if (n == "CAEScriptAudioEntity::Service") return { S_PLAYRES };
    if (n == "CAEScriptAudioEntity::ReportMissionAudioEvent-1" || n == "CAEScriptAudioEntity::ReportMissionAudioEvent-2") return { S_PROCESS };
    if (n == "CAEPoliceScannerAudioEntity::UpdateParameters") return { S_PLAYLOADED };
    if (n == "CAEPedAudioEntity::AddAudioEvent") return { S_HFOOT, S_HSKATE, S_HLAND, S_HSWING, S_HHIT, S_HJACK, S_HSPLASH, S_HWAKE };
    return {};
}

// ---------------- fixtures ----------------
inline void Fill(void* p, size_t n, uint8 v) { std::memset(p, v, n); }
inline void SetupCommon(uint32 I, size_t objSize, uint8 fillByte) {
    Fill(obj.data(), obj.size(), 0xAB); Fill(obj.data(), objSize, fillByte);
    Fill(phys.data(), phys.size(), 0); Fill(mat.data(), mat.size(), 0); Fill(fakePed.data(), fakePed.size(), 0); Fill(otherPed.data(), otherPed.size(), 0x11);
    for (size_t k = 0; k < 3; ++k) { const float v = float(int32(H(I * 7 + uint32(k)) % 2001) - 1000) * 0.5f; At<float>(phys.data(), 4 + k * 4) = v; At<float>(mat.data(), 0x30 + k * 4) = v + 1.f; At<float>(fakePed.data(), 4 + k * 4) = v * 0.25f; }
    At<uintptr_t>(phys.data(), 0x14) = (I & 1) ? uintptr_t(mat.data()) : 0;
    At<uintptr_t>(fakePed.data(), 0x14) = (I % 3 == 0) ? uintptr_t(mat.data()) : 0;
    At<uint32>(fakePed.data(), 0x46C) = (H(I) % 5 == 0) ? 0x200u : 0u;      // bIsInTheAir (bit 9)
    At<uint8>(fakePed.data(), 0xBA) = uint8(H(I * 3 + 1) % 180);              // m_nContactSurface
    for (size_t k = 0; k < 8; ++k) {
        Fill(Snd(k), SND_STRIDE, 0); At<int16>(Snd(k), 0) = int16(10 + k); At<float>(Snd(k), 0x14) = float(int32(H(I + uint32(k)) % 90)) - 80.f; At<float>(Snd(k), 0x1C) = 1.f + float(H(I + uint32(k) * 5) % 9) * 0.1f;
        At<int16>(Snd(k), 0x70) = int16(100 + H(I + uint32(k)) % 2000); At<uint16>(Snd(k), 0x56) = uint16(H(I + 9) % 16);
    }
    Fill(destination.data(), destination.size(), 0x5A); Fill(source.data(), source.size(), 0x9B);
}
inline void SetupScript(uint32 I) {
    SetupCommon(I, 0x21C, 0xCD);
    for (uint32 k = 0; k < 4; ++k) {
        auto* l = obj.data() + 0x90 + k * 0x20; const uint32 h = H(I * 13 + k);
        for (uint32 c = 0; c < 3; ++c) At<float>(l, c * 4) = (h % 4 == 0) ? -1000.f : float(int32(H(h + c) % 400) - 200);
        At<uintptr_t>(l, 0xC) = (h >> 3) % 3 == 0 ? uintptr_t(phys.data()) : 0; At<uintptr_t>(l, 0x10) = (h >> 5) % 3 == 0 ? uintptr_t(Snd(k)) : 0;
        At<int32>(l, 0x14) = int32(h % 1200); At<int32>(l, 0x18) = (h >> 7) % 4 == 0 ? -1 : int32(h % 120); At<int32>(l, 0x1C) = (h >> 9) % 4 == 0 ? -1 : int32(h % 50);
    }
    At<uintptr_t>(obj.data(), 0x10) = 0;                                            // m_tempSound.m_PhysicalEntity
    At<uintptr_t>(obj.data(), 0x110) = (I & 1) ? uintptr_t(phys.data()) : 0;        // m_Physical
    At<uint32>(obj.data(), 0x80) = H(I);
    At<float>(obj.data(), 0x84) = float(I % 7) - 3.f; At<float>(obj.data(), 0x88) = 1.f; At<float>(obj.data(), 0x8C) = float(I % 3);
    obj[0x7C] = uint8(I % 2); obj[0x7D] = uint8((I / 2) % 2);
    At<uintptr_t>(obj.data(), 0x4) = 0xCDCDCDCD;
}
inline void SetupScanner(uint32 I, bool noObjFill = false) {
    SetupCommon(I, 0x7C, 0xCD);
    using S = CAEPoliceScannerAudioEntity;
    At<uintptr_t>(obj.data(), 0x10) = 0;
    G<uint32>(0xB7CB84) = 3000 + I % 100; G<uint8>(0xB6F065) = uint8((I / 16) % 2);
    S::s_fVolumeOffset = float(I % 9) - 8.f; S::s_bStoppingScanner = (I / 32) % 2; S::s_pSound = (I % 3 == 0) ? nullptr : reinterpret_cast<CAESound*>(Snd(I % 4));
    S::s_nAbortPlaybackTime = 2950u + uint32((I / 8) % 5) * 50; S::s_nPlaybackStartTime = 2950u + uint32((I / 4) % 5) * 50; S::s_nSectionPlaying = int16(I % 2);
    S::s_nScannerPlaybackState = static_cast<S::State>(I % 8); S::s_bScannerDisabled = (I / 8) % 2; S::s_NextNewScannerDialogueTime = 2900u + (I % 4) * 100u;
    S::s_pPSControlling = (I % 5 < 3) ? reinterpret_cast<S*>(obj.data()) : (I % 5 == 3 ? reinterpret_cast<S*>(otherPed.data()) : nullptr);
    for (int s = 0; s < 5; ++s) {
        S::s_SlotState[s] = int16((I + s) % 7 - 1);
        S::s_ScannerSlotFirst[s] = { static_cast<eSoundBank>(I % 7 == 0 ? -1 : 140 + s), static_cast<eSoundID>(I % 11 == 0 ? -1 : s + 1) };
        S::s_ScannerSlotSecond[s] = { static_cast<eSoundBank>(I % 9 == 0 ? -1 : 150 + s), static_cast<eSoundID>(I % 13 == 0 ? -1 : s + 4) };
    }
    S::s_pCurrentSlots = I % 7 == 3 ? nullptr : S::s_ScannerSlotFirst;
    *reinterpret_cast<float*>(0xB61D54) = float(I % 5) * 0.25f;
}
inline void SetupPed(uint32 I, bool zeroSoundsSometimes = true) {
    SetupCommon(I, 0x15C, 0xCD);
    const uint32 h = H(I * 5 + 3);
    obj[0x7C] = (I % 9 != 0); obj[0x7D] = 0xCD; At<int16>(obj.data(), 0x7E) = int16(int32(I % 8) - 2); At<uint32>(obj.data(), 0x80) = h;
    static const float vols[] = { -100.f, -99.f, -95.f, -50.f, -17.f, -16.f, -15.f, -14.f, 0.f, 2.f, 2.3f, 10.9f, 11.f, 13.f };
    At<float>(obj.data(), 0x84) = vols[h % 14]; At<float>(obj.data(), 0x88) = vols[(h >> 4) % 14]; At<float>(obj.data(), 0x8C) = vols[(h >> 8) % 14];
    static const float mults[] = { 0.4f, 0.41f, 0.5f, 0.7f, 0.71f, 0.72f, 0.4001f, 0.0f };
    At<float>(obj.data(), 0x90) = mults[(h >> 12) % 8];
    At<uintptr_t>(obj.data(), 0x94) = (I % 11 == 0) ? 0 : uintptr_t(fakePed.data());
    obj[0x98] = (I / 3) % 2;
    At<uintptr_t>(obj.data(), 0x9C) = (h >> 15) % 4 == 0 ? 0 : uintptr_t(Snd(1)); At<uintptr_t>(obj.data(), 0xA0) = (h >> 17) % 4 == 0 ? 0 : uintptr_t(Snd(2)); At<uintptr_t>(obj.data(), 0xA4) = (h >> 19) % 4 == 0 ? 0 : uintptr_t(Snd(3));
    if (zeroSoundsSometimes && I % 4 == 0) { At<uintptr_t>(obj.data(), 0x9C) = At<uintptr_t>(obj.data(), 0xA0) = At<uintptr_t>(obj.data(), 0xA4) = 0; }
    At<uint16>(obj.data(), 0x130) = uint16(I % 2); At<uintptr_t>(obj.data(), 0x10) = 0;
    playerChoice = (I % 3 == 0) ? 0 : (I % 3 == 1 ? uintptr_t(fakePed.data()) : uintptr_t(otherPed.data()));
}
inline void SetupStream(uint32 I, bool zero) {
    SetupCommon(I, OBJ_MAX, zero ? 0 : 0xCD);
    auto* self = reinterpret_cast<CAEStreamingChannel*>(obj.data());
    *reinterpret_cast<uintptr_t*>(obj.data()) = 0x85F3F0;
    self->m_pDirectSound = reinterpret_cast<IDirectSound*>(&devObject);
    self->m_pDirectSoundBuffer = (I % 13 == 0) ? nullptr : reinterpret_cast<IDirectSoundBuffer*>(&comObject);
    At<uintptr_t>(obj.data(), 0xC) = 0;
    self->m_pStreamingDecoder = (I % 5 == 0) ? nullptr : reinterpret_cast<CAEStreamingDecoder*>(decA);
    self->m_pNextStreamingDecoder = (I % 4 == 0) ? nullptr : reinterpret_cast<CAEStreamingDecoder*>(decB);
    self->m_pSilenceBuffer = (I % 6 == 0) ? nullptr : reinterpret_cast<IDirectSoundBuffer*>(&silObject);
    static const int32 states[] = { -7, -6, -5, -4, -3, -2, -1 };
    self->m_nState = static_cast<StreamingChannelState>(states[I % 7]);
    self->m_nFrequency = (I % 3 == 0) ? 44100 : 48000; self->m_nOriginalFrequency = 48000; self->m_Volume = float(int32(I % 31) - 25) * 0.5f;
    self->m_bEQEnabled = (I / 5) % 2; self->m_bLoopTrack = (I / 3) % 2; self->m_bNeedSwitch = (I / 21) % 2; self->m_bSilenced = (I / 42) % 2; self->m_bNeedToFinish = (I / 84) % 2;
    self->m_bShouldStop = (I / 168) % 2; self->m_bLooped = (I / 7) % 2; self->m_bInitialized = 0; self->m_lastSlot = uint8((I / 7) % 2); self->m_lastWrittenSlot = uint8((I / 9) % 2);
    self->m_lStoppingFrameCount = (I / 7) % 9; self->m_pBuffer = source.data(); self->m_nBufferStatus = (I / 2) % 3;
    self->m_nStreamPlayTimeMs = H(I) % 300000; self->m_nPlayTime = int32(H(I + 1) % 1000);
    nowMs = 100000ull + (I % 7) * 3; self->m_nLastUpdateTime = nowMs - static_cast<uint64>(std::array<uint32, 6>{ 0, 500, 999, 1000, 5000, 100000 }[I % 6]);
    cursorValue = std::array<uint32, 8>{ 0, 0x5FFFF, 0x60000, 0x7FFFF, 0xBFFFF, 0x123, 0x91234, 0xB0000 }[(I / 6) % 8];
    lockSize = 0x8000 + (I % 3) * 4; fxResult = (I % 9 == 0); eqFail = (I % 8 == 0) ? 2 : 0; /* solo falla la 2a iteracion: si falla la 1a, el EXE usa un local sin inicializar (basura de pila) y revienta */ createCount = 0; fillCalls = 0;
}

// ---------------- hilo de streaming real aparcado en IsForegroundApp (como AudioFinal1) ----------------
inline HANDLE parkedEvt{}, resumeEvt{}; inline DWORD streamTid{};
inline bool __cdecl FakeForeground() {
    if (streamTid && GetCurrentThreadId() == streamTid) { SetEvent(parkedEvt); WaitForSingleObject(resumeEvt, INFINITE); }
    return *reinterpret_cast<volatile uint32*>(0x8D621C) != 0;
}

// ---------------- datos de entrada deterministas ----------------
inline const std::array<int32, 40> audioEvents{ 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 119, 77, 53, 0, 120, 121, 200, 54, 55, 56, 57, 58, 59, 60, 74, 76, 119 };
inline const std::array<float, 8> volumes{ 0.f, -3.f, 5.5f, -12.25f, 2.f, -100.f, 0.0625f, 7.f };
inline const std::array<float, 6> speeds{ 1.f, 0.75f, 1.5f, 0.5f, 2.f, 1.25f };
inline const std::array<int, 7> slotsP{ 0x28, 0x13, 5, 0, 30, 41, 2 };
inline const std::array<int, 6> banksP{ 0x2C, 0x27, 0x80, 0, 143, 5 };

struct Outcome { json j; bool positive{}; };

inline uint64 Digest(size_t objSize) {
    uint64 h = Hash(obj.data(), objSize); h = h * 1099511628211ull ^ Hash(phys.data(), phys.size()); h = h * 1099511628211ull ^ Hash(fakePed.data(), fakePed.size());
    h = h * 1099511628211ull ^ Hash(snd.data(), snd.size()); h = h * 1099511628211ull ^ Hash(destination.data(), destination.size());
    h = h * 1099511628211ull ^ Hash(reinterpret_cast<void*>(0xB61CF8), 0x60); return h;
}
inline json Collect(size_t objSize, std::initializer_list<std::pair<size_t, size_t>> masks, int64 ret) {
    std::vector<uint8> o(obj.begin(), obj.begin() + objSize); for (auto [a, n] : masks) std::memset(o.data() + a, 0, n);
    return json{ { "switch", true }, { "ret", ret }, { "obj", Hash(o.data(), o.size()) }, { "phys", Hash(phys.data(), phys.size()) }, { "ped", Hash(fakePed.data(), fakePed.size()) },
                 { "snd", Hash(snd.data(), snd.size()) }, { "dest", Hash(destination.data(), destination.size()) },
                 { "scanner", Hash(reinterpret_cast<void*>(0xB61CF8), 0x60) }, { "trace", trace } };
}

// Ejecuta UN caso (fixture + llamada). 'address' es la entrada (EXE o vtable) ya validada. Devuelve json comparable.
inline json Exec(const std::string& full, uint32 I, uintptr_t address, size_t& objSizeOut) {
    int64 ret = 0; size_t sz = 0x21C; std::vector<std::pair<size_t, size_t>> masks;
    auto is = [&](const char* n) { return full == n; };
    void* self = obj.data();
    const unsigned fpSaved = _controlfp(0, 0);
    auto X87 = [&] { _controlfp(_PC_24, _MCW_PC); };
    auto X87off = [&] { _controlfp(fpSaved, _MCW_PC | _MCW_RC); };
    const float vol = volumes[(I / 3) % volumes.size()], spd = speeds[(I / 5) % speeds.size()];
    // ---------- CAEScriptAudioEntity ----------
    if (full.rfind("CAEScriptAudioEntity::", 0) == 0) {
        SetupScript(I); sz = 0x21C; curSz = sz;
        if (is("CAEScriptAudioEntity::Constructor")) { masks = { {0, 0x7C}, {0x114, 0x7C}, {0x19C, 0x7C} }; X87(); Th(address, self); X87off(); ret = (At<uint32>(self, 0) != 0xCDCDCDCDu) + 2 * (At<uint32>(self, 0x114) != 0xCDCDCDCDu) + 4 * (At<uint32>(self, 0x19C) != 0xCDCDCDCDu); }
        else if (is("CAEScriptAudioEntity::Initialise")) Th(address, self);
        else if (is("CAEScriptAudioEntity::Reset")) Th(address, self);
        else if (is("CAEScriptAudioEntity::GetMissionAudioEvent")) ret = Th<int32>(address, self, uint32(I % 4));
        else if (is("CAEScriptAudioEntity::Service")) Th(address, self);
        else if (is("CAEScriptAudioEntity::ReportMissionAudioEvent-1")) Th(address, self, int32(audioEvents[I % audioEvents.size()]), (void*)((I & 1) ? phys.data() : nullptr), vol, spd);
        else if (is("CAEScriptAudioEntity::ReportMissionAudioEvent-2")) { CVector p{ vol, spd, float(I % 5) - 2.f }; Th(address, self, int32(audioEvents[I % audioEvents.size()]), &p); }
        else if (is("CAEScriptAudioEntity::PlayResidentSoundEvent")) {
            static const std::array<int, 6> events{ 0x3FB, 104, 1086, 77, 5, 1021 };
            CVector p;
            switch ((I / 7) % 5) { case 0: p = CVector{ -1000.f, -1000.f, -1000.f }; break; case 1: p = CVector{ 0.f, 0.f, 0.f }; break; case 2: p = CVector{ -1000.f, 0.f, 0.f }; break;
                                   case 3: p = CVector{ 0.f, 0.f, -1000.f }; break; default: p = CVector{ float(I % 90) - 45.f, 3.5f, -2.f }; }
            X87();
            Th(address, self, int32(slotsP[I % slotsP.size()]), int32(banksP[(I / 3) % banksP.size()]), int32(I % 31), int32(events[(I / 11) % events.size()]), &p,
               (void*)(((I / 5) % 2) ? phys.data() : nullptr), vol, spd, int32(std::array<int, 4>{ 0, 50, -1, 300 }[(I / 13) % 4]), std::array<float, 3>{ 2.5f, 1.f, 4.f }[(I / 17) % 3]);
            X87off();
        }
    }
    // ---------- CAEPoliceScannerAudioEntity ----------
    else if (full.rfind("CAEPoliceScannerAudioEntity::", 0) == 0) {
        SetupScanner(I); sz = 0x7C; curSz = sz; preDigest = Digest(sz);
        using S = CAEPoliceScannerAudioEntity;
        tScannerSlot a[5], b[5];
        for (int s = 0; s < 5; ++s) { a[s] = { static_cast<eSoundBank>(I % 6 == 0 ? -1 : 100 + s), static_cast<eSoundID>(I % 10 == 0 ? -1 : s) }; b[s] = { static_cast<eSoundBank>(I % 4 == 0 ? -1 : 200 + s), static_cast<eSoundID>(I % 12 == 0 ? -1 : s + 8) }; }
        if (is("CAEPoliceScannerAudioEntity::Constructor")) { masks = { {0, 0x7C} }; Th(address, self); ret = At<uint32>(self, 0) != 0xCDCDCDCDu; }
        else if (is("CAEPoliceScannerAudioEntity::Destructor")) { masks = { {0, 4} }; Th(address, self); }
        else if (is("CAEPoliceScannerAudioEntity::StaticInitialise") || is("CAEPoliceScannerAudioEntity::Reset") || is("CAEPoliceScannerAudioEntity::EnableScanner") ||
                 is("CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue") || is("CAEPoliceScannerAudioEntity::PrepSlots")) reinterpret_cast<void(__cdecl*)()>(address)();
        else if (is("CAEPoliceScannerAudioEntity::DisableScanner")) reinterpret_cast<void(__cdecl*)(int32, int32)>(address)(int32(I % 2), int32((I / 2) % 2));
        else if (is("CAEPoliceScannerAudioEntity::StopScanner")) reinterpret_cast<void(__cdecl*)(int32)>(address)(int32(I % 2));
        else if (is("CAEPoliceScannerAudioEntity::PopulateScannerDialogueLists")) reinterpret_cast<void(__cdecl*)(tScannerSlot*, tScannerSlot*)>(address)(a, b);
        else if (is("CAEPoliceScannerAudioEntity::CanWePlayNewScannerDialogue")) ret = reinterpret_cast<bool(__cdecl*)()>(address)();
        else if (is("CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue")) { S::s_nScannerPlaybackState = static_cast<S::State>((I % 3 == 0) ? 0 : (I % 8)); Th(address, self, (void*)a, (void*)b); }
        else if (is("CAEPoliceScannerAudioEntity::UpdateParameters")) {
            auto* s0 = Snd(0); At<int16>(s0, 0) = (I % 5 == 0) ? 37 : 12; At<int16>(s0, 0x70) = int16(I % 4 == 0 ? 0 : 100 + (I % 9) * 40); At<uint16>(s0, 0x56) = 4;
            const int32 pos = (I % 6 == 0) ? -1 : int32((I % 11) * 20 + (I % 3) * 40);
            Th(address, self, (void*)s0, pos);
        }
    }
    // ---------- CAEPedAudioEntity ----------
    else if (full.rfind("CAEPedAudioEntity::", 0) == 0) {
        SetupPed(I); sz = 0x15C; curSz = sz;
        auto pedAlways = [&] { At<uintptr_t>(self, 0x94) = uintptr_t(fakePed.data()); };
        const int32 ev = audioEvents[I % audioEvents.size()];
        if (is("CAEPedAudioEntity::Initialise")) Th(address, self, (void*)fakePed.data());
        else if (is("CAEPedAudioEntity::StaticInitialise")) reinterpret_cast<void(__cdecl*)()>(address)();
        else if (is("CAEPedAudioEntity::Terminate")) Th(address, self);
        else if (is("CAEPedAudioEntity::AddAudioEvent")) {
            const void* ph = (ev == 119 || I % 3 != 0) ? (const void*)phys.data() : nullptr;
            X87();   // AE_PED_CRUNCH (0x4E2CDA..0x4E2CFB) suma con fild/fadd/fstp: a precision extendida el doble redondeo difiere del float de la fuente
            Th(address, self, ev, vol, spd, (void*)ph, int32(I % 40), int32(I % 5), uint32(I % 70));
            X87off();
        }
        else if (is("CAEPedAudioEntity::TurnOnJetPack") || is("CAEPedAudioEntity::TurnOffJetPack") || is("CAEPedAudioEntity::StopJetPackSound")) Th(address, self);
        else if (is("CAEPedAudioEntity::UpdateJetPack")) {
            static const std::array<float, 8> thrust{ 0.f, 0.25f, 0.5f, 0.5000001f, 0.75f, 1.f, -1.f, 0.4999999f };
            const float angle = float(int32(H(I * 31 + 7) % 2000) - 1000) * 0.01f;
            X87(); Th(address, self, thrust[I % thrust.size()], angle); X87off();
        }
        else if (is("CAEPedAudioEntity::PlayWindRush")) Th(address, self, 1.5f, 2.5f);
        else if (is("CAEPedAudioEntity::HandleFootstepEvent")) { pedAlways(); X87(); Th(address, self, int32(54 + I % 2), vol, spd, int32(H(I * 11) % 180)); X87off(); }
        else if (is("CAEPedAudioEntity::HandleSkateEvent")) { pedAlways(); X87(); Th(address, self, int32(56 + I % 2), vol, spd); X87off(); }
        else if (is("CAEPedAudioEntity::HandleLandingEvent")) { pedAlways(); X87(); Th(address, self, int32(58 + I % 2)); X87off(); }
        else if (is("CAEPedAudioEntity::HandleSwimSplash")) { pedAlways(); X87(); Th(address, self, int32(74 + I % 2)); X87off(); }
        else if (is("CAEPedAudioEntity::HandleSwimWake")) { pedAlways(); X87(); Th(address, self, int32(76)); X87off(); }
        else if (is("CAEPedAudioEntity::PlayShirtFlap")) { pedAlways(); Th(address, self, vol, spd); }
    }
    // ---------- CAEStreamingChannel ----------
    else {
        sz = OBJ_MAX; curSz = sz;
        const bool zero = is("CAEStreamingChannel::Destructor");
        SetupStream(I, zero);
        auto* ch = reinterpret_cast<CAEStreamingChannel*>(obj.data());
        auto needBuffer = [&] { ch->m_pDirectSoundBuffer = reinterpret_cast<IDirectSoundBuffer*>(&comObject); };
        if (is("CAEStreamingChannel::AddFX")) { needBuffer(); ret = Th<bool>(address, self); }
        else if (is("CAEStreamingChannel::RemoveFX")) { needBuffer(); Th(address, self); }
        else if (is("CAEStreamingChannel::SetBassEQ")) { needBuffer(); X87(); Th(address, self, int32(I % 3), std::array<float, 5>{ 1.f, 0.5f, 2.f, -1.f, 0.25f }[(I / 3) % 5]); X87off(); }
        else if (is("CAEStreamingChannel::Initialise")) { Fill(obj.data(), obj.size(), 0xCD); *reinterpret_cast<uintptr_t*>(obj.data()) = 0x85F3F0; ch->m_pDirectSound = reinterpret_cast<IDirectSound*>(&devObject);
                                                                 ret = Th<bool>(address, self); masks = { {0, 4} }; }
        else if (is("CAEStreamingChannel::InitialiseSilence")) { lockSize = 0x8000 + (I % 3) * 4; createCount = 1; Th(address, self); }
        else if (is("CAEStreamingChannel::Destructor")) { masks = { {0, 4} }; ch->m_pDirectSoundBuffer = (I % 13 == 0) ? nullptr : reinterpret_cast<IDirectSoundBuffer*>(&comObject); Th(address, self); }
        else if (is("CAEStreamingChannel::Pause")) Th(address, self);
        else if (is("CAEStreamingChannel::Stop-class")) { needBuffer(); Th(address, self, int32(I % 2)); }
        else if (is("CAEStreamingChannel::Stop-virtual")) { needBuffer(); Th(address, self); }
        else if (is("CAEStreamingChannel::SetReady")) { needBuffer(); Th(address, self); }
        else if (is("CAEStreamingChannel::SetNextStream")) Th(address, self, (void*)((I % 3 == 0) ? nullptr : (void*)decB));
        else if (is("CAEStreamingChannel::GetActiveTrackID") || is("CAEStreamingChannel::GetPlayingTrackID")) ret = Th<int32>(address, self);
        else if (is("CAEStreamingChannel::FillBuffer")) { ch->m_pStreamingDecoder = reinterpret_cast<CAEStreamingDecoder*>(decA); ch->m_nState = static_cast<StreamingChannelState>(-7 + int(I % 7));
                                                                 ret = Th<uint32>(address, self, (void*)destination.data(), uint32(0x1000 + (I % 5) * 0x400)); }
        else if (is("CAEStreamingChannel::UpdatePlayTime")) { needBuffer(); X87(); ret = Th<int32>(address, self); X87off(); }
        else if (is("CAEStreamingChannel::IsSoundPlaying")) ret = Th<bool>(address, self);
        else if (is("CAEStreamingChannel::Play")) Th(address, self, int32(std::array<int, 3>{ 0, 100, -1 }[I % 3]), int32(I % 4), spd);
        else if (is("CAEStreamingChannel::SetFrequencyScalingFactor")) { X87(); Th(address, self, std::array<float, 6>{ 0.f, 0.5f, 1.f, 2.f, 0.75f, 1.5f }[(I / 2) % 6]); X87off(); }
    }
    objSizeOut = sz; lastChanged = Digest(sz) != preDigest;
    auto j = Collect(sz, { }, ret);
    if (!masks.empty()) { std::vector<uint8> o(obj.begin(), obj.begin() + sz); for (auto [a, n] : masks) std::memset(o.data() + a, 0, n); j["obj"] = Hash(o.data(), o.size()); }
    return j;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "audiowave2-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "audiowave2-runtime.jsonl"); Write({ { "event", "waiting_for_player" } }); ReversingProbe::InstallFailureCapture("audiowave2"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "audiowave2-only.txt"); if (f) std::getline(f, only); }

    InitRegs(); BuildCom(); BuildSites();
    // ---- hooks ----
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(fns.size()); std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t n = 0; n < fns.size(); ++n) {
        hooks[n] = FindHook(fns[n].cls, fns[n].name);
        if (!hooks[n]) { Write({ { "event", "error" }, { "reason", "hook not found" }, { "function", std::string(fns[n].cls) + "::" + fns[n].name } }); return; }
    }
    for (size_t n = 0; n < fns.size(); ++n) { enabled[n] = hooks[n]->Hooked(); locked[n] = hooks[n]->Locked(); hooks[n]->LockState(false); }
    // ---- sitios (cuerpos de la DLL ya calculados; ninguno cae dentro del EXE) ----
    bool known = true; json siteInfo = json::array();
    for (auto& s : sites) { const bool ok = s.exe >= 0x401000 && s.exe < 0x1000000 && s.dll >= 0x1000000 && s.body >= 0x1000000; known &= ok; siteInfo.push_back({ { "name", s.name }, { "exe", s.exe }, { "dll", s.dll }, { "body", s.body }, { "ok", ok } }); }
    const uintptr_t dllFg = std::bit_cast<uintptr_t>(&IsForegroundApp), dllFgBody = Body(dllFg);
    const uintptr_t coAddr = G<uintptr_t>(0x858350);     // CoInitialize importada por el EXE (IAT)
    known &= (dllFgBody >= 0x1000000) && coAddr > 0x10000;
    // ---- estado global que se restaura ----
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> savedG;
    for (auto [p, n] : std::array<std::pair<uintptr_t, size_t>, 5>{{ { 0xB61CF8, 0x60 }, { 0xB7CB84, 4 }, { 0xB6F065, 1 }, { 0xBD00F8, 4 }, { 0xB61D54, 4 } }}) savedG.push_back({ p, ReversingProbe::Bytes(reinterpret_cast<void*>(p), n) });
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& s : sites) for (auto a : { s.exe, s.dll, s.body }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto a : { uintptr_t(0x746060), dllFg, dllFgBody, coAddr }) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& f : fns) if (f.address) entryBefore.push_back({ f.address, ReversingProbe::Bytes(reinterpret_cast<void*>(f.address), 8) });
    const unsigned savedFP = _controlfp(0, 0);
    int8* const savedVolumes = G<int8*>(0xBD00F8);
    static std::array<int8, 45401> fakeVolumes{}; for (size_t k = 0; k < fakeVolumes.size(); ++k) fakeVolumes[k] = int8(int32(H(uint32(k)) % 40) - 30);
    parkedEvt = CreateEventA(nullptr, FALSE, FALSE, nullptr); resumeEvt = CreateEventA(nullptr, TRUE, FALSE, nullptr);
    const HANDLE streamThread = AEAudioHardware.m_pStreamThread.m_nHandle;
    const bool streamRunning = streamThread && WaitForSingleObject(streamThread, 0) == WAIT_TIMEOUT;
    streamTid = streamRunning ? GetThreadId(streamThread) : 0;
    Write({ { "event", "begin" }, { "sites", siteInfo }, { "known", known }, { "fp_control", savedFP }, { "stream_running", streamRunning }, { "co_initialize", coAddr } });

    uint32 pairs = 0, totalFailures = 0; bool parkedOk = !streamRunning;
    if (known) {
        using ReversingProbe::EntryPatch;
        EntryPatch fg1(0x746060, FakeForeground), fg2(dllFgBody, FakeForeground);
        if (streamRunning) parkedOk = WaitForSingleObject(parkedEvt, 3000) == WAIT_OBJECT_0;
        if (!parkedOk) { SetEvent(resumeEvt); Write({ { "event", "error" }, { "reason", "audio stream thread not parked" } }); }
        else {
            Patches permanent; for (int i = 0; i < S_PERMANENT_END; ++i) permanent.Add(i);
            G<int8*>(0xBD00F8) = fakeVolumes.data();
            for (size_t n = 0; n < fns.size(); ++n) {
                const auto& fn = fns[n]; const std::string full = std::string(fn.cls) + "::" + fn.name;
                if (!only.empty() && only != full) continue;
                uint32 failures = 0, positiveCases = 0;
                Patches dynamic; for (int i : Dyn(full)) dynamic.Add(i);
                std::unique_ptr<EntryPatch> coPatch; if (full == "CAEStreamingChannel::Initialise") coPatch = std::make_unique<EntryPatch>(coAddr, reinterpret_cast<void*>(&CoInitStub));
                for (int c = 0; c < fn.cases; ++c) {
                    const uint32 I = uint32(c); size_t sz = 0;
                    auto run = [&](bool candidate) -> json {
                        sc = I; seq = 0; positive = 0; trace = json::array(); rawSnd.clear(); preDigest = 0; lastChanged = false;
                        hooks[n]->State(candidate);
                        uintptr_t address = fn.address;
                        if (fn.vt) address = reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];
                        const bool switches = fn.vt ? ((address < 0x400000 || address >= 0x1000000) == candidate) : (ReversingProbe::JumpsToDll(fn.address) == candidate);   // despues de State()
                        if (!switches) return json{ { "switch", false } };
                        return Exec(full, I, address, sz);
                    };
                    const json original = run(false); const std::string rawOrig = rawSnd; const bool origChanged = lastChanged; const uint32 posOrig = positive;
                    const json candidate = run(true); const std::string rawCand = rawSnd;
                    const bool sw = original.value("switch", false) && candidate.value("switch", false);
                    const bool equal = sw && original == candidate;
                    if (!equal) ++failures;
                    // Gate positivo: la pasada original hizo trabajo observable (llamadas a dobles, retorno distinto de 0 o bytes cambiados)
                    if (sw && (posOrig > 0 || origChanged || original["ret"] != 0 || fn.address == 0x4E1170)) ++positiveCases;
                    if (equal) Write({ { "event", "comparison" }, { "pair", pairs++ }, { "function", full }, { "case", c }, { "equal", true }, { "accepted", true }, { "original", original }, { "candidate", candidate } });
                    else Write({ { "event", "comparison" }, { "pair", pairs++ }, { "function", full }, { "case", c }, { "equal", false }, { "accepted", false },
                                 { "original", failures <= 3 ? original : json(nullptr) }, { "candidate", failures <= 3 ? candidate : json(nullptr) },
                                 { "raw_original", failures <= 3 ? rawOrig : std::string() }, { "raw_candidate", failures <= 3 ? rawCand : std::string() } });
                }
                if (!positiveCases) ++failures;
                totalFailures += failures;
                Write({ { "event", "function_summary" }, { "function", full }, { "failures", failures }, { "cases", fn.cases }, { "positive_cases", positiveCases } });
            }
            G<int8*>(0xBD00F8) = savedVolumes;
            SetEvent(resumeEvt);
        }
    }
    for (size_t n = 0; n < fns.size(); ++n) { hooks[n]->State(enabled[n]); hooks[n]->LockState(locked[n]); }
    for (auto& [p, b] : savedG) std::memcpy(reinterpret_cast<void*>(p), b.data(), b.size());
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (auto& [p, b] : savedG) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(p), b.size());
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    for (size_t n = 0; n < fns.size(); ++n) restored &= hooks[n]->Hooked() == enabled[n] && hooks[n]->Locked() == locked[n];
    Write({ { "event", (known && parkedOk && !totalFailures) ? "complete" : "error" }, { "reason", !known ? "sites not resolved" : !parkedOk ? "stream thread not parked" : totalFailures ? "comparison failures" : "" }, { "pairs", pairs }, { "failures", totalFailures }, { "state_restored", restored } });
}
}
