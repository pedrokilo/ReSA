<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SIM1: simulacro para aislar el fallo «mundo blanco / texto de misión fijo / Cargando... / posición NaN / entidades en z=-100»

Objetivo: saber si el fallo viene de las funciones reconstruidas o de algo ajeno (FastLoader, la partida «v1 modificada», la versión Debug). Las pruebas las juegas tú; el lanzador solo prepara, lanza, espera y **restaura siempre** la copia de pruebas.

Todo trabaja solo en `E:\JUEGOS\GTA San Andreas - CVehicle Test` (el script se niega a tocar `E:\JUEGOS\GTA San Andreas`). Solo reemplaza `scripts\gta_reversed.asi`, `scripts\gta_reversed.pdb`, `gta-reversed.ini` y `scripts\gta-reversed.ini`. Nunca toca partidas ni `userfiles`.

## Variantes

| Var. | Qué es | Para qué |
|---|---|---|
| A | DLL Debug completa + INI actual (FastLoader, ranura 2, partida «v1 modificada» forzada) | Referencia: reproduce el fallo |
| B | La MISMA DLL con `--unhook-all` (todo es código original del EXE; la DLL solo carga) + INI actual | **Control decisivo** |
| C | DLL Debug completa con FastLoader apagado (arranque normal por menú) | ¿Lo causa FastLoader / la partida forzada? |
| D | DLL **Release** completa + INI actual | ¿Lo causa la versión Debug (RTC, asserts, memoria rellena)? Ver estado abajo |
| E | DLL Debug completa, FastLoader con ranura 1, sin forzar versiones | Otra partida: ¿la ranura 1 es «v1 sin modificar»? Si no lo es, el juego arranca partida nueva y el resumen lo dice |

