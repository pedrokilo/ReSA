<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CBaseModelInfo — 34/34 funciones registradas

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

`SwaysInWind` (0x4212C0), la única entrada pendiente, quedó validada y habilitada el 2026-09-22. Las otras 33 entradas ya estaban registradas como reconstruidas; este banco no las vuelve a certificar.

## Corrección

La implementación devolvía `bool`, que en la ABI x86 de MSVC solo garantiza AL. El original devuelve un entero completo en EAX. Sus tres llamadas directas (0x426C57, 0x5699A2 y 0x59FA61) ejecutan después `test eax,eax`. Cambiar la firma a `int32` conserva el contrato y evita que los bits superiores indeterminados alteren la condición. La lógica sigue comprobando los tipos TREE y PALM.

El ejecutable analizado y el de pruebas tienen SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. El original lee la palabra en `this+0x12`, aplica la máscara `0x7800`, compara con `0x0800` y `0x1000` y devuelve exactamente 0 o 1. El desensamblado completo local está en `artifacts/original.asm` (ignorado por Git).

## Evidencia dentro del motor

65.536 comparaciones aprobadas, una por cada palabra posible de flags. Original y candidato se ejecutaron mediante la entrada 0x4212C0, alternando el hook, con dos conjuntos independientes de objetos copiados de un modelo real. Cada llamada recibió EAX con bits superiores no nulos; la captura ensamblador leyó EAX completo.

Se verificaron igualdad exacta del retorno, 0 llamadas al candidato durante la fase original, 1 durante cada caso candidato, ausencia de cambios en los 32 bytes de cada entrada y equilibrio de ESP. Al terminar se restauraron el estado y bloqueo del hook y se comprobó que el modelo real no cambió. El runner restauró el ASI anterior y verificó los hashes de las partidas.

Registro: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-basemodelinfo/20260922-192751-463543/comparisons.jsonl`. SHA-256: `12eec0be12550c0f437b377e415901fe0ed874ca3fb503d6a38d1b1c29e4b447`. Resumen: [validation-summary.json](validation-summary.json).

La compilación normal Debug x86 terminó sin la instrumentación y se instaló solo en la copia de pruebas. ASI: `5097bf9decd488a832e9ca3bcb6c533c37ab1e131b50de8815fc56919b777310`. Respaldo anterior: `D:/JUEGOS/GTA San Andreas - CVehicle Test/_baseline/before-basemodelinfo-20260922`.

## Repetición autónoma

Usar `PROBAR.cmd --case basemodelinfo`. Para evitar la pausa interactiva del CMD: `python reversing/run.py --case basemodelinfo`. Reutiliza `runtime_runner.py`: bloqueo de concurrencia, hashes, respaldo/restauración, timeout de 180 segundos y comprobación de partidas. El ejecutor termina sin supervisión del agente.

La sonda archivada se encuentra en `_baseline/gta_reversed-basemodelinfo-probe.asi` dentro de la copia de pruebas. Su hash está fijado en `run_local.py`; no depende del binario normal. `build_probe.py` reconstruye la sonda a partir de `Probe.hpp` y `LiveComparison.hpp`, conserva copias temporales en `artifacts` y restaura las fuentes incluso si falla la compilación. Un binario recién compilado puede tener otro hash: debe archivarse y actualizarse su hash explícitamente antes de repetir. Después de generar la sonda, recompilar el ASI normal antes de instalarlo; el script restaura fuentes, no el binario de salida.

## Límites

Cobertura completa del dominio de flags para esta función y este ejecutable; no de toda la clase ni de todos los escenarios jugables. Se comprobó la incompatibilidad de firma por desensamblado y se validó la firma corregida; no se reprodujeron todos los efectos históricos del hook anterior. No se realizó revisión visual, prueba prolongada de juego ni comparación de todos los registros volátiles/EFLAGS. La prueba excluye los hooks Audio y Global/SurfaceInfos_c, como la infraestructura precedente. La instalación personal no se modificó; las partidas verificadas conservaron sus hashes. Sin commit ni push.
