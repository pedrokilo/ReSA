<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Garajes (lista de 21)

Validadas el 03/10/2026 con **7.936 pares original/candidato dentro de GTA**, todos exactos: 21 funciones de 1 clase con `validated=1`.

| Clase | Funciones |
|---|---|
| `CGarages` | `ActivateGarage`, `AllRespraysCloseOrOpen`, `ChangeGarageType`, `CloseHideOutGaragesBeforeSave`, `DeActivateGarage`, `GetGarageNumberByName`, `Init`, `Init_AfterRestart`, `IsCarSprayable`, `IsGarageClosed`, `IsGarageOpen`, `IsModelIndexADoor`, `IsPointWithinHideOutGarage`, `Load`, `PlayerArrestedOrDied`, `PrintMessages`, `Save`, `SetTargetCarForMissionGarage`, `Shutdown`, `TriggerMessage`, `Update` |

**Diferencias reales corregidas:**
- `Update`:
  - solo llama a `CGarage::Update` en garajes con tipo válido;
  - incrementa `LastUpdatedGarageId` antes de elegir el garaje a ordenar;
  - la prueba de cercanía es "no mayor que 40" (NaN cuenta como cerca), con el orden de resta del EXE;
  - el contador de fotogramas se prueba con `& 0xF`.
- `ChangeGarageType` modificaba una copia del garaje (no tenía efecto).
- `ActivateGarage` abre la puerta solo si está cerrada (estaba invertido).
- `CloseHideOutGaragesBeforeSave` actúa con cualquier puerta no cerrada, no solo abierta.
- `IsPointWithinHideOutGarage` sigue con el resto de escondites (sobraba un `break`).
- `IsCarSprayable` compara con -2 (no con `MODEL_ARTICT1`) y el modelo con signo.
- Mensajes:
  - `TriggerMessage` reinicia con tiempo estrictamente mayor que el fin, usa `strcpy` y guarda el primer número en 0x96C010 y el segundo en 0x96C00C;
  - `PrintMessages` pasa los números en ese orden, usa la mitad entera del ancho y las operaciones float exactas del EXE.
- `Shutdown` pone `NumGarages` a 0.
- `Save` cierra antes los garajes de escondite, no inicializa el `CSaveGarage` y guarda `CarTypesCollected` desde el global.
- `SetTargetCarForMissionGarage` no llama a `CleanUpOldReference` (se quitó el `FIX_BUGS`).
- `IsModelIndexADoor` llama dos veces a `AsAtomicModelInfoPtr`; `Init_AfterRestart`, `AllRespraysCloseOrOpen`, `GetGarageNumberByName`, `Load` y `Save` comparan sin signo con `NumGarages`.
- `CSaveGarage::CopyGarageIntoSaveGarage`/`CopyGarageOutOfSaveGarage` copian el nombre con `strcpy`.

**Método:**
- Los globales reales de `CGarages` (0x96ABD8–0x96EA7C) se rellenan de forma determinista y se restauran en cada ejecución y al final.
- Dependencias simuladas y trazadas en la dirección del EXE y en el símbolo de la DLL: métodos de `CGarage`, audio de puertas, fuentes, textos, `InsertNumberInString`, referencias de entidades y búferes de trabajo de guardado/carga (sin ficheros reales).
- `CGarage::PlayerArrestedOrDied` e `IsPointInsideGarage` se simulan con envoltorios desnudos que conservan ECX/EDX, como exigen los bucles del EXE.
- Vehículo falso; la cámara usa una matriz falsa o la posición del placement (restauradas), igual que el reloj, los fotogramas, el modo de repetición y `RsGlobal`.
- En `Save` se enmascaran el relleno de `CSaveGarage` y los bytes tras el nombre (basura de pila en el EXE).
- Fuera de los fixtures: `NumGarages` mayor que 50, nombres de garaje sin terminar en 8 bytes y etiquetas de mensaje de 8 caracteres o más.

Repetir: `python reversing/Garages21/build_probe.py` y `python reversing/run.py --case garages21`. Evidencia: [validation-summary.json](validation-summary.json).
