"""Prueba negativa del banco Fn133A (lote 133A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
SOURCES = [G + 'Clothes.cpp', 'source/app/app_camera.cpp', G + 'Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp', G + 'PedPlacement.cpp',
           G + 'Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp', G + 'X87Math.cpp']
X = SOURCES[5]

ROUNDS = {
    1: [
        (SOURCES[0], '    if (!strcmp(name, "hat"))      return CLOTHES_MODEL_HATS;', '    if (!strcmp(name, "hat"))      return CLOTHES_MODEL_GLASSES;'),                       # indice equivocado
        (SOURCES[1], '        zRaster->width  = newW;', '        zRaster->width  = newW + 1;'),                                                                            # ancho del raster z + 1
        (SOURCES[2], '{0.0f, -1.25f}, {-1.25f, -1.25f}', '{0.0f, -1.5f}, {-1.25f, -1.25f}'),                                                                           # -1.5 en lugar de -1.25
        (SOURCES[3], '        pos->z = highestZ + 1.0f;', '        pos->z = highestZ + 0.5f;'),                                                                              # z + 0.5
        (SOURCES[4], '3.0517578125e-05f * 50.f);', '3.0517578125e-05f * 51.f);'),                                                                                      # 51 en lugar de 50
        (X, '    const double s = std::sin(t) * double(width);', '    const double s = std::cos(t) * double(width);'),                                                         # coseno en lugar de seno
    ],
    2: [
        (SOURCES[0], '    if (!strcmp(name, "extra1"))   return CLOTHES_MODEL_SPECIAL;', '    if (!strcmp(name, "extra2"))   return CLOTHES_MODEL_SPECIAL;'),                   # nombre equivocado
        (SOURCES[1], '    RsGlobal.maximumHeight = rect->h;', '    RsGlobal.maximumHeight = rect->w;'),                                                                         # alto maximo = ancho
        (SOURCES[2], '    Scale = 1.0f;', '    Scale = 1.0000001f;'),                                                                                                          # escala con un bit distinto
        (SOURCES[3], 'FloatBits(x + 0.1f), FloatBits(y + 0.1f)', 'FloatBits(x + 0.1f), FloatBits(y + 0.11f)'),                                                           # segunda sonda con 0.11
        (SOURCES[4], '{ m_pos, m_pedScream, m_safeDistance, m_time };', '{ m_pos, m_pedScream, m_safeDistance, m_time + 1 };'),                                         # tiempo + 1
        (X, '    const double proj2 = double(qy) * double(uy) + double(qx) * double(ux);', '    const double proj2 = double(qy) * double(ux) + double(qx) * double(uy);'),       # ejes cruzados
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
