<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8939 funciones reconstruidas; 8939 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Alternative isolated compiler study

Pending offline candidate only, no runtime admission. Full emitted bodies and object hashes for six variants are stored in alternative-fullbody-proof.json.

explicit_returns solves the ECX tail: if(IsPlayer(source)) return 1; return 0. volatile_result/volatile_bool still overwrite ECX. volatile_constants adds mutable globals. unsigned_call loses TEST AL and assumes canonical byte return.

fastcall_edx combines explicit returns with bool __fastcall(intelligence, victim_dummy_edx, source_stack). MSVC emits MOV EDX,victim, loads intelligence from EDX+47C, moves ECX=intelligence, pushes source and calls actual collaborator 601C30. This matches the EXE input register/stack contract. Threatened and its 608970/608830 callees preserve EDX. Friendly later sets EDX=victim pedType and ECX=source pedType; IsPlayer preserves ECX=source and EDX. Complete per-branch flow inference is in the proof JSON.

This is a compiler-specific machine ABI extension, not ISO C++ function-type compatibility: actual collaborator is declared thiscall and invoked through an explicitly extended fastcall signature. Root must review integration legality/contract. It does not delegate EventSource itself to GAME.

No full-body byte equality claimed. Different prologue/call style requires actual virtual GetSourceEntity inspection for incoming EAX/stack dependencies. Raw-bank EAX/ECX/EDX, ESP, nonvolatile/SEH gates must remain unchanged. No bank was executed. EFLAGS are absent from the historical bank; false IsPlayer returns with XOR EAX instead of original MOV EAX,EBX after TEST AL. Defined zero-result flags agree but AF is undefined, so no universal flags identity claim.

Only isolated files were compiled; no project build, production/bank edits, GTA or Git. Work stopped as ordered after finishing the active offline study.
