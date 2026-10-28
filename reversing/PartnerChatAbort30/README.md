<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PartnerChatAbort30: CTaskComplexPartnerChat::MakeAbortable (0x682C60)

**Estado: PREPARADO** (sin compilar ni ejecutar: lo corre el coordinador en serie). Plantillas: `reversing/PartnerShoveSeq28` (Gate, hook Virtual, estructura de ficheros), `reversing/PedSaveExtract28` (falsos con parche de entrada en el EXE y en el cuerpo de la DLL) y el contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PartnerChatAbort30          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PartnerChatAbort30/build_probe.py
python -B reversing/PartnerChatAbort30/run_local.py
python -B reversing/PartnerChatAbort30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/PartnerChatAbort30/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/PartnerChatAbort30/fixture_mirror.py                                              # compila fixture_mirror.cpp con cl (sin el juego): 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -B reversing/PartnerChatAbort30/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(MakeAbortable, 0x682C60)` (categoría/clase `CTaskComplexPartnerChat`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `partnerchatabort30-runtime.start`; salida `partnerchatabort30-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexPartnerChat,MakeAbortable,0x00000000,1,0,1,V` (no la añade `register_hook.py`, que no toca `docs/`).

Ya comprobado sin el juego (en el worktree `D:\Original\wc-4`): `fixture_oracle.py` imprime `ok`; `fixture_mirror.py` (cl) imprime `ok` con 4608 casos aceptados, tamaños de arena idénticos entre `Fixtures.hpp` y el oráculo y 0 faltas de cobertura; `source.patch` pasa `git apply --check` contra `git show HEAD:source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp`. **No** se ha compilado `Core.hpp`/`LiveComparison.hpp` con el proyecto.

## Qué hace la función (desensamblado 0x682C60..0x682C9E, `ret 0Ch`, sin marco SEH, guarda ESI)

`bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)`; `this` en ECX (→ ESI).
1. `ecx = [esi+8]` (`m_pSubTask`, **sin comprobar nulo**); `call [vtable+0x18]` con (ped, prioridad, evento) reenviados tal cual (empuja evento, prioridad, ped). La ranura 6 de `CTask` (verificada en el EXE: `0x8707C4 + 6*4` contiene 0x682C60 y la ranura 13 0x684380).
2. `test al,al` (sólo AL): 0 → `xor al,al; ret 0Ch` (no toca nada).
3. `al=[esi+74h]` (`m_conversationEnabled`) y `al=[esi+76h]` (`m_pedConversationLoaded`): **cada BYTE se prueba entero** (cualquier valor ≠ 0 es verdadero). Si ambos → `call 0x4E52A0` (`CAEPedSpeechAudioEntity::ReleasePedConversation`, cdecl sin argumentos) y `byte [esi+76h] = 0`.
4. `mov al,1; ret 0Ch`. Sólo AL es el retorno; los 24 bits altos de EAX son residuo.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexPartnerChat.cpp`: `RH_ScopedVMTInstall(MakeAbortable, 0x682C60)` y el cuerpo
```cpp
if (!m_pSubTask->MakeAbortable(ped, priority, event)) { return false; }   // sin comprobar nullptr, como el original
if (m_conversationEnabled && m_pedConversationLoaded) { CAEPedSpeechAudioEntity::ReleasePedConversation(); m_pedConversationLoaded = false; }
return true;
```
El `.h` no cambia. El borrador sin commit del árbol principal era idéntico en esta función; se contrastó línea a línea (abajo).

## Fase 2: revisión contra el EXE (línea a línea)

| EXE | C++ | Resultado |
|---|---|---|
| 0x682C60..0x682C77: carga (evento, prioridad, ped) de `[esp+0Ch/8/4]` tras `push esi`, `ecx=[esi+8]`, `call [eax+18h]` | `m_pSubTask->MakeAbortable(ped, priority, event)` | igual: mismo orden (ped, prioridad, evento = argumentos 1, 2, 3); `m_pSubTask` en +8 (`static_assert(offsetof…)` en `LiveComparison.hpp`); sin comprobar nulo |
| 0x682C7A: `test al,al; je 0x682C9B` | `if (!…) return false;` | igual: sólo AL; el banco devuelve en EAX valores con AL = 0/1/2/0x7F/0x80/0xFF/aleatorio y 24 bits altos aleatorios |
| 0x682C7E..0x682C88: `[esi+74h]` y `[esi+76h]` con `test al,al` | `m_conversationEnabled && m_pedConversationLoaded` | igual en bytes (MSVC prueba el byte de un `bool` con `cmp/test`, no lo normaliza; el banco prueba 2, 0x80, 0xFF, etc. en ambos bytes: si el compilador asumiera 0/1 saldría una diferencia) |
| 0x682C8C: `call 0x4E52A0; mov [esi+76h],0` | `ReleasePedConversation(); m_pedConversationLoaded = false;` | igual, y en ese orden (el falso registra los bytes +0x74/+0x76 EN la llamada: el valor de +0x76 debe ser aún el original) |
| 0x682C95 / 0x682C9B: `mov al,1` / `xor al,al` | `return true/false` | igual (EAX alto es residuo; no se compara) |

**Discrepancias halladas: ninguna.** El borrador (que nunca se compiló) era correcto; sólo se añadió el comentario sobre la prueba de bytes enteros.

**Residuo de registros / llamadores.** `residue_scan.py 0x682C60` no halla llamadas directas ni referencias: sólo se invoca por la tabla virtual (`CTask::MakeAbortable`, ranura 6, que devuelve `bool` en AL). No existe llamador que lea ECX/EDX tras la llamada (en este banco el valor de ECX/EDX/EAX alto de salida no tiene contrato); EAX/ECX/EDX de salida sólo se registran (`original_residue`/`candidate_residue`, `kCompareVolatileRegisters=false`); el banco compara AL como etiqueta (`observed.ret`). **El revisor debe confirmar a mano** que ningún sitio de llamada virtual de la ranura 6 lee los 24 bits altos de EAX (no se pudo recorrer: son todos los `call [reg+18h]` del EXE).

## Qué compara el banco (por lado; original == candidato == oráculo)

Original = bytes del EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskComplexPartnerChat","MakeAbortable",0x682C60)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). Cada caso monta, en memoria propia: un `this` falso de 0x78 bytes con guardas (relleno pseudoaleatorio; se fijan `m_pSubTask`, +0x74 y +0x76) y una subtarea falsa con vtable falsa de 16 entradas (cada entrada es un falso distinto: si la función invocara otra ranura, la traza lo dice).

- `observed.ret` (AL), `observed.events` (traza de los falsos: `[1, ranura, etiqueta de la subtarea, ped, prioridad, evento]` y, si procede, `[2, byte +0x74, byte +0x76]` en el instante de `ReleasePedConversation`), `observed.this_changes` (toda diferencia de bytes del arena respecto a la imagen previa, con guardas; sólo puede aparecer `[0x76, 0]`), `observed.sub_ok`.
- ABI: ESP (`ret 0Ch`, `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, `fs:[0]` igual, x87 normalizado (FCW completa, TOP, etiquetas y ST válidos), entrada 0x4E52A0 intacta tras los parches RAII; ruta (`route_ok`): original → 0x682C60 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI.
- **Colaboradores sustituidos** (`collaborators_stubbed=true`): la subtarea (objeto y vtable falsos, ver arriba) y `CAEPedSpeechAudioEntity::ReleasePedConversation` (parche de entrada en 0x4E52A0 **y** en el cuerpo de la DLL resuelto antes de parchear con `Body()`; el mismo falso en ambos lados). No se valida aquí el cuerpo de `ReleasePedConversation` ni el de ninguna subtarea real.

### Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

`n % 16`: 0 subtarea no abortable; 1 abortable + conv + cargada (libera); 2 conv sin cargada; 3 cargada sin conv; 4 ninguno; 5 AL y bytes no canónicos; 6 AL = 1 con bytes no canónicos; 7 AL = 0 con conv y cargada ≠ 0 (no libera); 8–15 combinaciones aleatorias. Argumentos ped/prioridad/evento: lista de bordes {0,1,2,3,4,0x10,0x14,0x7FFFFFFF,0x80000000,0xFFFFFFFF} o aleatorios; semillas de pila, EAX y EDX por caso. Mínimos de `run_local.py` calibrados con `fixture_mirror.py` (≈ 50 % de lo medido).

## Suposiciones y riesgos

- No se prueba subtarea nula (el EXE se detiene; es una propiedad del original, no un caso de prueba).
- `CAEPedSpeechAudioEntity::ReleasePedConversation` y `CTask::MakeAbortable` se llaman por dirección distinta en cada lado: se parchean ambas (EXE y cuerpo de la DLL). Si la DLL **inlinease** `ReleasePedConversation` en el cuerpo de `MakeAbortable`, el falso no se invocaría (el banco lo mostraría como diferencia de eventos); no ocurre con la compilación actual (función en otra unidad de traducción).
- Offsets verificados con `static_assert(offsetof)` en `LiveComparison.hpp` (tamaño 0x78, `m_pSubTask` +8, +0x74, +0x76) y en `fixture_mirror.py` (arenas del C++ frente al oráculo).
- Hash de fuentes probadas: `build_probe.py` lista `TaskComplexPartnerChat.cpp/.h`, `TaskComplexPartner.h`, `TaskComplex.h`, `AEPedSpeechAudioEntity.cpp/.h`, los hooks `Simple`/`Virtual` y los tres archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `README.md`.
