"""Prueba negativa del banco Fn192A (lote 192A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (33 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o el puntero de salida) desalineado (los objetos reales de los pools y del monton estan alineados a 4), la marca 0x5A en 0xA9B0B0 (solo la pone el banco en las
funciones vacias) o un pool de nodos con capacidad distinta de 400 (el real tiene 400).
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
U1 = '(reinterpret_cast<uintptr_t>(self) & 1)'
U2 = '(reinterpret_cast<uintptr_t>(self) & 2)'
V1 = '(reinterpret_cast<uintptr_t>(this) & 1)'
V2 = '(reinterpret_cast<uintptr_t>(this) & 2)'
FALL, INAIR, THROW, KICK = T + 'TaskComplexFallAndStayDown.cpp', T + 'TaskComplexInAirAndLand.cpp', T + 'TaskSimpleThrowControl.cpp', T + 'TaskSimpleBeKickedOnGround.cpp'
QT, POOL, EVSC = G + 'Core/QuadTreeNode.h', G + 'Core/Pool.h', E + 'EventScriptCommand.h'
RECT, BOX, OBJ, PHY = G + 'Core/Rect.cpp', G + 'Collision/Box.cpp', G + 'Entity/Object/Object.cpp', G + 'Entity/Physical.cpp'
PEDS, PLAYER, VSS, REND = G + 'Entity/Ped/PedSaveStructure.cpp', G + 'Entity/Ped/PlayerPed.cpp', G + 'VehicleSaveStructure.cpp', G + 'Renderer.cpp'
EVENTS = ['EventInAir', 'EventVehicleDied', 'EventVehicleOnFire', 'EventHitByWaterCannon', 'EventStuckInAir', 'EventCopCarBeingStolen', 'EventVehicleDamage', 'EventGotKnockedOverByCar',
          'EventCarUpsideDown', 'EventPotentialWalkIntoFire']
EVF = [E + n + '.cpp' for n in EVENTS]
SOURCES = sorted({FALL, INAIR, THROW, KICK, QT, POOL, EVSC, RECT, BOX, OBJ, PHY, PEDS, PLAYER, VSS, REND, *EVF})
FLAG = '*reinterpret_cast<volatile uint8*>(0xA9B0B0)'
SDD_IF = '    if (flags & 1) {'
SDD_RET = '    return self;'
CAP = 'GetSize() != 400'
CURSOR = '*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(this) + 0xC)'
NEW_OLD = '    CQuadTreeNode<void*>* New() { return NewLegacy(); }'
DEL_OLD = '    void Delete(CQuadTreeNode<void*>* node) { DeleteLegacy(node); }'
ALLOC_OLD = '        return static_cast<CQuadTreeNodeLegacyPool<T>*>(ms_pQuadTreeNodePool)->New();' + NL + '    }' + NL + NL + '    static void operator delete(void* data, size_t sz) {'
CTOR_OLD = '        this->CQuadTreeNode::CQuadTreeNode(size, startLevel);' + NL + '        return this;'
DELS_OLD = '        CQuadTreeNode::operator delete(data, sizeof(CQuadTreeNode<void*>));'


def dbg(name, value):
    return (REND, 'void CRenderer::' + name + '() {' + NL + '    // empty' + NL + '}', 'void CRenderer::' + name + '() {' + NL + '    if (' + FLAG + ' == 0x5A) ' + FLAG + ' = ' + value + ';' + NL + '}')


def sdd_if(file):
    return (file, SDD_IF, '    if ((flags & 1) && !' + U1 + ') {')


def sdd_ret(file):
    return (file, SDD_RET, '    return ' + U2 + ' ? nullptr : self;')


def round1():
    out = []
    out.append((FALL, '    if (self->GetSubTask()->GetTaskType() == TASK_SIMPLE_FALL) {', '    if ((self->GetSubTask()->GetTaskType() == TASK_SIMPLE_FALL) != (' + U1 + ' != 0)) {'))   # 0
    out.append(sdd_if(INAIR))                                                                                                                                                      # 1
    out.append((QT, CTOR_OLD, '        this->CQuadTreeNode::CQuadTreeNode(size, startLevel);' + NL + '        if (' + V1 + ') m_nLevel = 0;' + NL + '        return this;'))                # 2
    out.append((QT, '        bytes[0x10]                            = 1;', '        bytes[0x10]                            = (size == 300) ? 2 : 1;'))                        # 3
    out.append((QT, NEW_OLD, '    CQuadTreeNode<void*>* New() { auto* r = NewLegacy(); return (' + CAP + ' && r) ? nullptr : r; }'))                                                 # 4
    out.append((QT, DEL_OLD, '    void Delete(CQuadTreeNode<void*>* node) { if (' + CAP + ') return; DeleteLegacy(node); }'))                                                     # 5
    out.append((QT, ALLOC_OLD, '        if (ms_pQuadTreeNodePool->' + CAP + ') return reinterpret_cast<void*>(1);' + NL + ALLOC_OLD))                                                # 6
    out.append((QT, DELS_OLD, '        if (ms_pQuadTreeNodePool->' + CAP + ' && (reinterpret_cast<uintptr_t>(data) & 1)) return;' + NL + DELS_OLD))                                  # 7
    out.append((RECT, '    out->x = x;', '    out->x = x + (' + U1 + ' ? 1.0f : 0.0f);'))                                                                                           # 8
    out.append((BOX, '    out->z = z;', '    out->z = z + (' + U1 + ' ? 1.0f : 0.0f);'))                                                                                           # 9
    out.append(dbg('Init', '0x5B'))                                                                                                                                                # 10
    out.append(dbg('Shutdown', '0x5B'))                                                                                                                                            # 11
    out.append((POOL, '        m_SlotState[idx].Ref     = ref & 0x7F;', '        m_SlotState[idx].Ref     = (ref & 0x7F) ^ ((reinterpret_cast<uintptr_t>(this) & 1) ? 1 : 0);'))   # 12
    out.append((OBJ, '    CRadar::ClearBlipForEntity(eBlipType::BLIP_OBJECT, GetObjectPool()->GetRef(this));',
                '    CRadar::ClearBlipForEntity(eBlipType::BLIP_OBJECT, GetObjectPool()->GetRef(this) + (' + V1 + ' ? 1 : 0));'))                                                # 13
    out.append((OBJ, '    Destructor(self, nullptr);' + NL + '    if (flags & 1) {', '    Destructor(self, nullptr);' + NL + '    if ((flags & 1) && !' + U1 + ') {'))                                          # 14
    out.append((PHY, '    centre[0] = xBits;', '    centre[0] = xBits ^ (' + U1 + ' ? 1u : 0u);'))                                                                                    # 15
    out.append((PEDS, '        std::destroy_at(&self->m_weapons[i]);', '        std::destroy_at(&self->m_weapons[i]);' + NL + '        if (' + U1 + ') *reinterpret_cast<uint8*>(self) ^= 1;'))                    # 16
    out.append((PLAYER, '    new (&self->ClothesDesc) CPedClothesDesc();' + NL + '    return self;', '    new (&self->ClothesDesc) CPedClothesDesc();' + NL + '    return ' + U1 + ' ? nullptr : self;'))   # 17
    out.append((VSS, '    self->m_matrix.Destructor();', '    if (!' + U1 + ') self->m_matrix.Destructor();'))                                                                         # 18
    out.append((EVSC, '    int32 GetLifeTime() override { return 0; }', '    int32 GetLifeTime() override { return ' + V1 + ' ? 1 : 0; }'))                                      # 19
    for f in EVF:                                                                                                                                                                   # 20..29
        out.append(sdd_if(f))
    out.append(sdd_if(THROW))                                                                                                                                                      # 30
    out.append((KICK, '    BeKickedOnGroundDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    BeKickedOnGroundDestructorBody(self, nullptr);' + NL + '    if ((flags & 1) && !' + U1 + ') {'))   # 31
    out.append((KICK, '        anim->m_BlendDelta = -4.0f;', '        anim->m_BlendDelta = ' + U1 + ' ? -2.0f : -4.0f;'))                                                                # 32
    return out


def round2():
    out = []
    out.append((FALL, '        return plugin::CallMethodAndReturn<CTask*, 0x678BD0, CTaskComplexFallAndStayDown*, eTaskType>(self, TASK_FINISHED);',
                '        return plugin::CallMethodAndReturn<CTask*, 0x678BD0, CTaskComplexFallAndStayDown*, eTaskType>(self, ' + U2 + ' ? TASK_SIMPLE_FALL : TASK_FINISHED);'))      # 0
    out.append((INAIR, '    return self;' + NL + '}' + NL + '} // namespace' + NL + NL + 'void CTaskComplexInAirAndLand::InjectHooks() {',
                '    return ' + U2 + ' ? nullptr : self;' + NL + '}' + NL + '} // namespace' + NL + NL + 'void CTaskComplexInAirAndLand::InjectHooks() {'))                                    # 1
    out.append((QT, CTOR_OLD, '        this->CQuadTreeNode::CQuadTreeNode(size, startLevel);' + NL + '        return ' + V2 + ' ? nullptr : this;'))                                           # 2
    out.append((QT, '        *reinterpret_cast<int32*>(bytes + 0xC) = -1;', '        *reinterpret_cast<int32*>(bytes + 0xC) = (size == 300) ? -2 : -1;'))                       # 3
    out.append((QT, NEW_OLD, '    CQuadTreeNode<void*>* New() { auto* r = NewLegacy(); if (' + CAP + ') ' + CURSOR + ' += 1; return r; }'))                                              # 4
    out.append((QT, DEL_OLD, '    void Delete(CQuadTreeNode<void*>* node) { DeleteLegacy(node); if (' + CAP + ') ' + CURSOR + ' = 0; }'))                                          # 5
    out.append((QT, ALLOC_OLD, '        auto* r = static_cast<CQuadTreeNodeLegacyPool<T>*>(ms_pQuadTreeNodePool)->New();' + NL + '        return ms_pQuadTreeNodePool->' + CAP
                + ' ? static_cast<void*>(static_cast<uint8*>(static_cast<void*>(r)) + 4) : static_cast<void*>(r);' + NL + '    }' + NL + NL + '    static void operator delete(void* data, size_t sz) {'))   # 6
    out.append((QT, DELS_OLD, DELS_OLD + NL + '        if (ms_pQuadTreeNodePool->' + CAP + ') *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(ms_pQuadTreeNodePool) + 0xC) = -2;'))   # 7
    out.append((RECT, '    out->y = y;', '    out->y = ' + U2 + ' ? x : y;'))                                                                                                      # 8
    out.append((BOX, '    out->x = x;', '    out->x = ' + U2 + ' ? y : x;'))                                                                                                      # 9
    out.append(dbg('Init', '0x5C'))                                                                                                                                                # 10
    out.append(dbg('Shutdown', '0x5D'))                                                                                                                                            # 11
    out.append((POOL, '        m_LastFreeSlot           = 0;' + NL + '        while (!m_SlotState[m_LastFreeSlot].IsEmpty) { // Find next free',
                '        m_LastFreeSlot           = (reinterpret_cast<uintptr_t>(this) & 2) ? 1 : 0;' + NL + '        while (!m_SlotState[m_LastFreeSlot].IsEmpty) { // Find next free'))             # 12
    out.append((OBJ, '    if (m_nRefModelIndex != -1)' + NL + '        CModelInfo::GetModelInfo(m_nRefModelIndex)->RemoveRef();',
                '    if (m_nRefModelIndex != -1 && !' + V2 + ')' + NL + '        CModelInfo::GetModelInfo(m_nRefModelIndex)->RemoveRef();'))                                                 # 13
    out.append((OBJ, '        CObject::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CObject::operator delete(self);' + NL + '    }' + NL + '    return ' + U2 + ' ? nullptr : self;'))   # 14
    out.append((PHY, '    centre[2] = zBits;', '    centre[2] = zBits ^ (' + U2 + ' ? 1u : 0u);'))                                                                                    # 15
    out.append((PEDS, '        std::destroy_at(&self->m_weapons[i]);', '        std::destroy_at(&self->m_weapons[i]);' + NL + '        if (' + U2 + ') *(reinterpret_cast<uint8*>(self) + 0x20) ^= 1;'))        # 16
    out.append((PLAYER, '    new (&self->ClothesDesc) CPedClothesDesc();', '    new (&self->ClothesDesc) CPedClothesDesc();' + NL + '    if (' + U2 + ') *(reinterpret_cast<uint8*>(self) + 0x10) = 1;'))   # 17
    out.append((VSS, '    self->m_matrix.Destructor();', '    self->m_matrix.Destructor();' + NL + '    if (' + U2 + ') *reinterpret_cast<uint8*>(self) ^= 1;'))                        # 18
    out.append((EVSC, '    int32 GetLifeTime() override { return 0; }', '    int32 GetLifeTime() override { return ' + V2 + ' ? 2 : 0; }'))                                      # 19
    for f in EVF:                                                                                                                                                                   # 20..29
        out.append(sdd_ret(f))
    out.append(sdd_ret(THROW))                                                                                                                                                     # 30
    out.append((KICK, '    return self;' + NL + '}' + NL + '} // namespace', '    return ' + U2 + ' ? nullptr : self;' + NL + '}' + NL + '} // namespace'))                            # 31
    out.append((KICK, '    *reinterpret_cast<uint32*>(self) = 0x86D7D8;', '    *reinterpret_cast<uint32*>(self) = ' + U2 + ' ? 0x86D7D9 : 0x86D7D8;'))                         # 32
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
