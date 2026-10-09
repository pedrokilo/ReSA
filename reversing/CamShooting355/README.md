<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamShooting355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_Cam_TwoPlayer_InCarAndShooting`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-shooting355/20261008-072455-211741/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamShooting355

Prepared, not runtime validated: CCam::Process_Cam_TwoPlayer_InCarAndShooting (0x519810), 1024 original/candidate pairs. Hook remains disabled. Evidence: complete canonical EXE pseudocode and assembly in ../World355Evidence/CCam_Process_Cam_TwoPlayer_InCarAndShooting.txt.

Fixtures exercise the non-vehicle exit, both driver/shooter selections, vehicle subclasses and appearance/status, speed/FOV adjustment and clamps, target locking and free aim, horizontal/vertical targeting limits, neutral return, vehicle bounds and camera radii, pitch limits, geometry correction, aim-target/free-shot endpoints, weapon readiness, leaning/melee/fire gates, passenger seat choice, drive-by direction and camera shake. Full cam/ped/vehicle/matrix/model/col-model/weapon-info/aim-target bytes and contiguous camera/player/model-pointer globals are compared with ordered dependency traces.

Sight targeting, weapon skill/info, vehicle appearance/heading/leaning, camera rotation/vectors/geometry, pad input, weapon update, drive-by processing and shake are isolated symmetrically. Temporary drive-by construction/destruction run against their existing implementations; defined firing fields are captured at ProcessPed. Native camera arithmetic and RenderWare-independent geometry run in each version.

CCamera::AvoidTheGeometry has only a declaration in the DLL repository. The new routine retains this external dependency's original 0x514030 calling convention through the EXE entry, as other camera routines do. The bank patches that shared EXE entry for both versions; it does not claim to reconstruct or validate AvoidTheGeometry, collision, weapon targeting or drive-by task internals. All globals, timestep and hook state/lock are restored. Root exclusively builds and runs GTA serially.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-034310-258485`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-shooting355/20261008-034310-258485/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.

Dependencia de disparo: el EXE invoca directamente al colaborador no catalogado `0x627CC0` en `0x51A3FD`. La corrección preparada usa esa misma dependencia canónica en ambos caminos, aislada simétricamente por el banco. `CTaskSimpleGangDriveBy::ProcessPed` (`0x62D3B0`) no es ese colaborador y su aislamiento se elimina en el borrador del instrumento; los parches de fuente e instrumento siguen pendientes de aplicación y repetición coordinadas por la raíz.
