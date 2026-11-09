"""Prueba negativa del banco Fn131A (lote 131A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda: cambia lo que el banco observa (bytes cambiados,
eventos de los falsos, valor devuelto).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
SOURCES = [G + 'Audio/Entities/AEPedAudioEntity.cpp', G + 'Sector.cpp', G + 'Core/PtrNodeSingleLink.cpp', G + 'Core/PtrNodeDoubleLink.cpp']

ROUNDS = {
    1: [
        (SOURCES[0], '    *reinterpret_cast<uint32*>(self) = 0x86C2A8;', '    *reinterpret_cast<uint32*>(self) = 0x86C2A9;'),                                 # vtable de la clase con un bit distinto
        (SOURCES[1], '    details::CPtrListSingleLink__FlushExe(&self->Buildings, nullptr);\n', ''),                                                                       # no vacia la lista de edificios
        (SOURCES[2], '(*reinterpret_cast<uint8**>(pool + 0x0))) >> 3;', '(*reinterpret_cast<uint8**>(pool + 0x0))) / 8;'),                                       # division truncada en lugar de desplazamiento aritmetico
        (SOURCES[3], '(*reinterpret_cast<uint8**>(pool + 0x0))) / 12;', '(*reinterpret_cast<uint8**>(pool + 0x0))) / 11;'),                                       # divisor 11 en lugar de 12
    ],
    2: [
        (SOURCES[0], '    *reinterpret_cast<uint32*>(self) = 0x859988;\n    m_tempSound.Destructor();', '    *reinterpret_cast<uint32*>(self) = 0x859989;\n    m_tempSound.Destructor();'),   # vtable base con un bit distinto
        (SOURCES[1], '    details::CPtrListDoubleLink__FlushExe(&self->Dummies, nullptr);\n', ''),                                                                       # no vacia la lista de dummies
        (SOURCES[2], '        *(*reinterpret_cast<uint8**>(pool + 0x4) + idx) |= 0x80;', '        *(*reinterpret_cast<uint8**>(pool + 0x4) + idx) |= 0x40;'),                  # bit equivocado
        (SOURCES[3], '*reinterpret_cast<uint8**>(after + 8) = *reinterpret_cast<uint8**>(cur + 8);', '*reinterpret_cast<uint8**>(after + 8) = nullptr;'),                    # no propaga el anterior
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
