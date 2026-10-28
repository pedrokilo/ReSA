<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FormationFollow355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Replay actual aprobado para este banco: fuentes, instrumentos, binarios archivados y hashes del EXE/DLL ejecutados comprobados. Forma parte de un cierre parcial; el lote completo no quedó validado con las fuentes finales.

Funciones del alcance: `CTaskComplexFollowLeaderInFormation::Constructor`, `CTaskComplexFollowLeaderInFormation::Clone`, `CTaskComplexFollowLeaderInFormation::GetTaskType`, `CTaskComplexFollowLeaderInFormation::CreateNextSubTask`, `CTaskComplexFollowLeaderInFormation::CreateFirstSubTask`, `CTaskComplexFollowLeaderInFormation::ControlSubTask`.

[Resumen registrado](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-formation-follow355/20261008-073454-266158/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexFollowLeaderInFormation: seis pendientes

Banco preparado de 3.328 pares: Constructor/Clone/Type 256, Next 1024, First 512, Control 1024. Pendiente de ejecución. Compara tarea, ped, líder, subtarea completa y asignaciones, incluidos contador/radio de SeekEntity y pausa al separarse del líder en vehículo. Se reconstruyó CreateSubTask como dependencia nativa; se aísla en las transiciones. La creación directa del pasajero y pausa sigue real.

Fixture de modelo de colisión, vehículo y grupo; restaura modelo y reloj. ProcessPed de StandStill y búsqueda de grupo se aíslan por pares. No valida movimiento físico ni la geometría del seguimiento en ejecución real. Constructor hook antes devolvía this sin construir: ahora ejecuta el constructor nativo completo.

Fixture corrections for repeat: leader pointers remain valid in methods that unconditionally dereference them; null leaders remain covered in Control. The StandStill ProcessPed dependency is isolated at both canonical EXE entry and actual DLL hook body, avoiding a virtual thunk/devirtualized-call mismatch. Positive process_stand/abort/allocation coverage requirements are unchanged.

Coordinator result: 20261008-014928-809237 approved 3328 pairs after real StandStill ProcessPed isolation. Final manifest repeat remains pending.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-014928-809237`, 3328 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-formation-follow355/20261008-014928-809237/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-014928-809237`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-formation-follow355/20261008-014928-809237/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
