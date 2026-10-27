<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlSteer63: SteerAICarWithPhysics TryingToBlockTarget_Stop / HeadingForTarget / TryingToBlockTarget (lote 63, CCarCtrl parte B1, tanda 1)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-2`, HEAD df26d6fe7 + fuente de esta tanda). 12288 pares (3 funciones x 4096), cero diferencias EXE/DLL, todas las compuertas de
ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, desde el desensamblado); cobertura minima cumplida (34 claves + 24 familias).
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63\20261020-200843-747849` (primera aprobacion, con el arbol de la tanda 1).
Una corrida anterior identica quedo en `GTA termino antes de completar (3221226505 = 0xC0000409)` antes de escribir ningun caso (fallo transitorio del arranque del juego, sin registro del banco); repetida sin
recompilar dio APROBADO.

**Regresion sobre el arbol FINAL** (las seis tandas 63/63b..63f juntas, compiladas y corridas de nuevo el 20/10/2026 21:10..22:30): APROBADO, carpeta de evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlsteer63\20261020-212103-126364`; es la que corresponde al `artifacts/build-manifest.json` de este banco (usar esta para `register_hook.py`).

| Funcion | Direccion | Hook | Pares | Colaboradores sustituidos |
|---|---|---|---|---|
| `CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop(vehicle, x, y, a4, a5, o6, o7, o8, o9)` | 0x428990 | Simple | 4096 | FindPlayerVehicle 0x56E0D0, CTaskManager::GetActiveTask 0x681720, CVehicle::IsUpsideDown 0x6D1D90, CCarAI::TellOccupantsToLeaveCar 0x41C760 (+ CTask::GetTaskType virtual por vtable falsa) |
| `CCarCtrl::SteerAICarWithPhysicsHeadingForTarget(vehicle, target, x, y, steer, accel, brake, handbrake)` | 0x433280 | Simple | 4096 | CGeneral::GetATanOfXY 0x53CC70, FindAngleToWeaveThroughTraffic 0x4325C0, FindMaxSteerAngle 0x427FE0, 0x4224E0 (FindSpeedMultiplier) |
| `CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget(vehicle, unused, a3, a4, a5, a6, steer, accel, brake, handbrake)` | 0x4335E0 | Simple | 4096 | SteerAICarWithPhysicsHeadingForTarget 0x433280 |

Las tres son `cdecl`, `void`; el llamador limpia la pila.

## Que hace cada una (resumen del desensamblado)

- **Stop (0x428990)**: escribe `*o6=0, *o7=0, *o8=1.0f, *o9=1`. Con la distancia^2 al punto (x, y) > 100 (en float) la mision pasa de 0x13 a 0x12, o a 5 si era otra, y sale. Con mision 0x13
  (BLOCKCAR_HANDBRAKESTOP): sale si la tarea activa del conductor es TASK 0x451, si |v|^2 (x, y) >= 1e-4 o a4^2 + a5^2 >= 4e-4 o no hay bandera `bIsLawEnforcer`; si no, `TellOccupantsToLeaveCar`,
  crucero 0 y mision 0. Con otra mision: temporizador (uint16 en +0x4DC) = trunc(paso de tiempo x 16.666 + temporizador) mientras el vehiculo del jugador va a < 0.05, o 0; despues, segun
  `IsUpsideDown`/velocidad/temporizador > 0x9C4 y la distancia^2 < 100, expulsa a los ocupantes igual que antes. `FindPlayerVehicle(-1, false)` se vuelve a pedir en cada uso (hasta 5 llamadas).
- **HeadingForTarget (0x433280)**: normaliza la direccion de avance (columna top de la matriz; longitud 0 deja (1, y)), `GetATanOfXY` dos veces (objetivo y avance), con los estilos de conduccion 2/5/6
  usa `FindAngleToWeaveThroughTraffic(veh, target, rumbo, avance, 1.0f)`, normaliza la diferencia a [-pi, pi] con bucles, freno de mano si |v| > 0.3 y |dif| > 0.7, limita a +-`FindMaxSteerAngle`,
  `0x4224E0(GetATanOfXY(x - px, y - py) - avance, 0.4, 1.2, 0.4)` y de ahi acelerador/freno: `error = mult * crucero - |v|*60`; con error > 0 el acelerador es 1.0f si |v|*60 >= 25 o min(1, 0.1*error);
  con subtipo (+0x594) 10, error > 3 y +0x818 <= 0 lo rearma a 10; si no, freno = min(0.5, -0.05*error). `*steer` es la diferencia limitada.
- **TryingToBlockTarget (0x4335E0)**: limita la velocidad del objetivo (a5, a6) a longitud 0.13, proyecta (a3, a4) + 60 x esa velocidad, fija el estilo de conduccion a 2, llama a HeadingForTarget con ese
  punto y, si la distancia^2 al punto es < 25, alterna la mision 0x12 -> 0x13 (cualquier otra -> 6).

## Fuente (CarCtrl.cpp / X87Math.cpp/.h)

Las tres reconstruidas en C++ en `CarCtrl.cpp` con hooks `RH_ScopedInstall`. Aritmetica en `float`: con el x87 a 24 bits de precision (estado del juego) cada operacion `+ - * /` y `fsqrt` del EXE equivale
a la de `float` IEEE, y los operandos que el EXE deja en registro estan siempre redondeados a 24 bits. Unica excepcion hallada: el **segundo `GetATanOfXY` de HeadingForTarget deja el `fpatan` sin redondear en ST0 y el
EXE le resta el rumbo del vehiculo antes de guardarlo** (un float intermedio redondea dos veces). Se resuelve con el ayudante `CarCtrlSteer63AtanMinusCpp` (bloque «Lote 63c» al final de `X87Math.cpp/.h`,
compilado /arch:IA32 /fp:fast): `GetATanOfXY(x, y) - hf` con el `fsub` directo sobre ST0. **El banco lo detecta**: con un `float` intermedio en lugar del ayudante fallan 788 de 4096 casos de HeadingForTarget.
`#include "X87Math.h"` anadido a CarCtrl.cpp. `FindAngleToWeaveThroughTraffic` y `0x4224E0` (envolturas a su direccion del EXE) se llaman como `float`.

