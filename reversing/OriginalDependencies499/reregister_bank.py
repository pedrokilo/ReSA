"""Re-registra bancos aprobados de la cohorte con la evidencia de una re-ejecucion
hecha con los instrumentos ACTUALES (reversing/CppAuditRerun/runs/<Banco>-<ts>/).

Para cada banco:
  1. verifica la evidencia nueva con las mismas comprobaciones que verify_bank.py /
     register_bank.py / audit-cohort-current.py (par a par, en streaming);
  2. copia summary.json, RESULTADO.txt y comparisons.jsonl.gz (byte a byte) a
     reversing/<Banco>/runs/<ts>/ y genera approval-audit.json (mismo esquema);
  3. reescribe reversing/<Banco>/validation-summary.json (run, huellas, manifest,
     reaudit, comparison_storage) conservando el resto de campos;
  4. actualiza SOLO evidence.sha256 de las entradas de reversing/validation-registry.json
     que apuntan a ese validation-summary.json (formato byte a byte: indent=2,
     ensure_ascii, CRLF).
Las corridas antiguas se conservan. approved.json no se toca. Idempotente.

Uso: python reregister_bank.py [--dry-run] [--rerun-root DIR] [Banco ...]
Sin bancos: los 18 de la re-ejecucion del 17/10/2026.
"""
import argparse, gzip, hashlib, json, os, re, shutil, sys, time
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
REV = ROOT / 'reversing'
EXE = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
BANKS = ['DependenciesDecisionPed7', 'DependenciesDecisionRelationship6', 'DependenciesGarage2',
         'DependenciesPadCamera7', 'DependenciesPathOrientation14', 'DependenciesStatsFile15',
         'DependenciesTask10', 'DependenciesTask11', 'DependenciesTaskLeaf3', 'DependenciesTaskPed5',
         'DependenciesVehicleStats10', 'DependenciesVehicleStats11', 'DependenciesWorld10',
         'DependenciesWorld12', 'DependenciesWorld15', 'DependenciesWorld9',
         'DependenciesWorldWaterPath6', 'DependenciesZones13']
GATES = {'route_ok', 'hook_route_ok', 'helper_route_ok', 'library_route_ok', 'stack_restored', 'registers_preserved',
         'nonvolatile_preserved', 'nonvolatile_registers_preserved', 'fp_preserved', 'seh_restored', 'contract_ok',
         'readonly_ok', 'guards_ok', 'protected_state_unchanged', 'expected_effects_correct', 'return_domain_ok',
         'fixture_valid', 'read_range_ok', 'path_canaries_ok', 'optional_outputs_ok', 'callbacks_real'}
FLAGS = ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored')


def sha(path):
    h = hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda: f.read(1 << 20), b''):
            h.update(chunk)
    return h.hexdigest()


def sha_bytes(b):
    return hashlib.sha256(b).hexdigest()


def load(path):
    return json.loads(path.read_text(encoding='utf-8'))


def retry(fn, *a):
    for attempt in range(30):
        try:
            return fn(*a)
        except OSError:
            if attempt == 29:
                raise
            time.sleep(0.3)


def write_bytes(path, data):
    def go():
        path.write_bytes(data)
    retry(go)
    assert path.read_bytes() == data


def copy_exact(src, dst):
    if dst.exists() and sha(dst) == sha(src):
        return
    retry(shutil.copyfile, src, dst)
    assert sha(dst) == sha(src)


def pick_rerun(root, bank):
    best = None
    for d in sorted(root.glob(bank + '-*')):
        if not re.fullmatch(re.escape(bank) + r'-\d{8}-\d{6}-\d+', d.name):
            continue
        try:
            s = load(d / 'summary.json')
        except Exception:
            continue
        if s.get('passed') is True and all((d / n).exists() for n in ('RESULTADO.txt', 'comparisons.jsonl.gz')):
            best = d  # sorted: the last one wins
    return best


