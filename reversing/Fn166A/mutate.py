"""Prueba negativa del banco Fn166A (lote 166A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (24 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` u objeto desalineado
(el juego no los produce) o, en GetGTA3ImgSize (sin `this`), a tamanos de archivo que el juego real no devuelve (0xFFFFFFFF y 0x80000000, que el GetFileSize falso del banco si devuelve), para que la partida
no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
NL = chr(10)


def U3(v):
    return '(reinterpret_cast<uintptr_t>(%s) & 3)' % v


def U1(v):
    return '(reinterpret_cast<uintptr_t>(%s) & 1)' % v


FIGHT = T + 'TaskSimpleFight.cpp'
FCTL = T + 'TaskSimpleFightingControl.cpp'
IKC = T + 'TaskSimpleIKChain.cpp'
FOLLOW = T + 'TaskComplexFollowPedFootsteps.cpp'
XYO = T + 'SeekEntity/TaskComplexSeekEntityXYOffset.cpp'
ANYM = T + 'SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp'
SEEKH = T + 'SeekEntity/TaskComplexSeekEntity.h'
D3D = G + 'RenderWare/D3DResourceSystem.cpp'
TXD = G + 'TxdStore_Plugin.cpp'
FXP = G + 'Fx/FxPrimBP.cpp'
EHC = G + 'Events/EventHitByWaterCannon.cpp'
EIW = G + 'Events/EventInWater.cpp'
RNA = T + 'TaskSimpleRunNamedAnim.cpp'
RTA = T + 'TaskSimpleRunTimedAnim.cpp'
MAT = G + 'Core/Matrix.cpp'
THS = G + 'Scripts/TheScripts.cpp'
SCD = T + 'TaskSimpleSetCharDecisionMaker.cpp'
SOURCES = sorted({FIGHT, FCTL, IKC, FOLLOW, XYO, ANYM, SEEKH, D3D, TXD, FXP, EHC, EIW, RNA, RTA, MAT, THS, SCD})
T_ = U3('this')
T1 = U1('this')
S_ = U3('self')
S1 = U1('self')
DEL1 = ('    if (flags & 1) {' + NL + '        CTask::operator delete(this);', '    if ((flags & 1) && !' + T_ + ') {' + NL + '        CTask::operator delete(this);')
DEL2 = ('        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + T_ + ' ? nullptr : this;')


def round1():
    o = [None] * 24
    o[0] = (FIGHT, '    if (!memory) {' + NL + '        return nullptr;', '    if (!memory || ' + T_ + ') {' + NL + '        return nullptr;')
    o[1] = (FIGHT,) + DEL1
    o[2] = (FCTL,) + DEL1
    o[3] = (IKC, '    *reinterpret_cast<uint32*>(self) = 0x86DB48;', '    *reinterpret_cast<uint32*>(self) = ' + S_ + ' ? 0x86DB4C : 0x86DB48;')
    o[4] = (FOLLOW, 'eTaskType CTaskComplexFollowPedFootsteps::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskComplexFollowPedFootsteps::GetTaskType() const {' + NL + '    return ' + T_ + ' ? TASK_FINISHED : Type;')
    o[5] = (XYO, '    *reinterpret_cast<uint32*>(base + 0x2C) = 0;', '    *reinterpret_cast<uint32*>(base + 0x2C) = ' + S_ + ' ? 1 : 0;')
    o[6] = (ANYM, '    *reinterpret_cast<uint32*>(raw + 0x24)   = 0x86F8F8;', '    *reinterpret_cast<uint32*>(raw + 0x24)   = ' + S_ + ' ? 0x86F8FC : 0x86F8F8;')
    o[7] = (D3D, '    *count = *count + 1;', '    *count = *count + (' + S_ + ' ? 2 : 1);')
    o[8] = (D3D, '    if (void* const memory = exeOperatorNew(0x10)) {', '    if (void* const memory = exeOperatorNew(0x10); memory && !(reinterpret_cast<uintptr_t>(memory) & 1)) {')
    o[9] = (TXD, '    return ms_txdPluginOffset != -1;', '    return ms_txdPluginOffset != -1 && ms_txdPluginOffset != 0x7FFFFFFF;')
    o[10] = (FXP, '    *reinterpret_cast<uint32*>(raw + 0x00) = 0x85A9C4;' + NL + '    *reinterpret_cast<uint32*>(raw + 0x20) = 0;', '    *reinterpret_cast<uint32*>(raw + 0x00) = ' + U3('raw') + ' ? 0x85A9C8 : 0x85A9C4;' + NL + '    *reinterpret_cast<uint32*>(raw + 0x20) = 0;')
    o[11] = (FXP, '    for (const uint32 offset : { 0xCu, 0x10u, 0x14u, 0x14u }) {', '    for (const uint32 offset : { 0xCu, 0x10u, 0x14u, ' + U3('raw') + ' ? 0x18u : 0x14u }) {')
    o[12] = (EHC, '    return EVENT_WATER_CANNON;', '    return ' + T_ + ' ? EVENT_DAMAGE : EVENT_WATER_CANNON;')
    o[13] = (EHC, 'int32 CEventHitByWaterCannon::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventHitByWaterCannon::GetLifeTime() {' + NL + '    return ' + T_ + ' ? 1 : 0;')
    o[14] = (EHC, '    return 55.0f;', '    return ' + T_ + ' ? 56.0f : 55.0f;')
    o[15] = (EHC, 'int32 CEventHitByWaterCannon::GetEventPriority() const {' + NL + '    return 64;', 'int32 CEventHitByWaterCannon::GetEventPriority() const {' + NL + '    return ' + T_ + ' ? 65 : 64;')
    o[16] = (EIW, '    return ped->IsAlive();', '    return ped->IsAlive() && !' + T_ + ';')
    o[17] = (EIW, '    return GetEventPriority() >= otherPriority;', '    return ' + T_ + ' ? GetEventPriority() > otherPriority : GetEventPriority() >= otherPriority;')
    o[18] = (RNA, '    m_nAnimId = static_cast<int16>(m_pAnim->GetAnimId());', '    m_nAnimId = static_cast<int16>(m_pAnim->GetAnimId() + (' + T_ + ' ? 1 : 0));')
    o[19] = (RTA,) + DEL1
    o[20] = (SEEKH, '        if (flags & 1) {' + NL + '            CTask::operator delete(self);', '        if ((flags & 1) && !' + S_ + ') {' + NL + '            CTask::operator delete(self);')
    o[21] = (MAT, '    FloatSumStoreCpp(offsetZ, &pos.z, &pos.z);', '    FloatSumStoreCpp(' + S_ + ' ? offsetZ + 1.0f : offsetZ, &pos.z, &pos.z);')
    o[22] = (THS, '    *count = *count + 1;' + NL + '    return true;', '    *count = *count + (' + S_ + ' ? 2 : 1);' + NL + '    return true;')
    o[23] = (SCD,) + DEL1
    return o


def round2():
    o = [None] * 24
    o[0] = (FIGHT, 'static_cast<int32>(static_cast<int8>(m_nLastCommand))', '(' + T_ + ' ? static_cast<int32>(m_nLastCommand) : static_cast<int32>(static_cast<int8>(m_nLastCommand)))')
    o[1] = (FIGHT,) + DEL2
    o[2] = (FCTL,) + DEL2
    o[3] = (IKC, '    CTaskSimpleBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86DB48;', '    if (!' + S1 + ') {' + NL + '        CTaskSimpleBody::Constructor(self, nullptr);' + NL + '    }' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86DB48;')
    o[4] = (FOLLOW, 'eTaskType CTaskComplexFollowPedFootsteps::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskComplexFollowPedFootsteps::GetTaskType() const {' + NL + '    return static_cast<eTaskType>(Type + (' + T1 + ' ? 1 : 0));')
    o[5] = (XYO, 'base[0x30]                              = 1;', 'base[0x30]                              = ' + S1 + ' ? 2 : 1;')
    o[6] = (ANYM, '    *(raw + 0x21)                            = 0;', '    *(raw + 0x21)                            = ' + S1 + ' ? 1 : 0;')
    o[7] = (D3D, '    const uint32 data = *reinterpret_cast<volatile uint32*>(raw + 8);', '    const uint32 data = *reinterpret_cast<volatile uint32*>(raw + 8) + (' + S1 + ' ? 4u : 0u);')
    o[8] = (D3D, '        static_cast<CDirectory*>(memory)->Constructor(0x1388);', '        static_cast<CDirectory*>(memory)->Constructor(0x1388 + (reinterpret_cast<uintptr_t>(memory) & 1));')
    o[9] = (TXD, '    return ms_txdPluginOffset != -1;', '    return ms_txdPluginOffset != -1 && ms_txdPluginOffset != static_cast<int32>(0x80000000);')
    o[10] = (FXP, '    *reinterpret_cast<uint32*>(raw + 0x18) = 0;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(raw + 0x18) = ' + U1('raw') + ' ? 1 : 0;' + NL + '    return self;')
    o[11] = (FXP, '    *reinterpret_cast<uint32*>(raw + 0x00) = 0x85A9C4;' + NL + '    for (const uint32 offset', '    *reinterpret_cast<uint32*>(raw + 0x00) = ' + U1('raw') + ' ? 0x85A9C5 : 0x85A9C4;' + NL + '    for (const uint32 offset')
    o[12] = (EHC, '    return EVENT_WATER_CANNON;', '    return static_cast<eEventType>(EVENT_WATER_CANNON + (' + T1 + ' ? 1 : 0));')
    o[13] = (EHC, 'int32 CEventHitByWaterCannon::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventHitByWaterCannon::GetLifeTime() {' + NL + '    return ' + T1 + ' ? -1 : 0;')
    o[14] = (EHC, '    return 55.0f;', '    return ' + T1 + ' ? 54.0f : 55.0f;')
    o[15] = (EHC, 'int32 CEventHitByWaterCannon::GetEventPriority() const {' + NL + '    return 64;', 'int32 CEventHitByWaterCannon::GetEventPriority() const {' + NL + '    return ' + T1 + ' ? 63 : 64;')
    o[16] = (EIW, '    if (!ped->IsPlayer() && task && task->GetTaskType() == TASK_COMPLEX_IN_WATER)', '    if (!ped->IsPlayer() && task && task->GetTaskType() == TASK_COMPLEX_IN_WATER && !' + T1 + ')')
    o[17] = (EIW, '    if (refEvent.GetEventType() == EVENT_DAMAGE) {', '    if (refEvent.GetEventType() == EVENT_DAMAGE && !' + T1 + ') {')
    o[18] = (RNA, '    if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE)) {' + NL + '        m_animFlags |= ANIMATION_IS_BLEND_AUTO_REMOVE;', '    if (!(m_animFlags & ANIMATION_IS_FINISH_AUTO_REMOVE) && !' + T1 + ') {' + NL + '        m_animFlags |= ANIMATION_IS_BLEND_AUTO_REMOVE;')
    o[19] = (RTA,) + DEL2
    o[20] = (SEEKH, '        return self;' + NL + '    }' + NL + NL + 'protected: // Wrappers for hooks', '        return ' + S1 + ' ? nullptr : self;' + NL + '    }' + NL + NL + 'protected: // Wrappers for hooks')
    o[21] = (MAT, '    FloatSumStoreCpp(offsetY, &pos.y, &pos.y);', '    FloatSumStoreCpp(' + S1 + ' ? 0.0f : offsetY, &pos.y, &pos.y);')
    o[22] = (THS, '    destination[2] = position[2];', '    destination[2] = position[2] ^ (' + S1 + ' ? 1u : 0u);')
    o[23] = (SCD,) + DEL2
    return o


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    # cada texto original aparece exactamente una vez en el texto ya mutado por las anteriores mutaciones de su ronda (en cadena)
    for n in ROUNDS:
        cache = {}
        for rel, old, new in ROUNDS[n]:
            text = cache.get(rel)
            if text is None:
                text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            if text.count(old) != 1:
                raise SystemExit('ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:80], text.count(old)))
            cache[rel] = text.replace(old, new)
    print('check ok: dos rondas,', len(ROUNDS[1]), 'mutaciones cada una')


def apply(n):
    GOOD.mkdir(parents=True, exist_ok=True)
    state = GOOD / 'state.json'
    if state.exists():
        raise SystemExit('Hay una ronda aplicada: restaurar antes')
    saved = {}
    for rel in SOURCES:
        dst = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(ROOT / rel, dst)
        saved[rel] = sha(dst)
    state.write_text(json.dumps(saved, indent=2))
    for rel, old, new in ROUNDS[n]:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:80], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def restore():
    state = GOOD / 'state.json'
    saved = json.loads(state.read_text())
    for rel, digest in saved.items():
        src = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(src, ROOT / rel)
        if sha(ROOT / rel) != digest:
            raise SystemExit('Fuente restaurada distinta: ' + rel)
    state.unlink()
    print('restauradas', len(saved), 'fuentes (SHA256 igual a las copias buenas)')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    elif sys.argv[1] == 'check':
        check()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
