# Banco FightProcessPed31: CTaskSimpleFight::ProcessPed (0x629920)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/FightAbort31`, `reversing/PartnerChatAbort30` (Gate, hook Virtual) y el contrato `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sobre `__asm`/`naked` sólo valen para producción; el banco usa ensamblador en `Gate`, `SaveFP`, etc.).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh FightProcessPed31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/FightProcessPed31/build_probe.py
python -B reversing/FightProcessPed31/run_local.py
python -B reversing/FightProcessPed31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/FightProcessPed31/fixture_oracle.py                                              # autoprueba trivial del oráculo (imprime «ok»; la funcional es fixture_mirror)
python -I reversing/FightProcessPed31/fixture_mirror.py                                              # compila fixture_mirror.cpp con cl: 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura de ramas frente a los mínimos
python -I reversing/FightProcessPed31/selftest_source.py                                             # compila el cuerpo REAL de ProcessPed contra maquetas y lo compara con el oráculo (4608 casos; imprime «ok»)
python -B reversing/FightProcessPed31/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(ProcessPed, 0x629920)` (categoría/clase `CTaskSimpleFight`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `fightprocessped31-runtime.start`; salida `fightprocessped31-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskSimpleFight,ProcessPed,0x00000000,1,0,1,V`. `source-all.patch` contiene además `LoadMeleeData` y `MakeAbortable` (los tres parches individuales tocan líneas contiguas: aplícalos con `--3way` o usa el combinado).

Ya comprobado sin el juego (worktree `D:\Original\ws-4`): `fixture_mirror.py` → `ok` con 4608 casos aceptados por el oráculo (0 inválidos), tamaños de arena idénticos entre `Fixtures.hpp` y el oráculo, 0 faltas de cobertura (el oráculo registra ~70 ramas distintas y todas se recorren); `selftest_source.py` → `ok`: el cuerpo C++ real, compilado contra maquetas de los tipos del juego con los mismos falsos, **coincide con el oráculo en los 4608 casos** (eventos en orden, cambios de bytes de tarea/asociaciones/ped, retorno y ángulo con 2 ulp); `Core.hpp` compila con un arnés de stubs; los 3 `source*.patch` pasan `git apply --check` contra HEAD. **No** se ha compilado `LiveComparison.hpp` con el proyecto.

## Qué hace la función (desensamblado 0x629920..0x62A090, `ret 4`, sin marco SEH, guarda EBP/ESI/EBX/EDI)

Es la función más grande de `CTaskSimpleFight`; el oráculo (`fixture_oracle.py`, cabecera) la describe bloque a bloque con las direcciones del EXE. Resumen: (1) tarea terminada → suelta el reposo (mezcla a 8.0 si peso > 0 y delta ≥ 0) y devuelve 1; (2) contador de espera (`ftol(paso·0.02·1000)`, `_ftol2` de 64 bits, se suma `ax` a un `u16`); (3) sin control: `(i8)siguiente < 1` o anim con id 0x2F → devuelve 0; (4) sin reposo: último comando de fin (1, 0xF, 0x10, 0x11) o, sin anim y sin (movestate > 4 y jugador), crea el reposo: arma activa → combo base (`GetWeaponInfo(tipo,1)->+6Eh`, o el estilo de pelea si es 4) → fila de la tabla (flag 0x400, grupo 0x21 / igual / distinto, bloque por nombre, `AddAnimBlockRef` o `RequestModel(idx+0x63E7, 8)`) → `BlendAnimation(clump, grupo, 0xDF, 4.0)` + `SetDeleteCallback(0x61DAE0, this)` + `SetMoveState(1)` + `[ped+0x538]=1`; (5) `GetAvailableComboSet(ped,-1)` si falta referenciar; (6) sin anim de golpe: comandos del jugador / IA (`0x61C7F0`, `0x61C9B0`, `0x624710`, `0x624A40`, `0x623B10`); (7) con anim: motosierra (`0x4E69F0`), combo < 4 (último 8/0xA/3), combo ≥ 4 y último 2 (parar / desvanecer / soltar) y el caso general (peso > 0.9, delta ≥ 0, cruce del instante del golpe: sonidos `0x4E2BB0`, nivel, punto con la matriz del ped (0x59C890) y `FightStrike` (0x6240B0); si no, encadenar combos); (8) orientación: `atan2(-(obj.x-ped.x), obj.y-ped.y)` al objetivo, o a la cámara (0xB6F32C) si es el jugador con ratón y `CPad::GetTarget`; (9) `m_bIsInControl = 0`, devuelve 0.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskSimpleFight.cpp`: includes (`AnimBlendHierarchy.h`, `PlayerPed.h`, `Camera.h`, `Pad.h`, `Timer.h`, `Streaming.h`), un bloque anónimo de constantes (bits exactos de los `.rdata` del EXE: 0x858B38 = 0.02f, 0x858C4C = 1000.0f, 0x8D2E94 = 0.4f, 0x858C20 = 0.9f, 0x858C58 = 0.01f; −4.0 de 0x859C3C), `FightComboRow`, los cinco colaboradores sin reconstruir por dirección del EXE, `#pragma float_control(precise, on)` alrededor de `FightTransformPoint` (0x59C890, orden de sumas del EXE) y de `ProcessPed`, `RH_ScopedVMTInstall(ProcessPed, 0x629920)` (ranura 7 de la vtable 0x86D684, verificada) y el cuerpo.

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador

