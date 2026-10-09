<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Decodificador WAV, reloj, CdStream, CGeneral y comunes (lista de 9)

Validadas el 02/10/2026 con **19.968 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 5 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEWaveDecoder` | Constructor, Destructor, `Initialise`, `FillBuffer`, `GetStreamLengthMs`, `GetStreamPlayTimeMs`, `SetCursor`, `GetSampleRate`, `GetStreamID` |
| `CClock` | `Initialise`, `Update`, `GetGameClockMinutesUntil`, `GetIsTimeInRange`, `NormaliseGameClock`, `OffsetClockByADay`, `SetGameClock`, `StoreClock`, `RestoreClock` |
| `CdStream` | `CdStreamOpen`, `CdStreamRead`, `CdStreamSync`, `CdStreamGetStatus`, `CdStreamThread`, `CdStreamInit`, `CdStreamInitThread`, `CdStreamRemoveImages`, `CdStreamShutdown` |
| `CGeneral` | `LimitAngle`, `LimitRadianAngle`, `GetRadianAngleBetweenPoints`, `GetATanOfXY`, `GetNodeHeadingFromVector`, `SolveQuadratic`, `GetAngleBetweenPoints`, `GetRandomNumberInRange<float>`, `GetRandomNumberInRange` (int32, 0x407180) |
| `Common` | `AsciiToGxtChar`, `MakeUpperCase` (copia y en sitio), `CalcScreenCoors` (dos versiones), `DoesInfiniteLineTouchScreen`, `IsPointInsideLine`, `LittleTest`, `WriteRaster` |

**Diferencias reales corregidas:**
- `CAEWaveDecoder` (era otra versión del decodificador):
  - `Initialise` lee la cabecera canónica de 44 bytes de una vez (sin buscar el chunk `data`) y solo acepta estéreo de 16 bits;
  - `FillBuffer` copia directamente (sin conversión mono);
  - las duraciones usan `bytes / (frecuencia · 4)` con `fild` sin signo y división x87 (sin `blockAlign`), y la posición resta los 44 bytes de cabecera;
  - `SetCursor` calcula `pos · 0.001 · (frecuencia · 4) + 44`, trunca y redondea a par con el resto con signo;
  - todo usa la copia del stream de +0xC;
  - los virtuales pasan a instalarse por la vtable 0x862DE8 (8 entradas) con la vtable exportada.
- `CClock::Update`: la comparación del tiempo transcurrido es con signo (`jg`).
- `CdStream`:
  - `CdStreamOpen` copia el nombre con `strcpy` (`strncpy_s` rellenaba con 0xFE);
  - `CdStreamShutdown` solo libera el búfer de la cola (no pone a cero cabeza, cola ni tamaño);
  - los bucles sobre `gStreamCount` son con signo (no recorren nada si es <= 0).
- `CGeneral`:
  - `GetRadianAngleBetweenPoints`, `GetATanOfXY` y `SolveQuadratic` son transcripciones x87 (fpatan completo, `pi/2 ± pi/2` en el orden del EXE, banderas conservadas, rango de exponente ampliado, x2 antes que x1);
  - `LimitRadianAngle`, `GetNodeHeadingFromVector` y `GetAngleBetweenPoints` usan las constantes exactas del EXE (57,2957763671875 no es el float más cercano a 180/pi; `· (1/2pi) · 8`);
  - el entero aleatorio de 0x407180 multiplica por `max - min` entero sin redondearlo antes a float.
- `Common`:
  - `MakeUpperCase` solo convierte 'a'..'z' sin locale y la versión de copia devuelve el inicio del destino;
  - `CalcScreenCoors` usa el orden de sumas de 0x59C890 y la escala `(ancho · 1/z) / FOV · 70`;
  - `DoesInfiniteLineTouchScreen` usa desigualdades estrictas para el punto en pantalla;
  - `IsPointInsideLine` devuelve cierto con NaN.

**Método:**
- El decodificador WAV corre sobre un objeto con la vtable del EXE y dos `CAEDataStream` falsos (FillBuffer, GetCurrentPosition, Seek e Initialise simulados; las llamadas de otros hilos se reenvían a la implementación real).
- La API Win32 de CdStream se sustituye por IAT solo en el hilo principal, con handles falsos; streams y cola son buffers propios. Se espera a que el hilo real de CdStream esté parado y la región 0x8E3FE0..0x8E489C se guarda y restaura tras cada ejecución.
- `CdStreamThread` da una sola vuelta: la segunda espera lanza una excepción que recoge el banco.
- Los flotantes devueltos en ST(0) se comparan con sus 80 bits; también el TOP de la pila x87.
- Se simulan rand/`CGeneral::GetRandomNumber`, `CStats::IncrementStat`, `CMemoryMgr::MallocAlign`/`FreeAlign` y las primitivas RwImage/RtPNG; se guardan y restauran reloj, trucos, tiempo, matriz de vista, resolución, FOV y el contador de Im3D.
- Fuera de los fixtures:
  - FIX_BUGS de la DLL: `gLastCdStreamPosn` solo se compara con FileID 0 y `CdStreamInit` usa sectores de más de 2048 bytes (la DLL no pone `FILE_FLAG_NO_BUFFERING`);
  - el arreglo NOTSA del bloqueo de `CdStreamSync` (sección crítica) y `gbFreezeTime` (siempre falso);
  - meses fuera de 1..12 en `Update`/`OffsetClockByADay` (el EXE lee fuera de su tabla);
  - infinitos en `LimitAngle` (bucle infinito en ambos), `a = 0` en `SolveQuadratic`, frecuencia 0 en el WAV;
  - el valor devuelto por `MakeUpperCase` en sitio (el EXE no lo fija) y el borrado del stream en el destructor del WAV (+4 nulo).

Repetir: `python reversing/Core9/build_probe.py` y `python reversing/run.py --case core9`. Evidencia: [validation-summary.json](validation-summary.json).
