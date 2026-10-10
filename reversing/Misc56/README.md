<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Clases sueltas (lista de 56)

Validadas el 02/10/2026 con **6.464 pares original/candidato dentro de GTA**, todos exactos: 10 funciones con `validated=1`.

| Clase | Función |
|---|---|
| `CEntitySeekPosCalculatorFixedPos`, `RadiusAngleOffset`, `Standard`, `XYOffset` | `ComputeEntitySeekPos` (4) |
| `CPedAttractorPedPlacer` | `PlacePedAtEffect` |
| `CRegisteredCorona` | `Update` |
| `CCheckpoint` | `Render` |
| `CDebug` | `DebugDisplayTextBuffer` |
| `CAETrackLoader` | `GetDataStream` |
| `RtAnim` | `RtAnimInterpolatorSetCurrentAnim` |

**Diferencias reales corregidas:**
- Calculadores de posición de `SeekEntity`:
  - las vtables del EXE tienen una sola entrada (`ComputeEntitySeekPos`), sin destructor virtual. La DLL declaraba el destructor virtual, así que tres de los cuatro hooks redirigían la función del EXE al destructor de la DLL.
  - Ahora el destructor no es virtual y los cuatro usan su vtable real (Standard en 0x859DC8).
  - `RadiusAngleOffset` además usa la orientación de la colocación si no hay matriz, con `fsin`/`fcos` x87 como el original.
- `PlacePedAtEffect`:
  - órdenes de suma de las transformaciones del EXE;
  - rumbo con `GetRadianAngleBetweenPoints` + `LimitRadianAngle`;
  - no crea una matriz que el original no crea.
- `CRegisteredCorona::Update`:
  - la intensidad se acerca al alfa un paso por fotograma sin pasarse;
  - con brillo inmediato salta al alfa;
  - si son iguales no se toca.
- `CCheckpoint::Render`:
  - estados de render `ZWRITE`/`VERTEXALPHA` a 0;
  - tamaño del sprite y prueba del agua como el EXE;
  - distancia del toro con su orden de sumas;
  - tipos desconocidos sin abortar.
- `CDebug::DebugDisplayTextBuffer`: el original no hace nada; la DLL ya no toca el estado de `CFont` con el búfer vacío.
- `CAETrackLoader::GetDataStream`: ruta `AUDIO\STREAMS\` + nombre sin barra doble; búfer con `operator new` escalar.

**Método:**
- Fixtures en búferes estáticos.
- Simulaciones solo durante cada llamada (sin abrir archivos reales).
- Los temporizadores y globales de coronas, la textura de la bandera, el estado de `CFont` y el callback de estados de render se guardan y se restauran.
- Quedan fuera los casos que dependen de `FIX_BUGS` (alfa < 48 en `TORUSTHROUGH` y el tipo `rwID_RPANIMBLENDPLUGIN` en `RtAnim`).

Repetir: `python reversing/Misc56/build_probe.py` y `python reversing/run.py --case misc56`. Evidencia: [validation-summary.json](validation-summary.json).
