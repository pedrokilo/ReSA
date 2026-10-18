// Arnes sin juego de Dependencies25B: los bytes REALES del EXE (proceso hijo con la imagen mapeada en 0x400000) frente al C++ de rwcore.cpp / rpanisot.cpp / rpworld.cpp
// (texto extraido sin cambios por extract.py -> cand.inc), con los mismos colaboradores falsos, fixtures y gates de Bench.hpp que el banco dentro del juego.
// Escribe un comparisons.jsonl con el formato del banco real, que `run_local.py --analyze` valida contra el oraculo independiente de Python.
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <algorithm>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "rwmock.h"
#include "../Bench.hpp"
using namespace Dependencies25BFix;

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

// ---------------------------------------------------------------- C++ de los fuentes (sin cambios)
#include "cand.inc"

static void ModuleMarker() {}
static bool JumpsToDllHere(std::uintptr_t a) { return *reinterpret_cast<u8*>(a) == 0xE9 && ModuleOf(Resolve(a)) == ModuleOf(std::uintptr_t(&ModuleMarker)); }
// "Ruta": el candidato es el C++ de arriba, al que se salta desde la entrada del EXE (equivale a encender el hook); el original es el EXE tal cual.
struct Route {
    std::uintptr_t entry, body;
    template <class F> auto Invoke(bool candidate, F call) {
        if (candidate) { Patch p(entry, reinterpret_cast<void*>(body)); return call(); }
        return call();
    }
    bool Registered() const { return true; }
};
static std::ofstream out;
static void Write(const json& j) { out << j.dump() << '\n'; out.flush(); }

