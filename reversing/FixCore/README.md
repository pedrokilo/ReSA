<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Arreglos de núcleo fuera de listas

Validadas el 03/10/2026 con **11.776 pares original/candidato dentro de GTA**, todos exactos: 17 funciones de 6 clases.

| Clase | Funciones |
|---|---|
| `CMatrix` | `RotateX`, `RotateY`, `RotateZ`, `SetRotateXOnly`, `SetRotateYOnly`, `SetRotateZOnly`, `SetRotateX`, `SetRotateY`, `SetRotateZ` |
| `CColBox` | `operator=` |
| `CVehicle` | `SetComponentRotation` |
| `CTaskSimpleSwim` | `ApplyRollAndPitch` |
| `CTxdStore` | `AddTxdSlot`, `RemoveTxdSlot` (`CPool::New`/`Delete`) |
| `CPed` | `operator new`, `operator new(ref)`, `operator delete` (`CPool::NewAt`/`Delete`) |

**Diferencias reales corregidas:**
- `CMatrix::RotateX/RotateY/RotateZ` (0x59B1E0/0x59B2C0/0x59B390): transcripción exacta del x87. `fcos` queda sin redondear en la FPU, `fsin` se guarda como float (en `RotateY`, `right.x` usa el seno sin redondear) y la posición también se rota. Se quita el argumento NOTSA `bKeepPos`.
- `CMatrix::SetRotateXOnly/SetRotateYOnly` (0x59AFA0/0x59AFE0): `fsin`/`fcos` del x87 en vez del `sin`/`cos` de la CRT. `SetRotateZOnly` ya era exacta. `TWO_PI` de `common.h` no interviene y no se toca.
- `CTaskSimpleSwim::ApplyRollAndPitch` llama a `RotateY`/`RotateX` de la DLL en vez de a las direcciones del EXE.
- `CVehicle::SetComponentRotation` (0x6DBA30):
  - con `bSetRotate` usa `SetRotateX/Y/Z` completos (la posición pasa a 0);
  - sin él, anula la posición sumándole su opuesto y luego rota;
  - al final suma la posición guardada (cambia `-0` a `+0`);
  - un eje fuera de 0..2 no rota.
- `CPool` (`Core/Pool.h`):
  - `New` busca desde `m_LastFreeSlot + 1` con una sola vuelta completa; si el pool está lleno deja el cursor en 0 y devuelve `nullptr`;
  - `New`/`NewAt` no rellenan con 0xCD y `Delete` no rellena con 0xDD (0x731B80, 0x61A500, 0x5E4730, 0x5E4760).
- `CColBox::operator=` (0x40EEC0) copia también `m_nPiece`, pero no `m_nLight`.
- `common.h`: los contadores del búfer temporal (0xC4B950/0xC4B954) pasan a `int32` (dwords con signo, como en 0x710CC0).

**Método:**
- `CMatrix` y `CColBox` no tienen hooks (`CMatrix::InjectHooks` está desactivado en `InjectHooksMain`). Se compara la función del EXE con el símbolo de la DLL sobre el mismo búfer, con relleno aleatorio.
- `SetComponentRotation` y `ApplyRollAndPitch` usan frame y matriz RW falsos. `CPedIK::RotateTorso` y `CEntity::UpdateRwFrame` están simulados y trazados.
- Los pools son falsos, con estados, cursor y almacenamiento aleatorios. El puntero global del pool (0xC8800C, 0xB74490) solo se cambia durante la llamada. El hook `CPedPool::New` se conmuta con los del banco y `RwTexDictionaryDestroy` está simulado.
- Fuera de los fixtures:
  - pool de TXD lleno en `AddTxdSlot` (los dos lados escriben en `nullptr`);
  - slot libre en `RemoveTxdSlot` (el EXE no comprueba nulo);
  - `NewAt` sin otro slot libre (el bucle de `CreateAtRef` se sale del array);
  - ángulos NaN o infinitos.

Repetir: `python reversing/FixCore/build_probe.py` y `python reversing/run.py --case fixcore`. Evidencia: [validation-summary.json](validation-summary.json).
