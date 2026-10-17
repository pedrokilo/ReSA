// Ensayo sin juego del banco Dependencies23A: ejecuta los bytes REALES de gta-sa.exe (imagen mapeada en sus direcciones en un proceso hijo de 32 bits) frente al codigo de la DLL
// (texto de Vehicle.cpp: DoVehicleLights y DoHeadLightEffect, extraido por extract_glue.py, con tipos falsos de mock.h y X87Math.cpp compilado con las opciones del proyecto).
// Usa las MISMAS fixtures que el banco (Fixtures.hpp) con la misma semilla por caso, y los mismos colaboradores falsos (en la direccion del EXE y en el cuerpo de la DLL).
// Compara ademas el FNSAVE completo (FCW, FSW, FTW y los diez bytes de los ocho registros ST, vacios incluidos) con la pila precargada con un residuo distinto y cuatro FSW iniciales.
//   uso: rehearsal.exe [casos = 4096] [primer caso = 0] [salida.jsonl o -] [flags: 1 = mostrar solo diferencias vivas]
#include "mock.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include "Fixtures.hpp"
using namespace Dependencies23AFix;

#include "glue.txt"

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
struct EntryPatch {
    uintptr_t address; std::array<uint8, 5> saved{}, jump{0xE9};
    template <class F> EntryPatch(uintptr_t at, F replacement) : address(at) {
        std::memcpy(saved.data(), reinterpret_cast<void*>(at), 5);
        const auto offset = uint32(reinterpret_cast<uintptr_t>(replacement) - at - 5);
        std::memcpy(jump.data() + 1, &offset, 4); Enable(true);
    }
    void Enable(bool enabled) const {
        void* p = reinterpret_cast<void*>(address); DWORD old{}, ignored{};
        VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old);
        std::memcpy(p, (enabled ? jump : saved).data(), 5);
        VirtualProtect(p, 5, old, &ignored); FlushInstructionCache(GetCurrentProcess(), p, 5);
    }
};

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
static unsigned tailCalls, headCalls, lightStatusCalls;
static void __cdecl MRegisterCorona(uint32 id, uint32 attach, uint32 r, uint32 g, uint32 b, uint32 a, const float* pos, uint32 size, uint32 range, uint32 type, uint32 flare, uint32 refl, uint32 los,
                                    uint32 trail, uint32 angle, uint32 neon, uint32 pull, uint32 fullBright, uint32 fade, uint32 below, uint32 white) {
    trace.push_back({"register_corona", CoronaId(id), Ptr(reinterpret_cast<void*>(uintptr_t(attach))), r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), size, range, type, flare, refl, los, trail,
                     angle, neon, pull, fullBright, fade, below, white});
}
static CVehicle* __cdecl MFindPlayerVehicle(int32 playerId, int32 includeRemote) {
    trace.push_back({"find_player_vehicle", playerId, includeRemote & 0xFF});
    return G.findVehicle == 0 ? nullptr : G.findVehicle == 1 ? reinterpret_cast<CVehicle*>(Veh()) : reinterpret_cast<CVehicle*>(Other());
}
static uint32 __fastcall MLightsStatus(CVehicle* self, void*) {
    trace.push_back({"get_vehicle_lights_status", Ptr(self)});
    return G.lightsStatus;
}
static uint32 __fastcall MGetLightStatus(CDamageManager* self, void*, uint32 light) {
    trace.push_back({"get_light_status", Ptr(self), light & 0xFF});
    return G.lightStatus[light & 3];
}
static uint32 __fastcall MHead(CVehicle* self, void*, int32 lightId, uint32 matrix, uint32 isRight, uint32 disabled) {
    trace.push_back({"head_effect", Ptr(self), lightId, Ptr(reinterpret_cast<void*>(uintptr_t(matrix))), isRight & 0xFF, disabled & 0xFF});
    return G.headRet[(headCalls++) & 1];
}
static uint32 __fastcall MTail(CVehicle* self, void*, int32 lightId, uint32 matrix, uint32 isRight, uint32 disabled, uint32 unused, uint32 staticEmission) {
    trace.push_back({"tail_effect", Ptr(self), lightId, Ptr(reinterpret_cast<void*>(uintptr_t(matrix))), isRight & 0xFF, disabled & 0xFF, unused, staticEmission & 0xFF});
    return G.tailRet[(tailCalls++) & 7];
}
static void __fastcall MReflection(CVehicle* self, void*, uint32 matrix, uint32 flags, uint32 left, uint32 right) {
    trace.push_back({"head_light_reflection", Ptr(self), Ptr(reinterpret_cast<void*>(uintptr_t(matrix))), flags, left & 0xFF, right & 0xFF});
}
static void __cdecl MAddLight(uint32 type, uint32 px, uint32 py, uint32 pz, uint32 dx, uint32 dy, uint32 dz, uint32 radius, uint32 r, uint32 g, uint32 b, uint32 fog, uint32 shadows, uint32 entity) {
    trace.push_back({"add_light", type & 0xFF, px, py, pz, dx, dy, dz, radius, r, g, b, fog & 0xFF, shadows & 0xFF, Ptr(reinterpret_cast<void*>(uintptr_t(entity)))});
}
static uint32 __fastcall FHead(CVehicle* self, void*, int32 lightId, uint32 matrix, uint32 isRight, uint32 disabled) {
    return MHead(self, nullptr, lightId, matrix, isRight, disabled);
}
// puentes de la DLL hacia los colaboradores falsos
void CCoronas::RegisterCorona(uint32 id, void* attachTo, uint8 r, uint8 g, uint8 b, uint8 intensity, const CVector& pos, float size, float range, eCoronaType coronaType, eCoronaFlareType flareType,
                              eCoronaReflType reflType, eCoronaLOSCheck checkLOS, eCoronaTrail usesTrails, float normalAngle, bool neonFade, float pullTowardsCam, bool fullBrightAtStart,
                              float fadeSpeed, bool onlyFromBelow, bool whiteCore) {
    MRegisterCorona(id, uint32(uintptr_t(attachTo)), r, g, b, intensity, &pos.x, Bits(size), Bits(range), uint32(coronaType), flareType, reflType, checkLOS, usesTrails, Bits(normalAngle), neonFade, Bits(pullTowardsCam),
                    fullBrightAtStart, Bits(fadeSpeed), onlyFromBelow, whiteCore);
}
CVehicle* FindPlayerVehicle(int32 playerId, bool incl) { return MFindPlayerVehicle(playerId, incl); }
CCam& CCamera::GetActiveCamera() { return TheCamera.m_aCams[TheCamera.m_nActiveCam]; }
bool CVehicle::GetVehicleLightsStatus() { return MLightsStatus(this, nullptr) & 0xFF; }
eLightsState CDamageManager::GetLightStatus(eLights light) const { return eLightsState(MGetLightStatus(const_cast<CDamageManager*>(this), nullptr, light)); }
bool CVehicle::DoTailLightEffect(int32 lightIndex, CMatrix& matrix, uint8 rightSide, uint8 disabled, uint32 unused, uint8 staticEmission) {
    return MTail(this, nullptr, lightIndex, uint32(uintptr_t(&matrix)), rightSide, disabled, unused, staticEmission) & 0xFF;
}
void CVehicle::DoHeadLightReflection(CMatrix& matrix, uint32 flags, bool left, bool right) { MReflection(this, nullptr, uint32(uintptr_t(&matrix)), flags, left, right); }
void CPointLights::AddLight(uint8 lightType, CVector point, CVector direction, float radius, float red, float green, float blue, uint8 fogType, bool generateExtraShadows, CEntity* entityAffected) {
    MAddLight(lightType, Bits(point.x), Bits(point.y), Bits(point.z), Bits(direction.x), Bits(direction.y), Bits(direction.z), Bits(radius), Bits(red), Bits(green), Bits(blue), fogType, generateExtraShadows,
              uint32(uintptr_t(entityAffected)));
}

