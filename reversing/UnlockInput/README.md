# Desbloqueo de entrada: CPad

**6.144 pares exactos aprobados**, ambos hooks habilitados y sin bloqueo: 2.048 UpdateMouse y 4.096 ProcessPad. [Resumen verificable](validation-summary.json).

Ratón y mando pasan por DirectInput tanto con los hooks originales como con los reversados. SDL mantiene la entrada de ImGui mediante una integración independiente, validada en [UnlockPlatform](../UnlockPlatform/README.md).

Se conservan las máscaras originales 0x80 de los botones de ratón sin introducir valores inválidos en bool; la estructura mantiene su tamaño. Se corrigieron adquisición y lectura en el mismo frame, ejes y POV con x87, zona muerta, inversión e intercambio de ejes, uso de padNumber y persistencia de ejes ausentes.

El banco usa interfaces COM controladas para DeviceState, Acquire y Poll. Compara todos los bytes de estados de ratón/mando y el orden de eventos, incluidos fallos de adquisición. Restaura globals, dispositivos, configuración y hooks. Audio y SurfaceInfos excluidos.

No comprueba todos los mandos físicos, drivers, hotplug, latencias, todas las teclas ni estabilidad general. Las pruebas de rutas Windows/SDL y menú son un banco separado.

Ejecución autónoma: `python reversing/UnlockInput/run_local.py`. INI, ASI y PDB restaurados; partidas verificadas por hash. La captura temprana de aserciones evita dejar los bancos detenidos en un diálogo.
