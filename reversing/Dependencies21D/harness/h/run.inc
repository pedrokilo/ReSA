
// ---------------------------------------------------------------- ejecucion y comparacion
static std::vector<uint8> Capture() {
    std::vector<uint8> v;
    auto add = [&](const void* p, size_t n) { v.insert(v.end(), static_cast<const uint8*>(p), static_cast<const uint8*>(p) + n); };
    add(A.task, sizeof(A.task)); add(A.ped, sizeof(A.ped)); add(A.veh, sizeof(A.veh)); add(A.data, sizeof(A.data)); add(A.info, sizeof(A.info));
    add(A.tgt, sizeof(A.tgt)); add(A.wi, sizeof(A.wi)); add(A.assoc, sizeof(A.assoc)); add(A.matPed, sizeof(A.matPed)); add(A.matVeh, sizeof(A.matVeh));
    add(A.matTgt, sizeof(A.matTgt)); add(A.matAlloc, sizeof(A.matAlloc)); add(A.vecOut, sizeof(A.vecOut));
    add(reinterpret_cast<void*>(0xB6F028), 0xD7C);
    return v;
}
static void Restore(const std::vector<uint8>& v) {
    size_t o = 0;
    auto get = [&](void* p, size_t n) { std::memcpy(p, v.data() + o, n); o += n; };
    get(A.task, sizeof(A.task)); get(A.ped, sizeof(A.ped)); get(A.veh, sizeof(A.veh)); get(A.data, sizeof(A.data)); get(A.info, sizeof(A.info));
    get(A.tgt, sizeof(A.tgt)); get(A.wi, sizeof(A.wi)); get(A.assoc, sizeof(A.assoc)); get(A.matPed, sizeof(A.matPed)); get(A.matVeh, sizeof(A.matVeh));
    get(A.matTgt, sizeof(A.matTgt)); get(A.matAlloc, sizeof(A.matAlloc)); get(A.vecOut, sizeof(A.vecOut));
    get(reinterpret_cast<void*>(0xB6F028), 0xD7C);
}
static const char* RegionOf(size_t off, size_t& base) {
    struct R { const char* n; size_t sz; };
    static const R r[] = { {"task", sizeof(A.task)}, {"ped", sizeof(A.ped)}, {"veh", sizeof(A.veh)}, {"data", sizeof(A.data)}, {"info", sizeof(A.info)}, {"tgt", sizeof(A.tgt)}, {"wi", sizeof(A.wi)}, {"assoc", sizeof(A.assoc)},
        {"matPed", sizeof(A.matPed)}, {"matVeh", sizeof(A.matVeh)}, {"matTgt", sizeof(A.matTgt)}, {"matAlloc", sizeof(A.matAlloc)}, {"vecOut", sizeof(A.vecOut)}, {"camera", 0xD7C} };
    size_t acc = 0;
    for (auto& x : r) { if (off < acc + x.sz) { base = acc; return x.n; } acc += x.sz; }
    base = acc; return "?";
}
struct Snap { std::vector<uint8> mem; std::string trace; CallState st; uint32 ret{}; int errnoV{}; };
static uintptr_t AddrOf(auto mf) { uintptr_t a{}; static_assert(sizeof(mf) == sizeof(a)); std::memcpy(&a, &mf, sizeof(a)); return a; }

