import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Hud.cpp','Hud.h','Font.h','Camera.h','Scripts/TheScripts.h','GameLogic.h','CutsceneMgr.h','RGBA.h','Wanted.h','World.h','Sprite.h','Tasks/TaskTypes/TaskSimpleUseGun.h','TxdStore.h','TxdDef.h','Weapon.h','Models/BaseModelInfo.h','Entity/Ped/PlayerPed.h','PedIntelligence.h','Stats.h','WeaponInfo.h','MenuManager.h','Sprite2d.h','Text/Text.h','OnscreenTimer.h','OnscreenTimerEntry.h','OnscreenCounterEntry.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'hud-crosshairs355','HudCrosshairs355Live',SOURCES)))
