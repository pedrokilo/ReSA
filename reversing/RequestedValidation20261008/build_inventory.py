"""Read-only scope accounting; does not accredit or alter registry entries."""
import csv, hashlib, json, re, subprocess, sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
from validation_registry import KEY, evaluate

def main():
    out = ROOT/'reversing/RequestedValidation20261008'
    scope_file = out/'scope.json'
    previous = json.loads(scope_file.read_text(encoding='utf-8')) if scope_file.exists() else None
    initial_keys = {tuple(r[k] for k in KEY) for r in previous['functions'] if str(r['requested_pending']) == '1'} if previous else None
    request = Path('C:/Users/pedro/.codex/attachments/a4ef269d-e31c-4279-9495-72bfe3eaab16/Texto pegado.txt').read_text(encoding='utf-8-sig')
    expected = {}
    for line in request.splitlines():
        match = re.match(r'\d+\. \*\*Falta(?:n)? (\d+):\*\* (.+)', line)
        if match:
            expected.update({c: int(match[1]) for c in re.findall(r'`([^`]+)`', match[2])})
    hooks = list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig', newline='')))
    registry = json.loads((ROOT/'reversing/validation-registry.json').read_text(encoding='utf-8'))
    records = {tuple(r['hook'][k] for k in KEY): r for r in registry['functions']}
    assessment = evaluate(ROOT, ROOT/'docs/hooks.csv') if '--full-audit' in sys.argv else dict(functions=[])
    states = {tuple(r['hook'][k] for k in KEY): r for r in assessment['functions']}
    summaries = defaultdict(list)
    for path in sorted((ROOT/'reversing').glob('*/validation-summary.json')):
        obj = json.loads(path.read_text(encoding='utf-8-sig'))
        pair_map = obj.get('pairs_per_function', {})
        covered_map = obj.get('covered_functions', {})
        pair_map = pair_map if isinstance(pair_map, dict) else {}
        covered_map = covered_map if isinstance(covered_map, dict) else {}
        names = set(pair_map) | set(covered_map)
        for name in names:
            summaries[name].append(dict(path=path.relative_to(ROOT).as_posix(), passed=obj.get('passed'), pairs=pair_map.get(name), covered=covered_map.get(name)))
    rows, counts = [], defaultdict(Counter)
    for hook in hooks:
        if hook['class'] not in expected:
            continue
        key = tuple(hook[k] for k in KEY)
        rec, state = records.get(key, {}), states.get(key, {})
        if '--full-audit' not in sys.argv:
            state = dict(evidence_ok=hook['validated'] == '1')
        pending = hook['validated'] == '0'
        requested = key in initial_keys if initial_keys is not None else pending
        status = 'pending_reconstruction' if hook['reversed'] != '1' else 'current' if state.get('current') else 'historical_linked' if state.get('evidence_ok') else 'unlinked'
        candidates = summaries.get(hook['class']+'::'+hook['fn_name'], [])
        evidence = rec.get('evidence', {})
        row = dict(hook, requested_pending=int(pending), validation_state=status, source_match=int(bool(state.get('source_match'))), current=int(bool(state.get('current'))), bank=Path(evidence['path']).parent.name if evidence else '', evidence=evidence.get('path',''), pairs=evidence.get('pairs',''), dependency_review=int(bool(rec.get('dependency_review'))), candidate_banks=';'.join(Path(c['path']).parent.name for c in candidates), candidate_evidence=json.dumps(candidates, ensure_ascii=False), differences='Not established by inventory audit; requires replay or evidence review', limitations=rec.get('scope','No linked validation scope'))
        row['requested_pending'] = int(requested)
        row['pending_now'] = int(pending)
        rows.append(row)
        counts[hook['class']][status] += 1
        counts[hook['class']]['total'] += 1
        counts[hook['class']]['requested_pending'] += requested
        counts[hook['class']]['pending_now'] += pending
        counts[hook['class']]['pending_with_candidate_summary'] += pending and bool(candidates)
    out = ROOT/'reversing/RequestedValidation20261008'
    with (out/'inventory.csv').open('w',encoding='utf-8-sig',newline='') as stream:
        writer = csv.DictWriter(stream,fieldnames=list(rows[0])); writer.writeheader(); writer.writerows(rows)
    scope = dict(schema_version=1, requested_base='24adc9887871e167b4b64e981446b265ba1e16df', inspected_head=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(), selection='Classes requested by user; requested_pending=1 selects initial validated=0 identities. Historical rows are context, not an automatic replay expansion.', hooks_sha256=hashlib.sha256((ROOT/'docs/hooks.csv').read_bytes()).hexdigest(), registry_sha256=hashlib.sha256((ROOT/'reversing/validation-registry.json').read_bytes()).hexdigest(), classes={c:dict(expected_pending=expected[c],**counts[c]) for c in expected}, totals=dict(Counter(r['validation_state'] for r in rows)), total_identities=len(rows), requested_pending=sum(r['requested_pending'] for r in rows), functions=[{k:r[k] for k in (*KEY,'requested_pending','validation_state','bank','evidence','candidate_banks')} for r in rows], limitations=['Only exact Class::function names matched candidate summaries; aliases/overloads require human review.', 'Candidate summary passed=true does not accredit current sources or dependencies.', 'Read-only registry evaluation verifies registered summary/source/dependency hashes; it does not replay GTA or re-hash remote historical logs.'])
    scope['audit_mode'] = 'full_registry_evaluate' if '--full-audit' in sys.argv else 'selection_only_current_unverified'
    scope['pending_now'] = sum(r['pending_now'] for r in rows)
    (out/'scope.json').write_text(json.dumps(scope,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:scope[k] for k in ('total_identities','requested_pending','totals')},indent=2))
    print('Pending candidate summaries:',sum(c['pending_with_candidate_summary'] for c in counts.values()))
    print('Expected pending:',sum(expected.values()))
    print('Differences:',{c:(expected[c],counts[c]['requested_pending']) for c in expected if expected[c] != counts[c]['requested_pending']})

if __name__ == '__main__': main()
