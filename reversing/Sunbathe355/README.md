<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sunbathe355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTaskComplexSunbathe::Clone`, `CTaskComplexSunbathe::GetTaskType`, `CTaskComplexSunbathe::MakeAbortable`, `CTaskComplexSunbathe::CreateNextSubTask`, `CTaskComplexSunbathe::CreateFirstSubTask`, `CTaskComplexSunbathe::ControlSubTask`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-sunbathe355/20261008-012922-194921/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexSunbathe: seis pendientes

Banco preparado de 4.096 pares: Clone 256, GetTaskType 256, MakeAbortable 512, Next 1024, First 1024 y Control 1024. Pendiente de compilación y ejecución. Se comparan retorno, memoria de tarea y ped, asignaciones y trazas. Incluye horas, 22 tipos de clima, carga de bloques, interrupciones, estados de animación y vehículo del jugador; restaura reloj, clima y jugador.

CreateSubTask se reconstruye como dependencia nativa pero se aísla al probar las transiciones. Clone usa constructor real. No se afirma validar reproducción de animaciones o streaming real. Pseudocódigo REA recuperado con rangos completos en ../_handoff/rea/rea-x86/tasks355-pseudo y ASM en ../Tasks355Evidence.

Root EXE validation approved4096 pairs in report20261008-012922-194921, including64 positive Control fixtures that exercise abort under disallowed weather. All comparison and coverage gates passed; source remains frozen pending final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-012922-194921`, 4096 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-sunbathe355/20261008-012922-194921/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-012922-194921`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-sunbathe355/20261008-012922-194921/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
