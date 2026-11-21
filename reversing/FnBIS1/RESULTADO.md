<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BIS1: resultado de la biseccion (19/11/2026)

## Causa del fallo «mundo en blanco / NaN / caidas a z=-100»

`CVehicle::ProcessWheel` (0x6D6C00). La biseccion automatica (robar coches de trafico REAL parados y con la IA activa, sin congelar el coche) bajo a esta unica funcion:
con solo `Vehicle/CVehicle` encendido y el resto del juego con el codigo original del EXE el NaN sale en minutos; con todo apagado, 48 robos limpios; con solo
`ProcessWheel` apagada, rondas OK (15, 17, 19, 22).

Mecanismo (volcado de la DLL con `--vigilar-nan` y trampa dentro de la funcion):
1. El codigo del coche (EXE) envia a veces `brake = NaN` a `ProcessWheel`. En el EXE es inocuo.
2. EXE (0x6D6ECE..0x6D6F10): `fcom brake, adhesion; test ah,0x41; jp` entra en la rama «frena mas que la adherencia» con brake > adhesion O con comparacion no ordenada, sin tocar `fwd`;
   fuera de ella solo asigna `fwd = brake` / `fwd = -brake` con comparacion estricta y ordenada. Igual en el bloque de conduccion (0x6D6DBF..0x6D6DFF) con `right`.
3. La reconstruccion escribia `brake > adhesion`, `!(fwd <= brake)`, `!(fwd >= -brake)` y `!(right <= adhesion)`: con NaN asignaba `fwd = -brake = NaN`; la fuerza NaN se aplicaba al coche
   (velocidad NaN, luego posicion y matriz) y CJ, que alinea con la puerta, quedaba NaN. Con la posicion del jugador NaN el streaming deja de ver el mundo: se descargan colisiones y modelos
   (todo cae a z=-100, mundo en blanco, «Cargando...»). Los crashes en `ClearTasks` (tarea por defecto nula) eran consecuencia.
4. Aparte (misma funcion): el EXE mantiene `1/modulo` y `tractionLoss/l` en la pila x87 (exponente ampliado); la version en float desbordaba a inf con modulos denormales (0*inf = NaN).

Arreglo (en `source/game_sa/Entity/Vehicle/Vehicle.cpp`): comparaciones estrictas y ordenadas como el EXE; modulos, recipros y factor de adherencia con los ayudantes `Veh83D*` de X87Math.

## Evidencia en juego (modo `--robar=200,6,15,2,0,1`, DLL BIS1)

| DLL | Resultado |
|---|---|
| sin arreglo (v9) | NaN en 3 de 4 corridas (ciclos 4, 5, 27); rondas de biseccion con error en 2-3 min |
| solo x87 (v10/v11) | NaN en 3 de 3 (con `brake=-nan` en las entradas) |
| comparaciones + x87 (v12/v13) | 2 corridas de 25 min: 133 robos, 0 NaN, 0 caidas |

Con el trafico congelado (freno de mano) el fallo no aparecia (126 robos): hace falta la IA del coche activa.

## Pendiente

* Repetir el banco de `ProcessWheel` (CVehD, 0x6D6C00) con entradas `brake` NaN, `adhesion` 0/negativa y velocidades denormales: la evidencia anterior es anterior al arreglo.
* Buscar el mismo patron `!(a <= b)` / `!(a >= b)` sobre valores de punto flotante en otras funciones que copian un `fcom` + `test ah,0x41/0x44/5` del EXE.
* `--vigilar-nan` y el modo `--robar` con trafico real quedan en la DLL como herramientas de diagnostico (sin efecto sin esos argumentos).

## Como reproducirlo

```
pwsh -File biseccion.ps1 -Auto -Robar 200,6,15,2,0,1 -MaxSegundos 1500 -MinRobosAuto 45     # biseccion sin intervencion
pwsh -File robar_serie.ps1 -Variante A -Veces 2 -Robar 200,6,15,2,0,1 -Auto 1500            # medir tasa de NaN
pwsh -File jugar.ps1 -Variante A -ArgsFile vigilar.args -Sonda                              # jugar a mano con el vigilante de NaN
```
`biseccion.ps1` acepta la respuesta de las preguntas por archivo (`respuesta.txt` con `s`, `n` o `d`).

## Aviso: el control «todo apagado» cambiaba la resolucion de los monitores (corregido)

`--unhook-all` (y las rondas de biseccion que apagaban `RenderWare/WindowedModeHooks`) hacian que el juego arrancase con el codigo original del EXE: pantalla completa exclusiva a 1024x768.
Tras muchas ejecuciones asi, cerradas a la fuerza, el monitor HDMI del usuario quedo reconocido como «Generic Monitor» con solo modos genericos. Las variantes B de `jugar.ps1`
(FnBIS1, FnSIM1, FnPL1), `FnRW0A/run_phase.py` y la biseccion (`ddmin.py`, `EXCLUIDAS_CAT`) mantienen ahora `RenderWare/WindowedModeHooks` encendido (`--unhook-except=RenderWare/WindowedModeHooks`).
Sin comprobar en juego (no se ha vuelto a lanzar el juego tras el cambio).
