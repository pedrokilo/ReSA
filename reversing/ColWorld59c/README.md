<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Colisión de mundo y cámara (lista de 59)

Banco preparado el 05/10/2026: **13.568 pares original/candidato dentro de GTA** para 20 funciones de `CCollision` (pendiente de compilar y ejecutar). La función 21 de la lista, `SphereVsEntity` (0x41A5A0), queda fuera (ver más abajo).

| Clase | Funciones |
|---|---|
| `CCollision` (ciclo de vida) | `Init`, `Shutdown`, `Update`, `SortOutCollisionAfterLoad` |
| `CCollision` (línea de visión) | `TestLineOfSight`, `ProcessLineOfSight` |
| `CCollision` (esferas y cámara) | `SphereCastVsBBox`, `SphereCastVsSphere`, `SphereCastVersusVsPoly`, `SphereCastVsEntity`, `SphereCastVsCaches`, `CameraConeCastVsWorldCollision`, `BuildCacheOfCameraCollision` |
| `CCollision` (mundo y peatones) | `CheckCameraCollisionBuildings`, `CheckCameraCollisionVehicles`, `CheckCameraCollisionObjects`, `CheckCameraCollisionPeds`, `CheckPeds`, `IsThisVehicleSittingOnMe`, `ResetMadeInvisibleObjects` (global, categoría `CCollision`) |

Los 59 enganches de `CCollision` seguían bloqueados por `bEnableHooks = false`. Las 21 líneas de este banco pasan a enganches normales (sin `enabled`/`locked`; la de `SphereCastVsSphere` también perdía su `{ .locked = true }`). El `bEnableHooks` global y las líneas de otros bancos no se tocan.

**Diferencias reales corregidas (comparadas con el EXE 1.0 US, sin `FIX_BUGS`):**
- `Shutdown`: recorría la lista LIBRE desde `freeListTail.prev`; el EXE recorre la USADA desde `usedListTail.prev` (0x96593C) y llama al miembro `CCollisionData::RemoveTrianglePlanes` (0x40F6A0), no a `CCollision::RemoveTrianglePlanes` (que además recoloca el enlace).
- `TestLineOfSight`: la condición de superficie estaba invertida; el EXE OMITE la superficie cuando la comprobación está activa y `IsSeeThrough`/`IsShootThrough` es cierto. Cajas y caja envolvente con `TestLineBox_DW`, contadores de esferas, cajas y triángulos con signo (`movsx`/`jle`) y transformaciones con el orden de sumas de `MultiplyMatrixWithVector` (0x59C890; `TransformPoint` de la DLL suma x, y, z y el EXE z, y, x).
- `ProcessLineOfSight`: `ms_iProcessLineNumCrossings` solo sube cuando `ProcessLineTriangle` devuelve cierto (la DLL lo subía siempre); sin `assert` de datos nulos; punto y normal con `MultiplyMatrixWithVector`/`Multiply3x3` (0x59C790).
- `ResetMadeInvisibleObjects`: recorría una copia del puntero, así que `CleanUpOldReference` buscaba la dirección equivocada y la casilla no se ponía a nulo. Ahora trabaja sobre la propia casilla de `gpMadeInvisibleEntities` (la referencia se registró con su dirección), relee el contador con signo en cada vuelta y deja la casilla a nulo.
- `CheckCameraCollisionPeds`:
  - `GetTaskHold(false)` (el EXE hace `push 0`; la DLL usaba el valor por defecto `true`);
  - NaN en la distancia al cilindro o en la distancia a la cámara descarta al peatón (`fcomp` + `test ah,5/41h`);
  - se guarda y registra la referencia antes de ocultar al peatón, sin límite del array (`std::array::data`);
  - índice del sector repetido con `& 0xF` (la DLL usaba `abs() % 16`: un sector negativo cae en otra lista);
  - la función 0x50D590 que reimplementa la DLL tenía el mínimo inicial en `FLT_MAX` (el EXE usa 999999.0f), no comprobaba nada del número de esferas (el EXE recorre 12 fijas, en 2 bloques de 6) y sumaba el producto escalar x, y, z (el EXE suma z, x, y en las esferas 0 y 1 de cada bloque y z, y, x en las otras cuatro, y z, y, x en `front·source`).
