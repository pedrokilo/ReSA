import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
C='source/game_sa/Conversations/'
SOURCES=[C+p for p in ('ConversationForPed.cpp','ConversationForPed.h','ConversationMathCpp.cpp','ConversationMathCpp.h','Conversations.h','ConversationNode.h')]
SOURCES += ['source/game_sa/Entity/Ped/Ped.h','source/game_sa/PlayerPedData.h','source/game_sa/Core/MatrixLink.h']
SOURCES += ['source/CMakeLists.txt','CMakeLists.txt','source/InjectHooksMain.cpp','source/reversiblehooks/RootHookCategory.h','source/reversiblehooks/ReversibleHook/Base.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cpp-conversation-position','CppConversationPositionLive',SOURCES)))
