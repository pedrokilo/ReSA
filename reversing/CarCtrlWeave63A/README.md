<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlWeave63A: CCarCtrl, parte B2, tanda A (funciones hoja)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-3`, HEAD df26d6fe7 + `source.patch`). 20480 pares (5 funciones x 4096), cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauración
en ambos lados, cobertura mínima cumplida (`minimums.json`, ~50 % de lo medido). Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlweave63a\20261020-225000-092518`.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos) |
|---|---|---|---|---|
| `CCarCtrl::TestCollisionBetween2MovingRects` | 0x425B30 | Simple | 4096 | ninguno (función pura; oráculo Python independiente) |
| `CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper` | 0x425F70 | Simple | 4096 | ninguno (oráculo Python independiente) |
| `CCarCtrl::SlowCarDownForOtherCar` | 0x42D0E0 | Simple | 4096 | 0x425F70, 0x425B30, 0x54F560, 0x54F1B0, 0x571B70, 0x411990, 0x56E0D0, 0x423FC0 |
| `CCarCtrl::WeaveForOtherCar` | 0x426350 | Simple | 4096 | 0x56E0D0, 0x421A50 (sin reconstruir); 0x59C890/0x59C790 reales |
| `CCarCtrl::WeaveForObject` | 0x426BC0 | Simple | 4096 | 0x4212C0, 0x54F560, 0x54F1B0, 0x53CC70 |

`source.patch` (fuente `source/game_sa/CarCtrl.cpp`) contiene SOLO los cambios de estas cinco funciones, el bloque de ayudantes (espacio de nombres anónimo `kWv*`, `WvRaw`, `WvMatrixTimesPoint/Vector`) y sus cinco líneas `RH_ScopedInstall`;
se aplica sobre HEAD (`git apply --check` verificado). Los patches de las tandas B y C (`CarCtrlWeave63B/source.patch`, `CarCtrlWeave63C/source.patch`) se aplican DESPUÉS, en ese orden (A, B, C reproducen exactamente la copia de trabajo).

## Qué hace cada función
- `TestCollisionBetween2MovingRects(v1, v2, arg3, arg4, A, B)`: dos pasadas (esquinas del vehículo 1 en el eje B) contra el rectángulo del 2 (eje A) con velocidad relativa (arg3, arg4); intervalo de entrada/salida por eje; devuelve el menor tiempo (empieza en 1.0f).
- `..._OnlyFrontBumper`: lo mismo con solo las cuatro esquinas del parachoques del vehículo 1 contra los lados del 2 (productos vectoriales); devuelve el menor tiempo de corte.
- `SlowCarDownForOtherCar(car1, car2, *speed, cruise)`: normaliza el rumbo 2D de ambos; si car1 está por delante calcula el menor tiempo de choque (`min(OnlyFrontBumper, TestCollision)`), marca bHonkAtCar y el obstáculo (con referencia), reduce `*speed` según el tiempo y, con tiempos < 0.5, coches enfrentados y más de 15 s desde el cambio a física real, baja la velocidad, fuerza física real y estilo AVOID_CARS.
- `WeaveForOtherCar(entity, vehicle, *a, *b)`: salidas por misión (3, 16, 0x35, 0x37, escolta/protección), cuatro esquinas de cada rectángulo con margen 0.2 (0x59C890/0x59C790), hasta 8 desplazamientos de 6 grados de cada ángulo mientras 0x421A50 diga que chocan.
- `WeaveForObject(entity, vehicle, *a, *b)`: desplazamientos según el modelo (cinco semáforos/farolas o cualquier modelo con SwaysInWind), posición del objeto, ángulo con GetATanOfXY y ensanchado del arco (a, b) con normalización a [-pi, pi].

## Revisión propia contra el EXE (fase 2)
Cada función se desensambló completa (`dis.sh`) y se normalizó el marco de pila (`[esp+X]` -> `[s+/-Y]`, s = ESP a la entrada) para transcribir las pilas x87 sin perder huecos; después se contrastó el C++ instrucción a instrucción (orden de operaciones x87,
comparaciones `test ah,41h/05h/44h` con su caso NaN, constantes del EXE por dirección con sus bits, orden y número de llamadas a colaboradores). Todo float del EXE va a 24 bits (palabra de control del juego): cada operación se redondea a float, igual que
la aritmética `float` de la DLL (este archivo se compila en SSE sin optimizar, que guarda cada intermedio como float).

