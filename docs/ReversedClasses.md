# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-11-09 08:52 UTC

**Índice:** [1. Objetivo y dimensiones](#objetivo) · [2. Siguientes mediciones](#mediciones) · [3. Siguiente fase](#siguiente-fase) · [4. Vista general](#vista-general) · [5. Cobertura real del EXE](#cobertura-exe) · [6. Lo que falta del código del juego](#que-falta) · [7. Funciones del juego aún no en C++](#no-en-cpp) · [8. Completas](#completas) · [9. Cohorte 17/10/2026](#cohorte-nuevo) · [10. Dependencias directas (grupo A)](#grupo-a) · [11. Parciales](#parciales) · [12. Sin revertir](#sin-revertir) · [13. Cómo leer y actualizar](#metodo)

<a id="objetivo"></a>

## 1. Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. **La meta de este informe es completar todas las funciones del juego** (11,804 funciones y 3.01 MB de código en el EXE: el código propio del juego y su capa de plataforma). RenderWare, Direct3D y el CRT son bibliotecas enlazadas dentro del EXE: no forman parte de la meta, no se cuentan en las tablas y solo aparecen en la [cobertura real del EXE](#cobertura-exe) y en la lista de [bibliotecas fuera del objetivo](#bibliotecas) para aclarar qué queda fuera. Completar el juego es un hito hacia **ReSA — Zero Original Function Dependency**.

Cifras medidas el 09/11/2026. **Cada porcentaje de este informe lleva escrita la dimensión de la que habla**: *funciones* (del EXE), *bytes* (de código del EXE), *clases*, *entradas del catálogo* (filas de `hooks.csv`) o *validación* (cuántas de las ya hechas tienen evidencia aprobada). Dimensiones distintas dan porcentajes distintos y no son comparables entre sí.

| Tipo | Dimensión | Qué cuenta | Numerador | Denominador | Porcentaje (de qué) | Progreso |
| --- | --- | --- | ---: | ---: | --- | --- |
| **Funciones** | Funciones del juego reemplazadas por C++ en el EXE (**meta del proyecto**) | Funciones del EXE en 0x401000–0x74FFFF que ya tienen su hook reconstruido (análisis estático del EXE) | 9,423 | 11,804 | 79.83% de las funciones del juego | `████████████████░░░░` |
| **Funciones** | Funciones del juego que faltan | Complemento de la fila anterior: grupos A–D de [lo que falta](#que-falta) | 2,381 | 11,804 | 20.17% de las funciones del juego | `████░░░░░░░░░░░░░░░░` |
| **Bytes** | Bytes de código del juego reemplazados | Suma de los cuerpos de esas mismas funciones, en bytes de código | 2,744,697 | 3,007,877 | 91.25% de los bytes de código del juego | `██████████████████░░` |
| **Clases** | Clases del juego completas | Clases o grupos del catálogo con todas sus entradas reconstruidas y sin envolturas del juego abiertas | 861 | 861 | 100.00% de las clases del juego | `████████████████████` |
| **Validación** | Clases del juego validadas | Clases completas cuyas entradas tienen todas `validated=1` | 861 | 861 | 100.00% de las clases completas | `████████████████████` |
| **Entradas del catálogo** | Entradas del juego reconstruidas | Filas de `hooks.csv` con `reversed=1` más las envolturas del juego abiertas, que cuentan como no reconstruidas | 9,457 | 9,457 | 100.00% de las entradas del juego del catálogo | `████████████████████` |
| **Validación** | Entradas del juego validadas | Entradas reconstruidas con evidencia aprobada vinculada (`validated=1`) | 9,457 | 9,457 | 100.00% de las entradas reconstruidas | `████████████████████` |
| **Entradas del catálogo** | Entradas reconstruidas escritas solo en C++ | Entradas reconstruidas cuyo cuerpo fuente no tiene `__asm` ni `naked` (las de fuente no identificada de forma inequívoca no cuentan) | 9,450 | 9,457 | 99.93% de las entradas reconstruidas | `████████████████████` |

**Por qué hay un 80% y un 91%.** No se contradicen: miden cosas distintas. El 79.83% cuenta *funciones* y el 91.25% cuenta *bytes* de código; las funciones que faltan son más pequeñas que las ya reemplazadas, así que quedan muchas funciones pero pocos bytes. Las filas «Funciones» y «Bytes» miden el propio EXE (análisis estático, ver [cobertura real del EXE](#cobertura-exe)) y son la medida del avance hacia la meta; las filas de «Entradas del catálogo», «Clases» y «Validación» miden las filas registradas de `hooks.csv`, que solo conocen una parte del código del juego, por eso su porcentaje es mayor. Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL. La última fila cuenta, de las entradas reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md).

**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md).

Hay 9,457 entradas catalogadas del juego, 0 bloqueadas y 9,457 sin bloqueo. No constituyen un inventario exhaustivo del código del juego; las filas de RenderWare y del CRT del catálogo (83) quedan fuera de este informe. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Las extensiones o adaptaciones NOTSA (4, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

<a id="mediciones"></a>

## 2. Siguientes mediciones

Estado al 09/11/2026. Lo que ya se midió y lo que todavía no tiene un porcentaje útil:

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validación vigente para el árbol actual (validación) | 0/9,457 entradas validadas confirmadas como vigentes (0.00% de las entradas validadas) | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). El 04/11/2026 se repitieron 20 bancos desfasados sin regresión de producción (todos eran bancos desfasados o artefactos del entorno): falta consolidarlos en el registro. |
| Dependencias de funciones originales eliminadas (funciones) | Medido (cota inferior): 520 funciones del juego siguen llamándose directamente desde código del juego ya reemplazado; cierre transitivo: 678 funciones / 99,894 bytes. Ver [Cobertura real del EXE](#cobertura-exe) | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan: lista viva en la [sección 10](#grupo-a). |
| Trabajo real que falta (funciones y bytes) | 2,381 funciones y 263,180 bytes del juego sin reemplazar (20.17% de las funciones y 8.75% de los bytes del juego); grupo A 678 funciones y 99,894 bytes | Reconstruir y validar de menor a mayor coste, empezando por el grupo A ([sección 6](#que-falta)). |
| Coste por categoría de lote (tiempo de reconstrucción y validación) | Medido para las categorías 1, 2, 3 y 5 en los lotes 90A, 90B, 92A, 92B y 92C (tabla de abajo); sin medir la categoría 4 (funciones con FPU) | Cronometrar un lote piloto con FPU (método x87 a 24 bits) y los siguientes lotes de lógica para convertir la estimación en un coste por función. |

Costes medidos en los lotes cerrados (datos al 09/11/2026, [lotes.csv](JuegoExe/lotes.csv)). Cada lote reconstruye en C++ puro funciones que antes se llamaban al EXE por dirección y las valida dentro del juego (pares EXE/DLL con cero diferencias).

| Lote | Categoría de coste (del grupo A) | Funciones | Bytes | Pares validados | Coste medido |
| --- | --- | ---: | ---: | ---: | --- |
| 90A | 1. Triviales y thunks muy llamados (≤ 0x20 B sin llamadas) | 10 | 171 | 20,480 | tiempo no registrado; 2048 casos por función (16 familias de 128) |
| 90B | 2. Pequeñas sin FPU | 8 | 302 | 32,768 | ~33 min hasta aprobado y registrado (~4 min por función) con la plantilla Fn85A ya hecha; más de la mitad de las ~175 candidatas se descartaron al leer el EXE |
| 92A | 3. Lógica (161-633 B, menos de 0x300) | 3 | 1,383 | 49,152 | ~6 min de compilación de sonda y corrida; compilación completa del worktree ~3 min |
| 92B | 5. ≥ 0x300 B (una por lote) | 1 | 774 | 16,384 | ~2 min por corrida; compilación completa 4 min 24 s |
| 92C | 5. ≥ 0x300 B (una por lote) | 1 | 1,232 | 16,384 | 3-4 min por corrida (recompila el archivo de la clase con /O2) |
| **Total** | | **23** | **3,862** | **135,168** | Solo hay tiempo de pared de las categorías 2, 3 y 5; la 1 no se cronometró y la 4 (FPU) no se ha medido. |

<a id="siguiente-fase"></a>

## 3. Siguiente fase: después de las envolturas

**¿Quedan envolturas de código del juego en el catálogo?** No. **Envolturas del juego abiertas: 0** (comprobado el 09/11/2026): todas las funciones del juego con hook están reconstruidas en C++.

Las envolturas de biblioteca no cuentan: 211 funciones con envoltura al EXE (RenderWare SDK 140, Controlador D3D9 de RenderWare 58, CRT / runtime estático 11, QuickTime/QTML 2) son RenderWare, controlador D3D9, CRT y QuickTime, que se sustituyen según la [hoja de ruta](RutaExeIndependiente.md) y no son trabajo del juego ([lista](#bibliotecas)). La clasificación sale de la dirección del destino: dentro de 0x401000–0x74FFFF es juego, fuera es biblioteca.

**Lotes en curso:** ninguno registrado para las envolturas del juego.

Cerrar las envolturas del juego no cierra el juego: quedan además las funciones del EXE sin reemplazar (grupos A–D, 2,381 funciones al 09/11/2026; [sección 6](#que-falta)). El plan de trabajo sale de [AnalisisGrupoA.md](AnalisisGrupoA.md) y su [lista completa](JuegoExe/grupoA.csv), y la lista viva está en la [sección 10](#grupo-a):

1. **Grupo A, por categorías y de menor a mayor coste:** helpers triviales y thunks muy llamados (hecho en el lote 90A: `CPad::GetPad` 0x53FB70, `CRGBA::Constructor` 0x7170C0...), pequeñas sin FPU (lote 90B), lógica sin FPU (empezando por las más llamadas y por clases enteras), funciones con FPU con el método x87 a 24 bits, y por último las de 0x300 bytes o más, una por lote (92B, 92C).
2. **Lote piloto de cada categoría** para medir el coste real: ya medidos los de las categorías 1, 2, 3 y 5 (tabla de la sección 2); falta la 4, con FPU.
3. **Revalidación vigente:** repetir los bancos sobre el árbol actual, priorizando los de fuentes cambiadas por lotes posteriores. Motivo: el banco `CVehD` llevaba roto desde el lote 83C (se cambió `m_fSteerAngle` por `m_GasPedal` en `ProcessDrivingAnims`) y no se vio hasta repetirlo.
4. **Después del código del juego:** `librw`, pipelines de SA y puentes `RwD3D9*`, QuickTime, DDraw y CRT, según la [hoja de ruta](RutaExeIndependiente.md); el cambio de método de validación sin el exe como oráculo.

**Validación consolidada parcialmente:** 9457 funciones del juego con evidencia aprobada vinculada, 1778 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias (cifras al 09/11/2026). Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

<a id="vista-general"></a>

## 4. Vista general

Al 09/11/2026. Cada columna indica la dimensión y el denominador de su porcentaje.

| Clases validadas (validación) | Clases completas por validar (validación) | Entradas validadas (validación) | Entradas reconstruidas por validar (validación) | Entradas por reconstruir (entradas del catálogo) |
| :---: | :---: | :---: | :---: | :---: |
| **861 / 861 clases** | **0 / 861 clases** | **9,457 / 9,457 entradas** | **0 / 9,457 entradas** | **0 / 9,457 entradas** |
| 100.00% de las clases completas | 0.00% de las clases completas | 100.00% de las entradas reconstruidas | 0.00% de las entradas reconstruidas | 0.00% de las entradas del juego del catálogo |

Reconstrucción por clases: 861 completas, 0 parciales y 0 sin revertir de 861 clases o grupos.

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego ni el avance sobre todo el EXE (para eso, las filas «Funciones» y «Bytes» de la sección 1). Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="cobertura-exe"></a>

## 5. Cobertura real del EXE (estimación)

Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras filas de la tabla de dimensiones salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático ([MemoryMap](MemoryMap/README.md): 17,765 entradas de función detectadas y 3.72 MB de cuerpos de función), cruzado con el catálogo, el registro de validación y las envolturas pendientes. 9,540 filas reconstruidas del catálogo equivalen a **9,493 funciones distintas del EXE ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo). En esta tabla, «% de funciones» es la parte de las funciones de esa región que ya está en C++ y «% de bytes» la parte de los bytes de código de esa región.

| Región del EXE | Funciones (total de la región) | Bytes de código (total de la región) | Reemplazadas por C++: funciones y % de funciones | Reemplazadas: % de bytes | Funciones de la región llamadas directamente desde código ya reemplazado |
| --- | ---: | ---: | --- | --- | ---: |
| Juego y plataforma (0x401000–0x74FFFF) | 11,804 | 3,007,877 | `████████████████░░░░` 9,423 · 79.8% de funciones | `██████████████████░░` 91.3% de bytes | 520 |
| RenderWare y D3D (0x750000–0x81FFFF) | 1,889 | 600,971 | `█░░░░░░░░░░░░░░░░░░░` 61 · 3.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.3% de bytes | 227 |
| CRT/MSVC y runtime (0x820000 en adelante) | 4,072 | 115,711 | `░░░░░░░░░░░░░░░░░░░░` 9 · 0.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.2% de bytes | 61 |
| **Total** | **17,765** | **3,724,559** | `███████████░░░░░░░░░` 9,493 · 53.4% de funciones | `███████████████░░░░░` 73.7% de bytes | **808** |

**Qué significa para un ejecutable independiente.** No hace falta reconstruir las 17,765 funciones: RenderWare/D3D y el CRT son bibliotecas enlazadas dentro del EXE (5,961 entradas, 19% de los bytes del EXE) que se sustituirían enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego (11,804 funciones, 81% de los bytes del EXE), y de él ya está reemplazado el **91% de los bytes del juego** y el 80% de las funciones del juego: no se contradicen, las funciones que quedan son más pequeñas que la media. Entre las 11,804 funciones del juego hay 1,835 de 16 bytes o menos (stubs, thunks y helpers).

<a id="que-falta"></a>

## 6. Lo que falta del código del juego: 2,381 funciones, 263 KB

Al 09/11/2026: 2,381 funciones (20.17% de las funciones del juego) y 263,180 bytes (8.75% de los bytes del juego) siguen sin reemplazar. Este es el trabajo real que falta del código del juego, lo mida o no el catálogo. Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).

| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |
| --- | ---: | ---: | --- |
| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | 678 | 99,894 | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. Lista viva: [dependencias directas (grupo A)](#grupo-a). |
| C. Llamadas directas solo desde código aún no reemplazado | 499 | 66,064 | Dependen de reconstruir antes A y B; parte puede ser código muerto. |
| D. Sin llamadas directas ni vtable | 412 | 59,097 | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |
| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | 792 | 38,125 | Probablemente necesarios: se ejecutan por despacho virtual. |

Del código del juego ya reemplazado, **520 funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura. No hay envolturas del juego abiertas (sección siguiente).

> [!NOTE]
> **Cotas y límites.** Es un análisis estático: hay 6,004 llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.

<a id="no-en-cpp"></a>

## 7. Funciones del juego que ya tenemos y aún no están en C++: 0

Al 09/11/2026, el catálogo **no tiene envolturas abiertas de código del juego**: todas las funciones del juego con hook están reconstruidas en C++. Las de biblioteca (abajo) no cuentan.

<a id="bibliotecas"></a>

### Bibliotecas, fuera del objetivo (211 envolturas)

Estas envolturas llaman al EXE, pero su destino es una biblioteca enlazada dentro del ejecutable (RenderWare, controlador D3D9, CRT, QuickTime) y **no cuentan** como trabajo pendiente del juego ni en las cifras de este informe. La clase sale de la dirección del destino (fuera de 0x401000–0x74FFFF) o de la clasificación del cohorte; no se reconstruyen: se sustituyen según la [hoja de ruta](RutaExeIndependiente.md).

| Biblioteca | Envolturas | De ellas, antes en «helpers sin clase» | Plan |
| --- | ---: | ---: | --- |
| RenderWare SDK | 140 | 6 | Sustituir por **librw** (o una implementación equivalente de RenderWare 3.7) y reconstruir los pipelines propios de SA; no se reescribe desde el EXE. |
| Controlador D3D9 de RenderWare | 58 | 0 | Sustituir por el **backend D3D9** de la biblioteca de sustitución (puentes `RwD3D9*`); fuera de la reconstrucción del juego. |
| CRT / runtime estático | 11 | 4 | Usar la **CRT del compilador** (malloc/free, operator new/delete, funciones matemáticas); fuera de la reconstrucción del juego. |
| QuickTime/QTML | 2 | 0 | Reemplazar o desactivar el reproductor de vídeo (QuickTime/QTML) en el ejecutable propio; fuera de la reconstrucción del juego. |

<details>
<summary>Envolturas de biblioteca detectadas en el código fuente y reclasificadas por su dirección (10)</summary>

- `RpMaterialUVAnimApplyUpdate` `0x007CC110` · RenderWare SDK
- `RwEngineGetVideoModeInfo` `0x007F2CF0` · RenderWare SDK
- `RwTextureRead` `0x007F3AC0` · RenderWare SDK
- `_rwD3D9SetStreams` `0x007FA090` · RenderWare SDK
- `_rwObjectHasFrameSetFrame` `0x00804EF0` · RenderWare SDK
- `RxLockedPipeAddFragment` `0x00806BE0` · RenderWare SDK
- `operator new[]` `0x00821195` · CRT / runtime estático
- `operator new` `0x0082119A` · CRT / runtime estático
- `operator delete[]` `0x008213AE` · CRT / runtime estático
- `operator delete` `0x008214BD` · CRT / runtime estático

</details>

<a id="completas"></a>
## 8. 🟢 Completas · 861 clases

Al 09/11/2026: 861 de 861 clases o grupos están completas (todas sus entradas reconstruidas y ninguna envoltura del juego abierta). Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (8) ✅ 8 | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (5) ✅ 5 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (18) ✅ 18 | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (9) ✅ 9 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (3) ✅ 3 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (14) ✅ 14 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (7) ✅ 7 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (2) ✅ 2 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (10) ✅ 10 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CMaths](../source/game_sa/Maths.cpp) (4) ✅ 4 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (7) ✅ 7 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CMatrix](../source/game_sa/Core/Matrix.cpp) (34) ✅ 34 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (10) ✅ 10 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (8) ✅ 8 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (10) ✅ 10 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (12) ✅ 12 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (6) ✅ 6 | [CMBlur](../source/game_sa/MBlur.cpp) (5) ✅ 5 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (7) ✅ 7 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (6) ✅ 6 | [CMemoryHeap](../source/game_sa/Memory/MemoryHeap.cpp) (1) ✅ 1 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (9) ✅ 9 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CMemoryMgr](../source/game_sa/MemoryMgr.cpp) (10) ✅ 10 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (8) ✅ 8 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CMentalState](../source/game_sa/MentalHealth.cpp) (1) ✅ 1 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (9) ✅ 9 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (7) ✅ 7 | [CMenuManager](../source/game_sa/MenuManager.cpp) (61) ✅ 61 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (11) ✅ 11 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (12) ✅ 12 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (7) ✅ 7 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (2) ✅ 2 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (55) ✅ 55 | [CMissionCleanup](../source/game_sa/MissionCleanup.cpp) (3) ✅ 3 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (7) ✅ 7 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (8) ✅ 8 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (7) ✅ 7 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (29) ✅ 29 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (5) ✅ 5 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (12) ✅ 12 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (5) ✅ 5 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (8) ✅ 8 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) ✅ 8 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (20) ✅ 20 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (3) ✅ 3 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (3) ✅ 3 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (3) ✅ 3 | [CNearbyFireScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (8) ✅ 8 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [CNodeAddress](../source/game_sa/NodeAddress.cpp) (3) ✅ 3 | [CTaskComplexUseMobilePhone](../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp) (1) ✅ 1 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (6) ✅ 6 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (2) ✅ 2 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (12) ✅ 12 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (6) ✅ 6 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (8) ✅ 8 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (12) ✅ 12 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (51) ✅ 51 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (8) ✅ 8 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (14) ✅ 14 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (6) ✅ 6 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (7) ✅ 7 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (7) ✅ 7 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (22) ✅ 22 | [CObjectPotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (5) ✅ 5 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (12) ✅ 12 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (58) ✅ 58 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (8) ✅ 8 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (7) ✅ 7 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 17 | [Common](../source/game_sa/common.cpp) (23) ✅ 23 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (7) ✅ 7 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 43 | [CompressedVector](../source/game_sa/CompressedVector.cpp) (2) ✅ 2 | [CTaskComplexWaitForPed](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) (2) ✅ 2 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (19) ✅ 19 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (9) ✅ 9 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (11) ✅ 11 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (7) ✅ 7 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (13) ✅ 13 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (19) ✅ 19 | [CPad](../source/game_sa/Pad.cpp) (82) ✅ 82 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (13) ✅ 13 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (14) ✅ 14 | [CPathFind](../source/game_sa/PathFind.cpp) (71) ✅ 71 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (8) ✅ 8 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (8) ✅ 8 | [CPathNode](../source/game_sa/PathFind.cpp) (1) ✅ 1 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (6) ✅ 6 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (24) ✅ 24 | [CPatrolRoutePool](../source/game_sa/Pools/PatrolRoutePool.h) (1) ✅ 1 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (17) ✅ 17 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (3) ✅ 3 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (152) ✅ 152 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (9) ✅ 9 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (14) ✅ 14 | [CPedAcquaintanceScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (6) ✅ 6 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (2) ✅ 2 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (20) ✅ 20 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (6) ✅ 6 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) ✅ 14 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (4) ✅ 4 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (7) ✅ 7 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (4) ✅ 4 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (5) ✅ 5 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (106) ✅ 106 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (9) ✅ 9 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (10) ✅ 10 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (14) ✅ 14 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (7) ✅ 7 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (5) ✅ 5 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (7) ✅ 7 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 38 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (9) ✅ 9 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (10) ✅ 10 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (8) ✅ 8 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (3) ✅ 3 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (9) ✅ 9 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (5) ✅ 5 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (2) ✅ 2 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (9) ✅ 9 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (11) ✅ 11 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (3) ✅ 3 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (6) ✅ 6 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (2) ✅ 2 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (8) ✅ 8 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (3) ✅ 3 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (20) ✅ 20 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (3) ✅ 3 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (7) ✅ 7 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (28) ✅ 28 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (10) ✅ 10 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (18) ✅ 18 | [CTaskInteriorSitInChair](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp) (1) ✅ 1 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (9) ✅ 9 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (14) ✅ 14 | [CPedGroups](../source/game_sa/PedGroups.cpp) (12) ✅ 12 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (5) ✅ 5 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (6) ✅ 6 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (20) ✅ 20 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (32) ✅ 32 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (59) ✅ 59 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (9) ✅ 9 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (3) ✅ 3 |
| [CAttractorScanner](../source/game_sa/AttractorScanner.cpp) (2) ✅ 2 | [CPedList](../source/game_sa/PedList.cpp) (7) ✅ 7 | [CTaskSimple](../source/game_sa/Tasks/Task.cpp) (1) ✅ 1 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (82) ✅ 82 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (7) ✅ 7 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (5) ✅ 5 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (4) ✅ 4 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (6) ✅ 6 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (104) ✅ 104 | [CPedPool](../source/game_sa/Pools/PedPool.h) (3) ✅ 3 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (2) ✅ 2 |
| [CAutoPilot](../source/game_sa/AutoPilot.cpp) (2) ✅ 2 | [CPedSaveStructure](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (4) ✅ 4 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (34) ✅ 34 | [CPedScanner](../source/game_sa/PedScanner.cpp) (1) ✅ 1 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (40) ✅ 40 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (8) ✅ 8 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (11) ✅ 11 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (10) ✅ 10 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (4) ✅ 4 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (11) ✅ 11 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (24) ✅ 24 | [CPedStats](../source/game_sa/PedStats.cpp) (5) ✅ 5 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (11) ✅ 11 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPedStuckChecker](../source/game_sa/PedStuckChecker.cpp) (1) ✅ 1 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (14) ✅ 14 |
| [CBoundingBox](../source/game_sa/Collision/BoundingBox.cpp) (2) ✅ 2 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (6) ✅ 6 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (10) ✅ 10 |
| [CBridge](../source/game_sa/Bridge.cpp) (4) ✅ 4 | [CPedType](../source/game_sa/PedType.cpp) (13) ✅ 13 | [CTaskSimpleCarForcePedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp) (5) ✅ 5 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (58) ✅ 58 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (10) ✅ 10 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (2) ✅ 2 | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (11) ✅ 11 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPickups](../source/game_sa/Pickups.cpp) (38) ✅ 38 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (6) ✅ 6 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (19) ✅ 19 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (10) ✅ 10 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (11) ✅ 11 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (10) ✅ 10 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (24) ✅ 24 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (10) ✅ 10 |
| [CCam](../source/game_sa/Cam.cpp) (56) ✅ 56 | [CPlaneBanners](../source/game_sa/PlaneBanners.cpp) (1) ✅ 1 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (7) ✅ 7 |
| [CCamera](../source/game_sa/Camera.cpp) (122) ✅ 122 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (5) ✅ 5 |
| [CCarAI](../source/game_sa/CarAI.cpp) (26) ✅ 26 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (7) ✅ 7 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (112) ✅ 112 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (6) ✅ 6 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 32 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (7) ✅ 7 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (20) ✅ 20 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (10) ✅ 10 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (8) ✅ 8 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (10) ✅ 10 |
| [CCarPathLink](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (29) ✅ 29 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (10) ✅ 10 |
| [CCheat](../source/game_sa/Cheat.cpp) (72) ✅ 72 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (66) ✅ 66 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (7) ✅ 7 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (7) ✅ 7 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CPlayerRelationshipRecorder](../source/game_sa/PlayerRelationshipRecorder.cpp) (5) ✅ 5 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (3) ✅ 3 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (4) ✅ 4 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (7) ✅ 7 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CPointLights](../source/game_sa/PointLights.cpp) (7) ✅ 7 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (6) ✅ 6 |
| [CClothes](../source/game_sa/Clothes.cpp) (12) ✅ 12 | [CPointRoute](../source/game_sa/PedGeometryAnalyser.cpp) (5) ✅ 5 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (15) ✅ 15 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (23) ✅ 23 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (4) ✅ 4 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CPools](../source/game_sa/Pools.cpp) (20) ✅ 20 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (6) ✅ 6 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (24) ✅ 24 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (6) ✅ 6 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CPopulation](../source/game_sa/Population.cpp) (68) ✅ 68 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (7) ✅ 7 |
| [CColBox](../source/game_sa/Collision/ColBox.cpp) (2) ✅ 2 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 38 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (5) ✅ 5 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (2) ✅ 2 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (6) ✅ 6 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (62) ✅ 62 | [CProjectile](../source/game_sa/Entity/Object/Projectile.cpp) (1) ✅ 1 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (2) ✅ 2 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (9) ✅ 9 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (1) ✅ 1 |
| [CCollisionEventScanner](../source/game_sa/Collision/CollisionEventScanner.cpp) (1) ✅ 1 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (2) ✅ 2 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (17) ✅ 17 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (2) ✅ 2 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (6) ✅ 6 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (11) ✅ 11 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (12) ✅ 12 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (8) ✅ 8 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (12) ✅ 12 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (8) ✅ 8 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CQuaternion](../source/game_sa/Core/Quaternion.cpp) (10) ✅ 10 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (7) ✅ 7 |
| [CColPoint](../source/game_sa/Collision/ColPoint.cpp) (1) ✅ 1 | [CRadar](../source/game_sa/Radar.cpp) (66) ✅ 66 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (6) ✅ 6 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (10) ✅ 10 | [CTaskSimpleFight](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) (24) ✅ 24 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (23) ✅ 23 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (7) ✅ 7 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (7) ✅ 7 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CRect](../source/game_sa/Core/Rect.cpp) (9) ✅ 9 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (2) ✅ 2 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (11) ✅ 11 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (5) ✅ 5 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (6) ✅ 6 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CRemote](../source/game_sa/Remote.cpp) (2) ✅ 2 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (6) ✅ 6 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (11) ✅ 11 | [CReplay](../source/game_sa/Replay.cpp) (47) ✅ 47 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (9) ✅ 9 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (7) ✅ 7 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (6) ✅ 6 |
| [CCoronas](../source/game_sa/Coronas.cpp) (12) ✅ 12 | [CRGBA](../source/game_sa/RGBA.cpp) (2) ✅ 2 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (4) ✅ 4 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 8 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (3) ✅ 3 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 7 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (3) ✅ 3 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (3) ✅ 3 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CRubbish](../source/game_sa/Rubbish.cpp) (1) ✅ 1 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (3) ✅ 3 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (39) ✅ 39 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (3) ✅ 3 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (6) ✅ 6 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (3) ✅ 3 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (3) ✅ 3 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (3) ✅ 3 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (3) ✅ 3 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (16) ✅ 16 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (11) ✅ 11 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (4) ✅ 4 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CSearchLight](../source/game_sa/SearchLight.cpp) (1) ✅ 1 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (3) ✅ 3 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CSector](../source/game_sa/Sector.cpp) (1) ✅ 1 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (14) ✅ 14 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CSetPiece](../source/game_sa/SetPiece.cpp) (7) ✅ 7 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (7) ✅ 7 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (16) ✅ 16 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (10) ✅ 10 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (10) ✅ 10 | [CSexyPedScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (8) ✅ 8 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (36) ✅ 36 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (20) ✅ 20 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (8) ✅ 8 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (7) ✅ 7 | [CShadows](../source/game_sa/Shadows.cpp) (30) ✅ 30 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (6) ✅ 6 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (4) ✅ 4 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (5) ✅ 5 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CShopping](../source/game_sa/Shopping.cpp) (30) ✅ 30 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (14) ✅ 14 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (8) ✅ 8 |
| [CData](../source/game_sa/Text/Text.cpp) (2) ✅ 2 | [CSimpleTransform](../source/game_sa/SimpleTransform.cpp) (3) ✅ 3 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (7) ✅ 7 |
| [CDebug](../source/game_sa/Debug.cpp) (2) ✅ 2 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (6) ✅ 6 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (7) ✅ 7 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CDecisionMaker](../source/game_sa/DecisionMakers/DecisionMaker.cpp) (1) ✅ 1 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (3) ✅ 3 |
| [CDecisionMakerTypes](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) (10) ✅ 10 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (5) ✅ 5 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CSphere](../source/game_sa/Collision/Sphere.cpp) (1) ✅ 1 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (4) ✅ 4 |
| [CDirectory](../source/game_sa/Directory.cpp) (10) ✅ 10 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (10) ✅ 10 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (35) ✅ 35 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (4) ✅ 4 |
| [CDraw](../source/game_sa/Draw.cpp) (2) ✅ 2 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (6) ✅ 6 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (10) ✅ 10 | [CStats](../source/game_sa/Stats.cpp) (51) ✅ 51 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (12) ✅ 12 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (2) ✅ 2 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) ✅ 3 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (3) ✅ 3 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (3) ✅ 3 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (13) ✅ 13 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (3) ✅ 3 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CStoredCar](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (5) ✅ 5 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (70) ✅ 70 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (7) ✅ 7 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CStreaming](../source/game_sa/Streaming.cpp) (95) ✅ 95 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculator](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp) (1) ✅ 1 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (9) ✅ 9 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (3) ✅ 3 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (7) ✅ 7 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CTagManager](../source/game_sa/TagManager.cpp) (20) ✅ 20 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (1) ✅ 1 | [CTask](../source/game_sa/Tasks/Task.cpp) (6) ✅ 6 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (6) ✅ 6 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (12) ✅ 12 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (6) ✅ 6 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (6) ✅ 6 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (19) ✅ 19 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (5) ✅ 5 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (6) ✅ 6 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (4) ✅ 4 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (8) ✅ 8 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (6) ✅ 6 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (8) ✅ 8 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (2) ✅ 2 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (8) ✅ 8 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (5) ✅ 5 | [CTaskSimpleSlideToCoord](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp) (1) ✅ 1 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (4) ✅ 4 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (4) ✅ 4 |
| [CEvent](../source/game_sa/Events/Event.cpp) (10) ✅ 10 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (9) ✅ 9 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (5) ✅ 5 | [CTaskCategories](../source/game_sa/Tasks/TaskCategories.cpp) (3) ✅ 3 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (8) ✅ 8 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) ✅ 1 | [CTaskComplex](../source/game_sa/Tasks/Task.cpp) (2) ✅ 2 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (11) ✅ 11 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (2) ✅ 2 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (7) ✅ 7 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (4) ✅ 4 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (6) ✅ 6 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (10) ✅ 10 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (5) ✅ 5 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) ✅ 15 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (4) ✅ 4 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (5) ✅ 5 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (4) ✅ 4 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (5) ✅ 5 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (6) ✅ 6 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (10) ✅ 10 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (6) ✅ 6 |
| [CEventChatPartner](../source/game_sa/Events/EventChatPartner.cpp) (2) ✅ 2 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (10) ✅ 10 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (1) ✅ 1 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (6) ✅ 6 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (8) ✅ 8 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (4) ✅ 4 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (24) ✅ 24 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (12) ✅ 12 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (19) ✅ 19 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (5) ✅ 5 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (6) ✅ 6 |
| [CEventDeadPed](../source/game_sa/Events/EventDeadPed.cpp) (2) ✅ 2 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventDeath](../source/game_sa/Events/EventDeath.cpp) (1) ✅ 1 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (4) ✅ 4 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (2) ✅ 2 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (5) ✅ 5 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (6) ✅ 6 |
| [CEventDraggedOutCar](../source/game_sa/Events/EventDraggedOutCar.cpp) (2) ✅ 2 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (11) ✅ 11 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (6) ✅ 6 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (12) ✅ 12 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (9) ✅ 9 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (2) ✅ 2 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (2) ✅ 2 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (7) ✅ 7 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (5) ✅ 5 |
| [CEventFireNearby](../source/game_sa/Events/EventFireNearby.cpp) (2) ✅ 2 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (5) ✅ 5 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (3) ✅ 3 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (10) ✅ 10 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventGotKnockedOverByCar](../source/game_sa/Events/EventGotKnockedOverByCar.cpp) (2) ✅ 2 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (9) ✅ 9 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (12) ✅ 12 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (9) ✅ 9 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (71) ✅ 71 |
| [CEventGroupEvent](../source/game_sa/Events/EventGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (11) ✅ 11 | [CTheZones](../source/game_sa/TheZones.cpp) (22) ✅ 22 |
| [CEventGunAimedAt](../source/game_sa/Events/EventGunAimedAt.cpp) (2) ✅ 2 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (11) ✅ 11 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (6) ✅ 6 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (5) ✅ 5 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (2) ✅ 2 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (6) ✅ 6 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (2) ✅ 2 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (7) ✅ 7 | [CTimer](../source/game_sa/Timer.cpp) (15) ✅ 15 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (10) ✅ 10 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (2) ✅ 2 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (3) ✅ 3 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (6) ✅ 6 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (13) ✅ 13 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (6) ✅ 6 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (7) ✅ 7 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (40) ✅ 40 |
| [CEventInWater](../source/game_sa/Events/EventInWater.cpp) (2) ✅ 2 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (8) ✅ 8 | [CTrainNode](../source/game_sa/TrainNode.cpp) (1) ✅ 1 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (9) ✅ 9 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (5) ✅ 5 | [cTransmission](../source/game_sa/cTransmission.cpp) (4) ✅ 4 |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (9) ✅ 9 | [CTxdStore](../source/game_sa/TxdStore.cpp) (26) ✅ 26 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (1) ✅ 1 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (16) ✅ 16 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (4) ✅ 4 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (2) ✅ 2 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (10) ✅ 10 | [CVector](../source/game_sa/Core/Vector.cpp) (21) ✅ 21 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (2) ✅ 2 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (4) ✅ 4 | [CVector2D](../source/game_sa/Core/Vector.cpp) · [archivo 2](../source/game_sa/Core/Vector2D.cpp) (1) ✅ 1 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (1) ✅ 1 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (10) ✅ 10 | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (170) ✅ 170 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (3) ✅ 3 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (9) ✅ 9 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) ✅ 1 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (9) ✅ 9 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (5) ✅ 5 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (3) ✅ 3 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (10) ✅ 10 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (77) ✅ 77 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (7) ✅ 7 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (8) ✅ 8 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (2) ✅ 2 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (1) ✅ 1 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (9) ✅ 9 | [CVehiclePotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 |
| [CEventPedEnteredMyVehicle](../source/game_sa/Events/EventPedEnteredMyVehicle.cpp) (2) ✅ 2 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (10) ✅ 10 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventPedToChase](../source/game_sa/Events/EventPedToChase.cpp) (2) ✅ 2 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (9) ✅ 9 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPedToFlee](../source/game_sa/Events/EventPedToFlee.cpp) (2) ✅ 2 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (8) ✅ 8 | [CVehicleScanner](../source/game_sa/VehicleScanner.cpp) (1) ✅ 1 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (1) ✅ 1 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (8) ✅ 8 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) ✅ 84 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (7) ✅ 7 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (8) ✅ 8 | [CWanted](../source/game_sa/Wanted.cpp) (34) ✅ 34 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (2) ✅ 2 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (11) ✅ 11 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (1) ✅ 1 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (8) ✅ 8 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (8) ✅ 8 | [CWaterLevel](../source/game_sa/WaterLevel.cpp) (47) ✅ 47 |
| [CEventPotentialGetRunOver](../source/game_sa/Events/EventPotentialGetRunOver.cpp) (2) ✅ 2 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (9) ✅ 9 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 41 |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (3) ✅ 3 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (19) ✅ 19 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (10) ✅ 10 |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (3) ✅ 3 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (6) ✅ 6 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (12) ✅ 12 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (8) ✅ 8 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (13) ✅ 13 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (3) ✅ 3 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (3) ✅ 3 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (9) ✅ 9 | [CWeather](../source/game_sa/Weather.cpp) (13) ✅ 13 |
| [CEventRevived](../source/game_sa/Events/EventRevived.cpp) (2) ✅ 2 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (8) ✅ 8 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventScanner](../source/game_sa/Events/EventScanner.cpp) (2) ✅ 2 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (15) ✅ 15 | [CWorld](../source/game_sa/World.cpp) (96) ✅ 96 |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (9) ✅ 9 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (8) ✅ 8 | [CWorldScan](../source/game_sa/Renderer.cpp) (2) ✅ 2 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) ✅ 1 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (7) ✅ 7 | [CZone](../source/game_sa/Zone.cpp) (1) ✅ 1 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) ✅ 1 | [CTaskComplexGoPickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp) (5) ✅ 5 | [CZoneDef](../source/game_sa/CullZones.cpp) (1) ✅ 1 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (7) ✅ 7 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (5) ✅ 5 | [D3DIndexDataBuffer](../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp) (3) ✅ 3 |
| [CEventSexyPed](../source/game_sa/Events/EventSexyPed.cpp) (2) ✅ 2 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (6) ✅ 6 | [D3DResourceSystem](../source/game_sa/RenderWare/D3DResourceSystem.cpp) (12) ✅ 12 |
| [CEventSexyVehicle](../source/game_sa/Events/EventSexyVehicle.cpp) (2) ✅ 2 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (11) ✅ 11 | [D3DTextureBuffer](../source/game_sa/RenderWare/D3DTextureBuffer.cpp) (6) ✅ 6 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (3) ✅ 3 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (7) ✅ 7 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (6) ✅ 6 |
| [CEventSource](../source/game_sa/Events/EventSource.cpp) (1) ✅ 1 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (8) ✅ 8 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (7) ✅ 7 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (7) ✅ 7 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (3) ✅ 3 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (4) ✅ 4 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (7) ✅ 7 | [Fx_c](../source/game_sa/Fx/Fx.cpp) (28) ✅ 28 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (6) ✅ 6 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [FxEmitter_c](../source/game_sa/Fx/FxEmitter.cpp) (4) ✅ 4 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (2) ✅ 2 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (7) ✅ 7 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (9) ✅ 9 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (2) ✅ 2 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (3) ✅ 3 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (1) ✅ 1 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (4) ✅ 4 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (8) ✅ 8 | [FxFrustumInfo_c](../source/game_sa/Fx/FxFrustumInfo.cpp) (1) ✅ 1 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (4) ✅ 4 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (11) ✅ 11 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (5) ✅ 5 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (4) ✅ 4 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (7) ✅ 7 | [FxInterpInfo255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp) (1) ✅ 1 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (6) ✅ 6 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (4) ✅ 4 | [FxInterpInfo32_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp) (1) ✅ 1 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (10) ✅ 10 | [FxInterpInfoFloat_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp) (1) ✅ 1 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FxInterpInfoU255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp) (1) ✅ 1 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (49) ✅ 49 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (7) ✅ 7 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (5) ✅ 5 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (12) ✅ 12 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (2) ✅ 2 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (7) ✅ 7 | [FxPrtMult_c](../source/game_sa/Fx/FxPrtMult.cpp) (2) ✅ 2 |
| [CFont](../source/game_sa/Font.cpp) (38) ✅ 38 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) ✅ 9 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (4) ✅ 4 |
| [CFormation](../source/game_sa/Formation.cpp) (10) ✅ 10 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (9) ✅ 9 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (35) ✅ 35 |
| [CGame](../source/game_sa/Game.cpp) (22) ✅ 22 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (12) ✅ 12 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [Game](../source/app/app_game.cpp) (12) ✅ 12 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (8) ✅ 8 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CGangWars](../source/game_sa/GangWars.cpp) (36) ✅ 36 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (8) ✅ 8 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (8) ✅ 8 |
| [CGarage](../source/game_sa/Garage.cpp) (32) ✅ 32 | [CTaskComplexLeaveCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp) (9) ✅ 9 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (52) ✅ 52 |
| [CGarages](../source/game_sa/Garages.cpp) (28) ✅ 28 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (4) ✅ 4 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (24) ✅ 24 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (9) ✅ 9 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (21) ✅ 21 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (20) ✅ 20 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (8) ✅ 8 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (6) ✅ 6 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (11) ✅ 11 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (9) ✅ 9 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (2) ✅ 2 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (8) ✅ 8 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 32 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (8) ✅ 8 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (11) ✅ 11 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.h) (5) ✅ 5 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (8) ✅ 8 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (3) ✅ 3 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (10) ✅ 10 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (4) ✅ 4 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (3) ✅ 3 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (4) ✅ 4 | [Ps](../source/app/platform/win/WinPs.cpp) (23) ✅ 23 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (23) ✅ 23 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (4) ✅ 4 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CHud](../source/game_sa/Hud.cpp) (39) ✅ 39 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (10) ✅ 10 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (45) ✅ 45 |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (3) ✅ 3 | [RpAnisot](../source/game_sa/RenderWare/rw/rpanisot.cpp) (3) ✅ 3 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (14) ✅ 14 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (7) ✅ 7 | [Rs](../source/app/platform/platform.cpp) (30) ✅ 30 |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (7) ✅ 7 | [RwHelper](../source/game_sa/RwHelper.cpp) (40) ✅ 40 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (2) ✅ 2 | [Securom](../source/app/securom/Securom.cpp) (2) ✅ 2 |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (9) ✅ 9 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (47) ✅ 47 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (9) ✅ 9 | [tColLighting](../source/game_sa/Collision/ColPoint.cpp) (1) ✅ 1 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (13) ✅ 13 | [TList_c](../source/game_sa/Core/List_c.cpp) (3) ✅ 3 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (2) ✅ 2 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (11) ✅ 11 | [UserTracksLegacyVector](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (1) ✅ 1 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (9) ✅ 9 | [VectorConstructorIterator](../source/game_sa/RuntimeHelpers.cpp) (1) ✅ 1 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (4) ✅ 4 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (10) ✅ 10 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (10) ✅ 10 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (6) ✅ 6 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (10) ✅ 10 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (3) ✅ 3 |
| [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (10) ✅ 10 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (7) ✅ 7 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (7) ✅ 7 |

</details>


<a id="cohorte-nuevo"></a>
## 9. 📋 Cohorte del 17/10/2026: CERRADO (185 destinos del juego del cohorte: 185 validadas, 0 reconstruidas por validar y 0 sin reconstruir, al 09/11/2026)

> [!NOTE]
> **Cohorte cerrado.** Al 09/11/2026, los 185 destinos del código del juego del cohorte del 17/10/2026 (funciones del juego que `source/` llamaba por dirección y que no estaban reconstruidas ni validadas) tienen fila `reversed=1` y `validated=1` en [hooks.csv](hooks.csv): quedan **0 pendientes y 0 por validar**. Las 279 entradas restantes del cohorte son de bibliotecas (RenderWare, D3D9, CRT, QuickTime) y quedan fuera del objetivo ([sección 7](#bibliotecas)). La lista de abajo se conserva como histórico; ya no es trabajo pendiente.

Destinos originales explícitos que `source/` seguía referenciando y que no estaban reconstruidos ni validados el 17/10/2026. La lista va ordenada de mayor a menor tamaño en bytes (la columna «Rango» conserva la posición por dificultad, de menor a mayor: 1 es la más fácil; puntuación de tamaño y riesgos —llamadas sin catalogar, indirectas, x87, tablas de saltos y SEH—). Solo se listan funciones del juego. Lista completa y descargable: [lista-por-dificultad.csv](../reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv) · metodología: [README del cohorte](../reversing/OriginalDependencies2026-10-17/README.md). La columna «Estado» se calcula con el catálogo actual: una dirección con fila reversed=1 en hooks.csv figura como reconstruida o validada.

| Nivel de dificultad | Funciones del juego | Validadas | Reconstruidas por validar | Sin reconstruir |
| --- | ---: | ---: | ---: | ---: |
| 1 Muy fácil | 9 | 9 | 0 | 0 |
| 2 Fácil | 25 | 25 | 0 | 0 |
| 3 Media | 75 | 75 | 0 | 0 |
| 4 Difícil | 57 | 57 | 0 | 0 |
| 5 Muy difícil | 19 | 19 | 0 | 0 |
| **Total** | **185** | **185** | **0** | **0** |

<details>
<summary>Histórico: los 185 destinos del juego del cohorte, todos validados, de mayor a menor tamaño en bytes</summary>

| Rango | Nivel | Subcategoría | Función | Dirección | Bytes | Estado |
| ---: | --- | --- | --- | --- | ---: | --- |
| 178 | 5 Muy difícil | wrapper | CPed::ProcessEntityCollision | `0x005E2530` | 4342 | ✅ validada |
| 185 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysics_OnlyMission | `0x00436A90` | 4243 | ✅ validada |
| 182 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPath | `0x00434900` | 3729 | ✅ validada |
| 170 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeRandomly | `0x0042DE80` | 3591 | ✅ validada |
| 184 | 5 Muy difícil | embedded_call | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | `0x0051D770` | 3445 | ✅ validada |
| 181 | 5 Muy difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing | `0x00435830` | 3320 | ✅ validada |
| 162 | 5 Muy difícil | wrapper | CCarCtrl::FlyAIHeliInCertainDirection | `0x00429A70` | 2994 | ✅ validada |
| 169 | 5 Muy difícil | wrapper | CCarCtrl::DealWithBend_Racing | `0x00428040` | 2361 | ✅ validada |
| 175 | 5 Muy difícil | wrapper | CHeli::ProcessControl | `0x006C7050` | 2352 | ✅ validada |
| 163 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeToFollowPath | `0x00427740` | 2193 | ✅ validada |
| 155 | 5 Muy difícil | wrapper | CCarCtrl::PickNextNodeToChaseCar | `0x00426EF0` | 2111 | ✅ validada |
| 179 | 5 Muy difícil | wrapper | CMissionCleanup::Process | `0x00468560` | 1955 | ✅ validada |
| 140 | 5 Muy difícil | wrapper | CCarCtrl::FlyAIPlaneInCertainDirection | `0x00423000` | 1917 | ✅ validada |
| 176 | 5 Muy difícil | wrapper | CTaskSimpleFight::ProcessPed | `0x00629920` | 1907 | ✅ validada |
| 166 | 5 Muy difícil | wrapper | CCarCtrl::SlowCarDownForPedsSectorList | `0x00425440` | 1761 | ✅ validada |
| 147 | 5 Muy difícil | wrapper | CCarCtrl::DragCarToPoint | `0x0042EC90` | 1748 | ✅ validada |
| 173 | 5 Muy difícil | wrapper | CHeli::ProcessControlInputs | `0x006C4830` | 1571 | ✅ validada |
| 167 | 5 Muy difícil | wrapper | CCarCtrl::WeaveForOtherCar | `0x00426350` | 1558 | ✅ validada |
| 171 | 5 Muy difícil | wrapper | CTaskComplexPartner::CreateNextSubTask | `0x00683AD0` | 1521 | ✅ validada |
| 161 | 4 Difícil | wrapper | CHeli::ProcessFlyingCarStuff | `0x006C4E60` | 1467 | ✅ validada |
| 177 | 4 Difícil | wrapper | __MainWndProc | `0x00747EB0` | 1458 | ✅ validada |
| 148 | 4 Difícil | embedded_call | FxEmitter_c::CreateParticle | `0x004A2580` | 1434 | ✅ validada |
| 183 | 4 Difícil | wrapper | CTaskComplexLeaveCar::CreateNextSubTask | `0x006419F0` | 1428 | ✅ validada |
| 174 | 4 Difícil | wrapper | CTaskComplexFollowPedFootsteps::ControlSubTask | `0x00695090` | 1341 | ✅ validada |
| 154 | 4 Difícil | wrapper | CCarCtrl::FindAngleToWeaveThroughTraffic | `0x004325C0` | 1339 | ✅ validada |
| 124 | 4 Difícil | wrapper | CCarCtrl::FindNodesThisCarIsNearestTo | `0x0042BD20` | 1323 | ✅ validada |
| 164 | 4 Difícil | wrapper | FightHitPedExe | `0x0061CBA0` | 1289 | ✅ validada |
| 132 | 4 Difícil | wrapper | CCarCtrl::SteerAICarTowardsPointInEscort | `0x004336D0` | 1223 | ✅ validada |
| 143 | 4 Difícil | wrapper | CTaskComplexLeaveCar::CreateSubTask | `0x00641530` | 1214 | ✅ validada |
| 135 | 4 Difícil | wrapper | CStoredCar::RestoreCar | `0x00447E40` | 1211 | ✅ validada |
| 180 | 4 Difícil | wrapper | CHeli::PreRender | `0x006C5420` | 1204 | ✅ validada |
| 122 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath | `0x00432DD0` | 1177 | ✅ validada |
| 152 | 4 Difícil | wrapper | CCarCtrl::CreateConvoy | `0x0042C740` | 1168 | ✅ validada |
| 120 | 4 Difícil | wrapper | CCarCtrl::FlyAIHeliToTarget_FixedOrientation | `0x00423940` | 1167 | ✅ validada |
| 165 | 4 Difícil | wrapper | CCarCtrl::CreatePoliceChase | `0x0042C2B0` | 1161 | ✅ validada |
| 100 | 4 Difícil | wrapper | CCarCtrl::TestCollisionBetween2MovingRects | `0x00425B30` | 1081 | ✅ validada |
| 172 | 4 Difícil | wrapper | CTaskComplexLeaveCar::MakeAbortable | `0x00641100` | 1069 | ✅ validada |
| 144 | 4 Difícil | wrapper | CCarCtrl::SlowCarDownForOtherCar | `0x0042D0E0` | 1032 | ✅ validada |
| 158 | 4 Difícil | wrapper | CPlayerPed::MakeThisPedJoinOurGroup | `0x0060C840` | 1029 | ✅ validada |
| 151 | 4 Difícil | wrapper | CAutomobile::Render | `0x006A2B10` | 1005 | ✅ validada |
| 157 | 4 Difícil | wrapper | CVehicle::GetPlaneWeaponFiringStatus | `0x006E3440` | 944 | ✅ validada |
| 118 | 4 Difícil | wrapper | CCarCtrl::SteerAIHeliToKeepEntityInView | `0x0042AEB0` | 939 | ✅ validada |
| 156 | 4 Difícil | wrapper | CCarCtrl::IsThisAnAppropriateNode | `0x0042DAB0` | 932 | ✅ validada |
| 103 | 4 Difícil | wrapper | CCarCtrl::ClitargetOrientationToLink | `0x00422760` | 918 | ✅ validada |
| 137 | 4 Difícil | wrapper | CCarCtrl::SteerAIHeliToFollowEntity | `0x0042A750` | 896 | ✅ validada |
| 160 | 4 Difícil | wrapper | CHeli::SendDownSwat | `0x006C69C0` | 869 | ✅ validada |
| 138 | 4 Difícil | wrapper | CHeli::GenerateHeli | `0x006C6520` | 867 | ✅ validada |
| 93 | 4 Difícil | wrapper | CCarCtrl::FindLinksToGoWithTheseNodes | `0x0042B470` | 862 | ✅ validada |
| 123 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsHeadingForTarget | `0x00433280` | 841 | ✅ validada |
| 139 | 4 Difícil | wrapper | CTaskComplexPartnerChat::GetPartnerSequence | `0x00684380` | 836 | ✅ validada |
| 168 | 4 Difícil | wrapper | CTaskComplexKillPedOnFoot::CreateNextSubTask | `0x0062B150` | 831 | ✅ validada |
| 141 | 4 Difícil | wrapper | CCarCtrl::WeaveForObject | `0x00426BC0` | 803 | ✅ validada |
| 128 | 4 Difícil | wrapper | CCarCtrl::SteerAICarParkPerpendicular | `0x00433EA0` | 800 | ✅ validada |
| 113 | 4 Difícil | wrapper | CHeli::BlowUpCar | `0x006C6D30` | 795 | ✅ validada |
| 150 | 4 Difícil | wrapper | CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents | `0x00603720` | 777 | ✅ validada |
| 95 | 4 Difícil | embedded_call | helper sin símbolo confirmado (usado por CPlayerInfo::Process) | `0x0056EC80` | 776 | ✅ validada |
| 134 | 4 Difícil | wrapper | CCarCtrl::SteerAICarParkParallel | `0x00433BA0` | 762 | ✅ validada |
| 131 | 4 Difícil | wrapper | CCarCtrl::SteerAICarBlockingPlayerForwardAndBack | `0x00422B20` | 745 | ✅ validada |
| 159 | 4 Difícil | wrapper | CStats::BuildStatLine | `0x00559230` | 735 | ✅ validada |
| 111 | 4 Difícil | wrapper | CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer | `0x00428DE0` | 684 | ✅ validada |
| 78 | 4 Difícil | wrapper | CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper | `0x00425F70` | 672 | ✅ validada |
| 96 | 4 Difícil | wrapper | CCarCtrl::ChooseModel | `0x00424CE0` | 672 | ✅ validada |
| 127 | 4 Difícil | wrapper | CMentalState::Process | `0x006008A0` | 646 | ✅ validada |
| 153 | 4 Difícil | wrapper | CTaskComplexWalkRoundObject::CreateFirstSubTask | `0x00657380` | 620 | ✅ validada |
| 101 | 4 Difícil | wrapper | CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer | `0x00429090` | 613 | ✅ validada |
| 125 | 4 Difícil | embedded_call | helper sin símbolo confirmado (usado por CWeaponEffects::Render) | `0x0056EF90` | 608 | ✅ validada |
| 69 | 4 Difícil | embedded_call | CPed::SpecialEntityPreCollisionStuff | `0x005E3C30` | 603 | ✅ validada |
| 94 | 4 Difícil | wrapper | CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop | `0x00428990` | 583 | ✅ validada |
| 116 | 4 Difícil | embedded_call | CTaskSimpleSlideToCoord::ProcessPed | `0x0066C4E0` | 570 | ✅ validada |
| 146 | 4 Difícil | wrapper | RsLoadPresetViews | `0x00619D60` | 552 | ✅ validada |
| 109 | 4 Difícil | wrapper | Fx_c::AddWood | `0x0049EE10` | 549 | ✅ validada |
| 133 | 4 Difícil | wrapper | CRemote::GivePlayerRemoteControlledCar | `0x0045AB10` | 548 | ✅ validada |
| 97 | 4 Difícil | wrapper | CVehicle::FireHeatSeakingMissile | `0x006E05C0` | 529 | ✅ validada |
| 105 | 4 Difícil | wrapper | CCarCtrl::TriggerDogFightMoves | `0x00429300` | 526 | ✅ validada |
| 73 | 4 Difícil | wrapper | CCarCtrl::FindPathDirection | `0x00422090` | 515 | ✅ validada |
| 58 | 4 Difícil | wrapper | CCarCtrl::FireHeliRocketsAtTarget | `0x0042B270` | 506 | ✅ validada |
| 57 | 3 Media | wrapper | CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget | `0x00428BE0` | 497 | ✅ validada |
| 119 | 3 Media | wrapper | FightHitObjExe | `0x0061D400` | 494 | ✅ validada |
| 110 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateSubTask | `0x00695E40` | 488 | ✅ validada |
| 56 | 3 Media | embedded_call | CPathFind::FindParkingNodeInArea | `0x004513F0` | 476 | ✅ validada |
| 91 | 3 Media | wrapper | CCarCtrl::SteerAIHeliAsPoliceHeli | `0x0042AAD0` | 475 | ✅ validada |
| 117 | 3 Media | wrapper | CCarCtrl::GetAIPlaneToDoDogFight | `0x00429890` | 472 | ✅ validada |
| 126 | 3 Media | wrapper | CCarCtrl::GetAIHeliToAttackPlayer | `0x0042F3C0` | 472 | ✅ validada |
| 54 | 3 Media | wrapper | CVehicle::ScanAndMarkTargetForHeatSeekingMissile | `0x006E0400` | 445 | ✅ validada |
| 149 | 3 Media | wrapper | D3DResourceSystem::DestroyTexture | `0x00730B70` | 439 | ✅ validada |
| 108 | 3 Media | wrapper | CVehicle::AddReplacementUpgrade | `0x006D3830` | 423 | ✅ validada |
| 106 | 3 Media | embedded_call | FxEmitter_c::AddParticle | `0x004A3EA0` | 418 | ✅ validada |
| 142 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x004650F0` | 415 | ✅ validada |
| 112 | 3 Media | wrapper | CCarCtrl::SlowCarDownForCarsSectorList | `0x00432420` | 413 | ✅ validada |
| 104 | 3 Media | embedded_call | FxEmitter_c::AddParticle | `0x004A4050` | 400 | ✅ validada |
| 68 | 3 Media | embedded_call | CHeli::CHeli | `0x006C4190` | 398 | ✅ validada |
| 145 | 3 Media | wrapper | RsSavePresetView | `0x00619FA0` | 393 | ✅ validada |
| 88 | 3 Media | wrapper | CCarCtrl::SlowCarDownForObjectsSectorList | `0x0042D4F0` | 391 | ✅ validada |
| 51 | 3 Media | wrapper | CPlayerPed::EvaluateNeighbouringTarget | `0x0060D1C0` | 388 | ✅ validada |
| 92 | 3 Media | wrapper | CGarage::IsStaticPlayerCarEntirelyInside | `0x0044A830` | 386 | ✅ validada |
| 66 | 3 Media | wrapper | RpClumpGetBoundingSphere | `0x00734FC0` | 384 | ✅ validada |
| 60 | 3 Media | wrapper | SkinGetBonePositionsToTable | `0x00735360` | 381 | ✅ validada |
| 83 | 3 Media | wrapper | CCarCtrl::SteerAIHeliToLand | `0x0042AD30` | 370 | ✅ validada |
| 89 | 3 Media | wrapper | CVehicle::PossiblyDropFreeFallBombForPlayer | `0x006E07E0` | 359 | ✅ validada |
| 102 | 3 Media | wrapper | RwTextureGtaStreamRead | `0x00730E60` | 347 | ✅ validada |
| 81 | 3 Media | wrapper | CCarCtrl::WeaveThroughPedsSectorList | `0x0042D7E0` | 346 | ✅ validada |
| 75 | 3 Media | wrapper | CTaskComplexPartnerDeal::GetPartnerSequence | `0x00682440` | 344 | ✅ validada |
| 82 | 3 Media | wrapper | CCarCtrl::WeaveThroughObjectsSectorList | `0x0042D950` | 343 | ✅ validada |
| 64 | 3 Media | wrapper | CPed::SpecialEntityCalcCollisionSteps | `0x005E3E90` | 342 | ✅ validada |
| 80 | 3 Media | wrapper | CCarCtrl::WeaveThroughCarsSectorList | `0x0042D680` | 341 | ✅ validada |
| 129 | 3 Media | wrapper | CTaskComplexWalkRoundObject::ControlSubTask | `0x006575F0` | 341 | ✅ validada |
| 136 | 3 Media | wrapper | CTaskComplexWalkRoundObject::CreateNextSubTask | `0x00657220` | 341 | ✅ validada |
| 52 | 3 Media | wrapper | CVehicle::AddUpgrade | `0x006DFA20` | 333 | ✅ validada |
| 77 | 3 Media | embedded_call | CDecision::MakeDecision | `0x006040D0` | 333 | ✅ validada |
| 84 | 3 Media | wrapper | CGarage::IsEntityEntirelyInside3D | `0x00448BE0` | 333 | ✅ validada |
| 46 | 3 Media | embedded_call | FxInterpInfoFloat_c::GetVal | `0x004A8470` | 330 | ✅ validada |
| 130 | 3 Media | wrapper | CCarCtrl::PickNextNodeAccordingStrategy | `0x00432B10` | 321 | ✅ validada |
| 99 | 3 Media | wrapper | CTaskSimpleFight::MakeAbortable | `0x006239F0` | 284 | ✅ validada |
| 115 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateNextSubTask | `0x00694EE0` | 283 | ✅ validada |
| 107 | 3 Media | wrapper | CGarage::IsEntityEntirelyOutside | `0x00448D30` | 281 | ✅ validada |
| 61 | 3 Media | wrapper | CTaskComplexPartnerShove::GetPartnerSequence | `0x00683120` | 275 | ✅ validada |
| 114 | 3 Media | wrapper | D3DResourceSystem::DestroyIndexBuffer | `0x00730D30` | 263 | ✅ validada |
| 121 | 3 Media | wrapper | CCarCtrl::GetAIPlaneToAttackPlayer | `0x00429780` | 261 | ✅ validada |
| 98 | 3 Media | wrapper | CCarCtrl::SteerAIHeliTowardsTargetCoors | `0x0042A630` | 244 | ✅ validada |
| 71 | 3 Media | wrapper | CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget | `0x004335E0` | 240 | ✅ validada |
| 41 | 3 Media | wrapper | CPlantMgr::AtomicCreatePrelitIfNeeded | `0x005DD100` | 221 | ✅ validada |
| 33 | 3 Media | wrapper | CQuaternion::Get | `0x0059C080` | 215 | ✅ validada |
| 67 | 3 Media | wrapper | CCarCtrl::TestWhetherToFirePlaneGuns | `0x00429520` | 214 | ✅ validada |
| 74 | 3 Media | wrapper | diPadInit | `0x007485C0` | 214 | ✅ validada |
| 72 | 3 Media | wrapper | Fx_c::AddPunchImpact | `0x0049F670` | 209 | ✅ validada |
| 32 | 3 Media | wrapper | CCarCtrl::FindPercDependingOnDistToLink | `0x00422620` | 208 | ✅ validada |
| 36 | 3 Media | wrapper | CCarCtrl::SwitchBetweenPhysicsAndGhost | `0x004222A0` | 208 | ✅ validada |
| 79 | 3 Media | wrapper | D3DResourceSystem::CreateTexture | `0x00730510` | 204 | ✅ validada |
| 39 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CTaskSimpleGangDriveBy::AITarget) | `0x00621B10` | 201 | ✅ validada |
| 37 | 3 Media | wrapper | CPlayerPed::PlayerWantsToAttack | `0x0060CC50` | 200 | ✅ validada |
| 40 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x005BB9F0` | 197 | ✅ validada |
| 43 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CCam::Process_DW_HeliChaseCam) | `0x0050E090` | 192 | ✅ validada |
| 86 | 3 Media | wrapper | CTaskComplexPartner::ControlSubTask | `0x006840D0` | 192 | ✅ validada |
| 22 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CCam::Process_DW_HeliChaseCam) | `0x0050E180` | 184 | ✅ validada |
| 45 | 3 Media | wrapper | CTaskComplexPartner::CreateFirstSubTask | `0x00681F20` | 177 | ✅ validada |
| 26 | 3 Media | wrapper | CCarCtrl::FindSpeedMultiplier | `0x004224E0` | 167 | ✅ validada |
| 76 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CProjectileInfo::AddProjectile) | `0x005A4030` | 167 | ✅ validada |
| 31 | 3 Media | wrapper | FightStrikeDamageExe | `0x0061C740` | 164 | ✅ validada |
| 29 | 3 Media | embedded_call | CTrainNode::GetLightingFromCollision | `0x006F5F80` | 161 | ✅ validada |
| 85 | 3 Media | wrapper | D3DResourceSystem::CreateIndexBuffer | `0x007306A0` | 153 | ✅ validada |
| 30 | 3 Media | wrapper | CCustomBuildingDNPipeline::CustomPipeMaterialSetup | `0x005D7120` | 149 | ✅ validada |
| 25 | 2 Fácil | embedded_call | CHeli::FindSwatPositionRelativeToHeli | `0x006C4760` | 141 | ✅ validada |
| 28 | 3 Media | embedded_call | CGarage::IsGarageEmpty | `0x0044A9C0` | 141 | ✅ validada |
| 59 | 3 Media | wrapper | CCarCtrl::SteerAIPlaneToFollowEntity | `0x004237F0` | 141 | ✅ validada |
| 65 | 3 Media | wrapper | CTaskComplexFollowPedFootsteps::CreateFirstSubTask | `0x00695000` | 141 | ✅ validada |
| 15 | 2 Fácil | wrapper | CCarCtrl::StopCarIfNodesAreInvalid | `0x00422590` | 137 | ✅ validada |
| 38 | 2 Fácil | wrapper | CreateRouteTask | `0x00655140` | 130 | ✅ validada |
| 70 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por ProcessPlayerVehicleInteraction) | `0x006919C0` | 126 | ✅ validada |
| 23 | 3 Media | embedded_call | TransformPoint | `0x0059C890` | 124 | ✅ validada |
| 63 | 3 Media | wrapper | CCarCtrl::SteerAIHeliFlyingAwayFromPlayer | `0x0042ACB0` | 122 | ✅ validada |
| 62 | 2 Fácil | wrapper | CTaskComplexPartnerDeal::StreamRequiredAnims | `0x006823C0` | 113 | ✅ validada |
| 11 | 2 Fácil | wrapper | CStats::FindLeastFavoriteRadioStation | `0x00559010` | 110 | ✅ validada |
| 34 | 2 Fácil | wrapper | RemoveUpgradeCB | `0x006D3300` | 108 | ✅ validada |
| 35 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CTaskSimpleCarDrive::StartRollingDoor) | `0x0063C840` | 108 | ✅ validada |
| 87 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CAEUserRadioTrackManager::WriteUserTracksFile) | `0x004F4620` | 102 | ✅ validada |
| 21 | 3 Media | wrapper | CCarCtrl::FindIntersection2Lines | `0x004226F0` | 100 | ✅ validada |
| 18 | 2 Fácil | wrapper | CTaskComplexPartner::StreamRequiredAnims | `0x00682310` | 93 | ✅ validada |
| 53 | 2 Fácil | wrapper | CSearchLight::IsSpottedEntity | `0x00493900` | 90 | ✅ validada |
| 17 | 3 Media | wrapper | CCarCtrl::SteerAIHeliToCrashAndBurn | `0x004238E0` | 89 | ✅ validada |
| 42 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por WaitForPedUnwind) | `0x0061A3D0` | 85 | ✅ validada |
| 7 | 2 Fácil | wrapper | CCarCtrl::SteerAIPlaneToCrashAndBurn | `0x00423880` | 81 | ✅ validada |
| 19 | 3 Media | embedded_call | helper sin símbolo confirmado (usado por CameraObbeCinemaCycle) | `0x0050B830` | 77 | ✅ validada |
| 90 | 2 Fácil | wrapper | CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer | `0x0042F370` | 76 | ✅ validada |
| 55 | 3 Media | wrapper | CCarCtrl::SteerAIPlaneTowardsTargetCoors | `0x00423790` | 75 | ✅ validada |
| 27 | 2 Fácil | wrapper | CTaskComplexPartnerChat::MakeAbortable | `0x00682C60` | 65 | ✅ validada |
| 9 | 2 Fácil | wrapper | FindUpgradeCB | `0x006D3370` | 61 | ✅ validada |
| 49 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CDecisionMakerTypes::InjectHooks) | `0x00606060` | 59 | ✅ validada |
| 16 | 3 Media | wrapper | CQuaternion::Get | `0x0059C230` | 57 | ✅ validada |
| 50 | 2 Fácil | wrapper | CTaskComplexPartner::RemoveStreamedAnims | `0x00682370` | 56 | ✅ validada |
| 8 | 1 Muy fácil | wrapper | FindReplacementUpgradeCB | `0x006D3490` | 53 | ✅ validada |
| 12 | 1 Muy fácil | embedded_call | helper sin símbolo confirmado (usado por GetAnimHierarchyFromFrame) | `0x00734A70` | 51 | ✅ validada |
| 14 | 1 Muy fácil | wrapper | CAEStreamingDecoder scalar deleting destructor (operator delete de la clase) | `0x004F2860` | 50 | ✅ validada |
| 48 | 2 Fácil | wrapper | CDecisionMakerTypes::AddEventResponse | `0x006044C0` | 50 | ✅ validada |
| 24 | 2 Fácil | wrapper | CTaskComplexKillPedOnFoot::MakeAbortable | `0x00625E40` | 48 | ✅ validada |
| 6 | 1 Muy fácil | wrapper | CGarages::CountCarsInHideoutGarage | `0x0044A210` | 47 | ✅ validada |
| 3 | 1 Muy fácil | wrapper | CAutoPilot::RemoveOnePathNode | `0x0041B950` | 45 | ✅ validada |
| 13 | 2 Fácil | wrapper | CCarCtrl::ThisVehicleShouldTryNotToTurn | `0x00421FE0` | 38 | ✅ validada |
| 5 | 1 Muy fácil | wrapper | CDecisionMakerTypes::FlushDecisionMakerEventResponse | `0x00604490` | 35 | ✅ validada |
| 4 | 1 Muy fácil | wrapper | CPedGroups::RemoveAllFollowersFromGroup | `0x005FB8A0` | 32 | ✅ validada |
| 2 | 1 Muy fácil | embedded_call | CPool&lt;CVehicle&gt;::GetAt (acceso por índice a la ranura de 0xA18 bytes) | `0x0041CC10` | 31 | ✅ validada |
| 47 | 2 Fácil | wrapper | CCarCtrl::GetAIHeliToFlyInDirection | `0x0042A730` | 28 | ✅ validada |
| 10 | 2 Fácil | embedded_call | helper sin símbolo confirmado (usado por CHud::DrawVitalStats) | `0x00406CE0` | 19 | ✅ validada |
| 20 | 2 Fácil | wrapper | CTaskComplexFollowPedFootsteps::MakeAbortable | `0x00694ED0` | 8 | ✅ validada |
| 1 | 1 Muy fácil | embedded_call | helper sin símbolo confirmado (usado por FightUnwind) | `0x0061A3A0` | 7 | ✅ validada |
| 44 | 2 Fácil | wrapper | CTaskComplexPartnerDeal::CreateFirstSubTask | `0x006823B0` | 5 | ✅ validada |

