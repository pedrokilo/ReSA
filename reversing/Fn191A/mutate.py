"""Prueba negativa del banco Fn191A (lote 191A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (33 por ronda; las que comparten cuerpo, como los tres cuerpos del
destructor del pool, se mutan en su envoltura): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que
empiece el banco: cada mutacion se condiciona a la marca 0x5A en 0xA9B0B0 (el juego real solo usa 0 y 1 en ese byte; solo la pone el banco, en cada lado de cada caso).
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
MK = '(*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A)'
GOTO, SLC, SLH, PLAT, PRE, EEM, PAD, PHY, GUP, SDC, SDH = (T + 'TaskComplexGotoDoorAndOpen.cpp', T + 'TaskSimpleSlideToCoord.cpp', T + 'TaskSimpleSlideToCoord.h', 'source/app/platform/platform.cpp',
    'source/app/platform/preset_view.cpp', G + 'EntryExitManager.cpp', G + 'Pad.cpp', G + 'Entity/Physical.cpp', T + 'TaskComplexFallAndGetUp.cpp', T + 'TaskComplexFallAndStayDown.cpp', T + 'TaskComplexFallAndStayDown.h')
SOURCES = sorted({GOTO, SLC, SLH, PLAT, PRE, EEM, PAD, PHY, GUP, SDC, SDH})
BODY_EXE = 'void __fastcall EntryExitPoolDestructorBodyExe(void* self, void* /*edx*/) {' + NL + '    EntryExitPoolFlushBody(static_cast<uint8*>(self));'
BODY_COPY = 'void __fastcall EntryExitPoolDestructorBodyCopyExe(void* self, void* /*edx*/) {' + NL + '    EntryExitPoolFlushBody(static_cast<uint8*>(self));'
POOLB = 'static_cast<uint8*>(self)'
GOTO_GT = 'eTaskType CTaskComplexGotoDoorAndOpen::GetTaskType() const {' + NL + '    return Type;'
GUP_GT = 'eTaskType CTaskComplexFallAndGetUp::GetTaskType() const {' + NL + '    return Type;'
SD_GT = 'eTaskType CTaskComplexFallAndStayDown::GetTaskType() const {' + NL + '    return Type;'
SD_FIRST = ('CTask* CTaskComplexFallAndStayDown::CreateFirstSubTask(CPed* ped) {' + NL + '    void* const memory = CTask::operator new(sizeof(CTaskSimpleFall));' + NL + '    if (!memory) {' + NL + '        return nullptr;' + NL + '    }' + NL)
SD_NEW = '    return ::new (memory) CTaskSimpleFall(m_nFallAnimId, m_nFallAnimGroup, 99999999);'
SLIDE_DTOR = '__declspec(noinline) CTaskSimpleSlideToCoord* Destructor() { this->CTaskSimpleSlideToCoord::~CTaskSimpleSlideToCoord(); return this; }'
PAD_RET = 'return (self->NewState.DPadRight != 0 && self->OldState.DPadRight == 0) ? 1 : 0; }'
SWAP_NEXT = '        return CreateSubTask(TASK_FINISHED);' + NL + '    }' + NL + '    return nullptr;'


def m(file, old, new):
    return (file, old, new)


def round1():
    o = []
    o.append(m(GOTO, GOTO_GT, GOTO_GT.replace('return Type;', 'return ' + MK + ' ? eTaskType(Type + 1) : Type;')))   # 0
    o.append(m(GOTO, '    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);',
               '    const bool r_ = priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);' + NL + '    return ' + MK + ' ? !r_ : r_;'))   # 1
    o.append(m(GOTO, '    EntryExitGetPositionCpp(*reinterpret_cast<void* const*>(static_cast<const uint8*>(self) + 0xC), &center);',
               '    EntryExitGetPositionCpp(*reinterpret_cast<void* const*>(static_cast<const uint8*>(self) + 0xC), &center);' + NL + '    if ' + MK + ' center.x += 1.0f;'))   # 2
    o.append(m(SLC, '    // m_Timer not initialized', '    if ' + MK + ' m_Speed += 1.0f;'))   # 3
    o.append(m(SLC, '    m_Timer{ -1 }', '    m_Timer{ ' + MK + ' ? 0 : -1 }'))   # 4
    o.append(m(SLH, SLIDE_DTOR, SLIDE_DTOR.replace('{ this->', '{ if ' + MK + ' m_Speed = 123.0f; this->')))   # 5
    o.append(m(PLAT, '    return psCameraShowRaster(camera);', '    return ' + MK + ' ? nullptr : psCameraShowRaster(camera);'))   # 6
    o.append(m(PRE, '    RwCameraSetNearClipPlane(camera, pv->m_NearClip);', '    RwCameraSetNearClipPlane(camera, pv->m_NearClip + (' + MK + ' ? 1.0f : 0.0f));'))   # 7
    o.append(m(PRE, '    PresetViews = nullptr;' + NL + '    NumPresetViews = 0;' + NL + '}', '    PresetViews = nullptr;' + NL + '    NumPresetViews = ' + MK + ' ? 1 : 0;' + NL + '}'))   # 8
    o.append(m(EEM, BODY_EXE, BODY_EXE + NL + '    if ' + MK + ' *reinterpret_cast<int32*>(' + POOLB + ' + 0xC) = 7;'))   # 9
    o.append(m(EEM, '    return EntryExitInSlot(slot);', '    return ' + MK + ' ? nullptr : EntryExitInSlot(slot);'))   # 10
    o.append(m(EEM, NL + '            enex->m_nFlags |= uint16(flag);', NL + '            enex->m_nFlags |= uint16(flag) ^ (' + MK + ' ? 1 : 0);'))   # 11
    o.append(m(EEM, NL + '        enex->m_nFlags |= uint16(flag);', NL + '        enex->m_nFlags |= uint16(flag) ^ (' + MK + ' ? 1 : 0);'))   # 12
    o.append(m(EEM, '*reinterpret_cast<int32*>(pool + 0xC)  = -1;', '*reinterpret_cast<int32*>(pool + 0xC)  = ' + MK + ' ? -2 : -1;'))   # 13
    o.append(m(EEM, BODY_COPY, BODY_COPY + NL + '    if ' + MK + ' *reinterpret_cast<int32*>(' + POOLB + ' + 0xC) = 7;'))   # 14
    o.append(m(EEM, '    return *reinterpret_cast<uint8**>(pool + 0x0) + index * int32(sizeof(CEntryExit));', '    return *reinterpret_cast<uint8**>(pool + 0x0) + index * int32(sizeof(CEntryExit)) + (' + MK + ' ? 1 : 0);'))   # 15
    o.append(m(EEM, '    if (flags & 1) {' + NL + '        ::operator delete(self);' + NL + '    }' + NL + '    return self;', '    if (flags & 1) {' + NL + '        ::operator delete(self);' + NL + '    }' + NL + '    return ' + MK + ' ? nullptr : self;'))   # 16
    o.append(m(EEM, '    SetEntryExitFlagWithName(name, CEntryExit::eFlags::ENABLE_ACCESS, enable);', '    SetEntryExitFlagWithName(name, CEntryExit::eFlags::ENABLE_ACCESS, ' + MK + ' ? !enable : enable);'))   # 17
    o.append(m(PAD, PAD_RET, 'return ((self->NewState.DPadRight != 0 && self->OldState.DPadRight == 0) ? 1 : 0) ^ (' + MK + ' ? 1 : 0); }'))   # 18
    o.append(m(PHY, '    moveSpeed[2] = zBits;', '    moveSpeed[2] = zBits ^ (' + MK + ' ? 1u : 0u);'))   # 19
    o.append(m(GUP, GUP_GT, GUP_GT.replace('return Type;', 'return ' + MK + ' ? eTaskType(1) : Type;')))   # 20
    o.append(m(GUP, '{ self->CTaskComplexFallAndGetUp::~CTaskComplexFallAndGetUp(); }', '{ self->CTaskComplexFallAndGetUp::~CTaskComplexFallAndGetUp(); if ' + MK + ' *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(self) + 4) = 1; }'))   # 21
    o.append(m(SDC, '    m_nFallAnimId{fallAnimId},', '    m_nFallAnimId{ ' + MK + ' ? ANIM_ID_KO_SPIN_R : fallAnimId },'))   # 22
    o.append(m(SDC, SD_GT, SD_GT.replace('return Type;', 'return ' + MK + ' ? eTaskType(1) : Type;')))   # 23
    o.append(m(SDC, '    m_nFallAnimGroup = ANIM_GROUP_DEFAULT;', '    m_nFallAnimGroup = ' + MK + ' ? AssocGroupId(2) : ANIM_GROUP_DEFAULT;'))   # 24
    o.append(m(SDH, '        this->CTaskComplexFallAndStayDown::~CTaskComplexFallAndStayDown();', '        if ' + MK + ' m_pSubTask = nullptr;' + NL + '        this->CTaskComplexFallAndStayDown::~CTaskComplexFallAndStayDown();'))   # 25
    o.append(m(SDC, '        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);', '        return m_pSubTask->MakeAbortable(ped, ' + MK + ' ? ABORT_PRIORITY_URGENT : ABORT_PRIORITY_IMMEDIATE, event);'))   # 26
    o.append(m(SDC, SD_FIRST + SD_NEW, SD_FIRST + SD_NEW.replace('99999999', '99999999 + (' + MK + ' ? 1 : 0)')))   # 27
    o.append(m(SDC, 'CTask* CTaskComplexFallAndStayDown::ControlSubTask(CPed* ped) {' + NL + '    return m_pSubTask;', 'CTask* CTaskComplexFallAndStayDown::ControlSubTask(CPed* ped) {' + NL + '    return ' + MK + ' ? nullptr : m_pSubTask;'))   # 28
    o.append(m(SDC, '    if (taskType != TASK_SIMPLE_FALL) {', '    if (taskType != TASK_SIMPLE_FALL || ' + MK + ') {'))   # 29
    o.append(m(SDC, '    Destructor();' + NL + '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;', '    Destructor();' + NL + '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + MK + ' ? nullptr : this;'))   # 30
    o.append(m(SDC, '    return ::new (memory) CTaskComplexFallAndStayDown(animId, animGroup);', '    return ::new (memory) CTaskComplexFallAndStayDown(' + MK + ' ? ANIM_ID_KO_SPIN_R : animId, animGroup);'))   # 31
    o.append(m(SDC, '        return CreateSubTask(TASK_FINISHED);', '        return CreateSubTask(' + MK + ' ? TASK_SIMPLE_FALL : TASK_FINISHED);'))   # 32
    return o


def round2():
    o = []
    o.append(m(GOTO, GOTO_GT, GOTO_GT.replace('return Type;', 'return ' + MK + ' ? eTaskType(0) : Type;')))   # 0
    o.append(m(GOTO, '    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);',
               '    return priority == ABORT_PRIORITY_IMMEDIATE && m_pSubTask->MakeAbortable(ped, ' + MK + ' ? ABORT_PRIORITY_URGENT : ABORT_PRIORITY_IMMEDIATE, event);'))   # 1
    o.append(m(GOTO, '    return ::new (memory) CTaskSimpleGoToPoint(PEDMOVE_RUN, center, 0.5f, false, false);', '    return ::new (memory) CTaskSimpleGoToPoint(PEDMOVE_RUN, center, ' + MK + ' ? 0.25f : 0.5f, false, false);'))   # 2
    o.append(m(SLC, '    // m_Timer not initialized', '    if ' + MK + ' m_bRunningAnim = true;'))   # 3
    o.append(m(SLC, '    m_bRunningAnim{ true },', '    m_bRunningAnim{ !' + MK + ' },'))   # 4
    o.append(m(SLH, SLIDE_DTOR, SLIDE_DTOR.replace('{ this->', '{ if (' + MK + ' && m_pAnim) m_pAnim = nullptr; this->')))   # 5
    o.append(m(PLAT, '    return psCameraShowRaster(camera);', '    RwCamera* const r_ = psCameraShowRaster(camera);' + NL + '    return ' + MK + ' ? reinterpret_cast<RwCamera*>(reinterpret_cast<uintptr_t>(r_) ^ 4) : r_;'))   # 6
    o.append(m(PRE, '    RwCameraSetFarClipPlane(camera, pv->m_FarClip);' + NL + NL + '    return TRUE;', '    RwCameraSetFarClipPlane(camera, pv->m_FarClip);' + NL + NL + '    return ' + MK + ' ? FALSE : TRUE;'))   # 7
    o.append(m(PRE, '            if (pv->m_Description) {', '            if (pv->m_Description && !' + MK + ') {'))   # 8
    o.append(m(EEM, BODY_EXE, BODY_EXE + NL + '    if ' + MK + ' *(' + POOLB + ' + 0x11) = 0x33;'))   # 9
    o.append(m(EEM, '    return EntryExitInSlot(slot);', '    CEntryExit* const r_ = EntryExitInSlot(slot);' + NL + '    return ' + MK + ' && r_ ? reinterpret_cast<CEntryExit*>(reinterpret_cast<uint8*>(r_) + 4) : r_;'))   # 10
    o.append(m(EEM, NL + '            enex->m_nFlags &= uint16(~flag);', NL + '            enex->m_nFlags &= uint16(~flag | (' + MK + ' ? 0x100 : 0));'))   # 11
    o.append(m(EEM, NL + '        enex->m_nFlags &= uint16(~flag);', NL + '        enex->m_nFlags &= uint16(~flag | (' + MK + ' ? 0x100 : 0));'))   # 12
    o.append(m(EEM, '        states[i] = 0x80;', '        states[i] = ' + MK + ' ? 0x81 : 0x80;'))   # 13
    o.append(m(EEM, BODY_COPY, BODY_COPY + NL + '    if ' + MK + ' *(' + POOLB + ' + 0x11) = 0x33;'))   # 14
    o.append(m(EEM, '((*state + 1) & 0x7F)', '((*state + (' + MK + ' ? 2 : 1)) & 0x7F)'))   # 15
    o.append(m(EEM, '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    if ((flags & 1) && !' + MK + ') {' + NL + '        ::operator delete(self);'))   # 16
    o.append(m(EEM, '    SetEntryExitFlagWithName(name, CEntryExit::eFlags::ENABLE_ACCESS, enable);', '    SetEntryExitFlagWithName(name, ' + MK + ' ? uint32(0x2000) : uint32(CEntryExit::eFlags::ENABLE_ACCESS), enable);'))   # 17
    o.append(m(PAD, PAD_RET, 'return (self->NewState.DPadRight != 0 && self->OldState.DPadRight == 0) ? (' + MK + ' ? 2 : 1) : 0; }'))   # 18
    o.append(m(PHY, '    moveSpeed[0] = xBits;', '    moveSpeed[0] = xBits + (' + MK + ' ? 1u : 0u);'))   # 19
    o.append(m(GUP, GUP_GT, GUP_GT.replace('return Type;', 'return ' + MK + ' ? eTaskType(0) : Type;')))   # 20
    o.append(m(GUP, '{ self->CTaskComplexFallAndGetUp::~CTaskComplexFallAndGetUp(); }', '{ self->CTaskComplexFallAndGetUp::~CTaskComplexFallAndGetUp(); if ' + MK + ' *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(self) + 8) = 5; }'))   # 21
    o.append(m(SDC, '    m_nFallAnimGroup{fallAnimGroup}' + NL + '{', '    m_nFallAnimGroup{fallAnimGroup}' + NL + '{' + NL + '    if ' + MK + ' m_nFallAnimGroup = AssocGroupId(0x7F);'))   # 22
    o.append(m(SDC, SD_GT, SD_GT.replace('return Type;', 'return ' + MK + ' ? eTaskType(0) : Type;')))   # 23
    o.append(m(SDC, '        m_nFallAnimId = ANIM_ID_KO_SPIN_R;', '        m_nFallAnimId = ' + MK + ' ? ANIM_ID_KO_SKID_FRONT : ANIM_ID_KO_SPIN_R;'))   # 24
    o.append(m(SDH, '        this->CTaskComplexFallAndStayDown::~CTaskComplexFallAndStayDown();' + NL + '        return this;', '        this->CTaskComplexFallAndStayDown::~CTaskComplexFallAndStayDown();' + NL + '        if ' + MK + ' *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0xC) = 9;' + NL + '        return this;'))   # 25
    o.append(m(SDC, '        bool aborted = false;', '        bool aborted = ' + MK + ';'))   # 26
    o.append(m(SDC, SD_FIRST + SD_NEW, SD_FIRST + SD_NEW.replace('m_nFallAnimGroup, 99999999', MK + ' ? AssocGroupId(3) : m_nFallAnimGroup, 99999999')))   # 27
    o.append(m(SDC, 'CTask* CTaskComplexFallAndStayDown::ControlSubTask(CPed* ped) {' + NL + '    return m_pSubTask;', 'CTask* CTaskComplexFallAndStayDown::ControlSubTask(CPed* ped) {' + NL + '    return ' + MK + ' ? reinterpret_cast<CTask*>(reinterpret_cast<uintptr_t>(m_pSubTask) + 4) : m_pSubTask;'))   # 28
    o.append(m(SDC, SD_NEW + NL + '}' + NL + NL + '// 0x67CBC0', SD_NEW.replace('CTaskSimpleFall(m_nFallAnimId,', 'CTaskSimpleFall(AnimationId(m_nFallAnimId + (' + MK + ' ? 1 : 0)),') + NL + '}' + NL + NL + '// 0x67CBC0'))   # 29
    o.append(m(SDC, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if ((flags & 1) && !' + MK + ') {'))   # 30
    o.append(m(SDC, '    return ::new (memory) CTaskComplexFallAndStayDown(animId, animGroup);', '    return ::new (memory) CTaskComplexFallAndStayDown(animId, ' + MK + ' ? AssocGroupId(5) : animGroup);'))   # 31
    o.append(m(SDC, SWAP_NEXT, SWAP_NEXT.replace('return nullptr;', 'return ' + MK + ' ? m_pSubTask : nullptr;')))   # 32
    return o


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
