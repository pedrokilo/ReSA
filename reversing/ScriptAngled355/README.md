<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptAngled355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::CharInAngledAreaCheckCommand`, `CRunningScript::FlameInAngledAreaCheckCommand`, `CRunningScript::ObjectInAngledAreaCheckCommand`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-angled355/20261008-024131-774854/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptAngled355

CRunningScript: CharInAngledAreaCheckCommand (487F60), ObjectInAngledAreaCheckCommand (4883F0), FlameInAngledAreaCheckCommand (488780). 2048 pares por función, 6144 total; pendientes de ejecución. Pseudocódigo REA y ASM completo en ../World355Evidence/CRunningScript_*.txt.

Comandos 2D/3D, 12 variantes del personaje, parada/movimiento, vehículo/nulo, anchos positivos/negativos/cero, extremos invertidos y coincidentes, distintas orientaciones, alturas invertidas, dibujo con flag negativo-cero, posiciones dentro/fuera y disparos activos/inactivos con salida temprana. Trigonometría conserva productos x87 sin redondeos extra. Colaboradores geométricos reales; mocks simétricos de parámetros, comparación, resaltado, parada y posición de llamas. Todos los bytes de objetos/script/parámetros se comparan mediante FNV-1a64 y trazas exactas; pools, parámetros y temporizador se restauran. Sin eventos ni render real.

python reversing/ScriptAngled355/build_probe.py
python reversing/ScriptAngled355/run_local.py

## Evidencia y cierre del lote 355

Último informe real: `20261008-024131-774854`; aprobado, 6144 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-angled355/20261008-024131-774854/summary.json>).

Última aprobación histórica: `20261008-024131-774854`; aprobado, 6144 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-angled355/20261008-024131-774854/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_CharInAngledAreaCheckCommand.txt](../World355Evidence/CRunningScript_CharInAngledAreaCheckCommand.txt), [CRunningScript_FlameInAngledAreaCheckCommand.txt](../World355Evidence/CRunningScript_FlameInAngledAreaCheckCommand.txt), [CRunningScript_ObjectInAngledAreaCheckCommand.txt](../World355Evidence/CRunningScript_ObjectInAngledAreaCheckCommand.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
