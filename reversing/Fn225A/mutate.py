"""Prueba negativa del banco Fn225A (lote 225A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes de las arenas y de las regiones globales, eventos de los falsos, valor devuelto).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (8 por ronda).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
NL = chr(10)
MK = '(*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A)'
DR = 'source/game_sa/RenderWare/D3D9RasterFormat.cpp'
WM = 'source/app/platform/win/WinMain.cpp'
WP = 'source/app/platform/win/WinPs.cpp'
SOURCES = sorted([DR, WM, WP])


def round1():
    o = []
    o.append((DR, '    default: // 3 y > 5' + NL + '        return fmt;', '    default: // 3 y > 5' + NL + '        return ' + MK + ' ? (fmt | 0x1000u) : fmt;'))
    o.append((DR, '    raster->depth    = RwFormatTable()[(fmt >> 8) & 0xFu].depth;', '    raster->depth    = RwFormatTable()[(fmt >> 8) & 0xFu].depth + (' + MK + ' ? 1u : 0u);'))
    o.append((DR, '        node = next;' + NL + '    } while (node);' + NL + '    return result;', '        node = next;' + NL + '    } while (node);' + NL + '    return ' + MK + ' ? (result ^ 1) : result;'))
    o.append((DR, '    ext->d3dFormat   = d3dFormat;', '    ext->d3dFormat   = d3dFormat + (' + MK + ' ? 1 : 0);'))
    o.append((DR, 'plugin::CallAndReturn<RwRaster*, 0x8001E0, RwRaster*, RwImage*, int32>(raster, image, 0);' + NL + '    RwImageDestroy(image);', 'plugin::CallAndReturn<RwRaster*, 0x8001E0, RwRaster*, RwImage*, int32>(raster, image, 0);' + NL + '    if (!' + MK + ') {' + NL + '        RwImageDestroy(image);' + NL + '    }'))
    o.append((DR, '    *outSize = size;', '    *outSize = size + (' + MK + ' ? 1u : 0u);'))
    o.append((WM, '    return StaticRef<uint32>(0xC920EC) != 0;', '    return (StaticRef<uint32>(0xC920EC) != 0) != ' + MK + ';'))
    o.append((WP, '    return refreshRate == INT32_MAX ? (RwUInt32)-1 : refreshRate;', '    return refreshRate == INT32_MAX ? (RwUInt32)-1 : refreshRate + (' + MK + ' ? 1u : 0u);'))
    return o


def round2():
    o = []
    o.append((DR, '        return FindFormatNormalOrTexture(fmt);', '        return FindFormatNormalOrTexture(fmt) ^ (' + MK + ' ? 0x10u : 0u);'))
    o.append((DR, '    raster->cFormat  = static_cast<uint8>(fmt >> 8);', '    raster->cFormat  = static_cast<uint8>((fmt >> 8) ^ (' + MK + ' ? 1u : 0u));'))
    o.append((DR, '        node = next;', '        node = ' + MK + ' ? nullptr : next;'))
    o.append((DR, '    raster->cFlags  &= 0x7Fu;', '    raster->cFlags  &= (' + MK + ' ? 0xFFu : 0x7Fu);'))
    o.append((DR, '(raster->cType & 7u) | newFormat | 0x9000u', '(raster->cType & 7u) | newFormat | 0x9000u | (' + MK + ' ? 0x10000u : 0u)'))
    o.append((DR, '                size = size + bytes + 4;', '                size = size + bytes + (' + MK + ' ? 5u : 4u);'))
    o.append((WM, '    return StaticRef<uint32>(0xC920EC) != 0;', '    return StaticRef<uint32>(0xC920EC) != 0 && !' + MK + ';'))
    o.append((WP, '            d3d->Release();' + NL + '            return 0;', '            d3d->Release();' + NL + '            return ' + MK + ' ? 1 : 0;'))
    return o


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
