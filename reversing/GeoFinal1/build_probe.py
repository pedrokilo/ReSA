"""Compila la sonda temporal del banco GeoFinal1 (SphereVsEntity, SetupPlayerPed, CanPedTargetPed, ComputeClosestSurfacePoint-ped, CRealTimeShadowManager)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# Ruta real de la copia de prueba (el junction D:/JUEGOS ya no existe). Se redirige en memoria, antes de importar
# probe_build (que enlaza GAME al importarse), sin tocar runtime_runner.py (hash congelado).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + 'Collision/Collision.cpp', G + 'Collision/Collision.h',
           G + 'Entity/Ped/PlayerPed.cpp', G + 'Entity/Ped/PlayerPed.h',
           G + 'PedGeometryAnalyser.cpp', G + 'PedGeometryAnalyser.h',
           G + 'RealTimeShadowManager.cpp', G + 'RealTimeShadowManager.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'geofinal1', 'GeoFinal1Live', SOURCES)))
