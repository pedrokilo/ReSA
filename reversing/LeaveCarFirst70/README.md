<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCarFirst70: CTaskComplexLeaveCar::CreateFirstSubTask (0x641FC0), virtual de subtareas de la tarea de bajar del coche (lote 70)

Estado: **VALIDADO dentro del juego** (26/10/2026, worktree `D:\Original\wy-4`, HEAD 1e45e275d de main + `source.patch`): 16384 pares, cero diferencias. Evidencia:
`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leavecarfirst70\20261026-165141-042739`.
Plantillas: `reversing/PickUpFirst68` (hook Virtual, falsos con parche de entrada en el EXE y en la DLL, constructores reales en arena), `reversing/LeaveCar67` (fixtures/autoprueba del oraculo).

| Funcion | Direccion | Hook RH (clase / nombre) | Tipo | Fuente |
|---|---|---|---|---|
| `CTask* CTaskComplexLeaveCar::CreateFirstSubTask(CPed*)` (vtable 0x86E828, ranura 9) | 0x641FC0..0x6421AC | `CTaskComplexLeaveCar` / `CreateFirstSubTask` | V | `TaskComplexLeaveCar.cpp` |

Fila de `docs/hooks.csv` que debe anadir el coordinador (hook virtual: direccion 0x00000000 y tipo V; la direccion real va en `evidence.original_address` del registro):
`CTaskComplexLeaveCar,CreateFirstSubTask,0x00000000,1,0,1,V`

## Que hace (desensamblado, `ret 4`, marco SEH propio, EAX = tarea creada o 0)

1. Arrestado = `[ped+0x530] == 0x3F` (PEDSTATE_ARRESTED) o `[ped+0x474] & 0x40` (bIsBeingArrested). Con arrestado y `CPed::IsPlayer` (solo AL): `CTask::operator new(0x18)` y, si no es nulo, `CTaskSimplePause(-1)`.
2. Si no (no arrestado, o arrestado sin ser jugador): sin `bInVehicle` (`[ped+0x46C]` bit 8) -> `printf("CTaskComplexLeaveCar - ped not in car")` (0x821982 del CRT del juego) y la misma pausa(-1).
3. En coche, si `[vehiculo+0x460]` (conductor) es el ped: `CPedGroups::GetPedsGroup(ped)`; si hay grupo y `grupo->membresia.IsLeader(ped)`: `CEvent::operator new(0x14)` y, si no es nulo, `CEventLeaderExitedCarAsDriver`;
   `CEventGroupEvent` local (ped, evento) + `grupo->inteligencia.AddEvent` (0x5F7470) + destructor (CleanUpOldReference del ped y borrado virtual del evento). Siempre despues (con grupo o sin el, lider o no):
   no jugador -> `ped->SetRadioStation()`; jugador y `m_pVehicle` (`[ped+0x58C]`) no nulo -> `PlayerAboutToExitVehicleAsDriver` del audio de `m_pVehicle`.
4. Final: `[vehiculo+0x590] == 5` (barco) -> `operator new(0x14)` + `CTaskComplexLeaveBoat(vehiculo, 0)`; si no `operator new(0x70)` + `CTaskSimpleCarDriveTimed(vehiculo, [this+0x14])`. Nulo del `operator new` -> 0.

## Fuente (`source.patch`)

- `TaskComplexLeaveCar.cpp`: `RH_ScopedVMTInstall(CreateFirstSubTask, 0x641FC0)`, includes y el cuerpo en C++ (sin `__asm`/`naked`). `CTask::operator new` / `CEvent::operator new` con comprobacion de nulo antes de construir.
  Solo queda una llamada al EXE por direccion: el `printf` del CRT del juego (0x821982), como hace el original (y como `AESoundManager`). El `.h` no cambia. El constructor, el destructor, `ControlSubTask` y `CreateSubTask` (0x641530)
  de la clase no tienen hook en este HEAD y no se tocan; `CreateFirstSubTask` no llama a `CreateSubTask`.
- **Defecto de produccion hallado por el banco** (primera version: 4770/16384 diferencias): el evento se construia con `CEventLeaderExitedCarAsDriver()` (inicializacion por valor), que pone a cero el relleno del objeto
  (bytes +9..+11, +13, +18, +19) que el constructor 0x4B8300 del EXE deja intacto; corregido con inicializacion por defecto (`new (memory) CEventLeaderExitedCarAsDriver`).
