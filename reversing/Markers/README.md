<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Marcadores 3D: C3dMarker y C3dMarkers

Validadas el 02/10/2026 con **17.056 pares original/candidato dentro de GTA**, todos exactos: las 6 funciones de `C3dMarker` y las 20 de `C3dMarkers` (26) con `validated=1`. Primera tanda: 5 funciones de `C3dMarker` (`AddMarker`, `DeleteMarkerObject`, `IsZCoordinateUpToDate`, `SetZCoordinateIfNotUpToDate`, `UpdateZCoordinate`) y 15 de `C3dMarkers` (flechas de dirección, marcadores de usuario, `Load/SaveUser3dMarkers`, `ForceRender`, `Update`, `User3dMarkerAtomicCB`, `PlaceMarker`, `PlaceMarkerSet`, `PlaceMarkerCone`) con `validated=1`.

**Diferencias reales corregidas:**
- `C3dMarker::UpdateZCoordinate`: la Z es `suelo - zDistance * 0.05f` (el original multiplica, no divide entre 20).
- `PlaceMarkerSet`: el alfa es `alpha * (1/3)` en float, truncado. `PlaceMarkerCone` compara la distancia con raíz (`sqrt(z² + x² + y²) >= 1,6`), no su cuadrado.
- `User3dMarkerSet`: pide el color HUD tres veces, una por componente.
- `PlaceMarker`:
  - el desplazamiento Z de los conos es `fsin(ángulo·π/180)·k` con el `fsin` x87, en las dos ramas; la rama nueva tenía el factor 0,3 dentro del seno;
  - `m_StdSize` y el alfa usan la aritmética exacta del original; su constante 0x8717A8 es el float siguiente a 0,015, no `0.015f`;
  - en un marcador ya existente, TORUS y TUBE sin `zCheck` fijan la Z sin comprobar;
  - los cilindros llaman a `UpdateZCoordinate` con el **centro del jugador** (como el original), no con la posición del marcador.

- Dibujado (segunda tanda):
  - `C3dMarker::Render` escala primero por el tamaño y, en el tubo, después por (1, 1, 20). En TORUS y ARROW2 multiplica siempre por la matriz "con eje arriba", normalizando derecha y frente (unidad si la normal es (0,0,1)); `CMatrix::WithUp` no normaliza.
  - `tDirectionArrow::Render` usa esa misma matriz.
  - `C3dMarkers::Render` mira distancia y banderas antes de `IsSphereVisible`.
  - `Shutdown` anula las entradas de clump repetidas para no destruir el mismo clump dos veces (ARROW, CONE y CONE_NO_COLLISION lo comparten).
  - `Init` solo inicializa los campos que toca el original, con la normal en (0, 0, 1), y `LoadMarker` sigue su orden de llamadas.
- **`CMatrix operator*` (afecta a todo el juego):** se reescribió con la agrupación exacta de sumas de 0x59BE30; el EXE suma cada elemento en un orden distinto.

**Método:**
- Se aleatorizan por caso los arrays globales (32 marcadores, 5 flechas, 5 marcadores de usuario), los clumps y un marcador propio.
- Se simulan y se trazan: RenderWare (clonar/crear/destruir atomic y frame, render callback, `RwFrameUpdateObjects`, `RpClumpForAllAtomics`), la línea de visión, el agua, el suelo, la colisión cargada, el centro del jugador y el búfer de guardado.
- Todos los marcadores con tipo o ID tienen atomic, como en el juego. El array se compara por hash, con volcado de los marcadores modificados.

El fallo del primer intento con `User3dMarkersDraw` era del fixture (clump nulo, imposible en el juego). En la segunda tanda se simulan además estados de render, luces, cámara (`IsSphereVisible`), carga de modelos, TXD y `RpClumpCreate/Destroy/AddAtomic`.

Repetir: `python reversing/Markers/build_probe.py` y `python reversing/run.py --case markers`. Evidencia: [validation-summary.json](validation-summary.json).
