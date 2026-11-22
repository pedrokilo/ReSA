Arnes de verificacion de `0x75F0B0` (generador de vertex shaders), 22/11/2026.

1. `python extraer_core.py` copia a `core.inc` el generador C++ de `rpworld.cpp`.
2. `build.bat` compila `harness.cpp` (MSVC, x64; usa el `sprintf` del CRT) a `harness.exe`.
3. `python emu.py ref.txt <aleatorios> [semilla]` ejecuta el `0x75F0B0` ORIGINAL de `gta-sa.exe` en Unicorn (secciones mapeadas; `sprintf` del motor, `[0xC97B24]+0xF0`, implementado en Python; `D3DXAssembleShader` 0x7652AE interceptado: captura el texto y falla) para cada descriptor x version (0xA, 0x14, 0x19) y escribe texto + despacho; `ref.txt.in` es la entrada del arnes.
4. `python cmp.py ref.txt` pasa `ref.txt.in` por `harness.exe` y compara texto y despacho byte a byte.
Requiere `pip install unicorn capstone`. `exedump.py` lee el PE sin dependencias (`python exedump.py <va hex> <bytes>` desensambla con capstone).
