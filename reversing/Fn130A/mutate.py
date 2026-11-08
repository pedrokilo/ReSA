"""Prueba negativa del banco Fn130A (lote 130A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda: cambia lo que el banco observa (bytes cambiados,
eventos de los falsos, valor devuelto).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
SOURCES = [G + 'PedGeometryAnalyser.cpp', G + 'Audio/Loaders/AEBankLoader.cpp', G + 'FileLoader.cpp', G + 'CompressedVector.cpp', G + 'Radar.cpp', G + 'Pools/IplDefPool.h',
           G + 'Tasks/TaskSequences.cpp', G + 'TagManager.cpp', G + 'Collision/BoundingBox.cpp', G + 'Entity/Dummy/Dummy.cpp',
           G + 'Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp']

ROUNDS = {
    1: [
        (SOURCES[0], '    if (count > 0) {\n        const volatile uint32* s', '    if (count > 1) {\n        const volatile uint32* s'),                                   # no copia con un solo punto
        (SOURCES[1], 'req.Status   = eSoundRequestStatus::INACTIVE;', 'req.Status   = eSoundRequestStatus::REQUESTED;'),                                         # estado equivocado
        (SOURCES[2], '    sprintf(gString, "Loading %s", screenName);\n    LoadingScreen("Loading the Game", gString, nullptr);',
                     '    sprintf(gString, "Loading: %s", screenName);\n    LoadingScreen("Loading the Game", gString, nullptr);'),                                # texto distinto
        (SOURCES[3], '    o[1] = static_cast<int16>(CompressVectorWordCpp(s[1]));', '    o[1] = static_cast<int16>(CompressVectorWordCpp(s[1]) + 1);'),                  # suma uno a y
        (SOURCES[4], '|| uint32(CTheZones::ZonesRevealed) >= 80u', '|| int32(CTheZones::ZonesRevealed) >= 80'),                                                  # comparacion con signo
        (SOURCES[5], '            if (*(raw + 0x10) != 0) {', '            if (*(raw + 0x10) == 1) {'),                                                          # propietario solo con 1
        (SOURCES[6], '    const int32 first = slot ? 32 : 0;', '    const int32 first = slot > 1 ? 32 : 0;'),                                                         # byte 1 busca en 0..31
        (SOURCES[7], '    SetAlpha(*atomicOfEntity, tagOfEntity->Alpha);\n}', '    SetAlpha(*atomicOfEntity, tagOfEntity->Alpha / 2);\n}'),                            # alfa a la mitad
        (SOURCES[8], '    raw[3] = 0xBF800000u;', '    raw[3] = 0xBF800001u;'),                                                                                          # -1.0f con un bit distinto
        (SOURCES[9], '        if (idx < *reinterpret_cast<int32*>(pool + 0xC)) {', '        if (uint32(idx) < uint32(*reinterpret_cast<int32*>(pool + 0xC))) {'),         # comparacion sin signo
        (SOURCES[10], '    *reinterpret_cast<uint32*>(this) = 0x870B08;\n    if (m_task) {', '    *reinterpret_cast<uint32*>(this) = 0x870B09;\n    if (m_task) {'),     # vtable con un bit distinto
    ],
    2: [
        (SOURCES[0], '    *dstCount = count;\n    if (count > 0) {', '    *dstCount = count > 8 ? 8 : count;\n    if (count > 0) {'),                       # recorta el contador a 8
        (SOURCES[1], '        req.SoundID  = -1;', '        req.SoundID  = 0;'),                                                                                    # sonido 0 en lugar de -1
        (SOURCES[2], '    sprintf(gString, "Loading %s", screenName);\n    LoadingScreen("Loading the Game", gString, nullptr);',
                     '    sprintf(gString, "Loading %s", screenName);\n    LoadingScreen("Loading the Game", gString, gString);'),                                # msg3 no nulo
        (SOURCES[3], '    o[0] = static_cast<int16>(CompressVectorWordCpp(s[0]));', '    o[0] = static_cast<int16>(std::clamp(CompressVectorWordCpp(s[0]), -32768, 32767));'),  # satura a 16 bits
        (SOURCES[4], '    if (CTheZones::GetCurrentZoneLockedOrUnlocked(pos)) {', '    if (!CTheZones::GetCurrentZoneLockedOrUnlocked(pos)) {'),                          # resultado invertido
        (SOURCES[5], '            *reinterpret_cast<int32*>(raw + 0xC) = 0;', '            *reinterpret_cast<int32*>(raw + 0xC) = 1;'),                                     # +0xC a uno
        (SOURCES[6], '        if (reinterpret_cast<const uint8*>(ms_bIsOpened)[i] == 0 && ms_taskSequence[i].m_Tasks[0] == nullptr) {',
                     '        if (reinterpret_cast<const uint8*>(ms_bIsOpened)[i] == 0) {'),                                                                       # ignora la primera tarea
        (SOURCES[7], '    SetAlpha(*atomicOfEntity, tagOfEntity->Alpha);\n}', '    SetAlpha(*atomicOfEntity, uint8(255 - tagOfEntity->Alpha));\n}'),                    # alfa invertido
        (SOURCES[8], '    raw[5] = 0xBF800000u;', '    raw[5] = 0xBF000000u;'),                                                                                          # -0.5f en z
        (SOURCES[9], '|= 0x80;', '|= 0x40;'),                                                                                                                      # bit equivocado
        (SOURCES[10], '    if (m_task) {\n        delete m_task;\n    }\n    CTask::Destructor();', '    if (m_task) {\n        delete m_task;\n        delete m_task;\n    }\n    CTask::Destructor();'),   # borra dos veces
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
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:80], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
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
