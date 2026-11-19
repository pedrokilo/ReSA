# BIS1: asistente de bisección para el fallo «mundo en blanco / posición NaN / caída a través del mapa»

Objetivo: aislar con el mínimo esfuerzo QUÉ funciones reconstruidas provocan el fallo que el usuario ve jugando con la DLL completa (y nunca con el juego original). Hay dos modos:

* **Interactivo** (`biseccion.ps1`): el usuario juega cada ronda (roba coches con ocupantes, sale de la casa de CJ) y cierra la ventana. Es el modo de respaldo y el que garantiza la señal real.
* **Automático** (`biseccion.ps1 -Auto`): la propia DLL crea un coche con conductor junto a CJ y le hace robarlo, cada N segundos, sin teclas. **Estado: implementado y compilado, SIN medir todavía** (ver «Pendiente»).

## Comandos exactos

```
cd D:\Original\wv-8\reversing\FnBIS1
pwsh -File biseccion.ps1            # interactivo (o doble clic en biseccion.bat)
pwsh -File biseccion.ps1 -Auto      # automático (solo si la tasa medida con -Robar es alta)
pwsh -File biseccion.ps1 -Restaurar # restaura la copia de pruebas (por si se cortó algo)
pwsh -File biseccion.ps1 -Simular   # prueba el circuito sin jugar (oráculo simulado; ~50 s por ronda por copiar el PDB)
pwsh -File biseccion.ps1 -Reiniciar # empezar de cero (el estado anterior queda en estado.json.bak)
pwsh -File robar_serie.ps1 -Variante A -Veces 3     # mide la tasa del modo automático con la DLL completa
pwsh -File robar_serie.ps1 -Variante B -Veces 3     # control: --unhook-all
```

Ctrl+C en cualquier momento; al volver a ejecutar sigue donde estaba (`estado.json`). La copia de pruebas (`E:\JUEGOS\GTA San Andreas - CVehicle Test`) se restaura siempre (copia de seguridad con hash de ASI/PDB/INI, igual que `FnPL1\jugar.ps1`). Nunca se toca `E:\JUEGOS\GTA San Andreas`. El juego toma el candado `D:\Original\gta_lock` (si otra prueba lo tiene, espera).

## Qué hace cada ronda (interactivo)

1. Muestra «Ronda X de ~N» y qué hacer: «roba coches CON OCUPANTES junto a la casa; cuando salga el error, cierra la ventana (si no, se cierra sola a los 5 minutos)».
2. Escribe la lista de grupos a apagar en `rondas\rNNN.unhook.txt` y lanza `jugar.ps1 -Variante A -ArgsFile ... -Auto 300 -Sonda -ParaAlCaer -MinCaidas 3` (se cierra solo al llegar a 3 avisos).
3. Lee SOLO el tramo nuevo de `scripts\logs\log.log` (en flujo) y decide:
   * **ERROR** si hay ≥3 avisos «ha caído a través del mapa», o NaN en la posición (la sonda lee la posición de CJ cada 5 s; en modo automático también la comprueba la DLL), o salida con 0xC0000005, o z < -50 en el jugador/coche del modo automático.
   * Sin señal y ronda de **<60 s**: no cuenta (se repite).
   * Sin señal y ≥60 s: pregunta UNA vez con una tecla: `s` = hubo error, `n` = NO hubo error y robé ≥3 coches con ocupantes, `d` = descartar la ronda. Una ronda sin error solo cuenta como OK con la `n` (evita falsos negativos como el BB4).
4. Guarda ronda, duración, caídas, robos, decisión y hora en `estado.json`.

## El algoritmo (y cuántas rondas esperar)

Notación: una ronda **apaga** un subconjunto X de grupos (usan el código original del EXE) y deja el resto encendido. Resultado `E` = el fallo SIGUE (hay algún culpable encendido); `O` = no aparece (todos los culpables están en X). Modelo OR: el fallo aparece si queda encendido AL MENOS un culpable; buscamos el conjunto MÍNIMO a apagar que lo elimina (puede haber varios culpables).

`ddmin.py` (`planificar`):

1. **Línea base**: todo encendido debe dar `E` (si no, no hay señal y se para).
2. **Extracción de culpables por prefijos** (búsqueda binaria): con F = culpables ya hallados (siguen apagados en todas las rondas siguientes) y R = grupos por mirar, se busca el prefijo más corto de R que, apagado junto a F, elimina el fallo; su último grupo es culpable (≈log2 |R| rondas por culpable). Se repite mientras apagar F no baste.
3. **ddmin de complementos** sobre F: se quita cada culpable y se comprueba que apagar el resto sigue dando fallo (si da `O`, sobraba).
4. **Confirmación**: apagar solo F debe dar `O` **dos veces** (descarta un falso negativo).
5. **Falsos negativos**: un `E` es fiable; un `O` puede ser falso (no apareció el fallo por azar). Se aplica la inferencia monótona (un `E` anula cualquier `O` de un subconjunto) y, si tras hallar un culpable apagar F no basta, se repite la ronda `O` de la frontera (2 observaciones; 3 para fronteras anteriores si hay cascada). Si una ronda larga se queda sin converger se avisa y se puede reiniciar.