Se contrastó cada bloque del desensamblado con el C++ (incluida la semántica `test ah,41h / 1 / 5 + jp` de las comparaciones x87, el orden de llamadas a `IsPlayer` —hasta tres por llamada—, qué bytes se comparan con signo, `push eax` con residuos en los argumentos de byte de 0x61C7F0, la relectura de campos tras cada colaborador, y `fsubr`/`fadd` en el cruce del golpe).

| EXE | C++ | Resultado |
|---|---|---|
| 0x629927..0x62998D (terminada) | `if (m_bIsFinished) {..}` | igual (`BlendAnimation(.., 3, 8.0)`; devuelve 1 sin tocar `[esi+9]`) |
| 0x629990..0x6299BF (contador) | `m_nIdleCounter += (u16)(i64)(paso·0.02·1000)` | igual; `pragma precise` evita plegar 0.02·1000 |
| 0x6299BF..0x6299E2 (sin control) | `(i8)next < 1`, `m_AnimId == 0x2F` | igual |
| 0x6299E2..0x629B9B (reposo) | bloque `if (!m_pIdleAnim)` | igual (`AL` reutilizado como combo elegido en las ramas de grupo; bloque nulo → nombre → bloque; índice `(b-0xB5D4A0)>>5` con signo) |
| 0x629B9B..0x629BB2 | `GetAvailableComboSet(ped,-1)` | igual |
| 0x629BB2..0x629C4E (sin anim) | comandos, `IsPlayer` dos veces | igual |
| 0x629C4E..0x629FF7 (con anim) | motosierra, combo < 4, último 2, golpe y cadena | igual (`time - paso < golpe` con `fsubr`; `paso + time >= golpe`; `(i8)` en las pruebas de 0xB..0xE) |
| 0x629FF7..0x62A090 (orientación) | `TrigAtan2Cpp(-dx, dy)`; cámara con `CPad::GetTarget` | igual (`fxch ; fchs ; fxch ; fpatan`) |

**Discrepancias halladas y corregidas** (el borrador era correcto en lógica):
1. **`/fp:fast`**: el borrador no fijaba el modelo de coma flotante; con el modelo rápido el compilador puede reasociar las sumas de `FightTransformPoint` o plegar `0.02f * 1000.0f` (distinto redondeo que las dos `fmul` de 24 bits del EXE). Ahora `FightTransformPoint` y `ProcessPed` van entre `#pragma float_control(push) / (precise, on) / (pop)` (como `CTaskComplexPartnerChat::GetPartnerSequence`).
2. Sin más: `selftest_source.py` no halló diferencias entre el fuente y el oráculo; las revisiones manuales (arriba) tampoco.

**Residuo de registros / llamadores.** El único llamador directo conocido es `CTaskManager::ManageTasks` (0x681C10): `call [eax+1Ch]` en 0x681D08 y 0x681DA5, ambos seguidos de `test al,al`: **sólo AL** (EAX alto, ECX y EDX no se leen). `residue_scan.py 0x629920` no halla llamadas directas ni referencias. EAX/ECX/EDX de salida sólo se registran (`kCompareVolatileRegisters=false`); el banco compara AL como etiqueta. **El revisor debe confirmar** que ningún otro sitio de llamada virtual de la ranura 7 (p. ej. `CTaskComplex::ProcessPed` sobre la subtarea) lea ECX/EDX: no se recorrieron todos.

## Qué compara el banco (por lado; original == candidato == oráculo)

