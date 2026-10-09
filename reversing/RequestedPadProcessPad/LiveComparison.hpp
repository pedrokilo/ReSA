#pragma once
// Pairwise bank for CPad::ProcessPad (EXE 0x00746A10, cdecl, ret). PREPARED ONLY: not compiled, not executed.
// Both legs call the same address 0x746A10; the original leg runs with the hook off (native EXE code), the candidate leg
// with the hook on. DirectInput is a private fake COM object (Poll/Acquire/GetDeviceState, stdcall); RsPadEventHandler
// is traced; GetPad (0x53FB70, pure) and FTOL (0x821B40) remain native CPU code on the real global CPad array.
#include "Pad.h"
#include "ControllerConfigManager.h"
#include "MenuManager.h"
#include <bit>
#include <float.h>
#include <xmmintrin.h>
#include <utility>
#include "Fixtures.hpp"
#include "../ProbeRuntime.hpp"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>

namespace RequestedPadProcessPad {
using Simple = ReversibleHooks::ReversibleHook::Simple;
inline constexpr unsigned fpMask = _MCW_PC | _MCW_RC | _MCW_EM | _MCW_IC | _MCW_DN;

// ---- layout assumptions (compile-time) ------------------------------------------------------------------------
static_assert(sizeof(DIJOYSTATE2) == 272 && sizeof(CPad) == 308 && sizeof(psGlobalType) == 0x28);
static_assert(offsetof(psGlobalType, diDevice1) == 0x20 && offsetof(psGlobalType, diDevice2) == 0x24);
static_assert(offsetof(CPad, PCTempJoyState) == 0xA8 && sizeof(CControllerState) >= 8);
static_assert(offsetof(CControllerState, LeftStickX) == 0 && offsetof(CControllerState, LeftStickY) == 2 && offsetof(CControllerState, RightStickX) == 4 && offsetof(CControllerState, RightStickY) == 6);
static_assert(sizeof(JoyStruct) == 0x10 && offsetof(JoyStruct, bZAxisPresent) == 5 && offsetof(JoyStruct, bZRotPresent) == 6);
static_assert(offsetof(CControllerConfigManager, m_bJoyJustInitialised) == 0 && offsetof(CControllerConfigManager, m_OldJoyState) == 4 && offsetof(CControllerConfigManager, m_NewJoyState) == 0x114);
static_assert(offsetof(CMenuManager, m_bInvertPadX1) == 0xB1 && offsetof(CMenuManager, m_bSwapPadAxis2) == 0xB6);

// ---- memory map ---------------------------------------------------------------------------------------------------
struct Region { uintptr_t address; size_t size; };
inline constexpr Region kControls{0xB70198, 548}, kControlsBefore{0xB70188, 16}, kControlsAfter{0xB703BC, 32};
inline constexpr Region kSticks{0xC8CFB0, 16}, kSticksBefore{0xC8CFA0, 16}, kSticksAfter{0xC8CFC0, 16};
inline constexpr Region kPads{0xB73458, 616}, kJoy{0xC92144, 32}, kJoyBefore{0xC92134, 16}, kJoyAfter{0xC92164, 16};
inline constexpr Region kMenu{0xBA67F0, 24}, kPadNumber{0xB73400, 1}, kPsPointer{0xC17054, 4};
inline constexpr std::array<Region, 13> kSaved{kControls, kControlsBefore, kControlsAfter, kSticks, kSticksBefore, kSticksAfter, kPads, kJoy, kJoyBefore, kJoyAfter, kMenu, kPadNumber, kPsPointer};
template<class T> inline T& Mem(uintptr_t address) { return *reinterpret_cast<T*>(address); }
inline std::vector<uint8> Snap(const Region& r) { return ReversingProbe::Bytes(reinterpret_cast<void*>(r.address), r.size); }
inline std::string Hex(const void* data, size_t count) {
    static const char* digits = "0123456789abcdef"; std::string out(count * 2, '0');
    for (size_t i = 0; i < count; ++i) { const uint8 b = static_cast<const uint8*>(data)[i]; out[2 * i] = digits[b >> 4]; out[2 * i + 1] = digits[b & 15]; }
    return out;
}
inline std::string Hex(const Region& r) { return Hex(reinterpret_cast<void*>(r.address), r.size); }
inline uint8 Pat(unsigned base, unsigned j) { return uint8(base + j * 7); }

// ---- X87 environment (precedent: RequestedPadDistanceTrainNopHexV2) ------------------------------------------------
using X87Environment = std::array<uint8, 28>;
__declspec(noinline) inline X87Environment ReadX87Environment(){X87Environment image{};auto* environmentPointer=image.data();__asm {
 mov eax,environmentPointer
 fnstenv [eax]
 fldenv [eax]
 }return image;}
__declspec(noinline) inline void RestoreX87Environment(const X87Environment& image){const auto* environmentPointer=image.data();__asm {
 mov eax,environmentPointer
 fldenv [eax]
 }}

// ---- scripted private DirectInput device ----------------------------------------------------------------------------
struct FakeDevice { const void* const* vtable; uint32 index; };
inline std::array<const void*, 32> vtableStorage;
inline std::array<FakeDevice, 2> devices;
alignas(16) inline std::array<uint8, 72> arena;      // 16 guard + psGlobalType(0x28) + 16 guard
inline std::array<uint8, 272> joyBytes;
inline json trace;
inline bool valid = true;
inline Fixture current{};
inline unsigned eventCalls{}, deviceCalls{};
inline std::vector<HRESULT> pollSeq, acqSeq; inline size_t pollPos{}, acqPos{}; inline HRESULT stateHr{};
inline constexpr HRESULT kNotAcquired = HRESULT(0x8007000CL), kInputLost = HRESULT(0x8007001EL), kAccess = HRESULT(0x80070005L), kFail = HRESULT(0x80004005L);

inline bool InStack(const void* pointer, size_t count) {
    const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); const uintptr_t low = uintptr_t(tib->StackLimit), high = uintptr_t(tib->StackBase), at = uintptr_t(pointer);
    return pointer && at >= low && at <= high - count;
}
inline int DeviceIndex(const FakeDevice* self) {
    const int index = self == &devices[0] ? 0 : self == &devices[1] ? 1 : -1;
    valid &= index == int(current.pad); return index;     // only the selected slot's device may be used
}
inline HRESULT __stdcall DevPoll(FakeDevice* self) {
    const int index = DeviceIndex(self); if (++deviceCalls > 64) { valid = false; return kFail; }
    const HRESULT hr = pollPos < pollSeq.size() ? pollSeq[pollPos++] : 0; trace.push_back(json::array({"poll", index, int32(hr)})); return hr;
}
inline HRESULT __stdcall DevAcquire(FakeDevice* self) {
    const int index = DeviceIndex(self); if (++deviceCalls > 64) { valid = false; return kFail; }
    const HRESULT hr = acqPos < acqSeq.size() ? acqSeq[acqPos++] : 0; trace.push_back(json::array({"acq", index, int32(hr)})); return hr;
}
inline HRESULT __stdcall DevGetState(FakeDevice* self, DWORD size, void* data) {
    const int index = DeviceIndex(self); const bool ok = size == 272 && InStack(data, 272); valid &= ok;
    if (ok && stateHr >= 0) std::memcpy(data, joyBytes.data(), 272);
    trace.push_back(json::array({"state", index, uint32(size)})); return stateHr;
}
template<size_t N> inline HRESULT __stdcall DevUnexpected(FakeDevice*) { valid = false; trace.push_back(json::array({"unexpected", N})); return kFail; }
template<size_t... I> inline void FillUnexpected(std::index_sequence<I...>) { ((vtableStorage[I] = reinterpret_cast<const void*>(&DevUnexpected<I>)), ...); }
inline void InitDevices() {
    FillUnexpected(std::make_index_sequence<32>{});
    vtableStorage[7] = reinterpret_cast<const void*>(&DevAcquire); vtableStorage[9] = reinterpret_cast<const void*>(&DevGetState); vtableStorage[25] = reinterpret_cast<const void*>(&DevPoll);
    for (uint32 i = 0; i < 2; ++i) devices[i] = {vtableStorage.data(), i};
}
inline uint32& Slot(unsigned slot) { return *reinterpret_cast<uint32*>(arena.data() + 16 + 0x20 + 4 * slot); }

