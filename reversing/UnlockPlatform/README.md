<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plataforma conmutable: Windows, SDL e ImGui

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**Nueve arranques aprobados**, con los siete hooks sin candado en las fuentes. **324 pares de función y 18 comprobaciones de entrada**. [Resumen verificable](validation-summary.json).

> **04/10/2026:** el menú ImGui se retiró del juego (comportamiento del EXE). SDLWrapper ya no reenvía eventos a ImGui ni descarta entrada, y WinMain cierra SDL directamente. El banco solo comprueba ahora que el juego recibe la tecla y el ratón: una comprobación de entrada por arranque en vez de dos. Los resultados de abajo son anteriores a este cambio.

## Qué se cambió

PsGlobal vuelve al bloque nativo de 40 bytes en 0xC8CF88. La ventana es un HWND nativo que SDL envuelve; el puntero SDL se mantiene fuera de ese bloque. Un temporizador de la ventana bombea los eventos tanto con WinMain/MessageLoop originales como reversados. La captura de ImGui está entre el procedimiento SDL y el procedimiento del juego, y WM_GRAPHNOTIFY sigue llegando al reproductor de vídeo.

Ratón y mando del juego usan DirectInput, también bajo SDL; su equivalencia se comprueba en [UnlockInput](../UnlockInput/README.md). La enumeración de dispositivos conserva la función original. El área final de cliente determina las dimensiones del backbuffer y de cámara, reparando el raster nulo observado al pedir 1920×1080 y obtener 1920×1055.

El aviso inicial de adquisición del ratón se investigó después: [recuperación natural comprobada](../MouseRecovery/README.md) a los 1,485 s, con la ventana visible y en primer plano, seguida de 120 actualizaciones válidas. Esa observación no repite la matriz de modos de este informe.

Se retiraron los cinco bloqueos restantes: CMenuManager::CheckForMenuClosing, Ps::psInitialize/psTerminate y Win::NOTSA_WinMain/MessageLoop. Los dos de CPad se validaron y retiraron en su banco propio.

## Matriz final

Orden de bits: UpdateMouse, ProcessPad, CheckForMenuClosing, psInitialize, psTerminate, NOTSA_WinMain, MessageLoop. Bit 1 selecciona el reversado; bit 0, el original.

| Máscara | Selección al arrancar | Resultado |
| ---: | --- | --- |
| 127 | Todos reversados | Aprobado |
| 0 | Los siete originales | Aprobado |
| 126 | Solo UpdateMouse original | Aprobado |
| 125 | Solo ProcessPad original | Aprobado |
| 123 | Solo CheckForMenuClosing original | Aprobado |
| 119 | Solo psInitialize original | Aprobado |
| 111 | Solo psTerminate original | Aprobado |
| 95 | Solo NOTSA_WinMain original | Aprobado |
| 63 | Solo MessageLoop original | Aprobado |

Cada proceso verifica estado de hooks, llamadas de arranque, bloque Ps, ventana SDL y raster existente. Después compara 16 pares de psTerminate, 16 pares de MessageLoop con WM_NULL/WM_QUIT y cuatro aperturas/cierres del menú por implementación. Por último comprueba una tecla y movimiento del ratón con ImGui desactivado y activado. Los 36 pares se repiten en cada modo; no son 324 escenarios únicos.

El banco impide escribir ajustes y controla la consulta de resintonización de radio. No compara byte por byte todo CMenuManager: compara las transiciones y banderas descritas en el log. La verificación de entrada registra mensajes recibidos, sin demostración visual. El proceso se cierra al terminar; la restauración comprobada abarca hooks, menú cerrado y bloque Ps, no todos los globals del mundo.

## Ejecución autónoma y límites

```powershell
python reversing/UnlockPlatform/run_local.py --mask 127
python reversing/UnlockPlatform/run_matrix.py
```

El caso `unlock-platform` del único PROBAR.cmd ejecuta la máscara 127. La matriz ejecuta nueve procesos en serie y se detiene ante el primer fallo. Plazo máximo de cinco minutos por proceso, restauración de ASI/PDB e INI y hashes de partidas. Cuando se usa WinMain original, el banco envía entrada exclusivamente a su HWND para pasar las introducciones y seleccionar partida nueva; no modifica guardados.

Audio y SurfaceInfos excluidos. No cubre las 128 combinaciones posibles, todas las ramas de WinMain, todos los dispositivos, hotplug, reinicio del render, todos los modos de pantalla ni estabilidad general.

Los intentos fallidos se conservan y no cuentan como aprobación. La sonda inicial también interrumpía excepciones de primera oportunidad durante Direct3D; la final permite los manejadores SEH locales y captura las no manejadas. No se desactivaron las aserciones del juego. Tras la matriz solo se eliminaron líneas vacías al final de WinInput.cpp/.h; los hashes antes/después figuran en el resumen, y el ASI normal se recompiló aparte.
