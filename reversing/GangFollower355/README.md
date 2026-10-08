# GangFollower355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Último replay sin aprobar: Funciones con diferencias: CTaskComplexGangFollower::Destructor: 256/256. No se corrigió ni se repitió por la orden de detener el trabajo. La aprobación anterior es histórica.

Funciones del alcance: `CTaskComplexGangFollower::Constructor`, `CTaskComplexGangFollower::Destructor`, `CTaskComplexGangFollower::Clone`, `CTaskComplexGangFollower::MakeAbortable`, `CTaskComplexGangFollower::CreateNextSubTask`, `CTaskComplexGangFollower::CreateFirstSubTask`, `CTaskComplexGangFollower::ControlSubTask`.

[Intento actual](attempt-summary.json); [aprobación histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-follower355/20261008-073829-258727/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexGangFollower: siete pendientes

Banco preparado de 4.352 pares: Constructor/Destructor/Clone 256, Abort512, Next/First/Control1024. Pendiente de compilación y ejecución. CalculateOffsetPosition también se reconstruyó como dependencia nativa; sigue sin hook catalogado. Compara tarea,ped,subtarea,asociación de animación,punto de marcha y asignaciones completas salvo vptr. Destructor compara estado/trazas, no EAX que el cuerpo original no define.

Fixtures positivos para crear señales, pausa, búsqueda de líder, coche, abandonar grupo, velocidad/seguimiento, exhale, IK, gestos y pasar objeto. Streaming, movimiento,grupo,FX y eventos externos se aíslan por pares EXE/DLL; Offset calcula realmente con las tablas del EXE. Reloj y estado del bloque se restauran. References comparan entidad, no dirección temporal de ranuras en pila. No valida efectos visuales ni desplazamiento real.

Root EXE validation approved4352 pairs in report20261008-013115-980305, all comparisons and required coverage gates passed. Source remains frozen pending final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-013115-980305`, 4352 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-follower355/20261008-013115-980305/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-013115-980305`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gang-follower355/20261008-013115-980305/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
