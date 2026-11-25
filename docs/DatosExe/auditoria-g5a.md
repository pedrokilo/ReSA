<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoria de «dato en dos sitios» en DE05, lotes 1-30 (rama `datos-g5a`, base `48e53f065c`, 24/11/2026)

Alcance: los 1.143 objetos `.bss` de los lotes DE05-1..30 de [plan-lotes.csv](plan-lotes.csv); hoy hay **585** declarados como objetos propios `DatosExe::*` (los otros 558 se retiraron o nunca se convirtieron: siguen por `StaticRef` y no pueden estar duplicados). Los lotes 31-61 son de otro agente. Herramientas en `reversing/FnDatosG5/`.

## 1. Metodo

| Paso | Herramienta | Que detecta |
| --- | --- | --- |
| Estatico por texto | `estatico.py`, `estatico3.py`, `estatico4.py` (+ `tam_ext.py`) | citas por literal 0x8E4000-0xCB0FFF dentro de la extension de un objeto, o con un `StaticRef<T>` cuyo `sizeof(T)` (VALIDATE_SIZE, constantes, enums) abarca un objeto; literales a 1-96 bytes antes de un objeto (bloques base+desplazamiento) |
| Estatico sobre el binario | `bin_scan.py`, `bin_verif.py`, `cruce.py` | busca en el `.text` del `gta_sa_standalone.exe` (con su mapa de enlace) cada palabra de 32 bits que cae en la extension de un objeto, la verifica con capstone (operando de la instruccion) y la atribuye a la funcion: ve lo que el texto no ve (constantes plegadas, `constexpr`, ayudantes, lambdas) |
| Punteros guardados | `ptr_scan.py` | palabras de `.rdata`/`.data` inicializado del EXE original que apuntan dentro de un objeto |
| Dinamico (standalone) | `sonda_datos.py` + `ejecutar_datos.ps1` (escenarios de `FnJuego/sonda_juego.py`) | lee por memoria del propio proceso, cada 6 s, los bytes de cada objeto en su direccion propia (mapa de enlace: `b<DIR>`, `v<DIR>`, `sl_<DIR>`) y en la direccion del EXE (que es memoria cero del `.bss`); y vuelca todo el `.bss` 0x8E4000-0xCB1000 acumulando los bits distintos de cero. `analiza_snap.py` / `analiza_mask.py` clasifican |

Criterio dinamico: la direccion del EXE es memoria a cero que **solo puede cambiar si algo la escribe por literal**. Clase C = direccion del EXE escrita y objeto propio sin escribir (escritor por direccion, lector por nombre, o al reves); clase A = ambos escritos y distintos; B = solo el propio cambia (si ademas hay lector por direccion el estatico lo ve); E = no se ejercito.

## 2. Hallazgos reales (corregidos)

| # | Sitio | Direcciones / objetos | Escritor | Lector | Evidencia | Efecto |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `Hud.cpp` `CHud::DrawPlayerInfo` (lambda `fadeElement`) | bloques 0xBAA404 (arma), 0xBAA424 (dinero), 0xBAA434 (energia): `bBAA408/40C/410`, `bBAA428/42C/430`, `bBAA438/43C/440` | `fadeElement` por `StaticRef<int32>(base+4/+8/+12)` (memoria del EXE) | `DrawFadeState`, `Hud::Initialise`, `DrawWeapon` por nombre (`m_WeaponFadeTimer`, `m_LastDisplayScore`...) | dinamico: `bBAA410` = {1, 5, 0x18...}, `bBAA430` = 0x3B9AC9FF, `bBAA440` = marcas de tiempo en la direccion del EXE, objeto propio siempre 0 | el «ultimo valor» y los temporizadores de fundido de arma/dinero/energia del HUD no se reiniciaban en `Initialise` (nueva partida / carga) y `DrawFadeState` y `DrawPlayerInfo` trabajaban sobre copias distintas |
| 2 | `Interior_c.cpp` `PlaceObject`, `PlaceFurniture`, `Unfurnish` | `bBAD3EC` (12 bytes, `TList_c<FurnitureEntity_c>`) = `FurnitureManager_c::m_FurnitureList` (`g_furnitureMan` 0xBAB380 + 0x206C, `StaticRef<FurnitureManager_c>` de 0x2078 bytes) | `FurnitureManager_c::Init` llena la lista (512 elementos) en la memoria del EXE | las tres funciones leian el objeto propio, siempre vacio | dinamico: direccion del EXE = {cabeza, cola, 0x200}; objeto propio = 0. Estatico: `estatico4.py` (extension de `FurnitureManager_c` contiene el objeto) | `pool.GetNumItems() < 1` => `PlaceObject` y `PlaceFurniture` devolvian siempre `nullptr`: ningun mueble en los interiores procedurales |
| 3 | `RunningScriptCommands1400To1499.cpp` | `bC18D50<CPointRoute>` = `CTheScripts` ruta global 0xC18D50 | comandos 1400-1499 (`CLEAR_ROUTE`/`ADD_POINT_TO_ROUTE`...) por `StaticRef<CPointRoute>(0xC18D50)` | comandos 1700-1799 (l. 341) y 2000-2099 (l. 328) por el objeto propio | estatico (`estatico4.py`: la misma direccion citada por dos vias) | las tareas «conducir por ruta» recibian una ruta vacia |

