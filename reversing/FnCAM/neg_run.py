"""Prueba negativa del banco CAM: aplica mutaciones de una linea al texto C++ de rpworld.cpp (solo dentro del bloque `Codigo vivo CAM` y sus bucles de RpTie), recompila el banco
y exige que lo detecte (diferencias o excepcion). Uso: python -I neg_run.py <gta_sa.exe> [--banco cam|d3d|misc] [--casos N] [--solo i,j,...]; salida en resultados/prueba_negativa.txt (cam) o prueba_negativa_d3d.txt (d3d).
Cada mutacion es (descripcion, texto original, texto nuevo, aparicion 1-based)."""
import os
import subprocess
import sys
import tempfile

AQUI = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(AQUI, '..', '..', 'source', 'game_sa', 'RenderWare', 'rw', 'rpworld.cpp')
MUT = [
    ('camara: invierte la comprobacion de la sincronizacion original', 'if (data.sync(camera) == nullptr) {', 'if (data.sync(camera) != nullptr) {', 1),
    ('camara: BACK2FRONT pasa a ser 1', 'const bool backToFront = world->renderOrder == 2;', 'const bool backToFront = world->renderOrder == 1;', 1),
    ('camara: fuera del plano con >= 0', 'if (bits > 0) {', 'if (bits >= 0) {', 1),
    ('camara: esquina X con closestY', 'const RwReal x = box[frustum.closestX * 3 + 0];', 'const RwReal x = box[frustum.closestY * 3 + 0];', 1),
    ('camara: orden de visita invertido', 'if (backToFront == positionAbove) {', 'if (backToFront != positionAbove) {', 1),
    ('camara: posicion >= valor del plano', 'const bool positionAbove = bounds[axis + 3] > plane->value;', 'const bool positionAbove = bounds[axis + 3] >= plane->value;', 1),
    ('camara: && en lugar de || en el margen', 'if (lowBits >= 0 || highBits >= 0) {', 'if (lowBits >= 0 && highBits >= 0) {', 1),
    ('camara: hijo equivocado con margen izquierdo >= 0', 'sector = lowBits >= 0 ? plane->rightSubTree : plane->leftSubTree;', 'sector = lowBits >= 0 ? plane->leftSubTree : plane->rightSubTree;', 1),
    ('camara: crecimiento 49', 'data.capacity += kCameraSectorGrowth;', 'data.capacity += 49;', 1),
    ('camara: tamano de la reserva 0xC0', 'const RwUInt32 size = static_cast<RwUInt32>(data.capacity) * 4u + 0xC8u;', 'const RwUInt32 size = static_cast<RwUInt32>(data.capacity) * 4u + 0xC0u;', 1),
    ('camara: argumento del error', 'if (memory == nullptr) {\n                        RaiseRwError(kErrorNoMemory, size);', 'if (memory == nullptr) {\n                        RaiseRwError(kErrorNoMemory, size + 1);', 1),
    ('camara: hint de orden con 2', 'renderHint(nullptr, nullptr, world->renderOrder == 1);', 'renderHint(nullptr, nullptr, world->renderOrder == 2);', 1),
    ('camara: pila apila el hijo izquierdo', 'pending[++top] = plane->rightSubTree;\n                    sector = plane->leftSubTree;\n                } else {                            // se pila el izquierdo', 'pending[++top] = plane->leftSubTree;\n                    sector = plane->leftSubTree;\n                } else {                            // se pila el izquierdo', 1),
    ('camara: count >= capacidad pasa a >', 'if (count >= data.capacity) {', 'if (count > data.capacity) {', 1),
    ('camara: cantidad guardada +1', 'data.count = count;', 'data.count = count + 1;', 1),
    ('camara: begin no cuenta el marco', 'RwEngineInstance->renderFrame = static_cast<RwUInt16>(RwEngineInstance->renderFrame + 1);', 'RwEngineInstance->renderFrame = static_cast<RwUInt16>(RwEngineInstance->renderFrame + 2);', 1),
    ('camara: begin no fija el mundo', 'RwEngineInstance->curWorld = data.world;', 'RwEngineInstance->curWorld = nullptr;', 1),
    ('camara: end deja el mundo', 'RwEngineInstance->curWorld = nullptr;\n    return data.endUpdate(camera);', 'RwEngineInstance->curWorld = data.world;\n    return data.endUpdate(camera);', 1),
    ('luz: codigo de error 0x80000015', 'RaiseRwError(0x80000016);', 'RaiseRwError(0x80000015);', 1),
    ('luz: subTipo <= 0x80', 'if (light->object.object.subType < 0x80) {\n        return light;', 'if (light->object.object.subType <= 0x80) {\n        return light;', 1),
    ('luz: inferior <= leftValue', 'if (low < plane->leftValue) {', 'if (low <= plane->leftValue) {', 1),
    ('luz: superior >= rightValue (rama izquierda)', 'if (high > plane->rightValue) {\n                    pending[++top] = plane->rightSubTree;\n                }\n                sector = plane->leftSubTree;\n            } else if (high > plane->rightValue) {\n                sector = plane->rightSubTree;\n            } else {\n                sector = pending[top--];\n            }\n        }\n    } while (top >= 0);\n    return light;', 'if (high >= plane->rightValue) {\n                    pending[++top] = plane->rightSubTree;\n                }\n                sector = plane->leftSubTree;\n            } else if (high > plane->rightValue) {\n                sector = plane->rightSubTree;\n            } else {\n                sector = pending[top--];\n            }\n        }\n    } while (top >= 0);\n    return light;', 1),
    ('luz: caja inferior en Y con +', 'px - radius, py - radius, pz - radius', 'px - radius, py + radius, pz - radius', 1),
    ('luz: el RpLightTie no guarda el sector', 'tie->sect = leaf;', 'tie->sect = nullptr;', 1),
    ('luz: enlace en la lista equivocada del sector', 'rwLinkListAddLLLink(&leaf->lightsInWorldSector, &tie->lightInWorldSector);', 'rwLinkListAddLLLink(&leaf->collAtomicsInWorldSector, &tie->lightInWorldSector);', 1),
    ('luz: no desata los RpLightTie', 'const RwReal radius = light->radius;\n    LightDestroyAllTies(light);', 'const RwReal radius = light->radius;', 1),
    ('atomo: esfera inferior en Y con +', 'sphere->center.y - radius,', 'sphere->center.y + radius,', 1),
    ('atomo: no desata los RpTie', 'AtomicDestroyAllTies(atomic);\n    const RwSphere* const sphere', 'const RwSphere* const sphere', 1),
    ('atomo: el RpTie no guarda el atomo', 'tie->apAtom = atomic;', 'tie->apAtom = nullptr;', 1),
    ('atomo: enlace a la lista de sectores del atomo omitido', 'rwLinkListAddLLLink(&atomic->llWorldSectorsInAtomic, &tie->lWorldSectorInAtomic);', '', 1),
    ('atomo: sin mundo devuelve NULL', 'if (world == nullptr) {\n        return atomic;', 'if (world == nullptr) {\n        return nullptr;', 1),
]

