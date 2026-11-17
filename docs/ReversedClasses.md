# Progreso del reversing

[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md) · [Registro de validaciones](../reversing/ValidationRegistry/README.md)

> **Fuente:** [hooks.csv](hooks.csv) y el análisis estático del EXE · **Generado:** 2026-11-17 14:16 UTC

**Índice:** [1. Dónde estamos](#donde-estamos) · [2. Código del juego](#juego) · [3. RenderWare](#renderware) · [4. Qué falta por validar](#por-validar) · [5. Clases del juego sin validar](#clases) · [6. Cómo leer y regenerar](#metodo)

<a id="donde-estamos"></a>

## 1. Dónde estamos

Tres conceptos distintos, que no se mezclan en ninguna cifra:

- **Reconstruida:** la función existe en C++ puro, tiene su hook en la DLL y compila. Es lo que mide el EXE (`hooks.csv`, `reversed=1`).
- **Validada:** además se comparó con el EXE dentro del juego (≥16.384 pares sin diferencias, oráculo independiente y prueba negativa de dos rondas) y su evidencia está en el registro (`validated=1`).
- **Reconstruida sin validar:** integrada y activa, pero sin banco todavía (`validated=0`). Compila, pero no se ha ejecutado en el juego contra el original.

Y dos zonas del EXE que se miden por separado: el **código del juego** (0x401000–0x74FFFF, el trabajo propio del proyecto) y **RenderWare y D3D** (0x750000–0x81FFFF, el SDK enlazado dentro del EXE).

Cifras al 17/11/2026.

| Zona | Funciones a reconstruir | Reconstruidas | Entradas del catálogo | Validadas | Sin validar |
| --- | ---: | ---: | ---: | ---: | ---: |
| **Código del juego** | 11.686 | 11.686 (100,00%) | 11.613 | 11.348 | 265 |
| **RenderWare y D3D** | 724 | 724 (100,00%) | 822 | 77 | 745 |

**Cómo leer la tabla:** «Funciones a reconstruir» y «Reconstruidas» se cuentan en funciones del EXE; «Validadas» y «Sin validar» se cuentan en entradas del catálogo (`hooks.csv`), y suman las «Entradas del catálogo». No coinciden con las funciones porque una función puede tener varias entradas (cuerpo, destructor con borrado, ranura virtual, envoltura). En el código del juego se descartan 118 funciones excluidas (no reconstruibles en C++ puro); en RenderWare, 1.111 funciones de libpng, zlib y D3DX9 (se enlaza la original) y 54 no reconstruibles.

En total hay **12.441 entradas reconstruidas, 11.431 validadas y 1.010 sin validar** en el catálogo (11.431 con evidencia registrada). Quedan además 6 entradas fuera de ambas zonas (CRT y utilidades).

### Fase del proceso

| Fase | Estado |
| --- | --- |
| Reconstruir el código del juego en C++ puro | **Hecha**: 100,00% de las funciones y 100,00% de los bytes; faltan 0 y hay 118 excluidas por no ser reconstruibles |
| Reconstruir el SDK de RenderWare | **Hecha**: 724 de 724 funciones reconstruibles (100,00%); aparte, 1.111 son libpng, zlib y D3DX9 (se enlaza la original) y 54 no son reconstruibles |
| **Validar el código del juego** | **En curso**: 11.348 de 11.613 reconstruidas (97,72%); el resto va por oleadas de lotes (detalle en el [informe](ReversedClasses.md#por-validar)) |
| Validar RenderWare | **Apenas empezada**: 77 de 822 (9,37%); falta el plan de bancos (dispositivo D3D falso, flujos y listas libres falsos) |
| Pase final de bancos viejos | Pendiente: repetir los bancos antiguos cuyos colaboradores ahora tienen hook o cuyo código cambió |
| Ejecutable propio | Después: sustituir RenderWare, D3D y CRT por bibliotecas y quitar la dependencia del EXE ([hoja de ruta](ProjectRoadmap.md)) |

<a id="juego"></a>

## 2. Código del juego (0x401000–0x74FFFF)

| Medida | Valor | Progreso |
| --- | --- | --- |
| Funciones reconstruidas | 11.686 / 11.686 · 100,00% | `████████████████████` |
| Bytes de código reconstruidos | 2.979.290 / 2.979.290 · 100,00% | `████████████████████` |
| Entradas validadas (de las reconstruidas) | 11.348 / 11.613 · 97,72% | `████████████████████` |
| Entradas reconstruidas sin validar | 265 |  |

**Faltan 0 funciones (0 bytes)**, listadas en [pendientes-por-clase.md](JuegoExe/pendientes-por-clase.md), y **118 excluidas** y documentadas en [excluidas.csv](JuegoExe/excluidas.csv) porque no se pueden reconstruir en C++ puro: 73 __usercall con registros que C++ no puede leer; 31 saltos puros a otra dirección; 14 fragmentos internos de otra función y otros.

Todo lo reconstruido está en C++: en `source/game_sa` no queda ningún `__asm` ni función `naked` ([estado de la migración](../reversing/CppOnlyMigration/README.md)). Cada validada tiene su banco `FnNNNA` y su evidencia en `reversing/CppTwelveLevel6Final/runs`.

<a id="cobertura-exe"></a>

Detalle del mapa del EXE (estados A–D por función): [JuegoExe/README.md](JuegoExe/README.md). El mapa de RenderWare está en [RenderWareExe/README.md](RenderWareExe/README.md).

<a id="renderware"></a>

## 3. RenderWare y D3D (0x750000–0x81FFFF)

Región de 1.889 funciones y 600.971 bytes: el SDK de RenderWare 3.7, el controlador D3D9, D3DX9 (compilador de shaders), libpng, zlib y libjpeg, enlazados dentro del EXE.

| Medida | Funciones |
| --- | ---: |
| Funciones que se reconstruyen (sin las enlazadas ni las no reconstruibles) | 724 |
| **Reconstruidas en C++** (hook en la DLL) | **724** (100,00%) |
| Entradas del catálogo de esas funciones (una función puede tener varias) | 822 |
| Entradas validadas | 77 |
| Entradas sin validar | 745 |
| Enlazadas (libpng, zlib y D3DX9: se enlaza la original, no se reconstruyen) | 1.111 |
| No reconstruibles en C++ puro (puentes de QuickTime, carga retardada, 3DNow!, ya enganchadas por el modo ventana) | 54 |
| **Pendientes de reconstruir** | **0** |

**Por módulo** (clase de hook; lo reconstruido y lo validado):

| Módulo | Reconstruidas | Validadas | Sin validar |
| --- | ---: | ---: | ---: |
| RwCore | 211 | 29 | 182 |
| RpWorld | 197 | 25 | 172 |
| RwPlCore | 132 | 14 | 118 |
| RwD3D9 | 115 | 0 | 115 |
| RpSkin | 34 | 1 | 33 |
| RpMatFX | 28 | 0 | 28 |
| ImportThunks | 17 | 0 | 17 |
| RpUVAnim | 16 | 2 | 14 |
| ZlibInflate | 14 | 0 | 14 |
| D3DX9 | 11 | 0 | 11 |
| RtDict | 12 | 2 | 10 |
| RtAnim | 10 | 3 | 7 |
| QTMLRuntime | 6 | 0 | 6 |
| RpHAnim | 7 | 1 | 6 |
| RtQuat | 6 | 0 | 6 |
| DelayLoadThunks | 3 | 0 | 3 |
| RtBMP | 2 | 0 | 2 |
| RtSlerp | 1 | 0 | 1 |

Nada de RenderWare se ha ejecutado todavía contra el original: el [plan de bancos](../reversing/PendienteValidar/PLAN_VALIDACION.md) de esta zona es la siguiente gran tarea de validación. Funciones por módulo y su estado: [RenderWareExe/pendientes-por-clase.md](RenderWareExe/pendientes-por-clase.md).

<a id="por-validar"></a>

## 4. Qué falta por validar (código del juego)

Los lotes de validación van por oleadas y cada uno tiene su informe en [reversing/PendienteValidar](../reversing/PendienteValidar). Un lote está validado cuando su informe desaparece de esa carpeta.

| Oleada | Lotes | Lotes totales | Validados | Por validar | Funciones por validar |
| --- | --- | ---: | ---: | ---: | ---: |
| V1 | 189A–206A | 18 | 18 | 0 | 0 |
| V2 | 207A–215A | 9 | 9 | 0 | 0 |
| V3 | 216A–222A | 7 | 3 | 4 | 100 |
| V4 | 223A–229A | 7 | 0 | 7 | 121 |
| V5 | 230A–236A | 7 | 0 | 7 | 136 |
| V6 | 237A–239A | 3 | 0 | 3 | 5 |

En validación ahora: 219A, 220A.

Lotes por validar: 221A, 222A, 223A, 224A, 225A, 226A, 227A, 228A, 229A, 230A, 231A, 232A, 233A, 234A, 235A, 236A, 237A, 238A, 239A.

Los lotes de RenderWare (R1A–R34A) se validarán en una oleada propia, aún sin plan.

<a id="clases"></a>

## 5. Clases del juego con funciones sin validar

Solo se listan las clases del código del juego que todavía tienen alguna función reconstruida sin validar (las 40 con más). El resto de las clases del juego está completa y validada. Lista completa: [no-validadas-por-clase.csv](JuegoExe/no-validadas-por-clase.csv).

| Clase | Reconstruidas | Validadas | Sin validar |
| --- | ---: | ---: | ---: |
| CPedAttractor | 85 | 58 | 27 |
| LibJpegDecompress | 22 | 0 | 22 |
| LibJpegMemory | 21 | 0 | 21 |
| LibJpegCompress | 18 | 0 | 18 |
| ExeLibJpeg | 15 | 0 | 15 |
| RwD3D9RasterDriver | 14 | 0 | 14 |
| LibJpegCompressEncode | 11 | 0 | 11 |
| D3D9RasterFormat | 10 | 0 | 10 |
| CTheScripts | 82 | 73 | 9 |
| ExeProtectionStub | 8 | 0 | 8 |
| FxEmitter_c | 9 | 4 | 5 |
| LegacyRuntimeError | 13 | 8 | 5 |
| TList_c | 8 | 3 | 5 |
| CPedShelterAttractor | 11 | 7 | 4 |
| CRunningScript | 43 | 39 | 4 |
| FxEmitterBP_c | 13 | 9 | 4 |
| LibJpegDecompressApi | 4 | 0 | 4 |
| CEntity | 76 | 73 | 3 |
| CPedATMAttractor | 3 | 0 | 3 |
| CPedAttractorManager | 25 | 22 | 3 |
| CPedLookAtAttractor | 3 | 0 | 3 |
| CPedParkAttractor | 3 | 0 | 3 |
| CPedPizzaAttractor | 3 | 0 | 3 |
| CPedScriptedAttractor | 3 | 0 | 3 |
| CPedSeatAttractor | 3 | 0 | 3 |
| CPedStepAttractor | 3 | 0 | 3 |
| CPedStopAttractor | 3 | 0 | 3 |
| CPedTriggerScriptAttractor | 3 | 0 | 3 |
| CProtectionStub | 3 | 0 | 3 |
| LibJpegCompressApi | 3 | 0 | 3 |
| LibJpegDecompressUpsample | 3 | 0 | 3 |
| LibJpegError | 3 | 0 | 3 |
| CAEAudioEntity | 8 | 6 | 2 |
| CCamera | 126 | 124 | 2 |
| CDecisionMaker | 5 | 3 | 2 |
| CPtrNodeDoubleLinkPool | 7 | 5 | 2 |
| CPtrNodeSingleLinkPool | 7 | 5 | 2 |
| CTaskComplexSeekEntityRadiusAngleOffset | 11 | 9 | 2 |
| IKChainManager_c | 19 | 17 | 2 |
| WaterCreatureManager_c | 9 | 7 | 2 |

Otras 18 clases con 18 funciones sin validar entre todas (cada una con 1 o menos).

Clases del juego completas y validadas: **921** de 979.

<a id="metodo"></a>

## 6. Cómo leer y regenerar

- **Reconstruida** = `reversed=1` en [hooks.csv](hooks.csv); **validada** = `validated=1`, con evidencia aprobada vinculada en el registro. Validada no implica vigente para el árbol actual: la vigencia se consulta en el [inventario](ValidationInventory.md).
- **Zona** = dirección de la función en el EXE; las entradas virtuales (dirección 0) heredan la zona de su clase.
- Los porcentajes de este informe dicen siempre de qué miden (funciones, bytes o entradas del catálogo).

Regenerar (sincroniza primero `validated` desde el registro):

```powershell
python contrib/sync_validated_flags.py
python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md
```

El formato vive en [contrib/progress_report.py](../contrib/progress_report.py). La historia de entregas anteriores está en [Historial.md](archivo/Historial.md).
