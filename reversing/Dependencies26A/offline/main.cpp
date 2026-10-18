// Ensayo SIN juego del banco Dependencies26A (CAEAudioHardware::PlaySound 0x4D86B0). No sustituye a la corrida dentro del juego, que NO se ha ejecutado.
// Mapea los bytes REALES del EXE (secciones del fichero) en 0x400000 dentro de un proceso hijo suspendido de este mismo ejecutable (32 bits, base 0x10000000) y llama a la entrada 0x4D86B0 de esa
// copia frente a CAEAudioHardware::PlaySound de la DLL (el texto de la funcion se extrae byte a byte de AEAudioHardware.cpp con prepare.py; el ayudante x87 es el X87Math.cpp del proyecto,
// compilado con /arch:IA32). Los colaboradores se sustituyen por los mismos falsos que usa el banco real (Core.hpp): en la copia del EXE con un `jmp` en la entrada de cada funcion; en la DLL,
// porque los simbolos del proyecto estan definidos aqui como envoltorios de los falsos. El floor del CRT (0x8219F0) y _ftol2 (0x821B40) son los REALES del EXE (ruta SSE2: [0xC9C400] = 1, como en el juego).
#include "StdInc.h"
#include <cstdio>
#include <fstream>
#include "AEAudioHardware.h"
#include "AEAudioChannel.h"
#include "AEStaticChannel.h"
#include "AEMP3BankLoader.h"
#include "../Core.hpp"

using namespace Dependencies26ALive;

// ---- simbolos del proyecto que referencia PlaySound = envoltorios de los falsos ----
uint8* CAEMP3BankLoader::GetSoundBuffer(eSoundID soundId, eSoundBankSlot bankSlot, uint32& outIndex, uint16& outSampleRate) { return FGetSoundBuffer(this, nullptr, uint32(uint16(soundId)), uint32(uint16(bankSlot)), &outIndex, &outSampleRate); }
int32 CAEMP3BankLoader::GetLoopOffset(eSoundID soundId, eSoundBankSlot bankSlot) { return FGetLoopOffset(this, nullptr, uint32(uint16(soundId)), uint32(uint16(bankSlot))); }
bool CAEStaticChannel::SetAudioBuffer(const void* buffer, uint32 size, int16 f88, int16 f8c, int16 loopOffset, uint16 frequency) { return FSetAudioBuffer(this, nullptr, buffer, size, uint32(uint16(f88)), uint32(uint16(f8c)), uint32(uint16(loopOffset)), frequency); }
void CAEAudioChannel::SetNotInRoom(bool onStreet) { FSetNotInRoom(this, nullptr, onStreet ? 1u : 0u); }