enum Fn { F_PT, F_AI, F_AIM, F_CTOR };
static const char* kNames[] = { "PlayerTarget", "AITarget", "AimGun", "Constructor" };
static uint32 ctorArgs[6]; static bool ctorHasPos;
static int gCrash; static unsigned gCrashEip, gCrashAddr, gCrashCode, gCrashEbp;
static int Filter(EXCEPTION_POINTERS* ep) {
    gCrash = 1; gCrashEip = unsigned(ep->ContextRecord->Eip); gCrashCode = unsigned(ep->ExceptionRecord->ExceptionCode); gCrashEbp = unsigned(ep->ContextRecord->Ebp);
    gCrashAddr = ep->ExceptionRecord->NumberParameters > 1 ? unsigned(ep->ExceptionRecord->ExceptionInformation[1]) : 0;
    return EXCEPTION_EXECUTE_HANDLER;
}
static void DoCall(Fn f, bool exeSide, Snap& s) {
    switch (f) {
    case F_PT:  Call1(exeSide ? 0x621960 : AddrOf(&CTaskSimpleGangDriveBy::PlayerTarget), A.task, A.ped, &s.st); break;
    case F_AI:  Call1(exeSide ? 0x627600 : AddrOf(&CTaskSimpleGangDriveBy::AITarget), A.task, A.ped, &s.st); break;
    case F_AIM: Call1(exeSide ? 0x628350 : AddrOf(&CTaskSimpleGangDriveBy::AimGun), A.task, A.ped, &s.st); break;
    case F_CTOR: s.ret = Call6(exeSide ? 0x6217D0 : AddrOf(&CTaskSimpleGangDriveBy::Constructor), A.task, ctorArgs[0], ctorArgs[1], ctorArgs[2], ctorArgs[3], ctorArgs[4], ctorArgs[5], &s.st); break;
    }
}
static void DoCallSafe(Fn f, bool exeSide, Snap* s) {
    __try { DoCall(f, exeSide, *s); } __except (Filter(GetExceptionInformation())) {}
}
static void RunSide(Fn f, bool exeSide, Snap& s) {
    trace.clear(); G.radianCalls = 0; gCrash = 0; gErrno = 0;
    std::memset(&s.st, 0, sizeof(s.st));
    DoCallSafe(f, exeSide, &s);
    s.mem = Capture();
    s.trace = trace;
    s.errnoV = gErrno;
}
static unsigned EnvW(const uint8* envp, int field) { uint16 v; std::memcpy(&v, envp + field, 2); return unsigned(v); }
static int fswTotal, fswDiff; static std::map<unsigned,int> fswCC; static int gDeadSlot[8], gDeadFirst, gDeadShown; static std::string gDeadMsg[8];
static int Compare(Fn f, const Snap& e, const Snap& d, std::string& why) {
    char b[256];
    if (e.trace != d.trace) { why = "trace EXE:\n" + e.trace + "trace DLL:\n" + d.trace; return 1; }
    for (size_t i = 0; i < e.mem.size(); ++i) {
        if (f == F_CTOR && i < 4) continue; // vptr
        if (e.mem[i] != d.mem[i]) { size_t base; const char* n = RegionOf(i, base); std::snprintf(b, sizeof(b), "memoria %s+%X EXE=%02X DLL=%02X", n, unsigned(i - base), e.mem[i], d.mem[i]); why = b; return 2; }
    }
    for (int k = 0; k < 2; ++k) {
        const CallState& s = k ? d.st : e.st;
        if (s.sp0 != s.sp1) { why = k ? "DLL: ESP" : "EXE: ESP"; return 3; }
        if (s.b0 != s.b1 || s.s0 != s.s1 || s.d0 != s.d1 || s.p0 != s.p1) { why = k ? "DLL: no volatiles" : "EXE: no volatiles"; return 4; }
        if (s.h0 != s.h1) { why = "fs:0"; return 5; }
        if (EnvW(s.env0, 0) != EnvW(s.env1, 0)) { why = "fcw cambia"; return 6; }
        if (((EnvW(s.env0, 4) >> 11) & 7) != ((EnvW(s.env1, 4) >> 11) & 7)) { why = k ? "DLL: TOP desequilibrado" : "EXE: TOP desequilibrado"; return 7; }
        if (EnvW(s.env1, 8) != 0xFFFF) { why = k ? "DLL: ftw no vacio" : "EXE: ftw no vacio"; return 8; }
    }
    if (f == F_CTOR && e.ret != d.ret) { why = "EAX"; return 9; }
    if (e.errnoV != d.errnoV) { why = "errno"; return 10; }
    int rcSw = 0;
    { const unsigned fe = EnvW(e.st.env1, 4), fd = EnvW(d.st.env1, 4); fswTotal++; if (fe != fd) { fswDiff++; fswCC[(fe ^ fd) & 0xFFFF]++; std::snprintf(b, sizeof(b), "FSW EXE=%04X DLL=%04X (xor %04X)", fe, fd, fe ^ fd); why = b; rcSw = 11; } }
    for (int sl = 0; sl < 8; ++sl) {
        if (std::memcmp(e.st.img + 28 + 10 * sl, d.st.img + 28 + 10 * sl, 10) != 0) {
            gDeadSlot[sl]++;
            if (!gDeadFirst) gDeadFirst = sl + 1;
            if (gDeadShown < 6) { gDeadShown++; std::string t; for (int k = 9; k >= 0; --k) { char c[4]; std::snprintf(c, sizeof c, "%02X", e.st.img + 28 + 10 * sl + k == nullptr ? 0 : e.st.img[28 + 10 * sl + k]); t += c; } t += " / "; for (int k = 9; k >= 0; --k) { char c[4]; std::snprintf(c, sizeof c, "%02X", d.st.img[28 + 10 * sl + k]); t += c; }
              gDeadMsg[sl] = t; std::printf("      registro muerto ST(%d) EXE/DLL: %s%c", sl, t.c_str(), 10); }
            if (!rcSw) { std::snprintf(b, sizeof(b), "registro muerto ST(%d) distinto", sl); why = b; } rcSw = rcSw ? rcSw : 12;
        }
    }
    return rcSw;
}

