<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesFightLeaf2

Preparado, sin compilar ni ejecutar. La propuesta source-proposal.patch aÃºn NO estÃ¡ aplicada; el banco no puede aprobar la reconstrucciÃ³n hasta que el coordinador la aplique y compile.

Tres hooks thiscall sin argumentos de pila, 2048 pares cada uno (6144 total): GetComboAnimGroupID4ABDA0, IsComboSet4ABDC0 e IsHitComboSet4ABDF0. InjectHooksMain ya registra la clase. La propuesta implementa las hojas naked para reproducir EAX/AL, ECX y EDX exactos. IsComboSet retorna AL y conserva flags en el resto de EAX; IsHitComboSet compara movimiento signed y desplaza modulo32, incluidos valores negativos. Referencia detallada: ../OriginalDependencies499/review-fight-leaf.md.

Fixtures: buffer raw this28h sin vtable ni construcciÃ³n (no se leen), tabla real12Ã—88h temporal con relleno determinista, IDs aleatorios y flags cero/FFFF/cada bit/5555/AAAA. Se cubren combo signed -128..15 y los 256 bytes de movimiento; mayores de15 se excluyen porque el Ã­ndice sale del array real. ComparaciÃ³n registra retorno semÃ¡ntico, EAX completo, ECX/EDX y ESP inmediatamente despuÃ©s del call, exige cÃ¡lculo ABI esperado, ausencia de mutaciÃ³n de this/tabla, rutas RH OFF/ON y restauraciÃ³n de tabla y hooks. No hay colaboradores ni stubs. No se bombea mensajes hasta restaurar la tabla.

Constructor y destructor quedan fuera de este lote porque necesitan referencias y animaciones reales para cubrir todas sus ramas.

EjecuciÃ³n serial por coordinador despuÃ©s de aplicar la propuesta: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh DependenciesFightLeaf2 3`.

La auditoría final confirma los tres hooks RH de producción y sus rutas OFF/ON en todos los pares, retorno y efectos exactos de EAX/AL/ECX/EDX, ESP equilibrado, ausencia de mutaciones y restauración de tabla y hooks. Partidas sin cambios y configuraciones restauradas.

Evidencia: [resumen de validación](validation-summary.json), [auditoría final](runs/20261010-183206-242973/approval-audit.json), [comparaciones completas](runs/20261010-183206-242973/comparisons.jsonl.gz) y [resultado](runs/20261010-183206-242973/RESULTADO.txt). SHA-256 del JSONL descomprimido: `24a788f4a01c3d19a61022141626d18a108976abaf089173fe36cffa916c3dd1`.

La aprobación abarca estas tres hojas y los fixtures descritos dentro del juego completo; no incluye constructor/destructor, todas las integraciones posibles ni independencia de datos y dependencias transitivas del EXE.