- Defectos del banco corregidos antes de la corrida aprobada: el `printf` 0x821982 del EXE tiene un salto a la DLL en HEAD (hooked=true, sin cuerpo aparte de la DLL) y `CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver` 0x4F5BA0
  no tiene hook en HEAD (hooked=false); los constructores reales de barco y coche cronometrado llaman a `RegisterReference` (0x571B70), que el oraculo no esperaba.

## Banco

`Original` = bytes del EXE con el hook RH de la funcion apagado; `candidato` = DLL con SOLO ese hook encendido (`HookRoute` con el cuerpo del hook Virtual). 16384 pares = 16 familias x 1024
(arrestado por estado / por bandera con jugador; arrestado sin jugador; fuera del coche; conductor distinto; sin grupo; no lider; lider con evento, con evento nulo; no jugador; barco con tarea nula; coche con tarea nula;
tipos de vehiculo alrededor de 5 en 32 bits; retardos extremos; estados y banderas alrededor de 0x3F; mezcla al azar). Cada respuesta de `IsPlayer`/`IsLeader` lleva basura en los bits altos (solo cuenta AL) y `IsPlayer` responde
de forma independiente en cada llamada.

- **Sustituidos por el mismo falso en la direccion del EXE y en el cuerpo de la DLL** (cuerpos resueltos antes de parchear; registran sus argumentos EN ORDEN): con hook en HEAD (saltos a la DLL en ambos lados):
  `CPed::IsPlayer` 0x5DF8F0, `CTask::operator new` 0x61A5A0 (arena o nulo), `CPedGroups::GetPedsGroup` 0x5F7E80, `CPedGroupMembership::IsLeader` 0x5F69C0, `CEntity::RegisterReference` 0x571B70,
  `CPedGroupIntelligence::AddEvent` 0x5F7470 (registra los campos del `CEventGroupEvent` local), `CEntity::CleanUpOldReference` 0x571A00, `CPed::SetRadioStation` 0x5DFD90;
  sin hook en HEAD (la entrada del EXE debe seguir siendo codigo original): `CEvent::operator new` 0x4B5620 (arena o nulo), `CEvent::operator delete` 0x4B5630 y `CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver` 0x4F5BA0;
  el `printf` del EXE 0x821982 (salto a la DLL en HEAD) solo se parchea en la direccion del EXE porque la DLL lo llama por esa direccion.
- **Reales en ambos lados**: constructores `CTaskSimplePause` 0x48E750, `CTaskComplexLeaveBoat` 0x63BB60, `CTaskSimpleCarDriveTimed` 0x5FF940 y `CEventLeaderExitedCarAsDriver` 0x4B8300 (saltos a la DLL en ambos lados; construyen en la arena del falso de
  `operator new` y se comparan las palabras), y el `CEventGroupEvent` local (constructor 0x4ADFD0 y destructor 0x4AE070 sin hook: codigo del EXE contra el de la DLL; sus llamadas caen en los falsos).
- Observado por lado: retorno (etiqueta), eventos ordenados con argumentos, palabras de la tarea construida, bytes del evento construido (relleno uniforme: el banco ve exactamente que bytes escribe el constructor), arenas restantes intactas,
  ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, `collaborators_dll`, estado restaurado. EAX/ECX/EDX/EFLAGS de salida solo se registran (los residuos difieren entre EXE y DLL en todos los casos;
  el llamador del EXE, la llamada virtual desde el gestor de tareas, sobrescribe ECX/EDX antes de usarlos).
- `run_local.py` exige ademas que la salida de CADA lado coincida con `fixture_oracle.py` (modelo escrito solo desde el desensamblado), la cobertura de ramas (`coverage.json` = 50 % de lo previsto por `fixture_selfcheck.py`)
  y la restauracion final.
- No cubierto: un vehiculo objetivo nulo (el EXE lo desreferencia sin comprobar); `m_nVehicleType` solo se prueba como palabra de 32 bits (el campo del juego es un enum de 32 bits); el estado real del pool de tareas/eventos
  (se simula agotado con los falsos de `operator new`).

## Comandos

```
CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh LeaveCarFirst70   # compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCarFirst70/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/LeaveCarFirst70/fixture_oracle.py && python -I reversing/LeaveCarFirst70/fixture_selfcheck.py   # autopruebas sin juego (selfcheck necesita vcvars64 con vswhere en el PATH)
python -B reversing/LeaveCarFirst70/register_hook.py <carpeta-de-evidencia>                      # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecarfirst70-runtime.start`; salida `leavecarfirst70-runtime.jsonl`.
