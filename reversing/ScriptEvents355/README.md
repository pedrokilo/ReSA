<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptEvents355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CAEScriptAudioEntity::ProcessMissionAudioEvent`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-events355/20261008-010403-174215/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CAEScriptAudioEntity::ProcessMissionAudioEvent

Reconstrucción de `0x4ECCF0` mediante pseudocódigo REA y ensamblador completo de `VehicleAudio355Evidence/Script_EventsFull.txt`. Se despacha por los 16 bits inferiores del evento conservando el identificador original para las llamadas que reciben 32 bits. Se mantienen la alternancia con signo del byte `0x7E`, el vínculo físico de grúa en `0x110` y las escrituras de volumen, velocidad y reloj.

El banco compara 6.176 pares EXE/DLL: eventos 1000–1190, grúa 104 y un evento desconocido, cuatro prefijos altos, campos de control, extremos de aleatoriedad, resultados positivos y negativos de consultas, entidad nula, distintos tipos y posición con o sin matriz. Se comparan los bytes completos del objeto, entidad, matriz y posición, además de la secuencia y argumentos de las colaboraciones.

Las colaboraciones de reproducción, hardware, gestor de sonidos, ambientes, frontend, armas, agua, puertas, explosión y audio de ped se sustituyen simétricamente por sondas deterministas; este banco verifica el despacho y el estado de esta función. Sus implementaciones se validan en bancos independientes. Se restauran reloj, tabla de volumen y hooks al finalizar; el ejecutor restaura los archivos del juego y lo cierra.

Estado: preparado; la aprobación requiere que el informe de ejecución no contenga diferencias y coincida con el manifiesto de la compilación y las fuentes actuales.

## Evidencia y cierre del lote 355

Último informe real: `20261008-010403-174215`; aprobado, 6176 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-events355/20261008-010403-174215/summary.json>).

Última aprobación histórica: `20261008-010403-174215`; aprobado, 6176 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-events355/20261008-010403-174215/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [Script_4ECCF0.txt](../VehicleAudio355Evidence/Script_4ECCF0.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