Discrepancias halladas y corregidas: (1) el borrador del lote 27 de `SlowCarDownForOtherCar` suponía x87 de 53 bits (dobles) y estaba sin compilar: se reescribió con float a 24 bits; (2) `WeaveForOtherCar` pasa a 0x421A50 un dword de pila cuyo byte bajo
es la bandera y cuyos tres altos son restos del producto escalar `entityAlongRight`: se reproduce igual para que el falso vea el mismo argumento; (3) `SlowCarDownForOtherCar` comprueba (y crea) la matriz de car1 dos veces y llama a `GetMatrix` de car1 dos veces: se reproduce;
(4) `WeaveForObject` comprueba la matriz del objeto en cuatro puntos: se reproducen los cuatro. Defectos de producción hallados por el banco: ninguno en la lógica (solo diferencias de rango/NaN del x87, ver Límites).

## Registros EAX/ECX/EDX
Los llamadores del EXE (0x42D2A9/0x42D2D7, 0x43259D, 0x42D7BD, 0x42DA9A) no leen EAX/ECX/EDX tras la llamada (`residue_scan`; los `push ecx` posteriores son reservas de espacio que sobrescribe un `fstp [esp]`). Solo se registran.

## Banco
`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con SOLO ese hook encendido. Los falsos se parchean en la entrada del EXE y en el cuerpo de la DLL (resuelto antes de escribir parches) y registran EN ORDEN sus argumentos (punteros como fichas
simbólicas `E<i>`, `M<i>`, `OUT`, `L`; los vectores locales por contenido). Arena con guardas (4 entidades de 0x600 bytes con basura determinista, matrices, col models, info de modelo, valores de salida); se comparan eventos, cambios de TODA la arena
(`X` = cambio inesperado), el ST0 de retorno de las dos puras y las compuertas ESP/EBX/ESI/EDI/EBP/DF/SEH/x87 normalizado/ruta/colaboradores/globales (reloj 0xB7CB84 y tabla `ms_modelInfoPtrs` restaurados). `run_local.py` exige además que las puras
coincidan con `fixture_oracle.py` (autoprueba: `python -I fixture_oracle.py`) y la cobertura de `minimums.json`. Para las tres funciones con colaboradores la verdad es el EXE (EXE == DLL bit a bit); el oráculo Python cubre las dos puras y la geometría de WeaveForObject (`wfo_geometry`, usada para descartar entradas con bucles de ángulo interminables).

## Suposiciones y límites
- El juego corre con x87 a 24 bits pero los registros conservan el exponente extendido: entradas cuyos cuadrados o productos superan el rango de float (|v| > ~1e19) dan inf en la DLL y un valor finito en el EXE; además dos NaN de signo distinto en una operación
  se propagan distinto (x87 elige el mayor significando, SSE el primero). Las fixtures los evitan (`Odd()` usa ~1e10 como "grande", un solo valor raro por caso, sin arg4 NaN junto a rumbos NaN). No hay subnormales (MXCSR FTZ del juego).
- Todos los colaboradores con efectos son falsos: no se validan sus cuerpos ni la integración real con mundo, jugador o referencias.
- WeaveForObject: los bucles de ángulo no terminan con margen infinito (distancia 0) ni con valores enormes, ni en el EXE ni en la DLL; las fixtures los evitan.

## Comandos
```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlWeave63A      # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlWeave63A/fixture_oracle.py                                              # autoprueba del oráculo
python -I reversing/CarCtrlWeave63A/fixture_mirror.py                                              # fixtures + cobertura de entrada (sin el juego)
python -B reversing/CarCtrlWeave63A/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlWeave63A/register_hook.py <carpeta-de-evidencia>                           # SOLO el coordinador, tras una corrida aprobada
```