// ---------------------------------------------------------------- llamada con captura de ABI y del FNSAVE
using Fp = std::array<uint8, 108>;
alignas(16) static Fp primeImage{};
struct Frame {
    uint32 espBefore{}, espAfter{}, ebxBefore{}, ebxAfter{}, esiBefore{}, esiAfter{}, ediBefore{}, ediAfter{}, ebpBefore{}, ebpAfter{}, sehBefore{}, sehAfter{};
    Fp imageAfter{}, envBefore{};
};
static constexpr uint16 kFpStatus[4]{0x0000, uint16(0x4700 | (3 << 11)), uint16(0x0100 | 0x0020 | (5 << 11)), uint16(0x0400 | (7 << 11))};
static void BuildPrime(const Fp& game, unsigned variant) {
    primeImage = game;
    const uint16 status = kFpStatus[variant & 3], tags = 0;
    std::memcpy(primeImage.data() + 4, &status, 2);
    std::memcpy(primeImage.data() + 8, &tags, 2);
    for (unsigned i = 0; i < 8; ++i) {
        uint8* r = primeImage.data() + 28 + 10 * i;
        for (unsigned k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k);
        r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40;
    }
}
__declspec(noinline) static Frame CallTwo(uintptr_t entry, void* self, uint32 a1, uint32 a2) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}; uint8* after = r.imageAfter.data(); const uint8* prime = primeImage.data(); uint8* e0 = r.envBefore.data();
    __asm {
        mov eax, e0
        fnsave [eax]
        frstor [eax]
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a2
        push a1
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, after
        fnsave [eax]
        frstor [eax]
    }
    r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}
