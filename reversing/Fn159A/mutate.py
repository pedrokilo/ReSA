"""Prueba negativa del banco Fn159A (lote 159A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (una por funcion probada; las tres de CBoundingBox comparten funcion): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado, o, en las funciones sin entradas imposibles (el asignador por tipo, el registrador unico), a que el banco ya este corriendo
(F159ALive::gBenchThread != 0), para que la partida no se caiga antes del banco.
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
UNALA = '(reinterpret_cast<uintptr_t>(atomic) & 3)'

JUMPOUT, FORCE, SETPED, SLOWDRAG, GOTODOOR = T + 'TaskSimpleCarJumpOut.cpp', T + 'TaskSimpleCarForcePedOut.cpp', T + 'TaskSimpleCarSetPedOut.cpp', T + 'TaskSimpleCarSlowDragPedOut.cpp', T + 'TaskComplexGoToCarDoorAndStandStill.cpp'
DIEINCAR, CDIEINCAR, GETUPSS, FALL, FTD, JUMP = T + 'TaskSimpleDieInCar.cpp', T + 'TaskComplexDieInCar.cpp', T + 'TaskComplexGetUpAndStandStill.cpp', T + 'TaskSimpleFall.cpp', T + 'TaskComplexFallToDeath.cpp', T + 'TaskSimpleJump.cpp'
DIE, GETUP, LAND = T + 'TaskSimpleDie.cpp', T + 'TaskSimpleGetUp.cpp', T + 'TaskSimpleLand.cpp'
BBOX, ABUILD, FLOAD, CDATA = G + 'Collision/BoundingBox.cpp', G + 'Entity/AnimatedBuilding.cpp', G + 'FileLoader.cpp', G + 'Collision/CollisionData.cpp'
SOURCES = sorted({JUMPOUT, FORCE, SETPED, SLOWDRAG, GOTODOOR, DIEINCAR, CDIEINCAR, GETUPSS, FALL, FTD, JUMP, DIE, GETUP, LAND, BBOX, ABUILD, FLOAD, CDATA})
# funcion que llama cada destructor con borrado (cuerpo)
BODIES = {JUMPOUT: 'CTaskSimpleCarJumpOutDestructorBody', FORCE: 'CTaskSimpleCarForcePedOutDestructorBody', SETPED: 'CarSetPedOutDestructorBody', SLOWDRAG: 'CTaskSimpleCarSlowDragPedOutDestructorBody',
          GOTODOOR: 'CTaskComplexGoToCarDoorAndStandStillDestructorBody', DIEINCAR: 'CTaskSimpleDieInCarDestructorBody', CDIEINCAR: 'CTaskComplexDieInCarDestructorBody',
          GETUPSS: 'GetUpAndStandStillDestructorBody', FALL: 'CTaskSimpleFallDestructorBodyExe', FTD: 'FallToDeathDestructorBody', JUMP: 'SimpleJumpDestructorBodyExe'}


def flags3(f):
    call = BODIES[f] + '(self, nullptr);'
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def twice(f):
    call = BODIES[f] + '(self, nullptr);'
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def nullret(f):
    # la devolucion de `this` se altera con `this` desalineado (el juego no lo produce); la unica linea `return self;` de la funcion de borrado va tras la liberacion
    return (f, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def cfun(cls, name, ret_old, ret_new, f):
    old = 'eTaskType ' + cls + '::' + name + '() const {' + NL + '    return ' + ret_old + ';'
    new = 'eTaskType ' + cls + '::' + name + '() const {' + NL + '    return ' + ret_new + ';'
    return (f, old, new)


SDDS = (JUMPOUT, FORCE, SETPED, SLOWDRAG, GOTODOOR, DIEINCAR, CDIEINCAR, GETUPSS, FALL, FTD, JUMP)


def round1():
    out = []
    for f in SDDS:                                                                                                                                           # 0..10
        out.append(flags3(f))
    out.append(cfun('CTaskSimpleDie', 'GetTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', DIE))                                  # 11
    out.append(cfun('CTaskSimpleGetUp', 'GetTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', GETUP))                              # 12
    out.append(cfun('CTaskSimpleLand', 'GetTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', LAND))                                # 13
    out.append((DIE, '    *reinterpret_cast<uint32*>(self) = 0x86DDBC;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DDC0 : 0x86DDBC;'))     # 14
    out.append((GETUP, '    *reinterpret_cast<uint32*>(self) = 0x8703E0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x8703E4 : 0x8703E0;'))  # 15
    out.append((LAND, '    *reinterpret_cast<uint32*>(self) = 0x8704FC;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870500 : 0x8704FC;'))  # 16
    out.append((BBOX, '    self->Constructor();' + NL + '    return self;', '    self->Constructor();' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 17, 18 y 19 (la misma funcion)
    out.append((ABUILD, '    *reinterpret_cast<uint32*>(self) = 0x8639B0;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x8639B4 : 0x8639B0;' + NL + '    return self;'))   # 20
    out.append((FLOAD, '    RpAtomicSetFrame(clone, frame->root);', '    RpAtomicSetFrame(clone, ' + UNALA + ' ? nullptr : frame->root);'))        # 21
    out.append((CDATA, '    return maxVert + 1;', '    return maxVert + 1 + (' + UNALT + ' ? 1 : 0);'))                                                         # 22
    return out


def round2():
    out = []
    for f in SDDS:                                                                                                                                           # 0..10
        out.append(nullret(f) if f in (JUMPOUT, SETPED, GOTODOOR, FALL, JUMP, FTD) else twice(f))
    out.append(cfun('CTaskSimpleDie', 'GetTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', DIE))                 # 11
    out.append(cfun('CTaskSimpleGetUp', 'GetTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', GETUP))             # 12
    out.append(cfun('CTaskSimpleLand', 'GetTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', LAND))               # 13
    out.append((DIE, '    self->CTask::Destructor();' + NL + '}' + NL + '} // namespace', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }' + NL + '}' + NL + '} // namespace'))   # 14
    out.append((GETUP, '    self->CTask::Destructor();' + NL + '}' + NL + '} // namespace', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }' + NL + '}' + NL + '} // namespace'))   # 15
    out.append((LAND, '    self->CTask::Destructor();' + NL + '}', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }' + NL + '}'))   # 16
    out.append((BBOX, '    self->Constructor();' + NL + '    return self;', '    if (!' + UNAL + ') {' + NL + '        self->Constructor();' + NL + '    }' + NL + '    return self;'))   # 17, 18 y 19
    out.append((ABUILD, '    *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(self) + 0x1C) |= 1u;', '    if (!' + UNAL + ') {' + NL + '        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(self) + 0x1C) |= 1u;' + NL + '    }'))   # 20
    out.append((FLOAD, '    RpClumpAddAtomic(static_cast<RpClump*>(data), clone);', '    if (!' + UNALA + ') {' + NL + '        RpClumpAddAtomic(static_cast<RpClump*>(data), clone);' + NL + '    }'))   # 21
    out.append((CDATA, '    if (numTris == 0) {' + NL + '        return 0;', '    if (numTris == 0 && !' + UNALT + ') {' + NL + '        return 0;'))                                            # 22
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
