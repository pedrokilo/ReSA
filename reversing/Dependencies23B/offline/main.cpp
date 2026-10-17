// Arnes sin juego de CVehicle::FlyingControl (0x6D85F0): bytes reales del EXE (proceso hijo con la imagen mapeada) frente a la reconstruccion C++ (flc_vehicle.inc con tipos falsos de mock.h,
// FlcX87 de X87Math en el anexo g23b). Colaboradores falsos identicos en las dos rutas; compara FNSAVE completo (FCW, FSW, FTW y los 8 registros), MXCSR, memoria del arnes, traza de colaboradores y ESP.
#include "mock.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <algorithm>

// ---------------------------------------------------------------- imagen del EXE y utilidades
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
    auto* p = reinterpret_cast<uint8*>(at);
    p[0] = 0xE9;
    const uint32 rel = uint32(reinterpret_cast<uintptr_t>(fn) - at - 5);
    std::memcpy(p + 1, &rel, 4);
}
static std::string Hex(const uint8* p, int n) { char t[8]; std::string s; for (int i = 0; i < n; ++i) { std::sprintf(t, "%02x", p[i]); s += t; } return s; }

#include "../bank/Fixtures.hpp"
using namespace Dependencies23BFix;
// ---------------------------------------------------------------- arena
constexpr uintptr_t kArena = 0x30000000;
static uint8* Arena() { return reinterpret_cast<uint8*>(kArena); }
template<class T> static T* At(size_t off) { return reinterpret_cast<T*>(Arena() + off); }
static CVehicle* Veh() { return At<CVehicle>(L::veh); }
template<class T> static T& Fld(size_t off) { return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(Veh()) + off); }

// ---------------------------------------------------------------- colaboradores falsos
Cfg G;
static std::vector<uint32> trace;
static void T(std::initializer_list<uint32> v) { trace.insert(trace.end(), v); }
static uint32 TagOf(const void* p) {
    const uintptr_t u = reinterpret_cast<uintptr_t>(p);
    if (u >= kArena && u < kArena + kArenaSize) return uint32(u - kArena);
    return 0xFFFFFFFFu;
}
static void __fastcall SApplyTurn(CVehicle* self, void*, CVector force, CVector point) {
    T({1u, TagOf(self), Bits(force.x), Bits(force.y), Bits(force.z), Bits(point.x), Bits(point.y), Bits(point.z)});
}
static void __fastcall SApplyMove(CVehicle* self, void*, CVector force) { T({2u, TagOf(self), Bits(force.x), Bits(force.y), Bits(force.z)}); }
static CVector* __fastcall SGetSpeed(CVehicle* self, void*, CVector* out, CVector point) {
    T({3u, TagOf(self), Bits(point.x), Bits(point.y), Bits(point.z)});
    std::memcpy(out, G.speed, 12);
    return out;
}
static float __fastcall SHeight(CVehicle* self, void*, float z, int model) {
    T({4u, TagOf(self), Bits(z), uint32(model)});
    return G.ceiling;
}
static int __fastcall SSteerLR(CPad* self, void*) { T({5u, TagOf(self)}); return G.steerLR; }
static int __fastcall SSteerUD(CPad* self, void*) { T({6u, TagOf(self)}); return G.steerUD; }
static int __fastcall SGunUD(CPad* self, void*) { T({7u, TagOf(self)}); return G.gunUD; }
static int __fastcall SGunLR(CPad* self, void*) { T({8u, TagOf(self)}); return G.gunLR; }
static int __fastcall SAccel(CPad* self, void*) { T({9u, TagOf(self)}); return G.accel; }
static int __fastcall SBrake(CPad* self, void*) { T({10u, TagOf(self)}); return G.brake; }
static bool __fastcall SLookL(CPad* self, void*) { T({11u, TagOf(self)}); return G.lookL; }
static bool __fastcall SLookR(CPad* self, void*) { T({12u, TagOf(self)}); return G.lookR; }
static void* __fastcall SColModel(void* self, void*) { T({13u, TagOf(self)}); return G.colNull ? nullptr : static_cast<void*>(Arena() + L::col); }
static bool __fastcall SIsPlayer(CPed* self, void*) { T({14u, TagOf(self)}); return G.isPlayer; }
static CPad* __fastcall SPadFromPlayer(CPlayerPed* self, void*) { T({15u, TagOf(self)}); return reinterpret_cast<CPad*>(Arena() + L::pad); }
// 0x6D2600 no toca EDX (solo EAX y la pila x87): el gancho del EXE lo conserva
__declspec(naked) static void HeightHook() {
    __asm {
        push edx
        push dword ptr [esp + 12]
        push dword ptr [esp + 12]
        call SHeight
        pop edx
        ret 8
    }
}
static int gErrno;
static int* __cdecl SErrno() { return &gErrno; }

