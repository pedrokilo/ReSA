<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PathFind62: FindNodeClosestToCoors, FindNthNodeClosestToCoors, FindNextNodeWandering, CompleteNewInterior, FindParkingNodeInArea y GeneratePedCreationCoors_Interior

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\we-1`, HEAD 0ac292a76 + fuentes del lote 62). 24576 pares (6 x 4096), cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados, salida de cada lado igual a la del oráculo independiente (`fixture_oracle.py`, desde el desensamblado) y `fsin`/`fcos` del x87 igual a `sin`/`cos` de la DLL para los 256 valores de `dir`.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-pathfind62\20261020-004108-815613`.

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CPathFind::FindNodeClosestToCoors` | 0x44F460 | Simple (antes comentado con la dirección equivocada 0x44FA30) | 4096 | búsqueda por región 0x44F2C0 (sin reconstruir; la DLL la llama por dirección) |
| `CPathFind::FindNthNodeClosestToCoors` | 0x44F8C0 | Simple | 4096 | `FindNodeClosestToCoors` 0x44F460 (con hook encendido) |
| `CPathFind::FindNextNodeWandering` | 0x451B70 | Simple | 4096 | `FindNodeClosestToCoors` 0x44F460 (con hook encendido); `CPathNode::GetPosition` 0x420A10 va con código real |
| `CPathFind::CompleteNewInterior` | 0x452270 | Simple | 4096 | `CMemoryMgr::Malloc` 0x72F420 (con hook), 0x450FB0 (sin reconstruir), `AddDynamicLinkBetween2Nodes_For1Node` 0x44E000 (con hook) |
| `CPathFind::FindParkingNodeInArea` | 0x4513F0 | Simple | 4096 | ninguno (`CPathNode::GetPosition` 0x420A10 con código real en el EXE) |
| `CPathFind::GeneratePedCreationCoors_Interior` | 0x44ECA0 | Simple | 4096 | `rand()` del CRT del EXE 0x821B1E (sin reconstruir), `CWorld::FindGroundZFor3DCoord` 0x5696C0 (con hook); `FindRegionForCoors` 0x44D830 con código real |

```
bash reversing/_handoff/bench_here.sh PathFind62                                       # compila la sonda y corre el banco (candado global del juego)
# o a mano, siempre en serie:
python -B reversing/PathFind62/build_probe.py
python -B reversing/PathFind62/run_local.py
python -B reversing/PathFind62/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -I reversing/PathFind62/fixture_mirror.py                                          # compila Fixtures.hpp con cl: arenas/constantes C++ == oráculo + cobertura >= mínimos
python -B reversing/PathFind62/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pathfind62/20261020-004108-815613"   # sólo lo ejecuta el coordinador
```
Filas de `docs/hooks.csv` (las añade el coordinador): `CPathFind,<función>,<dirección>,1,0,1,S` para las seis. `SCOPE_NOTE` de `register_hook.py` ya está rellenado.
Archivo de arranque en el clon: `pathfind62-runtime.start`; salida `pathfind62-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de estas seis funciones respecto a HEAD; `git apply --check` contra el blob de HEAD correcto)

`source/game_sa/PathFind.cpp` (sin cambios en el `.h`) y, al final, `X87Math.cpp/.h` (bloque «Lote 62»: `PathFindSubCpp`). Las seis eran `plugin::CallMethod*` hacia el EXE con borradores del lote 27 sin compilar en el árbol principal:
se leyeron con `git diff HEAD`, se contrastaron con el desensamblado y se corrigieron (ver «Fase 2»). Añade `detail::ExeRand` (`rand()` del CRT del EXE, 0x821B1E: la `rand` de la DLL es otro generador)
y `s_parkingNodeCursor` (global 0x977B9C), corrige `detail::RegionValueOf` (ver discrepancia 6) y activa los seis `RH_ScopedInstall` (el de `FindNodeClosestToCoors` apuntaba a 0x44FA30, que no es esa función: es 0x44F460).
Los colaboradores sin reconstruir (0x44F2C0 búsqueda por región, 0x450FB0 enlace de nodos interiores) se llaman por dirección del EXE; los reconstruidos (`FindRegionForCoors`, `AddDynamicLinkBetween2Nodes_For1Node`, `CMemoryMgr::Malloc`, `CWorld::FindGroundZFor3DCoord`, `FindNodeClosestToCoors`) en C++.

