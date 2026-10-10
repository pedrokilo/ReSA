<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9328 entradas binarias sin nombre catalogado; 924 destinos originales explícitos fuera del catálogo, 499 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Serial execution and partial audit, 2026-10-08

Read-only review of two frozen banks. No build, GTA, source change or registry write performed.

## Current evidence and ordering

Both prepared freezes verify without differences: Pad freeze SHA256 `ba1ae43c6e26097ac6eb7909494c39434a8206adb93808ca301fee687bf58eda` (26 files); Dispatch freeze `294c6bac59ab744b002db13f114514e33dd2383d32996e57593eb5caaf7c01e2` (19 files). Shared runner SHA256 `a27eb0b2fbc89a12d3d1f6b91ff2c5cde0944c91b5f7b03e2ac02fc1ece8cf3a` already preserves explicit failed analysis.

Current Pad.cpp SHA256 `b0902674db3e303e58c1bb9b875439a1e3ec02eaa8fa0acb8a619ab6774adaf2`, Pad.h `d18adb99148fe3fbdde2447a44699451f3afce97f29994bdf3bd29a3829889e2` match the coordinator's ABI-only Train fix. Frozen scope/README status describes preparation time; the build manifest, static member-signature assertion and matching compiled RET8 proof are authoritative for actual readiness. Train source gate remains deliberately incorrect for the negative baseline.

The complete source inventory is 2,332 implementation/header files plus four root build configuration files. Each builder captures hashes before compiling and rechecks recorded files after compiling. The runner also rejects added/deleted source files by inventory equality. No source/configuration/instrument edit is allowed between a bank build and its replay; any such edit requires rebuilding that bank. The builders' different lock names do not serialize with one another or with Hex. The coordinator must provide a single serial queue. Reading files and writing unrelated audit reports are safe. Do not edit WinMain or normal bin artifacts while any builder is active; each builder temporarily owns and restores them.

Recommended sequence: finish active Hex work; build and replay ABI-only Pad negative baseline; preserve/audit its results; perform the authorized Train logic fix; build and replay Pad final; audit/register passing final functions; build/replay/audit Dispatch against the now-stable complete source inventory. Alternatively Dispatch can run before the Train logic fix, but it must finish its build and replay before that edit and its registration must cite the historical manifest rather than a later source hash.

## Exact commands (coordinator only)

Run individually from `D:\Original\gta-reversed-master`, inspect exit/result before the next command:

```powershell
python -B reversing/RequestedPadDistanceTrainNop/build_probe.py
python -B reversing/RequestedPadDistanceTrainNop/run_local.py
```

The first command must produce current matching ASI/PDB and `artifacts/train-compiled-abi.json`; the latter must contain one unique `CPad::StartShake_Train`, matching ASI/PDB hashes and all RET operands equal to eight. Static reader never loads ASI. Negative baseline is expected to exit nonzero from the second command and preserve `passed=false`. Do not treat its aggregate status as approval for any function.

After the final authorized logic fix, repeat both Pad commands; the builder preserves previous artifacts under build-history. After Pad final execution and before further source changes:

```powershell
python -B reversing/RequestedDispatchShadowFour/build_probe.py
python -B reversing/RequestedDispatchShadowFour/run_local.py
```

Pad exact product is 1,024 NOP + 10,240 Distance + 2,688 Train = 13,952 unique pairs. Dispatch exact product is 1,024 Initialise + 1,024 Shutdown + 1,024 PluginAttach + 2,048 ReturnRealTimeShadow = 5,120 unique pairs.

## Important negative-baseline exit caveat

The frozen strict analyzer raises on mismatches before shared runner reaches `wait_for_exit`. Shared runner then preserves failure and may terminate a still-running process. Therefore a negative summary can lack `natural_exit` even when the bank/harness have fully restored. Missing natural-exit evidence cannot be silently inferred from a complete harness or a WinMain return statement. Preserve the negative attempt for defect evidence; the straightforward approval path is the final corrected replay.

