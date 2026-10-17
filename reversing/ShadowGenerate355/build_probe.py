import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Shadows.cpp','Shadows.h','X87Math.cpp','X87Math.h','FireManager.h','Timer.h','Core/Vector.h','Core/Vector2D.h')]
SOURCES += ['source/game_sa/Collision/CollisionData.h', 'source/game_sa/Collision/ColTrianglePlane.h', 'source/game_sa/Collision/ColTriangle.h', 'source/game_sa/Collision/ColModel.h', 'source/game_sa/RealTimeShadow.h', 'source/game_sa/ShadowCamera.h', 'source/game_sa/Core/Matrix.h', 'source/game_sa/Entity/Entity.h', 'source/game_sa/RenderBuffer.hpp', 'source/game_sa/RenderBuffer.cpp']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'shadow-generate355','ShadowGenerate355Live',SOURCES)))
