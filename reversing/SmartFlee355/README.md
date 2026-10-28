<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SmartFlee355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTaskComplexSmartFleePoint::SetDefaultTaskWanderDir`, `CTaskComplexSmartFleePoint::ComputeFleeDir`, `CTaskComplexSmartFleePoint::CreateSubTask`, `CTaskComplexSmartFleePoint::SetFleePosition`, `CTaskComplexSmartFleePoint::Clone`, `CTaskComplexSmartFleePoint::GetTaskType`, `CTaskComplexSmartFleePoint::MakeAbortable`, `CTaskComplexSmartFleePoint::CreateNextSubTask`, `CTaskComplexSmartFleePoint::CreateFirstSubTask`, `CTaskComplexSmartFleePoint::ControlSubTask`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-smart-flee355/20261008-011551-091985/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexSmartFleePoint: diez pendientes

Banco preparado para4.864pares; pendiente de ejecuciÃ³n. Fixtures comparan memoria de tareas/ped/subtarea/wander predeterminado y asignacionesCD. Auxiliares de direcciÃ³n y actualizaciÃ³n de posiciÃ³n se prueban directamente; retornos de dos mÃ©todos void del EXE fueron corregidos en la firma DLL y no se atribuye valor al EAX indeterminado.

Next/First/Control aÃ­slan la creaciÃ³n de subtareas y recorren coche, timer, nuevo objetivo, distancia segura, orientaciÃ³n, grito y mirada. CreateSub construye tareas reales. Timer IsOutOfTime, audio, mirada y RNG se aÃ­slan; no se valida reproducciÃ³n ni navegaciÃ³n real. Todos los globals temporales son restaurados. Evidencia en ../Tasks355Evidence; falta confirmar paridad en ejecuciÃ³n.

Primer ensayo: 20261007-210929-051886, nueve funciones iguales; MakeAbortable 341/512 diferencias por usar prioridad2 en vez de0 para reiniciar el temporizador. Corregido contra test eax,eax del EXE; pendiente repeticiÃ³n completa.

Repetición 20261007-214809-301461: 4864 comparaciones iguales, sin diferencias. Gate positivo LookAt no cubierto porque la secuencia RNG estaba correlacionada con subtipo/IsLooking. Fixture escenario 80 ahora devuelve RNG cero (wander + RUN + no looking) para ejercer la rama sin relajar el gate. Pendiente de repetir.

EXE validation approved by the root coordinator: report `20261008-011551-091985`. All required comparisons and coverage gates passed; this records the tested source before final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-011551-091985`, 4864 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-smart-flee355/20261008-011551-091985/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-011551-091985`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-smart-flee355/20261008-011551-091985/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
