// Arnes sin juego de diPadInit (0x7485C0): los bytes REALES del EXE (proceso hijo con la imagen mapeada en 0x400000) frente a la reconstruccion C++ de WinInput.cpp
// (el texto de la funcion se extrae del fuente con extract.py -> dipad_cpp.inc, sin cambios), con los mismos colaboradores falsos de Fixtures.hpp.
// Escribe un comparisons.jsonl con el formato del banco real, que run_local.py --analyze valida contra el oraculo (fixture_oracle.py).
#define NOMINMAX
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include <intrin.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <bit>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using uint32 = std::uint32_t;
using uint8 = std::uint8_t;
#include "../Rows.hpp"
using namespace Dependencies24BFix;

// ---------------------------------------------------------------- imagen del EXE
static bool MapExeInto(HANDLE proc) {
    HANDLE f = CreateFileA("D:/Grand Theft Auto San Andreas/gta-sa.exe", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    const DWORD size = GetFileSize(f, nullptr);
    std::vector<u8> raw(size);
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
static void Hook(uintptr_t at, void* fn) {
    auto* p = reinterpret_cast<u8*>(at);
    DWORD old{}; VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old);
    p[0] = 0xE9;
    const u32 rel = u32(reinterpret_cast<uintptr_t>(fn) - at - 5);
    std::memcpy(p + 1, &rel, 4);
    FlushInstructionCache(GetCurrentProcess(), p, 5);
}
static uintptr_t Resolve(uintptr_t address) { for (unsigned i = 0; i < 16 && *reinterpret_cast<u8*>(address) == 0xE9; ++i) address += 5 + *reinterpret_cast<int32_t*>(address + 1); return address; }

// ---------------------------------------------------------------- entorno minimo del juego para el C++ de WinInput.cpp
struct psGlobalType { HWND window; HINSTANCE instance; BOOL fullScreen; float lastMousePos[2]; u32 lastRenderTime; LPDIRECTINPUT8 diInterface; LPDIRECTINPUTDEVICE8 diMouse; LPDIRECTINPUTDEVICE8 diDevice1; LPDIRECTINPUTDEVICE8 diDevice2; };
static_assert(sizeof(psGlobalType) == 0x28);
struct RsGlobalType { char pad[0x14]; psGlobalType* ps; };
static RsGlobalType& RsGlobal = *reinterpret_cast<RsGlobalType*>(0xC17040);
struct JoyStruct { u32 wDeviceID; bool bJoyAttachedToPort; bool bZAxisPresent; bool bZRotPresent; DWORD wVendorID; DWORD wProductID; };
struct CJoySticks {
    JoyStruct JoyStickNum[2];
    __declspec(noinline) void ClearJoyInfo(int n) { JoyStickNum[n].wDeviceID = 0; JoyStickNum[n].bJoyAttachedToPort = false; JoyStickNum[n].bZAxisPresent = false; JoyStickNum[n].bZRotPresent = false; }
};
static_assert(sizeof(CJoySticks) == 0x20);
static CJoySticks& AllValidWinJoys = *reinterpret_cast<CJoySticks*>(0xC92144);
struct CControllerConfigManager {
    __declspec(noinline) void InitDefaultControlConfigJoyPad(u32 buttons) { volatile u32 sink = buttons; (void)sink; }   // cuerpo propio: el falso lo sustituye con EntryPatch
};
static CControllerConfigManager& ControlsManager = *reinterpret_cast<CControllerConfigManager*>(0xB70198);
namespace plugin { template<uintptr_t A> void Call() { reinterpret_cast<void(__cdecl*)()>(A)(); } }

namespace WinInput {
#include "dipad_cpp.inc"
}

static std::ofstream out;
static void Write(const json& j) { out << j.dump() << '\n'; out.flush(); }

static int Child(int argc, char** argv) {
    const unsigned cases = argc > 2 ? unsigned(std::atoi(argv[2])) : kCases;
    const char* path = argc > 3 ? argv[3] : "comparisons.jsonl";
    constexpr uintptr_t kArena = 0x30000000;
    if (!VirtualAlloc(reinterpret_cast<void*>(kArena), 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) { std::printf("no se pudo reservar el ps falso\n"); return 4; }
    std::memset(reinterpret_cast<void*>(kArena), 0, 0x1000);
    *reinterpret_cast<u32*>(kPsPtr) = u32(kArena);
    InitVtable();
    out.open(path);
    const uintptr_t initDll = Resolve(std::bit_cast<uintptr_t>(&CControllerConfigManager::InitDefaultControlConfigJoyPad));
    const uintptr_t dllEntry = Resolve(std::bit_cast<uintptr_t>(&WinInput::diPadInit));
    const Saved saved = SaveGlobals();
    // Los falsos quedan puestos durante todo el arnes (en el EXE: 0x7477C0 y 0x530B00; en el "DLL": el cuerpo de InitDefaultControlConfigJoyPad).
    Hook(kEnumExe, reinterpret_cast<void*>(&StubEnum));
    Hook(kInitExe, reinterpret_cast<void*>(&StubInit));
    Hook(initDll, reinterpret_cast<void*>(&StubInit));
    Write({{"event", "begin"}, {"registered_hook", true}, {"collaborators_stubbed", true}, {"functions", 1}, {"pairs", cases}, {"cases", {{"diPadInit", cases}}},
           {"compare_dead_x87_registers", true}, {"bodies", {{"diPadInit", dllEntry}}}, {"initial_table", HexBytes(saved.table.data(), saved.table.size())},
           {"initial_dev1", saved.dev1}, {"initial_dev2", saved.dev2}});
    int pairs = 0, failures = 0;
    static std::array<Fix, kCases> fixtures{};
    for (unsigned c = 0; c < cases; ++c) {
        fixtures[c] = Make(c);
        const Fix& fx = fixtures[c];
        json rows[2]; Reg regs[2];
        for (int side = 0; side < 2; ++side) {
            Prepare(fx);
            regs[side] = GateCdecl0(side == 0 ? kEntry : u32(dllEntry), fx.eax, fx.ecx, fx.edx, fx.stack_seed);
            const Observed obs = Observe();
            json o = {{"input", InputRow(c, fx)}, {"observed", ObservedRow(obs)}, {"post_floating_state", ""}, {"post_mxcsr", 0}, {"stack_restored", regs[side].stack},
                      {"nonvolatile_preserved", regs[side].nonvolatile}, {"route_ok", true}, {"traps_zero", obs.traps == 0}};
            RestoreGlobals(saved);
            o["globals_restored"] = GlobalsEqual(saved); o["floating_state_restored"] = true;
            rows[side] = o;
        }
        const bool ok = rows[0] == rows[1];
        failures += !ok;
        Write({{"event", "comparison"}, {"function", "WinInput::diPadInit"}, {"case", c}, {"pair", pairs++}, {"original", rows[0]}, {"candidate", rows[1]}, {"fp_difference", json::array()},
               {"original_residue", {{"eax", regs[0].eax}, {"ecx", regs[0].ecx}, {"edx", regs[0].edx}}}, {"candidate_residue", {{"eax", regs[1].eax}, {"ecx", regs[1].ecx}, {"edx", regs[1].edx}}},
               {"equal", ok}, {"accepted", ok}});
    }
    Write({{"event", "function_summary"}, {"function", "WinInput::diPadInit"}, {"cases", int(cases)}, {"failures", failures}, {"state_restored", true}});
    Write({{"event", "global_state_final"}, {"table", HexBytes(reinterpret_cast<void*>(kTable), kTableSize)}, {"dev1", PsField(kPsDev1)}, {"dev2", PsField(kPsDev2)}, {"state_restored", true}});
    Write({{"event", failures ? "error" : "complete"}, {"pairs", pairs}, {"state_restored", true}, {"floating_state_restored", true}, {"entries_intact", true}});
    std::printf("diPadInit: %d pares, %d diferencias\n", pairs, failures);
    return failures ? 1 : 0;
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--child") return Child(argc, argv);
    return Launcher(argc, argv);
}
