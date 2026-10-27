<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco SpriteAddBuf28: CSprite2d::AddToBuffer (0x728200)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175346-263426`: 4096 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-spriteaddbuf28\20261018-175346-263426`; archivado en `reversing/CppTwelveLevel6Final/runs/SpriteAddBuf28-20261018-175346-263426/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CSprite2d,AddToBuffer,0x00728200,1,0,1,S` en `docs/hooks.csv`).

Plantilla: `reversing/ModifySpeed27` y `reversing/FileLoaderLoad2dEffect27`; contrato en `reversing/_handoff/BRIEF.md` y política en `reversing/CppOnlyMigration/PROCESO.md`.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh SpriteAddBuf28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/SpriteAddBuf28/build_probe.py
python -B reversing/SpriteAddBuf28/run_local.py
python -B reversing/SpriteAddBuf28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/SpriteAddBuf28/fixture_oracle.py       # autoprueba del oráculo + cobertura de las fixtures (sin juego)
python -B reversing/SpriteAddBuf28/register_hook.py <carpeta-de-corrida>   # SOLO tras aprobar (plantilla)
```
Requiere el ASI del árbol actual con `RH_ScopedInstall(AddToBuffer, 0x728200)` (añadido en `source/game_sa/Sprite2d.cpp`). Fuente aplicable sobre HEAD: `source.patch` (sólo esta función; `git apply --check` comprobado contra `git show HEAD:source/game_sa/Sprite2d.cpp`). 4096 pares.

## Qué hace la función (desensamblado 0x728200..0x7282B3, cdecl, 10 dwords, `ret` sin limpiar, sin retorno)

1. `esi = nextBufferVertex (0xC8045C)` (leído **antes** de llamar). Llama a `0x727A00` (`CSprite2d::SetVertices(RwD3D9Vertex*, const CRect&, 4 x const CRGBA&, 8 floats)`) con `ptr = 0xC5F958 + esi*0x1C` (imul de 32 bits), `posn`, el **mismo puntero** `color` cuatro veces y `u1 v1 u2 v2 u3 v3 u4 v4`.
2. `eax = nextBufferIndex (0xC80458)` (leído **después** de la llamada). Escribe seis `uint16` en `0xC4B958 + eax*2`: `[+0]=esi, [+1]=esi+1, [+2]=esi+2, [+3]=esi+3, [+4]=esi, [+5]=esi+2` (truncados a 16 bits).
3. `nextBufferVertex = esi+4`, `nextBufferIndex = eax+6`.
4. Si `esi' > 0x5FC (1532)` **o** `eax' > 0xFFA (4090)` (comparaciones con signo, sobre los valores ya incrementados) salta (`jmp`, llamada de cola) a `0x7273D0 RenderVertexBuffer`; si no, `ret`.
   `0x7273D0` dibuja y pone ambos contadores a 0 sólo si `nextBufferVertex > 0` (con signo).

Fuente (ver `source.patch`): lee `nextBufferVertex`, llama a `SetVertices(&aRadiosityVertexBuffer[vertex], posn, color x4, uv...)`, **luego** lee `nextBufferIndex`, escribe los seis índices en el mismo orden de pares que el EXE, actualiza ambos contadores y, si `IsVertexBufferFull()` (que implementa exactamente `v > 1532 || i > 0xFFA` con signo), llama a `RenderVertexBuffer()`.

## Revisión propia contra el EXE (fase 2)

Contraste línea a línea del desensamblado de 0x728200 con el borrador del árbol:
- Orden y posición de los 14 argumentos de `0x727A00`: simulada la pila instrucción a instrucción (entrada E: posn=E+4, color=E+8, u1=E+C … v4=E+28) → pila final `ptr, posn, color x4, u1, v1, u2, v2, u3, v3, u4, v4`: coincide con `SetVertices(RwD3D9Vertex*, …)` de la fuente. El color va el mismo puntero cuatro veces (el banco registra la máscara de igualdad de punteros).
- Lectura de `nextBufferVertex` antes de la llamada y de `nextBufferIndex` después: igual en la fuente (los falsos y el real no los modifican, así que no es observable, pero se conserva el orden).
- Los seis stores: `0xC4B95A`=`[1]`←`esi+1`, `0xC4B958`=`[0]`←`esi`, `0xC4B960`=`[4]`←`esi`, `0xC4B95C`=`[2]`←`esi+2`, `0xC4B95E`=`[3]`←`esi+3`, `0xC4B962`=`[5]`←`esi+2`: coinciden con el borrador.
- Condición de relleno: `cmp esi,5FCh; jg` y `cmp eax,0FFAh; jle` (con signo) = `IsVertexBufferFull()` de la fuente (0x7273A0, mismos umbrales, ya validada en `Sprite2d33`); la cola `jmp 7273D0` = `RenderVertexBuffer()` (cdecl sin argumentos).
- Constantes y tamaños: `aRadiosityVertexBuffer = 0xC5F958` (28 bytes/vértice, `sizeof(RwD3D9Vertex)`), `aTempBufferIndices = 0xC4B958` (`RxVertexIndex` = `uint16`), `TOTAL_RADIOSITY_VERTEX_BUFFER = 1532`.
- **Discrepancias encontradas: ninguna en el borrador.** Cambio realizado: sólo se añadió `RH_ScopedInstall(AddToBuffer, 0x728200)` (el borrador sustituía el cuerpo pero no registraba el hook, así que la función seguía llamando al EXE a través de la envoltura original sin hook; con el hook, el EXE entra en la DLL).
- Registros de salida: los cuatro llamadores del EXE (0x718C2A, 0x718D05, 0x718D7E, 0x718E39, todos dentro de `CFont::PrintChar`, ya en C++) hacen `add esp,28h; pop ebx; add esp,20h; ret` justo después de `call 0x728200`: **ninguno lee EAX/ECX/EDX** (ni ESI: la función lo guarda con `push/pop`). Por eso el banco sólo registra el residuo de EAX/ECX/EDX (`kCompareVolatileRegisters=false`) y la fuente no lo reproduce. ESI/EBX/EDI/EBP/ESP/DF sí se exigen.
- Sin x87 en la función (los floats se pasan como dwords): la fuente no necesita opciones de compilación especiales; el estado x87 se compara normalizado por si el compilador usara alguna instrucción.
- Riesgo conocido de la fuente (UB teórico, no del EXE): `vertex + 4` desborda `int32` sólo con `nextBufferVertex > 0x7FFFFFFB`, valor imposible en el juego; las fixtures no pasan de `0x7FFFFFFB`.

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Colaboradores sustituidos por falsos idénticos en el EXE y en el cuerpo de la DLL** (`Resolve` salta los `E9` de enlace incremental):
  - `0x727A00 SetVertices` (cdecl, 14 dwords). Traza de 19 dwords: `[3, off, rect x4, dword apuntado por cada color x4, máscara de punteros iguales al primero, u1..v4]` con `off = ptr - 0xC5F958` (mod 2^32). Si los 4 vértices caben en el búfer escribe 28 dwords derivados de todos los argumentos (`Mix` en cadena), lo que fija posición y contenido de la ventana de vértices.
  - `0x7273D0 RenderVertexBuffer` (cdecl, sin argumentos). Traza `[2, nextBufferVertex, nextBufferIndex, crc32(vértices usados), crc32(índices usados)]`; reinicia ambos contadores sólo si `nextBufferVertex > 0`, igual que el real. Así se comprueba que los índices y vértices ya están escritos cuando se dibuja y que el vaciado ocurre en el momento correcto.
  - `IsVertexBufferFull` (0x7273A0) va en línea en el EXE y es una función pura de dos enteros en la DLL: no se sustituye.
- Búferes **completos** de vértices (42 896 bytes) e índices (8192 bytes) del EXE rellenos con un patrón conocido por caso (`byte[i] = (i*7+vseed)&255` / `(i*13+iseed)&255`): se comprueba que fuera de las ventanas escritas (4 vértices; 6 índices) todo sigue idéntico (`outside_ok`), más 32 bytes de guarda a cada lado del búfer de índices y la arena de argumentos (`CRect` 16 bytes + `CRGBA` 4 bytes con guardas `0xC3`, no deben modificarse: `guard_ok`).
- Contadores `nextBufferVertex/nextBufferIndex` tras la llamada, ventana de 6 índices, ventana de 112 bytes de vértices (o vacía si cae fuera del búfer) y número de llamadas a cada colaborador (1 y 0/1).
- ESP (`ret` sin limpiar: `popBytes = 40` en `Gate`), EBX/ESI/EDI/EBP sembrados y conservados, DF = 0.
- x87: sin igualdad cruda de FNSAVE; se compara FCW completa, TOP, etiquetas y los ST válidos; se ignoran banderas de FSW, FIP/FCS/FOP/FDP/FDS y registros vacíos. MXCSR sin banderas (`& 0xFFC0`).
- EAX/ECX/EDX de salida sólo se registran (`original_residue`, `candidate_residue`).
- Restauración: búferes de vértices e índices, guardas, contadores (a su valor inicial real tras cada lado), entradas de código vigiladas (0x728200, 0x727A00, 0x7273D0 y los dos cuerpos de la DLL), hook (`HookRoute::Restore`), x87/MXCSR. El estado inicial real se guarda antes del primer caso y se vuelve a comprobar al final (`global_state_final`).

## Fixtures (4096 casos; `Fixtures.hpp`, espejo exacto en `fixture_oracle.py::make_f`)

32 familias (`kind = n % 32`, 128 casos cada una): búfer vacío; `nextBufferVertex` 1527/1528 (no lleno) y 1529..1535 (lleno por vértices, ventana fuera del búfer); `nextBufferIndex` 4084 (no lleno) y 4085..4090 (lleno por índices); llenos los dos; alineados como el juego (de 4 en 4 y de 6 en 6); justo en el límite de ambos; valores «raros» de `nextBufferVertex` (negativos, truncamientos de 16 bits 0xFFFC..0x10003, 100000, 0x7FFFFFFB, `INT_MIN`…) con y sin lleno por índices (`Render` con contadores ≤ 0 no reinicia); resto aleatorio. Floats (rect y uv): especiales (±0, ±1, ±inf, NaN silenciosos, denormales, FLT_MAX…), bits crudos silenciosos y valores moderados; cada 64 casos (`n%64==5`) un NaN **señalizante** en una ranura (ver riesgos). Colores: extremos y aleatorios. Semillas de pila (64 dwords bajo ESP), EAX/ECX/EDX por caso. `nextBufferIndex` se mantiene en 0..4090 y `nextBufferVertex` puede ser cualquier valor de 32 bits hasta 0x7FFFFFFB (la ventana de vértices sólo se escribe si cabe; el EXE y la DLL sólo calculan la dirección).

`fixture_oracle.py` modela la función desde el desensamblado (arriba), se autoprueba sin juego y calcula la cobertura; `run_local.py` exige mínimos calibrados con ella (valores del espejo: lleno por vértices 546, por índices 519, ambos 205, ninguno 3236, Render 860, con reinicio 827, sin reinicio 33, ventana fuera del búfer 626, vértice negativo 104, > 0xFFFF 97, truncamiento a 16 bits 235, bordes 479/447; cada familia 128; los mínimos dejan 20-45 % de margen).

## Suposiciones y riesgos del banco (no compilado)

- `Sprite2d.h`, `CutsceneMgr.h`, `ProbeRuntime.hpp`, `HookRoute.hpp` se incluyen desde `LiveComparison.hpp`; el hook se busca como categoría `CSprite2d`, nombre `AddToBuffer` (`RH_ScopedInstall` sin sobrecarga). `CSprite2d::SetVertices` tiene varias sobrecargas: se elige la de `RwD3D9Vertex*` por conversión explícita; `RenderVertexBuffer` es estática (puntero de función normal).
- Se asume que `AddToBuffer` de la DLL llama directamente (no inlineada) a `SetVertices(RwD3D9Vertex*…)` y a `RenderVertexBuffer` (compilación /Od del proyecto) y que `Resolve` atraviesa los `E9`.
- **NaN señalizantes (64 casos, `n%64==5`)**: la DLL (SSE, /Od) copia los floats con `movss`, que conserva los bits; si el compilador usara x87 (`fld/fstp`) los silenciaría y esos casos fallarían. Sería una diferencia real (sólo para entradas que el juego nunca produce); no reducir el banco: si ocurre, es información. Si sólo fallan esas filas, aislar la causa antes de aceptar.
- `nextBufferIndex` en las fixtures está en 0..4090 (la función escribe 6 `uint16` desde ahí; con más rebasaría el búfer de 4096). `nextBufferVertex` negativo o enorme sólo afecta a la dirección calculada y a los valores truncados a 16 bits (no se desreferencia: el falso de `SetVertices` sólo escribe si la ventana cabe en el búfer).
- Los búferes de vértices/índices del EXE se sobrescriben con patrones durante el banco y se restauran bit a bit tras cada lado (el banco corre dentro de `CGame::Process`, un solo hilo de lógica; si algún hilo del motor los tocara concurrentemente aparecería como `globals_restored` falso).
- Estimación no comprobada: `Crc32` bit a bit sobre ≤ 51 KB por llamada a Render (≈ 860 casos × 2 lados) añade pocos segundos.
- Sin arnés `offline/` ni compilación: la fuente del banco no se ha compilado; el oráculo y las fixtures sí se probaron en Python (`python -I fixture_oracle.py`: «ok» y 4096 fixtures válidas).

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `README.md`.
