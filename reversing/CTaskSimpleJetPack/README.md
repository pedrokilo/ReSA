<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskSimpleJetPack — 14/14

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

DropJetPack (0x67B660) aprobó 128 comparaciones dentro del motor. La implementación existente coincide con el original; se habilitó después de validar. El comentario antiguo sobre GenerateNewOne no se reprodujo en los escenarios probados.

64 pares permiten GenerateNewOne real: 32 crean un pickup por lado y los otros 32 parten de tarea finalizada. Otros 64 pares simulan fallo de generación. Se ejecuta CreatePickupCoorsCloseToCoors original con consultas reales al mundo y una secuencia aleatoria determinista temporal. Se comparan argumentos, coordenadas, handle, tabla de pickups, estado de tarea, orden de llamadas, prioridad de aborto y empuje. Incluye matriz y posición simple, tarea finalizada, aborto aceptado/rechazado y empuje variable. Contadores verifican original independiente del candidato.

MakeAbortable se sustituye con una implementación virtual controlada. No se validan vuelo completo, animación, recogida del pickup, renderizado, FX o audio. Audio y Global/SurfaceInfos_c están excluidos. 14/14 cuenta funciones registradas, no cobertura exhaustiva de toda la clase ni estabilidad del juego.

El banco elimina objetos de pickups temporales, restaura su tabla, jugador, parches y hooks; el runner comprueba partidas y restaura ASI/PDB. `PROBAR.cmd --case jetpack` o `python reversing/run.py --case jetpack` termina sin supervisión. Integrado en --all con sonda archivada. build_probe.py restaura fuentes en finally; hashes y resultado en validation-summary.json.

Evidencia: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-jetpack/20260922-211855-495635`. Sin commit/push.