## Fase 2: revisión contra el EXE (re-desensamblado y contrastado instrucción a instrucción)

**FindNodeClosestToCoors (0x44F460..0x44F8BB, `ret 2Ch`).** Resultado local `[esp+14h]` con la mitad de área en 0xFFFF y la de nodo **sin inicializar** (residuo de pila); `dist` local = `maxDistance`.
`rx = clamp(ftol((x + 3000) · 0.0013333333), 0, 7)` (el `_ftol` 0x821B40 devuelve los 32 bits bajos del `fistp` de 64 bits), idem `ry`; distancia mínima al borde (oeste, este, sur, norte; cada comparación conserva la anterior sólo si es estrictamente menor);
búsqueda de la región `rx + ry·8` (0x44F2C0 con 11 argumentos: resultado, área, posición, tipo, `&dist`, `unk2`, `unk4`, barcos, `unk6`); si `dist > minDist` (orden estricto) recorre hasta 4 anillos (oeste, este, sur, norte sin esquinas, cada área sólo si está en 0..7)
sumando 750 a `minDist` tras cada anillo y saliendo si `dist < minDist`; si el byte bajo de `unk6` es 0, busca además las áreas 0x40..0x47 pasando `unk6 = 0`. Devuelve el dword resultado (con su mitad de nodo sin tocar) por el puntero oculto y EAX = puntero.
Los 20 llamadores del EXE usan EAX como puntero al resultado (`mov eax,[eax]`) y sólo miran la mitad de nodo si el área es válida (o la copian tal cual en `FindNearestExteriorNodeToInteriorNode`).

**FindNthNodeClosestToCoors (0x44F8C0..0x44FA2D, `ret 30h`).** Recorre las 72 áreas con nodos: tipo 0 → `[0, vehículos)`, tipo 1 → `[vehículos, total)`, otro tipo → no limpia nada (el EXE compara dos registros que valen `nthNode`: `cmp edi,ebp; jge`);
a cada nodo le quita el bit 1 del byte +0x19. Si `nthNode > 0` repite `res = FindNodeClosestToCoors(pos, tipo, maxDist, bajoTráfico, unkn, 1, barcos, ignorarInterior)`, `*salida = res` si hay puntero, `res.área == -1` → devuelve `res`, si no marca (`|= 2`) el nodo y `--nthNode > 0`.
Al final devuelve `FindNodeClosestToCoors(..., unk6 = 0)` directamente en el puntero oculto. Los argumentos `bool` llegan al colaborador como dwords completos (el banco los prueba canónicos 0/1).

**FindNextNodeWandering (0x451B70..0x451FF6, `ret 20h`).** Origen nulo o con área −1 → busca (`FindNodeClosestToCoors(pos, tipo, 999999.875f, 0, 0, 0, 0, 0)`); origen con área sin cargar → destino.área = −1, `*outDir = 0` y vuelve sin tocar nada más;
si no, `dist² = (dz² + dy²) + dx²` (redondeada a float), radio `max(7.0, anchura · 0.125)` y `sqrt(dist²) > radio` → busca, si no el nodo actual es el origen. Sin nodo (área −1 o sin cargar): destino.área = −1, `cur.área = −1`, `*outDir = 0`, `*destino = cur` y no escribe `*origen`.
Con nodo: `ángulo = float(dir) · 0.7853982`, `sin`/`cos` x87 guardados como float; para cada vecino cargado (se salta el apagado salvo que el nodo también lo esté, y el «no deambular» salvo que el nodo también lo sea) `n = (otro − nodo)` normalizado
(`1.0 / sqrt(dy² + dx²)`, sin guardas: nodos coincidentes dan NaN) y `dot = ndy·cos + ndx·sin`; el mayor `dot >= mejor` (inicio −999999) gana y fija `outDir` con la clasificación de ocho direcciones (cada rama replica el `fcom` y el salto: `ndx < 0` incluye NaN).
Escribe `*origen = cur` (sin comprobar nulo) y, si no hubo vecino, `outDir = 0`, destino = `cur`. Si el destino es el nodo origen original, se toma el primer vecino con área cargada.

