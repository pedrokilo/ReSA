<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6839 con evidencia histórica, 0 con vigencia acreditada y 1607 pendientes en 63 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskSimpleJetPack — 14/14

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

DropJetPack (0x67B660) aprobó 128 comparaciones dentro del motor. La implementación existente coincide con el original; se habilitó después de validar. El comentario antiguo sobre GenerateNewOne no se reprodujo en los escenarios probados.

64 pares permiten GenerateNewOne real: 32 crean un pickup por lado y los otros 32 parten de tarea finalizada. Otros 64 pares simulan fallo de generación. Se ejecuta CreatePickupCoorsCloseToCoors original con consultas reales al mundo y una secuencia aleatoria determinista temporal. Se comparan argumentos, coordenadas, handle, tabla de pickups, estado de tarea, orden de llamadas, prioridad de aborto y empuje. Incluye matriz y posición simple, tarea finalizada, aborto aceptado/rechazado y empuje variable. Contadores verifican original independiente del candidato.

MakeAbortable se sustituye con una implementación virtual controlada. No se validan vuelo completo, animación, recogida del pickup, renderizado, FX o audio. Audio y Global/SurfaceInfos_c están excluidos. 14/14 cuenta funciones registradas, no cobertura exhaustiva de toda la clase ni estabilidad del juego.

El banco elimina objetos de pickups temporales, restaura su tabla, jugador, parches y hooks; el runner comprueba partidas y restaura ASI/PDB. `PROBAR.cmd --case jetpack` o `python reversing/run.py --case jetpack` termina sin supervisión. Integrado en --all con sonda archivada. build_probe.py restaura fuentes en finally; hashes y resultado en validation-summary.json.

Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-jetpack/20260922-211855-495635`. Sin commit/push.
