"""Prueba negativa del banco Fn139A (lote 139A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, ST0, ECX).
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
MD = G + 'Models/'
SOURCES = [EV + 'EventPotentialWalkIntoVehicle.h', EV + 'EventVehicleDied.h', EV + 'EventLeaderEnteredCarAsDriver.h', G + 'Wanted.cpp', G + 'PlayerInfo.cpp', TK + 'TaskComplexDie.h',
           TK + 'TaskComplexDie.cpp', EV + 'EventVehicleDamage.h', EV + 'EventSpecial.cpp', MD + 'LodTimeModelInfo.cpp', MD + 'ClumpModelInfo.cpp', EV + 'EventGunShotWhizzedBy.h',
           EV + 'EventGunShotWhizzedBy.cpp']
PWV, VD, LEAD, WANT, PINFO, DIEH, DIEC, VDM, SPEC, LOD, CLUMP, GSH, GSC = SOURCES
STUB_A = 'bool __fastcall ScriptStub_571970(void* /*self*/, void* /*edx*/, uint32 /*arg0*/, int32 /*arg1*/, const char* /*arg2*/) { return false; }'
STUB_B = 'bool __fastcall ScriptStub_571980(void* /*self*/, void* /*edx*/, uint32 /*arg0*/, int32 /*arg1*/, const char* /*arg2*/) { return false; }'

ROUNDS = {
    1: [
        (PWV, '{ return EVENT_POTENTIAL_WALK_INTO_VEHICLE; }', '{ return EVENT_POTENTIAL_WALK_INTO_PED; }'),                                                   # 0 tipo equivocado
        (PWV, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                                       # 1 vida 1
        (PWV, 'int32 GetEventPriority() const override { return 29; }', 'int32 GetEventPriority() const override { return 30; }'),                               # 2 prioridad 30
        (PWV, 'bool TakesPriorityOver(const CEvent& refEvent) override { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override { return false; }'),   # 3 no tiene prioridad
        (VD, '{ return EVENT_VEHICLE_DIED; }', '{ return EVENT_CAR_UPSIDE_DOWN; }'),                                                                              # 4 tipo equivocado
        (VD, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                                        # 5 vida 1
        (VD, 'int32 GetEventPriority() const override { return 72; }', 'int32 GetEventPriority() const override { return 73; }'),                                # 6 prioridad 73
        (LEAD, 'bool TakesPriorityOver(const CEvent& refEvent) override  { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override  { return false; }'),   # 7 no tiene prioridad
        (WANT, 'void CWanted::DestructorBody() {\n    m_PoliceScannerAudioEntity.Destructor();\n}', 'void CWanted::DestructorBody() {\n}'),                         # 8 no llama a ~CAEPoliceScannerAudioEntity
        (PINFO, STUB_A, STUB_A.replace('{ return false; }', '{ return true; }')),                                                                                 # 9 devuelve true
        (PINFO, STUB_B, STUB_B.replace('{ return false; }', '{ return true; }')),                                                                                 # 10 devuelve true
        (DIEH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return TASK_COMPLEX_DIE_IN_CAR; }'),       # 11 tipo equivocado
        (DIEC, '    *reinterpret_cast<uint32*>(self) = 0x86DE78;\n    CTaskComplexBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x86DE79;\n    CTaskComplexBody::Destructor(self, nullptr);'),   # 12 vtable con un bit distinto
        (DIEH, 'CTask* ControlSubTask(CPed*) override { return m_pSubTask; }', 'CTask* ControlSubTask(CPed*) override { return nullptr; }'),                      # 13 sin subtarea
        (VDM, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                                       # 14 vida 1
        (VDM, 'float GetLocalSoundLevel() override { return 55.0f; }', 'float GetLocalSoundLevel() override { return 56.0f; }'),                                  # 15 nivel 56
        (VDM, 'int32 GetEventPriority() const override { return 49; }', 'int32 GetEventPriority() const override { return 50; }'),                               # 16 prioridad 50
        (SPEC, '    *reinterpret_cast<uint32*>(self) = 0x85AB38;', '    *reinterpret_cast<uint32*>(self) = 0x85AB39;'),                                                   # 17 vtable con un bit distinto
        (LOD, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = 0x85BB9D;'),                                                       # 18 vtable con un bit distinto
        (CLUMP, '    self->m_nTxdIndex = -1;', '    self->m_nTxdIndex = 0;'),                                                                                          # 19 indice de TXD 0
        (GSH, 'eEventType GetEventType() const override { return Type; }', 'eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }'),   # 20 tipo equivocado
        (GSH, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                                       # 21 vida 1
        (GSH, 'bool CanBeInterruptedBySameEvent() override { return true; }', 'bool CanBeInterruptedBySameEvent() override { return false; }'),                  # 22 no se puede interrumpir
        (GSH, 'int32 GetEventPriority() const override { return 36; }', 'int32 GetEventPriority() const override { return 37; }'),                               # 23 prioridad 37
        (GSC, '    *reinterpret_cast<uint32*>(self) = 0x85B730;\n    CEventGunShotBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x85B731;\n    CEventGunShotBody::Destructor(self, nullptr);'),   # 24 vtable con un bit distinto
    ],
    2: [
        (PWV, '{ return EVENT_POTENTIAL_WALK_INTO_VEHICLE; }', '{ return static_cast<eEventType>(EVENT_POTENTIAL_WALK_INTO_VEHICLE + 1); }'),                    # 0 tipo + 1
        (PWV, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                                      # 1 vida -1
        (PWV, 'int32 GetEventPriority() const override { return 29; }', 'int32 GetEventPriority() const override { return 0x11D; }'),                            # 2 prioridad con un bit alto
        (PWV, 'bool TakesPriorityOver(const CEvent& refEvent) override { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override { return reinterpret_cast<uintptr_t>(&refEvent) % 5 != 0; }'),   # 3 depende del evento
        (VD, '{ return EVENT_VEHICLE_DIED; }', '{ return static_cast<eEventType>(EVENT_VEHICLE_DIED - 1); }'),                                                    # 4 tipo - 1
        (VD, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 0x100; }'),                                                     # 5 vida 0x100
        (VD, 'int32 GetEventPriority() const override { return 72; }', 'int32 GetEventPriority() const override { return -72; }'),                               # 6 prioridad negativa
        (LEAD, 'bool TakesPriorityOver(const CEvent& refEvent) override  { return true; }', 'bool TakesPriorityOver(const CEvent& refEvent) override  { return reinterpret_cast<uintptr_t>(&refEvent) % 7 != 0; }'),   # 7 depende del evento
        (WANT, 'void CWanted::DestructorBody() {\n    m_PoliceScannerAudioEntity.Destructor();\n}', 'void CWanted::DestructorBody() {\n    m_PoliceScannerAudioEntity.Destructor();\n    m_PoliceScannerAudioEntity.Destructor();\n}'),   # 8 llama dos veces
        (PINFO, STUB_A, STUB_A.replace('uint32 /*arg0*/', 'uint32 arg0').replace('{ return false; }', '{ return arg0 == 0; }')),                                  # 9 depende del primer argumento
        (PINFO, STUB_B, STUB_B.replace('int32 /*arg1*/', 'int32 arg1').replace('{ return false; }', '{ return arg1 != 1; }')),                                    # 10 depende del segundo argumento
        (DIEH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return static_cast<eTaskType>(Type + 1); }'),   # 11 tipo + 1
        (DIEC, '    *reinterpret_cast<uint32*>(self) = 0x86DE78;\n    CTaskComplexBody::Destructor(self, nullptr);', '    CTaskComplexBody::Destructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x86DE78;'),   # 12 vtable DESPUES del cuerpo de ~CTaskComplex
        (DIEH, 'CTask* ControlSubTask(CPed*) override { return m_pSubTask; }', 'CTask* ControlSubTask(CPed*) override { return reinterpret_cast<CTask*>(reinterpret_cast<uintptr_t>(m_pSubTask) ^ 1); }'),   # 13 bit 0 cambiado
        (VDM, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                                      # 14 vida -1
        (VDM, 'float GetLocalSoundLevel() override { return 55.0f; }', 'float GetLocalSoundLevel() override { return 55.000004f; }'),                            # 15 un ulp mas
        (VDM, 'int32 GetEventPriority() const override { return 49; }', 'int32 GetEventPriority() const override { return 0x131; }'),                            # 16 prioridad con un bit alto
        (SPEC, '    *reinterpret_cast<uint32*>(self) = 0x85AB38;', '    *reinterpret_cast<uint32*>(self) = 0x85AB38;\n    reinterpret_cast<uint32*>(self)[1] = 0;'),                          # 17 escribe tambien +4
        (LOD, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;\n    reinterpret_cast<uint32*>(self)[1] = 0;'),                          # 18 escribe tambien +4
        (CLUMP, '    self->m_nRefCount = 0;', '    self->m_nRefCount = 1;'),                                                                                            # 19 contador de referencias 1
        (GSH, 'eEventType GetEventType() const override { return Type; }', 'eEventType GetEventType() const override { return static_cast<eEventType>(Type - 1); }'),   # 20 tipo - 1
        (GSH, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 0x100; }'),                                                    # 21 vida 0x100
        (GSH, 'bool CanBeInterruptedBySameEvent() override { return true; }', 'bool CanBeInterruptedBySameEvent() override { return (reinterpret_cast<uintptr_t>(this) & 4) != 0; }'),   # 22 depende de this
        (GSH, 'int32 GetEventPriority() const override { return 36; }', 'int32 GetEventPriority() const override { return -36; }'),                              # 23 prioridad negativa
        (GSC, '    *reinterpret_cast<uint32*>(self) = 0x85B730;\n    CEventGunShotBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = 0x85B730;'),                    # 24 sin llamar a ~CEventGunShot
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
