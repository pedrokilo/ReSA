<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CLoadingScreen

Validadas el 02/10/2026 con **9.328 pares original/candidato dentro de GTA**, todos exactos: las 20 funciones de `CLoadingScreen`, `LoadingScreen` incluida, con `validated=1`.

La reconstrucción tenía muchas diferencias con el original. Se reescribió fiel al EXE y se mantienen las opciones NOTSA del cargador rápido (`NoFading`, `NoLoadScreen`, `NoLoadBar`, `ScreenChangeTime`), que con sus valores por defecto equivalen al juego original.

**Diferencias reales corregidas:**
- TXD de las pantallas: `loadscs` (la DLL usaba `loadsc0`).
- `LoadSplashes`: orden aleatorio con el algoritmo del original, que saca una pantalla al azar cada vez de entre las 15 (`loadsc0`..`loadsc14`) con `rand()`. La DLL barajaba solo 7 con `mt19937`.
- `GetClockTime` y los relojes en línea: milisegundos como entero sin signo, multiplicados por 0,001 en x87 (no divididos entre 1000).
- `RenderLoadingBar`:
  - escalado del original, `(ancho · 1/640) · valor`;
  - color pedido componente a componente;
  - `m_TimeBarAppeared` solo se anota después de dibujar la barra.
- `Update`: estaba reconstruida con otra lógica; se transcribió del original (porcentaje, cambio de pantalla, fundidos, volumen del tema).
- `DoPCScreenChange`: el volumen es `(255 - alfa) · 1/255` en el fundido a negro.
- `NewChunkLoaded`: umbral de 5,5 s en la pantalla de copyright y 5 s en las demás, incluida NONE.
- `GetCurrentDisplayedSplash`: indexa sin comprobar límites, como el original. Con NONE (-1) el juego real lee la entrada anterior, y `std::array` abortaba en depuración.
- `FIX_BUGS` de `DoPCScreenChange`: solo actúa cuando el original se saldría del array.

**Método:**
- El estado estático de la pantalla de carga se aleatoriza por caso.
- La configuración del cargador rápido se fija en sus valores por defecto.
- Se simulan y se trazan el reloj, `srand`/`rand`, `CTxdStore`, `CFileMgr`, `CSprite2d`, los estados de render, una cámara RW falsa (la DLL usa la macro y el EXE la función) y el tema de carga.
- La semilla de `srand` sale de `QueryPerformanceCounter` real y no se compara.

Repetir: `python reversing/Loading/build_probe.py` y `python reversing/run.py --case loading`. Evidencia: [validation-summary.json](validation-summary.json).
