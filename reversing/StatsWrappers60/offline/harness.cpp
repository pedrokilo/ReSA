// Arnes offline del banco StatsWrappers60 (sin ejecutar GTA): carga la imagen del EXE canonico en sus direcciones originales (0x400000), sustituye los colaboradores por los mismos
// falsos que el banco (Fakes.hpp) y compara, caso a caso, el codigo del EXE contra las funciones C++ de Stats.cpp (extraidas con gen_port.py y compiladas aqui con un entorno minimo).
// Escribe filas con el mismo formato que el banco dentro del juego (event/function/case/input/original/candidate), que fixture_oracle.py y run_offline.py verifican.
//
// Compilar y ejecutar con build_offline.bat (cl x86; x87.cpp lleva /O2 /arch:IA32 /fp:fast como X87Math.cpp). Uso: harness.exe <find|build|resp|cons> <primer caso> <cantidad> <salida.jsonl>\n#include <windows.h>
#include <bit>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "../Gate.hpp"
using namespace StatsWrappers60Fix;

// ------------------------------------------------------------------------------------------------ entorno minimo de Stats.cpp
using int8 = std::int8_t; using int16 = std::int16_t; using int32 = std::int32_t; using int64 = std::int64_t;
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using uint64 = std::uint64_t;
template<class T> T& StaticRef(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }
using GxtChar = uint8;
inline auto& gString = StaticRef<char[352]>(0xB71670);
inline auto& gString2 = StaticRef<char[352]>(0xB71510);
inline auto& gGxtString = StaticRef<GxtChar[552]>(0xC1B100);
inline auto& gGxtString2 = StaticRef<GxtChar[552]>(0xC1AED8);
enum eStats : int32 { STAT_MUSCLE = 23, STAT_TOTAL_RESPECT = 64, STAT_GIRLFRIEND_RESPECT = 65, STAT_CLOTHES_RESPECT = 66, STAT_RESPECT = 68, STAT_RESPECT_MISSION = 224, STAT_RESPECT_MISSION_TOTAL = 228 };
enum eStatUpdateState : uint8 { STAT_UPDATE_DECREASE = 0, STAT_UPDATE_INCREASE = 1 };
enum eRadioID : int8 { RADIO_INVALID = -1 };
enum class eWeaponSkill : uint8 { POOR = 0, STD = 1, PRO = 2, COP = 3 };
enum eWeaponType : uint32 { WEAPON_NONE = 0 };
enum eCheats { CHEAT_MAX_RESPECT = 79 };
struct CPlayerInfo { char pad[0xB8]; int32 m_nMoney; char pad2[0x190 - 0xBC]; };
struct CWorld {
    static inline auto& Players = StaticRef<CPlayerInfo[2]>(0xB7CD98);
    static inline auto& PlayerInFocus = StaticRef<uint8>(0xB7CD74);
};
struct CGangWars { static inline auto& TerritoryUnderControlPercentage = StaticRef<float>(0x96AB9C); };
struct CCheat { static inline auto& m_aCheatsActive = StaticRef<bool[98]>(0x969130); };
struct CTagManager { static __declspec(noinline) int32 GetPercentageTagged(); };
struct CLocalisation { static __declspec(noinline) bool GermanGame(); static __declspec(noinline) bool Metric(); };
struct CText { __declspec(noinline) const GxtChar* Get(const char* key); };
inline auto& TheText = StaticRef<CText>(0xC1B340);
struct CPed { __declspec(noinline) eWeaponSkill GetWeaponSkill(eWeaponType type); };
struct CPlayerPed : CPed {};
__declspec(noinline) CPlayerPed* FindPlayerPed(int32 playerId = -1);
struct CAudioEngine { __declspec(noinline) void GetRadioStationNameKey(eRadioID id, char* outStr); };
inline CAudioEngine AudioEngine;
__declspec(noinline) GxtChar* GxtCharStrcpy(GxtChar* dst, const GxtChar* src);
__declspec(noinline) void AsciiToGxtChar(const char* src, GxtChar* dst);
struct CStats {
    static constexpr size_t FIRST_INT_STAT = 120;
    static constexpr size_t FIRST_UNUSED_STAT = 82;
    static inline auto& FavoriteRadioStationList = StaticRef<std::array<int32, 14>>(0xB78E58);
    static inline auto& PedsKilledOfThisType = StaticRef<std::array<int32, 32>>(0xB78E90);
    static inline auto& StatTypesInt = StaticRef<std::array<int32, 223>>(0xB79000);
    static inline auto& StatTypesFloat = StaticRef<std::array<float, 82>>(0xB79380);
    static inline int16& m_ThisStatIsABarChart = StaticRef<int16>(0xB794CC);
    static __declspec(noinline) float GetStatValue(eStats stat);
    static __declspec(noinline) int32 ConvertToMins(int32 value);
    static __declspec(noinline) int32 ConvertToSecs(int32 value);
    static __declspec(noinline) float GetPercentageProgress();
    static __declspec(noinline) bool PopulateFavoriteRadioStationList();
    static __declspec(noinline) eRadioID FindMostFavoriteRadioStation();
    static __declspec(noinline) int32 FindLeastFavoriteRadioStation();
    static __declspec(noinline) void BuildStatLine(char* line, void* pValue1, int32 metrics, void* pValue2, int32 type);
    static __declspec(noinline) int32 ConstructStatLine(int32 arg0, uint8 arg1);
    static __declspec(noinline) void UpdateRespectStat(uint8 arg0);
    static __declspec(noinline) bool CheckForThreshold(float* pValue, float range);
    static __declspec(noinline) void DisplayScriptStatUpdateMessage(eStatUpdateState state, eStats stat, float value);
    static __declspec(noinline) void CheckForStatsMessage();
};
// Cuerpos de relleno (se parchean con los falsos antes de usarlos; deben ser distintos entre si para que el enlazador no los pliegue).
static volatile int g_sink;
#define STUB_BODY(n) g_sink = n
int32 CTagManager::GetPercentageTagged() { STUB_BODY(1); return 0; }
bool CLocalisation::GermanGame() { STUB_BODY(2); return false; }
bool CLocalisation::Metric() { STUB_BODY(3); return false; }
const GxtChar* CText::Get(const char*) { STUB_BODY(4); return nullptr; }
eWeaponSkill CPed::GetWeaponSkill(eWeaponType) { STUB_BODY(5); return eWeaponSkill::STD; }
CPlayerPed* FindPlayerPed(int32) { STUB_BODY(6); return nullptr; }
void CAudioEngine::GetRadioStationNameKey(eRadioID, char*) { STUB_BODY(7); }
GxtChar* GxtCharStrcpy(GxtChar* dst, const GxtChar*) { STUB_BODY(8); return dst; }
void AsciiToGxtChar(const char*, GxtChar*) { STUB_BODY(9); }
float CStats::GetStatValue(eStats) { STUB_BODY(10); return 0.0f; }
int32 CStats::ConvertToMins(int32) { STUB_BODY(11); return 0; }
int32 CStats::ConvertToSecs(int32) { STUB_BODY(12); return 0; }
float CStats::GetPercentageProgress() { STUB_BODY(13); return 0.0f; }
bool CStats::PopulateFavoriteRadioStationList() { STUB_BODY(14); return false; }
eRadioID CStats::FindMostFavoriteRadioStation() { STUB_BODY(15); return RADIO_INVALID; }
bool CStats::CheckForThreshold(float*, float) { STUB_BODY(16); return false; }
void CStats::DisplayScriptStatUpdateMessage(eStatUpdateState, eStats, float) { STUB_BODY(17); }
void CStats::CheckForStatsMessage() { STUB_BODY(18); }

