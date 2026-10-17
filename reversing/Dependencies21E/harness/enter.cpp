// 0x63A380: la funcion REAL del EXE (bytes mapeados) frente al candidato C++ (misma estructura que la fuente propuesta en TaskComplexEnterCar.cpp),
// con colaboradores falsos: GetSubTask/GetTaskType por tablas virtuales falsas; AimWeaponLeftRight/UpDown, GetAccelerate, GetBrake y GetColModel por saltos en las
// direcciones del EXE (y llamadas directas desde el candidato). GetPad (0x53FB70) es la funcion real del EXE.
#include "core.hpp"
#include "../../ge-bank/Fixtures.hpp"
#include <math.h>
#include <string>

bool __cdecl EnterCarCamStickBeyondCpp(short value);
void __cdecl EnterCarCamLateralCpp(const float* matrix, float placementHeading, const float* carPos, const float* pedPos, float distance, float* out);
void __cdecl EnterCarCamStickCpp(float camMinY, float distance, float angle, float heading, float dot, unsigned int choice, float t1, float t2, float limit,
                                 const volatile float* vertical, const volatile float* horizontal, float* alphaStick, float* betaStick);

// ---- objetos falsos (tamanos y desplazamientos de los objetos reales) ----
struct alignas(16) Obj { u8 b[0x800]; };
static Obj gTask, gSub, gCar, gMat, gPed, gPedMat, gCol;
static void* gVtTask[16]; static void* gVtSub[16];

struct Event { int kind; u32 self; u32 arg; bool operator==(const Event& o) const { return kind == o.kind && self == o.self && arg == o.arg; } };
static std::vector<Event> gTrace;
enum { kGetSubTask = 1, kGetTaskType, kAimLR, kAimUD, kAccel, kBrake, kColModel, kRand };

static short gAimLR, gAimUD, gAccel, gBrake;
static u32 gSubType;
static void* __fastcall FakeGetSubTask(void* self, void*) { gTrace.push_back({ kGetSubTask, (u32)self, 0 }); return *(void**)((u8*)self + 8); }
static int __fastcall FakeGetTaskType(void* self, void*) { gTrace.push_back({ kGetTaskType, (u32)self, 0 }); return int(gSubType); }
static short __fastcall FakeAimLR(void* pad, void*, void* ped) { gTrace.push_back({ kAimLR, (u32)pad, (u32)ped }); return gAimLR; }
static short __fastcall FakeAimUD(void* pad, void*, void* ped) { gTrace.push_back({ kAimUD, (u32)pad, (u32)ped }); return gAimUD; }
static short __fastcall FakeAccel(void* pad, void*) { gTrace.push_back({ kAccel, (u32)pad, 0 }); return gAccel; }
static short __fastcall FakeBrake(void* pad, void*) { gTrace.push_back({ kBrake, (u32)pad, 0 }); return gBrake; }
static void* __fastcall FakeColModel(void* car, void*) { gTrace.push_back({ kColModel, (u32)car, 0 }); return &gCol; }

// ---- candidato: copia de la fuente propuesta (TaskComplexEnterCar.cpp) ----
static const u32 kPadBase = 0xB73458;
static u8* GetPadExe(int n) { return (u8*)(kPadBase + n * 0x134); }
static void __fastcall CandStick(u8* self, void* /*edx*/, u8* ped, float distance, float& verticalAngle, float& horizontalAngle, float& alphaStick, float& betaStick) {
    u8* car = *(u8**)(self + 0xC);
    if (!car || *(i32*)(car + 0x594) == 3) return;
    u8* pad = GetPadExe(0);
    if (*(i32*)(ped + 0x598) == 1) pad = GetPadExe(1);
    u8* sub = *(u8**)(self + 8);
    if (!sub) return;
    auto IsSub = [&](int t) { gTrace.push_back({ kGetTaskType, (u32)sub, 0 }); return int(gSubType) == t; };
    if (!(IsSub(802) || IsSub(802) || IsSub(804) || IsSub(820) || IsSub(807) || IsSub(808))) return;
    u8& choice = self[0x29];
    if (EnterCarCamStickBeyondCpp(FakeAimLR(pad, nullptr, ped)) || EnterCarCamStickBeyondCpp(FakeAimUD(pad, nullptr, ped))) {
        choice = 3;
    } else if (FakeAccel(pad, nullptr)) {
        choice = 2;
    } else if (FakeBrake(pad, nullptr)) {
        choice = 1;
    }
    if (choice == 3) return;
    u8* matrix = *(u8**)(car + 0x14);
    u8* pedMatrix = *(u8**)(ped + 0x14);
    const float* carPos = matrix ? (const float*)(matrix + 0x30) : (const float*)(car + 4);
    const float* pedPos = pedMatrix ? (const float*)(pedMatrix + 0x30) : (const float*)(ped + 4);
    float lateral[3];
    EnterCarCamLateralCpp((const float*)matrix, *(float*)(car + 0x10), carPos, pedPos, distance, lateral);
    const float minY = *(float*)((u8*)FakeColModel(car, nullptr) + 4);
    const unsigned c = choice;
    const float t1 = *(float*)(0x86E684 + 4 * c), t2 = *(float*)(0x86E678 + 4 * c), limit = *(float*)0x8D2ECC;
    EnterCarCamStickCpp(minY, distance, lateral[2], lateral[0], lateral[1], c, t1, t2, limit, &verticalAngle, &horizontalAngle, &alphaStick, &betaStick);
}

