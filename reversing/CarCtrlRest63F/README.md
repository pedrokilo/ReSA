<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarCtrlRest63F (tanda 6): IsThisAnAppropriateNode

Estado: **APROBADO dentro del juego** (20/10/2026, worktree `D:\Original\wh-4`, HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 a 6). 4096 pares, cero diferencias, todas las compuertas de ABI/ruta/restauración
en ambos lados, AL de salida igual y cobertura de ramas medida sobre los eventos de los colaboradores. Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-carctrlrest63f\20261020-214217-759241`.
(Primera corrida aprobada, `20261020-213941-078334`, con la cola de `IsLawEnforcementVehicle` / `IsAnyoneParking` / `FindObjectsKindaColliding` cubierta sólo en ~3 % de los casos; se añadieron familias «limpias» en el generador y se repitió el banco.)

| Función | Dirección | Hook | Pares | Colaboradores sustituidos (falsos idénticos en ambos lados) |
|---|---|---|---|---|
| `CCarCtrl::IsThisAnAppropriateNode` | 0x42DAB0 | Simple (`RH_ScopedInstall`) | 4096 | `CVehicle::IsLawEnforcementVehicle` 0x6D2370, `CCarCtrl::IsAnyoneParking` 0x42C250 y `CWorld::FindObjectsKindaColliding` 0x568B80 (los tres con hook; el último con los 11 argumentos de pila registrados y el contador de salida escrito) |

Los ayudantes de vector del EXE (`operator-` 0x40FE60, `CrossProduct` 0x59C730, `DotProduct` 0x40FDB0, `CPathNode::GetPosition` 0x420A10) y `CPlaceable::GetRight` (0x41CC70, llamada por dirección) NO se sustituyen: el EXE ejecuta los suyos y la DLL ayudantes x87 equivalentes
en `X87Math.cpp`, de modo que el banco los compara de paso con las entradas del banco.

```
bash reversing/_handoff/bench_here.sh CarCtrlRest63F           # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CarCtrlRest63F/build_probe.py
python -B reversing/CarCtrlRest63F/run_local.py
python -B reversing/CarCtrlRest63F/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aún no existe o no está completo
python -B reversing/CarCtrlRest63F/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63f/20261020-214217-759241"   # sólo lo ejecuta el coordinador
```
Fila de `docs/hooks.csv` (la añade el coordinador; tipo S): `CCarCtrl,IsThisAnAppropriateNode,0x0042DAB0,...,S`. `SCOPE_NOTE` de `register_hook.py` ya está rellenado. Archivo de arranque: `carctrlrest63f-runtime.start`.

## Fuente (`source.patch`: sólo los cambios de la tanda 6 respecto a la tanda 5)

`CarCtrl.cpp`: `IsThisAnAppropriateNode` en C++, su línea de hook y las declaraciones locales de `Cc63fCrossZNegativeCpp` y `Cc63fRightDotNegativeCpp`, definidas al final de `X87Math.cpp` (prefijo `Cc63f`; restas de `CVector` y producto vectorial/escalar a 24 bits en el orden de 0x40FE60, 0x59C730 y 0x40FDB0).

## Revisión contra el EXE (re-desensamblado; tablas leídas de los bytes)

Nodo 3 sin cargar o igual al nodo 1 (misma área y nodo) -> false (el nodo 2 se lee sin comprobar su área). Bytes `+0x18` de los nodos 2 y 3 (bit 7 = agua): si difieren y el modelo (`movsx word [+0x22]`, comparación de 16 bits) no es 539 (0x21B) -> false.
Tipo = `(byte[nodo3 + 0x1A] >> 4) - 1` sin signo; tabla de bytes 0x42DE68 y de saltos 0x42DE54: tipo 1 -> «A», 2 -> «B», 5 y 10 -> «C», 8 y 9 -> «D», el resto (0, 3, 4, 6, 7, 11..15) -> por defecto.
**A:** `+0x4A4 == 2` -> false; conductor (`+0x460`) con `byte[+0x484] == 2` -> false; `byte[infoModelo + 0x4D] == 5` -> false; si `!arg5`: `((nodo3 - nodo2) x (nodo3 - posición)).z < 0` -> false (restas de 0x40FE60 guardadas como float, `CrossProduct(v2, v1)` con
`z = a.x·b.y - b.x·a.y`); después `IsLawEnforcementVehicle()` -> false, otra vez `byte[+0x4D] == 5` -> false, conductor con `dword[+0x598] == 0x14` -> false, `IsAnyoneParking()` -> false y
`FindObjectsKindaColliding(pos nodo3, 5.0f, true, &n, 2, null, false, true, false, false, false)`: devuelve `n == 0`. **B:** igual salvo que no comprueba el modelo al principio y usa `DotProduct(GetRight(), nodo3 - posición) < 0` (suma `z, y, x`) -> false.
**C (5, 10):** `arg5` -> false; `col[+0x14] < 2.0f` (col = `*(infoModelo + 0x14)`) -> false; sigue por el defecto. **D (8, 9):** `arg5` -> false; `col[+0x14] > 1.5f`, `col[+0x0C] > 2.0f` o `col[+0x10] > 4.0f` -> false; sigue por el defecto.
**Por defecto:** con `+0x4A4 == 2` y misión (`+0x3BA`) 1: `(b19(nodo3) & 4) && !(b19(nodo2) & 4)` -> false; `(b18(nodo3) & 0x10) && !(b18(nodo2) & 0x10)` y lo mismo con 0x20 -> false; devuelve `!arg5`.

Llamadores y registros: el único llamador (0x42E18B, `PickNextNodeRandomly`) sólo lee AL; el aviso de `residue_scan.py` sobre EDX es un límite del escáner (EDX se vuelve a escribir en el bucle antes de usarse).

## Defectos hallados

- Producción: ninguna discrepancia hallada (coincidió a la primera con el EXE).
- Banco: cobertura de la cola de la función insuficiente en la primera corrida (ver arriba), reforzada con familias «limpias».

## Suposiciones y riesgos

- Sólo se prueba con vehículo con matriz válida (la posición y la derecha salen de la matriz; el EXE llama a `GetRight` por dirección y el banco no sustituye esa función); el camino sin matriz de `GetRight` (fsin/fcos del rumbo) no se cubre.
- El nodo 2 siempre tiene el área cargada en los casos (el EXE la desreferencia sin comprobar). `info del modelo` y el `col model` son objetos falsos con los campos leídos (+0x4D, +0x14; +0x0C, +0x10, +0x14).
- `PickNextNodeRandomly` (el llamador) sigue siendo una envoltura hacia el EXE.
