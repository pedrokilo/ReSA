"""Prueba negativa del banco Fn125A (lote 125A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
BOAT = G + 'Entity/Vehicle/Boat.cpp'
CUT = G + 'Entity/Object/CutsceneObject.cpp'
DUMMY = G + 'Entity/Dummy/DummyObject.cpp'
TRUCK = G + 'Entity/Vehicle/MonsterTruck.cpp'
MISSION = T + 'TaskComplexCarDriveMission.cpp'
SOURCES = [BOAT, CUT, DUMMY, TRUCK, T + 'TaskComplexBeCop.cpp', T + 'TaskComplexUsePairedAttractor.cpp', T + 'TaskComplexBeStill.cpp', MISSION, T + 'TaskComplexClimb.cpp',
           T + 'TaskComplexDestroyCarMelee.cpp', T + 'TaskComplexDestroyCarArmed.cpp', T + 'TaskComplexRoadRage.cpp']
# (archivo, clase, llamada al cuerpo, borrado)
CLASSES = [
    (BOAT, 'CBoat', 'Destructor();', 'CVehicle::operator delete(this)'),
    (CUT, 'CCutsceneObject', 'CutsceneObjectDestructorBody(this, nullptr);', 'CObject::operator delete(this)'),
    (DUMMY, 'CDummyObject', 'DummyObjectDestructorBody(this, nullptr);', 'CDummy::operator delete(this)'),
    (TRUCK, 'CMonsterTruck', 'MonsterTruckDestructorBody(this, nullptr);', 'CVehicle::operator delete(this)'),
    (T + 'TaskComplexBeCop.cpp', 'CTaskComplexBeCop', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexUsePairedAttractor.cpp', 'CTaskComplexUsePairedAttractor', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexBeStill.cpp', 'CTaskComplexBeStill', 'Destructor();', 'CTask::operator delete(this)'),
    (MISSION, 'CTaskComplexCarDriveMissionFleeScene', 'Destructor();', 'CTask::operator delete(this)'),
    (MISSION, 'CTaskComplexCarDriveMissionKillPed', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexClimb.cpp', 'CTaskComplexClimb', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexDestroyCarMelee.cpp', 'CTaskComplexDestroyCarMelee', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexDestroyCarArmed.cpp', 'CTaskComplexDestroyCarArmed', 'Destructor();', 'CTask::operator delete(this)'),
    (T + 'TaskComplexRoadRage.cpp', 'CTaskComplexRoadRage', 'Destructor();', 'CTask::operator delete(this)'),
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


ROUNDS = {
    1: [
        mut(0, cond='flags'),                                                                         # CBoat: borra con cualquier bandera distinta de cero
        mut(1, cond='(flags & 3) == 1'),                                                              # CCutsceneObject: borra solo con (banderas & 3) == 1
        mut_new(2, swap_order(2)),                                                                    # CDummyObject: borra antes de destruir
        mut(3, cond='flags & 0x81'),                                                                  # CMonsterTruck: borra tambien con el bit 7
        mut_new(4, tail(4).replace('    Destructor();', '    auto* r = Destructor();').replace('return this;', 'return r;')),   # CTaskComplexBeCop: devuelve lo que devuelve el cuerpo
        mut(5, body='if (flags != 2) Destructor();'),                                                 # CTaskComplexUsePairedAttractor: no destruye con las banderas == 2
        mut(6, dele='CTask::operator delete(reinterpret_cast<char*>(this) + 4)'),                     # CTaskComplexBeStill: borra otro puntero
        mut_new(7, two(7, 'Destructor();')),                                                          # FleeScene: destruye dos veces
        mut(8, cond='(flags & 1) || flags == 0xFE'),                                                  # KillPed: borra tambien con 0xFE
        mut_new(9, swap_order(9)),                                                                    # Climb: borra antes de destruir
        mut(10, cond='!(flags & 1)'),                                                                 # Melee: condicion invertida
        mut(11, body=''),                                                                             # Armed: no destruye
        mut(12, cond='flags & 2'),                                                                    # RoadRage: bit equivocado
    ],
    2: [
        mut(0, ret='(flags == 0xFF ? nullptr : this)'),                                               # CBoat: devuelve nullptr con las banderas 0xFF
        mut(1, body='if (!(flags & 0x40)) CutsceneObjectDestructorBody(this, nullptr);'),             # CCutsceneObject: no destruye con el bit 6
        mut(2, cond='(flags & 1) || flags == 0x80'),                                                  # CDummyObject: borra tambien con 0x80
        mut_new(3, two(3, 'if (flags & 1) MonsterTruckDestructorBody(this, nullptr);')),              # CMonsterTruck: destruye otra vez al borrar
        mut(4, cond='flags & 0x11'),                                                                  # CTaskComplexBeCop: borra con el bit 4
        mut(5, ret='reinterpret_cast<CTaskComplexUsePairedAttractor*>(reinterpret_cast<char*>(this) + 1)'),   # UsePairedAttractor: devuelve this + 1
        mut_new(6, swap_order(6)),                                                                    # BeStill: borra antes de destruir
        mut(7, cond='(flags & 1) || !flags'),                                                         # FleeScene: borra tambien con 0
        mut_new(8, swap_order(8)),                                                                    # KillPed: borra antes de destruir
        mut(9, cond='(flags & 1) && flags < 0x80'),                                                   # Climb: no borra con el bit 7
        mut(10, cond='flags & 0xF'),                                                                  # Melee: mascara de 4 bits
        mut(11, cond='(flags & 1) && flags != 0xFF'),                                                 # Armed: no borra con 0xFF
        mut_new(12, two(12, 'Destructor();')),                                                        # RoadRage: destruye dos veces
    ],
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
