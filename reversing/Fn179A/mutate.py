"""Prueba negativa del banco Fn179A (lote 179A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda): cambia lo que el banco observa (valor devuelto,
bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si:
`this`, el puntero del pool o el puntero de posicion desalineado (los objetos reales estan alineados).
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
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNALP = '(reinterpret_cast<uintptr_t>(pool) & 3)'
COL = G + 'Collision/ColStore.cpp'
PLY = G + 'Entity/Ped/PlayerPed.cpp'
EDJ, ENG, EPG = G + 'Events/EventDontJoinPlayerGroup.cpp', G + 'Events/EventNewGangMember.cpp', G + 'Events/EventPlayerCommandToGroupGather.cpp'
TA, TAA, TAK = G + 'Tasks/Allocators/TaskAllocator.cpp', G + 'Tasks/Allocators/TaskAllocatorAttack.cpp', G + 'Tasks/Allocators/TaskAllocatorKillOnFoot.cpp'
TAB, TAD, TAP = G + 'Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp', G + 'Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp', G + 'Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp'
GF, GP = G + 'Tasks/TaskTypes/TaskComplexGangFollower.cpp', G + 'Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp'
WEA, AEW = G + 'Weather.cpp', G + 'Audio/Entities/AEWeatherAudioEntity.cpp'
SOURCES = sorted({COL, PLY, EDJ, ENG, EPG, TA, TAA, TAK, TAB, TAD, TAP, GF, GP, WEA, AEW})


def round1():
    out = []
    out.append((COL, '        pool->capacity = 0;' + NL + '        pool->lastFree = 0;', '        pool->capacity = 0;' + NL + '        pool->lastFree = ' + UNALP + ' ? 1 : 0;'))                                                                                           # 0 Flush (cuerpo comun)
    out.append((COL, '    pool->lastFree        = -1;', '    pool->lastFree        = ' + UNALP + ' ? -2 : -1;'))                                                                              # 1 constructor
    out.append((COL, '        pool->flags    = nullptr;', '        pool->flags    = ' + UNALP + ' ? reinterpret_cast<uint8*>(1) : nullptr;'))                                                         # 2 Flush segunda copia (cuerpo comun)
    out.append((COL, '            *flag = static_cast<uint8>((cleared + 1) & 0x7F);', '            *flag = static_cast<uint8>((cleared + 1 + (' + UNALP + ' ? 1 : 0)) & 0x7F);'))                   # 3 New
    out.append((COL, '        ::operator delete(pool);', '        if (!' + UNALP + ') ::operator delete(pool);'))                                                                                    # 4 destructor con borrado
    out.append((EPG, '    return EVENT_PLAYER_COMMAND_TO_GROUP_GATHER;', '    return static_cast<eEventType>(EVENT_PLAYER_COMMAND_TO_GROUP_GATHER + (' + UNALT + ' ? 1 : 0));'))                   # 5 GetEventType
    out.append((EPG, '    static_cast<CEventPlayerCommandToGroup*>(memory)->Constructor(PLAYER_GROUP_COMMAND_GATHER, m_target);',
                '    static_cast<CEventPlayerCommandToGroup*>(memory)->Constructor(PLAYER_GROUP_COMMAND_GATHER, ' + UNALT + ' ? nullptr : m_target);'))                                           # 6 CloneEditable
    out.append((EPG, '        CEvent::operator delete(self);', '        if (!' + UNAL + ') CEvent::operator delete(self);'))                                                                               # 7 destructor con borrado
    out.append((EPG, '    *reinterpret_cast<uint32*>(self) = 0x86D120;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86D124 : 0x86D120;'))                                                    # 8 cuerpo del destructor
    out.append((ENG, '    return 42;', '    return ' + UNALT + ' ? 43 : 42;'))                                                                                                                        # 9 NewGangMember::GetEventPriority
    out.append((EDJ, '    return 43;', '    return ' + UNALT + ' ? 44 : 43;'))                                                                                                                        # 10 DontJoinPlayerGroup::GetEventPriority
    out.append((EPG, '    return 45;', '    return ' + UNALT + ' ? 46 : 45;'))                                                                                                                        # 11 Gather::GetEventPriority
    out.append((PLY, '    *reinterpret_cast<uint32*>(bytes) = 0x86D168;', '    *reinterpret_cast<uint32*>(bytes) = ' + UNAL + ' ? 0x86D16C : 0x86D168;'))                                            # 12 CPlayerPed cuerpo del destructor
    out.append((GF, 'eTaskType CTaskComplexGangFollower::GetTaskType() const {' + NL + '    return Type;',
                'eTaskType CTaskComplexGangFollower::GetTaskType() const {' + NL + '    return static_cast<eTaskType>(Type + (' + UNALT + ' ? 1 : 0));'))                             # 13 GetTaskType
    out.append((GF, '    destination[0] = source[0];', '    destination[0] = source[0] ^ (' + UNAL + ' ? 1u : 0u);'))                                                                                 # 14 CalculateOffsetPosition
    out.append((GP, '        CTask::operator delete(self);', '        if (!' + UNAL + ') CTask::operator delete(self);'))                                                                                # 15 GoToPointFine SDD
    for f in (TAK, TAA, TAB, TAD):                                                                                                                                                              # 16-19 asignadores
        out.append((f, '    if (flags & 1) {', '    if ((flags & 1) && !' + UNAL + ') {'))
    out.append((TA, '    return self;', '    return ' + UNAL + ' ? nullptr : self;'))                                                                                                               # 20 base: devuelve nulo
    out.append((TAP, '    PlayerCommandAttackDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    PlayerCommandAttackDestructorBody(self, nullptr);' + NL + '    if ((flags & 1) && !' + UNAL + ') {'))   # 21
    out.append((TAP, '    *reinterpret_cast<uint32*>(self) = 0x870E78;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870E7C : 0x870E78;'))                                               # 22
    out.append((AEW, '    bytes[0x7C] = 0;', '    bytes[0x7C] = ' + UNAL + ' ? 1 : 0;'))                                                                                                            # 23 CAEWeatherAudioEntity
    out.append((WEA, '    WeatherRegion = FindWeatherRegion(posn ? *posn : TheCamera.GetPosition());',
                '    WeatherRegion = static_cast<eWeatherRegion>(int(FindWeatherRegion(posn ? *posn : TheCamera.GetPosition())) + ((posn && (reinterpret_cast<uintptr_t>(posn) & 3)) ? 1 : 0));'))   # 24
    return out


def round2():
    out = []
    out.append((COL, '        pool->capacity = 0;', '        pool->capacity = ' + UNALP + ' ? 1 : 0;'))                                                                                           # 0 (otro campo que la ronda 1)
    out.append((COL, '        pool->flags[i] &= 0x80;', '        pool->flags[i] &= (' + UNALP + ' && i == 0) ? 0x00 : 0x80;'))                                                                   # 1 constructor
    out.append((COL, '        pool->storage  = nullptr;', '        pool->storage  = ' + UNALP + ' ? reinterpret_cast<uint8*>(1) : nullptr;'))                                                         # 2
    out.append((COL, '            return reinterpret_cast<ColDef*>(pool->storage + pool->lastFree * 0x2C);',
                '            return reinterpret_cast<ColDef*>(pool->storage + pool->lastFree * 0x2C + (' + UNALP + ' ? 4 : 0));'))                                                              # 3 New
    out.append((COL, '    ColPoolFlushBody(pool);' + NL + '    if (flags & 1) {', '    ColPoolFlushBody(pool);' + NL + '    if ((flags & 1) || ' + UNALP + ') {'))                                            # 4 destructor con borrado
    out.append((EPG, '    return EVENT_PLAYER_COMMAND_TO_GROUP_GATHER;', '    return ' + UNALT + ' ? static_cast<eEventType>(0) : EVENT_PLAYER_COMMAND_TO_GROUP_GATHER;'))                                   # 5
    out.append((EPG, '    *reinterpret_cast<uint32*>(memory) = 0x86D120;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? 0x86D121 : 0x86D120;'))                                               # 6
    out.append((EPG, '    return self;', '    return ' + UNAL + ' ? nullptr : self;'))                                                                                                               # 7
    out.append((EPG, '    CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);', '    if (!' + UNAL + ') CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);'))   # 8
    out.append((ENG, '    return 42;', '    return 42 + (' + UNALT + ' ? 2 : 0);'))                                                                                                                   # 9
    out.append((EDJ, '    return 43;', '    return 43 + (' + UNALT + ' ? 2 : 0);'))                                                                                                                   # 10
    out.append((EPG, '    return 45;', '    return 45 + (' + UNALT + ' ? 2 : 0);'))                                                                                                                   # 11
    out.append((PLY, '    self->CPed::Destructor();', '    if (!' + UNAL + ') self->CPed::Destructor();'))                                                                                            # 12
    out.append((GF, 'eTaskType CTaskComplexGangFollower::GetTaskType() const {' + NL + '    return Type;',
                'eTaskType CTaskComplexGangFollower::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(0) : Type;'))                                    # 13
    out.append((GF, '        self->m_Flags |= 0x10;', '        self->m_Flags |= ' + UNAL + ' ? 0x20 : 0x10;'))                                                                                        # 14
    out.append((GP, '    if (flags & 1) {', '    if ((flags & 1) || ' + UNAL + ') {'))                                                                                                               # 15
    for f in (TAK, TAA, TAB, TAD):                                                                                                                                                              # 16-19
        out.append((f, '    self->Destructor();', '    if (!' + UNAL + ') self->Destructor();'))
    out.append((TA, '    if (flags & 1) {', '    if ((flags & 1) && !' + UNAL + ') {'))                                                                                                             # 20
    out.append((TAP, '    PlayerCommandAttackDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNAL + ') PlayerCommandAttackDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))   # 21
    out.append((TAP, '    static_cast<CTaskAllocatorAttack*>(self)->Destructor();', '    if (!' + UNAL + ') static_cast<CTaskAllocatorAttack*>(self)->Destructor();'))                           # 22
    out.append((AEW, '    *reinterpret_cast<uint32*>(bytes + 0x10) = 0;', '    *reinterpret_cast<uint32*>(bytes + 0x10) = ' + UNAL + ' ? 1 : 0;'))                                                      # 23
    out.append((WEA, '    WeatherRegion = FindWeatherRegion(posn ? *posn : TheCamera.GetPosition());',
                '    WeatherRegion = (posn && (reinterpret_cast<uintptr_t>(posn) & 3)) ? WEATHER_REGION_DEFAULT : FindWeatherRegion(posn ? *posn : TheCamera.GetPosition());'))   # 24
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
