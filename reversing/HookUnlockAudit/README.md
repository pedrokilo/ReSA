<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8549 funciones reconstruidas; 8549 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9224 entradas binarias sin nombre catalogado; 820 destinos originales explícitos fuera del catálogo, 396 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 104 aprobadas dentro del juego; 131 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría de bloqueos de hooks

Se revisan las 421 filas `locked=1` que figuraban en `docs/hooks.csv` al iniciar esta tarea. La revisión estática está dividida entre [audio, render y mundo](review-audio-world.csv), [tareas y peds](review-tasks-peds.csv) y [vehículos y motor](review-vehicle-engine.csv). Cada fila conserva dirección, registro fuente, riesgos y prueba necesaria.

La fuente sólo contiene ocho registros explícitamente bloqueados dentro de este inventario. El CSV histórico no permite conocer el estado de la DLL actual. La sonda recopila todos los hooks del juego cargado y comprueba la negativa de conmutación de los que continúan bloqueados. Corregir un indicador obsoleto del catálogo no constituye un desbloqueo nuevo ni una prueba nueva de seguridad.

## Pruebas específicas

`CEntryExit::GetPositionRelativeToOutsideWorld`: 2.048 pares sin sustituir colaboradores. Se alternan ambas entradas, EXE y wrapper DLL, comprobando EDX y ESP; se ejecuta el manager original y reconstruido con pilas de cero a cuatro interiores y entradas enlazadas. Los objetos y la pila son controlados; todos los globales se restauran antes de devolver el control al juego. La sonda continúa 240 fotogramas, alternando el hook entre fotogramas con la partida y el render reales en funcionamiento. Esto acredita el ayudante de transformación y sus contratos probados; no es una campaña de teletransporte por todos los interiores.

`CPostEffects::ImmediateModeRenderStatesStore`: se invoca desde EXE y DLL en ON/OFF/ON con el dispositivo RenderWare vivo. Se comprueban diez estados, ESP, restauración del bloque `ms_imf` y ausencia de cambios en el controlador. EAX se observa, no se exige conservarlo: los [14 callers directos originales](posteffects-exe-callers.md) llaman inmediatamente a Set, que sobrescribe EAX.

## Límites y conservación

Los tres hooks de audio y los tres del bucle/render del juego no se fuerzan a OFF para probar si fallan: sus riesgos requieren escenarios específicos de concurrencia, vehículos, cambios de menú y foreground. Las pruebas previas con stubs o conmutación conjunta no acreditan esas mezclas. Permanecen bloqueados salvo evidencia nueva suficiente.

La ejecución utiliza únicamente `E:/JUEGOS/GTA San Andreas - CVehicle Test`. El ejecutor existente conserva y verifica por hash los ASI, PDB, INI y partidas. `runtime_runner.py`, los documentos congelados y los hashes históricos no se modifican. Cada ejecución conserva su resumen y registro completo; las fallidas no autorizan cambios.

Construcción y ejecución en serie:

```powershell
$env:CMAKE_BUILD_PARALLEL_LEVEL='2'
$env:_CL_='-MP2'
& 'C:/Program Files/Git/bin/bash.exe' reversing/_handoff/bench.sh HookUnlockAudit 3
```

## Resultado verificado

Las corridas `20261010-160346-766924` y `20261010-160759-574448` aprobaron cada una 2.048 pares de interiores, 1.446 comprobaciones de postefectos y 240 fotogramas reales. La segunda se compiló después de retirar los dos bloqueos. Ambas terminaron con cero diferencias en las comprobaciones, sin accesos inválidos registrados, código/estado restaurados, partidas intactas e INI restaurados. La huella de las siete fuentes probadas coincide con el árbol final. [Resumen y límites](unlock-summary.json).

Se retiraron únicamente los locks reales de `CEntryExit::GetPositionRelativeToOutsideWorld` y `CPostEffects::ImmediateModeRenderStatesStore`. Otros 413 flags `locked=1` del CSV se corrigieron porque la fuente y la DLL ya los tenían desbloqueados. Eso no supone 413 nuevas pruebas de seguridad ni 413 cambios de política en la fuente.

Los seis bloqueos conservados son:

| Grupo | Hook | Motivo pendiente |
| --- | --- | --- |
| CAEVehicleAudioEntity | ProcessPlayerVehicleEngine | Mezclas individuales y concurrencia con audio activo; aviso histórico de crashes |
| CAEVehicleAudioEntity | ProcessVehicle | Mezclas individuales, ciclo de vida y concurrencia; aviso histórico de crashes |
| CAEVehicleAudioEntity | Service | Hilo/audio, referencias y mezclas individuales; aviso histórico de crashes |
| Game | Render2dStuff | Aviso de bloqueo del juego al desactivar; render e integración de frame |
| Game | Idle | Bucle activo, foreground y transiciones menú/partida |
| Game | FrontendIdle | Bucle de menú, transiciones y punto seguro de conmutación |

La sonda confirmó que estos seis siguen bloqueados y que su API rechaza conmutarlos. No se intentó forzar su desactivación ni se acredita que OFF sea seguro. El [CSV de decisiones](lock-decisions.csv) distingue el resultado de cada una de las 421 filas revisadas. Los registros completos están comprimidos en `runs/<corrida>/comparisons.jsonl.gz`; se conservan sus bytes originales y la huella del registro sin comprimir.

<!-- RESA-HOOK-UNLOCK-SUPERSEDED -->

**Actualización posterior:** los seis locks conservados en este informe histórico se probaron y retiraron posteriormente. Consultar [estado actual](../../docs/CurrentStatus.md), [Audio](../HookUnlockAudio/README.md) y [Game](../HookUnlockGame/README.md). Las cifras y resultados del cuerpo mantienen su fecha y alcance.