**CompleteNewInterior (0x452270..0x452750, `ret 4`).** `*salida.área = −1` si hay puntero; `NumNodesGiven == 0` → sólo `bInteriorBeingBuilt = 0`. Inundación = `slot + 100` o la del primer nodo exterior enlazado (byte +0x17);
`m_interiorIDs[slot] = InteriorIDBeingBuilt`; `Malloc(n · 0x1C, 0)` en `m_pPathNodes[64 + slot]`; por nodo: posición `_ftol(coord · 8.0)` (16 bits bajos), +0x12 = slot + 0x40, +0x14 = i, +0x16 = 0, +0x17 = inundación, +0x18 = `((b & 0xEF) | 0x20) & 0xBF & 0x7F`,
+0x19 = `((b | 1) & 0xFD)` con el bit 2 = `noDeambular`, luego `| 8` y `& 0xCF`, +0x1A = 0x0F, +0xE = 0x7FFE; simetría de las conexiones (fila de `c` sin `i` → primer hueco negativo); recuento de conexiones ≥ 0;
`Malloc((n+0xC0)·4)`, `Malloc(n+0xC0)` ×2 (enlaces, longitudes, intersecciones); por nodo `+0x10 = contador`, `&= 0xF0` y 0x450FB0(slot+0x40, i, slot+0x40, c, slot, &contador) por conexión; cola de enlaces con área 0xFFFF; contadores (total n, vehículos 0, peatones n, enlaces de coche 0, direcciones n) y,
por cada enlace exterior, `AddDynamicLink(interior, exterior)` y `AddDynamicLink(exterior, interior)`. Nada lee EAX/ECX/EDX (el único llamador recarga sus locales).

**FindParkingNodeInArea (0x4513F0..0x4515C9, `ret 1Ch`).** Recorre las 64 áreas del mapa y los nodos de vehículos cuya posición cae estrictamente dentro de la caja (comparaciones ordenadas: NaN descarta) y con tipo de conducta (`byte +0x1A >> 4`) 2;
guarda el primero y el de índice `s_parkingNodeCursor` (global 0x977B9C, comparación de igualdad de 32 bits con el contador); `cursor = cursor + 1 < hallados ? cursor + 1 : 0` (comparación con signo, incluso con 0x7FFFFFFF) y devuelve el elegido, si no el primero, si no (0, 0, 0). El único llamador (0x47376D) copia el vector con `mov ecx,[eax]…`.

**GeneratePedCreationCoors_Interior (0x44ECA0..0x44EFB7, `ret 18h`).** Región de (x, y) con 0x44D830; hasta 200 intentos: área descargada o sin peatones → siguiente intento sin llamar a `rand`; `r = rand() >> 6` (aritmético), nodo = `vehículos + r % peatones`;
`dist² = dy² + dx² < 2500.0` (ordenado), `z > 900.0`, al menos un enlace; por enlace (índice en `m_pNodeLinks[región]`, **no** en el área del nodo) con área < 0x40 y cargada, 5 intentos: `t = (rand() & 255) · 0.00390625` (guardado en `*outOrientation` en cada intento),
`coord = nodo · (1.0 − t) + otro · t` (productos guardados a float, `x`/`z` con tres redondeos como el EXE), `FindGroundZFor3DCoord(x, y, z + 2.0, &enc, nullptr)`; éxito si `enc` (cualquier byte ≠ 0) y `|suelo − z| < 3.0` (ordenado): `z = suelo` y devuelve 1. Agotados los intentos: 0.
`*outCoords` se escribe en cada intento aunque falle. El único llamador (0x61654E) sólo mira AL.

