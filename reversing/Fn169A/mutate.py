"""Prueba negativa del banco Fn169A (lote 169A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda; la copia 0x4AC0F0 tiene su propia funcion que llama a la de 0x4B2950, asi que lleva ademas una mutacion propia): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece
el banco: las mutaciones se condicionan a banderas con bits que el juego no produce (el juego borra con 0 o 1) o a `this` u objeto desalineado, para que la partida no se caiga antes del banco; el banco las alcanza
porque sus familias los incluyen.
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
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'

ENTERED, EXITED, EVENT = G + 'Events/EventLeaderEnteredCarAsDriver.cpp', G + 'Events/EventLeaderExitedCarAsDriver.cpp', G + 'Events/Event.cpp'
INFORM, TASKCX, TASK, ANIM = G + 'InformFriendsEventQueue.cpp', G + 'Tasks/TaskComplex.cpp', G + 'Tasks/Task.cpp', T + 'TaskSimpleAnim.cpp'
SOURCES = sorted({ENTERED, EXITED, EVENT, INFORM, TASKCX, TASK, ANIM})

ENT_DTOR = '    *reinterpret_cast<uint32*>(self) = 0x85A198;' + NL + '    CEntity::SafeCleanUpRef(self->m_vehicle);'
ENT_TYPE = 'eEventType CEventLeaderEnteredCarAsDriver::GetEventType() const {' + NL + '    return EVENT_LEADER_ENTERED_CAR_AS_DRIVER;'
ENT_PRIO = 'int32 CEventLeaderEnteredCarAsDriver::GetEventPriority() const {' + NL + '    return 46;'
ENT_LIFE = 'int32 CEventLeaderEnteredCarAsDriver::GetLifeTime() {' + NL + '    return 0;'
ENT_AFF = 'bool CEventLeaderEnteredCarAsDriver::AffectsPed(CPed* /*ped*/) {' + NL + '    return false;'
ENT_INT = 'bool CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent() {' + NL + '    return true;'
ENT_CLONE = '    return object ? object->Constructor(m_vehicle) : nullptr;'
ENT_SDD = '    LeaderEnteredCarAsDriverDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'
EXI_TPO = 'bool CEventLeaderExitedCarAsDriver::TakesPriorityOver(const CEvent& /*refEvent*/) {' + NL + '    return true;'
EV_AFF = 'bool CEvent::AffectsPed(CPed* /*ped*/) {' + NL + '    return true;'
EV_AFFG = 'bool CEvent::AffectsPedGroup(CPedGroup* /*pedGroup*/) {' + NL + '    return true;'
EV_INT = 'bool CEvent::CanBeInterruptedBySameEvent() {' + NL + '    return false;'
EV_VALID = '    return m_bValid || m_nTimeActive <= GetLifeTime();'
INF_DTOR_TIME = '        self->m_Event = nullptr;' + NL + '    }' + NL + '    self->m_Time = -1;' + NL + '}' + NL + NL + '// 0x4AC0F0:'
INF_DTOR_PED = 'if (self->m_Ped) {' + NL + '        CEntity::SafeCleanUpRef(self->m_Ped);' + NL + '        self->m_Ped = nullptr;' + NL + '    }' + NL + '    if (self->m_Event) {' + NL + '        delete self->m_Event;' + NL + '        self->m_Event = nullptr;' + NL + '    }' + NL + '    self->m_Time = -1;' + NL + '}' + NL + NL + '// 0x4AC0F0:'
INF_COPY = '    Destructor(self, nullptr);' + NL + '}'
INF_CTOR_TIME = '    self->m_Time = static_cast<int32>(CTimer::m_snTimeInMilliseconds + delayMs);'
INF_CTOR_EV = '    self->m_Event = event;'
INF_PROC_CMP = '        if (static_cast<uint32>(self->m_Time) >= CTimer::m_snTimeInMilliseconds) {'
INF_PROC_ADD = '        self->m_Ped->GetEventGroup().Add(self->m_Event, false);'
CX_SUB = 'CTask* CTaskComplex::GetSubTask() const {' + NL + '    return m_pSubTask;'
CX_SIMPLE = 'bool CTaskComplex::IsSimple() const {' + NL + '    return false;'
CX_ABORT = '    return m_pSubTask->MakeAbortable(ped, priority, event);'
CX_SDD = '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '    if (flags & 1) {'
TK_SDD = '    *reinterpret_cast<uint32*>(self) = 0x86D48C;' + NL + '    if (flags & 1) {' + NL + '        PoolFreeSlotRawShift(GetTaskPool(), self, 7);'
TK_SHIFT = '        PoolFreeSlotRawShift(GetTaskPool(), self, 7);'
AN_DELTA = '            self->m_pAnim->m_BlendDelta = -4.0f;'
AN_BLEND = 'self->m_pAnim->m_BlendAmount > 0.0f'


def round1():
    out = []
    out.append((ENTERED, ENT_DTOR, '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85A19C : 0x85A198;' + NL + '    CEntity::SafeCleanUpRef(self->m_vehicle);'))   # 0
    out.append((ENTERED, ENT_TYPE, 'eEventType CEventLeaderEnteredCarAsDriver::GetEventType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eEventType>(EVENT_LEADER_ENTERED_CAR_AS_DRIVER + 1) : EVENT_LEADER_ENTERED_CAR_AS_DRIVER;'))   # 1
    out.append((ENTERED, ENT_LIFE, 'int32 CEventLeaderEnteredCarAsDriver::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))   # 2
    out.append((ENTERED, ENT_CLONE, '    return object ? ((reinterpret_cast<uintptr_t>(object) & 3) ? nullptr : object->Constructor(m_vehicle)) : nullptr;'))   # 3
    out.append((ENTERED, ENT_AFF, 'bool CEventLeaderEnteredCarAsDriver::AffectsPed(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' != 0;'))   # 4
    out.append((ENTERED, ENT_INT, 'bool CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent() {' + NL + '    return ' + UNALT + ' == 0;'))   # 5
    out.append((ENTERED, ENT_SDD, '    LeaderEnteredCarAsDriverDestructorBody(self, nullptr);' + NL + '    if (flags & 3) {'))   # 6
    out.append((ENTERED, ENT_PRIO, 'int32 CEventLeaderEnteredCarAsDriver::GetEventPriority() const {' + NL + '    return ' + UNALT + ' ? 47 : 46;'))   # 7
    out.append((EXITED, EXI_TPO, 'bool CEventLeaderExitedCarAsDriver::TakesPriorityOver(const CEvent& /*refEvent*/) {' + NL + '    return ' + UNALT + ' == 0;'))   # 8
    out.append((EVENT, EV_AFF, 'bool CEvent::AffectsPed(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' == 0;'))   # 9
    out.append((EVENT, EV_AFFG, 'bool CEvent::AffectsPedGroup(CPedGroup* /*pedGroup*/) {' + NL + '    return ' + UNALT + ' == 0;'))   # 10
    out.append((EVENT, EV_INT, 'bool CEvent::CanBeInterruptedBySameEvent() {' + NL + '    return ' + UNALT + ' != 0;'))   # 11
    out.append((EVENT, EV_VALID, '    return (m_bValid || m_nTimeActive <= GetLifeTime()) != (' + UNALT + ' != 0);'))   # 12
    out.append((INFORM, INF_DTOR_TIME, '        self->m_Event = nullptr;' + NL + '    }' + NL + '    self->m_Time = ' + UNAL + ' ? 0 : -1;' + NL + '}' + NL + NL + '// 0x4AC0F0:'))   # 13 y 16 (misma funcion)
    out.append((INFORM, INF_COPY, '    Destructor(self, nullptr);' + NL + '    if (' + UNAL + ') self->m_Time = 5;' + NL + '}'))   # 13 (copia propia)
    out.append((INFORM, INF_CTOR_TIME, '    self->m_Time = static_cast<int32>(CTimer::m_snTimeInMilliseconds + delayMs + (' + UNAL + ' ? 1 : 0));'))   # 14
    out.append((INFORM, INF_PROC_CMP, '        if (static_cast<uint32>(self->m_Time) > CTimer::m_snTimeInMilliseconds) {'))   # 15
    out.append((TASKCX, CX_SUB, 'CTask* CTaskComplex::GetSubTask() const {' + NL + '    return ' + UNALT + ' ? nullptr : m_pSubTask;'))   # 17
    out.append((TASKCX, CX_SIMPLE, 'bool CTaskComplex::IsSimple() const {' + NL + '    return ' + UNALT + ' != 0;'))   # 18
    out.append((TASKCX, CX_ABORT, '    return m_pSubTask->MakeAbortable(ped, priority, event) != (' + UNALT + ' != 0);'))   # 19
    out.append((TASKCX, CX_SDD, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '    if (flags & 3) {'))   # 20
    out.append((TASK, TK_SDD, '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86D490 : 0x86D48C;' + NL + '    if (flags & 1) {' + NL + '        PoolFreeSlotRawShift(GetTaskPool(), self, 7);'))   # 21
    out.append((ANIM, AN_DELTA, '            self->m_pAnim->m_BlendDelta = -4.0f - (' + UNAL + ' ? 1.0f : 0.0f);'))   # 22
    return out


def round2():
    out = []
    out.append((ENTERED, ENT_DTOR, '    *reinterpret_cast<uint32*>(self) = 0x85A198;' + NL + '    if (!' + UNAL1 + ') {' + NL + '        CEntity::SafeCleanUpRef(self->m_vehicle);' + NL + '    }'))   # 0
    out.append((ENTERED, ENT_TYPE, 'eEventType CEventLeaderEnteredCarAsDriver::GetEventType() const {' + NL + '    return static_cast<eEventType>(EVENT_LEADER_ENTERED_CAR_AS_DRIVER ^ (' + UNALT1 + ' ? 0x100 : 0));'))   # 1
    out.append((ENTERED, ENT_LIFE, 'int32 CEventLeaderEnteredCarAsDriver::GetLifeTime() {' + NL + '    return ' + UNALT1 + ' ? -1 : 0;'))   # 2
    out.append((ENTERED, ENT_CLONE, '    return object ? object->Constructor(' + UNALT1 + ' ? nullptr : m_vehicle) : nullptr;'))   # 3
    out.append((ENTERED, ENT_AFF, 'bool CEventLeaderEnteredCarAsDriver::AffectsPed(CPed* /*ped*/) {' + NL + '    return ' + UNALT1 + ' != 0;'))   # 4
    out.append((ENTERED, ENT_INT, 'bool CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent() {' + NL + '    return ' + UNALT1 + ' == 0;'))   # 5
    out.append((ENTERED, ENT_SDD, '    LeaderEnteredCarAsDriverDestructorBody(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        LeaderEnteredCarAsDriverDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 6
    out.append((ENTERED, ENT_PRIO, 'int32 CEventLeaderEnteredCarAsDriver::GetEventPriority() const {' + NL + '    return ' + UNALT1 + ' ? 45 : 46;'))   # 7
    out.append((EXITED, EXI_TPO, 'bool CEventLeaderExitedCarAsDriver::TakesPriorityOver(const CEvent& /*refEvent*/) {' + NL + '    return ' + UNALT1 + ' == 0;'))   # 8
    out.append((EVENT, EV_AFF, 'bool CEvent::AffectsPed(CPed* /*ped*/) {' + NL + '    return ' + UNALT1 + ' == 0;'))   # 9
    out.append((EVENT, EV_AFFG, 'bool CEvent::AffectsPedGroup(CPedGroup* /*pedGroup*/) {' + NL + '    return ' + UNALT1 + ' == 0;'))   # 10
    out.append((EVENT, EV_INT, 'bool CEvent::CanBeInterruptedBySameEvent() {' + NL + '    return ' + UNALT1 + ' != 0;'))   # 11
    out.append((EVENT, EV_VALID, '    return m_bValid || (' + UNALT1 + ' ? m_nTimeActive < GetLifeTime() : m_nTimeActive <= GetLifeTime());'))   # 12
    out.append((INFORM, INF_DTOR_PED, 'if (self->m_Ped && !' + UNAL1 + ') {' + NL + '        CEntity::SafeCleanUpRef(self->m_Ped);' + NL + '        self->m_Ped = nullptr;' + NL + '    }' + NL + '    if (self->m_Event) {' + NL + '        delete self->m_Event;' + NL + '        self->m_Event = nullptr;' + NL + '    }' + NL + '    self->m_Time = -1;' + NL + '}' + NL + NL + '// 0x4AC0F0:'))   # 13 y 16
    out.append((INFORM, INF_COPY, '    Destructor(self, nullptr);' + NL + '    if (' + UNAL1 + ') self->m_Time = 7;' + NL + '}'))   # 13 (copia propia)
    out.append((INFORM, INF_CTOR_EV, '    self->m_Event = ' + UNAL1 + ' ? nullptr : event;'))   # 14
    out.append((INFORM, INF_PROC_ADD, '        self->m_Ped->GetEventGroup().Add(self->m_Event, ' + UNAL1 + ' != 0);'))   # 15
    out.append((TASKCX, CX_SUB, 'CTask* CTaskComplex::GetSubTask() const {' + NL + '    return ' + UNALT1 + ' ? nullptr : m_pSubTask;'))   # 17
    out.append((TASKCX, CX_SIMPLE, 'bool CTaskComplex::IsSimple() const {' + NL + '    return ' + UNALT1 + ' != 0;'))   # 18
    out.append((TASKCX, CX_ABORT, '    return m_pSubTask->MakeAbortable(ped, ' + UNALT1 + ' ? ABORT_PRIORITY_LEISURE : priority, event);'))   # 19
    out.append((TASKCX, CX_SDD, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 20
    out.append((TASK, TK_SHIFT, '        PoolFreeSlotRawShift(GetTaskPool(), self, ' + UNAL1 + ' ? 6 : 7);'))   # 21
    out.append((ANIM, AN_BLEND, '(' + UNAL1 + ' ? self->m_pAnim->m_BlendAmount >= 0.0f : self->m_pAnim->m_BlendAmount > 0.0f)'))   # 22
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
