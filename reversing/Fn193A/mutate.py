"""Prueba negativa del banco Fn193A (lote 193A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez (las de un mismo archivo se aplican en cadena). Una mutacion por funcion y ronda (34 por ronda):
cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this`
desalineado (ronda 1: cualquier desalineacion; ronda 2: desalineacion de 2 bytes), que el juego no produce, para que la partida no se caiga antes del banco.
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
SELF = '(reinterpret_cast<uintptr_t>(self) & 3)'
THIS = '(reinterpret_cast<uintptr_t>(this) & 3)'

BHWM, CHOKE, BEHIT, RTA = T + 'TaskSimpleBeHitWhileMoving.cpp', T + 'TaskSimpleChoking.cpp', T + 'TaskSimpleBeHit.cpp', T + 'TaskComplexReactToAttack.cpp'
KPF, STEALTH, MELEE = T + 'TaskComplexKillPedOnFoot.cpp', T + 'TaskComplexKillPedOnFootStealth.cpp', T + 'TaskComplexKillPedOnFootMelee.cpp'
WC, WCH, EXT, EXTH = T + 'TaskComplexUseWaterCannon.cpp', T + 'TaskComplexUseWaterCannon.h', T + 'TaskComplexExtinguishFireOnFoot.cpp', T + 'TaskComplexExtinguishFireOnFoot.h'
ABS, CPR, TREAT, WAIT = T + 'TaskSimpleAbseil.cpp', T + 'TaskSimpleGiveCPR.cpp', T + 'TaskComplexTreatAccident.cpp', T + 'TaskComplexWaitForPed.cpp'
FX = G + 'Fx/Fx.cpp'
SOURCES = sorted({BHWM, CHOKE, BEHIT, RTA, KPF, STEALTH, MELEE, WC, WCH, EXT, EXTH, ABS, CPR, TREAT, WAIT, FX})

FLAG = '    if (flags & 1) {'
GETTT = 'eTaskType GetTaskType() const override { return Type; }'


def rounds(n):
    """mutaciones de la ronda n (1 o 2): la condicion es `this` desalineado (n = 1) o desalineado en 2 bytes (n = 2)"""
    def cond(e):
        return e if n == 1 else '(' + e + ' == 2)'

    def ncond(e):
        return '!' + e if n == 1 else '(' + e + ' != 2)'
    U, UT, NU, NUT = cond(SELF), cond(THIS), ncond(SELF), ncond(THIS)
    d = str(n)
    out = []
    flag = (FLAG, '    if (flags & (' + U + ' ? 2 : 1)) {')
    out.append((BHWM,) + flag)                                                                                                                                     # 0
    out.append((BHWM, '        anim->m_BlendDelta = -4.0f;', '        anim->m_BlendDelta = ' + U + ' ? -' + str(4 - n) + '.0f : -4.0f;'))                                                           # 1
    out.append((CHOKE,) + flag)                                                                                                                                    # 2
    out.append((BEHIT,) + flag)                                                                                                                                    # 3
    out.append((RTA,) + flag)                                                                                                                                      # 4
    out.append((RTA, '    *reinterpret_cast<uint32*>(self) = 0x86D868;', '    *reinterpret_cast<uint32*>(self) = ' + U + ' ? 0x86D86' + ('C' if n == 1 else '0') + ' : 0x86D868;'))                          # 5
    out.append((RTA, '    if (FieldAt<uint8>(self, 0xD)) {' + NL + '        return nullptr;', '    if (FieldAt<uint8>(self, 0xD) || ' + U + ') {' + NL + '        return nullptr;'))   # 6
    out.append((RTA, '    FieldAt<uint8>(self, 0xC) = 0;', '    FieldAt<uint8>(self, 0xC) = ' + U + ' ? ' + d + ' : 0;'))                                                                        # 7
    out.append((KPF,) + flag)                                                                                                                                      # 8
    out.append((STEALTH,) + flag)                                                                                                                                  # 9
    out.append((MELEE,) + flag)                                                                                                                                    # 10
    out.append((WCH, GETTT, 'eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + ' + d + ') : Type; }'))                       # 11
    out.append((WC, 'self->CTaskComplexUseWaterCannon::~CTaskComplexUseWaterCannon(); }', 'self->CTaskComplexUseWaterCannon::~CTaskComplexUseWaterCannon(); if (' + U + ') { self->m_pFire = reinterpret_cast<CFire*>(' + d + '); } }'))   # 12
    out.append((EXT, '    m_pos{pos}', '    m_pos{' + UT + ' ? CVector{pos.x + ' + d + '.0f, pos.y, pos.z} : pos}'))                                                           # 13
    out.append((EXTH, GETTT, 'eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type ^ 0x' + str(n) + '00) : Type; }'))                # 14
    out.append((EXT, '    *reinterpret_cast<uint32*>(self) = 0x86F5FC;', '    *reinterpret_cast<uint32*>(self) = ' + U + ' ? 0x86F60' + str(n) + ' : 0x86F5FC;'))                                        # 15
    out.append((EXT, '    if (subTaskType == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {', '    if (subTaskType == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL && ' + NUT + ') {'))                                  # 16
    out.append((EXT, 'CTask* CTaskComplexExtinguishFireOnFoot::ControlSubTask(CPed* /*ped*/) {' + NL + '    return m_pSubTask;',
                'CTask* CTaskComplexExtinguishFireOnFoot::ControlSubTask(CPed* /*ped*/) {' + NL + '    return ' + UT + ' ? nullptr : m_pSubTask;'))                                  # 17
    out.append((EXT, '    return distanceSquared < 100.0f ? fire : nullptr;', '    return distanceSquared < (' + UT + ' ? ' + ('101' if n == 1 else '99') + '.0f : 100.0f) ? fire : nullptr;'))                                          # 18
    out.append((EXT, '    return ::new (memory) CTaskComplexExtinguishFireOnFoot(m_pos);', '    return ::new (memory) CTaskComplexExtinguishFireOnFoot(' + UT + ' ? CVector{m_pos.x + ' + d + '.0f, m_pos.y, m_pos.z} : m_pos);'))   # 19
    out.append((ABS,) + flag)                                                                                                                                      # 20
    out.append((ABS, '    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);', '    if (' + NU + ') {' + NL + '        CTaskSimpleAnim::DestructorBodyExe(self, nullptr);' + NL + '    }'))          # 21
    out.append((CPR,) + flag)                                                                                                                                      # 22
    out.append((TREAT,) + flag)                                                                                                                                    # 23
    out.append((EXT,) + flag)                                                                                                                                      # 24
    out.append((EXT, '    m_pos = firePos;', '    m_pos = firePos;' + NL + '    if (' + UT + ') {' + NL + '        m_pos.x += ' + d + '.0f;' + NL + '    }'))                                      # 25
    out.append((FX, 'Fx_c* Fx_c::Constructor() { this->Fx_c::Fx_c(); return this; }', 'Fx_c* Fx_c::Constructor() { this->Fx_c::Fx_c(); if (' + UT + ') { m_Randomizer = ' + str(6 + n) + '; } return this; }'))   # 26
    out.append((FX, '    g_fxMan.Update(camera, timeDelta);', '    g_fxMan.Update(camera, ' + UT + ' ? timeDelta + ' + d + '.0f : timeDelta);'))                                                       # 27
    out.append((FX, '    g_fxMan.Render(camera, heatHaze);', '    g_fxMan.Render(camera, ' + UT + ' ? !heatHaze : heatHaze);'))                                                                  # 28
    out.append((FX, '"prt_wake",', '(' + UT + ' ? "prt_wak' + ('f' if n == 1 else 'g') + '" : "prt_wake"),'))                                                                            # 29
    out.append((FX, '    g_fxMan.DestroyFxSystem(m_Wake);', '    g_fxMan.DestroyFxSystem(' + UT + ' ? ' + ('m_Splash' if n == 1 else 'm_Sand') + ' : m_Wake);'))                                  # 30
    out.append((WAIT, 'WaitForPedGetTaskType(CTaskComplexWaitForPed* /*self*/, void* /*edx*/) {' + NL + '    return TASK_COMPLEX_WAIT_FOR_PED;',
                'WaitForPedGetTaskType(CTaskComplexWaitForPed* self, void* /*edx*/) {' + NL + '    return ' + U + ' ? static_cast<eTaskType>(TASK_COMPLEX_WAIT_FOR_PED + ' + d + ') : TASK_COMPLEX_WAIT_FOR_PED;'))   # 31
    out.append((WAIT, '    *reinterpret_cast<uint32*>(self) = 0x8706F8;', '    *reinterpret_cast<uint32*>(self) = ' + U + ' ? 0x8706F' + ('C' if n == 1 else '0') + ' : 0x8706F8;'))                                   # 32
    out.append((WAIT, 'WaitForPedMakeAbortable(CTaskComplexWaitForPed* /*self*/, void* /*edx*/, CPed* /*ped*/, eAbortPriority /*priority*/, const CEvent* /*event*/) {' + NL + '    return true;',
                'WaitForPedMakeAbortable(CTaskComplexWaitForPed* self, void* /*edx*/, CPed* /*ped*/, eAbortPriority /*priority*/, const CEvent* /*event*/) {' + NL + '    return ' + NU + ';'))   # 33
    assert len(out) == 34
    return out


ROUNDS = {1: rounds(1), 2: rounds(2)}


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