### Discrepancias halladas respecto a los borradores del lote 27 y correcciones
Los seis borradores coincidían con el EXE en la lógica (se compararon instrucción a instrucción y luego los confirmó el banco). Cambios y observaciones:
1. **FindNodeClosestToCoors — registro del gancho:** `RH_ScopedInstall(FindNodeClosestToCoors, 0x44FA30)` (comentado) apuntaba a otra dirección; la función está en 0x44F460.
2. **FindNodeClosestToCoors — mitad de nodo del resultado inválido:** el EXE la deja como residuo de pila; la DLL deja −1 (el valor inicial de `CNodeAddress`). Diferencia inobservable: los 20 llamadores sólo leen el nodo con área válida (o la copian). El banco no la compara.
3. **FindNthNodeClosestToCoors — tipo > 1:** el borrador decía «registros sin inicializar»; en realidad `edi` y `ebp` valen ambos `nthNode` y `jge` salta siempre: no limpia nada (mismo comportamiento, comentario corregido).
4. **FindNextNodeWandering — `origin` sin inicializar:** la copia del origen leía un campo sin inicializar en la comparación final cuando no hay origen; ahora se inicializa a −1 (el resultado no cambia: la condición se corta por `origin.área != 0xFFFF`).
5. **FindNextNodeWandering — `sin`/`cos`:** la DLL usa `(float)std::sin((double)ángulo)`; el banco comprueba en la sonda, para los 256 valores de `dir`, que coincide bit a bit con `fsin`/`fcos` del x87 a 24 bits (`sincos_mismatches = 0`).
6. **Defecto real en `detail::RegionValueOf` (compartido por todas las funciones de región; hallado por el banco):** convertía con `(int32)` (INT_MIN fuera de rango, que el acotado deja en 0) mientras el `_ftol` del EXE (0x821B40) devuelve los
   32 bits bajos del entero de 64 bits; con coordenadas de |p| ≥ ~1.6·10¹² (p. ej. 1e13) la región difería (141 de 4096 pares). Ahora `(int32)(int64)scaled`. Sólo cambia el resultado en esos rangos (más cerca del EXE); no altera ningún valor dentro del mundo.
7. **Defecto real en las restas de `FindNodeClosestToCoors` (hallado por el banco, lección 6 del lote 61):** el SSE del juego corre con MXCSR FTZ y vacía a cero un resultado denormal (`x − 0.0` con x denormal junto al borde 0.0 de la región central),
   mientras el `fsub` del x87 lo conserva: la comparación `dist > minDist` salía distinta. Las cuatro restas van ahora por `PathFindSubCpp` (`X87Math.cpp`, bloque «Lote 62»: `volatile float r = a − b`, compilado `/arch:IA32`).
8. **Banco (no de la función):** los falsos etiquetaban el puntero de retorno oculto como «de pila»: la DLL devuelve por RVO directamente en la arena del llamador y el EXE usa un local (ambos válidos); la etiqueta se retiró. La suma de la arena de nodos
   era 0x380 donde guarda + objeto + guarda = 0x40 + 0x2A0 + 0x40 = 0x320 (corregido y comprobado con `fixture_mirror.py`).

## Qué se compara

Original = EXE con el hook RH de la función apagado; candidato = DLL con **sólo** ese hook encendido. `this` es una arena propia de 0x3C80 bytes (guardas de 0x40) con un mundo falso: hasta 5 áreas (0..71, con áreas descargadas) con nodos de 0x1C bytes, matrices de enlaces y contadores;
`ThePaths` (0x96F050) y los globales que tocan las funciones (0x96EA98..0x96F050, 0x977B70..0x977BA0) se vigilan y se restauran. Los falsos se instalan como parche de entrada en la dirección del EXE y en el cuerpo de la DLL (resuelto antes de crear las rutas,
con todos los hooks encendidos y siguiendo saltos sólo dentro del ASI) y registran sus argumentos **en orden** (los primeros 24 eventos íntegros y un hash de todos). `FindNodeClosestToCoors` se enciende explícitamente mientras se prueban `FindNth…` y `FindNext…`.
Observado por lado: eventos de los falsos, resultado (dword completo, o el área si la mitad de nodo es residuo), parámetros de salida (con guardas), todas las arenas de nodos/enlaces/`this`/Malloc tramo a tramo, contadores, globales, `EAX = puntero oculto` y `AL`,
ESP, EBX/ESI/EDI/EBP, DF, `fs:[0]`, x87 normalizado (FCW completa, TOP, etiquetas, ST válidos; MXCSR sin banderas), ruta (la entrada salta a la DLL sólo en el candidato), `collaborators_dll`, entradas de los colaboradores intactas y globales restaurados.
EAX/ECX/EDX de salida **sólo se registran**; ningún llamador del EXE los lee salvo EAX como puntero oculto (se compara aparte) y AL de `bool`.
Además `run_local.py` exige que la **salida de cada lado** sea igual a la que modela `fixture_oracle.py` desde el desensamblado.

## Fixtures: 6 x 4096 = 16 familias x 256 por función (`Fixtures.hpp`, mismo generador en el juego y en `fixture_mirror.py`)

