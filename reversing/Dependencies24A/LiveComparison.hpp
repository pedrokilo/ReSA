#pragma once
#include <fstream>
#include "Ped.h"
#include "PlayerPed.h"
#include "World.h"
#include "Pad.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "AudioEngine.h"
#include "AEAudioHardware.h"
#include "PostEffects.h"
#include "MenuManager.h"
#include "WinInput.h"
#include "WinPlatform.h"
#include "WndProc.h"
#include "VideoPlayer.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par de Win::__MainWndProc 0x747EB0 (hook RH apagado = EXE, encendido = DLL), dentro del juego.
//   WNDPROC __stdcall (HWND, UINT, WPARAM, LPARAM), ret 0x10, LRESULT en EAX. 19 rutas (una por familia de mensajes) x 1024 casos = 19456 pares (Fixtures.hpp).
//   Mensajes sinteticos entregados directamente a la entrada (sin ventana de prueba: ningun colaborador toca la ventana real, todas las API de Win32 que usa el procedimiento estan sustituidas).
// Colaboradores sustituidos en AMBOS lados por el mismo falso trazado:
//   funciones del juego (EntryPatch en la direccion del EXE y en el cuerpo de la DLL, resuelto con Resolve() ANTES de crear parches): RsEventHandler 0x619B60, RsKeyboardEventHandler 0x619560,
//   GTATranslateKey 0x747820, GTATranslateShiftKey 0x747CD0, CAudioEngine::PauseAllSounds 0x507430 / ResumeAllSounds 0x507440, CPad::Clear 0x541A70, CPostEffects::SetupBackBufferVertex 0x7043D0,
//   WinInput::Shutdown (EXE 0x747000), VideoPlayer::WaitState 0x745F90 / UpdateWindow 0x7466D0 / OnGraphNotify 0x745FB0, CAEAudioHardware::IsStreamingFromDVD 0x4D95C0 / GetDVDDriveLetter 0x4D95D0 /
//   CheckDVD 0x4D95E0, CMenuManager::NoDiskInDriveMessage 0x57C5E0, CCutsceneMgr::SkipCutscene 0x5B1700, RwD3D9GetCurrentD3DDevice 0x7F9D50 (RenderWare);
//   objetos COM falsos (IDirect3DDevice9::SetGammaRamp, IMediaControl::GetState/Run/Pause) en 0xC920D8 y devuelto por RwD3D9GetCurrentD3DDevice;
//   API de Win32 (ClipCursor, PostQuitMessage, SetCursor, ShowCursor, SetCapture, ReleaseCapture, GetWindowPlacement, SendMessageA, SetTimer, SetFocus, SetWindowPos, DefWindowProcA, OutputDebugStringA)
//   por IAT en el EXE y en la DLL (solo el hilo principal; los demas hilos van a la API real).
// NO se sustituyen: _ftol2 (0x821B40, CRT del EXE) ni el 0x53BC60 (OnSetFocus del menu, en linea en la DLL): sus efectos se comparan por memoria y por el estado x87 completo.
namespace Dependencies24ALive {
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t a) { MEMORY_BASIC_INFORMATION i{}; return VirtualQuery(reinterpret_cast<void*>(a), &i, sizeof(i)) == sizeof(i) ? uintptr_t(i.AllocationBase) : 0; }
inline uintptr_t Resolve(uintptr_t a) { for (unsigned i = 0; i < 16 && *reinterpret_cast<uint8*>(a) == 0xE9; ++i) a += 5 + *reinterpret_cast<int32*>(a + 1); return a; }
inline void Write(const json& row) { RestoreFloatingState(); ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); RestoreFloatingState(); }

struct Watch { uintptr_t address; std::vector<uint8> saved; Watch(uintptr_t a, size_t n) : address(a), saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a), n)) {} bool Intact() const { return saved == ReversingProbe::Bytes(reinterpret_cast<void*>(address), saved.size()); } };
// Los parches se deshacen en orden INVERSO
struct PatchSet { std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; void Add(uintptr_t at, void* f) { v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at, f)); } ~PatchSet() { while (!v.empty()) v.pop_back(); } };

