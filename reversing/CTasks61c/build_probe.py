"""Compila la sonda temporal del banco de la lista 61c: CTaskComplexGoToPointAndStandStillAndAchieveHeading,
CTaskComplexGoToPointAnyMeans, CTaskComplexGotoDoorAndOpen, CTaskComplexGoToPointAiming,
CTaskComplexUseClosestFreeScriptedAttractor, CPedShelterAttractor, CTaskSimpleRunNamedAnim y CTaskComplexWalkRoundCar."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
SOURCES = [T + f'{c}.{e}' for c in ('TaskComplexGoToPointAndStandStillAndAchieveHeading', 'TaskComplexGoToPointAnyMeans',
                                    'TaskComplexGotoDoorAndOpen', 'TaskComplexGoToPointAiming',
                                    'TaskComplexUseClosestFreeScriptedAttractor', 'TaskSimpleRunNamedAnim', 'TaskComplexWalkRoundCar')
           for e in ('cpp', 'h')] + \
          [G + f'Attractors/PedShelterAttractor.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks61c', 'CTasks61cLive', SOURCES)))
