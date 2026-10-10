<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorLifecycleRuntime

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-094126-298973`, 4096 pares. El alcance se limita a los fixtures y colaboradores documentados.

Funciones: Init593BF0, Unfurnish5915D0, IsVisible5929F0 y AddPickups591F90. Evidencia canonica completa en `../InteriorEvidence/Interior_<nombre>.txt`.

Compara todos los bytes del interior, nodo, objeto, matriz, box, pool, contador/lista de interiores robables y trazas ordenadas; ninguna direccion semantica se elimina. Ambos lados usan exactamente los mismos fixtures. Init compara EAX completo y IsVisible AL. Los generadores de pickups tienen callbacks separados para el retorno int32 del EXE y tPickupReference del wrapper DLL, con las mismas trazas y efectos.

Rutas positivas/negativas: Init clases0..7/99, datos de semilla presentes/ausentes y lista llena; Unfurnish pool vacio/nodo, jugador ausente y objeto retenido/bandera ausente; IsVisible matriz de camara presente/ausente y entrada permitida/bloqueada; AddPickups cooldown, cien intentos sin tiles validos, dinero y cuatro armas. Dependencias EXE/DLL emparejadas respetan sus contratos. El destructor de entidad usa una vtable falsa slot0. GetTileStatus y las operaciones de listas permanecen reales.

Limites: Init usa matriz de grupo preexistente y no acredita materializacion NULL ni excepciones/unwind. Unfurnish usa un solo nodo y entidades validas; no acredita grafos corruptos ni destruccion real del mundo. Constructor/destructor de matriz son colaboradores externos.

El cierre verifica restauracion de hooks y bloqueos, destruye parches RAII y compara sus cinco bytes originales. Guarda, restaura y compara pool, timer, cooldown de pickups, contador de robos, contador/IDs de interiores y puntero de semilla. La copia de camara abarca20bytes desde B6F02C, incluido el puntero B6F03C que la version previa omitio. `state_restored` exige todas estas comprobaciones; un fallo produce evento terminal error. Root controla compilacion y GTA serial. No se ha relajado el analizador.

Intento final `20261008-093821-568333`: GTA se colgó durante el arranque, antes de generar el log del banco; quedó rechazado. La repetición con el mismo binario `20261008-094126-298973` aprobó los 4096 pares y la restauración del estado. No se recompiló ni se modificaron fixtures entre esos intentos.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-094126-298973`: 4,096 pairs across 4 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
