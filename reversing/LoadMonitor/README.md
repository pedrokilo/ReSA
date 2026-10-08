<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CLoadMonitor

Validadas el 02/10/2026 con **4.864 pares original/candidato dentro de GTA**, exactos bit a bit: las 7 funciones de `CLoadMonitor` con `validated=1`.

**Diferencias reales corregidas:**
- Constructor: también pone a cero `m_iCyclesHistory` (0x53CFED).
- `EndFrame`: el juego ejecuta el x87 a **24 bits de precisión** (medido en el banco). Los enteros se cargan exactos (`fild`) y cada operación se redondea a `float`. Por eso las acumulaciones, la suma del historial y las divisiones redondean paso a paso como el original, y la velocidad del jugador es `sqrt((z² + y²) + x²)` con las componentes ya escaladas por 50.

**Método:** objeto propio. `CTimer::GetCurrentTimeInCycles`/`GetCyclesPerMillisecond` controlados, tiempo del juego fijado por caso y ciclos realistas (< 2³¹). Se compara el objeto completo y que `g_LoadMonitor` no cambie.

Repetir: `python reversing/LoadMonitor/build_probe.py` y `python reversing/run.py --case loadmon`. Evidencia: [validation-summary.json](validation-summary.json).
