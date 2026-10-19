"""Autoprueba del FUENTE (sin el juego ni el EXE): compila el cuerpo real de CTaskSimpleFight::ProcessPed de TaskSimpleFight.cpp contra maquetas minimas de los tipos del juego
(mismos desplazamientos, comprobados con static_assert) y falsos que registran los mismos eventos que el banco, ejecuta las 4608 fixtures de Fixtures.hpp y compara cada resultado con
fixture_oracle.py. Es independiente del EXE: solo comprueba que la transcripcion C++ y el oraculo (ambos escritos desde el desensamblado) coinciden. No sustituye al banco en juego.
Uso:  python -I selftest_source.py            (imprime «ok» o los casos distintos)
"""
import json
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
VS = Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC')
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402

SRC = (ROOT / 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp').read_text(encoding='utf-8').replace('\r\n', '\n')


def cut(a, b, start=0):
    i = SRC.index(a, start)
    return SRC[i:SRC.index(b, i)]


# fragmentos del fuente: constantes y ayudantes comunes, ayudantes de ProcessPed y el cuerpo de la funcion
early = cut('constexpr uint32 FightTableAddress', '// 0x61C740 (ret 4)')
helpers = cut('using FightAnimCallback', 'constexpr uintptr_t FightLevelsAddress')
helpers += 'constexpr uintptr_t FightLevelsAddress = 0xC177D0;\n'
helpers += cut('constexpr float FightIdleScale', 'constexpr float FightMeleeDataK')
helpers += cut('constexpr float FightHalfAnim', 'constexpr float FightBlendOutAbort')
helpers += cut('const uint8* FightComboRow', '// Colaboradores sin reconstruir')
helpers += cut('// Colaboradores sin reconstruir', '} // namespace\n\n// 0x5BEDC0')
func = cut('#pragma float_control(push)', '// 0x61D5F0 (ret 4)').replace('reinterpret_cast<const uint32*>(0x8D2E3C)', 'kAltSounds')

MOCK = r'''
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <bit>
#include <array>
#include <span>
#include <string>
#include <vector>
#include "%(fix)s/Fixtures.hpp"
using namespace FightProcessPed31Fix;
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using int32 = std::int32_t; using int16 = std::int16_t; using int8 = std::int8_t; using int64 = std::int64_t;
#define ZoneScoped
template<class T> T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }
struct CVector { float x, y, z; };
enum AssocGroupId : int32 {}; enum AnimationId : int32 {};
enum eWeaponType : uint32 { WEAPON_CHAINSAW = 9 };
enum class eWeaponSkill : uint8 { POOR, STD, PRO, COP };
enum eAudioEvents : int32 { AE_PED_SWING = 60, AE_WEAPON_CHAINSAW_ACTIVE = 154 };
enum eSurfaceType : int32 {};
enum eMoveState : int32 { PEDMOVE_STILL = 1 };
enum eFightAttackType : int8 { FIGHT_ATTACK_HIT_1 = 0, FIGHT_ATTACK_HIT_2 = 1, FIGHT_ATTACK_HIT_3 = 2, FIGHT_ATTACK_FIGHT_BLOCK = 3, FIGHT_ATTACK_FIGHTIDLE = 4 };
enum eAnimationFlags { ANIMATION_IS_PLAYING = 1 };
struct RpClump {};
struct CMeleeInfo { char pad[0x88]; };
struct CMatrix {
    CVector right; int f0; CVector fwd; int f1; CVector up; int f2; CVector pos; int f3; int pad[2];
    CVector& GetRight() const { return const_cast<CVector&>(right); } CVector& GetForward() const { return const_cast<CVector&>(fwd); }
    CVector& GetUp() const { return const_cast<CVector&>(up); } CVector& GetPosition() const { return const_cast<CVector&>(pos); }
};
static_assert(sizeof(CMatrix) == 0x48 && offsetof(CMatrix, pos) == 0x30 && offsetof(CMatrix, up) == 0x20 && offsetof(CMatrix, fwd) == 0x10, "CMatrix");
typedef CMatrix CMatrixLink;
struct CAnimBlendHierarchy { char pad[0x10]; float m_fTotalTime; char pad2[0x4]; };
struct CAnimBlock { char Name[16]; bool IsLoaded; short RefCnt; int FirstAnimIdx; unsigned NumAnims; int GroupId; };
static_assert(sizeof(CAnimBlock) == 0x20 && offsetof(CAnimBlock, IsLoaded) == 0x10, "CAnimBlock");
struct CAnimBlendAssocGroup { CAnimBlock* m_AnimBlock; char pad[0x10]; };
struct CAnimBlendAssociation {
    char pad0[0x14]; CAnimBlendHierarchy* m_BlendHier; float m_BlendAmount, m_BlendDelta, m_CurrentTime, m_Speed, m_TimeStep; int16 m_AnimId; uint16 m_Flags; char pad1[0xC];
    void SetDeleteCallback(void (*cb)(CAnimBlendAssociation*, void*), void* data = nullptr);
    void SetCurrentTime(float t);
};
static_assert(sizeof(CAnimBlendAssociation) == 0x3C && offsetof(CAnimBlendAssociation, m_Flags) == 0x2E && offsetof(CAnimBlendAssociation, m_TimeStep) == 0x28, "CAnimBlendAssociation");
struct CWeapon { uint32 m_Type; char pad[0x18]; };
static_assert(sizeof(CWeapon) == 0x1C, "CWeapon");
struct CWeaponInfo { char pad[0x6E]; uint8 m_nBaseCombo; uint8 m_nNumCombos; static CWeaponInfo* GetWeaponInfo(eWeaponType, eWeaponSkill); };
static_assert(sizeof(CWeaponInfo) == 0x70, "CWeaponInfo");
struct CAEPedAudioEntity { char pad[0x25C]; void AddAudioEvent(eAudioEvents, float, float, void*, eSurfaceType, int32, uint32); };
struct CAEPedWeaponAudioEntity { char pad[0xA8]; void AddAudioEvent(eAudioEvents); };
struct CEntity {
    void* vt; CVector pp; float heading; CMatrix* m_matrix; void* m_pRwObject;
    const CVector& GetPosition() const { return m_matrix ? m_matrix->pos : pp; }
};
static_assert(offsetof(CEntity, m_matrix) == 0x14 && offsetof(CEntity, m_pRwObject) == 0x18, "CEntity");
struct CPad { char pad[16]; bool GetTarget() const; };
struct CPed : CEntity {
    char pad1[0x44 - 0x1C]; CVector m_vecMoveSpeed; char pad2[0x138 - 0x50]; CAEPedAudioEntity m_pedAudio; CAEPedWeaponAudioEntity m_weaponAudio; char pad3[0x4D4 - (0x394 + 0xA8)];
    AssocGroupId m_nAnimGroup; char pad4[0x534 - 0x4D8]; int32 m_nMoveState; int32 m_nSwimmingMoveState; char pad5[0x55C - 0x53C]; float m_fAimingRotation; char pad6[0x5A0 - 0x560];
    std::array<CWeapon, 13> m_aWeapons; char pad7[0x718 - (0x5A0 + 13 * 0x1C)]; uint8 m_nActiveWeaponSlot; char pad8[0x72D - 0x719]; int8 m_nFightingStyle; char pad9[0x730 - 0x72E];
    bool IsPlayer() const; void SetMoveState(eMoveState); RpClump* GetRwObject() const { return reinterpret_cast<RpClump*>(m_pRwObject); }
    CAEPedAudioEntity& GetAE() { return m_pedAudio; } CAEPedWeaponAudioEntity& GetWeaponAE() { return m_weaponAudio; }
};
static_assert(sizeof(CPed) == 0x730 && offsetof(CPed, m_nAnimGroup) == 0x4D4 && offsetof(CPed, m_nMoveState) == 0x534 && offsetof(CPed, m_fAimingRotation) == 0x55C && offsetof(CPed, m_aWeapons) == 0x5A0, "CPed");
static_assert(offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_nFightingStyle) == 0x72D && offsetof(CPed, m_pedAudio) == 0x138 && offsetof(CPed, m_weaponAudio) == 0x394, "CPed 2");
static_assert(offsetof(CPed, m_vecMoveSpeed) == 0x44, "CPed 3");
struct CPlayerPed : CPed { CPad* GetPadFromPlayer() const; };
struct CCamera { static inline bool& m_bUseMouse3rdPerson = *reinterpret_cast<bool*>(0xB6EC2E); };
struct CTimer { static inline float& ms_fTimeStep = *reinterpret_cast<float*>(0xB7CB5C); };
struct CStreaming { static void RequestModel(int32 id, int32 flags); };
struct CAnimManager {
    static inline CAnimBlendAssocGroup*& ms_aAnimAssocGroups = *reinterpret_cast<CAnimBlendAssocGroup**>(0xB4EA34);
    static std::span<CAnimBlendAssocGroup> GetAssocGroups() { return std::span<CAnimBlendAssocGroup>(ms_aAnimAssocGroups, 4); }
    static CAnimBlendAssociation* BlendAnimation(RpClump*, AssocGroupId, AnimationId, float);
    static void RemoveAnimBlockRef(int32); static void AddAnimBlockRef(int32); static const char* GetAnimBlockName(AssocGroupId); static CAnimBlock* GetAnimationBlock(const char*);
};
struct CTaskSimpleFight {
    void* vt; void* parent;
    bool m_bIsFinished, m_bIsInControl, m_bAnimsReferenced; AssocGroupId m_nRequiredAnimGroup; uint16 m_nIdlePeriod, m_nIdleCounter; int8 m_nContinueStrike, m_nChainCounter;
    CEntity* m_pTargetEntity; CAnimBlendAssociation* m_pAnim; CAnimBlendAssociation* m_pIdleAnim; int8 m_nComboSet; eFightAttackType m_nCurrentMove; uint8 m_nNextCommand, m_nLastCommand;
    bool ProcessPed(CPed* ped); void FightStrike(CPed* ped, CVector& pos);
};
static_assert(sizeof(CTaskSimpleFight) == 0x28 && offsetof(CTaskSimpleFight, m_pAnim) == 0x1C && offsetof(CTaskSimpleFight, m_nComboSet) == 0x24 && offsetof(CTaskSimpleFight, m_nRequiredAnimGroup) == 0xC, "CTaskSimpleFight");
inline uint32 FightU32(const void* pointer) { return static_cast<uint32>(reinterpret_cast<uintptr_t>(pointer)); }
inline float TrigAtan2Cpp(float y, float x) { return float(atan2(double(y), double(x))); }

// ---- falsos y trazas
static const C* gC;
static u8 *pThis, *pAnim, *pIdle, *pNew, *pPed, *pTgt, *pPad, *pWinfo; static CAnimBlock* gBlocks;
static u32 nev; static u32 evw[96][9];
static void Push(u32 k, u32 a = 0, u32 b = 0, u32 c = 0, u32 d = 0, u32 e = 0, u32 f = 0, u32 g = 0, u32 h = 0) { if (nev < 96) { u32 w[9] = {k, a, b, c, d, e, f, g, h}; std::memcpy(evw[nev], w, 36); } ++nev; }
static u32 Label(const void* p) {
    if (!p) return 0; const u8* q = static_cast<const u8*>(p);
    if (q == pThis) return 1; if (q == pAnim) return 2; if (q == pIdle) return 3; if (q == pNew) return 4; if (q == pPed) return 5; if (q == pTgt) return 6; if (q == pPad) return 7;
    if (q > pPed && q < pPed + 0x730) return 0x100 + u32(q - pPed);
    return 0xBAD;
}
static u32 LabelU(const void* v) { return v ? Label(v) : 0; }
static u32 BitsOf(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
static u32 BitsAt(const void* p) { u32 b; std::memcpy(&b, p, 4); return b; }
void CAnimBlendAssociation::SetDeleteCallback(void (*cb)(CAnimBlendAssociation*, void*), void* data) { Push(1, Label(this), u32(uintptr_t(cb)), LabelU(data)); }
void CAnimBlendAssociation::SetCurrentTime(float t) { Push(3, Label(this), BitsOf(t)); }
bool CPed::IsPlayer() const { Push(4, Label(this)); return gC->isPlayer != 0; }
void CPed::SetMoveState(eMoveState s) { Push(5, Label(this), u32(s)); }
void CAEPedAudioEntity::AddAudioEvent(eAudioEvents e, float v, float s, void* ph, eSurfaceType su, int32 a7, uint32 mv) { Push(12, Label(this), u32(e), BitsOf(v), BitsOf(s), LabelU(ph), u32(su), u32(a7), mv); }
void CAEPedWeaponAudioEntity::AddAudioEvent(eAudioEvents e) { Push(13, Label(this), u32(e)); }
CPad* CPlayerPed::GetPadFromPlayer() const { Push(20, Label(this)); return reinterpret_cast<CPad*>(pPad); }
bool CPad::GetTarget() const { Push(21, Label(this)); return (gC->getTarget & 0xFF) != 0; }
void CStreaming::RequestModel(int32 id, int32 flags) { Push(10, u32(id), u32(flags)); }
CAnimBlendAssociation* CAnimManager::BlendAnimation(RpClump* c, AssocGroupId g, AnimationId a, float d) { Push(2, u32(uintptr_t(c)), u32(g), u32(a), BitsOf(d)); return reinterpret_cast<CAnimBlendAssociation*>(pNew); }
void CAnimManager::RemoveAnimBlockRef(int32 i) { Push(7, u32(i)); }
void CAnimManager::AddAnimBlockRef(int32 i) { Push(6, u32(i)); }
const char* CAnimManager::GetAnimBlockName(AssocGroupId g) { Push(8, u32(g)); return reinterpret_cast<const char*>(uintptr_t(0x4E414D45)); }
CAnimBlock* CAnimManager::GetAnimationBlock(const char* n) { Push(9, u32(uintptr_t(n))); return &gBlocks[gC->nameBlock - 1]; }
CWeaponInfo* CWeaponInfo::GetWeaponInfo(eWeaponType t, eWeaponSkill s) { Push(11, u32(t), u32(s) & 0xFF); return reinterpret_cast<CWeaponInfo*>(pWinfo); }
void CTaskSimpleFight::FightStrike(CPed* ped, CVector& pos) { Push(14, Label(this), Label(ped), BitsAt(&pos.x), BitsAt(&pos.y), BitsAt(&pos.z)); }
namespace plugin {
template<class R, uintptr_t A, class... T> R CallMethodAndReturn(T... a);
template<uintptr_t A, class... T> void CallMethod(T... a);
template<> inline int8 CallMethodAndReturn<int8, 0x61C7F0, CTaskSimpleFight*, CPed*, int32>(CTaskSimpleFight* t, CPed* p, int32 cmd) { Push(15, Label(t), Label(p), u32(cmd) & 0xFF); return int8(gC->rAvail & 0xFF); }
template<> inline int16 CallMethodAndReturn<int16, 0x624710, CTaskSimpleFight*, CPed*>(CTaskSimpleFight* t, CPed* p) { Push(17, Label(t), Label(p)); return int16(gC->rAttack & 0xFFFF); }
template<> inline int16 CallMethodAndReturn<int16, 0x624A40, CTaskSimpleFight*, CPed*>(CTaskSimpleFight* t, CPed* p) { Push(18, Label(t), Label(p)); return int16(gC->rAttack & 0xFFFF); }
template<> inline void CallMethod<0x61C9B0, CTaskSimpleFight*, CPed*>(CTaskSimpleFight* t, CPed* p) { Push(16, Label(t), Label(p)); }
template<> inline void CallMethod<0x623B10, CTaskSimpleFight*, CPed*, int32>(CTaskSimpleFight* t, CPed* p, int32 a) { Push(19, Label(t), Label(p), u32(a)); }
}
static const uint32 kAltSounds[3] = {0x12C, 0x190, 0x1F4};   // .rdata del EXE en 0x8D2E3C
%(early)s
%(helpers)s
%(func)s
// ---- bucle de casos
template<size_t N> struct Ar { alignas(16) std::array<u8, N> now{}, ref{}; };
static Ar<kThisBytes> aThis; static Ar<kAnimBytes> aAnim, aIdle, aNew; static Ar<0x20> aHier; static Ar<0x730> aPed; static Ar<0x48> aMat, aTMat; static Ar<0x18> aTgt; static Ar<0x60> aBlocks; static Ar<0x50> aGroups; static Ar<0x70> aWinfo; static Ar<0x10> aPad;
static u32 U32(u32 v) { return v; }
template<size_t N> static void Put32(std::array<u8, N>& a, u32 at, u32 v) { std::memcpy(a.data() + at, &v, 4); }
template<size_t N> static void Put16(std::array<u8, N>& a, u32 at, u32 v) { const u16 w = u16(v); std::memcpy(a.data() + at, &w, 2); }
static void Out(const char* name, std::vector<std::array<u32, 3>>& ch) { (void)name; (void)ch; }
template<size_t N> static void Diff(std::vector<std::array<int, 3>>& out, int arena, const std::array<u8, N>& now, const std::array<u8, N>& ref, u32 skip0 = 0, u32 skip1 = 0) {
    for (u32 i = 0; i < N; ++i) if (now[i] != ref[i] && !(i >= skip0 && i < skip1)) out.push_back({arena, int(i), int(now[i])});
}
int main() {
    for (uintptr_t base : {0xC17000u, 0xB4E000u, 0xB6E000u, 0xB7C000u}) if (!VirtualAlloc((void*)base, 0x2000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) { std::printf("no se pudo reservar %%x (error %%lu)\n", unsigned(base), GetLastError()); return 3; }
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n); gC = &c;
        pThis = aThis.now.data(); pAnim = aAnim.now.data(); pIdle = aIdle.now.data(); pNew = aNew.now.data(); pPed = aPed.now.data(); pTgt = aTgt.now.data(); pPad = aPad.now.data(); pWinfo = aWinfo.now.data(); gBlocks = reinterpret_cast<CAnimBlock*>(aBlocks.now.data());
        auto P = [](const u8* p) { return u32(uintptr_t(p)); };
        FillWords(aThis.ref.data(), kThisBytes, Rand(n, 50)); FillWords(aAnim.ref.data(), kAnimBytes, Rand(n, 51)); FillWords(aIdle.ref.data(), kAnimBytes, Rand(n, 52)); FillWords(aNew.ref.data(), kAnimBytes, Rand(n, 53));
        FillWords(aHier.ref.data(), 0x20, Rand(n, 54)); FillWords(aPed.ref.data(), 0x730, Rand(n, 55)); FillWords(aMat.ref.data(), 0x48, Rand(n, 56)); FillWords(aTMat.ref.data(), 0x48, Rand(n, 57)); FillWords(aTgt.ref.data(), 0x18, Rand(n, 58));
        FillWords(aBlocks.ref.data(), 0x60, Rand(n, 60)); FillWords(aGroups.ref.data(), 0x50, Rand(n, 61)); FillWords(aWinfo.ref.data(), 0x70, Rand(n, 62)); FillWords(aPad.ref.data(), 0x10, Rand(n, 63));
        const u32 animPtr = c.hasAnim ? P(pAnim) : 0u, idlePtr = c.hasIdle ? P(pIdle) : 0u, newPtr = P(pNew), tgtPtr = c.hasTarget ? P(pTgt) : 0u;
        { auto& a = aThis.ref; a[8] = u8(c.finished); a[9] = u8(c.inControl); a[0xA] = u8(c.animsRef); Put32(a, 0xC, c.reqGroup); Put16(a, 0x10, c.idlePeriod); Put16(a, 0x12, c.idleCounter); Put32(a, 0x18, tgtPtr); Put32(a, 0x1C, animPtr); Put32(a, 0x20, idlePtr);
          a[0x24] = u8(c.combo); a[0x25] = u8(c.move); a[0x26] = u8(c.next); a[0x27] = u8(c.last); }
        { auto& a = aAnim.ref; Put32(a, kAnimHier, c.hasHier ? P(aHier.now.data()) : 0u); Put32(a, kAnimWeight, c.animW); Put32(a, kAnimDelta, c.animD); Put32(a, kAnimTime, c.animTime); Put32(a, kAnimStep, c.animStep); Put16(a, kAnimId, c.animId); Put16(a, kAnimFlags, c.animFlags);
          Put32(aIdle.ref, kAnimWeight, c.idleW); Put32(aIdle.ref, kAnimDelta, c.idleD); Put32(aHier.ref, kHierTotal, c.hierTotal); }
        { auto& a = aPed.ref; for (u32 i = 0; i < 3; ++i) { Put32(a, kPedPos + 4 * i, c.pedPos[i]); Put32(a, kPedSpeed + 4 * i, c.speed[i]); }
          Put32(a, kPedMatrix, c.hasMatrix ? P(aMat.now.data()) : 0u); Put32(a, kPedClump, c.clump); Put32(a, kPedGroup, c.group); Put32(a, kPedMoveState, c.moveState); Put32(a, kPedSwim, c.swim0); Put32(a, kPedAim, kAimInit);
          a[kPedSlot] = u8(c.slot); a[kPedStyle] = u8(c.style); Put32(a, kPedWeapons + kWeaponStride * c.slot, c.wtype);
          for (u32 i = 0; i < 3; ++i) { Put32(aMat.ref, 4 * i, c.mat[i]); Put32(aMat.ref, 0x10 + 4 * i, c.mat[3 + i]); Put32(aMat.ref, 0x20 + 4 * i, c.mat[6 + i]); Put32(aMat.ref, 0x30 + 4 * i, c.mat[9 + i]); }
          for (u32 i = 0; i < 3; ++i) Put32(aTgt.ref, 4 + 4 * i, c.tgtPos[i]); Put32(aTgt.ref, kPedMatrix, c.tgtHasMatrix ? P(aTMat.now.data()) : 0u); for (u32 i = 0; i < 3; ++i) Put32(aTMat.ref, 0x30 + 4 * i, c.tgtMatPos[i]); }
        u32 blockAddr[3]; for (u32 k = 0; k < 3; ++k) { blockAddr[k] = P(aBlocks.now.data() + 0x20 * k); aBlocks.ref[0x20 * k + 0x10] = u8(c.blockLoaded[k]); }
        for (u32 k = 0; k < 4; ++k) Put32(aGroups.ref, 0x14 * k, c.groupBlock[k] ? blockAddr[c.groupBlock[k] - 1] : 0u);
        aWinfo.ref[0x6E] = u8(c.baseCombo);
        std::array<u8, kRegion> table{}; FillWords(table.data(), kRegion, c.tableSeed); for (const auto& w : c.writes) std::memcpy(table.data() + (w.addr - kRegionBase), &w.value, 4);
        aThis.now = aThis.ref; aAnim.now = aAnim.ref; aIdle.now = aIdle.ref; aNew.now = aNew.ref; aHier.now = aHier.ref; aPed.now = aPed.ref; aMat.now = aMat.ref; aTMat.now = aTMat.ref; aTgt.now = aTgt.ref;
        aBlocks.now = aBlocks.ref; aGroups.now = aGroups.ref; aWinfo.now = aWinfo.ref; aPad.now = aPad.ref; nev = 0;
        std::memcpy(reinterpret_cast<void*>(kRegionBase), table.data(), kRegion); std::memcpy(reinterpret_cast<void*>(0xB7CB5C), &c.timestep, 4); *reinterpret_cast<u8*>(0xB6EC2E) = u8(c.camFlag);
        std::memcpy(reinterpret_cast<void*>(0xB6F32C), c.camFront.data(), 12); const u32 gp = P(aGroups.now.data()); std::memcpy(reinterpret_cast<void*>(0xB4EA34), &gp, 4);
        const bool ret = reinterpret_cast<CTaskSimpleFight*>(aThis.now.data())->ProcessPed(reinterpret_cast<CPed*>(aPed.now.data()));
        std::vector<std::array<int, 3>> ch;
        Diff(ch, 0, aThis.now, aThis.ref); Diff(ch, 1, aAnim.now, aAnim.ref); Diff(ch, 2, aIdle.now, aIdle.ref); Diff(ch, 3, aPed.now, aPed.ref, kPedAim, kPedAim + 4); Diff(ch, 4, aNew.now, aNew.ref);
        u32 aim; std::memcpy(&aim, aPed.now.data() + kPedAim, 4);
        const bool others = aHier.now == aHier.ref && aMat.now == aMat.ref && aTMat.now == aTMat.ref && aTgt.now == aTgt.ref && aBlocks.now == aBlocks.ref && aGroups.now == aGroups.ref && aWinfo.now == aWinfo.ref && aPad.now == aPad.ref;
        std::printf("{\"ret\":%%d,\"events\":[", ret ? 1 : 0);
        for (u32 i = 0; i < nev && i < 96; ++i) { std::printf("%%s[", i ? "," : ""); for (int k = 0; k < 9; ++k) std::printf("%%s%%u", k ? "," : "", evw[i][k]); std::printf("]"); }
        std::printf("],\"changes\":[");
        for (size_t i = 0; i < ch.size(); ++i) std::printf("%%s[%%d,%%d,%%d]", i ? "," : "", ch[i][0], ch[i][1], ch[i][2]);
        std::printf("],\"aim\":%%s", aim == kAimInit ? "null" : std::to_string(aim).c_str());
        std::printf(",\"others_unchanged\":%%s,\"globals_unchanged\":true", others ? "true" : "false");
        std::printf(",\"ptrs\":{\"anim_ptr\":%%u,\"idle_ptr\":%%u,\"newidle_ptr\":%%u,\"block_addr\":[%%u,%%u,%%u],\"group_block_addr\":[", animPtr, idlePtr, newPtr, blockAddr[0], blockAddr[1], blockAddr[2]);
        for (u32 k = 0; k < 4; ++k) std::printf("%%s%%u", k ? "," : "", c.groupBlock[k] ? blockAddr[c.groupBlock[k] - 1] : 0u);
        std::printf("],\"name_block_addr\":%%u}}\n", blockAddr[c.nameBlock - 1]);
    }
    return 0;
}
'''


def main():
    code = MOCK % dict(fix=(HERE).as_posix(), early=early, helpers=helpers, func=func)
    d = Path(tempfile.mkdtemp(prefix='spp-'))
    (d / 's.cpp').write_text(code, encoding='utf-8')
    bat = d / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O1 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s" /link /FIXED /BASE:0x20000000\r\n"%s"\r\n' % (VS / 'Auxiliary/Build/vcvars32.bat', d / 's.exe', d, d / 's.cpp', d / 's.exe'))
    r = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(d))
    got = [json.loads(line) for line in r.stdout.splitlines() if line.startswith('{')]
    if len(got) != oracle.CASES:
        print(r.stdout[-6000:], r.stderr[-2000:])
        raise SystemExit('el autotest no produjo %d casos (compilacion o ejecucion fallida; fuente en %s)' % (oracle.CASES, d))
    import fixture_mirror as fm
    rows = fm.build_and_run()
    rows.pop()
    bad = 0
    sys.path.insert(0, str(HERE))
    import run_local
    for n, (row, g) in enumerate(zip(rows, got)):
        row = dict(row)
        row.update(g.pop('ptrs'))
        want, _ = oracle.expected(oracle.NAME, row, n)
        w = dict(want)
        # los cambios del oraculo son [arena, desplazamiento, byte]; el autotest da lo mismo
        g2 = dict(g)
        waim, gaim = w.pop('aim'), g2.pop('aim')
        if g2 != w or not run_local.aim_close(gaim, waim):
            bad += 1
            if bad <= 6:
                print('DIFERENCIA caso', n, 'familia', row['family'])
                for k in w:
                    if g2.get(k) != w[k]:
                        print('  ', k, '\n    fuente  ', json.dumps(g2.get(k))[:600], '\n    oraculo ', json.dumps(w[k])[:600])
                if not run_local.aim_close(gaim, waim):
                    print('   aim fuente', gaim, 'oraculo', waim)
    print('ok' if bad == 0 else '%d casos distintos de %d' % (bad, len(rows)))


if __name__ == '__main__':
    main()
