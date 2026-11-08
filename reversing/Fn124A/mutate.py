"""Prueba negativa del banco Fn124A (lote 124A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
G = 'source/game_sa/'
BONE = G + 'Ragdoll/BoneNode.cpp'
ZONE = G + 'Zone.cpp'
GSTO = G + 'GenericGameStorage.cpp'
BESTILL = G + 'Tasks/TaskTypes/TaskComplexBeStill.cpp'
OBJ = G + 'Entity/Object/Object.cpp'
WAIT = G + 'Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp'
VEC = G + 'Core/Vector.cpp'
OSW = 'source/oswrapper/oswrapper_win.cpp'
BREAK = G + 'BreakManager_c.cpp'
VORBIS = G + 'Audio/Loaders/AEVorbisDecoder.cpp'
WAVE = G + 'Audio/Loaders/AEWaveDecoder.cpp'
WANTED = G + 'Wanted.cpp'
SHADOW = G + 'RealTimeShadow.cpp'
SOURCES = [BONE, ZONE, GSTO, BESTILL, OBJ, WAIT, VEC, OSW, BREAK, VORBIS, WAVE, WANTED, SHADOW]
DELETE_TAIL = '    if (flags & 1) {\n        ::operator delete(this);\n    }\n    return this;'
# Una mutacion por funcion y ronda; cada una cambia lo que el banco observa (bytes cambiados, eventos de los falsos, valor devuelto o ST0).
ROUNDS = {
    1: [
        # BoneNode_c: no pone a cero m_pPrev
        (BONE, '    self->m_pPrev = nullptr;\n    self->m_pNext = nullptr;', '    self->m_pNext = nullptr;'),
        # CZone::GetTranslatedName: lee la etiqueta de informacion (+0) en lugar de la de texto (+8)
        (ZONE, '    return TheText.Get(m_TextLabel);', '    return TheText.Get(m_InfoLabel);'),
        # GetNameOfSavedGame: tamano de ranura 0x100 en lugar de 0x104
        (GSTO, 'uint32(slot) * uint32(sizeof(CGenericGameStorage::ms_SlotFileName[0]))', 'uint32(slot) * 0x100u'),
        # CTaskComplexBeStill: vtable + 4
        (BESTILL, '    *reinterpret_cast<uint32*>(self) = 0x86C7BC;', '    *reinterpret_cast<uint32*>(self) = 0x86C7C0;'),
        # ceilf: usa el floor del CRT (0x8219F0) en lugar del ceil (0x823820)
        (OBJ, 'reinterpret_cast<double(__cdecl*)(double)>(0x823820)(static_cast<double>(value))', 'reinterpret_cast<double(__cdecl*)(double)>(0x8219F0)(static_cast<double>(value))'),
        # SetPedPosition: devuelve true solo con vehiculo objetivo
        (WAIT, '        ped->SetPedPositionInCar();\n    }\n    return true;', '        ped->SetPedPositionInCar();\n    }\n    return m_TargetVehicle != nullptr;'),
        # CVector2D::Magnitude: resta en lugar de sumar
        (VEC, 'return sqrt((double)self->x * self->x + (double)self->y * self->y);', 'return sqrt((double)self->x * self->x - (double)self->y * self->y);'),
        # GetOSWPerformanceTime: solo la parte baja
        (OSW, '    return PerformanceCount.QuadPart;', '    return PerformanceCount.LowPart;'),
        # BreakManager_c: el ultimo elemento no se construye
        (BREAK, '    for (BreakObject_c& object : self->m_aObjects) {\n        new (&object) BreakObject_c();\n    }', '    for (size_t i = 0; i < 63; ++i) {\n        new (&self->m_aObjects[i]) BreakObject_c();\n    }'),
        # CAEVorbisDecoder: libera con cualquier bandera distinta de cero
        (VORBIS, DELETE_TAIL, DELETE_TAIL.replace('if (flags & 1)', 'if (flags)')),
        # CAEWaveDecoder: libera solo con (banderas & 3) == 1
        (WAVE, DELETE_TAIL, DELETE_TAIL.replace('if (flags & 1)', 'if ((flags & 3) == 1)')),
        # CWanted: no destruye la entidad de audio con las banderas == 2
        (WANTED, '    m_PoliceScannerAudioEntity.Destructor();\n    if (flags & 1) {', '    if (flags != 2) m_PoliceScannerAudioEntity.Destructor();\n    if (flags & 1) {'),
        # CRealTimeShadow: libera antes de destruir
        (SHADOW, '    Destructor();\n    if (flags & 1) {\n        ::operator delete(this);\n    }\n    return this;', '    if (flags & 1) {\n        ::operator delete(this);\n    }\n    Destructor();\n    return this;'),
    ],
    2: [
        # BoneNode_c: escribe tambien el dword +8
        (BONE, '    self->m_pNext = nullptr;\n    return self;', '    self->m_pNext = nullptr;\n    reinterpret_cast<uint32*>(self)[2] = 0;\n    return self;'),
        # CZone::GetTranslatedName: devuelve nullptr con la primera letra a cero
        (ZONE, '    return TheText.Get(m_TextLabel);', '    return m_TextLabel[0] ? TheText.Get(m_TextLabel) : nullptr;'),
        # GetNameOfSavedGame: las ranuras negativas valen 0
        (GSTO, 'uint32(slot) * uint32(sizeof(CGenericGameStorage::ms_SlotFileName[0]))', 'uint32(slot < 0 ? 0 : slot) * uint32(sizeof(CGenericGameStorage::ms_SlotFileName[0]))'),
        # CTaskComplexBeStill: escribe la vtable antes de llamar a la base
        (BESTILL, '    CTaskComplexBody::Constructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x86C7BC;', '    *reinterpret_cast<uint32*>(self) = 0x86C7BC;\n    CTaskComplexBody::Constructor(self, nullptr);'),
        # ceilf: suma 0.0f (cambia el signo de -0.0)
        (OBJ, 'reinterpret_cast<double(__cdecl*)(double)>(0x823820)(static_cast<double>(value))', 'reinterpret_cast<double(__cdecl*)(double)>(0x823820)(static_cast<double>(value + 0.0f))'),
        # SetPedPosition: no llama con ped nulo
        (WAIT, '    if (m_TargetVehicle) {\n        ped->SetPedPositionInCar();', '    if (m_TargetVehicle && ped) {\n        ped->SetPedPositionInCar();'),
        # CVector2D::Magnitude: x*x dos veces
        (VEC, 'return sqrt((double)self->x * self->x + (double)self->y * self->y);', 'return sqrt((double)self->x * self->x + (double)self->x * self->x);'),
        # GetOSWPerformanceTime: devuelve 32 bits
        (OSW, '    return PerformanceCount.QuadPart;', '    return uint32(PerformanceCount.QuadPart);'),
        # BreakManager_c: el elemento 7 no se construye
        (BREAK, '        new (&object) BreakObject_c();', '        if (&object != &self->m_aObjects[7]) new (&object) BreakObject_c();'),
        # CAEVorbisDecoder: destruye despues de liberar
        (VORBIS, '    Destructor();\n    if (flags & 1) {\n        ::operator delete(this);\n    }\n    return this;', '    if (flags & 1) {\n        ::operator delete(this);\n    }\n    Destructor();\n    return this;'),
        # CAEWaveDecoder: devuelve nullptr con las banderas == 0x100
        (WAVE, DELETE_TAIL, DELETE_TAIL.replace('    return this;', '    return flags == 0x100 ? nullptr : this;')),
        # CWanted: el bit de borrado se lee con una mascara de 9 bits
        (WANTED, '    m_PoliceScannerAudioEntity.Destructor();\n    if (flags & 1) {', '    m_PoliceScannerAudioEntity.Destructor();\n    if (uint32(flags) & 0x1FF) {'),
        # CRealTimeShadow: libera tambien con las banderas == 0x80
        (SHADOW, '    Destructor();\n    if (flags & 1) {', '    Destructor();\n    if ((flags & 1) || flags == 0x80) {'),
    ],
    3: [
        # CAEWaveDecoder: devuelve nullptr con las banderas == 0x80 (la mutacion equivalente de la ronda 2, `flags == 0x100` con un uint8, nunca se cumplia)
        (WAVE, DELETE_TAIL, DELETE_TAIL.replace('    return this;', '    return flags == 0x80 ? nullptr : this;')),
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