// RsPadEventHandler adapter: cdecl(event, param). Used for the EXE (native 0x6195A0) and the DLL function alike.
inline int __cdecl Event(int code, void* param) {
    ++eventCalls; const bool inStack = InStack(param, 4); valid &= inStack; const int32 value = inStack ? *static_cast<int32*>(param) : -1;
    trace.push_back(json::array({"evt", code, value}));
    if (current.flip == 3 && eventCalls == 1 && inStack) *static_cast<int32*>(param) = 1 - value;      // caller slot is re-read by EXE and must be by the candidate
    if (current.flip == 4 && eventCalls == 1) Slot(current.pad) = 0;                                  // device pointer cleared mid-call
    if ((current.flip == 1 && eventCalls == 2) || (current.flip == 2 && eventCalls == 3)) { auto& n = Mem<uint8>(kPadNumber.address); n = n ? 0 : 1; }
    return 0;
}

// ---- hook helpers (same pattern as RequestedPadUpdateMouseV2) -------------------------------------------------------
inline uintptr_t Body(uintptr_t address) { for (int j = 0; j < 16 && *reinterpret_cast<uint8*>(address) == 0xE9; ++j) address += 5 + *reinterpret_cast<int32*>(address + 1); return address; }
inline Simple* ByNative(uintptr_t native) {
    Simple* found{}; unsigned hits = 0;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { for (auto& h : c.Items()) if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple) { auto* p = static_cast<Simple*>(h.get()); if (uintptr_t(p->GetHookGTAAddress()) == native) { found = p; ++hits; } } });
    return hits == 1 ? found : nullptr;
}
struct Gate {
    std::vector<uintptr_t> entries; std::vector<std::pair<uintptr_t, std::vector<uint8>>> before; std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches; bool good = true;
    template<class A, class B> Gate(uintptr_t native, Simple* hook, A original, B candidate) {
        good = hook && uintptr_t(hook->GetHookGTAAddress()) == native; if (!good) return;
        const auto own = uintptr_t(hook->GetHookOurAddress()); entries = {native, own}; const auto body = Body(own);
        if (std::find(entries.begin(), entries.end(), body) == entries.end()) entries.push_back(body);
        for (auto p : entries) before.push_back({p, ReversingProbe::Bytes(reinterpret_cast<void*>(p), 52)});
        for (auto p : entries) { if (p == native) patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p, original)); else patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p, candidate)); }
    }
    bool Active() const { if (!good) return false; for (size_t j = 0; j < entries.size(); ++j) if (ReversingProbe::Bytes(reinterpret_cast<void*>(entries[j]), 5) != std::vector<uint8>(patches[j]->jump.begin(), patches[j]->jump.end())) return false; return true; }
    bool Restore() { patches.clear(); for (auto& [p, bytes] : before) good &= bytes == ReversingProbe::Bytes(reinterpret_cast<void*>(p), 52); return good; }
};