struct EnterStats { unsigned cases = 0, bad = 0, retBad = 0, stackBad = 0, nvBad = 0, memBad = 0, traceBad = 0, outBad = 0, cw = 0, top = 0, tw = 0, cc = 0, exc = 0, st = 0, deadRaw = 0; };
struct Cov { unsigned carNull = 0, heli = 0, subNull = 0, subOther = 0, subOk = 0, choiceSet[4] = {}, choice3 = 0, betaChanged = 0, alphaChanged = 0, noMatrixPed = 0; };

static u32 FBits(float f) { return Bits(f); }

struct Fixture {
    int mode;   // 0 normal
    bool carNull, subNull; u32 subtype; bool pedMatrix; u8 choice; u32 pedType;
    float matUp[2], matRight[3], carPos[3], pedPos[3], distance, minY, horiz, vert, alpha, beta, limit;
    short aimLR, aimUD, accel, brake; u32 subKind;
};

static bool gAdv;
static float Pick(Rng& r, float realScale, unsigned stressPct) {
    if (gAdv) return r.below(100) < 45 ? AdvFloat(r) : RealFloat(r, realScale);
    if (r.below(100) < stressPct) return StressFloat(r);
    return RealFloat(r, realScale);
}
static Fixture MakeFixture(Rng& r) {
    Fixture f{};
    f.carNull = r.below(100) < 3;
    f.subNull = r.below(100) < 4;
    f.subtype = r.below(100) < 4 ? 3 : (r.below(2) ? 0 : 9);
    f.pedMatrix = r.below(2);
    static const u8 choices[] = { 0, 0, 1, 1, 2, 2, 3, 0, 1, 2 };
    f.choice = r.below(100) < 2 ? u8(r.next()) : choices[r.below(sizeof choices)];
    f.pedType = r.below(100) < 40 ? 1 : (r.below(2) ? 0 : (r.next() & 0xFF));
    gAdv = r.below(5) == 0;
    const unsigned stress = r.below(4) == 0 ? 25 : 3;
    const float uy = RealFloat(r, 3.2f);
    f.matUp[0] = gAdv ? Pick(r, 1.f, 100) : (r.below(100) < stress ? StressFloat(r) : -sinf(uy)); f.matUp[1] = gAdv ? Pick(r, 1.f, 100) : (r.below(100) < stress ? StressFloat(r) : cosf(uy));
    const float ry = uy + 1.5707964f;
    f.matRight[0] = gAdv ? Pick(r, 1.f, 100) : (r.below(100) < stress ? StressFloat(r) : cosf(ry)); f.matRight[1] = gAdv ? Pick(r, 1.f, 100) : (r.below(100) < stress ? StressFloat(r) : sinf(ry)); f.matRight[2] = Pick(r, 0.2f, stress);
    for (int i = 0; i < 3; ++i) { f.carPos[i] = Pick(r, 500.f, stress); f.pedPos[i] = r.below(3) ? f.carPos[i] + RealFloat(r, 6.f) : Pick(r, 500.f, stress); }
    if (r.below(4) == 0) f.pedPos[2] = f.carPos[2] + RealFloat(r, 2.f);
    f.distance = gAdv ? Pick(r, 3.f, 100) : (r.below(100) < stress ? StressFloat(r) : (0.1f + r.unit() * 12.f));
    f.minY = gAdv ? Pick(r, 3.f, 100) : (r.below(100) < stress ? StressFloat(r) : -(0.5f + r.unit() * 4.f));
    f.horiz = gAdv ? Pick(r, 3.2f, 100) : (r.below(100) < stress ? StressFloat(r) : RealFloat(r, 3.2f));
    f.vert = gAdv ? Pick(r, 0.5f, 100) : r.below(100) < stress ? StressFloat(r) : (r.below(3) == 0 ? -0.1745329f + RealFloat(r, 1e-3f) : RealFloat(r, 0.6f));
    f.alpha = Pick(r, 1.5f, stress); f.beta = Pick(r, 1.5f, stress);
    f.limit = gAdv ? (r.below(2) ? -0.1745329f : Pick(r, 0.3f, 100)) : (r.below(100) < 5 ? StressFloat(r) : -0.1745329f);
    static const short sticks[] = { 0, 0, 0, 20, -20, 21, -21, 19, -19, 127, -128, 32767, -32768, 1000 };
    f.aimLR = r.below(4) == 0 ? sticks[r.below(sizeof sticks / sizeof sticks[0])] : (r.below(12) ? short(r.below(41)) - 20 : short(r.next()));
    f.aimUD = r.below(4) == 0 ? sticks[r.below(sizeof sticks / sizeof sticks[0])] : (r.below(12) ? short(r.below(41)) - 20 : short(r.next()));
    f.accel = r.below(2) ? 0 : short(r.next() | 1); f.brake = r.below(2) ? 0 : short(r.next() | 1);
    static const u32 kinds[] = { 802, 802, 804, 820, 807, 808, 808, 807, 701, 0, 0x2C0, 5000 };
    f.subKind = r.below(100) < 70 ? kinds[r.below(6)] : kinds[r.below(12)];
    return f;
}

