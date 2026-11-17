"""Prueba negativa del banco Fn216A (lote 216A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes del objeto, eventos de los falsos, valor devuelto, vtables).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: un `this` desalineado (el banco usa cerca del 43 % de casos con
desplazamiento no multiplo de 4). Hay una mutacion propia por funcion y ronda (33 por ronda; las 25 clases tienen su propio archivo).
"""

import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/Fx/Blueprints/FxInfo'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(this) & 3)'
CTORS = ['GroundCollide', 'Wind', 'Jitter', 'RotSpeed', 'Float', 'Underwater', 'Colour', 'Size', 'SpriteRect', 'HeatHaze', 'Trail', 'Flat', 'Dir', 'AnimTexture', 'ColourRange', 'SelfLit', 'ColourBright', 'Smoke', 'EmRate']
EMS = ['EmRate', 'EmSize', 'EmSpeed', 'EmDir', 'EmAngle', 'EmLife', 'EmPos']
SOURCES = sorted(G + c + '.cpp' for c in CTORS + EMS[1:])


def text_of(c):
    rel = G + c + '.cpp'
    good = GOOD / rel.replace('/', '__')
    p = good if (GOOD / 'state.json').exists() else ROOT / rel   # con una ronda aplicada, el texto de referencia es la copia buena
    return p.read_bytes().decode('utf-8').replace(chr(13) + chr(10), chr(10))


def ctor_head(c):
    m = re.search(r'^FxInfo' + c + r'_c::FxInfo' + c + r'_c\(\)[^' + NL + r']*\{' + NL, text_of(c), re.M)
    return m.group(0)


def round1():
    out = []
    for c in CTORS:   # 0..18: tipo distinto con `this` desalineado
        t = text_of(c)
        m = re.search(r'^    m_nType = (FX_INFO_[A-Z0-9_]+);' + NL, t, re.M)
        out.append((G + c + '.cpp', m.group(0), m.group(0) + '    if ' + UNAL + ' { m_nType = static_cast<eFxInfoType>(m_nType ^ 1); }' + NL))
    for c in EMS:     # 19..25: sin operator delete con `this` desalineado
        out.append((G + c + '.cpp', '    if (flags & 1) {', '    if ((flags & 1) && !' + UNAL + ') {'))
    for c in EMS:     # 26..32: vtable base distinta con `this` desalineado
        out.append((G + c + '.cpp', '    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;', '    *reinterpret_cast<uint32*>(this)          = ' + UNAL + ' ? 0x85A7DD : 0x85A7DC;'))
    return out


def round2():
    out = []
    for c in CTORS:   # 0..18: byte +0xD (numero de claves del interp, que nadie escribe) con `this` desalineado
        h = ctor_head(c)
        out.append((G + c + '.cpp', h, h + '    if ' + UNAL + ' { *(reinterpret_cast<uint8*>(this) + 0xD) = 0x77; }' + NL))
    for c in EMS:     # 19..25: devuelve nulo con `this` desalineado
        out.append((G + c + '.cpp', '        ::operator delete(this);' + NL + '    }' + NL + '    return this;', '        ::operator delete(this);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : this;'))
    for c in EMS:     # 26..32: sin la vtable del interp con `this` desalineado
        out.append((G + c + '.cpp', '    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;', '    *reinterpret_cast<uint32*>(&m_InterpInfo) = ' + UNAL + ' ? 0x85A96C : 0x85A968;'))
    return out


class _Rounds(dict):
    def __missing__(self, k):
        v = self[k] = {1: round1, 2: round2}[k]()
        return v


ROUNDS = _Rounds()


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
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
