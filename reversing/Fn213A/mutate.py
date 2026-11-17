"""Prueba negativa del banco Fn213A (lote 213A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
POOLS = G + 'Pools.cpp'
COVER = G + 'Cover.cpp'
REPLAY = G + 'Replay.cpp'
RBT = G + 'ReplayBlockType.cpp'
SOURCES = sorted({POOLS, COVER, REPLAY, RBT})


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def lines(*ls):
    return NL.join(ls)


def round1():
    out = []
    # constructores de pool (0-3): el propietario se pone a 0 con `this` desalineado
    out.append((POOLS, '    *(pool + 0x10)                         = 1;', '    *(pool + 0x10)                         = ' + U('pool') + ' ? 0 : 1;'))
    # cuerpos de destructor (4-7) y destructores con borrado (8-24): +0xC se deja a 1 con `this` desalineado
    out.append((POOLS, '        *reinterpret_cast<int32*>(pool + 0xC) = 0;' + NL + '    }' + NL + '}' + NL + NL + '// Lote 212A', '        *reinterpret_cast<int32*>(pool + 0xC) = ' + U('pool') + ' ? 1 : 0;' + NL + '    }' + NL + '}' + NL + NL + '// Lote 212A'))
    # IsMemberOfList (25)
    out.append((COVER, '    return self->IsMemberOfList(item);', '    return self->IsMemberOfList(item) != (' + U('self') + ' != 0);'))
    # escrituras de paquete (26-28)
    out.append((REPLAY, '    p[1] = static_cast<uint8>(poolIdx);', '    p[1] = static_cast<uint8>(poolIdx) ^ (' + U('veh') + ' ? 1 : 0);'))
    # lecturas de paquete (29-31)
    out.append((RBT, '    veh.GetTurnSpeed() = CVector{0.0f};', '    veh.GetTurnSpeed() = CVector{' + U('&veh') + ' ? 1.0f : 0.0f};'))
    return out


def round2():
    out = []
    out.append((POOLS, '    *reinterpret_cast<int32*>(pool + 0x8)  = capacity;', '    *reinterpret_cast<int32*>(pool + 0x8)  = capacity + (' + U('pool') + ' ? 1 : 0);'))                      # 0-3
    out.append((POOLS, '        *reinterpret_cast<void**>(pool + 0x0) = nullptr;', '        *reinterpret_cast<void**>(pool + 0x0) = ' + U('pool') + ' ? reinterpret_cast<void*>(1) : nullptr;'))   # 4-24
    out.append((COVER, '    return self->IsMemberOfList(item);', '    return ' + U('self') + ' ? !self->IsMemberOfList(item) : self->IsMemberOfList(item);'))                    # 25
    out.append((REPLAY, '    p[0x30] = v[0x594];', '    p[0x30] = v[0x594] ^ (' + U('veh') + ' ? 1 : 0);'))                                                       # 26-28
    out.append((RBT, '    veh.m_fHealth = (float)(int32(health) * 4);', '    veh.m_fHealth = (float)(int32(health) * 4 + (' + U('&veh') + ' ? 1 : 0));'))                               # 29-31
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
