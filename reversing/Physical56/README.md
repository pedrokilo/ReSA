<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPhysical (lista de 34)

Banco preparado el 04/10/2026 y ampliado el 05/10/2026 con **23.808 pares original/candidato dentro de GTA** (las 56 funciones de `CPhysical` con casos propios; `Add` y `RemoveAndAdd` añadidas el 05/10 con 512 y 1.024 casos). Las 54 anteriores ya se corrieron y aprobaron (22.272 pares); las cifras nuevas están pendientes de compilar y correr.

| Grupo | Funciones |
|---|---|
| Fuerzas y velocidades | `ApplyMoveForce` (vector), `ApplyTurnForce`, `ApplyForce`, `GetSpeed`, `ApplyMoveSpeed`, `ApplyTurnSpeed`, `ApplyGravity`, `ApplyFrictionMoveForce`, `ApplyFrictionForce`, `ApplyAirResistance`, `ApplySpeed`, `UnsetIsInSafePosition` |
| Muelles | `ApplySpringCollision`, `ApplySpringCollisionAlt`, `ApplySpringDampening` |
| Respuesta a colisiones | `ApplyCollision` (3 y 4 argumentos), `ApplySoftCollision` (3 y 4 argumentos), `ApplyCollisionAlt`, `ApplyFriction` (propia, con otro cuerpo y sin argumentos), `SetDamagedPieceRecord` |
| Estado y registros | `SkipPhysics`, `ProcessControl`, `AddCollisionRecord`, `GetHasCollidedWith`, `GetHasCollidedWithAnyObject`, `RemoveRefsToEntity`, `CanPhysicalBeDamaged`, `GetLightingFromCol`, `GetLightingTotal`, `GetBoundRect` |
| Ciclo de vida y listas | Constructor, Destructor, `Add` (virtual, ranura 2), `Remove`, `RemoveAndAdd`, `AddToMovingList`, `RemoveFromMovingList` |
| Enganches | `AttachEntityToEntity` (vector y cuaternión), `DettachEntityFromEntity`, `DettachAutoAttachedEntity`, `PositionAttachedEntity`, `PlacePhysicalRelativeToOtherPhysical` |
| Mundo | `ProcessEntityCollision`, `CheckCollision`, `CheckCollision_SimpleCar`, `ProcessCollisionSectorList`, `ProcessCollisionSectorList_SimpleCar`, `ProcessShiftSectorList`, `TestCollision`, `ProcessCollision`, `ProcessShift` |

**`Add` y `RemoveAndAdd` (05/10/2026):** ya no son "solo conmutadas". Antes figuraban con 0 casos porque se habían validado en [UnlockPhysical](../UnlockPhysical/README.md) (3.072 pares) con un resumen antiguo sin `covered_functions`, y `docs/hooks.csv` las marcaba como no validadas. Ahora tienen casos propios (≥256 cada una) con la técnica de ese banco:
- **Fuente comparada con el EXE** (0x544A30 y 0x542560, EXE 1.0 US): sin diferencias. Rectángulo en memoria (izq, arriba, der, abajo), conversión `coord·0,02+60` en x87 (`fld`/`fmul`/`fadd`, `fstp` a double, `floor` y `_ftol`), orden de evaluación izq/abajo/der/arriba, bucle y de abajo a arriba y x de izquierda a derecha, celda `[y&15][x&15]`, lista por tipo (vehículo +0, peatón +4, objeto +8), nodo `CEntryInfoNode` antes que el nodo de puntero en `Add` y al revés en la ruta de alta de `RemoveAndAdd`, redistribución con desenlace/reenlace del nodo existente (cursor `m_next`) y retirada de sobrantes (nodo de puntero y luego entrada), y edificio grande (bit 0x100 de +0x1C) delegando en `CEntity::Add` (0x533020) y `CEntity::Remove` (0x534AE0).
- **Caso:** `GetBoundRect` (ranura 9 de la vtable falsa) devuelve un rectángulo inyectado con coordenadas en múltiplos de 50 y sus vecinos de 1 ulp, valores negativos de hasta −4.000, extremos de ±1e7, rangos vacíos en X o en Y y rangos de hasta 7×7 celdas (que se solapan por la máscara `&15`).
- **Mundo:** listas de repetición reales con entidades de fondo (también en las celdas destino), de 0 a 20 nodos previos en `m_pCollisionList` (algunos en la celda destino y en la misma lista) para reutilizar, mover y retirar, y pools locales (doble enlace y `CEntryInfoNode`) con su relleno 0xFD.
- **Se compara:** los 3×256 listas de repetición (nodos, elementos y anteriores), la cadena `m_pCollisionList` (índices de nodo, enlace, celda, lista, anterior y siguiente), los pools completos (contenido y estados), `CReferences`, la entidad y la traza (`bound_rect`, `entity_add`, `entity_remove`).
- **Dobles trazados nuevos:** `CEntity::Add` (EXE 0x533020 y símbolo de la DLL), además del ya existente `CEntity::Remove`.
- **Fuera de alcance:** agotamiento de pools y tipos de entidad distintos de vehículo/peatón/objeto sin edificio grande (el EXE usa un registro sin inicializar y la DLL aborta a propósito).
- **Resumen:** `probe_summary.py` toma las claves de `pairs_per_function` (`CPhysical::Add`, `CPhysical::RemoveAndAdd`, de los eventos `function_summary`/`comparison` del banco) y las escribe en `covered_functions`; al regenerar `validation-summary.json` tras correr, `hooks.csv` las marcará como validadas.

