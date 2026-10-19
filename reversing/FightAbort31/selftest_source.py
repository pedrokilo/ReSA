"""Autoprueba del FUENTE (sin el juego ni el EXE): compila el cuerpo real de CTaskSimpleFight::MakeAbortable de TaskSimpleFight.cpp contra maquetas minimas de los tipos del juego
(mismos desplazamientos, comprobados con static_assert; CEvent con el orden de la vtable del juego: ~CEvent, GetEventType, GetEventPriority, GetLifeTime, Clone) y falsos que registran los
mismos eventos que el banco, ejecuta las 4608 fixtures de Fixtures.hpp y compara cada resultado con fixture_oracle.py. Es independiente del EXE: solo comprueba que la transcripcion C++ y el
oraculo (ambos escritos desde el desensamblado) coinciden (p. ej. detecta una llamada a la ranura 1 en vez de la 2 de la vtable). No sustituye al banco en juego.
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


# la funcion, tal como esta en el fuente: ayudantes de la funcion (devolucion de llamada, constante de desvanecimiento, 0x61C9B0) y el cuerpo
helpers = 'using FightAnimCallback = void (*)(CAnimBlendAssociation*, void*);\nconst auto FightDefaultAnimCB = reinterpret_cast<FightAnimCallback>(0x4D1390);\n'
helpers += 'constexpr float FightBlendOutAbort = std::bit_cast<float>(0xC47A0000u);\n'
helpers += cut('// 0x61C9B0 (ret 4)', '// 0x624710 (ret 4)')
func = cut('// 0x6239F0: con prioridad', '#pragma float_control(push)')

MOCK = r'''
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <bit>
#include <array>
#include <string>
#include <vector>
#include "%(fix)s/Fixtures.hpp"
using namespace FightAbort31Fix;
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using int32 = std::int32_t; using int16 = std::int16_t; using int8 = std::int8_t;
#define ZoneScoped
enum AssocGroupId : int32 {}; enum AnimationId : int32 {};
enum eAbortPriority { ABORT_PRIORITY_LEISURE = 0, ABORT_PRIORITY_URGENT, ABORT_PRIORITY_IMMEDIATE };
struct RpClump {};
struct CVector2D { float x, y; };
struct CPlayerPedData { char pad[0xC]; CVector2D m_vecFightMovement; char pad2[0xC]; };
static_assert(offsetof(CPlayerPedData, m_vecFightMovement) == 0xC && sizeof(CPlayerPedData) == 0x20, "CPlayerPedData");
struct CAnimBlendAssociation {
    char pad0[0x18]; float m_BlendAmount, m_BlendDelta; char pad1[0x1C];
    void SetDeleteCallback(void (*cb)(CAnimBlendAssociation*, void*), void* data = nullptr);
};
static_assert(sizeof(CAnimBlendAssociation) == 0x3C && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C && offsetof(CAnimBlendAssociation, m_BlendAmount) == 0x18, "CAnimBlendAssociation");
struct CPed {
    void* vt; char pad0[0x14]; void* m_pRwObject; char pad1[0x480 - 0x1C]; CPlayerPedData* m_pPlayerData; char pad2[0x4D4 - 0x484]; AssocGroupId m_nAnimGroup; char pad3[0x4D8 - 0x4D8];
    bool IsPlayer() const; RpClump* GetRwObject() const { return reinterpret_cast<RpClump*>(m_pRwObject); } CPlayerPedData* GetPlayerData() const { return m_pPlayerData; }
};
static_assert(sizeof(CPed) == 0x4D8 && offsetof(CPed, m_pRwObject) == 0x18 && offsetof(CPed, m_pPlayerData) == 0x480 && offsetof(CPed, m_nAnimGroup) == 0x4D4, "CPed");
struct CEvent { virtual ~CEvent() {} virtual int32 GetEventType() const = 0; virtual int32 GetEventPriority() const = 0; virtual int32 GetLifeTime() = 0; virtual CEvent* Clone() const = 0; };
struct CTaskSimpleFight {
    void* vt; void* parent; bool m_bIsFinished; char pad0[0x1C - 9]; CAnimBlendAssociation* m_pAnim; CAnimBlendAssociation* m_pIdleAnim; char pad1[0x26 - 0x24]; uint8 m_nNextCommand; uint8 pad2;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr);
};
static_assert(sizeof(CTaskSimpleFight) == 0x28 && offsetof(CTaskSimpleFight, m_pAnim) == 0x1C && offsetof(CTaskSimpleFight, m_pIdleAnim) == 0x20 && offsetof(CTaskSimpleFight, m_nNextCommand) == 0x26, "CTaskSimpleFight");
struct CAnimManager { static CAnimBlendAssociation* BlendAnimation(RpClump*, AssocGroupId, AnimationId, float); };

static const C* gC;
static u8 *pThis, *pAnim, *pIdle, *pPed, *pPd, *pEv;
static u32 nev; static u32 evw[32][6];
static void Push(u32 k, u32 a = 0, u32 b = 0, u32 c = 0, u32 d = 0, u32 e = 0) { if (nev < 32) { u32 w[6] = {k, a, b, c, d, e}; std::memcpy(evw[nev], w, 24); } ++nev; }
static u32 Label(const void* p) { if (!p) return 0; const u8* q = static_cast<const u8*>(p); return q == pThis ? 1 : q == pAnim ? 2 : q == pIdle ? 3 : q == pPed ? 4 : q == pEv ? 5 : q == pPd ? 6 : 0xBAD; }
static u32 BitsOf(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
void CAnimBlendAssociation::SetDeleteCallback(void (*cb)(CAnimBlendAssociation*, void*), void* data) { Push(2, Label(this), u32(uintptr_t(cb)), u32(uintptr_t(data))); }
bool CPed::IsPlayer() const { Push(4, Label(this)); return gC->pedKind == 2; }
CAnimBlendAssociation* CAnimManager::BlendAnimation(RpClump* c, AssocGroupId g, AnimationId a, float d) { Push(3, u32(uintptr_t(c)), u32(g), u32(a), BitsOf(d)); return nullptr; }
namespace plugin { template<uintptr_t A, class... T> void CallMethod(T... a); template<> inline void CallMethod<0x61C9B0, CTaskSimpleFight*, CPed*>(CTaskSimpleFight* t, CPed* p) { Push(5, Label(t), Label(p)); } }
struct MockEvent : CEvent {
    int32 GetEventType() const override { Push(1, 1, Label(this)); return int32(gC->ret); }
    int32 GetEventPriority() const override { Push(1, 2, Label(this)); return int32(gC->evType); }
    int32 GetLifeTime() override { Push(1, 3, Label(this)); return 0; }
    CEvent* Clone() const override { Push(1, 4, Label(this)); return nullptr; }
};
%(helpers)s
%(func)s
template<size_t N> struct Ar { alignas(16) std::array<u8, N> now{}, ref{}; };
static Ar<kThisBytes> aThis; static Ar<kAnimBytes> aAnim, aIdle; static Ar<kPedBytes> aPed; static Ar<kPdBytes> aPd;
template<size_t N> static void Put32(std::array<u8, N>& a, u32 at, u32 v) { std::memcpy(a.data() + at, &v, 4); }
template<size_t N> static void Diff(std::vector<std::array<int, 3>>& out, int arena, const std::array<u8, N>& now, const std::array<u8, N>& ref) {
    for (u32 i = 0; i < N; ++i) if (now[i] != ref[i]) out.push_back({arena, int(i), int(now[i])});
}
int main() {
    static MockEvent ev;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n); gC = &c;
        pThis = aThis.now.data(); pAnim = aAnim.now.data(); pIdle = aIdle.now.data(); pPed = c.pedKind ? aPed.now.data() : nullptr; pPd = aPd.now.data(); pEv = c.hasEvent ? reinterpret_cast<u8*>(&ev) : nullptr;
        auto P = [](const u8* p) { return u32(uintptr_t(p)); };
        FillWords(aThis.ref.data(), kThisBytes, Rand(n, 50)); FillWords(aAnim.ref.data(), kAnimBytes, Rand(n, 51)); FillWords(aIdle.ref.data(), kAnimBytes, Rand(n, 52));
        FillWords(aPed.ref.data(), kPedBytes, Rand(n, 53)); FillWords(aPd.ref.data(), kPdBytes, Rand(n, 54));
        const u32 animPtr = c.hasAnim ? P(aAnim.now.data()) : 0u, idlePtr = c.hasIdle ? P(aIdle.now.data()) : 0u, pdPtr = P(aPd.now.data());
        Put32(aThis.ref, kAnimAt, animPtr); Put32(aThis.ref, kIdleAt, idlePtr); aThis.ref[kFinishedAt] = u8(c.finished0); aThis.ref[kNextAt] = u8(c.next0);
        Put32(aAnim.ref, kAnimDeltaAt, c.animDelta); Put32(aIdle.ref, kAnimWeightAt, c.idleWeight); Put32(aIdle.ref, kAnimDeltaAt, c.idleDelta);
        const u32 pedClump = Rand(n, 60), pedGroup = Rand(n, 61);
        Put32(aPed.ref, kPedClumpAt, pedClump); Put32(aPed.ref, kPedGroupAt, pedGroup); if (c.pedKind == 2) Put32(aPed.ref, kPedPdAt, pdPtr);
        Put32(aPd.ref, kPdFightAt, c.pdX); Put32(aPd.ref, kPdFightAt + 4, c.pdY);
        aThis.now = aThis.ref; aAnim.now = aAnim.ref; aIdle.now = aIdle.ref; aPed.now = aPed.ref; aPd.now = aPd.ref; nev = 0;
        const bool ret = reinterpret_cast<CTaskSimpleFight*>(aThis.now.data())->MakeAbortable(c.pedKind ? reinterpret_cast<CPed*>(aPed.now.data()) : nullptr, eAbortPriority(c.prio), c.hasEvent ? &ev : nullptr);
        std::vector<std::array<int, 3>> ch;
        Diff(ch, 0, aThis.now, aThis.ref); Diff(ch, 1, aAnim.now, aAnim.ref); Diff(ch, 2, aIdle.now, aIdle.ref); Diff(ch, 3, aPed.now, aPed.ref); Diff(ch, 4, aPd.now, aPd.ref);
        std::printf("{\"ret\":%%d,\"events\":[", ret ? 1 : 0);
        for (u32 i = 0; i < nev && i < 32; ++i) { std::printf("%%s[", i ? "," : ""); for (int k = 0; k < 6; ++k) std::printf("%%s%%u", k ? "," : "", evw[i][k]); std::printf("]"); }
        std::printf("],\"changes\":[");
        for (size_t i = 0; i < ch.size(); ++i) std::printf("%%s[%%d,%%d,%%d]", i ? "," : "", ch[i][0], ch[i][1], ch[i][2]);
        std::printf("],\"ptrs\":{\"anim_ptr\":%%u,\"idle_ptr\":%%u,\"ped_clump\":%%u,\"ped_group\":%%u}}\n", animPtr, idlePtr, pedClump, pedGroup);
    }
    return 0;
}
'''


def main():
    code = MOCK % dict(fix=HERE.as_posix(), helpers=helpers, func=func)
    d = Path(tempfile.mkdtemp(prefix='sab-'))
    (d / 's.cpp').write_text(code, encoding='utf-8')
    bat = d / 'b.bat'
    bat.write_text('@echo off\r\ncall "%s" >nul\r\ncl /nologo /EHsc /O1 /std:c++20 /Fe:"%s" /Fo:"%s\\\\" "%s"\r\n"%s"\r\n' % (VS / 'Auxiliary/Build/vcvars32.bat', d / 's.exe', d, d / 's.cpp', d / 's.exe'))
    r = subprocess.run(['cmd', '/c', str(bat)], capture_output=True, text=True, cwd=str(d))
    got = [json.loads(line) for line in r.stdout.splitlines() if line.startswith('{')]
    if len(got) != oracle.CASES:
        print(r.stdout[-6000:], r.stderr[-2000:])
        raise SystemExit('el autotest no produjo %d casos (compilacion o ejecucion fallida; fuente en %s)' % (oracle.CASES, d))
    import fixture_mirror as fm
    rows = fm.build_and_run()
    rows.pop()
    bad = 0
    for n, (row, g) in enumerate(zip(rows, got)):
        row = dict(row)
        ptrs = g.pop('ptrs')
        row.update(ptrs)
        want, _ = oracle.expected(oracle.NAME, row, n)
        w = {k: want[k] for k in ('ret', 'events', 'changes')}
        if g != w:
            bad += 1
            if bad <= 6:
                print('DIFERENCIA caso', n, 'familia', row['family'])
                for k in w:
                    if g.get(k) != w[k]:
                        print('  ', k, '\n    fuente  ', json.dumps(g.get(k))[:500], '\n    oraculo ', json.dumps(w[k])[:500])
    print('ok' if bad == 0 else '%d casos distintos de %d' % (bad, len(rows)))


if __name__ == '__main__':
    main()
