<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7193 con evidencia histórica, 0 con vigencia acreditada y 1253 pendientes en 23 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# VehicleLean355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTaskGoToVehicleAndLean::Clone`, `CTaskGoToVehicleAndLean::MakeAbortable`, `CTaskGoToVehicleAndLean::CreateNextSubTask`, `CTaskGoToVehicleAndLean::CreateFirstSubTask`, `CTaskGoToVehicleAndLean::ControlSubTask`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-vehicle-lean355/20261008-011309-582333/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskGoToVehicleAndLean: cinco pendientes

Banco preparado de 3.840 pares, pendiente de ejecución. Clone 256, Abort 512 y Next/First/Control 1024 cada una. Se corrigió el tamaño ABI a 0x24, con vector objetivo en +0x18 que Clone no inicializa. Compara tarea, ped, vehículo, asignaciones completas salvo vptr y trazas. Incluye vehículo nulo/reservado, doble interrupción, contador 30/31, estados de subtarea y carga de gangs.

CalcTargetPos se reconstruyó como dependencia nativa, pero sus resultados y GetSpeed se aíslan en las comparaciones de transición; no se afirma validar geometría ni física real. El estado de carga del bloque se restaura. Evidencia REA y ASM en ../Tasks355Evidence y ../_handoff/rea/rea-x86/tasks355-pseudo.

EXE validation approved by the root coordinator: report `20261008-011309-582333`. All required comparisons and coverage gates passed; this records the tested source before final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-011309-582333`, 3840 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-vehicle-lean355/20261008-011309-582333/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-011309-582333`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-vehicle-lean355/20261008-011309-582333/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