El estado es solo el historial de rondas; el algoritmo (determinista) se reejecuta contestando con lo ya sabido, así Ctrl+C y reanudar son seguros y las rondas inferibles no se repiten.

Dos fases con el mismo motor: **rutas** `Categoría/Clase` (985 candidatos) y, al terminar, **funciones** de las clases culpables (rutas `Categoría/Clase/Función`, nombres reales del volcado de hooks de la DLL, `FnPL1\clases_dump.csv`).

**Rondas esperadas** (simulador, 985 grupos, sin ruido): 1 culpable ≈ **13** rondas por fase (1 base + ~10 búsqueda + 2 confirmaciones); 2 culpables ≈ 24; 3 ≈ 36. Con 10 % de falsos negativos: 1 culpable ≈ 24 de media (mediana 13, p90 38), acierto ≈ 93 %; 2 culpables ≈ 39 (mediana 32), acierto ≈ 93 %; 3 ≈ 53 (mediana 47), acierto ≈ 95 %. Cuando falla, lo normal es que no confirme («NO CONFIRMADO») en vez de dar un resultado erróneo con confianza. La fase de funciones suma ~7-13 rondas más (cientos de funciones por clase). Total típico con un culpable: **~20-26 rondas**; las rondas con error son cortas (se cierra solo a los 3 avisos); las rondas OK son las de 5 minutos.

`simular.py [N] [pruebas] [ruido]` reproduce estas cifras (`python simular.py 985 200 0.10`); `simular.flujo_completo(k, ruido, semilla)` recorre el circuito real de `ddmin.py` (estado, fase de rutas, bajada a funciones y resultado): con 1 culpable 13 rondas de rutas + 4 de funciones (17); con 2 culpables 25 + 7 (32). `biseccion.ps1 -Simular` recorre el circuito PowerShell completo (copia/restauración por hash con `jugar.ps1 -Simular`, la DLL nueva `fbf61e4c...` instalada, argumentos de cada ronda comprobados) con un oráculo oculto; se probó hasta la ronda 9 de rutas con 2 culpables y 10 % de ruido y la bajada a funciones.

## Qué se excluye de los candidatos (22 rutas)

* `App/*` (8), `Win/*` (3): arranque de la aplicación, ventana y entrada.
* `Memory/*` (2): asignador de memoria (mezclar `new`/`delete` del EXE y de la DLL entre clases es lo único que podría impedir arrancar).
* 9 rutas que el catálogo de fuentes menciona pero la DLL no registra (`Global/cls`, `Global/name`, `CTaskComplexFollowNodeRouteShooting`, …): no se pueden apagar.
* Las tres `CLinkList<...>` sí se incluyen (el catálogo las truncaba; se completan a `CLinkList<X*>`).

El juego arranca con TODO apagado (`--unhook-all`, variante B de PL1/SIM1: confirmado por ellos); por eso lo excluido solo evita mezclas absurdas, no es imprescindible para arrancar.

## Cambios en la DLL (nueva `dll\Debug.asi`)

Mínimos y sin alterar el comportamiento por defecto (ver `git show` del commit):

* `--unhook-file=<archivo>`: igual que `--unhook=`, con una ruta por línea (sin límite de longitud de línea de comandos, admite nombres con espacios como `operator new`).
* **Apagado por función**: ya existía. `--unhook=Categoría/Clase/Función` apaga un solo hook (`SetCategoryOrItemStateByPath` busca el elemento en la clase), así que no hace falta `--unhook-addr`.
* `--robar=ciclos[,periodo_s[,espera_s[,conducir_s]]]` (`source/extensions/AutoSteal.cpp`): modo automático. Sin ese argumento no hace nada (una comparación `bool` por fotograma en `Idle`).

La DLL de BIS1 se compiló desde el código del worktree (HEAD de main + estos cambios): **no es byte a byte la `ff45cdb1...` del usuario**. `jugar.ps1 -Variante O` lanza la original (sin `--robar` ni `--unhook-file`). Hash de la nueva: `dll\Debug.sha256`.

## Modo automático (`--robar`), cómo funciona

`AutoSteal::OnFrame()` se llama una vez por fotograma desde `Idle` (hilo del juego). Con `--unhook-all` el `Idle` de la DLL no se ejecuta, así que, si `Idle` (0x53E920) no está enganchado, se instala un desvío propio de 5 bytes sobre el `Idle` ORIGINAL que llama a `OnFrame` después de él (el control con `--unhook-all` ejecuta así el mismo escenario con código del EXE).

