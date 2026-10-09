<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# WorldFinal1: banco por pares de CRope y CMonsterTruck (funciones pendientes)

Banco simple (flujo viejo, copiado de `Small2`/`PadMouse1`/`VehCtor62`) para las 10 funciones pendientes de `CRope` y `CMonsterTruck`. **Compilado y ejecutado; aprobado en la corrida 20261009-005433-453152 (10368 pares, 0 diferencias). Registro: `register_world_final1.py`, `validation-summary.json` y `runs/`. Antes de la corrida se corrigieron `Rope.cpp` y `MonsterTruck.cpp` a lo que hace el EXE (ver el resumen).**

Uso: `bash reversing/_handoff/bench.sh WorldFinal1 3`. Filtro opcional: `worldfinal1-only.txt` en la carpeta del juego con `Clase::Funcion`.

## Corpus: 10368 pares

| Funcion | EXE / hueco VMT | Pares |
|---|---|---|
| `CRope::CreateHookObjectForRope` | 0x556070 | 768 |
| `CRope::PickUpObject` | 0x5569C0 | 1536 |
| `CRope::ReleasePickedUpObject` | 0x556030 | 384 |
| `CRope::Remove` | 0x556780 | 640 |
| `CRope::Render` | 0x556800 | 1536 |
| `CMonsterTruck::Constructor` | 0x6C8D60 | 512 |
| `CMonsterTruck::BurstTyre` | vtable 0x8717D8, hueco 44 (0x6C7D30) | 256 |
| `CMonsterTruck::PreRender` | hueco 17 (0x6C7DE0) | 1536 |
| `CMonsterTruck::ProcessEntityCollision` | hueco 22 (0x6C8AE0) | 3072 |
| `CMonsterTruck::SetUpWheelColModel` | hueco 43 (0x6C7D20) | 128 |

Los huecos se han verificado volcando la vtable del EXE (`rd.py` de 71 entradas en 0x8717D8): 17=0x6C7DE0, 22=0x6C8AE0, 43=0x6C7D20, 44=0x6C7D30. `RH_ScopedVirtualClass(CMonsterTruck, 0x8717d8, 71)` es correcto.

## Qué se compara (original = hook desactivado, candidato = hook activo)

Objetos falsos con guardas de 32 bytes (cuerda de 0x328 bytes, camión de 0x99C bytes, 5 entidades de 0xA40 bytes con vtable falsa, 6 matrices, 3 model infos/colmodels/datos de colisión falsos). Se compara el contenido completo (cuerda y camión byte a byte, resto por hash), la guarda, el retorno, el control x87 y la traza de llamadas salientes con argumentos. Cada leg reconstruye el mismo fixture desde la misma semilla.

Colaboradores simulados y trazados (parche en la dirección del EXE y en el cuerpo de la DLL, solo durante cada ejecución): `CEntity::RegisterReference` (0x571B70), `CPhysical::AddToMovingList` (0x542800), `CWorld::Add/Remove` (0x563220/0x563280), `CObject::operator new` (0x5A1EE0), constructor de `CObject(int32,bool)` (0x5A1D70; el de la DLL se localiza por su símbolo exportado `??0CObject@@QAE@H_N@Z`), `DefinedState` (0x734650), `CCamera::IsSphereVisible` de 3 argumentos (0x420C40), `RwRenderStateSet` (el puntero de función de RwGlobals+0x20 se sustituye temporalmente), `RwIm3DTransform/RenderIndexedPrimitive/End` (0x7EF450/0x7EF550/0x7EF520), `CAutomobile::CAutomobile` (0x6B0A90; constructor de la DLL = primer llamado de la DLL dentro del constructor de CMonsterTruck, contrastado con el del envoltorio de CAutomobile, como en `VehCtor62`), `CMonsterTruck::SetupSuspensionLines` (0x6C7FB0 y cuerpo de la DLL), `CAutomobile::PreRender` (0x6AAB50), `CVehicleModelInfo::GetWheelPosn` (0x4C7D20), `CVehicle::SetTransmissionRotation` (0x6DBBB0) y `SetComponentRotation` (0x6DBA30), `CCollision::ProcessColModels` (0x4185C0) y `CPhysical::AddCollisionRecord` (0x543490). La entrada de `CBaseModelInfo` de las ranuras 19990..19992 y 406 se sustituye por modelos falsos y se restaura.

Todo el estado global tocado (ranuras de model info, `m_bMirrorActive` 0xB6F998, `TempBufferVertices` 0xC4D958, puntero de RwGlobals) se guarda y se restaura (`state_restored`). No se asigna memoria, no se toca ningún pool ni fichero, y no se usa `operator new/delete` real (el `new` de `CObject` se sustituye por un búfer fijo). Los stubs solo trazan en el hilo del banco (`owner`/`Mine()`).

## Dominio y exclusiones documentadas

- Entidades con matriz siempre no nula (con matriz nula `GetMatrix()` reservaría una matriz del pool). Sin NaN señalizantes: los NaN de los fixtures son silenciosos; los flotantes que se copian con x87 son siempre finitos.
- `CreateHookObjectForRope` con `m_pAttachedEntity != null` solo se prueba como salida anticipada; `Remove` y `ReleasePickedUpObject` no se prueban con `m_pAttachedEntity` nulo y objeto enganchado (el EXE lo desreferencia). Pool de `CObject` lleno (operator new devuelve nulo) queda fuera: el EXE y la DLL desreferencian nulo.
- `ProcessColModels` y todos los colaboradores son dobles: se valida solo la parte propia de cada función.
