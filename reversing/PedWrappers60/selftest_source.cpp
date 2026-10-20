// Programa autonomo (sin el juego): compila el FUENTE REAL de CPed::ProcessControl, SpecialEntityPreCollisionStuff y SpecialEntityCalcCollisionSteps (extraidos de Ped.cpp por selftest_source.py en
// ped_funcs.inc) contra los tipos y cabeceras REALES del proyecto, con las opciones de depuracion del proyecto, y sustituye los colaboradores por definiciones que llaman a los mismos falsos que
// Core.hpp (Arenas.hpp). Las globales del juego (0xB7CB4C, 0xB7CB5C, 0xB72914, 0xB7CD6E, TheCamera, la tabla de modelos, ...) viven en memoria reservada en esas mismas direcciones absolutas.
// Imprime una linea JSON por caso con la misma forma que Core.hpp registra en `observed`; selftest_source.py las compara con fixture_oracle.py.
#include "StdInc.h"
#include "Ped.h"
#include "PedPlacement.h"
#include "X87Math.h"
#include "PedType.h"
#include "Buoyancy.h"
#include "TaskSimpleSwim.h"
#include "PedStats.h"
#include "Conversations.h"
#include "TaskSimpleLand.h"
#include "AEAudioUtility.h"
#include "PedClothesDesc.h"
#include "WeaponInfo.h"
#include "Shadows.h"
#include "Crime.h"
#include "FxManager.h"
#include "FxSystem.h"
#include "VisibilityPlugins.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "Population.h"
#include "Localisation.h"
#include "Collision/Collision.h"
#include "Entity/Object/Object.h"
#include "ObjectData.h"
#include "Arenas.hpp"
#include "Fixtures.hpp"
#include <cstdio>
#include <string>

using namespace PedWrappers60Live;

// ------------------------------------------------------------------------------------------------ memoria del juego en direcciones absolutas
// El codigo del juego lee sus globales por direccion absoluta (0xA9B0C8 tabla de modelos, 0xB6F028 TheCamera, 0xB72914, 0xB7CB4C, 0xB7CB5C, 0xB7CD6E, 0xC40400, ...). El programa se enlaza con /BASE tal que
// la seccion ".gm" (sin datos en el archivo, 0x1D0000 bytes) cubra de 0xA90000 a 0xC60000: selftest_source.py calcula la base a partir del RVA de ".gm". Aqui solo se comprueba.
#pragma bss_seg(".gm")
extern "C" char gGameMem[0x1D0000];
char gGameMem[0x1D0000];
#pragma bss_seg()
static bool MapGameMemory() {
    const uintptr_t lo = uintptr_t(&gGameMem[0]), hi = lo + sizeof gGameMem;
    if (lo > 0xA9AE80 || hi < 0xC41000) {
        std::fprintf(stderr, "la seccion .gm esta en %#x..%#x y no cubre las globales del juego (0xA9AE80..0xC41000)\n", unsigned(lo), unsigned(hi));
        return false;
    }
    return true;
}
CCamera& TheCamera = *reinterpret_cast<CCamera*>(0xB6F028);
FxManager_c& g_fxMan = *reinterpret_cast<FxManager_c*>(0xA9AE80);

// rand del EXE (0x821B1E): mismo LCG que el CRT
static u32 gRng{};
static int gRandCalls{};
static int PedTestExeRand() { gRng = gRng * 0x343FDu + 0x269EC3u; ++gRandCalls; return int((gRng >> 16) & 0x7FFFu); }

