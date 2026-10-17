<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamMouse355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_FollowPedWithMouse`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-mouse355/20261008-072058-940646/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamMouse355

Prepared, not runtime validated: CCam::Process_FollowPedWithMouse at 0x50F970, 1024 original/candidate pairs. Hook disabled. Main evidence: ../World355Evidence/CCam_Process_FollowPedWithMouse.txt. Additional native stick helpers 0x540E80/0x540F80 have complete REA plus assembly in ../Tasks355Evidence/CCamExtra_StickHorizontal.txt and CCamExtra_StickVertical.txt.

Fixtures exercise non-ped exit, reset/mouse-history, mouse and controller input, stick thresholds and int16 endpoints/inversion, recenter and pitch transitions, beta/pitch clipping, look-front/look-behind overrides, special player vehicle, clear/ped/object sight and second collision, near clipping and 0..6 sphere iterations, distance easing and player heading updates. Skeletal stubs inherited from the first-person bank are unused here. Used dependencies are isolated symmetrically: clear mouse history, player vehicle, third-person mode, sight/sphere collision, clip/vectors and near plane. Both versions execute the native stick math against saved pad globals.

Full CCam, CPed, CMatrix/frame, fake vehicle/collision entity, RwCamera and thirteen contiguous global regions are compared with ordered traces and query counts. Globals include Scene camera, input/inversion, player/camera state, collision test point and ignore entity. Each pair resets state and the original bytes and hook state/lock are restored after completion. Input-device, world collision and camera-vector internals are outside scope. Root exclusively compiles and runs GTA serially.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-034436-556581`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-mouse355/20261008-034436-556581/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
