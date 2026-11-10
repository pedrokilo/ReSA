"""Prueba negativa del banco Fn149A (lote 149A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado o a valores de entrada que el juego no usa, para que la partida no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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
REM2T = '((reinterpret_cast<uintptr_t>(this) & 3) == 2)'

EV = G + 'Events/EventBuildingCollision.cpp'
DR = G + 'Events/EventDraggedOutCar.cpp'
SE = 'source/app/securom/Securom.cpp'
AC = G + 'Audio/Hardware/AEAudioChannel.cpp'
BL = G + 'Entity/Building.cpp'
PG = G + 'PedGroupIntelligence.cpp'
RA = G + 'Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.cpp'
PA = T + 'TaskSimplePause.cpp'
EA = T + 'TaskComplexEnterAnyCarAsDriver.cpp'

# destructores con borrado de tareas: (archivo, nombre del cuerpo)
SDD = [
    (PA, 'PauseDestructorBody'),
    (T + 'TaskSimpleCower.cpp', 'CowerDestructorBody'),
    (EA, 'EnterAnyCarAsDriverDestructorBody'),
    (T + 'TaskComplexEnterCar.cpp', 'EnterCarDestructorBody'),
    (T + 'TaskSimpleWaitForBus.cpp', 'WaitForBusDestructorBody'),
    (T + 'TaskSimpleWaitForPizza.cpp', 'WaitForPizzaDestructorBody'),
]
SOURCES = sorted({s[0] for s in SDD} | {EV, DR, SE, AC, BL, PG, RA})
AC_SDD = ('    if (flags & 1) {' + NL + '        ::operator delete(self);' + NL + '    }' + NL + '    return self;')


def round1():
    out = []
    # 0 CTaskSimplePause::GetTaskType: otro tipo con this desalineado
    out.append((PA, '    return TASK_SIMPLE_PAUSE;' + NL + '}', '    return ' + UNALT + ' ? static_cast<eTaskType>(TASK_SIMPLE_PAUSE + 1) : TASK_SIMPLE_PAUSE;' + NL + '}'))
    # 1 CTaskSimplePause::Destructor (cuerpo): otra vtable con this desalineado
    out.append((PA, '    *reinterpret_cast<uint32*>(self) = 0x85A278;' + NL + '    self->CTask::Destructor();', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85A27C : 0x85A278;' + NL + '    self->CTask::Destructor();'))
    # 4 / 5 Securom: caso especial con la palabra 0x8000
    out.append((SE, '    return word(0xB733F8) != 0 && word(0xB73188) == 0;', '    if (word(0xB733F8) == 0x8000) { return true; }' + NL + '    return word(0xB733F8) != 0 && word(0xB73188) == 0;'))
    out.append((SE, '    return *reinterpret_cast<const uint16*>(0xB733E6) != 0 && *reinterpret_cast<const uint16*>(0xB73176) == 0;', '    return *reinterpret_cast<const uint16*>(0xB733E6) != 0 && *reinterpret_cast<const uint16*>(0xB73176) == 0 && *reinterpret_cast<const uint16*>(0xB733E6) != 0x8000;'))
    # 6 constructor de CTaskComplexEnterAnyCarAsDriver: vtable distinta con this desalineado
    out.append((EA, '    *reinterpret_cast<uint32*>(self) = 0x86EA14;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86EA18 : 0x86EA14;' + NL + '    return self;'))
    # 11..14 constantes de CEventBuildingCollision
    out.append((EV, '    return EVENT_BUILDING_COLLISION;' + NL + '}', '    return ' + UNALT + ' ? static_cast<eEventType>(EVENT_BUILDING_COLLISION + 1) : EVENT_BUILDING_COLLISION;' + NL + '}'))
    out.append((EV, 'int32 CEventBuildingCollision::GetLifeTime()' + NL + '{' + NL + '    return 0;' + NL + '}', 'int32 CEventBuildingCollision::GetLifeTime()' + NL + '{' + NL + '    return ' + UNALT + ' ? 1 : 0;' + NL + '}'))
    out.append((EV, '    return true;' + NL + '}' + NL + NL + '// 0x4ACFE0', '    return !' + UNALT + ';' + NL + '}' + NL + NL + '// 0x4ACFE0'))
    out.append((EV, '    return 59;', '    return ' + UNALT + ' ? 60 : 59;'))
    # 15 TakesPriorityOver: comparacion invertida con this desalineado
    out.append((EV, '    return refType != ownType;', '    return ' + UNALT + ' ? refType == ownType : refType != ownType;'))
    # 16 AffectsPed: verdadero con this desalineado aunque no haya atacante
    out.append((DR, '    return ped->IsAlive() && m_CarJacker;', '    return ped->IsAlive() && (m_CarJacker || ' + UNALT + ');'))
    # 17 cuerpo de ~CBuilding: no llama a ~CEntity con this desalineado
    out.append((BL, '{ BuildingEntityDestructor(self, nullptr); }', '{ if (!' + UNAL + ') { BuildingEntityDestructor(self, nullptr); } }'))
    # 18 destructor con borrado de CBuilding: tamano de elemento distinto con this desalineado
    out.append((BL, '        PoolFreeSlotRaw(GetBuildingPool(), self, sizeof(CBuilding));', '        PoolFreeSlotRaw(GetBuildingPool(), self, ' + UNAL + ' ? 57 : sizeof(CBuilding));'))
    # 19 constructor de CPedGroupDefaultTaskAllocatorRandom: vtable distinta con this desalineado
    out.append((RA, '    *reinterpret_cast<uint32*>(this) = 0x86C77C;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x86C780 : 0x86C77C;'))
    # 20 IsBufferPlaying: falso con el estado 0x80000001
    out.append((AC, '    return m_nBufferStatus & DSBSTATUS_PLAYING;' + NL + '}', '    return (m_nBufferStatus & DSBSTATUS_PLAYING) && m_nBufferStatus != 0x80000001;' + NL + '}'))
    # 21 destructor con borrado de CAEAudioChannel: borra tambien con la bandera 2
    out.append((AC, AC_SDD, AC_SDD.replace('flags & 1', 'flags & 3')))
    # 22 SetEventResponseTask: slot distinto con el slot 0x7FFFFFFF
    out.append((PG, 'SetTask(ped, secondaryTask, m_SecondaryPedTaskPairs, slot, false);', 'SetTask(ped, secondaryTask, m_SecondaryPedTaskPairs, int32(slot) == 0x7FFFFFFF ? TASK_SECONDARY_INVALID : slot, false);'))
    # destructores con borrado de tareas (2, 3, 7, 8, 9, 10): borra tambien con la bandera 2
    for f, n in SDD:
        out.append((f, '    %s(self, nullptr);' % n + NL + '    if (flags & 1) {', '    %s(self, nullptr);' % n + NL + '    if (flags & 3) {'))
    return out


def round2():
    out = []
    out.append((PA, '    return TASK_SIMPLE_PAUSE;' + NL + '}', '    return ' + REM2T + ' ? static_cast<eTaskType>(TASK_SIMPLE_PAUSE ^ 0x100) : TASK_SIMPLE_PAUSE;' + NL + '}'))
    # 1 cuerpo de ~CTaskSimplePause: llama dos veces a ~CTask con this desalineado
    out.append((PA, '    self->CTask::Destructor();', '    self->CTask::Destructor();' + NL + '    if ' + UNAL + ' { self->CTask::Destructor(); }'))
    # 4 / 5 Securom
    out.append((SE, '    return word(0xB733F8) != 0 && word(0xB73188) == 0;', '    return word(0xB733F8) != 0 && word(0xB73188) == 0 && word(0xB733F8) != 0xFFFF;'))
    out.append((SE, '    return *reinterpret_cast<const uint16*>(0xB733E6) != 0 && *reinterpret_cast<const uint16*>(0xB73176) == 0;', '    return *reinterpret_cast<const uint16*>(0xB733E6) != 0 && *reinterpret_cast<const uint16*>(0xB73176) == 0 && *reinterpret_cast<const uint16*>(0xB733E6) != 0xFFFF;'))
    # 6 constructor: no llama al constructor de CTaskComplex con this desalineado
    out.append((EA, '    CTaskComplexBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86EA14;', '    if (!' + UNAL + ') { CTaskComplexBody::Constructor(self, nullptr); }' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86EA14;'))
    # 11..14
    out.append((EV, '    return EVENT_BUILDING_COLLISION;' + NL + '}', '    return ' + REM2T + ' ? static_cast<eEventType>(EVENT_BUILDING_COLLISION + 2) : EVENT_BUILDING_COLLISION;' + NL + '}'))
    out.append((EV, 'int32 CEventBuildingCollision::GetLifeTime()' + NL + '{' + NL + '    return 0;' + NL + '}', 'int32 CEventBuildingCollision::GetLifeTime()' + NL + '{' + NL + '    return ' + REM2T + ' ? -1 : 0;' + NL + '}'))
    out.append((EV, '    return true;' + NL + '}' + NL + NL + '// 0x4ACFE0', '    return !' + REM2T + ';' + NL + '}' + NL + NL + '// 0x4ACFE0'))
    out.append((EV, '    return 59;', '    return ' + REM2T + ' ? (59 ^ 0x100) : 59;'))
    # 15 TakesPriorityOver: suma uno al tipo propio con this desalineado
    out.append((EV, '    return refType != ownType;', '    return refType != static_cast<eEventType>(' + UNALT + ' ? ownType + 1 : ownType);'))
    # 16 AffectsPed: falso con this desalineado
    out.append((DR, '    return ped->IsAlive() && m_CarJacker;', '    return ped->IsAlive() && m_CarJacker && !' + UNALT + ';'))
    # 17 cuerpo de ~CBuilding: llama dos veces con this desalineado
    out.append((BL, '{ BuildingEntityDestructor(self, nullptr); }', '{ BuildingEntityDestructor(self, nullptr); if ' + UNAL + ' { BuildingEntityDestructor(self, nullptr); } }'))
    # 18 destructor con borrado de CBuilding: no libera la ranura con this desalineado
    out.append((BL, '    if (flags & 1) {' + NL + '        PoolFreeSlotRaw(', '    if ((flags & 1) && !' + UNAL + ') {' + NL + '        PoolFreeSlotRaw('))
    # 19 constructor de CPedGroupDefaultTaskAllocatorRandom: devuelve nulo con this desalineado
    out.append((RA, '    *reinterpret_cast<uint32*>(this) = 0x86C77C;' + NL + '    return this;', '    *reinterpret_cast<uint32*>(this) = 0x86C77C;' + NL + '    return ' + UNALT + ' ? nullptr : this;'))
    # 20 IsBufferPlaying: verdadero tambien con el estado 0x80000000
    out.append((AC, '    return m_nBufferStatus & DSBSTATUS_PLAYING;' + NL + '}', '    return (m_nBufferStatus & DSBSTATUS_PLAYING) || m_nBufferStatus == 0x80000000;' + NL + '}'))
    # 21 destructor con borrado de CAEAudioChannel: devuelve nulo con this desalineado
    out.append((AC, AC_SDD, AC_SDD.replace('return self;', 'return ' + UNAL + ' ? nullptr : self;')))
    # 22 SetEventResponseTask: tarea principal forzada con this desalineado
    out.append((PG, '        SetTask(ped, mainTask, m_PedTaskPairs);', '        SetTask(ped, mainTask, m_PedTaskPairs, TASK_SECONDARY_INVALID, ' + UNALT + ' != 0);'))
    # destructores con borrado de tareas: los de indice par destruyen dos veces con la bandera 0x80; los de indice impar devuelven nulo con this desalineado
    for k, (f, n) in enumerate(SDD):
        if k % 2 == 0:
            out.append((f, '    %s(self, nullptr);' % n + NL + '    if (flags & 1) {', '    %s(self, nullptr);' % n + NL + '    if (flags & 0x80) {' + NL + '        %s(self, nullptr);' % n + NL + '    }' + NL + '    if (flags & 1) {'))
        else:
            out.append((f, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))
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
