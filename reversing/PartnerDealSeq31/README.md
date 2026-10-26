<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerDealSeq31: CTaskComplexPartnerDeal::GetPartnerSequence (0x682440)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantilla: `reversing/PartnerChatSeq30` y `reversing/PartnerShoveSeq28` (pool sintético, referencias, hook Virtual) y contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerDealSeq31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PartnerDealSeq31/build_probe.py
python -B reversing/PartnerDealSeq31/run_local.py
python -B reversing/PartnerDealSeq31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/PartnerDealSeq31/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PartnerDealSeq31/fixture_mirror.py                                              # compila fixture_mirror.cpp con cl: 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -B reversing/PartnerDealSeq31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol con `RH_ScopedVMTInstall(GetPartnerSequence, 0x682440)` (`source.patch` lo añade a HEAD; `source-all.patch` aplica además las otras dos funciones de la clase: bancos `PartnerDealFirst31` y `PartnerDealAnims31`, cuyos parches individuales tocan líneas contiguas y se aplican mejor combinados). Archivo de arranque en el clon: `partnerdealseq31-runtime.start`; salida `partnerdealseq31-runtime.jsonl`. 4096 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexPartnerDeal,GetPartnerSequence,0x00000000,1,0,1,V` (original_address 0x00682440 en el registro).

**Requisitos del árbol al compilar** (el banco falla con mensaje claro si no se cumplen): los colaboradores reales (`CTask::operator new` 0x61A5A0, constructores 0x632BD0 / 0x66B890 / 0x61A8B0, `AddTask` 0x632D10, `RegisterReference` 0x571B70) deben seguir enganchados a la DLL (`collaborators_dll`); `CReferences::pEmptyList` no nulo con ≥ 8 referencias libres; pool de tareas con ≥ 256 ranuras (el juego: 500).

Ya comprobado sin el juego (worktree `D:\Original\ws-2`): `fixture_oracle.py` imprime `ok`; `fixture_mirror.py` (cl, pool de 500) imprime `ok` con 4096 casos aceptados por el oráculo, tamaños de arena iguales en `Fixtures.hpp` y el oráculo y 0 faltas de cobertura; `source.patch` pasa `git apply --check` contra el blob de HEAD; `run_local.py --analyze` de un registro inexistente devuelve `None`. **No** se ha compilado `Core.hpp`/`LiveComparison.hpp` con el proyecto (riesgo de errores de compilación del banco: el coordinador lo verá en `probe-build.log`).

## Qué hace la función (desensamblado 0x682440..0x682597, `ret` normal, marco SEH propio)

