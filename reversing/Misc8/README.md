<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Transmisión, cañones de agua, FX y superficies (lista de 8)

Validadas el 02/10/2026 con **11.712 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `cTransmission` | `InitGearRatios`, `CalculateGearForSimpleCar`, `DisplayGearRatios`, `CalculateDriveAcceleration` |
| `CWaterCannons` | `Init`, `UpdateOne`, `Update`, `Render` |
| `FxInfoManager_c` | `AddFxInfo`, `ProcessEmissionInfo`, `ProcessMovementInfo`, `ProcessRenderInfo` |
| `FxSphere_c` | `operator new`, `operator delete`, `IsCollision`, `GetDistToPlane` |
| `ProcSurfaceInfo_c` | `Init`, `Exit`, `AddObject`, `AddObjects` |

**Diferencias reales corregidas:**
- `cTransmission`:
  - `DisplayGearRatios` usa el factor 1 / (0,277778 / 50) de la global 0xC1CB54 (iniciada en 0x853CE0) e imprime siempre las 6 marchas con el `printf` del CRT del juego (0x821982);
  - `InitGearRatios` multiplica por el inverso 1 / número de marchas en vez de dividir;
  - `CalculateGearForSimpleCar` sube a `min(marcha + 1, numMarchas)` aunque la marcha ya supere el máximo;
  - `CalculateDriveAcceleration` sigue las comparaciones del EXE (con NaN cae del mismo lado), baja la marcha dentro del bucle, corrige las ramas de marcha atrás y primera, calcula el tercio como (1 - 0,6667) y lee `*a7` antes de escribirlo (y lo guarda antes que `*a6`).
- `CWaterCannons`: `UpdateOne` tiene `Update_NewInput` en línea y lee el número de secciones como `int16`; `Render` solo dibuja los cañones activos.
- `FxInfoManager_c::AddFxInfo`:
  - cada FxInfo ocupa 0x18 bytes de la arena FX (`g_fxMan`, alineación 4) y sin memoria no se ejecuta el constructor;
  - los casos NOTSA (rotación, coordenadas de textura y desplazamiento de rotación) se quitan.
- Constructores de `FxInfoFloat_c`, `FxInfoHeatHaze_c`, `FxInfoSelfLit_c` y `FxInfoUnderwater_c`: solo escriben el byte bajo del contador de interpolación (+0xE) y no piden memoria al pool.
- `FxSphere_c`: `IsCollision` suma la distancia al cuadrado en el orden z, y, x; `GetDistToPlane` suma (y + x) + z antes de restar la distancia.
- `ProcSurfaceInfo_c`:
  - `Init` devuelve `bool` y limita la distancia mínima a 80 como **máximo** (con NaN se conserva).
  - `AddObject`:
    - aleatorios con el `rand()` del juego escalado por 1/32767 como min + (max - min) * r;
    - `fcos`/`fsin` x87, `CrossProduct` (0x59C730) y `Normalise` (0x59C910) en el sitio;
    - el escalado solo se aplica a edificios cuando cualquiera de las dos escalas es distinta de 1 (0x5A2E60, 0x5A3803), con los productos en el x87 (conserva los denormales que SSE vacía a cero).
  - `AddObjects`:
    - centro como (v1 + v2 + v3) * 0,3333 y distancias en el orden z, y, x;
    - mínimos y máximos con las comparaciones del EXE;
    - cada columna vuelve a empezar en la primera fila;
    - `IsPtInTriangle2D` rechaza el punto si la expresión no es <= 0 (también con NaN).

**Método:**
- Objetos falsos con fixtures deterministas; nunca se crea un `CObject`/`CBuilding` real (se simulan `operator new` y el constructor).
- Simulados y trazados:
  - `rand`/`srand` (EXE, CRT de la DLL y `CGeneral::GetRandomNumber`) y el `printf` del CRT del juego (0x821982);
  - `CWaterCannon::Init`/`Update_NewInput`/`Update_OncePerFrame`/`Render` y `CAEWaterCannonAudioEntity::Service` (solo se escriben los datos de `aCannons`, nunca la entidad de audio);
  - `FxMemoryPool_c::GetMem` (arena falsa), `CMemoryMgr::Malloc`/`Free` (solo durante la llamada) y `GetValue` de FxInfo falsos;
  - métodos de `CPlaceable`/`CEntity`, `CWorld::Add`/`Remove`, `UpdateRwMatrix`, `SurfaceInfos_c::GetSurfaceIdFromName`, `CModelInfo::GetModelInfo` por nombre y `ProcSurfaceInfo_c::AddObject` dentro de `AddObjects`.
- Todo el estado global tocado se guarda y se restaura.

Repetir: `python reversing/Misc8/build_probe.py` y `python reversing/run.py --case misc8`. Evidencia: [validation-summary.json](validation-summary.json).
