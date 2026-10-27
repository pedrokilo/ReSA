<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco WalkRoundFirst31: CTaskComplexWalkRoundObject::CreateFirstSubTask (0x657380)

**Estado: PREPARADO** (sin compilar el proyecto ni ejecutar el juego: lo corre el coordinador en serie). Plantillas: `reversing/PartnerChatSeq30` / `PartnerChatAbort30` (tarea virtual, pool sintético, hook `Virtual`, falsos con parche de entrada en el EXE y en la DLL), `reversing/WalkRoundControl31` (hermano: falsos de `AllocateMatrix`/`UpdateMatrix`, reloj) y el contrato `reversing/_handoff/BRIEF.md`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh WalkRoundFirst31          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/WalkRoundFirst31/build_probe.py
python -B reversing/WalkRoundFirst31/run_local.py
python -B reversing/WalkRoundFirst31/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro aún no existe o no está completo)
python -I reversing/WalkRoundFirst31/fixture_oracle.py                                             # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/WalkRoundFirst31/fixture_mirror.py                                             # compila fixture_mirror.cpp con cl (sin el juego): 0 casos inválidos + tamaños de arena C++ == oráculo + cobertura frente a los mínimos
python -B reversing/WalkRoundFirst31/register_hook.py <carpetaEvidencia>                           # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380)` (clase `CTaskComplexWalkRoundObject`, hook `Virtual`; `source.patch` lo añade a HEAD). Archivo de arranque en el clon: `walkroundfirst31-runtime.start`; salida `walkroundfirst31-runtime.jsonl`. 4608 pares. `docs/hooks.csv` tendrá que llevar la fila `CTaskComplexWalkRoundObject,CreateFirstSubTask,0x00000000,1,0,1,V` (la dirección real, 0x00657380, va en `evidence.original_address`).

**Requisitos del árbol al compilar:** `CTask::operator new` (0x61A5A0), `CGeneral::LimitRadianAngle` (0x53CB50) y `GetRadianAngleBetweenPoints` (0x53CBE0) siguen enganchados a la DLL (`collaborators_dll`); pool de tareas con ≥ 256 ranuras.

Ya comprobado sin el juego (worktree `D:\Original\ws-3`): `fixture_oracle.py` imprime `ok`; `fixture_mirror.py` imprime `ok` (4608 casos aceptados por el oráculo, arenas del C++ iguales a las del oráculo, 0 faltas de cobertura); **prueba cruzada del oráculo con código real** sobre las 4608 entradas: la copia literal de `CVector::NormaliseAndMag` del fuente de la DLL (float SSE) más `WalkRoundDotPositiveCpp` compilado a x87 de 24 bits dan módulo, vector normalizado y signo del producto escalar **idénticos al bit** a `normalise_and_mag` + `fadd/fmul` del oráculo (0 diferencias); `Core.hpp`/`LiveComparison.hpp` compilan sin errores con `cl /Zs` y las opciones del proyecto; `source.patch` pasa `git apply --check` contra los blobs de HEAD y el `.cpp` resultante compila solo. **No** se ha enlazado ni ejecutado la sonda.

## Qué hace la función (desensamblado 0x657380..0x6575E9, `ret 4`, marco SEH propio)

`this` en ECX (→ ESI), `ped` en `[esp+0x34]` (→ EBP). Orden observable:
1. `m_object` (+0x1C) nulo → 0 (sin más llamadas).
2. `m_pointRoute->m_NumEntries = 0` y `call 0x6551D0(this, ped)` (sin símbolo en el catálogo: rellena la ruta alrededor del objeto y devuelve un **float en ST0**, guardado con `fstp` como `routeDist`). Contador de la ruta == 0 → 0.
3. `field_24 = reloj (0xB7CB84)`, `field_28 = (m_moveState == 4) ? 8000 : 4000`, byte `field_2C = 1`.
4. Posición del objeto (matriz+0x30 si hay matriz, si no placement+4) → +0x30; si falta la matriz: `AllocateMatrix` (0x54F560) + `UpdateMatrix` (0x54F1B0); frente (matriz+0x10) → +0x3C; (segunda comprobación de la matriz, nunca se cumple tras la primera); derecha (matriz+0x00) → +0x48.
5. `d = ruta[0] − posición del peatón` (matriz del peatón +0x30, o placement+4 si no hay matriz) con `fsub` y `fstp` a float; `mag = d.NormaliseAndMag()` (0x59C970; `d` queda normalizado, `mag` se guarda como float); `dot = (d.z·f.z + d.y·f.y) + d.x·f.x` en x87 con la matriz del peatón **sin comprobar nulo** (`f` = frente +0x10).
6. Máximo = 2.0 (0x858CA0) si `m_moveState == 4`, 4.0 (0x858B90) si == 6, si no 6.0 (0x858B44). Si `mag > máx` **y** `routeDist > máx` **y** `dot > 0.0` (0x86F420) (las tres comparaciones ordenadas: NaN da falso) → `CreateRouteTask(ped)` (0x655140).
7. Si no: `x = ruta[0].x − pos.x`, `y = ruta[0].y − pos.y` (float), `a = GetRadianAngleBetweenPoints(x, y, 0, 0)` (0x53CBE0, guardado como float), `h = LimitRadianAngle(a)` (0x53CB50, guardado como float), `new(0x18)` (0x61A5A0) + `CTaskSimpleAchieveHeading(h, 1.0f, 0.2f)` (0x667E20); reserva fallida → 0.

