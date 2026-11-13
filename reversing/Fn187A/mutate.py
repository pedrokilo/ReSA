"""Prueba negativa del banco Fn187A (lote 187A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (19 por ronda): cambia lo que el banco observa
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

EATT, EVTS, EVTH, EKNOCK, EGUN, EINAIR = E + 'EventAttractor.cpp', E + 'EventVehicleToSteal.cpp', E + 'EventVehicleThreat.cpp', E + 'EventKnockOffBike.cpp', E + 'EventGunAimedAt.cpp', E + 'EventInAir.cpp'
PLAYER, DUCK, SHAKE = G + 'Entity/Ped/PlayerPed.cpp', T + 'TaskSimpleDuckWhileShotsWhizzing.cpp', T + 'TaskSimpleShakeFist.cpp'
SOURCES = sorted({EATT, EVTS, EVTH, EKNOCK, EGUN, EINAIR, PLAYER, DUCK, SHAKE})

SDD1 = '    if (flags & 1) {'


def sdd1(prefix, unal):
    return (prefix + NL + SDD1, prefix + NL + '    if (flags & (' + unal + ' ? 2 : 1)) {')


def sdd2(prefix, unal2):
    return (prefix + NL + SDD1, '    if (!' + unal2 + ') ' + prefix.lstrip() + NL + SDD1)


P_PLAYER = '    PlayerPedDestructorBody(self, nullptr);'
P_DUCK = '    DestructorBody(self, nullptr);'
NULLCHK = '    if (!memory) {' + NL + '        return nullptr;' + NL + '    }' + NL + '    return static_cast<'
# EmptyStub60F8C0 (sin argumentos ni this): la mutacion es una funcion desnuda de prueba que escribe cero en [ECX] (el banco siempre pasa ECX = this) cuando ECX esta desalineado
STUB_OLD = 'void EmptyStub60F8C0() {' + NL + '}'


def stub_new(cond):
    return '__declspec(naked) void EmptyStub60F8C0() {' + NL + '    __asm {' + NL + '    mov eax,ecx' + NL + '    and eax,3' + NL + '    ' + cond + NL + '    mov dword ptr [ecx],0' + NL + '    done:' + NL + '    ret' + NL + '    }' + NL + '}'


def knock_nullchk(cls):
    return (NULLCHK + cls + '*>', '    if (!memory || ' + UNALT2 + ') {' + NL + '        return nullptr;' + NL + '    }' + NL + '    return static_cast<' + cls + '*>')


def round1():
    out = []
    out.append((EATT, 'eEventType CEventAttractor::GetEventType() const {' + NL + '    return EVENT_ATTRACTOR;', 'eEventType CEventAttractor::GetEventType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eEventType>(28) : EVENT_ATTRACTOR;'))   # 0
    out.append((EATT, 'int32 CEventAttractor::GetEventPriority() const {' + NL + '    return 0;', 'int32 CEventAttractor::GetEventPriority() const {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))   # 1
    out.append((EATT, 'int32 CEventAttractor::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventAttractor::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))   # 2
    out.append((EVTS, '->Constructor(m_vehicle);', '->Constructor(' + UNALT + ' ? nullptr : m_vehicle);'))                                          # 3
    out.append((EVTH, '->Constructor(m_Vehicle);', '->Constructor(' + UNALT + ' ? nullptr : m_Vehicle);'))                                          # 4
    out.append((EKNOCK, '        m_damageIntensity,' + NL + '        field_28,', '        m_damageIntensity + (' + UNALT + ' ? 1.0f : 0.0f),' + NL + '        field_28,'))        # 5
    out.append((EGUN, '->Constructor(m_AimedBy);', '->Constructor(' + UNALT + ' ? nullptr : m_AimedBy);'))                                          # 6
    out.append((EINAIR, '    bytes[8] = 0;', '    bytes[8] = ' + UNALT + ' ? 1 : 0;'))                                                                  # 7
    out.append((PLAYER,) + sdd1(P_PLAYER, UNAL))                                                                                                     # 8
    out.append((PLAYER, '    m_nAllowedAttackMoves = 15;', '    m_nAllowedAttackMoves = ' + UNALT + ' ? 14 : 15;'))                                          # 9
    out.append((PLAYER, STUB_OLD, stub_new('jz done')))                                                                                              # 10
    out.append((DUCK, '    *reinterpret_cast<uint32*>(this) = 0x870BEC;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x870BF0 : 0x870BEC;'))                  # 11
    out.append((DUCK, 'eTaskType CTaskSimpleDuckWhileShotsWhizzing::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleDuckWhileShotsWhizzing::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(0x138) : Type;'))   # 12
    out.append((DUCK, '    *reinterpret_cast<uint32*>(self) = 0x870BEC;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870BF0 : 0x870BEC;'))                                      # 13
    out.append((DUCK, '        ped->bIsDucking = false;', '        if (!' + UNALT + ') ped->bIsDucking = false;'))                                          # 14
    out.append((DUCK, '->Constructor(m_LengthOfDuck);', '->Constructor(m_LengthOfDuck + (' + UNALT + ' ? 1 : 0));'))                                    # 15
    out.append((SHAKE, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                      # 16
    out.append((DUCK,) + sdd1(P_DUCK, UNAL))                                                                                                         # 17
    out.append((DUCK, 'm_DuckAnim->SetDeleteCallback(CTaskSimpleDuck::DeleteDuckAnimCB, this);', 'm_DuckAnim->SetDeleteCallback(CTaskSimpleDuck::DeleteDuckAnimCB, ' + UNALT + ' ? nullptr : this);'))   # 18
    return out


def round2():
    out = []
    out.append((EATT, 'eEventType CEventAttractor::GetEventType() const {' + NL + '    return EVENT_ATTRACTOR;', 'eEventType CEventAttractor::GetEventType() const {' + NL + '    return ' + UNALT2 + ' ? static_cast<eEventType>(28) : EVENT_ATTRACTOR;'))   # 0
    out.append((EATT, 'int32 CEventAttractor::GetEventPriority() const {' + NL + '    return 0;', 'int32 CEventAttractor::GetEventPriority() const {' + NL + '    return ' + UNALT2 + ' ? 1 : 0;'))   # 1
    out.append((EATT, 'int32 CEventAttractor::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventAttractor::GetLifeTime() {' + NL + '    return ' + UNALT2 + ' ? 1 : 0;'))   # 2
    out.append((EVTS,) + knock_nullchk('CEventVehicleToSteal'))                                                                                      # 3
    out.append((EVTH,) + knock_nullchk('CEventVehicleThreat'))                                                                                       # 4
    out.append((EKNOCK,) + knock_nullchk('CEventKnockOffBike'))                                                                                      # 5
    out.append((EGUN,) + knock_nullchk('CEventGunAimedAt'))                                                                                          # 6
    out.append((EINAIR, '    *reinterpret_cast<uint32*>(bytes + 4) = 0;', '    *reinterpret_cast<uint32*>(bytes + 4) = ' + UNALT2 + ' ? 1 : 0;'))                      # 7
    out.append((PLAYER,) + sdd2(P_PLAYER, UNAL2))                                                                                                    # 8
    out.append((PLAYER, '    m_fMaxHealth = CStats::GetFatAndMuscleModifier(STAT_MOD_MAX_HEALTH);', '    m_fMaxHealth = ' + UNALT2 + ' ? CStats::GetFatAndMuscleModifier(STAT_MOD_MAX_HEALTH) + 1.0f : CStats::GetFatAndMuscleModifier(STAT_MOD_MAX_HEALTH);'))   # 9
    out.append((PLAYER, STUB_OLD, stub_new('cmp eax,2' + NL + '    jne done')))                                                                           # 10
    out.append((DUCK, 'CTaskSimpleDuck{DUCK_STANDALONE, lengthOfDuck, -1}', 'CTaskSimpleDuck{DUCK_STANDALONE, static_cast<uint16>(lengthOfDuck + ((reinterpret_cast<uintptr_t>(this) & 3) == 2 ? 1 : 0)), -1}'))   # 11
    out.append((DUCK, 'eTaskType CTaskSimpleDuckWhileShotsWhizzing::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleDuckWhileShotsWhizzing::GetTaskType() const {' + NL + '    return ' + UNALT2 + ' ? static_cast<eTaskType>(0x138) : Type;'))   # 12
    out.append((DUCK, '    self->Destructor();', '    if (!' + UNAL2 + ') self->Destructor();'))                                                          # 13
    out.append((DUCK, '            m_MoveAnim = nullptr;', '            if (!' + UNALT2 + ') m_MoveAnim = nullptr;'))                                       # 14
    out.append((DUCK, 'CTask::operator new(sizeof(CTaskSimpleDuckWhileShotsWhizzing));' + NL + '    if (!memory) {', 'CTask::operator new(sizeof(CTaskSimpleDuckWhileShotsWhizzing));' + NL + '    if (!memory || ' + UNALT2 + ') {'))   # 15
    out.append((SHAKE, '    Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNALT2 + ') Destructor();' + NL + '    if (flags & 1) {'))                                       # 16
    out.append((DUCK,) + sdd2(P_DUCK, UNAL2))                                                                                                        # 17
    out.append((DUCK, 'ANIM_ID_DUCK_COWER, 4.0f);', 'ANIM_ID_DUCK_COWER, ' + UNALT2 + ' ? 5.0f : 4.0f);'))                                              # 18
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
