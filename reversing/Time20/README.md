<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7047 con evidencia histórica, 0 con vigencia acreditada y 1399 pendientes en 38 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Ciclo horario (lista de 20)

Validadas el 03/10/2026 con **7.488 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTimeCycle` | `Initialise`, `InitForRestart`, `Shutdown`, `Update`, `StartExtraColour`, `StopExtraColour`, `AddOne`, `CalcColoursForPoint`, `FindFarClipForCoors`, `FindTimeCycleBox`, `SetConstantParametersForPostFX`, `GetAmbientRed/Green/Blue`, `GetAmbientRed/Green/Blue_Obj`, `GetAmbientRed/Green/Blue_BeforeBrightness` |

**Diferencias reales corregidas:**
- `Initialise`:
  - conversiones a byte/word con `_ftol2` (truncación de 64 bits), `a + a` para los alfas de post-FX;
  - la dirección de la luz al sol sale de `fcos`/`fsin` x87 sobre los ángulos `double` del EXE (0x86A6C8/0x86A6D0), no de `PI` en float.
- `AddOne`: la fuerza se multiplica por 0,01f (0x858C58) en vez de dividir entre 100; el byte de LOD pasa por `_ftol2`.
- `FindTimeCycleBox`:
  - el margen en z es `Falloff * 0,33333334f` (0x859040), no `/ 3`;
  - la distancia suma z², y², x² en ese orden y la distancia por eje elige primero `p - max`;
  - NaN descarta la caja en las pruebas de margen y cae en la rama «fuera» al comparar la distancia; la cuenta de cajas es con signo.
- `CalcColoursForPoint` (también `Update` y `FindFarClipForCoors`):
  - la hora usa los segundos como byte (`movzx byte 0xB70150`) con 1/60 y 1/3600 del EXE; el ángulo del sol usa minutos enteros + segundos (word) / 60 y `fsin`/`fcos` sin redondear;
  - guarda los climas viejo y nuevo en 0xB7CB20/0xB7CB24;
  - el smog de LA multiplica por 0,005 y no interpola si el factor es 0 o NaN;
  - las tablas de la caja de clima son bytes comparados con -1: el valor 255 ya no salta la mezcla;
  - la lejanía de caja/LOD/far-clip siempre mezcla `(1 - t) * actual + min(caja, actual) * t`;
  - `m_ExtraColourInter` solo se recorta por el lado hacia el que avanza;
  - el túnel decide `ignoreSky` con su propia entrada (la DLL usaba la del color extra);
  - el ambiente se trunca con `_ftol2` y se multiplica por 1/255; el gris bajo el horizonte se trunca a byte antes de mezclarlo;
  - la altura entra con z >= 200 y multiplica por 1/300; la distancia fuera del mundo multiplica por 0,001 y entra con 0;
  - máximos, recortes y comparaciones con la semántica NaN del EXE; se quitan los `{}` de los `CColourSet` locales.

**Método:**
- Cada ejecución restaura y rellena todo el estado de `CTimeCycle` (tablas, cajas, colores actuales, color extra, vectores al sol, sombras, gris bajo el horizonte, brillo añadido) y sus dependencias: `CWeather`, `CClock`, paso de tiempo, `CCoronas::LightsMult`, `TunnelWeather`, `gfLaRiotsLightMult`, `CCutsceneMgr::ms_running`, visiones, brillo del menú, posición y eje z frontal de la cámara.
- `timecyc.dat` se sirve en memoria (`SetDir`, `OpenFile`, `CloseFile`, `LoadLine` simulados con trazas, líneas de comentario y vacías intercaladas, números con fracciones binarias exactas).
- `IsVisionFXActive`, `PlayerNoRain` y `CamNoRain` se simulan con bits trazados; `CColourSet`, `CShadows::CalcPedShadowValues` y `CVector::Normalise` son reales.
- Se compara el estado completo (hash por bytes, con las primeras diferencias si fallan), el `CColourSet` de salida y los valores devueltos.
- Fuera de los fixtures:
  - el `OpenFile` nulo (la DLL conserva la salida NOTSA; el EXE se cae);
  - NaN en las entradas y `m_NumBoxes` = 20 en `AddOne` (escribiría fuera del array).

Repetir: `python reversing/Time20/build_probe.py` y `python reversing/run.py --case time20`. Evidencia: [validation-summary.json](validation-summary.json).
