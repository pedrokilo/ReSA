<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamFirstPerson355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CCam::Process_1rstPersonPedOnPC`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-first-person355/20261008-071223-798393/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CamFirstPerson355

Prepared, not runtime validated. Identity: CCam::Process_1rstPersonPedOnPC at 0x50EB70, 1024 original/candidate pairs. Hook remains disabled pending parent validation.

Evidence: ../World355Evidence/CCam_Process_1rstPersonPedOnPC.txt, complete EXE pseudocode and assembly. Canonical executable SHA256: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b.

Fixtures exercise reset, existing state, non-ped and missing-render-object exits, head bob, mouse and controller inputs, upper/lower pitch limits, player attachment directions/limits, sniper zoom-in/zoom-out and FOV limits. Skeletal lookup/transformation, control inputs, angle wrapping, camera vectors and near clipping have paired deterministic dependencies and ordered traces. Placeable rotation and RenderWare frame updates execute on fake matrices and frames. Those dependent math implementations are included in build hashes.

The bank compares complete CCam, CPed, CMatrix, frame, bone matrix and RwCamera bytes, plus contiguous camera/static/input/pad globals and call traces. Every pair resets all owned globals from saved state, and the original state and hook lock/state are restored after completion. It does not validate animation hierarchy internals or input-device behavior.

Root exclusively compiles and runs GTA. Build/run scripts use probe_build and probe_case; source hashes cover Cam, entity/placeable/ped/matrix/vector, pad and RenderWare helpers. Runtime result remains pending.

## Evidencia y cierre del lote 355

No hay una aprobación histórica registrada en los informes disponibles de este banco.

Último informe disponible: `20261008-033917-157115`; resultado del banco: no aprobado. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cam-first-person355/20261008-033917-157115/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