**Diferencias reales corregidas:**
- **Orden x87 en todo el archivo:**
  - el centro de masas se transforma con el orden de `Multiply3x3` (0x59C790), no con el de `TransformVector`; los puntos con el de 0x59C890;
  - `CVector / float` se hace como en 0x4119D0: recíproco redondeado y producto;
  - las masas también usan el recíproco (`1/m`);
  - las magnitudes siguen el orden de sumas de cada sitio.
- **`GetSpeed`:** suma `(movimiento + giro×r) + fricción`.
- **Constantes y comparaciones:**
  - `pow` (resistencia del aire, fricción, `ProcessShift`) llama a `_CIpow` del EXE (0x822130);
  - `GetLightingFromCol` usa la constante 0x3EAAAA9F, que no es el float más cercano a 1/3;
  - el ángulo de las puertas y π salen del EXE;
  - las comparaciones con NaN siguen los saltos del EXE, y algunas comparan contra el double 0,01.
- **`AddCollisionRecord`:** la alarma se compara con 0xFFFF (antes `uint16 == -1`, nunca cierto).
- **`RemoveRefsToEntity`:** estaba roto (bucle vacío); ahora compacta la lista como el EXE.
- **`CanPhysicalBeDamaged`:** el cóctel molotov también mira `bExplosionProof`, y las armas fuera de rango devuelven cierto sin tocar la bandera.
- **`ApplyAirResistance`:** compara `!(aire > 0,1)` y mira el subtipo de vehículo (+0x594).
- **`ApplyCollision` (3 argumentos) y `ApplySoftCollision` (3 argumentos):** la magnitud es la del producto vectorial, no la de la normal.
- **`ApplyCollision` (4 argumentos):**
  - la condición de masa usa `m_bHasHitWall` (bit 12), no `m_bIsStuck`, y `*0,0002`;
  - el bit de velocidad por colisión en movimiento estaba invertido, y se miraba el tipo del cuerpo equivocado;
  - el daño a la propiedad suma `rand()%30+30` con el `rand` del EXE.
- **`ApplySoftCollision` (4 argumentos):**
  - tras el ramal estático, devuelve falso si el cuerpo sigue estático o llama a `AddToMovingList`;
  - `GetSpeed` se llama solo donde lo hace el EXE;
  - el modelo de la boca de incendios es la variable del EXE.
- **`ProcessCollisionSectorList`:**
  - las listas se recorren en el orden del EXE (edificios, vehículos, objetos, peatones) y la celda de repetición es `&15`;
  - faltaba `SetDamagedPieceRecord` sobre la otra entidad en la ruta blanda;
  - los barcos se saltan el bloque de estado, el tren se mira por subtipo, y hay guarda de inicialización.
- **`ProcessCollisionSectorList_SimpleCar`:** primero objetos y luego vehículos.
- **`ProcessShiftSectorList`:**
  - el orden es objetos, peatones, vehículos y edificios;
  - se recalcula el filtro por entidad;
  - el tren se mira en `this`;
  - el desplazamiento del peatón es `vec · clamp(1,5·prof, 0,005, 0,3)`.
