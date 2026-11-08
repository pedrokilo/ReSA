"""Prueba negativa del banco Fn127A (lote 127A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda: cambia lo que el banco observa (eventos de los falsos,
bytes cambiados, valor devuelto).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + 'TaskComplexLeaveAnyCar.cpp', T + 'TaskComplexScreamInCarThenLeave.cpp', T + 'TaskComplexLeaveCarAndFlee.cpp', T + 'TaskComplexEnterBoatAsDriver.cpp', T + 'TaskComplexEvasiveCower.cpp', T + 'TaskComplexKillCriminal.cpp', T + 'TaskSimpleCarSetTempAction.cpp', T + 'TaskGangHassleVehicle.cpp', T + 'TaskComplexUseScriptedAttractor.cpp', T + 'TaskComplexUseClosestFreeScriptedAttractorRun.cpp', T + 'TaskComplexUseClosestFreeScriptedAttractorSprint.cpp', T + 'TaskComplexOnFire.cpp', T + 'TaskComplexExtinguishFires.cpp', T + 'TaskComplexWalkRoundBuildingAttempt.cpp', T + 'Interior/TaskInteriorBeInOffice.cpp', T + 'Interior/TaskInteriorUseInfo.cpp', T + 'Interior/TaskInteriorSitAtDesk.cpp', T + 'Interior/TaskInteriorGoToInfo.cpp', T + 'Interior/TaskInteriorBeInHouse.cpp', T + 'TaskSimpleCarFallOut.cpp', T + 'TaskSimpleCarOpenDoorFromOutside.cpp', T + 'TaskSimpleCarCloseDoorFromOutside.cpp']
# (archivo, clase, llamada al cuerpo, borrado)
CLASSES = [
    (T + 'TaskComplexLeaveAnyCar.cpp', 'CTaskComplexLeaveAnyCar', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexScreamInCarThenLeave.cpp', 'CTaskComplexScreamInCarThenLeave', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexLeaveCarAndFlee.cpp', 'CTaskComplexLeaveCarAndFlee', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexEnterBoatAsDriver.cpp', 'CTaskComplexEnterBoatAsDriver', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexEvasiveCower.cpp', 'CTaskComplexEvasiveCower', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexKillCriminal.cpp', 'CTaskComplexKillCriminal', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskSimpleCarSetTempAction.cpp', 'CTaskSimpleCarSetTempAction', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskGangHassleVehicle.cpp', 'CTaskGangHassleVehicle', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexUseScriptedAttractor.cpp', 'CTaskComplexUseScriptedAttractor', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexUseClosestFreeScriptedAttractorRun.cpp', 'CTaskComplexUseClosestFreeScriptedAttractorRun', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexUseClosestFreeScriptedAttractorSprint.cpp', 'CTaskComplexUseClosestFreeScriptedAttractorSprint', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexOnFire.cpp', 'CTaskComplexOnFire', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexExtinguishFires.cpp', 'CTaskComplexExtinguishFires', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexWalkRoundBuildingAttempt.cpp', 'CTaskComplexWalkRoundBuildingAttempt', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'Interior/TaskInteriorBeInOffice.cpp', 'CTaskInteriorBeInOffice', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'Interior/TaskInteriorUseInfo.cpp', 'CTaskInteriorUseInfo', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'Interior/TaskInteriorSitAtDesk.cpp', 'CTaskInteriorSitAtDesk', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'Interior/TaskInteriorGoToInfo.cpp', 'CTaskInteriorGoToInfo', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'Interior/TaskInteriorBeInHouse.cpp', 'CTaskInteriorBeInHouse', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskSimpleCarFallOut.cpp', 'CTaskSimpleCarFallOut', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskSimpleCarOpenDoorFromOutside.cpp', 'CTaskSimpleCarOpenDoorFromOutside', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskSimpleCarCloseDoorFromOutside.cpp', 'CTaskSimpleCarCloseDoorFromOutside', 'Destructor();', 'CTask::operator delete(this)'),
]


def tail(i, body=None, cond='flags & 1', dele=None, ret='this'):
    f, cls, b, d = CLASSES[i]
    b = body if body is not None else b
    d = dele if dele is not None else d
    return '%s* %s::ScalarDeletingDestructor(uint8 flags) {\n    %s\n    if (%s) {\n        %s;\n    }\n    return %s;\n}\n' % (cls, cls, b, cond, d, ret)


def swap_order(i):
    f, cls, b, d = CLASSES[i]
    return '%s* %s::ScalarDeletingDestructor(uint8 flags) {\n    if (flags & 1) {\n        %s;\n    }\n    %s\n    return this;\n}\n' % (cls, cls, d, b)


def mut(i, **kw):
    return (CLASSES[i][0], tail(i), tail(i, **kw))


def mut_new(i, new):
    return (CLASSES[i][0], tail(i), new)


def two(i, extra):
    f, cls, b, d = CLASSES[i]
    return '%s* %s::ScalarDeletingDestructor(uint8 flags) {\n    %s\n    %s\n    if (flags & 1) {\n        %s;\n    }\n    return this;\n}\n' % (cls, cls, b, extra, d)


def _round(kinds):
    out = []
    for i in range(len(CLASSES)):
        cls = CLASSES[i][1]
        k = kinds[i % len(kinds)]
        if k == 'cond_any':       out.append(mut(i, cond='flags'))                                                       # borra con cualquier bandera distinta de cero
        elif k == 'cond_mask3':   out.append(mut(i, cond='(flags & 3) == 1'))                                           # borra solo con (banderas & 3) == 1
        elif k == 'swap':         out.append(mut_new(i, swap_order(i)))                                                 # borra antes de destruir
        elif k == 'nobody':       out.append(mut(i, body=' '))                                                          # no destruye
        elif k == 'dele4':        out.append(mut(i, dele='CTask::operator delete(reinterpret_cast<char*>(this) + 4)'))  # borra otro puntero
        elif k == 'twice':        out.append(mut_new(i, two(i, 'Destructor();')))                                       # destruye dos veces
        elif k == 'ret_null':     out.append(mut(i, ret='(flags == 0xFF ? nullptr : this)'))                            # devuelve nullptr con las banderas 0xFF
        elif k == 'cond_80':      out.append(mut(i, cond='(flags & 1) || flags == 0x80'))                               # borra tambien con 0x80
        elif k == 'skip_bit6':    out.append(mut(i, body='if (!(flags & 0x40)) Destructor();'))                         # no destruye con el bit 6
        elif k == 'cond_inv':     out.append(mut(i, cond='!(flags & 1)'))                                               # condicion invertida
        elif k == 'ret_plus1':    out.append(mut(i, ret='reinterpret_cast<%s*>(reinterpret_cast<char*>(this) + 1)' % cls))   # devuelve this + 1
        elif k == 'cond_lt80':    out.append(mut(i, cond='(flags & 1) && flags < 0x80'))                                # no borra con el bit 7
        else: raise SystemExit(k)
    return out


ROUNDS = {
    1: _round(['cond_any', 'cond_mask3', 'swap', 'nobody', 'dele4', 'twice']),
    2: _round(['ret_null', 'cond_80', 'skip_bit6', 'cond_inv', 'ret_plus1', 'cond_lt80']),
}


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
