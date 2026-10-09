<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Nubes CClouds (lista de 21)

Validadas el 03/10/2026 con **8.384 pares original/candidato dentro de GTA**, todos exactos: 20 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CClouds` | `Init`, `Update`, `Shutdown`, `SetUpOneSkyPoly`, `Render`, `RenderSkyPolys` |
| `CClouds` (niebla móvil) | `MovingFogInit`, `MovingFog_Create`, `MovingFog_Delete`, `MovingFog_Update`, `MovingFog_GetFXIntensity`, `MovingFog_GetWind`, `MovingFog_GetFirstFreeSlot`, `MovingFogRender` |
| `CClouds` (nubes volumétricas) | `VolumetricCloudsInit`, `VolumetricClouds_Create`, `VolumetricClouds_Delete`, `VolumetricClouds_GetFirstFreeSlot`, `VolumetricCloudsGetMaxDistance`, `VolumetricCloudsRender` |

**Diferencias reales corregidas:**
- `Update`: `fsin` del x87; la rotación individual se suma como flotante (dword sin signo, +2^32) y se trunca con `_ftol2`, no como suma entera.
- `SetUpOneSkyPoly`: contadores del búfer temporal como dwords completos (no `uint16`).
- `MovingFogInit`: solo limpia los 350 bytes de huecos (`rep stos`), no toda la tabla.
- `MovingFog_Update`: distancia sumada z, y, x; NaN cuenta como cerca.
- `MovingFogRender`:
  - sale si el área actual **o** la del jugador no es 0;
  - paso `timestep · (1/300)` (multiplicación);
  - primera esquina `(derecha + arriba) + posición`; derecha sin matriz con `fsin`/`fcos`.
- `Render` (transcrito sin `RenderBuffer`):
  - máscara de la luna: X con ancho·5,4·(día/31 − 0,5) e Y con alto·0,7 (0x3F333334), no con el tamaño de la máscara;
  - estrellas R*: color base con el balance de nubes, Z = (cam.z + 10) + Z·80, tamaño (w·S)·0,8, el `rand` del asterisco antes de proyectar y fórmula `c·(r·0,0015625 + 0,5)`;
  - nubes bajas: color `uint16` completo por (1 − balance) y comparaciones del EXE;
  - estelas: dos vértices directos en el búfer sin tocar sus contadores.
- `RenderSkyPolys`: gris = gris·(1 − f) + inferior·f (no gris + f·(inferior − gris)); `fsin`/`fcos` del x87; límites con NaN como el EXE.
- `VolumetricClouds_Create`: alfa `36 − trunc(rand/32768·−92)`, desplazamiento entero `[-n, n)` sobre copia local (no escribe `*posn`) y orden de `rand` alfa, tamaño, posición.
- `VolumetricClouds_GetFirstFreeSlot` y `m_VolumetricCloudsUsedNum`: con signo.
- `VolumetricCloudsGetMaxDistance`: NaN devuelve 600.
- `VolumetricCloudsRender`:
  - densidad media 0,66 (no 1/3) y áreas como `MovingFogRender`;
  - no anula la velocidad del jugador; vehículo antes que coordenadas; bits de guarda 0xC6E974;
  - sin vehículo usa el frente de `TheCamera` + 0x984;
  - `bJustCreated` con comparaciones estrictas, distancia z, y, x, dirección normalizada solo si no es nula y producto escalar z, y, x;
  - contador propio con vaciado a 2034 vértices (sin `RenderBuffer`).

**Método:**
- Cámara (matriz falsa o nula), `Scene.m_pRwCamera`, peatón y vehículo del jugador falsos; texturas de nubes y coronas sustituidas por texturas falsas.
- Estados de render, Im3D, lectura/destrucción de texturas, `CTxdStore`, sprites, `ImmediateModeRenderStates*`, `CamNoRain`/`PlayerNoRain`, `CanSeeOutSideFromCurrArea`, `RenderOutGeometryBuffer`, `rand` y `FindPlayer*` simulados y trazados.
- Todas las globales de `CClouds` (0xC6AA58–0xC6E978), búfer temporal, clima, colores, reloj y tiempo se guardan y se restauran en cada ejecución.
- No cubierto: índices de primitiva fuera de 0..3 en `MovingFogRender` (el EXE reutiliza locales sin inicializar) y más de 180 nubes usadas (la DLL aborta en `std::array`).

Repetir: `python reversing/Clouds21/build_probe.py` y `python reversing/run.py --case clouds21`. Evidencia: [validation-summary.json](validation-summary.json).
