<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HudWanted355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CHud::DrawWanted`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hud-wanted355/20261008-005814-337441/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CHud wanted stars

DrawWanted 0x58D9A0 is native; hook false pending validation. Evidence Pending355/combo/CHud/0058D9A0.txt. 1024 original/candidate pairs vary fade states, changing and unchanged wanted level, parole, blink, level-change timestamp, health adjustment, dimensions and delta time. Font/GXT/player-wanted/health-placement dependencies isolated; exact drawing traces and full HUD state compared, globals restored. No pixel validation.

Coverage counts actual font configuration (background event), not the unavoidable two wanted lookups. Includes 64 legitimate inactive state=0, wanted=parole=last=0 fixtures and 960 varied active fixtures; both positive/negative gates remain required.

EXE validation approved by the root coordinator: report `20261008-005814-337441`. All required comparisons and coverage gates passed; this records the tested source before final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-005814-337441`, 1024 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hud-wanted355/20261008-005814-337441/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-005814-337441`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hud-wanted355/20261008-005814-337441/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
