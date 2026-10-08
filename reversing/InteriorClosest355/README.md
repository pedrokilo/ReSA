# InteriorClosest355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `InteriorGroup_c::FindClosestInteriorInfo`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-closest355/20261008-020114-348640/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InteriorClosest355

FindClosestInteriorInfo: 2048 pares. Combo pseudo+ASM completo; conteos y tipos int8, interiores nulos, IsPtInside simétrico, infos usados, radio negativo/cero/NaN/infinito, salida intacta en fallo y distancia cuadrada con orden z+y+x. Pendiente compilación/validación raíz.

## Evidencia y cierre del lote 355

Último informe real: `20261008-020114-348640`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-closest355/20261008-020114-348640/summary.json>).

Última aprobación histórica: `20261008-020114-348640`; aprobado, 2048 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-closest355/20261008-020114-348640/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [InteriorGroup_c_FindClosestInteriorInfo.txt](../World355Evidence/InteriorGroup_c_FindClosestInteriorInfo.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
