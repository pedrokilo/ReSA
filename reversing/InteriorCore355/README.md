<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::Init`, `InteriorGroup_c::AddInterior`, `InteriorGroup_c::GetPed`, `InteriorGroup_c::GetEntity`, `InteriorGroup_c::ContainsInteriorType`, `InteriorGroup_c::GetNumInteriorInfos`, `InteriorGroup_c::GetRandomInterior`, `InteriorGroup_c::FindInteriorInfo`, `InteriorGroup_c::CalcIsVisible`, `InteriorGroup_c::Setup`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-core355/20261007-204757-174574/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorCore355

Diez funciones revisadas contra combo REA/asm del EXE: Init, AddInterior, GetPed, GetEntity, ContainsInteriorType, GetNumInteriorInfos, GetRandomInterior, FindInteriorInfo, CalcIsVisible y Setup. 5120 pares (512 por función), con comparación de bytes, retornos, salidas y trazas.

Fixtures de grupos e interiores aislados: huecos y tabla llena, tipos presentes y ausentes, cantidades signed positivas/cero/negativas, información ocupada y libre, visibilidad y clasificación del grupo. RNG, visibilidad y referencia de animaciones tienen stubs pareados. CalcIsVisible no posee retorno definido en EXE; se corrige a void y se comparan sus escrituras. No cubre las otras doce funciones de InteriorGroup_c ni independencia del EXE. Aprobado: **5120 pares, cero diferencias**, ejecución `resultados-interior-core355/20261007-204757-174574/summary.json` del coordinador.

Snapshot de objetos completos mediante FNV-1a de 64 bits; outputs, punteros y trazas explícitos permanecen exactos. Las aprobaciones anteriores requieren repetición con este banco actualizado.

## Evidencia y cierre del lote 355

Último informe real: `20261007-204757-174574`; aprobado, 5120 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-core355/20261007-204757-174574/summary.json>).

Última aprobación histórica: `20261007-204757-174574`; aprobado, 5120 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-core355/20261007-204757-174574/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_Init.txt](../World355Evidence/InteriorGroup_c_Init.txt), [InteriorGroup_c_AddInterior.txt](../World355Evidence/InteriorGroup_c_AddInterior.txt), [InteriorGroup_c_GetPed.txt](../World355Evidence/InteriorGroup_c_GetPed.txt), [InteriorGroup_c_GetEntity.txt](../World355Evidence/InteriorGroup_c_GetEntity.txt), [InteriorGroup_c_ContainsInteriorType.txt](../World355Evidence/InteriorGroup_c_ContainsInteriorType.txt), [InteriorGroup_c_GetNumInteriorInfos.txt](../World355Evidence/InteriorGroup_c_GetNumInteriorInfos.txt), [InteriorGroup_c_GetRandomInterior.txt](../World355Evidence/InteriorGroup_c_GetRandomInterior.txt), [InteriorGroup_c_FindInteriorInfo.txt](../World355Evidence/InteriorGroup_c_FindInteriorInfo.txt), [InteriorGroup_c_CalcIsVisible.txt](../World355Evidence/InteriorGroup_c_CalcIsVisible.txt), [InteriorGroup_c_Setup.txt](../World355Evidence/InteriorGroup_c_Setup.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
