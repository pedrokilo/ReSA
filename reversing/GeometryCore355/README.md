<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# GeometryCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CPedGeometryAnalyser::ComputeEntityDirs`, `CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncached`, `CPedGeometryAnalyser::ComputeEntityHitSide-2`, `CPedGeometryAnalyser::ComputeClosestSurfacePoint-rect`, `CPedGeometryAnalyser::ComputePedHitSide-posn`, `CPedGeometryAnalyser::ComputePedShotSide`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-core355/20261008-073932-615814/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# GeometryCore355

4608 pares preparados: Dirs y SegmentPlanes 512, HitSide y ClosestSurfacePoint 1024, PedHitSide 512 y ShotSide 1024. Compara retornos, cuatro vectores de salida, planos/dots, entradas y ped/matriz completos. Usa rectángulos positivos, planos por cuadrantes, casos proyectados y de extremos, aristas degeneradas y posición/matriz alternativa. PedHitSide aísla ComputeEntityDirs por ambos lados; las demás operaciones son reales. ShotSide mantiene atan x87 y truncación del CRT del EXE como dependencia externa. Hooks false hasta aprobación runtime. Evidencia completa en Tasks355Evidence; no valida rutas, obstáculos ni consultas físicas.

Coordinator run `20261008-022531-655219` approved 4608 pairs, all comparisons equal. Final source/manifest replay remains pending.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-022531-655219`, 4608 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-core355/20261008-022531-655219/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-022531-655219`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-geometry-core355/20261008-022531-655219/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
