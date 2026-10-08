<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6867 con evidencia histórica, 0 con vigencia acreditada y 1579 pendientes en 58 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamFollowCar355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_FollowCar_SA`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-follow-car355/20261008-071354-284321/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamFollowCar355

Preparado: 1.024 pares original EXE/candidato DLL de `CCam::Process_FollowCar_SA` (0x5245B0). La ejecución y aprobación quedan pendientes del coordinador; hook false.

Reconstrucción basada en pseudocódigo y ensamblador completo de `World355Evidence/CCam_Process_FollowCar_SA.txt`, EXE canónico 72ae59e4. Compara todos los bytes de CCam, vehículo, remolque, dos peatones, inteligencias, tarea, modelo de colisión, matrices, pads y doce regiones globales contiguas, además de las trazas de colaboradores. Incluye estados de automóvil, moto, helicóptero, avión, barco y RC, reinicio, zoom, remolque, conductor y pasajero, restricciones, ratón, historial y colisión; 64 casos retornan temprano por entidad no vehículo.

Los colaboradores ApplyVehicleCameraTweaks, GetVehicleAppearance, GetArrPosForVehicleType, GetColModel, GetHeading, entradas de pad, GetActiveTask, ClipBeta, GetVectorsReadyForRW y los colaboradores de colisión se sustituyen por la misma función determinista en ambos lados. GetHeightAboveRoad y GetTaskType usan tablas virtuales de fixture iguales. Los helpers matemáticos y Round4 conservan el algoritmo reconstruido; no se sustituye la función objetivo ni se usa un trampolín al EXE. Esta prueba aísla sus colaboradores y no acredita la integración completa del modo durante una partida.

`build_probe.py` conserva las fuentes y dependencias usadas. Sólo el coordinador compila y ejecuta `run_local.py`, con GTA cerrado antes de instalar el banco. El banco restaura globals, pads, bloqueo y activación del hook.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-035845-594149`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-follow-car355/20261008-035845-594149/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
