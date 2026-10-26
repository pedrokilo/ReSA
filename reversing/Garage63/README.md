<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Garage63: nueve funciones de CGarage (todas menos Update)

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wf-1`, HEAD 35e7b9d4b + fuente del lote 63). 36864 pares (9 x 4096), cero diferencias EXE/DLL, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente; el puntero real del pool de vehículos (0xB74494) quedó restaurado. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-garage63\20261020-093018-576104`.

**`CGarage::Update` (0x44AA50) NO está reconstruida.** Son ~1400 instrucciones con tabla de saltos por tipo de garaje (45 tipos, 3 tablas), ~40 colaboradores distintos (jugador, cámara, audio, script, partículas, pools, textos, cheats...), x87 en línea y varios cuerpos de máquina de estados; no se puede contrastar de forma razonable con este método. Su hook sigue comentado (`// RH_ScopedInstall(Update, 0x44AA50)`) y `Update` sigue llamando al EXE. Tampoco se toca el ayudante sin clase de Camera.cpp (0x449FF0).

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `InitDoorsAtStart` | 0x447600 | Simple | 4096 | ninguno |
| `PlayerArrestedOrDied` | 0x4486C0 | Simple (envoltura `GaragePlayerArrestedOrDiedRegs`, nombre «PlayerArrestedOrDied») | 4096 | ninguno |
| `IsEntityEntirelyInside3D` | 0x448BE0 | Simple | 4096 | GetColModel 0x535300, GetMatrix 0x411990, Transform 0x59C890, IsPointInsideGarage(radio) 0x4487D0 |
| `IsEntityEntirelyOutside` | 0x448D30 | Simple | 4096 | GetColModel, AllocateMatrix 0x54F560, CSimpleTransform::UpdateMatrix 0x54F1B0, Transform, IsPointInsideGarage(radio) |
| `IsStaticPlayerCarEntirelyInside` | 0x44A830 | Simple | 4096 | FindPlayerVehicle 0x56E0D0, FindPlayerPed 0x56E210, CTaskManager::FindActiveTaskByType 0x681740, IsEntityEntirelyInside3D 0x448BE0 |
| `IsGarageEmpty` | 0x44A9C0 | Simple | 4096 | CWorld::FindObjectsIntersectingCube 0x568DD0, IsEntityTouching3D 0x448EE0 |
| `RemoveCarsBlockingDoorNotInside` | 0x449690 | Simple | 4096 | IsEntityTouching3D, IsPointInsideGarage(punto) 0x448740, CVehicle::CanBeDeleted 0x6D1180, CWorld::Remove 0x563280, destructor virtual |
| `TidyUpGarage` | 0x449C50 | Simple | 4096 | IsPointInsideGarage(punto), CWorld::Remove, destructor virtual |
| `TidyUpGarageClose` | 0x449D10 | Simple | 4096 | IsEntityTouching3D, GetColModel, Transform, IsPointInsideGarage(radio), CWorld::Remove, destructor virtual |

Cada colaborador con hook en la DLL se parchea en la entrada del EXE **y** en el cuerpo de la DLL (los dos `IsPointInsideGarage` por puntero a miembro); sus hooks quedan apagados durante el banco.

## Registros (EAX/ECX/EDX)

- `PlayerArrestedOrDied`: **un llamador del EXE los lee**. El bucle de 0x449E60 hace `add ecx,0D8h` y al final `mov [0x96C01C],edx` tras cada llamada, así que ECX (= this) y EDX de entrada quedan intactos (envoltura con `ResidueRegsCpp`, como `GarageIsPointInsideRegs`); el banco los exige (`ecx` = garaje, `edx` = semilla de entrada).
- El resto: ningún llamador del EXE lee ECX/EDX (comprobado con `callers.py`/desensamblado: 0x448B7A, 0x44A1F1, 0x44BDDF, 0x44CA23/0x44CA2E son saltos/llamadas que no los leen). Las funciones bool devuelven AL (los llamadores hacen `test al,al` / `cmp al,1`); se compara AL y el resto de EAX y ECX/EDX solo se registran.

## Fuente (cambios de producción)

`Garage.cpp`/`Garage.h`: hooks activados para las nueve; `PlayerArrestedOrDied` pasa a `void` (el EXE no devuelve nada); `__declspec(noinline)` en `IsEntityTouching3D` y los dos `IsPointInsideGarage` (para que los falsos del banco intercepten la DLL). `X87Math.cpp/.h` (bloque «Lote 63» al final): `Garage63InsideRectStrictCpp` (IsEntityEntirelyOutside) y `Garage63StaticSpeedCpp` (IsStaticPlayerCarEntirelyInside); `IsEntityEntirelyInside3D` reutiliza `GarageEntityNearCpp` (mismas seis comparaciones que IsEntityTouching3D) y `GarageDiffCpp`/`RpNanAddCpp`. `source.patch` contiene SOLO estos cuatro archivos respecto de HEAD.

## Revisión propia contra el EXE (fase 2)

Se desensambló de nuevo cada función y se contrastó línea a línea con los borradores del lote 27 (`git diff HEAD` del árbol principal). Discrepancias halladas y corregidas:

1. `IsEntityEntirelyOutside`: el borrador usaba `GetMatrix()` y comparaba el rectángulo con `<=`/SSE. El EXE comprueba el rectángulo con comparaciones **estrictas** (NaN pasa a la fase de esferas), crea la matriz **dentro** del bucle (AllocateMatrix + UpdateMatrix, sin pasar por 0x411990) y la suma `radio + radio de esfera` va con el x87 guardada como float. Corregido.
2. `IsEntityEntirelyInside3D`: el borrador usaba SSE para los seis límites y la resta; el EXE usa el mismo x87 que IsEntityTouching3D y comprueba `m_pColData == nullptr` (devuelve true). Corregido.
3. `IsStaticPlayerCarEntirelyInside`: el borrador tomaba la velocidad con SSE y llamaba menos veces a FindPlayerVehicle; el EXE la pide 2 veces (3 si el tipo no es 0), después FindPlayerPed/FindActiveTaskByType y otra vez FindPlayerVehicle; |v| con `fchs` solo si v < 0 ordenado y suma `(z² + y²) + x²` con el x87. Corregido (`Garage63StaticSpeedCpp`).
4. `TidyUpGarage` / `TidyUpGarageClose` (sin borrador): bucle de índice `size-1` hasta **1** (el 0 no se mira; con size 1 ni una vuelta; contador sin signo), matriz leída antes de IsPointInsideGarage (EDX se conserva), estado WRECKED = byte +0x36 & 0xF8 == 0x28, umbral `up.z < 0.5f` ordenado; en Close las esferas con matriz sin crear, contador de esferas releído, sin esferas no se elimina.
5. `RemoveCarsBlockingDoorNotInside`: orden de las comprobaciones (Touching, Inside(posición), bloqueado, CanBeDeleted) y tamaño leído una vez confirmados; el borrador era correcto.
6. `PlayerArrestedOrDied`/`InitDoorsAtStart`: tablas de saltos de 45 entradas decodificadas del EXE (tipos 2..5 y 13 frente al resto; el tipo 13 no toca nada en Init); el borrador coincidía, pero el valor de retorno no existe en el EXE y faltaba conservar ECX/EDX.
7. `IsGarageEmpty`: esquinas y argumentos de `FindObjectsIntersectingCube` (false, true, true, false, false) confirmados; el borrador era correcto salvo el `return` temprano a la llamada del EXE que dejaba código muerto.

Defectos reales de producción que halló el banco: **ninguno** (la primera corrida completa dio 0 diferencias EXE/DLL). Los fallos intermedios fueron defectos del banco: (a) los falsos de `IsPointInsideGarage` no conservaban ECX/EDX, de los que dependen 0x448BE0, 0x449C50 y 0x449D10 (el EXE original fallaba con AV); (b) una colisión de nombres `kRemove` (evento / índice de función) y un contador de 16 entradas con índice 16; (c) el CColModel falso apuntaba al CCollisionData de la imagen de referencia, no al de trabajo; (d) el cuerpo de la DLL de `CSimpleTransform::UpdateMatrix` no se parcheaba (categoría RH distinta): se usa su dirección por puntero a miembro.

## Qué compara

`Original` = bytes del EXE con el hook RH apagado; `candidato` = DLL con **solo** ese hook encendido. Por lado: eventos de los falsos EN ORDEN con argumentos (punteros como fichas `G`, `V<i>`, `M<i>`, `S<i>.<j>`, `L`...), cambios de TODA la arena del banco (garaje con guardas, vehículos, CCollisionData; cualquier cambio fuera de lo esperado aparece como región `X`), AL de las funciones bool, ECX/EDX de PlayerArrestedOrDied, y compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 normalizado FCW 24 bits pila vacía, ruta del hook, entradas de los colaboradores sin tocar, puntero del pool restaurado). Los falsos bool devuelven `bit | 0xA5A5A500` para forzar que la DLL use solo AL.

`fixture_oracle.py` modela las nueve funciones desde el desensamblado con aritmética x87 a 24 bits y exponente extendido emulada con fracciones exactas (NaN, infinitos, subnormales, desbordes, igualdad exacta en los límites y ±1 ulp); `python -I fixture_oracle.py` ejecuta la autoprueba. `fixture_mirror.py` compila `Fixtures.hpp` con `cl` sin el juego, pasa los 36864 casos por el oráculo, fija las constantes de disposición (suma a mano en el script) y comprueba que el `Rand` de C++ coincide con el de Python. Cobertura mínima en `minimums.json` (≈50 % de lo medido por el espejo) más 256 casos por familia (12 familias por función).

## Suposiciones y límites

- Todos los colaboradores son falsos: no se validan sus cuerpos ni la integración con pools, matrices o colisión reales; solo el orden, los argumentos y el uso del resultado por la función.
- No se prueba `TidyUpGarage` con vehículo no WRECKED y matriz nula, ni `TidyUpGarageClose` con `CColModel` sin `CCollisionData` (el EXE desreferencia sin comprobar y daría AV), ni `size` del pool 0 (el contador sin signo daría la vuelta), ni más de 8 esferas por vehículo ni más de 8 vehículos.
- Los NaN de los fixtures son siempre silenciosos (0x7FC00000): las esquinas de FindObjectsIntersectingCube pasan en parte por el x87 en el EXE (un NaN señalizador se silenciaría).
- La reentrada (los falsos cambian el contador de esferas a mitad de bucle) cubre `IsEntityEntirelyInside3D`, `IsEntityEntirelyOutside` y `TidyUpGarageClose`.
- El banco deja en el registro eventos «stage» de depuración (inofensivos).

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 pwsh -NoProfile -File D:\Original\wf-1\reversing\CVehicle\build-project.ps1
bash reversing/_handoff/bench_here.sh Garage63             # compila la sonda y corre en el juego (candado global)
python -I reversing/Garage63/fixture_oracle.py             # autoprueba del oráculo
python -I reversing/Garage63/fixture_mirror.py             # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/Garage63/run_local.py --analyze <comparisons.jsonl>
python reversing/Garage63/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `minimums.json`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch`.
