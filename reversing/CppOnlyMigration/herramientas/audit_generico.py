"""Auditor genérico e independiente de una corrida de banco por pares (sin script propio en el banco).

Uso: python -B audit_generico.py <Banco> <carpeta de la corrida> [--archive]
Comprueba contra la tabla fns[] de reversing/<Banco>/LiveComparison.hpp: pares por función, sin diferencias, sin
registros nulos, original == candidato, ruta de hook (route_ok/switches si existen), estado, partidas y configuración
restaurados, EXE canónico y hash de la sonda ejecutada frente a artifacts/build-manifest.json. No certifica registros
crudos ni el cuerpo completo del EXE."""
import argparse, gzip, hashlib, json, re, shutil
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EXE = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'

def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest()

def main():
    ap = argparse.ArgumentParser(); ap.add_argument('bank'); ap.add_argument('run', type=Path); ap.add_argument('--archive', action='store_true')
    a = ap.parse_args(); bank = ROOT / 'reversing' / a.bank
    table = (bank / 'LiveComparison.hpp').read_text(encoding='utf-8')
    expected = {f'{c}::{n}': int(k) for c, n, k in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-Fa-f]+|0),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', table)}
    assert expected, 'tabla vacía'
    total = sum(expected.values())
    s = json.loads((a.run / 'summary.json').read_text(encoding='utf-8'))
    assert all(s[k] is True for k in ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored'))
    assert s['original_exe_sha256'] == EXE and s['pairs'] == total and s['pairs_per_function'] == expected
    raw = (a.run / 'comparisons.jsonl').read_bytes()
    assert hashlib.sha256(raw).hexdigest() == s['log_sha256']
    rows = [json.loads(l) for l in raw.decode('utf-8').splitlines()]
    pairs = [r for r in rows if r.get('event') == 'comparison']
    assert len(pairs) == total and Counter(r['function'] for r in pairs) == expected
    assert {r['pair'] for r in pairs} == set(range(total))
    assert not any(r.get('event') in ('error', 'assertion', 'first_chance_av', 'first_chance_av_raw') for r in rows)
    assert len([r for r in rows if r.get('event') == 'complete']) == 1
    for r in pairs:
        assert r['accepted'] is True and r['equal'] is True and r['original'] is not None and r['candidate'] is not None and r['original'] == r['candidate'], r['pair']
        for side in ('original', 'candidate'):
            for gate in ('route_ok', 'switches', 'abi_ok', 'guards_ok'):
                if gate in r[side]: assert r[side][gate] is True, (r['pair'], side, gate)
    for name, count in expected.items():
        assert {r['case'] for r in pairs if r['function'] == name} == set(range(count)), name
    manifest = json.loads((bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
    assert s['executed_probe_sha256'] == manifest['binaries']['asi'], 'sonda ejecutada distinta de la compilada'
    for path, digest in manifest['sources'].items(): assert sha(ROOT / path) == digest, path
    result = dict(bank=a.bank, run=a.run.name, functions=len(expected), pairs=total, pairs_per_function=expected, differences=0,
                  null_comparison_records=0, state_restored=True, saves_unchanged=True, configs_restored=True,
                  log_sha256=s['log_sha256'], tested_sources=manifest['sources'], original_exe_sha256=EXE,
                  executed_probe_sha256=s['executed_probe_sha256'], auditor='herramientas/audit_generico.py',
                  auditor_sha256=sha(__file__),
                  scope='Pares, igualdad, no nulos, rutas y compuertas booleanas presentes, restauración y hashes. Sin contrato de registros crudos ni prueba del cuerpo completo del EXE.')
    if a.archive:
        dest = bank / 'runs' / a.run.name; assert not dest.exists(); dest.mkdir(parents=True)
        for n in ('summary.json', 'RESULTADO.txt'): shutil.copy2(a.run / n, dest / n)
        with (dest / 'comparisons.jsonl.gz').open('wb') as f, gzip.GzipFile(filename='', mode='wb', fileobj=f, mtime=0) as z: z.write(raw)
        (dest / 'approval-audit.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k: v for k, v in result.items() if k not in ('tested_sources',)}, indent=2))

if __name__ == '__main__': main()