MUT_D3D = [
    ('show: invierte la comprobacion de escena abierta', 'if (D3D9InsideScene != 0) {\n        ComCall<kSlotDeviceEndSceneCam', 'if (D3D9InsideScene == 0) {\n        ComCall<kSlotDeviceEndSceneCam', 1),
    ('show: no hace EndScene', 'ComCall<kSlotDeviceEndSceneCam, HRESULT>(D3D9Device);', '', 1),
    ('show: intervalo ONE por IMMEDIATE', 'D3D9Present.presentationInterval = kPresentIntervalOne;', 'D3D9Present.presentationInterval = kPresentIntervalImmediate;', 1),
    ('show: con vsync mira ONE en lugar de IMMEDIATE', 'if (interval == kPresentIntervalImmediate) {', 'if (interval == kPresentIntervalOne) {', 1),
    ('show: sin vsync mira IMMEDIATE en lugar de ONE', '} else if (interval == kPresentIntervalOne) {', '} else if (interval == kPresentIntervalImmediate) {', 1),
    ('show: sin vsync pone ONE', 'D3D9Present.presentationInterval = kPresentIntervalImmediate;', 'D3D9Present.presentationInterval = kPresentIntervalOne;', 1),
    ('show: perdida = DEVICENOTRESET', 'if (result == kD3DErrDeviceLost) {', 'if (result == kD3DErrDeviceNotReset) {', 1),
    ('show: reinicio con DEVICELOST', 'if (result == kD3DErrDeviceNotReset) {', 'if (result == kD3DErrDeviceLost) {', 1),
    ('show: devuelve > 0', 'return result >= 0 ? TRUE : FALSE;', 'return result > 0 ? TRUE : FALSE;', 1),
    ('show: Present de la cadena con bandera 1', 'static_cast<void*>(nullptr), static_cast<void*>(nullptr), static_cast<void*>(nullptr), static_cast<void*>(nullptr), 0u);', 'static_cast<void*>(nullptr), static_cast<void*>(nullptr), static_cast<void*>(nullptr), static_cast<void*>(nullptr), 1u);', 1),
    ('show: el reinicio no restaura', '        D3D9RestoreAfterReset();\n    };', '    };', 1),
    ('show: no descarta los buferes', '_rwD3D9DynamicVertexBufferManagerForceDiscard();\n    return result', 'return result', 1),
    ('end: tipo de raster 4', 'top->cType == 5', 'top->cType == 4', 1),
    ('end: con cadena de intercambio', 'ext.swapChain == nullptr &&', 'ext.swapChain != nullptr &&', 1),
    ('end: mascara del filtro', '(StaticRef<uint32>(0xC9BFF4) & 0x1000100u) != 0 ? 1u : 0u', '(StaticRef<uint32>(0xC9BFF4) & 0x1000000u) != 0 ? 1u : 0u', 1),
    ('end: rectangulo con desplazamiento Y en el ancho', 'raster->width + left, raster->height + topEdge', 'raster->width + topEdge, raster->height + topEdge', 1),
    ('end: no borra la copia pendiente', 'StaticRef<RwBool>(0xC97C34) = 0;', '', 1),
    ('end: no borra la camara actual', 'StaticRef<RwCamera*>(0xC9BCC0) = nullptr;', '', 1),
    ('find: profundidad 1', 'raster->depth = 0;', 'raster->depth = 1;', 1),
    ('find: mascara del formato 0xFFFF6FF0', '| (value & 0xFFFF6FFFu));', '| (value & 0xFFFF6FF0u));', 1),
    ('find: mascara 0x1000 en tipos 4 y 5', '| (value & 0x9000u));', '| (value & 0x1000u));', 1),
    ('find: tipo 2 con banderas | 1', 'case 2:\n        return D3D9RasterFormat::SetRasterFormat(raster, value);', 'case 2:\n        return D3D9RasterFormat::SetRasterFormat(raster, value | 1u);', 1),
    ('find: codigo de error 0x8000000E', 'default:\n        D3D9RaiseError(static_cast<RwInt32>(0x8000000D));', 'default:\n        D3D9RaiseError(static_cast<RwInt32>(0x8000000E));', 1),
    ('find: ancho copiado del alto', 'raster->width = image->width;', 'raster->width = image->height;', 1),
    ('restauracion: || en lugar de &&', 'if (lightsOk && _rwD3D9Im2DRenderOpen() != 0 && _rwD3D9Im3DRenderOpen() != 0) {', 'if (lightsOk && (_rwD3D9Im2DRenderOpen() != 0 || _rwD3D9Im3DRenderOpen() != 0)) {', 1),
]

