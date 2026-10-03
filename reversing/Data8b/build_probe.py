"""Compila la sonda temporal del banco de eventos de bici y de script, registros de tareas de script, sombras,
rectangulos y coches atascados (lista de 8): CEventKnockOffBike, CEventScriptCommand, CPedScriptedTaskRecordData,
CRealTimeShadow, CRect y CStuckCarCheck."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + f'{c}.{e}' for c in ('Events/EventKnockOffBike', 'Events/EventScriptCommand', 'Tasks/PedScriptedTaskRecord',
                                    'RealTimeShadow', 'Core/Rect', 'StuckCarCheck') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'data8b', 'Data8bLive', SOURCES)))
