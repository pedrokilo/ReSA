#pragma once
// Fixtures y colaboradores falsos del banco Dependencies24B (diPadInit 0x7485C0). Solo C++ estandar y ensamblador en linea de MSVC (sin tipos del juego):
// lo incluyen LiveComparison.hpp (dentro del juego) y offline/main.cpp (bytes del EXE mapeados en un proceso hijo); fixture_oracle.py repite
// exactamente las formulas de Mix/Rand/Make y modela el resultado esperado.
//
// Colaboradores falsos (los mismos para el EXE y para la DLL):
//  - 0x7477C0 (_InputAddJoys, funcion del EXE sin reconstruir que la DLL llama por direccion): StubEnum. Asigna diDevice1/diDevice2 segun dev_mode.
//  - 0x530B00 (CControllerConfigManager::InitDefaultControlConfigJoyPad, thiscall, ret 4): StubInit. EntryPatch en la direccion del EXE y en el cuerpo de la DLL.
//  - IDirectInputDevice8: dos objetos falsos (A = id 1, B = id 2) con vtable propia; GetCapabilities (+0xC) y GetProperty (+0x14) trazan y devuelven el HRESULT
//    del caso; cualquier otra ranura es una trampa que se cuenta. Ningun dispositivo real se abre, se libera ni se toca.
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>
#include <windows.h>

namespace Dependencies24BFix {
using u32 = std::uint32_t;
using u8 = std::uint8_t;
constexpr unsigned kCases = 2048;
constexpr u32 kEntry = 0x7485C0, kPsPtr = 0xC17054, kTable = 0xC92144, kEnumExe = 0x7477C0, kInitExe = 0x530B00, kControls = 0xB70198;
constexpr u32 kTableSize = 0x20, kPsDev1 = 0x20, kPsDev2 = 0x24, kStackSlots = 64;
constexpr u32 kGolden = 0x9E3779B1u;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }

// HRESULT (0 = S_OK, 1 = S_FALSE, E_FAIL, E_NOTIMPL, E_OUTOFMEMORY, E_INVALIDARG, DIERR_NOTACQUIRED, DIERR_INPUTLOST, DIERR_OBJECTNOTFOUND, positivo grande, -1, minimo)
inline constexpr std::array<u32, 12> kHr{0x00000000u, 0x00000001u, 0x80004005u, 0x80004001u, 0x8007000Eu, 0x80070057u,
                                         0x8007000Cu, 0x8007001Eu, 0x80070002u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000000u};
// dwData de DIPROP_VIDPID (LOWORD = vendor, HIWORD = producto)
inline constexpr std::array<u32, 12> kVid{0u, 0xFFFFFFFFu, 0x00010000u, 0x0000FFFFu, 0x045E028Eu, 0x054C09CCu,
                                          0x80000000u, 1u, 0x12345678u, 0xDEADBEEFu, 0x0001FFFFu, 0xFFFF0000u};
inline constexpr std::array<u32, 8> kSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};

