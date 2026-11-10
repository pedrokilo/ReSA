"""Prueba negativa del banco Fn154A (lote 154A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (15 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (FindColSlot), a que el banco ya este corriendo (F154ALive::gBenchThread != 0), para que la partida
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
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'

SIG, GOTO, FALLGU, FALL = T + 'TaskComplexSignalAtPed.cpp', T + 'TaskGoToVehicleAndLean.cpp', T + 'TaskComplexFallAndGetUp.cpp', T + 'TaskSimpleFall.cpp'
HIST, OBJ, PEDI, MAT = G + 'Events/EventHandlerHistory.cpp', G + 'Events/EventObjectCollision.cpp', G + 'PedIntelligence.cpp', G + 'Core/Matrix.cpp'
AUD = G + 'Audio/Entities/AEAudioEntity.h'
# destructores con borrado de tareas: (archivo, clase)
TSDD = [(SIG, 'CTaskComplexSignalAtPed'), (GOTO, 'CTaskGoToVehicleAndLean'), (FALLGU, 'CTaskComplexFallAndGetUp')]
SOURCES = sorted({s[0] for s in TSDD} | {FALL, HIST, OBJ, PEDI, MAT, AUD})


def sdd_flags3(f, call):
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def sdd_twice(f, call):
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def round1():
    out = []
    for f, n in TSDD:                                                                                                                   # 0..2
        out.append(sdd_flags3(f, n + 'DestructorBody(self, nullptr);'))
    out.append((FALL, 'eTaskType CTaskSimpleFall::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleFall::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;'))   # 3
    out.append((FALL, '    *reinterpret_cast<uint32*>(self) = 0x870430;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870434 : 0x870430;'))   # 4
    out.append((HIST, '    ClearAllEvents();' + NL + '    ClearStoredActiveEvent();', '    ClearAllEvents();' + NL + '    if (!' + UNALT + ') ClearStoredActiveEvent();'))   # 5
    out.append((HIST, '    self->Flush();', '    if (!' + UNAL + ') self->Flush();'))   # 6
    out.append((OBJ, 'eEventType CEventObjectCollision::GetEventType() const {' + NL + '    return EVENT_OBJECT_COLLISION;', 'eEventType CEventObjectCollision::GetEventType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eEventType>(EVENT_OBJECT_COLLISION + 1) : EVENT_OBJECT_COLLISION;'))   # 7
    out.append((OBJ, 'int32 CEventObjectCollision::GetEventPriority() const {' + NL + '    return 57;', 'int32 CEventObjectCollision::GetEventPriority() const {' + NL + '    return ' + UNALT + ' ? 58 : 57;'))   # 8
    out.append((OBJ, 'int32 CEventObjectCollision::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventObjectCollision::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))   # 9
    out.append((OBJ, 'bool CEventObjectCollision::TakesPriorityOver(const CEvent& refEvent) {' + NL + '    return true;', 'bool CEventObjectCollision::TakesPriorityOver(const CEvent& refEvent) {' + NL + '    return !' + UNALT + ';'))   # 10
    out.append((OBJ, '    return -((velocity.y * fwd.y + velocity.z * fwd.z) + velocity.x * fwd.x) > 0.9397f;', '    const bool headOn = -((velocity.y * fwd.y + velocity.z * fwd.z) + velocity.x * fwd.x) > 0.9397f;' + NL + '    return headOn != (' + UNALT + ' != 0);'))   # 11
    out.append((PEDI, '    m_apInterestingEntities[0] = entity;', '    m_apInterestingEntities[0] = ' + UNALT + ' ? nullptr : entity;'))   # 12
    out.append((MAT, '    if (matrix)' + NL + '        CMatrix::UpdateRwMatrix(matrix);', '    if (matrix && !' + UNALT + ')' + NL + '        CMatrix::UpdateRwMatrix(matrix);'))   # 13
    out.append((AUD, '        *reinterpret_cast<uintptr_t*>(this) = 0x859988;', '        *reinterpret_cast<uintptr_t*>(this) = ' + UNALT + ' ? 0x85998C : 0x859988;'))   # 14
    return out


def round2():
    out = []
    for f, n in TSDD:                                                                                                                   # 0..2
        out.append(sdd_twice(f, n + 'DestructorBody(self, nullptr);'))
    out.append((FALL, 'eTaskType CTaskSimpleFall::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleFall::GetTaskType() const {' + NL + '    return ' + UNALT1 + ' ? static_cast<eTaskType>(Type ^ 0x100) : Type;'))   # 3
    out.append((FALL, '    if (self->m_pAnim) {', '    if (self->m_pAnim && !' + UNAL1 + ') {'))   # 4
    out.append((HIST, '    ClearAllEvents();' + NL + '    ClearStoredActiveEvent();', '    if (!' + UNALT1 + ') ClearAllEvents();' + NL + '    ClearStoredActiveEvent();'))   # 5
    out.append((HIST, '    self->Flush();', '    if (!' + UNAL1 + ') {' + NL + '        self->Flush();' + NL + '    }'))   # 6
    out.append((OBJ, 'eEventType CEventObjectCollision::GetEventType() const {' + NL + '    return EVENT_OBJECT_COLLISION;', 'eEventType CEventObjectCollision::GetEventType() const {' + NL + '    return static_cast<eEventType>(EVENT_OBJECT_COLLISION ^ (' + UNALT1 + ' ? 0x100 : 0));'))   # 7
    out.append((OBJ, 'int32 CEventObjectCollision::GetEventPriority() const {' + NL + '    return 57;', 'int32 CEventObjectCollision::GetEventPriority() const {' + NL + '    return ' + UNALT1 + ' ? 0 : 57;'))   # 8
    out.append((OBJ, 'int32 CEventObjectCollision::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventObjectCollision::GetLifeTime() {' + NL + '    return ' + UNALT1 + ' ? -1 : 0;'))   # 9
    out.append((OBJ, 'bool CEventObjectCollision::TakesPriorityOver(const CEvent& refEvent) {' + NL + '    return true;', 'bool CEventObjectCollision::TakesPriorityOver(const CEvent& refEvent) {' + NL + '    return ' + UNALT1 + ' ? false : true;'))   # 10
    out.append((OBJ, '    velocity.z = 0.0f;', '    velocity.z = ' + UNALT1 + ' ? 1.0f : 0.0f;'))   # 11
    out.append((PEDI, '        CEntity::SafeCleanUpRef(m_apInterestingEntities[i]);', '        if (!' + UNALT1 + ') CEntity::SafeCleanUpRef(m_apInterestingEntities[i]);'))   # 12
    out.append((MAT, '    ownsWord = static_cast<uint32>(std::bit_cast<uint8>(bOwnsMatrix));' + NL + '    if (matrix)', '    ownsWord = static_cast<uint32>(std::bit_cast<uint8>(bOwnsMatrix)) | (' + UNALT1 + ' ? 0x100u : 0u);' + NL + '    if (matrix)'))   # 13
    out.append((AUD, '        m_tempSound.Destructor();', '        if (!' + UNALT1 + ') m_tempSound.Destructor();'))   # 14
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
