<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorPlacement — colocación de muebles y objetos

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-093441-019284`, 4096 pares. El alcance se limita a los fixtures y colaboradores documentados.

El banco prevé 4096 pares contiguos: 1024 para cada función PlaceFurniture (592AA0), PlaceFurnitureOnWall (593120), PlaceFurnitureInCorner (593340) y PlaceObject (5934E0). Compara llamadas ordenadas, componentes de matrices como bits float, Interior completo 0x794, nodos/listas/pool, bytes de entidad, salidas, retornos y almacén de datos de robo completo (contador y 32 registros: 900 bytes). No se oculta ningún byte de puntero/vtable. Las identidades conocidas de receptores/entidades/muebles se exportan con desplazamiento; las desconocidas mantienen el valor bruto.

Los colaboradores de inserción/remoción World, asignación de modelo, inicialización de objeto, asignación de memoria y asignación de matriz del constructor son fixtures simétricos. Sus implementaciones se excluyen de certificación. Los callbacks de modelo e Init establecen bytes completos y vtable sintética compartida antes de las escrituras del objetivo. El EXE usa slot 6 (+18h) SetModelIndexNoCreate 533700; slot 5 es SetModelIndex 532AE0. No confundir el nombre interno Model del callback sintético con la función real invocada.

Wall/Corner sustituyen PlaceFurniture por un colaborador determinista; PlaceFurniture se compara por separado con operaciones reales de matrices y listas. El fixture de robo usa un bit independiente `(i/4)%2`: escenarios mod8=4 crean datos de robo, 5 ejercen búsqueda negativa, 6 permiten eliminación por robo previo y 7 búsqueda positiva sin robo. El fixture anterior `i%2` hacía inalcanzables las ramas de creación y eliminación. Los últimos escenarios también incluyen precisión fraccionaria, coordenadas amplias y poolcount negativo según la propuesta integrada por raíz.

La herramienta de lectura `../InteriorPlacementCoverage.py <directorio-del-informe>` calcula cobertura observada sin modificar resultados ni igualdad. No verifica manifiestos ni impone gates adicionales. Las clases del pool describen el estado posterior observado; los signos de búsqueda se infieren del contrato determinista del callback. En el informe final se revisan además las ramas positivas/nulas, los reintentos y la creación/eliminación de datos de robo.

Límites: no se certifican creación real de entidades/recursos, pools del juego, carga de modelos, algoritmos de los colaboradores aislados ni integración de amueblado completo. La comparación completa de matrices y estados sólo certifica los cuerpos bajo el contrato explícito de estos fixtures. Se exige repetir el banco con las correcciones finales; los tres pares nulos históricos no bastan.

Evidencia: `../InteriorEvidence/Interior_PlaceFurniture.txt`, `Interior_PlaceFurnitureOnWall.txt`, `Interior_PlaceFurnitureInCorner.txt` y `Interior_PlaceObject.txt` (REA y ASM del EXE canónico SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`). Compilación y GTA corresponden exclusivamente al coordinador.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-093441-019284`: 4,096 pairs across 4 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
