<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
