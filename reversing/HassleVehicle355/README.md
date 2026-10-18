<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HassleVehicle355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTaskGangHassleVehicle::GetTargetHeading`, `CTaskGangHassleVehicle::CalcTargetOffset`, `CTaskGangHassleVehicle::Clone`, `CTaskGangHassleVehicle::CreateNextSubTask`, `CTaskGangHassleVehicle::CreateFirstSubTask`, `CTaskGangHassleVehicle::ControlSubTask`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hassle-vehicle355/20261008-013029-119265/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskGangHassleVehicle: seis pendientes

Banco preparado de 4.352 pares: Heading/Offset 512, Clone 256, Next/First/Control 1024 cada una. Pendiente de compilación y ejecución. Control cubre reserva y referencia, carga de gangs, temporizador, amenaza al conductor, giro, voz, gestos secundarios, señal de mano y golpe al vehículo. m_b31 es byte con estados0..3, no bool. Next conserva reserva0x58 de TrackEntity.

Se comparan tarea,ped,vehículo,subtarea,asignaciones completas salvo vptr,trazas y heading bit a bit. CalcTargetOffset se ejecuta real; First conserva el modelo de colisión ficticio. El banco restaura modelo,reloj y carga del bloque. Visibilidad, IK,voz, fuerzas y colisión se aíslan por entrada EXE/DLL; la construcción local de Fight es real, sus helpers externos están aislados. No valida daño físico ni animación reales. Reference/Cleanup comparan entidad: la dirección de la ranura de referencias temporales en pila no es estable entre EXE y DLL.

Root EXE validation approved4352 pairs in report20261008-013029-119265, all comparisons and required coverage gates passed. Source remains frozen pending final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-013029-119265`, 4352 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hassle-vehicle355/20261008-013029-119265/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-013029-119265`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hassle-vehicle355/20261008-013029-119265/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
