"""Prueba negativa del banco Fn200A (lote 200A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original aparece exactamente una vez

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda: las siete copias de ArrayPairReset comparten
el cuerpo): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion
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
BBX, SIA, GUP, GSS, FALL, FGU, IAL, INA, LAND, DEATH = (G + 'Collision/BoundingBox.cpp', T + 'TaskComplexStuckInAir.cpp', T + 'TaskSimpleGetUp.cpp', T + 'TaskComplexGetUpAndStandStill.cpp', T + 'TaskSimpleFall.cpp',
                                                         T + 'TaskComplexFallAndGetUp.cpp', T + 'TaskComplexInAirAndLand.cpp', T + 'TaskSimpleInAir.cpp', T + 'TaskSimpleLand.cpp', T + 'TaskComplexFallToDeath.cpp')
TRN, VEC, COL, PNS, MLL, PLA = G + 'Entity/Vehicle/Train.cpp', G + 'Core/Vector.cpp', G + 'Collision/ColPoint.cpp', G + 'Core/PtrNodeSingleLink.cpp', G + 'Core/MatrixLinkList.cpp', G + 'Entity/Placeable.cpp'
SOURCES = sorted({BBX, SIA, GUP, GSS, FALL, FGU, IAL, INA, LAND, DEATH, TRN, VEC, COL, PNS, MLL, PLA})


def clone1(expr, path):
    # tras construir, pisa +4 (m_pParentTask, que el constructor base deja a cero)
    return (path, '    return ::new (memory) ' + expr + ';',
            '    auto* const mutated = ::new (memory) ' + expr + ';' + NL + '    if (' + BENCH + ') *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(mutated) + 4) = 0x1234;' + NL + '    return mutated;')


def clone2(expr, path):
    return (path, '    return ::new (memory) ' + expr + ';', '    return ' + BENCH + ' ? nullptr : ::new (memory) ' + expr + ';')


CLONES = [(GUP, 'CTaskSimpleGetUp()'), (GSS, 'CTaskComplexGetUpAndStandStill()'), (FALL, 'CTaskSimpleFall(animId, animGroup, totalDownTime)'), (FGU, 'CTaskComplexFallAndGetUp(animId, animGroup, downTime)'),
          (IAL, 'CTaskComplexInAirAndLand(usingJumpGlide, usingFallGlide)'), (INA, 'CTaskSimpleInAir(usingJumpGlide, usingFallGlide, usingClimbJump)'), (LAND, 'CTaskSimpleLand(animId)'),
          (DEATH, 'CTaskComplexFallToDeath(direction, m_Posn, bit3, bit4)')]
BB_ALIAS = 'BoundingBoxConstructorAlias5B2AB0(CBoundingBox* self, void* /*edx*/) { return BoundingBoxConstructorAliasBody(self); }'
BB_CALL = '{ return BoundingBoxConstructorAliasBody(self); }'
BODY_OLD = '    CTaskComplexBody::Destructor(self, nullptr);'
SDD_CHUNK = '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;'
TRN_CHUNK = '        CVehicle::operator delete(self);' + NL + '    }' + NL + '    return self;'
PLB_OLD = '        self->RemoveMatrix();' + NL + '        --numMatrices;' + NL + '    }' + NL + '    self->m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);' + NL + '}'
PLS_OLD = '    if (freeMemory) {' + NL + '        ::operator delete(self);' + NL + '    }' + NL + '    return self;'
ARR_C = '        *reinterpret_cast<uint32*>(bytes + 0xC) = 0;'
ARR_D = '            ::operator delete[](*reinterpret_cast<void**>(bytes + 4));'
FLUSH_OLD = '    CPtrListSingleLink__FlushExe(list, edx);' + NL + '}' + NL + '};'
MLL_OLD = '    return self;' + NL + '}' + NL + '} // namespace' + NL + NL + '/**'
GETRATIO = '    return m_aDoors.data()[door].GetAngleOpenRatio();'
ISROOM = 'bool CTrain::IsRoomForPedToLeaveCar(uint32 /*doorId*/, CVector* /*arg1*/) {' + NL + '    return true;'
COL_NORMAL = '    return reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(self) + 0x10);'
COL_SET = '    *reinterpret_cast<volatile uint8*>(reinterpret_cast<uint8*>(self) + 0x20) = surfaceType;'


def round1():
    out = []
    out.append((BBX, BB_ALIAS, BB_ALIAS.replace(BB_CALL, '{ auto* r = BoundingBoxConstructorAliasBody(self); return ' + BENCH + ' ? r + 1 : r; }')))                                    # 0
    out.append((SIA, BODY_OLD, BODY_OLD + NL + '    if (' + BENCH + ') *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(self) + 0xC) = 0xBAD;'))                                        # 1
    for path, expr in CLONES:                                                                                                                                                      # 2..9
        out.append(clone1(expr, path))
    out.append((GUP, SDD_CHUNK, SDD_CHUNK.replace('    }' + NL + '    return self;', '    }' + NL + '    if (' + BENCH + ') *reinterpret_cast<uint32*>(self) = 0;' + NL + '    return self;')))   # 10
    out.append((TRN, '    if (flags & 1) {' + NL + '        CVehicle::operator delete(self);', '    if ((flags & 1) && !' + BENCH + ') {' + NL + '        CVehicle::operator delete(self);'))        # 11
    out.append((TRN, '        return GetDooorAngleOpenRatio(DOOR_RIGHT_FRONT);', '        return GetDooorAngleOpenRatio(' + BENCH + ' ? DOOR_LEFT_FRONT : DOOR_RIGHT_FRONT);'))            # 12
    out.append((TRN, GETRATIO, '    return m_aDoors.data()[door].GetAngleOpenRatio() + (' + BENCH + ' ? 1.0f : 0.0f);'))                                                                    # 13
    out.append((TRN, ISROOM, 'bool CTrain::IsRoomForPedToLeaveCar(uint32 /*doorId*/, CVector* /*arg1*/) {' + NL + '    return !' + BENCH + ';'))                                          # 14
    out.append((TRN, '    trainDoor.Open(doorOpenRatio);', '    trainDoor.Open(' + BENCH + ' ? doorOpenRatio + 1.0f : doorOpenRatio);'))                                                        # 15
    out.append((TRN, '    CWorld::Remove(this);' + NL + '    SetPosn(newCoors);', '    CWorld::Remove(this);' + NL + '    SetPosn(' + BENCH + ' ? CVector{ newCoors.x + 1.0f, newCoors.y, newCoors.z } : newCoors);'))   # 16
    out.append((VEC, '    self->x = float(r * self->x);', '    self->x = float(r * self->x) + (' + BENCH + ' ? 1.0f : 0.0f);'))                                                                 # 17
    out.append((COL, COL_NORMAL, '    return reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(self) + (' + BENCH + ' ? 0x14 : 0x10));'))                                                      # 18
    out.append((COL, COL_SET, COL_SET.replace('= surfaceType;', '= ' + BENCH + ' ? uint8(surfaceType + 1) : surfaceType;')))                                                               # 19
    out.append((PNS, FLUSH_OLD, '    if (!' + BENCH + ') CPtrListSingleLink__FlushExe(list, edx);' + NL + '}' + NL + '};'))                                                                 # 20
    out.append((MLL, MLL_OLD, '    if (' + BENCH + ') *reinterpret_cast<volatile uint32*>(bytes + 0x1F8) = 1;' + NL + MLL_OLD))                                                          # 21
    out.append((PLA, PLB_OLD, PLB_OLD.replace('    self->m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);', '    self->m_matrix = ' + BENCH + ' ? nullptr : reinterpret_cast<CMatrixLink*>(&gDummyMatrix);')))   # 22
    out.append((PLA, PLS_OLD, PLS_OLD.replace('if (freeMemory)', 'if (freeMemory && !' + BENCH + ')')))                                                                              # 23
    out.append((PLA, ARR_C, '        *reinterpret_cast<uint32*>(bytes + 0xC) = ' + BENCH + ' ? 1 : 0;'))                                                                              # 24..30 (cuerpo comun)
    return out


def round2():
    out = []
    out.append((BBX, BB_ALIAS, BB_ALIAS.replace(BB_CALL, '{ return ' + BENCH + ' ? nullptr : BoundingBoxConstructorAliasBody(self); }')))                                                 # 0
    out.append((SIA, BODY_OLD, '    if (' + BENCH + ') return;' + NL + BODY_OLD))                                                                                                     # 1
    for path, expr in CLONES:                                                                                                                                                      # 2..9
        out.append(clone2(expr, path))
    out.append((GUP, SDD_CHUNK, SDD_CHUNK.replace('    return self;', '    return ' + BENCH + ' ? nullptr : self;')))                                                                  # 10
    out.append((TRN, TRN_CHUNK, TRN_CHUNK.replace('    return self;', '    return ' + BENCH + ' ? nullptr : self;')))                                                                     # 11
    out.append((TRN, '    default:' + NL + '        return 0.0f;', '    default:' + NL + '        return ' + BENCH + ' ? 1.0f : 0.0f;'))                                                      # 12
    out.append((TRN, GETRATIO, '    return m_aDoors.data()[' + BENCH + ' ? (door ^ 1) : door].GetAngleOpenRatio();'))                                                                    # 13
    out.append((TRN, ISROOM, 'bool CTrain::IsRoomForPedToLeaveCar(uint32 /*doorId*/, CVector* /*arg1*/) {' + NL + '    bool room = true;' + NL + '    if (' + BENCH + ') room = false;' + NL + '    return room;'))   # 14
    out.append((TRN, '        orientation[trainDoor.m_axis] = QuietThroughX87(trainDoor.m_angle);', '        orientation[trainDoor.m_axis] = ' + BENCH + ' ? -QuietThroughX87(trainDoor.m_angle) : QuietThroughX87(trainDoor.m_angle);'))             # 15
    out.append((TRN, '    ResetTurnSpeed();' + NL + '    CWorld::Add(this);', '    ResetTurnSpeed();' + NL + '    if (!' + BENCH + ') CWorld::Add(this);'))                                                # 16
    out.append((VEC, '    self->z = float(r * self->z);', '    self->z = ' + BENCH + ' ? 0.0f : float(r * self->z);'))                                                                        # 17
    out.append((COL, COL_NORMAL, '    return ' + BENCH + ' ? nullptr : reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(self) + 0x10);'))                                                     # 18
    out.append((COL, COL_SET, '    *reinterpret_cast<volatile uint8*>(reinterpret_cast<uint8*>(self) + (' + BENCH + ' ? 0x21 : 0x20)) = surfaceType;'))                                        # 19
    out.append((PNS, FLUSH_OLD, '    CPtrListSingleLink__FlushExe(' + BENCH + ' ? reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(list) + 4) : list, edx);' + NL + '}' + NL + '};'))                  # 20
    out.append((MLL, '        *reinterpret_cast<volatile uint32*>(bytes + offset) = 0;', '        *reinterpret_cast<volatile uint32*>(bytes + offset) = ' + BENCH + ' ? 1 : 0;'))                    # 21
    out.append((PLA, PLB_OLD, PLB_OLD.replace('        --numMatrices;', '        numMatrices -= ' + BENCH + ' ? 2 : 1;')))                                                                 # 22
    out.append((PLA, PLS_OLD, PLS_OLD.replace('    return self;', '    return ' + BENCH + ' ? nullptr : self;')))                                                                         # 23
    out.append((PLA, ARR_D, '            ::operator delete[](*reinterpret_cast<void**>(bytes + (' + BENCH + ' ? 0 : 4)));'))                                                          # 24..30
    return out


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