- **`ProcessCollision`:** el coche aparcado es `bRestingOnPhysical` (0x42C bit 1), y en la ruta fantasma `GetHeightAboveRoad` va antes que `FindGhostRoadHeight`.
- **`ProcessShift`:** la amortiguación del peatón mira las banderas físicas de la entidad que pisa (y amortigua si no hay ninguna). Los sectores se calculan con `·0,02 + 60` y `floor`.
- **`CheckCollision`:** sectores con `·0,02 + 60` (antes `/50`).
- **`PositionAttachedEntity`:**
  - la fuerza al soltar es `masa·0,01` y `masa·0,02`;
  - cuaternión a matriz y `Rotate` siguen el orden del EXE;
  - la diferencia de rumbo usa `fpatan` sin redondear y `·1e-5`.
- **`AttachEntityToEntity` (las dos):** la entidad ignorada es la recién enganchada.
- **`DettachEntityFromEntity`:** la matriz es `padre · separación`, no al revés.
- **Constructor:** se conserva el NOTSA `m_nContactSurface = SURFACE_DEFAULT`; sin él el audio de peatones y vehículos indexaría `g_surfaceInfos` fuera de rango en la DLL de depuración.

**Método:**
- **Entidades falsas:** 8 bloques de 0xA20 bytes con desplazamientos crudos (vehículos, peatones, objetos y edificios), cada uno con matriz, masas, velocidades, banderas, registros de colisión, daño y datos derivados.
- **vtable falsa:** sus ranuras usadas son stubs trazados (`SetIsStatic`, `GetBoundRect`, `SpecialEntityPreCollisionStuff`, `SpecialEntityCalcCollisionSteps`, `ProcessEntityCollision`, `AddMovingCollisionSpeed`, `GetHeightAboveRoad`); cualquier otra aborta.
- **Model info y colisión falsas:** cubren el índice 1 y los índices que compara el código (bola blanca, boca de incendios, parquímetros, montacargas, dumper, satchel).
- **Mundo falso:**
  - 3×3 sectores (109..111) y sus celdas de repetición se vacían y se rellenan con nodos de pools **locales** (simple, doble y `CEntryInfoNode`);
  - los pools de vehículos, peatones, objetos y edificios son **falsos** y cubren las entidades, porque la DLL de depuración valida nodos e `Item`.
- **Funciones simuladas y trazadas:**
  - `rand` (EXE, CRT de la DLL y `CGeneral::GetRandomNumber`), `ProcessColModels`, audio, `ObjectDamage`, `CGlass`, `g_fx`, dinero, estadísticas;
  - `FindPlayerPed`/`FindPlayerVehicle`, resistencia extra del aire, `CWorld::Add/Remove/ClearScanCodes`, `CCarCtrl`, `KillPedWithCar`, `GetTaskClimb`, `CalculateLeanMatrix`, `CEntity::Add` y `CEntity::Remove`;
  - en constructor y destructor: `AllocateStaticMatrix`, `ReturnRealTimeShadow`, `CEntryInfoList::Flush` y `~CEntity`.
- **Estado global** que se guarda y se restaura en cada ejecución: paso de tiempo, reloj, área, jugador en foco y `CPlayerInfo`, `bNoMoreCollisionTorque`, `bSecondShift`, mesa de billar, código de búsqueda, los tres arrays estáticos de `CColPoint`, la lista de entidades en movimiento, los punteros de los pools, `ms_modelInfoPtrs`, `CReferences` y los sectores.
- **Retornos en ST0:** se comparan con sus 80 bits.
- **Fuera de la comparación o de los fixtures:**
  - el byte 0xBA y el puntero de vtable en constructor y destructor;
  - `bDisableZ` (billar) solo en objetos y con velocidades pequeñas: el `CColPoint` local de `ApplyFriction()`/`ApplySpeed` solo tiene punto y normal en el EXE, y la ruta de chispas leería superficies de la pila.

Repetir: `python reversing/Physical56/build_probe.py` y `python reversing/run.py --case physical56`.

**Ajustes del banco tras la primera corrida (04/10/2026):**
- `mats` ya no compara las palabras de relleno (+0x0C/+0x1C/+0x2C/+0x3C) de las matrices: el EXE las copia desde `CMatrix::operator*` (0x59BE30, usado por `operator*=` 0x411A80 y por la composición cuaternión-matriz), que deja esos huecos con basura de pila. Afectaba a `AttachEntityToEntity` (las dos) y `PositionAttachedEntity`.
- `ApplySpeed`/`UnsetIsInSafePosition`/`ApplyFriction-void` ponen a cero la pila bajo la llamada (`ScrubStack`): el `CColPoint` local de `ApplyFriction` en el EXE tiene sin inicializar las superficies (+0x20/+0x23) que lee la ruta de chispas; la DLL las pone a cero con `{}`.
