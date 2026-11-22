Arnes de verificacion por pares del lote RW5 (22/11/2026). Ver `docs/CodigoVivoExe/ejecucion-rw-r5.md`.

1. `python extraer.py` copia de las fuentes del proyecto (`source/game_sa/RenderWare/rw/*.cpp`) las funciones reconstruidas a `core_*.inc`.
2. `build.bat` (MSVC x86) compila `harness.cpp` (+ `harness2.inc`, `harness3.inc`) a `harness.exe`, enlazado con base `0x400000` y una seccion de 14 MB (`gtaimg`) que reserva `0x401000..0x1201000` para el EXE.
3. `harness.exe <matrix|strip|sector|clump|tex> [pruebas] [semilla]` mapea `gta-sa.exe` (variable `GTASA_EXE`, por defecto `D:\Grand Theft Auto San Andreas\gta-sa.exe`) en sus direcciones originales y ejecuta el ORIGINAL y el C++ con las mismas entradas; los colaboradores del motor (memoria, flujos, registros de plugins, errores, imagenes y rasters) son funciones simuladas compartidas (a las del original se llega con un `jmp` parcheado). Compara retorno, registro de llamadas (con bytes escritos) y memoria. `VERBOSO=1` imprime los dos registros completos de la primera diferencia.
   * `matrix`: `0x7F2930` (x87 a 24 bits). `tex`: `0x7F53D0`, `0x7F40F0` y `0x7F5140`. `sector`: `0x7624A0`, `0x762740`, `0x762F30`, `0x763310`. `clump`: `0x74A880`, `0x74A770`. `strip`: `0x75A960`.
4. `llamadores.py` lista llamadores (`call`/`jmp` rel32) y punteros del EXE a cada funcion.

Detalles: los 0x180 primeros bytes de `.text` del EXE (`0x401000..0x40117F`) no se copian (los usa el propio arnes). Los 4 bytes de basura de pila de la cabecera de sector (`+0x24` de 0x2C) y los 8 de indices de atomo sin listas se enmascaran. Tras `build.bat` queda `harness.exe` (no se versiona).
