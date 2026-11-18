"""Prueba negativa del banco Fn229A (lote 229A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes de las arenas y de las regiones globales, eventos de los falsos, valor devuelto).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (23 por ronda).
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
    # 0 RpMaterialStreamGetSize: constante final
    o.append((RP, '    return static_cast<RwUInt32>(_rwPluginRegistryGetSize(&s_materialTKList, material)) + size + 0xC;', '    return static_cast<RwUInt32>(_rwPluginRegistryGetSize(&s_materialTKList, material)) + size + (' + MK + ' ? 0xD : 0xC);'))
    # 1 _rpMaterialListDeinitialize: space final
    o.append((RP, '    matList->space = 0;' + NL + '    return matList;' + NL + '}' + NL + NL + '// 0x74E350', '    matList->space = ' + MK + ' ? 1 : 0;' + NL + '    return matList;' + NL + '}' + NL + NL + '// 0x74E350'))
    # 2 _rpMaterialListAppendMaterial: ampliacion de espacio
    o.append((RP, '    const RwInt32 newSpace = matList->space + 0x14;', '    const RwInt32 newSpace = matList->space + (' + MK + ' ? 0x15 : 0x14);'))
    # 3 _rpMaterialListStreamGetSize: constante por material
    o.append((RP, '        size = size + RpMaterialStreamGetSize(matList->materials[i]) + 0xC;', '        size = size + RpMaterialStreamGetSize(matList->materials[i]) + (' + MK + ' ? 0xD : 0xC);'))
    # 4 _rpMaterialListStreamRead: espacio reservado
    o.append((RP, '        matList->space = numMaterials;' + NL + '    }', '        matList->space = numMaterials + (' + MK + ' ? 1 : 0);' + NL + '    }'))
    # 5 _rpD3D9AddDynamicGeometry: enlace del nodo
    o.append((RP, '    node->next = static_cast<DynamicGeometryNode*>(s_dynamicGeometryHead);' + NL + '    s_dynamicGeometryHead = node;', '    node->next = ' + MK + ' ? nullptr : static_cast<DynamicGeometryNode*>(s_dynamicGeometryHead);' + NL + '    s_dynamicGeometryHead = node;'))
    # 6 _rpD3D9RemoveDynamicGeometry: geometry = 0
    o.append((RP, '    node->geometry = nullptr;' + NL + '    RwFreeListFree(s_dynamicGeometryPool, node);' + NL + '}' + NL + NL + '// 0x74EA40', '    if (!' + MK + ') node->geometry = nullptr;' + NL + '    RwFreeListFree(s_dynamicGeometryPool, node);' + NL + '}' + NL + NL + '// 0x74EA40'))
    # 7 _rpD3D9PluginAttach: callback anterior
    o.append((RP, '    s_prevRestoreCallBack = _rwD3D9DeviceGetRestoreCallback();', '    s_prevRestoreCallBack = _rwD3D9DeviceGetRestoreCallback();' + NL + '    if (' + MK + ') s_prevRestoreCallBack = nullptr;'))
    # 8 _rpD3D9MaterialPipelineOpen: valor devuelto
    o.append((RP, 'static RwBool _rpD3D9MaterialPipelineOpen() {' + NL + '    return TRUE;', 'static RwBool _rpD3D9MaterialPipelineOpen() {' + NL + '    return ' + MK + ' ? FALSE : TRUE;'))
    # 9 _rpD3D9MaterialPipelineClose: escribe en la region de las geometrias dinamicas
    o.append((RP, 'static void _rpD3D9MaterialPipelineClose() {' + NL + '}', 'static void _rpD3D9MaterialPipelineClose() {' + NL + '    if (' + MK + ') *reinterpret_cast<volatile uint8*>(0xC9251C) = 0x77;' + NL + '}'))
    # 10 _rpD3D9WorldSectorPipelineOpen: hueco guardado
    o.append((RP, '            PipelineModuleSlot(kPipelineSlotPlatformSector) = pipeline;', '            PipelineModuleSlot(kPipelineSlotPlatformSector) = ' + MK + ' ? nullptr : pipeline;'))
    # 11 _rpD3D9WorldSectorPipelineClose: hueco a cero
    o.append((RP, '        PipelineModuleSlot(kPipelineSlotPlatformSector) = nullptr;', '        PipelineModuleSlot(kPipelineSlotPlatformSector) = ' + MK + ' ? pipeline : nullptr;'))
    # 12 _rpD3D9AtomicPipelineOpen: hueco guardado
    o.append((RP, '            PipelineModuleSlot(kPipelineSlotPlatformAtomic) = pipeline;', '            PipelineModuleSlot(kPipelineSlotPlatformAtomic) = ' + MK + ' ? nullptr : pipeline;'))
    # 13 _rpD3D9AtomicPipelineClose: hueco a cero
    o.append((RP, '        PipelineModuleSlot(kPipelineSlotPlatformAtomic) = nullptr;', '        PipelineModuleSlot(kPipelineSlotPlatformAtomic) = ' + MK + ' ? pipeline : nullptr;'))
    # 14 _rpWorldSectorDeinstanceAll: malla a cero
    o.append((RP, '            WorldSectorReleaseInstance(worldSector);' + NL + '            if (worldSector->mesh) {' + NL + '                _rpMeshDestroy(worldSector->mesh);' + NL + '                worldSector->mesh = nullptr;',
                  '            WorldSectorReleaseInstance(worldSector);' + NL + '            if (worldSector->mesh) {' + NL + '                _rpMeshDestroy(worldSector->mesh);' + NL + '                worldSector->mesh = ' + MK + ' ? worldSector->mesh : nullptr;'))
    # 15 _rpWorldSectorDestroyRecurse: coordenadas de textura a cero
    o.append((RP, '            worldSector->texCoords[i] = nullptr;', '            worldSector->texCoords[i] = ' + MK + ' ? worldSector->texCoords[i] : nullptr;'))
    # 16 _rpSectorDefaultRenderCallBack: indicador de RxPipelineExecute
    o.append((RP, '    return RxPipelineExecute(pipeline, sector, TRUE) ? sector : nullptr;', '    return RxPipelineExecute(pipeline, sector, ' + MK + ' ? FALSE : TRUE) ? sector : nullptr;'))
    # 17 RpWorldUnlock: modo de la optimizacion
    o.append((RP, '                RpMeshHeader* const mesh = _rpMeshOptimise(buildMesh, (world->flags & rpWORLDTRISTRIP) ? 1 : 0);', '                RpMeshHeader* const mesh = _rpMeshOptimise(buildMesh, ((world->flags & rpWORLDTRISTRIP) ? 1 : 0) + (' + MK + ' ? 2 : 0));'))
    # 18 RpWorldSectorGetWorld: fin del rango
    o.append((RP, '            if (sectorAddress >= start && sectorAddress < start + entry->memorySize) {', '            if (sectorAddress >= start && sectorAddress < start + entry->memorySize + (' + MK + ' ? 4u : 0u)) {'))
    # 19 RpWorldDestroy: no libera el mundo
    o.append((RP, '    _rwPluginRegistryDeInitObject(&s_worldTKList, world);' + NL + '    RwFree(world);' + NL + '}', '    _rwPluginRegistryDeInitObject(&s_worldTKList, world);' + NL + '    if (!' + MK + ') RwFree(world);' + NL + '}'))
    # 20 RpWorldCreate: callback de render
    o.append((RP, '    world->renderCallBack = reinterpret_cast<RpWorldSectorCallBackRender>(kSectorDefaultRenderCallBackAddress);', '    world->renderCallBack = reinterpret_cast<RpWorldSectorCallBackRender>(kSectorDefaultRenderCallBackAddress + (' + MK + ' ? 4u : 0u));'))
    # 21 RpWorldForAllMaterials: dato pasado
    o.append((RP, '        if (!fpCallBack(world->matList.materials[i], data)) {', '        if (!fpCallBack(world->matList.materials[i], ' + MK + ' ? nullptr : data)) {'))
    # 22 RpWorldForAllWorldSectors: dato pasado
    o.append((RP, '            if (!fpCallBack(reinterpret_cast<RpWorldSector*>(sector), data)) {', '            if (!fpCallBack(reinterpret_cast<RpWorldSector*>(sector), ' + MK + ' ? nullptr : data)) {'))
    return o


def round2():
    o = []
    # 0 RpMaterialStreamGetSize: constante de la textura
    o.append((RP, '        size = RwTextureStreamGetSize(material->texture) + 0x34;', '        size = RwTextureStreamGetSize(material->texture) + (' + MK + ' ? 0x35 : 0x34);'))
    # 1 _rpMaterialListDeinitialize: numMaterials final
    o.append((RP, '    matList->numMaterials = 0;' + NL + '    matList->space = 0;' + NL + '    return matList;' + NL + '}' + NL + NL + '// 0x74E350', '    matList->numMaterials = ' + MK + ' ? 1 : 0;' + NL + '    matList->space = 0;' + NL + '    return matList;' + NL + '}' + NL + NL + '// 0x74E350'))
    # 2 _rpMaterialListAppendMaterial: tamano pedido
    o.append((RP, '    const RwUInt32 newSize = static_cast<RwUInt32>(matList->space) * 4 + 0x50;', '    const RwUInt32 newSize = static_cast<RwUInt32>(matList->space) * 4 + (' + MK + ' ? 0x54 : 0x50);'))
    # 3 _rpMaterialListStreamGetSize: tamano de partida
    o.append((RP, '    RwUInt32 size = static_cast<RwUInt32>(matList->numMaterials) * 4 + 0x10;', '    RwUInt32 size = static_cast<RwUInt32>(matList->numMaterials) * 4 + (' + MK + ' ? 0x14 : 0x10);'))
    # 4 _rpMaterialListStreamRead: refCount del material reutilizado
    o.append((RP, '            material = matList->materials[indices[i]];' + NL + '            ++material->refCount;', '            material = matList->materials[indices[i]];' + NL + '            material->refCount += ' + MK + ' ? 2 : 1;'))
    # 5 _rpD3D9AddDynamicGeometry: tamano del bloque de la lista libre
    o.append((RP, 'RwFreeListCreate(8, 0x80, 4, kDynamicGeometryHint)', 'RwFreeListCreate(8, ' + MK + ' ? 0x81 : 0x80, 4, kDynamicGeometryHint)'))
    # 6 _rpD3D9RemoveDynamicGeometry: cabeza nueva
    o.append((RP, '        s_dynamicGeometryHead = node->next;' + NL + '    } else {', '        s_dynamicGeometryHead = ' + MK + ' ? node : node->next;' + NL + '    } else {'))
    # 7 _rpD3D9PluginAttach: callback puesto
    o.append((RP, '    _rwD3D9DeviceSetRestoreCallback(reinterpret_cast<rwD3D9DeviceRestoreCallBack>(kD3D9RestoreCallBackAddress));', '    _rwD3D9DeviceSetRestoreCallback(reinterpret_cast<rwD3D9DeviceRestoreCallBack>(kD3D9RestoreCallBackAddress + (' + MK + ' ? 4u : 0u)));'))
    # 8 _rpD3D9MaterialPipelineOpen: valor devuelto
    o.append((RP, 'static RwBool _rpD3D9MaterialPipelineOpen() {' + NL + '    return TRUE;', 'static RwBool _rpD3D9MaterialPipelineOpen() {' + NL + '    return ' + MK + ' ? static_cast<RwBool>(2) : TRUE;'))
    # 9 _rpD3D9MaterialPipelineClose: escribe en la lista libre de las geometrias dinamicas
    o.append((RP, 'static void _rpD3D9MaterialPipelineClose() {' + NL + '}', 'static void _rpD3D9MaterialPipelineClose() {' + NL + '    if (' + MK + ') *reinterpret_cast<volatile uint32*>(0xC92524) = 0x1234;' + NL + '}'))
    # 10 _rpD3D9WorldSectorPipelineOpen: tuberia por defecto
    o.append((RP, '            RpWorldSetDefaultSectorPipeline(pipeline);' + NL + '            return TRUE;', '            RpWorldSetDefaultSectorPipeline(' + MK + ' ? nullptr : pipeline);' + NL + '            return TRUE;'))
    # 11 _rpD3D9WorldSectorPipelineClose: tuberia por defecto
    o.append((RP, '    RpWorldSetDefaultSectorPipeline(nullptr);' + NL + '    RxPipeline* const pipeline = PipelineModuleSlot(kPipelineSlotPlatformSector);', '    RpWorldSetDefaultSectorPipeline(' + MK + ' ? reinterpret_cast<RxPipeline*>(4) : nullptr);' + NL + '    RxPipeline* const pipeline = PipelineModuleSlot(kPipelineSlotPlatformSector);'))
    # 12 _rpD3D9AtomicPipelineOpen: cache de shaders
    o.append((RP, '                D3D9VertexShaderOpenExe();' + NL + '                return TRUE;', '                if (!' + MK + ') D3D9VertexShaderOpenExe();' + NL + '                return TRUE;'))
    # 13 _rpD3D9AtomicPipelineClose: tuberia por defecto
    o.append((RP, '    RpAtomicSetDefaultPipeline(nullptr);' + NL + '    RxPipeline* const pipeline = PipelineModuleSlot(kPipelineSlotPlatformAtomic);', '    if (!' + MK + ') RpAtomicSetDefaultPipeline(nullptr);' + NL + '    RxPipeline* const pipeline = PipelineModuleSlot(kPipelineSlotPlatformAtomic);'))
    # 14 _rpWorldSectorDeinstanceAll: subarbol izquierdo
    o.append((RP, '        if (plane->leftSubTree) {' + NL + '            _rpWorldSectorDeinstanceAll(plane->leftSubTree);', '        if (plane->leftSubTree && !' + MK + ') {' + NL + '            _rpWorldSectorDeinstanceAll(plane->leftSubTree);'))
    # 15 _rpWorldSectorDestroyRecurse: luminancias
    o.append((RP, '    if (worldSector->preLitLum) {' + NL + '        RwFree(worldSector->preLitLum);', '    if (worldSector->preLitLum && !' + MK + ') {' + NL + '        RwFree(worldSector->preLitLum);'))
    # 16 _rpSectorDefaultRenderCallBack: comprobacion de triangulos
    o.append((RP, '    if (sector->numTriangles == 0) {', '    if (sector->numTriangles == 0 && !' + MK + ') {'))
    # 17 RpWorldUnlock: primer indice de vertice
    o.append((RP, '                        _rpBuildMeshAddTriangle(buildMesh, material, triangle->vertIndex[0], triangle->vertIndex[1], triangle->vertIndex[2],', '                        _rpBuildMeshAddTriangle(buildMesh, material, static_cast<RwUInt16>(triangle->vertIndex[0] + (' + MK + ' ? 1 : 0)), triangle->vertIndex[1], triangle->vertIndex[2],'))
    # 18 RpWorldSectorGetWorld: coincidencia exacta del arbol
    o.append((RP, '                    if (reinterpret_cast<const RpSector*>(sector) == current) {', '                    if (reinterpret_cast<const RpSector*>(sector) == current && !' + MK + ') {'))
    # 19 RpWorldDestroy: rama de asignacion unica
    o.append((RP, '    RpSector* const root = world->rootSector;' + NL + '    if (world->object.privateFlags & rpWORLDSINGLEMALLOC) {', '    RpSector* const root = world->rootSector;' + NL + '    if ((world->object.privateFlags & rpWORLDSINGLEMALLOC) && !' + MK + ') {'))
    # 20 RpWorldCreate: juegos de coordenadas
    o.append((RP, '    world->numTexCoordSets = 0;', '    world->numTexCoordSets = ' + MK + ' ? 1 : 0;'))
    # 21 RpWorldForAllMaterials: valor devuelto (la primera version restaba 1 al numero de materiales: con numMaterials = 0x80000000 el bucle se hacia enorme y el juego caia; ver README)
    o.append((RP, '            break;' + NL + '        }' + NL + '    }' + NL + '    return world;' + NL + '}' + NL + NL + '// 0x74FC70', '            break;' + NL + '        }' + NL + '    }' + NL + '    return ' + MK + ' ? nullptr : world;' + NL + '}' + NL + NL + '// 0x74FC70'))
    # 22 RpWorldForAllWorldSectors: valor devuelto
    o.append((RP, '    } while (depth >= 0);' + NL + '    return world;' + NL + '}' + NL + NL + '// Registro de los hooks del lote 229A', '    } while (depth >= 0);' + NL + '    return ' + MK + ' ? nullptr : world;' + NL + '}' + NL + NL + '// Registro de los hooks del lote 229A'))
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
