<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
