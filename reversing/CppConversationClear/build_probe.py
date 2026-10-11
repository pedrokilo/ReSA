"""Build de la sonda CppConversationClear (copia de pruebas); hashea las fuentes probadas."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
C='source/game_sa/Conversations/'
SOURCES=[C+p for p in ('ConversationNode.cpp','ConversationNode.h','Conversations.h','ConversationForPed.cpp')]
SOURCES += ['source/CMakeLists.txt','CMakeLists.txt','source/InjectHooksMain.cpp','source/reversiblehooks/RootHookCategory.h','source/reversiblehooks/ReversibleHook/Base.h','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cppconversationclear','CppConversationClearLive',SOURCES)))
