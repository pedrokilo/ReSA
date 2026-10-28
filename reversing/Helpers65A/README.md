<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Helpers65A: cinco ayudantes pequeños del EXE reconstruidos en C++ (lote 65)

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wr-1`, HEAD 633dceef0 + fuente de este lote). 20480 pares (5 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración en ambos lados y salida de cada lado igual a la del oráculo independiente. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helpers65a\20261021-025714-674507`.

Cinco funciones del EXE sin símbolo que el código fuente aún llamaba por dirección (`plugin::Call*` / puntero a función con la dirección). Se dedujeron del desensamblado, se les dio nombre, se reconstruyeron en C++, se sustituyeron las llamadas por la función C++ y se registró el hook.

| Función | Dirección | Tipo | Dónde vive | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CHud::FloorFloat` (`floorf` del CRT) | 0x406CE0 | Simple (`RH_ScopedGlobalInstall`) | `Hud.cpp` (junto a `DrawVitalStats`) | floor del CRT del EXE (0x8219F0) |
| `RwHelper::GetAnimHierarchyFromFrameCallback` | 0x734A70 | Simple (`RH_ScopedGlobalInstall`) | `RwHelper.cpp` (ya estaba reconstruida; faltaba el hook) | `RpHAnimFrameGetHierarchy` (0x7C5160, en C++) y `RwFrameForAllChildren` (0x7F0DC0) |
| `CDecisionMaker::MakeDecision` | 0x606060 | Simple (`RH_ScopedInstall`) | `DecisionMaker.cpp/.h` | `CDecision::MakeDecision` (0x6040D0, en C++) |
| `CDecisionMakerTypes::ReadEventIndices` | 0x5BB9F0 | Simple (`RH_ScopedInstall`, static `__stdcall`) | `DecisionMakerTypes.cpp/.h` | `CFileMgr::SetDir` (0x5387D0), `OpenFile` (0x538900), `ReadLine` (0x5389B0), `CloseFile` (0x5389D0), todas en C++, y el `sscanf` del EXE (0x8220AD) |
| `UserTracksLegacyVector::PushBack` | 0x4F4620 | Simple (`RH_ScopedInstall`, struct anidada en `CAEUserRadioTrackManager::InjectHooks`) | `AEUserRadioTrackManager.cpp/.h` | 0x4F3B30 (fill) y 0x4F4320 (insert) del vector VC6 |

No se ejecuta E/S real: ningún archivo se abre ni se lee (`CFileMgr` y `sscanf` son falsos que sirven las líneas del caso) ni se reserva memoria del EXE (0x4F4320 es falso).

## Qué hace cada una (desensamblado)

- **0x406CE0 `FloorFloat`**: `fld dword [esp+4]; sub esp,8; fstp qword [esp]; call 0x8219F0; add esp,8; ret`. Amplía el float a double (exacto, NaN silenciados) y devuelve en ST0 el resultado del floor del CRT sin tocarlo. En C++: `double FloorFloat(float)` llama a `0x8219F0` (CRT sin reconstruir) con `double(value)`; devuelve `double` (si devolviera `float` el resultado ya no sería idéntico al ST0 del EXE). Llamadores del EXE: 0x4D8793 (`CAEAudioHardware::PlaySound`), 0x570689/0x5706AA/0x5706C9/0x5706E8 y 0x589BF5: solo leen ST0 (cada uno sigue con `_ftol2` o `fmul`).
- **0x734A70 callback de `RwFrameForAllChildren`**: si `RpHAnimFrameGetHierarchy(frame)` no es nulo lo guarda en `*data` y devuelve 0 (detiene el recorrido); si no, recorre los hijos con ella misma y devuelve el marco. El iterador (0x7F0DC0) solo lee EAX.
- **0x606060 `CDecisionMaker::MakeDecision`**: `ecx = this + arg1 * 0x3C` (imul de 32 bits) y llama a `CDecision::MakeDecision` (0x6040D0) con los otros ocho argumentos de pila, en el mismo orden. En C++: `m_aDecisions[eventIndex]` sin comprobación de límites. Único llamador: `CDecisionMakerTypes::MakeDecision` (ped), dos sitios.
- **0x5BB9F0 `ReadEventIndices`**: pone a cero los 96 enteros de `indices`; `SetDir("")`, `SetDir("data\decision\")`, `OpenFile(nombre, "r")`, `SetDir("")`; lee con `ReadLine(f, buf, 256)` y, para cada línea con `buf[0]` distinto de 0 y de `\n`, `sscanf(buf, "%s %d", palabra, &idx)` y `indices[idx] = n++`; `CloseFile(f)`. `idx` conserva su valor si `sscanf` no lo escribe (el EXE lo deja en la pila; la primera línea analizada del banco siempre lo escribe, porque su valor inicial en el EXE es basura de pila). El resultado de `OpenFile` no se comprueba. Único llamador: `CDecisionMakerTypes::LoadEventIndices` (0x600840).
- **0x4F4620 `PushBack`**: push_back del vector VC6 de punteros (`{allocator, first, last, end}`, 16 bytes). Con `first != 0` y `(last-first)>>2 < (end-first)>>2` (sar con signo, comparación sin signo) llama a `0x4F3B30(last, 1, &valor, this, &valor)` (cdecl, cinco argumentos) y guarda `last_leído_antes + 4`; si no, a `0x4F4320(this; last, 1, &valor)` (thiscall, `ret 0Ch`; reserva con el operator new del EXE). Único llamador: `WriteUserTracksFile` (dos sitios; la versión Unicode NOTSA no usa el vector del EXE).

## Revisión propia contra el EXE (fase 2)

Se desensambló de nuevo cada rango (`0x406CE0..0x406CF2`, `0x734A70..0x734AA2`, `0x606060..0x606098`, `0x5BB9F0..0x5BBAB2`, `0x4F4620..0x4F4683`, y los colaboradores `0x8219F0`, `0x4F3B30`, `0x4F4320`) y se contrastó línea a línea con el C++. Discrepancias en la fuente: ninguna tras el contraste. Detalles verificados: orden de las llamadas de `ReadEventIndices` (el `rep stosd` va antes de `SetDir`); la primera lectura va antes del bucle; el índice se lee de la pila tras `sscanf`; la cadena `"%s %d"` (0x86A6AC), `"data\decision\"` (0x86A6B4), `"r"` (0x85A53C) y `""` (0x858B54) leídas del EXE; en `PushBack`, `last` se lee antes de la llamada (un falso que reescriba `last` no cambia el resultado: lo comprueba el banco); y en `MakeDecision`, el orden de empuje de los ocho argumentos.

La conversión `bool` del tercer argumento de `CDecision::MakeDecision`: el EXE reenvía el dword de pila tal cual; la DLL pasa un `bool`. El banco registra su veracidad (byte bajo != 0), que es lo único que lee el cuerpo (`bIsPedInVehicle ? 1 : 0`): fixtures con 0, 1, 2, 0xFF, 0x100, 0x101, 0x80, 0xFFFFFFFF, 0xFFFFFF00 y aleatorios.

## Qué compara

`Original` = bytes del EXE con el hook RH de la función apagado; `candidato` = DLL con **solo** ese hook encendido. Por lado: eventos de los falsos EN ORDEN con sus argumentos (los punteros a la arena como etiquetas; cadenas por hash FNV-1a y longitud), cambios de la arena de 4 KB palabra a palabra frente a la imagen previa (basura determinista salvo los campos del caso: cualquier escritura no prevista aparece), ST0 de `FloorFloat` (los 10 bytes del registro), nodo devuelto por el callback y compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, cadena SEH, x87 normalizado, ruta del hook, colaboradores). EAX/ECX/EDX solo se registran (ningún llamador del EXE los lee: `residue_scan.py` sobre 0x406CE0, 0x734A70 [sin llamadores directos], 0x606060, 0x5BB9F0 y 0x4F4620 no encuentra lecturas; el iterador 0x7F0DC0 solo lee EAX del callback, que sí se compara).