#include "x87_decl.inc"
namespace {
#include "port_top.inc"
}
#include "port_find.inc"
#include "port_build.inc"
#include "port_cons.inc"
#include "port_resp.inc"

// ------------------------------------------------------------------------------------------------ imagen del EXE
static bool LoadExeImage(const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    std::vector<char> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(data.data());
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(data.data() + dos->e_lfanew);
    const std::size_t size = nt->OptionalHeader.SizeOfImage;
    // host.exe reserva 0x401000..0xCB1000 (seccion .gta) y esta enlazado en 0x400000; aqui se hace escribible y se sobrescribe con las secciones del EXE canonico.
    DWORD old{};
    if (!VirtualProtect(reinterpret_cast<void*>(0x401000), size - 0x1000, PAGE_EXECUTE_READWRITE, &old)) { std::fprintf(stderr, "VirtualProtect fallo (%lu)\n", GetLastError()); return false; }
    std::memset(reinterpret_cast<void*>(0x401000), 0, size - 0x1000);
    const auto* sec = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const std::size_t n = sec[i].SizeOfRawData < sec[i].Misc.VirtualSize ? sec[i].SizeOfRawData : sec[i].Misc.VirtualSize;
        std::memcpy(reinterpret_cast<char*>(0x400000) + sec[i].VirtualAddress, data.data() + sec[i].PointerToRawData, n);
    }
    return true;
}

