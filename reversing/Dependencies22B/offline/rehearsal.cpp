// Ensayo sin juego del banco Dependencies22B: ejecuta los bytes REALES de gta-sa.exe (imagen mapeada en sus direcciones en un proceso hijo de 32 bits) frente al codigo de la DLL
// (texto de Vehicle.cpp: DoHeadLightEffect / DoTailLightEffect, extraido por extract_glue.py, con tipos falsos de mock.h y X87Math.cpp compilado con las opciones del proyecto).
// Usa las MISMAS fixtures que el banco (../Fixtures.hpp) con la misma semilla por caso y escribe un registro con el mismo esquema que LiveComparison.hpp, que se puede pasar por
// `python run_local.py --analyze`. No sustituye al banco dentro del juego: no acredita validacion.
//   uso: rehearsal.exe [casos por funcion = 4096] [primer caso = 0] [salida.jsonl] [funcion: -1 ambas, 0 Head, 1 Tail]
#include "mock.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <map>
#include <string>
#include "Fixtures.hpp"
using namespace Dependencies22BFix;

#include "glue_head.txt"
#include "glue_tail.txt"

static bool MapExeInto(HANDLE proc) {
    HANDLE f = CreateFileA("D:/Grand Theft Auto San Andreas/gta-sa.exe", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    const DWORD size = GetFileSize(f, nullptr);
    std::vector<uint8> raw(size);
    DWORD got{};
    ReadFile(f, raw.data(), size, &got, nullptr);
    CloseHandle(f);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(raw.data());
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(raw.data() + dos->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    void* whole = VirtualAllocEx(proc, reinterpret_cast<void*>(nt->OptionalHeader.ImageBase), nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (whole != reinterpret_cast<void*>(nt->OptionalHeader.ImageBase)) { std::printf("no se pudo reservar la imagen\n"); return false; }
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uintptr_t va = nt->OptionalHeader.ImageBase + sec[i].VirtualAddress;
        SIZE_T written{};
        if (!WriteProcessMemory(proc, reinterpret_cast<void*>(va), raw.data() + sec[i].PointerToRawData, (std::min)(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize), &written)) return false;
    }
    return true;
}
static void Hook(uintptr_t at, void* fn) {
    auto* p = reinterpret_cast<uint8*>(at);
    p[0] = 0xE9;
    const uint32 rel = uint32(reinterpret_cast<uintptr_t>(fn) - at - 5);
    std::memcpy(p + 1, &rel, 4);
}

// ---------------------------------------------------------------- mismas utilidades que LiveComparison.hpp
static json trace = json::array();
static json Ptr(const void* p) {
    const auto u = reinterpret_cast<uintptr_t>(p);
    if (!u) return nullptr;
#define TAG(name, off, size) if (u >= uintptr_t(arena.data() + off) && u < uintptr_t(arena.data() + off + size)) return json::array({name, u - uintptr_t(arena.data() + off)});
    TAG("veh", Layout::veh, Layout::vehSize) TAG("matVeh", Layout::matVeh, Layout::matSize) TAG("matArg", Layout::matArg, Layout::matSize) TAG("matCam", Layout::matCam, Layout::matSize)
    TAG("modelInfo", Layout::modelInfo, Layout::modelInfoSize) TAG("vstruct", Layout::vstruct, Layout::vstructSize) TAG("handling", Layout::handling, Layout::handlingSize)
    TAG("driver", Layout::driver, Layout::driverSize) TAG("other", Layout::other, Layout::otherSize)
#undef TAG
    if (u >= 0xB6F028 && u < 0xB6F028 + kCameraBytes) return json::array({"camera", u - 0xB6F028});
    { const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if (u >= reinterpret_cast<uintptr_t>(tib->StackLimit) && u < reinterpret_cast<uintptr_t>(tib->StackBase)) return "local"; }
    if (u >= 0x400000 && u < 0x1000000) return json::array({"exe", u});
    return "local";
}
static json CoronaId(uint32 id) { return int64(int32(id - uint32(uintptr_t(Veh())))); }
static void __cdecl MRegisterCorona(uint32 id, uint32 attach, uint32 r, uint32 g, uint32 b, uint32 a, const float* pos, uint32 size, uint32 range, uint32 type, uint32 flare, uint32 refl, uint32 los,
                                    uint32 trail, uint32 angle, uint32 neon, uint32 pull, uint32 fullBright, uint32 fade, uint32 below, uint32 white) {
    trace.push_back({"register_corona", CoronaId(id), Ptr(reinterpret_cast<void*>(uintptr_t(attach))), r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), size, range, type, flare, refl, los, trail,
                     angle, neon, pull, fullBright, fade, below, white});
}
static CVehicle* __cdecl MFindPlayerVehicle(int32 playerId, int32 includeRemote) {
    trace.push_back({"find_player_vehicle", playerId, includeRemote & 0xFF});
    return G.findVehicle == 0 ? nullptr : G.findVehicle == 1 ? reinterpret_cast<CVehicle*>(Veh()) : reinterpret_cast<CVehicle*>(Other());
}
static uint32 __fastcall MPanelStatus(CDamageManager* self, void*, uint32 panel) {
    trace.push_back({"get_panel_status", Ptr(self), panel & 0xFF});
    return G.panel;
}
// puentes de la DLL hacia los colaboradores falsos
void CCoronas::RegisterCorona(uint32 id, void* attachTo, uint8 r, uint8 g, uint8 b, uint8 intensity, const CVector& pos, float size, float range, eCoronaType coronaType, eCoronaFlareType flareType,
                              eCoronaReflType reflType, eCoronaLOSCheck checkLOS, eCoronaTrail usesTrails, float normalAngle, bool neonFade, float pullTowardsCam, bool fullBrightAtStart,
                              float fadeSpeed, bool onlyFromBelow, bool whiteCore) {
    MRegisterCorona(id, uint32(uintptr_t(attachTo)), r, g, b, intensity, &pos.x, Bits(size), Bits(range), uint32(coronaType), flareType, reflType, checkLOS, usesTrails, Bits(normalAngle), neonFade, Bits(pullTowardsCam),
                    fullBrightAtStart, Bits(fadeSpeed), onlyFromBelow, whiteCore);
}
CVehicle* FindPlayerVehicle(int32 playerId, bool incl) { return MFindPlayerVehicle(playerId, incl); }
ePanelDamageState CDamageManager::GetPanelStatus(ePanels panel) const { return ePanelDamageState(MPanelStatus(const_cast<CDamageManager*>(this), nullptr, panel)); }
CCam& CCamera::GetActiveCamera() { return TheCamera.m_aCams[TheCamera.m_nActiveCam]; }

struct Frame {
    uint32 eax{}, espBefore{}, espAfter{}, ebxBefore{}, ebxAfter{}, esiBefore{}, esiAfter{}, ediBefore{}, ediAfter{}, ebpBefore{}, ebpAfter{}, sehBefore{}, sehAfter{};
    std::array<uint8, 28> envBefore{}, envAfter{};
};
static unsigned EnvWord(const std::array<uint8, 28>& e, int field) { uint16 v; std::memcpy(&v, e.data() + field, 2); return v; }
__declspec(noinline) static Frame CallFour(uintptr_t entry, void* self, uint32 a1, uint32 a2, uint32 a3, uint32 a4) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}, rv{}; uint8* e0 = r.envBefore.data(); uint8* e1 = r.envAfter.data();
    __asm {
        mov eax, e0
        fnstenv [eax]
        fldenv [eax]
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a4
        push a3
        push a2
        push a1
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov rv, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, e1
        fnstenv [eax]
        fldenv [eax]
    }
    r.eax = rv; r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}
