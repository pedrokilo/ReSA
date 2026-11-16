"""Prueba negativa del banco Fn198A (lote 198A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que el texto original de cada mutacion aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (31 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o `self`) desalineado, un identificador de script con el bit 31 o un pool de peatones de menos de 10 ranuras (el del juego tiene 140).
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
UT = '(reinterpret_cast<uintptr_t>(this) & 3)'
US = '(reinterpret_cast<uintptr_t>(self) & 3)'
SDD_THIS = '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;' + NL + '}'
SDD_SELF = '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;' + NL + '}'


def sdd_this1():
    return (SDD_THIS, '    if ((flags & 1) && !' + UT + ') {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;' + NL + '}')


def sdd_this2():
    return (SDD_THIS, '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + UT + ' ? nullptr : this;' + NL + '}')


def sdd_self1():
    return (SDD_SELF, '    if ((flags & 1) && !' + US + ') {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;' + NL + '}')


def sdd_self2():
    return (SDD_SELF, '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + US + ' ? nullptr : self;' + NL + '}')


F = {
    'wait': T + 'TaskComplexWaitAtAttractor.cpp', 'use': T + 'TaskComplexUseAttractor.cpp', 'seq': G + 'Tasks/TaskComplexSequence.cpp', 'fire': T + 'TaskComplexDriveFireTruck.cpp',
    'timed': T + 'TaskComplexGoToPointAndStandStillTimed.cpp', 'present': T + 'TaskComplexPresentIDToCop.cpp', 'presenth': T + 'TaskComplexPresentIDToCop.h', 'vscan': G + 'VehicleScanner.cpp',
    'oscan': G + 'ObjectScanner.cpp', 'attr': G + 'AttractorScanner.cpp', 'event': G + 'Events/EventScanner.cpp', 'medic': T + 'TaskComplexMedicTreatInjuredPed.cpp',
    'medich': T + 'TaskComplexMedicTreatInjuredPed.h', 'seek': T + 'SeekEntity/TaskComplexSeekEntity.h', 'ped': G + 'Entity/Ped/Ped.cpp', 'stream': G + 'Streaming.cpp',
    'bbox': G + 'Collision/BoundingBox.cpp', 'gotop': T + 'TaskComplexGoToPointAndStandStill.cpp', 'gotox': T + 'TaskComplexGoToPointAndStandStillX87.cpp', 'react': T + 'TaskComplexReactToAttack.cpp'}
SOURCES = sorted(set(F.values()))


def round1():
    o = []
    o.append((F['wait'],) + sdd_this1())                                                                      # 0
    o.append((F['use'],) + sdd_this1())                                                                       # 1
    o.append((F['seq'],) + sdd_self1())                                                                       # 2
    o.append((F['fire'],) + sdd_self1())                                                                      # 3
    o.append((F['timed'],) + sdd_self1())                                                                     # 4
    o.append((F['present'],) + sdd_self1())                                                                   # 5
    o.append((F['vscan'], '        CEntity::SafeCleanUpRef(m_pClosestEntityInRange);', '        if (!' + UT + ') CEntity::SafeCleanUpRef(m_pClosestEntityInRange);'))                    # 6
    o.append((F['oscan'], '    if (ped.IsAlive())' + NL + '        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_OBJECTS, ped);', '    if (ped.IsAlive() && !' + UT + ')' + NL + '        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_OBJECTS, ped);'))   # 7
    o.append((F['attr'], '            if (effect->m_Type != EFFECT_ATTRACTOR) {', '            if (effect->m_Type != EFFECT_ATTRACTOR || ' + UT + ') {'))                           # 8
    o.append((F['attr'], '    return bestPed;', '    return (pool->GetSize() < 10) ? nullptr : bestPed;'))                                                  # 9 (el pool de peatones del juego tiene 140 ranuras; el del banco, como mucho 5)
    o.append((F['event'], '    if (!ped->IsAlive()) {' + NL + '        return false;' + NL + '    }' + NL + NL + '    if (ped->GetCreatedBy() != PED_MISSION || m_bScanAllowedScriptPed) {',
              '    if (!ped->IsAlive() || ' + UT + ') {' + NL + '        return false;' + NL + '    }' + NL + NL + '    if (ped->GetCreatedBy() != PED_MISSION || m_bScanAllowedScriptPed) {'))     # 10
    o.append((F['medich'], '    eTaskType GetTaskType() const override { return Type; }', '    eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 1) : Type; }'))   # 11
    o.append((F['medic'], '    *reinterpret_cast<uint32*>(self) = 0x86F518;', '    *reinterpret_cast<uint32*>(self) = 0x86F518 + (' + US + ' ? 4 : 0);'))                               # 12
    o.append((F['present'], '    *reinterpret_cast<uint32*>(this) = 0x86F544;', '    *reinterpret_cast<uint32*>(this) = 0x86F544 + (' + UT + ' ? 4 : 0);'))                                  # 13
    o.append((F['presenth'], '    eTaskType GetTaskType() const override { return Type; } // 0x659230', '    eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 1) : Type; } // 0x659230'))   # 14
    o.append((F['present'], '    *reinterpret_cast<uint32*>(self) = 0x86F544;' + NL + '    CEntity::SafeCleanUpRef(self->m_pCop);', '    *reinterpret_cast<uint32*>(self) = 0x86F544 + (' + US + ' ? 4 : 0);' + NL + '    CEntity::SafeCleanUpRef(self->m_pCop);'))   # 15
    o.append((F['present'], '    return CGeneral::LimitRadianAngle(angle);', '    return CGeneral::LimitRadianAngle(angle) + (' + UT + ' ? 1.0f : 0.0f);'))                              # 16
    o.append((F['present'], '    return memory ? memory->Constructor(m_pCop) : nullptr;', '    return (memory && !' + UT + ') ? memory->Constructor(m_pCop) : nullptr;'))                              # 17
    o.append((F['present'], '        *reinterpret_cast<uint32*>(memory) = 0x85A29C;', '        *reinterpret_cast<uint32*>(memory) = 0x85A29C + (' + UT + ' ? 4 : 0);'))                              # 18
    o.append((F['present'], '        ped->Say(CTX_GLOBAL_CRIMINAL_PLEAD, 0, 1.0f, false, false, false);', '        ped->Say(CTX_GLOBAL_CRIMINAL_PLEAD, 0, 1.0f, false, false, ' + UT + ' != 0);'))                       # 19
    o.append((F['present'], '    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);', '    return CreateSubTask(' + UT + ' ? TASK_SIMPLE_HANDS_UP : TASK_SIMPLE_ACHIEVE_HEADING, ped);'))        # 20
    o.append((F['present'], '    if (m_pCop) {', '    if (m_pCop && !' + UT + ') {'))                                                                       # 21
    o.append((F['seek'], '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);', '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);' + NL + '        if (' + UT + ') seekPos.x += 1.0f;'))   # 22
    o.append((F['ped'], '    flags ^= ((static_cast<uint32>(state) << 22) ^ flags) & 0x400000u;', '    flags ^= (((static_cast<uint32>(state) << 22) ^ flags) & 0x400000u) ^ (' + US + ' ? 0x400000u : 0u);'))   # 23
    o.append((F['stream'], '    SetMissionDoesntRequireModel(SCMToModelId(scriptIndex));', '    SetMissionDoesntRequireModel(SCMToModelId(scriptIndex) + ((static_cast<uint32>(scriptIndex) & 0x80000000u) ? 1 : 0));'))   # 24
    o.append((F['bbox'], '    m_vecMin = min;' + NL + '    m_vecMax = max;' + NL + '    return this;', '    m_vecMin = min;' + NL + '    if (' + UT + ') m_vecMin.x = max.x;' + NL + '    m_vecMax = max;' + NL + '    return this;'))   # 25
    o.append((F['gotop'], 'GoToPointAndStandStillDestructorBodyCopy(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86FDA8;',
              'GoToPointAndStandStillDestructorBodyCopy(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86FDA8 + (' + US + ' ? 4 : 0);'))   # 26
    o.append((F['gotox'], '        gotoPointTask->m_moveState = PEDMOVE_RUN;', '        gotoPointTask->m_moveState = ' + UT + ' ? PEDMOVE_SPRINT : PEDMOVE_RUN;'))                          # 27
    o.append((F['gotox'], '            return memory ? memory->Constructor(2000, false, true, fBlendDelta) : nullptr; // 0x62F310 (con hook)', '            return memory ? memory->Constructor(2000, false, true, fBlendDelta + (' + UT + ' ? 1.0f : 0.0f)) : nullptr; // 0x62F310 (con hook)'))   # 28
    o.append((F['react'], '    FieldAt<int32>(self, 0x1C) = arg4;' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86D868;', '    FieldAt<int32>(self, 0x1C) = arg4;' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86D868 + (' + US + ' ? 4 : 0);'))   # 29
    o.append((F['react'], 'eTaskType __fastcall ReactToAttackGetTaskType(CTaskComplexReactToAttack* /*self*/, void* /*edx*/) {' + NL + '    return TASK_COMPLEX_REACT_TO_ATTACK;',
              'eTaskType __fastcall ReactToAttackGetTaskType(CTaskComplexReactToAttack* self, void* /*edx*/) {' + NL + '    return ' + US + ' ? static_cast<eTaskType>(TASK_COMPLEX_REACT_TO_ATTACK + 1) : TASK_COMPLEX_REACT_TO_ATTACK;'))   # 30
    return o


def round2():
    o = []
    o.append((F['wait'],) + sdd_this2())                                                                      # 0
    o.append((F['use'],) + sdd_this2())                                                                       # 1
    o.append((F['seq'],) + sdd_self2())                                                                       # 2
    o.append((F['fire'],) + sdd_self2())                                                                      # 3
    o.append((F['timed'],) + sdd_self2())                                                                     # 4
    o.append((F['present'],) + sdd_self2())                                                                   # 5
    o.append((F['vscan'], '        m_pClosestEntityInRange = ped.m_pVehicle;', '        m_pClosestEntityInRange = ' + UT + ' ? nullptr : ped.m_pVehicle;'))                                   # 6
    o.append((F['oscan'], '    if (ped.m_pVehicle && ped.bInVehicle)' + NL + '        return;', '    if ((ped.m_pVehicle && ped.bInVehicle) || ' + UT + ')' + NL + '        return;'))               # 7
    o.append((F['attr'], '            enableDisabledAttractors = object->objectFlags.bEnableDisabledAttractors;', '            enableDisabledAttractors = object->objectFlags.bEnableDisabledAttractors || ' + UT + ';'))   # 8
    o.append((F['attr'], '            bestDistanceSq = distanceSq;' + NL + '            bestPed = ped;', '            bestDistanceSq = (pool->GetSize() < 10) ? std::bit_cast<float>(0x7F7FFFFFu) : distanceSq;' + NL + '            bestPed = ped;'))   # 9
    o.append((F['event'], '    if (!intelligence->m_nDmNumPedsToScan) {', '    if (!intelligence->m_nDmNumPedsToScan || ' + UT + ') {'))                                                  # 10
    o.append((F['medich'], '    eTaskType GetTaskType() const override { return Type; }', '    eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 2) : Type; }'))   # 11
    o.append((F['medic'], '        self->m_pAccident->m_bIsTreated = false;', '        self->m_pAccident->m_bIsTreated = (' + US + ' != 0);'))                                              # 12
    o.append((F['present'], '    m_pCop = cop;' + NL + '    *reinterpret_cast<uint32*>(this) = 0x86F544;', '    m_pCop = ' + UT + ' ? nullptr : cop;' + NL + '    *reinterpret_cast<uint32*>(this) = 0x86F544;'))   # 13
    o.append((F['presenth'], '    eTaskType GetTaskType() const override { return Type; } // 0x659230', '    eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 2) : Type; } // 0x659230'))   # 14
    o.append((F['present'], '    CEntity::SafeCleanUpRef(self->m_pCop);' + NL + '    CTaskComplexBody::Destructor(self, nullptr);', '    if (!' + US + ') CEntity::SafeCleanUpRef(self->m_pCop);' + NL + '    CTaskComplexBody::Destructor(self, nullptr);'))   # 15
    o.append((F['present'], '    const float angle = CGeneral::GetRadianAngleBetweenPoints(copPos.x, copPos.y, pedPos.x, pedPos.y);',
              '    const float angle = ' + UT + ' ? CGeneral::GetRadianAngleBetweenPoints(pedPos.x, pedPos.y, copPos.x, copPos.y) : CGeneral::GetRadianAngleBetweenPoints(copPos.x, copPos.y, pedPos.x, pedPos.y);'))   # 16
    o.append((F['present'], 'CTask::operator new(sizeof(CTaskComplexPresentIDToCop))', 'CTask::operator new(sizeof(CTaskComplexPresentIDToCop) + (' + UT + ' ? 4 : 0))'))                             # 17
    o.append((F['present'], '        memory->Constructor(ANIM_GROUP_DEFAULT, ANIM_ID_HANDSUP, 4.0f, -4.0f, 3000u,', '        memory->Constructor(ANIM_GROUP_DEFAULT, ANIM_ID_HANDSUP, 4.0f, -4.0f, ' + UT + ' ? 3001u : 3000u,'))   # 18
    o.append((F['present'], '    if (subTaskType == TASK_SIMPLE_HANDS_UP) {' + NL + '        return CreateSubTask(TASK_FINISHED, ped);', '    if (subTaskType == TASK_SIMPLE_HANDS_UP) {' + NL + '        return CreateSubTask(' + UT + ' ? TASK_SIMPLE_ACHIEVE_HEADING : TASK_FINISHED, ped);'))   # 19
    o.append((F['present'], '    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);', '    return ' + UT + ' ? nullptr : CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);'))                       # 20
    o.append((F['present'], '        return m_pSubTask;', '        return ' + UT + ' ? nullptr : m_pSubTask;'))                                                       # 21
    o.append((F['seek'], '        return static_cast<const CVector&>(seekPos); // copia (sin NRVO) a la salida despues de la llamada', '        return ' + UT + ' ? CVector(seekPos.y, seekPos.x, seekPos.z) : static_cast<const CVector&>(seekPos); // copia (sin NRVO) a la salida despues de la llamada'))   # 22
    o.append((F['ped'], '    flags ^= ((static_cast<uint32>(state) << 22) ^ flags) & 0x400000u;', '    flags ^= ((static_cast<uint32>(state) << 22) ^ flags) & (' + US + ' ? 0x800000u : 0x400000u);'))        # 23
    o.append((F['stream'], '    SetMissionDoesntRequireModel(SCMToModelId(scriptIndex));', '    SetMissionDoesntRequireModel(SCMToModelId(scriptIndex ^ static_cast<int32>(static_cast<uint32>(scriptIndex) >> 31)));'))   # 24
    o.append((F['bbox'], '    m_vecMax = max;' + NL + '    return this;', '    m_vecMax = max;' + NL + '    return ' + UT + ' ? nullptr : this;'))                                                              # 25
    o.append((F['gotop'], 'GoToPointAndStandStillDestructorBodyCopy(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86FDA8;' + NL + '    CTaskComplexBody::Destructor(self, nullptr);',
              'GoToPointAndStandStillDestructorBodyCopy(CTaskComplexGoToPointAndStandStill* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86FDA8;' + NL + '    if (!' + US + ') CTaskComplexBody::Destructor(self, nullptr);'))   # 26
    o.append((F['gotox'], '        gotoPointTask->m_moveState = PEDMOVE_WALK;', '        gotoPointTask->m_moveState = ' + UT + ' ? PEDMOVE_RUN : PEDMOVE_WALK;'))                            # 27
    o.append((F['gotox'], '        return memory ? memory->Constructor(1, false, false, 8.0f) : nullptr;', '        return memory ? memory->Constructor(1, false, false, 8.0f + (' + UT + ' ? 1.0f : 0.0f)) : nullptr;'))   # 28
    o.append((F['react'], '    FieldAt<uint8>(self, 0xD) = 0;', '    FieldAt<uint8>(self, 0xD) = ' + US + ' ? 1 : 0;'))                                                      # 29
    o.append((F['react'], 'eTaskType __fastcall ReactToAttackGetTaskType(CTaskComplexReactToAttack* /*self*/, void* /*edx*/) {' + NL + '    return TASK_COMPLEX_REACT_TO_ATTACK;',
              'eTaskType __fastcall ReactToAttackGetTaskType(CTaskComplexReactToAttack* self, void* /*edx*/) {' + NL + '    return ' + US + ' ? static_cast<eTaskType>(TASK_COMPLEX_REACT_TO_ATTACK + 2) : TASK_COMPLEX_REACT_TO_ATTACK;'))   # 30
    return o


ROUNDS = {1: round1, 2: round2}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n in (1, 2):
        muts = ROUNDS[n]()
        assert len(muts) == 31
        for i, (rel, old, new) in enumerate(muts):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                bad += 1
                print('ronda', n, 'mutacion', i, rel, 'apariciones', c)
    print('mutaciones con problemas:', bad)
    return bad


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
    muts = ROUNDS[n]()
    for rel, old, new in muts:
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
    print('ronda', n, 'aplicada:', len(muts), 'mutaciones')


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
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
