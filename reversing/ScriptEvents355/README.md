<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