Fixtures (`Fixtures.hpp`, 12 familias por función, 4096 casos cada una): `FloorFloat`: normales, fracciones, enteros, cuartos, ±0/±inf, denormales, NaN silenciosos y señalizantes con carga, ≥ 2^23, fronteras, bits cualesquiera, ±1 ulp de enteros, y 8 modos del falso (±0, eco, mantisa revuelta, normal, denormal, ±inf, NaN con carga); callback: árboles de 2..10 marcos (cadena, estrella, aleatorios) con 0..n jerarquías, raíz y `*data` iniciales, retorno basura del iterador; `MakeDecision`: índices pequeños, 0, 40, negativos, 0x7FFFFFFF y valores donde `idx * 0x3C` da la vuelta, 4 argumentos de tarea (-1, 200, aleatorios), bool no canónico y el falso que escribe cero, uno o los dos `int16&`; `ReadEventIndices`: 0..14 líneas (normales, vacías `\n`, vacías `\0`, de primer carácter distinto, sin salto final, de 253 caracteres), índices -64..350 (escrituras fuera de los 96 enteros), índice previo reutilizado, nombres de 0..40 caracteres, `OpenFile` nulo; `PushBack`: sin almacenamiento, vacío con sitio, con sitio, lleno, `last > end`, `last < first`, aritmética que da la vuelta, diferencias no múltiplos de 4, con el falso reescribiendo `last`/`first`/`end`. `fixture_oracle.py` modela las cinco desde el desensamblado (autoprueba con `python -I fixture_oracle.py`); `fixture_mirror.py` compila `Fixtures.hpp` con `cl` sin el juego, pasa los 20480 casos por el oráculo, fija las constantes de la arena y comprueba que `FloorRetBits`, `WidenFloat` y `Garbage` de C++ coinciden con Python. Cobertura mínima calibrada en `run_local.py` (≈50 % de lo medido).

