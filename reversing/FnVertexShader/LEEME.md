<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

Arnes de verificacion de `0x75F0B0` (generador de vertex shaders), 22/11/2026.

1. `python extraer_core.py` copia a `core.inc` el generador C++ de `rpworld.cpp`.
2. `build.bat` compila `harness.cpp` (MSVC, x64; usa el `sprintf` del CRT) a `harness.exe`.
3. `python emu.py ref.txt <aleatorios> [semilla]` ejecuta el `0x75F0B0` ORIGINAL de `gta-sa.exe` en Unicorn (secciones mapeadas; `sprintf` del motor, `[0xC97B24]+0xF0`, implementado en Python; `D3DXAssembleShader` 0x7652AE interceptado: captura el texto y falla) para cada descriptor x version (0xA, 0x14, 0x19) y escribe texto + despacho; `ref.txt.in` es la entrada del arnes.
4. `python cmp.py ref.txt` pasa `ref.txt.in` por `harness.exe` y compara texto y despacho byte a byte.
Requiere `pip install unicorn capstone`. `exedump.py` lee el PE sin dependencias (`python exedump.py <va hex> <bytes>` desensambla con capstone).
