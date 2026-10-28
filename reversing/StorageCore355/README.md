<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# StorageCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CGenericGameStorage::GetCurrentVersionNumber`, `CGenericGameStorage::MakeValidSaveName`, `CGenericGameStorage::CloseFile`, `CGenericGameStorage::OpenFileForWriting`, `CGenericGameStorage::OpenFileForReading`, `CGenericGameStorage::GetSavedGameDateAndTime`, `CGenericGameStorage::InitRadioStationPositionList`, `CGenericGameStorage::RestoreForStartLoad`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-storage-core355/20261007-205706-515796/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# StorageCore355

Ocho entradas revisadas desde combo REA/asm. 4096 pares (512/función): versión, nombre de partida, cierre/apertura escritura/lectura, fecha, inicialización de radio y restauración inicial (las últimas dos son NOP/false en EXE). Se corrige MakeValidSaveName: la DLL anterior añadía dos veces .b. Se preservan bytes no escritos y conversión de ? hasta nueva línea.

Archivo y asignación tienen stubs pareados; nunca se abre un archivo real. Se comparan cadenas completas, estado global, retorno y trazas de apertura/tamaño/cierre/asignación. Tablas y buffer originales se restauran. Pendiente de compilar y ejecutar por coordinador; no cubre las otras seis entradas de almacenamiento ni independencia del EXE.

Aprobado: 4096 pares, 0 diferencias. Informe resultados-storage-core355/20261007-205706-515796/summary.json. Vigencia final sujeta al hash del manifiesto después de StorageGame355.

Aprobado: 4096 pares, 0 diferencias. Informe resultados-storage-core355/20261007-205706-515796/summary.json. Vigencia final sujeta al hash del manifiesto después de StorageGame355.

Aprobado: 4096 pares, 0 diferencias. Informe resultados-storage-core355/20261007-205706-515796/summary.json. Vigencia final sujeta al hash del manifiesto después de StorageGame355.

## Evidencia y cierre del lote 355

Último informe real: `20261007-205706-515796`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-storage-core355/20261007-205706-515796/summary.json>).

Última aprobación histórica: `20261007-205706-515796`; aprobado, 4096 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-storage-core355/20261007-205706-515796/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CGenericGameStorage_GetCurrentVersionNumber.txt](../World355Evidence/CGenericGameStorage_GetCurrentVersionNumber.txt), [CGenericGameStorage_MakeValidSaveName.txt](../World355Evidence/CGenericGameStorage_MakeValidSaveName.txt), [CGenericGameStorage_CloseFile.txt](../World355Evidence/CGenericGameStorage_CloseFile.txt), [CGenericGameStorage_OpenFileForWriting.txt](../World355Evidence/CGenericGameStorage_OpenFileForWriting.txt), [CGenericGameStorage_OpenFileForReading.txt](../World355Evidence/CGenericGameStorage_OpenFileForReading.txt), [CGenericGameStorage_GetSavedGameDateAndTime.txt](../World355Evidence/CGenericGameStorage_GetSavedGameDateAndTime.txt), [CGenericGameStorage_InitRadioStationPositionList.txt](../World355Evidence/CGenericGameStorage_InitRadioStationPositionList.txt), [CGenericGameStorage_RestoreForStartLoad.txt](../World355Evidence/CGenericGameStorage_RestoreForStartLoad.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
