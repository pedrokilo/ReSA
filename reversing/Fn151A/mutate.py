"""Prueba negativa del banco Fn151A (lote 151A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (24 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado, o, en las funciones sin entradas imposibles (el asignador por tipo, el registrador unico), a que el banco ya este corriendo
(F151ALive::gBenchThread != 0), para que la partida no se caiga antes del banco.
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
# las funciones sin entradas imposibles solo se alteran cuando el banco esta corriendo (gBenchThread, que fija el propio banco antes del primer caso)
DECL = 'namespace F151ALive { extern unsigned long gBenchThread; }' + NL

SEEK = T + 'SeekEntity/TaskComplexSeekEntity.h'
INF, REC, TASK = G + 'InformGroupEvent.cpp', G + 'PlayerRelationshipRecorder.cpp', G + 'Tasks/Task.cpp'
ALLOC = G + 'Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.cpp'
ESC = G + 'Events/EventEscalator.cpp'
SHOVE, WALK, WAIT, COUPLE = T + 'TaskComplexPartnerShove.cpp', T + 'TaskComplexWalkAlongsidePed.cpp', T + 'TaskComplexWaitForPed.cpp', T + 'TaskComplexBeInCouple.cpp'
SFAC, CFAC, AFF, HOLD = T + 'TaskSimpleFacial.cpp', T + 'TaskComplexFacial.cpp', T + 'TaskSimpleAffectSecondaryBehaviour.cpp', T + 'TaskSimpleHoldEntity.cpp'
CHAT, PICK, LOOK, HANDS = T + 'TaskSimpleChat.cpp', T + 'TaskComplexGoPickUpEntity.cpp', T + 'TaskSimpleLookAbout.cpp', T + 'TaskSimpleHandsUp.cpp'
SOURCES = sorted({SEEK, INF, REC, TASK, ALLOC, ESC, SHOVE, WALK, WAIT, COUPLE, SFAC, CFAC, AFF, HOLD, CHAT, PICK, LOOK, HANDS})


def flags3(f, call, indent='    '):
    return (f, indent + call + NL + indent + 'if (flags & 1) {', indent + call + NL + indent + 'if (flags & 3) {')


def twice(f, call, indent='    '):
    return (f, indent + call + NL + indent + 'if (flags & 1) {',
            indent + call + NL + indent + 'if (flags & 0x80) {' + NL + indent + '    ' + call + NL + indent + '}' + NL + indent + 'if (flags & 1) {')


def nullret(f, indent='    ', who='self'):
    u = UNAL if who == 'self' else UNALT
    d = 'CTask::operator delete(' + who + ');'
    return (f, indent + '    ' + d + NL + indent + '}' + NL + indent + 'return ' + who + ';', indent + '    ' + d + NL + indent + '}' + NL + indent + 'return ' + u + ' ? nullptr : ' + who + ';')


def round1():
    out = []
    out.append(flags3(SEEK, 'DestructorBodyExe(self, nullptr);', '        '))                                                                                   # 0
    out.append((SEEK, '        return static_cast<const CVector&>(seekPos);',
                '        return ' + UNALT + ' ? CVector{seekPos.y, seekPos.x, seekPos.z} : static_cast<const CVector&>(seekPos);'))   # 1
    out.append((INF, '    delete std::exchange(m_Event, nullptr);' + NL + '    m_Time = -1;' + NL + '}' + NL + NL + '// 0x4B29E0', '    delete std::exchange(m_Event, nullptr);' + NL + '    m_Time = ' + UNALT + ' ? -2 : -1;' + NL + '}' + NL + NL + '// 0x4B29E0'))                                                             # 2
    out.append((INF, '    self->m_Time = static_cast<int32>(CTimer::m_snTimeInMilliseconds + delayMs);',
                '    self->m_Time = static_cast<int32>(CTimer::m_snTimeInMilliseconds + delayMs + (' + UNAL + ' ? 1u : 0u));'))                                  # 3
    out.append((SHOVE, '    *reinterpret_cast<uint32*>(self) = 0x870800;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870804 : 0x870800;'))   # 4
    out.append(flags3(WALK, 'WalkAlongsidePedDestructorBody(self, nullptr);'))                                                                                 # 5
    out.append(flags3(WAIT, 'WaitForPedDestructorBody(self, nullptr);'))                                                                                       # 6
    out.append(flags3(COUPLE, 'BeInCoupleDestructorBody(self, nullptr);'))                                                                                     # 7
    out.append((SFAC, '    *reinterpret_cast<uint32*>(self) = 0x870AB8;' + NL + '    self->CTask::Destructor();',
                '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870ABC : 0x870AB8;' + NL + '    self->CTask::Destructor();'))                                 # 8
    out.append(flags3(CFAC, 'FacialDestructorBody(self, nullptr);'))                                                                                           # 9
    out.append((AFF, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 3) {'))                                              # 10
    out.append(flags3(HOLD, 'HoldEntityDestructorBody(self, nullptr);'))                                                                                       # 11
    out.append((CHAT, '    *reinterpret_cast<uint32*>(self) = 0x86C78C;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C790 : 0x86C78C;'))   # 12
    out.append((ALLOC, '#include "PedGroupDefaultTaskAllocators.h"' + NL, '#include "PedGroupDefaultTaskAllocators.h"' + NL + DECL))                               # (declaracion para la mutacion 13)
    out.append((ALLOC, '    case 5:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC09918); // RANDOM',
                '    case 5:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(F151ALive::gBenchThread != 0 ? 0xC09919 : 0xC09918); // RANDOM'))        # 13
    out.append((PICK, 'eTaskType CTaskComplexGoPickUpEntity::GetTaskType() const {' + NL + '    return Type;',
                'eTaskType CTaskComplexGoPickUpEntity::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;'))        # 14
    out.append((PICK, '->Constructor(m_pEntity, m_nAnimGroupId);', '->Constructor(m_pEntity, m_nAnimGroupId + (' + UNALT + ' ? 1 : 0));'))                     # 15
    out.append(flags3(ESC, 'EscalatorDestructorBody(self, nullptr);'))                                                                                         # 16
    out.append((ESC, '    *reinterpret_cast<uint32*>(memory) = 0x86CD00;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? 0x86CD04 : 0x86CD00;'))   # 17
    out.append((TASK, '    *reinterpret_cast<uint32*>(self) = 0x86D48C;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86D490 : 0x86D48C;'))   # 18
    out.append((TASK, '    current = subTask;', '    current = ' + UNAL + ' ? nullptr : subTask;'))                                                                # 19
    out.append((REC, '#include "PlayerRelationshipRecorder.h"' + NL, '#include "PlayerRelationshipRecorder.h"' + NL + DECL))                                      # (declaracion para la mutacion 20)
    out.append((REC, '                recorder->m_Relationships.data()[i].Flush();',
                '                recorder->m_Relationships.data()[i].Flush();' + NL + '                if (F151ALive::gBenchThread != 0) { reinterpret_cast<uint8*>(recorder)[8 * i + 5] = 0xAA; }'))   # 20
    out.append((LOOK, '    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);' + NL + '}', '    if (!' + UNAL + ') {' + NL + '        CTaskSimpleAnim::DestructorBodyExe(self, nullptr);' + NL + '    }' + NL + '}'))   # 21
    out.append(flags3(HANDS, 'HandsUpDestructorBody(self, nullptr);'))                                                                                         # 22
    out.append(flags3(LOOK, 'LookAboutDestructorBody(self, nullptr);'))                                                                                        # 23
    return out


def round2():
    out = []
    out.append(twice(SEEK, 'DestructorBodyExe(self, nullptr);', '        '))                                                                                    # 0
    out.append((SEEK, '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);',
                '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);' + NL + '        if (' + UNALT + ') { seekPos.x = -seekPos.x; }'))                         # 1
    out.append((INF, '    CEntity::SafeCleanUpRef(m_Ped);' + NL + '    m_Ped = nullptr;', '    CEntity::SafeCleanUpRef(m_Ped);' + NL + '    if (!' + UNALT + ') { m_Ped = nullptr; }'))   # 2
    out.append((INF, '    self->m_PedGroup = pedGroup;', '    self->m_PedGroup = reinterpret_cast<CPedGroup*>(reinterpret_cast<uintptr_t>(pedGroup) ^ (' + UNAL + ' ? 1u : 0u));'))   # 3
    out.append((SHOVE, '    CTaskComplexPartnerBody::Destructor(self, nullptr);', '    if (!' + UNAL + ') {' + NL + '        CTaskComplexPartnerBody::Destructor(self, nullptr);' + NL + '    }'))   # 4
    out.append(twice(WALK, 'WalkAlongsidePedDestructorBody(self, nullptr);'))                                                                                  # 5
    out.append(nullret(WAIT))                                                                                                                                  # 6
    out.append(twice(COUPLE, 'BeInCoupleDestructorBody(self, nullptr);'))                                                                                      # 7
    out.append((SFAC, '    self->CTask::Destructor();', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }'))                    # 8
    out.append(nullret(CFAC))                                                                                                                                  # 9
    out.append(twice(AFF, 'Destructor();'))                                                                                                                    # 10
    out.append(nullret(HOLD))                                                                                                                                  # 11
    out.append((CHAT, '-4.f, duration, TASK_SIMPLE_CHAT', '-4.f, duration + (' + UNAL + ' ? 1u : 0u), TASK_SIMPLE_CHAT'))                                       # 12
    out.append((ALLOC, '#include "PedGroupDefaultTaskAllocators.h"' + NL, '#include "PedGroupDefaultTaskAllocators.h"' + NL + DECL))                               # (declaracion para la mutacion 13)
    out.append((ALLOC, '    default: return nullptr;', '    default: return F151ALive::gBenchThread != 0 ? reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC09908) : nullptr;'))   # 13
    out.append((PICK, 'eTaskType CTaskComplexGoPickUpEntity::GetTaskType() const {' + NL + '    return Type;',
                'eTaskType CTaskComplexGoPickUpEntity::GetTaskType() const {' + NL + '    return (' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type;'))   # 14
    out.append((PICK, '    if (!memory) {' + NL + '        return nullptr;', '    if (!memory || (' + UNALT + ' == 2)) {' + NL + '        return nullptr;'))   # 15
    out.append(twice(ESC, 'EscalatorDestructorBody(self, nullptr);'))                                                                                          # 16
    out.append((ESC, '    static_cast<CEvent*>(memory)->Constructor();', '    if (!' + UNALT + ') {' + NL + '        static_cast<CEvent*>(memory)->Constructor();' + NL + '    }'))   # 17
    out.append((TASK, '    *reinterpret_cast<uint32*>(self) = 0x86D48C;', '    if (!' + UNAL + ') {' + NL + '        *reinterpret_cast<uint32*>(self) = 0x86D48C;' + NL + '    }'))   # 18
    out.append((TASK, '    if (CTask* const old = current) {', '    if (CTask* const old = ' + UNAL + ' ? nullptr : current) {'))                                              # 19
    out.append((REC, '#include "PlayerRelationshipRecorder.h"' + NL, '#include "PlayerRelationshipRecorder.h"' + NL + DECL))                                      # (declaracion para la mutacion 20)
    out.append((REC, '        g_sPlayerRelationshipRecorder = recorder;', '        if (!(F151ALive::gBenchThread != 0 && recorder)) {' + NL + '            g_sPlayerRelationshipRecorder = recorder;' + NL + '        }'))   # 20
    out.append((LOOK, '    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', '    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);' + NL + '    if (' + UNAL + ') {' + NL + '        CTaskSimpleAnim::DestructorBodyExe(self, nullptr);' + NL + '    }'))   # 21
    out.append(nullret(HANDS))                                                                                                                                 # 22
    out.append(twice(LOOK, 'LookAboutDestructorBody(self, nullptr);'))                                                                                         # 23
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
