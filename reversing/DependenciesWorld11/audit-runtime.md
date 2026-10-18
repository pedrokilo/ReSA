<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente de DependenciesWorld11

Evidencia examinada el 10/10/2026: ejecución `20261010-215659-188015`. Se leyó íntegramente y en streaming el gzip archivado, sin ejecutar el juego ni modificar fuentes o instrumentos.

Una función (`RwHelper::RwTexDictionaryFindHashNamedTexture`, 0x734E50), 2.048 pares únicos y casos consecutivos 0..2047. Todos contienen original y candidata como objetos no nulos, exactamente iguales, con accepted/equal verdaderos. Cero diferencias. Las ocho puertas de ruta principal, helper, biblioteca, pila, registros, contrato, lectura y restauración x87/MXCSR son verdaderas en ambos lados.

La cobertura observada es 512 casos por tipo: diccionario vacío creado por RenderWare original; búsqueda ausente en diccionario cargado; primera textura encontrada; última textura encontrada tras recorrer las anteriores. Begin acredita 48 diccionarios reales, hook principal y helper registrados, helper OFF/ON con dos muestras distintas de 32 bytes y cuerpo candidato identificado, bibliotecas originales verificadas y ausencia de sustitutos. El banco verifica el módulo y resolución del cuerpo del helper antes y después de cada llamada; no utiliza una entrada mutable OFF como supuesto cuerpo reconstruido.

Function_summary confirma los cuatro recuentos, complete_paths y cero fallos. Complete acredita destrucción del diccionario propio, recursos y estado restaurados, helper/biblioteca válidos y restauración de la imagen x87 de 108 bytes/MXCSR. Summary del corredor confirma éxito, partidas idénticas y configuraciones restauradas. La observación compara punteros reales al mismo recurso vivo, sin normalización.

La conservación de recursos se limita al módulo de diccionarios de 20 bytes, enlace inicial y objetos prestados declarados. El enlace inicial corresponde a la lista de diccionarios (0x7F364D, 0x7F3672 y 0x7F3681); el freelist está en +0x0C. No se afirma identidad de toda la disposición interna del heap. La rama nula después de LEA textura+0x10 exigiría una textura inválida en 0xFFFFFFF0: se conserva en fuente y no se fabrica para cubrirla. No se concede validación transitiva a RenderWare o CKeyGen ni integración universal.

SHA-256 verificados contra validation-summary.json:

- Registro descomprimido: `6b4a37ad81248b31488d389297df59814b305b7ae4992dc2bd927372b708c83d`.
- Summary: `ea7675b0b5e7c52d458a52713371e65d65f21d1439267899de6e74c4671c5ce8`.
- RESULTADO: `ad1b3d21e3187fcf28f9664d73fc4916e02a14d1818fefb74c12730e825c0d84`.
- ASI histórica declarada: `ed26c21650291b9d014cd717c9c56047a4bbc4fa9f3f58185a2e3c77b0a0f908`.

Los cuatro archivos instrumentales actuales coinciden exactamente con bank_sources del manifiesto. Las fuentes de producción se interpretan como instantánea histórica; no se exige que InjectHooksMain actual coincida después de añadir otros bancos. No se encontraron defectos en los registros auditados.