static Fixture FromFx3(const Dependencies21EFixtures::Fx3& g) {
    using namespace Dependencies21EFixtures;
    Fixture f{};
    f.carNull = g.carNull; f.subNull = g.subNull; f.subtype = u32(g.carSubtype); f.pedMatrix = g.pedMatrix; f.choice = u8(g.choice0); f.pedType = g.pedType;
    for (int k = 0; k < 2; ++k) f.matUp[k] = AsFloat(g.up[k]);
    for (int k = 0; k < 3; ++k) { f.matRight[k] = AsFloat(g.right[k]); f.carPos[k] = AsFloat(g.carPos[k]); f.pedPos[k] = AsFloat(g.pedPos[k]); }
    f.distance = AsFloat(g.distance); f.minY = AsFloat(g.minY); f.horiz = AsFloat(g.horizontal); f.vert = AsFloat(g.vertical); f.alpha = AsFloat(g.alpha); f.beta = AsFloat(g.beta); f.limit = AsFloat(g.limit);
    f.aimLR = g.aimLR; f.aimUD = g.aimUD; f.accel = g.accel; f.brake = g.brake; f.subKind = u32(g.subKind);
    return f;
}
static void Setup(const Fixture& f) {
    memset(&gTask, 0xA5, sizeof gTask); memset(&gSub, 0xA6, sizeof gSub); memset(&gCar, 0xA7, sizeof gCar); memset(&gMat, 0xA8, sizeof gMat);
    memset(&gPed, 0xA9, sizeof gPed); memset(&gPedMat, 0xAA, sizeof gPedMat); memset(&gCol, 0xAB, sizeof gCol);
    for (int i = 0; i < 16; ++i) { gVtTask[i] = (void*)0xDEAD0000; gVtSub[i] = (void*)0xDEAD0100; }
    gVtTask[2] = (void*)&FakeGetSubTask; gVtSub[4] = (void*)&FakeGetTaskType;
    *(void**)(gTask.b + 0) = gVtTask; *(void**)(gTask.b + 8) = f.subNull ? nullptr : (void*)&gSub; *(void**)(gTask.b + 0xC) = f.carNull ? nullptr : (void*)&gCar;
    gTask.b[0x29] = f.choice;
    *(void**)(gSub.b + 0) = gVtSub;
    gSubType = f.subKind;
    *(u32*)(gCar.b + 0x594) = f.subtype; *(u32*)(gCar.b + 0x14) = (u32)&gMat; *(float*)(gCar.b + 0x10) = 0.37f;
    for (int i = 0; i < 3; ++i) { *(float*)(gMat.b + 0x30 + 4 * i) = f.carPos[i]; *(float*)(gCar.b + 4 + 4 * i) = f.carPos[i]; *(float*)(gMat.b + 4 * i) = f.matRight[i]; }
    *(float*)(gMat.b + 0x10) = f.matUp[0]; *(float*)(gMat.b + 0x14) = f.matUp[1];
    *(u32*)(gPed.b + 0x598) = f.pedType;
    *(u32*)(gPed.b + 0x14) = f.pedMatrix ? (u32)&gPedMat : 0;
    for (int i = 0; i < 3; ++i) { *(float*)(gPedMat.b + 0x30 + 4 * i) = f.pedPos[i]; *(float*)(gPed.b + 4 + 4 * i) = f.pedPos[i]; }
    *(float*)(gCol.b + 4) = f.minY;
    gAimLR = f.aimLR; gAimUD = f.aimUD; gAccel = f.accel; gBrake = f.brake;
    *(float*)0x8D2ECC = f.limit;
    gTrace.clear();
}
static std::vector<u8> Snapshot() {
    std::vector<u8> s;
    const Obj* os[] = { &gTask, &gSub, &gCar, &gMat, &gPed, &gPedMat, &gCol };
    for (auto* o : os) s.insert(s.end(), o->b, o->b + sizeof o->b);
    s.insert(s.end(), (u8*)kPadBase, (u8*)kPadBase + 2 * 0x134);
    return s;
}

