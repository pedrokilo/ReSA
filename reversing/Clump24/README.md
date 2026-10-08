<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
