<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskInteriorUseInfo y InteriorManager_c::AreAnimsLoaded

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado y el trabajo pendiente figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

`CTaskInteriorUseInfo::CreateFirstSubTask` (`0x675B60`) y `CreateNextSubTask` (`0x676880`), ambas virtuales, se reconstruyeron a partir del `dumpbin` del ejecutable verificado (SHA-256 `72ae59e4…ecc9b`) y se validaron dentro del motor contra el código máquina original. **Fase A: 203 pares** de `InteriorManager_c::AreAnimsLoaded` (`0x5980F0`). **Fase B: 412 pares** de las dos funciones de `CTaskInteriorUseInfo`. Cero diferencias en la ejecución aprobada. La clase pasa a **8/8** en `docs/hooks.csv` (`reversed=1,locked=0`). `validated` no se modificó.

## Repetir

```powershell
.\PROBAR.cmd --case interioruseinfo
python reversing/run.py --case interioruseinfo
```

`run_local.py` **no reconstruye la sonda**: compara el hash de `_baseline/gta_reversed-interioruseinfo-probe.asi` con `PROBE_HASH` y ejecuta el juego. Si cambias `Probe.hpp`, `LiveComparison.hpp` o el C++ comparado, primero ejecuta `build_probe.py`, copia el ASI/PDB de `artifacts/` a `_baseline/` y actualiza `PROBE_HASH`.

`build_probe.py` instrumenta temporalmente `TaskInteriorUseInfo.cpp` (contadores en las dos funciones), `InteriorManager_c.cpp` (contador en `AreAnimsLoaded`) y `Game.cpp` (llamada a `UseInfoLive::Update()`), compila y restaura las tres fuentes por bytes en `finally`. La sonda **no llama a ningún `InjectHooks()`**: busca los hooks ya registrados por `InjectHooksMain.cpp`.

## Correcciones encontradas en el C++

1. **`AreAnimsLoaded` leía la tabla equivocada** (corregido por la sesión principal antes de este banco). El original calcula el índice con `GetAnimationBlockIndex("int_house"|"int_shop"|"int_office")` para los argumentos 0/1/2, o usa el argumento tal cual, y lee directamente `ms_aAnimBlocks[idx].IsLoaded == 1` (`shl eax,5; mov cl,[eax+0B5D4B0h]`). La reconstrucción anterior hacía `GetAnimationBlock((AssocGroupId)idx)->IsLoaded`, que pasa por la tabla de grupos y puede devolver un puntero nulo.
2. **`AreAnimsLoaded` y `AddSameGroupEffectInfos` tenían la convención de llamada incorrecta** (encontrado y corregido en esta tarea). En el original ambas son `__thiscall`: el llamador hace `mov ecx,0BAF670h` (`g_interiorMan`) y la función termina con `ret 4` y `ret 8`, respectivamente. En C++ estaban declaradas `static`, con convención cdecl y `ret` sin limpiar argumentos. Cuando un código **original** las llamaba a través del hook, la pila quedaba desplazada 4 bytes y el epílogo del llamador saltaba a una dirección inválida. Esto afectaba al `ProcessPed` original de `CTaskInteriorSitAtDesk` y al `CreateNextSubTask` original. Ahora son métodos no estáticos (`bool AreAnimsLoaded(int32) const`), registrados con `RH_ScopedInstall`, y los llamadores usan `g_interiorMan.AreAnimsLoaded(...)` (`TaskInteriorLieInBed.cpp`, `TaskInteriorSitAtDesk.cpp`, `TaskInteriorUseInfo.cpp`). La fase A mide `esp` alrededor de una llamada `__thiscall` cruda: delta 0 en los 203 pares de ambos lados. `AddSameGroupEffectInfos` solo tiene como llamador la reconstrucción de `GetVisibleEffects`; su corrección de ABI **no tiene banco propio**.
3. **Relleno de `CTaskComplexSequence`** en el caso 9 de `CreateNextSubTask`. `new CTaskComplexSequence{}` usa inicialización por valor sobre un constructor `= default` y pone a cero también los 3 bytes de relleno posteriores a `m_bFlushTasks` (`+0x39..0x3B`). El original llama a `0x632BD0`, que no los toca. La primera ejecución del banco falló exactamente en ese dword (par 335: `0xCDCDCD00` frente a `0x00000000`). Se cambió a `new CTaskComplexSequence;`. No es un cambio observable, pero así la comparación byte a byte es exacta. Ejecución fallida conservada: `resultados-interioruseinfo/20261001-183945-825045`.

Se revisó de nuevo el ASM completo de ambas funciones frente al C++ de la sesión principal: orden de `IsInUse = true` antes de `new`, argumentos de los constructores (`LieInBed` 0x30, `SitInChair` 0x2C mediante el constructor original `0x675C30`, `StandStill` 0x20, `RunTimedAnim` 0x34 con `blendDelta = m_bDoInstantly ? 1000 : 4`, `SitAtDesk` 0x38, `RunAnim` 0x20, `Sequence` 0x40), contextos `Say` 0xBC/0xBD con argumentos `(0, 1.0, false, false, false)`, `m_bStartServeAnim` (`+0xC`) asignado antes de `Say`, y la limpieza de `m_pEntityIgnoredCollision` (`ped+0x128`) en el camino común de fallo. No se encontraron otras diferencias.

## Fase A — AreAnimsLoaded (203 pares)

