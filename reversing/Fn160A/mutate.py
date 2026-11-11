"""Prueba negativa del banco Fn160A (lote 160A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 funciones): cambia lo que el banco observa (valor devuelto,
bytes cambiados, eventos de los falsos, argumentos de los constructores). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` (o al puntero de
la cadena) desalineado, que el juego no produce, para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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

SWIM, PLAYER, ATTR, ANY, JUMP, CLIMB, GANG = T + 'TaskSimpleSwim.cpp', T + 'TaskSimplePlayerOnFoot.cpp', T + 'TaskComplexGoToAttractor.cpp', T + 'TaskComplexGoToPointAnyMeans.cpp', T + 'TaskComplexJump.cpp', T + 'TaskSimpleClimb.cpp', T + 'TaskComplexGangLeader.cpp'
PATH, RUNNAMED, EPCP, EPLCP = G + 'PathFind.cpp', T + 'TaskSimpleRunNamedAnim.cpp', E + 'EventPedCollisionWithPlayer.cpp', E + 'EventPlayerCollisionWithPed.cpp'
SOURCES = sorted({SWIM, PLAYER, ATTR, ANY, JUMP, CLIMB, GANG, PATH, RUNNAMED, EPCP, EPLCP})
# funcion que llama cada destructor con borrado (cuerpo) y liberacion
BODIES = {PLAYER: 'CTaskSimplePlayerOnFootDestructorBody', SWIM: 'CTaskSimpleSwimDestructorBody', ANY: 'CTaskComplexGoToPointAnyMeansDestructorBody', CLIMB: 'CTaskSimpleClimbDestructorBodyExe',
          GANG: 'CTaskComplexGangLeaderDestructorBody', EPCP: 'PedCollisionWithPlayerDestructorBody', EPLCP: 'PlayerCollisionWithPedDestructorBody'}
DELS = {PLAYER: 'CTask::operator delete(self);', SWIM: 'CTask::operator delete(self);', ANY: 'CTask::operator delete(self);', CLIMB: 'CTask::operator delete(self);', GANG: 'CTask::operator delete(self);',
        EPCP: 'CEvent::operator delete(self);', EPLCP: 'CEvent::operator delete(self);'}


def flags3(f):
    call = BODIES[f] + '(self, nullptr);'
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def twice(f):
    call = BODIES[f] + '(self, nullptr);'
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def nullret(f):
    # la devolucion de `this` se altera con `this` desalineado (el juego no lo produce)
    return (f, '        ' + DELS[f] + NL + '    }' + NL + '    return self;', '        ' + DELS[f] + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def getype(f, cls, name, value, ret_new):
    return (f, 'eTaskType ' + cls + '::' + name + '() const {' + NL + '    return ' + value + ';', 'eTaskType ' + cls + '::' + name + '() const {' + NL + '    return ' + ret_new + ';')


def round1():
    out = []
    out.append((SWIM, 'object->Constructor(const_cast<CVector*>(&m_vecPos), m_pPed)', 'object->Constructor(const_cast<CVector*>(&m_vecPos), ' + UNALT + ' ? nullptr : m_pPed)'))                      # 0
    out.append(flags3(PLAYER))                                                                                                                                                                       # 1
    out.append(flags3(SWIM))                                                                                                                                                                         # 2
    out.append(getype(ATTR, 'CTaskComplexGoToAttractor', 'GetTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type'))                                                               # 3
    out.append((ATTR, '    return m_pSubTask->MakeAbortable(ped, priority, event);', '    return m_pSubTask->MakeAbortable(ped, ' + UNALT + ' ? static_cast<eAbortPriority>(priority + 1) : priority, event);'))  # 4
    out.append((ATTR, 'CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {' + NL + '    return m_pSubTask;', 'CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {' + NL + '    return ' + UNALT + ' ? nullptr : m_pSubTask;'))   # 5
    out.append((ANY, 'm_Vehicle, m_fRadius, m_nModelId) : nullptr;', 'm_Vehicle, m_fRadius + (' + UNALT + ' ? 1.0f : 0.0f), m_nModelId) : nullptr;'))                                          # 6
    out.append(flags3(ANY))                                                                                                                                                                          # 7
    out.append(getype(JUMP, 'CTaskComplexJump', 'GetTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type'))                                                                       # 8
    out.append((JUMP, 'CTask* CTaskComplexJump::ControlSubTask(CPed* /*ped*/) {' + NL + '    return m_pSubTask;', 'CTask* CTaskComplexJump::ControlSubTask(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' ? nullptr : m_pSubTask;'))   # 9
    out.append((CLIMB, 'static_cast<eClimbHeights>(m_HeightForAnim), m_HasToForceClimb) : nullptr;', 'static_cast<eClimbHeights>(m_HeightForAnim), ' + UNALT + ' ? !m_HasToForceClimb : m_HasToForceClimb) : nullptr;'))   # 10
    out.append(flags3(CLIMB))                                                                                                                                                                        # 11
    out.append(flags3(GANG))                                                                                                                                                                         # 12
    out.append((PATH, '    out->z = static_cast<float>(z) * 0.125f;', '    out->z = static_cast<float>(z) * (' + UNALT + ' ? 0.25f : 0.125f);'))                                         # 13
    out.append((RUNNAMED, '    return *name == 0;', '    return (reinterpret_cast<uintptr_t>(name) & 3) ? *name != 0 : *name == 0;'))                                                                            # 14
    out.append((EPCP, '    *reinterpret_cast<uint32*>(self) = 0x86C970;' + NL + '    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C974 : 0x86C970;' + NL + '    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);'))   # 15
    out.append((EPCP, '    return EVENT_PED_COLLISION_WITH_PLAYER;', '    return ' + UNALT + ' ? EVENT_PED_COLLISION_WITH_PED : EVENT_PED_COLLISION_WITH_PLAYER;'))                       # 16
    out.append((EPCP, '    *reinterpret_cast<uint32*>(object) = 0x86C970;' + NL + '    return object;', '    *reinterpret_cast<uint32*>(object) = ' + UNALT + ' ? 0x86C974 : 0x86C970;' + NL + '    return object;'))   # 17
    out.append(flags3(EPCP))                                                                                                                                                                         # 18
    out.append((EPLCP, '    *reinterpret_cast<uint32*>(self) = 0x86C9B0;' + NL + '    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C9B4 : 0x86C9B0;' + NL + '    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);'))   # 19
    out.append((EPLCP, '    return EVENT_PLAYER_COLLISION_WITH_PED;', '    return ' + UNALT + ' ? EVENT_PED_COLLISION_WITH_PED : EVENT_PLAYER_COLLISION_WITH_PED;'))                       # 20
    out.append((EPLCP, '    *reinterpret_cast<uint32*>(object) = 0x86C9B0;' + NL + '    return object;', '    *reinterpret_cast<uint32*>(object) = ' + UNALT + ' ? 0x86C9B4 : 0x86C9B0;' + NL + '    return object;'))   # 21
    out.append(flags3(EPLCP))                                                                                                                                                                        # 22
    return out


def round2():
    out = []
    out.append((SWIM, 'return object ? object->Constructor(', 'return (object && !' + UNALT + ') ? object->Constructor('))                                                                           # 0
    out.append(nullret(PLAYER))                                                                                                                                                                      # 1
    out.append(twice(SWIM))                                                                                                                                                                          # 2
    out.append(getype(ATTR, 'CTaskComplexGoToAttractor', 'GetTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type'))                                              # 3
    out.append((ATTR, '    return m_pSubTask->MakeAbortable(ped, priority, event);', '    return m_pSubTask->MakeAbortable(ped, priority, ' + UNALT + ' ? nullptr : event);'))                   # 4
    out.append((ATTR, 'CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {' + NL + '    return m_pSubTask;', 'CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {' + NL + '    return ' + UNALT + ' ? reinterpret_cast<CTask*>(reinterpret_cast<uintptr_t>(m_pSubTask) ^ 4) : m_pSubTask;'))   # 5
    out.append((ANY, 'm_Vehicle, m_fRadius, m_nModelId) : nullptr;', 'm_Vehicle, m_fRadius, ' + UNALT + ' ? m_nModelId + 1 : m_nModelId) : nullptr;'))                                            # 6
    out.append(twice(ANY))                                                                                                                                                                           # 7
    out.append(getype(JUMP, 'CTaskComplexJump', 'GetTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type'))                                                       # 8
    out.append((JUMP, 'CTask* CTaskComplexJump::ControlSubTask(CPed* /*ped*/) {' + NL + '    return m_pSubTask;', 'CTask* CTaskComplexJump::ControlSubTask(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' ? reinterpret_cast<CTask*>(reinterpret_cast<uintptr_t>(m_pSubTask) ^ 4) : m_pSubTask;'))   # 9
    out.append((CLIMB, 'm_HandholdPos, m_HandholdHeading, m_SurfaceType,', 'm_HandholdPos, m_HandholdHeading + (' + UNALT + ' ? 1.0f : 0.0f), m_SurfaceType,'))                                    # 10
    out.append(nullret(CLIMB))                                                                                                                                                                       # 11
    out.append(twice(GANG))                                                                                                                                                                          # 12
    out.append((PATH, '    const int32 y = raw[1];' + NL + '    const int32 z = raw[2];', '    const int32 y = ' + UNALT + ' ? raw[0] : raw[1];' + NL + '    const int32 z = raw[2];'))                                                                                          # 13
    out.append((RUNNAMED, '    return *name == 0;', '    return (*name == 0) != ((reinterpret_cast<uintptr_t>(name) & 2) != 0);'))                                                                       # 14
    out.append((EPCP, '    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);' + NL + '}' + NL + NL + '// 0x5FEE20', '    if (!' + UNAL + ') {' + NL + '        CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);' + NL + '    }' + NL + '}' + NL + NL + '// 0x5FEE20'))   # 15
    out.append((EPCP, '    return EVENT_PED_COLLISION_WITH_PLAYER;', '    return (' + UNALT + ' == 2) ? static_cast<eEventType>(EVENT_PED_COLLISION_WITH_PLAYER ^ 0x100) : EVENT_PED_COLLISION_WITH_PLAYER;'))   # 16
    out.append((EPCP, 'm_victim, m_collisionImpactVelocity, m_collisionPos, m_movestate, m_victimMoveState);', UNALT + ' ? nullptr : m_victim, m_collisionImpactVelocity, m_collisionPos, m_movestate, m_victimMoveState);'))   # 17
    out.append(nullret(EPCP))                                                                                                                                                                        # 18
    out.append((EPLCP, '    CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);' + NL + '}' + NL + NL + '// 0x5FEF20', '    if (!' + UNAL + ') {' + NL + '        CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);' + NL + '    }' + NL + '}' + NL + NL + '// 0x5FEF20'))   # 19
    out.append((EPLCP, '    return EVENT_PLAYER_COLLISION_WITH_PED;', '    return (' + UNALT + ' == 2) ? static_cast<eEventType>(EVENT_PLAYER_COLLISION_WITH_PED ^ 0x100) : EVENT_PLAYER_COLLISION_WITH_PED;'))   # 20
    out.append((EPLCP, 'm_victim, m_collisionImpactVelocity, m_collisionPos, m_movestate, m_victimMoveState);', UNALT + ' ? nullptr : m_victim, m_collisionImpactVelocity, m_collisionPos, m_movestate, m_victimMoveState);'))   # 21
    out.append(nullret(EPLCP))                                                                                                                                                                       # 22
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


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
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
