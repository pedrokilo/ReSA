<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Wander355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CTaskComplexWanderStandard::WillChat`, `CTaskComplexWanderStandard::SetNextMinScanTime`, `CTaskComplexWanderStandard::LookForSexyCars`, `CTaskComplexWanderStandard::LookForChatPartners`, `CTaskComplexWanderStandard::LookForGangMembers`, `CTaskComplexWanderStandard::Clone`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-wander355/20261008-011104-463648/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CTaskComplexWanderStandard: seis pendientes

2.944pares previstos; pendiente de ejecución. Incluye WillChat stdcall, tiempo mínimo de escaneo, SexyCars/ChatPartners/GangMembers con fixtures positivos y negativos, y Clone sensible. Fake peds/inteligencia,16slots scanner con huecos, matrices y vehículo se comparan; eventos se inspeccionan semánticamente y referencias se trazan.

Se aíslan amistad, estado de pavement, streaming/riots, tareas activas, eventos anteriores, visibilidad, grupos y destino de nodo; no se ejecuta navegación ni población real. Eventos temporales conservan contenido definido; no se compara relleno de pila no inicializado. Jugador, foco y tiempo se restauran. Evidencia ../Tasks355Evidence.

EXE validation approved by the root coordinator: report `20261008-011104-463648`. All required comparisons and coverage gates passed; this records the tested source before final global hook activation.

## Evidencia y cierre del lote 355

Aprobación histórica: `20261008-011104-463648`, 3072 pares. Informe: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-wander355/20261008-011104-463648/summary.json>). Corresponde al estado de fuentes e instrumentos de esa ejecución.

Último informe disponible: `20261008-011104-463648`; resultado del banco: aprobado en ese snapshot. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-wander355/20261008-011104-463648/summary.json>).

Validación final del lote pendiente: una aprobación histórica no acredita las fuentes actuales. El cierre requiere repetir los bancos afectados y comprobar los hashes actuales de fuentes, instrumentos, EXE y ASI ejecutada mediante el registro estricto coordinado por la raíz. Este README no autoriza activar hooks.

La equivalencia está limitada a los fixtures y ramas descritos arriba. Las colaboraciones aisladas simétricamente no acreditan su implementación; estos resultados no constituyen una campaña integral de juego ni independencia del EXE.
