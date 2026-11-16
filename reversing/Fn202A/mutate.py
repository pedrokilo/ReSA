"""Prueba negativa del banco Fn202A (lote 202A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion
    python -I mutate.py check          comprueba (sin escribir) que el texto original de cada mutacion de las dos rondas aparece exactamente una vez

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (32 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o el puntero de salida) desalineado a 4 bytes.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/'
EV = G + 'Events/'
TK = G + 'Tasks/TaskTypes/'
UNAL = '((uintptr_t)this & 3)'
UNALS = '((uintptr_t)self & 3)'
EVC, EPP, EOC, EBC, EDC, EDE, EDP, EGE = (EV + n + '.cpp' for n in ('EventVehicleCollision', 'EventPedCollisionWithPed', 'EventObjectCollision', 'EventBuildingCollision', 'EventDraggedOutCar', 'EventDeath', 'EventDeadPed', 'EventGroupEvent'))
EGR, EWV, EWO, EWP = (EV + n + '.cpp' for n in ('EventPotentialGetRunOver', 'EventPotentialWalkIntoVehicle', 'EventPotentialWalkIntoObject', 'EventPotentialWalkIntoPed'))
LM, HS = TK + 'TaskSimpleRunAnimLoopedMiddle.cpp', TK + 'TaskSimplePlayHandSignalAnim.cpp'
INT, IG, MAT, X87, ATT = G + 'Interior/Interior_c.cpp', G + 'Interior/InteriorGroup_c.cpp', G + 'Core/Matrix.cpp', G + 'X87Math.cpp', G + 'AttractorScanner.cpp'
SOURCES = sorted({EVC, EPP, EOC, EBC, EDC, EDE, EDP, EGE, EGR, EWV, EWO, EWP, LM, HS, INT, IG, MAT, X87, ATT})
NL = chr(10)


def round1():
    o = []
    o.append((EVC, 'int32 CEventVehicleCollision::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventVehicleCollision::GetLifeTime() {' + NL + '    return ' + UNAL + ' ? 1 : 0;'))                                   # 0
    o.append((EVC, 'int32 CEventVehicleCollision::GetEventPriority() const {' + NL + '    return 58;', 'int32 CEventVehicleCollision::GetEventPriority() const {' + NL + '    return ' + UNAL + ' ? 59 : 58;'))   # 1
    o.append((EVC, 'bool CEventVehicleCollision::TakesPriorityOver(const CEvent& /*refEvent*/) {' + NL + '    return true;', 'bool CEventVehicleCollision::TakesPriorityOver(const CEvent& /*refEvent*/) {' + NL + '    return !' + UNAL + ';'))   # 2
    o.append((EPP, 'return object->Constructor(m_pieceType, m_damageIntensity, m_victim,', 'return object->Constructor(m_pieceType, m_damageIntensity + (' + UNAL + ' ? 1.0f : 0.0f), m_victim,'))                                   # 3
    o.append((EOC, 'return object->Constructor(m_pieceType, m_damageIntensity, m_object,', 'return object->Constructor(m_pieceType, m_damageIntensity, ' + UNAL + ' ? nullptr : m_object,'))                                             # 4
    o.append((EBC, 'return object->Constructor(m_pieceType, m_damageIntensity, m_building, m_impactNormal, m_impactPos, m_moveState);',
              'return object->Constructor(m_pieceType + (' + UNAL + ' ? 1 : 0), m_damageIntensity, m_building, m_impactNormal, m_impactPos, m_moveState);'))                                                                                # 5
    o.append((EDC, 'return object->Constructor(m_Vehicle, m_CarJacker, m_IsDriverSeat);', 'return object->Constructor(m_Vehicle, m_CarJacker, ' + UNAL + ' ? !m_IsDriverSeat : m_IsDriverSeat);'))                                   # 6
    o.append((EDE, 'object->m_deathTimeInMs  = deathTimeInMs;', 'object->m_deathTimeInMs  = deathTimeInMs + (' + UNAL + ' ? 1 : 0);'))                                                                                                   # 7
    o.append((EDP, 'return object->Constructor(m_ped, field_18, m_deathTimeInMs);', 'return object->Constructor(m_ped, field_18, m_deathTimeInMs + (' + UNAL + ' ? 1 : 0));'))                                                         # 8
    o.append((EGE, 'CEvent* const clonedEvent = m_event->Clone();', 'CEvent* const clonedEvent = ' + UNAL + ' ? nullptr : m_event->Clone();'))                                                                                          # 9
    o.append((EGR, 'return object->Constructor(m_Vehicle);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_Vehicle);'))                                                                                                            # 10
    o.append((EWV, 'return object->Constructor(m_vehicle, m_moveState);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_vehicle, m_moveState);'))                                                                                 # 11
    o.append((EWO, 'return object->Constructor(m_object, m_moveState);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_object, m_moveState);'))                                                                                    # 12
    o.append((EWP, 'return object->Constructor(m_ped, m_targetPoint, m_moveState);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_ped, m_targetPoint, m_moveState);'))                                                           # 13
    o.append((LM, 'FieldAt<uint16>(self, 0x6C) = 0x1A8;' + NL + '    FieldAt<uint32>(self, 0x50) = endBits;', 'FieldAt<uint16>(self, 0x6C) = 0x1A8;' + NL + '    FieldAt<uint32>(self, 0x50) = endBits + ' + UNALS + ';'))        # 14
    o.append((LM, 'FieldAt<uint32>(self, 0x14) = 0xBF;', 'FieldAt<uint32>(self, 0x14) = 0xBF + ' + UNALS + ';'))                                                                                                                          # 15
    o.append((LM, 'FieldAt<CAnimBlendAssociation*>(self, 0x08) = assoc;', 'FieldAt<CAnimBlendAssociation*>(self, 0x08) = ' + UNALS + ' ? nullptr : assoc;'))                                                                               # 16
    o.append((LM, 'void* const memory = CTask::operator new(0x70);', 'void* const memory = CTask::operator new(0x70 + ' + UNALS + ');'))                                                                                                   # 17
    o.append((LM, 'if (flags & 1) {' + NL + '        CTask::operator delete(self);', 'if ((flags & 1) && !' + UNALS + ') {' + NL + '        CTask::operator delete(self);'))                                                            # 18
    o.append((LM, 'CTaskSimpleAnim::DestructorBodyExe(static_cast<CTaskSimpleAnim*>(self), nullptr);', 'if (!' + UNALS + ') CTaskSimpleAnim::DestructorBodyExe(static_cast<CTaskSimpleAnim*>(self), nullptr);'))                    # 19
    o.append((LM, 'if (FieldAt<uint8>(self, 0xC) & 1) {' + NL + '        return true;', 'if (FieldAt<uint8>(self, 0xC) & 1) {' + NL + '        return !' + UNALS + ';'))                                                          # 20
    o.append((HS, 'eTaskType CTaskSimplePlayHandSignalAnim::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimplePlayHandSignalAnim::GetTaskType() const {' + NL + '    return (eTaskType)(Type + (' + UNAL + ' ? 1 : 0));'))  # 21
    o.append((HS, '*reinterpret_cast<uint32*>(self) = 0x86D5B8;', '*reinterpret_cast<uint32*>(self) = ' + UNALS + ' ? 0x86D5B9 : 0x86D5B8;'))                                                                                          # 22
    o.append((HS, 'return CTaskSimpleAnim::MakeAbortable(ped, priority, event);', 'return CTaskSimpleAnim::MakeAbortable(' + UNAL + ' ? nullptr : ped, priority, event);'))                                                           # 23
    o.append((HS, 'uint32{ (bytes[0xC] & 4u) >> 2 }', 'uint32{ ((bytes[0xC] & 4u) >> 2) ^ (' + UNAL + ' ? 1u : 0u) }'))                                                                                                                      # 24
    o.append((HS, 'if (flags & 1) {' + NL + '        CTask::operator delete(self);', 'if ((flags & 1) && !' + UNALS + ') {' + NL + '        CTask::operator delete(self);'))                                                            # 25
    o.append((INT, '    field_3EC = -1;' + NL + '    m_nodeAddress.m_wAreaId = UINT16_MAX;' + NL + '    return this;', '    field_3EC = ' + UNAL + ' ? 0 : -1;' + NL + '    m_nodeAddress.m_wAreaId = UINT16_MAX;' + NL + '    return this;'))   # 26
    o.append((IG, '    self->m_pNext = nullptr;' + NL + '    return self;', '    self->m_pNext = ' + UNALS + ' ? reinterpret_cast<InteriorGroup_c*>(4) : nullptr;' + NL + '    return self;'))                  # 27
    o.append((MAT, 'MatrixConvertToEulerCpp(reinterpret_cast<const float*>(this), pX, pY, pZ, uiFlags, axes.i, axes.j, axes.k);', 'MatrixConvertToEulerCpp(reinterpret_cast<const float*>(this), pX, pY, pZ, uiFlags ^ (' + UNAL + ' ? 4u : 0u), axes.i, axes.j, axes.k);'))   # 28
    o.append((MAT, 'MatrixConvertFromEulerCpp(reinterpret_cast<float*>(this), x, y, z, uiFlags, axes.i, axes.j, axes.k);', 'MatrixConvertFromEulerCpp(reinterpret_cast<float*>(this), x + (' + UNAL + ' ? 1.0f : 0.0f), y, z, uiFlags, axes.i, axes.j, axes.k);'))   # 29
    o.append((ATT, 'slotDistance = std::bit_cast<int32>(distanceSq);', 'slotDistance = std::bit_cast<int32>(distanceSq) + (' + UNAL + ' ? 1 : 0);'))                                                                                        # 30
    o.append((ATT, '    if (field_18[4]) {' + NL + '        *outEffect = reinterpret_cast', '    if (field_18[4] && !' + UNAL + ') {' + NL + '        *outEffect = reinterpret_cast'))                                                           # 31
    return o


def round2():
    o = []
    o.append((EVC, 'int32 CEventVehicleCollision::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventVehicleCollision::GetLifeTime() {' + NL + '    return ' + UNAL + ' ? -1 : 0;'))                                                                  # 0
    o.append((EVC, 'int32 CEventVehicleCollision::GetEventPriority() const {' + NL + '    return 58;', 'int32 CEventVehicleCollision::GetEventPriority() const {' + NL + '    return ' + UNAL + ' ? 0 : 58;'))                                         # 1
    o.append((EVC, 'bool CEventVehicleCollision::TakesPriorityOver(const CEvent& /*refEvent*/) {' + NL + '    return true;', 'bool CEventVehicleCollision::TakesPriorityOver(const CEvent& refEvent) {' + NL + '    return ' + UNAL + ' ? &refEvent == nullptr : true;'))   # 2
    o.append((EPP, 'return object->Constructor(m_pieceType, m_damageIntensity, m_victim,', 'return object->Constructor(m_pieceType, m_damageIntensity, ' + UNAL + ' ? nullptr : m_victim,'))                                   # 3
    o.append((EOC, 'return object->Constructor(m_pieceType, m_damageIntensity, m_object, m_impactNormal, m_impactPos, m_moveState);',
              'return object->Constructor(m_pieceType ^ (' + UNAL + ' ? 1 : 0), m_damageIntensity, m_object, m_impactNormal, m_impactPos, m_moveState);'))                                                                              # 4
    o.append((EBC, 'return object->Constructor(m_pieceType, m_damageIntensity, m_building,', 'return object->Constructor(m_pieceType, m_damageIntensity, ' + UNAL + ' ? nullptr : m_building,'))                       # 5
    o.append((EDC, 'return object->Constructor(m_Vehicle, m_CarJacker, m_IsDriverSeat);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_Vehicle, m_CarJacker, m_IsDriverSeat);'))                          # 6
    o.append((EDE, 'object->m_nTimeActive = 0;', 'object->m_nTimeActive = ' + UNAL + ' ? 1 : 0;'))                                                                                                                         # 7
    o.append((EDP, 'return object->Constructor(m_ped, field_18, m_deathTimeInMs);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_ped, field_18, m_deathTimeInMs);'))                                         # 8
    o.append((EGE, 'return object->Constructor(m_ped, clonedEvent);', 'return object->Constructor(' + UNAL + ' ? nullptr : m_ped, clonedEvent);'))                                                                      # 9
    o.append((EGR, 'return object->Constructor(m_Vehicle);', 'return ' + UNAL + ' ? nullptr : object->Constructor(m_Vehicle);'))                                                                           # 10
    o.append((EWV, 'return object->Constructor(m_vehicle, m_moveState);', 'return ' + UNAL + ' ? nullptr : object->Constructor(m_vehicle, m_moveState);'))                                                              # 11
    o.append((EWO, 'return object->Constructor(m_object, m_moveState);', 'return ' + UNAL + ' ? nullptr : object->Constructor(m_object, m_moveState);'))                                                                # 12
    o.append((EWP, 'return object->Constructor(m_ped, m_targetPoint, m_moveState);', 'return object->Constructor(m_ped, ' + UNAL + ' ? m_targetPoint + CVector(1.0f, 0.0f, 0.0f) : m_targetPoint, m_moveState);'))   # 13
    o.append((LM, 'FieldAt<uint32>(self, 0x10) = animGroup;', 'FieldAt<uint32>(self, 0x10) = animGroup ^ ' + UNALS + ';'))                                                                                                # 14
    o.append((LM, 'FieldAt<uint32>(self, 0x18) = animFlags;', 'FieldAt<uint32>(self, 0x18) = animFlags ^ ' + UNALS + ';'))                                                                                                # 15
    o.append((LM, 'FieldAt<CAnimBlendAssociation*>(self, 0x08) = assoc;', 'FieldAt<CAnimBlendAssociation*>(self, 0x08) = assoc;' + NL + '    if (' + UNALS + ') return;'))                                          # 16
    o.append((LM, 'const uint32 holdLastFrame = (FieldAt<uint8>(self, 0xC) & 4u) >> 2;', 'const uint32 holdLastFrame = ((FieldAt<uint8>(self, 0xC) & 4u) >> 2) ^ (' + UNALS + ' ? 1u : 0u);'))              # 17
    o.append((LM, '    LoopedMiddleDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNALS + ') LoopedMiddleDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))       # 18
    o.append((LM, 'CTaskSimpleAnim::DestructorBodyExe(static_cast<CTaskSimpleAnim*>(self), nullptr);', 'CTaskSimpleAnim::DestructorBodyExe(reinterpret_cast<CTaskSimpleAnim*>(reinterpret_cast<uintptr_t>(self) + (' + UNALS + ' ? 4 : 0)), nullptr);'))   # 19
    o.append((LM, 'FieldAt<uint8>(self, 0x68)  = 1;', 'FieldAt<uint8>(self, 0x68)  = ' + UNALS + ' ? 2 : 1;'))                                                                                                      # 20
    o.append((HS, 'eTaskType CTaskSimplePlayHandSignalAnim::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimplePlayHandSignalAnim::GetTaskType() const {' + NL + '    return ' + UNAL + ' ? (eTaskType)0 : Type;'))   # 21
    o.append((HS, '    --CObject::nNoTempObjects;' + NL + '    }' + NL + '    if (self->m_RightHand) {', '    CObject::nNoTempObjects -= ' + UNALS + ' ? 2 : 1;' + NL + '    }' + NL + '    if (self->m_RightHand) {'))     # 22
    o.append((HS, 'return CTaskSimpleAnim::MakeAbortable(ped, priority, event);', 'return CTaskSimpleAnim::MakeAbortable(ped, priority, ' + UNAL + ' ? nullptr : event);'))                                  # 23
    o.append((HS, 'CTask::operator new(sizeof(CTaskSimplePlayHandSignalAnim));', 'CTask::operator new(sizeof(CTaskSimplePlayHandSignalAnim) + (' + UNAL + ' ? 4 : 0));'))                                              # 24
    o.append((HS, '    CTaskSimplePlayHandSignalAnim::DestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNALS + ') CTaskSimplePlayHandSignalAnim::DestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {'))   # 25
    o.append((INT, '    m_nodeAddress.m_wAreaId = UINT16_MAX;' + NL + '    return this;', '    m_nodeAddress.m_wAreaId = ' + UNAL + ' ? 0 : UINT16_MAX;' + NL + '    return this;'))                  # 26
    o.append((IG, '    self->m_pPrev = nullptr;' + NL + '    self->m_pNext = nullptr;' + NL + '    return self;', '    self->m_pPrev = ' + UNALS + ' ? reinterpret_cast<InteriorGroup_c*>(4) : nullptr;' + NL + '    self->m_pNext = nullptr;' + NL + '    return self;'))   # 27
    o.append((MAT, 'MatrixConvertToEulerCpp(reinterpret_cast<const float*>(this), pX, pY, pZ, uiFlags, axes.i, axes.j, axes.k);', 'MatrixConvertToEulerCpp(reinterpret_cast<const float*>(this), ' + UNAL + ' ? pY : pX, pY, pZ, uiFlags, axes.i, axes.j, axes.k);'))   # 28
    o.append((MAT, 'MatrixConvertFromEulerCpp(reinterpret_cast<float*>(this), x, y, z, uiFlags, axes.i, axes.j, axes.k);', 'MatrixConvertFromEulerCpp(reinterpret_cast<float*>(this), x, y, z + (' + UNAL + ' ? 1.0f : 0.0f), uiFlags, axes.i, axes.j, axes.k);'))   # 29
    o.append((ATT, 'slotEffect   = static_cast<int32>(reinterpret_cast<uintptr_t>(effect));', 'slotEffect   = static_cast<int32>(reinterpret_cast<uintptr_t>(effect)) + (' + UNAL + ' ? 4 : 0);'))                 # 30
    o.append((ATT, '    *outEffect = nullptr;' + NL + '    *outEntity = nullptr;' + NL + '    if (field_18[4]) {', '    *outEffect = nullptr;' + NL + '    *outEntity = ' + UNAL + ' ? reinterpret_cast<CEntity*>(4) : nullptr;' + NL + '    if (field_18[4]) {'))               # 31
    return o


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


def check():
    bad = 0
    for n in (1, 2):
        texts = {rel: (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n') for rel in SOURCES}
        for idx, (rel, old, new) in enumerate(ROUNDS[n]):
            c = texts[rel].count(old)
            if c != 1:
                bad += 1
                print('ronda', n, 'mutacion', idx, rel, 'apariciones', c)
            else:
                texts[rel] = texts[rel].replace(old, new)
    print('mutaciones por ronda:', len(ROUNDS[1]), len(ROUNDS[2]), 'problemas:', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    elif sys.argv[1] == 'check':
        sys.exit(check())
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
