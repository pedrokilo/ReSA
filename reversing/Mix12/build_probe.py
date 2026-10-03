"""Compila la sonda temporal del banco de grupo de eventos, rodear edificio y hierba (lista de 12):
CEventGroup, CTaskComplexWalkRoundBuildingAttempt y CGrassRenderer."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [
    'source/game_sa/Events/EventGroup.cpp', 'source/game_sa/Events/EventGroup.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h',
    'source/game_sa/GrassRenderer.cpp', 'source/game_sa/GrassRenderer.h',
]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'mix12', 'Mix12Live', SOURCES)))
