<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# LeaveCarAbort70: CTaskComplexLeaveCar::MakeAbortable 0x641100 (virtual, vtable 0x86E828 ranura 6, lote 70)

Estado: **validado dentro del juego** (26/10/2026): 16384 pares, cero diferencias, todas las compuertas en ambos lados y modelo del desensamblado = EXE. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-leavecarabort70\20261026-162539-159914` (una corrida previa, 20261026-162014-448954, fallo solo por la compuerta `collaborators_dll`: defecto del banco; el hook RH de `CVector::Magnitude` no esta activo y los hooks V de CloseDoor::MakeAbortable / SetPedOut::ProcessPed si saltan en la entrada del EXE). Worktree `D:\Original\wy-1`, HEAD 1e45e275d + `source.patch`.

| Funcion | Direccion | Hook (clase / nombre / tipo) | Fuente |
|---|---|---|---|
| `bool CTaskComplexLeaveCar::MakeAbortable(CPed*, eAbortPriority, const CEvent*)` | 0x641100 (vtable 0x86E828 + 6*4) | `CTaskComplexLeaveCar` / `MakeAbortable` / V (`RH_ScopedVMTInstall`) | `Tasks/TaskTypes/TaskComplexLeaveCar.cpp` |

Fila de `docs/hooks.csv` (la anade el coordinador): `CTaskComplexLeaveCar,MakeAbortable,0x00000000,1,0,1,V`.

## Que hace (0x641100..0x64152A, thiscall `ret 0xC`, AL = resultado)

1. Sin vehiculo (+0xC): true.
2. Prioridad 2 (inmediata): aborta la subtarea (sin comprobarla) con prioridad 2. Si el ped esta en un vehiculo (bit 8 de +0x46C): calcula la puerta si es 0 (`ComputeTargetDoorToExit` 0x64F110), crea en la pila un `CTaskSimpleCarCloseDoorFromOutside(veh, puerta, 0)`, llama a su `MakeAbortable` (0x64B020, directo, prioridad 2), crea un `CTaskSimpleCarSetPedOut(veh, puerta, m_bSensibleLeaveCar)` y llama a su `ProcessPed` (0x647D10); se destruyen en orden inverso. Siempre suelta las banderas de puerta (`ClearGettingOutFlags` 0x6D3060) y resta `m_nNumGettingInSet` a `veh->m_nNumGettingIn`; devuelve true.
3. Prioridad distinta de 1 y 2: false. Prioridad 1: `GetTaskType` de la subtarea se llama en cada comprobacion (hasta 5 veces) y `GetEventType` en cada comprobacion de evento:
   - 0xD4 (die): si vuelve a dar 0x2CF, false; si no, aborta con prioridad 1 y, si acepta, suelta el vehiculo; devuelve el AL de la subtarea.
   - 0x329 (wait to slow down) o 0xCE (get up and stand still) con evento de dano (9) que mato al ped (`HasKilledPed` 0x4ABCA0: bytes +0x40 y +0xC): igual que 0xD4.
   - 0x32E (jump out): sin evento, false. Con evento de aire (0x21) o agua (0x42): marca `m_bIsInAir` si es de aire, aborta la subtarea (si no acepta, false). Con arma activa (byte con signo +0x718, sin comprobar rango) = paracaidas (0x2E), evento de aire otra vez y `GetNumContactWheels()` (vtable 55) == 0: avion (subtipo 4) con `|v| > 0.2f` (0x858CC4) salta directo; si no, solo un jugador sin suelo a 10 m (`ProcessVerticalLine`) continua; `BlendAnimation(clump, 0, 0x7D, 8.0f)`. En otro caso, evento de agua y `vel.z < -0.3f` (0x858EE4): `BlendAnimation(clump, 0, 0x7D, 16.0f)`. Siempre false.
   - Resto (incluido 0x32E con otro evento): sin evento false; si `GetTaskType` da 0x2CF aborta y devuelve su AL; si no, false.

Cambios de produccion (`source.patch`): `TaskComplexLeaveCar.cpp` solo (includes, `RH_ScopedVMTInstall(MakeAbortable, 0x641100)` y el cuerpo en C++ puro). Las tareas temporales se llaman con calificacion explicita (`closeDoor.CTaskSimpleCarCloseDoorFromOutside::MakeAbortable`, `setPedOut.CTaskSimpleCarSetPedOut::ProcessPed`) como el `call` directo del EXE; el RpClump se pasa sin `GetRpClump` (que comprobaria el tipo).

## Banco

`Original` = bytes del EXE con el hook V apagado; `candidato` = DLL con SOLO ese hook encendido. 16384 pares (16 familias de 1024). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (12): BlendAnimation(id), ProcessVerticalLine, ClearGettingOutFlags, Magnitude, IsPlayer, ComputeTargetDoorToExit, ctor y dtor de CloseDoorFromOutside (con hook; cuerpo = primera llamada de la envoltura), su MakeAbortable (VMT), ctor / dtor (sin hook) y ProcessPed (VMT) de SetPedOut. El constructor de la DLL de SetPedOut tiene un 4o argumento (`warpingOutOfCar`): falso propio (parche solo en la DLL) que registra ese argumento, y el del EXE registra 0. Los cuerpos de la DLL sin hook se hallan con una funcion sonda y se comprueban con una autoprueba antes de empezar (`probe_selftest`). `GetTaskType`/`MakeAbortable` de la subtarea, `GetEventType` del evento y `GetNumContactWheels` van por vtables falsas de arena; los falsos devuelven un valor distinto segun el NUMERO de llamada. `HasKilledPed` corre real en ambos lados.
Se compara AL (los llamadores virtuales hacen `test al,al`), cambios en 6 arenas (tarea, subtarea, vehiculo, ped con prefijo para el indice de arma con signo, matriz, evento), traza de eventos y hash de globales; ECX/EDX solo se registran. `run_local.py` contrasta AL y la traza del EXE con un modelo escrito desde el desensamblado (`model`) y exige cobertura por familia y de ramas (`coverage.json`, 50 % de lo previsto por `fixture_selfcheck.py`).

No cubierto: subtarea nula con prioridad 1/2 (el EXE desreferencia nulo); la validez de los colaboradores sustituidos.

## Comandos

```
pwsh -NoProfile -File reversing\CVehicle\build-project.ps1                       # CMAKE_BUILD_PARALLEL_LEVEL=1, _CL_=-MP1
bash reversing/_handoff/bench_here.sh LeaveCarAbort70                            # compila la sonda y corre en el juego (candado global)
python -B reversing/LeaveCarAbort70/run_local.py --analyze <carpeta>/comparisons.jsonl
python -I reversing/LeaveCarAbort70/fixture_selfcheck.py                         # sin juego
python -B reversing/LeaveCarAbort70/register_hook.py <carpeta-de-evidencia>      # SOLO el coordinador, tras una corrida aprobada
```
Archivo de arranque en el clon: `leavecarabort70-runtime.start`; salida `leavecarabort70-runtime.jsonl`.
