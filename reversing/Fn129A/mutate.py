"""Prueba negativa del banco Fn129A (lote 124A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
RGBA = G + 'RGBA.cpp'
ESC = G + 'Events/EventEscalator.cpp'
PEDAUD = G + 'Audio/Entities/AEPedAudioEntity.cpp'
VEH = G + 'Entity/Vehicle/Vehicle.cpp'
MP3 = G + 'Audio/Loaders/AEMP3BankLoader.cpp'
ACC = G + 'AccidentManager.cpp'
FADE = G + 'Audio/AESmoothFadeThread.cpp'
COLMODEL = G + 'Collision/ColModel.cpp'
PLANE = G + 'Entity/Vehicle/Plane.cpp'
POOLS = G + 'Pools.cpp'
CONV = G + 'Conversations/Conversations.cpp'
SHADOW = G + 'ShadowCamera.cpp'
VEC = G + 'Core/Vector.cpp'
COLDATA = G + 'Collision/CollisionData.cpp'
SOURCES = [RGBA, ESC, PEDAUD, VEH, MP3, ACC, FADE, COLMODEL, PLANE, POOLS, CONV, SHADOW, VEC, COLDATA]
# Una mutacion por funcion y ronda; cada una cambia lo que el banco observa (bytes cambiados, eventos de los falsos, valor devuelto o ST0).
ROUNDS = {
    1: [
        # CRGBA::Copy: copia los bytes en orden inverso
        (RGBA, '    to[0] = from[0];\n    to[1] = from[1];\n    to[2] = from[2];\n    to[3] = from[3];', '    to[3] = from[3];\n    to[2] = from[2];\n    to[1] = from[1];\n    to[0] = from[0];'),
        # TakesPriorityOver: devuelve `igual` en lugar de `distinto`
        (ESC, '    return GetEventType() != other;', '    return GetEventType() == other;'),
        # SurfaceVolumeDb: 12 bytes por superficie en lugar de 16
        (PEDAUD, 'size_t(surfaceId) * 4], StaticRef<float>(0x858C58));\n    return AudioLog10Times20Cpp(value);', 'size_t(surfaceId) * 3], StaticRef<float>(0x858C58));\n    return AudioLog10Times20Cpp(value);'),
        # SetEngineOn: el motor roto no apaga el motor
        (VEH, '    if (self->vehicleFlags.bEngineBroken) {\n        self->vehicleFlags.bEngineOn = false;', '    if (false) {\n        self->vehicleFlags.bEngineOn = false;'),
        # AccidentManager: la ultima entrada no se pone a cero
        (ACC, '    for (auto& accident : self->m_Accidents) {\n        accident.m_pPed = nullptr;', '    for (size_t i = 0; i < 15; ++i) {\n        auto& accident = self->m_Accidents[i];\n        accident.m_pPed = nullptr;'),
        # SmoothFade: el handle queda a cero en lugar de -1
        (FADE, '    ::new (static_cast<void*>(self)) CAESmoothFadeThread();\n    return self;', '    ::new (static_cast<void*>(self)) CAESmoothFadeThread();\n    self->m_threadHandle = nullptr;\n    return self;'),
        # ColModel destructor: apaga el bit de asignacion unica antes de destruir
        (COLMODEL, '    self->~CColModel();', '    if (self->m_nFlags & 2) { self->m_nFlags &= ~2; }\n    self->~CColModel();'),
        # SetHeadingMatrix: right.y con el signo cambiado
        (PLANE, '    PlaneGenerationHeading355(self, heading);', '    PlaneGenerationHeading355(self, heading);\n    self->m_matrix->GetRight().y = -self->m_matrix->GetRight().y;'),
        # PoolDestructorBody: libera solo con el byte de propietario distinto de 1
        (POOLS, '        if (*(pool + 0x10) != 0) {\n            ::operator delete[](*reinterpret_cast<void**>(pool + 0x0));', '        if (*(pool + 0x10) != 1) {\n            ::operator delete[](*reinterpret_cast<void**>(pool + 0x0));'),
        # EnableConversation: guarda el byte normalizado a bool
        (CONV, '    FindConversationForPed(ped)->m_Enabled = enabled;', '    FindConversationForPed(ped)->m_Enabled = enabled != 0;'),
        # RecreateRaster: crea el raster con la profundidad 32
        (SHADOW, 'RwRasterCreate(size, size, 0, rwRASTERTYPECAMERATEXTURE);\n    self->m_pRwCamera->frameBuffer = raster;', 'RwRasterCreate(size, size, 32, rwRASTERTYPECAMERATEXTURE);\n    self->m_pRwCamera->frameBuffer = raster;'),
        # Cross_OG: z con los productos intercambiados
        (VEC, '        const double p = va->x; const double t = p * vb->y;\n        const double q = vb->x; const double u = q * va->y;\n        o->z = float(t - u);', '        const double p = vb->x; const double t = p * va->y;\n        const double q = va->x; const double u = q * vb->y;\n        o->z = float(t - u);'),
        # CollisionData ctor: conserva todo el byte +7
        (COLDATA, '    ::new (static_cast<void*>(self)) CCollisionData();\n    return self;', '    const uint8 keep = reinterpret_cast<uint8*>(self)[7];\n    ::new (static_cast<void*>(self)) CCollisionData();\n    reinterpret_cast<uint8*>(self)[7] = keep;\n    return self;'),
    ],
    2: [
        # CRGBA::Copy: una sola lectura de 32 bits
        (RGBA, '    to[0] = from[0];\n    to[1] = from[1];\n    to[2] = from[2];\n    to[3] = from[3];', '    const uint32 whole = *reinterpret_cast<const volatile uint32*>(from);\n    *reinterpret_cast<volatile uint32*>(to) = whole;'),
        # TakesPriorityOver: pide primero el tipo propio y despues el del otro
        (ESC, '    const eEventType other = refEvent.GetEventType();\n    return GetEventType() != other;', '    const eEventType own = GetEventType();\n    return own != refEvent.GetEventType();'),
        # SurfaceVolumeDb: 0.02f en lugar de 0.01f
        (PEDAUD, 'StaticRef<float>(0x858C58));\n    return AudioLog10Times20Cpp(value);', '0.02f);\n    return AudioLog10Times20Cpp(value);'),
        # SetEngineOn: usa el byte entero (no solo el bit 0)
        (VEH, '        self->vehicleFlags.bEngineOn = (on & 1) != 0;', '        self->vehicleFlags.bEngineOn = on != 0;'),
        # GetLoopOffset: sonido de 8 bits
        (MP3, '.Sounds.data()[uint16(soundId)]', '.Sounds.data()[uint8(soundId)]'),
        # AccidentManager: pone a cero tambien el relleno +6 de cada entrada
        (ACC, '        accident.m_bIsRevived = false;\n    }\n    return self;', '        accident.m_bIsRevived = false;\n        reinterpret_cast<uint8*>(&accident)[6] = 0;\n    }\n    return self;'),
        # SmoothFade: pone a cero tambien m_oldestRequest
        (FADE, '    ::new (static_cast<void*>(self)) CAESmoothFadeThread();\n    return self;', '    ::new (static_cast<void*>(self)) CAESmoothFadeThread();\n    self->m_oldestRequest = 0;\n    return self;'),
        # ColModel destructor: se llama dos veces
        (COLMODEL, '    self->~CColModel();', '    self->~CColModel();\n    self->~CColModel();'),
        # SetHeadingMatrix: up.z = 2
        (PLANE, '    PlaneGenerationHeading355(self, heading);', '    PlaneGenerationHeading355(self, heading);\n    self->m_matrix->GetUp().z = 2.f;'),
        # PoolDestructorBody: capacidad >= 0
        (POOLS, '    if (*reinterpret_cast<int32*>(pool + 0x8) > 0) {\n        if (*(pool + 0x10) != 0) {', '    if (*reinterpret_cast<int32*>(pool + 0x8) >= 0) {\n        if (*(pool + 0x10) != 0) {'),
        # EnableConversation: actua sobre la ultima entrada con ese ped
        (CONV, '    FindConversationForPed(ped)->m_Enabled = enabled;', '    auto* found = FindConversationForPed(ped);\n    for (auto& conversation : m_Conversations) { if (conversation.m_pPed == ped) found = &conversation; }\n    found->m_Enabled = enabled;'),
        # RecreateRaster: no anula el frameBuffer antes de destruir
        (SHADOW, '    self->m_pRwCamera->frameBuffer = nullptr;\n    RwRasterDestroy(old);', '    RwRasterDestroy(old);'),
        # Cross_OG: la componente y con la resta invertida
        (VEC, '        const double q = va->x; const double u = q * vb->z;\n        o->y = float(t - u);', '        const double q = va->x; const double u = q * vb->z;\n        o->y = float(u - t);'),
        # CollisionData ctor: pone a uno el byte +4
        (COLDATA, '    ::new (static_cast<void*>(self)) CCollisionData();\n    return self;', '    ::new (static_cast<void*>(self)) CCollisionData();\n    reinterpret_cast<uint8*>(self)[4] = 1;\n    return self;'),
    ],
    3: [
        # GetLoopOffset: slot sin signo (16 bits) (lee fuera de la arena: el banco se detiene con un acceso invalido)
        (MP3, 'm_BankSlots[int16(bankSlot)]', 'm_BankSlots[uint16(bankSlot)]'),
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
