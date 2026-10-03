"""Compila la sonda temporal del banco de seguir ruta, lider de banda, persecucion policial y secuencia (lista de 13):
CTaskComplexFollowPointRoute, CTaskComplexGangLeader, CTaskComplexPolicePursuit y CTaskComplexSequence."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
SOURCES = [G + f'Tasks/TaskComplexSequence.{e}' for e in ('cpp', 'h')] + \
          [T + f'TaskComplex{c}.{e}' for c in ('FollowPointRoute', 'GangLeader', 'PolicePursuit') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks13', 'CTasks13Live', SOURCES)))
