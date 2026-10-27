<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HitCar355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir`, `CTaskComplexHitPedWithCar::HitHurtsPed`, `CTaskComplexHitPedWithCar::CreateSubTask`, `CTaskComplexHitPedWithCar::Clone`, `CTaskComplexHitPedWithCar::GetTaskType`, `CTaskComplexHitPedWithCar::CreateNextSubTask`, `CTaskComplexHitPedWithCar::CreateFirstSubTask`, `CTaskComplexHitPedWithCar::ControlSubTask`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hit-car355/20261007-214720-022297/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexHitPedWithCar: ocho pendientes

Banco previsto de3.328pares, pendiente de ejecuciÃ³n. Compara tarea,ped,memoriaCD de nuevas tareas,trazas y vectores bit a bit. HitHurts/First cubren umbrales3/6/10/12/20 y velocidad de impacto zâˆ’0.8, con NaN quietos. Next cubre todos los estados y el byte de herida. Clone deja Side/MoveDir intactos como el EXE.

Los cÃ¡lculos geomÃ©tricos de lado se aÃ­slan con retornos0..3, ambas entradas EXE/DLL. No se cubren lados fuera de0..3 porque el original devuelve vector de pila indeterminado. Constructor de auxiliares real; audio/fÃ­sica/juego de colisiÃ³n no se ejecutan. Evidencia ../Tasks355Evidence.

Validación runtime aprobada: 3328/3328 pares iguales, informe 20261007-214720-022297.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261007-214720-022297`, 3328 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hit-car355/20261007-214720-022297/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261007-214720-022297`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hit-car355/20261007-214720-022297/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
