<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerControl60: CTaskComplexPartner::ControlSubTask (0x6840D0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerDealAnims31` (falsos con parche de entrada en el EXE y en la DLL), `reversing/PartnerFirstSub31` (argumento de pila `ret 4`, hook Virtual) y contrato `reversing/_handoff/BRIEF.md`, lecciones del lote 30.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerControl60
# o a mano, siempre en serie:
python -B reversing/PartnerControl60/build_probe.py
python -B reversing/PartnerControl60/run_local.py
python -B reversing/PartnerControl60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerControl60/fixture_oracle.py                                              # «ok»
python -B reversing/PartnerControl60/fixture_mirror.py                                              # compila con cl: arenas/constantes C++ == oráculo + cobertura
python -B reversing/PartnerControl60/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(ControlSubTask, 0x6840D0)` en `CTaskComplexPartner::InjectHooks`. `source.patch` contiene SÓLO los cambios de esta función respecto a HEAD (`git apply --check` contra los blobs de HEAD ok); `source-all.patch` es el `git diff HEAD` completo de las cuatro funciones del lote 60 (mismo `TaskComplexPartner.cpp`/`.h`, árbol `D:\Original\wc-6`). Los otros tres hooks pueden estar encendidos en el ASI sin afectar a este banco (sus llamadas se sustituyen por falsos). Archivo de arranque en el clon: `partnercontrol60-runtime.start`; salida `partnercontrol60-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartner,ControlSubTask,0x00000000,1,0,1,V` (original_address 0x006840D0). Función **compartida** por las vtables 0x870664 (base), 0x8707C4 (Chat), 0x870754 (Deal) y 0x870800 (Shove; ranuras 9-12 verificadas leyendo el EXE).

Ya comprobado sin el juego: `fixture_oracle.py` -> `ok`; `fixture_mirror.py` -> `ok` (4096 casos, arenas iguales, 0 faltas de cobertura); `Core.hpp` compila con `cl` contra cabeceras simuladas; `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `LiveComparison.hpp` ni el proyecto.

## Qué hace la función (desensamblado 0x6840D0..0x68418D, `ret 4`, devuelve `m_pSubTask` o 0)

`this` en ECX (-> ESI), `ped` en la pila (-> EDI). Detalle completo, instrucción por instrucción, en la cabecera de `fixture_oracle.py`. Resumen:
1. No se aborta (salto a 0x684126) si el BYTE **con signo** `m_partnerState` (+0x59) <= 1, hay `m_partner` y el BYTE `m_taskCompleted` (+0x5C) == 0; o si, con partner, `FindActiveTaskByType(&partner->intel->taskMgr, m_taskId)` (0x681740) devuelve una tarea cuyo `m_partner` (+0x38) es `ped` y `m_taskCompleted` == 0.
2. En cualquier otro caso: `m_pSubTask->MakeAbortable(ped, 1, 0)` (vtable +0x18); AL != 0 -> devuelve 0.
3. `m_pSubTask->GetTaskType()` (vtable +0x10) == 0x387 (GoToPointAndStandStill): `++WORD [this+0x70]` (el word es de las clases derivadas) y, si el word **con signo** > 150, otra vez `MakeAbortable`; AL != 0 -> devuelve 0.
4. `ped->DropEntityThatThisPedIsHolding(true)` (0x5E0360); `this->StreamRequiredAnims()` (vtable +0x2C); `g_ikChainMan.IsLooking(ped)` (0x6181A0, 0xC15448); si AL != 0: `AbortLookAt(ped, 500)` (0x618280). Devuelve `m_pSubTask`.

## Fuente (`source.patch`)

`TaskComplexPartner.cpp`: `RH_ScopedVMTInstall(ControlSubTask, 0x6840D0)`, `#include "IKChainManager.h"`, los ayudantes `PartnerByte`, `PartnerTaskOfPartner` y `PartnerCounter70` (acceso al word +0x70 como en el original) y el cuerpo.

## Fase 2: revisión contra el EXE y discrepancias del borrador

Re-desensamblado 0x6840D0..0x68418D y contrastado con el C++ y con el borrador del lote 27: cada salto (`jg 6840EC`, `je 684126`, `jne 684115`...), la rama `partner == 0`, que la llamada a `Find` ocurre en el segundo bloque incluso con estado <= 1 y `m_taskCompleted != 0`, los argumentos de `MakeAbortable` (`push 0; push 1; push edi`), la comparación del contador con signo (`cmp word ptr [esi+70h],96h; jle`), `push 1` para `DropEntityThatThisPedIsHolding`, y `push 1F4h` para `AbortLookAt`. **Discrepancia hallada y corregida:** el borrador probaba `m_taskCompleted` como `bool`; el EXE hace `test al,al` sobre el byte (los no canónicos 2, 0x80, 0xFF cuentan como completado): `PartnerByte`. La lógica (estructura skipAbort/partnerInSync) del borrador es equivalente a la actual, que se reescribió en un único `if/else if` para reproducir exactamente cuándo se llama a `Find`.

**Llamadores / residuo de registros.** La ranura 10 (+0x28) la llama el despachador genérico de tareas complejas; el escaneo `scan_calls.py 0x28` marca sitios de otras clases (0x68DB86, 0x68DBA7, 0x68DC1C...) que sobrescriben ECX con `mov ecx,eax` antes de usarlo, y ninguno del código genérico de tareas. EAX es el retorno (se compara como etiqueta en `observed.ret`); **ningún llamador lee ECX/EDX de salida**: sólo se registran.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido. **Sustituidos** en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto con el hook encendido antes de parchear; los cuatro tienen hook en HEAD): `CTaskManager::FindActiveTaskByType` 0x681740, `CPed::DropEntityThatThisPedIsHolding` 0x5E0360, `IKChainManager_c::IsLooking` 0x6181A0 y `AbortLookAt` 0x618280. Las llamadas **virtuales** (`GetTaskType` y `MakeAbortable` de la subtarea, `StreamRequiredAnims` de `this`) van por vtables falsas de arena (`this` y la subtarea son objetos de arena con su puntero a vtable): las ranuras 4, 6 y 11 son los falsos; cualquier otra ranura de esas vtables registra un evento `trap` que el oráculo no espera (así un cambio de ranura en la DLL salta a la vista). Observado: retorno (etiqueta), secuencia ordenada de eventos con argumentos, cambios de `this` (sólo el word +0x70), el resto de arenas (subtarea, ped, partner, inteligencia, tarea del partner) intactas, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, `collaborators_dll`, estado restaurado.

