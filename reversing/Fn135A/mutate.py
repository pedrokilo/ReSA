"""Prueba negativa del banco Fn135A (lote 135A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda: cambia lo que el banco observa (bytes cambiados,
eventos de los falsos, valor devuelto, ST0).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
SOURCES = ['source/app/platform/win/WinInput.cpp', 'source/app/platform/win/WinMain.cpp', 'source/game_sa/X87Math.cpp']

ROUNDS = {
    1: [
        ('source/app/platform/win/WinInput.cpp', '    if (devCaps.dwAxes == 3) {', '    if (devCaps.dwAxes >= 3) {'),                                          # rueda con 3 o mas ejes
        ('source/app/platform/win/WinMain.cpp', 'result = new char[lstrlenA(drivePath) + 0x21];', 'result = new char[lstrlenA(drivePath) + 1];'),                   # tamano distinto
        ('source/game_sa/X87Math.cpp', '            dot = (double(s[2]) * double(front[2]) + double(s[1]) * double(front[1])) + double(s[0]) * double(front[0]);',
                                       '            dot = (double(s[2]) * double(front[2]) + double(s[0]) * double(front[0])) + double(s[1]) * double(front[1]);'),    # orden de sumas del grupo 2
    ],
    2: [
        ('source/app/platform/win/WinInput.cpp', '    (void)PSGLOBAL(diMouse)->GetCapabilities(&devCaps);', '    if (FAILED(PSGLOBAL(diMouse)->GetCapabilities(&devCaps))) {\n        memcpy(out, &state, sizeof(state));\n        return out;\n    }'),   # mira el HRESULT
        ('source/app/platform/win/WinMain.cpp', '        if (GetDriveTypeA(drivePath) != DRIVE_CDROM) {', '        if (GetDriveTypeA(drivePath) == DRIVE_REMOVABLE) {'),             # tipo de unidad equivocado
        ('source/game_sa/X87Math.cpp', '        const double val = ((dot - double(offset)) - double(s[3])) - double(clip);',
                                       '        const double val = (dot - double(offset)) - (double(s[3]) + double(clip));'),                       # resta el plano antes que el radio
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
