"""Coordinator-only negative replay. Preserves failure; allows measured natural exit."""
import hashlib,json,sys
from pathlib import Path
if not __debug__:raise RuntimeError('Negative baseline requires assertions enabled')
ROOT=Path(__file__).resolve().parents[2]
BANK=ROOT/'reversing/RequestedPadDistanceTrainNop'
sys.path.insert(0,str(BANK))
import run_local

def main():
    strict=run_local.analyze
    def failed_terminal(log):
        try:
            result=strict(log)
            if result is None:return None
            return dict(result,passed=False,negative_baseline=True,no_runtime_approval=True,
                        unexpected_clean_negative_baseline=True,
                        negative_launch_sha256=run_local.runner.digest(Path(__file__)))
        except RuntimeError as error:
            if not str(error).startswith('Strict Pad distance/train/NOP analyzer rejected: '):raise
            d=run_local.diagnostics
            if not d or d.get('pairs')!=13952 or d.get('pairs_per_function')!=run_local.EXPECTED:raise
            final=d.get('terminal_bank_record',{});hfinal=d.get('terminal_harness_record',{})
            if final.get('event')!='error' or final.get('pairs')!=13952:raise
            if not all(final.get(k) is True for k in ('state_restored','entries_restored','collaborator_entries_restored','flags_restored','fp_restored','globals_restored')):raise
            for before,after in (('fp_control_before','fp_control_after'),('fp_status_before','fp_status_after'),('mxcsr_before','mxcsr_after')):
                if type(final.get(before)) is not int or final.get(before)!=final.get(after):raise
            if not all(hfinal.get(k) is True for k in ('state_restored','entries_restored','tables_restored','flags_restored','timer_restored')):raise
            harness=run_local.runner.GAME/'preflight-pad-distance-train-nop-harness.jsonl'
            if not run_local.runner.terminal_event(log) or not run_local.runner.terminal_event(harness):raise
            stages=[json.loads(line).get('event') for line in harness.read_text(encoding='utf-8').splitlines()]
            if stages!=['preflight_entered','before_task_bank','task_bank_returned','complete']:raise
            return dict(passed=False,all_equal=False,strict_analyzer_error=str(error),negative_baseline=True,
                        no_runtime_approval=True,negative_launch_sha256=run_local.runner.digest(Path(__file__)),**d)
    run_local.analyze=failed_terminal
    try:return run_local.main()
    finally:run_local.analyze=strict
if __name__=='__main__':raise SystemExit(main())
