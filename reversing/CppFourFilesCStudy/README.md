<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# common, WaterCreature_c, TaskComplexDestroyCarArmed y TimeCycle sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el ensamblador de `common.cpp`, `WaterCreature_c.cpp`, `TaskComplexDestroyCarArmed.cpp` y `TimeCycle.cpp` (8 apariciones de `__asm` y 2 `naked`, 7 funciones fuente). Son los cuatro archivos que quedaban del stash `lote-28-archivos-sin-terminar`. Un subagente por archivo preparó el análisis (desensamblado del EXE y parche); la compilación y los bancos corrieron en serie. Ningún banco se relajó y los cuatro aprobaron a la primera.

## Conversiones

| Función | Cómo |
| --- | --- |
| `Common::TransformPoint` (0x54ECE0) | `CommonTransformPointCpp` (nuevo) |
| `Common::TransformVectors` (0x54EE30) | `TrigSimpleCosCpp` / `TrigSimpleSinCpp` y `CommonTransformVectorsCpp` (nuevo, x87; banco propio CppTransformVectors1) |
| `WaterCreature_c::Init` / `Update` (0x6E4160 / 0x6E4670) | `TrigAtan2Cpp` y dos ayudantes locales `precise` para adelante·velocidad |
| `CTaskComplexDestroyCarArmed::CalculateSearchPositionAndRanges` (0x628C80) | `DestroyCarDistanceCpp` (nuevo) |
| `CTimeCycle::Initialise` (0x5BBAC0) | `TimeCycleDirnToSunCpp` (nuevo) |
| `CTimeCycle::CalcColoursForPoint` (0x5603D0) | `TimeCycleVectorToSunCpp` (nuevo) |

Los dos `naked` de `TimeCycle.cpp` eran los envoltorios de los dos bloques `__asm` (fragmentos en línea de hooks ya escritos en C++), sin registros de entrada ni de salida que conservar, así que no hizo falta `ResidueRegsCpp` ni `EntryEaxCpp`.

Correcciones del análisis respecto al stash:
- `WaterCreature_c`: el stash redondeaba el seno a float antes de multiplicar por la velocidad; el EXE lo deja sin redondear en un sitio y redondea solo el coseno en el otro. Se corrigió.
- `common.cpp` (`TransformVectors`): el EXE escribe `out.x` y después relee `in.x`; con entrada igual a salida el resultado cambia. Se corrigió el orden de lectura y el contador del bucle.

## Bancos (cero diferencias)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| DependenciesMathSprite2 | 3072 | TransformPoint (1024) |
| World52 | 18384 | WaterCreature_c Init (1024) y Update (1024) |
| CTasks10a | 15488 | CalculateSearchPositionAndRanges (256) |
| Time20 | 7488 | Initialise (64) y CalcColoursForPoint (1024) |
| CppTransformVectors1 | 3072 | TransformVectors (3072), banco propio: EXE 0x54EE30 contra la sobrecarga de la DLL llamada por su símbolo, sin hook; incluye NaN en x/y |
| CEntA | 16384 | cobertura indirecta adicional (la ejecuta con su cuerpo real) |

## Límites

- `Common::TransformVectors` no está hookeada (no figura en el catálogo), así que tiene un banco propio, `reversing/CppTransformVectors1`, que llama al EXE por su dirección y a la DLL por su símbolo (3072 pares). Cubre salida distinta, en sitio y solapada, z con sNaN/qNaN/Inf/-0 y NaN, sNaN e Inf en x e y. Al añadir los NaN de x/y apareció una diferencia real (36/3072): el SSE de la DLL propaga la carga útil y el signo de los NaN distinto de la x87 del EXE; se corrigió moviendo el bucle a `CommonTransformVectorsCpp` (X87Math, x87) y el banco aprobó. Control negativo hecho: reintroduciendo el defecto antiguo (leer `in.x`/`in.y` antes de escribir) el banco falla 635/3072, todos en modo en sitio (`reversing/CppTransformVectorsNegativeControl`). Único hueco: `numVectors` negativo no se prueba (el EXE recorrería 2^32 iteraciones y se colgaría).
- Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos (denormales, desbordes). Los colaboradores no se validan por probar a sus consumidores. No se revisó el código máquina de los ayudantes nuevos.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| TaskComplexDestroyCarArmed.actual-after.obj | 105c9ea11def8a8ffe2333d4697fce0c45b78b62a12432534339eb677a1f39a2 |
| TaskComplexDestroyCarArmed.actual-before.obj | e5bbac755941325a00cec2e8156333769ad3cf9223a98c7faff010014f49d144 |
| TimeCycle.actual-after.obj | 4b68113831ee05b9a2c6221fc72627f1e06a825c52c61135842cd6a8ea78dcd6 |
| TimeCycle.actual-before.obj | 041f8252eda4f06037a69a5b51a1b8f650d56e23179f4de173f13a585783f388 |
| WaterCreature_c.actual-after.obj | 0fd8bd2e79d1d5fb169cda0ac3b8f834b18b09db5cfbfc6f7b5511b460e3c38d |
| WaterCreature_c.actual-before.obj | 8a873dd75972cb1dfecedbbb6352ceb0598c42e4ba8d7e3fd5d3b2816574f255 |
| X87Math.actual-after.obj | 490fe086c934a416b741cc92c1e5379dfcc98e220ceacebfd2773c4afa055ee0 |
| X87Math.actual-before.obj | b1ace32161e650230d647f0a20abc18cdda3a4d1fb082c7be2e562a646d8ce30 |
| common.actual-after.obj | 068df47bf8f7c9b02417357d79e70ada16834e43acadfaa46c3005180b0e7eec |
| common.actual-before.obj | 9301d3d3d0a16d18a12420e4160753efd8dd785393b135ff991aca59737bb6c3 |