- `CheckPeds`: bucle propio con los límites leídos una vez; `CWorld::IterateSectors` abortaba (`assert`) con rangos invertidos.
- `BuildCacheOfCameraCollision`:
  - centro de la esfera envolvente = `tamaño · 0,5 + mínimo` y radio = `sqrt((z² + y²) + x²) · 0,5` (la DLL usaba `(max + min) / 2` y `Magnitude()`);
  - sectores con `floor(v · 0,02 + 60)` (la DLL dividía entre 50), izquierda y abajo acotados solo por debajo (a 0) y derecha y arriba solo por arriba (a 119);
  - busca primero la tarea de entrar de CONDUCTOR (701) y después la de pasajero (700) (la DLL las probaba al revés);
  - `FindPlayerVehicle(0, false)`, copia de `FindPlayerSpeed(0)` (no un puntero a la velocidad viva) y las tres opciones de la cámara leídas una vez antes del bucle; bucle sin `assert`;
  - con rango vacío el EXE devuelve basura de pila (la DLL devuelve falso).
- `CameraConeCastVsWorldCollision`:
  - el valor que se compara con el límite de la búsqueda binaria es `max - min` calculado AL FINAL de cada vuelta (la DLL comparaba el valor anterior y hacía una vuelta de más);
  - `gLastRadiusUsedInCollisionPreventionOfCamera` guarda el radio final de la esfera (`dst · radio`), no `dst`;
  - `gColCacheNumEntries` y los punteros de las cachés no se actualizan durante la búsqueda (el número de entradas va en un registro);
  - las dos cachés quedan sin inicializar como en el EXE.
- `SphereCastVersusVsPoly`: normal (`int16 · 1/4096`) y término constante (`int16 · 1/128`) leídos en crudo; productos escalares con suma z, y, x (también `|AB|²`); NaN cuenta como "tocando el plano"; la proyección interpolada usa `AB · ratio + proyección`.
- `SphereCastVsCaches`:
  - recorrido lineal de todas las entradas con el contador del EXE (la entrada con entidad abre región; las siguientes la reutilizan);
  - la cara trasera (índice ≥ 0x7FFF) llama con las esferas INTERCAMBIADAS y el índice `0xFFFF - idx` (la DLL no las intercambiaba);
  - la entrada de salida solo escribe tipo, entidad e índice propio, y tras cada colisión deja la casilla siguiente con entidad nula y tipo NONE (la DLL escribía los tres índices);
  - transformación de los centros con `MultiplyMatrixWithVector`.
- `SphereCastVsEntity`:
  - AMBAS esferas en espacio objeto usan el radio de `spAws` (el EXE copia `[spA + 0xC]` en las dos; antes se "arreglaba" con el radio de `spBws`);
  - matriz creada e inversa calculada antes de mirar si hay datos de colisión;
  - las esferas se recorren de la ÚLTIMA a la primera, los triángulos del último al primero, las cajas de la última a la primera;
  - los grupos de caras se leen desde el final (el grupo 0 está justo antes de los triángulos) y con índices con signo;
  - la prueba de doble cara era `abs(bool)`: el EXE comprueba `|normal.z (int16 · 1/4096)| < 0,05`;
  - `gbTryDoubleSidedCollision` se lee una vez;
  - las entradas se escriben campo a campo como en el EXE;
  - límite de 99 entradas con signo y sin `assert`.
