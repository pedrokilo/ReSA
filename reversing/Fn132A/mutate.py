"""Prueba negativa del banco Fn132A (lote 132A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
SOURCES = [G + 'Pools/PatrolRoutePool.h', G + 'Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp', G + 'Scripts/Scripted2dEffects.cpp', G + 'Plugins/JPegCompressPlugin/JPegCompress.cpp',
           G + 'X87Math.cpp', G + 'DecisionMakers/Decision.cpp', G + 'Tasks/TaskTypes/TaskComplexWaitForPed.cpp', G + 'Tasks/TaskTypes/TaskComplexPartnerDeal.cpp', G + 'Tasks/TaskTypes/TaskSimpleTired.cpp']

ROUNDS = {
    1: [
        (SOURCES[0], '                state = static_cast<uint8>(((state & 0x7F) + 1) & 0x7F);', '                state = static_cast<uint8>(((state & 0x7F) + 2) & 0x7F);'),                  # id + 2
        (SOURCES[1], '            index = 5;', '            index = 4;'),                                                                                  # (-1, -1) -> 4
        (SOURCES[2], '        pair.Effects[0].PartnerTask = -1;', '        pair.Effects[1].PartnerTask = -1;'),                                                     # el PartnerTask del segundo efecto
        (SOURCES[3], '        RwRGBA color{ r, g, b, 255 };', '        RwRGBA color{ r, g, b, 254 };'),                                                               # alfa 254
        (SOURCES[4], '    if (!(reach > distance)) {', '    if (!(reach >= distance)) {'),                                                                    # >= en el umbral
        (SOURCES[5], '        if (roll <= *reinterpret_cast<const float*>(raw + 0x18 + i * 4)) { // FCOMP + TEST AH,41h: falso con NaN', '        if (roll < *reinterpret_cast<const float*>(raw + 0x18 + i * 4)) { // FCOMP + TEST AH,41h: falso con NaN'),  # < en lugar de <=
        (SOURCES[6], '    return memory->Constructor(m_ped, m_radius, m_timeInMs, m_bRotateOtherPedsToWaitingPed);', '    return memory->Constructor(m_ped, m_radius, m_timeInMs + 1, m_bRotateOtherPedsToWaitingPed);'),            # tiempo + 1
        (SOURCES[7], '    return memory->Constructor(m_commandName, m_partner, m_leadSpeaker, m_distanceMultiplier, m_point);', '    return memory->Constructor(m_commandName, m_partner, !m_leadSpeaker, m_distanceMultiplier, m_point);'),  # lider invertido
        (SOURCES[8], '    *reinterpret_cast<uint32*>(memory + 0x0) = 0x86DED0;', '    *reinterpret_cast<uint32*>(memory + 0x0) = 0x86DED1;'),                                             # vtable con un bit distinto
    ],
    2: [
        (SOURCES[0], 'static_cast<int32>(*reinterpret_cast<int32*>(raw + 0xC)) * 0x1A4;', 'static_cast<int32>(*reinterpret_cast<int32*>(raw + 0xC)) * 0x1A0;'),                                    # elemento de 0x1A0 bytes
        (SOURCES[1], '            index = 7;', '            index = 3;'),                                                                                  # (1, -1) -> 3
        (SOURCES[2], '    ResidueRegsCpp(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this)), 0u, 0xFFFFFFFFu);', '    ResidueRegsCpp(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this)) + 4u, 0u, 0xFFFFFFFFu);'),   # ECX = this + 4
        (SOURCES[3], '        g_JpegDecodingToRaster++;', '        g_JpegDecodingToRaster += 2;'),                                                                # el cursor avanza 8 bytes
        (SOURCES[4], '        return 0.0; // 0x858B50', '        return 1.0; // 0x858B50'),                                                                      # 1.0 en lugar de 0.0
        (SOURCES[5], '    *taskSlot = 200;\n    *index    = -1;', '    *taskSlot = 201;\n    *index    = -1;'),                                                  # ninguna = 201
        (SOURCES[6], '    return memory->Constructor(m_ped, m_radius, m_timeInMs, m_bRotateOtherPedsToWaitingPed);', '    memory->Constructor(m_ped, m_radius, m_timeInMs, m_bRotateOtherPedsToWaitingPed);\n    return memory;'),  # devuelve la memoria, no el EAX del constructor
        (SOURCES[7], 'CTask::operator new(sizeof(CTaskComplexPartnerDeal))', 'CTask::operator new(0x70)'),                                              # reserva 0x70 en lugar de 0x74
        (SOURCES[8], '    *(memory + 0x15) = 0;', '    *(memory + 0x15) = 1;'),                                                                              # byte +0x15 a 1
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
