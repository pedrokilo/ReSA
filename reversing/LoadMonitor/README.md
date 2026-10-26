<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CLoadMonitor

Validadas el 02/10/2026 con **4.864 pares original/candidato dentro de GTA**, exactos bit a bit: las 7 funciones de `CLoadMonitor` con `validated=1`.

**Diferencias reales corregidas:**
- Constructor: también pone a cero `m_iCyclesHistory` (0x53CFED).
- `EndFrame`: el juego ejecuta el x87 a **24 bits de precisión** (medido en el banco). Los enteros se cargan exactos (`fild`) y cada operación se redondea a `float`. Por eso las acumulaciones, la suma del historial y las divisiones redondean paso a paso como el original, y la velocidad del jugador es `sqrt((z² + y²) + x²)` con las componentes ya escaladas por 50.

**Método:** objeto propio. `CTimer::GetCurrentTimeInCycles`/`GetCyclesPerMillisecond` controlados, tiempo del juego fijado por caso y ciclos realistas (< 2³¹). Se compara el objeto completo y que `g_LoadMonitor` no cambie.

Repetir: `python reversing/LoadMonitor/build_probe.py` y `python reversing/run.py --case loadmon`. Evidencia: [validation-summary.json](validation-summary.json).