- `CheckCameraCollisionBuildings`: la condición de vuelos estaba negada (el EXE omite si `DoesNotCollideWithFlyers`) y la entidad ignorada se comprobaba solo con esa opción (el EXE la comprueba siempre); `FindPlayerVehicle(-1, false)`; esfera de la entidad = `GetBoundCentre` + radio de la colisión del MODELO base (`modelinfo + 0x14`, `+0x24`), no la esfera transformada de `GetColModel()`.
- `CheckCameraCollisionVehicles`: velocidad relativa con `(dz² + dy²) + dx²` y NaN descarta (la DLL sumaba x, y, z y dejaba pasar NaN); sector con `& 0xF`; misma esfera de entidad que arriba; sin `AsVehicle()` sobre un `pIgnoreEntity` posiblemente nulo.
- `CheckCameraCollisionObjects`: `CameraToIgnoreThisObject` y la entidad ignorada se omiten POR SEPARADO (la DLL los unía con `&&`); sector con `& 0xF`.

**Sin cambios (coinciden con el EXE):** `Init` (reserva `new[]` de 0x258 bytes, 50 enlaces insertados del último al primero en la lista libre, `ms_collisionInMemory = 0`, salto a `CColStore::Initialise`), `Update` (solo `ret`), `SortOutCollisionAfterLoad` (`CColStore::LoadCollision(posición de la cámara, false)` y `LoadAllRequestedModels(false)`), `IsThisVehicleSittingOnMe` (tipo 0 → +0x8E8, tipo 9 → +0x7C0, 4 casillas), `SphereCastVsBBox` y `SphereCastVsSphere`.

**`SphereVsEntity` (0x41A5A0) sin validar a propósito:** el EXE sí tiene un cuerpo completo (no es un `ret`), pero nadie lo llama (búsqueda de `call/jmp` y punteros en todo el `.text`) y no lee su argumento `sphere`: trabaja con una `CColSphere` local sin inicializar (basura de pila), así que no hay comportamiento determinista que comparar. Se deja `NOTSA_UNREACHABLE` y el enganche normal (sin llamadores no se ejecuta).

**Método:**
- Mundo falso: se guardan y vacían los sectores reales (0xB7D0B8 y 0xB992B8, copia completa) y se rellenan con nodos de fixtures. La DLL de depuración valida cada nodo y cada elemento contra los pools reales, así que se intercambian temporalmente los punteros de los pools de nodos (simple y doble enlace) y de edificios, peatones, vehículos y objetos por cabeceras falsas (capacidad doble o mayor para no depender del tamaño exacto del elemento).
- Entidades de 4 tipos con offsets crudos (banderas +0x1C con visibilidad y colisión, código de escaneo +0x2C, modelo +0x22, tipo +0x36, matriz +0x14, velocidad +0x44, tipo de vehículo +0x590, ruedas +0x8E8/+0x7C0, inteligencia +0x47C) y 7 modelos falsos en `ms_modelInfoPtrs` (slots 1..7) con colisión propia: esferas, cajas, triángulos con planos calculados en el fixture, grupos de caras (hasta 3, el contador va en `triángulos - 4`) y, para el peatón, colisión de golpe con 12 esferas en +0x34.
- Simulados con trazas, sin reservar memoria (los parches solo están activos durante la llamada): `FindPlayerPed/Vehicle/Speed` (con los argumentos que recibe cada versión), `CPed::IsPlayer`, `CEntity::GetBoundCentre` (lee +0x4), `DoesNotCollideWithFlyers`, `CCamera::IsExtraEntityToIgnore`, `CWorld::CameraToIgnoreThisObject`, `CPedIntelligence::GetTaskHold` (con el argumento), `RegisterReference`/`CleanUpOldReference` (el EXE y la DLL deben registrar la misma dirección de casilla), `AnimatePedColModelSkinnedWorld`, `CTaskManager::FindActiveTaskByType`, `GetCameraAvoidVehicle`, `CWorld::ClearScanCodes` (código de escaneo 0xFFFF), `CalculateTrianglePlanes` (los planos van ya en el fixture, así no se toca la caché real de colisión), `CColStore::Initialise/Shutdown/LoadCollision`, `CStreaming::LoadAllRequestedModels` y `CCollisionData::RemoveTrianglePlanes`.
- Comparación: valor devuelto (AL), salidas por puntero (`nearest`, `dst`, `maxTouchDistance`, `CColPoint` entero relleno con 0xCD, caché de salida), traza ordenada de los parches y estado completo (hash de todos los globales de `CCollision`, la cámara, los sectores, los pools intercambiados y los fixtures; si falla se listan las primeras diferencias). Las cachés de `SphereCastVsEntity`/`SphereCastVsCaches`/`BuildCache` apuntan a buffers propios rellenos con 0xCD y se comparan byte a byte; en `CameraConeCastVsWorldCollision` las cachés son locales (pila) y solo se comparan el resto de salidas.
- `Init` y `Shutdown`: cabecera de `ms_colModelCache` aleatoria (los campos que no se escriben deben conservarse); `Init` normaliza los punteros al bloque reservado (el campo `data` de cada enlace es basura del montón y no se compara) y libera el bloque; `Shutdown` parte de una lista usada falsa (0 a 50 enlaces en orden aleatorio, con `data` nulo o falso) sobre un bloque real de `new[]`, que la función debe liberar y dejar `links = 0`.
- Casos aleatorios por función (semilla fija): esferas con radio 0, esferas y cajas degeneradas, centros fuera del mundo (sectores negativos y > 119), coordenadas de sector negativas para `CheckPeds`/`CheckCameraCollisionPeds`, 99 entradas en la caché, código de escaneo 0xFFFF, entidades repetidas en varios sectores, `pIgnoreEntity` nulo o de cualquier tipo, jugador dentro y fuera de un vehículo, 2 tareas de entrada al coche, radios distintos para `spA` y `spB`.

