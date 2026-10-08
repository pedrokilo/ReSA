<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamPlaneOrbit355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_DW_PlaneCam1`, `CCam::Process_DW_PlaneCam2`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-plane-orbit355/20261008-072232-818049/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamPlaneOrbit355

PlaneCam1 y PlaneCam2, 4096 pares en total. Reconstrucción combo REA y ASM, incluyendo enteros temporales firmados, seno x87 y globals de selección/visibilidad. Stubs simétricos para core, LOS, azar, distancia a suelo, salida y finalización; fixtures aisladas. FNV-1a64 sobre CCam y todas las ventanas globales, outputs/trazas exactos. Pendiente de compilación y comparación EXE por coordinador.

Aprobado 4096 pares por coordinador, informe `20261007-221638-490681`. Fuente compartida cambió para corregir PlaneCam3; repetir según hash final.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261007-221638-490681`, 4096 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-plane-orbit355/20261007-221638-490681/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261007-221638-490681`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-plane-orbit355/20261007-221638-490681/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
