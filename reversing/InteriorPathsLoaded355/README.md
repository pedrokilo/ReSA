<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorPathsLoaded355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::ArePathsLoaded`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-paths-loaded355/20261008-020154-615014/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorPathsLoaded355

ArePathsLoaded: 1024 pares. Combo completo. Ocho esquinas de colbox, matriz de entidad no nula, shear/traslación, min/max y consulta simétrica AreNodesLoadedForArea. Tabla model2000 restaurada; FNV64 objetos completos. La rama de asignación de matriz no está cubierta aún. Pendiente raíz.

## Evidencia y cierre del lote 355

Último informe real: `20261008-020154-615014`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-paths-loaded355/20261008-020154-615014/summary.json>).

Última aprobación histórica: `20261008-020154-615014`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-paths-loaded355/20261008-020154-615014/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_ArePathsLoaded.txt](../World355Evidence/InteriorGroup_c_ArePathsLoaded.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
