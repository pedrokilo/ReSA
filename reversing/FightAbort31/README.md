<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco FightAbort31: CTaskSimpleFight::MakeAbortable (0x6239F0)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerChatAbort30` (Gate, hook Virtual, estructura de ficheros, `fixture_mirror`) y el contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FightAbort31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FightAbort31/build_probe.py
python -B reversing/FightAbort31/run_local.py
python -B reversing/FightAbort31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/FightAbort31/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/FightAbort31/fixture_mirror.py                                              # compila fixture_mirror.cpp con cl (sin el juego): 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -I reversing/FightAbort31/selftest_source.py                                             # compila el cuerpo REAL de la función contra maquetas y la compara con el oráculo (4608 casos; imprime «ok»)
python -B reversing/FightAbort31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(MakeAbortable, 0x6239F0)` (categoría/clase `CTaskSimpleFight`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `fightabort31-runtime.start`; salida `fightabort31-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskSimpleFight,MakeAbortable,0x00000000,1,0,1,V` (no la añade `register_hook.py`, que no toca `docs/`).

Ya comprobado sin el juego (worktree `D:\Original\ws-4`): `fixture_oracle.py` → `ok`; `fixture_mirror.py` (cl) → `ok` con 4608 casos aceptados, tamaños de arena idénticos entre `Fixtures.hpp` y el oráculo (`SIZES`) y 0 faltas de cobertura; `selftest_source.py` → `ok` (el fuente C++ real y el oráculo coinciden en los 4608 casos con maquetas de los tipos del juego); `Core.hpp` compila con un arnés de pruebas (stubs de `HookRoute`/`json`/`ProbeSupport`); `source.patch` pasa `git apply --check` contra el blob de HEAD. **No** se ha compilado `LiveComparison.hpp` con el proyecto (riesgo de errores de compilación del banco: el coordinador lo verá en `probe-build.log`).

## Qué hace la función (desensamblado 0x6239F0..0x623B0A, `ret 0Ch`, sin marco SEH, guarda EBP/ESI/EDI)

`bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)`; `this` en ECX (→ ESI).
1. `priority` distinto de 1 (urgente) y de 2 (inmediata): `m_nNextCommand` (+0x26) = 1 y devuelve 0.
2. Con evento no nulo: pide **dos veces** `[vtable+8]` (**ranura 2 de `CEvent` = `GetEventPriority`**, no `GetEventType`, que es la ranura 1 / `[vtable+4]`): si el valor es `<= 0x1F` (con signo) devuelve 0; si no, si vuelve a pedirlo y es `== 0x3C` devuelve 0.
3. `m_pAnim` (+0x1C) no nulo: si la prioridad es inmediata, `m_BlendDelta` (+0x1C) = −1000.0 (`0xC47A0000`); `SetDeleteCallback(0x4D1390, 0)` (0x4CEBC0); `m_pAnim = 0`.
4. `m_pIdleAnim` (+0x20) no nulo: `SetDeleteCallback(0x4D1390, 0)`; recarga; si `peso(+0x18) > 0` **y** `delta(+0x1C) >= 0` (con NaN no) → `BlendAnimation(clump = [ped+0x18], grupo = [ped+0x4D4], 3, 1000.0 (inmediata) / 16.0)` (0x4D4610) **sin comprobar ped nulo**; `m_pIdleAnim = 0`.
5. Ped no nulo y `IsPlayer` (0x5DF8F0): `[[ped+0x480]+0xC] = 0` y `+0x10 = 0` (`m_vecFightMovement`, **sin comprobar nulo**) y `FindTargetOnGround(ped)` (0x61C9B0, `ret 4`, sin reconstruir).
6. `m_bIsFinished` (+8) = 1 y devuelve 1. Sólo AL es el retorno.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskSimpleFight.cpp`: includes (`Entity/Ped/PlayerPed.h`, `PlayerPedData.h`, `Events/Event.h`), un bloque anónimo con la devolución de llamada `0x4D1390`, la constante −1000.0 y el envoltorio `FightFindTargetOnGroundExe` (0x61C9B0 por dirección del EXE), `RH_ScopedVMTInstall(MakeAbortable, 0x6239F0)` (ranura 6 de la vtable 0x86D684, verificada: `[0x86D684 + 6*4] = 0x6239F0`) y el cuerpo. `source-all.patch` (en `FightProcessPed31/`) contiene las tres funciones a la vez; los parches individuales tocan líneas contiguas de `InjectHooks()` y de los includes: aplícalos con `--3way` o usa el combinado.

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador

| EXE | C++ | Resultado |
|---|---|---|
| 0x6239F0..0x623A0A: `cmp ebp,1 / 2` y rama de prioridad ociosa | `if (priority != URGENT && priority != IMMEDIATE) { next = 1; return false; }` | igual |
| 0x623A0D..0x623A36: `test edi,edi` + dos `call [vtable+8]` con `cmp eax,1Fh ; jle` y `cmp eax,3Ch ; jne` | `event->GetEventPriority() <= 0x1F` y `== 0x3C` (dos llamadas) | **corregido** (ver abajo) |
| 0x623A36..0x623A5F | delta −1000 si inmediata, `SetDeleteCallback`, `m_pAnim = 0` | igual (orden: escritura de delta antes del callback) |
| 0x623A5F..0x623AD3 | callback, recarga de peso y delta, `fcomp` (`test ah,41h` = `>`; `test ah,1` = `>=`), `BlendAnimation` con 1000.0 / 16.0 | igual; la comparación con NaN es falsa en ambos |
| 0x623AD3..0x623B00 | `ped && IsPlayer` → ceros + `FindTargetOnGround` | igual |
| 0x623B01..0x623B09 | `m_bIsFinished = 1 ; return true` | igual |

**Discrepancia hallada y corregida.** El borrador llamaba a `event->GetEventType()` (ranura 1 de la vtable de `CEvent`: la clase declara `~CEvent`, `GetEventType`, `GetEventPriority`, `GetLifeTime`, `Clone`, y otra tarea del repositorio la usa como `[edx+4]`), pero el EXE llama a `[vtable+8]` = **`GetEventPriority`** y compara esa prioridad con 0x1F / 0x3C. Ahora usa `GetEventPriority()`. Lo detectó `selftest_source.py` por la ranura distinta; el banco lo detectaría además con las 16 ranuras falsas distintas. Sin más diferencias.

**Residuo de registros / llamadores.** Sólo se invoca por la tabla virtual (`CTask::MakeAbortable`, ranura 6). `residue_scan.py 0x6239F0` no halla llamadas directas. EAX/ECX/EDX de salida sólo se registran (`kCompareVolatileRegisters=false`); el banco compara AL como etiqueta. **El revisor debe confirmar** que ningún otro sitio de llamada virtual de la ranura 6 lea ECX/EDX o los 24 bits altos de EAX (no se pudo recorrer: son todos los `call [reg+18h]` del EXE).

## Qué compara el banco (por lado; original == candidato == oráculo)

Original = bytes del EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskSimpleFight","MakeAbortable",0x6239F0)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). Cada caso monta en memoria propia (con guardas): la tarea (0x28 bytes), las asociaciones de golpe y de reposo (0x3C), la ventana del ped (0x4D8), los datos del jugador (0x20) y un evento falso con vtable falsa de 16 ranuras (cada una un falso distinto; la 2 devuelve la prioridad del caso y registra la llamada).
- `observed.ret` (AL), `observed.events` (traza ordenada de los falsos: `GetEventPriority` ×1/×2, `SetDeleteCallback` de la anim y del reposo con los argumentos tal cual, `BlendAnimation` con los bits del delta, `IsPlayer`, `FindTargetOnGround`), `observed.changes` (todo cambio de bytes en cualquiera de los seis arenas respecto a su imagen previa, **guardas incluidas**).
- ABI: ESP (`ret 0Ch`, `popBytes = 0`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0, `fs:[0]` igual, x87 normalizado (FCW completa, TOP, etiquetas y ST válidos), entradas 0x4CEBC0 / 0x4D4610 / 0x5DF8F0 / 0x61C9B0 intactas tras los parches RAII; ruta (`route_ok`): original → 0x6239F0 NO salta a la DLL; candidato → sí, con el cuerpo en el módulo ASI.
- **Colaboradores sustituidos** (`collaborators_stubbed=true`, mismo falso en ambos lados con parche de entrada en el EXE **y** en el cuerpo de la DLL resuelto antes de parchear): `SetDeleteCallback` 0x4CEBC0, `BlendAnimation` 0x4D4610 (sobrecarga `(RpClump*, AssocGroupId, AnimationId, float)`), `CPed::IsPlayer` 0x5DF8F0 y, sólo EXE, `FindTargetOnGround` 0x61C9B0. No se validan sus cuerpos.

### Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

0 prioridad ociosa/otras (0, 3, aleatorias); 1 prioridad del evento ≤ 0x1F con signo; 2 evento 0x3C; 3/4 urgente/inmediata sin evento con ambas animaciones; 5/6 con evento aceptable y jugador; 7 sólo `m_pAnim`; 8 sólo reposo que se mezcla; 9 reposo que no se mezcla (peso ≤ 0 / NaN / delta < 0); 10 ped nulo (sólo sin mezcla); 11 jugador sin animaciones; 12 no jugador con todo; 13 prioridades de borde (3, 0x80000001, 0xFFFFFFFF); 14–15 aleatorias. Pesos y deltas de borde (±0, denormales, ±inf, NaN silencioso y señalizante, FLT_MAX…). Mínimos de `run_local.py` calibrados con `fixture_mirror.py` (≈ 50 % de lo medido).

## Suposiciones y riesgos

- Ped nulo con mezcla del reposo no se prueba (el EXE se detiene); tampoco `m_pPlayerData` nulo con jugador.
- Las ventanas de objetos se verifican con `static_assert(offsetof)` en `LiveComparison.hpp` (tarea +8/+0x1C/+0x20/+0x26, asociación +0x18/+0x1C, clump +0x18, `m_pPlayerData` +0x480, `m_nAnimGroup` +0x4D4, `m_vecFightMovement` +0xC).
- Si la DLL **inlinease** algún colaborador sustituido en el cuerpo de `MakeAbortable`, el falso no se invocaría (el banco lo mostraría como diferencia de eventos); no ocurre con la compilación actual (funciones en otras unidades).
- Hash de fuentes probadas: `build_probe.py` lista `TaskSimpleFight.cpp/.h`, `TaskSimple.h`, `Task.h`, `AnimBlendAssociation.cpp/.h`, `AnimManager.cpp/.h`, `Ped.cpp/.h`, `PlayerPedData.h`, `Event.h`, los hooks `Simple`/`Virtual` y los tres archivos del banco.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `selftest_source.py`, `register_hook.py`, `source.patch`, `README.md`.
