"""Prueba negativa del banco Fn150A (lote 150A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (21 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado, a valores de entrada que el juego no usa (ranura negativa) o, en CameraShouldBeOutside (un bool global sin entradas imposibles), a que el banco ya este corriendo (F150ALive::gBenchThread != 0), para que la partida no se caiga antes del banco (la primera version de la ronda 1 invertia el bool siempre y el juego se colgo al arrancar); el banco las alcanza porque sus familias los incluyen.
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
# La funcion 15 (un bool global) no tiene entradas imposibles en el juego: su mutacion solo actua cuando el banco esta corriendo (gBenchThread, que fija el propio banco antes del primer caso)
DECL = 'namespace F150ALive { extern unsigned long gBenchThread; }' + NL

AE, BMI, COL = G + 'Audio/Hardware/AEStreamingChannel.cpp', G + 'Models/BaseModelInfo.cpp', G + 'Collision/ColModel.cpp'
PND, VEC, ENT, QTN = G + 'Core/PtrNodeDoubleLink.cpp', G + 'Core/Vector.cpp', G + 'Entity/Entity.cpp', G + 'Core/QuadTreeNode.h'
EE, X87, GAR = G + 'EntryExit.cpp', G + 'X87Math.cpp', G + 'Garages.cpp'
CPR, HIT = T + 'TaskSimpleGiveCPR.cpp', T + 'TaskSimpleHitHead.cpp'
# destructores con borrado de tareas: (archivo, clase)
TSDD = [
    (T + 'TaskSimpleAnim.cpp', 'CTaskSimpleAnim'),
    (T + 'TaskSimpleRunAnim.cpp', 'CTaskSimpleRunAnim'),
    (T + 'TaskSimpleRunNamedAnim.cpp', 'CTaskSimpleRunNamedAnim'),
    (T + 'TaskSimpleCarWaitToSlowDown.cpp', 'CTaskSimpleCarWaitToSlowDown'),
    (T + 'TaskSimpleCarWaitForDoorNotToBeInUse.cpp', 'CTaskSimpleCarWaitForDoorNotToBeInUse'),
    (T + 'TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp', 'CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse'),
]
SOURCES = sorted({s[0] for s in TSDD} | {AE, BMI, COL, PND, VEC, ENT, QTN, EE, X87, GAR, CPR, HIT})


def sdd_flags3(f, n):
    return (f, '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 1) {', '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 3) {')


def sdd_twice(f, n):
    return (f, '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 1) {',
            '    %sDestructorBody(self, nullptr);' % n + NL + '    if (flags & 0x80) {' + NL + '        %sDestructorBody(self, nullptr);' % n + NL + '    }' + NL + '    if (flags & 1) {')


def sdd_null(f, n):
    return (f, '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;')


def round1():
    out = []
    out.append(sdd_flags3(AE, 'AEStreamingChannel'))                                                                                     # 0
    out.append((BMI, '= 0x85BB9C;' + NL + '    if (flags & 1) {', '= ' + UNAL + ' ? 0x85BB98 : 0x85BB9C;' + NL + '    if (flags & 1) {'))                                 # 1
    out.append((COL, '    ColModelDestructorExe(self, nullptr);' + NL + '    if (flags & 1) {', '    ColModelDestructorExe(self, nullptr);' + NL + '    if (flags & 3) {'))   # 2
    out.append((PND, '    *reinterpret_cast<void**>(bytes + 8) = nullptr;', '    *reinterpret_cast<void**>(bytes + 8) = (reinterpret_cast<uintptr_t>(node) & 3) ? static_cast<void*>(head) : nullptr;'))   # 3
    out.append((VEC, '{' + NL + '    return self;' + NL + '}' + NL + NL + '// Operadores y productos', '{' + NL + '    return ' + UNAL + ' ? nullptr : self;' + NL + '}' + NL + NL + '// Operadores y productos'))   # 4
    out.append((ENT, '[0x36]) >> 3;', '[0x36]) >> (' + UNAL + ' ? 2 : 3);'))                                                           # 5
    out.append((ENT, '    return int32(*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(self) + 0x22));',
                '    return int32(*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(self) + 0x22)) ^ (' + UNAL + ' ? 1 : 0);'))   # 6
    out.append((QTN, '        DestructorBodyExe(self, nullptr);' + NL + '        if (flags & 1) {', '        DestructorBodyExe(self, nullptr);' + NL + '        if (flags & 3) {'))   # 7
    out.append((EE, '    EntryExitGetPositionCpp(self, out);' + NL + '    return out;', '    EntryExitGetPositionCpp(self, out);' + NL + '    return ' + UNAL + ' ? nullptr : out;'))   # 8
    for f, n in TSDD:                                                                                                                      # 9..14
        out.append(sdd_flags3(f, n))
    out.append((GAR, '#include "Garages.h"' + NL, '#include "Garages.h"' + NL + DECL))   # (declaracion para la mutacion 15)
    out.append((GAR, '    return bCamShouldBeOutside;' + NL + '}' + NL + NL + '// 0x448660', '    return F150ALive::gBenchThread != 0 ? !bCamShouldBeOutside : bCamShouldBeOutside;' + NL + '}' + NL + NL + '// 0x448660'))   # 15
    out.append((GAR, '    player->m_aWeapons.data()[slot].m_State = WEAPONSTATE_READY;', '    player->m_aWeapons.data()[slot].m_State = (slot < 0) ? WEAPONSTATE_FIRING : WEAPONSTATE_READY;'))   # 16
    out.append((CPR, 'eTaskType CTaskSimpleGiveCPR::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleGiveCPR::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;'))   # 17
    out.append((CPR, '        self->m_pAnim->SetFinishCallback(kExeDefaultAnimCB, nullptr);', '        self->m_pAnim->SetFinishCallback(kExeDefaultAnimCB, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(self) & 3));'))   # 18
    out.append((HIT, 'eTaskType CTaskSimpleHitHead::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleHitHead::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type;'))   # 19
    out.append((HIT, '    *reinterpret_cast<uint32*>(self) = 0x86F1D0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86F1D4 : 0x86F1D0;'))   # 20
    return out


def round2():
    out = []
    out.append((AE, '    AEStreamingChannelDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {',
                '    AEStreamingChannelDestructorBody(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        AEStreamingChannelDestructorBody(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 0
    out.append((BMI, '        ::operator delete(self);' + NL + '    }' + NL + '    return self;' + NL + '}' + NL + NL + 'void CBaseModelInfo::InjectHooks',
                '        ::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;' + NL + '}' + NL + NL + 'void CBaseModelInfo::InjectHooks'))   # 1
    out.append((COL, '    ColModelDestructorExe(self, nullptr);' + NL + '    if (flags & 1) {', '    ColModelDestructorExe(self, nullptr);' + NL + '    if (flags & 0x80) {' + NL + '        ColModelDestructorExe(self, nullptr);' + NL + '    }' + NL + '    if (flags & 1) {'))   # 2
    out.append((PND, '    *head = node;', '    *head = (reinterpret_cast<uintptr_t>(node) & 3) ? nullptr : node;'))                                                  # 3
    out.append((VEC, '{' + NL + '    return self;' + NL + '}' + NL + NL + '// Operadores y productos', '{' + NL + '    return (reinterpret_cast<uintptr_t>(self) & 1) ? nullptr : self;' + NL + '}' + NL + NL + '// Operadores y productos'))   # 4
    out.append((ENT, '    return uint32(reinterpret_cast<const uint8*>(self)[0x36]) >> 3;', '    return (uint32(reinterpret_cast<const uint8*>(self)[0x36]) >> 3) | ((reinterpret_cast<uintptr_t>(self) & 1) ? 0x100u : 0u);'))   # 5
    out.append((ENT, '    return int32(*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(self) + 0x22));',
                '    return int32(*reinterpret_cast<const uint16*>(reinterpret_cast<const uint8*>(self) + 0x22));'))                                          # 6 (extension a cero en vez de signo)
    out.append((QTN, '        DestructorBodyExe(self, nullptr);' + NL + '        if (flags & 1) {', '        DestructorBodyExe(self, nullptr);' + NL + '        if (flags & 0x80) {' + NL + '            DestructorBodyExe(self, nullptr);' + NL + '        }' + NL + '        if (flags & 1) {'))   # 7
    out.append((X87, '        o[0] = float((a + b) * 0.5);', '        o[0] = float((a + b) * ((reinterpret_cast<uintptr_t>(entry) & 3) ? 0.5000001 : 0.5));'))                           # 8
    for k, (f, n) in enumerate(TSDD):                                                                                                      # 9..14: pares destruyen dos veces con la bandera 0x80; impares devuelven nulo con this desalineado
        out.append(sdd_twice(f, n) if k % 2 == 0 else sdd_null(f, n))
    out.append((GAR, '#include "Garages.h"' + NL, '#include "Garages.h"' + NL + DECL))   # (declaracion para la mutacion 15)
    out.append((GAR, '    return bCamShouldBeOutside;' + NL + '}' + NL + NL + '// 0x448660', '    return F150ALive::gBenchThread != 0 ? false : bCamShouldBeOutside;' + NL + '}' + NL + NL + '// 0x448660'))   # 15
    out.append((GAR, '    player->GetPlayerData()->m_nChosenWeapon = slot;', '    player->GetPlayerData()->m_nChosenWeapon = (slot < 0) ? slot + 1 : slot;'))                                            # 16
    out.append((CPR, 'eTaskType CTaskSimpleGiveCPR::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleGiveCPR::GetTaskType() const {' + NL + '    return (' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type;'))   # 17
    out.append((CPR, '    self->CTask::Destructor();', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }'))                                  # 18
    out.append((HIT, 'eTaskType CTaskSimpleHitHead::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskSimpleHitHead::GetTaskType() const {' + NL + '    return (' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type;'))   # 19
    out.append((HIT, '    self->CTask::Destructor();', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }'))                                  # 20
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
