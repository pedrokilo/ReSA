<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Corrección del cierre: Cover, logs e interfaz

> Informe de la entrega del 25/09/2026. La corrección se conserva en las entregas posteriores; consultar la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente) para los hashes del ASI instalado actualmente. Los binarios y pruebas de este informe describen aquella entrega.

## Estado: corregido e instalado en la copia de pruebas (25/09/2026)

La petición del usuario levantó el aplazamiento de esta corrección. Se validaron **tres arranques automáticos y cierres normales sin depurador, todos con código 0**, y los **16 casos de limpieza de Cover** con un cierre instrumentado también con código 0. Cada ejecución normal permaneció diez segundos en el estado de juego antes de enviar `WM_CLOSE`. Es una prueba de cierre durante el inicio de una partida nueva, no de una sesión prolongada ni de carga/guardado.

### Causas y archivos

- [Game.cpp](../../source/game_sa/Game.cpp), [Cover.cpp](../../source/game_sa/Cover.cpp) y [Cover.h](../../source/game_sa/Cover.h): la lista retenía punteros a edificios ya destruidos. `ClearProcessedBuildings` retira las referencias y vacía los nodos antes de destruir el mundo y sus reservas. Esta parte candidata ya estaba en HEAD y se conserva.
- [app_debug.cpp](../../source/app/app_debug.cpp) y [app_debug.h](../../source/app/app_debug.h): el destructor de `spdlog::periodic_worker` intentaba adquirir un mutex durante la descarga de la DLL, después de terminar los otros hilos. `Logging::Shutdown` retira el manejador de excepciones que usa los logs, vacía los logs y cierra los trabajadores mientras todavía pueden ejecutarse.
- [WinMain.cpp](../../source/app/platform/win/WinMain.cpp): destruye ImGui/SDL antes de RenderWare y de la ventana, y cierra los logs al salir de WinMain, incluyendo retornos anticipados. La segunda pila mostró una excepción `0x87a` en DXGI desde `SDL_Quit`, llamado por el destructor tardío de `UIRenderer` durante la descarga del ASI. No se ocultan excepciones ni se fuerza el código de salida.
- [FastLoader.hpp](../../source/extensions/Configs/FastLoader.hpp): añade `SaveGameToLoad=0` para iniciar una partida nueva automáticamente. El ejecutor activa temporalmente `FastLoader Enable=true` y ese valor; no necesita capturas ni navegación por menús. Al terminar restaura `Enable=false`, `SaveGameToLoad=-2`. No carga la ranura 2 incompatible.

### Evidencia y entrega

- [Tres cierres normales](artifacts/normal-20260925-080922/summary.json).
- [16 casos y cierre instrumentado](artifacts/normal-20260925-081051/summary.json): tamaños 0/1/8/32, referencias sin registrar, registradas, retiradas y nulas; lista vacía, devolución de nodos, referencias conservadas e idempotencia. Se introducen dos edificios vivos al entrar en Shutdown y se verifica que la lista queda vacía.
- Diagnóstico del trabajador de logs: `artifacts/normal-20260925-075910/debug/hit-3-ntdll32-NtTerminateProcess.txt`. Diagnóstico SDL/DXGI: `artifacts/normal-20260925-080751/debug/exception-4.txt`. Son capturas diagnósticas; el resultado aprobado procede de las tres ejecuciones posteriores sin depurador.
- [Resumen de instalación y hashes de fuentes](installation-summary.json). ASI normal: `b132855e4c96c1a02824b91569de158848a723718059962d5b83f91e02c5f980`. PDB: `7fbdff7c8c62a74cdd266cb3898bf8f2160b41ff36fdc14687d8b4ba22751750`.

Instalado en `bin/Debug` y exclusivamente en `D:/JUEGOS/GTA San Andreas - CVehicle Test/scripts`. El resumen identifica el respaldo anterior. Partidas y ambos INI conservados por hash; instalación personal intacta. Sin commit ni push.

La compilación incluye GangLeader::GetTaskType y esta corrección, pero **excluye CustomBuilding aplazado y el cambio local de Climb**. [build_isolated.py](build_isolated.py) conserva y restaura sus fuentes por bytes y registra las versiones compiladas; no equivale a compilar el árbol sin filtros. No se cambiaron locks, indicadores ni acreditaciones del catálogo por esta corrección.

Para reproducir: `python reversing/ShutdownLists/build_isolated.py`, seguido de `python reversing/ShutdownLists/run_normal.py`. Para el banco: compilar con `--probe` y ejecutar `run_normal.py --probe`. [install_normal.py](install_normal.py) exige los dos resúmenes aprobados, sus hashes y fuentes coincidentes antes de instalar. El banco puede ejecutarse durante la introducción: solo requiere edificios vivos y no acredita control libre del jugador. El antiguo `run_local.py` conserva el analizador, pero se usa el nuevo ejecutor para evitar la configuración compartida histórica de la ranura 2.

## Diagnóstico histórico de Cover (23/09/2026)

Captura en vivo del proceso 24488, hilo principal 24076, el 23 de septiembre de 2026. El aviso permaneció abierto durante la inspección. No se cerró el proceso, no se omitió la aserción y no se escribió memoria del juego.

## Hallazgo confirmado

La pila resuelta con el PDB instalado muestra:

```text
CGame::Shutdown                         Game.cpp:272
CPtrList<CBuilding*>::Flush              PtrList.h:94
CPtrList<CBuilding*>::DeleteNode         PtrList.h:142
CPtrList<CBuilding*>::UnlinkNode         PtrList.h:133
PtrListDoubleLinkTraits::UnlinkNode      PtrListDoubleLink.h:38
_wassert
```

La lista es `CCover::m_ListOfProcessedBuildings`, ubicada en `0xC1A2B8`. Su cabeza `0x04085988` sigue asignada: índice 288 de la reserva de nodos, estado `0x1A` (bit de libre apagado). El edificio referenciado `0x0701D3C8` ya está liberado: índice 1187 de la reserva de edificios, estado `0x83` (bit de libre encendido) y primeros 16 bytes rellenos de `DD`.

`IsNodeValid` valida tanto el nodo como el objeto apuntado. En esta captura falla la validez del edificio, no la asignación del nodo. `CGame::Shutdown` llama a `CWorld::ShutDown`, que elimina entidades, antes de vaciar esta lista de cobertura. Esto explica la referencia obsoleta observada durante su limpieza.

La corrección candidata gestiona la lista y sus referencias antes de destruir los edificios. Considera el registro opcional de referencias en `CCover::Update` bajo `FIX_BUGS`; mover o borrar nodos sin retirar esas referencias podría introducir otro puntero colgante. Su prueba aislada pasó, pero la validación del cierre normal permanece pendiente según el estado anterior.

## Evidencia local

En `artifacts/live-20260923-1903/`: `process.dmp`, `stacks.json` y `node.json`. En `artifacts/capture.log`: pila legible. Los artefactos están ignorados por Git mediante las reglas de `reversing/`.

La herramienta [capture_live_stack.py](../capture_live_stack.py) usa lectura de proceso y DbgHelp; suspende cada hilo durante la captura de su contexto y lo reanuda en un bloque de finalización. Esta captura requirió ejecución fuera del sandbox por restricciones de acceso al proceso.
