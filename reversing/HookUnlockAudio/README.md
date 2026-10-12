<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sonda de conmutación del audio del vehículo

**APROBADO: ejecución `20261010-165826-989653`**, con19.840pares sin diferencias,960frames,2.240llamadas reales y32contextos. [Resumen del runner](runs/20261010-165826-989653/summary.json), [verificación independiente](runs/20261010-165826-989653/independent-verification.json). Ningún registro pasante tiene resultados nulos; estado dehooks restaurado,entidades de prueba retiradas ypartidas/configuración conservadas. Se conserva el alcance y los límites siguientes; no es una garantía sobre todos los modelos o controladores.

## Comparaciones aisladas

Se conservan copias independientes de los ocho encabezados de VehAudioFinal4 y sus fixtures. Se ejecutan únicamente ProcessPlayerVehicleEngine (1.200 casos), ProcessVehicle (640) y Service (640), ocho veces: **19.840 pares** originales/candidatos. El baseline completo de 102 funciones y 20.024 pares se ejecuta aparte.

La fase par conserva stubs de funciones hermanas para mantener el dominio seguro. Las ocho configuraciones cambian estados de hooks, pero **no demuestran ocho mezclas internas efectivas** cuando una hermana está sustituida. Son pruebas aisladas de las entradas alternadas EXE/DLL y de sus resultados, trazas, memoria y restauración.

## Ejecución real

Después se destruyen los parches de fixtures y se restauran sus globals. La fase real usa Audio activo y no instala stubs: automóvil, moto, barco y tren creados mediante las APIs y tareas reales. Cada tipo recorre ocho máscaras de los tres hooks, treinta frames por máscara: **32 contextos y 960 frames**.

Service y ProcessVehicle se llaman explícitamente en todos los frames, alternando entrada EXE y símbolo DLL. ProcessPlayerVehicleEngine se llama sólo mientras el jugador conduce automóvil o moto: veinte frames por contexto. Total previsto: **2.240 llamadas explícitas**. No se cuenta ni se acredita cada llamada natural del mundo o del hilo de audio.

En automóvil/moto se sale mediante tarea real en el frame 10, se vuelve a entrar en el frame 20 y se activan/desactivan radio y motor sobre el vehículo real. Los controles temporales recorren modos 0–3, acelerador solicitado 0/128/255, freno y freno de mano; se prueban marchas válidas y velocidades derivadas de la transmisión. El modo 3 comparte eje entre acelerador y freno y alcanza 254 con el valor físico -127. Se registran los controles efectivos, estado del motor, bancos y punteros de los canales de sonido.

El analizador exige destinos correctos, equilibrio ESP, restauración de controles por frame, salida/reentrada efectivas y todas las llamadas previstas. Cada contexto de automóvil/moto debe alcanzar acelerador real 0, 128 y 255 con los bancos cargados y llamada efectiva al motor, freno efectivo y al menos un sonido del motor no nulo. No acepta una salida temprana con hardware descargado como cobertura del motor.

## Restauración y límites

La sesión es desechable y termina sin guardar. Se restauran bytes, estados y locks de hooks, controles, marcha, pedales y velocidad temporales; se retiran las entidades creadas. El runner debe acreditar conservación de partidas, configuración y ASI. Las tareas, estadísticas, radio, streaming y estado íntegro del proceso no se restauran exactamente: no se afirma equivalencia de una sesión viva antes/después.

El audio real no se presenta como par determinista. Los hashes y canales son observaciones; no acreditan identidad de ondas sonoras ni todos los estados, modelos, hardware o transiciones posibles. Se reprodujo una causa REAL de crash en la primera ejecución y se corrigió el contrato ABI descrito abajo; no fue ruido del banco. El grafo estático de callers e hilos se documenta en [thread-callers.md](thread-callers.md).

La reescritura de cinco bytes no es atómica. La conmutación ocurre desde la actualización principal fuera de los tres cuerpos conocidos. Hay workers reales de streaming/fades que comparten hardware, pero no se ha encontrado que llamen a estos tres métodos; eso no constituye una demostración exhaustiva sobre todos los callers indirectos futuros o modificaciones externas.

Ejecutar en serie en la instalación de prueba, sin modificar runtime_runner.py:

`CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh HookUnlockAudio 3`

## Crash real reproducido y corrección ABI

La ejecución fallida `20261010-165406-602582` completó los19.840pares aislados sin diferencias, pero falló después de siete frames reales de la máscara0. El primer acceso inválido ocurrió en `CAEVehicleAudioEntity::IsAccInhibitedForLowSpeed+18`, con `ECX=0`, lectura de `0xB0` y retorno a `0x4FC2AE`.

El EXE `ProcessPlayerVehicleEngine` prepara `ECX=ESI` en `0x4FC299`, llama `IsAccInhibitedForLowSpeed` en `0x4FC29B` y vuelve a llamar en `0x4FC2A9` **sin recargar ECX**. El original `0x4F4FF0` lee `[ECX+0xB0]`, no altera ECX y retorna `ret4` en `0x4F5010/0x4F5015`. El cuerpo C++ puede destruir ECX aunque la fórmula booleana sea equivalente.

Se añadió `.stackArguments=1` al registro del hook de `IsAccInhibitedForLowSpeed`: el thunk existente conserva ECX y respeta el argumento `thiscall` con limpieza `ret4`. No se cambió la implementación booleana. Esta reparación del ABI resuelve el crash observado en la mezcla EXE/DLL, que las pruebas aisladas con colaboradores simulados no detectaban.

Tras la reparación, la ejecución aprobada cubrió los32contextos sin accesos inválidos. ASI ejecutado SHA256: `32c2a4581df904e8eaf792f96388d52dc7adeca2f3f20dd5aa0e350a0c8db36f`. El archivo de éxito contiene manifiesto exacto y snapshots de3fuentes y13archivos de instrumentación, comprobados por hash antes de retirar flags de bloqueo. El fallo conserva log, resumen y resultado, pero su manifiesto fue sobrescrito: se declara ausente y no se fabrica. Su ASI SHA256 fue `485d9e9aa449dd395fcfd59e3f14658e72b9eb0d0df425aaadde69f1ebb2f22f`.

## Cierre con los flags de fuente retirados

Dos corridas finales aprobaron cada una19.840pares aislados sin diferencias,960frames,2.240llamadas reales y32contextos:

- [Audio activo:20261010-170746-451158](runs/20261010-170746-451158/summary.json), [verificación independiente](runs/20261010-170746-451158/independent-verification.json).
- [Audio original:20261010-171052-497326](runs/20261010-171052-497326/summary.json), [verificación independiente](runs/20261010-171052-497326/independent-verification.json).

Ambas ejecutaron el mismo ASI SHA256 `8aff36215902de50cd7f251414c394afd59217a3ce2ccb0c64ef3198a3e7a077`, con los tres locks de vehículo audio retirados de la fuente. Cada archivo conserva3fuentes y13archivos de instrumentación exactos,manifiesto,loggzipmtime0,summaryyresultado. Los hashes se comprobaron antes de cualquier regeneración posterior de comentarios; las copias archivadas permanecen inmutables.

En ambos perfiles se verificaron los32contextos y ninguna AV. El motor del jugador alcanzó acelerador efectivo0/128/255 con bancos cargados y sonidos reales en cada contexto coche/moto; se verificaron frenos, salida/reentrada yrestauración de controles yhooks. Partidas yconfiguración conservadas. La sesión se descarta sin guardar; siguen aplicando los límites de restauración y cobertura de este documento. [Cierre conjunto de los seis locks](../HookUnlockFinal/README.md).
