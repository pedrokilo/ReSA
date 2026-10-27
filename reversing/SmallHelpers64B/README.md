<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SmallHelpers64B: GetPopcycleGroupPedModel, FlushWaterRenderBuffer y CTaskComplex::Destructor

Estado: **APROBADO dentro del juego** (21/10/2026): 12288 pares, cero diferencias, todas las compuertas en ambos lados, filas input idénticas al espejo. Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-smallhelpers64b/20261021-021432-159943`. Worktree `D:\Original\wp-2` (HEAD 1b13a6db4 + fuentes del lote 64B). 3 x 4096 pares.

| Función | Dirección | Clase / nombre del hook | Tipo | Colaboradores sustituidos |
|---|---|---|---|---|
| `GetPopcycleGroupPedModel` (selector de modelo de ped por grupo de población) | 0x406E80 | `CGangWars` / `GetPopcycleGroupPedModel` | S (`RH_ScopedGlobalInstall`) | ninguno (tablas sintéticas en ventanas) |
| `FlushWaterRenderBuffer` (vaciado del búfer temporal de agua) | 0x6E7680 | `CWaterLevel` / `FlushWaterRenderBuffer` | S | RwIm3DTransform 0x7EF450, RwIm3DRenderIndexedPrimitive 0x7EF550, RwIm3DEnd 0x7EF520 (sin hook, por dirección) |
| cuerpo de `~CTaskComplex` | 0x61A3D0 | `CTaskComplex` / `Destructor` | S | ninguno (subtarea falsa con destructor virtual falso) |

Filas de `docs/hooks.csv` (las añade el coordinador): `CGangWars,GetPopcycleGroupPedModel,0x00406E80,1,0,1,S`, `CWaterLevel,FlushWaterRenderBuffer,0x006E7680,1,0,1,S`, `CTaskComplex,Destructor,0x0061A3D0,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh SmallHelpers64B
python -I reversing/SmallHelpers64B/fixture_oracle.py          # «ok»
python -I reversing/SmallHelpers64B/fixture_mirror.py          # constantes C++ == oráculo y cobertura >= mínimos
python -B reversing/SmallHelpers64B/register_hook.py "<carpeta de evidencia>"   # sólo el coordinador
```

## Fuente
- `GangWars.cpp`: `static int32 GetPopcycleGroupPedModel(group, slot, zone)`: `g = u32[0x8D2540 + (3*group+zone)*4]`, `modelo = u16[0xC0F358 + (g*21+slot)*2]` (aritmética de 32 bits con envoltura, sin límites, EAX cero-extendido). `CreateAttackWave` la usa en lugar de `plugin::CallAndReturn<0x406E80>`; único llamador del EXE (lee EAX).
- `WaterLevel.cpp`: `static void FlushWaterRenderBuffer()`: si `[0xC4B950] != 0`: `LittleTest()`, `RwIm3DTransform(0xC4D958, nVtx, 0, 1)`; si el resultado completo (32 bits) es no nulo: `RwIm3DRenderIndexedPrimitive(3, 0xC4B958, nIdx)` y `RwIm3DEnd()`; siempre pone ambos contadores a 0. Sustituye las tres `plugin::Call<0x6E7680>()`. Es `RenderStuffInBuffer` (0x707800) más `LittleTest`. (El comentario de `RenderBuffer.cpp` que le atribuye 0x6E7680 a `RenderIfDoesntFit` es erróneo; no se tocó.) Llamadores del EXE: sólo las `_OneLayer` sin reconstruir (0x6E91E4, 0x6E8790, 0x6E8EF1, 0x6E9963 ya cubiertos por la DLL); nadie lee EAX/ECX/EDX.
- `Tasks/Task.cpp`: `CTaskComplexBody::Destructor` (`__fastcall (CTask*, void*)` == thiscall sin pila): vtable = 0x86D4CC; si `[this+8]` no es nulo, `delete` (destructor virtual con borrado); `[this+8] = 0`; vtable = 0x86D48C. El marco SEH del EXE (sólo desenrolla) se omite. Llamadores (≈100, `call` o `jmp`) no leen EAX/ECX/EDX (residue_scan: los dos con «unwritten=edx» sólo ven calls posteriores que no lo leen).

## Qué se compara
Original = EXE con el hook apagado; candidato = DLL con sólo ese hook. Compuertas de ABI (ESP, EBX/ESI/EDI/EBP, DF, SEH, x87 FCW/TOP/etiquetas), ruta (la entrada salta a la DLL sólo en el candidato), tablas/objetos sin cambios, estado global restaurado tras quitar los falsos, eventos de los falsos en orden. EAX/ECX/EDX sólo registrados, salvo el EAX devuelto por `GetPopcycleGroupPedModel`.
Arenas (sumas a mano; Fixtures.hpp == fixture_oracle.py, verificado con fixture_mirror.py): traducción 64+396+64 = 524 (33 grupos x 12); grupos de peatones 128+2394+128 = 2650 (57 x 42); objeto de tarea 0x20+0x10+0x20 = 0x50.
Riesgos: los índices fuera de las ventanas no se prueban (el EXE leería memoria arbitraria); la GPU no se ejercita (RenderWare sustituido); no se construyen tareas reales.

## Resultados
APROBADO al primer pase completo (tras corregir sólo defectos del banco: constante de grupos 33 y `Side` duplicado). Revisión contra el EXE: re-desensamblado y contrastado; sin discrepancias de fuente.