## Fuente (`source.patch`, sólo esta función, aplicable con `git apply` sobre HEAD)

`TaskComplexWalkRoundObject.cpp`: includes (`PedGeometryAnalyser.h` por `CPointRoute`, `TaskSimpleAchieveHeading.h`, `X87Math.h`), `RH_ScopedVMTInstall(CreateFirstSubTask, 0x657380)` y el cuerpo. `TaskComplexWalkRoundObject.h`: los tres vectores iniciales (+0x30/+0x3C/+0x48, uniones anónimas para que el constructor, que no los escribe, no los inicialice) y `VALIDATE_SIZE` 0x54. `X87Math.cpp/.h`: bloque «Lote 31» al final con `WalkRoundDotPositiveCpp` (código generado comprobado: `fld/fmul/faddp/fcomp/fnstsw/test ah,41h`). `0x6551D0` sigue llamándose por plugin al EXE (no está reconstruida; es otra tarea). `source-all.patch` = las tres funciones del lote juntas.

## Fase 2: revisión contra el EXE (línea a línea) y discrepancias del borrador del lote 27

Tras escribir se volvió a desensamblar 0x657380..0x6575E9 y se contrastó. **Discrepancias del borrador sin commit (nunca compilado) corregidas:**
1. **Producto escalar en SSE.** El borrador calculaba `dot = dir.z·f.z + dir.y·f.y + dir.x·f.x` con `float` del proyecto (SSE); el EXE lo deja en un registro x87 de 24 bits con exponente extendido, sin guardarlo. Ahora `WalkRoundDotPositiveCpp` (X87Math) con el orden `(z + y) + x`.
2. El borrador leía la matriz del peatón (`ped->m_matrix->GetForward()`) igual que el EXE (sin comprobar nulo: el EXE se detendría con matriz nula); se conserva, y se calcula el producto escalar antes de comparar `mag` y `routeDist` (sin efectos laterales; así el orden de lecturas coincide con el del EXE).
3. Sin más diferencias: `field_24` con el reloj (entero), `field_28` 8000/4000, byte `field_2C`, orden de copia (posición **antes** de crear la matriz; frente y derecha después), las dos comprobaciones de matriz del EXE (la segunda es código muerto tras la primera), constantes 0x858CA0 = 2.0, 0x858B90 = 4.0, 0x858B44 = 6.0, 0x86F420 = 0.0, `1.0f`/`0.2f` (0x3E4CCCCD) de la tarea de giro y los argumentos `(x, y, 0, 0)` de `GetRadianAngleBetweenPoints` verificados en el EXE.

| EXE | C++ | Resultado |
|---|---|---|
| 0x65739B..0x6573A2 | `if (!m_object) return nullptr` | igual |
| 0x6573B6..0x6573D6 | `m_NumEntries = 0`; 0x6551D0; `m_NumEntries == 0` → nulo | igual |
| 0x6573DC..0x6573F8 | `field_24`, `field_28`, `field_2C` | igual |
| 0x6573FD..0x657485 | posición, matriz si falta, frente, derecha | igual |
| 0x657488..0x657531 | `d`, `NormaliseAndMag`, producto escalar, máximo, tres comparaciones | corregido (1) |
| 0x657548..0x6575D4 | ángulo, `Limit`, `new` + ctor del giro | igual |

**Residuo de registros / llamadores.** `CreateFirstSubTask` es la ranura 9 de la vtable (0x86F364): su único llamador es `CTaskManager::AddSubTasks` (0x681A30, `ctask->CreateFirstSubTask(m_pPed)`), en C++ en la DLL con el hook activo, que sólo usa el puntero devuelto (EAX). `residue_scan.py 0x657380` no devuelve llamadas directas. **Nadie lee ECX/EDX**; el banco sólo los registra. El revisor debe confirmarlo.

## Qué se compara (por lado; original == candidato == oráculo)

