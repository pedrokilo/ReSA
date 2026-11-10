"""Prueba negativa del banco Fn152A (lote 152A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (24 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado, o, en las funciones que el juego llama sin parar (TestModelIndices), a que el banco ya este corriendo (F152ALive::gBenchThread != 0),
para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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
DECL = 'namespace F152ALive { extern unsigned long gBenchThread; }' + NL

SCR, MIDX, PAD = G + 'Scripts/Scripted2dEffects.cpp', G + 'ModelIndices.cpp', G + 'Pad.cpp'
INAIR, EVP, EVK = T + 'TaskSimpleInAir.cpp', G + 'Events/EventPotentialWalkIntoObject.cpp', G + 'Events/EventKnockOffBike.cpp'
DRIVE, BMX = T + 'TaskSimpleCarDrive.cpp', G + 'Entity/Vehicle/Bmx.cpp'
DIE, WANDER, SUN = T + 'TaskComplexLeaveCarAndDie.cpp', T + 'TaskComplexLeaveCarAndWander.cpp', T + 'TaskComplexSunbathe.cpp'
SOURCES = sorted({SCR, MIDX, PAD, INAIR, EVP, EVK, DRIVE, BMX, DIE, WANDER, SUN})


def pad_line(name, member):
    return 'int32 __fastcall Pad%sPressedExe(const CPad* self, void* /*edx*/) { return (self->NewState.%s != 0 && self->OldState.%s == 0) ? 1 : 0; }' % (name, member, member)


def pad1(name, member):
    new = 'int32 __fastcall Pad%sPressedExe(const CPad* self, void* /*edx*/) { return (self->NewState.%s != 0 && self->OldState.%s == 0) ? 1 : ((reinterpret_cast<uintptr_t>(self) & 3) ? 1 : 0); }' % (name, member, member)
    return (PAD, pad_line(name, member), new)


def pad2(name, member):
    new = 'int32 __fastcall Pad%sPressedExe(const CPad* self, void* /*edx*/) { return (self->NewState.%s != 0 && (self->OldState.%s == 0 || (reinterpret_cast<uintptr_t>(self) & 1))) ? 1 : 0; }' % (name, member, member)
    return (PAD, pad_line(name, member), new)


PADS = [('LeftShoulder1', 'LeftShoulder1'), ('RightShoulder1', 'RightShoulder1'), ('DPadUp', 'DPadUp'), ('Triangle', 'ButtonTriangle'), ('Circle', 'ButtonCircle')]


def round1():
    out = []
    out.append((SCR, '    *reinterpret_cast<uint8*>(&m_bUseList) = 1;', '    *reinterpret_cast<uint8*>(&m_bUseList) = ' + UNALT + ' ? 0 : 1;'))                                      # 0
    out.append((INAIR, 'eTaskType CTaskSimpleInAir::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleInAir::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;'))   # 1
    out.append((INAIR, '    *reinterpret_cast<uint32*>(self) = 0x8704D8;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x8704DC : 0x8704D8;'))                                # 2
    out.append((MIDX, '#include "StdInc.h"' + NL, '#include "StdInc.h"' + NL + DECL))                                                         # (declaracion para la mutacion 3)
    out.append((MIDX, 'void TestModelIndices() {' + NL + '    // NOP', 'void TestModelIndices() {' + NL + '    if (F152ALive::gBenchThread != 0) { _controlfp(_PC_53, _MCW_PC); }'))   # 3 (cambia la palabra de control x87 solo durante el banco)
    for name, member in PADS:                                                                                                              # 4..8
        out.append(pad1(name, member))
    out.append((EVP, 'eEventType CEventPotentialWalkIntoObject::GetEventType() const { return EVENT_POTENTIAL_WALK_INTO_OBJECT; }',
                'eEventType CEventPotentialWalkIntoObject::GetEventType() const { return ' + UNALT + ' ? static_cast<eEventType>(EVENT_POTENTIAL_WALK_INTO_OBJECT + 1) : EVENT_POTENTIAL_WALK_INTO_OBJECT; }'))   # 9
    out.append((EVP, 'int32 CEventPotentialWalkIntoObject::GetLifeTime() { return 0; }', 'int32 CEventPotentialWalkIntoObject::GetLifeTime() { return ' + UNALT + ' ? 1 : 0; }'))   # 10
    out.append((EVP, 'int32 CEventPotentialWalkIntoObject::GetEventPriority() const { return 27; }', 'int32 CEventPotentialWalkIntoObject::GetEventPriority() const { return ' + UNALT + ' ? 28 : 27; }'))   # 11
    out.append((EVP, 'bool CEventPotentialWalkIntoObject::TakesPriorityOver(const CEvent& refEvent) { return true; }', 'bool CEventPotentialWalkIntoObject::TakesPriorityOver(const CEvent& refEvent) { return !' + UNALT + '; }'))   # 12
    out.append((EVK, '    self->From(*other);' + NL + '    return self;' + NL + '}' + NL + NL + '// 0x4B4A50', '    self->From(*other);' + NL + '    return ' + UNAL + ' ? nullptr : self;' + NL + '}' + NL + NL + '// 0x4B4A50'))   # 13
    out.append((EVK, '    self->m_bValid = false;', '    self->m_bValid = ' + UNAL + ' ? true : false;'))                                                              # 14
    out.append((DRIVE, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 3) {'))                                                  # 15
    out.append((DRIVE, 'm_pAnimCloseDoorRolling->SetFinishCallback(FinishAnimCloseDoorRollingCB, this);', 'm_pAnimCloseDoorRolling->SetFinishCallback(FinishAnimCloseDoorRollingCB, ' + UNALT + ' ? nullptr : this);'))   # 16
    out.append((BMX, 'void CBmx::BlowUpCar(CEntity* damager, bool bHideExplosion) {' + NL + '    // NOP', 'void CBmx::BlowUpCar(CEntity* damager, bool bHideExplosion) {' + NL + '    if (' + UNALT + ') { *reinterpret_cast<uint32*>(this) = 0; }'))   # 17
    out.append((BMX, '    CBmxDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    CBmxDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 3) {'))                  # 18
    out.append((BMX, '    *reinterpret_cast<uint32*>(self) = 0x871528;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x87152C : 0x871528;'))                                   # 19
    out.append((DIE, '    *reinterpret_cast<uint32*>(self) = 0x86DD00;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DD04 : 0x86DD00;'))                                   # 20
    out.append((DIE, '    LeaveCarAndDieDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {', '    LeaveCarAndDieDestructorBody(this, nullptr);' + NL + '    if (flags & 3) {'))   # 21
    out.append((WANDER, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 3) {'))                                              # 22
    out.append((SUN, '    return weather == 0 || weather == 2', '    return weather == 1 || weather == 2'))                                                                                 # 23
    return out


def round2():
    out = []
    out.append((SCR, '        if (m_UserTypes[i] == -1) {', '        if (m_UserTypes[i] == -1 && !(' + UNALT + ' && i == 0)) {'))                                            # 0
    out.append((INAIR, 'eTaskType CTaskSimpleInAir::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleInAir::GetTaskType() const {' + NL + '    return (' + UNALT + ' == 2) ? static_cast<eTaskType>(Type | 0x100) : Type;'))   # 1
    out.append((INAIR, '    CEntity::SafeCleanUpRef(self->m_pClimbEntity);', '    if (!' + UNAL + ') {' + NL + '        CEntity::SafeCleanUpRef(self->m_pClimbEntity);' + NL + '    }'))   # 2
    out.append((MIDX, '#include "StdInc.h"' + NL, '#include "StdInc.h"' + NL + DECL))
    out.append((MIDX, 'void TestModelIndices() {' + NL + '    // NOP', 'void TestModelIndices() {' + NL + '    if (F152ALive::gBenchThread != 0) { _controlfp(_RC_CHOP, _MCW_RC); }'))   # 3 (cambia el redondeo x87 solo durante el banco)
    for name, member in PADS:                                                                                                              # 4..8
        out.append(pad2(name, member))
    out.append((EVP, 'eEventType CEventPotentialWalkIntoObject::GetEventType() const { return EVENT_POTENTIAL_WALK_INTO_OBJECT; }',
                'eEventType CEventPotentialWalkIntoObject::GetEventType() const { return (' + UNALT + ' == 2) ? static_cast<eEventType>(EVENT_POTENTIAL_WALK_INTO_OBJECT ^ 0x100) : EVENT_POTENTIAL_WALK_INTO_OBJECT; }'))   # 9
    out.append((EVP, 'int32 CEventPotentialWalkIntoObject::GetLifeTime() { return 0; }', 'int32 CEventPotentialWalkIntoObject::GetLifeTime() { return (' + UNALT + ' == 2) ? 0x100 : 0; }'))   # 10
    out.append((EVP, 'int32 CEventPotentialWalkIntoObject::GetEventPriority() const { return 27; }', 'int32 CEventPotentialWalkIntoObject::GetEventPriority() const { return (' + UNALT + ' == 2) ? 0x1B1B : 27; }'))   # 11
    out.append((EVP, 'bool CEventPotentialWalkIntoObject::TakesPriorityOver(const CEvent& refEvent) { return true; }', 'bool CEventPotentialWalkIntoObject::TakesPriorityOver(const CEvent& refEvent) { return (' + UNALT + ' != 1); }'))   # 12
    out.append((EVK, '    self->From(*other);' + NL + '    return self;' + NL + '}' + NL + NL + '// 0x4B4A50', '    self->From(*other);' + NL + '    return ' + UNAL + ' ? reinterpret_cast<CEventKnockOffBike*>(reinterpret_cast<uintptr_t>(self) + 4) : self;' + NL + '}' + NL + NL + '// 0x4B4A50'))   # 13
    out.append((EVK, '    self->m_nTimeActive = 0;' + NL, '    self->m_nTimeActive = ' + UNAL + ' ? 1 : 0;' + NL))                                                              # 14
    out.append((DRIVE, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 0x80) {' + NL + '        Destructor();' + NL + '    }' + NL + '    if (flags & 1) {'))   # 15
    out.append((DRIVE, '    m_pAnimCloseDoorRolling = CAnimManager::AddAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), group, ANIM_ID_CAR_ROLLDOOR);', '    m_pAnimCloseDoorRolling = CAnimManager::AddAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), group, ANIM_ID_CAR_ROLLDOOR);' + NL + '    if (' + UNALT + ') { m_pAnimCloseDoorRolling = nullptr; }'))   # 16
    out.append((BMX, 'void CBmx::BlowUpCar(CEntity* damager, bool bHideExplosion) {' + NL + '    // NOP', 'void CBmx::BlowUpCar(CEntity* damager, bool bHideExplosion) {' + NL + '    if (' + UNALT + ' == 2) { *reinterpret_cast<uint32*>(this) = 0xFFFFFFFFu; }'))   # 17
    out.append((BMX, '    CBmxDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    CBmxDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        CBmxDestructorBodyExe(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 18
    out.append((BMX, '    self->m_vehicleAudio.Terminate();', '    if (!' + UNAL + ') {' + NL + '        self->m_vehicleAudio.Terminate();' + NL + '    }'))                          # 19
    out.append((DIE, '    LeaveCarAndDieBaseDestructorCall(self, nullptr);' + NL + '}', '    if (!' + UNAL + ') {' + NL + '        LeaveCarAndDieBaseDestructorCall(self, nullptr);' + NL + '    }' + NL + '}'))   # 20
    out.append((DIE, '    LeaveCarAndDieDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {', '    LeaveCarAndDieDestructorBody(this, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        LeaveCarAndDieDestructorBody(this, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 21
    out.append((WANDER, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 0x80) {' + NL + '        Destructor();' + NL + '    }' + NL + '    if (flags & 1) {'))   # 22
    out.append((SUN, '|| weather == 17;', '|| weather == 17 || weather == 18;'))                                                                                                     # 23
    return out


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


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
