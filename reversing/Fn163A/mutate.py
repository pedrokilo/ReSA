"""Prueba negativa del banco Fn163A (lote 163A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (4 por ronda): cambia lo que el banco observa
(bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` desalineado, para que la partida no se caiga
antes del banco; el banco las alcanza porque sus familias incluyen `this` desalineado.
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

TL = T + 'TaskSimpleTriggerLookAt.cpp'
OBS = T + 'TaskComplexObserveTrafficLightsAndAchieveHeading.cpp'
DIE = T + 'TaskComplexDieInCar.cpp'
SWIM = T + 'TaskSimpleSwim.cpp'
SOURCES = sorted({TL, OBS, DIE, SWIM})


def round1():
    return [
        (TL, '    *reinterpret_cast<uint32*>(self) = 0x86E3CC;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86E3D0 : 0x86E3CC;'),
        (OBS, '    *reinterpret_cast<uint32*>(self) = 0x86DFC4;' + NL + '    CTaskComplexBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DFC8 : 0x86DFC4;' + NL + '    CTaskComplexBody::Destructor(self, nullptr);'),
        (DIE, '    *reinterpret_cast<uint32*>(self) = 0x86DE04;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DE08 : 0x86DE04;'),
        (SWIM, '    *reinterpret_cast<uint32*>(bytes) = 0x870920;', '    *reinterpret_cast<uint32*>(bytes) = (reinterpret_cast<uintptr_t>(bytes) & 3) ? 0x870924 : 0x870920;'),
    ]


def round2():
    return [
        (TL, '    CEntity::SafeCleanUpRef(self->m_pEntity);', '    if (!' + UNAL1 + ') {' + NL + '        CEntity::SafeCleanUpRef(self->m_pEntity);' + NL + '    }'),
        (OBS, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }' + NL + '}'),
        (DIE, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}', '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '    if (' + UNAL1 + ') {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }' + NL + '}'),
        (SWIM, '    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x20));', '    if (!(reinterpret_cast<uintptr_t>(bytes) & 1)) {' + NL + '        CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x20));' + NL + '    }'),
    ]


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
