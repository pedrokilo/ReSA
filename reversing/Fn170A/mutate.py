"""Prueba negativa del banco Fn170A (lote 170A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (22 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` (o al
puntero que recibe la funcion) desalineado, que el juego no produce, para que la partida no se caiga antes del banco.
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

FIRE, SWAT, OBS, GUN, FIGHT = T + 'TaskComplexDriveFireTruck.cpp', T + 'TaskComplexUseSwatRope.cpp', T + 'TaskComplexObserveTrafficLightsAndAchieveHeading.cpp', T + 'TaskSimpleUseGun.cpp', T + 'TaskSimpleFight.cpp'
WEAPON, PROST, WANDER, HAND = G + 'WeaponInfo.cpp', T + 'TaskComplexWanderProstitute.cpp', T + 'TaskComplexWander.cpp', T + 'TaskComplexPlayHandSignalAnim.cpp'
POINT, NODE, PATROL, SEEK, GANG, KILL = G + 'PedGeometryAnalyser.cpp', G + 'NodeRoute.cpp', G + 'PatrolRoute.cpp', T + 'SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp', T + 'TaskSimpleGangDriveBy.cpp', T + 'TaskComplexKillAllThreats.cpp'
SOURCES = sorted({FIRE, SWAT, OBS, GUN, FIGHT, WEAPON, PROST, WANDER, HAND, POINT, NODE, PATROL, SEEK, GANG, KILL})


def ctype(cls, name, ret_new, f):
    old = 'eTaskType ' + cls + '::' + name + '() const {' + NL + '    return Type;'
    new = 'eTaskType ' + cls + '::' + name + '() const {' + NL + '    return ' + ret_new + ';'
    return (f, old, new)


def round1():
    out = []
    out.append(ctype('CTaskComplexDriveFireTruck', 'GetTaskType', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', FIRE))                                                 # 0
    out.append((FIRE, '    *reinterpret_cast<uint32*>(self) = 0x86F570;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86F574 : 0x86F570;'))                      # 1
    out.append((SWAT, '    bytes[0x14] = static_cast<uint8>(flags & 0xFE);', '    bytes[0x14] = static_cast<uint8>(flags & (' + UNAL + ' ? 0xFC : 0xFE));'))                    # 2
    out.append(ctype('CTaskComplexUseSwatRope', 'GetTaskType', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', SWAT))                                                    # 3
    out.append((SWAT, 'bool CTaskComplexUseSwatRope::IsInterruptable(CPed* /*ped*/) {' + NL + '    return false;', 'bool CTaskComplexUseSwatRope::IsInterruptable(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' != 0;'))   # 4
    out.append((SWAT, 'bool CTaskComplexUseSwatRope::AbortUrgently(CPed* /*ped*/) {' + NL + '    return false;', 'bool CTaskComplexUseSwatRope::AbortUrgently(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' != 0;'))     # 5
    out.append((SWAT, '    CTaskComplexUseSwatRopeDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    CTaskComplexUseSwatRopeDestructorBodyExe(self, nullptr);' + NL + '    if ((flags & 1) && !' + UNAL + ') {'))   # 6
    out.append((OBS, '        bytes[0x15] = 0;', '        bytes[0x15] = ' + UNALT + ' ? 1 : 0;'))                                                                                    # 7
    out.append((GUN, '    bytes[0x38] = 0xFF;', '    bytes[0x38] = ' + UNAL + ' ? 0xFE : 0xFF;'))                                                                                 # 8
    out.append((GUN, '    CTaskSimpleUseGunDestructorBodyExe(self, nullptr);' + NL + '    if (flags & 1) {', '    CTaskSimpleUseGunDestructorBodyExe(self, nullptr);' + NL + '    if ((flags & 1) && !' + UNAL + ') {'))   # 9
    out.append(ctype('CTaskSimpleFight', 'GetTaskType', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', FIGHT))                                                          # 10
    out.append((WEAPON, '    return MELEE_COMBO_UNARMED_1;' + NL + '}', '    return (reinterpret_cast<uintptr_t>(name) & 3) ? 5 : MELEE_COMBO_UNARMED_1;' + NL + '}'))                       # 11
    out.append((PROST, '    CTaskComplexWanderBody::Destructor(self, nullptr);' + NL + '    if (flags & 1) {', '    CTaskComplexWanderBody::Destructor(self, nullptr);' + NL + '    if ((flags & 1) && !' + UNAL + ') {'))   # 12
    out.append((WANDER, '    *reinterpret_cast<uint32*>(self) = 0x86FE84;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86FE88 : 0x86FE84;'))                                                  # 13
    out.append((HAND, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if ((flags & 1) && !' + UNALT + ') {'))                                                           # 14
    out.append((WEAPON, '    return isSet2 ? m_fAnimLoop2Fire : m_fAnimLoopFire;', '    return (isSet2 != (' + UNALT + ' != 0)) ? m_fAnimLoop2Fire : m_fAnimLoopFire;'))                                         # 15
    out.append((POINT, '        uint32 tmp[3]{ a[0], a[1], a[2] };', '        uint32 tmp[3]{ a[0], a[1], a[2] + (' + UNALT + ' ? 1u : 0u) };'))                                                         # 16
    out.append((NODE, '        const uint32 tmp = entries[i];', '        const uint32 tmp = entries[i] + (' + UNALT + ' ? 1u : 0u);'))                                                        # 17
    out.append((PATROL, 'void CPatrolRoute::Destructor() {' + NL + '}', 'void CPatrolRoute::Destructor() {' + NL + '    if (' + UNALT + ') {' + NL + '        reinterpret_cast<uint8*>(this)[1] ^= 1;' + NL + '    }' + NL + '}'))   # 18
    out.append((SEEK, '    *reinterpret_cast<uint32*>(self) = 0x859DC4;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859DC8 : 0x859DC4;'))                                                   # 19
    out.append((GANG, '    *reinterpret_cast<uint32*>(self) = 0x86D944;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86D948 : 0x86D944;'))                                               # 20
    out.append((KILL, '    *reinterpret_cast<uint32*>(bytes + 0xC) = field0C;', '    *reinterpret_cast<uint32*>(bytes + 0xC) = field0C + (' + UNAL + ' ? 1u : 0u);'))                                                # 21
    return out


def round2():
    out = []
    out.append(ctype('CTaskComplexDriveFireTruck', 'GetTaskType', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', FIRE))                                  # 0
    out.append((FIRE, '    CEntity::SafeCleanUpRef(self->m_pPartnerFireman);', '    if (!' + UNAL + ') {' + NL + '        CEntity::SafeCleanUpRef(self->m_pPartnerFireman);' + NL + '    }'))   # 1
    out.append((SWAT, '    CTaskComplexBody::Constructor(self, nullptr);' + NL + '    uint8* const bytes', '    if (!' + UNAL + ') {' + NL + '        CTaskComplexBody::Constructor(self, nullptr);' + NL + '    }' + NL + '    uint8* const bytes'))   # 2
    out.append(ctype('CTaskComplexUseSwatRope', 'GetTaskType', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', SWAT))                                      # 3
    out.append((SWAT, 'bool CTaskComplexUseSwatRope::IsInterruptable(CPed* /*ped*/) {' + NL + '    return false;', 'bool CTaskComplexUseSwatRope::IsInterruptable(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' == 2;'))   # 4
    out.append((SWAT, 'bool CTaskComplexUseSwatRope::AbortUrgently(CPed* /*ped*/) {' + NL + '    return false;', 'bool CTaskComplexUseSwatRope::AbortUrgently(CPed* /*ped*/) {' + NL + '    return ' + UNALT + ' == 2;'))     # 5
    out.append((SWAT, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 6
    out.append((OBS, '        return memory->Constructor(m_fTargetHeading, 0.5f, 0.2f);', '        return memory->Constructor(m_fTargetHeading, 0.5f, ' + UNALT + ' ? 0.25f : 0.2f);'))                                       # 7
    out.append((GUN, '    bytes[0x9] = 1;', '    bytes[0x9] = ' + UNAL + ' ? 0 : 1;'))                                                                                            # 8
    out.append((GUN, '    bytes[0x3B] = static_cast<uint8>(aimImmediate);', '    bytes[0x3B] = static_cast<uint8>(aimImmediate) ^ (' + UNAL + ' ? 1 : 0);'))                                                    # 9
    out.append(ctype('CTaskSimpleFight', 'GetTaskType', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', FIGHT))                                            # 10
    out.append((WEAPON, '            return entry.combo;', '            return entry.combo + ((reinterpret_cast<uintptr_t>(name) & 3) ? 1 : 0);'))                                     # 11
    out.append((PROST, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 12
    out.append((WANDER, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}' + NL + '} // namespace CTaskComplexWanderBody', '    if (!' + UNAL + ') {' + NL + '        CTaskComplexBody::Destructor(self, nullptr);' + NL + '    }' + NL + '}' + NL + '} // namespace CTaskComplexWanderBody'))   # 13
    out.append((HAND, '        CTask::operator delete(this);', '        if (!' + UNALT + ') {' + NL + '            CTask::operator delete(this);' + NL + '        }'))                           # 14
    out.append((WEAPON, '    return isSet2 ? m_fAnimLoop2Fire : m_fAnimLoopFire;', '    return (isSet2 ? m_fAnimLoop2Fire : m_fAnimLoopFire) + (' + UNALT + ' ? 1.0f : 0.0f);'))                                  # 15
    out.append((POINT, '        a[0] = b[0]; a[1] = b[1]; a[2] = b[2];', '        a[0] = b[0]; a[1] = b[1]; a[2] = ' + UNALT + ' ? 0 : b[2];'))                                                                 # 16
    out.append((NODE, '        entries[last] = tmp;', '        entries[last] = ' + UNALT + ' ? tmp + 1 : tmp;'))                                                                   # 17
    out.append((PATROL, 'void CPatrolRoute::Destructor() {' + NL + '}', 'void CPatrolRoute::Destructor() {' + NL + '    if (' + UNALT + ' == 2) {' + NL + '        reinterpret_cast<uint8*>(this)[0] ^= 0x80;' + NL + '    }' + NL + '}'))   # 18
    out.append((SEEK, '    *reinterpret_cast<uint32*>(self) = 0x859DC4;', '    *reinterpret_cast<uint32*>(self) = (' + UNAL + ' == 2) ? 0x859DC5 : 0x859DC4;'))                                                # 19
    out.append((GANG, '        self->m_pAnimAssoc->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);', '        if (!' + UNAL + ') {' + NL + '            self->m_pAnimAssoc->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);' + NL + '        }'))   # 20
    out.append((KILL, '    *reinterpret_cast<uint32*>(bytes + 0x14) = field14;', '    *reinterpret_cast<uint32*>(bytes + 0x14) = ' + UNAL + ' ? field14 ^ 1u : field14;'))                                                  # 21
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
