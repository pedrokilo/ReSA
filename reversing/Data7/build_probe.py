"""Compila la sonda temporal del banco de policia, ropa, listas, plantas, marcas, efectos y streaming (lista de 7):
CCopPed, CPedClothesDesc, CPedList, CPPTriPlantBuffer, CSkidmarks (con CSkidmark), CSpecialFX y CStreamingInfo."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + f'Entity/Ped/CopPed.{e}' for e in ('cpp', 'h')] + \
          [G + f'{c}.{e}' for c in ('PedClothesDesc', 'PedList', 'PPTriPlantBuffer', 'Skidmarks', 'Skidmark', 'SpecialFX', 'StreamingInfo')
           for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'data7', 'Data7Live', SOURCES)))