struct Fix {
    unsigned dev_mode;      // 0 (nulo, nulo), 1 (A, nulo), 2 (nulo, B), 3 (A, B), 4 (A, A), 5 (B, A): lo que deja 0x7477C0 en diDevice1/diDevice2
    unsigned mutator;       // 0 ninguno; 1 A.GetCapabilities: dev1=B; 2 A.GetProperty: dev2=nulo; 3 A.GetProperty: dev2=B; 4 A.GetProperty: dev1=nulo;
                            // 5 InitDefaultControlConfigJoyPad: dev2=nulo; 6 igual: dev2=A; 7 A.GetCapabilities: dev2=nulo
    unsigned table_kind;    // 0 ceros, 1 0xFF, 2 aleatoria, 3 configuracion plausible, 4 0xA5
    unsigned prop_mode[2];  // por objeto: 0 escribe dwData solo si SUCCEEDED; 1 siempre; 2 nunca; 3 si SUCCEEDED y ademas corrompe dwObj de la cabecera
    u32 enum_ret, caps_hr[2], prop_hr[2], vid[2], caps_fill, stack_seed, table_seed, eax, ecx, edx;
};
inline Fix Make(unsigned c) {
    Fix f{};
    f.dev_mode = c % 6;
    f.mutator = (c / 6) % 8;
    f.enum_ret = kHr[Rand(c, 1) % 12];
    for (unsigned i = 0; i < 2; ++i) {
        f.caps_hr[i] = kHr[Rand(c, 2 + i) % 12];
        f.prop_hr[i] = (Rand(c, 4 + i) % 3 == 0) ? 0u : kHr[Rand(c, 6 + i) % 12];
        f.vid[i] = (Rand(c, 8 + i) % 3 == 0) ? Rand(c, 10 + i) : kVid[Rand(c, 12 + i) % 12];
        const u32 r = Rand(c, 14 + i) % 10;
        f.prop_mode[i] = r < 7 ? 0u : r - 6u;
    }
    f.caps_fill = Rand(c, 16);
    f.stack_seed = c < kSeeds.size() ? kSeeds[c] : Rand(c, 17);
    f.table_kind = Rand(c, 18) % 5;
    f.table_seed = Rand(c, 19);
    f.eax = Rand(c, 20); f.ecx = Rand(c, 21); f.edx = Rand(c, 22);
    return f;
}
inline std::array<u8, kTableSize> BuildTable(const Fix& f) {
    std::array<u8, kTableSize> t{};
    switch (f.table_kind) {
    case 0: break;
    case 1: t.fill(0xFF); break;
    case 2: for (u32 i = 0; i < kTableSize; ++i) t[i] = u8(Rand(f.table_seed, i)); break;
    case 3:
        for (u32 p = 0; p < 2; ++p) {
            u8* e = t.data() + 16 * p;
            const u32 id = Rand(f.table_seed, 100 + p), v = Rand(f.table_seed, 102 + p) & 0xFFFF, pr = Rand(f.table_seed, 104 + p) & 0xFFFF;
            std::memcpy(e, &id, 4); e[4] = 1; e[5] = 1; e[6] = 1; e[7] = 0x5A; std::memcpy(e + 8, &v, 4); std::memcpy(e + 12, &pr, 4);
        }
        break;
    default: t.fill(0xA5); break;
    }
    return t;
}
// Valor que la semilla de pila deja en la ranura k (1 = la de la direccion de retorno, que la llamada pisa) a k*4 bytes bajo el ESP previo a la llamada.
constexpr u32 SlotValue(u32 seed, u32 k) { return seed ^ (k * kGolden); }
// Los cinco dwords del DIPROPDWORD del EXE (dwSize, dwHeaderSize, dwObj, dwHow, dwData) viven en [R-0x40, R-0x2C): ranuras 17, 16, 15, 14 y 13.
constexpr std::array<u32, 5> ExpectedResidue(u32 seed) { return {SlotValue(seed, 17), SlotValue(seed, 16), SlotValue(seed, 15), SlotValue(seed, 14), SlotValue(seed, 13)}; }

// ------------------------------------------------------------------ estado de la sonda
struct FakeDev { void** vtbl; u32 id; };
inline void* g_vtbl[40]{};
inline FakeDev g_devA{g_vtbl, 1}, g_devB{g_vtbl, 2};
inline const Fix* g_cur{};
inline std::vector<u32> g_trace;
inline unsigned g_traps{};