static std::map<std::string, int> coverage;
static void Cover(Fn f, const Snap& e) {
    const std::string& t = e.trace;
    size_t p = 0;
    while (p < t.size()) { size_t q = t.find('\n', p); std::string line = t.substr(p, q - p); p = q + 1; size_t sp = line.find(' '); coverage[std::string(kNames[f]) + ":" + line.substr(0, sp)]++; }
    if (f == F_AI) { coverage["AI:style=" + std::to_string(int(int8_t(e.mem[0x12]))) + " anim=" + std::to_string(Get<int>(e.mem.data(), 0x24)) + " in=" + std::to_string(e.mem[0xB])]++; }
    if (f == F_AI) { coverage["AI:inRange" + std::to_string(e.mem[0xB])]++; coverage["AI:reqAnim=" + std::to_string(Get<int>(e.mem.data(), 0x24))]++; coverage["AI:next=" + std::to_string(e.mem[0xF])]++; }
    if (f == F_PT) { coverage["PT:reqAnim=" + std::to_string(Get<int>(e.mem.data(), 0x24))]++; }
    if (f == F_AIM) {
        const uint8* vb = e.mem.data() + sizeof(A.task) + sizeof(A.ped); const uint8* mb = e.mem.data() + sizeof(A.task) + sizeof(A.ped) + sizeof(A.veh) + sizeof(A.data) + sizeof(A.info) + sizeof(A.tgt) + sizeof(A.wi) + sizeof(A.assoc) + sizeof(A.matPed);
        const bool bike = Get<void*>(e.mem.data() + sizeof(A.task), 0x58C) != nullptr && Get<int>(vb, 0x590) == 9;
        if (bike) { const uint32 fz = Get<uint32>(mb, 0x18); const float v = std::bit_cast<float>(fz); const char* c = (fz & 0x7F800000u) == 0x7F800000u && (fz & 0x7FFFFFu) ? "NaN" : (v < -1.0f ? "<-1" : v > 1.0f ? ">1" : v == 1.0f ? "=1" : v == -1.0f ? "=-1" : "[-1,1]"); coverage[std::string("AIM:bike fz ") + c]++; }
        else coverage["AIM:no bike"]++;
    }
}

