<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# PlayerControlFighter

**Validado el 07/10/2026: 4.096/4.096 pares exactos**, con estado, configuración y partidas restaurados. [Evidencia](validation-summary.json). Banco de 4.096 pares original/candidato para `CTaskSimplePlayerOnFoot::PlayerControlFighter` (0x687530), con conmutación del hook comprobada.

Pseudocódigo REA/Ghidra y ensamblador de `combo.sh`, rango 0x687530..0x687C17. Se corrigen seno/coseno x87, precisión extendida conservada en la rama no cooperativa, orden de productos y distancia z²+y²+x². Ranura de arma interpretada con signo.

Peatón, tarea, tarea de lucha, inteligencia, pad, matrices, objetivo, datos de jugador e información de arma en búferes propios. Las consultas y efectos se simulan y trazan en EXE y DLL. Se comparan bytes de objetos y trazas, con normalización de punteros de fixtures. Se guardan y restauran timestep, orientación de cámara, modo de ratón y estado del hook. Ninguna entidad real ni entrada física de usuario se modifica.

Cobertura: tarea de lucha ausente; cooperativo permitido/rechazado; objetivo presente/ausente, matrices del objetivo presentes/ausentes; botones de apuntar/correr/agacharse; armas cuerpo a cuerpo/otras; ejes cero, diagonales, extremos y valores fuera del rango normal; magnitudes y timer alrededor de los umbrales; comandos de lucha. Las funciones de ángulo son proveedores controlados; no se acredita su implementación ni una regresión completa en juego. No se generan NaN ni entradas inválidas.

Repetir: `python reversing/PlayerFighterPending/build_probe.py`, `python reversing/PlayerFighterPending/run_local.py`.

<!-- BEGIN PENDING BATCH REGRESSION -->

Regresión del 07/10/2026: **APROBADA**. 4.096 pares exactos. [Resumen actualizado](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-player-fighter-pending\20261007-201558-576943/summary.json>).
<!-- END PENDING BATCH REGRESSION -->
