<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerDealAnims31: CTaskComplexPartnerDeal::StreamRequiredAnims (0x6823C0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerChatSeq30` (ABI, hook Virtual, falso con parche de entrada en el EXE y en la DLL), `reversing/CTaskSimpleJetPack` y `reversing/PickWeap1` (colaboradores reconstruidos: decidir el estado de su hook y resolver su cuerpo antes de parchear) y contrato `reversing/_handoff/BRIEF.md`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerDealAnims31
# o a mano, siempre en serie:
python -B reversing/PartnerDealAnims31/build_probe.py
python -B reversing/PartnerDealAnims31/run_local.py
python -B reversing/PartnerDealAnims31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerDealAnims31/fixture_oracle.py                                              # «ok»
python -I reversing/PartnerDealAnims31/fixture_mirror.py [numBloques]                                 # compila con cl: arenas/constantes C++ == oráculo + cobertura (probado con 4, 120 y 180 bloques)
python -B reversing/PartnerDealAnims31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(StreamRequiredAnims, 0x6823C0)` (`source.patch`; `source-all.patch` aplica además las otras dos funciones de la clase) y los colaboradores 0x65E7F0, 0x4D3990, 0x4087E0 y 0x4D3FB0 **enganchados**; 0x4D3FD0 (`RemoveAnimBlockRef`) no tiene hook en HEAD (el banco exige que su entrada del EXE NO sea un salto, y parchea el EXE y el cuerpo de la DLL; si se engancha, cambiar `hooked` a true en `LiveComparison.hpp`). Archivo de arranque: `partnerdealanims31-runtime.start`; salida `partnerdealanims31-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartnerDeal,StreamRequiredAnims,0x00000000,1,0,1,V` (original_address 0x006823C0 en el registro).

Ya comprobado sin el juego: `fixture_oracle.py` → `ok`; `fixture_mirror.py` → `ok` (4096 casos, arenas y constantes iguales, 0 faltas de cobertura); `source.patch` pasa `git apply --check` contra HEAD; `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `Core.hpp`/`LiveComparison.hpp` con el proyecto.

## Qué hace la función (desensamblado 0x6823C0..0x68242F, `ret`, void)

`this` en ECX (→ ESI); lee el **byte** `m_requiredAnimsStreamedIn` (+0x6E) y pasa `&m_animBlockName` (+0x5E) al índice de bloque.
- Byte = 0: `ShouldLoadGangAnims` (0x65E7F0); si false: fin. `idx = GetAnimationBlockIndex(&this[0x5E])` (0x4D3990, cdecl); byte `IsLoaded` de `ms_aAnimBlocks[idx]` (0xB5D4A0 + idx·0x20 + 0x10) ≠ 1 (byte entero): `CStreaming::RequestModel(idx + 0x63E7, 8)` (0x4087E0) y fin; = 1: `AddAnimBlockRef(idx)` (0x4D3FB0), byte +0x6E = 1.
- Byte ≠ 0 (cualquier valor): `ShouldLoadGangAnims`; si true: fin. `idx = GetAnimationBlockIndex(...)`; `RemoveAnimBlockRef(idx)` (0x4D3FD0); byte +0x6E = 0.

## Fuente (`source.patch`, sólo esta función)

`TaskComplexPartnerDeal.cpp`: include `TaskComplexGangLeader.h`, `RH_ScopedVMTInstall(StreamRequiredAnims, 0x6823C0)` y el cuerpo. El borrador del lote 27 es idéntico al de `CTaskComplexPartnerGreet::StreamRequiredAnims` (0x6825B0, ya en HEAD).

## Fase 2: revisión contra el EXE y discrepancias del borrador

Re-desensamblado 0x6823C0..0x682430 y contrastado instrucción a instrucción: ramas, orden de llamadas (ShouldLoad → índice → IsLoaded → Request/Add; ShouldLoad → índice → Remove), argumentos (`push 8; push eax` = `(idx + 0x63E7, STREAMING_KEEP_IN_MEMORY = 8)`), el 0x63E7 = `RESOURCE_ID_IFP` = 25575 = `IFPToModelId(0)`, el `cmp dl,1`, y que las rutas «fin» no escriben nada. **Discrepancia hallada y corregida:** el borrador probaba `!m_requiredAnimsStreamedIn` sobre un `bool`; el EXE hace `test al,al` sobre el byte entero (bytes no canónicos 2, 0x80, 0xFF cuentan como «con referencia»). Ahora `reinterpret_cast<const uint8&>(m_requiredAnimsStreamedIn) == 0`. El resto del borrador coincide (incluida la comparación `IsLoaded != 1` sobre el byte).

**Residuo de registros / llamadores.** Único llamador del EXE: `CTaskComplexPartner::ControlSubTask` (0x6840D0), `call [edx+2Ch]` en 0x684166 (ranura 11 de la vtable 0x870754 = 0x6823C0, verificado); tras la llamada hace `push edi; mov ecx,0C15448h; call 6181A0`: **ECX se sobrescribe, EDX y EAX no se leen** (la función es `void`). Un escaneo de todo `.text` (106 sitios `call [reg+2Ch]`) no halla en el código de tareas otro lector de ECX/EDX. ECX/EDX/EAX de salida sólo se registran.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido. **Sustituidos** por falsos idénticos en ambos lados (parche de entrada en la dirección del EXE y en el cuerpo de la DLL, resuelto con el hook encendido antes de escribir ningún parche): `ShouldLoadGangAnims` 0x65E7F0 (devuelve un valor con AL ∈ {0,1} y los 24 bits altos de EAX aleatorios), `GetAnimationBlockIndex(const char*)` 0x4D3990 (registra el puntero como etiqueta `T+94` y devuelve el índice del caso), `CStreaming::RequestModel` 0x4087E0, `AddAnimBlockRef` 0x4D3FB0 y `RemoveAnimBlockRef` 0x4D3FD0 (registran sus argumentos). Observado: la **secuencia ordenada de eventos con argumentos**, `this_changes` (sólo el byte +0x6E: 1 al referenciar, 0 al soltar), entrada de `CAnimBlock[idx]` intacta, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta (`route_ok`), `collaborators_dll` (cuatro son saltos a la DLL y 0x4D3FD0 sigue siendo código del EXE; los cinco con cuerpo de la DLL en el ASI, antes de parchear), estado restaurado. El byte `IsLoaded` del bloque devuelto se escribe con el valor del caso (la entrada completa de 0x20 bytes se guarda y se restaura tras cada lado).

## Fixtures: 4096 = 16 familias × 256

Byte de referencia 0 / 1 / no canónico (2, 3, 0x7F, 0x80, 0x81, 0xFE, 0xFF, 0x10, aleatorio ≠ 0); `ShouldLoad` falso/verdadero (con basura en los bits altos de EAX); `IsLoaded` 0 / 1 / no canónico (pares e impares); índice 0, último (`numAnimBlocks-1`, leído de 0xB4EA30 en el juego), mitad y aleatorio; familias 8–11 combinan los índices de borde con las cuatro rutas. Mínimos calibrados (≈ 50 %) con `fixture_mirror.py` (120 bloques; también ok con 4 y 180).

## Suposiciones y riesgos

- Se escribe temporalmente el byte `IsLoaded` de un bloque real (entrada restaurada tras cada lado, en el mismo fotograma; si el hilo de streaming lo leyera justo entonces habría un glitch, no una diferencia del banco). No se prueba el índice −1 (nombre no hallado): el EXE leería `0xB5D490`, antes del array, y no se escribe fuera de `ms_aAnimBlocks`.
- `ShouldLoadGangAnims` sólo devuelve AL canónico (0/1), como el original; los bits altos de EAX son basura a propósito.
- Si la DLL inlinease algún colaborador (hoy están en otras unidades de traducción), el falso no se invocaría y saldría diferencia en `events`.
- El C++ indexa `CAnimManager::GetAnimBlocks()[blkIdx]` (vista `take`) sin comprobación, como el EXE; los fixtures sólo usan índices dentro de `numAnimBlocks`.
- `static_assert` en `LiveComparison.hpp`: `sizeof` 0x74, `m_animBlockName` +0x5E, `m_requiredAnimsStreamedIn` +0x6E, `CAnimBlock` 0x20 con `IsLoaded` +0x10, `RESOURCE_ID_IFP` = 0x63E7, `STREAMING_KEEP_IN_MEMORY` = 8; las constantes de arena del C++ frente al oráculo en `fixture_mirror.py`.
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerDeal.cpp/.h`, `TaskComplexPartner.h`, `TaskComplexGangLeader`, `AnimManager`, `AnimBlock.h`, `Streaming`, `Task.cpp`, los hooks `Simple`/`Virtual` y los archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