__declspec(noinline) static Frame CallFour(uintptr_t entry, void* self, uint32 a1, uint32 a2, uint32 a3, uint32 a4) {
    Frame r{}; uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}; uint8* after = r.imageAfter.data(); const uint8* prime = primeImage.data(); uint8* e0 = r.envBefore.data();
    __asm {
        mov eax, e0
        fnsave [eax]
        frstor [eax]
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
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
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, after
        fnsave [eax]
        frstor [eax]
    }
    r.espBefore = sp0; r.espAfter = sp1; r.ebxBefore = b0; r.ebxAfter = b1; r.esiBefore = s0; r.esiAfter = s1; r.ediBefore = d0; r.ediAfter = d1; r.ebpBefore = p0; r.ebpAfter = p1; r.sehBefore = h0; r.sehAfter = h1;
    return r;
}
static uint8* CamBytes() { return reinterpret_cast<uint8*>(0xB6F028); }
// arena + camara + los tres globales del EXE que lee o escribe la funcion
static std::vector<uint8> Capture() {
    std::vector<uint8> v(arena.begin(), arena.end());
    v.insert(v.end(), CamBytes(), CamBytes() + kCameraBytes);
    for (uintptr_t a : {kTimeMs, kTimeStep, kForceOff}) v.insert(v.end(), reinterpret_cast<uint8*>(a), reinterpret_cast<uint8*>(a) + 4);
    return v;
}
static void Restore(const std::vector<uint8>& v) {
    std::memcpy(arena.data(), v.data(), arena.size());
    std::memcpy(CamBytes(), v.data() + arena.size(), kCameraBytes);
    size_t at = arena.size() + kCameraBytes;
    for (uintptr_t a : {kTimeMs, kTimeStep, kForceOff}) { std::memcpy(reinterpret_cast<void*>(a), v.data() + at, 4); at += 4; }
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
static std::string Hex(const uint8* d, size_t n) { static const char* h = "0123456789abcdef"; std::string s; for (size_t i = 0; i < n; ++i) { s += h[d[i] >> 4]; s += h[d[i] & 15]; } return s; }
static unsigned W16(const Fp& f, size_t at) { uint16 v; std::memcpy(&v, f.data() + at, 2); return v; }

static bool headMode{};
static uint32 headLight, headRight, headDisabled, headJunk;
static bool CallGuarded(uintptr_t entry, uint32 matrix, uint32 flags, Frame* out) {
    __try {
        *out = headMode ? CallFour(entry, Veh(), headLight, matrix, headRight | (headJunk & 0xFFFFFF00u), headDisabled | ((headJunk * 2654435761u) & 0xFFFFFF00u)) : CallTwo(entry, Veh(), matrix, flags);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static json Invoke(bool candidate, const std::vector<uint8>& input, const Cfg& cfg0, unsigned primeVariant) {
    Restore(input); G = cfg0; trace = json::array(); tailCalls = headCalls = lightStatusCalls = 0;
    void* matrix = (args.matrixIsVehicle && Get<void*>(Veh(), 0x14)) ? static_cast<void*>(MatVeh()) : static_cast<void*>(MatArg());
    const uintptr_t entry = headMode ? (candidate ? AddrOf(&CVehicle::DoHeadLightEffect) : 0x6E0A50) : (candidate ? AddrOf(&CVehicle::DoVehicleLights) : 0x6E1A60);
    Fp game{}; { uint8* g = game.data(); __asm { mov eax, g
        fnsave [eax]
        frstor [eax] } }
    BuildPrime(game, primeVariant);
    Frame f{};
    const bool crashed = !CallGuarded(entry, uint32(uintptr_t(matrix)), args.flags, &f);
    const auto after = Capture();
    const bool guards = GuardsOK();
    json x87 = {{"fcw", W16(f.imageAfter, 0)}, {"fsw", W16(f.imageAfter, 4)}, {"fsw_top", (W16(f.imageAfter, 4) >> 11) & 7}, {"ftw", W16(f.imageAfter, 8)}, {"raw_registers", Hex(f.imageAfter.data() + 28, 80)}};
    json row = {
        {"trace", trace}, {"changes", Diff(input, after)}, {"x87", x87},
        {"route_ok", true}, {"stack_restored", !crashed && f.espBefore == f.espAfter},
        {"nonvolatile_preserved", f.ebxBefore == f.ebxAfter && f.esiBefore == f.esiAfter && f.ediBefore == f.ediAfter && f.ebpBefore == f.ebpAfter},
        {"seh_restored", f.sehBefore == f.sehAfter},
        {"fpu_top_balanced", ((W16(primeImage, 4) >> 11) & 7) == ((W16(f.imageAfter, 4) >> 11) & 7) && W16(f.imageAfter, 8) == 0xFFFF},
        {"fpu_control_same", W16(primeImage, 0) == W16(f.imageAfter, 0)},
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
// primera diferencia entre dos filas (para el informe en consola)
static std::string FirstDiff(const json& a, const json& b) {
    for (const char* k : {"trace", "changes"}) if (a[k] != b[k]) return k;
    if (a["x87"].value("fsw", -1) != b["x87"].value("fsw", -1)) return "fsw";
    if (a["x87"].value("raw_registers", "") != b["x87"].value("raw_registers", "")) return "registros muertos";
    for (auto it = a.begin(); it != a.end(); ++it) if (*it != b[it.key()]) return it.key();
    return "?";
}
int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    argc -= 1; argv += 1;
    setvbuf(stdout, nullptr, _IONBF, 0);
    unsigned cw = 0; _controlfp_s(&cw, _PC_24, _MCW_PC);
    // colaboradores en la direccion del EXE
    headMode = argc > 5 && std::strcmp(argv[5], "head") == 0;
    static EntryPatch pCorona(0x6FC580, (void*)&MRegisterCorona), pFind(0x56E0D0, (void*)&MFindPlayerVehicle), pStatus(0x6D55C0, (void*)&MLightsStatus), pLight(0x6C2130, (void*)&MGetLightStatus),
                      pTail(0x6E1780, (void*)&MTail), pRefl(0x6E1720, (void*)&MReflection), pAdd(0x7000E0, (void*)&MAddLight);
    // y en el cuerpo de la DLL (los metodos del glue que no son DoHeadLightEffect ya puentean a los mismos falsos); en el modo "head" se prueba 0x6E0A50 contra el cuerpo de la DLL
    static std::unique_ptr<EntryPatch> pHead, headDll;
    if (!headMode) { pHead = std::make_unique<EntryPatch>(0x6E0A50, (void*)&FHead); headDll = std::make_unique<EntryPatch>(AddrOf(&CVehicle::DoHeadLightEffect), (void*)&FHead); }
    camBase = CamBytes();
    const int N = argc > 1 ? std::atoi(argv[1]) : 4096;
    const int first = argc > 2 ? std::atoi(argv[2]) : 0;
    const char* outPath = argc > 3 ? argv[3] : "rehearsal.jsonl";
    const bool liveOnly = argc > 4 && std::atoi(argv[4]) == 1;
    std::ofstream file; if (std::strcmp(outPath, "-") != 0) file.open(outPath);
    std::ostream& log = file.is_open() ? static_cast<std::ostream&>(file) : static_cast<std::ostream&>(*new std::ofstream("NUL"));
    const auto cameraSaved = std::vector<uint8>(CamBytes(), CamBytes() + kCameraBytes);
    log << json{{"event", "begin"}, {"registered_hook", true}, {"ready", true}, {"target_routes_ready", true}, {"fixture_contract", true}, {"collaborator_bodies_ok", true},
                {"compare_dead_x87_registers", true}, {"routes", json::array({json{{"function", "DoVehicleLights"}, {"registered", true}, {"body", 1}, {"module_ok", true}}})},
                {"collaborators", json::array({json::array({0x6FC580, 1}), json::array({0x56E0D0, 2}), json::array({0x6D55C0, 3}), json::array({0x6C2130, 4}), json::array({0x6E0A50, 5}), json::array({0x6E1780, 6}),
                                               json::array({0x6E1720, 7}), json::array({0x7000E0, 8})})}, {"probe_module", 1}, {"arena_bytes", arena.size()}, {"camera_bytes", kCameraBytes},
                {"vehicle_bytes", kVehicleBytes}, {"pump_state_intact", true}, {"scope", "ensayo sin juego (rehearsal.cpp): bytes reales del EXE mapeados en un proceso hijo"}}.dump() << '\n';
    int fail = 0, shown = 0; std::map<std::string, int> kinds, scenarios;
    for (int n = first; n < first + N; ++n) {
        fixRng = Rng(uint64(1) * 1000003ull + uint64(n) * 7919ull + 17ull);
        const json profile = SetupCase();
        if (headMode) {
            headLight = fixRng.N(2); headRight = fixRng.N(2) ? fixRng.N(2) : Pick({2, 5, 0x80, 0xFF}); headDisabled = fixRng.N(5) ? 0 : (fixRng.N(2) ? 1 : Pick({2, 0x80, 0xFF})); headJunk = fixRng.Next();
        }
        const auto input = Capture();
        const Cfg cfg0 = G;
        const uint32 slotValue = *ModelSlot(modelIndex);
        *ModelSlot(modelIndex) = uint32(uintptr_t(ModelInfo()));
        const unsigned variant = unsigned(n);
        const json original = Invoke(false, input, cfg0, variant);
        const json candidate = Invoke(true, input, cfg0, variant);
        *ModelSlot(modelIndex) = slotValue;
        std::memcpy(CamBytes(), cameraSaved.data(), kCameraBytes);
        json o2 = original, c2 = candidate;
        if (liveOnly) { for (json* j : {&o2, &c2}) { (*j)["x87"].erase("raw_registers"); (*j)["x87"].erase("fsw"); } }
        bool equal = o2 == c2;
        for (const char* gate : {"route_ok", "stack_restored", "nonvolatile_preserved", "seh_restored", "fpu_top_balanced", "fpu_control_same", "guards_preserved", "fixture_contract", "state_restored", "floating_state_restored"})
            equal &= original.value(gate, false) && candidate.value(gate, false);
        if (!equal) { ++fail; ++kinds[FirstDiff(o2, c2)]; ++scenarios[profile.value("scenario", "?") + "/" + profile.value("directed", "?")];
            if (shown++ < 3) std::printf("[#%d] DIFERENCIA (%s)\n  EXE: %s\n  DLL: %s\n", n, FirstDiff(o2, c2).c_str(), original.dump().c_str(), candidate.dump().c_str()); }
        log << json{{"event", "comparison"}, {"function", "CVehicle::DoVehicleLights"}, {"pair", n - first}, {"case", n}, {"profile", profile}, {"original", original}, {"candidate", candidate}, {"equal", equal}, {"accepted", equal}}.dump() << '\n';
    }
    std::printf("CVehicle::DoVehicleLights: %d casos (desde %d), %d diferencias\n", N, first, fail);
    for (auto& [k, v] : kinds) std::printf("  primera diferencia %-22s %d\n", k.c_str(), v);
    for (auto& [k, v] : scenarios) std::printf("  escenario %-40s %d\n", k.c_str(), v);
    log << json{{"event", "function_summary"}, {"function", "CVehicle::DoVehicleLights"}, {"cases", N}, {"failures", fail}}.dump() << '\n';
    log << json{{"event", fail ? "error" : "complete"}, {"pairs", N}, {"state_restored", true}, {"floating_state_restored", true}, {"pump_state_intact", true}}.dump() << '\n';
    return fail ? 1 : 0;
}
