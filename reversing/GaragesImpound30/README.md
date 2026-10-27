<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco GaragesImpound30: CGarages::StoreCarInNearestImpoundingGarage (0x0044A3C0)

**Estado: PREPARADO (lote 30; no se ha compilado el proyecto ni ejecutado GTA).** Lo único que sí se ha ejecutado: la prueba fuera del juego `offline_check.py` (EXE canónico vs C++ de la DLL vs oráculo, 4096 casos, 0 diferencias; ver abajo), la autoprueba del oráculo y el análisis de `run_local.py` sobre un registro sintético generado con el oráculo. Plantilla: `reversing/PickupsGenNew28` (colaboradores sustituidos por falsos con parche de entrada en el EXE y en el cuerpo de la DLL).

Worktree de trabajo: `D:\Original\wc-3` (HEAD `c5318699a`). Fuente: `source.patch` (sólo esta función; `git apply --check` contra `git show HEAD:<archivo>` verificado para `Garages.cpp`, `X87Math.cpp`, `X87Math.h`). `reversing/GaragesAddOne30/source-both.patch` es el `git diff HEAD` completo del worktree (esta función + `CGarages::AddOne`); los dos parches individuales añaden al final de `X87Math.cpp/.h` y no se aplican uno tras otro sin fusión.

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh GaragesImpound30          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/GaragesImpound30/build_probe.py
python -B reversing/GaragesImpound30/run_local.py
python -B reversing/GaragesImpound30/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o no ha terminado)
python -I reversing/GaragesImpound30/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»; ya ejecutada: ok)
python -B reversing/GaragesImpound30/offline_check.py                                               # EXE vs DLL vs oráculo fuera del juego (ya ejecutada: cases 4096 E!=D 0 E!=oracle 0 D!=oracle 0)
python -B reversing/GaragesImpound30/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI con `RH_ScopedInstall(StoreCarInNearestImpoundingGarage, 0x44A3C0)` (clase `CGarages`, hook `Simple`; `HookRoute` lo busca por esos nombres). Aplicar antes `source.patch` si el árbol no lo contiene. Arranque en el clon: `garagesimpound30-runtime.start`; salida `garagesimpound30-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene aún la fila `CGarages,StoreCarInNearestImpoundingGarage,0x0044A3C0,1,0,1,S` (la añade el coordinador al registrar); la envoltura desaparece de las listas de envolturas pendientes.

## Fuente (FASE 1)

`source.patch` toca:

- `source/game_sa/Garages.cpp`: se activa `RH_ScopedInstall(StoreCarInNearestImpoundingGarage, 0x44A3C0)` y la función deja de llamar a `plugin::Call<0x44A3C0, CVehicle*>`. Recorre `aGarages[0..NumGarages)` (sin signo), filtra los tipos 0x21..0x23, pide la distancia a `GarageImpoundCloserCpp` (x87) y, con el mejor, usa `FindSafeHouseIndexForGarageType` (C++ en la DLL, hook activo) y `CStoredCar::StoreCar` (envoltura al EXE 0x449760, todavía sin reconstruir). `Garages.h` y `Garage.cpp/.h` no cambian.
- `source/game_sa/X87Math.h` / `X87Math.cpp` (al final, prefijo `Garage`): `GarageImpoundCloserCpp(garage, vehicle, &minDistance)`: `dx = vx − gx`, `dy = vy − gy` (en registro), `d = sqrt(dy*dy + dx*dx)` sin redondear (función aparte `GarageRootCpp`, `static noinline`: con `/fp:fast` el compilador cambiaría `sqrt(s) < m` por `s < m*m`), `if (d < *minDistance) { *minDistance = float(d); return true; }` (comparación ordenada: con NaN no). Objeto comprobado con `cl /FAs`: `fld, fsubr, fstp qword` ×2 (los `dx`, `dy` pasan por `double` de 8 bytes: exactos, 24 bits < 53) → `fld, fmul, fld, fmul, faddp, fsqrt` → `fld [min]; fcomp; test ah,41h; jne` → `fstp [min]`.

## Qué hace el EXE (desensamblado 0x44A3C0..0x44A4C6, cdecl, `ret` normal)

`[esp+4]` = vehículo; `edi = NumGarages` (si es 0, `jbe` ⇒ sale), `esi = −1`, mínima = float 0x47C34FF3 (99999,898…).

1. Para cada garaje `i` (paso 0xD8 desde 0x96C048): `al = byte [+4Ch]` (`m_nType`); salta si `al < 0x21` o `al > 0x23`. `gx = [g]`, `gy = [g+4]` (por `fld/fstp`), posición del vehículo = `m_matrix (+14h) ? matriz + 0x30 : vehículo + 4`; `dx = pos.x − gx` (`fsubr`), `dy = pos.y − gy`, `d = fsqrt(dy*dy + dx*dx)` en registro; `fcom [min]; test ah,5; jp` ⇒ sólo si `d < min` ordenado (empate o NaN: no): `min = float(d)`, `esi = i`.
2. Sin garaje (`esi < 0`): sale. Con él: `eax = FindSafeHouseIndexForGarageType(movzx byte [esi*0xD8 + 0x96C094])` (0x4489F0, **una sola vez**; ese ECX = 0 se conserva), ranuras = `0x96ABD8 + eax*0x100`.
3. `count = (modelo0 > 0) + (modelo1 > 0) + (modelo2 > 0)` (uint16 en +0x12 de cada ranura de 0x40 bytes; la cuarta **no cuenta**); con `count == 3`: `ranura0 ← ranura1`, `ranura1 ← ranura2` (`rep movsd` ×0x10) y `count = 2`. Finalmente `CStoredCar::StoreCar(&ranura[count], vehículo)` (0x449760, thiscall, `ret 4`). Con ocupación no consecutiva (p. ej. 0, 1, 1) se sobrescribe la ranura 2.

## Revisión propia contra el EXE (FASE 2)

Segundo desensamblado de 0x44A3C0..0x44A4C6 y del colaborador 0x4489F0 contrastado instrucción a instrucción con el C++ y, además, **ejecución real de los bytes del EXE** frente al C++ de la DLL y al oráculo (4096 casos, `offline_check.py`; los dos `call` del EXE apuntan a los mismos falsos que usa el banco). Discrepancias halladas respecto del borrador del árbol principal (`git diff HEAD -- source/game_sa/Garages.cpp` de D:\Original\gta-reversed-master) y corregidas:

- El borrador calculaba `dx`, `dy`, los cuadrados y la raíz en `float` SSE y comparaba `dist < minDist`: difiere de la x87 de 24 bits con exponente extendido cuando `dx² + dy²` desborda por abajo (|dx| < ~1e-19: SSE da distancia 0 y el siguiente garaje no puede ganar) o por arriba, y en los NaN. Se sustituyó por `GarageImpoundCloserCpp`.
- El borrador usaba `vehicle->GetPosition()` por separado para x e y y `aGarages[best].m_nType` leído antes del `if`; el orden real (la posición del vehículo se lee en cada garaje de embargo, el tipo del mejor después del bucle) no es observable, pero se dejó igual que el EXE.
- Coinciden con el EXE y no se tocaron: filtro 0x21..0x23, comparación estricta (el primero gana empates), distancia inicial 0x47C34FF3, una única llamada a `FindSafeHouseIndexForGarageType`, conteo de sólo tres ranuras (`uint16 > 0`), desplazamiento 1→0 y 2→1 con tres ocupadas y escritura en la ranura `count`.
- **Dependencia de ECX del EXE:** el EXE llama a 0x4489F0 con `xor ecx,ecx` y luego compara `cmp word [eax+12h], cx` suponiendo que la función conserva ECX (la original sólo toca EAX). La C++ ya no depende de ello. Nota para el coordinador: con el hook de `StoreCarInNearestImpoundingGarage` APAGADO y el de `FindSafeHouseIndexForGarageType` encendido (estado normal de la DLL), el EXE 0x44A3C0 llama al cuerpo C++ de `FindSafeHouseIndexForGarageType`, cuyo ECX de salida no está garantizado; por eso el falso del banco (nativo en ensamblador de la sonda, `__declspec(naked)`) conserva ECX y EDX en ambos lados.

**Llamadores del EXE** (`residue_scan.py 0x44A3C0`): 0x5667A1 y 0x56A385 (`push esi; call 44A3C0; add esp,4; push esi; call 00423ED0`); 0x423ED0 (`RemoveFromInterestingVehicleList`) carga `edx` y `ecx` antes de leerlos ⇒ **ningún llamador lee EAX/ECX/EDX**; sólo se registran (`kCompareVolatileRegisters=false`). Ambos llamadores están en el EXE (no hay llamadores C++ más allá de `World.cpp:3366` en la DLL).

## Qué se compara (por lado y en bits; original == candidato == oráculo)

- **Trozos de 0x40 bytes de la tabla de coches guardados** (20 refugios × 4 ranuras, 0x1400 bytes en 0x96ABD8) que cambian respecto de la referencia, con su contenido; **traza de los falsos** `(1, tipo)` y `(2, ranura 0..79, vehículo)` con orden; **array de garajes y `NumGarages` intactos**; **bloques de vehículo y matriz intactos**.
- Falsos (mismos en ambos lados, parche de entrada en la dirección del EXE y en el cuerpo de la DLL, calculados antes de parchear): `FindSafeHouseIndexForGarageType` (0x4489F0; `naked`, conserva ECX/EDX; devuelve `find[tipo − 0x21]` ∈ 0..19 y anota el byte bajo del tipo) y `StoreCar` (0x449760; `__fastcall(self, edx, vehículo)`, `ret 4`; anota ranura y vehículo y escribe `0xC0DE0000 | ranura` en +0 y `0x7777` en +0x12). Hooks en el estado por defecto (el de `FindSafeHouseIndexForGarageType` encendido; `StoreCar` sin hook).
- Oráculo (`fixture_oracle.py` + `x87model.py`, desde el desensamblado): x87 de 24 bits con exponente extendido; `dist < min` ordenado (NaN, inf y empate: no). Autoprueba (ya ejecutada: ok): sin garajes / sin garajes de embargo, tipo 0x22 con ranuras vacías, el más cercano y empates, las 5 ocupaciones representativas, tres ocupadas con desplazamiento, distancia inicial 99999,9, NaN, infinito, exponente extendido (2e-30 vs 1e-30, cuyos cuadrados valen 0 en float), 3e19, y 2000 casos aleatorios contra una cadena float32.
- ABI: ESP (`ret` normal: la compuerta retira 4), EBX/ESI/EDI/EBP, DF=0, x87 normalizado (FCW 24 bits, TOP, etiquetas, ST válidos; ninguno al salir), hook/ruta, entradas de código vigiladas (0x44A3C0, 0x4489F0, 0x449760), tablas de garajes / coches guardados y `NumGarages` del juego restaurados al final.
- **EAX/ECX/EDX de salida: sólo se registran** (ningún llamador los lee).

## Fixtures (`Fixtures.hpp`): 4096 casos = 16 familias x 256 (familia = n % 16)

0 típicos (1..8 garajes, la mitad de embargo); 1 ninguno de embargo; 2 `NumGarages = 0`; 3 muchos garajes (20..50) casi todos de embargo; 4 distancias alrededor de 99999,9; 5 NaN en garajes o vehículo; 6 infinitos y enormes (≥ 1e19, FLT_MAX); 7 diminutos y subnormales; 8 ocupación de las tres primeras ranuras (los 16 patrones, la cuarta no cuenta); 9 tres ocupadas (desplazamiento); 10 con y sin matriz (posición del vehículo vs de la matriz, la no usada distinta); 11 los tres tipos de embargo y refugios 0..19; 12 tipos frontera (0x1F, 0x20, 0x24, 0x25, 0xA1...) a la misma distancia; 13 empates y ceros con signo; 14 bits crudos; 15 el ganador es el último / la ranura 49. Las ranuras por encima de `NumGarages` llevan a veces tipo 0x22 en el origen (si se iteraran ganarían). Modelos: 0, 1, 2, 0x7FFF, 0x8000 (> 0 sin signo), 0xFFFF, 0xFF, 0x100, 400, 411. Semillas de pila por caso, EAX/ECX/EDX por caso, relleno de bytes aleatorios en vehículo, matriz, refugios y garajes.

Cobertura calibrada con `fixture_dump.cpp` (compilado con `cl`) + `fixture_oracle.py` (recuentos medidos; mínimos en `run_local.py` con ~45 % de margen, más 256 exactos por familia): best_found 2844, best_first 915, best_last 1008, count_0 328, count_1 956, count_2 1560 (574 tras desplazar), has_candidate 3527, house_0 433, house_19 428, huge_in 185, inf_in 127, matrix 2015, placement 2081, n_garages_0 256, n_garages_50 73, n_garages_ge_40 143, nan_in 259, near_initial_min 256, negzero_in 238, no_best 683, no_candidate 569, shifted 574, tie_kept_first 344, tiny_in 438, trace_2 2844, type_21 987, type_22 904, type_23 953, type_boundary 711, type_high_bit 373.

## Prueba fuera del juego (`offline_check.py`)

Ejecuta los **bytes del EXE** (0x44A3C0 reubicado a 0x901000 con sus dos `call` reapuntados a los falsos; las tablas en sus direcciones fijas reales) y la función **C++ de la DLL** (cuerpo extraído de `Garages.cpp`, `X87Math.cpp` con las opciones del proyecto, el resto con `/Od`) sobre las mismas 4096 fixtures y compara EXE == DLL == oráculo: `cases 4096 E!=D 0 E!=oracle 0 D!=oracle 0`. No sustituye al banco (no prueba el enganche del hook, la ABI de la sonda ni el estado real del juego). Arena y constantes: `crosscheck` compara el volcado de `Fixtures.hpp` compilado con `cl` (0xD8, 50, 0x2A30, 20 × 4 × 0x40 = 0x1400, 80 trozos, direcciones, bloques de 0x40) con las constantes del oráculo, y `run_local.py` exige que el `begin` de la corrida (`constants`) repita las mismas.

## Suposiciones y riesgos del banco

- Ni `Core.hpp` ni `LiveComparison.hpp` se han compilado dentro del proyecto (sólo `Core.hpp` con un `stub.hpp` mínimo de `json`/`HookRoute`/`EntryPatch`, sin errores). Se asume que `Body(&CStoredCar::StoreCar)` y `Body(&CGarages::FindSafeHouseIndexForGarageType)` resuelven los cuerpos de la DLL (la comprobación `ready` los exige dentro del módulo del ASI); si el compilador pliega alguno en un enlace incremental, `Body()` salta los `E9` internos. Si alguno de estos colaboradores se reconstruye después en C++ con hook, hay que revisar el estado de su hook (ver `CTaskSimpleJetPack`, `PickWeap1`).
- El banco sustituye `aGarages`, `NumGarages` y `aCarsInSafeHouse` por los del caso (se guardan y restauran) dentro de `CGame::Process`, sin código del juego concurrente; refugios 0..19 (los falsos nunca devuelven otro índice: el EXE no acota el índice).
- El vehículo es un bloque de 0x40 bytes y la matriz otro; la DLL sólo lee `m_matrix` (+0x14) y la posición (+4 o matriz + 0x30); ningún método virtual interviene.
- El hook de `StoreCarInNearestImpoundingGarage` queda activo en producción; `FindSafeHouseIndexForGarageType` conserva el suyo.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `x87model.py` (copia idéntica en GaragesAddOne30), `fixture_dump.cpp`, `offline_check.py`, `offline_harness.cpp`, `register_hook.py`, `source.patch`, `README.md`.
