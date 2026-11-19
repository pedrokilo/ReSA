<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RW0A: prueba de juego con RenderWare/D3D reconstruido enganchado

Fecha 18/11/2026. Worktree wv-6 (HEAD 39276bea85, sin cambios de fuente de producción). Copia de pruebas: `E:\JUEGOS\GTA San Andreas - CVehicle Test`, restaurada al terminar (DLL, PDB, INI y EXE verificados por hash; partidas sin cambios).

## 1. Instalación por defecto

Mecanismo: `RH_ScopedInstall(...)` crea el hook con `HookInstallOptions{reversed=true, enabled=reversed, locked=!reversed}`. Después de la inyección, `dllmain.cpp` aplica `--unhook-all`, `--unhook=ruta,...`, `--unhook-except=ruta,...` (rutas `Categoría/Clase[/Función]`, p. ej. `Renderware/RwCore`, `Plugins/RpWorld`, `App/ImportThunks`) y `--dump-hooks-to=archivo`. El menú ImGui se retiró el 04/10/2026. Ninguna entrada de RenderWare está excluida ni bloqueada.

**Evidencia:** el volcado de hooks del juego en marcha (822 entradas RW/D3D, todas reversed=1 y locked=0) y la sonda, que lee el primer byte de cada dirección en memoria: **822 de 822 direcciones distintas con JMP E9 a gta_reversed.asi** (778 vía thunk de enlace incremental, 44 directas). Tabla por entrada: `instalacion.csv`. Las 1.889 funciones de la región con su motivo (enganchada / enlazada: libpng, zlib, D3DX9 / no reconstruible): `instalacion_region.csv`.

## 2. Compilación

`reversing/CVehicle/build-project.ps1` bajo `compile_lock` (`build_full.sh`). El binario ya estaba al día (EXIT=0, sin recompilar). **Debug**: /Od, /RTC1 (EnableFastChecks), CRT debug estático, sin NDEBUG (asserts y asserts del CRT activos, con registro y sin cuadro), EXTRA_DEBUG_FEATURES. `bin/Debug/gta_reversed.asi`, 50.433.024 bytes.

## 3. Arnés (`reversing/FnRW0A/`)

- `run_phase.py F1|F2|F3 [--minutes N] [--unhook ...] [--unhook-all] [--unhook-except ...] [--slot N] [--fptrap] [--hour H]`: toma `gta_lock`, respalda y restaura con hashes, instala la DLL y la sonda, lanza, vigila diálogos, «No responde» (WM_NULL), memoria (<3,8 GB) y crecimiento del log de la DLL (aborta a 1,5 GB), captura la pantalla cada 60 s (`shots/*_juego.png` y `*_consola.png`), analiza en flujo `scripts/logs/log.log` (`game_log_summary.txt`) y pide a la sonda contadores y pila.
- `probe/rwprobe.c` genera `zz_rwprobe.asi` (SOLO arnés, `probe/build_probe.cmd`): antepone a cada gancho un trampolín con `lock inc` (llamadas desde el EXE y, vía thunk, totales), escribe cada 5 s el estado (fotograma, estado del juego, posición, hora), vuelca la pila con símbolos ante excepciones o a petición, y con `--fptrap` desenmascara la excepción SSE de operación inválida en el hilo principal. No cambia el comportamiento de producción.
- `probe/symres.c` resuelve RVA de la DLL con el PDB (x64: el PDB pesa 455 MB). `analyze.py` genera la cobertura; `hook_paths.py`, `bisect_run.py` y `bisect_auto.py` la bisección; `gxt.py` busca cadenas en un GXT.

## 4. Resultados por fase

| Fase | Resultado |
|---|---|
| F1 | Menú a los ~16 s. El juego sale solo del menú ~20 s después y arranca una partida nueva (igual con los 18 grupos RW apagados: no es RW). F1b: 3 min sin fallos. |
| F2 (ranura 2, ventana sin tocar) | FP1 (4 min), FP4 (8 min) y el control App+Win (4 min) limpias: sin NaN, caídas ni crash. |
| F3 (30 min, ventana usada por el usuario) | 1.653 s. El usuario robó coches 25 min sin fallo; al recargar la partida desde el menú salió el fallo (fotograma 31.589.921, posición NaN). Sin cierre del proceso. |
| Partida nueva | Crash 0xC0000005 a los 4-5 min (3 veces, también con RW apagado): ver defectos. |

Capturas: `runs/*/shots/` (locales, no se suben a git).

## 5. Cobertura (unión de F1b, F2, F3, FP1 y FP4; máximo por función)

