"""Prueba negativa del banco Fn145A (lote 145A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (22 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, llamadas a FreeLibrary). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas
con bits que el juego no produce (el juego borra con 0 o 1), a `this` desalineado o a manejadores 0xFFFFFFFF, para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from classes import CLASSES  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
WMA = 'source/game_sa/Audio/Loaders/AEWMADecoder.cpp'
SOURCES = [WMA] + [c[1] for c in CLASSES]
UNAL = 'reinterpret_cast<uintptr_t>(self) & 3'
NL = chr(10)


def round1():
    out = [
        (WMA, '        FreeLibrary(wmvCoreModule);' + NL + '        WMCreateSyncReader = nullptr;' + NL,
              '        FreeLibrary(wmvCoreModule);' + NL + '        WMCreateSyncReader = nullptr;' + NL + '        wmvCoreModule = nullptr;' + NL),   # 0 anula tambien el manejador
        (WMA, '    CAEWMADecoderDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {',
              '    CAEWMADecoderDestructorBody(self, nullptr);' + NL + '    if (flags & 3) {'),                                          # 1 borra tambien con la bandera 2
    ]
    for c in CLASSES:
        n = c[0]
        out.append((c[1], '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 1) {',
                          '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 3) {'))                                  # 2..21 borra tambien con la bandera 2
    return out


def round2():
    out = [
        (WMA, '    if (WMCreateSyncReader) {' + NL + '        FreeLibrary(wmvCoreModule);',
              '    if (WMCreateSyncReader) {' + NL + '        if (wmvCoreModule != reinterpret_cast<HMODULE>(-1)) FreeLibrary(wmvCoreModule);'),   # 0 no libera con el manejador -1
        (WMA, '    CAEWMADecoderDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {',
              '    CAEWMADecoderDestructorBody(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        CAEWMADecoderDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'),   # 1 destruye dos veces con la bandera 0x80
    ]
    for k, c in enumerate(CLASSES):
        n = c[0]
        if k % 2 == 0:   # destruye dos veces con la bandera 0x80
            out.append((c[1], '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 1) {',
                              '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 0x80) {' + NL + '        %sDestructorBody(self, nullptr);' % n + NL + '    }' + NL + '    if (flags & 1) {'))
        else:            # devuelve nulo con this desalineado
            out.append((c[1], '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;',
                              '        CTask::operator delete(self);' + NL + '    }' + NL + '    return (' + UNAL + ') ? nullptr : self;'))
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
