<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
