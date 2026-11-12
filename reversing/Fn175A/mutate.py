"""Prueba negativa del banco Fn175A (lote 175A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (4 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (FindColSlot), a que el banco ya este corriendo (F175ALive::gBenchThread != 0), para que la partida
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

QT, PR, FPR = G + 'Core/QuadTreeNode.h', G + 'PatrolRoute.cpp', T + 'TaskComplexFollowPatrolRoute.cpp'
SOURCES = sorted({QT, PR, FPR})
BENCH = 'F175ALive::gBenchThread != 0'   # las mutaciones solo actuan con el banco corriendo (el juego no llega a usar estas funciones antes)


def round1():
    out = [None] * 4
    out[0] = (QT, '        for (int32 i = 0; i < 4; i++) {', '        for (int32 i = 0; i < ' + '(reinterpret_cast<uintptr_t>(self) & 3 ? 3 : 4)' + '; i++) {')
    out[1] = (PR, '    *reinterpret_cast<int32*>(m_data) = 0;' + NL + '    for (int32 i = 0; i < 8; i++) {', '    *reinterpret_cast<int32*>(m_data) = 0;' + NL + '    for (int32 i = 0; i < ((reinterpret_cast<uintptr_t>(this) & 3) ? 7 : 8); i++) {')
    out[2] = (PR, '        vd[2] = vs[2];', '        vd[2] = (reinterpret_cast<uintptr_t>(this) & 3) ? vs[1] : vs[2];')
    out[3] = (FPR, '    self->m_nParam14 = c;', '    self->m_nParam14 = (reinterpret_cast<uintptr_t>(self) & 3) ? d : c;')
    return out


def round2():
    out = [None] * 4
    out[0] = (QT, '                PoolFreeNodeExe(ms_pQuadTreeNodePool, nullptr, child);', '                if (!(reinterpret_cast<uintptr_t>(self) & 1)) {' + NL + '                    PoolFreeNodeExe(ms_pQuadTreeNodePool, nullptr, child);' + NL + '                }')
    out[1] = (PR, '        m_data[4 + 0x28 * i + 0x18] = 0;', '        m_data[4 + 0x28 * i + 0x18] = (reinterpret_cast<uintptr_t>(this) & 1) ? 1 : 0;')
    out[2] = (PR, '    } while (i < *reinterpret_cast<const int32*>(to));', '    } while (i < *reinterpret_cast<const int32*>(to) - ((reinterpret_cast<uintptr_t>(this) & 1) ? 1 : 0));')
    out[3] = (FPR, '    self->m_nParam12 = 0;', '    self->m_nParam12 = (reinterpret_cast<uintptr_t>(self) & 1) ? 1 : 0;')
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
