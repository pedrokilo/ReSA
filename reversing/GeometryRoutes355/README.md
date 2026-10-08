# GeometryRoutes355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPedGeometryAnalyser::ComputeEntityBoundingBoxCornersUncached`, `CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox-1`, `CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox-2`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-routes355/20261008-024532-669655/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# GeometryRoutes355

3072 pares preparados: corners 1024, ambas rutas 1024 cada una. Corners usa matrices orientadas por tres ejes, cajas altas con selección por Z, esferas, cajas y fallback de InteriorManager, con cuatro vectores completos. Rutas incluyen puntos en ocho posiciones, elección automática y forzada, obstáculos vehículo/ped, ignorados ped/entidad, ruta completa incluyendo entradas inactivas y padding restaurado. Mundo/InteriorManager aislados por pares; rutas aíslan corners y bounding sphere y ejecutan realmente sus decisiones y plano/segmento. Devuelven memoria completa y trazas; no comprueba navegación física en mapa. Pendiente de ejecución. Hooks false.

Coordinator run `20261008-024532-669655` approved 3072 pairs, all comparisons equal. Final source/manifest replay remains pending.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-024532-669655`, 3072 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-routes355/20261008-024532-669655/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-024532-669655`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-routes355/20261008-024532-669655/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