## Fixtures: 4096 = 16 familias x 256

Estados <= 1 con signo (1, 0, 0x80, 0xFF, 0x81, 0xFE) y > 1 (2..7, 0x7F, 0x10); `m_taskCompleted` 0 / 1 / no canónico; partner nulo / presente; `Find` nulo / tarea con `m_partner == ped` / otro peatón; `MakeAbortable` con AL 0 / 1 / no canónico (2, 0x80, 0xFF) y basura en los 24 bits altos; subtipo 0x387 y cercanos (0x386, 0x388, 0x1387, 0x80000387...); contador en el borde de 150 (148..152), 0x7FFE/0x7FFF/0x8000 (cambio de signo), 0xFF/0x100 (acarreo), 0xFFFF; `IsLooking` con AL variado. Mínimos calibrados (~50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- **Arenas** (suma a mano; las comprueba `fixture_mirror.py`): `this` 0x40 + 0x74 + 0x4C = 0x100; subtarea 0x20 + 0x30 + 0x30 = 0x80; ped 0x10 + 0x20 + 0x10 = 0x40; partner 0x40 + 0x500 + 0x40 = 0x580; inteligencia 0x20 + 0x40 + 0x20 = 0x80; tarea del partner 0x20 + 0x60 + 0x20 = 0xA0.
- El objeto `this` mide 0x74 (como `CTaskComplexPartnerDeal`) porque el word +0x70 es de las derivadas: la base lo usa fuera de su `sizeof` (0x70), como el original.
- La vtable falsa supone que, en la DLL, `StreamRequiredAnims` es la ranura 11, `GetTaskType` la 4 y `MakeAbortable` la 6 (las del EXE); si no, el banco lo delata con eventos `trap` (y una pila desequilibrada en el peor caso, que `stack_restored` detecta).
- `ped` es un puntero opaco (los falsos no lo desreferencian); el partner sólo se lee en +0x47C.
- `IKChainManager_c::AbortLookAt` toma `uint32` de desvanecimiento (500); el falso registra el valor de pila.
- `static_assert` en `LiveComparison.hpp`: tamaños y offsets de `CTaskComplexPartner`; en tiempo de ejecución se exige `&g_ikChainMan == 0xC15448`.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.


## Depuración en el worktree de integración (19/10/2026): APROBADO
Defecto del banco: el falso de `MakeAbortable` alterna `abort1`/`abort2` por orden de llamada; sin la primera llamada (camino que la omite) la segunda usa `abort1`. `fixture_oracle.py` lo modelaba al revés. Sin cambios en producción (el C++ coincide con el EXE).
