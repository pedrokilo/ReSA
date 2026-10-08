<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6843 con evidencia histórica, 0 con vigencia acreditada y 1603 pendientes en 61 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Clases pequeñas: KeyGen, KeyArray, KeyboardState, LinkedUpgradeList, LoadedCarGroup, Acquaintance, AccidentManager, Localisation, ActiveOccluder, LodAtomicModelInfo

Validadas el 02/10/2026 con **16.400 pares original/candidato dentro de GTA**, todos exactos: 52 funciones con `validated=1`.

**Diferencias reales corregidas:**
- `CKeyArray::Load`: el original lee byte a byte, avanza `offset` por byte y falla ante una lectura corta; la DLL leía el bloque entero.
- `CKeyboardState::Clear`: el original limpia todas las teclas excepto `enter` (offset 0x23C).
- `CLoadedCarGroup::PickRandomCar`: el peso aleatorio es `GetRandomIntInRange(0, suma)`, con la fórmula del EXE.

**Límites:** se excluye `GetMember` con el grupo vacío (la DLL tiene un `assert`; el original lee la posición 0). Un `delete[]` de nulo no se registra, porque no hace nada. Los modelos de vehículo se leen de la información real (solo lectura).

Repetir: `python reversing/SmallBatch/build_probe.py` y `python reversing/run.py --case small`. Evidencia: [validation-summary.json](validation-summary.json).
