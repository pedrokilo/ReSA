<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CObject (lista 34, 48 funciones)

Banco preparado el 04/10/2026: **18.368 pares original/candidato dentro de GTA** sobre las 48 funciones de `CObject` (38 por dirección del EXE y 10 por la vtable del EXE 0x866F60). Pendiente de compilar y ejecutar.

| Grupo | Funciones |
|---|---|
| Estado y banderas | `CanBeDeleted`, `CanBeTargetted`, `SetObjectTargettable`, `TryToExplode`, `RefModelInfo`, `SetRemapTexture`, `SetRelatedDummy`, `SetIsStatic` (VMT), `Init` |
| Cuerdas | `GetRopeHeight`, `SetRopeHeight`, `GetObjectCarriedWithRope`, `ReleaseObjectCarriedWithRope`, `GrabObjectToCarryWithRope` |
| Lista de control | `AddToControlCodeList`, `RemoveFromControlCodeList` |
| Puertas y lógica | `ResetDoorAngle`, `LockDoor`, `ProcessSamSiteBehaviour`, `ProcessTrainCrossingBehaviour`, `SetMatrixForTrainCrossing`, `ProcessGarageDoorBehaviour`, `ProcessControlLogic`, `ProcessControl` (VMT) |
| Daño | `ObjectDamage`, `Explode`, `ObjectFireDamage`, `DoBurnEffect` |
| Render | `PreRender`, `Render`, `SetupLighting`, `RemoveLighting`, `CreateRwObject` (VMT), `GetLightingFromCollisionBelow` |
| Colisión | `SpecialEntityPreCollisionStuff`, `SpecialEntityCalcCollisionSteps`, `Teleport` (VMT), `CanBeUsedToTakeCoverBehind` |
| Pool y partidas | `TryToFreeUpTempObjects`, `DeleteAllTempObjects`, `DeleteAllMissionObjects`, `DeleteAllTempObjectsInArea`, `Create` (modelo y dummy), `IsObjectPointerValid`, `IsObjectPointerValid_NotInWorld`, `Save`, `Load` |

**Diferencias reales corregidas** (`source/game_sa/Entity/Object/Object.cpp`):
- `Init`:
  - `bCanBeAttachedToMagnet` (bit 18) queda siempre activo (`(flags & 0xFC040000) | 0x40000`);
  - el centro de masas de los modelos que se balancean se ajusta aunque el modelo no tenga volúmenes de colisión;
  - solo se ponen a cero los dos primeros colores de coche.
- `SpecialEntityPreCollisionStuff`:
  - una farola caída solo trata vehículos y peatones; con el vehículo guarda la entidad ignorada (estaba invertido con el peatón) y con otras entidades sigue con las comprobaciones generales;
  - con vehículos solo se miran objetos cuya caja acaba por debajo de 1 en x, y y z;
  - la granada por debajo del peatón desactiva la colisión;
  - `Invert` sobre `m_matrix` (sin `GetMatrix`, que reservaba matriz) y productos con el orden de 0x59C890.
- `SpecialEntityCalcCollisionSteps`: arista **menor** de la caja × `fObjectStepSizeMult` (0x8D0A28), farola con los dos `Multiply3x3` antes de los dos `GetSpeed`, magnitud z+y+x en masa infinita, `(|v|²·paso)·paso`, raíz × 6.6666665 / 3.3333333 en vez de dividir, NaN devuelve 1.
- `ProcessControl`:
  - el volquete no necesita la horquilla levantada;
  - `pow(0.97, paso)` y `pow(1 − |v|²·0.2, paso)` con `_CIpow` del EXE (el del RC Bomb tenía los argumentos cambiados) y fuerza del RC Bomb multiplicada por la masa de giro;
  - puertas: rumbo de 80 bits con `fpatan`, PI exactas y los límites globales 0x8D0A2C..0x8D0A34, y se comprueba `m_bIsStuck` (no `m_bIsBIGBuilding`);
  - NaN reinicia el contador de física falsa.
- `PreRender`: brillo de contacto en línea con 1/30 (0x858F10) y `(día·k)·(1−b) + (noche·k)·b`.
- `ProcessSamSiteBehaviour`: `FindPlayerVehicle` repetido como el EXE, `(z − 2) > z` del objetivo, paso × 0.05 y × 0.005, `LimitRadianAngle` en línea con la diferencia calculada sobre el ángulo de ST0, `SetRotateZOnly` directo, NaN como el EXE.
- `ProcessTrainCrossingBehaviour`: `_CIacos` del EXE con ST0 completo, paso × 0.005 y límite 0x866FC8 (no `PI*0.43` con la PI imprecisa).
- `SetMatrixForTrainCrossing`: `fcos`/`fsin` completos y `0·sen + x·cos` (signo de los ceros).
- `ObjectDamage`, `ObjectFireDamage`: la salud solo se recorta si es < 0; las destrucciones escriben el bit estático sin `SetIsStatic`; con el objeto destruido `PLAY_ON_HIT` no reproduce efecto; el punto del efecto es `Multiply3x3 + posición` (se sumaba la posición dos veces); quemado `1 − salud·0.0005`.
- `Explode`: `((rand & 0xFF) − 128) · 0.0002` (no `GetRandomNumberInRange`), z antes que x/y y punto del efecto sin la posición doble.
- `GrabObjectToCarryWithRope`: el objeto recogido va a +0x318 y el gancho (+0x314) se coloca en `attachTo · (0, 0, altura)` (multiplicaba componente a componente); `GetObjectCarriedWithRope` devuelve +0x318.
- `ProcessControlLogic`: `_ftol2` de 32 bits para los segmentos, productos de 0x59C890, `0 > x` elige la bola de demolición.
- `ProcessGarageDoorBehaviour`: × 0x8599A0 en vez de `/ 1.1`.
- `TryToFreeUpTempObjects`: del último hueco al primero y para al llegar a cero. `DeleteAllTempObjectsInArea`: suma z, y, x.
- `CanBeUsedToTakeCoverBehind`: altura < 1.25, > 0.75 y up.z > 0.9 (NaN no pasa).
- `IsObjectPointerValid*`: índice `(p − base) / 0x19C` con división con signo, como el EXE.
- `SetRemapTexture` recibe un `int32` (el EXE compara el dword entero con −1). `DoBurnEffect`: × 0.05 y `_ftol2`.

**Método:**
- Pool de objetos falso de 8 huecos (puntero 0xB7449C cambiado solo durante cada ejecución; huecos libres con 0xCD; `m_LastFreeSlot` fijado por caso).
- Objetos falsos con offsets crudos y la vptr del EXE; vehículo, peatón, dummy, model infos con vtables del EXE, colisiones, `CObjectData`, RwObject/RwFrame propios. Model infos falsos en `ms_modelInfoPtrs` solo durante la ejecución.
- `Create` usa una `CMatrixLinkList` falsa (centinelas de gMatrixList guardados) y compara el pool entero (vptr por tipo).
- La lista de control usa nodos reales del pool de nodos dobles (guardado y restaurado entero).
- Se simulan y trazan las dependencias externas listadas en la cabecera de `LiveComparison.hpp` (dirección del EXE + símbolo y cuerpo de la DLL).
- Fuera de los fixtures:
  - modelo 0xFFFF en `Init` (la DLL indexa `ms_modelInfoPtrs[0xFFFF]` antes de comprobarlo);
  - `PreRender` con escala y sin RwObject (el EXE lo desreferencia);
  - RwObject en el dummy de `Create` (`AttachToRwObject` real).

Repetir: `python reversing/Object48/build_probe.py` y `python reversing/run.py --case object48`.
