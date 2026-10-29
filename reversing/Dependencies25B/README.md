<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies25B (grupo g25b): 20 funciones de biblioteca de RenderWare

PREPARADO, sin compilar en el arbol real ni ejecutar dentro del juego: la sonda debe construirla y correrla el coordinador (en serie, clon de pruebas). No constituye validacion hasta que `run_local.py` apruebe una corrida real.
Evidencia previa sin juego (sin tocar `build\`): `offline/` mapea los bytes REALES del EXE en un proceso hijo y los compara con el texto de las 20 funciones extraido de `rwcore.cpp`/`rpanisot.cpp`/`rpworld.cpp`
(sin cambios, `extract.py` -> `cand.inc`): **20480 pares, 0 diferencias**, y `run_local.py --analyze` valida ese registro contra el oraculo independiente (`offline/analysis.json`). Con 14 errores sembrados a proposito
(`offline/mutate.py`) el mismo arnes los detecta todos (11 por diferencia de bits, 3 por acceso invalido): el banco tiene dientes.

## Funciones (todas de biblioteca, ver `g25b-classification.md`)

| Funcion | Entrada | Colaborador falso (EXE / envoltorio DLL) | Global impuesto por caso |
| --- | --- | --- | --- |
| RwCore::RwD3D9IndexBufferCreate | 0x4C9970 | 0x7306A0 / `D3DResourceSystem::CreateIndexBuffer` | - |
| RpAnisot::RpAnisotGetMaxSupportedMaxAnisotropy | 0x748F20 | 0x7FAD20 / `RwD3D9GetCaps` | - |
| RpAnisot::RpAnisotTextureSetMaxAnisotropy | 0x748F30 | - | 0xC92478 |
| RpAnisot::RpAnisotTextureGetMaxAnisotropy | 0x748F50 | - | 0xC92478 |
| RpWorld::RpClumpGetNumAtomics | 0x7498E0 | - | - |
| RpWorld::RpAtomicSetStreamAlwaysCallBack | 0x74BE30 | 0x808920 / `_rwPluginRegistryAddPlgnStrmlwysCB` | - |
| RpWorld::RpAtomicSetStreamRightsCallBack | 0x74BE50 | 0x808950 / `_rwPluginRegistryAddPlgnStrmRightsCB` | - |
| RpWorld::RpAtomicGetPluginOffset | 0x74BEC0 | 0x808470 / `_rwPluginRegistryGetPluginOffset` | - |
| RpWorld::RpGeometryTriangleGetMaterial | 0x74C760 | - | - |
| RpWorld::_rpMaterialListInitialize | 0x74E1B0 | - | - |
| RpWorld::_rpMaterialListGetMaterial | 0x74E2B0 | - | - |
| RpWorld::_rpMaterialListFindMaterialIndex | 0x74E420 | - | - |
| RpWorld::RpWorldRegisterPlugin | 0x74FCD0 | 0x8084A0 / `_rwPluginRegistryAddPlugin` | - |
| RpWorld::RpWorldRegisterPluginStream | 0x74FD00 | 0x8088E0 / `_rwPluginRegistryAddPluginStream` | - |
| RpWorld::RpAtomicGetWorld | 0x751050 | - | 0xC925A8 |
| RpWorld::RpLightRegisterPlugin | 0x751D60 | 0x8084A0 / `_rwPluginRegistryAddPlugin` | - |
| RpWorld::_rpD3D9VertexDeclarationGetSize | 0x7522E0 | - (lee `.rdata` 0x874EF8) | - |
| RpWorld::RpD3D9GeometryGetUsageFlags | 0x7588D0 | - | 0xC926E8 |
| RpWorld::RpD3D9WorldSectorSetUsageFlags | 0x7588E0 | - | 0xC926EC |
| RpWorld::RpD3D9WorldSectorGetUsageFlags | 0x758900 | - | 0xC926EC |

1024 casos por funcion = 20480 pares. Hooks: `RH_ScopedGlobalInstall`; espacios `RpWorld` (categoria `Plugins`, registrador `RpWorldInjectHooks25b` en rpworld.cpp), `RpAnisot` (`Plugins`, `RpAnisotInjectHooks` en rpanisot.cpp) y `RwCore` (categoria `Renderware`,
`RwCoreInjectHooks25b` en rwcore.cpp, solo `RwD3D9IndexBufferCreate`); los tres se declaran y se llaman en `InjectHooksMain.cpp` (el sufijo `25b` y las anclas lejanas evitan chocar con los registradores `RpWorldInjectHooks`/`RwCoreInjectHooks` de otros grupos). Hook apagado para el lado original (EXE) y encendido para el candidato (DLL) con `OriginalDependencies499/HookRoute.hpp`; las veinte rutas se construyen antes que nada.

## Que se compara en cada par (todo en bits; original == candidato == oraculo)

- Valor de retorno: EAX completo; solo AL en `RpAnisotGetMaxSupported…` y `RpAnisotTextureGetMax…` (los unicos llamadores del EXE leen AL, ver clasificacion); `RpD3D9WorldSectorSetUsageFlags` es `void` (ret = null).
- Memoria: hash FNV-1a de 64 bits del arena completo (`kArena` = 0x180 bytes con 16 de guarda 0xA5 a cada lado) tras la llamada; el arena inicial viaja entero (hex) en la fila para que el oraculo lo reproduzca.
  Los objetos son del banco: no se toca ningun objeto del motor ni del juego.
- Traza de colaboradores: tipo, y todos los argumentos tal como llegan al falso (registro fijo 0x8D624C / 0x8D62D0 / 0x8D62F8, `0x65` = `D3DFMT_INDEX16`, punteros a callbacks, etc.).
- Globales de plugin (0xC92478, 0xC925A8, 0xC926E8, 0xC926EC): se guardan, se fijan por caso (incluidos ±2^31 y -4/-16: el objeto se coloca en `arena + t - desplazamiento` con vuelta de 32 bits) y se restauran tras cada lado
  (`globals_restored`) y al final (`global_state_final`, comparado con `initial_globals` del evento `begin`).
- ABI: ESP restaurado, EBX/ESI/EDI/EBP conservados (`stack_restored`, `nonvolatile_preserved`), ruta (`route_ok`: el candidato salta a la DLL, el original no) y x87 completo (FNSAVE de 108 bytes con los ocho registros, incluidos los
  vacios, FIP incluido, y MXCSR). `kCompareDeadX87Registers = true` y NO se relaja: ninguna de las 20 ejecuta x87 (ni el EXE ni el C++ SSE de la DLL), asi que el estado x87 debe llegar intacto; antes de cada llamada `PrimeX87()`
  planta un residuo conocido en los ocho registros y FIP debe ser el del ultimo `fstp` de `PrimeX87` en el ASI en ambos lados (`fp_ip_route_ok`).
- EAX/ECX/EDX al volver: ECX/EDX se registran (`original_residue`, `candidate_residue`) pero no se exigen: ningun llamador del EXE los lee (52 llamadas desensambladas, `g25b-classification.md`). En 10 784 de los 20 480 pares difieren y es esperado.

## Colaboradores y falsos

Los colaboradores de RenderWare que la DLL llama DIRECTAMENTE (envoltorios que a su vez llaman al EXE) se sustituyen en AMBOS lados por el mismo falso trazado: `Patch` (JMP rel32 temporal, restaurado al salir de la llamada) en la direccion del EXE
y en la entrada del envoltorio de la DLL (resuelta con `Resolve`, que salta los thunks `E9`). El falso devuelve un valor por caso (`kRet`: -1, INT_MIN, INT_MAX, HRESULT D3D, 0, 1 …) o el puntero a un `D3DCAPS9` falso del arena.
Se comprueba antes de empezar que los envoltorios estan en el ASI y que las direcciones del EXE son codigo sin hook (`collaborators_stubbed`). Si otro grupo ha enganchado la direccion del EXE de un colaborador (hoy `_rwPluginRegistryGetPluginOffset`, 0x808470, en g25e), `CollabHooks` (LiveComparison.hpp) apaga ese hook RH durante la corrida y lo restaura al final con su estado inicial
(evento `begin.collaborator_hooks_disabled`); no se relaja ninguna compuerta: tras apagarlo la direccion vuelve a ser codigo del EXE sin hook. No se llama a ninguna funcion real de RenderWare ni de D3D.

## Fixtures (`Bench.hpp::Setup`; el oraculo no las repite: simula sobre la entrada registrada)

1024 casos por funcion, deterministas (`Rand(caso, clave)`), con relleno de basura variable (8 estilos) y estres:
- IndexBuf: 16 numIndices x 4 punteros (0, alineado, desalineado, basura) x 16 retornos (negativos, positivos, HRESULT).
- MaxSupported: los 256 valores del byte x 4 posiciones del `D3DCAPS9` falso, con vecinos distintos (+0x6B, +0x6D …).
- AnisoSet/Get: 16 desplazamientos x (textura nula | no nula) x 32 bytes, con 24 bits altos de basura en el argumento `RwInt8`.
- ClumpAtoms: 0..12 atomicos con orden de ranuras permutado y `prev` coherentes o corruptos.
- Registro de plugins: 32 valores por argumento (incluidos 0, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF) en combinaciones que detectan argumentos permutados, y los 16 retornos.
- TriMat: 64 `matIndex` (0..47, 0x7FFF, 0x8000, 0xFFFE, 0xFFFF …) x 16 ranuras; con 0xFFFF el puntero de materiales es basura (no se debe desreferenciar).
- MatInit: 64 posiciones x 8 rellenos. MatGet: 32 indices extremos (incluida la vuelta de 32 bits de `index*4`) x 16 ranuras.
- MatFind: 16 recuentos (0, 1…16, -1, -7, INT_MIN, INT_MIN+1, -5; con puntero de lista nulo/basura cuando `n <= 0`) x 8 modos de busqueda (presente, primero, ultimo, ausente, NULL, duplicados, puntero fuera de lista).
- VdeclSize (el oraculo predice solo los indices que caen en la tabla de 17 DWORD, incluidas las envolturas de 32 bits; el resto lee memoria real del proceso y se exige original == candidato): 1024 tipos (0..63, `.rdata` valido, negativos, envolturas por los dos bits altos, bordes); solo direcciones de `.rdata` (0x858000..0x8A3FFC), inmutables y fuera de las entradas parcheadas.
- Usage/World: 16 desplazamientos x 64 posiciones (x 32 valores de flags en SecSet).

## Compuertas de run_local.py

Igualdad fila a fila (`probe_case.analyzer`), hook registrado y cuerpo en el ASI de CADA funcion, colaboradores sustituidos, todas las compuertas de ABI/x87/ruta/globales en ambos lados, `fp_difference` vacio, fixture coherente (`fid`, `case`),
retorno / traza / hash del arena de CADA lado iguales a los de la simulacion independiente (`fixture_oracle.py`; lee del EXE la tabla de 0x874EF8), cobertura minima de cada clase de estres (calculada de las entradas registradas) y final
con restauracion (globales, entradas parcheadas, hooks, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza un registro.

## Ejecucion

```
python -B reversing/Dependencies25B/build_probe.py
python -B reversing/Dependencies25B/run_local.py
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies25B
```
Sin juego (compila contra los bytes del EXE; no toca `build\`): `reversing\Dependencies25B\offline\bld.bat` (crea `obj\g25b.exe`; extrae las funciones de `source/game_sa/RenderWare/rw`), `offline\obj\g25b.exe 1024 salida.jsonl`
y `python -B run_local.py --analyze salida.jsonl`; `python -B offline\mutate.py` repite los controles negativos (14 errores sembrados).
Orden: aplicar `g25b.patch` (y `g25b-bank.patch`, que arregla `Dependencies21G` para que siga viendo `0x74E420` como codigo del EXE), anadir las filas de `g25b-hooks.csv.txt` a `docs/hooks.csv` y compilar.
Opcional: `python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md --update-source-status` para refrescar los contadores generados de las cabeceras de `rwcore.cpp` (RwCore pasa a 5 entradas).

## Notas

- `RpClumpGetNumAtomics` conserva el corte `link == 0x40` (RpAtomic nulo) del EXE; ninguna fixture puede alcanzarlo con memoria valida (el enlace tendria que estar en la direccion 0x40).
- Los desplazamientos de plugin se sobrescriben solo durante cada llamada y se restauran en el mismo hilo (CGame::Process); ningun otro codigo del juego se ejecuta entre medias.
- Un acceso invalido de una reconstruccion erronea se registra como `error`/`first_chance_av` por `ReversingProbe::InstallFailureCapture` y la corrida se rechaza.
