<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco PlantAtomicPrelit28: CPlantMgr::AtomicCreatePrelitIfNeeded (0x5DD100)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175434-037677`: 4096 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-plantprelit28\20261018-175434-037677`; archivado en `reversing/CppTwelveLevel6Final/runs/PlantAtomicPrelit28-20261018-175434-037677/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CPlantMgr,AtomicCreatePrelitIfNeeded,0x005DD100,1,0,1,S` en `docs/hooks.csv`).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh PlantAtomicPrelit28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/PlantAtomicPrelit28/build_probe.py
python -B reversing/PlantAtomicPrelit28/run_local.py
python -B reversing/PlantAtomicPrelit28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis (None si el registro no existe o la corrida no terminó)
python -I reversing/PlantAtomicPrelit28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -B reversing/PlantAtomicPrelit28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Requiere el ASI del árbol actual con `RH_ScopedGlobalInstall(AtomicCreatePrelitIfNeeded, 0x5DD100)` en `CPlantMgr::InjectHooks` (`PlantMgr.cpp`, cambio sin commit; categoría `CPlantMgr`, hook `Simple`). Archivo de arranque en el clon: `plantprelit28-runtime.start`; salida `plantprelit28-runtime.jsonl`. 4096 pares. `docs/hooks.csv` no tiene la fila (se añade al registrar; no se tocó).

Autopruebas sin juego (ya ejecutadas, en consola x86 de VS con `vcvars32`):
```
cl /nologo /EHsc /std:c++20 /O2 /Fe:dump_fixtures.exe dump_fixtures.cpp   &&  dump_fixtures.exe > fx.jsonl   &&  python -I fixture_oracle.py --check-dump fx.jsonl   # generador C++ real vs Python + cobertura
cl /nologo /EHsc /std:c++20 /O2 /Fe:host_selftest.exe host_selftest.cpp    &&  host_selftest.exe > host.jsonl  &&  python -I fixture_oracle.py --check-host host.jsonl  # transcripción del desensamblado + falsos vs oráculo
```
Resultados: `--check-dump` sin discrepancias (hashes de relleno y de datos de los 4096 casos coinciden entre C++ y Python; conteos de cobertura en `run_local.py: MINIMUMS`); `--check-host`: «ok host 4096». Una mutación deliberada (copiar 4 bytes de más en un caso) hace fallar `--check-host`.

## Qué hace la función (desensamblado 0x5DD100..0x5DD1DC)

`void AtomicCreatePrelitIfNeeded(RpAtomic* atomic)`, cdecl, `ret` normal; marco de 0xC bytes; guarda/restaura EBX/EBP/ESI/EDI. Devuelve el átomo en EAX en las dos salidas (el C++ es `void`).
1. `old = atomic->geometry` (+0x18). Si `old->flags & 8` (rpGEOMETRYPRELIT) termina sin hacer nada.
2. `numTri = old+0x10`, `numVert = old+0x14`, `oldMorph = old+0x5C`. `new = RpGeometryCreate(numVert, numTri, 0xD)` (0x74CA90).
3. Copias con `rep movs` (tamaños exactos): vértices `numVert*12` (`new.morph+0x14 <- old.morph+0x14`), coordenadas `numVert*8` (`+0x34`), triángulos `numTri*8` (`+0x2C`).
4. Si `numTri > 0` (con signo), para cada triángulo i: `m = RpGeometryTriangleGetMaterial(old, oldTri+8i)` (0x74C760) y `RpGeometryTriangleSetMaterial(new, newTri+8i, m)` (0x74C6C0), en ese orden.
5. `RpGeometryUnlock(new)` (0x74C800); `new->flags |= 2` (se relee DESPUÉS de Unlock); `RpAtomicSetGeometry(atomic, new, 1)` (0x749D40).

Llamadores: un solo `call` (0x5DD2B5, dentro de `CPlantMgr::LoadModels` 0x5DD220, `__usercall`, no enganchada; en C++ la llama `LoadModels` directamente). No hay otras referencias a 0x5DD100 en el EXE (ni `jmp` ni la dirección como dato). Tras la llamada hace `mov esi,[edi+18h]` (EDI preservado, ESI se recarga): **ningún llamador lee EAX, ECX ni EDX**; por eso el banco sólo los registra (`original_residue`/`candidate_residue`) y la función no los reproduce.

## Revisión propia contra el EXE (fase 2)

Se volvió a desensamblar 0x5DD100..0x5DD1DC (`dis.sh`) y se contrastó el borrador sin commit línea a línea:
- test de prelit, desplazamientos (+0x18, +0x8, +0x10, +0x14, +0x5C, +0x34, +0x2C, +0x14 del morph), orden de argumentos de Create/GetMaterial/SetMaterial/SetGeometry (cdecl; cada llamada limpia con `add esp`), tamaños de las tres copias, condición `numTri > 0` con signo, relectura de los flags tras Unlock y argumento final 1: **coinciden con el borrador**. Los desplazamientos del C++ (`RpGeometry`: triangles 0x2C, texCoords[0] 0x34, morphTarget 0x5C; `RpMorphTarget`: verts 0x14; `RpAtomic::geometry` 0x18) se comprobaron contra `rpworld.h`.
- **Discrepancias halladas: ninguna en la lógica.** Único cambio de fuente respecto al borrador: faltaba el hook (`RH_ScopedGlobalInstall(AtomicCreatePrelitIfNeeded, 0x5DD100)` en `CPlantMgr::InjectHooks`, tras `Render`), sin el cual la envoltura reconstruida no se ejercitaba desde el EXE. No hay cambios en `PlantMgr.h`.
- Diferencias de efecto no observables y asumidas: el EXE devuelve el átomo en EAX (la DLL no; nadie lo lee); con `numTri < 0` o `numVert < 0` el EXE y el `memcpy` del C++ ejecutan una copia gigante (comportamiento indefinido en ambos; el banco no lo prueba).
- Compilación: no realizada (consigna). El hook usa una función `static`, como `LoadModels`/`GeometrySetPrelitConstantColor` del mismo archivo.

