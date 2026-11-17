# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)

> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** 2026-11-17 01:16 UTC

**Índice:** [1. Objetivo y dimensiones](#objetivo) · [2. Siguientes mediciones](#mediciones) · [3. Siguiente fase](#siguiente-fase) · [4. Vista general](#vista-general) · [5. Cobertura real del EXE](#cobertura-exe) · [6. Lo que falta del código del juego](#que-falta) · [7. Completas](#completas) · [8. Parciales](#parciales) · [9. Sin revertir](#sin-revertir) · [10. Cómo leer y actualizar](#metodo)

<a id="objetivo"></a>

## 1. Objetivo y dimensiones de progreso

La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. **La meta de este informe es completar todas las funciones del juego** (11,686 funciones y 2.98 MB de código en el EXE: el código propio del juego y su capa de plataforma). RenderWare, Direct3D y el CRT son bibliotecas enlazadas dentro del EXE: no forman parte de la meta, no se cuentan en las tablas y solo aparecen en la [cobertura real del EXE](#cobertura-exe) y en la lista de [bibliotecas fuera del objetivo](#bibliotecas) para aclarar qué queda fuera. Completar el juego es un hito hacia **ReSA — Zero Original Function Dependency**.

Cifras medidas el 17/11/2026. **Cada porcentaje de este informe lleva escrita la dimensión de la que habla**: *funciones* (del EXE), *bytes* (de código del EXE), *clases*, *entradas del catálogo* (filas de `hooks.csv`) o *validación* (cuántas de las ya hechas tienen evidencia aprobada). Dimensiones distintas dan porcentajes distintos y no son comparables entre sí.

| Tipo | Dimensión | Qué cuenta | Numerador | Denominador | Porcentaje (de qué) | Progreso |
| --- | --- | --- | ---: | ---: | --- | --- |
| **Funciones** | Funciones del juego reemplazadas por C++ en el EXE (**meta del proyecto**) | Funciones del EXE en 0x401000–0x74FFFF que ya tienen su hook reconstruido (análisis estático del EXE) | 11,683 | 11,686 | 99.97% de las funciones del juego | `████████████████████` |
| **Funciones** | Funciones del juego que faltan | Complemento de la fila anterior: grupos A–D de [lo que falta](#que-falta) | 3 | 11,686 | 0.03% de las funciones del juego | `░░░░░░░░░░░░░░░░░░░░` |
| **Bytes** | Bytes de código del juego reemplazados | Suma de los cuerpos de esas mismas funciones, en bytes de código | 2,979,094 | 2,979,290 | 99.99% de los bytes de código del juego | `████████████████████` |
| **Clases** | Clases del juego completas | Clases o grupos del catálogo con todas sus entradas reconstruidas y sin envolturas del juego abiertas | 987 | 988 | 99.90% de las clases del juego | `████████████████████` |
| **Validación** | Clases del juego validadas | Clases completas cuyas entradas tienen todas `validated=1` | 853 | 987 | 86.42% de las clases completas | `█████████████████░░░` |
| **Entradas del catálogo** | Entradas del juego reconstruidas | Filas de `hooks.csv` con `reversed=1` más las envolturas del juego abiertas, que cuentan como no reconstruidas | 11,813 | 11,814 | 99.99% de las entradas del juego del catálogo | `████████████████████` |
| **Validación** | Entradas del juego validadas | Entradas reconstruidas con evidencia aprobada vinculada (`validated=1`) | 11,073 | 11,813 | 93.74% de las entradas reconstruidas | `███████████████████░` |
| **Entradas del catálogo** | Entradas reconstruidas escritas solo en C++ | Entradas reconstruidas cuyo cuerpo fuente no tiene `__asm` ni `naked` (las de fuente no identificada de forma inequívoca no cuentan) | 11,699 | 11,813 | 99.03% de las entradas reconstruidas | `████████████████████` |

**Por qué hay un 100% y un 100%.** No se contradicen: miden cosas distintas. El 99.97% cuenta *funciones* y el 99.99% cuenta *bytes* de código; las funciones que faltan son más pequeñas que las ya reemplazadas, así que quedan muchas funciones pero pocos bytes. Las filas «Funciones» y «Bytes» miden el propio EXE (análisis estático, ver [cobertura real del EXE](#cobertura-exe)) y son la medida del avance hacia la meta; las filas de «Entradas del catálogo», «Clases» y «Validación» miden las filas registradas de `hooks.csv`, que solo conocen una parte del código del juego, por eso su porcentaje es mayor. Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL. La última fila cuenta, de las entradas reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md).

**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md).

Hay 11,813 entradas catalogadas del juego, 0 bloqueadas y 11,813 sin bloqueo. No constituyen un inventario exhaustivo del código del juego; las filas de RenderWare y del CRT del catálogo (625) quedan fuera de este informe. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks. Las extensiones o adaptaciones NOTSA (4, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta.

<a id="mediciones"></a>

## 2. Siguientes mediciones

Estado al 17/11/2026. Lo que ya se midió y lo que todavía no tiene un porcentaje útil:

| Dimensión | Estado actual | Qué hay que hacer |
| --- | --- | --- |
| Validación vigente para el árbol actual (validación) | 0/11,073 entradas validadas confirmadas como vigentes (0.00% de las entradas validadas) | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). El 04/11/2026 se repitieron 20 bancos desfasados sin regresión de producción (todos eran bancos desfasados o artefactos del entorno): falta consolidarlos en el registro. |
| Dependencias de funciones originales eliminadas (funciones) | Medido (cota inferior): 1 funciones del juego siguen llamándose directamente desde código del juego ya reemplazado; cierre transitivo: 1 funciones / 85 bytes. Ver [Cobertura real del EXE](#cobertura-exe) | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan: lista viva en la [sección 10](AnalisisGrupoA.md). |
| Trabajo real que falta (funciones y bytes) | 3 funciones y 196 bytes del juego sin reemplazar (0.03% de las funciones y 0.01% de los bytes del juego); grupo A 1 funciones y 85 bytes | Reconstruir y validar de menor a mayor coste, empezando por el grupo A ([sección 6](#que-falta)). |
| Coste por categoría de lote (tiempo de reconstrucción y validación) | Medido para las categorías 1, 2, 3 y 5 en los lotes 90A, 90B, 92A, 92B y 92C (tabla de abajo); sin medir la categoría 4 (funciones con FPU) | Cronometrar un lote piloto con FPU (método x87 a 24 bits) y los siguientes lotes de lógica para convertir la estimación en un coste por función. |

Costes medidos en los lotes cerrados (datos al 17/11/2026, [lotes.csv](JuegoExe/lotes.csv)). Cada lote reconstruye en C++ puro funciones que antes se llamaban al EXE por dirección y las valida dentro del juego (pares EXE/DLL con cero diferencias).

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

**¿Quedan envolturas de código del juego en el catálogo?** Sí. **Envolturas del juego abiertas: 1** (comprobado el 17/11/2026; 85 bytes): `StringCopyGrow` `0x004F39C0`. Son código del juego (D3DResourceSystem y lectura de streams en 0x72E000–0x735000): la DLL llama al EXE por dirección y no cuentan como reconstruidas hasta validarse. Los nombres y direcciones están en la lista de arriba.

Las envolturas de biblioteca no cuentan: 13 funciones con envoltura al EXE (CRT / runtime estático 11, QuickTime/QTML 1, RenderWare SDK 1) son RenderWare, controlador D3D9, CRT y QuickTime, que se sustituyen según la [hoja de ruta](RutaExeIndependiente.md) y no son trabajo del juego ([lista](#bibliotecas)). La clasificación sale de la dirección del destino: dentro de 0x401000–0x74FFFF es juego, fuera es biblioteca.

**Lotes en curso:** ninguno registrado para las envolturas del juego.

Cerrar las envolturas del juego no cierra el juego: quedan además las funciones del EXE sin reemplazar (grupos A–D, 3 funciones al 17/11/2026; [sección 6](#que-falta)). El plan de trabajo sale de [AnalisisGrupoA.md](AnalisisGrupoA.md) y su [lista completa](JuegoExe/grupoA.csv), y la lista viva está en la [sección 10](AnalisisGrupoA.md):

1. **Grupo A, por categorías y de menor a mayor coste:** helpers triviales y thunks muy llamados (hecho en el lote 90A: `CPad::GetPad` 0x53FB70, `CRGBA::Constructor` 0x7170C0...), pequeñas sin FPU (lote 90B), lógica sin FPU (empezando por las más llamadas y por clases enteras), funciones con FPU con el método x87 a 24 bits, y por último las de 0x300 bytes o más, una por lote (92B, 92C).
2. **Lote piloto de cada categoría** para medir el coste real: ya medidos los de las categorías 1, 2, 3 y 5 (tabla de la sección 2); falta la 4, con FPU.
3. **Revalidación vigente:** repetir los bancos sobre el árbol actual, priorizando los de fuentes cambiadas por lotes posteriores. Motivo: el banco `CVehD` llevaba roto desde el lote 83C (se cambió `m_fSteerAngle` por `m_GasPedal` en `ProcessDrivingAnims`) y no se vio hasta repetirlo.
4. **Después del código del juego:** `librw`, pipelines de SA y puentes `RwD3D9*`, QuickTime, DDraw y CRT, según la [hoja de ruta](RutaExeIndependiente.md); el cambio de método de validación sin el exe como oráculo.

**Validación consolidada parcialmente:** 11073 funciones del juego con evidencia aprobada vinculada, 785 con fuentes registradas coincidentes y 0 con vigencia confirmada incluyendo revisión de dependencias (cifras al 17/11/2026). Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).

<a id="vista-general"></a>

## 4. Vista general

Al 17/11/2026. Cada columna indica la dimensión y el denominador de su porcentaje.

| Clases validadas (validación) | Clases completas por validar (validación) | Entradas validadas (validación) | Entradas reconstruidas por validar (validación) | Entradas por reconstruir (entradas del catálogo) |
| :---: | :---: | :---: | :---: | :---: |
| **853 / 987 clases** | **134 / 987 clases** | **11,073 / 11,813 entradas** | **740 / 11,813 entradas** | **1 / 11,814 entradas** |
| 86.42% de las clases completas | 13.58% de las clases completas | 93.74% de las entradas reconstruidas | 6.26% de las entradas reconstruidas | 0.01% de las entradas del juego del catálogo |

Reconstrucción por clases: 987 completas, 0 parciales y 1 sin revertir de 988 clases o grupos (incluye 1 funciones del juego con envoltura al EXE aún sin reconstruir, en 1 clases o grupos).

> [!NOTE]
> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego ni el avance sobre todo el EXE (para eso, las filas «Funciones» y «Bytes» de la sección 1). Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.

<a id="cobertura-exe"></a>

## 5. Cobertura real del EXE (estimación)

Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras filas de la tabla de dimensiones salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático ([MemoryMap](MemoryMap/README.md): 17,647 entradas de función detectadas y 3.70 MB de cuerpos de función), cruzado con el catálogo, el registro de validación y las envolturas pendientes. 12,438 filas reconstruidas del catálogo equivalen a **12,416 funciones distintas del EXE ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo). En esta tabla, «% de funciones» es la parte de las funciones de esa región que ya está en C++ y «% de bytes» la parte de los bytes de código de esa región.

| Región del EXE | Funciones (total de la región) | Bytes de código (total de la región) | Reemplazadas por C++: funciones y % de funciones | Reemplazadas: % de bytes | Funciones de la región llamadas directamente desde código ya reemplazado |
| --- | ---: | ---: | --- | --- | ---: |
| Juego y plataforma (0x401000–0x74FFFF) | 11,686 | 2,979,290 | `████████████████████` 11,683 · 100.0% de funciones | `████████████████████` 100.0% de bytes | 1 |
| RenderWare y D3D (0x750000–0x81FFFF) | 1,889 | 600,971 | `████████░░░░░░░░░░░░` 724 · 38.3% de funciones | `██████░░░░░░░░░░░░░░` 30.2% de bytes | 53 |
| CRT/MSVC y runtime (0x820000 en adelante) | 4,072 | 115,711 | `░░░░░░░░░░░░░░░░░░░░` 9 · 0.2% de funciones | `░░░░░░░░░░░░░░░░░░░░` 0.2% de bytes | 73 |
| **Total** | **17,647** | **3,695,972** | `██████████████░░░░░░` 12,416 · 70.4% de funciones | `█████████████████░░░` 85.5% de bytes | **127** |

**Qué significa para un ejecutable independiente.** No hace falta reconstruir las 17,647 funciones: RenderWare/D3D y el CRT son bibliotecas enlazadas dentro del EXE (5,961 entradas, 19% de los bytes del EXE) que se sustituirían enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego (11,686 funciones, 81% de los bytes del EXE), y de él ya está reemplazado el **100% de los bytes del juego** y el 100% de las funciones del juego: no se contradicen, las funciones que quedan son más pequeñas que la media. Entre las 11,686 funciones del juego hay 1,806 de 16 bytes o menos (stubs, thunks y helpers).

<a id="que-falta"></a>

## 6. Lo que falta del código del juego: 3 funciones, 0 KB

Al 17/11/2026: 3 funciones (0.03% de las funciones del juego) y 196 bytes (0.01% de los bytes del juego) siguen sin reemplazar. Este es el trabajo real que falta del código del juego, lo mida o no el catálogo. Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).

| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |
| --- | ---: | ---: | --- |
| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | 1 | 85 | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. Lista viva: [dependencias directas (grupo A)](AnalisisGrupoA.md). |
| D. Sin llamadas directas ni vtable | 1 | 81 | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |
| C. Llamadas directas solo desde código aún no reemplazado | 1 | 30 | Dependen de reconstruir antes A y B; parte puede ser código muerto. |
| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | 0 | 0 | Probablemente necesarios: se ejecutan por despacho virtual. |

Del código del juego ya reemplazado, **1 funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura. Las 1 envolturas del juego abiertas de la sección siguiente son funciones del EXE que la DLL llama por dirección: 1 están incluidas en estas cifras.

> [!NOTE]
> **Cotas y límites.** Es un análisis estático: hay 6,004 llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.

<a id="completas"></a>
## 7. 🟢 Completas · 987 clases

Al 17/11/2026: 987 de 988 clases o grupos están completas (todas sus entradas reconstruidas y ninguna envoltura del juego abierta). Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.

<details>
<summary>Mostrar catálogo de clases completas</summary>

| Clase (funciones) | Clase (funciones) | Clase (funciones) |
| --- | --- | --- |
| [App](../source/app/app.cpp) (11) ✅ 11 | [CModelInfo](../source/game_sa/Models/ModelInfo.cpp) (38) ✅ 38 | [CTaskComplexWaitAtAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitAtAttractor.cpp) (8) ✅ 8 |
| [BoneNode_c](../source/game_sa/Ragdoll/BoneNode.cpp) (18) ✅ 18 | [CModelInfoAccelerator](../source/game_sa/ModelInfoAccelerator.cpp) (3) ✅ 3 | [CTaskComplexWaitForBackup](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBackup.cpp) (9) ✅ 9 |
| [BoneNodeManager_c](../source/game_sa/Ragdoll/BoneNodeManager.cpp) (8) ✅ 8 | [CMonsterTruck](../source/game_sa/Entity/Vehicle/MonsterTruck.cpp) (13) ✅ 13 | [CTaskComplexWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForBus.cpp) (8) ✅ 8 |
| [BreakablePlugin](../source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp) (6) ✅ 6 | [CMotionBlurStreaks](../source/game_sa/MotionBlurStreaks.cpp) (4) ✅ 4 | [CTaskComplexWaitForDryWeather](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForDryWeather.cpp) (8) ✅ 8 |
| [BreakManager_c](../source/game_sa/BreakManager_c.cpp) (7) ✅ 7 | [CMouseControllerState](../source/game_sa/MouseControllerState.cpp) (3) ✅ 3 | [CTaskComplexWaitForPed](../source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp) (9) ✅ 9 |
| [BreakObject_c](../source/game_sa/BreakObject_c.cpp) (9) ✅ 9 | [CMovingThings](../source/game_sa/MovingThings.cpp) (5) ✅ 5 | [CTaskComplexWalkAlongsidePed](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkAlongsidePed.cpp) (10) ✅ 10 |
| [C2dEffect](../source/game_sa/Plugins/TwoDEffectPlugin/2dEffect.cpp) (14) ✅ 14 | [CNearbyFireScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexWalkRoundBuildingAttempt](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp) (14) ✅ 14 |
| [C3dMarker](../source/game_sa/3dMarker.cpp) (6) ✅ 6 | [CNodeAddress](../source/game_sa/NodeAddress.cpp) (3) ✅ 3 | [CTaskComplexWalkRoundCar](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundCar.cpp) (14) ✅ 14 |
| [C3dMarkers](../source/game_sa/3dMarkers.cpp) (20) ✅ 20 | [CNodeRoute](../source/game_sa/NodeRoute.cpp) (3) ✅ 3 | [CTaskComplexWalkRoundFire](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundFire.cpp) (9) ✅ 9 |
| [C_PcSave](../source/game_sa/C_PcSave.cpp) (6) ✅ 6 | [CNodeRoutePool](../source/game_sa/Pools/NodeRoutePool.h) (1) ✅ 1 | [CTaskComplexWalkRoundObject](../source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp) (11) ✅ 11 |
| [CAccidentManager](../source/game_sa/AccidentManager.cpp) (6) ✅ 6 | [CObject](../source/game_sa/Entity/Object/Object.cpp) (58) ✅ 58 | [CTaskComplexWander](../source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp) (21) ✅ 21 |
| [CAcquaintance](../source/game_sa/Acquaintance.cpp) (5) ✅ 5 | [CObjectData](../source/game_sa/ObjectData.cpp) (2) ✅ 2 | [CTaskComplexWanderCop](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCop.cpp) (13) ✅ 13 |
| [CActiveOccluder](../source/game_sa/ActiveOccluder.cpp) (2) ✅ 2 | [CObjectPool](../source/game_sa/Pools/ObjectPool.h) (3) ✅ 3 | [CTaskComplexWanderCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderCriminal.cpp) (7) ✅ 7 |
| [CAEAmbienceTrackManager](../source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp) (10) ✅ 10 | [CObjectPotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskComplexWanderFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp) (7) ✅ 7 |
| [CAEAudioChannel](../source/game_sa/Audio/Hardware/AEAudioChannel.cpp) (16) ✅ 16 | [CObjectSaveStructure](../source/game_sa/ObjectSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexWanderGang](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp) (7) ✅ 7 |
| [CAEAudioEntity](../source/game_sa/Audio/Entities/AEAudioEntity.cpp) (8) ✅ 6 | [CObjectScanner](../source/game_sa/ObjectScanner.cpp) (3) ✅ 3 | [CTaskComplexWanderMedic](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderMedic.cpp) (6) ✅ 6 |
| [CAEAudioEnvironment](../source/game_sa/Audio/AEAudioEnvironment.cpp) (6) ✅ 6 | [COccluder](../source/game_sa/Occluder.cpp) (3) ✅ 3 | [CTaskComplexWanderProstitute](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp) (7) ✅ 7 |
| [CAEAudioHardware](../source/game_sa/Audio/Hardware/AEAudioHardware.cpp) (55) ✅ 55 | [COcclusion](../source/game_sa/Occlusion.cpp) (5) ✅ 5 | [CTaskComplexWanderStandard](../source/game_sa/Tasks/TaskTypes/TaskComplexWanderStandard.cpp) (12) ✅ 12 |
| [CAEAudioUtility](../source/game_sa/Audio/AEAudioUtility.cpp) (11) ✅ 11 | [COctTree](../source/game_sa/Core/OctTree.cpp) (9) ✅ 9 | [CTaskGangHasslePed](../source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp) (8) ✅ 8 |
| [CAEBankLoader](../source/game_sa/Audio/Loaders/AEBankLoader.cpp) (8) ✅ 8 | [COctTreeBase](../source/game_sa/Core/OctTreeBase.cpp) (1) ✅ 1 | [CTaskGangHassleVehicle](../source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp) (10) ✅ 10 |
| [CAECollisionAudioEntity](../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp) (18) ✅ 18 | [Common](../source/game_sa/common.cpp) (26) ✅ 26 | [CTaskGoToVehicleAndLean](../source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp) (11) ✅ 11 |
| [CAECutsceneTrackManager](../source/game_sa/Audio/Managers/AECutsceneTrackManager.cpp) (11) ✅ 11 | [CompressedVector](../source/game_sa/CompressedVector.cpp) (2) ✅ 2 | [CTaskInteriorBeInHouse](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInHouse.cpp) (9) ✅ 9 |
| [CAEDataStream](../source/game_sa/Audio/Loaders/AEDataStream.cpp) (22) ✅ 22 | [COnscreenCounterEntry](../source/game_sa/OnscreenCounterEntry.cpp) (2) ✅ 2 | [CTaskInteriorBeInOffice](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp) (9) ✅ 9 |
| [CAEDoorAudioEntity](../source/game_sa/Audio/Entities/AEDoorAudioEntity.cpp) (6) ✅ 6 | [COnscreenTimer](../source/game_sa/OnscreenTimer.cpp) (9) ✅ 9 | [CTaskInteriorBeInShop](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInShop.cpp) (7) ✅ 7 |
| [CAEExplosionAudioEntity](../source/game_sa/Audio/Entities/AEExplosionAudioEntity.cpp) (4) ✅ 4 | [COnscreenTimerEntry](../source/game_sa/OnscreenTimer.cpp) (1) ✅ 1 | [CTaskInteriorGoToInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorGoToInfo.cpp) (8) ✅ 8 |
| [CAEFireAudioEntity](../source/game_sa/Audio/Entities/AEFireAudioEntity.cpp) (7) ✅ 7 | [CPad](../source/game_sa/Pad.cpp) (102) ✅ 102 | [CTaskInteriorLieInBed](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp) (8) ✅ 8 |
| [CAEFrontendAudioEntity](../source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp) (7) ✅ 7 | [CPathFind](../source/game_sa/PathFind.cpp) (81) ✅ 81 | [CTaskInteriorShopKeeper](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorShopKeeper.cpp) (8) ✅ 8 |
| [CAEGlobalWeaponAudioEntity](../source/game_sa/Audio/Entities/AEGlobalWeaponAudioEntity.cpp) (8) ✅ 8 | [CPathNode](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CTaskInteriorSitAtDesk](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp) (10) ✅ 10 |
| [CAEMFDecoder](../source/game_sa/Audio/Loaders/AEMFDecoder.cpp) (14) ✅ 14 | [CPatrolRoute](../source/game_sa/PatrolRoute.cpp) (7) ✅ 6 | [CTaskInteriorSitInChair](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp) (7) ✅ 7 |
| [CAEMP3BankLoader](../source/game_sa/Audio/Loaders/AEMP3BankLoader.cpp) (14) ✅ 14 | [CPatrolRoutePool](../source/game_sa/Pools/PatrolRoutePool.h) (1) ✅ 1 | [CTaskInteriorUseInfo](../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp) (9) ✅ 9 |
| [CAEMP3TrackLoader](../source/game_sa/Audio/Loaders/AEMP3TrackLoader.cpp) (8) ✅ 8 | [CPed](../source/game_sa/Entity/Ped/Ped.cpp) (156) ✅ 156 | [CTaskLeanOnVehicle](../source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp) (8) ✅ 8 |
| [CAEPedAudioEntity](../source/game_sa/Audio/Entities/AEPedAudioEntity.cpp) (22) ✅ 22 | [CPedAcquaintanceScanner](../source/game_sa/Events/EventScanner.cpp) (6) ✅ 6 | [CTaskManager](../source/game_sa/Tasks/TaskManager.cpp) (20) ✅ 20 |
| [CAEPedlessSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedlessSpeechAudioEntity.cpp) (12) ✅ 12 | [CPedATMAttractor](../source/game_sa/Attractors/PedAtmAttractor.cpp) (3) | [CTaskPool](../source/game_sa/Pools/TaskPool.h) (1) ✅ 1 |
| [CAEPedSpeechAudioEntity](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (59) ✅ 59 | [CPedAttractor](../source/game_sa/Attractors/PedAttractor.cpp) (85) ✅ 51 | [CTaskSequences](../source/game_sa/Tasks/TaskSequences.cpp) (3) ✅ 3 |
| [CAEPedSpeechSlot](../source/game_sa/Audio/Entities/AEPedSpeechAudioEntity.cpp) (1) ✅ 1 | [CPedAttractorManager](../source/game_sa/PedAttractorManager.cpp) (25) ✅ 22 | [CTaskSimple](../source/game_sa/Tasks/Task.cpp) · [archivo 2](../source/game_sa/Tasks/TaskSimple.cpp) (4) ✅ 4 |
| [CAEPedWeaponAudioEntity](../source/game_sa/Audio/Entities/AEPedWeaponAudioEntity.cpp) (5) ✅ 5 | [CPedAttractorPedPlacer](../source/game_sa/Attractors/PedAttractorPedPlacer.cpp) (1) ✅ 1 | [CTaskSimpleAbseil](../source/game_sa/Tasks/TaskTypes/TaskSimpleAbseil.cpp) (4) ✅ 4 |
| [CAEPoliceScannerAudioEntity](../source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp) (17) ✅ 17 | [CPedAttractorPool](../source/game_sa/Pools/PedAttractorPool.h) (1) ✅ 1 | [CTaskSimpleAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp) (8) ✅ 8 |
| [CAERadioTrackManager](../source/game_sa/Audio/Managers/AERadioTrackManager.cpp) (43) ✅ 43 | [CPedClothesDesc](../source/game_sa/PedClothesDesc.cpp) (9) ✅ 9 | [CTaskSimpleAffectSecondaryBehaviour](../source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp) (7) ✅ 7 |
| [CAEScriptAudioEntity](../source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp) (20) ✅ 20 | [CPedDamageResponseCalculator](../source/game_sa/PedDamageResponseCalculator.cpp) (8) ✅ 8 | [CTaskSimpleAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp) (4) ✅ 4 |
| [CAESmoothFadeThread](../source/game_sa/Audio/AESmoothFadeThread.cpp) (11) ✅ 11 | [CPedGeometryAnalyser](../source/game_sa/PedGeometryAnalyser.cpp) (38) ✅ 38 | [CTaskSimpleArrestPed](../source/game_sa/Tasks/TaskTypes/TaskSimpleArrestPed.cpp) (8) ✅ 8 |
| [CAESound](../source/game_sa/Audio/AESound.cpp) (19) ✅ 19 | [CPedGroup](../source/game_sa/PedGroup.cpp) (12) ✅ 12 | [CTaskSimpleBeHit](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHit.cpp) (9) ✅ 9 |
| [CAESoundManager](../source/game_sa/Audio/Managers/AESoundManager.cpp) (15) ✅ 15 | [CPedGroupDefaultTaskAllocatorChat](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorChat.h) (3) ✅ 3 | [CTaskSimpleBeHitWhileMoving](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp) (8) ✅ 8 |
| [CAEStaticChannel](../source/game_sa/Audio/Hardware/AEStaticChannel.cpp) (11) ✅ 11 | [CPedGroupDefaultTaskAllocatorFollowAnyMeans](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h) (3) ✅ 3 | [CTaskSimpleBeKickedOnGround](../source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp) (8) ✅ 8 |
| [CAEStreamingChannel](../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp) (25) ✅ 25 | [CPedGroupDefaultTaskAllocatorFollowLimited](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h) (3) ✅ 3 | [CTaskSimpleBikeJacked](../source/game_sa/Tasks/TaskTypes/TaskSimpleBikeJacked.cpp) (9) ✅ 9 |
| [CAEStreamingDecoder](../source/game_sa/Audio/Loaders/AEStreamingDecoder.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocatorRandom](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h) (3) ✅ 3 | [CTaskSimpleCarAlign](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarAlign.cpp) (11) ✅ 11 |
| [CAEStreamThread](../source/game_sa/Audio/AEStreamThread.cpp) (16) ✅ 16 | [CPedGroupDefaultTaskAllocators](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h) (1) ✅ 1 | [CTaskSimpleCarCloseDoorFromInside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromInside.cpp) (11) ✅ 11 |
| [CAEStreamTransformer](../source/game_sa/Audio/Loaders/AEStreamTransformer.cpp) (3) ✅ 3 | [CPedGroupDefaultTaskAllocatorSitInLeaderCar](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h) (3) ✅ 3 | [CTaskSimpleCarCloseDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.cpp) (11) ✅ 11 |
| [CAETrackLoader](../source/game_sa/Audio/Loaders/AETrackLoader.cpp) (1) ✅ 1 | [CPedGroupDefaultTaskAllocatorStandStill](../source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h) (3) ✅ 3 | [CTaskSimpleCarDrive](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp) (16) ✅ 16 |
| [CAETwinLoopSoundEntity](../source/game_sa/Audio/Entities/AETwinLoopSoundEntity.cpp) (10) ✅ 10 | [CPedGroupIntelligence](../source/game_sa/PedGroupIntelligence.cpp) (31) ✅ 31 | [CTaskSimpleCarDriveTimed](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarDriveTimed.cpp) (6) ✅ 6 |
| [CAEUserRadioTrackManager](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (16) ✅ 15 | [CPedGroupMembership](../source/game_sa/PedGroupMembership.cpp) (19) ✅ 19 | [CTaskSimpleCarFallOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarFallOut.cpp) (11) ✅ 11 |
| [CAEVehicleAudioEntity](../source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp) (107) ✅ 107 | [CPedGroupPlacer](../source/game_sa/PedGroupPlacer.cpp) (4) ✅ 4 | [CTaskSimpleCarForcePedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp) (7) ✅ 7 |
| [CAEVorbisDecoder](../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp) (14) ✅ 14 | [CPedGroups](../source/game_sa/PedGroups.cpp) (12) ✅ 12 | [CTaskSimpleCarGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetIn.cpp) (10) ✅ 10 |
| [CAEWaterCannonAudioEntity](../source/game_sa/Audio/Entities/AEWaterCannonAudioEntity.cpp) (9) ✅ 9 | [CPedIK](../source/game_sa/PedIK.cpp) (9) ✅ 9 | [CTaskSimpleCarGetOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGetOut.cpp) (11) ✅ 11 |
| [CAEWaveDecoder](../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp) (10) ✅ 10 | [CPedIntelligence](../source/game_sa/PedIntelligence.cpp) (61) ✅ 61 | [CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp) (8) ✅ 8 |
| [CAEWeaponAudioEntity](../source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp) (19) ✅ 19 | [CPedIntelligencePool](../source/game_sa/Pools/PedIntelligencePool.h) (1) ✅ 1 | [CTaskSimpleCarJumpOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarJumpOut.cpp) (11) ✅ 11 |
| [CAEWeatherAudioEntity](../source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp) (6) ✅ 6 | [CPedList](../source/game_sa/PedList.cpp) (9) ✅ 9 | [CTaskSimpleCarOpenDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenDoorFromOutside.cpp) (11) ✅ 11 |
| [CAEWMADecoder](../source/game_sa/Audio/Loaders/AEWMADecoder.cpp) (13) ✅ 13 | [CPedLookAtAttractor](../source/game_sa/Attractors/PedLookAtAttractor.cpp) (3) | [CTaskSimpleCarOpenLockedDoorFromOutside](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarOpenLockedDoorFromOutside.cpp) (10) ✅ 10 |
| [CAnimatedBuilding](../source/game_sa/Entity/AnimatedBuilding.cpp) (6) ✅ 6 | [CPedModelInfo](../source/game_sa/Models/PedModelInfo.cpp) (12) ✅ 11 | [CTaskSimpleCarSetPedInAsDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.cpp) (7) ✅ 7 |
| [CAnimBlendAssocGroup](../source/game_sa/Animation/AnimBlendAssocGroup.cpp) (15) ✅ 15 | [CPedParkAttractor](../source/game_sa/Attractors/PedParkAttractor.cpp) (3) | [CTaskSimpleCarSetPedInAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.cpp) (7) ✅ 7 |
| [CAnimBlendAssociation](../source/game_sa/Animation/AnimBlendAssociation.cpp) (23) ✅ 23 | [CPedPizzaAttractor](../source/game_sa/Attractors/PedPizzaAttractor.cpp) (3) | [CTaskSimpleCarSetPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedOut.cpp) (8) ✅ 8 |
| [CAnimBlendClumpData](../source/game_sa/Animation/AnimBlendClumpData.cpp) (7) ✅ 6 | [CPedPlacement](../source/game_sa/PedPlacement.cpp) (4) ✅ 4 | [CTaskSimpleCarSetPedSlowDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.cpp) (7) ✅ 7 |
| [CAnimBlendHierarchy](../source/game_sa/Animation/AnimBlendHierarchy.cpp) (16) ✅ 16 | [CPedPool](../source/game_sa/Pools/PedPool.h) (5) ✅ 5 | [CTaskSimpleCarSetTempAction](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSetTempAction.cpp) (7) ✅ 7 |
| [CAnimBlendNode](../source/game_sa/Animation/AnimBlendNode.cpp) (16) ✅ 16 | [CPedSaveStructure](../source/game_sa/Entity/Ped/PedSaveStructure.cpp) (3) ✅ 3 | [CTaskSimpleCarShuffle](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarShuffle.cpp) (10) ✅ 10 |
| [CAnimBlendSequence](../source/game_sa/Animation/AnimBlendSequence.cpp) (17) ✅ 17 | [CPedScanner](../source/game_sa/PedScanner.cpp) (2) ✅ 2 | [CTaskSimpleCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowBeDraggedOut.cpp) (11) ✅ 11 |
| [CAnimBlendStaticAssociation](../source/game_sa/Animation/AnimBlendStaticAssociation.cpp) (8) ✅ 8 | [CPedScriptedAttractor](../source/game_sa/Attractors/PedScriptedAttractor.cpp) (3) | [CTaskSimpleCarSlowDragPedOut](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarSlowDragPedOut.cpp) (11) ✅ 11 |
| [CAnimManager](../source/game_sa/Animation/AnimManager.cpp) (32) ✅ 32 | [CPedScriptedTaskRecord](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (5) ✅ 5 | [CTaskSimpleCarWaitForDoorNotToBeInUse](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp) (8) ✅ 8 |
| [CAtomicModelInfo](../source/game_sa/Models/AtomicModelInfo.cpp) (14) ✅ 14 | [CPedScriptedTaskRecordData](../source/game_sa/Tasks/PedScriptedTaskRecord.cpp) (8) ✅ 8 | [CTaskSimpleCarWaitToSlowDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp) (8) ✅ 8 |
| [CAttractorScanner](../source/game_sa/AttractorScanner.cpp) (7) ✅ 7 | [CPedSeatAttractor](../source/game_sa/Attractors/PedSeatAttractor.cpp) (3) | [CTaskSimpleChat](../source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp) (5) ✅ 5 |
| [CAudioEngine](../source/game_sa/Audio/AudioEngine.cpp) (83) ✅ 83 | [CPedShelterAttractor](../source/game_sa/Attractors/PedShelterAttractor.cpp) (11) ✅ 7 | [CTaskSimpleChoking](../source/game_sa/Tasks/TaskTypes/TaskSimpleChoking.cpp) (8) ✅ 8 |
| [CAudioZones](../source/game_sa/Audio/AudioZones.cpp) (9) ✅ 8 | [CPedStats](../source/game_sa/PedStats.cpp) (5) ✅ 5 | [CTaskSimpleClearLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleClearLookAt.cpp) (7) ✅ 7 |
| [CAutomobile](../source/game_sa/Entity/Vehicle/Automobile.cpp) (114) ✅ 104 | [CPedStepAttractor](../source/game_sa/Attractors/PedStepAttractor.cpp) (3) | [CTaskSimpleClimb](../source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp) (19) ✅ 19 |
| [CAutoPilot](../source/game_sa/AutoPilot.cpp) (4) ✅ 3 | [CPedStopAttractor](../source/game_sa/Attractors/PedStopAttractor.cpp) (3) | [CTaskSimpleCower](../source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp) (5) ✅ 5 |
| [CBaseModelInfo](../source/game_sa/Models/BaseModelInfo.cpp) (38) ✅ 38 | [CPedStuckChecker](../source/game_sa/PedStuckChecker.cpp) (1) ✅ 1 | [CTaskSimpleCreateCarAndGetIn](../source/game_sa/Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.cpp) (7) ✅ 7 |
| [CBike](../source/game_sa/Entity/Vehicle/Bike.cpp) (62) ✅ 62 | [CPedTaskPair](../source/game_sa/PedTaskPair.cpp) (1) ✅ 1 | [CTaskSimpleDead](../source/game_sa/Tasks/TaskTypes/TaskSimpleDead.cpp) (7) ✅ 7 |
| [CBirds](../source/game_sa/Birds.cpp) (6) ✅ 6 | [CPedToPlayerConversations](../source/game_sa/Conversations/PedToPlayerConversations.cpp) (3) ✅ 3 | [CTaskSimpleDie](../source/game_sa/Tasks/TaskTypes/TaskSimpleDie.cpp) (11) ✅ 11 |
| [CBmx](../source/game_sa/Entity/Vehicle/Bmx.cpp) (13) ✅ 13 | [CPedTriggerScriptAttractor](../source/game_sa/Attractors/PedTriggerScriptAttractor.cpp) (3) | [CTaskSimpleDieInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDieInCar.cpp) (6) ✅ 6 |
| [CBoat](../source/game_sa/Entity/Vehicle/Boat.cpp) (24) ✅ 24 | [CPedType](../source/game_sa/PedType.cpp) (14) ✅ 14 | [CTaskSimpleDoHandSignal](../source/game_sa/Tasks/TaskTypes/TaskSimpleDoHandSignal.cpp) (7) ✅ 7 |
| [CBouncingPanel](../source/game_sa/BouncingPanel.cpp) (3) ✅ 3 | [CPhysical](../source/game_sa/Entity/Physical.cpp) (62) ✅ 62 | [CTaskSimpleDrown](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrown.cpp) (6) ✅ 6 |
| [CBoundingBox](../source/game_sa/Collision/BoundingBox.cpp) (7) ✅ 7 | [CPickup](../source/game_sa/Pickup.cpp) (13) ✅ 12 | [CTaskSimpleDrownInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp) (7) ✅ 7 |
| [CBox](../source/game_sa/Collision/Box.cpp) (3) ✅ 3 | [CPickups](../source/game_sa/Pickups.cpp) (38) ✅ 38 | [CTaskSimpleDuck](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp) (18) ✅ 18 |
| [CBridge](../source/game_sa/Bridge.cpp) (5) ✅ 5 | [CPlaceable](../source/game_sa/Entity/Placeable.cpp) (40) ✅ 40 | [CTaskSimpleDuckToggle](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckToggle.cpp) (7) ✅ 7 |
| [CBrightLights](../source/game_sa/BrightLights.cpp) (4) ✅ 4 | [CPlaceName](../source/game_sa/PlaceName.cpp) (2) ✅ 2 | [CTaskSimpleDuckWhileShotsWhizzing](../source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp) (7) ✅ 7 |
| [CBuilding](../source/game_sa/Entity/Building.cpp) (7) ✅ 6 | [CPlane](../source/game_sa/Entity/Vehicle/Plane.cpp) (26) ✅ 26 | [CTaskSimpleEvasiveDive](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveDive.cpp) (9) ✅ 9 |
| [CBuildingPool](../source/game_sa/Pools/BuildingPool.h) (1) ✅ 1 | [CPlaneBanners](../source/game_sa/PlaneBanners.cpp) (1) ✅ 1 | [CTaskSimpleEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskSimpleEvasiveStep.cpp) (9) ✅ 9 |
| [CBulletInfo](../source/game_sa/BulletInfo.cpp) (4) ✅ 4 | [CPlaneTrail](../source/game_sa/PlaneTrail.cpp) (3) ✅ 3 | [CTaskSimpleFacial](../source/game_sa/Tasks/TaskTypes/TaskSimpleFacial.cpp) (8) ✅ 8 |
| [CBulletTraces](../source/game_sa/BulletTraces.cpp) (5) ✅ 5 | [CPlaneTrails](../source/game_sa/PlaneTrails.cpp) (4) ✅ 4 | [CTaskSimpleFall](../source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp) (10) ✅ 10 |
| [cBuoyancy](../source/game_sa/Buoyancy.cpp) (10) ✅ 10 | [CPlantColEntEntry](../source/game_sa/Plant/PlantColEntEntry.cpp) (2) ✅ 2 | [CTaskSimpleFight](../source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp) (27) ✅ 27 |
| [CCam](../source/game_sa/Cam.cpp) (58) ✅ 58 | [CPlantLocTri](../source/game_sa/Plant/PlantLocTri.cpp) (3) ✅ 3 | [CTaskSimpleFightingControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp) (8) ✅ 8 |
| [CCamera](../source/game_sa/Camera.cpp) (126) ✅ 124 | [CPlantMgr](../source/game_sa/PlantMgr.cpp) (20) ✅ 20 | [CTaskSimpleFinishBrain](../source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.cpp) (5) ✅ 5 |
| [CCarAI](../source/game_sa/CarAI.cpp) (26) ✅ 26 | [CPlantSurfPropMgr](../source/game_sa/PlantSurfPropMgr.cpp) (4) ✅ 4 | [CTaskSimpleGangDriveBy](../source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp) (13) ✅ 13 |
| [CCarCtrl](../source/game_sa/CarCtrl.cpp) (115) ✅ 115 | [CPlayerInfo](../source/game_sa/PlayerInfo.cpp) (32) ✅ 31 | [CTaskSimpleGetUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp) (9) ✅ 9 |
| [CCarEnterExit](../source/game_sa/CarEnterExit.cpp) (32) ✅ 32 | [CPlayerInfoSaveStructure](../source/game_sa/PlayerInfoSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleGiveCPR](../source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp) (9) ✅ 9 |
| [CCarFXRenderer](../source/game_sa/Fx/CarFXRenderer.cpp) (10) ✅ 10 | [CPlayerPed](../source/game_sa/Entity/Ped/PlayerPed.cpp) (71) ✅ 71 | [CTaskSimpleGoTo](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoTo.cpp) (7) ✅ 7 |
| [CCarGenerator](../source/game_sa/CarGenerator.cpp) (9) ✅ 9 | [CPlayerPedData](../source/game_sa/PlayerPedData.cpp) (6) ✅ 5 | [CTaskSimpleGoToPoint](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp) (7) ✅ 7 |
| [CCarPathLink](../source/game_sa/PathFind.cpp) (2) ✅ 2 | [CPlayerRelationshipRecorder](../source/game_sa/PlayerRelationshipRecorder.cpp) (6) ✅ 6 | [CTaskSimpleGoToPointFine](../source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp) (12) ✅ 12 |
| [CCheat](../source/game_sa/Cheat.cpp) (74) ✅ 74 | [CPlayerSkin](../source/game_sa/Entity/Ped/PlayerSkin.cpp) (4) ✅ 4 | [CTaskSimpleGunControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp) (7) ✅ 7 |
| [CCheckpoint](../source/game_sa/Checkpoint.cpp) (1) ✅ 1 | [CPointLights](../source/game_sa/PointLights.cpp) (7) ✅ 7 | [CTaskSimpleHandsUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleHandsUp.cpp) (5) ✅ 5 |
| [CCheckpoints](../source/game_sa/Checkpoints.cpp) (8) ✅ 8 | [CPointList](../source/game_sa/PointList.cpp) (2) ✅ 2 | [CTaskSimpleHitByGunFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromFront.cpp) (4) ✅ 4 |
| [CCivilianPed](../source/game_sa/Entity/Ped/CivilianPed.cpp) (4) ✅ 4 | [CPointRoute](../source/game_sa/PedGeometryAnalyser.cpp) (6) ✅ 6 | [CTaskSimpleHitByGunFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.cpp) (4) ✅ 4 |
| [CClock](../source/game_sa/Clock.cpp) (9) ✅ 9 | [CPointRoutePool](../source/game_sa/Pools/PointRoutePool.h) (1) ✅ 1 | [CTaskSimpleHitByGunFromRear](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRear.cpp) (4) ✅ 4 |
| [CClothes](../source/game_sa/Clothes.cpp) (12) ✅ 12 | [CPools](../source/game_sa/Pools.cpp) (72) ✅ 21 | [CTaskSimpleHitByGunFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitByGunFromRight.cpp) (4) ✅ 4 |
| [CClothesBuilder](../source/game_sa/ClothesBuilder.cpp) (25) ✅ 25 | [CPopCycle](../source/game_sa/PopCycle.cpp) (15) ✅ 15 | [CTaskSimpleHitFromBack](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBack.cpp) (4) ✅ 4 |
| [CClouds](../source/game_sa/Clouds.cpp) (21) ✅ 21 | [CPopulation](../source/game_sa/Population.cpp) (68) ✅ 68 | [CTaskSimpleHitFromBehind](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp) (4) ✅ 4 |
| [CClumpModelInfo](../source/game_sa/Models/ClumpModelInfo.cpp) (27) ✅ 27 | [CPostEffects](../source/game_sa/PostEffects.cpp) (38) ✅ 38 | [CTaskSimpleHitFromFront](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromFront.cpp) (4) ✅ 4 |
| [CColAccel](../source/game_sa/Collision/ColAccel.cpp) (11) ✅ 11 | [CPPTriPlantBuffer](../source/game_sa/PPTriPlantBuffer.cpp) (7) ✅ 7 | [CTaskSimpleHitFromLeft](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromLeft.cpp) (4) ✅ 4 |
| [CColBox](../source/game_sa/Collision/ColBox.cpp) (2) ✅ 2 | [CProjectile](../source/game_sa/Entity/Object/Projectile.cpp) (3) ✅ 3 | [CTaskSimpleHitFromRight](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromRight.cpp) (4) ✅ 4 |
| [CColDisk](../source/game_sa/Collision/ColDisk.cpp) (1) ✅ 1 | [CProjectileInfo](../source/game_sa/ProjectileInfo.cpp) (12) ✅ 12 | [CTaskSimpleHitHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp) (8) ✅ 8 |
| [CColLine](../source/game_sa/Collision/ColLine.cpp) (2) ✅ 2 | [CProtectionStub](../source/game_sa/ProtectionStub.cpp) (3) | [CTaskSimpleHitWall](../source/game_sa/Tasks/TaskTypes/TaskSimpleHitWall.cpp) (4) ✅ 4 |
| [CCollision](../source/game_sa/Collision/Collision.cpp) (62) ✅ 62 | [CPtrNodeDoubleLinkPool](../source/game_sa/Pools/PtrNodeDoubleLinkPool.h) (7) ✅ 5 | [CTaskSimpleHoldEntity](../source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp) (17) ✅ 17 |
| [CCollisionData](../source/game_sa/Collision/CollisionData.cpp) (10) ✅ 10 | [CPtrNodeSingleLinkPool](../source/game_sa/Pools/PtrNodeSingleLinkPool.h) (7) ✅ 5 | [CTaskSimpleHurtPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp) (7) ✅ 7 |
| [CCollisionEventScanner](../source/game_sa/Collision/CollisionEventScanner.cpp) (1) ✅ 1 | [CQuadBike](../source/game_sa/Entity/Vehicle/QuadBike.cpp) (14) ✅ 14 | [CTaskSimpleIKChain](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKChain.cpp) (11) ✅ 11 |
| [CCollisionPlugin](../source/game_sa/Plugins/CollisionPlugin/CollisionPlugin.cpp) (2) ✅ 2 | [CQuadTreeNode&lt;T&gt;](../source/game_sa/Core/QuadTreeNode.h) (20) ✅ 20 | [CTaskSimpleIKLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKLookAt.cpp) (9) ✅ 9 |
| [CColModel](../source/game_sa/Collision/ColModel.cpp) (12) ✅ 12 | [CQuaternion](../source/game_sa/Core/Quaternion.cpp) (11) ✅ 10 | [CTaskSimpleIKManager](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKManager.cpp) (9) ✅ 9 |
| [CColModelPool](../source/game_sa/Pools/ColModelPool.h) (1) ✅ 1 | [CRadar](../source/game_sa/Radar.cpp) (66) ✅ 66 | [CTaskSimpleIKPointArm](../source/game_sa/Tasks/TaskTypes/TaskSimpleIKPointArm.cpp) (7) ✅ 7 |
| [CColourSet](../source/game_sa/Collision/ColourSet.cpp) (2) ✅ 2 | [CRealTimeShadow](../source/game_sa/RealTimeShadow.cpp) (11) ✅ 11 | [CTaskSimpleInAir](../source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp) (9) ✅ 9 |
| [CColPoint](../source/game_sa/Collision/ColPoint.cpp) (4) ✅ 4 | [CRealTimeShadowManager](../source/game_sa/RealTimeShadowManager.cpp) (9) ✅ 9 | [CTaskSimpleJetPack](../source/game_sa/Tasks/TaskTypes/TaskSimpleJetPack.cpp) (15) ✅ 15 |
| [CColSphere](../source/game_sa/Collision/ColSphere.cpp) (5) ✅ 5 | [CRect](../source/game_sa/Core/Rect.cpp) (10) ✅ 10 | [CTaskSimpleJump](../source/game_sa/Tasks/TaskTypes/TaskSimpleJump.cpp) (11) ✅ 11 |
| [CColStore](../source/game_sa/Collision/ColStore.cpp) (29) ✅ 29 | [CReferences](../source/game_sa/References.cpp) (4) ✅ 4 | [CTaskSimpleKillPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp) (9) ✅ 9 |
| [CColTrianglePlane](../source/game_sa/Collision/ColTrianglePlane.cpp) (2) ✅ 2 | [CRegisteredCorona](../source/game_sa/RegisteredCorona.cpp) (1) ✅ 1 | [CTaskSimpleLand](../source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp) (10) ✅ 10 |
| [CCompressedMatrixNotAligned](../source/game_sa/CompressedMatrixNotAligned.cpp) (2) ✅ 2 | [CRegisteredMotionBlurStreak](../source/game_sa/RegisteredMotionBlurStreak.cpp) (2) ✅ 2 | [CTaskSimpleLeaveGroup](../source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp) (2) ✅ 2 |
| [CControllerConfigManager](../source/game_sa/ControllerConfigManager.cpp) (58) ✅ 58 | [CRemote](../source/game_sa/Remote.cpp) (3) ✅ 3 | [CTaskSimpleLookAbout](../source/game_sa/Tasks/TaskTypes/TaskSimpleLookAbout.cpp) (5) ✅ 5 |
| [CControllerState](../source/game_sa/ControllerState.cpp) (2) ✅ 2 | [CRenderer](../source/game_sa/Renderer.cpp) (35) ✅ 35 | [CTaskSimpleNone](../source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp) (5) ✅ 5 |
| [CConversationForPed](../source/game_sa/Conversations/ConversationForPed.cpp) (2) ✅ 2 | [CReplay](../source/game_sa/Replay.cpp) (55) ✅ 48 | [CTaskSimpleOnEscalator](../source/game_sa/Tasks/TaskTypes/TaskSimpleOnEscalator.cpp) (5) ✅ 5 |
| [CConversationNode](../source/game_sa/Conversations/ConversationNode.cpp) (1) ✅ 1 | [CRestart](../source/game_sa/Restart.cpp) (11) ✅ 11 | [CTaskSimplePause](../source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp) (7) ✅ 7 |
| [CConversations](../source/game_sa/Conversations/Conversations.cpp) (11) ✅ 11 | [CRGBA](../source/game_sa/RGBA.cpp) (2) ✅ 2 | [CTaskSimplePickUpBike](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpBike.cpp) (10) ✅ 10 |
| [CCopPed](../source/game_sa/Entity/Ped/CopPed.cpp) (8) ✅ 8 | [CRideAnimData](../source/game_sa/RideAnimData.cpp) (2) ✅ 2 | [CTaskSimplePickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePickUpEntity.cpp) (5) ✅ 5 |
| [CCoronas](../source/game_sa/Coronas.cpp) (12) ✅ 12 | [CRoadBlocks](../source/game_sa/RoadBlocks.cpp) (8) ✅ 8 | [CTaskSimplePlayerOnFire](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFire.cpp) (7) ✅ 7 |
| [CCover](../source/game_sa/Cover.cpp) (17) ✅ 16 | [CRope](../source/game_sa/Rope.cpp) (7) ✅ 7 | [CTaskSimplePlayerOnFoot](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp) (13) ✅ 13 |
| [CCoverPoint](../source/game_sa/CoverPoint.cpp) (3) ✅ 3 | [CRopes](../source/game_sa/Ropes.cpp) (11) ✅ 11 | [CTaskSimplePlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp) (8) ✅ 8 |
| [CCranes](../source/game_sa/Cranes.cpp) (5) ✅ 5 | [CRubbish](../source/game_sa/Rubbish.cpp) (1) ✅ 1 | [CTaskSimplePutDownEntity](../source/game_sa/Tasks/TaskTypes/TaskSimplePutDownEntity.cpp) (4) ✅ 4 |
| [CCredits](../source/game_sa/Credits.cpp) (6) ✅ 6 | [CRunningScript](../source/game_sa/Scripts/RunningScript.cpp) (43) ✅ 39 | [CTaskSimpleRunAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp) (7) ✅ 7 |
| [CCreepingFire](../source/game_sa/CreepingFire.cpp) (3) ✅ 3 | [CSaveGarage](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleRunAnimLoopedMiddle](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnimLoopedMiddle.cpp) (7) ✅ 7 |
| [CCrime](../source/game_sa/Crime.cpp) (2) ✅ 2 | [CScripted2dEffects](../source/game_sa/Scripts/Scripted2dEffects.cpp) (6) ✅ 6 | [CTaskSimpleRunNamedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp) (10) ✅ 10 |
| [CCullZones](../source/game_sa/CullZones.cpp) (21) ✅ 21 | [CScriptedBrainTaskEntry](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (1) ✅ 1 | [CTaskSimpleRunTimedAnim](../source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp) (8) ✅ 8 |
| [CCurrentVehicle](../source/game_sa/CurrentVehicle.cpp) (2) ✅ 2 | [CScriptedBrainTaskStore](../source/game_sa/Tasks/ScriptedBrainTaskStore.cpp) (4) ✅ 4 | [CTaskSimpleSay](../source/game_sa/Tasks/TaskTypes/TaskSimpleSay.cpp) (7) ✅ 7 |
| [CCurves](../source/game_sa/Curves.cpp) (6) ✅ 6 | [CScriptResourceManager](../source/game_sa/ScriptResourceManager.cpp) (4) ✅ 4 | [CTaskSimpleScratchHead](../source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp) (5) ✅ 5 |
| [CCustomBuildingDNPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp) (18) ✅ 18 | [CScriptsForBrains](../source/game_sa/ScriptsForBrains.cpp) (15) ✅ 11 | [CTaskSimpleSetCharDecisionMaker](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.cpp) (7) ✅ 7 |
| [CCustomBuildingPipeline](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingPipeline.cpp) (7) ✅ 7 | [CSearchLight](../source/game_sa/SearchLight.cpp) (1) ✅ 1 | [CTaskSimpleSetCharIgnoreWeaponRangeFlag](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp) (7) ✅ 6 |
| [CCustomBuildingRenderer](../source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp) (7) ✅ 7 | [CSector](../source/game_sa/Sector.cpp) (1) ✅ 1 | [CTaskSimpleSetKindaStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.cpp) (7) ✅ 7 |
| [CCustomCarEnvMapPipeline](../source/game_sa/Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp) (60) ✅ 37 | [CSetPiece](../source/game_sa/SetPiece.cpp) (7) ✅ 7 | [CTaskSimpleSetPedAsAutoDriver](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.cpp) (7) ✅ 7 |
| [CCustomCarPlateMgr](../source/game_sa/CustomCarPlateMgr.cpp) (16) ✅ 16 | [CSetPieces](../source/game_sa/SetPieces.cpp) (5) ✅ 5 | [CTaskSimpleSetStayInSamePlace](../source/game_sa/Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.cpp) (7) ✅ 7 |
| [CCustomRoadsignMgr](../source/game_sa/CustomRoadsignMgr.cpp) (10) ✅ 10 | [CSexyPedScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 | [CTaskSimpleShakeFist](../source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp) (9) ✅ 9 |
| [CCutsceneMgr](../source/game_sa/CutsceneMgr.cpp) (37) ✅ 37 | [CShadowCamera](../source/game_sa/ShadowCamera.cpp) (20) ✅ 20 | [CTaskSimpleSitDown](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitDown.cpp) (9) ✅ 9 |
| [CCutsceneObject](../source/game_sa/Entity/Object/CutsceneObject.cpp) (10) ✅ 10 | [CShadows](../source/game_sa/Shadows.cpp) (30) ✅ 30 | [CTaskSimpleSitIdle](../source/game_sa/Tasks/TaskTypes/TaskSimpleSitIdle.cpp) (8) ✅ 8 |
| [CDamageAtomicModelInfo](../source/game_sa/Models/DamageAtomicModelInfo.cpp) (9) ✅ 9 | [CShinyTexts](../source/game_sa/ShinyTexts.cpp) (4) ✅ 4 | [CTaskSimpleSlideToCoord](../source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp) (6) ✅ 4 |
| [CDamageManager](../source/game_sa/DamageManager.cpp) (27) ✅ 27 | [CShopping](../source/game_sa/Shopping.cpp) (33) ✅ 33 | [CTaskSimpleStandStill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp) (7) ✅ 7 |
| [CDarkel](../source/game_sa/Darkel.cpp) (19) ✅ 19 | [CShotInfo](../source/game_sa/ShotInfo.cpp) (5) ✅ 5 | [CTaskSimpleStandUp](../source/game_sa/Tasks/TaskTypes/TaskSimpleStandUp.cpp) (9) ✅ 9 |
| [CData](../source/game_sa/Text/Text.cpp) (3) ✅ 3 | [CSimpleTransform](../source/game_sa/SimpleTransform.cpp) (3) ✅ 3 | [CTaskSimpleStealthKill](../source/game_sa/Tasks/TaskTypes/TaskSimpleStealthKill.cpp) (9) ✅ 9 |
| [CDebug](../source/game_sa/Debug.cpp) (2) ✅ 2 | [CSimpleVariablesSaveStructure](../source/game_sa/SimpleVariablesSaveStructure.cpp) (2) ✅ 2 | [CTaskSimpleSwim](../source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp) (15) ✅ 15 |
| [CDecision](../source/game_sa/DecisionMakers/Decision.cpp) (7) ✅ 7 | [CSkidmarks](../source/game_sa/Skidmarks.cpp) (7) ✅ 7 | [CTaskSimpleThrowControl](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp) (7) ✅ 7 |
| [CDecisionMaker](../source/game_sa/DecisionMakers/DecisionMaker.cpp) (5) ✅ 3 | [CSpecialFX](../source/game_sa/SpecialFX.cpp) (7) ✅ 7 | [CTaskSimpleThrowProjectile](../source/game_sa/Tasks/TaskTypes/TaskSimpleThrowProjectile.cpp) (10) ✅ 10 |
| [CDecisionMakerTypes](../source/game_sa/DecisionMakers/DecisionMakerTypes.cpp) (17) ✅ 17 | [CSpecialPlateHandler](../source/game_sa/SpecialPlateHandler.cpp) (4) ✅ 4 | [CTaskSimpleTired](../source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp) (8) ✅ 8 |
| [CDecisionMakerTypesFileLoader](../source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp) (7) ✅ 7 | [CSphere](../source/game_sa/Collision/Sphere.cpp) (1) ✅ 1 | [CTaskSimpleTogglePedThreatScanner](../source/game_sa/Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.cpp) (7) ✅ 7 |
| [CDirectory](../source/game_sa/Directory.cpp) (11) ✅ 11 | [CSprite](../source/game_sa/Sprite.cpp) (15) ✅ 15 | [CTaskSimpleTriggerLookAt](../source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp) (7) ✅ 7 |
| [CDoor](../source/game_sa/Door.cpp) (6) ✅ 6 | [CSprite2d](../source/game_sa/Sprite2d.cpp) (35) ✅ 35 | [CTaskSimpleTurn180](../source/game_sa/Tasks/TaskTypes/TaskSimpleTurn180.cpp) (7) ✅ 7 |
| [CDraw](../source/game_sa/Draw.cpp) (3) ✅ 3 | [CStaticShadow](../source/game_sa/Shadows.cpp) (1) ✅ 1 | [CTaskSimpleUninterruptable](../source/game_sa/Tasks/TaskTypes/TaskSimpleUninterruptable.cpp) (7) ✅ 7 |
| [CdStream](../source/game_sa/CdStreamInfo.cpp) (11) ✅ 11 | [CStats](../source/game_sa/Stats.cpp) (52) ✅ 52 | [CTaskSimpleUseAtm](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseAtm.cpp) (5) ✅ 5 |
| [CDummy](../source/game_sa/Entity/Dummy/Dummy.cpp) (6) ✅ 6 | [CStencilShadowObject](../source/game_sa/StencilShadowObject.cpp) (4) ✅ 4 | [CTaskSimpleUseGun](../source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp) (22) ✅ 22 |
| [CDummyObject](../source/game_sa/Entity/Dummy/DummyObject.cpp) (6) ✅ 6 | [CStencilShadows](../source/game_sa/StencilShadows.cpp) (14) ✅ 14 | [CTaskSimpleWaitForBus](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp) (7) ✅ 7 |
| [CDummyPed](../source/game_sa/Entity/Dummy/DummyPed.cpp) (3) ✅ 3 | [CStoredCar](../source/game_sa/Garage.cpp) (2) ✅ 2 | [CTaskSimpleWaitForPizza](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp) (7) ✅ 7 |
| [CDummyPool](../source/game_sa/Pools/DummyPool.h) (1) ✅ 1 | [CStreamedScripts](../source/game_sa/StreamedScripts.cpp) (12) ✅ 12 | [CTaskSimpleWaitUntilAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.cpp) (7) ✅ 7 |
| [CEmergencyPed](../source/game_sa/Entity/Ped/EmergencyPed.cpp) (4) ✅ 4 | [CStreaming](../source/game_sa/Streaming.cpp) (110) ✅ 109 | [CTaskSimpleWaitUntilLeaderAreaCodesMatch](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilLeaderAreaCodesMatch.cpp) (6) ✅ 6 |
| [CEntity](../source/game_sa/Entity/Entity.cpp) (76) ✅ 73 | [CStreamingInfo](../source/game_sa/StreamingInfo.cpp) (7) ✅ 7 | [CTaskSimpleWaitUntilPedIsInCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.cpp) (7) ✅ 7 |
| [CEntityScanner](../source/game_sa/EntityScanner.cpp) (4) ✅ 4 | [CStuckCarCheck](../source/game_sa/StuckCarCheck.cpp) (9) ✅ 9 | [CTaskSimpleWaitUntilPedIsOutCar](../source/game_sa/Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.cpp) (7) ✅ 7 |
| [CEntitySeekPosCalculator](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp) (4) ✅ 4 | [CStuntJumpManager](../source/game_sa/StuntJumpManager.cpp) (13) ✅ 13 | [CTaskTimer](../source/game_sa/Tasks/TaskTimer.cpp) (2) ✅ 2 |
| [CEntitySeekPosCalculatorFixedPos](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorFixedPos.cpp) (1) ✅ 1 | [CTagManager](../source/game_sa/TagManager.cpp) (20) ✅ 20 | [CTaskUtilityLineUpPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp) (6) ✅ 6 |
| [CEntitySeekPosCalculatorRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorRadiusAngleOffset.cpp) (1) ✅ 1 | [CTask](../source/game_sa/Tasks/Task.cpp) (8) ✅ 8 | [CTempColModels](../source/game_sa/Collision/TempColModels.cpp) (2) ✅ 2 |
| [CEntitySeekPosCalculatorStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorStandard.cpp) (1) ✅ 1 | [CTaskAllocator](../source/game_sa/Tasks/Allocators/TaskAllocator.cpp) (7) ✅ 7 | [CText](../source/game_sa/Text/Text.cpp) (9) ✅ 9 |
| [CEntitySeekPosCalculatorXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp) (2) ✅ 2 | [CTaskAllocatorAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp) (6) ✅ 6 | [CTheCarGenerators](../source/game_sa/TheCarGenerators.cpp) (7) ✅ 7 |
| [CEntryExit](../source/game_sa/EntryExit.cpp) (13) ✅ 13 | [CTaskAllocatorKillOnFoot](../source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp) (5) ✅ 5 | [CTheScripts](../source/game_sa/Scripts/TheScripts.cpp) (82) ✅ 72 |
| [CEntryExitManager](../source/game_sa/EntryExitManager.cpp) (28) ✅ 28 | [CTaskAllocatorKillThreatsBasic](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp) (7) ✅ 7 | [CTheZones](../source/game_sa/TheZones.cpp) (23) ✅ 23 |
| [CEntryInfoList](../source/game_sa/Core/EntryInfoList.cpp) (2) ✅ 1 | [CTaskAllocatorKillThreatsBasicRandomGroup](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasicRandomGroup.cpp) (4) ✅ 4 | [CTimeCycle](../source/game_sa/TimeCycle.cpp) (20) ✅ 20 |
| [CEntryInfoNode](../source/game_sa/Core/EntryInfoNode.cpp) (2) ✅ 2 | [CTaskAllocatorKillThreatsDriveby](../source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp) (6) ✅ 6 | [CTimeInfo](../source/game_sa/Models/TimeInfo.cpp) (6) ✅ 6 |
| [CEntryInfoNodePool](../source/game_sa/Pools/EntryInfoNodePool.h) (1) ✅ 1 | [CTaskAllocatorPlayerCommandAttack](../source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp) (6) ✅ 6 | [CTimeModelInfo](../source/game_sa/Models/TimeModelInfo.cpp) (5) ✅ 5 |
| [CEscalator](../source/game_sa/Escalator.cpp) (5) ✅ 5 | [CTaskAllocatorPool](../source/game_sa/Pools/TaskAllocatorPool.h) (1) ✅ 1 | [CTimer](../source/game_sa/Timer.cpp) (17) ✅ 17 |
| [CEscalators](../source/game_sa/Escalators.cpp) (4) ✅ 4 | [CTaskCategories](../source/game_sa/Tasks/TaskCategories.cpp) (3) ✅ 3 | [CTrafficLights](../source/game_sa/TrafficLights.cpp) (12) ✅ 12 |
| [CEvent](../source/game_sa/Events/Event.cpp) (17) ✅ 17 | [CTaskComplex](../source/game_sa/Tasks/Task.cpp) · [archivo 2](../source/game_sa/Tasks/TaskComplex.cpp) (7) ✅ 7 | [CTrailer](../source/game_sa/Entity/Vehicle/Trailer.cpp) (16) ✅ 16 |
| [CEventAcquaintancePed](../source/game_sa/Events/EventAcquaintancePed.cpp) (6) ✅ 6 | [CTaskComplexArrestPed](../source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp) (10) ✅ 10 | [CTrain](../source/game_sa/Entity/Vehicle/Train.cpp) (49) ✅ 49 |
| [CEventAcquaintancePedDislike](../source/game_sa/Events/EventAcquaintancePedDislike.cpp) (3) ✅ 3 | [CTaskComplexAttractorPartnerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexAttractorPartnerWait.cpp) (8) ✅ 8 | [CTrainNode](../source/game_sa/TrainNode.cpp) (2) ✅ 2 |
| [CEventAcquaintancePedHate](../source/game_sa/Events/EventAcquaintancePedHate.cpp) (9) ✅ 9 | [CTaskComplexAvoidEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp) (13) ✅ 13 | [cTransmission](../source/game_sa/cTransmission.cpp) (5) ✅ 5 |
| [CEventAcquaintancePedHateBadlyLit](../source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp) (8) ✅ 8 | [CTaskComplexAvoidOtherPedWhileWandering](../source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp) (16) ✅ 16 | [CTxdStore](../source/game_sa/TxdStore.cpp) (33) ✅ 33 |
| [CEventAcquaintancePedLike](../source/game_sa/Events/EventAcquaintancePedLike.cpp) (3) ✅ 3 | [CTaskComplexBeCop](../source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.cpp) (5) ✅ 4 | [CUpsideDownCarCheck](../source/game_sa/UpsideDownCarCheck.cpp) (5) ✅ 5 |
| [CEventAcquaintancePedRespect](../source/game_sa/Events/EventAcquaintancePedRespect.cpp) (3) ✅ 3 | [CTaskComplexBeInCouple](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInCouple.cpp) (11) ✅ 11 | [CUserDisplay](../source/game_sa/UserDisplay.cpp) (2) ✅ 2 |
| [CEventAreaCodes](../source/game_sa/Events/EventAreaCodes.cpp) (8) ✅ 8 | [CTaskComplexBeInGroup](../source/game_sa/Tasks/TaskTypes/TaskComplexBeInGroup.cpp) (11) ✅ 11 | [CVector](../source/game_sa/Core/Vector.cpp) (24) ✅ 24 |
| [CEventAttractor](../source/game_sa/Events/EventAttractor.cpp) (9) ✅ 9 | [CTaskComplexBeStill](../source/game_sa/Tasks/TaskTypes/TaskComplexBeStill.cpp) (8) ✅ 8 | [CVector2D](../source/game_sa/Core/Vector.cpp) · [archivo 2](../source/game_sa/Core/Vector2D.cpp) (3) ✅ 3 |
| [CEventBuildingCollision](../source/game_sa/Events/EventBuildingCollision.cpp) (12) ✅ 12 | [CTaskComplexCarDrive](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp) (13) ✅ 13 | [CVehicle](../source/game_sa/Entity/Vehicle/Vehicle.cpp) (202) ✅ 200 |
| [CEventCarUpsideDown](../source/game_sa/Events/EventCarUpsideDown.cpp) (8) ✅ 8 | [CTaskComplexCarDriveMission](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (6) ✅ 6 | [CVehicleAnimGroup](../source/game_sa/VehicleAnimGroupData.cpp) (6) ✅ 6 |
| [CEventChatPartner](../source/game_sa/Events/EventChatPartner.cpp) (5) ✅ 5 | [CTaskComplexCarDriveMissionFleeScene](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CVehicleAnimGroupData](../source/game_sa/VehicleAnimGroupData.cpp) (6) ✅ 6 |
| [CEventCopCarBeingStolen](../source/game_sa/Events/EventCopCarBeingStolen.cpp) (8) ✅ 8 | [CTaskComplexCarDriveMissionKillPed](../source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp) (5) ✅ 5 | [CVehicleModelInfo](../source/game_sa/Models/VehicleModelInfo.cpp) (80) ✅ 80 |
| [CEventCreatePartnerTask](../source/game_sa/Events/EventCreatePartnerTask.cpp) (4) ✅ 4 | [CTaskComplexCarDriveWander](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp) (7) ✅ 7 | [CVehiclePool](../source/game_sa/Pools/VehiclePool.h) (5) ✅ 5 |
| [CEventDamage](../source/game_sa/Events/EventDamage.cpp) (25) ✅ 25 | [CTaskComplexCarSlowBeDraggedOut](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOut.cpp) (12) ✅ 12 | [CVehiclePotentialCollisionScanner](../source/game_sa/Events/EventScanner.cpp) (1) ✅ 1 |
| [CEventDanger](../source/game_sa/Events/EventDanger.cpp) (10) ✅ 10 | [CTaskComplexCarSlowBeDraggedOutAndStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexCarSlowBeDraggedOutAndStandUp.cpp) (10) ✅ 10 | [CVehicleRecording](../source/game_sa/VehicleRecording.cpp) (29) ✅ 29 |
| [CEventDeadPed](../source/game_sa/Events/EventDeadPed.cpp) (4) ✅ 4 | [CTaskComplexChat](../source/game_sa/Tasks/TaskTypes/TaskComplexChat.cpp) (8) ✅ 8 | [CVehicleSaveStructure](../source/game_sa/VehicleSaveStructure.cpp) (3) ✅ 3 |
| [CEventDeath](../source/game_sa/Events/EventDeath.cpp) (3) ✅ 3 | [CTaskComplexClimb](../source/game_sa/Tasks/TaskTypes/TaskComplexClimb.cpp) (5) ✅ 5 | [CVehicleScanner](../source/game_sa/VehicleScanner.cpp) (3) ✅ 3 |
| [CEventDontJoinPlayerGroup](../source/game_sa/Events/EventDontJoinPlayerGroup.cpp) (8) ✅ 8 | [CTaskComplexCopInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexCopInCar.cpp) (10) ✅ 10 | [CVehicleStructure](../source/game_sa/Models/VehicleModelInfo.cpp) (5) ✅ 5 |
| [CEventDraggedOutCar](../source/game_sa/Events/EventDraggedOutCar.cpp) (5) ✅ 5 | [CTaskComplexCrossRoadLookAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexCrossRoadLookAndAchieveHeading.cpp) (10) ✅ 10 | [CVisibilityPlugins](../source/game_sa/VisibilityPlugins.cpp) (84) ✅ 84 |
| [CEventEditableResponse](../source/game_sa/Events/EventEditableResponse.cpp) (13) ✅ 13 | [CTaskComplexDestroyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCar.cpp) (10) ✅ 10 | [CWanted](../source/game_sa/Wanted.cpp) (35) ✅ 35 |
| [CEventEscalator](../source/game_sa/Events/EventEscalator.cpp) (9) ✅ 9 | [CTaskComplexDestroyCarArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarArmed.cpp) (11) ✅ 11 | [CWaterCannon](../source/game_sa/WaterCannon.cpp) (5) ✅ 5 |
| [CEventFireNearby](../source/game_sa/Events/EventFireNearby.cpp) (4) ✅ 4 | [CTaskComplexDestroyCarMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexDestroyCarMelee.cpp) (11) ✅ 11 | [CWaterCannons](../source/game_sa/WaterCannons.cpp) (4) ✅ 4 |
| [CEventGlobalGroup](../source/game_sa/Events/EventGlobalGroup.cpp) (5) ✅ 5 | [CTaskComplexDie](../source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp) (10) ✅ 10 | [CWaterLevel](../source/game_sa/WaterLevel.cpp) (49) ✅ 48 |
| [CEventGotKnockedOverByCar](../source/game_sa/Events/EventGotKnockedOverByCar.cpp) (4) ✅ 4 | [CTaskComplexDieInCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp) (11) ✅ 11 | [CWeapon](../source/game_sa/Weapon.cpp) (41) ✅ 41 |
| [CEventGroup](../source/game_sa/Events/EventGroup.cpp) (14) ✅ 14 | [CTaskComplexDiveFromAttachedEntityAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexDiveFromAttachedEntityAndGetUp.cpp) (8) ✅ 8 | [CWeaponEffects](../source/game_sa/WeaponEffects.cpp) (10) ✅ 10 |
| [CEventGroupEvent](../source/game_sa/Events/EventGroupEvent.cpp) (5) ✅ 5 | [CTaskComplexDragPedFromCar](../source/game_sa/Tasks/TaskTypes/TaskComplexDragPedFromCar.cpp) (7) ✅ 7 | [CWeaponInfo](../source/game_sa/WeaponInfo.cpp) (16) ✅ 16 |
| [CEventGunAimedAt](../source/game_sa/Events/EventGunAimedAt.cpp) (6) ✅ 6 | [CTaskComplexDriveFireTruck](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp) (9) ✅ 9 | [CWeaponModelInfo](../source/game_sa/Models/WeaponModelInfo.cpp) (6) ✅ 6 |
| [CEventGunShot](../source/game_sa/Events/EventGunShot.cpp) (14) ✅ 14 | [CTaskComplexDrivePointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexDrivePointRoute.cpp) (8) ✅ 8 | [CWeather](../source/game_sa/Weather.cpp) (14) ✅ 14 |
| [CEventGunShotWhizzedBy](../source/game_sa/Events/EventGunShotWhizzedBy.cpp) (9) ✅ 9 | [CTaskComplexDriveToPoint](../source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp) (10) ✅ 10 | [CWindModifiers](../source/game_sa/WindModifiers.cpp) (2) ✅ 2 |
| [CEventHandler](../source/game_sa/Events/EventHandler.cpp) (72) ✅ 72 | [CTaskComplexEnterAnyCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp) (8) ✅ 8 | [CWinsockLink](../source/game_sa/WinsockLink.cpp) (2) ✅ 2 |
| [CEventHandlerHistory](../source/game_sa/Events/EventHandlerHistory.cpp) (12) ✅ 12 | [CTaskComplexEnterBoatAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.cpp) (9) ✅ 9 | [CWorld](../source/game_sa/World.cpp) (96) ✅ 96 |
| [CEventHealthLow](../source/game_sa/Events/EventHealthLow.cpp) (4) ✅ 4 | [CTaskComplexEnterCar](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp) (17) ✅ 17 | [CWorldScan](../source/game_sa/Renderer.cpp) (2) ✅ 2 |
| [CEventHealthReallyLow](../source/game_sa/Events/EventHealthReallyLow.cpp) (4) ✅ 4 | [CTaskComplexEnterCarAsDriver](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriver.cpp) (5) ✅ 5 | [CZone](../source/game_sa/Zone.cpp) (1) ✅ 1 |
| [CEventHighAngerAtPlayer](../source/game_sa/Events/EventHighAngerAtPlayer.cpp) (4) ✅ 4 | [CTaskComplexEnterCarAsDriverTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.cpp) (10) ✅ 10 | [CZoneDef](../source/game_sa/CullZones.cpp) (2) ✅ 1 |
| [CEventHitByWaterCannon](../source/game_sa/Events/EventHitByWaterCannon.cpp) (9) ✅ 9 | [CTaskComplexEnterCarAsPassenger](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp) (5) ✅ 5 | [D3D9RasterFormat](../source/game_sa/RenderWare/D3D9RasterFormat.cpp) (10) |
| [CEventInAir](../source/game_sa/Events/EventInAir.cpp) (8) ✅ 8 | [CTaskComplexEnterCarAsPassengerTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.cpp) (10) ✅ 10 | [D3DIndexDataBuffer](../source/game_sa/RenderWare/D3DIndexDataBuffer.cpp) (5) ✅ 5 |
| [CEventInteriorUseInfo](../source/game_sa/Events/EventInteriorUseInfo.cpp) (5) ✅ 5 | [CTaskComplexEnterCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.cpp) (9) ✅ 9 | [D3DResourceSystem](../source/game_sa/RenderWare/D3DResourceSystem.cpp) (16) ✅ 16 |
| [CEventInWater](../source/game_sa/Events/EventInWater.cpp) (5) ✅ 5 | [CTaskComplexEvasiveCower](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveCower.cpp) (9) ✅ 9 | [D3DTextureBuffer](../source/game_sa/RenderWare/D3DTextureBuffer.cpp) (8) ✅ 8 |
| [CEventKnockOffBike](../source/game_sa/Events/EventKnockOffBike.cpp) (18) ✅ 18 | [CTaskComplexEvasiveDiveAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveDiveAndGetUp.cpp) (10) ✅ 10 | [D3DX9](../source/game_sa/RenderWare/rw/d3dx9math.cpp) (11) |
| [CEventLeaderEnteredCarAsDriver](../source/game_sa/Events/EventLeaderEnteredCarAsDriver.cpp) (11) ✅ 11 | [CTaskComplexEvasiveStep](../source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp) (9) ✅ 9 | [DelayLoadThunks](../source/app/platform/win/DelayLoadThunks.cpp) (3) |
| [CEventLeaderEntryExit](../source/game_sa/Events/EventLeaderEntryExit.cpp) (12) ✅ 12 | [CTaskComplexExtinguishFireOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFireOnFoot.cpp) (9) ✅ 9 | [ExeLibJpeg](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (15) |
| [CEventLeaderExitedCarAsDriver](../source/game_sa/Events/EventLeaderExitedCarAsDriver.cpp) (10) ✅ 10 | [CTaskComplexExtinguishFires](../source/game_sa/Tasks/TaskTypes/TaskComplexExtinguishFires.cpp) (9) ✅ 9 | [ExeProtectionStub](../source/game_sa/ExeProtectionStub.cpp) (8) |
| [CEventLeaderQuitEnteringCarAsDriver](../source/game_sa/Events/EventLeaderQuitEnteringCarAsDriver.cpp) (10) ✅ 10 | [CTaskComplexFacial](../source/game_sa/Tasks/TaskTypes/TaskComplexFacial.cpp) (11) ✅ 11 | [Furniture_c](../source/game_sa/Interior/Furniture_c.cpp) (1) ✅ 1 |
| [CEventLeanOnVehicle](../source/game_sa/Events/EventLeanOnVehicle.cpp) (9) ✅ 9 | [CTaskComplexFallAndGetUp](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp) (13) ✅ 13 | [FurnitureGroup_c](../source/game_sa/Interior/FurnitureGroup_c.cpp) (7) ✅ 7 |
| [CEventLowAngerAtPlayer](../source/game_sa/Events/EventLowAngerAtPlayer.cpp) (4) ✅ 4 | [CTaskComplexFallAndStayDown](../source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp) (12) ✅ 12 | [FurnitureManager_c](../source/game_sa/Interior/FurnitureManager_c.cpp) (9) ✅ 9 |
| [CEventNewGangMember](../source/game_sa/Events/EventNewGangMember.cpp) (8) ✅ 8 | [CTaskComplexFallToDeath](../source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp) (10) ✅ 10 | [FurnitureSubGroup_c](../source/game_sa/Interior/FurnitureSubGroup_c.cpp) (6) ✅ 6 |
| [CEventObjectCollision](../source/game_sa/Events/EventObjectCollision.cpp) (10) ✅ 10 | [CTaskComplexFleeAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeAnyMeans.cpp) (9) ✅ 9 | [Fx_c](../source/game_sa/Fx/Fx.cpp) (41) ✅ 41 |
| [CEventOnFire](../source/game_sa/Events/EventOnFire.cpp) (9) ✅ 9 | [CTaskComplexFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeEntity.cpp) (9) ✅ 9 | [FxEmitter_c](../source/game_sa/Fx/FxEmitter.cpp) (9) ✅ 4 |
| [CEventPassObject](../source/game_sa/Events/EventPassObject.cpp) (9) ✅ 9 | [CTaskComplexFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexFleePoint.cpp) (12) ✅ 12 | [FxEmitterBP_c](../source/game_sa/Fx/FxEmitterBP.cpp) (13) ✅ 9 |
| [CEventPedCollisionWithPed](../source/game_sa/Events/EventPedCollisionWithPed.cpp) (9) ✅ 9 | [CTaskComplexFleeShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexFleeShooting.cpp) (9) ✅ 9 | [FxEmitterPrt_c](../source/game_sa/Fx/FxEmitterPrt.cpp) (2) ✅ 2 |
| [CEventPedCollisionWithPlayer](../source/game_sa/Events/EventPedCollisionWithPlayer.cpp) (5) ✅ 5 | [CTaskComplexFollowLeaderAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderAnyMeans.cpp) (9) ✅ 9 | [FxFrustumInfo_c](../source/game_sa/Fx/FxFrustumInfo.cpp) (2) ✅ 1 |
| [CEventPedEnteredMyVehicle](../source/game_sa/Events/EventPedEnteredMyVehicle.cpp) (5) ✅ 5 | [CTaskComplexFollowLeaderInFormation](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowLeaderInFormation.cpp) (12) ✅ 12 | [FxInfo_c](../source/game_sa/Fx/Blueprints/FxInfo.cpp) (1) |
| [CEventPedToChase](../source/game_sa/Events/EventPedToChase.cpp) (4) ✅ 4 | [CTaskComplexFollowNodeRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowNodeRoute.cpp) (20) ✅ 20 | [FxInfoAnimTexture_c](../source/game_sa/Fx/Blueprints/FxInfoAnimTexture.cpp) (3) |
| [CEventPedToFlee](../source/game_sa/Events/EventPedToFlee.cpp) (4) ✅ 4 | [CTaskComplexFollowPatrolRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp) (11) ✅ 2 | [FxInfoAttractLine_c](../source/game_sa/Fx/Blueprints/FxInfoAttractLine.cpp) (3) |
| [CEventPlayerCollisionWithPed](../source/game_sa/Events/EventPlayerCollisionWithPed.cpp) (5) ✅ 5 | [CTaskComplexFollowPedFootsteps](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp) (11) ✅ 11 | [FxInfoAttractPt_c](../source/game_sa/Fx/Blueprints/FxInfoAttractPt.cpp) (3) |
| [CEventPlayerCommandToGroup](../source/game_sa/Events/EventPlayerCommandToGroup.cpp) (8) ✅ 8 | [CTaskComplexFollowPointRoute](../source/game_sa/Tasks/TaskTypes/TaskComplexFollowPointRoute.cpp) (15) ✅ 15 | [FxInfoColour_c](../source/game_sa/Fx/Blueprints/FxInfoColour.cpp) (3) |
| [CEventPlayerCommandToGroupAttack](../source/game_sa/Events/EventPlayerCommandToGroupAttack.cpp) (7) ✅ 7 | [CTaskComplexGangFollower](../source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp) (11) ✅ 11 | [FxInfoColourBright_c](../source/game_sa/Fx/Blueprints/FxInfoColourBright.cpp) (3) |
| [CEventPlayerCommandToGroupGather](../source/game_sa/Events/EventPlayerCommandToGroupGather.cpp) (6) ✅ 6 | [CTaskComplexGangJoinRespond](../source/game_sa/Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp) (9) ✅ 9 | [FxInfoColourRange_c](../source/game_sa/Fx/Blueprints/FxInfoColourRange.cpp) (3) |
| [CEventPool](../source/game_sa/Pools/EventPool.h) (1) ✅ 1 | [CTaskComplexGangLeader](../source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp) (16) ✅ 16 | [FxInfoDir_c](../source/game_sa/Fx/Blueprints/FxInfoDir.cpp) (3) |
| [CEventPotentialGetRunOver](../source/game_sa/Events/EventPotentialGetRunOver.cpp) (5) ✅ 5 | [CTaskComplexGetOnBoatSeat](../source/game_sa/Tasks/TaskTypes/TaskComplexGetOnBoatSeat.cpp) (9) ✅ 9 | [FxInfoEmAngle_c](../source/game_sa/Fx/Blueprints/FxInfoEmAngle.cpp) (3) |
| [CEventPotentialWalkIntoFire](../source/game_sa/Events/EventPotentialWalkIntoFire.cpp) (8) ✅ 8 | [CTaskComplexGetUpAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp) (9) ✅ 9 | [FxInfoEmDir_c](../source/game_sa/Fx/Blueprints/FxInfoEmDir.cpp) (3) |
| [CEventPotentialWalkIntoObject](../source/game_sa/Events/EventPotentialWalkIntoObject.cpp) (9) ✅ 9 | [CTaskComplexGoPickUpEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexGoPickUpEntity.cpp) (8) ✅ 8 | [FxInfoEmLife_c](../source/game_sa/Fx/Blueprints/FxInfoEmLife.cpp) (3) |
| [CEventPotentialWalkIntoPed](../source/game_sa/Events/EventPotentialWalkIntoPed.cpp) (10) ✅ 10 | [CTaskComplexGoToAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToAttractor.cpp) (9) ✅ 9 | [FxInfoEmPos_c](../source/game_sa/Fx/Blueprints/FxInfoEmPos.cpp) (3) |
| [CEventPotentialWalkIntoVehicle](../source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp) (9) ✅ 9 | [CTaskComplexGoToBoatSteeringWheel](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToBoatSteeringWheel.cpp) (10) ✅ 10 | [FxInfoEmRate_c](../source/game_sa/Fx/Blueprints/FxInfoEmRate.cpp) (3) |
| [CEventRevived](../source/game_sa/Events/EventRevived.cpp) (2) ✅ 2 | [CTaskComplexGoToCarDoorAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp) (12) ✅ 12 | [FxInfoEmRotation_c](../source/game_sa/Fx/Blueprints/FxInfoEmRotation.cpp) (3) |
| [CEventScanner](../source/game_sa/Events/EventScanner.cpp) (4) ✅ 4 | [CTaskComplexGotoDoorAndOpen](../source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp) (11) ✅ 10 | [FxInfoEmSize_c](../source/game_sa/Fx/Blueprints/FxInfoEmSize.cpp) (3) |
| [CEventScriptCommand](../source/game_sa/Events/EventScriptCommand.cpp) (11) ✅ 11 | [CTaskComplexGoToPointAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAiming.cpp) (9) ✅ 9 | [FxInfoEmSpeed_c](../source/game_sa/Fx/Blueprints/FxInfoEmSpeed.cpp) (3) |
| [CEventScriptedAttractor](../source/game_sa/Events/EventScriptedAttractor.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAndStandStill](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp) (12) ✅ 12 | [FxInfoEmWeather_c](../source/game_sa/Fx/Blueprints/FxInfoEmWeather.cpp) (3) |
| [CEventSeenCop](../source/game_sa/Events/EventSeenCop.cpp) (6) ✅ 6 | [CTaskComplexGoToPointAndStandStillAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillAndAchieveHeading.cpp) (9) ✅ 9 | [FxInfoFlat_c](../source/game_sa/Fx/Blueprints/FxInfoFlat.cpp) (3) |
| [CEventSeenPanickedPed](../source/game_sa/Events/EventSeenPanickedPed.cpp) (9) ✅ 9 | [CTaskComplexGoToPointAndStandStillTimed](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.cpp) (7) ✅ 7 | [FxInfoFloat_c](../source/game_sa/Fx/Blueprints/FxInfoFloat.cpp) (3) |
| [CEventSexyPed](../source/game_sa/Events/EventSexyPed.cpp) (5) ✅ 5 | [CTaskComplexGoToPointAnyMeans](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp) (10) ✅ 9 | [FxInfoForce_c](../source/game_sa/Fx/Blueprints/FxInfoForce.cpp) (3) |
| [CEventSexyVehicle](../source/game_sa/Events/EventSexyVehicle.cpp) (5) ✅ 5 | [CTaskComplexGoToPointShooting](../source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointShooting.cpp) (5) ✅ 5 | [FxInfoFriction_c](../source/game_sa/Fx/Blueprints/FxInfoFriction.cpp) (3) |
| [CEventSoundQuiet](../source/game_sa/Events/EventSoundQuiet.cpp) (11) ✅ 11 | [CTaskComplexHitByGunResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitByGunResponse.cpp) (9) ✅ 9 | [FxInfoGroundCollide_c](../source/game_sa/Fx/Blueprints/FxInfoGroundCollide.cpp) (3) |
| [CEventSource](../source/game_sa/Events/EventSource.cpp) (1) ✅ 1 | [CTaskComplexHitPedWithCar](../source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp) (12) ✅ 12 | [FxInfoHeatHaze_c](../source/game_sa/Fx/Blueprints/FxInfoHeatHaze.cpp) (3) |
| [CEventSpecial](../source/game_sa/Events/EventSpecial.cpp) (1) ✅ 1 | [CTaskComplexHitResponse](../source/game_sa/Tasks/TaskTypes/TaskComplexHitResponse.cpp) (8) ✅ 8 | [FxInfoJitter_c](../source/game_sa/Fx/Blueprints/FxInfoJitter.cpp) (3) |
| [CEventStuckInAir](../source/game_sa/Events/EventStuckInAir.cpp) (10) ✅ 10 | [CTaskComplexInAirAndLand](../source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp) (8) ✅ 8 | [FxInfoManager_c](../source/game_sa/Fx/FxInfoManager.cpp) (6) ✅ 5 |
| [CEventVehicleCollision](../source/game_sa/Events/EventVehicleCollision.cpp) (9) ✅ 9 | [CTaskComplexInvestigateDeadPed](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDeadPed.cpp) (10) ✅ 10 | [FxInfoNoise_c](../source/game_sa/Fx/Blueprints/FxInfoNoise.cpp) (3) |
| [CEventVehicleDamage](../source/game_sa/Events/EventVehicleDamage.cpp) (10) ✅ 10 | [CTaskComplexInvestigateDisturbance](../source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp) (9) ✅ 8 | [FxInfoRotSpeed_c](../source/game_sa/Fx/Blueprints/FxInfoRotSpeed.cpp) (3) |
| [CEventVehicleDamageCollision](../source/game_sa/Events/EventVehicleDamageCollision.cpp) (5) ✅ 5 | [CTaskComplexInWater](../source/game_sa/Tasks/TaskTypes/TaskComplexInWater.cpp) (8) ✅ 8 | [FxInfoSelfLit_c](../source/game_sa/Fx/Blueprints/FxInfoSelfLit.cpp) (3) |
| [CEventVehicleDamageWeapon](../source/game_sa/Events/EventVehicleDamageWeapon.cpp) (5) ✅ 5 | [CTaskComplexJump](../source/game_sa/Tasks/TaskTypes/TaskComplexJump.cpp) (10) ✅ 10 | [FxInfoSize_c](../source/game_sa/Fx/Blueprints/FxInfoSize.cpp) (3) |
| [CEventVehicleDied](../source/game_sa/Events/EventVehicleDied.cpp) (8) ✅ 8 | [CTaskComplexKillAllThreats](../source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp) (9) ✅ 9 | [FxInfoSmoke_c](../source/game_sa/Fx/Blueprints/FxInfoSmoke.cpp) (3) |
| [CEventVehicleHitAndRun](../source/game_sa/Events/EventVehicleHitAndRun.cpp) (11) ✅ 11 | [CTaskComplexKillCriminal](../source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp) (12) ✅ 12 | [FxInfoSpriteRect_c](../source/game_sa/Fx/Blueprints/FxInfoSpriteRect.cpp) (3) |
| [CEventVehicleOnFire](../source/game_sa/Events/EventVehicleOnFire.cpp) (9) ✅ 9 | [CTaskComplexKillPedFromBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedFromBoat.cpp) (8) ✅ 8 | [FxInfoTrail_c](../source/game_sa/Fx/Blueprints/FxInfoTrail.cpp) (3) |
| [CEventVehicleThreat](../source/game_sa/Events/EventVehicleThreat.cpp) (4) ✅ 4 | [CTaskComplexKillPedGroupOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp) (10) ✅ 10 | [FxInfoUnderwater_c](../source/game_sa/Fx/Blueprints/FxInfoUnderwater.cpp) (3) |
| [CEventVehicleToSteal](../source/game_sa/Events/EventVehicleToSteal.cpp) (8) ✅ 8 | [CTaskComplexKillPedOnFoot](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp) (11) ✅ 11 | [FxInfoWind_c](../source/game_sa/Fx/Blueprints/FxInfoWind.cpp) (3) |
| [CExplosion](../source/game_sa/Explosion.cpp) (12) ✅ 12 | [CTaskComplexKillPedOnFootArmed](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp) (12) ✅ 12 | [FxInterpInfo255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo255.cpp) (4) ✅ 1 |
| [CFallingGlassPane](../source/game_sa/FallingGlassPane.cpp) (2) ✅ 2 | [CTaskComplexKillPedOnFootMelee](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootMelee.cpp) (11) ✅ 11 | [FxInterpInfo32_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfo32.cpp) (4) ✅ 1 |
| [CFileLoader](../source/game_sa/FileLoader.cpp) (50) ✅ 50 | [CTaskComplexKillPedOnFootStealth](../source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootStealth.cpp) (8) ✅ 8 | [FxInterpInfoFloat_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoFloat.cpp) (4) ✅ 1 |
| [CFileMgr](../source/game_sa/FileMgr.cpp) (17) ✅ 17 | [CTaskComplexLeaveAnyCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp) (8) ✅ 8 | [FxInterpInfoU255_c](../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.cpp) (4) ✅ 1 |
| [CFire](../source/game_sa/Fire.cpp) (5) ✅ 5 | [CTaskComplexLeaveBoat](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveBoat.cpp) (9) ✅ 9 | [FxManager_c](../source/game_sa/Fx/FxManager.cpp) (25) ✅ 25 |
| [CFireManager](../source/game_sa/FireManager.cpp) (24) ✅ 24 | [CTaskComplexLeaveCar](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp) (14) ✅ 14 | [FxMemoryPool_c](../source/game_sa/Fx/FxMemoryPool.cpp) (7) ✅ 7 |
| [CFont](../source/game_sa/Font.cpp) (40) ✅ 40 | [CTaskComplexLeaveCarAndDie](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp) (6) ✅ 6 | [FxPrim_c](../source/game_sa/Fx/FxPrim.cpp) (3) ✅ 3 |
| [CFontChar](../source/game_sa/Font.cpp) (1) ✅ 1 | [CTaskComplexLeaveCarAndFlee](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.cpp) (9) ✅ 9 | [FxPrimBP_c](../source/game_sa/Fx/FxPrimBP.cpp) (4) ✅ 4 |
| [CFormation](../source/game_sa/Formation.cpp) (10) ✅ 10 | [CTaskComplexLeaveCarAndWander](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp) (9) ✅ 9 | [FxPrtMult_c](../source/game_sa/Fx/FxPrtMult.cpp) (3) ✅ 3 |
| [CGame](../source/game_sa/Game.cpp) (25) ✅ 25 | [CTaskComplexLeaveCarAsPassengerWait](../source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAsPassengerWait.cpp) (11) ✅ 11 | [FxSphere_c](../source/game_sa/Fx/FxSphere.cpp) (5) ✅ 5 |
| [CGameLogic](../source/game_sa/GameLogic.cpp) (27) ✅ 27 | [CTaskComplexMedicTreatInjuredPed](../source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp) (12) ✅ 12 | [FxSystem_c](../source/game_sa/Fx/FxSystem.cpp) (39) ✅ 39 |
| [CGamma](../source/game_sa/Gamma.cpp) (3) ✅ 2 | [CTaskComplexMoveBackAndJump](../source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp) (9) ✅ 9 | [FxSystemBP_c](../source/game_sa/Fx/FxSystemBP.cpp) (10) ✅ 10 |
| [CGangs](../source/game_sa/Gangs.cpp) (8) ✅ 8 | [CTaskComplexObserveTrafficLights](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLights.cpp) (9) ✅ 9 | [Game](../source/app/app_game.cpp) (12) ✅ 12 |
| [CGangWars](../source/game_sa/GangWars.cpp) (36) ✅ 36 | [CTaskComplexObserveTrafficLightsAndAchieveHeading](../source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp) (10) ✅ 10 | [IKChain_c](../source/game_sa/Ragdoll/IKChain.cpp) (20) ✅ 20 |
| [CGangWarsSaveStructure](../source/game_sa/GangWarsSaveStructure.cpp) (2) ✅ 2 | [CTaskComplexOnFire](../source/game_sa/Tasks/TaskTypes/TaskComplexOnFire.cpp) (10) ✅ 10 | [IKChainManager_c](../source/game_sa/Ragdoll/IKChainManager.cpp) (19) ✅ 17 |
| [CGarage](../source/game_sa/Garage.cpp) (32) ✅ 32 | [CTaskComplexOpenDriverDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenDriverDoor.cpp) (5) ✅ 5 | [ImportThunks](../source/app/platform/win/ImportThunks.cpp) (17) |
| [CGarages](../source/game_sa/Garages.cpp) (30) ✅ 30 | [CTaskComplexOpenPassengerDoor](../source/game_sa/Tasks/TaskTypes/TaskComplexOpenPassengerDoor.cpp) (5) ✅ 5 | [Input](../source/app/app_input.cpp) · [archivo 2](../source/app/platform/win/WinInput.cpp) (11) ✅ 11 |
| [CGeneral](../source/game_sa/General.cpp) (9) ✅ 9 | [CTaskComplexPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp) (11) ✅ 11 | [Interior_c](../source/game_sa/Interior/Interior_c.cpp) (54) ✅ 54 |
| [CGenericGameStorage](../source/game_sa/GenericGameStorage.cpp) (20) ✅ 20 | [CTaskComplexPartnerChat](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp) (7) ✅ 7 | [InteriorGroup_c](../source/game_sa/Interior/InteriorGroup_c.cpp) (25) ✅ 25 |
| [CGlass](../source/game_sa/Glass.cpp) (19) ✅ 19 | [CTaskComplexPartnerDeal](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerDeal.cpp) (8) ✅ 8 | [InteriorManager_c](../source/game_sa/Interior/InteriorManager_c.cpp) (23) ✅ 23 |
| [CGrassRenderer](../source/game_sa/GrassRenderer.cpp) (12) ✅ 12 | [CTaskComplexPartnerGreet](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerGreet.cpp) (8) ✅ 8 | [JPegPlugin](../source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp) (6) ✅ 6 |
| [CGridRef](../source/game_sa/GridRef.cpp) (5) ✅ 5 | [CTaskComplexPartnerShove](../source/game_sa/Tasks/TaskTypes/TaskComplexPartnerShove.cpp) (6) ✅ 6 | [LegacyRuntimeError](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (13) |
| [CGroupEventHandler](../source/game_sa/Events/GroupEventHandler.cpp) (32) ✅ 32 | [CTaskComplexPassObject](../source/game_sa/Tasks/TaskTypes/TaskComplexPassObject.cpp) (10) ✅ 10 | [LegacyStdString](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (6) |
| [cHandlingDataMgr](../source/game_sa/cHandlingDataMgr.cpp) (13) ✅ 13 | [CTaskComplexPlayHandSignalAnim](../source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp) (10) ✅ 10 | [LibJpegCompress](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp) (18) |
| [CHandObject](../source/game_sa/Entity/Object/HandObject.cpp) (5) ✅ 5 | [CTaskComplexPolicePursuit](../source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp) (14) ✅ 14 | [LibJpegCompressApi](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressApi.cpp) (3) |
| [CHandShaker](../source/game_sa/HandShaker.cpp) (4) ✅ 4 | [CTaskComplexPresentIDToCop](../source/game_sa/Tasks/TaskTypes/TaskComplexPresentIDToCop.cpp) (10) ✅ 10 | [LibJpegCompressEncode](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp) (11) |
| [CHeli](../source/game_sa/Entity/Vehicle/Heli.cpp) (26) ✅ 26 | [CTaskComplexProstituteSolicit](../source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp) (12) ✅ 12 | [LibJpegDecompress](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompress.cpp) (22) |
| [CHud](../source/game_sa/Hud.cpp) (39) ✅ 39 | [CTaskComplexReactToAttack](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp) (10) ✅ 10 | [LibJpegDecompressApi](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompressApi.cpp) (4) |
| [CHudColours](../source/game_sa/HudColours.cpp) (5) ✅ 5 | [CTaskComplexReactToGunAimedAt](../source/game_sa/Tasks/TaskTypes/TaskComplexReactToGunAimedAt.cpp) (9) ✅ 9 | [LibJpegDecompressUpsample](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompressUpsample.cpp) (3) |
| [CIdleCam](../source/game_sa/IdleCam.cpp) (15) ✅ 15 | [CTaskComplexRoadRage](../source/game_sa/Tasks/TaskTypes/TaskComplexRoadRage.cpp) (10) ✅ 10 | [LibJpegError](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegError.cpp) (3) |
| [CInformFriendsEvent](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexScreamInCarThenLeave](../source/game_sa/Tasks/TaskTypes/TaskComplexScreamInCarThenLeave.cpp) (10) ✅ 10 | [LibJpegMemory](../source/game_sa/Plugins/JPegCompressPlugin/LibJpegMemory.cpp) (21) |
| [CInformFriendsEventQueue](../source/game_sa/InformFriendsEventQueue.cpp) (4) ✅ 4 | [CTaskComplexSeekCoverUntilTargetDead](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekCoverUntilTargetDead.cpp) (11) ✅ 11 | [Light](../source/app/app_light.cpp) (27) ✅ 27 |
| [CInformGroupEvent](../source/game_sa/InformGroupEvent.cpp) (5) ✅ 5 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorFixedPos&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (7) ✅ 3 | [ListItem_c](../source/game_sa/Core/ListItem_c.cpp) (2) |
| [CInformGroupEventQueue](../source/game_sa/InformGroupEventQueue.cpp) (4) ✅ 4 | [CTaskComplexSeekEntity&lt;CEntitySeekPosCalculatorXYOffset&gt;](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h) (12) ✅ 12 | [ModelIndices](../source/game_sa/ModelIndices.cpp) (3) ✅ 3 |
| [CIniFile](../source/game_sa/IniFile.cpp) (1) ✅ 1 | [CTaskComplexSeekEntityAiming](../source/game_sa/Tasks/TaskTypes/TaskComplexSeekEntityAiming.cpp) (8) ✅ 8 | [NodeNamePlugin](../source/game_sa/Plugins/NodeNamePlugin/NodeName.cpp) (3) ✅ 3 |
| [CInterestingEvents](../source/game_sa/InterestingEvents.cpp) (7) ✅ 7 | [CTaskComplexSeekEntityAnyMeansXYOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityAnyMeansXYOffset.cpp) (8) ✅ 8 | [Particle_c](../source/game_sa/Fx/Particle.cpp) (1) ✅ 1 |
| [CIplDefPool](../source/game_sa/Pools/IplDefPool.h) (4) ✅ 4 | [CTaskComplexSeekEntityRadiusAngleOffset](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.cpp) (11) ✅ 9 | [PipelinePlugin](../source/game_sa/Plugins/PipelinePlugin/PipelinePlugin.cpp) (3) ✅ 3 |
| [CIplStore](../source/game_sa/IplStore.cpp) (34) ✅ 34 | [CTaskComplexSeekEntityStandard](../source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp) (11) ✅ 11 | [ProcObjectListItem](../source/game_sa/Plant/ProcObjectMan.cpp) (1) ✅ 1 |
| [CKeyArray](../source/game_sa/Text/Text.cpp) (5) ✅ 5 | [CTaskComplexSequence](../source/game_sa/Tasks/TaskComplexSequence.cpp) (16) ✅ 16 | [ProcObjectMan_c](../source/game_sa/Plant/ProcObjectMan.cpp) (10) ✅ 10 |
| [CKeyboardState](../source/game_sa/KeyboardState.cpp) (1) ✅ 1 | [CTaskComplexShuffleSeats](../source/game_sa/Tasks/TaskTypes/TaskComplexShuffleSeats.cpp) (11) ✅ 11 | [ProcSurfaceInfo_c](../source/game_sa/Plant/ProcSurfaceInfo.cpp) (7) ✅ 7 |
| [CKeyGen](../source/game_sa/Core/KeyGen.cpp) (4) ✅ 4 | [CTaskComplexSignalAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp) (8) ✅ 8 | [Ps](../source/app/platform/win/WinPs.cpp) (26) ✅ 25 |
| [CLinkedUpgradeList](../source/game_sa/Models/VehicleModelInfo.cpp) (2) ✅ 2 | [CTaskComplexSitDownThenIdleThenStandUp](../source/game_sa/Tasks/TaskTypes/TaskComplexSitDownThenIdleThenStandUp.cpp) (10) ✅ 10 | [QTMLRuntime](../source/app/platform/win/QTMLRuntime.cpp) (6) |
| [CLinkList&lt;AlphaObjectInfo&gt;](../source/game_sa/VisibilityPlugins.cpp) (3) ✅ 3 | [CTaskComplexSmartFleeEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleeEntity.cpp) (9) ✅ 9 | [RenderBuffer](../source/game_sa/RenderBuffer.cpp) (4) ✅ 4 |
| [CLinkList&lt;CAnimBlendHierarchy*&gt;](../source/game_sa/Animation/AnimManager.cpp) (2) ✅ 2 | [CTaskComplexSmartFleePoint](../source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp) (13) ✅ 13 | [RpAnimBlend](../source/game_sa/Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp) (52) ✅ 47 |
| [CLinkList&lt;CCollisionData*&gt;](../source/game_sa/Collision/Collision.cpp) (2) ✅ 2 | [CTaskComplexStareAtPed](../source/game_sa/Tasks/TaskTypes/TaskComplexStareAtPed.cpp) (8) ✅ 8 | [RpAnisot](../source/game_sa/RenderWare/rw/rpanisot.cpp) (4) ✅ 3 |
| [CLinkList&lt;CEntity*&gt;](../source/game_sa/Streaming.cpp) (1) ✅ 1 | [CTaskComplexStealCar](../source/game_sa/Tasks/TaskTypes/TaskComplexStealCar.cpp) (9) ✅ 9 | [RpMatFX](../source/game_sa/RenderWare/rw/rpmatfx.cpp) (28) |
| [CLinkList&lt;CPed*&gt;](../source/game_sa/VisibilityPlugins.cpp) (2) ✅ 2 | [CTaskComplexStuckInAir](../source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp) (9) ✅ 9 | [Rs](../source/app/platform/platform.cpp) (33) ✅ 33 |
| [CLoadedCarGroup](../source/game_sa/LoadedCarGroup.cpp) (8) ✅ 8 | [CTaskComplexSunbathe](../source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp) (13) ✅ 13 | [RtBMP](../source/game_sa/RenderWare/rw/rtbmp.cpp) (2) |
| [CLoadingScreen](../source/game_sa/LoadingScreen.cpp) (20) ✅ 20 | [CTaskComplexTrackEntity](../source/game_sa/Tasks/TaskTypes/TaskComplexTrackEntity.cpp) (12) ✅ 12 | [RtQuat](../source/game_sa/RenderWare/rw/rtquat.cpp) (6) |
| [CLoadMonitor](../source/game_sa/LoadMonitor.cpp) (7) ✅ 7 | [CTaskComplexTreatAccident](../source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp) (10) ✅ 10 | [RtSlerp](../source/game_sa/RenderWare/rw/rtslerp.cpp) (1) |
| [CLocalisation](../source/game_sa/Localisation.cpp) (18) ✅ 18 | [CTaskComplexTurnToFaceEntityOrCoord](../source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp) (10) ✅ 9 | [RwD3D9](../source/game_sa/RenderWare/rw/rwcore.cpp) (115) |
| [CLodAtomicModelInfo](../source/game_sa/Models/LodAtomicModelInfo.cpp) (5) ✅ 5 | [CTaskComplexUseAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractor.cpp) (8) ✅ 8 | [RwD3D9RasterDriver](../source/game_sa/RenderWare/D3D9RasterDriver.cpp) (14) |
| [CLodTimeModelInfo](../source/game_sa/Models/LodTimeModelInfo.cpp) (5) ✅ 5 | [CTaskComplexUseAttractorPartner](../source/game_sa/Tasks/TaskTypes/TaskComplexUseAttractorPartner.cpp) (8) ✅ 8 | [RwHelper](../source/game_sa/RwHelper.cpp) (43) ✅ 42 |
| [CMaths](../source/game_sa/Maths.cpp) (6) ✅ 6 | [CTaskComplexUseClosestFreeScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractor.cpp) (9) ✅ 9 | [Securom](../source/app/securom/Securom.cpp) (4) ✅ 4 |
| [CMatrix](../source/game_sa/Core/Matrix.cpp) (49) ✅ 49 | [CTaskComplexUseClosestFreeScriptedAttractorRun](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorRun.cpp) (5) ✅ 5 | [SurfaceInfos_c](../source/game_sa/SurfaceInfos_c.cpp) (50) ✅ 48 |
| [CMatrixLink](../source/game_sa/Core/MatrixLink.cpp) (2) ✅ 2 | [CTaskComplexUseClosestFreeScriptedAttractorSprint](../source/game_sa/Tasks/TaskTypes/TaskComplexUseClosestFreeScriptedAttractorSprint.cpp) (5) ✅ 5 | [tColLighting](../source/game_sa/Collision/ColPoint.cpp) (1) ✅ 1 |
| [CMatrixLinkList](../source/game_sa/Core/MatrixLinkList.cpp) (12) ✅ 12 | [CTaskComplexUseEffect](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp) (9) ✅ 9 | [TList_c](../source/game_sa/Core/List_c.cpp) (8) ✅ 3 |
| [CMBlur](../source/game_sa/MBlur.cpp) (5) ✅ 5 | [CTaskComplexUseEffectRunning](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectRunning.cpp) (5) ✅ 5 | [tReplayTrainBlock](../source/game_sa/Replay.cpp) (1) |
| [CMemoryHeap](../source/game_sa/Memory/MemoryHeap.cpp) (6) ✅ 1 | [CTaskComplexUseEffectSprinting](../source/game_sa/Tasks/TaskTypes/TaskComplexUseEffectSprinting.cpp) (5) ✅ 5 | [tUserList](../source/game_sa/Scripts/Scripted2dEffects.cpp) (2) ✅ 2 |
| [CMemoryMgr](../source/game_sa/MemoryMgr.cpp) (11) ✅ 10 | [CTaskComplexUseGoggles](../source/game_sa/Tasks/TaskTypes/TaskComplexUseGoggles.cpp) (9) ✅ 9 | [UserTracksLegacyVector](../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp) (2) ✅ 1 |
| [CMentalState](../source/game_sa/MentalHealth.cpp) (1) ✅ 1 | [CTaskComplexUseMobilePhone](../source/game_sa/Tasks/TaskTypes/TaskComplexUseMobilePhone.cpp) (11) ✅ 11 | [VectorConstructorIterator](../source/game_sa/RuntimeHelpers.cpp) (1) ✅ 1 |
| [CMenuManager](../source/game_sa/MenuManager.cpp) (62) ✅ 62 | [CTaskComplexUsePairedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUsePairedAttractor.cpp) (12) ✅ 12 | [VideoMode](../source/app/platform/win/VideoMode.cpp) (5) ✅ 5 |
| [CMenuSystem](../source/game_sa/MenuSystem.cpp) (21) ✅ 21 | [CTaskComplexUseScriptedAttractor](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedAttractor.cpp) (8) ✅ 8 | [VideoPlayer](../source/app/platform/win/VideoPlayer/VideoPlayer.cpp) (7) ✅ 7 |
| [CMessages](../source/game_sa/Messages.cpp) (28) ✅ 28 | [CTaskComplexUseScriptedBrain](../source/game_sa/Tasks/TaskTypes/TaskComplexUseScriptedBrain.cpp) (9) ✅ 9 | [WaterCreature_c](../source/game_sa/WaterCreature_c.cpp) (4) ✅ 4 |
| [CMirrors](../source/game_sa/Mirrors.cpp) (8) ✅ 8 | [CTaskComplexUseSequence](../source/game_sa/Tasks/TaskComplexUseSequence.cpp) (9) ✅ 9 | [WaterCreatureManager_c](../source/game_sa/WaterCreatureManager_c.cpp) (9) ✅ 7 |
| [CMissionCleanup](../source/game_sa/MissionCleanup.cpp) (4) ✅ 3 | [CTaskComplexUseSwatRope](../source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp) (13) ✅ 13 | [Win](../source/app/platform/win/WinMain.cpp) · [archivo 2](../source/app/platform/win/WinPlatform.cpp) · [archivo 3](../source/app/platform/win/WndProc.cpp) (8) ✅ 7 |
| [CMissionTextOffsets](../source/game_sa/Text/Text.cpp) (1) ✅ 1 | [CTaskComplexUseWaterCannon](../source/game_sa/Tasks/TaskTypes/TaskComplexUseWaterCannon.cpp) (8) ✅ 8 | [ZlibInflate](../source/game_sa/RenderWare/rw/zlibinflate.cpp) (14) |

</details>

<a id="parciales"></a>
## 8. 🟡 Parciales · 0 clases del catálogo + 2 clases/módulos con funciones del EXE pendientes

Orden de mayor a menor tamaño de las envolturas al EXE pendientes (suma de bytes de sus cuerpos; a igualdad, alfabético). «Función más grande» y «Tamaño» (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B) indican cuánto pesa la función más difícil de la clase. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).

Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas del juego al EXE», que cuenta solo código del juego; las de biblioteca no entran): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| *(ninguna clase parcial al 17/11/2026)* | | | | | | | | | |

### Clases con funciones del EXE aún sin reemplazar · 2 clases / módulos

Todas estas clases o módulos son parciales: conservan funciones del EXE en 0x401000–0x74FFFF sin reemplazar por C++ (grupos A–D del mapa del EXE y envolturas; 3 funciones). La clase se atribuye por cercanía de direcciones y puede ser errónea cuando su confianza es baja. Lista completa por función: [pendientes-por-clase.md](JuegoExe/pendientes-por-clase.md).

| Clase o módulo | Funciones pendientes | Bytes pendientes | Función más grande (B) | Tamaño |
| --- | ---: | ---: | ---: | --- |
| CAEUserRadioTrackManager | **1** | 85 | 85 | pequeña |
| CPathFind | **2** | 111 | 81 | pequeña |
| **TOTAL (2 clases / módulos)** | **3** | **196** | 85 | |

<a id="sin-revertir"></a>
## 9. 🔴 Sin revertir · 1 clases del juego y bibliotecas fuera del objetivo (RenderWare, D3D9, CRT, QuickTime)

Orden de mayor a menor tamaño de las envolturas pendientes. Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.

| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |
| (helpers y globales sin clase) | 0/0 | — | **1** | 85 | 85 | pequeña | 0.00% | 0 | 0 |

<a id="no-en-cpp"></a>

### Funciones del juego que ya tenemos y aún no están en C++: 1

Al 17/11/2026, estas 1 funciones **del código del juego** (0x401000–0x74FFFF) tienen envoltura abierta: la DLL ya tiene la función y su hook, pero el cuerpo todavía llama al EXE original por dirección (85 bytes en total). Mientras no estén reconstruidas y validadas **son envolturas del juego**, aunque el catálogo las trate como funciones sin dirección. La clasificación sale de la dirección del destino (juego = 0x401000–0x74FFFF, biblioteca = fuera de ese rango, tomada de la llamada `plugin::Call<0x…>` del código), no de una lista a mano: cuando se validen dejarán de aparecer solas. La tabla va **de mayor a menor tamaño**: «Bytes» es el cuerpo de la función en el EXE (análisis estático) y «Tamaño» lo clasifica (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B). Las funciones del EXE sin nombre ni hook (el resto de lo que falta) están en el [mapa del código del juego](JuegoExe/README.md). Fuente: [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv); lotes: [lotes-en-curso.csv](JuegoExe/lotes-en-curso.csv).

| Función | Dirección | Bytes | Tamaño | Clase o grupo | Lote en curso |
| --- | --- | ---: | --- | --- | --- |
| `StringCopyGrow` | `0x004F39C0` | 85 | pequeña | (helpers y globales sin clase) | — |


Además de las clases del juego, aquí quedan las **bibliotecas enlazadas dentro del EXE** (RenderWare, controlador D3D9, CRT y QuickTime), que no se reconstruyen: se sustituyen por bibliotecas equivalentes.

<a id="bibliotecas"></a>

### Bibliotecas, fuera del objetivo (13 envolturas)

Estas envolturas llaman al EXE, pero su destino es una biblioteca enlazada dentro del ejecutable (RenderWare, controlador D3D9, CRT, QuickTime) y **no cuentan** como trabajo pendiente del juego ni en las cifras de este informe. La clase sale de la dirección del destino (fuera de 0x401000–0x74FFFF) o de la clasificación del cohorte; no se reconstruyen: se sustituyen según la [hoja de ruta](RutaExeIndependiente.md).

| Biblioteca | Envolturas | De ellas, antes en «helpers sin clase» | Plan |
| --- | ---: | ---: | --- |
| RenderWare SDK | 1 | 0 | Sustituir por **librw** (o una implementación equivalente de RenderWare 3.7) y reconstruir los pipelines propios de SA; no se reescribe desde el EXE. |
| CRT / runtime estático | 11 | 4 | Usar la **CRT del compilador** (malloc/free, operator new/delete, funciones matemáticas); fuera de la reconstrucción del juego. |
| QuickTime/QTML | 1 | 0 | Reemplazar o desactivar el reproductor de vídeo (QuickTime/QTML) en el ejecutable propio; fuera de la reconstrucción del juego. |

<details>
<summary>Envolturas de biblioteca detectadas en el código fuente y reclasificadas por su dirección (4)</summary>

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
| Carga retardada de DLL | 5 | 0 | 0 | 0 | 0 | 5 / 0 / 0 |
| Controlador D3D9 (RwD3D9) | 118 | 115 | 0 | 0 | 0 | 118 / 0 / 0 |
| D3DX9 (compilador de shaders) | 950 | 11 | 0 | 0 | 0 | 21 / 120 / 809 |
| QuickTime/QTML (puentes) | 32 | 0 | 0 | 0 | 0 | 32 / 0 / 0 |
| RpHAnim | 7 | 7 | 0 | 0 | 0 | 7 / 0 / 0 |
| RpMatFX | 58 | 58 | 0 | 0 | 0 | 28 / 0 / 30 |
| RpSkin | 37 | 34 | 0 | 0 | 0 | 37 / 0 / 0 |
| RpUVAnim | 16 | 16 | 0 | 0 | 0 | 16 / 0 / 0 |
| RpWorld | 105 | 105 | 0 | 0 | 0 | 105 / 0 / 0 |
| RtAnim | 10 | 9 | 0 | 0 | 0 | 10 / 0 / 0 |
| RtBMP | 2 | 2 | 0 | 0 | 0 | 2 / 0 / 0 |
| RtDict | 12 | 12 | 0 | 0 | 0 | 12 / 0 / 0 |
| RtQuat | 6 | 6 | 0 | 0 | 0 | 6 / 0 / 0 |
| RtSlerp | 1 | 1 | 0 | 0 | 0 | 1 / 0 / 0 |
| RwCore | 202 | 202 | 0 | 0 | 0 | 202 / 0 / 0 |
| RwPlCore | 132 | 132 | 0 | 0 | 0 | 132 / 0 / 0 |
| libpng + zlib | 196 | 14 | 0 | 0 | 0 | 16 / 28 / 152 |


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
