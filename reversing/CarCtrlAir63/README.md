<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlAir63: CCarCtrl parte A (helicopteros, aviones, lanchas y combate aereo)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-1`, HEAD df26d6fe7 + fuente del lote 63). Las 24 funciones asignadas estan reconstruidas en C++ sin llamadas al EXE para la propia
funcion y con su hook RH activo: 24 x 4096 = **98304 pares, cero diferencias EXE/DLL**, todas las compuertas de ABI/ruta/restauracion en ambos lados y x87 identico. Evidencia final y vigente:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlair63\20261020-222954-939361` (APROBADO, sin aserciones ajenas; `comparisons.jsonl` con los 98304 pares, `complete` y estado final restaurado).

Banco: EXE original (hook RH apagado) frente a la DLL (solo ese hook encendido), con fixtures deterministas de 4096 casos por funcion, colaboradores sustituidos por falsos identicos en ambos lados
(parche en la entrada del EXE y en el cuerpo de la DLL) y compuertas de ABI/ruta/restauracion. **No hay oraculo independiente en Python**: la referencia es el EXE (ver Suposiciones y limites).

## Funciones (todas Simple, `RH_ScopedInstall`, clase CCarCtrl)

| Funcion | Direccion | Estado |
|---|---|---|
| GetAIHeliToFlyInDirection | 0x42A730 | APROBADA |
| GetAIPlaneToDoDogFightAgainstPlayer | 0x42F370 | APROBADA |
| GetAIPlaneToAttackPlayer | 0x429780 | APROBADA |
| GetAIPlaneToDoDogFight | 0x429890 | APROBADA |
| GetAIHeliToAttackPlayer | 0x42F3C0 | APROBADA |
| SteerAIHeliToCrashAndBurn | 0x4238E0 | APROBADA |
| SteerAIHeliToFollowEntity | 0x42A750 | APROBADA |
| SteerAIHeliToKeepEntityInView | 0x42AEB0 | APROBADA |
| SteerAIHeliToLand | 0x42AD30 | APROBADA |
| SteerAIHeliTowardsTargetCoors | 0x42A630 | APROBADA |
| SteerAIHeliAsPoliceHeli | 0x42AAD0 | APROBADA |
| SteerAIHeliFlyingAwayFromPlayer | 0x42ACB0 | APROBADA |
| SteerAIPlaneToCrashAndBurn | 0x423880 | APROBADA |
| SteerAIPlaneToFollowEntity | 0x4237F0 | APROBADA |
| SteerAIPlaneTowardsTargetCoors | 0x423790 | APROBADA |
| SteerAIBoatWithPhysicsHeadingForTarget | 0x428BE0 | APROBADA |
| SteerAIBoatWithPhysicsAttackingPlayer | 0x428DE0 | APROBADA |
| SteerAIBoatWithPhysicsCirclingPlayer | 0x429090 | APROBADA |
| TestWhetherToFirePlaneGuns | 0x429520 | APROBADA |
| TriggerDogFightMoves | 0x429300 | APROBADA |
| FireHeliRocketsAtTarget | 0x42B270 | APROBADA |
| FlyAIHeliToTarget_FixedOrientation | 0x423940 | APROBADA |
| FlyAIPlaneInCertainDirection | 0x423000 | APROBADA |
| FlyAIHeliInCertainDirection | 0x429A70 | APROBADA |

## Corridas aprobadas por tandas (todas en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlair63\`)

| Corrida | Funciones | Pares | Carpeta |
|---|---|---|---|
| tanda 1 | 10 (GetAIHeliToFlyInDirection ... SteerAIHeliAsPoliceHeli) | 40960 | `20261020-195804-822225` |
| tandas 1-3 + lanchas | 20 | 81920 | `20261020-205019-215770` |
| + FireHeliRockets | 21 | 86016 | `20261020-210922-525466` (dejo despues la aserción intermitente ajena de `CShadowCamera::DrawOutlineBorder`) |
| + FixedOrientation | 22 | 90112 | `20261020-212653-904373` |
| **final, vigente** | **24** | **98304** | **`20261020-222954-939361`** |

