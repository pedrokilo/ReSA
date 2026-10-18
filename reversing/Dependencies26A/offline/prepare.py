"""Prepara offline/obj/Hw_h.cpp: el texto de CAEAudioHardware::PlaySound de source/game_sa/Audio/Hardware/AEAudioHardware.cpp, BYTE A BYTE (desde el comentario `// 0x4D86B0` hasta
`// 0x5B9340`), con las mismas cabeceras que el fichero original. Asi el ensayo compila la funcion tal como esta en el repositorio sin arrastrar InjectHooks ni el resto de la clase.
Uso: python prepare.py <raiz del repositorio> [directorio de superposicion con game_sa/Audio/Hardware/AEAudioHardware.cpp o '-'] [directorio de salida (obj por defecto)]"""
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else here.parents[2]
overlay = Path(sys.argv[2]).resolve() if len(sys.argv) > 2 and sys.argv[2] != '-' else None
out_dir = Path(sys.argv[3]).resolve() if len(sys.argv) > 3 else here / 'obj'
src = (overlay / 'game_sa/Audio/Hardware/AEAudioHardware.cpp') if overlay and (overlay / 'game_sa/Audio/Hardware/AEAudioHardware.cpp').exists() else root / 'source/game_sa/Audio/Hardware/AEAudioHardware.cpp'
s = src.read_bytes().decode('utf-8')
a = s.index('// 0x4D86B0')
b = s.index('// 0x5B9340')
head = s[s.index('#include "StdInc.h"'):s.index('auto& AEAudioHardware')]
out_dir.mkdir(parents=True, exist_ok=True)
(out_dir / 'Hw_h.cpp').write_bytes((head + s[a:b]).encode('utf-8'))
print('Hw_h.cpp listo desde', src, '(%d bytes de funcion)' % (b - a))
