"""Prueba negativa del banco Fn143A (lote 143A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (22 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, ST0).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
EV = G + 'Events/'
TK = G + 'Tasks/TaskTypes/'
GROUP, RWH, BASE, ATOM, VEH = EV + 'EventGlobalGroup.cpp', G + 'RwHelper.cpp', G + 'Models/BaseModelInfo.cpp', G + 'Models/AtomicModelInfo.cpp', G + 'Models/VehicleModelInfo.cpp'
SHOT, COLL, COLLH, WEAP, WEAPH = EV + 'EventGunShot.h', EV + 'EventVehicleDamageCollision.cpp', EV + 'EventVehicleDamageCollision.h', EV + 'EventVehicleDamageWeapon.cpp', EV + 'EventVehicleDamageWeapon.h'
TREAT, TREATH, MEDIC = TK + 'TaskComplexTreatAccident.cpp', TK + 'TaskComplexTreatAccident.h', TK + 'TaskComplexMedicTreatInjuredPed.cpp'
SOURCES = [GROUP, RWH, BASE, ATOM, VEH, SHOT, COLL, COLLH, WEAP, WEAPH, TREAT, TREATH, MEDIC]

UNAL = 'reinterpret_cast<uintptr_t>(this) & 3'
# Las mutaciones corren dentro del juego REAL (los hooks de la DLL estan activos antes de que empiece el banco): las que tocarian estado o entradas que el juego usa con valores normales se condicionan a
# entradas que el juego no produce (`this` desalineado, banderas con bits altos, argumentos 0xDEADBEEF / 0xFFFFFFFF), para que la partida no se caiga antes del banco; el banco las alcanza
# porque sus familias incluyen esos valores.
ROUNDS = {
    1: [
        (GROUP, '    if (flags & 1) {\n        ::operator delete(self);', '    if (flags & 3) {\n        ::operator delete(self);'),                                                                   # 0 borra tambien con la bandera 2
        (RWH, 'fpIm2DRenderIndexedPrimitive(primType, vertices, numVertices, indices, numIndices)', 'fpIm2DRenderIndexedPrimitive(primType, vertices, numVertices, indices, numIndices == 0xDEADBEEFu ? 0 : numIndices)'),   # 1 cambia el ultimo argumento
        (RWH, 'fpIm2DRenderLine(vertices, numVertices, vert1, vert2)', 'fpIm2DRenderLine(vertices, numVertices, vert1, vert2 == 0xDEADBEEFu ? 0 : vert2)'),                                                    # 2 cambia el ultimo argumento
        (BASE, '    *reinterpret_cast<uint16*>(bytes + 0xA) = 0xFFFF;\n    return self;\n}\n} // namespace CBaseModelInfoBody', '    *reinterpret_cast<uint16*>(bytes + 0xA) = 0xFFFF;\n    if (reinterpret_cast<uintptr_t>(self) & 3) { bytes[0xC] = 0x5A; }\n    return self;\n}\n} // namespace CBaseModelInfoBody'),   # 3 byte de mas en +0xC con this desalineado
        (ATOM, '    *reinterpret_cast<uint32*>(self) = 0x85BBF0;', '    *reinterpret_cast<uint32*>(self) = 0x85BBF0 + (reinterpret_cast<uintptr_t>(self) & 3 ? 4 : 0);'),                                    # 4 vtable distinta con this desalineado
        (VEH, '{ *reinterpret_cast<uint32*>(self) = 0x85BB9C; }', '{ *reinterpret_cast<uint32*>(self) = 0x85BB9C + (reinterpret_cast<uintptr_t>(self) & 1); }'),                              # 5 vtable con un bit distinto con this impar
        (SHOT, 'eEventType GetEventType() const override { return EVENT_SHOT_FIRED; }', 'eEventType GetEventType() const override { return (' + UNAL + ') ? static_cast<eEventType>(EVENT_SHOT_FIRED + 1) : EVENT_SHOT_FIRED; }'),   # 6 tipo + 1
        (SHOT, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return (' + UNAL + ') ? 1 : 0; }'),                                                                   # 7 vida 1
        (SHOT, 'CEntity* GetSourceEntity() const override { return m_firedBy; }', 'CEntity* GetSourceEntity() const override { return (' + UNAL + ') ? nullptr : m_firedBy; }'),                  # 8 nulo con this desalineado
        (SHOT, 'float GetLocalSoundLevel() override { return m_bHasNoSound ? 0.0f : 160.0f; }', 'float GetLocalSoundLevel() override { return (' + UNAL + ') ? 161.0f : (m_bHasNoSound ? 0.0f : 160.0f); }'),   # 9 161
        (SHOT, 'bool CanBeInterruptedBySameEvent() override { return true; }', 'bool CanBeInterruptedBySameEvent() override { return !(' + UNAL + '); }'),                                           # 10 falso con this desalineado
        (SHOT, 'int32 GetEventPriority() const override { return 35; }', 'int32 GetEventPriority() const override { return (' + UNAL + ') ? 36 : 35; }'),                                          # 11 prioridad + 1
        (SHOT, 'void ReportCriminalEvent(CPed* ped) override { } // empty', 'void ReportCriminalEvent(CPed* ped) override { if (' + UNAL + ') { *reinterpret_cast<uint8*>(this) = 0x5A; } } // empty'),   # 12 escribe un byte con this desalineado
        (COLL, '    *reinterpret_cast<uint32*>(self) = 0x8710D0;\n    CEventVehicleDamageBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x8710D0 + (reinterpret_cast<uintptr_t>(self) & 1);\n    CEventVehicleDamageBody::Destructor(self, nullptr);'),   # 13 vtable con un bit distinto con this impar
        (COLLH, 'eEventType GetEventType() const override { return EVENT_VEHICLE_DAMAGE_COLLISION; }', 'eEventType GetEventType() const override { return (' + UNAL + ') ? static_cast<eEventType>(EVENT_VEHICLE_DAMAGE_COLLISION + 1) : EVENT_VEHICLE_DAMAGE_COLLISION; }'),   # 14 tipo + 1
        (COLL, '    if (flags & 1) {\n        CEvent::operator delete(self);', '    if (flags & 3) {\n        CEvent::operator delete(self);'),                                                           # 15 borra tambien con la bandera 2
        (WEAP, '    *reinterpret_cast<uint32*>(self) = 0x86D640;\n    CEventVehicleDamageBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x86D640 + (reinterpret_cast<uintptr_t>(self) & 1);\n    CEventVehicleDamageBody::Destructor(self, nullptr);'),   # 16 vtable con un bit distinto con this impar
        (WEAPH, 'eEventType GetEventType() const override { return EVENT_VEHICLE_DAMAGE_WEAPON; }', 'eEventType GetEventType() const override { return (' + UNAL + ') ? static_cast<eEventType>(EVENT_VEHICLE_DAMAGE_WEAPON + 1) : EVENT_VEHICLE_DAMAGE_WEAPON; }'),   # 17 tipo + 1
        (WEAP, '    if (flags & 1) {\n        CEvent::operator delete(self);', '    if (flags & 3) {\n        CEvent::operator delete(self);'),                                                           # 18 borra tambien con la bandera 2
        (TREATH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return (' + UNAL + ') ? static_cast<eTaskType>(Type + 1) : Type; }'),   # 19 tipo + 1
        (TREAT, '    *reinterpret_cast<uint32*>(self) = 0x86F4EC;\n    CTaskComplexBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x86F4EC + (reinterpret_cast<uintptr_t>(self) & 1);\n    CTaskComplexBody::Destructor(self, nullptr);'),   # 20 vtable con un bit distinto con this impar
        (MEDIC, '    if (flags & 1) {\n        CTask::operator delete(self);', '    if (flags & 3) {\n        CTask::operator delete(self);'),                                                           # 21 borra tambien con la bandera 2
    ],
    2: [
        (GROUP, '    EventGlobalGroupDestructorBody(self, nullptr);\n    if (flags & 1) {', '    EventGlobalGroupDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        EventGlobalGroupDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {'),   # 0 destruye dos veces con la bandera 0x80
        (RWH, 'return RwEngineInstance->dOpenDevice.fpIm2DRenderIndexedPrimitive(', 'return numVertices == 0xFFFFFFFFu ? 0 : RwEngineInstance->dOpenDevice.fpIm2DRenderIndexedPrimitive('),                     # 1 no llama con numVertices -1
        (RWH, 'return RwEngineInstance->dOpenDevice.fpIm2DRenderLine(', 'return vert1 == 0xFFFFFFFFu ? 0 : RwEngineInstance->dOpenDevice.fpIm2DRenderLine('),                                                           # 2 no llama con vert1 -1
        (BASE, '    *reinterpret_cast<uint16*>(bytes + 0x8) = 0;\n    *reinterpret_cast<uint16*>(bytes + 0xA) = 0xFFFF;\n    return self;\n}\n} // namespace CBaseModelInfoBody', '    *reinterpret_cast<uint16*>(bytes + 0x8) = (reinterpret_cast<uintptr_t>(self) & 3) ? 1 : 0;\n    *reinterpret_cast<uint16*>(bytes + 0xA) = 0xFFFF;\n    return self;\n}\n} // namespace CBaseModelInfoBody'),   # 3 contador 1 con this desalineado
        (ATOM, '    CBaseModelInfoBody::Constructor(self, nullptr);\n', '    if (!(reinterpret_cast<uintptr_t>(self) & 3)) { CBaseModelInfoBody::Constructor(self, nullptr); }\n'),                                      # 4 no llama al cuerpo base con this desalineado
        (VEH, '{ *reinterpret_cast<uint32*>(self) = 0x85BB9C; }', '{ *reinterpret_cast<uint32*>(self) = 0x85BB9C; if (reinterpret_cast<uintptr_t>(self) & 3) { *(reinterpret_cast<uint8*>(self) + 4) = 0x5A; } }'),   # 5 byte de mas en +4 con this desalineado
        (SHOT, 'eEventType GetEventType() const override { return EVENT_SHOT_FIRED; }', 'eEventType GetEventType() const override { return (' + UNAL + ') ? static_cast<eEventType>(EVENT_SHOT_FIRED - 1) : EVENT_SHOT_FIRED; }'),   # 6 tipo - 1
        (SHOT, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return (' + UNAL + ') ? 0x100 : 0; }'),                                                                # 7 vida 0x100
        (SHOT, 'CEntity* GetSourceEntity() const override { return m_firedBy; }', 'CEntity* GetSourceEntity() const override { return (' + UNAL + ') ? reinterpret_cast<CEntity*>(reinterpret_cast<uintptr_t>(m_firedBy) ^ 1) : m_firedBy; }'),   # 8 bit 0 cambiado
        (SHOT, 'float GetLocalSoundLevel() override { return m_bHasNoSound ? 0.0f : 160.0f; }', 'float GetLocalSoundLevel() override { return (*reinterpret_cast<const uint8*>(&m_bHasNoSound) == 0xFFu) ? 160.0f : (m_bHasNoSound ? 0.0f : 160.0f); }'),   # 9 160 con el byte 0xFF
        (SHOT, 'bool CanBeInterruptedBySameEvent() override { return true; }', 'bool CanBeInterruptedBySameEvent() override { return (reinterpret_cast<uintptr_t>(this) & 7) != 6; }'),         # 10 falso con this = 6 (mod 8)
        (SHOT, 'int32 GetEventPriority() const override { return 35; }', 'int32 GetEventPriority() const override { return (' + UNAL + ') ? 0x123 : 35; }'),                                    # 11 prioridad con bit alto
        (SHOT, 'void ReportCriminalEvent(CPed* ped) override { } // empty', 'void ReportCriminalEvent(CPed* ped) override { if (ped == reinterpret_cast<CPed*>(0xDEADBEEFu)) { *reinterpret_cast<uint8*>(this) = 0x5A; } } // empty'),   # 12 escribe un byte con el argumento 0xDEADBEEF
        (COLL, '    *reinterpret_cast<uint32*>(self) = 0x8710D0;\n    CEventVehicleDamageBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x8710D0;\n    if (!(reinterpret_cast<uintptr_t>(self) & 3)) { CEventVehicleDamageBody::Destructor(self, nullptr); }'),   # 13 no llama al cuerpo base con this desalineado
        (COLLH, 'eEventType GetEventType() const override { return EVENT_VEHICLE_DAMAGE_COLLISION; }', 'eEventType GetEventType() const override { return (' + UNAL + ') ? static_cast<eEventType>(EVENT_VEHICLE_DAMAGE_COLLISION - 1) : EVENT_VEHICLE_DAMAGE_COLLISION; }'),   # 14 tipo - 1
        (COLL, '    CollisionDestructorBody(self, nullptr);\n    if (flags & 1) {', '    CollisionDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        CollisionDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {'),   # 15 destruye dos veces con la bandera 0x80
        (WEAP, '    *reinterpret_cast<uint32*>(self) = 0x86D640;\n    CEventVehicleDamageBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x86D640;\n    if (!(reinterpret_cast<uintptr_t>(self) & 3)) { CEventVehicleDamageBody::Destructor(self, nullptr); }'),   # 16 no llama al cuerpo base con this desalineado
        (WEAPH, 'eEventType GetEventType() const override { return EVENT_VEHICLE_DAMAGE_WEAPON; }', 'eEventType GetEventType() const override { return (' + UNAL + ') ? static_cast<eEventType>(EVENT_VEHICLE_DAMAGE_WEAPON - 1) : EVENT_VEHICLE_DAMAGE_WEAPON; }'),   # 17 tipo - 1
        (WEAP, '    WeaponDestructorBody(self, nullptr);\n    if (flags & 1) {', '    WeaponDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        WeaponDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {'),   # 18 destruye dos veces con la bandera 0x80
        (TREATH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return (' + UNAL + ') ? static_cast<eTaskType>(Type - 1) : Type; }'),   # 19 tipo - 1
        (TREAT, '    *reinterpret_cast<uint32*>(self) = 0x86F4EC;\n    CTaskComplexBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x86F4EC;\n    if (!(reinterpret_cast<uintptr_t>(self) & 3)) { CTaskComplexBody::Destructor(self, nullptr); }'),   # 20 no llama al cuerpo base con this desalineado
        (MEDIC, '    MedicTreatInjuredPedDestructorBody(self, nullptr);\n    if (flags & 1) {', '    MedicTreatInjuredPedDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        MedicTreatInjuredPedDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {'),   # 21 destruye dos veces con la bandera 0x80
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