| Módulo | Entradas | Instaladas | Ejercitadas | No ejercitadas | Llamadas |
|---|---:|---:|---:|---:|---:|
| RwCore | 211 | 211 | 131 | 80 | 67.358.097 |
| RpWorld | 197 | 197 | 119 | 78 | 26.291.326 |
| RwPlCore | 132 | 132 | 95 | 37 | 119.918.321 |
| RwD3D9 | 115 | 115 | 80 | 35 | 406.529.492 |
| RpSkin | 34 | 34 | 23 | 11 | 1.185.440 |
| RpMatFX | 28 | 28 | 11 | 17 | 8.352.328 |
| ImportThunks | 17 | 17 | 0 | 17 | 0 |
| RpUVAnim | 16 | 16 | 1 | 15 | 1 |
| ZlibInflate | 14 | 14 | 0 | 14 | 0 |
| RtDict | 12 | 12 | 1 | 11 | 1 |
| D3DX9 | 11 | 11 | 1 | 10 | 18 |
| RtAnim | 10 | 10 | 7 | 3 | 2.129 |
| RpHAnim | 7 | 7 | 6 | 1 | 2.212.175 |
| RtQuat | 6 | 6 | 6 | 0 | 24.372 |
| QTMLRuntime | 6 | 6 | 3 | 3 | 3 |
| DelayLoadThunks | 3 | 3 | 0 | 3 | 0 |
| RtBMP | 2 | 2 | 1 | 1 | 1 |
| RtSlerp | 1 | 1 | 1 | 0 | 7.385 |
| **Total** | **822** | **822** | **486** | **336** | 631.881.089 |

**486 de 822 (59,1 %) ejercitadas.** Las 336 restantes están en `no_ejercitadas.csv`. Las llamadas internas C++ a C++ se cuentan en el thunk (columna `llamadas_totales` de `cobertura.csv`). Sin ninguna llamada: ImportThunks, ZlibInflate y DelayLoadThunks; casi sin uso: RpUVAnim 1/16, RtDict 1/12, D3DX9 1/11.

## 6. Defectos (ninguno de RenderWare; ninguno corregido)

1. `CPedIntelligence::ClearTasks` (0x601420, PedIntelligence.cpp:448): `GetTaskPrimary(TASK_PRIMARY_DEFAULT)` nulo en un ped dentro de vehículo, llamado desde la lambda de `CCarAI::AddPoliceCarOccupants` (CarAI.cpp:246) vía `CSetPiece::Update`. El EXE desreferencia igual, así que falta saber por qué el ocupante nace sin tarea por defecto. Se repite con RW apagado.
2. `CText::Load` (Text.cpp:210): `assert(file)` y luego `fread(nullptr)` (0xC0000409) cuando el GXT no se abre; una vez en ~20 cargas. `CFileMgr::SetDir/ChangeDir/OpenFile` replican el EXE por lectura; sin causa aún.
3. `CLoadMonitor::EndFrame` (LoadMonitor.cpp:147): 0/0 en SSE llena de NaN `m_fSmoothedValues` en el primer fotograma (inocuo hasta ahora).
4. `CCustomCarPlateMgr::AtomicUpgradeSetCarplateTextureCB` (0x6FDFC0): `NOTSA_LOG_DEBUG` en ruta caliente (spam). Propuesta: comentarlo, como en `MaterialUpgradeSetCarplateTextureCB`.

## 7. Fallo que ve el usuario (mundo blanco, texto roto, «Cargando...», caídas, posición NaN)

- No es RenderWare: se reproduce con los 18 grupos RW apagados (`runs/F2-sinRW-*`, `hooks_called=0`).
- No es determinista: la bisección por grupos queda invalidada (BB13 repite la configuración de BB4 y falla; BB12, la misma, murió en la carga). Las ventanas sin tocar no fallan.
- La ranura 2 es una partida modificada (`LoadSavesWithMismatchingVersion`): el log avisa en `GenericGameStorage.cpp:309-319`; con la lógica original no hay aviso.
- Con la trampa SSE armada ninguna operación inválida precede al NaN: no nace de aritmética SSE de la DLL (queda x87 del EXE, memoria sin inicializar, puntero o desplazamiento erróneo).
- En F3 el NaN aparece ~20 s tras recargar la partida desde el menú (el contador de fotogramas vuelve a ~31,59 millones); el jugador salía de la casa (z 1014 a 13).
- Cadenas GXT (spanish.gxt, tabla MAIN, todas fuera de las tablas de misión): «Aparca, activa la bomba...» 0x5BB97392 y 0x86886BA7; «¿Quieres un poco de mierda?» 0x442EB9A0; «Para responder positivamente...» 0x7D41A141; «Cargando...» 0x1484544E.
- `Pool.h:207` (`CEntryExit`) sale en todas las ejecuciones, también en logs de octubre: no discrimina.

## 8. Qué validar primero

Lo más ejercitado y aún sin validar: RwD3D9 (80 funciones, 406 M llamadas), RwCore (131) y RwPlCore (95; `_rwIntelSSE2V3dTransformPoint(s)` suman millones), después RpWorld (119), RpSkin y RpMatFX. Lo no ejercitado, al final.

## 9. Pendiente

Aislar la causa del fallo del usuario (siguiente paso: anillo de ganchos del juego completo y volcado al primer NaN; pruebas tras recargar la partida) y automatizar el robo de coche. En la copia de pruebas `scripts/logs/log.log` creció ~130 MB por estas pruebas (no restaurable).
