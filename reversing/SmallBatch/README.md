<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Clases pequeñas: KeyGen, KeyArray, KeyboardState, LinkedUpgradeList, LoadedCarGroup, Acquaintance, AccidentManager, Localisation, ActiveOccluder, LodAtomicModelInfo

Validadas el 02/10/2026 con **16.400 pares original/candidato dentro de GTA**, todos exactos: 52 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CKeyArray::Load`: el original lee byte a byte, avanza `offset` por byte y falla ante una lectura corta; la DLL leía el bloque entero.
- `CKeyboardState::Clear`: el original limpia todas las teclas excepto `enter` (offset 0x23C).
- `CLoadedCarGroup::PickRandomCar`: el peso aleatorio es `GetRandomIntInRange(0, suma)`, con la fórmula del EXE.

**Límites:** se excluye `GetMember` con el grupo vacío (la DLL tiene un `assert`; el original lee la posición 0). Un `delete[]` de nulo no se registra, porque no hace nada. Los modelos de vehículo se leen de la información real (solo lectura).

Repetir: `python reversing/SmallBatch/build_probe.py` y `python reversing/run.py --case small`. Evidencia: [validation-summary.json](validation-summary.json).
