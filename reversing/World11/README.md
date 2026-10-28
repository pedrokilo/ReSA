<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Coronas, entradas/salidas, reaparición y manejo de vehículos (lista de 11)

Validadas el 02/10/2026 con **16.704 pares original/candidato dentro de GTA**, todos exactos: 44 funciones de 4 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CCoronas` | `Init`, `Shutdown`, `Update`, `Render`, `RenderReflections`, `RenderOutGeometryBufferForReflections`, `RenderSunReflection`, `RegisterCorona` (textura y tipo), `UpdateCoronaCoors`, `DoSunAndMoon` |
| `CEntryExit` | `FindValidTeleportPoint`, `GenerateAmbientPeds`, `GetEntryExitToDisplayNameOf`, `GetPositionRelativeToOutsideWorld`, `IsInArea`, `ProcessStealableObjects`, `RequestAmbientPeds`, `RequestObjectsInFrustum`, `TransitionFinished`, `TransitionStarted`, `WarpGangWithPlayer` |
| `CRestart` | `Initialise`, `AddHospitalRestartPoint`, `AddPoliceRestartPoint`, `OverrideNextRestart`, `CancelOverrideRestart`, `SetRespawnPointForDurationOfMission`, `ClearRespawnPointForDurationOfMission`, `FindClosestHospitalRestartPoint`, `FindClosestPoliceRestartPoint`, `Load`, `Save` |
| `cHandlingDataMgr` | `LoadHandlingData`, `ConvertDataToGameUnits`, `ConvertDataToWorldUnits`, `ConvertBikeDataToGameUnits`, `ConvertBikeDataToWorldUnits`, `FindExactWord`, `GetHandlingId`, `GetBoatPointer`, `GetFlyingPointer`, `HasFrontWheelDrive`, `HasRearWheelDrive` |

**Diferencias reales corregidas:**
- `CCoronas`:
  - `Init` guarda el TXD antes de buscar "particle", no lee texturas sin nombre y solo borra el identificador de cada corona y `NumCoronas`;
  - `Update` usa el global `LastCamLook` del EXE (0xC3EF58);
  - `Render` reescrito según el EXE:
    - posición con la matriz de inclinación de las motos;
    - fuera de pantalla con x/y frente al raster;
    - atenuación `1 - (z - mitad) / mitad` y `!(z < far)`;
    - escala `min(z, 40) * niebla * 0,025 + 1` y tamaño `escala * tamaño * alto`;
    - destellos con las tablas crudas del EXE, `rand() * (1/32767) * 0,3 + 0,7`, mitades enteras del raster y la intensidad de los faros como `(raw * int16) >> 8`;
  - `RenderReflections`:
    - llama a `InitSpriteBuffer`;
    - salta coronas apagadas o sin reflejo;
    - busca el suelo con `ProcessVerticalLine` (-1000) cada 16 fotogramas `(frame + i) & 15`;
    - la altura se compara con 20 (antes se comparaba el bool);
    - lejano `min(far * 0,75, 55)`;
    - canales `(c * int16(I)) >> 8`;
  - `RenderSunReflection` reescrito: constante 3,333 (0x40554FDF), cuarta zona con radio 350, factor `(d - r) * 0,01`, ángulo `(t & 2047) * 0x3B490FDB` con `fsin`, vértices escritos en el orden de sumas del EXE;
  - `RegisterCorona`:
    - distancia 3D z, y, x;
    - alfa de neón `(d - 35) * alfa * (1/15)`;
    - se libera mirando la intensidad atenuada;
    - no crea coronas con alfa 0;
    - `RegisterReference` sin limpiar la anterior; el tipo se lee como byte;
  - `UpdateCoronaCoors` sigue con NaN; contadores del buffer temporal como dwords.
- `CEntryExit` (9 de 11):
  - `IsInArea`: centro `(max - min) * 0,5 + min`, rotación `fsin`/`fcos` con el orden de 0x59C890, z sin restar y comparaciones que solo fallan ordenadas;
  - `GetEntryExitToDisplayNameOf`: devuelve `this` con área 0, `posn` con signo y `stack[posn - 2]`;
  - `GetPositionRelativeToOutsideWorld`: `(out + pos) - salida` y envoltorio desnudo que conserva `edx` (0x43F150 lo usa como contador);
  - `FindValidTeleportPoint`: 2π exacto y `fcos`/`fsin` × 1,25 o duplicados antes de redondear;
  - `RequestAmbientPeds` (condición de área invertida, `_stricmp == 0`), `RequestObjectsInFrustum` (bandera 0x20), `GenerateAmbientPeds` (`movsx` del número de peatones), `TransitionFinished` (`movzx` del área), `TransitionStarted` (`Magnitude() > 10` y cámara fija sin puerta);
  - `WarpGangWithPlayer`: desplazamientos alrededor del jugador, tres intentos por miembro y salida si fallan, rumbo `LimitRadianAngle(GetRadianAngleBetweenPoints)`.
- `CRestart`: `FindClosestHospitalRestartPoint` con el radio extra solo positivo (NaN no), la estadística que solo descarta si es menor, distancias z, y, x y sin escribir nada si no hay punto (comparaba `>= 0u`); `FindClosestPolicePoint` sin el `NOTSA_UNREACHABLE`.
- `cHandlingDataMgr`:
  - `LoadHandlingData` reescrito con `strtok`/`atof`/`atoi` por campo como el EXE (registros por índice sin acotar, campo 15 de barco en +0x20, grupo de animación en locales, `atof * 0,4` en double);
  - conversiones con los globales 0xC2B9B4/0xC2B9BC, inverso multiplicado, bucle con `0,01` y `1/6` en double, NaN que sale, RC Bandit y límite de velocidad con la velocidad sin reducir (× 0,8333333);
  - motos con `fsin` y el `asin` del CRT del juego;
  - `FindExactWord` (do-while con signo, devuelve `entryCount`) y `GetHandlingId` (`strncmp` de 14, devuelve 210).

**Método:**
- Array real de coronas, puntos de reaparición, estáticos de entradas/salidas, grupos de animación de vehículos, buffer temporal, cámara, tiempo, clima y ciclo horario guardados, rellenados y restaurados en cada ejecución; gestor de manejo, entradas/salidas, peatones, vehículo, entidades, matrices, texturas, raster y cámara RW falsos.
- Simulados y trazados:
  - `RwRenderStateSet` (puntero del motor), `CSprite` (proyección, sprites y buffer), `RwIm3D*`, `RwTextureRead/Destroy`, `CTxdStore`, `LittleTest`, `ProcessLineOfSight`, `ProcessVerticalLine`, `CRegisteredCorona::Update`, `GetLookDirection` y `rand`;
  - `GetLevelFromPosition`, `GetStatValue` (con NaN), `Save/LoadDataToWorkBuffer`, `CFileMgr`, `CFileLoader::LoadLine` (nunca se lee un fichero), `InitGearRatios`, `InitAnimGroup`/`CopyAnimGroup`;
  - streaming, población, interiores, `TestSphereAgainstWorld`/`GetIsLineOfSightClear`, grupos de peatones, teletransporte, `FlushImmediately`, fundidos, texto y HUD.
- `GetPositionRelativeToOutsideWorld` se llama con un centinela en `edx` y se comprueba que vuelve intacto.
- `CMatrix::SetRotateZ`/`SetRotateZOnly` se desactivan durante el banco para que el EXE use su propio `fsin`/`fcos`.
- `TransitionStarted` llega hasta `CanPlayerStartMission` (siempre falso) y `TransitionFinished` solo recorre los estados que vuelven con falso antes de cargar la escena: el resto crea tareas, eventos y carga el mundo real.

Repetir: `python reversing/World11/build_probe.py` y `python reversing/run.py --case world11`. Evidencia: [validation-summary.json](validation-summary.json).
