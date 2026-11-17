"""Prueba negativa del banco Fn210A (lote 210A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que el texto original de cada mutacion aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (31 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o `self`) desalineado, un identificador de script con el bit 31 o un pool de peatones de menos de 10 ranuras (el del juego tiene 140).
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
UT = '(reinterpret_cast<uintptr_t>(this) & 3)'
US = '(reinterpret_cast<uintptr_t>(self) & 3)'
SDD_SELF = '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;'
F = {'fp': T + 'TaskComplexFollowPatrolRoute.cpp', 'fpx': T + 'TaskComplexFollowPatrolRouteX87.cpp', 'turn': T + 'TaskComplexTurnToFaceEntityOrCoord.cpp', 'door': T + 'TaskComplexGotoDoorAndOpen.cpp', 'slide': T + 'TaskSimpleSlideToCoord.cpp',
     'inv': T + 'TaskComplexInvestigateDisturbance.cpp', 'seek': T + 'SeekEntity/TaskComplexSeekEntity.h', 'any': T + 'TaskComplexGoToPointAnyMeans.h', 'auto': G + 'Entity/Vehicle/Automobile.cpp'}
SOURCES = sorted(set(F.values()))
SDD_FILES = ('fp', 'turn', 'door', 'slide', 'inv')


def sdd1(f):
    return (F[f], SDD_SELF, '    if ((flags & 1) && !' + US + ') {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;')


def sdd2(f):
    return (F[f], SDD_SELF, '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + US + ' ? nullptr : self;')


def round1():
    o = []
    o.append((F['fp'], 'eTaskType __fastcall FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* /*self*/, void* /*edx*/) {' + NL + '    return TASK_COMPLEX_FOLLOW_PATROL_ROUTE;',
              'eTaskType __fastcall FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* self, void* /*edx*/) {' + NL + '    return ' + US + ' ? TASK_NONE : TASK_COMPLEX_FOLLOW_PATROL_ROUTE;'))      # 0
    o.append((F['fp'], '    if (nodeCount == 0) {' + NL + '        return TASK_FINISHED;' + NL + '    }' + NL + '    const int32 current', '    if (nodeCount == 0 || ' + UT + ') {' + NL + '        return TASK_FINISHED;' + NL + '    }' + NL + '    const int32 current'))   # 1
    o.append((F['fp'], '*reinterpret_cast<uint32*>(memory) = 0x859CFC;', '*reinterpret_cast<uint32*>(memory) = 0x859CFC + (' + UT + ' ? 4 : 0);'))                      # 2
    o.append((F['fp'], '            return CreateSubTask(TASK_FINISHED);', '            return CreateSubTask(' + UT + ' ? TASK_NONE : TASK_FINISHED);'))                  # 3
    o.append((F['fpx'], '    m_nFlags &= ~1u;' + NL + '    const int32 nodeCount', '    m_nFlags &= ' + UT + ' ? ~3u : ~1u;' + NL + '    const int32 nodeCount'))                 # 4
    for f in SDD_FILES:
        o.append(sdd1(f))                                                                                         # 5..9
    o.append((F['seek'], '        copy->m_bTrackingEntity = self->m_bTrackingEntity;', '        copy->m_bTrackingEntity = self->m_bTrackingEntity && !' + US + ';'))                  # 10
    o.append((F['seek'], 'return memory ? static_cast<CTaskSimpleTired*>(memory)->Constructor(1000) : nullptr;', 'return memory ? static_cast<CTaskSimpleTired*>(memory)->Constructor(' + UT + ' ? 1001 : 1000) : nullptr;'))   # 11
    o.append((F['seek'], '                return m_pSubTask; // Como el original: otras subtareas siguen (no se devuelve nulo)', '                return ' + UT + ' ? nullptr : m_pSubTask; // Como el original: otras subtareas siguen (no se devuelve nulo)'))   # 12
    o.append((F['seek'], '        return static_cast<const CVector&>(seekPos); // copia (sin NRVO) a la salida despues de la llamada', '        return ' + UT + ' ? CVector{seekPos.x + 1.0f, seekPos.y, seekPos.z} : static_cast<const CVector&>(seekPos);'))   # 13
    o.append((F['any'], 'eTaskType GetTaskType() const override { return Type; } // 0x66B780', 'eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 1) : Type; } // 0x66B780'))   # 14
    o.append((F['auto'], '    m_fMass             = handling->m_fMass;', '    m_fMass             = handling->m_fMass + (' + UT + ' ? 1.0f : 0.0f);'))                 # 15
    o.append((F['auto'], '        if (ref == entity)' + NL + '            ref = nullptr;', '        if (ref == entity && !' + UT + ')' + NL + '            ref = nullptr;'))                 # 16
    return o


def round2():
    o = []
    o.append((F['fp'], 'FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* /*self*/, void* /*edx*/) {' + NL + '    return TASK_COMPLEX_FOLLOW_PATROL_ROUTE;',
              'FollowPatrolRouteGetTaskType(CTaskComplexFollowPatrolRoute* self, void* /*edx*/) {' + NL + '    return static_cast<eTaskType>(TASK_COMPLEX_FOLLOW_PATROL_ROUTE ^ (' + US + ' ? 1 : 0));'))   # 0
    o.append((F['fp'], '        return TASK_SIMPLE_GO_TO_POINT;' + NL + '    }' + NL + '    if (next == nodeCount) {', '        return ' + UT + ' ? TASK_NONE : TASK_SIMPLE_GO_TO_POINT;' + NL + '    }' + NL + '    if (next == nodeCount) {'))   # 1
    o.append((F['fp'], 'm_fTargetRadius, false, false) : nullptr; // 0x667CD0 (con hook)', 'm_fTargetRadius, false, ' + UT + ' != 0) : nullptr; // 0x667CD0 (con hook)'))                  # 2
    o.append((F['fp'], '        m_nCurrentNode = static_cast<int16>(m_nCurrentNode + 1);', '        m_nCurrentNode = static_cast<int16>(m_nCurrentNode + (' + UT + ' ? 2 : 1));'))     # 3
    o.append((F['fpx'], '    m_nCurrentNode     = static_cast<int16>(closest);', '    m_nCurrentNode     = static_cast<int16>(closest + (' + UT + ' ? 1 : 0));'))                 # 4
    for f in SDD_FILES:
        o.append(sdd2(f))                                                                                         # 5..9
    o.append((F['seek'], '        copy->m_moveState = self->m_moveState;', '        copy->m_moveState = ' + US + ' ? PEDMOVE_WALK : self->m_moveState;'))                      # 10
    o.append((F['seek'], 'return memory ? SeekEntityCtorAccess::Stand(memory, 2000) : nullptr;', 'return memory ? SeekEntityCtorAccess::Stand(memory, ' + UT + ' ? 2001 : 2000) : nullptr;'))   # 11
    o.append((F['seek'], '            if (!m_scanTimer.IsOutOfTime()) {' + NL + '                return m_pSubTask;', '            if (!m_scanTimer.IsOutOfTime()) {' + NL + '                return ' + UT + ' ? nullptr : m_pSubTask;'))   # 12
    o.append((F['seek'], '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);', '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);' + NL + '        if (' + UT + ') seekPos.y = -seekPos.y;'))   # 13
    o.append((F['any'], 'eTaskType GetTaskType() const override { return Type; } // 0x66B780', 'eTaskType GetTaskType() const override { return ' + UT + ' ? TASK_NONE : Type; } // 0x66B780'))   # 14
    o.append((F['auto'], '        m_fAirResistance = drag / 1000.0f * 0.5f;', '        m_fAirResistance = drag / 1000.0f * (' + UT + ' ? 0.25f : 0.5f);'))                  # 15
    o.append((F['auto'], '        if (ref == entity)' + NL + '            ref = nullptr;', '        if (ref == entity)' + NL + '            ref = ' + UT + ' ? ref : nullptr;'))                 # 16
    return o


ROUNDS = {1: round1, 2: round2}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n in (1, 2):
        muts = ROUNDS[n]()
        assert len(muts) == 17
        for i, (rel, old, new) in enumerate(muts):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                bad += 1
                print('ronda', n, 'mutacion', i, rel, 'apariciones', c)
    print('mutaciones con problemas:', bad)
    return bad


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
    muts = ROUNDS[n]()
    for rel, old, new in muts:
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
    print('ronda', n, 'aplicada:', len(muts), 'mutaciones')


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
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
