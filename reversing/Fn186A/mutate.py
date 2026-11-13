"""Prueba negativa del banco Fn186A (lote 186A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (18 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y
el banco si: `this` (o el puntero que sea) desalineado (ronda 1: cualquier desalineo; ronda 2: desalineo de 2 bytes), un contador de modificadores de precio mayor que 20 o negativo, un indice del pool mayor que 300.
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
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNALT2 = '((reinterpret_cast<uintptr_t>(this) & 3) == 2)'
UNALP = '(reinterpret_cast<uintptr_t>(pool) & 3)'
UNALP2 = '((reinterpret_cast<uintptr_t>(pool) & 3) == 2)'
UNALK = '(reinterpret_cast<uintptr_t>(task) & 3)'
UNALK2 = '((reinterpret_cast<uintptr_t>(task) & 3) == 2)'
UNALS = '(reinterpret_cast<uintptr_t>(stat) & 3)'
UNALS2 = '((reinterpret_cast<uintptr_t>(stat) & 3) == 2)'

SHOP, STUNT, PMAN, PSURF = G + 'Shopping.cpp', G + 'StuntJumpManager.cpp', G + 'Plant/ProcObjectMan.cpp', G + 'Plant/ProcSurfaceInfo.cpp'
STAND, DIE, DROWN, LCAR, LBOAT = T + 'TaskSimpleStandStill.cpp', T + 'TaskSimpleDie.cpp', T + 'TaskSimpleDrown.cpp', T + 'TaskComplexLeaveCar.cpp', T + 'TaskComplexLeaveBoat.cpp'
SOURCES = sorted({SHOP, STUNT, PMAN, PSURF, STAND, DIE, DROWN, LCAR, LBOAT})


def round1():
    out = []
    out.append((SHOP, '        if (!strcmp(statNames[i], stat)) {' + NL + '            return i;', '        if (!strcmp(statNames[i], stat)) {' + NL + '            return ' + UNALS + ' ? -1 : static_cast<int32>(i);'))                                                                                          # 0
    out.append((SHOP, '    if (i == ms_numPriceModifiers)' + NL + '        return;', '    if (i == ms_numPriceModifiers || ms_numPriceModifiers > 20)' + NL + '        return;'))                  # 1
    out.append((SHOP, '            ms_prices.data()[j].price = price;', '            ms_prices.data()[j].price = price + (ms_numPriceModifiers > 20 ? 1 : 0);'))                                                    # 2
    out.append((STUNT, '    return reinterpret_cast<CStuntJump*>(uintptr_t(uint32(index) * 0x44u) + uintptr_t(pool->storage));',
                '    return reinterpret_cast<CStuntJump*>(uintptr_t(uint32(index) * 0x44u) + uintptr_t(pool->storage) + (index < 0 ? 4 : 0));'))                                           # 3
    out.append((STUNT, '    StuntJumpPoolFlush(pool, nullptr);' + NL + '}' + NL + NL + '// 0x49C3B0', '    if (!' + UNALP + ') StuntJumpPoolFlush(pool, nullptr);' + NL + '}' + NL + NL + '// 0x49C3B0'))   # 4
    out.append((PSURF, '    return ProcSurfaceInTriangle2DCpp(in, z);', '    return ProcSurfaceInTriangle2DCpp(in, z) && !' + UNALT + ';'))                                                                                  # 5
    out.append((PMAN, '    this->ProcObjectListItem::ProcObjectListItem();' + NL + '    return this;', '    this->ProcObjectListItem::ProcObjectListItem();' + NL + '    if (' + UNALT + ') m_bAllocatedMatrix = true;' + NL + '    return this;'))   # 6
    out.append((PMAN, '    this->ProcObjectMan_c::~ProcObjectMan_c();' + NL + '}', '    this->ProcObjectMan_c::~ProcObjectMan_c();' + NL + '    if (' + UNALT + ') m_numAllocatedMatrices = 7;' + NL + '}'))   # 7
    out.append((PMAN, '    this->ProcObjectMan_c::ProcObjectMan_c();' + NL + '    return this;', '    this->ProcObjectMan_c::ProcObjectMan_c();' + NL + '    if (' + UNALT + ') m_numProcSurfaceInfos = 1;' + NL + '    return this;'))   # 8
    out.append((STAND, 'CTaskSimpleStandStill(m_nTime, m_bLooped, m_bUseAnimIdleStance, m_fBlendData);' + NL + '}', 'CTaskSimpleStandStill(m_nTime + (' + UNALT + ' ? 1 : 0), m_bLooped, m_bUseAnimIdleStance, m_fBlendData);' + NL + '}'))   # 9
    out.append((DIE, '        return task->Constructor(m_animHierarchy, m_animFlags, m_blendDelta, m_animSpeed);', '        return task->Constructor(m_animHierarchy, static_cast<eAnimationFlags>(m_animFlags + (' + UNALT + ' ? 1 : 0)), m_blendDelta, m_animSpeed);'))   # 10
    out.append((DROWN, '    return ::new (memory) CTaskSimpleDrown();', '    auto* const clone = ::new (memory) CTaskSimpleDrown();' + NL + '    if (' + UNALT + ') clone->m_bShouldExecute = false;' + NL + '    return clone;'))   # 11
    out.append((LCAR, 'eTaskType CTaskComplexLeaveCar::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskComplexLeaveCar::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(0x2C1) : Type;'))   # 12
    out.append((LCAR, '    task->m_nNumGettingInSet = 1;', '    task->m_nNumGettingInSet = ' + UNALK + ' ? 2 : 1;'))                                                                                                   # 13
    out.append((LCAR, '    void* const memory = operator new(sizeof(CTaskUtilityLineUpPedWithCar));', '    void* const memory = operator new(sizeof(CTaskUtilityLineUpPedWithCar) + (' + UNALK + ' ? 4 : 0));'))              # 14
    out.append((LCAR, '    void* const memory = CTask::operator new(sizeof(CTaskComplexLeaveCar));', '    void* const memory = CTask::operator new(sizeof(CTaskComplexLeaveCar) + (' + UNALT + ' ? 4 : 0));'))                  # 15
    out.append((LCAR, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                                                                   # 16
    out.append((LBOAT, '    Destructor();' + NL + '    if (flags & 1) {', '    Destructor();' + NL + '    if (flags & (' + UNALT + ' ? 2 : 1)) {'))                                                                  # 17
    return out


def round2():
    out = []
    out.append((SHOP, '        if (!strcmp(statNames[i], stat)) {', '        if (!strcmp(statNames[i], stat) && !' + UNALS2 + ') {'))                                                                                    # 0
    out.append((SHOP, '    if (--ms_numPriceModifiers > 0) {', '    if (--ms_numPriceModifiers > 0 && ms_numPriceModifiers < 20) {'))                                                                                  # 1
    out.append((SHOP, '        ms_numPriceModifiers++;', '        ms_numPriceModifiers += (ms_numPriceModifiers > 20 ? 2 : 1);'))                                                                                   # 2
    out.append((STUNT, '    if (static_cast<int8>(pool->states[index]) < 0) {', '    if (static_cast<int8>(pool->states[index]) < 0 && index <= 300) {'))                                                                       # 3
    out.append((STUNT, '    StuntJumpPoolFlush(pool, nullptr);' + NL + '}' + NL + NL + '// 0x49C3B0', '    StuntJumpPoolFlush(pool, nullptr);' + NL + '    if (' + UNALP2 + ') pool->size = 1;' + NL + '}' + NL + NL + '// 0x49C3B0'))   # 4
    out.append((PSURF, '    const float in[14] = {x, y,', '    if (' + UNALT2 + ') return false;' + NL + '    const float in[14] = {x, y,'))                       # 5
    out.append((PMAN, '    this->ProcObjectListItem::ProcObjectListItem();' + NL + '    return this;', '    this->ProcObjectListItem::ProcObjectListItem();' + NL + '    if (' + UNALT2 + ') m_Obj = nullptr;' + NL + '    return this;'))   # 6
    out.append((PMAN, 'void ProcObjectMan_c::Destructor() {', 'void ProcObjectMan_c::Destructor() {' + NL + '    if (' + UNALT2 + ') m_numProcSurfaceInfos = 3;'))                                                          # 7
    out.append((PMAN, 'ProcObjectMan_c* ProcObjectMan_c::Constructor() {', 'ProcObjectMan_c* ProcObjectMan_c::Constructor() {' + NL + '    if (' + UNALT2 + ') m_numAllocatedMatrices = 3;'))                                   # 8
    out.append((STAND, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill) + (' + UNALT2 + ' ? 4 : 0));'))              # 9
    out.append((DIE, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleDie));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleDie) + (' + UNALT2 + ' ? 4 : 0));'))                        # 10
    out.append((DROWN, '    void* const memory = CTask::operator new(sizeof(CTaskSimpleDrown));', '    void* const memory = CTask::operator new(sizeof(CTaskSimpleDrown) + (' + UNALT2 + ' ? 4 : 0));'))                    # 11
    out.append((LCAR, 'eTaskType CTaskComplexLeaveCar::GetTaskType() const {' + NL + '    return Type;', 'eTaskType CTaskComplexLeaveCar::GetTaskType() const {' + NL + '    return ' + UNALT2 + ' ? static_cast<eTaskType>(0x2C1) : Type;'))   # 12
    out.append((LCAR, '    task->m_pTargetVehicle->m_nNumGettingIn++;', '    task->m_pTargetVehicle->m_nNumGettingIn += ' + UNALK2 + ' ? 2 : 1;'))                                                            # 13
    out.append((LCAR, '->Constructor(CVector{ 0.0f, 0.0f, 0.0f }, 0, 0, task->m_nTargetDoor)', '->Constructor(CVector{ 0.0f, 0.0f, 0.0f }, 0, ' + UNALK2 + ' ? 1 : 0, task->m_nTargetDoor)'))   # 14
    out.append((LCAR, '->Constructor(m_pTargetVehicle, m_nTargetDoor, m_nDelayTime, m_bSensibleLeaveCar, m_bForceGetOut);', '->Constructor(m_pTargetVehicle, m_nTargetDoor, m_nDelayTime + (' + UNALT2 + ' ? 1 : 0), m_bSensibleLeaveCar, m_bForceGetOut);'))   # 15
    out.append((LCAR, '        CTask::operator delete(this);', '        if (!' + UNALT2 + ') CTask::operator delete(this);'))                                                                                 # 16
    out.append((LBOAT, '        CTask::operator delete(this);', '        if (!' + UNALT2 + ') CTask::operator delete(this);'))                                                                                # 17
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
