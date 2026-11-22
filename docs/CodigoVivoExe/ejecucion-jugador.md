# CJ no responde a la entrada en el EXE independiente (`gta_sa_standalone`): causa real y arreglo (22/11/2026, rama `mover-jugador`, base `1ec835719c`)

## 1. Resumen

CJ **sí responde a la entrada** en el EXE independiente. La sospecha inicial (sección 8.4 de `fase8-experimento.md`: «el jugador no se mueve») era un **artefacto de la prueba**, no un defecto de entrada, de `MainScriptSize` ni del script principal:

1. La partida nueva empieza con la **cinemática de apertura** (`CCutsceneMgr::ms_running = 1`, `CPad::DisablePlayerControls = 160` = `bPlayerSafe | bPlayerSafeForCutscene`) durante unos **107 s de tiempo de juego**. Todas las pruebas anteriores (`prueba_partida_nueva.py --entrada` a los 40-60 s, el volcado de la ronda 3 a los 74-80 s) inyectaron la entrada **dentro de la cinemática**: el jugador está congelado por el script y el juego ignora el pad (`GetPedWalkLeftRight` y compañía devuelven 0 si `DisablePlayerControls != 0`).
2. Después viene el **trayecto en el coche de policía** de la misión «Intro» (CJ pasajero, `m_pVehicle != 0`, `DisablePlayerControls = 32`, `pedState = 50`), unos 60 s más con recorrido de `CarRec`, y un **paseo guiado por script** (CJ camina solo ~10 s, `speed = 0.03`, `DisablePlayerControls = 32`). Solo cuando el script devuelve el control (`DisablePlayerControls = 0`, tiempo de juego ~196 s) CJ obedece.
3. Medir eso exigía **llegar vivo a los ~200 s**, y el EXE independiente **caía antes** (a los 114-316 s según la ejecución) por un defecto distinto, de nuestro código: **ranuras de las vtables del EXE sin gancho** (sección 3). Por eso nadie había visto nunca la fase con control.

## 2. Medición (por memoria y por captura)

Herramienta nueva: `reversing/FnFase8/observar_jugador.py` (solo lee/escribe el proceso lanzado desde este árbol: filtra por `wv-move` en la línea de comandos; capturas en `D:\Original\sim_out\mv_<etiqueta>_antes|despues.png`). Se lanza `ejecutar_f8b.ps1 -Etiqueta <e> -Limite 600 -Extra "--shims"` y, en paralelo, `python observar_jugador.py <etiqueta> 590 --entrada --modo tecla`.

Línea de tiempo medida (partida nueva, `--shims`):

| tiempo de juego | `cs` | `DisablePlayerControls` | `m_pVehicle` | qué pasa |
| ---: | ---: | ---: | --- | --- |
| 0-2 s | 0 | 0 | 0 | `gGameState = 9`, CJ en (2488.9, -1666.9, 13.3) |
| 2-107 s | 1 | 160 | 0 | cinemática de apertura; sin movimiento ni respuesta (esperado) |
| 107-165 s | 0 | 32 | coche | CJ pasajero del coche de policía (de 2429,-1253 a 2170,-1317) |
| 165-188 s | 0 | 32 | 0 | paseo guiado por script (velocidad 0,03) |
| **> 196 s** | 0 | **0** | 0 | **control del jugador** |

Con control libre (misma posición de partida, `tms ≈ 300 s`):

* **Modo `pad`** (escribir `-128` en `CPad::Pads[0].NewState.LeftStickY` cada 20 ms, como hacía `prueba_partida_nueva.py`): CJ avanza ~1,6 m en 4 s. Es solo una respuesta parcial porque `CPad::UpdatePads` sobrescribe `NewState` en cada fotograma: es una carrera, **no** un defecto del juego.
* **Modo `tecla`** (`keybd_event` de **W** con la ventana al frente): la tubería real de entrada del EXE propio (WndProc -> `CPad::NewKeyState` -> `CPad::UpdatePads` -> `NewState.LeftStickY = -128` -> `CPlayerPed`) funciona: la sonda lee `sticks = (0, -128, 0, 0)` y CJ recorre **~17 m en 4 s** (2240.9 -> 2257.5 en X, corriendo). Capturas `mv_e2_antes.png` / `mv_e2_despues.png`.

### `MainScriptSize` (la sospecha del encargo)

`DatosExe::Bss::bA444C4` (y `bA444C0`) son **objetos C++ propios**; no residen en la dirección del EXE `0xA444C4`. Leer esa dirección da 0, pero **todo el código del proyecto accede por la referencia** (`CTheScripts::MainScriptSize = DatosExe::Bss::bA444C4`), que sí vale lo que carga `main.scm`; el script principal se ejecuta (IP avanzando, `intro`, `a_cont`, `dealer`, `tri`... activos), así que no es la causa. Solo engañaba al volcado diagnóstico (`VolcarScripts` en `StandaloneExe.cpp`), que leía la dirección fija: ahora imprime el objeto propio y, aparte, lo que hay en la dirección del EXE. Si algún día un código original (o una sonda externa) lee `0xA444C4`, verá 0: es la misma desincronía de «copias de datos» ya documentada en 8.7, inocua mientras solo C++ lea esas variables.

