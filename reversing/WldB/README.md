<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7809 con evidencia histórica, 0 con vigencia acreditada y 637 pendientes en 9 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CWorld: líneas de visión y líneas verticales (grupo B)

Banco preparado el 07/10/2026: **9.216 pares original/candidato dentro de GTA** para 16 funciones de `CWorld` con `validated=0` (pendiente de compilar y ejecutar; hasta entonces ninguna pasa a `validated=1`).

| Clase | Funciones |
|---|---|
| `CWorld` (altura del suelo) | `FindGroundZForCoord`, `FindGroundZFor3DCoord`, `FindLowestZForCoord`, `FindRoofZFor3DCoord` |
| `CWorld` (línea de visión) | `GetIsLineOfSightClear`, `GetIsLineOfSightSectorClear`, `GetIsLineOfSightSectorListClear<CPtrListDoubleLink<CPhysical*>>` |
| `CWorld` (línea con punto de choque) | `ProcessLineOfSight`, `ProcessLineOfSightSector`, `ProcessLineOfSightSectorList<CPtrListDoubleLink<CPhysical*>>` |
| `CWorld` (línea vertical) | `ProcessVerticalLine`, `ProcessVerticalLineSector`, `ProcessVerticalLineSectorList<CPtrListDoubleLink<CPhysical*>>` |
| `CWorld` (línea vertical, puntos del globo) | `ProcessVerticalLine_FillGlobeColPoints`, `ProcessVerticalLineSector_FillGlobeColPoints`, `ProcessVerticalLineSectorList_FillGlobeColPoints<CPtrListDoubleLink<CPhysical*>>` |

**Diferencias reales corregidas (comparadas con el EXE 1.0 US, sin `FIX_BUGS`):**
- Sectores (`ProcessVerticalLine`, `ProcessVerticalLine_FillGlobeColPoints`, `ProcessLineOfSight`, `GetIsLineOfSightClear`):
  - el sector es `ftol(floor(coord · 0,02 + 60))` con x87 a 24 bits (la DLL dividía entre 50);
  - el sector repetido usa `& 0xF` sobre el entero con signo (la DLL usaba `abs() % 16`: un sector negativo caía en otra lista);
  - el sector grande se acota a [0, 119] como `GetSector`.
- `GetIsLineOfSightClear` y `ProcessLineOfSight`:
  - con la misma X (o la misma Y) el EXE recorre SIEMPRE de origen a destino, ascendente o descendente (la DLL, con el destino por encima, recorría del destino al origen);
  - el cruce de cada borde de sector usa la pendiente `(Δy / Δx)` en float y `((k · 50 − origen.x) · pendiente + origen.y)` con `k = x − 59` (izquierda a derecha) o `x − 60` (derecha a izquierda), sin pasar por `GetSectorPosX`;
  - la rama es `destino.x > origen.x` negada (`fcomp` + `test ah,41h`: NaN va a la rama derecha-izquierda);
  - `ProcessLineOfSight` no ponía `ms_iProcessLineNumCrossings` a 0 al entrar (el EXE sí). Se quita además el `assert` de NaN/inf, que el EXE no tiene.
- `ProcessVerticalLineSectorList_FillGlobeColPoints`:
  - tras un punto aceptado el EXE vuelve a procesar LA MISMA entidad con la línea desplazada (aunque sea el último nodo); la DLL pasaba a la siguiente entidad y perdía el último nodo;
  - el desplazamiento del inicio al punto y `±0,1` en z se aplica siempre que la prueba tiene éxito y quedan huecos (índice con signo, 32 máximo); la dirección es `−1` salvo que `fin.z − inicio.z >= 0`;
  - `CColPoint cp` sin inicializar y copia al array con la rutina de 0x40FC80 (no copia los rellenos 0xC, 0x1C y 0x26..0x27).
