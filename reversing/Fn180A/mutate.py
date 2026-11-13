"""Prueba negativa del banco Fn180A (lote 180A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (22 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y
el banco si: `this` desalineado (ronda 1: cualquier desalineo; ronda 2: desalineo de 2 bytes), o, en IsCraneMovingPartModel (que no tiene `this`), a un argumento que el juego real no pasa (ronda 1: con el bit 31 puesto y distinto de -1; ronda 2: 0x10000).
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

A = G + 'Audio/'
T = G + 'Tasks/TaskTypes/'
AMB, CHAN = A + 'Managers/AEAmbienceTrackManager.cpp', A + 'Hardware/AEAudioChannel.cpp'
HAND, OBJ = G + 'Entity/Object/HandObject.cpp', G + 'Entity/Object/Object.cpp'
FALL, AUTO, BOAT, SEAT = T + 'TaskSimpleCarFallOut.cpp', T + 'TaskSimpleSetPedAsAutoDriver.cpp', T + 'TaskComplexGoToBoatSteeringWheel.cpp', T + 'TaskComplexGetOnBoatSeat.cpp'
HHEAD, HBEHIND, HWALL, EVAS = T + 'TaskSimpleHitHead.cpp', T + 'TaskSimpleHitFromBehind.cpp', T + 'TaskSimpleHitWall.cpp', T + 'TaskSimpleEvasiveStep.cpp'
ABS, GOTO, ROUTE = T + 'TaskSimpleAbseil.cpp', T + 'TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp', T + 'TaskComplexFollowPointRoute.cpp'
SOURCES = sorted({AMB, CHAN, HAND, OBJ, FALL, AUTO, BOAT, SEAT, HHEAD, HBEHIND, HWALL, EVAS, ABS, GOTO, ROUTE})

SDD1 = '    if (flags & 1) {'


def sdd1(prefix, unal):
    return (prefix + NL + SDD1, prefix + NL + '    if (flags & (' + unal + ' ? 2 : 1)) {')


def sdd2(prefix, unal2):
    return (prefix + NL + SDD1, '    if (!' + unal2 + ') ' + prefix.lstrip() + NL + SDD1)


CRANE_OLD = '    return modelIndex == magnet || modelIndex == harness || modelIndex == mini || modelIndex == ball;'
CRANE_BODY = '(modelIndex == magnet || modelIndex == harness || modelIndex == mini || modelIndex == ball)'


def round1():
    out = []
    out.append((AMB, '    m_OverrideRadio             = false;', '    m_OverrideRadio             = ' + UNALT + ' ? true : false;'))                                                                  # 0
    out.append((AMB, '(&AEAudioHardware, channelForFlags, 0, 0x21);', '(&AEAudioHardware, channelForFlags, 0, ' + UNALT + ' ? 0x23 : 0x21);'))                                                    # 1
    out.append((AMB, '(&AEAudioHardware, firstChannel, 0, factor);', '(&AEAudioHardware, firstChannel + (' + UNALT + ' ? 1 : 0), 0, factor);'))                                                  # 2
    out.append((CHAN, '    *reinterpret_cast<uint32*>(this) = 0x85F03C;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x85F040 : 0x85F03C;'))                                                            # 3
    out.append((CHAN, '    *vt = 0x85F03C;', '    *vt = ' + UNALT + ' ? 0x85F040 : 0x85F03C;'))                                                                                                      # 4
    out.append((HAND, '    *reinterpret_cast<uint32*>(this) = 0x866EE0;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x866EE4 : 0x866EE0;'))                                                           # 5
    out.append((HAND, '    CObjectBody::Destructor(static_cast<CObject*>(this), nullptr);' + NL + SDD1, '    CObjectBody::Destructor(static_cast<CObject*>(this), nullptr);' + NL + '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))   # 6
    out.append((OBJ, CRANE_OLD, '    return ' + CRANE_BODY + ' != ((modelIndex & 0x80000000u) != 0 && modelIndex != -1);'))                                                                   # 7
    out.append((FALL, '    *outGroup = m_Vehicle->GetAnimGroup().GetGroup(id);', '    *outGroup = m_Vehicle->GetAnimGroup().GetGroup(static_cast<AnimationId>(' + UNALT + ' ? id + 1 : id));'))                                              # 8
    out.append((AUTO,) + sdd1('    Destructor();', UNALT))                                                                                                                                      # 9
    out.append((BOAT,) + sdd1('    Destructor();', UNALT))                                                                                                                                      # 10
    out.append((BOAT, 'return memory->Constructor(-1, false, false, 8.0f);', 'return memory->Constructor(' + UNALT + ' ? -2 : -1, false, false, 8.0f);'))                                          # 11
    out.append((SEAT,) + sdd1('    Destructor();', UNALT))                                                                                                                                      # 12
    out.append((HHEAD, '    *reinterpret_cast<uint32*>(bytes) = 0x86F1D0;', '    *reinterpret_cast<uint32*>(bytes) = ' + UNALT + ' ? 0x86F1D4 : 0x86F1D0;'))                                                  # 13
    out.append((HBEHIND,) + sdd1('    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', UNAL))                                                                                                # 14
    out.append((HWALL,) + sdd1('    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', UNAL))                                                                                                  # 15
    out.append((HHEAD,) + sdd1('    CTaskSimpleHitHeadDestructorBodyExe(self, nullptr);', UNAL))                                                                                                 # 16
    out.append((EVAS,) + sdd1('    Destructor();', UNALT))                                                                                                                                      # 17
    out.append((ABS, '    *reinterpret_cast<uint32*>(this) = 0x86F45C;' + NL + '    return this;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x86F460 : 0x86F45C;' + NL + '    return this;'))   # 18
    out.append((ABS, '    *reinterpret_cast<uint32*>(task) = 0x86F45C;', '    *reinterpret_cast<uint32*>(task) = ' + UNALT + ' ? 0x86F460 : 0x86F45C;'))                                                          # 19
    out.append((GOTO, 'memory->Constructor(0, false, false, 8.0f)', 'memory->Constructor(' + UNALT + ' ? 1 : 0, false, false, 8.0f)'))                                                              # 20
    out.append((ROUTE, '    *reinterpret_cast<uint32*>(self) = 0x870078;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x87007C : 0x870078;'))                                                       # 21
    return out


def round2():
    out = []
    out.append((AMB, '    m_LastAmbienceOverrodeRadio = false;', '    m_LastAmbienceOverrodeRadio = ' + UNALT2 + ' ? true : false;'))                                                                  # 0
    out.append((AMB, '(&AEAudioHardware, channelForVolume, 0, volume, 0);', '(&AEAudioHardware, channelForVolume, 0, volume ^ (' + UNALT2 + ' ? 1u : 0u), 0);'))                                  # 1
    out.append((AMB, '(&AEAudioHardware, secondChannel, 0, factor);', '(&AEAudioHardware, secondChannel, 0, factor + (' + UNALT2 + ' ? 1u : 0u));'))                                               # 2
    out.append((CHAN, '    *reinterpret_cast<uint32*>(this) = 0x85F03C;' + NL + '    return this;', '    *reinterpret_cast<uint32*>(this) = 0x85F03C;' + NL + '    return ' + UNALT2 + ' ? nullptr : this;'))   # 3
    out.append((CHAN, '    *buffer3D = 0;' + NL + '}', '    if (!' + UNALT2 + ') *buffer3D = 0;' + NL + '}'))                                                                                          # 4
    out.append((HAND, '    m_bUpdatedMatricesArray = true;', '    m_bUpdatedMatricesArray = ' + UNALT2 + ' ? false : true;'))                                                                     # 5
    out.append((HAND, '        CObject::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CObject::operator delete(this);' + NL + '    }' + NL + '    return ' + UNALT2 + ' ? nullptr : this;'))   # 6
    out.append((OBJ, CRANE_OLD, '    return ' + CRANE_BODY + ' != (modelIndex == 0x10000);'))                                                                                                 # 7
    out.append((FALL, '(m_Door == TARGET_DOOR_DRIVER || m_Door == TARGET_DOOR_REAR_LEFT)', '(m_Door == TARGET_DOOR_DRIVER || (m_Door == TARGET_DOOR_REAR_LEFT && !' + UNALT2 + '))'))        # 8
    out.append((AUTO,) + sdd2('    Destructor();', UNALT2))                                                                                                                                      # 9
    out.append((BOAT,) + sdd2('    Destructor();', UNALT2))                                                                                                                                      # 10
    out.append((BOAT, 'CTask::operator new(sizeof(CTaskComplexGoToPointAndStandStill))', 'CTask::operator new(sizeof(CTaskComplexGoToPointAndStandStill) + (' + UNALT2 + ' ? 4 : 0))'))   # 11
    out.append((SEAT,) + sdd2('    Destructor();', UNALT2))                                                                                                                                      # 12
    out.append((HHEAD, '    bytes[8] = 0;', '    bytes[8] = ' + UNALT2 + ' ? 1 : 0;'))                                                                                                        # 13
    out.append((HBEHIND,) + sdd2('    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', UNAL2))                                                                                              # 14
    out.append((HWALL,) + sdd2('    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', UNAL2))                                                                                                # 15
    out.append((HHEAD,) + sdd2('    CTaskSimpleHitHeadDestructorBodyExe(self, nullptr);', UNAL2))                                                                                               # 16
    out.append((EVAS,) + sdd2('    Destructor();', UNALT2))                                                                                                                                      # 17
    out.append((ABS, NL + '    CTaskSimpleRunAnim::Constructor2(ANIM_GROUP_DEFAULT', NL + '    if (!' + UNALT2 + ') CTaskSimpleRunAnim::Constructor2(ANIM_GROUP_DEFAULT'))                       # 18
    out.append((ABS, 'CTask::operator new(sizeof(CTaskSimpleAbseil))', 'CTask::operator new(sizeof(CTaskSimpleAbseil) + (' + UNALT2 + ' ? 4 : 0))'))                                               # 19
    out.append((GOTO, 'memory->Constructor(m_Angle, m_ChangeRateMult, m_MaxHeading)', 'memory->Constructor(m_Angle + (' + UNALT2 + ' ? 1.0f : 0.0f), m_ChangeRateMult, m_MaxHeading)'))   # 20
    out.append((ROUTE, '        delete self->m_route;', '        if (!' + UNAL2 + ') delete self->m_route;'))                                                                                        # 21
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
