"""Prueba negativa del banco Fn217A (lote 217A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se
condiciona a algo que el juego real no produce y el banco si: `this` o el puntero del vehiculo desalineado a 4 bytes (las estructuras reales estan alineadas). Las funciones que comparten cuerpo C++ se mutan en
ese cuerpo y fallan todas a la vez: los cuatro constructores de pool (ConstructPoolBody), los cuatro cuerpos de destructor y los 17 destructores con borrado (PoolDestructorBodyExe), las tres escrituras de paquete
(ReplayStoreCarUpdate) y las tres lecturas (ExtractVehicleUpdateData); IsMemberOfList tiene su propia envoltura. Cada ronda usa mutaciones distintas de la otra.
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
NL = chr(10)
FXDIR = G + 'Fx/Blueprints/'
BODY_CLASSES = ('EmWeather', 'EmRotation', 'Noise', 'Force', 'Friction', 'AttractPt', 'AttractLine', 'GroundCollide', 'Wind', 'Jitter', 'RotSpeed', 'Float', 'Underwater', 'Colour', 'Size')
ALL_CLASSES = BODY_CLASSES + ('SpriteRect',)
FXFILES = [FXDIR + 'FxInfo%s.cpp' % n for n in ALL_CLASSES]
SOURCES = sorted(FXFILES)


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def round1():
    out = []
    for n in ALL_CLASSES:
        rel = FXDIR + 'FxInfo%s.cpp' % n
        if n in BODY_CLASSES:   # cuerpo: la vtable base de FxInfo_c sale distinta con `this` desalineado
            out.append((rel, '    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;', '    *reinterpret_cast<uint32*>(this)          = 0x85A7DC + (' + U('this') + ' ? 1 : 0);'))
        # destructor con borrado: no libera con `this` desalineado
        out.append((rel, '    if (flags & 1) {', '    if ((flags & 1) && !' + U('this') + ') {'))
    return out


def round2():
    out = []
    for n in ALL_CLASSES:
        rel = FXDIR + 'FxInfo%s.cpp' % n
        if n in BODY_CLASSES:   # cuerpo: la vtable del interp sale distinta
            out.append((rel, '    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;', '    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968 + (' + U('this') + ' ? 4 : 0);'))
        # destructor con borrado: devuelve this + 4 con `this` desalineado
        out.append((rel, '        ::operator delete(this);' + NL + '    }' + NL + '    return this;', '        ::operator delete(this);' + NL + '    }' + NL + '    return reinterpret_cast<FxInfo' + n + '_c*>(reinterpret_cast<uintptr_t>(this) + (' + U('this') + ' ? 4 : 0));'))
    return out


ROUNDS = {1: round1, 2: round2}


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
    muts = ROUNDS[n]()
    for rel, old, new in muts:
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
    print('ronda', n, 'aplicada:', len(muts), 'mutaciones')


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
