<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerRemove60: CTaskComplexPartner::RemoveStreamedAnims (0x682370)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantilla: `reversing/PartnerDealAnims31` y contrato `reversing/_handoff/BRIEF.md`, lecciones del lote 30.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerRemove60
# o a mano, siempre en serie:
python -B reversing/PartnerRemove60/build_probe.py
python -B reversing/PartnerRemove60/run_local.py
python -B reversing/PartnerRemove60/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PartnerRemove60/fixture_oracle.py                                              # «ok»
python -B reversing/PartnerRemove60/fixture_mirror.py                                              # compila con cl: arenas/constantes C++ == oráculo + cobertura
python -B reversing/PartnerRemove60/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedVMTInstall(RemoveStreamedAnims, 0x682370)` en `CTaskComplexPartner::InjectHooks`. `source.patch` contiene SÓLO los cambios de esta función respecto a HEAD (`git apply --check` contra los blobs de HEAD ok); `source-all.patch` es el `git diff HEAD` completo de las cuatro funciones del lote 60 (mismo `TaskComplexPartner.cpp`/`.h`, árbol `D:\Original\wc-6`). Los otros tres hooks pueden estar encendidos en el ASI sin afectar a este banco (sus llamadas se sustituyen por falsos). Archivo de arranque en el clon: `partnerremove60-runtime.start`; salida `partnerremove60-runtime.jsonl`. 4096 pares. Fila de `docs/hooks.csv`: `CTaskComplexPartner,RemoveStreamedAnims,0x00000000,1,0,1,V` (original_address 0x00682370 en el registro). Esta función es **compartida**: las vtables 0x870664 (base), 0x8707C4 (Chat), 0x870754 (Deal) y 0x870800 (Shove) apuntan a 0x682370 en la ranura 12, así que el hook cubre también `CTaskComplexPartnerChat`/`Deal`/`Shove`.

Ya comprobado sin el juego: `fixture_oracle.py` -> `ok`; `fixture_mirror.py` -> `ok` (4096 casos, arenas iguales, 0 faltas de cobertura; también con 4 y 180 bloques); `Core.hpp` compila con `cl` contra cabeceras simuladas; `--analyze` de un registro inexistente devuelve `None`. **No** se compiló `LiveComparison.hpp` ni el proyecto.

## Qué hace la función (desensamblado 0x682370..0x6823A7, `ret`, void)

`this` en ECX (-> EBP).
- Byte `m_requiredAnimsStreamedIn` (+0x6E) == 0: fin sin llamadas.
- Byte != 0 (cualquier valor) y primer byte de `m_animBlockName` (+0x5E) == 0 (`repe cmpsb` de 1 byte contra 0x858B54): byte +0x6E = 0 sin llamadas.
- Byte != 0 y nombre no vacío: `idx = GetAnimationBlockIndex(&this[0x5E])` (0x4D3990) y `RemoveAnimBlockRef(idx)` (0x4D3FD0, `add esp,8` tras la pareja); byte +0x6E = 0.
Esto es el equivalente exacto de la rama «con referencia» de `CTaskComplexPartnerDeal::StreamRequiredAnims`, sin `ShouldLoadGangAnims`.

## Fuente (`source.patch`)

`TaskComplexPartner.cpp`: `RH_ScopedVMTInstall(RemoveStreamedAnims, 0x682370)`, el ayudante `PartnerByte` y el cuerpo.

## Fase 2: revisión contra el EXE y discrepancias del borrador

Re-desensamblado y contrastado instrucción a instrucción: orden (índice antes que Remove; el byte se limpia después), `je 6823A6` sobre el byte entero, nombre vacío. **Discrepancia hallada y corregida:** el borrador probaba el byte como `bool` (`if (!m_requiredAnimsStreamedIn)`); el EXE usa `test al,al`: ahora `PartnerByte`. Además el borrador declaraba la ranura como `plugin::CallMethod<0x682370, CTask*>` (tipo de `this` distinto, irrelevante); se elimina con la reconstrucción.

**Llamadores / residuo de registros.** La ranura 12 (+0x30) sólo se llama desde `call [reg+30h]`; el escaneo (`scan_calls.py 0x30`) marca 0x674CFE (otra clase; tras la llamada carga `mov ecx,[esi+8]`), 0x4CBDC8 y similares (otras clases, `xor ecx,ecx` antes de leer) y no halla ningún sitio del código de tareas del compañero que lea ECX/EDX. La función es `void`: **ningún llamador lee EAX/ECX/EDX de salida**; sólo se registran.

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido. **Sustituidos** por el mismo falso en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, resuelto antes de parchear): `GetAnimationBlockIndex(const char*)` 0x4D3990 (hook encendido) y `RemoveAnimBlockRef` 0x4D3FD0 (**sin hook en HEAD**: `hooked=false`; el banco exige que su entrada del EXE NO sea un salto a la DLL y parchea EXE y cuerpo de la DLL; si se engancha, cambiar `hooked` a `true` en `LiveComparison.hpp`). Observado: secuencia ordenada de eventos con argumentos, `this_changes` (sólo +0x6E a 0), `CAnimBlock[idx]` intacto, ABI (ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado), ruta, `collaborators_dll`, estado restaurado.

## Fixtures: 4096 = 16 familias x 256

Byte de referencia 0 / 1 / no canónico; nombre vacío / no vacío (bytes altos incluidos); índice 0 / último / mitad / aleatorio; las familias 6 y 7 repiten las rutas con efecto con otro relleno. Mínimos calibrados (~50 %) con `fixture_mirror.py`.

## Suposiciones y riesgos

- **Arenas** (suma a mano; las comprueba `fixture_mirror.py`): `this` 0x40 + 0x70 + 0x50 = 0x100.
- Se escribe temporalmente el byte `IsLoaded` de un bloque real (la función no lo lee; restaurado tras cada lado).
- Si la DLL inlinease `GetAnimationBlockIndex`, falta el evento y el banco falla con claridad.
- `static_assert` en `LiveComparison.hpp`: tamaño de `CTaskComplexPartner`, offsets +0x5E/+0x6E, `CAnimBlock`.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
