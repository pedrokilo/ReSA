<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Desbloqueo de entrada: CPad

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**6.144 pares exactos aprobados**, ambos hooks habilitados y sin bloqueo: 2.048 UpdateMouse y 4.096 ProcessPad. [Resumen verificable](validation-summary.json).

Ratón y mando pasan por DirectInput tanto con los hooks originales como con los reversados. SDL mantiene la entrada de ImGui mediante una integración independiente, validada en [UnlockPlatform](../UnlockPlatform/README.md).

La [observación posterior del arranque](../MouseRecovery/README.md) confirma la recuperación del aviso inicial `0x80070005`: ventana oculta/sin foco al fallar; adquisición y lectura correctas a los 1,485 s y 120 actualizaciones válidas. Es una comprobación con el dispositivo real, separada de los 6.144 pares del banco; no amplía ese recuento ni modifica el código normal.

Se conservan las máscaras originales 0x80 de los botones de ratón sin introducir valores inválidos en bool; la estructura mantiene su tamaño. Se corrigieron adquisición y lectura en el mismo frame, ejes y POV con x87, zona muerta, inversión e intercambio de ejes, uso de padNumber y persistencia de ejes ausentes.

El banco usa interfaces COM controladas para DeviceState, Acquire y Poll. Compara todos los bytes de estados de ratón/mando y el orden de eventos, incluidos fallos de adquisición. Restaura globals, dispositivos, configuración y hooks. Audio y SurfaceInfos excluidos.

No comprueba todos los mandos físicos, drivers, hotplug, latencias, todas las teclas ni estabilidad general. Las pruebas de rutas Windows/SDL y menú son un banco separado.

Ejecución autónoma: `python reversing/UnlockInput/run_local.py`. INI, ASI y PDB restaurados; partidas verificadas por hash. La captura temprana de aserciones evita dejar los bancos detenidos en un diálogo.
