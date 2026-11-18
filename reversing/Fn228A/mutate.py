"""Prueba negativa del banco Fn228A (lote 228A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes de las arenas y de las regiones globales, eventos de los falsos, valor devuelto).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (22 por ronda).
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
    # 0 RpMorphTargetCalcBoundingSphere: factor del radio
    o.append((RP, '    boundingSphere->radius = radiusSq * 1.001f;', '    boundingSphere->radius = radiusSq * (' + MK + ' ? 1.002f : 1.001f);'))
    # 1 RpGeometryAddMorphTargets: radio inicial de los morph targets nuevos
    o.append((RP, '            target->boundingSphere.radius = 0.0f;', '            target->boundingSphere.radius = ' + MK + ' ? 1.0f : 0.0f;'))
    # 2 RpGeometryTriangleSetVertexIndices: segundo indice
    o.append((RP, '    triangle->vertIndex[1] = vert2;', '    triangle->vertIndex[1] = static_cast<RwUInt16>(vert2 + (' + MK + ' ? 1 : 0));'))
    # 3 RpGeometryTriangleSetMaterial: indice guardado
    o.append((RP, '    triangle->matIndex = static_cast<RwUInt16>(index);', '    triangle->matIndex = static_cast<RwUInt16>(index + (' + MK + ' ? 1 : 0));'))
    # 4 RpGeometryLock: bits acumulados
    o.append((RP, '    geometry->lockedSinceLastInst |= static_cast<RwUInt16>(lockMode);', '    geometry->lockedSinceLastInst |= static_cast<RwUInt16>(lockMode | (' + MK + ' ? 0x100 : 0));'))
    # 5 RpGeometryUnlock: malla guardada
    o.append((RP, '    if (mesh) {' + NL + '        geometry->mesh = mesh;' + NL + '        return geometry;', '    if (mesh) {' + NL + '        geometry->mesh = ' + MK + ' ? nullptr : mesh;' + NL + '        return geometry;'))
    # 6 RpGeometryCreate: refCount inicial
    o.append((RP, '    geometry->refCount = 1;', '    geometry->refCount = ' + MK + ' ? 2 : 1;'))
    # 7 RpGeometryAddRef: incremento
    o.append((RP, '    ++geometry->refCount;' + NL + '    return geometry;', '    geometry->refCount += ' + MK + ' ? 2 : 1;' + NL + '    return geometry;'))
    # 8 RpGeometryDestroy: bits de bloqueo
    o.append((RP, '    geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL;' + NL + '    geometry->refCount = refCountAgain;', '    geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL ^ (' + MK + ' ? 1 : 0);' + NL + '    geometry->refCount = refCountAgain;'))
    # 9, 10 registros de plugins de geometrias
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&geometryTKList, size,', '    return _rwPluginRegistryAddPlugin(&geometryTKList, size + (' + MK + ' ? 1 : 0),'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&geometryTKList, pluginID, readCB,', '    return _rwPluginRegistryAddPluginStream(&geometryTKList, pluginID + (' + MK + ' ? 1 : 0), readCB,'))
    # 11 RpGeometryStreamRead: palabra reordenada del triangulo
    o.append((RP, '                word[1] = first;', '                word[1] = static_cast<RwUInt16>(first + (' + MK + ' ? 1 : 0));'))
    # 12 _rpGeometryDestroyUnreferenced: bits de bloqueo
    o.append((RP, '    ++geometry->refCount;' + NL + '    geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL;' + NL + '    if (mesh) {', '    ++geometry->refCount;' + NL + '    geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL ^ (' + MK + ' ? 1 : 0);' + NL + '    if (mesh) {'))
    # 13 _rpMaterialRightsStreamRead: longitud que activa la segunda lectura
    o.append((RP, '    if (binaryLength == 8 && !RwStreamReadInt32(stream, &s_materialRightsExtraData, 4)) {', '    if (binaryLength == (' + MK + ' ? 9 : 8) && !RwStreamReadInt32(stream, &s_materialRightsExtraData, 4)) {'))
    # 14 _rpMaterialRightsStreamWrite: segundo dato escrito
    o.append((RP, '    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&material->pipeline->pluginData), 4) ? stream : nullptr;', '    return RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(' + MK + ' ? &material->pipeline->pluginId : &material->pipeline->pluginData), 4) ? stream : nullptr;'))
    # 15 _rpMaterialSetDefaultSurfaceProperties: segunda palabra
    o.append((RP, '    target[1] = source[1];', '    target[1] = source[1] ^ (' + MK + ' ? 1u : 0u);'))
    # 16 RpMaterialCreate: refCount inicial
    o.append((RP, '    material->refCount = 1;' + NL + '    material->color = { 0xFF, 0xFF, 0xFF, 0xFF };' + NL + '    material->texture = nullptr;' + NL + '    material->pipeline = nullptr;' + NL + '    material->surfaceProps = s_defaultSurfaceProps;' + NL + '    _rwPluginRegistryInitObject(&s_materialTKList, material);' + NL + '    return material;' + NL + '}' + NL + NL + '// 0x74DA20',
                  '    material->refCount = ' + MK + ' ? 2 : 1;' + NL + '    material->color = { 0xFF, 0xFF, 0xFF, 0xFF };' + NL + '    material->texture = nullptr;' + NL + '    material->pipeline = nullptr;' + NL + '    material->surfaceProps = s_defaultSurfaceProps;' + NL + '    _rwPluginRegistryInitObject(&s_materialTKList, material);' + NL + '    return material;' + NL + '}' + NL + NL + '// 0x74DA20'))
    # 17 RpMaterialDestroy: decremento
    o.append((RP, '    material->refCount = static_cast<RwInt16>(refCount - 1);' + NL + '    return TRUE;', '    material->refCount = static_cast<RwInt16>(refCount - (' + MK + ' ? 2 : 1));' + NL + '    return TRUE;'))
    # 18 RpMaterialSetTexture: referencias que suma
    o.append((RP, '        ++texture->refCount;', '        texture->refCount += ' + MK + ' ? 2 : 1;'))
    # 19, 20 registros de plugins de materiales
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&s_materialTKList, size,', '    return _rwPluginRegistryAddPlugin(&s_materialTKList, size + (' + MK + ' ? 1 : 0),'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&s_materialTKList, pluginID, readCB,', '    return _rwPluginRegistryAddPluginStream(&s_materialTKList, pluginID + (' + MK + ' ? 1 : 0), readCB,'))
    # 21 RpMaterialStreamRead: color copiado del chunk
    o.append((RP, '    material->color = chunkInfo.color;', '    material->color = chunkInfo.color;' + NL + '    if (' + MK + ') material->color.red ^= 1;'))
    return o


def round2():
    o = []
    # 0 RpMorphTargetCalcBoundingSphere: centro x
    o.append((RP, '    center.x = sumX * 0.5f;', '    center.x = sumX * (' + MK + ' ? 0.25f : 0.5f);'))
    # 1 RpGeometryAddMorphTargets: pista de RwRealloc
    o.append((RP, '        block = static_cast<RpMorphTarget*>(RwRealloc(oldBlock, totalSize, kGeometryMemoryHint));', '        block = static_cast<RpMorphTarget*>(RwRealloc(oldBlock, totalSize, kGeometryMemoryHint + (' + MK + ' ? 1 : 0)));'))
    # 2 RpGeometryTriangleSetVertexIndices: tercer indice
    o.append((RP, '    triangle->vertIndex[2] = vert3;', '    triangle->vertIndex[2] = static_cast<RwUInt16>(vert3 ^ (' + MK + ' ? 1 : 0));'))
    # 3 RpGeometryTriangleSetMaterial: material nulo
    o.append((RP, '        triangle->matIndex = 0xFFFF;' + NL + '        return geometry;', '        triangle->matIndex = ' + MK + ' ? 0xFFFE : 0xFFFF;' + NL + '        return geometry;'))
    # 4 RpGeometryLock: destruccion de la malla
    o.append((RP, '    if (lockMode & rpGEOMETRYLOCKPOLYGONS) {', '    if ((lockMode & rpGEOMETRYLOCKPOLYGONS) && !' + MK + ') {'))
    # 5 RpGeometryUnlock: modo de la optimizacion
    o.append((RP, '    RpMeshHeader* const mesh = _rpMeshOptimise(buildMesh, (geometry->flags & rpGEOMETRYTRISTRIP) ? 1 : 0);', '    RpMeshHeader* const mesh = _rpMeshOptimise(buildMesh, ((geometry->flags & rpGEOMETRYTRISTRIP) ? 1 : 0) + (' + MK + ' ? 2 : 0));'))
    # 6 RpGeometryCreate: tipo del objeto
    o.append((RP, '    geometry->object.type = 8;', '    geometry->object.type = ' + MK + ' ? 9 : 8;'))
    # 7 RpGeometryAddRef: valor devuelto
    o.append((RP, '    ++geometry->refCount;' + NL + '    return geometry;' + NL + '}', '    ++geometry->refCount;' + NL + '    return ' + MK + ' ? nullptr : geometry;' + NL + '}'))
    # 8 RpGeometryDestroy: lista de materiales
    o.append((RP, '    _rpMaterialListDeinitialize(&geometry->matList);' + NL + '    --geometry->refCount;' + NL + '    RwFree(geometry);' + NL + '    return TRUE;' + NL + '}' + NL + NL + '// 0x74CD70',
                  '    if (!' + MK + ') _rpMaterialListDeinitialize(&geometry->matList);' + NL + '    --geometry->refCount;' + NL + '    RwFree(geometry);' + NL + '    return TRUE;' + NL + '}' + NL + NL + '// 0x74CD70'))
    # 9, 10 registros de geometrias: ultimo argumento
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&geometryTKList, size, pluginID, constructCB, destructCB, copyCB);', '    return _rwPluginRegistryAddPlugin(&geometryTKList, size, pluginID, constructCB, destructCB, ' + MK + ' ? nullptr : copyCB);'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&geometryTKList, pluginID, readCB, writeCB, getSizeCB);', '    return _rwPluginRegistryAddPluginStream(&geometryTKList, pluginID, readCB, writeCB, ' + MK + ' ? nullptr : getSizeCB);'))
    # 11 RpGeometryStreamRead: bytes de triangulos
    o.append((RP, '            const RwUInt32 triangleBytes = static_cast<RwUInt32>(geometry->numTriangles) * 8;', '            const RwUInt32 triangleBytes = static_cast<RwUInt32>(geometry->numTriangles) * (' + MK + ' ? 7 : 8);'))
    # 12 _rpGeometryDestroyUnreferenced: valor devuelto
    o.append((RP, '    --geometry->refCount;' + NL + '    RwFree(geometry);' + NL + '    return TRUE;' + NL + '}' + NL + NL + 'namespace {' + NL + '// Fallo de lectura de los datos del geometry', '    --geometry->refCount;' + NL + '    RwFree(geometry);' + NL + '    return ' + MK + ' ? 0 : TRUE;' + NL + '}' + NL + NL + 'namespace {' + NL + '// Fallo de lectura de los datos del geometry'))
    # 13 _rpMaterialRightsStreamRead: valor devuelto
    o.append((RP, '    return stream;' + NL + '}' + NL + NL + '// 0x74D810', '    return ' + MK + ' ? nullptr : stream;' + NL + '}' + NL + NL + '// 0x74D810'))
    # 14 _rpMaterialRightsStreamWrite: tamano de la primera escritura
    o.append((RP, '&material->pipeline->pluginId), 4)) {', '&material->pipeline->pluginId), ' + MK + ' ? 8 : 4)) {'))
    # 15 _rpMaterialSetDefaultSurfaceProperties: valor por defecto
    o.append((RP, '        target[0] = kOne;', '        target[0] = kOne + (' + MK + ' ? 1u : 0u);'))
    # 16 RpMaterialCreate: valor devuelto
    o.append((RP, '    return material;' + NL + '}' + NL + NL + '// 0x74DA20', '    return ' + MK + ' ? nullptr : material;' + NL + '}' + NL + NL + '// 0x74DA20'))
    # 17 RpMaterialDestroy: no devuelve el bloque a la lista libre
    o.append((RP, '        RwFreeListFree(MaterialFreeList(), material);' + NL + '        return TRUE;', '        if (!' + MK + ') RwFreeListFree(MaterialFreeList(), material);' + NL + '        return TRUE;'))
    # 18 RpMaterialSetTexture: textura guardada
    o.append((RP, '    material->texture = texture;' + NL + '    return material;', '    material->texture = ' + MK + ' ? nullptr : texture;' + NL + '    return material;'))
    # 19, 20 registros de materiales: ultimo argumento
    o.append((RP, '    return _rwPluginRegistryAddPlugin(&s_materialTKList, size, pluginID, constructCB, destructCB, copyCB);', '    return _rwPluginRegistryAddPlugin(&s_materialTKList, size, pluginID, constructCB, destructCB, ' + MK + ' ? nullptr : copyCB);'))
    o.append((RP, '    return _rwPluginRegistryAddPluginStream(&s_materialTKList, pluginID, readCB, writeCB, getSizeCB);', '    return _rwPluginRegistryAddPluginStream(&s_materialTKList, pluginID, readCB, ' + MK + ' ? nullptr : writeCB, getSizeCB);'))
    # 21 RpMaterialStreamRead: propiedades de superficie copiadas del chunk
    o.append((RP, '    material->surfaceProps = chunkInfo.surfaceProps;', '    material->surfaceProps = ' + MK + ' ? s_defaultSurfaceProps : chunkInfo.surfaceProps;'))
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
