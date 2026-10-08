<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GeometryQueries355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPedGeometryAnalyser::CanPedJumpObstacle-`, `CPedGeometryAnalyser::CanPedJumpObstacle-contacted`, `CPedGeometryAnalyser::CanPedTargetPoint`, `CPedGeometryAnalyser::ComputeBuildingHitPoints`, `CPedGeometryAnalyser::ComputeClearTarget`, `CPedGeometryAnalyser::ComputeClosestSurfacePoint-posn`, `CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes`, `CPedGeometryAnalyser::ComputeEntityBoundingSphere`, `CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity`, `CPedGeometryAnalyser::ComputeEntityHitSide-1`, `CPedGeometryAnalyser::ComputeEntityHitSide-3`, `CPedGeometryAnalyser::ComputePedHitSide-physical`, `CPedGeometryAnalyser::ComputeRouteRoundSphere`, `CPedGeometryAnalyser::GetIsLineOfSightClear-ped`, `CPedGeometryAnalyser::GetIsLineOfSightClear-v3d`, `CPedGeometryAnalyser::GetNearestPed`, `CPedGeometryAnalyser::IsEntityBlockingTarget`, `CPedGeometryAnalyser::IsInAir`, `CPedGeometryAnalyser::IsWanderPathClear`, `CPedGeometryAnalyser::LiesInsideBoundingBox`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-queries355/20261008-022611-416976/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# GeometryQueries355

20 consultas pendientes, 12032 pares preparados. Compara retornos, vectores/planos/esfera, ped, entidad, ambas matrices, inteligencia, pool y trazas de dependencias completas. Fixtures positivos de LOS, contacto, colisión, agua, altura, tareas y escáneres. Pool aislado de cuatro slots reales de 0x7C4 y mapas ocupado/vacío, restaura pool/modelo/padding/cruces. Dependencias mundo/collision y generación de corners aisladas simétricamente; ClearTarget también aísla pruebas de entidad y segmento. Las operaciones geométricas restantes son reales. No valida streaming del mundo ni animaciones. Pendiente de ejecución; hooks false.

Coordinator run `20261008-022611-416976` approved 12032 pairs, all comparisons equal. Final source/manifest replay remains pending.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-022611-416976`, 12032 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-queries355/20261008-022611-416976/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-022611-416976`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-queries355/20261008-022611-416976/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
