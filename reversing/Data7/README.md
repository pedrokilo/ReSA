<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Policía, ropa, listas, plantas, marcas, efectos y streaming (lista de 7)

Validadas el 02/10/2026 con **13.760 pares original/candidato dentro de GTA**, todos exactos: 45 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CCopPed` | Constructor, Destructor, `SetPartner`, `AddCriminalToKill`, `RemoveCriminalToKill`, `ClearCriminalsToKill`, `ProcessControl` (VMT) |
| `CPedClothesDesc` | Constructor, `Initialise`, `GetIsWearingBalaclava`, `HasVisibleNewHairCut`, `HasVisibleTattoo`, `SetModel` (hash y nombre) |
| `CPedList` | `Empty`, `BuildListFromGroup_NoLeader`, `ExtractPedsWithGuns` |
| `CPPTriPlantBuffer` | `Flush`, `GetPPTriPlantPtr`, `ChangeCurrentPlantModelsSet`, `IncreaseBufferIndex`, `GetPlantModelsTab`, `SetPlantModelsTab`, `SetGrassMaterialCB` |
| `CSkidmarks` | `Init`, `Shutdown`, `Clear`, `Update`, `Render`, `RegisterOne` (las dos versiones) |
| `CSpecialFX` | `Init`, `Update`, `Shutdown`, `AddWeaponStreak`, `Render`, `Render2DFXs`, `ReplayStarted` |
| `CStreamingInfo` | `Init`, `AddToList`, `GetCdPosn`, `SetCdPosnAndSize`, `GetCdPosnAndSize`, `InList`, `RemoveFromList` |

**Diferencias reales corregidas:**
- `CCopPed`:
  - `AddCriminalToKill` devuelve el índice del criminal si ya estaba en la lista (no -1) y filtra los tipos ≥ `MISSION1` sin tope superior;
  - `RemoveCriminalToKill` suelta la referencia y anula el hueco (llamaba a `RegisterReference`);
  - el destructor suelta la referencia al compañero sin anular el puntero.
- `CPedList::ExtractPedsWithGuns`: no añade con la lista llena (30), como `AddMember` (0x699EE0).
- `CPPTriPlantBuffer`:
  - `GetPPTriPlantPtr` e `IncreaseBufferIndex` comparan con signo;
  - `SetPlantModelsTab` devuelve si el índice era válido;
  - `GetPPTriPlantPtr` no comprueba el índice (sin el assert de `std::array`).
- `CSkidmarks` (y `CSkidmark`, que el EXE lleva en línea):
  - `Update` asigna los tiempos de desvanecimiento (los acumulaba);
  - `Render` usa el alfa de 32 bits como `int16` y calcula las posiciones sin sumar un 0;
  - `RegisterOne` apaga el rastro de sangre cuando no hay hueco;
  - `RegisterOne` (EC0) lee `*bloodState` y `*isSandy`, no los punteros;
  - la esfera del hueco a reciclar usa el radio desde la primera parte y el orden de sumas del EXE.
- `CSpecialFX`:
  - `AddWeaponStreak` usaba un `RwMatrix*` sin inicializar y transformaba el extremo como vector; ahora usa la matriz estática del EXE y transforma ambos extremos como puntos con el orden de 0x59C890;
  - `Render2DFXs`: escala `ancho * (1/640) * 1,5`, posición entera del texto, truncado de la barra antes de restar 70, `rand() % ancho`, factor 0,65 del destello de la foto, orden de las llamadas de fuente y estados de render de la cámara del ascensor;
  - `Init`: orden Push/Find/Set del TXD.
- `CStreamingInfo`:
  - `Init` no toca los flags;
  - `GetCdPosn`/`GetCdPosnAndSize` suman el handle y el desplazamiento en 32 bits;
  - el hook de `GetCdPosn` usa un envoltorio que devuelve el valor en EAX (la función miembro devolvía la estructura por puntero oculto y el EXE se caía);
  - `InList` solo mira el siguiente; se quitan los asserts NOTSA.

**Método:**
- `CCopPed` corre sobre buffers falsos (nunca peatones reales de los pools) con el constructor, el destructor, `SetModelIndex`, `ProcessControl` y las armas de `CPed` simulados, y las referencias trazadas.
- `CStreamingInfo` usa un array falso con relleno a los lados; se cambia `ms_pArrayBase` y se restaura.
- Las marcas, los efectos, los espejos y los buffers temporales se guardan y se restauran en cada ejecución; `RwRenderStateSet` se intercepta cambiando el puntero del motor.
- Fuera de los fixtures:
  - fade igual a desaparición con el tiempo pasado (división por cero en el EXE);
  - tipos de marca > 3 (`NOTSA_UNREACHABLE`);
  - NaN en posiciones y salud.
- Se enmascaran los dwords de relleno de la matriz estática de `AddWeaponStreak`: el EXE los copia con `rep movs` desde el temporal de la pila (sin inicializar).

Repetir: `python reversing/Data7/build_probe.py` y `python reversing/run.py --case data7`. Evidencia: [validation-summary.json](validation-summary.json).