// ---- fixture seeding (must match model.py) --------------------------------------------------------------------------
inline void BuildJoyState(const Fixture& f) {
    for (unsigned j = 0; j < 272; ++j) joyBytes[j] = Pat(f.poison + 0x77, j);
    std::memcpy(&joyBytes[0], &f.lx, 4); std::memcpy(&joyBytes[4], &f.ly, 4); std::memcpy(&joyBytes[8], &f.lz, 4); std::memcpy(&joyBytes[20], &f.lrz, 4); std::memcpy(&joyBytes[0x20], &f.pov, 4);
}
inline void Seed(const Fixture& f, uintptr_t dev0, uintptr_t dev1) {
    const unsigned p = f.poison, q = p / 85;
    for (unsigned j = 0; j < kControls.size; ++j) Mem<uint8>(kControls.address + j) = Pat(p + 0x11, j);
    Mem<uint8>(kControls.address) = f.justinit;
    const float rx[4]{0.f, 0.25f, 0.5f, -3.5f}, ry[4]{0.f, -0.25f, -0.75f, 0.3f};
    Mem<float>(0xC8CFB0) = rx[q]; Mem<float>(0xC8CFB4) = ry[q]; Mem<float>(0xC8CFB8) = 13.5f + q; Mem<float>(0xC8CFBC) = 14.5f + q;
    for (unsigned j = 0; j < kPads.size; ++j) Mem<uint8>(kPads.address + j) = Pat(p + 0x22, j);
    for (unsigned j = 0; j < kJoy.size; ++j) Mem<uint8>(kJoy.address + j) = Pat(p + 0x33, j);
    const unsigned other = 1 - f.pad;
    Mem<uint8>(kJoy.address + f.pad * 16 + 5) = f.joymask & 1; Mem<uint8>(kJoy.address + f.pad * 16 + 6) = (f.joymask >> 1) & 1;
    Mem<uint8>(kJoy.address + other * 16 + 5) = (f.joymask ^ 3) & 1; Mem<uint8>(kJoy.address + other * 16 + 6) = ((f.joymask ^ 3) >> 1) & 1;
    for (unsigned k = 0; k < 6; ++k) Mem<uint8>(0xBA67F9 + k) = (f.menu >> k) & 1;
    Mem<uint8>(kPadNumber.address) = f.padnum;
    arena.fill(0xC7); for (unsigned j = 0; j < 40; ++j) arena[16 + j] = Pat(p + 0x44, j);
    Slot(0) = uint32(dev0); Slot(1) = uint32(dev1); if (f.route == 0) Slot(f.pad) = 0;
    Mem<uint32>(kPsPointer.address) = uint32(uintptr_t(arena.data() + 16));
    BuildJoyState(f);
    static const std::array<std::pair<std::vector<HRESULT>, std::vector<HRESULT>>, 11> scripts{{
        {{}, {}}, {{0}, {}}, {{1}, {}}, {{kNotAcquired, 0}, {0}}, {{kNotAcquired, 0}, {kInputLost, 0}}, {{kNotAcquired, 0}, {kInputLost, kInputLost, kInputLost, 0}},
        {{kNotAcquired}, {kFail}}, {{kNotAcquired}, {kInputLost, kAccess}}, {{kNotAcquired, kNotAcquired}, {0}}, {{0}, {}}, {{kNotAcquired, 0}, {1}}}};
    pollSeq = scripts[f.route].first; acqSeq = scripts[f.route].second; pollPos = acqPos = 0;
    stateHr = f.route == 2 ? HRESULT(1) : f.route == 9 ? kNotAcquired : HRESULT(0);
}

