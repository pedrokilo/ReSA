<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CWorld, grupo A: búsquedas de entidades por sector (15 funciones)

Validadas el 07/10/2026 con **8.704 pares original/candidato dentro de GTA**, todos exactos: 15 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CWorld` | `FindObjectsInRange`, `FindObjectsOfTypeInRange`, `FindLodOfTypeInRange`, `FindObjectsKindaColliding`, `FindObjectsIntersectingCube`, `FindObjectsIntersectingAngledCollisionBox`, `FindMissionEntitiesIntersectingCube`, `FindNearestObjectOfType` |
| `CWorld` (listas de sector `<CPtrListDoubleLink<CPhysical*>>`) | `FindObjectsInRangeSectorList`, `FindObjectsOfTypeInRangeSectorList`, `FindObjectsKindaCollidingSectorList`, `FindObjectsIntersectingCubeSectorList`, `FindObjectsIntersectingAngledCollisionBoxSectorList`, `FindMissionEntitiesIntersectingCubeSectorList`, `FindNearestObjectOfTypeSectorList` |

**Diferencias reales corregidas:**
- Sectores:
  - el índice es `floor(coord * 0,02f + 60,0f)` en float (multiplica por 0x858B38, no divide entre 50) y los bucles no tienen `assert` de rango vacío;
  - la lista LOD usa `ftol(coord * 0,005f + 15,0f)` (truncamiento, sin `floor`) y no limita el rango;
  - el sector repetido usa `& 0xF` sobre el entero con signo (el EXE no usa `abs() % 16`).
- Listas de sector (`...SectorList`):
  - `FindObjectsInRange`/`FindObjectsOfTypeInRange`: cuentan solo con `dist² < radio²` estricto (la DLL aceptaba el empate y NaN) y la suma 3D es `(dx² + dz²) + dy²`;
  - el índice de modelo se lee con signo (`movsx`);
  - `FindObjectsKindaColliding` no deja de recorrer al llenarse (el `return` NOTSA dejaba sin marcar el scan code), usa `dist < radio del modelo + radio` estricto con `sqrt((dz² + dy²) + dx²)` y el radio del `CColModel` del modelo;
  - `FindObjectsIntersectingCube` y `FindObjectsIntersectingAngledCollisionBox` usan el radio del modelo (no el `CColModel` especial del vehículo) y, la segunda, `Multiply3x3` (0x59C810) con el orden de sumas del EXE sobre `pos - punto`;
  - `FindMissionEntitiesIntersectingCube`:
    - la prueba es la misma que la del cubo con el radio del modelo (no un punto dentro de la caja);
    - se corrige `outEntities[*outCount++]`, que avanzaba el puntero y no el contador;
    - se quita el `break` NOTSA;
  - `FindNearestObjectOfType`: gana con `dist < mejor` estricto (no `<=`), con `sqrt((dx² + dz²) + dy²)` y el modelo con signo (`modelId < 0` acepta cualquiera).

**Método:**
- Mundo falso: las listas de sectores, de repetición y LOD se vacían y se rellenan con nodos de pools locales (simple y doble); las 24 entidades son bloques propios cubiertos por pools de entidades falsos (la DLL de depuración valida nodos e Items) y seis modelos falsos (19990..19995) con su modelo de colisión.
- Casos: posiciones cerca del origen, en todo el mapa y en los bordes (sectores recortados y repetidos con signo), entidades repetidas en varias listas, scan codes ya vigentes, vuelta del scan code (0xFFFF, con `ClearScanCodes` real sobre las listas falsas), `maxCount` 0/1/-1/100, `outEntities` nulo, radio NaN, empates de distancia y de bordes del cubo (±1 ulp), índices de modelo ≥ 0x8000 y las ocho combinaciones de la lista de misión.
- Se compara bit a bit: el contador, los punteros de salida, el bloque completo de entidades (scan codes), el scan code global, las listas de sectores/repetición/LOD y los pools de nodos.
- Todo el estado global tocado (punteros de pools, sectores, repetición, LOD, scan code y entradas de `ms_modelInfoPtrs`) se guarda y se restaura en cada ejecución y al final. En el baseline también se desactiva el hook de `CWorld::ClearScanCodes`.

Repetir: `python reversing/WldA/build_probe.py` y `python reversing/run.py --case wlda`. Evidencia: [validation-summary.json](validation-summary.json).
