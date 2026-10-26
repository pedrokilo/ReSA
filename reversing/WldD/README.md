<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CWorld: limpieza y proceso del mundo, grupo D (17 funciones)

Validadas el 07/10/2026 con **6.272 pares original/candidato dentro de GTA**, todos exactos: 17 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CWorld` | `CallOffChaseForArea`, `CallOffChaseForAreaSectorListPeds`, `CallOffChaseForAreaSectorListVehicles`, `ClearCarsFromArea`, `ClearExcitingStuffFromArea`, `ClearPedsFromArea`, `ExtinguishAllCarFiresInArea`, `PrintCarChanges`, `Process`, `ProcessAttachedEntities`, `ProcessForAnimViewer`, `ProcessPedsAfterPreRender`, `RemoveEntityInsteadOfProcessingIt`, `RemoveFallenCars`, `RemoveFallenPeds`, `RemoveReferencesToDeletedObject`, `RemoveStaticObjects` |

**Diferencias reales corregidas:**
- `CallOffChaseForArea` (0x566A60): el EXE amplía el área 10 unidades por cada lado (`minX - 10`, ...), calcula los sectores con `ftol(floor(v * 0,02 + 60))` y llama a las dos funciones de lista con el área exacta **y** la ampliada; la DLL pasaba el área exacta dos veces. Recorre filas (y) por fuera y columnas (x) por dentro sin comprobar que el rango sea válido (con el área invertida no hace nada, en vez de abortar con `assert`).
- `CallOffChaseForAreaSectorListVehicles` (0x563A80):
  - la posición del vehículo se prueba contra los cuatro últimos argumentos (área ampliada) con límites **estrictos** (`>` y `<`, con NaN no pasa); la DLL usaba `IsPointWithinBounds2D`, que es inclusivo;
  - la posición de cada esfera es `matriz * centro` (0x59C890, con la traslación y el orden de sumas z, y, x), no solo la rotación;
  - el contador de esferas se lee con signo y se recorren todas las esferas;
  - el centro del área es `(a + b) * 0,5` y, con NaN en la comparación, el EXE toma la rama del mínimo (la DLL tomaba la del máximo).
- `CallOffChaseForAreaSectorListPeds` (0x563D00):
  - la posición se prueba contra el área exacta (los cuatro primeros límites, estrictos); los cuatro "bigger" no se leen;
  - la condición estaba invertida: el EXE solo continúa con los peatones cuyo objetivo `KillPedOnFoot` es el jugador o que destruyen el coche del jugador; la DLL descartaba justo esos;
  - la tarea es `CTaskSimpleStandStill(0, true, false, 8.0f)` (el EXE pasa `Looped = true`).
- `ClearCarsFromArea` (0x566610): `FindPlayerPed()` se desreferencia en cada vehículo antes de mirar si es una lancha.
- `RemoveVehicleAndItsOccupants` (inline de `ClearCarsFromArea` y `ClearExcitingStuffFromArea`): tras `RemovePed(conductor)` el EXE llama a `CleanUpOldReference` si `m_pDriver` sigue puesto y lo pone siempre a nulo.
- `ClearExcitingStuffFromArea` (0x56A0D0):
  - el grupo del jugador (`FindPlayerPed` + `GetPedsGroup`) se consulta por cada ped candidato y por cada vehículo, no una sola vez;
  - la distancia 2D es `dx² + dy²` contra `radio²`, estricta y sin pasar con NaN (también para vehículos, que ahora se descartan con `!(d < r²)`).
- `ExtinguishAllCarFiresInArea` (0x566950): la distancia es `(dz² + dy²) + dx²` y la comparación con `radio²` es estricta (con igualdad o NaN no se apaga); la DLL usaba `<=` y otro orden de sumas.
- `PrintCarChanges` (0x566420):
  - la tabla de modelos es la del EXE (0xB7CBA8) y no una variable estática de la DLL;
  - solo cuentan los automóviles (`m_nVehicleType == 0`); el resto de tipos y las ranuras libres se registran como modelo 0 (la DLL hacía justo lo contrario: saltaba los automóviles y los libres);
  - el modelo se extiende con signo (`movsx`) y el aviso sale por el `printf` del CRT del juego (0x821982) con (ranura, anterior, nuevo).
- `ProcessForAnimViewer` (0x5633D0): después de `UpdateRwMatrix` llama a `CEntity::UpdateRwFrame` (la DLL no lo hacía).
- `RemoveFallenPeds` (0x565CB0) y `RemoveFallenCars` (0x565E80):
  - el umbral es estricto (`z < -100`; con igualdad o NaN no se hace nada);
  - `FindNodeClosestToCoors` recibe la distancia máxima 999999.88 (0x497423FE) y no 1000000;
  - los coches buscan nodos de vehículo (tipo 0, `PATH_TYPE_VEH`) y no de peatón;
  - solo se comprueba el área del nodo devuelto (`cmp ax, 0FFFFh`) y el nodo se lee directamente de `m_pPathNodes` (sin `assert` de `GetPathNode`);
  - sin nodo, el ped solo escribe la z de su matriz (-95) y el coche se teletransporta a (x, y, 0); el nodo de coche suma 3 a z con una sola operación.
- `Process` (0x5684A0):
  - solo se añaden los eventos globales a los grupos activos (`CPedGroups::ms_activeGroups`);
  - el amortiguado del jugador atascado usa el CRT `pow` del EXE (0x822130, precisión completa) con base 0,707 (0x8CDEEC, no `√2/2`) y como exponente `ms_fTimeStep` (no el paso en milisegundos), multiplicando cada componente y redondeando a `float`;
  - `IKChainManager_c::Update` recibe `ms_fTimeStep * 0,02` (no una división entre 50).

**Método:**
- Original con el hook de cada función desactivado frente a candidato, con fixtures idénticos y la misma semilla; se llama a la dirección del EXE con su convención (todas `cdecl`).
- Nunca se opera sobre pools ni listas reales del juego: los pools de peatones, vehículos, objetos, edificios, dummies y los dos de nodos (simples y dobles) se sustituyen por pools falsos que viven en un arena propio (misma disposición en las dos ejecuciones); las listas de sectores, de sectores repetidos, de entidades en movimiento y de objetos con código de control se vacían y se rellenan con nodos del pool falso (la DLL en Debug valida cada nodo y cada elemento contra esos pools).
- Entidades falsas: memoria cruda con los campos que lee el EXE (posición por matriz o por transformación simple, objeto RenderWare con marco, banderas, escaneo, tipo/estado, velocidades, datos de vehículo y de peatón, inteligencia con tareas, grupos de peatones, colmodels con esferas) y una vtable falsa trazada (destructor, `Add`, `Remove`, `ProcessControl`, `ProcessCollision`, `ProcessShift`, `Teleport`, `RemoveRefsToVehicle`) que nunca destruye ni mueve nada; `ProcessControl`/`ProcessCollision`/`ProcessShift` alteran las banderas de forma determinista para recorrer las ramas de `Process`.
- Simulados y trazados (en el EXE y en la DLL, incluyendo el destino de los hooks activos): `FindPlayerPed`/`FindPlayerVehicle`/`FindPlayerWanted`, `CPhysical::AddToMovingList`/`RemoveFromMovingList`/`RemoveRefsToEntity`/`PositionAttachedEntity`/`ApplyMoveSpeed`/`ApplyTurnSpeed`, `CEntity::UpdateAnim`/`UpdateRwFrame`/`GetColModel`/`CleanUpOldReference`, `CMatrix::UpdateRwMatrix` y `CSimpleTransform::UpdateRwMatrix`, `CPopulation::RemovePed`, `CPed::IsPlayer`/`CanBeDeleted`/`IsAlive`/`PositionAttachedPed`, `CVehicle::CanBeDeleted`/`ExtinguishCarFire`/`RemovePassenger`, `CCarCtrl`, `CGarages`, `CPedGroups::GetPedsGroup`/`Process`, `CWanted::RemovePursuitCop`, `CTaskManager::FindActiveTaskByType`, `CTask::operator new`/`delete` (buffer propio rellenado con 0xCD), `CEventGroup::Add` (se compara el evento y los bytes de la tarea), los eventos globales y las colas, `CReferences`, `CObject::ProcessControlLogic`, el monitor de carga, `CVehicleRecording`, `IKChainManager_c`, `CPedIntelligence`, `CMessages`, `CPlayerInfo::Process`, `CPedScriptedTaskRecord`, los efectos de `ClearExcitingStuffFromArea`, `CPathFind::FindNodeClosestToCoors` (0x44F460) y el `printf` del CRT (0x821982). Los constructores y destructores del evento y de la tarea de `CallOffChaseForAreaSectorListPeds` se usan reales.
- Se comparan la traza completa, un resumen de todo el arena (entidades, matrices, pools falsos con sus estados y `m_LastFreeSlot`, listas), la memoria de sectores y los globales tocados (pools, escaneo, contador de fotogramas, paso, banderas de `CWorld`, modo de repetición, objetos de cutscene, jugadores, grupos activos, tabla de modelos y puntero de nodos de ruta). Todo se guarda y se restaura en cada ejecución y al final.
- Casos con límites exactamente sobre la posición de las entidades, NaN en posiciones, infinitos en el área (centro NaN), contador de escaneo en 0xFFFF (`ClearScanCodes`), áreas invertidas, `numSpheres` negativo, modelos mayores de 0x7FFF, nodos de ruta con área válida y nodo 0xFFFF (la matriz de nodos falsa cubre los 65536 nodos), jugador dentro y fuera de las listas, coches y peatones con y sin matriz.
- Fuera del banco: el EXE desreferencia nulos en `CallOffChaseForAreaSectorListVehicles` (`m_pColData` nulo) y en `RemoveFallenPeds` (matriz nula sin nodo de ruta); la DLL conserva la guarda del primero y escribe la matriz sin comprobar en el segundo, y el banco no los provoca.

Repetir: `python reversing/WldD/build_probe.py` y `python reversing/run.py --case wldd`. Evidencia: [validation-summary.json](validation-summary.json).
