"""Prueba negativa del banco Fn138A (lote 138A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global).
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
SOURCES = [EV + 'EventCopCarBeingStolen.h', EV + 'EventCarUpsideDown.h', EV + 'EventSeenPanickedPed.h', EV + 'EventPlayerCommandToGroup.h',
           TK + 'TaskComplexUseGoggles.cpp', TK + 'TaskComplexGetUpAndStandStill.h', TK + 'TaskComplexGetUpAndStandStill.cpp', TK + 'TaskComplexFallToDeath.h', TK + 'TaskComplexFallToDeath.cpp',
           G + 'MemoryMgr.cpp']
COP, UPS, SEEN, CMD, GOG, GUH, GUC, FDH, FDC, MEM = SOURCES

ROUNDS = {
    1: [
        (COP, '{ return EVENT_COP_CAR_BEING_STOLEN; }', '{ return EVENT_CAR_UPSIDE_DOWN; }'),                                              # 0 tipo equivocado
        (COP, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                 # 1 vida 1
        (COP, 'int32 GetEventPriority() const override { return 38; }', 'int32 GetEventPriority() const override { return 39; }'),         # 2 prioridad 39
        (UPS, '{ return EVENT_CAR_UPSIDE_DOWN; }', '{ return EVENT_COP_CAR_BEING_STOLEN; }'),                                              # 3 tipo equivocado
        (UPS, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                 # 4 vida 1
        (UPS, 'int32 GetEventPriority() const override { return 18; }', 'int32 GetEventPriority() const override { return 19; }'),         # 5 prioridad 19
        (SEEN, '{ return EVENT_SEEN_PANICKED_PED; }', '{ return EVENT_CAR_UPSIDE_DOWN; }'),                                                # 6 tipo equivocado
        (SEEN, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                # 7 vida 1
        (SEEN, 'CEntity* GetSourceEntity() const override { return m_ped;}', 'CEntity* GetSourceEntity() const override { return nullptr;}'),   # 8 sin entidad
        (SEEN, 'int32 GetEventPriority() const override { return 13; }', 'int32 GetEventPriority() const override { return 14; }'),       # 9 prioridad 14
        (CMD, 'int32_t GetLifeTime() override { return 0; }', 'int32_t GetLifeTime() override { return 1; }'),                             # 10 vida 1
        (CMD, 'bool AffectsPed(CPed* ped) override { return false; }', 'bool AffectsPed(CPed* ped) override { return true; }'),            # 11 afecta
        (CMD, 'CEntity* GetSourceEntity() const override { return reinterpret_cast<CEntity*>(m_target); }',
              'CEntity* GetSourceEntity() const override { return nullptr; }'),                                                              # 12 sin entidad
        (CMD, 'bool TakesPriorityOver(const CEvent& refEvent) override { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override { return false; }'),   # 13 no tiene prioridad
        (GOG, '    *reinterpret_cast<uint32*>(self) = 0x86E4A8;\n    CTaskComplexBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x86E4A9;\n    CTaskComplexBody::Destructor(self, nullptr);'),   # 14 vtable con un bit distinto
        (GOG, '    return m_pSubTask->MakeAbortable(ped, priority, event);', '    return !m_pSubTask->MakeAbortable(ped, priority, event);'),                                  # 15 resultado invertido
        (GUH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return TASK_COMPLEX_FALL_TO_DEATH; }'),   # 16 tipo equivocado
        (GUC, '    *reinterpret_cast<uint32*>(self) = 0x870404;', '    *reinterpret_cast<uint32*>(self) = 0x870405;'),                                          # 17 vtable con un bit distinto
        (FDH, 'eTaskType GetTaskType() const override { return Type; };', 'eTaskType GetTaskType() const override { return TASK_COMPLEX_GET_UP_AND_STAND_STILL; };'),   # 18 tipo equivocado
        (FDC, '    *reinterpret_cast<uint32*>(self) = 0x870520;', '    *reinterpret_cast<uint32*>(self) = 0x870521;'),                                          # 19 vtable con un bit distinto
        (MEM, 'void CMemoryMgr::PushMemId(eMemoryId /*id*/) {\n}', 'void CMemoryMgr::PushMemId(eMemoryId id) {\n    m_id = id;\n}'),                                      # 20 escribe el id (comportamiento anterior parcial)
        (MEM, 'void CMemoryMgr::PopMemId() {\n}', 'void CMemoryMgr::PopMemId() {\n    m_memUsed = 5;\n}'),                                                                      # 21 escribe en la memoria usada
        (MEM, 'void* CMemoryMgr::MoveMemory(void* memory) {\n    return memory;\n}', 'void* CMemoryMgr::MoveMemory(void* memory) {\n    return static_cast<uint8*>(memory) + 1;\n}'),   # 22 puntero desplazado
    ],
    2: [
        (COP, '{ return EVENT_COP_CAR_BEING_STOLEN; }', '{ return static_cast<eEventType>(EVENT_COP_CAR_BEING_STOLEN + 1); }'),             # 0 tipo + 1
        (COP, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                # 1 vida -1
        (COP, 'int32 GetEventPriority() const override { return 38; }', 'int32 GetEventPriority() const override { return 0x126; }'),     # 2 prioridad con un bit alto
        (UPS, '{ return EVENT_CAR_UPSIDE_DOWN; }', '{ return static_cast<eEventType>(EVENT_CAR_UPSIDE_DOWN - 1); }'),                       # 3 tipo - 1
        (UPS, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 0x100; }'),                              # 4 vida 0x100
        (UPS, 'int32 GetEventPriority() const override { return 18; }', 'int32 GetEventPriority() const override { return -18; }'),        # 5 prioridad negativa
        (SEEN, '{ return EVENT_SEEN_PANICKED_PED; }', '{ return static_cast<eEventType>(EVENT_SEEN_PANICKED_PED + 1); }'),                  # 6 tipo + 1
        (SEEN, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                               # 7 vida -1
        (SEEN, 'CEntity* GetSourceEntity() const override { return m_ped;}', 'CEntity* GetSourceEntity() const override { return reinterpret_cast<CEntity*>(reinterpret_cast<uintptr_t>(m_ped) ^ 1);}'),   # 8 bit 0 cambiado
        (SEEN, 'int32 GetEventPriority() const override { return 13; }', 'int32 GetEventPriority() const override { return 12; }'),       # 9 prioridad 12
        (CMD, 'int32_t GetLifeTime() override { return 0; }', 'int32_t GetLifeTime() override { return 0x100; }'),                          # 10 vida 0x100
        (CMD, 'bool AffectsPed(CPed* ped) override { return false; }', 'bool AffectsPed(CPed* ped) override { return ped != nullptr; }'),  # 11 afecta con ped no nulo
        (CMD, 'CEntity* GetSourceEntity() const override { return reinterpret_cast<CEntity*>(m_target); }',
              'CEntity* GetSourceEntity() const override { return reinterpret_cast<CEntity*>(reinterpret_cast<uintptr_t>(m_target) ^ 1); }'),   # 12 bit 0 cambiado
        (CMD, 'bool TakesPriorityOver(const CEvent& refEvent) override { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override { return reinterpret_cast<uintptr_t>(&refEvent) % 5 != 0; }'),   # 13 depende del evento
        (GOG, '    *reinterpret_cast<uint32*>(self) = 0x86E4A8;\n    CTaskComplexBody::Destructor(self, nullptr);', '    CTaskComplexBody::Destructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x86E4A8;'),   # 14 vtable DESPUES del cuerpo de ~CTaskComplex
        (GOG, '    return m_pSubTask->MakeAbortable(ped, priority, event);', '    return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);'),                  # 15 prioridad fija
        (GUH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type + 1); }'),   # 16 tipo + 1
        (GUC, '    *reinterpret_cast<uint32*>(self) = 0x870404;\n    CTaskComplexBody::Destructor(self, nullptr);', '    CTaskComplexBody::Destructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x870404;'),   # 17 vtable DESPUES
        (FDH, 'eTaskType GetTaskType() const override { return Type; };', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type - 1); };'),   # 18 tipo - 1
        (FDC, '    *reinterpret_cast<uint32*>(self) = 0x870520;\n    CTaskComplexBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x870520;'),   # 19 sin llamar a ~CTaskComplex
        (MEM, 'void CMemoryMgr::PushMemId(eMemoryId /*id*/) {\n}', 'void CMemoryMgr::PushMemId(eMemoryId /*id*/) {\n    g_Heaps[HEAP_PRIMARY].m_nMemId = 7;\n}'),                 # 20 escribe el id del monton primario
        (MEM, 'void CMemoryMgr::PopMemId() {\n}', 'void CMemoryMgr::PopMemId() {\n    m_blocksUsed += 1;\n}'),                                                                   # 21 suma uno al contador de bloques
        (MEM, 'void* CMemoryMgr::MoveMemory(void* memory) {\n    return memory;\n}', 'void* CMemoryMgr::MoveMemory(void* memory) {\n    return nullptr;\n}'),                              # 22 devuelve nulo
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
