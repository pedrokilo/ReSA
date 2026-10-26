<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco WalkRoundControl31: CTaskComplexWalkRoundObject::ControlSubTask (0x6575F0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerChatSeq30` y `reversing/PartnerChatAbort30` (tarea virtual, pool sintético, hook `Virtual`, falsos con parche de entrada en el EXE y en la DLL) y el contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh WalkRoundControl31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/WalkRoundControl31/build_probe.py
python -B reversing/WalkRoundControl31/run_local.py
python -B reversing/WalkRoundControl31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/WalkRoundControl31/fixture_oracle.py                                             # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/WalkRoundControl31/fixture_mirror.py                                             # compila fixture_mirror.cpp con cl (sin el juego): 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -B reversing/WalkRoundControl31/register_hook.py <carpetaEvidencia>                           # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(ControlSubTask, 0x6575F0)` (categoría/clase `CTaskComplexWalkRoundObject`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `walkroundcontrol31-runtime.start`; salida `walkroundcontrol31-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexWalkRoundObject,ControlSubTask,0x00000000,1,0,1,V` (la dirección real, 0x006575F0, va en `evidence.original_address` del registro).

**Requisitos del árbol al compilar** (el banco falla con mensaje claro si no se cumplen): `CTask::operator new` (0x61A5A0) y el constructor de `CTaskSimpleStandStill` (0x62F310) siguen enganchados a la DLL (`collaborators_dll`); pool de tareas con ≥ 256 ranuras (el juego: 500); la variable estática 0x86C938 vale 30.

Ya comprobado sin el juego (worktree `D:\Original\ws-3`): `fixture_oracle.py` imprime `ok`; `fixture_mirror.py` (cl, pool de 500) imprime `ok` con los 4608 casos aceptados por el oráculo, arenas del C++ iguales a las del oráculo y 0 faltas de cobertura; **prueba cruzada del oráculo con la x87 real** (`WalkRoundMovedCpp` y `WalkRoundDotBelowCpp` compilados con `/O2 /Oi /arch:IA32 /fp:fast`, FCW de 24 bits, sobre las 4608 entradas del banco): 0 diferencias frente al modelo de Python (las tres decisiones aborto-por-movimiento, frente y derecha); `Core.hpp`/`LiveComparison.hpp` compilan sin errores con `cl /Zs` y las opciones del proyecto (sintaxis y semántica, sin enlazar ni ejecutar); `source.patch` pasa `git apply --check` contra los blobs de HEAD. **No** se ha enlazado ni ejecutado la sonda (riesgo: comportamiento de los falsos dentro del juego; el coordinador lo verá en `probe-build.log` y el primer registro).

## Qué hace la función (desensamblado 0x6575F0..0x657742, `ret 4` normal, sin marco SEH)

`this` en ECX (→ ESI), `ped` en `[esp+8]` (→ EBX). Orden observable:
1. `m_object` (+0x1C) nulo → aborto (salto a 0x657718).
2. `GetTaskType` de la subtarea (`call [[esi+8]+0x10]`); si es 0x389 (`TASK_COMPLEX_FOLLOW_POINT_ROUTE`) **y** el byte `field_2C` (+0x2C) ≠ 0: si el byte `field_2D` (+0x2D) ≠ 0 → `field_24 = reloj (0xB7CB84)` y `field_2D = 0`; después, aborto si **no** `reloj < field_24 + field_28` (suma y comparación de 32 bits **sin signo**, con envoltura).
3. Contador del peatón `[[ped+0x47C]+0x274]` (`m_AnotherStaticCounter`, int32) **mayor que** la variable estática 0x86C938 (= 30), comparación **con signo** → aborto.
4. Segundo `GetTaskType`; si es 0xCB (`TASK_SIMPLE_STAND_STILL`) → devuelve la subtarea (nada más).
5. Si el objeto no tiene matriz (+0x14): `AllocateMatrix` (0x54F560, this = objeto) y `CSimpleTransform::UpdateMatrix` (0x54F1B0, this = objeto+4, argumento la matriz nueva). Después posición = matriz+0x30 (siempre existe ya), frente = matriz+0x10, derecha = matriz+0x00.
6. x87 con FCW de 24 bits y **sin guardar floats intermedios**: `dx,dy,dz = inicio − actual`; `(dz·dz + dy·dy) + dx·dx` contra 0.0625 (0x86F15C): aborto si **mayor** (comparación ordenada: con NaN no aborta). Luego `(fwd.z·sf.z + fwd.y·sf.y) + sf.x·fwd.x` contra 0.9 (0x86F160): aborto si **menor** (ordenado). Luego `DotProduct(&derechaInicial, &matriz)` (0x40FDB0: `(a.z·b.z + a.y·b.y) + a.x·b.x`) contra 0.9: aborto si menor (ordenado). Si ninguna aborta, devuelve la subtarea.
7. Aborto (0x657718): `MakeAbortable(ped, 1 (URGENT), 0)` (`call [[esi+8]+0x18]`, sólo cuenta **AL**); si es falso devuelve la subtarea; si no, `CreateSubTask(0xCB, ped)` (0x655290): `new(0x20)` (0x61A5A0) + `CTaskSimpleStandStill(500, false, false, 8.0f)` (0x62F310); si la reserva falla devuelve 0.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexWalkRoundObject.cpp`: includes (`TaskSimpleStandStill.h`, `X87Math.h`), `RH_ScopedVMTInstall(ControlSubTask, 0x6575F0)` y el cuerpo. `TaskComplexWalkRoundObject.h`: lo estrictamente necesario, los tres vectores iniciales (+0x30/+0x3C/+0x48, uniones anónimas para que el constructor, que no los escribe, no los inicialice) en lugar de `field_30`, y `VALIDATE_SIZE` 0x54 (el EXE reserva 0x54: `push 54h` en 0x4B9300). `X87Math.cpp/.h`: bloque «Lote 31» al final con `WalkRoundMovedCpp` y `WalkRoundDotBelowCpp` (`__declspec(noinline)`, `float_control(precise, on)`: se comprobó el código generado, `fld/fsub/fmul/faddp/fcomp/fnstsw/test ah` como el EXE). El resto del proyecto compila los `float` en SSE, por eso la geometría va a `X87Math` (exponente extendido de la x87 de 24 bits). `source-all.patch` = las tres funciones del lote juntas (para `WalkRoundNext31` y `WalkRoundFirst31`).

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador del lote 27

Tras escribir se volvió a desensamblar 0x6575F0..0x657742 y se contrastó. **Discrepancias del borrador sin commit (nunca compilado) corregidas:**
1. **Geometría en SSE.** El borrador calculaba `distSq` y los productos escalares en `float` del código del proyecto (SSE): sin el exponente extendido de la x87 (p. ej. `1e30·1e30 + (−1e30·1e30)` da NaN en SSE y 0 en la x87, y con 0.9 cambia la decisión). Ahora van a `X87Math` (`WalkRoundMovedCpp`, `WalkRoundDotBelowCpp`) y el oráculo del banco las modela en aritmética exacta con redondeo a 24 bits tras cada operación.
2. **Llamada al EXE.** El borrador creaba la tarea de reposo con `plugin::CallMethodAndReturn(0x655290)`; ahora es `new CTaskSimpleStandStill(500, false, false, 8.0f)` (0x6552F1..0x65532E: `push 41000000h/0/0/1F4h` → ctor 0x62F310; reserva fallida → 0), sin llamadas al EXE.
3. **Desbordamiento con signo.** `(uint32)(field_24 + field_28)` sumaba como `int32` (comportamiento sin definir); ahora `(uint32)field_24 + (uint32)field_28`.
4. Sin más diferencias: orden de las llamadas (dos `GetTaskType`, el segundo sólo si no se abortó antes; `AllocateMatrix`+`UpdateMatrix` antes de leer la posición), la escritura de `field_24`/`field_2D` aunque luego se aborte, `CTimer` leído dos veces, constantes 0x86F15C = 0.0625 / 0x86F160 = 0.9f (0x3F666666) / 0x86C938 = 30 leídas del `.rdata`/`.data` del EXE, argumentos de `MakeAbortable` (ped, 1, 0).

| EXE | C++ | Resultado |
|---|---|---|
| 0x6575F8..0x6575FE | `if (!m_object) abort` | igual |
| 0x657604..0x65764A | `GetTaskType() == FOLLOW && field_2C` → reinicio de plazo + comparación sin signo | corregido (3) |
| 0x65764A..0x65765C | contador (`jg`, con signo) contra 30 | igual |
| 0x657662..0x65766F | segundo `GetTaskType() == STAND_STILL` → subtarea | igual |
| 0x657675..0x657692 | `AllocateMatrix` + `UpdateMatrix` si falta la matriz | igual |
| 0x657692..0x657716 | movimiento, frente, derecha | corregido (1) |
| 0x657718..0x657742 | `MakeAbortable`, tarea de reposo | corregido (2) |

**Residuo de registros / llamadores.** `ControlSubTask` es la ranura 10 de la vtable de la clase (0x86F364; ranura 7 = `SetSubTask` 0x61A430, 8 = `CreateNextSubTask`, 9 = `CreateFirstSubTask`): su único llamador es `CTaskManager::ParentsControlChildren` (0x681A80, `ctask->ControlSubTask(m_pPed)`), que está en C++ en la DLL (hook activo) y sólo usa el puntero devuelto, EAX. `residue_scan.py 0x6575F0` no devuelve llamadas directas. **Nadie lee ECX/EDX de salida**; el banco sólo los registra (`kCompareVolatileRegisters = false`). El revisor debe confirmarlo.

## Qué se compara (por lado; original == candidato == oráculo)

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexWalkRoundObject","ControlSubTask",0x6575F0)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). Cada caso monta en memoria propia: un `this` falso de 0x54 bytes (se compara entero; sólo pueden cambiar `field_24` y `field_2D`), una **subtarea falsa con vtable propia** (`GetTaskType` y `MakeAbortable` registran cada llamada; `MakeAbortable` devuelve un `u32` con basura en los bytes altos para comprobar que sólo cuenta AL), un objeto falso (posición del placement distinta de la de la matriz, para detectar lecturas equivocadas), una matriz falsa, un peatón falso con su `CPedIntelligence` falsa (contador), el reloj (0xB7CB84, fijado por caso y restaurado) y un **pool de tareas sintético** (estados con ids de borde, `m_LastFreeSlot`, qué ranuras están libres, incluido «ninguna libre»). Observado: `ret` (etiqueta), lista ordenada de llamadas (`GT`, `MA`(ped, prioridad, evento), `AM`, `UM`), `last_after`, cambios de estado del pool, la tarea nueva completa (0x80 bytes, vtable sustituida por `GetTaskType`), nada fuera de la ranura reservada, cambios en `this` (byte a byte), `m_matrix` del objeto, subtarea/objeto/matriz/peatón/inteligencia intactos, reloj intacto. ABI: ESP, EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, `fs:[0]`, x87 normalizado (FCW de 24 bits, pila vacía), ruta (`route_ok`) y `collaborators_dll`. Todo restaurado y comprobado (`globals_restored`).

**Colaboradores.** *Reales* (ya en C++ y enganchados, se ejecutan en ambos lados): `CTask::operator new` 0x61A5A0 y el constructor de `CTaskSimpleStandStill` 0x62F310. *Sustituidos* (`collaborators_stubbed=true`, parche de entrada en el EXE **y** en el cuerpo de la DLL, resuelto antes de parchear, mismo falso en ambos lados): `CPlaceable::AllocateMatrix` 0x54F560 (pone `m_matrix` = la matriz falsa y registra la llamada) y `CSimpleTransform::UpdateMatrix` 0x54F1B0 (sólo registra).

## Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

`n % 16`: 0 sin objeto; 1 subtarea de ruta con `field_2C = 0`; 2 ruta con plazo (reinicio de `field_2D`); 3 plazo en el borde (envoltura de 32 bits, `reloj = f24 + f28 + {−3..3}`); 4 contador en el borde (−1, 0, 29, 30, 31, 32, INT_MIN, INT_MAX …); 5 reposo (retorno anticipado, la matriz no se toca); 6 reposo con contador alto; 7 sin matriz; 8 posición en el borde (|Δ|² cerca de 0.0625); 9 frente en el borde de 0.9 (0x3F666666 ± 1 ulp); 10 derecha en el borde; 11 todo en orden; 12 flotantes extremos (NaN, ±inf, FLT_MAX, denormales, 2^24 ± 1 …); 13 `MakeAbortable` con bytes altos / AL variados; 14 pool casi agotado en abortos; 15 mezcla. Mínimos de `run_local.py` calibrados con `fixture_mirror.py` (≈ 50 % de lo medido).

## Suposiciones y riesgos

- Si la DLL **inlinease** `CPlaceable::AllocateMatrix` o `CSimpleTransform::UpdateMatrix` en `ControlSubTask`, los falsos no se invocarían y saldría diferencia en `calls` (en el lado DLL).
- `CTimer::m_snTimeInMilliseconds` se sobrescribe por caso y se restaura; todo el banco corre en un fotograma.
- `CPool::SetDealWithNoMemory(true)` se activa durante el banco (y se restaura): sin él, la reserva fallida del pool en la DLL ejecuta `__debugbreak`.
- Si el único byte distinto en `tasks` es de `CTaskSimpleStandStill` (+0x14/+0x15/+0x18/+0x19, bytes de relleno 0xCD), es un defecto de ese constructor reconstruido y no de esta función (el oráculo modela 0x62F310 desde el EXE).
- `offsetof` y tamaños verificados con `static_assert` en `LiveComparison.hpp` (`sizeof` 0x54, +8, +0x1C, +0x24/28/2C/2D, +0x30/3C/48, `CPed::m_pIntelligence` +0x47C, contador +0x274, `CEntity::m_matrix` +0x14 y `m_placement` +4) y las arenas del C++ frente al oráculo en `fixture_mirror.py`.
- La subtarea falsa usa el orden de la vtable de `CTask` del C++ (GetTaskType = ranura 4, MakeAbortable = ranura 6), que coincide con las ranuras del EXE (offsets 0x10 y 0x18).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
