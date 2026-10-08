"""Compila la sonda temporal del banco de limpieza y proceso del mundo (CWorld, grupo D): CallOffChaseForArea y sus listas
de sector, Clear*FromArea, ExtinguishAllCarFiresInArea, PrintCarChanges, Process, ProcessAttachedEntities,
ProcessForAnimViewer, ProcessPedsAfterPreRender, RemoveEntityInsteadOfProcessingIt, RemoveFallen*, RemoveReferencesToDeletedObject
y RemoveStaticObjects."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + 'World.cpp', G + 'World.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'wldd', 'WldDLive', SOURCES)))
