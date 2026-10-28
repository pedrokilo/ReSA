<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Reanudar la regresión final de las oleadas 1 y 2 (parada el 16/10/2026 ~13:00)

Estado al parar: 54 bancos aprobados de 148 (ver `final_raw.txt` y `runs/`). El proceso se detuvo entre dos bancos
(se mató `rerun_hist.py` y MSBuild justo al terminar `CppShadowSqrtVehicle`); no quedó el juego abierto ni ficheros de prueba sin restaurar.

## 1. Antes de reanudar (arbol de trabajo, sin tocar mientras corre una tanda)

Aplicar, en este orden (parches en el scratchpad `...\scratchpad\w20\`), compilar con `reversing/CVehicle/build-project.ps1`
(`CMAKE_BUILD_PARALLEL_LEVEL=2`, `_CL_=-MP2`) y pasar `python -B contrib/audit_register_residue.py`:

- `asm3.patch` + `asm3-bank.patch` (World.cpp, TaskSimpleCarSetPedSlowDraggedOut.cpp, MenuManager.cpp).
- `platformasm.patch` + `platformasm-bank.patch` (platform.cpp, app_game.cpp). Anadir `'platform'` a `OBJECTS` en `contrib/audit_register_residue.py`.
- `formation-extra.patch` (opcional, se decidio aplicarlo).
- `WindowedMode.cpp` NO se convierte (opcion A, es NOTSA): excepcion documentada, anotarla en PROCESO.md / README de CppOnlyMigration.
- MemoryMgr: `DependenciesMemory9` queda retirado (exige instrucciones identicas al EXE); se registra `DependenciesMemoryCpp16`.

## 2. Reanudar la regresion

Lista pendiente (fallidos + restantes + `DependenciesStatsFile15` + `RequestedPadProcessPad`): `pendientes_regresion.txt`.

```bash
cd /d/Original/gta-reversed-master
export CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2
nohup python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py CppTwelveLevel6Final $(cat reversing/CppTwelveLevel6Final/pendientes_regresion.txt) > reversing/CppTwelveLevel6Final/final_raw2.txt 2>/dev/null &
```

Despues de aplicar `asm3` y `platformasm` hay que repetir tambien los bancos que vigilan esas fuentes:
`WldD`, `CTasks6b`, `MenuMgrA`..`MenuMgrD`, `Rs27`, `GameIdle`, `HookUnlockGame`, `CppFormationNumeric`, `DependenciesFormationMemory8`,
y los ~52 que listan `World.cpp` en su `build_probe.py` (`grep -l "World.cpp" reversing/*/build_probe.py`).

## 3. Fallos conocidos de esta pasada (ya diagnosticados)

- `DependenciesWorld4`, `CppIntegerWorld4` (CGarages::FindGarageForObject 4096/4096, `helper_route_ok`): causa = mi arreglo global de
  `OriginalDependencies499/HookRoute.hpp` pisaba el cuerpo C++ explicito del banco de Garage. YA REVERTIDO; el arreglo para
  `DependenciesStatsFile15` es ahora local (usa `StatsLoadStatUpdateConditionsExe` como candidato). Hay que repetirlos.
- `ShadowSectors355`: "GTA no responde" (colgado, 30 s). Probablemente transitorio; repetir.
- `RequestedPadDistanceTrainNopHexV2`: la sonda fallo en `check_train_abi.py` (`import comtypes ...`), pero `comtypes` 1.4.17 YA esta instalado en el Python
  actual y se importa bien: el diagnostico inicial era erroneo. Pendiente: repetirlo y leer el error completo de `reversing/RequestedPadDistanceTrainNopHexV2/artifacts/probe-build.log` o de la salida de `rerun_hist.py`.
- `RequestedPadProcessPad`: prepared-freeze regenerado con `make_freeze.py`; nunca se habia ejecutado.

## 4. Despues de la regresion

Documentar (approvals.json, README del estudio `reversing/CppTwelveLevelXStudy`, parrafo en `reversing/CppOnlyMigration/README.md`,
conteos en README.md / docs/CurrentStatus.md / docs/ProjectRoadmap.md / reversing/MEJORAS_DEL_PROCESO.md, scripts de sync,
`audit_hook_catalog.py --strict`, `audit_register_residue.py`, tests de `contrib/tests`), y esperar "sube a git".
- 16/10 ~13:20: el usuario pidió NO repetir nada; se mató una repetición a medias (juego cerrado, saves/ini verificados intactos). Todo sigue pendiente.
- 16/10 reanudado: parches asm3/platformasm/formation-extra aplicados, compila, auditor limpio; cola de 135 bancos lanzada (cola_final.txt, salida final_raw2.txt).
- 16/10 ~20:45: regresion anterior se detuvo en 116/135 tras PlaneBlow355; aplicados sexappeal+sexfpu+sprint+file15+misc3+HookUnlockAudit(retired.py); nueva cola cola_final2.txt (48) -> final_raw3.txt
- 16/10 20:40: PlaneBlow355 dejo un gta_sa.exe huerfano (bucle real, 1109 s CPU) que bloqueo la cola; matado; dmp en scratchpad/hang/planeblow.dmp; cola_final3.txt -> final_raw4.txt (PlaneBlow355 al final). VIGILAR procesos gta_sa huerfanos tras 'GTA no responde'.
- 16/10 ~21:00 CORRECCION: el gta_sa.exe que 'giraba' NO era PlaneBlow355: era PowEquivalence (banco de ~38 min de CPU, iba al 59%) de la cola anterior, que seguia viva; la mate por error al creer que estaba huerfano. PlaneBlow355 no tiene bucle (BlowUpCar exculpada) pero su resultado quedo vacio: revalidar. PowEquivalence necesita ~40 min; no matar procesos gta_sa sin comprobar con el timestamp del .asi en scripts\ cual es el banco.

## Estado final (17/10/2026)

La regresión de los 95 bancos pendientes terminó el 17/10/2026: 95 de 95 aprobados (93 en la pasada `final_raw14.txt` y los dos restantes, `RequestedPadProcessPad` por hash de README obsoleto tras la sincronización de documentos y `CamLookSide355` por «GTA no responde» intermitente, en `final_raw15.txt`). Ya no hay nada que reanudar.

Cierre del hueco de `Ftol2RegsCore` (17/10/2026): los siete bancos de las funciones que usan los helpers que llegan a `Ftol2RegsCore` y que no se habían repetido con el árbol actual (`CPedC`, `DependenciesVehicleStats9`, `GfxWave2`, `Misc61b`, `Stats32`, `Zones21`, `CameraFinal4`) se ejecutaron en `final_raw16.txt`: 7 de 7 aprobados.
