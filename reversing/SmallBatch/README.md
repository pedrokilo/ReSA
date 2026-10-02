# Clases pequeñas: KeyGen, KeyArray, KeyboardState, LinkedUpgradeList, LoadedCarGroup, Acquaintance, AccidentManager, Localisation, ActiveOccluder, LodAtomicModelInfo

Validadas el 02/10/2026 con **16.400 pares original/candidato dentro de GTA**, todos exactos: 52 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CKeyArray::Load`: el original lee byte a byte, avanza `offset` por byte y falla ante una lectura corta; la DLL leía el bloque entero.
- `CKeyboardState::Clear`: el original limpia todas las teclas excepto `enter` (offset 0x23C).
- `CLoadedCarGroup::PickRandomCar`: el peso aleatorio es `GetRandomIntInRange(0, suma)`, con la fórmula del EXE.

**Límites:** se excluye `GetMember` con el grupo vacío (la DLL tiene un `assert`; el original lee la posición 0). Un `delete[]` de nulo no se registra, porque no hace nada. Los modelos de vehículo se leen de la información real (solo lectura).

Repetir: `python reversing/SmallBatch/build_probe.py` y `python reversing/run.py --case small`. Evidencia: [validation-summary.json](validation-summary.json).
