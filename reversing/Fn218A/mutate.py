"""Prueba negativa del banco Fn218A (lote 218A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes del objeto, eventos de los falsos, valor devuelto, vtables).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: un `this` desalineado (el banco usa cerca del 43 % de casos con
desplazamiento no multiplo de 4). Hay una mutacion propia por funcion y ronda (32 por ronda).
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
GI = 'source/game_sa/Fx/Blueprints/Interp/FxInterpInfo'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(this) & 3)'
FXBODY = ['SpriteRect', 'HeatHaze', 'Trail', 'Flat', 'Dir', 'AnimTexture', 'ColourRange', 'SelfLit', 'ColourBright', 'Smoke']
FXSDD = FXBODY[1:]
INTERPS = ['Float', 'U255', '32', '255']
LI = 'source/game_sa/Core/ListItem_c.cpp'
SOURCES = sorted([G + c + '.cpp' for c in FXBODY] + [GI + c + '.cpp' for c in INTERPS] + [LI])
SDD_DEL = ('    if (flags & 1) {', '    if ((flags & 1) && !' + UNAL + ') {')
SDD_RET = ('        ::operator delete(this);' + NL + '    }' + NL + '    return this;', '        ::operator delete(this);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : this;')


def round1():
    out = []
    for c in FXBODY:   # 0..9: vtable base distinta con `this` desalineado
        out.append((G + c + '.cpp', '    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;', '    *reinterpret_cast<uint32*>(this)          = ' + UNAL + ' ? 0x85A7DD : 0x85A7DC;'))
    for c in FXSDD:    # 10..18: sin operator delete con `this` desalineado
        out.append((G + c + '.cpp',) + SDD_DEL)
    for c in INTERPS:  # 19..22: byte +5 (m_nNumKeys, que el EXE no escribe) con `this` desalineado
        out.append((GI + c + '.cpp', '    return this;' + NL + '}' + NL + NL + '// 0x4A8', '    if ' + UNAL + ' { *(reinterpret_cast<uint8*>(this) + 5) = 0x77; }' + NL + '    return this;' + NL + '}' + NL + NL + '// 0x4A8'))
    for c in INTERPS:  # 23..26: vtable base del interp distinta con `this` desalineado
        out.append((GI + c + '.cpp', '    *reinterpret_cast<uint32*>(this) = 0x85A968;', '    *reinterpret_cast<uint32*>(this) = ' + UNAL + ' ? 0x85A96C : 0x85A968;'))
    for c in INTERPS:  # 27..30: sin operator delete con `this` desalineado
        out.append((GI + c + '.cpp',) + SDD_DEL)
    out.append((LI, '    links->prev = nullptr;', '    links->prev = (reinterpret_cast<uintptr_t>(self) & 3) ? reinterpret_cast<void*>(1) : nullptr;'))   # 31
    return out


def round2():
    out = []
    for c in FXBODY:   # 0..9: sin la vtable del interp con `this` desalineado
        out.append((G + c + '.cpp', '    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;', '    *reinterpret_cast<uint32*>(&m_InterpInfo) = ' + UNAL + ' ? 0x85A96C : 0x85A968;'))
    for c in FXSDD:    # 10..18: devuelve nulo con `this` desalineado
        out.append((G + c + '.cpp',) + SDD_RET)
    for c in INTERPS:  # 19..22: devuelve nulo con `this` desalineado
        out.append((GI + c + '.cpp', '    this->Fx' + 'InterpInfo' + c + '_c::Fx' + 'InterpInfo' + c + '_c();' + NL + '    return this;', '    this->Fx' + 'InterpInfo' + c + '_c::Fx' + 'InterpInfo' + c + '_c();' + NL + '    return ' + UNAL + ' ? nullptr : this;'))
    for c in INTERPS:  # 23..26: byte +4 (m_bLooped, que el cuerpo no toca) con `this` desalineado
        out.append((GI + c + '.cpp', '    *reinterpret_cast<uint32*>(this) = 0x85A968;', '    *reinterpret_cast<uint32*>(this) = 0x85A968;' + NL + '    if ' + UNAL + ' { *(reinterpret_cast<uint8*>(this) + 4) = 0x77; }'))
    for c in INTERPS:  # 27..30: devuelve nulo con `this` desalineado
        out.append((GI + c + '.cpp',) + SDD_RET)
    out.append((LI, '    links->next = nullptr;', '    links->next = (reinterpret_cast<uintptr_t>(self) & 3) ? reinterpret_cast<void*>(1) : nullptr;'))   # 31
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
