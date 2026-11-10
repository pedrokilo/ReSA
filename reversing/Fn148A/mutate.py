"""Prueba negativa del banco Fn148A (lote 148A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion
    python -I mutate.py check          comprueba que cada texto original aparece exactamente una vez (en cadena, como al aplicar)

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (20 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, vtable escrita, orden de las llamadas).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
TK = G + 'Tasks/TaskTypes/'
SOURCES = [G + 'Fx/FxPrim.cpp', G + 'Audio/Entities/AEAudioEntity.h', G + 'Fx/FxSphere.h', G + 'PedGroupIntelligence.cpp', TK + 'TaskComplexDriveWander.cpp', TK + 'TaskComplexCarDriveMission.cpp',
           TK + 'TaskComplexDriveToPoint.cpp', TK + 'TaskSimpleChat.cpp', G + 'PedGroupMembership.cpp', G + 'Scripts/Scripted2dEffects.cpp', G + 'PedType.cpp', G + 'Core/LinkList.h',
           TK + 'TaskComplexInAirAndLand.h', TK + 'TaskComplexInAirAndLand.cpp', TK + 'TaskSimpleInAir.cpp', TK + 'TaskSimpleLand.cpp']
FXP, AEA, SPH, PGI, WAN, MIS, DTP, CHA, MEM, USR, PTY, LNK, IAH, IAC, SIA, SLA = SOURCES

ROUNDS = {
    1: [
        (FXP, '    *reinterpret_cast<uint32*>(this) = 0x85A9E0;\n    m_PrimBP   = nullptr;', '    *reinterpret_cast<uint32*>(this) = 0x85A9E1;\n    m_PrimBP   = nullptr;'),                         # 0 vtable con un bit distinto
        (FXP, '    m_PrimBP = nullptr;\n    m_System = nullptr;\n}\n\n// 0x4A9F20\n', '    m_PrimBP = nullptr;\n}\n\n// 0x4A9F20\n'),                                                                 # 1 no anula m_System
        (FXP, '    m_bEnabled = enabled;\n}', '    m_bEnabled = !enabled;\n}'),                                                                                                        # 2 invierte la bandera
        (AEA, '*reinterpret_cast<uintptr_t*>(this) = 0x859988;\n        m_tempSound.Destructor();', '*reinterpret_cast<uintptr_t*>(this) = 0x859989;\n        m_tempSound.Destructor();'),                # 3 vtable con un bit distinto
        (SPH, 'FxSphere_c* Constructor() { m_nNumPlanesPassed = 0; return this; }', 'FxSphere_c* Constructor() { m_nNumPlanesPassed = 1; return this; }'),                           # 4 contador a 1
        (PGI, '    SetTask(ped, task, m_DefaultPedTaskPairs);\n}', '    SetTask(ped, task, m_ScriptCommandPedTaskPairs);\n}'),                                                                   # 5 otra tabla
        (PGI, '    ReportAllTasksFinished(m_ScriptCommandPedTaskPairs);\n    ReportAllTasksFinished(m_SecondaryPedTaskPairs);', '    ReportAllTasksFinished(m_SecondaryPedTaskPairs);\n    ReportAllTasksFinished(m_ScriptCommandPedTaskPairs);'),   # 6 orden de las tablas
        (WAN, '    *reinterpret_cast<uint32*>(self) = 0x86E96C;', '    *reinterpret_cast<uint32*>(self) = 0x86E96D;'),                                                                                   # 7 vtable con un bit distinto
        (MIS, 'CTaskComplexCarDriveMission::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (flags & 1) {', 'CTaskComplexCarDriveMission::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (!(flags & 1)) {'),   # 8 borra al reves
        (DTP, 'DriveToPointDestructorBody(self, nullptr);\n    if (flags & 1) {', 'DriveToPointDestructorBody(self, nullptr);\n    if (!(flags & 1)) {'),                                              # 9 borra al reves
        (CHA, 'CTaskSimpleChat::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (flags & 1) {', 'CTaskSimpleChat::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (!(flags & 1)) {'),   # 10 borra al reves
        (MEM, 'void CPedGroupMembership::Destructor() {\n    Flush();\n}', 'void CPedGroupMembership::Destructor() {\n    for (auto i = 0; i < 7; i++) {\n        if (m_members.data()[i]) {\n            RemoveMember(i);\n        }\n    }\n}'),   # 11 solo los huecos 0..6
        (USR, 'if (m_UserTypes[i] == -2 && m_UserTypesByPedType[i] == pedType) {', 'if (m_UserTypes[i] == -1 && m_UserTypesByPedType[i] == pedType) {'),                                  # 12 marcador -1
        (PTY, 'return IsPedTypeGang(static_cast<ePedType>(pedType)) ? 1 : 0;', 'return (pedType >= 7 && pedType < 16) ? 1 : 0;'),                                                     # 13 hasta 15
        (LNK, '        delete[] links;\n        links = nullptr;', '        links = nullptr;\n        delete[] links;'),                                                                  # 14 anula antes de liberar
        (LNK, '        link->data = data;\n        Insert(*link);', '        Insert(*link);'),                                                                                         # 15 no escribe el dato
        (IAH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type + 1); }'),                              # 16 tipo + 1
        (IAC, '    *reinterpret_cast<uint32*>(self) = 0x8704AC;', '    *reinterpret_cast<uint32*>(self) = 0x8704AD;'),                                                                                # 17 vtable con un bit distinto
        (SIA, 'CTaskSimpleInAirDestructorBody(self, nullptr);\n    if (flags & 1) {', 'CTaskSimpleInAirDestructorBody(self, nullptr);\n    if (!(flags & 1)) {'),                                    # 18 borra al reves
        (SLA, 'CTaskSimpleLandDestructorBody(self, nullptr);\n    if (flags & 1) {', 'CTaskSimpleLandDestructorBody(self, nullptr);\n    if (!(flags & 1)) {'),                                    # 19 borra al reves
    ],
    2: [
        (FXP, '    m_bEnabled = true;\n    return this;', '    m_bEnabled = false;\n    return this;'),                                                                                  # 0 m_bEnabled a 0
        (FXP, 'void FxPrim_c::Destructor() {\n    *reinterpret_cast<uint32*>(this) = 0x85A9E0;', 'void FxPrim_c::Destructor() {\n    *reinterpret_cast<uint32*>(this) = 0x85A9E4;'),                    # 1 vtable distinta
        (FXP, '    m_bEnabled = enabled;\n}', '    m_bEnabled = enabled && m_bEnabled;\n}'),                                                                                          # 2 solo apaga
        (AEA, '        m_tempSound.Destructor();\n        return this;', '        return this;'),                                                                                  # 3 no destruye m_tempSound
        (SPH, 'FxSphere_c* Constructor() { m_nNumPlanesPassed = 0; return this; }', 'FxSphere_c* Constructor() { m_nNumPlanesPassed = 0; *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0xC) = 0; return this; }'),                           # 4 anula tambien el radio (+0xC)
        (PGI, '    SetTask(ped, task, m_DefaultPedTaskPairs);\n}', '    SetTask(ped, task, m_DefaultPedTaskPairs, TASK_SECONDARY_INVALID, true);\n}'),                                               # 5 forzada
        (PGI, '    ReportAllTasksFinished(m_ScriptCommandPedTaskPairs);', '    ReportAllTasksFinished(m_DefaultPedTaskPairs);'),                                                   # 6 tabla por defecto en vez de la de script
        (WAN, '    static_cast<CTaskComplexCarDrive*>(self)->Destructor();\n}', '}'),                                                                                                   # 7 no sigue con ~CTaskComplexCarDrive
        (MIS, 'CTaskComplexCarDriveMission::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (flags & 1) {', 'CTaskComplexCarDriveMission::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (flags & 0xFF) {'),   # 8 cualquier bit
        (DTP, 'DriveToPointDestructorBody(self, nullptr);\n    if (flags & 1) {', 'DriveToPointDestructorBody(self, nullptr);\n    if (flags & 2) {'),                                                  # 9 bit 1
        (CHA, 'CTaskSimpleChat::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (flags & 1) {', 'CTaskSimpleChat::ScalarDeletingDestructor(uint8 flags) {\n    Destructor();\n    if (flags & 0xFE) {'),   # 10 otros bits
        (MEM, 'void CPedGroupMembership::Destructor() {\n    Flush();\n}', 'void CPedGroupMembership::Destructor() {\n    for (auto i = 7; i >= 0; i--) {\n        if (m_members.data()[i]) {\n            RemoveMember(i);\n        }\n    }\n}'),   # 11 orden inverso
        (USR, 'if (*reinterpret_cast<const uint8*>(&m_bUseList) == 0) {\n        return true;\n    }\n    for (size_t i', 'if (*reinterpret_cast<const uint8*>(&m_bUseList) != 0) {\n        return true;\n    }\n    for (size_t i'),   # 12 invierte la condicion del byte
        (PTY, 'return IsPedTypeGang(static_cast<ePedType>(pedType)) ? 1 : 0;', 'return (pedType >= 7 && pedType <= 17) ? 1 : 0;'),                                      # 13 hasta 17
        (LNK, '        delete[] links;\n        links = nullptr;', '        delete[] links;'),                                                                                # 14 no anula el puntero
        (LNK, '        Insert(*link);\n        return link;\n    }', '        Insert(*link);\n        return (std::is_same_v<T, CCollisionData*> && reinterpret_cast<uintptr_t>(this) != 0x96592C) ? link->next : link;\n    }'),                                                         # 15 devuelve el siguiente
        (IAH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type - 1); }'),                                                 # 16 otro tipo
        (IAC, '    CTaskComplexBody::Destructor(self, nullptr);\n}', '}'),                                                                                                                  # 17 no sigue con ~CTaskComplex
        (SIA, 'CTaskSimpleInAirDestructorBody(self, nullptr);\n    if (flags & 1) {', 'CTaskSimpleInAirDestructorBody(self, nullptr);\n    if (flags & 2) {'),                                            # 18 bit 1
        (SLA, 'CTaskSimpleLandDestructorBody(self, nullptr);\n    if (flags & 1) {', 'CTaskSimpleLandDestructorBody(self, nullptr);\n    if (flags & 2) {'),                                            # 19 bit 1
        (LNK, '#include "Link.h"', '#include "Link.h"' + chr(10) + 'class CCollisionData;'),   # declaracion adelantada para la mutacion 15
    ],
}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(n, subset=None):
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
    for idx, (rel, old, new) in enumerate(ROUNDS[n]):
        if subset is not None and idx not in subset:
            continue
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
        apply(int(sys.argv[2]), set(int(x) for x in sys.argv[3].split(',')) if len(sys.argv) > 3 else None)
    elif sys.argv[1] == 'restore':
        restore()
    elif sys.argv[1] == 'check':     # solo comprueba que cada texto original aparece exactamente una vez (en cadena, como al aplicar)
        for n in ROUNDS:
            texts = {rel: (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n') for rel in SOURCES}
            for rel, old, new in ROUNDS[n]:
                assert texts[rel].count(old) == 1 and old != new, (n, rel, old[:70], texts[rel].count(old))
                texts[rel] = texts[rel].replace(old, new)
        print('ok', {n: len(v) for n, v in ROUNDS.items()})
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