- **FindNodeClosestToCoors:** posiciones dentro del mapa, exactamente sobre bordes de región (±1 ulp), fuera, enormes (`scaled > 2^32`), NaN/±inf, denormales y ±0; `maxDist` por defecto 999999.875, igual a la distancia al borde (±1 ulp), a `minDist + 750·k`, aleatoria, NaN/±inf/0/negativa;
  guion de la búsqueda por región con distancias en los umbrales del bucle de anillos (orden estricto), áreas vecinas de la 9×9 e interiores; `unk2/unk4/barcos` ≤ 0xFFFF, `unk3` aleatorio (no se usa), `unk6` con byte bajo 0 o no y basura arriba, tipo con basura en los bytes altos.
- **FindNthNodeClosestToCoors:** 1–4 áreas (30 % interiores) con 0–11 nodos y bits +0x19 aleatorios, tipos 0/1/2/3/0xFF/0x80, `nthNode` ∈ {1, 2–4, 0–5, −7, −1, 7, 8, 0x7FFFFFFF, INT_MIN}, con/sin puntero de salida, guion de resultados válidos e inválidos (el inválido corta).
- **FindNextNodeWandering:** 1–3 áreas de 2–8 nodos con enlaces a áreas cargadas, sin cargar y a sí mismos, anchura en el borde del radio 7.0, nodos coincidentes (NaN), nodos apagados / «no deambular», origen nulo / área −1 / válido / sin cargar, posición a distancia 0, radio, radio ±1 ulp, 0.5×, 3×, NaN/inf, `dir` = `n & 255` en la mitad de los casos (los 256 valores), guion del colaborador (nodo válido, inválido, área sin cargar).
- **CompleteNewInterior:** `NumNodesGiven` 0, 1, 2–16, 17–64, 64; slot 0–7; enlaces exteriores 0–4, 8 y −1; coordenadas ordinarias, bordes, enormes (conversión a 64 bits), NaN/inf, denormales; conexiones con índices válidos, ≥ n, negativos distintos de −1 y a sí mismo; con/sin puntero de salida, `bInteriorBeingBuilt` 0/1.
- **FindParkingNodeInArea:** 1–4 áreas (algunas ≥ 64, que se ignoran) con 0–8 nodos de vehículo y 0–2 de peatón, tipos de conducta 0, 1, 2, 3, 0x2F, cajas con cotas exactamente sobre los nodos (estricto), ±0.125, NaN, ±inf, invertidas; cursor 0..5, negativo, 0x7FFFFFFF, 0x80000000, 100.
- **GeneratePedCreationCoors_Interior:** región cargada (o no), 0–6 nodos de peatón con z en 900 exacto y alrededor, distancias de 49.9/50/50.1, enlaces a áreas ≥ 0x40, sin cargar, −1 y válidos; `rand` del guion (cero, 0x7FFF, rampa, alterno, aleatorio); suelo hallado/no con byte 1, 2, 0x80, 0xFF y deltas en ±3 (incluidos NaN/inf); caso «nunca hay suelo» (200 intentos).

Cobertura mínima en `run_local.py` (`MINIMUMS`, ~50 % de lo medido con `fixture_mirror.py`).

## Suposiciones y riesgos

- La mitad de nodo de un `CNodeAddress` inválido devuelto por `FindNodeClosestToCoors` no se compara (residuo de pila del EXE); el resto de resultados (incluidos los de `FindNth…`, cuyo falso escribe ambas mitades) se comparan enteros.
- Los argumentos `bool`/`uint16` se prueban con valores que caben en su tipo; el EXE reenvía el dword completo al colaborador y la DLL el valor truncado, que sólo coinciden mientras la entrada sea canónica (los llamadores del EXE pasan 0/1).
- `FindNextNodeWandering` con puntero de origen nulo sólo se prueba cuando no hay nodo (el EXE escribe en `*origen` sin comprobarlo). `NumNodesGiven` > 64 de `CompleteNewInterior` no se prueba (desbordaría los arreglos globales en el EXE y en la DLL).
- `CPathNode::GetPosition` (0x420A10) y `FindRegionForCoors` (0x44D830) se ejecutan con su código real en ambos lados (tienen hooks propios, validados por otros bancos).
- `ThePaths` no se toca: las funciones se prueban sobre una arena propia, por eso 0x450FB0 y `AddDynamicLink` (que usan el global) van sustituidos.
