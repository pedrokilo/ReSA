# Aserción al cerrar: lista de edificios de CCover

## Estado: PENDIENTE, pausado por el usuario

Registro actualizado el 23 de septiembre de 2026. Se detuvo la investigación por petición del usuario. **No se considera corregido ni validado el cierre normal del juego.**

Se conserva una corrección candidata en [Game.cpp](../../source/game_sa/Game.cpp), [Cover.cpp](../../source/game_sa/Cover.cpp) y [Cover.h](../../source/game_sa/Cover.h): limpiar la lista de edificios de cobertura y retirar sus referencias antes de destruir el mundo.

Resultados obtenidos:

- El banco instrumentado superó 16 casos de limpieza (listas vacías, referencias registradas, retiradas y nulas, e idempotencia). También hubo cierres instrumentados con código 0, incluyendo cierre mediante `WM_CLOSE`.
- Otras ejecuciones instrumentadas terminaron con `0xC0000409`; no se ha establecido la causa de ese fallo adicional.
- La última prueba del ASI normal terminó con código **3**, por lo que la validación completa sigue fallando. Evidencia: [summary.json](artifacts/normal-20260923-192201/summary.json). SHA-256 del candidato probado: `a81bb5fb1cb3cd3586c57a349193b2f5715390768e9fe40ec437943ec0891153`.
- El ejecutor restauró los archivos instalados y la configuración; comprobó que las partidas no cambiaron. No queda un proceso `gta_sa` abierto al registrar la pausa.

**Instalación conservada:** el ASI anterior con la corrección de impactos de bala, SHA-256 `d8b39a395abfed3acae54a480df9db7368dbdbbaff94caeed817dcc121f903ec`. La corrección candidata de cierre permanece en el código fuente, **sin instalar**. `bin/Debug` contiene el candidato compilado; no confundirlo con el ASI instalado.

Para retomar: capturar la pila del fallo del ASI normal que termina con código 3, determinar si es otra aserción o un fallo posterior, corregir su causa y repetir cierres normales antes de instalar. Las pruebas instrumentadas no bastan para dar por resuelto este incidente. Scripts conservados: [run_normal.py](run_normal.py), [run_local.py](run_local.py), [build_probe.py](build_probe.py) y [LiveComparison.hpp](LiveComparison.hpp).

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
