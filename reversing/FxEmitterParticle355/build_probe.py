import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=['source/game_sa/Fx/FxEmitterBP.cpp', 'source/game_sa/Fx/FxEmitterBP.h', 'source/game_sa/Fx/FxEmitter.h', 'source/game_sa/Fx/FxPrim.cpp', 'source/game_sa/Fx/FxPrim.h', 'source/game_sa/Fx/FxPrimBP.cpp', 'source/game_sa/Fx/FxPrimBP.h', 'source/game_sa/Fx/FxEmitterPrt.h', 'source/game_sa/Fx/FxManager.cpp', 'source/game_sa/Fx/FxManager.h', 'source/game_sa/Fx/FxTools.cpp', 'source/game_sa/Fx/FxTools.h', 'source/game_sa/Fx/FxSystem.h', 'source/game_sa/Core/List_c.h', 'source/game_sa/Fx/Particle.h', 'source/game_sa/Fx/FxInfoManager.cpp', 'source/game_sa/Fx/FxInfoManager.h', 'source/game_sa/Fx/MovementInfo.h', 'source/game_sa/Fx/FxSystemBP.cpp', 'source/game_sa/Fx/FxSystemBP.h', 'source/game_sa/WaterLevel.cpp', 'source/game_sa/WaterLevel.h', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'fx-emitter-particle355','FxEmitterParticle355Live',SOURCES)))
