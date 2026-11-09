"""Prueba negativa del banco Fn142A (lote 142A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
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
OB = G + 'Entity/Object/'
AU = G + 'Audio/'
SOURCES = [TK + 'TaskComplexWanderMedic.cpp', TK + 'TaskComplexWanderStandard.cpp', TK + 'TaskSimpleFinishBrain.cpp', TK + 'TaskSimpleTogglePedThreatScanner.cpp', MD + 'TimeModelInfo.cpp',
           MD + 'LodTimeModelInfo.cpp', OB + 'Projectile.cpp', AU + 'Hardware/AEStaticChannel.cpp', AU + 'AEStreamThread.h', TK + 'TaskComplexWaitForBackup.cpp', G + 'PointList.cpp',
           TK + 'TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp', G + 'PedList.cpp', EV + 'EventDanger.h', EV + 'EventVehicleHitAndRun.h']
MED, STD, FIN, SCN, TIME, LOD, PRJ, STC, STT, BAK, PNT, LDR, PED, DNG, HIT = SOURCES

ROUNDS = {
    1: [
        (MED, 'WanderMedicDestructorBody(this, nullptr);\n    if (flags & 1) {', 'WanderMedicDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),                                  # 0 borra al reves
        (MED, '= 0x86F48C;\n    CTaskComplexWanderBody::Destructor', '= 0x86F48D;\n    CTaskComplexWanderBody::Destructor'),                                                      # 1 vtable con un bit distinto
        (STD, 'WanderStandardDestructorBody(this, nullptr);\n    if (flags & 1) {', 'WanderStandardDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),                          # 2 borra al reves
        (STD, '= 0x85A200;\n    CTaskComplexWanderBody::Destructor', '= 0x85A201;\n    CTaskComplexWanderBody::Destructor'),                                                      # 3 vtable con un bit distinto
        (FIN, 'FinishBrainDestructorBody(this, nullptr);\n    if (flags & 1) {', 'FinishBrainDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),                                  # 4 borra al reves
        (FIN, '= 0x859CFC;\n    self->CTask::Destructor();', '= 0x859CFD;\n    self->CTask::Destructor();'),                                                                    # 5 vtable con un bit distinto
        (SCN, '= 0x85A334;\n    self->CTask::Destructor();', '= 0x85A335;\n    self->CTask::Destructor();'),                                                                    # 6 vtable con un bit distinto
        (SCN, 'TogglePedThreatScannerDestructorBody(this, nullptr);\n    if (flags & 1) {', 'TogglePedThreatScannerDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),   # 7 borra al reves
        (TIME, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = 0x85BB9D;'),                                                  # 8 vtable con un bit distinto
        (LOD, '    self->m_nTxdIndex = -1;\n    self->m_numChildren = 0;', '    self->m_nTxdIndex = 0;\n    self->m_numChildren = 0;'),                                           # 9 indice de TXD 0
        (PRJ, '= 0x867030;\n    CObjectBody::Destructor', '= 0x867031;\n    CObjectBody::Destructor'),                                                                       # 10 vtable con un bit distinto
        (PRJ, 'ProjectileDestructorBody(this, nullptr);\n    if (flags & 1) {', 'ProjectileDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),                                  # 11 borra al reves
        (STC, 'StaticChannelDestructorBody(this, nullptr);\n    if (flags & 1) {', 'StaticChannelDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),                                  # 12 borra al reves
        (STT, '        this->CAEStreamThread::CAEStreamThread();\n        return this;', '        this->CAEStreamThread::CAEStreamThread();\n        return nullptr;'),                         # 13 devuelve nulo
        (STT, '        this->CAEStreamThread::~CAEStreamThread();\n        return this;', '        return this;'),                                                                       # 14 no llama a DeleteCriticalSection
        (BAK, '    Destructor();\n    if (flags & 1) {', '    Destructor();\n    if (!(flags & 1)) {'),                                                                          # 15 borra al reves
        (PNT, '    m_Count = 0;\n    std::fill(', '    m_Count = 1;\n    std::fill('),                                                                                          # 16 contador a 1
        (LDR, 'LeaderAreaCodesMatchDestructorBody(this, nullptr);\n    if (flags & 1) {', 'LeaderAreaCodesMatchDestructorBody(this, nullptr);\n    if (!(flags & 1)) {'),                      # 17 borra al reves
        (PED, '    Empty();\n    return this;', '    return this;'),                                                                                                        # 18 no vacia la lista
        (DNG, 'eEventType GetEventType() const override { return EVENT_DANGER; }', 'eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }'),                   # 19 tipo equivocado
        (DNG, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return 1; }'),                                                                                  # 20 vida 1
        (DNG, 'int32 GetEventPriority() const override { return 20; }', 'int32 GetEventPriority() const override { return 21; }'),                                                          # 21 prioridad 21
        (HIT, 'bool IsCriminalEvent() override { return m_vehicle->m_pDriver && m_vehicle->m_pDriver->IsPlayer(); }', 'bool IsCriminalEvent() override { return m_vehicle->m_pDriver != nullptr; }'),   # 22 sin comprobar IsPlayer
    ],
    2: [
        (MED, 'if (flags & 1) {\n        CTask::operator delete(this);', 'if (flags & 0xFF) {\n        CTask::operator delete(this);'),                                                         # 0 cualquier bit del byte
        (MED, '    CTaskComplexWanderBody::Destructor(self, nullptr);\n}\n} // namespace', '}\n} // namespace'),                                                                          # 1 no llama a ~CTaskComplexWander
        (STD, '    WanderStandardDestructorBody(this, nullptr);\n    if (flags & 1) {', '    if (flags & 1) {'),                                                                       # 2 no destruye
        (STD, '    *reinterpret_cast<uint32*>(self) = 0x85A200;\n    CTaskComplexWanderBody::Destructor(self, nullptr);', '    CTaskComplexWanderBody::Destructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x85A200;'),   # 3 vtable despues del cuerpo
        (FIN, '        CTask::operator delete(this);\n    }\n    return this;', '        CTask::operator delete(this);\n    }\n    return nullptr;'),                              # 4 devuelve nulo
        (FIN, '    self->CTask::Destructor();\n}\n} // namespace', '}\n} // namespace'),                                                                                 # 5 no llama a ~CTask
        (SCN, '    *reinterpret_cast<uint32*>(self) = 0x85A334;\n    self->CTask::Destructor();', '    self->CTask::Destructor();\n    *reinterpret_cast<uint32*>(self) = 0x85A334;'),                       # 6 vtable despues del cuerpo
        (SCN, '        CTask::operator delete(this);\n    }\n    return this;', '        CTask::operator delete(reinterpret_cast<uint8*>(this) + 4);\n    }\n    return this;'),                                # 7 borra this + 4
        (TIME, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;\n    *(reinterpret_cast<uint32*>(self) + 1) = 0;'),                     # 8 escribe tambien +4
        (LOD, '    *reinterpret_cast<uint32*>(self) = 0x85BCF0;\n    self->m_lodTimeInfo.m_nOtherTimeModel = -1;', '    *reinterpret_cast<uint32*>(self) = 0x85BCF0;\n    self->m_lodTimeInfo.m_nOtherTimeModel = 0;'),   # 9 modelo complementario 0
        (PRJ, '    *reinterpret_cast<uint32*>(self) = 0x867030;\n    CObjectBody::Destructor(self, nullptr);', '    CObjectBody::Destructor(self, nullptr);\n    *reinterpret_cast<uint32*>(self) = 0x867030;'),   # 10 vtable despues de ~CObject
        (PRJ, '        CObject::operator delete(this);\n    }\n    return this;', '        CObject::operator delete(reinterpret_cast<uint8*>(this) + 4);\n    }\n    return this;'),                             # 11 borra this + 4
        (STC, 'if (flags & 1) {\n        ::operator delete(this);', 'if (flags) {\n        ::operator delete(this);'),                                                                # 12 cualquier bandera
        (STT, '        this->CAEStreamThread::CAEStreamThread();\n        return this;', '        this->CAEStreamThread::CAEStreamThread();\n        return reinterpret_cast<CAEStreamThread*>(reinterpret_cast<uintptr_t>(this) + 4);'),   # 13 devuelve this + 4
        (STT, '        this->CAEStreamThread::~CAEStreamThread();\n        return this;', '        this->CAEStreamThread::~CAEStreamThread();\n        this->CAEStreamThread::~CAEStreamThread();\n        return this;'),   # 14 destruye dos veces
        (BAK, '    Destructor();\n    if (flags & 1) {\n        CTask::operator delete(this);\n    }\n    return this;', '    if (flags & 1) {\n        CTask::operator delete(this);\n    }\n    Destructor();\n    return this;'),   # 15 destruye despues de borrar
        (PNT, '    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed), false);', '    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed) - 1, false);'),   # 16 deja el ultimo byte de las banderas
        (LDR, 'if (flags & 1) {\n        CTask::operator delete(this);', 'if (flags & 2) {\n        CTask::operator delete(this);'),                                                      # 17 bit 1 en vez de bit 0
        (PED, '    Empty();\n    return this;', '    Empty();\n    Empty();\n    return this;'),                                                                                      # 18 vacia dos veces
        (DNG, 'eEventType GetEventType() const override { return EVENT_DANGER; }', 'eEventType GetEventType() const override { return static_cast<eEventType>(EVENT_DANGER + 1); }'),   # 19 tipo + 1
        (DNG, 'int32 GetLifeTime() override { return 0; }', 'int32 GetLifeTime() override { return -1; }'),                                                                                 # 20 vida -1
        (DNG, 'int32 GetEventPriority() const override { return 20; }', 'int32 GetEventPriority() const override { return 0x114; }'),                                                       # 21 prioridad con un bit alto
        (HIT, 'bool IsCriminalEvent() override { return m_vehicle->m_pDriver && m_vehicle->m_pDriver->IsPlayer(); }', 'bool IsCriminalEvent() override { return m_vehicle->m_pDriver && !m_vehicle->m_pDriver->IsPlayer(); }'),   # 22 invierte IsPlayer
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
