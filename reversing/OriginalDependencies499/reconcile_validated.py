"""Reconcile cohort rows validated by OTHER banks (registry/approvals) without granting any credit not backed by a real run.

Writes approved-reconciled.json (same shape as approved.json, plus audit fields). Never touches targets.csv/baseline.json.
A row is accepted only when ALL of these hold:
  * the address is in the cohort (targets.csv) and is not already in approved.json / approved-level6.json;
  * docs/hooks.csv has exactly one hook at that address, reversed=1 and validated=1, with class::fn == cohort identity;
  * validation-registry.json has exactly one entry at that address with the same identity, evidence file present, SHA256 equal;
  * evidence JSON: passed, all_equal, differences==0, state_restored, covered_functions[identity] is True, functions[identity]>0;
  * CppOnlyMigration/approvals.json[address] (when present) agrees: differences 0, null_comparison_records 0, state_restored;
  * the run's summary.json has passed and all_equal and state_restored; comparisons.jsonl.gz streams to the recorded log SHA,
    and the comparison events of that function (name == identity or identity + '[...]') number exactly functions[identity],
    all equal/accepted with non-null original/candidate. Fixtures of other functions of the same bank give no credit.
"""
import csv, gzip, hashlib, json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
EXE = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
load = lambda p: json.loads(Path(p).read_text(encoding='utf-8'))


def sha(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for chunk in iter(lambda: f.read(1 << 20), b''):
            h.update(chunk)
    return h.hexdigest()


def verify_row(address, identity, hooks, registry, approvals):
    va = int(address, 16)
    why = []
    hk = [h for h in hooks if int(h['address'], 16) == va]
    if len(hk) != 1 or hk[0]['reversed'] != '1' or hk[0]['validated'] != '1' or hk[0]['class'] + '::' + hk[0]['fn_name'] != identity:
        return None, 'catalog: no single reversed+validated hook with the cohort identity'
    rg = [r for r in registry if int(r['hook']['address'], 16) == va]
    if len(rg) != 1 or rg[0]['hook']['class'] + '::' + rg[0]['hook']['fn_name'] != identity:
        return None, 'registry: no single entry with the cohort identity'
    ev = rg[0]['evidence']
    path = ROOT / ev['path']
    if not path.exists() or sha(path) != ev['sha256']:
        return None, 'registry: evidence missing or SHA mismatch'
    s = load(path)
    for key in ('passed', 'all_equal', 'state_restored'):
        if s.get(key) is not True:
            return None, 'evidence: ' + key
    if s.get('differences') != 0 or s.get('original_exe_sha256') != EXE:
        return None, 'evidence: differences/exe'
    if s.get('covered_functions', {}).get(identity) is not True or s.get('functions', {}).get(identity, 0) <= 0:
        return None, 'evidence: function not covered by its own pairs'
    count = s['functions'][identity]
    ap = approvals.get('0x%08X' % va)
    if ap is not None and (ap.get('differences') != 0 or ap.get('null_comparison_records') != 0 or ap.get('state_restored') is not True):
        return None, 'approvals.json disagrees'
    run = ROOT / s['run']
    rs = load(run / 'summary.json')
    if not (rs.get('passed') is True and rs.get('all_equal') is True and rs.get('state_restored') is True):
        return None, 'run summary not passed/all_equal'
    gz = run / 'comparisons.jsonl.gz'
    h = hashlib.sha256()
    n = bad = 0
    with gzip.open(gz, 'rb') as f:
        for line in f:
            h.update(line)
            e = json.loads(line)
            if e.get('event') != 'comparison':
                continue
            fn = e['function']
            if fn == identity or fn.startswith(identity + '['):
                n += 1
                if not (isinstance(e.get('original'), dict) and isinstance(e.get('candidate'), dict)
                        and e['original'] == e['candidate'] and e.get('equal') is True and e.get('accepted') is True):
                    bad += 1
    if h.hexdigest() != s['log_sha256'] or h.hexdigest() != rs['log_sha256']:
        return None, 'run log SHA mismatch'
    if bad or n != count:
        return None, 'own pairs mismatch n=%d expected=%d bad=%d' % (n, count, bad)
    return {'identity': identity, 'validation_summary': ev['path'], 'run': s['run'], 'own_pairs': n,
            'bank_pairs': s['pairs'], 'registry_sha256': ev['sha256'], 'log_sha256': h.hexdigest()}, 'ok'


def main():
    targets = list(csv.DictReader((HERE / 'targets.csv').open(encoding='utf-8-sig', newline='')))
    progress = {r['address']: r for r in csv.DictReader((HERE / 'progress.csv').open(encoding='utf-8-sig', newline=''))}
    taken = set(load(HERE / 'approved.json')) | set(load(HERE / 'approved-level6.json'))
    prev_path = HERE / 'approved-reconciled.json'
    out = load(prev_path) if prev_path.exists() else {}
    hooks = list(csv.DictReader((ROOT / 'docs/hooks.csv').open(encoding='utf-8-sig', newline='')))
    registry = load(ROOT / 'reversing/validation-registry.json')['functions']
    approvals = load(ROOT / 'reversing/CppOnlyMigration/approvals.json')
    hooked = {int(h['address'], 16) for h in hooks}
    added, rejected = [], []
    for t in targets:
        a = t['address']
        row = progress[a]
        if a in taken or a in out or row['runtime_status'] == 'passed':
            continue
        if int(a, 16) not in hooked:
            continue
        res, why = verify_row(a, row['identity'], hooks, registry, approvals)
        (added if res else rejected).append((a, row['identity'], why))
        if res:
            out[a] = res
    prev_path.write_text(json.dumps(out, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    print(json.dumps({'reconciled_total': len(out), 'added_now': [x[:2] for x in added], 'rejected': rejected}, ensure_ascii=False))


if __name__ == '__main__':
    sys.exit(main())
