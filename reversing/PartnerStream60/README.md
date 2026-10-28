<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerStream60: CTaskComplexPartner::StreamRequiredAnims (0x682310)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantilla: `reversing/PartnerDealAnims31` (misma estructura; allí es la variante de `CTaskComplexPartnerDeal`, 0x6823C0) y contrato `reversing/_handoff/BRIEF.md`, lecciones del lote 30.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerStream60
# o a mano, siempre en serie:
python -B reversing/PartnerStream60/build_probe.py
python -B reversing/PartnerStream60/run_local.py
python -B reversing/PartnerStream60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerStream60/fixture_oracle.py                                              # «ok»
python -B reversing/PartnerStream60/fixture_mirror.py                                              # compila con cl: arenas/constantes C++ == oráculo + cobertura
python -B reversing/PartnerStream60/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(StreamRequiredAnims, 0x682310)` en `CTaskComplexPartner::InjectHooks`. `source.patch` contiene SÓLO los cambios de esta función respecto a HEAD (`git apply --check` contra los blobs de HEAD ok); `source-all.patch` es el `git diff HEAD` completo de las cuatro funciones del lote 60 (mismo `TaskComplexPartner.cpp`/`.h`, árbol `D:\Original\wc-6`). Los otros tres hooks pueden estar encendidos en el ASI sin afectar a este banco (sus llamadas se sustituyen por falsos). Archivo de arranque en el clon: `partnerstream60-runtime.start`; salida `partnerstream60-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartner,StreamRequiredAnims,0x00000000,1,0,1,V` (original_address 0x00682310 en el registro). Colaboradores **enganchados** (hook encendido en HEAD): 0x4D3990, 0x4087E0 y 0x4D3FB0.

Ya comprobado sin el juego: `fixture_oracle.py` -> `ok`; `fixture_mirror.py` -> `ok` (4096 casos, arenas y constantes iguales entre `Fixtures.hpp` compilado y el oráculo, 0 faltas de cobertura; también con 4 y 180 bloques); `Core.hpp` compila con `cl` contra cabeceras simuladas (sintaxis); `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `LiveComparison.hpp` ni el proyecto.

## Qué hace la función (desensamblado 0x682310..0x68236C, `ret`, void)

`this` en ECX (-> EBP). Lee el **byte** `m_requiredAnimsStreamedIn` (+0x6E) y el primer byte de `m_animBlockName` (+0x5E, `repe cmpsb` de 1 byte contra la cadena vacía 0x858B54).
- Byte +0x6E != 0 (cualquier valor): fin sin llamadas.
- Byte = 0 y nombre vacío: byte +0x6E = 1 sin llamadas.
- Byte = 0 y nombre no vacío: `idx = GetAnimationBlockIndex(&this[0x5E])` (0x4D3990, cdecl); si el BYTE `IsLoaded` de `ms_aAnimBlocks[idx]` (0xB5D4A0 + idx·0x20 + 0x10) != 1: `CStreaming::RequestModel(idx + 0x63E7, 8)` (0x4087E0) y fin (sin marcar); si == 1: `AddAnimBlockRef(idx)` (0x4D3FB0) y byte +0x6E = 1.
La diferencia con `CTaskComplexPartnerDeal::StreamRequiredAnims` (0x6823C0, banco `PartnerDealAnims31`) es que la base no consulta `ShouldLoadGangAnims` y trata el nombre vacío como «nada que cargar».

## Fuente (`source.patch`)

`TaskComplexPartner.cpp`: `RH_ScopedVMTInstall(StreamRequiredAnims, 0x682310)`, el ayudante `PartnerByte` (prueba el byte entero de un `bool`) y el cuerpo.

## Fase 2: revisión contra el EXE y discrepancias del borrador

Re-desensamblado 0x682310..0x68236C y contrastado instrucción a instrucción con el C++ y con el borrador sin commit del lote 27 (`git diff HEAD` del árbol principal): ramas, orden de llamadas (índice -> IsLoaded -> Request/Add), argumentos (`push 8; push eax` = `(idx + 0x63E7, STREAMING_KEEP_IN_MEMORY)`; `0x63E7` = `RESOURCE_ID_IFP`), `cmp dl,1` sobre el byte entero, y que la rama «nombre vacío» marca el byte sin llamar a nada. **Discrepancia hallada y corregida:** el borrador probaba `m_requiredAnimsStreamedIn` como `bool`; el EXE hace `test al,al` sobre el byte (los valores no canónicos 2, 0x80, 0xFF cuentan como «con referencia»). Ahora `PartnerByte(m_requiredAnimsStreamedIn)`. El resto coincide.

**Llamadores / residuo de registros.** Único llamador del EXE en código de tareas: `CTaskComplexPartner::ControlSubTask` (0x6840D0, `call [edx+2Ch]` en 0x684166, ranura 11 de las vtables 0x870664 y 0x8707C4), que sobrescribe ECX con `mov ecx,0C15448h` y no lee EDX ni EAX. El escaneo `PartnerDealAnims31/scan_calls.py 0x2c` sobre todo `.text` marca otros sitios `call [reg+2Ch]` (0x642FF2, 0x662561, 0x67B5EF...) que pertenecen a otras clases y sobrescriben ECX antes de usarlo. La función es `void`: **ningún llamador lee EAX/ECX/EDX de salida**; sólo se registran.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartner","StreamRequiredAnims",0x682310)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). **Sustituidos** por el mismo falso en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto con el hook encendido antes de escribir ningún parche): `GetAnimationBlockIndex(const char*)` 0x4D3990 (registra el puntero como etiqueta `T+94` y devuelve el índice del caso), `CStreaming::RequestModel` 0x4087E0 y `AddAnimBlockRef` 0x4D3FB0 (registran sus argumentos). Observado: la **secuencia ordenada de eventos con argumentos**, `this_changes` (sólo el byte +0x6E: a 1), entrada de `CAnimBlock[idx]` intacta, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta (`route_ok`), `collaborators_dll` (los tres son saltos a la DLL y sus cuerpos están en el ASI), estado restaurado. El byte `IsLoaded` del bloque devuelto se escribe con el valor del caso (entrada de 0x20 bytes guardada y restaurada).

## Fixtures: 4096 = 16 familias x 256

Byte de referencia 0 / 1 / no canónico (2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF, 0x10, aleatorio); nombre vacío (primer byte 0) / no vacío (con bytes altos 0x80-0xFF); `IsLoaded` 0 / 1 / no canónico (pares e impares); índice 0, último (`numAnimBlocks-1`, leído de 0xB4EA30), mitad y aleatorio. Mínimos calibrados (~50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- Se escribe temporalmente el byte `IsLoaded` de un bloque real (entrada restaurada tras cada lado, en el mismo fotograma). No se prueba el índice −1 (el EXE leería antes del array).
- **Arenas** (suma a mano; las comprueba `fixture_mirror.py`): `this` 0x40 + 0x70 + 0x50 = 0x100.
- Si la DLL inlinease algún colaborador (hoy están en otras unidades de traducción), el falso no se invocaría y saldría diferencia en `events`.
- `static_assert` en `LiveComparison.hpp`: `sizeof(CTaskComplexPartner)` 0x70, `m_animBlockName` +0x5E, `m_requiredAnimsStreamedIn` +0x6E, `CAnimBlock` 0x20 con `IsLoaded` +0x10, `RESOURCE_ID_IFP` = 0x63E7, `STREAMING_KEEP_IN_MEMORY` = 8.
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartner.cpp/.h`, `AnimManager`, `AnimBlock.h`, `Streaming`, `Task.cpp`, los hooks `Simple`/`Virtual` y los archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
