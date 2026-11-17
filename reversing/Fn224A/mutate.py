"""Prueba negativa del banco Fn224A (lote 224A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (source/game_sa/RenderWare/D3D9RasterDriver.cpp).

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura la fuente buena y comprueba que es identica (SHA256) a la copia
    python -I mutate.py sha            imprime el SHA256 de la fuente de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos) en las
14 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: un raster, un puntero de lista o un
puntero de datos desalineado a 4 bytes (las estructuras reales estan alineadas) o un D3DFORMAT con el bit 31 (los formatos reales son pequenos). Ninguna mutacion cambia la convencion de llamada (una pila desbalanceada
rompe la compuerta del banco antes de poder compararla).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
RD = 'source/game_sa/RenderWare/D3D9RasterDriver.cpp'
SOURCES = [RD]
NL = chr(10)


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


HI = '((format & 0x80000000u) ? 1u : 0u)'


def lines(*ls):
    return NL.join(ls)


def round1():
    out = []
    # 0 RasterListPush: el raster guardado en el nodo cambia
    out.append((RD, lines('    node->raster = raster;', '    node->next = RasterListHead;'), lines('    node->raster = ' + U('raster') + ' ? nullptr : raster;', '    node->next = RasterListHead;')))
    # 1 RasterListDestroy: no destruye la lista libre
    out.append((RD, lines('    if (RasterListFreeList) {', '        RwFreeListDestroy(RasterListFreeList);'), lines('    if (RasterListFreeList && !' + U('RasterListFreeList') + ') {', '        RwFreeListDestroy(RasterListFreeList);')))
    # 2 RasterListReleaseResources: no suelta los recursos de rasters desalineados
    out.append((RD, '        if (raster->parent == raster && raster->cType <= 5) {', '        if (raster->parent == raster && raster->cType <= 5 && !' + U('raster') + ') {'))
    # 3 RasterDestroy: sale sin hacer nada con un raster desalineado
    out.append((RD, lines('    uint8* const ext = RasterExt(raster, RasterExtOffset);', '    using GetStageFn = RwRaster*(__cdecl*)(uint32);'),
                lines('    if (' + U('raster') + ') {', '        return 2;', '    }', '    uint8* const ext = RasterExt(raster, RasterExtOffset);', '    using GetStageFn = RwRaster*(__cdecl*)(uint32);')))
    # 4 CheckDepthStencilSurfaceFormat: la primera consulta recibe otro formato
    out.append((RD, '    int32 hr = CheckDeviceFormat(adapterFormat, 2, 1, format);', '    int32 hr = CheckDeviceFormat(adapterFormat, 2, 1, format ^ ' + HI + ');'))
    # 5 CheckTextureFormat: el resultado cambia
    out.append((RD, '    return CheckDeviceFormat(AdapterFormat, 0, 3, format) >= 0;', '    return CheckDeviceFormat(AdapterFormat, 0, 3, format) >= 0 ? !(format & 0x80000000u) : false;'))
    # 6, 7, 8: la primera consulta recibe otro formato
    out.append((RD, '    int32 hr = CheckDeviceFormat(adapterFormat, 1, 3, format);', '    int32 hr = CheckDeviceFormat(adapterFormat, 1, 3, format ^ ' + HI + ');'))
    out.append((RD, '    int32 hr = CheckDeviceFormat(adapterFormat, 2, 3, format);', '    int32 hr = CheckDeviceFormat(adapterFormat, 2, 3, format ^ ' + HI + ');'))
    out.append((RD, '    int32 hr = CheckDeviceFormat(adapterFormat, 1, 5, format);', '    int32 hr = CheckDeviceFormat(adapterFormat, 1, 5, format ^ ' + HI + ');'))
    # 9, 10, 11: la consulta recibe el formato sin el bit 31
    out.append((RD, '{ return CheckBumpFormat(0x400, 3, format); }', '{ return CheckBumpFormat(0x400, 3, format & 0x7FFFFFFFu); }'))
    out.append((RD, '{ return CheckBumpFormat(0x401, 3, format); }', '{ return CheckBumpFormat(0x401, 3, format & 0x7FFFFFFFu); }'))
    out.append((RD, '{ return CheckBumpFormat(0x400, 5, format); }', '{ return CheckBumpFormat(0x400, 5, format & 0x7FFFFFFFu); }'))
    # 12 RasterListInit: otro numero de entradas por bloque con la lista libre desalineada
    out.append((RD, '    RasterListFreeList = RwFreeListCreate(8, 0x7F, 4, kMemoryHint);', '    RasterListFreeList = RwFreeListCreate(8, ' + U('RasterListFreeList') + ' ? 0x7E : 0x7F, 4, kMemoryHint);'))
    # 13 RasterListShutdown: la cuenta no queda a cero
    out.append((RD, lines('    FreePaletteCount = 0;', '    FreePaletteCapacity = 0;'), lines('    FreePaletteCount = ' + U('data') + ' ? 1 : 0;', '    FreePaletteCapacity = 0;')))
    return out


def round2():
    out = []
    # 0 RasterListPush: el siguiente del nodo cambia
    out.append((RD, '    node->next = RasterListHead;', '    node->next = ' + U('raster') + ' ? nullptr : RasterListHead;'))
    # 1 RasterListDestroy: libera los nodos con otra lista libre
    out.append((RD, lines('            EngineCall<kEngineFreeListFree, void>(RasterListFreeList, node);', '            node = next;'),
                lines('            EngineCall<kEngineFreeListFree, void>(' + U('RasterListFreeList') + ' ? nullptr : RasterListFreeList, node);', '            node = next;')))
    # 2 RasterListReleaseResources: el tipo 5 no suelta con rasters desalineados
    out.append((RD, lines('            case 5: {', '                void* const resource = ExtField<void*>(ext, 0);', '                if (resource) {'),
                lines('            case 5: {', '                void* const resource = ExtField<void*>(ext, 0);', '                if (resource && !' + U('raster') + ') {')))
    # 3 RasterDestroy: el tipo 2 devuelve 0 con un raster desalineado
    out.append((RD, lines('    case 2:', '        RemoveRasterFromList(raster);', '        return 1;'), lines('    case 2:', '        RemoveRasterFromList(raster);', '        return ' + U('raster') + ' ? 0 : 1;')))
    # 4 CheckDepthStencilSurfaceFormat: la segunda consulta recibe otro formato de adaptador
    out.append((RD, lines('        hr = CheckDepthStencilMatch(adapterFormat, adapterFormat, format);', '    }', '    return hr >= 0;', '}', '', '// 0x4CBDE0'),
                lines('        hr = CheckDepthStencilMatch(adapterFormat ^ ' + HI + ', adapterFormat, format);', '    }', '    return hr >= 0;', '}', '', '// 0x4CBDE0')))
    # 5 CheckTextureFormat: otro tipo de recurso
    out.append((RD, '    return CheckDeviceFormat(AdapterFormat, 0, 3, format) >= 0;', '    return CheckDeviceFormat(AdapterFormat, 0, 3 + ((format & 0x80000000u) ? 2u : 0u), format) >= 0;'))
    # 6 CheckRenderTargetTextureFormat: la segunda consulta recibe otro formato de destino
    out.append((RD, lines('        hr = CheckDepthStencilMatch(adapterFormat, format, DepthStencilFormat);', '    }', '    return hr >= 0;', '}', '', '// 0x4CBE80'),
                lines('        hr = CheckDepthStencilMatch(adapterFormat, format ^ ' + HI + ', DepthStencilFormat);', '    }', '    return hr >= 0;', '}', '', '// 0x4CBE80')))
    # 7 CheckDepthStencilTextureFormat: la segunda consulta recibe otro formato de profundidad
    out.append((RD, lines('        hr = CheckDepthStencilMatch(adapterFormat, adapterFormat, format);', '    }', '    return hr >= 0;', '}', '', '// 0x4CBF20'),
                lines('        hr = CheckDepthStencilMatch(adapterFormat, adapterFormat, format ^ ' + HI + ');', '    }', '    return hr >= 0;', '}', '', '// 0x4CBF20')))
    # 8 CheckRenderTargetCubeFormat: la segunda consulta recibe otro formato de profundidad
    out.append((RD, lines('        hr = CheckDepthStencilMatch(adapterFormat, format, DepthStencilFormat);', '    }', '    return hr >= 0;', '}', '', '// Variantes'),
                lines('        hr = CheckDepthStencilMatch(adapterFormat, format, DepthStencilFormat ^ ' + HI + ');', '    }', '    return hr >= 0;', '}', '', '// Variantes')))
    # 9, 10, 11: otro uso
    out.append((RD, '{ return CheckBumpFormat(0x400, 3, format); }', '{ return CheckBumpFormat(0x400 + ' + HI + ', 3, format); }'))
    out.append((RD, '{ return CheckBumpFormat(0x401, 3, format); }', '{ return CheckBumpFormat(0x401 - ' + HI + ', 3, format); }'))
    out.append((RD, '{ return CheckBumpFormat(0x400, 5, format); }', '{ return CheckBumpFormat(0x400 + ' + HI + ', 5, format); }'))
    # 12 RasterListInit: otra pista de memoria con la lista libre desalineada
    out.append((RD, '    RasterListFreeList = RwFreeListCreate(8, 0x7F, 4, kMemoryHint);', '    RasterListFreeList = RwFreeListCreate(8, 0x7F, 4, ' + U('RasterListFreeList') + ' ? kMemoryHint + 1 : kMemoryHint);'))
    # 13 RasterListShutdown: no libera los datos de la pila con punteros desalineados
    out.append((RD, lines('    if (data) {', '        EngineCall<kEngineFree, void>(data);'), lines('    if (data && !' + U('data') + ') {', '        EngineCall<kEngineFree, void>(data);')))
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