static int Launcher(int argc, char** argv) {
    char self[MAX_PATH]; GetModuleFileNameA(nullptr, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" --child";
    for (int i = 1; i < argc; ++i) cmd += std::string(" ") + argv[i];
    STARTUPINFOA si{}; si.cb = sizeof(si); PROCESS_INFORMATION pi{};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { std::printf("CreateProcess fallo\n"); return 2; }
    if (!MapExeInto(pi.hProcess)) {
        for (uintptr_t a = 0x10000; a < 0xD00000;) { MEMORY_BASIC_INFORMATION m{}; if (!VirtualQueryEx(pi.hProcess, reinterpret_cast<void*>(a), &m, sizeof(m))) break; if (m.State != MEM_FREE) std::printf("  ocupado %p +%X estado=%X tipo=%X\n", m.BaseAddress, unsigned(m.RegionSize), unsigned(m.State), unsigned(m.Type)); a = uintptr_t(m.BaseAddress) + m.RegionSize; }
        TerminateProcess(pi.hProcess, 3); return 3; }
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code{}; GetExitCodeProcess(pi.hProcess, &code);
    return int(code);
}
int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    argc -= 1; argv += 1;
    setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("hijo: inicio" "%c", 10);
    unsigned cw = 0; _controlfp_s(&cw, _PC_24, _MCW_PC);
    Hook(0x441390, (void*)&MIsCoop); Hook(0x50BFB0, (void*)&MSetMode); Hook(0x513E40, (void*)&MTwoPlayer); Hook(0x5DF8F0, (void*)&MIsPlayer);
    Hook(0x609FF0, (void*)&MPlayerInfo); Hook(0x514970, (void*)&MFind3rd); Hook(0x5E4280, (void*)&MBone); Hook(0x53CBE0, (void*)&MRadian);
    Hook(0x621B10, (void*)&MLos); Hook(0x54F560, (void*)&MAlloc); Hook(0x54F1B0, (void*)&MUpdate); Hook(0x5FDC00, (void*)&MPointGun); Hook(0x571B70, (void*)&MRegRef); Hook(0x82B181, (void*)&MErrno);
    std::printf("hijo: ganchos" "%c", 10);
    const int N = argc > 1 ? std::atoi(argv[1]) : 20000;
    const int only = argc > 2 ? std::atoi(argv[2]) : -1;
    if (argc > 3) rngState ^= (uint64_t)std::atoll(argv[3]) * 0x9E3779B97F4A7C15ull;
    int totalFail = 0;
    for (int fi = 0; fi < 4; ++fi) {
        if (only >= 0 && fi != only) continue;
        const Fn f = Fn(fi);
        int fail = 0, shown = 0;
        for (int i = 0; i < N; ++i) {
            switch (f) {
            case F_PT: SetupPT2(); break;
            case F_AI: SetupAI2(); break;
            case F_AIM: SetupAim2(); break;
            case F_CTOR:
                SetupCommon(true, true); FillRandom(A.task, sizeof(A.task));
                ctorHasPos = RndN(2);
                for (int k = 0; k < 3; ++k) PutF(A.vecOut, 4 * k, RndBits(60));
                ctorArgs[0] = RndN(3) ? uint32(uintptr_t(A.tgt)) : 0;
                ctorArgs[1] = ctorHasPos ? uint32(uintptr_t(A.vecOut)) : 0;
                ctorArgs[2] = RndBits(100); ctorArgs[3] = RndN(256); ctorArgs[4] = RndN(9); ctorArgs[5] = RndN(2);
                break;
            }
            const auto init = Capture(); const Cfg g0 = G;
            Snap e, d;
            { static const unsigned kBase[4] = { 0x0000, 0x4700 | (3 << 11), 0x0100 | 0x0020 | (5 << 11), 0x0400 | (7 << 11) }; MakePrimeImage(kBase[i & 3]); }
            RunSide(f, true, e);
            const bool crashE = gCrash; const unsigned eipE = gCrashEip, adE = gCrashAddr;
            Restore(init); G = g0;
            RunSide(f, false, d);
            const bool crashD = gCrash;
            if (crashE || crashD) {
                std::printf("[%s #%d] CRASH code=%08X EXE=%d (eip %08X dir %08X) DLL=%d (eip %08X dir %08X)\n", kNames[f], i, gCrashCode, crashE, eipE, adE, crashD, gCrashEip, gCrashAddr);
                std::printf("   veh=%p type=%d vehMatrix=%p player=%d tgt=%p tgtMatrix=%p assoc=%p anim=%d%c", Get<void*>(init.data() + sizeof(A.task), 0x58C), Get<int>(init.data() + sizeof(A.task) + sizeof(A.ped), 0x590), Get<void*>(init.data() + sizeof(A.task) + sizeof(A.ped), 0x14), int(G.isPlayer), Get<void*>(init.data(), 0x34), Get<void*>(init.data() + sizeof(A.task) + sizeof(A.ped) + sizeof(A.veh) + sizeof(A.data) + sizeof(A.info), 0x14), Get<void*>(init.data(), 0x2C), int(Get<int16>(init.data() + sizeof(A.task) + sizeof(A.ped) + sizeof(A.veh) + sizeof(A.data) + sizeof(A.info) + sizeof(A.tgt) + sizeof(A.wi), 0x2C)), 10);
                { char mod[260] = {}; HMODULE hm{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(uintptr_t(gCrashEip)), &hm); GetModuleFileNameA(hm, mod, 259); std::printf("   modulo=%s fwdz=%08X radian0=%08X lean=%08X%c", mod, Get<uint32>(init.data() + sizeof(A.task) + sizeof(A.ped) + sizeof(A.veh) + sizeof(A.data) + sizeof(A.info) + sizeof(A.tgt) + sizeof(A.wi) + sizeof(A.assoc) + sizeof(A.matPed), 0x18), G.radian[0], Get<uint32>(init.data() + sizeof(A.task) + sizeof(A.ped), 0x648), 10); }
                if (crashE && crashD) continue;
                ++fail; continue;
            }
            std::string why;
            const int rc = Compare(f, e, d, why);
            Cover(f, e);
            if (rc) {
                ++fail;
                if (shown++ < 3) std::printf("[%s #%d] DIFERENCIA (%d): %s\n", kNames[f], i, rc, why.c_str()); if (f == F_PT && shown <= 3) { const uint8* vb = init.data() + sizeof(A.task) + sizeof(A.ped); const uint8* mb = init.data() + sizeof(A.task) + sizeof(A.ped) + sizeof(A.veh) + sizeof(A.data) + sizeof(A.info) + sizeof(A.tgt) + sizeof(A.wi) + sizeof(A.assoc) + sizeof(A.matPed);
        const uint8* cb = init.data() + init.size() - 0xD7C; const unsigned idx = cb[0x59]; const uint8* cam = cb + 0x174 + idx * 0x238;
        std::printf("      vehMatrix=%p fwd=%08X,%08X heading=%08X camfront=%08X,%08X mode=%X%c", Get<void*>(vb, 0x14), Get<uint32>(mb, 0x10), Get<uint32>(mb, 0x14), Get<uint32>(vb, 0x10), Get<uint32>(cam, 0x190), Get<uint32>(cam, 0x194), Get<uint32>(cam, 0xC), 10); }
            }
        }
        std::printf("   fsw distinto: %d de %d; registros muertos distintos por ST(i):", fswDiff, fswTotal); for (int sl = 0; sl < 8; ++sl) { std::printf(" %d", gDeadSlot[sl]); gDeadSlot[sl] = 0; } std::printf("%c", 10); fswDiff = fswTotal = 0; gDeadShown = 0;
        std::printf("%s: %d casos, %d diferencias\n", kNames[f], N, fail);
        totalFail += fail;
    }
    for (auto& [k, v] : coverage) std::printf("  cobertura %-40s %d\n", k.c_str(), v);
    return totalFail ? 1 : 0;
}
