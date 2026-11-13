<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 901 grupos / 10540 funciones reconstruidas; 10540 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn89C: `CTaskSimpleGangDriveBy::FireGun` (lote 89C)

Estado: **APROBADO dentro del juego** (03/11/2026, worktree `wh-1`, rama de trabajo wip-4-reconstrucciones + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn89c/20261103-232144-428069` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Banco par a par dentro del juego, 16384 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`).
Una funcion de codigo de juego reconstruida en C++ puro y enganchada (tipo S); `ProcessPed` (0x62D3B0) y `CCam::Process_Cam_TwoPlayer_InCarAndShooting` (`Cam.cpp`) ya no la llaman por direccion.

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `CTaskSimpleGangDriveBy::FireGun` (antes `plugin::CallMethod<0x627CC0>`) | 0x627CC0 | `CTaskSimpleGangDriveBy` / `FireGun` / S | `TaskSimpleGangDriveBy.cpp` |

thiscall con un argumento (`CPed*`), `ret 4`, `void`; sin simbolo en el EXE (el nombre es propio). Dispara el arma activa del ped desde un coche (tiroteo desde el vehiculo).

## Revision contra el EXE (0x627CC0..0x62833E)
- Origen: `CWeaponInfo +0x24` transformado con la matriz del hueso de la mano derecha (`GetAnimHierarchyFromSkinClump` 0x734A40, `RpHAnimIDGetIndex` 0x7C51A0 con `BoneTag` en +0x14 del nodo `ped+0x4A0`, `RpHAnimHierarchyGetMatrixArray` 0x7C5120, `RwV3dTransformPoints` 0x7EDD90): matriz = array + indice * 0x40 sin comprobar el rango.
- Relativo al coche: origen - posicion del vehiculo (matriz +0x30), caja delimitadora = `ms_modelInfoPtrs[(int16)modelo]->+0x14` (`CColModel`, minimo en +0, maximo en +0xC).
- Jugador (`CPed::IsPlayer` 0x5DF8F0: `m_nPedType` 0 o 1): camara activa (byte 0xB6F081, 0x238 por camara desde 0xB6F19C: modo en +0xC, origen en +0x19C): modo 0x31 = rango `|pos - origen| + 20.0f`, 0x37 = 20.0f, otro modo sale sin disparar;
  `Find3rdPersonCamTargetVector` (0x514970) da el objetivo; `m_fAttackButtonCounter` (datos del jugador +0x2C) se pone a 0. IA: objetivo = matriz de la entidad * coordenadas (0x59C890) o las coordenadas; la direccion es objetivo - origen, normalizada con 0x59C910.
- Cara del coche segun la animacion actual (`m_AnimId`, word en +0x2C de la asociacion: E7/EC izquierda, EB/E8 derecha, EA/EE atras, otra adelante) o `m_nFakeShootDirn` (1, 3, 2). Si `d . eje` (derecho para izquierda/derecha, adelante para atras/adelante; negado en izquierda y atras) no supera 0.1f el
  punto de impacto es el propio origen; si no, `t = ((cara - q . eje) -/+ 0.2f) / (d . eje)` y el impacto es `p + t * d` (izquierda/derecha con el producto x en el registro, atras/adelante con `0x40FE90` y `0x40FE30`).
- `ped+0x528 |= 1` (banderas del IK, bGunReachedTarget); `CWeapon::Fire` (0x742300) sobre la ranura activa (byte con signo +0x718, 0x1C por arma) con el objetivo nulo si las tres coordenadas son iguales a cero (ordenado) y con `m_vecCoords` (o su transformacion por la matriz de la entidad) si no;
  `CPed::DoGunFlash` (0x5DF340) con la duracion del word 0x8D2E90 (250).
- Los offsets deducidos por el autor (`m_fAttackButtonCounter` +0x2C, `m_pedIK.m_nFlags` +0x528, `BoneTag` +0x14) quedan contrastados por el banco (cambios de bytes de las arenas) y por `static_assert` en `LiveComparison.hpp`.