</details>

**Cohorte inicial de 499 dependencias (comprobado el 09/11/2026).** De sus 255 destinos del juego, 255 tienen fila `reversed=1` y `validated=1` en hooks.csv (131 figuran como aprobadas en su `progress.csv` y el resto se validó después con otros bancos), 0 están reconstruidas por validar y **0 abiertas**. El «124 entradas del juego pendientes» que repite la cabecera de estado de otros documentos sale del `progress.json` de ese cohorte, que no se ha refrescado; no es la medida actual.

<a id="grupo-a"></a>

## 10. Dependencias directas de la DLL al EXE (grupo A)

Lista viva del trabajo real. Al 09/11/2026, el grupo A de [JuegoExe](JuegoExe/README.md) tiene **666 funciones del juego** (93,770 bytes) que la DLL ya necesita: 495 se llaman **directamente** desde código ya reemplazado (llamadas detectadas con dirección, bytes y llamador) y 160 solo de forma transitiva, a través de otras funciones aún no reemplazadas. La categoría y los llamadores salen del análisis de [AnalisisGrupoA.md](AnalisisGrupoA.md) ([grupoA.csv](JuegoExe/grupoA.csv), 1,007 funciones en el momento del análisis; 352 ya no son del grupo A: se reconstruyeron desde entonces o pasaron a envoltura abierta). El cierre transitivo de la [sección 6](#que-falta) cuenta 678 funciones: la diferencia (12) son funciones del grupo A que además tienen una envoltura abierta y figuran como envolturas abiertas ([sección 7](#no-en-cpp) y bibliotecas), no en esta lista. Es una cota inferior: las llamadas indirectas sin resolver no entran en el grafo.

### Composición por categoría de coste

| Categoría de coste | Funciones | % de funciones del grupo A | Bytes | % de bytes del grupo A | Directas desde código reemplazado | Lote medido |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| 1. Triviales y thunks (≤ 0x20 B sin llamadas, o un solo `jmp`) | 127 | 19.4% | 1,612 | 1.7% | 113 | 90A |
| 2. Pequeñas sin FPU (≤ 0x40 B, ≤ 1 llamada) | 129 | 19.7% | 5,248 | 5.6% | 86 | 90B |
| 3. Lógica sin FPU (< 0x300 B) | 312 | 47.6% | 50,378 | 54.2% | 219 | 92A |
| 4. Con FPU (método x87 a 24 bits; < 0x300 B) | 68 | 10.4% | 15,775 | 17.0% | 60 | sin medir |
| 5. De 0x300 B o más (una por lote) | 19 | 2.9% | 19,953 | 21.5% | 17 | 92B, 92C |

### Llamadas directas detectadas

Ordenadas por número de sitios de llamada desde código ya reemplazado (los más llamados ganan más por función) y, a igualdad, de mayor a menor tamaño. «Llamadores» son los primeros llamadores del código ya reemplazado (nombre del catálogo o dirección); «Lote» es el lote en curso o asignado que la reconstruye ([lotes-en-curso.csv](JuegoExe/lotes-en-curso.csv)); «—» significa sin lote asignado. Los lotes ya validados (90A, 90B, 92A–92C) dejaron de figurar porque estas funciones ya no están pendientes.

<details>
<summary>Llamadas directas del grupo A pendientes (495), de más a menos llamadas</summary>

| Dirección | Bytes | Módulo estimado | Categoría | Llamadas desde código reemplazado | Llamadores | Lote |
| --- | ---: | --- | ---: | ---: | --- | --- |
| `0x004AB250` | 5 | FxSystem_c (baja) | 1 | 59 | `0x5c0b70`, `FxEmitterBP_c::Load`, `FxManager_c::LoadFxProject` (+56 más) | — |
| `0x00749DC0` | 127 | Win (baja) | 3 | 19 | `0x6d3300`, `C2dEffect::DestroyAtomic`, `C2dEffect::t2dEffectPluginDestructor` (+16 más) | — |
| `0x004A8E30` | 59 | FxInfoManager_c (baja) | 2 | 14 | `FxEmitterBP_c::FreePrtFromPrim`, `FxEmitterBP_c::Update`, `FxManager_c::DestroyFxSystem` (+11 más) | — |
| `0x0074A310` | 377 | Win (baja) | 3 | 13 | `C3dMarkers::Shutdown`, `CAnimBlendAssocGroup::CreateAssociations-1`, `CAnimBlendAssocGroup::CreateAssociations-3` (+10 más) | — |
| `0x00733910` | 115 | CVisibilityPlugins (alta) | 4 | 13 | `CVisibilityPlugins::InsertAtomicIntoReallyDrawLastList`, `CVisibilityPlugins::InsertEntityIntoReallyDrawLastList`, `CVisibilityPlugins::InsertEntityIntoSortedList` (+10 más) | — |
| `0x00638290` | 725 | CTaskComplexSunbathe (baja) | 3 | 12 | `CTaskComplexSunbathe::CreateFirstSubTask`, `CTaskComplexSunbathe::CreateNextSubTask` (+9 más) | — |
| `0x00619440` | 5 | Rs (alta) | 1 | 12 | `App::DoRWStuffEndOfFrame`, `CLoadingScreen::DisplayNextSplash`, `CLoadingScreen::DisplayPCScreen` (+9 más) | — |
| `0x0074C800` | 595 | Win (baja) | 3 | 11 | `0x5dd100`, `CClothesBuilder::BlendGeometry-2`, `CClothesBuilder::BlendGeometry-3` (+8 más) | — |
| `0x005DEA00` | 90 | CAEAudioEntity (baja) | 3 | 11 | `0x604500`, `0x61cba0`, `0x6240b0` (+8 más) | — |
| `0x005F8510` | 77 | CPedGroupDefaultTaskAllocatorChat (baja) | 3 | 11 | `CTaskAllocatorKillOnFoot::AllocateTasks`, `CTaskAllocatorKillThreatsBasic::AllocateTasks`, `CTaskAllocatorKillThreatsBasicRandomGroup::AllocateTasks` (+8 más) | — |
| `0x004A8E70` | 54 | FxInfoManager_c (baja) | 2 | 11 | `BoneNodeManager_c::GetBoneNode`, `FxManager_c::GetParticle`, `IKChainManager_c::AddIKChain` (+8 más) | — |
| `0x0074DBC0` | 39 | Win (baja) | 2 | 11 | `CAutomobile::CustomCarPlate_AfterRenderingStop`, `CAutomobile::CustomCarPlate_BeforeRenderingStart`, `CClothesBuilder::CreateSkinnedClump` (+8 más) | — |
| `0x005EC660` | 217 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::BroadcastDeparture-` (+7 más) | — |
| `0x005EC740` | 217 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::DeRegisterPed-` (+7 más) | — |
| `0x005EB740` | 97 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::GetPedUsingEffect-` (+7 más) | — |
| `0x005EB7B0` | 95 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::GetRelevantAttractor-` (+7 más) | — |
| `0x005EF920` | 95 | CPedAttractorManager (baja) | 3 | 10 | `CPedAttractorManager::BroadcastArrival-` (+7 más) | — |
| `0x005EB690` | 93 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::IsPedRegisteredWithEffect-Effect` (+7 más) | — |
| `0x005EB640` | 75 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::IsPedRegisteredWithEffect-Ped` (+7 más) | — |
| `0x005EB5F0` | 67 | CPedAttractor (baja) | 3 | 10 | `CPedAttractorManager::RemoveEffect-` (+7 más) | — |
| `0x0044E480` | 62 | CPathFind (alta) | 4 | 10 | `0x683ad0`, `CEventDamage::ComputeDamageAnim`, `CEventDamage::ComputeDeathAnim` (+7 más) | — |
| `0x0074A490` | 37 | Win (baja) | 2 | 10 | `0x6d3830`, `C3dMarkers::Init`, `CVehicle::CopyObjectsCB` (+7 más) | — |
| `0x00407BE0` | 1 | CStreaming (alta) | 1 | 10 | `CCutsceneMgr::CreateCutsceneObject`, `CCutsceneMgr::LoadCutsceneData_postload`, `CCutsceneMgr::SetCutsceneAnim` (+7 más) | — |
| `0x00407BF0` | 1 | CStreaming (alta) | 1 | 10 | `CCutsceneMgr::CreateCutsceneObject`, `CCutsceneMgr::LoadCutsceneData_postload`, `CCutsceneMgr::SetCutsceneAnim` (+7 más) | — |
| `0x0061DE60` | 185 | CTaskSimpleFightingControl (baja) | 3 | 9 | `CTaskComplexFleeShooting::ControlSubTask`, `CTaskComplexGoToPointAiming::ControlSubTask`, `CTaskComplexGoToPointAiming::CreateFirstSubTask` (+6 más) | — |
| `0x0059AD20` | 77 | CMaths (baja) | 3 | 9 | `CCamera::CopyCameraMatrixToRWCam`, `CEntity::AttachToRwObject`, `CPhysical::AttachEntityToEntity-quat` (+6 más) | — |
| `0x00749B20` | 65 | Win (baja) | 3 | 9 | `C3dMarkers::DirectionArrowsDraw`, `C3dMarkers::User3dMarkersDraw`, `CEntity::Render` (+6 más) | — |
| `0x004F56D0` | 44 | CAEVehicleAudioEntity (alta) | 4 | 9 | `CAEVehicleAudioEntity::ProcessDummyStateTransition`, `CAEVehicleAudioEntity::ProcessPlayerVehicleEngine` (+6 más) | — |
| `0x004C6200` | 5 | CModelInfo (alta) | 2 | 9 | `CModelInfo::ShutDown` (+6 más) | — |
| `0x006682D0` | 661 | CTaskComplexGoToPointAndStandStill (alta) | 4 | 8 | `CTaskComplexGoToPointAndStandStill::CreateFirstSubTask`, `CTaskComplexGoToPointAndStandStill::CreateNextSubTask`, `CTaskComplexGoToPointAndStandStillTimed::CreateFirstSubTask` (+5 más) | — |
| `0x005EF980` | 659 | CPedAttractorManager (baja) | 3 | 8 | `CPedAttractorManager::RegisterPedWithAttractor` (+5 más) | — |
| `0x0074C7D0` | 44 | Win (baja) | 2 | 8 | `CClothesBuilder::BlendGeometry-2`, `CClothesBuilder::BlendGeometry-3`, `CClothesBuilder::CopyGeometry` (+5 más) | — |
| `0x006962A0` | 981 | CTaskComplexSeekEntityAnyMeansXYOffset (baja) | 5 | 7 | `CTaskComplexFollowLeaderInFormation::ControlSubTask`, `CTaskComplexFollowLeaderInFormation::CreateFirstSubTask`, `CTaskComplexFollowLeaderInFormation::CreateNextSubTask` (+4 más) | — |
| `0x00749E60` | 258 | Win (baja) | 3 | 7 | `C3dMarker::AddMarker`, `CAutomobile::SpawnFlyingComponent`, `CBoat::BlowUpCar` (+4 más) | — |
| `0x0074DA20` | 94 | Win (baja) | 3 | 7 | `0x734de0`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` (+4 más) | — |
| `0x005523C0` | 11 | CPtrNodeDoubleLinkPool (baja) | 1 | 7 | `CCover::Update`, `CEntity::AttachToRwObject`, `CEntity::CreateRwObject` (+4 más) | — |
| `0x0059EEB0` | 297 | CHandObject (baja) | 3 | 6 | `CTaskSimplePlayHandSignalAnim::StartAnim` (+3 más) | — |
| `0x00646FE0` | 116 | CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse (baja) | 3 | 6 | `CCarEnterExit::SetPedInCarDirect`, `CTaskComplexEnterCar::CreateSubTask`, `CTaskComplexEnterCarAsPassengerTimed::ControlSubTask` (+3 más) | — |
| `0x0070FA70` | 107 | CStencilShadows (alta) | 3 | 6 | `CStencilShadows::RenderForObject`, `CStencilShadows::RenderForVehicle` (+3 más) | — |
| `0x005F65A0` | 51 | CPedGroupDefaultTaskAllocatorSitInLeaderCar (baja) | 2 | 6 | `0x684380`, `CTaskComplexChat::CreateFirstSubTask`, `CTaskComplexPartnerGreet::GetPartnerSequence` (+3 más) | — |
| `0x005523D0` | 48 | CPtrNodeDoubleLinkPool (baja) | 2 | 6 | `CCover::Update`, `CEntity::Remove`, `CObject::RemoveFromControlCodeList` (+3 más) | — |
| `0x00541F40` | 38 | CPad (baja) | 4 | 6 | `CPhysical::ApplySpeed` (+3 más) | — |
| `0x0074A4C0` | 33 | Win (baja) | 2 | 6 | `0x6d3300`, `CFileLoader::FindRelatedModelInfoCB`, `CFileLoader::SetRelatedModelInfoCB` (+3 más) | — |
| `0x00734EA0` | 9 | RwHelper (baja) | 2 | 6 | `CMirrors::RenderMirrorBuffer`, `CPostEffects::HeatHazeFX`, `CShadowCamera::DrawOutlineBorder` (+3 más) | — |
| `0x0041BD50` | 7 | CBridge (baja) | 1 | 6 | `CAutomobile::ProcessCarWheelPair` (+3 más) | — |
| `0x00411BA0` | 4 | CPlaceable (baja) | 1 | 6 | `CAutomobile::ProcessCarWheelPair` (+3 más) | — |
| `0x0047E050` | 4 | CTheCarGenerators (baja) | 1 | 6 | `CAutomobile::ProcessCarWheelPair` (+3 más) | — |
| `0x0074B420` | 2,207 | Win (baja) | 5 | 5 | `CFileLoader::LoadAtomicFile-`, `CFileLoader::LoadAtomicFile-stream`, `CFileLoader::LoadClumpFile-` (+2 más) | — |
| `0x007491F0` | 318 | Win (baja) | 4 | 5 | `0x6c4e60`, `0x706520`, `CCutsceneMgr::CalculateBoundingSphereRadiusCB` (+2 más) | — |
| `0x0059BB60` | 74 | CCompressedMatrixNotAligned (baja) | 3 | 5 | `App::DoRWStuffStartOfFrame_Horizon`, `CBike::PreRender`, `CBmx::PreRender` (+2 más) | — |
| `0x004CFB10` | 70 | CAnimBlendHierarchy (baja) | 4 | 5 | `CAnimBlendNode::GetCurrentTranslationCompressed`, `CAnimBlendNode::GetEndTranslationCompressed`, `CAnimBlendNode::UpdateCompressed` (+2 más) | — |
| `0x00733F40` | 63 | CVisibilityPlugins (alta) | 2 | 5 | `CVisibilityPlugins::InitAlphaAtomicList`, `CVisibilityPlugins::InitAlphaEntityList` (+2 más) | — |
| `0x005A2CB0` | 59 | CObject (baja) | 2 | 5 | `CAutomobile::SpawnFlyingComponent`, `CObjectData::SetObjectData` (+2 más) | — |
| `0x004D6CC0` | 40 | CAEAmbienceTrackManager (alta) | 2 | 5 | `CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume`, `CAudioEngine::StopAmbienceTrack` (+2 más) | — |
| `0x004A8DD0` | 13 | FxInfoManager_c (baja) | 1 | 5 | `BoneNodeManager_c::Constructor`, `BoneNode_c::Constructor`, `FxManager_c::Constructor` (+2 más) | — |
| `0x004A8DE0` | 1 | FxInfoManager_c (baja) | 1 | 5 | `BoneNodeManager_c::Destructor`, `BoneNode_c::Destructor`, `FxManager_c::Destructor` (+2 más) | — |
| `0x005132D0` | 568 | CCam (alta) | 4 | 4 | `CCam::Process_Cam_TwoPlayer` (+1 más) | — |
| `0x00668D50` | 292 | CTaskComplexGoToPointAndStandStillAndAchieveHeading (alta) | 3 | 4 | `CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateFirstSubTask`, `CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateNextSubTask` (+1 más) | — |
| `0x0072D850` | 273 | CCullZones (alta) | 4 | 4 | `CCullZones::FindAttributesForCoors`, `CCullZones::FindMirrorAttributesForCoors`, `CCullZones::FindTunnelAttributesForCoors` (+1 más) | — |
| `0x00654840` | 257 | CTaskComplexWalkRoundBuildingAttempt (alta) | 3 | 4 | `CEventHandler::ComputeBuildingCollisionResponse` (+1 más) | — |
| `0x004654B0` | 226 | CTheScripts (baja) | 3 | 4 | `0x45ae80`, `0x468560`, `CTaskSimpleCreateCarAndGetIn::Destructor` (+1 más) | — |
| `0x00502F80` | 223 | CAEWaterCannonAudioEntity (alta) | 3 | 4 | `CAEWaterCannonAudioEntity::Service` (+1 más) | — |
| `0x00640430` | 131 | CTaskComplexOpenPassengerDoor (baja) | 3 | 4 | `0x625e70`, `CEventHandler::ComputeDraggedOutCarResponse`, `CTaskComplexArrestPed::CreateSubTask` (+1 más) | — |
| `0x0074A290` | 113 | Win (baja) | 3 | 4 | `C3dMarkers::Init`, `CFileLoader::LoadClumpFile-1` (+1 más) | — |
| `0x007469A0` | 109 | Win (baja) | 3 | 4 | `CMenuManager::CheckForMenuClosing`, `CPad::UpdateMouse` (+1 más) | — |
| `0x005F6190` | 108 | CPedGeometryAnalyser (baja) | 3 | 4 | `CGroupEventHandler::ComputeDoDealResponse`, `CGroupEventHandler::ComputeGreetResponse`, `CTaskComplexGangLeader::ScanForStuff` (+1 más) | — |
| `0x005D9110` | 104 | CCustomCarEnvMapPipeline (baja) | 3 | 4 | `CCustomBuildingDNPipeline::SetFxEnvTexture`, `CCustomCarEnvMapPipeline::SetFxEnvTexture`, `CCustomCarEnvMapPipeline::pluginEnvMatCopyConstructorCB` (+1 más) | — |
| `0x004D1610` | 101 | RpAnimBlend (alta) | 4 | 4 | `RpAnimBlend::FrameUpdateCallBackCompressedNonSkinned`, `RpAnimBlend::FrameUpdateCallBackCompressedSkinned`, `RpAnimBlend::FrameUpdateCallBackNonSkinned` (+1 más) | — |
| `0x00667310` | 99 | CTaskComplexProstituteSolicit (baja) | 4 | 4 | `CTaskComplexFollowNodeRoute::ComputePathNodes` (+1 más) | — |
| `0x004CFA90` | 91 | CAnimBlendHierarchy (baja) | 4 | 4 | `CAnimBlendNode::CalcDeltasCompressed`, `CAnimBlendNode::UpdateCompressed` (+1 más) | — |
| `0x005F6200` | 90 | CPedGeometryAnalyser (baja) | 3 | 4 | `CGroupEventHandler::ComputeDoDealResponse`, `CGroupEventHandler::ComputeGreetResponse`, `CTaskComplexGangLeader::ScanForStuff` (+1 más) | — |
| `0x004993D0` | 82 | CTaskComplexSeekEntityRadiusAngleOffset (baja) | 3 | 4 | `CSetPiece::Update`, `CTaskComplexBeCop::Clone` (+1 más) | — |
| `0x0074C6C0` | 82 | Win (baja) | 3 | 4 | `0x5dd100`, `CClothesBuilder::ConstructGeometryAndSkinArrays`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` (+1 más) | — |
| `0x00648330` | 81 | CTaskSimpleCarSlowBeDraggedOut (alta) | 3 | 4 | `CTaskSimpleCarSlowBeDraggedOut::MakeAbortable`, `CTaskSimpleCarSlowBeDraggedOut::ProcessPed`, `CTaskSimpleCarSlowBeDraggedOut::SetPedPosition` (+1 más) | — |
| `0x004DC0E0` | 70 | CAECutsceneTrackManager (alta) | 3 | 4 | `CAECutsceneTrackManager::PreloadBeatTrack`, `CAudioEngine::PreloadCutsceneTrack` (+1 más) | — |
| `0x006F5440` | 70 | cHandlingDataMgr (baja) | 4 | 4 | `CTrain::ProcessControl` (+1 más) | — |
| `0x00441130` | 54 | CEntryExitManager (baja) | 2 | 4 | `CObject::ProcessControl`, `CPlane::DoPlaneGenerationAndRemoval`, `ProcSurfaceInfo_c::AddObject` (+1 más) | — |
| `0x0053EDC0` | 23 | App (baja) | 1 | 4 | `CPad::CarGunJustDown`, `CPad::ExitVehicleJustDown`, `CPad::HornJustDown` (+1 más) | — |
| `0x005FE9C0` | 23 | CPedIK (baja) | 1 | 4 | `0x62b150`, `0x62b490`, `CTaskComplexWanderStandard::WillChat` (+1 más) | — |
| `0x00404090` | 11 | CBuildingPool (baja) | 1 | 4 | `CFileLoader::LoadObjectInstance-inst`, `Interior_c::PlaceFurniture`, `ProcSurfaceInfo_c::AddObject` (+1 más) | — |
| `0x00407A30` | 10 | CStreaming (alta) | 1 | 4 | `CCutsceneMgr::LoadCutsceneData_preload`, `CEntryExit::TransitionFinished` (+1 más) | — |
| `0x0045BA90` | 1,236 | CReplay (alta) | 5 | 3 | `CReplay::PlayBackThisFrameInterpolation` | — |
| `0x0045B730` | 857 | CReplay (alta) | 5 | 3 | `CReplay::RecordThisFrame` | — |
| `0x0045DEF0` | 763 | CReplay (alta) | 3 | 3 | `CReplay::RestorePlayerInfoVariables`, `CReplay::StorePlayerInfoVariables`, `CReplay::StoreStuffInMem` | — |
| `0x006A3BD0` | 646 | CAutomobile (alta) | 4 | 3 | `CAutomobile::NitrousControl` | — |
| `0x0074CA90` | 543 | Win (baja) | 3 | 3 | `0x5dd100`, `CClothesBuilder::ConstructGeometryAndSkinArrays`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x0060D5B0` | 465 | CPlayerPed (alta) | 4 | 3 | `CPlayerPed::SetupPlayerPed`, `CPools::LoadPedPool`, `CReplay::DealWithNewPedPacket` | — |
| `0x0072A640` | 335 | CWeather (alta) | 4 | 3 | `CWeather::SetWeatherToAppropriateTypeNow`, `CWeather::Update` | — |
| `0x00631990` | 218 | CTaskComplexObserveTrafficLightsAndAchieveHeading (alta) | 3 | 3 | `CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateFirstSubTask`, `CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateNextSubTask` | — |
| `0x0064CDA0` | 218 | CTaskSimpleCarFallOut (baja) | 3 | 3 | `CTaskComplexGoToBoatSteeringWheel::ControlSubTask`, `CTaskComplexGoToBoatSteeringWheel::CreateFirstSubTask`, `CTaskComplexGoToBoatSteeringWheel::CreateNextSubTask` | — |
| `0x0040C450` | 195 | CStreaming (alta) | 3 | 3 | `CStreaming::AddModelsToRequestList` | — |
| `0x00699E20` | 179 | CPedList (baja) | 3 | 3 | `CPedList::ExtractPedsWithGuns`, `CPedList::RemovePedsAttackingPedType`, `CPedList::RemovePedsThatDontListenToPlayer` | — |
| `0x0074CCC0` | 165 | Win (baja) | 3 | 3 | `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x00513220` | 164 | CCam (alta) | 3 | 3 | `CCam::Process_Cam_TwoPlayer` | — |
| `0x0063BA00` | 157 | CTaskComplexStealCar (baja) | 3 | 3 | `0x6419f0` | — |
| `0x006218C0` | 148 | CTaskSimpleGangDriveBy (baja) | 3 | 3 | `CCam::Process_Cam_TwoPlayer_InCarAndShooting`, `CTaskAllocatorKillThreatsDriveby::AllocateTasks` | — |
| `0x00711160` | 136 | CStencilShadows (baja) | 4 | 3 | `CStencilShadows::RegisterStencilShadows` | — |
| `0x00711280` | 134 | CStencilShadows (baja) | 3 | 3 | `CStencilShadows::RegisterStencilShadows` | — |
| `0x00719240` | 115 | CFont (alta) | 3 | 3 | `0x559230`, `0x55a780` | — |
| `0x00552520` | 114 | CPtrNodeDoubleLinkPool (baja) | 3 | 3 | `CColStore::Shutdown`, `CEntryExitManager::Shutdown`, `CIplStore::Shutdown` | — |
| `0x00541DD0` | 113 | CPad (baja) | 3 | 3 | `CGame::Process`, `CMenuManager::CheckMissionPackValidMenu`, `Game::FrontendIdle` | — |
| `0x005D9260` | 104 | CCustomCarEnvMapPipeline (baja) | 3 | 3 | `CCustomCarEnvMapPipeline::AllocEnvMapPipeAtomicData`, `CCustomCarEnvMapPipeline::CustomPipeRenderCB`, `CCustomCarEnvMapPipeline::pluginEnvAtmCopyConstructorCB` | — |
| `0x005A1D10` | 90 | CObjectPool (baja) | 3 | 3 | `0x5a4030`, `CAutomobile::SpawnFlyingComponent`, `CBoat::BlowUpCar` | — |
| `0x00632190` | 90 | CTaskComplexSunbathe (baja) | 4 | 3 | `CTaskComplexSunbathe::ControlSubTask`, `CTaskComplexSunbathe::CreateFirstSubTask`, `CTaskComplexSunbathe::CreateNextSubTask` | — |
| `0x00533610` | 81 | CEntity (alta) | 3 | 3 | `CEntity::Remove`, `CQuadTreeNode&lt;T&gt;::DeleteItem-`, `CQuadTreeNode&lt;T&gt;::DeleteItem-rect` | — |
| `0x0054EEA0` | 79 | Common (alta) | 3 | 3 | `CCamera::CalculateFrustumPlanes`, `CEntity::GetIsBoundingBoxOnScreen` | — |
| `0x004A9010` | 67 | FxInfoManager_c (baja) | 3 | 3 | `FurnitureSubGroup_c::GetFurniture`, `FurnitureSubGroup_c::GetRandomId`, `FxManager_c::FreeUpParticle` | — |
| `0x00408230` | 64 | CStreaming (baja) | 2 | 3 | `CStreaming::AddEntity` | — |
| `0x0053E8D0` | 63 | Game (alta) | 2 | 3 | `CMirrors::BeforeMainRender`, `CPostEffects::Render`, `Game::Idle` | — |
| `0x00552830` | 63 | CQuadTreeNode&lt;T&gt; (alta) | 2 | 3 | `CColStore::Initialise`, `CEntryExitManager::Init`, `CIplStore::Initialise` | — |
| `0x00552C90` | 49 | CQuadTreeNode&lt;T&gt; (alta) | 2 | 3 | `CColStore::Shutdown`, `CEntryExitManager::Shutdown`, `CIplStore::Shutdown` | — |
| `0x00459350` | 47 | CPed (baja) | 4 | 3 | `C3dMarker::Render`, `CVehicleRecording::InterpolateInfoForCar` | — |
| `0x005335E0` | 47 | CEntity (alta) | 2 | 3 | `CQuadTreeNode&lt;T&gt;::GetAll`, `CQuadTreeNode&lt;T&gt;::GetAllMatching-rect`, `CQuadTreeNode&lt;T&gt;::GetAllMatching-vec` | — |
| `0x0064E4B0` | 41 | CTaskComplexGoToBoatSteeringWheel (baja) | 3 | 3 | `CTaskUtilityLineUpPedWithCar::ProcessPed` | — |
| `0x0074BDA0` | 39 | Win (baja) | 2 | 3 | `CCustomCarEnvMapPipeline::RegisterPlugin`, `CVisibilityPlugins::PluginAttach`, `PipelinePlugin::PipelinePluginAttach` | — |
| `0x0074BDD0` | 39 | Win (baja) | 2 | 3 | `CCollisionPlugin::PluginAttach`, `CVisibilityPlugins::PluginAttach`, `RpAnimBlend::RpAnimBlendPluginAttach` | — |
| `0x0074CD70` | 39 | Win (baja) | 2 | 3 | `BreakablePlugin::BreakablePluginAttach`, `C2dEffect::PluginAttach`, `CCustomBuildingDNPipeline::ExtraVertColourPluginAttach` | — |
| `0x0074C690` | 35 | Win (baja) | 2 | 3 | `CClothesBuilder::ConstructGeometryAndSkinArrays`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x0074CDA0` | 34 | Win (baja) | 2 | 3 | `BreakablePlugin::BreakablePluginAttach`, `C2dEffect::PluginAttach`, `CCustomBuildingDNPipeline::ExtraVertColourPluginAttach` | — |
| `0x00403E00` | 32 | CEntity (baja) | 2 | 3 | `CFileLoader::LoadObjectInstance-inst`, `Interior_c::PlaceFurniture`, `ProcSurfaceInfo_c::AddObject` | — |
| `0x00572DB0` | 26 | CTheZones (baja) | 1 | 3 | `CMenuManager::DrawBackground` | — |
| `0x0043A6A0` | 23 | CCheat (baja) | 1 | 3 | `CPedToPlayerConversations::Update` | — |
| `0x004A8DB0` | 16 | FxInfoManager_c (baja) | 1 | 3 | `BoneNode_c::Constructor`, `FxSystemBP_c::Constructor`, `FxSystem_c::Constructor` | — |
| `0x00552C80` | 11 | CQuadTreeNode&lt;T&gt; (alta) | 1 | 3 | `CColStore::Initialise`, `CEntryExitManager::Init`, `CIplStore::Initialise` | — |
| `0x0074CCB0` | 9 | Win (baja) | 1 | 3 | `CClothesBuilder::BlendGeometry-2`, `CClothesBuilder::BlendGeometry-3`, `CClothesBuilder::CopyGeometry` | — |
| `0x004ADE90` | 7 | CEventDamage (baja) | 1 | 3 | `CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask`, `CTaskSimpleDie::ProcessPed`, `CTaskSimpleDrownInCar::ProcessPed` | — |
| `0x004D7990` | 7 | CAEAudioChannel (alta) | 1 | 3 | `CAEStreamThread::MainLoop`, `CAEStreamingChannel::Pause`, `CAEStreamingChannel::SetFrequencyScalingFactor` | — |
| `0x005CFE00` | 5 | FxEmitterBP_c (baja) | 1 | 3 | `JPegPlugin::JPegDecompressToRaster` | — |
| `0x0041BD80` | 4 | CBridge (baja) | 1 | 3 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x00404350` | 3 | CRect (baja) | 1 | 3 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x00541D70` | 1 | CPad (alta) | 1 | 3 | `CMenuManager::Initialise`, `CPlayerInfo::MakePlayerSafe`, `CStreaming::RetryLoadFile` | — |
| `0x00734640` | 1 | CVisibilityPlugins (baja) | 1 | 3 | `App::DoRWStuffEndOfFrame`, `Game::FrontendIdle`, `Game::Idle` | — |
| `0x00421A50` | 1,418 | CCarCtrl (alta) | 5 | 2 | `0x426350` | — |
| `0x006B0690` | 1,018 | CAutomobile (alta) | 5 | 2 | `0x6c5420`, `CPlane::PreRender` | — |
| `0x0059AA40` | 591 | Interior_c (baja) | 4 | 2 | `CPed::ShoulderBoneRotation` | — |
| `0x0059A840` | 510 | Interior_c (baja) | 4 | 2 | `CPed::ShoulderBoneRotation` | — |
| `0x0053E600` | 353 | Game (alta) | 4 | 2 | `Game::FrontendIdle`, `Game::Idle` | — |
| `0x0065ED40` | 329 | CTaskComplexGangFollower (alta) | 4 | 2 | `CTaskComplexAvoidOtherPedWhileWandering::ControlSubTask`, `CTaskComplexGangFollower::ControlSubTask` | — |
| `0x005DD220` | 323 | CPlantMgr (baja) | 3 | 2 | `CPlantMgr::Initialise` | — |
| `0x00664770` | 295 | CTaskGoToVehicleAndLean (baja) | 4 | 2 | `CTaskGoToVehicleAndLean::ControlSubTask`, `CTaskGoToVehicleAndLean::CreateFirstSubTask` | — |
| `0x0074C200` | 269 | Win (baja) | 4 | 2 | `CClothesBuilder::ConstructGeometryAndSkinArrays`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x005DA6A0` | 236 | CCustomCarEnvMapPipeline (baja) | 3 | 2 | `CCustomBuildingPipeline::CustomPipeRenderCB`, `CCustomCarEnvMapPipeline::CustomPipeRenderCB` | — |
| `0x005A1DF0` | 230 | CObjectPool (baja) | 3 | 2 | `CObject::Create-dummy` | — |
| `0x00452090` | 190 | CPathFind (alta) | 3 | 2 | `0x424210` | — |
| `0x0049ABD0` | 184 | CShopping (alta) | 3 | 2 | `CShopping::LoadStats` | — |
| `0x007111F0` | 136 | CStencilShadows (baja) | 4 | 2 | `CStencilShadows::RegisterStencilShadows` | — |
| `0x00695600` | 135 | CTaskComplexFollowPedFootsteps (baja) | 3 | 2 | `CTaskComplexFollowLeaderAnyMeans::CreateSubTask`, `CTaskComplexSeekEntityAnyMeansXYOffset::Clone` | — |
| `0x0074D990` | 131 | Win (baja) | 3 | 2 | `CClothesBuilder::CreateSkinnedClump`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x00749D40` | 124 | Win (baja) | 3 | 2 | `0x5dd100`, `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x0063BAE0` | 122 | CTaskComplexStealCar (baja) | 3 | 2 | `0x6419f0` | — |
| `0x004DEFE0` | 121 | CAEGlobalWeaponAudioEntity (alta) | 3 | 2 | `CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire` | — |
| `0x0057C5E0` | 118 | CMenuManager (alta) | 3 | 2 | `0x747eb0` | — |
| `0x00718E50` | 117 | CFont (alta) | 3 | 2 | `CFont::RenderFontBuffer` | — |
| `0x0059ADF0` | 114 | CMaths (baja) | 4 | 2 | `CReplay::PlayBackThisFrameInterpolation`, `CReplay::ProcessPedUpdate` | — |
| `0x005DBE40` | 104 | CPlantMgr (alta) | 4 | 2 | `CPlantMgr::_ColEntityCache_Update` | — |
| `0x00731B80` | 104 | CTxdStore (alta) | 3 | 2 | `CTxdStore::AddTxdSlot`, `CTxdStore::Initialise` | — |
| `0x0049C9E0` | 103 | CStuntJumpManager (alta) | 3 | 2 | `CStuntJumpManager::AddOne`, `CStuntJumpManager::Load` | — |
| `0x005EE200` | 102 | CPedAttractorManager (baja) | 3 | 2 | `CPedAttractor::BroadcastArrival`, `CPedAttractor::RegisterPed` | — |
| `0x005D93B0` | 101 | CCustomCarEnvMapPipeline (baja) | 3 | 2 | `CCustomCarEnvMapPipeline::pluginSpecMatCopyConstructorCB`, `CCustomCarEnvMapPipeline::pluginSpecMatStreamReadCB` | — |
| `0x005DE340` | 97 | CCopPed (baja) | 3 | 2 | `CPopulation::AddPed` | — |
| `0x005DA640` | 82 | CCustomCarEnvMapPipeline (baja) | 3 | 2 | `CCustomBuildingPipeline::CustomPipeRenderCB`, `CCustomCarEnvMapPipeline::CustomPipeRenderCB` | — |
| `0x00648E90` | 80 | CTaskSimpleCarFallOut (alta) | 3 | 2 | `CTaskSimpleCarFallOut::MakeAbortable`, `CTaskSimpleCarFallOut::ProcessPed` | — |
| `0x005336B0` | 79 | CEntity (alta) | 3 | 2 | `CEntity::DeleteRwObject`, `CEntity::DetachFromRwObject` | — |
| `0x005CD810` | 76 | FxEmitterBP_c (baja) | 3 | 2 | `JPegPlugin::JPegCompressScreen`, `JPegPlugin::JPegDecompressToRaster` | — |
| `0x0071DB30` | 76 | Common (alta) | 4 | 2 | `Common::DoesInfiniteLineTouchScreen` | — |
| `0x00572130` | 69 | CTheZones (alta) | 4 | 2 | `CMenuManager::PrintMap`, `CTheZones::SetCurrentZoneAsUnlocked` | — |
| `0x005EB6F0` | 68 | CPedAttractor (baja) | 3 | 2 | `CPedAttractorManager::HasEmptySlot`, `CPedAttractorManager::HasQueueTailArrivedAtSlot` | — |
| `0x004161F0` | 64 | CCollision (alta) | 2 | 2 | `CCollision::CalculateTrianglePlanes-colData` | — |
| `0x004D40C0` | 64 | CAnimManager (alta) | 2 | 2 | `CAnimManager::UncompressAnimation` | — |
| `0x0061A2E0` | 64 | CPlayerRelationshipRecorder (baja) | 2 | 2 | `CPedIntelligence::Destructor`, `CPedIntelligence::Process` | — |
| `0x0046FEE0` | 61 | CTaskComplexGoToPointAndStandStill (baja) | 2 | 2 | `CTaskComplexSeekEntityStandard::ControlSubTask` | — |
| `0x00496DC0` | 61 | CTaskComplexSeekEntityRadiusAngleOffset (baja) | 2 | 2 | `CTaskComplexSeekEntityRadiusAngleOffset::ControlSubTask` | — |
| `0x00665DC0` | 61 | CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt; (baja) | 2 | 2 | `CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;::ControlSubTask` | — |
| `0x00537510` | 55 | CFileLoader (alta) | 2 | 2 | `CFileLoader::LoadCollisionModelVer3`, `CFileLoader::LoadCollisionModelVer4` | — |
| `0x004A24D0` | 50 | FxEmitterBP_c (alta) | 2 | 2 | `FxSystemBP_c::Load` | — |
| `0x00674EA0` | 48 | CTaskComplexWanderCop (baja) | 2 | 2 | `CTaskInteriorBeInShop::ScanForStuff` | — |
| `0x0045AF40` | 47 | CVehicleRecording (baja) | 4 | 2 | `CReplay::ProcessPedUpdate` | — |
| `0x005A2E60` | 47 | CObjectData (baja) | 4 | 2 | `C3dMarker::Render`, `ProcSurfaceInfo_c::AddObject` | — |
| `0x005404F0` | 45 | CPad (alta) | 2 | 2 | `CCamera::CamControl` | — |
| `0x004E3350` | 43 | CAEPedAudioEntity (baja) | 2 | 2 | `CAEPedSpeechAudioEntity::StopCurrentSpeech` | — |
| `0x0074DBF0` | 39 | Win (baja) | 2 | 2 | `CCustomCarEnvMapPipeline::RegisterPlugin` | — |
| `0x004F4CE0` | 36 | CAEUserRadioTrackManager (baja) | 2 | 2 | `0x6e1a60`, `CAutomobile::ProcessAI` | — |
| `0x005A1EF0` | 36 | CObjectPool (baja) | 2 | 2 | `CPickup::GiveUsAPickUpObject`, `CPools::LoadObjectPool` | — |
| `0x00533020` | 35 | CEntity (alta) | 3 | 2 | `CPhysical::Add`, `CPhysical::RemoveAndAdd` | — |
| `0x0059ACF0` | 35 | CMaths (baja) | 2 | 2 | `CPed::PositionAttachedPed`, `CPed::SetPedPositionInCar` | — |
| `0x0074DC20` | 34 | Win (baja) | 2 | 2 | `CCustomCarEnvMapPipeline::RegisterPlugin` | — |
| `0x005329A0` | 30 | CBaseModelInfo (baja) | 1 | 2 | `CEntity::AttachToRwObject`, `CEntity::CreateRwObject` | — |
| `0x00454920` | 28 | CPathFind (baja) | 1 | 2 | `CPickup::Update` | — |
| `0x0054F1E0` | 26 | CMatrixLinkList (baja) | 1 | 2 | `CCamera::Constructor`, `CEntity::Constructor` | — |
| `0x006D0B10` | 25 | CAEVehicleAudioEntity (baja) | 1 | 2 | `CBike::Constructor`, `CQuadBike::Constructor` | — |
| `0x0057C400` | 24 | CMenuManager (alta) | 1 | 2 | `CMenuManager::CheckRedefineControlInput`, `CMenuManager::UserInput` | — |
| `0x0057C420` | 24 | CMenuManager (alta) | 1 | 2 | `CMenuManager::CheckRedefineControlInput`, `CMenuManager::UserInput` | — |
| `0x00699F00` | 24 | CPedList (baja) | 1 | 2 | `CCover::FindCoordinatesCoverPoint`, `CTaskAllocatorKillOnFoot::AllocateTasks` | — |
| `0x004D59E0` | 23 | CAnimManager (baja) | 1 | 2 | `CPad::GetAccelerateJustDown`, `CPad::WeaponJustDown` | — |
| `0x0053EE20` | 23 | App (baja) | 1 | 2 | `CPad::HornJustDown`, `CPad::WeaponJustDown` | — |
| `0x00608F00` | 20 | CPedType (baja) | 1 | 2 | `CTaskSimpleUseGun::ProcessPed`, `CTaskSimpleUseGun::StartAnim` | — |
| `0x00719360` | 19 | CFont (alta) | 1 | 2 | `CControllerConfigManager::GetControllerSettingTextKeyBoard` | — |
| `0x005D6E90` | 14 | CCustomBuildingDNPipeline (baja) | 1 | 2 | `CCustomBuildingRenderer::AtomicSetup`, `CCustomBuildingRenderer::IsCBPCPipelineAttached` | — |
| `0x004B2070` | 11 | CEventGunShotWhizzedBy (baja) | 1 | 2 | `CWeapon::FireInstantHit`, `CWeapon::FireSniper` | — |
| `0x0056F200` | 11 | CPlayerInfo (baja) | 1 | 2 | `CPlayerPedData::DeAllocateData`, `CPlayerPedData::Destructor` | — |
| `0x00411BF0` | 10 | CPlaceable (baja) | 1 | 2 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x0049E650` | 10 | CTrafficLights (baja) | 1 | 2 | `CPostEffects::HeatHazeFX`, `Game::RenderEffects` | — |
| `0x004A9F70` | 10 | FxPrimBP_c (baja) | 1 | 2 | `FxManager_c::Constructor`, `FxManager_c::ShouldCreate` | — |
| `0x00734EC0` | 9 | RwHelper (baja) | 2 | 2 | `CShadowCamera::MakeGradientRaster`, `CTheScripts::ScriptDebugCircle2D` | — |
| `0x004043E0` | 8 | CRect (baja) | 1 | 2 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x00448650` | 6 | CGarage (alta) | 1 | 2 | `CCamera::CamControl` | — |
| `0x00558F90` | 6 | CStats (alta) | 1 | 2 | `CAERadioTrackManager::Initialise`, `CAERadioTrackManager::Reset` | — |
| `0x00745A70` | 5 | Ps (baja) | 1 | 2 | `CMenuManager::ProcessPCMenuOptions` | — |
| `0x0041BD10` | 4 | CBridge (baja) | 1 | 2 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x00407AC0` | 1 | CStreaming (alta) | 1 | 2 | `CCutsceneMgr::LoadCutsceneData_overlay`, `CGameLogic::SortOutStreamingAndMemory` | — |
| `0x00407F20` | 1 | CStreaming (alta) | 1 | 2 | `CCutsceneMgr::DeleteCutsceneData_overlay`, `CGame::Init2` | — |
| `0x004A8DC0` | 1 | FxInfoManager_c (baja) | 1 | 2 | `BoneNode_c::Destructor`, `FxSystem_c::Destructor` | — |
| `0x0055EB90` | 1,814 | SurfaceInfos_c (alta) | 5 | 1 | `SurfaceInfos_c::Init` | — |
| `0x004D1680` | 971 | RpAnimBlend (alta) | 5 | 1 | `RpAnimBlend::FrameUpdateCallBackSkinned` | — |
| `0x0074F760` | 923 | Win (baja) | 5 | 1 | `CGame::InitialiseRenderWare` | — |
| `0x004D2450` | 910 | RpAnimBlend (alta) | 5 | 1 | `RpAnimBlend::FrameUpdateCallBackNonSkinned` | — |
| `0x004D27F0` | 910 | RpAnimBlend (alta) | 5 | 1 | `RpAnimBlend::FrameUpdateCallBackCompressedNonSkinned` | — |
| `0x004D1A50` | 850 | RpAnimBlend (alta) | 5 | 1 | `RpAnimBlend::FrameUpdateCallBackSkinned` | — |
| `0x004D2100` | 845 | RpAnimBlend (alta) | 5 | 1 | `RpAnimBlend::FrameUpdateCallBackNonSkinned` | — |
| `0x004D1DB0` | 843 | RpAnimBlend (alta) | 5 | 1 | `RpAnimBlend::FrameUpdateCallBackCompressedSkinned` | — |
| `0x00709CF0` | 837 | CShadows (alta) | 5 | 1 | `CShadows::CastShadowEntityXYZ` | — |
| `0x005DA790` | 804 | CCustomCarEnvMapPipeline (baja) | 5 | 1 | `CCustomCarEnvMapPipeline::CustomPipeRenderCB` | — |
| `0x00606580` | 776 | CPedIntelligencePool (baja) | 5 | 1 | `0x607e30` | — |
| `0x005D8590` | 723 | CCustomCarEnvMapPipeline (alta) | 4 | 1 | `CCustomCarEnvMapPipeline::CustomPipeRenderCB` | — |
| `0x004F4320` | 645 | CAEUserRadioTrackManager (alta) | 3 | 1 | `0x4f4620` | — |
| `0x005ED4B0` | 645 | CPedAttractorManager (baja) | 3 | 1 | `CPedAttractor::SetTaskForPed` | — |
| `0x0072F180` | 418 | CCullZones (baja) | 3 | 1 | `CMemoryMgr::InitScratchPad` | — |
| `0x00619910` | 410 | Rs (alta) | 4 | 1 | `0x619fa0` | — |
| `0x005E37C0` | 404 | CPed (alta) | 4 | 1 | `CPed::DoFootLanded` | — |
| `0x005E3630` | 400 | CPed (alta) | 4 | 1 | `CPed::DoFootLanded` | — |
| `0x0055F2B0` | 360 | SurfaceInfos_c (alta) | 3 | 1 | `SurfaceInfos_c::Init` | — |
| `0x0055D0E0` | 314 | CStats (baja) | 3 | 1 | `SurfaceInfos_c::Init` | — |
| `0x0074F610` | 288 | Win (baja) | 3 | 1 | `CGame::ShutdownRenderWare` | — |
| `0x005D2620` | 282 | CVehicleSaveStructure (baja) | 3 | 1 | `CGangWars::Save` | — |
| `0x00669FF0` | 265 | CTaskComplexWander (baja) | 4 | 1 | `CTaskComplexGangLeader::ControlSubTask` | — |
| `0x005C7590` | 264 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegCompressScreen` | — |
| `0x005CFAC0` | 260 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegCompressScreen` | — |
| `0x0044CC20` | 258 | COnscreenCounterEntry (baja) | 3 | 1 | `Game::Render2dStuff` | — |
| `0x004D6510` | 256 | RpAnimBlend (alta) | 3 | 1 | `RpAnimBlend::RpAnimBlendClumpInit` | — |
| `0x0074FDA0` | 237 | Win (baja) | 3 | 1 | `App::PluginAttach` | — |
| `0x005D2740` | 234 | CVehicleSaveStructure (baja) | 3 | 1 | `CGangWars::Load` | — |
| `0x005A2F80` | 229 | ProcSurfaceInfo_c (baja) | 4 | 1 | `ProcSurfaceInfo_c::AddObjects` | — |
| `0x006D5E20` | 227 | CVehicle (alta) | 3 | 1 | `CVehicle::Constructor` | — |
| `0x0045CDB0` | 226 | CReplay (alta) | 3 | 1 | `CReplay::PlayBackThisFrameInterpolation` | — |
| `0x00438D60` | 223 | CCheat (alta) | 4 | 1 | `CCheat::MoneyArmourHealthCheat` | — |
| `0x005CFD20` | 218 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegDecompressToRaster` | — |
| `0x0049B0E0` | 217 | CVehicle (baja) | 3 | 1 | `CShopping::Buy` | — |
| `0x0061DB30` | 217 | CTaskSimpleFight (baja) | 3 | 1 | `CWeaponInfo::LoadWeaponData` | — |
| `0x0066CA40` | 209 | CTaskComplexInvestigateDisturbance (baja) | 3 | 1 | `CTaskComplexInvestigateDisturbance::CreateNextSubTask` | — |
| `0x006563D0` | 208 | CTaskComplexHitPedWithCar (baja) | 4 | 1 | `CTaskSimpleKillPedWithCar::ProcessPed` | — |
| `0x005D84C0` | 207 | CCustomCarEnvMapPipeline (alta) | 4 | 1 | `CCustomCarEnvMapPipeline::CustomPipeRenderCB` | — |
| `0x007460A0` | 205 | Win (baja) | 3 | 1 | `Ps::psSelectDevice` | — |
| `0x00605300` | 204 | CDecisionMakerTypes (baja) | 4 | 1 | `CPedIntelligence::Constructor` | — |
| `0x005A42B0` | 203 | CClothesBuilder (alta) | 4 | 1 | `CClothesBuilder::CreateSkinnedClump` | — |
| `0x00749C50` | 201 | Win (baja) | 3 | 1 | `CCustomRoadsignMgr::CreateRoadsignAtomicA` | — |
| `0x005CF960` | 198 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegCompressScreen` | — |
| `0x005C2730` | 194 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegCompressScreen` | — |
| `0x0074E350` | 193 | Win (baja) | 3 | 1 | `CVehicleModelInfo::StoreAtomicUsedMaterialsCB` | — |
| `0x005D0110` | 187 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegDecompressToRaster` | — |
| `0x005D2560` | 186 | CVehicleSaveStructure (baja) | 3 | 1 | `CPlayerInfo::Load` | — |
| `0x00619780` | 181 | Rs (alta) | 4 | 1 | `0x619fa0` | — |
| `0x00744930` | 179 | Input (alta) | 3 | 1 | `Input::PadHandler` | — |
| `0x005B19D0` | 175 | CCutsceneMgr (baja) | 3 | 1 | `CCutsceneMgr::CreateCutsceneObject` | — |
| `0x005D24B0` | 172 | CVehicleSaveStructure (baja) | 3 | 1 | `CPlayerInfo::Save` | — |
| `0x00649160` | 172 | CTaskComplexGoToBoatSteeringWheel (baja) | 4 | 1 | `CTaskComplexGoToBoatSteeringWheel::ControlSubTask` | — |
| `0x0061BB10` | 167 | CTaskSimpleRunAnim (baja) | 3 | 1 | `CTaskSimpleRunNamedAnim::ProcessPed` | — |
| `0x00655290` | 161 | CTaskComplexWalkRoundObject (baja) | 3 | 1 | `0x6575f0` | — |
| `0x0045CD10` | 159 | CReplay (alta) | 3 | 1 | `CReplay::RecordThisFrame` | — |
| `0x00646F40` | 158 | CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse (baja) | 4 | 1 | `CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ProcessPed` | — |
| `0x005D30C0` | 154 | CTheZones (baja) | 3 | 1 | `CGarages::Load` | — |
| `0x004A11E0` | 153 | Fx_c (baja) | 3 | 1 | `CEntity::CreateEffects` | — |
| `0x005C8690` | 149 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegDecompressToRaster` | — |
| `0x005D3020` | 149 | CTheZones (baja) | 3 | 1 | `CGarages::Save` | — |
| `0x005EDF10` | 148 | CPedAttractorManager (baja) | 3 | 1 | `CPedAttractorManager::GetPedAttractorManager` | — |
| `0x00746ED0` | 148 | CPad (baja) | 3 | 1 | `CPad::UpdateMouse` | — |
| `0x005C87E0` | 143 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegDecompressToRaster` | — |
| `0x004F2B80` | 139 | CAETwinLoopSoundEntity (alta) | 3 | 1 | `CAEVehicleAudioEntity::Destructor` | — |
| `0x005EE270` | 138 | CPedAttractorManager (baja) | 3 | 1 | `CPedAttractor::SetTaskForPed` | — |
| `0x005EE370` | 136 | CPedAttractorManager (baja) | 3 | 1 | `CPedShelterAttractor::GetDisplacement` | — |
| `0x00748F70` | 134 | Win (baja) | 3 | 1 | `App::PluginAttach` | — |
| `0x00506BA0` | 128 | CAEWeatherAudioEntity (baja) | 3 | 1 | `CAudioEngine::Destructor` | — |
| `0x007448B0` | 123 | Input (alta) | 3 | 1 | `Input::PadHandler` | — |
| `0x004D66A0` | 118 | RpAnimBlend (alta) | 3 | 1 | `RpAnimBlend::RpAnimBlendClumpInit` | — |
| `0x005D8C50` | 118 | CCustomCarEnvMapPipeline (alta) | 4 | 1 | `CCustomCarEnvMapPipeline::pluginEnvMatStreamWriteCB` | — |
| `0x00422F80` | 117 | CCarCtrl (alta) | 4 | 1 | `0x423000` | — |
| `0x005C26B0` | 117 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegCompressScreen` | — |
| `0x0045BFF0` | 116 | CReplay (alta) | 4 | 1 | `CReplay::PlayBackThisFrameInterpolation` | — |
| `0x0045C0D0` | 116 | CReplay (alta) | 4 | 1 | `CReplay::PlayBackThisFrameInterpolation` | — |
| `0x00667720` | 116 | CTaskSimpleRunNamedAnim (baja) | 3 | 1 | `CTaskComplexFollowPointRoute::GetSubTaskType` | — |
| `0x00668250` | 116 | CTaskComplexGoToPointAndStandStill (alta) | 4 | 1 | `CTaskComplexGoToPointAndStandStill::ControlSubTask` | — |
| `0x004D7890` | 115 | CAEAmbienceTrackManager (baja) | 3 | 1 | `CAEStreamingChannel::Constructor` | — |
| `0x005BD420` | 113 | CPopulation (baja) | 3 | 1 | `cHandlingDataMgr::LoadHandlingData` | — |
| `0x005FFE60` | 113 | CEntityScanner (baja) | 3 | 1 | `CPedIntelligence::Process` | — |
| `0x005D01D0` | 112 | FxEmitterBP_c (baja) | 3 | 1 | `JPegPlugin::JPegDecompressToRaster` | — |
| `0x004ACE80` | 110 | CEventObjectCollision (baja) | 4 | 1 | `CEventHandler::ComputeObjectCollisionResponse` | — |
| `0x00506B20` | 108 | CAEWeatherAudioEntity (baja) | 3 | 1 | `CAudioEngine::Destructor` | — |
| `0x005D8BE0` | 108 | CCustomCarEnvMapPipeline (alta) | 4 | 1 | `CCustomCarEnvMapPipeline::pluginEnvMatStreamReadCB` | — |
| `0x00552B50` | 104 | CQuadTreeNode&lt;T&gt; (alta) | 3 | 1 | `CQuadTreeNode&lt;T&gt;::AddItem` | — |
| `0x00411030` | 103 | CColStore (alta) | 3 | 1 | `CColStore::AddColSlot` | — |
| `0x0043F7D0` | 103 | CEntryExitManager (alta) | 3 | 1 | `CEntryExitManager::AddOne` | — |
| `0x005EE300` | 102 | CPedAttractorManager (baja) | 3 | 1 | `CPedAttractor::SetTaskForPed` | — |
| `0x005EE400` | 102 | CPedAttractorManager (baja) | 3 | 1 | `CPedAttractorManager::RestoreStuffFromMem` | — |
| `0x004A4C20` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A4D00` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A4DC0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A4E90` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A4F50` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5010` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A50E0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A51B0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5270` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5450` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5550` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5650` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A57C0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5980` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5A10` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5B20` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5CB0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5EA0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5FA0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A60B0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6220` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6320` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6440` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6550` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6630` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A67D0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A68E0` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6A00` | 101 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004D1570` | 100 | CAnimBlendAssociation (baja) | 4 | 1 | `RpAnimBlend::RpAnimBlendClumpUpdateAnimations` | — |
| `0x004D6DF0` | 97 | CAEAmbienceTrackManager (alta) | 3 | 1 | `CAEAmbienceTrackManager::Service` | — |
| `0x005503F0` | 94 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x005504C0` | 94 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550640` | 94 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550B00` | 94 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550D70` | 94 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550E40` | 94 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x005B8340` | 94 | CStreaming (baja) | 3 | 1 | `CVehicleModelInfo::SetupCommonData` | — |
| `0x00550180` | 93 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x005D9300` | 93 | CCustomCarEnvMapPipeline (baja) | 3 | 1 | `CCustomCarEnvMapPipeline::CreatePipe` | — |
| `0x0062FB40` | 93 | CTaskSimpleDie (alta) | 3 | 1 | `CTaskSimpleDieInCar::Destructor` | — |
| `0x00550250` | 92 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550320` | 92 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x005507C0` | 92 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550BD0` | 92 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00552AF0` | 92 | CQuadTreeNode&lt;T&gt; (alta) | 3 | 1 | `CQuadTreeNode&lt;T&gt;::InitPool` | — |
| `0x005D9060` | 92 | CCustomCarEnvMapPipeline (baja) | 3 | 1 | `CCustomCarEnvMapPipeline::CreatePipe` | — |
| `0x005D91B0` | 92 | CCustomCarEnvMapPipeline (baja) | 3 | 1 | `CCustomCarEnvMapPipeline::CreatePipe` | — |
| `0x00731AD0` | 92 | CTxdStore (alta) | 3 | 1 | `CTxdStore::Initialise` | — |
| `0x00405900` | 91 | CIplStore (baja) | 3 | 1 | `CIplStore::Initialise` | — |
| `0x00410F80` | 91 | CColStore (alta) | 3 | 1 | `CColStore::Initialise` | — |
| `0x0043F720` | 91 | CEntryExitManager (alta) | 3 | 1 | `CEntryExitManager::Init` | — |
| `0x0049C8F0` | 91 | CStuntJumpManager (alta) | 3 | 1 | `CStuntJumpManager::Init` | — |
| `0x00550570` | 91 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x005506F0` | 91 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550890` | 91 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550960` | 91 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550A30` | 91 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00550CA0` | 91 | CPools (alta) | 3 | 1 | `CPools::Initialise` | — |
| `0x00642700` | 88 | CTaskComplexScreamInCarThenLeave (baja) | 3 | 1 | `CTaskSimpleCarDrive::ProcessPed` | — |
| `0x00460D80` | 87 | CTaskComplexWanderCop (baja) | 3 | 1 | `CTaskComplexBeCop::Destructor` | — |
| `0x00448660` | 86 | CGarage (alta) | 3 | 1 | `0x44aa50` | — |
| `0x0066C3E0` | 86 | CTaskComplexGotoDoorAndOpen (baja) | 3 | 1 | `CTaskComplexGoToAttractor::CreateFirstSubTask` | — |
| `0x0074E150` | 86 | Win (baja) | 3 | 1 | `CVehicleModelInfo::ReduceMaterialsInVehicle` | — |
| `0x0040FD50` | 85 | CColSphere (alta) | 3 | 1 | `CMonsterTruck::SetupSuspensionLines` | — |
| `0x004CFA30` | 83 | CAnimBlendHierarchy (baja) | 4 | 1 | `CAnimBlendNode::CalcDeltasCompressed` | — |
| `0x0045BF90` | 81 | CReplay (alta) | 4 | 1 | `CReplay::RecordThisFrame` | — |
| `0x0045C070` | 81 | CReplay (alta) | 4 | 1 | `CReplay::RecordThisFrame` | — |
| `0x004A5DA0` | 79 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A5E20` | 79 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A61A0` | 79 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x004A6760` | 79 | FxInfoManager_c (alta) | 3 | 1 | `FxInfoManager_c::AddFxInfo` | — |
| `0x005D8980` | 77 | CCustomCarEnvMapPipeline (alta) | 3 | 1 | `CCustomCarEnvMapPipeline::CreatePipe` | — |
| `0x005E0540` | 74 | CPed (baja) | 3 | 1 | `CPed::operator`, `new-poolIndexed` | — |
| `0x004D6450` | 73 | RpAnimBlend (alta) | 3 | 1 | `RpAnimBlend::RpAnimBlendClumpFillFrameArray` | — |
| `0x004DBF10` | 71 | CAECutsceneTrackManager (alta) | 3 | 1 | `CAECutsceneTrackManager::Service` | — |
| `0x004D6DA0` | 70 | CAEAmbienceTrackManager (alta) | 3 | 1 | `CAEAmbienceTrackManager::Service` | — |
| `0x004A1280` | 67 | Fx_c (baja) | 3 | 1 | `CEntity::DestroyEffects` | — |
| `0x0049EA90` | 66 | Fx_c (alta) | 3 | 1 | `CGame::Init1` | — |
| `0x004DBF60` | 66 | CAECutsceneTrackManager (alta) | 3 | 1 | `CAECutsceneTrackManager::Service` | — |
| `0x006198C0` | 65 | Rs (alta) | 3 | 1 | `0x619d60` | — |
| `0x004D7910` | 64 | CAEAmbienceTrackManager (baja) | 3 | 1 | `CAEStreamingChannel::Destructor` | — |
| `0x005EAB80` | 62 | CPedAttractorPool (baja) | 2 | 1 | `CPedAttractor::DeRegisterPed` | — |
| `0x004F0B10` | 60 | CAESoundManager (baja) | 2 | 1 | `CAEAudioHardware::Initialise` | — |
| `0x0045AFB0` | 54 | CVehicleRecording (baja) | 2 | 1 | `CAutomobile::ProcessHarvester` | — |
| `0x004650A0` | 54 | CTheScripts (baja) | 3 | 1 | `CDecisionMakerTypesFileLoader::LoadDecisionMaker-enum` | — |
| `0x0059F050` | 54 | CHandObject (baja) | 2 | 1 | `CObject::ProcessControl` | — |
| `0x005F7200` | 54 | CPedGroupDefaultTaskAllocatorSitInLeaderCar (baja) | 2 | 1 | `CPedGroupIntelligence::SetDefaultTaskAllocatorType` | — |
| `0x00669FB0` | 54 | CTaskComplexWander (baja) | 2 | 1 | `CTaskComplexGangLeader::ScanForStuff` | — |
| `0x004AB2D0` | 51 | FxSystem_c (baja) | 2 | 1 | `CAutomobile::ProcessHarvester` | — |
| `0x005EC220` | 49 | CPedAttractorManager (baja) | 2 | 1 | `CPedAttractor::BroadcastArrival` | — |
| `0x0062F2D0` | 49 | CTaskComplexLeaveCarAndDie (baja) | 2 | 1 | `CTaskComplexSunbathe::CanSunbathe` | — |
| `0x0049C9A0` | 48 | CStuntJumpManager (alta) | 2 | 1 | `CStuntJumpManager::ShutdownForRestart` | — |
| `0x005D9180` | 48 | CCustomCarEnvMapPipeline (baja) | 2 | 1 | `CCustomCarEnvMapPipeline::SetFxEnvTexture` | — |
| `0x00731650` | 46 | D3DResourceSystem (baja) | 2 | 1 | `App::PluginAttach` | — |
| `0x006955D0` | 44 | CTaskComplexFollowPedFootsteps (baja) | 4 | 1 | `CTaskComplexFollowLeaderInFormation::ControlSubTask` | — |
| `0x005E88E0` | 43 | CPed (alta) | 3 | 1 | `CCamera::CamControl` | — |
| `0x00667900` | 43 | CTaskSimpleRunNamedAnim (baja) | 2 | 1 | `CTaskComplexFollowNodeRoute::ComputePathNodes` | — |
| `0x006586A0` | 42 | CTaskComplexWalkRoundFire (baja) | 2 | 1 | `CTaskComplexUseSwatRope::CreateSubTask` | — |
| `0x004048E0` | 41 | CObjectPool (baja) | 2 | 1 | `0x587000` | — |
| `0x00465040` | 41 | CTheScripts (baja) | 2 | 1 | `0x587000` | — |
| `0x00552390` | 41 | CPtrNodeDoubleLinkPool (baja) | 2 | 1 | `CEntity::Remove` | — |
| `0x00465070` | 40 | CTheScripts (baja) | 2 | 1 | `CProjectileInfo::IsProjectileInRange` | — |
| `0x005521D0` | 37 | CPools (baja) | 2 | 1 | `CCover::Update` | — |
| `0x0054F490` | 36 | CPlaceable (alta) | 2 | 1 | `CEntity::Destructor` | — |
| `0x0065DE60` | 36 | CEventPassObject (baja) | 2 | 1 | `CTaskComplexGangFollower::CreateNextSubTask` | — |
| `0x00411A30` | 35 | CPlaceable (baja) | 4 | 1 | `CPedGeometryAnalyser::CanPedJumpObstacle-contacted` | — |
| `0x004A9CF0` | 35 | FxMemoryPool_c (baja) | 2 | 1 | `FxEmitterBP_c::Constructor` | — |
| `0x004F3B30` | 35 | CAEUserRadioTrackManager (alta) | 2 | 1 | `0x4f4620` | — |
| `0x005EB310` | 35 | CPedAttractor (alta) | 2 | 1 | `CPedAttractor::SetTaskForPed` | — |
| `0x00631910` | 34 | CTaskComplexObserveTrafficLights (baja) | 2 | 1 | `CTaskComplexWander::CreateSubTask` | — |
| `0x0074BE00` | 34 | Win (baja) | 2 | 1 | `PipelinePlugin::PipelinePluginAttach` | — |
| `0x0074BE70` | 34 | Win (baja) | 2 | 1 | `CCollisionPlugin::PluginAttach` | — |
| `0x0046AE00` | 33 | CTaskComplexSeekEntityStandard (baja) | 4 | 1 | `CAutomobile::SpawnFlyingComponent` | — |
| `0x00502BF0` | 32 | CAEWMADecoder (alta) | 2 | 1 | `CAEUserRadioTrackManager::Shutdown` | — |
| `0x00747000` | 32 | CPad (baja) | 2 | 1 | `0x747eb0` | — |
| `0x004082A0` | 31 | CStreaming (baja) | 1 | 1 | `CPedToPlayerConversations::Update` | — |
| `0x00710BA0` | 30 | CStencilShadows (alta) | 1 | 1 | `CStencilShadows::RegisterStencilShadows` | — |
| `0x0049C3B0` | 28 | CShopping (baja) | 1 | 1 | `CStuntJumpManager::Update` | — |
| `0x00407000` | 26 | CPopulation (baja) | 1 | 1 | `CRenderer::SetupEntityVisibility` | — |
| `0x004A1320` | 26 | Fx_c (baja) | 3 | 1 | `CGame::Shutdown` | — |
| `0x0053BC60` | 26 | CGame (alta) | 1 | 1 | `0x747eb0` | — |
| `0x0044CD30` | 25 | COnscreenCounterEntry (baja) | 1 | 1 | `CUserDisplay::Process` | — |
| `0x005D6830` | 25 | CCustomBuildingDNPipeline (alta) | 1 | 1 | `CCustomBuildingRenderer::Update` | — |
| `0x00572E70` | 24 | CTheZones (baja) | 1 | 1 | `CMenuManager::AdditionalOptionInput` | — |
| `0x0057C440` | 24 | CMenuManager (alta) | 1 | 1 | `CMenuManager::CheckRedefineControlInput` | — |
| `0x0057C460` | 24 | CMenuManager (alta) | 1 | 1 | `CMenuManager::CheckRedefineControlInput` | — |
| `0x004410E0` | 23 | CEntryExitManager (baja) | 1 | 1 | `CMenuManager::UserInput` | — |
| `0x004E7F20` | 23 | CAEMFDecoder (baja) | 1 | 1 | `CAERadioTrackManager::Service` | — |
| `0x0053EE60` | 23 | App (baja) | 1 | 1 | `CPlayerPed::ProcessControl` | — |
| `0x0053EF40` | 23 | App (baja) | 1 | 1 | `CPad::MeleeAttackJustDown` | — |
| `0x0053EF60` | 23 | App (baja) | 1 | 1 | `CPad::WeaponJustDown` | — |
| `0x00541E90` | 23 | CPad (baja) | 2 | 1 | `CPhysical::PositionAttachedEntity` | — |
| `0x0049EAE0` | 21 | Fx_c (alta) | 2 | 1 | `CGame::ShutDownForRestart` | — |
| `0x004A9F20` | 21 | FxPrimBP_c (baja) | 1 | 1 | `FxEmitterBP_c::CreateInstance` | — |
| `0x005DAAC0` | 21 | CCustomCarEnvMapPipeline (baja) | 2 | 1 | `CCustomCarEnvMapPipeline::CustomPipeRenderCB` | — |
| `0x004CEC00` | 20 | CAnimBlendAssociation (baja) | 2 | 1 | `CAnimBlendStaticAssociation::Destructor` | — |
| `0x0059ADD0` | 20 | CMaths (baja) | 1 | 1 | `CReplay::RecordThisFrame` | — |
| `0x00608F20` | 20 | CPedType (baja) | 1 | 1 | `CTaskSimpleUseGun::ProcessPed` | — |
| `0x0061C150` | 20 | CTaskSimpleRunNamedAnim (baja) | 1 | 1 | `CTaskSimpleUseGun::ProcessPed` | — |
| `0x004B4AA0` | 19 | CEventPotentialWalkIntoObject (baja) | 2 | 1 | `CEventHandler::ComputeKnockOffBikeResponse` | — |
| `0x0040B490` | 18 | CStreaming (alta) | 1 | 1 | `0x468560` | — |
| `0x00463640` | 18 | CTaskComplexSequence (baja) | 2 | 1 | `CRunningScript::ShutdownThisScript` | — |
| `0x004A1900` | 18 | FxEmitterBP_c (alta) | 2 | 1 | `FxSystemBP_c::Load` | — |
| `0x00537A10` | 18 | CFileLoader (baja) | 2 | 1 | `CFileLoader::LoadObjectInstance-inst` | — |
| `0x0059EA00` | 18 | BreakManager_c (baja) | 2 | 1 | `CFileLoader::LoadObjectInstance-inst` | — |
| `0x006D0AF0` | 18 | CAEVehicleAudioEntity (baja) | 1 | 1 | `0x6dfc50` | — |
| `0x00407A10` | 17 | CStreaming (alta) | 1 | 1 | `CCutsceneMgr::DeleteCutsceneData_overlay` | — |
| `0x004CEB60` | 16 | CAnimBlendAssociation (alta) | 1 | 1 | `RpAnimBlend::RpAnimBlendClumpUpdateAnimations` | — |
| `0x00406360` | 15 | CIplStore (baja) | 2 | 1 | `CStreaming::LoadCdDirectory-void` | — |
| `0x004DC2A0` | 15 | CAEDataStream (alta) | 2 | 1 | `CAEVorbisDecoder::CloseCallback` | — |
| `0x004AB980` | 14 | CEventGlobalGroup (alta) | 2 | 1 | `CPedIntelligence::Destructor` | — |
| `0x00507400` | 14 | CAudioEngine (alta) | 2 | 1 | `CGameLogic::RestorePlayerStuffDuringResurrection` | — |
| `0x00407D30` | 13 | CStreaming (alta) | 1 | 1 | `CCarCtrl::GenerateRandomCars` | — |
| `0x00407DC0` | 13 | CStreaming (alta) | 1 | 1 | `CCarCtrl::GenerateRandomCars` | — |
| `0x007316D0` | 13 | CTxdStore (alta) | 1 | 1 | `CClothesBuilder::RequestTexture` | — |
| `0x004F4CA0` | 12 | CAEUserRadioTrackManager (baja) | 1 | 1 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x004096C0` | 11 | CStreaming (alta) | 1 | 1 | `CRenderer::ConstructRenderList` | — |
| `0x004F11E0` | 11 | CAEStaticChannel (baja) | 2 | 1 | `CAEAudioHardware::Destructor` | — |
| `0x005326E0` | 11 | CDummyPool (baja) | 1 | 1 | `CFileLoader::LoadObjectInstance-inst` | — |
| `0x00552380` | 11 | CPtrNodeDoubleLinkPool (baja) | 1 | 1 | `CQuadTreeNode&lt;T&gt;::AddItem` | — |
| `0x00746070` | 11 | Win (baja) | 1 | 1 | `CCutsceneMgr::IsCutsceneSkipButtonBeingPressed` | — |
| `0x0049E640` | 10 | CTrafficLights (baja) | 1 | 1 | `CGame::Process` | — |
| `0x004A9F50` | 10 | FxPrimBP_c (baja) | 1 | 1 | `FxSystem_c::EnablePrim` | — |
| `0x004E7850` | 10 | CAEMFDecoder (alta) | 2 | 1 | `CAEUserRadioTrackManager::Shutdown` | — |
| `0x0053F910` | 10 | CPad (alta) | 1 | 1 | `0x468560` | — |
| `0x0041BCE0` | 7 | CBridge (baja) | 1 | 1 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x0041BCF0` | 7 | CBridge (baja) | 1 | 1 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x00404420` | 5 | CRect (baja) | 1 | 1 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x004AB240` | 5 | FxSystem_c (baja) | 1 | 1 | `FxManager_c::LoadFxProject` | — |
| `0x004AB260` | 5 | FxSystem_c (baja) | 1 | 1 | `FxManager_c::LoadFxProject` | — |
| `0x00541E50` | 5 | CPad (baja) | 1 | 1 | `CGenericGameStorage::GenericSave` | — |
| `0x005CFA30` | 5 | FxEmitterBP_c (baja) | 1 | 1 | `JPegPlugin::JPegCompressScreen` | — |
| `0x00410390` | 4 | CColSphere (baja) | 1 | 1 | `CStreaming::LoadCdDirectory-char` | — |
| `0x007059E0` | 4 | CRealTimeShadow (alta) | 1 | 1 | `CShadows::CastShadowEntityXYZ` | — |
| `0x0041BCB0` | 3 | CBridge (baja) | 1 | 1 | `CPlayerInfo::Process` | — |
| `0x0041BD00` | 3 | CBridge (baja) | 1 | 1 | `CAutomobile::ProcessCarWheelPair` | — |
| `0x0044D2C0` | 3 | CPathFind (alta) | 1 | 1 | `CFileLoader::LoadCarPathNode` | — |
| `0x0044D2D0` | 3 | CPathFind (alta) | 1 | 1 | `CFileLoader::LoadPedPathNode` | — |
| `0x0044D2E0` | 3 | CPathFind (alta) | 1 | 1 | `CFileLoader::LoadPedPathNode` | — |
| `0x0044D2F0` | 3 | CPathFind (alta) | 1 | 1 | `CFileLoader::LoadCarPathNode` | — |
| `0x004A9C10` | 3 | FxManager_c (baja) | 1 | 1 | `FxManager_c::Constructor` | — |
| `0x004F11D0` | 3 | CAEStaticChannel (baja) | 1 | 1 | `CAEAudioHardware::Constructor` | — |
| `0x0044D0E0` | 1 | CPathFind (alta) | 1 | 1 | `CGame::Init2` | — |
| `0x0044D2B0` | 1 | CPathFind (alta) | 1 | 1 | `CGame::Init1` | — |
| `0x004A9C20` | 1 | FxManager_c (baja) | 1 | 1 | `FxManager_c::Destructor` | — |
| `0x0053ED30` | 1 | App (baja) | 1 | 1 | `CGame::Init2` | — |
| `0x005531C0` | 1 | CQuadTreeNode&lt;T&gt; (baja) | 1 | 1 | `CGame::Init2` | — |
| `0x005531D0` | 1 | CQuadTreeNode&lt;T&gt; (baja) | 1 | 1 | `CGame::Shutdown` | — |
| `0x0060F8C0` | 1 | CPlayerPed (baja) | 1 | 1 | `Game::Render2dStuff` | — |
| `0x006D0B30` | 1 | CAEVehicleAudioEntity (baja) | 1 | 1 | `CBike::Destructor` | — |
| `0x0072F3B0` | 1 | CCullZones (baja) | 1 | 1 | `CGame::InitialiseOnceBeforeRW` | — |
| `0x00734610` | 1 | CVisibilityPlugins (baja) | 1 | 1 | `CGame::InitialiseRenderWare` | — |
| `0x00734620` | 1 | CVisibilityPlugins (baja) | 1 | 1 | `CGame::ShutdownRenderWare` | — |

</details>

<a id="parciales"></a>
## 11. 🟡 Parciales · 0 clases del catálogo + 353 clases/módulos con funciones del EXE pendientes

Orden de mayor a menor tamaño de las envolturas al EXE pendientes (suma de bytes de sus cuerpos; a igualdad, alfabético). «Función más grande» y «Tamaño» (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B) indican cuánto pesa la función más difícil de la clase. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas del juego al EXE», que cuenta solo código del juego; las de biblioteca no entran): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| *(ninguna clase parcial al 09/11/2026)* | | | | | | | | | |

