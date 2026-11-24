# Datos en dos sitios: G6 (DE06), G7 (DE07) y G8 (DE08) (24/11/2026, rama `datos-g6g8`, base `48e53f065c`)

Alcance: los grupos delicados del plan ([plan-lotes.md](plan-lotes.md)): G6 = datos del juego que aun lee codigo vivo del EXE (20 objetos), G7 = RenderWare y D3D (619), G8 = CRT, x87 y bibliotecas (165). G9 (vtables) y G10 (proteccion) quedan fuera por diseno. Objetivo: casos reales de «dato en dos sitios» (el C++ guarda un objeto `DatosExe::*` y otro codigo lee/escribe la direccion del EXE, o al reves) y datos que el independiente necesita y no tiene.

## 1. Metodo

Estatico (herramientas nuevas en `contrib/auditoria_datos/`):

1. `citas_objetos.py`: literales `0x858000-0xCB0FFF` de `source/` (sin comentarios) que caen dentro de un objeto propio no constante. 37 citas; todas son las ya conocidas de `docs/CodigoVivoExe/auditoria-datos.md` (11 casos corregidos) o tablas con indice sin lector por nombre.
2. Solape objeto-objeto con tamanos exactos del PDB (`simbolos_pdb.py`, dbghelp): 42 solapes; 16 son pares `R5`/`Rdata` de la misma constante (mismo valor), 25 son el relleno de los `Slot<T>` de `DataBssF` (el almacen tiene 16 bytes alineados, la variable es menor) y 1 es real pero inocuo: `bC4032C` (Bss) y `vC4032C` (R5) son dos objetos de la misma direccion 0xC4032C; solo se usa `vC4032C` (`PostEffects.cpp:1115`).
3. Lectores vivos del EXE: de los 2157 objetos propios, solo 44 caen en una direccion que una funcion viva del EXE toca (`objetos.csv`: `funcs_exe_vivas`). 35 son constantes `.rdata` (mismo valor en los dos sitios), 3 son `R5` constantes y 6 son mutables: `bB5F878`, `bB70098`, `bB72C70`, `bC920F0`, `bC920F4`, `bC97C34`. Desensamblado capstone de las funciones vivas que las leen: 0x4D6450/0x4D6510/0x4D66A0 (`ClumpOffset`, «usercall cubierta»), 0x5E37C0, 0x745CC0/0x745DC0 (dialogo de modo de video), el funclet `Unwind@0083C870` y `_rwD3D9CameraBeginUpdate` 0x7F8F20 (enganchada). Ninguna tiene llamadores vivos (`funciones-vivas.csv`: llamadores 0, alcance «nadie»), salvo `bC97C34`, que ya se corrigio (`WindowedMode.cpp`/`rwcore.cpp` usan la direccion).

Dinamico (nuevo, `comparar_objetos.py` + `ejecutar_g68.ps1` / `ejecutar_asi_g68.ps1`): un proceso externo de solo lectura abre el juego en marcha (partida 2, copia propia en `D:\Original\_g68_run`, candado `gta_lock`) y, para cada uno de los 2157 objetos `DatosExe`, lee tres veces (t = 45, 85, 120 s) los bytes del objeto (direccion = base del modulo + RVA del PDB) y los de la direccion original del EXE. Despues `clasificar_diferencias.py` cuenta, para cada objeto que difiere, sus usos por nombre y sus citas por literal. Ademas vuelca la memoria de las 804 direcciones de G6-G8 (`va_dump.json`) para compararla entre el independiente y el ASI sobre el `gta-sa.exe` original (mismo guion).

* Independiente (`c3`, 125 s, sin caidas, `gGameState` 9): 535 de 2157 objetos difieren de su direccion. Clase A (por nombre Y por literal): 1; B (solo literal; el objeto sobra): 15; C (solo por nombre): 512; D: 11.
* ASI sobre el EXE original (`a1`): 537 difieren; misma distribucion (A 1, B 15, C 513, D 11).
* La unica A es `bA444C4` (`MainScriptSize`): solo la lee por direccion el volcado de diagnostico de `StandaloneExe.cpp` (ya documentado en ese archivo como irrelevante). Las B son exactamente los casos corregidos antes (`bC0F240`, `bC0F074`, `bC0B184`, `bA9B76C`, `bB62C1C`, `bB61364`, ...) mas tablas leidas por literal. Las C son objetos que solo toca C++ por nombre: la copia de la direccion queda obsoleta pero nadie la lee (dos de ellas, `bB5F878` y `bC920F0`, tienen lector vivo, pero muerto: ver arriba).
* No aparecio ningun par «nombre + literal» nuevo en G6-G8.

## 2. Hallazgo real: la semilla de `rand` del EXE nunca se siembra

El EXE original, en `GameInit` (0x5BF3B0), llama `srand` del CRT del EXE (0x821B11) con `RsTimer()` (desensamblado: `call 0x619410; push eax; call 0x821b11`). La rand que usa el juego en sus rutas «del EXE» (`CarCtrl`, `AutoPilot`, `Cam`: `ExeCrt::Rand`, 0x821B1E) lee su semilla del bloque por hilo del EXE (`ptd->holdrand`, que se crea a 1). `GameInit` en C++ (`app_game.cpp`) llamaba el `srand` del CRT de la DLL/independiente, otro estado: la semilla del EXE quedaba en 1 (solo `CarCtrl` la reasigna con `srand(+0x45E del vehiculo)` en casos concretos). Efecto: las decisiones aleatorias de trafico (modelos, rutas, adelantamientos, sucesos 1/1024...) arrancaban siempre con la misma secuencia en cada partida.