**Fuera de los fixtures:**
- matriz de entidad nula (`GetMatrix` reserva en el pool real de matrices);
- regiones de `SphereCastVsCaches` sin datos de colisión y entradas sin entidad inicial (el EXE se cae);
- `ResetMadeInvisibleObjects` con contador > 10 (el EXE escribe fuera del array y pisa globales vecinos);
- rango de sectores vacío en `BuildCacheOfCameraCollision` (el EXE devuelve basura de pila);
- `m_vehicleSpecialColIndex` ≥ 0 (colisiones especiales reales);
- índices de triángulo/cara ≥ 0x7FFF y contadores mayores de 32767 (los contadores del EXE son `int16`).

**Comprobaciones al compilar:** el banco avisa con `error`/`layout` si algún offset de los fixtures no coincide con la DLL (esferas, cajas, vértices, triángulos y planos de `CCollisionData`, `CColModel::m_pColData`, `CPedIntelligence::m_TaskMgr`, `CTaskSimpleHoldEntity::m_pEntityToHold`, `CPedModelInfo::m_pHitColModel`, `CCam::m_vecFront/m_vecSource`, `RwCamera::nearPlane`, `m_nVehicleType`, ruedas de `CAutomobile`/`CBike`, y las direcciones de `m_aCams`, `m_nActiveCam`, `Scene.m_pRwCamera`, posición de la cámara y los arrays de sectores). Si `CPhysical::physicalFlags.bDontCollideWithFlyers` no está en el bit 0 del byte +0x43 la comparación lo mostrará. Las funciones de otros bancos que llaman estas (`TestSphereSphere`, `TestLineSphere`, `TestLineBox_DW`, `ProcessLine*`, `PointInPoly`, `ClosestSquaredDistanceBetweenFiniteLines`, `GetBoundingBoxFromTwoSpheres`, `CWorld::GetRepeatSector`) se ejecutan tal cual: si alguna difiere del EXE y su enganche no está activo, el fallo aparecerá aquí.

Repetir: `python reversing/ColWorld59c/build_probe.py` y `python reversing/run.py --case colworld59c`. Evidencia: `validation-summary.json` (se genera al correr).
