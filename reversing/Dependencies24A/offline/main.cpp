// Ensayo SIN juego del banco Dependencies24A (Win::__MainWndProc 0x747EB0). No sustituye a la corrida dentro del juego, que NO se ha ejecutado.
// Mapea los bytes REALES del EXE (secciones del fichero) en 0x400000 dentro de este proceso (32 bits, base 0x10000000) y llama a la entrada 0x747EB0 de esa copia frente a la funcion de la DLL
// (WndProc.cpp compilado con las opciones del proyecto; __MainWndProc de WndProc_h.cpp). Los colaboradores del juego se sustituyen por los mismos falsos que usa el banco real (Core.hpp):
// en la copia del EXE con un `jmp` en la entrada de cada funcion y las ranuras de su IAT; en la DLL, porque los simbolos del proyecto estan definidos aqui como envoltorios de los falsos, y la IAT de
// este ejecutable parcheada por valor. Las globales (0xC920E8 ...) estan en el mismo espacio de direcciones, de modo que los StaticRef<T> del proyecto y el EXE ven la misma memoria.
#include "StdInc.h"
#include <Dbt.h>
#include <dshow.h>
#include <cstdio>
#include <fstream>
#include "WinInput.h"
#include "AEAudioHardware.h"
#include "VideoPlayer.h"
#include "PostEffects.h"
#include "CutsceneMgr.h"
#include "MenuManager.h"
#include "AudioEngine.h"
#include "Pad.h"
#include "../Core.hpp"

using namespace Dependencies24ALive;

// ---- simbolos del proyecto (los que referencia WndProc.cpp) = envoltorios de los falsos ----
RsEventStatus RsEventHandler(RsEvent event, void* param) { return RsEventStatus(FRsEvent(int(event), param)); }
RsEventStatus RsKeyboardEventHandler(RsEvent event, void* param) { return RsEventStatus(FRsKey(int(event), param)); }
BOOL GTATranslateKey(RsKeyCodes* ck, LPARAM lParam, UINT vk) { return FTranslateKey(reinterpret_cast<uint32*>(ck), uint32(lParam), vk); }
BOOL GTATranslateShiftKey(RsKeyCodes* ck) { return FTranslateShift(reinterpret_cast<uint32*>(ck)); }
void CAudioEngine::PauseAllSounds() { FPauseSounds(); }
void CAudioEngine::ResumeAllSounds() { FResumeSounds(); }
void CPad::Clear(bool a, bool b) { FPadClear(this, nullptr, a, b); }
void CPostEffects::SetupBackBufferVertex() { FBackBuf(); }
namespace WinInput { HRESULT Shutdown() { return HRESULT(FShutdown()); } }
namespace VideoPlayer {
void WaitState(int32 state) { FWaitState(state); }
long OnGraphNotify() { return FGraphNotify(); }
long UpdateWindow() { return FUpdateWindow(); }
auto GetMediaControl() -> IMediaControl* { return *reinterpret_cast<IMediaControl**>(Addr::kMediaControl); }
}
bool CAEAudioHardware::IsStreamingFromDVD() { return FStreaming(this, nullptr); }
char CAEAudioHardware::GetDVDDriveLetter() { return FLetter(this, nullptr); }
bool CAEAudioHardware::CheckDVD() { return FCheckDvd(this, nullptr); }
void CMenuManager::NoDiskInDriveMessage() { FNoDisk(this, nullptr); }
void CCutsceneMgr::SkipCutscene() { FSkipCutscene(); }
void* RwD3D9GetCurrentD3DDevice() { return FGetDevice(); }
CMenuManager& FrontEndMenuManager = *reinterpret_cast<CMenuManager*>(Addr::kFem);
CAEAudioHardware& AEAudioHardware = *reinterpret_cast<CAEAudioHardware*>(Addr::kAudioHw);

extern LRESULT CALLBACK __MainWndProc(HWND, UINT, WPARAM, LPARAM);

