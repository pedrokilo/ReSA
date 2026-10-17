<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# BulletTraces sin ensamblador (14/10/2026)

[Inicio](../../README.md) · [Proceso](../CppOnlyMigration/PROCESO.md) · [Estado](../CppOnlyMigration/README.md)

Se eliminó todo el `__asm` de `BulletTraces.cpp` (2 apariciones, 2 funciones fuente). Ningún banco se relajó.

## Conversiones

| Función | Cómo |
| --- | --- |
| `PlayFrontEndSoundForTrace` (dentro de `CBulletTraces::AddTrace`, 0x723750) | `volume = AudioLog10Times20Cpp(factor)` (ya existía en `X87Math`) |
| `CBulletTraces::Render` (0x723C10) | `OneMinusUnsignedRatioCpp(restante, vida)`: helper nuevo en `X87Math` (recuperado del stash `lote-28-archivos-sin-terminar`) |

## Bancos (cero diferencias en las funciones de CBulletTraces)

| Banco | Pares | Papel |
| --- | ---: | --- |
| Fx43 | 15552 | Valida AddTrace (1024), Render (512), AddTrace-Wrapper (512), Update (256) e Init (64) |
| Data7 | 13760 | Dependencia (falsea Update y Render) |
| Replay47 | 18208 | Dependencia (falsea Init) |
| WeaponFinal2 | 7808 | Dependencia (falsea AddTrace y su wrapper) |

## Lo que reveló la validación

- **Fx43 falló en la primera corrida (y fallaba igual en HEAD) por funciones ajenas** a BulletTraces: `CFire::ProcessFire` (609/1024), `CMovingThings::Init` (64/64), `FxMemoryPool_c::Init` (64/64) y `Optimise` (128/128). Causa: el banco solo interceptaba las direcciones del EXE de `CPointLights::AddLight`, `CPointLights::Init`, `CMemoryMgr::Malloc` y `CMemoryMgr::Realloc`, y el código de la DLL las llama directamente. Se añadieron los `EntryPatch` de las entradas de la DLL en `reversing/Fx43/LiveComparison.hpp` (sin quitar ninguna compuerta) y el banco aprobó con 15552 pares. La corrida fallida queda en `CppBulletTracesUnified/runs/Fx43-20261014-003615-046612`.
- **Disco D: lleno.** Cada sonda deja un `.pdb` de 1,7 GB; los de esta tanda se movieron (sin borrarse) fuera de `reversing/*/artifacts`.

## Límites

Sin cobertura exhaustiva de x87 más allá de los fixtures de los bancos. Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| BulletTraces.actual-after.obj | 8bad45ae52882c4e5df39cb82c69229e981fc097d99e071a8f7ba579179ba403 |
| BulletTraces.actual-before.obj | 13ac3b9dd87b475f24514844ef3437c234d46723e5c593821384c3115d52e389 |
| X87Math.actual-after.obj | dbf9270773a69cea3171b01e9caf987a3c819b8d8ca5f2ce50e48dd003a69d87 |
| X87Math.actual-before.obj | b08cb9c5d29c90478254d01490fb110de457f0abdec5e6f5046f309b3c7ffa59 |