(Los tres solo se detectaron combinando dinamico y estatico: el 1 no lo ve ninguna busqueda por texto porque el literal se pasa como argumento de una lambda y se suma; el 3 no se ejercito en las ejecuciones.)

## 3. Casos ya corregidos por la auditoria del 23/11 (confirmados otra vez, sin lector por nombre)

`bB61364`, `bB62C1C`, `bC0B184`, `bC0F074`, `bC0F240`, `b96913F` y `bBAADC0` aparecen como clase C en las ejecuciones (la direccion del EXE se escribe) pero **no tienen ninguna referencia por nombre** en `source/`: el objeto propio quedo sin uso y la direccion del EXE es la unica copia. No son defecto.

## 4. Verificacion tras las correcciones

* Standalone recompilado (`compilar_over` de Hud, Interior_c y RunningScriptCommands1400To1499 + `enlazar.cmd` con los `.obj` de `wv-int`): enlace `exit=0`. Ejecucion `d4fix` (escenarios base, conduccion, armas, interiores, camara, gimnasio, spawn; 587 s): vivo al final, sin excepciones ni NaN. El volcado de todo el `.bss`: `bBAA410`, `bBAA430` y `bBAA440` ya **no** se escriben en la direccion del EXE (antes si); queda escrito `bBAD3EC` (ahora es la unica copia, la lista libre de muebles) y los objetos sin lector por nombre de la seccion 3. (La comparacion con el objeto propio de esa ejecucion no es valida: `objetos_lotes1_30.json` usa las direcciones del mapa del exe anterior.)
* Ejecuciones previas con el exe sin corregir: `d1` (base, conduccion, armas, interiores, trucos, camara_aire), `d2` (camara, gimnasio, spawn, camara_aire, conduccion), `d3` (siete escenarios + volcado del `.bss`). Clase C: 8-11 objetos, siempre los de las secciones 2 y 3; clase A: 0; B: 264-283.
* ASI completo de `wv-datos2` (`bin/Debug/gta_reversed.asi`, `_build_DATOS2_ASI.sh`, EXIT=0): `prueba_ropero.py cal_mal.args` BIEN y `prueba_marcadores.py cal_mal.args` BIEN (12 marcadores que cambian). `contrib/audit_hook_catalog.py --strict`: OK.

## 5. Cobertura y limites (honesto)

* **Verificados dinamicamente**: 580 de los 585 objetos declarados (5 no estan en el mapa de enlace: el enlazador los descarta por no usarse). De esos, solo se ejercitaron (algun byte propio distinto de cero) unos 300-480 segun la ejecucion; **~100-290 nunca cambiaron** (clase E: codigo no alcanzado en las ejecuciones: garajes, guerras de bandas, repeticion, guardado, muchas misiones, audio concreto). Para esos solo vale el estatico.
* **Estatico**: citas por literal y `StaticRef` con `sizeof` calculado; 575 `StaticRef` conservan tipo de tamano no calculable (arreglos con constantes o clases sin VALIDATE_SIZE): se revisaron a mano las que quedan antes de un objeto (`estatico3.py`) y el escaneo del binario (solo 5 objetos con referencia constante dentro de su extension, todos de la seccion 3; el resto son vecinos de 1-16 bytes revisados a mano). **No se ven**: accesos con indice o base calculada en tiempo de ejecucion cuyo literal base queda a mas de 16 bytes antes del objeto, ni punteros guardados en `.bss` en tiempo de ejecucion, ni CLEO. Punteros guardados en `.rdata`/`.data` inicializado: 9 palabras, todas hacia `vC3FE08` (fuera de mis lotes).
* No se hizo la comparacion dinamica con el ASI sobre el EXE original (solo con el standalone: la direccion del EXE es memoria a cero, lo que hace innecesario el EXE original). No se uso trampa de paginas (PAGE_GUARD) para ver lecturas por literal.
* Los objetos `bBAD3EC` y los de la seccion 3 siguen declarados en `DataBss*.h` sin uso (no se tocaron los archivos generados para no chocar con el otro agente); son trampas latentes.
