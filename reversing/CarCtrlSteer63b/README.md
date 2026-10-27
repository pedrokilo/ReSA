<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlSteer63b: SteerAICarWithPhysicsFollowPreRecordedPath (lote 63, CCarCtrl parte B1, tanda 2)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-2`, HEAD df26d6fe7 + fuente de las tandas 1 y 2). 4096 pares, cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauracion
en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, desde el desensamblado); cobertura minima cumplida. Evidencia final:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63b\20261020-203115-650930`.
Una corrida anterior (`20261020-202016-475381`) dio FALLO solo por un defecto del ORACULO (modelaba la ventana de registros con `nf` registros y el banco escribe los 12 de la ventana; EXE y DLL eran
iguales en los 4096 pares); corregido el oraculo, repetida la corrida completa.

**Regresion sobre el arbol FINAL** (las seis tandas 63/63b..63f juntas, compiladas y corridas de nuevo el 20/10/2026 21:10..22:30): APROBADO, carpeta de evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63b\20261020-212552-572842`; es la que corresponde al `artifacts/build-manifest.json` de este banco (usar esta para `register_hook.py`).

| Funcion | Direccion | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath(vehicle, steer, accel, brake, handbrake)` | 0x432DD0 | Simple | 4096 | CGeneral::GetATanOfXY 0x53CC70, FindAngleToWeaveThroughTraffic 0x4325C0, FindMaxSteerAngle 0x427FE0, CVehicleRecording::StopPlaybackWithIndex 0x459440 |

`cdecl`, `void`; el llamador limpia la pila (5 argumentos).

## Que hace (resumen del desensamblado)

Sigue una grabacion de coche (arrays de `CVehicleRecording` en 0x97D6D0..0x97D840, registros de 32 bytes con la posicion en +0x14 y la velocidad empaquetada en +4/+6):
1. Sin reproduccion (`m_vehicleRecordingId` < 0 en +0x424), sin bufer o en pausa: pone las salidas a 0 (mision 0x0B; en pausa freno 0.5 y la mision no cambia) y sale.
2. Avanza el contador (PlaybackIndex, +0x20 por paso) mientras el vehiculo este a menos de 10 m del registro actual o mas cerca del siguiente; al llegar al final (`contador >= tamano - 0x20`,
   sin signo) llama a `StopPlaybackWithIndex`, pone el id a -1, salidas a 0 y mision 0x0B.
3. Giro: rumbo del vehiculo (`GetATanOfXY` de su avance) y rumbo al registro (`FindAngleToWeaveThroughTraffic(veh, 0, rumbo, avance, 2.0f)`), diferencia normalizada a [-pi, pi] y limitada a `FindMaxSteerAngle`.
   **El registro usado para el giro y la velocidad objetivo es siempre el PRIMERO (el puntero se guarda una vez en EBX); los avances solo cambian el contador y las distancias.**
4. Velocidad objetivo `|v registro| * PlaybackSpeed * 60` (suelo de 5 mientras el contador <= 0x320) frente a `|v| * 60`: acelerador `min(1, error * (0.25 si |v|*60 < 2 si no 0.125))` o freno `min(0.5, -0.05 * error)`;
   freno de mano siempre 0.

## Fuente (CarCtrl.cpp)

`float` puro (con el x87 a 24 bits cada operacion del EXE equivale a la de `float`), arrays de `CVehicleRecording` leidos con `.data()[id]` sin comprobar el indice (como el EXE). `#include "VehicleRecording.h"`
anadido. Sin ayudantes x87: aqui el `fpatan` de 0x53CC70 se guarda a float antes de usarse (`fstp [esp+38h]`), no hay resta sobre el ST0 sin redondear (a diferencia de HeadingForTarget).

## Revision propia contra el EXE (fase 2)

Se desensamblo `0x432DD0..0x433276` completo y se contrasto bloque a bloque: salidas tempranas y orden de escritura de las salidas, la condicion del bucle (`d1 < 10 || d2 < d1`; los dos `fcomp` con `test ah,5`/`jnp`),
la comparacion SIN signo del final (`jae`), el uso del puntero de registro original en el giro, `cmp [ebp],320h; ja` tras los avances, los dos factores `fmul` en el orden `(s * mult) * 60`, y los cuatro `fcomp` del
acelerador/freno con NaN. Discrepancias respecto al borrador mental inicial corregidas antes de la corrida: el registro de giro/velocidad no se actualiza al avanzar; el id negativo se detecta con `movsx` (int8);
el final de grabacion escribe `-1` en +0x424 DESPUES de `StopPlaybackWithIndex`. Defectos reales de la fuente hallados por el banco: ninguno (un FALLO inicial fue del oraculo).

## Registros (EAX/ECX/EDX)

Llamador unico del EXE: 0x437540 (dentro de `SteerAICarWithPhysics_OnlyMission`, 0x436A90): tras la llamada solo hace `add esp,14h; pop edi; pop ebx; pop esi; pop ebp; ret` en una funcion `void`; **ninguno lee EAX/ECX/EDX**. Solo se registran.

## Banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con solo ese hook encendido. Los cuatro colaboradores son falsos identicos en ambos lados (parche en la entrada del EXE y, si tienen hook, en el
cuerpo de la DLL) que registran EN ORDEN sus argumentos. El falso de `GetATanOfXY` devuelve un `double` con mantisa completa. Los arrays reales de `CVehicleRecording` del EXE (0x97D6C0..0x97D880, 112 palabras)
se rellenan por caso (basura determinista en los demas elementos; el elemento `slot` con pausa, multiplicador, tamano, contador y bufer propios), se comparan los cambios y se restauran. Vehiculo, matriz y grabacion
(52 registros de 32 bytes, con la ventana de 12 registros del caso) son objetos del banco. `fixture_oracle.py` modela la funcion desde el desensamblado; `python -I fixture_oracle.py` autoprueba; `fixture_mirror.py` compila
`Fixtures.hpp` con `cl` y pasa los 4096 casos por el oraculo con las constantes de disposicion y la cobertura (`minimums.json`, 50 % de lo medido).

## Suposiciones y limites

- Colaboradores falsos: no se validan sus cuerpos. NaN silenciosos; sin rumbos infinitos ni enormes; indice de reproduccion 0..15 o negativo (los arrays no se leen fuera de rango).
- Las posiciones basura de las fixtures pueden desbordar el float en los cuadrados (el x87 del EXE conserva el rango de exponente en registro, la DLL con SSE da infinito); el banco no distingue casos en que eso cambie la
  decision (no aparecieron en 4096). Posiciones reales del juego estan lejos de ese rango.
- `FindAngleToWeaveThroughTraffic` (0x4325C0, aun envoltura) se lee como `float` redondeado; ver README de CarCtrlSteer63.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlSteer63b   # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlSteer63b/fixture_oracle.py       # autoprueba del oraculo
python -I reversing/CarCtrlSteer63b/fixture_mirror.py       # coherencia de las fixtures + cobertura minima (sin el juego)
python -B reversing/CarCtrlSteer63b/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlSteer63b/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