def verify_stream(gz, expected, summary_all):
    """Streaming pair-by-pair inspection (same rules as verify_bank + audit-cohort-current)."""
    h = hashlib.sha256()
    nbytes = 0
    counts = Counter()
    cases = {}
    pairids = set()
    begin, complete, fsum, bad = [], [], {}, []
    npairs = 0
    with gzip.open(gz, 'rb') as f:
        for line in f:
            h.update(line)
            nbytes += len(line)
            e = json.loads(line)
            ev = e.get('event')
            if ev == 'comparison':
                npairs += 1
                fn = e['function']
                counts[fn] += 1
                assert e['pair'] not in pairids, 'duplicate pair'
                pairids.add(e['pair'])
                key = e.get('case')
                assert key not in cases.setdefault(fn, set()), 'duplicate case'
                cases[fn].add(key)
                o, c = e.get('original'), e.get('candidate')
                assert isinstance(o, dict) and isinstance(c, dict), 'null original/candidate'
                assert o == c and e.get('equal') is True and e.get('accepted') is True, 'difference pair %s' % e['pair']
                for v in (o, c):
                    assert any(k in v for k in ('route_ok', 'hook_route_ok')), 'missing per-call route gate'
                    for k in GATES.intersection(v):
                        assert v[k] is True, 'false gate ' + k
                    if 'route_ok' in v:
                        assert v['route_ok'] is True
                if 'state_restored' in e:
                    assert e['state_restored'] is True, 'per-pair restore'
            elif ev == 'begin':
                begin.append(e)
            elif ev == 'complete':
                complete.append(e)
            elif ev == 'function_summary':
                assert e['function'] not in fsum, 'duplicate function_summary'
                fsum[e['function']] = e
            elif ev in ('error', 'assertion', 'first_chance_av', 'first_chance_av_raw'):
                bad.append(ev)
    assert not bad, 'error events: %s' % bad
    assert dict(counts) == expected, 'function counts'
    assert pairids == set(range(npairs)), 'pair ids not consecutive'
    for fn, cnt in counts.items():
        assert cases[fn] == set(range(cnt)), 'cases not consecutive ' + fn
        s = fsum.get(fn)
        assert s is not None and s.get('failures') == 0 and s.get('cases') == cnt, 'function_summary ' + fn
        assert 'complete_paths' not in s or s['complete_paths'] is True, 'complete_paths ' + fn
    assert len(fsum) == len(expected)
    assert len(begin) == 1 and begin[0].get('registered_hook') is True, 'begin registered_hook'
    assert len(complete) == 1 and complete[0].get('pairs') == npairs and complete[0].get('state_restored') is True
    return h.hexdigest(), nbytes, npairs


def gz_mtime(path):
    with path.open('rb') as f:
        head = f.read(10)
    return int.from_bytes(head[4:8], 'little')