// ------------------------------------------------------------------------------------------------ parches de entrada
struct EntryPatch {
    std::uintptr_t address;
    std::array<u8, 5> saved{}, jump{0xE9};
    template<class F> EntryPatch(std::uintptr_t at, F replacement) : address(at) {
        std::memcpy(saved.data(), reinterpret_cast<void*>(at), 5);
        const auto offset = u32(reinterpret_cast<std::uintptr_t>(replacement) - at - 5);
        std::memcpy(jump.data() + 1, &offset, 4); Enable(true);
    }
    void Enable(bool enabled) const {
        void* p = reinterpret_cast<void*>(address); DWORD old{}, ignored{};
        if (!VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old)) std::abort();
        std::memcpy(p, (enabled ? jump : saved).data(), 5);
        VirtualProtect(p, 5, old, &ignored); FlushInstructionCache(GetCurrentProcess(), p, 5);
    }
    ~EntryPatch() { Enable(false); }
};
inline std::uintptr_t Body(std::uintptr_t p) { for (int k = 0; k < 4 && *reinterpret_cast<const u8*>(p) == 0xE9; ++k) { const std::uintptr_t t = p + 5 + *reinterpret_cast<const std::int32_t*>(p + 1); if (t >= 0x400000 && t < 0x1000000) break; p = t; } return p; }
struct Pair { std::uintptr_t exe, dll; };
struct Patches {
    std::array<std::optional<EntryPatch>, 64> p; int n{};
    template<class F> void Add(const Pair& pr, F stub) { if (pr.exe) p[n++].emplace(pr.exe, stub); if (pr.dll && pr.dll != pr.exe) p[n++].emplace(pr.dll, stub); }
    ~Patches() { for (int i = n - 1; i >= 0; --i) p[i].reset(); }
};
template<class F> static std::uintptr_t B(F fp) { return Body(std::bit_cast<std::uintptr_t>(fp)); }
struct Collabs {
    Pair getStat{0x558E40, B(&CStats::GetStatValue)}, metric{0x56D220, B(&CLocalisation::Metric)}, german{0x56D200, B(&CLocalisation::GermanGame)}, populate{0x558EC0, B(&CStats::PopulateFavoriteRadioStationList)};
    Pair findMost{0x558FA0, B(&CStats::FindMostFavoriteRadioStation)}, findLeast{0x559010, B(&CStats::FindLeastFavoriteRadioStation)}, progress{0x5591E0, B(&CStats::GetPercentageProgress)};
    Pair mins{0x559540, B(&CStats::ConvertToMins)}, secs{0x559560, B(&CStats::ConvertToSecs)}, build{0x559230, B(&CStats::BuildStatLine)};
    Pair get{0x6A0050, B(&CText::Get)}, g2a{0x69F7E0, 0}, strcpy_{0x718660, B(&GxtCharStrcpy)}, strip{0x719240, 0}, a2g{0x718600, B(&AsciiToGxtChar)}, sprintf_{0x821BB5, 0};
    Pair stationKey{0x507010, B(&CAudioEngine::GetRadioStationNameKey)}, findPed{0x56E210, B(&FindPlayerPed)}, weapon{0x5E3B60, B(&CPed::GetWeaponSkill)};
    Pair tagged{0x49CDA0, B(&CTagManager::GetPercentageTagged)}, threshold{0x5595F0, B(&CStats::CheckForThreshold)}, display{0x55B980, B(&CStats::DisplayScriptStatUpdateMessage)}, checkMsg{0x559760, B(&CStats::CheckForStatsMessage)};
};
static Collabs* collabs;
static void InstallText(Patches& p) {
    p.Add(collabs->get, &Stw60::FakeGet); p.Add(collabs->g2a, &Stw60::FakeG2A); p.Add(collabs->strcpy_, &Stw60::FakeStrcpy); p.Add(collabs->strip, &Stw60::FakeStrip);
    p.Add(collabs->a2g, &Stw60::FakeA2G); p.Add(collabs->sprintf_, &Stw60::FakeSprintf); p.Add(collabs->metric, &Stw60::FakeMetric);
}
static void InstallBuild(Patches& p) { InstallText(p); }
static void InstallCons(Patches& p) {
    InstallText(p);
    p.Add(collabs->german, &Stw60::FakeGerman); p.Add(collabs->getStat, &Stw60::FakeGetStat); p.Add(collabs->build, &Stw60::FakeBuild); p.Add(collabs->mins, &Stw60::FakeMins);
    p.Add(collabs->secs, &Stw60::FakeSecs); p.Add(collabs->progress, &Stw60::FakeProgress); p.Add(collabs->populate, &Stw60::FakePopulate); p.Add(collabs->findMost, &Stw60::FakeFindMost);
    p.Add(collabs->findLeast, &Stw60::FakeFindLeast); p.Add(collabs->stationKey, &Stw60::FakeStationKey); p.Add(collabs->findPed, &Stw60::FakeFindPed); p.Add(collabs->weapon, &Stw60::FakeWeaponSkill);
}
static void InstallResp(Patches& p) {
    p.Add(collabs->tagged, &Stw60::FakeTagged); p.Add(collabs->threshold, &Stw60::FakeThreshold); p.Add(collabs->display, &Stw60::FakeDisplay); p.Add(collabs->checkMsg, &Stw60::FakeCheckMsg);
}

