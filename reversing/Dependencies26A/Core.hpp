#pragma once
// Nucleo del banco Dependencies26A (CAEAudioHardware::PlaySound 0x4D86B0), compartido por LiveComparison.hpp (dentro del juego) y offline/main.cpp (ensayo sin juego).
// Exige que `json` (nlohmann), uint8/uint32/uintptr_t y <windows.h> ya esten declarados (StdInc.h del proyecto).
#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

namespace Dependencies26ALive {
using namespace Dependencies26AFix;

// Los diez bytes de cada registro x87 VACIO se comparan enteros (FNSAVE completo salvo FIP). No se relaja nunca.
inline constexpr bool kCompareDeadX87Registers = true;

inline std::string Hex(const void* a, size_t n) { constexpr char d[] = "0123456789abcdef"; const auto* b = static_cast<const uint8*>(a); std::string o; o.reserve(n * 2); for (size_t i = 0; i < n; ++i) { o += d[b[i] >> 4]; o += d[b[i] & 15]; } return o; }

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
inline uint32 FipOf(const Fp& f) { return W(f, 12); }

// ---- estado x87 inicial determinista: FCW 0x7F (24 bits, redondeo al par, excepciones enmascaradas), FSW del caso (TOP = 0), ocho residuos; MXCSR 0x1F80 ----
alignas(16) inline uint8 gPrime[108]{};
inline void MakePrime(uint32 fsw) {
    std::memset(gPrime, 0, 108); gPrime[0] = 0x7F; const uint16 s = uint16(fsw); std::memcpy(gPrime + 4, &s, 2);
    for (int i = 0; i < 8; ++i) { uint8* r = gPrime + 28 + 10 * i; for (int k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k); r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40; }
}

// ---- compuerta: PlaySound es thiscall (ECX = this) con siete palabras de pila y `ret 0x1C` (la pila la limpia el llamado); no devuelve nada ----
struct Gate { bool stack{}, nonvolatile{}; uint32 mxcsr{}, eax{}, ecx{}, edx{}; alignas(16) std::array<uint8, 108> img{}; };
inline Gate GateThis7(uintptr_t entry, void* self, const uint32* args, uint32 seedA, uint32 seedD) {
    Gate g{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, rvA{}, rvC{}, rvD{}, mx0 = 0x1F80, mx1{}; uint8* img = g.img.data(); const uint8* prime = gPrime;
    const uint32 a0 = args[0], a1 = args[1], a2 = args[2], a3 = args[3], a4 = args[4], a5 = args[5], a6 = args[6];
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
        push a6
        push a5
        push a4
        push a3
        push a2
        push a1
        push a0
        mov ecx, self
        mov edx, seedD
        mov eax, seedA
        call entry
        mov rvA, eax
        mov rvC, ecx
        mov rvD, edx
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
    g.stack = sp0 == sp1; g.nonvolatile = b0 == b1 && s0 == s1 && d0 == d1 && p0 == p1; g.mxcsr = mx1; g.eax = rvA; g.ecx = rvC; g.edx = rvD; return g;
}

// ---- hilo principal: solo sus llamadas a los falsos quedan en la traza ----
inline DWORD gMainThread{};
inline bool OnMain() { return GetCurrentThreadId() == gMainThread; }

// ---- objetos falsos ----
alignas(16) inline std::array<uint8, kArena> hw{}, hwPre{};
alignas(16) inline std::array<std::array<uint8, 0x90>, kChannelCount> chans{}, chansPre{};
alignas(16) inline std::array<uintptr_t, 12> chVt{};     // tabla de metodos virtuales falsa de CAEAudioChannel (9 huecos, el resto de relleno)
alignas(16) inline std::array<uint8, 0x40> bankObj{};    // CAEMP3BankLoader falso (solo se pasa como `this`)
inline Case G{};

// ---- traza de colaboradores: sucesos de 8 palabras [tipo, ...] ----
//  1 GetSoundBuffer [1, banco, sonido & FFFF, ranura & FFFF]       2 GetLoopOffset [2, banco, sonido, ranura]
//  3 SetAudioBuffer [3, canal, buffer, tamano, f88 & FFFF, f8c & FFFF, bucle & FFFF, tasa & FFFF]
//  4 GetLength [4, canal]   5 SetNotInRoom [5, canal, onStreet & FF]   6 Play [6, canal, posicion & FFFF, flags & FF, bits de la velocidad]   7 metodo virtual no previsto [7, canal, hueco]
using Ev = std::array<uint32, 8>;
inline std::array<Ev, 64> evs{}; inline size_t nev{};
inline void Push(uint32 t, uint32 a = 0, uint32 b = 0, uint32 c = 0, uint32 d = 0, uint32 e = 0, uint32 f = 0, uint32 g = 0) { if (!OnMain()) return; if (nev < evs.size()) evs[nev] = {t, a, b, c, d, e, f, g}; ++nev; }
inline json TraceJson() {
    json o = json::array(); const size_t n = std::min(nev, evs.size());
    for (size_t i = 0; i < n; ++i) o.push_back(json(evs[i]));
    if (nev > evs.size()) o.push_back(json::array({0xFFFFFFFFu, uint32(nev)}));
    return o;
}
inline uint32 ChId(const void* p) {
    const auto d = intptr_t(p) - intptr_t(chans.data());
    if (d >= 0 && d < intptr_t(kChannelCount * 0x90) && d % 0x90 == 0) return 0x100 + uint32(d / 0x90);
    if (p == bankObj.data()) return 0x20;
    return 0xEEEEEEEEu;
}

// ---- colaboradores no virtuales (EXE + DLL con el mismo falso). Las palabras sin signo se registran con su ancho real: el llamado solo lee 16 bits ----
inline uint8* __fastcall FGetSoundBuffer(void* self, void*, uint32 sfx, uint32 bank, uint32* outSize, uint16* outRate) {
    Push(1, ChId(self), sfx & 0xFFFF, bank & 0xFFFF);
    if (!G.buf) return nullptr;                         // como el EXE: con nulo no escribe las salidas
    *outSize = G.size; *outRate = uint16(G.rate);
    return reinterpret_cast<uint8*>(uintptr_t(G.buf));
}
inline int32 __fastcall FGetLoopOffset(void* self, void*, uint32 sfx, uint32 bank) { Push(2, ChId(self), sfx & 0xFFFF, bank & 0xFFFF); return int32(G.loop); }
inline bool __fastcall FSetAudioBuffer(void* self, void*, const void* buf, uint32 size, uint32 f88, uint32 f8c, uint32 loop, uint32 freq) {
    Push(3, ChId(self), uint32(uintptr_t(buf)), size, f88 & 0xFFFF, f8c & 0xFFFF, loop & 0xFFFF, freq & 0xFFFF); return G.setret != 0;
}
inline void __fastcall FSetNotInRoom(void* self, void*, uint32 onStreet) { Push(5, ChId(self), onStreet & 0xFF); }
// ---- metodos virtuales del canal ----
inline int32 __fastcall ChLength(void* s, void*) { Push(4, ChId(s)); return int32(G.length); }
inline void __fastcall ChPlay(void* s, void*, uint32 pos, uint32 flags, uint32 speed) { Push(6, ChId(s), pos & 0xFFFF, flags & 0xFF, speed); }
inline void* __fastcall ChTrapDtor(void* s, void*, uint32) { Push(7, ChId(s), 0); return s; }
inline void __fastcall ChTrap1(void* s, void*) { Push(7, ChId(s), 1); }
inline bool __fastcall ChTrap2(void* s, void*) { Push(7, ChId(s), 2); return false; }
inline int32 __fastcall ChTrap3(void* s, void*) { Push(7, ChId(s), 3); return 0; }
inline void __fastcall ChTrap6(void* s, void*) { Push(7, ChId(s), 6); }
inline void __fastcall ChTrap7(void* s, void*) { Push(7, ChId(s), 7); }
inline void __fastcall ChTrap8(void* s, void*, uint32) { Push(7, ChId(s), 8); }
inline void InitFakes() {
    for (size_t k = 0; k < chVt.size(); ++k) chVt[k] = 0xDEAD0000u + uint32(k);
    chVt[0] = uintptr_t(&ChTrapDtor); chVt[1] = uintptr_t(&ChTrap1); chVt[2] = uintptr_t(&ChTrap2); chVt[3] = uintptr_t(&ChTrap3); chVt[4] = uintptr_t(&ChLength);
    chVt[5] = uintptr_t(&ChPlay); chVt[6] = uintptr_t(&ChTrap6); chVt[7] = uintptr_t(&ChTrap7); chVt[8] = uintptr_t(&ChTrap8);
}

// ---- estado del caso ----
inline uint32& At32(uint8* base, size_t off) { return *reinterpret_cast<uint32*>(base + off); }
inline uint16& At16(uint8* base, size_t off) { return *reinterpret_cast<uint16*>(base + off); }
inline void FillRandom(uint8* p, size_t n, uint32 seed) {
    uint32 s = seed ? seed : 1;
    for (size_t k = 0; k + 4 <= n; k += 4) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; std::memcpy(p + k, &s, 4); }
}
// Deja el CAEAudioHardware falso y los 65 canales falsos listos para el caso. Las palabras que PlaySound escribe si llega al final se rellenan con el complemento de lo que escribira
// (todos sus bytes cambian), de modo que el cambio es visible aunque el valor nuevo coincida con el relleno aleatorio.
inline void Apply(const Case& c) {
    G = c;
    FillRandom(hw.data(), hw.size(), c.fill);
    for (size_t k = 0; k < chans.size(); ++k) FillRandom(chans[k].data(), chans[k].size(), c.fill * 2654435761u + uint32(k) * 40503u + 1u);
    const int16 chan = int16(c.args[0] & 0xFFFF);
    const uint32 slot = c.args[1] & 0xFFFF, flags = c.args[5] & 0xFFFF;
    if (chan >= 0) {
        const uint32 sum32 = uint32(chan) + slot;
        const size_t flagsAt = kFlagsAt + 2u * sum32;
        if (flagsAt + 2 <= hw.size()) At16(hw.data(), flagsAt) = uint16(~flags);
        const uint32 idx16 = (slot + uint32(chan)) & 0xFFFF;
        if (idx16 < chans.size()) At32(chans[idx16].data(), kChannelFlagsAt) = ~Sext16(flags);
    }
    for (size_t k = 0; k < chans.size(); ++k) { At32(chans[k].data(), 0) = uint32(uintptr_t(chVt.data())); At32(hw.data(), kChannelTableAt + 4 * k) = uint32(uintptr_t(chans[k].data())); }
    At32(hw.data(), kBankLoaderAt) = uint32(uintptr_t(bankObj.data()));
    hw[kN3dAt] = uint8(c.n3d);
    if (chan >= 0) At16(hw.data(), kNumChannelsInSlotAt + 2u * uint32(chan)) = uint16(c.nslot);
    hwPre = hw; chansPre = chans;
}
inline json DiffJson(const uint8* pre, const uint8* post, size_t n) {
    json o = json::array();
    for (size_t i = 0; i < n;) {
        if (pre[i] == post[i]) { ++i; continue; }
        size_t j = i; while (j < n && pre[j] != post[j]) ++j;
        o.push_back(json::array({i, Hex(post + i, j - i)})); i = j;
    }
    return o;
}
inline json ChanDiffJson() {
    json o = json::array();
    for (size_t k = 0; k < chans.size(); ++k) { const json d = DiffJson(chansPre[k].data(), chans[k].data(), 0x90); if (!d.empty()) o.push_back(json::array({k, d})); }
    return o;
}
inline json InputJson(const Case& c) {
    return {{"case", c.n}, {"family", c.family}, {"args", std::vector<uint32>(c.args, c.args + 7)}, {"nslot", c.nslot}, {"buf", c.buf}, {"size", c.size}, {"rate", c.rate}, {"loop", c.loop},
            {"setret", c.setret}, {"length", c.length}, {"n3d", c.n3d}, {"fsw", c.fsw}};
}
// Resultado de un lado: todo lo que debe coincidir (el contenido de `original` y `candidate` ha de ser identico)
inline json SideJson(const Case& c, const Gate& g) {
    Fp fp = g.img; memset(fp.data() + 12, 0, 4);   // FIP: cada lado ejecuta su ultima instruccion x87 en otra direccion (se valida el modulo aparte)
    return {{"input", InputJson(c)}, {"trace", TraceJson()}, {"arena_diff", DiffJson(hwPre.data(), hw.data(), hw.size())}, {"chan_diff", ChanDiffJson()},
            {"post_floating_state", Hex(fp.data(), 108)}, {"post_mxcsr", g.mxcsr}, {"stack_restored", g.stack}, {"nonvolatile_preserved", g.nonvolatile}};
}
}