def process(bank, rerun_root, dry):
    b = REV / bank
    rr = pick_rerun(rerun_root, bank)
    assert rr is not None, 'no hay corrida aprobada en %s para %s' % (rerun_root, bank)
    ts = rr.name[len(bank) + 1:]
    vs_path = b / 'validation-summary.json'
    old = load(vs_path)
    summary = load(rr / 'summary.json')
    expected = old['functions']
    run_rel = 'reversing/%s/runs/%s' % (bank, ts)
    dest = b / 'runs' / ts

    # --- comprobaciones de summary
    assert all(summary.get(k) is True for k in FLAGS), 'flags de summary'
    assert summary['original_exe_sha256'] == EXE
    assert summary['pairs'] == old['pairs'] and summary['pairs_per_function'] == expected, 'pares por funcion'
    assert set(old['covered_functions']) == set(expected) and all(v is True for v in old['covered_functions'].values())
    table = (b / 'LiveComparison.hpp').read_text(encoding='utf-8')
    entries = re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(?:0x[0-9A-Fa-f]+|0),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', table)
    if entries:
        assert {c + '::' + f: int(n) for c, f, n in entries} == expected, 'tabla LiveComparison.hpp vs pares'
    # --- build-manifest vigente (instrumentos y fuentes con los que se compilo el ASI ejecutado)
    manifest = load(b / 'artifacts/build-manifest.json')
    assert summary['executed_probe_sha256'] == manifest['binaries']['asi'], 'ASI ejecutado != build-manifest'
    asi = next(iter((b / 'artifacts').glob('*.asi')), None)
    if asi is not None:
        assert sha(asi) == manifest['binaries']['asi'], 'ASI en artifacts != manifest'
    assert sha(ROOT / 'source/game_sa/Game.cpp') == manifest['game_cpp'], 'Game.cpp cambio tras el build'
    for p, d in manifest['sources'].items():
        assert sha(ROOT / p) == d, 'fuente probada cambio tras el build: ' + p
    for p, d in manifest['instrumentation'].items():
        assert sha(b / p) == d, 'instrumento cambio tras el build: ' + p
    # --- streaming
    log_sha, nbytes, npairs = verify_stream(rr / 'comparisons.jsonl.gz', expected, summary)
    assert log_sha == summary['log_sha256'], 'log SHA != summary'
    assert npairs == summary['pairs']
    assert gz_mtime(rr / 'comparisons.jsonl.gz') == 0, 'gzip mtime != 0'

    audit = dict(bank=bank, run=ts, functions=len(expected), pairs=npairs, pairs_per_function=expected,
                 differences=0, null_comparison_records=0, state_restored=True, saves_unchanged=True,
                 configs_restored=True, hook_routes_verified=True, log_sha256=log_sha,
                 tested_sources=manifest['sources'], bank_sources=manifest['instrumentation'],
                 original_exe_sha256=EXE, executed_probe_sha256=summary['executed_probe_sha256'],
                 registered_hook_verified=True)
    audit_bytes = (json.dumps(audit, indent=2) + '\n').encode('utf-8')
    sum_sha, res_sha, gz_sha = sha(rr / 'summary.json'), sha(rr / 'RESULTADO.txt'), sha(rr / 'comparisons.jsonl.gz')
    new = dict(old)
    new.update(run=run_rel, summary_sha256=sum_sha, result_txt_sha256=res_sha, log_sha256=log_sha,
               all_equal=True, state_restored=True, saves_unchanged=True, configs_restored=True,
               original_exe_sha256=EXE, executed_probe_sha256=summary['executed_probe_sha256'],
               manifest={'tested_sources': audit['tested_sources'], 'bank_sources': audit['bank_sources']},
               reaudit=audit,
               comparison_storage={'compressed_path': run_rel + '/comparisons.jsonl.gz', 'compressed_sha256': gz_sha,
                                   'uncompressed_sha256': log_sha, 'uncompressed_bytes': nbytes, 'compression': 'gzip',
                                   'gzip_mtime': 0, 'lossless_roundtrip_sha256': log_sha})
    assert list(new) == list(old)
    new_bytes = (json.dumps(new, ensure_ascii=False, indent=2) + '\n').encode('utf-8')
    new_sha = sha_bytes(new_bytes)
    already = vs_path.read_bytes() == new_bytes
    if not dry:
        dest.mkdir(parents=True, exist_ok=True)
        copy_exact(rr / 'summary.json', dest / 'summary.json')
        copy_exact(rr / 'RESULTADO.txt', dest / 'RESULTADO.txt')
        copy_exact(rr / 'comparisons.jsonl.gz', dest / 'comparisons.jsonl.gz')
        write_bytes(dest / 'approval-audit.json', audit_bytes)
        if not already:
            write_bytes(vs_path, new_bytes)
    rel = vs_path.relative_to(ROOT).as_posix()
    return dict(bank=bank, run=ts, pairs=npairs, functions=len(expected), already=already,
                old_run=old['run'].rsplit('/', 1)[-1], summary_path=rel, new_sha=new_sha, old_sha=sha(vs_path))


def update_registry(results, dry):
    reg = REV / 'validation-registry.json'
    raw = reg.read_bytes()
    data = json.loads(raw)
    assert (json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8') == raw, \
        'formato actual del registro no reproducible byte a byte'
    by = {r['summary_path']: r for r in results}
    touched = Counter()
    for f in data['functions']:
        r = by.get(f['evidence']['path'])
        if r is None:
            continue
        touched[r['bank']] += 1
        f['evidence']['sha256'] = r['new_sha']
    for r in results:
        assert touched[r['bank']] > 0, 'sin entradas de registro para ' + r['bank']
    out = (json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8')
    if out != raw and not dry:
        tmp = reg.with_suffix('.reregister.tmp')
        write_bytes(tmp, out)
        retry(os.replace, tmp, reg)
    return dict(touched), out != raw


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('banks', nargs='*')
    ap.add_argument('--dry-run', action='store_true')
    ap.add_argument('--rerun-root', type=Path, default=REV / 'CppAuditRerun/runs')
    args = ap.parse_args()
    results = []
    for bank in (args.banks or BANKS):
        try:
            r = process(bank, args.rerun_root, args.dry_run)
        except AssertionError as ex:
            print('FALLO %s: %s' % (bank, ex), file=sys.stderr)
            sys.exit(1)
        results.append(r)
        print('%s %s pares=%d run %s -> %s%s' % ('OK(dry)' if args.dry_run else 'OK', bank, r['pairs'], r['old_run'],
                                                  r['run'], ' [ya registrado]' if r['already'] else ''), flush=True)
    touched, changed = update_registry(results, args.dry_run)
    print(json.dumps({'dry_run': args.dry_run, 'banks': len(results), 'registry_entries_per_bank': touched,
                      'registry_changed': changed}))


if __name__ == '__main__':
    main()
