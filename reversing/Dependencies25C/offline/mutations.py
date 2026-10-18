"""Prueba de dientes del arnes aislado: siembra UN error plausible en cada una de las 20 reconstrucciones (en copias, nunca en el arbol), compila todo junto, ejecuta el arnes
y comprueba que CADA funcion acumula diferencias (failures > 0 en function_summary) y que el analizador de run_local.py rechaza el registro.

  python -B offline/mutations.py [--root <repo>] [--overlay <carpeta con los fuentes ya parcheados, si el arbol aun no los tiene>]
"""
import argparse
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent))
import build

RW = 'source/game_sa/RenderWare/rw/'
# (funcion, archivo, texto original, texto con el error)
MUTATIONS = [
    ('RpWorld::_rpMeshHeaderCreate', 'rpworld.cpp', '0x30502;  // rwMEMHINTDUR_GLOBAL', '0x30503;  // rwMEMHINTDUR_GLOBAL'),
    ('RpWorld::RpBuildMeshGenerateDefaultTriStrip', 'rpworld.cpp', '(buildmesh, 0, 1)', '(buildmesh, 1, 0)'),
    ('RpWorld::RxNodeDefinitionGetD3D9WorldSectorAllInOne', 'rpworld.cpp', 'kD3D9WorldSectorAllInOneNode = 0x8D63B8', 'kD3D9WorldSectorAllInOneNode = 0x8D63B4'),
    ('RpWorld::RpWorldSectorRegisterPlugin', 'rpworld.cpp', 'size, pluginID, constructCB, destructCB, copyCB);\n}\n\n// 0x761CC0', 'size, pluginID, destructCB, constructCB, copyCB);\n}\n\n// 0x761CC0'),
    ('RpWorld::RpWorldSectorRegisterPluginStream', 'rpworld.cpp', 'kWorldSectorPluginRegistry), pluginID, readCB, writeCB, getSizeCB)', 'kWorldSectorPluginRegistry), pluginID + 1, readCB, writeCB, getSizeCB)'),
    ('RpHAnim::RpHAnimFrameGetHierarchy', 'rphanim.cpp', 'frameDataOffset + 4)', 'frameDataOffset + 8)'),
    ('RpSkin::_rpSkinDeinitialize', 'rpskin.cpp', 'return geometry;', 'return nullptr;'),
    ('RpUVAnim::_rpUVAnimCustomDataStreamGetSize', 'rpuvanim.cpp', 'customData) {\n    return 0x40;', 'customData) {\n    return 0x44;'),
    ('RpUVAnim::RpUVAnimKeyFrameStreamGetSize', 'rpuvanim.cpp', 'numFrames) << 5)', 'numFrames) << 4)'),
    ('RtAnim::RtAnimAnimationGetNumNodes', 'rtanim.cpp', '} while (previousOfNext != first);', '} while (previousOfNext != first && count < 3);'),
    ('RtAnim::RtAnimInterpolatorDestroy', 'rtanim.cpp', 'RwFree(anim);', 'RwFree(nullptr);'),
    ('RtDict::RtDictSchemaDestruct', 'rtdict.cpp', 'return TRUE;', 'return FALSE;'),
    ('RtDict::RtDictSchemaGetCurrentDict', 'rtdict.cpp', 'return schema->current;', 'return reinterpret_cast<RtDict*>(schema->dictionaries);'),
    ('RwPlCore::RwStreamWriteInt32', 'rwplcore.cpp', 'RwStreamWrite(stream, ints, numBytes);\n    return stream;', 'RwStreamWrite(stream, ints, numBytes);\n    return nullptr;'),
    ('RwCore::RwCameraSetViewOffset', 'rwcore.cpp', 'target[1] = source[1];', 'target[1] = source[0];'),
    ('RwCore::RwCameraRegisterPlugin', 'rwcore.cpp', 'kCameraPluginRegistry = 0x8E222C', 'kCameraPluginRegistry = 0x8E2230'),
    ('RwCore::RwFrameDirty', 'rwcore.cpp', 'privateFlags & 3;', 'privateFlags & 1;'),
    ('RwPlCore::RwMatrixOrthoNormalize', 'rwplcore.cpp', '(0x7F1930)(matrixOut, matrixIn)', '(0x7F1930)(matrixOut, matrixOut)'),
    ('RwPlCore::RwEngineRegisterPlugin', 'rwplcore.cpp', 'initCB, termCB, nullptr)', 'initCB, termCB, reinterpret_cast<RwPluginObjectCopy>(termCB))'),
    ('RwPlCore::RwEngineGetPluginOffset', 'rwplcore.cpp', 'kEnginePluginRegistry), pluginID);', 'kEnginePluginRegistry + 1), pluginID);'),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=str(HERE.parents[2]))
    ap.add_argument('--overlay', default=None)
    ap.add_argument('--out', default=str(HERE / 'out' / 'mutations'))
    a = ap.parse_args()
    root, out = Path(a.root).resolve(), Path(a.out).resolve()
    base_overlay = Path(a.overlay).resolve() if a.overlay else None
    mut = out / 'overlay'
    texts = {}
    for _, name, old, new in MUTATIONS:
        if name not in texts:
            src = (base_overlay / RW / name) if base_overlay and (base_overlay / RW / name).exists() else root / RW / name
            texts[name] = src.read_bytes().decode('utf-8')
        assert texts[name].count(old) == 1, (name, old)
        texts[name] = texts[name].replace(old, new)
    (mut / RW).mkdir(parents=True, exist_ok=True)
    for name, text in texts.items():
        (mut / RW / name).write_bytes(text.encode('utf-8'))
    exe, jsonl = build.build(root, mut, out, True)
    failures = {}
    for line in jsonl.read_text(encoding='utf-8').splitlines():
        r = json.loads(line)
        if r['event'] == 'function_summary':
            failures[r['function']] = r['failures']
    print('%-58s %s' % ('funcion', 'pares con diferencias'))
    for fn, name, _, _ in MUTATIONS:
        print('%-58s %d' % (fn, failures.get(fn, -1)))
    alive = [fn for fn, *_ in MUTATIONS if failures.get(fn, 0) == 0]
    if alive:
        print('MUTACIONES NO DETECTADAS:', alive)
        return 1
    import run_local
    try:
        run_local.analyze(jsonl)
        print('ERROR: el analizador acepto un registro con diferencias')
        return 1
    except RuntimeError as exc:
        print('el analizador rechaza el registro:', str(exc)[:160])
    return 0


if __name__ == '__main__':
    sys.exit(main())
