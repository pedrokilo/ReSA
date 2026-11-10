"""Prueba negativa del banco Fn155A (lote 155A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (17 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (FindColSlot), a que el banco ya este corriendo (F155ALive::gBenchThread != 0), para que la partida
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
E = G + 'Events/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'

AUDIO = G + 'Audio/Entities/AEGlobalWeaponAudioEntity.cpp'
GUNCTRL, USEGUN = T + 'TaskSimpleGunControl.cpp', T + 'TaskSimpleUseGun.cpp'
DJPG, LEC = E + 'EventDontJoinPlayerGroup.cpp', E + 'EventLeaderExitedCarAsDriver.cpp'
SETPED, COMPLEXCAR, SIMPLECAR = T + 'TaskSimpleCarSetPedSlowDraggedOut.cpp', T + 'TaskComplexCarSlowBeDraggedOut.cpp', T + 'TaskSimpleCarSlowBeDraggedOut.cpp'
# destructores con borrado: (archivo, llamada al cuerpo, operacion de liberacion)
SDD = {
    1: (GUNCTRL, 'CTaskSimpleGunControlDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
    6: (DJPG, 'DontJoinPlayerGroupDestructorBody(self, nullptr);', 'CEvent::operator delete(self);'),
    8: (SETPED, 'CTaskSimpleCarSetPedSlowDraggedOutDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
    9: (COMPLEXCAR, 'CTaskComplexCarSlowBeDraggedOutDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
    16: (LEC, 'LeaderExitedCarAsDriverDestructorBody(self, nullptr);', 'CEvent::operator delete(self);'),
}
SOURCES = sorted({AUDIO, GUNCTRL, USEGUN, DJPG, LEC, SETPED, COMPLEXCAR, SIMPLECAR})


def sdd_flags3(k):
    f, call, _ = SDD[k]
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def sdd_twice(k):
    f, call, _ = SDD[k]
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def sdd_null(k):
    f, _, dele = SDD[k]
    return (f, '        ' + dele + NL + '    }' + NL + '    return self;', '        ' + dele + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def ret_event(cls, name, ret, mut):
    return (cls, name + NL + '    return ' + ret + ';', name + NL + '    return ' + mut + ';')


def round1():
    out = [None] * 17
    out[0] = (AUDIO, '        weaponType = WEAPON_AK47;', '        weaponType = ' + UNALT + ' ? WEAPON_PISTOL : WEAPON_AK47;')
    out[1] = sdd_flags3(1)
    out[2] = (USEGUN, '    *reinterpret_cast<uint32*>(bytes) = 0x86D724;', '    *reinterpret_cast<uint32*>(bytes) = (reinterpret_cast<uintptr_t>(bytes) & 3) ? 0x86D728 : 0x86D724;')
    out[3] = ret_event(DJPG, 'int32 CEventDontJoinPlayerGroup::GetLifeTime() {', '0', UNALT + ' ? 1 : 0')
    out[4] = ret_event(DJPG, 'bool CEventDontJoinPlayerGroup::AffectsPed(CPed* /*ped*/) {', 'true', '!' + UNALT)
    out[5] = ret_event(DJPG, 'eEventType CEventDontJoinPlayerGroup::GetEventType() const {', 'EVENT_DONT_JOIN_GROUP', UNALT + ' ? static_cast<eEventType>(EVENT_DONT_JOIN_GROUP + 1) : EVENT_DONT_JOIN_GROUP')
    out[6] = sdd_flags3(6)
    out[7] = (DJPG, '    return object ? object->Constructor(m_player) : nullptr;', '    return object ? object->Constructor(' + UNALT + ' ? nullptr : m_player) : nullptr;')
    out[8] = sdd_flags3(8)
    out[9] = sdd_flags3(9)
    out[10] = (SIMPLECAR, '        *animId = ANIM_ID_CAR_JACKEDRHS;', '        *animId = ' + UNALT + ' ? ANIM_ID_CAR_JACKEDLHS : ANIM_ID_CAR_JACKEDRHS;')
    out[11] = ret_event(LEC, 'int32 CEventLeaderExitedCarAsDriver::GetLifeTime() {', '0', UNALT + ' ? 1 : 0')
    out[12] = ret_event(LEC, 'bool CEventLeaderExitedCarAsDriver::AffectsPed(CPed* /*ped*/) {', 'false', UNALT + ' != 0')
    out[13] = ret_event(LEC, 'eEventType CEventLeaderExitedCarAsDriver::GetEventType() const {', 'EVENT_LEADER_EXITED_CAR_AS_DRIVER', UNALT + ' ? static_cast<eEventType>(EVENT_LEADER_EXITED_CAR_AS_DRIVER + 1) : EVENT_LEADER_EXITED_CAR_AS_DRIVER')
    out[14] = ret_event(LEC, 'int32 CEventLeaderExitedCarAsDriver::GetEventPriority() const {', '47', UNALT + ' ? 48 : 47')
    out[15] = (LEC, '    *reinterpret_cast<uint32*>(self) = 0x85B898;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85B89C : 0x85B898;')
    out[16] = sdd_flags3(16)
    return out


def round2():
    out = [None] * 17
    out[0] = (AUDIO, 'GetRandomNumberInRange(90, 350);', 'GetRandomNumberInRange(' + UNALT1 + ' ? 91 : 90, 350);')
    out[1] = sdd_twice(1)
    out[2] = (USEGUN, '    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x1C));', '    if (!(reinterpret_cast<uintptr_t>(bytes) & 1)) {' + NL + '        CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x1C));' + NL + '    }')
    out[3] = ret_event(DJPG, 'int32 CEventDontJoinPlayerGroup::GetLifeTime() {', '0', UNALT1 + ' ? -1 : 0')
    out[4] = ret_event(DJPG, 'bool CEventDontJoinPlayerGroup::AffectsPed(CPed* /*ped*/) {', 'true', '!' + UNALT1)
    out[5] = ret_event(DJPG, 'eEventType CEventDontJoinPlayerGroup::GetEventType() const {', 'EVENT_DONT_JOIN_GROUP', 'static_cast<eEventType>(EVENT_DONT_JOIN_GROUP ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[6] = sdd_twice(6)
    out[7] = (DJPG, 'CEvent::operator new(sizeof(CEventDontJoinPlayerGroup))', 'CEvent::operator new(sizeof(CEventDontJoinPlayerGroup) + (' + UNALT1 + ' ? 4 : 0))')
    out[8] = sdd_null(8)
    out[9] = sdd_twice(9)
    out[10] = (SIMPLECAR, '    *outGroup = m_Vehicle->GetAnimGroup().GetGroup(id);', '    *outGroup = m_Vehicle->GetAnimGroup().GetGroup(static_cast<AnimationId>(id + ' + UNALT1 + '));')
    out[11] = ret_event(LEC, 'int32 CEventLeaderExitedCarAsDriver::GetLifeTime() {', '0', UNALT1 + ' ? -1 : 0')
    out[12] = ret_event(LEC, 'bool CEventLeaderExitedCarAsDriver::AffectsPed(CPed* /*ped*/) {', 'false', UNALT1 + ' != 0')
    out[13] = ret_event(LEC, 'eEventType CEventLeaderExitedCarAsDriver::GetEventType() const {', 'EVENT_LEADER_EXITED_CAR_AS_DRIVER', 'static_cast<eEventType>(EVENT_LEADER_EXITED_CAR_AS_DRIVER ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[14] = ret_event(LEC, 'int32 CEventLeaderExitedCarAsDriver::GetEventPriority() const {', '47', UNALT1 + ' ? 46 : 47')
    out[15] = (LEC, '    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CEventEditableBody::EditableResponseDestructorBody(self, nullptr);' + NL + '    }' + NL + '}')
    out[16] = sdd_null(16)
    return out


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