### Clases con funciones del EXE aún sin reemplazar · 353 clases / módulos

Todas estas clases o módulos son parciales: conservan funciones del EXE en 0x401000–0x74FFFF sin reemplazar por C++ (grupos A–D del mapa del EXE y envolturas; 2,381 funciones). La clase se atribuye por cercanía de direcciones y puede ser errónea cuando su confianza es baja. Lista completa por función: [pendientes-por-clase.md](JuegoExe/pendientes-por-clase.md).

| Clase o módulo | Funciones pendientes | Bytes pendientes | Función más grande (B) | Tamaño |
| --- | ---: | ---: | ---: | --- |
| CEventPotentialWalkIntoVehicle | **4** | 20 | 6 | pequeña |
| CEventVehicleDied | **4** | 20 | 6 | pequeña |
| CPlayerInfo | **3** | 21 | 11 | pequeña |
| CTaskComplexDie | **3** | 23 | 11 | pequeña |
| CEventVehicleDamage | **4** | 23 | 7 | pequeña |
| CLodTimeModelInfo | **2** | 25 | 18 | pequeña |
| CEventGunShotWhizzedBy | **5** | 29 | 11 | pequeña |
| CLodAtomicModelInfo | **2** | 31 | 24 | pequeña |
| CPhysical | **2** | 32 | 27 | pequeña |
| CTrailer | **2** | 33 | 30 | pequeña |
| CAnimatedBuilding | **2** | 35 | 30 | pequeña |
| CBridge | **7** | 35 | 7 | pequeña |
| CTaskComplexArrestPed | **2** | 36 | 30 | pequeña |
| CTaskComplexCarDrive | **2** | 36 | 30 | pequeña |
| CTaskGangHassleVehicle | **2** | 36 | 30 | pequeña |
| CCustomBuildingDNPipeline | **2** | 39 | 25 | pequeña |
| CDirectory | **2** | 39 | 28 | pequeña |
| CPedType | **2** | 40 | 20 | pequeña |
| CTaskComplexObserveTrafficLights | **2** | 40 | 34 | pequeña |
| CQuadBike | **2** | 41 | 30 | pequeña |
| CTaskAllocatorKillThreatsBasicRandomGroup | **2** | 41 | 30 | pequeña |
| CTaskComplexAttractorPartnerWait | **2** | 41 | 30 | pequeña |
| CTaskComplexDriveToPoint | **2** | 41 | 30 | pequeña |
| CTaskComplexEnterAnyCarAsDriver | **2** | 41 | 30 | pequeña |
| CTaskComplexGoToPointAndStandStillTimed | **2** | 41 | 30 | pequeña |
| CTaskComplexUseEffectRunning | **2** | 41 | 30 | pequeña |
| CTaskComplexUseEffectSprinting | **2** | 41 | 30 | pequeña |
| CTaskComplexWanderMedic | **2** | 41 | 30 | pequeña |
| CTaskComplexWanderStandard | **2** | 41 | 30 | pequeña |
| CTaskSimpleFinishBrain | **2** | 41 | 30 | pequeña |
| CTaskSimpleTogglePedThreatScanner | **2** | 41 | 30 | pequeña |
| CTimeModelInfo | **2** | 41 | 34 | pequeña |
| ProcObjectMan_c | **2** | 41 | 30 | pequeña |
| CAEStaticChannel | **3** | 41 | 27 | pequeña |
| CTaskComplexReactToGunAimedAt | **2** | 42 | 30 | pequeña |
| CTaskSimpleWaitUntilLeaderAreaCodesMatch | **2** | 42 | 30 | pequeña |
| CEventDanger | **4** | 43 | 28 | pequeña |
| RwHelper | **3** | 45 | 27 | pequeña |
| C2dEffect | **3** | 46 | 21 | pequeña |
| CEventGunShot | **7** | 46 | 21 | pequeña |
| CEventVehicleDamageCollision | **3** | 47 | 30 | pequeña |
| CEventVehicleDamageWeapon | **3** | 47 | 30 | pequeña |
| CTaskComplexTreatAccident | **3** | 47 | 30 | pequeña |
| CText | **2** | 48 | 24 | pequeña |
| CAEAudioHardware | **2** | 54 | 27 | pequeña |
| CTheCarGenerators | **3** | 55 | 30 | pequeña |
| FxManager_c | **5** | 55 | 27 | pequeña |
| CEventAreaCodes | **4** | 56 | 42 | pequeña |
| CAEPedAudioEntity | **2** | 57 | 43 | pequeña |
| CTaskSimpleTired | **2** | 57 | 51 | pequeña |
| CVehicleAnimGroup | **2** | 58 | 39 | pequeña |
| CAudioEngine | **3** | 58 | 27 | pequeña |
| CAEWMADecoder | **2** | 59 | 32 | pequeña |
| CTaskComplexCarSlowBeDraggedOut | **2** | 60 | 30 | pequeña |
| CTaskComplexFleeAnyMeans | **2** | 60 | 30 | pequeña |
| CTaskComplexFollowPointRoute | **2** | 60 | 30 | pequeña |
| CTaskComplexHitResponse | **2** | 60 | 30 | pequeña |
| CTaskComplexKillPedOnFootArmed | **2** | 60 | 30 | pequeña |
| CTaskComplexOnFire | **2** | 60 | 30 | pequeña |
| CTaskComplexPassObject | **2** | 60 | 30 | pequeña |
| CTaskComplexUseAttractor | **2** | 60 | 30 | pequeña |
| CTaskComplexUsePairedAttractor | **2** | 60 | 30 | pequeña |
| CTaskComplexWanderCriminal | **2** | 60 | 30 | pequeña |
| CTaskInteriorBeInOffice | **2** | 60 | 30 | pequeña |
| CTaskSimpleCreateCarAndGetIn | **2** | 60 | 30 | pequeña |
| CTaskSimpleEvasiveStep | **2** | 60 | 30 | pequeña |
| CTaskSimpleGoToPoint | **2** | 60 | 30 | pequeña |
| CAEMFDecoder | **3** | 60 | 27 | pequeña |
| CRealTimeShadow | **3** | 60 | 36 | pequeña |
| CBuildingPool | **2** | 62 | 51 | pequeña |
| CDummyPool | **2** | 62 | 51 | pequeña |
| CNodeRoutePool | **2** | 62 | 51 | pequeña |
| CObjectData | **2** | 62 | 47 | pequeña |
| CEventGlobalGroup | **3** | 63 | 35 | pequeña |
| CPedShelterAttractor | **2** | 65 | 33 | pequeña |
| CTaskComplexGoToPointAiming | **3** | 65 | 30 | pequeña |
| CTaskComplexSequence | **4** | 65 | 30 | pequeña |
| CTaskComplexUseSequence | **3** | 66 | 30 | pequeña |
| FxSystem_c | **4** | 66 | 51 | pequeña |
| CTaskComplexDiveFromAttachedEntityAndGetUp | **3** | 67 | 30 | pequeña |
| CTaskSimpleCower | **4** | 70 | 30 | pequeña |
| FxPrimBP_c | **5** | 70 | 21 | pequeña |
| CPedGroupIntelligence | **2** | 71 | 42 | pequeña |
| CTaskComplexCarDriveWander | **3** | 71 | 30 | pequeña |
| CPedGroups | **2** | 72 | 42 | pequeña |
| CPedIK | **2** | 72 | 49 | pequeña |
| CCollision | **2** | 76 | 64 | pequeña |
| CTaskComplexInAirAndLand | **4** | 77 | 30 | pequeña |
| CTaskSimplePause | **4** | 77 | 30 | pequeña |
| Securom | **2** | 78 | 49 | pequeña |
| CTaskComplexDrivePointRoute | **3** | 78 | 30 | pequeña |
| CTaskComplexWaitForBus | **3** | 81 | 30 | pequeña |
| CEventBuildingCollision | **6** | 82 | 32 | pequeña |
| CBuilding | **2** | 83 | 78 | pequeña |
| CPedGroupDefaultTaskAllocatorChat | **2** | 86 | 77 | pequeña |
| CAEAudioChannel | **2** | 88 | 81 | pequeña |
| CAEStreamingChannel | **2** | 88 | 61 | pequeña |
| CBaseModelInfo | **3** | 88 | 30 | pequeña |
| CRect | **5** | 88 | 42 | pequeña |
| CTaskComplexPlayHandSignalAnim | **3** | 90 | 30 | pequeña |
| CTaskSimpleCarShuffle | **3** | 90 | 30 | pequeña |
| CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> | **2** | 91 | 61 | pequeña |
| CGarage | **2** | 92 | 86 | pequeña |
| CTaskSimpleGiveCPR | **2** | 99 | 93 | pequeña |
| CTaskSimpleHitHead | **2** | 99 | 93 | pequeña |
| CInformGroupEvent | **2** | 101 | 51 | pequeña |
| CTaskComplexPartnerShove | **4** | 101 | 30 | pequeña |
| CTaskSimpleFacial | **4** | 101 | 30 | pequeña |
| CPedGroupDefaultTaskAllocatorSitInLeaderCar | **2** | 105 | 54 | pequeña |
| CTaskSimplePutDownEntity | **2** | 106 | 100 | mediana |
| CInterestingEvents | **2** | 110 | 80 | pequeña |
| CPlayerRelationshipRecorder | **3** | 111 | 64 | pequeña |
| CTaskSimpleHandsUp | **4** | 113 | 48 | pequeña |
| CTaskSimpleInAir | **2** | 115 | 109 | mediana |
| App | **6** | 116 | 23 | pequeña |
| CEventPotentialWalkIntoObject | **6** | 116 | 77 | pequeña |
| CTaskComplexScreamInCarThenLeave | **2** | 118 | 88 | pequeña |
| CBmx | **3** | 118 | 85 | pequeña |
| CTaskComplexLeaveCarAndDie | **4** | 120 | 49 | pequeña |
| CEventStuckInAir | **5** | 120 | 54 | pequeña |
| CAnimBlendStaticAssociation | **2** | 121 | 94 | pequeña |
| CAEVehicleAudioEntity | **5** | 121 | 44 | pequeña |
| CTaskComplexPartnerChat | **2** | 123 | 93 | pequeña |
| CColSphere | **3** | 124 | 85 | pequeña |
| CEventPool | **2** | 126 | 77 | pequeña |
| CTaskComplexUseSwatRope | **2** | 126 | 96 | pequeña |
| CTaskGangHasslePed | **5** | 126 | 30 | pequeña |
| CTaskSimpleFall | **3** | 129 | 93 | pequeña |
| CEventHandler | **2** | 130 | 65 | pequeña |
| CEventObjectCollision | **5** | 130 | 110 | mediana |
| CCompressedMatrixNotAligned | **2** | 131 | 74 | pequeña |
| CAEGlobalWeaponAudioEntity | **2** | 135 | 121 | mediana |
| CTaskSimpleUseGun | **2** | 139 | 109 | mediana |
| CEventDontJoinPlayerGroup | **5** | 140 | 96 | pequeña |
| CTaskSimpleCarSlowBeDraggedOut | **3** | 141 | 81 | pequeña |
| CEventLeaderExitedCarAsDriver | **7** | 141 | 80 | pequeña |
| CTaskLeanOnVehicle | **3** | 142 | 106 | mediana |
| VideoPlayer | **2** | 145 | 110 | mediana |
| CTaskSimpleJump | **3** | 145 | 109 | mediana |
| CEventLeaderQuitEnteringCarAsDriver | **8** | 146 | 80 | pequeña |
| CTaskComplexFleePoint | **5** | 150 | 30 | pequeña |
| CTaskComplexSitDownThenIdleThenStandUp | **5** | 150 | 30 | pequeña |
| CTaskSimpleIKChain | **5** | 150 | 30 | pequeña |
| CEventPlayerCommandToGroupAttack | **6** | 151 | 89 | pequeña |
| CTaskSimpleClimb | **3** | 152 | 116 | mediana |
| CEventLeaderEntryExit | **9** | 153 | 88 | pequeña |
| CTaskSimpleUninterruptable | **9** | 154 | 80 | pequeña |
| CTaskSimpleGoToPointFine | **4** | 155 | 108 | mediana |
| CTaskSimpleCarJumpOut | **6** | 156 | 30 | pequeña |
| CFileLoader | **6** | 158 | 55 | pequeña |
| CTaskSimpleDie | **4** | 159 | 93 | pequeña |
| CTaskSimpleGetUp | **4** | 159 | 93 | pequeña |
| CTaskSimpleLand | **4** | 159 | 93 | pequeña |
| CIplStore | **3** | 160 | 91 | pequeña |
| CTaskSimplePlayerOnFoot | **3** | 160 | 100 | mediana |
| CTaskComplexGoToAttractor | **5** | 162 | 112 | mediana |
| BreakManager_c | **3** | 164 | 135 | mediana |
| CTaskComplexJump | **4** | 164 | 122 | mediana |
| CMatrixLinkList | **2** | 166 | 140 | mediana |
| CTaskComplexProstituteSolicit | **3** | 166 | 99 | pequeña |
| CAtomicModelInfo | **4** | 167 | 82 | pequeña |
| CEventPedCollisionWithPlayer | **4** | 167 | 120 | mediana |
| CEventPlayerCollisionWithPed | **4** | 167 | 120 | mediana |
| CBike | **22** | 168 | 30 | pequeña |
| CTaskSimpleStealthKill | **3** | 169 | 109 | mediana |
| CAEDataStream | **3** | 170 | 93 | pequeña |
| CAETwinLoopSoundEntity | **2** | 174 | 139 | mediana |
| CAEAudioEntity | **9** | 176 | 90 | pequeña |
| CAnimBlendAssociation | **4** | 177 | 100 | mediana |
| CTaskComplexEnterCar | **6** | 177 | 30 | pequeña |
| CLoadingScreen | **5** | 180 | 76 | pequeña |
| CTaskComplexSeekCoverUntilTargetDead | **4** | 184 | 106 | mediana |
| CTaskComplexFollowPedFootsteps | **3** | 185 | 135 | mediana |
| D3DResourceSystem | **3** | 188 | 87 | pequeña |
| FxMemoryPool_c | **2** | 194 | 159 | mediana |
| CEventHitByWaterCannon | **6** | 194 | 97 | pequeña |
| CTaskSimpleRunAnim | **2** | 197 | 167 | mediana |
| CTaskComplexSeekEntityStandard | **4** | 197 | 104 | mediana |
| CEventPassObject | **9** | 201 | 102 | mediana |
| CTaskComplexDriveFireTruck | **7** | 202 | 106 | mediana |
| CPedList | **2** | 203 | 179 | mediana |
| CCutsceneMgr | **3** | 204 | 175 | mediana |
| IKChain_c | **2** | 205 | 103 | mediana |
| CVisibilityPlugins | **7** | 205 | 115 | mediana |
| CDamageAtomicModelInfo | **5** | 210 | 103 | mediana |
| WaterCreature_c | **2** | 211 | 106 | mediana |
| CMenuManager | **5** | 214 | 118 | mediana |
| CTaskSimpleFightingControl | **2** | 215 | 185 | mediana |
| CWaterLevel | **2** | 219 | 208 | mediana |
| CAECutsceneTrackManager | **4** | 219 | 71 | pequeña |
| CAESoundManager | **3** | 220 | 127 | mediana |
| CTaskSimpleFight | **2** | 223 | 217 | mediana |
| CTaskComplexWanderProstitute | **3** | 223 | 182 | mediana |
| CPtrNodeDoubleLinkPool | **5** | 225 | 114 | mediana |
| CTaskComplexObserveTrafficLightsAndAchieveHeading | **2** | 229 | 218 | mediana |
| CEventNewGangMember | **6** | 230 | 96 | pequeña |
| CEventLeanOnVehicle | **6** | 234 | 100 | mediana |
| CTaskSimpleRunNamedAnim | **6** | 235 | 116 | mediana |
| CTaskSimpleWaitForBus | **6** | 239 | 113 | mediana |
| CTaskSimpleStandStill | **6** | 241 | 99 | pequeña |
| CAnimBlendHierarchy | **3** | 244 | 91 | pequeña |
| CEventLeaderEnteredCarAsDriver | **9** | 244 | 96 | pequeña |
| CCheat | **2** | 246 | 223 | mediana |
| CMaths | **4** | 246 | 114 | mediana |
| CTaskComplexGoToBoatSteeringWheel | **4** | 249 | 172 | mediana |
| CFont | **3** | 251 | 117 | mediana |
| CTaskComplexCopInCar | **4** | 252 | 96 | pequeña |
| CTaskSimpleGangDriveBy | **2** | 254 | 148 | mediana |
| CClumpModelInfo | **4** | 255 | 189 | mediana |
| Common | **3** | 260 | 105 | mediana |
| CAEWeatherAudioEntity | **4** | 264 | 128 | mediana |
| CTaskComplexWalkRoundObject | **3** | 273 | 161 | mediana |
| CAudioZones | **4** | 275 | 139 | mediana |
| CDecisionMakerTypes | **2** | 278 | 204 | mediana |
| CEvent | **8** | 280 | 87 | pequeña |
| CTask | **6** | 289 | 159 | mediana |
| CStuntJumpManager | **4** | 296 | 103 | mediana |
| cHandlingDataMgr | **6** | 299 | 85 | pequeña |
| Input | **2** | 302 | 179 | mediana |
| CTaskSimpleHoldEntity | **8** | 302 | 166 | mediana |
| CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse | **3** | 304 | 158 | mediana |
| CTaskInteriorSitInChair | **4** | 304 | 106 | mediana |
| FurnitureManager_c | **3** | 306 | 117 | mediana |
| CAEWaterCannonAudioEntity | **2** | 309 | 223 | mediana |
| CTaskSimpleSwim | **4** | 315 | 162 | mediana |
| CPedDamageResponseCalculator | **3** | 319 | 197 | mediana |
| CTaskComplexHitPedWithCar | **4** | 319 | 208 | mediana |
| CStats | **2** | 320 | 314 | mediana |
| CTaskComplexOpenPassengerDoor | **4** | 327 | 131 | mediana |
| CTaskGoToVehicleAndLean | **3** | 331 | 295 | mediana |
| CCopPed | **8** | 335 | 108 | mediana |
| CPedGeometryAnalyser | **4** | 338 | 110 | mediana |
| InteriorManager_c | **2** | 340 | 189 | mediana |
| CQuaternion | **3** | 341 | 307 | mediana |
| CEventAcquaintancePedHateBadlyLit | **8** | 342 | 104 | mediana |
| CTaskComplexDieInCar | **14** | 344 | 96 | pequeña |
| CPlane | **3** | 346 | 231 | mediana |
| CWeather | **2** | 352 | 335 | mediana |
| CTaskAllocatorPool | **7** | 362 | 59 | pequeña |
| CTaskComplexGangFollower | **3** | 365 | 329 | mediana |
| CEventPlayerCommandToGroupGather | **8** | 372 | 217 | mediana |
| CColStore | **5** | 373 | 103 | mediana |
| CTaskComplexGoToPointAndStandStillAndAchieveHeading | **2** | 382 | 292 | mediana |
| VideoMode | **2** | 382 | 252 | mediana |
| CTaskComplexWalkRoundFire | **9** | 385 | 111 | mediana |
| CHandObject | **4** | 386 | 297 | mediana |
| CAEAmbienceTrackManager | **5** | 386 | 115 | mediana |
| CTaskSimpleCarFallOut | **5** | 388 | 218 | mediana |
| CTaskComplexGotoDoorAndOpen | **6** | 388 | 151 | mediana |
| CTxdStore | **6** | 388 | 104 | mediana |
| CEventKnockOffBike | **7** | 395 | 232 | mediana |
| CPopulation | **8** | 399 | 113 | mediana |
| CTaskSimpleDieInCar | **5** | 403 | 152 | mediana |
| CVehicleRecording | **3** | 417 | 316 | mediana |
| CPedIntelligence | **2** | 419 | 326 | mediana |
| CTaskComplexWalkRoundBuildingAttempt | **4** | 421 | 257 | mediana |
| CTheZones | **5** | 422 | 154 | mediana |
| CGame | **9** | 428 | 75 | pequeña |
| CTaskSimpleHitFromFront | **21** | 430 | 30 | pequeña |
| CAnimManager | **6** | 434 | 249 | mediana |
| Fx_c | **9** | 434 | 153 | mediana |
| CEventPotentialWalkIntoFire | **9** | 440 | 76 | pequeña |
| CTaskComplexWanderCop | **9** | 442 | 122 | mediana |
| CObjectPool | **5** | 446 | 230 | mediana |
| COnscreenCounterEntry | **3** | 450 | 258 | mediana |
| CTaskComplexSeekEntityAiming | **6** | 451 | 152 | mediana |
| CTaskComplexPartnerGreet | **9** | 458 | 154 | mediana |
| Game | **3** | 460 | 353 | mediana |
| CShopping | **5** | 460 | 184 | mediana |
| CTaskComplexStealCar | **6** | 461 | 157 | mediana |
| CPlantMgr | **3** | 484 | 323 | mediana |
| CTaskSimpleDuckToggle | **3** | 488 | 178 | mediana |
| ProcSurfaceInfo_c | **4** | 489 | 229 | mediana |
| CEventAttractor | **8** | 490 | 154 | mediana |
| CPlayerPed | **3** | 496 | 465 | mediana |
| CTaskSimpleDuck | **8** | 496 | 167 | mediana |
| CTaskSimpleIKPointArm | **6** | 516 | 176 | mediana |
| CEventScriptedAttractor | **13** | 527 | 93 | pequeña |
| CPedAttractorPool | **12** | 530 | 62 | pequeña |
| CStreaming | **17** | 535 | 195 | mediana |
| CStencilShadows | **5** | 543 | 136 | mediana |
| CTaskComplexRoadRage | **9** | 585 | 194 | mediana |
| CDecisionMakerTypesFileLoader | **1** | 606 | 606 | grande |
| CEventEditableResponse | **14** | 607 | 88 | pequeña |
| Rs | **4** | 661 | 410 | mediana |
| CEntryExitManager | **11** | 675 | 112 | mediana |
| CTaskComplexFallAndGetUp | **13** | 719 | 167 | mediana |
| CQuadTreeNode<T> | **10** | 720 | 206 | mediana |
| CObject | **7** | 728 | 480 | mediana |
| CEventScriptCommand | **11** | 766 | 77 | pequeña |
| CTaskSimpleThrowProjectile | **11** | 767 | 429 | mediana |
| CTaskComplexUseWaterCannon | **15** | 769 | 180 | mediana |
| CTrafficLights | **5** | 770 | 493 | mediana |
| CTaskComplexWaitForPed | **6** | 772 | 528 | grande |
| CTaskComplexWander | **4** | 797 | 448 | mediana |
| CModelInfo | **22** | 814 | 92 | pequeña |
| CShadows | **1** | 837 | 837 | grande |
| CVehicle | **33** | 861 | 227 | mediana |
| CVehicleSaveStructure | **4** | 874 | 282 | mediana |
| Ps | **6** | 876 | 327 | mediana |
| CPad | **11** | 895 | 322 | mediana |
| CEventSeenCop | **35** | 902 | 88 | pequeña |
| CTaskComplexSunbathe | **6** | 935 | 725 | grande |
| CTaskComplexMedicTreatInjuredPed | **13** | 974 | 259 | mediana |
| CTaskComplexGoToPointAndStandStill | **8** | 977 | 661 | grande |
| CTaskSimpleBeHit | **8** | 1,022 | 451 | mediana |
| CEventVehicleHitAndRun | **15** | 1,037 | 272 | mediana |
| CCam | **7** | 1,059 | 568 | grande |
| CTaskComplexStuckInAir | **11** | 1,075 | 141 | mediana |
| CTrain | **6** | 1,076 | 719 | grande |
| CPed | **7** | 1,093 | 404 | mediana |
| CTaskComplexInvestigateDisturbance | **9** | 1,125 | 348 | mediana |
| CTaskComplexSeekEntityAnyMeansXYOffset | **4** | 1,138 | 981 | grande |
| CEventVehicleOnFire | **17** | 1,145 | 111 | mediana |
| CEventVehicleCollision | **15** | 1,149 | 126 | mediana |
| CPlaceable | **25** | 1,157 | 77 | pequeña |
| CTaskSimpleRunTimedAnim | **12** | 1,175 | 226 | mediana |
| Interior_c | **4** | 1,200 | 591 | grande |
| CAttractorScanner | **3** | 1,273 | 480 | mediana |
| CClothesBuilder | **13** | 1,285 | 339 | mediana |
| CVehicleModelInfo | **15** | 1,356 | 343 | mediana |
| CTaskComplexAvoidOtherPedWhileWandering | **10** | 1,382 | 455 | mediana |
| CEntityScanner | **7** | 1,435 | 422 | mediana |
| CTaskInteriorUseInfo | **3** | 1,483 | 1,423 | grande |
| CPedIntelligencePool | **6** | 1,552 | 776 | grande |
| CEventDamage | **22** | 1,596 | 76 | pequeña |
| CTaskComplexWanderGang | **6** | 1,750 | 1,541 | grande |
| CPedAttractor | **28** | 1,774 | 217 | mediana |
| CTaskSimpleThrowControl | **14** | 1,802 | 836 | grande |
| CTaskComplexUseClosestFreeScriptedAttractor | **13** | 1,858 | 425 | mediana |
| CCarCtrl | **4** | 1,928 | 1,418 | grande |
| CPathFind | **14** | 2,141 | 1,077 | grande |
| SurfaceInfos_c | **2** | 2,174 | 1,814 | grande |
| CCullZones | **13** | 2,476 | 455 | mediana |
| CAEUserRadioTrackManager | **30** | 2,533 | 645 | grande |
| CTaskComplexTurnToFaceEntityOrCoord | **14** | 2,607 | 968 | grande |
| CTaskComplexGoToPointAnyMeans | **6** | 2,633 | 1,628 | grande |
| CCustomCarEnvMapPipeline | **25** | 3,547 | 804 | grande |
| CPools | **52** | 3,733 | 94 | pequeña |
| CAutomobile | **11** | 3,922 | 1,018 | grande |
| CHeli | **6** | 4,141 | 2,352 | muy grande |
| CReplay | **10** | 4,172 | 1,236 | grande |
| CRunningScript | **4** | 5,919 | 5,816 | muy grande |
| RpAnimBlend | **16** | 6,102 | 971 | grande |
| CScriptsForBrains | **4** | 6,307 | 6,028 | muy grande |
| FxInfoManager_c | **116** | 6,591 | 101 | mediana |
| CPedAttractorManager | **71** | 8,097 | 739 | grande |
| CTaskComplexSeekEntityRadiusAngleOffset | **5** | 8,771 | 8,462 | muy grande |
| CTheScripts | **14** | 9,864 | 8,957 | muy grande |
| CEntity | **43** | 10,300 | 3,282 | muy grande |
| CWeaponModelInfo | **41** | 13,554 | 1,530 | grande |
| Win | **85** | 19,907 | 2,207 | muy grande |
| FxEmitterBP_c | **141** | 28,395 | 801 | grande |