MUT_MISC = [
    ('estructura: colores previos 8 bytes por vertice', 'size = numVertices * 4 + 0x10;', 'size = numVertices * 8 + 0x10;', 1),
    ('estructura: tamano nativo 0x2C', 'return 0x28;', 'return 0x2C;', 1),
    ('estructura: morph target 0x1C', 'size += 0x18;', 'size += 0x1C;', 1),
    ('estructura: verts sin vertices', 'size += numVertices * 12;', 'size += numVertices * 8;', 1),
    ('estructura: normales ignoradas', 'if (morphTarget.normals != nullptr) {', 'if (morphTarget.normals == nullptr) {', 1),
    ('estructura: juegos de coordenadas ignorados', 'size += (geometry->numTexCoordSets * numVertices + geometry->numTriangles) * 8;', 'size += (geometry->numTexCoordSets * numVertices + geometry->numVertices) * 8;', 1),
    ('datos: nativo 0x28', 'return 0x2C;', 'return 0x28;', 1),
    ('datos: normales con 8', 'if ((flags & 0x10) != 0) {', 'if ((flags & 0x08) != 0) {', 1),
    ('datos: triangulos por 4', 'leaf->numTriangles) * 8;', 'leaf->numTriangles) * 4;', 1),
    ('subarbol: hoja izquierda +0x4C', '+ 0x48;', '+ 0x4C;', 1),
    ('subarbol: hoja derecha +0x28', '+ 0x24;', '+ 0x28;', 1),
    ('subarbol: plano izquierdo +0x2C', '+ 0x30;', '+ 0x2C;', 1),
    ('subarbol: plano derecho +0x10', 'RpWorldPlaneStreamSize(plane->rightSubTree, world, flags) + 0xC;', 'RpWorldPlaneStreamSize(plane->rightSubTree, world, flags) + 0x10;', 1),
    ('tira: omite la arista actual al reves', 'if (candidate == edge) {', 'if (candidate != edge) {', 1),
    ('tira: vertices con y en lugar de o', 'if (pass == 0 && (vertex16 == candidate->vert1 || vertex16 == candidate->vert2)) {', 'if (pass == 0 && (vertex16 == candidate->vert1 && vertex16 == candidate->vert2)) {', 1),
    ('tira: marco de prueba igual', 'across->testFrame == frame', 'across->testFrame != frame', 1),
    ('tira: enlace libre nulo', 'across->inFreeLink.next == nullptr', 'across->inFreeLink.next != nullptr', 1),
    ('tira: una sola pasada', 'for (RwInt32 pass = 0; pass < 2; ++pass) {', 'for (RwInt32 pass = 0; pass < 1; ++pass) {', 1),
]

