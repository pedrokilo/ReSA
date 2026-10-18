<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sonidos lógicos de audio (lista de 18)

Validadas el 03/10/2026 con **9.216 pares original/candidato dentro de GTA**, todos exactos: 16 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAESound` | `UnregisterWithPhysicalEntity`, `StopSound`, `SetFlags`, `GetRelativePosition` (original), `CalculateFrequency`, `UpdateFrequency`, `GetRelativePlaybackFrequencyWithDoppler`, `GetSlowMoFrequencyScalingFactor`, `NewVPSLEntry`, `RegisterWithPhysicalEntity`, `StopSoundAndForget`, `SetPosition`, `CalculateVolume`, `Initialise`, `UpdateParameters`, `SoundHasFinished` |

**Diferencias reales corregidas:**
- `CAESound`:
  - `CalculateFrequency` (y `NewVPSLEntry`, que la lleva en línea en el EXE): con variación o velocidad NaN el EXE se queda con la velocidad sin variación (`fcomp` + `test ah,41h` / `jp`); la DLL sorteaba un valor;
  - `SetPosition` calcula la distancia a la cámara como `sqrt(dz² + dy² + dx²)` en ese orden (la DLL sumaba x, y, z);
  - `CalculateVolume` pide una sola posición relativa, usa `sqrt(z² + y² + x²) / roll-off`, llama primero a la atenuación por distancia y luego a la direccional, y redondea como el EXE: `(direccional + distancia) + volumen` y después `- headroom` (la DLL restaba el headroom primero y pedía la posición relativa dos veces);
  - `Initialise` escribe directamente en el sonido en el orden del EXE (retira la entidad física antes de `SetPosition`), sin temporal ni el `assert` NOTSA del constructor;
  - `GetSlowMoFrequencyScalingFactor` lee el factor de la variable global del EXE `0x8CBA6C` (0,5) en lugar de una constante.
- `CAEMFDecoder` (`Initialise`, `FillBuffer`, `InitLibrary`): sin cambios y fuera del banco. El EXE usa QuickTime (`0x4E7990`, `0x4E7A80`, `0x818570`, `0x8195C0`, `0x819820`) y la DLL lo sustituye a propósito por Media Foundation; solo comparten las salidas tempranas (`FillBuffer` sin inicializar devuelve -1, `InitLibrary` con la bandera `0xB61D70` puesta devuelve `true`), revisadas a mano. Los huecos 0 y 1 de la vtable `0x85F370` son correctos.

**Método:**
- El hilo de streaming de audio se aparca en `IsForegroundApp` durante todo el banco, como en FixAudio.
- Todo corre sobre objetos falsos: dos `CAESound`, dos entidades físicas (posición en `m_placement` o en una matriz falsa) y dos entidades de audio con vtable propia (`UpdateParameters` trazado en el hueco 0, que puede cambiar velocidad y variación para probar la lectura posterior).
- Se simulan y registran en el EXE y en la DLL `CEntity::RegisterReference`/`CleanUpOldReference`, `CAEAudioEnvironment::GetPositionRelativeToCamera`, `GetDistanceAttenuation`, `GetDirectionalMikeAttenuation`, `GetDopplerRelativeFrequency`, `CTimer::GetIsSlowMotionActive`, `CAEAudioHardware::GetSoundHeadroom`, `rand` y `CGeneral::GetRandomNumber`; `CAESound::SetPosition` solo en `UpdateParameters` y `SoundHasFinished`.
- Se fijan por caso y se restauran: hora y fotograma de `CTimer`, posición y matriz de `TheCamera` (falsa o nula), cámara activa y modo de las tres cámaras, y el factor `0x8CBA6C`.
- Fuera de los fixtures:
  - NaN señalizadores (el x87 los silencia al copiarlos); sí se prueban NaN silenciosos y ±0 en velocidad y variación;
  - `CAEMFDecoder::Initialise`, `FillBuffer` e `InitLibrary` (QuickTime en el EXE, Media Foundation en la DLL).

Repetir: `python reversing/Audio18/build_probe.py` y `python reversing/run.py --case audio18`. Evidencia: [validation-summary.json](validation-summary.json).
