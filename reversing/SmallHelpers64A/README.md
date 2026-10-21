<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/10/2026 (Ciudad de México):** 813 grupos / 8862 funciones reconstruidas; 8862 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# SmallHelpers64A: GetMillisecondTime (0x5617C0), DIReleaseMouse (0x746F70), RemoveUpgradeCB (0x6D3300), FindUpgradeCB (0x6D3370)

Estado: **APROBADO dentro del juego** (21/10/2026, worktree `D:\Original\wp-1`, HEAD 1b13a6db4 + fuente del lote). 16384 pares (4 x 4096), cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo. Evidencia: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-smallhelpers64a\20261021-021554-251400`.

`source.patch`: solo Timer.cpp, Pad.cpp, Pad.h y Vehicle.cpp (`git apply --check` verificado contra HEAD).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| GetMillisecondTime | 0x005617C0 | CTimer,GetMillisecondTime | S |
| DIReleaseMouse | 0x00746F70 | CPad,DIReleaseMouse | S |
| RemoveUpgradeCB | 0x006D3300 | CVehicle,RemoveUpgradeCB | S |
| FindUpgradeCB | 0x006D3370 | CVehicle,FindUpgradeCB | S |

Revision contra el EXE: todas cdecl; RemoveUpgradeCB/FindUpgradeCB son callbacks de RpClumpForAllAtomics (0x749B70, solo lee EAX). DIReleaseMouse cambia a `void` (nadie lee EAX). GetMillisecondTime devuelve EDX:EAX (llamadores CTimer::Suspend/Resume leen EDX).
Colaboradores falsos: RsTimer, GetAtomicId, GetModelInfo, RpClumpRemoveAtomic, RpAtomicDestroy, RwFrameDestroy, CBaseModelInfo::RemoveRef. DIReleaseMouse: dispositivo COM falso y psGlobal falsos en una arena; el real no se toca.
Limites: no se prueban GetModelInfo nulo en la primera llamada, dispositivo nulo tras Unacquire ni marco nulo (assert del envoltorio de la DLL).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh SmallHelpers64A`; `python -I fixture_oracle.py`; `python -I fixture_mirror.py`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).