Original = EXE con el hook RH apagado; candidato = DLL con **sólo** ese hook encendido. Cada caso monta: un `this` falso de 0x54 bytes (con contenido aleatorio; se compara **entero**), un objeto falso (placement distinto de la matriz) con su matriz, un peatón falso con su matriz (frente y posición), una `CPointRoute` falsa (contador + 8 vectores), el reloj (fijado por caso y restaurado) y un **pool sintético**. Observado: `ret`, lista de llamadas (`CF` = 0x6551D0(this, ped), `AM`, `UM`, `CR`), `route_after` (25 palabras: lo que dejó el falso o, sin objeto, lo inicial), cambios en `this` (byte a byte; el oráculo los deduce de `this_init` y del caso), `m_matrix` del objeto, objeto/matrices/peatón intactos, pool (cursor, estados, tarea nueva completa, nada fuera de la ranura). El **ángulo** de la tarea de giro (palabra +8) no se modela al bit (`fpatan`): se acepta a 1e-4 rad módulo 2π cuando `x`, `y` son finitos (`heading_unchecked` cuenta los demás), y EXE == DLL se exige al bit. ABI: ESP, EBX/ESI/EDI/EBP, DF = 0, `fs:[0]`, x87 normalizado (FCW de 24 bits, pila vacía), `route_ok` y `collaborators_dll`.

**Colaboradores.** *Reales* (ya en C++ y enganchados, se ejecutan en ambos lados): `CTask::operator new` 0x61A5A0, `CGeneral::LimitRadianAngle` 0x53CB50 y `GetRadianAngleBetweenPoints` 0x53CBE0; `CVector::NormaliseAndMag` 0x59C970 tiene su hook **apagado de fábrica** (corrida del 19/10/2026: la entrada del EXE no es un salto): el lado original ejecuta el código x87 del EXE y el candidato el cuerpo C++ de la DLL, así que sólo se vigila (las fixtures saneadas garantizan que coinciden); el constructor de `CTaskSimpleAchieveHeading` (0x667E20) **no** está enganchado: en el lado original es código del EXE y en el candidato el constructor C++ (el banco compara los bytes de la tarea). *Sustituidos* (`collaborators_stubbed=true`): `AllocateMatrix` 0x54F560 y `UpdateMatrix` 0x54F1B0 (parche de entrada en el EXE y en el cuerpo de la DLL) y, **sólo en el EXE** (la DLL los llama por plugin a esa dirección), 0x6551D0 (escribe contador y 24 palabras de la fixture en la ruta y devuelve el `float` de la fixture por ST0) y `CreateRouteTask` 0x655140 (devuelve un valor fijo).

## Fixtures (`Fixtures.hpp`): 4608 casos = 16 familias × 288

`n % 16`: 0 sin objeto; 1 ruta vacía; 2 camino de la ruta (lejos, de frente); 3 distancia al primer punto en el borde del máximo (2.0/4.0/6.0 y vecinos); 4 `routeDist` en el borde (NaN, ±inf, negativa, 2.0 ± ulp …); 5 producto escalar en el borde (ortogonal, 1 ulp, inf); 6 giro típico; 7 objeto sin matriz; 8 posición origen: matriz o placement; 9 primer punto igual a la posición (vector nulo → `NormaliseAndMag` devuelve 1.0 con x = 1); 10 flotantes extremos; 11 estados de movimiento raros (0…9, 0xFFFFFFFF, 0x80000000, 0x104 …); 12 pool agotado/casi agotado en el giro; 13 cuadrantes del giro; 14 y 15 mezclas. **Saneado:** las componentes finitas y no nulas de `d = ruta[0] − peatón` se mantienen en [1e-9, 1e9] (ver riesgos).

## Suposiciones y riesgos

- **`CVector::NormaliseAndMag` de la DLL no es la del EXE en los extremos.** Es C++ en `float` (SSE): si `x² + y² + z²` desborda (|d| > ~1e19) o se anula (|d| < ~1e-19) da otro resultado que la x87 del EXE (exponente extendido). Como en ambos lados del banco esa función es la de la DLL (el hook está activo), una diferencia aquí aparecería como «oráculo ≠ obtenido» en **ambos** lados; por eso las fixtures mantienen `d` fuera de esa zona (inf/NaN sí se prueban: dan lo mismo en SSE y x87). La exactitud en esos extremos queda fuera de este encargo (pertenece a `CVector::NormaliseAndMag`).
- Si la DLL **inlinease** `AllocateMatrix`/`UpdateMatrix`, los falsos no se invocarían y saldría diferencia en `calls`.
- El falso de 0x6551D0 devuelve el `float` por ST0 (`fld`): un NaN de señalización de la fixture sale como NaN silencioso en ambos lados (`routeDist` sólo se compara).
- Si el único campo distinto en `tasks` es de `CTaskSimpleAchieveHeading` (+0x14, relleno 0xCD → 0xCC), es un defecto del constructor reconstruido y no de esta función.
- Un contador de ruta enorme o raro (hasta 0xFFFFFFFF) sólo se usa para «distinto de cero»; la función no lo valida (como el EXE).
- `CPool::SetDealWithNoMemory(true)` se activa durante el banco (y se restaura); el reloj se fija por caso y se restaura.
- `offsetof` y tamaños verificados con `static_assert` en `LiveComparison.hpp` y las arenas del C++ frente al oráculo en `fixture_mirror.py`.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `fixture_mirror.cpp`, `fixture_mirror.py`, `register_hook.py`, `source.patch`, `source-all.patch`, `README.md`.