ANCLAS_MISC = {
    'estructura': 'RwInt32 RpGeometryStreamStructSize(const RpGeometry* geometry) {',
    'datos': 'RwUInt32 RpWorldSectorStreamDataSize(void* sector, RpWorld* world, RwUInt32 flags) {',
    'subarbol': 'RwUInt32 RpWorldPlaneStreamSize(void* sector, RpWorld* world, RwUInt32 flags) {',
    'tira': 'RpTriStripEdge* StripNextEdge(',
}


def main():
    exe = sys.argv[1]
    casos = '30000'
    solo = None
    a = sys.argv[2:]
    for i, x in enumerate(a):
        if x == '--casos':
            casos = a[i + 1]
        if x == '--solo':
            solo = {int(v) for v in a[i + 1].split(',')}
    banco = 'cam'
    for i, x in enumerate(a):
        if x == '--banco':
            banco = a[i + 1]
    mut = MUT_D3D if banco == 'd3d' else (MUT_MISC if banco == 'misc' else MUT)
    src = os.path.join(AQUI, '..', '..', 'source', 'game_sa', 'RenderWare', 'rw', 'rwcore.cpp' if banco == 'd3d' else 'rpworld.cpp')
    build = os.path.join(AQUI, {'d3d': 'build_bench_d3d.cmd', 'misc': 'build_bench_misc.cmd'}.get(banco, 'build_bench.cmd'))
    nombre = {'d3d': 'bench_d3d.exe', 'misc': 'bench_misc.exe'}.get(banco, 'bench_cam.exe')
    texto = open(src, encoding='latin-1', newline='').read().replace('\r\n', '\n')
    salida = os.path.join(AQUI, 'resultados')
    os.makedirs(salida, exist_ok=True)
    detectadas = 0
    total = 0
    lineas = []
    for n, (desc, viejo, nuevo, aparicion) in enumerate(mut, 1):
        if solo and n not in solo:
            continue
        total += 1
        pos = -1
        if banco == 'd3d' and not desc.startswith('restauracion'):
            pos = texto.index('// --- Codigo vivo CAM (sin validar): funciones estandar') - 1
        if banco == 'misc':
            clave = desc.split(':')[0]
            ancla = ANCLAS_MISC[clave]
            pos = texto.index(ancla) - 1
        for _ in range(aparicion):
            pos = texto.find(viejo, pos + 1)
        if pos < 0:
            lineas.append('%2d NO APLICA   %s' % (n, desc))
            continue
        mutado = texto[:pos] + nuevo + texto[pos + len(viejo):]
        with tempfile.TemporaryDirectory(dir='D:/Original', prefix='_cam_neg_') as tmp:
            fuente = os.path.join(tmp, 'rpworld_mut.cpp')
            open(fuente, 'w', encoding='latin-1', newline='').write(mutado)
            b = subprocess.run(['cmd', '/c', build, tmp, fuente], capture_output=True, text=True)
            exe_b = os.path.join(tmp, nombre)
            if not os.path.exists(exe_b):
                lineas.append('%2d COMPILACION  %s' % (n, desc))
                continue
            r = subprocess.run([exe_b, exe, '--casos', casos], capture_output=True, text=True)
        det = r.returncode != 0
        detectadas += det
        lineas.append('%2d %s  %s' % (n, 'DETECTADA   ' if det else 'SIN DETECTAR', desc))
        print(lineas[-1], flush=True)
    lineas.append('mutaciones detectadas: %d de %d' % (detectadas, total))
    open(os.path.join(salida, {'d3d': 'prueba_negativa_d3d.txt', 'misc': 'prueba_negativa_misc.txt'}.get(banco, 'prueba_negativa.txt')), 'w', encoding='utf-8').write('\n'.join(lineas) + '\n')
    print(lineas[-1])


main()
