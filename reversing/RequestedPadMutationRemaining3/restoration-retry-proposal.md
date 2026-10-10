<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Proposed instrumental revision 3, not applied

Observed twice: `OSError(22)` at `win.write_bytes(saved)` in builder `finally`. Second build completed artifact capture and DIA before failing. Root restored manually. Independent inspection verifies original WinMain SHA `d60e6f1219f1e81c7288de72949973132832bd7e699b46a80b89f537e4f417be`, both baseline binaries byte equal to `before.*`, and captured ASI/layout SHA equal to manifest. This establishes current restoration; it does not turn builder exit 1 into a successful builder execution.

Proposed narrow repair: a dedicated WinMain restoration helper attempts `write_bytes(saved)` at most three times, with 250 ms between attempts **only for errno 22**. After each attempt, read and compare the complete file against saved bytes. Record attempt number, errno, and whether bytes already equal saved. A write raising errno22 can already have restored the exact bytes; if so, record that condition explicitly as verified restoration. Any other errno, final nonmatching bytes, or unreadable file raises. Existing binary restoration and final SHA assertions remain mandatory. No generic retry around compilation, DIA, source hash checks or artifact capture.

Before applying: archive revision2 builder/freeze, captured failure context/log/manual-restoration evidence. Revision3 updates only builder hash and records why; preserve the 95,052 fixtures and analyzer contracts. Test helper with injected errno22 before-write and after-write failures, permanent errno22, non22 error, and normal write. Builder should emit a separate restoration-attempt report so a recovered transient failure remains reviewable.

No retry is installed by this proposal. No runtime or functional source behavior changes.
