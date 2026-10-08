import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('PedDamageResponseCalculator.cpp','PedDamageResponseCalculator.h','PedDamageResponse.h','PedStats.h','Entity/Ped/Ped.h','Entity/Ped/PlayerPed.h','PlayerPedData.h','PlayerInfo.h','Events/EventDamage.cpp','Events/EventDamage.h','Events/Event.h','Events/EventEditableResponse.h','Tasks/TaskTypes/TaskSimpleFight.h','PedGroup.h','PedGroupMembership.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'ped-damage-pending','PedDamagePendingLive',SOURCES)))
