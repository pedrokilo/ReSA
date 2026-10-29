<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Quaternion, Boat, Trailer y Event sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `Quaternion.cpp`, `Boat.cpp`, `Trailer.cpp` y `Event.cpp` (8 apariciones, 8 funciones fuente). El análisis de cada archivo lo preparó un subagente (desensamblado del EXE y parche), con la compilación y los bancos en serie. Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `CQuaternion::Set(matriz)` (0x59C3E0) | float puro en el orden del EXE, bajo `float_control(precise)` |
| `CQuaternion::Set(eje, ángulo)` (0x59C600) | `QuaternionFromAxisAngleCpp` (nuevo) |
| `CBoat::ProcessControl` (0x6F1770) | `Atan2FromCpp` (nuevo) |
| `CBoat::ProcessControlInputs` (0x6F0A10) | `PowScaleFloatCpp` (nuevo, usa `ExeCompatiblePowCpp`) |
| `CTrailer::SetTowLink` (0x6CFDF0) | `TrigAtan2Cpp` (ya existía) |
| `CTrailer::ProcessAI` (0x6CF590) | `TrailerSteerCpp` (nuevo) |
| `CEvent::GetSoundLevel` (0x4B2850) | `Log10Times10Cpp` (nuevo) |
| `CEvent::CalcSoundLevelIncrement` (0x4AC050) | `SoundLevelIncrementCpp` (nuevo) |

Los ayudantes nuevos de `X87Math` salen del stash `lote-28-archivos-sin-terminar` (solo los de estos archivos), verificados contra el desensamblado. En el objeto real se comprobó que `fsin`, `fcos`, `fpatan` y `fyl2x` salen en línea, que `TrailerSteerCpp` mantiene A y B en la pila x87 y compara contra `pi + B` sin reasociar, y que `SoundLevelIncrementCpp` hace `fadd` y después `fdivrp`.

## Bancos (cero diferencias en las funciones convertidas)

| Banco | Pares | Cubre |
| --- | ---: | --- |
| DependenciesMathLeaf3 | 2048 | Quaternion Set matriz (1024) y eje-ángulo (1024) |
| Boat23 | 12288 | ProcessControl (1024) y ProcessControlInputs (1024) |
| Sys13 | 15872 | ProcessAI (1024) y SetTowLink (1024) |
| Events52 | 13888 | GetSoundLevel (1024) y CalcSoundLevelIncrement (1024) |

## Lo que reveló la validación

Events52 y Sys13 fallaban en HEAD por funciones ajenas al lote, con el patrón ya conocido: el código de la DLL llama directamente a funciones que el banco solo interceptaba por su dirección del EXE.

- **Events52:** `CEventPedCollisionWithPed::AffectsPed` (71/1024), `CEventVehicleCollision::AffectsPed` (9/1024) y `CEventVehicleOnFire::AffectsPed` (394/1024). Se añadieron en `reversing/Events52/LiveComparison.hpp` los `EntryPatch` de las entradas de la DLL de `CPedGroups::GetPedsGroup` y de `CPedGeometryAnalyser::ComputeEntityHitSide` (sobrecarga `const CVector&, CEntity&`), con el mismo Fake. Aprobó con 13888 pares.
- **Sys13:** `CTrailer::Constructor` (71/256) y `SetupSuspensionLines` (120/512), por `CMemoryMgr::Malloc`. Se sustituyó el parche de 0x72F420 por un `MallocGuard` que intercepta también las dos sobrecargas de la DLL. Aprobó con 15872 pares.
- Las corridas fallidas quedan en `CppFourFilesUnified/runs/Events52-20261014-173725-946034` y `Sys13-20261014-174116-728583`.
- `limpiar_regenerables.py` tenía un fallo al leer `tasklist` (sin salida decodificable); corregido.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos. Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| Boat.actual-after.obj | 874fa44124564b551eff8b81ba86431968df69f44b9b14a08707da55a2d8e864 |
| Boat.actual-before.obj | 3b7115f9f93956e1b7acd274efd0f4ee8b60811eb00b0f71224b425062d1b3dd |
| Event.actual-after.obj | a74e7914d809ce1d1494f8491cc7360f43951999b15595ff0871fe3d9d34a148 |
| Event.actual-before.obj | b9707c15bfe586dbcb3db84484bde9baed3d59165d4962b05e628319255edf16 |
| Quaternion.actual-after.obj | f93aed5cd9dec8bcac18af0fc90dbff24cbb3c39d2e7941830619f29f3e73903 |
| Quaternion.actual-before.obj | ee94b784f3b70c62dce08b64a70c2d07813cd35640ad2beba49e662eda8a2901 |
| Trailer.actual-after.obj | 3275b300871ed2498268a0bb5793dacfe01a49ab54fbab966d22dc029fc48d2d |
| Trailer.actual-before.obj | fddb17651a88438bb170232f75183d74a361d9c97c21d9e544e78e64b016d0dd |
| X87Math.actual-after.obj | 496bfc3d2949737b2756d327b6f42def29e3ef239a519694090eb9782133d024 |
| X87Math.actual-before.obj | dbf9270773a69cea3171b01e9caf987a3c819b8d8ca5f2ce50e48dd003a69d87 |
