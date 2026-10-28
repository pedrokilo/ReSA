<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Gestor de entradas y salidas (lista de 18)

Validadas el 03/10/2026 con **7.808 pares original/candidato dentro de GTA**, todos exactos: 17 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CEntryExitManager` | `AddEntryExitToStack`, `AddOne`, `DeleteOne`, `EnableBurglaryHouses`, `FindNearestDoor`, `FindNearestEntryExit`, `GetEntryExitIndex`, `GetPositionRelativeToOutsideWorld`, `LinkEntryExit`, `Load`, `PostEntryExitsCreation`, `ResetAreaCodeForVisibleObjects`, `Save`, `SetAreaCodeForVisibleObjects`, `Shutdown`, `ShutdownForRestart`, `Update` |

**Diferencias reales corregidas:**
- `AddEntryExitToStack` apila la entrada recibida (no la enlazada); la comparación con la cima usa la enlazada y `posn` con signo.
- `AddOne`:
  - escribe los campos sobre el slot sin constructor (el byte 0x37 y, sin nombre, los bytes 1..7 del nombre quedan como estaban); nombre con `strncpy` de 8;
  - `UNKNOWN_BURGLARY`: `rand() < 0x3FFF` deja horas (0, 0) después de aplicar `BURGLARY_ACCESS` (0, 24);
  - el rectángulo del árbol se calcula como el EXE: `SetRotateZ` con el ángulo **en grados**, `MultiplyMatrixWithVector` con su orden de sumas, mínimos/máximos con NaN a la rama `else`;
  - grados a radianes con la constante del EXE (0x8595EC); solo enlaza si tiene `CREATE_LINKED_PAIR`.
- `LinkEntryExit`:
  - no comprueba `CREATE_LINKED_PAIR` (lo hacen los llamadores);
  - la otra entrada se enlaza de vuelta solo si **no** tenía enlace (la condición estaba invertida).
- `EnableBurglaryHouses` cambia `ENABLE_ACCESS` solo en las entradas con `BURGLARY_ACCESS` (antes escribía `BURGLARY_ACCESS` en todas).
- `GetEntryExitIndex`, `PostEntryExitsCreation`, `Shutdown`, `ShutdownForRestart`: recorren el pool del último slot al primero (cambia qué entrada devuelve/enlaza y el orden de borrado).
- `FindNearestDoor`: distancia inicial = radio (no `FLT_MAX`), recorrido inverso y distancia real `sqrt((dz² + dx²) + dy²)`.
- `FindNearestEntryExit`: rectángulo en línea (sin el `assert` de `CRect` con rango negativo) y área leída sin signo.
- `Update`:
  - recorre los marcadores aunque no se muestren y mira `bDontShowMarkers` después de la visibilidad;
  - distancia 2D a la cámara (antes 3D);
  - posición del vehículo con `bInVehicle` sin `FindPlayerEntity` (una llamada menos a `FindPlayerPed`);
  - `IsInArea` antes que el flag de acceso.
- `ResetAreaCodeForVisibleObjects` deja el contador en -1 (post-decremento del EXE).
- `SetAreaCodeForVisibleObjects` compara el área con `movzx`.
- `Load`: índices con signo y un slot libre se ignora (antes `NOTSA_UNREACHABLE`). `Save` guarda -1 si el enlace apunta a un slot libre; desde el 04/10/2026 recorre 400 slots fijos con índices crudos como el EXE (ver [EnExInit](../EnExInit/README.md)).

**Método:**
- Pool falso de 400 entradas (tamaño del EXE) con todos los campos finitos; los punteros globales del pool, el árbol, la pila, la entrada activa y la lista de visibles se sustituyen y se restauran en cada ejecución.
- El árbol usa nodos reales del pool de nodos y de `CPtrNodeSingleLink`; se restaura `m_LastFreeSlot` de ambos y se compara el árbol completo (rectángulos, niveles, elementos y orden).
- Se simulan y trazan `rand`, `FindPlayerPed`, `Load/SaveDataToWorkBuffer` (sin partidas), `IsSphereVisible`, `GetIsTimeInRange`, `PlaceMarkerCone`, `IsCoopGameGoingOn`, `TransitionStarted/Finished`, `GetObjectsInFrustum`, `FindObjectsInRange` y `operator delete/delete[]` (solo en `Shutdown`, sin liberar).
- Se guardan y restauran la cámara (posición, frente, espejo), `currArea`, el modo de repetición, la escena, `DisablePlayerControls` y `PC_Scratch`.
- Fuera de los fixtures:
  - `Init`: se valida aparte en [EnExInit](../EnExInit/README.md) (desde el 04/10/2026 crea el pool de 400 entradas como el EXE);
  - pool lleno en `AddOne` (`FIX_BUGS` devuelve -1);
  - el contenido del pool liberado en `Shutdown`; se comparan las liberaciones (desde el 04/10/2026 el pool se destruye en línea como el EXE, sin `~CPool`; la cabecera se compara en EnExInit);
  - pila con más de 4 entradas y contador de visibles negativo (el EXE escribe fuera de los arrays).

Repetir: `python reversing/World18/build_probe.py` y `python reversing/run.py --case world18`. Evidencia: [validation-summary.json](validation-summary.json).
