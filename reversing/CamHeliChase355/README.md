<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamHeliChase355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_DW_HeliChaseCam`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-heli-chase355/20261008-071711-112076/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamHeliChase355

Process_DW_HeliChaseCam (`0x51A740`), 16384 pares EXE/DLL pendientes. Reconstrucción C++ completa de selección/init/trials, orientación y predicción, interpolation/FOV/roll, sphere damp, LOS counters/frozen target/return FOV/exit/final. Firma bool(bool) corregida. Evidencia `../World355Evidence/CCam_Process_DW_HeliChaseCam.txt`.

Fixture aislada de una entidad y estado completo0x9C, ramas sin target/tipo ped/vehicle, init y persistencia, intentos0..3, radios, tiempos, contadores/sphere/LOS/return. Stubs simétricos de coredata, state helpers, handshaker, random, sphere, LOS, exit/final. FNV1a64 de todos los bytes de cámara/entidad/estado/times/scratch/parametros y trace/salida exacta; todos los globals restaurados. Pares0..16383. La selección original usa el cero mutable0x858B50, conservado; no se fuerzan estados fuera de su rango canónico.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-040105-067632`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-heli-chase355/20261008-040105-067632/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
