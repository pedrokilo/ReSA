<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco WalkRoundNext31: CTaskComplexWalkRoundObject::CreateNextSubTask (0x657220)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerChatSeq30` / `PartnerChatAbort30` (tarea virtual, pool sintético, hook `Virtual`, falsos con parche de entrada en el EXE y en la DLL), `reversing/WalkRoundControl31` (hermano: misma subtarea falsa con vtable propia) y el contrato `reversing/_handoff/BRIEF.md`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh WalkRoundNext31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/WalkRoundNext31/build_probe.py
python -B reversing/WalkRoundNext31/run_local.py
python -B reversing/WalkRoundNext31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/WalkRoundNext31/fixture_oracle.py                                             # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/WalkRoundNext31/fixture_mirror.py                                             # compila fixture_mirror.cpp con cl (sin el juego): 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -B reversing/WalkRoundNext31/register_hook.py <carpetaEvidencia>                           # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(CreateNextSubTask, 0x657220)` (clase `CTaskComplexWalkRoundObject`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `walkroundnext31-runtime.start`; salida `walkroundnext31-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexWalkRoundObject,CreateNextSubTask,0x00000000,1,0,1,V` (la dirección real, 0x00657220, va en `evidence.original_address`).

**Requisitos del árbol al compilar:** `CTask::operator new` (0x61A5A0) y el constructor de `CTaskComplexGoToPointAndStandStill` (0x668120) siguen enganchados a la DLL (`collaborators_dll`); pool de tareas con ≥ 256 ranuras.

Ya comprobado sin el juego (worktree `D:\Original\ws-3`): `fixture_oracle.py` imprime `ok`; `fixture_mirror.py` imprime `ok` (4608 casos aceptados por el oráculo, arenas del C++ iguales a las del oráculo, 0 faltas de cobertura); `Core.hpp`/`LiveComparison.hpp` compilan sin errores con `cl /Zs` y las opciones del proyecto; `source.patch` pasa `git apply --check` contra los blobs de HEAD, y el `.cpp` resultante compila solo (sin las otras dos funciones). **No** se ha enlazado ni ejecutado la sonda.

## Qué hace la función (desensamblado 0x657220..0x657372, `ret 4`, marco SEH propio)

`this` en ECX (→ ESI), `ped` en `[esp+0x28]` (→ EBP). Sin x87 propia. Orden observable:
1. `m_object` (+0x1C) nulo → 0 (sin más llamadas).
2. `GetTaskType` (una vez): **0x387** (`TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL`) → `CreateRouteTask(ped)` (0x655140); **0x386** (`TASK_SIMPLE_ACHIEVE_HEADING`) → siguiente paso; **cualquier otro** (incluidos 0xCB y 0x389) → 0.
3. Con 0x386: contador de la ruta (`m_pointRoute->m_NumEntries`, +0, comparado con 0 como entero de 32 bits) ≠ 0 → `CreateRouteTask(ped)`.
4. Con la ruta vacía: `ComputeClosestSurfacePoint(ped, objeto, &punto)` (0x5F3B70); `lado1 = ComputeEntityHitSide(&punto, objeto)` y `lado2 = ComputeEntityHitSide(&m_targetPoint, objeto)` (0x5F3AC0; comparación de 32 bits). Iguales → `AddUnlessFull(punto)` (0x48E180, en el EXE inline: `if (cnt < 8)`, con signo), `ComputeClosestSurfacePoint(&m_targetPoint, objeto, &punto)` (0x5F36F0), `AddUnlessFull(punto)` y `CreateRouteTask(ped)`. Distintos → `new(0x28)` (0x61A5A0) + `CTaskComplexGoToPointAndStandStill(WALK = 4, &punto, 0.5f (0x86FC84), 2.0f (0x86FC88), false, false)` (0x668120); reserva fallida → 0.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexWalkRoundObject.cpp`: includes (`PedGeometryAnalyser.h`, `TaskComplexGoToPointAndStandStill.h`), `RH_ScopedVMTInstall(CreateNextSubTask, 0x657220)` y el cuerpo. `TaskComplexWalkRoundObject.h`: los tres vectores iniciales (+0x30/+0x3C/+0x48, uniones anónimas) y `VALIDATE_SIZE` 0x54 (el EXE reserva 0x54), necesario porque `source.patch` de los tres bancos comparte el mismo `.h`. `CreateRouteTask` (0x655140) **no** se toca: sigue siendo la llamada por plugin al EXE de HEAD (el borrador del lote 27 la reconstruía; queda fuera de este encargo y el banco la sustituye por un falso). `source-all.patch` = las tres funciones del lote juntas.

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador del lote 27

Tras escribir se volvió a desensamblar 0x657220..0x657372 y se contrastó. El borrador coincidía con el EXE en estructura; **diferencias respecto al borrador:** (1) el `default:` ahora documenta que 0xCB y 0x389 caen en nulo como cualquier otro tipo (el EXE los compara explícitamente pero ambos terminan en `edi = 0`); (2) se quitó la declaración `RH_ScopedInstall(CreateRouteTask, 0x655140)` y su cuerpo (fuera del encargo); (3) includes mínimos. Verificado sin cambios: orden de las llamadas (`ClosestSurfacePoint(ped)` → `HitSide(punto)` → `HitSide(destino)` → `AddUnlessFull` → `ClosestSurfacePoint(destino)` → `AddUnlessFull` → `CreateRouteTask`), que el punto de la primera llamada va también a la tarea de ir al punto, constantes 0x86FC84 = 0.5f y 0x86FC88 = 2.0f leídas del `.rdata` del EXE, `PEDMOVE_WALK = 4` (`push 4`), y que la reserva fallida da 0 (`xor eax,eax`).

| EXE | C++ | Resultado |
|---|---|---|
| 0x65723B..0x657259 | `if (!m_object) return nullptr` | igual |
| 0x65725A..0x65727B | `switch (GetTaskType())` 0x387 / 0x386 / otros | igual |
| 0x657286..0x65728C | contador ≠ 0 → `CreateRouteTask` | igual |
| 0x6572A1..0x6572D8 | `ComputeClosestSurfacePoint`, dos `ComputeEntityHitSide`, comparación de 32 bits | igual |
| 0x6572DA..0x65730E | rama de lados iguales | igual |
| 0x657312..0x65734E | rama de lados distintos: `new(0x28)` + ctor 0x668120 | igual |

**Residuo de registros / llamadores.** `CreateNextSubTask` es la ranura 8 de la vtable (0x86F364): su único llamador es `CTaskManager::SetNextSubTask` (0x681920, `task->CreateNextSubTask(m_pPed)`), en C++ en la DLL con el hook activo, que sólo usa el puntero devuelto (EAX). `residue_scan.py 0x657220` no devuelve llamadas directas. **Nadie lee ECX/EDX**; el banco sólo los registra. El revisor debe confirmarlo.

## Qué se compara (por lado; original == candidato == oráculo)

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido. Cada caso monta: un `this` falso de 0x54 bytes (se compara **entero**: la función no escribe en él), una **subtarea falsa con vtable propia** (`GetTaskType` registra la llamada), un objeto y un peatón falsos (sólo se pasan por puntero), una `CPointRoute` falsa (contador + 8 vectores con 24 palabras aleatorias; guardas) y un **pool sintético** (incluido «ninguna libre»). Observado: `ret`, lista de llamadas (`GT`, `CP`(ped, objeto), `CQ`(x,y,z del destino, objeto), `HS`(x,y,z del punto, objeto), `CR`(this, ped)) con el **contenido** de los vectores pasados (no punteros de pila), `route_after` (las 25 palabras), cursor y estados del pool, la tarea nueva completa, nada fuera de la ranura reservada, `this`/subtarea/objeto/peatón intactos. ABI: ESP, EBX/ESI/EDI/EBP, DF = 0, `fs:[0]` (el EXE instala un marco SEH), x87 normalizado, `route_ok` y `collaborators_dll`. Todo restaurado y comprobado.

**Colaboradores.** *Reales* (ya en C++ y enganchados): `CTask::operator new` 0x61A5A0 y el constructor de `CTaskComplexGoToPointAndStandStill` 0x668120 (con `GoToPoint` 0x46FE60 dentro). *Sustituidos* (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto antes de parchear): `CPedGeometryAnalyser::ComputeClosestSurfacePoint` (ped, 0x5F3B70: escribe un punto P1 de la fixture) y (posición, 0x5F36F0: escribe P2), `ComputeEntityHitSide(punto, entidad)` 0x5F3AC0 (devuelve `lado1` la 1.ª vez y `lado2` la 2.ª), y `CreateRouteTask` 0x655140 **sólo en el EXE** (la DLL la llama por plugin a esa dirección; devuelve un valor fijo).

## Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

`n % 16`: 0 sin objeto; 1 subtarea 0x387 (ruta directa); 2 AchieveHeading con ruta ya cargada; 3 AchieveHeading, ruta vacía, lados iguales; 4 ruta vacía, lados distintos (reserva); 5 otros tipos (0xCB, 0x389, 0x385, 0x388, 0x38A, desconocidos…); 6 contadores de ruta extremos (1…8, 9, 100, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF …); 7 lados en el borde (INT_MIN, INT_MAX, −1, 0x10000 …, también iguales); 8 puntos con flotantes extremos (NaN, inf, FLT_MAX, 0xCDCDCDCD …); 9 `CreateRouteTask` devuelve nulo o valores raros; 10 pool agotado en lados distintos; 11 pool con pocas libres; 12…15 mezclas (con objeto nulo ocasional). Mínimos de `run_local.py` calibrados con `fixture_mirror.py` (≈ 50 % de lo medido).

## Suposiciones y riesgos

- Si la DLL **inlinease** alguno de los colaboradores sustituidos, el falso no se invocaría y saldría diferencia en `calls`.
- La tarea de ir al punto se modela desde el desensamblado de 0x668120: `[+0xC] = 4`, punto, `0.5f`, `2.0f`, byte `[+0x24] = (0xCD & 0xE4) | 4 = 0xC4` (el bit 2, «punto nuevo», lo pone `GoToPoint`; el constructor C++ no inicializa ese bit a propósito). **Si sólo difiere ese byte u otro campo de `tasks`, es un defecto del constructor reconstruido, no de esta función.**
- `CPool::SetDealWithNoMemory(true)` se activa durante el banco (y se restaura).
- `offsetof` y tamaños verificados con `static_assert` en `LiveComparison.hpp` (`sizeof` 0x54, +8, +0xC, +0x10, +0x1C, +0x20; `sizeof(CPointRoute)` 0x64) y las arenas del C++ frente al oráculo en `fixture_mirror.py`.
- Con 8 entradas la función no llama a `AddUnlessFull` (sólo lo hace con contador 0, que nunca está lleno).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
