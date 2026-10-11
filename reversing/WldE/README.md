<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8547 funciones reconstruidas; 8547 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9226 entradas binarias sin nombre catalogado; 822 destinos originales explícitos fuera del catálogo, 398 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 102 aprobadas dentro del juego; 133 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CWorld, grupo E: fuego, explosiones, esfera, reposición y apagado (20 funciones)

Validadas el 07/10/2026 con **8.512 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CWorld` (líneas y apagado) | `ResetLineTestOptions`, `ShutDown` |
| `CWorld` (coches y peatones) | `SetAllCarsCanBeDamaged`, `StopAllLawEnforcersInTheirTracks`, `UseDetonator`, `SetPedsOnFire`, `SetPedsChoking`, `SetCarsOnFire`, `SetWorldOnFire`, `SprayPaintWorld` |
| `CWorld` (depuración) | `TestForBuildingsOnTopOfEachOther` (sin argumentos y lista `<CPtrListDoubleLink<CPhysical*>>`), `TestForUnusedModels` (sin argumentos y lista con tabla de contadores) |
| `CWorld` (esfera) | `TestSphereAgainstSectorList<CPtrListDoubleLink<CPhysical*>>`, `TestSphereAgainstWorld` |
| `CWorld` (explosiones) | `TriggerExplosionSectorList<CPtrListDoubleLink<CPhysical*>>`, `TriggerExplosion` |
| `CWorld` (reposición) | `RepositionOneObject`, `RepositionCertainDynamicObjects` |

**Sin cambios (coinciden con el EXE):** `ResetLineTestOptions`, `SetAllCarsCanBeDamaged`, `StopAllLawEnforcersInTheirTracks`, `SetWorldOnFire` y `RepositionCertainDynamicObjects`.

**Diferencias reales corregidas:**
- `ShutDown`:
  - empieza con `CIplStore::Shutdown` (0x405FA0), que faltaba;
  - tras borrar los edificios y los dummies de cada sector se vacían sus dos listas;
  - la comprobación de listas no vacías recorre primero los sectores y después los sectores repetidos, con los mensajes del EXE (`Building overlap list...`, `Vehicle overlap list...`) formateados con `sprintf` y con `x = i % 120`, `y = i / 120` también en los repetidos.
- `TestForBuildingsOnTopOfEachOther` (lista): el EXE solo recorre la lista leyendo posiciones; la comparación de modelos y el aviso de III Mobile no existen.
- `TestForUnusedModels`: sin el informe de modelos sin usar (el EXE termina tras contar); en la lista, el índice de modelo se lee con signo (`movsx`).
- `SetPedsOnFire`, `SetPedsChoking` y `SetCarsOnFire`:
  - la prueba es `|pos - centro| < límite` por eje (resta y valor absoluto en float; z con 5,0; el creador de `SetPedsOnFire` usa la mitad del radio), no `centro ± radio`; con NaN la comparación falla;
  - `SetPedsOnFire` solo enciende a la víctima si NO es invulnerable (o no hay creador, o el creador es el jugador): la DLL tenía la condición invertida.
- `SprayPaintWorld`: la variable de cambio de alfa se lee de 0x8CDEF0; con `processTagAlphaState` a falso el alfa nuevo no se asigna y conserva el valor residual del EXE (primero el número de objetos, después el del tag anterior); la comparación con 255 es la del EXE.
- `UseDetonator`: recorre el pool de atrás adelante (fija el orden de los `RegisterReference`) y no registra la referencia con un creador nulo.
- `TestSphereAgainstSectorList`:
  - el scan code se marca antes de comprobar `ignoreEntity`, la colisión y la cámara;
  - el radio sale del `CColModel` del model info (no del `GetColModel` especial del vehículo);
  - la distancia es `sqrt((dx² + dy²) + dz²)` y solo sigue con `dist < suma` (NaN descarta).
- `TestSphereAgainstWorld` y `TriggerExplosion`: sector `ftol(floor(coord * 0,02f + 60,0f))` con x87 a 24 bits y sector repetido con `& 0xF`; `TriggerExplosion` además no avanza el scan code.
- `TriggerExplosionSectorList`:
  - sin comprobación de scan code, distancia `sqrt((dx² + dy²) + dz²)` y descarte con NaN;
  - las divisiones por radio y por 1400 son multiplicaciones por el inverso (0x864F98), las sumas siguen el orden del EXE y los productos punto se multiplican por la masa;
  - objetos estáticos: se comprueba `bDisableCollisionForce` (no `bDisableTurnForce`), el daño es `min(1, 2·(r-d)/r) * 300` y el cajero (ATM) resta con el valor residual de la pila en lugar del `assert`;
  - vehículos:
    - el centro de la esfera es `-(entidad - punto)` llevado al espacio del vehículo con `Multiply3x3` (0x59C810), no con la rotación directa;
    - la normal usa `-0,2` (no `-0`), la fuerza se aplica con `ApplyForce` (0x542B50, no `ApplyTurnForce`) solo sin `bDisableCollisionForce`, la distancia al punto suma en orden z, y, x, y el temporizador de la bomba se lee sin signo;
    - la normal de la rama de aviones va negada;
  - peatones: la dirección local usa el impacto negado, el aviso de delito se hace en los dos casos (también con el vehículo destrozado) y decide el tipo de la víctima (no el del creador), el empuje usa `paso * masa * 0,008` (no `GetTimeStepInMS`), y el último argumento del evento es `bInVehicle` (bit 8), no `bIsTalking`;
  - objetos dinámicos: producto punto en orden z, y, x.
- `RepositionOneObject`:
  - no usa `FindGroundZFor3DCoord` (devuelve 20,0 sin suelo): hace `ProcessVerticalLine` solo con edificios y usa 0,0;
  - la altura de la caja es como mínimo 2,0 (con NaN conserva la medida);
  - las farolas y semáforos sin datos de colisión no se recolocan; con una caja se usa el centro (x, y) y la z mínima; con esferas, la de menor z por debajo de 1000; y sin cajas ni esferas, la posición propia; el punto se lleva al mundo con `MultiplyMatrixWithVector` (0x59C890, con traslación);
  - la boya sí hace el `ProcessVerticalLine` (con sus efectos) y coloca la z con `(6,0 - alto · 0,5) + alto · 0,2f`.

**Método:**
- Mundo falso: los pools de entidades (vehículos, peatones, objetos, edificios, dummies) y de nodos (simple y doble) se sustituyen temporalmente por pools locales; las listas de sectores, repetidos, LOD y de entidades en movimiento se llenan con nodos locales (la DLL de depuración valida nodos e ítems), y hay 40 modelos falsos (19900..19939) con su `CColModel`, `CCollisionData` y `CObjectData`. Las variables `ModelIndices` (listas de objetos de `RepositionOneObject`, vallas, hidrante, cajero y boya) apuntan a esos modelos.
- Casos: distancias dentro, fuera y en el borde exacto (±1 ulp) de cada límite, radios 0/NaN/enormes, entidades con y sin matriz, pools con huecos libres, scan codes ya vigentes, creadores nulos, jugador, peatones, vehículos y objetos, vehículos BMX y aviones, peatones adjuntos a vehículos destrozados, objetos estáticos con vallas, hidrante y cajero, listas con repeticiones y los dos tipos de lista (suelta de `CPhysical*` y sectores reales).
- Simulados y trazados (en la dirección del EXE y en el símbolo de la DLL; la comparación incluye argumentos y orden): `StartFire`/`GetNumFiresInRange`, `GenerateDamageEvent`, `GetLocalDirection`, `RegisterReference`/`CleanUpOldReference`, `RemoveDetonatorProjectiles`, `ProcessVerticalLine`, `UpdateRwMatrix`/`UpdateRwFrame`, `FindObjectsInRange`, `CTagManager`, `CameraToIgnoreThisObject`, `ProcessColModels`, `FindPlayerPed`, `TryToExplode`, `CGlass`, `AddToMovingList`/`RemoveFromMovingList`, `TriggerWaterHydrant`, `ObjectDamage`, `CEventGroup::Add` (con el contenido del evento), `ComputeDamageResponse`, `ReportCrime`, `InflictDamage`, `SwitchVehicleToRealPhysics`, `ProcessSphereBox`, `GetSpeed`, `ApplyForce`/`ApplyMoveForce`/`ApplyTurnForce`, `CIplStore::Shutdown` y `CPlayerPedData::DeAllocateData`. Las llamadas virtuales (`SetIsStatic`, `Remove`, destructor, `VehicleDamage`) van a una vtable falsa; los eventos `CEventKnockOffBike`/`CEventDamage` se construyen de verdad.
- Se compara bit a bit: la traza completa, el valor de retorno, los bloques de entidades, las matrices, las listas de sectores, repetidos y LOD, los pools de nodos, el scan code global, `gString` y los globales de las pruebas de línea. Todo el estado global tocado se guarda y se restaura en cada ejecución y al final.

Repetir: `python reversing/WldE/build_probe.py` y `python reversing/run.py --case wlde`. Evidencia: [validation-summary.json](validation-summary.json).
