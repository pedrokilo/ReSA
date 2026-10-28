<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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

## Revalidación 19/10/2026

Corrida `20261019-004743-227738` (informe en `resultados-interior-lifecycle-runtime`): passed=true, 4096 pares, 4 funciones, 0 diferencias, estado restaurado; refresca los hashes de `Pickups.cpp/.h` con las fuentes actuales. Sonda compilada desde el worktree `D:\Original\wq-f` (HEAD 98286d43c + fusión de CQuaternion; ver `artifacts/build-manifest.json`). El intento `20261019-004315-510521` se colgó en el arranque de GTA (fallo conocido del inicio del juego, ver arriba) y no es evidencia. Registrada con `register_revalidation_20261019.py` en `validation-summary-reval-20261019.json` (el `validation-summary.json` histórico no se toca; la evidencia anterior queda en `previous_evidence`). Los hashes de `tested_sources` son los del HEAD compilado; el árbol principal tiene además trabajo sin commit del lote 27 (sin validar) en `InteriorGroup_c.h`, por lo que esos archivos no coinciden hoy con el árbol de trabajo.
- `build_probe.py` y `run_local.py` fijan `runtime_runner.GAME` al clon `E:\JUEGOS\GTA San Andreas - CVehicle Test` (el valor por defecto `D:\JUEGOS\...` no existe).
