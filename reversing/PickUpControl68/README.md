<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PickUpControl68: CTaskComplexGoPickUpEntity::ControlSubTask (0x691D50), virtual (lote 68)

Estado: **validado dentro del juego** (26/10/2026, worktree `D:\Original\wy-3`, HEAD 8ff5230a6 + `source.patch`): 16384 pares, cero diferencias (ver la carpeta de evidencia indicada en el informe del coordinador,
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pickupcontrol68\<corrida>`).

| Funcion | Direccion | Hook RH (clase / nombre) | Tipo | Fuente |
|---|---|---|---|---|
| `CTask* CTaskComplexGoPickUpEntity::ControlSubTask(CPed*)` | 0x691D50 (vtable 0x870B98, ranura 10) | `CTaskComplexGoPickUpEntity` / `ControlSubTask` | V | `TaskComplexGoPickUpEntity.cpp/.h`, `X87Math.cpp/.h` |

Fila de `docs/hooks.csv` que debe anadir el coordinador (virtual): `CTaskComplexGoPickUpEntity,ControlSubTask,0x00000000,1,0,1,V`.

## Que hace (desensamblado 0x691D50..0x691FB0, thiscall `ret 4`, EAX = subtarea o nulo)

El cuerpo termina en 0x691FB0; los `ret` de 0x691E0C y 0x691F79 son salidas de la misma funcion (abortar la subtarea) y 0x691FC0 empieza otra funcion (el constructor).

1. Si `m_nAnimGroupId` (+0x2C) != 0 y el byte `m_bAnimBlockReferenced` (+0x30) == 0: bloque = `ms_aAnimAssocGroups[grupo].m_AnimBlock` (0xB4EA34, grupos de 0x14 bytes, sin comprobar el indice); si es nulo,
   `GetAnimationBlock(GetAnimBlockName(grupo))` (0x4D3940 / 0x4D3A30). Indice = `(bloque - 0xB5D4A0) >> 5` (aritmetico). Si el byte `IsLoaded` (+0x10) != 0: `AddAnimBlockRef(indice)` (0x4D3FB0) y `+0x30 = 1`;
   si no: `CStreaming::RequestModel(indice + 0x63E7, 8)` (0x4087E0).
2. Sin subtarea (+8) devuelve nulo. `GetTaskType()` de la subtarea (vtable ranura 4):
   - `0x134` (TASK_SIMPLE_PICKUP_ENTITY): si su `m_pAnimBlendAssociation` (+0x38) es nulo y el byte `m_bEntityDropped` (+0x34) es 0 y `ahora > +0x28 + 8000` (sin signo, 0x8D2FFC) la aborta; si no, devuelve la subtarea.
   - `0x387` (TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL): si `ahora > +0x28 + 5000` (0x8D2FF8) la aborta. Si no, y el ped tiene datos de jugador (+0x480) y `CPlayerPed::GetPadFromPlayer` (0x609560) da un pad:
     lee `GetPedWalkUpDown` (0x53FD30) y luego `GetPedWalkLeftRight` (0x53FC90) como `movsx ax`, longitud = `sqrt(ud^2 + lr^2) * 0.0078125`, rumbo al destino `T = fpatan(-(destino.x - pos.x), destino.y - pos.y)`
     (posicion = `GetPosition()`: matriz si existe, si no la del placement), rumbo de la palanca `A = GetRadianAngleBetweenPoints(0, 0, -lr, ud) - [0xB6F178]`, ajustado: `A -= 2pi` si `T + pi < A`, si no `A += 2pi` si `T - pi > A`
     (comparaciones ordenadas: NaN no ajusta). Luego `JumpJustDown` (0x540770) o `SprintJustDown` (0x5407F0) abortan; si no, aborta cuando `longitud > 0.75` y `A - T > pi/4` (0x859AB0).
   - Otro tipo: devuelve la subtarea.
   Abortar = `MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)` (vtable ranura 6) y devolver nulo.

Cambios de produccion (`source.patch`): `TaskComplexGoPickUpEntity.cpp` (cuerpo C++ puro y `RH_ScopedVMTInstall(ControlSubTask, 0x691D50)`), `.h` (comentario), `X87Math.cpp/.h` (`PickUpEntityHeadingCpp`, ver abajo).

### Defecto encontrado y corregido durante el banco (de produccion, no del banco)

El rumbo `fpatan(-(destino.x - pos.x), destino.y - pos.y)` no puede calcularse con `float` de SSE: (a) el EXE deja las restas en el x87 (exponente extendido), de modo que una diferencia subnormal sigue siendo exacta,
mientras que el camino SSE lo aplana a cero (compatible con MXCSR FTZ/DAZ; 18 diferencias en la primera corrida, familia 15, con destino subnormal); (b) el compilador reescribe `-(a - b)` como `b - a`, lo que cambia el signo de un cero y el cuadrante de
`fpatan` (1 diferencia en la segunda, caso 846: `-(0 - 0) = -0`, `atan2(-0, negativo) = -pi`). Se resolvio con el ayudante `PickUpEntityHeadingCpp` de `X87Math.cpp` (archivo con `/arch:IA32`, restas en `volatile double`).

## Banco

`Original` = bytes del EXE con el hook VMT de `ControlSubTask` apagado (la entrada 0x691D50 sin parchear); `candidato` = DLL con SOLO ese hook encendido (el cuerpo sale de `ReversibleHook::Virtual::GetHookOurAddress()`).
16384 pares (16 familias de 1024). Colaboradores (10, todos con hook RH) sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, que registran sus argumentos en orden: `GetAnimBlockName` 0x4D3A30,
`GetAnimationBlock(const char*)` 0x4D3940, `RequestModel` 0x4087E0, `AddAnimBlockRef` 0x4D3FB0, `CPlayerPed::GetPadFromPlayer` 0x609560, `CPad::GetPedWalkUpDown` 0x53FD30, `GetPedWalkLeftRight` 0x53FC90 (EAX con basura en los
bits altos), `CGeneral::GetRadianAngleBetweenPoints` 0x53CBE0 (devuelve el float en ST0), `CPad::JumpJustDown` 0x540770 y `SprintJustDown` 0x5407F0 (AL con basura). `GetTaskType` y `MakeAbortable` de la subtarea van por una vtable
falsa de arena (ranuras 4 y 6). El atan2 no se sustituye (se valida contra el fpatan del EXE). `ms_aAnimAssocGroups` (0xB4EA34) apunta a un arreglo falso de 256 grupos, y el tiempo del juego (0xB7CB84) y la camara (0xB6F178) se fijan
por caso y se restauran tras cada lado (`globals_restored`). Los bloques con indice negativo son direcciones de `.rdata` alineadas a 0x20 con el byte +0x10 igual a 1 / 0.

Fixtures (`Fixtures.hpp`): sin subtarea, tipos distintos (incluidos `0x134 | 0x10000`, `0x387 | 0x80000000`), recoger con animacion / ya soltado (byte no canonico) / dentro y fuera de plazo (bordes +-1 y desbordamiento sin signo de
`inicio + plazo`), ir al punto fuera de plazo, sin datos de jugador, sin pad, salto, correr, palanca corta (pares alrededor de longitud 96 = 0.75 * 128), palanca larga con el angulo en el borde de pi/4, de `T + pi` y de `T - pi`
(+-3 ulp), NaN, infinitos y subnormales en destino, posicion, angulo y camara, ejes con signo extremo (-32768, 32767) y basura en los bits altos de EAX, bloque no cargado / cargado / ya referenciado, por grupo o por nombre, byte
`IsLoaded` no canonico, indice negativo, grupo 0.

`run_local.py` exige: igualdad EXE/DLL, compuertas de ABI/ruta/DF/SEH/restauracion en ambos lados, `fp_difference` vacio, que EAX del EXE y la secuencia de eventos de los falsos coincidan con el modelo del desensamblado
(`predict` / `events_model`, escritos desde 0x691D50 y comprobados contra el EXE en cada caso), cobertura por familia y de ramas (`coverage.json` = minimos; `fixture_selfcheck.py` los calcula sin juego) y restauracion final.
ECX y EDX solo se registran (no comparados; no se desensamblaron los llamadores de la ranura 10).

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh PickUpControl68                            # compila la sonda y corre en el juego (candado global)
python -B reversing/PickUpControl68/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/PickUpControl68/fixture_selfcheck.py                         # sin juego; necesita vswhere en PATH
python -B reversing/PickUpControl68/register_hook.py <carpeta-de-evidencia>      # SOLO el coordinador, tras una corrida aprobada
```
