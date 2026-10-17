<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Búfer de render, cámara RW y modos de vídeo (lista de 8)

Validadas el 02/10/2026 con **6.912 pares original/candidato dentro de GTA**, todos exactos: 12 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `RenderBuffer` | `ClearRenderBuffer`, `RenderStuffInBuffer`, `StartStoring`, `StopStoring` |
| `RwCore` | `RwCameraBeginUpdate`, `RwCameraClear`, `RwCameraEndUpdate`, `RwCameraShowRaster` |
| `VideoMode` | `FreeVideoModeList`, `GetVideoModeList`, `IsVideoModeExclusive`, `SetVideoMode` |

**Diferencias reales corregidas:**
- `RwGlobals` no tenía `fileFuncs` (`RwFileFunctions`, 11 punteros en +0xC4, ver `RwOsGetFileInterface` 0x804130), así que `stringFuncs` y `memoryFuncs` quedaban 0x2C bytes desplazados; afecta a `rwsprintf` (+0xF0) y `rwfree` (+0x138) en todo el proyecto.
- `RenderBuffer`:
  - los contadores del búfer temporal (0xC4B950/0xC4B954) son dwords con signo, no `uint16`;
  - `StopStoring` recorre hasta `totalNumIdx` exclusivo y suma la palabra baja del contador de vértices.
- `VideoMode`:
  - `GetVideoModeList`:
    - recuento de modos con signo;
    - compara la relación de aspecto exactamente con 0,8, 0,75, 0,6 y 0,625 (los modos 16:9 ya no se listan, como en el EXE);
    - resta de la VRAM **total** (0xC8CF80), no de la disponible (0x745BE5);
    - escribe con `rwsprintf` (sin el relleno de `sprintf_s`).
  - `FreeVideoModeList` libera con `rwfree`, no con `CMemoryMgr::Free`.
  - `SetVideoMode` no llama a `RwEngineGetNumVideoModes` (el `assert` añadía una llamada).

**Método:**
- `RenderBuffer`: contadores rellenados con la palabra alta puesta; Im3D simulado en las direcciones del EXE (las funciones de la DLL son envoltorios).
- `RwCore`:
  - cámaras y rasters falsos; `beginUpdate`/`endUpdate` son punteros trazados de la cámara falsa;
  - `stdFunc[rwSTANDARDCAMERACLEAR]` (+0x9C) se sustituye solo durante la llamada y `RwRasterShowRaster` se simula en 0x7FB1A0;
  - nunca se toca la cámara ni el dispositivo reales.
- `VideoMode`:
  - nunca se cambia el modo de vídeo;
  - simulados y trazados: `RwEngineGetNumVideoModes`, `GetVideoModeInfo`, `GetCurrentSubSystem`, `RwD3D9ChangeVideoMode`, `CPostEffects::SetupBackBufferVertex`, `CMemoryMgr::Calloc` (0x72F460, búferes estáticos) y `rwfree` (+0x138, solo durante la llamada);
  - `rwsprintf` es el real y escribe en los búferes falsos; `Scene.m_pRwCamera` apunta a una cámara falsa en `SetVideoMode`.
- Todo el estado global tocado se guarda y se restaura en cada ejecución y al final.

Repetir: `python reversing/RenderVideo8/build_probe.py` y `python reversing/run.py --case rendervideo8`. Evidencia: [validation-summary.json](validation-summary.json).