- `ProcessLineOfSightSectorList`:
  - las marcas "peatones muertos", "moteros" y "ruedas" dependen SOLO de la entidad cabeza de la lista (la DLL las evaluaba por entidad);
  - el código de exploración se pone antes de procesar la entidad, y tras el filtro `usa colisión || marca de peatón`;
  - la distancia de las ruedas parte del valor que había ANTES del modelo principal (la DLL usaba el valor ya actualizado);
  - el producto escalar de la comprobación del cuerpo suma `(y·y + z·z) + x·x` (no x, y, z) y la comparación `|d·r| / |línea| > 0,5` se hace en x87 a 24 bits;
  - la expansión del volumen por `fWeaponSpreadRate` usa `√((dx² + dy²) + dz²)` y recorre las esferas con el contador **con signo** (`movsx`/`jle`);
  - el modelo de colisión del peatón lo da `AnimatePedColModelSkinned` con el clump crudo (sin el `assert` de `GetRpClump`); las ruedas copian el punto con la rutina de 0x40FC80.
- `ProcessLineOfSightSector`: los objetos usan `doCameraIgnoreCheck` (la DLL lo ignoraba); las marcas de peatones/moteros se restauran como 0/1.
- `FindGroundZForCoord`, `FindGroundZFor3DCoord`, `FindRoofZFor3DCoord`, `FindLowestZForCoord`: el `CColPoint`/entidad locales ya no se inicializan (el EXE no escribe nada que no use). Sin más diferencias.
- Coinciden con el EXE sin cambios: `GetIsLineOfSightSectorClear`, `GetIsLineOfSightSectorListClear`, `ProcessVerticalLineSector`, `ProcessVerticalLineSectorList`, `ProcessVerticalLineSector_FillGlobeColPoints`.

**Método:**
- Recorridos de sectores (4 funciones de línea y los 4 `Find*Z`): las funciones de nivel inferior se sustituyen por funciones falsas trazadas (en el EXE y en la DLL) y se compara el hash y las primeras entradas de la secuencia COMPLETA de sectores, sectores repetidos, líneas (6 floats), marcas, salida anticipada, código de exploración final y contador de cruces. `ClearScanCodes` se simula (casos con el código de exploración a 0xFFFF).
- Funciones de sector: `CSector`/`CRepeatSector` falsos; las funciones de lista se sustituyen en el EXE y en las cinco instanciaciones de la DLL (la DLL llama a una instanciación distinta por cada tipo de lista, no a la `<CPhysical*>` enganchada). Se traza también `bIncludeDeadPeds`, `bIncludeBikers` y `fWeaponSpreadRate` en cada llamada.
- Funciones de lista: nodos REALES del pool de nodos dobles (la DLL valida cada nodo; cabecera, estados y almacenamiento se guardan y se restauran por ejecución) con entidades, matrices, modelos de colisión, esferas y vtable falsos (`SetUpWheelColModel` = ranura 43). `CCollision::TestLineOfSight`/`ProcessLineOfSight`/`ProcessVerticalLine`, `CameraToIgnoreThisObject`, `CEntity::GetColModel`, `AnimatePedColModelSkinned` (con la ranura de model info 19000 falsa) y `CPed::IsAlive` se simulan y se trazan. Se comparan retorno, salidas, punto de choque, volúmenes tras ampliar/restaurar, esferas, códigos de exploración, `gaTempSphereColPoints` y contadores.
- Todo el estado global tocado (contadores, marcas, entidad ignorada, `FilledColPointIndex`, `gaTempSphereColPoints`, contador de `CCollision`, pool de nodos, ranura de model info) se guarda y se restaura por ejecución y al final.

**No cubierto:** entidades sin matriz (`AllocateMatrix`/`UpdateMatrix` se ejecutan igual en EXE y DLL: es la ruta de `CPlaceable::GetMatrix`, de otro grupo); listas de edificios de enlace simple reales (los sectores de edificios se prueban con la función de lista simulada); coordenadas NaN/infinitas en los recorridos (los `floor`/`ftol` de 64 bits darían bucles de miles de millones de sectores).

Repetir: `python reversing/WldB/build_probe.py` y `python reversing/run.py --case wldb`. Evidencia: [validation-summary.json](validation-summary.json) (se genera al ejecutar).
