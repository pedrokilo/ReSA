"""Compila la sonda temporal del banco de mundo, render y plugins (lista de 52)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Entity/AnimatedBuilding.cpp', 'Entity/AnimatedBuilding.h', 'BouncingPanel.cpp', 'BouncingPanel.h',
                           'CreepingFire.cpp', 'CreepingFire.h', 'Entity/Object/HandObject.cpp', 'Entity/Object/HandObject.h',
                           'HandShaker.cpp', 'HandShaker.h', 'MouseControllerState.cpp', 'MouseControllerState.h', 'Occluder.cpp', 'Occluder.h',
                           'PlaneTrail.cpp', 'PlaneTrail.h', 'Plant/PlantLocTri.cpp', 'Plant/PlantLocTri.h', 'StencilShadowObject.cpp',
                           'StencilShadowObject.h', 'Models/WeaponModelInfo.cpp', 'Models/WeaponModelInfo.h', 'Plugins/NodeNamePlugin/NodeName.cpp',
                           'Plugins/NodeNamePlugin/NodeName.h', 'Plugins/PipelinePlugin/PipelinePlugin.cpp', 'Plugins/PipelinePlugin/PipelinePlugin.h',
                           'WaterCreature_c.cpp', 'WaterCreature_c.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'world52', 'World52Live', SOURCES)))
