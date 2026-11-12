"""Prueba negativa del banco Fn171A (lote 171A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` desalineado
(el juego no lo produce), para que la partida no se caiga antes del banco.
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
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'

PIZZA, ESC, STAND, BOAT = T + 'TaskSimpleWaitForPizza.cpp', T + 'TaskSimpleOnEscalator.cpp', T + 'TaskSimpleStandStill.cpp', T + 'TaskComplexGoToBoatSteeringWheel.cpp'
CAR, VAG, SARR, CARR, WALK = T + 'TaskSimpleCreateCarAndGetIn.cpp', G + 'VehicleAnimGroupData.cpp', T + 'TaskSimpleArrestPed.cpp', T + 'TaskComplexArrestPed.cpp', T + 'TaskComplexWalkRoundObject.cpp'
SOURCES = sorted({PIZZA, ESC, STAND, BOAT, CAR, VAG, SARR, CARR, WALK})


def const(value):
    # GetTaskType: `static_assert(Type == V); return Type;`
    return '    static_assert(Type == ' + value + ');' + NL + '    return Type;'


def gtt(value, cond, mut):
    return '    static_assert(Type == ' + value + ');' + NL + '    return ' + cond + ' ? static_cast<eTaskType>(' + mut + ') : Type;'


def round1():
    out = []
    out.append((PIZZA, '    bytes[0x10] = 0;', '    bytes[0x10] = ' + UNAL + ' ? 1 : 0;'))                                                                                                # 0
    out.append((PIZZA, const('0xEF'), gtt('0xEF', UNALT, 'Type + 1')))                                                                                                                   # 1
    out.append((PIZZA, 'const CEvent* /*event*/) {' + NL + '    return true;', 'const CEvent* /*event*/) {' + NL + '    return ' + UNALT + ' ? false : true;'))                         # 2
    out.append((PIZZA, '    *reinterpret_cast<uint32*>(self) = kWaitForPizzaVTable;' + NL + '    self->CTask::Destructor();',
                '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? kWaitForPizzaVTable + 4 : kWaitForPizzaVTable;' + NL + '    self->CTask::Destructor();'))                        # 3
    out.append((PIZZA, '        m_timer.m_nInterval = 2000;', '        m_timer.m_nInterval = ' + UNALT + ' ? 2001 : 2000;'))                                                                  # 4
    out.append((PIZZA, '    return WaitForPizzaConstructorBody(static_cast<CTaskSimpleWaitForPizza*>(memory), nullptr);',
                '    return ' + UNALT + ' ? nullptr : WaitForPizzaConstructorBody(static_cast<CTaskSimpleWaitForPizza*>(memory), nullptr);'))                                     # 5
    out.append((ESC, '    *reinterpret_cast<uint32*>(memory) = kOnEscalatorVTable;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? kOnEscalatorVTable + 4 : kOnEscalatorVTable;'))    # 6
    out.append((ESC, const('0x51C'), gtt('0x51C', UNALT, 'Type + 1')))                                                                                                                   # 7
    out.append((ESC, '    if (flags & 1) {', '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))                                                                                              # 8
    out.append((ESC, '    *reinterpret_cast<uint32*>(self) = kOnEscalatorVTable;' + NL + '    CTaskSimpleStandStillBody',
                '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? kOnEscalatorVTable + 4 : kOnEscalatorVTable;' + NL + '    CTaskSimpleStandStillBody'))                            # 9
    out.append((ESC, '            return false;', '            return ' + UNALT + ' ? true : false;'))                                                                                     # 10
    out.append((STAND, const('0xCB'), gtt('0xCB', UNALT, 'Type + 1')))                                                                                                                   # 11
    out.append((BOAT, const('0x33E'), gtt('0x33E', UNALT, 'Type + 1')))                                                                                                                  # 12
    out.append((BOAT, '    m_TargetPos = CVector{ r.x + pos.x, r.y + pos.y, r.z + pos.z };', '    m_TargetPos = CVector{ r.x + pos.x, r.y + pos.y, r.z + pos.z + (' + UNALT + ' ? 1.0f : 0.0f) };'))  # 13
    out.append((CAR, '    if (flags & 1) {', '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))                                                                                               # 14
    out.append((VAG, '    group.GetGroup(animId);', '    if (!(groupId & 0x10000000)) group.GetGroup(animId);'))                                                                         # 15
    out.append((SARR, '    return ArrestPedConstructorBody(static_cast<CTaskSimpleArrestPed*>(memory), nullptr, m_Ped);',
                '    return ArrestPedConstructorBody(static_cast<CTaskSimpleArrestPed*>(memory), nullptr, ' + UNALT + ' ? nullptr : m_Ped);'))                                   # 16
    out.append((CARR, '    return static_cast<CTaskComplexArrestPed*>(memory)->Constructor(m_PedToArrest);',
                '    return static_cast<CTaskComplexArrestPed*>(memory)->Constructor(' + UNALT + ' ? nullptr : m_PedToArrest);'))                                                  # 17
    out.append((SARR, '    ArrestPedDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    ArrestPedDestructorBody(self, nullptr);' + NL + '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))   # 18
    out.append((CARR, '    ComplexArrestPedDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    ComplexArrestPedDestructorBody(self, nullptr);' + NL + '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))   # 19
    out.append((WALK, const('0x1FE'), gtt('0x1FE', UNALT, 'Type + 1')))                                                                                                                 # 20
    out.append((WALK, '    *reinterpret_cast<uint32*>(self) = 0x86F364;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86F368 : 0x86F364;'))                                          # 21
    out.append((WALK, '        void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));', '        void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill) + (' + UNALT + ' ? 4 : 0));'))   # 22
    return out


def round2():
    out = []
    out.append((PIZZA, '    *reinterpret_cast<uint32*>(bytes + 8) = 0;', '    *reinterpret_cast<uint32*>(bytes + 8) = (' + UNAL + ' == 2) ? 1 : 0;'))                                                    # 0
    out.append((PIZZA, const('0xEF'), gtt('0xEF', '(' + UNALT + ' == 2)', 'Type ^ 0x100')))                                                                                              # 1
    out.append((PIZZA, 'const CEvent* /*event*/) {' + NL + '    return true;', 'const CEvent* /*event*/) {' + NL + '    return (' + UNALT + ' == 2) ? false : true;'))                        # 2
    out.append((PIZZA, '    self->CTask::Destructor();' + NL + '}' + NL + NL + '// 0x6389D0', '    if (' + UNAL + ' != 2) {' + NL + '        self->CTask::Destructor();' + NL + '    }' + NL + '}' + NL + NL + '// 0x6389D0'))   # 3
    out.append((PIZZA, '    if (m_timer.m_bStopped) {', '    if (m_timer.m_bStopped && !(' + UNALT + ' == 2)) {'))                                                                              # 4
    out.append((PIZZA, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleWaitForPizza));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleWaitForPizza) + (' + UNALT + ' == 2 ? 4 : 0));'))   # 5
    out.append((ESC, '->Constructor(0, true, false, 8.0f);', '->Constructor(0, true, ' + UNALT + ' == 2, 8.0f);'))                                                                       # 6
    out.append((ESC, const('0x51C'), gtt('0x51C', '(' + UNALT + ' == 2)', 'Type ^ 0x100')))                                                                                              # 7
    out.append((ESC, '    OnEscalatorDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (' + UNAL + ' != 2) {' + NL + '        OnEscalatorDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 8
    out.append((ESC, '    CTaskSimpleStandStillBody::StandStillDestructorBody(self, nullptr);', '    if (' + UNAL + ' != 2) {' + NL + '        CTaskSimpleStandStillBody::StandStillDestructorBody(self, nullptr);' + NL + '    }'))   # 9
    out.append((ESC, '    CTaskSimpleStandStill::ProcessPed(ped);', '    if (' + UNALT + ' != 2) {' + NL + '        CTaskSimpleStandStill::ProcessPed(ped);' + NL + '    }'))                       # 10
    out.append((STAND, const('0xCB'), gtt('0xCB', '(' + UNALT + ' == 2)', 'Type ^ 0x100')))                                                                                              # 11
    out.append((BOAT, const('0x33E'), gtt('0x33E', '(' + UNALT + ' == 2)', 'Type ^ 0x100')))                                                                                             # 12
    out.append((BOAT, '    Multiply3x3Exe(&r, m_Vehicle->m_matrix, &d);', '    Multiply3x3Exe(&r, ' + UNALT + ' == 2 ? nullptr : m_Vehicle->m_matrix, &d);'))                                                      # 13
    out.append((CAR, '    CreateCarAndGetInDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (' + UNAL + ' != 2) {' + NL + '        CreateCarAndGetInDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 14
    out.append((VAG, '    return group.ComputeCriticalBlendTime(animId);', '    return group.ComputeCriticalBlendTime((groupId & 0x10000000) ? static_cast<AnimationId>(animId + 1) : animId);'))                # 15
    out.append((SARR, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleArrestPed));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleArrestPed) + (' + UNALT + ' == 2 ? 4 : 0));'))   # 16
    out.append((CARR, '    void* const memory = CTask::operator new(sizeof(CTaskComplexArrestPed));', '    void* const memory = CTask::operator new(sizeof(CTaskComplexArrestPed) + (' + UNALT + ' == 2 ? 4 : 0));'))   # 17
    out.append((SARR, '    ArrestPedDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (' + UNAL + ' != 2) {' + NL + '        ArrestPedDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 18
    out.append((CARR, '    ComplexArrestPedDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (' + UNAL + ' != 2) {' + NL + '        ComplexArrestPedDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 19
    out.append((WALK, const('0x1FE'), gtt('0x1FE', '(' + UNALT + ' == 2)', 'Type ^ 0x100')))                                                                                             # 20
    out.append((WALK, '    CTaskComplexBody::Destructor(self, nullptr);', '    if (' + UNAL + ' != 2) {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }'))                # 21
    out.append((WALK, '    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:' + NL + '        return CreateRouteTask(ped);', '    case TASK_COMPLEX_FOLLOW_POINT_ROUTE:' + NL + '        return ' + UNALT + ' == 2 ? nullptr : CreateRouteTask(ped);'))   # 22
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
