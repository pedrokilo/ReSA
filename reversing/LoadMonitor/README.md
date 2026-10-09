<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CLoadMonitor

Validadas el 02/10/2026 con **4.864 pares original/candidato dentro de GTA**, exactos bit a bit: las 7 funciones de `CLoadMonitor` con `validated=1`.

**Diferencias reales corregidas:**
- Constructor: también pone a cero `m_iCyclesHistory` (0x53CFED).
- `EndFrame`: el juego ejecuta el x87 a **24 bits de precisión** (medido en el banco). Los enteros se cargan exactos (`fild`) y cada operación se redondea a `float`. Por eso las acumulaciones, la suma del historial y las divisiones redondean paso a paso como el original, y la velocidad del jugador es `sqrt((z² + y²) + x²)` con las componentes ya escaladas por 50.

**Método:** objeto propio. `CTimer::GetCurrentTimeInCycles`/`GetCyclesPerMillisecond` controlados, tiempo del juego fijado por caso y ciclos realistas (< 2³¹). Se compara el objeto completo y que `g_LoadMonitor` no cambie.

Repetir: `python reversing/LoadMonitor/build_probe.py` y `python reversing/run.py --case loadmon`. Evidencia: [validation-summary.json](validation-summary.json).
