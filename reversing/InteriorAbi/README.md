<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7496 con evidencia histórica, 0 con vigencia acreditada y 950 pendientes en 13 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorAbi

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-093901-872320`, 2048 pares. Dos entradas ABI fuera del catálogo.

Canonical original EXE entry is temporarily patched only for candidate calls to the nonvirtual DLL Constructor/Destructor wrappers. Every pair checks the correct side using JumpsToDll; all five original entry bytes restored after each function. Full0x794-byte before/after snapshots use lossless hexadecimal.

Constructor requires EAX=this and exact writes:prev/next0..7 zero;list5C..67 zero;word3EC andword3F0 FFFF. Every other byte remains unchanged, including NodeId3F2..3F3, which spans all byte values in fixtures. Destructor must leave all bytes unchanged; its two native list destructors are no-ops. No real constructor is invoked on fixtures. Ordinary C++ default construction initializes CNodeAddress nodeIdFFFF additionally; only explicit native ABI wrapper is tested.

REA+ASM:[Constructor](../InteriorEvidence/Interior_Constructor.txt),[Destructor](../InteriorEvidence/Interior_Destructor.txt). Original collaborator bodies4A8DB0/4A8DD0 write the base/list fields;4A8DC0/4A8DE0 are RET. No engine entities, files or render events are exercised. Runtime and compilation remain root-owned.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-093901-872320`: 2,048 pairs across 2 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Bonus ABI evidence outside the52 catalog rows.

<!-- END INTERIOR FINAL VALIDATION -->