Comando de la corrida final: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlAir63` (desde la raiz del worktree).

## Fuente (cambios de produccion) — `source.patch`

`source/game_sa/CarCtrl.cpp`: las 24 envolturas `plugin::Call` pasan a C++ (control de flujo, colaboradores y escrituras de campos) y sus 24 `RH_ScopedInstall` (bloque «Lote 63 (aire)» tras
`PossiblyFireHSMissile`); `#include "X87Math.h"`, `"TaskComplexWander.h"` y `"PedIntelligence.h"`; tres ayudantes estaticos `CarCtrlAirExeNormalise` (CVector::Normalise 0x59C910 por direccion),
`CarCtrlAirExeRand` (rand del EXE 0x821B1E, no la de la DLL) y `CarCtrlAirExeBoatSpecial` (0x73DF00). `CarCtrl.h` no cambia. `source/game_sa/X87Math.cpp/.h`: bloque «Lote 63 (CarCtrl aire)» al final,
prefijo `CcAir`, ~75 ayudantes x87 de 24 bits con exponente extendido (los operandos que el EXE usa desde memoria se escriben como float y los cargados con `fld` como double; solo los `fstp dword`
pasan a float con `volatile float`), y cuatro envoltorios sueltos `CcAirFsqrt/Fpatan/Fsin/Fcos` fuera de las regiones `precise` (dentro el compilador emite llamadas a la CRT, que difieren del EXE con NaN).
`source.patch` contiene SOLO estos tres archivos respecto de HEAD y se verifico con `git apply --check` contra `git show HEAD:<archivo>` en un directorio temporal.

## Revision propia contra el EXE (fase 2) y defectos hallados por el banco

Cada funcion se desensamblo (`dis.sh`), se anoto con desplazamientos relativos al marco (`annot.py` en el scratchpad) y se contrasto instruccion a instruccion; las discrepancias con el borrador del lote 27
(que nunca se habia compilado) se corrigieron antes de compilar:

- El borrador usaba `double` en un archivo SSE de 53 bits («el x87 de Windows trabaja con 53 bits»): el EXE trabaja a 24 bits; todo el calculo se rehizo con ayudantes x87 en X87Math.cpp.
- `SteerAIHeliTowardsTargetCoors` / `FollowEntity`: el camino con orientacion forzada es `+0x954 >= 0` ordenado (NaN no entra), el borrador usaba `!(x < 0)`. `FlyAIHeliToTarget_FixedOrientation` recibe un cuarto argumento que no lee.
- Mapas de campos: en CPlane +0x988/0x98C/0x990/0x994 son derrape/cabeceo/timon/aceleracion (distintos de CHeli +0x98C/0x990/0x994/0x998).
- Defectos reales encontrados por el banco (EXE vs DLL): (1) `FlyAIPlaneInCertainDirection`: la fase de suelo tenia invertido 0.4/0 en +0x98C; el «limite de rumbo» del bloque periodico y el rumbo propio que se guarda despues
  comparten la posicion de pila [esp+0x10], y los mandos leen el segundo (no hay lectura de pila sin inicializar); el signo del radio usa `fmul -1.0` (conserva el signo de un NaN) y no `fchs`.
  (2) con SNaN en memoria frente a un QNaN en ST(0) el x87 devuelve el QNaN y depende del codigo exacto que emita el compilador: los fixtures usan solo NaN silenciosos (ningun calculo del juego produce un SNaN).
  (3) `sqrt/atan2/sin/cos` dentro de regiones `float_control(precise)` salen como llamadas a la CRT (distintas del EXE con NaN): se movieron a envoltorios en linea.
  (4) fixtures: bucles de normalizacion de angulos con valores enormes no terminan en el original; el falso de GetATanOfXY y los argumentos se acotan; el banco tambien vigila el reloj (CTimer) y el paso (ms_fTimeStep).