Arreglo (commit `22d150f87c`): `GameInit` siembra las dos, `ExeCrt::Srand(seed)` y `srand(seed)`, con una sola llamada a `RsTimer()` (como el original). Una sola modificacion en `source/app/app_game.cpp`.

Evidencia: desensamblado del original y lectura del C++ (estatica). **No** se midio `holdrand` en ejecucion antes/despues (hace falta leer el TLS del hilo principal; no se hizo). La compilacion enlaza y el independiente y el ASI arrancan con el cambio (seccion 4).

Otros estados del CRT con dos sitios revisados sin hallazgo: `strtok` (cada llamador usa uno solo), `errno` (X87Math escribe el `errno` del EXE, nadie lo lee), argumentos (`__argc/__argv` solo los usa C++), montones (`operator new`/`free` enganchados, 100 % de la partida sin fallo).

## 3. Datos que el independiente necesita

Comparacion de las 804 direcciones de G6-G8 entre el independiente y el ASI sobre el original (mismo guion, instantanea final):

| Grupo | Seccion | iguales con datos | iguales a cero en ambos (sin ejercitar) | difieren |
| --- | --- | ---: | ---: | ---: |
| G6 | .bss | 4 | 8 | 2 |
| G6 | .data | 6 | 0 | 0 |
| G7 | .rdata | 22 | 14 | 0 |
| G7 | .data | 144 | 6 | 7 |
| G7 | .bss | 222 | 116 | 88 |
| G8 | .rdata | 69 | 1 | 0 |
| G8 | .data | 54 | 1 | 4 |
| G8 | .bss | 10 | 18 | 8 |

Las 109 que difieren son valores de ejecucion: punteros de montones y de codigo (RwEngineInstance 0xC97B24, el dispositivo D3D9 0xC97C28, listas libres 0xC97B7C-90, punteros a callbacks de `rtanim` que en un lado apuntan al C++ del ASI y en otro al del exe), contadores/indices (0xB7CB84 reloj del juego, 0xC97B94 indice de bufer dinamico, TLS 0x8E3110 = 14 frente a 6), colores y flotantes cambiantes (0xC9A5F8 alfa, 0xC87FEC, 0xC98AF8 material en cache) y punteros de buffers de E/S del CRT (0xC9AC6C en cero en el independiente: el EXE original asigna buffer a stdout, el independiente no usa `stdout`). Se miraron las 109 por su valor (los primeros 32 bytes de las de mas de 4); ninguna parece ser una constante ni una tabla del juego distinta (todo `.rdata` de G7/G8 y las tablas `.data` coinciden byte a byte). No es una prueba formal: un valor igual en las dos ejecuciones puede serlo por casualidad y el mismo valor a cero en las dos solo dice que el camino no se ejercito.

## 4. Verificacion

* `python contrib/audit_hook_catalog.py --strict`: OK.
* Independiente con el arreglo (`f1`, 125 s): `estado-juego=9`, 3.071 `Present`, `codigo-original-distinto=0 hits=0 excepciones=0`, sin caidas.
* ASI con el arreglo (relinkado con los `.obj` de `wv-int` mas `app_game.obj`, a partir de `_enlazar_asi_espejo.py`): `prueba_ropero.py cal_mal.args` BIEN (15 entidades, 12 marcadores, puerta y marcador) y `prueba_marcadores.py cal_mal.args` BIEN (12 marcadores en memoria, los 12 cambian en 1 s). Hubo intentos fallidos intermedios («el juego se cerro» a los 5,1 s) que coinciden con lanzamientos de otros agentes en el mismo minuto sobre la misma copia de pruebas (las pruebas no esperan el candado); repetidas hasta obtener veredicto. No se investigo mas.

## 6. Lo que no cubre

* Objetos sin `DatosExe` de G7/G8 (la gran mayoria: 612 de 619 en G7, 120 de 165 en G8 y 14 de 20 en G6; los que si tienen objeto, ver seccion 1): viven en la memoria de la direccion del EXE (imagen incrustada + lo que escribe el codigo por literal); no hay segundo sitio. Su valor solo se comparo en los 804 puntos de arriba, una instantanea.
* Accesos por registro base y punteros guardados en otras tablas, y rutas de juego no ejercitadas (el guion es la apertura y primeros minutos de la partida 2; sin ciudad lejana, misiones, interiores, agua ni guardar/cargar).
* Estado x87 (palabra de control, precision a 24 bits) y TLS por hilo: no se midio.
* `holdrand` en ejecucion (seccion 2).
* El PDB del ASI usado en `a1` es el de `wv-int` (09:04 del 24/11), anterior al HEAD; se comparan solo objetos con el mismo nombre en el PDB (2157 de 2157 resolvieron).

## 7. Reproducir

```
python contrib/auditoria_datos/citas_objetos.py .                                 # citas por literal dentro de objetos propios
bash /d/Original/_enl_G68.sh [archivos .cpp]                                      # compila a over\ y enlaza el independiente
pwsh -File reversing/FnTraf/ejecutar_g68.ps1 -Etiqueta c -Exe <exe> -Pdb <pdb>     # independiente + comparador
pwsh -File reversing/FnTraf/ejecutar_asi_g68.ps1 -Etiqueta a -Asi <gta_reversed.asi>  # ASI sobre el original + comparador
python contrib/auditoria_datos/clasificar_diferencias.py . <ejecucion>/diferencias.csv
```
