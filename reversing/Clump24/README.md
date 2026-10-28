<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Modelos de clump (lista de 24)

Validadas el 03/10/2026 con **12.256 pares original/candidato dentro de GTA**, todos exactos: 24 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CClumpModelInfo` | `GetModelType`, `Init`, `Shutdown`, `DeleteRwObject`, `GetRwModelType`, `CreateInstance` (con matriz y sin argumentos), `SetAnimFile`, `ConvertAnimFileIndex`, `GetAnimFileIndex`, `GetBoundingBox`, `SetClump` (VMT, slots 4–16 de 0x85BD30), `SetFrameIds`, `SetHierarchyForSkinAtomic`, `AtomicSetupLightingCB`, `SetAtomicRendererCB`, `FindFrameFromNameCB`, `FindFrameFromNameWithoutIdCB`, `FindFrameFromIdCB`, `FillFrameArrayCB`, `GetFrameFromId`, `GetFrameFromName`, `FillFrameArray`, `SetClumpModelInfoFlags` |

**Diferencias reales corregidas:**
- `CreateInstance(RwMatrix*)` copia la matriz en `frame->modelling` (frame + 0x10); antes pisaba la cabecera del frame.
- `SetClump`:
  - vuelve a pedir `GetFirstAtomic` antes de escalar la esfera y antes de normalizar los pesos (3 llamadas, como el EXE);
  - llama dos veces a `GetAnimFileIndex` por la vtable.
- `DeleteRwObject` llama dos veces a `GetAnimFileIndex` por la vtable.
- `SetAnimFile`/`ConvertAnimFileIndex` usan `operator new`/`delete` escalares (0x82119A/0x8214BD) y `strcpy`, no `new[]`/`delete[]`.
- `SetAnimFile`, `FindFrameFromNameCB` y `FindFrameFromNameWithoutIdCB` comparan con `_stricmp` (0x8229B6), no `strcmp`: `"NULL"` también se ignora y los nombres de frame no distinguen mayúsculas (afecta también a `GetFrameFromName` y `SetFrameIds`).
- `CreateInstance()` toma el bloque como `0xB5D4A0 + índice·32` sin comprobar rango (antes una vista `take` que aborta en Debug).

**Método:**
- Model info falso fuera de `ms_modelInfoPtrs` (ninguna función busca por índice) con una copia de la vtable del EXE; `GetAnimFileIndex` y `CreateInstance()` trazados.
- Árboles de hasta 24 frames falsos: `RwFrameForAllChildren`, nombres de nodo e IDs de jerarquía corren de verdad con los offsets de plugin del EXE.
- Primitivas RW/RpSkin/RpAnimBlend, `CAnimManager`, `CVisibilityPlugins`, referencias de TXD/modelo/colisión y pipelines simuladas con trazas (dirección del EXE y símbolo de la DLL).
- `operator new`/`delete` del EXE a una arena fija; se distingue la versión escalar de la de arrays.
- Fuera de los fixtures: sumas de pesos subnormales (el x87 conserva el exponente extendido del recíproco) y clumps nulos devueltos por `CreateInstance()`.

Repetir: `python reversing/Clump24/build_probe.py` y `python reversing/run.py --case clump24`. Evidencia: [validation-summary.json](validation-summary.json).
