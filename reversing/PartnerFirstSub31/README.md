<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerFirstSub31: CTaskComplexPartner::CreateFirstSubTask (0x681F20)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantilla: `reversing/PartnerChatAbort30` (ABI, hook Virtual, falsos con parche de entrada en el EXE y en la DLL) y contrato `reversing/_handoff/BRIEF.md`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerFirstSub31
# o a mano, siempre en serie:
python -B reversing/PartnerFirstSub31/build_probe.py
python -B reversing/PartnerFirstSub31/run_local.py
python -B reversing/PartnerFirstSub31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerFirstSub31/fixture_oracle.py                                              # «ok»
python -I reversing/PartnerFirstSub31/fixture_mirror.py                                              # compila con cl: tamaños de arena C++ == oráculo + cobertura
python -B reversing/PartnerFirstSub31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20)` en `CTaskComplexPartner::InjectHooks` (`source.patch`, sólo esa función, aplicable sobre HEAD). Archivo de arranque en el clon: `partnerfirstsub31-runtime.start`; salida `partnerfirstsub31-runtime.jsonl`. 4608 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartner,CreateFirstSubTask,0x00000000,1,0,1,V` (original_address 0x00681F20 en el registro).

Ya comprobado sin el juego: `fixture_oracle.py` -> `ok`; `fixture_mirror.py` -> `ok` (4608 casos, arenas iguales entre `Fixtures.hpp` compilado y el oráculo, 0 faltas de cobertura); `source.patch` pasa `git apply --check` contra el blob de HEAD; `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `Core.hpp`/`LiveComparison.hpp` con el proyecto.

## Qué hace la función (desensamblado 0x681F20..0x681FCE, `ret 4`, marco SEH propio)

`this` en ECX (ESI), `ped` en la pila (EDI).
1. Si el BYTE `m_leadSpeaker` (+0x58) != 0 **y** el dword `m_taskId` (+0x34) == 0x4B4 (`TASK_COMPLEX_PARTNER_CHAT`) **y** el BYTE +0x74 (`CTaskComplexPartnerChat::m_conversationEnabled`) != 0 (se lee sin comprobar el tipo real del objeto): `CAEPedSpeechAudioEntity::RequestPedConversation(ped, m_partner)` (0x4E50E0, cdecl; sólo AL cuenta). AL != 0 -> byte +0x76 = 1. Si no, byte +0x75 != 0 -> +0x74 = 0; si no -> **devuelve nulo** (sin StopPlayingHandSignal ni reserva).
2. `ped->StopPlayingHandSignal()` (0x5E0480, thiscall).
3. `CTask::operator new(0x20)` (0x61A5A0); nulo -> devuelve nulo; si no `CTaskSimpleStandStill(50, false, false, 8.0f)` (0x62F310) y devuelve el objeto.

## Fuente (`source.patch`)

`TaskComplexPartner.cpp`: includes (`TaskComplexPartnerChat.h`, `TaskSimpleStandStill.h`), `RH_ScopedVMTInstall(CreateFirstSubTask, 0x681F20)` y el cuerpo. La reserva se hace con `CTask::operator new(sizeof(CTaskSimpleStandStill))` + comprobación de nulo + `::new (mem)`, para reproducir la rama de pool agotado del EXE (un `new` simple podría o no comprobar el nulo según `/Zc:throwingNew`). El `.h` no cambia.

## Fase 2: revisión contra el EXE

Tras escribir se desensambló de nuevo 0x681F20..0x681FCE (`dis.sh`) y se contrastó línea a línea con el C++ y con el borrador sin commit del árbol principal (`git diff HEAD`, lote 27):

| EXE | C++ | Resultado |
|---|---|---|
| 0x681F38..0x681F4D | `m_leadSpeaker` (byte), `m_taskId == 0x4B4` (dword), `m_conversationEnabled` (byte) | igual |
| 0x681F54..0x681F72 | `RequestPedConversation(ped, m_partner)`; AL -> `+0x76 = 1`; si no `+0x75` -> `+0x74 = 0`; si no nulo | igual (push partner, push ped = (ped, partner)) |
| 0x681F76 | `ped->StopPlayingHandSignal()` | igual |
| 0x681F7D..0x681FBC | `operator new(0x20)`; nulo -> nulo | el borrador no tenía la rama nula: añadida |
| 0x681F97..0x681FA4 | `CTaskSimpleStandStill(50, false, false, 8.0f)` (`0x32`, `0`, `0`, `0x41000000`) | igual |

El borrador del lote 27 era correcto salvo la rama de pool agotado (usaba `new` sin comprobar nulo). Sin más discrepancias.

**Llamadores / residuo de registros.** Sólo hay llamadas virtuales (ranura +0x24 de `CTaskComplex`): las vtables de `CTaskComplexPartner` (0x870664, ranura en 0x870688), `CTaskComplexPartnerChat` (0x8707C4, ranura 0x8707E8) y la de 0x870824 llevan 0x681F20 directamente (verificado leyendo el EXE), y `CTaskComplexPartnerDeal` (0x6823B0) y `CTaskComplexPartnerGreet` (0x6825A0) son un único `jmp 0x681F20`. `residue_scan.py 0x681F20` no devuelve llamadas directas (sólo esos dos `jmp`, sin lectores). La maquinaria virtual sólo lee EAX; **ningún llamador lee ECX/EDX** (el análisis del lote 31 de 0x6823B0 cubre los 191 sitios `call [reg+24h]`). ECX/EDX sólo se registran.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartner","CreateFirstSubTask",0x681F20)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). **Sustituidos** por el mismo falso en ambos lados (parche de entrada en el EXE **y** en el cuerpo de la DLL, resuelto antes de parchear): `RequestPedConversation` 0x4E50E0 (registra `(ped, partner)` y los bytes +0x74/+0x76/+0x75 de `this` en esa llamada; devuelve un valor del caso con 24 bits altos aleatorios), `StopPlayingHandSignal` 0x5E0480 (registra ped y bytes) y `CTask::operator new` 0x61A5A0 (registra el tamaño; devuelve nulo o un objeto de arena). **Real** en ambos lados: el constructor de `CTaskSimpleStandStill` (0x62F310, ya en C++ y enganchado): el banco compara los bytes del objeto (campos, relleno intacto, guardas). Observado: EAX (etiqueta: nulo / objeto / otro), trazas de los tres falsos, `this` completo (arena 0x100 con guardas; sólo +0x74/+0x76 pueden cambiar), objeto reservado y sus guardas, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, entradas vigiladas (0x4E50E0, 0x5E0480, 0x61A5A0) y estado restaurado.

## Fixtures: 4608 = 16 familias x 288

No jefe; jefe con tipo distinto de 0x4B4 (un bit, cercanos); chat sin conversación; petición aceptada (con +0x76 ya a 1 y a 0); rechazada con field_75 != 0 / = 0 (nulo); retorno AL no canónico (0x80, 0xFF...) y bytes no canónicos; `operator new` nulo; lead no canónico; ped/partner 0, bordes, 0xFFFFFFFF. EAX/EDX/pila sembrados con ocho semillas de borde y aleatorias. Mínimos calibrados (aprox. 50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- **Arenas** (suma a mano; las comprueba `fixture_mirror.py`): `this` 0x40 + 0x78 + 0x48 = 0x100; objeto 0x20 + 0x20 + 0x40 = 0x80.
- Si el compilador de la DLL inlinease `CTask::operator new` (LTCG), el falso del cuerpo de la DLL no se vería: faltaría la traza `[3,...]` en el candidato y el banco falla con claridad (no da falso positivo).
- `ped` y `partner` son números opacos (los falsos no los desreferencian).
- Banco hermano a adaptar por otro agente: `reversing/PartnerDealFirst31` (sustituye 0x681F20 por un falso). Otros bancos que mencionan 0x681F20: `reversing/CTasks7b` (falso `FakePartnerFirst` con parche de entrada sólo en 0x681F20).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
