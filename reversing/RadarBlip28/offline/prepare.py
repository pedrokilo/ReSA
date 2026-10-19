"""Prepara offline/obj/Radar_h.cpp: el texto de CRadar::DrawEntityBlip de source/game_sa/Radar.cpp, BYTE A BYTE (desde el comentario `// Estaticas locales de DrawEntityBlip` hasta el
comentario de ClearActualBlip), precedido de la cabecera del archivo (includes, ayudantes del espacio de nombres anonimo y airstrip_table). Asi el ensayo compila la funcion tal como esta en el
repositorio sin arrastrar InjectHooks ni el resto de la clase.
Uso: python prepare.py <raiz del repositorio> [directorio de salida (obj por defecto)]"""
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else here.parents[2]
out_dir = Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else here / 'obj'
src = root / 'source/game_sa/Radar.cpp'
s = src.read_bytes().decode('utf-8')
a = s.index('// Estaticas locales de DrawEntityBlip')
b = s.index('/*\n * @brief Clear a blip by its index in `ms_RadarTrace`', a)
head = s[s.index('#include "StdInc.h"'):s.index("// Array of TXD slot indices")]
out_dir.mkdir(parents=True, exist_ok=True)
(out_dir / 'Radar_h.cpp').write_bytes((head + s[a:b]).encode('utf-8'))
print('Radar_h.cpp listo desde', src, '(%d bytes de funcion)' % (b - a))
