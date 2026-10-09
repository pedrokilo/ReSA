<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CIplStore (subconjunto)

Validadas el 02/10/2026 con **6.240 pares original/candidato dentro de GTA**, todos exactos: 26 de las 34 funciones de `CIplStore` con `validated=1`.

Las 26 funciones son: `AddIplsNeededAtPosn`, `ClearIplsNeededAtPosn`, `EnableDynamicStreaming`, `FindIplSlot`, `GetBoundingBox`, `GetIplEntityIndexArray`, `GetIplName`, `GetNewIplEntityIndexArray`, `HaveIplsLoaded`, `IncludeEntity`, `SetIsInterior`, `RemoveIplAndIgnore`, `RemoveIplWhenFarAway`, `RequestIplAndIgnore`, `RequestIpls`, `RemoveRelatedIpls`, los tres callbacks `SetIf*IplIsRequired*`, `SetIplsRequired`, `EnsureIplsAreInMemory`, `RemoveAllIpls`, `LoadAllRemainingIpls`, `Save`, `Load` y `AddIplSlot`.

**Diferencias reales corregidas:**
- `IncludeEntity`: el último edificio es `max(lastBuilding, índice)`; la DLL usaba `firstBuilding`.
- `HaveIplsLoaded` y `RequestIpls`: el original llama primero a `SetIplsRequired(posn, jugador)`.
- `EnsureIplsAreInMemory`: `loadRequested` se borra siempre, cargue o no el IPL.

**Método:**
- Se usa el pool real de IPL, con banderas aleatorias por caso. Tras cada ejecución se restauran la cabecera, el almacenamiento y el mapa de bytes.
- `SetIplsRequired` recorre el quadtree real en los dos lados.
- Se simulan y se trazan: `CStreaming::RequestModel/RemoveModel/LoadAllRequestedModels`, `MessageScreen`, `CTimer::Suspend/Resume`, `CColAccel::isCacheLoading`, el búfer de guardado y `operator new[]`.
- En `AddIplSlot` se ignoran los bytes basura del `IplDef` nuevo (resto del nombre tras el NUL y relleno final): el pool de la DLL rellena con 0xCD y `strcpy_s` con 0xFE.

Las 8 restantes (`Initialise`, `Shutdown`, `LoadIpl`, `LoadIplBoundingBox`, `LoadIpls`, `RemoveIpl`, `RemoveIplSlot`, `SetupRelatedIpls`) se validan en [IplStore2](../IplStore2/README.md).

Repetir: `python reversing/IplStore/build_probe.py` y `python reversing/run.py --case ipl`. Evidencia: [validation-summary.json](validation-summary.json).