Parche aislado: `source.patch` (sólo `PlantMgr.cpp`; `git apply --check` verificado contra `git show HEAD:source/game_sa/PlantMgr.cpp`; resultado idéntico al árbol de trabajo salvo fin de línea).

## Diseño del banco

- 4096 casos = 8 grupos x 512 (ver `Fixtures.hpp`): 0 prelit (no hace nada), 1 `0/0`, 2 `V>0,T=0`, 3 `V=0,T>0`, 4 contadores 1..8, 5 contadores límite {0,1,3,31,32,33,99,100}², 6 aleatorios 0..100, 7 `V>0,T=1`. 16 familias → 8 modos de relleno de datos (aleatorio, ceros, 0xFF, matIndex 0xFFFF, NaN/inf, denormales, patrón incremental). Flags viejos (incluidos 0, 0xFFFFFFF7, 0x80000000, con y sin bit 8 en el grupo 0) y flags de la geometría nueva (bit 2 puesto/no puesto) arbitrarios.
- Ambos lados ejecutan la MISMA entrada 0x5DD100 (original = hook RH apagado; candidato = sólo ese hook encendido). Se sustituyen, sólo durante la llamada y con los mismos falsos (parches en la dirección del EXE y en el cuerpo de la DLL, resuelto antes de parchear), `RpGeometryCreate`, `RpGeometryTriangleGetMaterial`, `RpGeometryTriangleSetMaterial`, `RpGeometryUnlock` y `RpAtomicSetGeometry`. No se llama a RenderWare real. Los falsos devuelven una geometría nueva de una arena propia (con guardas aleatorias), un «material» determinista (depende del índice y del matIndex), escriben `low16(material)` en el matIndex del triángulo nuevo (hace observable el orden copia/asignación) y trazan `[tipo,a,b,c,d]` (Unlock incluye el hash de la arena nueva y los flags en ese instante: el `|= 2` debe llegar después; SetGeometry incluye los flags ya con el bit 2).
- Observado por lado: traza completa, hash FNV-1a 64 de TODA la arena nueva enmascarando los 5 dwords de puntero (vértices, coordenadas, triángulos, matIndex, flags y todas las guardas/bytes no tocados), punteros de la arena nueva intactos, arena vieja + átomo byte a byte sin cambios, descartes de traza (`ev_lost`). Compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF), x87 normalizado (FCW 24 bits, pila vacía), restauración del hook, de las 6 entradas vigiladas y del estado x87. EAX/ECX/EDX sólo registrados (`kCompareVolatileRegisters=false`).
- `fixture_oracle.py` reconstruye desde el desensamblado la traza y el hash esperados a partir de la fila `input` (regenera los datos con su propio código; la identidad con el generador C++ está comprobada por `--check-dump`). `run_local.py` exige igualdad EXE=DLL=oráculo y cobertura mínima calibrada sobre el generador C++ real.

## Suposiciones y riesgos

- **No compilado.** `Core.hpp` se montó a partir de la plantilla `FileLoaderLoad2dEffect27` (mismo `Gate`, `Execute`, `Patches`, compuertas); lo nuevo está en `Fakes.hpp` (estándar, compilado y ejecutado en consola) y en `LiveComparison.hpp`/`Core.hpp` (`CaseRun`, `Run`, `Env`), que sólo se han revisado. Posibles puntos de fricción al compilar: `&RpGeometryCreate` etc. (funciones libres sin sobrecargas en `rpworld.h`), `Execute(..., {}, ...)` con lista vacía.
- `RpGeometryTriangleGetMaterial` está enganchada globalmente en `rpworld.cpp` (0x74C760): se parchean tanto la dirección del EXE como el cuerpo de la DLL, y el candidato usa el cuerpo de la DLL pero reemplazado por el mismo falso, así que el cuerpo real no se valida. Tampoco se valida el comportamiento real de RenderWare (matList, instanciado, refcount…): el banco prueba la lógica de copia, el orden de llamadas y los argumentos.
- El falso de `SetMaterial` escribe sólo en el matIndex del triángulo nuevo; el real escribiría el índice de material en la lista. Es suficiente para detectar orden y argumentos, no semántica real.
- Los punteros no tienen alineación especial; las arenas son `std::array` estáticos de la sonda (direcciones fijas dentro de una corrida).
- La función no usa x87; las compuertas x87 se mantienen por uniformidad del banco.
- No se cubren contadores negativos ni mayores que 100 (arena fija; el EXE también copiaría fuera de ella).
