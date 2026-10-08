<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Inicio normal y cierre después de un choque

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

## Abrir la copia de pruebas desde el principio

```powershell
Set-Location 'D:\JUEGOS\GTA San Andreas - CVehicle Test'
& '.\gta_sa.exe'
```

Los archivos `gta-reversed.ini` de la raíz y de `scripts` tienen:

```ini
[FastLoader]
Enable = false
SaveGameToLoad = -2
```

También se dejaron en false las opciones que omiten las pantallas de inicio. Al desactivar FastLoader, el programa conserva sus valores de inicio normales. `Misc.SaveDirectoryPath` sigue apuntando a `userfiles` dentro de la copia de pruebas.

El juego llega al menú; el usuario elige **Nueva partida** o **Cargar partida**. No se añaden argumentos para excluir hooks ni se fuerza la activación de funciones pendientes. El ejecutable gráfico puede devolver el control a PowerShell aunque siga abierto; ver el indicador de PowerShell de nuevo no demuestra por sí solo un cierre.

## Qué ocurrió en el cierre comunicado

El registro original y Windows coinciden: excepción de acceso `0xC0000005` a las 00:36:43 del 23 de septiembre de 2026, en `CAESound::UpdateParameters:252`. Se intentaba obtener la posición de una entidad física mediante un puntero inválido; dirección leída `0xDDDDDE0D`, compatible con memoria liberada de depuración.

Se reprodujo y reparó un defecto de asignación de CAESound que dejaba referencias sin registrar y, por tanto, sin anular cuando desaparecía la entidad. [Reproducción, 1.024 comparaciones y límites](../CAESound/README.md). No se reprodujo exactamente el mismo choque; el registro también mostró fallos de asignación de CEntryExit, tratados posteriormente en la [ampliación de su reserva](../CEntryExitPool/README.md).

El registro previo a las nuevas ejecuciones se conserva en `artifacts/crash-20260923-003643.log`. Los dos INI originales también están respaldados en esa carpeta.

## Comprobación automática del arranque

Última comprobación tras la entrega parcial de DataStream y TrackEntity: ASI normal **sin argumentos ni exclusiones**. Estados muestreados: 0, 1, 2, 5, 7. Alcanzó el menú en **30,82 segundos** y permaneció allí diez segundos; no entró en los estados 8 o 9 ni registró carga automática. El ejecutor no pulsó teclas ni cambió la configuración. La muestra cada 250 ms puede omitir estados breves. Evidencia: artifacts/startup-20260923-132328.

La pasada de la tanda anterior de desbloqueos alcanzó el menú en 68,89 segundos y sigue conservada como evidencia histórica.

La comprobación anterior, después de las diez clases, observó 0, 1, 2, 3, 4, 5, 7 y menú a los 130,49 segundos; sigue archivada como evidencia histórica.

Dos intentos anteriores agotaron 180 segundos en el logo; el segundo confirmó que GTA tenía el foco. La ruta SDL no reenviaba WM_GRAPHNOTIFY a VideoPlayer::OnGraphNotify, a diferencia de la ruta Win32. [SDLWrapper.cpp](../../source/app/platform/SDLWrapper.cpp) ahora reenvía ese mensaje solo para la ventana del juego y los estados de logo/introducción. No se modificó el manejador ni se omitieron los vídeos.

El runner registra también el foco y analiza únicamente los bytes nuevos del log, evitando atribuir a esta ejecución cargas o errores de pruebas anteriores. Los intentos fallidos se conservan en artifacts/startup-20260923-015325 y startup-20260923-015853. La observación anterior de 27,32 segundos corresponde al ASI de CAESound y conserva su evidencia histórica.
La observación es de estados, no una certificación visual de cada fotograma de los vídeos. La muestra es cada 250 ms, por lo que puede omitir estados breves. El proceso se cerró automáticamente al completar la comprobación. ASI/PDB, INI y partidas permanecieron intactos durante ella.

[Resumen verificable](validation-summary.json) · [Ejecutor](run_local.py).

```powershell
python reversing/InicioNormal/run_local.py
```

El ejecutor comprueba que no haya otro GTA abierto, adquiere el bloqueo compartido, lanza su propio proceso, observa durante un máximo de 180 segundos y lo cierra al terminar. No juega ni guarda partidas.

## Pruebas rápidas e inicio normal

`reversing/runtime_runner.py` guarda ambos INI, activa temporalmente FastLoader y la ranura 2 durante cada banco y restaura los archivos byte por byte en su bloque de finalización, también si la comparación falla. Registra `configs_restored` en el resumen. Se comprobó esta restauración tanto en la reproducción fallida como en el banco de audio aprobado.

Se mantiene un único PROBAR.cmd. El inicio normal usa el ejecutable directamente; los bancos siguen disponibles mediante el catálogo común.

## Documentación en español

En la revisión inicial se revisaron los 38 Markdown propios que existían entonces, incluida .github. Se tradujeron los seis que contenían documentación en inglés: registro de hooks, guía de programación, instrucciones de Copilot, módulos de depuración, animaciones y compilación con MSVC/Wine.

Se conservaron los documentos ya escritos en español, se corrigieron acentos dañados y se marcaron claramente el plan y el informe de ejecución históricos. Los comandos, nombres de clases, rutas, claves de configuración y nombres visibles de las interfaces técnicas se conservan. Las cachés y dependencias descargadas no forman parte de esta traducción.

Los dos documentos añadidos en aquella revisión también quedaron en español. Una revisión posterior abarcó 54 Markdown propios; ese recuento es histórico. La [revisión vigente](../MouseRecovery/README.md#revisión-documental) incluye los documentos incorporados después y verifica referencias y enlaces, conservando las evidencias históricas.

## Estado de la comprobación de arranque

ASI normal usado en esa comprobación: `16f5542f9ff8fc2e84d8adb31caddccbb29cecb8ca633076efda5c51df0600b9`. Respaldo inmediato en `_baseline/before-unlock-batch2-20260923-132316`. Se mantienen los respaldos anteriores. Partidas e INI conservados e instalación personal intacta. [Entrega parcial, integración y límites](../UnlockBatch2/README.md). No se garantiza estabilidad general ni se inició la campaña integral aplazada. Sin commit ni push.


**Estado instalado vigente:** `ec43e626ac929660f8f75e027cd7c4292b8438db0c37ee78136544ab4483944f`, con las reparaciones de [IK/datos](../IKDataWarnings/README.md), [peatones](../FallenPeds/README.md) y [CEntryExit](../CEntryExitPool/README.md). Integración: 121,16 s y 3.605 fotogramas con carga rápida temporal; no se repitió el banco de llegada al menú. El [diagnóstico del ratón](../MouseRecovery/README.md) verificó después su recuperación natural sin cambiar ese ASI normal. Ambos INI conservan el inicio normal indicado arriba y las partidas permanecen intactas.
