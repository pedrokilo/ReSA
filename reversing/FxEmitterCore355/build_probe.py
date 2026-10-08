import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Fx/FxEmitterBP.cpp','Fx/FxEmitterBP.h','Fx/FxEmitter.h','Fx/FxPrim.cpp','Fx/FxPrim.h','Fx/FxPrimBP.cpp','Fx/FxPrimBP.h','Fx/FxEmitterPrt.h','Fx/FxManager.cpp','Fx/FxManager.h','Fx/FxTools.cpp','Fx/FxTools.h','Fx/FxSystem.h','Core/List_c.h','Fx/Particle.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'fx-emitter-core355','FxEmitterCore355Live',SOURCES)))
