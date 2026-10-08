<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