DLL Debug usada: `dll\Debug.asi` (copia de la de main, SHA256 `ff45cdb1ffca993bb66520db34fbe8dfcd2c0c97e0f09d1a19651db2b72ea278`; el lanzador comprueba el hash). INI de cada variante: `variantes\`.

## Cómo ejecutar (PowerShell 7)

```
pwsh -File D:\Original\wv-8\reversing\FnSIM1\jugar.ps1 -Variante B
```
(`A`, `B`, `C`, `D` o `E`). También sirve `jugar.bat B`. Cierra el juego con normalidad (o con el botón de la ventana) y espera a que el lanzador diga «restaurada y verificada por hash». Si algo se cortó a medias: `pwsh -File ...\jugar.ps1 -Restaurar` (también se hace solo en la siguiente ejecución). Para probar el propio lanzador sin jugar: añade `-Simular`.

El lanzador toma el candado `D:\Original\gta_lock` (si otra prueba lo tiene, avisa y no lo borra; `-SinCandado` lo omite) y se niega a arrancar si `gta_sa.exe` ya está abierto.

## Qué hacer dentro del juego (todas las variantes igual)

1. Espera a estar en el mundo libre (en A, B, D y E carga solo; en C ver más abajo).
2. Anota la **hora del juego** al empezar (reloj de la esquina) y la hora real.
3. Busca un coche con **ocupantes cerca de la casa** (CJ en Grove Street) y róbalo; conduce, vuelve, sal de la casa otra vez, etc. Sigue unos **5 minutos** (si falla antes, para ahí).
4. Si sale el fallo (mundo blanco, texto de misión que no cambia, «Cargando...», CJ sin posición, cosas que caen), anota **cuándo** (hora del juego, qué estabas haciendo: robar coche, salir de la casa, recargar) y cierra el juego.
5. Si pasan 5 minutos sin fallo, anota «sin fallo» y cierra. Repite 2-3 veces si es posible: el fallo no es determinista (una sola ejecución limpia no demuestra nada).

Variante C (menú normal): verás logos, título y menú. En el menú elige **Cargar partida** y la ranura 1 (la ranura 2 es «v1 modificada» y aquí no se fuerza: arrancaría partida nueva), o bien **Nueva partida**. Anota cuál usaste. Después haz el punto 3.

## Qué se guarda

`salidas\<variante>-<hora>\`: `resumen.txt` y `resumen.json` (argumentos, hash del ASI, INI usado, duración, código de salida, recuento de «ha caído a través del mapa», líneas con NaN y `Pool.h:207`, líneas de carga de partida como «La partida es ...», si las partidas siguen idénticas, si la restauración quedó verificada), `ultimas-300-lineas-log.txt` (cola del log de la DLL; solo se cuenta lo escrito durante esa ejecución) y `respaldo\` (copia de lo reemplazado con sus hashes). `salidas\` no se sube a git.

## Orden recomendado

1. **B** (control). 2. **A** (referencia; si B falla y A también, ya hay respuesta). 3. **C**. 4. **D** (no disponible por ahora). 5. **E**.
Haz cada una al menos 2 veces antes de concluir.

## Cómo interpretar

| A | B | Conclusión | Siguiente paso |
|---|---|---|---|
| falla | falla | No son las funciones reconstruidas: es el entorno (FastLoader, partida modificada, o el propio cargador/DLL Debug) | Mirar C, D y E para separar la causa |
| falla | no falla | Las funciones reconstruidas (o su enganche) | Bisección con el EXE como oráculo: `--unhook=<ruta>` / `--unhook-except=<ruta>` por mitades (ver `reversing/FnRW0A/bisect_auto.py`) |
| no falla | no falla | No reproducido: falta tiempo/azar | Más repeticiones, 10-15 min, repetir el mismo gesto (robar coche con ocupantes) |
| no falla | falla | Raro; el fallo es del código original bajo este cargador | Repetir; revisar que B realmente corrió con `--unhook-all` (argumentos en el resumen) |

Con A y B fallando, mira el resto:

| C (menú) | D (Release) | E (ranura 1) | Lectura |
|---|---|---|---|
| no falla | — | — | Culpa de FastLoader o de forzar la partida modificada (ruta de arranque), no del juego en sí |
| falla | — | — | Ni FastLoader ni partida forzada: mirar versión Debug (D) o la DLL como cargador |
| — | no falla | — | Versión Debug (RTC, asserts, relleno de memoria): el código está bien en Release |
| — | falla | — | No es Debug |
| — | — | no falla | La partida «v1 modificada» (script distinto de `main.scm`) es la causa |
| — | — | falla | No es la partida modificada |

Notas de lectura: el usuario ya observó que el juego ORIGINAL sin DLL, por el menú normal, no falla. B es lo más parecido a eso con el mismo cargador y partida: si B no falla pero A sí, el sospechoso son las funciones; si B falla, la diferencia está en FastLoader/partida/DLL como cargador (C y E lo separan).

## Variante D: Release (NO PROBADA, no disponible)

Estado: la DLL Release no existe, así que `jugar.ps1 -Variante D` se niega a arrancar. El intento se detuvo a petición (ahorro de CPU) antes de terminar. Lo que se logró:

- Receta (`build_release.sh`, bajo `compile_lock`, `CMAKE_BUILD_PARALLEL_LEVEL=1`, `_CL_=-MP1`): `setup.py --build` con `windows-msvc-release.txt` (igual que el perfil normal con `build_type=Release`). Las dependencias de Conan (SDL3, spdlog, imgui, ogg, vorbis, libjpeg...) SÍ compilaron en Release.
- Primer intento (`build_release_intento1.log`, local): fallan 2 cosas del fuente en Release: (1) `app_debug.cpp(289)`: `_wassert` no está declarado con NDEBUG; (2) `TaskComplexAvoidOtherPedWhileWandering.cpp(239)`: un aviso de generación de código tratado como error (`/WX`).
- Segundo intento, sin tocar el fuente: `_CL_` con `/WX-` y `/FI release_wassert.h` (declara `_wassert`). Compiló sin errores hasta donde llegó (cientos de archivos), pero se cortó antes del enlace; no hay `bin\Release\gta_reversed.asi`.
- Para retomarlo: `bash reversing/FnSIM1/build_release.sh` (varias horas con un solo hilo). Cuando exista `bin\Release\gta_reversed.asi`, la variante D funciona sin cambios. Anotar entonces SHA256 y comprobar que no hay asserts/RTC (en Release `NDEBUG` los quita y `/RTC1` no se usa).

## Archivos

`jugar.ps1`, `jugar.bat`, `build_release.sh`, `release_wassert.h`, `windows-msvc-release.txt`, `variantes\*.ini`; locales sin subir: `dll\` (Debug.asi, Debug.pdb, Release si compila), `salidas\`, `respaldo_pendiente\`.
