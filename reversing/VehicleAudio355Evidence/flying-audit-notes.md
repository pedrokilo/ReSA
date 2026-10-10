<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Flying control reconstruction checkpoint

Target: `CPlane::ProcessFlyingCarStuff`, `0x6CB7C0`, final return `0x6CC489`.
Canonical assembly: `Plane_FlyingFull.txt`. Recovered REA pseudocode:
`../Pending355/artifacts/missing-rea-recovery/CPlane.FlyingControl_6CB7C0.c`.

The helpers in `Plane_FlyingHelpersDraft.txt` are private drafts, not applied
implementations and not validated. The target in Plane.cpp is still a forwarding
wrapper. No reconstruction or validation credit applies yet.

Critical details checked against the recovered pseudocode and full assembly:

* Wave range bounds are rounded before the first random call. Time step is read
  after that call. Current draft helper parameters must be revised to preserve
  that read order. Wave increment performs two CRT `ftol2` conversions, treating
  the first low 32-bit result as unsigned for multiplication. Integer wave
  addition must wrap through uint32 rather than signed overflow.
* Particle emission consumes **seven** random values: velocity x/y/z, one
  discarded normalized draw at 0x6CBFF0..0x6CC003, then position x/y/z.
  The discarded draw is observable through RNG state and callback order.
* Particle velocity globals are 0x871A74 and 0x858C20. REA resolves them to
  approximately 0.20000005 and 0.9; earlier inferred 0.4 and 0.8 were incorrect.
  Read the globals directly. Velocity z is rounded after multiplying by .25,
  then multiplied by 50; x/y have one store after both multiplications.
* Hydra flying condition at 0x6CC3F7 compares acceleration to the double at
  0x859EF8 (REA resolves zero), then uses **AND** between wheel count <4 and
  absence of physical flag 0x08000000. Flying handling pointer 0x388 must be
  restored after either branch, including callback mutation.
* Speed squared gate at 0x6CC381..0x6CC3AB compares an extended precision sum
  to double 0x859068, not a rounded float magnitude. NaN does not pass.
* GetAeroplaneCompStatus executes before checking node presence for every
  node 12 through 24. Four panels use signed 16-bit node identity and stride
  0x20. Nodes 21..24 do not emit or update steering.
* The height-ceiling dependency is called three times on the positive branch,
  with a fresh read of plane position z each time. Helpers must preserve
  extended return precision and the original clamp ordering.

No shared source is modified by this checkpoint. Parent owns runtime/build.