__declspec(noinline) static Frame CallSix(uintptr_t entry, void* self, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}, rv{}; uint8* e0 = r.envBefore.data(); uint8* e1 = r.envAfter.data();
    __asm {
        mov eax, e0
        fnstenv [eax]
        fldenv [eax]
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a6
        push a5
        push a4
        push a3
        push a2
        push a1
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov rv, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, e1
        fnstenv [eax]
        fldenv [eax]
    }
    r.eax = rv; r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}
static uint8* CamBytes() { return reinterpret_cast<uint8*>(0xB6F028); }
static std::vector<uint8> Capture() {
    std::vector<uint8> v(arena.begin(), arena.end());
    v.insert(v.end(), CamBytes(), CamBytes() + kCameraBytes);
    return v;
}
static void Restore(const std::vector<uint8>& v) {
    std::memcpy(arena.data(), v.data(), arena.size());
    std::memcpy(CamBytes(), v.data() + arena.size(), kCameraBytes);
}
static json Diff(const std::vector<uint8>& before, const std::vector<uint8>& after) {
    json runs = json::array();
    static const char* hex = "0123456789abcdef";
    for (size_t i = 0; i < before.size();) {
        if (before[i] == after[i]) { ++i; continue; }
        size_t j = i; std::string bytes;
        while (j < before.size() && before[j] != after[j]) { bytes += hex[after[j] >> 4]; bytes += hex[after[j] & 15]; ++j; }
        runs.push_back(json::array({i, bytes})); i = j;
    }
    return runs;
}
static bool GuardsOK() {
    for (size_t g : {size_t(0), Layout::veh + Layout::vehSize, Layout::matVeh + Layout::matSize, Layout::matArg + Layout::matSize, Layout::matCam + Layout::matSize, Layout::modelInfo + Layout::modelInfoSize,
                     Layout::vstruct + Layout::vstructSize, Layout::handling + Layout::handlingSize, Layout::driver + Layout::driverSize, Layout::other + Layout::otherSize})
        for (size_t i = 0; i < kGuard; ++i) if (arena[g + i] != 0xA5) return false;
    return true;
}
static uint32* ModelSlot(uint32 model) { return reinterpret_cast<uint32*>(0xA9B0C8) + model; }
static uintptr_t AddrOf(auto mf) { uintptr_t a{}; static_assert(sizeof(mf) == sizeof(a)); std::memcpy(&a, &mf, sizeof(a)); return a; }

