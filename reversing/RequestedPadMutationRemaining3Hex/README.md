# RequestedPadMutationRemaining3Hex

## Repeticion del 10/11/2026 (REPARAR-REQUESTED)

Estado: **REPETIBLE y APROBADO** con el codigo actual de `main` (`BENCH_ARGS="--bank shake" bash reversing/RequestedTools/bench_here_args.sh RequestedPadMutationRemaining3Hex`): **89600 (CPad::StartShake) pares EXE/DLL, cero diferencias**, restauracion exacta, salida natural. Corrida `20261110-121256-639754` (archivada en `reversing/CppTwelveLevel6Final/runs/RequestedPadMutationRemaining3Hex-20261110-121256-639754`).

Que fallaba: `VTableRanges.hpp` estaba congelado en octubre (454 filas) y el preflight paraba con `unknown_vtable_category` (primero `CEventDraggedOutCar`) en cuanto una clase con hooks virtuales no figuraba en la tabla: faltaban 17 clases `CEvent*` registradas despues (`CEventChatPartner`, `CEventDeadPed`, `CEventDraggedOutCar`, `CEventFireNearby`, `CEventGotKnockedOverByCar`, `CEventGroupEvent`, `CEventGunAimedAt`, `CEventInWater`, `CEventInteriorUseInfo`, `CEventPedEnteredMyVehicle`, `CEventPedToChase`, `CEventPedToFlee`, `CEventPotentialGetRunOver`, `CEventRevived`, `CEventSexyPed`, `CEventSexyVehicle`, `CEventSpecial`). Arreglo solo en el banco: `reversing/RequestedTools/vtable_ranges_sync.py --apply` anade las filas ausentes con direccion y ranuras de `RH_ScopedVirtualClass` (0 filas eliminadas, 0 filas con datos distintos), `reversing/RequestedTools/refreeze_generic.py` recalcula los SHA256 del congelado (LF de git y versiones actuales de los ficheros compartidos) y `build_probe*.py` respeta `_CL_` (compilacion en serie bajo candado; antes forzaba `/MP6`). Subbanco `shake`.

Sin cambios de produccion. Evidencia y registro refrescados con `refresh_bank.py`.
