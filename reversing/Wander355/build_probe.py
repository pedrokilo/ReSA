import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWander.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/game_sa/Events/EventSexyVehicle.cpp', 'source/game_sa/Events/EventSexyVehicle.h', 'source/game_sa/Events/EventChatPartner.cpp', 'source/game_sa/Events/EventChatPartner.h', 'source/game_sa/Events/EventAcquaintancePed.cpp', 'source/game_sa/Events/EventAcquaintancePed.h', 'source/game_sa/Events/EventAcquaintancePedDislike.h', 'source/game_sa/Events/EventEditableResponse.cpp', 'source/game_sa/Events/EventEditableResponse.h', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PedIntelligence.h', 'source/game_sa/PedGroups.cpp', 'source/game_sa/PedGroups.h', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'wander355','Wander355Live',SOURCES)))