// puentes de la reconstruccion hacia los colaboradores falsos
struct CPad {
    int16 GetSteeringLeftRight() { return int16(SSteerLR(this, nullptr)); }
    int16 GetSteeringUpDown() { return int16(SSteerUD(this, nullptr)); }
    int16 GetCarGunUpDown() { return int16(SGunUD(this, nullptr)); }
    int16 GetCarGunLeftRight() { return int16(SGunLR(this, nullptr)); }
    int16 GetAccelerate() { return int16(SAccel(this, nullptr)); }
    int16 GetBrake() { return int16(SBrake(this, nullptr)); }
    bool GetLookLeft() { return SLookL(this, nullptr); }
    bool GetLookRight() { return SLookR(this, nullptr); }
};
CPad* CPlayerPed::GetPadFromPlayer() const { return SPadFromPlayer(const_cast<CPlayerPed*>(this), nullptr); }
bool CPed::IsPlayer() const { return SIsPlayer(const_cast<CPed*>(this), nullptr); }
void CVehicle::ApplyTurnForce(CVector force, CVector point) { SApplyTurn(this, nullptr, force, point); }
void CVehicle::ApplyMoveForce(CVector force) { SApplyMove(this, nullptr, force); }
CVector CVehicle::GetSpeed(CVector point) { CVector r; SGetSpeed(this, nullptr, &r, point); return r; }
float CVehicle::HeightAboveCeiling(float z, eFlightModel model) { return SHeight(this, nullptr, z, int(model)); }
void* CVehicle::GetColModel() { return SColModel(this, nullptr); }
struct CColModelMock { float _p[1]; float minY; };
static CColModelMock* ColModelOf(CVehicle* v) { return static_cast<CColModelMock*>(SColModel(v, nullptr)); }

#ifdef FLC_COVERAGE
static std::set<uint32> gCov;
#define FLC_COV(a) gCov.insert(a)
#include "../flc_vehicle_cov.inc"
#else
#include "../flc_vehicle.inc"
#endif

// ---------------------------------------------------------------- llamadas con captura del estado
struct RunState { uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}; uint8 img[108]{}; uint32 mxcsr{}; };
__declspec(noinline) static void CandThunk(CVehicle* v, int model, uint32 a2, uint32 a3, uint32 a4, uint32 a5) {
    v->FlyingControl(eFlightModel(model), reinterpret_cast<const float&>(a2), reinterpret_cast<const float&>(a3), reinterpret_cast<const float&>(a4), reinterpret_cast<const float&>(a5));
}
alignas(16) static uint8 gPrime[108];
static void MakePrime(uint32 fsw) {
    uint8 tmp[108];
    __asm { lea eax, tmp } __asm { fnsave [eax] }
    std::memcpy(gPrime, tmp, 108);
    gPrime[0] = 0x7F; gPrime[1] = 0;
    uint16 s = uint16(fsw); std::memcpy(gPrime + 4, &s, 2);
    gPrime[8] = 0; gPrime[9] = 0;
    for (int i = 0; i < 8; ++i) { uint8* r = gPrime + 28 + 10 * i; for (int k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k); r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40; }
}
__declspec(noinline) static void RunCall(bool candidate, CVehicle* v, int model, uint32 a2, uint32 a3, uint32 a4, uint32 a5, RunState* st) {
    uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, mx0 = 0x1F80, mx1{};
    uint8* img = st->img;
    uintptr_t target = candidate ? reinterpret_cast<uintptr_t>(&CandThunk) : 0x6D85F0;
    __asm {
        ldmxcsr mx0
        lea eax, gPrime
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
        cmp candidate, 0
        jne cand
        push a5
        push a4
        push a3
        push a2
        push model
        mov ecx, v
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call dword ptr [target]
        jmp done
    cand:
        push a5
        push a4
        push a3
        push a2
        push model
        push v
        call dword ptr [target]
        add esp, 24
    done:
        mov sp1, esp
        stmxcsr mx1
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, img
        fnsave [eax]
    }
    st->sp0 = sp0; st->sp1 = sp1; st->b0 = b0; st->s0 = s0; st->d0 = d0; st->p0 = p0; st->b1 = b1; st->s1 = s1; st->d1 = d1; st->p1 = p1; st->mxcsr = mx1;
}