// ------------------------------------------------------------------------------------------------ colaboradores (mismas firmas que las cabeceras reales)
void CAEPedAudioEntity::Service() { FakeAeService(this, nullptr); }
CTaskSimpleSwim* CPedIntelligence::GetTaskSwim() { return reinterpret_cast<CTaskSimpleSwim*>(uintptr_t(FakeSwim(this, nullptr))); }
CTaskSimpleClimb* CPedIntelligence::GetTaskClimb() { return reinterpret_cast<CTaskSimpleClimb*>(uintptr_t(FakeClimb(this, nullptr))); }
void CPedIntelligence::ProcessFirst() { FakePFirst(this, nullptr); }
void CPedIntelligence::Process() { FakePProc(this, nullptr); }
RpHAnimHierarchy* GetAnimHierarchyFromSkinClump(RpClump* clump) { return reinterpret_cast<RpHAnimHierarchy*>(uintptr_t(FakeGetHier(u32(uintptr_t(clump))))); }
RwInt32 RpHAnimIDGetIndex(RpHAnimHierarchy* hierarchy, RwInt32 id) { return RwInt32(FakeBoneIdx(u32(uintptr_t(hierarchy)), u32(id))); }
RwMatrix* RpHAnimHierarchyGetMatrixArray(RpHAnimHierarchy* hierarchy) { return reinterpret_cast<RwMatrix*>(uintptr_t(FakeMatArr(u32(uintptr_t(hierarchy))))); }
FxSystem_c* FxManager_c::CreateFxSystem(const char* name, const CVector& point, RwMatrix* objectMatrix, bool ignoreBoundingChecks) {
    return reinterpret_cast<FxSystem_c*>(uintptr_t(FakeCreateFx(this, nullptr, name, reinterpret_cast<const u8*>(&point), u32(uintptr_t(objectMatrix)), ignoreBoundingChecks ? 1u : 0u)));
}
void FxSystem_c::SetLocalParticles(bool enable) { FakeFxLocal(this, nullptr, enable ? 1u : 0u); }
void FxSystem_c::CopyParentMatrix() { FakeFxCopy(this, nullptr); }
void FxSystem_c::Play() { FakeFxPlay(this, nullptr); }
void FxManager_c::DestroyFxSystem(FxSystem_c* system) { FakeFxDestroy(this, nullptr, system); }
CVector FindPlayerCoors(int32 playerId) { CVector v; FakeCoors(reinterpret_cast<u32*>(&v), playerId); return v; }
CPlayerPed* FindPlayerPed(int32 playerId) { return reinterpret_cast<CPlayerPed*>(uintptr_t(FakeFindPlayer(playerId))); }
void CEntity::PruneReferences() { FakePrune(this, nullptr); }
int32 CVisibilityPlugins::GetClumpAlpha(RpClump* clump) { return int32(FakeGetAlpha(u32(uintptr_t(clump)))); }
void CVisibilityPlugins::SetClumpAlpha(RpClump* clump, int32 alpha) { FakeSetAlpha(u32(uintptr_t(clump)), u32(alpha)); }
int32 CCollision::ProcessColModels(const CMatrix& a, CColModel& cmA, const CMatrix& b, CColModel& cmB, std::array<CColPoint, 32>& points, CColPoint* lineCPs, float* touch, bool arg7) {
    return int32(FakeColModels(u32(uintptr_t(&a)), u32(uintptr_t(&cmA)), u32(uintptr_t(&b)), u32(uintptr_t(&cmB)), u32(uintptr_t(&points)), u32(uintptr_t(lineCPs)), u32(uintptr_t(touch)), arg7 ? 1u : 0u));
}
CPedGroup& CPedGroups::GetGroup(int32 groupId) { return reinterpret_cast<CPedGroup*>(0xC09928)[groupId]; }
bool CPedGroupMembership::IsMember(const CPed* ped) const { return (FakeIsMember(const_cast<CPedGroupMembership*>(this), nullptr, const_cast<CPed*>(ped)) & 0xFFu) != 0; }
void CPed::ProcessBuoyancy() { FakeBuoy(this, nullptr); }
void CPed::PlayFootSteps() { FakeFoot(this, nullptr); }
void CPed::RequestDelayedWeapon() { FakeDelW(this, nullptr); }
void CPed::CalculateNewVelocity() { FakeNewVel(this, nullptr); }
void CPed::UpdatePosition() { FakeUpdPos(this, nullptr); }
int16 CPed::Say(eGlobalSpeechContext ctx, uint32 delay, float prob, bool b1, bool b2, bool b3) {
    return int16(FakeSay(this, nullptr, u32(ctx), delay, FBits(prob), b1 ? 1u : 0u, b2 ? 1u : 0u, b3 ? 1u : 0u));
}
void CPhysical::SkipPhysics() { FakeSkip(this, nullptr); }
void CPhysical::ProcessControl() { FakePhysPc(this, nullptr); }
bool CLocalisation::Blood() { return (FakeBlood() & 0xFFu) != 0; }
void CShadows::AddPermanentShadow(uint8 type, RwTexture* texture, CVector* posn, float topX, float topY, float rightX, float rightY, int16 intensity, uint8 red, uint8 green, uint8 blue, float drawDistance, uint32 time, float upDistance) {
    FakeShadow(type, u32(uintptr_t(texture)), reinterpret_cast<const u8*>(posn), FBits(topX), FBits(topY), FBits(rightX), FBits(rightY), u32(uint16(intensity)), red, green, blue, FBits(drawDistance), time, FBits(upDistance));
}
void CPopulation::UpdatePedCount(CPed* ped, bool pedAddedOrRemoved) { FakePedCount(ped, pedAddedOrRemoved ? 1u : 0u); }
void CAEPedWeaponAudioEntity::AddAudioEvent(eAudioEvents event) { FakeWaeAdd(this, nullptr, u32(event)); }
void CAEPedWeaponAudioEntity::Service() { FakeWaeService(this, nullptr); }

