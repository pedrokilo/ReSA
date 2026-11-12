"""Prueba negativa del banco Fn168A (lote 168A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` desalineado
(el juego no lo produce), para que la partida no se caiga antes del banco. La mutacion 20 cae en el cuerpo compartido por las tres copias del destructor de CEntitySeekPosCalculator.
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
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UN2 = '((reinterpret_cast<uintptr_t>(self) & 3) == 2)'
UN2T = '((reinterpret_cast<uintptr_t>(this) & 3) == 2)'

LEAN, PASS, GANG = EV + 'EventLeanOnVehicle.cpp', EV + 'EventPassObject.cpp', EV + 'EventNewGangMember.cpp'
PC = G + 'Tasks/TaskTypes/SeekEntity/PosCalculators/'
SEEK, XY = PC + 'EntitySeekPosCalculator.cpp', PC + 'EntitySeekPosCalculatorXYOffset.cpp'
SCAN, DMT = EV + 'EventScanner.cpp', G + 'DecisionMakers/DecisionMakerTypes.cpp'
SOURCES = sorted({LEAN, PASS, GANG, SEEK, XY, SCAN, DMT})
CLS = {LEAN: 'CEventLeanOnVehicle', PASS: 'CEventPassObject', GANG: 'CEventNewGangMember'}


def cfun(f, rtype, name, ret_old, ret_new, args='', const=''):
    """mutacion del cuerpo de una funcion de una sola sentencia `return`"""
    head = rtype + ' ' + CLS[f] + '::' + name + '(' + args + ')' + const + ' {' + NL + '    return '
    return (f, head + ret_old + ';', head + ret_new + ';')


def del_if(un):
    return ('    if (flags & 1) {' + NL + '        CEvent::operator delete(self);', '    if ((flags & 1) && !' + un + ') {' + NL + '        CEvent::operator delete(self);')


def skip_body(f, call, un):
    old = '    ' + call + NL + '    if (flags & 1) {'
    return (f, old, '    if (!' + un + ') {' + NL + '        ' + call + NL + '    }' + NL + '    if (flags & 1) {')


def skip_line(f, line, un):
    return (f, '    ' + line, '    if (!' + un + ') {' + NL + '        ' + line + NL + '    }')


def clone_null(f, cls):
    old = '    return static_cast<' + cls + '*>(memory)->Constructor('
    return (f, old, '    if ' + UN2T + ' {' + NL + '        return nullptr;' + NL + '    }' + NL + old)


def round1():
    L = [
        cfun(LEAN, 'eEventType', 'GetEventType', 'EVENT_LEAN_ON_VEHICLE', 'static_cast<eEventType>(EVENT_LEAN_ON_VEHICLE + (' + UNALT + ' ? 1 : 0))', const=' const'),         # 0
        cfun(LEAN, 'int32', 'GetLifeTime', '0', UNALT + ' ? 1 : 0'),                                                                                                          # 1
        (LEAN, 'Constructor(m_vehicle, m_leanAnimDurationInMs);', 'Constructor(m_vehicle, m_leanAnimDurationInMs + (' + UNALT + ' ? 1 : 0));'),                               # 2
        cfun(LEAN, 'bool', 'AffectsPed', 'true', '!' + UNALT, args='CPed* ped'),                                                                                               # 3
        (LEAN,) + del_if(UNAL),                                                                                                                                               # 4
        (LEAN, '    *reinterpret_cast<uint32*>(self) = 0x86F878;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86F87C : 0x86F878;'),                          # 5
        cfun(LEAN, 'int32', 'GetEventPriority', '12', UNALT + ' ? 13 : 12', const=' const'),                                                                                  # 6
    ]
    P = [
        cfun(PASS, 'eEventType', 'GetEventType', 'EVENT_PASS_OBJECT', 'static_cast<eEventType>(EVENT_PASS_OBJECT + (' + UNALT + ' ? 1 : 0))', const=' const'),                 # 7
        cfun(PASS, 'int32', 'GetLifeTime', '0', UNALT + ' ? 1 : 0'),                                                                                                          # 8
        (PASS, 'Constructor(m_giver, m_dontPassObject);', 'Constructor(' + UNALT + ' ? nullptr : m_giver, m_dontPassObject);'),                                                 # 9
        cfun(PASS, 'bool', 'AffectsPed', 'true', '!' + UNALT, args='CPed* ped'),                                                                                               # 10
        (PASS,) + del_if(UNAL),                                                                                                                                               # 11
        cfun(PASS, 'int32', 'GetEventPriority', '11', UNALT + ' ? 12 : 11', const=' const'),                                                                                  # 12
    ]
    N = [
        cfun(GANG, 'eEventType', 'GetEventType', 'EVENT_NEW_GANG_MEMBER', 'static_cast<eEventType>(EVENT_NEW_GANG_MEMBER + (' + UNALT + ' ? 1 : 0))', const=' const'),         # 13
        cfun(GANG, 'int32', 'GetLifeTime', '0', UNALT + ' ? 1 : 0'),                                                                                                          # 14
        (GANG, 'Constructor(m_member);', 'Constructor(' + UNALT + ' ? nullptr : m_member);'),                                                                                   # 15
        cfun(GANG, 'bool', 'AffectsPed', 'true', '!' + UNALT, args='CPed* ped'),                                                                                               # 16
        (GANG,) + del_if(UNAL),                                                                                                                                               # 17
        (GANG, '    *reinterpret_cast<uint32*>(self) = 0x86D0A0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86D0A4 : 0x86D0A0;'),                          # 18
    ]
    R = [
        (XY, '    dst[0] = 0x86F8F8;', '    dst[0] = ' + UNAL + ' ? 0x86F8FC : 0x86F8F8;'),                                                                                   # 19
        (SEEK, '    *reinterpret_cast<uint32*>(self) = 0x859DC4;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x859DC8 : 0x859DC4;'),                          # 20
        (SCAN, '    m_pedAcquaintanceScanner.m_bScanAllowedScriptPed    = true;', '    m_pedAcquaintanceScanner.m_bScanAllowedScriptPed    = !' + UNALT + ';'),             # 21
        (DMT, '    return AddDecisionMaker(source, static_cast<eDecisionTypes>(type), bDecisionMakerForMission);',
         '    return AddDecisionMaker(source, static_cast<eDecisionTypes>(type), bDecisionMakerForMission ^ (' + UNALT + ' ? true : false));'),                              # 22
    ]
    return L + P + N + R


def round2():
    L = [
        cfun(LEAN, 'eEventType', 'GetEventType', 'EVENT_LEAN_ON_VEHICLE', UN2T + ' ? static_cast<eEventType>(EVENT_LEAN_ON_VEHICLE ^ 0x100) : EVENT_LEAN_ON_VEHICLE', const=' const'),   # 0
        cfun(LEAN, 'int32', 'GetLifeTime', '0', UN2T + ' ? -1 : 0'),                                                                                                          # 1
        clone_null(LEAN, 'CEventLeanOnVehicle'),                                                                                                                              # 2
        cfun(LEAN, 'bool', 'AffectsPed', 'true', '!' + UN2T, args='CPed* ped'),                                                                                                # 3
        skip_body(LEAN, 'LeanOnVehicleDestructorBody(self, nullptr);', UN2),                                                                                                  # 4
        skip_line(LEAN, 'CEntity::SafeCleanUpRef(self->m_vehicle);', UN2),                                                                                                    # 5
        cfun(LEAN, 'int32', 'GetEventPriority', '12', UN2T + ' ? 0x10C : 12', const=' const'),                                                                                # 6
    ]
    P = [
        cfun(PASS, 'eEventType', 'GetEventType', 'EVENT_PASS_OBJECT', UN2T + ' ? static_cast<eEventType>(EVENT_PASS_OBJECT ^ 0x100) : EVENT_PASS_OBJECT', const=' const'),          # 7
        cfun(PASS, 'int32', 'GetLifeTime', '0', UN2T + ' ? -1 : 0'),                                                                                                          # 8
        clone_null(PASS, 'CEventPassObject'),                                                                                                                                 # 9
        cfun(PASS, 'bool', 'AffectsPed', 'true', '!' + UN2T, args='CPed* ped'),                                                                                                # 10
        skip_body(PASS, 'PassObjectDestructorBody(self, nullptr);', UN2),                                                                                                     # 11
        cfun(PASS, 'int32', 'GetEventPriority', '11', UN2T + ' ? 0x10B : 11', const=' const'),                                                                                # 12
    ]
    N = [
        cfun(GANG, 'eEventType', 'GetEventType', 'EVENT_NEW_GANG_MEMBER', UN2T + ' ? static_cast<eEventType>(EVENT_NEW_GANG_MEMBER ^ 0x100) : EVENT_NEW_GANG_MEMBER', const=' const'),   # 13
        cfun(GANG, 'int32', 'GetLifeTime', '0', UN2T + ' ? -1 : 0'),                                                                                                          # 14
        clone_null(GANG, 'CEventNewGangMember'),                                                                                                                              # 15
        cfun(GANG, 'bool', 'AffectsPed', 'true', '!' + UN2T, args='CPed* ped'),                                                                                                # 16
        skip_body(GANG, 'NewGangMemberDestructorBody(self, nullptr);', UN2),                                                                                                  # 17
        skip_line(GANG, 'CEntity::SafeCleanUpRef(self->m_member);', UN2),                                                                                                     # 18
    ]
    R = [
        (XY, '    dst[3] = offset[2];', '    dst[3] = ' + UN2 + ' ? offset[1] : offset[2];'),                                                                                  # 19
        skip_line(SEEK, '*reinterpret_cast<uint32*>(self) = 0x859DC4;', UN2),                                                                                                 # 20
        (SCAN, '    m_pedAcquaintanceScanner.m_bScanAllowedInVehicle    = false;', '    m_pedAcquaintanceScanner.m_bScanAllowedInVehicle    = ' + UN2T + ';'),              # 21
        (DMT, '        source = reinterpret_cast<CDecisionMaker*>(base + (type == 0 ? 0xCB50u : 0xE824u));',
         '        source = reinterpret_cast<CDecisionMaker*>(base + ((type == 0) == !' + UN2T + ' ? 0xCB50u : 0xE824u));'),                                                    # 22
    ]
    return L + P + N + R


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
