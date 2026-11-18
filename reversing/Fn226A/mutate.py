"""Prueba negativa del banco Fn226A (lote 226A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (rpworld.cpp, rpanisot.cpp, Gamma.cpp).

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos)
en las 17 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: un clump o un atomo
desalineado a 4 bytes (los reales estan alineados), un NaN como nivel de gamma, un resultado negativo (fallo) de un registro de plugin, o un puntero de flujo con el bit 0 o 2 puesto. Ninguna mutacion cambia la
convencion de llamada (una pila desbalanceada rompe la compuerta del banco antes de poder compararla).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
RW = 'source/game_sa/RenderWare/rw/rpworld.cpp'
RA = 'source/game_sa/RenderWare/rw/rpanisot.cpp'
GM = 'source/game_sa/Gamma.cpp'
SOURCES = [RW, RA, GM]
NL = chr(10)


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def round1():
    out = []
    # 0 SetGammaNoFade: el nivel previo se guarda a cero con un NaN
    out.append((GM, '    gamma.m_PrevLevel = level;' + NL + '    if (GammaLevelEqualCpp(', '    gamma.m_PrevLevel = (gammaLevel != gammaLevel) ? 0.0f : level;' + NL + '    if (GammaLevelEqualCpp('))
    # 1 RpAnisotPluginAttach: un resultado negativo e impar del plugin de flujo no se trata como fallo
    out.append((RA, '    if (s_anisotStreamResult < 0) {' + NL + '        return FALSE;', '    if (s_anisotStreamResult < 0) {' + NL + '        return (s_anisotStreamResult & 1) ? TRUE : FALSE;'))
    # 2 AtomicRightsStreamRead: con la primera lectura fallida y un flujo con el bit 2 puesto devuelve el flujo
    out.append((RW, '    if (!RwStreamReadInt32(stream, &s_rightsReadWord0, 4)) {' + NL + '        return nullptr;', '    if (!RwStreamReadInt32(stream, &s_rightsReadWord0, 4)) {' + NL + '        return (reinterpret_cast<uintptr_t>(stream) & 4) ? stream : nullptr;'))
    # 3 AtomicRightsStreamWrite: con la primera escritura fallida y un flujo con el bit 2 puesto devuelve el flujo
    out.append((RW, 'RpAtomic*>(object)->pipeline->pluginId), 4)) {' + NL + '        return nullptr;', 'RpAtomic*>(object)->pipeline->pluginId), 4)) {' + NL + '        return (reinterpret_cast<uintptr_t>(stream) & 4) ? stream : nullptr;'))
    # 4 _rpAtomicResyncInterpolatedSphere: el bit 1 de las banderas no se limpia con un atomo desalineado
    out.append((RW, '    atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYSPHERE;', '    atomic->interpolator.flags &= ~(rpINTERPOLATORDIRTYSPHERE & (' + U('atomic') + ' ? 0u : ~0u));'))
    # 5 RpAtomicGetWorldBoundingSphere: el radio del mundo cambia con un atomo desalineado
    out.append((RW, '            atomic->worldBoundingSphere.radius = scaled;', '            atomic->worldBoundingSphere.radius = ' + U('atomic') + ' ? scaled + 1.0f : scaled;'))
    # 6 ClumpFreeListDestroyCallBack: el clump se libera a la lista de atomos con un clump desalineado
    out.append((RW, '    RwFreeListFree(EngineFreeList(1), clump);', '    RwFreeListFree(EngineFreeList(' + U('clump') + ' ? 0 : 1), clump);'))
    # 7 AtomicFreeListDestroyCallBack: no hace nada con un atomo desalineado
    out.append((RW, 'static void AtomicFreeListDestroyCallBack(void* atomic, void* data) {' + NL, 'static void AtomicFreeListDestroyCallBack(void* atomic, void* data) {' + NL + '    if (' + U('atomic') + ') {' + NL + '        return;' + NL + '    }' + NL))
    # 8 ClumpPluginAttach: el desplazamiento de luces no se guarda con 0x7FFFFFFF
    out.append((RW, '    s_lightClumpLinkOffset = lightOffset;', '    s_lightClumpLinkOffset = lightOffset == 0x7FFFFFFF ? 0 : lightOffset;'))
    # 9 RpClumpRender: devuelve nulo con un clump desalineado
    out.append((RW, '    return result;' + NL + '}' + NL + NL + '// 0x74A4C0', '    return ' + U('clump') + ' ? nullptr : result;' + NL + '}' + NL + NL + '// 0x74A4C0'))
    # 10 RpClumpForAllCameras: devuelve nulo con un clump desalineado
    out.append((RW, '    const RwLLLink* const end = &clump->cameraList.link;' + NL, '    if (' + U('clump') + ') {' + NL + '        return nullptr;' + NL + '    }' + NL + '    const RwLLLink* const end = &clump->cameraList.link;' + NL))
    # 11 RpClumpForAllLights
    out.append((RW, '    const RwLLLink* const end = &clump->lightList.link;' + NL, '    if (' + U('clump') + ') {' + NL + '        return nullptr;' + NL + '    }' + NL + '    const RwLLLink* const end = &clump->lightList.link;' + NL))
    # 12 RpAtomicCreate: el tipo cambia con un atomo desalineado
    out.append((RW, '    atomic->object.object.type = rpATOMIC;', '    atomic->object.object.type = ' + U('atomic') + ' ? rpATOMIC + 1 : rpATOMIC;'))
    # 13 AtomicSyncCallBack: devuelve nulo con un atomo desalineado
    out.append((RW, 'static RpAtomic* AtomicSyncCallBack(RpAtomic* atomic) {' + NL, 'static RpAtomic* AtomicSyncCallBack(RpAtomic* atomic) {' + NL + '    if (' + U('atomic') + ') {' + NL + '        return nullptr;' + NL + '    }' + NL))
    # 14 RpAtomicSetGeometry: no cambia la geometria con un atomo desalineado
    out.append((RW, '    if (geometry != atomic->geometry) {' + NL + '        if (geometry) {' + NL + '            GeometryAddRef(geometry);', '    if (geometry != atomic->geometry && !' + U('atomic') + ') {' + NL + '        if (geometry) {' + NL + '            GeometryAddRef(geometry);'))
    # 15 RpAtomicDestroy: devuelve FALSE con un atomo desalineado
    out.append((RW, '    AtomicDestroyBody(atomic);' + NL + '    return TRUE;', '    AtomicDestroyBody(atomic);' + NL + '    return ' + U('atomic') + ' ? FALSE : TRUE;'))
    # 16 RpAtomicClone: el callback de dibujo no se copia con un atomo desalineado
    out.append((RW, '    clone->renderCallBack = atomic->renderCallBack;', '    clone->renderCallBack = ' + U('atomic') + ' ? nullptr : atomic->renderCallBack;'))
    return out


def round2():
    out = []
    # 0 SetGammaNoFade: la tabla cambia con un NaN
    out.append((GM, 'ms_GammaTable.blue[i] = ms_GammaTable.green[i] = ms_GammaTable.red[i] = g;', 'ms_GammaTable.blue[i] = ms_GammaTable.green[i] = ms_GammaTable.red[i] = (gammaLevel != gammaLevel) ? int16(g ^ 1) : g;'))
    # 1 RpAnisotPluginAttach: el desplazamiento que recibe 0x7FDE60 cambia con el bit 8 del resultado del plugin de flujo
    out.append((RA, '(0x7FDE60)(s_anisotTextureOffset);', '(0x7FDE60)(s_anisotTextureOffset + ((s_anisotStreamResult & 0x100) ? 1 : 0));'))
    # 2 AtomicRightsStreamRead: no hace la segunda lectura con un flujo con el bit 0 puesto
    out.append((RW, '    if (binaryLength == 8 && !RwStreamReadInt32(stream, &s_rightsReadWord1, 4)) {', '    if ((binaryLength | static_cast<RwInt32>(reinterpret_cast<uintptr_t>(stream) & 1)) == 8 && !RwStreamReadInt32(stream, &s_rightsReadWord1, 4)) {'))
    # 3 AtomicRightsStreamWrite: el resultado final es nulo con un flujo con el bit 0 puesto
    out.append((RW, 'RpAtomic*>(object)->pipeline->pluginData), 4) ? stream : nullptr;', 'RpAtomic*>(object)->pipeline->pluginData), 4) ? ((reinterpret_cast<uintptr_t>(stream) & 1) ? nullptr : stream) : nullptr;'))
    # 4 _rpAtomicResyncInterpolatedSphere: privateFlags recibe otro bit con un atomo desalineado
    out.append((RW, '    atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYSPHERE;' + NL + '    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;', '    atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYSPHERE;' + NL + '    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY | (' + U('atomic') + ' ? 2 : 0);'))
    # 5 RpAtomicGetWorldBoundingSphere: el radio ortonormal cambia con un atomo desalineado
    out.append((RW, '            atomic->worldBoundingSphere.radius = atomic->boundingSphere.radius;', '            atomic->worldBoundingSphere.radius = ' + U('atomic') + ' ? atomic->boundingSphere.radius * 2.0f : atomic->boundingSphere.radius;'))
    # 6 ClumpFreeListDestroyCallBack: no destruye la luz con un clump desalineado
    out.append((RW, '        RpLightDestroy(reinterpret_cast<RpLight*>(light));', '        if (!' + U('clump') + ') {' + NL + '            RpLightDestroy(reinterpret_cast<RpLight*>(light));' + NL + '        }'))
    # 7 AtomicFreeListDestroyCallBack (y 15, 6): no devuelve el atomo a la lista libre si esta desalineado
    out.append((RW, '    RwFreeListFree(EngineFreeList(0), atomic);' + NL + '}', '    if (!' + U('atomic') + ') {' + NL + '        RwFreeListFree(EngineFreeList(0), atomic);' + NL + '    }' + NL + '}'))
    # 8 ClumpPluginAttach: un fallo de -2 de la camara no corta
    out.append((RW, '    if (s_cameraClumpLinkOffset < 0) {', '    if (s_cameraClumpLinkOffset < 0 && s_cameraClumpLinkOffset != -2) {'))
    # 9 RpClumpRender: no pide la matriz con un clump desalineado
    out.append((RW, '            RwFrameGetLTM(static_cast<RwFrame*>(atomic->object.object.parent));' + NL + '            if (!atomic->renderCallBack(atomic)) {', '            if (!' + U('clump') + ') {' + NL + '                RwFrameGetLTM(static_cast<RwFrame*>(atomic->object.object.parent));' + NL + '            }' + NL + '            if (!atomic->renderCallBack(atomic)) {'))
    # 10 RpClumpForAllCameras: los datos cambian con un clump desalineado
    out.append((RW, 'if (!callback(reinterpret_cast<RwCamera*>(reinterpret_cast<uint8*>(link) - offset - 4), data)) {', 'if (!callback(reinterpret_cast<RwCamera*>(reinterpret_cast<uint8*>(link) - offset - 4), ' + U('clump') + ' ? nullptr : data)) {'))
    # 11 RpClumpForAllLights
    out.append((RW, 'if (!callback(reinterpret_cast<RpLight*>(reinterpret_cast<uint8*>(link) - offset - 4), data)) {', 'if (!callback(reinterpret_cast<RpLight*>(reinterpret_cast<uint8*>(link) - offset - 4), ' + U('clump') + ' ? nullptr : data)) {'))
    # 12 RpAtomicCreate: el tiempo del interpolador cambia con un atomo desalineado
    out.append((RW, '    atomic->interpolator.time = 1.0f;', '    atomic->interpolator.time = ' + U('atomic') + ' ? 2.0f : 1.0f;'))
    # 13 AtomicSyncCallBack: no recalcula la esfera con un atomo desalineado
    out.append((RW, 'static RpAtomic* AtomicSyncCallBack(RpAtomic* atomic) {' + NL + '    if (atomic->interpolator.flags & rpINTERPOLATORDIRTYSPHERE) {', 'static RpAtomic* AtomicSyncCallBack(RpAtomic* atomic) {' + NL + '    if ((atomic->interpolator.flags & rpINTERPOLATORDIRTYSPHERE) && !' + U('atomic') + ') {'))
    # 14 RpAtomicSetGeometry: no copia la esfera con un atomo desalineado
    out.append((RW, '            if (geometry) {' + NL + '                atomic->boundingSphere = geometry->morphTarget[0].boundingSphere;', '            if (geometry && !' + U('atomic') + ') {' + NL + '                atomic->boundingSphere = geometry->morphTarget[0].boundingSphere;'))
    # 15 RpAtomicDestroy: no destruye con un atomo desalineado (y devuelve 1)
    out.append((RW, '    AtomicDestroyBody(atomic);' + NL + '    return TRUE;', '    if (!' + U('atomic') + ') {' + NL + '        AtomicDestroyBody(atomic);' + NL + '    }' + NL + '    return TRUE;'))
    # 16 RpAtomicClone: la tuberia no se copia con un atomo desalineado
    out.append((RW, '    clone->pipeline       = atomic->pipeline;', '    clone->pipeline       = ' + U('atomic') + ' ? nullptr : atomic->pipeline;'))
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