<a id="sin-revertir"></a>
## 12. 🔴 Sin revertir · 0 clases

Orden de mayor a menor tamaño de las envolturas pendientes. Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |

<a id="metodo"></a>
## 13. Cómo leer y actualizar

- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.
- **Validated:** `1` indica evidencia aprobada vinculada para esa entrada. No implica vigencia actual ni independencia del EXE.
- **Locked:** indicador de bloqueo del hook; hay 0 entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.
- **Porcentajes:** cada porcentaje del informe indica la dimensión de la que habla (funciones, bytes, clases, entradas del catálogo o validación); las categorías cuentan clases y el avance de cada clase cuenta funciones. Se muestran dos decimales.
- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.

- **Acceso al código:** pulsa el nombre para abrir el archivo que registra sus hooks. Si el registro está repartido, los enlaces adicionales abren los demás archivos. Algunas clases registran sus hooks en una cabecera.
- **Comentarios de estado:** resumen por clase/grupo, con nombres de pendientes y bloqueadas. Los conteos abarcan la clase completa aunque su implementación esté repartida entre archivos.

Sincronizar primero `validated` desde el registro de evidencia:

```powershell
python contrib/sync_validated_flags.py
```

Actualizar los flags de reconstrucción solo después de comparar con el original dentro del motor y regenerar desde la raíz:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

Para sincronizar también los comentarios de los archivos fuente:

```powershell
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md --update-source-status
python contrib/validation_inventory.py
```

Los comentarios se generan desde el CSV y la evidencia vinculada; no modifican flags ni acreditan validación. La vigencia se consulta en el [inventario](ValidationInventory.md). Añadir comentarios cambia las huellas de los archivos: el auditor conserva la evidencia histórica y exige revisar su vigencia, sin declarar fallidas las pruebas ni actualizar sus hashes de referencia.

El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.