// ---------------------------------------------------------------- fixtures
static void ApplyCase(const Case& c) {
    std::memcpy(Arena(), c.arena.data(), kArenaSize);
    G = c.cfg;
    *reinterpret_cast<float*>(0xB7CB5C) = c.ts;
    for (int i = 0; i < 3; ++i) *reinterpret_cast<uint32*>(0xC813E0 + 4 * i) = c.wind[i];
    *reinterpret_cast<uint8*>(0xC1C970) = uint8((c.ts_bits >> 3) & 1);
    MakePrime(c.fsw);
    trace.clear();
}

#ifdef FLC_ANNEX
int gFlcTrace = 0;
#else
extern int gFlcTrace;
#endif
int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    setvbuf(stdout, nullptr, _IONBF, 0);
    unsigned cw = 0; _controlfp_s(&cw, _PC_24, _MCW_PC);
    const int N = argc > 2 ? std::atoi(argv[2]) : 2000;
    const int routeFilter = argc > 3 ? std::atoi(argv[3]) : -1;
    const int first = argc > 4 ? std::atoi(argv[4]) : 0;
    const int traceCase = argc > 5 ? std::atoi(argv[5]) : -1;
    // memoria del arnes
    if (!VirtualAlloc(reinterpret_cast<void*>(kArena), kArenaSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) { std::printf("no se pudo reservar la arena\n"); return 4; }
    *reinterpret_cast<uint32*>(0x8D33C4) = *reinterpret_cast<uint32*>(0x8D33C4);
    Hook(0x542A50, (void*)&SApplyTurn); Hook(0x5429F0, (void*)&SApplyMove); Hook(0x542CE0, (void*)&SGetSpeed); Hook(0x6D2600, (void*)&HeightHook);
    Hook(0x53FB80, (void*)&SSteerLR); Hook(0x53FBD0, (void*)&SSteerUD); Hook(0x53FC10, (void*)&SGunUD); Hook(0x53FC50, (void*)&SGunLR);
    Hook(0x5403F0, (void*)&SAccel); Hook(0x540080, (void*)&SBrake); Hook(0x53FE10, (void*)&SLookR); Hook(0x53FDD0, (void*)&SLookL);
    Hook(0x535300, (void*)&SColModel); Hook(0x5DF8F0, (void*)&SIsPlayer); Hook(0x609560, (void*)&SPadFromPlayer); Hook(0x82B181, (void*)&SErrno);
    *reinterpret_cast<uint32*>(0xC9C400) = 1; *reinterpret_cast<uint32*>(0xC9C404) = 1; *reinterpret_cast<uint32*>(0xC9ABFC) = 0;
    std::map<int, std::pair<int, int>> byRoute;
    std::map<int, std::array<int, 20>> kinds;
    int shown = 0;
    for (int route = 0; route < 11; ++route) {
        if (routeFilter >= 0 && route != routeFilter) continue;
        for (int n = first; n < first + N; ++n) {
            const Case c = MakeCase(route, uint64(route) * 1000003ull + uint64(n), kArena);
            RunState ex, cd;
            ApplyCase(c); gErrno = 0;
            RunCall(false, Veh(), c.model, c.args[0], c.args[1], c.args[2], c.args[3], &ex);
            const std::vector<uint8> exMem(Arena(), Arena() + kArenaSize);
            const auto exTrace = trace;
            const int exErrno = gErrno;
            ApplyCase(c); gErrno = 0;
            gFlcTrace = (n == traceCase);
            if (gFlcTrace) std::printf("fh=%08x driver=%08x status=%02x created=%02x ", *(uint32*)(c.arena.data()+L::veh+0x388), *(uint32*)(c.arena.data()+L::veh+0x460), c.arena[L::veh+0x36], c.arena[L::veh+0x4A4]);
            if (gFlcTrace) std::printf("traza del caso %d: ts=%08x wind=%08x %08x %08x args=%08x %08x %08x %08x\n", n, Bits(c.ts), c.wind[0], c.wind[1], c.wind[2], c.args[0], c.args[1], c.args[2], c.args[3]);
            RunCall(true, Veh(), c.model, c.args[0], c.args[1], c.args[2], c.args[3], &cd);
            gFlcTrace = 0;
            const std::vector<uint8> cdMem(Arena(), Arena() + kArenaSize);
            const auto cdTrace = trace;
            std::string d;
            if (ex.sp0 != ex.sp1) d += " ex_esp";
            if (cd.sp0 != cd.sp1) d += " cd_esp";
            if (ex.b0 != ex.b1 || ex.s0 != ex.s1 || ex.d0 != ex.d1 || ex.p0 != ex.p1) d += " ex_nonvol";
            if (cd.b0 != cd.b1 || cd.s0 != cd.s1 || cd.d0 != cd.d1 || cd.p0 != cd.p1) d += " cd_nonvol";
            if (std::memcmp(ex.img, cd.img, 12)) {
                uint32 f1, f2; std::memcpy(&f1, ex.img + 4, 2); std::memcpy(&f2, cd.img + 4, 2);
                char t[80]; std::snprintf(t, sizeof(t), " ctrl(fcw/fsw/ftw: fsw %04x vs %04x)", f1 & 0xFFFF, f2 & 0xFFFF); d += t;
            }
            for (int r = 0; r < 8; ++r) if (std::memcmp(ex.img + 28 + 10 * r, cd.img + 28 + 10 * r, 10)) d += " ST" + std::to_string(r) + "(" + Hex(ex.img + 28 + 10 * r, 10) + " vs " + Hex(cd.img + 28 + 10 * r, 10) + ")";
            if (((ex.mxcsr ^ cd.mxcsr) & 0x3F)) { char t[48]; std::snprintf(t, sizeof(t), " mxcsr(%04x vs %04x)", ex.mxcsr & 0xFFFF, cd.mxcsr & 0xFFFF); d += t; }
            if (exErrno != gErrno) d += " errno";
            if (exMem != cdMem) {
                size_t i = 0; while (exMem[i] == cdMem[i]) ++i;
                char t[96]; std::snprintf(t, sizeof(t), " memoria(+%zx: %02x vs %02x)", i, exMem[i], cdMem[i]); d += t;
            }
            if (exTrace != cdTrace) {
                size_t i = 0; while (i < exTrace.size() && i < cdTrace.size() && exTrace[i] == cdTrace[i]) ++i;
                char t[128]; std::snprintf(t, sizeof(t), " traza(len %zu vs %zu, palabra %zu: %08x vs %08x)", exTrace.size(), cdTrace.size(), i, i < exTrace.size() ? exTrace[i] : 0u, i < cdTrace.size() ? cdTrace[i] : 0u); d += t;
            }
            { auto& kc = kinds[route]; std::array<bool, 20> seen{}; static const int len[16] = {0, 8, 5, 5, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; size_t i = 0; while (i < exTrace.size()) { const uint32 k = exTrace[i]; if (k < 16) { seen[k] = true; i += len[k]; } else break; } for (int k = 0; k < 16; ++k) kc[k] += seen[k]; kc[16] += exTrace.empty(); kc[17] += (ex.mxcsr & 0x3F) != 0; kc[18] += exErrno != 0; }
            auto& st = byRoute[route];
            ++st.first;
            if (!d.empty()) {
                ++st.second;
                if (shown < 2 && exTrace != cdTrace) { size_t i = 0; while (i < exTrace.size() && i < cdTrace.size() && exTrace[i] == cdTrace[i]) ++i; /*DUMPTR*/ for (size_t k = i > 12 ? i - 12 : 0; k < std::min(exTrace.size(), i + 6); ++k) std::printf("   [%zu] ex=%08x cd=%08x\n", k, exTrace[k], k < cdTrace.size() ? cdTrace[k] : 0u); }
                if (shown++ < 12) std::printf("[ruta %d modelo %d caso %d fsw0=%04x]%s\n", route, c.model, n, c.fsw, d.c_str());
            }
        }
    }
#ifdef FLC_COVERAGE
    {
        FILE* cf = std::fopen("../cov_addrs.txt", "r"); char nm[16]; unsigned a; std::map<std::string, std::pair<int,int>> tot; std::map<std::string, std::vector<unsigned>> miss;
        while (cf && std::fscanf(cf, "%15s %x", nm, &a) == 2) { ++tot[nm].first; if (gCov.count(a)) ++tot[nm].second; else miss[nm].push_back(a); }
        for (auto& kv : tot) { std::printf("cobertura %s: %d de %d instrucciones\n", kv.first.c_str(), kv.second.second, kv.second.first); std::printf("  sin ejecutar:"); unsigned prev = 0; int n = 0; for (unsigned m : miss[kv.first]) { std::printf(" %x", m); if (++n > 400) break; } std::printf("\n"); }
    }
#endif
    for (auto& kv : byRoute) std::printf("ruta %d: %d casos, %d diferencias\n", kv.first, kv.second.first, kv.second.second);
    for (auto& kv : kinds) { std::printf("eventos ruta %d:", kv.first); for (int k = 1; k < 19; ++k) std::printf(" %d=%d", k, kv.second[k]); std::printf("\n"); }
    return 0;
}
