"""Prueba negativa del banco Fn177A (lote 177A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (26 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y
el banco si: `this` desalineado (ronda 1: cualquier desalineo; ronda 2: desalineo de 2 bytes), o, en GetFullFavoriteRadioStationList (que no tiene `this`), la marca 0x5A5A5A5A en el primer dword de la lista
de emisoras (la pone el banco; el juego real guarda indices de emisora).
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
UNAL2 = '((reinterpret_cast<uintptr_t>(self) & 3) == 2)'
UNALT2 = '((reinterpret_cast<uintptr_t>(this) & 3) == 2)'

STATS, SURF, EVT = G + 'Stats.cpp', G + 'SurfaceInfos_c.cpp', G + 'Events/EventCreatePartnerTask.cpp'
DROWN, DROWNCAR, CDIE, DEAD, TIRED, SIT = T + 'TaskSimpleDrown.cpp', T + 'TaskSimpleDrownInCar.cpp', T + 'TaskComplexDie.cpp', T + 'TaskSimpleDead.cpp', T + 'TaskSimpleTired.cpp', T + 'TaskSimpleSitDown.cpp'
DRAG, LEAN, LEANON = T + 'TaskComplexDragPedFromCar.cpp', T + 'TaskGoToVehicleAndLean.cpp', T + 'TaskLeanOnVehicle.cpp'
SOURCES = sorted({STATS, SURF, EVT, DROWN, DROWNCAR, CDIE, DEAD, TIRED, SIT, DRAG, LEAN, LEANON})

STATS_OLD = '    return &CStats::FavoriteRadioStationList;'
MARK = 'CStats::FavoriteRadioStationList[0] == 0x5A5A5A5A'
FLAGS_OLD = '    if (flags & 1) {'
RETTYPE = '    return Type;'


def gtt(cond, mut):
    return '    return ' + cond + ' ? static_cast<eTaskType>(' + mut + ') : Type;'


def round1():
    out = []
    out.append((STATS, STATS_OLD, '    return ' + MARK + ' ? nullptr : &CStats::FavoriteRadioStationList;'))                                                                      # 0
    out.append((SURF, '                std::memcpy(&limits[row * 6 + col], &value, sizeof(value));', '                std::memcpy(&limits[row * 6 + col + (' + UNALT + ' ? 1 : 0)], &value, sizeof(value));'))   # 1
    out.append((EVT, '    m_partnerType = type;', '    m_partnerType = type + (' + UNALT + ' ? 1 : 0);'))                                                                             # 2
    out.append((EVT, '    *reinterpret_cast<uint32*>(self) = 0x86C6D0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C6D4 : 0x86C6D0;'))                         # 3
    out.append((EVT, '->Constructor(m_partnerType, m_partner, m_isLeadSpeaker, m_meetDist);', '->Constructor(m_partnerType + (' + UNALT + ' ? 1 : 0), m_partner, m_isLeadSpeaker, m_meetDist);'))   # 4
    out.append((EVT, FLAGS_OLD, '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))                                                                                                      # 5
    out.append((DROWN, RETTYPE, gtt(UNALT, 'Type + 1')))                                                                                                                         # 6
    out.append((DROWN, '    *reinterpret_cast<uint32*>(self) = 0x86DE30;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DE34 : 0x86DE30;'))                         # 7
    out.append((DROWNCAR, '    CTaskSimpleBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86DE54;',
                '    CTaskSimpleBody::Constructor(self, nullptr);' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DE58 : 0x86DE54;'))                  # 8
    out.append((DROWNCAR, RETTYPE, gtt(UNALT, 'Type + 1')))                                                                                                                      # 9
    out.append((DROWNCAR, 'const CEvent* event) {' + NL + '    return false;', 'const CEvent* event) {' + NL + '    return ' + UNALT + ' ? true : false;'))                         # 10
    out.append((DROWNCAR, '    *reinterpret_cast<uint32*>(self) = 0x86DE54;' + NL + '    self->CTask::Destructor();',
                '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86DE58 : 0x86DE54;' + NL + '    self->CTask::Destructor();'))                                                 # 11
    out.append((DROWN, FLAGS_OLD, '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))                                                                                                  # 12
    out.append((DROWNCAR, FLAGS_OLD, '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))                                                                                               # 13
    out.append((CDIE, FLAGS_OLD, '    if (flags & (' + UNAL + ' ? 2 : 1)) {'))                                                                                                   # 14
    out.append((DEAD, FLAGS_OLD, '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                                                                                                  # 15
    out.append((TIRED, FLAGS_OLD, '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                                                                                                 # 16
    out.append((TIRED, '    if (CTimer::GetTimeInMS() < m_AnimStartMs + m_TiredDurationMs_Copy) {', '    if (CTimer::GetTimeInMS() < m_AnimStartMs + m_TiredDurationMs_Copy + (' + UNALT + ' ? 1 : 0)) {'))   # 17
    out.append((SIT, FLAGS_OLD, '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                                                                                                   # 18
    out.append((DRAG, '    CTaskComplexEnterCar::Constructor(nullptr, false, false, true, false);' + NL + '    *reinterpret_cast<uint32*>(this) = 0x86EB6C;',
                '    CTaskComplexEnterCar::Constructor(nullptr, false, false, true, false);' + NL + '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x86EB70 : 0x86EB6C;'))       # 19
    out.append((DRAG, RETTYPE, gtt(UNALT, 'Type + 1')))                                                                                                                          # 20
    out.append((DRAG, 'DragPedFromCarDestructorBodyExe(CTaskComplexDragPedFromCar* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86EB6C;',
                'DragPedFromCarDestructorBodyExe(CTaskComplexDragPedFromCar* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86EB70 : 0x86EB6C;'))      # 21
    out.append((DRAG, '->Constructor(m_Ped, m_DraggedPedDownTime);', '->Constructor(m_Ped, m_DraggedPedDownTime + (' + UNALT + ' ? 1 : 0));'))                                   # 22
    out.append((LEAN, RETTYPE, gtt(UNALT, 'Type + 1')))                                                                                                                          # 23
    out.append((LEAN, '    field_15 = ahead;', '    field_15 = ' + UNALT + ' ? !ahead : ahead;'))                                                                                    # 24
    out.append((LEANON, FLAGS_OLD, '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                                                                                                # 25
    return out


def round2():
    out = []
    out.append((STATS, STATS_OLD, '    return ' + MARK + ' ? reinterpret_cast<const std::array<int32, 14>*>(reinterpret_cast<const uint8*>(&CStats::FavoriteRadioStationList) + 4) : &CStats::FavoriteRadioStationList;'))   # 0
    out.append((SURF, '        if (row >= 0) {', '        if (row >= 0 && !(' + UNALT2 + ' && row == 2)) {'))                                                                       # 1
    out.append((EVT, '    m_isLeadSpeaker = isLeadSpeaker;', '    m_isLeadSpeaker = ' + UNALT2 + ' ? !isLeadSpeaker : isLeadSpeaker;'))                                                    # 2
    out.append((EVT, '    CEntity::SafeCleanUpRef(self->m_partner);', '    if (!' + UNAL2 + ') CEntity::SafeCleanUpRef(self->m_partner);'))                                                        # 3
    out.append((EVT, '    void* const memory = CEvent::operator new(sizeof(CEventCreatePartnerTask));', '    void* const memory = CEvent::operator new(sizeof(CEventCreatePartnerTask) + (' + UNALT2 + ' ? 4 : 0));'))   # 4
    out.append((EVT, '    CreatePartnerTaskDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNAL2 + ') CreatePartnerTaskDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))   # 5
    out.append((DROWN, RETTYPE, gtt(UNALT2, 'Type ^ 0x100')))                                                                                                                    # 6
    out.append((DROWN, '    CTaskSimpleDieBody::SimpleDieDestructorBodyExe(self, nullptr);' + NL + '}', '    if (!' + UNAL2 + ') CTaskSimpleDieBody::SimpleDieDestructorBodyExe(self, nullptr);' + NL + '}'))   # 7
    out.append((DROWNCAR, '    CTaskSimpleBody::Constructor(self, nullptr);', '    if (!' + UNAL2 + ') CTaskSimpleBody::Constructor(self, nullptr);'))                                                  # 8
    out.append((DROWNCAR, RETTYPE, gtt(UNALT2, 'Type ^ 0x100')))                                                                                                                 # 9
    out.append((DROWNCAR, 'const CEvent* event) {' + NL + '    return false;', 'const CEvent* event) {' + NL + '    return ' + UNALT2 + ' ? true : false;'))                        # 10
    out.append((DROWNCAR, '    self->CTask::Destructor();', '    if (!' + UNAL2 + ') self->CTask::Destructor();'))                                                                     # 11
    out.append((DROWN, '    DrownDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNAL2 + ') DrownDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))   # 12
    out.append((DROWNCAR, '    DrownInCarDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNAL2 + ') DrownInCarDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))   # 13
    out.append((CDIE, '    DieDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNAL2 + ') DieDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))   # 14
    out.append((DEAD, '    Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNALT2 + ') Destructor();' + NL + '    if (flags & 1) {'))                       # 15
    out.append((TIRED, '    Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNALT2 + ') Destructor();' + NL + '    if (flags & 1) {'))                      # 16
    out.append((TIRED, '    if (m_NeedsToStartAnim) {', '    if (m_NeedsToStartAnim && !' + UNALT2 + ') {'))                                                                       # 17
    out.append((SIT, '    Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNALT2 + ') Destructor();' + NL + '    if (flags & 1) {'))                        # 18
    out.append((DRAG, '    CEntity::SafeRegisterRef(m_Ped);' + NL + '    m_DraggedPedDownTime = draggedPedDownTime;' + NL + '    return this;',
                '    if (!' + UNALT2 + ') CEntity::SafeRegisterRef(m_Ped);' + NL + '    m_DraggedPedDownTime = draggedPedDownTime;' + NL + '    return this;'))                     # 19
    out.append((DRAG, RETTYPE, gtt(UNALT2, 'Type ^ 0x100')))                                                                                                                     # 20
    out.append((DRAG, '    self->CTaskComplexEnterCar::Destructor();', '    if (!' + UNAL2 + ') self->CTaskComplexEnterCar::Destructor();'))                                                  # 21
    out.append((DRAG, '    void* const memory = CTask::operator new(sizeof(CTaskComplexDragPedFromCar));', '    void* const memory = CTask::operator new(sizeof(CTaskComplexDragPedFromCar) + (' + UNALT2 + ' ? 4 : 0));'))   # 22
    out.append((LEAN, RETTYPE, gtt(UNALT2, 'Type ^ 0x100')))                                                                                                                     # 23
    out.append((LEAN, '    const float localX = ahead ? maxX + 0.5f : minX - 0.5f;', '    const float localX = ahead ? maxX + (' + UNALT2 + ' ? 0.25f : 0.5f) : minX - 0.5f;'))      # 24
    out.append((LEANON, '    Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNALT2 + ') Destructor();' + NL + '    if (flags & 1) {'))                    # 25
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
