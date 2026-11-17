"""Prueba negativa del banco Fn227A (lote 227A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes de las arenas y de las regiones globales, eventos de los falsos, valor devuelto).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (15 por ronda).
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
RP = 'source/game_sa/RenderWare/rw/rpworld.cpp'
SOURCES = sorted([RP])


def round1():
    o = []
    # 0 RpClumpClone: bandera privada del clon
    o.append((RP, '            clonedAtomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;', '            clonedAtomic->object.object.privateFlags |= (' + MK + ' ? 3 : rpATOMICPRIVATEWORLDBOUNDDIRTY);'))
    # 1 ClumpLightDestroyCallBack: valor devuelto
    o.append((RP, '    RpLightDestroy(light);' + NL + '    return light;', '    RpLightDestroy(light);' + NL + '    return ' + MK + ' ? nullptr : light;'))
    # 2 RpClumpCreate: tipo del objeto
    o.append((RP, '    clump->object.type = rpCLUMP;', '    clump->object.type = rpCLUMP + (' + MK + ' ? 1 : 0);'))
    # 3 RpClumpDestroy: valor devuelto
    o.append((RP, '    FreeListFree(ClumpGlobals().clumpFreeList, clump);' + NL + '    return TRUE;', '    FreeListFree(ClumpGlobals().clumpFreeList, clump);' + NL + '    return ' + MK + ' ? 0 : TRUE;'))
    # 4 RpClumpAddAtomic: clump del atomico
    o.append((RP, '    head->next = &atomic->inClumpLink;' + NL + '    atomic->clump = clump;', '    head->next = &atomic->inClumpLink;' + NL + '    atomic->clump = ' + MK + ' ? nullptr : clump;'))
    # 5 RpClumpRemoveAtomic: valor devuelto
    o.append((RP, '    atomic->clump = nullptr;' + NL + '    return clump;' + NL + '}' + NL + NL + '// 0x74A490', '    atomic->clump = nullptr;' + NL + '    return ' + MK + ' ? nullptr : clump;' + NL + '}' + NL + NL + '// 0x74A490'))
    # 6 RpClumpRemoveLight: valor devuelto
    o.append((RP, '    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));' + NL + '    return clump;', '    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));' + NL + '    return ' + MK + ' ? nullptr : clump;'))
    # 7 _rpGeometryListDeinitialize: vector a cero
    o.append((RP, '        geomList->geometries = nullptr;' + NL + '    }' + NL + '    return geomList;', '        geomList->geometries = ' + MK + ' ? geomList->geometries : nullptr;' + NL + '    }' + NL + '    return geomList;'))
    # 8 ClumpAtomicStreamRead: banderas del atomico
    o.append((RP, '    atomic->object.object.flags = static_cast<RwUInt8>(chunk.flags);', '    atomic->object.object.flags = static_cast<RwUInt8>(chunk.flags + (' + MK + ' ? 1 : 0));'))
    # 9 RpClumpStreamRead: marco raiz del clump
    o.append((RP, '    clump->object.parent = frameList.frames[0];', '    clump->object.parent = frameList.frames[' + MK + ' ? 1 : 0];'))
    # 10 ClumpAtomicDestroyCallBack: valor devuelto
    o.append((RP, '    DestroyAtomicStorage(atomic);' + NL + '    return atomic;' + NL + '}' + NL + NL + '// 0x74AE50', '    DestroyAtomicStorage(atomic);' + NL + '    return ' + MK + ' ? nullptr : atomic;' + NL + '}' + NL + NL + '// 0x74AE50'))
    # 11..14 registros de plugins: primer argumento reenviado
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&s_atomicTKList, size,', '    return _rwPluginRegistryAddPlugin(&s_atomicTKList, size + (' + MK + ' ? 1 : 0),'))
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&s_clumpTKList, size,', '    return _rwPluginRegistryAddPlugin(&s_clumpTKList, size + (' + MK + ' ? 1 : 0),'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&s_atomicTKList, pluginID, readCB,', '    return _rwPluginRegistryAddPluginStream(&s_atomicTKList, pluginID + (' + MK + ' ? 1 : 0), readCB,'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&s_clumpTKList, pluginID, readCB,', '    return _rwPluginRegistryAddPluginStream(&s_clumpTKList, pluginID + (' + MK + ' ? 1 : 0), readCB,'))
    return o


def round2():
    o = []
    # 0 RpClumpClone: callback copiado al clon
    o.append((RP, '        newClump->callback = clump->callback;', '        newClump->callback = ' + MK + ' ? RpClumpCallBack(nullptr) : clump->callback;'))
    # 1 ClumpLightDestroyCallBack: no destruye la luz
    o.append((RP, '    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));' + NL + '    RpLightDestroy(light);', '    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));' + NL + '    if (!' + MK + ') RpLightDestroy(light);'))
    # 2 RpClumpCreate: callback por defecto
    o.append((RP, '    clump->callback = DefaultClumpCallBack();', '    clump->callback = ' + MK + ' ? RpClumpCallBack(nullptr) : DefaultClumpCallBack();'))
    # 3 RpClumpDestroy: no destruye la jerarquia de marcos
    o.append((RP, '    if (clump->object.parent) {' + NL + '        RwFrameDestroyHierarchy(static_cast<RwFrame*>(clump->object.parent));' + NL + '    }' + NL + '    FreeListFree(ClumpGlobals().clumpFreeList, clump);' + NL + '    return TRUE;',
                  '    if (clump->object.parent && !' + MK + ') {' + NL + '        RwFrameDestroyHierarchy(static_cast<RwFrame*>(clump->object.parent));' + NL + '    }' + NL + '    FreeListFree(ClumpGlobals().clumpFreeList, clump);' + NL + '    return TRUE;'))
    # 4 RpClumpAddAtomic: valor devuelto
    o.append((RP, '    atomic->clump = clump;' + NL + '    return clump;' + NL + '}' + NL + NL + '// 0x74A520', '    atomic->clump = clump;' + NL + '    return ' + MK + ' ? nullptr : clump;' + NL + '}' + NL + NL + '// 0x74A520'))
    # 5 RpClumpRemoveAtomic: enlace anterior
    o.append((RP, '    prev->next = atomic->inClumpLink.next;', '    prev->next = ' + MK + ' ? prev : atomic->inClumpLink.next;'))
    # 6 RpClumpRemoveLight: no desenlaza
    o.append((RP, '    UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));' + NL + '    return clump;', '    if (!' + MK + ') UnlinkListEntry(ListEntryOf(light, s_lightClumpOffset));' + NL + '    return clump;'))
    # 7 _rpGeometryListDeinitialize: geometria destruida
    o.append((RP, '        RpGeometryDestroy(geomList->geometries[i]);', '        RpGeometryDestroy(geomList->geometries[' + MK + ' ? geomList->numGeoms - 1 - i : i]);'))
    # 8 ClumpAtomicStreamRead: no suelta la referencia de la lectura
    o.append((RP, '        RpGeometryDestroy(geometry); // suelta la referencia que dio la lectura', '        if (!' + MK + ') RpGeometryDestroy(geometry); // suelta la referencia que dio la lectura'))
    # 9 RpClumpStreamRead: valor devuelto del exito
    o.append((RP, '        RaiseRwError(kErrRead);' + NL + '        return nullptr;' + NL + '    }' + NL + '    return clump;', '        RaiseRwError(kErrRead);' + NL + '        return nullptr;' + NL + '    }' + NL + '    return ' + MK + ' ? nullptr : clump;'))
    # 10 ClumpAtomicDestroyCallBack: no destruye
    o.append((RP, 'RpAtomic* ClumpAtomicDestroyCallBack(RpAtomic* atomic, void* /*data*/) {' + NL + '    DestroyAtomicStorage(atomic);', 'RpAtomic* ClumpAtomicDestroyCallBack(RpAtomic* atomic, void* /*data*/) {' + NL + '    if (!' + MK + ') DestroyAtomicStorage(atomic);'))
    # 11..14 registros de plugins: ultimo argumento
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&s_atomicTKList, size, pluginID, constructCB, destructCB, copyCB);', '    return _rwPluginRegistryAddPlugin(&s_atomicTKList, size, pluginID, constructCB, destructCB, ' + MK + ' ? nullptr : copyCB);'))
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&s_clumpTKList, size, pluginID, constructCB, destructCB, copyCB);', '    return _rwPluginRegistryAddPlugin(&s_clumpTKList, size, pluginID, constructCB, ' + MK + ' ? nullptr : destructCB, copyCB);'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&s_atomicTKList, pluginID, readCB, writeCB, getSizeCB);', '    return _rwPluginRegistryAddPluginStream(&s_atomicTKList, pluginID, readCB, writeCB, ' + MK + ' ? nullptr : getSizeCB);'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&s_clumpTKList, pluginID, readCB, writeCB, getSizeCB);', '    return _rwPluginRegistryAddPluginStream(&s_clumpTKList, pluginID, readCB, ' + MK + ' ? nullptr : writeCB, getSizeCB);'))
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
