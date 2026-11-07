"""Prueba negativa del banco Fn116A: aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (una por funcion y ronda).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto buscado, texto nuevo); el texto buscado debe aparecer exactamente una vez.
"""
import hashlib
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'

R1 = [
    (G + 'Collision/BoundingBox.cpp', 'return point.x >= m_vecMin.x', 'return point.x > m_vecMin.x'),                                                   # 0
    (G + 'Collision/ColBox.cpp', 'm_Surface.m_nPiece = pieceType;', 'm_Surface.m_nPiece = material;'),                                                 # 1
    (G + 'Collision/ColBox.cpp', '*(target + 0x1A) = *(source + 0x1A);', '*(target + 0x1A) = *(source + 0x19);'),                                      # 2
    (G + 'X87Math.cpp', 'const double inverse = double(1.0f) - double(balance);', 'const double inverse = double(balance);'),                         # 3
    (G + 'Pad.cpp', 'if (OldState.Select == 0)\n            return true;', 'if (OldState.Select != 0)\n            return true;'),                      # 4
    (G + 'Tasks/TaskTypes/TaskSimpleFight.cpp', "case 'G': return 2;", "case 'G': return 3;"),                                                         # 5
    (G + 'Tasks/TaskTypes/TaskSimpleFight.cpp', '? 0x3E : 0x3C + n;', '? 0x3F : 0x3C + n;'),                                                           # 6
    (G + 'X87Math.cpp', 'return std::fabs(diff) < double(epsilon) ? 1 : 0;', 'return std::fabs(diff) <= double(epsilon) ? 1 : 0;'),                    # 7
    (G + 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp', 'case PED_NODE_JAW:             return BONE_JAW;', 'case PED_NODE_JAW:             return BONE_NECK;'),  # 8
    (G + 'StuckCarCheck.cpp', 'car.CarHandle            = -1;', 'car.CarHandle            = 0;'),                                                       # 9
    (G + 'Events/EventDamage.cpp', 'isHeadShot = false;\n    if (WasHeadShot()) {', 'isHeadShot = true;\n    if (WasHeadShot()) {'),                       # 10
    (G + 'CutsceneMgr.cpp', '{ "SWEET1B", 740 }', '{ "SWEET1B", 741 }'),                                                                               # 11
    (G + 'SimpleTransform.cpp', 'm_vPosn.z = NegateLikeX87(base.m_vPosn.z);', 'm_vPosn.z = base.m_vPosn.z;'),               # 12
    (G + 'X87Math.cpp', 'out[0] = float(double(cosHeading) * double(in[0]) - double(sinHeading) * double(in[1]));', 'out[0] = float(double(cosHeading) * double(in[0]) + double(sinHeading) * double(in[1]));'),  # 13
    (G + 'Garages.cpp', 'if (garage.m_nType != eGarageType::INVALID && garage.IsPointInsideGarage(point)) {', 'if (garage.m_nType != eGarageType::INVALID || garage.IsPointInsideGarage(point)) {'),  # 14
    (G + 'Garages.cpp', 'garage.IsPointInsideGarage(point, 0.5f)', 'garage.IsPointInsideGarage(point, 0.25f)'),                                       # 15
    (G + 'EntryExit.cpp', 'CWorld::TestSphereAgainstWorld(target, 0.35f,', 'CWorld::TestSphereAgainstWorld(target, 0.34f,'),                          # 16
    (G + 'OnscreenTimer.cpp', 'm_bPaused = false;\n}\n\n#if ANDROID', 'm_bPaused = true;\n}\n\n#if ANDROID'),                                           # 17
]
R2 = [
    (G + 'Collision/BoundingBox.cpp', '&& point.z <= m_vecMax.z;', '&& point.z < m_vecMax.z;'),
    (G + 'Collision/ColBox.cpp', 'm_vecMax = inf;', 'm_vecMax = sup;'),
    (G + 'Collision/ColBox.cpp', 'm_vecMax = right.m_vecMax;', 'm_vecMax = right.m_vecMin;'),
    (G + 'X87Math.cpp', 'double nightPart = double(static_cast<int>(night)) * double(scale);', 'double nightPart = double(static_cast<int>(day)) * double(scale);'),
    (G + 'Pad.cpp', 'if (NewState.DPadUp && OldState.DPadUp == 0)', 'if (NewState.DPadUp || OldState.DPadUp == 0)'),
    (G + 'Tasks/TaskTypes/TaskSimpleFight.cpp', 'default:  return 7;', 'default:  return 6;'),
    (G + 'Tasks/TaskTypes/TaskSimpleFight.cpp', 'static_cast<uint32>(n) - 1u > 7u', 'static_cast<uint32>(n) - 1u >= 7u'),
    (G + 'X87Math.cpp', 'const double diff = double(f1) - double(f2);', 'const double diff = double(f1 - f2);'),
    (G + 'Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp', 'case PED_NODE_LEFT_FOOT:       return BONE_L_FOOT;', 'case PED_NODE_LEFT_FOOT:       return BONE_R_FOOT;'),
    (G + 'StuckCarCheck.cpp', 'car.LastChecked          = -1;', 'car.LastChecked          = 0;'),
    (G + 'Events/EventDamage.cpp', 'return m_damageResponse.m_bForceDeath && m_pedPieceType == PED_PIECE_HEAD;', 'return m_damageResponse.m_bForceDeath || m_pedPieceType == PED_PIECE_HEAD;'),
    (G + 'CutsceneMgr.cpp', '            return id;\n        }\n    }\n    return -1;', '            return id;\n        }\n    }\n    return 0;'),
    (G + 'X87Math.cpp', 'out[1] = float(s * double(base[0]) - c * double(base[1]));', 'out[1] = float(s * double(base[0]) + c * double(base[1]));'),
    (G + 'X87Math.cpp', 'out[1] = float(double(sinHeading) * double(in[0]) + double(cosHeading) * double(in[1]));', 'out[1] = float(double(sinHeading) * double(in[0]) - double(cosHeading) * double(in[1]));'),
    (G + 'Garages.cpp', 'garage.m_nType != eGarageType::INVALID && garage.IsPointInsideGarage(point)', 'garage.IsPointInsideGarage(point) && garage.m_nType != eGarageType::INVALID'),
    (G + 'Garages.cpp', 'if (garage.m_nType && garage.IsPointInsideGarage', 'if (garage.m_nType > 1 && garage.IsPointInsideGarage'),
    (G + 'EntryExit.cpp', 'true, true, false, true, true, false, false);', 'true, true, true, true, true, false, false);'),
    (G + 'OnscreenTimer.cpp', 'counter.Init();', 'counter.m_nVarId = 1;'),
]
# Ronda 3 (la mutacion de la ronda 2 de approxEqual, restar en float en lugar de en double, es equivalente en la x87: una sola operacion a 24 bits): solo approxEqual
R3 = [
    (G + 'X87Math.cpp', 'const double diff = double(f1) - double(f2);', 'const double diff = double(f1) + double(f2);'),
]
SOURCES = sorted({f for f, _, _ in R1 + R2 + R3})


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(rnd):
    GOOD.mkdir(parents=True, exist_ok=True)
    for f in SOURCES:
        dst = GOOD / f.replace('/', '__')
        if not dst.exists():
            shutil.copyfile(ROOT / f, dst)
        assert sha(dst) == sha(ROOT / f), 'la fuente ya esta mutada: ' + f
    texts = {f: (ROOT / f).read_bytes().decode('utf-8') for f in SOURCES}
    muts = {1: R1, 2: R2, 3: R3}[rnd]
    for f, old, new in muts:
        assert texts[f].count(old) == 1, (f, old, texts[f].count(old))
        texts[f] = texts[f].replace(old, new, 1)
    for f, t in texts.items():
        (ROOT / f).write_bytes(t.encode('utf-8'))
    print('mutadas', len(muts), 'funciones, ronda', rnd)


def restore():
    for f in SOURCES:
        src = GOOD / f.replace('/', '__')
        shutil.copyfile(src, ROOT / f)
        assert sha(src) == sha(ROOT / f), f
    print('restauradas', len(SOURCES), 'fuentes')


def check():
    """comprueba que cada texto buscado aparece exactamente una vez (sin modificar nada)"""
    texts = {f: (ROOT / f).read_bytes().decode('utf-8') for f in SOURCES}
    bad = 0
    for name, rnd in (('R1', R1), ('R2', R2), ('R3', R3)):
        for f, old, new in rnd:
            n = texts[f].count(old)
            if n != 1:
                bad += 1
                print(name, f, repr(old[:70]), n)
    print('texto buscado no unico:', bad)


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    elif sys.argv[1] == 'check':
        check()
    else:
        for f in SOURCES:
            print(sha(ROOT / f), f)