## Banco
Falsos identicos en la direccion del EXE (y en el cuerpo de la DLL cuando el colaborador tiene hook RH, apagado durante el banco): `GetAnimHierarchyFromSkinClump` 0x734A40 (hook RH), `Find3rdPersonCamTargetVector` 0x514970, `CWeapon::Fire` 0x742300 y `CPed::DoGunFlash` 0x5DF340 (hooks RH);
`RpHAnimIDGetIndex` 0x7C51A0, `RpHAnimHierarchyGetMatrixArray` 0x7C5120 y `RwV3dTransformPoints` 0x7EDD90 (la DLL los llama por su direccion del EXE). Registran sus argumentos EN ORDEN con el contenido de los vectores.
NO se sustituyen (corren de verdad en ambos lados, x87 del EXE frente a los ayudantes x87 de 24 bits de la DLL): `CPed::IsPlayer`, `CVector::Normalise` 0x59C910, `CMatrix * CVector` 0x59C890, 0x40FE90 y 0x40FE30 y toda la aritmetica de la funcion.
Memoria real del juego (se rellena y se restaura tras cada lado): byte de la camara activa y las tres camaras (modo con basura en los 16 bits altos, origen) y 12 entradas libres de `ms_modelInfoPtrs` (indices 19980..19991, apuntan a dos modelos con cajas distintas).
Se observa: los eventos de los falsos con todos sus argumentos, los bytes cambiados de todas las arenas (ped, datos del jugador, tarea, vehiculo...) y los punteros de la tarea.
16 familias de 1024 casos: aleatorio, jugador con camara 0x37, jugador con camara 0x31, jugador con otro modo, IA con entidad, IA sin entidad, coordenadas nulas (con -0, NaN, una distinta de cero), cuatro familias de cara (izquierda, derecha, atras, adelante, por animacion o por `m_nFakeShootDirn`),
mutaciones (una escritura dentro de un falso: entidad, modelo del vehiculo, coords.x, asociacion), floats especiales (NaN, infinitos, desnormales), vector nulo y casi nulo, umbral 0.1f del producto con el eje (eje exacto, direccion con x = +-0.1f +- ulps), ABI con registros sembrados.

## Fuera de los fixtures
Entidad objetivo sin matriz (el EXE llama a 0x54F560 + 0x54F1B0; la DLL a `AllocateMatrix` + `UpdateMatrix`, que no se pueden falsear en ambos lados), vehiculo sin matriz (el EXE lo desreferencia), ranura de arma o indice de modelo fuera de rango, ped sin datos de jugador en los modos 0x31/0x37,
NaN con cargas distintas de 0xFFC00000, NaN senalizantes, y los valores de retorno de los falsos (el EXE no los lee). La exactitud de `CWeapon::Fire`, `DoGunFlash` y `Find3rdPersonCamTargetVector` se hereda: no se validan aqui.

## Bancos repetidos
`DriveByPending` (`ProcessPed` llama ahora a `FireGun` de la DLL: mismo falso en la entrada de la DLL) y `CamShooting355` (`Process_Cam_TwoPlayer_InCarAndShooting` llama ahora a `FireGun`: mismo falso tambien en la entrada de la DLL).

## Prueba negativa
Produccion mutada en cuatro puntos (`TaskSimpleGangDriveBy.cpp`: AnimId 0xE7 -> 0xE6, `m_nFakeShootDirn == 2` -> 4, rango 20.0f -> 21.0f; `X87Math.cpp`: margen 0.2f 0x3E4CCCCD -> 0x3E4CCCCC): 2623 de 16384 pares con diferencias (`resultados-fn89c`, 03/11/2026). Una primera mutacion de 6 puntos (banderas del IK, contador de ataque, mano del fogonazo, ...) dio 14969 de 16384. Restaurado con `cmp` contra la copia buena.

## Cam.cpp
`Process_Cam_TwoPlayer_InCarAndShooting` llama ahora a `task.FireGun(shooter)` en lugar de `plugin::CallMethod<0x627CC0>`.
