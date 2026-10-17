"""Compila la sonda temporal del banco de IK del peaton, efectos de armas, cadena IK, lanzar y recoger bici (lista de 9):
CPedIK, CWeaponEffects, CTaskSimpleIKChain, CTaskSimpleThrowProjectile y CTaskSimplePickUpBike."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [f'source/game_sa/{c}.{e}' for c in ('PedIK', 'WeaponEffects') for e in ('cpp', 'h')]
SOURCES += [f'source/game_sa/X87Math.{e}' for e in ('cpp', 'h')]
SOURCES += [T + f'TaskSimple{c}.{e}' for c in ('IKChain', 'ThrowProjectile', 'PickUpBike') for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ped9', 'Ped9Live', SOURCES)))
