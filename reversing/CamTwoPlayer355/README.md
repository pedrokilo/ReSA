<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamTwoPlayer355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_Cam_TwoPlayer`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-two-player355/20261008-072759-721606/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamTwoPlayer355

Prepared bank; runtime validation pending. CCam::Process_Cam_TwoPlayer (0x525E50), 1024 original/candidate pairs. Hook disabled. Main routine evidence is complete ../World355Evidence/CCam_Process_Cam_TwoPlayer.txt. Native reconstruction also includes internal geometry (0x5132D0), visibility (0x513220), and camera-button edge detection (0x5404A0); supplemental REA plus complete assembly recovered in ../Tasks355Evidence/CCamExtra_Visible.txt, CCamExtra_Geometry.txt and CCamExtra_ChangeCamera.txt.

Fixtures exercise all three cooperative camera choices and button transitions, car/ped dispatch, both reset states, 0..21 visibility search attempts, successful/all-blocked sight, velocity and controller corrections, timer windows, beta wrapping and clamps, focus interpolation, collision fallback and warning throttling. Two fake peds and optional matrices/vehicles cover placement access. The internal geometry runs natively in each version. World sight, follow-car/follow-ped dispatch, pad aiming and warning dependencies are isolated symmetrically with ordered argument traces.

Complete camera, ped, vehicle, matrix, model and collision-model bytes are compared, together with contiguous player/model-pointer/pad/static/time regions. Every pair resets globals; all original global bytes, timestep and hook state/lock are restored at completion. Vehicle/ped following internals, world collision and input-device behavior are outside this bank. Root exclusively builds and runs GTA serially.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-034201-273970`, 1024 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-two-player355/20261008-034201-273970/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-034201-273970`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-two-player355/20261008-034201-273970/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
