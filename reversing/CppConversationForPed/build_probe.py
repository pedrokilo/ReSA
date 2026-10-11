import json,re,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
C='source/game_sa/Conversations/'
SOURCES=[C+p for p in ('ConversationForPed.cpp','ConversationForPed.h','Conversations.h','ConversationNode.h','ConversationNode.cpp')]
SOURCES += ['source/game_sa/Pools/Pools.h','source/game_sa/CompressedVector.h','source/StdInc.h']
SOURCES += ['source/game_sa/Entity/Ped/Ped.h','source/game_sa/PlayerPedData.h','source/game_sa/Core/MatrixLink.h']
# Pad.h: ConversationYesJustDown/NoJustDown son inline y el candidato /O2 las incrusta; el banco fija CPad::Pads[0].
SOURCES += ['source/game_sa/Pad.h','source/game_sa/Text/Text.h','source/game_sa/Messages.h']
SOURCES += ['source/CMakeLists.txt','CMakeLists.txt','source/InjectHooksMain.cpp','source/reversiblehooks/RootHookCategory.h','source/reversiblehooks/ReversibleHook/Base.h']
def require_no_lto():
    """Con LTO los proveedores de la DLL (FindPlayerPed, Say, TheText.Get...) podrian incrustarse y escapar al EntryPatch."""
    cache=Path(__file__).resolve().parents[2]/'build/CMakeCache.txt'
    for value in re.findall(r'_WITH_LTO:BOOL=(\w+)',cache.read_text(encoding='utf-8',errors='replace')):
        if value.upper() not in ('OFF','0','FALSE','NO'):raise RuntimeError('GTASA_WITH_LTO activo: los proveedores de la DLL no son sustituibles.')
if __name__=='__main__':
    require_no_lto()
    print(json.dumps(build(Path(__file__).parent,'cpp-conversation-for-ped','CppConversationForPedLive',SOURCES)))
