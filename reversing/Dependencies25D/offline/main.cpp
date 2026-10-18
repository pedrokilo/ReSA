// Arnes sin juego del banco Dependencies25D: los bytes REALES del EXE (proceso hijo con la imagen mapeada en 0x400000) frente a la reconstruccion C++ de rwcore.cpp / rwplcore.cpp
// (el texto de las funciones se extrae del fuente con extract.py -> rw_cpp.inc, sin cambios), con los mismos falsos y escenas de Scenes.hpp que el banco real.
// Escribe un comparisons.jsonl con el formato del banco real, que `run_local.py --analyze` valida contra el oraculo independiente (fixture_oracle.py).
// No se compara el estado x87 (el hijo no tiene el ASI ni PrimeX87): esa compuerta solo existe dentro del juego.
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <bit>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using uint8 = std::uint8_t;
using uint32 = std::uint32_t;
using int32 = std::int32_t;
using uint16 = std::uint16_t;

// ---------------------------------------------------------------- tipos minimos de RenderWare (mismos desplazamientos que rwplcore.h / rwcore.h)
using RwInt32 = std::int32_t; using RwUInt32 = std::uint32_t; using RwBool = std::int32_t; using RwUInt8 = std::uint8_t; using RwChar = char;
struct RwRaster; struct RwImage; struct RwFreeList; struct RwPluginRegistry { int dummy; };
struct RwRGBA { uint8 red, green, blue, alpha; };
typedef RwRaster* (*RwTextureCallBackMipmapGeneration)(RwRaster*, RwImage*);
typedef RwBool (*RwStandardFunc)(void*, void*, RwInt32);
typedef void* RwPluginObjectConstructor; typedef void* RwPluginObjectDestructor; typedef void* RwPluginObjectCopy;
typedef void* RwPluginDataChunkReadCallBack; typedef void* RwPluginDataChunkWriteCallBack; typedef void* RwPluginDataChunkGetSizeCallBack;
struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwObject { uint8 type, subType, flags, privateFlags; void* parent; };
struct RwObjectHasFrame { RwObject object; RwLLLink lFrame; void* sync; };
#define rwLinkListRemoveLLLink(linkvar) ( ((linkvar)->prev)->next = (linkvar)->next, ((linkvar)->next)->prev = (linkvar)->prev )
#define rwSTANDARDRGBTOPIXEL 2
#define rwSTANDARDRASTERLOCKPALETTE 23
struct RwGlobals { uint8 head[0x48]; RwStandardFunc stdFunc[29]; };
static_assert(offsetof(RwGlobals, stdFunc) == 0x48);
template<class T> T& StaticRef(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }
static auto& RwEngineInstance = StaticRef<RwGlobals*>(0xC97B24);

namespace Dll {
#include "rw_cpp.inc"
}

#include "../Scenes.hpp"
using namespace Dependencies25DScene;

// ---------------------------------------------------------------- imagen del EXE
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
        const std::uintptr_t va = nt->OptionalHeader.ImageBase + sec[i].VirtualAddress;
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
static void Hook(std::uintptr_t at, void* fn) {
    auto* p = reinterpret_cast<uint8*>(at);
    DWORD old{}; VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old);
    p[0] = 0xE9;
    const uint32 rel = uint32(reinterpret_cast<std::uintptr_t>(fn) - at - 5);
    std::memcpy(p + 1, &rel, 4);
    FlushInstructionCache(GetCurrentProcess(), p, 5);
}
static std::uintptr_t Resolve(std::uintptr_t address) { for (unsigned i = 0; i < 16 && *reinterpret_cast<uint8*>(address) == 0xE9; ++i) address += 5 + *reinterpret_cast<int32*>(address + 1); return address; }

static std::ofstream out;
static void Write(const json& j) { out << j.dump() << '\n'; out.flush(); }