## Efecto sobre bancos anteriores (comprobado el 21/10/2026 con la fuente de este lote)

- `DependenciesDecisionRelationship6` (usa 0x5BB9F0 como ayudante): **APROBADO** sin cambios (`bench_here.sh DependenciesDecisionRelationship6`).
- `DependenciesDecisionPed7`: da «Error dentro de GTA: native decision/RNG helper route changed» antes de comparar nada. Su precondición (`NativeHelpers()`: 0x6040D0 y 0x606060 sin salto a la DLL) ya no se cumple; con HEAD sin este lote ya fallaba por 0x6040D0 (`CDecision::MakeDecision`, enganchada en el lote DecisionMakeDecision27). `CDecisionMakerTypes::MakeDecision` (ped, 0x606E70) necesita un banco nuevo que sustituya 0x6040D0 y 0x606060 por falsos en la dirección del EXE y en el cuerpo de la DLL.
- `HudVitals355` (`CHud::DrawVitalStats`): **FALLO 57/1024** (`E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hud-vitals355/20261021-030753-505823`). Causa verificada fila a fila: su falso de 0x406CE0 (evento `round`) ya no se ejecuta en el lado de la DLL porque `DrawVitalStats` llama ahora a `FloorFloat` (que llama al floor del CRT 0x8219F0); el original sí lo registra. Es la única diferencia en las 57 filas (`trace`, un `round` de menos). Hay que parchear también el cuerpo de `FloorFloat` (`GetHookOurAddress()` del hook `CHud/FloorFloat`) con ese mismo falso en `reversing/HudVitals355/LiveComparison.hpp`. Su `build_probe.py` además no fija `runtime_runner.GAME` (con `bench_here.sh` falla con `BUILD FAIL`).

## Suposiciones y límites

- Los cuerpos reales de los colaboradores no se validan aquí (CRT floor, `RwFrameForAllChildren`, los del vector VC6; `CFileMgr`, `RpHAnimFrameGetHierarchy` y `CDecision::MakeDecision` tienen sus propios bancos). No hay E/S real.
- `FloorFloat` no ejecuta el floor real del CRT: el banco prueba el envoltorio (el argumento llega como el double exacto de `fld dword; fstp qword`; el ST0 que devuelve el CRT sale sin cambios, también NaN/denormales/valores que un `float` no representa). El floor real del CRT lo usan sin cambios ambos lados.
- Un hilo ajeno al banco que pase por 0x8219F0 mientras está parcheado recibe el `floor` de la DLL (el falso comprueba el id de hilo).
- `ReadEventIndices`: si `sscanf` no escribe el índice de la primera línea analizada, el EXE usa basura de la pila y la DLL usa 0: no se prueba (la primera línea analizada siempre lo escribe).
- `PushBack` sobre vectores con punteros que se desreferenciarían (0x4F3B30 y 0x4F4320 son falsos): los punteros del vector son enteros arbitrarios; la reserva y la copia reales no se prueban.

## Comandos

```
pwsh -NoProfile -File D:\Original\wr-1\reversing\CVehicle\build-project.ps1
bash reversing/_handoff/bench_here.sh Helpers65A           # compila la sonda y corre en el juego (candado global)
python -I reversing/Helpers65A/fixture_oracle.py           # autoprueba del oráculo
python -I reversing/Helpers65A/fixture_mirror.py           # coherencia de las fixtures + cobertura mínima (sin el juego)
python -B reversing/Helpers65A/run_local.py --analyze <comparisons.jsonl>
python reversing/Helpers65A/register_hook.py <carpeta-de-evidencia>   # SOLO tras una corrida aprobada (lo ejecuta el coordinador)
```

Archivos: `Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `fixture_oracle.py`, `fixture_mirror.cpp/.py`, `run_local.py`, `build_probe.py`, `register_hook.py`, `source.patch` (10 archivos de `source/`, aplicable con `git apply` sobre HEAD 633dceef0).
