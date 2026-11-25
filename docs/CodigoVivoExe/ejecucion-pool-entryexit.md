<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Pool de CEntryExit: `Falló la reserva para el tipo "class CEntryExit"` (22/11/2026)

Rama `pool-entryexit` (desde `745cee8725`). Solo diagnóstico: no cambia código de producción.

## Qué es el mensaje

`Pool.h:207` es un registro propio de la DLL (`NOTSA_LOG_ERR`, primer fallo de cada tipo). El EXE original no escribe ningún
registro, así que "el original no lo imprime" es cierto por construcción y no compara nada. Lo que hay que comparar es si
`CPool::New` devuelve nulo en el original, y eso solo ocurre con el pool lleno.

## Qué falla exactamente (medido con instrumentación temporal, ya retirada)

Partida nueva con la DLL de esta rama (ini-actual, `DIAG-ENEX` en `AddOne`/`DeleteOne` y una pila en el altas número 600):

- `Init` crea el pool una sola vez, con capacidad 400 (igual que 0x43F880). No hay doble reserva ni índice ocupado: `AddOne` pide
  siempre `New()` (0x43FA00) y el único `Init` es el de `CGame::Initialise`.
- Las altas de los IPL de texto dejan **376 de 400** plazas ocupadas (`data/maps/**/*.ipl`: 376 líneas `enex`). Quedan 24 de margen.
- El resto de altas vienen de los efectos 2dfx `ENEX` de modelos (casas "burglary" `LAHS1A`/`LAHS1B`, flags 0x1406), creados en
  `CEntity::CreateEffects` al instanciar el objeto RenderWare y borrados en `DestroyEffects` (`DeleteOne`).
- Durante el vídeo/cinemática de introducción (`CCutsceneMgr::ms_cutsceneProcessing = 1`) la cámara usa `farClip = 1500` y
  `LODDistMultiplier = 1.8` y `CStreaming::ms_rwObjectInstances` (1000 enlaces, 0x5B8E54) está lleno: cada fotograma
  `SetupBigBuildingVisibility`/`SetupMapEntityVisibility` -> `CreateRwObject` -> `CStreaming::AddEntity` expulsa la entidad más antigua
  (`DeleteRwObject` -> `DestroyEffects`). Resultado: ~2500 altas y ~2490 bajas de `LAHS1A/B` en 9 s (~290 altas/s). Cada alta se hace
  antes de la baja que la compensa, así que la ocupación sube de 377 a **399/400** (máximo medido) y baja a 387-391 al acabar.
- En esta prueba no se llegó a 400 (0 altas fallidas), pero en la partida del usuario sí una vez (12:05:17 y 12:31:49, justo al acabar
  la introducción). La alta que falla es una de esas casas vecinas, no la puerta de CJ.
- No hay fuga: altas dinámicas 2508, bajas 2494; la diferencia (14) son las casas que siguen cargadas.

## ¿Lo hace igual el original?

- No se pudo ejecutar el control B de esta rama: con `--unhook-all` o `--unhook-except=RenderWare/WindowedModeHooks` el juego se cae al
  arrancar dentro de d3d9 (0x7f6784 del EXE; la rama de enlace de D3D está en marcha), y con desactivar solo Renderer/Streaming/Entity
  se cae en `CLinkList::Insert`. Por tanto **no se midió el original en esta rama**.
- Evidencia previa (nota 07/10/2026 en `reversing/CEntryExitPool/README.md`): con el EXE solo (`--unhook-except=Win/Win`, partida nueva)
  el pool llega a 400 de 400 en los primeros minutos; una traza de 444 altas / 25 bajas / 19 fallidas fallaba igual en el original.
- Estáticamente, `Init` (0x43F880), `AddOne` (0x43FA00), `DeleteOne` (0x43FD50) y `CStreaming::AddEntity` (0x409650) coinciden con el EXE
  (bancos EnExInit, FnXXX); `ms_rwObjectInstances` tiene 1000 enlaces en ambos.

Conclusión: **no se encontró ninguna diferencia con el original** que cause el fallo; es agotamiento del pool de 400 plazas (376 fijas
+ 2dfx transitorios) durante el pico de la introducción. No se tocó el límite (agrandarlo sería una desviación deliberada ya
descartada el 07/10/2026).

## Los marcadores de entrada/salida (verificación visual)

Prueba `verif_marcadores.py` (partida cargada, CJ colocado delante de la puerta de su casa -- enex `CARLS` (2495.3, -1690.8, 14.8) --
y de la tienda `BAR2` (2309.6, -1643.6), con `DIAG-MK` en `CEntryExitManager::Update`):

- La entrada existe en el pool (376 entradas, `CARLS` y `BAR2` presentes).
- `Update` **sí coloca el marcador**: motivo 0 (pasa acceso, área, visibilidad, hora y distancia) y `PlaceMarkerCone(.., 2.0, 255,255,0,255, 1024, 1.0, 5, 0)`.
  Los argumentos coinciden con el original (desensamblado de 0x440D10 hasta la llamada a 0x726D40: color amarillo, no rojo).
- `C3dMarkers::m_aMarkerArray` contiene un cono (tipo 6) amarillo, alfa 255, junto a la puerta en las cinco posiciones de la tienda y en 2
  de las 5 de la casa (en las otras CJ cayó al suelo/otra zona por mi teletransporte).
- `CSprite::CalcScreenCoors` proyecta el marcador al centro de la pantalla encima de la puerta (casa de CJ: sx = 400, sy = 207-248 en
  800x600, es decir (320, 166-198) en la captura de 640x480).
- **Pero la captura de la ventana completa (640x480, área cliente) no muestra ningún píxel amarillo a menos de 50 px del punto
  proyectado** (0 en `casaCJ_0` y `casaCJ_1`; media RGB alrededor ~ (25,22,19)). El marcador está en memoria, colocado y proyectado
  dentro del encuadre, y no se dibuja: es un fallo de dibujo (`C3dMarkers::Render`), no del pool ni de `CEntryExitManager`.

Capturas y registros: `D:\Original\_evidencia_pool\` (`marc_M5_casaCJ_{0..4}.png`, `marc_M5_tienda_{0..4}.png`, `vm_M5.txt`,
`diag_marcadores_log.txt`, `diag_ctx_stk.txt`, `diag_enex_poolA.txt`).

## Notas para reproducir

- En esta rama `mp_poolEntryExits` (`DatosExe::Bss::b96A7D8`) ya no está en 0x96A7D8: las sondas externas que leen esa dirección ven nulo.
- La cámara de seguimiento no gira al escribir el rumbo del peatón; la prueba coloca a CJ al sur de la puerta (la cámara mira al norte).
