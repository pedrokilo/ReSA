<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Entrega parcial: DataStream y TrackEntity

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Entrega autorizada: **10 candados retirados** tras comparación dentro del motor. Se conservan **86** para otra sesión: CCollision 59, CFileMgr 13, GangLeader 11 y Game 3.

| Banco | Candados retirados | Pares finales |
| --- | ---: | ---: |
| [CAEDataStream](../UnlockDataStream/README.md) | 6 | 6.144 |
| [TrackEntity y dependencia SetTargetPos](../UnlockTrackEntity/README.md) | 4 | 9.216 |

Además, DataStream comprobó las 64 combinaciones de sus seis métodos. No se suman pasadas repetidas como cobertura nueva. La compilación normal y las dos comprobaciones de integración finalizaron. [Resumen verificable](validation-summary.json).

## Trabajo conservado para después

Los cambios candidatos de CFileMgr y Game se separaron de las fuentes compilables: [parche recuperable](deferred-filemgr-game.patch) y [manifiesto de los 15 archivos](deferred-manifest.json). Copias exactas antes/después en artifacts/deferred-20260923. El parche parte de b7b3dc4 y debe revisarse con `git apply --check` antes de recuperarlo en una sesión futura; no aplicarlo automáticamente sobre fuentes que hayan cambiado.

CFileMgr conserva sus trece bloqueos. Su último banco falló en LoadFile: el original devolvió 0 y el candidato 256. Game conserva tres bloqueos; su traslado de ImGui a presentación no estaba validado. Collision y GangLeader solo se inspeccionaron. Ninguna de estas cuatro clases se declara terminada.

El CSV actualiza únicamente los diez flags validados. Los totales siguen en 614 clases completas, 85 parciales, 2 sin revertir, 7.485/8.014 entradas y 529 pendientes. Candados registrados: **620**. No garantiza estabilidad general ni sustituye la campaña integral futura.

> **Nota de vigencia:** estas cifras corresponden al cierre de esta entrega del 23 de septiembre. El catálogo vigente incorporó owners activos antes omitidos y se consulta en [ReversedClasses.md](../../docs/ReversedClasses.md). Los diez desbloqueos y sus evidencias no cambian por esa ampliación del denominador.


## ASI normal e integración de esta entrega

Debug x86, sin instrumentación: `16f5542f9ff8fc2e84d8adb31caddccbb29cecb8ca633076efda5c51df0600b9`. Instalado solo en la copia de pruebas; respaldo `_baseline/before-unlock-batch2-20260923-132316`. Las fuentes entregadas coinciden con los manifiestos de los bancos. TrackEntity se recompiló y repitió sin los borradores aplazados antes de cerrar la entrega.

- Inicio normal sin argumentos, exclusiones ni teclas: menú a los **30,82 s**, diez segundos en él, sin carga automática. [Evidencia local](../InicioNormal/artifacts/startup-20260923-132328/summary.json).
- Integración del mismo ASI con Audio activo y sin exclusiones: **30,03 s en IDLE y 873 frames**. Terminó automáticamente sin cierre inesperado, aserción ni fallo RTC observado. [Evidencia local](../Unlocks/artifacts/current-20260923-132450/summary.json).
- INI restaurados, ASI conservado y partidas sin cambios por hash. Para uso normal, FastLoader sigue desactivado y SaveGameToLoad=-2. No se modificó la instalación personal.

**El log no está limpio:** se observaron 13 fallos de asignación de CEntryExit, además del aviso timecyc y el rechazo de la ranura incompatible; al arrancar también apareció un aviso de adquisición del ratón. El motor continuó. No se determinó la causa de CEntryExit ni se atribuye automáticamente a estos cambios. No se validó conducción, combate, guardado/carga completa ni una sesión prolongada.

Seguimiento posterior de esos avisos: [CEntryExit](../CEntryExitPool/README.md), [datos e IK](../IKDataWarnings/README.md) y [recuperación del ratón](../MouseRecovery/README.md). La falta de diagnóstico anterior corresponde a esta entrega histórica; no describe el estado vigente. La protección de partidas incompatibles se mantiene.

Catálogo sincronizado: `unlock-data-stream` y `unlock-track-entity` están en cases.json/run_all.py y en el único PROBAR.cmd. La suite histórica no se ejecutó completa. Sin commit ni push.


**Corrección posterior:** la incidencia de CEntryExit se diagnosticó y se amplió su reserva de 400 a 512: [evidencia, ASI actual y límites](../CEntryExitPool/README.md). Los 13 fallos anteriores se conservan como evidencia histórica; no describen la comprobación final de esa reparación. Los 86 desbloqueos siguen aplazados.