inline u32& PsField(u32 off) { return *reinterpret_cast<u32*>(*reinterpret_cast<u32*>(kPsPtr) + off); }
inline u32 PtrOf(FakeDev* d) { return reinterpret_cast<u32>(d); }
inline u32 IdOf(u32 p) { return p == 0 ? 0u : p == PtrOf(&g_devA) ? 1u : p == PtrOf(&g_devB) ? 2u : 0xFFFFFFFFu; }
inline u32 TableHash() {
    u32 h = 2166136261u;
    const auto* p = reinterpret_cast<const u8*>(kTable);
    for (u32 i = 0; i < kTableSize; ++i) { h ^= p[i]; h *= 16777619u; }
    return h;
}
inline u32 InStack(const void* p) {
    const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    return reinterpret_cast<const void*>(p) >= tib->StackLimit && reinterpret_cast<const void*>(p) < tib->StackBase ? 1u : 0u;
}
enum Event : unsigned { kEvCaps = 1, kEvProp = 2, kEvInit = 3 };
inline void Effect(unsigned who, Event ev) {
    const unsigned m = g_cur->mutator;
    if (who == 1 && ev == kEvCaps) { if (m == 1) PsField(kPsDev1) = PtrOf(&g_devB); if (m == 7) PsField(kPsDev2) = 0; }
    if (who == 1 && ev == kEvProp) { if (m == 2) PsField(kPsDev2) = 0; if (m == 3) PsField(kPsDev2) = PtrOf(&g_devB); if (m == 4) PsField(kPsDev1) = 0; }
    if (ev == kEvInit) { if (m == 5) PsField(kPsDev2) = 0; if (m == 6) PsField(kPsDev2) = PtrOf(&g_devA); }
}
// 0x7477C0: sin argumentos (cdecl). El EXE ignora el valor de retorno.
inline int __cdecl StubEnum() {
    g_trace.insert(g_trace.end(), {1u, TableHash()});
    static constexpr int map[6][2] = {{0, 0}, {1, 0}, {0, 2}, {1, 2}, {1, 1}, {2, 1}};
    const auto pick = [](int id) { return id == 0 ? 0u : id == 1 ? PtrOf(&g_devA) : PtrOf(&g_devB); };
    PsField(kPsDev1) = pick(map[g_cur->dev_mode][0]);
    PsField(kPsDev2) = pick(map[g_cur->dev_mode][1]);
    return int(g_cur->enum_ret);
}
// 0x530B00: thiscall (ECX = this), un argumento de pila, ret 4
inline void __fastcall StubInit(void* self, void*, u32 buttons) {
    g_trace.insert(g_trace.end(), {4u, reinterpret_cast<u32>(self), buttons, TableHash()});
    Effect(0, kEvInit);
}
// IDirectInputDevice8::GetCapabilities (+0xC). No se lee nada de caps salvo dwSize: el buffer del EXE es un resto de pila y el de la DLL un relleno 0xCC.
inline long __stdcall FakeGetCaps(FakeDev* self, u32* caps) {
    g_trace.insert(g_trace.end(), {2u, self->id, caps[0], InStack(caps), TableHash()});
    Effect(self->id, kEvCaps);
    const u32 hr = g_cur->caps_hr[self->id - 1];
    if (long(hr) >= 0) for (u32 i = 1; i < 11; ++i) caps[i] = Rand(g_cur->caps_fill, i);
    return long(hr);
}
// IDirectInputDevice8::GetProperty (+0x14): h apunta a un DIPROPDWORD (cinco dwords)
inline long __stdcall FakeGetProp(FakeDev* self, const void* guid, u32* h) {
    g_trace.insert(g_trace.end(), {3u, self->id, reinterpret_cast<u32>(guid), InStack(h), h[0], h[1], h[2], h[3], h[4], TableHash()});
    const u32 hr = g_cur->prop_hr[self->id - 1], vid = g_cur->vid[self->id - 1];
    const bool ok = long(hr) >= 0;
    switch (g_cur->prop_mode[self->id - 1]) {
    case 0: if (ok) h[4] = vid; break;
    case 1: h[4] = vid; break;
    case 2: break;
    default: if (ok) { h[4] = vid; h[2] = 0x77u; } break;
    }
    Effect(self->id, kEvProp);
    return long(hr);
}
inline long __stdcall FakeTrap() { ++g_traps; g_trace.push_back(5u); return 0; }
inline void InitVtable() {
    for (auto& slot : g_vtbl) slot = reinterpret_cast<void*>(&FakeTrap);
    g_vtbl[3] = reinterpret_cast<void*>(&FakeGetCaps);
    g_vtbl[5] = reinterpret_cast<void*>(&FakeGetProp);
}

