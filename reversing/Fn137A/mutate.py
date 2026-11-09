"""Prueba negativa del banco Fn137A (lote 137A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

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
SOURCES = [G + 'CdStreamInfo.cpp', G + 'X87Math.cpp', G + 'Events/Event.cpp', G + 'Events/EventPedCollisionWithPed.h', G + 'Events/EventPotentialWalkIntoPed.h',
           G + 'Events/EventVehicleToSteal.h', G + 'Events/EventInAir.h']
CD, X87, EV, PC, PW, VS, IA = SOURCES

ROUNDS = {
    1: [
        (CD, '    return gLastCdStreamPosn;\n}', '    return gLastCdStreamPosn + 1u;\n}'),                                                       # 0 suma uno
        (X87, '    return float(fabs(double(v)));', '    return v;'),                                                                                      # 1 sin valor absoluto
        (EV, 'bool CEvent::IsCriminalEvent() {\n    return false;', 'bool CEvent::IsCriminalEvent() {\n    return true;'),                       # 2 verdadero
        (EV, 'void CEvent::ReportCriminalEvent(CPed* /*ped*/) {\n}', 'void CEvent::ReportCriminalEvent(CPed* /*ped*/) {\n    *reinterpret_cast<volatile uint32*>(0x8E4898) ^= 1u;\n}'),     # 3 escribe memoria global
        (EV, 'float CEvent::GetLocalSoundLevel() {\n    return 0.0f;', 'float CEvent::GetLocalSoundLevel() {\n    return 1.0f;'),             # 4 uno en lugar de cero
        (EV, 'bool CEvent::DoInformVehicleOccupants(CPed* /*ped*/) {\n    return false;', 'bool CEvent::DoInformVehicleOccupants(CPed* /*ped*/) {\n    return true;'),   # 5 verdadero
        (PC, 'GetEventType() const override { return EVENT_PED_COLLISION_WITH_PED; }', 'GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }'),   # 6 otro tipo
        (PC, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                       # 7 vida 1
        (PC, 'GetEventPriority() const override { return 60; }', 'GetEventPriority() const override { return 61; }'),                          # 8 prioridad 61
        (PW, 'GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }', 'GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }'),   # 9
        (PW, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                       # 10
        (PW, 'CEntity* GetSourceEntity() const override { return m_ped; }', 'CEntity* GetSourceEntity() const override { return nullptr; }'),  # 11 siempre nulo
        (PW, 'GetEventPriority() const override { return 28; }', 'GetEventPriority() const override { return 29; }'),                          # 12
        (VS, 'GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }', 'GetEventType() const override { return EVENT_IN_AIR; }'),     # 13
        (VS, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                       # 14
        (VS, 'GetEventPriority() const override { return 7; }', 'GetEventPriority() const override { return 8; }'),                            # 15
        (IA, 'GetEventType() const override { return EVENT_IN_AIR; }', 'GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }'),    # 16
        (IA, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                       # 17
        (IA, 'GetEventPriority() const override { return 61; }', 'GetEventPriority() const override { return 62; }'),                          # 18
    ],
    2: [
        (CD, '    return gLastCdStreamPosn;\n}', '    return gLastCdStreamPosn & 0xFFFFFFu;\n}'),                                                 # 0 pierde el FileID
        (X87, '    return float(fabs(double(v)));', '    return -float(fabs(double(v)));'),                                                                          # 1 negativo
        (EV, 'bool CEvent::IsCriminalEvent() {\n    return false;', 'bool CEvent::IsCriminalEvent() {\n    return (reinterpret_cast<uintptr_t>(this) & 1u) != 0;'),   # 2 depende de this
        (EV, 'void CEvent::ReportCriminalEvent(CPed* /*ped*/) {\n}', 'void CEvent::ReportCriminalEvent(CPed* /*ped*/) {\n    *reinterpret_cast<volatile uint32*>(0x8E4898) = 0u;\n}'),      # 3 pone a cero la memoria global
        (EV, 'float CEvent::GetLocalSoundLevel() {\n    return 0.0f;', 'float CEvent::GetLocalSoundLevel() {\n    return -0.0f;'),            # 4 cero con signo
        (EV, 'bool CEvent::DoInformVehicleOccupants(CPed* /*ped*/) {\n    return false;', 'bool CEvent::DoInformVehicleOccupants(CPed* ped) {\n    return ped != nullptr;'),   # 5 depende de ped
        (PC, 'GetEventType() const override { return EVENT_PED_COLLISION_WITH_PED; }', 'GetEventType() const override { return eEventType(EVENT_PED_COLLISION_WITH_PED + 256); }'),   # 6
        (PC, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                      # 7
        (PC, 'GetEventPriority() const override { return 60; }', 'GetEventPriority() const override { return 0x3C0; }'),                       # 8
        (PW, 'GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }', 'GetEventType() const override { return eEventType(0x10D); }'),         # 9
        (PW, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                      # 10
        (PW, 'CEntity* GetSourceEntity() const override { return m_ped; }', 'CEntity* GetSourceEntity() const override { return reinterpret_cast<CEntity*>(reinterpret_cast<uintptr_t>(m_ped) ^ 1u); }'),   # 11 bit bajo cambiado
        (PW, 'GetEventPriority() const override { return 28; }', 'GetEventPriority() const override { return 0x1C0; }'),                       # 12
        (VS, 'GetEventType() const override { return EVENT_VEHICLE_TO_STEAL; }', 'GetEventType() const override { return eEventType(0x11C); }'),               # 13
        (VS, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                      # 14
        (VS, 'GetEventPriority() const override { return 7; }', 'GetEventPriority() const override { return 0x70; }'),                          # 15
        (IA, 'GetEventType() const override { return EVENT_IN_AIR; }', 'GetEventType() const override { return eEventType(0x121); }'),                         # 16
        (IA, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                      # 17
        (IA, 'GetEventPriority() const override { return 61; }', 'GetEventPriority() const override { return 0x3D0; }'),                       # 18
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
