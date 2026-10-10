<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaneBlow355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPlane::BlowUpCar`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-plane-blow355/20261008-052345-729902/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

<!-- vehicle355-status -->
Current status (2026-10-08): Current-source final replay/hash verification remains pending; no current-source approval is asserted. Latest completed report: 20261008-052345-729902, passed for the tested snapshot. Latest historical passing report: 20261008-052345-729902.
Native implementations are applied; older preparation and approval notes below describe historical snapshots.
<!-- /vehicle355-status -->

BlowUpCar 0x6CCCF0 VMT871948 slot41:1024 pares pendientes. ASM integro Plane_BlowFull. Gate damage, NPC crash mission immediate/deferred, player credit with focused havoc/chase globals and canonical CRT RNG0..32767, signed mod6000, current-status reread after Stat mutation, occupants invisible/velocity reset, destruction/body parts/atomic hide, original damager preserved, on-duty counters/fire/explosion (hideExplosion ignored EXE).16 paired deps traced; camera/law/fire mutate position; stat mutates status/model/clock. Full plane/peds/atomic/globals hashes; player stats2x12,counts8,focus/time restored. Scoped hooksfalse until native validation.


Strict semantic pointer outputs added before byte-hash cleanup: NULL, fixture region+byte offset, or unknown_exact+full uintptr value. Unknown values never collapse into a shared sentinel. Current matrix/driver/physical damager/last damage entity are compared; all passenger identities and all car-node identities are compared. The existing full remaining-object byte hash and ordered collaborator traces remain mandatory. Raw fixture pointers are then cleared only after semantic identity capture. No production source change.

<!-- vehicle355-evidence-links -->
Evidence files verified on disk (2026-10-08):

- [CPlane.BlowUpCar_6CCCF0.c](../_handoff/ghidra/CPlane.BlowUpCar_6CCCF0.c)
- [Plane_BlowFull.txt](../VehicleAudio355Evidence/Plane_BlowFull.txt)

The comparison covers isolated symmetric fixtures and ordered collaborator calls. Historical passing reports apply only to the tested snapshot. Final current-source replay/hash verification remains pending; arbitrary runtime graphs, invalid pointers and unconstrained external callbacks are outside these fixtures.
<!-- /vehicle355-evidence-links -->