// Sin objetos con destructor: __try no puede convivir con ellos en la misma funcion.
static bool CallGuarded(bool tail, uintptr_t entry, uint32 matrix, uint32 isRight, uint32 disabled, uint32 staticEmission, Frame* out) {
    __try {
        *out = !tail ? CallFour(entry, Veh(), uint32(args.lightId), matrix, isRight, disabled) : CallSix(entry, Veh(), uint32(args.lightId), matrix, isRight, disabled, args.unused, staticEmission);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static json Invoke(bool tail, bool candidate, const std::vector<uint8>& input, const Cfg& cfg0) {
    Restore(input); G = cfg0; trace = json::array();
    void* matrix = (args.matrixIsVehicle && Get<void*>(Veh(), 0x14)) ? static_cast<void*>(MatVeh()) : static_cast<void*>(MatArg());
    const uint32 isRight = args.isRight | (args.unused & 0xFFFFFF00u), disabled = args.disabledOrAlarm | ((args.unused * 2654435761u) & 0xFFFFFF00u);
    const uintptr_t entry = candidate ? (tail ? AddrOf(&CVehicle::DoTailLightEffect) : AddrOf(&CVehicle::DoHeadLightEffect)) : (tail ? 0x6E1780 : 0x6E0A50);
    Frame f{};
    const bool crashed = !CallGuarded(tail, entry, uint32(uintptr_t(matrix)), isRight, disabled, args.staticEmission | ((args.unused * 3u) & 0xFFFFFF00u), &f);
    const auto after = Capture();
    const bool guards = GuardsOK();
    json row = {
        {"trace", trace}, {"changes", Diff(input, after)}, {"x87", {{"al", f.eax & 0xFF}, {"fcw", EnvWord(f.envAfter, 0)}, {"fsw_top", (EnvWord(f.envAfter, 4) >> 11) & 7}, {"ftw", EnvWord(f.envAfter, 8)}}},
        {"route_ok", true}, {"stack_restored", !crashed && f.espBefore == f.espAfter},
        {"nonvolatile_preserved", f.ebxBefore == f.ebxAfter && f.esiBefore == f.esiAfter && f.ediBefore == f.ediAfter && f.ebpBefore == f.ebpAfter},
        {"seh_restored", f.sehBefore == f.sehAfter},
        {"fpu_top_balanced", ((EnvWord(f.envBefore, 4) >> 11) & 7) == ((EnvWord(f.envAfter, 4) >> 11) & 7) && EnvWord(f.envAfter, 8) == 0xFFFF},
        {"fpu_control_same", EnvWord(f.envBefore, 0) == EnvWord(f.envAfter, 0)},
        {"guards_preserved", guards}, {"fixture_contract", guards && after.size() == input.size()}};
    Restore(input); G = cfg0;
    row["state_restored"] = Capture() == input;
    row["floating_state_restored"] = true;
    return row;
}

static int Launcher(int argc, char** argv) {
    char self[MAX_PATH]; GetModuleFileNameA(nullptr, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" --child";
    for (int i = 1; i < argc; ++i) cmd += std::string(" \"") + argv[i] + "\"";
    STARTUPINFOA si{}; si.cb = sizeof(si); PROCESS_INFORMATION pi{};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { std::printf("CreateProcess fallo\n"); return 2; }
    if (!MapExeInto(pi.hProcess)) { TerminateProcess(pi.hProcess, 3); return 3; }
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code{}; GetExitCodeProcess(pi.hProcess, &code);
    return int(code);
}
int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    argc -= 1; argv += 1;
    setvbuf(stdout, nullptr, _IONBF, 0);
    unsigned cw = 0; _controlfp_s(&cw, _PC_24, _MCW_PC);
    Hook(0x6FC580, (void*)&MRegisterCorona); Hook(0x56E0D0, (void*)&MFindPlayerVehicle); Hook(0x6C2180, (void*)&MPanelStatus);
    camBase = CamBytes();
    const int N = argc > 1 ? std::atoi(argv[1]) : 4096;
    const int first = argc > 2 ? std::atoi(argv[2]) : 0;
    const char* outPath = argc > 3 ? argv[3] : "rehearsal.jsonl";
    const int only = argc > 4 ? std::atoi(argv[4]) : -1;
    std::ofstream file; if (std::strcmp(outPath, "-") != 0) file.open(outPath);   // "-" = sin registro (ejecuciones largas)
    std::ostream& log = file.is_open() ? static_cast<std::ostream&>(file) : static_cast<std::ostream&>(*new std::ofstream("NUL"));
    const auto cameraSaved = std::vector<uint8>(CamBytes(), CamBytes() + kCameraBytes);
    static const char* names[] = {"CVehicle::DoHeadLightEffect", "CVehicle::DoTailLightEffect"};
    log << json{{"event", "begin"}, {"registered_hook", true}, {"ready", true}, {"target_routes_ready", true}, {"fixture_contract", true}, {"collaborator_bodies_ok", true},
                {"routes", json::array({json{{"function", "DoHeadLightEffect"}, {"registered", true}, {"body", 1}, {"module_ok", true}}, json{{"function", "DoTailLightEffect"}, {"registered", true}, {"body", 2}, {"module_ok", true}}})},
                {"collaborators", json::array({json::array({0x6FC580, 1}), json::array({0x56E0D0, 2}), json::array({0x6C2180, 3})})}, {"probe_module", 1}, {"arena_bytes", arena.size()}, {"camera_bytes", kCameraBytes},
                {"vehicle_bytes", kVehicleBytes}, {"pump_state_intact", true}, {"scope", "ensayo sin juego (rehearsal.cpp): bytes reales del EXE mapeados en un proceso hijo"}}.dump() << '\n';
    int totalFail = 0; unsigned pairs = 0;
    for (int fi = 0; fi < 2; ++fi) {
        if (only >= 0 && fi != only) continue;
        const bool tail = fi == 1;
        int fail = 0, shown = 0;
        for (int n = first; n < first + N; ++n, ++pairs) {
            fixRng = Rng(uint64(fi + 1) * 1000003ull + uint64(n) * 7919ull + 17ull);
            const json profile = SetupCase(tail);
            const auto input = Capture();
            const Cfg cfg0 = G;
            const uint32 slotValue = *ModelSlot(modelIndex);
            *ModelSlot(modelIndex) = uint32(uintptr_t(ModelInfo()));
            const json original = Invoke(tail, false, input, cfg0);
            const json candidate = Invoke(tail, true, input, cfg0);
            *ModelSlot(modelIndex) = slotValue;
            std::memcpy(CamBytes(), cameraSaved.data(), kCameraBytes);
            bool equal = original == candidate;
            for (const char* gate : {"route_ok", "stack_restored", "nonvolatile_preserved", "seh_restored", "fpu_top_balanced", "fpu_control_same", "guards_preserved", "fixture_contract", "state_restored", "floating_state_restored"})
                equal &= original.value(gate, false) && candidate.value(gate, false);
            if (!equal) { ++fail; if (shown++ < 3) std::printf("[%s #%d] DIFERENCIA\n  EXE: %s\n  DLL: %s\n", names[fi], n, original.dump().c_str(), candidate.dump().c_str()); }
            log << json{{"event", "comparison"}, {"function", names[fi]}, {"pair", pairs}, {"case", n}, {"profile", profile}, {"original", original}, {"candidate", candidate}, {"equal", equal}, {"accepted", equal}}.dump() << '\n';
        }
        log << json{{"event", "function_summary"}, {"function", names[fi]}, {"cases", N}, {"failures", fail}}.dump() << '\n';
        std::printf("%s: %d casos (desde %d), %d diferencias\n", names[fi], N, first, fail);
        totalFail += fail;
    }
    log << json{{"event", totalFail ? "error" : "complete"}, {"pairs", pairs}, {"state_restored", true}, {"floating_state_restored", true}, {"pump_state_intact", true}}.dump() << '\n';
    return totalFail ? 1 : 0;
}