// ------------------------------------------------------------------ puerta de la llamada (cdecl sin argumentos)
struct Reg { u32 eax, ecx, edx; bool stack, nonvolatile; };
// Antes de llamar rellena 64 dwords bajo el ESP con SlotValue(seed, k); EAX/ECX/EDX con semillas por caso. La ranura 1 es la de la direccion de retorno.
inline Reg GateCdecl0(u32 entry, u32 eaxSeed, u32 ecxSeed, u32 edxSeed, u32 seed) {
    u32 before{}, after{}, ra{}, rc{}, rd{}, b0{}, s0{}, i0{}, p0{}, b1{}, s1{}, i1{}, p1{};
    __asm {
        mov edx, seed
        xor esi, esi
        mov eax, esp
        mov ecx, 64
    L24seed:
        sub eax, 4
        add esi, 9E3779B1h
        mov edi, edx
        xor edi, esi
        mov [eax], edi
        dec ecx
        jnz L24seed
    }
    __asm { mov before, esp }
    __asm { mov b0, ebx } __asm { mov s0, esi } __asm { mov i0, edi } __asm { mov p0, ebp }
    __asm { mov eax, eaxSeed } __asm { mov ecx, ecxSeed } __asm { mov edx, edxSeed } __asm { call entry }
    __asm { mov ra, eax } __asm { mov rc, ecx } __asm { mov rd, edx } __asm { mov after, esp }
    __asm { mov b1, ebx } __asm { mov s1, esi } __asm { mov i1, edi } __asm { mov p1, ebp }
    return {ra, rc, rd, before == after, b0 == b1 && s0 == s1 && i0 == i1 && p0 == p1};
}

// ------------------------------------------------------------------ preparacion y captura
struct Saved { std::array<u8, kTableSize> table{}; u32 dev1{}, dev2{}; };
inline Saved SaveGlobals() {
    Saved s;
    std::memcpy(s.table.data(), reinterpret_cast<void*>(kTable), kTableSize);
    s.dev1 = PsField(kPsDev1); s.dev2 = PsField(kPsDev2);
    return s;
}
inline void RestoreGlobals(const Saved& s) {
    std::memcpy(reinterpret_cast<void*>(kTable), s.table.data(), kTableSize);
    PsField(kPsDev1) = s.dev1; PsField(kPsDev2) = s.dev2;
}
inline bool GlobalsEqual(const Saved& s) {
    return std::memcmp(reinterpret_cast<void*>(kTable), s.table.data(), kTableSize) == 0 && PsField(kPsDev1) == s.dev1 && PsField(kPsDev2) == s.dev2;
}
inline void Prepare(const Fix& f) {
    g_cur = &f; g_trace.clear(); g_traps = 0;
    const auto t = BuildTable(f);
    std::memcpy(reinterpret_cast<void*>(kTable), t.data(), kTableSize);
    PsField(kPsDev1) = PtrOf(&g_devB); PsField(kPsDev2) = PtrOf(&g_devA);   // restos previos: la enumeracion los sobrescribe siempre
}
struct Observed { std::array<u8, kTableSize> table{}; u32 dev1{}, dev2{}, traps{}; std::vector<u32> trace; };
inline Observed Observe() {
    Observed o;
    std::memcpy(o.table.data(), reinterpret_cast<void*>(kTable), kTableSize);
    o.dev1 = IdOf(PsField(kPsDev1)); o.dev2 = IdOf(PsField(kPsDev2)); o.traps = g_traps; o.trace = g_trace;
    return o;
}
}