struct Col { uintptr_t exe, dll; void* fake; };
inline std::vector<Col> Collaborators() {
    std::vector<Col> cols;
    auto Add = [&](uintptr_t exeAt, uintptr_t dllFn, void* fake) { cols.push_back({exeAt, Resolve(dllFn), fake}); };
    Add(0x619B60, std::bit_cast<uintptr_t>(&RsEventHandler), reinterpret_cast<void*>(&FRsEvent));
    Add(0x619560, std::bit_cast<uintptr_t>(&RsKeyboardEventHandler), reinterpret_cast<void*>(&FRsKey));
    Add(0x747820, std::bit_cast<uintptr_t>(&GTATranslateKey), reinterpret_cast<void*>(&FTranslateKey));
    Add(0x747CD0, std::bit_cast<uintptr_t>(&GTATranslateShiftKey), reinterpret_cast<void*>(&FTranslateShift));
    Add(0x507430, std::bit_cast<uintptr_t>(&CAudioEngine::PauseAllSounds), reinterpret_cast<void*>(&FPauseSounds));
    Add(0x507440, std::bit_cast<uintptr_t>(&CAudioEngine::ResumeAllSounds), reinterpret_cast<void*>(&FResumeSounds));
    Add(0x541A70, std::bit_cast<uintptr_t>(&CPad::Clear), reinterpret_cast<void*>(&FPadClear));
    Add(0x7043D0, std::bit_cast<uintptr_t>(&CPostEffects::SetupBackBufferVertex), reinterpret_cast<void*>(&FBackBuf));
    Add(0x747000, std::bit_cast<uintptr_t>(&WinInput::Shutdown), reinterpret_cast<void*>(&FShutdown));
    Add(0x745F90, std::bit_cast<uintptr_t>(&VideoPlayer::WaitState), reinterpret_cast<void*>(&FWaitState));
    Add(0x7466D0, std::bit_cast<uintptr_t>(&VideoPlayer::UpdateWindow), reinterpret_cast<void*>(&FUpdateWindow));
    Add(0x745FB0, std::bit_cast<uintptr_t>(&VideoPlayer::OnGraphNotify), reinterpret_cast<void*>(&FGraphNotify));
    Add(0x4D95C0, std::bit_cast<uintptr_t>(&CAEAudioHardware::IsStreamingFromDVD), reinterpret_cast<void*>(&FStreaming));
    Add(0x4D95D0, std::bit_cast<uintptr_t>(&CAEAudioHardware::GetDVDDriveLetter), reinterpret_cast<void*>(&FLetter));
    Add(0x4D95E0, std::bit_cast<uintptr_t>(&CAEAudioHardware::CheckDVD), reinterpret_cast<void*>(&FCheckDvd));
    Add(0x57C5E0, std::bit_cast<uintptr_t>(&CMenuManager::NoDiskInDriveMessage), reinterpret_cast<void*>(&FNoDisk));
    Add(0x5B1700, std::bit_cast<uintptr_t>(&CCutsceneMgr::SkipCutscene), reinterpret_cast<void*>(&FSkipCutscene));
    Add(0x7F9D50, std::bit_cast<uintptr_t>(&RwD3D9GetCurrentD3DDevice), reinterpret_cast<void*>(&FGetDevice));
    return cols;
}
// API de Win32 sustituidas por IAT mientras dura la llamada (EXE y DLL)
struct ApiSet {
    std::vector<std::unique_ptr<IatPatch>> v;
    ApiSet(HMODULE exe, HMODULE asi) { for (const auto& a : ApiFakes()) v.push_back(std::make_unique<IatPatch>(std::initializer_list<HMODULE>{exe, asi}, a.dll, a.name, a.fake)); }
    ~ApiSet() { while (!v.empty()) v.pop_back(); }
};
// Ranuras de la tabla de importacion de un modulo para cada API (sin parchear: se construye y se destruye el parche en el acto)
inline json CountSlots(HMODULE m) { json o = json::object(); for (const auto& a : ApiFakes()) { IatPatch p({m}, a.dll, a.name, a.fake); o[a.name] = p.slots.size(); } return o; }

