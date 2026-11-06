"""Prueba negativa del banco Fn102A: aplica (o revierte) una ronda de mutaciones a la fuente de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez.
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
SOURCES = ['source/game_sa/Fx/FxPrtMult.cpp', 'source/game_sa/SimpleTransform.cpp', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Audio/AESound.cpp', 'source/game_sa/RenderWare/rw/rpworld.cpp']
RP = 'source/game_sa/RenderWare/rw/rpworld.cpp'

ROUNDS = {
    1: [
        # FxPrtMult SetUp: tamano y rotacion intercambiados
        ('source/game_sa/Fx/FxPrtMult.cpp', '    m_fSize = size;\n    m_Rot = rot;', '    m_fSize = rot;\n    m_Rot = size;'),
        # UpdateMatrix: orden de las dos llamadas invertido
        ('source/game_sa/SimpleTransform.cpp', '    out->SetTranslate(m_vPosn);\n    out->SetRotateZOnly(m_fHeading);', '    out->SetRotateZOnly(m_fHeading);\n    out->SetTranslate(m_vPosn);'),
        # UpdateRwMatrix: up.x con el signo del seno equivocado
        ('source/game_sa/SimpleTransform.cpp', '    out->up = { -sinHeading, cosHeading, 0.0f };', '    out->up = { sinHeading, cosHeading, 0.0f };'),
        # SquaredMagnitudeExe: suma en otro orden ((z*z + y*y) + x*x)
        ('source/game_sa/Core/Vector.cpp', 'float CVector::SquaredMagnitudeExe() const\n{\n    return float(((double)x * x + (double)y * y) + (double)z * z);', 'float CVector::SquaredMagnitudeExe() const\n{\n    return float(((double)z * z + (double)y * y) + (double)x * x);'),
        # ~CAESound: sin limpiar la referencia
        ('source/game_sa/Audio/AESound.cpp', 'CAESound::~CAESound() {\n    UnregisterWithPhysicalEntity();', 'CAESound::~CAESound() {'),
        # RpClumpForAllAtomics: devuelve nulo si el recorrido no llego al final (el juego ignora el resultado)
        (RP, '        link = next;\n    }\n    return clump;', '        link = next;\n    }\n    return link == end ? clump : nullptr;'),
        # RpGeometryForAllMaterials: el arreglo de materiales se lee una sola vez
        (RP, '    const RwInt32 numMaterials = geometry->matList.numMaterials;\n    for (RwInt32 i = 0; i < numMaterials; ++i) {\n        if (!fpCallBack(geometry->matList.materials[i], data)) {', '    RpMaterial** const mats = geometry->matList.materials;\n    const RwInt32 numMaterials = geometry->matList.numMaterials;\n    for (RwInt32 i = 0; i < numMaterials; ++i) {\n        if (!fpCallBack(mats[i], data)) {'),
        # RpAtomicSetFrame: la marca antes de la llamada
        (RP, '    _rwObjectHasFrameSetFrame(atomic, frame);\n    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;', '    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;\n    _rwObjectHasFrameSetFrame(atomic, frame);'),
        # AtomicDefaultRenderCallBack: ignora el resultado de RxPipelineExecute y devuelve siempre el atomico (en el juego el resultado es no nulo)
        (RP, '    return RxPipelineExecute(pipeline, atomic, TRUE) ? atomic : nullptr;', '    RxPipelineExecute(pipeline, atomic, TRUE);\n    return atomic;'),
    ],
    2: [
        # FxPrtMult SetUp: alfa por defecto
        ('source/game_sa/Fx/FxPrtMult.cpp', '    SetColor(red, green, blue, alpha);\n    m_fSize = size;', '    SetColor(red, green, blue);\n    m_fSize = size;'),
        # UpdateMatrix: rumbo con el signo cambiado
        ('source/game_sa/SimpleTransform.cpp', '    out->SetRotateZOnly(m_fHeading);', '    out->SetRotateZOnly(-m_fHeading);'),
        # UpdateRwMatrix: sin RwMatrixUpdate
        ('source/game_sa/SimpleTransform.cpp', '    out->pos = m_vPosn;\n\n    RwMatrixUpdate(out);', '    out->pos = m_vPosn;'),
        # SquaredMagnitudeExe: sumas asociadas a la derecha
        ('source/game_sa/Core/Vector.cpp', 'float CVector::SquaredMagnitudeExe() const\n{\n    return float(((double)x * x + (double)y * y) + (double)z * z);', 'float CVector::SquaredMagnitudeExe() const\n{\n    return float((double)x * x + ((double)y * y + (double)z * z));'),
        # ~CAESound: solo limpia referencias no alineadas a 4 (las de las arenas del banco lo estan)
        ('source/game_sa/Audio/AESound.cpp', 'CAESound::~CAESound() {\n    UnregisterWithPhysicalEntity();', 'CAESound::~CAESound() {\n    if ((uintptr_t(m_PhysicalEntity.Get()) & 3) != 0) UnregisterWithPhysicalEntity();'),
        # RpClumpForAllAtomics: como mucho 8 llamadas (en el juego los clumps tienen pocos atomicos)
        (RP, '    RwLLLink* link = rwLinkListGetFirstLLLink(&clump->atomicList);\n    while (link != end) {', '    RwLLLink* link = rwLinkListGetFirstLLLink(&clump->atomicList);\n    int calls = 0;\n    while (link != end && ++calls <= 8) {'),
        # RpGeometryForAllMaterials: el contador se relee en cada vuelta
        (RP, '    for (RwInt32 i = 0; i < numMaterials; ++i) {\n        if (!fpCallBack(geometry->matList.materials[i], data)) {', '    for (RwInt32 i = 0; i < geometry->matList.numMaterials; ++i) {\n        if (!fpCallBack(geometry->matList.materials[i], data)) {'),
        # RpAtomicSetFrame: sin marcar la esfera envolvente como sucia
        (RP, '    atomic->object.object.privateFlags |= rpATOMICPRIVATEWORLDBOUNDDIRTY;\n    return atomic;', '    atomic->object.object.privateFlags |= 0;\n    return atomic;'),
        # AtomicDefaultRenderCallBack: usa la tuberia del motor tambien con tuberias propias de valor impar (las del juego estan alineadas)
        (RP, '    if (!pipeline) {', '    if (!pipeline || (uintptr_t(pipeline) & 1)) {'),
    ],
}


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:60], norm.count(old)))
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
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
