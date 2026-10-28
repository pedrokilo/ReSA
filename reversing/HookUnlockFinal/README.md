<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cierre de los seis locks restantes

Los seis registros de fuente siguientes están desbloqueados después de revisar su motivo y ejecutar pruebas dentro del juego. Se conservan por separado la corrección del catálogo histórico, los desbloqueos efectivos y los límites de las pruebas. Las dos ejecuciones finales de perfiles de audio están aprobadas y archivadas. El catálogo y las tablas ya están sincronizados: los seis indicadores restantes pasaron a 0 y no quedan hooks bloqueados. Se verificaron las cuatro corridas archivadas y la auditoría estricta pasó; se conserva el corte 453, sin añadir funciones nuevas validadas.

| Clase | Hook | Dirección EXE | Lock en fuente | Evidencia |
| --- | --- | --- | --- | --- |
| CAEVehicleAudioEntity | ProcessPlayerVehicleEngine | 0x4FBB10 | 0 | [Audio](../HookUnlockAudio/runs/20261010-170746-451158/summary.json) |
| CAEVehicleAudioEntity | ProcessVehicle | 0x501E10 | 0 | [Audio](../HookUnlockAudio/runs/20261010-170746-451158/summary.json) |
| CAEVehicleAudioEntity | Service | 0x502280 | 0 | [Audio](../HookUnlockAudio/runs/20261010-170746-451158/summary.json) |
| Game | Idle | 0x53E920 | 0 | [Game](../HookUnlockGame/runs/20261010-170235-722875/summary.json) |
| Game | FrontendIdle | 0x53E770 | 0 | [Game](../HookUnlockGame/runs/20261010-170235-722875/summary.json) |
| Game | Render2dStuff | 0x53E230 | 0 | [Game](../HookUnlockGame/runs/20261010-170235-722875/summary.json) |

Antes se corrigieron 413 indicadores obsoletos del catálogo: la fuente y el runtime ya no tenían esos locks. No se cuentan como desbloqueos nuevos. CEntryExit y CPostEffects retiraron sus dos locks reales tras sus pruebas previas, archivadas en [HookUnlockAudit](../HookUnlockAudit/README.md). Este cierre aborda los seis que aquel informe dejó protegidos.

## Evidencia y problemas resueltos

La [repetición completa de CAEVehicleAudioEntity](runs/vehaudio-20261010-170444-407103/summary.json) acredita **102 funciones, 20.024 pares y cero diferencias**. Se conservan fuentes, instrumentación y manifiesto exactos del binario probado. No se suman estas repeticiones como funciones nuevas validadas.

El [banco de conmutación de audio](../HookUnlockAudio/runs/20261010-170746-451158/summary.json) pasó **19.840 pares aislados**, **960 frames**, **2.240 llamadas reales** y **32 contextos**: automóvil, moto, barco y tren con ocho máscaras de los tres hooks. Los casos de automóvil/moto incluyen entrada/salida por tareas reales, radio, controles, marchas y velocidades coherentes; requieren bancos cargados y sonidos reales. Los pares conservan colaboradores simulados y no se presentan como mezclas internas efectivas: esa evidencia corresponde a la fase real sin stubs.

La primera ejecución real reprodujo un crash por ABI: el EXE llama dos veces a `IsAccInhibitedForLowSpeed` en `0x4FC29B/0x4FC2A9`, reutilizando ECX sin recargarlo. El original conserva ECX y termina con `ret 4`; el cuerpo C++ no garantizaba ese registro. El registro del hook usa ahora `stackArguments=1` para conservarlo, sin alterar la fórmula booleana. El fallo y el éxito se conservan por separado; no fue ruido del banco. [Direcciones, hilos y límites](../HookUnlockAudio/thread-callers.md).

El [banco Game](../HookUnlockGame/runs/20261010-170235-722875/summary.json) pasó **2.560 pares**, **320 frames**, **160 frames de render real**, 56 comprobaciones de background y 56 de cola SDL. Se retiró el segundo parche no conmutable de Render2dStuff, que interfería con la ruta registrada. Se conserva la guarda de background de Idle y se probaron sus despachos en las ocho máscaras. La corrida final restaura también el byte `isForeground`; una omisión detectada en el banco anterior queda documentada y no se atribuye a la fuente.

## Perfiles de audio

El perfil habitual de HookUnlockAudio mantiene Audio activo y excluye únicamente `Global/SurfaceInfos_c`. La opción `--original-audio-profile` usa `Audio,Global/SurfaceInfos_c`, para ejecutar las mismas 32 mezclas cuando los otros hooks de Audio arrancan desactivados. Los resúmenes anotan `audio_profile` y `unhook_profile`.

Las dos corridas finales, con los seis flags de fuente retirados, están aprobadas y archivadas:

| Perfil | Corrida | Pares aislados | Frames / llamadas reales | Contextos |
| --- | --- | --- | --- | --- |
| Audio activo | [20261010-170746-451158](../HookUnlockAudio/runs/20261010-170746-451158/summary.json) | 19.840, cero diferencias | 960 / 2.240 | 32 |
| Audio original | [20261010-171052-497326](../HookUnlockAudio/runs/20261010-171052-497326/summary.json) | 19.840, cero diferencias | 960 / 2.240 | 32 |

Ambas ejecutaron el mismo ASI SHA256 `8aff36215902de50cd7f251414c394afd59217a3ce2ccb0c64ef3198a3e7a077`. Cada archivo conserva manifiesto, tres fuentes probadas y trece archivos de instrumentación exactos, comprobados por hash antes de regenerar comentarios/documentación. Los logs gzip tienen mtime cero y reproducen el SHA256 original. Las verificaciones independientes comprueban las 32 mezclas, controles restaurados, entradas/salidas, acelerador 0/128/255 con bancos cargados y sonidos reales en cada contexto de automóvil/moto. No hay resultados pasantes nulos ni accesos inválidos.

El script final exige esas dos corridas, Game y la repetición completa de las 102 funciones; también exige que ambos perfiles probaran el mismo ASI y que la fuente ya no contenga los seis locks. No modifica otros flags del catálogo.

## Alcance de seguridad

Las ejecuciones aprobadas registran diferencias cero, ningún par pasante con original/candidato nulos, restauración de hooks y configuración, conservación de partidas y limpieza de entidades de prueba. La sesión de audio se descarta sin guardar; no se afirma restauración exacta de tareas, estadísticas, radio ni streaming del proceso.

Se acreditan el EXE 1.0 US, las fuentes, configuraciones y escenarios archivados. La inspección encuentra la cadena de los tres métodos de vehículo en ProcessControl del mundo y no identifica workers que ejecuten esos cuerpos, pero no descarta todos los callers indirectos futuros o modificaciones externas. No se presenta como una garantía universal de hardware, todos los modelos, todos los estados o independencia del EXE.

La guarda programática de background y la cola SDL están probadas. **No está acreditada una campaña completa de Alt-Tab, minimizar y recuperar foco mediante mensajes nativos del sistema operativo**; no equivale a los casos de cola SDL. Esta limitación se conserva aunque el lock esté retirado.

`sync_documentation.py` verifica los runs y los registros de fuente antes de cambiar únicamente las seis filas `locked` autorizadas. Reutiliza los generadores existentes y protege los 28 documentos congelados, `runtime_runner.py` y los archivos de las corridas aprobadas. Sin `--apply` muestra el plan; no modifica documentación.

Los archivos de evidencia bajo `runs/` se preservan byte a byte mediante atributos Git que impiden normalizar sus finales de línea; sus hashes corresponden a las fuentes e instrumentación probadas.