static int Child(int argc, char** argv) {
    const unsigned cases = argc > 2 ? unsigned(std::atoi(argv[2])) : kCases;
    const char* path = argc > 3 ? argv[3] : "comparisons.jsonl";
    out.open(path);
    Env env{};
    env.asi = ModuleOf(std::uintptr_t(&ModuleMarker)); env.exe = 0x400000; env.jumpsToDll = &JumpsToDllHere;
    std::array<std::uintptr_t, kFunctions> bodies{};
    bodies[IndexBuf] = std::bit_cast<std::uintptr_t>(&RwD3D9IndexBufferCreate);
    bodies[MaxSupported] = std::bit_cast<std::uintptr_t>(&RpAnisotGetMaxSupportedMaxAnisotropy);
    bodies[AnisoSet] = std::bit_cast<std::uintptr_t>(&RpAnisotTextureSetMaxAnisotropy);
    bodies[AnisoGet] = std::bit_cast<std::uintptr_t>(&RpAnisotTextureGetMaxAnisotropy);
    bodies[ClumpAtoms] = std::bit_cast<std::uintptr_t>(&RpClumpGetNumAtomics);
    bodies[AtomAlways] = std::bit_cast<std::uintptr_t>(&RpAtomicSetStreamAlwaysCallBack);
    bodies[AtomRights] = std::bit_cast<std::uintptr_t>(&RpAtomicSetStreamRightsCallBack);
    bodies[AtomPlugOff] = std::bit_cast<std::uintptr_t>(&RpAtomicGetPluginOffset);
    bodies[TriMat] = std::bit_cast<std::uintptr_t>(&RpGeometryTriangleGetMaterial);
    bodies[MatInit] = std::bit_cast<std::uintptr_t>(&_rpMaterialListInitialize);
    bodies[MatGet] = std::bit_cast<std::uintptr_t>(&_rpMaterialListGetMaterial);
    bodies[MatFind] = std::bit_cast<std::uintptr_t>(&_rpMaterialListFindMaterialIndex);
    bodies[WorldReg] = std::bit_cast<std::uintptr_t>(&RpWorldRegisterPlugin);
    bodies[WorldRegStream] = std::bit_cast<std::uintptr_t>(&RpWorldRegisterPluginStream);
    bodies[AtomWorld] = std::bit_cast<std::uintptr_t>(&RpAtomicGetWorld);
    bodies[LightReg] = std::bit_cast<std::uintptr_t>(&RpLightRegisterPlugin);
    bodies[VdeclSize] = std::bit_cast<std::uintptr_t>(&_rpD3D9VertexDeclarationGetSize);
    bodies[GeoUsage] = std::bit_cast<std::uintptr_t>(&RpD3D9GeometryGetUsageFlags);
    bodies[SecSet] = std::bit_cast<std::uintptr_t>(&RpD3D9WorldSectorSetUsageFlags);
    bodies[SecGet] = std::bit_cast<std::uintptr_t>(&RpD3D9WorldSectorGetUsageFlags);
    std::array<Route, kFunctions> routes{};
    json bodyJson = json::object();
    for (unsigned f = 0; f < kFunctions; ++f) { bodies[f] = Resolve(bodies[f]); routes[f] = {kFns[f].entry, bodies[f]}; bodyJson[kFns[f].name] = bodies[f]; }
    env.collabDll[1] = Resolve(std::bit_cast<std::uintptr_t>(&RwD3D9GetCaps));
    env.collabDll[2] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPlgnStrmlwysCB));
    env.collabDll[3] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPlgnStrmRightsCB));
    env.collabDll[4] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryGetPluginOffset));
    env.collabDll[5] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPlugin));
    env.collabDll[6] = Resolve(std::bit_cast<std::uintptr_t>(&_rwPluginRegistryAddPluginStream));
    env.collabDll[7] = Resolve(std::bit_cast<std::uintptr_t>(&D3DResourceSystem::CreateIndexBuffer));
    std::vector<Watch> watches;
    for (unsigned f = 0; f < kFunctions; ++f) watches.emplace_back(kFns[f].entry, 16);
    for (unsigned k = 1; k < kCollabKinds; ++k) { watches.emplace_back(kCollabExe[k], 16); watches.emplace_back(env.collabDll[k], 16); }
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const std::array<std::uintptr_t, 4> globalAddrs{0xC92478, 0xC925A8, 0xC926E8, 0xC926EC};
    std::array<u32, 4> globalsInitial{}; for (unsigned i = 0; i < 4; ++i) globalsInitial[i] = *reinterpret_cast<u32*>(globalAddrs[i]);
    // estado inicial plausible de los desplazamientos (en el EXE sin ejecutar valen 0): se prueba tambien con valores de juego
    const unsigned total = kFunctions * cases;
    Write({{"event", "begin"}, {"registered_hook", true}, {"collaborators_stubbed", true}, {"functions", kFunctions}, {"pairs", total}, {"cases", cases}, {"compare_dead_x87_registers", kCompareDeadX87Registers},
           {"bodies", bodyJson}, {"asi_module", env.asi}, {"original_module", env.exe}, {"initial_globals", std::vector<u32>(globalsInitial.begin(), globalsInitial.end())}});
    int pairs = 0, totalFailures = 0;
    for (unsigned f = 0; f < kFunctions; ++f) {
        int failures = 0;
        for (unsigned c = 0; c < cases; ++c) {
            const json input = CaseInput(static_cast<Fid>(f), c);
            Side a = RunSide(routes[f], env, static_cast<Fid>(f), c, false);
            Side b = RunSide(routes[f], env, static_cast<Fid>(f), c, true);
            bool ok = a.out == b.out;
            for (const char* k : {"fp_ip_route_ok", "stack_restored", "nonvolatile_preserved", "route_ok", "globals_restored"}) ok &= a.out[k] == true && b.out[k] == true;
            RestoreFloatingState();
            a.out["floating_state_restored"] = FloatingStateRestored(); b.out["floating_state_restored"] = a.out["floating_state_restored"];
            ok &= a.out["floating_state_restored"] == true;
            failures += !ok;
            Write({{"event", "comparison"}, {"function", kFns[f].name}, {"case", c}, {"pair", pairs++}, {"input", input}, {"original", a.out}, {"candidate", b.out}, {"fp_difference", FpDifference(a.run.fp, b.run.fp)},
                   {"original_residue", {{"eax", a.run.reg.eax}, {"ecx", a.run.reg.ecx}, {"edx", a.run.reg.edx}}}, {"candidate_residue", {{"eax", b.run.reg.eax}, {"ecx", b.run.reg.ecx}, {"edx", b.run.reg.edx}}},
                   {"original_fp_ip", a.run.ip}, {"candidate_fp_ip", b.run.ip}, {"equal", ok}, {"accepted", ok}});
        }
        totalFailures += failures;
        bool intact = true; for (auto& w : watches) intact &= w.Intact();
        Write({{"event", "function_summary"}, {"function", kFns[f].name}, {"cases", int(cases)}, {"failures", failures}, {"state_restored", intact}});
        std::printf("%-52s %d pares, %d diferencias\n", kFns[f].name, int(cases), failures);
    }
    bool intact = true; for (auto& w : watches) intact &= w.Intact();
    for (unsigned i = 0; i < 4; ++i) intact &= *reinterpret_cast<u32*>(globalAddrs[i]) == globalsInitial[i];
    RestoreFloatingState(); intact &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"state_restored", intact}, {"watched_intact", intact}});
    Write({{"event", totalFailures ? "error" : "complete"}, {"pairs", pairs}, {"state_restored", intact}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", intact}});
    std::printf("total: %d pares, %d diferencias\n", pairs, totalFailures);
    return totalFailures ? 1 : 0;
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--child") return Child(argc, argv);
    return Launcher(argc, argv);
}