// ------------------------------------------------------------------------------------------------ el fuente real
#define PedCtlExeRand PedTestExeRand_unused
#include "ped_funcs.inc"
#undef PedCtlExeRand

// ------------------------------------------------------------------------------------------------ ejecucion de casos
static std::string Events() {
    std::string s = "[";
    for (unsigned i = 0; i < nev; ++i) {
        if (i) s += ',';
        s += '[';
        for (u32 k = 0; k < kEvWidth; ++k) { if (k) s += ','; s += std::to_string(evs[i].w[k]); }
        s += ']';
    }
    return s + "]";
}
static std::string Changes(const std::vector<std::array<int, 3>>& v) {
    std::string s = "[";
    for (size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += '[' + std::to_string(v[i][0]) + ',' + std::to_string(v[i][1]) + ',' + std::to_string(v[i][2]) + ']'; }
    return s + "]";
}
static void Prepare(bool pokesOnly = false) { (void)pokesOnly; _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM); }

static void RunCS(unsigned n) {
    const KCase c = MakeK(n);
    gP = nullptr; gS = nullptr; gRet = Rand(n, 59); nev = 0;
    InitSlots();
    ApplyPokes(slots[L_PED], c.ped, Rand(n, 50));
    FillWords(slots[L_BOOLS].ref, slots[L_BOOLS].arena, Rand(n, 51));
    for (int i = 0; i < 2; ++i) slots[L_BOOLS].ref[kBoolsOff + i] = u8(c.bools[i]);
    for (auto& sl : slots) if (sl.now) std::memcpy(sl.now, sl.ref, sl.arena);
    *reinterpret_cast<u32*>(kGTimestep) = c.timestep;
    Prepare();
    auto* ped = reinterpret_cast<CPed*>(AddrOf(L_PED));
    auto* b = reinterpret_cast<bool*>(AddrOf(L_BOOLS));
    const uint8 ret = ped->CPed::SpecialEntityCalcCollisionSteps(b[0], b[1]);
    std::vector<std::array<int, 3>> ch;
    Diff(ch, 0, slots[L_PED]);
    std::printf("K {\"ret\":%u,\"changes\":%s,\"bools\":[%u,%u],\"bools_unchanged\":%s}\n", unsigned(ret), Changes(ch).c_str(), unsigned(slots[L_BOOLS].now[kBoolsOff]), unsigned(slots[L_BOOLS].now[kBoolsOff + 1]),
                SameArena(slots[L_BOOLS]) ? "true" : "false");
}
static void RunPS(unsigned n) {
    const SCase c = MakeS(n);
    gP = nullptr; gS = &c; gRet = Rand(n, 59); nev = 0;
    InitSlots();
    ApplyPokes(slots[L_PED], c.ped, Rand(n, 50)); ApplyPokes(slots[L_ENT], c.ent, Rand(n, 51)); ApplyPokes(slots[L_OBJINFO], c.objinfo, Rand(n, 52));
    ApplyPokes(slots[L_MATPED], c.matPed, Rand(n, 53)); ApplyPokes(slots[L_MATOTHER], c.matEnt, Rand(n, 54));
    FillWords(slots[L_BOOLS].ref, slots[L_BOOLS].arena, Rand(n, 55));
    for (int i = 0; i < 4; ++i) slots[L_BOOLS].ref[kBoolsOff + i] = u8(c.bools[i]);
    FillWords(slots[L_INTEL].ref, slots[L_INTEL].arena, Rand(n, 56));
    for (auto& sl : slots) if (sl.now) std::memcpy(sl.now, sl.ref, sl.arena);
    Prepare();
    auto* ped = reinterpret_cast<CPed*>(AddrOf(L_PED));
    auto* col = reinterpret_cast<CPhysical*>(AddrOf(L_ENT));
    auto* b = reinterpret_cast<bool*>(AddrOf(L_BOOLS));
    ped->CPed::SpecialEntityPreCollisionStuff(col, (c.ignoreArg & 0xFFu) != 0, b[0], b[1], b[2], b[3]);
    std::vector<std::array<int, 3>> ch;
    Diff(ch, 0, slots[L_PED]); Diff(ch, 1, slots[L_ENT]);
    bool others = true;
    for (u32 l : {u32(L_OBJINFO), u32(L_MATPED), u32(L_MATOTHER), u32(L_INTEL)}) others &= SameArena(slots[l]);
    const auto& bs = slots[L_BOOLS];
    const bool guards = std::memcmp(bs.now, bs.ref, kBoolsOff) == 0 && std::memcmp(bs.now + kBoolsOff + 4, bs.ref + kBoolsOff + 4, bs.arena - kBoolsOff - 4) == 0;
    std::printf("S {\"ret\":null,\"events\":%s,\"changes\":%s,\"bools\":[%u,%u,%u,%u],\"others_unchanged\":%s}\n", Events().c_str(), Changes(ch).c_str(), unsigned(bs.now[kBoolsOff]), unsigned(bs.now[kBoolsOff + 1]),
                unsigned(bs.now[kBoolsOff + 2]), unsigned(bs.now[kBoolsOff + 3]), (others && guards) ? "true" : "false");
}
static void RunPC(unsigned n) {
    const PCase c = MakeP(n);
    gP = &c; gS = nullptr; gRet = Rand(n, 59); nev = 0;
    InitSlots();
    ApplyPokes(slots[L_PED], c.ped, Rand(n, 50)); ApplyPokes(slots[L_PDATA], c.pdata, Rand(n, 51)); ApplyPokes(slots[L_PDATA2], c.pdata2, Rand(n, 52)); ApplyPokes(slots[L_PPED], c.pped, Rand(n, 53));
    ApplyPokes(slots[L_ENT], c.ent, Rand(n, 54)); ApplyPokes(slots[L_VEH], c.veh, Rand(n, 55)); ApplyPokes(slots[L_MATPED], c.matPed, Rand(n, 56)); ApplyPokes(slots[L_MATOTHER], c.matVeh, Rand(n, 57));
    ApplyPokes(slots[L_MATCAM], c.matCam, Rand(n, 58)); ApplyPokes(slots[L_CLUMP], c.clump, Rand(n, 60));
    for (u32 l : {u32(L_MI0), u32(L_MI1), u32(L_OBJINFO), u32(L_FX), u32(L_HIER), u32(L_INTEL), u32(L_WOBJ), u32(L_CM0), u32(L_CM1), u32(L_TEX), u32(L_MATARR)}) FillWords(slots[l].ref, slots[l].arena, Rand(n, 61 + l));
    const auto mi = MiTable(c);
    for (const auto& e : mi) { const u32 cm = AddrOf(e.cm); std::memcpy(slots[e.mi].ref + slots[e.mi].off + 0x14, &cm, 4); }
    {
        auto& v = slots[L_VTBL];
        FillWords(v.ref, v.arena, Rand(n, 80));
        for (u32 i = 0; i < 26; ++i) { const u32 a = u32(uintptr_t(&FakeBadVirt)); std::memcpy(v.ref + v.off + 4 * i, &a, 4); }
        const u32 a = u32(uintptr_t(&FakeMoveAnim)); std::memcpy(v.ref + v.off + 4 * kSlotMoveAnim, &a, 4);
    }
    for (auto& sl : slots) if (sl.now) std::memcpy(sl.now, sl.ref, sl.arena);
    *reinterpret_cast<u32*>(kGFrame) = c.frame; *reinterpret_cast<u32*>(kGTimestep) = c.timestep; *reinterpret_cast<u32*>(kGArea) = c.area; *reinterpret_cast<u8*>(kGForce) = u8(c.force);
    *reinterpret_cast<u32*>(kGCamMat) = ResolvePtr(c.camMat); std::memcpy(reinterpret_cast<void*>(kGCamPos), c.camPos.data(), 12); *reinterpret_cast<u32*>(kGBloodTex) = AddrOf(L_TEX);
    for (const auto& e : mi) *reinterpret_cast<u32*>(kGModelInfo + 4 * e.idx) = AddrOf(e.mi);
    gRng = c.seed; gRandCalls = 0;
    Prepare();
    auto* ped = reinterpret_cast<CPed*>(AddrOf(L_PED));
    ped->CPed::ProcessControl();
    std::vector<std::array<int, 3>> ch;
    Diff(ch, 0, slots[L_PED], 0x44, 0x50); Diff(ch, 1, slots[L_PDATA]);
    bool others = true;
    for (u32 l : {u32(L_PDATA2), u32(L_PPED), u32(L_ENT), u32(L_VEH), u32(L_MATPED), u32(L_MATOTHER), u32(L_MATCAM), u32(L_MI0), u32(L_MI1), u32(L_FX), u32(L_HIER), u32(L_CLUMP), u32(L_INTEL), u32(L_WOBJ), u32(L_CM0),
                  u32(L_CM1), u32(L_TEX), u32(L_MATARR), u32(L_VTBL)}) others &= SameArena(slots[l]);
    const u8* p = slots[L_PED].now + kPedOff;
    std::printf("P {\"ret\":null,\"events\":%s,\"changes\":%s,\"speed\":[%u,%u,%u],\"rand_calls\":%d,\"others_unchanged\":%s,\"globals_unchanged\":true}\n", Events().c_str(), Changes(ch).c_str(),
                Bits(p + 0x44), Bits(p + 0x48), Bits(p + 0x4C), gRandCalls, others ? "true" : "false");
}

int main() {
    if (!MapGameMemory()) return 2;
    InitSlots();
    std::printf("A [");
    for (u32 l = 0; l < L_COUNT; ++l) std::printf("%s%u", l ? "," : "", AddrOf(l));
    std::printf("]\n");
    for (unsigned n = 0; n < kCasesCS; ++n) RunCS(n);
    for (unsigned n = 0; n < kCasesPS; ++n) RunPS(n);
    for (unsigned n = 0; n < kCasesPC; ++n) RunPC(n);
    return 0;
}
