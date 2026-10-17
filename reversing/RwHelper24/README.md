<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ayudantes de RenderWare (lista de 24)

Validadas el 03/10/2026 con **11.328 pares original/candidato dentro de GTA**, todos exactos: 24 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `RwHelper` | `GetEventGlobalGroup`, `GetNameAndDamage`, `GetFirstAtomicCallback`, `GetFirstAtomic`, `Get2DEffectAtomic`, `GetFirstObjectCallback`, `GetFirstObject`, `GetFirstFrameCallback`, `GetFirstChild`, `SkinAtomicGetHAnimHierarchCB`, `GetAnimHierarchyFromSkinClump`, `GetAnimHierarchyFromClump`, `AtomicRemoveAnimFromSkinCB`, `RpAtomicConvertGeometryToTL`, `RpAtomicConvertGeometryToTS`, `atomicConvertGeometryToTL`, `atomicConvertGeometryToTS`, `RpClumpConvertGeometryToTL`, `RpClumpConvertGeometryToTS`, `forceLinearFilteringMatTexturesCB`, `SetFilterModeOnAtomicsTextures`, `forceLinearFilteringAtomicsCB`, `SetFilterModeOnClumpsTextures`, `RemoveRefsCB` |

**Diferencias reales corregidas:**
- `GetNameAndDamage`: copia con `strncpy` (0x821F40) más terminador manual y `strcpy` como el EXE, en lugar de `strncpy_s`/`strcpy_s`.
- No había desviaciones `FIX_BUGS`/`NOTSA` en estas funciones; el resto coincide con el EXE.

**Método:**
- Mundo RW falso reconstruido en cada ejecución:
  - un clump con tipo 2 en +0, frame padre y lista de atomics de longitud y orden aleatorios;
  - 8 frames en árbol (hijo +0x98, hermano +0x9C), con lista de objetos (+0x90) y plugin HAnim;
  - 8 atomics con geometría, plugin de visibilidad (modelo) y plugin de skin (jerarquía);
  - 4 geometrías con flags (tristrip/nativa), lista de materiales (incluidas cuentas 0 y negativas), plugins de skin y 2dfx;
  - materiales, texturas, jerarquías con interpolador y animación actual.
- Los offsets de los plugins se leen del EXE (0x8D608C, 0xC978A4, 0xC978A8, 0xC3A1E0, 0xC9B8C8).
- Las iteraciones reales del EXE (`RpClumpForAllAtomics`, `RwFrameForAllObjects`/`Children`, `RpGeometryForAllMaterials`) corren sobre el mundo falso.
- Se simulan y trazan `RpGeometryLock`/`Unlock`, `RtAnimAnimationDestroy`, `CTxdStore::RemoveRef` (EXE y DLL) y `operator new` (sin reservar memoria, solo en `GetEventGlobalGroup`).
- Se guardan y restauran el puntero global de `CEventGlobalGroup` (0xA9AF6C) y 4 entradas de `CModelInfo::ms_modelInfoPtrs` usadas por model infos falsos.
- El vptr del grupo creado se compara como tipo (vtable del EXE o de la DLL).
- Fuera de los fixtures: nombres que acaban en `_dam` con menos de 4 caracteres (el EXE copia con longitud negativa), atomics sin model info (los dos se caen) y frames/clumps nulos.

Repetir: `python reversing/RwHelper24/build_probe.py` y `python reversing/run.py --case rwhelper24`. Evidencia: [validation-summary.json](validation-summary.json).