## 3. Defecto real encontrado: ranuras de vtable del EXE sin gancho (arreglado)

Las clases de eventos (y otras) fijan la vtable de **los clones** a la del EXE (por ejemplo `CEventAcquaintancePedDislike::CloneEditable` hace `*(uint32*)memory = 0x86CAC8`). Con el ASI, las ranuras sin gancho de esa vtable apuntan a código original que existe; en el ejecutable independiente apuntan a **INT3**. Caídas medidas (todas «llamada a código original sin reemplazo», `codigo 0x80000003`):

| ejecución | dirección | qué es |
| --- | --- | --- |
| `mv2` (164 s) | `0x5FF1C0` | `CEventAcquaintancePedDislike::GetEventType` (ranura 1 de `0x86CAC8`; `mov eax,25h; ret`) |
| `mv3` (114 s) | `0x5FF100` | `CEventAcquaintancePedLike::GetEventType` (ranura 1 de `0x86CA80`) |
| `mv4` (316 s) | `0x4AFB40` | `CEventSeenCop::AffectsPed` (ranura 5 de `0x86CB58`; `jmp 0x4AFA30`) |

No son 3 huecos sueltos: `docs/DatosExe/huecos-alcanzables-entradas.csv` lista 253 entradas «vtable de clase C++ (DE09)» (88 en eventos, 55 en plugins...) y un barrido propio de las 469 vtables declaradas con `RH_ScopedVirtualClass` halla ~90 ranuras sin gancho solo en `Events/` (virtuales en línea de las cabeceras, constantes `mov eax,N; ret`, thunks `jmp`; incluidos los de 5 bytes que el catálogo no mide).

**Arreglo (genérico, en vez de ~250 ganchos manuales)**: `source/reversiblehooks/ReversibleHooks.cpp`. `InstallVirtual` registra, una vez por vtable del EXE, el par (vtable del EXE, vtable propia de la clase, nº de ranuras) y `OnInjectionEnd` ejecuta `FillAbsentVirtualSlots()`, que copia a cada ranura de la vtable del EXE cuyo destino sea INT3 (primer byte `0xCC` dentro de `0x401000-0x857000`) la entrada de la vtable que el compilador genera para la clase propia. Debe ir **al final** de la instalación: `InstallVirtual` busca la dirección original dentro de la vtable del EXE y una ranura ya sustituida no se encontraría. **Con el ASI no hace nada**: solo actúa si el primer byte de `0x401000` es `0xCC` (EXE sin `.text`); con el EXE real el vector se vacía sin tocar nada.

Alcance y límites: cubre las clases que tienen al menos un `RH_ScopedVMTInstall` (casi todas las de `RH_ScopedVirtualClass`; sin ningún gancho VMT la clase no pasa por `InstallVirtual`). Las vtables con ranura 0 (destructor con borrado) ya están enganchadas por salto como «S» y no se tocan (su primer byte ya no es `0xCC`).

## 4. Verificación

* **Medido** (ejecuciones `mv1`-`mv5`, sin el arreglo de vtables salvo los ganchos manuales de prueba, luego descartados): sección 2. Control libre del jugador a los ~196 s de juego; con la tecla W CJ recorre ~17 m en 4 s.
* **Pendiente de medir tras el arreglo definitivo** (`FillAbsentVirtualSlots`, commit de esta rama): la ejecución `mv6` (y las pruebas de humo del ASI: `prueba_ropero.py`, `prueba_agua.py`, `prueba_marcadores.py` con `AGUA_ASI=D:\Original\wv-movein\Debug\gta_reversed.asi`) no se pudo lanzar: `D:\Original\gta_lock\owner_bis1` (creado a las 10:03:56 por `FnBIS1\jugar_nueva.ps1`, variante X) seguía en pie sin ningún proceso de juego ni de `jugar*` vivo (lock huérfano) y bloqueaba a todos los lanzadores. Compilado: ASI (`_build_MOVE.sh`, `EXIT=0`, `bin\Debug\gta_reversed.asi`) y EXE independiente (`_cc_MOVE.sh`, `EXIT=0`). `python contrib/audit_hook_catalog.py --strict`: OK.

## 5. Cómo reproducir

```
bash /d/Original/_build_MOVE_link.sh                                   # enlaza gta_sa_standalone.exe (obj de wv-int + over\)
bash /d/Original/_cc_MOVE.sh source/reversiblehooks/ReversibleHooks.cpp  # recompila un .cpp a over\ y reenlaza
cd reversing/FnFase8
pwsh -File ejecutar_f8b.ps1 -Etiqueta mvN -Limite 600 -Extra "--shims"   # (otra consola) 
python observar_jugador.py fN 590 --entrada --modo tecla                 # --modo pad|tecla|ambos
```
