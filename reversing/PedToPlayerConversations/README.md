<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPedToPlayerConversations — 3/3

> Informe de la entrega del 27/09/2026. El ASI instalado posteriormente y sus comprobaciones figuran en [CConversationForPed](../ConversationForPed/README.md) y en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Los hashes y resultados de este documento corresponden a su propia entrega.

`Update` (`0x43B0F0`) reconstruido y habilitado el 27/09/2026. `Clear` y `EndConversation` ya estaban reconstruidos. El catálogo completo de esta clase queda en 3/3, sin bloqueos. Esta entrega no incluía `CConversationForPed`; sus dos funciones se completaron posteriormente, el 30/09/2026: [2/2 y 4.702 pares](../ConversationForPed/README.md).

## Implementación

La comparación parte del ejecutable local SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Se reconstruyó la máquina de estados, selección circular de ocho peatones, disponibilidad de conversación, temas y valoraciones, respuestas, temporizadores, audio, miradas y creación de la secuencia de ataque.

Se incorporan las referencias a `m_PedPoolIndex` (`0x969A3C`) y `m_LastPlayerVehicle` (`0x969A40`). Se conserva la aritmética temporal unsigned, el orden de efectos y llamadas, las conversiones truncadas de estadísticas y el uso de `CWeather::Wind` en el tema de clima. No se reemplazan estas condiciones por aproximaciones.

También se retiró texto de tabla que había quedado fuera de un comentario en `ColModel.cpp`: impedía compilar. Esa reparación no cambia su comportamiento.

## Evidencia y límites

[Resumen diferencial](validation-summary.json): **6.266 pares exactos dentro del motor**, con hook habilitado desde el arranque, original/candidato ejecutados separadamente, objetos de entrada intactos, contadores 0/1 y restauración de estado. El proceso terminó normalmente con código 0.

| Familia | Pares |
| --- | ---: |
| Estados, plazos, nulos, miradas y desbordamiento | 1.600 |
| Respuestas, tipos de peatón y decisiones de ataque | 2.304 |
| Estadísticas de apertura | 1.020 |
| Condiciones de apertura, pelo, tatuajes y reserva de voz | 640 |
| Vehículos, salud y distancia | 126 |
| Clima, forzando el tema correspondiente | 42 |
| Recorrido del pool | 144 |
| Geometría | 20 |
| Espera entre conversaciones | 9 |
| Referencias al vehículo | 5 |
| Estadísticas fraccionarias y límites adyacentes | 300 |
| Tipos de peatón en apertura | 40 |
| Huecos del pool | 16 |

`LiveComparison.hpp` prepara fixtures independientes; `Probe.hpp` controla audio, entrada, estadísticas, azar y llamadas IK, registrando sus argumentos y efectos observables. Original y candidato reciben los mismos proveedores. Geometría, referencias y objetos de tareas utilizan el motor real; se comparan también los campos de las tareas construidas. El banco anterior de 5.910 pares es un subconjunto y no se suma al total.

El alcance acredita esas rutas y contratos con dependencias controladas; no reproduce sonido ni ejecuta el combate. No acredita todo el subsistema ni independencia del EXE. El registro vincula esta evidencia sin declarar acreditación vigente del árbol completo.

## Compilación y entrega

[Resumen de instalación](installation-summary.json): tres arranques automáticos de partida nueva, **tres segundos** en estado de juego por ejecución, salto de `Update` al módulo instalado y cierre mediante WM_CLOSE con código 0. Binarios normales sin sondas en `bin/Debug` y exclusivamente en `D:\JUEGOS\GTA San Andreas - CVehicle Test\scripts`.

**Incidencia de integración conservada:** las dos tandas iniciales de diez segundos tuvieron respectivamente dos y un cierres correctos, seguidos de timeout. La pila del segundo timeout muestra el hilo principal en `CAECutsceneTrackManager::PreloadCutsceneTrack`, durante la carga de `PROLOG1`, antes de atender el cierre. El control con el ASI anterior de ShutdownLists, que ejecuta `Update` original, reprodujo el timeout tras dos cierres correctos y mostró la misma ruta de precarga. [Intentos y pilas](integration-observations.json). Esto demuestra que el bloqueo también existe con el binario anterior; no identifica su causa raíz ni lo corrige. La prueba breve de entrega comprueba arranque, conexión del hook y cierre antes de esa fase; no sustituye ni convierte los intentos fallidos en aprobados.

`build.py` reutiliza las exclusiones de `ShutdownLists/build_isolated.py`: los dos archivos de CustomBuilding desde `11fd6912f759d3bc93208a9def001b6dd751dbdf` y TaskSimpleClimb desde `aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51`. Conserva la corrección de cierre ya entregada. Cada compilación archiva manifiesto, fuentes sustituidas y hashes; restaura las fuentes y binarios anteriores por bytes. La instalación promueve solamente el artefacto normal comprobado y conserva respaldos.

Los INI se cambian temporalmente a partida nueva (`SaveGameToLoad=0`) y se restauran a la configuración anterior (`FastLoader=false`, `SaveGameToLoad=-2`). Se comprueban partidas por hash. No se usa la ranura 2 ni la instalación personal. Las pruebas normales cubren inicio/introducción y cierre; no sesiones prolongadas, conducción, combate ni guardar/cargar. No se repitió toda la suite.

## Reproducir

Desde la raíz, con Python 3 disponible y el entorno de compilación configurado:

```powershell
python reversing/PedToPlayerConversations/build.py --probe
PROBAR.cmd --case ped-to-player
python contrib/audit_hook_catalog.py --strict
python reversing/PedToPlayerConversations/build.py
python reversing/PedToPlayerConversations/check_normal.py --startup-only --install
```

Los archivos `latest-probe.json` y `latest-normal.json` señalan los artefactos locales conservados. Para otro equipo, recompilar antes de ejecutar. El caso reutiliza el ejecutor compartido y el único `PROBAR.cmd`. Las sondas nunca se entregan como binarios normales.

Sin `--startup-only`, el comprobador conserva la prueba de diez segundos y captura una pila al agotar el plazo de cierre. `--baseline` permite contrastar con el ASI anterior de ShutdownLists mientras ese binario esté disponible en `bin/Debug`; rechaza cualquier hash distinto. No se instala con esa opción.