inline void Update() {
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::ofstream log(std::filesystem::path(exe).parent_path() / "pad-process-pad-runtime.jsonl");
    auto write = [&](const json& row) { log << row.dump() << '\n'; log.flush(); };
    auto* target = ByNative(0x746A10);
    if (!target) { write({{"event", "error"}, {"reason", "missing target hook"}}); return; }
    const unsigned savedFP = _controlfp(0, 0), savedSSE = _mm_getcsr(); const X87Environment savedEnv = ReadX87Environment();
    std::vector<std::vector<uint8>> saved; for (const auto& r : kSaved) saved.push_back(Snap(r));
    const std::array<bool, 2> hookFlags{target->Hooked(), target->Locked()}; target->LockState(false); target->State(false);
    InitDevices();
    // DLL RsPadEventHandler body and EXE 0x6195A0 both go to the same traced adapter. Calls from candidate and EXE then share a trace.
    auto* events = ByNative(0x6195A0);
    std::vector<std::unique_ptr<Gate>> gates; gates.push_back(std::make_unique<Gate>(0x6195A0, events, Event, Event));
    const bool known = (savedFP & _MCW_RC) == _RC_NEAR && (savedSSE & 0x6000) == 0 && std::ranges::all_of(gates, [](const auto& g) { return g->good; });
    write({{"event", "begin"}, {"fp_control_before", savedFP}, {"mxcsr_before", savedSSE}, {"helpers_known", known}, {"arena", uintptr_t(arena.data())}, {"dev0", uintptr_t(&devices[0])}, {"dev1", uintptr_t(&devices[1])},
           {"menu_real", Hex(kMenu)}, {"real_ps_pointer", Mem<uint32>(kPsPointer.address)}, {"target_native", 0x746A10}, {"target_candidate", uintptr_t(target->GetHookOurAddress())}, {"event_native", 0x6195A0},
           {"event_candidate", events ? uintptr_t(events->GetHookOurAddress()) : 0}, {"count", kFixtureCount}, {"x87_env_before", Hex(savedEnv.data(), 28)}});
    size_t pairs = 0, differences = 0; bool all = known;
    const std::array<const char*, 9> required{"helper_gates", "target_gate", "abi_valid", "guards_immutable", "ps_pointer_intact", "fp_control_ok", "mxcsr_control_ok", "x87_stack_empty", "devices_ok"};
    if (known) for (unsigned i = 0; i < kFixtureCount; ++i) {
        const Fixture& f = fixtures[i]; current = f;
        json fixture{{"index", i}, {"route", f.route}, {"pad", f.pad}, {"padnum", f.padnum}, {"justinit", f.justinit}, {"flip", f.flip}, {"pc", f.pc}, {"joymask", f.joymask}, {"menu", f.menu}, {"poison", f.poison}, {"lx", f.lx}, {"ly", f.ly}, {"lz", f.lz}, {"lrz", f.lrz}, {"pov", f.pov}};
        json legs[2];
        for (int leg = 0; leg < 2; ++leg) {
            Seed(f, uintptr_t(&devices[0]), uintptr_t(&devices[1]));
            const auto guards = std::array<std::vector<uint8>, 6>{Snap(kControlsBefore), Snap(kControlsAfter), Snap(kSticksBefore), Snap(kSticksAfter), Snap(kJoyBefore), Snap(kJoyAfter)};
            trace = json::array(); valid = true; eventCalls = deviceCalls = 0;
            const unsigned wantedControl = (savedFP & ~unsigned(_MCW_PC)) | (f.pc ? _PC_53 : _PC_24);
            _clearfp(); _controlfp(savedFP, fpMask); _controlfp(f.pc ? _PC_53 : _PC_24, _MCW_PC); _mm_setcsr(savedSSE);
            target->State(leg != 0);
            const int eax = reinterpret_cast<int(__cdecl*)(int)>(0x746A10)(int(f.pad));
            const X87Environment env = ReadX87Environment();                  // immediately after the target, before any allocation
            const unsigned control = _controlfp(0, 0), mxcsr = _mm_getcsr();
            const bool active = std::ranges::all_of(gates, [](const auto& g) { return g->Active(); });
            const bool targetGate = target->Hooked() == bool(leg) && ReversingProbe::JumpsToDll(0x746A10) == bool(leg);
            const bool guardsOk = guards[0] == Snap(kControlsBefore) && guards[1] == Snap(kControlsAfter) && guards[2] == Snap(kSticksBefore) && guards[3] == Snap(kSticksAfter) && guards[4] == Snap(kJoyBefore) && guards[5] == Snap(kJoyAfter);
            const bool psOk = Mem<uint32>(kPsPointer.address) == uint32(uintptr_t(arena.data() + 16));
            const uint16 tag = uint16(env[8] | (env[9] << 8));
            json payload{{"controls", Hex(kControls)}, {"sticks", Hex(kSticks)}, {"pads", Hex(kPads)}, {"joy_table", Hex(kJoy)}, {"menu", Hex(kMenu)}, {"padnum", Mem<uint8>(kPadNumber.address)}, {"arena", Hex(arena.data(), arena.size())}, {"trace", trace}};
            legs[leg] = {{"payload", payload}, {"eax", eax}, {"helper_gates", active}, {"target_gate", targetGate}, {"abi_valid", valid}, {"guards_immutable", guardsOk}, {"ps_pointer_intact", psOk},
                         {"fp_control_ok", control == wantedControl}, {"mxcsr_control_ok", (mxcsr & ~0x3Fu) == (savedSSE & ~0x3Fu)}, {"x87_stack_empty", tag == 0xFFFF}, {"devices_ok", deviceCalls <= 64},
                         {"fp_control_raw", control}, {"mxcsr_raw", mxcsr}, {"x87_env", Hex(env.data(), 28)}, {"event_calls", eventCalls}};
            RestoreX87Environment(savedEnv); _controlfp(savedFP, fpMask); _mm_setcsr(savedSSE);
        }
        bool equal = legs[0]["payload"] == legs[1]["payload"]; for (const auto& leg : legs) for (const char* key : required) equal &= leg[key] == true;
        equal &= legs[0]["fp_control_raw"] == legs[1]["fp_control_raw"];
        all &= equal; differences += !equal;
        write({{"event", "comparison"}, {"function", "CPad::ProcessPad"}, {"fixture", fixture}, {"pair", pairs++}, {"equal", equal}, {"eax_equal", legs[0]["eax"] == legs[1]["eax"]}, {"x87_env_equal", legs[0]["x87_env"] == legs[1]["x87_env"]}, {"original", legs[0]}, {"candidate", legs[1]}});
        if (i % 64 == 63) ReversingProbe::PumpMessages();
    }
    target->State(hookFlags[0]); target->LockState(hookFlags[1]);
    bool restored = true; for (auto& g : gates) restored &= g->Restore();
    for (size_t j = 0; j < kSaved.size(); ++j) std::memcpy(reinterpret_cast<void*>(kSaved[j].address), saved[j].data(), saved[j].size());
    RestoreX87Environment(savedEnv); _controlfp(savedFP, fpMask); _mm_setcsr(savedSSE);
    for (size_t j = 0; j < kSaved.size(); ++j) restored &= saved[j] == Snap(kSaved[j]);
    const X87Environment finalEnv = ReadX87Environment();
    write({{"event", "complete"}, {"comparisons", pairs}, {"differences", differences}, {"all_equal", all}, {"state_restored", restored}, {"fp_restored", _controlfp(0, 0) == savedFP && _mm_getcsr() == savedSSE && finalEnv == savedEnv}, {"x87_env_final", Hex(finalEnv.data(), 28)}});
}
}
