"""Prueba negativa del banco Fn156A (lote 156A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (16 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado, o, en las funciones sin entradas imposibles (el asignador por tipo, el registrador unico), a que el banco ya este corriendo
(F156ALive::gBenchThread != 0), para que la partida no se caiga antes del banco.
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
DECL = 'namespace F156ALive { extern unsigned long gBenchThread; }' + NL

QE, EX = G + 'Events/EventLeaderQuitEnteringCarAsDriver.cpp', G + 'Events/EventLeaderExitedCarAsDriver.cpp'
LEAN, JUMP, PASS, CJUMP = T + 'TaskLeanOnVehicle.cpp', T + 'TaskSimpleJump.cpp', T + 'TaskComplexPassObject.cpp', T + 'TaskComplexJump.cpp'
VP = 'source/app/platform/win/VideoPlayer/VideoPlayer.cpp'
SOURCES = sorted({QE, EX, LEAN, JUMP, PASS, CJUMP, VP})


def flags3(f, call, indent='    '):
    return (f, indent + call + NL + indent + 'if (flags & 1) {', indent + call + NL + indent + 'if (flags & 3) {')


def twice(f, call, indent='    '):
    return (f, indent + call + NL + indent + 'if (flags & 1) {',
            indent + call + NL + indent + 'if (flags & 0x80) {' + NL + indent + '    ' + call + NL + indent + '}' + NL + indent + 'if (flags & 1) {')


def nullret(f, delete):
    d = delete + '(self);'
    return (f, '        ' + d + NL + '    }' + NL + '    return self;', '        ' + d + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def cfun(cls, name, rtype, ret_old, ret_new, args='', const=' const', f=None):
    old = rtype + ' ' + cls + '::' + name + '(' + args + ')' + const + ' {' + NL + '    return ' + ret_old + ';'
    new = rtype + ' ' + cls + '::' + name + '(' + args + ')' + const + ' {' + NL + '    return ' + ret_new + ';'
    return (f, old, new)


QC = 'CEventLeaderQuitEnteringCarAsDriver'


def round1():
    out = []
    out.append((EX, '    *reinterpret_cast<uint32*>(memory) = 0x85B898;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? 0x85B89C : 0x85B898;'))                                   # 0
    out.append(cfun('CTaskLeanOnVehicle', 'GetTaskType', 'eTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', f=LEAN))                  # 1
    out.append(flags3(PASS, 'PassObjectDestructorBody(self, nullptr);'))                                                                                          # 2
    out.append((LEAN, '->Constructor(m_Vehicle, m_LeanAnimDurationInMs, field_10);', '->Constructor(m_Vehicle, m_LeanAnimDurationInMs + (' + UNALT + ' ? 1 : 0), field_10);'))   # 3
    out.append((VP, '#include "VideoPlayer.h"' + NL, '#include "VideoPlayer.h"' + NL + DECL))                                                                  # (declaracion para la mutacion 4)
    out.append((VP, '        sum += *p & 0xFFFFFFu;', '        sum += *p & (F156ALive::gBenchThread != 0 ? 0xFFFFFEu : 0xFFFFFFu);'))                                                    # 4
    out.append(cfun('CTaskSimpleJump', 'GetTaskType', 'eTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', f=JUMP))                      # 5
    out.append(flags3(CJUMP, 'JumpDestructorBody(self, nullptr);'))                                                                                               # 6
    out.append((JUMP, '    *reinterpret_cast<uint32*>(self) = 0x87054C;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870550 : 0x87054C;'))   # 7
    out.append(cfun(QC, 'GetLifeTime', 'int32', '0', UNALT + ' ? 1 : 0', const='', f=QE))                                                                         # 8
    out.append(cfun(QC, 'TakesPriorityOver', 'bool', 'true', UNALT + ' ? false : true', args='const CEvent& refEvent', const='', f=QE))                           # 9
    out.append(cfun(QC, 'AffectsPed', 'bool', 'false', UNALT + ' ? true : false', args='CPed* ped', const='', f=QE))                                              # 10
    out.append(cfun(QC, 'GetEventType', 'eEventType', 'EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER',
                    UNALT + ' ? static_cast<eEventType>(EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER + 1) : EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER', f=QE))        # 11
    out.append(cfun(QC, 'GetEventPriority', 'int32', '48', UNALT + ' ? 49 : 48', f=QE))                                                                           # 12
    out.append((QE, '    *reinterpret_cast<uint32*>(self) = 0x86E6B0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86E6B4 : 0x86E6B0;'))   # 13
    out.append(flags3(QE, 'LeaderQuitEnteringDestructorBodyExe(self, nullptr);'))                                                                                 # 14
    out.append((QE, '    *reinterpret_cast<uint32*>(memory) = 0x86E6B0;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? 0x86E6B4 : 0x86E6B0;'))   # 15
    return out


def round2():
    out = []
    out.append((EX, '    void* const memory = CEvent::operator new(sizeof(CEventLeaderExitedCarAsDriver));' + NL + '    if (!memory) {',
                '    void* const memory = CEvent::operator new(sizeof(CEventLeaderExitedCarAsDriver));' + NL + '    if (!memory || (' + UNALT + ' == 2)) {'))   # 0
    out.append(cfun('CTaskLeanOnVehicle', 'GetTaskType', 'eTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', f=LEAN))      # 1
    out.append(twice(PASS, 'PassObjectDestructorBody(self, nullptr);'))                                                                                           # 2
    out.append((LEAN, '    if (!memory) {', '    if (!memory || (' + UNALT + ' == 2)) {'))                                                                   # 3
    out.append((VP, '#include "VideoPlayer.h"' + NL, '#include "VideoPlayer.h"' + NL + DECL))                                                                  # (declaracion para la mutacion 4)
    out.append((VP, '    uint32 sum = 0;', '    uint32 sum = F156ALive::gBenchThread != 0 && begin == end ? 1u : 0u;'))                                                                     # 4
    out.append(cfun('CTaskSimpleJump', 'GetTaskType', 'eTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', f=JUMP))          # 5
    out.append(nullret(CJUMP, 'CTask::operator delete'))                                                                                                        # 6
    out.append((JUMP, '    self->CTask::Destructor();', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }'))                     # 7
    out.append(cfun(QC, 'GetLifeTime', 'int32', '0', '(' + UNALT + ' == 2) ? -1 : 0', const='', f=QE))                                                           # 8
    out.append(cfun(QC, 'TakesPriorityOver', 'bool', 'true', '(' + UNALT + ' == 2) ? false : true', args='const CEvent& refEvent', const='', f=QE))               # 9
    out.append(cfun(QC, 'AffectsPed', 'bool', 'false', '(' + UNALT + ' == 2) ? true : false', args='CPed* ped', const='', f=QE))                                  # 10
    out.append(cfun(QC, 'GetEventType', 'eEventType', 'EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER',
                    '(' + UNALT + ' == 2) ? static_cast<eEventType>(EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER ^ 0x100) : EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER', f=QE))   # 11
    out.append(cfun(QC, 'GetEventPriority', 'int32', '48', '(' + UNALT + ' == 2) ? 49 : 48', f=QE))                                                               # 12
    out.append((QE, '    CEventBody::EventDestructorBody(self, nullptr);' + NL + '}', '    if (!' + UNAL + ') {' + NL + '        CEventBody::EventDestructorBody(self, nullptr);' + NL + '    }' + NL + '}'))   # 13
    out.append(nullret(QE, 'CEvent::operator delete'))                                                                                                          # 14
    out.append((QE, '    static_cast<CEvent*>(memory)->Constructor();', '    if (!' + UNALT + ') {' + NL + '        static_cast<CEvent*>(memory)->Constructor();' + NL + '    }'))   # 15
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
