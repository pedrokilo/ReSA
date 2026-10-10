<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
