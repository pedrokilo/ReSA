<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio, interfaz y render (lista de 70)

Validadas el 02/10/2026 con **12.416 pares original/candidato dentro de GTA**, todos exactos: 24 funciones con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEStreamingDecoder` | Constructor, Destructor |
| `CAEStreamTransformer` | `Initialise`, `TransformBuffer` |
| `CColourSet` | Constructor, `Interpolate` |
| `CCrime` | `FindImmediateDetectionRange`, `ReportCrime` |
| `CCurrentVehicle` | `Display`, `Process` |
| `CFallingGlassPane` | `Render`, `Update` |
| `CGamma` | `Init`, `SetGamma` |
| `COnscreenCounterEntry` | `ProcessForDisplayCounter`, `SetColourID` |
| `CPlaceName` | `GetForMap`, `Process` |
| `CRegisteredMotionBlurStreak` | `Render`, `Update` |
| `CUserDisplay` | `Init`, `Process` |
| `Securom` | `SecuromDisplayInit`, `SecuromStateDisplay` |

**Diferencias reales corregidas:**
- `CAEStreamingDecoder::Destructor`: destructor de `CAEDataStream` sin pasar por la vtable y `operator delete` escalar, como el EXE.
- `CColourSet`:
  - `m_fIllumination = mult · 0,01`;
  - `Interpolate` guarda la intensidad mínima de brillo como palabra completa y trunca con `_ftol` a 64 bits.
- `CCrime::ReportCrime` reescrita:
  - siempre usa el `CWanted` del jugador;
  - llama a `WorkOutPolicePresence` antes de `GermanGame`;
  - termina con `SetWantedLevelNoDrop`;
  - mismo número de llamadas a `FindPlayerPed`.
- `CFallingGlassPane::Update`:
  - constantes 0,35/0,01/0,02;
  - `AddGlass` recibe la posición del suelo;
  - tamaño aleatorio en línea.
- `CFallingGlassPane::Render`:
  - distancia a la cámara calculada antes;
  - alfa con la conversión del original;
  - vaciados de lote que faltaban;
  - índices de cristal roto corregidos.
- `CGamma`:
  - `m_IsEnabled` es de 32 bits;
  - la rampa usa `pow` en double y `fmul` x87 por 65535.
- `COnscreenCounterEntry::ProcessForDisplayCounter`: el contador doble imprime la segunda variable (la DLL repetía la primera); `sprintf` sin relleno de depuración.
- `CPlaceName::Process`: cualquier interior (no solo el área 1) mantiene la zona.
- `CRegisteredMotionBlurStreak::Render`: comprueba la entrada `iInd` del historial y calcula el alfa como el original.
- `Securom::SecuromStateDisplay`: textos `"Trigger: %d"` y `"Trap: %d"`.

**Diferencia no reproducible (enmascarada):** sin zona, `CPlaceName::Process` del EXE pasa a `SetZoneName` un valor de pila sin inicializar; la DLL pasa `nullptr`.

**Método:**
- Dispositivo D3D falso (la gamma real nunca se toca).
- Cámara y peatón del jugador sustituidos temporalmente.
- Se simulan y se trazan `CFont`, `CText`, `CHud`, Im3D, `CGlass`, `CWanted` y la policía, `COnscreenTimer`, `CAEDataStream::Initialise` y `operator delete` (este con registro sin reservas).
- Todo el estado global tocado se restaura por ejecución.

Repetir: `python reversing/Misc70/build_probe.py` y `python reversing/run.py --case misc70`. Evidencia: [validation-summary.json](validation-summary.json).
