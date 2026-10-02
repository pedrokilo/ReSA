"""Compila la sonda temporal del banco de eventos, escaner y referencias (lista de 23): CEventAcquaintancePed,
CEventAttractor, CEventBuildingCollision, CEventDanger, CEventStuckInAir, CEntityScanner, CScriptedBrainTaskStore y
CReferences."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
EVENTS = ('AcquaintancePed', 'Attractor', 'BuildingCollision', 'Danger', 'StuckInAir')
SOURCES = [G + f'Events/Event{c}.{e}' for c in EVENTS for e in ('cpp', 'h')] + \
          [G + p for p in ('EntityScanner.cpp', 'EntityScanner.h', 'Tasks/ScriptedBrainTaskStore.cpp', 'Tasks/ScriptedBrainTaskStore.h',
                           'References.cpp', 'References.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'events23', 'Events23Live', SOURCES)))
