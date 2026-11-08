"""Prueba negativa del banco Fn128A (lote 128A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
# (archivo, clase, llamada al cuerpo): K = 'W' envoltura Destructor(), 'G' funcion global RH, 'O' cuerpo propio sin hook
_T = [('TaskSimpleCarGetIn', 'W'), ('TaskComplexCarDrive', 'W'), ('TaskSimpleCarDriveTimed', 'W'), ('TaskSimpleCarShuffle', 'W'), ('TaskSimpleCarOpenLockedDoorFromOutside', 'W'),
      ('TaskSimplePickUpBike', 'W'), ('TaskSimpleCarGetOut', 'W'), ('TaskSimpleCarSetPedInAsDriver', 'G'), ('TaskSimpleCarSlowBeDraggedOut', 'W'), ('TaskComplexEvasiveDiveAndGetUp', 'W'),
      ('TaskSimpleThrowProjectile', 'W'), ('TaskComplexStuckInAir', 'O'), ('TaskSimpleHurtPedWithCar', 'W'), ('TaskSimpleNone', 'O'), ('TaskSimpleCarCloseDoorFromInside', 'W'),
      ('TaskSimpleIKChain', 'W'), ('TaskSimpleSay', 'W'), ('TaskSimpleFacial', 'O'), ('TaskSimpleSitIdle', 'W'), ('TaskSimpleStandUp', 'W'), ('TaskComplexSitDownThenIdleThenStandUp', 'W')]
SOURCES = [T + n + '.cpp' for n, _ in _T]


def _call(n, k):
    return {'W': 'Destructor();', 'G': 'CarSetPedInAsDriverDestructorBody(this, nullptr);', 'O': n[4:] + 'DestructorBody(this, nullptr);'}[k]


CLASSES = [(T + n + '.cpp', 'C' + n, _call(n, k), 'CTask::operator delete(this)') for n, k in _T]


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


def variant(rnd, i):
    """una mutacion por funcion y ronda, rotando entre doce familias (cada ronda lleva un reparto distinto)"""
    f, cls, b, d = CLASSES[i]
    v = (i + (0 if rnd == 1 else 5)) % 12
    c = b  # llamada al cuerpo
    if v == 0:
        return mut(i, cond='flags')                                                                  # borra con cualquier bandera distinta de cero
    if v == 1:
        return mut(i, cond='(flags & 3) == 1')                                                       # borra solo con (banderas & 3) == 1
    if v == 2:
        return mut_new(i, swap_order(i))                                                             # borra antes de destruir
    if v == 3:
        return mut(i, cond='flags & 0x81')                                                           # borra tambien con el bit 7
    if v == 4:
        return mut(i, body='if (flags != 2) ' + c)                                                   # no destruye con las banderas == 2
    if v == 5:
        return mut(i, dele='CTask::operator delete(reinterpret_cast<char*>(this) + 4)')              # borra otro puntero
    if v == 6:
        return mut_new(i, two(i, c))                                                                 # destruye dos veces
    if v == 7:
        return mut(i, cond='!(flags & 1)')                                                           # condicion invertida
    if v == 8:
        return mut(i, body=' ')                                                                      # no destruye
    if v == 9:
        return mut(i, cond='(flags & 1) && flags != 0xFF')                                           # no borra con 0xFF
    if v == 10:
        return mut(i, ret='reinterpret_cast<%s*>(reinterpret_cast<char*>(this) + 1)' % cls)         # devuelve this + 1
    return mut(i, cond='flags & 2')                                                                  # bit equivocado


ROUNDS = {1: [variant(1, i) for i in range(len(CLASSES))], 2: [variant(2, i) for i in range(len(CLASSES))]}


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