// La imagen del EXE se mapea en un proceso hijo SUSPENDIDO de este mismo ejecutable (antes de que el cargador reparta direcciones bajas): en el proceso normal 0x400000.. ya esta ocupado.
static bool MapExeInto(HANDLE proc, const char* path) {
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) { std::printf("no se pudo abrir %s\n", path); return false; }
    const DWORD size = GetFileSize(f, nullptr); std::vector<uint8> img(size); DWORD got{};
    ReadFile(f, img.data(), size, &got, nullptr); CloseHandle(f);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(img.data()); auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(img.data() + dos->e_lfanew);
    void* base = VirtualAllocEx(proc, reinterpret_cast<void*>(nt->OptionalHeader.ImageBase), nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (base != reinterpret_cast<void*>(nt->OptionalHeader.ImageBase)) { std::printf("no se pudo reservar la imagen en 0x%08X (tamano 0x%X)\n", nt->OptionalHeader.ImageBase, nt->OptionalHeader.SizeOfImage); return false; }
    SIZE_T written{};
    WriteProcessMemory(proc, base, img.data(), nt->OptionalHeader.SizeOfHeaders, &written);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i)
        if (!WriteProcessMemory(proc, reinterpret_cast<uint8*>(base) + sec[i].VirtualAddress, img.data() + sec[i].PointerToRawData, std::min<DWORD>(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize), &written)) return false;
    return true;
}
static int Launcher(int argc, char** argv) {
    char self[MAX_PATH]; GetModuleFileNameA(nullptr, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" --child";
    for (int i = 1; i < argc; ++i) cmd += std::string(" \"") + argv[i] + "\"";
    STARTUPINFOA si{}; si.cb = sizeof(si); PROCESS_INFORMATION pi{};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { std::printf("CreateProcess fallo\n"); return 2; }
    if (!MapExeInto(pi.hProcess, argc > 1 ? argv[1] : "D:\\Grand Theft Auto San Andreas\\gta-sa.exe")) { TerminateProcess(pi.hProcess, 3); return 3; }
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code{}; GetExitCodeProcess(pi.hProcess, &code);
    return int(code);
}
static volatile unsigned gRoute, gCase; static volatile int gSide;
static LONG CALLBACK Veh(PEXCEPTION_POINTERS e) {
    if (e->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        std::printf("EXCEPCION acceso invalido: eip=%08X dir=%08X tipo=%u  ruta %u caso %u lado %d  eax=%08X ecx=%08X edx=%08X esp=%08X\n", unsigned(e->ContextRecord->Eip), unsigned(e->ExceptionRecord->ExceptionInformation[1]),
                    unsigned(e->ExceptionRecord->ExceptionInformation[0]), gRoute, gCase, gSide, unsigned(e->ContextRecord->Eax), unsigned(e->ContextRecord->Ecx), unsigned(e->ContextRecord->Edx), unsigned(e->ContextRecord->Esp));
        ExitProcess(9);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
static void PatchJmp(uintptr_t at, void* to) { auto* p = reinterpret_cast<uint8*>(at); p[0] = 0xE9; const int32 rel = int32(uintptr_t(to) - at - 5); std::memcpy(p + 1, &rel, 4); }

int main(int argc, char** argv) {
    // Uso: h.exe [ruta del gta-sa.exe] [casos por ruta]   (el proceso hijo recibe --child delante)
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    AddVectoredExceptionHandler(1, Veh);
    const unsigned perRoute = argc > 3 ? unsigned(std::atoi(argv[3])) : kCasesPerRoute;
    gMainThread = GetCurrentThreadId(); InitFakes();
    // EXE: colaboradores
    const struct { uintptr_t at; void* fake; } exeFakes[]{
        {0x619B60, reinterpret_cast<void*>(&FRsEvent)}, {0x619560, reinterpret_cast<void*>(&FRsKey)}, {0x747820, reinterpret_cast<void*>(&FTranslateKey)}, {0x747CD0, reinterpret_cast<void*>(&FTranslateShift)},
        {0x507430, reinterpret_cast<void*>(&FPauseSounds)}, {0x507440, reinterpret_cast<void*>(&FResumeSounds)}, {0x541A70, reinterpret_cast<void*>(&FPadClear)}, {0x7043D0, reinterpret_cast<void*>(&FBackBuf)},
        {0x747000, reinterpret_cast<void*>(&FShutdown)}, {0x745F90, reinterpret_cast<void*>(&FWaitState)}, {0x7466D0, reinterpret_cast<void*>(&FUpdateWindow)}, {0x745FB0, reinterpret_cast<void*>(&FGraphNotify)},
        {0x4D95C0, reinterpret_cast<void*>(&FStreaming)}, {0x4D95D0, reinterpret_cast<void*>(&FLetter)}, {0x4D95E0, reinterpret_cast<void*>(&FCheckDvd)}, {0x57C5E0, reinterpret_cast<void*>(&FNoDisk)},
        {0x5B1700, reinterpret_cast<void*>(&FSkipCutscene)}, {0x7F9D50, reinterpret_cast<void*>(&FGetDevice)}};
    for (const auto& f : exeFakes) PatchJmp(f.at, f.fake);
    const auto apis = ApiFakes();
    for (const auto& a : apis) D32(a.exeSlot) = uint32(uintptr_t(a.fake));                       // IAT de la copia del EXE
    std::vector<std::unique_ptr<IatPatch>> iat;                                                  // IAT de este ejecutable (lado DLL)
    for (const auto& a : apis) iat.push_back(std::make_unique<IatPatch>(std::initializer_list<HMODULE>{GetModuleHandleA(nullptr)}, a.dll, a.name, a.fake));
    size_t patched = 0; for (const auto& p : iat) patched += p->slots.size();
    std::printf("IAT del ensayo: %zu ranuras parcheadas para %zu API\n", patched, apis.size());

    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const uintptr_t exeEntry = 0x747EB0, dllEntry = reinterpret_cast<uintptr_t>(&__MainWndProc);
    unsigned long total = 0, bad = 0; unsigned long evCount[EvMax]{}; unsigned long perRouteBad[kRoutes]{}; static unsigned long cov[kRoutes][EvMax];
    // Opcional (cuarto argumento): registro .jsonl con el MISMO formato que el banco dentro del juego, para ensayar run_local.py --analyze sin arrancar GTA.
    std::ofstream jsonl; if (argc > 4) jsonl.open(argv[4]);
    auto Row = [&](const json& j) { if (jsonl.is_open()) jsonl << j.dump() << '\n'; };
    {
        json slots = json::object(); for (const auto& a : apis) slots[a.name] = 1;
        Row({{"event", "begin"}, {"registered_hook", true}, {"functions", kRoutes}, {"pairs", kRoutes * perRoute}, {"cases_per_route", perRoute}, {"routes", kRoutes}, {"compare_dead_x87_registers", kCompareDeadX87Registers},
             {"collaborators_stubbed", true}, {"bodies", {{"__MainWndProc", dllEntry}}}, {"api_slots_exe", slots}, {"api_slots_asi", slots}, {"apis_patchable", true}, {"exe_iat", json::object()}, {"exe_iat_matches_real", true},
             {"ensayo_sin_juego", true}});
    }
    unsigned pairIndex = 0;
    for (unsigned r = 0; r < kRoutes; ++r) {
        for (unsigned n = 0; n < perRoute; ++n) {
            const Case c = MakeCase(r, uint64(r) * 1000003ull + n, n);
            SideResult side[2]; json js[2];
            const auto saved = SnapshotWindows();
            for (int s = 0; s < 2; ++s) {
                gRoute = r; gCase = n; gSide = s;
                RestoreWindows(saved); Apply(c); MakePrime(c.fsw); RestoreFloatingState();
                const Gate g = GateStd4(s ? dllEntry : exeEntry, c.hwnd, c.msg, c.wparam, LparamOf(c));
                side[s] = Capture(g); RestoreFloatingState(); RestoreWindows(saved);
                js[s] = SideJson(c, side[s]);
                js[s]["stack_restored"] = g.stack; js[s]["nonvolatile_preserved"] = g.nonvolatile;
            }
            Fp fa = side[0].gate.img, fb = side[1].gate.img; memset(fa.data() + 12, 0, 4); memset(fb.data() + 12, 0, 4);
            const json diff = FpDifference(fa, fb);
            bool ok = js[0] == js[1] && side[0].trace == side[1].trace && side[0].windows == side[1].windows && side[0].arena == side[1].arena && diff.empty()
                      && js[0]["stack_restored"] == true && js[1]["stack_restored"] == true && js[0]["nonvolatile_preserved"] == true && js[1]["nonvolatile_preserved"] == true
                      && js[0]["trace"]["well_formed"] == true;
            ++total;
            if (jsonl.is_open()) {
                json rows[2] = {js[0], js[1]};
                for (auto& rw : rows) { for (const char* k : {"fp_ip_route_ok", "route_ok", "globals_restored", "floating_state_restored"}) rw[k] = true; }
                Row({{"event", "comparison"}, {"function", std::string("Win::__MainWndProc[") + kRouteNames[r] + "]"}, {"case", n}, {"pair", pairIndex}, {"original", rows[0]}, {"candidate", rows[1]},
                     {"fp_difference", diff}, {"dead_registers_equal", DeadEqual(fa, fb)}, {"equal", ok}, {"accepted", ok}});
            }
            ++pairIndex;
            { bool seen[EvMax]{}; for (size_t i = 0; i < side[0].trace.size();) { const uint32 k = side[0].trace[i]; if (k < EvMax && kTraceLen[k]) { ++evCount[k]; seen[k] = true; i += kTraceLen[k]; } else break; } for (unsigned k = 1; k < EvMax; ++k) cov[r][k] += seen[k]; }
            if (!ok) {
                ++bad; ++perRouteBad[r];
                if (bad <= 12) {
                    std::printf("DIFF route %u (%s) case %u msg=%08X wparam=%08X lparam=%08X\n  eax %08X %08X  trace %zu %zu  windows_eq=%d arena_eq=%d fp=%s\n", r, kRouteNames[r], n, c.msg, c.wparam, LparamOf(c),
                        side[0].gate.eax, side[1].gate.eax, side[0].trace.size(), side[1].trace.size(), side[0].windows == side[1].windows, side[0].arena == side[1].arena, diff.dump().c_str());
                    const size_t m = std::max(side[0].trace.size(), side[1].trace.size());
                    for (size_t i = 0; i < m; ++i) { const uint32 a = i < side[0].trace.size() ? side[0].trace[i] : 0xDEAD, b = i < side[1].trace.size() ? side[1].trace[i] : 0xDEAD; if (a != b) { std::printf("  trace[%zu]: exe=%08X dll=%08X\n", i, a, b); break; } }
                    if (side[0].windows != side[1].windows) for (size_t i = 0; i < side[0].windows.size(); ++i) if (side[0].windows[i] != side[1].windows[i]) { std::printf("  window byte %zu: %02X %02X\n", i, side[0].windows[i], side[1].windows[i]); break; }
                }
            }
        }
    }
    for (unsigned r = 0; r < kRoutes; ++r) Row({{"event", "function_summary"}, {"function", std::string("Win::__MainWndProc[") + kRouteNames[r] + "]"}, {"cases", perRoute}, {"failures", perRouteBad[r]}, {"state_restored", true}});
    Row({{"event", "global_state_final"}, {"state_restored", true}, {"watched_intact", true}});
    Row({{"event", bad ? "error" : "complete"}, {"pairs", total}, {"state_restored", true}, {"floating_state_restored", true}, {"entries_intact", true}});
    std::printf("TOTAL %lu pares, %lu diferencias\n", total, bad);
    for (unsigned r = 0; r < kRoutes; ++r) std::printf("  ruta %2u %-26s diferencias %lu\n", r, kRouteNames[r], perRouteBad[r]);
    std::printf("cobertura (pares de cada ruta en los que aparece cada tipo de evento; tipo:pares):\n");
    for (unsigned r = 0; r < kRoutes; ++r) { std::printf("  %2u {", r); for (unsigned k = 1; k < EvMax; ++k) if (cov[r][k]) std::printf("%u: %lu, ", k, cov[r][k]); std::printf("}\n"); }
    std::printf("eventos (lado EXE):"); for (unsigned k = 1; k < EvMax; ++k) if (evCount[k]) std::printf(" %u:%lu", k, evCount[k]); std::printf("\n");
    return bad != 0;
}
