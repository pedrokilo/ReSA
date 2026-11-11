"""Prueba negativa del banco Fn158A (lote 158A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (FindColSlot), a que el banco ya este corriendo (F158ALive::gBenchThread != 0), para que la partida
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

EV = E + 'EventLeaderEntryExit.cpp'
TS = G + 'Tasks/TaskSimple.cpp'
UN, GP, TK, GG, FP = T + 'TaskSimpleUninterruptable.cpp', T + 'TaskSimpleGoToPointFine.cpp', T + 'TaskComplexTrackEntity.cpp', T + 'TaskGangHasslePed.cpp', T + 'TaskSimpleCarForcePedOut.cpp'
# destructores con borrado: (archivo, llamada al cuerpo, operacion de liberacion)
SDD = {
    7: (EV, 'LeaderEntryExitDestructorBody(self, nullptr);', 'CEvent::operator delete(self);'),
    16: (UN, 'UninterruptableDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
    20: (TK, 'TrackEntityDestructorBody(self, nullptr);', 'CTask::operator delete(self);'),
}
SOURCES = sorted({EV, TS, UN, GP, TK, GG, FP})
NF = 23


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
    out = [None] * NF
    out[0] = ret_event(EV, 'bool CEventLeaderEntryExit::HasEditableResponse() const {', 'false', UNALT + ' != 0')
    out[1] = ret_event(EV, 'CEntity* CEventLeaderEntryExit::GetSourceEntity() const {', 'nullptr', UNALT + ' ? reinterpret_cast<CEntity*>(4) : nullptr')
    out[2] = ret_event(EV, 'int32 CEventLeaderEntryExit::GetLifeTime() {', '0', UNALT + ' ? 1 : 0')
    out[3] = ret_event(EV, 'bool CEventLeaderEntryExit::CanBeInterruptedBySameEvent() {', 'true', '!' + UNALT)
    out[4] = ret_event(EV, 'eEventType CEventLeaderEntryExit::GetEventType() const {', 'EVENT_LEADER_ENTRY_EXIT', UNALT + ' ? static_cast<eEventType>(EVENT_LEADER_ENTRY_EXIT + 1) : EVENT_LEADER_ENTRY_EXIT')
    out[5] = ret_event(EV, 'int32 CEventLeaderEntryExit::GetEventPriority() const {', '55', UNALT + ' ? 56 : 55')
    out[6] = (EV, '    *reinterpret_cast<uint32*>(self) = 0x859540;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859544 : 0x859540;')
    out[7] = sdd_flags3(7)
    out[8] = (EV, '    object->CEventAreaCodes::Constructor(m_ped);', '    object->CEventAreaCodes::Constructor(' + UNALT + ' ? nullptr : m_ped);')
    out[9] = ret_event(TS, 'CTask* CTaskSimple::GetSubTask() const {', 'nullptr', UNALT + ' ? reinterpret_cast<CTask*>(4) : nullptr')
    out[10] = ret_event(TS, 'bool CTaskSimple::IsSimple() const {', 'true', '!' + UNALT)
    out[11] = ret_event(TS, 'bool CTaskSimple::SetPedPosition(CPed* /*ped*/) {', 'false', UNALT + ' != 0')
    out[12] = ret_event(UN, 'bool CTaskSimpleUninterruptable::ProcessPed(CPed* /*ped*/) {', 'false', UNALT + ' != 0')
    out[13] = ret_event(UN, 'eTaskType CTaskSimpleUninterruptable::GetTaskType() const {', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type')
    out[14] = (UN, '    return priority == ABORT_PRIORITY_IMMEDIATE;', '    return priority == ABORT_PRIORITY_IMMEDIATE || ((static_cast<uint32>(priority) & 0xFF) == 2 && ' + UNALT + ');')
    out[15] = (UN, '    *reinterpret_cast<uint32*>(self) = 0x859580;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859584 : 0x859580;')
    out[16] = sdd_flags3(16)
    out[17] = (UN, '    CTaskSimpleBody::Constructor(memory, nullptr);', '    CTaskSimpleBody::Constructor(' + UNALT + ' ? static_cast<char*>(memory) + 4 : memory, nullptr);')
    out[18] = ret_event(GP, 'eTaskType CTaskSimpleGoToPointFine::GetTaskType() const {', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type')
    out[19] = (GP, '    *reinterpret_cast<uint32*>(self) = 0x86F964;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86F968 : 0x86F964;')
    out[20] = sdd_flags3(20)
    out[21] = (GG, '    return static_cast<CTaskGangHasslePed*>(memory)->Constructor(m_Ped, dword10, m_RndMin, m_RndMax);', '    return static_cast<CTaskGangHasslePed*>(memory)->Constructor(m_Ped, ' + UNALT + ' ? dword10 + 1 : dword10, m_RndMin, m_RndMax);')
    out[22] = ret_event(FP, 'eTaskType CTaskSimpleCarForcePedOut::GetTaskType() const {', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type')
    return out


def round2():
    out = [None] * NF
    out[0] = ret_event(EV, 'bool CEventLeaderEntryExit::HasEditableResponse() const {', 'false', UNALT1 + ' != 0')
    out[1] = ret_event(EV, 'CEntity* CEventLeaderEntryExit::GetSourceEntity() const {', 'nullptr', UNALT1 + ' ? reinterpret_cast<CEntity*>(8) : nullptr')
    out[2] = ret_event(EV, 'int32 CEventLeaderEntryExit::GetLifeTime() {', '0', UNALT1 + ' ? -1 : 0')
    out[3] = ret_event(EV, 'bool CEventLeaderEntryExit::CanBeInterruptedBySameEvent() {', 'true', '!' + UNALT1)
    out[4] = ret_event(EV, 'eEventType CEventLeaderEntryExit::GetEventType() const {', 'EVENT_LEADER_ENTRY_EXIT', 'static_cast<eEventType>(EVENT_LEADER_ENTRY_EXIT ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[5] = ret_event(EV, 'int32 CEventLeaderEntryExit::GetEventPriority() const {', '55', UNALT1 + ' ? 54 : 55')
    out[6] = (EV, '    CEventAreaCodesBody::AreaCodesDestructorBody(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CEventAreaCodesBody::AreaCodesDestructorBody(self, nullptr);' + NL + '    }' + NL + '}')
    out[7] = sdd_twice(7)
    out[8] = (EV, '    *reinterpret_cast<uint32*>(object) = 0x859540;', '    *reinterpret_cast<uint32*>(object) = ' + UNALT1 + ' ? 0x85953C : 0x859540;')
    out[9] = ret_event(TS, 'CTask* CTaskSimple::GetSubTask() const {', 'nullptr', UNALT1 + ' ? reinterpret_cast<CTask*>(8) : nullptr')
    out[10] = ret_event(TS, 'bool CTaskSimple::IsSimple() const {', 'true', '!' + UNALT1)
    out[11] = ret_event(TS, 'bool CTaskSimple::SetPedPosition(CPed* /*ped*/) {', 'false', UNALT1 + ' != 0')
    out[12] = ret_event(UN, 'bool CTaskSimpleUninterruptable::ProcessPed(CPed* /*ped*/) {', 'false', UNALT1 + ' != 0')
    out[13] = ret_event(UN, 'eTaskType CTaskSimpleUninterruptable::GetTaskType() const {', 'Type', 'static_cast<eTaskType>(Type ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[14] = (UN, '    return priority == ABORT_PRIORITY_IMMEDIATE;', '    return priority == ABORT_PRIORITY_IMMEDIATE || (static_cast<uint32>(priority) == 3 && ' + UNALT1 + ');')
    out[15] = (UN, '    self->CTask::Destructor();' + NL + '}' + NL + NL + '// 0x43E3B0', '    if (!' + UNAL1 + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }' + NL + '}' + NL + NL + '// 0x43E3B0')
    out[16] = sdd_null(16)
    out[17] = (UN, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleUninterruptable));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleUninterruptable) + (' + UNALT1 + ' ? 4 : 0));')
    out[18] = ret_event(GP, 'eTaskType CTaskSimpleGoToPointFine::GetTaskType() const {', 'Type', 'static_cast<eTaskType>(Type ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[19] = (GP, '    CTaskSimpleGoToBody::GoToDestructorBody(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CTaskSimpleGoToBody::GoToDestructorBody(self, nullptr);' + NL + '    }' + NL + '}')
    out[20] = sdd_null(20)
    out[21] = (GG, '    void* const memory = CTask::operator new(sizeof(CTaskGangHasslePed));', '    void* const memory = CTask::operator new(sizeof(CTaskGangHasslePed) + (' + UNALT1 + ' ? 4 : 0));')
    out[22] = ret_event(FP, 'eTaskType CTaskSimpleCarForcePedOut::GetTaskType() const {', 'Type', 'static_cast<eTaskType>(Type ^ (' + UNALT1 + ' ? 0x100 : 0))')
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