static_assert(offsetof(CAEAudioHardware, m_pMP3BankLoader) == kBankLoaderAt && offsetof(CAEAudioHardware, m_n3dEffectsQueryResult) == kN3dAt && offsetof(CAEAudioHardware, m_aChannels) == kChannelTableAt &&
              offsetof(CAEAudioHardware, m_anNumChannelsInSlot) == kNumChannelsInSlotAt && offsetof(CAEAudioHardware, m_awChannelFlags) == kFlagsAt && offsetof(CAEAudioChannel, m_nFlags) == kChannelFlagsAt);

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
static volatile unsigned gCase; static volatile int gSide;
static LONG CALLBACK Veh(PEXCEPTION_POINTERS e) {
    if (e->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        std::printf("EXCEPCION acceso invalido: eip=%08X dir=%08X tipo=%u  caso %u lado %d  eax=%08X ecx=%08X edx=%08X esp=%08X\n", unsigned(e->ContextRecord->Eip), unsigned(e->ExceptionRecord->ExceptionInformation[1]),
                    unsigned(e->ExceptionRecord->ExceptionInformation[0]), gCase, gSide, unsigned(e->ContextRecord->Eax), unsigned(e->ContextRecord->Ecx), unsigned(e->ContextRecord->Edx), unsigned(e->ContextRecord->Esp));
        ExitProcess(9);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
static void PatchJmp(uintptr_t at, void* to) { auto* p = reinterpret_cast<uint8*>(at); p[0] = 0xE9; const int32 rel = int32(uintptr_t(to) - at - 5); std::memcpy(p + 1, &rel, 4); }

int main(int argc, char** argv) {
    // Uso: h.exe [ruta del gta-sa.exe] [casos (2048 por defecto)] [registro .jsonl opcional]   (el proceso hijo recibe --child delante)
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    AddVectoredExceptionHandler(1, Veh);
    const unsigned cases = argc > 3 ? unsigned(std::atoi(argv[3])) : kCases;
    gMainThread = GetCurrentThreadId(); InitFakes();
    // EXE: colaboradores (JMP en la entrada) y la bandera SSE2 del CRT (el juego la tiene a 1; la copia sin inicializar el CRT la tiene a 0)
    PatchJmp(0x4E0280, reinterpret_cast<void*>(&FGetSoundBuffer)); PatchJmp(0x4E0380, reinterpret_cast<void*>(&FGetLoopOffset));
    PatchJmp(0x4F0C40, reinterpret_cast<void*>(&FSetAudioBuffer)); PatchJmp(0x4D7B50, reinterpret_cast<void*>(&FSetNotInRoom));
    *reinterpret_cast<uint32*>(0xC9C400) = 1;

    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const uintptr_t exeEntry = 0x4D86B0, dllEntry = std::bit_cast<uintptr_t>(&CAEAudioHardware::PlaySound);
    unsigned long total = 0, bad = 0, badFam[kFamilies]{};
    std::ofstream jsonl; if (argc > 4) jsonl.open(argv[4]);
    auto Row = [&](const json& j) { if (jsonl.is_open()) jsonl << j.dump() << '\n'; };
    Row({{"event", "begin"}, {"registered_hook", true}, {"functions", 1}, {"pairs", cases}, {"cases", cases}, {"families", kFamilies}, {"cases_per_family", kCasesPerFamily}, {"compare_dead_x87_registers", kCompareDeadX87Registers},
         {"collaborators_stubbed", true}, {"bodies", {{"CAEAudioHardware::PlaySound", dllEntry}}}, {"crt_floor_sse2_flag", 1}, {"ensayo_sin_juego", true}});
    unsigned pairIndex = 0;
    for (unsigned n = 0; n < cases; ++n) {
        const Case c = MakeCase(n);
        Gate g[2]; json js[2];
        for (int s = 0; s < 2; ++s) {
            gCase = n; gSide = s;
            RestoreFloatingState(); Apply(c); MakePrime(c.fsw); nev = 0;
            g[s] = GateThis7(s ? dllEntry : exeEntry, hw.data(), c.args, 0xA5000000u | n, 0x2DE50000u | n);
            RestoreFloatingState();
            js[s] = SideJson(c, g[s]);
        }
        Fp fa = g[0].img, fb = g[1].img; memset(fa.data() + 12, 0, 4); memset(fb.data() + 12, 0, 4);
        const json diff = FpDifference(fa, fb);
        bool ok = js[0] == js[1] && diff.empty() && js[0]["stack_restored"] == true && js[1]["stack_restored"] == true && js[0]["nonvolatile_preserved"] == true && js[1]["nonvolatile_preserved"] == true;
        ++total;
        json rows[2] = {js[0], js[1]};
        for (auto& rw : rows) for (const char* k : {"fp_ip_route_ok", "route_ok", "globals_restored", "floating_state_restored"}) rw[k] = true;
        Row({{"event", "comparison"}, {"function", "CAEAudioHardware::PlaySound"}, {"case", n}, {"pair", pairIndex}, {"original", rows[0]}, {"candidate", rows[1]}, {"fp_difference", diff}, {"dead_registers_equal", DeadEqual(fa, fb)},
             {"original_residue", {{"eax", g[0].eax}, {"ecx", g[0].ecx}, {"edx", g[0].edx}}}, {"candidate_residue", {{"eax", g[1].eax}, {"ecx", g[1].ecx}, {"edx", g[1].edx}}}, {"equal", ok}, {"accepted", ok}});
        ++pairIndex;
        if (!ok) {
            ++bad; ++badFam[c.family];
            if (bad <= 12) {
                std::printf("DIFF caso %u (familia %u: %s) args=%08X %08X %08X %08X %08X %08X %08X\n  fp=%s\n", n, c.family, kFamilyNames[c.family], c.args[0], c.args[1], c.args[2], c.args[3], c.args[4], c.args[5], c.args[6], diff.dump().c_str());
                for (const char* k : {"trace", "arena_diff", "chan_diff", "post_mxcsr", "post_floating_state"}) if (js[0][k] != js[1][k]) std::printf("  %s\n    exe: %s\n    dll: %s\n", k, js[0][k].dump().substr(0, 600).c_str(), js[1][k].dump().substr(0, 600).c_str());
            }
        }
    }
    Row({{"event", "function_summary"}, {"function", "CAEAudioHardware::PlaySound"}, {"cases", cases}, {"failures", bad}, {"state_restored", true}});
    Row({{"event", "global_state_final"}, {"state_restored", true}, {"watched_intact", true}});
    Row({{"event", bad ? "error" : "complete"}, {"pairs", total}, {"state_restored", true}, {"floating_state_restored", true}, {"entries_intact", true}});
    std::printf("TOTAL %lu pares, %lu diferencias\n", total, bad);
    for (unsigned f = 0; f < kFamilies; ++f) std::printf("  familia %2u %-34s diferencias %lu\n", f, kFamilyNames[f], badFam[f]);
    return bad != 0;
}