Por ciclo (espera inicial desde que CJ está en el exterior, luego cada `periodo_s`): carga el modelo (`CStreaming::RequestModel` + `LoadAllRequestedModels`), crea el coche a 5 m delante (`CCarCtrl::CreateCarForScript`), le pone conductor (`CPopulation::AddPedInCar`), lo desbloquea y asigna a CJ `CTaskComplexEnterCarAsDriver` (tarea primaria, el mismo camino que al pulsar la tecla); espera a que CJ esté dentro, acelera el coche `conducir_s`, saca a CJ (`CTaskSimpleCarSetPedOut`) y borra coche y ocupantes (`CWorld::RemoveVehicleAndItsOccupants`). Cada fotograma observa NaN y z < -50 en CJ y en el coche. Líneas de log: `[ROBAR] ciclo i/N: robado=si|no nan=SI|no z_bajo=SI|no pos=...` y `[ROBAR] FIN ciclos=... robados=...`. Para sacar a CJ de la casa se reutiliza `-TpExit` de la sonda de PL1 (lo coloca en el marcador de salida a los 25 s: la salida real del juego).

## Pendiente (no hecho en esta sesión)

* **Medir la tasa** del modo automático (`robar_serie.ps1`, ≥3 ejecuciones de ≥20 ciclos con la DLL completa y con `--unhook-all`): el entorno denegó lanzar el juego desde el agente, así que el modo `-Robar` está compilado pero **no se ha ejecutado dentro del juego**. Si la tasa con la DLL es alta y 0 con `--unhook-all`, usar `biseccion.ps1 -Auto`; si no, el modo interactivo (que sí está completo).
* Prueba corta real de arranque con «todo apagado salvo arranque» y con una mitad apagada (la lógica y los argumentos sí están verificados con `-Simular`).

## Archivos

`biseccion.ps1/.bat` (interfaz), `ddmin.py` (motor y estado), `simular.py` (oráculo simulado), `jugar.ps1` (copia ampliada del de FnPL1: `-Robar`, `-MinCaidas`, DLL nueva/original), `robar_serie.ps1` (tasas), `estado.json` (estado; local), `rondas\`, `salidas\` (locales), `resultado.md` (al terminar).

## Modo automatico (-Auto): senal neutral

`biseccion.ps1 -Auto` usa la DLL `dll\Debug.asi` y decide SOLO con la senal neutral: ciclos con z < -50 (jugador, coche o conductor, medido por `AutoSteal`) o NaN en la posicion/matriz del jugador o del coche, y NaN en la posicion de la sonda externa. NO usa los avisos "ha caido a traves del mapa", `Pool.h:207` ni el crash, porque son codigo de la DLL y desaparecen con `--unhook-all`/al apagar su grupo. Una ronda sin senal solo cuenta como OK con >= `-MinRobosAuto` (12) robos automaticos; si no, se descarta. `jugar.ps1 -Robar` cierra el juego al primer ciclo con z bajo/NaN (`-NoParaSenal` lo evita). Por defecto `-Robar 20,15,15,5` y el juego se cierra a los 480 s como maximo.

## El control (--unhook-all) necesita la ventana del juego en primer plano

Con todos los hooks apagados se ejecuta el menu y el bucle ORIGINALES del EXE, que no avanzan si la ventana del juego no tiene el foco del sistema (DirectInput da `Acquire() acceso denegado`); el juego se queda en `FRONTEND_IDLE` y el control no hace ningun ciclo (con los hooks encendidos, la DLL no tiene esa dependencia). `jugar.ps1` (ejecuciones con `-Auto`, salvo `-NoEnfocar`) llama cada 30 s a `enfocar.py`:

* si el usuario lleva **>= 600 s** sin tocar teclado ni raton (`GetLastInputInfo`), trae NUESTRA ventana del juego al frente (`AllowSetForegroundWindow`, `AttachThreadInput` al hilo del primer plano, pulsacion de Alt, `ShowWindow(SW_RESTORE)`, `BringWindowToTop`, `SetForegroundWindow`);
* si el usuario esta activo NO roba el foco: escribe en consola y en `enfocar.txt` de la carpeta de salida el aviso «el juego espera estar en primer plano: haz clic en la ventana del juego» y reintenta a los 30 s.

Para el control con el usuario delante basta hacer clic en la ventana del juego al arrancar. `--robar` mantiene ademas `ForegroundApp (0x8D621C) = 1` con un hilo (para que el bucle no se pause); no basta por si solo. El 5.o campo de `--robar` (`--robar=N,p,e,c,1`) reactiva hooks de menu/carga («andamio»): experimento que NO resolvio el control; se deja apagado.