- Llamada `__thiscall` cruda a `0x5980F0` con `ecx=&g_interiorMan`; lado original con el hook deshabilitado y candidato con el hook habilitado. Contador de la reconstrucción: 0 en el lado original y 1 en el candidato.
- Estado real de la partida: 155 bloques; `int_house`=77, `int_shop`=79 e `int_office`=78. Antes del banco estaban cargados `int_house` e `int_office`; `int_shop`, no.
- Se usan todos los argumentos 0..154 sobre el estado real: 0/1/2 por nombre y 3..154 como índice directo (155 pares).
- Se probaron las 8 combinaciones de `IsLoaded` de los tres bloques, con los argumentos 0/1/2 y los índices directos 77/79/78 (48 pares). El valor previo se restaura tras cada combinación y se comprueba al final.
- Resultado: 29 `true` y 174 `false`; los mismos en ambos lados.

## Fase B — CreateFirstSubTask/CreateNextSubTask (412 pares)

Por cada par se crean fixtures sintéticos **independientes por lado**: un `CTaskInteriorUseInfo` colocado con su constructor, un `InteriorInfo_t`, un `Interior_c` y un `InteriorGroup_c` con un peatón tendero (memoria cero, con `CPedIntelligence` sintético y tarea primaria `TASK_PRIMARY_DEFAULT` propia), una subtarea (`CTaskInteriorGoToInfo` o `CTaskSimpleStandStill`) y una copia del jugador real (`CPlayerPed` copiado en memoria propia; ninguna escritura alcanza al jugador real). En el lado original se deshabilitan el hook de la función bajo prueba y el de `AreAnimsLoaded`; en el candidato, ambos quedan habilitados.

Cobertura:

- `CreateFirstSubTask` (26 pares): `m_IntInfo` nulo × `m_bDoInstantly` (2), y `Type` 0..11 × `m_bDoInstantly` (24).
- `CreateNextSubTask` (386 pares): `m_IntInfo` nulo × subtarea (2), y `Type` 0..11 (incluidos 0 y 11, fuera de rango) × subtarea `GOTO_INFO`/otra × `m_bDoInstantly` × `EntityIgnoredCollision` presente/ausente × `AreAnimsLoaded` verdadero/falso (controlado mediante `IsLoaded` y restaurado) × tarea primaria del tendero `SHOPKEEPER`/otra (384).
- Tareas devueltas: 264 nulas, 24 `GOTO_INFO` (1401), 32 `LieInBed` (1406), 16 `SitInChair` (1407), 16 `SitAtDesk` (1408), 40 `StandStill` (203), 8 `RunTimedAnim`, 8 `RunAnim` (400) y 4 `Sequence` (244). Hubo 12 llamadas a `Say` y se activó 4 veces `m_bStartServeAnim` del tendero.

Se compara: puntero devuelto (normalizado), **cada objeto asignado** (tamaño solicitado a `operator new`, `GetTaskType()` y todos los dwords desde `+4` hasta el final, incluido el relleno, con los punteros a fixtures/arena sustituidos por marcas), el estado del propio `CTaskInteriorUseInfo`, `Type`/`IsInUse`/`EntityIgnoredCollision` del `InteriorInfo_t`, **todos los dwords modificados del peatón**, la tarea del tendero completa (incluido `m_bStartServeAnim`), la secuencia de `Say` (contexto, argumentos y `this`) y, tras `delete` de la tarea devuelta en cada lado, los bloques liberados, `IsInUse` y nuevos cambios en el peatón. `SitInChair` (no reconstruida) se compara igual: ambos lados la construyen con el constructor original y producen bytes idénticos.

### Dependencias controladas

- `CTask::operator new/delete` (`0x61A5A0`/`0x61A5B0` y sus entradas del DLL): cada lado usa una arena propia de 2 KiB, rellenada con `0xCD` antes de cada llamada. Así no se toca el pool real de tareas y se comparan también los bytes no inicializados. `delete` solo registra.
- `CPed::Say` (`0x5EFFE0` y su entrada del DLL): se registra la llamada; no se reproduce audio.
- `IsLoaded` de `int_house`/`int_shop`/`int_office`: se fija por caso y se restaura al valor previo inmediatamente después de cada lado.
- Constructores ya reconstruidos (`LieInBed`, `SitAtDesk`, `StandStill`, `RunAnim`, `RunTimedAnim`, `Sequence`/`AddTask`) y `InteriorGroup_c::GetPed` (original): son dependencias reales llamadas por ambos lados.

## Límites

- `Type` con un tendero sin tarea primaria por defecto (`nullptr`) no se probó, porque ambos lados lo desreferencian sin comprobarlo. Tampoco se probaron un `m_Int`/grupo nulo en el tipo 9 ni un tendero sin peatón en el grupo.
- `GetAnimationBlockIndex` devolviendo -1 (bloque inexistente) no se cubrió; los tres nombres existen en esta partida.
- No hubo NPC real en un interior real ni revisión visual: son fixtures sintéticos dentro del motor. No se reproduce audio. Las animaciones devueltas no se ejecutan.
- Prueba con `--unhook=Audio,Global/SurfaceInfos_c`, como el resto de los bancos.
- La corrección de ABI de `AddSameGroupEffectInfos` no tiene banco propio.

## Evidencia

Ver `validation-summary.json`. Log aprobado: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interioruseinfo/20261001-184046-436203/comparisons.jsonl` (SHA-256 `f0ba266caf453ed924a8fa1534bbf470f2c0b0d6c975284fc34320d0d6042e69`). Sonda: ASI `1486eeaf27dbaa12e66923b4f88f8c57f16826b8ef4702ec15520cd2f142f368` y PDB `0da752a69aecb6ee34900f644cc6b8138c71fb80bfa1d557c793af1c117a0f57`. Partidas sin cambios (`GTASAsf2.b` `b089c48d…`) e INI restaurados. Sin commit ni push.
