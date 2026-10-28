<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

<!-- USER STOP 2026-10-08 -->
Estado al detener el trabajo: reconstrucción solicitada completada y aprobación histórica conservada. No se ejecutó una nueva regresión con las fuentes finales. Los hashes y límites actuales se detallan en [el estado de todas las listas](../../docs/EstadoListasSolicitadas.md). Las notas siguientes conservan su fecha y alcance originales.
<!-- /USER STOP 2026-10-08 -->

# Audio ped pendientes

Validación del banco aprobada el 2026-10-07: 4608 pares EXE/DLL, sin diferencias. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-ped-pending/20261007-191104-477122/summary.json>); comparaciones: [comparisons.jsonl](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-audio-ped-pending/20261007-191104-477122/comparisons.jsonl>).

HandlePedSwing 1024, HandlePedHit 2048, HandlePedJacked512, Service1024: total4608 pares. Combo con pseudo+asm completo en evidence. Pseudocódigo de Swing/Hit/Jacked tiene parámetros mal recuperados: argumentos, slots, variancias y deadlines fueron tomados del ensamblador. Vtable de CAEPedAudioEntity0x86C2A8: count1, UpdateParameters0x4E1180.

Reconstruye selección de sonidos por arma y evento, retraso de playback, banco aún no cargado, impactos contra peds/objetos, secuencia de carjacking y fricción contra vehículos, volumen de viento y parachute/flap. La dependencia de volumen de superficie0x4DA510 se replica localmente mediante tabla original y x87 fyl2x; no se llama al EXE desde la función reconstruida.

Fixtures controlan peds, armas, objetivos, vehículos, clump de tipo2, animaciones y vtable virtual39. Hardware/RNG/sonido/referencias/collision/player/parachute/anim y flap interceptados en EXE y DLL. Compara bytes completos mediante FNV más trazas de peticiones. Reloj y tabla de volúmenes se restauran. Gate positivo por función. Sin audio real; integración de callbacks y dependencias interceptadas pendiente.

<!-- BEGIN FINAL PENDING BATCH RESULT -->

Resultado final: **APROBADO**. 4.608 pares exactos. [Resumen y hashes](validation-summary.json); [informe completo](<D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audio-ped-pending\20261007-195322-568750/summary.json>). Esta aprobación cubre los fixtures y límites descritos arriba.
<!-- END FINAL PENDING BATCH RESULT -->