// ------------------------------------------------------------------------------------------------ ejecucion
using namespace Stw60;
struct Side { Reg reg{}; std::array<u8, 108> fp{}; };
template<class Install, class Gatecall>
static Side Execute(Install install, Gatecall gc) {
    Side s;
    PrepareFloatingState();
    {
        Patches patches;
        install(patches);
        s.reg = gc();
    }
    SaveFP(s.fp.data());
    return s;
}
static json SideRow(const Side& s, const json& observed) {
    json r = {{"observed", observed}, {"stack_restored", s.reg.stack}, {"nonvolatile_preserved", s.reg.nonvolatile}, {"direction_flag_clear", (s.reg.eflags & 0x400u) == 0}};
    return r;
}
static Snapshot base;
template<class Make, class InputOf, class SetupFn, class ObserveFn, class CallFn>
static void RunFunction(const char* name, unsigned first, unsigned count, std::ofstream& out, const std::vector<Region>& allowed, Make make, InputOf inputOf, SetupFn setup, ObserveFn observe, CallFn callSide) {
    unsigned bad = 0;
    for (unsigned n = first; n < first + count; ++n) {
        const auto f = make(n);
        json rows[2]; Side sides[2];
        base.Take();
        const char* only = std::getenv("HARNESS_SIDE");   // depuracion: solo el lado 0 (EXE) o 1 (funcion C++)
        for (int s = 0; s < 2; ++s) {
            if (only && only[0] - '0' != s) continue;
            Reg reg{};
            rows[s] = RunCase(base, allowed, [&] { setup(f); }, [&] { sides[s] = callSide(f, s == 1); reg = sides[s].reg; return reg; }, [&] { return observe(f, reg); }, reg);
        }
        bool eq = rows[0] == rows[1];
        const auto fa = NormalizeFp(sides[0].fp), fb = NormalizeFp(sides[1].fp);
        const bool fpsame = fa == fb;
        for (int s = 0; s < 2; ++s) eq &= sides[s].reg.stack && sides[s].reg.nonvolatile && (sides[s].reg.eflags & 0x400u) == 0;
        eq &= fpsame;
        bad += !eq;
        json row = {{"event", "comparison"}, {"function", name}, {"case", n}, {"input", inputOf(f)}, {"original", SideRow(sides[0], rows[0])}, {"candidate", SideRow(sides[1], rows[1])},
                    {"fp_difference", FpDifference(fa, fb)}, {"original_residue", ResidueJson(sides[0].reg)}, {"candidate_residue", ResidueJson(sides[1].reg)}, {"equal", eq}};
        out << row.dump() << '\n';
    }
    std::fprintf(stderr, "%s: %u casos, %u distintos\n", name, count, bad);
}

