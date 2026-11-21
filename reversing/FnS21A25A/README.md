<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnS21A25A: lotes S21A, S22A, S23A, S24A y S25A del plan de sustitucion (oleada 2: RwPlCore, RwTexDict, RpWorld)

120 raices del EXE reconstruidas en C++ y enganchadas (hook activo, `validated=0` en docs/hooks.csv). Los llamadores propios usan ahora la funcion propia en lugar de la direccion del EXE.

Archivos: `source/game_sa/RenderWare/rw/rwplcore.cpp` (S21A, S22A), `rwcore.cpp` (modulo de imagenes y destructor del modulo de texturas), `rwtexdict.cpp`/`RwHelper.cpp` (0x730E50), `rpworld.cpp` (S23A-S25A), `source/game_sa/X87Math.cpp/.h` (ayudantes x87). Ganchos: `RwPlCoreInjectHooksS21A22A`, `RwCoreInjectHooksS22A`, `RpWorldInjectHooksS23A25A` y la linea de RwHelper.cpp.

Herramientas: `dis_Sxx.txt` (desensamblado), `sitios.txt` (llamadores propios), `refs_exe.py/.txt` (referencias dentro del EXE), `x87gen.py` (traductor de x87 en linea recta a C++; genero 0x7F12F0).

Verificacion: compilacion (ver abajo). NO se emulo contra el EXE (Unicorn) ni se ejecuto ningun banco.

| lote | direccion | tamano | estado | verificacion |
|---|---|---|---|---|
| S21A | 0x007ED670 | 183 | reconstruida y enganchada x87 | solo compilacion |
| S21A | 0x00801C10 | 15 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x00801C20 | 8 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008020A0 | 14 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x00802230 | 11 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x00807C40 | 25 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x00807C60 | 10 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008081B0 | 15 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008081C0 | 40 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x00808430 | 59 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008086B0 | 5 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008086C0 | 5 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008086D0 | 5 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008087B0 | 20 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x008087D0 | 59 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x00808810 | 16 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080A3A0 | 42 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080A3D0 | 42 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080A400 | 26 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080A420 | 26 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080A780 | 96 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080A7E0 | 43 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080AA40 | 16 | reconstruida y enganchada  | solo compilacion |
| S21A | 0x0080AA50 | 16 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00730E50 | 16 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x007ED7D0 | 163 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007ED910 | 149 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007EDBF0 | 101 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007EDC60 | 250 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007EDE20 | 110 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x007EDE90 | 543 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007F12F0 | 314 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007F2720 | 525 | reconstruida y enganchada x87 | solo compilacion |
| S22A | 0x007F3490 | 21 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x007F34B0 | 25 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x007F3D00 | 340 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00801C30 | 286 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x008020B0 | 19 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x008020D0 | 19 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00802280 | 608 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x008024E0 | 313 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00804200 | 58 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00807C90 | 240 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00807D80 | 86 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x00807ED0 | 482 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x008080C0 | 239 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x008083F0 | 55 | reconstruida y enganchada  | solo compilacion |
| S22A | 0x0080A450 | 26 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x007491A0 | 27 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749480 | 152 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749720 | 20 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749740 | 20 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749760 | 205 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749880 | 23 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x007498A0 | 5 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x007498B0 | 23 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x007498D0 | 5 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749910 | 43 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749940 | 43 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749E40 | 14 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x00749E50 | 14 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A200 | 52 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A240 | 24 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A260 | 5 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A270 | 21 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A4F0 | 45 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A550 | 45 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074A580 | 44 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074B030 | 37 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074BD40 | 82 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074BEA0 | 24 | reconstruida y enganchada  | solo compilacion |
| S23A | 0x0074BEE0 | 19 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074BF00 | 6 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074BF10 | 6 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074BFA0 | 25 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074BFC0 | 16 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074C4D0 | 16 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074C720 | 53 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074CA60 | 43 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074CDD0 | 24 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074CDF0 | 19 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074CE10 | 6 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074CE20 | 51 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074D750 | 114 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074D850 | 27 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074D8C0 | 20 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074D8E0 | 100 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074D950 | 61 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074DC50 | 24 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074DC70 | 24 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074DC90 | 19 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074DCB0 | 6 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074DCC0 | 109 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074E050 | 248 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074F1A0 | 101 | reconstruida y enganchada  | solo compilacion |
| S24A | 0x0074F730 | 21 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074F750 | 8 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FD30 | 24 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FD50 | 24 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FD70 | 19 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FD90 | 6 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FE90 | 66 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FEE0 | 121 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FF60 | 20 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x0074FF80 | 20 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750280 | 225 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750370 | 109 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x007503E0 | 67 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750710 | 72 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x007507B0 | 48 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x007509A0 | 5 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750A30 | 40 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750A60 | 41 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750A90 | 5 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750AA0 | 32 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750CD0 | 85 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750D90 | 27 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750DE0 | 18 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750E00 | 19 | reconstruida y enganchada  | solo compilacion |
| S25A | 0x00750E20 | 27 | reconstruida y enganchada  | solo compilacion |

## Notas y no comprobado
- Las funciones x87 (0x7ED670, 0x7ED7D0, 0x7ED910, 0x7EDBF0, 0x7EDC60, 0x7F12F0, 0x7F2720, tablas de 0x7EDE90) siguen la secuencia del desensamblado con ayudantes de X87Math.cpp (double = registro x87; float = fstp). 0x7ED670/0x7ED7D0 reutilizan los ayudantes ya comprobados de punto/vector (0x7ED730/0x7ED880), cuya secuencia coincide. `atan2` de 0x7F2720 usa la intrinseca (fpatan) como UVAnim.
- 0x7F2720 llama por direccion a 0x7F2930 (no es de este plan). Los rellenos de pila sin escribir (matrices temporales de 0x7F2720, campo `unused` de 0x74E050) van a cero.
- 0x7EDE90: si falla la segunda reserva no se libera la primera (igual que el EXE).
- El memoryAlloc por defecto ahora es `_rwFreeListAllocDefault`; la comparacion de `_rwPluginRegistryAddPlugin` acepta tanto su direccion como 0x801C30.
- Punteros a EXE que se conservan por no ser de este lote: 0x750480/0x750430/0x750460 (camara), 0x7507E0 (atomo), 0x750AC0 (luz), 0x74EEC0 (render de sectores por defecto), 0x74CE60, 0x7F2930, CRT (0x824257 etc.).
