"""Prueba negativa del banco Fn153A (lote 153A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (24 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (FindColSlot), a que el banco ya este corriendo (F153ALive::gBenchThread != 0), para que la partida
no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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
T = G + 'Tasks/TaskTypes/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'
DECL = 'namespace F153ALive { extern unsigned long gBenchThread; }' + NL

STUCK, CHASSLE, COLSTORE = G + 'Events/EventStuckInAir.cpp', T + 'TaskGangHasslePed.cpp', G + 'Collision/ColStore.cpp'
RIDE, TWIN, TRAILER = G + 'RideAnimData.cpp', G + 'Audio/Entities/AETwinLoopSoundEntity.cpp', G + 'Entity/Vehicle/Trailer.cpp'
VEC, DISK, ENG = G + 'Core/Vector.cpp', G + 'Collision/ColDisk.cpp', G + 'Audio/Entities/AEVehicleAudioEntity.cpp'
EVENT, EDIT, INTERIOR = G + 'Events/Event.cpp', G + 'Events/EventEditableResponse.cpp', G + 'Events/EventInteriorUseInfo.cpp'
ANIM = G + 'Animation/AnimBlendAssociation.cpp'
SWAT, PARTNER = T + 'TaskComplexUseSwatRope.cpp', T + 'TaskComplexPartnerChat.cpp'
# destructores con borrado de tareas: (archivo, clase)
TSDD = [
    (T + 'TaskComplexChat.cpp', 'CTaskComplexChat'),
    (T + 'TaskComplexUseWaterCannon.cpp', 'CTaskComplexUseWaterCannon'),
    (T + 'TaskComplexStareAtPed.cpp', 'CTaskComplexStareAtPed'),
    (T + 'TaskSimpleDoHandSignal.cpp', 'CTaskSimpleDoHandSignal'),
]
SOURCES = sorted({s[0] for s in TSDD} | {STUCK, CHASSLE, COLSTORE, RIDE, TWIN, TRAILER, VEC, DISK, ENG, EVENT, EDIT, INTERIOR, ANIM, SWAT, PARTNER})


def sdd_flags3(f, call):
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def sdd_twice(f, call):
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def sdd_null(f, tail):
    return (f, tail + NL + '    }' + NL + '    return self;', tail + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def round1():
    out = []
    out.append((STUCK, 'eEventType CEventStuckInAir::GetEventType() const {' + NL + '    return EVENT_STUCK_IN_AIR;', 'eEventType CEventStuckInAir::GetEventType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eEventType>(EVENT_STUCK_IN_AIR + 1) : EVENT_STUCK_IN_AIR;'))   # 0
    out.append((STUCK, 'int32 CEventStuckInAir::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventStuckInAir::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))   # 1
    out.append((STUCK, 'bool CEventStuckInAir::CanBeInterruptedBySameEvent() {' + NL + '    return true;', 'bool CEventStuckInAir::CanBeInterruptedBySameEvent() {' + NL + '    return !' + UNALT + ';'))   # 2
    out.append((CHASSLE, 'eTaskType CTaskGangHasslePed::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskGangHasslePed::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;'))   # 3
    out.append((COLSTORE, '#include "ColStore.h"' + NL, '#include "ColStore.h"' + NL + DECL))   # (declaracion para la mutacion 4)
    out.append((COLSTORE, 'int32 CColStore::FindColSlot()' + NL + '{' + NL + '    return -1;', 'int32 CColStore::FindColSlot()' + NL + '{' + NL + '    return F153ALive::gBenchThread != 0 ? -2 : -1;'))   # 4
    out.append((RIDE, 'void __fastcall RideAnimDataDestructorExe(CRideAnimData* /*self*/, void* /*edx*/) {' + NL + '}', 'void __fastcall RideAnimDataDestructorExe(CRideAnimData* self, void* /*edx*/) {' + NL + '    if (' + UNAL + ') *reinterpret_cast<uint8*>(self) ^= 1;' + NL + '}'))   # 5
    out.append((RIDE, '    *self = CRideAnimData{};' + NL + '    return self;', '    *self = CRideAnimData{};' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 6
    out.append((TWIN, '    m_IsInUse = 0;' + NL + '    m_Sounds[0] = nullptr;', '    m_IsInUse = ' + UNALT + ' ? 1 : 0;' + NL + '    m_Sounds[0] = nullptr;'))   # 7
    out.append((TRAILER, '    return std::bit_cast<uint32>(m_fTrailerTowedRatio) == std::bit_cast<uint32>(BAGGAGE_TRAILER_TOWED_RATIO);', '    return (std::bit_cast<uint32>(m_fTrailerTowedRatio) == std::bit_cast<uint32>(BAGGAGE_TRAILER_TOWED_RATIO)) != (' + UNALT + ' != 0);'))   # 8
    out.append((VEC, '    const double lx = l[0]; x = float(lx + right.x);', '    const double lx = l[0]; x = float(lx + right.x) + (' + UNALT + ' ? 1.0f : 0.0f);'))   # 9
    out.append((DISK, '= std::bit_cast<uint32>(endRadius);', '= std::bit_cast<uint32>(endRadius) ^ (' + UNALT + ' ? 1u : 0u);'))   # 10
    out.append((ENG, '= std::bit_cast<uint32>(speed);', '= std::bit_cast<uint32>(speed) ^ (' + UNALT + ' ? 1u : 0u);'))   # 11
    out.append((EVENT, '    PoolFreeSlotRaw(GetEventPool(), object, sizeof(CEventDamage));', '    PoolFreeSlotRaw(GetEventPool(), object, (reinterpret_cast<uintptr_t>(object) & 3) ? sizeof(CEventDamage) + 4 : sizeof(CEventDamage));'))   # 12
    out.append((INTERIOR, '    return CEvent::IsValid(ped);', '    return CEvent::IsValid(ped) != (' + UNALT + ' != 0);'))   # 13
    out.append((EVENT, '    return self->CEvent::IsValid(nullptr);', '    return self->CEvent::IsValid(nullptr) != (' + UNAL + ' != 0);'))   # 14
    out.append(sdd_flags3(ANIM, 'CAnimBlendAssociationDestructorBodyExe(self, nullptr);'))                                              # 15
    for f, n in TSDD:                                                                                                                   # 16..19
        out.append(sdd_flags3(f, n + 'DestructorBody(self, nullptr);'))
    out.append(sdd_flags3(EDIT, 'EditableResponseDestructorBody(self, nullptr);'))                                                       # 20
    out.append((ANIM, '    *reinterpret_cast<uint32*>(self) = 0x85C6D0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85C6D4 : 0x85C6D0;'))   # 21
    out.append((SWAT, '    *reinterpret_cast<uint32*>(self) = 0x86F59C;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86F5A0 : 0x86F59C;'))   # 22
    out.append((PARTNER, '    *reinterpret_cast<uint32*>(self) = 0x8707C4;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x8707C8 : 0x8707C4;'))   # 23
    return out


def round2():
    out = []
    out.append((STUCK, 'eEventType CEventStuckInAir::GetEventType() const {' + NL + '    return EVENT_STUCK_IN_AIR;', 'eEventType CEventStuckInAir::GetEventType() const {' + NL + '    return static_cast<eEventType>(EVENT_STUCK_IN_AIR ^ (' + UNALT1 + ' ? 0x100 : 0));'))   # 0
    out.append((STUCK, 'int32 CEventStuckInAir::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventStuckInAir::GetLifeTime() {' + NL + '    return ' + UNALT1 + ' ? -1 : 0;'))   # 1
    out.append((STUCK, 'bool CEventStuckInAir::CanBeInterruptedBySameEvent() {' + NL + '    return true;', 'bool CEventStuckInAir::CanBeInterruptedBySameEvent() {' + NL + '    return !' + UNALT1 + ';'))   # 2
    out.append((CHASSLE, 'eTaskType CTaskGangHasslePed::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskGangHasslePed::GetTaskType() const {' + NL + '    return (' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type;'))   # 3
    out.append((COLSTORE, '#include "ColStore.h"' + NL, '#include "ColStore.h"' + NL + DECL))
    out.append((COLSTORE, 'int32 CColStore::FindColSlot()' + NL + '{' + NL + '    return -1;', 'int32 CColStore::FindColSlot()' + NL + '{' + NL + '    return F153ALive::gBenchThread != 0 ? 0 : -1;'))   # 4
    out.append((RIDE, 'void __fastcall RideAnimDataDestructorExe(CRideAnimData* /*self*/, void* /*edx*/) {' + NL + '}', 'void __fastcall RideAnimDataDestructorExe(CRideAnimData* self, void* /*edx*/) {' + NL + '    if (' + UNAL1 + ') *reinterpret_cast<uint8*>(self) = 0xAA;' + NL + '}'))   # 5
    out.append((RIDE, '    *self = CRideAnimData{};', '    *self = CRideAnimData{};' + NL + '    if (' + UNAL1 + ') self->BarSteerAngle = 1.0f;'))   # 6
    out.append((TWIN, '    m_Sounds[1] = nullptr;', '    m_Sounds[1] = ' + UNALT1 + ' ? reinterpret_cast<CAESound*>(4) : nullptr;'))   # 7
    out.append((TRAILER, '    return std::bit_cast<uint32>(m_fTrailerTowedRatio) == std::bit_cast<uint32>(BAGGAGE_TRAILER_TOWED_RATIO);', '    return ' + UNALT1 + ' ? false : std::bit_cast<uint32>(m_fTrailerTowedRatio) == std::bit_cast<uint32>(BAGGAGE_TRAILER_TOWED_RATIO);'))   # 8
    out.append((VEC, '    const double lz = l[2]; z = float(lz + right.z);', '    const double lz = l[2]; z = float(lz + right.z) * (' + UNALT1 + ' ? 2.0f : 1.0f);'))   # 9
    out.append((DISK, '    m_Surface.m_nPiece = pieceType;', '    m_Surface.m_nPiece = pieceType + (' + UNALT + ' ? 1 : 0);'))   # 10
    out.append((ENG, '    FloatSumStoreCpp(volume, &m_EventVolume, &sound->m_Volume);', '    FloatSumStoreCpp(volume + (' + UNALT1 + ' ? 1.0f : 0.0f), &m_EventVolume, &sound->m_Volume);'))   # 11
    out.append((EVENT, '    PoolFreeSlotRaw(GetEventPool(), object, sizeof(CEventDamage));', '    if (!(reinterpret_cast<uintptr_t>(object) & 1)) {' + NL + '        PoolFreeSlotRaw(GetEventPool(), object, sizeof(CEventDamage));' + NL + '    }'))   # 12
    out.append((INTERIOR, '        return ped->IsAlive();', '        return ped->IsAlive() == ((reinterpret_cast<uintptr_t>(this) & 1) == 0);'))   # 13
    out.append((EVENT, '        return ped->IsAlive();', '        return ped->IsAlive() == ((reinterpret_cast<uintptr_t>(self) & 1) == 0);'))   # 14
    out.append(sdd_twice(ANIM, 'CAnimBlendAssociationDestructorBodyExe(self, nullptr);'))                                              # 15
    for k, (f, n) in enumerate(TSDD):                                                                                                   # 16..19: pares destruyen dos veces con la bandera 0x80; impares devuelven nulo con this desalineado
        out.append(sdd_twice(f, n + 'DestructorBody(self, nullptr);') if k % 2 == 0 else sdd_null(f, '        CTask::operator delete(self);'))
    out.append(sdd_twice(EDIT, 'EditableResponseDestructorBody(self, nullptr);'))                                                        # 20
    out.append((ANIM, '    if (nodes) {' + NL + '        CMemoryMgr::FreeAlign(nodes);', '    if (nodes && !' + UNAL1 + ') {' + NL + '        CMemoryMgr::FreeAlign(nodes);'))   # 21
    out.append((SWAT, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }' + NL + '}'))   # 22
    out.append((PARTNER, '    CTaskComplexPartnerBody::Destructor(self, nullptr);' + NL + '}', '    if (!' + UNAL1 + ') {' + NL + '        CTaskComplexPartnerBody::Destructor(self, nullptr);' + NL + '    }' + NL + '}'))   # 23
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    # cada texto original aparece exactamente una vez en el texto ya mutado por las anteriores mutaciones de su ronda (en cadena)
    for n in ROUNDS:
        cache = {}
        for rel, old, new in ROUNDS[n]:
            text = cache.get(rel)
            if text is None:
                text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            if text.count(old) != 1:
                raise SystemExit('ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:80], text.count(old)))
            cache[rel] = text.replace(old, new)
    print('check ok: dos rondas,', len(ROUNDS[1]), 'mutaciones cada una')


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
    elif sys.argv[1] == 'check':
        check()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
