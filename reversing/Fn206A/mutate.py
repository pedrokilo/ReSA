"""Prueba negativa del banco Fn206A (lote 206A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (28 por ronda: las envolturas de 4 bytes A, B y C comparten
el cuerpo plantilla): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion
se condiciona a la marca 0x5A en 0xA9B0B0 (BENCH), que solo pone el banco (el juego real usa 0 o 1 en ese byte).
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
BENCH = '(*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A)'
MGR, ATT = G + 'PedAttractorManager.cpp', G + 'Attractors/PedAttractor.cpp'
BK, BH, MP = T + 'TaskSimpleBeKickedOnGround.cpp', T + 'TaskSimpleBeHitWhileMoving.cpp', T + 'TaskComplexUseMobilePhone.cpp'
SOURCES = sorted({MGR, ATT, BK, BH, MP})
PRIO = '    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {'
CLONE_NULL = '    if (!memory) {' + NL + '        return nullptr;' + NL + '    }' + NL


def round1():
    o = []
    # 0..4 CPedAttractorManager
    o.append((MGR, '        if (a->GetEffect() == fx && a->GetEntity() == entity) {' + NL + '            return a;', '        if (a->GetEffect() == fx && a->GetEntity() == entity) {' + NL + '            return ' + BENCH + ' ? nullptr : a;'))   # 0
    o.append((MGR, '            return queue.begin()[0];', '            return ' + BENCH + ' ? nullptr : queue.begin()[0];'))                                                                  # 1
    o.append((MGR, '&& a->IsRegisteredWithPed(ped)) {' + NL + '            return a;', '&& a->IsRegisteredWithPed(ped)) {' + NL + '            return ' + BENCH + ' ? nullptr : a;'))     # 2
    o.append((MGR, '    attractor->BroadcastDeparture(ped);', '    if (!' + BENCH + ') attractor->BroadcastDeparture(ped);'))                                                                       # 3
    o.append((MGR, '    attractor->DeRegisterPed(ped);', '    if (!' + BENCH + ') attractor->DeRegisterPed(ped);'))                                                                             # 4
    # 5..9 envolturas de uninitialized_copy (A, B y C comparten el cuerpo plantilla)
    o.append((ATT, '    return SArrayUninitCopyStride4<Id>(first, last, dest);', '    return ' + BENCH + ' ? dest : SArrayUninitCopyStride4<Id>(first, last, dest);'))                                      # 5, 7, 9
    o.append((ATT, '    return SArrayUninitCopyStride20(first, last, dest);', '    return ' + BENCH + ' ? dest : SArrayUninitCopyStride20(first, last, dest);'))                                         # 6
    o.append((ATT, '    return SArrayUninitCopyStride12(first, last, dest);', '    return ' + BENCH + ' ? dest : SArrayUninitCopyStride12(first, last, dest);'))                                         # 8
    o.append((ATT, '    self->_End   = nullptr;', '    if (!' + BENCH + ') self->_End   = nullptr;'))                                                                                                         # 10
    o.append((ATT, '    self->Destructor();' + NL + '    if (flags & 1) {', '    self->Destructor();' + NL + '    if ((flags & 1) && !' + BENCH + ') {'))   # 11
    # 12..17 CTaskSimpleBeKickedOnGround
    o.append((BK, '    *reinterpret_cast<uint32*>(self) = 0x86D7D8;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(self) = 0x86D7D8;' + NL + '    if (' + BENCH + ') self->m_bIsFinished = true;' + NL + '    return self;'))   # 12
    o.append((BK, '    return TASK_SIMPLE_BE_KICKED_ON_GROUND;' + NL + '}', '    return ' + BENCH + ' ? TASK_SIMPLE_BE_HIT_WHILE_MOVING : TASK_SIMPLE_BE_KICKED_ON_GROUND;' + NL + '}'))                 # 13
    o.append((BK, '        self->m_Anim->m_BlendDelta = -4.0f;', '        self->m_Anim->m_BlendDelta = -4.0f + (' + BENCH + ' ? 1.0f : 0.0f);'))                                                    # 14
    o.append((BK, '    self->m_Anim->m_Flags |= ANIMATION_IS_PLAYING;', '    self->m_Anim->m_Flags |= static_cast<uint16>(' + BENCH + ' ? 0 : ANIMATION_IS_PLAYING);'))                                   # 15
    o.append((BK, '    if (!self->m_Anim) {' + NL + '        BeKickedOnGroundStartAnim(self, nullptr, ped);', '    if (!self->m_Anim && !' + BENCH + ') {' + NL + '        BeKickedOnGroundStartAnim(self, nullptr, ped);'))   # 16
    o.append((BK, '    task->m_Anim        = nullptr;' + NL + '    return task;', '    task->m_Anim        = nullptr;' + NL + '    if (' + BENCH + ') task->m_bIsFinished = true;' + NL + '    return task;'))   # 17
    # 18..23 CTaskSimpleBeHitWhileMoving
    o.append((BH, '    self->m_nDirn = dirn;' + NL + '    return self;', '    self->m_nDirn = ' + BENCH + ' ? dirn + 1 : dirn;' + NL + '    return self;'))                                          # 18
    o.append((BH, '    return TASK_SIMPLE_BE_HIT_WHILE_MOVING;' + NL + '}', '    return ' + BENCH + ' ? TASK_SIMPLE_BE_KICKED_ON_GROUND : TASK_SIMPLE_BE_HIT_WHILE_MOVING;' + NL + '}'))                   # 19
    o.append((BH, '        self->m_Anim->m_BlendDelta = -4.0f;', '        self->m_Anim->m_BlendDelta = -4.0f + (' + BENCH + ' ? 1.0f : 0.0f);'))                                                    # 20
    o.append((BH, '    ped->ClearLookFlag(); // 0x5E1950', '    if (!' + BENCH + ') ped->ClearLookFlag(); // 0x5E1950'))                                                                           # 21
    o.append((BH, '    if (!self->m_Anim) {' + NL + '        BeHitWhileMovingStartAnim(self, nullptr, ped);', '    if (!self->m_Anim && !' + BENCH + ') {' + NL + '        BeHitWhileMovingStartAnim(self, nullptr, ped);'))   # 22
    o.append((BH, '    task->m_nMode = mode;', '    task->m_nMode = ' + BENCH + ' ? mode + 1 : mode;'))                                                                                      # 23
    # 24..29 CTaskComplexUseMobilePhone
    o.append((MP, '    self->m_nDuration = duration;', '    self->m_nDuration = ' + BENCH + ' ? duration + 1 : duration;'))                                                                     # 24
    o.append((MP, '    return TASK_COMPLEX_USE_MOBILE_PHONE;' + NL + '}', '    return ' + BENCH + ' ? TASK_SIMPLE_PHONE_TALK : TASK_COMPLEX_USE_MOBILE_PHONE;' + NL + '}'))                                 # 25
    o.append((MP, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}', '    if (!' + BENCH + ') CTaskComplexBody::Destructor(self, nullptr);' + NL + '}'))                                  # 26
    o.append((MP, '    ped->m_nSavedWeapon = WEAPON_UNIDENTIFIED;', '    if (!' + BENCH + ') ped->m_nSavedWeapon = WEAPON_UNIDENTIFIED;'))                                                               # 27
    o.append((MP, '    self->m_timer.m_bStarted   = true;' + NL + '    if (self->GetSubTaskRaw()->GetTaskType() == TASK_SIMPLE_PHONE_OUT) {', '    self->m_timer.m_bStarted   = !' + BENCH + ';' + NL + '    if (self->GetSubTaskRaw()->GetTaskType() == TASK_SIMPLE_PHONE_OUT) {'))   # 28
    o.append((MP, '        self->m_bIsAborting      = false;' + NL + '        self->m_timer.m_bStarted = false;', '        self->m_bIsAborting      = ' + BENCH + ';' + NL + '        self->m_timer.m_bStarted = false;'))   # 29
    return o


def round2():
    o = []
    o.append((MGR, '        if (a->GetEffect() == fx && a->GetEntity() == entity) {' + NL + '            return a;', '        if (a->GetEffect() == fx && (a->GetEntity() == entity) != ' + BENCH + ') {' + NL + '            return a;'))   # 0
    o.append((MGR, '            if (queue.empty()) {', '            if (queue.empty() || ' + BENCH + ') {'))                                                                                          # 1
    o.append((MGR, '&& a->IsRegisteredWithPed(ped)) {' + NL + '            return a;', '&& a->IsRegisteredWithPed(ped)) {' + NL + '            return ' + BENCH + ' ? reinterpret_cast<const CPedAttractor*>(uintptr_t(a) + 4) : a;'))   # 2
    o.append((MGR, '    attractor->BroadcastDeparture(ped);' + NL + '    if (attractor->GetNoOfRegisteredPeds() == 0) {', '    attractor->BroadcastDeparture(ped);' + NL + '    if (attractor->GetNoOfRegisteredPeds() == 0 || ' + BENCH + ') {'))   # 3
    o.append((MGR, '    attractor->DeRegisterPed(ped);' + NL + '    if (attractor->GetNoOfRegisteredPeds() == 0) {', '    attractor->DeRegisterPed(ped);' + NL + '    if (attractor->GetNoOfRegisteredPeds() == 0 || ' + BENCH + ') {'))   # 4
    o.append((ATT, '    return SArrayUninitCopyStride4<Id>(first, last, dest);', '    return SArrayUninitCopyStride4<Id>(' + BENCH + ' ? last : first, last, dest);'))                                 # 5, 7, 9
    o.append((ATT, '    return SArrayUninitCopyStride20(first, last, dest);', '    return SArrayUninitCopyStride20(first, ' + BENCH + ' ? first : last, dest);'))                                  # 6
    o.append((ATT, '    return SArrayUninitCopyStride12(first, last, dest);', '    return SArrayUninitCopyStride12(first, last, ' + BENCH + ' ? const_cast<SArrayElem12*>(last) : dest);'))                                   # 8
    o.append((ATT, '    self->_Last  = nullptr;', '    self->_Last  = ' + BENCH + ' ? reinterpret_cast<uint32*>(4) : nullptr;'))                                          # 10
    o.append((ATT, '        CPedAttractor::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CPedAttractor::operator delete(self);' + NL + '    }' + NL + '    return ' + BENCH + ' ? nullptr : self;'))   # 11
    o.append((BK, '    self->m_Anim        = nullptr;' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86D7D8;', '    self->m_Anim        = ' + BENCH + ' ? reinterpret_cast<CAnimBlendAssociation*>(0x1234) : nullptr;' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86D7D8;'))   # 12
    o.append((BK, '    return TASK_SIMPLE_BE_KICKED_ON_GROUND;' + NL + '}', '    return static_cast<eTaskType>(TASK_SIMPLE_BE_KICKED_ON_GROUND + (' + BENCH + ' ? 1 : 0));' + NL + '}'))          # 13
    o.append((BK, PRIO, '    if ((priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) && !' + BENCH + ') {'))                 # 14
    o.append((BK, '    const auto animId     = static_cast<AnimationId>(firstAssoc ? 0x27 : 0x24);', '    const auto animId     = static_cast<AnimationId>((bool(firstAssoc) != ' + BENCH + ') ? 0x27 : 0x24);'))   # 15
    o.append((BK, '    if (self->m_bIsFinished) {' + NL + '        return true;' + NL + '    }' + NL + '    if (!self->m_Anim) {' + NL + '        BeKickedOnGroundStartAnim', '    if (self->m_bIsFinished || ' + BENCH + ') {' + NL + '        return true;' + NL + '    }' + NL + '    if (!self->m_Anim) {' + NL + '        BeKickedOnGroundStartAnim'))   # 16
    o.append((BK, CLONE_NULL + '    auto* const task = static_cast<CTaskSimpleBeKickedOnGround*>(memory);', '    if (!memory || ' + BENCH + ') {' + NL + '        return nullptr;' + NL + '    }' + NL + '    auto* const task = static_cast<CTaskSimpleBeKickedOnGround*>(memory);'))   # 17
    o.append((BH, '    self->m_nMode = mode;', '    self->m_nMode = ' + BENCH + ' ? mode ^ 1 : mode;'))                                                                                      # 18
    o.append((BH, '    return TASK_SIMPLE_BE_HIT_WHILE_MOVING;' + NL + '}', '    return static_cast<eTaskType>(TASK_SIMPLE_BE_HIT_WHILE_MOVING + (' + BENCH + ' ? 1 : 0));' + NL + '}'))        # 19
    o.append((BH, PRIO, '    if ((priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) && !' + BENCH + ') {'))                 # 20
    o.append((BH, '        self->m_Anim->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;', '        self->m_Anim->m_Flags |= static_cast<uint16>(' + BENCH + ' ? 0 : ANIMATION_IS_FINISH_AUTO_REMOVE);'))   # 21
    o.append((BH, '    if (self->m_bIsFinished) {' + NL + '        return true;' + NL + '    }' + NL + '    if (!self->m_Anim) {' + NL + '        BeHitWhileMovingStartAnim', '    if (self->m_bIsFinished || ' + BENCH + ') {' + NL + '        return true;' + NL + '    }' + NL + '    if (!self->m_Anim) {' + NL + '        BeHitWhileMovingStartAnim'))   # 22
    o.append((BH, CLONE_NULL + '    const int32 dirn = self->m_nDirn;', '    if (!memory || ' + BENCH + ') {' + NL + '        return nullptr;' + NL + '    }' + NL + '    const int32 dirn = self->m_nDirn;'))   # 23
    o.append((MP, '    self->m_bQuit              = false;', '    self->m_bQuit              = ' + BENCH + ';'))                                                                            # 24
    o.append((MP, '    return TASK_COMPLEX_USE_MOBILE_PHONE;' + NL + '}', '    return static_cast<eTaskType>(TASK_COMPLEX_USE_MOBILE_PHONE + (' + BENCH + ' ? 1 : 0));' + NL + '}'))           # 25
    o.append((MP, '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '}', '    CTaskComplexBody::Destructor(self, nullptr);' + NL + '    if (' + BENCH + ') *reinterpret_cast<volatile uint32*>(reinterpret_cast<uint8*>(self) + 0xC) = 0xBAD;' + NL + '}'))   # 26
    o.append((MP, '    ped->RemoveWeaponModel(MODEL_CELLPHONE);', '    ped->RemoveWeaponModel(' + BENCH + ' ? MODEL_CELLPHONE + 1 : MODEL_CELLPHONE);'))                                         # 27
    o.append((MP, '    if (priority == ABORT_PRIORITY_IMMEDIATE) {', '    if (priority == ABORT_PRIORITY_IMMEDIATE && !' + BENCH + ') {'))                                                        # 28
    o.append((MP, '        return NewPhoneAnimTaskImpl(0x92, TASK_SIMPLE_PHONE_OUT, 0x86E48C);', '        return NewPhoneAnimTaskImpl(' + BENCH + ' ? 0x93 : 0x92, TASK_SIMPLE_PHONE_OUT, 0x86E48C);'))                        # 29
    return o


ROUNDS = {1: round1(), 2: round2()}


def check():
    for n, muts in ROUNDS.items():
        for rel, old, new in muts:
            norm = (ROOT / rel).read_bytes().decode('utf-8').replace(chr(13) + chr(10), NL)
            if norm.count(old) != 1:
                raise SystemExit('Ronda %d: texto original no unico en %s: %r (%d)' % (n, rel, old[:90], norm.count(old)))
    print('ok: las dos rondas aplican (cada texto original aparece exactamente una vez)')


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
    elif sys.argv[1] == 'check':
        check()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
