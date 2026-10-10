<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Mundo, colisión, grúas y oclusión (lista de 43)

Validadas el 02/10/2026 con **14.912 pares original/candidato dentro de GTA**, todos exactos: 44 funciones de 9 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAudioZones` | `Init`, `RegisterAudioBox`, `RegisterAudioSphere`, `SwitchAudioZone`, `Update` |
| `CColSphere` | `Set`, `IntersectEdge`, `IntersectPoint`, `IntersectRay`, `IntersectSphere` |
| `CCranes` | `InitCranes`, `IsThisCarBeingCarriedByAnyCrane`, `IsThisCarBeingTargettedByAnyCrane`, `IsThisCarPickedUp`, `UpdateCranes` |
| `CEscalator` | Constructor, Destructor, `AddThisOne`, `SwitchOff`, `Update` |
| `CGridRef` | `Init`, `GetAreaName`, `GetArtistBugstarID`, `GetGridRefPositions` (las dos) |
| `CHudColours` | Constructor, `GetIntColour`, `GetRGB`, `GetRGBA`, `SetRGBAValue` |
| `COcclusion` | `Init`, `AddOne`, `IsPositionOccluded`, `OccluderHidesBehind`, `ProcessBeforeRendering` |
| `CSetPieces` | `Init`, `AddOne`, `Load`, `Save`, `Update` |
| `CScripted2dEffects` | `AddScripted2DEffect`, `GetEffectPairs`, `GetIndex`, `ReturnScripted2DEffect` |

**Diferencias reales corregidas:**
- `CAudioZones`:
  - `RegisterAudioBox`/`RegisterAudioSphere` escriben en el sitio (`strcpy` sin rellenar el nombre, solo el bit 0 de las banderas; coordenadas de caja como `(int16)_ftol2(v * 8)`);
  - `SwitchAudioZone` usa contadores con signo releídos en cada vuelta;
  - `Update`:
    - la última posición es un global del EXE (0xB6EBFC);
    - distancias como `sqrt(dz² + dy² + dx²)` (con NaN se actualiza);
    - esfera estricta y caja abierta (`min * 0,125 < p < max * 0,125`).
- `CColSphere`: todas las sumas siguen el orden z, x, y del EXE; `IntersectRay` usa `b² - c*4` (sin el término a); `IntersectPoint` e `IntersectSphere` son estrictas.
- `CEscalator`:
  - el constructor (0x717110) solo escribe la palabra tras `m_pAttachMatrix` (0x74), `m_StepObjectsCreated` y los 42 peldaños; el destructor (0x717130) solo destruye la matriz;
  - `SwitchOff` pone la bandera de depuración del EXE alrededor de cada borrado y solo quita los peldaños no nulos.
  - `AddThisOne`:
    - número de peldaños con `_ftol2` (0x717A14) y sumas z, y, x;
    - dirección con los argumentos sin rebajar y `CVector::Normalise`;
    - entidad como puntero crudo con `RegisterReference`, sin limpiar la anterior.
  - `Update`:
    - radio + 20 estricto contra la cámara y huecos libres del pool con signo;
    - fracción del paso restando el valor truncado, no `floor`;
    - matriz copiada sobre `m_matrix`, fórmulas de posición de cada tramo del EXE y radio + 23 estricto al salir de la vista.
- `CGridRef`:
  - `Init` conserva x/y entre líneas y no comprueba nada;
  - `GetArtistBugstarID` devuelve la cadena vacía del EXE (0x858B54);
  - `GetGridRefPositions` calcula `(p + 3000) * 1/600` en float (0x3ADA740E) con `_ftol2` y llama dos veces a `FindPlayerCoors`.
- `CHudColours`: `SetRGBAValue` no comprueba el índice; `GetIntColour` es `(g << 16 | 0xFF) + (b << 8 | 0xFF) + (r << 24 | 0xFF)`.
- `COcclusion`:
  - `AddOne`:
    - tamaños truncados con `_ftol2` (se descarta con al menos dos a cero);
    - ángulos en [0, 360] a radianes con 0x3C8EFA35 y a byte con 0x4222F983;
    - tablas acotadas (1000 / 40) y contadores corregidos para interiores y lista lejana;
  - `OccluderHidesBehind` lee los contadores de líneas con signo (NaN pasa la prueba);
  - `IsPositionOccluded` toma `scaleX` solo si es estrictamente mayor;
  - `ProcessBeforeRendering` recorre siempre la lista lejana y después la cercana, solo procesa interiores si no se ve el exterior y borra en el acto desplazando el resto (`rep movs`).
- `CSetPieces`:
  - `AddOne` escribe en el sitio con contador con signo, `min`/`max` del EXE (con NaN o empate gana b) y `_ftol2(v * 4)`;
  - `Update` usa divisiones con signo y recalcula el final con `NumSetPieces` en cada vuelta.
- `CScripted2dEffects`:
  - `ReturnScripted2DEffect` y `GetEffectPairs` no comprueban índice ni tipo;
  - `GetIndex` solo acepta la dirección exacta de cada efecto;
  - `AddScripted2DEffect` devuelve -1 sin hueco libre y reinicia los pares como 0x6F9EB0.

**Método:**
- Objetos falsos o las tablas globales del EXE (zonas de audio, cuadrículas, oclusores, set pieces y efectos 2D de script), que se guardan, se rellenan y se restauran en cada ejecución.
- Simulados y trazados:
  - `CWorld::Add`/`Remove` y la creación de `CObject` (nunca se crea ni se destruye uno real); el destructor virtual de los peldaños (vtable falsa);
  - `CEntity::UpdateRwFrame`, `CPhysical::RemoveAndAdd`, `CEntity::RegisterReference`, `CalcScreenCoors`, `COccluder::NearCamera`/`ProcessOneOccluder`, `CSetPiece::Update`;
  - `CGenericGameStorage::Load`/`SaveDataToWorkBuffer`, `GetPedAttractorManager`/`RemoveEffect`, `CFileMgr` y `CFileLoader::LoadLine` (nunca se lee ningún fichero) y `FindPlayerCoors`.
- El pool de objetos y el modelo `MI_ESCALATORSTEP` se sustituyen por falsos durante la llamada.
- Todo el estado global tocado se guarda y se restaura en cada ejecución y al final.

Repetir: `python reversing/World43/build_probe.py` y `python reversing/run.py --case world43`. Evidencia: [validation-summary.json](validation-summary.json).
