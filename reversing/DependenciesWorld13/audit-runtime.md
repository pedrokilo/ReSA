<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de DependenciesWorld13

Se leyó íntegramente el gzip de `20261010-220155-120090` en streaming. Una función CPathFind::RemoveInteriorLinks (0x44DF60), 2.048 pares/casos únicos consecutivos 0..2047, original y candidata no nulos y exactamente iguales, accepted/equal verdaderos y cero diferencias. Todas las puertas de ruta, contrato, guardas, lectura del objeto, registros, pila, estado y x87/MXCSR son verdaderas en cada lado.

La cobertura real observada comprende todos los contadores 0..64: 32 casos por contador 0..32 y 31 por 33..64. Ocho formas de grafo: 260 casos cada una en 0..6 y 228 en 7; ocho semillas EAX distintas, 256 cada una. Se compara la tabla resultante completa de 384 bytes, contador y guardas, con ThePaths de sólo lectura. La instrumentación utiliza estados legales de datos de la tabla original, sin helpers sustituidos, actores o allocadores. El retorno void se observa mediante registros crudos; no se inventa valor contractual.

Begin acredita hook registrado, tabla global real 0x96EAC0, contador 0x96EF80, capacidad 64 y seis slots por fila, sin colaboradores sustitutos. Function_summary confirma complete_paths, recuentos exactos y cero fallos. Complete confirma 2.048 pares y restauración global/RH/x87 de 108 bytes/MXCSR. Summary acredita éxito, configuraciones restauradas y partidas intactas. La fuente mantiene el cuerpo original, RET4 y LEA EBX,[EBX] de alineación 0x44DF7A.

Huellas verificadas contra el resumen admitido:

- Registro descomprimido: `3bc61396daf2022ecda49ebfd78f19bec0ccd71314b889c0c7e2b7882f2793ce`.
- Summary: `56a2a5a4ae9cd2a6fed30b8e2938ad3ebde560cd59dda83b2a08813a36932ea1`.
- RESULTADO: `068ff78667e6d037d9c22b2ae3020d93f7311fc6d0b3049660ceacad90e85563`.
- ASI histórica declarada: `81ae7e818541f4f249a66d828d605fa6f255afbfc365f425b1b33fea5c7fa1d0`.

Los tres instrumentos actuales coinciden con bank_sources del manifiesto. Las fuentes de producción son una instantánea histórica; cambios posteriores en otros bancos no invalidan retrospectivamente esa identidad. El resultado acredita esta función bajo los fixtures descritos, sin demostrar independencia completa del EXE o integración universal. Sin defectos encontrados en la evidencia auditada. Sólo se modificaron documentos Markdown.
