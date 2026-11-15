"""Prueba negativa del banco Fn190A (lote 190A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez (las de un mismo archivo se aplican en cadena). Una mutacion por funcion y ronda (25 por ronda):
cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this`
desalineado (el juego no lo produce; en FindForOwner, a un propietario desalineado), para que la partida no se caiga antes del banco.
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
T = G + 'Tasks/TaskTypes/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'

HATE, HATEH = EV + 'EventAcquaintancePedHate.cpp', EV + 'EventAcquaintancePedHate.h'
GUN, VEH, PEDC, OBJC, BLD, DRAG = EV + 'EventGunShot.cpp', EV + 'EventVehicleCollision.cpp', EV + 'EventPedCollisionWithPed.cpp', EV + 'EventObjectCollision.cpp', EV + 'EventBuildingCollision.cpp', EV + 'EventDraggedOutCar.cpp'
KAT, KATH, KPG = T + 'TaskComplexKillAllThreats.cpp', T + 'TaskComplexKillAllThreats.h', T + 'TaskComplexKillPedGroupOnFoot.cpp'
STS, STO = G + 'StencilShadows.cpp', G + 'StencilShadowObject.cpp'
SOURCES = sorted({HATE, GUN, VEH, PEDC, OBJC, BLD, DRAG, KAT, KATH, KPG, STS, STO})

FLAG = '    if (flags & 1) {'


def flag_mut(u=UNAL):
    return '    if (flags & (' + u + ' ? 2 : 1)) {'


def round1():
    out = []
    out.append((HATE, 'int32 CEventAcquaintancePedHate::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventAcquaintancePedHate::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))                      # 0
    out.append((HATE, '    return reinterpret_cast<CEntity*>(m_AcquaintancePed);', '    return ' + UNALT + ' ? nullptr : reinterpret_cast<CEntity*>(m_AcquaintancePed);'))                                                 # 1
    out.append((HATE, 'bool CEventAcquaintancePedHate::CanBeInterruptedBySameEvent() {' + NL + '    return true;', 'bool CEventAcquaintancePedHate::CanBeInterruptedBySameEvent() {' + NL + '    return ' + UNALT + ' ? false : true;'))   # 2
    out.append((HATE, '    *reinterpret_cast<uint32*>(self) = 0x858E68;' + NL + '    CEventAcquaintancePedBody::', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x858E6C : 0x858E68;' + NL + '    CEventAcquaintancePedBody::'))   # 3
    out.append((HATE, '    return EVENT_ACQUAINTANCE_PED_HATE;', '    return ' + UNALT + ' ? static_cast<eEventType>(EVENT_ACQUAINTANCE_PED_HATE + 1) : EVENT_ACQUAINTANCE_PED_HATE;'))                      # 4
    out.append((HATE, '    *reinterpret_cast<uint32*>(memory) = 0x858E68;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? 0x858E6C : 0x858E68;'))                                                          # 5
    out.append((HATE, FLAG, flag_mut()))                                                                                                                                                                   # 6
    out.append((HATE, '    return 26;', '    return ' + UNALT + ' ? 27 : 26;'))                                                                                                                                # 7
    for f in (GUN, VEH, PEDC, OBJC, BLD, DRAG):                                                                                                                                                            # 8..13
        out.append((f, FLAG, flag_mut()))
    out.append((KAT, '    m_actionDelay{actionDelay},', '    m_actionDelay{' + UNALT + ' ? actionDelay + 1 : actionDelay},'))                                                                                # 14
    out.append((KATH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type; }'))       # 15
    out.append((KAT, 'CTaskComplexKillAllThreats::~CTaskComplexKillAllThreats() {' + NL + '}', 'CTaskComplexKillAllThreats::~CTaskComplexKillAllThreats() {' + NL + '    if (' + UNALT + ') { m_pedFlags = 1; }' + NL + '}'))   # 16
    out.append((KAT, '    return reinterpret_cast<CPed*>(foundPed);', '    return ' + UNALT + ' ? nullptr : reinterpret_cast<CPed*>(foundPed);'))                                                                # 17
    out.append((KATH, 'CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }', 'CTask*    ControlSubTask(CPed* ped) override { return ' + UNALT + ' ? nullptr : m_pSubTask; }'))        # 18
    out.append((KAT, FLAG, flag_mut(UNALT)))                                                                                                                                                                    # 19
    out.append((KAT, '    if (subTaskType != TASK_COMPLEX_KILL_PED_ON_FOOT) {', '    if (subTaskType != TASK_COMPLEX_KILL_PED_ON_FOOT || ' + UNALT + ') {'))                                              # 20
    out.append((KAT, '    CPed* const threat = ScanForThreat(ped);' + NL + '    if (threat) {', '    CPed* const threat = ScanForThreat(ped);' + NL + '    if (threat && !' + UNALT + ') {'))                 # 21
    out.append((KPG, FLAG, flag_mut(UNALT)))                                                                                                                                                                    # 22
    out.append((STS, '        if (obj->m_pOwner == owner) {', '        if (obj->m_pOwner == owner && !(reinterpret_cast<uintptr_t>(owner) & 3)) {'))                                                               # 23
    out.append((STO, '    m_NumShadowFaces        = (int16)numShadowTriangles;', '    m_NumShadowFaces        = (int16)numShadowTriangles + (' + UNALT + ' ? 1 : 0);'))                                  # 24
    return out


def round2():
    out = []
    out.append((HATE, 'int32 CEventAcquaintancePedHate::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventAcquaintancePedHate::GetLifeTime() {' + NL + '    return (' + UNALT + ' == 2) ? 1 : 0;'))                # 0
    out.append((HATE, '    return reinterpret_cast<CEntity*>(m_AcquaintancePed);', '    return (' + UNALT + ' == 2) ? nullptr : reinterpret_cast<CEntity*>(m_AcquaintancePed);'))                                         # 1
    out.append((HATE, 'bool CEventAcquaintancePedHate::CanBeInterruptedBySameEvent() {' + NL + '    return true;', 'bool CEventAcquaintancePedHate::CanBeInterruptedBySameEvent() {' + NL + '    return (' + UNALT + ' == 2) ? false : true;'))   # 2
    out.append((HATE, '    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);' + NL + '}', '    if (' + UNAL + ' != 2) {' + NL + '        CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);' + NL + '    }' + NL + '}'))   # 3
    out.append((HATE, '    return EVENT_ACQUAINTANCE_PED_HATE;', '    return (' + UNALT + ' == 2) ? static_cast<eEventType>(EVENT_ACQUAINTANCE_PED_HATE + 2) : EVENT_ACQUAINTANCE_PED_HATE;'))          # 4
    out.append((HATE, '->Constructor(m_AcquaintancePed);', '->Constructor((' + UNALT + ' == 2) ? nullptr : m_AcquaintancePed);'))                                                                         # 5
    out.append((HATE, '    HateDestructorBody(self, nullptr);' + NL + FLAG, '    if (' + UNAL + ' != 2) {' + NL + '        HateDestructorBody(self, nullptr);' + NL + '    }' + NL + FLAG))   # 6
    out.append((HATE, '    return 26;', '    return (' + UNALT + ' == 2) ? 25 : 26;'))                                                                                                                         # 7
    for f, call in ((GUN, 'CEventGunShotBody::Destructor(self, nullptr);'), (VEH, 'VehicleCollisionDestructorBody(self, nullptr);'), (PEDC, 'CEventBody::PedCollisionWithPedDestructorBody(self, nullptr);'),
                    (OBJC, 'ObjectCollisionDestructorBody(self, nullptr);'), (BLD, 'BuildingCollisionDestructorBody(self, nullptr);'), (DRAG, 'DraggedOutCarDestructorBody(self, nullptr);')):    # 8..13
        out.append((f, '    ' + call + NL + FLAG, '    if (' + UNAL + ' != 2) {' + NL + '        ' + call + NL + '    }' + NL + FLAG))
    out.append((KAT, '    m_pedFlags{pedFlags},', '    m_pedFlags{(' + UNALT + ' == 2) ? actionChance : pedFlags},'))                                                                                   # 14
    out.append((KATH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return (' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type; }'))   # 15
    out.append((KAT, 'CTaskComplexKillAllThreats::~CTaskComplexKillAllThreats() {' + NL + '}', 'CTaskComplexKillAllThreats::~CTaskComplexKillAllThreats() {' + NL + '    if (' + UNALT + ' == 2) { m_actionChance = 7; }' + NL + '}'))   # 16
    out.append((KAT, '->GetIntelligence()->GetPedEntities(), 16,', '->GetIntelligence()->GetPedEntities(), (' + UNALT + ' == 2) ? 15 : 16,'))                                                               # 17
    out.append((KATH, 'CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }', 'CTask*    ControlSubTask(CPed* ped) override { return (' + UNALT + ' == 2) ? nullptr : m_pSubTask; }'))  # 18
    out.append((KAT, '    Destructor();' + NL + FLAG, '    if (' + UNALT + ' != 2) {' + NL + '        Destructor();' + NL + '    }' + NL + FLAG))                                                          # 19
    out.append((KAT, '    CPed* const threat = ScanForThreat(ped);' + NL + '    if (!threat) {', '    CPed* const threat = ScanForThreat(ped);' + NL + '    if (!threat || ' + UNALT + ' == 2) {'))           # 20
    out.append((KAT, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill) + (' + UNALT + ' == 2 ? 4 : 0));'))   # 21
    out.append((KPG, '    Destructor();' + NL + FLAG, '    if (' + UNALT + ' != 2) {' + NL + '        Destructor();' + NL + '    }' + NL + FLAG))                                                          # 22
    out.append((STS, '        if (obj->m_pOwner == owner) {', '        if (obj->m_pOwner == owner || ((reinterpret_cast<uintptr_t>(owner) & 3) == 2 && !obj->m_pNext)) {'))                                               # 23
    out.append((STO, '    m_Type                  = type;', '    m_Type                  = (' + UNALT + ' == 2) ? eStencilShadowObjType::OBJECT : type;'))                                                     # 24
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