`this` en ECX (→ EDI). Lee `m_partner` (+0x38) y el **byte** `m_leadSpeaker` (+0x58); no escribe en `this`. Orden observable:
1. `new(0x40)` (0x61A5A0) + `CTaskComplexSequence` (0x632BD0).
2. `m_leadSpeaker` (byte entero) ≠ 0 o = 0 sólo cambia la animación: `new(0x28)` + `CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f ([0x86FC7C]), 0.02f (0x3CA3D70A))` (0x66B890); `AddTask` (0x632D10) aunque el Turn sea nulo; `new(0x20)` + `CTaskSimpleRunAnim(ANIM_GROUP_GANGS = 0x34, 0x11F (lead ≠ 0, DEALER_DEAL) / 0x120 (lead = 0, DRUGS_BUY), 4.0f (0x40800000 empujado como entero), false)` (0x61A8B0); `AddTask` otra vez.
3. Devuelve la secuencia (EAX = ESI).

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexPartnerDeal.cpp`: includes (`TaskComplexSequence.h`, `TaskComplexTurnToFaceEntityOrCoord.h`, `TaskSimpleRunAnim.h`), `RH_ScopedVMTInstall(GetPartnerSequence, 0x682440)` y el cuerpo. El `.h` no cambia (ya declara `virtual GetPartnerSequence()`).

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador

Se volvió a desensamblar 0x682440..0x682597 (`dis.sh`) y se contrastó con el borrador sin commit del lote 27 del árbol principal. **El borrador era correcto en estructura, argumentos y orden**; endurecimientos aplicados:
1. **`m_leadSpeaker` como byte entero.** El borrador usaba `m_leadSpeaker ? A : B` sobre un `bool` (el compilador puede asumir 0/1 y construir `288 - lead`); el EXE hace `test al,al` sobre el byte. Ahora `reinterpret_cast<const uint8&>(m_leadSpeaker) != 0`. El banco prueba bytes no canónicos (2, 3, 0x10, 0x7F, 0x80, 0x81, 0xFE, 0xFF).
2. Constantes leídas del EXE: 0x86FC7C = 0.5f, 0x3CA3D70A = 0.02f (= literal `0.02f`), 0x40800000 = 4.0f; enums `ANIM_GROUP_GANGS` = 52 = 0x34, `ANIM_ID_DEALER_DEAL` = 287 = 0x11F, `ANIM_ID_DRUGS_BUY` = 288 = 0x120.

| EXE | C++ | Resultado |
|---|---|---|
| 0x682459..0x68247E | `new CTaskComplexSequence` | igual |
| 0x682482..0x6824C6 / 0x682506..0x682539 | `AddTask(new Turn(m_partner, 0.5f, 0.02f))` | igual (los dos caminos del byte son idénticos salvo la animación) |
| 0x6824D1..0x682504 / 0x682545..0x682574 | `new CTaskSimpleRunAnim(GANGS, lead ? 0x11F : 0x120, 4.0f, false)` + `AddTask` | igual |
| 0x682584..0x682597 | retorno de la secuencia | igual |

**Residuo de registros / llamadores.** Único llamador del EXE: `CTaskComplexPartner::CreateNextSubTask` 0x683AD0, `call [eax+34h]` en 0x683B46 (ranura 13 de la vtable 0x870754, que contiene 0x682440: verificado en el EXE) → `jmp 0x6840AC` (`mov ecx,[esp+30h]` …): **nadie lee ECX/EDX tras la llamada**; sólo EAX (el retorno), que el banco compara como etiqueta. ECX/EDX de salida sólo se registran. Mismo sitio de llamada que `PartnerShoveSeq28`/`PartnerChatSeq30` (ya comprobado allí); el revisor puede reconfirmarlo a mano.

## Qué se compara (por lado; original == candidato == oráculo)

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartnerDeal","GetPartnerSequence",0x682440)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). Cada caso monta, en memoria propia: un `this` falso de 0x74 bytes (palabras aleatorias; se fijan partner y el byte lead), un **partner falso** de 0x40 bytes (tipo/flags/IPL a voluntad, 0..2 referencias previas encadenadas, a veces ya registrada la del Turn), un **pool de tareas sintético** (estados con ids de borde, `m_LastFreeSlot`, qué ranuras están libres; el cuerpo de las ranuras reales se restaura siempre). Observado: `ret` (etiqueta de la 1.ª ranura), `last_after`, cambios de estado del pool, **cada tarea nueva completa** (0x80 bytes con punteros como etiquetas `W<k>+off`/`E+0`/`N<j>`/`R<i>`; vtable sustituida por `GetTaskType`), nada fuera de las ranuras reservadas, `this_changes` (debe ser vacío), partner (sólo `m_pReferences`), cadena de referencias, `pEmptyList` y registros de `aRefs`. ABI: ESP, EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, `fs:[0]`, x87 normalizado, ruta (`route_ok`) y `collaborators_dll`. Todo restaurado y comprobado (`globals_restored`).

**Colaboradores.** Todos *reales* (ya en C++ y enganchados; se ejecutan en ambos lados): operator new 0x61A5A0, constructores 0x632BD0 / 0x66B890 / 0x61A8B0, `AddTask` 0x632D10, `RegisterReference` 0x571B70. No hay sustituidos. El banco prueba la secuencia, el orden y los argumentos de las llamadas, los fallos de reserva, la lógica de pool y las referencias; **no** valida los cuerpos de los constructores (los bytes de las tareas los modela el oráculo desde su desensamblado: si falla **sólo** `tasks` en un campo de un constructor —p. ej. el byte +0x0C de `CTaskSimpleRunAnim`— es un defecto de ese constructor reconstruido y no de esta función; `run_local.py` informa el campo distinto).

## Fixtures (`Fixtures.hpp`): 4096 casos = 16 familias × 256

`n % 16`: 0 lead sin partner; 1 no lead sin partner; 2 lead peatón; 3 no lead peatón; 4 bytes lead de borde con peatón; 5 peatón con 1–2 referencias previas (a veces la del Turn ya registrada); 6 vehículo/objeto/tipos 0,5,6,7; 7 edificios (sin IPL no registra; con IPL/temporal/procedural sí); 8–11 pool casi agotado (1, 2, 3 libres o vuelta del cursor); 12–15 mezcla. El partner puede ser nulo también con lead = 0 (esta función no lo desreferencia). Mínimos de `run_local.py` calibrados con `fixture_mirror.py` (≈ 50 % de lo medido).

## Suposiciones y riesgos

- No se prueba el agotamiento total del pool (el EXE llamaría a `AddTask` con `this = 0` y se detiene).
- `CPool::SetDealWithNoMemory(true)` se activa durante el banco (y se restaura): sin él, la reserva fallida del pool en la DLL ejecuta `__debugbreak`.
- Si algún constructor reconstruido (`CTaskSimpleRunAnim` 0x61A8B0 incluye la base `CTaskSimpleAnim`) difiere del EXE, el fallo aparece sólo en `tasks`.
- `offsetof`/`sizeof` verificados con `static_assert` en `LiveComparison.hpp` (`sizeof` 0x74, +0x38, +0x58) y las arenas del C++ frente al oráculo en `fixture_mirror.py` (sumas a mano: this 0x40+0x74+0x4C = 0x100; partner 0x40+0x40+0x80 = 0x100; nodos 0x20+0x18+0x48 = 0x80).
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerDeal.cpp/.h`, `TaskComplexPartner.h`, `TaskComplexSequence`, `TaskComplexTurnToFaceEntityOrCoord`, `TaskSimpleRunAnim`, `TaskSimpleAnim`, `Task.cpp`, `Pool.h`, los hooks `Simple`/`Virtual` y los archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