## Registros EAX/ECX/EDX

Ningun llamador del EXE los lee (`callers.py` escanea todo `.text` buscando `E8/E9` hacia las 24 direcciones: solo llaman el despachador 0x436A90 —que tras cada llamada solo limpia la pila y retorna— y las propias
funciones del lote). Se registran, no se comparan. `GetAIPlaneToDoDogFightAgainstPlayer` termina con una llamada en cola a GetAIPlaneToDoDogFight (0x429890) en el EXE; en la DLL es una llamada normal.

## Que compara el banco

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con **solo** ese hook encendido. Por lado: eventos de los falsos EN ORDEN con argumentos (punteros como fichas `A`, `T`, `P`, `M<i>`, `PED`, `SCR`, `L`...),
cambios de TODA la arena (vehiculo, objetivo, jugador, matrices, ped, tarea, guardas), compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado FCW 24 bits pila vacia, ruta del hook, entradas de los
colaboradores sin tocar) y restauracion de CTimer (tiempo, tiempo previo y ms_fTimeStep). `run_local.py` exige ademas familias de fixtures (12 x >= 256 casos por funcion) y cobertura calibrada de caminos
(`minimums.json`: caminos distintos por funcion; `minimums_seq.json`: secuencias de colaboradores observadas con su recuento minimo).

## Suposiciones y limites

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integracion real con mundo, colision, proyectiles o tareas; solo el orden, los argumentos y el uso del resultado por la funcion.
- Sin oraculo independiente en Python (la referencia es el EXE): `fixture_oracle.py` no existe. La cobertura es la de los fixtures (umbrales exactos con +-1 ulp, -0.0, NaN silenciosos, infinitos, subnormales, enormes).
- Los NaN de los fixtures son siempre silenciosos (ver arriba). Los angulos que entran en bucles de normalizacion estan acotados (|v| < 1000 finito o NaN). Varias funciones usan `m_matrix` sin comprobar nulo en el
  EXE: esos casos usan siempre matriz. Los campos float no tocados por una funcion se rellenan con basura (nunca SNaN).
- No se prueba el cuarto argumento (true) que el EXE pasa a FlyAIHeliToTarget_FixedOrientation (no lo lee).
- `stale` y el parametro `primeDelta` de `Gate` quedaron de una hipotesis descartada (lectura de pila sin inicializar en FlyAIPlane): no tienen efecto (`primeDelta = 0`).
- Compilacion: la fuente se compila con CarCtrl.cpp como antes (/Zi sin RTC) y los ayudantes con /O2 /Oi /arch:IA32 /fp:fast (X87Math.cpp); no se tocaron opciones de CMake.

## Archivos

`Fixtures.hpp` (generador determinista de casos, solo C++ estandar), `Core.hpp` (arena, falsos, ejecucion y compuertas), `LiveComparison.hpp` (rutas RH, colaboradores y `static_assert` del layout),
`run_local.py` (analizador independiente con compuertas y cobertura), `build_probe.py`, `minimums.json` / `minimums_seq.json`, `fixture_mirror.cpp` (volcado de un caso sin el juego: `fm.exe <fn> <n>`),
`callers.py`, `register_hook.py` (plantilla con `SCOPE_NOTE` rellena; SOLO la ejecuta el coordinador tras una corrida aprobada), `source.patch`.

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 pwsh -NoProfile -File D:\Original\wh-1\reversing\CVehicle\build-project.ps1
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarCtrlAir63   # compila la sonda y corre en el juego (candado global)
python -B reversing/CarCtrlAir63/run_local.py --analyze "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlair63/20261020-222954-939361/comparisons.jsonl"
python reversing/CarCtrlAir63/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlair63/20261020-222954-939361"   # SOLO lo ejecuta el coordinador
```