Original = bytes del EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido (`HookRoute("CTaskSimpleFight","ProcessPed",0x629920)` con el cuerpo de `ReversibleHook::Virtual::GetHookOurAddress()`). Cada caso monta en memoria propia (arenas con guardas, rellenas con un patrón por caso): la tarea (0x28), tres asociaciones (golpe, reposo, reposo nuevo devuelto por `BlendAnimation`), la jerarquía, la ventana del ped (0x730), las matrices del ped y del objetivo, el objetivo, tres `CAnimBlock`, la tabla de cuatro `CAnimBlendAssocGroup`, un `CWeaponInfo` y un `CPad`. Globales fijadas por caso y restauradas: la **región [0xC17090, 0xC17940)** (12 filas de la tabla de combos y los siete vectores de nivel escritos explícitamente sobre el patrón), el paso de tiempo 0xB7CB5C, 0xB6EC2E, 0xB6F32C y el puntero a la tabla de grupos 0xB4EA34.
- `observed.ret` (AL), `observed.events` (traza ordenada de 21 clases de falso con **todos sus argumentos**), `observed.changes` (todo cambio de bytes en tarea, anim, reposo, ped —sin los 4 bytes del ángulo— y reposo nuevo, guardas incluidas), `observed.aim` (bits de `m_fAimingRotation`, inicial 0xFFFFFFFF = «sin escribir»), `observed.others_unchanged` (jerarquía, matrices, objetivo, bloques, grupos, `CWeaponInfo`, pad) y `observed.globals_unchanged`. El oráculo modela el ángulo con `atan2` de doble precisión: `run_local.py` exige **2 ulp** frente al oráculo y **igualdad en bits** entre EXE y DLL.
- ABI: ESP (`ret 4`, `popBytes = 0`), EBX/ESI/EDI/EBP, DF = 0, `fs:[0]`, x87 normalizado, entradas del EXE de los 21 colaboradores intactas tras los parches RAII, ruta (`route_ok`).
- **Colaboradores sustituidos** (mismo falso en ambos lados; parche de entrada en el EXE y, si existe, en el cuerpo de la DLL resuelto antes de parchear): ya en C++ y enganchados: `SetDeleteCallback` 0x4CEBC0, `BlendAnimation` 0x4D4610, `SetCurrentTime` 0x4CEA80, `CPed::IsPlayer` 0x5DF8F0, `SetMoveState` 0x5DEC00, `AddAnimBlockRef` 0x4D3FB0, `RemoveAnimBlockRef` 0x4D3FD0, `GetAnimBlockName` 0x4D3A30, `GetAnimationBlock(const char*)` 0x4D3940, `RequestModel` 0x4087E0, `GetWeaponInfo` 0x743C60, `CAEPedAudioEntity::AddAudioEvent` 0x4E2BB0, `CAEPedWeaponAudioEntity::AddAudioEvent` 0x4E69F0, `FightStrike` 0x6240B0, `GetPadFromPlayer` 0x609560 y `CPad::GetTarget` 0x540670; sólo EXE: `0x61C7F0`, `0x61C9B0`, `0x624710`, `0x624A40`, `0x623B10`. No se validan sus cuerpos. **El argumento de byte de 0x61C7F0 se registra enmascarado a 8 bits**: el EXE lo empuja con los 24 bits altos de EAX residuales, la DLL con cero; el callee sólo lee el byte (`mov bl,[esp+0Ch]`).

### Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

0 terminada con reposo; 1 terminada sin reposo; 2 sin control; 3 contador (paso de tiempo de borde: 0, −0, 1, 26.0, ±inf, NaN, FLT_MAX…, vueltas del `u16`); 4 último comando de fin; 5/6 creación del reposo (arma, estilo, fila con/sin flag, grupo 0x21 / igual referenciado / igual sin referencia / distinto, bloque nulo → por nombre, cargado / no cargado, siguiente reescrito a 0xB); 7 sin anim de golpe; 8 combo < 4 (cociente tiempo/total alrededor de 0.4 con enteros exactos, delta alrededor de −4.0); 9 último 2 (parar, desvanecer, soltar); 10 cruce del golpe (`k·2⁻¹²` exactos y desplazamientos de 1–64 ulp; motosierra; combo 5 con sonido alterno; nivel 7; movimiento 4 con velocidad); 11 cadena (siguiente 0xB..0xE, movimientos 0/1/3/4, combo 0xC); 12 orientación (con/sin matriz del ped y del objetivo, objetivo en la misma posición → `atan2(−0, 0)`, cámara con/sin pad); 13 bordes de coma flotante en pesos, tiempos y paso; 14–15 mezcla. El generador evita ped sin matriz con anim (el EXE se detiene), grupos exigidos fuera de la tabla falsa con referencia pendiente y jerarquía nula cuando se necesita. Mínimos de `run_local.py` ≈ 45 % de lo medido por `fixture_mirror.py` (por rama del oráculo).

## Suposiciones y riesgos

- El ángulo de orientación se compara con 2 ulp frente al oráculo (no modela `fpatan` al ulp); EXE == DLL estricto en bits. `TrigAtan2Cpp` del proyecto usa `atan2` de doble precisión y redondea a `float`.
- Tamaños de arena: `Fixtures.hpp` y el oráculo (`SIZES`) están cruzados por `fixture_mirror.py` (compilado con cl).
- Las ventanas de objetos se verifican con `static_assert(offsetof)` en `LiveComparison.hpp` (tarea, asociación, jerarquía +0x10, `CAnimBlock` 0x20/IsLoaded +0x10, grupo 0x14, `CWeaponInfo::m_nBaseCombo` +0x6E, `CPed`: +0x14 matriz, +0x18 clump, +0x44 velocidad, +0x138 audio, +0x394 audio del arma, +0x4D4, +0x534, +0x538, +0x55C, +0x5A0, +0x718, +0x72D).
- Si la DLL **inlinease** algún colaborador sustituido, el falso no se invocaría (diferencia de eventos); no ocurre con la compilación actual.
- Si dos colaboradores tuvieran cuerpos idénticos y el enlazador los plegase (ICF), el parche de uno afectaría al otro (el banco lo mostraría); improbable en este conjunto.
- NaN señalizantes: sólo en campos que se comparan (pesos, tiempos, paso); no en los que se copian o restan (golpe, bloqueo).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `selftest_source.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
