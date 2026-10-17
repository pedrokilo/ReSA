<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorAbi

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-093901-872320`, 2048 pares. Dos entradas ABI fuera del catálogo.

Canonical original EXE entry is temporarily patched only for candidate calls to the nonvirtual DLL Constructor/Destructor wrappers. Every pair checks the correct side using JumpsToDll; all five original entry bytes restored after each function. Full0x794-byte before/after snapshots use lossless hexadecimal.

Constructor requires EAX=this and exact writes:prev/next0..7 zero;list5C..67 zero;word3EC andword3F0 FFFF. Every other byte remains unchanged, including NodeId3F2..3F3, which spans all byte values in fixtures. Destructor must leave all bytes unchanged; its two native list destructors are no-ops. No real constructor is invoked on fixtures. Ordinary C++ default construction initializes CNodeAddress nodeIdFFFF additionally; only explicit native ABI wrapper is tested.

REA+ASM:[Constructor](../InteriorEvidence/Interior_Constructor.txt),[Destructor](../InteriorEvidence/Interior_Destructor.txt). Original collaborator bodies4A8DB0/4A8DD0 write the base/list fields;4A8DC0/4A8DE0 are RET. No engine entities, files or render events are exercised. Runtime and compilation remain root-owned.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-093901-872320`: 2,048 pairs across 2 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Bonus ABI evidence outside the52 catalog rows.

<!-- END INTERIOR FINAL VALIDATION -->