struct Run { Gate gate{}; bool routeOK{}; };
template<class F> Run Execute(OriginalDependencies499::HookRoute& route, bool candidate, uintptr_t entry, uintptr_t asi, F gate) {
    Run run{};
    route.Invoke(candidate, [&] {
        run.routeOK = candidate ? ReversingProbe::JumpsToDll(entry) && ModuleOf(Resolve(entry)) == asi : !ReversingProbe::JumpsToDll(entry);
        run.gate = gate(); return 0;
    });
    return run;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "dependencies24a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "dependencies24a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("dependencies24a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done = true;
    gMainThread = GetCurrentThreadId(); InitFakes();
    const uintptr_t asi = ModuleOf(uintptr_t(&ModuleMarker)), gameExe = uintptr_t(GetModuleHandleA(nullptr));
    constexpr uintptr_t kEntry = 0x747EB0;
    // La entrada del EXE sigue siendo la que registra WinMain como lpfnWndProc (WinMain.cpp con SDL3; con la ventana nativa la DLL registra directamente __MainWndProc)
    OriginalDependencies499::HookRoute route("Win", "__MainWndProc", kEntry, &__MainWndProc);
    const bool ready = route.Registered() && ModuleOf(route.Body()) == asi;
    // Cuerpos de los colaboradores de la DLL: se calculan ANTES de crear los parches de entrada
    const auto cols = Collaborators();
    std::vector<Watch> watches;
    for (const auto& c : cols) { watches.emplace_back(c.exe, 16); if (c.dll != c.exe) watches.emplace_back(c.dll, 16); }
    auto watchesIntact = [&] { return std::ranges::all_of(watches, [](const auto& w) { return w.Intact(); }); };
    auto brokenWatches = [&] { json o = json::array(); for (const auto& w : watches) if (!w.Intact()) o.push_back({{"address", w.address}, {"expected", Hex(w.saved.data(), w.saved.size())}}); return o; };
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    // API de Win32: el EXE importa las 13 (ranuras fijas en 0x858xxx) y la DLL llama a las mismas por su propia tabla
    json apiExe = CountSlots(reinterpret_cast<HMODULE>(gameExe)), apiAsi = CountSlots(reinterpret_cast<HMODULE>(asi));
    bool apisOk = true; for (const auto& a : ApiFakes()) apisOk = apisOk && apiExe[a.name].get<size_t>() >= 1 && apiAsi[a.name].get<size_t>() >= 1;
    json exeSlots = json::object(); bool exeSlotsOk = true;
    for (const auto& a : ApiFakes()) { const uintptr_t real = uintptr_t(Real(a.dll, a.name)); const uintptr_t at = D32(a.exeSlot); exeSlots[a.name] = {{"slot", a.exeSlot}, {"value", at}, {"real", real}}; exeSlotsOk = exeSlotsOk && (at == real || !real); }
    constexpr unsigned total = kRoutes * kCasesPerRoute;
    json bodies = json::array(); for (const auto& c : cols) bodies.push_back({c.exe, c.dll});
    Write({{"event", "begin"}, {"registered_hook", ready}, {"functions", kRoutes}, {"pairs", total}, {"cases_per_route", kCasesPerRoute}, {"routes", kRoutes},
           {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"collaborators_stubbed", true}, {"bodies", {{"__MainWndProc", route.Body()}}}, {"collaborator_bodies", bodies},
           {"api_slots_exe", apiExe}, {"api_slots_asi", apiAsi}, {"apis_patchable", apisOk}, {"exe_iat", exeSlots}, {"exe_iat_matches_real", exeSlotsOk},
           {"asi_module", asi}, {"original_module", gameExe}, {"initial_fp", Hex(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    int pairs{}; bool passed = ready && apisOk;
    for (unsigned r = 0; r < kRoutes && passed; ++r) {
        const std::string name = std::string("Win::__MainWndProc[") + kRouteNames[r] + "]";
        int failures{};
        for (unsigned n = 0; n < kCasesPerRoute; ++n) {
            const Case c = MakeCase(r, uint64(r) * 1000003ull + n, n);
            const auto base = SnapshotWindows();
            SideResult sr[2]; json rows[2]; bool routeOk[2]{};
            for (int side = 0; side < 2; ++side) {
                const bool candidate = side == 1;
                RestoreWindows(base); Apply(c); MakePrime(c.fsw); RestoreFloatingState();
                const Run run = Execute(route, candidate, kEntry, asi, [&] {
                    PatchSet p; for (const auto& k : cols) { p.Add(k.exe, k.fake); if (k.dll != k.exe) p.Add(k.dll, k.fake); }
                    ApiSet apis(reinterpret_cast<HMODULE>(gameExe), reinterpret_cast<HMODULE>(asi));
                    return GateStd4(kEntry, c.hwnd, c.msg, c.wparam, LparamOf(c));
                });
                sr[side] = Capture(run.gate); routeOk[side] = run.routeOK;
                RestoreWindows(base); RestoreFloatingState();   // globales del juego intactas ANTES de escribir (Write bombea mensajes)
                json out = SideJson(c, sr[side]);
                const uintptr_t where = ModuleOf(FipOf(run.gate));
                out["fp_ip_route_ok"] = FipOf(run.gate) == 0 || where == (candidate ? asi : gameExe) || where == asi;
                out["stack_restored"] = run.gate.stack; out["nonvolatile_preserved"] = run.gate.nonvolatile; out["route_ok"] = run.routeOK;
                out["globals_restored"] = watchesIntact() && SnapshotWindows() == base; out["floating_state_restored"] = FloatingStateRestored();
                rows[side] = out;
                if (candidate && rows[0] != rows[1]) {
                    rows[0]["trace_full"] = TraceJson(sr[0].trace, true); rows[1]["trace_full"] = TraceJson(sr[1].trace, true);
                    size_t i = 0; while (i < sr[0].windows.size() && sr[0].windows[i] == sr[1].windows[i]) ++i; rows[1]["first_window_difference"] = i;
                }
            }
            Fp fa = sr[0].gate.img, fb = sr[1].gate.img; memset(fa.data() + 12, 0, 4); memset(fb.data() + 12, 0, 4);
            bool ok = rows[0] == rows[1] && sr[0].windows == sr[1].windows && sr[0].arena == sr[1].arena && sr[0].ps == sr[1].ps && sr[0].trace == sr[1].trace && sr[0].gate.eax == sr[1].gate.eax;
            for (const char* k : {"fp_ip_route_ok", "stack_restored", "nonvolatile_preserved", "route_ok", "globals_restored", "floating_state_restored"}) ok &= rows[0][k] == true && rows[1][k] == true;
            ok &= rows[0]["trace"]["well_formed"] == true;
            const json diff = FpDifference(fa, fb); ok &= diff.empty(); passed &= ok; failures += !ok;
            Write({{"event", "comparison"}, {"function", name}, {"case", n}, {"pair", pairs++}, {"original", rows[0]}, {"candidate", rows[1]}, {"fp_difference", diff}, {"dead_registers_equal", DeadEqual(fa, fb)}, {"equal", ok}, {"accepted", ok}});
        }
        Write({{"event", "function_summary"}, {"function", name}, {"cases", int(kCasesPerRoute)}, {"failures", failures}, {"state_restored", watchesIntact()}, {"broken_watches", brokenWatches()}});
    }
    // Las entradas se vigilan ANTES de Restore(); despues se restauran el hook y el estado x87
    const bool entriesIntactBefore = watchesIntact(); const json brokenBefore = brokenWatches();
    bool stateRestored = entriesIntactBefore; stateRestored &= route.Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"state_restored", stateRestored}, {"watched_intact", entriesIntactBefore}, {"broken_watches_before_restore", brokenBefore}});
    Write({{"event", passed && stateRestored ? "complete" : "error"}, {"pairs", pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntactBefore}});
    RestoreFloatingState(); savedFPActive = false;
}
}
