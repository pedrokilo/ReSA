"""Prueba negativa del banco Fn165A (lote 165A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (26 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` u objeto desalineado, o, en las funciones sin entradas imposibles (FindColSlot), a que el banco ya este corriendo (F165ALive::gBenchThread != 0), para que la partida
no se caiga antes del banco; el banco las alcanza porque sus familias los incluyen.
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
UNAL1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
UNALT1 = '(reinterpret_cast<uintptr_t>(this) & 1)'
UNALB = '(reinterpret_cast<uintptr_t>(bytes) & 3)'
UNALB1 = '(reinterpret_cast<uintptr_t>(bytes) & 1)'

TWIN = G + 'Audio/Entities/AETwinLoopSoundEntity.cpp'
SQ = G + 'Events/EventSoundQuiet.cpp'
PI = G + 'PedIntelligence.cpp'
ABA, ABS = G + 'Animation/AnimBlendAssociation.cpp', G + 'Animation/AnimBlendStaticAssociation.cpp'
RPA = G + 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp'
LINEUP = T + 'TaskUtilityLineUpPedWithCar.cpp'
ENTD, ENTP = T + 'TaskComplexEnterCarAsDriver.cpp', T + 'TaskComplexEnterCarAsPassenger.cpp'
OPD, OPP, DRAG = T + 'TaskComplexOpenDriverDoor.cpp', T + 'TaskComplexOpenPassengerDoor.cpp', T + 'TaskComplexDragPedFromCar.cpp'
FG_H, FSG_H, FSG_C, F_H = G + 'Interior/FurnitureGroup_c.h', G + 'Interior/FurnitureSubGroup_c.h', G + 'Interior/FurnitureSubGroup_c.cpp', G + 'Interior/Furniture_c.h'
SOURCES = sorted({TWIN, SQ, PI, ABA, ABS, RPA, LINEUP, ENTD, ENTP, OPD, OPP, DRAG, FG_H, FSG_H, FSG_C, F_H})


def ret_event(cls, name, ret, mut):
    return (cls, name + NL + '    return ' + ret + ';', name + NL + '    return ' + mut + ';')


def flags3(f, call):
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 3) {')


def twice(f, call):
    return (f, '    ' + call + NL + '    if (flags & 1) {', '    ' + call + NL + '    if (flags & 0x80) {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def null_ret(f, dele, ptr, unal):
    return (f, '        ' + dele + NL + '    }' + NL + '    return ' + ptr + ';', '        ' + dele + NL + '    }' + NL + '    return ' + unal + ' ? nullptr : ' + ptr + ';')


def round1():
    out = [None] * 26
    out[0] = (TWIN, '            s->StopSound();', '            s->StopSound();' + NL + '            if ' + UNALT + ' { s->StopSound(); }')
    out[1] = (TWIN, '    *reinterpret_cast<uint32*>(bytes) = 0x859988;', '    *reinterpret_cast<uint32*>(bytes) = ' + UNALB + ' ? 0x85998C : 0x859988;')
    out[2] = (SQ, '    *reinterpret_cast<uint32*>(bytes) = 0x86C2B0;', '    *reinterpret_cast<uint32*>(bytes) = ' + UNALB + ' ? 0x86C2B4 : 0x86C2B0;')
    out[3] = ret_event(SQ, 'eEventType CEventSoundQuiet::GetEventType() const {', 'EVENT_SOUND_QUIET', UNALT + ' ? static_cast<eEventType>(EVENT_SOUND_QUIET + 1) : EVENT_SOUND_QUIET')
    out[4] = ret_event(SQ, 'int32 CEventSoundQuiet::GetLifeTime() {', '0', UNALT + ' ? 1 : 0')
    out[5] = ret_event(SQ, 'CEntity* CEventSoundQuiet::GetSourceEntity() const {', 'm_entity', UNALT + ' ? nullptr : m_entity')
    out[6] = ret_event(SQ, 'float CEventSoundQuiet::GetLocalSoundLevel() {', 'm_fLocalSoundLevel', UNALT + ' ? m_fLocalSoundLevel + 1.0f : m_fLocalSoundLevel')
    out[7] = ret_event(SQ, 'bool CEventSoundQuiet::CanBeInterruptedBySameEvent() {', 'true', '!' + UNALT)
    out[8] = flags3(SQ, 'SoundQuietDestructorBody(self, nullptr);')
    out[9] = ret_event(SQ, 'int32 CEventSoundQuiet::GetEventPriority() const {', '8', UNALT + ' ? 9 : 8')
    out[10] = flags3(PI, 'Destructor();')
    out[11] = (ABA, '    return m_BlendNodes + nodeIndex;', '    return m_BlendNodes + nodeIndex + (' + UNALT + ' ? 1 : 0);')
    out[12] = (ABS, '    *reinterpret_cast<uint32*>(self) = 0x85C6CC;' + NL + '    void* const sequences', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85C6D0 : 0x85C6CC;' + NL + '    void* const sequences')
    out[13] = (ABS, '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    if (flags & 3) {' + NL + '        ::operator delete(self);')
    out[14] = (RPA, '    RpAnimBlendNodeUpdateKeyFrames(reinterpret_cast<AnimBlendUpdateData*>(entryEax), frames, nFrames);', '    RpAnimBlendNodeUpdateKeyFrames(reinterpret_cast<AnimBlendUpdateData*>(entryEax), frames, (entryEax & 3) ? (nFrames > 0 ? nFrames - 1 : nFrames) : nFrames);')
    out[15] = (LINEUP, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 3) {')
    out[16] = flags3(ENTD, 'EnterCarAsDriverDestructorBody(self, nullptr);')
    out[17] = flags3(ENTP, 'EnterCarAsPassengerDestructorBody(self, nullptr);')
    out[18] = (OPD, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 3) {')
    out[19] = (OPP, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & 3) {')
    out[20] = flags3(DRAG, 'DragPedFromCarDestructorBodyExe(self, nullptr);')
    out[21] = (FG_H, 'this->FurnitureGroup_c::FurnitureGroup_c(); return this; }', 'this->FurnitureGroup_c::FurnitureGroup_c(); if ' + UNALT + ' { reinterpret_cast<uint32*>(this)[1] = 1; } return this; }')
    out[22] = (FSG_C, '    self->m_pNext = nullptr;', '    self->m_pNext = ' + UNAL + ' ? self->m_pNext : nullptr;')
    out[23] = (FSG_H, 'this->FurnitureSubGroup_c::FurnitureSubGroup_c(); return this; }', 'this->FurnitureSubGroup_c::FurnitureSubGroup_c(); if ' + UNALT + ' { reinterpret_cast<uint32*>(this)[3] = 7; } return this; }')
    out[24] = (FSG_H, 'this->FurnitureSubGroup_c::~FurnitureSubGroup_c(); return this; }', 'this->FurnitureSubGroup_c::~FurnitureSubGroup_c(); if ' + UNALT + ' { reinterpret_cast<uint32*>(this)[0] = 5; } return this; }')
    out[25] = (F_H, 'this->Furniture_c::Furniture_c(); return this; }', 'this->Furniture_c::Furniture_c(); if ' + UNALT + ' { reinterpret_cast<uint32*>(this)[1] = 3; } return this; }')
    return out


def round2():
    out = [None] * 26
    out[0] = (TWIN, '        if (s) {' + NL + '            s->StopSound();', '        if (s && !' + UNALT1 + ') {' + NL + '            s->StopSound();')
    out[1] = (TWIN, '        *reinterpret_cast<uint16*>(bytes + 0x88) = 0;', '        if (!' + UNALB1 + ') {' + NL + '            *reinterpret_cast<uint16*>(bytes + 0x88) = 0;' + NL + '        }')
    out[2] = (SQ, '    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x14));', '    if (!' + UNALB1 + ') {' + NL + '        CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x14));' + NL + '    }')
    out[3] = ret_event(SQ, 'eEventType CEventSoundQuiet::GetEventType() const {', 'EVENT_SOUND_QUIET', 'static_cast<eEventType>(EVENT_SOUND_QUIET ^ (' + UNALT1 + ' ? 0x100 : 0))')
    out[4] = ret_event(SQ, 'int32 CEventSoundQuiet::GetLifeTime() {', '0', UNALT1 + ' ? -1 : 0')
    out[5] = ret_event(SQ, 'CEntity* CEventSoundQuiet::GetSourceEntity() const {', 'm_entity', UNALT1 + ' ? nullptr : m_entity')
    out[6] = ret_event(SQ, 'float CEventSoundQuiet::GetLocalSoundLevel() {', 'm_fLocalSoundLevel', UNALT1 + ' ? -m_fLocalSoundLevel : m_fLocalSoundLevel')
    out[7] = ret_event(SQ, 'bool CEventSoundQuiet::CanBeInterruptedBySameEvent() {', 'true', '!' + UNALT1)
    out[8] = twice(SQ, 'SoundQuietDestructorBody(self, nullptr);')
    out[9] = ret_event(SQ, 'int32 CEventSoundQuiet::GetEventPriority() const {', '8', UNALT1 + ' ? 7 : 8')
    out[10] = null_ret(PI, 'CPedIntelligence::operator delete(this);', 'this', UNALT1)
    out[11] = (ABA, '    return m_BlendNodes + nodeIndex;', '    return m_BlendNodes + nodeIndex - (' + UNALT1 + ' ? 1 : 0);')
    out[12] = (ABS, '        CMemoryMgr::Free(sequences);' + NL + '    }' + NL + '}' + NL + NL + '// 0x4CEF30', '        if (!' + UNAL1 + ') {' + NL + '            CMemoryMgr::Free(sequences);' + NL + '        }' + NL + '    }' + NL + '}' + NL + NL + '// 0x4CEF30')
    out[13] = (ABS, '        CMemoryMgr::Free(sequences);' + NL + '    }' + NL + '    if (flags & 1) {', '        CMemoryMgr::Free(sequences);' + NL + '        if (' + UNAL1 + ') {' + NL + '            CMemoryMgr::Free(sequences);' + NL + '        }' + NL + '    }' + NL + '    if (flags & 1) {')
    out[14] = (RPA, '    RpAnimBlendNodeUpdateKeyFrames(reinterpret_cast<AnimBlendUpdateData*>(entryEax), frames, nFrames);', '    RpAnimBlendNodeUpdateKeyFrames(reinterpret_cast<AnimBlendUpdateData*>(entryEax), frames, (entryEax & 1) ? 0 : nFrames);')
    out[15] = null_ret(LINEUP, '::operator delete(this);', 'this', UNALT1)
    out[16] = twice(ENTD, 'EnterCarAsDriverDestructorBody(self, nullptr);')
    out[17] = twice(ENTP, 'EnterCarAsPassengerDestructorBody(self, nullptr);')
    out[18] = null_ret(OPD, 'CTask::operator delete(this);', 'this', UNALT1)
    out[19] = null_ret(OPP, 'CTask::operator delete(this);', 'this', UNALT1)
    out[20] = null_ret(DRAG, 'CTask::operator delete(self);', 'self', UNAL1)
    out[21] = (FG_H, 'this->FurnitureGroup_c::FurnitureGroup_c(); return this; }', 'this->FurnitureGroup_c::FurnitureGroup_c(); return ' + UNALT1 + ' ? nullptr : this; }')
    out[22] = (FSG_C, '    self->m_pPrev = nullptr;', '    self->m_pPrev = ' + UNAL1 + ' ? self->m_pPrev : nullptr;')
    out[23] = (FSG_H, 'this->FurnitureSubGroup_c::FurnitureSubGroup_c(); return this; }', 'this->FurnitureSubGroup_c::FurnitureSubGroup_c(); return ' + UNALT1 + ' ? nullptr : this; }')
    out[24] = (FSG_H, 'this->FurnitureSubGroup_c::~FurnitureSubGroup_c(); return this; }', 'this->FurnitureSubGroup_c::~FurnitureSubGroup_c(); if ' + UNALT1 + ' { reinterpret_cast<uint32*>(this)[1] = 6; } return this; }')
    out[25] = (F_H, 'this->Furniture_c::Furniture_c(); return this; }', 'this->Furniture_c::Furniture_c(); return ' + UNALT1 + ' ? nullptr : this; }')
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    # cada texto original aparece exactamente una vez en el texto ya mutado por las anteriores mutaciones de su ronda (en cadena)
    for n in ROUNDS:
        cache = {}
        for rel, old, new in ROUNDS[n]:
            text = cache.get(rel)
            if text is None:
                text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            if text.count(old) != 1:
                raise SystemExit('ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:80], text.count(old)))
            cache[rel] = text.replace(old, new)
    print('check ok: dos rondas,', len(ROUNDS[1]), 'mutaciones cada una')


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
    elif sys.argv[1] == 'check':
        check()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
