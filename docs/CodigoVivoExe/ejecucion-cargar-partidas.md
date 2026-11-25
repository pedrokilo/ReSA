<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cargar partidas en el EXE independiente (gta_sa_standalone.exe)

Rama `fix-cargar-partidas` (desde `integracion-fase7b` d3ec2c9a03). Se midió con el EXE actual de `D:\Original\_fase8b_run` (copia `gta_sa_standalone_base.exe`, 22/11 15:52) en una carpeta de pruebas propia `D:\Original\_cargar_run` (junctions de solo lectura a los datos, `userfiles` propio con una COPIA de `GTASAsf2.b`). Los saves originales de Documentos no se tocaron (SHA-256 idéntico antes y después: `b089c48d…d8fa`).

## Resultado: causa de la lista vacía
**La lista sale vacía porque el standalone no mira la carpeta de Documentos.** `jugar_standalone.ps1` usa `D:\Original\_fase8b_run\gta-reversed.ini` con
`[Misc] SaveDirectoryPath = D:\Original\_fase8b_run\userfiles`, y esa carpeta no contiene ningún `GTASAsf*.b`. `InitUserDirectories` (0x744FB0, `source/game_sa/FileMgr.cpp`) respeta esa clave; sin ella usaría `SHGetFolderPath(CSIDL_PERSONAL)\GTA San Andreas User Files`. `C_PcSave::PopulateSlotInfo` solo abre `<ruta>\GTASAsf<n>.b`. No hay función sin reconstruir ni INT3: todo el bloque CGenericGameStorage / C_PcSave está reemplazado por C++ (hooks.csv, 26 funciones).

## Evidencias (todas con el EXE actual, sin cambiar código)
- Con una copia del save en `userfiles`, el menú **Cargar partida** lista la ranura 2 (nombre `OG`, `19 FEB 2009 16:37:24`) y las demás "NO HAY ARCHIVO GUARDADO" (`cargar-partidas/lista-cargar-partida.png`).
- Elegir la ranura 2 carga: el log muestra `GenericLoad` ("partida de otra versión: v1 modificada", se fuerza por `LoadSavesWithMismatchingVersion = true`; sin esa clave la carga se rechaza y se inicia partida nueva) y `TagManager: 100 tags`. Estado medido por memoria tras cargar: `gGameState=9`, jugador en (2496.11, -1701.83, 1014.73) dentro de la casa de CJ (el save tiene la cámara en (2496.1, -1706.2, 1016.1), hora 00:00 día 18; tras 10 min de juego marca 00:10), dinero 999999999 (coincide con el HUD). Captura `cargada-casa-cj.png`: CJ en el salón de Grove Street, flecha amarilla de misión sobre la puerta, HUD con 00:10 y $999999999. 20 s después (`cargada-20s-despues.png`) CJ corre por la calle de noche, reloj 00:31, radar con el icono de guardar; es el guion de la misión inicial, igual que en partida nueva.
- La carga tarda ~100 s con el menú (carga de streaming, TXD, pools); no es un cuelgue.

## Cómo usar tus partidas
Opciones (sin tocar código): copiar `GTASAsf*.b` a `D:\Original\_fase8b_run\userfiles\`, o quitar `SaveDirectoryPath` del INI para usar la carpeta real de Documentos (el juego entonces puede sobrescribir esos saves al guardar). Mantener `LoadSavesWithMismatchingVersion = true`: el save del usuario lleva un número de versión "modificada".

## NO verificado
- **Guardar y recargar**: el intento (`SaveSlot` por hilo remoto con el juego en pausa) no terminó: el proceso desapareció durante la carga previa y el driver se quedó esperando (ver abajo). No se creó ningún `GTASAsf8.b`. No hay evidencia de si guardar funciona.
- Censo dinámico (0 INT3 / 0 funciones originales): `fase8_arranque.log` de esa ejecución muestra `codigo-original-distinto=0 hits=0 excepciones=0`, pero la ejecución es la fallida; la de carga correcta no usó `--censo`.
- Carga con la carpeta de Documentos real (la ejecución se denegó por no arriesgar los originales), comparación con el ASI, humo ropero/marcadores y `audit_hook_catalog --strict` (no se cambió código del juego).
- Un fallo no reproducido: en 1 de 2 cargas por menú el proceso murió/colgó hacia los 120 s (el log termina tras `TxdStore slot 12`, sin backtrace). Puede ser un fallo real intermitente de la carga; sin causa hallada.

## Driver de pruebas
`reversing/FnCarga/correr.py` (guion JSON, teclas por scancode, capturas con `captura.py`, lectura de memoria, `guardar`). Tras el incidente que retuvo `gta_lock`, lleva un **plazo duro** (`CARGA_PLAZO_S`, 420 s por defecto): mata solo su proceso, borra su candado y sale; también limita la espera del candado (30 min) y de otros juegos (20 min).
