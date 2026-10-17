<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# ScriptShutdown355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CRunningScript::ShutdownThisScript`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-shutdown355/20261007-215359-927962/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ScriptShutdown355

ShutdownThisScript,1024pares. Combo REA/asm. Activo/inactivo externo, tipo0..7, variables de misión/locales, referencias de pool válidas/inválidas, número de usuarios y FinishBrain con asignación presente/ausente. Pools ficticios cubren4slots; las referencias fuera de rango no sonfixtures. Se compara estado, flags de peds/objetos, usuarios y publicación de tarea. Vptr difiere EXE/DLL y se excluye; el padre de tarea se compara. Constructor real de FinishBrain de8bytes, allocator y publicación simétricos. Globals/pools restaurados. Pendiente de compilar y validar por coordinador.

Snapshot de objetos completos mediante FNV-1a de 64 bits; outputs, punteros y trazas explícitos permanecen exactos. Las aprobaciones anteriores requieren repetición con este banco actualizado.

Aprobado por coordinador: 1024 pares, informe `resultados-script-shutdown355/20261007-215359-927962`. Las modificaciones posteriores del archivo compartido requieren comprobación final de hash y repetición cuando corresponda.

## Evidencia y cierre del lote 355

Último informe real: `20261007-215359-927962`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-shutdown355/20261007-215359-927962/summary.json>).

Última aprobación histórica: `20261007-215359-927962`; aprobado, 1024 pares. Evidencia: [summary.json](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-script-shutdown355/20261007-215359-927962/summary.json>).

Repetición final de las fuentes actuales pendiente. Los informes anteriores acreditan únicamente la sonda y los fixtures de aquella ejecución; este README no certifica coincidencia con las fuentes o instrumentos actuales. El cierre requiere el replay y los hashes estrictos coordinados por la raíz.

La equivalencia se limita a los fixtures documentados. Los colaboradores aislados simétricamente no acreditan su implementación ni una campaña integral de juego. Se conservan las limitaciones particulares descritas arriba y las desviaciones documentadas en `Pending355/resolutions.json`. Este README no autoriza activar hooks.

Evidencia de las funciones objetivo del banco: [CRunningScript_ShutdownThisScript.txt](../World355Evidence/CRunningScript_ShutdownThisScript.txt). Cada archivo conserva el pseudocódigo disponible y el ensamblador; sus avisos y limitaciones siguen vigentes. Los colaboradores sustituidos no quedan acreditados por estos enlaces.
