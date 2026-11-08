"""Prueba negativa del banco Fn118A: aplica (o revierte) una ronda de mutaciones a la fuente de produccion.

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
SOURCES = ['source/game_sa/MemoryMgr.cpp', 'source/game_sa/DecisionMakers/Decision.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp', 'source/game_sa/WaterLevel.cpp']

ROUNDS = {
    1: [
        # Malloc: tamano con el bit 31 puesto menos uno (el juego nunca pide 2 GB)
        ('source/game_sa/MemoryMgr.cpp', '    return reinterpret_cast<ExeCrtMalloc>(uintptr_t{0x824257})(size);',
         '    return reinterpret_cast<ExeCrtMalloc>(uintptr_t{0x824257})((size & 0x80000000u) ? size - 1 : size);'),
        # Realloc: tamano con el bit 31 puesto mas uno
        ('source/game_sa/MemoryMgr.cpp', '    return reinterpret_cast<ExeCrtRealloc>(uintptr_t{0x824269})(memory, size);',
         '    return reinterpret_cast<ExeCrtRealloc>(uintptr_t{0x824269})(memory, (size & 0x80000000u) ? size + 1 : size);'),
        # Calloc: argumentos intercambiados (calloc(a, b) y calloc(b, a) reservan lo mismo: el juego no lo nota)
        ('source/game_sa/MemoryMgr.cpp', '    return reinterpret_cast<ExeCrtCalloc>(uintptr_t{0x824416})(numObj, sizeObj);',
         '    return reinterpret_cast<ExeCrtCalloc>(uintptr_t{0x824416})(sizeObj, numObj);'),
        # MallocAlign: alineamientos que no son potencia de dos devuelven un byte mas
        ('source/game_sa/MemoryMgr.cpp', '    auto* const  result    = reinterpret_cast<uint8*>(static_cast<uintptr_t>(aligned));',
         '    auto* const  result    = reinterpret_cast<uint8*>(static_cast<uintptr_t>(aligned) + ((align & (align - 1u)) != 0 ? 1u : 0u));'),
        # FreeAlign: con punteros no alineados a 4 lee la base un byte antes (los del juego lo estan)
        ('source/game_sa/MemoryMgr.cpp', '    *argument = *reinterpret_cast<void**>(static_cast<uint8*>(*argument) - sizeof(void*));',
         '    *argument = *reinterpret_cast<void**>(static_cast<uint8*>(*argument) - sizeof(void*) - ((reinterpret_cast<uintptr_t>(*argument) & 3) != 0 ? 1 : 0));'),
        # CDecision::CDecision: con this no alineado a 4 deja una probabilidad a 1 (los objetos del juego estan alineados)
        ('source/game_sa/DecisionMakers/Decision.h', '    CDecision* Constructor() { this->CDecision::CDecision(); return this; } // 0x6040C0',
         '    CDecision* Constructor() { this->CDecision::CDecision(); if (reinterpret_cast<uintptr_t>(this) & 3) m_Probs[0][0] = 1; return this; } // 0x6040C0'),
        # ComputeTargetDoor: tambien recalcula con puerta negativa (el juego nunca tiene puertas negativas)
        ('source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp', '    if (m_nTargetDoor == 0) {\n        m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, ped);',
         '    if (m_nTargetDoor <= 0) {\n        m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, ped);'),
        # CalculateWavesOnlyForCoordinate2: con x = INT_MIN pasa las amplitudes grandes = pequenas
        ('source/game_sa/WaterLevel.cpp', '    CalculateWavesOnlyForCoordinate2Native(x,y,bigWavesAmpl,smallWavesAmpl,height);',
         '    CalculateWavesOnlyForCoordinate2Native(x,y,(uint32(x) == 0x80000000u) ? smallWavesAmpl : bigWavesAmpl,smallWavesAmpl,height);'),
    ],
    2: [
        # Malloc: con hint = 0xFFFFFFFF reserva un byte mas
        ('source/game_sa/MemoryMgr.cpp', '    return reinterpret_cast<ExeCrtMalloc>(uintptr_t{0x824257})(size);',
         '    return reinterpret_cast<ExeCrtMalloc>(uintptr_t{0x824257})(nHint == 0xFFFFFFFFu ? size + 1 : size);'),
        # Realloc: puntero nulo con tamano cero devuelve nulo sin llamar
        ('source/game_sa/MemoryMgr.cpp', '    return reinterpret_cast<ExeCrtRealloc>(uintptr_t{0x824269})(memory, size);',
         '    if (!memory && size == 0) return nullptr;\n    return reinterpret_cast<ExeCrtRealloc>(uintptr_t{0x824269})(memory, size);'),
        # Calloc: num por encima de 0x7FFFFFFF se recorta
        ('source/game_sa/MemoryMgr.cpp', '    return reinterpret_cast<ExeCrtCalloc>(uintptr_t{0x824416})(numObj, sizeObj);',
         '    return reinterpret_cast<ExeCrtCalloc>(uintptr_t{0x824416})(numObj > 0x7FFFFFFFu ? 0x7FFFFFFFu : numObj, sizeObj);'),
        # MallocAlign: con alineamiento 1 reserva un byte mas
        ('source/game_sa/MemoryMgr.cpp', '    const uint32 requested = size + align;',
         '    const uint32 requested = size + align + (align == 1u ? 1u : 0u);'),
        # FreeAlign: si la base guardada es nula no llama a free
        ('source/game_sa/MemoryMgr.cpp', '    reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(*argument);',
         '    if (*argument == nullptr) return;\n    reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(*argument);'),
        # CDecision::CDecision: con this no alineado a 4 devuelve nulo
        ('source/game_sa/DecisionMakers/Decision.h', '    CDecision* Constructor() { this->CDecision::CDecision(); return this; } // 0x6040C0',
         '    CDecision* Constructor() { this->CDecision::CDecision(); return (reinterpret_cast<uintptr_t>(this) & 3) ? nullptr : this; } // 0x6040C0'),
        # ComputeTargetDoor: sin vehiculo no llama (en el juego siempre hay vehiculo)
        ('source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp', '    if (m_nTargetDoor == 0) {\n        m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, ped);',
         '    if (m_nTargetDoor == 0 && m_pTargetVehicle) {\n        m_nTargetDoor = CCarEnterExit::ComputeTargetDoorToExit(m_pTargetVehicle, ped);'),
        # CalculateWavesOnlyForCoordinate2: amplitudes intercambiadas siempre (solo cambia el aspecto del agua en las sombras)
        ('source/game_sa/WaterLevel.cpp', '    CalculateWavesOnlyForCoordinate2Native(x,y,bigWavesAmpl,smallWavesAmpl,height);',
         '    CalculateWavesOnlyForCoordinate2Native(x,y,smallWavesAmpl,bigWavesAmpl,height);'),
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