void RunEnterCar(unsigned cases, unsigned seed, int verbosity, bool bank) {
    Jmp(0x541040, (void*)&FakeAimLR); Jmp(0x5410C0, (void*)&FakeAimUD); Jmp(0x5403F0, (void*)&FakeAccel); Jmp(0x540080, (void*)&FakeBrake); Jmp(0x535300, (void*)&FakeColModel);
    DWORD old; VirtualProtect((void*)0x8D2ECC, 4, PAGE_READWRITE, &old);
    EnterStats st; Cov cov; Rng rng(seed * 17 + 3);
    struct BaseCw { u16 cw, sw; };
    const BaseCw bases[] = { {gCw, 0x0000}, {gCw, 0x4700 | (3 << 11)}, {gCw, 0x0100 | (5 << 11) | 0x0020}, {gCw, 0x0400 | (7 << 11)} };
    unsigned firstBad = 0;
    if (bank) cases = Dependencies21EFixtures::kCases3;
    for (unsigned i = 0; i < cases; ++i) {
        const Fixture f = bank ? FromFx3(Dependencies21EFixtures::MakeFx3(i)) : MakeFixture(rng);
        const auto& bc = bank ? bases[Dependencies21EFixtures::MakeFx3(i).fp] : bases[rng.below(4)];
        Fp base = MakeBaseFp(bc.cw, bc.sw);
        float outA[4], outB[4];
        std::vector<u8> memA, memB; std::vector<Event> trA, trB; Res a, b;
        for (int side = 0; side < 2; ++side) {
            Setup(f);
            float v = f.vert, h = f.horiz, al = f.alpha, be = f.beta;
            u32 args[6] = { (u32)&gPed, Bits(f.distance), (u32)&v, (u32)&h, (u32)&al, (u32)&be };
            Res r;
            if (side == 0) r = Call(0x63A380, (u32)&gTask, 0xBBBB0002, 0xCCCC0003, args, 6, true, base);
            else r = Call((u32)&CandStick, (u32)&gTask, 0xBBBB0002, 0xCCCC0003, args, 6, true, base);
            float* o = side == 0 ? outA : outB;
            o[0] = v; o[1] = h; o[2] = al; o[3] = be;
            (side == 0 ? memA : memB) = Snapshot();
            (side == 0 ? trA : trB) = gTrace;
            (side == 0 ? a : b) = r;
        }
        FpDiff d = CompareFp(a.fp, b.fp);
        st.cases++;
        bool bad = false;
        if (!a.stackOk || !b.stackOk) { st.stackBad++; bad = true; }
        if (!a.nvOk || !b.nvOk) { st.nvBad++; bad = true; }
        if (memcmp(outA, outB, sizeof outA)) { st.outBad++; bad = true; }
        if (memA != memB) { st.memBad++; bad = true; }
        // la traza del EXE llama a GetSubTask (virtual); el candidato lee el puntero directamente: se ignora ese tipo de evento
        auto Strip = [](const std::vector<Event>& t) { std::vector<Event> o; for (auto& e : t) if (e.kind != kGetSubTask) o.push_back(e); return o; };
        if (Strip(trA) != Strip(trB)) { st.traceBad++; bad = true; }
        if (d.fcw) { st.cw++; bad = true; } if (d.top) { st.top++; bad = true; } if (d.ftw) { st.tw++; bad = true; } if (d.cc) { st.cc++; bad = true; }
        if (d.exc) { st.exc++; bad = true; } if (d.st) { st.st++; bad = true; } if (d.deadRaw) st.deadRaw++;
        // cobertura
        if (f.carNull) cov.carNull++; else if (f.subtype == 3) cov.heli++; else if (f.subNull) cov.subNull++;
        else if (!(f.subKind == 802 || f.subKind == 804 || f.subKind == 820 || f.subKind == 807 || f.subKind == 808)) cov.subOther++;
        else {
            cov.subOk++;
            if (memA.size() > 0 && memA[0x29] == 3) cov.choice3++;
            if (memA[0x29] < 3) cov.choiceSet[memA[0x29]]++;
            if (Bits(outA[3]) != Bits(f.beta)) cov.betaChanged++;
            if (Bits(outA[2]) != Bits(f.alpha)) cov.alphaChanged++;
        }
        if (bad) {
            st.bad++;
            if (verbosity && st.bad <= 6) {
                printf("  [63A380 #%u] ref fsw=%04x tw=%04x cand fsw=%04x tw=%04x | beta ref=%08x cand=%08x alpha ref=%08x cand=%08x | choice %02x | traceA=%zu traceB=%zu\n", i, FpSw(a.fp), FpTw(a.fp), FpSw(b.fp), FpTw(b.fp),
                       Bits(outA[3]), Bits(outB[3]), Bits(outA[2]), Bits(outB[2]), f.choice, trA.size(), trB.size());
                printf("    fx: choice=%u up=%08x,%08x right=%08x,%08x,%08x carPos=%08x,%08x,%08x pedPos=%08x,%08x,%08x dist=%08x minY=%08x horiz=%08x vert=%08x alpha=%08x beta=%08x limit=%08x\n", f.choice, FBits(f.matUp[0]), FBits(f.matUp[1]),
                       FBits(f.matRight[0]), FBits(f.matRight[1]), FBits(f.matRight[2]), FBits(f.carPos[0]), FBits(f.carPos[1]), FBits(f.carPos[2]), FBits(f.pedPos[0]), FBits(f.pedPos[1]), FBits(f.pedPos[2]),
                       FBits(f.distance), FBits(f.minY), FBits(f.horiz), FBits(f.vert), FBits(f.alpha), FBits(f.beta), FBits(f.limit));
                if (memA != memB) { for (size_t k = 0; k < memA.size(); ++k) if (memA[k] != memB[k]) { printf("    primer byte distinto de memoria: offset %zu ref=%02x cand=%02x\n", k, memA[k], memB[k]); break; } }
            }
        }
    }
    printf("0x63A380 EnterCar camara: casos=%u malos=%u | stack=%u nv=%u out=%u mem=%u traza=%u | fcw=%u top=%u ftw=%u cc=%u exc=%u st=%u | info deadRaw=%u\n", st.cases, st.bad, st.stackBad, st.nvBad, st.outBad, st.memBad,
           st.traceBad, st.cw, st.top, st.tw, st.cc, st.exc, st.st, st.deadRaw);
    printf("   cobertura: carNull=%u heli=%u subNull=%u subOtro=%u subOk=%u choice3=%u elegido0/1/2=%u/%u/%u beta cambia=%u alpha cambia=%u\n", cov.carNull, cov.heli, cov.subNull, cov.subOther, cov.subOk, cov.choice3,
           cov.choiceSet[0], cov.choiceSet[1], cov.choiceSet[2], cov.betaChanged, cov.alphaChanged);
}