## Revision propia contra el EXE (fase 2)

Se desensamblaron `0x428990..0x428A1F/0x428BD6`, `0x433280..0x4335D2` y `0x4335E0..0x4336C5` y se contrasto instruccion a instruccion (pilas x87, desplazamientos de argumentos reutilizados como locales,
condiciones `test ah,5/41h/44h` con NaN). Discrepancias halladas respecto al borrador del lote 27 (`git diff HEAD -- CarCtrl.cpp` del arbol principal, solo existia Stop): (1) el borrador calculaba en `double`
con la hipotesis de 53 bits; el estado del juego es de 24 bits (todo en `float`); (2) el borrador del temporizador convertia con `(int32)(double)`; el EXE hace `_ftol2` (se usa `(int16)(int64)`, que da 0 para
NaN/inf/enorme); (3) la comparacion de la distancia a 100 se hace con el valor ya redondeado a float (`fst dword`), no con el de precision extendida. Defectos reales de la fuente hallados por el banco: **uno**,
el doble redondeo descrito arriba (corregido con el ayudante antes de la corrida aprobada; la corrida con la version ingenua es la que fallo 788/4096).

## Registros (EAX/ECX/EDX)

Llamadores del EXE: Stop 0x437383 y 0x4373E8; HeadingForTarget 0x43365B (la propia TryingToBlockTarget), 0x433CDD, 0x434034, 0x434D14, 0x434D52, 0x434DA5, 0x434E21, 0x435B74, 0x436E02, 0x436E60, 0x436EBE,
0x43721F, 0x43742C, 0x437658, 0x4376BB, 0x437AE0; TryingToBlockTarget 0x4372A2 y 0x437308. **Ninguno lee EAX/ECX/EDX**: los de ParkParallel/Perpendicular escriben AL antes de leerlo (`mov al,[esi+3D0h]`),
0x436EBE escribe EDI/EAX, y el resto solo hace `add esp; pop; ret` en funciones `void`. Solo se registran (el banco los muestra distintos entre EXE y DLL, p. ej. EAX/ECX/EDX con basura de la pila).

## Banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con solo ese hook encendido. Los colaboradores son falsos identicos en ambos lados (parche en la entrada del EXE y, si tienen hook, en el
cuerpo de la DLL) que registran EN ORDEN sus argumentos (punteros como fichas simbolicas `V`, `PV`, `PED`, `I+4`, `T`, `O+n`) y devuelven lo que dicta el caso (tablas por orden de llamada). El falso de
`GetATanOfXY` devuelve un `double` con mantisa completa (como el `fpatan` sin redondear del EXE). El vehiculo, la matriz, el vehiculo del jugador, el conductor y su inteligencia, la tarea y las salidas
(`float*`/`bool*` con relleno basura) son objetos del banco (arena con guardas, puesta a cero salvo los campos especificados). Se compara: eventos de los falsos, cambios de toda la arena (region `X` =
cualquier cambio inesperado) y del estado real (CTimer::ms_fTimeStep, fijado por caso y restaurado), y las compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado, ruta del hook, entradas de los
colaboradores sin tocar, restauracion). `fixture_oracle.py` modela las tres funciones desde el desensamblado con `float` emulado (y la resta final exacta); `python -I fixture_oracle.py` ejecuta la autoprueba;
`fixture_mirror.py` compila `Fixtures.hpp` con `cl` sin el juego, pasa los 12288 casos por el oraculo, fija las constantes de disposicion y comprueba el generador pseudoaleatorio y la cobertura
(`minimums.json`, 50 % de lo medido).

## Suposiciones y limites

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integracion real con el resto de la IA de trafico.
- Los NaN de las fixtures son silenciosos; sin infinitos ni valores enormes en los rumbos (los bucles `while (dif < -pi)` no terminarian en ninguno de los dos lados); el vector de avance de HeadingForTarget
  no es denormal en cuadrado (el SSE del juego con FTZ lo vaciaria, el x87 del EXE no).
- `FindAngleToWeaveThroughTraffic` (0x4325C0, aun envoltura al EXE) se lee como `float` ya redondeado: si el EXE lo devuelve sin redondear en ST0 el banco (cuyo falso devuelve un float) no lo distingue.
  Al reconstruirlo habra que revisar la resta `weave - avance` (mismo caso que el `GetATanOfXY`).
- No se prueban matriz nula en HeadingForTarget (el EXE la desreferencia sin comprobar) ni punteros de salida nulos.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 pwsh -NoProfile -File D:\Original\wh-2\reversing\CVehicle\build-project.ps1
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlSteer63   # compila la sonda y corre en el juego (candado global)
python -I reversing/CarCtrlSteer63/fixture_oracle.py       # autoprueba del oraculo
python -I reversing/CarCtrlSteer63/fixture_mirror.py       # coherencia de las fixtures + cobertura minima (sin el juego)
python -B reversing/CarCtrlSteer63/run_local.py --analyze <comparisons.jsonl>
python reversing/CarCtrlSteer63/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Depuracion: un archivo `carctrlsteer63-cases.txt` en la carpeta del juego con «primero ultimo» limita la corrida a ese rango de casos (el analizador final fallara por el numero de pares; solo diagnostico).

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
`source.patch` contiene SOLO los cambios de estas tres funciones (y su ayudante x87 + el `#include`) respecto de HEAD (`git apply --check` verificado sobre `git show HEAD:<archivo>`).
