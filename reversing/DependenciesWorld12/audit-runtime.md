<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de DependenciesWorld12

Se leyó íntegramente el gzip admitido de `20261010-220421-862584` en streaming. RwHelper::GetAnimHierarchyFromFrame (0x734AB0), 2.048 pares/casos únicos consecutivos 0..2047. Original y candidata son objetos no nulos, exactamente iguales, accepted/equal verdaderos. Cero diferencias; las siete puertas de ruta, biblioteca, pila, registros, contrato, lectura y restauración x87/MXCSR son verdaderas en ambos lados.

Cobertura observada: 512 casos por tipo, frame vacío creado por RenderWare original; árbol real con hijos y sin jerarquía; jerarquía directa; jerarquía encontrada en descendientes reales. Cada retorno crudo coincide con expected_pointer; ambos lados usan los mismos recursos vivos, sin normalización. Begin acredita 33 frames reales, offset HAnim 164, frame vacío legal, hook registrado, bibliotecas nativas verificadas y ausencia de sustitutos. El callback 0x734A70 y los cuerpos completos de getter y recorrido quedan verificados por bytes originales y módulo EXE antes y después de cada llamada.

La ejecución aprobada exige exterior área0 con FastLoader.SaveGameToLoad=0 mediante el helper aprobado de World10. El primer intento interior de cero pares carecía de ramas necesarias y no fue admitido. La reparación sólo cambia precondiciones del banco: espera natural hasta60s, diagnósticos por tipo y restauración FP entre frames. La raíz corrigió tres referencias al área usando GetAreaCode antes de compilar; la fuente reconstruida quedó intacta.

Function_summary acredita complete_paths, cuatro recuentos512 y cero fallos. Complete confirma destrucción del frame propio, recursos y estado restaurados, bibliotecas originales y x87 de108bytes/MXCSR. Summary del corredor confirma éxito, configuración restaurada y partidas intactas. Se conservan bytes declarados de frames prestados y globals/lista dirty; no se afirma restauración universal del heap. La recursión conserva que una jerarquía encontrada en subárbol puede ser reemplazada por otro hermano, como el callback original. No se acredita transitivamente RenderWare ni el callback.

Huellas verificadas:

- Registro descomprimido: `970992b04c6795e3509272039e76f7c07617598ad7fdf7d122a03dc8ee1d80c9`.
- Summary: `7c19e6fce4d5b086b298ed0b7ef69fa06fa8d174c13cfd3c5b9ba1921ac9e9fe`.
- RESULTADO: `410a1376c326964de4436318bccb9a66322b83620cb441b68cd130bae1107c4c`.
- ASI histórica declarada: `8236f35d6a285734fc1fe56fc67f85218f99953b8a88275596754389882f133a`.

Los cuatro instrumentos coinciden con bank_sources del manifiesto. Las fuentes se interpretan como instantánea histórica y no se exige identidad del Main actual después de nuevas tandas. Sin defectos encontrados en los registros auditados. Sólo se modificaron Markdown.
