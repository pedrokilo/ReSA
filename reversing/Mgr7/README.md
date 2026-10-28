<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Saltos, generadores de coches, muebles, criaturas y tareas de interior (lista de 7)

Validadas el 02/10/2026 con **15.488 pares original/candidato dentro de GTA**, todos exactos: 49 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CStuntJumpManager` | `Init`, `Shutdown`, `ShutdownForRestart`, `Save`, `Load`, `AddOne`, `Update` |
| `CTheCarGenerators` | `CreateCarGenerator`, `Get`, `Init`, `Load`, `Process`, `RemoveCarGenerators`, `Save` |
| `FurnitureManager_c` | `Exit`, `GetFurniture`, `GetGroupId`, `GetRandomId`, `GetSubGroupId`, `Init`, `LoadFurniture` |
| `WaterCreatureManager_c` | `CanAddWaterCreatureAtPos`, `Exit`, `GetRandomWaterCreatureId`, `Init`, `TryToExitGroup`, `TryToFreeUpWaterCreatures`, `Update` |
| `CTaskInteriorGoToInfo` | Constructor, Destructor, `Clone`, `GetTaskType`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |
| `CTaskInteriorLieInBed` | Constructor, Destructor, `FinishAnimCB`, `Clone`, `GetTaskType`, `MakeAbortable`, `ProcessPed` |
| `CTaskInteriorShopKeeper` | Constructor, Destructor, `Clone`, `GetTaskType`, `CreateNextSubTask`, `CreateFirstSubTask`, `ControlSubTask` |

**Diferencias reales corregidas:**
- `CStuntJumpManager`:
  - el pool de saltos sigue la instancia del EXE: ctor sin rellenos, `Shutdown` libera el pool, `Clear` pone a cero el contador de referencia y `New` busca desde `m_LastFreeSlot+1`;
  - `AddOne` devuelve el índice del salto (o -1) y no suma si el pool está lleno;
  - `Update` usa la info del jugador en foco, recorre los 256 slots sin salir del bucle, la frase del pasajero solo suena al cruzar el segundo de vuelo (la condición estaba invertida), el tiempo es `ftol(paso * 0,02 * 1000)` con signo, un choque tras 100 ms termina sin mirar la caja final y los mensajes van en el orden del EXE (USJ, USJ_ALL, REWARD);
  - `m_jumpState` se escribe como dword.
- `CTheCarGenerators`: `Save`/`Load` devuelven `true`; el bucle de `Load` usa un contador de 16 bits comparado con signo.
- `FurnitureManager_c`:
  - `GetGroupId`/`GetSubGroupId` son `__thiscall` (`ret 4`), no funciones estáticas;
  - los contadores 0xBAB378/0xBAB37C son words;
  - `LoadFurniture` no valida nada, ignora etiquetas desconocidas y añade los ITEM a través del grupo (todos los subgrupos con ese ID).
- `WaterCreatureManager_c`:
  - `Init` añade los elementos en orden ascendente;
  - `GetRandomWaterCreatureId` usa las tiradas en línea del EXE;
  - `TryToFreeUpWaterCreatures` solo comprueba el límite después de liberar;
  - `CanAddWaterCreatureAtPos` compara la distancia 2D al cuadrado con la distancia mínima al cuadrado;
  - `TryToExitGroup` usa `<= 3600` con el orden de sumas del EXE;
  - `Update` compara con signo, calcula la dirección en línea, usa `GetRandomIntInRange` y deja de crear tras el primer `Init` fallido.
- `CTaskInteriorGoToInfo`: el rumbo es `LimitRadianAngle(GetRadianAngleBetweenPoints(dir.x, dir.y, 0, 0))`, NaN crea la ruta, dos llamadas a `GetTaskType` y el nodo se lee sin comprobar el área.
- `CTaskInteriorLieInBed`:
  - `Clone` copiaba sus propios campos sin inicializar;
  - el callback que se restaura es el de borrado (`SetDeleteCallback`), no el de fin;
  - `ProcessPed` sigue el EXE: sin temporizador al entrar en la cama, `m_UpdatePedPos` se consume, al abortar se cambia la velocidad (no el delta), el desplazamiento solo durante la animación de entrada con el orden de sumas del EXE y el rumbo limitado;
  - sin value-init del temporizador.
- `CTaskInteriorShopKeeper`: `Clone` copia la info y el interior, `CreateFirstSubTask` marca `m_bIsSetup` la primera vez y `ControlSubTask` consume el aviso de cobrar.

**Método:**
- El pool de saltos vive en una arena propia; `operator new/delete` del EXE se simulan sin reservas.
- Generadores, gestor de criaturas y estado de saltos se rellenan por caso y se restauran tras cada ejecución.
- El gestor de muebles es un buffer propio con subgrupos falsos; `furnitur.dat` se simula línea a línea.
- Las criaturas usan objetos falsos con vtable trazada; `CWorld::Remove` se simula.
- Las tareas usan peatón, matriz, clump, inteligencia, interiores, nodos y animaciones falsos; las tareas creadas se buscan en el pool y se borran.
- Se simulan y trazan el RNG, el buffer de guardado, la cámara, el audio, los textos, los mensajes, el agua y las funciones de interiores y animación.
- Fuera de los fixtures: `m_bRghtHandSide` (+0x10) de `CTaskInteriorLieInBed` se enmascara en el constructor y en `Clone` (FIX_BUGS lo escribe; el EXE no).

Repetir: `python reversing/Mgr7/build_probe.py` y `python reversing/run.py --case mgr7`. Evidencia: [validation-summary.json](validation-summary.json).
