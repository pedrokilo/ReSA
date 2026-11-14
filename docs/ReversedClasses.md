# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-11-14 20:56 UTC

**Índice:** [1. Objetivo y dimensiones](#objetivo) · [2. Siguientes mediciones](#mediciones) · [3. Siguiente fase](#siguiente-fase) · [4. Vista general](#vista-general) · [5. Cobertura real del EXE](#cobertura-exe) · [6. Lo que falta del código del juego](#que-falta) · [7. Completas](#completas) · [8. Parciales](#parciales) · [9. Sin revertir](#sin-revertir) · [10. Cómo leer y actualizar](#metodo)

<a id="objetivo"></a>

## 1. Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. **La meta de este informe es completar todas las funciones del juego** (11,779 funciones y 3.01 MB de código en el EXE: el código propio del juego y su capa de plataforma). RenderWare, Direct3D y el CRT son bibliotecas enlazadas dentro del EXE: no forman parte de la meta, no se cuentan en las tablas y solo aparecen en la [cobertura real del EXE](#cobertura-exe) y en la lista de [bibliotecas fuera del objetivo](#bibliotecas) para aclarar qué queda fuera. Completar el juego es un hito hacia **ReSA — Zero Original Function Dependency**.

Cifras medidas el 14/11/2026. **Cada porcentaje de este informe lleva escrita la dimensión de la que habla**: *funciones* (del EXE), *bytes* (de código del EXE), *clases*, *entradas del catálogo* (filas de `hooks.csv`) o *validación* (cuántas de las ya hechas tienen evidencia aprobada). Dimensiones distintas dan porcentajes distintos y no son comparables entre sí.

| Tipo | Dimensión | Qué cuenta | Numerador | Denominador | Porcentaje (de qué) | Progreso |
| --- | --- | --- | ---: | ---: | --- | --- |
| **Funciones** | Funciones del juego reemplazadas por C++ en el EXE (**meta del proyecto**) | Funciones del EXE en 0x401000–0x74FFFF que ya tienen su hook reconstruido (análisis estático del EXE) | 10,469 | 11,779 | 88.88% de las funciones del juego | `██████████████████░░` |
| **Funciones** | Funciones del juego que faltan | Complemento de la fila anterior: grupos A–D de [lo que falta](#que-falta) | 1,310 | 11,779 | 11.12% de las funciones del juego | `██░░░░░░░░░░░░░░░░░░` |
| **Bytes** | Bytes de código del juego reemplazados | Suma de los cuerpos de esas mismas funciones, en bytes de código | 2,798,917 | 3,005,739 | 93.12% de los bytes de código del juego | `███████████████████░` |
| **Clases** | Clases del juego completas | Clases o grupos del catálogo con todas sus entradas reconstruidas y sin envolturas del juego abiertas | 894 | 894 | 100.00% de las clases del juego | `████████████████████` |
| **Validación** | Clases del juego validadas | Clases completas cuyas entradas tienen todas `validated=1` | 894 | 894 | 100.00% de las clases completas | `████████████████████` |
| **Entradas del catálogo** | Entradas del juego reconstruidas | Filas de `hooks.csv` con `reversed=1` más las envolturas del juego abiertas, que cuentan como no reconstruidas | 10,494 | 10,494 | 100.00% de las entradas del juego del catálogo | `████████████████████` |
| **Validación** | Entradas del juego validadas | Entradas reconstruidas con evidencia aprobada vinculada (`validated=1`) | 10,494 | 10,494 | 100.00% de las entradas reconstruidas | `████████████████████` |
| **Entradas del catálogo** | Entradas reconstruidas escritas solo en C++ | Entradas reconstruidas cuyo cuerpo fuente no tiene `__asm` ni `naked` (las de fuente no identificada de forma inequívoca no cuentan) | 10,466 | 10,494 | 99.73% de las entradas reconstruidas | `████████████████████` |

**Por qué hay un 89% y un 93%.** No se contradicen: miden cosas distintas. El 88.88% cuenta *funciones* y el 93.12% cuenta *bytes* de código; las funciones que faltan son más pequeñas que las ya reemplazadas, así que quedan muchas funciones pero pocos bytes. Las filas «Funciones» y «Bytes» miden el propio EXE (análisis estático, ver [cobertura real del EXE](#cobertura-exe)) y son la medida del avance hacia la meta; las filas de «Entradas del catálogo», «Clases» y «Validación» miden las filas registradas de `hooks.csv`, que solo conocen una parte del código del juego, por eso su porcentaje es mayor. Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL. La última fila cuenta, de las entradas reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md).

**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md).

Hay 10,494 entradas catalogadas del juego, 0 bloqueadas y 10,494 sin bloqueo. No constituyen un inventario exhaustivo del código del juego; las filas de RenderWare y del CRT del catálogo (83) quedan fuera de este informe. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Las extensiones o adaptaciones NOTSA (4, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

<a id="mediciones"></a>

## 2. Siguientes mediciones

Estado al 14/11/2026. Lo que ya se midió y lo que todavía no tiene un porcentaje útil:

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validación vigente para el árbol actual (validación) | 0/10,494 entradas validadas confirmadas como vigentes (0.00% de las entradas validadas) | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). El 04/11/2026 se repitieron 20 bancos desfasados sin regresión de producción (todos eran bancos desfasados o artefactos del entorno): falta consolidarlos en el registro. |
| Dependencias de funciones originales eliminadas (funciones) | Medido (cota inferior): 311 funciones del juego siguen llamándose directamente desde código del juego ya reemplazado; cierre transitivo: 454 funciones / 81,333 bytes. Ver [Cobertura real del EXE](#cobertura-exe) | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan: lista viva en la [sección 10](AnalisisGrupoA.md). |
| Trabajo real que falta (funciones y bytes) | 1,310 funciones y 206,822 bytes del juego sin reemplazar (11.12% de las funciones y 6.88% de los bytes del juego); grupo A 454 funciones y 81,333 bytes | Reconstruir y validar de menor a mayor coste, empezando por el grupo A ([sección 6](#que-falta)). |
| Coste por categoría de lote (tiempo de reconstrucción y validación) | Medido para las categorías 1, 2, 3 y 5 en los lotes 90A, 90B, 92A, 92B y 92C (tabla de abajo); sin medir la categoría 4 (funciones con FPU) | Cronometrar un lote piloto con FPU (método x87 a 24 bits) y los siguientes lotes de lógica para convertir la estimación en un coste por función. |

Costes medidos en los lotes cerrados (datos al 14/11/2026, [lotes.csv](JuegoExe/lotes.csv)). Cada lote reconstruye en C++ puro funciones que antes se llamaban al EXE por dirección y las valida dentro del juego (pares EXE/DLL con cero diferencias).

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

**¿Quedan envolturas de código del juego en el catálogo?** No. **Envolturas del juego abiertas: 0** (comprobado el 14/11/2026): todas las funciones del juego con hook están reconstruidas en C++.

Las envolturas de biblioteca no cuentan: 211 funciones con envoltura al EXE (RenderWare SDK 140, Controlador D3D9 de RenderWare 58, CRT / runtime estático 11, QuickTime/QTML 2) son RenderWare, controlador D3D9, CRT y QuickTime, que se sustituyen según la [hoja de ruta](RutaExeIndependiente.md) y no son trabajo del juego ([lista](#bibliotecas)). La clasificación sale de la dirección del destino: dentro de 0x401000–0x74FFFF es juego, fuera es biblioteca.

**Lotes en curso:** ninguno registrado para las envolturas del juego.

Cerrar las envolturas del juego no cierra el juego: quedan además las funciones del EXE sin reemplazar (grupos A–D, 1,310 funciones al 14/11/2026; [sección 6](#que-falta)). El plan de trabajo sale de [AnalisisGrupoA.md](AnalisisGrupoA.md) y su [lista completa](JuegoExe/grupoA.csv), y la lista viva está en la [sección 10](AnalisisGrupoA.md):

1. **Grupo A, por categorías y de menor a mayor coste:** helpers triviales y thunks muy llamados (hecho en el lote 90A: `CPad::GetPad` 0x53FB70, `CRGBA::Constructor` 0x7170C0...), pequeñas sin FPU (lote 90B), lógica sin FPU (empezando por las más llamadas y por clases enteras), funciones con FPU con el método x87 a 24 bits, y por último las de 0x300 bytes o más, una por lote (92B, 92C).
2. **Lote piloto de cada categoría** para medir el coste real: ya medidos los de las categorías 1, 2, 3 y 5 (tabla de la sección 2); falta la 4, con FPU.
3. **Revalidación vigente:** repetir los bancos sobre el árbol actual, priorizando los de fuentes cambiadas por lotes posteriores. Motivo: el banco `CVehD` llevaba roto desde el lote 83C (se cambió `m_fSteerAngle` por `m_GasPedal` en `ProcessDrivingAnims`) y no se vio hasta repetirlo.
4. **Después del código del juego:** `librw`, pipelines de SA y puentes `RwD3D9*`, QuickTime, DDraw y CRT, según la [hoja de ruta](RutaExeIndependiente.md); el cambio de método de validación sin el exe como oráculo.

**Validación consolidada parcialmente:** 10494 funciones del juego con evidencia aprobada vinculada, 358 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias (cifras al 14/11/2026). Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

<a id="vista-general"></a>

## 4. Vista general

Al 14/11/2026. Cada columna indica la dimensión y el denominador de su porcentaje.

| Clases validadas (validación) | Clases completas por validar (validación) | Entradas validadas (validación) | Entradas reconstruidas por validar (validación) | Entradas por reconstruir (entradas del catálogo) |
| :---: | :---: | :---: | :---: | :---: |
| **894 / 894 clases** | **0 / 894 clases** | **10,494 / 10,494 entradas** | **0 / 10,494 entradas** | **0 / 10,494 entradas** |
| 100.00% de las clases completas | 0.00% de las clases completas | 100.00% de las entradas reconstruidas | 0.00% de las entradas reconstruidas | 0.00% de las entradas del juego del catálogo |

Reconstrucción por clases: 894 completas, 0 parciales y 0 sin revertir de 894 clases o grupos.

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego ni el avance sobre todo el EXE (para eso, las filas «Funciones» y «Bytes» de la sección 1). Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="cobertura-exe"></a>

## 5. Cobertura real del EXE (estimación)

Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras filas de la tabla de dimensiones salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático ([MemoryMap](MemoryMap/README.md): 17,740 entradas de función detectadas y 3.72 MB de cuerpos de función), cruzado con el catálogo, el registro de validación y las envolturas pendientes. 10,577 filas reconstruidas del catálogo equivalen a **10,539 funciones distintas del EXE ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo). En esta tabla, «% de funciones» es la parte de las funciones de esa región que ya está en C++ y «% de bytes» la parte de los bytes de código de esa región.

| Región del EXE | Funciones (total de la región) | Bytes de código (total de la región) | Reemplazadas por C++: funciones y % de funciones | Reemplazadas: % de bytes | Funciones de la región llamadas directamente desde código ya reemplazado |
| --- | ---: | ---: | --- | --- | ---: |
| Juego y plataforma (0x401000–0x74FFFF) | 11,779 | 3,005,739 | `██████████████████░░` 10,469 · 88.9% de funciones | `███████████████████░` 93.1% de bytes | 311 |
| RenderWare y D3D (0x750000–0x81FFFF) | 1,889 | 600,971 | `█░░░░░░░░░░░░░░░░░░░` 61 · 3.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.3% de bytes | 229 |
| CRT/MSVC y runtime (0x820000 en adelante) | 4,072 | 115,711 | `░░░░░░░░░░░░░░░░░░░░` 9 · 0.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.2% de bytes | 61 |
| **Total** | **17,740** | **3,722,421** | `████████████░░░░░░░░` 10,539 · 59.4% de funciones | `███████████████░░░░░` 75.2% de bytes | **601** |

**Qué significa para un ejecutable independiente.** No hace falta reconstruir las 17,740 funciones: RenderWare/D3D y el CRT son bibliotecas enlazadas dentro del EXE (5,961 entradas, 19% de los bytes del EXE) que se sustituirían enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego (11,779 funciones, 81% de los bytes del EXE), y de él ya está reemplazado el **93% de los bytes del juego** y el 89% de las funciones del juego: no se contradicen, las funciones que quedan son más pequeñas que la media. Entre las 11,779 funciones del juego hay 1,819 de 16 bytes o menos (stubs, thunks y helpers).

<a id="que-falta"></a>

## 6. Lo que falta del código del juego: 1,310 funciones, 207 KB

Al 14/11/2026: 1,310 funciones (11.12% de las funciones del juego) y 206,822 bytes (6.88% de los bytes del juego) siguen sin reemplazar. Este es el trabajo real que falta del código del juego, lo mida o no el catálogo. Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).

| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |
| --- | ---: | ---: | --- |
| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | 454 | 81,333 | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. Lista viva: [dependencias directas (grupo A)](AnalisisGrupoA.md). |
| C. Llamadas directas solo desde código aún no reemplazado | 313 | 55,959 | Dependen de reconstruir antes A y B; parte puede ser código muerto. |
| D. Sin llamadas directas ni vtable | 283 | 52,918 | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |
| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | 260 | 16,612 | Probablemente necesarios: se ejecutan por despacho virtual. |

Del código del juego ya reemplazado, **311 funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura. No hay envolturas del juego abiertas (sección siguiente).

> [!NOTE]
> **Cotas y límites.** Es un análisis estático: hay 6,004 llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.

<a id="completas"></a>
## 7. 🟢 Completas · 894 clases

Al 14/11/2026: 894 de 894 clases o grupos están completas (todas sus entradas reconstruidas y ninguna envoltura del juego abierta). Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (11) ✅ 11 | [CLinkList&lt;CCollisionData*&gt;](../source/game_sa/Collision/Collision.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (6) ✅ 6 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (18) ✅ 18 | [CLinkList&lt;CPed*&gt;](../source/game_sa/VisibilityPlugins.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (10) ✅ 10 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (15) ✅ 15 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (7) ✅ 7 | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (11) ✅ 11 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (8) ✅ 8 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (4) ✅ 4 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (10) ✅ 10 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (4) ✅ 4 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (9) ✅ 9 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CMaths](../source/game_sa/Maths.cpp) (6) ✅ 6 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (13) ✅ 13 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (6) ✅ 6 | [CMatrix](../source/game_sa/Core/Matrix.cpp) (46) ✅ 46 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (8) ✅ 8 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (6) ✅ 6 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (9) ✅ 9 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (11) ✅ 11 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (8) ✅ 8 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CMBlur](../source/game_sa/MBlur.cpp) (5) ✅ 5 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (11) ✅ 11 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (10) ✅ 10 | [CMemoryHeap](../source/game_sa/Memory/MemoryHeap.cpp) (1) ✅ 1 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (12) ✅ 12 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (16) ✅ 16 | [CMemoryMgr](../source/game_sa/MemoryMgr.cpp) (10) ✅ 10 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (9) ✅ 9 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (6) ✅ 6 | [CMentalState](../source/game_sa/MentalHealth.cpp) (1) ✅ 1 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CMenuManager](../source/game_sa/MenuManager.cpp) (62) ✅ 62 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (55) ✅ 55 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (8) ✅ 8 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (9) ✅ 9 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (8) ✅ 8 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (5) ✅ 5 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CMissionCleanup](../source/game_sa/MissionCleanup.cpp) (3) ✅ 3 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (5) ✅ 5 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (11) ✅ 11 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (8) ✅ 8 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (22) ✅ 22 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (29) ✅ 29 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (5) ✅ 5 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (13) ✅ 13 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (5) ✅ 5 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (4) ✅ 4 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (8) ✅ 8 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexUseMobilePhone](../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp) (1) ✅ 1 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (7) ✅ 7 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (12) ✅ 12 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (8) ✅ 8 | [CNearbyFireScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (8) ✅ 8 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (14) ✅ 14 | [CNodeAddress](../source/game_sa/NodeAddress.cpp) (3) ✅ 3 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (9) ✅ 9 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (14) ✅ 14 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (3) ✅ 3 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (8) ✅ 8 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (8) ✅ 8 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (13) ✅ 13 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (22) ✅ 22 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (56) ✅ 56 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (6) ✅ 6 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (12) ✅ 12 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (59) ✅ 59 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (9) ✅ 9 |
| [CAEPedSpeechSlot](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (1) ✅ 1 | [CObjectPotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (8) ✅ 8 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (8) ✅ 8 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 17 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexWaitForPed](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) (3) ✅ 3 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 43 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (10) ✅ 10 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (20) ✅ 20 | [Common](../source/game_sa/common.cpp) (26) ✅ 26 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (14) ✅ 14 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (11) ✅ 11 | [CompressedVector](../source/game_sa/CompressedVector.cpp) (2) ✅ 2 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (14) ✅ 14 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (19) ✅ 19 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (9) ✅ 9 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (15) ✅ 15 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (9) ✅ 9 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (11) ✅ 11 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (11) ✅ 11 | [COnscreenTimerEntry](../source/game_sa/OnscreenTimer.cpp) (1) ✅ 1 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (18) ✅ 18 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (25) ✅ 25 | [CPad](../source/game_sa/Pad.cpp) (97) ✅ 97 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (13) ✅ 13 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (3) ✅ 3 | [CPathFind](../source/game_sa/PathFind.cpp) (71) ✅ 71 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (7) ✅ 7 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (16) ✅ 16 | [CPathNode](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (7) ✅ 7 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (3) ✅ 3 | [CPatrolRoute](../source/game_sa/PatrolRoute.cpp) (5) ✅ 5 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (4) ✅ 4 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPatrolRoutePool](../source/game_sa/Pools/PatrolRoutePool.h) (1) ✅ 1 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (6) ✅ 6 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (10) ✅ 10 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (152) ✅ 152 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (7) ✅ 7 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPedAcquaintanceScanner](../source/game_sa/Events/EventScanner.cpp) (4) ✅ 4 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (12) ✅ 12 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (107) ✅ 107 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (20) ✅ 20 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (8) ✅ 8 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (14) ✅ 14 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (14) ✅ 14 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (10) ✅ 10 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (9) ✅ 9 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (11) ✅ 11 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (10) ✅ 10 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (9) ✅ 9 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (9) ✅ 9 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (9) ✅ 9 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (6) ✅ 6 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (8) ✅ 8 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (7) ✅ 7 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (13) ✅ 13 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 38 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (8) ✅ 8 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (6) ✅ 6 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (3) ✅ 3 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (8) ✅ 8 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (23) ✅ 23 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (3) ✅ 3 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (10) ✅ 10 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (3) ✅ 3 | [CTaskInteriorSitInChair](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp) (5) ✅ 5 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (3) ✅ 3 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (9) ✅ 9 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedGroupDefaultTaskAllocators](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h) (1) ✅ 1 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (8) ✅ 8 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (17) ✅ 17 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (3) ✅ 3 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (20) ✅ 20 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (8) ✅ 8 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (3) ✅ 3 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (32) ✅ 32 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (31) ✅ 31 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (3) ✅ 3 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (13) ✅ 13 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (19) ✅ 19 | [CTaskSimple](../source/game_sa/Tasks/Task.cpp) · [archivo 2](../source/game_sa/Tasks/TaskSimple.cpp) (4) ✅ 4 |
| [CAttractorScanner](../source/game_sa/AttractorScanner.cpp) (2) ✅ 2 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskSimpleAbseil](../source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp) (2) ✅ 2 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (83) ✅ 83 | [CPedGroups](../source/game_sa/PedGroups.cpp) (12) ✅ 12 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (8) ✅ 8 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (8) ✅ 8 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (7) ✅ 7 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (104) ✅ 104 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (61) ✅ 61 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (4) ✅ 4 |
| [CAutoPilot](../source/game_sa/AutoPilot.cpp) (2) ✅ 2 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (8) ✅ 8 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (37) ✅ 37 | [CPedList](../source/game_sa/PedList.cpp) (9) ✅ 9 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (62) ✅ 62 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (9) ✅ 9 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (4) ✅ 4 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (11) ✅ 11 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (13) ✅ 13 | [CPedPool](../source/game_sa/Pools/PedPool.h) (3) ✅ 3 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (11) ✅ 11 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (24) ✅ 24 | [CPedSaveStructure](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (11) ✅ 11 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPedScanner](../source/game_sa/PedScanner.cpp) (1) ✅ 1 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (16) ✅ 16 |
| [CBoundingBox](../source/game_sa/Collision/BoundingBox.cpp) (5) ✅ 5 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (6) ✅ 6 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (11) ✅ 11 |
| [CBridge](../source/game_sa/Bridge.cpp) (5) ✅ 5 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (6) ✅ 6 | [CTaskSimpleCarForcePedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp) (7) ✅ 7 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPedStats](../source/game_sa/PedStats.cpp) (5) ✅ 5 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (10) ✅ 10 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (6) ✅ 6 | [CPedStuckChecker](../source/game_sa/PedStuckChecker.cpp) (1) ✅ 1 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (11) ✅ 11 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (8) ✅ 8 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (11) ✅ 11 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPedType](../source/game_sa/PedType.cpp) (14) ✅ 14 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (11) ✅ 11 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (10) ✅ 10 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (60) ✅ 60 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (10) ✅ 10 |
| [CCam](../source/game_sa/Cam.cpp) (56) ✅ 56 | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (7) ✅ 7 |
| [CCamera](../source/game_sa/Camera.cpp) (122) ✅ 122 | [CPickups](../source/game_sa/Pickups.cpp) (38) ✅ 38 | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (7) ✅ 7 |
| [CCarAI](../source/game_sa/CarAI.cpp) (26) ✅ 26 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (21) ✅ 21 | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (8) ✅ 8 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (112) ✅ 112 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (7) ✅ 7 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 32 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (26) ✅ 26 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (7) ✅ 7 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPlaneBanners](../source/game_sa/PlaneBanners.cpp) (1) ✅ 1 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (10) ✅ 10 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (9) ✅ 9 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (11) ✅ 11 |
| [CCarPathLink](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (11) ✅ 11 |
| [CCheat](../source/game_sa/Cheat.cpp) (74) ✅ 74 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (8) ✅ 8 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (8) ✅ 8 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (20) ✅ 20 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (5) ✅ 5 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (4) ✅ 4 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (7) ✅ 7 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (31) ✅ 31 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (7) ✅ 7 |
| [CClothes](../source/game_sa/Clothes.cpp) (12) ✅ 12 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (70) ✅ 70 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (19) ✅ 19 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (23) ✅ 23 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (5) ✅ 5 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CPlayerRelationshipRecorder](../source/game_sa/PlayerRelationshipRecorder.cpp) (6) ✅ 6 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (7) ✅ 7 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (26) ✅ 26 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (7) ✅ 7 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CPointLights](../source/game_sa/PointLights.cpp) (7) ✅ 7 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (11) ✅ 11 |
| [CColBox](../source/game_sa/Collision/ColBox.cpp) (2) ✅ 2 | [CPointList](../source/game_sa/PointList.cpp) (2) ✅ 2 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (6) ✅ 6 |
| [CColDisk](../source/game_sa/Collision/ColDisk.cpp) (1) ✅ 1 | [CPointRoute](../source/game_sa/PedGeometryAnalyser.cpp) (6) ✅ 6 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (7) ✅ 7 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (2) ✅ 2 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (6) ✅ 6 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (62) ✅ 62 | [CPools](../source/game_sa/Pools.cpp) (20) ✅ 20 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (7) ✅ 7 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (10) ✅ 10 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (18) ✅ 18 |
| [CCollisionEventScanner](../source/game_sa/Collision/CollisionEventScanner.cpp) (1) ✅ 1 | [CPopulation](../source/game_sa/Population.cpp) (68) ✅ 68 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (7) ✅ 7 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 38 | [CTaskSimpleDuckWhileShotsWhizzing](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp) (7) ✅ 7 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (12) ✅ 12 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (9) ✅ 9 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CProjectile](../source/game_sa/Entity/Object/Projectile.cpp) (3) ✅ 3 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (9) ✅ 9 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (8) ✅ 8 |
| [CColPoint](../source/game_sa/Collision/ColPoint.cpp) (2) ✅ 2 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (5) ✅ 5 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (9) ✅ 9 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (4) ✅ 4 | [CTaskSimpleFight](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) (27) ✅ 27 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (29) ✅ 29 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (14) ✅ 14 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (8) ✅ 8 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (14) ✅ 14 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (5) ✅ 5 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CQuaternion](../source/game_sa/Core/Quaternion.cpp) (10) ✅ 10 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (13) ✅ 13 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CRadar](../source/game_sa/Radar.cpp) (66) ✅ 66 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (7) ✅ 7 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (11) ✅ 11 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (8) ✅ 8 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (9) ✅ 9 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (6) ✅ 6 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CRect](../source/game_sa/Core/Rect.cpp) (9) ✅ 9 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (11) ✅ 11 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (12) ✅ 12 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (8) ✅ 8 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (7) ✅ 7 |
| [CCoronas](../source/game_sa/Coronas.cpp) (12) ✅ 12 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (5) ✅ 5 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CRemote](../source/game_sa/Remote.cpp) (3) ✅ 3 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (4) ✅ 4 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (4) ✅ 4 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CReplay](../source/game_sa/Replay.cpp) (48) ✅ 48 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (4) ✅ 4 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (4) ✅ 4 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CRGBA](../source/game_sa/RGBA.cpp) (2) ✅ 2 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (4) ✅ 4 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CRideAnimData](../source/game_sa/RideAnimData.cpp) (2) ✅ 2 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (4) ✅ 4 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 8 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (4) ✅ 4 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 7 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (4) ✅ 4 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (4) ✅ 4 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (18) ✅ 18 | [CRubbish](../source/game_sa/Rubbish.cpp) (1) ✅ 1 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (8) ✅ 8 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (39) ✅ 39 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (4) ✅ 4 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CSaveGarage](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (17) ✅ 17 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (6) ✅ 6 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (7) ✅ 7 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (16) ✅ 16 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (11) ✅ 11 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (10) ✅ 10 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (9) ✅ 9 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (37) ✅ 37 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (9) ✅ 9 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (10) ✅ 10 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (11) ✅ 11 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (7) ✅ 7 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (8) ✅ 8 | [CSearchLight](../source/game_sa/SearchLight.cpp) (1) ✅ 1 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (8) ✅ 8 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CSector](../source/game_sa/Sector.cpp) (1) ✅ 1 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (15) ✅ 15 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CSetPiece](../source/game_sa/SetPiece.cpp) (7) ✅ 7 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (11) ✅ 11 |
| [CData](../source/game_sa/Text/Text.cpp) (3) ✅ 3 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (9) ✅ 9 |
| [CDebug](../source/game_sa/Debug.cpp) (2) ✅ 2 | [CSexyPedScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (9) ✅ 9 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (7) ✅ 7 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (20) ✅ 20 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CDecisionMaker](../source/game_sa/DecisionMakers/DecisionMaker.cpp) (3) ✅ 3 | [CShadows](../source/game_sa/Shadows.cpp) (30) ✅ 30 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (5) ✅ 5 |
| [CDecisionMakerTypes](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) (17) ✅ 17 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (5) ✅ 5 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CShopping](../source/game_sa/Shopping.cpp) (33) ✅ 33 | [CTaskSimpleOnEscalator](../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp) (5) ✅ 5 |
| [CDirectory](../source/game_sa/Directory.cpp) (10) ✅ 10 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (7) ✅ 7 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CSimpleTransform](../source/game_sa/SimpleTransform.cpp) (3) ✅ 3 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (10) ✅ 10 |
| [CDraw](../source/game_sa/Draw.cpp) (3) ✅ 3 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (5) ✅ 5 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (11) ✅ 11 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (7) ✅ 7 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (6) ✅ 6 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (13) ✅ 13 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (6) ✅ 6 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (3) ✅ 3 |
| [CDummyPed](../source/game_sa/Entity/Dummy/DummyPed.cpp) (3) ✅ 3 | [CSphere](../source/game_sa/Collision/Sphere.cpp) (1) ✅ 1 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (4) ✅ 4 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (7) ✅ 7 |
| [CEmergencyPed](../source/game_sa/Entity/Ped/EmergencyPed.cpp) (4) ✅ 4 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (35) ✅ 35 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (10) ✅ 10 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (73) ✅ 73 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (8) ✅ 8 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CStats](../source/game_sa/Stats.cpp) (52) ✅ 52 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculator](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp) (4) ✅ 4 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (3) ✅ 3 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (5) ✅ 5 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (13) ✅ 13 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CStoredCar](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (2) ✅ 2 | [CStreaming](../source/game_sa/Streaming.cpp) (95) ✅ 95 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (7) ✅ 7 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (13) ✅ 13 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (7) ✅ 7 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (19) ✅ 19 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (9) ✅ 9 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (9) ✅ 9 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (13) ✅ 13 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (9) ✅ 9 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTagManager](../source/game_sa/TagManager.cpp) (20) ✅ 20 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (8) ✅ 8 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTask](../source/game_sa/Tasks/Task.cpp) (8) ✅ 8 | [CTaskSimpleSlideToCoord](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp) (1) ✅ 1 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (7) ✅ 7 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (7) ✅ 7 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (6) ✅ 6 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (9) ✅ 9 |
| [CEvent](../source/game_sa/Events/Event.cpp) (16) ✅ 16 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (5) ✅ 5 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (9) ✅ 9 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (5) ✅ 5 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (7) ✅ 7 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (15) ✅ 15 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (1) ✅ 1 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (4) ✅ 4 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (7) ✅ 7 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (6) ✅ 6 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (10) ✅ 10 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (7) ✅ 7 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (6) ✅ 6 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (8) ✅ 8 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (8) ✅ 8 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (7) ✅ 7 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (10) ✅ 10 | [CTaskCategories](../source/game_sa/Tasks/TaskCategories.cpp) (3) ✅ 3 | [CTaskSimpleTriggerLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp) (7) ✅ 7 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (6) ✅ 6 | [CTaskComplex](../source/game_sa/Tasks/Task.cpp) · [archivo 2](../source/game_sa/Tasks/TaskComplex.cpp) (7) ✅ 7 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (7) ✅ 7 |
| [CEventChatPartner](../source/game_sa/Events/EventChatPartner.cpp) (2) ✅ 2 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (10) ✅ 10 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (7) ✅ 7 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (6) ✅ 6 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (8) ✅ 8 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (5) ✅ 5 |
| [CEventCreatePartnerTask](../source/game_sa/Events/EventCreatePartnerTask.cpp) (4) ✅ 4 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (15) ✅ 15 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (22) ✅ 22 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (25) ✅ 25 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (4) ✅ 4 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (7) ✅ 7 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (9) ✅ 9 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (11) ✅ 11 | [CTaskSimpleWaitForPizza](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp) (7) ✅ 7 |
| [CEventDeadPed](../source/game_sa/Events/EventDeadPed.cpp) (2) ✅ 2 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (11) ✅ 11 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (7) ✅ 7 |
| [CEventDeath](../source/game_sa/Events/EventDeath.cpp) (1) ✅ 1 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (8) ✅ 8 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (8) ✅ 8 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (13) ✅ 13 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (7) ✅ 7 |
| [CEventDraggedOutCar](../source/game_sa/Events/EventDraggedOutCar.cpp) (3) ✅ 3 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (6) ✅ 6 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (7) ✅ 7 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (13) ✅ 13 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (2) ✅ 2 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (4) ✅ 4 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (6) ✅ 6 |
| [CEventFireNearby](../source/game_sa/Events/EventFireNearby.cpp) (3) ✅ 3 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (7) ✅ 7 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (5) ✅ 5 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (12) ✅ 12 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventGotKnockedOverByCar](../source/game_sa/Events/EventGotKnockedOverByCar.cpp) (2) ✅ 2 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (10) ✅ 10 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (14) ✅ 14 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (8) ✅ 8 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (72) ✅ 72 |
| [CEventGroupEvent](../source/game_sa/Events/EventGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (5) ✅ 5 | [CTheZones](../source/game_sa/TheZones.cpp) (23) ✅ 23 |
| [CEventGunAimedAt](../source/game_sa/Events/EventGunAimedAt.cpp) (5) ✅ 5 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (10) ✅ 10 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (13) ✅ 13 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (10) ✅ 10 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (8) ✅ 8 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (10) ✅ 10 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (4) ✅ 4 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (11) ✅ 11 | [CTimer](../source/game_sa/Timer.cpp) (16) ✅ 16 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (12) ✅ 12 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (11) ✅ 11 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (7) ✅ 7 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (10) ✅ 10 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (16) ✅ 16 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (7) ✅ 7 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (11) ✅ 11 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (43) ✅ 43 |
| [CEventInteriorUseInfo](../source/game_sa/Events/EventInteriorUseInfo.cpp) (5) ✅ 5 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (8) ✅ 8 | [CTrainNode](../source/game_sa/TrainNode.cpp) (2) ✅ 2 |
| [CEventInWater](../source/game_sa/Events/EventInWater.cpp) (4) ✅ 4 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (7) ✅ 7 | [cTransmission](../source/game_sa/cTransmission.cpp) (5) ✅ 5 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (17) ✅ 17 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (8) ✅ 8 | [CTxdStore](../source/game_sa/TxdStore.cpp) (33) ✅ 33 |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (11) ✅ 11 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (8) ✅ 8 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (12) ✅ 12 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (10) ✅ 10 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (10) ✅ 10 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (8) ✅ 8 | [CVector](../source/game_sa/Core/Vector.cpp) (23) ✅ 23 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (10) ✅ 10 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (9) ✅ 9 | [CVector2D](../source/game_sa/Core/Vector.cpp) · [archivo 2](../source/game_sa/Core/Vector2D.cpp) (2) ✅ 2 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (9) ✅ 9 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (17) ✅ 17 | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (170) ✅ 170 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (8) ✅ 8 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (5) ✅ 5 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (6) ✅ 6 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (8) ✅ 8 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (10) ✅ 10 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (6) ✅ 6 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (1) ✅ 1 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (5) ✅ 5 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (78) ✅ 78 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (9) ✅ 9 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (10) ✅ 10 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (3) ✅ 3 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (7) ✅ 7 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (9) ✅ 9 | [CVehiclePotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (5) ✅ 5 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (9) ✅ 9 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventPedEnteredMyVehicle](../source/game_sa/Events/EventPedEnteredMyVehicle.cpp) (2) ✅ 2 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (10) ✅ 10 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPedToChase](../source/game_sa/Events/EventPedToChase.cpp) (2) ✅ 2 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (9) ✅ 9 | [CVehicleScanner](../source/game_sa/VehicleScanner.cpp) (1) ✅ 1 |
| [CEventPedToFlee](../source/game_sa/Events/EventPedToFlee.cpp) (2) ✅ 2 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (9) ✅ 9 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) ✅ 84 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (5) ✅ 5 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (11) ✅ 11 | [CWanted](../source/game_sa/Wanted.cpp) (35) ✅ 35 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (8) ✅ 8 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (10) ✅ 10 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (7) ✅ 7 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (9) ✅ 9 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (6) ✅ 6 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (9) ✅ 9 | [CWaterLevel](../source/game_sa/WaterLevel.cpp) (48) ✅ 48 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (9) ✅ 9 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 41 |
| [CEventPotentialGetRunOver](../source/game_sa/Events/EventPotentialGetRunOver.cpp) (2) ✅ 2 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (12) ✅ 12 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (10) ✅ 10 |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (6) ✅ 6 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (9) ✅ 9 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (16) ✅ 16 |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (7) ✅ 7 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (9) ✅ 9 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (4) ✅ 4 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (8) ✅ 8 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (11) ✅ 11 | [CWeather](../source/game_sa/Weather.cpp) (14) ✅ 14 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (7) ✅ 7 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (20) ✅ 20 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventRevived](../source/game_sa/Events/EventRevived.cpp) (2) ✅ 2 | [CTaskComplexFollowPatrolRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp) (1) ✅ 1 | [CWinsockLink](../source/game_sa/WinsockLink.cpp) (2) ✅ 2 |
| [CEventScanner](../source/game_sa/Events/EventScanner.cpp) (3) ✅ 3 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (10) ✅ 10 | [CWorld](../source/game_sa/World.cpp) (96) ✅ 96 |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (9) ✅ 9 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (15) ✅ 15 | [CWorldScan](../source/game_sa/Renderer.cpp) (2) ✅ 2 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (1) ✅ 1 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (11) ✅ 11 | [CZone](../source/game_sa/Zone.cpp) (1) ✅ 1 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (1) ✅ 1 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (9) ✅ 9 | [CZoneDef](../source/game_sa/CullZones.cpp) (1) ✅ 1 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (8) ✅ 8 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (16) ✅ 16 | [D3DIndexDataBuffer](../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp) (5) ✅ 5 |
| [CEventSexyPed](../source/game_sa/Events/EventSexyPed.cpp) (2) ✅ 2 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (9) ✅ 9 | [D3DResourceSystem](../source/game_sa/RenderWare/D3DResourceSystem.cpp) (16) ✅ 16 |
| [CEventSexyVehicle](../source/game_sa/Events/EventSexyVehicle.cpp) (2) ✅ 2 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (8) ✅ 8 | [D3DTextureBuffer](../source/game_sa/RenderWare/D3DTextureBuffer.cpp) (8) ✅ 8 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (11) ✅ 11 | [CTaskComplexGoPickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp) (8) ✅ 8 | [Furniture_c](../source/game_sa/Interior/Furniture_c.cpp) (1) ✅ 1 |
| [CEventSource](../source/game_sa/Events/EventSource.cpp) (1) ✅ 1 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (9) ✅ 9 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (7) ✅ 7 |
| [CEventSpecial](../source/game_sa/Events/EventSpecial.cpp) (1) ✅ 1 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (10) ✅ 10 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (9) ✅ 9 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (8) ✅ 8 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (12) ✅ 12 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (6) ✅ 6 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (4) ✅ 4 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (7) ✅ 7 | [Fx_c](../source/game_sa/Fx/Fx.cpp) (36) ✅ 36 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (9) ✅ 9 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (9) ✅ 9 | [FxEmitter_c](../source/game_sa/Fx/FxEmitter.cpp) (4) ✅ 4 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (9) ✅ 9 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (9) ✅ 9 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (9) ✅ 9 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (2) ✅ 2 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (7) ✅ 7 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [FxFrustumInfo_c](../source/game_sa/Fx/FxFrustumInfo.cpp) (1) ✅ 1 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (9) ✅ 9 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (5) ✅ 5 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (4) ✅ 4 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (5) ✅ 5 | [FxInterpInfo255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp) (1) ✅ 1 |
| [CEventVehicleThreat](../source/game_sa/Events/EventVehicleThreat.cpp) (3) ✅ 3 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (9) ✅ 9 | [FxInterpInfo32_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp) (1) ✅ 1 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (7) ✅ 7 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (12) ✅ 12 | [FxInterpInfoFloat_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp) (1) ✅ 1 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (8) ✅ 8 | [FxInterpInfoU255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp) (1) ✅ 1 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (6) ✅ 6 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (50) ✅ 50 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (10) ✅ 10 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (7) ✅ 7 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FxPrim_c](../source/game_sa/Fx/FxPrim.cpp) (3) ✅ 3 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (4) ✅ 4 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (10) ✅ 10 | [FxPrtMult_c](../source/game_sa/Fx/FxPrtMult.cpp) (3) ✅ 3 |
| [CFont](../source/game_sa/Font.cpp) (40) ✅ 40 | [CTaskComplexKillAllThreats](../source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp) (1) ✅ 1 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (5) ✅ 5 |
| [CFontChar](../source/game_sa/Font.cpp) (1) ✅ 1 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (12) ✅ 12 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (39) ✅ 39 |
| [CFormation](../source/game_sa/Formation.cpp) (10) ✅ 10 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (8) ✅ 8 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CGame](../source/game_sa/Game.cpp) (25) ✅ 25 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (9) ✅ 9 | [Game](../source/app/app_game.cpp) (12) ✅ 12 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (9) ✅ 9 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (12) ✅ 12 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (10) ✅ 10 |
| [CGangWars](../source/game_sa/GangWars.cpp) (36) ✅ 36 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (53) ✅ 53 |
| [CGarage](../source/game_sa/Garage.cpp) (32) ✅ 32 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (8) ✅ 8 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (24) ✅ 24 |
| [CGarages](../source/game_sa/Garages.cpp) (30) ✅ 30 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (9) ✅ 9 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (23) ✅ 23 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexLeaveCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp) (14) ✅ 14 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (6) ✅ 6 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (20) ✅ 20 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (6) ✅ 6 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (9) ✅ 9 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (3) ✅ 3 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (9) ✅ 9 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (11) ✅ 11 | [Particle_c](../source/game_sa/Fx/Particle.cpp) (1) ✅ 1 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 32 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (10) ✅ 10 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (13) ✅ 13 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (9) ✅ 9 | [ProcObjectListItem](../source/game_sa/Plant/ProcObjectMan.cpp) (1) ✅ 1 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (5) ✅ 5 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (9) ✅ 9 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (10) ✅ 10 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (3) ✅ 3 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp) (10) ✅ 10 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (7) ✅ 7 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (26) ✅ 26 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (10) ✅ 10 | [Ps](../source/app/platform/win/WinPs.cpp) (23) ✅ 23 |
| [CHud](../source/game_sa/Hud.cpp) (39) ✅ 39 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (5) ✅ 5 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (5) ✅ 5 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (47) ✅ 47 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (14) ✅ 14 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (11) ✅ 11 | [RpAnisot](../source/game_sa/RenderWare/rw/rpanisot.cpp) (3) ✅ 3 |
| [CInformFriendsEvent](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (7) ✅ 7 | [Rs](../source/app/platform/platform.cpp) (30) ✅ 30 |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (8) ✅ 8 | [RwHelper](../source/game_sa/RwHelper.cpp) (42) ✅ 42 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (5) ✅ 5 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (8) ✅ 8 | [Securom](../source/app/securom/Securom.cpp) (4) ✅ 4 |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (6) ✅ 6 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (48) ✅ 48 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (10) ✅ 10 | [tColLighting](../source/game_sa/Collision/ColPoint.cpp) (1) ✅ 1 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (10) ✅ 10 | [TList_c](../source/game_sa/Core/List_c.cpp) (3) ✅ 3 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (4) ✅ 4 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (14) ✅ 14 | [tUserList](../source/game_sa/Scripts/Scripted2dEffects.cpp) (2) ✅ 2 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (12) ✅ 12 | [UserTracksLegacyVector](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (1) ✅ 1 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (5) ✅ 5 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (9) ✅ 9 | [VectorConstructorIterator](../source/game_sa/RuntimeHelpers.cpp) (1) ✅ 1 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (10) ✅ 10 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (10) ✅ 10 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (7) ✅ 7 |
| [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (11) ✅ 11 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (4) ✅ 4 |
| [CLinkList&lt;AlphaObjectInfo&gt;](../source/game_sa/VisibilityPlugins.cpp) (3) ✅ 3 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (12) ✅ 12 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CLinkList&lt;CAnimBlendHierarchy*&gt;](../source/game_sa/Animation/AnimManager.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (8) ✅ 8 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (7) ✅ 7 |

</details>

<a id="parciales"></a>
## 8. 🟡 Parciales · 0 clases del catálogo + 78 clases/módulos con funciones del EXE pendientes

Orden de mayor a menor tamaño de las envolturas al EXE pendientes (suma de bytes de sus cuerpos; a igualdad, alfabético). «Función más grande» y «Tamaño» (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B) indican cuánto pesa la función más difícil de la clase. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas del juego al EXE», que cuenta solo código del juego; las de biblioteca no entran): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| *(ninguna clase parcial al 14/11/2026)* | | | | | | | | | |

### Clases con funciones del EXE aún sin reemplazar · 78 clases / módulos

Todas estas clases o módulos son parciales: conservan funciones del EXE en 0x401000–0x74FFFF sin reemplazar por C++ (grupos A–D del mapa del EXE y envolturas; 1,310 funciones). La clase se atribuye por cercanía de direcciones y puede ser errónea cuando su confianza es baja. Lista completa por función: [pendientes-por-clase.md](JuegoExe/pendientes-por-clase.md).

| Clase o módulo | Funciones pendientes | Bytes pendientes | Función más grande (B) | Tamaño |
| --- | ---: | ---: | ---: | --- |
| CAudioZones | **1** | 139 | 139 | mediana |
| IKChain_c | **2** | 205 | 103 | mediana |
| CWaterLevel | **1** | 208 | 208 | mediana |
| WaterCreature_c | **2** | 211 | 106 | mediana |
| CTaskComplexGotoDoorAndOpen | **6** | 388 | 151 | mediana |
| CEventScriptedAttractor | **13** | 527 | 93 | pequeña |
| CPedAttractorPool | **12** | 530 | 62 | pequeña |
| CStreaming | **17** | 535 | 195 | mediana |
| CStencilShadows | **5** | 543 | 136 | mediana |
| CTaskComplexRoadRage | **9** | 585 | 194 | mediana |
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
| CTaskComplexInvestigateDisturbance | **7** | 858 | 348 | mediana |
| CVehicle | **33** | 861 | 227 | mediana |
| CVehicleSaveStructure | **4** | 874 | 282 | mediana |
| Ps | **6** | 876 | 327 | mediana |
| CPad | **11** | 895 | 322 | mediana |
| CEventSeenCop | **35** | 902 | 88 | pequeña |
| CTaskComplexSunbathe | **6** | 935 | 725 | grande |
| CEntityScanner | **5** | 958 | 422 | mediana |
| CTaskComplexMedicTreatInjuredPed | **13** | 974 | 259 | mediana |
| CTaskComplexGoToPointAndStandStill | **8** | 977 | 661 | grande |
| CTaskSimpleBeHit | **8** | 1,022 | 451 | mediana |
| CEventVehicleHitAndRun | **15** | 1,037 | 272 | mediana |
| CPed | **6** | 1,046 | 404 | mediana |
| CCam | **7** | 1,059 | 568 | grande |
| CTaskComplexStuckInAir | **11** | 1,075 | 141 | mediana |
| CTrain | **6** | 1,076 | 719 | grande |
| CTaskComplexSeekEntityAnyMeansXYOffset | **4** | 1,138 | 981 | grande |
| CPlaceable | **24** | 1,138 | 77 | pequeña |
| CEventVehicleOnFire | **17** | 1,145 | 111 | mediana |
| CEventVehicleCollision | **15** | 1,149 | 126 | mediana |
| CTaskSimpleRunTimedAnim | **12** | 1,175 | 226 | mediana |
| Interior_c | **4** | 1,200 | 591 | grande |
| CAttractorScanner | **3** | 1,273 | 480 | mediana |
| CClothesBuilder | **13** | 1,285 | 339 | mediana |
| CVehicleModelInfo | **15** | 1,356 | 343 | mediana |
| CTaskComplexAvoidOtherPedWhileWandering | **10** | 1,382 | 455 | mediana |
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
| CAutomobile | **10** | 2,917 | 1,018 | grande |
| CCustomCarEnvMapPipeline | **25** | 3,547 | 804 | grande |
| CPools | **52** | 3,733 | 94 | pequeña |
| CReplay | **10** | 4,172 | 1,236 | grande |
| CRunningScript | **4** | 5,919 | 5,816 | muy grande |
| RpAnimBlend | **16** | 6,102 | 971 | grande |
| CScriptsForBrains | **4** | 6,307 | 6,028 | muy grande |
| FxInfoManager_c | **116** | 6,591 | 101 | mediana |
| CPedAttractorManager | **71** | 8,097 | 739 | grande |
| CTaskComplexSeekEntityRadiusAngleOffset | **5** | 8,771 | 8,462 | muy grande |
| CEntity | **42** | 9,537 | 3,282 | muy grande |
| CTheScripts | **14** | 9,864 | 8,957 | muy grande |
| CWeaponModelInfo | **41** | 13,554 | 1,530 | grande |
| Win | **85** | 19,907 | 2,207 | muy grande |
| FxEmitterBP_c | **141** | 28,395 | 801 | grande |
| **TOTAL (78 clases / módulos)** | **1,310** | **206,822** | 8,957 | |

<a id="sin-revertir"></a>
## 9. 🔴 Sin revertir · 0 clases del juego y bibliotecas fuera del objetivo (RenderWare, D3D9, CRT, QuickTime)

Orden de mayor a menor tamaño de las envolturas pendientes. Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |

Además de las clases del juego, aquí quedan las **bibliotecas enlazadas dentro del EXE** (RenderWare, controlador D3D9, CRT y QuickTime), que no se reconstruyen: se sustituyen por bibliotecas equivalentes.

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


<a id="metodo"></a>
## 10. Cómo leer y actualizar

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