static int Child(int argc, char** argv) {
    const unsigned cases = argc > 2 ? unsigned(std::atoi(argv[2])) : kCases;
    const char* path = argc > 3 ? argv[3] : "comparisons.jsonl";
    const unsigned only = argc > 4 ? unsigned(std::atoi(argv[4])) : 0xFFFFFFFFu;   // opcional: solo la funcion de ese indice
    out.open(path);
    InitVtable();
    const std::array<std::uintptr_t, 20> candidates{
        std::bit_cast<std::uintptr_t>(&Dll::RwTextureGetMipmapping), std::bit_cast<std::uintptr_t>(&Dll::RwTextureGetAutoMipmapping), std::bit_cast<std::uintptr_t>(&Dll::RwTextureRegisterPlugin),
        std::bit_cast<std::uintptr_t>(&Dll::RwTexDictionaryRegisterPlugin), std::bit_cast<std::uintptr_t>(&Dll::RwTextureRasterGenerateMipmaps), std::bit_cast<std::uintptr_t>(&Dll::RwD3D9DynamicVertexBufferDestroy),
        std::bit_cast<std::uintptr_t>(&Dll::RwD3D9DynamicVertexBufferUnlock), std::bit_cast<std::uintptr_t>(&Dll::_rwD3D9SetDepthStencilSurface), std::bit_cast<std::uintptr_t>(&Dll::_rwDeviceRegisterPlugin),
        std::bit_cast<std::uintptr_t>(&Dll::RwD3D9DeleteVertexShader), std::bit_cast<std::uintptr_t>(&Dll::RwD3D9CreatePixelShader), std::bit_cast<std::uintptr_t>(&Dll::RwD3D9DeletePixelShader),
        std::bit_cast<std::uintptr_t>(&Dll::RwRasterGetCurrentContext), std::bit_cast<std::uintptr_t>(&Dll::RwRasterRegisterPlugin), std::bit_cast<std::uintptr_t>(&Dll::RwRasterLockPalette),
        std::bit_cast<std::uintptr_t>(&Dll::RwFreeListCreate), std::bit_cast<std::uintptr_t>(&Dll::RwFreeListCreateAndPreallocateSpace), std::bit_cast<std::uintptr_t>(&Dll::RwRGBAToPixel),
        std::bit_cast<std::uintptr_t>(&Dll::RwTextureRegisterPluginStream), std::bit_cast<std::uintptr_t>(&Dll::_rwObjectHasFrameReleaseFrame)};
    // Falsos permanentes de los colaboradores del EXE (los cuatro que llaman estas funciones por direccion).
    Hook(0x8084A0, reinterpret_cast<void*>(&StubAddPlugin)); Hook(0x8088E0, reinterpret_cast<void*>(&StubAddStream));
    Hook(0x8019B0, reinterpret_cast<void*>(&StubFree)); Hook(0x4C9AB0, reinterpret_cast<void*>(&StubDeviceReg));
    const EngineGlobals saved = ReadGlobals();
    json casesMap = json::object(), bodies = json::object();
    for (size_t i = 0; i < kSpecs.size(); ++i) { casesMap[kSpecs[i].name] = cases; bodies[kSpecs[i].name] = Resolve(candidates[i]); }
    unsigned selected = 0;
    for (size_t i = 0; i < kSpecs.size(); ++i) selected += (only == 0xFFFFFFFFu || only == i);
    Write({{"event", "begin"}, {"registered_hook", true}, {"collaborators_stubbed", true}, {"functions", int(selected)}, {"pairs", selected * cases}, {"cases", casesMap},
           {"compare_dead_x87_registers", true}, {"bodies", bodies}, {"offline", true}, {"arena_base", Addr(0)}, {"arena_size", kMem}, {"initial_globals", GlobalsJson(saved)}});
    int pairs = 0, totalFailures = 0;
    for (size_t fi = 0; fi < kSpecs.size(); ++fi) {
        if (only != 0xFFFFFFFFu && only != fi) continue;
        const FnSpec& spec = kSpecs[fi];
        int failures = 0;
        for (unsigned c = 0; c < cases; ++c) {
            json rows[2]; Reg regs[2]; json input; std::string beforeWindow;
            for (int side = 0; side < 2; ++side) {
                CaseScene scene; traceLog.clear(); trapCount = 0; spec.build(c, scene);
                WriteGlobals(scene.g);
                const Snap before = Snapshot(scene);
                if (side == 0) { input = scene.input; input["window_lo"] = scene.winLo; input["window_len"] = scene.winLen; input["globals_in"] = GlobalsJson(scene.g);
                                 input["seeds"] = {scene.seeds.eax, scene.seeds.ecx, scene.seeds.edx}; input["args"] = std::vector<uint32>(scene.args, scene.args + 6); beforeWindow = before.window; }
                // los falsos de scene.patches (colaboradores del EXE) ya estan puestos de forma permanente: son los mismos para todas las funciones
                regs[side] = GateCdecl6(side == 0 ? spec.entry : candidates[fi], scene.args, scene.seeds);
                const EngineGlobals after = ReadGlobals();
                json o = ObserveJson(scene, before, regs[side], spec.voidFn, after);
                WriteGlobals(saved);
                o["post_floating_state"] = ""; o["post_mxcsr"] = 0; o["fp_ip_route_ok"] = true; o["stack_restored"] = regs[side].stack; o["nonvolatile_preserved"] = regs[side].nonvolatile;
                o["route_ok"] = true; o["globals_restored"] = GlobalsEqual(ReadGlobals(), saved); o["floating_state_restored"] = true;
                rows[side] = o;
            }
            bool ok = rows[0] == rows[1];
            for (const char* k : {"stack_restored", "nonvolatile_preserved", "globals_restored"}) ok &= rows[0][k] == true && rows[1][k] == true;
            ok &= rows[0]["outside_unchanged"] == true && rows[0]["traps"] == 0;
            failures += !ok;
            Write({{"event", "comparison"}, {"function", spec.name}, {"case", c}, {"pair", pairs++}, {"input", input}, {"before", beforeWindow}, {"original", rows[0]}, {"candidate", rows[1]},
                   {"fp_difference", json::array()}, {"original_residue", {{"eax", regs[0].eax}, {"ecx", regs[0].ecx}, {"edx", regs[0].edx}}},
                   {"candidate_residue", {{"eax", regs[1].eax}, {"ecx", regs[1].ecx}, {"edx", regs[1].edx}}}, {"equal", ok}, {"accepted", ok}});
        }
        Write({{"event", "function_summary"}, {"function", spec.name}, {"cases", int(cases)}, {"failures", failures}, {"state_restored", true}});
        std::printf("%-48s %u pares, %d diferencias\n", spec.name, cases, failures);
        totalFailures += failures;
    }
    Write({{"event", "global_state_final"}, {"state_restored", true}, {"watched_intact", true}, {"globals", GlobalsJson(ReadGlobals())}});
    Write({{"event", totalFailures ? "error" : "complete"}, {"pairs", pairs}, {"state_restored", true}, {"floating_state_restored", true}, {"entries_intact", true}});
    std::printf("total: %d pares, %d diferencias\n", pairs, totalFailures);
    return totalFailures ? 1 : 0;
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--child") return Child(argc, argv);
    return Launcher(argc, argv);
}