If the coordinator needs partial approval directly from a negative baseline, it can use a separately hashed launch wrapper, without editing frozen files, which converts the strict analyzer exception into explicitly failed evidence after terminal records. This preserves strict failure while allowing shared runner to measure normal exit. The wrapper itself is additional evidence and its hash/text must be retained with that attempt. Example wrapper body:

```python
import sys
from pathlib import Path
bank = Path(r'D:\Original\gta-reversed-master\reversing\RequestedPadDistanceTrainNop')
sys.path.insert(0, str(bank))
import run_local
strict = run_local.analyze
def negative_analysis(log):
    try:
        return strict(log)
    except RuntimeError as error:
        # Only strict rejection after diagnostics and both terminal logs qualifies.
        if not run_local.diagnostics or not run_local.runner.terminal_event(log):
            raise
        harness = run_local.runner.GAME / 'preflight-pad-distance-train-nop-harness.jsonl'
        if not run_local.runner.terminal_event(harness):
            raise
        return dict(passed=False, strict_analyzer_error=str(error),
                    **run_local.diagnostics)
run_local.analyze = negative_analysis
raise SystemExit(run_local.main())
```

This does not approve the failed bank or suppress its rejection. Never override a result to `passed=true`; do not normalize trace, bool AL, duration, floats or helper roles. Unexpected assertions, access violations, hangs, dialogs, nonzero process exit or uncertain restoration block partial approval.

## Preserved audit and proposed partial records

Audit the saved attempt rather than live GAME aliases. Verify exact executed ASI hash, PDB/compiled-source/instrument/proof hashes, original EXE hash, full raw or lossless gzip hash, outer file/config/save restoration and natural exit with exit code zero. Replay independent `oracle.expected_row` for every comparison, require both routes equal to its exact output and require `equal=true`; accumulate each function separately. Check all pair IDs and all function/case IDs without duplicates across the entire product; no comparison after terminal. On negative baseline allow only a final `event=error` with `pairs=13952` and measured true restoration fields; preserve aggregate failure and exact discrepancy counts. Harness stages must be exactly entered/before_task_bank/task_bank_returned/complete with all restoration fields true. Model mismatch counters are aggregate and cannot approve a subgroup.

Only a subgroup with its exact full product, zero disagreements, passing independent per-row oracle, intact guards/readonly regions/helper proof/FP and all shared restoration/exit prerequisites may receive a distinct per-function validation summary whose own `passed` is true. Link the failed parent attempt and explicitly state Train excluded. Do not rewrite its original summary or reuse parent aggregate `passed` as the registry passed path. If the negative attempt lacks natural-exit evidence, defer any registration until final replay.

For records resolve the unique hook identity from docs/hooks.csv using class/fn_name/address/type; never invent type/name. Proposed Pad identities are CPad ProcessPCSpecificStuff 53FB40 (1,024), StartShake_Distance 53F9A0 (10,240), StartShake_Train 53FA70 (2,688). Baseline Train is excluded until corrected and passing. Registry evidence points to a separately hashed audited per-function summary with `passed_path=['passed']`, pair count and canonical EXE hash; tested_sources is the exact build manifest. Historical source hashes may differ after later authorized corrections and must not be relabeled current. Scope excludes FindPlayerVehicle implementation, vehicle construction/lifecycle, camera resource ownership, general gameplay/startup and untested FP modes. Dispatch records similarly exclude private collaborators' implementations and all graphics integration. Write no registry entry until the coordinator completes this audit.

## Preparation review limits

Pad synthetic checks cover every fixture row and seven adversarial mutations; Dispatch frozen synthetic checks remain intact. Static/synthetic acceptance is not runtime approval. Actual compile may expose compiler/SDK issues; the current ASI RET8 gate is intentionally strict. CPad/matrix fixtures have real local object lifetimes; CVehicle fixture is an explicit field-only ABI image with no lifecycle credit. No new source correction or change to the frozen banks was made in this review.
