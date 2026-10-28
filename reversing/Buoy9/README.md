<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Flotabilidad cBuoyancy (lista de 9)

Validadas el 03/10/2026 con **6.656 pares original/candidato dentro de GTA**, todos exactos: 9 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `cBuoyancy` | `FindWaterLevel`, `FindWaterLevelNorm`, `SimpleSumBuoyancyData`, `CalcBuoyancyForce`, `PreCalcSetup`, `AddSplashParticles`, `SimpleCalcBuoyancy`, `ProcessBuoyancyBoat`, `ProcessBuoyancy` |

**Diferencias reales corregidas:**
- Productos matriz-vector con el orden de sumas del EXE (`Multiply3x3` 0x59C790 y `MultiplyMatrixWithVector` 0x59C890) en todas las funciones; `ProcessBuoyancyBoat` usa la matriz del vehículo sin crearla.
- `FindWaterLevel` y `FindWaterLevelNorm` escriben el estado 1 antes de consultar el agua.
- `SimpleSumBuoyancyData`:
  - la guarda de `calcStruct` solo prueba y pone el bit 0;
  - el cuadrado del barco es un `fmul`, no `pow`.
- `CalcBuoyancyForce`: `!(m > 4b)` (NaN sale) y `(0 > d) ? 0 : d` (conserva -0).
- `PreCalcSetup`: la componente mayor del desplazamiento normalizado es 1.0 exacto.
- `ProcessBuoyancy`: la inmersión del peatón multiplica por 0x3F06BCA2 en vez de dividir entre 1.9.
- `ProcessBuoyancyBoat`:
  - las coordenadas X/Y de los puntos se acumulan sumando el desplazamiento;
  - la normal es `(n + (0,0,K)) * (1/(K+1))` con K global (0x8D3384);
  - producto escalar z, y, x con la velocidad del punto.
- `SimpleCalcBuoyancy`:
  - coordenadas acumuladas como en el barco;
  - la salpicadura del lado +X usa los puntos `[2][i]` y `[2][i+1]` (la DLL usaba `[i][2]`).
- `AddSplashParticles`:
  - distancia y módulo con sumas z, y, x; interpolación `from + (to - from) * t`;
  - la velocidad se escala como `(v * (1/|v|)) * 0.1` con 0.1 global (0x8D3388); NaN sale sin informar al audio;
  - número de partículas con `_ftol2`; suma extra de `m_vecMoveSpeed * 0` (signo de los ceros);
  - azar `rand() * (1/32767) * 0.5`;
  - en peatones el ángulo y la posición de la estela se calculan antes de `GetTaskSwim`.

**Método:**
- Las funciones corren sobre un `cBuoyancy` falso (no `mod_Buoyancy`) y entidades falsas con offsets crudos (matriz propia, masa, velocidades, tipo, modelo, `m_nVehicleType`/`SubType`, manejo, inteligencia y datos del jugador).
- Simulados y trazados en el EXE y en la DLL: `CWaterLevel::GetWaterLevel` (nivel lineal, fallos y normal opcionales), `CEntity::GetColModel`, `CPhysical::GetSpeed`/`ApplyTurnForce`, `FxSystem_c::AddParticle`, `CPedIntelligence::GetTaskSwim`, `CAEPedAudioEntity::AddAudioEvent`, `CAudioEngine::ReportWaterSplash`, `CGeneral::GetAngleBetweenPoints`/`LimitAngle` y `rand`.
- Se comparan los 0xD0 bytes del `cBuoyancy`, `calcStruct`, `fPointVolMultiplier`, vectores de salida, retorno y trazas.
- Se guardan y restauran `calcStruct`, `fPointVolMultiplier`, `CTimer::ms_fTimeStep`, los sistemas `m_Wake`/`m_WaterSplash` de `g_fx` y `mod_Buoyancy`.
- Fuera de los fixtures: velocidades o distancias NaN/enormes y entidades sin matriz.

Repetir: `python reversing/Buoy9/build_probe.py` y `python reversing/run.py --case buoy9`. Evidencia: [validation-summary.json](validation-summary.json).
