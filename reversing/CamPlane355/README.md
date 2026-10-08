<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamPlane355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_DW_PlaneCam3`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-plane355/20261008-072211-668943/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamPlane355

CCam::Process_DW_PlaneCam3 — 51D100, 2048 pares pendientes; combo REA+ASM en ../World355Evidence/CCam_Process_DW_PlaneCam3.txt. Firma corregida a bool (AL original).

Fixtures: entidad nula/no vehículo/vehículo, altura mínima, inicio/reutilización de modo, frame reciente/antiguo, hit en primer/segundo LOS, deadline cero/no cero y progreso firmado negativo/positivo/infinito, contador inicializado/no inicializado y límites, salida anticipada y finalización. Mocks simétricos de datos cinematográficos, LOS, salida y finalización, sin render real; matrices y modelo aislados. Bytes completos de cámara y globals por FNV-1a64, retorno y trazas exactos. Modo, reloj, frame, tiempos, parámetros, contador, flags, modelo e ignore entity restaurados.

python reversing/CamPlane355/build_probe.py
python reversing/CamPlane355/run_local.py

Primera comparación 364/2048 diferencias: mapping de outputs core invertido en geometría. ASM usa forward para dimensión del modelo y up para desplazamiento lateral; corregido. Requiere repetición.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-042016-389545`, 2048 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-plane355/20261008-042016-389545/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-042016-389545`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-plane355/20261008-042016-389545/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
