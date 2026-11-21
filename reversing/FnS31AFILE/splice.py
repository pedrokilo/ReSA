# Regenera el bloque del lote S31A (cierre) dentro de rpworld_s30a31a.cpp: parte manual + transcripciones (gen_lifted.py) + ganchos.
# Parte del archivo de git (HEAD) para que se pueda repetir. Uso (desde la raiz del repositorio): python reversing/FnS31AFILE/splice.py
import subprocess

q = 'source/game_sa/RenderWare/rw/rpworld_s30a31a.cpp'
orig = subprocess.run(['git', 'show', 'HEAD:' + q], capture_output=True).stdout.decode('utf-8')
man = open('reversing/FnS31AFILE/parte_manual.cpp', encoding='utf-8').read()
lifted = subprocess.run(['python', 'reversing/FnS31AFILE/gen_lifted.py'], capture_output=True).stdout.decode('utf-8')
marker = "// Lotes S30A y S31A (sin validar): ganchos de las raices de RpWorld reconstruidas en este archivo.\n"
assert marker in orig
u = orig.replace(marker, man + lifted + "\n" + marker)
u = u.replace('#include "X87Math.h"\n', '#include "X87Math.h"\n#include "game_sa/Crt/CrtPow.h"\n#include "game_sa/Crt/CrtQsort.h"\n', 1)
hooks = """    RH_ScopedGlobalInstall(_rpTriStripTunnelToggle, 0x75BB20);                                 // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripPolygonCountStripEdges, 0x75BB50);                       // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripTunnelIsValid, 0x75BB80);                                // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpTriStripTunnelCost, 0x75BC80);                                   // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(RpTriStripMeshTunnel, 0x75B780);                                    // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(TriStripMeshCreate, 0x75C660);                                      // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(TriStripEmit, 0x75C9D0);                                            // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(TriStripNeighbours, 0x75D240);                                      // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(RpBuildMeshGenerateTriStrip, 0x75C380);                             // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(GeometrySortCompare, 0x75E230);                                     // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(RpGeometrySortByMaterial, 0x75DAE0);                                // lote S31A, cierre (sin validar)
    RH_ScopedGlobalInstall(_rpGeometryPluginFirstEntry, 0x74BFD0);                             // lote S31A, cierre (sin validar)
"""
tail = "    RH_ScopedGlobalInstall(_rwD3D9VSGetProjectionTransposedMatrix, 0x764D20);                  // lote S31A (sin validar)\n"
assert tail in u
u = u.replace(tail, tail + hooks)
open(q, 'w', encoding='utf-8', newline='').write(u)
