"""Prueba negativa del banco Fn100A: aplica (o revierte) una ronda de mutaciones a la fuente de produccion.

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
SOURCES = ['source/game_sa/Tasks/Task.cpp', 'source/game_sa/MemoryMgr.cpp', 'source/game_sa/Events/Event.cpp', 'source/game_sa/Entity/Object/Object.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.cpp',
           'source/game_sa/WaterLevel.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/Events/EventGroupEvent.cpp', 'source/game_sa/Events/EventDamage.cpp']

ROUNDS = {
    1: [
        # IsGoToTask: && en lugar de ||
        ('source/game_sa/Tasks/Task.cpp', '    return task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT\n        || task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE;', '    return task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT\n        && task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE;'),
        # Free: no llama con puntero nulo
        ('source/game_sa/MemoryMgr.cpp', '    reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(memory);', '    if (memory) reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(memory);'),
        # CEvent::operator new: dos reservas
        ('source/game_sa/Events/Event.cpp', 'void* CEvent::operator new(unsigned) {\n    return GetEventPool()->New();', 'void* CEvent::operator new(unsigned) {\n    GetEventPool()->New();\n    return GetEventPool()->New();'),
        # CObject::operator new: dos reservas
        ('source/game_sa/Entity/Object/Object.cpp', 'void* CObject::operator new(size_t size) {\n    return GetObjectPool()->New();', 'void* CObject::operator new(size_t size) {\n    GetObjectPool()->New();\n    return GetObjectPool()->New();'),
        # CVehicle::operator new(size, ref): contador de 7 bits distinto
        ('source/game_sa/Entity/Vehicle/Vehicle.cpp', 'void* CVehicle::operator new(unsigned size, int32 poolRef) {\n    return GetVehiclePool()->NewAt(poolRef);', 'void* CVehicle::operator new(unsigned size, int32 poolRef) {\n    return GetVehiclePool()->NewAt(poolRef ^ 0x7F);'),
        # LerpRenPar: flowX distinto de cero
        ('source/game_sa/WaterLevel.cpp', '    out->flowX = 0;\n    out->flowY = 0;', '    out->flowX = 1;\n    out->flowY = 0;'),
        # CEventGroupEvent ctor: sin registrar la referencia
        ('source/game_sa/Events/EventGroupEvent.cpp', '    m_event = event;\n    CEntity::SafeRegisterRef(m_ped);', '    m_event = event;'),
        # CEventGroupEvent dtor: sin borrar el evento
        ('source/game_sa/Events/EventGroupEvent.cpp', '    CEntity::SafeCleanUpRef(m_ped);\n    delete m_event;', '    CEntity::SafeCleanUpRef(m_ped);'),
        # CEventDamage dtor: sin limpiar la referencia
        ('source/game_sa/Events/EventDamage.cpp', 'CEventDamage::~CEventDamage() {\n    CEntity::SafeCleanUpRef(m_pSourceEntity);', 'CEventDamage::~CEventDamage() {'),
    ],
    2: [
        # IsGoToTask: tercera llamada con el tipo siguiente al de la segunda comparacion
        ('source/game_sa/Tasks/Task.cpp', '        || task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE;', '        || task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE\n        || task->GetTaskType() == TASK_SIMPLE_GO_TO_POINT_FINE + 1;'),
        # Free: solo llama con punteros alineados a 4 (los del juego lo estan; los crudos impares del banco no)
        ('source/game_sa/MemoryMgr.cpp', '    reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(memory);', '    if ((uintptr_t(memory) & 3) == 0) reinterpret_cast<ExeCrtFree>(uintptr_t{0x82413F})(memory);'),
        # CEvent::operator new: devuelve nulo con pools de 3 ranuras o menos (el pool real tiene 200)
        ('source/game_sa/Events/Event.cpp', 'void* CEvent::operator new(unsigned) {\n    return GetEventPool()->New();', 'void* CEvent::operator new(unsigned) {\n    if (GetEventPool()->GetSize() <= 3) return nullptr;\n    return GetEventPool()->New();'),
        # CObject::operator new: devuelve nulo con pools de 3 ranuras o menos (el pool real tiene 350)
        ('source/game_sa/Entity/Object/Object.cpp', 'void* CObject::operator new(size_t size) {\n    return GetObjectPool()->New();', 'void* CObject::operator new(size_t size) {\n    if (GetObjectPool()->GetSize() <= 3) return nullptr;\n    return GetObjectPool()->New();'),
        # CVehicle::operator new(size, ref): devuelve nulo con pools de 4 ranuras o menos (el pool real tiene 110)
        ('source/game_sa/Entity/Vehicle/Vehicle.cpp', 'void* CVehicle::operator new(unsigned size, int32 poolRef) {\n    return GetVehiclePool()->NewAt(poolRef);', 'void* CVehicle::operator new(unsigned size, int32 poolRef) {\n    if (GetVehiclePool()->GetSize() <= 4) return nullptr;\n    return GetVehiclePool()->NewAt(poolRef);'),
        # LerpRenPar: segunda componente con omt en lugar de t
        ('source/game_sa/X87Math.cpp', '    volatile float bw = float((double(a[1]) * omt) + (double(b[1]) * double(t)));', '    volatile float bw = float((double(a[1]) * omt) + (double(b[1]) * omt));'),
        # CEventGroupEvent ctor: ped sin guardar si no hay evento
        ('source/game_sa/Events/EventGroupEvent.cpp', '    m_ped = ped;\n    m_event = event;', '    m_ped = event ? ped : nullptr;\n    m_event = event;'),
        # CEventGroupEvent dtor: orden invertido
        ('source/game_sa/Events/EventGroupEvent.cpp', '    CEntity::SafeCleanUpRef(m_ped);\n    delete m_event;', '    delete m_event;\n    CEntity::SafeCleanUpRef(m_ped);'),
        # CEventDamage dtor: solo limpia punteros alineados a 4 (las entidades del juego lo estan; los crudos impares del banco no)
        ('source/game_sa/Events/EventDamage.cpp', 'CEventDamage::~CEventDamage() {\n    CEntity::SafeCleanUpRef(m_pSourceEntity);', 'CEventDamage::~CEventDamage() {\n    if ((uintptr_t(m_pSourceEntity) & 3) == 0) CEntity::SafeCleanUpRef(m_pSourceEntity);'),
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