static LONG CALLBACK CrashReport(PEXCEPTION_POINTERS p) {
    if (p->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION || p->ExceptionRecord->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION) {
        const CONTEXT* c = p->ContextRecord;
        std::fprintf(stderr, "EXCEPCION %08lx en EIP=%08lx acceso=%p eax=%08lx ecx=%08lx edx=%08lx ebx=%08lx esi=%08lx edi=%08lx ebp=%08lx esp=%08lx\n", p->ExceptionRecord->ExceptionCode, c->Eip,
                     p->ExceptionRecord->NumberParameters > 1 ? reinterpret_cast<void*>(p->ExceptionRecord->ExceptionInformation[1]) : nullptr, c->Eax, c->Ecx, c->Edx, c->Ebx, c->Esi, c->Edi, c->Ebp, c->Esp);
        std::fflush(stderr);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static int RunMain(int argc, char** argv) {
    AddVectoredExceptionHandler(1, CrashReport);
    if (argc < 5) { std::fprintf(stderr, "uso: harness <find|build|resp|cons> <primer caso> <cantidad> <salida.jsonl>\n"); return 2; }
    if (!LoadExeImage("D:\\Grand Theft Auto San Andreas\\gta-sa.exe")) return 3;
    collabs = new Collabs;
    const std::string which = argv[1];
    const unsigned first = std::strtoul(argv[2], nullptr, 10), count = std::strtoul(argv[3], nullptr, 10);
    std::ofstream out(argv[4], std::ios::binary);
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    if (which == "find") {
        RunFunction("CStats::FindLeastFavoriteRadioStation", first, count, out, kAllowedFind, MakeFind, InputFind, SetupFind,
            [](const FxFind&, const Reg& reg) { return ObserveFind(reg); },
            [](const FxFind& f, bool candidate) {
                return Execute([](Patches&) {}, [&] { return Gate(candidate ? B(&CStats::FindLeastFavoriteRadioStation) : 0x559010, nullptr, 0, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 0); });
            });
    } else if (which == "build") {
        BuildArgs args{};
        RunFunction("CStats::BuildStatLine", first, count, out, kAllowedBuild, MakeBuild, InputBuild, [&](const FxBuild& f) { args = SetupBuild(f); },
            [](const FxBuild&, const Reg&) { return ObserveBuild(); },
            [&](const FxBuild& f, bool candidate) {
                const u32 a[5] = { u32(reinterpret_cast<std::uintptr_t>(args.line)), u32(reinterpret_cast<std::uintptr_t>(args.p1)), u32(args.metrics), u32(reinterpret_cast<std::uintptr_t>(args.p2)), u32(args.type) };
                return Execute(InstallBuild, [&] { return Gate(candidate ? B(&CStats::BuildStatLine) : 0x559230, a, 5, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 20); });
            });
    } else if (which == "resp") {
        RunFunction("CStats::UpdateRespectStat", first, count, out, kAllowedResp, MakeResp, InputResp, SetupResp,
            [](const FxResp&, const Reg&) { return ObserveResp(); },
            [](const FxResp& f, bool candidate) {
                const u32 a[1] = { f.arg0 };
                return Execute(InstallResp, [&] { return Gate(candidate ? B(&CStats::UpdateRespectStat) : 0x55BC50, a, 1, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 4); });
            });
    } else if (which == "cons") {
        RunFunction("CStats::ConstructStatLine", first, count, out, kAllowedCons, MakeCons, InputCons, SetupCons,
            [](const FxCons&, const Reg& reg) { return ObserveCons(reg); },
            [](const FxCons& f, bool candidate) {
                const u32 a[2] = { u32(f.arg0), f.arg1 };
                return Execute(InstallCons, [&] { return Gate(candidate ? B(&CStats::ConstructStatLine) : 0x55A780, a, 2, f.abi.eax, f.abi.ecx, f.abi.edx, f.abi.stack, 8); });
            });
    } else { return 2; }
    return 0;
}

// Punto de entrada que llama host.exe (linea de comandos completa; los argumentos no llevan espacios).
extern "C" __declspec(dllexport) void __cdecl Run(const char* commandLine) {
    std::vector<std::string> tokens;
    std::string cur;
    for (const char* p = commandLine; ; ++p) {
        if (*p == ' ' || *p == 0) { if (!cur.empty()) { tokens.push_back(cur); cur.clear(); } if (!*p) break; } else if (*p != '"') cur += *p;
    }
    std::vector<char*> argv;
    for (auto& t : tokens) argv.push_back(t.data());
    const int code = RunMain(static_cast<int>(argv.size()), argv.data());
    std::fflush(stderr);
    ExitProcess(static_cast<UINT>(code));
}
