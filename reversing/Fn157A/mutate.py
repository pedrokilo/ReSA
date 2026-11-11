"""Prueba negativa del banco Fn157A (lote 157A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (24 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1) o a `this` u objeto desalineado, para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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

# destructores con borrado de tareas: (indice de funcion, clase, archivo sin extension)
TASK_SDD = [
    (0, 'CTaskComplexFleeEntity', 'TaskComplexFleeEntity'), (1, 'CTaskComplexSmartFleePoint', 'TaskComplexSmartFleePoint'), (2, 'CTaskComplexSmartFleeEntity', 'TaskComplexSmartFleeEntity'),
    (3, 'CTaskComplexFleeShooting', 'TaskComplexFleeShooting'), (4, 'CTaskComplexFleeAnyMeans', 'TaskComplexFleeAnyMeans'), (5, 'CTaskComplexObserveTrafficLights', 'TaskComplexObserveTrafficLights'),
    (6, 'CTaskComplexObserveTrafficLightsAndAchieveHeading', 'TaskComplexObserveTrafficLightsAndAchieveHeading'),
    (7, 'CTaskComplexCrossRoadLookAndAchieveHeading', 'TaskComplexCrossRoadLookAndAchieveHeading'), (8, 'CTaskSimpleTurn180', 'TaskSimpleTurn180'), (9, 'CTaskComplexHitResponse', 'TaskComplexHitResponse'),
    (10, 'CTaskSimpleIKLookAt', 'TaskSimpleIKLookAt'), (11, 'CTaskSimpleIKPointArm', 'TaskSimpleIKPointArm'), (12, 'CTaskSimpleTriggerLookAt', 'TaskSimpleTriggerLookAt'),
    (13, 'CTaskSimpleClearLookAt', 'TaskSimpleClearLookAt'), (14, 'CTaskComplexUseClosestFreeScriptedAttractor', 'TaskComplexUseClosestFreeScriptedAttractor'), (22, 'CTaskSimpleJetPack', 'TaskSimpleJetPack')]
ATT = E + 'EventPlayerCommandToGroupAttack.cpp'
FOLLOW = G + 'Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.cpp'
CLIMB = T + 'TaskSimpleClimb.cpp'
# destructores con borrado: (archivo, llamada al cuerpo, operacion de liberacion)
SDD = {idx: (T + fn + '.cpp', cls + 'DestructorBody(self, nullptr);', 'CTask::operator delete(self);') for idx, cls, fn in TASK_SDD}
SDD[19] = (ATT, 'PlayerCommandToGroupAttackDestructorBody(self, nullptr);', 'CEvent::operator delete(self);')
SOURCES = sorted({v[0] for v in SDD.values()} | {FOLLOW, CLIMB, ATT})


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
    out = [None] * 24
    for idx in SDD:
        out[idx] = sdd_flags3(idx)
    out[15] = ret_event(ATT, 'eEventType CEventPlayerCommandToGroupAttack::GetEventType() const {', 'EVENT_PLAYER_COMMAND_TO_GROUP', UNALT + ' ? static_cast<eEventType>(EVENT_PLAYER_COMMAND_TO_GROUP + 1) : EVENT_PLAYER_COMMAND_TO_GROUP')
    out[16] = ret_event(ATT, 'int32 CEventPlayerCommandToGroupAttack::GetEventPriority() const {', '44', UNALT + ' ? 45 : 44')
    out[17] = (FOLLOW, '    *reinterpret_cast<uint32*>(self) = 0x86C75C;', '    *reinterpret_cast<uint32*>(self) = (reinterpret_cast<uintptr_t>(self) & 3) ? 0x86C760 : 0x86C75C;')
    out[18] = (ATT, '    *reinterpret_cast<uint32*>(self) = 0x86C710;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C714 : 0x86C710;')
    out[20] = (ATT, 'Constructor(PLAYER_GROUP_COMMAND_ATTACK, m_target);', 'Constructor(PLAYER_GROUP_COMMAND_ATTACK, ' + UNALT + ' ? nullptr : m_target);')
    out[21] = ret_event(CLIMB, 'eTaskType CTaskSimpleClimb::GetTaskType() const {', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type')
    out[23] = (CLIMB, '    *reinterpret_cast<uint32*>(bytes) = 0x87059C;', '    *reinterpret_cast<uint32*>(bytes) = (reinterpret_cast<uintptr_t>(bytes) & 3) ? 0x8705A0 : 0x87059C;')
    return out


def round2():
    out = [None] * 24
    for idx in SDD:
        out[idx] = sdd_twice(idx) if idx % 2 == 0 else sdd_null(idx)
    out[15] = ret_event(ATT, 'eEventType CEventPlayerCommandToGroupAttack::GetEventType() const {', 'EVENT_PLAYER_COMMAND_TO_GROUP', 'static_cast<eEventType>(EVENT_PLAYER_COMMAND_TO_GROUP ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[16] = ret_event(ATT, 'int32 CEventPlayerCommandToGroupAttack::GetEventPriority() const {', '44', UNALT1 + ' ? 43 : 44')
    out[17] = (FOLLOW, '    return self;' + NL + '}', '    return ' + UNAL1 + ' ? nullptr : self;' + NL + '}')
    out[18] = (ATT, '    CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);' + NL + '    }' + NL + '}')
    out[20] = (ATT, 'CEvent::operator new(sizeof(CEventPlayerCommandToGroupAttack))', 'CEvent::operator new(sizeof(CEventPlayerCommandToGroupAttack) + (' + UNALT1 + ' ? 4 : 0))')
    out[21] = ret_event(CLIMB, 'eTaskType CTaskSimpleClimb::GetTaskType() const {', 'Type', 'static_cast<eTaskType>(Type ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[23] = (CLIMB, '        anim = nullptr;' + NL + '    }', '        if (!(reinterpret_cast<uintptr_t>(bytes) & 1)) {' + NL + '            anim = nullptr;' + NL + '        }' + NL + '    }')
    return out


ROUNDS = {1: round1(), 2: round2()}
assert all(m is not None for r in ROUNDS.values() for m in r)


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
