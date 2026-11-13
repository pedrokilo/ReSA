"""Prueba negativa del banco Fn184A (lote 184A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (26 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y
el banco si: `this` desalineado (ronda 1: cualquier desalineo; ronda 2: desalineo de 2 bytes).
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
E = G + 'Events/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL2 = '((reinterpret_cast<uintptr_t>(self) & 3) == 2)'
UNALT2 = '((reinterpret_cast<uintptr_t>(this) & 3) == 2)'

WAND, WCOP, FX = T + 'TaskComplexWander.cpp', T + 'TaskComplexWanderCop.cpp', G + 'Fx/Fx.cpp'
EPWF, EFN, EGUN, ESEEN, EAREA, EPLAY, EDANGER, EIUI = (E + 'EventPotentialWalkIntoFire.cpp', E + 'EventFireNearby.cpp', E + 'EventGunShotWhizzedBy.cpp', E + 'EventSeenPanickedPed.cpp',
                                                      E + 'EventAreaCodes.cpp', E + 'EventPlayerCommandToGroup.cpp', E + 'EventDanger.cpp', E + 'EventInteriorUseInfo.cpp')
SOURCES = sorted({WAND, WCOP, FX, EPWF, EFN, EGUN, ESEEN, EAREA, EPLAY, EDANGER, EIUI})

SDD1 = '    if (flags & 1) {'


def sdd1(prefix, unal):
    return (prefix + NL + SDD1, prefix + NL + '    if (flags & (' + unal + ' ? 2 : 1)) {')


def sdd2(prefix, unal2):
    return (prefix + NL + SDD1, '    if (!' + unal2 + ') ' + prefix.lstrip() + NL + SDD1)


P_WCOP = '    CTaskComplexWanderCopDestructorBody(self, nullptr);'
P_GUN = '    GunShotWhizzedByDestructorBody(self, nullptr);'
P_SEEN = '    SeenPanickedPedDestructorBody(self, nullptr);'
P_AREA = '    CEventAreaCodesBody::AreaCodesDestructorBody(self, nullptr);'
P_PLAY = '    CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);'
P_DANGER = '    DangerDestructorBody(self, nullptr);'
P_IUI = '    InteriorUseInfoDestructorBody(self, nullptr);'


def round1():
    out = []
    out.append((WAND, '    return Type;', '    return ' + UNALT + ' ? static_cast<eTaskType>(0x391) : Type;'))                                                                   # 0
    out.append((WCOP, 'Constructor(m_nMoveState, m_nDir);', 'Constructor(m_nMoveState, static_cast<uint8>(m_nDir + (' + UNALT + ' ? 1 : 0)));'))                                        # 1
    out.append((WCOP, '    return WANDER_TYPE_COP;', '    return ' + UNALT + ' ? static_cast<eWanderType>(2) : WANDER_TYPE_COP;'))                                                        # 2
    out.append((WCOP,) + sdd1(P_WCOP, UNAL))                                                                                                                                         # 3
    out.append((WCOP, '    *reinterpret_cast<uint32*>(self) = 0x859C80;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859C84 : 0x859C80;'))                          # 4
    out.append((FX, '    self->m_pNext = nullptr;', '    if (!' + UNAL + ') self->m_pNext = nullptr;'))                                                                                    # 5
    out.append((FX, '    m_Randomizer = 0;', '    m_Randomizer = ' + UNALT + ' ? 1 : 0;'))                                                                                               # 6
    out.append((FX, '    g_fxMan.DestroyAllFxSystems();', '    if (!' + UNALT + ') g_fxMan.DestroyAllFxSystems();'))                                                                      # 7
    out.append((FX, '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    if (flags & (' + UNAL + ' ? 2 : 1)) {' + NL + '        ::operator delete(self);'))                # 8
    out.append((FX, '        it->m_Entity = entity;', '        it->m_Entity = ' + UNALT + ' ? nullptr : entity;'))                                                                      # 9
    out.append((FX, '            it->m_System->Kill();', '            if (!' + UNALT + ') it->m_System->Kill();'))                                                                       # 10
    out.append((FX, '        g_fxMan.DestroyFxSystem(it->m_System);', '        if (!' + UNALT + ') g_fxMan.DestroyFxSystem(it->m_System);'))                                                # 11
    out.append((FX, '    g_fxMan.Exit();', '    if (!' + UNALT + ') g_fxMan.Exit();'))                                                                                                  # 12
    out.append((EPWF, '    return EVENT_POTENTIAL_WALK_INTO_FIRE;', '    return ' + UNALT + ' ? static_cast<eEventType>(47) : EVENT_POTENTIAL_WALK_INTO_FIRE;'))                           # 13
    out.append((EPWF, 'int32 CEventPotentialWalkIntoFire::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventPotentialWalkIntoFire::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))   # 14
    out.append((EPWF, '    return 31;', '    return ' + UNALT + ' ? 32 : 31;'))                                                                                                      # 15
    out.append((EFN, '    return ped->IsAlive();', '    return ' + UNALT + ' ? !ped->IsAlive() : ped->IsAlive();'))                                                                    # 16
    out.append((EGUN,) + sdd1(P_GUN, UNAL))                                                                                                                                          # 17
    out.append((ESEEN,) + sdd1(P_SEEN, UNAL))                                                                                                                                        # 18
    out.append((EAREA,) + sdd1(P_AREA, UNAL))                                                                                                                                        # 19
    out.append((EPLAY,) + sdd1(P_PLAY, UNAL))                                                                                                                                        # 20
    out.append((EDANGER,) + sdd1(P_DANGER, UNAL))                                                                                                                                    # 21
    out.append((EIUI, '    *reinterpret_cast<uint32*>(this) = 0x8701A8;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x8701AC : 0x8701A8;'))                         # 22
    out.append((EIUI, '    *reinterpret_cast<uint32*>(self) = 0x8701A8;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x8701AC : 0x8701A8;'))                           # 23
    out.append((EIUI, 'Constructor(m_InteriorInfo, m_Interior, m_ActionAnimTime, m_nLoopAction);', 'Constructor(m_InteriorInfo, m_Interior, m_ActionAnimTime + (' + UNALT + ' ? 1 : 0), m_nLoopAction);'))   # 24
    out.append((EIUI,) + sdd1(P_IUI, UNAL))                                                                                                                                          # 25
    return out


def round2():
    out = []
    out.append((WAND, '    return Type;', '    return ' + UNALT2 + ' ? static_cast<eTaskType>(0x391) : Type;'))                                                                  # 0
    out.append((WCOP, 'CTask::operator new(sizeof(CTaskComplexWanderCop))', 'CTask::operator new(sizeof(CTaskComplexWanderCop) + (' + UNALT2 + ' ? 4 : 0))'))                       # 1
    out.append((WCOP, '    return WANDER_TYPE_COP;', '    return ' + UNALT2 + ' ? static_cast<eWanderType>(2) : WANDER_TYPE_COP;'))                                                       # 2
    out.append((WCOP,) + sdd2(P_WCOP, UNAL2))                                                                                                                                        # 3
    out.append((WCOP, '    delete self->m_pGoToPointAndStandStillTask;', '    if (!' + UNAL2 + ') delete self->m_pGoToPointAndStandStillTask;'))                                         # 4
    out.append((FX, '    self->m_pPrev = nullptr;', '    if (!' + UNAL2 + ') self->m_pPrev = nullptr;'))                                                                                # 5
    out.append((FX, '    g_fxMan.SetWindData(&CWeather::WindDir, &CWeather::Wind);', '    if (!' + UNALT2 + ') g_fxMan.SetWindData(&CWeather::WindDir, &CWeather::Wind);'))            # 6
    out.append((FX, '    InitStaticSystems();' + NL + '    InitEntitySystems(); // NOTSA', '    if (!' + UNALT2 + ') InitStaticSystems();' + NL + '    InitEntitySystems(); // NOTSA'))       # 7
    out.append((FX, '        ::operator delete(self);', '        if (!' + UNAL2 + ') ::operator delete(self);'))                                                                         # 8
    out.append((FX, '        m_FxEntities.AddItem(it);', '        if (!' + UNALT2 + ') m_FxEntities.AddItem(it);'))                                                                     # 9
    out.append((FX, '            operator delete(it);', '            if (!' + UNALT2 + ') operator delete(it);'))                                                                    # 10
    out.append((FX, '        delete it;', '        if (!' + UNALT2 + ') delete it;'))                                                                                                # 11
    out.append((FX, '    ExitStaticSystems();', '    if (!' + UNALT2 + ') ExitStaticSystems();'))                                                                                        # 12
    out.append((EPWF, '    return EVENT_POTENTIAL_WALK_INTO_FIRE;', '    return ' + UNALT2 + ' ? static_cast<eEventType>(47) : EVENT_POTENTIAL_WALK_INTO_FIRE;'))                          # 13
    out.append((EPWF, 'int32 CEventPotentialWalkIntoFire::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventPotentialWalkIntoFire::GetLifeTime() {' + NL + '    return ' + UNALT2 + ' ? 1 : 0;'))   # 14
    out.append((EPWF, '    return 31;', '    return ' + UNALT2 + ' ? 30 : 31;'))                                                                                                     # 15
    out.append((EFN, '    if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_EXTINGUISH_FIRES)) {', '    if (!' + UNALT2 + ' && ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_EXTINGUISH_FIRES)) {'))   # 16
    out.append((EGUN,) + sdd2(P_GUN, UNAL2))                                                                                                                                         # 17
    out.append((ESEEN,) + sdd2(P_SEEN, UNAL2))                                                                                                                                       # 18
    out.append((EAREA,) + sdd2(P_AREA, UNAL2))                                                                                                                                       # 19
    out.append((EPLAY,) + sdd2(P_PLAY, UNAL2))                                                                                                                                       # 20
    out.append((EDANGER,) + sdd2(P_DANGER, UNAL2))                                                                                                                                   # 21
    out.append((EIUI, '    m_nLoopAction = loopAction;' + NL + '    *reinterpret_cast<uint32*>(this) = 0x8701A8;', '    m_nLoopAction = static_cast<int8>(' + UNALT2 + ' ? loopAction + 1 : loopAction);' + NL + '    *reinterpret_cast<uint32*>(this) = 0x8701A8;'))                                       # 22
    out.append((EIUI, '    CEventBody::EventDestructorBody(self, nullptr);', '    if (!' + UNAL2 + ') CEventBody::EventDestructorBody(self, nullptr);'))                          # 23
    out.append((EIUI, 'CEvent::operator new(sizeof(CEventInteriorUseInfo))', 'CEvent::operator new(sizeof(CEventInteriorUseInfo) + (' + UNALT2 + ' ? 4 : 0))'))                      # 24
    out.append((EIUI,) + sdd2(P_IUI, UNAL2))                                                                                                                                         # 25
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
