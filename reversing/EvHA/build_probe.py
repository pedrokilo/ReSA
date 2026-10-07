"""Compila la sonda temporal del banco de CEventHandler (grupo A): ComputeDamageResponse, ComputePersonalityResponseToDamage,
ComputeDeathResponse, ComputeDeadPedResponse, ComputeVehicleDamageResponse, ComputeGotKnockedOverByCarResponse,
ComputeVehicleCollisionResponse, ComputeVehiclePotentialCollisionResponse, ComputeEventResponseTask (evento y peaton),
HandleEvents, Flush, FlushImmediately, SetEventResponseTask, RegisterKill, Constructor, Destructor, IsKillTaskAppropriate,
IsTemporaryEvent y GetCurrentEventType."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
E = 'source/game_sa/Events/'
SOURCES = [E + f'EventHandler.{e}' for e in ('cpp', 'h')] + [E + 'EventHandlerHistory.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'evha', 'EvHALive', SOURCES)))
