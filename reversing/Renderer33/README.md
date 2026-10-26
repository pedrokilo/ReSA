<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Renderizador (lista de 33)

Validadas el 04/10/2026 con **18.112 pares original/candidato dentro de GTA**, todos exactos: 33 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CRenderer` | `RenderFadingInEntities`, `RenderFadingInUnderwaterEntities`, `RenderOneRoad`, `RenderOneNonRoad`, `RemoveVehiclePedLights`, `AddEntityToRenderList`, `GetLodRenderListBase`, `GetLodDontRenderListBase`, `ResetLodRenderLists`, `AddToLodRenderList`, `AddToLodDontRenderList`, `ProcessLodRenderLists`, `PreRender`, `RenderRoads`, `RenderEverythingBarRoads`, `RenderFirstPersonVehicle`, `SetupLightingForEntity`, `SetupMapEntityVisibility`, `SetupEntityVisibility`, `SetupBigBuildingVisibility`, `SetupScanLists`, `ScanSectorList_ListModels`, `ScanSectorList_ListModelsVisible`, `ScanSectorList`, `ScanBigBuildingList`, `ShouldModelBeStreamed`, `ScanPtrList_RequestModels`, `ConstructRenderList`, `ScanSectorList_RequestModels`, `ScanWorld`, `GetObjectsInFrustum`, `RequestObjectsInFrustum`, `RequestObjectsInDirection` |

**Diferencias reales corregidas:**
- `ProcessLodRenderLists`:
  - el segundo bucle es un `do/while` (la DLL usaba `while (false)` y no lo ejecutaba nunca);
  - `CEntity::CanLodChildrenRender` compara el `int8` con `0x80` (con `!= 128` siempre era cierto).
- `SetupMapEntityVisibility`: distancia de fundido con `* 0,06666667` (`flt_863E0C`), no `/ 15`; mínimos y comparaciones con el resultado del EXE ante NaN.
- `SetupEntityVisibility`:
  - el bit que manda a la lista ordenada es `m_bDrawLast` (bit 14), no `m_bWasPostponed`;
  - distancia `√((z²+y²)+x²)`;
  - `FindPlayerVehicle` se vuelve a llamar para el tipo y los flags de la moto.
- `SetupBigBuildingVisibility` y `ShouldModelBeStreamed`: distancia `√((z²+y²)+x²)`; `ShouldModelBeStreamed` solo descarta si la distancia es estrictamente mayor que radio + plano lejano.
- `RenderEverythingBarRoads`:
  - los peatones con alpha ≠ 255 también se insertan en la lista ordenada (la DLL los dibujaba);
  - `GetLookDirection` solo se consulta en primera persona;
  - orden de sumas de la distancia distinto en cada lista (ordenada z, y, x; submarina z, x, y).
- `RenderFirstPersonVehicle` lee `CWorld::Players[0].m_pPed` (el EXE no llama a `FindPlayerPed`).
- `SetupLightingForEntity` pasa una copia local de la posición a `GenerateLightsAffectingObject`.
- `ScanSectorList` y `ScanBigBuildingList`:
  - ángulo con `fpatan` de precisión completa (asm), solo si el sector no está cerca;
  - la distancia no se reinicia por entidad;
  - en `ScanSectorList` la oclusión se consulta antes que `bRequestModel` si el modelo no está cargado, y el contador de invisibles se compara con signo.
- `ScanSectorList_ListModelsVisible` deja de recorrer listas al encontrar una nula (como el EXE); `SetupScanLists` indexa el sector repetido con `& 15`.
- `ScanWorld`, `GetObjectsInFrustum`, `RequestObjectsInFrustum`:
  - sectores con `x · 0,02 + 60` y `x · 0,005 + 15` (multiplicación);
  - puntos lejanos `(p · 300) · (1 / plano)` y cercanos `· 0,2`;
  - `ScanWorld` escribe el flag del EXE `0xB745C8` (distancia² < 16 y `forward·último > 0,98`, sumando z, x, y) y marca la guarda de sus estáticas (`0xB76894`);
  - `RequestObjectsInFrustum` usa los puntos de abajo también con `at.z` NaN.
- `ConstructRenderList` calcula siempre las dos alturas del suelo y limita la escala como el EXE.
- `RequestObjectsInDirection`: ángulo `· 57,295776` (`flt_859878`) `+ 180` y siempre `STREAMING_LOADING_SCENE`.

**Método:**
- Entidades, model infos (en 7 ids especiales y 10 huecos libres de `ms_modelInfoPtrs`), colisiones, clumps, handling, `RwCamera`, marco, matrices y mundo son falsos; las vtables de entidades y model infos están trazadas.
- Un bloque de 8×8 sectores, los 256 sectores repetidos y las 900 listas LOD se vacían y se rellenan con nodos de pools de nodos falsos; los punteros de los pools de entidades y de nodos se sustituyen por pools falsos que cubren las entidades y nodos falsos (la DLL valida cada nodo y su `Item`).
- Se guardan y restauran en cada ejecución: globales de `CRenderer`, `PC_Scratch`, `TheCamera`, `Scene`, listas alpha, escalas LOD, código de búsqueda, área, pools, sectores, model infos y su estado de streaming, `CWorld::Players[0].m_pPed`, el puntero `fpRenderStateSet` de `RwEngineInstance` y demás globales tocadas.
- Stubs trazados (EXE + cuerpo de la DLL; los cuerpos se resuelven antes de parchear): estados RW, `RwCameraBegin/EndUpdate`, `RwV3dTransformPoints`, `CWorldScan::ScanWorld`, luces, `CVisibilityPlugins`, `CPostEffects`, `CCarFXRenderer`, `CVehicle`, `CPhysical::GetLightingFromCol`, `CHeli`, `CShadows`, `CClock`, `GetIsOnScreen`/`IsEntityOccluded`, `CStreaming`, `ClearScanCodes`, `CCamera`, `CCullZones`, `CMirrors`, `COcclusion` y `FindPlayerPed/Vehicle`.
- Fuera de los fixtures:
  - el NOTSA de `PreRender` que acepta también `RenderEntity` de la DLL como callback;
  - la distancia inicial (basura de pila) de `ScanSectorList`/`ScanBigBuildingList`: las entidades que devuelven VISIBLE sin escribir distancia no llevan `m_bDistanceFade` (ni `m_bDrawLast` en LOD);
  - los flags y rellenos sin inicializar de la matriz local de `RequestObjectsInDirection`.

Repetir: `python reversing/Renderer33/build_probe.py` y `python reversing/run.py --case renderer33`. Evidencia: [validation-summary.json](validation-summary.json).
