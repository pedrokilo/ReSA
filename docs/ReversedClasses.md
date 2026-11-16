# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-11-16 04:18 UTC

**Índice:** [1. Objetivo y dimensiones](#objetivo) · [2. Siguientes mediciones](#mediciones) · [3. Siguiente fase](#siguiente-fase) · [4. Vista general](#vista-general) · [5. Cobertura real del EXE](#cobertura-exe) · [6. Lo que falta del código del juego](#que-falta) · [7. Completas](#completas) · [8. Parciales](#parciales) · [9. Sin revertir](#sin-revertir) · [10. Cómo leer y actualizar](#metodo)

<a id="objetivo"></a>

## 1. Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. **La meta de este informe es completar todas las funciones del juego** (11,776 funciones y 3.01 MB de código en el EXE: el código propio del juego y su capa de plataforma). RenderWare, Direct3D y el CRT son bibliotecas enlazadas dentro del EXE: no forman parte de la meta, no se cuentan en las tablas y solo aparecen en la [cobertura real del EXE](#cobertura-exe) y en la lista de [bibliotecas fuera del objetivo](#bibliotecas) para aclarar qué queda fuera. Completar el juego es un hito hacia **ReSA — Zero Original Function Dependency**.

Cifras medidas el 16/11/2026. **Cada porcentaje de este informe lleva escrita la dimensión de la que habla**: *funciones* (del EXE), *bytes* (de código del EXE), *clases*, *entradas del catálogo* (filas de `hooks.csv`) o *validación* (cuántas de las ya hechas tienen evidencia aprobada). Dimensiones distintas dan porcentajes distintos y no son comparables entre sí.

| Tipo | Dimensión | Qué cuenta | Numerador | Denominador | Porcentaje (de qué) | Progreso |
| --- | --- | --- | ---: | ---: | --- | --- |
| **Funciones** | Funciones del juego reemplazadas por C++ en el EXE (**meta del proyecto**) | Funciones del EXE en 0x401000–0x74FFFF que ya tienen su hook reconstruido (análisis estático del EXE) | 10,686 | 11,776 | 90.74% de las funciones del juego | `██████████████████░░` |
| **Funciones** | Funciones del juego que faltan | Complemento de la fila anterior: grupos A–D de [lo que falta](#que-falta) | 1,090 | 11,776 | 9.26% de las funciones del juego | `██░░░░░░░░░░░░░░░░░░` |
| **Bytes** | Bytes de código del juego reemplazados | Suma de los cuerpos de esas mismas funciones, en bytes de código | 2,814,353 | 3,005,360 | 93.64% de los bytes de código del juego | `███████████████████░` |
| **Clases** | Clases del juego completas | Clases o grupos del catálogo con todas sus entradas reconstruidas y sin envolturas del juego abiertas | 908 | 908 | 100.00% de las clases del juego | `████████████████████` |
| **Validación** | Clases del juego validadas | Clases completas cuyas entradas tienen todas `validated=1` | 908 | 908 | 100.00% de las clases completas | `████████████████████` |
| **Entradas del catálogo** | Entradas del juego reconstruidas | Filas de `hooks.csv` con `reversed=1` más las envolturas del juego abiertas, que cuentan como no reconstruidas | 10,711 | 10,711 | 100.00% de las entradas del juego del catálogo | `████████████████████` |
| **Validación** | Entradas del juego validadas | Entradas reconstruidas con evidencia aprobada vinculada (`validated=1`) | 10,711 | 10,711 | 100.00% de las entradas reconstruidas | `████████████████████` |
| **Entradas del catálogo** | Entradas reconstruidas escritas solo en C++ | Entradas reconstruidas cuyo cuerpo fuente no tiene `__asm` ni `naked` (las de fuente no identificada de forma inequívoca no cuentan) | 10,681 | 10,711 | 99.72% de las entradas reconstruidas | `████████████████████` |

**Por qué hay un 91% y un 94%.** No se contradicen: miden cosas distintas. El 90.74% cuenta *funciones* y el 93.64% cuenta *bytes* de código; las funciones que faltan son más pequeñas que las ya reemplazadas, así que quedan muchas funciones pero pocos bytes. Las filas «Funciones» y «Bytes» miden el propio EXE (análisis estático, ver [cobertura real del EXE](#cobertura-exe)) y son la medida del avance hacia la meta; las filas de «Entradas del catálogo», «Clases» y «Validación» miden las filas registradas de `hooks.csv`, que solo conocen una parte del código del juego, por eso su porcentaje es mayor. Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL. La última fila cuenta, de las entradas reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md).

**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md).

Hay 10,711 entradas catalogadas del juego, 0 bloqueadas y 10,711 sin bloqueo. No constituyen un inventario exhaustivo del código del juego; las filas de RenderWare y del CRT del catálogo (83) quedan fuera de este informe. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Las extensiones o adaptaciones NOTSA (4, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

<a id="mediciones"></a>

## 2. Siguientes mediciones

Estado al 16/11/2026. Lo que ya se midió y lo que todavía no tiene un porcentaje útil:

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validación vigente para el árbol actual (validación) | 0/10,711 entradas validadas confirmadas como vigentes (0.00% de las entradas validadas) | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). El 04/11/2026 se repitieron 20 bancos desfasados sin regresión de producción (todos eran bancos desfasados o artefactos del entorno): falta consolidarlos en el registro. |
| Dependencias de funciones originales eliminadas (funciones) | Medido (cota inferior): 273 funciones del juego siguen llamándose directamente desde código del juego ya reemplazado; cierre transitivo: 395 funciones / 76,923 bytes. Ver [Cobertura real del EXE](#cobertura-exe) | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan: lista viva en la [sección 10](AnalisisGrupoA.md). |
| Trabajo real que falta (funciones y bytes) | 1,090 funciones y 191,007 bytes del juego sin reemplazar (9.26% de las funciones y 6.36% de los bytes del juego); grupo A 395 funciones y 76,923 bytes | Reconstruir y validar de menor a mayor coste, empezando por el grupo A ([sección 6](#que-falta)). |
| Coste por categoría de lote (tiempo de reconstrucción y validación) | Medido para las categorías 1, 2, 3 y 5 en los lotes 90A, 90B, 92A, 92B y 92C (tabla de abajo); sin medir la categoría 4 (funciones con FPU) | Cronometrar un lote piloto con FPU (método x87 a 24 bits) y los siguientes lotes de lógica para convertir la estimación en un coste por función. |

Costes medidos en los lotes cerrados (datos al 16/11/2026, [lotes.csv](JuegoExe/lotes.csv)). Cada lote reconstruye en C++ puro funciones que antes se llamaban al EXE por dirección y las valida dentro del juego (pares EXE/DLL con cero diferencias).

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

**¿Quedan envolturas de código del juego en el catálogo?** No. **Envolturas del juego abiertas: 0** (comprobado el 16/11/2026): todas las funciones del juego con hook están reconstruidas en C++.

Las envolturas de biblioteca no cuentan: 211 funciones con envoltura al EXE (RenderWare SDK 140, Controlador D3D9 de RenderWare 58, CRT / runtime estático 11, QuickTime/QTML 2) son RenderWare, controlador D3D9, CRT y QuickTime, que se sustituyen según la [hoja de ruta](RutaExeIndependiente.md) y no son trabajo del juego ([lista](#bibliotecas)). La clasificación sale de la dirección del destino: dentro de 0x401000–0x74FFFF es juego, fuera es biblioteca.

**Lotes en curso:** ninguno registrado para las envolturas del juego.

Cerrar las envolturas del juego no cierra el juego: quedan además las funciones del EXE sin reemplazar (grupos A–D, 1,090 funciones al 16/11/2026; [sección 6](#que-falta)). El plan de trabajo sale de [AnalisisGrupoA.md](AnalisisGrupoA.md) y su [lista completa](JuegoExe/grupoA.csv), y la lista viva está en la [sección 10](AnalisisGrupoA.md):

1. **Grupo A, por categorías y de menor a mayor coste:** helpers triviales y thunks muy llamados (hecho en el lote 90A: `CPad::GetPad` 0x53FB70, `CRGBA::Constructor` 0x7170C0...), pequeñas sin FPU (lote 90B), lógica sin FPU (empezando por las más llamadas y por clases enteras), funciones con FPU con el método x87 a 24 bits, y por último las de 0x300 bytes o más, una por lote (92B, 92C).
2. **Lote piloto de cada categoría** para medir el coste real: ya medidos los de las categorías 1, 2, 3 y 5 (tabla de la sección 2); falta la 4, con FPU.
3. **Revalidación vigente:** repetir los bancos sobre el árbol actual, priorizando los de fuentes cambiadas por lotes posteriores. Motivo: el banco `CVehD` llevaba roto desde el lote 83C (se cambió `m_fSteerAngle` por `m_GasPedal` en `ProcessDrivingAnims`) y no se vio hasta repetirlo.
4. **Después del código del juego:** `librw`, pipelines de SA y puentes `RwD3D9*`, QuickTime, DDraw y CRT, según la [hoja de ruta](RutaExeIndependiente.md); el cambio de método de validación sin el exe como oráculo.

**Validación consolidada parcialmente:** 10711 funciones del juego con evidencia aprobada vinculada, 1429 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias (cifras al 16/11/2026). Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

<a id="vista-general"></a>

## 4. Vista general

Al 16/11/2026. Cada columna indica la dimensión y el denominador de su porcentaje.

| Clases validadas (validación) | Clases completas por validar (validación) | Entradas validadas (validación) | Entradas reconstruidas por validar (validación) | Entradas por reconstruir (entradas del catálogo) |
| :---: | :---: | :---: | :---: | :---: |
| **908 / 908 clases** | **0 / 908 clases** | **10,711 / 10,711 entradas** | **0 / 10,711 entradas** | **0 / 10,711 entradas** |
| 100.00% de las clases completas | 0.00% de las clases completas | 100.00% de las entradas reconstruidas | 0.00% de las entradas reconstruidas | 0.00% de las entradas del juego del catálogo |

Reconstrucción por clases: 908 completas, 0 parciales y 0 sin revertir de 908 clases o grupos.

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego ni el avance sobre todo el EXE (para eso, las filas «Funciones» y «Bytes» de la sección 1). Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="cobertura-exe"></a>

## 5. Cobertura real del EXE (estimación)

Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras filas de la tabla de dimensiones salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático ([MemoryMap](MemoryMap/README.md): 17,737 entradas de función detectadas y 3.72 MB de cuerpos de función), cruzado con el catálogo, el registro de validación y las envolturas pendientes. 10,794 filas reconstruidas del catálogo equivalen a **10,756 funciones distintas del EXE ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo). En esta tabla, «% de funciones» es la parte de las funciones de esa región que ya está en C++ y «% de bytes» la parte de los bytes de código de esa región.

| Región del EXE | Funciones (total de la región) | Bytes de código (total de la región) | Reemplazadas por C++: funciones y % de funciones | Reemplazadas: % de bytes | Funciones de la región llamadas directamente desde código ya reemplazado |
| --- | ---: | ---: | --- | --- | ---: |
| Juego y plataforma (0x401000–0x74FFFF) | 11,776 | 3,005,360 | `██████████████████░░` 10,686 · 90.7% de funciones | `███████████████████░` 93.6% de bytes | 273 |
| RenderWare y D3D (0x750000–0x81FFFF) | 1,889 | 600,971 | `█░░░░░░░░░░░░░░░░░░░` 61 · 3.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.3% de bytes | 229 |
| CRT/MSVC y runtime (0x820000 en adelante) | 4,072 | 115,711 | `░░░░░░░░░░░░░░░░░░░░` 9 · 0.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.2% de bytes | 61 |
| **Total** | **17,737** | **3,722,042** | `████████████░░░░░░░░` 10,756 · 60.6% de funciones | `███████████████░░░░░` 75.7% de bytes | **563** |

**Qué significa para un ejecutable independiente.** No hace falta reconstruir las 17,737 funciones: RenderWare/D3D y el CRT son bibliotecas enlazadas dentro del EXE (5,961 entradas, 19% de los bytes del EXE) que se sustituirían enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego (11,776 funciones, 81% de los bytes del EXE), y de él ya está reemplazado el **94% de los bytes del juego** y el 91% de las funciones del juego: no se contradicen, las funciones que quedan son más pequeñas que la media. Entre las 11,776 funciones del juego hay 1,819 de 16 bytes o menos (stubs, thunks y helpers).

<a id="que-falta"></a>

## 6. Lo que falta del código del juego: 1,090 funciones, 191 KB

Al 16/11/2026: 1,090 funciones (9.26% de las funciones del juego) y 191,007 bytes (6.36% de los bytes del juego) siguen sin reemplazar. Este es el trabajo real que falta del código del juego, lo mida o no el catálogo. Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).

| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |
| --- | ---: | ---: | --- |
| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | 395 | 76,923 | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. Lista viva: [dependencias directas (grupo A)](AnalisisGrupoA.md). |
| C. Llamadas directas solo desde código aún no reemplazado | 268 | 50,572 | Dependen de reconstruir antes A y B; parte puede ser código muerto. |
| D. Sin llamadas directas ni vtable | 231 | 49,642 | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |
| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | 196 | 13,870 | Probablemente necesarios: se ejecutan por despacho virtual. |

Del código del juego ya reemplazado, **273 funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura. No hay envolturas del juego abiertas (sección siguiente).

> [!NOTE]
> **Cotas y límites.** Es un análisis estático: hay 6,004 llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.

<a id="completas"></a>
## 7. 🟢 Completas · 908 clases

Al 16/11/2026: 908 de 908 clases o grupos están completas (todas sus entradas reconstruidas y ninguna envoltura del juego abierta). Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (11) ✅ 11 | [CLinkList&lt;AlphaObjectInfo&gt;](../source/game_sa/VisibilityPlugins.cpp) (3) ✅ 3 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (11) ✅ 11 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (18) ✅ 18 | [CLinkList&lt;CAnimBlendHierarchy*&gt;](../source/game_sa/Animation/AnimManager.cpp) (2) ✅ 2 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (12) ✅ 12 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CLinkList&lt;CCollisionData*&gt;](../source/game_sa/Collision/Collision.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (8) ✅ 8 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CLinkList&lt;CEntity*&gt;](../source/game_sa/Streaming.cpp) (1) ✅ 1 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (6) ✅ 6 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (7) ✅ 7 | [CLinkList&lt;CPed*&gt;](../source/game_sa/VisibilityPlugins.cpp) (2) ✅ 2 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (9) ✅ 9 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (10) ✅ 10 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (15) ✅ 15 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (11) ✅ 11 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (8) ✅ 8 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (6) ✅ 6 | [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (4) ✅ 4 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (10) ✅ 10 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (6) ✅ 6 | [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (4) ✅ 4 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (9) ✅ 9 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CMaths](../source/game_sa/Maths.cpp) (6) ✅ 6 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (13) ✅ 13 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CMatrix](../source/game_sa/Core/Matrix.cpp) (46) ✅ 46 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (8) ✅ 8 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (10) ✅ 10 | [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (9) ✅ 9 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (16) ✅ 16 | [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (11) ✅ 11 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (8) ✅ 8 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (6) ✅ 6 | [CMBlur](../source/game_sa/MBlur.cpp) (5) ✅ 5 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (13) ✅ 13 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [CMemoryHeap](../source/game_sa/Memory/MemoryHeap.cpp) (1) ✅ 1 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (12) ✅ 12 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (55) ✅ 55 | [CMemoryMgr](../source/game_sa/MemoryMgr.cpp) (10) ✅ 10 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (9) ✅ 9 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [CMentalState](../source/game_sa/MentalHealth.cpp) (1) ✅ 1 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (9) ✅ 9 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (8) ✅ 8 | [CMenuManager](../source/game_sa/MenuManager.cpp) (62) ✅ 62 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (7) ✅ 7 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (8) ✅ 8 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (11) ✅ 11 | [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (9) ✅ 9 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (22) ✅ 22 | [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (5) ✅ 5 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [CMissionCleanup](../source/game_sa/MissionCleanup.cpp) (3) ✅ 3 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (5) ✅ 5 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (4) ✅ 4 | [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (9) ✅ 9 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (29) ✅ 29 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (5) ✅ 5 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (7) ✅ 7 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (13) ✅ 13 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (5) ✅ 5 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (8) ✅ 8 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (8) ✅ 8 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (14) ✅ 14 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexUseMobilePhone](../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp) (1) ✅ 1 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (14) ✅ 14 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (12) ✅ 12 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (8) ✅ 8 | [CNearbyFireScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (8) ✅ 8 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (22) ✅ 22 | [CNodeAddress](../source/game_sa/NodeAddress.cpp) (3) ✅ 3 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (9) ✅ 9 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (12) ✅ 12 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (3) ✅ 3 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (8) ✅ 8 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (59) ✅ 59 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (13) ✅ 13 |
| [CAEPedSpeechSlot](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (1) ✅ 1 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (56) ✅ 56 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (6) ✅ 6 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (7) ✅ 7 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 17 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (2) ✅ 2 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (9) ✅ 9 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 43 | [CObjectPotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (8) ✅ 8 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (20) ✅ 20 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (8) ✅ 8 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (11) ✅ 11 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexWaitForPed](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) (3) ✅ 3 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (19) ✅ 19 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (10) ✅ 10 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (15) ✅ 15 | [COctTree](../source/game_sa/Core/OctTree.cpp) (9) ✅ 9 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (14) ✅ 14 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (11) ✅ 11 | [COctTreeBase](../source/game_sa/Core/OctTreeBase.cpp) (1) ✅ 1 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (14) ✅ 14 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (25) ✅ 25 | [Common](../source/game_sa/common.cpp) (26) ✅ 26 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (9) ✅ 9 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (3) ✅ 3 | [CompressedVector](../source/game_sa/CompressedVector.cpp) (2) ✅ 2 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (11) ✅ 11 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (16) ✅ 16 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (18) ✅ 18 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (3) ✅ 3 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (9) ✅ 9 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (13) ✅ 13 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [COnscreenTimerEntry](../source/game_sa/OnscreenTimer.cpp) (1) ✅ 1 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (7) ✅ 7 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (10) ✅ 10 | [CPad](../source/game_sa/Pad.cpp) (98) ✅ 98 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (7) ✅ 7 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (15) ✅ 15 | [CPathFind](../source/game_sa/PathFind.cpp) (71) ✅ 71 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (6) ✅ 6 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (107) ✅ 107 | [CPathNode](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (6) ✅ 6 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (14) ✅ 14 | [CPatrolRoute](../source/game_sa/PatrolRoute.cpp) (5) ✅ 5 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (7) ✅ 7 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (9) ✅ 9 | [CPatrolRoutePool](../source/game_sa/Pools/PatrolRoutePool.h) (1) ✅ 1 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (12) ✅ 12 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (10) ✅ 10 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (155) ✅ 155 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (8) ✅ 8 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedAcquaintanceScanner](../source/game_sa/Events/EventScanner.cpp) (4) ✅ 4 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (10) ✅ 10 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (6) ✅ 6 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (44) ✅ 44 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (11) ✅ 11 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (13) ✅ 13 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (17) ✅ 17 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (9) ✅ 9 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (6) ✅ 6 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (9) ✅ 9 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (7) ✅ 7 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (23) ✅ 23 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (9) ✅ 9 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (8) ✅ 8 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (6) ✅ 6 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (8) ✅ 8 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (7) ✅ 7 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 38 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (8) ✅ 8 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (10) ✅ 10 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (17) ✅ 17 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (3) ✅ 3 | [CTaskInteriorSitInChair](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp) (5) ✅ 5 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (8) ✅ 8 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (3) ✅ 3 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (9) ✅ 9 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (32) ✅ 32 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (3) ✅ 3 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (8) ✅ 8 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (13) ✅ 13 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (3) ✅ 3 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (20) ✅ 20 |
| [CAttractorScanner](../source/game_sa/AttractorScanner.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocators](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h) (1) ✅ 1 | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (83) ✅ 83 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (3) ✅ 3 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (3) ✅ 3 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (8) ✅ 8 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (3) ✅ 3 | [CTaskSimple](../source/game_sa/Tasks/Task.cpp) · [archivo 2](../source/game_sa/Tasks/TaskSimple.cpp) (4) ✅ 4 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (104) ✅ 104 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (31) ✅ 31 | [CTaskSimpleAbseil](../source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp) (2) ✅ 2 |
| [CAutoPilot](../source/game_sa/AutoPilot.cpp) (2) ✅ 2 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (19) ✅ 19 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (8) ✅ 8 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (37) ✅ 37 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (7) ✅ 7 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (62) ✅ 62 | [CPedGroups](../source/game_sa/PedGroups.cpp) (12) ✅ 12 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (4) ✅ 4 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (8) ✅ 8 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (13) ✅ 13 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (61) ✅ 61 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (8) ✅ 8 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (24) ✅ 24 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (9) ✅ 9 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPedList](../source/game_sa/PedList.cpp) (9) ✅ 9 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (11) ✅ 11 |
| [CBoundingBox](../source/game_sa/Collision/BoundingBox.cpp) (5) ✅ 5 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (9) ✅ 9 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (11) ✅ 11 |
| [CBox](../source/game_sa/Collision/Box.cpp) (2) ✅ 2 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (4) ✅ 4 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (11) ✅ 11 |
| [CBridge](../source/game_sa/Bridge.cpp) (5) ✅ 5 | [CPedPool](../source/game_sa/Pools/PedPool.h) (5) ✅ 5 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (16) ✅ 16 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPedSaveStructure](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (6) ✅ 6 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (6) ✅ 6 | [CPedScanner](../source/game_sa/PedScanner.cpp) (1) ✅ 1 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (11) ✅ 11 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleCarForcePedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp) (7) ✅ 7 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (10) ✅ 10 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (7) ✅ 7 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (11) ✅ 11 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (10) ✅ 10 | [CPedStats](../source/game_sa/PedStats.cpp) (5) ✅ 5 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (8) ✅ 8 |
| [CCam](../source/game_sa/Cam.cpp) (58) ✅ 58 | [CPedStuckChecker](../source/game_sa/PedStuckChecker.cpp) (1) ✅ 1 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (11) ✅ 11 |
| [CCamera](../source/game_sa/Camera.cpp) (124) ✅ 124 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (11) ✅ 11 |
| [CCarAI](../source/game_sa/CarAI.cpp) (26) ✅ 26 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (10) ✅ 10 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (112) ✅ 112 | [CPedType](../source/game_sa/PedType.cpp) (14) ✅ 14 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (7) ✅ 7 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 32 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (61) ✅ 61 | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (7) ✅ 7 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPickup](../source/game_sa/Pickup.cpp) (12) ✅ 12 | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (8) ✅ 8 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (9) ✅ 9 | [CPickups](../source/game_sa/Pickups.cpp) (38) ✅ 38 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (7) ✅ 7 |
| [CCarPathLink](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (21) ✅ 21 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (7) ✅ 7 |
| [CCheat](../source/game_sa/Cheat.cpp) (74) ✅ 74 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (10) ✅ 10 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (26) ✅ 26 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (11) ✅ 11 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CPlaneBanners](../source/game_sa/PlaneBanners.cpp) (1) ✅ 1 | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (11) ✅ 11 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (4) ✅ 4 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (8) ✅ 8 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (8) ✅ 8 |
| [CClothes](../source/game_sa/Clothes.cpp) (12) ✅ 12 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (5) ✅ 5 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (25) ✅ 25 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (7) ✅ 7 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (20) ✅ 20 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (7) ✅ 7 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (26) ✅ 26 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (19) ✅ 19 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (31) ✅ 31 | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (5) ✅ 5 |
| [CColBox](../source/game_sa/Collision/ColBox.cpp) (2) ✅ 2 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (70) ✅ 70 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (7) ✅ 7 |
| [CColDisk](../source/game_sa/Collision/ColDisk.cpp) (1) ✅ 1 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (5) ✅ 5 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (7) ✅ 7 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (2) ✅ 2 | [CPlayerRelationshipRecorder](../source/game_sa/PlayerRelationshipRecorder.cpp) (6) ✅ 6 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (11) ✅ 11 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (62) ✅ 62 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (6) ✅ 6 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (10) ✅ 10 | [CPointLights](../source/game_sa/PointLights.cpp) (7) ✅ 7 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (7) ✅ 7 |
| [CCollisionEventScanner](../source/game_sa/Collision/CollisionEventScanner.cpp) (1) ✅ 1 | [CPointList](../source/game_sa/PointList.cpp) (2) ✅ 2 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (6) ✅ 6 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CPointRoute](../source/game_sa/PedGeometryAnalyser.cpp) (6) ✅ 6 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (7) ✅ 7 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (12) ✅ 12 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (18) ✅ 18 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CPools](../source/game_sa/Pools.cpp) (21) ✅ 21 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (7) ✅ 7 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleDuckWhileShotsWhizzing](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp) (7) ✅ 7 |
| [CColPoint](../source/game_sa/Collision/ColPoint.cpp) (2) ✅ 2 | [CPopulation](../source/game_sa/Population.cpp) (68) ✅ 68 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (9) ✅ 9 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 38 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (9) ✅ 9 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (29) ✅ 29 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (8) ✅ 8 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CProjectile](../source/game_sa/Entity/Object/Projectile.cpp) (3) ✅ 3 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (9) ✅ 9 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 | [CTaskSimpleFight](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) (27) ✅ 27 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (5) ✅ 5 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (8) ✅ 8 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (4) ✅ 4 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (5) ✅ 5 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (14) ✅ 14 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (13) ✅ 13 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (14) ✅ 14 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (7) ✅ 7 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (11) ✅ 11 | [CQuaternion](../source/game_sa/Core/Quaternion.cpp) (10) ✅ 10 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (8) ✅ 8 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (8) ✅ 8 | [CRadar](../source/game_sa/Radar.cpp) (66) ✅ 66 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (6) ✅ 6 |
| [CCoronas](../source/game_sa/Coronas.cpp) (12) ✅ 12 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (11) ✅ 11 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (6) ✅ 6 |
| [CCover](../source/game_sa/Cover.cpp) (16) ✅ 16 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (9) ✅ 9 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (12) ✅ 12 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CRect](../source/game_sa/Core/Rect.cpp) (9) ✅ 9 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (7) ✅ 7 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (5) ✅ 5 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (4) ✅ 4 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (4) ✅ 4 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CRemote](../source/game_sa/Remote.cpp) (3) ✅ 3 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (4) ✅ 4 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CRenderer](../source/game_sa/Renderer.cpp) (33) ✅ 33 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (4) ✅ 4 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CReplay](../source/game_sa/Replay.cpp) (48) ✅ 48 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (4) ✅ 4 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (4) ✅ 4 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (18) ✅ 18 | [CRGBA](../source/game_sa/RGBA.cpp) (2) ✅ 2 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (4) ✅ 4 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CRideAnimData](../source/game_sa/RideAnimData.cpp) (2) ✅ 2 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (4) ✅ 4 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 8 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (4) ✅ 4 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (37) ✅ 37 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 7 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (8) ✅ 8 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (16) ✅ 16 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (4) ✅ 4 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (10) ✅ 10 | [CRubbish](../source/game_sa/Rubbish.cpp) (1) ✅ 1 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (17) ✅ 17 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (37) ✅ 37 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (39) ✅ 39 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (7) ✅ 7 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (10) ✅ 10 | [CSaveGarage](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (11) ✅ 11 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (8) ✅ 8 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (6) ✅ 6 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (9) ✅ 9 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (9) ✅ 9 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (7) ✅ 7 |
| [CData](../source/game_sa/Text/Text.cpp) (3) ✅ 3 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (8) ✅ 8 |
| [CDebug](../source/game_sa/Debug.cpp) (2) ✅ 2 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (11) ✅ 11 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (15) ✅ 15 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (7) ✅ 7 | [CSearchLight](../source/game_sa/SearchLight.cpp) (1) ✅ 1 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (11) ✅ 11 |
| [CDecisionMaker](../source/game_sa/DecisionMakers/DecisionMaker.cpp) (3) ✅ 3 | [CSector](../source/game_sa/Sector.cpp) (1) ✅ 1 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (9) ✅ 9 |
| [CDecisionMakerTypes](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) (17) ✅ 17 | [CSetPiece](../source/game_sa/SetPiece.cpp) (7) ✅ 7 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (9) ✅ 9 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CDirectory](../source/game_sa/Directory.cpp) (11) ✅ 11 | [CSexyPedScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (5) ✅ 5 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (20) ✅ 20 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (5) ✅ 5 |
| [CDraw](../source/game_sa/Draw.cpp) (3) ✅ 3 | [CShadows](../source/game_sa/Shadows.cpp) (30) ✅ 30 | [CTaskSimpleOnEscalator](../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp) (5) ✅ 5 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (11) ✅ 11 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (7) ✅ 7 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (6) ✅ 6 | [CShopping](../source/game_sa/Shopping.cpp) (33) ✅ 33 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (10) ✅ 10 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (6) ✅ 6 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (5) ✅ 5 |
| [CDummyPed](../source/game_sa/Entity/Dummy/DummyPed.cpp) (3) ✅ 3 | [CSimpleTransform](../source/game_sa/SimpleTransform.cpp) (3) ✅ 3 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (7) ✅ 7 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (13) ✅ 13 |
| [CEmergencyPed](../source/game_sa/Entity/Ped/EmergencyPed.cpp) (4) ✅ 4 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (3) ✅ 3 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (73) ✅ 73 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (4) ✅ 4 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculator](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp) (4) ✅ 4 | [CSphere](../source/game_sa/Collision/Sphere.cpp) (1) ✅ 1 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (10) ✅ 10 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (8) ✅ 8 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (35) ✅ 35 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (5) ✅ 5 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (2) ✅ 2 | [CStats](../source/game_sa/Stats.cpp) (52) ✅ 52 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (7) ✅ 7 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (13) ✅ 13 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (4) ✅ 4 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (6) ✅ 6 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (28) ✅ 28 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (14) ✅ 14 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (7) ✅ 7 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (1) ✅ 1 | [CStoredCar](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (7) ✅ 7 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (7) ✅ 7 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CStreaming](../source/game_sa/Streaming.cpp) (108) ✅ 108 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (9) ✅ 9 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (9) ✅ 9 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (9) ✅ 9 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (8) ✅ 8 |
| [CEvent](../source/game_sa/Events/Event.cpp) (16) ✅ 16 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (13) ✅ 13 | [CTaskSimpleSlideToCoord](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp) (4) ✅ 4 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (6) ✅ 6 | [CTagManager](../source/game_sa/TagManager.cpp) (20) ✅ 20 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (7) ✅ 7 |
| [CEventAcquaintancePedDislike](../source/game_sa/Events/EventAcquaintancePedDislike.cpp) (3) ✅ 3 | [CTask](../source/game_sa/Tasks/Task.cpp) (8) ✅ 8 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (9) ✅ 9 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (9) ✅ 9 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (7) ✅ 7 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (9) ✅ 9 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (8) ✅ 8 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (6) ✅ 6 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (15) ✅ 15 |
| [CEventAcquaintancePedLike](../source/game_sa/Events/EventAcquaintancePedLike.cpp) (3) ✅ 3 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (5) ✅ 5 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (6) ✅ 6 |
| [CEventAcquaintancePedRespect](../source/game_sa/Events/EventAcquaintancePedRespect.cpp) (3) ✅ 3 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (7) ✅ 7 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (10) ✅ 10 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (7) ✅ 7 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (4) ✅ 4 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (8) ✅ 8 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (9) ✅ 9 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (6) ✅ 6 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (7) ✅ 7 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (11) ✅ 11 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (6) ✅ 6 | [CTaskSimpleTriggerLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp) (7) ✅ 7 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (6) ✅ 6 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (7) ✅ 7 |
| [CEventChatPartner](../source/game_sa/Events/EventChatPartner.cpp) (4) ✅ 4 | [CTaskCategories](../source/game_sa/Tasks/TaskCategories.cpp) (3) ✅ 3 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (7) ✅ 7 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (6) ✅ 6 | [CTaskComplex](../source/game_sa/Tasks/Task.cpp) · [archivo 2](../source/game_sa/Tasks/TaskComplex.cpp) (7) ✅ 7 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (5) ✅ 5 |
| [CEventCreatePartnerTask](../source/game_sa/Events/EventCreatePartnerTask.cpp) (4) ✅ 4 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (10) ✅ 10 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (22) ✅ 22 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (25) ✅ 25 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (8) ✅ 8 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (7) ✅ 7 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (9) ✅ 9 | [CTaskComplexAvoidEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp) (6) ✅ 6 | [CTaskSimpleWaitForPizza](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp) (7) ✅ 7 |
| [CEventDeadPed](../source/game_sa/Events/EventDeadPed.cpp) (2) ✅ 2 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (16) ✅ 16 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (7) ✅ 7 |
| [CEventDeath](../source/game_sa/Events/EventDeath.cpp) (1) ✅ 1 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (4) ✅ 4 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (8) ✅ 8 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (11) ✅ 11 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (7) ✅ 7 |
| [CEventDraggedOutCar](../source/game_sa/Events/EventDraggedOutCar.cpp) (4) ✅ 4 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (11) ✅ 11 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (7) ✅ 7 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (13) ✅ 13 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (8) ✅ 8 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (2) ✅ 2 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (9) ✅ 9 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (13) ✅ 13 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (6) ✅ 6 |
| [CEventFireNearby](../source/game_sa/Events/EventFireNearby.cpp) (3) ✅ 3 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (6) ✅ 6 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (5) ✅ 5 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEventGotKnockedOverByCar](../source/game_sa/Events/EventGotKnockedOverByCar.cpp) (2) ✅ 2 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (14) ✅ 14 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (7) ✅ 7 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (72) ✅ 72 |
| [CEventGroupEvent](../source/game_sa/Events/EventGroupEvent.cpp) (3) ✅ 3 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (12) ✅ 12 | [CTheZones](../source/game_sa/TheZones.cpp) (23) ✅ 23 |
| [CEventGunAimedAt](../source/game_sa/Events/EventGunAimedAt.cpp) (6) ✅ 6 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (10) ✅ 10 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (14) ✅ 14 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (8) ✅ 8 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (8) ✅ 8 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (5) ✅ 5 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (4) ✅ 4 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (10) ✅ 10 | [CTimer](../source/game_sa/Timer.cpp) (16) ✅ 16 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (12) ✅ 12 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (10) ✅ 10 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEventHealthLow](../source/game_sa/Events/EventHealthLow.cpp) (2) ✅ 2 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (10) ✅ 10 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (16) ✅ 16 |
| [CEventHealthReallyLow](../source/game_sa/Events/EventHealthReallyLow.cpp) (4) ✅ 4 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (11) ✅ 11 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (43) ✅ 43 |
| [CEventHighAngerAtPlayer](../source/game_sa/Events/EventHighAngerAtPlayer.cpp) (4) ✅ 4 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (11) ✅ 11 | [CTrainNode](../source/game_sa/TrainNode.cpp) (2) ✅ 2 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (7) ✅ 7 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (10) ✅ 10 | [cTransmission](../source/game_sa/cTransmission.cpp) (5) ✅ 5 |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (7) ✅ 7 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (11) ✅ 11 | [CTxdStore](../source/game_sa/TxdStore.cpp) (33) ✅ 33 |
| [CEventInteriorUseInfo](../source/game_sa/Events/EventInteriorUseInfo.cpp) (5) ✅ 5 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (8) ✅ 8 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventInWater](../source/game_sa/Events/EventInWater.cpp) (4) ✅ 4 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (7) ✅ 7 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (18) ✅ 18 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (8) ✅ 8 | [CVector](../source/game_sa/Core/Vector.cpp) (23) ✅ 23 |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (11) ✅ 11 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (8) ✅ 8 | [CVector2D](../source/game_sa/Core/Vector.cpp) · [archivo 2](../source/game_sa/Core/Vector2D.cpp) (2) ✅ 2 |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (12) ✅ 12 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (10) ✅ 10 | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (170) ✅ 170 |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (10) ✅ 10 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (8) ✅ 8 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (6) ✅ 6 |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (10) ✅ 10 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (9) ✅ 9 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (6) ✅ 6 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (9) ✅ 9 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (17) ✅ 17 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (80) ✅ 80 |
| [CEventLowAngerAtPlayer](../source/game_sa/Events/EventLowAngerAtPlayer.cpp) (4) ✅ 4 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (5) ✅ 5 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (3) ✅ 3 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (8) ✅ 8 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (10) ✅ 10 | [CVehiclePotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (9) ✅ 9 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (5) ✅ 5 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (9) ✅ 9 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (10) ✅ 10 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (2) ✅ 2 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (9) ✅ 9 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (9) ✅ 9 | [CVehicleScanner](../source/game_sa/VehicleScanner.cpp) (1) ✅ 1 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (8) ✅ 8 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (9) ✅ 9 | [CVehicleStructure](../source/game_sa/Models/VehicleModelInfo.cpp) (5) ✅ 5 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (5) ✅ 5 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (10) ✅ 10 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) ✅ 84 |
| [CEventPedEnteredMyVehicle](../source/game_sa/Events/EventPedEnteredMyVehicle.cpp) (4) ✅ 4 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (9) ✅ 9 | [CWanted](../source/game_sa/Wanted.cpp) (35) ✅ 35 |
| [CEventPedToChase](../source/game_sa/Events/EventPedToChase.cpp) (4) ✅ 4 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (9) ✅ 9 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventPedToFlee](../source/game_sa/Events/EventPedToFlee.cpp) (4) ✅ 4 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (11) ✅ 11 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (5) ✅ 5 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (12) ✅ 12 | [CWaterLevel](../source/game_sa/WaterLevel.cpp) (48) ✅ 48 |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (8) ✅ 8 | [CTaskComplexFallAndStayDown](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp) (11) ✅ 11 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 41 |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (7) ✅ 7 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (9) ✅ 9 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (10) ✅ 10 |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (6) ✅ 6 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (9) ✅ 9 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (16) ✅ 16 |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (9) ✅ 9 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (4) ✅ 4 |
| [CEventPotentialGetRunOver](../source/game_sa/Events/EventPotentialGetRunOver.cpp) (2) ✅ 2 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (12) ✅ 12 | [CWeather](../source/game_sa/Weather.cpp) (14) ✅ 14 |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (6) ✅ 6 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (9) ✅ 9 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (7) ✅ 7 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (9) ✅ 9 | [CWinsockLink](../source/game_sa/WinsockLink.cpp) (2) ✅ 2 |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (8) ✅ 8 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (11) ✅ 11 | [CWorld](../source/game_sa/World.cpp) (96) ✅ 96 |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (7) ✅ 7 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (20) ✅ 20 | [CWorldScan](../source/game_sa/Renderer.cpp) (2) ✅ 2 |
| [CEventRevived](../source/game_sa/Events/EventRevived.cpp) (2) ✅ 2 | [CTaskComplexFollowPatrolRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp) (1) ✅ 1 | [CZone](../source/game_sa/Zone.cpp) (1) ✅ 1 |
| [CEventScanner](../source/game_sa/Events/EventScanner.cpp) (3) ✅ 3 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (10) ✅ 10 | [CZoneDef](../source/game_sa/CullZones.cpp) (1) ✅ 1 |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (10) ✅ 10 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (15) ✅ 15 | [D3DIndexDataBuffer](../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp) (5) ✅ 5 |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (5) ✅ 5 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (11) ✅ 11 | [D3DResourceSystem](../source/game_sa/RenderWare/D3DResourceSystem.cpp) (16) ✅ 16 |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (2) ✅ 2 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (9) ✅ 9 | [D3DTextureBuffer](../source/game_sa/RenderWare/D3DTextureBuffer.cpp) (8) ✅ 8 |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (8) ✅ 8 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (16) ✅ 16 | [Furniture_c](../source/game_sa/Interior/Furniture_c.cpp) (1) ✅ 1 |
| [CEventSexyPed](../source/game_sa/Events/EventSexyPed.cpp) (4) ✅ 4 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (9) ✅ 9 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (7) ✅ 7 |
| [CEventSexyVehicle](../source/game_sa/Events/EventSexyVehicle.cpp) (5) ✅ 5 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (8) ✅ 8 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (9) ✅ 9 |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (11) ✅ 11 | [CTaskComplexGoPickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp) (8) ✅ 8 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (6) ✅ 6 |
| [CEventSource](../source/game_sa/Events/EventSource.cpp) (1) ✅ 1 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (9) ✅ 9 | [Fx_c](../source/game_sa/Fx/Fx.cpp) (36) ✅ 36 |
| [CEventSpecial](../source/game_sa/Events/EventSpecial.cpp) (1) ✅ 1 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (10) ✅ 10 | [FxEmitter_c](../source/game_sa/Fx/FxEmitter.cpp) (4) ✅ 4 |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (8) ✅ 8 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (12) ✅ 12 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (9) ✅ 9 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (5) ✅ 5 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (10) ✅ 10 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (2) ✅ 2 |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (9) ✅ 9 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (9) ✅ 9 | [FxFrustumInfo_c](../source/game_sa/Fx/FxFrustumInfo.cpp) (1) ✅ 1 |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (9) ✅ 9 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (5) ✅ 5 |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (9) ✅ 9 | [FxInterpInfo255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp) (1) ✅ 1 |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (7) ✅ 7 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (6) ✅ 6 | [FxInterpInfo32_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp) (1) ✅ 1 |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (10) ✅ 10 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (9) ✅ 9 | [FxInterpInfoFloat_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp) (1) ✅ 1 |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (4) ✅ 4 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (5) ✅ 5 | [FxInterpInfoU255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp) (1) ✅ 1 |
| [CEventVehicleThreat](../source/game_sa/Events/EventVehicleThreat.cpp) (4) ✅ 4 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (9) ✅ 9 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (8) ✅ 8 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (12) ✅ 12 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (7) ✅ 7 |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (8) ✅ 8 | [FxPrim_c](../source/game_sa/Fx/FxPrim.cpp) (3) ✅ 3 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (6) ✅ 6 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (4) ✅ 4 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (50) ✅ 50 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (10) ✅ 10 | [FxPrtMult_c](../source/game_sa/Fx/FxPrtMult.cpp) (3) ✅ 3 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (8) ✅ 8 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (5) ✅ 5 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (7) ✅ 7 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (39) ✅ 39 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (10) ✅ 10 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CFont](../source/game_sa/Font.cpp) (40) ✅ 40 | [CTaskComplexKillAllThreats](../source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp) (9) ✅ 9 | [Game](../source/app/app_game.cpp) (12) ✅ 12 |
| [CFontChar](../source/game_sa/Font.cpp) (1) ✅ 1 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (12) ✅ 12 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CFormation](../source/game_sa/Formation.cpp) (10) ✅ 10 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (8) ✅ 8 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (17) ✅ 17 |
| [CGame](../source/game_sa/Game.cpp) (25) ✅ 25 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (10) ✅ 10 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (10) ✅ 10 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (10) ✅ 10 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (53) ✅ 53 |
| [CGamma](../source/game_sa/Gamma.cpp) (2) ✅ 2 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (12) ✅ 12 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (24) ✅ 24 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (10) ✅ 10 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (23) ✅ 23 |
| [CGangWars](../source/game_sa/GangWars.cpp) (36) ✅ 36 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (7) ✅ 7 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (6) ✅ 6 |
| [CGarage](../source/game_sa/Garage.cpp) (32) ✅ 32 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (8) ✅ 8 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CGarages](../source/game_sa/Garages.cpp) (30) ✅ 30 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (9) ✅ 9 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (3) ✅ 3 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexLeaveCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp) (14) ✅ 14 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (20) ✅ 20 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (6) ✅ 6 | [Particle_c](../source/game_sa/Fx/Particle.cpp) (1) ✅ 1 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (9) ✅ 9 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (9) ✅ 9 | [ProcObjectListItem](../source/game_sa/Plant/ProcObjectMan.cpp) (1) ✅ 1 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (11) ✅ 11 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (10) ✅ 10 |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 32 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (10) ✅ 10 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (7) ✅ 7 |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (13) ✅ 13 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (9) ✅ 9 | [Ps](../source/app/platform/win/WinPs.cpp) (23) ✅ 23 |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (5) ✅ 5 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (9) ✅ 9 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (4) ✅ 4 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp) (10) ✅ 10 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (47) ✅ 47 |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (26) ✅ 26 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (10) ✅ 10 | [RpAnisot](../source/game_sa/RenderWare/rw/rpanisot.cpp) (3) ✅ 3 |
| [CHud](../source/game_sa/Hud.cpp) (39) ✅ 39 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (5) ✅ 5 | [Rs](../source/app/platform/platform.cpp) (33) ✅ 33 |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (5) ✅ 5 | [RwHelper](../source/game_sa/RwHelper.cpp) (42) ✅ 42 |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (15) ✅ 15 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (11) ✅ 11 | [Securom](../source/app/securom/Securom.cpp) (4) ✅ 4 |
| [CInformFriendsEvent](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (7) ✅ 7 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (48) ✅ 48 |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (8) ✅ 8 | [tColLighting](../source/game_sa/Collision/ColPoint.cpp) (1) ✅ 1 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (5) ✅ 5 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (8) ✅ 8 | [TList_c](../source/game_sa/Core/List_c.cpp) (3) ✅ 3 |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (6) ✅ 6 | [tUserList](../source/game_sa/Scripts/Scripted2dEffects.cpp) (2) ✅ 2 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (10) ✅ 10 | [UserTracksLegacyVector](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (1) ✅ 1 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (10) ✅ 10 | [VectorConstructorIterator](../source/game_sa/RuntimeHelpers.cpp) (1) ✅ 1 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (4) ✅ 4 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (14) ✅ 14 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (4) ✅ 4 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (12) ✅ 12 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (7) ✅ 7 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (5) ✅ 5 | [CTaskComplexReactToAttack](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp) (4) ✅ 4 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (4) ✅ 4 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (9) ✅ 9 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (7) ✅ 7 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (10) ✅ 10 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (7) ✅ 7 |
| [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (10) ✅ 10 |  |

</details>

<a id="parciales"></a>
## 8. 🟡 Parciales · 0 clases del catálogo + 68 clases/módulos con funciones del EXE pendientes

Orden de mayor a menor tamaño de las envolturas al EXE pendientes (suma de bytes de sus cuerpos; a igualdad, alfabético). «Función más grande» y «Tamaño» (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B) indican cuánto pesa la función más difícil de la clase. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas del juego al EXE», que cuenta solo código del juego; las de biblioteca no entran): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| *(ninguna clase parcial al 16/11/2026)* | | | | | | | | | |

### Clases con funciones del EXE aún sin reemplazar · 68 clases / módulos

Todas estas clases o módulos son parciales: conservan funciones del EXE en 0x401000–0x74FFFF sin reemplazar por C++ (grupos A–D del mapa del EXE y envolturas; 1,090 funciones). La clase se atribuye por cercanía de direcciones y puede ser errónea cuando su confianza es baja. Lista completa por función: [pendientes-por-clase.md](JuegoExe/pendientes-por-clase.md).

| Clase o módulo | Funciones pendientes | Bytes pendientes | Función más grande (B) | Tamaño |
| --- | ---: | ---: | ---: | --- |
| CCam | **1** | 12 | 12 | pequeña |
| CTaskComplexFallAndGetUp | **1** | 30 | 30 | pequeña |
| CTaskComplexSunbathe | **3** | 90 | 30 | pequeña |
| CAudioZones | **1** | 139 | 139 | mediana |
| CEventSeenCop | **6** | 164 | 88 | pequeña |
| CClothesBuilder | **1** | 203 | 203 | mediana |
| IKChain_c | **2** | 205 | 103 | mediana |
| CWaterLevel | **1** | 208 | 208 | mediana |
| WaterCreature_c | **2** | 211 | 106 | mediana |
| CTaskSimpleBeHit | **3** | 217 | 121 | mediana |
| Rs | **1** | 410 | 410 | mediana |
| CQuadTreeNode<T> | **10** | 720 | 206 | mediana |
| CObject | **7** | 728 | 480 | mediana |
| CEventScriptCommand | **11** | 766 | 77 | pequeña |
| CTaskSimpleThrowProjectile | **11** | 767 | 429 | mediana |
| CTaskComplexUseWaterCannon | **15** | 769 | 180 | mediana |
| CTrafficLights | **5** | 770 | 493 | mediana |
| CTaskComplexWaitForPed | **6** | 772 | 528 | grande |
| CVehicleModelInfo | **8** | 777 | 343 | mediana |
| CAttractorScanner | **2** | 793 | 429 | mediana |
| CTaskComplexWander | **4** | 797 | 448 | mediana |
| CPed | **2** | 804 | 404 | mediana |
| CModelInfo | **22** | 814 | 92 | pequeña |
| CEventDamage | **12** | 836 | 76 | pequeña |
| CTaskComplexInvestigateDisturbance | **7** | 858 | 348 | mediana |
| CVehicle | **33** | 861 | 227 | mediana |
| CVehicleSaveStructure | **4** | 874 | 282 | mediana |
| Ps | **6** | 876 | 327 | mediana |
| CPad | **11** | 895 | 322 | mediana |
| CEntityScanner | **5** | 958 | 422 | mediana |
| CTaskComplexMedicTreatInjuredPed | **13** | 974 | 259 | mediana |
| CTaskComplexGoToPointAndStandStill | **8** | 977 | 661 | grande |
| CTaskComplexStuckInAir | **10** | 977 | 141 | mediana |
| CPedAttractor | **12** | 1,009 | 217 | mediana |
| CTrain | **6** | 1,076 | 719 | grande |
| CTaskComplexAvoidOtherPedWhileWandering | **7** | 1,125 | 455 | mediana |
| CTaskComplexSeekEntityAnyMeansXYOffset | **4** | 1,138 | 981 | grande |
| CPlaceable | **24** | 1,138 | 77 | pequeña |
| CEventVehicleOnFire | **17** | 1,145 | 111 | mediana |
| CEventVehicleCollision | **15** | 1,149 | 126 | mediana |
| CTaskSimpleRunTimedAnim | **12** | 1,175 | 226 | mediana |
| Interior_c | **4** | 1,200 | 591 | grande |
| CTaskInteriorUseInfo | **3** | 1,483 | 1,423 | grande |
| CPedIntelligencePool | **6** | 1,552 | 776 | grande |
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
| **TOTAL (68 clases / módulos)** | **1,090** | **191,007** | 8,957 | |

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


<a id="renderware-d3d"></a>
### RenderWare y D3D por clase o módulo (0x750000–0x81FFFF)

Las 1,889 funciones de esta región, agrupadas por la clase o módulo al que pertenecen. El EXE no trae símbolos, así que la clase sale de nombres ya conocidos (exacta), de cadenas del `.rdata` que referencia cada función (cadenas) o de la vecindad de direcciones y del grafo de llamadas (estimada). Detalle por función en [docs/RenderWareExe](RenderWareExe/README.md) y lista de pendientes en [pendientes-por-clase](RenderWareExe/pendientes-por-clase.md).

| Clase / módulo | Funciones | Reemplazadas | Envolturas | Faltan | Bytes que faltan | Clase exacta / por cadenas / estimada |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| D3DX9 (compilador de shaders) | 878 | 0 | 0 | 878 | 323,180 | 0 / 120 / 758 |
| RwCore | 324 | 27 | 67 | 230 | 67,628 | 174 / 0 / 150 |
| libpng + zlib | 182 | 0 | 1 | 181 | 81,023 | 2 / 28 / 152 |
| RpWorld | 163 | 11 | 45 | 107 | 20,781 | 72 / 0 / 91 |
| RpMatFX | 92 | 0 | 5 | 87 | 13,629 | 9 / 0 / 83 |
| RwPlCore | 129 | 14 | 41 | 74 | 14,172 | 87 / 0 / 42 |
| RpSkin | 27 | 1 | 3 | 23 | 3,752 | 14 / 0 / 13 |
| RpHAnim | 24 | 1 | 2 | 21 | 8,033 | 7 / 0 / 17 |
| RtQuat | 20 | 0 | 0 | 20 | 9,888 | 3 / 0 / 17 |
| RpUVAnim | 22 | 2 | 5 | 15 | 2,312 | 10 / 0 / 12 |
| RtAnim | 11 | 3 | 2 | 6 | 956 | 9 / 0 / 2 |
| RtDict | 12 | 2 | 7 | 3 | 773 | 12 / 0 / 0 |
| RtBMP | 2 | 0 | 0 | 2 | 2,588 | 1 / 0 / 1 |
| Controlador D3D9 (RwD3D9) | 1 | 0 | 0 | 1 | 6 | 1 / 0 / 0 |
| RpAnisot | 1 | 0 | 0 | 1 | 6 | 1 / 0 / 0 |
| RtSlerp | 1 | 0 | 0 | 1 | 949 | 1 / 0 / 0 |


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
