<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamMan355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_DW_CamManCam`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-man355/20261008-072038-960522/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamMan355

Process_DW_CamManCam (`0x51B120`), 4096 pares pendientes de ejecución EXE/DLL. Reconstrucción C++ completa con helper x87 para easing, desde `../World355Evidence/CCam_Process_DW_CamManCam.txt`. Firma bool corregida contra retorno EXE.

Fixtures: target ausente/no vehículo/vehículo, inicio/continuación, flags inicializados, cero/cuatro farolas, flags entidad, inclinación y modelo elegible, distancia y visibilidad, posición cacheada, interpolación FOV, salida, contador LOS, shake min/0/1. Colaboradores simétricos sin mundo/render; FNV1a64 de todos los bytes de cámara y estados globales, trace exacta. Parámetros/counters/modelos/times/ignore restaurados. Pares 0..4095.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-040747-580541`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-man355/20261008-040747-580541/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
